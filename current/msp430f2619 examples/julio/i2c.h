//
// ************************************************************************
// **                                                                    **
// **  BIBLIOTECA DE I2C                                                 **
// **                                                                    **
// **                                                                    **
// ************************************************************************
//
#define NACK  255
#define ACK     0


// ** PROTOTIPOS...
//
void i2c_init(void);
void i2c_start(void);
void i2c_stop(void);
unsigned char i2c_write(unsigned char b);
unsigned char i2c_read(unsigned char confirmation);
unsigned char hi(unsigned int addr);
unsigned char lo(unsigned int addr);
unsigned char mem_found(unsigned char memoria);
unsigned char mem_bytewrite(unsigned char memoria, unsigned int addr, unsigned char dat);
unsigned char mem_blockwrite(unsigned char memoria, unsigned int addr, unsigned char *p, unsigned char n);
unsigned char mem_byteread(unsigned char memoria, unsigned int addr, unsigned char * dat);
unsigned char mem_blockread(unsigned char memoria, unsigned int addr, unsigned char *p, unsigned char n);

