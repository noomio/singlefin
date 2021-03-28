#if !defined(FIN_API_UART_H_INCLUDED)
#define FIN_API_UART_H_INCLUDED

int fin_uart_config(uint8_t serial, uint32_t baud_rate, uint32_t stop_bits, uint32_t bits_per_char, uint32_t parity, uint32_t flow_ctrl);
int fin_uart_write(uint8_t serial, uint8_t *buf, size_t nbytes);
int fin_uart_read(uint32_t serial, uint8_t *buf, size_t nbytes);
int fin_uart_deconfig(uint32_t serial);
int fin_uart_power_up(uint32_t serial);
int fin_uart_power_down(uint32_t serial);

#endif  /* FIN_API_UART_H_INCLUDED */
