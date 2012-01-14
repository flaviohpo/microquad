#ifndef __EEPROM
#define __EEPROM

//#define SIXTEEN_BIT_EEPROM
#define EEPROM_I2C_ADDR         0xAE // 8bit address

#define ADDR_VALIDATION         0x00
#define ADDR_FLIGHT_TIME        0x02
                                
#define ADDR_P_PITCH_MUL        0x04
#define ADDR_P_ROLL_MUL         0x06
#define ADDR_P_YAW_MUL          0x08    
                                
#define ADDR_P_PITCH_DIV        0x0A
#define ADDR_P_ROLL_DIV         0x0C
#define ADDR_P_YAW_DIV          0x0E

#define ADDR_REF_PITCH_MUL      0x10
#define ADDR_REF_ROLL_MUL       0x12
#define ADDR_REF_YAW_MUL        0x14

#define ADDR_REF_PITCH_DIV      0x16
#define ADDR_REF_ROLL_DIV       0x18
#define ADDR_REF_YAW_DIV        0x1A

typedef union{
    float f;
    char c[4];
}fourbytes;

typedef union{
    int i;
    unsigned int ui;
    char c[2];
    unsigned char uc[2];
}twobytes;

extern unsigned char EEPROMFound;

#endif //__EEPROM
