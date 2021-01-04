/*
 * debug_opt.h
 *
 *  Created on: 15 Dec.,2019
 *      Author: Nikolas Karakotas
 */

#ifndef TX_DEBUG_OPT_H_
#define TX_DEBUG_OPT_H_

#ifdef TX_USER_DEBUG_OPT
#include TX_USER_DEBUG_OPT
#else
#ifndef TX_DBG_TYPES_ON
#define TX_DBG_TYPES_ON               TX_DBG_OFF
#endif
#ifndef TX_DBG_MIN_LEVEL
#define TX_DBG_MIN_LEVEL              TX_DBG_MASK_LEVEL
#endif
#endif

#ifndef MALLOC_DBG
#define MALLOC_DBG		(TX_DBG_OFF)
#endif

#ifndef ITF_DBG
#define ITF_DBG			(TX_DBG_OFF)
#endif

#ifndef DSS_DBG
#define DSS_DBG			(TX_DBG_OFF)
#endif

#endif