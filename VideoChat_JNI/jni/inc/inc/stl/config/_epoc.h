/*
 * © Portions copyright (c) 2006-2007 Nokia Corporation.  All rights reserved.
 *
 * Copyright (c) 1997
 * Moscow Center for SPARC Technology
 *
 * Copyright (c) 1999 
 * Boris Fomitchev
 *
 * This material is provided "as is", with absolutely no warranty expressed
 * or implied. Any use is at your own risk.
 *
 * Permission to use or copy this software for any purpose is hereby granted 
 * without fee, provided the above notices are retained on all copies.
 * Permission to modify the code and to distribute modified code is granted,
 * provided the above notices are retained, and a notice that the code was
 * modified is included with the above copyright notice.
 *
 */

/*
 * Purpose of this file :
 *
 * A list of COMPILER-SPECIFIC portion of STLport settings.
 * This file is provided to help in manulal configuration
 * of STLport. This file is being included by stlcomp.h 
 * when STLport is unable to identify your compiler.
 * Please remove the error diagnostic below before adjusting 
 * macros.
 * 
 */
# ifndef _STLP_EPOC_H
#  define  _STLP_EPOC_H

// NON-STANDARD!!!!

#if defined(__WINS__)
#  pragma warning ( disable : 4018 4097 4100 4127 4244 4284 4511 4512 4514 4786 4800 ) //4355 4284  4231 4511 4512 4097 4786 4800 4018 4146 4244 4514 4127 4100 4663)
#  define _STLP_NO_FUNCTION_TMPL_PARTIAL_ORDER 1
// Uncomment if partial order of template functions is not available
#  define _STLP_NO_FUNC_PARTIAL_ORDERING 1
#endif
/*
#ifndef _STLP_NO_IOSTREAMS
#  define _STLP_NO_IOSTREAMS	               1
#endif

#  undef  _STLP_OWN_IOSTREAMS
#  define _STLP_NO_NATIVE_MBSTATE_T            1
//#  define _NOTHREADS                           1
#  define _STLP_NO_TYPEINFO                    1
*/
#ifndef __PLACEMENT_NEW_INLINE
#ifndef __E32STD_H__
inline void * operator new(unsigned int, void *_P) { return (_P); }
#define __PLACEMENT_NEW_INLINE
#endif // E32STD_H
#endif

//==========================================================

// the values choosen here as defaults try to give
// maximum functionality on the most conservative settings

// Mostly correct guess, change it for Alpha (and other environments
// that has 64-bit "long")
#  define _STLP_UINT32_T unsigned long

// Disables wchar_t functinality
#  define _STLP_NO_WCHAR_T  1

// Define if wchar_t is not a unique type, and is actually a typedef to unsigned short. 
// #  define _STLP_WCHAR_T_IS_USHORT 1

// Uncomment if long long is available
// #  define _STLP_LONG_LONG long long

// Uncomment if long double is not available
// #  define _STLP_NO_LONG_DOUBLE 1

// Uncomment this if your compiler does not support namespaces 
#  define _STLP_HAS_NO_NAMESPACES 1

// Uncomment if "using" keyword does not work with template types 
// # define _STLP_BROKEN_USING_DIRECTIVE 1

// Uncomment this if your compiler does not support exceptions
#  define _STLP_HAS_NO_EXCEPTIONS 1

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

// Edit relative path below (or put full path) to get native 
// compiler headers included. Default is "../include".
// C headers may reside in different directory, so separate macro is provided.
// Hint : never install STLport in the directory that ends with "include"
// # define _STLP_NATIVE_INCLUDE_PATH ../include
# define _STLP_NATIVE_C_INCLUDE_PATH libc
# define _STLP_NATIVE_INCLUDE_PATH libc

//==========================================================
# endif
