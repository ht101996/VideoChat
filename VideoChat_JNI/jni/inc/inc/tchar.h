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
 * File Name:       tchar.h
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

#if !defined (_STLP_OUTERMOST_HEADER_ID)
#  define _STLP_OUTERMOST_HEADER_ID 0x500100
#  include <stl/_prolog.h>
#elif (_STLP_OUTERMOST_HEADER_ID == 0x500100) && !defined (_STLP_DONT_POP_HEADER_ID)
#  define _STLP_DONT_POP_HEADER_ID
#endif

#if (defined(WIN32) || defined (_WIN32) || defined (__WIN32)) && (!defined (__SYMBIAN32__))
# include _STLP_NATIVE_C_HEADER(tchar.h)
#endif

#if (_STLP_OUTERMOST_HEADER_ID == 0x500100)
#  if ! defined (_STLP_DONT_POP_HEADER_ID)
#    include <stl/_epilog.h>
#    undef  _STLP_OUTERMOST_HEADER_ID
#  else
#    undef  _STLP_DONT_POP_HEADER_ID
#  endif
#endif

/*
 Local Variables:
 mode:C++
 End:
*/

