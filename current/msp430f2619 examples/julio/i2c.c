//
// ************************************************************************
// **                                                                    **
// **  BIBLIOTECA DE I2C                                                 **
// **                                                                    **
// **                                                                    **
// **                                                                    **
// ************************************************************************
//
#include <msp430x2619.h>
#include "delay.h"
#include "i2c.h"

#define sda_in          port3.in.pin1
#define sda_out         port3.out.pin1
#define scl             port3.out.pin2

#define SDA_OUT()       P3DIR = 0xd7    
#define SDA_IN()        P3DIR = 0xd5    


static union
{
  unsigned int addr;
  struct
  {
    unsigned char lo;
    unsigned char hi;
  } byte;
} convert;


void i2c_init(void)
{
  SDA_OUT();
  sda_out = 1;
  scl = 1;
  delay5us();
}

void i2c_start(void)
{
  SDA_OUT();

  sda_out = 1;
  delay5us();
  scl = 1;
  delay5us();

  sda_out = 0;
  delay5us();
  scl = 0;
  delay5us();
}

void i2c_stop(void)
{
  SDA_OUT();

  sda_out = 0;
  delay5us();
  scl = 1;
  delay5us();
  sda_out = 1;
  delay5us();
}

unsigned char i2c_write(unsigned char b)
{
  unsigned char mask = 0x80;
  unsigned char i;

  SDA_OUT();

  for (i=0; i<8; i++)
  {
    if((b & mask) > 0) sda_out = 1; else sda_out = 0;
    delay5us();
    scl = 1;
    delay5us();
    sda_out = 0;
    delay5us();
    mask >>= 1;
  }
 
  SDA_IN();
  delay5us();
  scl = 1;
  delay2us();
  if(sda_in == 1) i = 0xff; else i = 0;
  delay2us();
  scl = 0;
  delay5us();
  SDA_OUT();

  return i;
}

unsigned char i2c_read(unsigned char confirmation)
{
  unsigned char i;
  unsigned char bin = 0;

  SDA_IN();
  for (i=0; i<8; i++)
  {
    bin <<= 1;
    scl = 1;
    delay2us();
    if (sda_in == 1) bin += 1;
    delay2us();
    scl = 0;
    delay5us();
  }

  SDA_OUT();
  if(confirmation == 0) sda_out = 0; else sda_out = 1;
  scl = 1;
  delay5us();
  scl = 0;
  delay5us();
  sda_out = 0;

  return bin;
}


unsigned char hi(unsigned int addr)
{
  convert.addr = addr;
  return convert.byte.hi;
}

unsigned char lo(unsigned int addr)
{
  convert.addr = addr;
  return convert.byte.lo;
}

unsigned char mem_found(unsigned char memoria)
{
  unsigned char resp;

  i2c_start();
  resp = i2c_write(0xa0);
  i2c_stop();
  if(resp) return 0x00; else return 0xff;
}

unsigned char mem_bytewrite(unsigned char memoria, unsigned int addr, unsigned char dat)
{
  unsigned char h, l;

  h = hi(addr);
  l = lo(addr);

  i2c_start();
  if(i2c_write(memoria))
  {
    i2c_stop();
    return 0x00;
  }
  if(i2c_write(h))
  {
    i2c_stop();
    return 0x00;
  }  
  if(i2c_write(l))
  {
    i2c_stop();
    return 0x00;
  }  
  if(i2c_write(dat))
  {
    i2c_stop();
    return 0x00;
  }  
  i2c_stop();
  delayms(5);
  return 0xff;
}

unsigned char mem_blockwrite(unsigned char memoria, unsigned int addr, unsigned char *p, unsigned char n)
{
  unsigned char c, i, h, l;

  h = hi(addr);
  l = lo(addr);

  i2c_start();
  if(i2c_write(memoria))
  {
    i2c_stop();
    return 0x00;
  }
  if(i2c_write(h))
  {
    i2c_stop();
    return 0x00;
  }  
  if(i2c_write(l))
  {
    i2c_stop();
    return 0x00;
  }  
  // Block write...
  for(i=0; i<n; i++)
  {
    c = *(p+i);
    if(i2c_write(c))
    {
      i2c_stop();
      return 0x00;
    }
  }  
  i2c_stop();
  delayms(5);
  return 0xff;
}

unsigned char mem_byteread(unsigned char memoria, unsigned int addr, unsigned char * dat)
{
  unsigned char h, l;

  h = hi(addr);
  l = lo(addr);

  i2c_start();
  if(i2c_write(memoria))
  {
    i2c_stop();
    return 0x00;
  }
  if(i2c_write(h))
  {
    i2c_stop();
    return 0x00;
  }  
  if(i2c_write(l))
  {
    i2c_stop();
    return 0x00;
  }  
  i2c_start();
  // Muda de Slave Write para Slave Read...
  if(i2c_write(memoria+1))
  {
    i2c_stop();
    return 0x00;
  }
  // Le um byte e envia um NAK...
  *dat = i2c_read(0xff);  
  i2c_stop();
  return 0xff;
}

unsigned char mem_blockread(unsigned char memoria, unsigned int addr, unsigned char *p, unsigned char n)
{
  unsigned char i, h, l;

  h = hi(addr);
  l = lo(addr);

  i2c_start();
  if(i2c_write(memoria))
  {
    i2c_stop();
    return 0x00;
  }
  if(i2c_write(h))
  {
    i2c_stop();
    return 0x00;
  }  
  if(i2c_write(l))
  {
    i2c_stop();
    return 0x00;
  }  
  i2c_start();
  // Muda de Slave Write para Slave Read...
  if(i2c_write(memoria+1))
  {
    i2c_stop();
    return 0x00;
  }
  // Le n-1 bytes e envia AK...
  for(i=0; i<n-1; i++) *(p+i) = i2c_read(0x00);
  // Le n_�simo byte e envia um NAK...
  *(p+n-1) = i2c_read(0xff);  
  i2c_stop();
  return 0xff;
}




