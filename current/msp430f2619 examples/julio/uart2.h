#define baud_9600		104
#define baud_19200		52
#define baud_38400		26
#define baud_57600		17
#define baud_115200		8
#define baud_230400		4

void comport_init(unsigned char baud);
unsigned char comport_getc(void);
void comport_putc(unsigned char c);
unsigned char comport_rxfull(void);


