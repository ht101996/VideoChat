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
 * File Name:       pos_timer.h
 *
 * Reference:
 *
 * Author:          Kevin Hua
 *
 * Description:
 *      Timer interface.
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

#ifndef _POS_TIMER_H_
#define _POS_TIMER_H_

#include "pos_prolog.h"
#include "pos_type.h"
#include "pos_task.h"
//#include "pos_utl.h"
#include "pos_msgq.h"


/* timer flags */
#define POS_TM_UNUSE            0
#define POS_TM_NORMAL           1
#define POS_TM_AUTOLOAD         2

/* Timer max count */
#define POS_TIMER_MAX_CNT   32

typedef struct
{
    Uint16 tm_sec;  // 0-59
    Uint16 tm_min;  // 0-59
    Uint16 tm_hour; // 0-23
    Uint16 tm_day;  // 1-31
    Uint16 tm_month;// 1-12
    Uint16 tm_year; // year
    Uint16 tm_wday; // Day of week (0 ¨C 6; Sunday = 0).  
    Uint16 tm_yday; // 1-366, Day of year, January 1 = 1.
}Tpos_time;

#ifdef __cplusplus
extern "C"
{
#endif

POS_API Uint32 pos_sys_time();
POS_API Void pos_sleep(Uint32 time_value);
POS_API Void pos_get_local_time(Tpos_time *local_time);
POS_API Sint8* pos_get_format_time_str(Sint8 *buf); // buffer length no less than 32
POS_API Sint8* pos_get_format_time_str_1(Sint8 *buf);
POS_API Sint8* pos_get_format_date_str(Sint8 *buf); // buffer length no less than 32

typedef Void (*Tpos_timer_proc)(Void *data);

POS_API Sint32 pos_timer_init();
POS_API Sint32 pos_timer_uninit();

/**
 *	pos_timer_start
 *			   	start a timer  
 *	
 *	Parameter:
 *				value: time value.
 *              flag: timer type, like:POS_TM_UNUSE, POS_TM_NORMAL, POS_TM_AUTOLOAD
 *              module_id: if parameter "call_back" is null, timer will send a message to this module id by msgq.
 *              msg_type: message type.
 *              call_back: call back pointer, if this call back isn't null, timer will call this function.
 *              user_data: user data pointer.
 *
 *	Return:
 *				>=0: timer id.
 *              <0: no timer is avaiable.
 *
 *	Remark:
 *				
 */
POS_API Sint32 pos_timer_start(Sint32 value, Sint32 flag, Uint16 module_id, 
								Uint16 msg_type, Tpos_timer_proc call_back, Void *user_data);
POS_API Sint32 pos_timer_stop(Sint32 tm_id);



#ifdef __cplusplus
}
#endif

#endif // _POS_TIMER_H_
