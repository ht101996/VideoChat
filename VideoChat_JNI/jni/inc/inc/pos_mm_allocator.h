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
 * File Name:       MM_Allocator.h
 *
 * Reference:
 *
 * Author:          Kevin Hua
 *
 * Description:
 *      STL allocator base class defination.
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

#ifndef _MM_ALLOCATOR_H_
#define _MM_ALLOCATOR_H_

#include "pos_prolog.h"
#include "pos_mm.h"

#include <string>


_STLP_BEGIN_NAMESPACE

template <class _Tp>
class mm_allocator
    : public allocator<_Tp>
{
public:
  typedef _Tp        value_type;
  typedef _Tp*       pointer;
  typedef const _Tp* const_pointer;
  typedef _Tp&       reference;
  typedef const _Tp& const_reference;
  typedef size_t     size_type;
  typedef ptrdiff_t  difference_type;

#if defined (_STLP_MEMBER_TEMPLATE_CLASSES)
  template <class _Tp1> struct rebind {
    typedef mm_allocator<_Tp1> other;
  };
#endif
  mm_allocator() _STLP_NOTHROW {}
#if defined (_STLP_MEMBER_TEMPLATES)
  template <class _Tp1> mm_allocator(const mm_allocator<_Tp1>&) _STLP_NOTHROW {}
#endif
  mm_allocator(const mm_allocator<_Tp>&) _STLP_NOTHROW {}
  mm_allocator(__move_source<mm_allocator<_Tp> > src) _STLP_NOTHROW {}
  ~mm_allocator() _STLP_NOTHROW {}
  pointer address(reference __x) const {return &__x;}
  const_pointer address(const_reference __x) const { return &__x; }
  // __n is permitted to be 0.  The C++ standard says nothing about what the return value is when __n == 0.
  _Tp* allocate(size_type __n, const void* = 0) {
    if (__n > max_size()) {
      __THROW_BAD_ALLOC;
    }
    if (__n != 0) {
      size_type __buf_size = __n * sizeof(value_type);
      _Tp* __ret = __REINTERPRET_CAST(_Tp*, pos_mm_malloc(__buf_size));
      //_Tp* __ret = __REINTERPRET_CAST(_Tp*, __sgi_alloc::allocate(__buf_size));
#if defined (_STLP_DEBUG_UNINITIALIZED) && !defined (_STLP_DEBUG_ALLOC)
      if (__ret != 0) {
        memset((char*)__ret, _STLP_SHRED_BYTE, __buf_size);
      }
#endif
      return __ret;
    }
    else
      return 0;
  }
  // __p is permitted to be a null pointer, only if n==0.
  void deallocate(pointer __p, size_type __n) {
    _STLP_ASSERT( (__p == 0) == (__n == 0) )
    if (__p != 0) {
#if defined (_STLP_DEBUG_UNINITIALIZED) && !defined (_STLP_DEBUG_ALLOC)
      memset((char*)__p, _STLP_SHRED_BYTE, __n * sizeof(value_type));
#endif
      pos_mm_free((void *)__p);
      //__sgi_alloc::deallocate((void*)__p, __n * sizeof(value_type));
    }
  }
  // backwards compatibility
  void deallocate(pointer __p) const {  
      if (__p != 0) 
          pos_mm_free((void *)__p);
          //__sgi_alloc::deallocate((void*)__p, sizeof(value_type)); 
  }
  size_type max_size() const _STLP_NOTHROW  { return size_t(-1) / sizeof(value_type); }
  void construct(pointer __p, const_reference __val) { _STLP_STD::_Copy_Construct(__p, __val); }
  void destroy(pointer __p) { _STLP_STD::_Destroy(__p); }
#if defined(__MRC__)||(defined(__SC__) && !defined(__DMC__))
  template <class _T2> bool operator==(const mm_allocator<_T2>&) const _STLP_NOTHROW { return true; }
  template <class _T2> bool operator!=(const mm_allocator<_T2>&) const _STLP_NOTHROW { return false; }
#endif

#if defined (_STLP_USE_PARTIAL_SPEC_WORKAROUND) && !defined (_STLP_FUNCTION_TMPL_PARTIAL_ORDER)
  //This is just to make swap workaround for compiler without template function partial
  //happy.
  void swap(mm_allocator<_Tp>&) {}
#endif

#if defined (_STLP_NO_EXTENSIONS)
  /* STLport extension giving rounded size of an allocated memory buffer
   * This method do not have to be part of a user defined allocator implementation
   * and won't even be called if such a function was granted.
   */
protected:
#endif
  _Tp* allocate(size_type __n, size_type& __allocated_n) {
    if (__n > max_size()) {
      __THROW_BAD_ALLOC;
    }

    if (__n != 0) {
      size_type __buf_size = __n * sizeof(value_type);
      _Tp* __ret = __REINTERPRET_CAST(_Tp*, pos_mm_malloc(__buf_size));
      //_Tp* __ret = __REINTERPRET_CAST(_Tp*, __sgi_alloc::allocate(__buf_size));
#if defined (_STLP_DEBUG_UNINITIALIZED) && !defined (_STLP_DEBUG_ALLOC)
      if (__ret != 0) {
        memset((char*)__ret, _STLP_SHRED_BYTE, __buf_size);
      }
#endif
      __allocated_n = __buf_size / sizeof(value_type);
      return __ret;
    }
    else
      return 0;
  }
};

_STLP_END_NAMESPACE


#endif //_MM_ALLOCATOR_H_
