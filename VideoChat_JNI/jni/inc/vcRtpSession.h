#ifndef __INCLUCDE_VCRTPSESSION_H__
#define __INCLUCDE_VCRTPSESSION_H__

#include "rtpsession.h"
#include "rtppacket.h"
#include "rtpaddress.h"
#include "rtpnattvladdress.h"
#include "rtpnattvltransmitter.h"
#include "rtpsessionparams.h"
#include "rtperrors.h"
#include "rtpsourcedata.h"
#include "IMemoryPool.h"
#include "x264.h"


enum 
{
	ERTP_STATUS_UNKNOWN = -1,
	ERTP_STATUS_INIT,
	ERTP_STATUS_ADDRESS,

};

#define QCIF_WIDTH  176
#define QCIF_HEIGHT 144
#define BUFFER_SIZE 1024*720*6

#define MAX_RTP_PKT_LENGTH 1004//(1024-12-8)

class CTestVideoDlg;

void checkerror(int rtperr);

class VideoData
{
public:
Sint32 m_iLength;
unsigned char m_pBuffer[BUFFER_SIZE];
};

class VCRtpSession: public RTPSession
{
protected:
    void OnPollThreadStep();
    void ProcessRTPPacket(const RTPSourceData &srcdat, const RTPPacket &rtppack);
	//void OnRTPPacket(RTPPacket *pack,const RTPTime &receivetime,
	                        // const RTPAddress *senderaddress);
	//void OnAPPPacket(RTCPAPPPacket *apppacket,const RTPTime &receivetime,
	                        // const RTPAddress *senderaddress);
	void OnAPPPacket(RTCPAPPPacket *apppacket,const RTPTime &receivetime,const RTPAddress *senderaddress);
	void OnBYEPacket(RTPSourceData *srcdat);
	void OnBYETimeout(RTPSourceData *srcdat);

public:
	VCRtpSession();
	~VCRtpSession();
	void SetTVD(CTestVideoDlg *pdlg) {pTVD = pdlg;}
	void SetStatus(int status) {m_status = status;}
	int GetStatus() {return m_status;}
	void SendH264Nalu(x264_nal_t* nalHdr);
	void SetParamsForSendingH264();
	void SetParamsForSendingAAC();
	void SendAACData(unsigned char* pData, int len);


private:
	CTestVideoDlg *pTVD;
	int m_status;
	IMemoryPool<VideoData> m_recvPool;
	unsigned char *m_buffer;
	int m_current_size;
};


#endif