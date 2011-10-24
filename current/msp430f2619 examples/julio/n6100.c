#include <msp430x2619.h>
#include <signal.h>

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

#define LED             port3.out.pin0
#define sda_in          port3.in.pin1
#define sda_out         port3.out.pin1
#define scl             port3.out.pin2
#define insert          port3.in.pin3
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

#define RESET           port1.out.pin4     // RESX   --> P1.4
#define SDATA           port1.out.pin5     // SDA    --> P1.5
#define SCLK            port1.out.pin6     // SCLK   --> P1.6
#define CS              port1.out.pin7     // CSX    --> P1.7


#define SDA_OUT()       P3DIR = 0xd7
#define SDA_IN()        P3DIR = 0xd5



//
// ****************************************************************
// ** COMANDOS                                                   **
// ****************************************************************
//
#define SOFT_RESET      0x01
#define SLEEP_OUT       0x11
#define DISPLAY_ON      0x29
#define BOOSTER_ON      0x03
#define COLOR_INTERFACE 0x3A
#define COLOR_8_BIT     0x02
#define COLOR_SET       0x2D
#define MEM_ACCESS_CTRL 0x36
#define MIRROR_X        6
#define MIRROR_Y        7
#define VERT_WRITE      5
#define RGB             3
#define MEM_WRITE       0x2C
#define SET_X_ADDR      0x2A
#define SET_Y_ADDR      0x2B


//
// ****************************************************************
// ** CORES                                                      **
// ****************************************************************
//
#define RED             0xE0
#define GREEN           0x1C
#define BLUE            0x03
#define YELLOW          0xFC
#define MAGENTA         0xE3
#define CYAN            0x1F
#define BLACK           0x00
#define WHITE           0xFF

#define watchdog_off()  WDTCTL = WDTPW + WDTHOLD;


void set_clock(void);
void delayus(unsigned int tempo);
void delayms(unsigned int tempo);
void n6100_init(void);
void n6100_sendcommand(unsigned char comando);
void n6100_senddata(unsigned char dado);
void n6100_clear(unsigned char cor);
//void n6100_plotamono(unsigned char x, unsigned char y, unsigned char lx, unsigned char ly, unsigned char forecolor, unsigned char backcolor);
void n6100_plotacolor(unsigned char x, unsigned char y, unsigned char lx, unsigned char ly, unsigned char cor);
void Epson_init(void);


int main(void)
{
  int i;

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
  SDA_OUT();
  P4DIR = 0xff;
  P1DIR = 0xf0;


  for(i=0; i<5; i++)
  {
    LED = 0;
    delayms(200);
    LED = 1;
    delayms(200);
  }  

  //n6100_init();
  Epson_init();
  n6100_plotacolor(0, 0, 128, 128, WHITE);
  delayms(1000);
  n6100_plotacolor(24, 42, 84, 48, RED);
  delayms(1000);
  n6100_plotacolor(24, 42, 84, 48, BLUE);
  delayms(1000);
  while (1)
  {
    LED = 1;
    delayms(100);
    LED = 0;
    delayms(100);
  }

  return 0;
}

void set_clock(void)
{
  // Ajuste clock para oscilador externo (4.000MHz)
  //
  BCSCTL1 = 0x8b;
  BCSCTL2 = 0xc8;
  BCSCTL3 = 0xf0;
}

//
// ****************************************************************
// ** D E L A Y                                                  **
// ****************************************************************
//
void delayus(unsigned int tempo)
{
  do 
  {
    tempo--;
  } while(tempo != 0);
}

void delayms(unsigned int tempo)
{
  do
  {
    tempo--;
    delayus(1690);
  } while(tempo != 0);
}




void n6100_sendcommand(unsigned char comando)
{
  unsigned char mask = 0x80;
  unsigned char i;

  SDATA = 0;
  CS = 0;
  SCLK = 1;
  SCLK = 0;
  
  for (i=0; i<8; i++)
  {
    if(comando & mask) SDATA = 1; else SDATA = 0;
    SCLK = 1;
    SCLK = 0;
    mask >>= 1;
  }
  SDATA = 0;
  CS = 1;
}

void n6100_senddata(unsigned char dado)
{
  unsigned char mask = 0x80;
  unsigned char i;

  SDATA = 1;
  CS = 0;
  SCLK = 1;
  SCLK = 0;
  
  for (i=0; i<8; i++)
  {
    if(dado & mask) SDATA = 1; else SDATA = 0;
    SCLK = 1;
    SCLK = 0;
    mask >>= 1;
  }
  SDATA = 0;
  CS = 1;
}

void n6100_init(void)
{
  CS = 1;
  SDATA = 0;
  SCLK = 0;

  RESET = 1;
  delayus(5);
  RESET = 0;
  delayus(5);
  delayus(5);
  RESET = 1;
  delayms(5);

  n6100_sendcommand(SOFT_RESET);
  n6100_sendcommand(SLEEP_OUT);
  n6100_sendcommand(DISPLAY_ON);
  n6100_sendcommand(BOOSTER_ON);
  delayms(10);  
    
  n6100_sendcommand(COLOR_INTERFACE);
  n6100_senddata(COLOR_8_BIT);
    
  // initialize Look Up Table for 8 bit mode
  n6100_sendcommand(COLOR_SET);
  // red
  n6100_senddata(0x00);
  n6100_senddata(0x02);
  n6100_senddata(0x03);
  n6100_senddata(0x04);
  n6100_senddata(0x05);
  n6100_senddata(0x06);
  n6100_senddata(0x08);
  n6100_senddata(0x0F);
  // green
  n6100_senddata(0x00);
  n6100_senddata(0x02);
  n6100_senddata(0x03);
  n6100_senddata(0x04);
  n6100_senddata(0x05);
  n6100_senddata(0x06);
  n6100_senddata(0x08);
  n6100_senddata(0x0F);
  // blue
  n6100_senddata(0x00);
  n6100_senddata(0x03);
  n6100_senddata(0x06);
  n6100_senddata(0x0F);
    
  n6100_sendcommand(MEM_ACCESS_CTRL);
  n6100_senddata(0x00);
    
  n6100_clear(BLACK);
}


void Epson_init(void)
{
  CS = 1;
  SDATA = 0;
  SCLK = 0;

  RESET = 1;
  delayus(5);
  RESET = 0;
  delayus(5);
  delayus(5);
  RESET = 1;
  delayms(5);

  n6100_senddata(0x00);
  n6100_senddata(0x20);
  n6100_senddata(0x00);
  n6100_senddata(0x01);
  n6100_senddata(0xd1);
  n6100_senddata(0x94);
  n6100_senddata(0x20);
  n6100_senddata(0x0f);
  n6100_senddata(0xa7);
  n6100_senddata(0xbc);
  n6100_senddata(0x01);
  n6100_senddata(0x00);
  n6100_senddata(0x02);
  n6100_senddata(0x81);
  n6100_senddata(32);
  n6100_senddata(3);
  delayms(100);
  n6100_senddata(0xaf);
}



void n6100_clear(unsigned char cor) 
{
  int i;
    
  n6100_sendcommand(MEM_WRITE);
  for(i=0; i<17030; i++) n6100_senddata(cor);
}

/*
void n6100_plotamono(unsigned char x, unsigned char y, unsigned char lx, unsigned char ly, unsigned char forecolor, unsigned char backcolor)
{
  unsigned int i;
  unsigned char rx, ry, j, cor, mask;

  n6100_sendcommand(0x2a);
  n6100_senddata(x);
  n6100_senddata(x+lx-1);
  n6100_sendcommand(0x2b);
  n6100_senddata(y);
  n6100_senddata(y+ly-1);

  n6100_sendcommand(MEM_WRITE);

  for(ry=0; ry<48; ry++)
  {
    for(rx=0; rx<11; rx++)
    {
      i = 11*ry+rx;
      mask = 0x80;
      if(rx < 10)
      {
        for(j=0; j<8; j++)
        {
          if(*(p+i) & mask) cor = forecolor; else cor = backcolor;
          mask >>= 1;
          n6100_senddata(cor);
        }
      } else
      {
        for(j=0; j<4; j++)
        {
          if(*(p+i) & mask) cor = forecolor; else cor = backcolor;
          mask >>= 1;
          n6100_senddata(cor);
        }
      }
    }
  }

  n6100_sendcommand(0x2a);
  n6100_senddata(0x00);
  n6100_senddata(0x83);
  n6100_sendcommand(0x2b);
  n6100_senddata(0x00);
  n6100_senddata(0x83);
}

void n6100_plotacolor(unsigned char x, unsigned char y, unsigned char lx, unsigned char ly)
{
  unsigned int i;
  unsigned char rx, ry, cor;

  n6100_sendcommand(0x2a);
  n6100_senddata(x);
  n6100_senddata(x+lx-1);
  n6100_sendcommand(0x2b);
  n6100_senddata(y);
  n6100_senddata(y+ly-1);

  n6100_sendcommand(MEM_WRITE);

  for(ry=0; ry<ly; ry++)
  {
    for(rx=0; rx<lx; rx++)
    {
      i = lx*ry+rx;
      cor = *(p+i);
      n6100_senddata(cor);
    }
  }

  n6100_sendcommand(0x2a);
  n6100_senddata(0x00);
  n6100_senddata(0x83);
  n6100_sendcommand(0x2b);
  n6100_senddata(0x00);
  n6100_senddata(0x83);
}
*/

void n6100_plotacolor(unsigned char x, unsigned char y, unsigned char lx, unsigned char ly, unsigned char cor)
{
  unsigned char rx, ry;

  n6100_sendcommand(0x2a);
  n6100_senddata(x);
  n6100_senddata(x+lx-1);
  n6100_sendcommand(0x2b);
  n6100_senddata(y);
  n6100_senddata(y+ly-1);

  n6100_sendcommand(MEM_WRITE);

  for(ry=0; ry<ly; ry++)
  {
    for(rx=0; rx<lx; rx++)
    {
      n6100_senddata(cor);
    }
  }

  n6100_sendcommand(0x2a);
  n6100_senddata(0x00);
  n6100_senddata(0x83);
  n6100_sendcommand(0x2b);
  n6100_senddata(0x00);
  n6100_senddata(0x83);
}

