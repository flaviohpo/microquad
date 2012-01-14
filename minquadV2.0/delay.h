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
    DELAY_STICK_INDEX        = 0,
    DELAY_SECONDS_INDEX     ,
    DELAY_LOW_BATTERY_BUZ   ,
    DELAY_FLIGHT_TIME       ,
    DELAY_BATTERY_CHECK     ,
    DELAY_BACKLED           ,
    DELAY_SIDELEDS          ,
    TIMEDELAY_LEN         
}DELAY_INDEX;

void delay2us(void);
void delay5us(void);
void delayus(unsigned int tempo);
void delayms(unsigned int tempo);

void delay_set(DELAY_INDEX index, unsigned int time);
unsigned char delay_poll(DELAY_INDEX index);
void delay_process(void);