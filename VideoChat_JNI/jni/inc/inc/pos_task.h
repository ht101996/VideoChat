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
 * File Name:       pos_task.h
 *
 * Reference:
 *
 * Author:          Kevin Hua
 *
 * Description:
 *      Process, thread interface.
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

#ifndef _POS_TASK_H_
#define _POS_TASK_H_


#include "pos_prolog.h"
#include "pos_type.h" 
#if defined (__LINUX__)  
    #include <pthread.h>
    #include <semaphore.h>
    #include <pthread.h>
    #include <poll.h>
#endif

#define POS_TASK_HCP_MC         51
#define POS_THREAD_DEFAULT_STACK  65536   /* 64K */

#ifdef __WIN32__

#   define POS_PROCESS_PRIORITY_IDLE           IDLE_PRIORITY_CLASS
#   define POS_PROCESS_PRIORITY_BELOW_NORMAL   BELOW_NORMAL_PRIORITY_CLASS
#   define POS_PROCESS_PRIORITY_NORMAL         NORMAL_PRIORITY_CLASS
#   define POS_PROCESS_PRIORITY_ABOVE_NORMAL   ABOVE_NORMAL_PRIORITY_CLASS
#   define POS_PROCESS_PRIORITY_HIGH           HIGH_PRIORITY_CLASS
#   define POS_PROCESS_PRIORITY_REALTIME       REALTIME_PRIORITY_CLASS

#   define POS_THREAD_PRIORITY_IDLE            THREAD_PRIORITY_IDLE
#   define POS_THREAD_PRIORITY_LOWEST          THREAD_PRIORITY_LOWEST
#   define POS_THREAD_PRIORITY_BELOW_NORMAL    THREAD_PRIORITY_BELOW_NORMAL
#   define POS_THREAD_PRIORITY_NORNAL          THREAD_PRIORITY_NORMAL
#   define POS_THREAD_PRIORITY_ABOVE_NORMAL    THREAD_PRIORITY_ABOVE_NORMAL
#   define POS_THREAD_PRIORITY_HIGHEST         THREAD_PRIORITY_HIGHEST
#   define POS_THREAD_PRIORITY_REALTIME        THREAD_PRIORITY_TIME_CRITICAL

#elif defined (__LINUX__)  

#   define POS_PROCESS_PRIORITY_IDLE           19
#   define POS_PROCESS_PRIORITY_BELOW_NORMAL   10
#   define POS_PROCESS_PRIORITY_NORMAL         0
#   define POS_PROCESS_PRIORITY_ABOVE_NORMAL   -6
#   define POS_PROCESS_PRIORITY_HIGH           -19
#   define POS_PROCESS_PRIORITY_REALTIME       SCHED_FIFO

#   define POS_THREAD_PRIORITY_IDLE            19
#   define POS_THREAD_PRIORITY_LOWEST          12
#   define POS_THREAD_PRIORITY_BELOW_NORMAL    6
#   define POS_THREAD_PRIORITY_NORNAL          0
#   define POS_THREAD_PRIORITY_ABOVE_NORMAL    -6
#   define POS_THREAD_PRIORITY_HIGHEST         -19
#   define POS_THREAD_PRIORITY_REALTIME        SCHED_FIFO

#elif defined __SYMBIAN32__
#   define POS_PROCESS_PRIORITY_IDLE           EPriorityLow
#   define POS_PROCESS_PRIORITY_BELOW_NORMAL   EPriorityLow
#   define POS_PROCESS_PRIORITY_NORMAL         EPriorityBackground
#   define POS_PROCESS_PRIORITY_ABOVE_NORMAL   EPriorityForeground
#   define POS_PROCESS_PRIORITY_HIGH           EPriorityHigh
#   define POS_PROCESS_PRIORITY_REALTIME       EPriorityRealTimeServer

#   define POS_THREAD_PRIORITY_IDLE            EPriorityNull
#   define POS_THREAD_PRIORITY_LOWEST          EPriorityLess
#   define POS_THREAD_PRIORITY_BELOW_NORMAL    EPriorityNormal
#   define POS_THREAD_PRIORITY_NORNAL          EPriorityMore
#   define POS_THREAD_PRIORITY_ABOVE_NORMAL    EPriorityMuchMore
#   define POS_THREAD_PRIORITY_HIGHEST         EPriorityMuchMore
#   define POS_THREAD_PRIORITY_REALTIME        EPriorityRealTime

#endif

#if defined (__LINUX__)  
    typedef Void* (*POS_FUNC_ENTRY)(Void *);
#elif defined(__WIN32__)
    typedef Uint32 (*POS_FUNC_ENTRY)(Void *);
#elif defined __SYMBIAN32__
    typedef Sint32 (*POS_FUNC_ENTRY)(Void *);
#endif

typedef Void* Tpos_func_param;
typedef Sint32 Tpos_thread_id;
typedef Sint32 Tpos_process_id;

#if defined (__LINUX__)  
typedef Sint32 Tpos_thread_hdl;
typedef Void *Tpos_process_hdl;
typedef Void *Tpos_module_hdl;
#define POS_HDL_THREAD_INVALID (0)
#define POS_HDL_PROCESS_INVALID (POS_NULL)
#elif defined(__WIN32__)
typedef HANDLE  Tpos_thread_hdl;
typedef HANDLE  Tpos_process_hdl;
typedef HMODULE Tpos_module_hdl;
#define POS_HDL_THREAD_INVALID (0)
#define POS_HDL_PROCESS_INVALID (0)
#elif defined __SYMBIAN32__
typedef Void *Tpos_thread_hdl;
typedef Void *Tpos_process_hdl;
typedef Void *Tpos_module_hdl;
#define POS_HDL_THREAD_INVALID ((Void *)0)
#define POS_HDL_PROCESS_INVALID ((Void *)0)
#endif

#ifdef __cplusplus
extern "C"
{
#endif

POS_API Tpos_thread_hdl pos_thread_create(Tpos_thread_id tid, POS_FUNC_ENTRY thread_entry, Sint32 pri, Sint32 stack_size, Tpos_func_param param);
POS_API POS_RESULT pos_thread_init();
POS_API POS_RESULT pos_thread_uninit();
POS_API POS_RESULT pos_thread_exit(Tpos_thread_hdl thread);
POS_API POS_RESULT pos_thread_destroy(Tpos_thread_hdl thread);
POS_API Tpos_thread_hdl pos_get_current_thread();
POS_API POS_RESULT pos_thread_suspend(Tpos_thread_hdl thread);
POS_API POS_RESULT pos_thread_resume(Tpos_thread_hdl thread);
POS_API POS_RESULT pos_thread_signal_shield(Tpos_thread_hdl thread);
POS_API POS_RESULT pos_set_thread_priority(Tpos_thread_hdl thread, Sint32 priority);
POS_API Sint32 pos_get_thread_priority(Tpos_thread_hdl thread);
POS_API Uint32 pos_get_thread_id(Tpos_thread_hdl thread);
POS_API POS_RESULT pos_thread_is_exit(Tpos_thread_hdl thread);

POS_API Tpos_process_hdl pos_process_create(Sint8 *command_line);
POS_API POS_RESULT pos_process_init();
POS_API POS_RESULT pos_process_uninit();
POS_API POS_RESULT pos_process_exit(Tpos_process_hdl process);
POS_API POS_RESULT pos_process_destroy(Tpos_process_hdl process);
POS_API Tpos_process_hdl pos_get_current_process();
POS_API POS_RESULT pos_set_process_priority(Tpos_process_hdl process, Sint32 priority);
POS_API Sint32 pos_get_process_priority(Tpos_process_hdl process);
POS_API Uint32 pos_get_process_id(Tpos_process_hdl process);

#if !defined(__UNICODE__)
#   define pos_get_module_file_name pos_get_module_file_name_a
#else
#   define pos_get_module_file_name pos_get_module_file_name_w
#endif

POS_API Sint32 pos_get_module_file_name_a(Tpos_module_hdl module, Sint8 *filename, Sint32 size);
//POS_API Sint32 pos_get_module_file_name_w(Tpos_module_hdl module, Sint8 );

POS_API Sint32 pos_thread_local_alloc(Sint32 *index);
POS_API Void pos_thread_local_free(Sint32 index);
POS_API Sint32 pos_thread_local_set(Sint32 index, POS_HANDLE value);
POS_API POS_HANDLE pos_thread_local_get(Sint32 index);

POS_API Sint32 pos_task_main_init();
POS_API Sint32 pos_task_main_uninit();

#ifdef __cplusplus
}
#endif

#endif // _POS_TASK_H_
