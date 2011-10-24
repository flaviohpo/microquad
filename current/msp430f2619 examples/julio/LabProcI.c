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
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <msp430x2619.h>

#define led             port3.out.pin0
#define verde           port3.out.pin3
#define sda_in          port3.in.pin1
#define sda_out         port3.out.pin1
#define scl             port3.out.pin2
//#define insert          port3.in.pin3
#define txd             port3.out.pin4
#define rxd             port3.in.pin5
#define lcd_rs          port3.out.pin6
#define lcd_en          port3.out.pin7

#define lcd_data        P4OUT

#define sw1             port1.in.pin0
#define sw2             port1.in.pin1
#define sw3             port1.in.pin2
#define sw4             port1.in.pin3
#define lcd_reset       port1.out.pin4
#define lcd_din         port1.out.pin5
#define lcd_sck         port1.out.pin6
#define lcd_cs          port1.out.pin7

#define RESX            port1.out.pin4     // RESX   --> P1.4
#define SDA             port1.out.pin5     // SDA    --> P1.5
#define SCLK            port1.out.pin6     // SCLK   --> P1.6
#define CSX             port1.out.pin7     // CSX    --> P1.7


//#define SDA_OUT()       P3DIR = 0xd7
//#define SDA_IN()        P3DIR = 0xd5
#define SDA_OUT()       P3DIR = 0xdf
#define SDA_IN()        P3DIR = 0xdd


void configura_cpu(void)
{
  // Desliga o watch dog timer...
  //
  WDTCTL = WDTPW + WDTHOLD;

  // Definindo o basic clock...
  // ACLK = 32KHz, MCLK = 16MHz, SMCLK = 16MHz
  //
  BCSCTL1 = 0x07;
  BCSCTL2 = 0x88;
  BCSCTL3 = 0x84;


  // Configurando as portas
  //
  P3SEL = 0x30;
  SDA_OUT();
  P4DIR = 0xff;
  P1DIR = 0xf0;
}

