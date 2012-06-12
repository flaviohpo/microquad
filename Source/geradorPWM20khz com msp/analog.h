#ifndef __ANALOG
#define __ANALOG

#include "shorttypes.h"

#define ANALOG_ENABLE 0xFF

extern int16 AnalogOffset[8];
extern int16 AnalogValue[8];

void analog_init(void);
void analog_calibrate_channel(int channel);
void analog_refresh_all(void);
void analog_refresh_channel(int channel);
     
#endif /* __ANALOG */
