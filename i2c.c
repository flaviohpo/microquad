//
// ****************************************************************
// **    2                                                       **
// **  I   C                                                     **
// **                                    (C) by JCLima, 2010/1   **
// **                                                            **
// ****************************************************************
//

#include "i2c.h"
#include "msp430x261x.h"
#include "delay.h"
//
// ****************************************************************
// ** V A R I A V E I S                                          **
// ****************************************************************
//
unsigned char i2c_slave_address = 0x3a;    

//
// ****************************************************************
// ** P R I M I T I V A S                                        **
// ****************************************************************
//
void i2c_config(unsigned char slave_addr)
{
  i2c_init();
  i2c_slave_address = slave_addr;
}

void i2c_change_address(unsigned char slave_addr)
{
  i2c_slave_address = slave_addr;
}

void i2c_init(void)
{
  SDA_OUT();
  sda_on;
  scl_on;
  delay5us();
}

void i2c_start(void)
{
  sda_on;
  delay5us();
  scl_on;
  delay5us();

  sda_off;
  delay5us();
  scl_off;
  delay5us();
}

void i2c_stop(void)
{
  sda_off;
  delay5us();
  scl_on;
  delay5us();
  sda_on;
  delay5us();
}

unsigned char i2c_recv_ack(void)
{
  unsigned char i;

  SDA_IN();
  delay5us();
  scl_on;
  delay5us();
  if(sda_test) i=1; else i=0;
  delay5us();
  scl_off;
  delay5us();
  SDA_OUT();

  return i;
}

void i2c_send_ack(unsigned char confirmation)
{
  SDA_OUT();
  if(confirmation) sda_on; else sda_off;
  scl_on;
  delay5us();
  scl_off;
  delay5us();
  sda_off;
}

void i2c_bwrite(unsigned char b)
{
  unsigned char mask = 0x80;
  unsigned char i;

  SDA_OUT();
  for (i=0; i<8; i++)
  {
    if(b & mask) sda_on; else sda_off;
    delay5us();
    scl_on;
    delay5us();
    scl_off;
    delay5us();
    mask >>= 1;
  }
}

unsigned char i2c_bread(void)
{
  unsigned char i;
  unsigned char bin = 0;

  SDA_IN();
  for (i=0; i<8; i++)
  {
    bin <<= 1;
    scl_on;
    delay5us();
    if (sda_test) bin += 1;
    delay5us();
    scl_off;
    delay5us();
  }

  return bin;
}

//
// ****************************************************************
// ** F U N Ç Õ E S  D E R I V A D A S                           **
// ****************************************************************
//
unsigned char i2c_find_device(void)
{
  unsigned char status = 0;

  i2c_start();
  i2c_bwrite(i2c_slave_address);
  status = i2c_recv_ack();
  i2c_stop();

  return status;
}

unsigned char i2c_write_dummy(unsigned char b)
{
  unsigned char status = 0;

  i2c_start();
  i2c_bwrite(b);
  status = i2c_recv_ack();
  i2c_stop();

  return status;
}

unsigned char i2c_write_byte(unsigned char reg, unsigned char dat)
{
  unsigned char status = 0;

  i2c_start();
  i2c_bwrite(i2c_slave_address);
  status += i2c_recv_ack();
  i2c_bwrite(reg);
  status += i2c_recv_ack();
  i2c_bwrite(dat);
  status += i2c_recv_ack();
  i2c_stop();

  return status;
}

unsigned char i2c_write_multiples(unsigned char reg, unsigned char dat[], unsigned char len)
{
  unsigned char i = 0, status = 0;

  i2c_start();
  i2c_bwrite(i2c_slave_address);
  status += i2c_recv_ack();
  i2c_bwrite(reg);
  status += i2c_recv_ack();
  while((!status) && (i<len))
  {
    i2c_bwrite(dat[i++]);
    status += i2c_recv_ack();
  }
  i2c_stop();


  return status;
}

unsigned char i2c_write16_byte(unsigned int reg, unsigned char dat)
{
  unsigned char status = 0;

  i2c_start();
  i2c_bwrite(i2c_slave_address);
  status += i2c_recv_ack();
  i2c_bwrite(reg/256);
  status += i2c_recv_ack();
  i2c_bwrite(reg%256);
  status += i2c_recv_ack();
  i2c_bwrite(dat);
  status += i2c_recv_ack();
  i2c_stop();

  return status;
}

unsigned char i2c_write16_multiples(unsigned int reg, unsigned char dat[], unsigned char len)
{
  unsigned char i = 0, status = 0;

  i2c_start();
  i2c_bwrite(i2c_slave_address);
  status += i2c_recv_ack();
  i2c_bwrite(reg/256);
  status += i2c_recv_ack();
  i2c_bwrite(reg%256);
  status += i2c_recv_ack();
  while((!status) && (i<len))
  {
    i2c_bwrite(dat[i++]);
    status += i2c_recv_ack();
  }
  i2c_stop();


  return status;
}

unsigned char i2c_read_byte(unsigned char reg, unsigned char *dat)
{
  unsigned char status = 0;

  i2c_start();
  i2c_bwrite(i2c_slave_address);
  status += i2c_recv_ack();
  i2c_bwrite(reg);
  status += i2c_recv_ack();
  i2c_start();
  i2c_bwrite(i2c_slave_address+1);
  status += i2c_recv_ack();
  *dat = i2c_bread();
  i2c_send_ack(NACK);
  i2c_stop();

  return status;
}

unsigned char i2c_read_multiples(unsigned char reg, unsigned char dat[], unsigned char len)
{
  unsigned char i = 0, status = 0;

  i2c_start();
  i2c_bwrite(i2c_slave_address);
  status += i2c_recv_ack();
  i2c_bwrite(reg);
  status += i2c_recv_ack();
  i2c_start();
  i2c_bwrite(i2c_slave_address+1);
  status += i2c_recv_ack();

  while((!status) && (i<len-1))
  {  
    dat[i] = i2c_bread();
    i2c_send_ack(ACK);
    i++;
  }
  dat[i] = i2c_bread();
  i2c_send_ack(NACK);
  i2c_stop();

  return status;
}

unsigned char i2c_read16_byte(unsigned int reg, unsigned char *dat)
{
  unsigned char status = 0;

  i2c_start();
  i2c_bwrite(i2c_slave_address);
  status += i2c_recv_ack();
  i2c_bwrite(reg/256);
  status += i2c_recv_ack();
  i2c_bwrite(reg%256);
  status += i2c_recv_ack();
  i2c_start();
  i2c_bwrite(i2c_slave_address+1);
  status += i2c_recv_ack();
  *dat = i2c_bread();
  i2c_send_ack(NACK);
  i2c_stop();

  return status;
}

unsigned char i2c_read16_multiples(unsigned int reg, unsigned char dat[], unsigned char len)
{
  unsigned char i = 0, status = 0;

  i2c_start();
  i2c_bwrite(i2c_slave_address);
  status += i2c_recv_ack();
  i2c_bwrite(reg/256);
  status += i2c_recv_ack();
  i2c_bwrite(reg%256);
  status += i2c_recv_ack();
  i2c_start();
  i2c_bwrite(i2c_slave_address+1);
  status += i2c_recv_ack();

  while((!status) && (i<len-1))
  {  
    dat[i] = i2c_bread();
    i2c_send_ack(ACK);
    i++;
  }
  dat[i] = i2c_bread();
  i2c_send_ack(NACK);
  i2c_stop();

  return status;
}









