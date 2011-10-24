//
// ****************************************************************
// **                                                            **
// **  D E L A Y                                                 **
// **                                    (C) by JCLima, 2010/1   **
// **                                                            **
// ****************************************************************
//
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <msp430x2619.h>


void delay2us(void)
{
  unsigned int tempo = 4;

  do (tempo--);
  while (tempo != 0);
}

void delay5us(void)
{
  unsigned int tempo = 10;

  do (tempo--);
  while (tempo != 0);
}

void delayus(unsigned int tempo)
{
  do (tempo--);
  while (tempo != 0);
}  

void delayms(unsigned int tempo)
{
  do 
  {
    tempo--;
    delayus(1000);
  } while (tempo != 0);
}  

