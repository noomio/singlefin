#ifndef QAPI_HANDLER_REGISTRY_H
#define QAPI_HANDLER_REGISTRY_H
/**
  @file qapi_handler_registry.h
  @brief  Structures and variables used in QAPI handler registering.
  QuRT QAPI Handler Registry APIs performs the registering of QAPI handlers during boot up itself.

EXTERNAL FUNCTIONS
   None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.

Copyright (c) 2019  by Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.


=============================================================================*/
#include "qapi_txm_base.h"


extern volatile ULONG qapi_registry_status; //Contains the respective bit set if any handler has failed to register

/* Handler Registry Entry Type */
typedef struct
{
  UINT id_start;
  UINT num_ids;
  void *handler;
}qapi_handler_registry_item;

#endif // QAPI_HANDLER_REGISTRY_H
