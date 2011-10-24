#include <msp430x2619.h>

#define baud_9600       104
#define baud_19200      52
#define baud_38400      26
#define baud_57600      17
#define baud_115200     8
#define baud_230400     4

#define mod_9600        0x31
#define mod_19200       0x11
#define mod_38400       0x11
#define mod_57600       0x61
#define mod_115200      0x3b
#define mod_230400      0x27


void comport_init(unsigned char baud)
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
}

unsigned char comport_getc(void)
{
  while(!(IFG2 & UCA0RXIFG));
  return UCA0RXBUF;
}

void comport_putc(unsigned char c)
{
  while(!(IFG2 & UCA0TXIFG));
  UCA0TXBUF = c;
}

unsigned char comport_rxfull(void)
{
  return (IFG2 && UCA0RXIFG);
}

