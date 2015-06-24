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
 * File Name:       pos_type.h
 *
 * Reference:
 *
 * Author:          Kevin Hua
 *
 * Description:
 *      Base type defination.
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

#ifndef _POS_TYPE_H_
#define _POS_TYPE_H_

#include "pos_prolog.h"

typedef char Sint8;
typedef unsigned char Uint8;

typedef short Sint16;
typedef unsigned short Uint16;

typedef int Sint32;
typedef unsigned int Uint32;

#if defined(__WIN32__)
typedef __int64 Sint64;
typedef unsigned __int64 Uint64;
#else
typedef long long Sint64;
typedef unsigned long long Uint64;
#endif

typedef int Sint;
typedef unsigned int Uint;

typedef float Float;    // update from "typedef float Float32;"
typedef double Double;  // update from "typedef double Float64;"

typedef Sint8 Bool8;    // update from "typedef Uint8 Bool"
#define POS_TRUE 0x01   // update from "#define TRUE (Bool)1"
#define POS_FALSE 0x00  // update from "#define FALSE (Bool)0"

#define Void void           // update from "typedef void VOID"
#define POS_NULL (0)  // update from "#define POS_NULL (void *)0"

typedef Void * POS_HANDLE;
typedef Sint32 POS_STATUS;  // update from "typedef Sint32 Pos_status;"
typedef Sint32 POS_RESULT;

#define POS_FAIL -1
#define POS_OK   0


#define POS_FILLER1 Uint8 filler1_1
#define POS_FILLER2 Uint8 filler2_1;Uint8 filler2_2
#define POS_FILLER3 Uint8 filler3_1;Uint8 filler3_2;Uint8 filler3_3
#define POS_FILLER1_VALUE 0
#define POS_FILLER2_VALUE 0,0
#define POS_FILLER3_VALUE 0,0,0



#ifndef ENUM
# define ENUM enum
#endif

#ifndef UNION
# define UNION union
#endif

#ifndef CONST
# define CONST const
#endif

#ifndef STATIC
# define STATIC static
#endif

#ifndef INLINE
# define INLINE __inline
#endif

#endif // _POS_TYPE_H_
