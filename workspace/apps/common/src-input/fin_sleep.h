#if !defined(FIN_API_SLEEP_H_INCLUDED)
#define FIN_API_SLEEP_H_INCLUDED

#define sleep(T) qapi_Timer_Sleep(T, QAPI_TIMER_UNIT_MSEC, true)

#endif  /* FIN_API_SLEEP_H_INCLUDED */