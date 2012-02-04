#include "stdlib.h"
#include "stdio.h"
#include "math.h"
#include "msp430f2618.h"

#include "mma7455.h"
#include "delay.h"
#include "i2c.h"
#include "lcd6100.h"

int main(void){
    
    char resp, x_axis, y_axis, z_axis;
    
    WDTCTL = WDTPW + WDTHOLD;   // desabilita watchdog
    
    BCSCTL2 = SELM_0 + DIVM_0 + DIVS_0;
    DCOCTL = 0x77;      
    BCSCTL1 = 0xCF;     
    BCSCTL3 = 0xA0;
    
    lcd_init(LCDBackColor);
    
    P2DIR |= 0x08;
    P2OUT |= 0x08;
    
    i2c_config(0x1C);
    resp = i2c_find_device();

    if(resp == 0) 
        printf("Achei"); 
    else 
        printf("Nao achei");  
        
    delayms(2000);
    lcd_clear(LCDBackColor);
    
    i2c_write_byte(0x16, 0x05);
    delayms(1);
    i2c_read_byte(0x16, &resp);
    delayms(1);
    
    while(1){
        i2c_read_byte(0x06, &x_axis);  
        delayms(100);
        i2c_read_byte(0x07, &y_axis);  
        delayms(100);
        i2c_read_byte(0x08, &z_axis);  
        delayms(100);
    
        lcd_goto(0,0);
        printf("X:%4d", x_axis);
        lcd_goto(8,0);
        printf("Y:%4d", y_axis);
        lcd_goto(0,1);
        printf("Z:%4d", z_axis);
    }    
    return 0;
}
