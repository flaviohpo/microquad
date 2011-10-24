#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <msp430x2619.h>
#include "lcd2112.h"

#define led     port3.out.pin0

#define sw1     port1.in.pin0
#define sw2     port1.in.pin1
#define sw3     port1.in.pin2
#define sw4     port1.in.pin3

#define sda     port2.in.pin6 
#define scl     port2.in.pin7

unsigned char buffer[4000];
unsigned int contador;
const unsigned char hexadecimal[16] = {"0123456789ABCDEF"};


int putchar(int c);

void config_serial(void);
void send_serial(unsigned char c);
unsigned char receive_serial(void);

void configura_cpu(void);
void delayus(unsigned int tempo);
void delayms(unsigned int tempo);

unsigned char readkeyboard(void);
unsigned char waitpress(void);
void waitrelease(void);
void waitkey(unsigned char key);
unsigned char waitanykey(void);

void state_machine(void);

unsigned char hhex(unsigned char v);
unsigned char lhex(unsigned char v);

void sending(void);
void preencher(void);


int main (void)
{
  unsigned char v;

  WDTCTL = WDTPW + WDTHOLD;     // desliga watchdog timer
  configura_cpu();

  lcd_initmodule(nao_virado);
  config_serial();

  printf("SNIFFER...\n");  

  v = 0;
  while((v != '3') && (v != '4'))
  {
    v = waitanykey();
    if(v == '3') 
    {
      printf("Capturando...\n");
      printf("Ligue o I2C...");
      state_machine();
    }
  }

  while(1)
  {
//    i = 0;
//    while(str[i] != '\0') send_serial(str[i++]);
//    delayms(100);
  }    

  return 0;
}   

// Printf implementation...
//
int putchar(int c)
{
  lcd_wrchar(c);
  return 0;
}

// ** 9600 bps, 8N1 ...
//
void config_serial(void)
{
  UCA0CTL1 = 0x81;
  UCA0CTL0 = 0x00;
  UCA0BR0 = 104;
  UCA0BR1 = 0;
  UCA0MCTL = 0x31;
  UCA0CTL1 = 0x80;
}

unsigned char receive_serial(void)
{
  while(!(IFG2 & UCA0RXIFG));
  return UCA0RXBUF;
}

void send_serial(unsigned char c)
{
  while(!(IFG2 & UCA0TXIFG));
  UCA0TXBUF = c;                    // TX -> RXed character
}


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
  P3DIR = 0xd7;
  P3SEL = 0x30;
  P4DIR = 0xff;
  P5DIR = 0x70;
  P5SEL = 0x70;

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


void state_machine(void)
{
  unsigned int x;
  unsigned char nbits = 0;

  contador = 0;
  x = 0;

  estado_a:
    if (sw4 == 0) goto estado_g;
    if(sda && scl) goto estado_b;
    goto estado_a;

  estado_b:
    if (sw4 == 0) goto estado_g;
    if(!sda && scl)
    {
      led = 0;
      buffer[contador++] = '*';
      x = 0;
      nbits = 0;
      if(contador == 4000) goto estado_g;
      goto estado_c;
    }
    goto estado_b;
  estado_c:
    if (sw4 == 0) goto estado_g;
    if(!scl) goto estado_d;
    goto estado_c;
  estado_d:
    if (sw4 == 0) goto estado_g;
    if(sda && scl) goto estado_e;
    if(!sda && scl) goto estado_f;
    goto estado_d;
  estado_e:
    if (sw4 == 0) goto estado_g;
    if(sda && scl) goto estado_e;
    if(!scl)
    {
      x <<= 1;
      x++;
      nbits++;
      if(nbits == 9)
      {
        buffer[contador++] = hhex(x/2);
        buffer[contador++] = lhex(x/2);
        if(x%2) buffer[contador++] = '-'; else buffer[contador++] = '+';
        x = 0;
        nbits = 0;
        if(contador == 3990) goto estado_g;
      }
      goto estado_d;
    }
    if(!sda && scl) 
    {
      buffer[contador++] = '#';
      x = 0;
      nbits = 0;
      if(contador == 4000) goto estado_g;
      goto estado_c;
    }
    goto estado_e;
  estado_f:
    if (sw4 == 0) goto estado_g;
    if(!sda && scl) goto estado_f;
    if(!scl)
    {
      x <<= 1;
      nbits++;
      if(nbits == 9)
      {
        buffer[contador++] = hhex(x/2);
        buffer[contador++] = lhex(x/2);
        if(x%2) buffer[contador++] = '-'; else buffer[contador++] = '+';
        x = 0;
        nbits = 0;
        if(contador == 3990) goto estado_g;
      }
      goto estado_d;
    }
    if(sda && scl)
    {
      buffer[contador++] = '.';
      x = 0;
      nbits = 0;
      if(contador == 4000) goto estado_g;
      goto estado_b;
    }
    goto estado_f;
  estado_g:
    sending();
}

unsigned char hhex(unsigned char v)
{
  return hexadecimal[(v & 0xf0) >> 4];
}

unsigned char lhex(unsigned char v)
{
  return hexadecimal[(v & 0x0f)];
}

void sending(void)
{
  unsigned int i;
  unsigned char c;
  char str[20];
  
  i = 0;
  strcpy(str, "Enviando...");
  while(str[i] != '\0') send_serial(str[i++]);

  i = 0;
  itoa(contador, str, 10);
  while(str[i] != '\0') send_serial(str[i++]);

  i = 0;
  strcpy(str, "\r\n");
  while(str[i] != '\0') send_serial(str[i++]);
  

  for(i=0; i<contador; i++)
  {
    c = buffer[i];
    send_serial(buffer[i]);    
    if(c == '.')
    {
      send_serial('\n');
      send_serial('\r');
    }
  }  

  i = 0;
  strcpy(str, "Envio completado...\r\n");
  while(str[i] != '\0') send_serial(str[i++]);
}

void preencher(void)
{
  contador = 28;
  strcpy(buffer, "*B0+F0+AA-.*B1+00+00+78+9C-.");
}

