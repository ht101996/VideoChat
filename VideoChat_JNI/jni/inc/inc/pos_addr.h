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
 * File Name:       pos_addr.h
 *
 * Reference:
 *
 * Author:          Kevin Hua
 *
 * Description:
 *      The virtual address defination,our products are communicate by it,it makes by normal socket
 *      address, UUID or email and so on.
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

#ifndef _POS_ADDR_H_
#define _POS_ADDR_H_

#include "pos_prolog.h"
#include "pos_type.h"
#include "pos_socket.h"
#include "pos_string.h"


#ifdef __cplusplus
extern "C"
{
#endif

// network type
#define POS_NET_NONE    0
#define POS_NET_CLIENT  1
#define POS_NET_SERVER  2
#define POS_NET_BOTH    3

// internet protocol type
#define POS_INET_PROTOCOL_NONE  0
#define POS_INET_PROTOCOL_UDP   1
#define POS_INET_PROTOCOL_TCP   2

// connection type
#define POS_CONNECT_NONE    0
#define POS_CONNECT_NORMAL  1
#define POS_CONNECT_LISTEN  2
#define POS_CONNECT_MUC     3

// uuid length defination
#if defined (__UUID_20__)
    #define POS_UUID_LEN    20
#else //__UUID_16__
    #define POS_UUID_LEN    16
#endif

// virtual address type
#define POS_ADDR_SOCK   0   // socket address
#define POS_ADDR_UUID   1   // uuid address
#define POS_ADDR_LSID   2   // local service id address
#define POS_ADDR_RSID   3   // remote service id and socket address

typedef struct Tpos_addr_st
{
    Sint32 type;    // address type, such as POS_ADDR_SOCK
    //Sint16 sid;
    union
    {
        pos_sockaddr_in sockaddr;   // socket address in union
        Sint8 uuid[POS_UUID_LEN];   // uuid address in union
    } addr;         // address

}Tpos_addr;

typedef struct 
{
    Uint8 uuid_value[POS_UUID_LEN];

}Tpos_uuid;

/*------------------------------------------------------------------------------*/
/* common macros for Tpos_addr using */

/* whether or not the input Tpos_addr is in type of UUID */
#define POS_ADDR_IS_UUID(pos_addr_ptr)      ((pos_addr_ptr)->type == POS_ADDR_UUID)
/* whether or not the input Tpos_addr is in type of NETWORK ADDRESS */
#define POS_ADDR_IS_NETWORK(pos_addr_ptr)      ((pos_addr_ptr)->type == POS_ADDR_SOCK)

/* whether or not the input Sint32 or Uint32 is a valid network address */
#define POS_ADDR_IS_VALID_NETWORK(network_addr)      (((network_addr) != 0) && ((network_addr) != -1))
/* whether or not the input Sint16 or Uint16 is a valid network port */
#define POS_ADDR_IS_VALID_PORT(network_port)      (((network_port) != 0) && (((Uint32)network_port <= 65535)))

/* whether or not the input Tpos_addr is a valid POS_ADDRESS */
STATIC INLINE Bool8 POS_ADDR_IS_VALID(Tpos_addr *pos_addr_ptr)
{
    if(POS_ADDR_IS_UUID(pos_addr_ptr))
        return POS_TRUE;

    if(POS_ADDR_IS_NETWORK(pos_addr_ptr))
    {
        if((0 == pos_addr_ptr->addr.sockaddr.sin_port)
            || (0 == pos_addr_ptr->addr.sockaddr.sin_addr.s_addr)
            || (-1 == (Sint32)pos_addr_ptr->addr.sockaddr.sin_addr.s_addr))
            return POS_FALSE;
        else
            return POS_TRUE;
    }

    return POS_FALSE;
}

/* whether or not the input two Tpos_addr-s are equal in VALID fields */
STATIC INLINE Bool8 POS_ADDR_EQUAL(Tpos_addr *pos_addr_a, Tpos_addr *pos_addr_b)
{
    if(pos_addr_a->type != pos_addr_b->type)
        return POS_FALSE;
    if(pos_addr_a->type == POS_ADDR_UUID)
    {
        if(0 == memcmp(pos_addr_a->addr.uuid, pos_addr_b->addr.uuid, sizeof(pos_addr_a->addr.uuid)))
            return POS_TRUE;
        else
            return POS_FALSE;
    }

    if(pos_addr_a->type == POS_ADDR_SOCK)
    {
        if((pos_addr_a->addr.sockaddr.sin_port == pos_addr_b->addr.sockaddr.sin_port)
            && (pos_addr_a->addr.sockaddr.sin_addr.s_addr == pos_addr_b->addr.sockaddr.sin_addr.s_addr))
            return POS_TRUE;
    }

    return POS_FALSE;
}

/* whether or not the input two Tpos_addr-s are equal in network address, NOT care about PORT.
 * and if the Tpos_addr type is different, they will be thought as NOT equal */
STATIC INLINE Bool8 POS_ADDR_EQUAL_NETWORK(Tpos_addr *pos_addr_a, Tpos_addr *pos_addr_b)
{
    if(pos_addr_a->type != pos_addr_b->type)
        return POS_FALSE;
    if(pos_addr_a->type != POS_ADDR_SOCK)
        return POS_FALSE;
    if(pos_addr_a->addr.sockaddr.sin_addr.s_addr == pos_addr_b->addr.sockaddr.sin_addr.s_addr)
        return POS_TRUE;

    return POS_FALSE;
}

/* to access the NETWORK address from an input Tpos_addr,
 * if input is not a NETWORK type Tpos_addr, 0 will be returned, which is an invalid NETWORK address */
STATIC INLINE Sint32 POS_ADDR_GET_NETWORK(Tpos_addr *pos_addr_ptr)
{
    if(pos_addr_ptr->type != POS_ADDR_SOCK)
        return 0;
    return pos_addr_ptr->addr.sockaddr.sin_addr.s_addr;
}

/* to access the network PORT from an input Tpos_addr,
 * if input is not a NETWORK type Tpos_addr, 0 will be returned, which is an invalid PORT*/
STATIC INLINE Uint16 POS_ADDR_GET_PORT(Tpos_addr *pos_addr_ptr)
{
    if(pos_addr_ptr->type != POS_ADDR_SOCK)
        return 0;
    return pos_addr_ptr->addr.sockaddr.sin_port;
}

/* to access the network PORT from an input Tpos_addr,
 * if input is not a UUID type Tpos_addr, NULL pointer will be returned, which is an invalid UUID infor */
STATIC INLINE Sint8 *POS_ADDR_GET_UUID(Tpos_addr *pos_addr_ptr)
{
    if(pos_addr_ptr->type != POS_ADDR_UUID)
        return NULL;
    return pos_addr_ptr->addr.uuid;
}
/*------------------------------------------------------------------------------*/

POS_API Sint8 * pos_addrin2str(pos_sockaddr_in *addr_ptr, Sint8 *addr_str);
POS_API Sint8 * pos_addr2str(Tpos_addr *addr_ptr, Sint8 *addr_str);
POS_API Sint8 * pos_addr2uuid(Tpos_addr *addr_ptr, Sint8 *addr_str);
POS_API Sint8 * pos_addr2ip(Tpos_addr *addr_ptr, Sint8 *addr_str);
POS_API Uint16  pos_addr2port(Tpos_addr *addr_ptr);

#ifdef __cplusplus
}
#endif


#endif // _POS_ADDR_H_
