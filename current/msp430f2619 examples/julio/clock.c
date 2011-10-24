#include <msp430x2619.h>

#define DCOCLOCK   0
#define XT2CLOCK   0x88
#define LFXT1      0xc8

#define clock      XT2CLOCK

#define led        port3.out.pin0
#define pino       port1.out.pin0


void aguarda_us(unsigned int valor);

int main (void)
{
    WDTCTL = WDTPW + WDTHOLD;     // desliga watchdog timer

    // 16MHz...
    DCOCTL = 0x77;
    BCSCTL1 = 0x0f;
    BCSCTL2 = 0x08;
    BCSCTL3 = 0x8c;

    P3DIR = 0xd7;  
    P5SEL = 0x70;
    P5DIR = 0x70;

    P2SEL = 0x01;
    P2DIR = 0x01;

    P1SEL = 0x10;

    while(1)
    {
      led = 0;
      aguarda_us(50000);
      led = 1;
      aguarda_us(50000);
    }    

    return 0;
}   

void aguarda_us(unsigned int valor)
{
  volatile unsigned int i;
    
  for(i=0; i < valor; i++);
}


