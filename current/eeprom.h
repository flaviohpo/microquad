#ifndef __EEPROM
#define __EEPROM

//#define SIXTEEN_BIT_EEPROM
#define EEPROM_I2C_ADDR         0xA6 // 8bit address

#define _PITCH_P                0x00
#define _ROLL_P                 0x02
#define _YAW_P                  0x04

#define _PITCH_D                0x06
#define _ROLL_D                 0x08
#define _YAW_D                  0x0A

#define _PITCH_I                0x0C
#define _ROLL_I                 0x0E
#define _YAW_I                  0x10



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
