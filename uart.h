#ifndef uart
#define uart

#include <stdint.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"

#define UART0
//#define UART1
//#define UART5
//#define UART3
#define LOOPBACK_MODE


#define UART0_RECEIVE 0
#define UART1_RECEIVE 1
#define UART5_RECEIVE 5
#define UART3_RECEIVE 3

#define UART0_TRANSMIT 0
#define UART1_TRANSMIT 1
#define UART5_TRANSMIT 5
#define UART3_TRANSMIT 3

#define FIVE_BITS (0x0<<5)
#define SIX_BITS (0x1<<5)
#define SEVEN_BITS (0x2<<5)
#define EIGHT_BITS (0x3<<5)

void uart_init();
uint8_t uart_receive (uint8_t uart_no);
void uart_transmit (uint8_t data, uint8_t uart_no);
uint8_t* uart_receive_string (uint8_t* str, uint8_t uart_no);
void uart_transmit_string (uint8_t* data, uint8_t uart_no);

#endif