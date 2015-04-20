/*----------------------------------------------------------------------------------------------
*
* This file is ArcSoft's property. It contains ArcSoft's trade secret, proprietary and         
* confidential information. 
* 
* The information and code contained in this file is only for authorized ArcSoft employees 
* to design, create, modify, or review.
* 
* DO NOT DISTRIBUTE, DO NOT DUPLICATE OR TRANSMIT IN ANY FORM WITHOUT PROPER AUTHORIZATION.
* 
* If you are not an intended recipient of this file, you must not copy, distribute, modify, 
* or take any action in reliance on it. 
* 
* If you have received this file in error, please immediately notify ArcSoft and 
* permanently delete the original and any copy of any file and any printout thereof.
*
*-------------------------------------------------------------------------------------------------*/

/*
* File Name:       uftp_pub.h
*
* Reference:
*
* Author:          qqw
*
* Description:
*      Uftp public interfaces.
*
* History:
*      please record the history in the following format:
*      verx.y.z    date        author      description or bug/cr number
*      ----------------------------------------------------------------
*      ver0.0.1    09/07/2009  qqw     first draft
*  
*CodeReview Log:
*      please record the code review log in the following format:
*      verx.y.z    date        author      description
*      ----------------------------------------------------------------
* 
*/


#ifndef _UFTP_PUB_H_
#define _UFTP_PUB_H_


#if defined (WIN32_SWITCH)
#pragma warning (disable : 4200)
#endif


#include "pos_type.h"                   // platform type head

#ifndef WIN32
#define MAX_PATH 260
#endif

#ifdef __cplusplus
extern "C"
{
#endif

#define UFTP_FILE_NAME_LEN 	1024 //255
#define UFTP_APP_MODULE_NAME_LEN 16
#define UFTP_APP_TIMEOUT_PERIOD	3
#define UFTP_GPID_LEN	20
#define UFTP_USERID_LEN	31

#define UFTP_INVALID_SID	0
#define UFTP_FILE_BLOCK_SIZE 850

typedef enum EUFTP_Transfer_Role{
    EUFTP_ROLE_UNKNOWN =0 ,
	EUFTP_ROLE_PULLER,
	EUFTP_ROLE_PULLEE,
	EUFTP_ROLE_PUSHER,
	EUFTP_ROLE_PUSHEE
}UFTP_Transfer_Role;

enum EUFTP_APP_STOP_TYPE{
	EUFTP_PAUSE_TRANSFER = 0,			//pause transfer
	EUFTP_RESUME_TRANSFER,				//resume transfer (when the session was paused) 
	EUFTP_TERMINATE_TRANSFER,			//terminate the session, keep breakPoint
	EUFTP_TERMINATE_DEL_BREAK_TRANSFER,	//terminate the session and delete the breakPoint
	EUFTP_TERMINATE_DEL_FILE_TRANSFER,	//terminate the session , delete the breakPoint and the downloaded UTMP file
	EUFTP_SIGN_OUT_APP,
	EUFTP_STREAMING_OVER,			//notification of EUFTP_OBJ_LIVE_FILE loaded over				
	EUFTP_MAX_STOP_TYPE  = 255
};

enum EUFTP_FILE_OPEN_FAILURE_TYPE{
	EUFTP_FILE_OPEN_SUCCESS = 0,
	EUFTP_FILE_NOT_EXIST,
	EUFTP_ACCESS_DENIED				 
};


typedef enum EUFTP_APP_IDENTIFIER_Type{
	EUFTP_APP_UNKNOWN =-1,
	EUFTP_APP_PT,
	EUFTP_APP_MID,
	EUFTP_APP_MODULE_NAME
}UFTP_APP_IDENTIFIER_TYPE;

typedef struct Tuftp_App_Identifier_st{
	UFTP_APP_IDENTIFIER_TYPE  type;
	Sint16   opt;
	Sint16	   sid;
	union{
		Sint32 pt;
		mid_t mid;
		Uint8 moduleName[UFTP_APP_MODULE_NAME_LEN];
	}identifier;
}Tuftp_App_Identifier;

typedef enum EUFTP_App_Cfg_Type{
	EUFTP_APP_CFG_BOTH=0x00,	//both app and uftp cares
	EUFTP_APP_CFG_USER_TIMEOUT,
	EUFTP_APP_CFG_REMOTE_PATH,
	EUFTP_APP_CFG_USERID, //local userID
	EUFTP_APP_CFG_APPSID,
	EUFTP_APP_CFG_GPID,
	EUFTP_APP_CFG_REMOTE_USERID, //Remote userID
	EUFTP_APP_CFG_BOTH_MAX,

	EUFTP_APP_CFG_UFTP=0x100,	//the type and has nothing to do with app.
	EUFTP_APP_CFG_UFTP_MAX,

	EUFTP_APP_CFG_APP=0x200,	//the type and has nothing to do with uftp.
	EUFTP_APP_CFG_APP_MAX
}UFTP_App_Cfg_Type;

typedef struct Tuftp_App_Cfg_Entry_st{
	Uint32	type;
	Uint32	value_len;
	Uint8 buf [0];
}Tuftp_App_Cfg_Entry;

typedef struct Tuftp_FileObj_st{
	Uint64 fileSize;
	Uint64 breakSize;
	Uint32 fileNameLen;
	Sint8 fileName[UFTP_FILE_NAME_LEN];
}Tuftp_FileObj;

typedef struct Tuftp_MemObj_st{
	Uint32 memLen;
	Void* memAddr;
}Tuftp_MemObj;

typedef enum EUFTP_Transfer_Obj_Type{
	EUFTP_OBJ_UNKNOWN =-1,
	EUFTP_OBJ_FILE,
	EUFTP_OBJ_LIVE_FILE,
	EUFTP_OBJ_FILE_FROM_MEM,
	EUFTP_OBJ_MEM
}UFTP_OBJ_TYPE;

typedef struct Tuftp_TransObj_st{
	UFTP_OBJ_TYPE  type;
	union{
		Tuftp_FileObj file;
		Tuftp_MemObj mem;
}obj;
}Tuftp_TransObj;


typedef enum EUFTP_APP_Notification_Type{
	EUFTP_TERMINATE_NORMAL=0,       //normal terminal

	EUFTP_EXCEPTION_FILE_LOAD_FAILURE,         //发送方（pullee/pusher） 文件打开失败
	EUFTP_EXCEPTION_FILE_SAVE_FAILURE,           //接收方（puller/pushee）文件打开失败
	EUFTP_EXCEPTION_FILE_WRITE_CONFLICT,    //文件写冲突，打开文件前判断文件已被UFTP打开读、写
	EUFTP_EXCEPTION_FILE_READ_CONFLICT,     //文件读冲突，打开文件前判断文件已被UFTP打开写
	EUFTP_EXCEPTION_FILE_WRITE_ERROR,       //文件写返回出错，磁盘空间不足、磁盘写错误
	EUFTP_EXCEPTION_FILE_READ_ERROR,        //文件读返回错误，文件损坏
	EUFTP_EXCEPTION_FILE_INTEGRATION_ERROR, //文件验证异常

	EUFTP_EXCEPTION_LOW_DISK_SPACE,         //磁盘空间不足 //对方为Pusher，传送过程中Session保活失败，本地汇报异常
	EUFTP_EXCEPTION_QUEUING,         //对方为Pushee，传送过程中Session保活失败，本地汇报异常
	EUFTP_EXCEPTION_WAIT_LIVE_LOAD_TIMEOUT,         //对方为Puller，传送过程中Session保活失败，本地汇报异常
	EUFTP_EXCEPTION_PULLEE_FAILURE,         //对方为Pushee，传送过程中Session保活失败，本地汇报异常

	EUFTP_EXCEPTION_USER_WATIE_TIMEOUT,
	EUFTP_EXCEPTION_USER_REJECT,
	EUFTP_EXCEPTION_USER_CANCEL,

	EUFTP_EXCEPTION_APP_FAILURE,         //对方应用退出

	EUFTP_EXCEPTION_SIZE_TOO_LARGE,
	EUFTP_EXCEPTION_NAME_TOO_LONG,
	EUFTP_EXCEPTION_TYPE_NOT_SUPPORTED,

	EUFTP_FILE_FINGER_FSIZE_ERROR, //文件指纹或大小不一致
	EUFTP_FILE_PAHT_NO, //保存路径不存在
	EUFTP_FILE_MISS, //保存路径未设置
	EUFTP_FILE_PAHT_DENY, //保存路径不允许
	EUFTP_FILE_CONFIG_ERROR, //保存配置错误
	EUFTP_EXCEPTION_APP_TIMEOUT, //与应用保活超时
	EUFTP_EXCEPTION_PUSHER_TIMEOUT, //pusher超时
	EUFTP_EXCEPTION_PUSHEE_TIMEOUT, //pushee超时
	EUFTP_FILE_GPID_ERROR, //GPID不一致
	
	EUFTP_SESSION_NORMAL,
	EUFTP_UNKNOWN_EXCEPTION, //其他情况
	EUFTP_BREAK_FILE_LOADED,
	EUFTP_LOCAL_TOO_MANY_SESSION,	
	EUFTP_REMOTE_TOO_MANY_SESSION, //远程session数目超过限制

    EUFTP_LOCAL_SESSION_NOT_FOUND,    // local session not found

	EUFTP_EXCEPTION_ID_MAX
}UFTP_APP_Notification_Type;

typedef enum EUFTP_Inter_Msg_Type{
	EUFTP_APP_PUT_REQ,	
	EUFTP_APP_PUT_ACK,
	EUFTP_UFTP_PUT_ACK,
	EUFTP_CANCEL_TRANSFER_IND,
	EUFTP_GET_SESSION_STATUS_REQ,
	EUFTP_GET_SESSION_STATUS_ACK,
	EUFTP_SESSION_NOTIFY_IND,
	EUFTP_GET_BREAK_FILE_REQ, //获取续传文件列表的请求
	EUFTP_GET_BREAK_FILE_ACK, //获取续传文件列表的应答
	EUFTP_DEL_BREAK_FILE_IND, //删除未启动的续传文件的指示
	EUFTP_GET_PASSIVE_BREAK_FILE_REQ, //上传方获取续传文件列表的请求
	EUFTP_GET_PASSIVE_BREAK_FILE_ACK, //上传方获取续传文件列表的应答
	EUFTP_DEL_PASSIVE_BREAK_FILE_IND, //上传方删除未启动续传文件的指示
	EUFTP_SET_SHARE_DIR_IND,
	EUFTP_CHANGE_TRANSFER_ORDER_IND,  
	EUFTP_MSG_MAX = 0x0000ffff
}UFTP_Inter_Msg_Type;


enum {
	UFTP_SESSION_PRIORITY_UNKNOWN = -1,
	UFTP_SESSION_PRIORITY_LOW,
	UFTP_SESSION_PRIORITY_MIDDLE,
	UFTP_SESSION_PRIORITY_HIGH,
	UFTP_SESSION_PRIORITY_INTERRUPT
};

//	EUFTP_PUT_IND,
typedef struct Tuftp_AppPutReqSt
{
	Uint32	app_sid;								// partssid
	Tuftp_TransObj src_obj;
	Tuftp_App_Identifier	dst_app;
	Tpos_addr		dst_addr;
	Uint8	src_gpid[UFTP_GPID_LEN]; //服务端的GPID
	Bool8	keep_alive;
	Bool8	is_active;
	Bool8   is_break;
	Sint8	priority;	//May.5th  new function added
	Bool8	breakNeeded;
	Bool8   bRename;
	Uint16	cfg_num;
	Uint8	cfg_buf[0];
}Tuftp_AppPutReq;

typedef struct Tuftp_AppPutAckSt
{
	Uint32 app_sid;
	Uint32 uftp_sid;
	UFTP_Transfer_Role		role_type;
	UFTP_APP_Notification_Type		error_code;
}Tuftp_AppPutAck;

typedef struct Tuftp_UftpPutReqSt
{
	Uint32 uftp_sid;
	Uint32 peer_uftp_sid;
	Tuftp_TransObj src_obj;
	Tpos_addr	src_addr;
	UFTP_Transfer_Role role_type;
	Uint32 cfg_num;
	Bool8 isBreak;
	Bool8 isQueued;
	POS_FILLER2;
	Uint8	cfg_buf[0];
}Tuftp_UftpPutReq;

typedef struct Tuftp_UftpPutAckSt
{
	Uint32 uftp_sid;
	Tuftp_TransObj dst_obj;
	UFTP_APP_Notification_Type error_code;
	Sint8 userID[UFTP_USERID_LEN];
	Bool8 accept;
	Bool8 keep_alive;
	Bool8 cont_support;
	POS_FILLER1;
}Tuftp_UftpPutAck;

//	EUFTP_GET_SESSION_STATUS_REQ,
typedef struct Tuftp_GetSessionStatusReqSt
{
	Uint32 uftp_sid;						
	UFTP_Transfer_Role role_type;
	Bool8 status_needed;
	POS_FILLER3;
}Tuftp_GetSessionStatusReq;

//	EUFTP_GET_SESSION_STATUS_CNF,
typedef struct Tuftp_GetSessionStatusCnfSt
{
	Uint32 uftp_sid;						
	Sint32 transfer_progress;			// <0 means session is not found
	Sint32 transfer_rate;
}Tuftp_GetSessionStatusCnf;

//	EUFTP_CANCEL_TRANSFER_IND,
typedef struct Tuftp_CancelTransferIndSt
{
	Uint32 uftp_sid;	
	Uint32	uftp_cancel_type;
	UFTP_Transfer_Role role_type;
}Tuftp_CancelTransferInd;

//	EUFTP_SESSION_NOTIFY_IND,
typedef struct Tuftp_SessionNotifyIndSt
{
	Uint32 uftp_sid;						// part ssid
	UFTP_APP_Notification_Type session_status;
	Bool8 notifyPeerApp;
	Uint16	cancelType;
}Tuftp_SessionNotifyInd;

typedef struct Tuftp_App_Info_st{
	Sint8 userID[UFTP_USERID_LEN];
	Uint8 remoteGPID[UFTP_GPID_LEN];
	Sint8 remoteUserID[UFTP_USERID_LEN];
}Tuftp_App_Info;

typedef struct Tuftp_Break_File_st{
	Uint64 fileSize;
	Uint64 breakSize;	
	Uint32 uftpSid;
	Uint32 isUserCanceled;
	Uint8 remoteGPID[UFTP_GPID_LEN];
	Sint8 userID[UFTP_USERID_LEN];
	Sint8 remoteUserID[UFTP_USERID_LEN];
	Sint8 localFileName[UFTP_FILE_NAME_LEN];
	Sint8 remoteFileName[UFTP_FILE_NAME_LEN];
}Tuftp_Break_File;

typedef struct Tuftp_Break_File_List_st{
	Uint32 breakNum;
	Uint8 remoteGPID[UFTP_GPID_LEN]; //the pusher's gpid, the same as the gpid in Tuftp_Break_File
	Tuftp_Break_File break_file_info;
}Tuftp_Break_File_List;

typedef struct Tuftp_Del_Break_File_st{
	Bool8 removeTmpFile;
	POS_FILLER3;
	Tuftp_Break_File break_file_info;
}Tuftp_Del_Break_File;

typedef struct Tuftp_Share_Dir_st{
	Sint8 sharedDir[MAX_PATH];
}Tuftp_Share_Dir;

typedef struct Tuftp_SessionEntrySt{
	Uint32 uftp_sid;
	UFTP_Transfer_Role role_type;
} Tuftp_SessionEntry;

typedef struct Tuftp_ChangeTransferOrderSt
{
	Tuftp_SessionEntry		nextSession;       
	Uint32					updateSessionNum;
	Tuftp_SessionEntry		updateSessionArray[0];
}Tuftp_ChangerTransferOrderInd;

#define UFTP_QUEUED_SESSION_FILE_VERSION		1
#define UFTP_PUSHER_SESSION_FILE_VERSION        1

#define UFTP_QUEUED_SESSION_FILE_NAME		"QueuedSession.lst"
#define UFTP_PUSHER_SESSION_FILE_NAME		"PusherSession.lst"
#define UFTP_PUSHER_SESSION_FILE_NAME_NEW		"PusherSession.lst.new"
#define UFTP_PUSHER_SESSION_FILE_NAME_BAK		"PusherSession.lst.bak"


Sint32 uftp_init(Sint8 * INIpath);
Sint32 uftp_uninit();
Sint32 PushFile(Tpos_addr* dstAddr,Sint8* localFileName);
Sint32 PushFileByGPID(Sint8 *remoteGPID, Sint8 *localFileName);
Sint32 PushFileByIP(Sint8 *remoteIP, Sint8 *remotePort, Sint8 *localFileName);
Sint32 PullFile(Tpos_addr* dstAddr,Sint8* remoteFileName,Sint8* localFileName);
Sint32 PullFileByGPID(Sint8* GPID, Sint8* remoteFileName, Sint8* localFileName);
Sint32 PullFileByIP(Sint8* IP, Sint8* Port, Sint8* remoteFileName, Sint8* localFileName);
Sint32 GetFileByGPID(Sint8* RemoteGPID, Sint8* remoteFileName, Sint8* localFileName);
Sint32 GetFileByIP(Sint8* remoteIP, Sint8* remotePort, Sint8* remoteFileName, Sint8* localFileName);
Sint32 GetPusherSessionInfo(Sint8* FileName, Uint32* TotalSize, Sint32* Percent, Sint32* Speed, Sint32* SessionID);
Sint32 GetPusheeSessionInfo(Sint8* FileName, Uint32* TotalSize, Sint32* Percent, Sint32* Speed, Sint32* SessionID);
Sint32 ChangeSessionStatus(Uint32 sessionID, Sint32 responseRole, Sint32 errorCode, Uint32 optValue);

#ifdef __cplusplus
}
#endif

#endif //_UFTP_PUB_H_
