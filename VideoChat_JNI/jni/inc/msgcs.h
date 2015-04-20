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
 * File Name:       msgcs.h
 *
 * Reference:
 *
 * Author:          zzhuang
 *
 * Description:
 *       the public header for msgc-server; used when msgc-server is a libarary
 * 
 * History:
 *      please record the history in the following format:
 *      verx.y.z    date        author      description or bug/cr number
 *      ----------------------------------------------------------------
 *      ver0.0.1    12/11/2008   zzhuang    
 *      ver2.0.0    2010-02-05   Kevin Hua  update to os platform 2.0
 *  
 *CodeReview Log:
 *      please record the code review log in the following format:
 *      verx.y.z    date        author      description
 *      ----------------------------------------------------------------
 * 
 */
#ifndef MSGCS_H
#define MSGCS_H

#include "pos_prolog.h"
#include "pos_type.h"

#ifdef POS_DLL  // create or using dll library
#   ifdef MSGC_EXPORTS   // dll support
#       define MSGCS_API POS_EXPORTS_DECLSPEC
#   else    // dll using
#       define MSGCS_API POS_IMPORT_DECLSPEC
#   endif
#else   // create or using static library
#   define MSGCS_API
#endif


#ifdef __cplusplus
extern "C"
{
#endif

MSGCS_API Sint32 MsgcServerInit();   // unblock start message server
MSGCS_API Sint32 MsgcServerInitIni(Sint8 *pszPathName);
MSGCS_API Sint32 MsgcServerUnInit(); // unblock stop message server

MSGCS_API Sint32 MsgcServerMain();   // block start message server

#ifdef __cplusplus
}
#endif

/********************************************************/
#endif      /* MSGCS_H */

