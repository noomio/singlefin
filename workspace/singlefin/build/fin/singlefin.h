/*
 *  Duktape public API for Duktape @DUK_VERSION_FORMATTED@.
 *
 *  See the API reference for documentation on call semantics.  The exposed,
 *  supported API is between the "BEGIN PUBLIC API" and "END PUBLIC API"
 *  comments.  Other parts of the header are Duktape internal and related to
 *  e.g. platform/compiler/feature detection.
 *
 *  Git commit 6e7fc32250201874e3f51cfe88cf256cd60638be (6e7fc32-dirty).
 *  Git branch single-header.
 *
 *  See Duktape AUTHORS.rst and LICENSE.txt for copyright and
 *  licensing information.
 */



#if !defined(SINGLEFIN_H_INCLUDED)
#define SINGLEFIN_H_INCLUDED

#define FIN_SINGLE_FILE


/*
 *  Version and Git commit identification
 */

/* Duktape version, (major * 10000) + (minor * 100) + patch.  Allows C code
 * to #if (DUK_VERSION >= NNN) against Duktape API version.  The same value
 * is also available to ECMAScript code in Duktape.version.  Unofficial
 * development snapshots have 99 for patch level (e.g. 0.10.99 would be a
 * development version after 0.10.0 but before the next official release).
 */
#define FIN_VERSION                       10000L

/* Git commit, describe, and branch for Duktape build.  Useful for
 * non-official snapshot builds so that application code can easily log
 * which Duktape snapshot was used.  Not available in the ECMAScript
 * environment.
 */
#define FIN_GIT_COMMIT                    "6e7fc32250201874e3f51cfe88cf256cd60638be"
#define FIN_GIT_DESCRIBE                  "6e7fc32-dirty"
#define FIN_GIT_BRANCH                    "single-header"



/* External fin_config.h provides platform/compiler/OS dependent
 * typedefs and macros, and FIN_USE_xxx config options so that
 * the rest of SingleFin doesn't need to do any feature detection.
 * FIN_VERSION is defined before including so that configuration
 * snippets can react to it.
 */
#include "fin_config.h"

/*
 *  Avoid C++ name mangling
 */

#if defined(__cplusplus)
extern "C" {
#endif



/*
 *  C++ name mangling
 */

#if defined(__cplusplus)
/* end 'extern "C"' wrapper */
}
#endif

/*
 *  END PUBLIC API
 */

#endif  /* SINGLEFIN_H_INCLUDED */