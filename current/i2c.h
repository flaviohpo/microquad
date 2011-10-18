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

#define i2c_saida       0x06            // Para P3DIR  0000 0110 
#define i2c_entrada     0x04            // Para P3DIR  0000 0100 ou seje: 

#define SDA             BIT1
#define SCL             BIT2

#define SDA_OUT()       P3DIR |= (BIT2 + BIT1)      // Função que configura i2c como saída
#define SDA_IN()        P3DIR &= ~(BIT1)    // Função que configura i2c como entrada

#define sda_on          P3OUT |= SDA
#define sda_off         P3OUT &= ~SDA
#define scl_on          P3OUT |= SCL
#define scl_off         P3OUT &= ~SCL

#define sda_test        P3IN & SDA
//
// ****************************************************************
// ** P R O T Ó T I P O S  (PRIMITIVAS)                          **
// ****************************************************************
//
// Inicializa o barramento I2C e ajusta endereço do Slave Address
void i2c_config(unsigned char slave_addr);

//
// Troca endereço do Slave Address
void i2c_change_address(unsigned char slave_addr);

//
// Inicializa barramento do i2c (SDA=1, SCL=1)
void i2c_init(void);

//
// Start do i2c (para velocidade de 100KHz)
void i2c_start(void);

//
// Stop do i2c (para velocidade de 100KHz)
void i2c_stop(void);

//
// Le o estado do bit de ACK (para velocidade de 100KHz) :: 0 -> OK
unsigned char i2c_recv_ack(void);

//
// Envia bit de ACK ou de Not ACK (para velocidade de 100KHz)
void i2c_send_ack(unsigned char confirmation);

//
// Envia 8 bits de dados pelo i2c (para velocidade de 100KHz)
void i2c_bwrite(unsigned char b);

//
// Recebe 8 bitS de dados pelo i2c (para velocidade de 100KHz)
unsigned char i2c_bread(void);


//
// ****************************************************************
// ** P R O T Ó T I P O S  (DERIVADAS)                           **
// ****************************************************************
//
// Pesquisa por um dispositivo (i2c_config --> Slave Address ) :: 0 - Encontrou!!!
unsigned char i2c_find_device(void);

//
// Escreve um byte no i2c (start + dado + stop + recv_ack)
unsigned char i2c_write_dummy(unsigned char b);

//
// Escreve um byte no i2c (start + endereco8 + dado + stop + recv_ack)
unsigned char i2c_write_byte(unsigned char reg, unsigned char dat);

//
// Escreve multiplos byte no i2c (start + endereco8 + dado1..dado_n + stop + recv_ack)
unsigned char i2c_write_multiples(unsigned char reg, unsigned char dat[], unsigned char len);

//
// Escreve um byte no i2c (start + endereco16 + dado + stop + recv_ack)
unsigned char i2c_write16_byte(unsigned int reg, unsigned char dat);

//
// Escreve multiplos byte no i2c (start + endereco16 + dado1..dado_n + stop + recv_ack)
unsigned char i2c_write16_multiples(unsigned int reg, unsigned char dat[], unsigned char len);

//
// Le um byte no i2c (start + endereco8 + dado + stop + recv_ack)
unsigned char i2c_read_byte(unsigned char reg, unsigned char *dat);

//
// Escreve multiplos byte no i2c (start + endereco8 + dado1..dado_n + stop + recv_ack)
unsigned char i2c_read_multiples(unsigned char reg, unsigned char dat[], unsigned char len);

//
// Le um byte no i2c (start + endereco16 + dado + stop + recv_ack)
unsigned char i2c_read16_byte(unsigned int reg, unsigned char *dat);

//
// Escreve multiplos byte no i2c (start + endereco16 + dado1..dado_n + stop + recv_ack)
unsigned char i2c_read16_multiples(unsigned int reg, unsigned char dat[], unsigned char len);

