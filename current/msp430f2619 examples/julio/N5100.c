// ************************************************************************
// **                                                                    **
// **  BIBLIOTECA DO DISPLAY LCD NOKIA 5110/5120                         **
// **                                                                    **
// **  Esta biblioteca implementa as rotinas para um display grafico     **
// **  da Nokia com resolucao de 48x84 pixels. Este display utiliza o    **
// **  controlador da Philips PCF8533. As rotinas emulam um LCD com      **
// **  resulucao de 14x6 caracteres para um fonte de 7x5 pixels. As      **
// **  rotinas foram escritas para um microcontrolador trabalhando       **
// **  em 11.0592MHz!!!                                                  **
// **                                                                    **
// **  (C) by JCLima, 29/10/2005                                         **
// **                                                                    **
// ************************************************************************
//
#include <msp430x2619.h>
#include "lcd.h"
#include "delay.h"
#include "Tabela_ASCII.h"

extern const unsigned char asciitable[];
//
//  LCD Data & Control bits
//
sbit at 0x87 SCK;
sbit at 0x86 SDIN;
sbit at 0x85 DC;
sbit at 0x84 SCE;
sbit at 0x83 RESET;
sbit at 0x80 BACKLIT;

xdata unsigned char LCD_linecount;
xdata unsigned char LCD_charcount;


void lcd_initbus(void);
void lcd_wrcommand(unsigned char cmd);
void lcd_wrdata(unsigned char dat);
void lcd_clrscr(void);
void lcd_initmodule(void);
void lcd_on(void);
void lcd_off(void);
void lcd_reverse(void);
void lcd_normal(void);
void lcd_newline(void);
void lcd_goto(unsigned char x, unsigned char y);
void lcd_wrchar(unsigned char c);
void lcd_putchar(unsigned char c);
void lcd_putlogo(unsigned char tipo);
void lcd_backliton(void);
void lcd_backlitoff(void);


void lcd_initbus(void)
{
  RESET = 1;
  SCE = 1;
  SCK = 0;
  DC = 0;
  SDIN = 0;
  RESET = 0;
  delay(10);
  RESET = 1;
  delay(10);
}

void lcd_wrcommand(unsigned char cmd)
{
  unsigned char mask = 0x80;
  unsigned char i;

  SCE = 0;
  DC = 0;
  for (i=0; i<8; i++)
  {
    SDIN = (cmd & mask) ? 1: 0;
    SCK = 1;
    SCK = 0;
    mask >>= 1;
  }
  DC = 1;
  SCE = 1;
}

void lcd_wrdata(unsigned char dat)
{
  unsigned char mask = 0x80;
  unsigned char i;

  SCE = 0;
  DC = 1;
  for (i=0; i<8; i++)
  {
    SDIN = (dat & mask) ? 1: 0;
    SCK = 1;
    SCK = 0;
    mask >>= 1;
  }
  SCE = 1;
}

void lcd_clrscr(void)
{
  unsigned int i;

  lcd_wrcommand(0x40);
  lcd_wrcommand(0x80);
  for (i=0; i<504; i++)
  {
    lcd_wrdata(0x00);
  }
  lcd_wrcommand(0x40);
  lcd_wrcommand(0x80);

  LCD_linecount = 0;
  LCD_charcount = 0;
}

void lcd_initmodule(void)
{
  unsigned int i;

  lcd_wrcommand(0x21);
  lcd_wrcommand(0xC8);
  lcd_wrcommand(0x06);
  lcd_wrcommand(0x13);
  lcd_wrcommand(0x20);
  lcd_wrcommand(0x08);
  lcd_wrcommand(0x0C);

  lcd_wrcommand(0x40);
  lcd_wrcommand(0x80);
  for (i=0; i<504; i++)
  {
    lcd_wrdata(0x00);
  }
  lcd_wrcommand(0x40);
  lcd_wrcommand(0x80);

  LCD_linecount = 0;
  LCD_charcount = 0;
}

void lcd_on(void)
{
  lcd_wrcommand(0x0C);
}

void lcd_off(void)
{
  lcd_wrcommand(0x08);
}

void lcd_reverse(void)
{
  lcd_wrcommand(0x0D);
}

void lcd_normal(void)
{
  lcd_wrcommand(0x0C);
}

void lcd_newline(void)
{
  if (++LCD_linecount == LCD_NLINE) LCD_linecount = 0; 
  LCD_charcount = 0;
  lcd_wrcommand(0x40+LCD_linecount);
  lcd_wrcommand(0x80+LCD_charcount*6);
}

void lcd_goto(unsigned char x, unsigned char y)
{
  if (x > LCD_NCHAR-1) x = 0;
  if (y > LCD_NLINE-1) y = 0;

  LCD_charcount = x;
  LCD_linecount = y;
  lcd_wrcommand(0x40+LCD_linecount);
  lcd_wrcommand(0x80+LCD_charcount*6);
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

void lcd_putlogo(unsigned char tipo)
{
  unsigned int i;
  
  switch (tipo)
  {
    case 0  : for (i=0; i<504; i++)
              {
                lcd_wrdata(slogan[i]);
              }
              break;
    case 1  : for (i=0; i<504; i++)
              {
                lcd_wrdata(logo[i]);
              }
              break;
    case 2  : for (i=0; i<504; i++)
              {
                lcd_wrdata(mapamundi[i]);
              }
              break;
    case 3  : for (i=0; i<504; i++)
              {
                lcd_wrdata(abertura[i]);
              }
              break;
    case 4  : for (i=0; i<504; i++)
              {
                lcd_wrdata(bal[i]);
              }
              break;
    default : for (i=0; i<504; i++)
              {
                lcd_wrdata(slogan[i]);
              }
  }
}
