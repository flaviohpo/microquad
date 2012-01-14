#include "msp430f2618.h"
#include "misc.h"
#include "legacymsp430.h"
#include "timer.h"

// ps.: as funcoes passadas para estes ponteiros devem ser inline de preferencia
void(*interrupt_timer_A)(void); // transformar isso num ponteiro de funcoes
void(*interrupt_timer_B)(void); // transformar isso num ponteiro de funcoes

void timer_set_interrupts(void(*func_timer_A)(void), void(*func_timer_B)(void)){
    interrupt_timer_A = func_timer_A;
    interrupt_timer_B = func_timer_B;
}

void timer_set_interrupt_A(void(*func_timer_A)(void)){
   interrupt_timer_A = func_timer_A;
}


void timer_set_interrupt_B(void(*func_timer_B)(void)){
    interrupt_timer_B = func_timer_B;   
}


void timer_init(void){
    
    // timerB
       
    TBCCTL1 = CM_0 + CCIS_0 + CLLD_0 + OUTMOD_7;
    TBCCTL2 = CM_0 + CCIS_0 + CLLD_0 + OUTMOD_7;
    TBCCTL3 = CM_0 + CCIS_0 + CLLD_0 + OUTMOD_7;
    TBCCTL4 = CM_0 + CCIS_0 + CLLD_0 + OUTMOD_7;
    TBCCTL5 = CM_0 + CCIS_0 + CLLD_0 + OUTMOD_7;
    TBCCTL6 = CM_0 + CCIS_0 + CLLD_0 + OUTMOD_7;

    TBCCR1 = 0; 
    TBCCR2 = 0; 
    TBCCR3 = 0; 
    TBCCR4 = 0; 
    TBCCR5 = 0; 
    TBCCR6 = 0; 

    TBCCR0 = TIMER_B_LIMIT; // 400Hz
    TBCTL = TBCLGRP_0 + CNTL_0 + TBSSEL_1 + ID_3 + MC_1 + TBIE;
        
    // timerA
    TACCR1 = 0;
    TACCR0 = TIMER_A_LIMIT; // 2000Hz
    TACCTL0 = CM_0 + CCIS_0 + OUTMOD_0;
    TACCTL1 = CM_0 + CCIS_0 + OUTMOD_7;
    TACTL = TASSEL_1 + ID_3 + MC_1 + TAIE;


}


// delay
interrupt (TIMERA1_VECTOR) TIMERA1_ISR_HOOK(void){
 
    interrupt_timer_A();
        
    // apaga o flag da interrupt
    TACTL &= ~0x01;
}

interrupt (TIMERB1_VECTOR) TIMERB1_ISR_HOOK(void){
 
    interrupt_timer_B();
        
    // apaga o flag da interrupt
    TBCTL &= ~0x01;
}
