#include "stdio.h"          // printf, sprintf
#include "stdlib.h"         
#include "string.h"         // malloc, free
#include "legacymsp430.h"   // usado para interruption
#include "msp430f2618.h"    
#include "color.h"          // cores utilizadas no LCD
#include "delay.h"          // time delay, delay por polling
#include "analog.h"         // abstracao analog
#include "lcd6100.h"        // escrita e desenho no lcd
#include "shorttypes.h"     // uint16, uchar, etc

#define constrain(val, min, max) ((val)<(min)?(min):(val)>(max)?(max):val) // limita um valor entre min e max

#define MAX_CURRENT 4095
#define NUM_SAMPLES 5
#define NUM_MEDIANAS 7

#define CURRENT_AN_CH   3
#define LOAD_AN_CH      4
#define VOLTAGE_AN_CH   7
#define SPEED_AN_CH     5

typedef enum{
    INICIANDO       ,
    TESTANDO_2500   ,
    TESTANDO_3000   ,
    TESTANDO_3500   ,
    TESTANDO_4000   ,
    AGUARDA_2000    ,
}SM_TEST;

typedef struct{
    int Currents[4];
    int Voltages[4];
    int Loads[4];
}TEST_RESULT;

void setup(void);
void timer_init(void);
void clock_init(void);
unsigned int Mediana(unsigned int Samples[NUM_SAMPLES]);
unsigned int Media(unsigned int Samples[NUM_MEDIANAS]);
void mostra_atual(void);
void mostra_resultado(void);

TEST_RESULT LastResult;
unsigned int Current = 0, Voltage = 0;
int Load = 0, LoadOffset = 0;
    
int main(void){
       
    SM_TEST TestStep =  INICIANDO;
                 
    WDTCTL = WDTPW + WDTHOLD;   // desabilita watchdog
    
    setup();
       
    unsigned int CurrentSamples[NUM_SAMPLES];
    unsigned int VoltageSamples[NUM_SAMPLES];
    unsigned int LoadSamples[NUM_SAMPLES];
    
    unsigned int CurrentMedianas[NUM_MEDIANAS];
    unsigned int VoltageMedianas[NUM_MEDIANAS];
    unsigned int LoadMedianas[NUM_MEDIANAS];
        
    unsigned int Counter = 0;
    unsigned int SampleIndex = 0;
    unsigned int MedianaIndex = 0;
    
    analog_refresh_channel(LOAD_AN_CH);
    LoadOffset = AnalogValue[LOAD_AN_CH];
    
    while(1){
        
        Counter++;
        
        delayms(2);
        
        analog_refresh_channel(SPEED_AN_CH);
        analog_refresh_channel(LOAD_AN_CH);
        analog_refresh_channel(CURRENT_AN_CH);
        analog_refresh_channel(VOLTAGE_AN_CH);
        
        TBCCR1 = constrain(((AnalogValue[SPEED_AN_CH] >> 1) + 2000), 2000, 3999);        
        CurrentSamples[SampleIndex] = AnalogValue[CURRENT_AN_CH];
        VoltageSamples[SampleIndex] = AnalogValue[VOLTAGE_AN_CH];
        LoadSamples[SampleIndex] = AnalogValue[LOAD_AN_CH] - LoadOffset;

        CurrentMedianas[MedianaIndex] = Mediana(CurrentSamples);
        VoltageMedianas[MedianaIndex] = Mediana(VoltageSamples);
        LoadMedianas[MedianaIndex] = Mediana(LoadSamples);
                
        SampleIndex = (SampleIndex + 1) % NUM_SAMPLES;
        MedianaIndex = (MedianaIndex + 1) % NUM_MEDIANAS;
        
        Current = Media(CurrentMedianas);
        Load = (int)Media(LoadMedianas);
        Voltage = Media(VoltageMedianas);
        
        if(Counter == 50){
            Counter = 0;
                        
            switch(TestStep){
                case INICIANDO:
                    lcd_clear(LCDBackColor);
                    TestStep = TESTANDO_2500;
                break;
                
                case TESTANDO_2500:
                    mostra_atual();
                    if(TBCCR1 > 2500){
                        
                        LastResult.Currents[0] = Current;
                        LastResult.Loads[0] = Load;
                        LastResult.Voltages[0] = Voltage;
                        
                        TestStep = TESTANDO_3000;
                    }
                break;
                
                case TESTANDO_3000:
                                    mostra_atual();
                    if(TBCCR1 > 3000){
                        
                        LastResult.Currents[1] = Current;
                        LastResult.Loads[1] = Load;
                        LastResult.Voltages[1] = Voltage;
                        
                        TestStep = TESTANDO_3500;
                    }
                break;
                
                case TESTANDO_3500:
                    mostra_atual();
                    if(TBCCR1 > 3500){
                        
                        LastResult.Currents[2] = Current;
                        LastResult.Loads[2] = Load;
                        LastResult.Voltages[2] = Voltage;
                        
                        TestStep = TESTANDO_4000;
                    }
                break;
                
                case TESTANDO_4000:
                    mostra_atual();
                    if(TBCCR1 > 3950){
                        
                        LastResult.Currents[3] = Current;
                        LastResult.Loads[3] = Load;
                        LastResult.Voltages[3] = Voltage;
                        
                        mostra_resultado();
                        
                        TestStep = AGUARDA_2000;
                    }
                break;
                
                case AGUARDA_2000:
                    if(TBCCR1 < 2050){
                        
                        TestStep = INICIANDO;
                    }
                break;
            }
        }
    }
}

void mostra_atual(void){

    lcd_goto(0,0);
    printf("TESTING:      ");  
    lcd_goto(0,2);
    printf("Speed: %d    ", (TBCCR1 - 2000) / 20);
    lcd_goto(0,3);
    printf("SHUNT: %d    ", Current);
    lcd_goto(0,4);
    printf("Load: %d    ", Load);
    lcd_goto(0,5);
    printf("Volt: %d    ", (Voltage * 9) >> 1);

}

void mostra_resultado(void){
    lcd_goto(0,0);
    printf("LAST TEST:      ");
    lcd_goto(0,2);
    printf("SPEE CURR LOAD VOLT ");
    lcd_goto(0,3);
    printf("2500 %04d %04d %04d", LastResult.Currents[0], LastResult.Loads[0], LastResult.Voltages[0]);
    lcd_goto(0,4);
    printf("3000 %04d %04d %04d", LastResult.Currents[1], LastResult.Loads[1], LastResult.Voltages[1]);
    lcd_goto(0,5);
    printf("3500 %04d %04d %04d", LastResult.Currents[2], LastResult.Loads[2], LastResult.Voltages[2]);
    lcd_goto(0,6);
    printf("4000 %04d %04d %04d", LastResult.Currents[3], LastResult.Loads[3], LastResult.Voltages[3]);
}
unsigned int Mediana(unsigned int Samples[NUM_SAMPLES]){
    // ordena o vetor 
    int i, j;
    unsigned int Aux;
    for(i = 0; i<NUM_SAMPLES; i++){
        for(j = 0; j < (NUM_SAMPLES - 1); j++){
            if(Samples[j] > Samples[j+1]){
                Aux = Samples[j];
                Samples[j] = Samples[j+1];
                Samples[j+1] = Aux;
            }
        }
    }
    // retorna o valor NUM_SAMPLES / 2
    return Samples[NUM_SAMPLES >> 1];
}

unsigned int Media(unsigned int Samples[NUM_MEDIANAS]){
    long Total = 0;
    int i = 0;
    while(i < NUM_MEDIANAS){
        Total += Samples[i];
        i++;
    }       
    
    return (unsigned int)(Total / NUM_MEDIANAS);
}

// esta funcao utiliza registrador do uC diretamente
// tudo para 16MHz
void clock_init(void){
    BCSCTL2 = SELM_0 + DIVM_0 + DIVS_0;
    DCOCTL = 0x77;      
    BCSCTL1 = 0xCF;     
    BCSCTL3 = 0xA0;
}

// esta funcao utiliza registrador do uC diretamente
void timer_init(void){
    /* 
     * TBCCTL1, Capture/Compare Control Register 1
     * 
     * CM_0 -- No Capture
     * CCIS_0 -- CCIxA
     * OUTMOD_7 -- PWM output mode: 7 - PWM reset/set
     */

    TBCCTL1 = CM_0 + CCIS_0 + CLLD_0 + OUTMOD_7;

    /* TBCCR0, Timer_B Capture/Compare Register 0 */
    TBCCR0 = 4000; // 0.05ms = 20000hz

    /* TBCCR1, Timer_B Capture/Compare Register 1 */
    TBCCR1 = 2000; 

    /* 
     * TBCTL, Timer_B3 Control Register
     * 
     * CNTL_0 -- 16-bit, TBR(max) = 0FFFFh
     * TBSSEL_1 -- ACLK
     * ID_3 -- Divider - /8
     * MC_1 -- Up Mode
     */
    TBCTL = TBCLGRP_0 + CNTL_0 + TBSSEL_1 + ID_3 + MC_1 + TBIE;
    
    /* configura saidas do timerB - P4.1 */
    P4SEL = 0x02;
    P4DIR = 0x02;
    
}

// esta funcao utiliza registrador do uC diretamente
void setup(void){   
    P2DIR |= 0x08;
    P2OUT |= 0x08;
    
    clock_init();
    timer_init(); // inicia com 0 ppm no ESC (ESC fica esperando sinal valido beep --- beep --- beep)
    analog_init();
    
    eint(); // habilita interrupt

    lcd_init(LCDBackColor);    

}

// sample rate 20000hz
interrupt (TIMERB1_VECTOR) TIMERB1_ISR_HOOK(void){ 

    TBCTL &= ~0x01;
}

