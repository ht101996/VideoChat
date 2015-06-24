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
 * File Name:       pos_md5.h
 *
 * Reference:
 *
 * Author:          Kevin Hua
 *
 * Description:
 *      md5 interface.
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

#ifndef _POS_MD5_H_
#define _POS_MD5_H_

#include "pos_prolog.h"
#include "pos_type.h"
#include <stdarg.h>

#ifdef __cplusplus
extern "C"
{
#endif

POS_API Void * md5_buffer(CONST Sint8 *buf_ptr, Uint32 len, Void *resblock);

#define pos_hash_md5(buff, len, output)   md5_buffer((buff), (len), (output))
#define ps_hash_md5_buffer(buff, len, output)   md5_buffer((buff), (len), (output))

/**
 * Helper macro to calculate the approximate length required for base256 to
 * base64 conversion.
 */
#define POS_BASE256_TO_BASE64_LEN(len)	(len * 4 / 3 + 3)

/**
 * Helper macro to calculate the approximage length required for base64 to
 * base256 conversion.
 */
#define POS_BASE64_TO_BASE256_LEN(len)	(len * 3 / 4)


/**
 * Encode a buffer into base64 encoding.
 *
 * @param input	    The input buffer.
 * @param in_len    Size of the input buffer.
 * @param output    Output buffer. Caller must allocate this buffer with
 *		    the appropriate size.
 * @param out_len   On entry, it specifies the length of the output buffer. 
 *		    Upon return, this will be filled with the actual
 *		    length of the output buffer.
 *
 * @return	    POS_OK on success.
 */
Sint32 pos_base64_encode(CONST Uint8 *input, Sint32 in_len,
				     Sint8 *output, Sint32 *out_len);


/**
 * Decode base64 string.
 *
 * @param input	    Input string.
 * @param out	    Buffer to store the output. Caller must allocate
 *		    this buffer with the appropriate size.
 * @param out_len   On entry, it specifies the length of the output buffer. 
 *		    Upon return, this will be filled with the actual
 *		    length of the output.
 * @return	    POS_OK on success.
 */
Sint32 pos_base64_decode(CONST Sint8 *input, Sint32 in_len, 
				      Uint8 *out, Sint32 *out_len);


#ifdef __cplusplus
}
#endif


#endif // _POS_MD5_H_
