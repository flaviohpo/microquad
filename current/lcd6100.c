//
// ** LCD6100.c ***************************************************
// **                                                            **
// **       Biblioteca do display  gráfico colorido Nokia 6100.  **
// **    Esta biblioteca foi  configurada como 8bits/pixel para  **
// **    operações alfanuméricas (display "texto") e pode colar  **
// **    imagens em 12bits/pixel ou 8bits/pixel.                 **
// **                                                            **
// ****************************************(C) by JCLima, 2010/2 **
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

#include <stdio.h>
#include <stdlib.h>
#include "msp430f2618.h"
#include "color.h"
#include "delay.h"
#include "lcd6100.h"

unsigned char LCDLineCount = 0;
unsigned char LCDCharCount = 0;
unsigned char LCDBackColor = BLACK;
unsigned char LCDForeColor = LIME;

const char asciitable[640] = {
                            0x00,0x00,0x00,0x00,0x00,    // NULL char...
                            0x3E,0x0A,0x34,0x00,0x3E,
                            0x18,0x3E,0x00,0x3E,0x22,
                            0x00,0x3E,0x20,0x20,0x00,
                            0x22,0x1C,0x00,0x24,0x2A,
                            0x00,0x08,0x38,0x38,0x08,
                            0x1C,0x22,0x22,0x22,0x1C,
                            0x00,0x38,0x38,0x44,0x82,
                            0x1C,0x3E,0x62,0x62,0x32,
                            0x2E,0x26,0x23,0x23,0x22,
                            0x00,0x00,0x00,0x00,0x00,
                            0x1C,0x3E,0x36,0x22,0x36,
                            0x3E,0x3E,0x36,0x36,0x36,
                            0x00,0x00,0x00,0x00,0x00,
                            0x18,0x08,0x1C,0x22,0x22,
                            0x63,0x55,0x4D,0x55,0x6B,
                            0x00,0x3E,0x0A,0x04,0x78,
                            0x00,0x3E,0x0A,0x04,0x78,
                            0x00,0x38,0x44,0x5F,0x4E,
                            0x00,0x38,0x44,0x44,0x5F,
                            0x00,0x3C,0x3C,0x3C,0x3C,
                            0x00,0x3E,0x0A,0x04,0x78,
                            0x18,0x24,0x42,0x24,0x18,   // 22 quadrado aberto
                            0x18,0x3C,0x7E,0x3C,0x18,   // 23 quadrado fechado
                            0xC0,0xF0,0xFC,0xF0,0xC0,   // seta
                            0x03,0x0F,0x3F,0x0F,0x03,   // seta
                            0xFF,0x7E,0x3C,0x18,0x00,   // seta
                            0x00,0x18,0x3C,0x7E,0xFF,   // seta
                            0x00,0x3C,0x20,0x00,0x00,
                            0x08,0x14,0x00,0x14,0x08,
                            0x08,0x0C,0x0E,0x0C,0x08,
                            0x08,0x18,0x38,0x18,0x08,                            
                            0x00,0x00,0x00,0x00,0x00,    // space...
                            0x00,0x00,0x5F,0x00,0x00,
                            0x00,0x07,0x00,0x07,0x00,
                            0x14,0x7F,0x14,0x7F,0x14,
                            0x24,0x2A,0x7F,0x2A,0x12,
                            0x23,0x13,0x08,0x64,0x62,
                            0x36,0x49,0x55,0x22,0x50,
                            0x00,0x05,0x03,0x00,0x00,
                            0x00,0x1C,0x22,0x41,0x00,
                            0x00,0x41,0x22,0x1C,0x00,
                            0x14,0x08,0x3E,0x08,0x14,
                            0x08,0x08,0x3E,0x08,0x08,
                            0x00,0x50,0x30,0x00,0x00,
                            0x08,0x08,0x08,0x08,0x08,
                            0x00,0x60,0x60,0x00,0x00,
                            0x20,0x10,0x08,0x04,0x02,
                            0x3E,0x51,0x49,0x45,0x3E,
                            0x00,0x42,0x7F,0x40,0x00,
                            0x42,0x61,0x51,0x49,0x46,
                            0x21,0x41,0x45,0x4B,0x31,
                            0x18,0x14,0x12,0x7F,0x10,
                            0x27,0x45,0x45,0x45,0x39,
                            0x3C,0x4A,0x49,0x49,0x30,
                            0x01,0x71,0x09,0x05,0x03,
                            0x36,0x49,0x49,0x49,0x36,
                            0x06,0x49,0x49,0x29,0x1E,
                            0x00,0x36,0x36,0x00,0x00,
                            0x00,0x56,0x36,0x00,0x00,
                            0x08,0x14,0x22,0x41,0x00,
                            0x14,0x14,0x14,0x14,0x14,
                            0x00,0x41,0x22,0x14,0x08,
                            0x02,0x01,0x51,0x09,0x06,
                            0x32,0x49,0x79,0x41,0x3E,
                            0x7E,0x11,0x11,0x11,0x7E,
                            0x7F,0x49,0x49,0x49,0x36,
                            0x3E,0x41,0x41,0x41,0x22,
                            0x7F,0x41,0x41,0x22,0x1C,
                            0x7F,0x49,0x49,0x49,0x41,
                            0x7F,0x09,0x09,0x09,0x01,
                            0x3E,0x41,0x49,0x49,0x7A,
                            0x7F,0x08,0x08,0x08,0x7F,
                            0x00,0x41,0x7F,0x41,0x00,
                            0x20,0x40,0x41,0x3F,0x01,
                            0x7F,0x08,0x14,0x22,0x41,
                            0x7F,0x40,0x40,0x40,0x40,
                            0x7F,0x02,0x0C,0x02,0x7F,
                            0x7F,0x04,0x08,0x10,0x7F,
                            0x3E,0x41,0x41,0x41,0x3E,
                            0x7F,0x09,0x09,0x09,0x06,
                            0x3E,0x41,0x51,0x21,0x5E,
                            0x7F,0x09,0x19,0x29,0x46,
                            0x46,0x49,0x49,0x49,0x31,
                            0x01,0x01,0x7F,0x01,0x01,
                            0x3F,0x40,0x40,0x40,0x3F,
                            0x1F,0x20,0x40,0x20,0x1F,
                            0x3F,0x40,0x38,0x40,0x3F,
                            0x63,0x14,0x08,0x14,0x63,
                            0x07,0x08,0x70,0x08,0x07,
                            0x61,0x51,0x49,0x45,0x43,
                            0x00,0x7F,0x41,0x41,0x00,
                            0x02,0x04,0x08,0x10,0x20,
                            0x00,0x41,0x41,0x7F,0x00,
                            0x04,0x02,0x01,0x02,0x04,
                            0x40,0x40,0x40,0x40,0x40,
                            0x00,0x01,0x02,0x04,0x00,
                            0x20,0x54,0x54,0x54,0x78,
                            0x7F,0x48,0x44,0x44,0x38,
                            0x38,0x44,0x44,0x44,0x20,
                            0x38,0x44,0x44,0x48,0x7F,
                            0x38,0x54,0x54,0x54,0x18,
                            0x08,0x7E,0x09,0x01,0x02,
                            0x0C,0x52,0x52,0x52,0x3E,
                            0x7F,0x08,0x04,0x04,0x78,
                            0x00,0x44,0x7D,0x40,0x00,
                            0x20,0x40,0x44,0x3D,0x00,
                            0x7F,0x10,0x28,0x44,0x00,
                            0x00,0x41,0x7F,0x40,0x00,
                            0x7C,0x04,0x18,0x04,0x78,
                            0x7C,0x08,0x04,0x04,0x78,
                            0x38,0x44,0x44,0x44,0x38,
                            0x7C,0x14,0x14,0x14,0x08,
                            0x08,0x14,0x14,0x18,0x7C,
                            0x7C,0x08,0x04,0x04,0x08,
                            0x48,0x54,0x54,0x54,0x20,
                            0x04,0x3F,0x44,0x40,0x20,
                            0x3C,0x40,0x40,0x20,0x7C,
                            0x1C,0x20,0x40,0x20,0x1C,
                            0x3C,0x40,0x30,0x40,0x3C,
                            0x44,0x28,0x10,0x28,0x44,
                            0x0C,0x50,0x50,0x50,0x3C,
                            0x44,0x64,0x54,0x4C,0x44,
                            0x00,0x00,0x36,0x41,0x00,
                            0x00,0x00,0x7F,0x00,0x00,
                            0x00,0x41,0x36,0x08,0x00,
                            0x10,0x08,0x08,0x10,0x08,
                            0x78,0x46,0x41,0x46,0x78,
                         };

// printf para LCDNOKIA
int putchar(int c)
{
    lcd_putchar(c);
    return 0;
}
                         
void lcd_setcolor(unsigned char foreground_color, unsigned char background_color)
{
    LCDBackColor = background_color;
    LCDForeColor = foreground_color;
}

/*
    cmd = 0: writedata with stop
    cmd = 1 writedata without stop
    cmd = 2 writecommand with stop
    cmd = 3 writecommand without stop
*/
void n6100_send(unsigned char data, unsigned char cmd)
{
    unsigned char mask = 0x80;
    unsigned char i;
    
    if(cmd > 1){ 
        P5OUT &= ~SDA; // comando
    }
    else{
        P5OUT |= SDA; // dado
    }
    
    P5OUT &= ~CSX;
    P5OUT |= SCLK;
    P5OUT &= ~SCLK;
    
    for (i=0; i<8; i++){
        if(data & mask){
            P5OUT |= SDA; 
        }
        else{
            P5OUT &= ~SDA;
        }
        P5OUT |= SCLK;
        P5OUT &= ~SCLK;
        mask >>= 1;
    }
    
    if(cmd == 1 || cmd == 3)
        return;
        
    P5OUT &= ~SDA;
    P5OUT |= CSX;
}

void lcd_init(unsigned char color)
{
    P5DIR |= CSX + SDA + SCLK + RESX;
    
  P5OUT |= CSX;
  P5OUT &= ~SDA;
  P5OUT &= ~SCLK;

  P5OUT &= ~RESX;
  delayms(20);
  P5OUT |= RESX;
  delayms(20);

  // Sleep out...
  n6100_send(0x11,2);   
  // Ajusta o contraste...
  n6100_sendcom1(0x25, 0x40);
  // Liga o display e o booster...    
  n6100_send(0x29,2);
  n6100_send(0x03,2);
  delayms(10);  
    
  // Color Format... (8bits/pixel)
  n6100_sendcom1(0x3a, 0x02);
  // Inicializa a tabela de cores 8bits/pixel
  n6100_send(0x2d,2);
  // red
  n6100_send(0x00,0);
  n6100_send(0x02,0);
  n6100_send(0x05,0);
  n6100_send(0x07,0);
  n6100_send(0x09,0);
  n6100_send(0x0b,0);
  n6100_send(0x0d,0);
  n6100_send(0x0f,0);
  // green
  n6100_send(0x00,0);
  n6100_send(0x02,0);
  n6100_send(0x05,0);
  n6100_send(0x07,0);
  n6100_send(0x09,0);
  n6100_send(0x0b,0);
  n6100_send(0x0d,0);
  n6100_send(0x0f,0);
  // blue
  n6100_send(0x00,0);
  n6100_send(0x05,0);
  n6100_send(0x0b,0);
  n6100_send(0x0f,0);

  // Memory Access Control...  
  n6100_sendcom1(0x36, invertido);
  // Agradecimentos...
  lcd_fillrect(0, 0, 132, 132, color);
  //n6100_putlogo(2, 2, 128, 128, (unsigned char *)Matrix);
  //delayms(4000);
}

void lcd_clear(unsigned char color) 
{
  lcd_fillrect(0, 0, 132, 132, color);
}

void n6100_sendcom1(unsigned char comm, unsigned char dat)
{
    n6100_send(comm, 3);
    n6100_send(dat, 0);
}

void n6100_sendcom2(unsigned char comm, unsigned char dat1, unsigned char dat2)
{
    n6100_send(comm, 3);
    n6100_send(dat1, 1);
    n6100_send(dat2, 0);
}

void lcd_fillrect(unsigned char x, unsigned char y, unsigned char lx, unsigned char ly, unsigned char color)
{
    unsigned int addr, max;
    
    n6100_sendcom2(0x2a, x, x+lx-1);
    n6100_sendcom2(0x2b, y, y+ly-1);
    
    n6100_send(0x2c, 3);
    
    max = lx*ly;
    for(addr=0; addr<max; addr++)
    {
      n6100_send(color, 1);
    }
    
    P5OUT &= ~SDA;
    P5OUT |= CSX;
    
    n6100_sendcom2(0x2a, 0, 0x83);
    n6100_sendcom2(0x2b, 0, 0x83);
}


void lcd_drawcircle(unsigned int x0, unsigned int y0, unsigned int radius, unsigned char color, int width){
    int f = 1 - radius;
    int ddF_x = 0;
    int ddF_y = -2 * radius;
    int x = 0;
    int y = radius;
    lcd_fillrect(x0, y0 + radius,width,width, color);
    lcd_fillrect(x0, y0 - radius,width,width, color);
    lcd_fillrect(x0 + radius, y0,width,width, color);
    lcd_fillrect(x0 - radius, y0,width,width, color);
    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x + 1;
        lcd_fillrect(x0 + x, y0 + y,width,width, color);
        lcd_fillrect(x0 - x, y0 + y,width,width, color);
        lcd_fillrect(x0 + x, y0 - y,width,width, color);
        lcd_fillrect(x0 - x, y0 - y,width,width, color);
        lcd_fillrect(x0 + y, y0 + x,width,width, color);
        lcd_fillrect(x0 - y, y0 + x,width,width, color);
        lcd_fillrect(x0 + y, y0 - x,width,width, color);
        lcd_fillrect(x0 - y,y0 - x,width,width, color);   
    }
}

void lcd_drawline(int x0, int y0, int x1, int y1, unsigned char color) {
    int dy = y1 - y0;
    int dx = x1 - x0;
    int stepx, stepy;
    if (dy < 0) { 
        dy = -dy; 
        stepy = -1; 
    } 
    else { 
        stepy = 1; 
    }
    if (dx < 0) {
        dx = -dx; 
        stepx = -1; 
    } 
    else { 
        stepx = 1; 
    }
    dy <<= 1; // dy is now 2*dy
    dx <<= 1; // dx is now 2*dx
    
    lcd_drawpoint(x0, y0, color);

    if (dx > dy) {
        int fraction = dy - (dx >> 1); // same as 2*dy - dx
        while (x0 != x1) {
            if (fraction >= 0) {
                y0 += stepy;
                fraction -= dx; // same as fraction -= 2*dx
            }
            x0 += stepx;
            fraction += dy; // same as fraction -= 2*dy
            lcd_drawpoint(x0, y0, color);
        }
    } 
    else {
        int fraction = dx - (dy >> 1);
        while (y0 != y1) {
            if (fraction >= 0) {
                x0 += stepx;
                fraction -= dy;
            }
            y0 += stepy;
            fraction += dx;
            lcd_drawpoint(x0, y0, color);
        }
    }
}

void lcd_drawpoint(unsigned char x, unsigned char y, unsigned char color)
{
    n6100_sendcom2(0x2a, x, x);
    n6100_sendcom2(0x2b, y, y);
    
    n6100_send(0x2c, 3);
    n6100_send((unsigned char)color, 1);

    
    P5OUT &= ~SDA;
    P5OUT |= CSX;
    
    n6100_sendcom2(0x2a, 0, 0x83);
    n6100_sendcom2(0x2b, 0, 0x83);
}

void lcd_putlogo(unsigned char x, unsigned char y, unsigned char lx, unsigned char ly, unsigned char p[])
{
    unsigned int addr, max;

    n6100_sendcom2(0x2a, x, x+lx-1);
    n6100_sendcom2(0x2b, y, y+ly-1);
    
    n6100_send(0x2c, 3);
    
    max = lx*ly;
    for(addr=0; addr<max; addr++)
    {
        n6100_send(p[addr], 1);
    }
    
    P5OUT &= ~SDA;
    P5OUT |= CSX;
    
    n6100_sendcom2(0x2a, 0, 0x83);
    n6100_sendcom2(0x2b, 0, 0x83);
}

void lcd_newline(void)
{
    if (++LCDLineCount == LCD_NLINE){
        LCDLineCount = 0; 
    }
    LCDCharCount = 0;
}

void lcd_goto(unsigned char x, unsigned char y)
{
    if (x > LCD_NCHAR-1) x = 0;
    if (y > LCD_NLINE-1) y = 0;
    
    LCDCharCount = x;
    LCDLineCount = y;
}

void lcd_wrchar(unsigned char c)
{
  unsigned int j;
  unsigned char i, k, v, mask;

  // Posiciona área para copiar...
  i=6*LCDCharCount;
  n6100_sendcom2(0x2a, i, i+5);
  i=8*(LCDLineCount)+2;
  n6100_sendcom2(0x2b, i, i+7);

  // Rotina de escrita na memória...
  mask = 0x01;
  n6100_send(0x2c,2);

  for (k=0; k<8; k++)
  {
    j = (unsigned int) c * 5;
    for (i=0; i<5; i++)
    {
      v = asciitable[j];
      if(v & mask) n6100_send((unsigned char)LCDForeColor,0); else n6100_send((unsigned char)LCDBackColor,0);
      j += 1;
    }
    n6100_send((unsigned char)LCDBackColor,0);
    mask <<= 1;
  }
}

void lcd_putchar(unsigned char c)
{
  switch (c)
  {
    case 10 : lcd_newline();
              break;
    case 13 : LCDCharCount = 0;
              break;
    default : if (LCDCharCount++ < LCD_NCHAR) lcd_wrchar(c);
              else
              {
                lcd_newline();
                lcd_wrchar(c);
                LCDCharCount++;
              }
  }
}

void lcd_drawprogressbar(int x, int y, int lx, int ly, unsigned char color, unsigned char colorprogress, int percent){
    if(ly < 1 || lx < 1 || percent < 0)
        return;
        
    int lx_progress = (lx * percent) / 100;
    lcd_fillrect(x, y, lx_progress, ly, color);
    lcd_fillrect(x + lx_progress, y, lx - lx_progress, ly, colorprogress);
}


void lcd_putdot(int from, int to){
    if(from > to) return;
    lcd_goto(from, LCDLineCount);
    
    while(from < to){
        lcd_putchar('.');
        from++;
    }
}
