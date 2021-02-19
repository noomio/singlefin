/*===================================================================
Copyright (c) 2016-2019 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
===================================================================*/

/*
!! IMPORTANT NOTE: "DATA SERVICES" VERSION CATEGORY.�
*/

/** @file qapi_socket.h
 * @addtogroup qapi_socket_datatypes
 * @{
 *
 * @details The BSD socket interface API is a collection of standard functions
 * that allow the application to include Internet communications capabilities.
 * In general, the BSD socket interface relies on Client-Server architecture,
 * and uses a Socket object for every operation.
 * The interface supports TCP (SOCK_STREAM) and UDP (SOCK_DGRAM), Server
 * mode and Client mode, as well as IPv4 and IPv6 communication.
 * A socket can be configured with specific options (see @xhyperref{socketOptions, Socket Options}).
 * Due to the memory constrained properties of the device, it is mandatory to
 * follow the BSD socket programming guidelines and in particular, check for
 * return values of each function. There is a chance that an operation may
 * fail due to resource limitations. For example, the send function may be
 * able to send only some of the data and not all of it in a single call.
 * A subsequent call with the rest of the data is required. In some other
 * cases, an application thread may need to sleep in order to allow the system
 * to clear its queues, process data, and so on.
 *
 * @}
 */

#ifndef _QAPI_SOCKET_H_
#define _QAPI_SOCKET_H_

#include <stdint.h>

#include "qapi_data_txm_base.h"
#include "qapi_status.h"



/* Socket QAPI REQUEST ID DEFINES */
#define  TXM_QAPI_SOCKET_GET             TXM_QAPI_SOCKET_BASE + 1
#define  TXM_QAPI_SOCKET_BIND            TXM_QAPI_SOCKET_BASE + 2
#define  TXM_QAPI_SOCKET_LISTEN          TXM_QAPI_SOCKET_BASE + 3
#define  TXM_QAPI_SOCKET_ACCEPT          TXM_QAPI_SOCKET_BASE + 4
#define  TXM_QAPI_SOCKET_CONNECT         TXM_QAPI_SOCKET_BASE + 5
#define  TXM_QAPI_SOCKET_SETOPT          TXM_QAPI_SOCKET_BASE + 6
#define  TXM_QAPI_SOCKET_GETOPT          TXM_QAPI_SOCKET_BASE + 7
#define  TXM_QAPI_SOCKET_CLOSE           TXM_QAPI_SOCKET_BASE + 8
#define  TXM_QAPI_SOCKET_ERRNO           TXM_QAPI_SOCKET_BASE + 9
#define  TXM_QAPI_SOCKET_RECVFROM        TXM_QAPI_SOCKET_BASE + 10
#define  TXM_QAPI_SOCKET_RECV            TXM_QAPI_SOCKET_BASE + 11
#define  TXM_QAPI_SOCKET_SENDTO          TXM_QAPI_SOCKET_BASE + 12
#define  TXM_QAPI_SOCKET_SEND            TXM_QAPI_SOCKET_BASE + 13
#define  TXM_QAPI_SOCKET_SELECT          TXM_QAPI_SOCKET_BASE + 14
#define  TXM_QAPI_SOCKET_FD_ZERO         TXM_QAPI_SOCKET_BASE + 15
#define  TXM_QAPI_SOCKET_GET_FD_CLR      TXM_QAPI_SOCKET_BASE + 16
#define  TXM_QAPI_SOCKET_GET_FD_SET      TXM_QAPI_SOCKET_BASE + 17
#define  TXM_QAPI_SOCKET_GET_FD_ISSET    TXM_QAPI_SOCKET_BASE + 18
#define  TXM_QAPI_SOCKET_GET_SOCKNAME    TXM_QAPI_SOCKET_BASE + 19
#define  TXM_QAPI_SOCKET_GET_PEERNAME    TXM_QAPI_SOCKET_BASE + 20
#define  TXM_QAPI_SOCKET_SHUTDOWN        TXM_QAPI_SOCKET_BASE + 21


/** @addtogroup qapi_socket_datatypes
@{ */

/*
 * Address families (AF) for "domain" or "family" argument in qapi_socket()
 */
#define  AF_UNSPEC       0	 /**< Address family is unspecified. */
#define  AF_INET         2   /**< Address family is IPv4. */
#define  AF_INET6        3   /**< Address family is IPv6. */
#define  AF_INET_DUAL46  4   /**< Address family is IPv4 and IPv6. */
#define  AF_NONIP        11   /**< Address family is NONIP. */


/** Socket stream (TCP). */
#define     SOCK_STREAM    1

/** Socket datagram (UDP). */
#define     SOCK_DGRAM     2

/** Raw socket. */
#define     SOCK_RAW       3

/** NonIP socket. */
#define     SOCK_NONIP     4

/** 
 * @name BSD Socket Error Codes
   @{ */

#define ENOBUFS        1	/**< No buffer space is available. */
#define ETIMEDOUT      2	/**< Operation timed out. */
#define EISCONN        3	/**< Socket is already connected. */
#define EOPNOTSUPP     4	/**< Operation is not supported. */
#define ECONNABORTED   5	/**< Software caused a connection abort. */
#define EWOULDBLOCK    6	/**< Socket is marked nonblocking and the requested operation will block. */
#define ECONNREFUSED   7	/**< Connection	was refused. */
#define ECONNRESET     8	/**< Connection was reset by peer. */

#define EBADF          9    /**< An invalid descriptor was specified. */
#define EALREADY       10	/**< Operation is already in progress. */

#ifndef EINVAL
#define EINVAL         11	/**< Invalid argument was passed. */
#endif
#define EMSGSIZE       12	/**< Message is too long. */
#define EPIPE          13	/**< The local end has been shut down on a connection-oriented socket. */
#define EDESTADDRREQ   14	/**< Destination address is required. */
#define ESHUTDOWN      15	/**< Cannot send after a socket shutdown. */
#define ENOPROTOOPT    16	/**< Protocol is not available. */
#define EHAVEOOB       17	/**< Out of band. */

#ifndef ENOMEM
#define ENOMEM         18	/**< No memory is available. */
#endif

#define EADDRNOTAVAIL  19	/**< Cannot assign the requested address. */
#define EADDRINUSE     20	/**< Address is already in use. */
#define EAFNOSUPPORT   21	/**< Address family is not supported by the protocol family. */
#define EINPROGRESS    22	/**< Operation is in progress. */
#define ELOWER         23   /**< Lower layer (IP) error. */
#define ENOTSOCK       24	/**< Socket	operation on nonsocket. */
#define EIEIO          27	/**< I/O error. */
#define ETOOMANYREFS   28	/**< Too many references. */
#define EFAULT         29	/**< Bad address. */
#define ENETUNREACH    30	/**< Network is unreachable. */
#define ENOTCONN       31	/**< Socket is not connected. */

/** @} */ /* end_namegroup */

/** @name Socket Options
@xreflabel{hdr:socketOptions}
@{ */

#define     SOL_SOCKET     -1          /**< For use with [gs]etsockopt() at the socket level. */
#define     SOL_SOCKET     -1          /**< For use with [gs]etsockopt() at the socket level. */
#define     SO_ACCEPTCONN  0x00002     /**< Socket has had listen(). */
#define     SO_REUSEADDR   0x00004     /**< Allow local address reuse. */
#define     SO_KEEPALIVE   0x00008     /**< Keep connections alive. */
#define     SO_DONTROUTE   0x00010     /**< Not used. */
#define     SO_BROADCAST   0x00020     /**< Not used. */
#define     SO_USELOOPBACK 0x00040     /**< Not used. */
#define     SO_LINGER      0x00080     /**< Linger on close if data is present. */
#define     SO_OOBINLINE   0x00100     /**< Leave the received OOB data in line. */
#define     SO_TCPSACK     0x00200     /**< Allow TCP SACK (selective acknowledgment). */
#define     SO_WINSCALE    0x00400     /**< Set the scaling window option. */
#define     SO_TIMESTAMP   0x00800     /**< Set the TCP timestamp option. */
#define     SO_BIGCWND     0x01000     /**< Large initial TCP congenstion window. */
#define     SO_HDRINCL     0x02000     /**< User access to IP header for SOCK_RAW. */
#define     SO_NOSLOWSTART 0x04000     /**< Suppress slowstart on this socket. */
#define     SO_FULLMSS     0x08000     /**< Not used. */
#define     SO_NONIP_DSSHNDL  0x10000  /**< Set dss handle for nonip. */
#define     SO_NONIP_EXTYPE   0x20000  /**< Set exception type for nonip. */

#define     SO_SNDBUF      0x1001      /**< Send buffer size. */
#define     SO_RCVBUF      0x1002      /**< Receive buffer size. */
#define     SO_SNDTIMEO    0x1005      /**< Send a timeout. */
#define     SO_RCVTIMEO    0x1006      /**< Receive a timeout. */
#define     SO_ERROR       0x1007      /**< Socket error. */
#define     SO_RXDATA      0x1011      /**< Get a count of bytes in sb_rcv. */
#define     SO_TXDATA      0x1012      /**< Get a count of bytes in sb_snd. */
#define     SO_MYADDR      0x1013      /**< Return my IP address. */
#define     SO_NBIO        0x1014      /**< Set socket to Nonblocking mode. */
#define     SO_BIO         0x1015      /**< Set socket to Blocking mode. */
#define     SO_NONBLOCK    0x1016      /**< Set/get blocking mode via the optval parameter. */
#define     SO_CALLBACK    0x1017      /**< Set/get the TCP zero_copy callback routine. */
#define     SO_UDPCALLBACK 0x1019      /**< Set/get the UDP zero_copy callback routine. */

#define     SO_TCP_ACKDELAYTIME 0x2001   /**< Set time for delayed acks in (ms). */
#define     SO_TCP_NOACKDELAY   0x2002   /**< Suppress delayed ACKs. */
#define     TCP_MAXSEG     0x2003        /**< Set maximum segment size. */
#define     TCP_MAX_RXT    0X2005        /**< Set the max no of retxn. */
#define     TCP_OT         0X2006        /**< Set the overall timer for outstanding connection. */
#define     TCP_IRT        0X2007        /**< Set the initial retxn timeout value. */
#define     TCP_KEEPIDLE   0X2008        /**< Set the time before probing. */
#define     TCP_KEEPINTVL  0X2009        /**< Set the probe interval. */
#define     TCP_KEEPCNT    0X2010        /**< Set the max probes before drop. */

#define     IPPROTO_IP          0      /**< For use with [gs]etsockopt() at IPPROTO_IP level. */
#define     IP_HDRINCL          2      /**< IP header is included with the data. */
#define     IP_MULTICAST_IF     9      /**< Set/get the IP multicast interface. */
#define     IP_MULTICAST_TTL    10     /**< Set/get the IP multicast TTL. */
#define     IP_MULTICAST_LOOP   11     /**< Set/get the IP multicast loopback. */
#define     IP_ADD_MEMBERSHIP   12     /**< Add an IPv4 group membership. */
#define     IP_DROP_MEMBERSHIP  13     /**< Drop an IPv4 group membership. */
#define     IPV6_MULTICAST_IF   80     /**< Set the egress interface for multicast traffic. */
#define     IPV6_MULTICAST_HOPS 81     /**< Set the number of hops. */
#define     IPV6_MULTICAST_LOOP 82     /**< Enable/disable loopback for multicast. */
#define     IPV6_JOIN_GROUP     83     /**< Join an IPv6 MC group. */
#define     IPV6_LEAVE_GROUP    84     /**< Leave an IPv6 MC group. */
#define     IP_EXCLUDE_LIST     17     /**< Set/get the exclude list for 255 RAW socket. */
#define     IP_OPTIONS          1      /**< For use with [gs]etsockopt() at IP_OPTIONS level. */
#define     IP_TOS              3      /**< IPv4 type of service and precedence. */
#define     IP_TTL_OPT          4      /**< IPv4 time to live. */
#define     IPV6_SCOPEID        14     /**< IPv6 IF scope ID. */
#define     IPV6_UNICAST_HOPS   15     /**< IPv6 hop limit. */
#define     IPV6_TCLASS         16     /**< IPv6 traffic class. */

/** @} */ /* end_namegroup */

/** @name Flags for recv() and send()
@{ */

#define     MSG_OOB             0x1      /**< Send/receive out-of-band data. */
#define     MSG_PEEK            0x2      /**< Peek at the incoming message. */
#define     MSG_DONTROUTE       0x4      /**< Send without using routing tables. */
#define     MSG_DONTWAIT        0x20     /**< Send/receive is nonblocking. */
#define     MSG_ZEROCOPYSEND    0x1000   /**< Send with zero-copy. */


/** @} */ /* end_namegroup */


/** @name Flags for qapi_shutdown()
@{ */

#define SHUT_RD         0     /**< Shutdown "read" half. */
#define SHUT_WR         1     /**< Shutdown "write" half. */
#define SHUT_RDWR       2     /**< Shutdown both "read" and "write". */

/** @} */ /* end_namegroup */


/** Infinite time for the timeout_ms argument in qapi_select(). */
#define     QAPI_NET_WAIT_FOREVER   (0xFFFFFFFF)

/** Pre-pending qapi to internal socket structures' names to avoid conflict with internal structures. */
#define in6_addr   qapi_in6_addr
#define ip6_addr   qapi_ip6_addr
#define ip46addr_n qapi_ip46addr_n

/**
 * IPv4 Internet address.
 */
struct in_addr
{
   uint32_t s_addr;
   /**< IPv4 address in network order. */
};

/**
 * BSD-style socket IPv4 Internet address.
 */
struct sockaddr_in
{
   uint16_t         sin_family;
   /**< AF_INET. */

   uint16_t         sin_port;
   /**< UDP/TCP port number in network order. */

   struct in_addr   sin_addr;
   /**< IPv4 address in network order. */

   uint8_t          sin_zero[8];
   /**< Reserved -- must be zero. */
};


/**
 * IPv6 Internet address.
 */
typedef struct in6_addr
{
   uint8_t  s_addr[16];
   /**< 128-bit IPv6 address. */
} /** @cond */ip6_addr/** @endcond */;

/** @} */ /* end_addtogroup qapi_socket_datatypes */

/** @addtogroup qapi_socket_datatypes
@{ */

/**
 * BSD-style socket IPv6 Internet address.
 */
struct ip46addr_n
{
    uint16_t type;
    /**< AF_INET or AF_INET6. */
    union 
    {
        unsigned long	addr4;
        /**< IPv4 address. */
        uint8_t 		addr6[16];
        /**< IPv6 address. */
    } a;
    /**< Address union. */

    union 
    {
        unsigned long	gtwy4;
        /**< IPv4 gateway. */
        uint8_t 		gtwy6[16];
        /**< IPv6 gateway. */
    } g;
    /**< Gateway union. */
    
    uint32_t subnet;
    /**< Subnet. */
};

 /** Socket address information. */
struct sockaddr_in6
{
   uint16_t         sin_family;
   /**< AF_INET6. */

   uint16_t         sin_port;
   /**< UDP/TCP port number in network order. */

   uint32_t         sin_flowinfo;
   /**< IPv6 flow information. */

   struct in6_addr  sin_addr;
   /**< IPv6 address. */

   int32_t          sin_scope_id;
   /**< Set of interfaces for a scope. */
};

/**
 * Socket IPv4/IPv6 Internet address union.
 */
struct ip46addr
{
    uint16_t type;
    /**< AF_INET or AF_INET6. */
    union 
    {
        unsigned long   addr4;
        /**< IPv4 address. */
        ip6_addr        addr6;
        /**< IPv6 address. */
    } a;
    /**< Address union. */
};

/**
 * Generic socket Internet address.
 */
struct sockaddr
{
   uint16_t sa_family;
   /**< Address family. */
   uint16_t sa_port;
   /**< Port number in network order. */
   uint8_t  sa_data[32];
   /**< Big enough for 16-byte IPv6 address. */
};

/**
 * Exclude list endpoint.
 */
struct sockaddr_ep {
	struct sockaddr_ep * sockaddr_ep_next;	 /**< Next endpoint. */
	struct sockaddr * sockaddr_ep_addr;      /**< Endpoint address in exclude list. */
};

#undef  FD_SETSIZE
#define FD_SETSIZE  16
#undef  fd_set

/**
 * File descriptor sets for qapi_select().
 */
typedef struct
{
   uint32_t fd_count;
   /**< File descriptor count. */
   uint32_t fd_array[FD_SETSIZE];
   /**< File descriptor array. */
} fd_set;


/**
 *  This option is used to set the linger time that the socket will remain in the TCP FIN WAIT 1 state 
 *  before closing the socket and cleaning up the resources.
 */

struct   linger
{
    int   l_onoff;
    /** option on/off. */
    int   l_linger;   
    /** linger time. */
};

/*
 * @name Macros to Manipulate fd_set
@{ */

#undef FD_ZERO
#undef FD_CLR
#undef FD_SET
#undef FD_ISSET
#define FD_ZERO(set)            qapi_fd_zero((set))           /**< Clears a set. */
#define FD_CLR(handle, set)     qapi_fd_clr((handle), (set))    /**< Removes a given file descriptor from a set. */
#define FD_SET(handle, set)     qapi_fd_set((handle), (set))    /**< Adds a given file descriptor from a set. */
#define FD_ISSET(handle, set)   qapi_fd_isset((handle), (set))  /**< Tests to see if a file descriptor is part of the set after select() returns. */

/** @} */ /* end_namegroup */

/** @} */ /* end_addtogroup qapi_socket_datatypes */

#ifdef  QAPI_TXM_MODULE     // USER_MODE_DEFS

#define qapi_socket(a,b,c)              ((int)           (_txm_module_system_call12)(TXM_QAPI_SOCKET_GET           , (ULONG) a, (ULONG) b, (ULONG) c, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define qapi_bind(a,b,c)                ((qapi_Status_t) (_txm_module_system_call12)(TXM_QAPI_SOCKET_BIND          , (ULONG) a, (ULONG) b, (ULONG) c, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define qapi_listen(a,b)                ((qapi_Status_t) (_txm_module_system_call12)(TXM_QAPI_SOCKET_LISTEN        , (ULONG) a, (ULONG) b, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define qapi_accept(a,b,c)              ((int)           (_txm_module_system_call12)(TXM_QAPI_SOCKET_ACCEPT        , (ULONG) a, (ULONG) b, (ULONG) c, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define qapi_connect(a,b,c)             ((qapi_Status_t) (_txm_module_system_call12)(TXM_QAPI_SOCKET_CONNECT       , (ULONG) a, (ULONG) b, (ULONG) c, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define qapi_setsockopt(a,b,c,d,e)      ((qapi_Status_t) (_txm_module_system_call12)(TXM_QAPI_SOCKET_SETOPT        , (ULONG) a, (ULONG) b, (ULONG) c, (ULONG) d, (ULONG) e, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define qapi_getsockopt(a,b,c,d,e)      ((qapi_Status_t) (_txm_module_system_call12)(TXM_QAPI_SOCKET_GETOPT        , (ULONG) a, (ULONG) b, (ULONG) c, (ULONG) d, (ULONG) e, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define qapi_socketclose(a)             ((qapi_Status_t) (_txm_module_system_call12)(TXM_QAPI_SOCKET_CLOSE         , (ULONG) a, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define qapi_errno(a)                   ((int)           (_txm_module_system_call12)(TXM_QAPI_SOCKET_ERRNO         , (ULONG) a, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define qapi_recvfrom(a,b,c,d,e,f)      ((int)           (_txm_module_system_call12)(TXM_QAPI_SOCKET_RECVFROM      , (ULONG) a, (ULONG) b, (ULONG) c, (ULONG) d, (ULONG) e, (ULONG) f, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define qapi_recv(a,b,c,d)              ((int)           (_txm_module_system_call12)(TXM_QAPI_SOCKET_RECV          , (ULONG) a, (ULONG) b, (ULONG) c, (ULONG) d, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define qapi_sendto(a,b,c,d,e,f)        ((int)           (_txm_module_system_call12)(TXM_QAPI_SOCKET_SENDTO        , (ULONG) a, (ULONG) b, (ULONG) c, (ULONG) d, (ULONG) e, (ULONG) f, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define qapi_send(a,b,c,d)              ((int)           (_txm_module_system_call12)(TXM_QAPI_SOCKET_SEND          , (ULONG) a, (ULONG) b, (ULONG) c, (ULONG) d, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define qapi_select(a,b,c,d)            ((int)           (_txm_module_system_call12)(TXM_QAPI_SOCKET_SELECT        , (ULONG) a, (ULONG) b, (ULONG) c, (ULONG) d, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define qapi_fd_zero(a)                 ((qapi_Status_t) (_txm_module_system_call12)(TXM_QAPI_SOCKET_FD_ZERO       , (ULONG) a, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define qapi_fd_clr(a,b)                ((qapi_Status_t) (_txm_module_system_call12)(TXM_QAPI_SOCKET_GET_FD_CLR    , (ULONG) a, (ULONG) b, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define qapi_fd_set(a,b)                ((qapi_Status_t) (_txm_module_system_call12)(TXM_QAPI_SOCKET_GET_FD_SET    , (ULONG) a, (ULONG) b, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define qapi_fd_isset(a,b)              ((qapi_Status_t) (_txm_module_system_call12)(TXM_QAPI_SOCKET_GET_FD_ISSET  , (ULONG) a, (ULONG) b, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define qapi_getsockname(a,b,c)         ((qapi_Status_t) (_txm_module_system_call12)(TXM_QAPI_SOCKET_GET_SOCKNAME  , (ULONG) a, (ULONG) b, (ULONG) c, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define qapi_getpeername(a,b,c)         ((qapi_Status_t) (_txm_module_system_call12)(TXM_QAPI_SOCKET_GET_PEERNAME  , (ULONG) a, (ULONG) b, (ULONG) c, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define qapi_shutdown(a,b)              ((qapi_Status_t) (_txm_module_system_call12)(TXM_QAPI_SOCKET_SHUTDOWN  , (ULONG) a, (ULONG) b, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#else

UINT qapi_Socket_Handler(UINT id, UINT a1, UINT a2, UINT a3, UINT a4, UINT a5, UINT a6, UINT a7, UINT a8, UINT a9, UINT a10, UINT a11, UINT a12);


/** @ingroup qapi_socket
 *
 * Creates an endpoint for communication.
 *
 * @param[in] family    Protocol family used for communication.
 *                      The supported families are: \n
 *                      - AF_INET     -- IPv4 Internet protocols
 *                      - AF_INET6    -- IPv6 Internet protocols @vertspace{-14}
 * @param[in] type      Transport mechanism used for communication. 
 *                      The supported types are: \n
 *                      - SOCK_STREAM     -- TCP
 *                      - SOCK_DGRAM      -- UDP @vertspace{-14}
 * @param[in] protocol  Must be set to 0.
 *
 * @return On success, a handle for the new socket is returned. \n
 * On error, -1 is returned.
 */

int qapi_socket(int32_t family, int32_t type, int32_t protocol);

/** @ingroup qapi_bind
 *
 * Assigns an address to the socket created by qapi_socket().
 *
 * @param[in] handle    Socket handle returned from qapi_socket().
 * @param[in] addr      Pointer to an address to be assigned to the socket.
 *                      The actual address structure passed for the addr argument
 *                      will depend on the address family.
 * @param[in] addrlen   Specifies the size, in bytes, of the address pointed to by addr.
 *
 * @return On success, 0 is returned. On error, -1 is returned.
 * Errors:
 *  ENOBUFS : No Memory available.
 *  EINVAL : Invalid Arguments Passed.
 *  EADDRNOTAVAIL : Address not available.
 *  EADDRINUSE : Address is in Use , port is in use and SO_REUSEADDR not set.
 */
 
qapi_Status_t qapi_bind(int32_t handle, struct sockaddr *addr, int32_t addrlen);

/** @ingroup qapi_listen
 *
 * Marks the socket as a passive socket.
 *
 * @param[in] handle    Handle (returned from qapi_socket()) that refers to a SOCK_STREAM socket.
 * @param[in] backlog   Define the maximum length to which the queue of pending connections for the handle may grow.
 *
 * @return On success, 0 is returned. On error, -1 is returned.
 * Errors:
 *  ENOBUFS : No Memory available.
 *  EINVAL : Invalid Arguments Passed.
 *  EADDRNOTAVAIL : Address not available.
 *  EADDRINUSE : Address is in Use , port is in use and SO_REUSEADDR not set.
 */

qapi_Status_t qapi_listen(int32_t handle, int32_t backlog);

/** @ingroup qapi_accept
 *
 * Accepts a connection request from the peer on a SOCK_STREAM socket.
 *
 * This function is used with a SOCK_STREAM socket.
 * It extracts the first connection request on the queue of pending connections
 * for the listening socket (i.e., handle), creates a new connected socket, and returns
 * a new socket handle referring to that socket. The newly created socket is in the
 * Established state. The original socket (i.e., handle) is unaffected by this call.
 * If no pending connections are present on the queue, and the socket is not marked
 * as nonblocking, qapi_accept() blocks the caller until a connection is present.
 * If the socket is marked nonblocking and no pending connections are present on the queue,
 * qapi_accept() fails with the error EAGAIN or EWOULDBLOCK.
 *
 * @param[in] handle    Socket handle that has been created with qapi_socket(), bound to a
 *                      local address with qapi_bind(), and listens for connections after
 *                      qapi_listen().
 * @param[in] cliaddr   Pointer to a sockaddr structure. This structure is filled in with the address
 *                      of the peer socket. The exact format of the address returned (i.e., *cliaddr) is
 *                      determined by the socket's address family. When cliaddr is NULL, nothing is
 *                      filled in; in this case, addrlen should also be NULL.
 * @param[in] addrlen   Value-result argument: The caller must initialize it to contain the size
 *                      (in bytes) of the structure pointed to by cliaddr. On return, it will contain
 *                      the actual size of the peer address.
 *
 * @return On success, the call returns a positive integer that is a handle for the accepted socket. \n
 * On error, -1 is returned.
 * Errors:
 *  EINVAL : The Socket provided is not in listen state.Need to call qapi_listen().
 *  EWOULDBLOCK : If the socket is marked as NON Blocking and socket queue length is zero.
 *  ECONNABORTED : Connection is aborted.
 */

int qapi_accept(int32_t handle, struct sockaddr *cliaddr, int32_t *addrlen)  ;

/** @ingroup qapi_connect
 *
 * Initiates a connection on a socket
 *
 * If the socket is of type SOCK_DGRAM, *svraddr is the address to which datagrams are
 * sent by default, and the only address from which datagrams are received. If the socket
 * is of type SOCK_STREAM, this call attempts to make a connection to the socket that is
 * bound to the address specified by *srvaddr.
 *
 * @param[in] handle    Socket handle returned from qapi_socket().
 * @param[in] srvaddr   Pointer to the peer's address to which the socket is connected. 
 * @param[in] addrlen   Specify the size (in bytes) of *srvaddr.
 *
 * @return On success, 0 is returned. On error, -1 is returned.
 * Errors:
 *  EINVAL : Invalid Arguments passed as argument to connect.
 *  EWOULDBLOCK : If the socket is marked as NON Blocking and connection is in establishing state and not
 *               established.
 *  ECONNABORTED : Connection is aborted.
 *  EISCONN : Connection is already established.
 *  ETIMEDOUT : Connection Establishment Timed out no response from Peer.
 *  ENOBUFFS : No Memory.
 */

qapi_Status_t qapi_connect(int32_t handle, struct sockaddr *srvaddr, int32_t addrlen);

/** @ingroup qapi_setsockopt
 *
 * Sets the options for a socket.
 *
 * @param[in] handle    Socket handle returned from qapi_socket().
 * @param[in] level     Protocol level at which the option exists.
 * @param[in] optname   Name of the option. 
 * @param[in] optval    Pointer to the option value to be set.
 * @param[in] optlen    Option length in bytes.
 *
 * @return On success, 0 is returned. On error, -1 is returned.
 *  EINVAL : Invalid Arguments passed to set the value.
 *  ENOTCONN : Trying to set a socket options which requires socket to be in connected state
 *             for setting the same.
 *  ENP_PARAM : Trying to set a socket option which is either not supported or not present.
 *
 *  ENOBUFFS : No Memory.
 */

qapi_Status_t qapi_setsockopt(int32_t handle, int32_t level, int32_t optname, void *optval, int32_t optlen);

/** @ingroup qapi_getsockopt
 *
 * Gets the options for a socket.
 *
 * @param[in] handle    Socket handle returned from qapi_socket().
 * @param[in] level     Protocol level at which the option exists.
 * @param[in] optname   Name of the option. 
 * @param[in] optval    Pointer to a buffer in which the value for the requested option
 *                      is to be returned.
 * @param[in] optlen    Option length in bytes.
 *
 * @return On success, 0 is returned. On error, -1 is returned.
 *  EINVAL : Invalid Arguments passed to set the value.
 *  ENOTCONN : Trying to set a socket options which requires socket to be in connected state
 *             for setting the same.
 *  ENP_PARAM : Trying to set a socket option which is either not supported or not present.
 *
 *  ENOBUFFS : No Memory.
 */
 
qapi_Status_t qapi_getsockopt(int32_t handle, int32_t level, int32_t optname, void *optval, int32_t optlen);

/** @ingroup qapi_socketclose
 *
 * Closes a socket.
 *
 * @param[in] handle    Socket handle returned from qapi_socket().
 *
 * @return On success, 0 is returned. On error, -1 is returned.
 * Errors:
 *  ENOTCONN: Socket is not connected.
 *  EALREADY: Socket close already in progress.
 *  EINVAL :  Invalid socket parameter.
 */

qapi_Status_t qapi_socketclose(int32_t handle);

/** @ingroup qapi_errno
 *
 * Gets the last error code on a socket.
 *
 * @param[in] handle    Socket handle returned from qapi_socket().
 *
 * @return Socket error code or ENOTSOCK if socket is not found
 * Errors:
 * EFAULT : Invalid socket passed to get the error.
 */

int qapi_errno(int32_t handle);

/** @ingroup qapi_recvfrom
 *
 * Receives a message from a socket.
 *
 * @param[in] handle    Socket handle returned from qapi_socket().
 * @param[in] buf       Pointer to a buffer for the received message.
 * @param[in] len       Number of bytes to receive.       
 * @param[in] flags     0, or it is formed by ORing one or more of: \n
 *                      - MSG_PEEK -- Causes the receive operation to return data from
 *                      the beginning of the receive queue without removing that data
 *                      from the queue. Thus, a subsequent receive call will return
 *                      the same data.
 *                      - MSG_OOB -- Requests receipt of out-of-band data that would not be
 *                      received in the normal data stream.
 *                      - MSG_DONTWAIT -- Enables a nonblocking operation; if the operation
 *                      blocks, the call fails with the error EAGAIN or EWOULDBLOCK. @vertspace{-14}
 * @param[in] from      If not NULL, and the underlying protocol provides the source address,
 *                      this source address is filled in. When NULL, nothing is filled in;
 *                      in this case, fromlen is not used, and should also be NULL.
 * @param[in] fromlen   This is a value-result argument, which the caller should initialize before
 *                      the call to the size of the buffer associated with from, and modified on
 *                      return to indicate the actual size of the source address.
 *
 * @return The number of bytes received, or -1 if an error occurred.
 * Errors:
 *  ENOTCONN : Socket is in disconnected state.
 *  EWOULDBLOCK : If the socket is in Non Blocking state and we have a receive timeout set and
 *                we don't get any data for that duration.
 *  ENOBUFS: No memory.
 */

int qapi_recvfrom(int32_t handle, char *buf, int32_t len, int32_t flags, struct sockaddr *from, int32_t *fromlen);

/** @ingroup qapi_recv
 *
 * Receives a message from a socket.
 *
 * The qapi_recv() call is normally used only on a connected socket
 * and is identical to qapi_recvfrom(handle, buf, len, flags, NULL, NULL)
 *
 * @param[in] handle    Socket handle returned from qapi_socket().
 * @param[in] buf       Pointer to a buffer for the received message.
 * @param[in] len       Number of bytes to receive.       
 * @param[in] flags     0, or it is formed by ORing one or more of: \n
 *                      - MSG_PEEK -- Causes the receive operation to return data from
 *                      the beginning of the receive queue without removing that data
 *                      from the queue. Thus, a subsequent receive call will return
 *                      the same data.
 *                      - MSG_OOB -- Requests receipt of out-of-band data that would not be
 *                      received in the normal data stream.
 *                      - MSG_DONTWAIT -- Enables a nonblocking operation; if the operation
 *                      blocks, the call fails with the error EAGAIN or EWOULDBLOCK. @vertspace{-14} 
 *
 * @return The number of bytes received, or -1 if an error occurred.
 * Errors:
 *  ENOTCONN : Socket is in disconnected state.
 *  EWOULDBLOCK : If the socket is in Non Blocking state and we have a receive timeout set and
 *                we don't get any data for that duration.
 *  EPIPE: For a TCP Socket this shows that peer has disconnected the socket.
 *  ENOBUFS: No memory.
 */

int qapi_recv(int32_t handle, char *buf, int32_t len, int32_t flags);

/** @ingroup qapi_sendto
 *
 * Sends a message on a socket to a target.
 *
 * @param[in] handle    Socket handle returned from qapi_socket().
 * @param[in] buf       Pointer to a buffer containing the message to be sent.
 * @param[in] len       Number of bytes to send.       
 * @param[in] flags     0, or it is formed by ORing one or more of: \n
 *                      - MSG_OOB -- Sends out-of-band data on sockets that support this
 *                      notion (e.g., of type SOCK_STREAM); the underlying protocol
 *                      must also support out-of-band data.
 *                      - MSG_DONTWAIT -- Enables a nonblocking operation; if the operation
 *                      blocks, the call fails with the error EAGAIN or EWOULDBLOCK.
 *                      - MSG_DONTROUTE -- Don not use a gateway to send the packet; only send it to
 *                      hosts on directly-connected networks. This is usually used only by
 *                      diagnostic or routing programs. @vertspace{-14} 
 * @param[in] to        Pointer to the address of the target.
 * @param[in] tolen     Size in bytes of the target address.
 *
 * @return The number of bytes sent, or -1 if an error occurred and errno is set appropriately.
 *  ENOTCONN : Socket is in disconnected state.
 *  EWOULDBLOCK : If the socket is in Non Blocking state and we have a send timeout set and
 *                we don't get any data for that duration.
 *  EPIPE: For a TCP Socket this shows that peer has disconnected the socket.
 *  ENOBUFS: No memory.
 */

int qapi_sendto(int32_t handle, char *buf, int32_t len, int32_t flags, struct sockaddr *to, int32_t tolen);

/** @ingroup qapi_send
 *
 * Send a message on a socket.
 *
 * The call may be used only when the socket is in a connected state
 * (so that the intended recipient is known). It is equivalent to
 * qapi_sendto(handle, buf, len, flags, NULL, 0)
 *
 * @param[in] handle    Socket handle returned from qapi_socket().
 * @param[in] buf       Pointer to a buffer containing message to be sent.
 * @param[in] len       Number of bytes to send.       
 * @param[in] flags     0, or it is formed by ORing one or more of: \n
 *                      - MSG_OOB -- Sends out-of-band data on sockets that support this
 *                      notion (e.g., of type SOCK_STREAM); the underlying protocol
 *                      must also support out-of-band data.
 *                      - MSG_DONTWAIT -- Enables a nonblocking operation; if the operation
 *                      blocks, the call fails with the error EAGAIN or EWOULDBLOCK.
 *                      - MSG_DONTROUTE -- Do not use a gateway to send the packet; only send it to
 *                      hosts on directly-connected networks. This is usually used only by
 *                      diagnostic or routing programs. @vertspace{-14} 
 *
 * @return The number of bytes sent, or -1 if an error occurred and errno is set appropriately.
 *  Errors:
 *    ENOTCONN : Socket is in disconnected state.
 *    EWOULDBLOCK : If the socket is in Non Blocking state and we have a send timeout set and
 *                we don't get any data for that duration.
 *    EPIPE: For a TCP Socket this shows that peer has disconnected the socket.
 *    ENOBUFS: No memory.
 */

int qapi_send(int32_t handle, char *buf, int32_t len, int32_t flags);

/** @ingroup qapi_select
 *
 * Monitors multiple socket handles, waiting until one or more of them become "ready"
 * for some class of I/O operation (e.g., read, write, etc.). 
 *
 * The call causes the calling process to block waiting for activity on any of a
 * list of sockets. Arrays of socket handles are passed for read, write, 
 * and exception events. A timeout in milliseconds is also passed.
 *
 * @param[in] rd            Pointer to a list of read socket handles.
 * @param[in] wr            Pointer to a list of write socket handles.
 * @param[in] ex            Pointer to a list of exception socket handles.
 * @param[in] timeout_ms    Timeout values in milliseconds.
 *
 * @return The number of sockets that had an event occur and became ready.
 */

int qapi_select(fd_set *rd, fd_set *wr, fd_set *ex, int32_t timeout_ms);

/** @ingroup qapi_fd_zero
 *
 * Initializes a socket that is set to zero.
 *
 * @param[in] set   Pointer to a list of sockets.
 *
 * @return On success, 0 is returned. On error, -1 is returned.
 */
qapi_Status_t qapi_fd_zero(fd_set *set);

/** @ingroup qapi_fd_clr
 *
 * Removes a socket from the socket set.
 *
 * @param[in] handle    Socket handle returned from qapi_socket().
 * @param[in] set       Pointer to a list of sockets.
 *
 * @return On success, 0 is returned. On error, -1 is returned.
 */

qapi_Status_t qapi_fd_clr(int32_t handle, fd_set *set);

/** @ingroup qapi_fd_set
 *
 * Adds a socket to the socket set.
 *
 * @param[in] handle    Socket handle returned from qapi_socket().
 * @param[in] set       Pointer to a list of sockets.
 *
 * @return On success, 0 is returned. On error, -1 is returned.
 */

qapi_Status_t qapi_fd_set(int32_t handle, fd_set *set);

/** @ingroup qapi_fd_isset
 *
 * Checks whether a socket is a member of a socket set.
 *
 * @param[in] handle    Socket handle returned from qapi_socket().
 * @param[in] set       Pointer to a list of sockets.
 *
 * @return On success, 0 is returned if the socket is not a member;
 * 1 is returned if the socket is a member. \n
 * On error, -1 is returned.
 */

qapi_Status_t qapi_fd_isset(int32_t handle, fd_set *set);

/** @ingroup qapi_get_peer_name
 *   Returns the address of the peer connected to the socket in the buffer 
 *         pointed by the addr. 
 * @details
 *
 * @param[in] handle    Socket handle returned from qapi_socket()
 * @param[in] addr      Pointer to a user buffer of sockaraddr type which is filled
 *                      by the API with the peer addr information.
 *                       
 *                      
 * @param[in] addrlen   Specifies the size, in bytes, of the address pointed to by addr
 *
 * @return On success, 0 is returned. On error, -1 is returned.
 * Errors:
 * ENOTCONN : Socket is not in connected state.
 * EINVAL: Invalid arguments passed.
 * ENOBUFS: No Memory.
 */

qapi_Status_t qapi_getpeername(int32_t handle, struct sockaddr *addr, int *addrlen);

/** @ingroup qapi_get_sock_name
 *   Returns current address to which the socket is bound in the user provided
 *         buffer addr. 
 *          
 * @details
 *
 * @param[in] handle    Socket handle returned from qapi_socket()
 * @param[in] addr      Pointer to a user buffer of sockaraddr type which is filled
 *                      by the API with the peer addr info.
 *                       
 *                      
 * @param[in] addrlen   Specifies the size, in bytes, of the address pointed to by addr
 *
 * @return On success, 0 is returned. On error, -1 is returned.
 * Errors:
 * ENOTCONN : Socket is not in connected state.
 * EINVAL: Invalid arguments passed.
 * ENOBUFS: No Memory
 */
qapi_Status_t qapi_getsockname(int32_t handle, struct sockaddr *addr, int *addrlen);

/** 
 * @versiontable{2.0,2.45,
 * Data\_Services 1.2.0  &  Introduced. @tblendline
 * }
 * @ingroup qapi_shutdown
 * ShutDown Read , Write or Read Write operations on a socket
 *          
 * @details
 *
 * @param[in] handle    Socket handle returned from qapi_socket()
 * @param[in] how       Shutdown Opeartion Type
                        SHUT_RD
                        SHUT_RW
                        SHUT_RDRW
 *
 * @return On success, 0 is returned. On error, -1 is returned.
 * Errors:
 * ENOTCONN : Socket is not in connected state.
 * EINVAL: Invalid arguments passed.
 * ENOBUFS: No Memory
 */


int qapi_shutdown(int32_t handle , int how);
#endif

#endif /* _QAPI_SOCKET_H_ */
