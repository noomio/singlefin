#ifndef _QSEECOM_H_
#define _QSEECOM_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "qapi_txm_base.h"

#define MAX_APP_NAME_SIZE 64

#define GPFILE_SERVICE  0x7000 /** GP File Service ID */

#define ENABLE_CLK      1
#define DISABLE_CLK     0

typedef enum qseecom_ioctl_qapi
{
  QSEECOM_IOCTL_LOAD_APP_REQ = TXM_QAPI_QSEECOM_BASE,
  QSEECOM_IOCTL_REGISTER_LISTENER_REQ,
  QSEECOM_IOCTL_UNREGISTER_LISTENER_REQ,
  QSEECOM_IOCTL_RECEIVE_REQ,
  QSEECOM_IOCTL_SEND_RESP_REQ,
  QSEECOM_IOCTL_GET_QSEOS_VERSION_REQ,
  QSEECOM_IOCTL_GET_QSEECOM_FD,
  QSEECOM_IOCTL_FREE_QSEECOM_FD,
  QSEECOM_IOCTL_APP_LOADED_QUERY_REQ,
  QSEECOM_IOCTL_UNLOAD_APP_REQ,

  /* Mem object ioctl start */
  QSEECOM_IOCTL_MEM_ALLOC,
  QSEECOM_IOCTL_MEM_FREE,
  QSEECOM_IOCTL_FS_READ_TO_KERNEL_BUF,
  QSEECOM_IOCTL_ATTACH_MEM_TO_OBJ,
  QSEECOM_IOCTL_MAX = QSEECOM_IOCTL_LOAD_APP_REQ + TXM_QAPI_QSEECOM_NUM_IDS,
} qseecom_ioctl_qapi_t;

/*
 * struct qseecom_register_listener_req -
 *      for register listener ioctl request
 * @listener_id - service id (shared between userspace and QSE)
 * @ifd_data_fd - ion handle
 * @virt_sb_base - shared buffer base in user space
 * @sb_size - shared buffer size
 */
typedef struct qseecom_register_listener_req {
    UINT listener_id; /* in */
    INT ifd_data_fd; /* in */
    UINT sb_size; /* in */
} qseecom_register_listener_req_t;

/*
 * struct qseecom_load_img_data - for sending image length information and
 * ion file descriptor to the qseecom driver. ion file descriptor is used
 * for retrieving the ion file handle and in turn the physical address of
 * the image location.
 * @mdt_len - Length of the .mdt file in bytes.
 * @img_len - Length of the .mdt + .b00 +..+.bxx images files in bytes
 * @ion_fd - Ion file descriptor used when allocating memory.
 * @img_name - Name of the image.
 * @app_arch - Architecture of the image, i.e. 32bit or 64bit app
 */
struct qseecom_load_img_req {
  uint32_t mdt_len; /* in */
  uint32_t img_len; /* in */
  int32_t  ifd_data_fd; /* in */
  char     img_name[MAX_APP_NAME_SIZE]; /* in */
  uint32_t app_arch; /* in */
  uint32_t app_id; /* out*/
};

/*
 * struct qseecom_qseos_version_req - get qseos version
 * @qseos_version - version number
 */
typedef struct qseecom_qseos_version_req {
    UINT qseos_version; /* in */
} qseecom_qseos_version_req_t;

/*
 * struct qseecom_qseos_app_load_query - verify if app is loaded in qsee
 * @app_name[MAX_APP_NAME_SIZE]-  name of the app.
 * @app_id - app id.
 */
struct qseecom_qseos_app_load_query {
  char app_name[MAX_APP_NAME_SIZE]; /* in */
  uint32_t app_id; /* out */
  uint32_t app_arch;
};

typedef INT mem_user_handle_t;

struct mem_handle_data {
  mem_user_handle_t handle;
};

typedef enum mem_pool {
  DEFAULT_PHYSPOOL,
  QSEECOM_POOL
} mem_pool_t;


struct mem_allocation_data {
  size_t len;
  size_t align;
  mem_pool_t mem_pool;
};

struct file {
    void *private_data;
};

struct kernel_userspace_map {
  unsigned char *uaddr;
  int32_t mem_info;
  size_t len;
};

struct fs_info {
  int32_t mem_info;
  int32_t fd;
  uint32_t offset;  // Read file and write into buffer with this offset
  size_t size;
  size_t bytesReadWrite;
};

struct ta_ubuf_info {
  const void *appElf_ptr;
  size_t appElf_len;
};

#ifdef  QAPI_TXM_MODULE     // USER_MODE_DEFS

#define qseecom_handle_req(file, id, args)   (_txm_module_system_call12)(id, (ULONG) file, (ULONG) args, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0)

#elif  defined QAPI_TXM_SOURCE_CODE      // KERNEL_MODE_DEFS

#define qseecom_handle_req(file, id, args)   qseecom_ioctl_handler(id, (ULONG) file, (ULONG) args, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0)

UINT qseecom_ioctl_handler(UINT id, UINT a1, UINT a2, UINT a3, UINT a4, UINT a5, UINT a6, UINT a7, UINT a8, UINT a9, UINT a10, UINT a11, UINT a12);

int qseecom_req_clk(uint32_t enable);

#endif

#ifdef __cplusplus
}
#endif

#endif // _QSEECOM_H_
