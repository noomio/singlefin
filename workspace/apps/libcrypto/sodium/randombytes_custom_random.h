
#ifndef randombytes_custom_random_H
#define randombytes_custom_random_H

#include "export.h"
#include "randombytes.h"

#ifdef __cplusplus
extern "C" {
#endif

/* 
NOTE:
The randombytes_set_implementation() function defines the set of functions required by the randombytes_* interface.
This function should only be called once, before sodium_init().
https://libsodium.gitbook.io/doc/advanced/custom_rng
*/

SODIUM_EXPORT
extern struct randombytes_implementation randombytes_custom_implementation;

/* Backwards compatibility with libsodium < 1.0.18 */
#define randombytes_salsa20_implementation randombytes_custom_implementation

#ifdef __cplusplus
}
#endif

#endif
