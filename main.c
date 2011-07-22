#include "stdlib.h"
#include "stdio.h"
#include "signal.h"
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
                

int main(void){
    WDTCTL = WDTPW + WDTHOLD;  // desabilita watchdog
    
    // clock config
    DCOCTL = 0x77;      
    BCSCTL1 = 0xcf;     
    BCSCTL2 = 0xc8;   
    BCSCTL3 = 0xa0; 
    
    eint(); // habilita interruptions
    
    lcd_init(BLACK);
    TMRB_init(20000); // 10ms
    TMRA_init(16001); // 1ms
    analog_init();
    //ppm_init();
    
    int n = 0;
    
    while(1){
        analog_refresh_all();
        
        // refresh tela
        lcd_goto(0,0);
        for(n = 0; n < 3; n++){
            printf("Analog[%d]: %d      \n",n,AnalogValue[n]);
        }
        for(n = 0; n < PPM_NUMCH; n++){
            printf("Ch[%d]: %d       \n",n,PPMResult[n]);
        }
        printf("%d    ",(int)MILLISECONDS);
    }
}
