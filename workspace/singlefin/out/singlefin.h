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
#define FIN_GIT_COMMIT                    "019f17af9b12a6c6b70adc45104476d1d4fdda51"
#define FIN_GIT_DESCRIBE                  "019f17a-dirty"
#define FIN_GIT_BRANCH                    "single-header"


