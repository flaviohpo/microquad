#include "radiorec.h"
#include "msp430f2618.h"
#include "legacymsp430.h"
#include "shorttypes.h"
#include "delay.h"

// variaveis ajuste radio - roll pitch e yaw
int16 PPMOffset[8] = {0,0,0,0,0,0,0,0};
int16 PPMValue[8] = {0,0,0,0,0,0,0,0};
int16 PPMRaw[8] = {0,0,0,0,0,0,0,0};

void calibrate_radio(void){
    unsigned i = 8, j = 8;
    do{
        i--;
        PPMOffset[i] = 0;
    }while(i);
    i=8;
    do{
        i--;
        do{
            j--;
            PPMOffset[j] += PPMRaw[j];
        }while(j);
        j=8;
        delayms(50);
    }while(i);
    i=8;
    do{
        i--;
        PPMOffset[i] = PPMOffset[i] >> 3;
    }while(i);
}

// soh habilitar interruption depois que a P1 estiver configurada
interrupt (PORT1_VECTOR) PORT1_ISR_HOOK(void){
    int16 PPM_aux, channel_num, PPM_ch_counter;
    static uint16 TimeUpEdge[8] = {0,0,0,0,0,0,0,0};
    
    PPM_aux = TAR;                                                                              // captura aqui pra ser mais exato
    channel_num = 0;
    for(PPM_ch_counter = 0x01; PPM_ch_counter <= 0x80; PPM_ch_counter = PPM_ch_counter << 1){
        if(P1IFG & (PPM_ch_counter & PPM_P1MASK)){                                              // aqui eh usado PPM_P1_MASK, pq eh a interrupcao da P1 
            if(!(P1IES & PPM_ch_counter)){                                                      // low to high
                TimeUpEdge[channel_num] = PPM_aux;
                P1IES |= (PPM_ch_counter & PPM_P1MASK);                                         // configura high to low
            }
            else{                                                                               // high to low
            
                if(TimeUpEdge[channel_num] > PPM_aux){                                          // se deu overflow na contagem do timer
                    PPMRaw[channel_num] = ((TACCR0 - TimeUpEdge[channel_num] + PPM_aux));
                }
                else{                                                                           // se nao deu overflow na contagem do timer
                    PPMRaw[channel_num] = ((PPM_aux - TimeUpEdge[channel_num]));
                }
                P1IES &= ~(PPM_ch_counter & PPM_P1MASK);                                        // configure low to high      
            }
            P1IFG &= ~(PPM_ch_counter & PPM_P1MASK);                                            // apaga o interruption flag da P1
        }
        channel_num++;
    }
}

