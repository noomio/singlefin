#if !defined(FIN_API_DEBUG_OPT_H_INCLUDED)
#define FIN_API_DEBUG_OPT_H_INCLUDED

#ifdef TX_USER_DEBUG_OPT
//#include TX_USER_DEBUG_OPT add sneppet
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

#endif  /* FIN_API_DEBUG_OPT_H_INCLUDED */
