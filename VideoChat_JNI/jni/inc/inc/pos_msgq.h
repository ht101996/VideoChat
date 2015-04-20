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
 * File Name:       pos_msgq.h
 *
 * Reference:
 *
 * Author:          Kevin Hua
 *
 * Description:
 *      Message Queue interface.
 *
 * History:
 *      please record the history in the following format:
 *      verx.y.z    date        author      description or bug/cr number
 *      ----------------------------------------------------------------
 *      ver2.0.0    2010-02-05  Kevin Hua   first draft
 *  
 *CodeReview Log:
 *      please record the code review log in the following format:
 *      verx.y.z    date        author      description
 *      ----------------------------------------------------------------
 * 
 */

#ifndef _POS_MSGQ_H_
#define _POS_MSGQ_H_

#include "pos_prolog.h"
#include "pos_type.h"
#include "pos_task.h"


typedef enum _mid_q
{
    MID_Q_INVALID     = 0x00,
    MID_Q_MSGCS,                      /* MSGC server, message route center */
    MID_Q_SYS_UPDATER,                /* System upgrade module aka upgrade_worker */
    MID_Q_SYS_NETMONITOR,             /* System network status monitor aka reset_start */
    MID_Q_SYS_CGI,                    /* Configure web page CGI */

    MID_Q_M           = 0x20,         /* Modules of APP group starts here */
    MID_Q_UM,                         /* user module in main application */
    MID_Q_AXP,                        /* address exchange */
    MID_Q_RCP,                        /* real-control module */
    MID_Q_CM,                         /* connect-management module in main application */  

    MID_Q_UPNP        = 0x30,         /* port-management module */
    MID_Q_LUP,                        /* live-update module */
    MID_Q_TALK,                        /* talk module */
    MID_Q_GSP,                        /* group-service module */
    MID_Q_MGAC,                       /* main-group accesser module */
    MID_Q_AP,                         /* access point module */

    MID_Q_IPCACHE     = 0x40,         /* ipcache module */
    MID_Q_TIPCACHE,                   /* tipcache module */
    MID_Q_GANC,                       /* group access node, i.e. p2p */
    MID_Q_NAT,                        /* NAT */

    MID_Q_GAC,                        /* group access control, i.e. p2p */
    MID_Q_GNC,                        /* group network control, i.e. p2p */

    MID_Q_UPGRADE     = 0x50,         /* upgrade worker */

    MID_Q_SYS_DEBUGER = 0xA0,
    MID_Q_MAX,

    MID_Q_MULTICAST   = 0xB0,
    MID_Q_BROADCAST

}MID_Q_t;


typedef struct pos_msg_st
{
    Uint16 src_module_id;   // soruce module id
    Uint16 dst_module_id;   // destination module id
    Uint16 msg_id;          // message id
    Sint16 data_len;        // message length
    Sint8 *data_ptr;        // message data
}Tpos_msg;

typedef Sint32 (* MSG_EVENT)(Tpos_msg *);   // callback event function defination

typedef struct pos_msg_reg
{
    Uint16 module_id;           // module id
    Uint8 RESERVE[2];
    MSG_EVENT event_ptr;        // module callback event function
}Tpos_msg_reg;


#ifdef __cplusplus
extern "C"
{
#endif

POS_API Sint32 pos_msg_init(Sint32 module_num, Sint32 msg_num, Sint32 data_len);
POS_API Sint32 pos_msg_uninit();
POS_API Sint32 pos_msg_reg(Tpos_msg_reg *msg_reg_ptr);
POS_API Sint32 pos_msg_unreg(Uint16 module_id);
POS_API Sint32 pos_msg_send(Uint16 src_module_id,
                           Uint16 dst_module_id,
                           Uint16 msg_id,
                           Sint16 data_len,
                           Sint8 *data_ptr);
POS_API Sint32 pos_msg_free_size();

#ifdef __cplusplus
}
#endif


#endif // _POS_MSGQ_H_
