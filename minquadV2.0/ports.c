#include "msp430f2618.h"
#include "legacymsp430.h"
#include "ports.h"
#include "mytypes.h"

void(*interrupt_p1)(void) = 0;

void port_set_interrupt_p1(void(*func_p1)(void)){
    interrupt_p1 = func_p1;
}

void port_init_p1(uint8 InitialCondition, uint8 InterruptEnable, uint8 Outputs, uint8 Select){
    P1OUT = InitialCondition;
    P1IES = InterruptEnable;
    P1RES = InterruptEnable;
    P1IE = InterruptEnable;
    P1SEL = Select;
}

void port_init_p2(uint8 InitialCondition, uint8 InterruptEnable, uint8 Outputs, uint8 Select){
    P2OUT = InitialCondition;
    P2IES = InterruptEnable;
    P2IE = InterruptEnable;
    P2SEL = Select;
}

void port_init_p3(uint8 InitialCondition, uint8 Outputs, uint8 Select){
    P3OUT = InitialCondition;
    P3DIR = Outputs;
    P3SEL = Select;
}

void port_init_p4(uint8 InitialCondition, uint8 Outputs, uint8 Select){
    P4OUT = InitialCondition;
    P4DIR = Outputs;
    P4SEL = Select;
}

void port_init_p5(uint8 InitialCondition, uint8 Outputs, uint8 Select){
    P5OUT = InitialCondition;
    P5DIR = Outputs;
    P5SEL = Select;
}

void port_init_p6(uint8 InitialCondition, uint8 Outputs, uint8 Select){
    P6OUT = InitialCondition;
    P6DIR = Outputs;
    P6SEL = Select;
}

interrupt (PORT1_VECTOR) PORT1_ISR_HOOK(void){
    
    if(interrupt_p1 != 0){
        interrupt_p1();
    }    
      
    P1IFG = 0x00;
}
