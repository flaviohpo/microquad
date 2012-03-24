#ifndef __RADIO_H
#define __RADIO_H

#include "shorttypes.h"

#define PPM_P1MASK              0xFF

extern int16 PPMOffset[8];
extern int16 PPMValue[8];
extern int16 PPMRaw[8];

void calibrate_radio(void);

#endif // __RADIO_H
