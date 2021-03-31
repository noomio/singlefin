#if !defined(FIN_API_CRITICAL_H_INCLUDED)
#define FIN_API_CRITICAL_H_INCLUDED

#define FIN_ENTER_CRITICAL()                            do { __asm("cpsid i"); } while(0)
#define FIN_EXIT_CRITICAL()                             do { __asm("cpsie i"); } while(0)

#endif  /* FIN_API_CRITICAL_H_INCLUDED */