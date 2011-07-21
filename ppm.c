#include "msp430x261x.h"
#include "signal.h"
#include "ppm.h"

/* PPM Interruption */
int PPM_capture_time[6] = {0, 0, 0, 0, 0, 0};

volatile float PPMResult[6] = {0, 0, 0, 0, 0, 0};
volatile int PPMChecker = 1;

int PPM_ch_counter = 0;

void ppm_init(void){
    // configura interruptions na P1 e P2
    P1OUT &= ~(PPM_P1_MASK);
    P1SEL &= ~(PPM_P1_MASK);
    P1IFG = 0x00;
    P1IES &= ~(PPM_P1_MASK);
    P1IE |= PPM_P1_MASK;
    
    P2OUT &= ~(PPM_P2_MASK);
    P2SEL &= ~(PPM_P2_MASK);
    P2IFG = 0x00;
    P2IES &= ~(PPM_P2_MASK);
    P2IE |= PPM_P2_MASK;
}

// consagred
void PPM_refresh_channel(int channel, unsigned PPM_aux){
    if(PPM_capture_time[channel] > PPM_aux){ // se deu overflow na contagem do timer
        PPMResult[channel] = TBCCR0 - PPM_capture_time[channel] + PPM_aux;
    }
    else{ // se nao deu overflow na contagem do timer
        PPMResult[channel] = PPM_aux - PPM_capture_time[channel];
    }
    
    PPMChecker = 0; // atualiza checker na descida da borda
}

// consagred
interrupt (PORT1_VECTOR) PORT1_ISR_HOOK(void){
    
    unsigned int PPM_aux = TBR; // captura aqui pra ser mais exato
    // aqui eh usado PPM_P1_MASK, pq eh a interrupcao da P1 
    int channel_num = 0; // canal vai de 0 a 2 (0xE0)
    for(PPM_ch_counter = 0x20; PPM_ch_counter <= 0x80; PPM_ch_counter = PPM_ch_counter << 1){
        if(P1IFG & (PPM_ch_counter & PPM_P1_MASK)){
            if(!(P1IES & (PPM_ch_counter & PPM_P1_MASK))){ // low to high
                PPM_capture_time[channel_num] = PPM_aux;
                P1IES |= (PPM_ch_counter & PPM_P1_MASK); // configura high to low
            }
            else{ // high to low
                PPM_refresh_channel(channel_num, PPM_aux);
                P1IES &= ~(PPM_ch_counter & PPM_P1_MASK); // configure low to high      
            }
            P1IFG &= ~(PPM_ch_counter & PPM_P1_MASK); // apaga o interruption flag da P1.7
        }
        channel_num++;
    }
}

// consagred
interrupt (PORT2_VECTOR) PORT2_ISR_HOOK(void){
    
    unsigned PPM_aux = TBR; // captura aqui pra ser mais exato
    int channel_num = 3; // canal vai de 3 a 5
    for(PPM_ch_counter = 0x01; PPM_ch_counter <= 0x04; PPM_ch_counter = PPM_ch_counter << 1){
        if(P2IFG & (PPM_ch_counter & PPM_P2_MASK)){
            if(!(P2IES & (PPM_ch_counter & PPM_P2_MASK))){ // low to high
                PPM_capture_time[channel_num] = PPM_aux;
                P2IES |= (PPM_ch_counter & PPM_P2_MASK); // configura high to low
            }
            else{ // high to low
                PPM_refresh_channel(channel_num, PPM_aux);
                P2IES &= ~(PPM_ch_counter & PPM_P2_MASK); // configure low to high      
            }
            P2IFG &= ~(PPM_ch_counter & PPM_P2_MASK); // apaga o interruption flag da P1.7
        }
        channel_num++;
    } 
}
