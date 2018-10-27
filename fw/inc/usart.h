#ifndef USART_H
#define USART_H

#include <stdint.h>
#define MODEM_USART        0        
#define WIFI_USART         1
#define AUU_USART          2 
#define FLOWMETER_USART    3 
#define ASUPR_USART        5 


void uart_init(void);
void uart_irq_enable(uint8_t indx_uarts);

int uart_getchar(uint8_t indx_uarts, unsigned int timeout);
int uart_send_block(uint8_t indx_uarts, void *data, uint8_t len);
void uart_putchar(uint8_t indx_uarts, uint8_t byte);

#endif /* #ifndef USART_H */