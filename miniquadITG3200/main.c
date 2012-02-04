#include "hardware_config.h"
#include "i2c.h"
#include "miniquad.h"
#include "misc.h"
#include "resources.h"
#include "lcd6100.h"
#include "stdio.h"
#include "analog.h"
#include "msp430f2618.h"
#include "delay.h"

#define I2C_INTERVAL    1

/*
ITG3200 config:
8kHz
256Hz lowpass
*/

int main(void){
    unsigned char ITG3200_I2CAddress = 0x68;
    unsigned char EEPROM_I2CAddress = 0x50;
    
    unsigned char Data = 0;

    hardware_setup();
    
    lcd_init(LCDBackColor);
    

    // config ITG3200
    delayms(150);
    i2c_write_register(0x17, 0x05, ITG3200_I2CAddress);
    delayms(100);
    i2c_write_register(0x3E, 0x01, ITG3200_I2CAddress);
    delayms(100);
    i2c_write_register(0x16, 0x18, ITG3200_I2CAddress);
    delayms(100);
    i2c_write_register(0x15, 0x00, ITG3200_I2CAddress);
    delayms(100);
    
    // i2c test
    while(1){
        lcd_goto(0,0);
        printf("%X\n", UCB0STAT);        
        delayms(1);
        
        i2c_read_register(0, &Data, ITG3200_I2CAddress);
        printf("00: %02X\n", Data);
        delayms(I2C_INTERVAL);
        
        i2c_read_register(21, &Data, ITG3200_I2CAddress);
        printf("21: %02X\n", Data);
        delayms(I2C_INTERVAL);
        
        i2c_read_register(22, &Data, ITG3200_I2CAddress);
        printf("22: %02X\n", Data);
        delayms(I2C_INTERVAL);
        
        i2c_read_register(23, &Data, ITG3200_I2CAddress);
        printf("23: %02X\n", Data);
        delayms(I2C_INTERVAL);
    
        i2c_read_register(26, &Data, ITG3200_I2CAddress);
        printf("26: %02X\n", Data);
        delayms(I2C_INTERVAL);
        
        i2c_read_register(27, &Data, ITG3200_I2CAddress);
        printf("27: %02X\n", Data);
        delayms(I2C_INTERVAL);
        
        i2c_read_register(28, &Data, ITG3200_I2CAddress);
        printf("28: %02X\n", Data);
        delayms(I2C_INTERVAL);
        
        i2c_read_register(29, &Data, ITG3200_I2CAddress);
        printf("29: %02X\n", Data);
        delayms(I2C_INTERVAL);
        
        i2c_read_register(30, &Data, ITG3200_I2CAddress);
        printf("30: %02X\n", Data);
        delayms(I2C_INTERVAL);
        
        i2c_read_register(31, &Data, ITG3200_I2CAddress);
        printf("31: %02X\n", Data);
        delayms(I2C_INTERVAL);
        
        i2c_read_register(32, &Data, ITG3200_I2CAddress);
        printf("32: %02X\n", Data);
        delayms(I2C_INTERVAL);
        
        i2c_read_register(33, &Data, ITG3200_I2CAddress);
        printf("33: %02X\n", Data);
        delayms(I2C_INTERVAL);
        
        i2c_read_register(34, &Data, ITG3200_I2CAddress);
        printf("34: %02X\n", Data);
        delayms(I2C_INTERVAL);
        
        i2c_read_register(62, &Data, ITG3200_I2CAddress);
        printf("62: %02X\n", Data);
        delayms(I2C_INTERVAL);
        
    }
}
