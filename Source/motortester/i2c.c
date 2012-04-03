#include "msp430f2618.h"
#include "i2c.h"

unsigned int timeout = 0;

// Configurando a porta I2C...
//
void i2c_config(unsigned char slave_add){
    // Reseta UCSI e configura registradores...
    UCB0CTL1 = 0xc1;
    UCB0CTL0 = 0x0f;
    
    // Ajustando velocidade...  
    UCB0BR0 = 0xFF;
    UCB0BR1 = 0x00;
    
    P3SEL |= 0x02 | 0x04; // confi pins
    P3DIR &= ~(0x02 | 0x04);
    
    // Configurando Slave Address...
    UCB0I2CSA = (slave_add >> 1);
    UCB0CTL1 = 0xc0;
    
    timeout = 0;
    // Forçando Stop Condition... (TX Mode)
    UCB0CTL1 |= UCTR+UCTXSTP;
    while(!(UCB0CTL1 & UCTXSTP)){
        timeout++;
        if(timeout == 0){
            return 0xFF;
        }
    }
    return 0;
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
unsigned char i2c_start_rx(void){
    
    UCB0CTL1 &= ~UCTR;
    UCB0CTL1 |= UCTXSTT;
    while(!(UCB0CTL1 & UCTXSTT));
}

// Forçando Stop Condition... 
//
unsigned char i2c_stop_tx(void){
    timeout = 0;
    UCB0CTL1 |= UCTR+UCTXSTP;
    while(!(UCB0CTL1 & UCTXSTP)){
        timeout++;
        if(timeout == 0){
            return 0xFF;
        }
    }
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
  while((!status) && (i<len)) 
    status += i2c_send(dat[i++]);
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


