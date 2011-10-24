#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <msp430x2619.h>
#include "LabProcI.h"
#include "delay.h"
#include "lcd16x2.h"
#include "i2c.h"


// ****************************************************************
// ** PROTÓTIPOS                                                 **
// ****************************************************************
//
int putchar(int c);
void config_serial(void);
void send_serial(unsigned char c);
unsigned char receive_serial(void);


// ****************************************************************
// ** MAIN                                                       **
// ****************************************************************
//
int main (void)
{
//  unsigned char i;

  configura_cpu();

  P3SEL = 0x30;
  P3DIR = 0xdf;
  P5SEL = 0x70;
  P5DIR = 0x70;

  config_serial();
  i2c_init();
  lcd_init(cursorligado);
  printf("LAB PROC 2010/I");

  send_serial('J');
  send_serial('U');
  send_serial('L');
  send_serial('I');
  send_serial('O');

  while (1)
  {
    led = 0;
    delayms(30);
    led = 1;
    delayms(30);
  }
}

// Printf implementation...
//
int putchar(int c)
{
  lcd_wrchar(c);
  return 0;
}

void config_serial(void)
{
  UCA0CTL1 = 0x41;
  UCA0CTL0 = 0;
  UCA0BR0 = 104;
  UCA0BR1 = 0;
  UCA0MCTL = 0x31;
  UCA0CTL1 = 0x40;
}

void send_serial(unsigned char c)
{
  while(UCA0TXIFG == 1);
  UCA0TXBUF = c;
}

unsigned char receive_serial(void)
{
  while(UCA0RXIFG == 1);
  return UCA0RXBUF;
}


