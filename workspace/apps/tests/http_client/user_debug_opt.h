/*
 * debug_opt.h
 *
 *  Created on: 21 Nov.,2019
 *      Author: Nikolas Karakotas
 */

#ifndef TX_USER_DEBUG_OPT_H
#define TX_USER_DEBUG_OPT_H

#include <debug.h>

#undef MALLOC_DBG
#undef ITF_DBG
#define DSS_DBG 		(TX_DBG_ON)
#define HTTP_CLIENT_DBG (TX_DBG_ON)

#ifndef TX_DBG_TYPES_ON
#define TX_DBG_TYPES_ON               TX_DBG_ON
#endif

#ifndef TX_DBG_MIN_LEVEL
#define TX_DBG_MIN_LEVEL              TX_DBG_LEVEL_ALL
#endif

#define TX_DBG			(TX_DBG_ON)

#endif /* TX_USER_DEBUG_OPT_H */
