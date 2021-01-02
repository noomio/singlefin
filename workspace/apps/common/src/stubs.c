#include "stdlib.h"
#include "txm_module.h"
#include "stdio.h"

_Noreturn void abort (void){
	tx_thread_terminate(tx_thread_identify());
	puts("abort\r\n");
	for(;;);
}


_Noreturn void exit (int arg){
	tx_thread_terminate(tx_thread_identify());
	puts("exit\r\n");
	for(;;);
}

_Noreturn void _Exit (int arg){
	tx_thread_terminate(tx_thread_identify());
	puts("_Exit\r\n");
	for(;;);
}

_Noreturn void quick_exit (int arg){
	tx_thread_terminate(tx_thread_identify());
	puts("quick_exit\r\n");
	for(;;);
}

