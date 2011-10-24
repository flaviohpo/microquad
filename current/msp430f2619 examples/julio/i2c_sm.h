//
// ****************************************************************
// **    2                                                       **
// **  I   C                                                     **
// **                                    (C) by JCLima, 2010/1   **
// **                                                            **
// ****************************************************************
//

// Definições...
//

#define             ACK         0
#define             NACK        0xff


// Protótipos...
//
void i2c_config(unsigned char slave_add, unsigned char speed);
void i2c_change_slaveaddress(unsigned char slave_add);
void i2c_start_tx(void);
void i2c_start_rx(void);
void i2c_stop_tx(void);
void i2c_stop_rx(void);
unsigned char i2c_test(void);
unsigned char i2c_send(unsigned char b);

unsigned char i2c_write_dummy(void);
unsigned char i2c_write_one(unsigned char b1);
unsigned char i2c_write_one_data(unsigned char b1, unsigned char dat);
unsigned char i2c_write_one_buffer(unsigned char b1, unsigned char dat[], unsigned char len);
unsigned char i2c_write_two(unsigned char b1, unsigned char b2);
unsigned char i2c_write_two_data(unsigned char b1, unsigned char b2, unsigned char dat);
unsigned char i2c_write_two_buffer(unsigned char b1, unsigned char b2, unsigned char dat[], unsigned char len);
unsigned char i2c_write_buffer(unsigned char dat[], unsigned char len);

unsigned char i2c_wread_one_data(unsigned char b1, unsigned char *dat);
unsigned char i2c_wread_one_buffer(unsigned char b1, unsigned char dat[], unsigned char len);
unsigned char i2c_wread_two_data(unsigned char b1, unsigned char b2, unsigned char *dat);
unsigned char i2c_wread_two_buffer(unsigned char b1, unsigned char b2, unsigned char dat[], unsigned char len);

unsigned char i2c_read_data(unsigned char *dat);
unsigned char i2c_read_buffer(unsigned char dat[], unsigned char len);

unsigned char i2c_find_device(unsigned char address);

