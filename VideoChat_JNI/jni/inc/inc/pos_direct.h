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
 * File Name:       pos_direct.h
 *
 * Reference:
 *
 * Author:          Kevin Hua
 *
 * Description:
 *      //
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

#ifndef _POS_DIRECT_H_
#define _POS_DIRECT_H_

#include "pos_prolog.h"
#include "pos_tchar.h"
#include "pos_type.h"


#ifdef __cplusplus
extern "C"
{
#endif

#if defined(__WIN32__)
    #define POS_DIRECT_LENGTH_MAX    512
#else
    #define POS_DIRECT_LENGTH_MAX    64
#endif


#if !defined (__UNICODE__)
    #define pos_mkdir pos_mkdir_a
    #define pos_rmdir pos_rmdir_a
#else
    #define pos_mkdir pos_mkdir_w
    #define pos_rmdir pos_rmdir_w
#endif

POS_API Sint32 pos_mkdir_a(CONST Sint8 *dirname);
POS_API Sint32 pos_rmdir_a(CONST Sint8 *dirname);
POS_API Sint32 pos_mkdir_w(CONST wchar_t *dirname);
POS_API Sint32 pos_rmdir_w(CONST wchar_t *dirname);

//POS_API Sint32 pos_folder_exist_a();
//POS_API Sint32 pos_folder_exist_w();
//
//POS_API Sint32 pos_file_exist_a();
//POS_API Sint32 pos_file_exist_w();
//
//POS_API Sint32 pos_file_copy_a(); // in BaflUtils of symbian, CopyFile in Win32 SDK
//POS_API Sint32 pos_file_copy_w();
//
//POS_API Sint32 pos_file_delete_a();
//POS_API Sint32 pos_file_delete_w();
//
//POS_API Sint32 pos_file_rename_a();
//POS_API Sint32 pos_file_rename_w();

#ifdef __cplusplus
}
#endif

#endif // _POS_MAIN_H_
