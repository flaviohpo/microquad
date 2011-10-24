//#include <msp430x44x.h>
#include <msp430xG46x.h>
#include "system.h"
#include "mma.h"
#include "lcd.h"
#include "bits.h"
#include "test.h"
#include "periphery.h"
#include "at_flash.h"
#include "irda.h"
#include "mmc.h"
#include "rtc.h"
#include "menu.h"

#define POS_COL_START       0x3d
#define POS_COL_END         0x44
#define POS_ROW_START       0x00
#define POS_ROW_END         0x0f



// MMA coordinates array
extern int Coordinates[3];
unsigned int vcc;
int tmp, g;
int row_factor;
int col_factor;
int row_base;
int col_base;
char menu_pos;
char new_level_flag;
char jpos;
char j_flag=0;
char r=50,c=50;
char t_r, t_c;

void Delay (unsigned long a) { while (--a!=0); }

void TestJoystick(void) {

  // Center button as input
  P2DIR &= ~BIT6;

  while(1) {

 //  if(P2IN&BIT6)
 //     ClearLCD();

    t_r = r;
    t_c = c;

    jpos = GetJoystickPosition();
    Delay(100);

    switch(jpos) {
        case J_RIGHT: c++; break;
        case J_LEFT:  c--; break;
        case J_UP:    r--; break;
        case J_DOWN:  r++; break;
        case J_CENTER: InitRTC(); break;
        default:           break;
    }

    // clear ball
    if((c!=t_c)||(r!=t_r)) {
      ClearBall(t_r, t_c);
    }


    if(r>180)     r=1;
    if(r>110)     r=110;
    if(c>180)     c=1;
    if(c>110)     c=110;

    WriteBall(r, c);
  }

}

void TestMMA(void) {

  // calibrate
  for(g=0; g<64; g++) {
    GetCoordinates();
  }


  row_base = Coordinates[1];
  col_base = Coordinates[0];

  while(1) {

    // mma and ball ------------------------------------------------------------
    GetCoordinates();

    t_r = r;
    t_c = c;

    // with calibrate when start
    r = r + (Coordinates[1] - row_base)/100;
    c = c + (Coordinates[0] - col_base)/100;

    // clear ball
    if((c!=t_c)||(r!=t_r)) {
      ClearBall(t_r, t_c);
    }

    if(r>180)     r=1;
    if(r>110)     r=110;
    if(c>180)     c=1;
    if(c>110)     c=110;

    WriteBall(r, c);
    // -------------------------------------------------------------------------
  }

}

int main( void )
{

  // Stop watchdog timer
  WDTCTL = WDTPW + WDTHOLD;

  // Freq init
  //InitFreq_XT1(); // (32KHz)
  InitFreq_XT2(); // (8MHz)
  //InitFreq();     // (DCO: 8MHz)

  Delay(50000);

  // GPIO init
  GPIOInit();

  // Buttons init
  InitButtons();

  // Init Joystick
  InitJoystick();

  // MMA init
  InitMMA();

  // Interrupt enable
  _EINT();


  // Init LCD
  InitLCD();
  LCDSettings();
  DrawMenu();

  menu_pos=4;
  new_level_flag=1;
  // ClearLCD();
  DrawPointer(menu_pos, 1);

  //while(((P1IN&BIT7)==BIT7)||((P2IN&BIT6)==BIT6)) {
  while((P1IN&BIT7)==BIT7) {
    // joystick ----------------------------------------------------------------
    jpos = GetJoystickPosition();
    Delay(10000);

    switch(jpos) {
      case J_RIGHT: continue;   // break;
      case J_LEFT:  continue;   // break;
      case J_UP:    if(j_flag==0) {
                      DrawPointer(menu_pos, 0);
                      menu_pos--;
                      if(menu_pos>5) menu_pos=0;
                      DrawPointer(menu_pos, 1);
                      j_flag=1;
                    }
                    break;
      case J_DOWN:  if(j_flag==0) {

                      DrawPointer(menu_pos, 0);
                      menu_pos++;
                      if(menu_pos>5) menu_pos=5;
                      DrawPointer(menu_pos, 1);
                      j_flag=1;
                    }
                    break;
      default:      j_flag=0;
                    break;
    }
  }

  // check choise item
  if(menu_pos==0) {
    if(TestMMC()==1) {
      LCDWriteTestSuccess();
      while(1);
    }
    else {
      LCDWriteTestFailed();
      while(1);
    }
  }

  if(menu_pos==1) {
    if(TestATFlash()) {
      LCDWriteTestSuccess();
      while(1);
    }
    else {
      LCDWriteTestFailed();
      while(1);
    }
  }

  if(menu_pos==2) {
    if(TestIRDA()) {
      LCDWriteTestSuccess();
      while(1);
    }
    else {
      LCDWriteTestFailed();
      while(1);
    }
  }

  if(menu_pos==3) {

    if(TestUEXT()) {
      InitLCD();
      LCDWriteTestSuccess();
    }
    else {
      InitLCD();
      LCDWriteTestFailed();
      while(1);
    }

    // B2 as input
    P1DIR &= ~BIT7;

    Delay(1000);

    // continue test EXT when press B2
    while((P1IN&BIT7)==BIT7);

    if(TestEXT()) {
      InitLCD();
      LCDWriteTestSuccess();
      while(1);
    }
    else {
      InitLCD();
      LCDWriteTestFailed();
      while(1);
    }
  }

  if(menu_pos==4) {
    ClearLCD();
    InitMMA();
    TestMMA();
  }

  if(menu_pos==5) {
    ClearLCD();
    TestJoystick();
  }

  while(1);

/*
  // calibrate
  for(g=0; g<64; g++) {
    GetCoordinates();
  }


  //row_factor = Coordinates[1] - MeasureHalfVCC();
  //col_factor = Coordinates[0] - MeasureHalfVCC();
  row_base = Coordinates[1];
  col_base = Coordinates[0];

  // Sleep
  // GoToSleep();
  // LPM4;


  while(1) {

     // joystick ----------------------------------------------------------------
     jpos = GetJoystickPosition();

    switch(jpos) {
        case J_RIGHT: LCDWriteArrow(0,POS_COL_START,POS_COL_END,POS_ROW_START,POS_ROW_END,0x0070,0x000F); continue; break;
        case J_LEFT:  LCDWriteArrow(1,POS_COL_START,POS_COL_END,POS_ROW_START,POS_ROW_END,0x0070,0x000F); continue; break;
        case J_UP:    LCDWriteArrow(2,POS_COL_START,POS_COL_END,POS_ROW_START,POS_ROW_END,0x0070,0x000F); continue; break;
        case J_DOWN:  LCDWriteArrow(3,POS_COL_START,POS_COL_END,POS_ROW_START,POS_ROW_END,0x0070,0x000F); continue; break;
        default:      LCDWriteArrow(0,POS_COL_START,POS_COL_END,POS_ROW_START,POS_ROW_END,0xFFFF,0xFFFF); break;

        // case J_RIGHT: c++; break;
        // case J_LEFT:  c--; break;
        // case J_UP:    r--; break;
        // case J_DOWN:  r++; break;
        // default:           break;
    }
    // -------------------------------------------------------------------------

    // mma and ball ------------------------------------------------------------
    GetCoordinates();

    t_r = r;
    t_c = c;

    // fixed measure value
    //r = r + ((Coordinates[1]/10) - 243)/10;
    //c = c + ((Coordinates[0]/10) - 187)/10;

    // enteligent, but dont work corectly
    //r = r + (int)(Coordinates[1] - vcc - row_factor)/100;
    //c = c + (int)(Coordinates[0] - vcc - col_factor)/100;

    // with calibrate when start
    r = r + (Coordinates[1] - row_base)/100;
    c = c + (Coordinates[0] - col_base)/100;

    // clear ball
    if((c!=t_c)||(r!=t_r)) {
      ClearBall(t_r, t_c);
    }

    if(r>180)     r=1;
    if(r>110)     r=110;
    if(c>180)     c=1;
    if(c>110)     c=110;

    WriteBall(r, c);
    // -------------------------------------------------------------------------

    if(P2IN&BIT6) {
      ClearLCD();

      // calibrate
      for(g=0; g<64; g++) {
        GetCoordinates();
      }

      row_base = Coordinates[1];
      col_base = Coordinates[0];
    }

  }
*/
}
