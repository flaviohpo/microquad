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

extern volatile unsigned int TimeDelay[TIMEDELAY_LEN];

void delay2us(void);
void delay5us(void);
void delayus(unsigned int tempo);
void delayms(unsigned int tempo);
void set_delay(unsigned int position, unsigned int time);
unsigned char get_delay(unsigned int position);