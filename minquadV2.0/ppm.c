#include "msp430f2618.h"
#include "delay.h"
#include "stdlib.h"
#include "mytypes.h"
#include "misc.h"
#include "timer.h"
#include "ports.h"
#include "ppm.h"

uint16 PPMAux = 0;
uint16 TimeUpEdge[8] = {0,0,0,0,0,0,0,0};
uint16 PPMValue[8] = {0,0,0,0,0,0,0,0};
int16 PPMOffset[8] = {0,0,0,0,0,0,0,0};

uint8 ChannelNum = 0;    
uint8 PPMChannelCounter = 0;
uint8 PPMInChannelMask = 0;
uint8 PPMOutChannelMask = 0;

void ppm_interrupt(void);

void ppm_init(unsigned char InChannelMask, unsigned char OutChannelMask){
    PPMInChannelMask = InChannelMask;
    PPMOutChannelMask = OutChannelMask;
    
    timer_init();
    timer_set_interrupt_B(&ppm_interrupt);
}

inline void ppm_interrupt(void){
    PPMAux = get_ppm_timer_count(); // captura aqui pra ser bem na hora da interrupt
    // aqui eh usado PPM_P1_MASK, pq eh a interrupcao da P1 
    ChannelNum = 0;
    for(PPMChannelCounter = 0; PPMChannelCounter < 8; PPMChannelCounter++){
        if(Port1GetInterruptFlag((1 << PPMChannelCounter) & PPMInChannelMask)){
            if(!Port1GetInterruptEdge(PPMChannelCounter)){ // low to high
                TimeUpEdge[ChannelNum] = PPMAux;
                Port1SetInterruptEdge(PPMChannelCounter); // configura high to low
            }
            else{ // high to low
                if(TimeUpEdge[ChannelNum] > PPMAux){ // se deu overflow na contagem do timer
                    PPMValue[ChannelNum] = (TIMER_B_LIMIT - TimeUpEdge[ChannelNum] + PPMAux);
                }
                else{ // se nao deu overflow na contagem do timer
                    PPMValue[ChannelNum] = (PPMAux - TimeUpEdge[ChannelNum]);
                }
                Port1ClrInterruptEdge(PPMChannelCounter); // configure low to high      
            }
        }
        ChannelNum++;
    }
}

/* Atencao, esta funcao aceita tudo entao use constrain ao 
 * colocar dados no vetor de output
 */
 
inline void ppm_set_outputs(int16 OutputVector[6]){
    set_ppm_out_1(OutputVector[0]);   
    set_ppm_out_2(OutputVector[1]);
    set_ppm_out_3(OutputVector[2]);
    set_ppm_out_4(OutputVector[3]);
    set_ppm_out_5(OutputVector[4]);
    set_ppm_out_6(OutputVector[5]);
}

/* Atencao, esta funcao aceita tudo entao use constrain ao 
 * colocar dados no output
 */
 
inline void ppm_set_output(uchar Num, uint16 Val){
    switch(Num){
        case 0:
            set_ppm_out_1(Val);   
            break;
        case 1:
            set_ppm_out_2(Val);   
            break;
        case 2:
            set_ppm_out_3(Val);   
            break;
        case 3:
            set_ppm_out_4(Val);   
            break; 
        case 4:
            set_ppm_out_5(Val);   
            break; 
        case 5:
            set_ppm_out_6(Val);   
            break;
    }
}

void ppm_set_all_outputs(uint16 Value){
    set_ppm_out_1(Value);   
    set_ppm_out_2(Value);
    set_ppm_out_3(Value);
    set_ppm_out_4(Value);
    set_ppm_out_5(Value);
    set_ppm_out_6(Value);        
}

void ppm_calibrate(void){
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
            PPMOffset[j] += PPMValue[j];
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

