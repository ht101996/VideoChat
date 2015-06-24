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
 * File Name:       pos_prolog.h
 *
 * Reference:
 *
 * Author:          Kevin Hua
 *
 * Description:
 *      Prolog file for precompile.
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

#ifndef _POS_PROLOG_H_
#define _POS_PROLOG_H_


#include "stl/_prolog.h"
#include "stl/_epilog.h"

#include "pos_user.h"

/****************************************************************************************************
 *                                                                                                  *
 *                                Products Definition                                               *
 *                                                                                                  *
 *  __HARDWARE_PLATFORM_EM__        Embedded, such as F200, 5VT, Router                             *
 *  __HARDWARE_PLATFORM_DE__        Digital Equipment, such as Digital Photo Frame                  *
 *  __HARDWARE_PLATFORM_PC__        Personal Computer                                               *
 *  __HARDWARE_PLATFORM_MB__        Mobile, such as Symbian Phone, Windows Mobile Phone             *
 ****************************************************************************************************/

/****************************************************************************************************
 *                                                                                                  *
 *                                Operation System Defination                                       *
 *                                                                                                  *
 *  __WIN32__                       Windows                                                         *
 *  __LINUX__                       Linux, ucLinux                                                  *
 *  __SYMBIAN32__                   Symbian                                                         *
 *  __WINCE__                       Windows Mobile                                                  *
 *  __ANDROID__                     Google Android SDK                                              *
 ****************************************************************************************************/

/****************************************************************************************************
 *                                                                                                  *
 *                                Unicode Definition                                                *
 *                                                                                                  *
 *  __UNICODE__                     Enable Uincode                                                  *
 ****************************************************************************************************/

/****************************************************************************************************
 *                                                                                                  *
 *                                Virtual Address Definition                                        *
 *                                                                                                  *
 *  __UUID_16__                     Using 16 Bytes for UUID, is Default                             *
 *  __UUID_20__                     Using 20 Bytes for UUID                                         *
 ****************************************************************************************************/

/****************************************************************************************************
 *                                                                                                  *
 *                                Version                                                           *
 *                                                                                                  *
 *  __DEBUG__                       Enable Debug                                                    *
 *  __LOG__                         Enable Log, POS_LOG_FILE                                        *
 *  __SECURITY__                    Enable Security                                                 *
 *  __SECURITY_CHIP__               Enable Security Chip                                            *
 *  __SECURITY_LOAD_DSP__           Enable Load Form Securiy                                        *
 *  __PRODUCT__                     Enable Product                                                  *
 *  __STATIC_MEMORY__               Enable Memory Pool                                              *
 ****************************************************************************************************/

/****************************************************************************************************
 *                                                                                                  *
 *                                Watch Dog Definition                                              *
 *                                                                                                  *
 *  __WATCHDOG_HDW__                Enable hadware watch dog                                        *
 *  __WATCHDOG_SFW__                Enable software watch dog                                       *
 ****************************************************************************************************/

/****************************************************************************************************
 *                                                                                                  *
 *                                Product Line Definition                                           *
 *                                                                                                  *
 *  __PRODUCT_F__                   F Product Line                                                  *
 *  __PRODUCT_FREE_PP__             FreePP Product Line                                             *
 *  __PRODUCT_T__                   T Product Line                                                  *
 *  __PRODUCT_U__                   U Product Line                                                  *
 *  __PRODUCT_M__                   M Product Line                                                  *
 ****************************************************************************************************/

/****************************************************************************************************
 *                                                                                                  *
 *                                Service Definition                                                *
 *                                                                                                  *
 *  __SERVICE__                     Support Service Feature                                         *
 ****************************************************************************************************/

/****************************************************************************************************
 *                                                                                                  *
 *                                Program Type                                                      *
 *                                                                                                  *
 *  __PG_CONSOLE__                  Console Program                                                 *
 *  __PG_GUI__                      GUI Program                                                     *
 ****************************************************************************************************/

// product definition by user
#include "pos_product.h"

#if defined (__sun)
#  if defined (__GNUC__)
    // gcc
#  elif defined (__SUNPRO_CC)
    // sunprocc
#  elif defined (__APOGEE__)
    // Apogee 4.x
#  elif defined (__FCC_VERSION)
    // Fujitsu Compiler, v4.0 assumed
#  endif
#elif defined (__hpux)
    //
#  if defined (__GNUC__)
    // gcc
#  elif defined (__HP_aCC)
    //hpacc
#  endif
#elif defined (linux) || defined (__linux__)
#  if defined (__GNUC__) && defined (arm) && defined (__ELF__) && (__GNUC__ == 2) && (__GNUC_MINOR__ == 95)
    // arm-elf-gcc2.95.3 for F200 product.
//#    define _XOPEN_SOURCE 500
#    if !defined (__PRODUCT_F__)
#       define __PRODUCT_F__ 1
#    endif
#  elif defined (__GNUC__) && (__GNUC__ == 3) && (__GNUC_MINOR__ == 3)
#    if !defined (__PRODUCT_FREE_PP__)
#       define __PRODUCT_FREE_PP__ 1
#    endif
#  elif defined (__GNUC__) && (__GNUC__ == 4) && (__GNUC_MINOR__ == 1)
#  elif defined (__INTEL_COMPILER)
    // intel's icc
#  elif defined (__GNUC__)
    // gcc
#  endif
#  if !defined (__LINUX__)
#    define __LINUX__
#  endif
#elif defined (__FreeBSD__)
    //
#  if defined (__GNUC__)
    // gcc
#  endif
#  if !defined (__LINUX__)
#    define __LINUX__
#  endif
#elif defined (__OpenBSD__)
    //
#  if defined (__GNUC__)
    // gcc
#  endif
#  if !defined (__LINUX__)
#    define __LINUX__
#  endif
#elif defined (N_PLAT_NLM)
    //
#  ifdef __MWERKS__ 
    // Metrowerks CodeWarrior
#  endif
#elif defined (__sgi)
    // SGI Irix
#  if defined (__GNUC__)
    // gcc
#  else
    // sgi
#  endif
#elif defined (__OS400__)
    // AS/400 C++
#  if defined (__GNUC__)
    // gcc
#  else
    // as400
#  endif
#elif defined (_AIX)
    //
#  if defined (__xlC__) || defined (__IBMC__) || defined ( __IBMCPP__ )
    // AIX xlC, Visual Age C++ , OS-390 C++
#  endif
#elif defined (_CRAY)
    // Cray C++ 3.4 or 3.5
#elif defined (__DECCXX) || defined (__DECC)
    // DECC
#  ifdef __vms
    // vms
#  else
    // dec
#  endif
#elif defined (macintosh) || defined (_MAC)
    // mac
#  if defined (__MWERKS__)
    // mwerks
#  elif defined (__MRC__) || (defined (__SC__) && (__SC__ >= 0x882))
    // Apple MPW SCpp 8.8.2, Apple MPW MrCpp 4.1.0
#  endif
#elif defined (__APPLE__)
    // macosx
#  ifdef __GNUC__
    // gcc
#  endif
#elif defined (__CYGWIN__)
    // cygwin
#  if defined (__GNUC__)
    // gcc
#  endif
#elif defined (__MINGW32__)
    // WinGW
#  if defined (__GNUC__)
    // gcc
#  endif
# elif defined(__SYMBIAN32__)
    // Symbian Platform
#  if !defined (__PRODUCT_M__)
#   define __PRODUCT_M__ 1
#  endif

#  if defined(__SERIES60_30__)
#   define POS_S60MR        // symbian s60 3rd
#   define S60MR_SWITCH
#  elif defined(__SERIES60_31__)
#   define POS_S60FP1       // symbian s60 3rd fp1
#   define S60FP1_SWITCH
#  elif defined(__SERIES60_31__)
#   define POS_S60FP2       // symbian s60 3rd fp2
#   define S60FP2_SWITCH
#  endif

#  if defined(EKA2) && defined (__GCCE__)
    // symbian gcce in mobile
#  elif defined(EKA2) && defined (__WINSCW__)
    // Metrowerks CodeWarrior for Symbian EKA2
#   if !defined(__DEBUG__)
#    define __DEBUG__
#    define __LOG__
#   endif
#  elif defined defined(EKA2) && defined(__ARMCC__)
    // ARM RVCT for Symbian EKA2
#  endif
#elif defined (_WIN32) || defined (__WIN32) || defined (WIN32) || defined (__WIN32__) || defined (__WIN16) || defined (WIN16) || defined (_WIN16)
    // windows
#  if defined ( __BORLANDC__ )
        // Borland C++ ( 4.x - 5.x )
#  elif defined (__WATCOM_CPLUSPLUS__) || defined (__WATCOMC__)
        // watcom c++
#  elif defined (__COMO__) || defined (__COMO_VERSION_)
        // como
#  elif defined (__DMC__)
        // Digital Mars C++
#  elif defined (__SC__) && (__SC__ < 0x800)
        // Symantec 7.5
#  elif defined (__ICL)
        // Intel reference compiler for Win
#  elif defined (__MWERKS__)
        // mwerks
#  elif defined (_MSC_VER) && (_MSC_VER >= 1200) && defined (UNDER_CE)
        // Microsoft eMbedded Visual C++ 3.0, 4.0 (.NET)
#  elif defined (_MSC_VER)
        // Microsoft Visual C++ 6.0, 7.0, 7.1, 8.0
#   include <Winsock2.h>
#   include <windows.h>
#   if !defined (__PRODUCT_T__)
#       define __PRODUCT_T__ 1
#   endif
#   if !defined (__PRODUCT_U__)
#       define __PRODUCT_U__ 1
#   endif
#   if defined (_DEBUG) && !defined(__DEBUG__)
#    define __DEBUG__
#    define __LOG__
#   endif
#       if defined (_WIN32_WINDOWS) && !defined (_WIN32_WINNT) && defined (WINVER)
#           if (WINVER>=0x0500)
#               define POS_WINME            // windows me
#               define WINME_SWITCH
#           elif (WINVER>=0x0410)
#               define POS_WIN98            // windows 98
#               define WIN98_SWITCH
#           elif (WINVER>=0x0400)
#               define POS_WIN95            // windows 95
#               define WIN95_SWITCH
#           endif
#       elif defined(WINVER)// defined(_WIN32_WINNT) or not
#           if (WINVER>=0x0601)
#               define POS_WIN7             // windows 7
#               define WIN7_SWITCH
#           elif (WINVER>=0x0600)
#               define POS_WIN2K8           // windows 2008
#               define POS_VISTA            // windows vista
#               define WIN2K8_SWITCH
#               define VISTA_SWITCH
#           elif (WINVER>=0x0502)
#               define POS_WIN2K3_SP1       // windows server 2003 sp1
#               define POS_WINXP_SP2        // windows xp sp2
#               define WIN2K3_SP1_SWITCH
#               define WINXP_SP2_SWITCH
#           elif (WINVER>=0x0501)
#               define POS_WIN2K3           // windows server 2003
#               define POS_WINXP            // windows xp
#               define WIN2K3_SWITCH
#               define WINXP_SWITCH
#           elif (WINVER>=0x0500)
#               define POS_WIN2K            // windows 2000
#               define WIN2K_SWITCH
#           elif (WINVER>=0x0400)
#               define POS_WINNT            // windows nt 4
#               define WINNT_SWITCH
#           endif
#       endif
#  endif
#  if !defined (__WIN32__)
#    define __WIN32__
#  endif
#elif defined (WINCE)
    // windows mobile
#   if !defined (__PRODUCT_M__)
#       define __PRODUCT_M__ 1
#   endif

#   define POS_WINCE

#   if(_WIN32_WCE == 0x0502) // windows mobile 6.0
#   elif(_WIN32_WCE == 0x0501) // windows mobile 5.0
#   endif

#   if defined(WIN32_PLATFORM_PSPC) // windows pocket pc
#   elif defined(WIN32_PLATFORM_WFSP) // windows smart phone
#   endif

#   if defined (_DEBUG) && !defined(__DEBUG__)
#    define __DEBUG__
#    define __LOG__
#   endif

#  if !defined (__WINCE__)
#    define __WINCE__
#  endif
#elif defined(__ANDROID__)
#  if !defined (__LINUX__)
#    define __LINUX__
#  endif
#  if !defined (__PRODUCT_M__)
#    define __PRODUCT_M__ 1
#  endif
#else
#  error unknown platform.
#endif

#define POS_TLS_MAX             32              // define max tls count
#define POS_THREAD_MAX          512

// features of all product
#include "pos_feature.h"

//#if defined (__HARDWARE_PLATFORM_PC__)
//#ifndef __SERVICE__
//#   define __SERVICE__
//#endif
//#endif

/****************************************************************************************************
 *                                                                                                  *
 *                                  DLL Export and Import Prefix                                    *
 *                                                                                                  *
 *  The actual type have been defined in stl prolog file                                            *
 ****************************************************************************************************/
#ifdef POS_DLL  // create or using dll library
#define POS_EXPORTS_DECLSPEC _STLP_EXPORT_DECLSPEC
#define POS_IMPORT_DECLSPEC _STLP_IMPORT_DECLSPEC
#else
#define POS_EXPORTS_DECLSPEC
#define POS_IMPORT_DECLSPEC
#endif


/****************************************************************************************************
 *                                                                                                  *
 *                                  OS Platform DLL and LIB Switch MARCO                            *
 *                                                                                                  *
 *  POS_DLL             Need create or using dll library.                                           *
 *  POS_POS_EXPORTS     Create a dll library.                                                       *
 *  POS_API             Export os platform intferface, that is a sample for other module            *
 ****************************************************************************************************/
#ifdef POS_DLL  // create or using dll library
#   ifdef POS_EXPORTS   // dll support
#       define POS_API  POS_EXPORTS_DECLSPEC
#       ifdef __SYMBIAN32__
#           define POS_API_IN POS_EXPORTS_DECLSPEC
#       else
#           define POS_API_IN
#       endif
#   else    // dll using
#       define POS_API  POS_IMPORT_DECLSPEC
#       ifdef __SYMBIAN32__
#           define POS_API_IN POS_IMPORT_DECLSPEC
#       else
#           define POS_API_IN
#       endif
#   endif
#else   // create or using static library
#   define POS_API
#   define POS_API_IN
#endif





#endif // _POS_PROLOG_H_

