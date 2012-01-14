#ifndef __PPM_H
#define __PPM_H

extern uint16 PPMValue[8];
extern int16 PPMOffset[8];

void ppm_init(uchar InChannelMask, uchar OutChannelMask);
void ppm_set_all_outputs(unsigned int value);
void ppm_set_outputs(int16 OutputVector[]);
void ppm_set_output(uchar Num, uint16 Val);
void ppm_calibrate(void);

#endif // __PPM_H