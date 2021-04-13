/*
 *  fin_config.h configuration header generated by genconfig.py.
 *
 *  Git commit: None
 *  Git describe: None
 *  Git branch: None
 *
 *  Platform: THREADX
 *
 *  Architecture: arm32
 *
 *  Compiler: clang
 *
 */

#if !defined(FIN_CONFIG_H_INCLUDED)
#define FIN_CONFIG_H_INCLUDED

/*
 *  Intermediate helper defines
 */

/* DLL build detection */
/* not configured for DLL build */
#undef FIN_F_DLL_BUILD

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include <malloc.h>
#include <getopt.h>
#include <ctype.h>
#include <locale.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/time.h>
#include <time.h>

/*
 *  Platform: THREADX
 */


#include <txm_module.h>
#include <qapi_txm_base.h>
#include <qapi.h>
#include <qapi_adc_types.h>
#include <qapi_fs_types.h>
#include <qapi_fs.h>
#include <qapi_dss.h>
#include <qapi_uart.h>
#include <qapi_timer.h>
#include <qapi_device_info.h>
#include <qapi_socket.h>
#include <qapi_ns_utils.h>
#include <qapi_tlmm.h>
#include <qapi_txm_base.h>
#include <qapi_httpc.h>
#include <qapi_socket.h>
#include <qapi_netservices.h>
#include <qapi_dnsc.h>
#include <qapi_dss.h>
#include <qapi_netservices.h>
#include <qapi_timer.h>
#include <qapi_uart.h>
#include <qapi_status.h>
#include <qapi_device_info.h>

#if defined(FIN_COMPILING_SINGLEFIN) 

#include "qapi_i2c_master.h"
#include "qapi_spi_master.h"
#include <qapi_device_info.h>
#include <qapi_adc.h>
#include <qapi_gpioint.h>
#include <qurt_timetick.h>

#endif

#define FIN_USE_OS_STRING "threadx"

#include <qapi_quectel.h>

#if defined(FIN_MODULE_BG96) 
#define FIN_USE_MODULE_STRING "bg96"


#define PIN4       4
#define PIN5       5
#define PIN6       6
#define PIN7       7
#define PIN18      18
#define PIN19      19
#define PIN22      22
#define PIN23      23
#define PIN26      26
#define PIN27      27
#define PIN28      28
#define PIN40      40
#define PIN41      41
#define PIN64      64

#define Drive2mA  	QAPI_GPIO_2MA_E
#define Drive4mA  	QAPI_GPIO_4MA_E
#define Drive6mA  	QAPI_GPIO_6MA_E
#define Drive8mA  	QAPI_GPIO_8MA_E
#define Drive10mA 	QAPI_GPIO_10MA_E
#define Drive12mA 	QAPI_GPIO_12MA_E
#define Drive14mA 	QAPI_GPIO_14MA_E
#define Drive16mA 	QAPI_GPIO_16MA_E

#define PullUp     	QAPI_GPIO_PULL_UP_E
#define PullDown   	QAPI_GPIO_PULL_DOWN_E
#define PullNone   	QAPI_GPIO_NO_PULL_E
#define Keeper  	QAPI_GPIO_KEEPER_E

#define Input		QAPI_GPIO_INPUT_E
#define Output		QAPI_GPIO_OUTPUT_E
#endif

/*
 *  Architecture: arm32
 */

#define FIN_USE_ARCH_STRING "arm32"


/*
 *  Compiler: clang
 */


#define FIN_NORETURN(decl)  decl __attribute__((noreturn))

#define FIN_USE_COMPILER_STRING "clang"

#undef FIN_USE_VARIADIC_MACROS
#if defined(FIN_F_C99) || defined(FIN_F_CPP11)
#define FIN_USE_VARIADIC_MACROS
#endif


/*
 *  Fill-ins for platform, architecture, and compiler
 */

#if !defined(FIN_EXTERNAL_DECL)
#define FIN_EXTERNAL_DECL  extern
#endif
#if !defined(FIN_EXTERNAL)
#define FIN_EXTERNAL       /*empty*/
#endif
#if !defined(FIN_INTERNAL_DECL)
#if defined(FIN_SINGLE_FILE)
#define FIN_INTERNAL_DECL  static
#else
#define FIN_INTERNAL_DECL  extern
#endif
#endif
#if !defined(FIN_INTERNAL)
#if defined(FIN_SINGLE_FILE)
#define FIN_INTERNAL       static
#else
#define FIN_INTERNAL       /*empty*/
#endif
#endif
#if !defined(FIN_LOCAL_DECL)
#define FIN_LOCAL_DECL     static
#endif
#if !defined(FIN_LOCAL)
#define FIN_LOCAL          static
#endif

/*
 *  You may add overriding #define/#undef directives below for
 *  customization.  You of course cannot un-#include or un-typedef
 *  anything; these require direct changes above.
 */

/* __OVERRIDE_DEFINES__ */

#endif  /* FIN_CONFIG_H_INCLUDED */
