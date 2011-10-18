#ifndef __EEPROM
#define __EEPROM

#define EEPROM_I2C_ADDR         0x00A6 // 8bit address

#define _FLIGHT_MODE            0x0020

#define _PITCH_P                0x0100
#define _ROLL_P                 0x0102
#define _YAW_P                  0x0104

#define _PITCH_D                0x0106
#define _ROLL_D                 0x0108
#define _YAW_D                  0x010A

#define _PITCH_I                0x010C
#define _ROLL_I                 0x010E
#define _YAW_I                  0x0110



typedef union{
    float f;
    char c[4];
}fourbytes;

typedef union{
    int i;
    char c[2];
    unsigned char uc[2];
}twobytes;

extern unsigned char EEPROMFound;

#endif //__EEPROM
