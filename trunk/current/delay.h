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

#define TIMEDELAY_LEN           15

#define DELAY_STICK_INDEX       0
#define DELAY_SECONDS_INDEX     1
#define DELAY_LOW_BATTERY_BUZ   2
#define DELAY_FLIGHT_TIME       3
#define DELAY_BATTERY_CHECK     4

extern volatile unsigned int TimeDelay[TIMEDELAY_LEN];

void delay2us(void);
void delay5us(void);
void delayus(unsigned int tempo);
void delayms(unsigned int tempo);
void set_delay(unsigned int position, unsigned int time);
unsigned char get_delay(unsigned int position);