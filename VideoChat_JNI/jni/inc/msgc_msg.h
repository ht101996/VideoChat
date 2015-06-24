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
 * File Name:       msgc_msg.h
 *
 * Reference:
 *
 * Author:          zzhuang
 *
 * Description:
 *      message information: module id & msg id, structure definitions
  *
 * History:
 *      please record the history in the following format:
 *      verx.y.z    date        author      description or bug/cr number
 *      ----------------------------------------------------------------
 *      ver1.0.2    jan.23.2009    zzhuang     global definitions
 *      ver2.0.0    2010-02-05     Kevin Hua  update to os platform 2.0
 *  
 *CodeReview Log:
 *      please record the code review log in the following format:
 *      verx.y.z    date        author      description
 *      ----------------------------------------------------------------
 * 
 */

#ifndef MSGC_MSG_H
#define MSGC_MSG_H

#if defined (__WIN32__)
#pragma warning (disable : 4200)
#endif

#include "pos_prolog.h"
#include "pos_string.h"
#include "pos_type.h"
#include "pos_log.h"
//#include "pos_utl.h"

/* global module idetifications */
typedef enum 
{
    MID_INVALID     = 0,
    MID_MSGCS,              /* MSGC server, message route center */
    MID_SYS_UPGRADE,        /* System upgrade module aka upgrade_worker */
    MID_SYS_NETMONITOR,     /* System network status monitor aka reset_start */
    MID_SYS_CGI,            /* Configure web page CGI */
    MID_DAEMON,             /* daemon module to backgroud monitor the whole system */

    /*-------------------------------------------------------------*/
    MID_M,           //= 0x20, /* Modules of APP group starts here */
    MID_M_CMC,
    MID_UM,                 /* user module in main application */
    MID_UM_CMC,
    MID_AXP,                /* address exchange */
    MID_AXP_CMC,
    MID_RCP,                /* real-control module */
    MID_RCP_CMC,
    MID_CM,                 /* connect-management module in main application */ 
    
    MID_SOFTFXS,			/* soft fxs, it is a soft phone */
    MID_SOFTDIAL,			/* soft dial, it is a soft phone panel */

    MID_UPNP,        //= 0x30, /* port-management module */
    MID_UPNP_CMC,
    MID_LUP,                /* live-update module */
    MID_LUP_CMC,
    MID_TALK,               /* TALK module */
    MID_TALK_CMC,
    MID_DAS,                /* device-access module */
    MID_DAS_CMC,
    MID_CC,                 /* call-control module */
    MID_CC_CMC,
    MID_GSP,                /* group-service module */
    MID_GSP_CMC,
    MID_MGAC,               /* main-group accesser module */
    MID_MGAC_CMC,
    MID_AP,                 /* access poSint32 module */
    //MID_AP_CMC,

    MID_IPCACHE,     //= 0x40, /* ipcache module */
    MID_IPCACHE_CMC,
    MID_TIPCACHE,           /* tipcache module */
    MID_TIPCACHE_CMC,
    MID_GANC,               /* group access node, i.e. p2p */
    MID_GANC_CMC,
    
    MID_NAT,                /* NAT */
    MID_NAT_CMC,
    MID_NAT_CMS,

    MID_GAC,                /* group access control, i.e. p2p */
    MID_GAC_CMC,
    MID_GNC,                /* group network control, i.e. p2p */
    MID_GNC_CMC,

    MID_ARCQ,                /* ARC-Q service */
    MID_ARCQ_CMC,

    MID_UFTP,               /* uftp module */
    MID_UFTP_CMC,  

    MID_UPGRADE,     //= 0x50, /* upgrade worker */
    MID_UPGRADE_CMC,

    MID_SERVER,     //= 0x60,
    MID_SERVER_CMC,

    MID_AP_EXT,      //= 0x90, /* this is reserved for web/cgi and other external modules */

        

    /*-------------------------------------------------------------*/   
    MID_SYS_DEBUGER = 0x1FF,
    MID_MAX,

    MID_MULTICAST   = 0x20F,
    MID_BROADCAST

}Emsgc_mid;


/* global message idetifications */
typedef enum 
{
    MSGID_INVALID = 0,
    MSGID_SYS_START = 0x1, 
    MSGID_SYS_MODREG,       /*Module register to MSGC server*/
    MSGID_SYS_MODUNREG,     /*Module deregister to MSGC server*/
    MSGID_SYS_MODREGACK,    /*Module register/deregister acknowledge from MSGC server*/
    MSGID_SYS_MSGREG,       /*Multicast message register to MSGC server*/
    MSGID_SYS_MSGUNREG,     /*Multicast message deregister to MSGC server*/
    MSGID_SYS_MSGREGACK,    /*Multicast message register/deregister acknowledge from MSGC server*/
    MSGID_SYS_GETVERSION,   /*System software version query to UPDATER*/
    MSGID_SYS_DAIL,         /*PPPoE dail request to NETMONITOR*/
    MSGID_SYS_HANGUP,       /*PPPoE hangup request to NETMONITOR*/
    MSGID_SYS_SUSPENDDAIL,  /*PPPoE dail proccess suspend request to NETMONITOR*/
    MSGID_SYS_RESUMEDAIL,   /*PPPoE dail proccess resume request to NETMONITOR*/
    MSGID_GET_VERSION,
    MSGID_GET_VERSION_ACK,
    MSGID_DEBUGGER = 0x10000,
    MSGID_DAEMON = 0x1001f,
    MSGID_DAEMON_DISABLE,
    MSGID_DAEMON_ENABLE,
    MSGID_DAEMON_REPORT,         /* ONLY FOR TEST: @ MAY.31.2009: daemon report from client */

    MSGID_USER = 0x1003f,   /* MSGID of all user applications, such as cm/rpp/ganc, etc */

    MSGID_INIT,             /* to init the module or server or application */
    MSGID_UNINIT,           /* to uninit the module or server or application */
    MSGID_RESET,            /* to reset the module or server or application */
    MSGID_STATUS_QUERY,     /* to query the stauts for the module or server or application; maybe keep alive */
    MSGID_STATUS_QUERYACK,     /* to query the stauts for the module or server or application; maybe keep alive */

    /* events defitions
     * event: broadcast messages for those who are willing to receive */
    MSGID_EVENT_BASE = 0x11000,         /* the basic value for event */

    MSGID_EVENT_REBOOT,                 /* the whole system is going to reboot */
    MSGID_EVENT_RESET,                  /* the whole system is going to reset */
    MSGID_EVENT_RESTORE,                /* to restore the user settings */

    MSGID_EVENT_EXTERNAL_IP_CHANGED,    /* to notify that external ip address is changed */

    MSGID_EVENT_USER_OFFHOOK,           /* to notify user hooks off the fxs */
    MSGID_EVENT_USER_ONHOOK,            /* to notify user hooks on the fxs */
    MSGID_EVENT_CALL_PSTN_BEGIN,        /* to notify that a PSTN call begins */
    MSGID_EVENT_CALL_PSTN_END,          /* to notify that a PSTN call ends */
    MSGID_EVENT_CALL_IP_BEGIN,          /* to notify that an IP call begins */
    MSGID_EVENT_CALL_IP_END,            /* to notify that an IP call ends */

    MSGID_EVENT_IGD_SERVER_IND,          /* to notify that some events happened in IGD SERVER, i.e. upnp or upnp-manager */

    MSGID_EVENT_CALLFREEE_SERVER_IND,    /* to notify that the ip address or port for callfreee server is changed */
    
    MSGID_EVENT_UFTP_RESTART_IND,

    MSGID_EVENT_NAT_PATHTYPE_IND = 0x11020,    /* to notify the type of current NAT/ORM transfer path */

    MSGID_EVENT_GPID_CHANGED, // gpid change notification for p2p
    

    MSGID_EVENT_MAX = 0x110FF,          /* the max id of event, NOTE: THIS IS INVALID EVENT */

    MSGID_MAX =  0x007fffff,

}Emsgc_msgid;

/* NOTE: these are former version definitions,
 * left here for compile */
typedef Emsgc_mid mid_t;
typedef Emsgc_msgid msgid_t;


/*headroom reserved by system*/
#define MSG_HDR_LEN 64

/*max length of user data per message*/
//#define MSG_DATA_LEN_MAX 1280
#define MSG_DATA_LEN_MAX 4096

/*send and recv flags*/
#define MSG_FLAG_INPLACE 1

typedef struct msg_hdr
{
    Emsgc_msgid msgtype;
    Uint16 res;
    Uint16 datalen; 	/* Application data length */
    Uint8 data[0];

}Tmsgc_msg_header;

#define MSGC_MSG_TYPE(msgc_msg_ptr) ((msgc_msg_ptr)->msgtype)
#define MSGC_MSG_LEN(msgc_msg_ptr) ((msgc_msg_ptr)->datalen)
#define MSGC_MSG_DATA(msgc_msg_ptr) ((msgc_msg_ptr)->data)


/*--------------------------------------------------------------*/
/* TLV definitions */
/* msg option in TLV format structure definitions for all the msg parameters */
typedef struct 
{
    Uint8   t;         /* msg type */
    Uint8   rsvd;
    Uint16  l;
    Uint8   v[0];       /* payload in octets */

}Tcf_msg_opt;

#define MSGC_OPT_TYPE(opt_ptr)  ((opt_ptr)->t)
#define MSGC_OPT_LEN(opt_ptr)  ((opt_ptr)->l)
#define MSGC_OPT_VALUE(opt_ptr)  ((opt_ptr)->v)

#define EMSGC_OPT_RESEVED               0
#define MSGC_OPT_RESERVED_LEN       1
#define MSGC_ALIGN4(v)           ((Sint32(v)+3)&~3)
//#define MSGC_IS_ALIGN4(v)        (!(Sint32(v) & 3))
#define MSGC_IS_ALIGN4(v)       (!((*(Uint32 *)(&v)) & 3))

static __inline Sint32 MSGC_OPT_CHECK(Uint8 *m, Sint32 i_len, Tcf_msg_opt *opt)
{
    Sint32 len = i_len - (Sint32)((Sint8*)(opt) - (Sint8*)(m));
    Uint32 tmp_i = 0;

    if((len <= 0) || (opt == NULL))
        return -1;

    if(opt->t == EMSGC_OPT_RESEVED)
        return 0;

    tmp_i = (Uint32)opt->l & 0x0000FFFF;    /* NOTE BY ZZHUANG @ FEB.6.2009: convert from s16 to s32 may meet error!! */
    if((tmp_i + sizeof(Tcf_msg_opt)) > (Uint32)len)
    {
#ifdef TRACE_SWITCH
        POS_TRACE("MSGC: Message length error(Leave len: %d, opt len: %d!)!\n", len, opt->l);    
#endif
        return -1;
    }   

    return 0;
}

/* to get TLV from input buffer m; NOTE: m is in group of TLV, no Tcf_service_header existing!!
 * m: the beginning of the TLVs, and NOT changed after this GET;
 * i_len: the count of m in octets;
 * opt: the start checking address in TLV to access from input m;
 * return: the 1st found TLV if existed in m from opt position */
static __inline Tcf_msg_opt *MSGC_OPT_GET_NEXT(Uint8 *m, Sint32 i_len, Tcf_msg_opt *opt)
{
    Tcf_msg_opt *o;
    Sint32 len = i_len - (Sint32)((Sint8*)(opt) - (Sint8*)(m));

    if(len <= 0)
        return NULL;

    /* NOTE by zzhuang @ F3eb.12.2009 */
    /* NOTE: this subroutine may meet error when in big-endian platform!! */

    if(opt->t == EMSGC_OPT_RESEVED)
        o = (Tcf_msg_opt *)((Sint8*)opt + MSGC_OPT_RESERVED_LEN);
    else 
        o = (Tcf_msg_opt *)((Sint8*)opt + sizeof(Tcf_msg_opt) + opt->l);

    while(o->t == EMSGC_OPT_RESEVED) { 
        o = (Tcf_msg_opt*)((Sint8*)o + MSGC_OPT_RESERVED_LEN);
        if(--len == 0)
            return NULL;
    } 

    if(MSGC_OPT_CHECK(m, i_len, o) == 0)
        return o;
    return NULL;
}

/* to put TLV into input buffer m; NOTE: m is in group of TLV, no Tcf_service_header existing!!
 * m: the address to be filled with input TLV, NOT changed after TLV filling;
 * o_len: the offset of m to be filled with input TLV; it'll be plusd with L in TLV, i.e. data_len when subroutine returns;
 * data_type: the T of TLV to be put;
 * data_len: the L of TLV to be put;
 * data_value: the V of TLV to be put;
 * return: the filled TLV position in m */
static __inline Tcf_msg_opt *MSGC_OPT_PUT(Uint8 *m, Sint32 *o_len, Uint8 data_type, Sint32 data_len, Void *data_value)
{
    Tcf_msg_opt *o;
    if(data_type == EMSGC_OPT_RESEVED)
        return NULL;

    o = (Tcf_msg_opt *)(*o_len + (Sint8*)m);
    while(!MSGC_IS_ALIGN4(o)) {
        o->t = EMSGC_OPT_RESEVED;
        *o_len += MSGC_OPT_RESERVED_LEN;
        o = (Tcf_msg_opt *)((Sint8*)o + MSGC_OPT_RESERVED_LEN);
    }

    o->t = data_type;
    o->l = (Uint16)data_len;
    if(data_value != NULL)
        memcpy(o->v, data_value, data_len);
    *o_len += data_len + sizeof(Tcf_msg_opt);

    return o;
}


/***************************************************/
#endif /*MSGC_MSG_H*/

