#ifndef __BSP_UART_H
#define __BSP_UART_H

#include "imx6u.h"

void uart1_init(void);
void uart1_gpio_init(void);
void uart_disable(UART_Type *base);
void uart_enable(UART_Type *base);
void uart_soft_reset(UART_Type *base);
void putc(unsigned char c);
unsigned char getc(void);
void puts(char *str);
void uart_setbaudrate(UART_Type *base, unsigned int baudrate, unsigned int srcclock_hz);

#endif
