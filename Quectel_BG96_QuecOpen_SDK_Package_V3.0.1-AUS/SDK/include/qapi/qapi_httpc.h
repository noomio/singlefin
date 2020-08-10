/*===========================================================================

  Copyright (c) 2017-2019 Qualcomm Technologies, Inc.
  All Rights Reserved
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

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header:

when       who     what, where, why
--------   ---     ----------------------------------------------------------
2018-10-11 leo    (Tech Comm) Edited/added Doxygen comments and markup.
2017-09-02 leo	  (Tech Comm) Edited/added Doxygen comments and markup.
2017-04-25 leo    (Tech Comm) Edited/added Doxygen comments and markup.
25Jan2017 vpulimi  Created the module. First version of the file.
===========================================================================*/

/*
!! IMPORTANT NOTE: "DATA SERVICES" VERSION CATEGORY.?
*/

/**
  @file
  qapi_httpc.h

  
  HTTP client service provides a collection of API functions that allow the 
  application to enable and configure HTTP client services. The HTTP client 
  can be configured to support IPv4, IPv6, as well as HTTP mode, HTTPS 
  mode (secure) or both. 
*/


#ifndef _QAPI_HTTPC_H_
#define _QAPI_HTTPC_H_

#include <stdint.h>
#include "qapi_status.h"   /* qapi_Status_t */
#include "qapi_ssl.h"

#include "qapi_data_txm_base.h"
#include "qapi_status.h"
#include "qapi_driver_access.h"

/* HTTP client request IDs */
#define  TXM_QAPI_HTTPC_START                      TXM_QAPI_NET_HTTP_BASE + 1
#define  TXM_QAPI_HTTPC_STOP                       TXM_QAPI_NET_HTTP_BASE + 2
#define  TXM_QAPI_HTTPC_NEW_SESSION                TXM_QAPI_NET_HTTP_BASE + 3
#define  TXM_QAPI_HTTPC_FREE_SESSION               TXM_QAPI_NET_HTTP_BASE + 4
#define  TXM_QAPI_HTTPC_CONNECT                    TXM_QAPI_NET_HTTP_BASE + 5
#define  TXM_QAPI_HTTPC_DISCONNECT                 TXM_QAPI_NET_HTTP_BASE + 6
#define  TXM_QAPI_HTTPC_REQUEST                    TXM_QAPI_NET_HTTP_BASE + 7
#define  TXM_QAPI_HTTPC_SET_BODY                   TXM_QAPI_NET_HTTP_BASE + 8
#define  TXM_QAPI_HTTPC_SET_PARAM                  TXM_QAPI_NET_HTTP_BASE + 9
#define  TXM_QAPI_HTTPC_ADD_HEADER_FIELD           TXM_QAPI_NET_HTTP_BASE + 10
#define  TXM_QAPI_HTTPC_CLEAR_HEADER               TXM_QAPI_NET_HTTP_BASE + 11
#define  TXM_QAPI_HTTPC_SSL_CONFIG                 TXM_QAPI_NET_HTTP_BASE + 12
#define  TXM_QAPI_HTTPC_PROXY_CONNECT              TXM_QAPI_NET_HTTP_BASE + 13
#define  TXM_QAPI_HTTPC_CONFIG                     TXM_QAPI_NET_HTTP_BASE + 14


/** @addtogroup qapi_networking_httpc
@{ */

/**
 * @brief HTTP request types supported by qapi_Net_HTTPc_Request().
 */
typedef enum 
{
  /* Supported http client methods */
  QAPI_NET_HTTP_CLIENT_GET_E = 1,
  /**< HTTP get request. */
  QAPI_NET_HTTP_CLIENT_POST_E,
  /**< HTTP post request. */
  QAPI_NET_HTTP_CLIENT_PUT_E,
  /**< HTTP put request. */
  QAPI_NET_HTTP_CLIENT_PATCH_E,
  /**< HTTP patch request. */
  QAPI_NET_HTTP_CLIENT_HEAD_E,
  /**< HTTP head request. */
  QAPI_NET_HTTP_CLIENT_CONNECT_E
  /**< HTTP connect request. */
} qapi_Net_HTTPc_Method_e;

/**
 *  HTTP callback state returned by qapi_HTTPc_CB_t().
 */
typedef enum
{
  QAPI_NET_HTTPC_RX_ERROR_SERVER_CLOSED = -8, 
  /**< HTTP response error -- the server closed the connection. */
  QAPI_NET_HTTPC_RX_ERROR_RX_PROCESS = -7,
  /**< HTTP response error -- response is processing. */
  QAPI_NET_HTTPC_RX_ERROR_RX_HTTP_HEADER = -6,
  /**< HTTP response error -- header is processing. */
  QAPI_NET_HTTPC_RX_ERROR_INVALID_RESPONSECODE = -5,
  /**< HTTP response error -- invalid response code. */
  QAPI_NET_HTTPC_RX_ERROR_CLIENT_TIMEOUT = -4,
  /**< HTTP response error -- timeout waiting for a response. */
  QAPI_NET_HTTPC_RX_ERROR_NO_BUFFER = -3,
  /**< HTTP response error -- memory is unavailable. */
  QAPI_NET_HTTPC_RX_CONNECTION_CLOSED = -2,
  /**< HTTP response -- connection is closed. */
  QAPI_NET_HTTPC_RX_ERROR_CONNECTION_CLOSED = -1,
  /**< HTTP response error -- connection is closed. */
  QAPI_NET_HTTPC_RX_FINISHED = 0,
  /**< HTTP response -- response was received completely. */
  QAPI_NET_HTTPC_RX_MORE_DATA = 1,	
  /**< HTTP response -- there is more response data to be received. */
}qapi_Net_HTTPc_CB_State_e;

/**
 *  HTTP response data returned by qapi_HTTPc_CB_t().
 */
typedef struct 
{
  uint32_t      length;
  /**< HTTP response data buffer length. */
  uint32_t      resp_Code;
  /**< HTTP response code. */
  const void   *data;
  /**< HTTP response data. */  
  const void   *rsp_hdr;
  /**< HTTP response data header. */  
  uint32_t      rsp_hdr_len;
  /**< HTTP response data header length. */
} qapi_Net_HTTPc_Response_t;

/**
 *  HTTP socket options.
 */
typedef struct __qapi_Net_HTTPc_Sock_Opts_s
{  
  int32_t level;
  /**< Specifies the protocol level at which the option resides. */
  int32_t opt_name;
  /**< Socket option name. */
  void *opt_value;
  /**< Socket option value. */
  uint32_t opt_len;
  /**< Socket option length. */
}  /** @cond */ qapi_Net_HTTPc_Sock_Opts_t /** @endcond */;

/**
 *  Structure to configure an HTTP client session. 
 */
typedef  struct __qapi_Net_HTTPc_Config_s
{
  uint16_t addr_type;
  /**< Address type AF_UNSPEC, AF_INET or AF_INET6 (used for DNS resolution only). */
  uint32_t sock_options_cnt; 
  /**< Number of socket options. */
  qapi_Net_HTTPc_Sock_Opts_t *sock_options;
  /**< Socket options -- only the Linger option is currently supported. */
  uint16_t max_send_chunk;
  /**< Maximum send data chunk per transaction. */
  uint16_t max_send_chunk_delay_ms;
  /**< Maximum delay between send data chunks (msec). */
  uint8_t max_send_chunk_retries; 
  /**< Maximum send data chunk retries. */
  uint8_t max_conn_poll_cnt; 
  /**< Maximum connect polling count. */
  uint32_t max_conn_poll_interval_ms; 
  /**< Maximum connect polling interval */  

} qapi_Net_HTTPc_Config_t;

/**
 *  HTTP response user callback registered during qapi_Net_HTTPc_New_sess().
 *
 * @param[in] arg	 User payload information.
 * @param[in] state  HTTP response state.
 * @param[in] value	 HTTP response information.
 */
typedef void (*qapi_HTTPc_CB_t)
(
  void* arg,
  int32_t state,
  void* value
);

static __inline void qapi_httpc_cb_uspace_dispatcher(UINT cb_id,
                                                                  void *app_cb,
                                                                  UINT cb_param1,
                                                                  UINT cb_param2,
                                                                  UINT cb_param3,
                                                                  UINT cb_param4,
                                                                  UINT cb_param5,
                                                                  UINT cb_param6,
                                                                  UINT cb_param7,
                                                                  UINT cb_param8)
{
  void (*pfn_app_cb1) (void *, int32_t, void *);

  if (cb_id == DATA_CUSTOM_CB_HTTP_NEW_SESS)
  {
    qapi_Net_HTTPc_Response_t *http_rsp = (qapi_Net_HTTPc_Response_t *)cb_param3;
 
    pfn_app_cb1 = (void (*)(void *, int32_t, void *))app_cb;
    (pfn_app_cb1)((void *)cb_param1, (int32_t)cb_param2, (void *)cb_param3);
 
    /* Release the allocated resources for the application from the kernel */
    if (http_rsp)
    {
      if (http_rsp->rsp_hdr)
        tx_byte_release((void *)http_rsp->rsp_hdr);
         
      if (http_rsp->data)
        tx_byte_release((void *)http_rsp->data);
	
	  tx_byte_release((void *)http_rsp);
    }
  }
}

/**
 *  HTTP client handle used by most of qapi_Net_HTTPc_xxxx().
 */
typedef void* qapi_Net_HTTPc_handle_t;

/* HTTP client QAPIs */
#ifdef  QAPI_TXM_MODULE     // USER_MODE_DEFS
#define qapi_Net_HTTPc_Start()                    ((qapi_Status_t) (_txm_module_system_call12)(TXM_QAPI_HTTPC_START, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define qapi_Net_HTTPc_Stop()                     ((qapi_Status_t) (_txm_module_system_call12)(TXM_QAPI_HTTPC_STOP, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define qapi_Net_HTTPc_New_sess(a,b,c,d,e,f)      ((qapi_Net_HTTPc_handle_t) (_txm_module_system_call12)(TXM_QAPI_HTTPC_NEW_SESSION, (ULONG) a, (ULONG) b, (ULONG) c, (ULONG) d, (ULONG) e, (ULONG) f, (ULONG) qapi_httpc_cb_uspace_dispatcher, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))

/**
 * Macro that Releases Byte Pool Pointer for HTTP Client
 * Parameter 'a' : Handle 
 * On success, QAPI_OK is returned. On error, QAPI_ERROR is returned.
 * (This Macro is only used in DAM Space)
 */

#define qapi_Net_HTTPc_Free_sess(a)                httpc_destroy_indirection(a, TXM_QAPI_HTTPC_FREE_SESSION)

/** @} */

#define qapi_Net_HTTPc_Connect(a,b,c)             ((qapi_Status_t) (_txm_module_system_call12)(TXM_QAPI_HTTPC_CONNECT, (ULONG) a, (ULONG) b, (ULONG) c, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define qapi_Net_HTTPc_Disconnect(a)              ((qapi_Status_t) (_txm_module_system_call12)(TXM_QAPI_HTTPC_DISCONNECT, (ULONG) a, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define qapi_Net_HTTPc_Request(a,b,c)             ((qapi_Status_t) (_txm_module_system_call12)(TXM_QAPI_HTTPC_REQUEST, (ULONG) a, (ULONG) b, (ULONG) c, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define qapi_Net_HTTPc_Set_Body(a,b,c)            ((qapi_Status_t) (_txm_module_system_call12)(TXM_QAPI_HTTPC_SET_BODY, (ULONG) a, (ULONG) b, (ULONG) c, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define qapi_Net_HTTPc_Set_Param(a,b,c)           ((qapi_Status_t) (_txm_module_system_call12)(TXM_QAPI_HTTPC_SET_PARAM, (ULONG) a, (ULONG) b, (ULONG) c, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define qapi_Net_HTTPc_Add_Header_Field(a,b,c)    ((qapi_Status_t) (_txm_module_system_call12)(TXM_QAPI_HTTPC_ADD_HEADER_FIELD, (ULONG) a, (ULONG) b, (ULONG) c, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define qapi_Net_HTTPc_Clear_Header(a)            ((qapi_Status_t) (_txm_module_system_call12)(TXM_QAPI_HTTPC_CLEAR_HEADER, (ULONG) a, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define qapi_Net_HTTPc_Configure_SSL(a,b)         ((qapi_Status_t) (_txm_module_system_call12)(TXM_QAPI_HTTPC_SSL_CONFIG, (ULONG) a, (ULONG) b, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define qapi_Net_HTTPc_Configure(a,b)             ((qapi_Status_t) (_txm_module_system_call12)(TXM_QAPI_HTTPC_CONFIG, (ULONG) a, (ULONG) b, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define qapi_Net_HTTPc_Proxy_Connect(a,b,c,d)     ((qapi_Status_t) (_txm_module_system_call12)(TXM_QAPI_HTTPC_PROXY_CONNECT, (ULONG) a, (ULONG) b, (ULONG) c, (ULONG) d, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))

/*
 * Macro that Passes Byte Pool Pointer for HTTP Client 
 * Parameter 'a' : Handle
 * Parameter 'b' : Pointer to Byte Pool 
 * On success, QAPI_OK is returned. On error, QAPI_ERROR is returned.
 * (This Macro is only used in DAM Space)
 */

#define qapi_Net_HTTPc_Pass_Pool_Ptr(a,b)          httpc_set_byte_pool(a,b)

/** @} */

static __inline int httpc_set_byte_pool(qapi_Net_HTTPc_handle_t hndl, void *httpc_byte_pool_ptr)
{
  int ret = QAPI_ERROR;
  qapi_cb_params_uspace_ptr_t *uspace_memory_httpc = NULL;
  
  /* Retrieve the user space information stored internally */
  qapi_data_map_handle_to_u_addr(hndl, QAPI_APP_HTTP, (void**)&uspace_memory_httpc);
  /* Handle to user space information already exists */
  if (uspace_memory_httpc)
    return QAPI_OK;
  
  tx_byte_allocate(httpc_byte_pool_ptr, (VOID **) &uspace_memory_httpc, sizeof(qapi_cb_params_uspace_ptr_t), TX_NO_WAIT);
  if (NULL == uspace_memory_httpc)
    return QAPI_ERROR;
  
  memset(uspace_memory_httpc, 0x00, sizeof(qapi_cb_params_uspace_ptr_t));

  /* Initialize user space information */
  uspace_memory_httpc->usr_pool_ptr = httpc_byte_pool_ptr;  

  /* Map the user space information to the handle internally and store */ 
  ret = qapi_data_map_u_addr_to_handle((void *)(hndl), QAPI_APP_HTTP, uspace_memory_httpc, 0x00);
  if (ret != QAPI_OK)
  {
    /* Release the allocated resources */
    if (uspace_memory_httpc)
      tx_byte_release(uspace_memory_httpc);
  }

  return ret;
}

static __inline int httpc_destroy_indirection(qapi_Net_HTTPc_handle_t hndl, int val)
{
  int ret = QAPI_ERROR;
  qapi_cb_params_uspace_ptr_t *uspace_memory_httpc = NULL;
  
  /* Retrieve the user space information stored internally */
  qapi_data_map_handle_to_u_addr(hndl, QAPI_APP_HTTP, (void**)&uspace_memory_httpc);
  
  /* Release the allocated resources */
  if (uspace_memory_httpc)
    tx_byte_release(uspace_memory_httpc);
  
  /* Dispatch the call to the module manager */
  ret = ((qapi_Status_t) (_txm_module_system_call12)(val, (ULONG) hndl, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0));
  return ret;
}

#else

qapi_Status_t qapi_HTTPc_Handler(UINT id, UINT a1, UINT a2, UINT a3, UINT a4, UINT a5, UINT a6, UINT a7, UINT a8, UINT a9, UINT a10, UINT a11, UINT a12);

/**
 * Starts or restarts an HTTP client module.
 *
 * @details This function is invoked to start or restart the HTTP client after it is
 *          stopped via a call to qapi_Net_HTTPc_Stop().
 *
 * @return On success, 0 is returned. Other value on error.
 */
qapi_Status_t qapi_Net_HTTPc_Start(void);

/**
 * Stops an HTTP client module.
 *
 * @details This function is invoked to stop the HTTP client after it was started via 
 *          a call to qapi_Net_HTTPc_Start().
 *
 * @return On success, 0 is returned. Other value on error.
 */
qapi_Status_t qapi_Net_HTTPc_Stop(void);

/**
 * Creates a new HTTP client session. 
 *
 * @details To create a client session, the caller must invoke this function 
 *          and the handle to the newly created context is returned if successful. As part 
 *          of the function call, a user callback function is registered with the HTTP 
 *          client module that gets invoked for that particular session if there is some
 *          response data from the HTTP server. Passing in the SSL context information
 *          ensures that a secure session is created. Any HTTP connection or security 
 *          related configuration can be configured using the configuration QAPIs.
 *
 * @param[in] timeout   Timeout (in ms) of a session method (zero is not recommended).
 * @param[in] ssl_Object_Handle  SSL context for HTTPs connect (zero for no HTTPs session support).
 * @param[in] callback  Register a callback function; NULL for no support for a callback.
 * @param[in] arg       User data payload to be returned by the callback function.
 * @param[in] httpc_Max_Body_Length    Maximum body length for this session.
 * @param[in] httpc_Max_Header_Length  Maximum header length for this session.
 *
 * @return On success, #qapi_Net_HTTPc_handle_t is returned. NULL otherwise.
 */
qapi_Net_HTTPc_handle_t qapi_Net_HTTPc_New_sess(
        uint32_t timeout,
        qapi_Net_SSL_Obj_Hdl_t ssl_Object_Handle,
        qapi_HTTPc_CB_t callback,
        void* arg,
        uint32_t httpc_Max_Body_Length,
        uint32_t httpc_Max_Header_Length);

/**
 * Releases an HTTP client session.
 *
 * @details An HTTP client session that is connected to the HTTP server is disconnected 
            before releasing the resources associated with that session. 
 *
 * @param[in] handle  Handle to the HTTP client session.
 *
 * @return On success, 0 is returned. Other value on error.
 */
qapi_Status_t qapi_Net_HTTPc_Free_sess(qapi_Net_HTTPc_handle_t handle);

/**
 *  Connects an HTTP client session to the HTTP server.
 *
 * @details The HTTP client session is connected to the HTTP server in blocking mode. 
 *
 * @param[in] handle  Handle to the HTTP client session.
 * @param[in] URL     Server URL informtion.  
 * @param[in] port    Server port information. 
 *
 * @return On success, 0 is returned. Other value on error.
 */
qapi_Status_t qapi_Net_HTTPc_Connect(qapi_Net_HTTPc_handle_t handle, const char *URL, uint16_t port);

/**
 *  Connects an HTTP client session to the HTTP proxy server.
 *
 * @details The HTTP client session is connected to the HTTP server in blocking mode. 
 *
 * @param[in] handle        Handle to the HTTP client session.
 * @param[in] URL           Server URL information.  
 * @param[in] port          Server port information. 
 * @param[in] secure_proxy  Secure proxy connection.
 *
 * @return On success, 0 is returned. Other value on error.
 */
qapi_Status_t qapi_Net_HTTPc_Proxy_Connect(qapi_Net_HTTPc_handle_t handle, const char *URL, uint16_t port, uint8_t secure_proxy);

/**
 *  Disconnects an HTTP client session from the HTTP server.
 *
 * @details The HTTP client session that is connected to the HTTP server is disconnected 
            from the HTTP server. 
 *
 * @param[in] handle  Handle to the HTTP client session.
 *
 * @return On success, 0 is returned. Other value on error.
 */
qapi_Status_t qapi_Net_HTTPc_Disconnect(qapi_Net_HTTPc_handle_t handle);

/**
 *  Processes the HTTP client session requests.
 *
 * @details HTTP client session requests are processed and sent to the HTTP server. 
 *
 * @param[in] handle   Handle to the HTTP client session.
 * @param[in] cmd      HTTP request method information. 
 * @param[in] URL      Server URL information.
 *
 * @return On success, 0 is returned. Other value on error.
 */
qapi_Status_t qapi_Net_HTTPc_Request(qapi_Net_HTTPc_handle_t handle, qapi_Net_HTTPc_Method_e cmd, const char *URL);

/**
 *  Sets an HTTP client session body.
 *
 * @details Multiple invocations of this function will result in overwriting the 
            internal data buffer with the content of the last call. Maximum allowed 
            body size is configured at HTTP session creation. Setting body length 
            to zero would clear the HTTP body.   
 *
 * @param[in] handle        Handle to the HTTP client session.
 * @param[in] body          HTTP body related information buffer.
 * @param[in] body_Length   HTTP body buffer length.
 *
 * @return On success, 0 is returned. Other value on error.
 */
qapi_Status_t qapi_Net_HTTPc_Set_Body(qapi_Net_HTTPc_handle_t handle, const char *body, uint32_t body_Length);

/**
 *  Sets an HTTP client session parameter.
 *
 * @details Multiple invocations of this function will result in appending the 
            parameter key-value pair information to the internal data buffer. 
 *
 * @param[in] handle    Handle to the HTTP client session.
 * @param[in] key       HTTP key related information. 
 * @param[in] value     HTTP value associated with the key. 
 *
 * @return On success, 0 is returned. Other value on error.
 */
qapi_Status_t qapi_Net_HTTPc_Set_Param(qapi_Net_HTTPc_handle_t handle, const char *key, const char *value);

/**
 *  Adds an HTTP client session header field.
 *
 * @details Multiple invocations of this function will result in appending the 
            header type-value pair information to the internal header buffer.
            Maximum allowed header size is configured at HTTP session creation.
 *
 * @param[in] handle    Handle to the HTTP client session.
 * @param[in] type      HTTP header type related information. 
 * @param[in] value     HTTP value associated with the header type. 
 *
 * @return On success, 0 is returned. Other value on error.
 */
qapi_Status_t qapi_Net_HTTPc_Add_Header_Field(qapi_Net_HTTPc_handle_t handle, const char *type, const char *value);

/**
 *  Clears an HTTP client session header.
 *
 * @details Invocation of this function clears the entire content associated 
            with the internal header buffer. 
 *
 * @param[in] handle    Handle to the HTTP client session.
 *
 * @return On success, 0 is returned. Other value on error.
 */

qapi_Status_t qapi_Net_HTTPc_Clear_Header(qapi_Net_HTTPc_handle_t handle);

/**
 *  Configures an HTTP client session.
 *
 * @details Invocation of this function configures the HTTP client SSL session. 
 *
 * @param[in] handle    Handle to the HTTP client session.
 * @param[in] ssl_Cfg   SSL configuration information. 
 *
 * @return On success, 0 is returned. Other value on error.
 */
qapi_Status_t qapi_Net_HTTPc_Configure_SSL(qapi_Net_HTTPc_handle_t handle, qapi_Net_SSL_Config_t *ssl_Cfg);

/**
 *  Configures the HTTP client session based on the application requirement.
 *
 * @param[in] handle     Handle to the HTTP client session.
 * @param[in] httpc_Cfg  HTTP client configuration information. 
 *
 * @return On success, 0 is returned. Other value on error.
 */
qapi_Status_t qapi_Net_HTTPc_Configure(qapi_Net_HTTPc_handle_t handle, qapi_Net_HTTPc_Config_t *httpc_Cfg);
 /** @} */

#endif /* QAPI_TXM_MODULE  */
#endif /* _QAPI_HTTPC_H_ */
