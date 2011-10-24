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
unsigned char databuffer[6];


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

void init_i2c(void);
void start_i2c(void);
void stop_i2c(void);
void send_0(void);
void send_1(void);
unsigned char lebit(void);
unsigned char le_i2c(unsigned char acknowledge);
unsigned char escreve_i2c(unsigned char dado);

unsigned char mem_found(unsigned char address);


//unsigned char nunchuck_find(unsigned char address);
//unsigned char nunchuck_init(void);
//unsigned char nunchuck_getdata(void);
//unsigned char nunchuk_decode(unsigned char x);


// M A I N
//
int main (void)
{
//  unsigned char i, buff[8];

  WDTCTL = WDTPW + WDTHOLD; 
  configura_cpu();
  lcd_initmodule(nao_virado);
  config_serial(baud_115200, mod_115200);
  init_i2c();
  
  aonde = no_lcd;
  printf("Searching...\n\r");
  if(mem_found(0xa0) == ACK) printf("Found memory\n\r"); else printf("Do not found...\n\r");

  while(1)
  {
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
  P3DIR = 0xd7;  
  P3SEL = 0x30;
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



// Primitivas do I2C...
//
void init_i2c(void)
{
  dataout();
  sda_out = 1;
  scl = 1;
}

void start_i2c(void)
{   
  sda_out = 1;
  delayus(7);
  scl = 1;
  delayus(17);
  sda_out = 0;
  delayus(17);
  scl = 0;
  delayus(7);
}

void stop_i2c(void)
{
  sda_out = 0;
  delayus(7);
  scl = 1;
  delayus(17);
  sda_out = 1;
  delayus(17);
}

void send_0(void)
{
  sda_out = 0;
  delayus(7);
  scl = 1;
  delayus(17);
  scl = 0;
  delayus(7);
}

void send_1(void)
{
  sda_out = 1;
  delayus(7);
  scl = 1;
  delayus(17);
  scl = 0;
  delayus(7);
  sda_out = 0;
  delayus(7);
}

unsigned char lebit(void)
{
  unsigned char v;

  scl = 1;
  delayus(7);
  if(sda_in == 1) v = 1; else v = 0;  
  delayus(7);
  scl = 0;
  delayus(7);

  return v;
}

unsigned char le_i2c(unsigned char acknowledge)
{
  unsigned char i, x = 0, mask = 0x80;

  datain();
  for(i=0; i<8; i++)
  {
    if(lebit()) x += mask;
    mask >>= 1;
  }
  dataout();
  if(acknowledge == ACK) send_0(); else send_1();

  return x;
}

unsigned char escreve_i2c(unsigned char dado)
{
  unsigned char i, mask = 0x80;
    
  for(i=0; i<8; i++)
  {
    if ((dado & mask) > 0) send_1(); else send_0();
    mask >>= 1;
  }
  datain();
  i = lebit();
  dataout();

  return i;
}

// Primitivas do Dispositivo I2C
//
unsigned char mem_found(unsigned char address)
{
  unsigned char v;

  start_i2c();
  v = escreve_i2c(address);  
  stop_i2c();

  return v;
}







/*
// Primitivas de I2C...
//
void config_i2c(unsigned char slave_add)
{
  // Configurando a porta I2C...
  //
  UCB0CTL1 |= UCSWRST;
  UCB0CTL0 = 0x0f;
  UCB0CTL1 = 0x81;
  UCB0BR0 = 160;
  UCB0BR1 = 0;
  UCB0I2CSA = slave_add/2;
  UCB0CTL1 = 0x80;

  // Aguardando o stop condition...
  //
  while(UCB0CTL1 & UCTXSTP);  
}

unsigned char write_i2c(unsigned char *dado, unsigned char length)
{
  unsigned char i, status = 0;

  // Start i2c...(em tx mode!!!)
  //
  UCB0CTL1 |= UCTR + UCTXSTT;
  while(UCB0CTL1 & UCTXSTT);

  // Testa a ocorrencia de ACK/NACK do slave address
  //
  if(UCB0STAT & UCNACKIFG) status++;

  // Envia a sequencia de caracteres...
  //
  for(i=0; i<length; i++)
  {  
    UCB0TXBUF = *(dado+i);

    // Aguarda enviar o caracter...
    //
    while(!(IFG2 & UCB0TXIFG)); 

    nop();
    nop();

    // Testa a ocorrencia de ACK/NACK dos dados...
    //
    if(UCB0STAT & UCNACKIFG) status++;
  }

  // Stop i2c...
  //
  UCB0CTL1 |= UCTXSTP;
  while(UCB0CTL1 & UCTXSTP);

  return status;
}
*/


/*
// Primitivas nunchuck
//
unsigned char nunchuck_find(unsigned char address)
{
  unsigned char v;

  start_i2c();
  v = escreve_i2c(address);  
  stop_i2c();

  return v;
}

unsigned char nunchuck_init(void)
{
  unsigned char i, v = 0, buff1[8], buff2[8], databuff[6];

  start_i2c();
  v += escreve_i2c(0xa4);  
  v += escreve_i2c(0xf0);  
  v += escreve_i2c(0x55);  
  stop_i2c();

  start_i2c();
  v += escreve_i2c(0xa4);  
  v += escreve_i2c(0xfb);  
  v += escreve_i2c(0x00);  
  stop_i2c();

  start_i2c();
  v += escreve_i2c(0xa4);  
  v += escreve_i2c(0xfa);  
  stop_i2c();

  start_i2c();
  v += escreve_i2c(0xa5);  
  for(i=0; i<6; i++) databuff[i] = le_i2c(ACK);
  stop_i2c();

  start_i2c();
  v += escreve_i2c(0xa4);  
  v += escreve_i2c(0xf0);  
  v += escreve_i2c(0xaa);  
  stop_i2c();
  
  start_i2c();
  v += escreve_i2c(0xa4);  
  v += escreve_i2c(0x20);  
  stop_i2c();

  start_i2c();
  v += escreve_i2c(0xa5);  
  for(i=0; i<8; i++) buff1[i] = le_i2c(ACK);
  stop_i2c();

  start_i2c();
  v += escreve_i2c(0xa5);  
  for(i=0; i<8; i++) buff2[i] = le_i2c(ACK);
  stop_i2c();

  start_i2c();
  v += escreve_i2c(0xa4);  
  v += escreve_i2c(0x30);  
  stop_i2c();

  start_i2c();
  v += escreve_i2c(0xa5);  
  for(i=0; i<8; i++) buff1[i] = le_i2c(ACK);
  stop_i2c();

  start_i2c();
  v += escreve_i2c(0xa5);  
  for(i=0; i<8; i++) buff2[i] = le_i2c(ACK);
  stop_i2c();

  return v;
}

unsigned char nunchuck_getdata(void)
{
  unsigned char i, v = 0;

  start_i2c();
  v += escreve_i2c(0xa4);  
  v += escreve_i2c(0x00);  
  stop_i2c();

  start_i2c();
  v += escreve_i2c(0xa5);  
  for(i=0; i<6; i++) databuffer[i] = nunchuk_decode(le_i2c(ACK));
  stop_i2c();

  return v;
}

unsigned char nunchuk_decode(unsigned char x)
{
  x = (x ^ 0x17) + 0x17;
  return x;
}
*/
