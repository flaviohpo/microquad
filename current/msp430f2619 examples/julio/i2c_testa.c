#include <msp430x2619.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lcd16x2.h"
#include "i2c_sm.h"
#include "delay.h"
#include "LabProcI.h"

// Protótipos...
//
int putchar(int c);

// M A I N
//
int main (void)
{
  unsigned char resp, i, buff[80], buff_rx[80];

  configura_cpu();
//  lcd_initmodule(nao_virado);
  lcd_init(cursordesligado);
  i2c_config(SMART_CARD, speed_100);
  
  printf("ROTINAS de I2C\r\n");
  if(i2c_find_device(SMART_CARD) == ACK)
  {
    lcd_goto(0,0);
    printf("Smart Card ok...\r\n");
    strcpy(buff, "Julio Cesar");
    resp = i2c_write_one_buffer(0x00, buff, 12);
    delayms(10);
    lcd_goto(0,1);
    if(resp == ACK) printf("Escrita ok...\r\n");
    
    delayms(1000);
    lcd_goto(0,1);
    resp = i2c_wread_one_buffer(0x00, buff_rx, 12);
    if(resp == ACK)
    {
      printf("Leitura ok...\r\n");
      for(i=0; i<12; i++) printf("%c", buff_rx[i]);
    }
  } else printf("Smart Card nok!\r\n");

  while(1)
  {
  }    

  return 0;
}   

// Implementação do Printf...
//
int putchar(int c)
{
  lcd_putchar(c);
  return 0;
}




