/* byteorder.h: wrapper for ARM Linux asm/byteorder.h
 *
 * Copyright 2008 ARM Limited. All rights reserved.
 *
 * RCS $Revision: 172106 $
 * Checkin $Date: 2011-11-02 17:04:12 +0000 (Wed, 02 Nov 2011) $
 * Revising $Author: statham $
 */

/* Define Thumb preprocessor symbol to avoid GNU inline assembly */
#ifndef __thumb__
#define __thumb__
#include_next <asm/byteorder.h>
#undef __thumb__
#else
#include_next <asm/byteorder.h>
#endif

/* end of byteorder.h */

