/*===========================================================================
                         TEST_UTILS.C

DESCRIPTION
   File contains the utilities for the DAM demo application 
  
   Copyright (c) 2017 by Qualcomm Technologies INCORPORATED.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/
#include "test_utils.h"

TX_BYTE_POOL *byte_pool_test;

#define TEST_BYTE_POOL_SIZE 30720*8 

UCHAR free_memory_test[TEST_BYTE_POOL_SIZE];

/*=========================================================================
  FUNCTION:  dam_byte_pool_init

===========================================================================*/
/*!
    @brief
    Create byte pool for DAM application.

    @return
    None
*/
/*=========================================================================*/
qapi_Status_t dam_byte_pool_init(void)
{
  int ret;

  do
  {
    /* Allocate byte_pool_dam (memory heap) */
    ret = txm_module_object_allocate(&byte_pool_test, sizeof(TX_BYTE_POOL));
    if(ret != TX_SUCCESS)
    {
      LOG_ERROR("DAM_APP:Allocate byte_pool_dam fail \n");
      break;
    }

    /* Create byte_pool_dam */
    ret = tx_byte_pool_create(byte_pool_test, "Test application pool", free_memory_test, TEST_BYTE_POOL_SIZE);
    if(ret != TX_SUCCESS)
    {
      LOG_ERROR("DAM_APP:Create byte_pool_dam fail \n");
      break;
    }
    
  }while(0);

  return ret;
}

void *data_malloc(uint32_t size)
{
  void *data = NULL;
  uint32_t status = 0;

  if (0 == size)
  {
    return NULL;
  }

  status = tx_byte_allocate(byte_pool_test, (VOID **)&data, size, TX_NO_WAIT);

  if (TX_SUCCESS != status)
  {
    LOG_ERROR("DAM_APP:Failed to allocate memory with %d", status); 
    return NULL;
  }

  if(NULL != data)
  {
    memset(data, 0, size);
  }

  return data;
}

void data_free(void *data)
{
  uint32_t status = 0;
  
  if(NULL == data)
  {
    return;
  }

  status = tx_byte_release(data);
  
  if (TX_SUCCESS != status)
  {
    LOG_ERROR("DAM_APP:Failed to release memory with %d", status); 
  }

  data = NULL;
}

/*=========================================================================
  FUNCTION:  dam_format_log_msg

===========================================================================*/
/*!
    @brief
    Format debug message for logging.

    @return
    None
*/
/*=========================================================================*/
void dam_format_log_msg
(
  char *buf_ptr,
  int buf_size,
  char *fmt,
  ...
)
{
  va_list ap;

  /*-----------------------------------------------------------------------*/

  /* validate input param */
  if( NULL == buf_ptr || buf_size <= 0)
  {
    LOG_ERROR("DAM_APP:dam_format_log_msg: Bad Param buf_ptr:[%p], buf_size:%d",buf_ptr, buf_size);
    return;
  }

  /*-----------------------------------------------------------------------*/

  va_start( ap, fmt );

  vsnprintf( buf_ptr, (size_t)buf_size, fmt, ap );

  va_end( ap );

} /* dam_format_log_msg */

UINT memscpy
(
  void   *dst,
  UINT   dst_size,
  const  void  *src,
  UINT   src_size
)
{
  UINT  copy_size = (dst_size <= src_size)? dst_size : src_size;
  memcpy(dst, src, copy_size);
  return copy_size;
}

