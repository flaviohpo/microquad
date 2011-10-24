#include <msp430x2619.h>
#include <signal.h>     // definição das funções dint() e eint()
#include "uart1.h"

//
// Definições de baudrate (associação com modulador)
//
#define mod_9600        0x31
#define mod_19200       0x11
#define mod_38400       0x11
#define mod_57600       0x61
#define mod_115200      0x3b
#define mod_230400      0x27

//
// Tamanho dos buffers de entrada e saída...
//
#define RX_lenght  32

//
// Variaveis e bits de controle...
//
unsigned char RX_in_index, RX_out_index, RX_count;
unsigned char RX_combuffer[RX_lenght];
unsigned char evRXempty;
unsigned char evRXoverflow;


//
// Retorna o valor de RX_count
//
unsigned char comport_inputcount(void)
{
  return RX_count;
}

//
// Configura a porta serial 0
//
void comport_init(unsigned int baud)
{
  UCA0CTL1 = 0x81;
  UCA0CTL0 = 0x08;
  UCA0BR0 = baud;
  UCA0BR1 = 0;
  if(baud == baud_9600) UCA0MCTL = mod_9600;
  if(baud == baud_19200) UCA0MCTL = mod_19200;
  if(baud == baud_38400) UCA0MCTL = mod_38400;
  if(baud == baud_57600) UCA0MCTL = mod_57600;
  if(baud == baud_115200) UCA0MCTL = mod_115200;
  if(baud == baud_230400) UCA0MCTL = mod_230400;
  UCA0CTL1 = 0x80;

  IE2 |= UCA0RXIE;
  eint();

  RX_count = 0;
  RX_in_index = RX_out_index = 0;

  evRXempty    = 1;
  evRXoverflow = 0;
}

//
// Clear any error report and buffers!!!
//
void comport_flush(void)
{
  IFG2 &= ~UCA0RXIFG;

  RX_count = 0;
  RX_in_index = RX_out_index = 0;

  evRXempty    = 1;
  evRXoverflow = 0;
}

//
// Interrupt handlers for serial comport
//
interrupt (USCIAB0RX_VECTOR) RX_ISR(void)
{
  evRXempty = 0;
  if (RX_count++ == RX_lenght) evRXoverflow = 1;
  RX_combuffer[RX_in_index++] = UCA0RXBUF;
  if (RX_in_index == RX_lenght) RX_in_index = 0;
}

void comport_putc(unsigned char c)
{
  while(!(IFG2 & UCA0TXIFG));
  UCA0TXBUF = c;                   
}

unsigned char comport_getc(void)
{
  unsigned char tmp;
  while (evRXempty);

  tmp = RX_combuffer[RX_out_index++];
  if (RX_out_index == RX_lenght) RX_out_index = 0;
  if (--RX_count == 0) evRXempty = 1;
  
  return tmp;
}

//
// Pooling Functions...
//
unsigned char comport_recvchar(void)
{
  while(!(IFG2 & UCA0RXIFG));
  return UCA0RXBUF;
}

void comport_sendchar(unsigned char c)
{
  while(!(IFG2 & UCA0TXIFG));
  UCA0TXBUF = c;                   
}


