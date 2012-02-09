#include "stdio.h"
#include "stdlib.h"
#include "legacymsp430.h"
#include "msp430f2618.h"
#include "delay.h"
#include "analog.h"
#include "color.h"
#include "lcd6100.h"
#include "eeprom.h"
#include "i2c.h"
#include "microquad.h"
#include "usefullibs.h"
#include "menu.h"

#define Constrain(val,min,max) ((val)<(min)?(min):(val)>(max)?(max):val) // limita um valor entre min e max

// variaveis leitura radio raw
unsigned int TimeUpEdge[8] = {0,0,0,0,0,0,0,0};
int PPMValue[8] = {0,0,0,0,0,0,0,0};
unsigned int MotorOutput[6] = {0,0,0,0,0,0};

// prototipos
void set_PPM_output(void);
void clock_init(void);
void p1_init(void);
void timer_a3_init(void);
void timer_b0_init(void);
void setup(void);

int main(){
    int RXminval = 1900;
    int RXmaxval = 4100;

    WDTCTL = WDTPW + WDTHOLD;   // desabilita watchdog
    
    setup();    // inicializa perifericos e verifica se precisa calibrar o radio
    lcd_setcolor(BLACK, WHITE);   
    
    MENU *MainMenu = menu_create("RADIO", create_item("CH1", ITEMTYPE_VALUE_BAR_R,&RXminval,&RXmaxval,NULL,&PPMValue[0]), 8, 50, 15, 5, 0);
    menu_add_item(MainMenu, create_item("CH2 ", ITEMTYPE_VALUE_BAR_R,&RXminval,&RXmaxval,NULL,&PPMValue[1]));
    menu_add_item(MainMenu, create_item("CH3 ", ITEMTYPE_VALUE_BAR_R,&RXminval,&RXmaxval,NULL,&PPMValue[2]));
    menu_add_item(MainMenu, create_item("CH4 ", ITEMTYPE_VALUE_BAR_R,&RXminval,&RXmaxval,NULL,&PPMValue[3]));
    menu_add_item(MainMenu, create_item("CH5 ", ITEMTYPE_VALUE_BAR_R,&RXminval,&RXmaxval,NULL,&PPMValue[4]));
    menu_add_item(MainMenu, create_item("CH6 ", ITEMTYPE_VALUE_BAR_R,&RXminval,&RXmaxval,NULL,&PPMValue[5]));
    menu_add_item(MainMenu, create_item("CH7 ", ITEMTYPE_VALUE_BAR_R,&RXminval,&RXmaxval,NULL,&PPMValue[6]));
    menu_add_item(MainMenu, create_item("CH8 ", ITEMTYPE_VALUE_BAR_R,&RXminval,&RXmaxval,NULL,&PPMValue[7]));

    draw_menu(MainMenu, 1);

    while(1){
        menu_refresh(MainMenu);
        /*
        for(i = 0; i<6; i++){
            MotorOutput[i] = PPMValue[3];
        }
        set_PPM_output();*/
    }    
    __bis_SR_register(LPM4_bits);
}

interrupt (TIMERA0_VECTOR) TIMERA0_ISR_HOOK(void){
    // ta livre
    
}

void set_PPM_output(void){
    TBCCR1 = Constrain(MotorOutput[0], 2000, 4000); 
    TBCCR2 = Constrain(MotorOutput[1], 2000, 4000); 
    TBCCR3 = Constrain(MotorOutput[2], 2000, 4000);
    TBCCR4 = Constrain(MotorOutput[3], 2000, 4000);
    TBCCR5 = Constrain(MotorOutput[4], 2000, 4000);
    TBCCR6 = Constrain(MotorOutput[5], 2000, 4000);
}

// ok
void clock_init(void){
    BCSCTL2 = SELM_0 + DIVM_0 + DIVS_0;
    DCOCTL = 0x77;      
    BCSCTL1 = 0xCF;     
    BCSCTL3 = 0xA0;
    // SMCLK = 16MHz
    // ACKL = 16MHz
    // MCLK = 16MHz
}

// ok
void p1_init(void){
        /* Port 1 Output Register */
    P1OUT = PPM_P1MASK;

    /* Port 1 Resistor Enable Register */
    P1REN = PPM_P1MASK;

    /* Port 1 Interrupt Edge Select Register */
    P1IES = 0; // subida

    /* Port 1 Interrupt Flag Register */
    P1IFG = 0;

    /* Port 1 Interrupt Enable Register */
    P1IE = PPM_P1MASK;

}

void timer_a3_init(void){
    TACCTL0 = CM_0 + CCIS_0 + OUTMOD_0;// + CCIE;
    TACCR0 = 4000; //2ms
    TACTL = TASSEL_2 + ID_3 + MC_1;
}

void timer_b0_init(void){
    TBCCTL1 = CM_0 + CCIS_0 + CLLD_0 + OUTMOD_7;
    TBCCTL2 = CM_0 + CCIS_0 + CLLD_0 + OUTMOD_7;
    TBCCTL3 = CM_0 + CCIS_0 + CLLD_0 + OUTMOD_7;
    TBCCTL4 = CM_0 + CCIS_0 + CLLD_0 + OUTMOD_7;
    TBCCTL5 = CM_0 + CCIS_0 + CLLD_0 + OUTMOD_7;
    TBCCTL6 = CM_0 + CCIS_0 + CLLD_0 + OUTMOD_7;

    TBCCR0 = 5000;

    TBCCR1 = 0; 
    TBCCR2 = 0; 
    TBCCR3 = 0; 
    TBCCR4 = 0; 
    TBCCR5 = 0; 
    TBCCR6 = 0; 

    TBCTL = TBCLGRP_0 + CNTL_0 + TBSSEL_2 + ID_3 + MC_1;
    
    /* configura saidas do timerB - P4.1 a P4.6*/
    P4SEL = 0x7E;
    P4DIR = 0x7E;
}

unsigned int PPM_aux;
int channel_num, PPM_ch_counter;
// muito consagred
interrupt (PORT1_VECTOR) PORT1_ISR_HOOK(void){
    PPM_aux = TAR; // captura aqui pra ser mais exato
    // aqui eh usado PPM_P1_MASK, pq eh a interrupcao da P1 
    channel_num = 0;
    for(PPM_ch_counter = 0x01; PPM_ch_counter <= 0x80; PPM_ch_counter = PPM_ch_counter << 1){
        if(P1IFG & (PPM_ch_counter & PPM_P1MASK)){
            if(!(P1IES & PPM_ch_counter)){ // low to high
                TimeUpEdge[channel_num] = PPM_aux;
                P1IES |= (PPM_ch_counter & PPM_P1MASK); // configura high to low
            }
            else{ // high to low
            
                if(TimeUpEdge[channel_num] > PPM_aux){ // se deu overflow na contagem do timer
                    PPMValue[channel_num] = (TACCR0 - TimeUpEdge[channel_num] + PPM_aux);
                }
                else{ // se nao deu overflow na contagem do timer
                    PPMValue[channel_num] = (PPM_aux - TimeUpEdge[channel_num]);
                }
                P1IES &= ~(PPM_ch_counter & PPM_P1MASK); // configure low to high      
            }
            P1IFG &= ~(PPM_ch_counter & PPM_P1MASK); // apaga o interruption flag da P1.7
        }
        channel_num++;
    }
}

void setup(void){
    
    P2DIR |= 0x08;
    P2OUT |= 0x08;
    
    clock_init();
    
    timer_a3_init();
    timer_b0_init();
        
    p1_init();
    
    analog_init();
    
    eint();
    
    lcd_init(WHITE);
    
}
