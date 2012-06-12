#include "msp430f2618.h"
#include "legacymsp430.h"
#include "delay.h"

unsigned volatile int TimeDelay[TIMEDELAY_LEN];

void delay2us(void)
{
  unsigned int tempo = 6;

  do (tempo--);
  while (tempo != 0);
}

void delay5us(void)
{
  unsigned int tempo = 17;

  do (tempo--);
  while (tempo != 0);
}

// T = 0.2923us * tempo (aproximadamente...)
//
void delayus(unsigned int tempo)
{
  int i;
    
  for(i = 0; i < tempo; i++) nop();
}

// Aproximadamente 1ms...
//
void delayms(unsigned int tempo)
{
  int i;
    
  for(i = 0; i < tempo; i++) delayus(3420);
}

void set_delay(DELAY_INDEX index, unsigned int time){
    TimeDelay[index]=time;
}

unsigned char get_delay(DELAY_INDEX index){
    if(TimeDelay[index] == 0){
        return 1;
    }
    return 0;
}


// delay
interrupt (TIMERA1_VECTOR) TIMERA1_ISR_HOOK(void){ // 2ms
    int i = 0;
    i = TIMEDELAY_LEN;
           
    do{
        i--;
        if(TimeDelay[i] > 0){
            TimeDelay[i]--;
        }
    }while(i);
    // ver no datasheet se tem q zerar o flag da interrupt
    TACTL &= ~0x01;
}


