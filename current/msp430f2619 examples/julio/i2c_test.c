#include <msp430x20x3.h>

#define ACK             0
#define NACK            1

#define verde           port1.out.pin4
#define vermelho	    port1.out.pin5
#define sda_in          port1.in.pin0
#define sda_out         port1.out.pin0
#define scl             port1.out.pin1

#define datain()        P1DIR = 0x32
#define dataout()       P1DIR = 0x33


unsigned char buffer[8];


// ****************************************************************
// ** P R O T O T I P O S                                        **
// ****************************************************************
//
void delayus(unsigned int tempo);
void delayms(unsigned int tempo);

void init_i2c(void);
void start_i2c(void);
void stop_i2c(void);
void send_0(void);
void send_1(void);
unsigned char lebit(void);
unsigned char le_i2c(unsigned char acknowledge);
unsigned char escreve_i2c(unsigned char dado);

void configura_cpu(void);
unsigned char mem_found(unsigned char slave_address);
unsigned char mem_write(unsigned char slave_address, unsigned int address, unsigned char *buf, unsigned char len);
unsigned char mem_read(unsigned char slave_address, unsigned int address, unsigned char *buf, unsigned char len);


int main(void) 
{	
  char i;

  configura_cpu();
  init_i2c();

  verde = 1;
  vermelho = 1;
  delayms(3000);
  verde = 0;
  vermelho = 0;
  delayms(1000);

  if(mem_found(0xa0) == ACK) verde = 1; else vermelho = 1;

  for(i=0; i<5; i++)
  {
    buffer[0] = 0x20;
    buffer[1] = 0xa9;
    buffer[2] = 0x5f;
    buffer[3] = 0x8d;
    if(mem_write(0xa0, 0x0010, buffer, 4) == ACK) verde = 1; else vermelho = 1;
    if(mem_read(0xa0, 0x0010, buffer, 4) == ACK) verde = 1; else vermelho = 1;

    buffer[0] = 0x11;
    buffer[1] = 0x5e;
    buffer[2] = 0xcc;
    buffer[3] = 0xbd;
    if(mem_write(0xa0, 0x0014, buffer, 4) == ACK) verde = 1; else vermelho = 1;
    if(mem_read(0xa0, 0x0014, buffer, 4) == ACK) verde = 1; else vermelho = 1;
  }

  while(1) 
  {
  }
  
  return 0;
}

void configura_cpu(void)
{
  WDTCTL = WDTPW + WDTHOLD;                // Stop Watchdog Timer

  BCSCTL1 = CALBC1_8MHZ;			       // DCO em 8MHz...
  DCOCTL = CALDCO_8MHZ;
  BCSCTL2 = 0;
  BCSCTL3 = 0;

  dataout();
}

// T = 3u * tempo (aproximadamente)
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
	
	for(i = 0; i < tempo; i++) delayus(800);
}


// Primitivas do I2C...
//
void init_i2c(void)
{
  dataout();
  sda_out = 1;
  scl = 1;
  delayus(4);
}

void start_i2c(void)
{	
  sda_out = 0;
  delayus(4);
  scl = 0;
  delayus(4);
}

void stop_i2c(void)
{
  scl = 1;
  delayus(4);
  sda_out = 1;
  delayus(4);
}

void send_0(void)
{
  sda_out = 0;
  delayus(2);
  scl = 1;
  delayus(4);
  scl = 0;
  delayus(2);
}

void send_1(void)
{
  sda_out = 1;
  delayus(2);
  scl = 1;
  delayus(4);
  scl = 0;
  delayus(2);
  sda_out = 0;
  delayus(2);
}

unsigned char lebit(void)
{
  unsigned char v;

  scl = 1;
  delayus(2);
  if(sda_in == 1) v = 1; else v = 0;  
  delayus(2);
  scl = 0;
  delayus(2);

  return v;
}

unsigned char le_i2c(unsigned char acknowledge)
{
  unsigned char i, x = 0;

  datain();
  for(i=0; i<8; i++)
  {
    x <<= 1;
    if(lebit()) x ++;
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
  sda_out = 0;

  return i;
}

// Primitivas do Dispositivo I2C
//
unsigned char mem_found(unsigned char slave_address)
{
  unsigned char v;

  start_i2c();
  v = escreve_i2c(slave_address);  
  stop_i2c();

  return v;
}

unsigned char mem_write(unsigned char slave_address, unsigned int address, unsigned char *buf, unsigned char len)
{
  unsigned char i, v=0;

  start_i2c();
  v = escreve_i2c(slave_address);
  v += escreve_i2c(address/256);
  v += escreve_i2c(address%256);
  for(i=0; i<len; i++) v += escreve_i2c(*(buf+i));
  stop_i2c();
  delayms(10);
  
  return v;
}

unsigned char mem_read(unsigned char slave_address, unsigned int address, unsigned char *buf, unsigned char len)
{
  unsigned char i, v=0;

  start_i2c();
  v = escreve_i2c(slave_address);
  v += escreve_i2c(address/256);
  v += escreve_i2c(address%256);
  stop_i2c();
  start_i2c();
  v = escreve_i2c(slave_address+1);
  for(i=0; i<len-1; i++) *(buf+i) = le_i2c(ACK);
  *(buf+i) = le_i2c(NACK);
  stop_i2c();

  return v;
}



