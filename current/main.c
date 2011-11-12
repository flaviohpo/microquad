/*

DO!

melhorar o lowpass do gyro (deixar mais rapido)
procurar o que ta travando o controle
graficos analog
gravar parametros
carregar parametros
fazer tudo pro itg3200
adaptar acelerometro
fazer PID
utilizar as variaveis ao inves das constantes

*/

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
int AccelValue[3] = {0,0,0};
int AccelOffset[3] = {0,0,0};
int GyroValue[3] = {0,0,0};
int GyroOffset[3] = {0,0,0};
int ThrottleFiltered = 0;
// variaveis do controle
// for further implementation
//unsigned int ControlIntegralMul[3] = {0,0,0};
//unsigned int ControlIntegralDiv[3] = {0,0,0};
//unsigned int ControlDerivativeMul[3] = {0,0,0};
//unsigned int ControlDerivativeDiv[3] = {0,0,0};
unsigned int ControlProportionalMul[3] = {YAW_PROPORTIONAL_MUL, PITCH_PROPORTIONAL_MUL, ROLL_PROPORTIONAL_MUL};
unsigned int ControlProportionalDiv[3] = {YAW_PROPORTIONAL_DIV, PITCH_PROPORTIONAL_DIV, ROLL_PROPORTIONAL_DIV};
unsigned int ControlReferenceDiv[3] = {YAW_REF_DIV, PITCH_REF_DIV, ROLL_REF_DIV};
int ControlOldSample[3] = {0, 0, 0};
//unsigned int ControlResult[3] = {0,0,0};
//int ControlReference[3] = {0,0,0};

unsigned int MotorOutput[6] = {MIN_MOTOR,MIN_MOTOR,MIN_MOTOR,MIN_MOTOR,MIN_MOTOR,MIN_MOTOR};
unsigned int MotorOutputOld[6] = {MIN_MOTOR,MIN_MOTOR,MIN_MOTOR,MIN_MOTOR,MIN_MOTOR,MIN_MOTOR};

// general purpose
unsigned int FlightTime = 0; // guarda ate 9h de voo
volatile unsigned char ControlSample = 0;

// frase de entrada?
// imagem de entrada?
// sinais luminosos?

MENU* MainMenu;
MENU* AnalogMenu;
MENU* MotorMenu;
MENU* ControlMenu;
MENU* RadioMenu;
MENU* SensorMenu;
MENU* OptionMenu;

PROGRAM_STEP ProgStep;

int main(void){
    WDTCTL = WDTPW + WDTHOLD;   // desabilita watchdog
        
    unsigned char MenuDraw = 0;
    unsigned int BatteryColor = LIME;

    ACTION act = ACTION_NONE;
    MENU_RESPONSE resp = RESP_NONE;
    ProgStep = PROCESS_MAIN_MENU;
    
    clock_init(); 
    
    setup();
            
    menu_draw(MainMenu, 1);
    
    // iluminacao progressiva do LCD
    while(TACCR1 < LCD_MAX_BRIGHT){
        TACCR1 = TACCR1 + 1;
        delayus(1200);
    }
    
    while(1){
        
        analog_refresh_all();
        adjust_readings();
              
        switch(ProgStep){
            case PROCESS_MAIN_MENU:
                menu_draw(MainMenu, MenuDraw);
                if(get_delay(1)){
                    if(AnalogValue[BATTERY_ACH] < BATTERY_RED){
                        BatteryColor = RED;
                    }      
                    if(BatteryColor != RED){
                        if(AnalogValue[BATTERY_ACH] < BATTERY_YELLOW){
                            BatteryColor = YELLOW;
                        }
                    }
                    //draw batterybar
                    lcd_drawprogressbar(108, 2, 20, 4, BatteryColor, BLACK, ((AnalogValue[BATTERY_ACH]-MIN_BATTERY)/((MAX_BATTERY-MIN_BATTERY)/100)));                                
                    set_delay(1, BATTERY_CHECK_DELAY);
                    
                    //print flighttime
                    lcd_goto(11,0);
                    printf("%ds", FlightTime);
                }

                MenuDraw = 0;                
                act = get_radio_action();
                resp = menu_process(MainMenu, act);
                switch(resp){                    
                    case RESP_SUBMENU_IN:
                        ProgStep = (PROGRAM_STEP)(MainMenu->SelectedItem);
                        MenuDraw = 1;
                    break;
                    case RESP_EMERGENCY:
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
                        if(AnalogMenu->SelectedItem == CALIBR_INDEX){
                            analog_calibrate_channel(0);
                            analog_calibrate_channel(1);
                            analog_calibrate_channel(2);
                            analog_calibrate_channel(3);
                            analog_calibrate_channel(4);
                            analog_calibrate_channel(5);
                            analog_calibrate_channel(6);
                            analog_calibrate_channel(7);
                        }                    
                    break;                    
                    
                    case RESP_EMERGENCY:
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
                if(MenuDraw == 1){
                    set_all_motors(MIN_MOTOR);
                }
                MenuDraw = 0;
                set_motor_output();
                menu_refresh(MotorMenu);
                act = get_radio_action();
                resp = menu_process(MotorMenu, act);
                
                switch(resp){
                    case RESP_SUBMENU_OUT:
                        if(MotorMenu->SelectedItem == RETURN_INDEX){
                            set_all_motors(MIN_MOTOR);
                            ProgStep = PROCESS_MAIN_MENU;
                            MenuDraw = 1;
                        }
                        else{ // calibrate index
                            
                        }
                    break;
                    
                    case RESP_EMERGENCY:
                        set_all_motors(MIN_MOTOR);
                        screen_flash(RED, 200, 2);
                        ProgStep = PROCESS_MAIN_MENU;
                        MenuDraw = 1;
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
                    
                    case RESP_EMERGENCY:
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
                        if(RadioMenu->SelectedItem == CALIBR_INDEX){
                            calibrate_radio();
                        }                    
                    break;                    
                    
                    case RESP_EMERGENCY:
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
                    
                    case RESP_EMERGENCY:
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
            
            case PROCESS_OPTION_MENU:
                menu_draw(OptionMenu, MenuDraw);
                MenuDraw = 0;
                act = get_radio_action();
                resp = menu_process(OptionMenu, act);
                switch(resp){
                    case RESP_SUBMENU_OUT:
                        ProgStep = PROCESS_MAIN_MENU;
                        MenuDraw = 1;
                    break;
                    case RESP_SUBMENU_IN:
                        switch(OptionMenu->SelectedItem){
                            case SAVE_INDEX:
                                if(save_params()){
                                    // error processing 
                                }
                            break;
                            
                            case RESET_INDEX:
                                reset_defaults();
                            break;
                        }
                        
                    break;
                    case RESP_EMERGENCY:
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
                if(TACCR1 > 0){ //apaga tela
                    TACCR1 = TACCR1 - 1;
                    delayus(1000);
                }
                else{
                    if(ControlSample == 1){
                        ControlSample = 0;
                        control_loop();
                    }
                }
                    
                act = get_radio_action();
                switch(act){
                    case ACTION_EMERGENCY:
                        set_all_motors(MIN_MOTOR);
                        screen_flash(BLACK,500,2);
                        ProgStep = PROCESS_MAIN_MENU;
                        MenuDraw = 1;
                    break;
                    case ACTION_NONE:
                    case ACTION_UP:      
                    case ACTION_DOWN:     
                    case ACTION_LEFT:     
                    case ACTION_RIGHT:     
                    break;
                }
            break;
        }
    }
}

// tudo para 16MHz
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

void set_motor_output(void){
    TBCCR1 = constrain(MotorOutput[0], 0, MAX_MOTOR);
    TBCCR2 = constrain(MotorOutput[1], 0, MAX_MOTOR);
    TBCCR3 = constrain(MotorOutput[2], 0, MAX_MOTOR);
    TBCCR4 = constrain(MotorOutput[3], 0, MAX_MOTOR);
    TBCCR5 = constrain(MotorOutput[4], 0, MAX_MOTOR);
    TBCCR6 = constrain(MotorOutput[5], 0, MAX_MOTOR);
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
    TBCCR0 = 4512; // 2.5ms = 400hz

    /* TBCCR1, Timer_B Capture/Compare Register 1 */
    TBCCR1 = 0; 
    TBCCR2 = 0; 
    TBCCR3 = 0; 
    TBCCR4 = 0; 
    TBCCR5 = 0; 
    TBCCR6 = 0; 

    /* 
     * TBCTL, Timer_B3 Control Register
     * 
     * CNTL_0 -- 16-bit, TBR(max) = 0FFFFh
     * TBSSEL_1 -- ACLK
     * ID_3 -- Divider - /8
     * MC_1 -- Up Mode
     */
    TBCTL = TBCLGRP_0 + CNTL_0 + TBSSEL_1 + ID_3 + MC_1 + TBIE;
    
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

    TACTL = TASSEL_1 + ID_3 + MC_1 + TAIE;
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
    int i = 100;
    
    clock_init();
    timer_init(); // inicia com 0 ppm no ESC (ESC fica esperando sinal valido beep --- beep --- beep)
    
    P1_init(); // ppm receiver interrupt
    P2_init(); // itg3200 sample interrupt
    
    eint(); // habilita interrupt
    
    delayms(400);
        
    // calibra esc
    if(PPMValue[RADIO_THROTTLE_CH] > STICK_LOWER_THRESHOLD){
        while(i--){
            set_all_motors(PPMValue[RADIO_THROTTLE_CH]);
            delayms(80);
        }       
    }
    else{
        set_all_motors(MIN_MOTOR);
    }
    
    analog_init(); // config registradores        
    analog_calibrate_channel(0);
    analog_calibrate_channel(1);
    analog_calibrate_channel(2);
    analog_calibrate_channel(3);
    analog_calibrate_channel(4);
    analog_calibrate_channel(5);
    analog_calibrate_channel(6);
    analog_calibrate_channel(7);
        
    calibrate_radio();
        
    if(load_params()){
        
    }
        
    menu_init();
    ENABLE_BUZZER();
    BUZZER_ON();
    
    lcd_init(LCDBackColor);
    
    BUZZER_OFF();
}

void menu_init(void){
    MainMenu = menu_create(str_mainmenu, create_item(str_analogmenu, ITEMTYPE_SUBMENU, NULL, NULL, NULL, NULL), &val_janela_size_large, NULL, NULL, &val_arrow_center, NULL);
    menu_add_item(MainMenu, create_item(str_radiomenu, ITEMTYPE_SUBMENU, NULL, NULL, NULL, NULL));
    menu_add_item(MainMenu, create_item(str_motormenu, ITEMTYPE_SUBMENU, NULL, NULL, NULL, NULL));
    menu_add_item(MainMenu, create_item(str_controlmenu, ITEMTYPE_SUBMENU, NULL, NULL, NULL, NULL));
    menu_add_item(MainMenu, create_item(str_sensormenu, ITEMTYPE_SUBMENU, NULL, NULL, NULL, NULL));
    menu_add_item(MainMenu, create_item(str_optionmenu, ITEMTYPE_SUBMENU, NULL, NULL, NULL, NULL));    
    menu_add_item(MainMenu, create_item(str_letsfly, ITEMTYPE_SUBMENU, NULL, NULL, NULL, NULL));
    // analog menu
    AnalogMenu = menu_create(str_analogmenu, create_item(str_return, ITEMTYPE_SUBMENU, NULL, NULL, NULL, NULL), &val_janela_size_large, &val_bar_position_center, &val_value_position_right, &val_arrow_center, &val_bar_lenght_medium);
    menu_add_item(AnalogMenu, create_item(str_calibrate, ITEMTYPE_SUBMENU, NULL, NULL, NULL, NULL));
    menu_add_item(AnalogMenu, create_item(str_analogch0, ITEMTYPE_VALUE_BAR_R, &val_zero, &val_max_analog, NULL, (int*)&AnalogValue[0]));
    menu_add_item(AnalogMenu, create_item(str_analogch1, ITEMTYPE_VALUE_BAR_R, &val_zero, &val_max_analog, NULL, (int*)&AnalogValue[1]));
    menu_add_item(AnalogMenu, create_item(str_analogch2, ITEMTYPE_VALUE_BAR_R, &val_zero, &val_max_analog, NULL, (int*)&AnalogValue[2]));
    menu_add_item(AnalogMenu, create_item(str_analogch3, ITEMTYPE_VALUE_BAR_R, &val_zero, &val_max_analog, NULL, (int*)&AnalogValue[3]));
    menu_add_item(AnalogMenu, create_item(str_analogch4, ITEMTYPE_VALUE_BAR_R, &val_zero, &val_max_analog, NULL, (int*)&AnalogValue[4]));
    menu_add_item(AnalogMenu, create_item(str_analogch5, ITEMTYPE_VALUE_BAR_R, &val_zero, &val_max_analog, NULL, (int*)&AnalogValue[5]));    
    menu_add_item(AnalogMenu, create_item(str_analogch6, ITEMTYPE_VALUE_BAR_R, &val_zero, &val_max_analog, NULL, (int*)&AnalogValue[6]));    
    menu_add_item(AnalogMenu, create_item(str_analogch7, ITEMTYPE_VALUE_BAR_R, &val_zero, &val_max_analog, NULL, (int*)&AnalogValue[7]));    
    
    menu_add_item(AnalogMenu, create_item(str_analogof0, ITEMTYPE_VALUE_R, &val_zero, &val_max_analog, NULL, (int*)&AnalogOffset[0]));
    menu_add_item(AnalogMenu, create_item(str_analogof1, ITEMTYPE_VALUE_R, &val_zero, &val_max_analog, NULL, (int*)&AnalogOffset[1]));
    menu_add_item(AnalogMenu, create_item(str_analogof2, ITEMTYPE_VALUE_R, &val_zero, &val_max_analog, NULL, (int*)&AnalogOffset[2]));
    menu_add_item(AnalogMenu, create_item(str_analogof3, ITEMTYPE_VALUE_R, &val_zero, &val_max_analog, NULL, (int*)&AnalogOffset[3]));
    menu_add_item(AnalogMenu, create_item(str_analogof4, ITEMTYPE_VALUE_R, &val_zero, &val_max_analog, NULL, (int*)&AnalogOffset[4]));
    menu_add_item(AnalogMenu, create_item(str_analogof5, ITEMTYPE_VALUE_R, &val_zero, &val_max_analog, NULL, (int*)&AnalogOffset[5]));    
    menu_add_item(AnalogMenu, create_item(str_analogof6, ITEMTYPE_VALUE_R, &val_zero, &val_max_analog, NULL, (int*)&AnalogOffset[6]));    
    menu_add_item(AnalogMenu, create_item(str_analogof7, ITEMTYPE_VALUE_R, &val_zero, &val_max_analog, NULL, (int*)&AnalogOffset[7]));    
    // control menu
    ControlMenu = menu_create(str_controlmenu, create_item(str_return, ITEMTYPE_SUBMENU, NULL, NULL, NULL, NULL), &val_janela_size_large, &val_bar_position_center, &val_value_position_right, &val_arrow_center, &val_bar_lenght_medium);
    menu_add_item(ControlMenu, create_item(str_yawpm, ITEMTYPE_VALUE_RW, &val_one, &val_max_control, &val_one, (int*)&ControlProportionalMul[YAW_INDEX]));
    menu_add_item(ControlMenu, create_item(str_pitchpm, ITEMTYPE_VALUE_RW, &val_one, &val_max_control, &val_one, (int*)&ControlProportionalMul[PITCH_INDEX]));
    menu_add_item(ControlMenu, create_item(str_rollpm, ITEMTYPE_VALUE_RW, &val_one, &val_max_control, &val_one, (int*)&ControlProportionalMul[ROLL_INDEX]));        
    menu_add_item(ControlMenu, create_item(str_yawpd, ITEMTYPE_VALUE_RW, &val_zero, &val_max_control, &val_one, (int*)&ControlProportionalDiv[YAW_INDEX]));
    menu_add_item(ControlMenu, create_item(str_pitchpd, ITEMTYPE_VALUE_RW, &val_zero, &val_max_control, &val_one, (int*)&ControlProportionalDiv[PITCH_INDEX]));
    menu_add_item(ControlMenu, create_item(str_rollpd, ITEMTYPE_VALUE_RW, &val_zero, &val_max_control, &val_one, (int*)&ControlProportionalDiv[ROLL_INDEX]));        
    
    menu_add_item(ControlMenu, create_item(str_yawrefdiv, ITEMTYPE_VALUE_RW, &val_zero, &val_max_control, &val_one, (int*)&ControlReferenceDiv[YAW_INDEX]));        
    menu_add_item(ControlMenu, create_item(str_pitchrefdiv, ITEMTYPE_VALUE_RW, &val_zero, &val_max_control, &val_one, (int*)&ControlReferenceDiv[PITCH_INDEX]));        
    menu_add_item(ControlMenu, create_item(str_rollrefdiv, ITEMTYPE_VALUE_RW, &val_zero, &val_max_control, &val_one, (int*)&ControlReferenceDiv[ROLL_INDEX]));        
    
    // radio menu
    RadioMenu = menu_create(str_radiomenu, create_item(str_return, ITEMTYPE_SUBMENU, NULL, NULL, NULL, NULL), &val_janela_size_large, &val_bar_position_center, &val_value_position_right, &val_arrow_center, &val_bar_lenght_medium);
    menu_add_item(RadioMenu, create_item(str_calibrate, ITEMTYPE_SUBMENU, NULL, NULL, NULL, NULL));            
        
    menu_add_item(RadioMenu, create_item(str_radioch0, ITEMTYPE_VALUE_BAR_R, &val_min_radio, &val_max_radio, NULL, (int*)&PPMValue[0]));
    menu_add_item(RadioMenu, create_item(str_radioch1, ITEMTYPE_VALUE_BAR_R, &val_min_radio, &val_max_radio, NULL, (int*)&PPMValue[1]));
    menu_add_item(RadioMenu, create_item(str_radioch2, ITEMTYPE_VALUE_BAR_R, &val_min_radio, &val_max_radio, NULL, (int*)&PPMValue[2]));
    menu_add_item(RadioMenu, create_item(str_radioch3, ITEMTYPE_VALUE_BAR_R, &val_min_radio, &val_max_radio, NULL, (int*)&PPMValue[3]));
    menu_add_item(RadioMenu, create_item(str_radioch4, ITEMTYPE_VALUE_BAR_R, &val_min_radio, &val_max_radio, NULL, (int*)&PPMValue[4]));
    menu_add_item(RadioMenu, create_item(str_radioch5, ITEMTYPE_VALUE_BAR_R, &val_min_radio, &val_max_radio, NULL, (int*)&PPMValue[5]));    
    menu_add_item(RadioMenu, create_item(str_radioch6, ITEMTYPE_VALUE_BAR_R, &val_min_radio, &val_max_radio, NULL, (int*)&PPMValue[6]));    
    menu_add_item(RadioMenu, create_item(str_radioch7, ITEMTYPE_VALUE_BAR_R, &val_min_radio, &val_max_radio, NULL, (int*)&PPMValue[7]));
    
    menu_add_item(RadioMenu, create_item(str_radioof0, ITEMTYPE_VALUE_R, &val_zero, &val_max_radio, NULL, (int*)&PPMOffset[0]));
    menu_add_item(RadioMenu, create_item(str_radioof1, ITEMTYPE_VALUE_R, &val_zero, &val_max_radio, NULL, (int*)&PPMOffset[1]));
     menu_add_item(RadioMenu, create_item(str_radioof2, ITEMTYPE_VALUE_R, &val_zero, &val_max_radio, NULL, (int*)&PPMOffset[2]));
    menu_add_item(RadioMenu, create_item(str_radioof3, ITEMTYPE_VALUE_R, &val_zero, &val_max_radio, NULL, (int*)&PPMOffset[3]));
    menu_add_item(RadioMenu, create_item(str_radioof4, ITEMTYPE_VALUE_R, &val_zero, &val_max_radio, NULL, (int*)&PPMOffset[4]));
    menu_add_item(RadioMenu, create_item(str_radioof5, ITEMTYPE_VALUE_R, &val_zero, &val_max_radio, NULL, (int*)&PPMOffset[5]));    
    menu_add_item(RadioMenu, create_item(str_radioof6, ITEMTYPE_VALUE_R, &val_zero, &val_max_radio, NULL, (int*)&PPMOffset[6]));    
    menu_add_item(RadioMenu, create_item(str_radioof7, ITEMTYPE_VALUE_R, &val_zero, &val_max_radio, NULL, (int*)&PPMOffset[7]));
    // motor menu    
    MotorMenu = menu_create(str_motormenu, create_item(str_return, ITEMTYPE_SUBMENU, NULL, NULL, NULL, NULL), &val_janela_size_large, &val_bar_position_center, &val_value_position_right, &val_arrow_center, &val_bar_lenght_medium);
    menu_add_item(MotorMenu, create_item(str_motor1, ITEMTYPE_VALUE_BAR_RW, &val_min_motor, &val_max_motor, &val_int_motor, (int*)&MotorOutput[0]));
    menu_add_item(MotorMenu, create_item(str_motor2, ITEMTYPE_VALUE_BAR_RW, &val_min_motor, &val_max_motor, &val_int_motor, (int*)&MotorOutput[1]));
    menu_add_item(MotorMenu, create_item(str_motor3, ITEMTYPE_VALUE_BAR_RW, &val_min_motor, &val_max_motor, &val_int_motor, (int*)&MotorOutput[2]));
    menu_add_item(MotorMenu, create_item(str_motor4, ITEMTYPE_VALUE_BAR_RW, &val_min_motor, &val_max_motor, &val_int_motor, (int*)&MotorOutput[3]));
    menu_add_item(MotorMenu, create_item(str_motor5, ITEMTYPE_VALUE_BAR_RW, &val_min_motor, &val_max_motor, &val_int_motor, (int*)&MotorOutput[4]));
    menu_add_item(MotorMenu, create_item(str_motor6, ITEMTYPE_VALUE_BAR_RW, &val_min_motor, &val_max_motor, &val_int_motor, (int*)&MotorOutput[5]));    
    menu_add_item(MotorMenu, create_item(str_calibrate, ITEMTYPE_SUBMENU, NULL, NULL, NULL, NULL));    
    // sensor menu
    SensorMenu = menu_create(str_sensormenu, create_item(str_return, ITEMTYPE_SUBMENU, NULL, NULL, NULL, NULL), &val_janela_size_large, &val_bar_position_center, &val_value_position_right, &val_arrow_center, &val_bar_lenght_medium);
    menu_add_item(SensorMenu, create_item(str_accelx, ITEMTYPE_VALUE_BAR_R, &val_zero, &val_max_analog, NULL, (int*)&AccelValue[ACCEL_X_INDEX]));
    menu_add_item(SensorMenu, create_item(str_accely, ITEMTYPE_VALUE_BAR_R, &val_zero, &val_max_analog, NULL, (int*)&AccelValue[ACCEL_Y_INDEX]));
    menu_add_item(SensorMenu, create_item(str_accelz, ITEMTYPE_VALUE_BAR_R, &val_zero, &val_max_analog, NULL, (int*)&AccelValue[ACCEL_Z_INDEX]));
    menu_add_item(SensorMenu, create_item(str_gyrox, ITEMTYPE_VALUE_BAR_R, &val_zero, &val_max_analog, NULL, (int*)&GyroValue[GYRO_X_INDEX]));
    menu_add_item(SensorMenu, create_item(str_gyroy, ITEMTYPE_VALUE_BAR_R, &val_zero, &val_max_analog, NULL, (int*)&GyroValue[GYRO_Y_INDEX]));
    menu_add_item(SensorMenu, create_item(str_gyroz, ITEMTYPE_VALUE_BAR_R, &val_zero, &val_max_analog, NULL, (int*)&GyroValue[GYRO_Z_INDEX]));    
    menu_add_item(SensorMenu, create_item(str_graph, ITEMTYPE_SUBMENU, NULL, NULL, NULL, NULL));    
    // option menu
    OptionMenu = menu_create(str_optionmenu, create_item(str_return, ITEMTYPE_SUBMENU, NULL, NULL, NULL, NULL), &val_janela_size_large, &val_bar_position_center, &val_value_position_right, &val_arrow_center, &val_bar_lenght_medium);
    menu_add_item(OptionMenu, create_item(str_grava, ITEMTYPE_SUBMENU, NULL, NULL, NULL, NULL));    
    menu_add_item(OptionMenu, create_item(str_rstdefault, ITEMTYPE_SUBMENU, NULL, NULL, NULL, NULL));    
}

ACTION get_radio_action(void){
    // highest priority
    if(PPMValue[5] > STICK_UPPER_THRESHOLD){
        return ACTION_EMERGENCY;
    }
        
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

void set_all_motors(unsigned int val){
    MotorOutput[0] = val;
    MotorOutput[1] = val;
    MotorOutput[2] = val;
    MotorOutput[3] = val;
    MotorOutput[4] = val;
    MotorOutput[5] = val;
    
    set_motor_output();
}

void screen_flash(int Color, int interval, int times){
    // iluminacao progressiva do LCD
    while(times--){    
        while(TACCR1 > 0){
            TACCR1 = TACCR1 - 1;
            delayus(interval);
        }
        if(Color != 256){
            lcd_clear(Color);
        }
        while(TACCR1 < LCD_MAX_BRIGHT){
            TACCR1 = TACCR1 + 1;
            delayus(interval);
        }
    }
    while(TACCR1 < LCD_MAX_BRIGHT){
        TACCR1 = TACCR1 + 1;
        delayus(interval);
    }
}

void calibrate_radio(void){
    unsigned i = 8, j = 8;
    do{
        i--;
        PPMOffset[i] = 0;
    }while(i);
    i=8;
    do{
        i--;
        do{
            j--;
            PPMOffset[j] += PPMValue[j];
        }while(j);
        j=8;
        delayms(50);
    }while(i);
    i=8;
    do{
        i--;
        PPMOffset[i] = PPMOffset[i] >> 3;
    }while(i);
}

#ifdef TEST_LOOP_PERIOD
unsigned int tempoloop = 0;
unsigned int tempoloop2 = 0;
#endif // TEST_LOOP_PERIOD
/*
void control_loop(void){
    if(PPMValue[RADIO_THROTTLE_CH] > STICK_LOWER_THRESHOLD){
        // derivativo
        ControlOldSample[ROLL_INDEX] = GyroValue[ROLL_INDEX] - ControlOldSample[ROLL_INDEX];
        ControlOldSample[YAW_INDEX] = GyroValue[YAW_INDEX] - ControlOldSample[YAW_INDEX];
        ControlOldSample[PITCH_INDEX] = GyroValue[PITCH_INDEX] - ControlOldSample[PITCH_INDEX];
        
        ThrottleFiltered = (ThrottleFiltered * 7 + PPMValue[RADIO_THROTTLE_CH]) >> 3;
        
        MotorOutput[MOTOR_RIGHT] = ThrottleFiltered;
        MotorOutput[MOTOR_RIGHT] -= ((PPMValue[RADIO_ROLL_CH] - PPMOffset[RADIO_ROLL_CH]) * 5) >> 4; 
        MotorOutput[MOTOR_RIGHT] += ((GyroValue[ROLL_INDEX] - AnalogOffset[GYRO_ROLL_ACH]) * 5) >> 2;
        
        MotorOutput[MOTOR_LEFT] = ThrottleFiltered;
        MotorOutput[MOTOR_LEFT] += ((PPMValue[RADIO_ROLL_CH] - PPMOffset[RADIO_ROLL_CH]) * 5) >> 4;
        MotorOutput[MOTOR_LEFT] -= ((GyroValue[ROLL_INDEX] - AnalogOffset[GYRO_ROLL_ACH]) * 5) >> 2;
            
        MotorOutput[MOTOR_FRONT] = ThrottleFiltered;
        MotorOutput[MOTOR_FRONT] += ((PPMValue[RADIO_PITCH_CH] - PPMOffset[RADIO_PITCH_CH]) * 5) >> 4;
        MotorOutput[MOTOR_FRONT] += ((GyroValue[PITCH_INDEX] - AnalogOffset[GYRO_PITCH_ACH]) * 5) >> 2;
        
        MotorOutput[MOTOR_BACK] = ThrottleFiltered;
        MotorOutput[MOTOR_BACK] -= ((PPMValue[RADIO_PITCH_CH] - PPMOffset[RADIO_PITCH_CH]) * 5) >> 4;
        MotorOutput[MOTOR_BACK] -= ((GyroValue[PITCH_INDEX] - AnalogOffset[GYRO_PITCH_ACH]) * 5) >> 2;
                
        MotorOutput[MOTOR_RIGHT] -= (PPMValue[RADIO_YAW_CH] - PPMOffset[RADIO_YAW_CH]);
        MotorOutput[MOTOR_LEFT] -= (PPMValue[RADIO_YAW_CH] - PPMOffset[RADIO_YAW_CH]);
        MotorOutput[MOTOR_RIGHT] -= ((GyroValue[YAW_INDEX] - AnalogOffset[GYRO_YAW_ACH]) * 3) >> 1;
        MotorOutput[MOTOR_LEFT] -= ((GyroValue[YAW_INDEX] - AnalogOffset[GYRO_YAW_ACH]) * 3) >> 1;

        MotorOutput[MOTOR_FRONT] += (PPMValue[RADIO_YAW_CH] - PPMOffset[RADIO_YAW_CH]);
        MotorOutput[MOTOR_BACK] += (PPMValue[RADIO_YAW_CH] - PPMOffset[RADIO_YAW_CH]);
        MotorOutput[MOTOR_FRONT] += ((GyroValue[YAW_INDEX] - AnalogOffset[GYRO_YAW_ACH]) * 3) >> 1;
        MotorOutput[MOTOR_BACK] += ((GyroValue[YAW_INDEX] - AnalogOffset[GYRO_YAW_ACH]) * 3) >> 1;

        // derivativo
#ifdef DERIVATIVO        
        MotorOutput[MOTOR_RIGHT] -= (ControlOldSample[ROLL_INDEX]) * 1;
        MotorOutput[MOTOR_LEFT] += (ControlOldSample[ROLL_INDEX]) * 1;
        
        MotorOutput[MOTOR_FRONT] -= (ControlOldSample[PITCH_INDEX]) * 1;
        MotorOutput[MOTOR_BACK] += (ControlOldSample[PITCH_INDEX]) * 1;
        
        MotorOutput[MOTOR_RIGHT] += (ControlOldSample[YAW_INDEX]) * 1;
        MotorOutput[MOTOR_LEFT] += (ControlOldSample[YAW_INDEX]) * 1;
        MotorOutput[MOTOR_FRONT] -= (ControlOldSample[YAW_INDEX]) * 1;
        MotorOutput[MOTOR_BACK] -= (ControlOldSample[YAW_INDEX]) * 1;
#endif
        set_motor_output();
        
        ControlOldSample[YAW_INDEX] = GyroValue[YAW_INDEX];
        ControlOldSample[PITCH_INDEX] = GyroValue[PITCH_INDEX];
        ControlOldSample[ROLL_INDEX] = GyroValue[ROLL_INDEX];
        
        // filter gyro
        
        
        // maximo de 9h de voo podem ser registradas antes de dar overflow
        if(get_delay(DELAY_SECONDS_INDEX)){
            set_delay(DELAY_SECONDS_INDEX, 500);
            FlightTime++;
        }

        // testa o tempo do loop        
#ifdef TEST_LOOP_PERIOD
        if(tempoloop == 0){
            set_delay(14,10);
            tempoloop = TAR;
        }
        else{
            tempoloop2 = TAR;
            lcd_clear(WHITE);
            lcd_goto(5,5);
            printf("%d\n%d\n%d", tempoloop, tempoloop2, TimeDelay[14]);
            while(1);
        }
#endif // TEST_LOOP_PERIOD
    }
    else{
        set_all_motors(MIN_MOTOR);
    }
}
*/
void control_loop(void){
    if(PPMValue[RADIO_THROTTLE_CH] > STICK_LOWER_THRESHOLD){
        
        ThrottleFiltered = (ThrottleFiltered * 7 + PPMValue[RADIO_THROTTLE_CH]) >> 3;
        
        // lowpass no gyro        
        GyroValue[ROLL_INDEX] = (GyroValue[ROLL_INDEX] * 3 + AnalogValue[GYRO_ROLL_ACH]) >> 2;
        GyroValue[PITCH_INDEX] = (GyroValue[PITCH_INDEX] * 3 + AnalogValue[GYRO_PITCH_ACH]) >> 2;
        GyroValue[YAW_INDEX] = (GyroValue[YAW_INDEX] * 3 + AnalogValue[GYRO_YAW_ACH]) >> 2;
        
        MotorOutput[MOTOR_RIGHT] = ThrottleFiltered;
        MotorOutput[MOTOR_RIGHT] -= ((PPMValue[RADIO_ROLL_CH] - PPMOffset[RADIO_ROLL_CH]) * 5) >> 4; 
        MotorOutput[MOTOR_RIGHT] += ((GyroValue[ROLL_INDEX] - AnalogOffset[GYRO_ROLL_ACH]) * 5) >> 2;
        
        MotorOutput[MOTOR_LEFT] = ThrottleFiltered;
        MotorOutput[MOTOR_LEFT] += ((PPMValue[RADIO_ROLL_CH] - PPMOffset[RADIO_ROLL_CH]) * 5) >> 4;
        MotorOutput[MOTOR_LEFT] -= ((GyroValue[ROLL_INDEX] - AnalogOffset[GYRO_ROLL_ACH]) * 5) >> 2;
            
        MotorOutput[MOTOR_FRONT] = ThrottleFiltered;
        MotorOutput[MOTOR_FRONT] += ((PPMValue[RADIO_PITCH_CH] - PPMOffset[RADIO_PITCH_CH]) * 5) >> 4;
        MotorOutput[MOTOR_FRONT] += ((GyroValue[PITCH_INDEX] - AnalogOffset[GYRO_PITCH_ACH]) * 5) >> 2;
        
        MotorOutput[MOTOR_BACK] = ThrottleFiltered;
        MotorOutput[MOTOR_BACK] -= ((PPMValue[RADIO_PITCH_CH] - PPMOffset[RADIO_PITCH_CH]) * 5) >> 4;
        MotorOutput[MOTOR_BACK] -= ((GyroValue[PITCH_INDEX] - AnalogOffset[GYRO_PITCH_ACH]) * 5) >> 2;
                
        MotorOutput[MOTOR_RIGHT] -= (PPMValue[RADIO_YAW_CH] - PPMOffset[RADIO_YAW_CH]);
        MotorOutput[MOTOR_LEFT] -= (PPMValue[RADIO_YAW_CH] - PPMOffset[RADIO_YAW_CH]);
        MotorOutput[MOTOR_RIGHT] -= ((GyroValue[YAW_INDEX] - AnalogOffset[GYRO_YAW_ACH]) * 3) >> 1;
        MotorOutput[MOTOR_LEFT] -= ((GyroValue[YAW_INDEX] - AnalogOffset[GYRO_YAW_ACH]) * 3) >> 1;

        MotorOutput[MOTOR_FRONT] += (PPMValue[RADIO_YAW_CH] - PPMOffset[RADIO_YAW_CH]);
        MotorOutput[MOTOR_BACK] += (PPMValue[RADIO_YAW_CH] - PPMOffset[RADIO_YAW_CH]);
        MotorOutput[MOTOR_FRONT] += ((GyroValue[YAW_INDEX] - AnalogOffset[GYRO_YAW_ACH]) * 3) >> 1;
        MotorOutput[MOTOR_BACK] += ((GyroValue[YAW_INDEX] - AnalogOffset[GYRO_YAW_ACH]) * 3) >> 1;

        MotorOutput[MOTOR_FRONT] = (MotorOutput[MOTOR_FRONT] + MotorOutputOld[MOTOR_FRONT]) >> 1;
        MotorOutput[MOTOR_LEFT] =  (MotorOutput[MOTOR_LEFT] + MotorOutputOld[MOTOR_LEFT]) >> 1;
        MotorOutput[MOTOR_BACK] = (MotorOutput[MOTOR_BACK] + MotorOutputOld[MOTOR_BACK]) >> 1;
        MotorOutput[MOTOR_RIGHT] = (MotorOutput[MOTOR_RIGHT] + MotorOutputOld[MOTOR_RIGHT]) >> 1; 
        
        set_motor_output();
        
        MotorOutputOld[MOTOR_FRONT] = MotorOutput[MOTOR_FRONT];
        MotorOutputOld[MOTOR_LEFT] = MotorOutput[MOTOR_LEFT];
        MotorOutputOld[MOTOR_BACK] = MotorOutput[MOTOR_BACK];
        MotorOutputOld[MOTOR_RIGHT] = MotorOutput[MOTOR_RIGHT];
        
    }
    else{
        set_all_motors(MIN_MOTOR);
    }
}


// result 1 = fail
// result 0 = success
unsigned char save_params(void){
    unsigned char result = 0;
    twobytes VarAux;
    
    i2c_config(EEPROM_I2C_ADDR);
    result = i2c_find_device(EEPROM_I2C_ADDR);
    if(!result){
        
        VarAux.i = 0xAA;
        result |= i2c_write_one_buffer(ADDR_VALIDATION, VarAux.uc, 2);
        delayms(100);
        
        VarAux.ui = FlightTime;
        result |= i2c_write_one_buffer(ADDR_FLIGHT_TIME, VarAux.uc, 2);
        delayms(100);
        
        VarAux.i = ControlProportionalMul[PITCH_INDEX];
        result |= i2c_write_one_buffer(ADDR_P_PITCH_MUL, VarAux.uc, 2);
        delayms(100);
        
        VarAux.i = ControlProportionalMul[ROLL_INDEX];
        result |= i2c_write_one_buffer(ADDR_P_ROLL_MUL, VarAux.uc, 2);
        delayms(100);
        
        VarAux.i = ControlProportionalMul[YAW_INDEX];
        result |= i2c_write_one_buffer(ADDR_P_YAW_MUL, VarAux.uc, 2);
        delayms(100);

        VarAux.i = ControlProportionalDiv[PITCH_INDEX];
        result |= i2c_write_one_buffer(ADDR_P_PITCH_DIV, VarAux.uc, 2);
        delayms(100);

        VarAux.i = ControlProportionalDiv[ROLL_INDEX];
        result |= i2c_write_one_buffer(ADDR_P_ROLL_DIV, VarAux.uc, 2);
        delayms(100);

        VarAux.i = ControlProportionalDiv[YAW_INDEX];
        result |= i2c_write_one_buffer(ADDR_P_YAW_DIV, VarAux.uc, 2);
        delayms(100);
        
        screen_flash(256, 200, 2);
    }
    
    return result;
}

unsigned char load_params(void){
    unsigned char result = 0;
    twobytes VarAux;
    
    i2c_config(EEPROM_I2C_ADDR);
    result = i2c_find_device(EEPROM_I2C_ADDR);
    if(!result){
        i2c_wread_one_buffer(ADDR_VALIDATION, VarAux.uc, 2);
        if(VarAux.i == 0xAA){
            delayms(50);
            result |= i2c_wread_one_buffer(ADDR_FLIGHT_TIME, VarAux.uc, 2);
            FlightTime = VarAux.i;
            
            delayms(50);
            result |= i2c_wread_one_buffer(ADDR_P_PITCH_MUL, VarAux.uc, 2);
            ControlProportionalMul[PITCH_INDEX] = VarAux.i;
            
            delayms(50);
            result |= i2c_wread_one_buffer(ADDR_P_ROLL_MUL, VarAux.uc, 2);
            ControlProportionalMul[ROLL_INDEX] = VarAux.i;
                        
            delayms(50);
            result |= i2c_wread_one_buffer(ADDR_P_YAW_MUL, VarAux.uc, 2);
            ControlProportionalMul[YAW_INDEX] = VarAux.i;
            
            delayms(50);
            result |= i2c_wread_one_buffer(ADDR_P_PITCH_DIV, VarAux.uc, 2);
            ControlProportionalDiv[PITCH_INDEX] = VarAux.i;
            
            delayms(50);
            result |= i2c_wread_one_buffer(ADDR_P_ROLL_DIV, VarAux.uc, 2);
            ControlProportionalDiv[ROLL_INDEX] = VarAux.i;
            
            delayms(50);
            result |= i2c_wread_one_buffer(ADDR_P_YAW_DIV, VarAux.uc, 2);
            ControlProportionalDiv[YAW_INDEX] = VarAux.i;
            
            screen_flash(256, 200, 5);
        }    
        else{
            result |= reset_defaults();
        }    
    }
    
    return result;
    
}
    
unsigned char reset_defaults(void){
    FlightTime = 0;
    ControlProportionalMul[YAW_INDEX] = YAW_PROPORTIONAL_MUL;
    ControlProportionalMul[PITCH_INDEX] = PITCH_PROPORTIONAL_MUL;
    ControlProportionalMul[ROLL_INDEX] = ROLL_PROPORTIONAL_MUL;    
    ControlProportionalDiv[YAW_INDEX] = YAW_PROPORTIONAL_DIV;
    ControlProportionalDiv[PITCH_INDEX] = PITCH_PROPORTIONAL_DIV;
    ControlProportionalDiv[ROLL_INDEX] = ROLL_PROPORTIONAL_DIV;
    ControlReferenceDiv[YAW_INDEX] = YAW_REF_DIV;
    ControlReferenceDiv[PITCH_INDEX] = PITCH_REF_DIV;
    ControlReferenceDiv[ROLL_INDEX] = ROLL_REF_DIV;
    
    return (save_params());
    
}

void adjust_readings(void){
    GyroValue[YAW_INDEX] = (GyroValue[YAW_INDEX] * 3 + AnalogValue[GYRO_YAW_ACH]) >> 2;
    GyroValue[ROLL_INDEX] = (GyroValue[ROLL_INDEX] * 3 + AnalogValue[GYRO_ROLL_ACH]) >> 2;
    GyroValue[PITCH_INDEX] = (GyroValue[PITCH_INDEX] * 3 + AnalogValue[GYRO_PITCH_ACH]) >> 2;
}

// delay
interrupt (TIMERA1_VECTOR) TIMERA1_ISR_HOOK(void){ // 2ms
    int i = 10;
 
    do{
        i--;
        if(TimeDelay[i] > 0){
            TimeDelay[i]--;
        }
    }while(i);
    // ver no datasheet se tem q zerar o flag da interrupt
    TACTL &= ~0x01;
}

interrupt (TIMERB1_VECTOR) TIMERB1_ISR_HOOK(void){ // 2.5ms
    ControlSample = 1;
    TBCTL &= ~0x01;
}

