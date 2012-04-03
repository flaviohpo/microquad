#ifndef __I2C_H
#define __I2C_H

// debug purposes
//#define __I2C_TEST

#define I2C_HIGH_SPEED
//#define I2C_LOW_SPEED
#define I2C_DEFAULT_ADDRESS     0x68

// special addresses
#define I2C_GENERAL_CALL_ADDR       0x00
#define I2C_CBUS_ADDR               0x01
#define I2C_RESERVED_ADDR           0x03
#define I2C_HIGH_SPEED_MST_ADDR1    0x02
#define I2C_HIGH_SPEED_MST_ADDR2    0x04
#define I2C_HIGH_SPEED_MST_ADDR3    0x05
#define I2C_HIGH_SPEED_MST_ADDR4    0x06
#define I2C_HIGH_SPEED_MST_ADDR5    0x07

#define I2C_SDA                 BIT1 // P3
#define I2C_SCL                 BIT2 // P3

// macros
#define I2C_PORTS_CFG()         (P3SEL |= (I2C_SDA | I2C_SCL))
#define I2C_START_TRANSMIT()    (UCB0CTL1 |= UCTR + UCTXSTT)
#define I2C_START_RECEIVE()     do{UCB0CTL1 &= ~UCTR; UCB0CTL1 |= UCTXSTT; }while(0)
#define I2C_GENERATE_STOP()     (UCB0CTL1 |= UCTR + UCTXSTP)
#define I2C_GENERATE_NACK()     (UCB0CTL1 |= BIT3)
#define I2C_GET_TXIFG()         (IFG2 & BIT3)
#define I2C_GET_RXIFG()         (IFG2 & BIT2)
#define I2C_SET_SLAVE_ADDR(x)   (UCB0I2CSA = x)
#define I2C_SET_OWN_ADDR(x)     (UCB0I2COA = x)
#define I2C_GET_NACK()          (UCB0STAT & BIT3)
#define I2C_FILL_TX_BUFF(x)     (UCB0TXBUF = x)
#define I2C_IS_BUSY()           (UCB0STAT & 0x10)

typedef enum{
    I2C_STAT_NONE,
    I2C_STAT_ACK,
    I2C_STAT_DATA_READY,
    I2C_STAT_DONE,
    I2C_STAT_IDLE,
    I2C_STAT_BUSY,
    I2C_STAT_FOUND,
    I2C_STAT_NOT_FOUND,
    I2C_STAT_FAIL,
    I2C_STAT_WRITE_TIMEOUT,
    I2C_STAT_READ_TIMEOUT,
    I2C_STAT_ADDR_NACK,
    I2C_STAT_DATA_NACK,
}I2C_STATUS;

I2C_STATUS i2c_init_module(void);
I2C_STATUS i2c_find_address(unsigned char SlaveAddress);
I2C_STATUS i2c_write_register(unsigned char Register, unsigned char Data, unsigned char SlaveAddress);
I2C_STATUS i2c_read_register(unsigned char Register, unsigned char *Data, unsigned char SlaveAddress);
I2C_STATUS i2c_read_byte(unsigned char SendNack, unsigned char SendStop, unsigned char* Data);
I2C_STATUS i2c_write_byte(unsigned char SendStop, unsigned char Data);

#endif // __I2C_H

