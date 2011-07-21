#include "stdlib.h"
#include "stdio.h"
#include "msp430x261x.h"
#include "lcd6100.h"
#include "delay.h"
#include "ppm.h"
#include "timers.h"
#include "analog.h"

// printf para LCDNOKIA
int putchar(int c)
{
  lcd_putchar(c);
  return 0;
}
                

int main(){
    
    // clock config
    DCOCTL = 0x77;      
    BCSCTL1 = 0xcf;     
    BCSCTL2 = 0xc8;   
    BCSCTL3 = 0xa0; 
    
    lcd_init(WHITE);
    analog_init();
    
    int n = 0;
    
    while(1){
        analog_refresh_all();
        lcd_goto(0,0);
        for(n = 0; n < 8; n++){
            printf("Analog[%d]: %d      \n",n,AnalogValue[n]);
        }
    }
}
