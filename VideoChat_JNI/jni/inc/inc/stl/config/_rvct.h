/*
 * © Portions copyright (c) 2006-2007 Nokia Corporation.  All rights reserved.
 */  
// =============================================================================
//	stl_rvct.h
//	This is a list of settings for STLport specific to the ARM RVCT compiler.
//	Do not include this file directly.
// =============================================================================

# ifndef _STLP_RVCT_H
#  define  _STLP_RVCT_H


#if defined(__WINS__)
#  pragma warning ( disable : 4018 4097 4100 4127 4244 4284 4511 4512 4514 4786 4800 ) //4355 4284  4231 4511 4512 4097 4786 4800 4018 4146 4244 4514 4127 4100 4663)
#  define _STLP_NO_FUNCTION_TMPL_PARTIAL_ORDER 1
// Uncomment if partial order of template functions is not available
#  define _STLP_NO_FUNC_PARTIAL_ORDERING 1
#endif

#ifndef _STLP_NO_IOSTREAMS
//#  define _STLP_NO_IOSTREAMS	               1
#endif

//#  undef  _STLP_OWN_IOSTREAMS
//#define _STLP_OWN_IOSTREAMS
#define _STLP_USE_NEW_IOSTREAMS
//#  define _STLP_NO_NATIVE_MBSTATE_T            1
//#  define _NOTHREADS                           1
//#  define _STLP_NO_TYPEINFO                    1

#ifndef __PLACEMENT_NEW_INLINE
#ifndef __E32STD_H__
#ifdef __cplusplus
inline void * operator new(unsigned int, void *_P) throw() { return (_P); }
inline void operator delete(void* /*aPtr*/, void* /*aBase*/) __NO_THROW {}
#endif
#define __PLACEMENT_NEW_INLINE
#endif // E32STD_H
#endif


//==========================================================

// the values choosen here as defaults try to give
// maximum functionality on the most conservative settings

// Mostly correct guess, change it for Alpha (and other environments
// that has 64-bit "long")
#  define _STLP_UINT32_T unsigned long


// Define if wchar_t is not a unique type, and is actually a typedef to unsigned short. 
// #  define _STLP_WCHAR_T_IS_USHORT 1

// Uncomment if long long is available
 #  define _STLP_LONG_LONG long long

// Uncomment if long double is not available
// #  define _STLP_NO_LONG_DOUBLE 1

// Uncomment this if your compiler does not support namespaces 
//#  define _STLP_HAS_NO_NAMESPACES 1

// Uncomment if "using" keyword does not work with template types 
// # define _STLP_BROKEN_USING_DIRECTIVE 1

// Uncomment this if your compiler does not support exceptions
//#  define _STLP_HAS_NO_EXCEPTIONS 1
//#  define _STLP_USE_TRAP_LEAVE 1

// Header <new> that comes with the compiler 
// does not define bad_alloc exception
//#if defined(__WINS__)
// Possibly required ???
#  define _STLP_NO_BAD_ALLOC  1
//#endif

// Uncomment if member template methods are not available
// #  define _STLP_NO_MEMBER_TEMPLATES   1

// Uncomment if member template classes are not available
#if defined(__WINS__)
#  define _STLP_NO_MEMBER_TEMPLATE_CLASSES   1
#endif

// Uncomment if friend member templates are not available
#if defined(__WINS__)
// Possibly required ???
//#  define _STLP_NO_FRIEND_TEMPLATES   1
#endif

// Compiler does not accept friend declaration qualified with namespace name.
// #  define _STLP_NO_QUALIFIED_FRIENDS 1

// Uncomment if partial specialization is not available
#if defined(__WINS__)
#  define _STLP_NO_CLASS_PARTIAL_SPECIALIZATION 1
#endif

// Define if class being partially specialized require full name (template parameters)
// of itself for method declarations
// #  define _STLP_PARTIAL_SPEC_NEEDS_TEMPLATE_ARGS

// Compiler has problems specializing members of partially 
// specialized class
// #  define _STLP_MEMBER_SPECIALIZATION_BUG

// Compiler requires typename keyword on outline method definition 
// explicitly taking nested types/typedefs
// #define  _STLP_TYPENAME_ON_RETURN_TYPE

// * _STLP_STATIC_CONST_INIT_BUG: defined if the compiler can't handle a
//   constant-initializer in the declaration of a static const data member
//   of integer type.  (See section 9.4.2, paragraph 4, of the C++ standard.)
#if defined(__WINS__)
# define _STLP_STATIC_CONST_INIT_BUG
#endif

// Define if default constructor for builtin integer type fails to initialize it to 0
// #  define _STLP_DEFAULT_CONSTRUCTOR_BUG    1

// Defined if constructor
// required to explicitly call member's default constructors for const objects
// #  define _STLP_CONST_CONSTRUCTOR_BUG    1

// Define this if compiler lacks <exception> header
#  define _STLP_NO_EXCEPTION_HEADER 1

// Uncomment if native new-style C library headers lile <cstddef>, etc are not available.
#   define _STLP_HAS_NO_NEW_C_HEADERS 1

// uncomment if new-style headers <new> is available
// #  define _STLP_HAS_NEW_NEW_HEADER 1

// uncomment this if <iostream> and other STD headers put their stuff in ::namespace,
// not std::
// #  define _STLP_VENDOR_GLOBAL_STD

// uncomment this if <cstdio> and the like put stuff in ::namespace,
// not std::
#  define _STLP_VENDOR_GLOBAL_CSTD
/*
#  ifdef _PTHREADS
#    undef  _PTHREADS
#  endif
#  ifdef _STLP_PTHREADS
#    undef _STLP_PTHREADS
#  endif
#  ifdef _STLP_THREADS
#    undef _STLP_THREADS
#  endif
*/

#ifdef _STLP_USE_NEW_C_HEADERS
#undef _STLP_USE_NEW_C_HEADERS
#endif

#undef _STLP_NATIVE_C_HEADER
#ifndef __SYMBIAN32__
#define _STLP_NATIVE_C_HEADER(x) <libc/##x>
#endif
#define _STLP_NATIVE_HEADER(x) <x>



#  define _STLP_NO_NEW_HEADER
#  define _STLP_NO_THREADS

#  define _STLP_NO_EXCEPTIONS
//#   define _STLP_USE_TRAP_LEAVE

#   define _STLP_NO_OWN_IOSTREAMS
//#   undef  _STLP_OWN_IOSTREAMS
//#define _STLP_OWN_IOSTREAMS
#define _STLP_USE_NEW_IOSTREAMS
#   define _STLP_USE_NEWALLOC
//#   define _STLP_NO_NODE_ALLOC
#   define _STLP_NO_LONG_DOUBLE
#   define _STLP_BIG_ENDIAN
// this one causes recursive abs() calls
#   define _STLP_LABS
#   define _STLP_LDIV
#   define _STLP_CONST_CONSTRUCTOR_BUG
// #   define _STLP_HAS_NAMESPACES

//#define _STLP_NO_CSTD_FUNCTION_IMPORTS 


//#warning ********** COMPILER SETTINGS **********
// -----------------------------------------------------------------------------
//  wstring support
//  This is a bit confusing.
//  * _STLP_NO_WCHAR_T inhibits the standard C wchar functions (Symbian doesn't 
//    have them).
//  * _STLP_HAS_WCHAR_T triggers the typedef of wstring.  It's based on wint_t,
//    which isn't typedef's elsewhere, so we take care of it here.
// -----------------------------------------------------------------------------
//#define _STLP_NO_WCHAR_T  1
#define _STLP_HAS_WCHAR_T 1
//typedef unsigned int wint_t;
#  include <wchar.h>

#ifdef _WCHAR_T
#warning ********** _WCHAR_T is defined **********
#endif

#define _STLP_COMPLEX_SPECIALIZATION_BUG
#define _STLP_USE_EXCEPTIONS
#define _STLP_EXTERN_RANGE_ERRORS
//#define _STLP_NO_FORCE_INSTANTIATE
//#define __BUILDING_STLPORT
#  define _STLP_NO_NEW_NEW_HEADER 1
#  define _STLP_USE_DECLSPEC
#define _STLP_LIBSTD_CPP_NO_STATIC_VAR_

#endif // _STLP_RVCT_H
