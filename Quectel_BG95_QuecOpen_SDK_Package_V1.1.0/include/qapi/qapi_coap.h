
/******************************************************************************
 * Copyright © 2018,2019 Qualcomm Technologies, Inc.
 * \file    qapi_coap.h
 * \author  Qualcomm Technologies, Inc.
 * Interface File Containing Factory Bootstrapping Interfaces and DS
 *
 * This file implements a part of interfacing between LWM2M TO COAP.It consists the all declaration , 
 * prototypes  info  regarding qapi coap 
 ****************************************************************************************/

/**
 * @file qapi_coap.h
 *
 * @addtogroup qapi_networking_coap 
 * @{
 *
 * The Constrained Application Protocol (CoAP) provides a 
 * collection of API functions that allow the application to configure both the CoAP 
 * server and CoAP clients. 
 * 
 * @}
 */

/*
!! IMPORTANT NOTE: "DATA SERVICES" VERSION CATEGORY. 
*/

 #ifndef _QAPI_NET_COAP_H
#define _QAPI_NET_COAP_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "qapi_data_txm_base.h"
#include "qapi_status.h"
#include "qapi_driver_access.h"
#include "qapi_ssl.h"

/** @addtogroup qapi_networking_coap
@{ */

/** CoAP client request IDs. */
#define  TXM_QAPI_COAP_CREATE_SESSION                 TXM_QAPI_COAP_BASE + 1
#define  TXM_QAPI_COAP_DESTROY_SESSION                TXM_QAPI_COAP_BASE + 2
#define  TXM_QAPI_COAP_CLOSE_CONNECTION               TXM_QAPI_COAP_BASE + 3
#define  TXM_QAPI_COAP_CREATE_CONNECTION              TXM_QAPI_COAP_BASE + 4
#define  TXM_QAPI_COAP_INIT_MESSAGE                   TXM_QAPI_COAP_BASE + 5
#define  TXM_QAPI_COAP_SEND_MESSAGE                   TXM_QAPI_COAP_BASE + 6
#define  TXM_QAPI_COAP_FREE_MESSAGE                   TXM_QAPI_COAP_BASE + 7
#define  TXM_QAPI_COAP_SET_HEADER                     TXM_QAPI_COAP_BASE + 8
#define  TXM_QAPI_COAP_GET_HEADER                     TXM_QAPI_COAP_BASE + 9
#define  TXM_QAPI_COAP_SET_PAYLOAD                    TXM_QAPI_COAP_BASE + 10
#define  TXM_QAPI_COAP_GET_EXCHANGE_LIFETIME          TXM_QAPI_COAP_BASE + 11
#define  TXM_QAPI_SET_EXTENDED_CONFIG                 TXM_QAPI_COAP_BASE + 12
#define  TXM_QAPI_COAP_RECONNECT                      TXM_QAPI_COAP_BASE + 13

#define QAPI_COAP_DEFAULT_MAX_AGE                 60     /**< Default maximum age. */
#define QAPI_COAP_RESPONSE_TIMEOUT                2      /**< Default response timeout. */
#define QAPI_COAP_MAX_RETRANSMIT                  4      /**< Default maximum number of retranmissions. */
#define QAPI_COAP_MAX_LATENCY                    100     /**< Default maximum latency in seconds. */
#define QAPI_COAP_ACK_RANDOM_FACTOR              1.5     /**< Default acknowledgement of random factor. */


#define QAPI_COAP_HEADER_LEN                      4      /**< CoAP header length. */
#define QAPI_COAP_ETAG_LEN                        8      /**< Maximum number of bytes for ETag. */
#define QAPI_COAP_TOKEN_LEN                       8      /**< Maximum number of bytes for token. */
#define QAPI_COAP_MAX_ACCEPT_NUM                  2      /**< Maximum number of accept preferences to parse/store. */

#define QAPI_COAP_MAX_OPTION_HEADER_LEN           5      /**< Maximum CoAP option header length in bytes. */

#define QAPI_COAP_HEADER_VERSION_MASK             0xC0   /**< Header version mask. */
#define QAPI_COAP_HEADER_VERSION_POSITION         6      /**< Header version position. */
#define QAPI_COAP_HEADER_TYPE_MASK                0x30   /**< Header type Mask. */
#define QAPI_COAP_HEADER_TYPE_POSITION            4      /**< Header Type position. */
#define QAPI_COAP_HEADER_TOKEN_LEN_MASK           0x0F   /**< Header token length mask. */
#define QAPI_COAP_HEADER_TOKEN_LEN_POSITION       0      /**< Header token length position. */

#define QAPI_COAP_HEADER_OPTION_DELTA_MASK        0xF0   /**< Header option delta mask. */
#define QAPI_COAP_HEADER_OPTION_SHORT_LENGTH_MASK 0x0F   /**< Header option short length mask. */

#define QAPI_COAP_OBJECT_STRING_ID_MAX_LEN        6      /**< Maximum object string ID length. */
#define QAPI_COAP_INSTANCE_STRING_ID_MAX_LEN      6      /**< Maximum instance string ID length . */
#define QAPI_COAP_RESOURCE_STRING_ID_MAX_LEN      6      /**< Maximum resource string ID length. */
#define QAPI_COAP_IFACE_NAME_MAX_LEN              20     /**< Maximum interface name length. */


/**
 * Conservative size limit, as not all options have to be set at the same time.
 */
#ifndef QAPI_COAP_MAX_HEADER_SIZE
/*                            Hdr CoT Age  Tag              Obs  Tok               Blo strings */
#define QAPI_COAP_MAX_HEADER_SIZE  (4 + 3 + 5 + 1+COAP_ETAG_LEN + 3 + 1+COAP_TOKEN_LEN + 4 + 30) /* 70 */
#endif
#define QAPI_COAP_MAX_PACKET_SIZE  (COAP_MAX_HEADER_SIZE + REST_MAX_CHUNK_SIZE)
/*                                        0/14          48 for IPv6 (28 for IPv4) */
#if QAPI_COAP_MAX_PACKET_SIZE > (QAPI_COAP_UIP_BUFSIZE - QAPI_COAP_UIP_LLH_LEN - QAPI_COAP_UIP_IPUDPH_LEN)
//#error "UIP_CONF_BUFFER_SIZE too small for REST_MAX_CHUNK_SIZE"
#endif


/** Bitmap for set options. */
enum { QAPI_OPTION_MAP_SIZE = sizeof(uint8_t) * 8 };
#define QAPI_SET_OPTION(packet, opt) ((packet)->options[opt / QAPI_OPTION_MAP_SIZE] |= 1 << (opt % QAPI_OPTION_MAP_SIZE))
#define QAPI_IS_OPTION(packet, opt) ((packet)->options[opt / QAPI_OPTION_MAP_SIZE] & (1 << (opt % QAPI_OPTION_MAP_SIZE)))

#ifndef MIN
#define MIN(a, b) ((a) < (b)? (a) : (b))
#endif /* MIN */


UINT qapi_Coap_Handler(UINT id, UINT a, UINT b, UINT c, UINT d, UINT e, UINT f, UINT g, UINT h, UINT i, UINT j, UINT k, UINT l);


/** QAPI CoAP message types. */
typedef enum {
  QAPI_COAP_TYPE_CON, /**< Confirmables. */
  QAPI_COAP_TYPE_NON, /**< Non-confirmables. */
  QAPI_COAP_TYPE_ACK, /**< Acknowledgements. */
  QAPI_COAP_TYPE_RST  /**< Reset. */
}qapi_Coap_Message_Type_t;

/** QAPI CoAP request method codes. */
typedef enum {
  QAPI_COAP_EMPTY = 0, /**< Empty Request/Response. */
  QAPI_COAP_GET,       /**< GET Request. */
  QAPI_COAP_POST,      /**< POST Reuqest. */
  QAPI_COAP_PUT,       /**< PUT Request. */
  QAPI_COAP_DELETE     /**< DELETE Request. */
}qapi_Coap_Method_t ;

/** CoAP response codes. */
typedef enum {
  QAPI_NO_ERROR = 0,
  QAPI_COAP_IGNORE = 1,

  QAPI_CREATED_2_01 = 65,                    /**< CREATED. */
  QAPI_DELETED_2_02 = 66,                    /**< DELETED. */
  QAPI_VALID_2_03 = 67,                      /**< NOT_MODIFIED. */
  QAPI_CHANGED_2_04 = 68,                    /**< CHANGED. */
  QAPI_CONTENT_2_05 = 69,                    /**< OK. */
  QAPI_CONTINUE_2_31 = 95,                   /**< CONTINUE in case of block1. */

  QAPI_BAD_REQUEST_4_00 = 128,               /**< BAD_REQUEST. */
  QAPI_UNAUTHORIZED_4_01 = 129,              /**< UNAUTHORIZED. */
  QAPI_BAD_OPTION_4_02 = 130,                /**< BAD_OPTION. */
  QAPI_FORBIDDEN_4_03 = 131,                 /**< FORBIDDEN. */
  QAPI_NOT_FOUND_4_04 = 132,                 /**< NOT_FOUND. */
  QAPI_METHOD_NOT_ALLOWED_4_05 = 133,        /**< METHOD_NOT_ALLOWED. */
  QAPI_NOT_ACCEPTABLE_4_06 = 134,            /**< NOT_ACCEPTABLE. */
  QAPI_REQ_ENTITY_INCOMPLETE_4_08 = 136,     /**< REQ_ENTITY_INCOMPLETE. */
  QAPI_PRECONDITION_FAILED_4_12 = 140,       /**< BAD_REQUEST. */
  QAPI_REQUEST_ENTITY_TOO_LARGE_4_13 = 141,  /**< REQUEST_ENTITY_TOO_LARGE. */
  QAPI_UNSUPPORTED_MEDIA_TYPE_4_15 = 143,    /**< UNSUPPORTED_MEDIA_TYPE. */

  QAPI_INTERNAL_SERVER_ERROR_5_00 = 160,     /**< INTERNAL_SERVER_ERROR. */
  QAPI_NOT_IMPLEMENTED_5_01 = 161,           /**< NOT_IMPLEMENTED. */
  QAPI_BAD_GATEWAY_5_02 = 162,               /**< BAD_GATEWAY. */
  QAPI_SERVICE_UNAVAILABLE_5_03 = 163,       /**< SERVICE_UNAVAILABLE. */
  QAPI_GATEWAY_TIMEOUT_5_04 = 164,           /**< GATEWAY_TIMEOUT. */
  QAPI_PROXYING_NOT_SUPPORTED_5_05 = 165,    /**< PROXYING_NOT_SUPPORTED. */

  /** Erbium errors. */
  QAPI_MEMORY_ALLOCATION_ERROR = 192,
  QAPI_PACKET_SERIALIZATION_ERROR,

  /** Erbium hooks. */
  QAPI_MANUAL_RESPONSE

} qapi_Coap_Status_t;

/** CoAP content types. */
typedef enum {
  QAPI_TEXT_PLAIN = 0,                      /**< Plain text. */
  QAPI_TEXT_XML = 1,                        /**< XML text. */
  QAPI_TEXT_CSV = 2,                        /**< CSV text. */
  QAPI_TEXT_HTML = 3,                       /**< HTML text. */
  QAPI_IMAGE_GIF = 21,                      /**< GIF image. */
  QAPI_IMAGE_JPEG = 22,                     /**< JPEG image. */
  QAPI_IMAGE_PNG = 23,                      /**< PNG image. */
  QAPI_IMAGE_TIFF = 24,                     /**< TIFF image format. */
  QAPI_AUDIO_RAW = 25,                      /**< RAW audio. */
  QAPI_VIDEO_RAW = 26,                      /**< RAW video. */
  QAPI_APPLICATION_LINK_FORMAT = 40,        /**< Application link format. */
  QAPI_APPLICATION_XML = 41,                /**< Application XML. */
  QAPI_APPLICATION_OCTET_STREAM = 42,       /**< Application octet stream. */
  QAPI_APPLICATION_RDF_XML = 43,            /**< Application RDF XML. */
  QAPI_APPLICATION_SOAP_XML = 44,           /**< Application SOAP XML. */
  QAPI_APPLICATION_ATOM_XML = 45,           /**< Application ATOM XML. */
  QAPI_APPLICATION_XMPP_XML = 46,           /**< Application XMPP XML. */
  QAPI_APPLICATION_EXI = 47,                /**< Application EXI. */
  QAPI_APPLICATION_FASTINFOSET = 48,        /**< Application FastInfoSet */
  QAPI_APPLICATION_SOAP_FASTINFOSET = 49,   /**< Application SOAP FastInfoSet. */
  QAPI_APPLICATION_JSON = 50,               /**< Application JSON . */
  QAPI_APPLICATION_X_OBIX_BINARY = 51,      /**< Application X OBIX binary. */
  QAPI_M2M_TLV = 11542,                     /**< M2M TLV. */
  QAPI_M2M_JSON = 11543,                    /**< M2M JSON. */
}qapi_Coap_Content_Type_t;


/** CoAP header options. */
typedef enum {
  QAPI_COAP_OPTION_IF_MATCH = 1,       /**< 0-8 B. */
  QAPI_COAP_OPTION_URI_HOST = 3,       /**< 1-255 B. */
  QAPI_COAP_OPTION_ETAG = 4,           /**< 1-8 B. */
  QAPI_COAP_OPTION_IF_NONE_MATCH = 5,  /**< 0 B. */
  QAPI_COAP_OPTION_OBSERVE = 6,        /**< 0-3 B. */
  QAPI_COAP_OPTION_URI_PORT = 7,       /**< 0-2 B. */
  QAPI_COAP_OPTION_LOCATION_PATH = 8,  /**< 0-255 B. */
  QAPI_COAP_OPTION_URI_PATH = 11,      /**< 0-255 B. */
  QAPI_COAP_OPTION_CONTENT_TYPE = 12,  /**< 0-2 B. */
  QAPI_COAP_OPTION_MAX_AGE = 14,       /**< 0-4 B. */
  QAPI_COAP_OPTION_URI_QUERY = 15,     /**< 0-270 B. */
  QAPI_COAP_OPTION_ACCEPT = 17,        /**< 0-2 B. */
  QAPI_COAP_OPTION_TOKEN = 19,         /**< 1-8 B. */
  QAPI_COAP_OPTION_LOCATION_QUERY = 20, /**< 1-270 B. */
  QAPI_COAP_OPTION_BLOCK2 = 23,        /**< 1-3 B. */
  QAPI_COAP_OPTION_BLOCK1 = 27,        /**< 1-3 B. */
  QAPI_COAP_OPTION_SIZE = 28,          /**< 0-4 B. */
  QAPI_COAP_OPTION_PROXY_URI = 35,     /**< 1-270 B. */
  QAPI_COAP_OPTION_SIZE1 = 60,          /**< 0-4 B. */
  QAPI_COAP_OPTION_MAX                 /**< MAX COAP OPTION VALUE. */
}qapi_Coap_Option_t;

/** Enumerations added for get and set header parameters for CoAP packet.
 */
typedef enum {
  QAPI_COAP_URI_PATH,          /**< URI PATH. */
  QAPI_COAP_URI_QUERY,         /**< URI_QUERY. */
  QAPI_COAP_CONTENT_TYPE,      /**< CONTENT_TYPE. */
  QAPI_COAP_TOKEN,             /**< TOKEN. */
  QAPI_COAP_OBSERVE,           /**< OBSERVE. */
  QAPI_COAP_LOCATION_PATH ,    /**< LOCATION_PATH. */
  QAPI_COAP_SET_STATUS,        /**< SET_STATUS. */
  QAPI_COAP_MULTI_OPTION,      /**< MULTI_OPTION. */
  QAPI_COAP_BLOCK1,            /**< BLOCK1. */
  QAPI_COAP_BLOCK2,            /**< BLOCK2. */
  QAPI_COAP_ETAG,              /**< ETAG. */
  QAPI_COAP_LOCATION_QUERY,    /**< LOCATION_QUERY. */
  QAPI_COAP_MAXAGE,            /**< MAXAGE. */
  QAPI_COAP_ACCEPT,            /**< ACCEPT. */
  QAPI_COAP_URI_HOST,          /**< URI_HOST. */
  QAPI_COAP_SIZE1,             /**< BLOCK SIZE1. */
  QAPI_COAP_SIZE,              /**< BLOCK SIZE. */
}qapi_Coap_Header_type;


/** CoAP security mode Type. */
typedef enum {
  QAPI_COAP_MODE_PSK = 0,             /**< PSK. */
  QAPI_COAP_MODE_RAW_PUBLIC_KEY,      /**< RAW_PUBLIC_KEY. */
  QAPI_COAP_MODE_CERTIFICATE,         /**< CERTIFICATE. */
  QAPI_COAP_MODE_NONE,                /**< NON SEC. */
}qapi_Sec_Mode;

typedef enum {
        QAPI_COAP_PROTOCOL_UDP = 0x1,  /**< PROTOCOL_UDP. */
        QAPI_COAP_PROTOCOL_NIDD = 0x2, /**< PROTOCOL_NIDD. */
}qapi_Coap_Protocol_Type;


typedef enum {
       QAPI_COAP_EXTENDED_CONFIG_NONE, /**< EXTENDED_CONFIG_NONE. */
}qapi_Coap_Extended_Config_Options_t;

typedef void * qapi_Coap_Session_Hdl_t; /**< CoAP Session Handle. */


/** CoAP multi option information. */
typedef struct _qapi_Multi_Option_t {
  struct _qapi_Multi_Option_t *next;   /**< Pointer to next option in multi-option list. */
  uint8_t is_static;                   /**< Option static or not.  */
  uint8_t len;                         /**< Option length. */
  uint8_t *data;                       /**< Pointer to option value. */
}qapi_Multi_Option_t;


/** Parsed message structure. */
typedef struct {
  uint8_t *buffer; /**< Pointer to CoAP header / Incoming packet buffer / Memory to serialize packet. */
  uint8_t version;                        /**< CoAP version number. */
  qapi_Coap_Message_Type_t type;          /**< CoAP message type e.g. CON|NON|ACK|RST. */
  uint8_t code;                           /**< CoAP message code. */
  uint16_t mid;                           /**< CoAP message ID. */
  
  uint8_t options[(QAPI_COAP_OPTION_MAX -1) / QAPI_OPTION_MAP_SIZE + 1]; /**< Bitmap to check if option is set. */
  
  qapi_Coap_Content_Type_t content_type; /**< Parse options once and store; allows setting options in random order. */
  uint32_t max_age;                       /**< Max-Age Option indicates the maximum time a response may be cached . */
  size_t proxy_uri_len;                   /**< Proxy-Uri length.*/
  uint8_t *proxy_uri;                     /**< Proxy-Uri indicates URI to be used to make a request to forward-proxy .*/
  uint8_t etag_len;                       /**< ETAG option length.*/
  uint8_t etag[QAPI_COAP_ETAG_LEN];       /**< Option for differentiating between representations of the same resource that vary over time.*/  
  size_t uri_host_len;                    /**< Length of Uri-Host.*/
  uint8_t * uri_host;                     /**< Pointer to Uri-Host that specify the target server or client .*/
  qapi_Multi_Option_t *location_path;     /**< Length of location query.*/
  uint16_t uri_port;                      /**< Port of the target coap server/client to connect with .*/
  size_t location_query_len;              /**< Pointer to CoAP server/client to connect with .*/
  uint8_t *location_query;                /**< Pointer to query that results to a specific target resource  .*/
  qapi_Multi_Option_t *uri_path;          /**< Path that specify the target resource.*/
  uint32_t observe;                       /**< Option for implementing CoAP observe functionality  .*/
  uint8_t token_len;                      /**< Length of the Token ID .*/
  uint8_t token[QAPI_COAP_TOKEN_LEN];     /**< Token ID of the CoAP message .*/
  uint8_t accept_num;                     /**< Length of the accept field .*/
  
  qapi_Coap_Content_Type_t accept[QAPI_COAP_MAX_ACCEPT_NUM]; /**< The option used to indicate Content-Format is acceptable to the target server or client .*/
  
  uint8_t if_match_len;                   /**< Length of the accept if match field.*/
  uint8_t if_match[QAPI_COAP_ETAG_LEN];   /**< Option to make a request for one or more ETAG representations of the target resource .*/
  uint32_t block2_num;                    /**< Block number of the CoAP message with BLOCK2 option.*/
  uint8_t block2_more;                    /**< More blocks followed or not during BLockwise transfer  .*/
  uint16_t block2_size;                   /**< Size of the payload during BLockwise transfer .*/
  uint32_t block2_offset;                 /**< Block2 offset number.*/
  uint32_t block1_num;                    /**< Block number of the CoAP message with BLOCK1 option .*/
  uint8_t block1_more;                    /**< More blocks followed or not during blockwise transfer .*/
  uint16_t block1_size;                   /**< Size of the payload during blockwise transfer .*/
  uint32_t block1_offset;                 /**< Block1 offset number .*/
  uint32_t size;                          /**< Specifies the CoAP Message size.*/
  uint32_t size1;                         /**< Size1 option provides size information about the resource representation in a request.*/
  qapi_Multi_Option_t * uri_query;        /**< Query path that specices target resource .*/
  uint8_t if_none_match;                  /**< Option used to request conditional on the non existence of the target resource .*/
  uint16_t payload_len;                   /**< Length of the payload contained in CoAP message .*/
  uint8_t *payload;                       /**< Payload contained in the CoAP message .*/
  uint16_t buffer_len;                    /**< Length of the buffer .*/

}qapi_Coap_Packet_t;

/** CoAP transaction info for the request/response received from network. */
struct _qapi_Coap_Transaction_ 
{
   uint16_t            mID;              /**< MID of the message whose response this CB is called. */
  qapi_Coap_Packet_t * message;          /**< Message in whose reply this CB is called. */
  uint8_t *            buffer;           /**< RAW Buffer Payload of message whose response is this CB. */
  void *               userData;         /**< Usr Data Passed in the message in whose reply this CB is called. */
  uint16_t             buffer_len;       /**< RAW Buffer Length of message whose response is this CB. */
  uint8_t              ack_received;     /**< If set, implies a response to our message, else a timeout has happened. */
  time_t               response_timeout; /**< Timeout associated with this message. */
  uint8_t              retrans_counter;  /**< Number for times this message was re-transmitted. */
  time_t               retrans_time;     /**< Re-transmit time associated with this message. */
};


typedef struct _qapi_Coap_Transaction_  qapi_Coap_Transaction_t;


/**  Callback to be invoked when CoAP DL request is received
   This callback will be called in response to a DL message from the server to the CoAP 
   Client. The application needs to set this callback in qapi_Coap_Create_Session() via 
   qapi_Coap_Session_Info_t. 
   */
typedef  int32_t (*qapi_Coap_Client_Session_Cb)(qapi_Coap_Session_Hdl_t  hdl, qapi_Coap_Packet_t * message , void * usr_data);

/** CoAP block data transfer parameters.
   Application can set and get these parameters in a message via qapi_Coap_Set_Header() / 
   qapi_Coap_Get_Header. 
*/
typedef struct qapi_Coap_Block_Params_s{
  uint32_t  num ;           /**< Block number. */
  uint8_t   more;           /**< Additional data. */
  uint16_t  size;           /**< Block size. */
  uint32_t  offset;         /**< Block offset. */
}qapi_Coap_Block_Params_t;

/** CoAP structure to hold the information related to 
   CERTIFICATES/PSK which will be used by CoAP to create a secure connection client that 
   can provide information for any one of qapi_Sec_Modes. Security type to be used by the 
   application when calling qapi_Coap_Create_Connection(). 
*/
typedef struct qapi_Coap_Sec_Info_s {
   qapi_Net_SSL_Cert_t cert;             /**< IN PARAM:Certificate location. */ 
   qapi_Net_SSL_CAList_t ca_list;        /**< IN PARAM:CA List location. */
   qapi_Net_SSL_PSKTable_t psk;          /**< IN PARAM:PSK location. */
   qapi_Net_SSL_DICERT_t di_cert;        /**< IN PARAM DI_CERY location. */
}qapi_Coap_Sec_Info_t;

/** CoAP client configuration information to be set while creating a CoAP Session via
    qapi_Coap_Create_Session(). Use the default values for the configuration parameters.
*/
typedef struct qapi_Coap_Session_Info_s {
  void *   clientData ;                   /**< IN PARAM : Usr_data. This usr_data will be passed in the DL Callback. */
  uint32_t coap_max_retransmits;          /**< Optional IN PARAM : Max Number of COAP Rxmts for CON Messages DEF VAL =  4. */
  uint32_t coap_transaction_timeout;      /**< Optional IN PARAM :Transaction Timeout for CON Messages DEF VAL = 2 Seconds. */
  double   coap_ack_random_factor;        /**< Optional IN PARAM :DEF VAL = 1.5. */
  uint32_t coap_max_latency;              /**< Optional IN PARAM :DEF VAL = 100. */
  uint32_t coap_default_maxage;           /**< Optional IN PARAM :DEF VAL = 60. */
  qapi_Coap_Client_Session_Cb cb;         /**< IN PARAM : Callback to be called as a part of any DL Requests ex GET/PUT/POST?DELETE. */

}qapi_Coap_Session_Info_t;

/** Connection establishment configuration parameters
   qapi_Coap_Create_Connection().
*/
typedef struct qapi_Coap_Connection_Cfg_s {
   qapi_Sec_Mode  sec_Mode;              /**< SEC or NON SEC. */
   qapi_Coap_Protocol_Type proto;        /**< UDP or NIDD. */
   uint32_t   callHndl ;                 /**< Data call handle ,to be used for NIDD.*/
   char *   src_host;                    /**< Local IP. */
   uint16_t src_port;                    /**< Local port. */
   char *   dst_host;                    /**< Remote IP. */
   uint16_t dst_port;                    /**< Remote port. */
   uint16_t family_type;                 /**< IPv4 or IPv6. */
   boolean  resumption_enabled;          /**< Enble DTLS resumption. */ 
   int      session_resumption_timeout;  /**< Session timeout. */
   qapi_Net_SSL_Config_t * ssl_cfg;      /**< SSL configuration to be associated with the connection. */
   qapi_Coap_Sec_Info_t *  sec_info;     /**< Certificates/ PSK location information
                                         */
   boolean  disable_close_notify;        /**< Disable Close-Notify when shutting down the connection. */ 
}qapi_Coap_Connection_Cfg_t;


/**  
* This callback will be called in response to a CON message sent by the CoAP client via qapi_Coap_Send_Message().
* Application needs to set this callback in qapi_Coap_Send_Message() via qapi_Coap_Message_Params_t
*/

typedef void (*qapi_Coap_Transaction_Callback_t) (qapi_Coap_Session_Hdl_t hdl,qapi_Coap_Transaction_t * transacP, qapi_Coap_Packet_t* message);


/** CoAP configuration structure to be passed for creating a message. */
typedef struct qapi_Coap_Message_s {
  uint16_t * lastmid;                     /**< OUTPARAM: MID used to send the message. */
  uint8_t  * token;                       /**< INPARAM: Token value to be used to send the message. */
  uint8_t token_len;                      /**< INPARAM: Token length. */
  qapi_Coap_Transaction_Callback_t msg_cb;/**< INPARAM: This callback will be called in response to a CON message 
                                                        sent by the application. The application needs to set this callback in 
                                                        qapi_Coap_Send_Message()
                                          */
  void * msgUsrData;                      /**< INPARAM: Application user data associated with this message
                                                      same will be returned as a part of msg_cb set
                                                      above for CON messages. */
}qapi_Coap_Message_Params_t;
/** @} */

static __inline void qapi_Coap_Client_Session_uspace_dispatcher(UINT cb_id,
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
  void (*pfn_app_cb1) (qapi_Coap_Session_Hdl_t , qapi_Coap_Packet_t *, void *);
  void (*pfn_app_cb2) (qapi_Coap_Session_Hdl_t  ,qapi_Coap_Transaction_t *, qapi_Coap_Packet_t *);

  if (cb_id == DATA_CUSTOM_CB_COAP_CREATE_SESSION)
  {
    pfn_app_cb1 = (void (*)(qapi_Coap_Session_Hdl_t , qapi_Coap_Packet_t *, void *))app_cb;
    (pfn_app_cb1)((qapi_Coap_Session_Hdl_t)cb_param1, (qapi_Coap_Packet_t *)cb_param2, (void *)cb_param3);
  }
  if (cb_id == DATA_CUSTOM_CB_COAP_TRANSACTION)
  {
    pfn_app_cb2 = (void (*)(qapi_Coap_Session_Hdl_t ,qapi_Coap_Transaction_t *, qapi_Coap_Packet_t *))app_cb;
    (pfn_app_cb2)((qapi_Coap_Session_Hdl_t)cb_param1, (qapi_Coap_Transaction_t *) cb_param2,(qapi_Coap_Packet_t *)cb_param3);
  }
}



#ifdef  QAPI_TXM_MODULE     // USER_MODE_DEFS
  
    #define qapi_Coap_Create_Session(a,b)                  ((qapi_Status_t) (_txm_module_system_call12)(TXM_QAPI_COAP_CREATE_SESSION, (ULONG) a, (ULONG) b,  (ULONG) qapi_Coap_Client_Session_uspace_dispatcher, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
    #define qapi_Coap_Destroy_Session(a)                     ((qapi_Status_t) (_txm_module_system_call12)(TXM_QAPI_COAP_DESTROY_SESSION, (ULONG) a, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
    #define qapi_Coap_Close_Connection(a)                    ((qapi_Status_t) (_txm_module_system_call12)(TXM_QAPI_COAP_CLOSE_CONNECTION, (ULONG) a, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
    #define qapi_Coap_Create_Connection(a,b)                 ((qapi_Status_t) (_txm_module_system_call12)(TXM_QAPI_COAP_CREATE_CONNECTION, (ULONG) a, (ULONG) b, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
    #define qapi_Coap_Init_Message(a,b,c,d)                    ((qapi_Status_t) (_txm_module_system_call12)(TXM_QAPI_COAP_INIT_MESSAGE, (ULONG) a, (ULONG) b, (ULONG) c, (ULONG)d, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
    #define qapi_Coap_Send_Message(a,b,c)                      ((qapi_Status_t) (_txm_module_system_call12)(TXM_QAPI_COAP_SEND_MESSAGE, (ULONG) a, (ULONG) b, (ULONG) c, (ULONG) qapi_Coap_Client_Session_uspace_dispatcher, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
    #define qapi_Coap_Free_Message(a,b)                        ((qapi_Status_t) (_txm_module_system_call12)(TXM_QAPI_COAP_FREE_MESSAGE, (ULONG) a, (ULONG) b, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
    #define qapi_Coap_Set_Header(a,b,c,d,e)                    ((qapi_Status_t) (_txm_module_system_call12)(TXM_QAPI_COAP_SET_HEADER, (ULONG) a, (ULONG) b, (ULONG) c, (ULONG) d, (ULONG) e, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
    #define qapi_Coap_Get_Header(a,b,c,d,e)                    ((qapi_Status_t) (_txm_module_system_call12)(TXM_QAPI_COAP_GET_HEADER, (ULONG) a, (ULONG) b, (ULONG) c, (ULONG) d, (ULONG) e, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
    #define qapi_Coap_Set_Payload(a,b,c,d)                     ((qapi_Status_t) (_txm_module_system_call12)(TXM_QAPI_COAP_SET_PAYLOAD, (ULONG) a, (ULONG) b, (ULONG) c, (ULONG) d, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
    #define qapi_Coap_Get_Exchange_Lifetime(a,b)             ((qapi_Status_t) (_txm_module_system_call12)(TXM_QAPI_COAP_GET_EXCHANGE_LIFETIME, (ULONG) a, (ULONG) b, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
    #define qapi_Coap_Set_Extended_Config_Option(a,b,c,d)    ((qapi_Status_t) (_txm_module_system_call12)(TXM_QAPI_SET_EXTENDED_CONFIG, (ULONG) a, (ULONG) b, (ULONG) c, (ULONG) d, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
    #define qapi_Coap_Reconnect(a,b,c)                       ((qapi_Status_t) (_txm_module_system_call12)(TXM_QAPI_COAP_RECONNECT, (ULONG) a, (ULONG) b, (ULONG) c, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))


/** @addtogroup qapi_networking_coap
@{ */


/**
 * Macro that passes a byte pool pointer for the CoAP client.
 *
 * Parameter a -- Handle. \n
 * Parameter b -- Pointer to byte pool.
 *
 * On success, QAPI_OK is returned. On error, QAPI_ERROR is returned.
 *
 * @note1hang This macro is only used in the DAM space.
 */


#define qapi_Coap_Pass_Pool_Ptr(a,b)                      coap_update_byte_pool(a, b)


static __inline int coap_update_byte_pool(qapi_Coap_Session_Hdl_t hndl, void *coap_byte_pool_ptr)
{
  int ret = QAPI_ERROR;
  qapi_cb_params_uspace_ptr_t *uspace_memory_coap = NULL;
  
  /* Retrieve the user space information stored internally */
  qapi_data_map_handle_to_u_addr(hndl, QAPI_APP_COAP, (void**)&uspace_memory_coap);
  /* Handle to user space information already exists */
  if (uspace_memory_coap)
    return QAPI_OK;
  
  tx_byte_allocate(coap_byte_pool_ptr, (VOID **) &uspace_memory_coap, sizeof(qapi_cb_params_uspace_ptr_t), TX_NO_WAIT);
  if (NULL == uspace_memory_coap)
    return QAPI_ERROR;
  
  memset(uspace_memory_coap, 0x00, sizeof(qapi_cb_params_uspace_ptr_t));

  /* Initialize user space information */
  uspace_memory_coap->usr_pool_ptr = coap_byte_pool_ptr;  

  /* Map the user space information to the handle internally and store */ 
  ret = qapi_data_map_u_addr_to_handle((void *)(hndl), QAPI_APP_COAP, uspace_memory_coap, 0x00);
  if (ret != QAPI_OK)
  {
    /* Release the allocated resources */
    if (uspace_memory_coap)
      tx_byte_release(uspace_memory_coap);
  }

  return ret;
}

#else

/**
 * @versiontable{2.0,2.45,
 * Data\_Services 1.2.0  &  Introduced. @tblendline
 * }
 *   This function is used to create a client CoAP Session.
 *  
 * @param [out] session            Pointer to a QAPI coap client session context that 
 *                                 will be returned and is to be used for all other
 *                                 further CoAPQAPIs associated with this session.
 * @param [in] coap_session_config Configuration parameters associated with 
 *           this session.
 * @return on success            -  QAPI_OK 
 *         on failure            -  QAPI_ERROR
 */
qapi_Status_t qapi_Coap_Create_Session (qapi_Coap_Session_Hdl_t  * session, qapi_Coap_Session_Info_t * coap_session_config);


/**
 * @versiontable{2.0,2.45,
 * Data\_Services 1.2.0  &  Introduced. @tblendline
 * }
 *    This function is used to destroy a CoAP session.
 *  
 * @param  sessionHandle  CoAP Session Handle returned by Create Session.
 * @return on success - QAPI_OK
 *         on failure            -  QAPI_ERROR
 */
qapi_Status_t qapi_Coap_Destroy_Session(qapi_Coap_Session_Hdl_t sessionHandle);


/** 
 * @versiontable{2.0,2.45,
 * Data\_Services 1.2.0  &  Introduced. @tblendline
 * }
 *         This function is used to create a COAP connection with the remote server.

 *  
 * @param  sessionHandle     CoAP Session Handle returned by Create Session.
 * @param  coap_conn_config  Connection related config Parameters.
 * @return on success - QAPI_OK 
 *         on failure - QAPI_ERROR
 */
qapi_Status_t qapi_Coap_Create_Connection(qapi_Coap_Session_Hdl_t sessionHandle,qapi_Coap_Connection_Cfg_t * coap_conn_config );


/**
 * @versiontable{2.0,2.45,
 * Data\_Services 1.2.0  &  Introduced. @tblendline
 * }
 *   This function is used to close a CoAP aonnection associated with the session.
 *  
 * @param  sessionHandle  CoAP session handle returned by Create Session.
 * @return on success            -  QAPI_OK 
 *         on failure            -  QAPI_ERROR
 */
qapi_Status_t qapi_Coap_Close_Connection(qapi_Coap_Session_Hdl_t sessionHandle);


/**
 * @versiontable{2.0,2.45,
 * Data\_Services 1.2.0  &  Introduced. @tblendline
 * Data\_Services 1.5.0  &  API added in dam module. @tblendline
 * }
 *    Reconnect DTLS connection.
 *  
 * @param sessionhandle              CoAP Session Handle returned by Create Session .
 * @param new_local_ip               Local IP address if there is a change, or pass NULL to to use 
 *                                   previous IP address.
 * @param session_resumption_time    Session resumption timeout value .
 * @return New connection pointer
 */
qapi_Status_t qapi_Coap_Reconnect(qapi_Coap_Session_Hdl_t sessionhandle, char * new_local_ip, int session_resumption_time);


/** 
 * @versiontable{2.0,2.45,
 * Data\_Services 1.2.0  &  Introduced. @tblendline
 * }
 *    This function is used to create a new request message.
 * @param sessionHandle   CoAP Session Handle returned by Create Session .
 * @param message         Pointer to qapi_Coap_Packet_t. The memory for this packet will
 *                        be allocated by this API.
 * @param coap_msg_type   CON/NON-CON/ACK/RESET.
 * @param msg_code        GET/POST/PUT/DELETE.
 * @return on success - QAPI_OK 
 *         on failure - QAPI_ERROR
 */
qapi_Status_t qapi_Coap_Init_Message(qapi_Coap_Session_Hdl_t sessionHandle , qapi_Coap_Packet_t ** message , qapi_Coap_Message_Type_t coap_msg_type ,uint8_t msg_code );


/**
 * @versiontable{2.0,2.45,
 * Data\_Services 1.2.0  &  Introduced. @tblendline
 * }
 *         This function is used to send the message to coap. Failures would internally
 *   release the allocated resources.
 *  
 * @param sessionHandle    CoAP Session Handle returned by Create Session .
 * @param message          Pointer to qapi CoAP message.
 * @param msg_conf         Pointer to CoAP Message config structure
 * return  on success   -  0
 *         on error     - -1 
 */
qapi_Status_t qapi_Coap_Send_Message(qapi_Coap_Session_Hdl_t  sessionHandle , qapi_Coap_Packet_t * message, qapi_Coap_Message_Params_t * msg_conf);

/**
 * @versiontable{2.0,2.45,
 * Data\_Services 1.2.0  &  Introduced. @tblendline
 * } 
 *          This function is used to free the coap message.
 * @param sessionHandle   CoAP Session Handle returned by Create Session .
 * @param message         Pointer to qapi coap message 
 * @return  void
 */
qapi_Status_t qapi_Coap_Free_Message(qapi_Coap_Session_Hdl_t  sessionHandle ,qapi_Coap_Packet_t *message);


/** 
 * @versiontable{2.0,2.45,
 * Data\_Services 1.2.0  &  Introduced. @tblendline
 * }
 *     This function is used to set header parameters.
 *  
 * @param session      CoAP Session Handle returned by Create Session .
 * @param message      Pointer to coap message header to be filled.
 * @param header_type  Type of header field to be filled.
 * @param header_val   Pointer to header value.
 * @param val_len      Length of header value.
 * @return int value 
 */
qapi_Status_t qapi_Coap_Set_Header(qapi_Coap_Session_Hdl_t  session ,qapi_Coap_Packet_t*message , qapi_Coap_Header_type header_type , const void *header_val , size_t val_len); 


/**
 * @versiontable{2.0,2.45,
 * Data\_Services 1.2.0  &  Introduced. @tblendline
 * Data\_Services 1.5.0  &  Updated behaviour. @tblendline
 * }
 *        This function is used to get header parameters. 
 *
 *
 * @param session      CoAP Session Handle returned by Create Session .
 * @param message      Pointer to coap message header to get.
 * @param header_type  Type of header field to get.
 * @param header_val   Pointer to header value to store.
 * @param val_len      Length of header value.
 * 
 * Memory freeing information for this API : 
 * For header type such as QAPI_COAP_URI_PATH | QAPI_COAP_URI_QUERY |QAPI_COAP_LOCATION_PATH : 
 * API caller whether in DAM or Kernel has to explicitly free the memory allocated to the string ,
 * contained by header_val .
 *
 */
qapi_Status_t qapi_Coap_Get_Header(qapi_Coap_Session_Hdl_t  session ,qapi_Coap_Packet_t *message , qapi_Coap_Header_type header_type , void **header_val , size_t * val_len);


/**
 * @versiontable{2.0,2.45,
 * Data\_Services 1.2.0  &  Introduced. @tblendline
 * }
 *          This function is used to set coap payload.
 *
 * @param session      CoAP Session Handle returned by Create Session .
 * @param   packet     Pointer to qapi coap packet.
 * @param   payload    Pointer to payload.
 * @param   length     Length of payload. 
 * @return  on success - 1 
 *          on error   - 0
 */
qapi_Status_t qapi_Coap_Set_Payload(qapi_Coap_Session_Hdl_t  session ,qapi_Coap_Packet_t *packet, const void *payload, size_t length);


/**
 * @versiontable{2.0,2.45,
 * Data\_Services 1.2.0  &  Introduced. @tblendline
 * }
 *          This function is used to get the coap exchange lifetime.
 *          EXCHANGE_LIFETIME is the time from starting to send a confirmable message to
 *          the time when an acknowledgement is no longer expected; i.e., message-layer
 *          information about the message exchange can be purged.  EXCHANGE_LIFETIME
 *          includes a MAX_TRANSMIT_SPAN, a MAX_LATENCY forward, PROCESSING_DELAY, and a
 *          MAX_LATENCY for the way back. EXCHANGE_LIFETIME simplifies to:
 *          MAX_TRANSMIT_SPAN + (2 * MAX_LATENCY) + PROCESSING_DELAY or 247 seconds with
 *          the default transmission parameters.
 * @param session            CoAP Session Handle returned by Create Session .
 * @param exchange_lifetime  Pointer to lifetime. 
 * @return  on success - 1 
 *          on error   - 0
 */
qapi_Status_t  qapi_Coap_Get_Exchange_Lifetime(qapi_Coap_Session_Hdl_t  session  ,uint32_t *exchange_lifetime);


/**
 * @versiontable{2.0,2.45,
 * Data\_Services 1.2.0  &  Introduced. @tblendline
 * }
 *     This function which is used to set extended.
 *          configuration parameters. It is expected that the user will call this API
 *          before these parameters are being used in another API.
 * @param  session  CoAP Session Handle returned by Create Session .
 * @param   option  Option Type which we need to set.
 * @param   val     Values to be set.
 * @param   len     Option length.
 *          
 * @return  on success - 1 
 *          on error   - 0
 */
qapi_Status_t qapi_Coap_Set_Extended_Config_Option(qapi_Coap_Session_Hdl_t session , qapi_Coap_Extended_Config_Options_t option,
                                                                                                       void * val , int32_t len);
/** @} */

#endif
#endif
