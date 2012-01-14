#ifndef __TIMER_H
#define __TIMER_H

#define TIMER_B_LIMIT               4999
#define TIMER_A_LIMIT               999

#define get_ppm_timer_count()       TBR
#define get_bright_timer_count()    TAR
#define get_lcd_bright()            (((TAR>>3)*100)/(TIMER_A_LIMIT>>3))
#define set_lcd_bright(x)           (constrain(TACCR1, 0, TIMER_A_LIMIT)) 

#define set_ppm_out_1(x)            (constrain(TBCCR1, 0, TIMER_B_LIMIT))
#define set_ppm_out_2(x)            (constrain(TBCCR2, 0, TIMER_B_LIMIT))
#define set_ppm_out_3(x)            (constrain(TBCCR3, 0, TIMER_B_LIMIT))
#define set_ppm_out_4(x)            (constrain(TBCCR4, 0, TIMER_B_LIMIT))
#define set_ppm_out_5(x)            (constrain(TBCCR5, 0, TIMER_B_LIMIT))
#define set_ppm_out_6(x)            (constrain(TBCCR6, 0, TIMER_B_LIMIT))

void timer_set_interrupts(void(*func_timer_A)(void), void(*func_timer_B)(void));
void timer_set_interrupt_A(void(*func_timer_A)(void));
void timer_set_interrupt_B(void(*func_timer_B)(void));
void timer_init(void);

#endif // __TIMER_H
