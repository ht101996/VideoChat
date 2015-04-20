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
 * File Name:       pos_string.h
 *
 * Reference:
 *
 * Author:          Kevin Hua
 *
 * Description:
 *      Redefine character and memory interface in string.h.
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

#ifndef _POS_STRING_H_
#define _POS_STRING_H_

#include "pos_prolog.h"
#include <string.h>
#include "pos_stdlib.h"
#include "pos_type.h"


#ifdef M_WIDE_CHAR
#define TChar WChar
#else 
#define TChar Sint8
#endif

#ifdef __cplusplus
extern "C" {
#endif


#if defined (__WIN32__)
//#   define wcsncmp      the same
#   define stricmp      _stricmp
#   define strnicmp     _strnicmp

#   define wcsicmp      _wcsicmp
#   define wcsnicmp     _wcsnicmp
#elif defined (__LINUX__)
//#   define wcsncmp      the same
#   define stricmp      strcasecmp
#   define strnicmp     strncasecmp

#   define wcsicmp      wcscasecmp
#   define wcsnicmp     wcsncasecmp
#elif defined (__SYMBIAN32__)
#   define stricmp      _pos_stricmp
#   define strnicmp     _pos_strnicmp

#   define wcsncmp      _pos_wcsncmp
#   define wcsicmp      _pos_wcsicmp
#   define wcsnicmp     _pos_wcsnicmp
#   define wcsstr       _pos_wcsstr
#   define wcschr       _pos_wcschr
#   define wcsrchr      _pos_wcsrchr
#endif

POS_API int _pos_stricmp(const char *s1, const char *s2);
POS_API int _pos_strnicmp(const char *s1, const char *s2, size_t n);

POS_API int _pos_wcsncmp(const wchar_t *s1, const wchar_t *s2, size_t n);
POS_API int _pos_wcsicmp(const wchar_t *s1, const wchar_t *s2);
POS_API int _pos_wcsnicmp(const wchar_t *s1, const wchar_t *s2, size_t n);
POS_API wchar_t *_pos_wcsstr(const wchar_t *s1, const wchar_t *s2);
POS_API wchar_t *_pos_wcschr(const wchar_t *s, wchar_t c);
POS_API wchar_t *_pos_wcsrchr(const wchar_t *s, wchar_t c);

POS_API int pos_char2wchar(char * src_ptr, wchar_t * dst_ptr, size_t dst_len);
POS_API int pos_wchar2char(wchar_t * src_ptr, char * dst_ptr, size_t dst_len);
POS_API int pos_utf8_2_unicode(unsigned char* src_ptr, wchar_t* dst_ptr, size_t dst_len);
POS_API int pos_unicode_2_utf8(wchar_t* src_ptr, unsigned char* dst_ptr, size_t dst_len);



#if defined(__SYMBIAN32__)
	///////////////single char/////////////////
	#define pos_scscasecmp					strcasecmp
	#define pos_scslen		                strlen
	#define pos_scsstr		                strstr
	#define pos_scscat			            strcat
	#define pos_scscmp			            strcmp
	#define pos_scschr			            strchr
	#define pos_scscpy			            strcpy
	#define pos_scsncpy				        strncpy
	#define pos_scsncmp			            strncmp
	#define pos_satoi						atoi
	#define pos_satol						atol
	#define pos_ssprintf                    sprintf
	#define pos_scsncasecmp				    strncasecmp
	///////////////wide char/////////////////
	#define _MTEXT(x)					    L##x
	#define pos_wcslen		                wcslen
	#define pos_wcscat			            wcscat
	#define pos_wcscmp			            wcscmp
	#define pos_wcscpy			            wcscpy
	#define pos_watol						pos_watoi
	#define pos_wsprintf                   
#ifdef M_WIDE_CHAR
	POS_API Sint32	pos_watoi(CONST WChar *string);
	POS_API WChar*	pos_wcsstr(CONST WChar *string, CONST WChar *strCharSet);
	POS_API WChar*	pos_wcschr(CONST WChar *string, WChar c);
	POS_API WChar*  pos_wcsncpy(WChar *strDest, CONST WChar *strSource, Uint32 count);
	POS_API Sint32	pos_wcsncmp(CONST WChar *string1, CONST WChar *string2, Uint32 count);
	POS_API Sint32	pos_wcsicmp(CONST WChar *string1, CONST WChar *string2);
	POS_API Sint32	pos_wcsnicmp(CONST WChar *string1, CONST WChar *string2, Uint32 count);
#endif
#elif defined (_WINCE_) || defined(__WIN32__) || defined(_PURE_ADS_)
	///////////////single char/////////////////
	#define pos_scscasecmp					_stricmp
	#define pos_scslen		                strlen
	#define pos_ssprintf                    sprintf
	#define pos_scsstr		                strstr
	#define pos_scscat			            strcat
	#define pos_scscmp			            strcmp
	#define pos_scschr			            strchr
	#define pos_scscpy			            strcpy
	#define pos_scsncpy				        strncpy
	#define pos_scsncmp			            strncmp
	#define pos_satoi						atoi
	#define pos_satol						atol
	#define pos_scsncasecmp					_strnicmp
	///////////////wide char/////////////////
	#define _MTEXT(x)					    L##x
	#define pos_wcsicmp					    _wcsicmp
	#define pos_wcslen		                wcslen
	#define pos_wsprintf                    swprintf
	#define pos_wcsstr		                wcsstr
	#define pos_wcscat			            wcscat
	#define pos_wcscmp			            wcscmp
	#define pos_wcschr			            wcschr
	#define pos_wcscpy			            wcscpy
	#define pos_wcsncpy				        wcsncpy
	#define pos_wcsncmp			            wcsncmp
	#define pos_watoi						_wtoi
	#define pos_watol						_wtol
	#define pos_wcsnicmp					_wcsnicmp
#elif defined (_PALM_OS_)
	///////////////single char/////////////////
	#define	pos_scscpy						StrCopy
	#define pos_scsncpy					    StrNCopy
	#define pos_scscat						StrCat
	#define pos_scslen						StrLen
	#define pos_scscmp						StrCompare
	#define pos_scsncmp					    StrNCompare
	#define pos_scscasecmp					StrCaselessCompare
	#define pos_scsstr						StrStr
	#define pos_scschr						StrChr
	#define pos_ssprintf					StrPrintF
	#define	pos_satol						atoi
	#define pos_scsncasecmp					StrNCaselessCompare
	Sint32	pos_satoi(Sint8 *string);
#elif defined _BREW_OS_
	///////////////single char/////////////////
	#define pos_scslen		                STRLEN
	#define pos_ssprintf                    SPRINTF
	#define pos_scsstr		                STRSTR
	#define pos_scscat			            STRCAT
	#define pos_scscmp			            STRCMP
	#define pos_scschr			            STRCHR
	#define pos_scscpy			            STRCPY
	#define pos_scsncpy				        STRNCPY
	#define pos_scsncmp			            STRNCMP
	#define pos_scscasecmp					STRICMP
	#define pos_satoi						ATOI
	#define pos_satol						ATOI
	#define pos_scsncasecmp			        STRNICMP
#elif defined(__LINUX__)
	///////////////single char/////////////////
	#define pos_scscasecmp					strcasecmp
	#define pos_scslen		                strlen
	#define pos_ssprintf                    sprintf
	#define pos_scsstr		                strstr
	#define pos_scscat			            strcat
	#define pos_scscmp			            strcmp
	#define pos_scschr			            strchr
	#define pos_scscpy			            strcpy
	#define pos_scsncpy				        strncpy
	#define pos_scsncmp			            strncmp
	#define pos_satoi						atoi
	#define pos_satol						atol
	#define pos_scsncasecmp					strncasecmp
	///////////////wide char/////////////////
	#define _MTEXT(x)					    L##x
	#define pos_wcsicmp					    wcscasecmp
	#define pos_wcslen		                wcslen
	#define pos_wsprintf                    swprintf
	#define pos_wcsstr		                wcsstr
	#define pos_wcscat			            wcscat
	#define pos_wcscmp			            wcscmp
	#define pos_wcschr			            wcschr
	#define pos_wcscpy			            wcscpy
	#define pos_wcsncpy				        wcsncpy
	#define pos_wcsncmp			            wcsncmp
	#define pos_watoi						wcstol
	#define pos_watol						wcstol
	#define pos_wcsnicmp					wcsncasecmp
#endif	//#if defined(__SYMBIAN32__)


#if defined(__SYMBIAN32__) || defined (_WINCE_) || defined(__WIN32__) || defined(__LINUX__)
	#ifdef				M_WIDE_CHAR
		#define _MMT(x)						_MTEXT(x)
		#define pos_strlen		                pos_wcslen
		#define pos_strcat			            pos_wcscat
		#define pos_strcmp			            pos_wcscmp
		#define pos_strcpy			            pos_wcscpy
		#define pos_atol						pos_watoi
		#define	pos_atoi						pos_watoi
		#define	pos_strstr						pos_wcsstr
		#define	pos_strchr						pos_wcschr
		#define pos_strncpy					pos_wcsncpy
		#define	pos_strncmp					pos_wcsncmp
		#define	pos_stricmp					pos_wcsicmp
		#define pos_sprintf					pos_wsprintf
		#define	pos_strnicmp					pos_wcsnicmp
	#else	//ifndef M_WIDE_CHAR
		#define _MMT(x)						(x)
		#define pos_strlen		                pos_scslen
		#define pos_strcat			            pos_scscat
		#define pos_strcmp			            pos_scscmp
		#define pos_strcpy			            pos_scscpy
		#define pos_atol						pos_satoi
		#define	pos_atoi						pos_satoi
		#define	pos_strstr						pos_scsstr
		#define	pos_strchr						pos_scschr
		#define pos_strncpy					pos_scsncpy
		#define	pos_strncmp					pos_scsncmp
		#define	pos_stricmp					pos_scscasecmp
		#define pos_sprintf                    pos_ssprintf
		#define	pos_strnicmp					pos_scsncasecmp
	#endif //#ifdef M_WIDE_CHAR
#elif defined (_PALM_OS_) || defined (_BREW_OS_)
		#define _MMT(x)						(x)
		#define pos_strlen		                pos_scslen
		#define pos_strcat			            pos_scscat
		#define pos_strcmp			            pos_scscmp
		#define pos_strcpy			            pos_scscpy
		#define pos_atol						pos_satoi
		#define	pos_atoi						pos_satoi
		#define	pos_strstr						pos_scsstr
		#define	pos_strchr						pos_scschr
		#define pos_strncpy					pos_scsncpy
		#define	pos_strncmp					pos_scsncmp
		#define	pos_stricmp					pos_scscasecmp
		#define pos_sprintf                    pos_ssprintf
		#define	pos_strnicmp					pos_scsncasecmp
#endif //defined(__SYMBIAN32__) || defined (_WINCE_) || defined(__WIN32__)




#ifdef __cplusplus
}
#endif

#endif // _POS_STRING_H_
