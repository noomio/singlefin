/*
*
* @author :  Nikolas Karakotas
* @date   :  07/01/2021
*
*/
#include "fin_internal.h"

#define UART_NO_MAX 3
#define RECV_BUF_SIZE	128

struct uart_map {
	uint8_t serial_num;
	int8_t index;
	uint32_t port_id;
	qapi_UART_Handle_t handle;
	uint8_t recv_buf[RECV_BUF_SIZE];
	volatile uint8_t recvd[RECV_BUF_SIZE];
	volatile uint32_t recvd_head;
	volatile uint32_t recvd_tail;
};

struct uart_map uart_map[UART_NO_MAX] = {
	{UART1, -1, QAPI_UART_PORT_001_E, NULL, 0},
	{UART2, -1, QAPI_UART_PORT_002_E, NULL, 0},
	{UART3, -1, QAPI_UART_PORT_003_E, NULL, 0}
};

static void uart_rx_cb(uint32_t num_bytes, void *cb_data){
	int8_t index = *(int8_t*)cb_data;

	for(int i=0; i < num_bytes; i++){
		uint32_t head = __atomic_fetch_add(&uart_map[index].recvd_head,1,__ATOMIC_RELAXED);
		if(head+1 >= RECV_BUF_SIZE-1){
			__atomic_store_n(&uart_map[index].recvd_head,0,__ATOMIC_RELAXED);
			head = 0;
		}
		__atomic_store_n (&uart_map[index].recvd[head], uart_map[index].recv_buf[i], __ATOMIC_RELAXED);
	}

}

static void uart_tx_cb(uint32_t num_bytes1, void *cb_data){

}


int fin_uart_config(fin_uart_t uart, uint32_t baud_rate, uint32_t stop_bits, uint32_t bits_per_char, uint32_t parity, uint32_t flow_ctrl){

	qapi_UART_Open_Config_t uart_cfg;
	int index = -1;

	for(int i=0; i < UART_NO_MAX; i++){
		if(uart_map[i].serial_num == uart){
			index = i;
			uart_map[i].index = i;
			break;
		}
	}

	if(index > -1 ){

		uart_cfg.baud_Rate			= baud_rate;
		uart_cfg.enable_Flow_Ctrl	= flow_ctrl;
		uart_cfg.bits_Per_Char		= bits_per_char;
		uart_cfg.enable_Loopback 	= 0;
		uart_cfg.num_Stop_Bits		= stop_bits;
		uart_cfg.parity_Mode 		= parity;
		uart_cfg.rx_CB_ISR			= (qapi_UART_Callback_Fn_t)&uart_rx_cb;
		uart_cfg.tx_CB_ISR			= (qapi_UART_Callback_Fn_t)&uart_tx_cb;


		if(qapi_UART_Open(&uart_map[index].handle, uart_map[index].port_id, &uart_cfg) == QAPI_OK && 
			qapi_UART_Power_On(uart_map[index].handle) == QAPI_OK){
				while(qapi_UART_Receive (uart_map[index].handle, uart_map[index].recv_buf, RECV_BUF_SIZE, &uart_map[index].index) != QAPI_OK); // queue as per doc
				uart_map[index].recvd_head = 0;
				uart_map[index].recvd_tail = 0;
				return 0;
		}

	}

	return 1;

}

int fin_uart_write(fin_uart_t uart, uint8_t *buf, size_t nbytes){

	if(buf){

		for(int i=0; i < UART_NO_MAX; i++){
			if(uart_map[i].serial_num == uart){
				return qapi_UART_Transmit(uart_map[i].handle, buf, nbytes, &uart_map[i].serial_num);
			}
		}

	}

	return 1;
}

int fin_uart_read(fin_uart_t uart, uint8_t *buf, size_t nbytes){

	if(buf){

		for(int i=0; i < UART_NO_MAX; i++){
			if(uart_map[i].serial_num == uart){
				uint32_t head = __atomic_load_n(&uart_map[i].recvd_head,__ATOMIC_RELAXED);
				uint32_t z=0;
				while(nbytes){
					if(uart_map[i].recvd_tail != head){
						uint32_t tail = uart_map[i].recvd_tail;
						buf[z++] = __atomic_load_n(&uart_map[i].recvd[tail],__ATOMIC_RELAXED);
						uart_map[i].recvd_tail++;
						if(uart_map[i].recvd_tail >= RECV_BUF_SIZE-1)
							uart_map[i].recvd_tail = 0;
					}
					nbytes--;
				}

				// queue again - crude way
				while(qapi_UART_Receive (uart_map[i].handle, uart_map[i].recv_buf, RECV_BUF_SIZE, &uart_map[i].index) != QAPI_OK); // queue as per doc

				return z;

			}
		}

	}	

	return 0;
}

int fin_uart_deconfig(fin_uart_t uart){
	for(int i=0; i < UART_NO_MAX; i++){
		if(uart_map[i].serial_num == uart){
			qapi_UART_Power_Off(uart_map[i].handle);
			return qapi_UART_Close(uart_map[i].handle);
		}
	}

	return 1;
}


int fin_uart_power_down(fin_uart_t uart){
	for(int i=0; i < UART_NO_MAX; i++){
		if(uart_map[i].serial_num == uart){
			return qapi_UART_Power_Off(uart_map[i].handle);
		}
	}

	return 1;
}

int fin_uart_power_up(fin_uart_t uart){
	for(int i=0; i < UART_NO_MAX; i++){
		if(uart_map[i].serial_num == uart){
			return qapi_UART_Power_On(uart_map[i].handle);
		}
	}

	return 1;
}