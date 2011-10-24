//
// ****************************************************************
// **                                                            **
// **  BIBLIOTECA DO DISPLAY LCD 16x2                            **
// **                                                            **
// **  Esta biblioteca implementa as rotinas de escrita em um    **
// **  display LCD 16x2 utilizando o MSP430 trabalhando na       **
// **  frequencia de 16MHz!                                      **
// **                                                            **
// **                                   (C) by JCLima, 2010/1    **
// **                                                            **
// ****************************************************************
//
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <io.h>
#include <msp430x2619.h>
#include "LabProc.h"
#include "delay.h"
#include "lcd16x4.h"

//
// ****************************************************************
// ** C O N S T A N T E S  D O  L C D 20 x 4                     **
// ****************************************************************
//  
#define LCD_NCOL 16
#define LCD_NROW 4

#define LCD_ROW1 0x80
#define LCD_ROW2 0xc0
#define LCD_ROW3 0x90
#define LCD_ROW4 0xd0

//
// ****************************************************************
// ** Tabela de Conversão Hexadecimal para ASCII!!!              **
// ****************************************************************
//
const char ConstHexa[16] = { "0123456789ABCDEF" };

//
// ****************************************************************
// ** Tabela de Caracteres Especiais para o LCD!!!               **
// ****************************************************************
//
//  Escreve na CGRAM!
//  Cada caracter é codificado em 8 bytes no formato 5x7 ou 5x8!
//  Obs.: Os bits 5,6 e 7 não sao usadas!!!
//
//  Ex.: Caracter 0x04,0x11,0x04,0x0e,0x0e,0x04,0x11,0x04 fica:
//
//   7 6 5 4 3 2 1 0
//  +-+-+-+-+-+-+-+-+
//  | | | | | |*| | | <--- Linha 1 (000 00100)
//  | | | |*| | | |*| <--- Linha 2 (000 10001)
//  | | | | | |*| | | <--- Linha 3 (000 00100)
//  | | | | |*|*|*| | <--- Linha 4 (000 01110)
//  | | | | |*|*|*| | <--- Linha 5 (000 01110)
//  | | | | | |*| | | <--- Linha 6 (000 00100)
//  | | | |*| | | |*| <--- Linha 7 (000 10001)
//  | | | | | |*| | | <--- Linha 8 (000 00100)
//  +-+-+-+-+-+-+-+-+
//   ^ ^ ^
//   | | |
//   +-+-+---------------- Colunas Não Usadas
//
const unsigned char CaracteresEspeciais[64] = { 
                                      0x04, 0x11, 0x04, 0x0e,
                                      0x0e, 0x04, 0x11, 0x04,
                                      0x09, 0x06, 0x06, 0x09,
                                      0x00, 0x00, 0x00, 0x00,
                                      0x1f, 0x19, 0x13, 0x13,
                                      0x19, 0x1f, 0x1f, 0x1f,
                                      0x00, 0x00, 0x0a, 0x00,
                                      0x0e, 0x11, 0x00, 0x00,
                                      0x00, 0x00, 0x0a, 0x00,
                                      0x11, 0x0e, 0x00, 0x00,
                                      0x0e, 0x11, 0x04, 0x04,
                                      0x07, 0x00, 0x11, 0x0e,
                                      0x02, 0x02, 0x02, 0x04,
                                      0x08, 0x02, 0x02, 0x02,
                                      0x10, 0x14, 0x14, 0x14,
                                      0x1b, 0x10, 0x10, 0x1f
                                    };

//
// ****************************************************************
// ** VARIAVEIS                                                  **
// ****************************************************************
//  
unsigned char LCD_rowcount;
unsigned char LCD_colcount;

//
// ****************************************************************
// ** INICIALIZAÇÃO DO LCD                                       **
// ****************************************************************
//  
void lcd_init(unsigned char estado)
{
  lcd_en = 0;
  lcd_rs = 1;
  delay5us();

  lcd_wrcom(0x38);
  lcd_wrcom(0x38);
  lcd_wrcom(0x38);
  lcd_wrcom(estado);
  lcd_wrcom(0x06);
  lcd_wrcom(0x01);

  LCD_rowcount = 0;
  LCD_colcount = 0;

  delayms(40);
}

//
// ****************************************************************
// ** ESCREVE CARACTER. OBS.: Não ajusta posição do cursor!!!    **
// ****************************************************************
//  
void lcd_wrchar(unsigned char c)
{
  lcd_rs = 1;
  lcd_data = c;
  lcd_en = 1;
  delay5us();
  lcd_en = 0;
  delayms(5);         // Podem ter alguns modelos que necessitem + tempo!!!
}

//
// ****************************************************************
// ** ESCREVE COMANDO                                            **
// ****************************************************************
//  
void lcd_wrcom(unsigned char c)
{
  lcd_rs = 0;
  lcd_data = c;
  lcd_en = 1;
  delay5us();
  lcd_en = 0;
  delayms(10);        // Podem ter alguns modelos que necessitem + tempo!!!
}

//
// ****************************************************************
// ** LIMPA O DISPLAY                                            **
// ****************************************************************
//  
void lcd_clear()
{
  lcd_wrcom(0x01);
  lcd_wrcom(LCD_ROW1);
  LCD_rowcount = 0;
  LCD_colcount = 0;
  delayms(10);
}

//
// ****************************************************************
// ** MOSTRA O CURSOR                                            **
// ****************************************************************
//  
void lcd_cursoron()
{
  lcd_wrcom(0x0e);
}

//
// ****************************************************************
// ** ESCONDE O CURSOR                                           **
// ****************************************************************
//  
void lcd_cursoroff()
{
  lcd_wrcom(0x0c);
}

//
// ****************************************************************
// ** MOSTRA O CURSOR COM ATRIBUTO PISCANTE                      **
// ****************************************************************
//  
void lcd_cursorblink()
{
  lcd_wrcom(0x0f);
}

//
// ****************************************************************
// ** POSICIONA O CURSOR NA NOVA LINHA                           **
// ****************************************************************
//  
void lcd_newline()
{
  if (++LCD_rowcount == LCD_NROW) LCD_rowcount = 0; 
  LCD_colcount = 0;

  switch (LCD_rowcount)
  {
    case 0  : lcd_wrcom(LCD_ROW1);
              break;

    case 1  : lcd_wrcom(LCD_ROW2);
              break;

    case 2  : lcd_wrcom(LCD_ROW3);
              break;

    case 3  : lcd_wrcom(LCD_ROW4);
              break;

    default : lcd_wrcom(LCD_ROW1);
  }
}

//
// ****************************************************************
// ** POSICIONA O CURSOR                                         **
// ****************************************************************
//  
void lcd_goto(unsigned char x, unsigned char y)
{
  LCD_colcount = x;
  LCD_rowcount = y;
  
  switch (y)
  {
    case 0 : lcd_wrcom(LCD_ROW1+x);
             break;

    case 1 : lcd_wrcom(LCD_ROW2+x);
             break;
    
    case 2 : lcd_wrcom(LCD_ROW3+x);
             break;
    
    case 3 : lcd_wrcom(LCD_ROW4+x);
             break;

    default: lcd_wrcom(LCD_ROW1+x);
  }
  delayms(10);
}

//
// ****************************************************************
// ** IMPLEMENTAÇÃO DO PRINTF...                                 **
// ****************************************************************
//  
void lcd_putchar(unsigned char c)
{
  switch (c)
  {
    case 13 : if(++LCD_rowcount == LCD_NROW) LCD_rowcount = 0;
              switch (LCD_rowcount)
              {
                case 0 : lcd_wrcom(LCD_ROW1+LCD_colcount);
                break;
                case 1 : lcd_wrcom(LCD_ROW2+LCD_colcount);
                break;    
                case 2 : lcd_wrcom(LCD_ROW3+LCD_colcount);
                break;    
                case 3 : lcd_wrcom(LCD_ROW4+LCD_colcount);
                break;
              }
              break;

    case 10 : LCD_colcount = 0;
              switch (LCD_rowcount)
              {
                case 0 : lcd_wrcom(LCD_ROW1+LCD_colcount);
                break;
                case 1 : lcd_wrcom(LCD_ROW2+LCD_colcount);
                break;    
                case 2 : lcd_wrcom(LCD_ROW3+LCD_colcount);
                break;    
                case 3 : lcd_wrcom(LCD_ROW4+LCD_colcount);
                break;
              }
              break;

    default : lcd_wrchar(c);
              if (++LCD_colcount == LCD_NCOL)
              {
                LCD_colcount = 0;
                if (++LCD_rowcount == LCD_NROW) LCD_rowcount = 0;                
                switch (LCD_rowcount)
                {
                  case 0 : lcd_wrcom(LCD_ROW1+LCD_colcount);
                  break;
                  case 1 : lcd_wrcom(LCD_ROW2+LCD_colcount);
                  break;    
                  case 2 : lcd_wrcom(LCD_ROW3+LCD_colcount);
                  break;    
                  case 3 : lcd_wrcom(LCD_ROW4+LCD_colcount);
                  break;
                }
              }
  }
}

//
// ****************************************************************
// ** PROGRAMA DISPLAY PARA VISUALIZAR CARACTERES ESPECIAIS      **
// ** Programa os 8 caracteres da CGRAM!                         **
// **                                                            **
// ** Para usar os caracteres, basta imprimir os caracteres      **
// ** 0 à 7 - lcd_putchar(0x07) por exemplo.                     **
// ****************************************************************
//  
void lcd_specialchar(unsigned char *ptr)
{
  unsigned char i;

  lcd_wrcom(0x40);
  for (i=0; i<64; i++) lcd_wrchar(*(ptr+i));
  lcd_wrcom(0x80);
}

void lcd_chardefault(void)
{
  unsigned char i;
  const unsigned char *ptr;
  
  ptr = CaracteresEspeciais;

  lcd_wrcom(0x40);
  for (i=0; i<64; i++) lcd_wrchar(*(ptr+i));
  lcd_wrcom(0x80);
}


