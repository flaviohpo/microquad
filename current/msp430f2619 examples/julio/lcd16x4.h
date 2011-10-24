//
// ****************************************************************
// **                                                            **
// **  BIBLIOTECA DO DISPLAY LCD 16x4                            **
// **                                                            **
// **  Esta biblioteca implementa as rotinas de escrita em um    **
// **  display LCD 20x4 utilizando o MSC1211 trabalhando na      **
// **  frequencia de 11.0592MHz!!!                               **
// **                                                            **
// **  (C) by JCLima, 2006/1                                     **
// **                                                            **
// ****************************************************************
//
//
// ****************************************************************
// ** C O N S T A N T E S  D O  L C D 16 x 4                     **
// ****************************************************************
//  
#define cursorligado    0x0e
#define cursordesligado 0x0c
#define cursorpiscante  0x0f

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
//
// Exemplo de Declaração:
// const char CaracteresEspeciais[64] = { 
//                                      0x1f, 0x00, 0x00, 0x00,
//                                      0x00, 0x00, 0x00, 0x1f,
//                                      0x1f, 0x10, 0x10, 0x10,
//                                      0x10, 0x10, 0x10, 0x1f,
//                                      0x1f, 0x18, 0x18, 0x18,
//                                      0x18, 0x18, 0x18, 0x1f,
//                                      0x1f, 0x1c, 0x1c, 0x1c,
//                                      0x1c, 0x1c, 0x1c, 0x1f,
//                                      0x1f, 0x1e, 0x1e, 0x1e,
//                                      0x1e, 0x1e, 0x1e, 0x1f,
//                                      0x1f, 0x1f, 0x1f, 0x1f,
//                                      0x1f, 0x1f, 0x1f, 0x1f,
//                                      0x02, 0x02, 0x02, 0x04,
//                                      0x08, 0x02, 0x02, 0x02,
//                                      0x10, 0x14, 0x14, 0x14,
//                                      0x1b, 0x10, 0x10, 0x1f 
//                                     };
//


//
// ****************************************************************
// ** P R O T Ó T I P O S                                        **
// ****************************************************************
//  
void lcd_init(unsigned char estado);
void lcd_wrchar(unsigned char c);
void lcd_wrcom(unsigned char c);
void lcd_clear();
void lcd_cursoron();
void lcd_cursoroff();
void lcd_cursorblink();
void lcd_newline();
void lcd_goto(unsigned char x, unsigned char y);
void lcd_putchar(unsigned char c);
void lcd_specialchar(unsigned char *ptr);
void lcd_chardefault(void);

