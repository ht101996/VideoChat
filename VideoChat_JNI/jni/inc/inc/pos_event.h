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
 * File Name:       pos_event.h
 *
 * Reference:
 *
 * Author:          Kevin Hua
 *
 * Description:
 *      Event supports file, such as mutex, cond, critical section and semaphore.
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

#ifndef _POS_EVENT_H_
#define _POS_EVENT_H_

#include "pos_prolog.h"
#include "pos_type.h"

#if defined (__LINUX__)  
    #include <pthread.h>
    #include <semaphore.h>
    #include <pthread.h>
    #include <poll.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif

#define POS_MUTEX_FLAG_NULL     0
#define POS_MUTEX_TIMEOUT_VALUE_NULL    -1

#if defined (__LINUX__)  
typedef pthread_mutex_t  Tpos_cs_id;	//critical section
typedef pthread_mutex_t Tpos_mutex_id;
typedef pthread_cond_t  Tpos_cond_id;
typedef sem_t Tpos_sem_id;
#elif defined (__WIN32__)
typedef CRITICAL_SECTION  Tpos_cs_id;	//critical section
typedef HANDLE  Tpos_mutex_id;
typedef HANDLE  Tpos_cond_id;
typedef HANDLE  Tpos_sem_id;
#elif defined __SYMBIAN32__
typedef Void* Tpos_cs_id;
typedef Void* Tpos_mutex_id;
typedef Void* Tpos_cond_id;
typedef Void* Tpos_sem_id;
#endif

#define POS_NO_WAIT             0
#define POS_WAIT_FOREVER        ((Uint32)(-1))
typedef Uint32 Tpos_time_interval;     // POS_NO_WAIT:  return immediately
                                        // POS_WAIT_FOREVER: wait forever.
                                        // other: Wait up to a fixed amount of time.

POS_API POS_RESULT pos_cs_create(Tpos_cs_id *cs);
POS_API POS_RESULT pos_cs_obtain(Tpos_cs_id *cs);
POS_API POS_RESULT pos_cs_release(Tpos_cs_id *cs);
POS_API POS_RESULT pos_cs_delete(Tpos_cs_id *cs);

POS_API POS_RESULT pos_mutex_create(Tpos_mutex_id *mutex, Uint8 *name, Sint32 flag);
POS_API POS_RESULT pos_mutex_recursive_create(Tpos_mutex_id *mutex, Uint8 *name, Sint32 flag);
POS_API POS_RESULT pos_mutex_delete(Tpos_mutex_id *mutex);
POS_API POS_RESULT pos_mutex_obtain(Tpos_mutex_id *mutex, Tpos_cond_id *cond, Tpos_time_interval *timeout);
POS_API POS_RESULT pos_mutex_release(Tpos_mutex_id *mutex, Tpos_cond_id *cond);

POS_API POS_RESULT pos_cond_create(Tpos_cond_id *cond, Uint8 *name, Uint8 flag);
POS_API POS_RESULT pos_cond_delete(Tpos_cond_id *cond);
POS_API POS_RESULT pos_cond_signal(Tpos_cond_id *cond);
POS_API POS_RESULT pos_cond_broadcast(Tpos_cond_id *cond);
POS_API POS_RESULT pos_cond_wait(Tpos_cond_id *cond, Tpos_mutex_id *mutex, Tpos_time_interval *timeout);

POS_API POS_RESULT pos_sem_create(Tpos_sem_id *sem, Uint32 count);
POS_API POS_RESULT pos_sem_obtain(Tpos_sem_id *sem);
POS_API POS_RESULT pos_sem_release(Tpos_sem_id *sem);
POS_API POS_RESULT pos_sem_delete(Tpos_sem_id *sem);

#ifdef __cplusplus
}
#endif

#endif // _POS_EVENT_H_
