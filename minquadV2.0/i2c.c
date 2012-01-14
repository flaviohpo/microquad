//
// ****************************************************************
// **    2                                                       **
// **  I   C                                                     **
// **                                    (C) by JCLima, 2010/1   **
// **                                                            **
// ****************************************************************
//
#include "delay.h"
#include "msp430f2618.h"
#include "i2c.h"

//
// ****************************************************************
// ** V A R I A V E I S                                          **
// ****************************************************************
//
 char i2c_slave_address = 0x3a;    

//
// ****************************************************************
// ** P R I M I T I V A S                                        **
// ****************************************************************
//
void i2c_config( char slave_addr)
{
  i2c_init();
  i2c_slave_address = slave_addr;
}

void i2c_change_address( char slave_addr)
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

 char i2c_recv_ack(void)
{
   char i;

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

void i2c_send_ack( char confirmation)
{
  SDA_OUT();
  if(confirmation) sda_on; else sda_off;
  scl_on;
  delay5us();
  scl_off;
  delay5us();
  sda_off;
}

void i2c_bwrite( char b)
{
   char mask = 0x80;
   char i;

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

 char i2c_bread(void)
{
   char i;
   char bin = 0;

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
 char i2c_find_device(void)
{
   char status = 0;

  i2c_start();
  i2c_bwrite(i2c_slave_address);
  status = i2c_recv_ack();
  i2c_stop();

  return status;
}

 char i2c_write_dummy( char b)
{
   char status = 0;

  i2c_start();
  i2c_bwrite(b);
  status = i2c_recv_ack();
  i2c_stop();

  return status;
}

 char i2c_write_byte( char reg,  char dat)
{
   char status = 0;

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

char i2c_write_multiples( char reg,  char dat[],  int len)
{
  int i = 0, status = 0;

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

 char i2c_write16_byte( int reg,  char dat)
{
  char status = 0;

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

 char i2c_write16_multiples( int reg,  char dat[], int len)
{
   int i = 0, status = 0;

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

 char i2c_read_byte( char reg,  char *dat)
{
   char status = 0;

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

 char i2c_read_multiples( char reg,  char dat[], int len)
{
   int i = 0, status = 0;

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

 char i2c_read16_byte( int reg,  char *dat)
{
   char status = 0;

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

 char i2c_read16_multiples( int reg, char dat[], int len)
{
   int i = 0, status = 0;

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









