#pragma once

#include "pos_addr.h"
#include "msgc_msg.h"
#include "msgc_service.h"
#include "msgc_protocol.h"

#ifdef UFTP_API
#include "uftp_pub.h"
#endif

#include "cm.h"

#ifdef WIN32
#include <ObjBase.h>
#endif

#ifndef WIN32
#define WINAPI
#define interface		class
#define PURE			= 0
#ifndef NULL
#define NULL			0
#endif
#define E_INVALIDARG	-1
#define E_OUTOFMEMORY	-2
#define E_FAIL			-3
#define NOERROR			0
typedef int			HRESULT;
typedef unsigned char	BYTE;
typedef unsigned int	DWORD;
typedef int *			LPARAM;
#endif

#define RTP_VIDEO_PAYLOAD_TYPE	96 //h264
#define RTP_AUDIO_PAYLOAD_TYPE  97 //aac

enum {
	MSG_TYPE_DEFAULT = 0,
	MSG_TYPE_DEFAULT_ACK = 1,
	MSG_TYPE_UFTP_BEGIN = 9,
	MSG_TYPE_UFTP_ACTIVE_PUT,		  //the file receiver will recv this message from uftp module, by cm.
	/*
	* the file receiver can send this message directly to file sender, 
	* to notify file trasfer failure if it is necessary, this is optional, currently not included in sample code.
	*/
	MSG_TYPE_UPLOAD_FILE_NOTIFY,  
	MSG_TYPE_APP_MAX = 99,
	MSG_TYPE_RTP_VIDEO,
	MSG_TYPE_RTP_AUDIO,
	MSG_TYPE_RTCP_VIDEO,
	MSG_TYPE_RTCP_AUDIO,

	MSG_TYPE_MAX = 253
};

#ifdef UFTP_API
/*Message body of MSG_TYPE_UPLOAD_FILE_NOTIFY, app can define this message as their need, 
* the define of message below is just an example. 
*/
typedef struct App_File_Upload_ACK_st{
	Uint32 appSID;
	UFTP_APP_Notification_Type errorCode;
} App_File_Upload_ACK;
#endif

/*
*	Call back of cm, used for receiving data from network.
*/
interface ICMClientCallBack
{
public:

	virtual void HandleCmcEvent(
		DWORD msgID, 
		DWORD wParam, 
		LPARAM pParam, 
		void *srcAddr, 
		void *user = NULL
		) PURE;

};

interface IRTPCallBack
{
public:
	virtual void HandleRTPEvent(
		DWORD msgID, 
		DWORD wParam, 
		LPARAM pParam, 
		void *srcAddr, 
		void *user = NULL
		) PURE;
};

/*
*	Call back of msg center, used for receiving data from other local modules..
*/
interface IMsgcClientCallBack
{
public:

	virtual void HandleMsgccEvent(
		DWORD msgID,	//msg type
		DWORD wParam,	// protocol type
		LPARAM pParam,	// msg body
		void *user = NULL
		) PURE;

};

/************************************************************************************************
 *                                                                                              *
 *                  P2P PLATFORM INTERFACE														*
 *                                                                                              *
 ************************************************************************************************/

interface ICMClient
#ifdef WIN32
: public IUnknown
#endif
{
public:

	/*
	 *    Name: Init
	 *    Description:
	 *        Interface for init p2p platform.
	 *    Parameters:
	 *        configPath : the path for platform reading config file, if it is NULL, current working directory will be used.
	 *        pCallBack : callback function for receiving data form network.
	 *		  user: the class instance for processing network data, can be NULL.
	 *    Return:
	 *        =0 : success.
	 *        < 0 : failed, check the error number form p2p_errno.h.
	 *    Remark:
	 *
	 */
	virtual HRESULT WINAPI Init(char *configPath, ICMClientCallBack *pCallBack, void *user) PURE;

	virtual HRESULT WINAPI RTPInit(IRTPCallBack *pCallBack, void *videoUser, void *audioUser = NULL) PURE;

	 /*
	 *    Name: GetID
	 *    Description:
	 *        Interface for getting local GPID.
	 *    Parameters:
	 *        pID : array to store the GPID in 20B format.
	 *    Return:
	 *        =0 : success.
	 *        -1 : failed, currently GPID has not got form cm.
	 *    Remark:
	 *
	 */
	virtual HRESULT WINAPI GetID(BYTE pID[20]) PURE;

	 /*
	 *    Name: InitMsgcClient
	 *    Description:
	 *        Interface for init msgcenter.
	 *    Parameters:
	 *        pMsgcCallback : call back function for receiving data from msgcenter.
	 *		  param: the class instance for processing the received msg, can be NULL.
	 *    Return:
	 *        =0 : success.
	 *        -1 : failed, check the error number form p2p_errno.h.
	 *    Remark:
	 *
	 */
	virtual HRESULT WINAPI InitMsgcClient(IMsgcClientCallBack *pMsgcCallback, void *param = NULL) PURE;

	 /*
	 *    Name: SendTo
	 *    Description:
	 *        Interface for send a piece of data to a peer of GPID.
	 *    Parameters:
	 *        buf : msg payload
	 *		  len: payload length in Byte.
	 *        pID: GPID of the destination node.
	 *		  msgType: message type of this message, for the convenience of parse and process the package.
	 *    Return:
	 *        =0 : success.
	 *        <0 : failed, check the error number form p2p_errno.h.
	 *    Remark:
	 *
	 */
	virtual HRESULT WINAPI SendTo(const char *buf, int len, unsigned char pID[20], unsigned char msgType = 0) PURE;

	 /*
	 *    Name: SendTo
	 *    Description:
	 *        Interface for send a piece of data to a peer of IP/PORT.
	 *    Parameters:
	 *        buf : msg payload
	 *		  len: payload length in Byte.
	 *        dstip: the ipv4 address of the destination node, in 32B network bytes order
	 *        dstport: the port of destination node, in host bytes order.
	 *		  msgType: message type of this message, for the convenience of parse and process the package.
	 *    Return:
	 *        =0 : success.
	 *        <0 : failed, check the error number form p2p_errno.h.
	 *    Remark:
	 *
	 */
	virtual HRESULT WINAPI SendToip(const char *buf, int len, unsigned int dstip, unsigned short dstport, 
		unsigned char msgType = 0) PURE;

	 /*
	 *    Name: UnInit
	 *    Description:
	 *        Interface for uninit p2p platform.
	 *    Parameters:
	 *    Return:
	 *        =0 : success.
	 *    
	 *    Remark:
	 *
	 */
	virtual HRESULT WINAPI UnInit() PURE; 

/************************************************************************/
/*							     FILE TRANSFER API                       */
/************************************************************************/ 
#ifdef UFTP_API

	//UFTP interface by msgc
	 /*
	 *    Name: SendStartUploadReq
	 *    Description:
	 *        Interface for File sender to launch file transfer.
	 *    Parameters:
	 *        localSID : session id of app layer, for app to indentify a file transfer session,uftp will not use the id ,but return back to app.
	 *		  remoteFileName: full path file name of the remote side.
	 *        localFileName: full path file name of the local side.
	 *        gpid: the gpid of remote side.
	 *		  
	 *    Return:
	 *        =0 : success.
	 *        <0 : failed, check the error number form p2p_errno.h.
	 *    Remark:
	 *
	 */
	virtual HRESULT WINAPI SendStartUploadReq(unsigned int localSID, const char *remoteFileName, 
		const char *localFileName, const unsigned char gpid[20]) PURE;
	
	/*
	 *    Name: SendStartUploadReq
	 *    Description:
	 *        Interface for File sender to launch file transfer.
	 *    Parameters:
	 *        localSID : session id of app layer, for app to indentify a file transfer session,uftp will not use the id ,but return back to app.
	 *		  remoteFileName: full path file name of the remote side.
	 *        localFileName: full path file name of the local side.
	 *        dstip: the ipv4 address of remote side, in 32B network bytes order.
	 *		  dstport:the port of remote side, in host bytes order
	 *    Return:
	 *        =0 : success.
	 *        <0 : failed, check the error number form p2p_errno.h.
	 *    Remark:
	 *
	 */
	virtual HRESULT WINAPI SendStartUploadReq(unsigned int localSID, const char *remoteFileName, 
		const char *localFileName, const unsigned int dstip, const unsigned short dstport) PURE;

	 /*
	 *    Name: SendPutAck
	 *    Description:
	 *        Interface for File receiver to answer the transfer request.
	 *    Parameters:
	 *        acceptOrNot : accept the transfer request or not.
	 *		  errorCode: reasons for refuse the request.
	 *        savePath: full path file name for saving or re-saving the file.
	 *        uftpSid: local uftp session id , got from msgc callback.
	 *    Return:
	 *        =0 : success.
	 *        <0 : failed, check the error number form p2p_errno.h.
	 *    Remark:
	 *
	 */
	virtual HRESULT WINAPI SendPutAck(Bool8 acceptOrNot,UFTP_APP_Notification_Type errorCode,
		const Sint8 *savePath,Uint32 uftpSid) PURE;

	 /*
	 *    Name: SendCancelTransferInd
	 *    Description:
	 *        Interface for cancel file transfer.
	 *    Parameters:
	 *        uftp_sid : session id of uftp layer, uftp sid can be got from msgs from msgcenter.
	 *		  ctrl_type: cancle type, see details in uftp_pub.h.
	 *        role_type: role type in file transfer, role type can be got from msgs from msgcenter.
	 *    Return:
	 *        =0 : success.
	 *        <0 : failed, check the error number form p2p_errno.h.
	 *    Remark:
	 *
	 */
	virtual HRESULT WINAPI SendCancelTransferInd(unsigned int uftp_sid, EUFTP_APP_STOP_TYPE ctrl_type, 
		UFTP_Transfer_Role role_type) PURE;

	/*
	 *    Name: SendGetTransferStatusReq
	 *    Description:
	 *        Interface for get transfer status from uftp, this call is asynchronous, results can be got from msgcenter callback.
	 *    Parameters:
	 *        uftp_sid : session id of uftp layer, uftp sid can be got from msgs from msgcenter.
	 *        role_type: role type in file transfer, role type can be got from msgs from msgcenter.
	 *    Return:
	 *        =0 : success.
	 *        <0 : failed, check the error number form p2p_errno.h.
	 *    Remark:
	 *
	 */
	virtual HRESULT WINAPI SendGetTransferStatusReq(unsigned int uftp_sid, UFTP_Transfer_Role role_type) PURE;

	 /*
	 *    Name: SendUploadAck
	 *    Description:
	 *        Interface for file receiver to send a upload response directly to file sender.(not from uftp) This interface is optional.
	 *    Parameters:
	 *        appSID : File transfer session id of app layer(sender's), extract from ExtractFileInfo Interface.
	 *        errorCode: refuse or some other reasons .
	 *        dstAddr: the address of file sender.
	 *    Return:
	 *        =0 : success.
	 *        <0 : failed, check the error number form p2p_errno.h.
	 *    Remark:
	 *
	 */
	virtual HRESULT WINAPI SendUploadAck(Uint32 appSID, UFTP_APP_Notification_Type errorCode, Tpos_addr *dstAddr) PURE;

	 /*
	 *    Name: ExtractFileInfo
	 *    Description:
	 *        Interface extract some info of file transfer session .
	 *    Parameters:
	 *        pUftpPutReq : message of uftp to the receiver app, to indicate the file receiver  a putting file request . In Param.
	 *        localFileName: extract the file save path from pUftpPutReq. In/Out Param.
	 *        errorCode:  indicate whether the extraction is ok, or the error type of the extract procedure. In/Out Param.
	 *        appSID: extract the sender 's file transfer session id of app layer. In/Out Param.
	 *        GPID: extract the sender 's GPID. In/Out Param.
	 *    Return:
	 *        =0 : success.
	 *        <0 : failed, check the error number form p2p_errno.h.
	 *    Remark:
	 *
	 */
	virtual HRESULT WINAPI ExtractFileInfo(Tuftp_UftpPutReq *pUftpPutReq,Sint8 *localFileName,
		UFTP_APP_Notification_Type &errorCode, Uint32 &appSID,Uint8 *GPID) PURE;
#endif

	/************************************************************************/
	/*							     CM API                                 */
	/************************************************************************/ 
	/*
	*	Currently not used.
	*/
	virtual int WINAPI CanDirectConnect(void *dstGPID) PURE;

		 /*
	 *    Name: GetTransProtocol
	 *    Description:
	 *        Interface for get a connection's transfer protocol .UDP/TCP. This is a asynchronous call, results can be got from msgc callback.(ECM_NET_TRANS_PROTOCOL_QUERY_CNF)
	 *    Parameters:
	 *        dstAddr : the destination node's addr .
	 *    Return:
	 *        =0 : success.
	 *        -1: param error.
	 *    Remark:
	 *
	 */
	virtual int WINAPI GetTransProtocol(const Tpos_addr *dstAddr) PURE;
	/*
	*	Currently not used.
	*/
	virtual int WINAPI GetLanGPIDs(Uint32 ssid) PURE;

	virtual int WINAPI GetServerStatus(Bool8* isNatServerOk, Bool8* isTcpServerOk) PURE;

};

#ifdef __cplusplus
extern"C"
{
#endif

#ifdef WIN32
	#ifdef CMCLIENT_EXPORTS
		#define CMCLIENT_DLL __declspec(dllexport)
	#else
		#define CMCLIENT_DLL __declspec(dllimport)
	#endif
#else
	#define CMCLIENT_DLL
#endif

	extern CMCLIENT_DLL HRESULT GetCmcObject(ICMClient** ppIObject);

	typedef HRESULT (*GETCMCOBJECT)(ICMClient** ppIObject);

#ifdef __cplusplus
}
#endif
