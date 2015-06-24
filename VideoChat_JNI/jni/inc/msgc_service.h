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
 * File Name:       msgc_service.h
 *
 * Reference:
 *
 * Author:          zzhuang
 *
 * Description:
 *      callfreee services definitions
 *
 * History:
 *      please record the history in the following format:
 *      verx.y.z    date        author      description or bug/cr number
 *      ----------------------------------------------------------------
 *      ver0.0.2    jan.23.2009    zzhuang     global difinitions
 *      ver2.0.0    2010-02-05     Kevin Hua  update to os platform 2.0
 *  
 *CodeReview Log:
 *      please record the code review log in the following format:
 *      verx.y.z    date        author      description
 *      ----------------------------------------------------------------
 * 
 */

#ifndef MSGC_SERVICE_H
#define MSGC_SERVICE_H

/*-----------------------------------------------------------------------*/
#if defined (__WIN32__)
#pragma warning (disable : 4200)
#endif

#include "pos_type.h"


/* NOTE:
 * THESE DEFINITIONS ARE MADANTORY FOR ALL CALLFREEE DEVELOPINGS */

/* callfreee service type definitions,
 * service indicates to local entity communication in the same device/node,
 * these sids should be filled into data packages through msg center */
typedef enum
{
    ECF_SID_NULL    = -1,
    ECF_SID_RCP,        /* voice data, RCP/RPP, just like RTP/RTCP */
    ECF_SID_UM,         /* hareware mangement */

    ECF_SID_MSGC,       /* message center */
    ECF_SID_CM,         /* connect management */

    ECF_SID_AXP,        /* reserved for furture */
    ECF_SID_TALK,       /* talk service, i.e. T200 */
    ECF_SID_GSP,        /* share pstn */
    ECF_SID_GANC,       /* p2p service */
    ECF_SID_IPCH,       /* ipcache */

    ECF_SID_TIPCH,      /* tipcache */

    ECF_SID_LUP,        /* live update protocol */
    ECF_SID_UPGRADE,    /* upgrade_worker */

    ECF_SID_NAT,
    ECF_SID_UPNP,       /* upnp service */
    ECF_SID_AP,         /* access point, for external using */

    ECF_SID_SOFTFXS,    /* soft fxs, it is a soft phone */
    ECF_SID_SOFTDIAL,   /* soft dial, it is a soft phone panel */

    ECF_SID_UFTP,       /* uftp */
}Ecf_service;


/* callfreee service datagram common header,
 * service indicates to local entity communication in the same device/node,
 * this header should be prefixed before data packages through msg center */
typedef struct
{
    Uint32      v   :3;     /* service version */
    Uint32      eh  :1;     /* 0 - it is not external head, 1 - it is a external head; currently 0 */
    Uint32      pt  :7;     /* service type, such rpp, cm, and etc */
    Uint32      ct  :10;    /* subtype or command type of service */
    Uint32      len :13;    /* all this datagram's length, include header and body */
    Uint32          :30;
    Emsgc_mid   src;        /* the source module id for this datagram */
    Emsgc_mid   dst;        /* the destination module id for this datagram */

}Tcf_service_header;

/*----------------------------------------------------------*/
/* service datagram common structure */
/* a typical datagram through msg center:
 * -----------------------------------------------------
 * | msg_hdr   |   Tcf_serice_msg  | actual_user_data  |
 * -----------------------------------------------------
 * |<--- the pointer to be sent or received from msg center
 */

/* service datagram common data block */
typedef struct 
{
    Tcf_service_header  pl_header;
            /* NOTE: the length information in pl_header includes:
             * pl_header + session_id + time_stamp + msg_payload */

    Sint32              session_id;
            /* session id in a cnf should be the same as its req */

    Uint32              time_stamp;

    Uint8               msg_payload[0];
            /* msg body or cmd parameters, its length is in Tcf_service_header */

}Tcf_service_msg;

/* actual user data from msgc;
 * the data pointer starting from this structure will be received or sent through msgcenter */
typedef struct 
{
    Uint8           RESERVED_HEADER[sizeof(Tmsgc_msg_header)];
    Tcf_service_msg service_msg;

}Tcf_service_data;


/* some macros to access the service */
#define MSGC_MSG_HEADER(cfservice_data_ptr) ((cfservice_data_ptr)->RESERVED_HEADER)
#define MSGC_CFSERVICE_MSG(cfservice_data_ptr)   ((cfservice_data_ptr)->service_msg)
#define MSGC_CFSERVICE_SESSIONID(cfservice_msg_ptr) ((cfservice_msg_ptr)->session_id)
#define MSGC_CFSERVICE_TIMESTAMP(cfservice_msg_ptr) ((cfservice_msg_ptr)->time_stamp)
#define MSGC_CFSERVICE_PAYLOAD(cfservice_msg_ptr) ((cfservice_msg_ptr)->msg_payload)

#define MSGC_CFSERVICE_VERSION(cfservice_msg_ptr) ((cfservice_msg_ptr)->pl_header.v)
#define MSGC_CFSERVICE_EXTENSION(cfservice_msg_ptr) ((cfservice_msg_ptr)->pl_header.eh)
#define MSGC_CFSERVICE_PTYPE(cfservice_msg_ptr) ((cfservice_msg_ptr)->pl_header.pt)
#define MSGC_CFSERVICE_CMDTYPE(cfservice_msg_ptr) ((cfservice_msg_ptr)->pl_header.ct)
#define MSGC_CFSERVICE_DATALEN(cfservice_msg_ptr) ((cfservice_msg_ptr)->pl_header.len)
#define MSGC_CFSERVICE_SOURCE(cfservice_msg_ptr) ((cfservice_msg_ptr)->pl_header.src)
#define MSGC_CFSERVICE_DESTINATION(cfservice_msg_ptr) ((cfservice_msg_ptr)->pl_header.dst)

/*-----------------------------------------------------------------------*/
#endif /* MSGC_SERVICE_H */

