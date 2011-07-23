#include "stdio.h"
#include "stdlib.h"
#include "signal.h"
#include "msp430x261x.h"
#include "delay.h"
#include "analog.h"
#include "lcd6100.h"

#define PPM_P1MASK 0xFF

unsigned int Milliseconds = 0;
unsigned int TimeUpEdge[8] = {0,0,0,0,0,0,0,0};
unsigned int PPMValue[8] = {0,0,0,0,0,0,0,0};

void clock_init(void)
{
    /* 
     * Basic Clock System Control 2
     * 
     * SELM_0 -- DCOCLK
     * DIVM_0 -- Divide by 1
     * ~SELS -- DCOCLK
     * DIVS_0 -- Divide by 1
     * ~DCOR -- DCO uses internal resistor
     * 
     * Note: ~<BIT> indicates that <BIT> has value zero
     */
    BCSCTL2 = SELM_0 + DIVM_0 + DIVS_0;
    
    DCOCTL = 0x77;      
        /* 
     * Basic Clock System Control 1
     * 
     * XT2OFF -- Disable XT2CLK
     * XTS -- High Frequency
     * DIVA_0 -- Divide by 1
     */
    BCSCTL1 = 0xcf;     

    /* 
     * Basic Clock System Control 3
     * 
     * XT2S_3 -- Digital input signal
     * LFXT1S_2 -- If XTS = 0, XT1 = VLOCLK ; If XTS = 1, XT1 = 3 - 16-MHz crystal or resonator
     * XCAP_0 -- ~1 pF
     */
    BCSCTL3 = 0xa0;
}

void p1_init(){
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

void timer_a3_init(void)
{
    /* 
     * TACCTL0, Capture/Compare Control Register 0
     * 
     * CM_0 -- No Capture
     * CCIS_0 -- CCIxA
     * ~SCS -- Asynchronous Capture
     * ~SCCI -- Latched capture signal (read)
     * ~CAP -- Compare mode
     * OUTMOD_0 -- PWM output mode: 0 - OUT bit value
     * 
     * Note: ~<BIT> indicates that <BIT> has value zero
     */
    TACCTL0 = CM_0 + CCIS_0 + OUTMOD_0 + CCIE;

    /* TACCR0, Timer_A Capture/Compare Register 0 */
    TACCR0 = 4001;

    /* 
     * TACTL, Timer_A3 Control Register
     * 
     * TASSEL_2 -- SMCLK
     * ID_3 -- Divider - /8
     * MC_1 -- Up Mode
     */
    TACTL = TASSEL_2 + ID_3 + MC_1;
}

interrupt (TIMERA0_VECTOR) TIMERA0_ISR_HOOK(void){
    Milliseconds += 2;
    if(Milliseconds == 10){
        //faz o controle
        Milliseconds = 0;
    }
}

interrupt (PORT1_VECTOR) PORT1_ISR_HOOK(void){
    unsigned int PPM_aux = TAR; // captura aqui pra ser mais exato
    // aqui eh usado PPM_P1_MASK, pq eh a interrupcao da P1 
    int channel_num = 0, PPM_ch_counter;
    for(PPM_ch_counter = 0x01; PPM_ch_counter <= 0x80; PPM_ch_counter = PPM_ch_counter << 1){
        if(P1IFG & (PPM_ch_counter & PPM_P1MASK)){
            if(!(P1IES & PPM_ch_counter)){ // low to high
                TimeUpEdge[channel_num] = PPM_aux;
                P1IES |= (PPM_ch_counter & PPM_P1MASK); // configura high to low
            }
            else{ // high to low
                if(TimeUpEdge[channel_num] > PPM_aux){ // se deu overflow na contagem do timer
                    PPMValue[channel_num] = TACCR0 - TimeUpEdge[channel_num] + PPM_aux;
                }
                else{ // se nao deu overflow na contagem do timer
                    PPMValue[channel_num] = PPM_aux - TimeUpEdge[channel_num];
                }
                P1IES &= ~(PPM_ch_counter & PPM_P1MASK); // configure low to high      
            }
            P1IFG &= ~(PPM_ch_counter & PPM_P1MASK); // apaga o interruption flag da P1.7
        }
        channel_num++;
    }
}

void draw_rc_inputs(){
    unsigned int i = 0, k = 0, h = 0;
    lcd_goto(0,2);
    
    printf("ch[%d]: %d", 0, PPMValue[0]);
    k = 16 + (8*0); //posicao inicial em y
    /*
    if(PPMValue[0] > 4000){
        h=100;
    }
    else{
        if(PPMValue[0] < 2001){
            h = 0;
        }
        else{
            h = ((PPMValue[0] - 2000) * 10) / 200;
        }
    }*/
    lcd_drawprogressbar(80,16,120,24,WHITE, BLUE, 50);
/*
    for(i = 0; i < 8; i++){
        printf("ch[%d]: %d", i, PPMValue[i]);
        k = 16+(8*i); //posicao inicial em y
        if(PPMValue[i] > 4000){
            h=100;
        }
        else{
            if(PPMValue[i] < 2001){
                h = 0;
            }
            else{
                h = ((PPMValue[i] - 2000) * 10) / 200;
            }
        }
        lcd_drawprogressbar(60,k,120,k+8,WHITE, BLUE, h);
    }*/
}

int main(){
    WDTCTL = WDTPW + WDTHOLD;
    clock_init();
    timer_a3_init();
    p1_init();
    analog_init();
    
    eint();

    lcd_init(BLACK);
    
    delayms(200);
    
    draw_rc_inputs();
    
    while(1){
        delayms(10);
        
    }
}

