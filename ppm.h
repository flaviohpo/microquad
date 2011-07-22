#ifndef __PPM
#define __PPM

//#define PPM_P2_MASK (0x00)
#define PPM_P1_MASK (0xFD)
#define PPM_NUMCH 7

extern volatile int PPMResult[PPM_NUMCH];

void ppm_init(void);

#endif // __PPM
