//
// ****************************************************************
// **                                                            **
// **  LABORATORIO DE PROCESSADORES I (2010/I)                   **
// **                                                            **
// **  Esta biblioteca implementa as rotinas de configuração     **
// **  do MSP430.                                                **
// **                                                            **
// **                                    (C) by JCLima, 2010/1   **
// **                                                            **
// ****************************************************************
//
#include <msp430x261x.h>

//
// CONFIGURAÇÃO DA PORTA P1 (SOMENTE I/O)
//  
//    0    0     0     0     0     0     0     0    <---- P1SEL: 0x00
//    1    1     1     1     0     0     0     0    <---- P1DIR: 0xf0
// +----+-----+-----+-----+-----+-----+-----+-----+
// | CS | CLK | DIN | RST | SW4 | SW3 | SW2 | SW1 | P1
// +----+-----+-----+-----+-----+-----+-----+-----+
// |                      |                       |
//  \---------v----------/ \----------v----------/
//      DISPLAY GRAFICO            TECLADO
//
//
#define p1_como_io      0x00            // Para P1SEL
#define p1_disp_teclado 0xf0            // Para P1DIR

#define sw1             port1.in.pin0
#define sw2             port1.in.pin1
#define sw3             port1.in.pin2
#define sw4             port1.in.pin3

// Display Gráfico Monocromático
//
#define lcd_reset       port1.out.pin4
#define lcd_din         port1.out.pin5
#define lcd_sck         port1.out.pin6
#define lcd_cs          port1.out.pin7

// Display Gráfico Color
//
#define RESX            port1.out.pin4     
#define SDA             port1.out.pin5     
#define SCLK            port1.out.pin6     
#define CSX             port1.out.pin7     

//
// CONFIGURAÇÃO DA PORTA P2
//
// Esta porta admite várias configurações. Foram definidas três configurações
// básicas:
//           P2 só como I/O e todos os pinos são saída (default)
//           P2 só como I/O e todos os pinos são entrada
//           P2 acessando periféricos: ACLK
//  
//     0      0      0      0      0      0      0      0    <---- P2SEL: 0x00
//     1      1      1      1      1      1      1      1    <---- P2DIR: 0xff
// +------+------+------+------+------+------+------+------+
// | P2.7 | P2.6 | P2.5 | P2.4 | P2.3 | P2.2 | P2.1 | P2.0 | P2
// +------+------+------+------+------+------+------+------+
//                                                    ACLK
//
#define p2_ver_aclk     0x01    // Para P2SEL
#define p2_como_io      0x00    // Para P2SEL
#define p2_saidas       0xff    // Para P2DIR
#define p2_entradas     0x00    // Para P2DIR

//
// CONFIGURAÇÃO DA PORTA P3
//  
// Esta porta admite várias configurações. Foi definida a configuração para
// acessar os periféricos de I2C e da UCSI configurada como UART.
//
//    0    0     1     1     0       1     1     0    <---- P3SEL: 0x36
//    1    1     0     1     0       1    1/0    1    <---- P3DIR: 0xd5 ou 0xd7
// +----+----+-----+-----+--------+-----+-----+-----+
// | EN | RS | RxD | TxD | INSERT | SCL | SDA | LED | P3
// +----+----+-----+-----+--------+-----+-----+-----+
// |         |           |   |    |           |  |
//  \---v---/ \----v----/    v    \-----v----/   v
//     LCD       UART      SMART      PORTA     LED
//                         CARD        I2C
//
#define p3_uart_i2c     0x36            // Para P3SEL
#define p3_uart         0x30            // Para P3SEL
#define p3_como_io      0x00            // Para P3SEL
#define i2c_entrada     0xd5            // Para P3DIR
#define i2c_saida       0xd7            // Para P3DIR

#define led             port3.out.pin0    
#define sda_in          port3.in.pin1
#define sda_out         port3.out.pin1
#define scl             port3.out.pin2
#define insert          port3.in.pin3
#define txd             port3.out.pin4
#define rxd             port3.in.pin5
#define lcd_rs          port3.out.pin6
#define lcd_en          port3.out.pin7

#define SDA_OUT()       P3DIR = 0xd7    // Função que configura i2c como saída
#define SDA_IN()        P3DIR = 0xd5    // Função que configura i2c como entrada

//
// CONFIGURAÇÃO DA PORTA P4 (SOMENTE I/O)
//
// Esta porta admite várias configurações, porém será usada apenas como
// saída para controle do barramento de dados do LCD alfanumérico.
//
//     0      0      0      0      0      0      0      0    <---- P4SEL: 0x00
//     1      1      1      1      1      1      1      1    <---- P4DIR: 0xff
// +------+------+------+------+------+------+------+------+
// | P4.7 | P4.6 | P4.5 | P4.4 | P4.3 | P4.2 | P4.1 | P4.0 | P4
// +------+------+------+------+------+------+------+------+
//
//
#define p4_como_io      0x00    // Para P4SEL
#define p4_saidas       0xff    // Para P4DIR

#define lcd_data        P4OUT


//
// CONFIGURAÇÃO DA PORTA P5
//
// Esta porta admite várias configurações. Foram definidas três configurações
// básicas:
//           P5 só como I/O e todos os pinos são saída (default)
//           P5 só como I/O e todos os pinos são entrada
//           P5 acessando periféricos: ACLK,SMCLK e MCLK
//
// Obs.: A configuração "ver clocks" não funciona na versão atual do PROTEUS!!
//  
//     0      0      0      0      0      0      0      0    <---- P5SEL: 0x00
//     1      1      1      1      1      1      1      1    <---- P5DIR: 0xff
// +------+------+------+------+------+------+------+------+
// | P5.7 | P5.6 | P5.5 | P5.4 | P5.3 | P5.2 | P5.1 | P5.0 | P5
// +------+------+------+------+------+------+------+------+
//          ACLK  SMCLK   MCLK                               <---- CLOCK DO MSP
//    CE    CLK2    CS    CLK1   DATA    DR1   DOUT2   DR2   <---- TRW-24G
//
//
#define p5_ver_clocks   0x70    // Para P5SEL
#define p5_como_io      0x00    // Para P5SEL
#define p5_saidas       0xff    // Para P5DIR
#define p5_entradas     0x00    // Para P5DIR
#define p5_radio        0xb8    // Para P5DIR

#define TRW_dr1         port5.in.pin2
#define TRW_data_in     port5.in.pin3
#define TRW_data_out    port5.out.pin3
#define TRW_clk1        port5.out.pin4
#define TRW_cs          port5.out.pin5
#define TRW_ce          port5.out.pin7

#define TRW_OUT()       P5DIR = 0xb8;
#define TRW_IN()        P5DIR = 0xb0;


//
// CONFIGURAÇÃO DA PORTA P6
//
// Esta porta admite várias  configurações entre  I/Os e saídas e entradas 
// analógicas. A configuração padrão vai ajustar a porta como saída de IO.
// Nas funções de acesso ao A/D e D/A será modificado o estado da porta
// para permitir acesso as funções analógicas.
//
//     0      0      0      0      0      0      0      0    <---- P6SEL: 0x00
//     1      1      1      1      1      1      1      1    <---- P6DIR: 0xff
// +------+------+------+------+------+------+------+------+
// | P6.7 | P6.6 | P6.5 | P6.4 | P6.3 | P6.2 | P6.1 | P6.0 | P6
// +------+------+------+------+------+------+------+------+
//    A7     A6     A5     A4     A3     A2     A1     A0    <---- Periférico Principal
//   DAC1   DAC0   DAC1                                      <--+- Periférico Secundário 
//  SVSIN                                                    <--+
//
//
#define p6_como_io      0x00    // Para P6SEL
#define p6_saidas       0xff    // Para P6DIR
#define p6_entradas     0x00    // Para P6DIR


// Configuração da CPU... Clocks e IOs
//
void configura_cpu(void)
{
  // Desliga o watch Dog...
  //
  WDTCTL = WDTPW + WDTHOLD; 

  // configura o clock do MSP430F2619
  //
  //  MCLK: 16MHz
  // SMCLK: 16MHz
  //  ACLK: 32KHz
  //
  DCOCTL = 0x77;
  BCSCTL1 = 0x0f;
  BCSCTL2 = 0x08;
  BCSCTL3 = 0x8c;

  // Configura as portas conforme a Placa de Laboratório 2010/I
  //
  //
  P1SEL = p1_como_io;
  P1DIR = p1_disp_teclado;
  P2SEL = p2_como_io;
  P2DIR = p2_saidas;
  P3SEL = p3_uart_i2c;
  P3DIR = i2c_saida;
  P4SEL = p4_como_io;
  P4DIR = p4_saidas;
  P5SEL = p5_como_io;
  P5DIR = p5_radio;
  P6SEL = p6_como_io;
  P6DIR = p6_saidas;

  // Apaga o LED...
  //
  led = 1;
}

