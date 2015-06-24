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
 * File Name:       pos_socket.h
 *
 * Reference:
 *
 * Author:          Kevin Hua
 *
 * Description:
 *      Socket interface.
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

#ifndef _POS_SOCKET_H_
#define _POS_SOCKET_H_

#include "pos_prolog.h"
#include "pos_errno.h"

#if defined(__WIN32__)
    #include <mswsock.h>
    #pragma comment(lib, "ws2_32.lib")
    #pragma comment(lib, "Mswsock")
#elif defined(__LINUX__)
    #include <fcntl.h>
    #include <errno.h>
    #include <unistd.h>
    #include <signal.h>
    #include <stdlib.h>
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <sys/ioctl.h>
    #include <sys/time.h>
    #include <sys/select.h>
#if defined (__SERVICE__)
    #include <sys/epoll.h>
#endif
    #include <sys/sysinfo.h>
    
    #include <net/if.h>
    #include <netdb.h>

    #include <netinet/tcp.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <poll.h>

#elif defined __SYMBIAN32__
	#include <stdio.h>
    #include <libc\unistd.h>
	#include <libc\string.h>
	#include <libc\stdlib.h>
	#include <flogger.h>
	#include <libc\netinet\in.h>
	#include <libc\sys\socket.h>
    #include <libc\arpa\inet.h>
    #include <libc\sys\time.h>
    #include <libc\netdb.h>
#endif

#include "pos_type.h"
#ifdef __cplusplus
extern "C"
{
#endif


#if defined __WIN32__
#define POS_SOCKET					SOCKET
#define POS_SOCKET_INVALID          INVALID_SOCKET
#define POS_SOCKET_SELECT_ERROR     SOCKET_ERROR
#define POS_SOCKET_ERROR            SOCKET_ERROR
#define POS_SOCKET_LEN_T            int

#define POS_SOCKET_EWOULDBLOCK      WSAEWOULDBLOCK
#define POS_SOCKET_EINPROGRESS      WSAEINPROGRESS
#define POS_SOCKET_ECONNRESET       WSAECONNRESET
#define POS_SOCKET_ENOTCONN         WSAENOTCONN
#elif defined(__LINUX__)
#define POS_SOCKET					int
#define POS_SOCKET_INVALID          -1
#define POS_SOCKET_SELECT_ERROR     -1
#define POS_SOCKET_ERROR            -1
#define POS_SOCKET_LEN_T            socklen_t
#define POS_EPOLL_INVALID           -1

#define POS_SOCKET_EWOULDBLOCK      EWOULDBLOCK
#define POS_SOCKET_EINPROGRESS      EINPROGRESS
#define POS_SOCKET_ECONNRESET       ECONNRESET
#define POS_SOCKET_ENOTCONN         ENOTCONN
#elif defined (__SYMBIAN32__)
typedef Sint32 POS_SOCKET;
#define POS_SOCKET_INVALID POS_NULL
#define POS_SOCKET_SELECT_ERROR     -1
#define POS_SOCKET_ERROR            -1
#define POS_SOCKET_LEN_T            int

#define POS_SOCKET_EWOULDBLOCK      0
#define POS_SOCKET_EINPROGRESS      -1
#define POS_SOCKET_ECONNRESET       -1
#define POS_SOCKET_ENOTCONN         -1
#endif

#define POS_INADDR_ANY  INADDR_ANY
#define pos_inet_addr   inet_addr
#define pos_inet_ntoa   inet_ntoa
#define pos_htons       htons
#define pos_htonl       htonl
#define pos_ntohs       ntohs
#define pos_ntohl       ntohl

POS_API struct hostent *pos_gethostbyname_with_ap(Sint8 *name, Sint32 *pAPIndex);
POS_API Sint32 pos_inet_pton(Sint32 af, CONST Sint8 *src, Void *dst);   // supports only IPv4
POS_API Sint8* pos_inet_ntop(Sint32 af, CONST Void *src,                // supports only IPv4
                                Sint8 *dst, Sint32 cnt);

typedef struct sockaddr_in pos_sockaddr_in;
typedef struct sockaddr pos_sockaddr;

#if defined (__WIN32__)
#define pos_socket(family, type, protocol, param)      socket(family, type, protocol)
#define pos_connect         connect
#define pos_bind            bind
#define pos_listen          listen
#define pos_accept          accept
#define pos_recv            recv
#define pos_recvfrom        recvfrom
#define pos_send            send
#define pos_sendto          sendto
#define pos_shutdown        shutdown
#define pos_select          select
#define pos_getpeername     getpeername
#define pos_getsockname     getsockname
#define pos_gethostname     gethostname
#define pos_gethostbyname   gethostbyname
#define pos_getsockopt      getsockopt
#define pos_setsockopt      setsockopt
#define pos_ioctl           ioctlsocket
#define pos_close_socket	closesocket
POS_API Sint32 pos_set_ap(POS_SOCKET sockfd, Void *param);

#define pos_fd_set          fd_set
#define POS_FD_ZERO         FD_ZERO
#define POS_FD_SET          FD_SET
#define POS_FD_CLR          FD_CLR
#define POS_FD_ISSET        FD_ISSET
#define POS_FD_NFDS(fd)     (fd+1)
#elif defined (__LINUX__)
#define pos_socket(family, type, protocol, param)      socket(family, type, protocol)
#define pos_connect         connect
#define pos_bind            bind
#define pos_listen          listen
#define pos_accept          accept
#define pos_recv            recv
#define pos_recvfrom        recvfrom
#define pos_send            send
#define pos_sendto          sendto
#define pos_shutdown        shutdown
#define pos_select          select
#define pos_getpeername     getpeername
#define pos_getsockname     getsockname
#define pos_gethostname     gethostname
#define pos_gethostbyname   gethostbyname
#define pos_getsockopt      getsockopt
#define pos_setsockopt      setsockopt
#define pos_ioctl           ioctl
#define pos_close_socket    close
POS_API Sint32 pos_set_ap(POS_SOCKET sockfd, Void *param);

#define pos_open            open        // only enable in linux
#define pos_close           close       // only enable in linux
#define pos_create          create      // only enable in linux

#define pos_fd_set          fd_set
#define POS_FD_ZERO         FD_ZERO
#define POS_FD_SET          FD_SET
#define POS_FD_CLR          FD_CLR
#define POS_FD_ISSET        FD_ISSET
#define POS_FD_NFDS(fd)     (fd+1)   

#define pos_poll            poll
typedef struct pollfd       pos_pollfd;

#if defined (__SERVICE__)

#define pos_epoll_create    epoll_create
#define pos_epoll_ctl       epoll_ctl
#define pos_epoll_wait      epoll_wait

typedef int                 POS_EPOLL;
typedef struct epoll_event  pos_epoll_event;

#define POS_EPOLL_EPOLLIN   EPOLLIN
#define POS_EPOLL_EPOLLOUT  EPOLLOUT
#define POS_EPOLL_EPOLLPRI  EPOLLPRI
#define POS_EPOLL_EPOLLERR  EPOLLERR
#define POS_EPOLL_EPOLLHUP  EPOLLHUP
#define POS_EPOLL_EPOLLET   EPOLLET

#define POS_EPOLL_CTL_ADD   EPOLL_CTL_ADD
#define POS_EPOLL_CTL_DEL   EPOLL_CTL_DEL
#define POS_EPOLL_CTL_MOD   EPOLL_CTL_MOD
#endif

#elif defined(__SYMBIAN32__)
#define SYMBIAN_FDSET_SIZE 32
#define POS_FD_ZERO pos_zero_fd
#define POS_FD_CLR pos_clr_fd
#define POS_FD_SET pos_set_fd
#define POS_FD_ISSET pos_isset_fd
#define POS_FD_NFDS(fd)     (fd)

typedef struct fd_set_t{
	POS_SOCKET array[SYMBIAN_FDSET_SIZE];
	Sint32 count;
}pos_fd_set;

/*socket ioctl cmd*/
#define FIONBIO 0

POS_API POS_SOCKET pos_socket(Sint32 family, Sint32 type, Sint32 protocol, Void *);
POS_API Sint32 pos_connect(POS_SOCKET sockfd, struct sockaddr *addr, Sint32 addrlen);
POS_API Sint32 pos_bind(POS_SOCKET sockfd, struct sockaddr *addr, Sint32 addrlen);
POS_API Sint32 pos_listen(POS_SOCKET sockfd, Sint32 backlog);
POS_API POS_SOCKET pos_accept(POS_SOCKET sockfd, struct sockaddr *addr, Sint *addrlen);
POS_API Sint32 pos_recv(POS_SOCKET sockfd, Void *buf, Sint32 len, Sint32 flag);
POS_API Sint32 pos_recvfrom(POS_SOCKET sockfd, Void *buf, Sint32 len, Sint32 flag, struct sockaddr *addr, Sint *addrlen);
POS_API Sint32 pos_send(POS_SOCKET sockfd, Void *buf, Sint32 len, Sint32 flag);
POS_API Sint32 pos_sendto(POS_SOCKET sockfd, Void *buf, Sint32 len, Sint32 flag, struct sockaddr *addr, Sint32 addrlen);
POS_API Sint32 pos_shutdown(POS_SOCKET sockfd, Sint32 how);
POS_API Sint32 pos_getpeername(POS_SOCKET sockfd, struct sockaddr *addr, Sint *addrlen);
POS_API Sint32 pos_gethostname(Sint8 *name, Sint len);
#define pos_gethostbyname   gethostbyname
POS_API Sint32 pos_getsockname(POS_SOCKET sockfd, struct sockaddr *addr, Sint *addrlen);
POS_API Sint32 pos_getsockopt(POS_SOCKET sockfd, Sint32 level, Sint32 optname, Void *optval, Sint *optlen);
POS_API Sint32 pos_setsockopt(POS_SOCKET sockfd, Sint32 level, Sint32 optname, Void *optval, Sint32 optlen);
POS_API Sint32 pos_close_socket(POS_SOCKET sockfd);
POS_API Sint32 pos_set_ap(POS_SOCKET sockfd, Void *param);


/*type: FIONBIO,pArg: 0: blocking, 1: noBlocking*/
POS_API Sint32 pos_ioctl(POS_SOCKET sockfd, Sint32 type, Void *pArg);
POS_API Void pos_zero_fd(pos_fd_set *set);
POS_API Void pos_clr_fd(POS_SOCKET fd, pos_fd_set *set);
POS_API Sint32 pos_set_fd(POS_SOCKET fd, pos_fd_set *set);
POS_API Sint32 pos_isset_fd(POS_SOCKET fd, pos_fd_set *set);
POS_API Sint32 pos_select(POS_SOCKET nfds, pos_fd_set *readfds, pos_fd_set *writefds, pos_fd_set *exceptfds, struct timeval *tv);
#endif

POS_API Uint32 pos_net_traffix_add(Uint32 uiBytes);
POS_API Uint32 pos_net_traffix_get_lasted();
POS_API Uint32 pos_net_traffix_get_month();
POS_API Void pos_net_traffix_reset_lasted();
POS_API Void pos_net_traffix_reset_month();


typedef struct Tpos_ifi_info_st
{
    Sint8 ifi_name[16];                 // name of interface
    pos_sockaddr_in ifi_addr;           // address of interface
    pos_sockaddr_in ifi_netmask;        // netmask of interface
    Sint8 ifi_ismuc;                    // is support multicast
    Sint8 opt;                          // for user
    Sint8 chked;                        // 
    Sint8 pad8_0[1];
} Tpos_ifi_info;


POS_API Sint32 pos_sock_init();
POS_API Sint32 pos_sock_uninit();

POS_API Sint32 pos_ifi_info(Tpos_ifi_info *ifi_info_ptr, Uint32 *count_ptr);
POS_API Sint32 pos_enum_ip(Uint32 *puiIP, Sint32 *psiSize);
POS_API Uint32 pos_name_ip(Sint8 *szName);


#ifdef __cplusplus
}
#endif
#endif

