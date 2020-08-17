#ifndef __CRITICAL_H__
#define __CRITICAL_H__

#define TX_ENTER_CRITICAL()                            do { __asm("cpsid i"); } while(0)
#define TX_EXIT_CRITICAL()                             do { __asm("cpsie i"); } while(0)

#endif