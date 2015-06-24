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
 * File Name:       pos_ini.h
 *
 * Reference:
 *
 * Author:          Kevin Hua
 *
 * Description:
 *      Base INI file interface.
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

#ifndef _POS_INI_H_
#define _POS_INI_H_

#include "pos_prolog.h"
#include "pos_type.h"

#define		POS_INI_INT		0x00000001
#define		POS_INI_STRING		0x00000002

#ifdef __cplusplus
extern "C" {
#endif


typedef	struct _t_ini_file_s {
	Sint8*		category;		//value category
	Sint8*		value_name;	    //value name
	Void*		data_ptr;		//the point of the data
	Uint32		data_type;		//the type of data_ptr
	Uint32		data_count;	    //the length of data_ptr
}Tpos_ini_paras;

#ifdef  M_WIDE_CHAR
typedef	struct _t_ini_file_w {
	WChar*		category;		//value category
	WChar*		value_name;	    //value name
	Void*		data_ptr;		//the point of the data
	Uint32		data_type;		//the type of data_ptr
	Uint32		data_count;	    //the length of data_ptr
}Tpos_ini_paraw;

#endif

#ifdef		M_WIDE_CHAR
	#define pos_ini_start		pos_ini_startw
	#define pos_ini_end			pos_ini_endw
	#define pos_ini_write		pos_ini_writew
	#define pos_ini_read		pos_ini_readw
	#define pos_ini_remove		pos_ini_removew
	#define pos_ini_writesingle	pos_ini_writesinglew
	#define pos_ini_readsingle	pos_ini_readsinglew
	#define	Tpos_ini_para		Tpos_ini_paraw
    #define pos_ini_path        pos_ini_pathw
#else
	#define pos_ini_start		pos_ini_starts
	#define pos_ini_end			pos_ini_ends
	#define pos_ini_write		pos_ini_writes
	#define pos_ini_read		pos_ini_reads
	#define pos_ini_remove		pos_ini_removes
	#define pos_ini_writesingle	pos_ini_writesingles
	#define pos_ini_readsingle	pos_ini_readsingles
	#define	Tpos_ini_para		Tpos_ini_paras
    #define pos_ini_path        pos_ini_paths
#endif

/**
 *	pos_ini_start
 *			   get a POS_HANDLE HANDLE for read or write an initialization file  
 *	
 *	Parameter:
 *				file_para_ptr	(in)			point to a directory parameter structure, it is specified by the platform 											
 *											if file_para_ptr is NULL, it will to operate registry
 *
 *	Return:
 *				returns POS_NULL if failed , else succeed.
 *
 *	Remark:
 *				if 	pos_ini_start is succeed, user must transfer pos_ini_end.
 */
POS_API POS_HANDLE pos_ini_starts(CONST Void *file_para_ptr); 
#ifdef  M_WIDE_CHAR
POS_API POS_HANDLE pos_ini_startw(CONST Void *file_para_ptr);
#endif

/**
 *	pos_ini_write
 *			    copies data into the specified section of an initialization file   
 *	
 *	Parameter:
 *				ini_hdl		(in)			returned by pos_ini_start 											
 *				ini_para_ptr	(in,out)		point to a Tpos_ini_para structure 											
 *											
 *
 *	Return:
 *				If the function successfully , the return value is POS_TRUE
 *				else return POS_FALSE 
 *
 *	Remark:
 *				if category is NULL,it will point to a default Category.
 *				value_name , data_ptr, data_type, data_count is required.
 *				
 */
POS_API Sint32 pos_ini_writes(POS_HANDLE ini_hdl, Tpos_ini_paras * ini_para_ptr);
#ifdef  M_WIDE_CHAR
POS_API Sint32 pos_ini_writew(POS_HANDLE ini_hdl, Tpos_ini_paraw * ini_para_ptr);
#endif

/**
 *	pos_ini_read
 *			    retrieves data from the specified Category in an initialization file  
 *	
 *	Parameter:
 *				ini_hdl		(in)			returned by pos_ini_start 											
 *				ini_para_ptr	(in,out)		point to a Tpos_ini_para structure 											
 *											
 *
 *	Return:
 *				If the function successfully , the return value is POS_TRUE
 *				else return POS_FALSE 
 *
 *	Remark:
 *				if category is NULL,it will point to a default Category.
 *				value_name , data_ptr, data_type, data_count is required. 
 */
POS_API Sint32 pos_ini_reads(POS_HANDLE ini_hdl, Tpos_ini_paras * ini_para_ptr);
#ifdef  M_WIDE_CHAR
POS_API Sint32 pos_ini_readw(POS_HANDLE ini_hdl, Tpos_ini_paraw * ini_para_ptr);
#endif

/**
 *	pos_ini_remove
 *			    Remove data from an initialization file   
 *	
 *	Parameter:
 *				ini_hdl		(in)			returned by pos_ini_start 											
 *				ini_para_ptr	(in,out)		point to a Tpos_ini_para structure 											
 *											
 *
 *	Return:
 *				If the function successfully , the return value is POS_TRUE
 *				else return POS_FALSE 
 *
 *	Remark:
 *				if category is NULL,it will point to a default Category.
 *				if value_name is NULL,it will remove all data in this category, 
 *				
 */
POS_API Sint32 pos_ini_removes(POS_HANDLE ini_hdl, Tpos_ini_paras * ini_para_ptr);
#ifdef  M_WIDE_CHAR
POS_API Sint32 pos_ini_removew(POS_HANDLE ini_hdl, Tpos_ini_paraw * ini_para_ptr);
#endif

/**
 *	pos_ini_end
 *			    end read or write an initialization file
 *	
 *	Parameter:
 *				ini_hdl		(in)			returned by pos_ini_start 											
 *											
 *
 *	Return:
 *
 *	Remark:
 *					
 */
POS_API Void pos_ini_ends(POS_HANDLE ini_hdl);
#ifdef  M_WIDE_CHAR
POS_API Void pos_ini_endw(POS_HANDLE ini_hdl);
#endif

/**
 *	pos_ini_writesingle
 *			    copies data into the specified Category of an initialization file   
 *	
 *	Parameter:
 *				file_para_ptr	(in)			point to a directory parameter structure, it is specified by the platform 											
 *											if file_para_ptr is NULL, it will to operate registry
 *				ini_para_ptr	(in,out)		point to a Tpos_ini_para structure 											
 *											
 *
 *	Return:
 *				If the function successfully , the return value is POS_TRUE
 *				else return POS_FALSE 
 *
 *	Remark:
 *				if category is NULL,it will point to a default Category.
 *				value_name , data_ptr, data_type, data_count is required.
 *				
 */
POS_API Sint32 pos_ini_writesingles(CONST Void *file_para_ptr, Tpos_ini_paras * ini_para_ptr);
#ifdef  M_WIDE_CHAR
POS_API Sint32 pos_ini_writesinglew(CONST Void *file_para_ptr, Tpos_ini_paraw * ini_para_ptr);
#endif

/**
 *	pos_ini_readsingle
 *			    retrieves data from the specified section in an initialization file  
 *	
 *	Parameter:
 *				file_para_ptr	(in)			point to a directory parameter structure, it is specified by the platform 											
 *											if file_para_ptr is NULL, it will to operate registry
 *				ini_para_ptr	(in,out)		point to a Tpos_ini_para structure 											
 *											
 *
 *	Return:
 *				If the function successfully , the return value is POS_TRUE
 *				else return POS_FALSE 
 *
 *	Remark:
 *				if category is NULL,it will point to a default Category.
 *				value_name , data_ptr, data_type, data_count is required.
 */
POS_API Sint32 pos_ini_readsingles(CONST Void *file_para_ptr, Tpos_ini_paras * ini_para_ptr);
#ifdef  M_WIDE_CHAR
POS_API Sint32 pos_ini_readsinglew(CONST Void *file_para_ptr, Tpos_ini_paraw * ini_para_ptr);
#endif


#if	defined(_WINCE_ )||defined(__SYMBIAN32__)
/**
 *	pos_ini_find_next_category
 *			    find next category from an initialization file   
 *	
 *	Parameter:
 *				ini_hdl		(in)			returned by pos_ini_start 											
 *				category	(in,out)		point to a buffer to get category 											
 *				buf_size_ptr					point to buffer size		
 *
 *	Return:
 *				If the function successfully , the return value is POS_TRUE
 *				else return POS_FALSE 
 *
 *	Remark:
 *				if return POS_FALSE, include two types, one is have not more,
 *				the other is buffer to small, and buf_size_ptr will be set to wanted buff size.  
 *				
 */
#ifdef		M_WIDE_CHAR
	#define		pos_ini_find_next_category	pos_ini_find_next_categoryW
	#define		pos_ini_find_first_category	pos_ini_find_first_categoryW
#else
	#define		pos_ini_find_next_category	pos_ini_find_next_categoryS
	#define		pos_ini_find_first_category	pos_ini_find_first_categoryS
#endif
POS_API Sint32 pos_ini_find_next_categoryS(POS_HANDLE ini_hdl, Sint8* category, Sint32* buf_size_ptr);
#ifdef  M_WIDE_CHAR
POS_API Sint32 pos_ini_find_next_categoryW(POS_HANDLE ini_hdl, WChar* category, Sint32* buf_size_ptr);
#endif

POS_API Sint32 pos_ini_find_first_categoryS(POS_HANDLE ini_hdl, Sint8* category, Sint32* buf_size_ptr);
#ifdef  M_WIDE_CHAR
POS_API Sint32 pos_ini_find_first_categoryW(POS_HANDLE ini_hdl, WChar* category, Sint32* buf_size_ptr);
#endif

#endif

POS_API Sint32 pos_ini_paths(CONST Sint8 *file_path_ptr, CONST Sint8 *file_name_ptr, Sint8 *file_pathname_ptr);
//POS_API Sint32 pos_ini_pathw(CONST Void *fild_path_ptr, Void *file_name_ptr);


#ifdef __cplusplus
}
#endif

#endif // _POS_INI_H_
