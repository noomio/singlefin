/*
 * debug_opt.h
 *
 *  Created on: 21 Nov.,2019
 *      Author: Nikolas Karakotas
 */

#ifndef TX_DEBUG_OPT_H_
#define TX_DEBUG_OPT_H_

#include <debug.h>


#ifndef TX_DBG_TYPES_ON
#define TX_DBG_TYPES_ON               TX_DBG_ON
#endif

#ifndef TX_DBG_MIN_LEVEL
#define TX_DBG_MIN_LEVEL              TX_DBG_LEVEL_ALL
#endif

#define TX_DBG			(TX_DBG_ON)

#endif /* CORE_DEBUG_OPT_H_ */
