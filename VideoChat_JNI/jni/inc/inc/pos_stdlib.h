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
 * File Name:       pos_stdlib.h
 *
 * Reference:
 *
 * Author:          Kevin Hua
 *
 * Description:
 *      Redefine library interface in stdlib.h.
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

#ifndef _POS_STDLIB_H_
#define _POS_STDLIB_H_

#include "pos_prolog.h"

#include <stdlib.h>
#include <assert.h>

#ifdef __SYMBIAN32__
#include <e32base.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif

#define pos_assert assert

#define pos_max(a,b)    (((a) > (b)) ? (a) : (b))
#define pos_min(a,b)    (((a) < (b)) ? (a) : (b))

#define pos_rand(min, max)      ((Sint32)(((max)-(min)) * ((Double)rand()/RAND_MAX) + (min)))
#define pos_rand_seed(seed)     srand((seed))

#define POS_HEX_TO_DEC(x)       (((x)/10)<<4)+((x)%10)

#ifdef __cplusplus
}
#endif

#endif // _POS_STDLIB_H_
