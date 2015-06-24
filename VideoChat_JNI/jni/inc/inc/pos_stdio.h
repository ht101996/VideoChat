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
 * File Name:       pos_stdio.h
 *
 * Reference:
 *
 * Author:          Kevin Hua
 *
 * Description:
 *      Redefine IO interface in stdio.h.
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

#ifndef _POS_STDIO_H_
#define _POS_STDIO_H_

#include "pos_prolog.h"

#include <stdio.h>

#ifdef __cplusplus
extern "C"
{
#endif

#if defined (__WIN32__)
# define POS_SLASH       '\\'
# define POS_SLASH_STRING       "\\"
#else
# define POS_SLASH       '/'
# define POS_SLASH_STRING       "/"
#endif


#if defined (__WIN32__)
#   define snprintf     _snprintf
#   define vsnprintf    _vsnprintf
#   define snwprintf    _snwprintf
#elif defined (__LINUX__)  
#elif defined (__SYMBIAN32__)
#endif


#ifdef __cplusplus
}
#endif


#endif // _POS_STDIO_H_
