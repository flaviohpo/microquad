#include "msp430f2618.h"
#include "analog.h"
#include "delay.h"

int AnalogOffset[8] = {0,0,0,0,0,0,0,0};
int AnalogValue[8] = {0,0,0,0,0,0,0,0}; 

void analog_init(void){
    P6SEL = ANALOG_ENABLE;
    
    ADC12CTL0 &= ~ENC;
    ADC12CTL0 = REFON + ADC12ON;
    ADC12CTL1 = SHP;
    ADC12CTL0 |= ENC;
}

void analog_calibrate_channel(int i){
    int k = 0;
    
    if((0x01 << i) & P6SEL){
        AnalogOffset[i] = 0;
        for(k = 0; k < 8; k++ ){
            ADC12CTL0 &= ~ENC;
            ADC12MCTL0 = i;
            ADC12CTL0 |= ENC;
            ADC12CTL0 |= ADC12SC;
            while(ADC12CTL1 & ADC12BUSY);
            delayms(30);
            AnalogOffset[i] += ADC12MEM0;
        }
        AnalogOffset[i] = AnalogOffset[i] >> 3;
    }
}

inline void analog_refresh_all(void){
    int i;
    for(i = 0; i < 8; i++){
        if((0x01 << i) & P6SEL){
            ADC12CTL0 &= ~ENC;
            ADC12MCTL0 = i;
            ADC12CTL0 |= ENC;
            ADC12CTL0 |= ADC12SC;
            while(ADC12CTL1 & ADC12BUSY);
            AnalogValue[i] = ADC12MEM0;   
        }
        else{
            AnalogValue[i] = 0;
        }
    }
}

inline void analog_refresh_channel(int i){
    if((0x01 << i) & P6SEL){
        ADC12CTL0 &= ~ENC;
        ADC12MCTL0 = i;
        ADC12CTL0 |= ENC;
        ADC12CTL0 |= ADC12SC;
        while(ADC12CTL1 & ADC12BUSY);
        AnalogValue[i] = ADC12MEM0;   
    }
    else{
        AnalogValue[i] = 0;
    }
}
     
