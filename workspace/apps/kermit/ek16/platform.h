/* Unix platform.h for EK */

#include <stdio.h>
#include <sys/stat.h>

#ifndef IBUFLEN
#define IBUFLEN  512			/* File input buffer size */
#endif /* IBUFLEN */

#ifndef OBUFLEN
#define OBUFLEN  1024                   /* File output buffer size */
#endif /* OBUFLEN */

#define F_CTRLC
