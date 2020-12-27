#if !defined(EVENT_MNGR_H_INCLUDED)
#define EVENT_MNGR_H_INCLUDED

#include "txm_module.h"
#include "duktape.h"


#define EVENT_MSG_MAX	25

int event_mngr_task(ULONG arg);

typedef enum EVENT_MSG_IDS{
	MGC_TYPE_NULL = 0,
	MSG_TYPE_TIMER = 0xFFFF000F,
	MSG_TYPE_PRINT,
	MSG_TYPE_GPIO_INT,
	MSG_TYPE_AT_OPEN,
	MSG_TYPE_AT_CLOSE,
	MSG_TYPE_AT_WRITE,
	MSG_TYPE_AT_WRITE_CB,
	MSG_TYPE_AT_CB_PIPE,
	MSG_TYPE_I2C_OPEN,
	MSG_TYPE_I2C_CLOSE,
	MSG_TYPE_I2C_TRANSFER_CB
} event_msg_id_t;


typedef struct event_msg{
	event_msg_id_t msg_id;
	void *msg;	
}event_msg_t;

TX_SEMAPHORE 			*evnt_gatekeeper;
TX_EVENT_FLAGS_GROUP	*evnt_group;

/*
	EVENT MNGR MSG POLL
*/
TX_QUEUE *event_mngr_msg_queue;
unsigned char event_mngr_msg_mem[sizeof(event_msg_t)*EVENT_MSG_MAX];
#define EVENT_MNGR_ELEMENT_SIZE	sizeof(event_msg_t)/sizeof(uint32_t) /* Pointers*/


/*
*	EVENT MNGR TASK
*/
TX_THREAD* event_mngr_thread_handle; 
void *event_mngr_thread_stack;
#define EVENT_MNGR_TASK_STACK_SIZE			8*1024
TX_BYTE_POOL *event_mngr_task_byte_pool;
#define EVENT_MNGR_TASK_BYTE_POOL_SIZE		EVENT_MNGR_TASK_STACK_SIZE+(EVENT_MNGR_TASK_STACK_SIZE/2)
char event_mngr_task_mem[EVENT_MNGR_TASK_BYTE_POOL_SIZE];

/*
*	WORKER TASK
*/
int worker_task(ULONG arg);
TX_THREAD* worker_thread_handle; 
void *worker_thread_stack;
#define WORKER_TASK_STACK_SIZE			2*1024


/*
	TIMERS
*/

#define TIMERS_NAME_LEN 16
#define TIMERS_MAX	6

typedef struct timer_store{
	TX_TIMER *timer;
    char name[TIMERS_NAME_LEN];
    bool oneshot;
}timer_store_msg_t;



TX_BLOCK_POOL *timer_pool;
char timer_mem[TIMERS_MAX*sizeof(timer_store_msg_t)];

#define TIMER_MODULE_NAME	"timer"

#define TIMER_SET_TIMEOUT_NAME		"setTimeout"
duk_ret_t native_timer_set_timeout(duk_context *ctx);

#define TIMER_SET_INTERVAL_NAME		"setInterval"
duk_ret_t native_timer_set_interval(duk_context *ctx);

#define TIMER_CLEAR_INTERVAL_NAME		"clearInterval"
duk_ret_t native_timer_clear_interval(duk_context *ctx);


/*
*	GPIO
*/

#define GPIO_DIGITAL_IN_NAME		"DigitalIn"
duk_ret_t native_digital_in(duk_context *ctx);

#define GPIO_DIGITAL_OUT_NAME		"DigitalOut"
duk_ret_t native_digital_out(duk_context *ctx);

#define GPIO_DIGITAL_IN_OUT_NAME	"DigitalInOut"
duk_ret_t native_digital_in_out(duk_context *ctx);

#define GPIO_DIGITAL_INTERRUPT_IN_NAME	"InterruptIn"
duk_ret_t native_digital_interrupt_in(duk_context *ctx);

duk_ret_t native_init_gpio_consts(duk_context *ctx);


/*
*	AT
*/

#define AT_MSG_MAX 12
#define AT_DATA_MAX 4
#define AT_CMD_LEN 128

/*
* Follows qapi_quactel.h 
*/
typedef struct pipe_data
{
	char   data[2048];
	int    len;
} pipe_data_t; 

typedef void  (*at_callback_t)(pipe_data_t *resp);

typedef struct at_pipe_map_tbl{
	int stream;
	uint8_t port;
	const char *cmd;
    const char *open;
    const char *close;
    const char *data;
    const char *error;
	pipe_data_t pipe;	// used in callback
	char *buffer;		// duk buffer to be passed on 
	int buffer_tot_len;	
	at_callback_t callback;
	TX_SEMAPHORE *lock;
}at_pipe_map_tbl_t;

TX_QUEUE *at_msg_queue;
TX_QUEUE *at_data_queue;
unsigned char at_msg_mem[sizeof(event_msg_t)*(AT_MSG_MAX+AT_DATA_MAX)];
#define AT_MSG_ELEMENT_SIZE	sizeof(event_msg_t)/sizeof(uint32_t) /* Pointers*/

#define AT_NAME		"at"
duk_ret_t native_at(duk_context *ctx);
duk_ret_t native_init_at_consts(duk_context *ctx);


/*
*	I2C
*/

typedef struct I2C_MAP_TBL{
    uint32_t no;
    uint32_t instance;
    void *handle;
    uint32_t param;
    uint32_t transferred1;
	uint32_t transferred2;
    const char *open;
    const char *close;
    const char *data;
    const char *error;
    const char *rx_name_ref;
    void *rx_data_ref;	// reference from js
    TX_SEMAPHORE *lock;
}i2c_map_tbl_t;

#define I2C_NAME		"i2c"
duk_ret_t native_i2c(duk_context *ctx);
duk_ret_t native_init_i2c_consts(duk_context *ctx);

/*
	PRINT
*/
#define PRINT_NAME		"print"
duk_ret_t native_print(duk_context *ctx);

#endif