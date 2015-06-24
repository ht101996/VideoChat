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
 * File Name:       pos_mm.h
 *
 * Reference:
 *
 * Author:          Kevin Hua
 *
 * Description:
 *      // memory interface and memory manager interface.
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

#ifndef _POS_MM_H_
#define _POS_MM_H_

#include "pos_prolog.h"
#include "pos_type.h"

#ifdef __cplusplus
extern "C"
{
#endif

POS_API Void * pos_malloc(Sint32 iSize);
POS_API POS_RESULT pos_free(Void *pvBuf);

POS_API Sint8 * pos_dump(Uint8 *pszIn, Sint8 *pszOut, Sint32 iLen);

POS_API Sint32 pos_str2hexA(Sint8 *pszIn, Sint8 *pszOut);

#ifdef __cplusplus
}
#endif

#endif //_POS_MM_H_
