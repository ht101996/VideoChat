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
 * File Name:       msgcc.h
 *
 * Reference:
 *
 * Author:          zzhuang
 *
 * Description:
 *      message center client role for all the accessers into msgcenter
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

#ifndef MSGCC_H
#define MSGCC_H

#include "pos_prolog.h"
#include "pos_type.h"

#include "pos_addr.h"
#include "msgc_msg.h"
#include "msgc_protocol.h"
#include "msgc_service.h"

#ifdef POS_DLL  // create or using dll library
#   ifdef MSGC_EXPORTS   // dll support
#       define MSGCC_API POS_EXPORTS_DECLSPEC
#   else    // dll using
#       define MSGCC_API POS_IMPORT_DECLSPEC
#   endif
#else   // create or using static library
#   define MSGCC_API
#endif


#if defined (__cplusplus)
extern "C" {
#endif


/* version information structure for services */
typedef struct tag_cf_version
{
    Uint8   *pucVersion;    /* version in string format, major.middle.minor.build.revision, revision is optional, max length is 64 */
    Uint8   *pucBuildDate;  /* build date, max length is 64 */
    Uint8   *pucCopyRight;  /* copy ring infor, sampel: Arcsoft All Right Reserved, max length is 64 */

    /* NOTE: extention version information left for further usage */
#if 0
    Uint32  unMajor;        /* major version number */
    Uint32  unMiddle;       /* middle version number */
    Uint32  unMinor;        /* minor version number */
    Uint32  unBuild;        /* build version number, increasable only */
    Uint32  unRevision;     /* revirsion version number, increasable only; should get from svn server; or alwayse fill 0 */
#endif

}Tcf_version;

/* msgc-client infor registered to msgc-server */
typedef struct 
{
    Uint8 module_name[16];      /* module name in string format */
    Tcf_version cf_version;     /* version infor for this client */
    Bool8 daemon_supported;     /* whether or not the client need server to keep it alive: TRUE for yes, FALSE for no */
    Uint8 rsvd[3];
    Uint8 *cmd_line;            /* the cmd line to execute the client program, it should be full path, 
                                such as in linux  "/mnt/bin/msgc", in win32 "D:/cm.exe" */
}Tmsgc_client_infor;


/* init when client register to msgc-server */
MSGCC_API Sint32 pcp_msgcc_init(Emsgc_mid mid, Sint32 timeout, Tmsgc_client_infor *client_infor);
MSGCC_API Sint32 pcp_msgcc_init_ini(Emsgc_mid mid, Sint32 timeout, Tmsgc_client_infor *client_infor, Sint8 *filename);
MSGCC_API Sint32 pcp_msgcc_init_ex(mid_t mid, Sint32 timeout, Tmsgc_client_infor *client_infor, Tpos_addr *addr);

MSGCC_API Sint32 pcp_msgcc_fini(Sint32 handle);

MSGCC_API Sint32 pcp_msgcc_sendto(Sint32 handle, Emsgc_mid dst_mid, Sint8* buf, Sint32 len, Sint32 flags);
MSGCC_API Sint32 pcp_msgcc_recvfrom(Sint32 handle, Emsgc_mid *src_mid, Sint8* buf, Sint32 *len, Sint32 flags);
MSGCC_API POS_SOCKET pcp_msgcc_getfd(Sint32 handle);

MSGCC_API Sint32 pcp_msgcc_regmsg(Sint32 handle, Emsgc_msgid msg_id);
MSGCC_API Sint32 pcp_msgcc_regmsggrp(Sint32 handle, Void * group, Sint32 number, Sint32 interval);
MSGCC_API Sint32 pcp_msgcc_unregmsg(Sint32 handle, Emsgc_msgid msg_id);
MSGCC_API Sint32 pcp_msgcc_unregmsggrp(Sint32 handle, Void * group, Sint32 number, Sint32 interval);
MSGCC_API Sint32 pcp_msgcc_regmsgquery(Sint32 handle, Emsgc_msgid *buf, Sint32 *num);

MSGCC_API Sint32 pcp_msgcc_streamopen(Sint32 handle, Emsgc_mid dst_mid);
MSGCC_API Sint32 pcp_msgcc_streamclose(Sint32 handle);
MSGCC_API Sint32 pcp_msgcc_streamsend(Sint32 handle, Sint8* buf, Sint32 len, Sint32 flags);
MSGCC_API Sint32 pcp_msgcc_streamrecv(Sint32 handle, Sint8* buf, Sint32 len, Sint32 flags);

/* a default subroutine for msgc-client user to process unknown msg
 * NOTE: this subroutine should be called by msgc-client user,
 * when the msg from msgc-server is NOT MSGID_USER or EVENTs */
MSGCC_API Sint32 pcp_msgcc_defproc(Sint32 handle, Tmsgc_msg_header* msg, Sint32 len, Sint32 flags);
/* to access the version infor for msgc-client */
MSGCC_API Sint32 pcp_msgcc_getversion(Tcf_version *cf_ver);
/* to control the daemon fucntion in sdk:
 * Emsgc_mid controler: the controler of the module; NOTE: the mid should be the same as that calls pcp_msgcc_init;
 * Sint32 flag: 0 to disable daemon, 1 to enable daemon */
MSGCC_API Sint32 pcp_msgcc_daemonctl(Sint32 handle, Emsgc_mid controler, Sint32 flag);

/*-------------------------------------------------------------------------
 * NOTE: THIS SUBROUTINE IS ONLY FOR TEST AND SHOULD BE REMOVED WHEN RELEASED!!!
 * to control the CMS in sdk:
 * Sint32 handle: the msgcc handle for reporter_module; e.g. handle for MA;
 * Emsgc_mid dest_module: the module lost reported by controller; e.g. CMS;
 * Sint32 report_event: 0 for dest module lost, others reserved */
MSGCC_API Sint32 pcp_msgcc_module_report(Sint32 handle, Emsgc_mid dest_module, Sint32 report_event);

#if defined (__cplusplus)
}
#endif

#endif /*MSGCC_H*/

