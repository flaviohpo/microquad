#include "msp430f2618.h"
#include "system.h"

void init_cpu(void){
    //disable watchdog
    WDTCTL = WDTPW + WDTHOLD;   // desabilita watchdog
    
    BCSCTL2 = SELM_0 + DIVM_0 + DIVS_0;
    DCOCTL = 0x77;
    BCSCTL1 |= 0xD0;
    BCSCTL3 = 0xA0;
}
