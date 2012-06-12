#include "i2c.h"
#include "delay.h"
#include "msp430f2618.h"

// inicializa I2C na UCB0
// P3.2 e P3.1
I2C_STATUS i2c_init_module(void){   
    // desable i2c module
    UCB0CTL1 = UCSWRST;
    // config ports
    I2C_PORTS_CFG();
    // clock MSCLK
    UCB0CTL0 = UCMST + UCMODE_3 + UCSYNC;    
    // I2CMODE
    UCB0CTL1 = UCSSEL1 | UCSWRST;
    // baudrate
    // fbaud = FBRclk / UCBRx
    // 0x640 = 10kbps
    // 0xA0 = 100kbps
    // 0x28 = 400kbps
#ifdef I2C_LOW_SPEED // 10k
    UCB0BR0 = 0x40;
    UCB0BR1 = 0x06;
#else
 #ifdef I2C_HIGH_SPEED // 400k
    UCB0BR0 = 0xA0;
    UCB0BR1 = 0x00;
 #else // 100k
    UCB0BR0 = 0x28
    UCB0BR1 = 0x00;
 #endif
#endif
    // libera o reset
    UCB0CTL1 &= ~UCSWRST;

    if(I2C_IS_BUSY()){
        I2C_SET_SLAVE_ADDR(0);
        I2C_START_TRANSMIT();
        I2C_GENERATE_NACK();
        I2C_GENERATE_STOP();
    }
    
#ifdef __I2C_TEST
    delayms(10);
    
    UCB0I2CSA = 0x68;
    I2C_START_TRANSMIT();
    I2C_GENERATE_STOP();
#endif // __I2C_TEST

    return I2C_STAT_DONE;
}

I2C_STATUS i2c_find_address(unsigned char SlaveAddress){
    // seta slave address
    I2C_SET_SLAVE_ADDR(SlaveAddress);
    // start condition + slave address
    I2C_START_TRANSMIT();
    I2C_GENERATE_STOP();
    // se deu ack
    if(I2C_GET_NACK()){
        return I2C_STAT_NOT_FOUND;
    }
    return I2C_STAT_FOUND;
}

I2C_STATUS i2c_write_register(unsigned char Register, unsigned char Data, unsigned char SlaveAddress){
    I2C_STATUS Result = I2C_STAT_NONE;
    
    if(UCB0CTL1 & UCTXSTP){
        return I2C_STAT_BUSY;
    }
    
    I2C_SET_SLAVE_ADDR(SlaveAddress);
    I2C_START_TRANSMIT();
    
    Result = i2c_write_byte(0, Register);
    if(Result == I2C_STAT_DONE){
        Result = i2c_write_byte(1, Data);
    }

    return Result;
}

I2C_STATUS i2c_read_register(unsigned char Register, unsigned char *Data, unsigned char SlaveAddress){
    I2C_STATUS Result = I2C_STAT_NONE;
    
    // set slave address
    I2C_SET_SLAVE_ADDR(SlaveAddress);
    
    // send address + write
    I2C_START_TRANSMIT();
    
    // write register
    Result = i2c_write_byte( 0, Register);
    
    // if wrote register ok
    if(Result == I2C_STAT_DONE){
        // send address + read
        I2C_START_RECEIVE();
        
        // read one byte send nack and stop
        Result = i2c_read_byte(1, 1, Data);
    }

    return Result;
}

I2C_STATUS i2c_read_byte(unsigned char SendNack, unsigned char SendStop, unsigned char* Data){
    while(!I2C_GET_TXIFG());
    while(!I2C_GET_RXIFG());
    *Data = UCB0RXBUF;
    if(SendNack){
        I2C_GENERATE_NACK();
    }
    if(SendStop){
        I2C_GENERATE_STOP();
    }
    return I2C_STAT_DONE;
}

I2C_STATUS i2c_write_byte(unsigned char SendStop, unsigned char Data){
    while(!I2C_GET_TXIFG());
    UCB0TXBUF = Data;
    while(!I2C_GET_TXIFG());
    if(I2C_GET_NACK()){
        return I2C_STAT_DATA_NACK;
    }

    if(SendStop){
        I2C_GENERATE_STOP();
    }
    return I2C_STAT_DONE;
}

