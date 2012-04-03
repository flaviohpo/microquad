#include "uart.h"
#include "legacymsp430.h"
#include "msp430f2618.h"

void UART_init(){ // poderia passar o baudrate como parametro...
    UCA0CTL1 |= UCSWRST;
    UCA0CTL0 = 0x00;                                         
    UCA0CTL1 |= UCSSEL_1;
    UCA0BR0 = 130;
    UCA0BR1 = 6; 
    UCA0MCTL = UCBRF_0;
    UCA0CTL1 &= ~UCSWRST;
    
    P3SEL |= BIT4 | BIT5;
    P3DIR |= BIT4 | BIT5;
}

unsigned char UART_putchar(unsigned char c){
    while(!(UC0IFG & UCA0TXIFG));
    UCA0TXBUF = c;
    return 0;
}

