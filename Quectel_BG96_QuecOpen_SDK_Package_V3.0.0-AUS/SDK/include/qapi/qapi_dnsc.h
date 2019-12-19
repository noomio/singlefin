/*===========================================================================
  Copyright (c) 2017-2018  Qualcomm Technologies, Inc.  All Rights Reserved.
 
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies, Inc. and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies, Inc.
 ======================================================================*/

/**
 * @file qapi_dnsc.h
 *
 * @details The DNS (Domain Name System) Client service provides a collection
 * of API functions that allow the application to both configure DNS services
 * in the system, as well as translate domain names to their numerical IPv4
 * or IPv6 (or both) addresses, needed for the purpose of initiating
 * communications with a remote server or service.
 * The DNS service can be either manually configured, or automatically
 * configured when the DHCP client is enabled.
 *
 */



#ifndef _QAPI_DNSC_H_
#define _QAPI_DNSC_H_

#include "stdint.h"
#include "qapi_data_txm_base.h"
#include "qapi_status.h"

/* DNSc QAPI REQUEST ID DEFINES */
#define  TXM_QAPI_NET_DNSC_IS_STARTED                   TXM_QAPI_NET_DNSC_BASE + 1
#define  TXM_QAPI_NET_DNSC_COMMAND                      TXM_QAPI_NET_DNSC_BASE + 2
#define  TXM_QAPI_NET_DNSC_RESHOST                      TXM_QAPI_NET_DNSC_BASE + 3
#define  TXM_QAPI_NET_DNSC_RESHOST_ON_IFACE             TXM_QAPI_NET_DNSC_BASE + 4
#define  TXM_QAPI_NET_DNSC_GET_SERVER_LIST              TXM_QAPI_NET_DNSC_BASE + 5
#define  TXM_QAPI_NET_DNSC_ADD_SERVER                   TXM_QAPI_NET_DNSC_BASE + 6
#define  TXM_QAPI_NEt_DNSC_ADD_SERVER_ON_IFACE          TXM_QAPI_NET_DNSC_BASE + 7
#define  TXM_QAPI_NET_DNSC_DEL_SERVER                   TXM_QAPI_NET_DNSC_BASE + 8
#define  TXM_QAPI_NET_DNSC_DEL_SERVER_ON_IFACE          TXM_QAPI_NET_DNSC_BASE + 9
#define  TXM_QAPI_NET_DNSC_GETHOSTBYNAME                TXM_QAPI_NET_DNSC_BASE + 10
#define  TXM_QAPI_NET_DNSC_GETHOSTBYNAME2               TXM_QAPI_NET_DNSC_BASE + 11
#define  TXM_QAPI_NET_DNSC_GET_SERVER_INDEX             TXM_QAPI_NET_DNSC_BASE + 12
#define  TXM_QAPI_NET_DNSC_SET_CLIENT_TIMEOUT           TXM_QAPI_NET_DNSC_BASE + 13

/** @addtogroup qapi_net_dnsc_datatypes
@{ */

/** For use with qapi_Net_DNSc_Get_Server_List() to get IP addresses of DNS servers. */
#define MAX_DNS_SVR_NUM 4


/** DNS server port. */
#define     QAPI_DNS_PORT       53

/* Number of DNS servers in the system, which is a tunable configuration.
 *  Use ANY_SERVER_ID to populate a free entry, or use an index to update a specific entry.
 */
#define QAPI_NET_DNS_ANY_SERVER_ID         0xFFFF
/** DNS IPv4 primary server ID. */
#define QAPI_NET_DNS_V4_PRIMARY_SERVER_ID    0
/** DNS IPv4 secondary server ID. */
#define QAPI_NET_DNS_V4_SECONDARY_SERVER_ID  1
/** DNS IPv6 primary server ID. */
#define QAPI_NET_DNS_V6_PRIMARY_SERVER_ID    2
/** DNS IPv6 secondary server ID. */
#define QAPI_NET_DNS_V6_SECONDARY_SERVER_ID  3

/** Commands to start/stop/disable a DNS client. */

typedef enum {
    QAPI_NET_DNS_DISABLE_E, /**< Functionality is deprecated. Do not use. */
    QAPI_NET_DNS_START_E,   /**< Allocate space for internal data structures when called for
                                 the first time. For subsequent calls, increases the ref count; DNS query
                                 is allowed after the start command. Processes DNS responses from
                                 the server. */
    QAPI_NET_DNS_STOP_E     /**< Stop sending DNS requests and processing DNS responses;
                                 keeps internal data structures. Frees the space for internal data structures
                                 only when the ref count reaches 0. */
} qapi_Net_DNS_Command_t;

/**
 * @brief Use with qapi_Net_DNSc_Get_Server_List() to get IP addresses of DNS servers.
 */
typedef struct {
    struct ip46addr svr[MAX_DNS_SVR_NUM]; /**< DNS servers IP addresses. */
} qapi_Net_DNS_Server_List_t;

/**
 * @brief Data structure returned from qapi_gethostbyname() or qapi_gethostbyname2().
 *        Same as the UNIX struct hostent{}.
 */
struct qapi_hostent_s {
    char  *h_name;		 /**< Official name of the host. */
    char **h_aliases;	 /**< Alias list. */
    int    h_addrtype;	 /**< Host address type. */
    int    h_length; 	 /**< Length of the address. */
    char **h_addr_list;  /**< List of addresses. */
};

/** @} */

#ifdef  QAPI_TXM_MODULE     // USER_MODE_DEFS

#define qapi_Net_DNSc_Is_Started()                      ((UINT) (_txm_module_system_call12)(TXM_QAPI_NET_DNSC_IS_STARTED             , (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define qapi_Net_DNSc_Command(a)                        ((UINT) (_txm_module_system_call12)(TXM_QAPI_NET_DNSC_COMMAND                , (ULONG) a, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define qapi_Net_DNSc_Reshost(a,b)                      ((UINT) (_txm_module_system_call12)(TXM_QAPI_NET_DNSC_RESHOST                , (ULONG) a, (ULONG) b, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define qapi_Net_DNSc_Reshost_on_iface(a,b,c)           ((UINT) (_txm_module_system_call12)(TXM_QAPI_NET_DNSC_RESHOST_ON_IFACE       , (ULONG) a, (ULONG) b, (ULONG) c, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define qapi_Net_DNSc_Get_Server_List(a,b)              ((UINT) (_txm_module_system_call12)(TXM_QAPI_NET_DNSC_GET_SERVER_LIST        , (ULONG) a, (ULONG) b, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define qapi_Net_DNSc_Add_Server(a,b)                   ((UINT) (_txm_module_system_call12)(TXM_QAPI_NET_DNSC_ADD_SERVER             , (ULONG) a, (ULONG) b, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define qapi_Net_DNSc_Add_Server_on_iface(a,b,c)        ((UINT) (_txm_module_system_call12)(TXM_QAPI_NEt_DNSC_ADD_SERVER_ON_IFACE    , (ULONG) a, (ULONG) b, (ULONG) c, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define qapi_Net_DNSc_Del_Server(a)                     ((UINT) (_txm_module_system_call12)(TXM_QAPI_NET_DNSC_DEL_SERVER             , (ULONG) a, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define qapi_Net_DNSc_Del_Server_on_iface(a,b)          ((UINT) (_txm_module_system_call12)(TXM_QAPI_NET_DNSC_DEL_SERVER_ON_IFACE    , (ULONG) a, (ULONG) b, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define qapi_gethostbyname(a)                           ((qapi_hostent_s *) (_txm_module_system_call12)(TXM_QAPI_NET_DNSC_GETHOSTBYNAME          , (ULONG) a, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define qapi_gethostbyname2(a,b)                        ((qapi_hostent_s *) (_txm_module_system_call12)(TXM_QAPI_NET_DNSC_GETHOSTBYNAME2         , (ULONG) a, (ULONG) b, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define qapi_Net_DNSc_Get_Server_Index(a,b,c)           ((qapi_Status_t) (_txm_module_system_call12)(TXM_QAPI_NET_DNSC_GET_SERVER_INDEX       , (ULONG) a, (ULONG) b, (ULONG) c, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define qapi_Net_DNSc_Host_By_Name(a,b)                 ((UINT)(_txm_module_system_call12)(TXM_QAPI_NET_DNSC_GETHOSTBYNAME           , (ULONG) a, (ULONG) b, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define qapi_Net_DNSc_Get_Host_By_Name2(a,b,c)          ((UINT) (_txm_module_system_call12)(TXM_QAPI_NET_DNSC_GETHOSTBYNAME2         , (ULONG) a, (ULONG) b, (ULONG) c, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define qapi_Net_DNSc_Set_Client_timeout(a)             ((UINT) (_txm_module_system_call12)(TXM_QAPI_NET_DNSC_SET_CLIENT_TIMEOUT     , (ULONG) a, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))


#else

UINT qapi_Net_DNSc_Handler(UINT id, UINT a1, UINT a2, UINT a3, UINT a4, UINT a5, UINT a6, UINT a7, UINT a8, UINT a9, UINT a10, UINT a11, UINT a12);

/** @ingroup qapi_Net_DNSc_Is_Started
 * @brief Checks whether the DNS client has started.
 *
 * @return 0 if not started or 1 if started.
 */
int32_t qapi_Net_DNSc_Is_Started(void);

/** @ingroup qapi_Net_DNSc_Command
*
* Starts, stops, or disables the DNS client.
*
* @param[in] cmd   Command to start/stop/disable the DNS client.
*                  The supported commands are QAPI_NET_DNS_START_E, 
*                  QAPI_NET_DNS_STOP_E, and QAPI_NET_DNS_DISABLE_E.
* @return On success, 0 is returned. On error, -1 is returned.
*/

int32_t qapi_Net_DNSc_Command(qapi_Net_DNS_Command_t cmd);

/** @ingroup qapi_Net_DNSc_Reshost
 * @brief Resolves an IP address text string into an actual IP address.
 *
 * @param[in] hostname  Pointer to an IP address string or host name string.
 * @param[in] ipaddr    Pointer to struct ip46addr for the resolved IP address. 
 *                      The caller must specify which IP address (v4 or v6) it intends
 *                      to resolve to: \n
 *                      If ipaddr type is AF_INET, resolve to an IPv4 address. \n
 *                      If ipaddr type is AF_INET6, resolve to an IPv6 address.
 *
 * @return On success, 0 is returned. On error, < 0 is returned.
 */

int32_t qapi_Net_DNSc_Reshost(char *hostname, struct ip46addr *ipaddr);

/** @ingroup qapi_Net_DNSc_Reshost_Iface
 * @brief Resolves an IP address text string into an actual IP address for an interface.
 *
 * @param[in] hostname  Pointer to an IP address string or host name string.
 * @param[in] addr      Pointer to struct ip46addr for the resolved IP address.
 *                      The caller must specify which IP address (v4 or v6) it intends
 *                      to resolve to: \n
 *                      If addr type is AF_INET, resolve to an IPv4 address. \n
 *                      If addr type is AF_INET6, resolve to an IPv6 address.
 * @param[in] iface_index Name of the PDN/APN for which the address text string is to be resolved.
 *
 * @return On success, 0 is returned. On error, < 0 is returned.
 */

int32_t qapi_Net_DNSc_Reshost_on_iface(char *hostname, struct ip46addr *addr, char *iface_index);

/** @ingroup qapi_Net_DNSc_Get_Server_List
 * @brief Gets the list of configured DNS servers.
 *
 * @param[in] svr_list  Pointer to a buffer to contain the list.
 * @param[in] iface_index  Index of the configured DNS servers.
 *
 * @return On success, 0 is returned. On error, -1 is returned.
 */

int32_t qapi_Net_DNSc_Get_Server_List(qapi_Net_DNS_Server_List_t *svr_list, uint8_t iface_index);

/** @ingroup qapi_Net_DNSc_Add_Server
 * @brief Adds a DNS server to the system.
 *
 * @param[in] svr_addr Pointer to the DNS server's IP address string.
 *
 * @param[in] id    Server ID can be QAPI_NET_DNS_V4_PRIMARY_SERVER_ID,
 *                  QAPI_NET_DNS_V4_SECONDARY_SERVER_ID, QAPI_NET_DNS_V6_PRIMARY_SERVER_ID,
 *                  QAPI_NET_DNS_V6_SECONDARY_SERVER_ID, or QAPI_NET_DNS_ANY_SERVER_ID.
 *
 * @return On success, 0 is returned. On error, -1 is returned.
 */

int32_t qapi_Net_DNSc_Add_Server(char *svr_addr, uint32_t id);

/** @ingroup qapi_Net_DNSc_Add_Server_Iface
 * @brief Adds a DNS server to a PDN interface.
 *
 * @param[in] svr_addr  Pointer to DNS server's IP address string.
 *
 * @param[in] id    Server ID can be QAPI_NET_DNS_V4_PRIMARY_SERVER_ID,
 *                  QAPI_NET_DNS_V4_SECONDARY_SERVER_ID, QAPI_NET_DNS_V6_PRIMARY_SERVER_ID,
 *                  QAPI_NET_DNS_V6_SECONDARY_SERVER_ID, or QAPI_NET_DNS_ANY_SERVER_ID.
 *
 * @param[in] iface  Pointer to the name of the PDN on which the server is to be added.
 *
 * @return On success, 0 is returned. On error, -1 is returned.
 */

int32_t qapi_Net_DNSc_Add_Server_on_iface(char *svr_addr, uint32_t id, char *iface);


/** @ingroup qapi_Net_DNSc_Del_Server
 * @brief Removes a DNS server from the system.
 *
 * @param[in] id    Server ID, can be: \n
 *                  QAPI_NET_DNS_V4_PRIMARY_SERVER_ID, \n
 *                  QAPI_NET_DNS_V4_SECONDARY_SERVER_ID, \n
 *                  QAPI_NET_DNS_V6_PRIMARY_SERVER_ID, \n
 *                  QAPI_NET_DNS_V6_SECONDARY_SERVER_ID, or \n
 *                  QAPI_NET_DNS_ANY_SERVER_ID
 *
 * @return On success, 0 is returned. On error, -1 is returned.
 */

int32_t qapi_Net_DNSc_Del_Server(uint32_t id);

/** @ingroup qapi_Net_DNSc_Del_Server_Iface
 * @brief Removes a DNS server from an interface.
 *
 * @param[in] id    Server ID, can be: \n
 *                  QAPI_NET_DNS_V4_PRIMARY_SERVER_ID, \n
 *                  QAPI_NET_DNS_V4_SECONDARY_SERVER_ID, \n
 *                  QAPI_NET_DNS_V6_PRIMARY_SERVER_ID, \n
 *                  QAPI_NET_DNS_V6_SECONDARY_SERVER_ID, or \n
 *                  QAPI_NET_DNS_ANY_SERVER_ID
 *
 * @param[in] iface_index Name of interface from which to delete a DNS server.
 *
 * @return On success, 0 is returned. On error, -1 is returned.
 */

int32_t qapi_Net_DNSc_Del_Server_on_iface(uint32_t id, char *iface_index);

/** @cond */

/**
 * @brief Gets the host information for an IPv4 host with name.
 *
 * @details Implements a standard Unix version of gethostbyname().
 *          The returned structure should not be freed by the caller.
 *
 * @param[in] name  Pointer to either a host name or an IPv4 address in standard dot notation.
 *
 * @return On success, a pointer to a hostent structure. \n
 *         On error, NULL is returned.
 */
struct qapi_hostent_s * qapi_Net_DNSc_Get_Host_By_Name(char *name);

/**
 * @brief Gets the host information for an IPv4/Ipv6 host by name.
 *
 * @details Implements a standard Unix version of gethostbyname2().
 *          The returned hostent structure is not thread safe. It can be
 *          freed by internal DNS client routines if the entry ages out or if the
 *          table becomes full and space is needed for another entry.
 *
 * @param[in] name  Pointer to either a host name, an IPv4 address in standard dot notation,
 *                  or an IPv6 address in colon notation.
 * @param[in] af    Address family, either AF_INET or AF_INET6.
 *
 * @return On success, a pointer to a hostent structure. \n
 *         On error, NULL is returned.
 */
struct qapi_hostent_s * qapi_Net_DNSc_Get_Host_By_Name2(char *name, int32_t af);

/** @endcond */

/** @ingroup qapi_Net_DNSc_Get_Host_By_Name
 * @brief Gets the host information for an IPv4 host by name.
 *
 * @details Implements a standard Unix version of gethostbyname().
 *          The returned structure should not be freed by the caller.
 *
 * @param[in] name  Pointer to either a host name or an IPv4 address in standard dot notation.
 *
 * @param[out] ipaddr Resolved host information.
 *
 * @return On success, a pointer to a hostent structure. \n
 *         On error, NULL is returned.
 */
qapi_Status_t qapi_Net_DNSc_Host_By_Name(char *name, struct qapi_hostent_s* ipaddr);

/** @ingroup qapi_Net_DNSc_Get_Host_By_Name2
 * @brief Get host information for an IPv4/Ipv6 host by name.
 *
 * @details Implements a standard Unix version of gethostbyname2().
 *          The returned hostent structure is not thread safe. It can be
 *          freed by internal DNS client routines if the entry ages out or if the
 *          table becomes full and space is needed for another entry.
 *
 * @param[in] name  Pointer to either a host name, an IPv4 address in standard dot notation,
 *                  or an IPv6 address in colon notation.
 * @param[in] af    Address family, either AF_INET or AF_INET6.
 * @param[out] ipaddr Resolved host information.
 *
 * @return On success, a pointer to a hostent structure. \n
 *         On error, NULL is returned.
 */
qapi_Status_t qapi_Net_DNSc_Host_By_Name2(char *name, int32_t af, struct qapi_hostent_s* ipaddr);

/** @addtogroup qapi_net_dnsc_datatypes
@{ */

/**
 * Macro that returns a pointer to the hostent structure of a host with the given name.
 */
#define gethostbyname(__name) qapi_Net_DNSc_Get_Host_By_Name(__name)

/** @} */ /* end_addtogroup_qapi_net_dnsc_datatypes */      

/** @ingroup qapi_Net_DNSc_Get_Server_Index
 * @brief Gets the index at which a DNS server is added to the system.
 *
 * @param[in] svr_addr  Pointer to the DNS server's IP address string.
 *
 * @param[in] id  Pointer to the server index. This is filled with the position
 * at which svr_addr is added.
 *
 * @param[in] iface  Pointer to the interface string on which the server is added.
 * 
 * @return On success, QAPI_OK is returned. On error, QAPI_ERROR is returned.
 *
 */

qapi_Status_t qapi_Net_DNSc_Get_Server_Index(char *svr_addr, uint32_t *id, char *iface);

/** @addtogroup qapi_net_dnsc_datatypes
@{ */

/*
 * Macro that returns a pointer to a hostent struct of a host with the given name
 * and family(v4 or v6).
 *
 */
#define gethostbyname2(__name, __af) qapi_Net_DNSc_Get_Host_By_Name2(__name, __af)

/** @} */


qapi_Status_t qapi_Net_DNSc_Set_Client_timeout(uint32_t timeout_ms);

#endif

#endif /* _QAPI_DNSC_H_ */
