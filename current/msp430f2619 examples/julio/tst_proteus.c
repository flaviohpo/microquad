#include <msp430x2619.h>

// ****************************************************************
// ** T E C L A D O                                              **
// ****************************************************************
//  
#define sw1          port1.in.pin0
#define sw2          port1.in.pin1
#define sw3          port1.in.pin2
#define sw4          port1.in.pin3


// ****************************************************************
// ** C O N S T A N T E S  D O  L C D 16 x 2                     **
// ****************************************************************
//  
#define LCD_NCOL        16
#define LCD_NROW        2
#define LCD_ROW1        0x80
#define LCD_ROW2        0xc0

#define lcd_rs          port3.out.pin6
#define lcd_en          port3.out.pin7
#define lcd_data        P4OUT

#define cursorligado    0x0e
#define cursordesligado 0x0c
#define cursorpiscante  0x0f


// ****************************************************************
// ** Tabela de Convers�o Hexadecimal para ASCII!!!              **
// ****************************************************************
//
const char ConstHexa[16] = { "0123456789ABCDEF" };

// ****************************************************************
// ** VARIAVEIS                                                  **
// ****************************************************************
//  
unsigned char LCD_rowcount;
unsigned char LCD_colcount;


// ****************************************************************
// ** P R O T O T Y P E S                                        **
// ****************************************************************
//
void configura_cpu(void);
void config_serial(void);
void send_serial(unsigned char c);
unsigned char receive_serial(void);

void delay5us(void);
void delayus(unsigned int tempo);
void delayms(unsigned int tempo);

unsigned char readkeyboard(void);
unsigned char waitpress(void);
void waitrelease(void);
void waitkey(unsigned char key);
unsigned char waitanykey(void);

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



// ****************************************************************
// ** MAIN                                                       **
// ****************************************************************
//
int main (void)
{
  unsigned char v;
  configura_cpu();
  config_serial();
  lcd_init(cursorligado);
  while (1)
  {
    send_serial('J');
    delayms(10);
    if((IFG2 & UCA0RXIFG) == 1)
    {
      v = receive_serial();
      lcd_putchar(v);
    }
  }
}

void configura_cpu(void)
{
  WDTCTL = WDTPW + WDTHOLD;                // Stop Watchdog Timer

  BCSCTL1 = CALBC1_16MHZ && 0x0f;          // ACLK = 32KHz, MCLK = 16MHz, SMCLK = 16MHz
  DCOCTL = CALDCO_16MHZ;                   // Ajusta DCO = 16MHz
  BCSCTL2 = 0x88;
  BCSCTL3 = 0x84;

  P1DIR = 0xf0;

  P3SEL = 0x30;                            // P3.4,5 = USCI_A0 TXD/RXD
  P3DIR = 0xdf;

  P4DIR = 0xff;
}

// ** 9600 bps, 8N1 ...
//
void config_serial(void)
{
  UCA0CTL1 = 0x81;
  UCA0CTL0 = 0x00;
  UCA0BR0 = 130;
  UCA0BR1 = 6;
  UCA0MCTL = 0x0c;
  UCA0CTL1 = 0x80;
}

unsigned char receive_serial(void)
{
  while(!(IFG2 & UCA0RXIFG));
  return UCA0RXBUF;
}

void send_serial(unsigned char c)
{
  while(!(IFG2 & UCA0TXIFG));
  UCA0TXBUF = c;                    // TX -> RXed character
}


void delay5us(void)
{
    int i;
    
    for(i = 0; i < 2; i++) nop();
}

// T = 3u * tempo (aproximadamente)
//
void delayus(unsigned int tempo)
{
    int i;
    
    for(i = 0; i < tempo; i++) nop();
}

// Aproximadamente 1ms...
//
void delayms(unsigned int tempo)
{
    int i;
    
    for(i = 0; i < tempo; i++) delayus(800);
}

unsigned char readkeyboard(void)
{
  unsigned char tecla = 0;
  if(sw1 == 0) tecla = '1';
  if(sw2 == 0) tecla = '2';
  if(sw3 == 0) tecla = '3';
  if(sw4 == 0) tecla = '4';

  return tecla;
}

unsigned char waitpress(void)
{
  unsigned char v = 0;
  while(v == 0) 
  {
    v = readkeyboard();
    delayms(1);
  }

  return v;
}

void waitrelease(void)
{
  while(readkeyboard() != 0) delayms(1);
}

void waitkey(unsigned char key)
{
  while(readkeyboard() != key) delayms(1);
  waitrelease();
}

unsigned char waitanykey(void)
{
  unsigned char v;

  v = waitpress();
  waitrelease();

  return v;
}


// ****************************************************************
// ** INICIALIZA��O DO LCD                                       **
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

// ****************************************************************
// ** ESCREVE CARACTER. OBS.: N�o ajusta posi��o do cursor!!!    **
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

// ****************************************************************
// ** MOSTRA O CURSOR                                            **
// ****************************************************************
//  
void lcd_cursoron()
{
  lcd_wrcom(0x0e);
}

// ****************************************************************
// ** ESCONDE O CURSOR                                           **
// ****************************************************************
//  
void lcd_cursoroff()
{
  lcd_wrcom(0x0c);
}

// ****************************************************************
// ** MOSTRA O CURSOR COM ATRIBUTO PISCANTE                      **
// ****************************************************************
//  
void lcd_cursorblink()
{
  lcd_wrcom(0x0f);
}

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

    default : lcd_wrcom(LCD_ROW1);
  }
}

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
    
    default: lcd_wrcom(LCD_ROW1+x);
  }
  delayms(10);
}

// ****************************************************************
// ** IMPLEMENTA��O DO PRINTF...                                 **
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
              }
              break;

    case 10 : LCD_colcount = 0;
              switch (LCD_rowcount)
              {
                case 0 : lcd_wrcom(LCD_ROW1+LCD_colcount);
                break;
                case 1 : lcd_wrcom(LCD_ROW2+LCD_colcount);
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
                }
              }
  }
}

