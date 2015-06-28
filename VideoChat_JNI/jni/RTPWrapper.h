#include <android/log.h>
#include <pthread.h>
#include <queue>
#include "inc/pos_type.h"
#include "rtpsession.h"
#include "rtppacket.h"
#include "rtpsourcedata.h"
#include "rtpnattvltransmitter.h"
#include "rtpnattvladdress.h"
#include "rtpsessionparams.h"
#include "rtperrors.h"
#include "rtcprrpacket.h"
#include "rtcpsrpacket.h"
#include "rtpaddress.h"


#define log_enable

#ifdef log_enable

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "JNI", __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, "JNI", __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, "JNI", __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, "JNI", __VA_ARGS__)
#else
#define LOGI(...)
#define LOGE(...)
#define LOGD(...)
#define LOGW(...)
#endif
#define CORE_SAFEDELETE(p) if ((p) != NULL) { delete (p); (p) = NULL; }
#define CORE_SAFEDELETEARRAY(p) if ((p) != NULL) { delete [] (p); (p) = NULL; }
#define AUDIO_PLAY_DELAY_TIME	10000 //10ms 每播放完一帧时,需要延迟的时间 10000
#define AUDIO_SAMPLING_RATE	44100//8000 // 8000
#define VIDEO_SAMPLING_RATE	90000 //90000

#define RTP_PKT_HEADER_LENGTH (sizeof(double) + sizeof(uint32_t))// timestamp + sequenceNum

enum ePixBytes{
	RGB24_BYTES = 3,
	RGB32_BYTES
};

typedef struct
{
    int i_ref_idc;  /* nal_priority_e */
    int i_type;     /* nal_unit_type_e */
    int     i_payload;
    uint8_t *p_payload;
} h264_nal_t;

typedef struct
{
    int     i_payload;
    uint8_t *p_payload;
} aac_nal_t;

typedef struct _TNaluElement
{
	int len;
	uint8_t* data; // nalu
    double timestamp;
    uint32_t sequenceNumber;
} NaluElement;

typedef struct _TImgElement
{
	int width;
	int height;
	int* data; // rgba
    double timestamp;
    uint32_t sequenceNumber;
} ImgElement;

extern void SendH264Nalu(h264_nal_t* nalHdr, RTPSession& sess);
extern void SendAACNalu(aac_nal_t* nalHdr, RTPSession& sess);

struct AVCodecContext;
struct AVCodec;
struct AVFrame;
struct SwsContext;
struct SwrContext;
class VideoRTPSession;
class AudioRTPSession;


class AVBaseRTPSession : public RTPSession
{
public:
	AVBaseRTPSession(uint8_t type) : m_type(type), m_videoSess(NULL), m_audioSess(NULL) {}
	virtual ~AVBaseRTPSession() {}
	void SetVideoRTPSession(VideoRTPSession* videoSess) { m_videoSess = videoSess; }
	VideoRTPSession* GetVideoRTPSession() { return m_videoSess; }
	void SetAudioRTPSession(AudioRTPSession* audioSess) { m_audioSess = audioSess; }
	AudioRTPSession* GetAudioRTPSession() { return m_audioSess; }

protected:
	virtual void OnPollThreadStep();
    virtual void OnPollThreadError(int errcode);
    void ProcessRTPPacket(const RTPSourceData &srcdat, const RTPPacket &rtppack);
	virtual void OnRTCPCompoundPacket(RTCPCompoundPacket *pack,const RTPTime &receivetime,
	                                  const RTPAddress *senderaddress);
	virtual void OnTimeout(RTPSourceData *srcdat);

private:
    uint8_t			 m_type;
    VideoRTPSession* m_videoSess;
    AudioRTPSession* m_audioSess;
};

class VideoRTPSession
{
public:
	VideoRTPSession();
	virtual ~VideoRTPSession();
    void ProcessRTPPacket(const RTPSourceData &srcdat, const RTPPacket &rtppack);
    void Destroy();

private:
    uint8_t* pNaluBuf;
    int   bufLen;
    pthread_mutex_t* pLock;
};

class AudioRTPSession
{
public:
	AudioRTPSession();
	virtual ~AudioRTPSession();
    void ProcessRTPPacket(const RTPSourceData &srcdat, const RTPPacket &rtppack);
	void Destroy();

private:
    uint8_t* pNaluBuf;
    int   bufLen;
};

