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

// variaveis leitura radio raw
int TimeUpEdge[8] = {0,0,0,0,0,0,0,0};
int PPMValue[8] = {0,0,0,0,0,0,0,0};

int main(){
    WDTCTL = WDTPW + WDTHOLD;   // desabilita watchdog

    setup();    // inicializa perifericos e verifica se precisa calibrar o radio
    lcd_setcolor(BLACK, WHITE);   
    
    int RXminval = 0;
    int RXmaxval = 256;
    int BoolVal = CHECKED;  
    int BoolVal2 = UNCHECKED;  
          
    Item *oItem = create_item("CH1", ITEMTYPE_VALUE_BAR_R,&RXminval,&RXmaxval,NULL,&PPMValue[0]);
    MENU *MainMenu = menu_create("RADIO", oItem, 5, 50, 15, 5, 0);
    menu_add_item(MainMenu, create_item("CH2", ITEMTYPE_VALUE_BAR_R,&RXminval,&RXmaxval,NULL,&PPMValue[1]));
    menu_add_item(MainMenu, create_item("CH3", ITEMTYPE_VALUE_BAR_R,&RXminval,&RXmaxval,NULL,&PPMValue[2]));
    menu_add_item(MainMenu, create_item("CH4", ITEMTYPE_VALUE_BAR_R,&RXminval,&RXmaxval,NULL,&PPMValue[3]));
    menu_add_item(MainMenu, create_item("CH5", ITEMTYPE_VALUE_BAR_R,&RXminval,&RXmaxval,NULL,&PPMValue[4]));
    menu_add_item(MainMenu, create_item("CH6", ITEMTYPE_VALUE_BAR_R,&RXminval,&RXmaxval,NULL,&PPMValue[5]));
    menu_add_item(MainMenu, create_item("CH7", ITEMTYPE_VALUE_BAR_R,&RXminval,&RXmaxval,NULL,&PPMValue[6]));
    menu_add_item(MainMenu, create_item("CH8", ITEMTYPE_VALUE_BAR_R,&RXminval,&RXmaxval,NULL,&PPMValue[7]));
    menu_add_item(MainMenu, create_item("BOOL", ITEMTYPE_BOOLEAN_R,NULL,NULL,NULL,&BoolVal));
    menu_add_item(MainMenu, create_item("BOOL", ITEMTYPE_BOOLEAN_R,NULL,NULL,NULL,&BoolVal2));
     
    draw_menu(MainMenu, 1);

    int i = 0;
 
    while(1){
        while(i<10){
            MainMenu->SelectedItem = (i);
            delayms(500);   
            draw_menu(MainMenu, 0);
            menu_refresh(MainMenu);
            i++;
        }
        while(i){
            i--;
            MainMenu->SelectedItem = (i);
            delayms(500);   
            draw_menu(MainMenu, 0);
            menu_refresh(MainMenu);
        }
    }    
    __bis_SR_register(LPM4_bits);
  
    while(1){
        
            draw_menu(MainMenu, 0);
            menu_refresh(MainMenu);    
    }
}

interrupt (TIMERA0_VECTOR) TIMERA0_ISR_HOOK(void){
    // ta livre
}

// ok
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

// ok
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

void timer_a3_init(void){
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
    TACCR0 = 4095; //2ms

    /* 
     * TACTL, Timer_A3 Control Register
     * 
     * TASSEL_2 -- SMCLK
     * ID_3 -- Divider - /8
     * MC_1 -- Up Mode
     */
    TACTL = TASSEL_2 + ID_3 + MC_1;
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
                PPMValue[channel_num] = PPMValue[channel_num]>>4,
                P1IES &= ~(PPM_ch_counter & PPM_P1MASK); // configure low to high      
            }

            P1IFG &= ~(PPM_ch_counter & PPM_P1MASK); // apaga o interruption flag da P1.7

        }
        channel_num++;
    }
}

void setup(){
    
    P2DIR |= 0x08;
    P2OUT |= 0x08;
    
    clock_init();
    
    timer_a3_init();
    
    p1_init();
    
    analog_init();
    
    eint();
    
    lcd_init(WHITE);
    
}
