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

// ****************************************************************
// ** TABELA DE CORES BÁSICAS (8Bits) - FORMATO: RRRGGGBB        **
// ****************************************************************
//
#define BLACK           0x00
#define GRAY            0x49
#define MARROON         0x40
#define OLIVE           0x6c
#define GREEN           0x08
#define TEAL            0x09
#define NAVY            0x01
#define PURPLE          0x41
#define MAGENTA         0xe3
#define BLUE            0x03
#define AQUA            0x1f
#define LIME            0x1c
#define YELLOW          0xfc
#define RED             0xe0
#define SILVER          0xb6
#define WHITE           0xff
#define CYAN            0x1F
#define ORANGE          (YELLOW | RED)

//
// ****************************************************************
// ** V A R I Á V E I S  D O  D I S P L A Y                      **
// ****************************************************************
//
extern unsigned char LCD_linecount;
extern unsigned char LCD_charcount;
extern unsigned char color_back;
extern unsigned char color_fore;

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

extern unsigned char LCD_linecount;
extern unsigned char LCD_charcount ;
extern unsigned char color_back;
extern unsigned char color_fore;


extern const char asciitable[640];

//
// ****************************************************************
// ** P R O T Ó T I P O S                                        **
// ****************************************************************
//
void lcd_setcolor(unsigned char foreground_color, unsigned char background_color);
void n6100_send(unsigned char data, unsigned char cmd);
void lcd_init(unsigned char cor);
void lcd_clear(unsigned char cor); 
void n6100_sendcom1(unsigned char comm, unsigned char dat);
void n6100_sendcom2(unsigned char comm, unsigned char dat1, unsigned char dat2);
void lcd_fillrect(unsigned char x, unsigned char y, unsigned char lx, unsigned char ly, unsigned char cor);
void lcd_putlogo(unsigned char x, unsigned char y, unsigned char lx, unsigned char ly, unsigned char p[]);
void lcd_newline(void);
void lcd_goto(unsigned char x, unsigned char y);
void lcd_wrchar(unsigned char c);
void lcd_putchar(unsigned char c);
void lcd_drawcircle(unsigned int x, unsigned int y, unsigned int radius, unsigned char color, int width);
void lcd_drawline(int x0, int y0, int x1, int y1, int color, int width);
void lcd_drawprogressbar(int x0, int y0, int x1, int y1, int color, int colorprogress, int percent);


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





