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
 * File Name:       pos_feature.h
 *
 * Reference:
 *
 * Author:          Kevin Hua
 *
 * Description:
 *      product feature definition.
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

#ifndef _POS_FEATURE_H_
#define _POS_FEATURE_H_

#define __UUID_20__ 1

#if defined (__PRODUCT_F__)
#	define __SECURITY__ 1
#	define __SECURITY_CHIP__ 1
#	define __SECURITY_LOAD_DSP__ 1
#   define __WATCHDOG_HDW__ 1
#	define __HARDWARE_PLATFORM_EM__ 1
#elif defined (__PRODUCT_FREE_PP__)
#	define __SECURITY__ 1
#	define __SECURITY_CHIP__ 1
#   define __WATCHDOG_SFW__ 1
#	define __HARDWARE_PLATFORM_EM__ 1
#elif defined (__PRODUCT_T__) || defined (__PRODUCT_U__)
#	define __SECURITY__ 1
#	define __HARDWARE_PLATFORM_PC__ 1
#elif defined (__PRODUCT_M__)
#	define __SECURITY__ 1
#	define __HARDWARE_PLATFORM_MB__ 1
#else
//#error PLEASE DEFINITION THE PRODUCT LINE !
#	define __HARDWARE_PLATFORM_PC__ 1
#endif

// equipment name
#define EQ_MOBILE   "mobile"
#define EQ_PC       "pc"
#define EQ_PC_APP   "pc_app"
#define EQ_PC_WEB   "pc_web"

#endif // _POS_FEATURE_H_

