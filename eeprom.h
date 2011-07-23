#ifndef __EEPROM
#define __EEPROM

#define EEPROM_I2C_ADDR 0xA6

// transmitter data
#define _RADIO_ROLL_SLOPE 0x0003
#define _RADIO_PITCH_SLOPE 0x0007
#define _RADIO_YAW_SLOPE 0x000C
#define _RADIO_THROTTLE_SLOPE 0x0010
#define _RADIO_CH5_SLOPE 0x0014
#define _RADIO_CH6_SLOPE 0x0018

#define _RADIO_ROLL_OFFSET 0x002C
#define _RADIO_PITCH_OFFSET 0x0030
#define _RADIO_YAW_OFFSET 0x0034
#define _RADIO_THROTTLE_OFFSET 0x0038
#define _RADIO_CH5_OFFSET 0x003C
#define _RADIO_CH6_OFFSET 0x0040

#define _PITCH_P 0x0044
#define _PITCH_I 0x0048
#define _PITCH_D 0x004C

#define _ROLL_P 0x0050
#define _ROLL_I 0x0054
#define _ROLL_D 0x0058

#define _YAW_P 0x005C
#define _YAW_I 0x0060
#define _YAW_D 0x0064

typedef union{
    float f;
    char c[4];
}fourbytes;

extern unsigned char EEPROMFound;

#endif //__EEPROM
