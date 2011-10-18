#include "msp430f2618.h"
#include "analog.h"

unsigned int AnalogOffset[8] = {0,0,0,0, 0,0,0,0};
unsigned int AnalogValue[8] = {0,0,0,0, 0,0,0,0}; 

void analog_init(void){
    P6SEL = ANALOG_ENABLE;
    
    ADC12CTL0 &= ~ENC;
    ADC12CTL0 = REFON + ADC12ON;
    ADC12CTL1 = SHP;
    ADC12CTL0 |= ENC;           
}

void analog_calibrate_channel(int i){
    int k = 0;
    int j = 0;
    float media = 0;
    
    if((0x01 << i) & P6SEL){
        for(k = 0; k < 3000; k++ ){
            for(j = 0; j < 300; j++);
            ADC12CTL0 &= ~ENC;
            ADC12MCTL0 = i;
            ADC12CTL0 |= ENC;
            ADC12CTL0 |= ADC12SC;
            while(ADC12CTL1 & ADC12BUSY);
            media = (media * 0.95) + (ADC12MEM0 * 0.05); 
        }
        AnalogOffset[i] = (int)media;
    }
}

void analog_refresh_all(void){
    int i;
    for(i = 0; i < 8; i++){
        if((0x01 << i) & P6SEL){
            ADC12CTL0 &= ~ENC;
            ADC12MCTL0 = i;
            ADC12CTL0 |= ENC;
            ADC12CTL0 |= ADC12SC;
            while(ADC12CTL1 & ADC12BUSY);
            AnalogValue[i] = ADC12MEM0 - AnalogOffset[i];   
        }
        else{
            AnalogValue[i] = 0;
        }
    }
}

void analog_refresh_channel(int i){
    if((0x01 << i) & P6SEL){
        ADC12CTL0 &= ~ENC;
        ADC12MCTL0 = i;
        ADC12CTL0 |= ENC;
        ADC12CTL0 |= ADC12SC;
        while(ADC12CTL1 & ADC12BUSY);
        AnalogValue[i] = ADC12MEM0 - AnalogOffset[i];   
    }
    else{
        AnalogValue[i] = 0;
    }
}
     
