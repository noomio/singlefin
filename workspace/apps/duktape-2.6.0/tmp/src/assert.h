#ifndef QUEC_ASSERT_H_
#define QUEC_ASSERT_H_

#include "stdio.h"

#ifndef DBG_NOASSERT
#define QUEC_ASSERT(message, assertion) do { if(!(assertion)) \
		printf( message __FILE__ ); } while(0)
#else
#define QUEC_ASSERT(message, assertion)
#endif

#define QUEC_ERROR(message, expression, handler) do { if (!(expression)) { \
  printf(message); handler;}} while(0)

#endif