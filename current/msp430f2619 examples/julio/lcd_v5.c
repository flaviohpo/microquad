#include <io.h>
#include <stdlib.h>
#include <signal.h>
#include <msp430x2619.h>


#define led port3.out.pin0
#define en  port3.out.pin7
#define rs  port3.out.pin6
#define dado P4OUT


void init_lcd(void);
void wr_cmd(void);
void aguarda_1us(unsigned int valor);


int main (void)
{
    int i;

    WDTCTL = WDTPW + WDTHOLD;     // desliga watchdog timer

    BCSCTL1 = 0x07;
    BCSCTL2 = 0x88;
    BCSCTL3 = 0x84;

    P4DIR = 0xff;
    P3DIR = 0xd7;
    P1DIR = 0xf0;
    
    for(i=0; i<10; i++)
    {
      led = 0;
      aguarda_1us(50000);
      led = 1;
      aguarda_1us(50000);
    }    

    init_lcd();        
//    dado = 0x41;
//    wr_char();       

    while(1);
    return 0;
    
}   

void wr_cmd(void)
{       
        rs = 0;
        en = 1;        
        aguarda_1us(500);
        en = 0;
        aguarda_1us(5000);
}


    
void aguarda_1us(unsigned int valor)
{
   volatile unsigned int i;
    for(i=0; i < valor; i++)
    {}
}


void init_lcd(void)
{    

    dado = 0x38;
    wr_cmd();
    wr_cmd();
    wr_cmd();
    dado = 0x0e;
    wr_cmd();
    dado = 0x06;
    wr_cmd();
    dado = 0x01;
    wr_cmd();
}    

