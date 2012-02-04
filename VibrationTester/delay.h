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

typedef enum{
    TIMEDELAY_LEN         
}DELAY_INDEX;

extern volatile unsigned int TimeDelay[TIMEDELAY_LEN];

void delay2us(void);
void delay5us(void);
void delayus(unsigned int tempo);
void delayms(unsigned int tempo);
void set_delay(DELAY_INDEX index, unsigned int time);
unsigned char get_delay(DELAY_INDEX index);