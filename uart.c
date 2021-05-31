#include "uart.h"

void uart_init(){
#ifdef UART0
  
  //Enabling clock for the uart
  SYSCTL_RCGCUART_R = 0x1;
  

  //8 bits and enabling FIFO
  UART0_LCRH_R = EIGHT_BITS | (1<<4);
  //System clock = 16M and clock divider 16 and we want 9600 baud rate so the divider = 104.1667
  UART0_IBRD_R = 0x104;
  //fraction value calcuating by multiplying 64 and adding 0.5 for error (0.1667 * 64 + 0.5 = 11)
  UART0_FBRD_R = 11;
  
  //uart sandwich (disabling then enabling)
  UART0_CTL_R = 0;
  
  //Enabling TX and RX
  UART0_CTL_R = (1<<8) | (1<<9);
  
//if the loopback mode is defined then enable it
#ifdef LOOPBACK_MODE
  UART0_CTL_R|= (1<<7);
#endif
  
  UART0_CTL_R |= (1<<0);
  
  //Enabling clock for port A
  SYSCTL_RCGCGPIO_R |= 0x1;
  //PA1 output and PA0 input
  GPIO_PORTA_DIR_R |= (1<<1);
  GPIO_PORTA_DIR_R &= ~(1<<0);
  
  //Enabling uart for PA0 and PA1
  GPIO_PORTA_PCTL_R |= (1<<0) | (1<<4);
  //Using alternate functions for PA0 and PA1
  GPIO_PORTA_AFSEL_R |= 0x03;
  //Turning off analog function
  GPIO_PORTA_AMSEL_R = 0;
  //Enabling PA0 and PA1
  GPIO_PORTA_DEN_R |= 0x03;
  
#endif
  
#ifdef UART1
  
  //Enabling clock for the uart
  SYSCTL_RCGCUART_R = 0x2;
  
  //8 bits and enabling FIFO
  UART1_LCRH_R = EIGHT_BITS | (1<<4);
  //System clock = 16M and clock divider 16 and we want 9600 baud rate so the divider = 104.1667
  UART1_IBRD_R = 0x104;
  //fraction value calcuating by multiplying 64 and adding 0.5 for error (0.1667 * 64 + 0.5 = 11)
  UART1_FBRD_R = 11;
  
  //uart sandwich (disabling then enabling)
  UART1_CTL_R = 0;
  //Enabling TX and RX
  UART1_CTL_R = (1<<8) | (1<<9);

//if the loopback mode is defined then enable it
#ifdef LOOPBACK_MODE
  UART1_CTL_R|= (1<<7);
#endif
  
  UART1_CTL_R |= (1<<0);
  
  //Enabling clock for port B
  SYSCTL_RCGCGPIO_R |= 0x2;
  //PB1 output and PB0 input
  GPIO_PORTB_DIR_R |= (1<<1);
  GPIO_PORTB_DIR_R &= ~(1<<0);
  
  //Enabling uart for PB0 and PB1
  GPIO_PORTB_PCTL_R |= (1<<0) | (1<<4);
  //Using alternate functions for PB0 and PB1
  GPIO_PORTB_AFSEL_R = 0x03;
  //Turning off analog function
  GPIO_PORTB_AMSEL_R = 0;
  //Enabling Pb0 and PB1
  GPIO_PORTB_DEN_R = 0x03;
  
#endif
  
#ifdef UART5
  
  //Enabling clock for the uart
  SYSCTL_RCGCUART_R |= 0x20;
    
  //8 bits and enabling FIFO
  UART5_LCRH_R = EIGHT_BITS | (1<<4);
  //System clock = 16M and clock divider 16 and we want 9600 baud rate so the divider = 104.1667
  UART5_IBRD_R = 104;
  //fraction value calcuating by multiplying 64 and adding 0.5 for error (0.1667 * 64 + 0.5 = 11)
  UART5_FBRD_R = 11;
  
  //Selecting clock system
  UART5_CC_R = 0;

  //uart sandwich (disabling then enabling)
  UART5_CTL_R = 0;
  //Enabling TX and RX
  UART5_CTL_R = (1<<8) | (1<<9);

  //if the loopback mode is defined then enable it
#ifdef LOOPBACK_MODE
  UART5_CTL_R|= (1<<7);
#endif
  
  UART5_CTL_R |= (1<<0);
  
  //Enabling clock for port E
  SYSCTL_RCGCGPIO_R |= 0x10;
  
  //PE4 output and PE5 input
  GPIO_PORTE_DIR_R = (1<<5);
  GPIO_PORTE_DIR_R &= ~(1<<4);
  
  //Enabling uart for PD6 and PD7
  GPIO_PORTE_PCTL_R |= (1<<16) | (1<<20);
  //Using alternate functions for PE4 and PE5
  GPIO_PORTE_AFSEL_R = 0x30;
  //Turning off analog function
  GPIO_PORTE_AMSEL_R = 0;
  //Enabling PE4 and PE5
  GPIO_PORTE_DEN_R = 0x30;
  
#endif
  
#ifdef UART3
  
  //Enabling clock for the uart
  SYSCTL_RCGCUART_R |= 0x08;
  
  //8 bits and enabling FIFO
  UART3_LCRH_R = EIGHT_BITS | (1<<4);
  //System clock = 16M and clock divider 16 and we want 9600 baud rate so the divider = 104.1667
  UART3_IBRD_R = 104;
  //fraction value calcuating by multiplying 64 and adding 0.5 for error (0.1667 * 64 + 0.5 = 11)
  UART3_FBRD_R = 11;
  
  //Selecting clock system
  UART3_CC_R = 0;

  //uart sandwich (disabling then enabling)
  UART3_CTL_R = 0;
  //Enabling TX and RX
  UART3_CTL_R = (1<<8) | (1<<9);
  
  //if the loopback mode is defined then enable it
#ifdef LOOPBACK_MODE
  UART3_CTL_R|= (1<<7);
#endif
  
  UART3_CTL_R |= (1<<0);
  
  //Enabling clock for port C
  SYSCTL_RCGCGPIO_R |= 0x04;
  //PC7 output and PC6 input
  GPIO_PORTC_DIR_R |= (1<<7);
  GPIO_PORTC_DIR_R &= ~(1<<6);
  
  //Enabling uart for PC6 and PC7
  GPIO_PORTC_PCTL_R |= (1<<24) | (1<<28);
  //Using alternate functions for PC6 and PC7
  GPIO_PORTC_AFSEL_R = 0xC0;
  //Turning off analog function
  GPIO_PORTC_AMSEL_R = 0;
  //Enabling PC6 and PC7
  GPIO_PORTC_DEN_R = 0xC0;
#endif

}

uint8_t uart_receive (uint8_t uart_no){
  if (uart_no == UART0_RECEIVE){
    //Polling until data is received
    while ((UART0_FR_R&(1<<4)) != 0);
    //return the data register content
    return UART0_DR_R;
  }
  else if (uart_no == UART1_RECEIVE){
    //Polling until data is received
    while ((UART1_FR_R&(1<<4)) != 0);
    //return the data register content
    return UART1_DR_R;
  }
  else if (uart_no == UART5_RECEIVE){
    //Polling until data is received
    while ((UART5_FR_R&(1<<4)) != 0);
    //return the data register content
    return UART5_DR_R;
  }
  else if (uart_no == UART3_RECEIVE){
    //Polling until data is received
    while ((UART3_FR_R&(1<<4)) != 0);
    //return the data register content
    return UART3_DR_R;
  }
}

void uart_transmit (uint8_t data, uint8_t uart_no){
  
  if (uart_no == UART0_TRANSMIT){
    //Polling until data can be transmitted
    while ((UART0_FR_R&(1<<5)) != 0);
    //Put the data in the data register
    UART0_DR_R = data;
  }
  else if (uart_no == UART1_TRANSMIT){
    //Polling until data can be transmitted
    while ((UART1_FR_R&(1<<5)) != 0);
    //Put the data in the data register
    UART1_DR_R = data;
  }
  else if (uart_no == UART5_TRANSMIT){
    //Polling until data can be transmitted
    while ((UART5_FR_R&(1<<5)) != 0);
    //Put the data in the data register
    UART5_DR_R = data;
  }
  else if (uart_no == UART3_TRANSMIT){
    //Polling until data can be transmitted
    while ((UART3_FR_R&(1<<5)) != 0);
    //Put the data in the data register
    UART3_DR_R = data;
  }
}

uint8_t* uart_receive_string (uint8_t* str, uint8_t uart_no){
  uint8_t i=0;
  str[i] = uart_receive(UART0_RECEIVE);
  while (str[i] != '.'){
    i++;
    str[i] = uart_receive(UART0_RECEIVE);
  }
  str[i] = '\0';
}

void uart_transmit_string (uint8_t* data, uint8_t uart_no){
  while (*data != '\0'){
    uart_transmit(*data, UART0_TRANSMIT);
    data++;
  }
}