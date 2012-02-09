//
// **** i2c.h *****************************************************
// **    2                                                       **
// **  I   C                                                     **
// **                                    (C) by JCLima, 2011/1   **
// **                                                            **
// ****************************************************************
//

//
// ****************************************************************
// ** D E F I N I Ç Õ E S                                        **
// ****************************************************************
//
#define ACK                    0
#define NACK                   255

//#define i2c_saida       0x06            // Para P3DIR  0000 0110 
#define i2c_entrada     0x04            // Para P3DIR  0000 0100 ou seje: 

#define SDA_OUT()       P3DIR |= (BIT2 + 0x02)      // Função que configura i2c como saída
#define SDA_IN()        P3DIR &= ~(0x02)    // Função que configura i2c como entrada

#define sda_on          P3OUT |= 0x02
#define sda_off         P3OUT &= ~0x02
#define scl_on          P3OUT |= 0x04
#define scl_off         P3OUT &= ~0x04

#define sda_test        P3IN & 0x02
//
// ****************************************************************
// ** P R O T Ó T I P O S  (PRIMITIVAS)                          **
// ****************************************************************
//
// Inicializa o barramento I2C e ajusta endereço do Slave Address
void i2c_config( char slave_addr);

//
// Troca endereço do Slave Address
void i2c_change_address( char slave_addr);

//
// Inicializa barramento do i2c (0x02=1, 0x04=1)
void i2c_init(void);

//
// Start do i2c (para velocidade de 100KHz)
void i2c_start(void);

//
// Stop do i2c (para velocidade de 100KHz)
void i2c_stop(void);

//
// Le o estado do bit de ACK (para velocidade de 100KHz) :: 0 -> OK
 char i2c_recv_ack(void);

//
// Envia bit de ACK ou de Not ACK (para velocidade de 100KHz)
void i2c_send_ack( char confirmation);

//
// Envia 8 bits de dados pelo i2c (para velocidade de 100KHz)
void i2c_bwrite( char b);

//
// Recebe 8 bitS de dados pelo i2c (para velocidade de 100KHz)
 char i2c_bread(void);


//
// ****************************************************************
// ** P R O T Ó T I P O S  (DERIVADAS)                           **
// ****************************************************************
//
// Pesquisa por um dispositivo (i2c_config --> Slave Address ) :: 0 - Encontrou!!!
 char i2c_find_device(void);

//
// Escreve um byte no i2c (start + dado + stop + recv_ack)
 char i2c_write_dummy( char b);

//
// Escreve um byte no i2c (start + endereco8 + dado + stop + recv_ack)
 char i2c_write_byte( char reg,  char dat);

//
// Escreve multiplos byte no i2c (start + endereco8 + dado1..dado_n + stop + recv_ack)
 char i2c_write_multiples( char reg,  char dat[],  int len);

//
// Escreve um byte no i2c (start + endereco16 + dado + stop + recv_ack)
 char i2c_write16_byte( int reg,  char dat);

//
// Escreve multiplos byte no i2c (start + endereco16 + dado1..dado_n + stop + recv_ack)
 char i2c_write16_multiples( int reg,  char dat[],  int len);

//
// Le um byte no i2c (start + endereco8 + dado + stop + recv_ack)
 char i2c_read_byte( char reg,  char *dat);

//
// Escreve multiplos byte no i2c (start + endereco8 + dado1..dado_n + stop + recv_ack)
 char i2c_read_multiples( char reg,  char dat[],  int len);

//
// Le um byte no i2c (start + endereco16 + dado + stop + recv_ack)
 char i2c_read16_byte( int reg,  char *dat);

//
// Escreve multiplos byte no i2c (start + endereco16 + dado1..dado_n + stop + recv_ack)
 char i2c_read16_multiples( int reg,  char dat[],  int len);

