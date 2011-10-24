#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <msp430x2619.h>
#include "lcd2112.h"
#include "uart0.h"

// Declaração de pinos...
//
#define led         port3.out.pin0

#define sw1         port1.in.pin0
#define sw2         port1.in.pin1
#define sw3         port1.in.pin2
#define sw4         port1.in.pin3

#define sda_in      port3.in.pin1
#define sda_out     port3.out.pin1
#define scl         port3.out.pin2

#define dataout()   P3DIR = 0xd7
#define datain()    P3DIR = 0xd5


// Definições...
//
#define no_lcd      0
#define na_serial   1
#define NACK        1
#define ACK         0

// Constantes e Variáveis...
//
const unsigned char hexadecimal[16] = {"0123456789ABCDEF"};
unsigned char aonde = no_lcd;


// Protótipos...
//
int putchar(int c);

void configura_cpu(void);
void delayus(unsigned int tempo);
void delayms(unsigned int tempo);

unsigned char readkeyboard(void);
unsigned char waitpress(void);
void waitrelease(void);
void waitkey(unsigned char key);
unsigned char waitanykey(void);


// M A I N
//
int main (void)
{
  unsigned char v;

  WDTCTL = WDTPW + WDTHOLD; 
  configura_cpu();
  lcd_initmodule(nao_virado);
  config_serial(baud_9600, mod_9600);
  
  aonde = no_lcd;
  printf("Ola display...\n\r");
  aonde = na_serial;

  while(1)
  {
    v = receive_serial();
    printf("Voce iniciou uma conversa ao digitar %c\n\r", v);
    printf("Pressione as teclas e aguarde a mensagem...\r\n");

    v = waitanykey();
    if(v == '3') printf("Ola Bluethooth! Mensagem de teste 1\n\r");
    if(v == '4') printf("Ola Bluethooth! Mensagem de teste 2\n\r");
  }    

  return 0;
}   

// Implementação do Printf...
//
int putchar(int c)
{
  if(aonde == no_lcd) lcd_putchar(c); else send_serial(c);
  return 0;
}

// Configuração da CPU...
//
void configura_cpu(void)
{
  // configura o clock para 16MHz...
  DCOCTL = 0x77;
  BCSCTL1 = 0x0f;
  BCSCTL2 = 0x08;
  BCSCTL3 = 0x8c;

  // Configura as portas...
  P1DIR = 0xf0;
  P2DIR = 0x00;
  P3DIR = 0xd7;  // <---
  P3SEL = 0x36;
  P4DIR = 0xff;
  P5DIR = 0x70;
  P5SEL = 0x70;

  dataout();
  led = 1;
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

// Primitivas de Teclado...
//
unsigned char readkeyboard(void)
{
  unsigned char tecla = 0;
  if(sw1 == 0) tecla = '1';
  if(sw2 == 0) tecla = '2';
  if(sw3 == 0) tecla = '3';
  if(sw4 == 0) tecla = '4';

  return tecla;
}

unsigned char waitpress(void)
{
  unsigned char v = 0;
  while(v == 0) 
  {
    v = readkeyboard();
    delayms(1);
  }

  return v;
}

void waitrelease(void)
{
  while(readkeyboard() != 0) delayms(1);
}

void waitkey(unsigned char key)
{
  while(readkeyboard() != key) delayms(1);
  waitrelease();
}

unsigned char waitanykey(void)
{
  unsigned char v;

  v = waitpress();
  waitrelease();

  return v;
}

