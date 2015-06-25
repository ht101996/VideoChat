#include <fstream>
#include <time.h>
#include <unistd.h>
#include <jni.h>
#include "inc/p2p_interface.h"
#include "RTPWrapper.h"


//#define AUDIO_TIMESTAMP_INCREMENT ((double)(1024.0 / 8000.0))
//#define VIDEO_TIMESTAMP_INCREMENT ((double)(1.0 / 15.0)) // 29.745
//#define IMG_DISPLAY_INTERVAL (VIDEO_TIMESTAMP_INCREMENT * 1000000) // 25 ms 25000 30000
//#define AUDIO_PLAY_INTERVAL	 (AUDIO_TIMESTAMP_INCREMENT * 1000000) // 90 ms 90000 130000
#define NALU_BUF_SIZE (1024*1024*2) // 2M
//#define DST_SAMPLE_RATE 8000
//#define DST_CHANNEL_NUM 1
//#define DST_SAMPLE_FMT  AV_SAMPLE_FMT_S16
//#define PIXEL_FORMAT AV_PIX_FMT_BGRA
//#define VIDEO_PACK_BUF_LIMIT	90
//#define AUDIO_PACK_BUF_LIMIT	16 // 25
//#define AUDIO_PLAY_PACKET_COUNT	3 //10
//#define AUDIO_PLAY_TOLERATE_TIME	0.15 //s 感觉偏移存在,同步区域  0.12
//#define AUDIO_PLAY_TOLERATE2_TIME	160 //可以感觉到,临界同步区域
//#define AUDIO_START_WAIT_TIME	1500000 // wait 1 second before audio start
//#define VIDEO_START_WAIT_TIME	1500000 // wait 1 second before video start
//#define AV_RESET_INTERVAL	10 // reset firstTimestamp and firstSequenceNum every 15 seconds
//#define __DECODE__ 1
//#define FLIP 1

extern JavaVM *jvm;
extern jobject javaObj;

static void notifyJNIH264DataReceived(const jbyte* data, int length, double timestamp) {
	JNIEnv* jniEnv = NULL;
	jclass clientClass = NULL;
	jmethodID javaMethod = NULL;
	jbyteArray resByteArr = NULL;
	do {
		if (!jvm || !data || length == 0) {
			LOGE("notifyJNIH264DataReceived parameter  is NULL\n");
			break;
		}
		if (jvm->AttachCurrentThread(&jniEnv, NULL) != JNI_OK) {
			LOGE("AttachCurrentThread failed.\n");
			break;
		}
		if (javaObj == NULL) {
			LOGE("javaObj is NULL.\n");
			break;
		}
		clientClass = jniEnv->GetObjectClass(javaObj);
		if (clientClass == NULL) {
			LOGE("Get Class P2PClient failed.\n");
			break;
		}
		javaMethod = jniEnv->GetMethodID(clientClass, "receiveH264Data", "([BIID)V");
		if (javaMethod == NULL) {
			LOGE("Get method receiveH264Data .\n");
			break;
		}
		resByteArr = jniEnv->NewByteArray(length);
		if (resByteArr == NULL) {
			LOGE("NewByteArray Out of memory, LINE: %d\n", __LINE__);
			break;
		}
		LOGD("notifyJNIH264DataReceived resByteArr: %p, pixArrLen: %d, len: %d", resByteArr, jniEnv->GetArrayLength(resByteArr), length);
		jniEnv->SetByteArrayRegion(resByteArr, 0, length, data);
		if (jniEnv->ExceptionCheck()) {
			LOGE("SetIntArrayRegion Exception.\n");
			jniEnv->ExceptionDescribe();
		} else {
			jniEnv->CallVoidMethod(javaObj, javaMethod, resByteArr, 0, length, timestamp);
		}
	} while (false);
	if (jniEnv) {
		if (clientClass) {
			jniEnv->DeleteLocalRef(clientClass);
		}
		if (resByteArr) {
			jniEnv->DeleteLocalRef(resByteArr);
		}
	}
	if (jvm) {
		jvm->DetachCurrentThread();
	}
}
static void notifyJNIAACDataReceived(const jbyte* data, int length, double timestamp) {
	JNIEnv* jniEnv = NULL;
	jclass clientClass = NULL;
	jmethodID javaMethod = NULL;
	jbyteArray resByteArr = NULL;
	do {
		if (!jvm || !data) {
			LOGE("notifyJNIAACDataReceived parameter  is NULL\n");
			break;
		}
		if (jvm->AttachCurrentThread(&jniEnv, NULL) != JNI_OK) {
			LOGE("AttachCurrentThread failed.\n");
			break;
		}
		if (javaObj == NULL) {
			LOGE("javaObj is NULL.\n");
			break;
		}
		clientClass = jniEnv->GetObjectClass(javaObj);
		if (clientClass == NULL) {
			LOGE("Get Class P2PClient failed.\n");
			break;
		}
		javaMethod = jniEnv->GetMethodID(clientClass, "receiveAACData", "([BIID)V");
		if (javaMethod == NULL) {
			LOGE("Get method receiveH264Data .\n");
			break;
		}
		resByteArr = jniEnv->NewByteArray(length);
		if (resByteArr == NULL) {
			LOGE("NewByteArray Out of memory, LINE: %d\n", __LINE__);
			break;
		}
		LOGD("notifyJNIAACDataReceived resByteArr: %p, pixArrLen: %d, len: %d", resByteArr, jniEnv->GetArrayLength(resByteArr), length);
		jniEnv->SetByteArrayRegion(resByteArr, 0, length, data);
		if (jniEnv->ExceptionCheck()) {
			LOGE("SetIntArrayRegion Exception.\n");
			jniEnv->ExceptionDescribe();
		} else {
			jniEnv->CallVoidMethod(javaObj, javaMethod, resByteArr, 0, length, timestamp);
		}
	} while (false);
	if (jniEnv) {
		if (clientClass) {
			jniEnv->DeleteLocalRef(clientClass);
		}
		if (resByteArr) {
			jniEnv->DeleteLocalRef(resByteArr);
		}
	}
	if (jvm) {
		jvm->DetachCurrentThread();
	}
}


VideoRTPSession::VideoRTPSession():
	  m_width(0)
	, m_height(0)
	, pNaluBuf(NULL)
	, bufLen(0)
	, timestamp(0)
	, firstSequenceNum(0)
	, extraDeltaTime(0)
	, sequenceNum(0)
	, eFrom(DEV_UNDEFINED)
{
	pNaluBuf = (uint8_t*)malloc(NALU_BUF_SIZE);
}

VideoRTPSession::~VideoRTPSession()
{
	if (pNaluBuf) {
		free(pNaluBuf);
	}
}


void VideoRTPSession::ProcessRTPPacket(const RTPSourceData &srcdat,const RTPPacket &rtppack)
{
	LOGD("VideoRTPSession ProcessRTPPacket !");
	bool mark = rtppack.HasMarker();
	if (mark) {// mark的pack是完整包的最后一个pack

		size_t len = rtppack.GetPayloadLength() - RTP_PKT_HEADER_LENGTH;
		memcpy(pNaluBuf + bufLen,rtppack.GetPayloadData() + RTP_PKT_HEADER_LENGTH, len);
		bufLen += len;

		timestamp = *((double*)(rtppack.GetPayloadData()));
		sequenceNum = *((uint32_t*)(rtppack.GetPayloadData()+ sizeof(double)));
		LOGD("VideoRTPSession  Timestamp:%lf, SequenceNumber:%u, __LINE__: %d\n", timestamp, sequenceNum, __LINE__);

		notifyJNIH264DataReceived((const jbyte* )pNaluBuf, bufLen, timestamp);
		memset(pNaluBuf, 0, bufLen);
		bufLen = 0;
	} else {
		size_t len = rtppack.GetPayloadLength() - RTP_PKT_HEADER_LENGTH;
		LOGD("VideoRTPSession ProcessRTPPacket, mark== false, pNaluBuf:%p, bufLen:%d, len: %d, __LINE__: %d\n", pNaluBuf, bufLen, len, __LINE__);
		memcpy(pNaluBuf + bufLen, rtppack.GetPayloadData() + RTP_PKT_HEADER_LENGTH, len);
		bufLen += len;


	}
}

AudioRTPSession::AudioRTPSession() :
	  pNaluBuf(NULL)
	, bufLen(0)
	, m_videoSess(NULL)
	, isSendEnded(true)
	, m_isPlaying(false)
{
	pNaluBuf = (uint8_t*)malloc(NALU_BUF_SIZE);
	if (pNaluBuf == NULL) {
		LOGE("[ERR]pNaluBuf malloc: out of memory.\n");
	}
}

AudioRTPSession::~AudioRTPSession()
{
	if (pNaluBuf) {
		free(pNaluBuf);
	}

	isSendEnded = true;
}

void AudioRTPSession::ProcessRTPPacket(const RTPSourceData &srcdat,const RTPPacket &rtppack)
{
	if (rtppack.HasMarker()) {// mark的pack是完整包的最后一个pack
		double timestamp = *((double*)rtppack.GetPayloadData());
		uint32_t sequenceNum = *((uint32_t*)(rtppack.GetPayloadData() + sizeof(double)));

		LOGD("AudioRTPSession ProcessRTPPacket Mark=true, Timestamp:%lf, sequenceNum = %d, __LINE__: %d\n", timestamp,sequenceNum, __LINE__);

//		FILE* flog = fopen("/mnt/sdcard/audiorecv.log", "a");
//		if (flog) {
//			struct  timeval    stCurTime;
//			gettimeofday( &stCurTime, NULL );
//			double curtime = (double)stCurTime.tv_sec + (double)stCurTime.tv_usec / 1000000;
////			double currentAudioTime = firstNTPTimestamp + ((double)rtppack.GetTimestamp() - (double)firstTimestamp)*(1.0/AUDIO_SAMPLING_RATE) ;
//			fprintf(flog, "currentTime:%lf, firstTimestamp:%lf, Timestamp:%lf, SequenceNumber:%u\n", curtime, firstTimestamp, timestamp, sequenceNum);
//			fclose(flog);
//		}

		size_t len = rtppack.GetPayloadLength() - RTP_PKT_HEADER_LENGTH; // add nal unit start code
		memcpy(pNaluBuf + bufLen, rtppack.GetPayloadData() + RTP_PKT_HEADER_LENGTH , len);
		bufLen += len;
		if (pNaluBuf) {
			if (bufLen > 0) {
				LOGD("AudioRTPSession ProcessRTPPacket ....................rtppack.GetPayloadType:%d\n",rtppack.GetPayloadType());
				//set aac data by cxd
				notifyJNIAACDataReceived((const jbyte* )pNaluBuf, bufLen, timestamp);
			}
		}
		memset(pNaluBuf, 0, bufLen);//清空缓存，为下次做准备
		bufLen = 0;
	} else {
		LOGD("AudioRTPSession ProcessRTPPacket Mark == false !");
		size_t len = rtppack.GetPayloadLength() - RTP_PKT_HEADER_LENGTH;
		LOGD("AudioRTPSession ProcessRTPPacket pNaluBuf:%p, bufLen:%d, len: %d\n", pNaluBuf, bufLen, len);
		memcpy(pNaluBuf + bufLen, rtppack.GetPayloadData() + RTP_PKT_HEADER_LENGTH, len);
		bufLen += len;
	}
}

void AVBaseRTPSession::OnPollThreadStep()
{

	BeginDataAccess();
	bool flag = GotoFirstSourceWithData();
//	LOGD("AVBaseRTPSession::OnPollThreadStep START,cur type=%d flat=%d", m_type, flag);
	if (flag)
    {
        do
        {
            RTPPacket *pack;
            RTPSourceData *srcdat;

            srcdat = GetCurrentSourceInfo();

            while ((pack = GetNextPacket()) != NULL)
            {
                ProcessRTPPacket(*srcdat,*pack);
                DeletePacket(pack);
            }
//            LOGD("AVBaseRTPSession::OnPollThreadStep pack ==NULL,cur type=%d", m_type);
        } while (GotoNextSourceWithData());
    }

	EndDataAccess();
}

void AVBaseRTPSession::ProcessRTPPacket(const RTPSourceData &srcdat, const RTPPacket &rtppack)
{

	LOGD("AVBaseRTPSession::ProcessRTPPacket(cur type=%d, packet Type= %d, SSRC=%d, length = %d",
			m_type, rtppack.GetPayloadType(), rtppack.GetSSRC(), rtppack.GetPayloadLength());
    if (rtppack.GetPayloadType() == RTP_VIDEO_PAYLOAD_TYPE) {//96
    	if (m_videoSess) {
    		m_videoSess->ProcessRTPPacket(srcdat, rtppack);
    	}
    } else if (rtppack.GetPayloadType() == RTP_AUDIO_PAYLOAD_TYPE) {//97
    	if (m_audioSess) {
    		m_audioSess->ProcessRTPPacket(srcdat, rtppack);
    	}
	}
}

void AVBaseRTPSession::OnRTCPCompoundPacket(RTCPCompoundPacket *pack,const RTPTime &receivetime,
                                  const RTPAddress *senderaddress)
{

    RTCPPacket *rtcppack;
    pack->GotoFirstPacket();
    while ((rtcppack = pack->GetNextPacket()) != 0)
    {
        if (rtcppack->IsKnownFormat())
        {
             switch (rtcppack->GetPacketType())
             {
                  case RTCPPacket::SR:
                  {
                      RTCPSRPacket *p = (RTCPSRPacket *)rtcppack;
                      uint32_t senderssrc = p->GetSenderSSRC();

                      LOGD("Got SR  NTP timestamp: GetMSW = %d  , GetLSW = %d\n", p->GetNTPTimestamp().GetMSW(), p->GetNTPTimestamp().GetLSW());
                      LOGD("Got SR  RTP timestamp: %d, Packet count:%d, Octet count:%d\n",  p->GetRTPTimestamp(), p->GetSenderPacketCount(), p->GetSenderOctetCount());

                  }
                  break;

                  case RTCPPacket::RR:
                  {
                      RTCPRRPacket *p = (RTCPRRPacket *)rtcppack;
                      int num = p->GetReceptionReportCount();
					  if(num == 0)
						   return;

                      int index = 0;

                      unsigned int rtt = ((receivetime.ntptimetran() - p->GetLSR(index) - p->GetDLSR(index))*1000)/65536;
                      LOGD("Got RR  lostrate %d%%, lostpkt %d, jitter %ums, rtt %ums \n",
                           p->GetFractionLost(index),
                           p->GetLostPacketCount(index),
                           p->GetJitter(index) * 1000 / 65536,
                           rtt );

                  }
                  break;
             }
        }
    }

}

void AVBaseRTPSession::OnPollThreadError(int errcode)
{
//	FILE* flog = fopen("/mnt/sdcard/error.log", "a");
//	if (flog) {
//		fprintf(flog, "AVBaseRTPSession::OnPollThreadError type = %d, errcode = %d", m_type, errcode);
//		fclose(flog);
//	}
	LOGE("AVBaseRTPSession::OnPollThreadError type = %d, errcode = %d", m_type, errcode);
}

void AVBaseRTPSession::OnTimeout(RTPSourceData *srcdat)
{
	LOGE("AVBaseRTPSession::OnTimeout, local type =%d", m_type);
	if(srcdat->IsRTPAddressSet()) {
		RTPNATTVLAddress* addr2 = (RTPNATTVLAddress*)srcdat->GetRTPDataAddress();

		LOGE("AVBaseRTPSession::OnTimeout, AddressTyp==%d ",addr2->GetAddressType());
		if(addr2->GetAddressType() == 2)
			LOGE("AVBaseRTPSession::OnTimeout, gpid==%s ",(char*)addr2->getGPID());
	}
	LOGE("AVBaseRTPSession::OnTimeout, current type=%d, srcdat->GetSSRC(): %d", m_type,srcdat->GetSSRC());
}
