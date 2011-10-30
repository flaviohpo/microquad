#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "legacymsp430.h"
#include "msp430f2618.h"
#include "color.h"
#include "delay.h"
#include "analog.h"
#include "lcd6100.h"
#include "itg3200.h"
#include "eeprom.h"
#include "i2c.h"
#include "usefullibs.h"
#include "menu.h"
#include "microquad.h"
#include "resources.h"

// variaveis ajuste radio - roll pitch e yaw
int PPMOffset[8] = {0,0,0,0,0,0,0,0};

// variaveis leitura radio raw
unsigned int TimeUpEdge[8] = {0,0,0,0,0,0,0,0};
int PPMValue[8] = {0,0,0,0,0,0,0,0};

// variaveis sensores
int AccelValue[3] = {0, 0, 0};
int GyroValue[3] = {0, 0, 0};

// variaveis do controle
unsigned int ControlProportional[3] = {0,0,0};
// for further implementation
//unsigned int ControlIntegral[3] = {0,0,0};
//unsigned int ControlDerivative[3] = {0,0,0};
unsigned int MotorOutput[6] = {0,0,0,0,0,0};

MENU* MainMenu;
MENU* AnalogMenu;
MENU* MotorMenu;
MENU* ControlMenu;
MENU* RadioMenu;
MENU* SensorMenu;

PROGRAM_STEP ProgStep;

int main(void){
    WDTCTL = WDTPW + WDTHOLD;   // desabilita watchdog
        
    unsigned char MenuDraw = 1;
    
    ACTION act = ACTION_NONE;
    MENU_RESPONSE resp = RESP_NONE;
    ProgStep = PROCESS_MAIN_MENU;
    
    setup();

    while(1){
        switch(ProgStep){
            case PROCESS_MAIN_MENU:
                menu_draw(MainMenu, MenuDraw);
                MenuDraw = 0;                
                act = get_radio_action();
                resp = menu_process(MainMenu, act);
                switch(resp){                    
                    case RESP_SUBMENU_IN:
                        ProgStep = (PROGRAM_STEP)(MainMenu->SelectedItem);
                        MenuDraw = 1;
                    break;
                    
                    case RESP_SUBMENU_OUT:
                    case RESP_NONE:          
                    case RESP_BUSY:
                    case RESP_SEL_MIN_LIMIT:
                    case RESP_SEL_MAX_LIMIT:
                    case RESP_MAX_VALUE:
                    case RESP_MIN_VALUE:
                    case RESP_CHECKED:
                    case RESP_UNCHECKED:
                    case RESP_DONE:
                    break;
                }
            break;
            
            case PROCESS_ANALOG_MENU:
                analog_refresh_all();
                menu_draw(AnalogMenu, MenuDraw);
                MenuDraw = 0;
                menu_refresh(AnalogMenu);
                act = get_radio_action();
                resp = menu_process(AnalogMenu, act);
                switch(resp){
                    case RESP_SUBMENU_OUT:
                        if(AnalogMenu->SelectedItem == RETURN_INDEX){
                            ProgStep = PROCESS_MAIN_MENU;
                            MenuDraw = 1;
                        }
                    break;
                    
                    case RESP_SUBMENU_IN:
                    case RESP_NONE:          
                    case RESP_BUSY:
                    case RESP_SEL_MIN_LIMIT:
                    case RESP_SEL_MAX_LIMIT:
                    case RESP_MAX_VALUE:
                    case RESP_MIN_VALUE:
                    case RESP_CHECKED:
                    case RESP_UNCHECKED:
                    case RESP_DONE:
                    break;
                }
            break;
            
            case PROCESS_MOTOR_MENU:
                menu_draw(MotorMenu, MenuDraw);
                MenuDraw = 0;
                menu_refresh(MotorMenu);
                act = get_radio_action();
                resp = menu_process(MotorMenu, act);
                switch(resp){
                    case RESP_SUBMENU_OUT:
                        if(MotorMenu->SelectedItem == RETURN_INDEX){
                            ProgStep = PROCESS_MAIN_MENU;
                            MenuDraw = 1;
                        }
                        else{ // calibrate index
                            
                        }
                    break;
                    
                    case RESP_SUBMENU_IN:
                    case RESP_NONE:          
                    case RESP_BUSY:
                    case RESP_SEL_MIN_LIMIT:
                    case RESP_SEL_MAX_LIMIT:
                    case RESP_MAX_VALUE:
                    case RESP_MIN_VALUE:
                    case RESP_CHECKED:
                    case RESP_UNCHECKED:
                    case RESP_DONE:
                    break;
                }
            break;
            
            case PROCESS_SENSOR_MENU:
                menu_draw(SensorMenu, MenuDraw);
                MenuDraw = 0;
                menu_refresh(SensorMenu);
                act = get_radio_action();
                resp = menu_process(SensorMenu, act);
                switch(resp){
                    case RESP_SUBMENU_OUT:
                        if(SensorMenu->SelectedItem == RETURN_INDEX){
                            ProgStep = PROCESS_MAIN_MENU;
                            MenuDraw = 1;
                        }
                    break;
                    
                    case RESP_SUBMENU_IN:
                    case RESP_NONE:          
                    case RESP_BUSY:
                    case RESP_SEL_MIN_LIMIT:
                    case RESP_SEL_MAX_LIMIT:
                    case RESP_MAX_VALUE:
                    case RESP_MIN_VALUE:
                    case RESP_CHECKED:
                    case RESP_UNCHECKED:
                    case RESP_DONE:
                    break;
                }
            break;
            
            case PROCESS_RADIO_MENU:
                menu_draw(RadioMenu, MenuDraw);
                MenuDraw = 0;
                menu_refresh(RadioMenu);
                act = get_radio_action();
                resp = menu_process(RadioMenu, act);
                switch(resp){
                    case RESP_SUBMENU_OUT:
                        if(RadioMenu->SelectedItem == RETURN_INDEX){
                            ProgStep = PROCESS_MAIN_MENU;
                            MenuDraw = 1;
                        }
                    break;
                    
                    case RESP_SUBMENU_IN:
                    case RESP_NONE:          
                    case RESP_BUSY:
                    case RESP_SEL_MIN_LIMIT:
                    case RESP_SEL_MAX_LIMIT:
                    case RESP_MAX_VALUE:
                    case RESP_MIN_VALUE:
                    case RESP_CHECKED:
                    case RESP_UNCHECKED:
                    case RESP_DONE:
                    break;
                }
            break;
            
            case PROCESS_CONTROL_MENU:
                menu_draw(ControlMenu, MenuDraw);
                MenuDraw = 0;
                menu_refresh(ControlMenu);
                act = get_radio_action();
                resp = menu_process(ControlMenu, act);
                switch(resp){
                    case RESP_SUBMENU_OUT:
                        if(ControlMenu->SelectedItem == RETURN_INDEX){
                            ProgStep = PROCESS_MAIN_MENU;
                            MenuDraw = 1;
                        }
                    break;
                    
                    case RESP_SUBMENU_IN:
                    case RESP_NONE:          
                    case RESP_BUSY:
                    case RESP_SEL_MIN_LIMIT:
                    case RESP_SEL_MAX_LIMIT:
                    case RESP_MAX_VALUE:
                    case RESP_MIN_VALUE:
                    case RESP_CHECKED:
                    case RESP_UNCHECKED:
                    case RESP_DONE:
                    break;
                }
            break;
            
            case PROCESS_CONTROL:
            break;
        }
    }
}


interrupt (TIMERA0_VECTOR) TIMERA0_ISR_HOOK(void){ // 2ms
    // ta livre
}

// ok
void clock_init(void){
    BCSCTL2 = SELM_0 + DIVM_0 + DIVS_0;
    DCOCTL = 0x77;      
    BCSCTL1 = 0xCF;     
    BCSCTL3 = 0xA0;
}

// ok
void P1_init(void){
    /* Port 1 Output Register */
    P1OUT = PPM_P1MASK;
    /* Port 1 Resistor Enable Register */
    P1REN = PPM_P1MASK;
    /* Port 1 Interrupt Edge Select Register */
    P1IES = 0; // subida
    /* Port 1 Interrupt Flag Register */
    P1IFG = 0;
    /* Port 1 Interrupt Enable Register */
    P1IE = PPM_P1MASK;
}

void P2_init(void){ // p2 eh interruption do gyro
#ifdef ITG3200
    /* Port 2 Output Register */
    P2OUT = 0x01;
    /* Port 2 Resistor Enable Register */
    P2REN = 0x01;
    /* Port 2 Interrupt Edge Select Register */
    P2IES = 0; // subida
    /* Port 2 Interrupt Flag Register */
    P2IFG = 0;
    /* Port 2 Interrupt Enable Register */
    P2IE = 0x01;
#endif
}

void timer_init(void){
    /* 
     * TBCCTL1, Capture/Compare Control Register 1
     * 
     * CM_0 -- No Capture
     * CCIS_0 -- CCIxA
     * OUTMOD_7 -- PWM output mode: 7 - PWM reset/set
     */

    TBCCTL1 = CM_0 + CCIS_0 + CLLD_0 + OUTMOD_7;
    TBCCTL2 = CM_0 + CCIS_0 + CLLD_0 + OUTMOD_7;
    TBCCTL3 = CM_0 + CCIS_0 + CLLD_0 + OUTMOD_7;
    TBCCTL4 = CM_0 + CCIS_0 + CLLD_0 + OUTMOD_7;
    TBCCTL5 = CM_0 + CCIS_0 + CLLD_0 + OUTMOD_7;
    TBCCTL6 = CM_0 + CCIS_0 + CLLD_0 + OUTMOD_7;

    /* TBCCR0, Timer_B Capture/Compare Register 0 */
    TBCCR0 = 5025;

    /* TBCCR1, Timer_B Capture/Compare Register 1 */
    TBCCR1 = 2000; 
    TBCCR2 = 2000; 
    TBCCR3 = 2000; 
    TBCCR4 = 2000; 
    TBCCR5 = 2000; 
    TBCCR6 = 2000; 

    /* 
     * TBCTL, Timer_B3 Control Register
     * 
     * CNTL_0 -- 16-bit, TBR(max) = 0FFFFh
     * TBSSEL_1 -- ACLK
     * ID_3 -- Divider - /8
     * MC_1 -- Up Mode
     */
    TBCTL = TBCLGRP_0 + CNTL_0 + TBSSEL_1 + ID_3 + MC_1;
    
    /* configura saidas do timerB - P4.1 a P4.6*/
    P4SEL = 0x7E;
    P4DIR = 0x7E;
    
    /* 
     * TACCTL0, Capture/Compare Control Register 0
     * 
     * CM_0 -- No Capture
     * CCIS_0 -- CCIxA
     * ~SCS -- Asynchronous Capture
     * ~SCCI -- Latched capture signal (read)
     * ~CAP -- Compare mode
     * OUTMOD_0 -- PWM output mode: 0 - OUT bit value
     * 
     * Note: ~<BIT> indicates that <BIT> has value zero
     */
    TACCTL0 = CM_0 + CCIS_0 + OUTMOD_0;

    /* TACCR0, Timer_A Capture/Compare Register 0 */
    TACCR0 = 4001; //2ms

    TACCTL1 = CM_0 + CCIS_0 + OUTMOD_7;

    TACTL = TASSEL_1 + ID_3 + MC_1;
    TACCR1 = 0;

    P2SEL |= 0x08;
    P2DIR |= 0x08;
}


unsigned int PPM_aux;
int channel_num, PPM_ch_counter;
// muito consagred
interrupt (PORT1_VECTOR) PORT1_ISR_HOOK(void){
    PPM_aux = TAR; // captura aqui pra ser mais exato
    // aqui eh usado PPM_P1_MASK, pq eh a interrupcao da P1 
    channel_num = 0;
    for(PPM_ch_counter = 0x01; PPM_ch_counter <= 0x80; PPM_ch_counter = PPM_ch_counter << 1){
        if(P1IFG & (PPM_ch_counter & PPM_P1MASK)){
            if(!(P1IES & PPM_ch_counter)){ // low to high
                TimeUpEdge[channel_num] = PPM_aux;
                P1IES |= (PPM_ch_counter & PPM_P1MASK); // configura high to low
            }
            else{ // high to low
            
                if(TimeUpEdge[channel_num] > PPM_aux){ // se deu overflow na contagem do timer
                    PPMValue[channel_num] = (TACCR0 - TimeUpEdge[channel_num] + PPM_aux);
                }
                else{ // se nao deu overflow na contagem do timer
                    PPMValue[channel_num] = (PPM_aux - TimeUpEdge[channel_num]);
                }
                P1IES &= ~(PPM_ch_counter & PPM_P1MASK); // configure low to high      
            }
            P1IFG &= ~(PPM_ch_counter & PPM_P1MASK); // apaga o interruption flag da P1.7
        }
        channel_num++;
    }
}


interrupt (PORT2_VECTOR) PORT2_ISR_HOOK(void){

    P2IFG = 0x00;
}

void setup(void){   
    //P2DIR |= 0x08;
    //P2OUT |= 0x08;
    
    clock_init();
    timer_init(); // inicia com 0 ppm no ESC (ESC fica esperando sinal valido beep --- beep --- beep)

    lcd_init(LCDBackColor);
    
    P1_init(); // ppm receiver interrupt
    P2_init(); // itg3200 sample interrupt

    // iluminacao progressiva do LCD
    while(TACCR1 < LCD_MAX_BRIGHT){
        TACCR1 = TACCR1 + 1;
        delayus(1200);
    }
    
    analog_init(); // config registradores
    eint(); // habilita interrupt
    
    menu_init();
    
}

void menu_init(void){
    MainMenu = menu_create(str_mainmenu, create_item(str_analogmenu, ITEMTYPE_SUBMENU, NULL, NULL, NULL, NULL), &val_janela_size_medium, NULL, NULL, &val_arrow_center, NULL);
    menu_add_item(MainMenu, create_item(str_radiomenu, ITEMTYPE_SUBMENU, NULL, NULL, NULL, NULL));
    menu_add_item(MainMenu, create_item(str_motormenu, ITEMTYPE_SUBMENU, NULL, NULL, NULL, NULL));
    menu_add_item(MainMenu, create_item(str_controlmenu, ITEMTYPE_SUBMENU, NULL, NULL, NULL, NULL));
    menu_add_item(MainMenu, create_item(str_sensormenu, ITEMTYPE_SUBMENU, NULL, NULL, NULL, NULL));
    menu_add_item(MainMenu, create_item(str_letsfly, ITEMTYPE_SUBMENU, NULL, NULL, NULL, NULL));
    // analog menu
    AnalogMenu = menu_create(str_analogmenu, create_item(str_return, ITEMTYPE_SUBMENU, NULL, NULL, NULL, NULL), &val_janela_size_medium, &val_bar_position_center, &val_value_position_right, &val_arrow_center, &val_bar_lenght_medium);
    menu_add_item(AnalogMenu, create_item(str_analogch0, ITEMTYPE_VALUE_R, &val_zero, &val_max_analog, NULL, (int*)&AnalogValue[0]));
    menu_add_item(AnalogMenu, create_item(str_analogch1, ITEMTYPE_VALUE_R, &val_zero, &val_max_analog, NULL, (int*)&AnalogValue[1]));
    menu_add_item(AnalogMenu, create_item(str_analogch2, ITEMTYPE_VALUE_R, &val_zero, &val_max_analog, NULL, (int*)&AnalogValue[2]));
    menu_add_item(AnalogMenu, create_item(str_analogch3, ITEMTYPE_VALUE_R, &val_zero, &val_max_analog, NULL, (int*)&AnalogValue[3]));
    menu_add_item(AnalogMenu, create_item(str_analogch4, ITEMTYPE_VALUE_R, &val_zero, &val_max_analog, NULL, (int*)&AnalogValue[4]));
    menu_add_item(AnalogMenu, create_item(str_analogch5, ITEMTYPE_VALUE_R, &val_zero, &val_max_analog, NULL, (int*)&AnalogValue[5]));    
    menu_add_item(AnalogMenu, create_item(str_analogch6, ITEMTYPE_VALUE_R, &val_zero, &val_max_analog, NULL, (int*)&AnalogValue[6]));    
    menu_add_item(AnalogMenu, create_item(str_analogch7, ITEMTYPE_VALUE_R, &val_zero, &val_max_analog, NULL, (int*)&AnalogValue[7]));    
    // control menu
    ControlMenu = menu_create(str_controlmenu, create_item(str_return, ITEMTYPE_SUBMENU, NULL, NULL, NULL, NULL), &val_janela_size_medium, &val_bar_position_center, &val_value_position_right, &val_arrow_center, &val_bar_lenght_medium);
    menu_add_item(ControlMenu, create_item(str_yawp, ITEMTYPE_VALUE_RW, &val_zero, &val_max_control, &val_one, (int*)&ControlProportional[YAW_INDEX]));
    menu_add_item(ControlMenu, create_item(str_pitchp, ITEMTYPE_VALUE_RW, &val_zero, &val_max_control, &val_one, (int*)&ControlProportional[PITCH_INDEX]));
    menu_add_item(ControlMenu, create_item(str_rollp, ITEMTYPE_VALUE_RW, &val_zero, &val_max_control, &val_one, (int*)&ControlProportional[ROLL_INDEX]));        
    // radio menu
    RadioMenu = menu_create(str_radiomenu, create_item(str_return, ITEMTYPE_SUBMENU, NULL, NULL, NULL, NULL), &val_janela_size_medium, &val_bar_position_center, &val_value_position_right, &val_arrow_center, &val_bar_lenght_medium);
    menu_add_item(RadioMenu, create_item(str_radioch0, ITEMTYPE_VALUE_R, &val_min_radio, &val_max_radio, NULL, (int*)&PPMValue[0]));
    menu_add_item(RadioMenu, create_item(str_radioch1, ITEMTYPE_VALUE_R, &val_min_radio, &val_max_radio, NULL, (int*)&PPMValue[1]));
    menu_add_item(RadioMenu, create_item(str_radioch2, ITEMTYPE_VALUE_R, &val_min_radio, &val_max_radio, NULL, (int*)&PPMValue[2]));
    menu_add_item(RadioMenu, create_item(str_radioch3, ITEMTYPE_VALUE_R, &val_min_radio, &val_max_radio, NULL, (int*)&PPMValue[3]));
    menu_add_item(RadioMenu, create_item(str_radioch4, ITEMTYPE_VALUE_R, &val_min_radio, &val_max_radio, NULL, (int*)&PPMValue[4]));
    menu_add_item(RadioMenu, create_item(str_radioch5, ITEMTYPE_VALUE_R, &val_min_radio, &val_max_radio, NULL, (int*)&PPMValue[5]));    
    menu_add_item(RadioMenu, create_item(str_radioch6, ITEMTYPE_VALUE_R, &val_min_radio, &val_max_radio, NULL, (int*)&PPMValue[6]));    
    menu_add_item(RadioMenu, create_item(str_radioch7, ITEMTYPE_VALUE_R, &val_min_radio, &val_max_radio, NULL, (int*)&PPMValue[7]));
    menu_add_item(RadioMenu, create_item(str_calibrate, ITEMTYPE_SUBMENU, NULL, NULL, NULL, NULL));            
    // motor menu
    
    MotorMenu = menu_create(str_motormenu, create_item(str_return, ITEMTYPE_SUBMENU, NULL, NULL, NULL, NULL), &val_janela_size_medium, &val_bar_position_center, &val_value_position_right, &val_arrow_center, &val_bar_lenght_medium);
    menu_add_item(MotorMenu, create_item(str_motor1, ITEMTYPE_VALUE_RW, &val_min_motor, &val_max_motor, NULL, (int*)&MotorOutput[0]));
    menu_add_item(MotorMenu, create_item(str_motor2, ITEMTYPE_VALUE_RW, &val_min_motor, &val_max_motor, NULL, (int*)&MotorOutput[1]));
    menu_add_item(MotorMenu, create_item(str_motor3, ITEMTYPE_VALUE_RW, &val_min_motor, &val_max_motor, NULL, (int*)&MotorOutput[2]));
    menu_add_item(MotorMenu, create_item(str_motor4, ITEMTYPE_VALUE_RW, &val_min_motor, &val_max_motor, NULL, (int*)&MotorOutput[3]));
    menu_add_item(MotorMenu, create_item(str_motor5, ITEMTYPE_VALUE_RW, &val_min_motor, &val_max_motor, NULL, (int*)&MotorOutput[4]));
    menu_add_item(MotorMenu, create_item(str_motor6, ITEMTYPE_VALUE_RW, &val_min_motor, &val_max_motor, NULL, (int*)&MotorOutput[5]));    
    menu_add_item(MotorMenu, create_item(str_calibrate, ITEMTYPE_SUBMENU, NULL, NULL, NULL, NULL));    
    // sensor menu
    SensorMenu = menu_create(str_sensormenu, create_item(str_return, ITEMTYPE_SUBMENU, NULL, NULL, NULL, NULL), &val_janela_size_medium, &val_bar_position_center, &val_value_position_right, &val_arrow_center, &val_bar_lenght_medium);
    menu_add_item(SensorMenu, create_item(str_accelx, ITEMTYPE_VALUE_R, &val_zero, &val_max_analog, NULL, (int*)&AccelValue[ACCEL_X_INDEX]));
    menu_add_item(SensorMenu, create_item(str_accely, ITEMTYPE_VALUE_R, &val_zero, &val_max_analog, NULL, (int*)&AccelValue[ACCEL_Y_INDEX]));
    menu_add_item(SensorMenu, create_item(str_accelz, ITEMTYPE_VALUE_R, &val_zero, &val_max_analog, NULL, (int*)&AccelValue[ACCEL_Z_INDEX]));
    menu_add_item(SensorMenu, create_item(str_gyrox, ITEMTYPE_VALUE_R, &val_zero, &val_max_analog, NULL, (int*)&GyroValue[GYRO_X_INDEX]));
    menu_add_item(SensorMenu, create_item(str_gyroy, ITEMTYPE_VALUE_R, &val_zero, &val_max_analog, NULL, (int*)&GyroValue[GYRO_Y_INDEX]));
    menu_add_item(SensorMenu, create_item(str_gyroz, ITEMTYPE_VALUE_R, &val_zero, &val_max_analog, NULL, (int*)&GyroValue[GYRO_Z_INDEX]));    
    menu_add_item(SensorMenu, create_item(str_graph, ITEMTYPE_SUBMENU, NULL, NULL, NULL, NULL));    
}

ACTION get_radio_action(void){
    if(PPMValue[RADIO_ROLL_CH] > STICK_UPPER_THRESHOLD){
        return ACTION_RIGHT;
    }
    if(PPMValue[RADIO_ROLL_CH] < STICK_LOWER_THRESHOLD){
        return ACTION_LEFT;
    }
    if(PPMValue[RADIO_PITCH_CH] < STICK_LOWER_THRESHOLD){
        return ACTION_UP;
    }
    if(PPMValue[RADIO_PITCH_CH] > STICK_UPPER_THRESHOLD){
        return ACTION_DOWN;
    }
    return ACTION_NONE;
}
