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


const unsigned char mask[8] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01};

unsigned char nunchuck_buf[6];


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
unsigned char mem_found(unsigned char address);

unsigned char inicia_nunchuck(void);
unsigned char nunchuck_init(void);
unsigned char nunchuk_decode_byte (unsigned char x);
unsigned char nunchuck_send_request(void);
unsigned char nunchuck_get_data(void);


int main(void) 
{	
  int i;
  configura_cpu();
  init_i2c();

  verde = 0;
  vermelho = 0;

//  if(nunchuck_init() == ACK) verde = 1; else vermelho = 1;
  if(inicia_nunchuck() == ACK) verde = 1; else vermelho = 1;

  for(i=0; i<20; i++)
  {
    if(nunchuck_get_data() == ACK) verde = 1; else vermelho = 1;
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
}

void start_i2c(void)
{	
  sda_out = 1;
  delayus(2);
  scl = 1;
  delayus(4);
  sda_out = 0;
  delayus(4);
  scl = 0;
  delayus(2);
}

void stop_i2c(void)
{
  sda_out = 0;
  delayus(2);
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
    if(lebit()) x += mask[i];

  dataout();
  if(acknowledge == ACK) send_0(); else send_1();

  return x;
}

unsigned char escreve_i2c(unsigned char dado)
{
  unsigned char i;
	
  for(i=0; i<8; i++)
    if ((dado & mask[i]) > 0) send_1(); else send_0();

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


unsigned char nunchuck_init(void)
{
  unsigned char i = 0;

  start_i2c();
  i += escreve_i2c(0xa4);
  i += escreve_i2c(0x40);
  i += escreve_i2c(0x00);
  stop_i2c();

  return i;
}

unsigned char nunchuck_get_data(void)
{
  unsigned char i = 0, v = 0;

  start_i2c();
  v += escreve_i2c(0xa4);
  v += escreve_i2c(0x00);
  stop_i2c();

  delayms(1);

  start_i2c();
  v += escreve_i2c(0xa5);

  while(i < 5)
  {
    nunchuck_buf[i] = nunchuk_decode_byte (le_i2c(ACK));
    i++;
  }
  nunchuck_buf[5] = nunchuk_decode_byte (le_i2c(NACK));
  stop_i2c();

  return v;
}

unsigned char nunchuck_send_request(void)
{
  unsigned char i = 0;

  start_i2c();
  i += escreve_i2c(0xa4);
  i += escreve_i2c(0x00);
  stop_i2c();

  return i;
}

unsigned char nunchuk_decode_byte (unsigned char x)
{
  x = (x ^ 0x17) + 0x17;
  return x;
}

unsigned char inicia_nunchuck()
{
    unsigned char i=0, j, buffer[8];

    start_i2c();
    i += escreve_i2c(0xa4);
    i += escreve_i2c(0xf0);
    i += escreve_i2c(0x55);
    stop_i2c();

    start_i2c();
    i += escreve_i2c(0xa4);
    i += escreve_i2c(0xfb);
    i += escreve_i2c(0x00);
    stop_i2c();

    start_i2c();
    i += escreve_i2c(0xa4);
    i += escreve_i2c(0xfa);
    stop_i2c();

    start_i2c();
    i += escreve_i2c(0xa5);
    for(j=0; j<5; j++) buffer[j] = le_i2c(ACK);
    buffer[j] = le_i2c(NACK);
    stop_i2c();

    start_i2c();
    i += escreve_i2c(0xa4);
    i += escreve_i2c(0xf0);
    i += escreve_i2c(0xaa);
    stop_i2c();

    start_i2c();
    i += escreve_i2c(0xa4);
    i += escreve_i2c(0x40);
    i += escreve_i2c(0xac);
    i += escreve_i2c(0xb9);
    i += escreve_i2c(0x8b);
    i += escreve_i2c(0x35);
    i += escreve_i2c(0x1f);
    i += escreve_i2c(0xc8);
    stop_i2c();

    start_i2c();
    i += escreve_i2c(0xa4);
    i += escreve_i2c(0x46);
    i += escreve_i2c(0x28);
    i += escreve_i2c(0xbe);
    i += escreve_i2c(0xb2);
    i += escreve_i2c(0x89);
    i += escreve_i2c(0x07);
    i += escreve_i2c(0x18);
    stop_i2c();

    start_i2c();
    i += escreve_i2c(0xa4);
    i += escreve_i2c(0x4c);
    i += escreve_i2c(0x19);
    i += escreve_i2c(0xff);
    i += escreve_i2c(0x01);
    i += escreve_i2c(0xc6);
    stop_i2c();

    start_i2c();
    i += escreve_i2c(0xa4);
    i += escreve_i2c(0x20);
    stop_i2c();

    start_i2c();
    i += escreve_i2c(0xa5);
    for(j=0; j<7; j++) buffer[j] = le_i2c(ACK);
    buffer[j] = le_i2c(NACK);
    stop_i2c();

    start_i2c();
    i += escreve_i2c(0xa5);
    for(j=0; j<7; j++) buffer[j] = le_i2c(ACK);
    buffer[j] = le_i2c(NACK);
    stop_i2c();

    start_i2c();
    i += escreve_i2c(0xa4);
    i += escreve_i2c(0x30);
    stop_i2c();

    start_i2c();
    i += escreve_i2c(0xa5);
    for(j=0; j<7; j++) buffer[j] = le_i2c(ACK);
    buffer[j] = le_i2c(NACK);
    stop_i2c();

    start_i2c();
    i += escreve_i2c(0xa5);
    for(j=0; j<7; j++) buffer[j] = le_i2c(ACK);
    buffer[j] = le_i2c(NACK);
    stop_i2c();

    return i;
}

