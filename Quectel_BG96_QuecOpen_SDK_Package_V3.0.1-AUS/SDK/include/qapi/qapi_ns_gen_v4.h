/* Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
   All rights reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

/* Copyright (c) 2015 Qualcomm Atheros, Inc.
   All rights reserved.
   Qualcomm Atheros Confidential and Proprietary.
*/ 
/**
 @file qapi_ns_gen_v4.h

 @brief 
 IPv4 Networking Services (NS)
*/

#ifndef _QAPI_NS_GEN_V4_H_
#define _QAPI_NS_GEN_V4_H_

#include "stdint.h"
#include "qapi_netservices.h"

#include "qapi_data_txm_base.h"
#include "qapi_status.h"


/* Net QAPI REQUEST ID DEFINES */
#define  TXM_QAPI_NET_IPV4_CONFIG                       TXM_QAPI_NET_BASE + 1
#define  TXM_QAPI_NET_PING                              TXM_QAPI_NET_BASE + 2
#define  TXM_QAPI_NET_IPV4_ROUTE                        TXM_QAPI_NET_BASE + 3
#define  TXM_QAPI_NET_PING_2                            TXM_QAPI_NET_BASE + 21
#define  TXM_QAPI_NET_PING_3                            TXM_QAPI_NET_BASE + 23

/** @addtogroup qapi_net_services_datatypes
@{ */

/** 
 * Verifies whether the IPv4 address is multicast.
 *
 * This macro returns 1 if the passed IPv4 address is multicast.
 * IPv4 multicast addresses are in the range 224.0.0.0 through 239.255.255.255.
 *
 * @param[in] ipv4_Address    IPv4 address to check; must be in host order.
 *
 * @return    1 if the IPv4 address is multicast, 0 otherwise.
 *
 */

#define  QAPI_IPV4_IS_MULTICAST(ipv4_Address) \
	(((long)(ipv4_Address) &  0xf0000000) == 0xe0000000)

#define IF_NAMELEN   20        /**< Default maximum length for interface names. */

/**
 * @brief IPv4 ping input.
 */
typedef struct qapi_Net_Ping_V4_s
{
   uint32_t ipv4_addr;
   /**< Destination to ping. */

   uint32_t ipv4_src;
   /**< Source address. */

   uint32_t size;
   /**< Packet size. */

   uint32_t timeout;
   /**< Timeout value (in ms). */

} qapi_Net_Ping_V4_t;

#define QAPI_NET_PING_V4_DST_ADDR_MASK  0x0001
#define QAPI_NET_PING_V4_SRC_ADDR_MASK  0x0002
#define QAPI_NET_PING_V4_PKT_SIZE_MASK  0x0004
#define QAPI_NET_PING_V4_TIMEOUT_MASK   0x0008
#define QAPI_NET_PING_V4_TTL_MASK       0x0010
	 
/**
* @brief IPv4 ping input.
*/
typedef struct qapi_Net_Ping_V4_R2_s
{
	   uint32_t bitmask;
	   /**<  Bitmask */
	 
	   uint32_t ipv4_addr;
	   /**< Destination to ping. */
	 
	   uint32_t ipv4_src;
	   /**< Source address. */
	 
	   uint32_t size;
	   /**< Packet size. */
	 
	   uint32_t timeout;
	   /**< Timeout value (in ms). */
	 
	   uint32_t ttl;
	   /**< Time to live (TTL) or hop limit is a mechanism that limits the lifespan or lifetime of data in a computer or network */
	   
} qapi_Net_Ping_V4_R2_t;

/**Commands for the IPv4 configuration QAPI. */
typedef enum
{
    QAPI_NET_IPV4CFG_QUERY_E,
    /**< Get the IPv4 parameters of an interface, such as IP address, subnet mask, and default gateway. */

    QAPI_NET_IPV4CFG_STATIC_IP_E,
    /**< Assign the IPv4 address, subnet mask, and default gateway. */

    QAPI_NET_IPV4CFG_DHCP_IP_E,
    /**< Run the DHCPv4 client to obtain IPv4 parameters from the DHCPv4 server. */

    QAPI_NET_IPV4CFG_AUTO_IP_E,
    /**< Run auto-IP (automatic private IP addressing). */

    QAPI_NET_IPV4CFG_MAX_E
} qapi_Net_IPv4cfg_Command_t;

/**IPv4 routing object.  */
typedef struct
{
    uint32_t RSVD;
    /**< Reserved. */

    uint32_t ipRouteDest;
    /**< Destination IPv4 address of this route. */

    uint32_t ipRouteMask;
    /**< Indicates the mask to be logically ANDed with the
         destination address before being compared to the
         value in the ipRouteDest field. */

    uint32_t ipRouteNextHop;
    /**< IPv4 address of the next hop of this route.*/

    uint32_t ipRouteIfIndex;
    /**< Index value that uniquely identifies the local
         interface through which the next hop of this
         route should be reached. */

    uint32_t ipRouteProto;
    /**< Routing mechanism via which this route was learned. */

    char     ifName[IF_NAMELEN];
    /**< Textual name of the interface. */

} qapi_Net_IPv4_Route_t;

/**Maximum IPv4 routing configurations. */
#define QAPI_NET_IPV4_MAX_ROUTES    (3)

/**IPv4 routing objects list. */
typedef struct
{
    uint32_t                route_Count;
    /**< Number of #qapi_Net_IPv4_Route_t arrays in the routing table. */

    qapi_Net_IPv4_Route_t   route[QAPI_NET_IPV4_MAX_ROUTES];
    /**< Array of #qapi_Net_IPv4_Route_t types. */

} qapi_Net_IPv4_Route_List_t;

/** @} */ /* end_addtogroup qapi_net_services_datatypes */

#ifdef  QAPI_TXM_MODULE     // USER_MODE_DEFS

#define qapi_Net_IPv4_Config(a,b,c,d,e)                      ((qapi_Status_t) (_txm_module_system_call12)(TXM_QAPI_NET_IPV4_CONFIG                 , (ULONG) a, (ULONG) b, (ULONG) c, (ULONG) d, (ULONG) e, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define qapi_Net_Ping(a,b)                                   ((qapi_Status_t) (_txm_module_system_call12)(TXM_QAPI_NET_PING                        , (ULONG) a, (ULONG) b, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define qapi_Net_IPv4_Route(a,b,c,d,e,f)                     ((qapi_Status_t) (_txm_module_system_call12)(TXM_QAPI_NET_IPV4_ROUTE                  , (ULONG) a, (ULONG) b, (ULONG) c, (ULONG) d, (ULONG) e, (ULONG) f, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define qapi_Net_Ping_2(a,b)                                 ((qapi_Status_t) (_txm_module_system_call12)(TXM_QAPI_NET_PING_2                      , (ULONG) a, (ULONG) b, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define qapi_Net_Ping_3(a,b)                                 ((qapi_Status_t) (_txm_module_system_call12)(TXM_QAPI_NET_PING_3                      , (ULONG) a, (ULONG) b, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))

#else

/** @ingroup qapi_Net_IPv4_Config
 * Sets/gets IPv4 parameters, or triggers the DHCP client.
 *
 * @param[in] interface_Name    Pointer to the interface name.
 * @param[in] cmd   Command mode. Possible values are:
 *                  - QAPI_NET_IPv4CFG_QUERY_E (0) -- Get the IPv4 parameters of an interface.
 *                  - QAPI_NET_IPv4CFG_STATIC_IP_E (1) -- Assign the IPv4 address, subnet mask, and default gateway. @tablebulletend
 * @param[in] ipv4_Addr  Pointer to the IPv4 address in host order.
 * @param[in] subnet_Mask  Pointer to the IPv4 subnet mask in host order.
 * @param[in] gateway    Pointer to the IPv4 gateway address in host order.
 *
 * @return On success, 0 is returned. On error, -1 is returned.
 */
qapi_Status_t qapi_Net_IPv4_Config(
                const char *interface_Name,
                qapi_Net_IPv4cfg_Command_t cmd,
                uint32_t *ipv4_Addr,
                uint32_t *subnet_Mask,
                uint32_t *gateway);

/** @ingroup qapi_Net_Ping
 * 
 * Sends an IPv4 ping.
 *
 * @param[in] ipv4_Addr  IPv4 destination address in network order.
 * @param[in] size       Size of the ping payload in bytes.
 *
 * @return On success, 0 is returned. On error, -1 is returned.
 */
qapi_Status_t qapi_Net_Ping(uint32_t ipv4_Addr, uint32_t size);

/**	@ingroup qapi_Net_Ping2
 * @brief Sends an IPv4 ping request.
 * 
 * @param[in]      ping_buf     Pointer to IPv4 ping structure. The structure will
                           take the IPv4 destination address in network order, 
                           the IPv4 address to which to send the ping via this source,
                           the number of data bytes to send, and a Ping request timeout 
                           value (in ms). 
 * @param[out] ping_resp   Pointer to where to store the ping response code and the
                           type for the ICMP echo response received.
 *
 * @return
 * QAPI_OK --    Successful ping response is received. \n
 * QAPI_ERROR -- The response buffer is filled with an error code.
 */

qapi_Status_t qapi_Net_Ping_2(qapi_Net_Ping_V4_t *ping_buf, qapi_Ping_Info_Resp_t *ping_resp);

/** @ingroup qapi_Net_Ping3
* @brief Sends an IPv4 ping request.
* @param[in]      ping_buf     Pointer to IPv4 ping structure. The structure will
		                       take the IPv4 destination address in network order,
                               the IPv4 address to which to send the ping via this source,
			                   the number of data bytes to send, and a Ping request timeout
		                       value (in ms).
* @param[out]     ping_resp    Pointer to where to store the ping response code and the
	                           type for the ICMP echo response received.							   
* @return
	 * QAPI_OK --    Successful ping response is received. \n
	 * QAPI_ERROR -- The response buffer is filled with an error code.
*/
qapi_Status_t qapi_Net_Ping_3(qapi_Net_Ping_V4_R2_t *ping_buf, qapi_Ping_Info_Resp_R2_t *ping_resp);

/** @ingroup qapi_Net_IPv4_Route
 * 
 * Adds, deletes, or queries the IPv4 route.
 *
 * @param[in] interface_Name    Pointer to the interface name.
 * @param[in] cmd   Command mode. Possible values are: \n
 *                  - QAPI_NET_ROUTE_ADD_E (0) -- Add route.
 *                  - QAPI_NET_ROUTE_DEL_E (1) -- Delete route.
 *                  - QAPI_NET_ROUTE_SHOW_E (2) -- Show route. @tablebulletend
 * @param[in] ipv4_Addr  Pointer to the IPv4 address in host order.
 * @param[in] subnet_Mask    Pointer to the IPv4 subnet mask in host order.
 * @param[in] gateway    Pointer to the IPv4 gateway address in host order.
 * @param[in] route_List    Pointer to the buffer to contain the list of routes, 
                           returned with the QAPI_NET_ROUTE_SHOW_E command. 
 *
 * @return On success, 0 is returned. On error, -1 is returned.
 */
qapi_Status_t qapi_Net_IPv4_Route(
                const char *interface_Name,
                qapi_Net_Route_Command_t cmd,
                uint32_t *ipv4_Addr,
                uint32_t *subnet_Mask,
                uint32_t *gateway,
                qapi_Net_IPv4_Route_List_t *route_List);

#endif

#endif /* _QAPI_NS_GEN_V4_H_ */
