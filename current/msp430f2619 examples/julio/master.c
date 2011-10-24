#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <msp430x2619.h>
#include "LabProc.h"
#include "delay.h"
#include "nokia2112.h"
#include "Tabela_ASCII.h"
#include "i2c.h"
#include "sqrtf.c"


// ****************************************************************
// ** PROTÓTIPOS                                                 **
// ****************************************************************
//
int putchar(int c);
void jprintf(float f, int tamanho);
float jsqrt(float f);


// ****************************************************************
// ** MAIN                                                       **
// ****************************************************************
//
int main (void)
{
  float raiz;
  int i;

  configura_cpu();
  i2c_init();
  lcd_initmodule(nao_virado);

  // Testa a tabela ASCII...
  //
  for(i=0; i<128; i++) lcd_putchar(i);  
  delayms(1000);
  lcd_clrscr();

  printf("Ola Edinho...");
  lcd_goto(0,1);
  raiz = jsqrt(2.524);
  jprintf(-raiz, 4);

  lcd_goto(0,2);
  raiz = sqrt(2);
  jprintf(raiz, 4);

  while (1)
  {
    led = 0;
    delayms(100);
    led = 1;
    delayms(100);
  }
}

// Printf implementation...
//
int putchar(int c)
{
  lcd_wrchar(c);
  return 0;
}


void jprintf(float f, int tamanho)
{
  int fi;
  unsigned int ff;

  fi = f;
  switch (tamanho)
  {
    case 1 : ff = abs((f-fi)*10);
             printf ("f=%d.%01d", fi, ff);
             break;
    case 2 : ff = abs((f-fi)*100);
             printf ("f=%d.%02d", fi, ff);
             break;
    case 3 : ff = abs((f-fi)*1000);
             printf ("f=%d.%03d", fi, ff);
             break;
    case 4 : ff = abs((f-fi)*10000);
             printf ("f=%d.%04d", fi, ff);
             break;
  }
}


float jsqrt(float f)
{
  int i;
  float s = 1;

  for(i=0; i<15; i++)
  {
    s = 0.5*((f/s)+s);
  }
  return s;
}



