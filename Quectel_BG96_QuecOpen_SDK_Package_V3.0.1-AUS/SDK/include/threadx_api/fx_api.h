#ifndef FX_API_H
#define FX_API_H
/**
  @file fx_api.h
  @brief  Prototypes of functions used (by TXM module manager) for file operations

EXTERNAL FUNCTIONS
   qurt_efs_read

INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.

Copyright (c) 2013-2015  by Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.


=============================================================================*/

#ifndef _UINT32_DEFINED
typedef  unsigned long int  uint32;      /* Unsigned 32 bit value */
#define _UINT32_DEFINED
#endif

#ifndef _INT32_DEFINED
typedef  signed long int    int32;       /* Signed 32 bit value */
#define _INT32_DEFINED
#endif

extern int32 qurt_efs_read(int32 fd, void *buf, uint32 size, uint32 *bytesRead);

#define FX_FILE  int						/*File data type*/
#define FX_SUCCESS  ((unsigned int) 0x00)

/* Actual function mappings */
#define fx_file_read 	qurt_efs_read
#define fx_file_close 	efs_close

#endif /*FX_API_H*/

