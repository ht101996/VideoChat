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
 * File Name:       msgc_protocol.h
 *
 * Reference:
 *
 * Author:          zzhuang
 *
 * Description:
 *      callfreee protocol definitions
 *
 * History:
 *      please record the history in the following format:
 *      verx.y.z    date        author      description or bug/cr number
 *      ----------------------------------------------------------------
 *      ver0.0.1    jan.23.2009    zzhuang     global difinitions
 *      ver2.0.0    2010-02-05     Kevin Hua  update to os platform 2.0
 *  
 *CodeReview Log:
 *      please record the code review log in the following format:
 *      verx.y.z    date        author      description
 *      ----------------------------------------------------------------
 * 
 */

#ifndef MSGC_PROTOCOL_H
#define MSGC_PROTOCOL_H

/*-----------------------------------------------------------------------*/
#include "pos_type.h"


/* NOTE:
 * THESE DEFINITIONS ARE MADANTORY FOR ALL CALLFREEE DEVELOPINGS */

/* callfreee protocol type definitions,
 * protocol indicates to peer entity communication in different devices/nodes,
 * these pts should be filled into network data packages */
typedef enum
{
    ECF_PT_NULL     = -1,
    ECF_PT_RCP,

    ECF_PT_CM       = 0x03,
    ECF_PT_LUP      = 0x04,
    ECF_PT_DAS      = 0x05,
    ECF_PT_CC       = 0x06,
    ECF_PT_IPCH     = 0x07,
    ECF_PT_TIPCH    = 0x08,
    ECF_PT_GSP      = 0x09,
    ECF_PT_P2P      = 0x0A,
    ECF_PT_SRV      = 0x0B,
    ECF_PT_MOB      = 0x0C,
    ECF_PT_NAT      = 0x0D,
    ECF_PT_NATC     = 0x0E,
	ECF_PT_RTP     = 0x0F,

}Ecf_pt;


/* callfreee protocol datagram common header,
 * protocol indicates to peer entity communication in different devices/nodes,
 * this header is prefixed for all the callfreee protocol data package */
typedef struct
{
    Uint32      v   :3;     /* protocol version */
    Uint32      eh  :1;     /* 0 - it is not external head, 1 - it is a external head; currently 0 */
    Uint32      pt  :4;     /* datagram protocol type, such RCP/RPP, LUP, DAS, CC and so on */
    Uint32      aut :8;     /* head authentication code */
    Uint32      ct  :5;     /* subtype or command type of protocol */
    Uint32      len :11;    /* all this datagram's length, include header and body */

}Tcf_protocol_header;


/* some common macros to access the cf-protocol header */
#define MSGC_CFPROTOCOL_VERSION(cfprotocol_header_ptr) ((cfprotocol_header_ptr)->v)
#define MSGC_CFPROTOCOL_EXTENSION(cfprotocol_header_ptr) ((cfprotocol_header_ptr)->eh)
#define MSGC_CFPROTOCOL_PTYPE(cfprotocol_header_ptr) ((cfprotocol_header_ptr)->pt)
#define MSGC_CFPROTOCOL_AUTHENTIC(cfprotocol_header_ptr) ((cfprotocol_header_ptr)->aut)
#define MSGC_CFPROTOCOL_CMDTYPE(cfprotocol_header_ptr) ((cfprotocol_header_ptr)->ct)
#define MSGC_CFPROTOCOL_DATALEN(cfprotocol_header_ptr) ((cfprotocol_header_ptr)->len)

/*-----------------------------------------------------------------------*/
#endif /* MSGC_PROTOCOL_H */

