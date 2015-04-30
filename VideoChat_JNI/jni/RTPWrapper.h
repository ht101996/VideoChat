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

#ifdef __cplusplus
extern "C"{
#endif

#include "libswresample/swresample.h"

#ifdef __cplusplus
}
#endif

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
#define AUDIO_SAMPLING_RATE	8000 // 8000
#define VIDEO_SAMPLING_RATE	90000 //90000

#define RTP_PKT_HEADER_LENGTH (sizeof(double) + sizeof(uint32_t))// timestamp + sequenceNum

enum ePixBytes{
	RGB24_BYTES = 3,
	RGB32_BYTES
};

enum eDecSpeed{
	NORM_SPEED = 1,
	TWOX_SPEED,
	THREEX_SPEED
};

enum eRemoteDevice {
	DEV_UNDEFINED,
	DEV_ANDROID,
	DEV_IOS,
	DEV_PC
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
    void OnPollThreadStep();
    virtual void OnPollThreadError(int errcode);
    void ProcessRTPPacket(const RTPSourceData &srcdat, const RTPPacket &rtppack);

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
	bool hasSendEnded() { return isSendEnded; }
	bool isPlaying() { return m_isPlaying; }
	void switchPlayingStatus() { m_isPlaying  = m_isPlaying ? false : true;  }
	double getFirstTimestamp() { return firstTimestamp; }
    double getFirstNTPTimestamp() { return firstNTPTimestamp; }
	int decodeVideoFrame();
	int displayVideoImage();
	int syncDisplayOneVideoImage(ImgElement* pImg, double currentAudioTime);
//	int syncDisplayVideoImage(double usedVideoTime);
    void ProcessRTPPacket(const RTPSourceData &srcdat, const RTPPacket &rtppack);
	void SetAudioRTPSession(AudioRTPSession* audioSess) { m_audioSess = audioSess; }
	AudioRTPSession* GetAudioRTPSession() { return m_audioSess; }

private:
	bool rgb_rotate_90(unsigned char *src_rgb, unsigned int iWidth, unsigned int iHeight, bool direction, ePixBytes bytenum);
	void checkWidthHeight(int width, int height);
	int addToImageQueue(int* data, int width, int height, double timestamp, uint32_t sequenceNumber);
	void addToNaluQueue(uint8_t* data, int len, double timestamp, uint32_t sequenceNumber);
    AVCodecContext* video_dec_ctx;
    AVCodec* video_dec;
//	AVFrame *pFrame;
	AVFrame *pFrameRGB;
	struct SwsContext *img_convert_ctx;
	int m_width;
	int m_height;
	uint8_t* out_buffer;
    uint8_t* pNaluBuf;
	uint8_t* rgbbuf;
    int   bufLen;
    std::queue<NaluElement*>*	qNalu;
    std::queue<ImgElement*>*	qImage;
    pthread_t tid_decode;
    pthread_t tid_display;
    pthread_mutex_t* pLockDecode;
    pthread_mutex_t* pLockDisplay;
    eDecSpeed m_decspeed;
    volatile double firstTimestamp;
    volatile double firstNTPTimestamp;
    double timestamp;
    double extraDeltaTime;
    uint32_t firstSequenceNum;
    uint32_t sequenceNum;
    AudioRTPSession* m_audioSess;
    enum eRemoteDevice	 eFrom;
    bool mark;
    bool isSendEnded;
    char m_incrspeedcnt;
    bool m_isPlaying;
};

class AudioRTPSession
{
public:
	AudioRTPSession();
	virtual ~AudioRTPSession();
    void ProcessRTPPacket(const RTPSourceData &srcdat, const RTPPacket &rtppack);
	bool hasSendEnded() { return isSendEnded; }
	bool isPlaying() { return m_isPlaying; }
	void switchPlayingStatus() { m_isPlaying  = m_isPlaying ? false : true;  }
	int  decodeAudioFrame();
	int  playAudioPCM();
	void SetVideoRTPSession(VideoRTPSession* videoSess) { m_videoSess = videoSess; }
	VideoRTPSession* GetVideoRTPSession() { return m_videoSess; }
	double getFirstNTPTimestamp() {return firstNTPTimestamp; }
	double getFirstTimestamp() { return firstTimestamp; }
	void reset();

private:
	void addToAACQueue(uint8_t* data, int len, double timestamp, uint32_t sequenceNumber);
	int addToPCMQueue(uint8_t* data, int len, double timestamp, uint32_t sequenceNumber);

	AVCodec* audio_codec;
	AVCodecContext* audio_codec_ctx;
	AVFrame *pFrame;
	SwrContext *swr_ctx;
	uint8_t **dst_samples_data;
    uint8_t* pNaluBuf;
    int   bufLen;
    std::queue<NaluElement*>*	qAAC;
    std::queue<NaluElement*>*	qPCM;
    pthread_t tid_decode;
    pthread_t tid_play;
    pthread_mutex_t* pLockDecode;
    pthread_mutex_t* pLockPlay;
    int	  dst_nb_channels;
    int   dst_rate;
    int   dst_nb_samples;
    enum AVSampleFormat dst_sample_fmt;
    volatile double firstTimestamp;
    volatile double firstNTPTimestamp; //firstTimestampOfVideo - firstTimestampOfAudio
    double audioMinusVideo;
    uint32_t firstSequenceNum;
    double	m_sleeptime;
    VideoRTPSession* m_videoSess;
    bool isSendEnded;
    bool m_isPlaying;
    POS_FILLER2;
};

