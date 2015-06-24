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
 * File Name:       MM_Basic.h
 *
 * Reference:
 *
 * Author:          Kevin Hua
 *
 * Description:
 *      using memory manager allocator memory base class.
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

#ifndef _MM_BASIC_H_
#define _MM_BASIC_H_

#include "pos_prolog.h"
#include "pos_string.h"

class POS_API CMMBasic
{
public:
    CMMBasic();
    virtual ~CMMBasic();

    POS_API_IN void *operator new(size_t size);
    POS_API_IN void operator delete(void *p);
    POS_API_IN void operator delete(void *p, size_t size);

    POS_API_IN void *operator new(size_t size, void *_where);
    POS_API_IN void operator delete(void *p, void *_where);


    POS_API_IN void *operator new[](size_t size);
    POS_API_IN void operator delete[](void *p);
    POS_API_IN void operator delete[](void *p, size_t size);

    POS_API_IN void *operator new[](size_t size, void *_where);
    POS_API_IN void operator delete[](void *p, void *_where);
};

class POS_API CMMBasic_Auto
{
};

#if defined __STATIC_MEMORY__
    typedef CMMBasic CMMBase;
#else
    typedef CMMBasic_Auto CMMBase;
#endif


#endif //_MM_BASIC_H_
