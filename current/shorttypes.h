//SHORT DEF TYPES
#ifndef SHORT_TYPES
#define SHORT_TYPES

typedef unsigned char uchar;
typedef unsigned char uint8;
typedef unsigned int uint16;
typedef int int16;

typedef union{
    unsigned char BYTE;
    struct{
        unsigned char bit0 : 1;
        unsigned char bit1 : 1;
        unsigned char bit2 : 1;
        unsigned char bit3 : 1;
        unsigned char bit4 : 1;
        unsigned char bit5 : 1;
        unsigned char bit6 : 1;
        unsigned char bit7 : 1;
    }BITS;
}byte;

#endif
