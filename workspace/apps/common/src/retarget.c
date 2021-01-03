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
#include <locale.h>
#include "cli.h"

#define DEBUG_PORT	QAPI_UART_PORT_002_E

FILE *const stdin;
FILE *const stdout;
FILE *const stderr;

extern int main(void);
extern qapi_Status_t malloc_byte_pool_init(void);
extern TX_BYTE_POOL *malloc_get_pool(void);

static void init_debug(void);

extern int vsnprintf_(char* buffer, size_t count, const char* format, va_list va);

 __attribute__ ((noreturn)) int task_main_entry(void){
	setlocale(LC_ALL, "C");	
	init_debug();
	malloc_byte_pool_init();
	//TX_THREAD *t = tx_thread_identify();
	//UINT old_priority;
	//tx_thread_priority_change(t,64, &old_priority);
	main();
	for(;;);
}


static qapi_UART_Handle_t handle = NULL;

#if 0
/*
	QUEUE
*/

TX_BLOCK_POOL *pool;

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

TX_MUTEX *out_tx_mutex;
TX_SEMAPHORE *out_tx_done_sem;

TX_SEMAPHORE *in_rx_sem1,*in_rx_sem2;

#ifndef TX_PRINTF_LEN
#define TX_PRINTF_LEN 512
#elif TX_PRINTF_LEN < 128
#error "TX_PRINTF_LEN < 128"
#endif

#define RECV_QUEUE_MEM_SIZE 512

TX_QUEUE *recv_queue;
uint32_t recv_queue_mem[RECV_QUEUE_MEM_SIZE];


#define RX_SIZE RECV_QUEUE_MEM_SIZE/2
volatile uint8_t bytes1[RX_SIZE];
volatile uint8_t bytes2[RX_SIZE];
volatile uint32_t bytes1_cnt;
volatile uint32_t bytes2_cnt;

static void rx_queue_task(ULONG param);
TX_THREAD* rx_queue_thread_handle; 
TX_BYTE_POOL *rx_queue_thread_byte_pool;
void *rx_queue_thread_stack;
#define RX_QUEUE_THREAD_STACK_SIZE			1024
#define RX_QUEUE_THREAD_BYTE_POOL_SIZE		2*RX_QUEUE_THREAD_STACK_SIZE
char rx_queue_thread_mem[RX_QUEUE_THREAD_BYTE_POOL_SIZE];

TX_EVENT_FLAGS_GROUP	*evnt_rx;
const int byte_num1 = 1;
const int byte_num2 = 2;

static void uart_rx_cb(uint32_t num_bytes, void *cb_data){

	int num = *(int*)cb_data;

	if(num == byte_num1){
		__atomic_store(&bytes1_cnt, &num_bytes, __ATOMIC_SEQ_CST);
	}
	else if(num == byte_num2){
		__atomic_store(&bytes2_cnt, &num_bytes, __ATOMIC_SEQ_CST);
	}


	tx_event_flags_set(evnt_rx,num, TX_OR);
	//tx_semaphore_ceiling_put(sem_rx,1); // signal to  queue 


};



static void uart_tx_cb(uint32_t num_bytes1, void *cb_data){
	tx_semaphore_ceiling_put(out_tx_done_sem,1);
};


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

		txm_module_object_allocate(&recv_queue, sizeof(TX_QUEUE));
		tx_queue_create(recv_queue, "recv_queue_mem",1, recv_queue_mem, RECV_QUEUE_MEM_SIZE);


		txm_module_object_allocate(&out_tx_mutex, sizeof(TX_MUTEX));
		tx_mutex_create(out_tx_mutex,"out_tx_mutex", TX_NO_INHERIT);

		txm_module_object_allocate(&out_tx_done_sem, sizeof(TX_SEMAPHORE));
		tx_semaphore_create(out_tx_done_sem,"out_tx_done_sem", 1);


		/* RX */
		//txm_module_object_allocate(&in_rx_sem1, sizeof(TX_SEMAPHORE));
		//tx_semaphore_create(in_rx_sem1,"in_rx_sem1", 0);

		//txm_module_object_allocate(&in_rx_sem2, sizeof(TX_SEMAPHORE));
		//tx_semaphore_create(in_rx_sem2,"in_rx_sem2", 0);


		txm_module_object_allocate(&evnt_rx, sizeof(TX_EVENT_FLAGS_GROUP));
		tx_event_flags_create(evnt_rx,"evnt_rx");

		txm_module_object_allocate(&rx_queue_thread_byte_pool, sizeof(TX_BYTE_POOL));
		tx_byte_pool_create(rx_queue_thread_byte_pool, "rx_queue_thread_byte_pool", rx_queue_thread_mem, RX_QUEUE_THREAD_BYTE_POOL_SIZE);

		tx_byte_allocate(rx_queue_thread_byte_pool, (VOID **) &rx_queue_thread_stack, RX_QUEUE_THREAD_STACK_SIZE, TX_NO_WAIT);	
		txm_module_object_allocate(&rx_queue_thread_handle, sizeof(TX_THREAD));
		tx_thread_create(rx_queue_thread_handle,
							"rx_queue_task",
						   	rx_queue_task,
						   	(ULONG)0,
						   	rx_queue_thread_stack,
						   	RX_QUEUE_THREAD_STACK_SIZE,
						   	32,
						   	16,
						   	TX_NO_TIME_SLICE,
						   	TX_AUTO_START
		);

		while(qapi_UART_Receive (handle, bytes1, RX_SIZE, &byte_num1) != QAPI_OK); // queue as per doc
		while(qapi_UART_Receive (handle, bytes2, RX_SIZE, &byte_num2) != QAPI_OK); // queue as per doc




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


static void rx_queue_task(ULONG param){

	ULONG actual_events;
	bytes1_cnt = 0;
	bytes2_cnt = 0;

	for(;;){


		if(tx_event_flags_get(evnt_rx,byte_num1,TX_OR_CLEAR,&actual_events,1) == TX_SUCCESS){
			uint32_t val;
			__atomic_load(&bytes1_cnt,&val,__ATOMIC_SEQ_CST);
			for(int i =0; i < val; i++)
				tx_queue_send(recv_queue,&bytes1[i],TX_WAIT_FOREVER);
			while(qapi_UART_Receive (handle, bytes1, RX_SIZE, &byte_num1) != QAPI_OK); // queue as per doc
		}

		if(tx_event_flags_get(evnt_rx,byte_num2,TX_OR_CLEAR,&actual_events,1) == TX_SUCCESS){
			uint32_t val;
			__atomic_load(&bytes2_cnt,&val,__ATOMIC_SEQ_CST);
			for(int i =0; i < val; i++)
				tx_queue_send(recv_queue,&bytes2[i],TX_WAIT_FOREVER);
			while(qapi_UART_Receive (handle, bytes2, RX_SIZE, &byte_num2) != QAPI_OK); // queue as per doc
		}

		tx_thread_relinquish();

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

	int c = character; // make a local copy

	tx_mutex_get(out_tx_mutex,TX_WAIT_FOREVER);

	if(qapi_UART_Transmit(handle, &c, 1, NULL) == TX_SUCCESS)	// puts removes the new line!
		tx_semaphore_get(out_tx_done_sem,TX_WAIT_FOREVER);
	
	tx_mutex_put(out_tx_mutex);

	return c;
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

	if(!handle)
		return 0;
	
	int c = EOF;

	if(tx_queue_receive(recv_queue, &c, 10) != TX_SUCCESS)
		c = EOF;
	//tx_queue_receive(recv_queue_global, &c, TX_WAIT_FOREVER);

	return c;
}


int __wrap_fprintf(FILE *stream, const char *format, ...){
	puts("error\r\n");
	return 0;
}

size_t __wrap_write(int fildes, const void *buf, size_t nbytes1){

	// We only have stdout
	if(!handle)
		return 0;

	tx_mutex_get(out_tx_mutex,TX_WAIT_FOREVER);

	if(qapi_UART_Transmit(handle, buf, nbytes1, NULL) == QAPI_OK)	
		tx_semaphore_get(out_tx_done_sem,TX_WAIT_FOREVER);
	else
		nbytes1 = -1;
	
	tx_mutex_put(out_tx_mutex);

	return nbytes1;

}

int __wrap_fflush(FILE *stream){
	if(!handle)	return 0;

	tx_queue_flush(recv_queue);

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