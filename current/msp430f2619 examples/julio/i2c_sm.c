//
// ****************************************************************
// **    2                                                       **
// **  I   C                                                     **
// **                                    (C) by JCLima, 2010/1   **
// **                                                            **
// ****************************************************************
//
#include <msp430x2619.h>

#define             speed_100   160     // @16MHz --> 100KHz (Standard ou Slow Mode)

#define             ACK         0
#define             NACK        0xff


// Protótipos...
//
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

unsigned char i2c_find_device(unsigned char address);



// Declaração de pinos...
//
#define sda_in      port3.in.pin1
#define sda_out     port3.out.pin1
#define scl         port3.out.pin2

#define dataout()   P3DIR = 0xd7
#define datain()    P3DIR = 0xd5


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

unsigned char i2c_find_device(unsigned char address)
{
  i2c_change_slaveaddress(address);
  return i2c_write_dummy();
}


