

#ifndef UART_INTERRUPT_H_
#define UART_INTERRUPT_H_

#include <stdint.h>

extern volatile char received_char;

void uart_interrupt_init(void);
void uart_sendChar(char data);
void uart_sendStr(const char *data);
void UART1_Handler(void);

#endif /* UART_INTERRUPT_H_ */
