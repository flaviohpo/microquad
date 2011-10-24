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
#define LCD_NCHAR   16
#define LCD_NLINE   8

#define _graus 128
#define _up 129
#define _down 130
#define _left 131
#define _right 132
#define _enter 133

#define CR 0x0d
#define LF 0x0a

#define virado     1
#define nao_virado 0

//
// ****************************************************************
// ** P R O T O T Y P E S                                        **
// ****************************************************************
//
void lcd_wrcommand(unsigned char cmd);
void lcd_wrdata(unsigned char dat);
void lcd_initmodule(unsigned char state);
void lcd_setcontraste(unsigned char v);

void lcd_clrscr(void);
void lcd_on(void);
void lcd_off(void);
void lcd_reverse(void);
void lcd_normal(void);
void lcd_newline(void);
void lcd_goto(unsigned char x, unsigned char y);
void lcd_wrchar(unsigned char c);
void lcd_putchar(unsigned char c);
void lcd_putlogo(const unsigned char *p);


