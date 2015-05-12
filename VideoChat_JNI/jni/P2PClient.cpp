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
#define VIDEO_TIMESTAMP_INCREMENT	3000 // 30 fps

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
		javaMethod = jniEnv->GetStaticMethodID(javaClass, "onMsgReceive",
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
		jniEnv->CallStaticVoidMethod(javaClass, javaMethod, string1, string2);
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

// change the playing state
jint JNICALL Java_com_arcsoft_ais_arcvc_jni_P2PClient_pausePlaying
(JNIEnv * env, jobject clazz)
{
	bool isPlaying = false;
	if (audioSession) {
		audioSession->GetAudioRTPSession()->switchPlayingStatus();
		isPlaying = audioSession->GetAudioRTPSession()->isPlaying();
	}
	if (videoSession) {
		videoSession->GetVideoRTPSession()->switchPlayingStatus();
		isPlaying = videoSession->GetVideoRTPSession()->isPlaying();
	}
	return isPlaying;
}

//init
void JNICALL Java_com_arcsoft_ais_arcvc_jni_P2PClient_init
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

void JNICALL Java_com_arcsoft_ais_arcvc_jni_P2PClient_uninit
  (JNIEnv * env, jobject clazz)
{
	if (pIClient) {
		pIClient->UnInit();
		pIClient = NULL;
	}

	if (videoSession != NULL) {
		VideoRTPSession* videoSess = videoSession->GetVideoRTPSession();
		CORE_SAFEDELETE(videoSess);
		AudioRTPSession* audioSess = videoSession->GetAudioRTPSession();
		CORE_SAFEDELETE(audioSess);
		videoSession->Destroy();
	}

	if (audioSession != NULL) {
		if (NULL == videoSession) {
			VideoRTPSession* videoSess = audioSession->GetVideoRTPSession();
			CORE_SAFEDELETE(videoSess);
			AudioRTPSession* audioSess = audioSession->GetAudioRTPSession();
			CORE_SAFEDELETE(audioSess);
		}
		audioSession->Destroy();
	}

	CORE_SAFEDELETE(videoSession);
	CORE_SAFEDELETE(audioSession);
	env->DeleteGlobalRef(javaObj);

	memset(arrADTS, 0, sizeof(arrADTS));
	if (stAACFrameBuf.pBuf) {
		free(stAACFrameBuf.pBuf);
	}
}

void JNICALL Java_com_arcsoft_ais_arcvc_jni_P2PClient_sendMsg
(JNIEnv * env, jobject clazz, jstring jgpid, jstring jmsg)
{
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
		if (err == E_IF_OK) {
            LOGD("Sendto success!\n");
		} else {
            LOGE("Sendto invalid! error # %d.\n", err);
		}
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
	LOGI("srcGPID: %s\n",srcGPID);
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

		LOGD("startRTPSession gpid: %s", gpid);

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
		RTPSessionParams* sessparams = new RTPSessionParams;
		sessparams->SetOwnTimestampUnit(1.0/10.0);
		sessparams->SetAcceptOwnPackets(true);
		transparams->SetPortbase(VIDEO_PORT);
		sessparams->SetUsePollThread(true);
		transparams->SetP2PInterface(pIClient);

		videoSession = new AVBaseRTPSession(RTP_VIDEO_PAYLOAD_TYPE);
		status = videoSession->Create(*sessparams, transparams, RTPTransmitter::NATTVLProto);
		LOGI("videoSession send.Create status: %d", status);
		videoSession->SetVideoRTPSession(videoSess);
		videoSession->SetAudioRTPSession(audioSess);

		status = videoSession->AddDestination(addr2);
		LOGI("videoSession sess.AddDestination status: %d", status);
		videoSession->SetDefaultPayloadType(RTP_VIDEO_PAYLOAD_TYPE);//设置传输类型
		videoSession->SetDefaultMark(true);		//设置位
		videoSession->SetTimestampUnit(1.0/VIDEO_SAMPLING_RATE); //设置采样间隔
		videoSession->SetDefaultTimestampIncrement(VIDEO_TIMESTAMP_INCREMENT);//设置时间戳增加间隔  (时间戳增加间隔 =采样频率/帧率)
		LOGI("videoSession startRTPSession Done.");
#endif

#ifdef _HAVE_AUDIO_
		stAACFrameBuf.pBuf = (uint8_t*)malloc(MAX_AAC_FRAME_SIZE);
		stAACFrameBuf.buflen = stAACFrameBuf.framelength = 0;
		stAACFrameBuf.pBuf[0] = stAACFrameBuf.pBuf[1] = 0;

		//audioSession
		RTPNATTVLTransmissionParams* audioTransParams = new RTPNATTVLTransmissionParams;
		audioTransParams->SetPortbase(AUDIO_PORT);
		audioTransParams->SetP2PInterface(pIClient);

		RTPSessionParams* audioSessParams = new RTPSessionParams;
		audioSessParams->SetOwnTimestampUnit(1.0/10.0);
		audioSessParams->SetAcceptOwnPackets(true);
		audioSessParams->SetUsePollThread(true);

		audioSession = new AVBaseRTPSession(RTP_AUDIO_PAYLOAD_TYPE);
		status = audioSession->Create(*audioSessParams, audioTransParams, RTPTransmitter::NATTVLProto);
		LOGI("audioSession send.Create status: %d", status);
		audioSession->SetVideoRTPSession(videoSess);
		audioSession->SetAudioRTPSession(audioSess);

		status = audioSession->AddDestination(addr2);
		LOGI("audioSession sess.AddDestination status: %d", status);
		audioSession->SetDefaultPayloadType(RTP_AUDIO_PAYLOAD_TYPE);//设置传输类型
		audioSession->SetDefaultMark(true);		//设置位

		audioSession->SetTimestampUnit(1.0/AUDIO_SAMPLING_RATE); //设置采样间隔 44100
		audioSession->SetDefaultTimestampIncrement(AUDIO_TIMESTAMP_INCREMENT);//设置时间戳增加间隔 1024

		LOGI("audioSession startRTPSession Done.");

		audioSess->SetVideoRTPSession(videoSess);
		videoSess->SetAudioRTPSession(audioSess);
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

void SendH264Nalu(h264_nal_t* nalHdr, RTPSession& sess, const char* szPktType) {
	LOGI("SendH264Nalu !start");
	unsigned char *pSendbuf = nalHdr->p_payload; //发送数据指针
	int buflen = nalHdr->i_payload;

	char sendbuf[MAX_RTP_PKT_LENGTH] = {0};

	int pos = 0;
	memcpy(sendbuf, &nalHdr->i_ref_idc, sizeof(int));
	pos += sizeof(int);
	memcpy(sendbuf + pos, &nalHdr->i_type, sizeof(int));
	pos += sizeof(int);

	struct  timeval    stCurTime;
	gettimeofday( &stCurTime, NULL );
	double curtime = (double)stCurTime.tv_sec + (double)stCurTime.tv_usec / 1000000;

	if (!strcmp(szPktType, "img")) {
		++videoSequenceNum;
		LOGD("Send fps: %lf, currentTime: %lf, SequenceNumber: %u\n",(videoSequenceNum/curtime), curtime, videoSequenceNum);
//		FILE* flog = fopen("/mnt/sdcard/videoSend.log", "a");
//		if (flog) {
//			fprintf(flog, "SendH264Nalu currentTime: %lf, SequenceNumber: %u\n", curtime, videoSequenceNum);
//			fclose(flog);
//		}
	}

	int status;
//	LOGD("SendH264Nalu ! ");
	if (buflen <= MAX_RTP_PKT_LENGTH - pos - RTP_PKT_HEADER_LENGTH) { // 一包就能搞定
		sess.SetDefaultMark(true); // 因为是最后一包，所以true

		memcpy(sendbuf + pos, &curtime, sizeof(double));
		pos += sizeof(double);
		memcpy(sendbuf + pos, &videoSequenceNum, sizeof(uint32_t));
		pos += sizeof(uint32_t);

		memcpy(sendbuf + pos, pSendbuf, buflen);
		status = sess.SendPacket((void *) sendbuf, buflen + pos);
		LOGD("SendH264Nalu status: %d, buflen: %d --------File: %s, Line: %d", status, buflen, __FILE__, __LINE__);
	} else if (buflen > MAX_RTP_PKT_LENGTH - pos - RTP_PKT_HEADER_LENGTH) { // 分多包发送
		//设置标志位Mark为0
		sess.SetDefaultMark(false);

		//send first RTP pkg
		memcpy(sendbuf + pos, &curtime, sizeof(double));
		pos += sizeof(double);
		memcpy(sendbuf + pos, &videoSequenceNum, sizeof(uint32_t));
		pos += sizeof(uint32_t);

		memcpy(sendbuf + pos, pSendbuf, MAX_RTP_PKT_LENGTH - pos);
		status = sess.SendPacket((void *) sendbuf, MAX_RTP_PKT_LENGTH);
		LOGD("SendH264Nalu status: %d, buflen: %d --------File: %s, Line: %d", status, buflen, __FILE__, __LINE__);

		int restBufLen = buflen - (MAX_RTP_PKT_LENGTH - pos);
		pSendbuf += (MAX_RTP_PKT_LENGTH - pos);

		int k = 0, l = 0;
		k = restBufLen / (MAX_RTP_PKT_LENGTH - RTP_PKT_HEADER_LENGTH); // 要分几个完整包
		l = restBufLen % (MAX_RTP_PKT_LENGTH - RTP_PKT_HEADER_LENGTH); // 最后剩余的字节不足一个包

		for (int i = 0; i < k; i++) {
			if(i == (k-1) && l == 0) { // 因为是最后一包，所以true
				sess.SetDefaultMark(true);
			}
			pos = 0;
			memcpy(sendbuf + pos, &curtime, sizeof(double));
			pos += sizeof(double);
			memcpy(sendbuf + pos, &videoSequenceNum, sizeof(uint32_t));
			pos += sizeof(uint32_t);

			memcpy(sendbuf + pos, pSendbuf, MAX_RTP_PKT_LENGTH - RTP_PKT_HEADER_LENGTH);
			status = sess.SendPacket((void *) sendbuf, MAX_RTP_PKT_LENGTH);
			pSendbuf += (MAX_RTP_PKT_LENGTH - RTP_PKT_HEADER_LENGTH);
		}

		if (l > 0) {
			pos = 0;
			memcpy(sendbuf + pos, &curtime, sizeof(double));
			pos += sizeof(double);
			memcpy(sendbuf + pos, &videoSequenceNum, sizeof(uint32_t));
			pos += sizeof(uint32_t);

			sess.SetDefaultMark(true); // 因为是最后一包，所以true
			memcpy(sendbuf + pos, pSendbuf, l);
			status = sess.SendPacket((void *) sendbuf, l + pos);
		}
	} else {
		LOGI("I'm not ready for video, sorry!!!buflen: %d\n", buflen);
	}
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

void JNICALL Java_com_arcsoft_ais_arcvc_jni_P2PClient_send264Packet
  (JNIEnv * env, jobject clazz, jstring jstring, jobject nal)
{
	h264_nal_t h264nal;
	jbyteArray jpayload;
	GetJniObjectReferenceForH264(env, clazz, nal, h264nal, &jpayload);
	const char *szPktType = (const char *)env->GetStringUTFChars(jstring, 0);
	SendH264Nalu(&h264nal, *videoSession, szPktType);
	env->ReleaseStringUTFChars(jstring, szPktType);
	LOGD("destination gpid: %s, send264Packet, __LINE__: %d\n" , g_destGPID, __LINE__);
	env->ReleaseByteArrayElements(jpayload, (jbyte*)h264nal.p_payload, 0);
	env->DeleteLocalRef(jpayload);
}

static inline int getFrameLength(uint8_t* pAdts)
{
	return (int)(pAdts[3] & 0x02) << 11 | (int)(pAdts[3] & 0x01) << 11 | (int)(pAdts[4] & 0xe0) << 3 |
		   (int)(pAdts[4] & 0x1e) << 3  | (int)(pAdts[4] & 0x01) << 3  | (int)(pAdts[5] & 0xe0) >> 5;
}

static inline bool isADTSHeader(uint8_t* data)
{
	if (data[0] == arrADTS[0] && data[1] == arrADTS[1] && data[2] == arrADTS[2] && data[3] == arrADTS[3]) {
		return true;
	}
	return false;
}

static inline int getNextAACFrameOffset(uint8_t* data, int len)
{
	int ret = -1;
	int off = 0;

	while (len >= ADTS_HEADER_LENGTH) {
		if (isADTSHeader(data + off)) {
			ret = off;
			break;
		}
		++off;
		--len;
	}
	return ret;
}

void SendAACNalu(aac_nal_t* nalHdr, RTPSession& sess) {
	LOGI("SendAACNalu...start");

	uint8_t* pSendbuf = nalHdr->p_payload; //发送数据指针
	int buflen = 0;
	char sendbuf[MAX_RTP_PKT_LENGTH] = {0};
	int status;

	while (nalHdr->i_payload > 0) {
		LOGI("buflen: %d, __LINE__: %d\n", buflen, __LINE__);
		buflen = getNextAACFrameOffset(nalHdr->p_payload, nalHdr->i_payload);
		if (!buflen) {
			LOGI("buflen: %d, __LINE__: %d\n", buflen, __LINE__);
			int framelength = getFrameLength(nalHdr->p_payload);
			buflen = nalHdr->i_payload >= framelength ? framelength : nalHdr->i_payload;
		} else if (buflen < 0) {
			LOGI("buflen: %d, __LINE__: %d\n", buflen, __LINE__);
			buflen = nalHdr->i_payload;
		}

		pSendbuf = nalHdr->p_payload;
		nalHdr->p_payload += buflen;
		nalHdr->i_payload -= buflen;

		if (stAACFrameBuf.buflen + buflen + 2 > MAX_AAC_FRAME_SIZE) {
			LOGI("buflen: %d, __LINE__: %d\n", buflen, __LINE__);
			memcpy(stAACFrameBuf.pBuf, stAACFrameBuf.pBuf + stAACFrameBuf.buflen - sizeof(arrADTS), sizeof(arrADTS));
			stAACFrameBuf.buflen = sizeof(arrADTS);
		}

		/* put aac data into buffer */
		memcpy(stAACFrameBuf.pBuf + stAACFrameBuf.buflen, pSendbuf, buflen);
		stAACFrameBuf.buflen += buflen;
		stAACFrameBuf.pBuf[stAACFrameBuf.buflen] = stAACFrameBuf.pBuf[stAACFrameBuf.buflen + 1] = 0;

		/* get the first four bytes of adts header of the current aac stream */
		if (!arrADTS[0] && stAACFrameBuf.buflen > sizeof(arrADTS)) {
			memcpy(arrADTS, stAACFrameBuf.pBuf, sizeof(arrADTS));
		}

		while ((buflen = getNextAACFrameOffset(stAACFrameBuf.pBuf, stAACFrameBuf.buflen)) >= 0) {
			if (buflen > 0) {
				LOGI("buflen: %d, __LINE__: %d\n", buflen, __LINE__);
				stAACFrameBuf.buflen -= buflen;
				memmove(stAACFrameBuf.pBuf, stAACFrameBuf.pBuf + buflen, stAACFrameBuf.buflen);
			}

			LOGI("buflen: %d, __LINE__: %d\n", buflen, __LINE__);
			stAACFrameBuf.framelength = getFrameLength(stAACFrameBuf.pBuf);
			if (stAACFrameBuf.buflen >= stAACFrameBuf.framelength) {
				if ((stAACFrameBuf.buflen - stAACFrameBuf.framelength) < ADTS_HEADER_LENGTH || isADTSHeader(stAACFrameBuf.pBuf + stAACFrameBuf.framelength) ||
					(stAACFrameBuf.pBuf[stAACFrameBuf.framelength] == 0 && stAACFrameBuf.pBuf[stAACFrameBuf.framelength + 1] == 0)) {
					LOGI("buflen: %d, __LINE__: %d\n", buflen, __LINE__);
					pSendbuf = stAACFrameBuf.pBuf;
					buflen = stAACFrameBuf.framelength;
				} else {
					LOGI("buflen: %d, __LINE__: %d\n", buflen, __LINE__);
					buflen = getNextAACFrameOffset(stAACFrameBuf.pBuf + 1, stAACFrameBuf.buflen - 1);
					if (buflen > 0) {
						buflen += 1;
						stAACFrameBuf.buflen -= buflen;
					} else {
						buflen = stAACFrameBuf.buflen - sizeof(arrADTS);
						stAACFrameBuf.buflen = sizeof(arrADTS);
					}
					memmove(stAACFrameBuf.pBuf, stAACFrameBuf.pBuf + buflen, stAACFrameBuf.buflen);
					stAACFrameBuf.framelength = 0;
					stAACFrameBuf.pBuf[stAACFrameBuf.buflen] = stAACFrameBuf.pBuf[stAACFrameBuf.buflen + 1] = 0;
					continue;
				}
			} else {
				LOGI("buflen: %d, __LINE__: %d\n", buflen, __LINE__);
				break;
			}

			int pos = 0;
			struct  timeval    stCurTime;
			gettimeofday( &stCurTime, NULL );
			double curtime = (double)stCurTime.tv_sec + (double)stCurTime.tv_usec / 1000000;
			++audioSequenceNum;

//			FILE* flog = fopen("/mnt/sdcard/audioSend.log", "a");
//			if (flog) {
//				fprintf(flog, "SendAACNalu currentTime:%lf, SequenceNumber: %u\n", curtime, audioSequenceNum);
//				fclose(flog);
//			}

			if (buflen <= MAX_RTP_PKT_LENGTH - RTP_PKT_HEADER_LENGTH) { // 一包就能搞定
				sess.SetDefaultMark(true); // 因为是最后一包，所以true

				memcpy(sendbuf + pos, &curtime, sizeof(double));
				pos += sizeof(double);
				memcpy(sendbuf + pos, &audioSequenceNum, sizeof(uint32_t));
				pos += sizeof(uint32_t);

				memcpy(sendbuf + pos, pSendbuf, buflen);
				status = sess.SendPacket((void *) sendbuf, buflen + pos);
				LOGD("SendAACNalu status: %d, buflen: %d --------File: %s, Line: %d", status, buflen, __FILE__, __LINE__);
			} else if (buflen > MAX_RTP_PKT_LENGTH - RTP_PKT_HEADER_LENGTH) { // 分多包发送
				//设置标志位Mark为0
				sess.SetDefaultMark(false);

				//send first RTP pkg
				memcpy(sendbuf + pos, &curtime, sizeof(double));
				pos += sizeof(double);
				memcpy(sendbuf + pos, &audioSequenceNum, sizeof(uint32_t));
				pos += sizeof(uint32_t);

				memcpy(sendbuf + pos, pSendbuf, MAX_RTP_PKT_LENGTH - RTP_PKT_HEADER_LENGTH);
				status = sess.SendPacket((void *) sendbuf, MAX_RTP_PKT_LENGTH);
				LOGD("SendH264Nalu status: %d, buflen: %d --------File: %s, Line: %d", status, buflen, __FILE__, __LINE__);

				int restBufLen = buflen - (MAX_RTP_PKT_LENGTH - RTP_PKT_HEADER_LENGTH);
				pSendbuf += (MAX_RTP_PKT_LENGTH - RTP_PKT_HEADER_LENGTH);

				int k = 0, l = 0;
				k = restBufLen / (MAX_RTP_PKT_LENGTH - RTP_PKT_HEADER_LENGTH); // 要分几个完整包
				l = restBufLen % (MAX_RTP_PKT_LENGTH - RTP_PKT_HEADER_LENGTH); // 最后剩余的字节不足一个包

				for (int i = 0; i < k; i++) {
					if(i == (k-1) && l == 0) { // 因为是最后一包，所以true
						sess.SetDefaultMark(true);
					}
					pos = 0;
					memcpy(sendbuf + pos, &curtime, sizeof(double));
					pos += sizeof(double);
					memcpy(sendbuf + pos, &audioSequenceNum, sizeof(uint32_t));
					pos += sizeof(uint32_t);

					memcpy(sendbuf + pos, pSendbuf, MAX_RTP_PKT_LENGTH - RTP_PKT_HEADER_LENGTH);
					status = sess.SendPacket((void *) sendbuf, MAX_RTP_PKT_LENGTH);
					pSendbuf += (MAX_RTP_PKT_LENGTH - RTP_PKT_HEADER_LENGTH);
				}

				if (l > 0) {
					pos = 0;
					memcpy(sendbuf + pos, &curtime, sizeof(double));
					pos += sizeof(double);
					memcpy(sendbuf + pos, &audioSequenceNum, sizeof(uint32_t));
					pos += sizeof(uint32_t);

					sess.SetDefaultMark(true); // 因为是最后一包，所以true
					memcpy(sendbuf + pos, pSendbuf, l);
					status = sess.SendPacket((void *) sendbuf, l + pos);
				}
			} else {
				LOGI("I'm not ready for audio, sorry!!!buflen: %d\n", buflen);
			}

			buflen = 0;
			stAACFrameBuf.buflen -= stAACFrameBuf.framelength;
			memmove(stAACFrameBuf.pBuf, stAACFrameBuf.pBuf + stAACFrameBuf.framelength, stAACFrameBuf.buflen);
			stAACFrameBuf.framelength = 0;
		}
	}

	LOGI("SendAACNalu...end");
}

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
//	LOGI("i_payload: %d" , aacNal.i_payload);
//	LOGI("p_payload length: %d" , env->GetArrayLength((jbyteArray)jPayload));
	return 1;
}

void JNICALL Java_com_arcsoft_ais_arcvc_jni_P2PClient_sendAACPacket
  (JNIEnv * env, jobject clazz, jstring jstring, jobject nal)
{
	LOGD("sendAACPacket,start... __LINE__: %d\n" , __LINE__);
	aac_nal_t aacNal;
	jbyteArray jpayload;
	GetJniObjectReferenceForAAC(env, clazz, nal, aacNal, &jpayload);
	SendAACNalu(&aacNal, *audioSession);
	LOGD("destination gpid: %s, sendAACPacket, __LINE__: %d\n" , g_destGPID, __LINE__);
	env->ReleaseByteArrayElements(jpayload, (jbyte*)aacNal.p_payload, 0);
	env->DeleteLocalRef(jpayload);
	LOGD("sendAACPacket,end... __LINE__: %d\n" , __LINE__);
}

