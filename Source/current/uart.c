#include "uart.h"

void UART_init(){ // poderia passar o baudrate como parametro...
    UCA1CTL1 |= UCSWRST;
    UCA1CTL0 = 0x00;                                         
    UCA1CTL1 |= UCSSEL_1;
    //UCA1BR0 = 17;//57800
    UCA1BR0 = 26;//38400
    UCA1BR1 = 0; 
    //UCA1MCTL = UCBRF_2 | UCBRF_0 | UCOS16; // 57800
    UCA1MCTL = UCBRF_0 | UCOS16; // 38400
    UCA1CTL1 &= ~UCSWRST;
    //UC1IFG |= UCA1TXIE | UCA1RXIE;
    
    P3SEL |= BIT6 | BIT7;
    P3DIR |= BIT6 | BIT7;
}

int putchar(int c) // serve pra poder usar printf()
{
    while(!(UC1IFG & UCA1TXIFG));
    UCA1TXBUF = c;
    return 0;
}

