
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdlib.h>
#include <string.h>
#if !defined(_MSC_VER) && !defined(__BORLANDC__)
# include <unistd.h>
#endif

#include <sys/types.h>
#ifndef _WIN32
# include <sys/stat.h>
# include <sys/time.h>
#endif
#ifdef __linux__
# define _LINUX_SOURCE
#endif
#ifdef HAVE_SYS_RANDOM_H
# include <sys/random.h>
#endif
#ifdef __linux__
# ifdef HAVE_GETRANDOM
#  define HAVE_LINUX_COMPATIBLE_GETRANDOM
# else
#  include <sys/syscall.h>
#  if defined(SYS_getrandom) && defined(__NR_getrandom)
#   define getrandom(B, S, F) syscall(SYS_getrandom, (B), (int) (S), (F))
#   define HAVE_LINUX_COMPATIBLE_GETRANDOM
#  endif
# endif
#elif defined(__FreeBSD__)
# include <sys/param.h>
# if defined(__FreeBSD_version) && __FreeBSD_version >= 1200000
#  define HAVE_LINUX_COMPATIBLE_GETRANDOM
# endif
#endif
#if !defined(NO_BLOCKING_RANDOM_POLL) && defined(__linux__)
# define BLOCK_ON_DEV_RANDOM
#endif
#ifdef BLOCK_ON_DEV_RANDOM
# include <poll.h>
#endif
#ifdef HAVE_RDRAND
# pragma GCC target("rdrnd")
# include <immintrin.h>
#endif

#include "core.h"
#include "crypto_core_hchacha20.h"
#include "crypto_stream_chacha20.h"
#include "private/common.h"
#include "randombytes.h"
#include "randombytes_internal_random.h"
#include "runtime.h"
#include "utils.h"

#ifdef _WIN32
# include <windows.h>
# include <sys/timeb.h>
# define RtlGenRandom SystemFunction036
# if defined(__cplusplus)
extern "C"
# endif
BOOLEAN NTAPI RtlGenRandom(PVOID RandomBuffer, ULONG RandomBufferLength);
# pragma comment(lib, "advapi32.lib")
# ifdef __BORLANDC__
#  define _ftime ftime
#  define _timeb timeb
# endif
#endif

#define INTERNAL_RANDOM_BLOCK_SIZE crypto_core_hchacha20_OUTPUTBYTES

#if defined(__OpenBSD__) || defined(__CloudABI__) || defined(__wasi__)
# define HAVE_SAFE_ARC4RANDOM 1
#endif
#if defined(__CloudABI__) || defined(__wasm__)
# define NONEXISTENT_DEV_RANDOM 1
#endif

#ifndef SSIZE_MAX
# define SSIZE_MAX (SIZE_MAX / 2 - 1)
#endif
#ifndef S_ISNAM
# ifdef __COMPCERT__
#  define S_ISNAM(X) 1
# else
#  define S_ISNAM(X) 0
# endif
#endif

#ifndef TLS
# ifdef _WIN32
#  define TLS __declspec(thread)
# else
#  define TLS
# endif
#endif

typedef struct InternalRandomGlobal_ {
    int           initialized;
    int           random_data_source_fd;
    int           getentropy_available;
    int           getrandom_available;
    int           rdrand_available;
#ifdef HAVE_GETPID
    pid_t         pid;
#endif
} InternalRandomGlobal;

typedef struct InternalRandom_ {
    int           initialized;
    size_t        rnd32_outleft;
    unsigned char key[crypto_stream_chacha20_KEYBYTES];
    unsigned char rnd32[16U * INTERNAL_RANDOM_BLOCK_SIZE];
    uint64_t      nonce;
} InternalRandom;

static InternalRandomGlobal global = {
    SODIUM_C99(.initialized =) 0,
    SODIUM_C99(.random_data_source_fd =) -1
};

static TLS InternalRandom stream = {
    SODIUM_C99(.initialized =) 0,
    SODIUM_C99(.rnd32_outleft =) (size_t) 0U
};


/*
 * Get a high-resolution timestamp, as a uint64_t value
 */

static uint64_t
sodium_hrtime(void)
{
    struct timeval tv;

    if (gettimeofday(&tv, NULL) != 0) {
        sodium_misuse(); /* LCOV_EXCL_LINE */
    }
    return ((uint64_t) tv.tv_sec) * 1000000U + (uint64_t) tv.tv_usec;
}


/*
 * XOR the key with another same-length secret
 */

static inline void
randombytes_internal_random_xorkey(const unsigned char * const mix)
{
    unsigned char *key = stream.key;
    size_t         i;

    for (i = (size_t) 0U; i < sizeof stream.key; i++) {
        key[i] ^= mix[i];
    }
}

/*
 * Put `size` random bytes into `buf` and overwrite the key
 */

static void
randombytes_custom_random_buf(void * const buf, const size_t size)
{
    size_t i;
    int    ret;


}

/*
 * Pop a 32-bit value from the random pool
 *
 * Overwrite the key after the pool gets refilled.
 */

static uint32_t
randombytes_custom_random(void)
{
    uint32_t val;
    int      ret;


    return val;
}

static const char *
randombytes_custom_implementation_name(void)
{
    return "custom";
}

struct randombytes_implementation randombytes_custom_implementation = {
    SODIUM_C99(.implementation_name =) randombytes_custom_implementation_name,
    SODIUM_C99(.random =) randombytes_custom_random,
    SODIUM_C99(.stir =) NULL,
    SODIUM_C99(.uniform =) NULL,
    SODIUM_C99(.buf =) randombytes_custom_random_buf,
    SODIUM_C99(.close =) NULL
};
