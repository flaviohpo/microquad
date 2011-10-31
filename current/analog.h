#ifndef __ANALOG
#define __ANALOG

#define ANALOG_ENABLE 0xFF

extern int AnalogOffset[8];
extern int AnalogValue[8];

void analog_init(void);
void analog_calibrate_channel(int channel);
void analog_refresh_all(void);
void analog_refresh_channel(int channel);
     
#endif /* __ANALOG */
