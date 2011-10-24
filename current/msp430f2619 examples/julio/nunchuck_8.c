#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <msp430x2619.h>
#include "lcd2112.h"
#include "uart0.h"

// Vermelho = VCC
// 
//
//


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
#define             no_lcd      0
#define             na_serial   1

#define             DS1338      0xd0
#define             TMP275      0x90
#define             MEM_24LC    0xa2
#define             SMART_CARD  0xa0
#define             NUNCHUCK    0xa4

#define             speed_400   40      // @16MHz --> 400KHz (Fast Mode)
#define             speed_100   160     // @16MHz --> 100KHz (Standard ou Slow Mode)

#define             ACK         0


// Constantes e Variáveis...
//
unsigned char aonde = no_lcd;


// Protótipos...
//
int putchar(int c);

void cpu_config(void);
void delayus(unsigned int tempo);
void delayms(unsigned int tempo);

unsigned char readkeyboard(void);
unsigned char waitpress(void);
void waitrelease(void);
void waitkey(unsigned char key);
unsigned char waitanykey(void);

void i2c_config(unsigned char slave_add, unsigned char speed);
void i2c_change_slaveaddress(unsigned char slave_add);
void i2c_start_tx(void);
void i2c_start_rx(void);
void i2c_stop_tx(void);
void i2c_stop_rx(void);
unsigned char i2c_test(void);
unsigned char i2c_send(unsigned char b);

unsigned char i2c_write_dummy(void);
unsigned char i2c_write_one(unsigned char b1);
unsigned char i2c_write_one_data(unsigned char b1, unsigned char dat);
unsigned char i2c_write_one_buffer(unsigned char b1, unsigned char dat[], unsigned char len);
unsigned char i2c_write_two(unsigned char b1, unsigned char b2);
unsigned char i2c_write_two_data(unsigned char b1, unsigned char b2, unsigned char dat);
unsigned char i2c_write_two_buffer(unsigned char b1, unsigned char b2, unsigned char dat[], unsigned char len);
unsigned char i2c_write_buffer(unsigned char dat[], unsigned char len);

unsigned char i2c_wread_one_data(unsigned char b1, unsigned char *dat);
unsigned char i2c_wread_one_buffer(unsigned char b1, unsigned char dat[], unsigned char len);
unsigned char i2c_wread_two_data(unsigned char b1, unsigned char b2, unsigned char *dat);
unsigned char i2c_wread_two_buffer(unsigned char b1, unsigned char b2, unsigned char dat[], unsigned char len);

unsigned char i2c_read_data(unsigned char *dat);
unsigned char i2c_read_buffer(unsigned char dat[], unsigned char len);

unsigned char find_device(unsigned char address);

void nunchuck_decode(unsigned char dat[6]);
unsigned char nunchuck_init(void);
unsigned char nunchuck_getdata(void);


// M A I N
//
int main (void)
{
  unsigned char i, buff[80], buff_rx[80];

  WDTCTL = WDTPW + WDTHOLD; 
  cpu_config();
  lcd_initmodule(nao_virado);
  config_serial(baud_115200, mod_115200);
  i2c_config(NUNCHUCK, speed_100);

//  printf("Smart Card Drive\r\n");
//  if(i2c_write_dummy() == ACK)
//  {
//    printf("Smart Card ok\r\n");
//    strcpy(buff, "JuLIO CEsar M");
//    i2c_write_one_buffer(0x00, buff, 13);
//    delayms(10);
//    printf("Escrita ok...\r\n");
//    i2c_wread_one_buffer(0x00, buff_rx, 13);
//    printf("Leitura ok...\r\n");
//    for(i=0; i<13; i++) printf("%c", buff_rx[i]);  
//  } else printf("Smart Card nok");
//  delayms(2000);
//  lcd_clrscr();

  printf("NUNCHUCK driver\r\n");
  delayms(1000);
  lcd_goto(0,0);
  if(find_device(0xa4) == ACK)
  {
    printf("Achei o NUNCHUCK\r\n");
    nunchuck_init();
  } else printf("Nao Achei...    \r\n");

  while(1)
  {
    nunchuck_getdata();
    delayms(500);
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
void cpu_config(void)
{
  // configura o clock para 16MHz...
  DCOCTL = 0x77;
  BCSCTL1 = 0x0f;
  BCSCTL2 = 0x08;
  BCSCTL3 = 0x8c;

  // Configura as portas...
  P1DIR = 0xf0;
  P2DIR = 0x00;
  P3DIR = 0xd5;  
  P3SEL = 0x36;
  P4DIR = 0xff;
  P5DIR = 0x70;
  P5SEL = 0x70;

  // Apaga o led...
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


// ** Primitivas do i2c *****************
// **                                  **
// **************************************


// Configurando a porta I2C...
//
void i2c_config(unsigned char slave_add, unsigned char speed)
{
  // Reseta UCSI e configura registradores...
  UCB0CTL1 = 0xc1;
  UCB0CTL0 = 0x0f;

  // Ajustando velocidade...  
  UCB0BR0 = speed;
  UCB0BR1 = 0;

  // Configurando Slave Address...
  UCB0I2CSA = (slave_add >> 1);
  UCB0CTL1 = 0xc0;

  // Forçando Stop Condition... (TX Mode)
  UCB0CTL1 |= UCTR+UCTXSTP;
  while(!(UCB0CTL1 & UCTXSTP));  
}

void i2c_change_slaveaddress(unsigned char slave_add)
{
  UCB0I2CSA = (slave_add >> 1);
}

// Forçando Start Condition... (TX mode)
// Escreve slave address(w)
//
void i2c_start_tx(void)
{
  UCB0CTL1 |= UCTR+UCTXSTT;
  while(!(UCB0CTL1 & UCTXSTT));
}

// Forçando Start Condition... (RX mode)
// Escreve slave address(r)
//
void i2c_start_rx(void)
{
  UCB0CTL1 &= ~UCTR;
  UCB0CTL1 |= UCTXSTT;
  while(!(UCB0CTL1 & UCTXSTT));
}

// Forçando Stop Condition... 
//
void i2c_stop_tx(void)
{
  UCB0CTL1 |= UCTR+UCTXSTP;
  while(!(UCB0CTL1 & UCTXSTP));
}

// Forçando Stop Condition... (RX mode)
//
void i2c_stop_rx(void)
{
  UCB0CTL1 &= ~UCTR;
  UCB0CTL1 |= UCTXSTP;
  while(!(UCB0CTL1 & UCTXSTP));
}

// Testa condição de ack
//
unsigned char i2c_test(void)
{
  while(UCB0STAT & UCBBUSY); 
  return (UCB0STAT & UCNACKIFG);
}

// Envia um byte pela porta i2c...
//
unsigned char i2c_send(unsigned char b)
{
  UCB0TXBUF = b;
  // Aguarda até enviar o caracter...
  while(!(IFG2 & UCB0TXIFG)); 
  // Retorna a ocorrencia de ACK/NACK do envio...
  return (UCB0STAT & UCNACKIFG);
}

unsigned char i2c_write_dummy(void)
{
  unsigned char status = 0;

  i2c_start_tx();
  i2c_stop_tx();
  status += i2c_test();

  return status;
}

unsigned char i2c_write_one(unsigned char b1)
{
  unsigned char status = 0;

  i2c_start_tx();
  status += i2c_send(b1);
  i2c_stop_tx();
  status += i2c_test();

  return status;
}

unsigned char i2c_write_one_data(unsigned char b1, unsigned char dat)
{
  unsigned char status = 0;

  i2c_start_tx();
  status += i2c_send(b1);
  status += i2c_send(dat);
  i2c_stop_tx();
  status += i2c_test();

  return status;
}

unsigned char i2c_write_one_buffer(unsigned char b1, unsigned char dat[], unsigned char len)
{
  unsigned char i = 0, status = 0;

  i2c_start_tx();
  status += i2c_send(b1);
  while((!status) && (i<len)) status += i2c_send(dat[i++]);
  i2c_stop_tx();
  status += i2c_test();

  return status;
}

unsigned char i2c_write_two(unsigned char b1, unsigned char b2)
{
  unsigned char status = 0;

  i2c_start_tx();
  status += i2c_send(b1);
  status += i2c_send(b2);
  i2c_stop_tx();
  status += i2c_test();

  return status;
}

unsigned char i2c_write_two_data(unsigned char b1, unsigned char b2, unsigned char dat)
{
  unsigned char status = 0;

  i2c_start_tx();
  status += i2c_send(b1);
  status += i2c_send(b2);
  status += i2c_send(dat);
  i2c_stop_tx();
  status += i2c_test();

  return status;
}

unsigned char i2c_write_two_buffer(unsigned char b1, unsigned char b2, unsigned char dat[], unsigned char len)
{
  unsigned char i = 0, status = 0;

  i2c_start_tx();
  status += i2c_send(b1);
  status += i2c_send(b2);
  while((!status) && (i<len)) status += i2c_send(dat[i++]);
  i2c_stop_tx();
  status += i2c_test();

  return status;
}

unsigned char i2c_write_buffer(unsigned char dat[], unsigned char len)
{
  unsigned char i = 0, status = 0;

  i2c_start_tx();
  while((!status) && (i<len)) status += i2c_send(dat[i++]);
  i2c_stop_tx();
  status += i2c_test();

  return status;
}

unsigned char i2c_read_data(unsigned char *dat)
{
  unsigned char status = 0;

  i2c_start_rx();

  if(UCB0STAT & UCNACKIFG) status++;
  while(!(IFG2 & UCB0RXIFG));  
  *dat = UCB0RXBUF;

  i2c_stop_rx();
  status += i2c_test();

  return status;
}

unsigned char i2c_read_buffer(unsigned char dat[], unsigned char len)
{
  unsigned char i = 0, status = 0;

  i2c_start_rx();

  // Envia a sequencia de caracteres, se houver...
  //
  while((!status) && (i<len))
  {  
    // Testa a ocorrencia de ACK/NACK dos dados...
    if(UCB0STAT & UCNACKIFG) status++;
    // Aguarda até receber o caracter...
    while(!(IFG2 & UCB0RXIFG)); 
    // Le caracter do buffer
    dat[i] = UCB0RXBUF;
    i++;
  }

  i2c_stop_rx();
  status += i2c_test();

  return status;
}


unsigned char i2c_wread_one_data(unsigned char b1, unsigned char *dat)
{
  unsigned char status = 0;

  i2c_start_tx();
  status += i2c_send(b1);
  i2c_start_rx();

  if(UCB0STAT & UCNACKIFG) status++;
  while(!(IFG2 & UCB0RXIFG));  
  *dat = UCB0RXBUF;

  i2c_stop_rx();
  status += i2c_test();

  return status;
}

unsigned char i2c_wread_one_buffer(unsigned char b1, unsigned char dat[], unsigned char len)
{
  unsigned char i = 0, status = 0;

  i2c_start_tx();
  status += i2c_send(b1);
  i2c_start_rx();

  // Envia a sequencia de caracteres, se houver...
  //
  while((!status) && (i<len))
  {  
    // Testa a ocorrencia de ACK/NACK dos dados...
    if(UCB0STAT & UCNACKIFG) status++;
    // Aguarda até receber o caracter...
    while(!(IFG2 & UCB0RXIFG)); 
    // Le caracter do buffer
    dat[i] = UCB0RXBUF;
    i++;
  }

  i2c_stop_rx();
  status += i2c_test();

  return status;
}

unsigned char i2c_wread_two_data(unsigned char b1, unsigned char b2, unsigned char *dat)
{
  unsigned char status = 0;

  i2c_start_tx();
  status += i2c_send(b1);
  status += i2c_send(b2);
  i2c_start_rx();

  if(UCB0STAT & UCNACKIFG) status++;
  while(!(IFG2 & UCB0RXIFG));  
  *dat = UCB0RXBUF;

  i2c_stop_rx();
  status += i2c_test();

  return status;
}

unsigned char i2c_wread_two_buffer(unsigned char b1, unsigned char b2, unsigned char dat[], unsigned char len)
{
  unsigned char i = 0, status = 0;

  i2c_start_tx();
  status += i2c_send(b1);
  status += i2c_send(b2);
  i2c_start_rx();

  // Envia a sequencia de caracteres, se houver...
  //
  while((!status) && (i<len))
  {  
    // Testa a ocorrencia de ACK/NACK dos dados...
    if(UCB0STAT & UCNACKIFG) status++;
    // Aguarda até receber o caracter...
    while(!(IFG2 & UCB0RXIFG)); 
    // Le caracter do buffer
    dat[i] = UCB0RXBUF;
    i++;
  }

  i2c_stop_rx();
  status += i2c_test();

  return status;
}


// Primitivas do Dispositivo i2c
//
unsigned char find_device(unsigned char address)
{
  i2c_change_slaveaddress(address);
  return i2c_write_dummy();
}


void nunchuck_decode(unsigned char dat[6])
{
  unsigned char i;
 
  for(i=0; i<6; i++) dat[i] = (dat[i] ^ 0x17) + 0x17;
}


/*
unsigned char nunchuck_init(void)
{
  i2c_write_one_data(0x40, 0x00);
  return 0;
}
*/


unsigned char nunchuck_init(void)
{
  unsigned char buffer[8], calib[16];

  i2c_write_one_data(0xf0, 0x55);
//  delayms(10);
  i2c_write_one_data(0xfb, 0x00);
//  delayms(10);
  i2c_write_one(0xfa);
//  delayms(10);
  
  // descarta dados...
  i2c_read_buffer(buffer, 6);

  i2c_write_one_data(0xf0, 0xaa);
//  delayms(10);
  i2c_write_one(0x20);
//  delayms(10);
  i2c_read_buffer(calib, 8);
  i2c_read_buffer(calib, 6);
  // imprimir...

  i2c_write_one(0x30);
//  delayms(10);
  i2c_read_buffer(buffer, 8);
  i2c_read_buffer(buffer, 6);
  // imprimir...
  
  return 0;
}


unsigned char nunchuck_getdata(void)
{
  unsigned char i, b1, b2, b3, buffer[8];

  i2c_write_one(0x00);
  i2c_read_buffer(buffer, 7);
//  nunchuck_decode(buffer);

  b3 = buffer[6] & 0x03;
  b1 = buffer[6] & 0x01;
  b2 = (buffer[6] >> 1) & 0x01;

  lcd_goto(0, 1);
  printf("%02X\r\n", buffer[1]);
  printf("%02X\r\n", buffer[2]);
  printf("%02X\r\n", buffer[3]);
  printf("%02X\r\n", buffer[4]);
  printf("%02X\r\n", buffer[5]);
  printf("%02X\r\n", buffer[6]);
  printf("%d:  %d - %d \r\n", b3, b1, b2);

  return 0;
}

