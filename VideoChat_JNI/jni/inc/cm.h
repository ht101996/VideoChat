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
 * File Name:       CM.h
 *
 * Reference:
 *
 * Author:          genliu, Kevin Hua
 *
 * Description:
 *      Connect Manager (CM) interface file.
 *
 * History:
 *      please record the history in the following format:
 *      verx.y.z    date        author      description or bug/cr number
 *      ----------------------------------------------------------------
 *      ver0.0.1    09/10/2008  genliu     first draft
 *      ver1.0.0    10/06/2008  Kevin Hua  udpate     
 *  
 *CodeReview Log:
 *      please record the code review log in the following format:
 *      verx.y.z    date        author      description
 *      ----------------------------------------------------------------
 * 
 */


#ifndef _CM_H_
#define _CM_H_


#if defined (__WIN32__)
#pragma warning (disable : 4200)
#endif

#include "pos_prolog.h"
#include "pos_type.h"                   // platform type head
#include "pos_addr.h"





#ifdef POS_DLL  // create or using dll library
#   ifdef CM_EXPORTS   // dll support
#       define CM_API  POS_EXPORTS_DECLSPEC
#   else    // dll using
#       define CM_API  POS_IMPORT_DECLSPEC
#   endif
#else   // create or using static library
#   define CM_API
#endif


// debug cms mutex lock
#define CM_MUTEX_DEBUG_SWITCH	0


#ifdef __cplusplus
extern "C"
{
#endif

/*
 * connect manager callback type
 */
typedef enum Ecm_event_type
{
    CM_EVENT_REG_SUC = 0,               // complete register to server
    CM_EVENT_REG_FLD,                   // failed register to server
    CM_EVENT_DATA,                      // received data
    CM_EVENT_DATA_ACROSS,               // across data, no security process
    CM_EVENT_ERROR                      // error find
} ECM_EVENT_TYPE;

/*
 * connect manager error code
 */
typedef enum Ecm_error_code
{
    CM_ERROR = -500,                    // unknown error
    CM_ERROR_REG_PARAMETER,             // error register parameters
    CM_ERROR_REG_TIMEOUT,               // register timeout
    CM_ERROR_NOT_INIT,                  // not init
    CM_ERROR_LOSS_SERVER,               // ping pong timeout, or connect manager is shutdown
    CM_ERROR_SEND_DATA,                 // failed send data
    CM_ERROR_DECRYPT					// error decrypt data
} ECM_ERROR_CODE;


/*
 * register complete,
 */
typedef struct Tcm_EventRegSuc_st
{
    Sint32 m_iRegisterID;               // the number of register ID in connect manager client
}Tcm_EventRegSuc;

/*
 * register failed,
 */
typedef struct Tcm_EventRegFld_st
{
    ECM_ERROR_CODE m_Reason;            // reason of fail register
}Tcm_EventRegFld;

/*
 * receiving data form network
 */
typedef struct Tcm_EventData_st
{
    Sint8 *m_pBuf;              // received data buffer
    Sint32 *m_pLen;             // received data length

    Tpos_addr *m_pSrcAddr;      // source address
    Tpos_addr *m_pDstAddr;      // destination address
}Tcm_EventData;

/*
 * async error event
 */
typedef struct Tcm_EventError_st
{
    ECM_ERROR_CODE m_Reason;    // reason of fail register
    Tpos_addr *m_pSrcAddr;      // source address
    Tpos_addr *m_pDstAddr;      // destination address
}Tcm_EventError;

typedef enum Ecm_Event_Result
{
    CM_EVENT_RESULT_ERR = -1,           // error
    CM_EVENT_RESULT_SUC,                // success
    CM_EVENT_RESULT_REMAIN_MEMORY,      // do not delete memory, for p2p now
    
} ECM_EVENT_RESULT;
/*
 * callback function
 */
typedef ECM_EVENT_RESULT (* CM_EVENT)(ECM_EVENT_TYPE,     // event type of callback
                                      Sint8 *,            // data pointer
                                      Sint32 *);          // data length pointer

/*
 * priority of module
 */
typedef enum Ecm_priority
{
    CM_PRIORITY_NORMAL = 0,             // normal
    CM_PRIORITY_LOW,                    // low
    CM_PRIORITY_HIGH,                   // high
    CM_PRIORITY_UNKNOWN                  // unknown
} ECM_PRIORITY;

/*
 * register type
 */
typedef enum Ecm_Register_Type
{
    CM_REGISTER_TYPE_SOCKET = 0,        // register to server with internal socket
    CM_REGISTER_TYPE_MSGC,              // register to server with message center
    CM_REGISTER_TYPE_LIB,               // register to server with lib's interface
	CM_REGISTER_TYPE_IPC				// register to server with IPC
} ECM_REGISTERTYPE;

/*
 * protocol sub type info
 */
typedef struct Tcm_ProtocolSubID_st
{
    Uint32 m_uiID;                      // sub type id
    Uint32 m_uiSize;                    // sub type length
}Tcm_ProtocolSubID;

/*
 * protocol info, such as RTP, P2P and LUP
 */
typedef struct Tcm_ProtocolInfo_st
{
    Uint32 m_uiProtocolID;              // protocol id
    ECM_PRIORITY m_Priority;            // priority
    Uint32 m_uiSubCount;                // number of sub type
    Uint32 m_uiIsDefault;               // whether receive undecryptable data, 1 - yes, 0 - no
    Tcm_ProtocolSubID *m_pSubID;        // array of sub type
} Tcm_ProtocolInfo;

/*
 * module info, such as APP, P2P and so on
 */
typedef struct Tcm_ModuleInfo_st
{
    Sint8 m_strName[32];                // module name
	ECM_REGISTERTYPE m_registerType;	// register type
    CM_EVENT m_pEvent;                  // callback function
    Uint32 m_uiProtocolCount;           // number of protocol in this module
    Tcm_ProtocolInfo *m_pProtocolInfo;  // array of protocol
    Tpos_addr m_DstAddr;                // remote equipment address, RESERVE
} Tcm_ModuleInfo;

/*
#define CM_MAX_PT_VALUE     15          // the maximum of protocol value
#define CM_MAX_CT_VALUE     31          // the maximum of sub type value

// common datagram's header
typedef struct Tcm_dg_hdr_st
{
    Uint32 v:3;                   // version
    Uint32 eh:1;                  // 0 - it is not external head, 1 - it is a external head
    Uint32 pt:4;                  // datagram protocol type, such RCP/RPP, LUP, DAS, CC and so on
    Uint32 aut:8;                 // head authentication code
    Uint32 ct:5;                  // subtype of protocol
    Uint32 len:11;                // all this datagram's length, include head and body
}Tcm_dg_hdr;

#define CM_SC_REQ   0X1F
#define CM_SC_CNF   0X1E
*/

// 2011-06-07, lchen, change extend Tcm_dg_hdr
#define CM_MAX_PT_VALUE	127
#define CM_MAX_CT_VALUE	253

typedef struct Tcm_dg_hdr_st 
{
	Uint32 v	:4;
	Uint32 eh	:1;
	Uint32 pri	:3;				//encrypt type    0:not encrypt, 1 aes-128 2 aes-256 only use cmc module
	Uint32 aut	:8;
	Uint32 pt	:7;
	Uint32 cc	:1;				// 0-application message, 1-cmc message
	Uint32 ct	:8;
	Uint32 len	:11;
	Uint32 reversed1	: 5;	
	Uint32 exLen: 11;
	Uint32 reversed2	: 5;
} Tcm_dg_hdr;


// 2011-10-09, lchen, add extern pt, since original pt is defined in msgc_protocol.h
typedef enum ECF_PT_EXTERN_enum
{
	ECF_PT_SAMPLE	= 17,
	ECF_PT_TTD		= 18,
	ECF_PT_MT		= 19,
	ECF_PT_NATSERVER =20,
} ECF_PT_EXTERN;


#define CM_SC_REQ	0XFF
#define CM_SC_CNF	0XFE


/*
 * CM internal datagram's header for local server to peer server exchange
 * public for p2p, ipcache and statistical server using
 */
typedef struct Tcm_ss_hdr_st
{
    Tcm_dg_hdr m_Hdr;           // common header
    Uint32 seq:11;        // datagram sequence number for UDP simulate TCP
    Uint32 security:3;    // the higest security type of create equipment, such as SECURITY_CHIP_AES
    Uint32 encrypt:3;     // encrypt type of this datagram, such as SECURITY_CHIP_AES, defineded in asecurity.h
    Uint32 :6;
    Uint32 test_pt:4;
    Uint32 test_ct:5;
    Sint8 aut_s[4];
    Sint8 aut_c[4];
	Uint8 gpid[POS_UUID_LEN];
}Tcm_ss_hdr;

/*
 * session capability exchange datagram
 */
typedef struct Tcm_SC_st
{
    Tcm_dg_hdr m_Head;                  // datagram head
    Uint32 m_uiSessionID;               // source session id
    Uint32 m_uiSrcIP;                   // source ip
    Uint16 m_usSrcPort;                 // source port
    Sint16 m_sSrcChipInfoLen;           // source chip info length
    Sint8 m_szSrcChipInfo[0];           // source chip info data
}Tcm_SC;

/************************************************************************************************
 *                                                                                              *
 *                  CONNECT MANAGER NET RESOURCE  INTERFACE                                     *
 *                                                                                              *
 ************************************************************************************************/
typedef enum
{
    ECM_INTERFACE_TYPE_LAN = 0,             // eth1(LAN) in F200 or the first network card in PC side
    ECM_INTERFACE_TYPE_WAN,                 // eth0(WAN) in F200 or the second network card in PC side
    ECM_INTERFACE_TYPE_EXTERNAL,            // external interface both in F200 and PC.
    ECM_INTERFACE_TYPE_PPPOE0,              // interface pppoe0. 
    ECM_INTERFACE_TYPE_PPPOE1,              // interface pppoe1.
    ECM_INTERFACE_TYPE_ACCESS,              // access address.
    ECM_INTERFACE_TYPE_ALL,                 // all interfaces both in F200 and PC
}ECM_INTERFACE_TYPE;

typedef enum
{   
    ECM_PROTOCOL_UDP = 0,
    ECM_PROTOCOL_TCP,
    ECM_PROTOCOL_MUC,

	ECM_PROTOCOL_AUTO
}ECM_PROTOCOL_TYPE;

enum ECM_NET_MSG_TYPE
{
    ECM_NET_IP_REQ = 128,                   // quary IP request             
    ECM_NET_IP_CNF,                         // confirm for IP request
    
    ECM_NET_INTERFACE_REQ,                  // quary whole interface's information request
    ECM_NET_INTERFACE_CNF,                  // confirm for interface's information request
    
    ECM_NET_SERVER_PORT_REQ,                // quary the server port.
    ECM_NET_SERVER_PORT_CNF,                // confirm for server port quary request
    
    ECM_NET_PORT_APPLY_REQ,                 // apply some port request,using UPnP service
    ECM_NET_PORT_APPLY_CNF,                 // confirm for port apply request,using UPnP service
    
    ECM_NET_UUID_REQ,                       // quary device's uuid request
    ECM_NET_UUID_CNF,                       // confirm for uuid request
	ECM_NET_GPID_IND,						// indication when generate new gpid

    ECM_NET_VIRTUAL_ADDRESS_REQ,            // request for local virtual address 
    ECM_NET_VIRTUAL_ADDRESS_CNF,            // confirm for local virtual address
    
    ECM_NET_MAC_REQ,                        // quary MAC address
    ECM_NET_MAC_CNF,                        // confirm for MAC address    
    
    ECM_NET_PORT_RELEASE_IND,               // release one port by application    
    ECM_NET_INTERFACE_IND,                  // indication when interface's information change.
    ECM_NET_VIRTUAL_ADDRESS_IND,            // indication when virtual address change.
    ECM_NET_SERVER_PORT_IND,                // indication when server port change.
    ECM_NET_RESOURCE_PUBLISH_IND,           // publish resources when CMS starts.

    ECM_NET_HIGH_ID_REQ,                    // query(normally or forcibly) whether external ip is high id or not.
    ECM_NET_HIGH_ID_CNF,                    // confirm of high id request.

    ECM_NET_CALLFREEE_SERVER_ADDR_REQ,      // query callfreee server's address
    ECM_NET_CALLFREEE_SERVER_ADDR_CNF,      // confirm of request
    ECM_NET_CALLFREEE_SERVER_ADDR_IND,      // indication of callfreee server's the new address value

    ECM_NET_PORT_KEEPALIVE_FAILED_IND,

    ECM_NET_UPNP_DEVICE_STATUS_REQ,         // query current upnp device's status
    ECM_NET_UPNP_DEVICE_STATUS_CNF,         // confirm

    ECM_NET_SERVER_PORT_ADD_REQ ,           // add one server port
    ECM_NET_SERVER_PORT_ADD_CNF,            // confirm
    
    ECM_NET_SERVER_PORT_QUERY_REQ,          // query all server ports
    ECM_NET_SERVER_PORT_QUERY_CNF,          // confirm

    ECM_NET_SERVER_PORT_DELETE_REQ,         // delete one server port
    ECM_NET_SERVER_PORT_DELETE_CNF,         // confirm

    ECM_NET_SERVER_PORT_UPDATE_REQ,         // update one server port 
    ECM_NET_SERVER_PORT_UPDATE_CNF,         // confirm    
    
    ECM_NET_ADD_DECRYPT_ADDRESS_REQ,        // add an address to cm and cm will use it to decrypt data.
    ECM_NET_DELETE_DECRYPT_ADDRESS_REQ,     // delete an encrypt address to cm.

    ECM_NET_SET_UPNP_REQ,                   // set upnp service status
    ECM_NET_SET_UPNP_CNF,                   // confirm

    ECM_NET_GET_UPNP_REQ,                   // get upnp service status
    ECM_NET_GET_UPNP_CNF,                   // confirm

    ECM_NET_UPNP_DEVICE_TEST_REQ,           // test that whether there has available UPnP device
    ECM_NET_UPNP_DEVICE_TEST_CNF,           // confirm, no device or device is available

	ECM_NET_NAT_DIRECT_QUERY_REQ,			// query whether the src and dst are direct connected.
	ECM_NET_NAT_DIRECT_QUERY_CNF,			// confirm

	ECM_NET_TRANS_PROTOCOL_QUERY_REQ,		// query the protocol of transport layer
	ECM_NET_TRANS_PROTOCOL_QUERY_CNF,		// confirm

	ECM_NET_TCP_TURN_ROLE_QUERY_REQ,		// query the role of TCP Turn in config file
	ECM_NET_TCP_TURN_ROLE_QUERY_CNF,		// confirm

	ECM_NET_TCP_SERVER_PORT_REQ,			// query TCP Listening Port
	ECM_NET_TCP_SERVER_PORT_CNF,			// confirm

	ECM_NET_LAN_GPID_QUERY_REQ,				// query all GPID in LAN
	ECM_NET_LAN_GPID_QUERY_CNF				// confirm
};


// ECM_NET_IP_REQ
typedef struct Tcm_NetIPReqSt
{
    Uint32 m_uiSSID;                        // session id
    ECM_INTERFACE_TYPE m_InterfaceType;     // type value in enum struct ECM_INTERFACE_TYPE
}Tcm_NetIPReq;

// ECM_NET_IP_CNF 
typedef struct Tcm_NetIPCnfSt
{
    Uint32 m_uiSSID;                        // session id
    Sint32 m_iInterfaceCount;               // interface count
    Sint32 m_iResult;                       // 0 for success,refer to enum ECM_ERROR_CODE in cm.h
    struct
    {
        ECM_INTERFACE_TYPE m_Type;          // type of interface, refer to enum struct ECM_INTERFACE_TYPE
        Sint32 m_iIPCount;                  // count of ip in this network card    
        Uint32 m_uiIP[16];                  // IP array, network byte order
    }m_Interface[0];
}Tcm_NetIPCnf;

// ECM_NET_INTERFACE_REQ
typedef struct Tcm_NetInterfaceReqSt
{
    Uint32 m_uiSSID;                        // session id
    ECM_INTERFACE_TYPE m_InterfaceType;     // type value in enum struct ECM_INTERFACE_TYPE
}Tcm_NetInterfaceReq;

// ECM_NET_INTERFACE_CNF 
typedef struct Tcm_NetInterfaceCnfSt
{
    Uint32 m_uiSSID;                        // session id
    Sint32 m_iInterfaceCount;               // interface count
    Sint32 m_iResult;                       // 0 for success,or refer to enum ECM_ERROR_CODE in cm.h
    struct
    {
        ECM_INTERFACE_TYPE m_Type;          // type of interface, refer to enum struct ECM_INTERFACE_TYPE
        Sint32 m_iIfCount;                  // count of interface in this network card    
        Tpos_ifi_info m_Info[16];           // interface array
    }m_Interface[0];
}Tcm_NetInterfaceCnf;

// ECM_NET_SERVER_PORT_REQ
typedef struct Tcm_NetServerPortReqSt
{
    Uint32 m_uiSSID;                        // session id
}Tcm_NetServerPortReq;

// ECM_NET_SERVER_PORT_CNF
typedef struct Tcm_NetServerPortCnfSt
{
    Uint32 m_uiSSID;                        // session id 
    Tpos_addr m_ServerPort;                 // server port. network byte order
}Tcm_NetServerPortCnf;

// ECM_NET_UUID_REQ
typedef struct Tcm_NetUUIDReqSt
{
    Uint32 m_uiSSID;                        // session id
}Tcm_NetUUIDReq;

//  ECM_NET_UUID_CNF
typedef struct Tcm_NetUUIDCnfSt
{
    Uint32 m_uiSSID;                        // session id
    Uint8 m_ucUUID[POS_UUID_LEN];                     // uuid for this device
}Tcm_NetUUIDCnf;


//ECM_NET_GPID_IND,		
typedef struct Tcm_net_gpid_ind_st{
	Sint32 result;   //'1' for success, '0' for failed 
	Uint8 old_gpid[POS_UUID_LEN];
	Uint8	new_gpid[POS_UUID_LEN];
}Tcm_net_gpid_ind;

// ECM_NET_VIRTUAL_ADDRESS_REQ
typedef struct Tcm_NetVirtualAddressReqSt
{
    Uint32 m_uiSSID;                        // session id
}Tcm_NetVirtualAddressReq;

// ECM_NET_VIRTUAL_ADDRESS_CNF
typedef struct Tcm_NetVirtualAddressCnfSt
{
    Uint32 m_uiSSID;                        // session id
    Sint32 m_iResult;                       // 0 for success,or refer to enum ECM_ERROR_CODE.
    Tpos_addr m_VirtualAddress;             // local virtual address
}Tcm_NetVirtualAddressCnf;

// ECM_NET_MAC_REQ
typedef struct Tcm_NetMACReqSt
{
    Uint32 m_uiSSID;                        // session id
    ECM_INTERFACE_TYPE m_InterfaceType;     // interface type
}Tcm_NetMACReq;

// ECM_NET_MAC_CNF
typedef struct Tcm_NetMACCnfSt
{
    Uint32 m_uiSSID;                        // session id
    Sint32 m_iResult;                       // 0 for success,or refer to enum ECM_ERROR_CODE.
    ECM_INTERFACE_TYPE m_InterfaceType;     // interface type
    Uint8 m_ucMAC[6];                       // MAC address, 6 bytes.
    POS_FILLER2;
}Tcm_NetMACCnf;

// ECM_NET_INTERFACE_IND 
typedef struct Tcm_NetInterfaceIndSt
{
    ECM_INTERFACE_TYPE m_InterfaceType;     // interface type
    Sint32 m_iInterfaceInfoCount;           // interface information count
    struct 
    {
        Tpos_ifi_info m_OldIf;              // old interface information
        Tpos_ifi_info m_NewIf;              // new interface information
        Sint32 m_iHighIDStatus;             // high id status, 1: high; 0: low; -1: unknown
    }m_Interface[0];
}Tcm_NetInterfaceInd;

// ECM_NET_VIRTUAL_ADDRESS_IND
typedef struct Tcm_NetVirtualAddressIndSt
{
    Tpos_addr m_VirtualAddress;             // new local virtual address
}Tcm_NetVirtualAddressInd;

// ECM_NET_SERVER_PORT_IND
typedef struct Tcm_NetServerPortIndSt
{
    Tpos_addr m_OldServerPort;              // old server port. network byte order    
    Tpos_addr m_NewServerPort;              // new server port. network byte order    
}Tcm_NetServerPortInd;

// ECM_NET_RESOURCE_PUBLISH_IND
typedef struct Tcm_NetResourcePublishIndSt
{
    Tpos_addr m_ServerPort;                 // old server port. network byte order   
    Tpos_addr m_VirtualAddress;             // virtual address.
    Uint8 m_ucUUID[POS_UUID_LEN];
    Sint32 m_iInterfaceInfoCount;           // interface count
    struct
    {
        ECM_INTERFACE_TYPE m_InterfaceType; // interface type
        Uint8 m_ucMAC[6];
        POS_FILLER2;
        Sint32 m_iIfCount;                  // ip count
        Uint32 m_uiIP[16];                  // ip address array
    }m_Interface[0];
}Tcm_NetResourcePublishInd;


typedef enum
{
    ECM_HIGHID_QUERY_NORMALLY = 0,          // normal query,
    ECM_HIGHID_QUERY_FORCIBLY,              // force cm to query the address's high id status
}ECM_HIGHID_QUERY_TYPE;


//ECM_NET_HIGH_ID_REQ,
typedef struct Tcm_NetHighIDReqSt
{
    Uint32 m_uiSSID;                        // session id
    ECM_HIGHID_QUERY_TYPE m_Type;           // high id query type, normal or forcible
    Uint32 m_uiMaxTime;                     // max expire time, 1000 = 1s
}Tcm_NetHighIDReq;

//ECM_NET_HIGH_ID_CNF,
typedef struct Tcm_NetHighIDCnfSt
{
    Uint32 m_uiSSID;                        // session id
    ECM_HIGHID_QUERY_TYPE m_Type;           // high id query type, equal to the value in request.
    Sint32 m_iResult;                       // high id query result, 1: high id, 0:low id, -1: unknown.
}Tcm_NetHighIDCnf;

// ECM_NET_CALLFREEE_SERVER_ADDR_REQ      
typedef struct Tcm_NetCallfreeeServerAddrReqSt
{
    Uint32 m_uiSSID;                            // session id
    Uint8 m_ucServerName[64];                   // server name, refer to its default name.
}Tcm_NetCallfreeeServerAddrReq;

// ECM_NET_CALLFREEE_SERVER_ADDR_CNF  
typedef struct Tcm_NetCallfreeeServerAddrCnfSt
{
    Uint32 m_uiSSID;                            // session id, same to the value in request
    Uint8 m_ucServerName[64];                   // server name, same to the value in request
    Sint32 m_iResult;                           // -1: no address; -2: wrong server name; >0: num of server address
	Tpos_addr m_ServerAddress[0];               // server address
}Tcm_NetCallfreeeServerAddrCnf;

// ECM_NET_CALLFREEE_SERVER_ADDR_IND      
typedef struct Tcm_NetCallfreeeServerAddrIndSt
{
    Uint8 m_ucServerName[64];                   // server name
    Sint32 m_iResult;                             // >0: new address, num of address; -1: address is missing
    Tpos_addr m_ServerAddress[0];                  // server address.
}Tcm_NetCallfreeeServerAddrInd;

/*
*   provide UPnP service to application,like port mapping service
*/

// port apply type
typedef enum
{
    ECM_PORT_APPLY_ANY = 0,             //  assigned by CM
    ECM_PORT_APPLY_PREFER,              // a prefer port, and when failed CM could change assign other port 
    ECM_PORT_APPLY_SPECIFY,             // a specify port, CM can't change it.
}ECM_PORT_APPLY_TYPE;

// ECM_NET_PORT_APPLY_REQ
typedef struct Tcm_NetPortApplyReqSt
{
    Uint32 m_uiSSID;                        // session id
    Tpos_addr m_PortApply;                  // expect port to apply UPNP, network byte order
    ECM_PORT_APPLY_TYPE m_ApplyType;        // apply type, refer to ECM_PORT_APPLY_TYPE
    ECM_PROTOCOL_TYPE m_ProtocolType;       //  protocol type, refer to ECM_PROTOCOL_TYPE
}Tcm_NetPortApplyReq;

//  ECM_NET_PORT_APPLY_CNF
typedef struct Tcm_NetPortApplyCnfSt
{
    Uint32 m_uiSSID;                        // session id
    Sint32 m_iResult;                       // 0 for success,or refer to enum ECM_ERROR_CODE in cm.h.
    Tpos_addr m_PortApply;                  // expect port to apply UPNP, network byte order
    ECM_PORT_APPLY_TYPE m_ApplyType;        // apply type, refer to Tcm_NetPortApplyReq
    ECM_PROTOCOL_TYPE m_ProtocolType;       //  protocol type, refer to Tcm_NetPortApplyReq
}Tcm_NetPortApplyCnf;

// ECM_NET_PORT_RELEASE_IND
typedef struct Tcm_NetPortReleaseIndSt    
{                                           // this message send to CMS from application
    Tpos_addr m_PortRelease;                // release port value by application, network byte order
    ECM_PROTOCOL_TYPE m_ProtocolType;       //  protocol type, refer to ECM_PROTOCOL_TYPE
}Tcm_NetPortReleaseInd;

// ECM_NET_PORT_KEEPALIVE_FAILED_IND      

typedef struct Tcm_NetPortKeepaliveFailedIndSt
{
    Tpos_addr m_Port;                           // port value, network byte order.
    ECM_PROTOCOL_TYPE m_ProtocolType;           // protocol type
    Sint32 m_iCause;                            // -1: keepalive failed.
}Tcm_NetPortKeepaliveFailedInd;

//    ECM_NET_UPNP_DEVICE_STATUS_REQ,                     
typedef struct Tcm_NetUPnPDeviceStatusReqSt
{
    Uint32 m_uiSSID;                            // session id
}Tcm_NetUPnPDeviceStatusReq;

//    ECM_NET_UPNP_DEVICE_STATUS_CNF             
typedef struct Tcm_NetUPnPDeviceStatusCnfSt
{
    Uint32 m_uiSSID;                            // session id
    Sint32 m_iResult;                           // 0: no device; 1: unavailable; 2: available
}Tcm_NetUPnPDeviceStatusCnf;



/************************************************************************************************/
/*
    m_SrvAddr: server address, the address that cms should create socket with it.
    m_iProtocol: protocol type, refer to ECM_PROTOCOL_TYPE. 
                ECM_PROTOCOL_UDP: port: listen port, couldn't be 0; ip: bind ip, 0 means INADDR_ANY.
                ECM_PROTOCOL_TCP: port: listen port, couldn't be 0; ip: bind ip, 0 means INADDR_ANY.
                ECM_PROTOCOL_MUC: port: listen port, couldn't be 0; ip: multicast ip, couldn't be 0.
*/
/************************************************************************************************/

// ECM_NET_SERVER_PORT_ADD_REQ ,            
typedef struct
{
    Uint32 m_uiSSID;                            // session id
    Tpos_addr m_SrvAddr;                        // server address 
    ECM_PROTOCOL_TYPE m_Protocol;               // refer to ECM_PROTOCOL_TYPE.
}Tcm_NetServerPortAddReq;

// ECM_NET_SERVER_PORT_ADD_CNF,                  
typedef struct
{
    Uint32 m_uiSSID;                            // session id, same to request
    Sint32 m_iResult;                           // 0: success.
                                                // -1: exist port; 
                                                // -2: invalid parameter;
                                                // -3: exceed max count
}Tcm_NetServerPortAddCnf;  

// ECM_NET_SERVER_PORT_QUERY_REQ,                
typedef struct
{
    Uint32 m_uiSSID;                            // session id
    ECM_PROTOCOL_TYPE m_Protocol;               // refer to ECM_PROTOCOL_TYPE.
}Tcm_NetServerPortQueryReq;

// ECM_NET_SERVER_PORT_QUERY_CNF,                
typedef struct
{
    Uint32 m_uiSSID;                            // session id, same to request
    Sint32 m_iCount;                            // count of address
    ECM_PROTOCOL_TYPE m_Protocol;               // refer to ECM_PROTOCOL_TYPE.
    Tpos_addr m_SrvAddr[0];                     // server address         
}Tcm_NetServerPortQueryCnf;

// ECM_NET_SERVER_PORT_DELETE_REQ,               
typedef struct
{
    Uint32 m_uiSSID;                            // session id
    Tpos_addr m_SrvAddr;                        // server address 
    ECM_PROTOCOL_TYPE m_Protocol;               // refer to ECM_PROTOCOL_TYPE.
}Tcm_NetServerPortDeleteReq;

// ECM_NET_SERVER_PORT_DELETE_CNF,               
typedef struct
{
    Uint32 m_uiSSID;                            // session id, same to request
    Sint32 m_iResult;                           // 0: success. -1: not exist; -2: invalid parameter
}Tcm_NetServerPortDeleteCnf;

// ECM_NET_SERVER_PORT_UPDATE_REQ,               
typedef struct
{
    Uint32 m_uiSSID;                            // session id
    Tpos_addr m_OldSrvAddr;                     // old server address 
    Tpos_addr m_NewSrvAddr;                     // new server address
    ECM_PROTOCOL_TYPE m_Protocol;               // refer to ECM_PROTOCOL_TYPE.
}Tcm_NetServerPortUpdateReq;

// ECM_NET_SERVER_PORT_UPDATE_CNF,               
typedef struct
{
    Uint32 m_uiSSID;                            // session id, same to request
    Sint32 m_iResult;                           // 0: success. 
                                                // -1: not exist; 
                                                // -2: invalid parameter
                                                // -3: new port exist
}Tcm_NetServerPortUpdateCnf;

//    ECM_NET_ADD_DECRYPT_ADDRESS_REQ,        
typedef struct Tcm_NetAddDecryptAddressReqSt
{
    Tpos_addr m_Address;                        // add an address to cm
}Tcm_NetAddDecryptAddressReq;

//    ECM_NET_DELETE_DECRYPT_ADDRESS_REQ,     
typedef struct Tcm_NetDeleteDecryptAddressReqSt
{
    Tpos_addr m_Address;                        // the ip and port are same to the value in request
}Tcm_NetDeleteDecryptAddressReq;

typedef enum
{
    ECM_UPNP_SERVICE_DISABLE = 0,
    ECM_UPNP_SERVICE_ENABLE,
}EUPNP_SERVICE_STATUS;

//    ECM_NET_SET_UPNP_REQ,               
typedef struct Tcm_NetSetUPnPReqSt
{
    Uint32 m_uiSSID;                            // session id
    EUPNP_SERVICE_STATUS m_ServiceStatus;       // new service status
}Tcm_NetSetUPnPReq;

//    ECM_NET_SET_UPNP_CNF,               
typedef struct Tcm_NetSetUPnPCnfSt
{
    Uint32 m_uiSSID;                            // session id, same to the value in request
    Sint32 m_iResult;                           // 0: success; -1: wrong service status in request
}Tcm_NetSetUPnPCnf;

//    ECM_NET_GET_UPNP_REQ,               
typedef struct Tcm_NetGetUPnPReqSt
{
    Uint32 m_uiSSID;                            // session id
}Tcm_NetGetUPnPReq;

//    ECM_NET_GET_UPNP_CNF,               
typedef struct Tcm_NetGetUPnPCnfSt
{
    Uint32 m_uiSSID;                            // session id, same to the value in request
    EUPNP_SERVICE_STATUS m_ServiceStatus;       // current service status
}Tcm_NetGetUPnPCnf;


//    ECM_NET_UPNP_DEVICE_TEST_REQ,                     
typedef struct Tcm_NetUPnPDeviceTestReqSt
{
    Uint32 m_uiSSID;                            // session id
}Tcm_NetUPnPDeviceTestReq;


//    ECM_NET_UPNP_DEVICE_TEST_CNF             
typedef struct Tcm_NetUPnPDeviceTestCnfSt
{
    Uint32 m_uiSSID;                            // session id
    Sint32 m_iResult;                           // 0: no device; 1: available
}Tcm_NetUPnPDeviceTestCnf;


// ECM_NET_NAT_DIRECT_QUERY_REQ
typedef struct Tcm_NetNATDirectQueryReq_st
{
	Uint32	m_ssid;
	Sint8	m_dstGPID[POS_UUID_LEN];
} Tcm_NetNATDirectQueryReq;


// ECM_NET_NAT_DIRECT_QUERY_CNF
typedef struct Tcm_NetNATDirectQueryCnf_st
{
	Uint32	m_ssid;
	Sint8	m_dstGPID[POS_UUID_LEN];
	Sint32	m_result;							// 1: can direct; 0: can not direct; -1: no such connection;
	struct sockaddr_in	dstAddr;
} Tcm_NetNATDirectQueryCnf;


// ECM_NET_TRANS_PROTOCOL_QUERY_REQ
typedef struct Tcm_NetTransProtocolQueryReq_st 
{
	Uint32 m_ssid;
	Tpos_addr m_dstAddr;
} Tcm_NetTransProtocolQueryReq;


// ECM_NET_TRANS_PROTOCOL_QUERY_CNF
typedef struct Tcm_NetTransProtocolQueryCnf_st 
{
	Uint32 m_ssid;
	Tpos_addr m_dstAddr;
	Sint32 m_result;				// <0:	failed; >0:	protocol, see RFC 1700. For example, 6: TCP, 17: UDP
} Tcm_NetTransProtocolQueryCnf;


// ECM_NET_TCP_TURN_ROLE_QUERY_REQ
typedef struct Tcm_NetTCPTurnRoleQueryReq_st 
{
	// none
} Tcm_NetTCPTurnRoleQueryReq;


// ECM_NET_TCP_TURN_ROLE_QUERY_CNF
typedef struct Tcm_NetTCPTurnRoleQueryCnf_st 
{
	Sint32 m_result;					// the same as it in ConnectManager.ini, 0 - none, 1 - client, 2 - server
} Tcm_NetTCPTurnRoleQueryCnf;


// ECM_NET_TCP_SERVER_PORT_REQ
typedef struct Tcm_NetTCPServerPortReq_st 
{
	// none
} Tcm_NetTCPServerPortReq;


// ECM_NET_TCP_SERVER_PORT_CNF
typedef struct Tcm_NetTCPServerPortCnf_st
{
	Sint16 m_result;				// not used
	Uint16 m_TCPServerPort;			// network bit order
} Tcm_NetTCPServerPortCnf;


// ECM_NET_LAN_GPID_QUERY_REQ
typedef struct Tcm_NetLanGPIDQueryReq_st 
{
	Uint32 m_ssid;
} Tcm_NetLanGPIDQueryReq;


// ECM_NET_LAN_GPID_QUERY_CNF
typedef struct Tcm_NetLanGPIDQueryCnf_st 
{
	Uint32 m_ssid;
	Sint32 m_result;			// <0: failed, >=0: num of GPIDs
	Uint8 m_GPIDs[0];
} Tcm_NetLanGPIDQueryCnf;


/************************************************************************************************
 *                                                                                              *
 *					      CONNECT MANAGER MANUAL MAP INTERFACE                                  *
 *                                                                                              *
 ************************************************************************************************/
typedef enum 
{
    ECM_MANUALMAP_ADD_REQ = 200,            // add one node to manual map
    ECM_MANUALMAP_ADD_CNF,                  // confirm
    
    ECM_MANUALMAP_QUERY_REQ,                // query whole manual map information
    ECM_MANUALMAP_QUERY_CNF,                // confirm

    ECM_MANUALMAP_DELETE_REQ,               // delete one node from manual map
    ECM_MANUALMAP_DELETE_CNF,               // confirm

    ECM_MANUALMAP_UPDATE_REQ,               // update one node 
    ECM_MANUALMAP_UPDATE_CNF,               // confirm    
}ECM_MANUALMAP_MSG_TYPE;

// ECM_MANUALMAP_ADD_REQ = 200,            
typedef struct
{
    Uint32 m_uiSSID;                        // session id
    Tpos_addr m_AddrInfo;                   // address information
}Tcm_ManualMapAddReq;

// ECM_MANUALMAP_ADD_CNF,                  
typedef struct
{
    Uint32 m_uiSSID;                        // session id, same to request
    Sint32 m_iResult;                       // 0: success. 
                                            // -1: exist port; 
                                            // -2: invalid parameter;
                                            // -3: exceed max count
}Tcm_ManualMapAddCnf;  

// ECM_MANUALMAP_QUERY_REQ,                
typedef struct
{
    Uint32 m_uiSSID;                        // session id
}Tcm_ManualMapQueryReq;

// ECM_MANUALMAP_QUERY_CNF,                
typedef struct
{
    Uint32 m_uiSSID;                        // session id, same to request
    Sint32 m_iCount;                        // count of address
    Tpos_addr m_AddrInfo[0];                // address information    
}Tcm_ManualMapQueryCnf;

// ECM_MANUALMAP_DELETE_REQ,               
typedef struct
{
    Uint32 m_uiSSID;                        // session id
    Tpos_addr m_AddrInfo;                   // address information
}Tcm_ManualMapDeleteReq;

// ECM_MANUALMAP_DELETE_CNF,               
typedef struct
{
    Uint32 m_uiSSID;                        // session id, same to request
    Sint32 m_iResult;                       // 0: success. -1: not exist; -2: invalid parameter
}Tcm_ManualMapDeleteCnf;

// ECM_MANUALMAP_UPDATE_REQ,               
typedef struct
{
    Uint32 m_uiSSID;                        // session id
    Tpos_addr m_OldAddrInfo;                // old address information
    Tpos_addr m_NewAddrInfo;                // new address information
}Tcm_ManualMapUpdateReq;

// ECM_MANUALMAP_UPDATE_CNF,               
typedef struct
{
    Uint32 m_uiSSID;                        // session id, same to request
    Sint32 m_iResult;                       // 0: success. 
                                            // -1: not exist;
                                            // -2: invalid parameter
                                            // -3: new addr exist
}Tcm_ManualMapUpdateCnf;
typedef enum
{
	enum_cm_encrypt_no = 0,
	enum_cm_encrypt_aes128=1,
	enum_cm_encrypt_aes256=2,
}Ecm_EncryptType;
/************************************************************************************************
 *                                                                                              *
 *                  CONNECT MANAGER CLIENT (CMC) INTERFACE                                      *
 *                                                                                              *
 ************************************************************************************************/

/*
 *    Name: cm_init
 *    Description:
 *        Initialize connect manager, the cm client will be conenct
 *          to cm server, and makes register. User will be receive a
 *          result of CM_EVENT_REG_SUC or CM_EVENT_REG_FLD.
 *    Parameters:
 *        pModuleInfo : module information pointer.
 *        uiMsgcId : module message id.
 *		  configFilePath: path of config file
 *    Return:
 *        >=0 : register id.
 *        < 0 : failed, check the error number form ECM_ERROR_CODE.
 *    Remark:
 *
 */
CM_API Sint32 cm_init(Tcm_ModuleInfo *pModuleInfo, Uint32 uiMsgcId, Sint8 *configFilePath);


/*
 *    Name: cm_uninit
 *    Description:
 *        Uninitialize connect manager.
 *    Parameters:
 *        
 *    Return:
 *        All successful.
 *    Remark:
 *
 */
CM_API Sint32 cm_uninit(Sint32 iRegisterID);

/*
 *    Name: cm_register
 *    Description:
 *        register to connect manager server.
 *    Parameters:
 *        pModuleInfo : module information pointer.
 *        uiMsgcId : module message id.
 *    Return:
 *        < 0 : failed, check the error number form ECM_ERROR_CODE.
 *        otherwise : successful, return the register id.
 *    Remark:
 *
 */
CM_API Sint32 cm_register(Tcm_ModuleInfo *pModuleInfo, Sint32 iMsgcId);

/*
 *    Name: cm_unregister
 *    Description:
 *        un-register from connect manager server.
 *    Parameters:
 *        iRegisterID: the ID returned by cms_register(...) interface.
 *    Return:
 *        < 0 : failed, check the error number form ECM_ERROR_CODE.
 *        otherwise : successful.
 *    Remark:
 *
 */
CM_API Sint32 cm_unregister(Sint32 iRegisterID);

/*
 *    Name: cm_send
 *    Description:
 *        Send data to destination without success validate.
 *    Parameters:
 *        pBuf : data buffer.
 *        iLen : data length.
 *        pDstAddr : destination address.
 *		  iEncryptType: encrypt type ...
 *    Return:
 *        == iLen : send success
 *        otherwise : failed send.
 *    Remark:
 *
 */
CM_API Sint32 cm_send(Sint8 *pBuf, Sint32 iLen, Tpos_addr *pDstAddr, Ecm_EncryptType iEncryptType=enum_cm_encrypt_aes256);
CM_API Sint32 cm_send_across(Sint8 *pBuf, Sint32 iLen, Tpos_addr *pDstAddr);


/*
 *    Name: cm_sendto
 *    Description:
 *        Send data to destination without success validate.
 *    Parameters:
 *        pBuf : data buffer.
 *        iLen : data length.
 *        pDstAddr : destination address.
 *		  iEncryptType: encrypt type
 *    Return:
 *        == iLen : send success
 *        otherwise : failed send.
 *    Remark:
 *
 */
CM_API Sint32 cm_sendto(Sint8 *pBuf, Sint32 iLen, Tpos_addr *pDstAddr, Ecm_EncryptType iEncryptType=enum_cm_encrypt_aes256);
CM_API Sint32 cm_sendto_across(Sint8 *pBuf, Sint32 iLen, Tpos_addr *pDstAddr);


/*
 *    Name: cm_sendto_qos
 *    Description:
 *        Send data to destination with QoS service.
 *    Parameters:
 *        pBuf : data buffer.
 *        iLen : data length.
 *        pDstAddr : destination address.
 *        pQoSParam: QoS parameter
 *        iQoSLen: length of QoS parameter, no more than 64 bytes.
 *    Return:
 *        == iLen : send success
 *        otherwise : failed send.
 *    Remark:
 *
 */
CM_API Sint32 cm_sendto_qos(Sint8 *pBuf, Sint32 iLen, 
                            Tpos_addr *pDstAddr, Void *pQoSParam, 
                            Sint32 iQoSLen);


/*
 *    Name: cm_sendto_muc
 *    Description:
 *        Send data to destination with success validate, using muc connect.
 *    Parameters:
 *        pBuf : data buffer.
 *        iLen : data length.
 *        pDstAddr : destination address.
 *    Return:
 *        == iLen : send success
 *        otherwise : failed send.
 *    Remark:
 *
 */
CM_API Sint32 cm_sendto_muc(Sint8 *pBuf, Sint32 iLen, Tpos_addr *pDstAddr);
CM_API Sint32 cm_sendto_muc_across(Sint8 *pBuf, Sint32 iLen, Tpos_addr *pDstAddr);


/* Name: cm_send_tcp
 * Function: send data to destination, using TCP connection.
 * Parameters:
 *		pBuf		[in]	data buffer.
 *		iLen		[in]	data length.
 *		pDstAddr	[in]	destination address.
 * Return value:
 *		== iLen : send success
 *		otherwise : failed send.
*/
CM_API Sint32 cm_send_tcp(Sint8 *pBuf, Sint32 iLen, Tpos_addr *pDstAddr);


/* Name: cm_send_protocol
 * Function: send data to destination, using specified protocol
 * Parameters:
 *		pBuf			[in]	data buffer
 *		iLen			[in]	data length
 *		pDstAddr		[in]	destination address
 *		protocolType	[in]	specified protocol, see ECM_PROTOCOL_TYPE
 *								ECM_PROTOCOL_UDP	send by UDP
 *								ECM_PROTOCOL_TCP	send by TCP
 *								ECM_PROTOCOL_AUTO	send by auto negotiation, prefer TCP
 *	    iEncryptType:	encrypt type
 * Return value:
 *		== iLen: send success
 *		otherwise: failed
 */
CM_API Sint32 cm_send_protocol(Sint8 *pBuf, Sint32 iLen, Tpos_addr *pDstAddr, ECM_PROTOCOL_TYPE protocolType, Ecm_EncryptType iEncryptType=enum_cm_encrypt_aes256);


/*
 *    Name: cm_event_release
 *    Description:
 *        release datagram.
 *    Parameters:
 *        pBuf : data buffer.
 *    Return:
 *    Remark:
 *
 */
CM_API Void cm_event_release(Sint8 *pBuf);


/************************************************************************************************
 *                                                                                              *
 *                  CONNECT MANAGER SERVER (CMS) INTERFACE                                      *
 *                                                                                              *
 ************************************************************************************************/

/*
 *    Name: cm_init
 *    Description:
 *        Initialize connect manager, the cm client will conenct to cm server.
 *    Parameters:
 *        pParam : initialize parameters.
 *    Return:
 *        < 0 : failed, check the error number form ECM_ERROR_CODE.
 *        otherwise : successful.
 *    Remark:
 *
 */
CM_API Sint32 cms_init(Sint8 *configFilePath);


/*
 *    Name: cm_uninit
 *    Description:
 *        Uninitialize connect manager.
 *    Parameters:
 *        
 *    Return:
 *        All successful.
 *    Remark:
 *
 */
CM_API Sint32 cms_uninit(Void);


/*
 *    Name: cms_register
 *    Description:
 *        register to connect manager server.
 *    Parameters:
 *        pModuleInfo : module information pointer.
 *        uiMsgcId : module message id.
 *    Return:
 *        < 0 : failed, check the error number form ECM_ERROR_CODE.
 *        otherwise : successful, return the register id.
 *    Remark:
 *
 */
CM_API Sint32 cms_register(Tcm_ModuleInfo *pModuleInfo, Sint32 uiMsgcId);

/*
 *    Name: cms_unregister
 *    Description:
 *        un-register from connect manager server.
 *    Parameters:
 *        iRegisterID: the ID returned by cms_register(...) interface.
 *    Return:
 *        < 0 : failed, check the error number form ECM_ERROR_CODE.
 *        otherwise : successful.
 *    Remark:
 *
 */
CM_API Sint32 cms_unregister(Sint32 iRegisterID);

/*
 *    Name: cms_vconnect
 *    Description:
 *        Create a virtual connect for nat.
 *    Parameters:
 *        iNetType : net type of.
 *                  defined in pos_addr.h
 *                  #define POS_NET_CLIENT  0
 *                  #define POS_NET_SERVER  1
 *                  #define POS_NET_BOTH  2
 *        pSrcAddr : source address.
 *        pDstAddr : destination address.
 *    Return:
 *        < 0       : failed create.
 *        otherwise : success.
 *    Remark:
 *
 */
CM_API Sint32 cms_vconnect(Sint32 iNetType, Tpos_addr *pSrcAddr, Tpos_addr *pDstAddr);
CM_API Sint32 cms_add_connect(Sint32 iNetType, Sint32 iKeySupport, Tpos_addr *pDstAddr, Tpos_addr *pSecAddr);

/*
 *    Name: cms_find_vc
 *    Description:
 *        Find virtual connect by destination address.
 *    Parameters:
 *        pDstAddr : destination address.
 *    Return:
 *        > 0 : found.
 *        <=0 : failed.
 *    Remark:
 *
 */
typedef struct Tcm_connect_info_st
{
    Sint32 m_iExist;        // 0 - not exist, 1 -  exist
    Sint32 m_iNetType;      // POS_NET_CLIENT or POS_NET_SERVER
    Sint32 m_iKeySupport;   // POS_NET_NONE - none, POS_NET_CLIENT - remote support key, POS_NET_SERVER - local support key.
    Sint32 m_iLockKey;      // 0 - free, 1 - locked
}Tcm_connect_info;
CM_API Sint32 cms_find_vc(Tpos_addr *pDstAddr);
CM_API Sint32 cms_find_connect(Tpos_addr *pDstAddr, Tcm_connect_info *pConnectInfo);

/*
 *    Name: cms_send
 *    Description:
 *        Send data to destination without success validate.
 *    Parameters:
 *        pBuf : data buffer.
 *        iLen : data length.
 *        pDstAddr : destination address.
 *    Return:
 *        == iLen : send success
 *        otherwise : failed send.
 *    Remark:
 *
 */
CM_API Sint32 cms_send(Sint8 *pBuf, Sint32 iLen, Tpos_addr *pDstAddr, Ecm_EncryptType iEncryptType=enum_cm_encrypt_aes256);
CM_API Sint32 cms_send_across(Sint8 *pBuf, Sint32 iLen, Tpos_addr *pDstAddr);
CM_API Sint32 cms_send_plaintext(Sint8 *pBuf, Sint32 iLen, Tpos_addr *pDstAddr);


/*
 *    Name: cms_send
 *    Description:
 *        Send data to destination with success validate.
 *    Parameters:
 *        pBuf : data buffer.
 *        iLen : data length.
 *        pDstAddr : destination address.
 *    Return:
 *        == iLen : send success
 *        otherwise : failed send.
 *    Remark:
 *
 */
CM_API Sint32 cms_sendto(Sint8 *pBuf, Sint32 iLen, Tpos_addr *pDstAddr);
CM_API Sint32 cms_sendto_across(Sint8 *pBuf, Sint32 iLen, Tpos_addr *pDstAddr);
CM_API Sint32 cms_sendto_plaintext(Sint8 *pBuf, Sint32 iLen, Tpos_addr *pDstAddr);


/* Name: cms_send_muc
 * Function: send multi-cast data to destination
 * Parameters:
 *		pBuf		[in]	data buffer
 *		iLen		[in]	data length
 *		pDstAddr	[in]	destination address
 * Return value:
 *		<= 0:	failed
 *		otherwise: succ
*/
CM_API Sint32 cms_send_muc(Sint8 *pBuf, Sint32 iLen, Tpos_addr *pDstAddr);


/* Name: cms_send_tcp
* Function: send data to destination, using TCP connection.
* Parameters:
*		pBuf		[in]	data buffer.
*		iLen		[in]	data length.
*		pDstAddr	[in]	destination address.
* Return value:
*		<=0: failed
*		otherwise: succ
*/
CM_API Sint32 cms_send_tcp(Sint8 *pBuf, Sint32 iLen, Tpos_addr *pDstAddr);


/*
* Name: cms_send_by_port
* Description: send by specific port, used by port detecting
* Parameter:
*	port	[in]	after htons()
* Return:
*	== iLen : send success
*	otherwise : failed send.
*/
CM_API Sint32 cms_send_by_port(Sint8 *pBuf, Sint32 iLen, Tpos_addr *pDstAddr, Uint16 port);

#ifdef __cplusplus
}
#endif

#endif //_CM_H_
