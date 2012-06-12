/*

- fazer verificador de calibracao
- fazer grafico para o Vdrop

POSSIVEIS TCCs
--- CONTROLE
incluir acelerometro no loop de controle
controle de posicao(x,y,z) com gps e magnetometro
controle de altitude com barometro e yaw com magnet
controle de posicao indoor, com sonar e infrared
colocar uma camera mirando pro chao para controle de posicao+

--- SOFT
usar CI de wifi para que o quad seja um servidor wifi
fazer interface usb com android pra passar waypoints, coletar logs, config, etc
depois que o quad for um servidor de wifi, fazer um controle remoto no android para controlar
fazer um fpv com osd 

fazer o analog por DMA
gravar e ler da flash
gravar parametros
carregar parametros
fazer rotinas I2C com timeout

****************************************************************************
****************************************************************************
****************************************************************************

SOBRE POSICIONAMENTO DOS SENSORES

GYRO
pitch aumenta qd quad vai pra frente
pitch diminui qd quad vai pra traz

roll diminui qd o quad vai para a esquerda
roll aumenta qd o quad vai para a direita

yaw diminui horario
yaw aumenta anti-horario

ACCEL
qd quad inclina em pitch pra frente ACH[3] aumenta

qd quad inclina em roll pra esquerda ACH[4] aumenta

*/

#include "stdio.h"          // printf, sprintf
#include "stdlib.h"         
#include "string.h"         // malloc, free
#include "legacymsp430.h"   // usado para interruption
#include "msp430f2618.h"    
#include "color.h"          // cores utilizadas no LCD
#include "delay.h"          // time delay, delay por polling
#include "analog.h"         // abstracao analog
#include "lcd6100.h"        // escrita e desenho no lcd
#include "eeprom.h"         // not used
#include "i2c.h"            // not used
#include "usefullibs.h"     // list, queue e stack
#include "menu.h"           // menu manager
#include "microquad.h"      // todos defines de controle, arrays, constants, etc
#include "resources.h"      // todas strings, limits (menu)
#include "shorttypes.h"     // uint16, uchar, etc
#include "radiorec.h"       // leitura dos canais PPM input do radio

//#define DEBUG_MODE

//variaveis controle
volatile uchar ControlSample = 0;       // sample time flag - indica a hora de fazer um sample e um controle

// variaveis controle atuadores
uint16 MotorOutput[6] = {MIN_MOTOR, MIN_MOTOR, MIN_MOTOR, MIN_MOTOR, MIN_MOTOR, MIN_MOTOR};

long ThrottleFiltered = 0;
int16 PitchRollPSaturation = 0;

// variaveis controle realimentacao
int16 AccelValue[3] = {0,0,0};
int16 AccelOffset[3] = {0,0,0};
int16 GyroValue[3] = {0,0,0};
int16 GyroOffset[3] = {0,0,0};
int16 Angle[3] = {0,0,0};
long GyroSum[3] = {0,0,0};

// variaveis controle ganhos
int16 ControlProportionalMul[3];
int16 ControlProportionalDiv[3];
int16 ControlIntegralLimit[3];
int16 ControlIntegralDiv[3];

// variaveis dos filtros
int16 ThrottleLPMul;    
int16 ThrottleLPDiv;    
int16 MotorOutputLPMul; 
int16 MotorOutputLPDiv; 
int16 GyroLPMul;     
int16 GyroLPDiv;     
int16 AccelLPMul;        
int16 AccelLPDiv;        

// general purpose
long FlightTime = 0;

// analog graph
byte AnalogChecked;

unsigned char AnalogColours[8] = {BLUE, LIME, RED, YELLOW, CYAN, MAGENTA, SILVER, WHITE}; 
unsigned char AnalogGraph[8][GRAPH_LENGHT];   

// menu
MENU* MainMenu;
MENU* AnalogMenu;
MENU* MotorMenu;
MENU* ControlMenu;
MENU* FilterMenu;
MENU* RadioMenu;
MENU* SensorMenu;
MENU* OptionMenu;
MENU* VibrationAnalyzerMenu;
MENU* VoltageDropMenu;
MENU* ThrottleCalibrationMenu;

// variaveis do menu voltage drop
// 1790 = 8.36V entao a razao eh /21
unsigned int VdropMinBat = 0;
unsigned int VdropMaxBat = 0;
unsigned int Vbat = 0;

FLIGHT_MODE FlightMode = FLYMODE_ACRO;

int main(void){

    static PROGRAM_STEP ProgStep = PROCESS_MAIN_MENU;
                    
    WDTCTL = WDTPW + WDTHOLD;   // desabilita watchdog
    
    uchar MenuDraw = 0;
    uint16 BatteryColor = LIME;
    uint16 GPCounter = 0;

    ACTION act = ACTION_NONE;
    MENU_RESPONSE resp = RESP_NONE;

    setup();
          
    menu_draw(MainMenu, 1);
        
    while(1){

        // deadzone para RC input        
        for(GPCounter = 0; GPCounter < 8; GPCounter++){
            if(((PPMRaw[GPCounter] - PPMOffset[GPCounter]) < 30) && ((PPMRaw[GPCounter] - PPMOffset[GPCounter]) > -30)){
                PPMRaw[GPCounter] = PPMOffset[GPCounter];
            }
            PPMValue[GPCounter] = PPMRaw[GPCounter];
        }
                
        analog_refresh_all();
        adjust_readings();
                 
        switch(ProgStep){
            case PROCESS_MAIN_MENU:
                menu_draw(MainMenu, MenuDraw);
                if(get_delay(DELAY_SECONDS_INDEX)){
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
                    set_delay(DELAY_BATTERY_CHECK, BATTERY_CHECK_TIME);
                    
                    //print flighttime
                    lcd_goto(11,0);
                    printf("%lds", FlightTime);
                }

                MenuDraw = 0;                
                act = get_radio_action();
                resp = menu_process(MainMenu, act);
                
                switch(act){
                    case ACTION_TURN_C:
                    // change to acro mode
                    // pisca tela 2x amarelo
                       screen_flash(YELLOW, 200, 2);                    
                       MenuDraw = 1;                
                       FlightMode = FLYMODE_ACRO;
                    break;
                    
                    case ACTION_TURN_CW:
                    // change to stable mode
                    // pisca tela 2x azul
                        screen_flash(BLUE, 200, 2);
                        MenuDraw = 1;

                        Angle[PITCH_INDEX] = 0;
                        Angle[ROLL_INDEX] = 0;

                        FlightMode = FLYMODE_STABLE;
                    break;
                    
                    default:
                    // do nothing                    
                    break;
                }
                
                switch(resp){                    
                    case RESP_SUBMENU_IN:
                        if(PPMValue[RADIO_THROTTLE_CH] < STICK_LOWER_THRESHOLD){    
                            ProgStep = (PROGRAM_STEP)(MainMenu->SelectedItem);
                        }
                        else{
                            screen_flash(LIME, 200, 2);
                        }
                        MenuDraw = 1;
                    break;
                    
                    default:
                    break;
                }
            break;
                       
            case PROCESS_ANALOG_MENU:
                menu_draw(AnalogMenu, MenuDraw);

                if(MenuDraw == 1){                  
                    
                    lcd_fillrect(10,GRAPH_START-3,107,GRAPH_HEIGHT+6, BLACK);
                    lcd_fillrect(30,GRAPH_START,107-42,GRAPH_HEIGHT,GRAY);
                    
                    LCDBackColor = BLACK;
                    
                    for(GPCounter = 0; GPCounter < 4; GPCounter++){
                        lcd_goto(2,9+GPCounter);
                        LCDForeColor = AnalogColours[GPCounter];
                        printf("%d", GPCounter);
                    }
                    for(GPCounter = 4; GPCounter < 8; GPCounter++){
                        lcd_goto(16,5 + GPCounter);
                        LCDForeColor = AnalogColours[GPCounter];
                        printf("%d", GPCounter);
                    }
                    
                    LCDBackColor = LCD_DEFAULT_BACKCOLOR;
                    LCDForeColor = LCD_DEFAULT_FORECOLOR;
                }
                process_analog_graph();
                MenuDraw = 0;
                                
                menu_refresh(AnalogMenu);
                act = get_radio_action();
                resp = menu_process(AnalogMenu, act);
                switch(resp){
                    case RESP_SUBMENU_OUT:
                        switch(AnalogMenu->SelectedItem){
                            case RETURN_INDEX:
                                ProgStep = PROCESS_MAIN_MENU;
                                MenuDraw = 1;
                            break;
                            
                            case ANALOG_CH0_INDEX:
                                AnalogChecked.BITS.bit0 = 0;
                                analog_graph_clear(0);
                            break;
                            
                            case ANALOG_CH1_INDEX:
                                AnalogChecked.BITS.bit1 = 0;
                                analog_graph_clear(1);
                            break;
                            
                            case ANALOG_CH2_INDEX:
                                AnalogChecked.BITS.bit2 = 0;
                                analog_graph_clear(2);
                            break;
                            
                            case ANALOG_CH3_INDEX:
                                AnalogChecked.BITS.bit3 = 0;
                                analog_graph_clear(3);
                            break;
                            
                            case ANALOG_CH4_INDEX:
                                AnalogChecked.BITS.bit4 = 0;
                                analog_graph_clear(4);
                            break;
                            
                            case ANALOG_CH5_INDEX:
                                AnalogChecked.BITS.bit5 = 0;
                                analog_graph_clear(5);
                            break;
                            
                            case ANALOG_CH6_INDEX:
                                AnalogChecked.BITS.bit6 = 0;
                                analog_graph_clear(6);
                            break;
                            
                            case ANALOG_CH7_INDEX:
                                AnalogChecked.BITS.bit7 = 0;
                                analog_graph_clear(7);
                            break;
                        }
                    break;
                    
                    case RESP_SUBMENU_IN:
                        switch(AnalogMenu->SelectedItem){
                            case CALIBR_INDEX:
                                analog_calibrate_channel(0);
                                analog_calibrate_channel(1);
                                analog_calibrate_channel(2);
                                analog_calibrate_channel(3);
                                analog_calibrate_channel(4);
                                analog_calibrate_channel(5);
                                analog_calibrate_channel(6);
                                analog_calibrate_channel(7);
                            break;
                            
                            case ANALOG_CH0_INDEX:
                                AnalogChecked.BITS.bit0 = 1;
                            break;
                            
                            case ANALOG_CH1_INDEX:
                                AnalogChecked.BITS.bit1 = 1;
                            break;
                            
                            case ANALOG_CH2_INDEX:
                                AnalogChecked.BITS.bit2 = 1;
                            break;
                            
                            case ANALOG_CH3_INDEX:
                                AnalogChecked.BITS.bit3 = 1;
                            break;
                            
                            case ANALOG_CH4_INDEX:
                                AnalogChecked.BITS.bit4 = 1;
                            break;
                            
                            case ANALOG_CH5_INDEX:
                                AnalogChecked.BITS.bit5 = 1;
                            break;
                            
                            case ANALOG_CH6_INDEX:
                                AnalogChecked.BITS.bit6 = 1;
                            break;
                            
                            case ANALOG_CH7_INDEX:
                                AnalogChecked.BITS.bit7 = 1;
                            break;
                        }
                   
                    break;                    
                    
                    default:
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
                    break;
                    
                    case RESP_EMERGENCY:
                        set_all_motors(MIN_MOTOR);
                        lcd_init(LCDBackColor);
                        screen_flash(RED, 200, 2);
                        ProgStep = PROCESS_MAIN_MENU;
                        MenuDraw = 1;
                    break;
                        
                    default:
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
                    
                    default:
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
                        switch(RadioMenu->SelectedItem){
                            case CALIBR_INDEX:
                                calibrate_radio();
                                break;
                        }                    
                    break;                    
                    
                    default:
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
                    
                    default:
                    break;
                }
            break;

            case PROCESS_FILTER_MENU:
                menu_draw(FilterMenu, MenuDraw);
                MenuDraw = 0;
                menu_refresh(FilterMenu);
                act = get_radio_action();
                resp = menu_process(FilterMenu, act);
                switch(resp){
                    case RESP_SUBMENU_OUT:
                        if(FilterMenu->SelectedItem == RETURN_INDEX){
                            ProgStep = PROCESS_MAIN_MENU;
                            MenuDraw = 1;
                        }
                    break;
                    
                    default:
                    break;
                }
            break;

            
            case PROCESS_VIBRATION_ANALYZER_MENU:

                menu_draw(VibrationAnalyzerMenu, MenuDraw);              
                MenuDraw = 0;
                
                act = get_radio_action();
                resp = menu_process(VibrationAnalyzerMenu, act);
                
                if(ControlSample == 1){
                    ControlSample = 0;
                    process_vibration_analyzer_menu();
                }
                
                switch(resp){                   
                    case RESP_EMERGENCY:
                        set_all_motors(MIN_MOTOR);
                        lcd_init(LCDBackColor);
                        screen_flash(RED, 200, 2);
                        ProgStep = PROCESS_MAIN_MENU;
                        MenuDraw = 1;
                    break;
                    
                    default:
                    break;
                }
            break;
                  
            case PROCESS_VOLTAGE_DROP_MENU:
                menu_draw(VoltageDropMenu, MenuDraw);
                if(MenuDraw == 1){
                    // condicao inicial
                    VdropMinBat = AnalogValue[BATTERY_ACH] / 21;
                    VdropMaxBat = AnalogValue[BATTERY_ACH] / 21;
                }
                MenuDraw = 0;
                menu_refresh(VoltageDropMenu);
                
                act = get_radio_action();
                resp = menu_process(VoltageDropMenu, act);
                
                set_all_motors(PPMValue[RADIO_THROTTLE_CH]);
                Vbat = AnalogValue[BATTERY_ACH] / 21;
                // em regime
                // ajusta minimo valor
                if(Vbat < VdropMinBat){
                    VdropMinBat = Vbat;
                }
                else{
                    // ajusta maximo valor
                    if(Vbat > VdropMaxBat){
                        VdropMaxBat = Vbat;
                    }
                }
                switch(resp){
                    case RESP_SUBMENU_OUT:
                        if(VoltageDropMenu->SelectedItem == RETURN_INDEX){
                            ProgStep = PROCESS_MAIN_MENU;
                            MenuDraw = 1;
                        }
                    break;
                    
                    case RESP_EMERGENCY:
                        set_all_motors(MIN_MOTOR);
                        lcd_init(LCDBackColor);
                        screen_flash(RED, 200, 2);
                        ProgStep = PROCESS_MAIN_MENU;
                        MenuDraw = 1;
                    break;
                    
                    default:
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
                                    printf(str_failled);
                                    delayms(2000);
                                }
                            break;
                            
                            case RESET_INDEX:

                            break;
                        }
                        
                    break;
                    
                    default:
                    break;
                }
            break;
            
            case PROCESS_CONTROL:
            
#ifndef DEBUG_MODE 
                // modo normal
                if(TACCR1 > 0){ //apaga tela
                    TACCR1 = TACCR1 - 10;
                    delayus(10000);
                }
                else{
                    if(ControlSample == 1){
                        ControlSample = 0;
                    
                        switch(FlightMode){
                            case FLYMODE_ACRO:
                                control_loop_acro();
                            break;
                            
                            case FLYMODE_STABLE:    
                                control_loop_stable();
                            break;
                            
                            default:
                                screen_flash(RED,500,2);
                            break;
                        }
                    }
                }
#else // DEBUG_MODE
             // modo de ver variaveis no display
                if(ControlSample == 1){
                    ControlSample = 0;

                    switch(FlightMode){
                        case FLYMODE_ACRO:
                            control_loop_acro();
                        break;
                        
                        case FLYMODE_STABLE:    
                            control_loop_stable();
                        break;
                        
                        default:
                            screen_flash(RED,500,2);
                        break;
                    }
                        
                    lcd_goto(0,0);
                    //printf("\n\n%d   \n%d    \n%d   ", ControlIntegralResult[ROLL_INDEX], ControlIntegralResult[PITCH_INDEX], ControlIntegralResult[YAW_INDEX]);
                    printf("\n\n%d   \n%d     ", Angle[ROLL_INDEX], Angle[PITCH_INDEX]);
                }
#endif // DEBUG_MODE
                
                act = get_radio_action();
                switch(act){
                    case ACTION_EMERGENCY:
                        set_all_motors(MIN_MOTOR);
                        lcd_init(LCDBackColor);
                        screen_flash(BLACK,500,2);
                        ProgStep = PROCESS_MAIN_MENU;
                        MenuDraw = 1;
                    break;
                    
                    default:    
                    break;
                }
            break;
        }
    }
}

inline void process_vibration_analyzer_menu(void){
    static int16 ValorX = 0;
    static int16 ValorY = 0;
    
    MotorOutput[MOTOR_FRONT] = 2000 + (-PPMValue[RADIO_PITCH_CH] + PPMOffset[RADIO_PITCH_CH]) * 2;
    MotorOutput[MOTOR_BACK] = 2000 + (+PPMValue[RADIO_PITCH_CH] - PPMOffset[RADIO_PITCH_CH]) * 2;
    MotorOutput[MOTOR_LEFT] = 2000 + (-PPMValue[RADIO_ROLL_CH] + PPMOffset[RADIO_ROLL_CH]) * 2;
    MotorOutput[MOTOR_RIGHT] = 2000 + (+PPMValue[RADIO_ROLL_CH] - PPMOffset[RADIO_ROLL_CH]) * 2;
    
    set_motor_output();
    
    // apaga a seta atual
    lcd_drawline(LCD_CENTER_VIBRATION_X, LCD_CENTER_VIBRATION_Y, (ValorX >> 3) + LCD_CENTER_VIBRATION_X, (ValorY >> 3)+ LCD_CENTER_VIBRATION_Y, LCDBackColor);
    
    lcd_goto(5,15);
    printf("%04d %04d", ValorX, ValorY);
    
    ValorY = (ValorY * 3 + (-AccelValue[PITCH_INDEX] + AnalogOffset[ACCELY_ACH])) >> 2;
    ValorX = (ValorX * 3 + (-AccelValue[ROLL_INDEX] + AnalogOffset[ACCELX_ACH])) >> 2;
    
    // desenha a seta nova
    // valor maximo estimado = 200000, entao dando uma shiftada de 12 >> acho q cabe na tela do lcd
    lcd_drawline(LCD_CENTER_VIBRATION_X, LCD_CENTER_VIBRATION_Y, (ValorX >> 3) + LCD_CENTER_VIBRATION_X, (ValorY >> 3) + LCD_CENTER_VIBRATION_Y, LCDForeColor);
    
    //  mostra uma seta no meio da tela que aponta pro eixo que tem mais vibracao x,y
}

// esta funcao utiliza registrador do uC diretamente
// tudo para 16MHz
void clock_init(void){
    BCSCTL2 = SELM_0 + DIVM_0 + DIVS_0;
    DCOCTL = 0x77;      
    BCSCTL1 = 0xCF;     
    BCSCTL3 = 0xA0;
}

// esta funcao utiliza registrador do uC diretamente
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

// esta funcao utiliza registrador do uC diretamente
inline void set_motor_output(void){
    TBCCR1 = constrain(MotorOutput[0], 0, MAX_MOTOR);
    TBCCR2 = constrain(MotorOutput[1], 0, MAX_MOTOR);
    TBCCR5 = constrain(MotorOutput[2], 0, MAX_MOTOR);
    TBCCR4 = constrain(MotorOutput[3], 0, MAX_MOTOR);
    TBCCR3 = constrain(MotorOutput[4], 0, MAX_MOTOR);
    TBCCR6 = constrain(MotorOutput[5], 0, MAX_MOTOR);
}

// esta funcao utiliza registrador do uC diretamente
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
    TBCCR0 = 5001; // 2.5ms = 400hz

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

// esta funcao utiliza registrador do uC diretamente
void setup(void){   
    //P2DIR |= 0x08;
    //P2OUT |= 0x08;
    int i = 100;
    
    clock_init();
    timer_init(); // inicia com 0 ppm no ESC (ESC fica esperando sinal valido beep --- beep --- beep)
    
    P1_init(); // ppm receiver interrupt
    
#ifdef LIGHTS_ON_I2C
    P3SEL &= (0x02 | 0x04);
    P3DIR |= (0x02 | 0x04);
#endif
#ifdef LIGHTS_ON_UART
    P3SEL &= (0x20);
    P3DIR |= (0x20);
#endif
    
    eint(); // habilita interrupt

    lcd_init(LCDBackColor);    
    
    delayms(350);
 
    // iluminacao progressiva do LCD
    while(TACCR1 < LCD_MAX_BRIGHT){
        TACCR1 = TACCR1 + 1;
        delayus(1500);
    }
        
    // se canal 5 ta on
    if(PPMRaw[5] > STICK_UPPER_THRESHOLD){
        // e o throttle ta alto
        if(PPMRaw[RADIO_THROTTLE_CH] > STICK_LOWER_THRESHOLD){
            // significa que o cara quer programar o esc
            printf(str_programando_esc);           
            while(PPMRaw[5] > STICK_UPPER_THRESHOLD){
                set_all_motors(PPMRaw[RADIO_THROTTLE_CH]);    
            }
        }
        else{
            // significa que o cara quer usar motor grande
            init_vars();
        }
    }
    else{
    // se o canal 5 ta off
        // inicializa motor pequeno
        init_vars();   
        // se o throttle ta alto
        if(PPMRaw[RADIO_THROTTLE_CH] > STICK_LOWER_THRESHOLD){
            // significa que o cara quer calibrar o esc
            printf(str_calibrando_esc);
            while(i--){
                set_all_motors(PPMRaw[RADIO_THROTTLE_CH]);
                delayms(80);
            }       
        }
    }
    
    set_all_motors(MIN_MOTOR);
    printf(str_inicializando);           
     
    analog_init(); // config registradores        
    analog_calibrate_channel(0);
    printf(str_pontinho);
    analog_calibrate_channel(1);
    printf(str_pontinho);
    analog_calibrate_channel(2);
    printf(str_pontinho);
    analog_calibrate_channel(3);
    printf(str_pontinho);
    analog_calibrate_channel(4);
    printf(str_pontinho);
    analog_calibrate_channel(5);
    printf(str_pontinho);
    analog_calibrate_channel(6);
    printf(str_pontinho);
    analog_calibrate_channel(7);
    printf(str_pontinho);
        
    AccelOffset[PITCH_INDEX] = AnalogOffset[ACCELY_ACH];
    AccelOffset[ROLL_INDEX] = AnalogOffset[ACCELX_ACH];
    AccelOffset[YAW_INDEX] = AnalogOffset[ACCELZ_ACH];
    
    calibrate_radio();
    printf(str_pontinho);
    printf(str_pontinho);
    printf(str_pontinho);
    
    lcd_clear(LCDBackColor);        
    menu_init();
}

void process_analog_graph(void){
    int i, k, j, y;
    for(i = 0; i < 8; i++){
        if((1 << i) & AnalogChecked.BYTE){
            // apaga
            for(k = 0; k < (GRAPH_LENGHT - 1); k++){
                lcd_drawpoint(k+GRAPH_OFFSETX, AnalogGraph[i][k] ,GRAY);
                j = AnalogGraph[i][k]; // posicao atual
                y = AnalogGraph[i][k+1]; // proxima posicao
                if(j != y){ // se agora ta mais alto q depois
                    if(j < y){
                        y--; 
                        while(j++ < y){
                            lcd_drawpoint(k+GRAPH_OFFSETX, j ,GRAY);
                        }
                    }
                    else{
                        y++;
                        while(j-- > y){
                            lcd_drawpoint(k+GRAPH_OFFSETX, j ,GRAY);
                        }
                    }
                }
                AnalogGraph[i][k] = AnalogGraph[i][k+1];
            } 
            // apaga ultimo
            lcd_drawpoint(GRAPH_LENGHT - 1 + GRAPH_OFFSETX, AnalogGraph[i][GRAPH_LENGHT - 1] , GRAY);                       
            // insere mais um valor
            AnalogGraph[i][GRAPH_LENGHT - 1] = constrain(GRAPH_OFFSET - ((AnalogValue[i] >> 6) & 0xFF), GRAPH_START, GRAPH_START+GRAPH_HEIGHT);            
            // redesenha
            for(k = 0; k < (GRAPH_LENGHT - 1); k++){
                lcd_drawpoint(k+GRAPH_OFFSETX, AnalogGraph[i][k] ,AnalogColours[i]);
                j = AnalogGraph[i][k]; // posicao atual
                y = AnalogGraph[i][k+1]; // proxima posicao
                if(j != y){ // se agora ta mais alto q depois
                    if(j < y){
                        y--; 
                        while(j++ < y){
                            lcd_drawpoint(k+GRAPH_OFFSETX, j ,AnalogColours[i]);
                        }
                    }
                    else{
                        y++;
                        while(j-- > y){
                            lcd_drawpoint(k+GRAPH_OFFSETX, j ,AnalogColours[i]);
                        }
                    }
                }
            } 
            // desenha ultimo
            lcd_drawpoint(GRAPH_LENGHT - 1 + GRAPH_OFFSETX, AnalogGraph[i][GRAPH_LENGHT - 1] ,AnalogColours[i]);            
        }
    }    
}

void analog_graph_clear(int i){
    int k, j, y;
    // apaga o grafico
    for(k = 0; k < (GRAPH_LENGHT - 1); k++){
        lcd_drawpoint(k+GRAPH_OFFSETX, AnalogGraph[i][k] , GRAY);
        j = AnalogGraph[i][k]; // posicao atual
        y = AnalogGraph[i][k+1]; // proxima posicao
        if(j != y){ // se agora ta mais alto q depois
            if(j < y){
                y--; 
                while(j++ < y){
                    lcd_drawpoint(k+GRAPH_OFFSETX, j , GRAY);
                }
            }
            else{
                y++;
                while(j-- > y){
                    lcd_drawpoint(k+GRAPH_OFFSETX, j , GRAY);
                }
            }
        }
        //AnalogGraph[i][k] = AnalogGraph[i][k+1];
    } 
    // apaga ultimo
    lcd_drawpoint(GRAPH_LENGHT - 1 + GRAPH_OFFSETX, AnalogGraph[i][GRAPH_LENGHT - 1] , GRAY);
}

/* DEVE SER MONTADO NESTA ORDEM
    LETSFLY_INDEX               ,
    ANALOG_MENU_INDEX           ,
    CONTROL_MENU_INDEX          ,
    RADIO_MENU_INDEX            ,
    MOTOR_MENU_INDEX            ,
    SENSOR_MENU_INDEX           ,
    VOLTAGE_DROP_INDEX          ,
    VIBRATION_ANALYZER_INDEX    ,
    OPTION_MENU_INDEX           */

void menu_init(void){
    MainMenu = menu_create(str_main_menu, create_item(str_letsfly, ITEMTYPE_SUBMENU, NULL, NULL, NULL, NULL), &val_janela_size_large, NULL, NULL, &val_arrow_center, NULL);
    menu_add_item(MainMenu, create_item(str_analog_menu, ITEMTYPE_SUBMENU, NULL, NULL, NULL, NULL));
    menu_add_item(MainMenu, create_item(str_control_menu, ITEMTYPE_SUBMENU, NULL, NULL, NULL, NULL));
    menu_add_item(MainMenu, create_item(str_filter_menu, ITEMTYPE_SUBMENU, NULL, NULL, NULL, NULL));
    menu_add_item(MainMenu, create_item(str_radio_menu, ITEMTYPE_SUBMENU, NULL, NULL, NULL, NULL));
    menu_add_item(MainMenu, create_item(str_motor_menu, ITEMTYPE_SUBMENU, NULL, NULL, NULL, NULL));
    menu_add_item(MainMenu, create_item(str_sensor_menu, ITEMTYPE_SUBMENU, NULL, NULL, NULL, NULL));
    menu_add_item(MainMenu, create_item(str_voltage_drop_menu, ITEMTYPE_SUBMENU, NULL, NULL, NULL, NULL));    
    menu_add_item(MainMenu, create_item(str_vibration_analyzer_menu, ITEMTYPE_SUBMENU, NULL, NULL, NULL, NULL));    
    menu_add_item(MainMenu, create_item(str_option_menu, ITEMTYPE_SUBMENU, NULL, NULL, NULL, NULL));    

    // analog menu
    AnalogMenu = menu_create(str_analog_menu, create_item(str_return, ITEMTYPE_SUBMENU, NULL, NULL, NULL, NULL), &val_janela_size_small, &val_bar_position_center, &val_value_position_right, &val_arrow_center, &val_bar_lenght_medium);
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
    ControlMenu = menu_create(str_control_menu, create_item(str_return, ITEMTYPE_SUBMENU, NULL, NULL, NULL, NULL), &val_janela_size_large, &val_bar_position_center, &val_value_position_right, &val_arrow_center, &val_bar_lenght_medium);
    
    menu_add_item(ControlMenu, create_item(str_pitch_roll_sat, ITEMTYPE_VALUE_RW, &val_saturation_limit_low, &val_saturation_limit_high, &val_ten, (int*)&PitchRollPSaturation));
    menu_add_item(ControlMenu, create_item(str_yawpm, ITEMTYPE_VALUE_RW, &val_one, &val_max_control, &val_one, (int*)&ControlProportionalMul[YAW_INDEX]));
    menu_add_item(ControlMenu, create_item(str_pitchpm, ITEMTYPE_VALUE_RW, &val_one, &val_max_control, &val_one, (int*)&ControlProportionalMul[PITCH_INDEX]));
    menu_add_item(ControlMenu, create_item(str_rollpm, ITEMTYPE_VALUE_RW, &val_one, &val_max_control, &val_one, (int*)&ControlProportionalMul[ROLL_INDEX]));        
    menu_add_item(ControlMenu, create_item(str_yawpd, ITEMTYPE_VALUE_RW, &val_zero, &val_max_control, &val_one, (int*)&ControlProportionalDiv[YAW_INDEX]));
    menu_add_item(ControlMenu, create_item(str_pitchpd, ITEMTYPE_VALUE_RW, &val_zero, &val_max_control, &val_one, (int*)&ControlProportionalDiv[PITCH_INDEX]));
    menu_add_item(ControlMenu, create_item(str_rollpd, ITEMTYPE_VALUE_RW, &val_zero, &val_max_control, &val_one, (int*)&ControlProportionalDiv[ROLL_INDEX]));        
    menu_add_item(ControlMenu, create_item(str_yawid, ITEMTYPE_VALUE_RW, &val_zero, &val_ten, &val_one, (int*)&ControlIntegralDiv[YAW_INDEX]));       
    menu_add_item(ControlMenu, create_item(str_pitchid, ITEMTYPE_VALUE_RW, &val_zero, &val_ten, &val_one, (int*)&ControlIntegralDiv[PITCH_INDEX]));       
    menu_add_item(ControlMenu, create_item(str_rollid, ITEMTYPE_VALUE_RW, &val_zero, &val_ten, &val_one, (int*)&ControlIntegralDiv[ROLL_INDEX]));       
    menu_add_item(ControlMenu, create_item(str_yawilim, ITEMTYPE_VALUE_RW, &val_zero, &val_max_int, &val_hundred, (int*)&ControlIntegralLimit[YAW_INDEX]));       
    menu_add_item(ControlMenu, create_item(str_pitchilim, ITEMTYPE_VALUE_RW, &val_zero, &val_max_int, &val_hundred, (int*)&ControlIntegralLimit[PITCH_INDEX]));       
    menu_add_item(ControlMenu, create_item(str_rollilim, ITEMTYPE_VALUE_RW, &val_zero, &val_max_int, &val_hundred, (int*)&ControlIntegralLimit[ROLL_INDEX]));       

    // filters menu
    FilterMenu = menu_create(str_filter_menu, create_item(str_return, ITEMTYPE_SUBMENU, NULL, NULL, NULL, NULL), &val_janela_size_large, &val_bar_position_center, &val_value_position_right, &val_arrow_center, &val_bar_lenght_medium);
    menu_add_item(FilterMenu, create_item(str_throttle_lp_mul, ITEMTYPE_VALUE_RW, &val_zero, &val_max_control, &val_one, (int*)&ThrottleLPMul));        
    menu_add_item(FilterMenu, create_item(str_throttle_lp_div, ITEMTYPE_VALUE_RW, &val_zero, &val_max_control, &val_one, (int*)&ThrottleLPDiv));        
    menu_add_item(FilterMenu, create_item(str_motor_lp_mul, ITEMTYPE_VALUE_RW, &val_zero, &val_max_control, &val_one, (int*)&MotorOutputLPMul));        
    menu_add_item(FilterMenu, create_item(str_motor_lp_div, ITEMTYPE_VALUE_RW, &val_zero, &val_max_control, &val_one, (int*)&MotorOutputLPDiv));        
    menu_add_item(FilterMenu, create_item(str_gyro_lp_mul, ITEMTYPE_VALUE_RW, &val_zero, &val_max_control, &val_one, (int*)&GyroLPMul));        
    menu_add_item(FilterMenu, create_item(str_gyro_lp_div, ITEMTYPE_VALUE_RW, &val_zero, &val_max_control, &val_one, (int*)&GyroLPDiv));        
    menu_add_item(FilterMenu, create_item(str_accel_lp_mul, ITEMTYPE_VALUE_RW, &val_zero, &val_max_control, &val_one, (int*)&AccelLPMul));        
    menu_add_item(FilterMenu, create_item(str_accel_lp_div, ITEMTYPE_VALUE_RW, &val_zero, &val_max_control, &val_one, (int*)&AccelLPDiv));        
       
    // radio menu
    RadioMenu = menu_create(str_radio_menu, create_item(str_return, ITEMTYPE_SUBMENU, NULL, NULL, NULL, NULL), &val_janela_size_medium, &val_bar_position_center, &val_value_position_right, &val_arrow_center, &val_bar_lenght_medium);
    menu_add_item(RadioMenu, create_item(str_calibrate, ITEMTYPE_SUBMENU, NULL, NULL, NULL, NULL));            
    //menu_add_item(RadioMenu, create_item(str_calibrate_throttle, ITEMTYPE_SUBMENU, NULL, NULL, NULL, NULL));                    
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
    MotorMenu = menu_create(str_motor_menu, create_item(str_return, ITEMTYPE_SUBMENU, NULL, NULL, NULL, NULL), &val_janela_size_medium, &val_bar_position_center, &val_value_position_right, &val_arrow_center, &val_bar_lenght_medium);
    menu_add_item(MotorMenu, create_item(str_motor1, ITEMTYPE_VALUE_BAR_RW, &val_min_motor, &val_max_motor, &val_int_motor, (int*)&MotorOutput[0]));
    menu_add_item(MotorMenu, create_item(str_motor2, ITEMTYPE_VALUE_BAR_RW, &val_min_motor, &val_max_motor, &val_int_motor, (int*)&MotorOutput[1]));
    menu_add_item(MotorMenu, create_item(str_motor3, ITEMTYPE_VALUE_BAR_RW, &val_min_motor, &val_max_motor, &val_int_motor, (int*)&MotorOutput[2]));
    menu_add_item(MotorMenu, create_item(str_motor4, ITEMTYPE_VALUE_BAR_RW, &val_min_motor, &val_max_motor, &val_int_motor, (int*)&MotorOutput[3]));
    menu_add_item(MotorMenu, create_item(str_motor5, ITEMTYPE_VALUE_BAR_RW, &val_min_motor, &val_max_motor, &val_int_motor, (int*)&MotorOutput[4]));
    menu_add_item(MotorMenu, create_item(str_motor6, ITEMTYPE_VALUE_BAR_RW, &val_min_motor, &val_max_motor, &val_int_motor, (int*)&MotorOutput[5]));    
    menu_add_item(MotorMenu, create_item(str_calibrate, ITEMTYPE_SUBMENU, NULL, NULL, NULL, NULL));    
    // sensor menu
    SensorMenu = menu_create(str_sensor_menu, create_item(str_return, ITEMTYPE_SUBMENU, NULL, NULL, NULL, NULL), &val_janela_size_medium, &val_bar_position_center, &val_value_position_right, &val_arrow_center, &val_bar_lenght_medium);
    menu_add_item(SensorMenu, create_item(str_accelx, ITEMTYPE_VALUE_R, &val_min_int, &val_max_int, NULL, (int*)&AccelValue[ROLL_INDEX]));
    menu_add_item(SensorMenu, create_item(str_accely, ITEMTYPE_VALUE_R, &val_min_int, &val_max_int, NULL, (int*)&AccelValue[PITCH_INDEX]));
    menu_add_item(SensorMenu, create_item(str_accelz, ITEMTYPE_VALUE_R, &val_min_int, &val_max_int, NULL, (int*)&AccelValue[YAW_INDEX]));
    menu_add_item(SensorMenu, create_item(str_gyrox, ITEMTYPE_VALUE_R, &val_min_int, &val_max_int, NULL, (int*)&GyroValue[ROLL_INDEX]));
    menu_add_item(SensorMenu, create_item(str_gyroy, ITEMTYPE_VALUE_R, &val_min_int, &val_max_int, NULL, (int*)&GyroValue[PITCH_INDEX]));
    menu_add_item(SensorMenu, create_item(str_gyroz, ITEMTYPE_VALUE_R, &val_min_int, &val_max_int, NULL, (int*)&GyroValue[YAW_INDEX]));    
    // vibration analyzer menu
    VibrationAnalyzerMenu = menu_create(str_vibration_analyzer_menu, create_item(str_vibration_desc, ITEMTYPE_SUBMENU, NULL, NULL, NULL, NULL), &val_janela_size_large, &val_bar_position_center, &val_value_position_right, &val_arrow_center, &val_bar_lenght_medium);
    // vdrop menu
    VoltageDropMenu = menu_create(str_voltage_drop_menu, create_item(str_return, ITEMTYPE_SUBMENU, NULL, NULL, NULL, NULL), &val_janela_size_large, &val_bar_position_center, &val_value_position_right, &val_arrow_center, &val_bar_lenght_medium);
    menu_add_item(VoltageDropMenu, create_item(str_battery_max, ITEMTYPE_VALUE_R, &val_zero, &val_max_analog, NULL, (int*)&VdropMaxBat));
    menu_add_item(VoltageDropMenu, create_item(str_battery, ITEMTYPE_VALUE_R, &val_zero, &val_max_analog, NULL, (int*)&Vbat));    
    menu_add_item(VoltageDropMenu, create_item(str_battery_min, ITEMTYPE_VALUE_R, &val_zero, &val_max_analog, NULL, (int*)&VdropMinBat));
    // option menu
    OptionMenu = menu_create(str_option_menu, create_item(str_return, ITEMTYPE_SUBMENU, NULL, NULL, NULL, NULL), &val_janela_size_medium, &val_bar_position_center, &val_value_position_right, &val_arrow_center, &val_bar_lenght_medium);
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
    else{
        if(PPMValue[RADIO_ROLL_CH] < STICK_LOWER_THRESHOLD){
            return ACTION_LEFT;
        }
    }
    
    if(PPMValue[RADIO_PITCH_CH] < STICK_LOWER_THRESHOLD){
        return ACTION_UP;
    }
    else{
        if(PPMValue[RADIO_PITCH_CH] > STICK_UPPER_THRESHOLD){
            return ACTION_DOWN;
        }
    }
    
    if(PPMValue[RADIO_YAW_CH] < STICK_LOWER_THRESHOLD){
        return ACTION_TURN_C;
    }
    else{
        if(PPMValue[RADIO_YAW_CH] > STICK_UPPER_THRESHOLD){
            return ACTION_TURN_CW;
        }
    }
    
    return ACTION_NONE;
}

void set_all_motors(uint16 val){
    MotorOutput[0] = val;
    MotorOutput[1] = val;
    MotorOutput[2] = val;
    MotorOutput[3] = val;
    MotorOutput[4] = val;
    MotorOutput[5] = val;
    
    set_motor_output();
}

// esta funcao utiliza registrador do uC diretamente
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



inline void control_loop_acro(void){
    // variaveis controle sinais de controle

    static int16 ControlProportionalResult[3] = {0, 0, 0};
    static int16 ControlRadioResult[3] = {0, 0, 0};
    static int16 ControlIntegralResult[3] = {0, 0, 0};

    
    static int16 Error[3] = {0, 0, 0};
    static int16 MotorOutputOld[6] = {MIN_MOTOR, MIN_MOTOR, MIN_MOTOR, MIN_MOTOR, MIN_MOTOR, MIN_MOTOR};
    
    /*    
    GYRO
    pitch aumenta qd quad vai pra frente
    pitch diminui qd quad vai pra traz
    
    roll diminui qd o quad vai para a esquerda
    roll aumenta qd o quad vai para a direita
    
    yaw diminui horario
    yaw aumenta anti-horario
    
    MOTORES
    
    cima e baixo anti-horario
    esquerda e direita horario
    
    no yaw como funciona:
    - vai girar horario se aumentar a rotacao dos motores que giram anti-horario:
    MOTOR FRONT e BACK ++++ = girar horario
    MOTOR RIGHT e LEFT ++++ = anti-horario
    
    RADIO
    
    pitch pra frente diminui
    roll pra esquerda diminui
    yaw horario aumenta
    
    RADIO E GYRO TEM Q COINCIDIR!
    entao na hora do erro eu tenho q ver se eu somo ou subtraio
    
    o pitch ta invertido
    o roll ta ok
    o yaw ta invertido
    
    */
    
    if(PPMValue[RADIO_THROTTLE_CH] > STICK_LOWER_THRESHOLD){    
        ThrottleFiltered = ((ThrottleFiltered * (long)ThrottleLPMul + PPMValue[RADIO_THROTTLE_CH]) >> (int16)ThrottleLPDiv);
        
        // ajusta radio para funcionar de +313 a -312
        // 313 = 100º/s e -312 = -100º/s
        ControlRadioResult[ROLL_INDEX] = PPMValue[RADIO_ROLL_CH] - PPMOffset[RADIO_ROLL_CH];
        ControlRadioResult[YAW_INDEX] = PPMValue[RADIO_YAW_CH] - PPMOffset[RADIO_YAW_CH];
        ControlRadioResult[PITCH_INDEX] = PPMValue[RADIO_PITCH_CH] - PPMOffset[RADIO_PITCH_CH];

        ControlRadioResult[ROLL_INDEX] = (ControlRadioResult[ROLL_INDEX] * 5) >> 4; 
        ControlRadioResult[YAW_INDEX] = (ControlRadioResult[YAW_INDEX] * 5) >> 4; 
        ControlRadioResult[PITCH_INDEX] = (ControlRadioResult[PITCH_INDEX] * 5) >> 4; 
        
        // agora o radio ta entre -312 e +313
        // tem q ajustar o erro agora q tbm vai ficar nessa faixa
        Error[ROLL_INDEX] = ControlRadioResult[ROLL_INDEX] - GyroValue[ROLL_INDEX];
        Error[YAW_INDEX] = ControlRadioResult[YAW_INDEX] - GyroValue[YAW_INDEX];
        Error[PITCH_INDEX] = ControlRadioResult[PITCH_INDEX] - GyroValue[PITCH_INDEX];
        
        // agora multiplica pelo proportional        
        ControlProportionalResult[ROLL_INDEX] = constrain((Error[ROLL_INDEX] * ControlProportionalMul[ROLL_INDEX]) >> ControlProportionalDiv[ROLL_INDEX], -PitchRollPSaturation, PitchRollPSaturation);
        ControlProportionalResult[PITCH_INDEX] = constrain((Error[PITCH_INDEX] * ControlProportionalMul[PITCH_INDEX]) >> ControlProportionalDiv[PITCH_INDEX], -PitchRollPSaturation, PitchRollPSaturation);
        ControlProportionalResult[YAW_INDEX] = (Error[YAW_INDEX] * ControlProportionalMul[YAW_INDEX]) >> ControlProportionalDiv[YAW_INDEX];

        // integrador
        ControlIntegralResult[ROLL_INDEX] += Error[ROLL_INDEX];
        ControlIntegralResult[PITCH_INDEX] += Error[PITCH_INDEX];
        ControlIntegralResult[YAW_INDEX] += Error[YAW_INDEX];
        
        ControlIntegralResult[ROLL_INDEX] = constrain(ControlIntegralResult[ROLL_INDEX], -ControlIntegralLimit[ROLL_INDEX], ControlIntegralLimit[ROLL_INDEX]);
        ControlIntegralResult[PITCH_INDEX] = constrain(ControlIntegralResult[PITCH_INDEX], -ControlIntegralLimit[PITCH_INDEX], ControlIntegralLimit[PITCH_INDEX]);
        ControlIntegralResult[YAW_INDEX] = constrain(ControlIntegralResult[YAW_INDEX], -ControlIntegralLimit[YAW_INDEX], ControlIntegralLimit[YAW_INDEX]);
                
        // derivativo
        
        // output mixing        
        MotorOutput[MOTOR_RIGHT] = ThrottleFiltered - ControlProportionalResult[ROLL_INDEX] - (ControlIntegralResult[ROLL_INDEX] >> ControlIntegralDiv[ROLL_INDEX]) - ControlProportionalResult[YAW_INDEX] - (ControlIntegralResult[YAW_INDEX] >> ControlIntegralDiv[YAW_INDEX]);
        MotorOutput[MOTOR_LEFT] = ThrottleFiltered + ControlProportionalResult[ROLL_INDEX] + (ControlIntegralResult[ROLL_INDEX] >> ControlIntegralDiv[ROLL_INDEX]) - ControlProportionalResult[YAW_INDEX] - (ControlIntegralResult[YAW_INDEX] >> ControlIntegralDiv[YAW_INDEX]);
        MotorOutput[MOTOR_FRONT] = ThrottleFiltered  + ControlProportionalResult[PITCH_INDEX] + (ControlIntegralResult[PITCH_INDEX] >> ControlIntegralDiv[PITCH_INDEX]) + ControlProportionalResult[YAW_INDEX] + (ControlIntegralResult[YAW_INDEX] >> ControlIntegralDiv[YAW_INDEX]);
        MotorOutput[MOTOR_BACK] = ThrottleFiltered - ControlProportionalResult[PITCH_INDEX] - (ControlIntegralResult[PITCH_INDEX] >> ControlIntegralDiv[PITCH_INDEX]) + ControlProportionalResult[YAW_INDEX] + (ControlIntegralResult[YAW_INDEX] >> ControlIntegralDiv[YAW_INDEX]);

        // low pass na saida
        MotorOutput[MOTOR_FRONT] = (MotorOutput[MOTOR_FRONT] + MotorOutputOld[MOTOR_FRONT] * (int16)MotorOutputLPMul) >> (int16)MotorOutputLPDiv;
        MotorOutput[MOTOR_LEFT] =  (MotorOutput[MOTOR_LEFT] + MotorOutputOld[MOTOR_LEFT] * (int16)MotorOutputLPMul) >> (int16)MotorOutputLPDiv;
        MotorOutput[MOTOR_BACK] = (MotorOutput[MOTOR_BACK] + MotorOutputOld[MOTOR_BACK] * (int16)MotorOutputLPMul) >> (int16)MotorOutputLPDiv;
        MotorOutput[MOTOR_RIGHT] = (MotorOutput[MOTOR_RIGHT] + MotorOutputOld[MOTOR_RIGHT] * (int16)MotorOutputLPMul) >> (int16)MotorOutputLPDiv; 
    
#ifndef DEBUG_MODE
        set_motor_output();
#endif // DEBUG_MODE
            
        // coleta sinal atual pra depois fazer o lowpass
        MotorOutputOld[MOTOR_FRONT] = MotorOutput[MOTOR_FRONT];
        MotorOutputOld[MOTOR_LEFT] = MotorOutput[MOTOR_LEFT];
        MotorOutputOld[MOTOR_BACK] = MotorOutput[MOTOR_BACK];
        MotorOutputOld[MOTOR_RIGHT] = MotorOutput[MOTOR_RIGHT];

        if(get_delay(DELAY_FLIGHT_TIME)){
            FlightTime++;
            set_delay(DELAY_FLIGHT_TIME, 500);
        }
    }
    else{
        ControlIntegralResult[ROLL_INDEX] = 0;
        ControlIntegralResult[YAW_INDEX] = 0;
        ControlIntegralResult[PITCH_INDEX] = 0;
        
        Angle[ROLL_INDEX] = 0;
        Angle[PITCH_INDEX] = 0;
        
        set_all_motors(MIN_MOTOR);
    }
}


inline void control_loop_stable(void){
    static int16 ControlProportionalResult[3] = {0, 0, 0};
    static int16 ControlRadioResult[3] = {0, 0, 0};
    static int16 ControlIntegralResult[3] = {0, 0, 0};
    
    static int16 Error[3] = {0, 0, 0};
    static int16 MotorOutputOld[6] = {MIN_MOTOR, MIN_MOTOR, MIN_MOTOR, MIN_MOTOR, MIN_MOTOR, MIN_MOTOR};
    
    /*    
    GYRO
    pitch aumenta qd quad vai pra frente
    pitch diminui qd quad vai pra traz
    
    roll diminui qd o quad vai para a esquerda
    roll aumenta qd o quad vai para a direita
    
    yaw diminui horario
    yaw aumenta anti-horario
    
    MOTORES
    
    cima e baixo anti-horario
    esquerda e direita horario
    
    no yaw como funciona:
    - vai girar horario se aumentar a rotacao dos motores que giram anti-horario:
    MOTOR FRONT e BACK ++++ = girar horario
    MOTOR RIGHT e LEFT ++++ = anti-horario
    
    RADIO
    
    pitch pra frente diminui
    roll pra esquerda diminui
    yaw horario aumenta
    
    RADIO E GYRO TEM Q COINCIDIR!
    entao na hora do erro eu tenho q ver se eu somo ou subtraio
    
    o pitch ta invertido
    o roll ta ok
    o yaw ta invertido
    
    */
    
    if(PPMValue[RADIO_THROTTLE_CH] > STICK_LOWER_THRESHOLD){    
        ThrottleFiltered = ((ThrottleFiltered * (int16)ThrottleLPMul + PPMValue[RADIO_THROTTLE_CH]) >> (int16)ThrottleLPDiv);

        // integrador d o gyro
        GyroSum[ROLL_INDEX] += GyroValue[ROLL_INDEX];
        GyroSum[PITCH_INDEX] += GyroValue[PITCH_INDEX];
        GyroSum[YAW_INDEX] += GyroValue[YAW_INDEX];
        // tem q dividir isso pra que a cada 1s ele aumente no maximo ate 310
        // supondo q ele ta somando 312 (que eh o maximo) a cada 2.5ms, ele vai somar 400x em 1s
        // entao em 1s eu vou ter 312 * 400 = xxx entao eu tenho q dividir por 400 pra ter o valor real...
        // mas nao tem 2^x que de 400, entao tem q pensar um pouquinho
        // multiplicando por 5 e shifitando >> 11 da pra chegar em torno de 304 que ja esta bom
        Angle[ROLL_INDEX] = ((int)((GyroSum[ROLL_INDEX] * 5) >> 11) * 15 + AccelValue[ROLL_INDEX]) >> 4;
        Angle[PITCH_INDEX] = ((int)((GyroSum[PITCH_INDEX] * 5) >> 11) * 15 + AccelValue[PITCH_INDEX]) >> 4;
                
        // ajusta radio para funcionar de +313 a -312
        // 313 = 100º/s e -312 = -100º/s
        ControlRadioResult[ROLL_INDEX] = PPMValue[RADIO_ROLL_CH] - PPMOffset[RADIO_ROLL_CH];
        ControlRadioResult[YAW_INDEX] = PPMValue[RADIO_YAW_CH] - PPMOffset[RADIO_YAW_CH];
        ControlRadioResult[PITCH_INDEX] = PPMValue[RADIO_PITCH_CH] - PPMOffset[RADIO_PITCH_CH];

        ControlRadioResult[ROLL_INDEX] = (ControlRadioResult[ROLL_INDEX] * 5) >> 4; 
        ControlRadioResult[YAW_INDEX] = (ControlRadioResult[YAW_INDEX] * 5) >> 4; 
        ControlRadioResult[PITCH_INDEX] = (ControlRadioResult[PITCH_INDEX] * 5) >> 4; 
        
        // agora o radio ta entre -312 e +313
        // tem q ajustar o erro agora q tbm vai ficar nessa faixa
        Error[ROLL_INDEX] = ControlRadioResult[ROLL_INDEX] - Angle[ROLL_INDEX];
        Error[YAW_INDEX] = ControlRadioResult[YAW_INDEX] - GyroValue[YAW_INDEX];
        Error[PITCH_INDEX] = ControlRadioResult[PITCH_INDEX] - Angle[PITCH_INDEX];
        
        // agora multiplica pelo proportional        
        ControlProportionalResult[ROLL_INDEX] = constrain((Error[ROLL_INDEX] * ControlProportionalMul[ROLL_INDEX]) >> ControlProportionalDiv[ROLL_INDEX], -PitchRollPSaturation, PitchRollPSaturation);
        ControlProportionalResult[PITCH_INDEX] = constrain((Error[PITCH_INDEX] * ControlProportionalMul[PITCH_INDEX]) >> ControlProportionalDiv[PITCH_INDEX], -PitchRollPSaturation, PitchRollPSaturation);
        ControlProportionalResult[YAW_INDEX] = (Error[YAW_INDEX] * ControlProportionalMul[YAW_INDEX]) >> ControlProportionalDiv[YAW_INDEX];

        // integrador
        ControlIntegralResult[ROLL_INDEX] += Error[ROLL_INDEX];
        ControlIntegralResult[PITCH_INDEX] += Error[PITCH_INDEX];
        ControlIntegralResult[YAW_INDEX] += Error[YAW_INDEX];
        
        ControlIntegralResult[ROLL_INDEX] = constrain(ControlIntegralResult[ROLL_INDEX], -ControlIntegralLimit[ROLL_INDEX], ControlIntegralLimit[ROLL_INDEX]);
        ControlIntegralResult[PITCH_INDEX] = constrain(ControlIntegralResult[PITCH_INDEX], -ControlIntegralLimit[PITCH_INDEX], ControlIntegralLimit[PITCH_INDEX]);
        ControlIntegralResult[YAW_INDEX] = constrain(ControlIntegralResult[YAW_INDEX], -ControlIntegralLimit[YAW_INDEX], ControlIntegralLimit[YAW_INDEX]);
                
        // derivativo
        
        // output mixing        
        MotorOutput[MOTOR_RIGHT] = ThrottleFiltered - ControlProportionalResult[ROLL_INDEX] - (ControlIntegralResult[ROLL_INDEX] >> ControlIntegralDiv[ROLL_INDEX]) - ControlProportionalResult[YAW_INDEX] - (ControlIntegralResult[YAW_INDEX] >> ControlIntegralDiv[YAW_INDEX]);
        MotorOutput[MOTOR_LEFT] = ThrottleFiltered + ControlProportionalResult[ROLL_INDEX] + (ControlIntegralResult[ROLL_INDEX] >> ControlIntegralDiv[ROLL_INDEX]) - ControlProportionalResult[YAW_INDEX] - (ControlIntegralResult[YAW_INDEX] >> ControlIntegralDiv[YAW_INDEX]);
        MotorOutput[MOTOR_FRONT] = ThrottleFiltered  + ControlProportionalResult[PITCH_INDEX] + (ControlIntegralResult[PITCH_INDEX] >> ControlIntegralDiv[PITCH_INDEX]) + ControlProportionalResult[YAW_INDEX] + (ControlIntegralResult[YAW_INDEX] >> ControlIntegralDiv[YAW_INDEX]);
        MotorOutput[MOTOR_BACK] = ThrottleFiltered - ControlProportionalResult[PITCH_INDEX] - (ControlIntegralResult[PITCH_INDEX] >> ControlIntegralDiv[PITCH_INDEX]) + ControlProportionalResult[YAW_INDEX] + (ControlIntegralResult[YAW_INDEX] >> ControlIntegralDiv[YAW_INDEX]);

        // low pass na saida
        MotorOutput[MOTOR_FRONT] = (MotorOutput[MOTOR_FRONT] + MotorOutputOld[MOTOR_FRONT] * (int16)MotorOutputLPMul) >> (int16)MotorOutputLPDiv;
        MotorOutput[MOTOR_LEFT] =  (MotorOutput[MOTOR_LEFT] + MotorOutputOld[MOTOR_LEFT] * (int16)MotorOutputLPMul) >> (int16)MotorOutputLPDiv;
        MotorOutput[MOTOR_BACK] = (MotorOutput[MOTOR_BACK] + MotorOutputOld[MOTOR_BACK] * (int16)MotorOutputLPMul) >> (int16)MotorOutputLPDiv;
        MotorOutput[MOTOR_RIGHT] = (MotorOutput[MOTOR_RIGHT] + MotorOutputOld[MOTOR_RIGHT] * (int16)MotorOutputLPMul) >> (int16)MotorOutputLPDiv; 
    
#ifndef DEBUG_MODE
        set_motor_output();
#endif // DEBUG_MODE
            
        // coleta sinal atual pra depois fazer o lowpass
        MotorOutputOld[MOTOR_FRONT] = MotorOutput[MOTOR_FRONT];
        MotorOutputOld[MOTOR_LEFT] = MotorOutput[MOTOR_LEFT];
        MotorOutputOld[MOTOR_BACK] = MotorOutput[MOTOR_BACK];
        MotorOutputOld[MOTOR_RIGHT] = MotorOutput[MOTOR_RIGHT];

        if(get_delay(DELAY_FLIGHT_TIME)){
            FlightTime++;
            set_delay(DELAY_FLIGHT_TIME, 500);
        }
    }
    else{
        ControlIntegralResult[ROLL_INDEX] = 0;
        ControlIntegralResult[YAW_INDEX] = 0;
        ControlIntegralResult[PITCH_INDEX] = 0;
        
        Angle[ROLL_INDEX] = 0;
        Angle[PITCH_INDEX] = 0;
        
        set_all_motors(MIN_MOTOR);
    }
}


// result 1 = fail
// result 0 = success
uchar save_params(void){
    
    return 0;
}

uchar load_params(void){

    return 0;
}
    
// tem q fazer isso configuravel
inline void adjust_readings(void){
    int16 IntAux;

    // tira offset    
    IntAux = AnalogValue[GYRO_ROLL_ACH] - AnalogOffset[GYRO_ROLL_ACH];
    GyroValue[ROLL_INDEX] = (GyroValue[ROLL_INDEX] * (int16)GyroLPMul + IntAux ) >> (int16)GyroLPDiv;
    
    IntAux = -AnalogValue[GYRO_PITCH_ACH] + AnalogOffset[GYRO_PITCH_ACH];
    GyroValue[PITCH_INDEX] = (GyroValue[PITCH_INDEX] * (int16)GyroLPMul + IntAux ) >> (int16)GyroLPDiv;
        
    IntAux = -AnalogValue[GYRO_YAW_ACH] + AnalogOffset[GYRO_YAW_ACH];
    GyroValue[YAW_INDEX] = (GyroValue[YAW_INDEX] * (int16)GyroLPMul + IntAux ) >> (int16)GyroLPDiv;

    // tira offset    
    IntAux = -AnalogValue[ACCELX_ACH] + AnalogOffset[ACCELX_ACH];
    IntAux = (IntAux * 5) >> 4;
    AccelValue[ROLL_INDEX] = (AccelValue[ROLL_INDEX] * (int16)AccelLPMul + IntAux ) >> (int16)AccelLPDiv;
    
    IntAux = -AnalogValue[ACCELY_ACH] + AnalogOffset[ACCELY_ACH];
    IntAux = (IntAux * 5) >> 4;
    AccelValue[PITCH_INDEX] = (AccelValue[PITCH_INDEX] * (int16)AccelLPMul + IntAux) >> (int16)AccelLPDiv;
        
    IntAux = AnalogValue[ACCELZ_ACH] - AnalogOffset[ACCELZ_ACH]; 
    IntAux = (IntAux * 5) >> 4;
    AccelValue[YAW_INDEX] = (AccelValue[YAW_INDEX] * (int16)AccelLPMul + IntAux) >> (int16)AccelLPDiv;
}

void init_vars(){
    uint16 i, j;

    // saturation-
    PitchRollPSaturation = PITCH_ROLL_P_SATURATION;
    // proporcional
    ControlProportionalMul[YAW_INDEX] = YAW_PROPORTIONAL_MUL;
    ControlProportionalMul[PITCH_INDEX] = PITCH_PROPORTIONAL_MUL;
    ControlProportionalMul[ROLL_INDEX] = ROLL_PROPORTIONAL_MUL;
    ControlProportionalDiv[YAW_INDEX] = YAW_PROPORTIONAL_DIV;  
    ControlProportionalDiv[PITCH_INDEX] = PITCH_PROPORTIONAL_DIV;
    ControlProportionalDiv[ROLL_INDEX] = ROLL_PROPORTIONAL_DIV; 
    // integral
    ControlIntegralDiv[YAW_INDEX] = YAW_INTEGRAL_DIV;  
    ControlIntegralDiv[PITCH_INDEX] = PITCH_INTEGRAL_DIV;
    ControlIntegralDiv[ROLL_INDEX] = ROLL_INTEGRAL_DIV; 
    
    ControlIntegralLimit[YAW_INDEX] = YAW_INTEGRAL_LIMIT;  
    ControlIntegralLimit[PITCH_INDEX] = PITCH_INTEGRAL_LIMIT;
    ControlIntegralLimit[ROLL_INDEX] = ROLL_INTEGRAL_LIMIT; 
            
    
    // filters
    ThrottleLPMul   = THROTTLE_LP_MUL      ;
    ThrottleLPDiv   = THROTTLE_LP_DIV      ;
    MotorOutputLPMul= MOTOR_OUTPUT_LP_MUL  ;
    MotorOutputLPDiv= MOTOR_OUTPUT_LP_DIV  ;
    GyroLPMul       = GYRO_LP_MUL          ;
    GyroLPDiv       = GYRO_LP_DIV          ;
    AccelLPMul      = ACCEL_LP_MUL         ;
    AccelLPDiv      = ACCEL_LP_DIV         ;
    
    // outras
    AnalogChecked.BYTE = 0x00;
    for(i = 0; i < 8; i++){
        for(j = 0; j < GRAPH_LENGHT; j++){
            AnalogGraph[i][j] = 90;
        }
    }
}

// sample rate 400Hz
interrupt (TIMERB1_VECTOR) TIMERB1_ISR_HOOK(void){ // 2.5ms
    ControlSample = 1;
    TBCTL &= ~0x01;
}

