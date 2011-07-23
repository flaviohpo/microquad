#include "stdio.h"
#include "stdlib.h"
#include "signal.h"
#include "msp430x261x.h"
#include "delay.h"
#include "analog.h"
#include "lcd6100.h"
#include "eeprom.h"
#include "i2c.h"
#include "microquad.h"

volatile unsigned int Milliseconds = 0;
volatile unsigned int TimeUpEdge[8] = {0,0,0,0,0,0,0,0};
volatile unsigned int PPMValue[8] = {0,0,0,0,0,0,0,0};

float PPMSlope[8] = {1, 1, 1, 1, 1, 1, 1, 1};
float PPMOffset[8] = {0, 0, 0, 0, 0, 0, 0, 0};
int ChannelInput[8] = {0, 0, 0, 0, 0, 0, 0, 0};

char MotorArmed = 0;

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

void calibrate_radio(void){
    
    if(MotorArmed == 1 ){
        return;
    }

    int i = 0, k;    
    float PPMMin[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    float PPMMax[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    fourbytes ValorAux;        
    
    lcd_goto(2,5);
    printf("MOVE STICKS\nALL DIRECTIONS");
    delayms(3000);
    lcd_clear(BLACK);

    for(i = 0; i < 8; i++){ // zera offset e slope
        PPMOffset[i] = 0;
        PPMSlope[i] = 1;
    }

    for(i = 0; i < 8; i++){ // coloca um valor inicial em max min
        PPMMin[i] = PPMValue[i];
        PPMMax[i] = PPMMin[i];
    }
    
    for(k = 200; k > 0; k--){
        draw_rc_inputs();
        lcd_goto(0, 13);
        printf("%d ", k);
        delayms(1);
        for(i = 0; i < 8; i++){ // pega max min
            if(PPMMax[i] < PPMValue[i]){ // nao ta pegando maximo
                PPMMax[i] = PPMValue[i];
            }
            if(PPMMin[i] > PPMValue[i]){
                PPMMin[i] = PPMValue[i];
            }
        }
    }
    
    for(i = 0; i < 8; i++){ // calcula offset e slope pra deixar na faixa de 1000 a 2000
        PPMSlope[i] = 2000.0 / (PPMMax[i] - PPMMin[i]);
        PPMOffset[i] = 2000.0 - (PPMSlope[i] * PPMMin[i]);
    }

    // grava dados na EEPROM
    if(EEPROMFound){
        i2c_change_address(EEPROM_I2C_ADDR);
        
        // grava yaw slope 
        ValorAux.f = PPMSlope[YAW_CH];
        i2c_write16_multiples(_RADIO_YAW_SLOPE, ValorAux.c, 4);
        
        delayms(100);
        
        // grava throttle slope 
        ValorAux.f = PPMSlope[THROTTLE_CH];
        i2c_write16_multiples(_RADIO_THROTTLE_SLOPE, ValorAux.c, 4);
        
        delayms(100);
        
        // grava pitch slope 
        ValorAux.f = PPMSlope[PITCH_CH];
        i2c_write16_multiples(_RADIO_PITCH_SLOPE, ValorAux.c, 4);
        
        delayms(100);
        
        // grava roll slope 
        ValorAux.f = PPMSlope[ROLL_CH];
        i2c_write16_multiples(_RADIO_ROLL_SLOPE, ValorAux.c, 4);
        
        delayms(100);
        
        // grava ch5 slope 
        ValorAux.f = PPMSlope[CH5_CH];
        i2c_write16_multiples(_RADIO_CH5_SLOPE, ValorAux.c, 4);
        
        delayms(100);
        
        // grava ch6 slope 
        ValorAux.f = PPMSlope[CH6_CH];
        i2c_write16_multiples(_RADIO_CH6_SLOPE, ValorAux.c, 4);
        
        delayms(100);
        
        // grava yaw offset 
        ValorAux.f = PPMOffset[YAW_CH];
        i2c_write16_multiples(_RADIO_YAW_OFFSET, ValorAux.c, 4);
        
        delayms(100);
        
        // grava throttle offset 
        ValorAux.f = PPMOffset[THROTTLE_CH];
        i2c_write16_multiples(_RADIO_THROTTLE_OFFSET, ValorAux.c, 4);
        
        delayms(100);
        
        // grava pitch offset 
        ValorAux.f = PPMOffset[PITCH_CH];
        i2c_write16_multiples(_RADIO_PITCH_OFFSET, ValorAux.c, 4);
        
        delayms(100);
        
        // grava roll offset 
        ValorAux.f = PPMOffset[ROLL_CH];
        i2c_write16_multiples(_RADIO_ROLL_OFFSET, ValorAux.c, 4);
        
        delayms(100);
        
        // grava ch5 offset 
        ValorAux.f = PPMOffset[CH5_CH];
        i2c_write16_multiples(_RADIO_CH5_OFFSET, ValorAux.c, 4);
        
        delayms(100);
        
        // grava ch6 offset 
        ValorAux.f = PPMOffset[CH6_CH];
        i2c_write16_multiples(_RADIO_CH6_OFFSET, ValorAux.c, 4);
        
    }
}

void process_rc(void){
    ChannelInput[THROTTLE_CH] = PPMValue[THROTTLE_CH] * PPMSlope[THROTTLE_CH] + PPMOffset[THROTTLE_CH];
    ChannelInput[YAW_CH] = PPMValue[YAW_CH] * PPMSlope[YAW_CH] + PPMOffset[YAW_CH];
    ChannelInput[PITCH_CH] = PPMValue[PITCH_CH] * PPMSlope[PITCH_CH] + PPMOffset[PITCH_CH]; // pitch invertido
    ChannelInput[ROLL_CH] = PPMValue[ROLL_CH] * PPMSlope[ROLL_CH] + PPMOffset[ROLL_CH];
    ChannelInput[CH5_CH] = PPMValue[CH5_CH] * PPMSlope[CH5_CH] + PPMOffset[CH5_CH];
    ChannelInput[CH6_CH] = PPMValue[CH6_CH] * PPMSlope[CH6_CH] + PPMOffset[CH6_CH];
    ChannelInput[7] = PPMValue[7];
}

void load_transmitter_values(void){
    fourbytes ValorAux;

    if(EEPROMFound == 1){
        i2c_change_address(EEPROM_I2C_ADDR);    

        // load yaw slope e offset
        i2c_read16_multiples(_RADIO_YAW_SLOPE, ValorAux.c, 4);
        PPMSlope[YAW_CH] = ValorAux.f;
        i2c_read16_multiples(_RADIO_YAW_OFFSET, ValorAux.c, 4);
        PPMOffset[YAW_CH] = ValorAux.f;
        
        // load throttle slope  e offset
        i2c_read16_multiples(_RADIO_THROTTLE_SLOPE, ValorAux.c, 4);
        PPMSlope[THROTTLE_CH] = ValorAux.f;
        i2c_read16_multiples(_RADIO_THROTTLE_OFFSET, ValorAux.c, 4);
        PPMOffset[THROTTLE_CH] = ValorAux.f;
        
        // load pitch slope  e offset
        i2c_read16_multiples(_RADIO_PITCH_SLOPE, ValorAux.c, 4);
        PPMSlope[PITCH_CH] = ValorAux.f;
        i2c_read16_multiples(_RADIO_PITCH_OFFSET, ValorAux.c, 4);
        PPMOffset[PITCH_CH] = ValorAux.f;
        
        // load roll slope  e offset
        i2c_read16_multiples(_RADIO_ROLL_SLOPE, ValorAux.c, 4);
        PPMSlope[ROLL_CH] = ValorAux.f;
        i2c_read16_multiples(_RADIO_ROLL_OFFSET, ValorAux.c, 4);
        PPMOffset[ROLL_CH] = ValorAux.f;
    
        // load ch5 slope  e offset
        i2c_read16_multiples(_RADIO_CH5_SLOPE, ValorAux.c, 4);
        PPMSlope[CH5_CH] = ValorAux.f;
        i2c_read16_multiples(_RADIO_CH5_OFFSET, ValorAux.c, 4);
        PPMOffset[CH5_CH] = ValorAux.f;
    
        // load ch6 slope  e offset
        i2c_read16_multiples(_RADIO_CH6_SLOPE, ValorAux.c, 4);
        PPMSlope[CH6_CH] = ValorAux.f;
        i2c_read16_multiples(_RADIO_CH6_OFFSET, ValorAux.c, 4);
        PPMOffset[CH6_CH] = ValorAux.f;
    }
}

void draw_rc_inputs(){
    unsigned int i = 0, k = 0, h = 0;
    lcd_goto(0,0);
    printf("     RC inputs");
    lcd_goto(0,2);
    
    for(i = 0; i < 8; i++){
        printf("ch[%d]: %d   \n", i, ChannelInput[i]);
        k = 21 + (8 * i); //posicao inicial em y
        if(ChannelInput[i] > 4000){
            h=100;
        }
        else{
            if(ChannelInput[i] < 2001){
                h = 0;
            }
            else{
                h = ((ChannelInput[i] - 2000) * 10) / 200;
            }
        }
        lcd_drawprogressbar(80,k,45,4,WHITE, BLUE, h);
    }
}

int findfirst(int startaddress){
    for(; startaddress < 255; startaddress++){
        i2c_config(startaddress);
        if(!i2c_find_device()){
            return startaddress;
        }
    }   
    return 0;
}

int main(){
    WDTCTL = WDTPW + WDTHOLD;
    
    clock_init();
    timer_a3_init();
    p1_init();
    analog_init();
    i2c_init();
    
    eint();

    lcd_init(BLACK);
    
    //printf("address: %d", findfirst(0));
    //delayms(10000);
    
    i2c_config(EEPROM_I2C_ADDR);
    EEPROMFound = !(i2c_find_device());
    
    if(EEPROMFound){
        printf("EEPROM found\n");
        load_transmitter_values();
        //carregar os outros parametros
    }
    else{
        color_back = RED;
        color_fore = BLACK;
        printf("EEPROM not found\n");
        color_back = BLACK;
        color_fore = WHITE;
    }
    delayms(5000);
    
    //calibrate_radio();
    
    while(1){
        delayms(100);
        process_rc();
        draw_rc_inputs();
        
    }
}

