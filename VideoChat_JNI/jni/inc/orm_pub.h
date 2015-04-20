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
* File Name:       orm_pub.h
*
* Reference:
*
* Author:          yzli
*
* Description:
*      Overlay Routing Manager public interfaces.
*
* History:
*      please record the history in the following format:
*      verx.y.z    date        author      description or bug/cr number
*      ----------------------------------------------------------------
*      ver0.0.1    11/12/2008  yzli     first draft
*  
*CodeReview Log:
*      please record the code review log in the following format:
*      verx.y.z    date        author      description
*      ----------------------------------------------------------------
* 
*/

/*! @mainpage Overlay Routing Manager Public Interfaces
*
* @section warning_sec Warning
*
* This file is ArcSoft's property. It contains ArcSoft's trade secret, proprietary and 		
* confidential information. 
* 
* The information and code contained in this file is only for authorized ArcSoft employees 
* to design, create, modify, or review.
* 
* DO NOT DISTRIBUTE, DO NOT DUPLICATE OR TRANSIMIT IN ANY FORM WITHOUT PROPER AUTHORIZATION.
* 
* If you are not an intended recipient of this file, you must not copy, distribute, modify, 
* or take any action in reliance on it. 
* 
* If you have received this file in error, please immediately notify ArcSoft and 
* permanently delete the original and any copy of any file and any printout thereof.
*/
/*! \page intro Introduction
	@li lib dependency: msgc.lib cm.lib
	@li exe dependency: msgc.exe connectmanagers.exe
	@li Now you can proceed to the \ref advanced "advanced section".  
*/

/*! \page advanced Change Log
 * @li Overlay Paths Verification for D1.
 * @li Data Transfer for D2.
 * @li KnownAddr -> UUID.
 * @li Make sure you have first read \ref intro "the introduction".
*/

/*! \page todo TODO List
 *	@li todo list1 
 *	@li
 *	@li Make sure you have first read \ref intro "the introduction".
*/

/*! \page issue Open Issues
 *	@li	open issue1
 *  @li Make sure you have first read \ref intro "the introduction".
*/


#ifndef ORM_PUB_H
#define ORM_PUB_H

//!ORM return value for ORP Creat
typedef enum Eorm_create_result{
	ORP_CREATE_FAILURE = -1,
	ORP_CREATE_HOLD	   = 0,
	ORP_CREATE_IMMEDIATE=1

}EORM_CREATE_RESULT;


//!ORM  connect status notification
typedef enum Eorm_Notifier_Event{
	ORM_CONNECT_STATUS_UNKNOWN =0,		//un used
	//!ORP status notification success.
	ORM_CONNECT_STATUS_VERIFIED,	
	//!ORP status notification failed.
	ORM_CONNECT_STATUS_FAILED,	
	//!ORP status renew
	ORM_CONNECT_STATUS_RENEW,
	//!ORM Data Failure.
	ORM_DATA_FAILED,
	//!CM data handle request.	
	ORM_DATA_EVENT	
				
}EORM_NOTIFIER_EVENT;


typedef enum Eorm_Qos_Class{
	QOS_UNKNOWN =-1,	// domain path.
	QOS_DRSP =1,	// Min-Delay-Path:100%
	QOS_TRSP ,	// through put requirement not available yet
	QOS_BRSP ,  // bandwidth not available yet
	QOS_RRSP ,  // not available yet

	QOS_DRMP ,  // if domain delay > Min-Delay-Path delay : 100% Min-Delay-Path, domain 10% 
				// else 100% domain, 10% Min-Delay-Path
	QOS_TRMP ,  // domain path 100%, Min-Delay-Path 20%
	QOS_BRMP ,  // domain path 100%, more than 1 auxiliary path 10%
	QOS_RRMP ,  // domain path 100%, Min-Delay-Path 60%

}EORM_QOS_CLASS;


typedef struct Torm_Qos_Parameter{
	EORM_QOS_CLASS qos_class;		// parameters of singal or multipath support.see ORM_QOS_CLASS
	Sint8		  qos_lable;		// app lable for priority decesions.
	POS_FILLER3;
	Uint32		  qos_flowID;		// app session related infomation.
	Sint32		  qos_exType;
	Void *		  qos_extension;
}ORM_QOS_PARAM;


typedef enum Eorm_Qos_Parameter{
	//!basic functions.
	ORM_QOS_DEFAULT = 0,
	ORM_QOS_DELAY_SENSITIVE,
	ORM_QOS_BANDWIDTH_SENSITIVE,
	ORM_QOS_LOSTRATIO_SENSITIVE,
	ORM_QOS_RELIABILITY_SENSITIVE,

	ORM_QOS_REDUNDANT =10,
	//!

}EORM_QOS_PRARM;


typedef struct Torm_header_node_st{
	Tpos_addr fwd_addr;
	Sint32	  package_len;
	Sint32	  header_len;
	Sint8	  header_content[64];
	struct    Torm_header_node_st * next;
}Torm_header_node;

typedef struct Torm_capsule_header_st{
	Tpos_addr fwd_addr;
	Sint32	  package_len;
	Sint32	  header_len;
	Sint8	  header_content[64];
	struct    Torm_capsule_header_st * next;
}Torm_capsule_header;

typedef enum Eorm_Connection_Type{
	ORM_TYPE_CONNECT_UNKNOWN=0,
	ORM_TYPE_CONNECT_OHR=1,
	ORM_TYPE_CONNECT_TUNNEL,
	ORM_TYPE_CONNECT_DIRECT
}EORM_CONNECT_TYPE;

typedef enum Eorm_Connect_Protocol_enum {
	ORM_CONNECT_PROTOCOL_NONE = -1,
	ORM_CONNECT_PROTOCOL_UDP = 0,
	ORM_CONNECT_PROTOCOL_TCP
} EORM_CONNECT_PROTOCOL;

typedef enum Eorm_Multicast_Event{
	EVENT_NAT_PATHTYPE_IND=0,
	EVENT_NAT_FORWARD_START_IND,
	EVENT_NAT_FORWARD_STOP_IND
}EORM_MULTICAST_EVENT;


typedef struct Torm_Event_CVerify_st{
	//!correspondent peer's address
	Tpos_addr *dst_addr;
	//!local address
	Tpos_addr *src_addr;
	//!protocol, 0: UDP, 1: TCP.
	EORM_CONNECT_PROTOCOL protocol;
}Torm_Connect_Verify;


typedef struct Torm_Event_CFailed_st{
	//!correspondent peer's address
	Tpos_addr *dst_addr;
	//!local address
	Tpos_addr *src_addr;
	//!protocol, 0: UDP, 1: TCP.
	EORM_CONNECT_PROTOCOL protocol;
}Torm_Connect_Failure;

typedef struct Torm_Event_DReceive_st{
	//!correspondent peer's address
	Tpos_addr * dst_addr;
	//!local address
	Tpos_addr * src_addr;
	//! data buffer pointer
	Sint8     * buf_ptr;
	//!data length
	Sint32      data_len;
}Torm_Data_Receive;



typedef struct Torm_Event_DFailure_st{
	//!correspondent peer's address
	Tpos_addr * dst_addr;
	//!local address
	Tpos_addr * src_addr;
	//!data buffer pointer
	Sint8     * buf_ptr;
	//!data length
	Sint32      data_len;
	//!buffer index for cm
	Sint32		buf_index;
}Torm_Data_Failure;



typedef struct Torm_Event_Path_Notify_st{
	Tpos_addr			  known_addr;
	EORM_CONNECT_TYPE     connect_type;
	Tpos_addr				dst_addr;
}Torm_Path_Notify;

typedef struct Torm_Event_Forward_Notify_st{
	Sint32 statusCode;

}Torm_Forwarding_Notify;


// !call back function for connect status update



typedef Sint32 (* ORM_NOTIFIER)(EORM_NOTIFIER_EVENT,		//event type
								Sint8 *	,																//event content
								Sint32 *);															//event content length													

Sint32 orm_init(Sint8 *cmConfigFilePath);																			//init
Sint32 orm_register(ORM_NOTIFIER CallBack);							//call back register
Sint32 orm_uninit();																		//uninit



/**
 *	@brief	data pack function for orm or nat header.
 *			ORP need to be re-created if orm_pack failed.
 *	@param	knownAddr virtual dst addr. [in]
 *	@param  data_ptr address of buffer's pointer, return the new start of data. [in/out]
 *  @param  len_ptr address of data length, return the new length of data. [in/out]
 *  @param  fwdAddr_ptr return the mid hop address for NAT procedure. [out]
 *	@param	srcAdr	source address. [in]
 *  @return 1 for success. -1 for failed.
 */

Sint32 orm_pack(Tpos_addr *  known_addr,	//data packing before sending data to forwarder
				Sint32  handler,					
				Sint8    * * data_ptr,															//return value
				Sint32 	  *  len_ptr,																//return value datasize	
				Tpos_addr *  fwd_addr,													//return value fwdAddr	
				Tpos_addr *  src_addr,
				Sint32 buf_index,
				EORM_CONNECT_PROTOCOL sendProtocol);										

/**
 *	@brief ORP(Overlay Routing Path) creat request.
 *	@param	
 *		knownAddr		[in]	well-known address for P2P search and cm index.
 *		sendProtocol	[out]	if available, indicating transfer protocol. 0: UDP, 1: TCP.
 *	@return 
 *		1		if known address is available.
 *		-1		for waiting for status notification.
 *	
 */
EORM_CREATE_RESULT orm_create(Tpos_addr * known_addr, EORM_CONNECT_PROTOCOL *sendProtocol);			//create Overlay Path Request.


/**
 * @brief ORP(Overlay Routing Path) release request,
 *	
 */
Sint32 orm_release(Tpos_addr * known_addr, Sint32 handle);



/**
*	@brief	data pack function for orm or nat header.
*			ORP need to be re-created if orm_pack failed.
*	@param	knownAddr_ptr virtual dst addr. [in]
*   @param  data_len  length of packing data.[in]
*   @param  bufIndex  index of sending buffer.[in]
*	@param  header_list multi-pack headerlist.[out]
*   @param  qos_param qos paramenters.[in]
*   @return 1 for success. -1 for failed.
*/


Sint32 orm_multi_pack(Tpos_addr * knownAddr_ptr,
					  Sint32 data_len,
					  Sint32 bufIndex,
					  Torm_header_node ** header_list,
					  ORM_QOS_PARAM * qos_param);
/*
Sint32 orm_get_header(Tpos_addr * knownAddr_ptr,
					  Sint32 data_len,
					  Sint32 bufIndex,
					  Torm_header_node ** header_list,
					  ORM_QOS_PARAM * qos_param);
*/


/* Name: orm_NATDirectQuery
* Function: query the direct status between srcGPID and dstGPID
* Parameters:
*	dstGPID		[in]	destination GPID
*	dstAddr		[out]	if direct, return dst IP and Port
* return value: 
*	-1		orm not init
*	>=0		see EORM_CONNECT_TYPE, UNKNOWN includes not found.
*
*/
Sint32 orm_NATDirectQuery(Uint8 *dstGPID, struct sockaddr_in *dstAddr);


/* Name: orm_GetLanGPIDNum
 * Function: query the number of GPIDs in LAN
 * Parameters: none
 * Return value:
 *	<0		failed
 *	>=0		number of GPIDs
*/
Sint32 orm_GetLanGPIDNum();


/* Name: orm_GetLanGPIDs
 * Function: query the GPIDs in LAN
 * Parameters:
 *	buf			[out]	buf to save GPIDs
 *	bufSize		[in]	size of buf
 * Return value:
 *	<0		failed
 *	>=0		number of GPIDs in buf
*/
Sint32 orm_GetLanGPIDs(Uint8 *buf, Sint32 bufSize);


#endif /* end of ORM_PUB_H */



