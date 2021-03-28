#include "fin_internal.h"

_Noreturn void abort (void){
	tx_thread_terminate(tx_thread_identify());
	for(;;);
}


_Noreturn void exit (int arg){
	tx_thread_terminate(tx_thread_identify());
	for(;;);
}

_Noreturn void _Exit (int arg){
	tx_thread_terminate(tx_thread_identify());
	for(;;);
}

_Noreturn void quick_exit (int arg){
	tx_thread_terminate(tx_thread_identify());
	for(;;);
}

