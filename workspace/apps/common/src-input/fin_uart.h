#if !defined(FIN_API_UART_H_INCLUDED)
#define FIN_API_UART_H_INCLUDED

typedef enum fin_uart{
	UART1,
	UART2,
	UART3	
} fin_uart_t;

int fin_uart_config(fin_uart_t uart, uint32_t baud_rate, uint32_t stop_bits, uint32_t bits_per_char, uint32_t parity, uint32_t flow_ctrl);
int fin_uart_write(fin_uart_t uart, uint8_t *buf, size_t nbytes);
int fin_uart_read(fin_uart_t uart, uint8_t *buf, size_t nbytes);
int fin_uart_deconfig(fin_uart_t uart);
int fin_uart_power_up(fin_uart_t uart);
int fin_uart_power_down(fin_uart_t uart);

#endif  /* FIN_API_UART_H_INCLUDED */
