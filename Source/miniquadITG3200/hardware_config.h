#ifndef __HARDWARE_CONFIG_H
#define __HARDWARE_CONFIG_H

#define SMCLK           16000000l
#define ACLK            16000000l
#define MCLK            16000000l

#define ITG_INT         BIT0 // P2

#define LCD_BACKLIGHT_PIN       BIT3
#define LCD_BACKLIGHT_OUT()     (P2DIR |= LCD_BACKLIGHT_PIN)
#define SET_LCD_BACKLIGHT()     (P2OUT |= LCD_BACKLIGHT_PIN)

void clock_init(void);
void hardware_setup(void);

#endif //__HARWARE_CONFIG_H
