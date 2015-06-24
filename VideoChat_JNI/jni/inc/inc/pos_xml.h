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
 * File Name:       pos_xml.h
 *
 * Reference:
 *
 * Author:          Kevin Hua
 *
 * Description:
 *      XML interface.
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

#ifndef _POS_XML_H_
#define _POS_XML_H_

#include "pos_prolog.h"
#include "pos_type.h"

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef XML_UNICODE_WCHAR_T
#define XML_UNICODE
#endif

#ifdef XML_UNICODE     /* Information is UTF-16 encoded. */
#ifdef XML_UNICODE_WCHAR_T
typedef wchar_t TXML_Char;
typedef wchar_t TXML_LChar;
#else
typedef Uint16 TXML_Char;
typedef Sint8 TXML_LChar;
#endif /* XML_UNICODE_WCHAR_T */
#else                  /* Information is UTF-8 encoded. */
typedef Sint8 TXML_Char;
typedef Sint8 TXML_LChar;
#endif /* XML_UNICODE */

typedef Void *Tpos_xml_parser;

/*
 *    Name: pos_xml_start_element_hdl
 *    Description:
 *          element start handler function.
 *    Parameters:
 *          user_data_ptr : user data buffer, reserved.
 *          ele_name_ptr : element name pointer, 0 terminated.
 *          ele_atts_ptr : atts is array of name/value pairs, terminated by 0;
 *    Return:
 *          None;
 *    Remark:
 *
 */

typedef Void (*pos_xml_start_element_handler) (Void *user_data_ptr,
                                                 CONST TXML_Char *ele_name_ptr,
                                                 CONST TXML_Char **ele_atts_ptr);

/*
 *    Name: pos_xml_end_element_hdl
 *    Description:
 *          element end handler function.
 *    Parameters:
 *          user_data_ptr : user data buffer, reserved.
 *          ele_name_ptr : element name pointer, 0 terminated.
 *    Return:
 *          None;
 *    Remark:
 *
 */
typedef Void (*pos_xml_end_element_handler) (Void *user_data_ptr,
                                               CONST TXML_Char *ele_name_ptr);

/*
 *    Name: pos_xml_character_data_hdl
 *    Description:
 *          character data handler function, i.e element value.
 *    Parameters:
 *          user_data_ptr : user data buffer, reserved.
 *          str : str is not 0 terminated.
 *          len: string length.
 *    Return:
 *          None;
 *    Remark:
 *
 */
typedef Void (*pos_xml_character_data_handler)(Void *user_data_ptr,
                                                CONST TXML_Char *str,
                                                Sint32 len);

/*
 *    Name: xml_parser_create
 *    Description:
 *          Constructs a new parser; encoding is the encoding specified by the
 *      external protocol or NULL if there is none specified.
 *    Parameters:
 *          encoding : specified the encoding.
 *    Return:
 *          None;
 *    Remark:
 *
 */
POS_API Tpos_xml_parser pos_xml_parser_create(const TXML_Char *encoding);

/*
 *    Name: pos_xml_set_element_handler
 *    Description:
 *          Set handlers for start and end tags. 
 *    Parameters:
 *          p : parser pointer.
 *          start : element start handler.
 *          end : element end handler.
 *    Return:
 *          None;
 *    Remark:
 *
 */
POS_API Void pos_xml_set_element_handler(Tpos_xml_parser p,
                        pos_xml_start_element_handler start,
                        pos_xml_end_element_handler end);


/*
 *    Name: pos_xml_set_character_data_handler
 *    Description:
 *          Set a text handler. 
 *    Parameters:
 *          p : parser pointer.
 *          char_handler : character data handler.
 *    Return:
 *          None;
 *    Remark:
 *
 */
POS_API Void pos_xml_set_character_data_handler(Tpos_xml_parser p,
                            pos_xml_character_data_handler char_handler);

POS_API Void pos_xml_set_user_data(Tpos_xml_parser p, Void *user_data_ptr);

/*
 *    Name: pos_xml_parse
 *    Description:
 *          Parse one buffer string of the document 
 *    Parameters:
 *          p : parser pointer.
 *          str : string of the document.
 *          length : data length.
 *          if_final: informs the parser that this is the last piece of
 *          the document. Frequently, the last piece is empty (i.e. len is zero.)
 *    Return:
 *          0: success;
 *          -1: error.
 *    Remark:
 *
 */
POS_API Sint32 pos_xml_parse(Tpos_xml_parser p,
                                CONST Sint8 *str,
                                Sint32 len,
                                Sint32 is_final);

/*
 *    Name: pos_xml_parser_close
 *    Description:
 *          Free memory used by the parser. 
 *    Parameters:
 *          p : parser pointer.
 *    Return:
 *          None;
 *    Remark:
 *
 */
POS_API Void pos_xml_parser_close(Tpos_xml_parser p);


/*
 *    Name: pos_xml_get_current_line_number
 *    Description:
 *          Return the line number of the position. 
 *    Parameters:
 *          p : parser pointer.
 *    Return:
 *          >0;
 *    Remark:
 *
 */
POS_API Sint32 pos_xml_get_current_line_number(Tpos_xml_parser p);


/*
 *    Name: pos_xml_get_error
 *    Description:
 *          Return a string describing the error corresponding to code.
 *    Parameters:
 *          p : parser pointer.
 *    Return:
 *          error string;
 *    Remark:
 *
 */
POS_API CONST TXML_LChar * pos_xml_get_error(Tpos_xml_parser p);



#ifdef __cplusplus
}
#endif

#endif // _POS_XML_H_
