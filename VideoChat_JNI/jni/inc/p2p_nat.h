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
* File Name:       p2p_nat.h
*
* Reference:
*
* Author:          qqWang
*
* Description:
*      Definition of NatServer's message, etc.
*
* History:
*      please record the history in the following format:
*      verx.y.z    date        author      description or bug/cr number
*      ----------------------------------------------------------------
*      ver0.0.1    05/20/2014  qqwang   first draft
*  
*CodeReview Log:
*      please record the code review log in the following format:
*      verx.y.z    date        author      description
*      ----------------------------------------------------------------
* 
*/


#ifndef __P2P_NAT_H__
#define __P2P_NAT_H__

// 0: UDP, 1: RUDP, 2: TCP
#define NAT_SEND_UDP	0
#define NAT_SEND_RUDP	1
#define NAT_SEND_TCP	2
#define NAT_SEND_TYPE	NAT_SEND_UDP

/*
	Other nat related msgs and definitions, please refer to orm_const.h and orm_type.h
*/

typedef ENUM ENAT_MSG_CT_enum
{
	// client message
		ENAT_MSG_CLIENT				= 70,
		ENAT_MSG_CLIENT_REGISTER_REQ,
		ENAT_MSG_CLIENT_REGISTER_ACK,
		ENAT_MSG_CLIENT_KEEPALIVE,
		ENAT_MSG_CLIENT_QUERY,
		ENAT_MSG_CLIENT_QUERYHIT,
// 		ENAT_FORWARD_TRAVERSE_REQ,
// 		ENAT_FORWARD_TRAVERSE_IND,
		
		ENAT_MSG_SERVER_STATUS,

		// turn message
		ENAT_MSG_TURN				= 80,
		ENAT_MSG_TURN_REGISTER_REQ,
		ENAT_MSG_TURN_REGISTER_ACK,
		ENAT_MSG_TURN_UNREGISTER,

		// monitor message
		ENAT_MSG_MONITOR                        = 90,
		ENAT_MSG_MONITOR_ACK,
		ENAT_MSG_EOF
} ENAT_MSG_CT;


/******************************************************************************************/
/*						   Nat  message between client and NatServer     */


/*Note: 
	Please see orm_const.h in details for Nat messages between clientA and clientB,*/

/******************************************************************************************/ 

// ENAT_MSG_CLIENT_REGISTER_REQ
typedef struct Tnat_msg_client_register_req_st 
{
	Uint8 m_localGPID[POS_UUID_LEN];
	Uint32 m_localIP;
	Uint16 m_localPort;
} Tnat_msg_client_register_req;


// ENAT_MSG_CLIENT_REGISTER_ACK
typedef struct Tnat_msg_client_register_ack_st 
{
	// none
} Tnat_msg_client_register_ack;



// ENAT_MSG_CLIENT_QUERY
typedef struct Tnat_msg_client_query_st 
{
	Uint8 m_localGPID[POS_UUID_LEN];
	Uint8 m_remoteGPID[POS_UUID_LEN];
} Tnat_msg_client_query;


// ENAT_MSG_CLIENT_QUERYHIT
typedef struct Tnat_msg_client_queryhit_st
{
	Uint8 m_remoteGPID[POS_UUID_LEN];
	Uint32 m_extIP;
	Uint16 m_extPort;
	Uint16 m_localPort;
	Uint32 m_localIP;
	Uint32 m_turnIP;	//UDP turn server IP
	Uint16 m_turnPort;	//UDP turn server port

	Sint8 m_result;					// <=0: not found, 1: found
	POS_FILLER1;
} Tnat_msg_client_queryhit;

//ENAT_MSG_SERVER_STATUS
typedef struct Tnat_msg_server_status_st
{
	Sint32 servertype;
	Sint32 serverstatus;
} Tnat_msg_server_status;


/************************************************************************/
/*							turn message                                */
/************************************************************************/
// ENAT_MSG_TURN_REGISTER_REQ
typedef struct Tnat_msg_turn_register_req_st 
{
	Uint32 m_localIP;
	Uint16 m_localPort;
	Uint16 m_load;		//负载百分比0-100
} Tnat_msg_turn_register_req;


// ENAT_MSG_TURN_REGISTER_ACK
typedef struct Tnat_msg_turn_register_ack_st
{
	// none
} Tnat_msg_turn_register_ack;


// ENAT_MSG_TURN_UNREGISTER
typedef struct Tnat_msg_turn_unregister_st 
{
	Uint32 m_localIP;
	Uint16 m_localPort;
} Tnat_msg_turn_unregister;



//typedef struct Tnat_Msg_Forward_Req_st --------- this msg is processed in orm itself.



//typedef struct Tnat_Msg_Forward_Ind_st-----------this msg is processd in orm itself.

/*
//ENAT_FORWARD_TRAVERSE_REQ
typedef struct Tnat_Msg_Forward_Traverse_Req_st{
	Uint32 sponsorSessionID;
	Sint8  sponsorUUID[POS_UUID_LEN];
	Sint8  responsorUUID[POS_UUID_LEN];
	Uint32 sponsorIP;
	Uint16 sponsorPort;
	Uint16 responsorMapPort;
	Uint32 responsorMapIP;
	Uint32 extCounter;
	Sint8 *extent[0];
}Tnat_Forward_Traverse_Req;

//ENAT_FORWARD_TRAVERSE_IND
typedef struct Tnat_Msg_Forward_Traverse_Ind_st{
	Uint32 sponsorSessionID;
	Sint8  sponsorUUID[POS_UUID_LEN];
	Sint8  responsorUUID[POS_UUID_LEN];
	Uint32 sponsorMapIP;
	Uint16 sponsorMapPort;
	Uint16 sponsorPort;
	Uint32 sponsorIP;
	Uint32 responsorMapIP;
	Uint16 responsorMapPort;
	POS_FILLER2;
	Uint32 extCounter;
	Sint8  extent[0];
}Tnat_Forward_Traverse_Ind;
*/




#endif