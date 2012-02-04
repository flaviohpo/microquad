#include "delay.h"
#include "msp430f2618.h"

void delay2us(void)
{
  unsigned int tempo = 6;

  do (tempo--);
  while (tempo != 0);
}

void delay5us(void)
{
  unsigned int tempo = 17;

  do (tempo--);
  while (tempo != 0);
}

// T = 0.2923us * tempo (aproximadamente...)
//
void delayus(unsigned int tempo)
{
  int i;

  for(i = 0; i < tempo; i++) nop();
}

// Aproximadamente 1ms...
//
void delayms(unsigned int tempo)
{
  int i;

  for(i = 0; i < tempo; i++) delayus(3420);
}
