#ifndef __PPM
#define __PPM

#define PPM_P2_MASK (0x07)
#define PPM_P1_MASK (0xFF)

extern volatile float PPMResult[6];
extern volatile int PPMChecker;

void ppm_init(void);

#endif // __PPM
