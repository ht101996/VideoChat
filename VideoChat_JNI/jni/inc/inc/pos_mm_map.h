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
 * File Name:       mm_map.h
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
 *      ver0.0.1    10/22/2008  Kevin Hua   first draft
 *  
 *CodeReview Log:
 *      please record the code review log in the following format:
 *      verx.y.z    date        author      description
 *      ----------------------------------------------------------------
 * 
 */

#ifndef _MM_MAP_H_
#define _MM_MAP_H_

#include "pos_prolog.h"
#include "pos_mm_allocator.h"
#include <map>

_STLP_BEGIN_NAMESPACE

#if defined (__STATIC_MEMORY__)

template<typename _Key, typename _Tp, typename _Compare = less<_Key>, typename _Alloc = mm_allocator<pair<const _Key, _Tp> > >
class mm_map : public map<_Key, _Tp, _Compare, _Alloc>
{
public:
};

template<typename _Key, typename _Tp, typename _Compare = less<_Key>, typename _Alloc = mm_allocator<pair<const _Key, _Tp> > >
class mm_multimap : public multimap<_Key, _Tp, _Compare, _Alloc>
{
public:
};

#else

template<typename _Key, typename _Tp, typename _Compare = less<_Key>, typename _Alloc = allocator<pair<const _Key, _Tp> > >
class mm_map : public map<_Key, _Tp, _Compare, _Alloc>
{
public:
};

template<typename _Key, typename _Tp, typename _Compare = less<_Key>, typename _Alloc = allocator<pair<const _Key, _Tp> > >
class mm_multimap : public multimap<_Key, _Tp, _Compare, _Alloc>
{
public:
};

#endif

_STLP_END_NAMESPACE


#endif //_MM_MAP_H_
