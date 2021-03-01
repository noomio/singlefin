
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

/* platform dependent. provide implementations */
extern void hwrandombytes(uint8_t *buf, uint64_t size );
extern uint64_t cputimeusecs(void);

static uint64_t
sodium_hrtime(void)
{
    uint64_t ticks;

    if ((ticks = cputimeusecs()) == 0) {
        sodium_misuse(); /* LCOV_EXCL_LINE */
    }
    return ticks;
}

static int
_randombytes_getentropy(void * const buf, const size_t size)
{
    assert(size <= 256U);
    hwrandombytes(buf, size);
    return 0;
}

static int
randombytes_getentropy(void * const buf_, size_t size)
{
    unsigned char *buf = (unsigned char *) buf_;
    size_t         chunk_size = 256U;

    do {
        if (size < chunk_size) {
            chunk_size = size;
            assert(chunk_size > (size_t) 0U);
        }
        if (_randombytes_getentropy(buf, chunk_size) != 0) {
            return -1; /* LCOV_EXCL_LINE */
        }
        size -= chunk_size;
        buf += chunk_size;
    } while (size > (size_t) 0U);

    return 0;
}

static void
randombytes_custom_random_init(void)
{
    const int errno_save = errno;

    global.rdrand_available = 0;
    global.getentropy_available = 0;
    global.getrandom_available = 0;

    unsigned char fodder[16];

    if (randombytes_getentropy(fodder, sizeof fodder) == 0) {
        global.getentropy_available = 1;
        //errno = errno_save;  makes it fails??
        return;
    }
  

    //errno = errno_save;
    return;

}

/*
 * (Re)seed the generator using the entropy source
 */

static void
randombytes_custom_random_stir(void)
{

    stream.nonce = sodium_hrtime();
    assert(stream.nonce != (uint64_t) 0U);
    memset(stream.rnd32, 0, sizeof stream.rnd32);
    stream.rnd32_outleft = (size_t) 0U;
    if (global.initialized == 0) {
        randombytes_custom_random_init();
        global.initialized = 1;
    }


    if (global.getentropy_available != 0) {
         if (randombytes_getentropy(stream.key, sizeof stream.key) != 0) {
             sodium_misuse(); /* LCOV_EXCL_LINE */
         }
    }

    stream.initialized = 1;
}

/*
 * Reseed the generator if it hasn't been initialized yet
 */

static void
randombytes_custom_random_stir_if_needed(void)
{
    if (stream.initialized == 0) {
        randombytes_custom_random_stir();
    }
}


static int
randombytes_custom_random_close(void)
{
    int ret = -1;

    if (global.getentropy_available != 0) {
        ret = 0;
    }

    sodium_memzero(&stream, sizeof stream);

    return ret;
}
/*
 * RDRAND is only used to mitigate prediction if a key is compromised
 */

static void
randombytes_custom_random_xorhwrand(void)
{
/* LCOV_EXCL_START */
#ifdef HAVE_RDRAND
    unsigned int r;

    if (global.rdrand_available == 0) {
        return;
    }
    (void) _rdrand32_step(&r);
    * (uint32_t *) (void *)
        &stream.key[crypto_stream_chacha20_KEYBYTES - 4] ^= (uint32_t) r;
#endif
/* LCOV_EXCL_STOP */
}

/*
 * XOR the key with another same-length secret
 */

static inline void
randombytes_custom_random_xorkey(const unsigned char * const mix)
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

    randombytes_custom_random_stir_if_needed();
    COMPILER_ASSERT(sizeof stream.nonce == crypto_stream_chacha20_NONCEBYTES);
#if defined(ULLONG_MAX) && defined(SIZE_MAX)
# if SIZE_MAX > ULLONG_MAX
    /* coverity[result_independent_of_operands] */
    assert(size <= ULLONG_MAX);
# endif
#endif
    ret = crypto_stream_chacha20((unsigned char *) buf, (unsigned long long) size,
                                 (unsigned char *) &stream.nonce, stream.key);
    assert(ret == 0);
    for (i = 0U; i < sizeof size; i++) {
        stream.key[i] ^= ((const unsigned char *) (const void *) &size)[i];
    }
    randombytes_custom_random_xorhwrand();
    stream.nonce++;
    crypto_stream_chacha20_xor(stream.key, stream.key, sizeof stream.key,
                               (unsigned char *) &stream.nonce, stream.key);
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

    COMPILER_ASSERT(sizeof stream.rnd32 >= (sizeof stream.key) + (sizeof val));
    COMPILER_ASSERT(((sizeof stream.rnd32) - (sizeof stream.key))
                    % sizeof val == (size_t) 0U);
    if (stream.rnd32_outleft <= (size_t) 0U) {
        randombytes_custom_random_stir_if_needed();
        COMPILER_ASSERT(sizeof stream.nonce == crypto_stream_chacha20_NONCEBYTES);
        ret = crypto_stream_chacha20((unsigned char *) stream.rnd32,
                                     (unsigned long long) sizeof stream.rnd32,
                                     (unsigned char *) &stream.nonce,
                                     stream.key);
        assert(ret == 0);
        stream.rnd32_outleft = (sizeof stream.rnd32) - (sizeof stream.key);
        randombytes_custom_random_xorhwrand();
        randombytes_custom_random_xorkey(&stream.rnd32[stream.rnd32_outleft]);
        memset(&stream.rnd32[stream.rnd32_outleft], 0, sizeof stream.key);
        stream.nonce++;
    }
    stream.rnd32_outleft -= sizeof val;
    memcpy(&val, &stream.rnd32[stream.rnd32_outleft], sizeof val);
    memset(&stream.rnd32[stream.rnd32_outleft], 0, sizeof val);

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
    SODIUM_C99(.stir =) randombytes_custom_random_stir,
    SODIUM_C99(.uniform =) NULL,
    SODIUM_C99(.buf =) randombytes_custom_random_buf,
    SODIUM_C99(.close =) randombytes_custom_random_close
};
