/*
 *  Top-level include file to be used for all (internal) source files.
 *
 *  Source files should not include individual header files, as they
 *  have not been designed to be individually included.
 */

#if !defined(FIN_INTERNAL_H_INCLUDED)
#define FIN_INTERNAL_H_INCLUDED

/*
 *  The 'singlefin.h' header provides the public API, but also handles all
 *  compiler and platform specific feature detection, SingleFin feature
 *  resolution, inclusion of system headers, etc.  These have been merged
 *  because the public API is also dependent on e.g. detecting appropriate
 *  C types which is quite platform/compiler specific especially for a non-C99
 *  build.  The public API is also dependent on the resolved feature set.
 *
 *  Some actions taken by the merged header (such as including system headers)
 *  are not appropriate for building a user application.  The define
 *  FIN_COMPILING_SINGLEFIN allows the merged header to skip/include some
 *  sections depending on what is being built.
 */

#define FIN_COMPILING_SINGLEFIN
#include "singlefin.h"




#include "fin_debug.h"
#include "fin_debug_opt.h"
#include "fin_cli.h"
#include "fin_dss.h"
#include "fin_gpio.h"
#include "fin_http_client.h"
#include "fin_i2c.h"
#include "fin_spi.h"
#include "fin_adc.h"
#include "fin_uart.h"


#endif  /* FIN_INTERNAL_H_INCLUDED */
