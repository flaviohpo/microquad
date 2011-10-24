//
// ****************************************************************
// ** P i n o s  d o  H a r d w a r e  (M S P 4 3 0 F 2 0 1 3)   **
// ****************************************************************
//
//            VCC                 ///
//             |    +----v----+    |
//             +---[|         |]---+    
//      SDA <----->[|P1.0 P2.6|]
//      SCL <------[|P1.1 P2.7|]    (1)-- VCC      
//                -[|P1.2  TST|]----(2)-- 
//                -[|P1.3  RST|]<---(3)--[1K]--- VCC
//       VM <------[|P1.4  LED|]-   (4)--+  
//       VD <------[|P1.5 P1.6|]-        |
//                  +---------+         ///
//       
//  +---+---+----+----+---+---+-----+-----+
//  | X | X | VM | VD | X | X | SCL | SDA | 
//  +---+---+----+----+---+---+-----+-----+    
//    0   0    1    1   0   0    1    1/0     
//          
//  P1 = 0x32 (data in)
//  P1 = 0x33 (data out)
//
#include <msp430x20x3.h>

#define verm            port1.out.pin5
#define verde           port1.out.pin4
#define sda_in          port1.in.pin0 
#define sda_out         port1.out.pin0
#define scl             port1.out.pin1

#define datain()        P1DIR = 0x32      
#define dataout()       P1DIR = 0x33  


const unsigned char mask[8] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01};


// ****************************************************************
// ** P R O T O T I P O S                                        **
// ****************************************************************
//
void set_DCO(void);
void delayus(unsigned int tempo);
void delayms(unsigned int tempo);

void init_i2c(void);
void start_i2c(void);
void stop_i2c(void);
void send_0(void);
void send_1(void);
unsigned char lebit(void);
unsigned char le_i2c(void);
unsigned char escreve_i2c(unsigned char dado);

unsigned char mem_found(void);



int main(void) 
{	
	set_DCO();	
    init_i2c();
    
    verm = 0;
    verde = 0;
    if(mem_found()) verde = 1; else verm = 1;

	while(1) 
	{
	}
    return 0;
}

// Ajusta DCO em 8MHz...
//
void set_DCO(void)
{
  DCOCTL = DCO0+DCO1;                        // DCOCTL  = 0x60;
  BCSCTL1 = XT2OFF + RSEL3 + RSEL2 + RSEL0;  // BCSCTL1 = 0x8d;
  BCSCTL2 = 0x00;                            
  BCSCTL3 = 0x00;
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
	
	for(i = 0; i < tempo; i++) delayus(300);
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
  delayus(1);
  scl = 1;
  delayus(2);
  sda_out = 0;
  delayus(2);
  scl = 0;
  delayus(1);
}

void stop_i2c(void)
{
  sda_out = 0;
  scl = 1;
  delayus(2);
  sda_out = 1;
  delayus(2);
}

void send_0(void)
{
  sda_out = 0;
  scl = 1;
  delayus(2);
  scl = 0;
  delayus(2);
}

void send_1(void)
{
  sda_out = 1;
  scl = 1;
  delayus(2);
  scl = 0;
  sda_out = 0;
  delayus(2);
}

unsigned char lebit(void)
{
  unsigned char v;

  scl = 1;
  if(sda_in == 1) v = 1; else v = 0;  
  delayus(2);
  scl = 0;
  delayus(2);

  return v;
}

unsigned char le_i2c(void)
{
  unsigned char i, x = 0;

  datain();
  for(i=0; i<8; i++)
    if(lebit()) x += mask[i];

  dataout();
  send_0();

  return x;
}

unsigned char escreve_i2c(unsigned char dado)
{
  unsigned char i;
	
  for(i=0; i<8; i++)
    if ((dado & mask[i]) > 0) send_1(); else send_0();

  datain();
  if(lebit() == 0) i = 0xff; else i = 0;
  dataout();

  return i;
}



// Primitivas do Dispositivo I2C
//
unsigned char mem_found(void)
{
  unsigned char v;

  start_i2c();
  v = escreve_i2c(0xa0);  
  stop_i2c();

  return v;
}




