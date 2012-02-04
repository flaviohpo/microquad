#include "hardware_config.h"
#include "msp430f2618.h"
#include "legacymsp430.h"
#include "i2c.h"

void clock_init(void){
    BCSCTL2 = SELM_0 + DIVM_0 + DIVS_0;
    DCOCTL = 0x77;      
    BCSCTL1 = 0xCF;     
    BCSCTL3 = 0xA0;
}

void hardware_setup(void){
    WDTCTL = WDTPW + WDTHOLD;   // desabilita watchdog       
    
    clock_init();
    
    LCD_BACKLIGHT_OUT();
    SET_LCD_BACKLIGHT();

    i2c_init_module();
    



}
