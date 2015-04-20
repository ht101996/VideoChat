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
 * File Name:       pos_log.h
 *
 * Reference:
 *
 * Author:          Kevin Hua
 *
 * Description:
 *      Easy log interface, it is only support printf.
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

#ifndef _POS_LOG_H_
#define _POS_LOG_H_

#include "pos_prolog.h"
#include "pos_type.h"
#include <stdio.h>
#include <stdarg.h>

// TEMP DEFINATION: symbian write to file and the others write to console
#if defined (__SYMBIAN32__)
# define POS_LOG_FILE
#else
# if defined (__PG_GUI__)
#  define POS_LOG_GUI
# else
#  define POS_LOG_CONSOLE
# endif
#endif

#ifdef __ANDROID__
#include <android/log.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif

POS_API Sint32 pos_log_init(Sint8 *path);
POS_API Sint32 pos_log_uninit();

POS_API Void pos_log_noop(CONST Sint8 *fmt, ...);

#if defined (__WIN32__) || defined (__LINUX__)  
POS_API POS_RESULT pos_log_win32_2console(CONST Sint8 *fmt, ...);
POS_API POS_RESULT pos_log_win32_2gui(CONST Sint8 *fmt, ...);
#   if defined (__WIN32__)
POS_API POS_RESULT pos_log_win32_2debugview(CONST Sint8 *fmt, ...);
#   endif
POS_API POS_RESULT pos_log_win32_2file(CONST Sint8 *fmt, ...);
POS_API POS_RESULT pos_log_win32_2net(CONST Sint8 *fmt, ...);

#   if defined (POS_LOG_FILE)
#       define POS_LOG_IN pos_log_win32_2file
#   elif defined (POS_LOG_NET)
#       define POS_LOG_IN pos_log_win32_2net
#   elif defined (POS_LOG_GUI) || (defined (POS_LOG_DEBUGVIEW) && defined (__WIN32__))
#       define POS_LOG_IN pos_log_win32_2gui
#   else // POS_LOG_CONSOLE
#       define POS_LOG_IN pos_log_win32_2console
#   endif

#elif defined (__SYMBIAN32__)
POS_API POS_RESULT pos_log_sym_2console(CONST Sint8 *fmt, ...);
POS_API POS_RESULT pos_log_sym_2file(CONST Sint8 *fmt, ...);
POS_API POS_RESULT pos_log_sym_2net(CONST Sint8 *fmt, ...);

#   if defined (POS_LOG_FILE)
#       define POS_LOG_IN pos_log_sym_2file
#   elif defined (POS_LOG_NET)
#       define POS_LOG_IN pos_log_sym_2net
#   else // POS_LOG_CONSOLE
#       define POS_LOG_IN pos_log_sym_2console
#   endif

#endif

#if defined(__LOG__)
#   define POS_LOG     POS_LOG_IN
#   define POS_ERR     POS_LOG_IN
#   define POS_EMERG   POS_LOG_IN
#   define POS_TRACE   pos_log_noop
#ifdef __ANDROID__
//#define ANDROID_LOG_TAG "p2p"
//#define POS_LOG(...) __android_log_print(ANDROID_LOG_INFO, ANDROID_LOG_TAG, __VA_ARGS__)
#endif
#else
#   define POS_LOG     pos_log_noop
#   define POS_ERR     pos_log_noop
#   define POS_EMERG   pos_log_noop
#   define POS_TRACE   pos_log_noop
#endif





#ifdef __cplusplus
}
#endif



#endif // _POS_LOG_H_
