/* limits.h: arm linux version of limits.h
 *
 * Copyright 2008 ARM Limited. All rights reserved.
 *
 * RCS $Revision: 178362 $
 * Checkin $Date: 2013-01-10 16:08:02 +0000 (Thu, 10 Jan 2013) $
 * Revising $Author: pwright $
 */

#define __ARMCLIB_VERSION 5050106

/* Include standard armcc limits */
#include <limits_armcc.h>

/* Correct value of MB_LEN_MAX for Linux/glibc */
#undef MB_LEN_MAX
#define MB_LEN_MAX 16

/* GCC-compatible definition of long long limit synonyms */
#if (defined(__GNU_LIBRARY__) && defined(__USE_GNU)) || \
    (!defined(__GNU_LIBRARY__) && !defined(__STRICT_ANSI__))
#undef LONG_LONG_MIN
#define LONG_LONG_MIN LLONG_MIN
#undef LONG_LONG_MAX
#define LONG_LONG_MAX LLONG_MAX
#endif

/* Indicate that compiler limits are defined */
#ifndef _GCC_LIMITS_H_
#define _GCC_LIMITS_H_
#endif

/* end of limits.h */

