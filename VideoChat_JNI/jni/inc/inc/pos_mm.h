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
 * File Name:       mm.h
 *
 * Reference:
 *
 * Author:          Kevin Hua
 *
 * Description:
 *      memory manager interface.
 *
 * History:
 *      please record the history in the following format:
 *      verx.y.z    date        author      description or bug/cr number
 *      ----------------------------------------------------------------
 *      ver0.0.1    09/10/2008  Kevin Hua   first draft
 *  
 *CodeReview Log:
 *      please record the code review log in the following format:
 *      verx.y.z    date        author      description
 *      ----------------------------------------------------------------
 * 
 */

#ifndef _MM_H_
#define _MM_H_

#include "pos_prolog.h"

#ifdef __cplusplus
extern "C"
{
#endif

#if defined (__STATIC_MEMORY__)
#   define pos_mm_malloc mm_malloc_s
#   define pos_mm_free mm_free_s
#   define pos_mm_size mm_size_s
#else
#   define pos_mm_malloc mm_malloc_d
#   define pos_mm_free mm_free_d
#   define pos_mm_size mm_size_d
#endif

POS_API int pos_mm_init();
POS_API int pos_mm_uninit();

POS_API void *mm_malloc_s(unsigned int uiSize);
POS_API void mm_free_s(void *pMemory);
POS_API int mm_size_s();

POS_API void *mm_malloc_d(unsigned int uiSize);
POS_API void mm_free_d(void *pMemory);
POS_API int mm_size_d();


#ifdef __cplusplus
}
#endif

#endif //_MM_H_
