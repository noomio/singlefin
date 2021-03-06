#if !defined(FIN_API_DEBUG_H_INCLUDED)
#define FIN_API_DEBUG_H_INCLUDED

/** lower two bits indicate debug level
 * - 0 all
 * - 1 warning
 * - 2 serious
 * - 3 severe
 */
#define TX_DBG_LEVEL_ALL     0x00
#define TX_DBG_LEVEL_OFF     TX_DBG_LEVEL_ALL 	/* compatibility define only */
#define TX_DBG_LEVEL_WARNING 0x01 				/* bad checksums, dropped packets, ... */
#define TX_DBG_LEVEL_SERIOUS 0x02 				/* memory allocation failures, ... */
#define TX_DBG_LEVEL_SEVERE  0x03
#define TX_DBG_MASK_LEVEL    0x03


/** flag DEBUGF to enable that debug message */
#define TX_DBG_ON            0x80U
/** flag_DEBUGF to disable that debug message */
#define	TX_DBG_OFF           0x00U

#ifndef TX_DBG_NOASSERT
#define TX_ASSERT(message, assertion) do { if(!(assertion)) \
		printf( __FILE__ " " message  "\n"); \
	} while(0)
#else
#define TX_ASSERT(message, assertion)
#endif

#define TX_ERROR(message, expression, handler) do { if (!(expression)) { \
  TX_DEBUGF(TX_DBG_ON|TX_DBG_LEVEL_SEVERE,message); handler;}} while(0)


#ifdef TX_DEBUG
#define TX_DEBUGF(debug,message) do { \
							 if ( \
								   ((debug) & TX_DBG_ON) && \
								   ((debug) & TX_DBG_TYPES_ON) && \
								   ((signed short)((debug) & TX_DBG_MASK_LEVEL) >= TX_DBG_MIN_LEVEL)) { \
								 printf message; \
							   } \
							 } while(0)
#else 
#define TX_DEBUGF(debug,message)
#endif



#endif  /* FIN_API_DEBUG_H_INCLUDED */