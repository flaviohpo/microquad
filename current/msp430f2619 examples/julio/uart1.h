
#define baud_9600		104
#define baud_19200		52
#define baud_38400		26
#define baud_57600		17
#define baud_115200		8
#define baud_230400		4

unsigned char comport_inputcount(void);
void comport_init(unsigned int baud);
void comport_flush(void);
void comport_putc(unsigned char c);
unsigned char comport_getc(void);



//
// Pooling Functions...
//
void comport_sendchar(unsigned char c);
unsigned char comport_recvchar(void);


