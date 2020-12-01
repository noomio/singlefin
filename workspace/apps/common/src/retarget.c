/*
 * retarget.c
 *
 *  Created on: 24 Nov.,2019
 *      Author: Nikolas Karakotas
 */

#include <stdarg.h>
#include <stdio.h>
#include "qapi_uart.h"
#include "txm_module.h"

#define DEBUG_PORT	QAPI_UART_PORT_002_E

TX_SEMAPHORE *out_tx_mutex, *out_tx_done_sem, *in_tx_sem;
TX_BLOCK_POOL *pool;

extern int main(void);
extern qapi_Status_t malloc_byte_pool_init(void);
static void init_debug(void);

extern int vsnprintf_(char* buffer, size_t count, const char* format, va_list va);

 __attribute__ ((noreturn)) int task_main_entry(void){

	init_debug();
	malloc_byte_pool_init();
	main();
	for(;;);
}

static void uart_rx_cb(uint32_t num_bytes, void *cb_data){
	if(num_bytes > 0)
		tx_semaphore_ceiling_put(in_tx_sem,1);
};


static void uart_tx_cb(uint32_t num_bytes, void *cb_data){
	tx_semaphore_ceiling_put(out_tx_done_sem,1);
};

static qapi_UART_Handle_t handle = NULL;

#if 0
/*
	QUEUE
*/
typedef enum tracef_data_type{
	STR_CONST,
	STR_ALLOC
} tracef_data_type_t;

typedef struct tracef_msg{
	unsigned char *str;
	tracef_data_type_t type;
	uint16_t len;
} tracef_msg_t;

#define TRACEF_MSG_SIZE sizeof(tracef_msg_t) // 1 word, 32 bits
#ifndef TX_TRACEF_MSG_NUM
#define TX_TRACEF_MSG_NUM 128
#elif TX_TRACEF_MSG_NUM < 32
#error "TX_TRACEF_MSG_NUM < 32"
#endif
TX_QUEUE *tracef_msg_queue;
unsigned char tracef_msg_mem[TRACEF_MSG_SIZE*TX_TRACEF_MSG_NUM];


/*
*	TRACEF THREAD
*/
TX_THREAD* tracef_thread_handle; 
void *tracef_thread_stack;
#define TRACEF_THREAD_STACK_SIZE			1*1024
TX_BYTE_POOL *tracef_thread_byte_pool;
#define TRACEF_THREAD_BYTE_POOL_SIZE		4*1024
char tracef_thread_mem[TRACEF_THREAD_BYTE_POOL_SIZE];

static void tracef_thread(ULONG param);
#endif


#ifndef TX_PRINTF_LEN
#define TX_PRINTF_LEN 512
#elif TX_PRINTF_LEN < 128
#error "TX_PRINTF_LEN < 128"
#endif


char stdin_buf[128]; // Buffer size. Must be >= 4 and a multiple of 4.


static void init_debug(void){

	qapi_UART_Open_Config_t uart_cfg;

	memset (&uart_cfg, 0, sizeof (uart_cfg));
	uart_cfg.baud_Rate			= 115200;
	uart_cfg.enable_Flow_Ctrl	= QAPI_FCTL_OFF_E;
	uart_cfg.bits_Per_Char		= QAPI_UART_8_BITS_PER_CHAR_E;
	uart_cfg.enable_Loopback 	= 0;
	uart_cfg.num_Stop_Bits		= QAPI_UART_1_0_STOP_BITS_E;
	uart_cfg.parity_Mode 		= QAPI_UART_NO_PARITY_E;
	uart_cfg.rx_CB_ISR			= (qapi_UART_Callback_Fn_t)&uart_rx_cb;
	uart_cfg.tx_CB_ISR			= (qapi_UART_Callback_Fn_t)&uart_tx_cb;

	if(qapi_UART_Open(&handle, DEBUG_PORT, &uart_cfg) == QAPI_OK && 
		qapi_UART_Power_On(handle) == QAPI_OK){

		qapi_UART_Receive (handle, stdin_buf, sizeof(stdin_buf), NULL); // queue as per doc

		txm_module_object_allocate(&out_tx_mutex, sizeof(TX_MUTEX));
		tx_mutex_create(out_tx_mutex,"stdout_sem", TX_NO_INHERIT);

		txm_module_object_allocate(&out_tx_done_sem, sizeof(TX_SEMAPHORE));
		tx_semaphore_create(out_tx_done_sem,"out_tx_done_sem", 1);

		txm_module_object_allocate(&in_tx_sem, sizeof(TX_SEMAPHORE));
		tx_semaphore_create(in_tx_sem,"stdin_sem", 0);
#if 0
		txm_module_object_allocate(&tracef_msg_queue, sizeof(TX_QUEUE));
		tx_queue_create(tracef_msg_queue, "tracef_msg_mem",TRACEF_MSG_SIZE, tracef_msg_mem, sizeof(tracef_msg_mem));

		txm_module_object_allocate(&tracef_thread_byte_pool, sizeof(TX_BYTE_POOL));
		tx_byte_pool_create(tracef_thread_byte_pool, "tracef_byte_pool", tracef_thread_mem, TRACEF_THREAD_BYTE_POOL_SIZE);


	 	tx_byte_allocate(tracef_thread_byte_pool, (VOID **) &tracef_thread_stack, TRACEF_THREAD_STACK_SIZE, TX_NO_WAIT);		
		txm_module_object_allocate(&tracef_thread_handle, sizeof(TX_THREAD));
		tx_thread_create(tracef_thread_handle,
							"tracef_thread",
						   	tracef_thread,
						   	(ULONG)0,
						   	tracef_thread_stack,
						   	TRACEF_THREAD_STACK_SIZE,
						   	250,
						   	0,
						   	TX_NO_TIME_SLICE,
						   	TX_AUTO_START
		);
#endif

	}
 
}

#if 0
/*
 This method only works when printf is called from 
 kernel copy to user space using *_Pass_Pool_Ptr
*/
static void tracef_thread(ULONG param){


	for(;;){

		tracef_msg_t msg;
	 	
	 	msg.str = NULL;

		if(tx_queue_receive(tracef_msg_queue, &msg, TX_WAIT_FOREVER) == TX_SUCCESS){	
 			
 			if(msg.str){
 			
 				tx_mutex_get(out_tx_mutex,TX_WAIT_FOREVER);
	 			if(qapi_UART_Transmit(handle, msg.str, msg.len, NULL) == TX_SUCCESS)
	 				tx_semaphore_get(out_tx_done_sem,TX_WAIT_FOREVER);
	 			
	 			if(msg.type == STR_ALLOC)
	 				free(msg.str);

	 			tx_mutex_put(out_tx_mutex);

 			}

 		}

	}

}
#endif


int putchar(int character){
	
	if(!handle)
		return 0;

	tx_mutex_get(out_tx_mutex,TX_WAIT_FOREVER);

	if(qapi_UART_Transmit(handle, &character, 1, NULL) == TX_SUCCESS)	// puts removes the new line!
		tx_semaphore_get(out_tx_done_sem,TX_WAIT_FOREVER);
	
	tx_mutex_put(out_tx_mutex);

	return character;
}

int _putchar(int character){
	return putchar(character);
}

int __wrap_puts(const char *s){

	
	if(!handle)
		return 0;

	if(s){

		tx_mutex_get(out_tx_mutex,TX_WAIT_FOREVER);

		if(qapi_UART_Transmit(handle, s, strlen(s), NULL) == TX_SUCCESS)
			tx_semaphore_get(out_tx_done_sem,TX_WAIT_FOREVER);

		tx_mutex_put(out_tx_mutex);

	}

	return 0;
}


/*

*/
int __wrap_printf(const char *format, ...){

	va_list ap;
	int len;
	static char stdout_buf[TX_PRINTF_LEN];

	if(!handle)
		return 0;

		
	va_start( ap, format );
	len = vsnprintf_( stdout_buf, (size_t)TX_PRINTF_LEN, format, ap );
	va_end( ap );
	
	tx_mutex_get(out_tx_mutex,TX_WAIT_FOREVER);
	
	if(qapi_UART_Transmit(handle, stdout_buf, len, NULL) == TX_SUCCESS)
		tx_semaphore_get(out_tx_done_sem,TX_WAIT_FOREVER);

	tx_mutex_put(out_tx_mutex);


	return len;

}

/* STDIN */
char * __wrap_gets ( char * str ){
	return NULL;
}

int __wrap_getchar(void){

	stdin_buf[0] = -1;

	if(!handle)
		return 0;

	if(qapi_UART_Receive (handle, stdin_buf, sizeof(stdin_buf), (void*)1) == QAPI_OK){
 		tx_semaphore_get(in_tx_sem,TX_WAIT_FOREVER);
	}

	return (int)stdin_buf[0];
}

int __wrap_fprintf(FILE *stream, const char *format, ...){
	puts("error\r\n");
	return 0;
}


/*

	Tests

	for(uint32_t i = 0; i < 0xFFFF; i++){
		puts("\r\nputs:\r\n");
		puts("The quick brown fox jumps over the lazy dog \
			The quick brown fox jumps over the lazy dog \
			The quick brown fox jumps over the lazy dog \
			The quick brown fox jumps over the lazy dog \
			The quick brown fox jumps over the lazy dog \
			The quick brown fox jumps over the lazy dog");
	}


	for(uint32_t i = 0; i < 0xFFFF; i++){
		puts("\r\nprintf:\r\n");
		printf("%s","The quick brown fox jumps over the lazy dog \
			The quick brown fox jumps over the lazy dog \
			The quick brown fox jumps over the lazy dog \
			The quick brown fox jumps over the lazy dog \
			The quick brown fox jumps over the lazy dog");
	}

*/