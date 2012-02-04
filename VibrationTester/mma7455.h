#ifndef __MMA7455
#define __MMA7455

#define MMA7455_I2C_ADDR 0x3a // address with A0 = GND
// 10 bits reading
#define ACC_XOUTL   0x00
#define ACC_XOUTH   0x01
#define ACC_YOUTL   0x02
#define ACC_YOUTH   0x03
#define ACC_ZOUTL   0x04
#define ACC_ZOUTH   0x05
// 8 bits reading
#define ACC_XOUT8   0x06
#define ACC_YOUT8   0x07
#define ACC_ZOUT8   0x08
// status
#define ACC_STATUS  0x09
#define ACC_DETSRC  0x0A
#define ACC_TOUT    0x0B
#define ACC_I2CAD   0x0D
#define ACC_USRINF  0x0E
#define ACC_WHOAMI  0x0F
// offsets  
#define ACC_XOFFL   0x10
#define ACC_XOFFH   0x11
#define ACC_YOFFL   0x12
#define ACC_YOFFH   0x13
#define ACC_ZOFFL   0x14
#define ACC_ZOFFH   0x15
// control
#define ACC_MCTL    0x16 // | 7:Reserved | 6:DataReadStatusOutputToINT1 | 5:SPI3Wire | 4:SelfTestEnable | 3:Gsel[1] | 2:Gsel[0] | 1:Mode[1] | 0:Mode[0] |
#define ACC_INTRST  0x17
#define ACC_CTL1    0x18
#define ACC_CTL2    0x19
#define ACC_LDTH    0x1A
#define ACC_PDTH    0x1B
#define ACC_PW      0x1C
#define ACC_LT      0x1D
#define ACC_TW      0x1E

#endif //__MMA7455
