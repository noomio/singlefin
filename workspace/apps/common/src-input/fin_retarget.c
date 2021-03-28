/*
 * retarget.c
 *
 *  Created on: 24 Nov.,2019
 *      Author: Nikolas Karakotas
 */
#include "fin_internal.h"

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


	// toggle pseudo STATUS pin
	gpio_pin_config(64,QAPI_GPIO_PULL_UP_E,QAPI_GPIO_12MA_E,QAPI_GPIO_OUTPUT_E);
	gpio_pin_write(64,false);
	sleep(1);
	gpio_pin_write(64,true);

	// Now read fault 
	gpio_pin_release(64);
	gpio_pin_config(64,QAPI_GPIO_PULL_UP_E,QAPI_GPIO_12MA_E,QAPI_GPIO_INPUT_E);
	if(gpio_pin_read(64) == 0){
		puts("[ PROGRAM FAULT ]\r\n");
		exit(1);
	}
	
	main();
	for(;;);
}


static qapi_UART_Handle_t handle = NULL;

TX_MUTEX *out_tx_mutex;
TX_SEMAPHORE *out_tx_done_sem;

#ifndef TX_PRINTF_LEN
#define TX_PRINTF_LEN 512
#elif TX_PRINTF_LEN < 128
#error "TX_PRINTF_LEN < 128"
#endif

#define RECV_QUEUE_MEM_SIZE 512

TX_QUEUE *recv_queue;
uint32_t recv_queue_mem[RECV_QUEUE_MEM_SIZE];



static void rx_queue_task(ULONG param);
TX_THREAD* rx_queue_thread_handle; 
TX_BYTE_POOL *rx_queue_thread_byte_pool;
void *rx_queue_thread_stack;
#define RX_QUEUE_THREAD_STACK_SIZE			512
#define RX_QUEUE_THREAD_BYTE_POOL_SIZE		2*RX_QUEUE_THREAD_STACK_SIZE
char rx_queue_thread_mem[RX_QUEUE_THREAD_BYTE_POOL_SIZE];

#define RX_SIZE RECV_QUEUE_MEM_SIZE/2
volatile uint8_t bytes1[RX_SIZE];
volatile uint8_t bytes2[RX_SIZE];

TX_EVENT_FLAGS_GROUP	*evnt_rx;
const int BYTE_NUM_1 = 1;
const int BYTE_NUM_2 = 2;
#define RX_BUF_SIZE RECV_QUEUE_MEM_SIZE/4
volatile uint8_t rx_buf[RX_BUF_SIZE];
uint32_t rx_buf_head;
uint32_t rx_buf_tail;


static void uart_rx_cb(uint32_t num_bytes, void *cb_data){

	int num = *(int*)cb_data;

	if(num == BYTE_NUM_1){
		for(int i=0; i < num_bytes; i++){
			uint32_t index = __atomic_fetch_add(&rx_buf_head,1,__ATOMIC_RELAXED);
			if(index+1 >= RX_BUF_SIZE-1){
				__atomic_store_n(&rx_buf_head,0,__ATOMIC_RELAXED);
				index = 0;
			}
			__atomic_store_n (&rx_buf[index], bytes1[i], __ATOMIC_RELAXED);
		}
	}
	else if(num == BYTE_NUM_2){
		for(int i=0; i < num_bytes; i++){
			uint32_t index = __atomic_fetch_add(&rx_buf_head,1,__ATOMIC_RELAXED);
			if(index+1 >= RX_BUF_SIZE-1){
				__atomic_store_n(&rx_buf_head,0,__ATOMIC_RELAXED);
				index = 0;
			}
			__atomic_store_n (&rx_buf[index], bytes2[i], __ATOMIC_RELAXED);
		}
	}


	tx_event_flags_set(evnt_rx,num, TX_OR);


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

		while(qapi_UART_Receive (handle, bytes1, RX_SIZE, &BYTE_NUM_1) != QAPI_OK); // queue as per doc
		while(qapi_UART_Receive (handle, bytes2, RX_SIZE, &BYTE_NUM_2) != QAPI_OK); // queue as per doc


	}
 
}


static void rx_queue_task(ULONG param){

	ULONG actual_events;
	rx_buf_tail = 0;
	rx_buf_head = 0;

	for(;;){


		if(tx_event_flags_get(evnt_rx,BYTE_NUM_1,TX_OR_CLEAR,&actual_events,1) == TX_SUCCESS){
			while(qapi_UART_Receive (handle, bytes1, RX_SIZE, &BYTE_NUM_1) != QAPI_OK); // queue as per doc
		}

		if(tx_event_flags_get(evnt_rx,BYTE_NUM_2,TX_OR_CLEAR,&actual_events,1) == TX_SUCCESS){
			while(qapi_UART_Receive (handle, bytes2, RX_SIZE, &BYTE_NUM_2) != QAPI_OK); // queue as per doc
		}

		uint32_t head = __atomic_load_n(&rx_buf_head,__ATOMIC_RELAXED);
		while(rx_buf_tail != head){
			uint8_t byte = __atomic_load_n(&rx_buf[rx_buf_tail],__ATOMIC_RELAXED);
			rx_buf_tail++;
			if(rx_buf_tail >= RX_BUF_SIZE-1)
				rx_buf_tail = 0;
			tx_queue_send(recv_queue,&byte,TX_WAIT_FOREVER);
		}
		tx_thread_relinquish();

	}

}

int putchar(int character){
	
	if(!handle)
		return 0;

	int c = character; // make a local copy

	if(qapi_UART_Transmit(handle, &c, 1, NULL) == TX_SUCCESS)	// puts removes the new line!
		tx_semaphore_get(out_tx_done_sem,TX_WAIT_FOREVER);
	

	return c;
}

int _putchar(int character){
	return putchar(character);
}

int __wrap_puts(const char *s){

	
	if(!handle)
		return 0;

	tx_mutex_get(out_tx_mutex,TX_WAIT_FOREVER);

	int i = 0;
	int ret = 1;
   	while(s[i]){
	    if( putchar(s[i]) == -1) { 
	        ret = -1;
	    }
	    i++;
    }
	if(putchar('\n') == -1) {

	   ret = -1;
	}

	tx_mutex_put(out_tx_mutex);
   return ret; 

}


/*

*/
int __wrap_printf(const char *format, ...){

	va_list ap;
	int len;

	if(!handle)
		return 0;

	
	char *stdout_buf = malloc(TX_PRINTF_LEN);
	if(!stdout_buf)
		return 0;

	tx_mutex_get(out_tx_mutex,TX_WAIT_FOREVER);

	va_start( ap, format );
	//need to guard when no argumets will call putchar
	len = vsnprintf_( stdout_buf, (size_t)TX_PRINTF_LEN, format, ap ); 
	va_end( ap );

	if(qapi_UART_Transmit(handle, stdout_buf, len, NULL) == TX_SUCCESS)
		tx_semaphore_get(out_tx_done_sem,TX_WAIT_FOREVER);

	free(stdout_buf);

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
	
	int c;

	tx_queue_receive(recv_queue, &c, TX_WAIT_FOREVER);


	return c;
}


int __wrap_fprintf(FILE *stream, const char *format, ...){
	puts("__wrap_fprintf: error\r\n");
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