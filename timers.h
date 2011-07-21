#ifndef __TIMERS
#define __TIMERS

extern volatile unsigned long MILLISECONDS;
extern unsigned long TIMER_MS;
extern unsigned long TIMER_MS_OLD;

void TMRB_init(unsigned int period); // envia o periodo de contagem do timer junto
void TMRA_init(void);

#endif /* __TIMERS */
