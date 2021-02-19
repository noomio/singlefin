/*
Copyright (c) 2017-2019 by Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

/*===========================================================================

							Edit History

  $Header: //components/rel/core.tx/5.0/api/services/qapi_diag_codes.h#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/11/18   leo     (Tech Comm) Edited/added Doxygen comments and markup.
10/31/17   gn      Changes to support command deregistration
04/21/17   gn      Added Doxgyen comments and markup.
04/10/17   gn      DIAG QAPI header file name change to qapi_diag.h
03/24/17   gn      Changes to support DIAG delayed reponses on User space
02/24/17   gn      Changes to support DIAG QAPIs with DAM framework
===========================================================================*/

/** @file qapi_diag_codes.h
*/

#ifndef QAPI_DIAG_CODES_H
#define QAPI_DIAG_CODES_H 

#include "qapi_txm_base.h" // For TXM_QAPI_DIAG_BASE

/* Diag TXM IDs start from TXM_QAPI_DIAG_BASE and total IDs allocated for Diag are TXM_QAPI_DIAG_NUM_IDS */

/* TXM Bases for Diag services */
#define TXM_QAPI_DIAG_PACKET_BASE                 TXM_QAPI_DIAG_BASE
#define TXM_QAPI_DIAG_MSG_BASE                    TXM_QAPI_DIAG_PACKET_BASE  + 0x10
#define TXM_QAPI_DIAG_LOG_BASE                    TXM_QAPI_DIAG_MSG_BASE     + 0x10
#define TXM_QAPI_DIAG_EVENT_BASE                  TXM_QAPI_DIAG_LOG_BASE     + 0x10

/* TXM IDs for Diag packet service */
#define TXM_QAPI_DIAG_DISPATCH_TABLE_REGISTER	  TXM_QAPI_DIAG_PACKET_BASE
#define TXM_QAPI_DIAGPKT_GET_NEXT_DELAYED_RSP_ID  TXM_QAPI_DIAG_PACKET_BASE  + 0x1
#define TXM_QAPI_DIAGPKT_COMMIT                   TXM_QAPI_DIAG_PACKET_BASE  + 0x2
#define TXM_QAPI_DIAG_DISPATCH_TABLE_DEREGISTER   TXM_QAPI_DIAG_PACKET_BASE  + 0x3

/* TXM IDs for Diag messaging service */
#define TXM_QAPI_DIAG_MSG	 					  TXM_QAPI_DIAG_MSG_BASE
#define TXM_QAPI_DIAG_MSG_SPRINTF	 			  TXM_QAPI_DIAG_MSG_BASE     + 0x1

/* TXM IDs for Diag logging service */
#define TXM_QAPI_DIAG_LOG_SUBMIT	 		      TXM_QAPI_DIAG_LOG_BASE 
#define TXM_QAPI_DIAG_LOG_SET_LENGTH			  TXM_QAPI_DIAG_LOG_BASE     + 0x1
#define TXM_QAPI_DIAG_LOG_SET_CODE		    	  TXM_QAPI_DIAG_LOG_BASE     + 0x2
#define TXM_QAPI_DIAG_LOG_SET_TIMESTAMP			  TXM_QAPI_DIAG_LOG_BASE     + 0x3
#define TXM_QAPI_DIAG_LOG_STATUS		    	  TXM_QAPI_DIAG_LOG_BASE     + 0x4

/* TXM IDs for Diag event reporting service */
#define TXM_QAPI_DIAG_EVENT_REPORT		    	  TXM_QAPI_DIAG_EVENT_BASE 
#define TXM_QAPI_DIAG_EVENT_REPORT_PAYLOAD		  TXM_QAPI_DIAG_EVENT_BASE   + 0x1

#ifdef FEATURE_MMU_ENABLED  
/* Callback IDs for Diag */
#define QC_DIAG_CALLBACK_CMD_RSP_ID               CUSTOM_CB_QAPI_DIAG_BASE
#endif

/** @addtogroup qapi_diag
@{ */

/** 
 * @brief Flag to be used with qapi_diagpkt_commit to commit immediate responses.
 */
#define IMMEDIATE_RSP_FLAG 0x1
/** 
 * @brief Flag to be used with qapi_diagpkt_commit to commit delayed responses.
 */
#define DELAYED_RSP_FLAG   0x2

/** @} */ /* end_addtogroup qapi_diag */

#endif /* QAPI_DIAG_CODES_H */
