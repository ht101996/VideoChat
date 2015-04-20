#include <fstream>
#include <time.h>
#include <unistd.h>
#include <jni.h>
#include "inc/p2p_interface.h"
#include "RTPWrapper.h"

#ifdef __cplusplus
extern "C"{
#endif

#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libswresample/swresample.h"
#include "libavutil/opt.h"

#ifdef __cplusplus
}
#endif

#define AUDIO_TIMESTAMP_INCREMENT ((double)(1024.0 / 8000.0))
#define VIDEO_TIMESTAMP_INCREMENT ((double)(1.0 / 30.0)) // 29.745
#define IMG_DISPLAY_INTERVAL (VIDEO_TIMESTAMP_INCREMENT * 1000000) // 25 ms 25000 30000
#define AUDIO_PLAY_INTERVAL	 (AUDIO_TIMESTAMP_INCREMENT * 1000000) // 90 ms 90000 130000
#define NALU_BUF_SIZE (1024*1024*2) // 2M
#define DST_SAMPLE_RATE 8000
#define DST_CHANNEL_NUM 1
#define DST_SAMPLE_FMT  AV_SAMPLE_FMT_S16
#define PIXEL_FORMAT AV_PIX_FMT_BGRA
#define VIDEO_PACK_BUF_LIMIT	90
#define AUDIO_PACK_BUF_LIMIT	16 // 25
#define AUDIO_PLAY_PACKET_COUNT	3 //10
#define AUDIO_PLAY_TOLERATE_TIME	0.15 //s 感觉偏移存在,同步区域  0.12
#define AUDIO_PLAY_TOLERATE2_TIME	160 //可以感觉到,临界同步区域
#define AUDIO_START_WAIT_TIME	1500000 // wait 1 second before audio start
#define VIDEO_START_WAIT_TIME	1500000 // wait 1 second before video start
#define AV_RESET_INTERVAL	10 // reset firstTimestamp and firstSequenceNum every 15 seconds
#define __DECODE__ 1
#define FLIP 1

extern JavaVM *jvm;
extern jobject javaObj;
volatile double currentAudioPlayTime = 0;
pthread_mutex_t* g_pLockTimestamp = NULL;

static void notifyJNIShowImg(int* pixdata, int width, int height) {
	if (!pixdata) {
		LOGE("pixdata is NULL\n");
		return;
	}

	int len = width * height;
	JNIEnv* jniEnv = NULL;
	jclass javaClass = NULL;
	jmethodID javaMethod = NULL;
	jintArray jpixArr = NULL;
	do {
		if (!jvm) {
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
		javaClass = jniEnv->GetObjectClass(javaObj);
		if (javaClass == NULL) {
			LOGE("GetObjectClass failed.\n");
			break;
		}
		javaMethod = jniEnv->GetStaticMethodID(javaClass, "onVideoReceive",
				"([III)V");
		if (javaMethod == NULL) {
			LOGE("GetStaticMethodID failed.\n");
			break;
		}
		jpixArr = jniEnv->NewIntArray(len);
		if (jpixArr == NULL) {
			LOGE("NewIntArray Out of memory, LINE: %d\n", __LINE__);
			break;
		}
		LOGD("jpixArr: %p, pixArrLen: %d, len: %d, pixdata: %p, width: %d, height: %d\n", jpixArr, jniEnv->GetArrayLength(jpixArr), len, pixdata, width, height);
		jniEnv->SetIntArrayRegion(jpixArr, 0, len, pixdata);
		if (jniEnv->ExceptionCheck()) {
			LOGE("SetIntArrayRegion Exception.\n");
			jniEnv->ExceptionDescribe();
		} else {
			jniEnv->CallStaticVoidMethod(javaClass, javaMethod, jpixArr, width, height);
		}
	} while (false);
	if (jniEnv) {
		if (javaClass) {
			jniEnv->DeleteLocalRef(javaClass);
		}
		if (jpixArr) {
			jniEnv->DeleteLocalRef(jpixArr);
		}
	}
	if (jvm) {
		jvm->DetachCurrentThread();
	}
	// delete pixdata;
}

static void notifyJNIPlayAudio(uint8_t* audioData, int len) {
	LOGD("notifyJNIPlayAudio, LINE: %d\n", __LINE__);

	if (!audioData) {
		LOGE("audioData is NULL\n");
		return;
	}

	JNIEnv* jniEnv = NULL;
	jclass javaClass = NULL;
	jmethodID javaMethod = NULL;
	jbyteArray jpixArr = NULL;
	do {
		if (!jvm) {
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
		javaClass = jniEnv->GetObjectClass(javaObj);
		if (javaClass == NULL) {
			LOGE("GetObjectClass failed.\n");
			break;
		}
		javaMethod = jniEnv->GetStaticMethodID(javaClass, "onAudioReceive",
				"([B)V");
		if (javaMethod == NULL) {
			LOGE("GetStaticMethodID failed.\n");
			break;
		}
		jpixArr = jniEnv->NewByteArray(len);
		if (jpixArr == NULL) {
			LOGE("NewIntArray Out of memory, LINE: %d\n", __LINE__);
			break;
		}
		LOGD("jpixArr: %p, pixArrLen: %d, len: %d, audioData: %p \n", jpixArr, jniEnv->GetArrayLength(jpixArr), len, audioData);
		jniEnv->SetByteArrayRegion(jpixArr, 0, len, (jbyte*)audioData);
		if (jniEnv->ExceptionCheck()) {
			LOGE("SetIntArrayRegion Exception.\n");
			jniEnv->ExceptionDescribe();
		} else {
			LOGD("notifyJNIPlayAudio CallStaticVoidMethod, LINE: %d\n", __LINE__);
			jniEnv->CallStaticVoidMethod(javaClass, javaMethod, jpixArr, len);
		}
	} while (false);
	if (jniEnv) {
		if (javaClass) {
			jniEnv->DeleteLocalRef(javaClass);
		}
		if (jpixArr) {
			jniEnv->DeleteLocalRef(jpixArr);
		}
	}
	if (jvm) {
		jvm->DetachCurrentThread();
	}

	LOGD("notifyJNIPlayAudio, LINE: %d\n", __LINE__);
}

bool VideoRTPSession::rgb_rotate_90(unsigned char *src_rgb, unsigned int iWidth, unsigned int iHeight, bool direction, ePixBytes bytenum)
{
	if (!src_rgb) {
		LOGE("src_rgb is NULL\n");
		return false;
	}

	if (bytenum != RGB24_BYTES && bytenum != RGB32_BYTES) {
		LOGE("PIX_FMT is unsupported\n");
		return false;
	}

	unsigned int n = 0;
	unsigned int linesize = iWidth * bytenum;
	// unsigned char *dst_rgb  = (unsigned char*)malloc(iWidth*iHeight*3);
	uint8_t* dst_rgb = NULL;
	int i,j;

	if (!rgbbuf) {
		rgbbuf  = (uint8_t*)malloc(iWidth*iHeight*sizeof(int));
	}
	if (!rgbbuf) {
		LOGE("rgbbuf is NULL, out of memory, LINE: %d\n", __LINE__);
		return false;
	}
	dst_rgb = rgbbuf;

	if(direction)
	{
		for(j = iWidth; j > 0; j--)
		{
#if FLIP == 1
            for(i = 0; i < iHeight; i++)
            {
                memcpy(&dst_rgb[n], &src_rgb[linesize*i+(j-1)*bytenum], bytenum);
                n += bytenum;
            }
#else
			for(i = iHeight; i > 0; i--)
            {
                memcpy(&dst_rgb[n], &src_rgb[linesize*(i-1)+(j-1)*bytenum], bytenum);
                n += bytenum;
            }
#endif
		}
	}
	else
	{// Clockwise
		for(j = 0; j < iWidth; j++)
		{
			for(i = iHeight; i > 0; i--)
			{
				memcpy(&dst_rgb[n], &src_rgb[linesize*(i-1)+(j-1)*bytenum], bytenum);
				n += bytenum;
			}
		}
	}
	memcpy(src_rgb, dst_rgb, iWidth*iHeight*bytenum); // itmp = iLBytes*iHeight;
	// free(dst_rgb);
	return true;
}

void VideoRTPSession::checkWidthHeight(int width, int height)
{
	if (m_width != width || m_height != height) {
		m_width = width;
		m_height = height;
		if (rgbbuf) {
			free(rgbbuf);
			rgbbuf = NULL;
		}

		if (img_convert_ctx) {
			sws_freeContext(img_convert_ctx);
			img_convert_ctx = NULL;
		}
		
		if (out_buffer) {
			av_free(out_buffer);
			out_buffer = NULL;
		}
	}
}

void VideoRTPSession::addToNaluQueue(uint8_t* data, int len, double timestamp,
	    uint32_t sequenceNumber)
{
//	FILE* fh264 = fopen("/mnt/sdcard/raw.h264", "ab");
//	fwrite(data, 1, len * sizeof(uint8_t), fh264);
//	fclose(fh264);

	if (len > 0) {
		NaluElement* pNalu = new NaluElement;
		pNalu->data = new uint8_t[len];
		memcpy(pNalu->data, data, len * sizeof(uint8_t));
		pNalu->len = len;
		pNalu->timestamp = timestamp;
		pNalu->sequenceNumber = sequenceNumber;
		LOGD("addToNaluQueue,sequenceNumber:%u, LINE: %d\n",sequenceNumber, __LINE__);
		LOGD("addToNaluQueue,timestamp:%lf, LINE: %d\n",timestamp, __LINE__);
		pthread_mutex_lock(pLockDecode);
		qNalu->push(pNalu);
		pthread_mutex_unlock(pLockDecode);
	}
}

int VideoRTPSession::addToImageQueue(int* data, int width, int height, double timestamp,
	    uint32_t sequenceNumber)
{
	int len = width * height;
//	return 1;

//	FILE* frgba = fopen("/mnt/sdcard/raw.rgba", "ab");
//	int pktlen = len * sizeof(int)/* data */ + sizeof(double)/* timestamp */ + sizeof(sequenceNumber)/* sequenceNumber */;
//	fwrite(&pktlen, 1, sizeof(int), frgba);
//	fwrite(&timestamp, 1, sizeof(double), frgba);
//	fwrite(&sequenceNumber, 1, sizeof(uint32_t), frgba);
//	fwrite(data, 1, len * sizeof(int), frgba);
//	fclose(frgba);
//
//	FILE* flog = fopen("/mnt/sdcard/imgtimestamp.log", "a");
//	if (flog) {
//		fprintf(flog, "Timestamp:%lf, SequenceNumber:%u\n", timestamp, sequenceNumber);
//		fclose(flog);
//	}

	ImgElement* pImg = new ImgElement;
	pImg->data = new int[len];
	memcpy(pImg->data, data, len * sizeof(int));
	pImg->width = width;
	pImg->height = height;
	pImg->timestamp = timestamp;
	pImg->sequenceNumber = sequenceNumber;
	LOGD("addToImageQueue,sequenceNumber:%u, LINE: %d\n",sequenceNumber, __LINE__);
	LOGD("addToImageQueue,timestamp:%lf, LINE: %d\n",timestamp, __LINE__);

	//TODO SYNC VEDIO show some images
	//if in the scope of playing then play,else add to queue
	int ret = 1;//syncDisplayOneVideoImage(pImg, currentAudioPlayTime);
	LOGD("addToImageQueue, ret:%u, LINE: %d\n", ret, __LINE__);
	LOGD("addToImageQueue, currentAudioPlayTime:%lf, LINE: %d\n", currentAudioPlayTime, __LINE__);
	if (ret == 1) {
		//usleep(AUDIO_PLAY_DELAY_TIME);
		pthread_mutex_lock(pLockDisplay);
		qImage->push(pImg);
		pthread_mutex_unlock(pLockDisplay);
	} else {
		CORE_SAFEDELETEARRAY(pImg->data);
		CORE_SAFEDELETE(pImg);
	}
	return 0;
}

int VideoRTPSession::displayVideoImage()
{
//	return 1;
	while (!isPlaying()) {
		usleep(10000);
	}
	pthread_mutex_lock(pLockDisplay);
	if (qImage->size() > 0) {
//		static FILE* frgba = fopen("/mnt/sdcard/raw.rgba", "rb");
//		uint8_t arrPkt[sizeof(double)] = {0};
//		if (!frgba) {
//			return 1;
//		}
//		fread(arrPkt, 1, sizeof(int), frgba);
//		if (feof(frgba)) {
//			fseek(frgba, 0, SEEK_SET);
//		} else {
//			fseek(frgba, 0 - sizeof(int), SEEK_CUR);
//		}
//		long pos = ftell(frgba);
//		fread(arrPkt, 1, sizeof(int), frgba);
//		int pktlen = *((int*)arrPkt);
//		fread(arrPkt, 1, sizeof(double), frgba);
//		double timestamp = *((double*)arrPkt);
//	    fread(arrPkt, 1, sizeof(uint32_t), frgba);
//	    uint32_t sequenceNum = *((uint32_t*)arrPkt);
//		ImgElement* pImg = new ImgElement;
//		pImg->width = 240;
//		pImg->height = 320;
//		pImg->timestamp = timestamp;
//		pImg->sequenceNumber = sequenceNum;
//		pImg->data = new int[(pktlen - sizeof(double) - sizeof(uint32_t)) / sizeof(int)];
//		fread(pImg->data, 1, pktlen - sizeof(double) - sizeof(uint32_t), frgba);
		ImgElement* pImg = qImage->front();
		LOGD("displayVideoImage, timestamp:%lf, LINE: %d\n", pImg->timestamp, __LINE__);

		double usedVideoTime = (pImg->timestamp - firstTimestamp)*(1.0/VIDEO_SAMPLING_RATE);
		LOGD("VideoRTPSession displayVideoImage pImg->timestamp:%lf\n", pImg->timestamp);
		LOGD("VideoRTPSession displayVideoImage firstTimestamp:%lf\n", firstTimestamp);
		LOGD("VideoRTPSession displayVideoImage pImg->timestamp-firstTimestamp:%lf\n", pImg->timestamp-firstTimestamp);
		LOGD("VideoRTPSession displayVideoImage usedVideoTime:%lf\n", usedVideoTime);

		//sync time
//		struct  timeval    start;
//		struct  timeval    end;
//		gettimeofday( &start, NULL );
//		notifyJNIShowImg(pImg->data, pImg->width, pImg->height);
//		gettimeofday( &end, NULL );
//		long timeuse = 0;
//		timeuse = 1000000 * ( end.tv_sec - start.tv_sec ) + end.tv_usec - start.tv_usec;
//		LOGD("ffmpeg display video cost time is %ld\n", timeuse);
//		CORE_SAFEDELETEARRAY(pImg->data);
//		CORE_SAFEDELETE(pImg);
//		qImage->pop();
		if (!syncDisplayOneVideoImage(pImg, currentAudioPlayTime)) {
			CORE_SAFEDELETEARRAY(pImg->data);
			CORE_SAFEDELETE(pImg);
			qImage->pop();
		}
//		if (syncDisplayOneVideoImage(pImg, currentAudioPlayTime)) {
//			fseek(frgba, pos, SEEK_SET);
//		}
//		CORE_SAFEDELETEARRAY(pImg->data);
//		CORE_SAFEDELETE(pImg);

//		if (qImage->size() > VIDEO_PACK_BUF_LIMIT) {
//			for (int i = 0; i < (VIDEO_PACK_BUF_LIMIT - 60); ++i) {
//				pImg = qImage->front();
//				CORE_SAFEDELETEARRAY(pImg->data);
//				CORE_SAFEDELETE(pImg);
//				qImage->pop();
//			}
//			if (m_decspeed > NORM_SPEED) {
//				m_incrspeedcnt = 0;
//				m_decspeed = (eDecSpeed)((int)m_decspeed - 1);
//			}
//		}
	}

//	if (qImage->size() == 0 && m_decspeed < THREEX_SPEED) {
//		++m_incrspeedcnt;
//	}
//
//	if (m_incrspeedcnt > 2) {
//		m_incrspeedcnt = 0;
//		m_decspeed = (eDecSpeed)((int)m_decspeed + 1);
//	}
	pthread_mutex_unlock(pLockDisplay);
//	usleep(IMG_DISPLAY_INTERVAL); // image display interval
	return 0;
}

int VideoRTPSession::syncDisplayVideoImage(double currentAudioTime)
{
	while (qImage->size() > 0) {
		pthread_mutex_lock(pLockDisplay);
		ImgElement* pImg = qImage->front();

//		if (qImage->size() > PACK_BUF_LIMIT) {
//			for (int i = 0; i < (PACK_BUF_LIMIT - 10); ++i) {
//				pImg = qImage->front();
//				CORE_SAFEDELETEARRAY(pImg->data);
//				CORE_SAFEDELETE(pImg);
//				qImage->pop();
//			}
//
//			usleep(IMG_DISPLAY_INTERVAL); // image display interval
//		}
		int ret = syncDisplayOneVideoImage(pImg, currentAudioTime);
		if(ret == 0){
			CORE_SAFEDELETEARRAY(pImg->data);
			CORE_SAFEDELETE(pImg);
			qImage->pop();
			pthread_mutex_unlock(pLockDisplay);
		}else{
			pthread_mutex_unlock(pLockDisplay);
			break;
		}
	}
	return 0;
}

int VideoRTPSession::syncDisplayOneVideoImage(ImgElement* pImg, double currentAudioTime){
//	double currentVideoTime = firstNTPTimestamp + ((double)pImg->timestamp - (double)firstTimestamp)*(1.0/VIDEO_SAMPLING_RATE) ;
	LOGD("VideoRTPSession syncDisplayVideoImage firstTimestamp:%lf\n", firstTimestamp);
	LOGD("VideoRTPSession syncDisplayVideoImage firstNTPTimestamp:%lf\n", firstNTPTimestamp);
	LOGD("VideoRTPSession syncDisplayVideoImage pImg->sequenceNumber:%u\n", pImg->sequenceNumber);
	LOGD("VideoRTPSession syncDisplayVideoImage pImg->timestamp:%lf\n", pImg->timestamp);
	LOGD("VideoRTPSession syncDisplayVideoImage pImg->timestamp-firstTimestamp:%lf\n", pImg->timestamp-firstTimestamp);
//	LOGD("VideoRTPSession syncDisplayVideoImage currentVideoTime:%lf\n", currentVideoTime);
	LOGD("VideoRTPSession syncDisplayVideoImage currentAudioTime:%lf\n", currentAudioTime);
	double diffTime = pImg->timestamp - currentAudioTime;
	LOGD("VideoRTPSession syncDisplayVideoImage diffTime:%lf\n", diffTime);

	struct  timeval    stCurTime;
	gettimeofday( &stCurTime, NULL );
	static time_t curtime = stCurTime.tv_sec;
	if (stCurTime.tv_sec - curtime > AV_RESET_INTERVAL) {
		pthread_mutex_lock(g_pLockTimestamp);
		firstSequenceNum = firstTimestamp = 0;
		m_audioSess->reset();
		pthread_mutex_unlock(g_pLockTimestamp);
		curtime = stCurTime.tv_sec;
	}

//	FILE* flog = fopen("/mnt/sdcard/videodisplay.log", "a");
//	if (flog) {
//		struct  timeval    stCurTime;
//		gettimeofday( &stCurTime, NULL );
//		double curtime = (double)stCurTime.tv_sec + (double)stCurTime.tv_usec / 1000000;
//		static double hittime = curtime;
//		static double sign = 1;
//		if (curtime - hittime > 100) {
//			if (extraDeltaTime > 0.0009) {
//				sign *= -1;
//			} else if (extraDeltaTime < 0.0001) {
//				sign *= -1;
//			}
//			extraDeltaTime += 0.0001 * sign;
//			hittime = curtime;
//		}
//		fprintf(flog, "currentTime: %lf, currentVideoTime:%lf, currentAudioTime: %lf, diffTime: %lf, VSequenceNumber:%u\n", curtime, pImg->timestamp, currentAudioTime, diffTime, pImg->sequenceNumber);
//		fclose(flog);
//	}

	static bool bSyncVideo = true;
	static bool bDelay = true;
	int ret = 0;

	if (diffTime >= -AUDIO_PLAY_TOLERATE_TIME &&
			diffTime <= AUDIO_PLAY_TOLERATE_TIME) {
		notifyJNIShowImg(pImg->data, pImg->width, pImg->height);
		LOGD("VideoRTPSession syncDisplayVideoImage play diffTime:%lf\n", diffTime);
		//usleep(IMG_DISPLAY_INTERVAL); // image display interval
		bDelay = false;
		bSyncVideo = true;
	} else if (diffTime < -AUDIO_PLAY_TOLERATE_TIME) {//落后超过AUDIO_PLAY_TOLERATE_TIME,太多丢弃包
		LOGD("VideoRTPSession syncDisplayVideoImage delete diffTime:%lf\n", diffTime);
		bDelay = false;
		bSyncVideo = true;
	} else if (diffTime > AUDIO_PLAY_TOLERATE_TIME) {//超前超过AUDIO_PLAY_TOLERATE_TIME,延迟播放视频
		LOGD("VideoRTPSession syncDisplayVideoImage delay diffTime:%lf\n", diffTime);
		usleep(AUDIO_PLAY_DELAY_TIME);
		if (bSyncVideo) {
			static double llDelayTime = 0;
			if (!bDelay) {
				llDelayTime = 0;
			}
			llDelayTime += AUDIO_PLAY_DELAY_TIME;
			if (llDelayTime > 10000000) {
				bSyncVideo = false;
				llDelayTime = 0;
			}
			bDelay = true;
			ret = 1;
		}
	}

	if (!bSyncVideo) {
		notifyJNIShowImg(pImg->data, pImg->width, pImg->height);
		usleep(IMG_DISPLAY_INTERVAL); // image display interval
		ret = 0;
	}
	return ret;
}

void* thr_send_image_to_display(void* arg)
{
	VideoRTPSession* sess = (VideoRTPSession*)arg;
	usleep(VIDEO_START_WAIT_TIME);
	sess->switchPlayingStatus();
	while (!sess->hasSendEnded()) {
		if (sess->displayVideoImage()) {
			break;
		}
	}
	return (void*)0;
}


void* thr_decode_video(void* arg)
{
	VideoRTPSession* sess = (VideoRTPSession*)arg;
	while (!sess->hasSendEnded()) {
		if (sess->decodeVideoFrame() != 0) {
			break;
		}
	}
	return (void*)0;
}

VideoRTPSession::VideoRTPSession()
	: video_dec_ctx(NULL)
	, video_dec(NULL)
//	, pFrame(NULL)
	, pFrameRGB(NULL)
	, img_convert_ctx(NULL)
	, m_width(0)
	, m_height(0)
	, out_buffer(NULL)
	, rgbbuf(NULL)
	, mark(true)
	, isSendEnded(true)
	, pNaluBuf(NULL)
	, bufLen(0)
	, qNalu(NULL)
	, qImage(NULL)
	, tid_decode(0)
	, tid_display(0)
	, pLockDecode(NULL)
	, pLockDisplay(NULL)
	, m_decspeed(NORM_SPEED)
	, m_incrspeedcnt(0)
	, firstTimestamp(0)
	, firstNTPTimestamp(0)
	, timestamp(0)
	, firstSequenceNum(0)
	, extraDeltaTime(0)
	, sequenceNum(0)
	, m_isPlaying(false)
{
	av_register_all();

	video_dec = avcodec_find_decoder(AV_CODEC_ID_H264);
	if (!video_dec) {
		return;
	}
	video_dec_ctx = avcodec_alloc_context3(video_dec);
	if (avcodec_open2(video_dec_ctx, video_dec, NULL) < 0)
	{
		avcodec_close(video_dec_ctx);
		avcodec_free_context(&video_dec_ctx);
		video_dec = NULL;
		LOGI("%d avcodec_open error!\n", __LINE__);
	}

//	pFrame = av_frame_alloc();
	pFrameRGB = av_frame_alloc();
	pNaluBuf = (uint8_t*)malloc(NALU_BUF_SIZE);
	if (pNaluBuf == NULL) {
		LOGE("[ERR]pNaluBuf malloc: out of memory.\n");
	}
	
	qNalu = new std::queue<NaluElement*>;
	qImage = new std::queue<ImgElement*>;
	pLockDisplay = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(pLockDisplay, NULL);
//	pthread_create(&tid_display, NULL, thr_send_image_to_display, this);
	pLockDecode = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(pLockDecode, NULL);

	g_pLockTimestamp = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(g_pLockTimestamp, NULL);
//	if (!tid_display) {
//		isSendEnded = false;
//		pthread_create(&tid_display, NULL, thr_send_image_to_display, this);
//	}
}

VideoRTPSession::~VideoRTPSession()
{
	if (video_dec_ctx) {
		avcodec_close(video_dec_ctx);
		avcodec_free_context(&video_dec_ctx);
		video_dec = NULL;
	}

	if (rgbbuf) {
		free(rgbbuf);
	}
	
	if (pNaluBuf) {
		free(pNaluBuf);
	}

	if (img_convert_ctx) {
		sws_freeContext(img_convert_ctx);
	}
	
//	if (pFrame) {
//		av_frame_free(&pFrame);
//	}
	if (pFrameRGB) {
		av_frame_free(&pFrameRGB);
	}
	if (out_buffer) {
		av_free(out_buffer);
	}

	isSendEnded = true;
	pthread_join(tid_display, NULL);
	tid_display = 0;
	if (pLockDisplay) {
		pthread_mutex_destroy(pLockDisplay);
		free(pLockDisplay);
		pLockDisplay = NULL;
	}

	pthread_join(tid_decode, NULL);
	tid_decode = 0;
	if (pLockDecode) {
		pthread_mutex_destroy(pLockDecode);
		free(pLockDecode);
		pLockDecode = NULL;
	}

	if (g_pLockTimestamp) {
		pthread_mutex_destroy(g_pLockTimestamp);
		free(g_pLockTimestamp);
		g_pLockTimestamp = NULL;
	}

	while (qNalu->size() > 0) {
		NaluElement* pNalu = qNalu->front();
		CORE_SAFEDELETEARRAY(pNalu->data);
		CORE_SAFEDELETE(pNalu);
		qNalu->pop();
	}
	CORE_SAFEDELETE(qNalu);

	while (qImage->size() > 0) {
		ImgElement* pImg = qImage->front();
		CORE_SAFEDELETEARRAY(pImg->data);
		CORE_SAFEDELETE(pImg);
		qImage->pop();
	}
	CORE_SAFEDELETE(qImage);
}

void VideoRTPSession::ProcessRTPPacket(const RTPSourceData &srcdat,const RTPPacket &rtppack)
{
	LOGD("VideoRTPSession ProcessRTPPacket !");

	//LOGD("VideoRTPSession timestamp:%u, mark: %d, __LINE__: %d\n",rtppack.GetTimestamp(), mark, __LINE__);
	do {
		if (mark) {// mark的pack是完整包的最后一个pack
			LOGD("VideoRTPSession bufLen: %d, __LINE__: %d\n", bufLen, __LINE__);

			RTPTime rtptime = srcdat.SR_GetNTPTimestamp();
			LOGD("VideoRTPSession rtptime.GetDouble:%lf, __LINE__: %d\n",rtptime.GetDouble(), __LINE__);
			addToNaluQueue(pNaluBuf, bufLen, timestamp, sequenceNum);
			if (isSendEnded) {
				isSendEnded = false;
			}
			if (!tid_decode) {
				pthread_create(&tid_decode, NULL, thr_decode_video, this);
			}
#if __DECODE__ == 0
			bufLen = 0;
#else
			bufLen = 4;
#endif
//			if (NULL == pNaluBuf) {
//				pNaluBuf = (uint8_t*)malloc(bufLen);
//			} else {
//				pNaluBuf = (uint8_t*)realloc(pNaluBuf, bufLen);
//			}
//			if (NULL == pNaluBuf) {
//				LOGE("Not enough memory, __LINE__: %d\n", __LINE__);
//				break;
//			}
			
#if __DECODE__ == 1
			// nal unit start code
			pNaluBuf[0] = 0;
			pNaluBuf[1] = 0;
			pNaluBuf[2] = 0;
			pNaluBuf[3] = 1;
#endif
			size_t len = rtppack.GetPayloadLength() - 8 - RTP_PKT_HEADER_LENGTH; // add nal unit start code
//			pNaluBuf = (uint8_t*)realloc(pNaluBuf, bufLen + len);
//			if (NULL == pNaluBuf) {
//				LOGE("Not enough memory, __LINE__: %d\n", __LINE__);
//				break;
//			}
			memcpy(pNaluBuf + bufLen, rtppack.GetPayloadData() + 8 + RTP_PKT_HEADER_LENGTH, len);
			bufLen += len;

			timestamp = *((double*)(rtppack.GetPayloadData() + 8));
			sequenceNum = *((uint32_t*)(rtppack.GetPayloadData() + 8 + sizeof(double)));
			if (!firstTimestamp) {
				pthread_mutex_lock(g_pLockTimestamp);
				firstTimestamp = GetAudioRTPSession()->getFirstTimestamp() ? GetAudioRTPSession()->getFirstTimestamp() : timestamp;
				pthread_mutex_unlock(g_pLockTimestamp);
//				firstTimestamp = timestamp;
//				RTPTime rtptime = srcdat.SR_GetNTPTimestamp();
//				firstNTPTimestamp = rtptime.GetDouble();
			}
			if (!firstSequenceNum) {
				firstSequenceNum = sequenceNum;
			}
			timestamp = firstTimestamp + (double)(sequenceNum - firstSequenceNum) * (VIDEO_TIMESTAMP_INCREMENT);
			FILE* flog = fopen("/mnt/sdcard/videorecv.log", "a");
			if (flog) {
				struct  timeval    stCurTime;
				gettimeofday( &stCurTime, NULL );
				double curtime = (double)stCurTime.tv_sec + (double)stCurTime.tv_usec / 1000000;
//				double currentVideoTime = firstNTPTimestamp + ((double)rtppack.GetTimestamp() - (double)firstTimestamp)*(1.0/VIDEO_SAMPLING_RATE) ;
				fprintf(flog, "currentTime: %lf, firstTimestamp: %lf, Timestamp:%lf, SequenceNumber:%u\n", curtime, firstTimestamp, timestamp, sequenceNum);
				fclose(flog);
			}
			LOGD("VideoRTPSession firstTimestamp:%lf, __LINE__: %d\n",firstTimestamp, __LINE__);
			LOGD("VideoRTPSession  Timestamp:%lf, SequenceNumber:%u, __LINE__: %d\n", timestamp, sequenceNum, __LINE__);
#if __DECODE__ == 0
			FILE* frecv = fopen("/mnt/sdcard/recv.rgba", "ab");
			fwrite(rtppack.GetPayloadData() + 8, len, 1, frecv);
			fclose(frecv);
#endif
		} else {
//			memcpy(pNaluBuf + bufLen, rtppack.GetPayloadData(), rtppack.GetPayloadLength());
//			bufLen +=  rtppack.GetPayloadLength();
			size_t len = rtppack.GetPayloadLength() - RTP_PKT_HEADER_LENGTH;
//			pNaluBuf = (uint8_t*)realloc(pNaluBuf, bufLen + len);
//			if (NULL == pNaluBuf) {
//				LOGE("Not enough memory, __LINE__: %d\n", __LINE__);
//				break;
//			}
			LOGD("pNaluBuf:%p, bufLen:%d, len: %d, __LINE__: %d\n", pNaluBuf, bufLen, len, __LINE__);
			memcpy(pNaluBuf + bufLen, rtppack.GetPayloadData() + RTP_PKT_HEADER_LENGTH, len);
			bufLen += len;

//			timestamp = *((double*)rtppack.GetPayloadData());
//			sequenceNum = *((uint32_t*)(rtppack.GetPayloadData() + sizeof(double)));
#if __DECODE__ == 0
			FILE* frecv = fopen("/mnt/sdcard/recv.rgba", "ab");
			fwrite(rtppack.GetPayloadData(), len, 1, frecv);
			fclose(frecv);
#endif
		}
		mark = rtppack.HasMarker();
#if __DECODE__ == 0
		FILE* fmark = fopen("/mnt/sdcard/mark", "a");
		fwrite(mark?"1":"0", 1, 1, fmark);
		fclose(fmark);
#endif
	} while (false);
}

int VideoRTPSession::decodeVideoFrame()
{
	if (!video_dec) {
		LOGD("video_dec is NULL, __LINE__: %d\n", __LINE__);
		isSendEnded = true;
		return -1;
	}

	do {
//		LOGD("decodeVideoFrame bufLen: %d, __LINE__: %d\n", bufLen, __LINE__);
		AVPacket pkt;
		av_init_packet(&pkt);
		NaluElement naluElem;
		naluElem.data = NULL;
		naluElem.len = 0;

		pthread_mutex_lock(pLockDecode);
		if (qNalu->size() > 0) {
			NaluElement* pNalu = qNalu->front();
			naluElem = *pNalu;
			CORE_SAFEDELETE(pNalu);
			qNalu->pop();

//			char decspeed = (char)m_decspeed;
//			while (--decspeed > 0) {
//				if (qNalu->size() > 0) {
//					pNalu = qNalu->front();
//					CORE_SAFEDELETEARRAY(pNalu->data);
//					CORE_SAFEDELETE(pNalu);
//					qNalu->pop();
//				}
//			}

//			if (qNalu->size() > VIDEO_PACK_BUF_LIMIT) {
//				for (int i = 0; i < (VIDEO_PACK_BUF_LIMIT - 60); ++i) {
//					pNalu = qNalu->front();
//					CORE_SAFEDELETEARRAY(pNalu->data);
//					CORE_SAFEDELETE(pNalu);
//					qNalu->pop();
//				}
//			}
		}
		pthread_mutex_unlock(pLockDecode);
		pkt.data = naluElem.data;
		pkt.size = naluElem.len;
//		LOGI("decodeVideoFrame; now is doing!");

		if (pkt.size && pkt.data) {
#if __DECODE__ == 0
			if (bufLen > 0) {
				LOGD("ProcessRTPPacket ....................rtppack.GetPayloadType:%d\n",rtppack.GetPayloadType());
				notifyJNIShowImg((int*)pNaluBuf, bufLen / sizeof(int), 144, 176);
			}
#else
			LOGD("pkt.stream_index is %d\n", pkt.stream_index);
			if (pkt.stream_index == 0)
			{
				AVFrame *pFrame = av_frame_alloc();
//					AVFrame *pFrameRGB = av_frame_alloc();

				int got_picture = 0, ret = 0;
				struct  timeval    start;
				struct  timeval    end;
				gettimeofday( &start, NULL );
				ret = avcodec_decode_video2(video_dec_ctx, pFrame, &got_picture, &pkt);
				CORE_SAFEDELETEARRAY(naluElem.data);
				if (ret >= 0)
				{
					checkWidthHeight(video_dec_ctx->width, video_dec_ctx->height);
					if (out_buffer == NULL) {
						out_buffer = (uint8_t *)av_malloc(avpicture_get_size(PIXEL_FORMAT, video_dec_ctx->width, video_dec_ctx->height));
						avpicture_fill((AVPicture *)pFrameRGB, out_buffer, PIXEL_FORMAT, video_dec_ctx->width, video_dec_ctx->height);
					}

					if (!img_convert_ctx) {
						img_convert_ctx = sws_getContext(video_dec_ctx->width, video_dec_ctx->height, video_dec_ctx->pix_fmt,
								video_dec_ctx->width, video_dec_ctx->height, PIXEL_FORMAT, SWS_BICUBIC, NULL, NULL, NULL);
					}

					LOGD("decode video frame succeed, size is %dx%d\n", video_dec_ctx->width, video_dec_ctx->height);
					if (0 != got_picture)
					{
						sws_scale(img_convert_ctx, (const uint8_t* const*)pFrame->data, pFrame->linesize, 0, video_dec_ctx->height, pFrameRGB->data, pFrameRGB->linesize);
						rgb_rotate_90(pFrameRGB->data[0], video_dec_ctx->width, video_dec_ctx->height, true, RGB32_BYTES);
						gettimeofday( &end, NULL );
						long timeuse = 0;
						static long count = 0;
						timeuse = 1000000 * ( end.tv_sec - start.tv_sec ) + end.tv_usec - start.tv_usec;
						LOGD("ffmpeg decode packet count: %ld\n", ++count);
//						FILE* flog = fopen("/mnt/sdcard/video.log", "a");
//						if (flog) {
//							fprintf(flog, "ffmpeg decode video cost time is %ld\n", timeuse);
//							fclose(flog);
//						}
						LOGD("ffmpeg decode video cost time is %ld\n", timeuse);
						LOGD("naluElem.timestamp is %lf\n", naluElem.timestamp);
						LOGD("naluElem.sequenceNumber is %d\n", naluElem.sequenceNumber);

						if (addToImageQueue((int*)pFrameRGB->data[0], video_dec_ctx->height, video_dec_ctx->width, naluElem.timestamp, naluElem.sequenceNumber)) {
							return 1;
						}
						if (!tid_display) {
							pthread_create(&tid_display, NULL, thr_send_image_to_display, this);
						}
//								notifyJNIShowImg((int*)pFrameRGB->data[0], video_dec_ctx->height, video_dec_ctx->width);
					}
//							av_free(out_buffer);
				}

				av_frame_free(&pFrame);
//					av_frame_free(&pFrameRGB);
			}
#endif
		}
		// if (img_convert_ctx) {
			// sws_freeContext(img_convert_ctx);
		// }
	} while (false);
	return 0;
}

void* thr_send_pcm_to_play(void* arg)
{
	AudioRTPSession* sess = (AudioRTPSession*)arg;
	usleep(AUDIO_START_WAIT_TIME);
	sess->switchPlayingStatus();
	while (!sess->hasSendEnded()) {
		if (sess->playAudioPCM()) {
			break;
		}
	}
	return (void*)0;
}

void* thr_decode_audio(void* arg)
{
	AudioRTPSession* sess = (AudioRTPSession*)arg;
	while (!sess->hasSendEnded()) {
		if (sess->decodeAudioFrame()) {
			break;
		}
	}
	return (void*)0;
}

AudioRTPSession::AudioRTPSession()
	: audio_codec(NULL)
	, audio_codec_ctx(NULL)
	, pFrame(NULL)
	, swr_ctx(NULL)
	, dst_samples_data(NULL)
	, pNaluBuf(NULL)
	, bufLen(0)
	, qAAC(NULL)
	, qPCM(NULL)
	, tid_decode(0)
	, tid_play(0)
	, pLockDecode(NULL)
	, pLockPlay(NULL)
	, dst_nb_channels(0)
	, dst_rate(0)
	, dst_nb_samples(0)
	, dst_sample_fmt(AV_SAMPLE_FMT_NONE)
	, firstTimestamp(0)
	, firstNTPTimestamp(0)
	, firstSequenceNum(0)
	, audioMinusVideo(-0.8)
	, m_sleeptime(AUDIO_PLAY_INTERVAL)
	, m_videoSess(NULL)
	, isSendEnded(true)
	, m_isPlaying(false)
{
	av_register_all();

	audio_codec = avcodec_find_decoder(CODEC_ID_AAC);
	if (!audio_codec)
	{
		LOGE("codec not found\n");
		exit(1);
	}

	audio_codec_ctx = avcodec_alloc_context3(audio_codec);

	if (avcodec_open2(audio_codec_ctx, audio_codec, NULL) < 0)
	{
		LOGE("could not open codec\n");
		exit(1);
	}

	pFrame = av_frame_alloc();
	pNaluBuf = (uint8_t*)malloc(NALU_BUF_SIZE);
	if (pNaluBuf == NULL) {
		LOGE("[ERR]pNaluBuf malloc: out of memory.\n");
	}

	qAAC = new std::queue<NaluElement*>;
	qPCM = new std::queue<NaluElement*>;
	pLockPlay = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(pLockPlay, NULL);
	pLockDecode = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(pLockDecode, NULL);

//	if (!tid_play) {
//		isSendEnded = false;
//		pthread_create(&tid_play, NULL, thr_send_pcm_to_play, this);
//	}
}

AudioRTPSession::~AudioRTPSession()
{
	if (swr_ctx) {
		swr_free(&swr_ctx);
		for (int i = 0; i < dst_nb_channels; ++i) {
			av_freep(&dst_samples_data[i]);
		}
		av_freep(&dst_samples_data);
	}

	if (pFrame) {
		av_frame_free(&pFrame);
	}

	if (audio_codec_ctx) {
		avcodec_close(audio_codec_ctx);
		avcodec_free_context(&audio_codec_ctx);
	}

	if (audio_codec) {
		av_free(audio_codec);
	}

	if (pNaluBuf) {
		free(pNaluBuf);
	}

	isSendEnded = true;
	pthread_join(tid_play, NULL);
	tid_play = 0;
	if (pLockPlay) {
		pthread_mutex_destroy(pLockPlay);
		free(pLockPlay);
		pLockPlay = NULL;
	}

	pthread_join(tid_decode, NULL);
	tid_decode = 0;
	if (pLockDecode) {
		pthread_mutex_destroy(pLockDecode);
		free(pLockDecode);
		pLockDecode = NULL;
	}

	while (qAAC->size() > 0) {
		NaluElement* pAAC = qAAC->front();
		CORE_SAFEDELETEARRAY(pAAC->data);
		CORE_SAFEDELETE(pAAC);
		qAAC->pop();
	}
	CORE_SAFEDELETE(qAAC);

	while (qPCM->size() > 0) {
		NaluElement* pPCM = qPCM->front();
		CORE_SAFEDELETEARRAY(pPCM->data);
		CORE_SAFEDELETE(pPCM);
		qPCM->pop();
	}
	CORE_SAFEDELETE(qPCM);
}

void AudioRTPSession::ProcessRTPPacket(const RTPSourceData &srcdat,const RTPPacket &rtppack)
{
	LOGD("AudioRTPSession ProcessRTPPacket !");
	if (rtppack.HasMarker()) {// mark的pack是完整包的最后一个pack
		LOGD("AudioRTPSession bufLen: %d, __LINE__: %d\n", bufLen, __LINE__);
		double timestamp = *((double*)rtppack.GetPayloadData());
		uint32_t sequenceNum = *((uint32_t*)(rtppack.GetPayloadData() + sizeof(double)));
		if (!firstTimestamp) {
			pthread_mutex_lock(g_pLockTimestamp);
			firstTimestamp = GetVideoRTPSession()->getFirstTimestamp() ? GetVideoRTPSession()->getFirstTimestamp() : timestamp;
			pthread_mutex_unlock(g_pLockTimestamp);
			firstTimestamp -= audioMinusVideo;
//			RTPTime rtptime = srcdat.SR_GetNTPTimestamp();
//			firstNTPTimestamp = rtptime.GetDouble();
		}
		if (!firstSequenceNum) {
			firstSequenceNum = sequenceNum;
		}
		timestamp = firstTimestamp + (double)(sequenceNum - firstSequenceNum) * AUDIO_TIMESTAMP_INCREMENT;

		LOGD("AudioRTPSession firstNTPTimestamp:%lf, __LINE__: %d\n",firstNTPTimestamp, __LINE__);
		LOGD("AudioRTPSession firstTimestamp:%lf, __LINE__: %d\n",firstTimestamp, __LINE__);
		LOGD("AudioRTPSession Timestamp:%lf, SequenceNumber:%u, __LINE__: %d\n", timestamp, sequenceNum, __LINE__);

		FILE* flog = fopen("/mnt/sdcard/audiorecv.log", "a");
		if (flog) {
			struct  timeval    stCurTime;
			gettimeofday( &stCurTime, NULL );
			double curtime = (double)stCurTime.tv_sec + (double)stCurTime.tv_usec / 1000000;
//			double currentAudioTime = firstNTPTimestamp + ((double)rtppack.GetTimestamp() - (double)firstTimestamp)*(1.0/AUDIO_SAMPLING_RATE) ;
			fprintf(flog, "currentTime:%lf, firstTimestamp:%lf, Timestamp:%lf, SequenceNumber:%u\n", curtime, firstTimestamp, timestamp, sequenceNum);
			fclose(flog);
		}

		size_t len = rtppack.GetPayloadLength() - RTP_PKT_HEADER_LENGTH; // add nal unit start code
		memcpy(pNaluBuf + bufLen, rtppack.GetPayloadData() + RTP_PKT_HEADER_LENGTH, len);
		bufLen += len;
		if (pNaluBuf) {
			if (bufLen > 0) {
				LOGD("AudioRTPSession ProcessRTPPacket ....................rtppack.GetPayloadType:%d\n",rtppack.GetPayloadType());
				addToAACQueue(pNaluBuf, bufLen, timestamp, sequenceNum);
				if (isSendEnded) {
					isSendEnded = false;
				}
				if (!tid_decode) {
					pthread_create(&tid_decode, NULL, thr_decode_audio, this);
				}
			}
		}
		memset(pNaluBuf, 0, bufLen);//清空缓存，为下次做准备
		bufLen = 0;
	} else {
		LOGD("AudioRTPSession ProcessRTPPacket __LINE__: %d\n", __LINE__);
		size_t len = rtppack.GetPayloadLength() - RTP_PKT_HEADER_LENGTH;
		LOGD("AudioRTPSession ProcessRTPPacket pNaluBuf:%p, bufLen:%d, len: %d\n", pNaluBuf, bufLen, len);
		memcpy(pNaluBuf + bufLen, rtppack.GetPayloadData() + RTP_PKT_HEADER_LENGTH, len);
		bufLen += len;
	}
}

int AudioRTPSession::playAudioPCM()
{
//	return 1;
	while (!isPlaying()) {
		usleep(10000);
	}
	pthread_mutex_lock(pLockPlay);
	if (qPCM->size() > 0) {
//		NaluElement* pPCM = qPCM->front();
//		CORE_SAFEDELETEARRAY(pPCM->data);
//		CORE_SAFEDELETE(pPCM);
//		qPCM->pop();
//
//		double pPCM0Timestamp = 0;
//		uint32_t sequenceNum = 0;
		int totalLen = 0;
//		int playAudioCnt = qPCM->size() >= AUDIO_PLAY_PACKET_COUNT ? AUDIO_PLAY_PACKET_COUNT : qPCM->size();
//		uint8_t* audioData = new uint8_t[playAudioCnt * qPCM->front()->len];
//		static FILE* fpcm = fopen("/mnt/sdcard/out.g726.bit4.8K.pcm", "rb");
//		if (fpcm) {
//			if (fread(audioData, 1, 2048, fpcm) < 2048) {
//				fseek(fpcm, 0, SEEK_SET);
//				return 1;
//			}
//		}
//		for (int i = 0; i < playAudioCnt; ++i) {
//			NaluElement* pPCM = qPCM->front();
//			if (i == 0) {
//				pPCM0Timestamp = pPCM->timestamp;
//				sequenceNum = pPCM->sequenceNumber;
//			}
//			memcpy(audioData + totalLen, pPCM->data, pPCM->len);
//			totalLen += pPCM->len;
//
//			CORE_SAFEDELETEARRAY(pPCM->data);
//			CORE_SAFEDELETE(pPCM);
//			qPCM->pop();
//		}

//		static FILE* fpcm = fopen("/mnt/sdcard/raw.pcm", "rb");
//		uint8_t arrPkt[sizeof(double)] = {0};
//		if (!fpcm) {
//			return 1;
//		}
//		fread(arrPkt, 1, sizeof(int), fpcm);
//		if (feof(fpcm)) {
//			fseek(fpcm, 0, SEEK_SET);
//		} else {
//			fseek(fpcm, 0 - sizeof(int), SEEK_CUR);
//		}
//		long pos = ftell(fpcm);
//		fread(arrPkt, 1, sizeof(int), fpcm);
//		int pktlen = *((int*)arrPkt);
//		fread(arrPkt, 1, sizeof(double), fpcm);
//		double timestamp = *((double*)arrPkt);
//	    fread(arrPkt, 1, sizeof(uint32_t), fpcm);
//	    uint32_t sequenceNum = *((uint32_t*)arrPkt);
//		NaluElement* pPCM = new NaluElement;
//		pPCM->len = pktlen - sizeof(double) - sizeof(uint32_t);
//		pPCM->timestamp = timestamp;
//		pPCM->sequenceNumber = sequenceNum;
//		pPCM->data = new uint8_t[pPCM->len];
//		fread(pPCM->data, 1, pPCM->len, fpcm);
		NaluElement* pPCM = qPCM->front();
		double timestamp = pPCM->timestamp;

//		LOGD("AudioRTPSession playAudioPCM totalLen:%u\n", totalLen = pPCM->len);
//		LOGD("AudioRTPSession playAudioPCM playAudioCnt:%u\n", playAudioCnt);
//		FILE* flog = fopen("/mnt/sdcard/audiodisplay.log", "a");
//		if (flog) {
//			struct  timeval    stCurTime;
//			gettimeofday( &stCurTime, NULL );
//			double curtime = (double)stCurTime.tv_sec + (double)stCurTime.tv_usec / 1000000;
//			fprintf(flog, "currentTime: %lf, timestamp:%lf, SequenceNumber:%u\n", curtime, pPCM0Timestamp, sequenceNum);
//			fclose(flog);
//		}
//		double timestamp = pPCM->timestamp;
//		struct  timeval    start;
//		struct  timeval    end;
//		gettimeofday( &start, NULL );
		notifyJNIPlayAudio(pPCM->data, pPCM->len);
//		gettimeofday( &end, NULL );
//		long timeuse = 0;
//		timeuse = 1000000 * ( end.tv_sec - start.tv_sec ) + end.tv_usec - start.tv_usec;
//		LOGD("ffmpeg play audio cost time is %ld\n", timeuse);
//		CORE_SAFEDELETEARRAY(audioData);
//		FILE* flog = fopen("/mnt/sdcard/audioplay.log", "a");
//		if (flog) {
//			fprintf(flog, "ffmpeg play audio cost time is %ld\n", timeuse);
//			fclose(flog);
//		}
		CORE_SAFEDELETEARRAY(pPCM->data);
		CORE_SAFEDELETE(pPCM);
		qPCM->pop();

		double currentAudioTime = timestamp;//firstNTPTimestamp + ((double)pPCM0Timestamp - (double)firstTimestamp)*(1.0/AUDIO_SAMPLING_RATE) ;
		currentAudioPlayTime = currentAudioTime;
		LOGD("AudioRTPSession playAudioPCM currentAudioPlayTime:%lf\n", currentAudioPlayTime);
//		LOGD("AudioRTPSession playAudioPCM firstNTPTimestamp:%lf\n", firstNTPTimestamp);
//		LOGD("AudioRTPSession playAudioPCM pPCM->timestamp:%lf\n", pPCM0Timestamp);
//		LOGD("AudioRTPSession playAudioPCM firstTimestamp:%lf\n", firstTimestamp);
//		LOGD("AudioRTPSession playAudioPCM pPCM->timestamp-firstTimestamp:%lf\n", pPCM0Timestamp-firstTimestamp);
//		LOGD("AudioRTPSession playAudioPCM currentAudioTime:%lf\n", currentAudioTime);

		//TODO SYNC VEDIO show some images
//		GetVideoRTPSession()->syncDisplayVideoImage(currentAudioTime);


//		if (qPCM->size() > AUDIO_PACK_BUF_LIMIT) {
//			for (int i = 0; i < (AUDIO_PACK_BUF_LIMIT - 10); ++i) {
//				NaluElement* pPCM = qPCM->front();
//				CORE_SAFEDELETEARRAY(pPCM->data);
//				CORE_SAFEDELETE(pPCM);
//				qPCM->pop();
//			}
////			if (m_decspeed > NORM_SPEED) {
////				m_incrspeedcnt = 0;
////				m_decspeed = (eDecSpeed)((int)m_decspeed - 1);
////			}
//		}
	}

//	if (qImage->size() == 0 && m_decspeed < THREEX_SPEED) {
//		++m_incrspeedcnt;
//	}
//
//	if (m_incrspeedcnt > 2) {
//		m_incrspeedcnt = 0;
//		m_decspeed = (eDecSpeed)((int)m_decspeed + 1);
//	}
	pthread_mutex_unlock(pLockPlay);
	usleep(m_sleeptime); // 128000
//	m_sleeptime -= 34;
	return 0;
}

void AudioRTPSession::addToAACQueue(uint8_t* data, int len, double timestamp,
		uint32_t sequenceNumber)
{
//	FILE* faac = fopen("/mnt/sdcard/raw.aac", "ab");
//	fwrite(data, 1, len * sizeof(uint8_t), faac);
//	fclose(faac);

	if (len > 0) {
		NaluElement* pAAC = new NaluElement;
		pAAC->data = new uint8_t[len];
		if (!pAAC->data) {
			CORE_SAFEDELETE(pAAC);
			return;
		}
		memcpy(pAAC->data, data, len * sizeof(uint8_t));
		pAAC->len = len;
		pAAC->timestamp = timestamp;
		pAAC->sequenceNumber = sequenceNumber;
		pthread_mutex_lock(pLockDecode);
		qAAC->push(pAAC);
		pthread_mutex_unlock(pLockDecode);
	}
}

int AudioRTPSession::addToPCMQueue(uint8_t* data, int len, double timestamp,
		uint32_t sequenceNumber)
{
//	return 1;
//	FILE* fpcm = fopen("/mnt/sdcard/raw.pcm", "ab");
//	int pktlen = len/* data */ + sizeof(double)/* timestamp */ + sizeof(uint32_t)/* sequenceNumber */;
//	fwrite(&pktlen, 1, sizeof(int), fpcm);
//	fwrite(&timestamp, 1, sizeof(double), fpcm);
//	fwrite(&sequenceNumber, 1, sizeof(uint32_t), fpcm);
//	fwrite(data, 1, len * sizeof(uint8_t), fpcm);
//	fclose(fpcm);
//
//	FILE* flog = fopen("/mnt/sdcard/pcmtimestamp.log", "a");
//	if (flog) {
//		fprintf(flog, "Timestamp: %lf\n", timestamp);
//		fclose(flog);
//	}
//
	NaluElement* pPCM = new NaluElement;
	pPCM->data = new uint8_t[len];
	if (!pPCM->data) {
		CORE_SAFEDELETE(pPCM);
		return 1;
	}
	memcpy(pPCM->data, data, len * sizeof(uint8_t));
	pPCM->len = len;
	pPCM->timestamp = timestamp;
	pPCM->sequenceNumber = sequenceNumber;
	pthread_mutex_lock(pLockPlay);
	qPCM->push(pPCM);
	pthread_mutex_unlock(pLockPlay);
	return 0;
}

int AudioRTPSession::decodeAudioFrame()
{
	int got_frame;
	int src_sample_size = 0;
	int ret = 0;
	int dst_linesize = 0;
	int dst_bufsize = 0;
	int src_rate = 0;
	int src_nb_channels = 0;
	enum AVSampleFormat src_sample_fmt = AV_SAMPLE_FMT_NONE;

	if (!audio_codec) {
		LOGD("audio_codec is NULL, __LINE__: %d\n", __LINE__);
		isSendEnded = true;
		return -1;
	}

	AVPacket avpkt;
	av_init_packet(&avpkt);
	NaluElement aacElem;
	aacElem.data = NULL;
	aacElem.len = 0;
	aacElem.timestamp = 0;

	pthread_mutex_lock(pLockDecode);
	if (qAAC->size() > 0) {
		NaluElement* pAAC = qAAC->front();
		aacElem = *pAAC;
		LOGD("aacElem.timestamp is %lf\n", aacElem.timestamp);
		CORE_SAFEDELETE(pAAC);
		qAAC->pop();

//		char decspeed = (char)m_decspeed;
//		while (--decspeed > 0) {
//			if (qNalu->size() > 0) {
//				pNalu = qNalu->front();
//				CORE_SAFEDELETEARRAY(pNalu->data);
//				CORE_SAFEDELETE(pNalu);
//				qNalu->pop();
//			}
//		}

		if (qAAC->size() > AUDIO_PACK_BUF_LIMIT) {
			for (int i = 0; i < (AUDIO_PACK_BUF_LIMIT - 10); ++i) {
				pAAC = qAAC->front();
				CORE_SAFEDELETEARRAY(pAAC->data);
				CORE_SAFEDELETE(pAAC);
				qAAC->pop();
			}
		}
	}
	pthread_mutex_unlock(pLockDecode);
	avpkt.data = aacElem.data;
	avpkt.size = aacElem.len;

	/* decode and resample */
	while (avpkt.size > 0 && avpkt.data)
	{
		struct  timeval    start;
		struct  timeval    end;
		gettimeofday( &start, NULL );
		int len = avcodec_decode_audio4(audio_codec_ctx, pFrame, &got_frame, &avpkt);
		if (len <= 0) {
			LOGE("Error while decoding, ret: %d\n", len);
			break;
		}

		if (!src_sample_size) {
			src_sample_size = av_get_bytes_per_sample(audio_codec_ctx->sample_fmt);
			src_rate = audio_codec_ctx->sample_rate;
			src_sample_fmt = audio_codec_ctx->sample_fmt;
			src_nb_channels = audio_codec_ctx->channels;
			LOGD("bit rate: %03d\n", audio_codec_ctx->bit_rate);
			LOGD("decoder name: %s\n", audio_codec_ctx->codec->long_name);
			LOGD("channels:  %d \n", audio_codec_ctx->channels);
			LOGD("sample per second  %d \n", audio_codec_ctx->sample_rate);
		}

		if (got_frame > 0)
		{
			LOGD("decodeAudioFrame success, LINE: %d\n", __LINE__);
//			FILE* fpcm = fopen("/mnt/sdcard/test.pcm", "ab");
//			fwrite(pFrame->data[0], 1, pFrame->nb_samples * src_sample_size, fpcm);
//			fclose(fpcm);
			/*double pts = (double) avpkt.pts / 45000;
			printf("[time: %.3f] sound sample \n", pts);*/

			if (src_sample_fmt != DST_SAMPLE_FMT || src_rate != DST_SAMPLE_RATE) {
				if (NULL == swr_ctx) {
					/* create resampler context */
					swr_ctx = swr_alloc();
					if (!swr_ctx) {
						LOGE("Could not allocate resampler context\n");
						ret = -1;
						break;
					}

					dst_rate = DST_SAMPLE_RATE;
					dst_nb_channels = DST_CHANNEL_NUM;
					dst_sample_fmt = DST_SAMPLE_FMT;

					/* set options */
					av_opt_set_int(swr_ctx, "in_channel_layout", av_get_default_channel_layout(src_nb_channels), 0);
					av_opt_set_int(swr_ctx, "in_sample_rate", src_rate, 0);
					av_opt_set_sample_fmt(swr_ctx, "in_sample_fmt", src_sample_fmt, 0);
					av_opt_set_int(swr_ctx, "out_channel_layout", av_get_default_channel_layout(dst_nb_channels), 0);
					av_opt_set_int(swr_ctx, "out_sample_rate", dst_rate, 0);
					av_opt_set_sample_fmt(swr_ctx, "out_sample_fmt", dst_sample_fmt, 0);

					/* initialize the resampling context */
					if ((ret = swr_init(swr_ctx)) < 0) {
						LOGE("Failed to initialize the resampling context\n");
					}

					/* compute the number of converted samples: buffering is avoided
					 * ensuring that the output buffer will contain at least all the
					 * converted input samples */
					int max_dst_nb_samples = dst_nb_samples =
						av_rescale_rnd(pFrame->nb_samples, dst_rate, src_rate, AV_ROUND_UP);

					/* buffer is going to be directly written to a rawaudio file, no alignment */
					ret = av_samples_alloc_array_and_samples(&dst_samples_data, &dst_linesize, dst_nb_channels,
															 dst_nb_samples, dst_sample_fmt, 1);

					/* compute destination number of samples */
					dst_nb_samples = av_rescale_rnd(swr_get_delay(swr_ctx, src_rate) +
													pFrame->nb_samples, dst_rate, src_rate, AV_ROUND_UP);
					if (dst_nb_samples > max_dst_nb_samples) {
						av_freep(&dst_samples_data[0]);
						ret = av_samples_alloc(dst_samples_data, &dst_linesize, dst_nb_channels,
							dst_nb_samples, dst_sample_fmt, 1);
						if (ret < 0) {
							break;
						}
						max_dst_nb_samples = dst_nb_samples;
					}
				}

				/* convert to destination format */
				ret = swr_convert(swr_ctx, dst_samples_data, dst_nb_samples,
								  (const uint8_t **)pFrame->data, pFrame->nb_samples);
				gettimeofday( &end, NULL );
				long timeuse = 0;
				static long count = 0;
				timeuse = 1000000 * ( end.tv_sec - start.tv_sec ) + end.tv_usec - start.tv_usec;
				LOGD("ffmpeg decode audio cost time is %ld\n", timeuse);
//				FILE* flog = fopen("/mnt/sdcard/audio.log", "a");
//				if (flog) {
//					fprintf(flog, "ffmpeg decode audio cost time is %ld\n", timeuse);
//					fclose(flog);
//				}
				dst_bufsize = av_samples_get_buffer_size(&dst_linesize, dst_nb_channels,
														 ret, dst_sample_fmt, 1);

				LOGD("audio convert success.\n");
				ret = 0;
				LOGD("aacElem.timestamp is %lf\n", aacElem.timestamp);
				if (addToPCMQueue(dst_samples_data[0], dst_bufsize, aacElem.timestamp, aacElem.sequenceNumber)) {
					return 1;
				}
			} else {
				LOGD("aacElem.timestamp is %lf\n", aacElem.timestamp);
				if (addToPCMQueue(pFrame->data[0], pFrame->nb_samples * src_sample_size, aacElem.timestamp, aacElem.sequenceNumber)) {
					return 1;
				}
			}

			if (!tid_play) {
				pthread_create(&tid_play, NULL, thr_send_pcm_to_play, this);
			}
		}

		avpkt.size -= len;
		avpkt.data += len;
	}

	av_free_packet(&avpkt);
	usleep(32000); // audio play interval
	return ret;
}

void AudioRTPSession::reset()
{
	firstSequenceNum = firstTimestamp = 0;
	m_sleeptime = AUDIO_PLAY_INTERVAL;
	audioMinusVideo = -0.4;
}

void AVBaseRTPSession::OnPollThreadStep()
{
	BeginDataAccess();
    if (GotoFirstSourceWithData())
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
        } while (GotoNextSourceWithData());
    }

	EndDataAccess();
}

void AVBaseRTPSession::ProcessRTPPacket(const RTPSourceData &srcdat, const RTPPacket &rtppack)
{
	LOGD("AVBaseRTPSession::ProcessRTPPacket rtppack.GetPayloadType()= %d",rtppack.GetPayloadType());
	LOGD("AVBaseRTPSession::ProcessRTPPacket rtppack.GetPayloadLength()= %d",rtppack.GetPayloadLength());
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

void AVBaseRTPSession::OnPollThreadError(int errcode)
{
//	FILE* flog = fopen("/mnt/sdcard/error.log", "a");
//	if (flog) {
//		fprintf(flog, "AVBaseRTPSession::OnPollThreadError type = %d, errcode = %d", m_type, errcode);
//		fclose(flog);
//	}
	LOGE("AVBaseRTPSession::OnPollThreadError type = %d, errcode = %d", m_type, errcode);
}
