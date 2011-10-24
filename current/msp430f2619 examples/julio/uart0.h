#include <msp430x2619.h>

#define baud_9600		104
#define baud_19200		52
#define baud_38400		26
#define baud_57600		17
#define baud_115200		8
#define baud_230400		4

#define mod_9600		0x31
#define mod_19200		0x11
#define mod_38400		0x11
#define mod_57600		0x61
#define mod_115200		0x3b
#define mod_230400		0x27


void config_serial(unsigned char baud, unsigned char modul);
unsigned char receive_serial(void);
void send_serial(unsigned char c);


