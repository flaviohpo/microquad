//
// ****************************************************************
// **                                                            **
// **  LABORATORIO DE PROCESSADORES I (2010/I)                   **
// **                                                            **
// **  Rotinas de delay por software (@16MHz)                    **
// **                                                            **
// **                                    (C) by JCLima, 2010/1   **
// **                                                            **
// ****************************************************************
//
#include "msp430f2618.h"
#include "timer.h"
#include "delay.h"

#define DELAY_500US_SIGN    (TIMEDELAY_LEN-1)
#define DELAY_1MS_SIGN      (TIMEDELAY_LEN)

unsigned volatile int TimeDelay[TIMEDELAY_LEN];
unsigned int DelayVectorIndex = TIMEDELAY_LEN;

void delay2us(void){
  unsigned int tempo = 6;

  do (tempo--);
  while (tempo != 0);
}

void delay5us(void){
  unsigned int tempo = 17;

  do (tempo--);
  while (tempo != 0);
}

// T = 0.2923us * tempo (aproximadamente...)
//
void delayus(unsigned int tempo){
  int i;
    
  for(i = 0; i < tempo; i++) nop();
}

// Aproximadamente 1ms...
//
void delayms(unsigned int tempo){
  int i;
    
  for(i = 0; i < tempo; i++) delayus(3420);
}

void delay_set(DELAY_INDEX index, unsigned int time){
    TimeDelay[index]=time;
}

unsigned char delay_poll(DELAY_INDEX index){
    if(TimeDelay[index] == 0){
        return 1;
    }
    return 0;
}

// esta funcao deve ser chamada de 500 em 500us
inline void delay_process(void){
    if(DelayVectorIndex == DELAY_500US_SIGN){
        // faz alguma coisa com 500us
        DelayVectorIndex = DELAY_1MS_SIGN;
    }
    else{
        do{
            DelayVectorIndex--;
            if(TimeDelay[DelayVectorIndex] > 0){
                TimeDelay[DelayVectorIndex]--;
            }
        }while(DelayVectorIndex);
           
        DelayVectorIndex = DELAY_500US_SIGN;
    }    
}
    