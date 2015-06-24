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
 * File Name:       pos_file.h
 *
 * Reference:
 *
 * Author:          Kevin Hua
 *
 * Description:
 *      Normal file and memory file support, such as file open, close, write, read and so on.
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

#ifndef _POS_FILE_H_
#define _POS_FILE_H_

#include "pos_prolog.h"
#include "pos_type.h"

#ifdef __cplusplus
extern "C" {
#endif

#define		POS_INVALID_HANDLE  POS_NULL

#define		POS_STREAM_BEGIN	0
#define		POS_STREAM_END		1
#define		POS_STREAM_CUR		2

//Opens for reading. If the file does not exist or cannot be found,call fails.
#define		POS_STREAM_READ		0x0001 
//Opens an empty file for writing. If the given file exists, its contents are destroyed. 
#define		POS_STREAM_WRITE	0x0002 
//Opens for writing at the end of the file (appending) without removing the EOF marker 
//before writing new data to the file; creates the file first if it doesn¡¯t exist.
#define		POS_STREAM_APPEND	0x0003
//Opens for both reading and writing. (The file must exist.) 
#define		POS_STREAM_R_PLUS	0x0004
//Opens an empty file for both reading and writing. If the given file exists, its contents are destroyed. 
#define		POS_STREAM_W_PLUS	0x0005
//Opens for reading and appending
#define		POS_STREAM_A_PLUS	0x0006

#ifdef		M_WIDE_CHAR 
	#define	pos_stream_file_open		    pos_stream_open_from_file_w
	#define	pos_stream_file_is_exist		pos_stream_is_file_exist_w
	#define	pos_stream_file_create		    pos_stream_file_create_w
	#define	pos_stream_file_delete          pos_stream_file_delete_w
	#define	pos_stream_file_rename		    pos_stream_file_rename_w
	#define	pos_stream_file_copy			pos_stream_file_copy_w
	#define	pos_stream_file_move			pos_stream_file_move_w
#else
	#define	pos_stream_file_open		    pos_stream_open_from_file_s
	#define	pos_stream_file_is_exist		pos_stream_is_file_exist_s
	#define	pos_stream_file_create		    pos_stream_file_create_s
	#define	pos_stream_file_delete		    pos_stream_file_delete_s
	#define	pos_stream_file_rename		    pos_stream_file_rename_s
	#define	pos_stream_file_copy			pos_stream_file_copy_s
	#define	pos_stream_file_move			pos_stream_file_move_s
#endif 
/////////////////////Stream open and close functions///////////////////////

/**
 *	pos_stream_file_open
 *				open a stream from file 
 *	
 *	Parameter:
 *				file_para	(in)		the point to a file parameter structure, it is specified by the platform.
 *				mode        (in)		file read and write mode
 *
 *	Return:
 *				returns the POS_HANDLE handle if successful , INVALID_POS_HANDLE if failed.
 *
 *	Remark:
 *				
 */
POS_API POS_HANDLE pos_stream_open_from_file_s(CONST Void *file_para,Uint16 mode);
POS_API POS_HANDLE pos_stream_open_from_file_w(CONST Void *file_para,Uint16 mode);


/**
 *	pos_stream_open_from_mem_block
 *				open a stream from memory block
 *				  
 *	
 *	Parameter:
 *				mem				(in)		the point to a allocated memory block
 *				mem_size		(in)		the size of memory block
 *
 *	Return:
 *				returns the POS_HANDLE handle if successful , INVALID_POS_HANDLE if failed.
 *
 *	Remark:
 *				
 */

POS_API POS_HANDLE pos_stream_open_from_mem_block(Void* mem_ptr,Sint32 mem_size);

/**
 *	pos_stream_file_close
 *			   close a stream	  
 *	
 *	Parameter:
 *				stream_handle	 (in)		the handle of POS_HANDLE
 *
 *	Return:
 *				returns POS_TRUE if successful , POS_FALSE if failed.
 *
 *	Remark:
 *				
 */
POS_API Sint32 pos_stream_file_close(POS_HANDLE stream);


//////////////////////the functions only for file stream//////////////////////

/**
 *	Apos_stream_file_is_exist
 *			   	Check the file exist  
 *	
 *	Parameter:
 *				file_para	 (in)       the point to a file parameter structure, it is specified by the platform.
 *
 *	Return:
 *				returns POS_TRUE if file exist ,otherwise return POS_FALSE
 *
 *	Remark:
 *				
 */
POS_API Sint32 pos_stream_is_file_exist_s(CONST Void* file_para);
POS_API Sint32 pos_stream_is_file_exist_w(CONST Void* file_para);

/**
 *	AMStreamFileCreate
 *				create a file stream
 *	
 *	Parameter:
 *				file_para	 (in)       the point to a file parameter structure, it is specified by the platform.
 *
 *	Return:
 *				returns the POS_HANDLE handle if successful , POS_NULL if failed.
 *
 *	Remark:
 *				
 */
#define pos_stream_file_create_s(file_para) pos_stream_open_from_file_s((file_para),POS_STREAM_WRITE)
#define pos_stream_file_create_w(file_para) pos_stream_open_from_file_w((file_para),POS_STREAM_WRITE)

/**
 *	MStreamFileDelete
 *				delete a file 
 *	
 *	Parameter:
 *				
 *				file_para	 (in)       the point to a file parameter structure, it is specified by the platform.
 *
 *	Return:
 *				returns POS_TRUE if delete success ,otherwise return POS_FALSE.
 *
 *	Remark:
 *				
 */
POS_API Sint32 pos_stream_file_delete_s(CONST Void* file_para);
POS_API Sint32 pos_stream_file_delete_w(CONST Void* file_para);

/**
 *	MStreamFileRename
 *				rename a file 
 *	
 *	Parameter:
 *				
 *				old_file_para	 (in)		the old file parameter
 *				new_file_para	 (in)		the new file parameter
 *			
 *	Return:
 *				returns POS_TRUE if destory success ,otherwise return POS_FALSE.
 *
 *	Remark:
 *				
 */

POS_API Sint32 pos_stream_file_rename_s(CONST Void * old_file_para , CONST Void* new_file_para);
POS_API Sint32 pos_stream_file_rename_w(CONST Void * old_file_para , CONST  Void* new_file_para);


/**
 *	MStreamFileCopy
 *				copy a file 
 *	
 *	Parameter:
 *				
 *				dst_file_para	 (in)		the dst file parameter
 *				src_file_para	 (in)		the src file parameter
 *				bFailIfExists	 (in)		if false overwrite dst file	
 *			
 *	Return:
 *				returns POS_TRUE if destory success ,otherwise return POS_FALSE.
 *
 *	Remark:
 *				
 */

POS_API Sint32 pos_stream_file_copy_s(CONST Void * dst_file_para , CONST Void* src_file_para, Sint32 fail_if_exists);
POS_API Sint32 pos_stream_file_copy_w(CONST Void * dst_file_para , CONST Void* src_file_para, Sint32 fail_if_exists);


/**
 *	MStreamFileMove
 *				move a file 
 *	
 *	Parameter:
 *				
 *				dst_file_para	 (in)		the dst file parameter
 *				src_file_para	 (in)		the src file parameter
 *			
 *	Return:
 *				returns POS_TRUE if destory success ,otherwise return POS_FALSE.
 *
 *	Remark:
 *				
 */
POS_API Sint32 pos_stream_file_move_s(CONST Void * dst_file_para , CONST Void* src_file_para);
POS_API Sint32 pos_stream_file_move_w(CONST Void * dst_file_para , CONST Void* src_file_para);



////////////////////////the functions for both file and memory stream/////////////////////////////

/**
 *	pos_stream_file_size
 *				get total size from a stream. 
 *	
 *	Parameter:
 *				
 *				stream_handle    (in)     the handle of POS_HANDLE
 *
 *	Return:
 *				returns the stream size
 *
 *	Remark:
 *				
 */
POS_API Sint32 pos_stream_file_size(POS_HANDLE stream);


/**
 *	pos_stream_file_read
 *				reads data from  a stream. 
 *	
 *	Parameter:
 *				
 *				stream_handle	(in)		the handle of POS_HANDLE
 *				buf				(in)       the buf to read in
 *				size            (in)       the size of buf
 *
 *	Return:
 *				returns the number of full items actually read,
 *
 *	Remark:
 *				
 */
POS_API Sint32 pos_stream_file_read(POS_HANDLE stream,Void* buf,Sint32 size);


/**
 *	pos_stream_file_write
 *				writes data to a stream. 
 *	
 *	Parameter:
 *				
 *				stream_handle	 (in)		the handle of POS_HANDLE
 *				buf				 (in)       pointer to data to be written
 *				size             (in)       the size of buf
 *
 *	Return:
 *				returns the number of full items actually written,
 *
 *	Remark:
 *				
 */
POS_API Sint32 pos_stream_file_write(POS_HANDLE stream,Void* buf,Sint32 size);


/**
 *	pos_stream_file_flush
 *				flushes a stream 
 *	
 *	Parameter:
 *				
 *				stream_handle	 (in)		the handle of POS_HANDLE
 *
 *	Return:
 *				if the stream was successfully flushed,return POS_TRUE
 *				Otherwise,  returns POS_FALSE
 *
 *	Remark:
 *				
 */
POS_API Sint32 pos_stream_file_flush(POS_HANDLE stream);


/**
 *	pos_stream_file_seek
 *				Moves the stream pointer to a specified location  
 *	
 *	Parameter:
 *				
 *				stream_handle	 (in)		the handle of POS_HANDLE
 *				start			 (in)       Initial position
 *				offset           (in)       Number of bytes from start
 *
 *	Return:
 *				If successful, stream_seek returns 0 
 *				Otherwise,  returns a nonzero value
 *
 *	Remark:
 *				
 */
POS_API Sint32 pos_stream_file_seek(POS_HANDLE stream,Sint16 start, Sint32 offset);


/**
 *	pos_stream_file_tell
 *				Get the current position of a stream pointer  
 *	
 *	Parameter:
 *				
 *				stream_handle	 (in)		the handle of POS_HANDLE
 *
 *	Return:
 *				returns the current stream position, -1 if failed.
 *
 *	Remark:
 *				
 */
POS_API Sint32 pos_stream_file_tell(POS_HANDLE stream);

#ifdef __cplusplus
}
#endif

#endif // _POS_FILE_H_
