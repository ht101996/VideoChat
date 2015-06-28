#include <stdio.h>
#include "P2PClient.h"
#include "inc/pos_addr.h"
#include "p2p_nat.h"
#include "inc/p2p_interface.h"
#include "inc/p2p_errno.h"
#include "pos_timer.h"
#include "RTPWrapper.h"

#define _HAVE_VIDEO_
#define _HAVE_AUDIO_

#define MAX_RTP_PKT_LENGTH 1004//(1024-12-8)
// #define MAX_RTP_PKT_LENGTH 101384//(1024-12-8)
#define MAX_AAC_FRAME_SIZE 8192*2 // 8192 == 2^13
#define ADTS_HEADER_LENGTH 7
#define AUDIO_TIMESTAMP_INCREMENT	1024 // 160
#define VIDEO_TIMESTAMP_INCREMENT	6000//3000 // 30 fps

enum {
	//MSG_TYPE_DEFAULT = 0,
	//MSG_TYPE_DEFAULT_ACK = 1,
	//MSG_TYPE_UFTP_BEGIN = 9,
	//MSG_TYPE_UFTP_ACTIVE_PUT,		  //the file receiver will recv this message from uftp module, by cm.
	/*
	* the file receiver can send this message directly to file sender,
	* to notify file trasfer failure if it is necessary, this is optional, currently not included in sample code.
	*/
	//MSG_TYPE_UPLOAD_FILE_NOTIFY,
	//MSG_TYPE_MAX = 253
	MSG_TYPE_APPLY_CONNECTION = 2,
	MSG_TYPE_CLOSE_CONNECTION = 3,
	MSG_TYPE_ACCEPT_CONNECTION = 4,
	MSG_TYPE_REJECT_CONNECTION = 5
};

typedef struct AACFrameBuf {
	uint8_t* pBuf;
	int buflen;
	int framelength;
} AACFrameBuf;
ICMClient* pIClient;
JavaVM *jvm;
jobject javaObj;
AVBaseRTPSession* videoSession;
AVBaseRTPSession* audioSession;
AACFrameBuf stAACFrameBuf;
uint8_t arrADTS[4] = {0}; // {0xff, 0xf1, 0xec, 0x40};
uint32_t videoSequenceNum = 0;
uint32_t audioSequenceNum = 0;

class CMsgcClientCallBack: public IMsgcClientCallBack {
public:
	virtual void HandleMsgccEvent(DWORD msgID,	//msg type
			DWORD wParam, // protocol type
			LPARAM pParam, // msg buffer
			void *user = NULL);

};

void CMsgcClientCallBack::HandleMsgccEvent(DWORD msgID,	//msg type
		DWORD wParam, // protocol type
		LPARAM pParam, // msg buffer
		void *user) {
}

class CCmClientCallBack: public ICMClientCallBack {
public:
	virtual void HandleCmcEvent(DWORD msgType, DWORD wParam, LPARAM pParam,
			void *srcAddr, void *user = NULL);
};

static void notifyJNI(char* gpid, char* msg) {
	JNIEnv* jniEnv = NULL;
	jclass javaClass = NULL;
	jmethodID javaMethod = NULL;
	jstring string1 = NULL;
	jstring string2 = NULL;

	do {
		if (!jvm) {
			break;
		}
		if (jvm->AttachCurrentThread(&jniEnv, NULL) != JNI_OK) {
			break;
		}
		if (javaObj == NULL) {
			LOGE("javaObj is NULL.\n");
			break;
		}
		javaClass = jniEnv->GetObjectClass(javaObj);
		if (javaClass == NULL) {
			break;
		}
		javaMethod = jniEnv->GetMethodID(javaClass, "receiveStringMsg",
				"(Ljava/lang/String;Ljava/lang/String;)V");
		if (javaMethod == NULL) {
			break;
		}
		string1 = jniEnv->NewStringUTF(gpid);
		if (string1 == NULL) {
			break;
		}
		string2 = jniEnv->NewStringUTF(msg);
		if (string2 == NULL) {
			break;
		}
		jniEnv->CallVoidMethod(javaObj, javaMethod, string1, string2);
	} while (false);

	if (jniEnv) {
		if (javaClass) {
			jniEnv->DeleteLocalRef(javaClass);
		}
		if (string1) {
			jniEnv->DeleteLocalRef(string1);
		}
		if (string2) {
			jniEnv->DeleteLocalRef(string2);
		}
	}
	if (jvm) {
		jvm->DetachCurrentThread();
	}
}

/* Handle msg from network*/
void CCmClientCallBack::HandleCmcEvent(DWORD msgType, DWORD wParam, LPARAM pParam,
		void *srcAddr, void *user) {
	//__android_log_print(ANDROID_LOG_INFO, "P2PClient HandleCmcEvent", "msgType: %s\n",msgType);
	//__android_log_print(ANDROID_LOG_INFO, "P2PClient HandleCmcEvent", "wParam: %s\n",wParam);
	LOGD("receive a msg.\n");
	Sint8 srcGPID[41] = { '\0' };
	Sint8 srcIP[20] = { '\0' };
	strcpy(srcIP, inet_ntoa(((Tpos_addr*) srcAddr)->addr.sockaddr.sin_addr));
	for (int i = 0; i != 20; ++i) {
		sprintf(srcGPID + 2 * i, "%02x",(Uint8) (((Tpos_addr*) srcAddr)->addr.uuid[i]));
	}
	//__android_log_print(ANDROID_LOG_INFO, " HandleCmcEvent", "receive a msg.2");
	__android_log_print(ANDROID_LOG_INFO, "P2PClient", "srcGPID: %s\n",srcGPID);
	//__android_log_print(ANDROID_LOG_INFO, "P2PClient", "srcIP: %s\n",srcIP);
	switch (msgType) {
	case MSG_TYPE_DEFAULT: //default ct value == 0
	{

		//__android_log_print(ANDROID_LOG_INFO, " HandleCmcEvent ", "receive a msg.=");
		Sint32 msglen = (Sint32) wParam;
		unsigned char *msg = (unsigned char *) pParam;
		__android_log_print(ANDROID_LOG_INFO, "HandleCmcEvent ", "msg: %s\n",msg);
		Uint32 timestamp = 0;
		memcpy(&timestamp, msg, 4);
//		if (((Tpos_addr*) srcAddr)->type == POS_ADDR_SOCK) {
//		} else {
//		}
		struct tm* sendtime = localtime((time_t*) &timestamp);
		if (((Tpos_addr*) srcAddr)->type == POS_ADDR_SOCK) {
			pIClient->SendToip((char*) msg, 4,
					((Tpos_addr*) srcAddr)->addr.sockaddr.sin_addr.s_addr,
					((Tpos_addr*) srcAddr)->addr.sockaddr.sin_port, 2);
		} else {
			pIClient->SendTo((char*) msg, 4,
					(unsigned char*) ((Tpos_addr*) srcAddr)->addr.uuid, 2);
		}
		notifyJNI(srcGPID, (char*) msg + 4);
	}
		break;

	case 2: {
//        unsigned char *msg = (unsigned char *) pParam;
//        Uint32 systime = pos_sys_time();
//        Uint32 timebegin = 0;
//        memcpy(&timebegin, msg, 4);
//        qDebug("msg ack recved time begin %s , time end %s, time diff %u \n", asctime(localtime((time_t*)&timebegin)), asctime(localtime((time_t*)&systime)), systime-timebegin);
	}
		break;

	case MSG_TYPE_UFTP_ACTIVE_PUT: {
//        Sint8 localFileName[UFTP_FILE_NAME_LEN] = {'\0'};
//        UFTP_APP_Notification_Type errorCode;
//        Uint32 appSID;
//        Uint8 GPID[20];
//        //Tsession_item appRecord;
//        Tuftp_UftpPutReq *pPutReq = (Tuftp_UftpPutReq*)pParam;
//        if (m_pModHandler->getIClientInstance()->ExtractFileInfo(pPutReq, localFileName, errorCode, appSID, GPID)){
//            m_pModHandler->getIClientInstance()->SendPutAck(true, EUFTP_SESSION_NORMAL, localFileName, pPutReq->uftp_sid);
//            //appRecord.appSid = appSID;
//            //appRecord.role_type = pPutReq->role_type;
//            qDebug("cmclienttest:: on UFTP_ACTIVE_PUT - UftpSID:%d, RoleType:%d.\n",
//                   pPutReq->uftp_sid, pPutReq->role_type);
//
//            //((Debuger*)user)->addTransferSession(pPutReq->uftp_sid,appRecord);
//        }
//        else{
//            m_pModHandler->getIClientInstance()->SendPutAck(false, EUFTP_FILE_PAHT_DENY, localFileName, pPutReq->uftp_sid);
//            qDebug("cmclienttest:: on UFTP_ACTIVE_PUT - UftpSID:%d, RoleType:%d. save path err!\n",
//                   pPutReq->uftp_sid, pPutReq->role_type);
//        }
	}
		break;
	default:
		break;
	}
}

void StrToGpid(BYTE *gpid, const Sint8 *buf) {
	int i;
	Sint8 tmp[3];
	tmp[2] = 0;
	for (i = 0; i < 20; i++) {
		memcpy((void*) tmp, (void*) &buf[2 * i], 2);
		gpid[i] = strtoul(tmp, 0, 16);
	}
}


static void SendH264Data(unsigned char *pSendbuf, int length, const char* szPktType, jlong timestamp) {
	LOGI("sendH264Data   start !! szPktType=%s data length : %d, timestamp :%lld",
			szPktType, length,  timestamp);
	char sendbuf[MAX_RTP_PKT_LENGTH] = {0};

	int pos = 0, packetSize = 0, srcPos = 0, desPos = 0;


	double curtime = (double)timestamp / 1000000;
	bool flag = false;
	if (!strcmp(szPktType, "img")) {
		++videoSequenceNum;
	}

	do {
		if((length - srcPos) <= MAX_RTP_PKT_LENGTH - RTP_PKT_HEADER_LENGTH) {
			packetSize = length - srcPos + RTP_PKT_HEADER_LENGTH;
			videoSession->SetDefaultMark(true);
			flag = true;
		}
		else {
			packetSize = MAX_RTP_PKT_LENGTH;
			videoSession->SetDefaultMark(false);
			flag = false;
		}
		memcpy(sendbuf + desPos, &curtime, sizeof(double));
		desPos += sizeof(double);

		//add sequence number
		memcpy(sendbuf + desPos, &videoSequenceNum, sizeof(uint32_t));
		desPos += sizeof(uint32_t);

		//set H264 data
		memcpy(sendbuf + desPos, pSendbuf + srcPos, packetSize - RTP_PKT_HEADER_LENGTH);
		int status =  videoSession->SendPacket((void *) sendbuf, packetSize);
		srcPos += packetSize - RTP_PKT_HEADER_LENGTH;
//		LOGD("sendH264Data status: %d, buflen: %d, Mark = %d, Line: %d", status, packetSize, flag, __LINE__);

		memset(sendbuf, 0, sizeof(char) * MAX_RTP_PKT_LENGTH);
		desPos = 0;


	}while(srcPos < length);
}

int GetJniObjectReferenceForH264(JNIEnv *env, jobject jo, jobject odata, h264_nal_t& h264nal, jbyteArray* outPayload) {
	/* Get a reference to obj's class */
	jfieldID fid;
	jclass cls = env->GetObjectClass(odata);
	/* Look for the instance field s in cls */
	jobject jPayload;
	fid = (env)->GetFieldID(cls, "payload", "[B");
	if (fid == NULL) {
		return 0; /* failed to find the field */
	} else {
		jPayload = env->GetObjectField(odata, fid);
		h264nal.p_payload = (uint8_t *) env->GetByteArrayElements((jbyteArray)jPayload, 0);
		*outPayload = (jbyteArray)jPayload;
	}
	fid = (env)->GetFieldID(cls, "refIdc", "I");
	if (fid == NULL) {
		return 0; /* failed to find the field */
	} else {
		jint jRefIdc = env->GetIntField(odata, fid);
		h264nal.i_ref_idc = jRefIdc;
	}
	fid = (env)->GetFieldID(cls, "type", "I");
	if (fid == NULL) {
		return 0; /* failed to find the field */
	} else {
		jint jType = env->GetIntField(odata, fid);
		h264nal.i_type = jType;
	}
	fid = (env)->GetFieldID(cls, "payloadLength", "I");
	if (fid == NULL) {
		return 0; /* failed to find the field */
	} else {
		jint jPayLoadLength = env->GetIntField(odata, fid);
		h264nal.i_payload = jPayLoadLength;
	}
	env->DeleteLocalRef(cls);
//	LOGI("i_payload: %d" , h264nal.i_payload);
//	LOGI("i_type: %d" , h264nal.i_type);
//	LOGI("i_ref_idc: %d" , h264nal.i_ref_idc);
//	LOGI("p_payload length: %d" , env->GetArrayLength((jbyteArray)jPayload));
	return 1;
}


//static inline int getFrameLength(uint8_t* pAdts)
//{
//	return (int)(pAdts[3] & 0x02) << 11 | (int)(pAdts[3] & 0x01) << 11 | (int)(pAdts[4] & 0xe0) << 3 |
//		   (int)(pAdts[4] & 0x1e) << 3  | (int)(pAdts[4] & 0x01) << 3  | (int)(pAdts[5] & 0xe0) >> 5;
//}
//
//static inline bool isADTSHeader(uint8_t* data)
//{
//	if (data[0] == arrADTS[0] && data[1] == arrADTS[1] && data[2] == arrADTS[2] && data[3] == arrADTS[3]) {
//		return true;
//	}
//	return false;
//}


int GetJniObjectReferenceForAAC(JNIEnv *env, jobject jo, jobject odata, aac_nal_t& aacNal, jbyteArray* outPayload) {
	/* Get a reference to obj's class */
	jfieldID fid;
	jclass cls = env->GetObjectClass(odata);
	/* Look for the instance field s in cls */
	jobject jPayload;
	fid = (env)->GetFieldID(cls, "payload", "[B");
	if (fid == NULL) {
		return 0; /* failed to find the field */
	} else {
		jPayload = env->GetObjectField(odata, fid);
		aacNal.p_payload = (uint8_t *) env->GetByteArrayElements((jbyteArray)jPayload, 0);
		*outPayload = (jbyteArray)jPayload;
	}
	fid = (env)->GetFieldID(cls, "payloadLength", "I");
	if (fid == NULL) {
		return 0; /* failed to find the field */
	} else {
		jint jPayLoadLength = env->GetIntField(odata, fid);
		aacNal.i_payload = jPayLoadLength;
	}
	env->DeleteLocalRef(cls);
	return 1;
}


//init
void JNICALL Java_com_arcsoft_ais_arcvc_jni_P2PClient_nativeInit
(JNIEnv * env, jobject clazz, jstring jiniDirPath)
{
	HRESULT hResult = GetCmcObject(&pIClient);
	if (hResult != E_IF_OK) {
		__android_log_write(ANDROID_LOG_ERROR, "JNI init error", "GetCmcObject error");
		return;
	}

	hResult = E_IF_OK;
	CMsgcClientCallBack *msgc_callback = new CMsgcClientCallBack();
	hResult = pIClient->InitMsgcClient(msgc_callback, NULL);
	if(hResult != E_IF_OK) {
		__android_log_write(ANDROID_LOG_ERROR, "JNI init error", "InitMsgcClient error");
		return;
	}

	char *p_cmd_path = (char *)env->GetStringUTFChars(jiniDirPath, (jboolean*)0);
	CCmClientCallBack *cmc_callback = new CCmClientCallBack();
	hResult = pIClient->Init(p_cmd_path, cmc_callback, NULL);
	if(hResult != E_IF_OK) {
		__android_log_write(ANDROID_LOG_ERROR, "JNI init error", "pIClient Init error");
		return;
	}
	env->GetJavaVM(&jvm);
	javaObj = env->NewGlobalRef(clazz);
}

void JNICALL Java_com_arcsoft_ais_arcvc_jni_P2PClient_nativeUninit
  (JNIEnv * env, jobject clazz)
{
	LOGD("Java_com_arcsoft_ais_arcvc_jni_P2PClient_uninit Start!!!!");

	LOGD("Java_com_arcsoft_ais_arcvc_jni_P2PClient_uninit mark 1 ");
	if (videoSession != NULL) {
		VideoRTPSession* videoSess = videoSession->GetVideoRTPSession();
		videoSess->Destroy();
		CORE_SAFEDELETE(videoSess);
//		AudioRTPSession* audioSess = videoSession->GetAudioRTPSession();
//		CORE_SAFEDELETE(audioSess);
		videoSession->Destroy();
	}
	LOGD("Java_com_arcsoft_ais_arcvc_jni_P2PClient_uninit mark 2 ");
	if (audioSession != NULL) {
		if (NULL == videoSession) {
//			VideoRTPSession* videoSess = audioSession->GetVideoRTPSession();
//			CORE_SAFEDELETE(videoSess);
			AudioRTPSession* audioSess = audioSession->GetAudioRTPSession();
			audioSess->Destroy();
			CORE_SAFEDELETE(audioSess);
		}
		audioSession->Destroy();
	}
	LOGD("Java_com_arcsoft_ais_arcvc_jni_P2PClient_uninit mark 3 ");
	CORE_SAFEDELETE(videoSession);
	CORE_SAFEDELETE(audioSession);
	if (pIClient) {
		pIClient->UnInit();
		pIClient = NULL;
	}
	env->DeleteGlobalRef(javaObj);
	LOGD("Java_com_arcsoft_ais_arcvc_jni_P2PClient_uninit mark 4 ");
	memset(arrADTS, 0, sizeof(arrADTS));
	if (stAACFrameBuf.pBuf) {
		free(stAACFrameBuf.pBuf);
	}
	LOGD("Java_com_arcsoft_ais_arcvc_jni_P2PClient_uninit end~~");
}

void JNICALL Java_com_arcsoft_ais_arcvc_jni_P2PClient_sendMsg
(JNIEnv * env, jobject clazz, jstring jgpid, jstring jmsg)
{
	if(pIClient == NULL)
		return;
	char *gpid = (char *)env->GetStringUTFChars(jgpid, 0);
	char *msg = (char *)env->GetStringUTFChars(jmsg, 0);
	HRESULT err = E_IF_OK;
	Sint8 *pMsgBuf = NULL;
	unsigned char pGPID[POS_UUID_LEN];
	Sint8 msglen = strlen(msg);
	StrToGpid(pGPID, gpid);

	pMsgBuf = new Sint8[msglen + sizeof(Uint32) + 1];
	Uint32 systime = pos_sys_time();
	if(pMsgBuf)
	{
		memcpy(pMsgBuf, &systime, sizeof(Uint32));
		strcpy(pMsgBuf + sizeof(Uint32), msg);
		struct tm* sendtime = localtime((time_t*)&systime);
		err = pIClient->SendTo(pMsgBuf, msglen + sizeof(Uint32), pGPID, 0);
//		if (err == E_IF_OK) {
//            LOGD("Sendto success!\n");
//		} else {
//            LOGE("Sendto invalid! error # %d.\n", err);
//		}
	}

	env->ReleaseStringUTFChars(jgpid, gpid);
	env->ReleaseStringUTFChars(jmsg, msg);
	CORE_SAFEDELETEARRAY(pMsgBuf);
}

//getLocalGpid
jstring JNICALL Java_com_arcsoft_ais_arcvc_jni_P2PClient_getLocalGpid(JNIEnv * env,
	jobject clazz)
{
	BYTE GPID[20];
	Sint8 srcGPID[41] = { '\0' };
	if (pIClient) {
		pIClient->GetID(GPID);
		for (int i = 0; i != 20; ++i) {
			sprintf(srcGPID + 2 * i, "%02x", (Uint8) GPID[i]);
		}
	}
	jstring string1 = env->NewStringUTF(srcGPID);
	return string1;
}

char g_destGPID[100];//FOR DEBUG LOG

void JNICALL Java_com_arcsoft_ais_arcvc_jni_P2PClient_startRTPSession
  (JNIEnv * env, jobject clazz, jstring gpidOfRemotePeer)
{
	if(pIClient)
	 {
		LOGI("startRTPSession");

		char *gpid = (char *)env->GetStringUTFChars(gpidOfRemotePeer, 0);
		strcpy(g_destGPID, gpid);

		LOGD("startRTPSession Destination gpid: %s", gpid);

		videoSequenceNum = audioSequenceNum = 0;
		int status = 10;
		Sint32 init_ret;
		CRTPCallBack * pRtpCb = new CRTPCallBack();
		///////////////////////////////////////////////////////////

		VideoRTPSession* videoSess = new VideoRTPSession;
		AudioRTPSession* audioSess = new AudioRTPSession;

		//const char* destGPIDStr = "9999999999999999999999999999999999999999";
		unsigned char destGPID[20];
		StrToGpid(destGPID, gpid);
		env->ReleaseStringUTFChars(gpidOfRemotePeer, gpid);
		RTPNATTVLAddress addr2((Sint8*)destGPID);

#ifdef _HAVE_VIDEO_
		//videoSession
		RTPNATTVLTransmissionParams* transparams = new RTPNATTVLTransmissionParams;
		RTPSessionParams* sessparams = new RTPSessionParams();
		sessparams->SetOwnTimestampUnit(1.0/10.0);
		sessparams->SetAcceptOwnPackets(true);
		transparams->SetPortbase(VIDEO_PORT);
//		sessparams->SetUsePollThread(true);
		transparams->SetP2PInterface(pIClient);

		videoSession = new AVBaseRTPSession(RTP_VIDEO_PAYLOAD_TYPE);
		status = videoSession->Create(*sessparams, transparams, RTPTransmitter::NATTVLProto);
		LOGI("startRTPSession videoSession send.Create status: %d, LocalSSRC:%d", status, videoSession->GetLocalSSRC());
		videoSession->SetVideoRTPSession(videoSess);
//		videoSession->SetAudioRTPSession(audioSess);

		status = videoSession->AddDestination(addr2);
//		LOGI("videoSession sess.AddDestination status: %d", status);
		videoSession->SetDefaultPayloadType(RTP_VIDEO_PAYLOAD_TYPE);//设置传输类型
		videoSession->SetDefaultMark(true);		//设置位
		videoSession->SetTimestampUnit(1.0/VIDEO_SAMPLING_RATE); //设置采样间隔
		videoSession->SetDefaultTimestampIncrement(VIDEO_TIMESTAMP_INCREMENT);//设置时间戳增加间隔  (时间戳增加间隔 =采样频率/帧率)
//		LOGI("videoSession startRTPSession Done.");
#endif

#ifdef _HAVE_AUDIO_
		stAACFrameBuf.pBuf = (uint8_t*)malloc(MAX_AAC_FRAME_SIZE);
		stAACFrameBuf.buflen = stAACFrameBuf.framelength = 0;
		stAACFrameBuf.pBuf[0] = stAACFrameBuf.pBuf[1] = 0;

		//audioSession
		RTPNATTVLTransmissionParams* audioTransParams = new RTPNATTVLTransmissionParams;
		audioTransParams->SetPortbase(AUDIO_PORT);
		audioTransParams->SetP2PInterface(pIClient);

		RTPSessionParams* audioSessParams = new RTPSessionParams();
		audioSessParams->SetOwnTimestampUnit(1.0/10.0);
		audioSessParams->SetAcceptOwnPackets(true);
//		audioSessParams->SetUsePollThread(true);

		audioSession = new AVBaseRTPSession(RTP_AUDIO_PAYLOAD_TYPE);
		status = audioSession->Create(*audioSessParams, audioTransParams, RTPTransmitter::NATTVLProto);
		LOGI("startRTPSession audioSession send.Create status: %d, localSSRC:%d", status, audioSession->GetLocalSSRC());
//		audioSession->SetVideoRTPSession(videoSess);
		audioSession->SetAudioRTPSession(audioSess);

		status = audioSession->AddDestination(addr2);
//		LOGI("audioSession sess.AddDestination status: %d", status);
		audioSession->SetDefaultPayloadType(RTP_AUDIO_PAYLOAD_TYPE);//设置传输类型
		audioSession->SetDefaultMark(true);		//设置位

		audioSession->SetTimestampUnit(1.0/AUDIO_SAMPLING_RATE); //设置采样间隔 44100
		audioSession->SetDefaultTimestampIncrement(AUDIO_TIMESTAMP_INCREMENT);//设置时间戳增加间隔 1024

//		LOGI("audioSession startRTPSession Done.");

//		audioSess->SetVideoRTPSession(videoSess);
//		videoSess->SetAudioRTPSession(audioSess);
#endif

		RTPTransmitter* videoTransmitter = NULL;
		RTPTransmitter* audioTransmitter = NULL;
		if (videoSession) {
			videoTransmitter = videoSession->getTransmitter();
		}
		if (audioSession) {
			audioTransmitter = audioSession->getTransmitter();
		}
		init_ret = pIClient->RTPInit(pRtpCb, videoTransmitter, audioTransmitter);
		LOGI("audio&video Session send init_ret: %d", init_ret);
	 } else {
		 LOGE("pIClient == NULL");
	 }
}

void JNICALL Java_com_arcsoft_ais_arcvc_jni_P2PClient_send264Packet
  (JNIEnv * env, jobject clazz, jstring jstring, jobject nal, jlong timestamp)
{
	h264_nal_t h264nal;
	jbyteArray jpayload;
	GetJniObjectReferenceForH264(env, clazz, nal, h264nal, &jpayload);
	const char *szPktType = (const char *)env->GetStringUTFChars(jstring, 0);
	SendH264Data(h264nal.p_payload, h264nal.i_payload, szPktType, timestamp);
	env->ReleaseStringUTFChars(jstring, szPktType);
	env->ReleaseByteArrayElements(jpayload, (jbyte*)h264nal.p_payload, 0);
	env->DeleteLocalRef(jpayload);
}


void JNICALL Java_com_arcsoft_ais_arcvc_jni_P2PClient_sendAACESData
  (JNIEnv *env, jobject clazz, jbyteArray data, jint length, jlong timestamp)
{
	LOGD("sendAACESData destination gpid: %s, timestamp: %lld\n" , g_destGPID, timestamp);
	if(data == NULL || length == 0)
		return;
	char sendbuf[MAX_RTP_PKT_LENGTH] = {0};
	int srcPos = 0, desPos = 0;
	int packetSize = 0;
	jboolean isCopy;
	jbyte* pData = env->GetByteArrayElements(data, &isCopy);
	bool flag = false;
	do{
		if((length - srcPos) <= MAX_RTP_PKT_LENGTH - RTP_PKT_HEADER_LENGTH) {
			packetSize = length - srcPos + RTP_PKT_HEADER_LENGTH;
			audioSession->SetDefaultMark(true);
			flag = true;
		}
		else {
			packetSize = MAX_RTP_PKT_LENGTH;
			audioSession->SetDefaultMark(false);
			flag = false;
		}
		//add time stamp
		double curtime = (double) timestamp / 1000000;
		memcpy(sendbuf + desPos, &curtime, sizeof(double));
		desPos += sizeof(double);

		//add sequence number
		++ audioSequenceNum;
		memcpy(sendbuf + desPos, &audioSequenceNum, sizeof(uint32_t));
		desPos += sizeof(uint32_t);

		//set audio AAC ES data
		memcpy(sendbuf + desPos, pData + srcPos, packetSize - RTP_PKT_HEADER_LENGTH);
		int status = audioSession->SendPacket((void *) sendbuf, packetSize);
		srcPos += packetSize - RTP_PKT_HEADER_LENGTH;
//		LOGD("sendAACESData status: %d, buflen: %d, Mark = %d, Line: %d", status, packetSize, flag, __LINE__);

		memset(sendbuf, 0, sizeof(char) * MAX_RTP_PKT_LENGTH);
		desPos = 0;
	}while(srcPos < length);

	env->ReleaseByteArrayElements(data, pData, 0);
}
