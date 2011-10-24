//
// ****************************************************************
// **                                                            **
// **  LABORATORIO DE PROCESSADORES I (2010/I)                   **
// **                                                            **
// **  Esta biblioteca implementa as rotinas de display gráfico  **
// **  do Nokia2112.                                             **
// **                                                            **
// **                                    (C) by JCLima, 2010/1   **
// **                                                            **
// ****************************************************************
//
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <msp430x2619.h>
#include "lcd2112.h"
#include "Tabela_ASCII.h"


#define RESX            port1.out.pin4     // RESX   --> P1.4
#define SDA             port1.out.pin5     // SDA    --> P1.5
#define SCLK            port1.out.pin6     // SCLK   --> P1.6
#define CSX             port1.out.pin7     // CSX    --> P1.7

//
// ****************************************************************
// ** C O N S T A N T E S                                        **
// ****************************************************************
//
const unsigned char mask[8] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01};

//
// ****************************************************************
// ** V A R I Á V E I S  D O  D I S P L A Y                      **
// ****************************************************************
//
unsigned char LCD_linecount;
unsigned char LCD_charcount;


//
// ****************************************************************
// ** P R I M I T I V A S  D O  D I S P L A Y  ST7545            **
// ****************************************************************
//
//
// Implementa a temporização da interface SPI de 3 fios para 
// envio de um comando (A0 = 0).
//
void lcd_wrcommand(unsigned char cmd)
{
  unsigned char i;

  CSX = 0;
  SDA = 0;
  SCLK = 1;
  SCLK = 0;
  
  for (i=0; i<8; i++)
  {
    if (cmd & mask[i]) SDA = 1; else SDA = 0;
    SCLK = 1;
    SCLK = 0;
  }

  CSX = 1;
}

//
// Implementa a temporização da interface SPI de 3 fios para 
// escrita de 8 pixeis no display (A0 = 1)
//
void lcd_wrdata(unsigned char dat)
{
  unsigned char i;
 
  CSX = 0;
  SDA = 1;
  SCLK = 1;
  SCLK = 0;
  
  for (i=0; i<8; i++)
  {
    if (dat & mask[i]) SDA = 1; else SDA = 0;
    SCLK = 1;
    SCLK = 0;
  }

  CSX = 1;
}

// ST2007
//
void lcd_initmodule(unsigned char state)
{
  unsigned int i; 

  lcd_wrcommand(0x24); // VOR
  lcd_wrcommand(0x90); // Electronic Volume
  lcd_wrcommand(0xa4); // All off
  lcd_wrcommand(0x2f); // Power Boost on
  lcd_wrcommand(0xb0); // Page 0
  lcd_wrcommand(0x10); // Column 0
  lcd_wrcommand(0x00);
  
  // Zera o conteúdo da RAM  
  for(i=0; i<864; i++) lcd_wrdata(0x00);

  if(state == virado) lcd_wrcommand(0xc8); else lcd_wrcommand(0xa1);
  
  lcd_wrcommand(0xaf); // display on

  LCD_linecount = 0;
  LCD_charcount = 0;
}

void lcd_setcontraste(unsigned char v)
{
  lcd_wrcommand(0xe1);
  lcd_wrcommand(v);
}

void lcd_clrscr(void)
{
  unsigned int i;

  lcd_wrcommand(0xb0); // Page 0
  lcd_wrcommand(0x10); // Column 0
  lcd_wrcommand(0x00);
  
  // Zera o conteúdo da RAM  
  for(i=0; i<864; i++) lcd_wrdata(0x00);

  LCD_linecount = 0;
  LCD_charcount = 0;
}

void lcd_on(void)
{
  lcd_wrcommand(0xaf);
}

void lcd_off(void)
{
  lcd_wrcommand(0xae);
}

void lcd_reverse(void)
{
  lcd_wrcommand(0xa7);
}

void lcd_normal(void)
{
  lcd_wrcommand(0xa6);
}

void lcd_newline(void)
{
  if (++LCD_linecount == LCD_NLINE) LCD_linecount = 0; 
  LCD_charcount = 0;
  lcd_wrcommand(0xb0+LCD_linecount);
  lcd_wrcommand(0x10);
  lcd_wrcommand(0x00);
}

void lcd_goto(unsigned char x, unsigned char y)
{
  unsigned char k;
  
  if (x > LCD_NCHAR-1) x = 0;
  if (y > LCD_NLINE-1) y = 0;

  LCD_charcount = x;
  LCD_linecount = y;
  
  lcd_wrcommand(0xb0+LCD_linecount);
  k = LCD_charcount*6;
  lcd_wrcommand(0x10+(k/16));
  lcd_wrcommand(0x00+(k%16));
}

void lcd_wrchar(unsigned char c)
{
  unsigned int j;
  unsigned char i, v;

  j = (unsigned int) c * 5;
  for (i=0; i<5; i++)
  {
    v = asciitable[j];
    lcd_wrdata(v);
    j += 1;
  }
  lcd_wrdata(0x00);
}

void lcd_putchar(unsigned char c)
{
  switch (c)
  {
    case 13 : lcd_newline();
              break;
    case 10 : LCD_charcount = 0;
              lcd_goto(LCD_charcount, LCD_linecount);
              break;
    default : if (LCD_charcount++ < LCD_NCHAR) lcd_wrchar(c);
              else
              {
                lcd_newline();
                lcd_wrchar(c);
                LCD_charcount++;
              }
  }
}

void lcd_putlogo(const unsigned char *p)
{
  unsigned int i;

  for(i=0; i<864; i++) lcd_wrdata(*(p+i));
}



