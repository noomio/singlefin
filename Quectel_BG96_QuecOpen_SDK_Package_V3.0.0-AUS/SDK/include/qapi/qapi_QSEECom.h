/**
 * @file qapi_QSEECom.h
 *
 * @brief HLOS QSEEComAPI Library Functions
 *
 * Contains the library functions for accessing the QSEECom driver.
 */
/*
 * Copyright (c) 2010-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __QAPI_QSEECOM_H_
#define __QAPI_QSEECOM_H_

#include <stdint.h>
#include <stdbool.h>
#include "qseecom.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup qapi_qsee
@{ */

/**
 * @brief Handle to the loaded trusted application (TA).
 *
 * This handle is returned by qapi_QSEECom_start_app().
 */
typedef struct qapi_QSEECom_handle {
    unsigned char *mem_sbuffer;     /**< Not used -- set to NULL. */
} qapi_QSEECom_handle_t;

/**
 * @brief Starts a trusted application.
 *
 * Loads and starts a trusted application. The application is verified as
 * secure by digital signature verification.
 *
 * A trusted application is built using the QTEE tool chain. The binaries
 * are split into multiple files and are saved to the file system.
 *
 * Example: \n
 *
 * The directory "/firmware/image" contains a trusted application named "my_ta". The
 * trusted application is split into "my_ta.b00", "my_ta.b01", "my_ta.b02",
 * "my_ta.b03", "my_ta.b04", "my_ta.b05", and "my_ta.mdt".
 * @code
 * qapi_QSEECom_handle_t *handle = NULL;
 * qapi_QSEECom_start_app(&handle, "/firmware/image", "my_ta", 0);
 * ...
 * qapi_QSEECom_shutdown_app(&handle);
 * @endcode
 *
 * @param[in,out] clnt_handle Handle to the loaded trusted application.
 * @param[in] path Name of the directory that contains the trusted application.
 * @param[in] fname Name of the trusted application file name without the extension.
 * @param[in] sb_size Not used -- set to 0.
 *
 * @return Zero on success, negative value on failure.
 */
int qapi_QSEECom_start_app(qapi_QSEECom_handle_t **clnt_handle,
        const char *path, const char *fname, uint32_t sb_size);

/**
 * @brief Starts a trusted application from a HLOS provided buffer.
 * 
 * Buffer(appElf_ptr) allocated should be a virtual address obtained through 
 * QuRT mem region create API, incase this API is intended to be called from 
 * kernel space.
 *
 * Loads and starts a trusted application. The application is verified as
 * secure by digital signature verification.
 *
 * A trusted application is built using the QTEE tool chain. This function 
 * doesn't support split bin contents in the Buffer(appElf_ptr).
 *
 * Buffer(appElf_ptr) should contain the exact contents of the TA mbn.
 *
 * Example: \n
 *
 * @code
 * qapi_QSEECom_handle_t *handle = NULL;
 * qapi_QSEECom_start_app_from_buffer(&handle, "ta_unique_name", &appElf_buf, appElf_len);
 * ...
 * qapi_QSEECom_shutdown_app(&handle);
 * @endcode
 *
 * @param[in,out] clnt_handle Handle to the loaded trusted application.
 * @param[in] name Unique name of the trusted application.
 * @param[in] appElf_ptr Pointer to the buffer holding the the TA.
 * @param[in] appElf_len size of the buffer holding the TA.
 *
 * @return Zero on success, negative value on failure.
 */

int qapi_QSEECom_start_app_from_buffer(
        qapi_QSEECom_handle_t **clnt_handle,
        const char *name,
        const void *appElf_ptr,
        size_t appElf_len);

/**
 * @brief Shuts down a started trusted application.
 *
 * See qapi_QSEECom_start_app() for a usage example.
 *
 * @param[in] handle Handle to the loaded trusted application.
 *
 * @return Zero on success, negative on failure.
 */
int qapi_QSEECom_shutdown_app(qapi_QSEECom_handle_t **handle);

/**
 * @brief Dumps QSEE logs to a file on target EFS.
 *
 * Upon calling this API, entire contents of the QSEE logs are dumped
 * to a file under EFS.
 *
 * "/core/qsee_log_<count>.txt" is the file name convention used to 
 * save the logs. For example, "/core/qsee_log_001.txt"
 *
 * @return Zero on success, negative on failure.
 */
int qapi_get_qsee_log(void);

/** @} */ /* end_addtogroup qapi_qsee */

#ifdef __cplusplus
}
#endif

#endif
