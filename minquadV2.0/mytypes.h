//SHORT DEF TYPES0
#ifndef __MY_TYPES_H
#define __MY_TYPES_H

typedef unsigned char uchar;
typedef unsigned char uint8;
typedef unsigned int uint16;
typedef int int16;

// BOOL TYPEDEF
#ifndef __BOOL
#define __BOOL
#define FALSE 0
#define false FALSE
#define TRUE 1
#define true TRUE
typedef unsigned char bool;
#endif // __BOOL

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

#endif // __MY_TYPES_H
