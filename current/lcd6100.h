//
// ** LCD6100.h ***************************************************
// **                                                            **
// **       Biblioteca do display  gráfico colorido Nokia 6100.  **
// **    Esta biblioteca foi  configurada como 8bits/pixel para  **
// **    operações alfanuméricas (display "texto") e pode colar  **
// **    imagens em 12bits/pixel ou 8bits/pixel.                 **
// **                                                            **
// ****************************************(C) by JCLima, 2010/2 **
//
//                       +----+
//      0                |    | 131   <--- CONECTOR VERDE S1D15G00 
//     +-----------------+    +----+       CONECTOR AMARELO PCF8833
//    0|  ---X--->                 |                     Nokia 6100
//     | |                         |                     Nokia 7250
//     | |    Color: BBGGGRRR      |                     Nokia 2600
//     | Y                         |
//     | |         -->--           |       INICIALIZAÇÃO
//     | |            /            |
//     | V           /             | Memory Data Access Control
//     |           -->--           |            0x08
//     |              /            |             |
//     |             /             |             |
//     |           -->--           |<------------+
//  131|                           |
//     +---------------------------+
//                             
//
//
// 
//    avacalhado por flavio em 20/11/2010
//    - lcd_drawcircle(...);
//    - lcd_drawline(...);
//    - otimizacao do codigo:
//        - funcao n6100_sendcommand e n6100_senddata viraram uma so: n6100_send(char,char)
//        - o codigo foi reduzido em aproximadamente 20% de tamanho
//        - fonts.h foi movido para lcd6100.h
//        - prototipos foram movidos para lcd6100.h
//        - cor ORANGE e LIME foram adicionadas
//    
//    continuacao flavio 24/7/2011
//        void lcd_drawpoint(unsigned char x, unsigned char y, int color)
//    
//    continuacao flavio 23/10/2011
//        adicionado typedef COLOR
//        cor ORANGE foi removida
//        
//                             
//
//
// ****************************************************************
// ** V A R I Á V E I S  D O  D I S P L A Y                      **
// ****************************************************************
//
extern unsigned char LCDLineCount;
extern unsigned char LCDCharCount;

extern unsigned char LCDBackColor;
extern unsigned char LCDForeColor;

// Display Gráfico Color 
//
#define RESX            BIT2
#define SDA             BIT1     
#define SCLK            BIT3     
#define CSX             BIT0    

#define LCD_NCHAR       21
#define LCD_NLINE       16

#define direto          0x48
#define invertido       0x00

extern const char asciitable[640];

//
// ****************************************************************
// ** P R O T Ó T I P O S                                        **
// ****************************************************************
//

void lcd_setcolor(unsigned char foreground_color, unsigned char background_color);
void n6100_send(unsigned char data, unsigned char cmd);
void lcd_init(unsigned char color);
void lcd_clear(unsigned char color); 
void n6100_sendcom1(unsigned char comm, unsigned char dat);
void n6100_sendcom2(unsigned char comm, unsigned char dat1, unsigned char dat2);
void lcd_drawpoint(unsigned char x, unsigned char y, unsigned char color);
void lcd_fillrect(unsigned char x, unsigned char y, unsigned char lx, unsigned char ly, unsigned char color);
void lcd_putlogo(unsigned char x, unsigned char y, unsigned char lx, unsigned char ly, unsigned char p[]);
void lcd_newline(void);
void lcd_goto(unsigned char x, unsigned char y);
void lcd_wrchar(unsigned char c);
void lcd_putchar(unsigned char c);
void lcd_drawcircle(unsigned int x, unsigned int y, unsigned int radius, unsigned char color, int width);
void lcd_drawline(int x0, int y0, int x1, int y1, unsigned char color);
void lcd_drawprogressbar(int x0, int y0, int x1, int y1, unsigned char color, unsigned char colorprogress, int percent);
void lcd_putdot(int from, int to);


// ************************************************************************
// **                                                                    **
// **  TABELA ASCII para LCDs Gráficos                                   **
// **  Font 7x5                                                          **
// **                                                                    **
// **  Tabela de 0..127                                                  **
// **                                                                    **
// **                                        (C) by JCLima, 18/03/2010   **
// **                                                                    **
// ************************************************************************
//