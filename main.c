#include "stdio.h"
#include "stdlib.h"
#include "signal.h"
#include "msp430x261x.h"
#include "delay.h"
#include "analog.h"
#include "lcd6100.h"
#include "eeprom.h"
#include "i2c.h"
#include "microquad.h"

volatile unsigned int MenuCounter = 0;

char MenuVisible = FALSE;       // indica se o menu deve ser processado ou nao
char MotorArmed = FALSE;        // seguranca, o loop de controle so eh feito quando esta variavei for TRUE
char DrawRCInputCheck = FALSE;
char DrawControlRefCheck = FALSE;
char ShowSettingsCheck = FALSE;

MENU_STEPSET MenuStep = DISPLAY;        // gerencia o menu principal
MENU_STEPSET MenuAnalog = DISPLAY;      // gerencia os canais analog visiveis
MENU_STEPSET MenuPID = DISPLAY;
MENU_OPTION MenuOption = READY;         // guarda a opcao escolhida no menu principal
FLIGHT_MODE FlightMode = NORMAL;
PID_TUNING_OPT PIDMenuOption = CHANGE_P_ROLL;

// variaveis usadas no menu analog graph
char AnalogSelect = 7;      // serve para selecionar a entrada no menu analog
char AnalogChecked = 0x00;  // verifica se a entrada X deve ser mostrada no grafico
char AnalogGraph[8][GRAPH_LENGHT];   // um vetor para cada entrada analogica, guarda as ultimas GRAPH_LENGHT leituras, total = 800B
char AnalogColours[8] = {BLUE, LIME, RED, YELLOW, ORANGE, MAGENTA, AQUA, WHITE}; 

// variaveis ajuste radio - roll pitch e yaw
int PPMOffset[4] = {0, 0, 0, 0};

// controle - roll pitch e yaw
int Gain[3][3] = {{1, 1, 1}, {0, 0, 0}, {0, 0, 0}};
int ControlRef[4] = {0, 0, 0, 0};
int MotorOut[4] = {0, 0, 0, 0};
// var menu ajuste do controle
int StickCounter = 1;

// variaveis leitura radio raw
volatile int TimeUpEdge[8] = {0,0,0,0,0,0,0,0};            // 
volatile int PPMValue[8] = {0,0,0,0,0,0,0,0};

int main(){
    WDTCTL = WDTPW + WDTHOLD;   // desabilita watchdog

    INICIO: //loop main, vir para ca no caso de um reset
    setup();    // inicializa perifericos e verifica se precisa calibrar o radio

    lcd_clear(BLACK);
    
    // estados iniciais dos menus
    MenuOption = READY;
    MenuStep = DISPLAY;
    MenuVisible = TRUE;     // inicializa mostrando o menu
    
    while(1){
        
        // fazer um battery check cada 30s
        
        if(MotorArmed == TRUE){
            main_loop();
        }

        if(MenuVisible == TRUE){
            analog_refresh_all();
            process_menu();
        }
        else{
            if(PPMValue[CH6_CH] > STICK_UP_THRESHOLD){
                MenuVisible = TRUE;
                MotorArmed = FALSE;
            }
        }
    }
}

// loop de controle
void main_loop(){
    int i;
    // read channel
    rx_get_channels();
    // arm disarm nao tem
    // calibra gyro quando throttle < 1%
    if(Throttle < 2){
        for(i = 0; i<4; i++){
            MotorOut[i] = 0;
        }
        calibrate_gyro();
    }
    else{
        // read analog
        analog_refresh_channel(GYRO_YAW_CH);
        analog_refresh_channel(GYRO_PITCH_CH);
        analog_refresh_channel(GYRO_ROLL_CH);
        // adiciona throttle
        MotorOut[FRONT] = Throttle;
        MotorOut[LEFT] = Throttle;
        MotorOut[RIGHT] = Throttle;
        MotorOut[BACK] = Throttle;
        // adiciona gyro e rc roll
        AnalogValue[GYRO_ROLL_CH] = (AnalogValue[GYRO_ROLL_CH] >> 2) * Gain[PROPORTIONAL][ROLL] / 100; // passa a leitura do gyro para 10bits
        ControlRef[ROLL] = ControlRef[ROLL] * ROLL_RC_GAIN / 100;
        ControlRef[ROLL] += AnalogValue[GYRO_ROLL_CH]
        
        MotorOut[LEFT] += ControlRef[ROLL];
        MotorOut[RIGHT] -= ControlRef[ROLL];
        // adiciona pitch
        AnalogValue[GYRO_PITCH_CH] = (AnalogValue[GYRO_PITCH_CH] >> 2) * Gain[PROPORTIONAL][PITCH] / 100; // passa a leitura do gyro para 10bits
        ControlRef[PITCH] = ControlRef[PITCH] * PITCH_RC_GAIN / 100;
        ControlRef[PITCH] += AnalogValue[GYRO_PITCH_CH]
        
        MotorOut[BACK] += ControlRef[PITCH];
        MotorOut[FRONT] -= ControlRef[PITCH];
        // adiciona yaw
        AnalogValue[GYRO_YAW_CH] = (AnalogValue[GYRO_YAW_CH] >> 2) * Gain[PROPORTIONAL][YAW] / 100; // passa a leitura do gyro para 10bits
        ControlRef[YAW] = ControlRef[YAW] * YAW_RC_GAIN / 100;
        ControlRef[YAW] += AnalogValue[GYRO_YAW_CH]
        
        MotorOut[RIGHT] += ControlRef[YAW];
        MotorOut[LEFT] += ControlRef[YAW];
        MotorOut[BACK] -= ControlRef[YAW];
        MotorOut[FRONT] -= ControlRef[YAW];                
        // satura low para nao parar de girar
        for(i = 0; i<4; i++){
            if(MotorOut[i] < MIN_COMMAND){
                MotorOut[i] = MIN_COMMAND;
            }
        }
        // envia comando
        
    }
}

//--- Get and scale RX channel inputs ---
void rx_get_channels(void){
    ControlRef[ROLL] = (PPMValue[ROLL_CH] - PPMOffset[ROLL]) >> 4;
    ControlRef[YAW] = (PPMValue[YAW_CH] - PPMOffset[YAW]) >> 4;
    ControlRef[PITCH] = (PPMValue[PITCH_CH] - PPMOffset[PITCH]) >> 4;
    ControlRef[THROTTLE] = (PPMValue[THROTTLE_CH] - PPMOffset[THROTTLE]) >> 4;
}

void process_menu(void){
    int i;
    switch(MenuStep){
        case DISPLAY:
        
            DrawRCInputCheck = FALSE;
            DrawControlRefCheck = FALSE;
            ShowSettingsCheck = FALSE;
            
            lcd_goto(0,14);
            printf("DISPLAY             ");
            
            lcd_clear(BLACK);
            lcd_goto(0,0);
            printf("   MAIN MENU\n\n");
/*            READY,
            CHANGE_FLIGHT_MODE,
            RADIO_RAW = 0,
            ANALOG_MONITOR,
            CHANGE_PID,
            RE_CALIBRATE_GYRO,
            RE_CALIBRATE_RADIO,
            INITIAL_SETTINGS            */
            for(i=0; i < MENU_LENGHT; i++){
                printf("  ");
                printf(MenuOptionNames[i]);
                printf("\n");
            }
            MenuStep = SELECT;
            break;
        
        case SELECT: // fica neste step a maior parte do tempo
            lcd_goto(0,14);
            printf("SELECT %d       ", (int)MenuOption);
            
            lcd_goto(0,(int)MenuOption + 2);
            color_fore = RED;
            printf(">");
            color_fore = LIME;
            
            // movimento pra baixo e pra cima
            if(PPMValue[PITCH_CH] < STICK_DOWN_THRESHOLD){
                MenuStep = WAIT_PITCH_DOWN;
            }
            else{
                if(PPMValue[PITCH_CH] > STICK_UP_THRESHOLD){
                    MenuStep = WAIT_PITCH_UP;
                }
            }
            
            // select
            if(PPMValue[ROLL_CH] > STICK_UP_THRESHOLD){
                MenuStep = WAIT_ROLL_LEFT;
            }
            
            break;
        
        case WAIT_PITCH_DOWN:
            lcd_goto(0,14);
            printf("WAIT PITCH DOWN    ");
            
            if(PPMValue[PITCH_CH] > STICK_DOWN_THRESHOLD){
                lcd_goto(0,(int)MenuOption + 2);
                printf(" ");
                if((int)MenuOption > 0){
                    MenuOption = (MENU_OPTION)(MenuOption - 1);
                }
                MenuStep = SELECT;
            }
            break;
        
        case WAIT_PITCH_UP:
            lcd_goto(0,14);
            printf("WAIT PITCH UP    ");

            if(PPMValue[PITCH_CH] < STICK_UP_THRESHOLD){
                lcd_goto(0,(int)MenuOption + 2);
                printf(" ");
                if((int)MenuOption < MENU_LENGHT - 1){
                    MenuOption = (MENU_OPTION)(MenuOption + 1);
                }
                MenuStep = SELECT;
            }
            break;
        
        case WAIT_ROLL_LEFT:
            lcd_goto(0,14);
            printf("WAIT ROLL LEFT    ");
            
            if(PPMValue[ROLL_CH] < STICK_UP_THRESHOLD){
                lcd_clear(BLACK);
                lcd_goto(0,0);  
                MenuStep = PROCESS_OPTION;
            }
            break;
            
        case WAIT_CH7_BACK:
            lcd_goto(0,14);
            printf("WAIT CH7 BACK    ");
            
            if(PPMValue[CH7_CH] < STICK_DOWN_THRESHOLD){
                MenuStep = DISPLAY;
            }
            break;
                    
        case PROCESS_OPTION:
            process_option();
            break;
            
        case WAIT_ROLL_RIGHT:
            break;    
    }
}

void process_option(){
    int i;

    switch (MenuOption){
        case RADIO_RAW:
            draw_rc_inputs();
            break;
            
        case CHANGE_FLIGHT_MODE:
            MenuStep = DISPLAY;
            MenuVisible = FALSE;
            
            if(FlightMode == UFO){
                FlightMode = NORMAL;
            }
            else{
                FlightMode = (FLIGHT_MODE)((int)FlightMode + 1);
            }
            
            lcd_clear(BLACK);
            lcd_goto(0,5);
            printf("Flight mode Change to\n\n     ");
            printf(FlightModeNames[(int)FlightMode]);
            
            // Gravar na EEPROM o mode
            
            delayms(3000);
            MenuVisible = TRUE;
            
            break;
            
        case ANALOG_MONITOR:
            process_analog_menu();
            break;
        
        case CONTROL_REF:
            rx_get_channels();
            draw_control_ref();
            break;
        
        case READY:
            lcd_clear(BLACK);
            for(i = 5; i >= 0; i--){ // contagem regressiva
                lcd_goto(8,8);
                printf("%d", i);
                delayms(1000);
            }
            MotorArmed = TRUE;
            MenuVisible = FALSE;
            lcd_clear(BLACK);
            MenuStep = DISPLAY;
            break;
            
        case INITIAL_SETTINGS:
            MenuStep = DISPLAY;
            MenuVisible = FALSE;

            set_defaults();
                        
            lcd_clear(BLACK);
            calibrate_gyro();
            
            lcd_clear(BLACK);
            calibrate_radio();
                        
            lcd_clear(BLACK);
            lcd_goto(0,5);          
            printf("SETTINGS\nSET TO\nDEFAULTS");

            delayms(2500);
            MenuVisible = TRUE;
            // seta o flag initial setup
            break;          
        
        case RE_CALIBRATE_GYRO:
            MenuStep = DISPLAY;
            MenuVisible = FALSE;
            calibrate_gyro();
            MenuVisible = TRUE;
            break;

        case RE_CALIBRATE_RADIO:
            MenuStep = DISPLAY;
            MenuVisible = FALSE;
            calibrate_radio();
            MenuVisible = TRUE;
            break;
            
        case CHANGE_PID:
            process_PID_menu();
            break;
            
        case SHOW_SETTINGS:
            if(ShowSettingsCheck == FALSE){
                show_settings();
                ShowSettingsCheck = TRUE;
            }
            break;
            
        case SAVE_CURRENT_GAINS:
            save_current_gains();
            MenuStep = DISPLAY;
            break;
    }
    
    if(PPMValue[CH7_CH] > STICK_UP_THRESHOLD){
        MenuStep = WAIT_CH7_BACK;
        MenuAnalog = DISPLAY;   // serve para colocar o menu analog na posicao inicial para quando for acessado denovo
        MenuPID = DISPLAY;
        analog_graph_clear_all();
        lcd_clear(BLACK);
    }
}

void process_PID_menu(void){
    int i, j;
    switch(MenuPID){
        case DISPLAY:
            lcd_clear(BLACK);
        
            lcd_goto(0,14);
            printf("DISPLAY             ");
            
            lcd_goto(0,0);
            printf("   PID MENU\n\n");
/*            READY,
            CHANGE_FLIGHT_MODE,
            RADIO_RAW = 0,
            ANALOG_MONITOR,
            CHANGE_PID,
            RE_CALIBRATE_GYRO,
            RE_CALIBRATE_RADIO,
            INITIAL_SETTINGS            */
            for(i=0; i<9; i++){
                printf("  ");
                printf(PIDMenuOptionNames[i]);
                printf("\n");
            }
            
            MenuPID = SELECT;
            break;
        
        case SELECT: // fica neste step a maior parte do tempo
            lcd_goto(0,14);
            printf("SELECT %d       ", (int)PIDMenuOption);
            
            lcd_goto(16,2);
            for(i=0; i < 3; i++){
                for(j=0; j<3; j++){
                    lcd_goto(16, LCD_linecount);
                    printf("%d  \n",Gain[i][j]);
                }
            }
            
            lcd_goto(0,(int)PIDMenuOption + 2);
            color_fore = RED;
            printf(">");
            color_fore = LIME;
            
            // movimento pra baixo e pra cima
            if(PPMValue[PITCH_CH] < STICK_DOWN_THRESHOLD){
                MenuPID = WAIT_PITCH_DOWN;
            }
            else{
                if(PPMValue[PITCH_CH] > STICK_UP_THRESHOLD){
                    MenuPID = WAIT_PITCH_UP;
                }
            }
            
            // select
            if(PPMValue[ROLL_CH] > STICK_UP_THRESHOLD){
                MenuPID = WAIT_ROLL_LEFT;
            }
            else{
                if(PPMValue[ROLL_CH] < STICK_DOWN_THRESHOLD){
                    MenuPID = WAIT_ROLL_RIGHT;
                }
            }            
            
            break;
        
        case WAIT_PITCH_DOWN:
            lcd_goto(0,14);
            printf("WAIT PITCH DOWN    ");
            
            if(PPMValue[PITCH_CH] > STICK_DOWN_THRESHOLD){
                lcd_goto(0,(int)PIDMenuOption + 2);
                printf(" ");
                if((int)PIDMenuOption > 0){
                    PIDMenuOption = (MENU_OPTION)(PIDMenuOption - 1);
                }
                MenuPID = SELECT;
            }
            break;
        
        case WAIT_PITCH_UP:
            lcd_goto(0,14);
            printf("WAIT PITCH UP    ");

            if(PPMValue[PITCH_CH] < STICK_UP_THRESHOLD){
                lcd_goto(0,(int)PIDMenuOption + 2);
                printf(" ");
                if((int)PIDMenuOption < 9 - 1){
                    PIDMenuOption = (MENU_OPTION)(PIDMenuOption + 1);
                }
                MenuPID = SELECT;
            }
            break;
        
        case WAIT_ROLL_RIGHT:
            lcd_goto(0,14);
            printf("WAIT ROLL LEFT    ");
            
            delayms(300 - StickCounter);
            if(StickCounter < 256){
                StickCounter = (StickCounter * 4);
            }
            
            if(Gain[(int)PIDMenuOption >> 4][(int)PIDMenuOption & 0x0F] > 0){
                Gain[(int)PIDMenuOption >> 4][(int)PIDMenuOption & 0x0F]--;                
            }
            
            if(PPMValue[ROLL_CH] > STICK_DOWN_THRESHOLD){
                
                StickCounter = 1;
            }
            MenuPID = SELECT;
            break;
            
        case WAIT_ROLL_LEFT:
            lcd_goto(0,14);
            printf("WAIT ROLL LEFT    ");
            
            delayms(300 - StickCounter);
            if(StickCounter < 256){
                StickCounter = (StickCounter * 4);
            }
            
            Gain[(int)PIDMenuOption >> 4][(int)PIDMenuOption & 0x0F]++;
            
            if(PPMValue[ROLL_CH] < STICK_UP_THRESHOLD){
                StickCounter = 1;
            }
            MenuPID = SELECT;
            break;
            
        case WAIT_CH7_BACK:
            lcd_goto(0,14);
            printf("WAIT CH7 BACK    ");
            
            if(PPMValue[CH7_CH] < STICK_DOWN_THRESHOLD){
                MenuPID = DISPLAY;
            }
            break;
                    
        case PROCESS_OPTION:
            break;
               
    }
}

// juntar os pontos
void draw_analog_graph(void){
    int i, k, j, y;
    for(i = 0; i < 8; i++){
        if((1 << i) & AnalogChecked){
            // apaga
            for(k = 0; k < (GRAPH_LENGHT - 1); k++){
                lcd_drawpoint(k+GRAPH_OFFSETX, AnalogGraph[i][k] ,BLACK);
                j = AnalogGraph[i][k]; // posicao atual
                y = AnalogGraph[i][k+1]; // proxima posicao
                if(j != y){ // se agora ta mais alto q depois
                    if(j < y){
                        y--; 
                        while(j++ < y){
                            lcd_drawpoint(k+GRAPH_OFFSETX, j ,BLACK);
                        }
                    }
                    else{
                        y++;
                        while(j-- > y){
                            lcd_drawpoint(k+GRAPH_OFFSETX, j ,BLACK);
                        }
                    }
                }
                AnalogGraph[i][k] = AnalogGraph[i][k+1];
            } 
            // apaga ultimo
            lcd_drawpoint(GRAPH_LENGHT - 1 + GRAPH_OFFSETX, AnalogGraph[i][GRAPH_LENGHT - 1] ,BLACK);
            
            /* shifta
            for(k = 0; k < (GRAPH_LENGHT - 1); k++){
                AnalogGraph[i][k] = AnalogGraph[i][k+1];
            }*/
            
            // insere mais um valor
            AnalogGraph[i][GRAPH_LENGHT - 1] = GRAPH_OFFSET - ((AnalogValue[i] >> 6) & 0xFF);
            
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

void show_settings(void){
    lcd_clear(BLACK);
    lcd_goto(4,0);
    printf("SETTINGS\n\n");
    
    printf("P Pitch");
    lcd_putdot(LCD_charcount, 16);
    printf("%d\n", Gain[PROPORTIONAL][PITCH]);
    
    printf("P Roll");
    lcd_putdot(LCD_charcount, 16);
    printf("%d\n", Gain[PROPORTIONAL][ROLL]);
    
    printf("P Yaw");
    lcd_putdot(LCD_charcount, 16);
    printf("%d\n", Gain[PROPORTIONAL][YAW]);
    
    printf("D Pitch");
    lcd_putdot(LCD_charcount, 16);
    printf("%d\n", Gain[DERIVATIVE][PITCH]);
    
    printf("D Roll");
    lcd_putdot(LCD_charcount, 16);
    printf("%d\n", Gain[DERIVATIVE][ROLL]);
    
    printf("D Yaw");
    lcd_putdot(LCD_charcount, 16);
    printf("%d\n", Gain[DERIVATIVE][YAW]);
    
    printf("I Pitch");
    lcd_putdot(LCD_charcount, 16);
    printf("%d\n", Gain[INTEGRAL][PITCH]);
    
    printf("I Roll");
    lcd_putdot(LCD_charcount, 16);
    printf("%d\n", Gain[INTEGRAL][ROLL]);
    
    printf("I Yaw");
    lcd_putdot(LCD_charcount, 16);
    printf("%d\n", Gain[INTEGRAL][YAW]);
        
    printf("Pitch Offset");
    lcd_putdot(LCD_charcount, 16);
    printf("%d\n", PPMOffset[PITCH]);
    
    printf("Roll Offset");
    lcd_putdot(LCD_charcount, 16);
    printf("%d\n", PPMOffset[ROLL]);
    
    printf("Yaw Offset");
    lcd_putdot(LCD_charcount, 16);
    printf("%d\n", PPMOffset[YAW]);
    
    printf("Throt. Offset");
    lcd_putdot(LCD_charcount, 16);
    printf("%d\n", PPMOffset[THROTTLE]);
    
}

// arrumar
void analog_graph_clear(int i){
    int k, j, y;
    for(k = 0; k < (GRAPH_LENGHT - 1); k++){
        lcd_drawpoint(k+GRAPH_OFFSETX, AnalogGraph[i][k] ,BLACK);
        j = AnalogGraph[i][k]; // posicao atual
        y = AnalogGraph[i][k+1]; // proxima posicao
        if(j != y){ // se agora ta mais alto q depois
            if(j < y){
                y--; 
                while(j++ < y){
                    lcd_drawpoint(k+GRAPH_OFFSETX, j ,BLACK);
                }
            }
            else{
                y++;
                while(j-- > y){
                    lcd_drawpoint(k+GRAPH_OFFSETX, j ,BLACK);
                }
            }
        }
        //AnalogGraph[i][k] = AnalogGraph[i][k+1];
    } 
    // apaga ultimo
    lcd_drawpoint(GRAPH_LENGHT - 1 + GRAPH_OFFSETX, AnalogGraph[i][GRAPH_LENGHT - 1] ,BLACK);
}

void process_analog_menu(void){
    draw_analog_graph();
    switch(MenuAnalog){
        case DISPLAY:
            lcd_goto(4,0);
            printf("ANALOG GRAPH   ");
            
            lcd_goto(0,14);
            printf("DISPLAY             ");

            refresh_analog_menu();
            MenuAnalog = SELECT;
            break;
            
        case SELECT:
            lcd_goto(0,14);
            printf("SELECT            ");

            // move para os lados
            if(PPMValue[ROLL_CH] > STICK_UP_THRESHOLD){
                MenuAnalog = WAIT_ROLL_LEFT;
            }
            else{
                if(PPMValue[ROLL_CH] < STICK_DOWN_THRESHOLD){
                    MenuAnalog = WAIT_ROLL_RIGHT;
                }
            }
            
            // check uncheck
            if(PPMValue[PITCH_CH] < STICK_DOWN_THRESHOLD){
                MenuAnalog = WAIT_PITCH_DOWN;
            }
            else{
                if(PPMValue[PITCH_CH] > STICK_UP_THRESHOLD){
                    MenuAnalog = WAIT_PITCH_UP;
                }
            }
            
            break;
            
        case WAIT_PITCH_DOWN:
            lcd_goto(0,14);
            printf("WAIT PITCH DOWN    ");

            if(PPMValue[PITCH_CH] > STICK_DOWN_THRESHOLD){
                AnalogChecked = AnalogChecked^(1<<AnalogSelect);
                MenuAnalog = DISPLAY;
            }
            break;
            
        case WAIT_PITCH_UP:
            lcd_goto(0,14);
            printf("WAIT PITCH UP    ");

            if(PPMValue[PITCH_CH] < STICK_UP_THRESHOLD){
                AnalogChecked = AnalogChecked^(1<<AnalogSelect);
                analog_graph_clear(AnalogSelect);
                MenuAnalog = DISPLAY;
            }
            break;
            
        case WAIT_ROLL_RIGHT:
                lcd_goto(0,14);
                printf("WAIT ROLL RIGHT ");

                if(PPMValue[ROLL_CH] > STICK_DOWN_THRESHOLD){
                    if(AnalogSelect > 0){
                        AnalogSelect--;
                    }
                    MenuAnalog = DISPLAY;
                }
            break;
            
        case WAIT_ROLL_LEFT:
                lcd_goto(0,14);
                printf("WAIT ROLL LEFT   ");
        
                if(PPMValue[ROLL_CH] < STICK_UP_THRESHOLD){
                   if(AnalogSelect < 7){
                        AnalogSelect++;
                    }
                    MenuAnalog = DISPLAY;

                }
            break;
            
        case PROCESS_OPTION:
            break;
            
        case WAIT_CH7_BACK:
            break;
    }
}

// ok
void refresh_analog_menu(void){
    int i;
    lcd_goto(1,12);
    for(i = 0; i < 8; i++){             // para as 8 entradas analogicas
        if((1 << i) & AnalogChecked){   // se a entrada analogica estiver visible
            color_back = LIME;          // a cor do fundo muda para azul
            color_fore = BLACK;
        }
        else{                           // senao
            color_back = BLACK;         // a cor do fundo muda para preto
            color_fore = LIME;
        }
        
        if(AnalogSelect == i){          // se o cursor estiver em cima   
            color_fore = RED;           // a cor da letra muda para vermelho 
        }                                                                
        else{                           // se nao                        
            if(color_back == LIME){
                color_fore = BLACK;
            }
        }                               
        
        printf("%d",i);
        // ajusta as cores para o normal
        color_fore = LIME;
        color_back = BLACK;
        printf(" ");
    }
    printf("                    ");
}

interrupt (TIMERA0_VECTOR) TIMERA0_ISR_HOOK(void){
    // ta livre
}

// ok
void clock_init(void)
{
    /* 
     * Basic Clock System Control 2
     * 
     * SELM_0 -- DCOCLK
     * DIVM_0 -- Divide by 1
     * ~SELS -- DCOCLK
     * DIVS_0 -- Divide by 1
     * ~DCOR -- DCO uses internal resistor
     * 
     * Note: ~<BIT> indicates that <BIT> has value zero
     */
    BCSCTL2 = SELM_0 + DIVM_0 + DIVS_0;
    
    DCOCTL = 0x77;      
        /* 
     * Basic Clock System Control 1
     * 
     * XT2OFF -- Disable XT2CLK
     * XTS -- High Frequency
     * DIVA_0 -- Divide by 1
     */
    BCSCTL1 = 0xcf;     

    /* 
     * Basic Clock System Control 3
     * 
     * XT2S_3 -- Digital input signal
     * LFXT1S_2 -- If XTS = 0, XT1 = VLOCLK ; If XTS = 1, XT1 = 3 - 16-MHz crystal or resonator
     * XCAP_0 -- ~1 pF
     */
    BCSCTL3 = 0xa0;
}

// ok
void p1_init(){
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

void timer_a3_init(void)
{
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
    TACCTL0 = CM_0 + CCIS_0 + OUTMOD_0 + CCIE;

    /* TACCR0, Timer_A Capture/Compare Register 0 */
    TACCR0 = 4001; //2ms

    /* 
     * TACTL, Timer_A3 Control Register
     * 
     * TASSEL_2 -- SMCLK
     * ID_3 -- Divider - /8
     * MC_1 -- Up Mode
     */
    TACTL = TASSEL_2 + ID_3 + MC_1;
}

// consagred
interrupt (PORT1_VECTOR) PORT1_ISR_HOOK(void){
    unsigned int PPM_aux = TAR; // captura aqui pra ser mais exato
    // aqui eh usado PPM_P1_MASK, pq eh a interrupcao da P1 
    int channel_num = 0, PPM_ch_counter;
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

// consagred
void calibrate_gyro(void){
    int i, k;
    //samples
    unsigned int AnalogMean[3][15]; //15 * 15 samples = 225 
    
    lcd_clear(BLACK);
    lcd_goto(2,1);
    printf("CALIBRATE GYRO\n");
    
    AnalogOffset[GYRO_ROLL] = 0;
    AnalogOffset[GYRO_PITCH] = 0;
    AnalogOffset[GYRO_YAW] = 0;
    
    for(i = 0; i < 3; i++){
        for(k = 0; k < 15; k++){
            AnalogMean[i][k] = 0;
        }
    }
    
    for(i = 0; i < 15; i++){
        for(k = 0; k < 15; k++){
            delayms(10);
            lcd_goto(15,7);
            printf("%d  ",i*k);
            analog_refresh_all();
            AnalogMean[GYRO_ROLL][i] += AnalogValue[GYRO_ROLL];
            AnalogMean[GYRO_PITCH][i] += AnalogValue[GYRO_PITCH];
            AnalogMean[GYRO_YAW][i] += AnalogValue[GYRO_YAW];
        }
        AnalogMean[GYRO_ROLL][i] = AnalogMean[GYRO_ROLL][i] / 15;
        AnalogMean[GYRO_PITCH][i] = AnalogMean[GYRO_PITCH][i] / 15;
        AnalogMean[GYRO_YAW][i] = AnalogMean[GYRO_YAW][i] / 15;
    }
    
    for(i = 0; i < 15; i++){
        AnalogOffset[GYRO_YAW] += AnalogMean[GYRO_YAW][i];
        AnalogOffset[GYRO_ROLL] += AnalogMean[GYRO_ROLL][i];
        AnalogOffset[GYRO_PITCH] += AnalogMean[GYRO_PITCH][i];
    }
    AnalogOffset[GYRO_YAW] = AnalogOffset[GYRO_YAW] / 15;
    AnalogOffset[GYRO_PITCH] = AnalogOffset[GYRO_PITCH] / 15;
    AnalogOffset[GYRO_ROLL] = AnalogOffset[GYRO_ROLL] / 15;

    lcd_goto(2,8);
    printf("COMPLETE\n\n    PITCH: %d\n    ROLL: %d\n    YAW: %d   ", AnalogOffset[GYRO_PITCH], AnalogOffset[GYRO_ROLL], AnalogOffset[GYRO_YAW]);
    
    delayms(1000);
}

// consagred
void calibrate_radio(void){
    
    int k;
    
    if(MotorArmed == 1 ){ // nao faz se os motores estiverem armados
        return;
    }
    
    PPMOffset[YAW] = 0;
    PPMOffset[ROLL] = 0;
    PPMOffset[PITCH] = 0;
    PPMOffset[THROTTLE] = 0;        
    
    for(k = 5; k > 0; k--){
        draw_rc_inputs();
        lcd_goto(0, 14);
        printf("%d ", k);
        delayms(500);
        
        // amostra
        PPMOffset[YAW] += PPMValue[YAW_CH];
        PPMOffset[ROLL] += PPMValue[ROLL_CH];
        PPMOffset[PITCH] += PPMValue[PITCH_CH];
        PPMOffset[THROTTLE] += PPMValue[THROTTLE_CH];
    }
    
    PPMOffset[YAW] /= 5;
    PPMOffset[ROLL] /= 5;
    PPMOffset[PITCH] /= 5;
    PPMOffset[THROTTLE] /= 5;
    
    lcd_clear(BLACK);
    /*
    // grava dados na EEPROM
    if(EEPROMFound){
        i2c_change_address(EEPROM_I2C_ADDR);
        
        delayms(100);
        
        // grava yaw offset 
        ValorAux.i = PPMOffset[YAW];
        i2c_write16_multiples(_RADIO_YAW_OFFSET, ValorAux.c, 2);
        
        delayms(100);
        
        // grava pitch offset 
        ValorAux.i = PPMOffset[PITCH];
        i2c_write16_multiples(_RADIO_PITCH_OFFSET, ValorAux.c, 2);
        
        delayms(100);
        
        // grava roll offset 
        ValorAux.i = PPMOffset[ROLL];
        i2c_write16_multiples(_RADIO_ROLL_OFFSET, ValorAux.c, 2);
        
        delayms(100);
        
        // grava roll offset 
        ValorAux.i = PPMOffset[THROTTLE];
        i2c_write16_multiples(_RADIO_THROTTLE_OFFSET, ValorAux.c, 2);
        
        delayms(100);
        
        
    }*/
    lcd_goto(2,1);
    printf(" OFFSETS\n\n ROLL: %d\n PITCH: %d\n YAW: %d\n THROTTLE: %d\n\n", PPMOffset[ROLL], PPMOffset[PITCH], PPMOffset[YAW], PPMOffset[THROTTLE]);
    delayms(1000);
}

void load_eeprom_values(void){
    twobytes ValorAux;

    i2c_change_address(EEPROM_I2C_ADDR);    

    //-------------- PROPORTIONAL
    
    i2c_read16_multiples(_ROLL_P, ValorAux.c, 2);
    Gain[PROPORTIONAL][ROLL] = ValorAux.i;                        
    
    delayms(100);
    
    i2c_read16_multiples(_PITCH_P, ValorAux.c, 2);
    Gain[PROPORTIONAL][PITCH] = ValorAux.i;
    
    delayms(100);
    
    i2c_read16_multiples(_YAW_P, ValorAux.c, 2);
    Gain[PROPORTIONAL][YAW] = ValorAux.i;
    
    delayms(100);
    
    //------------------ derivative
       
        i2c_read16_multiples(_ROLL_D, ValorAux.c, 2);
    Gain[DERIVATIVE][ROLL] = ValorAux.i;                        
    
    delayms(100);
    
    i2c_read16_multiples(_PITCH_D, ValorAux.c, 2);
    Gain[DERIVATIVE][PITCH] = ValorAux.i;
    
    delayms(100);
    
    i2c_read16_multiples(_YAW_D, ValorAux.c, 2);
    Gain[DERIVATIVE][YAW] = ValorAux.i;
    
    delayms(100);
    
    //--------------------- INTEGRAL
    
    i2c_read16_multiples(_ROLL_I, ValorAux.c, 2);
    Gain[INTEGRAL][ROLL] = ValorAux.i;
    
    delayms(100);
    
    i2c_read16_multiples(_PITCH_I, ValorAux.c, 2);
    Gain[INTEGRAL][PITCH] = ValorAux.i;
    
    delayms(100);
    
    i2c_read16_multiples(_YAW_I, ValorAux.c, 2);
    Gain[INTEGRAL][YAW] = ValorAux.i;
    
    delayms(100);
    
    printf("PARAMS LOADED\n");
}

void save_current_gains(void){
    twobytes ValorAux;
    
    lcd_clear(BLACK);
    lcd_goto(5,8);
    
    if(EEPROMFound){
        //proportional
        ValorAux.i = Gain[PROPORTIONAL][PITCH];
        i2c_write16_multiples(_PITCH_P, ValorAux.c, 2);
        delayms(100);   
        
        ValorAux.i = Gain[PROPORTIONAL][ROLL];
        i2c_write16_multiples(_ROLL_P, ValorAux.c, 2);
        delayms(100);
        
        ValorAux.i = Gain[PROPORTIONAL][YAW];
        i2c_write16_multiples(_YAW_P, ValorAux.c, 2);
        delayms(100);
        //derivative
        ValorAux.i = Gain[DERIVATIVE][PITCH];
        i2c_write16_multiples(_PITCH_D, ValorAux.c, 2);
        delayms(100);
        
        ValorAux.i = Gain[DERIVATIVE][ROLL];
        i2c_write16_multiples(_ROLL_D, ValorAux.c, 2);
        delayms(100);
        
        ValorAux.i = Gain[DERIVATIVE][YAW];
        i2c_write16_multiples(_YAW_D, ValorAux.c, 2);
        delayms(100);
        //integral
        ValorAux.i = Gain[INTEGRAL][PITCH];
        i2c_write16_multiples(_PITCH_I, ValorAux.c, 2);
        delayms(100);
        
        ValorAux.i = Gain[INTEGRAL][ROLL];
        i2c_write16_multiples(_ROLL_I, ValorAux.c, 2);
        delayms(100);
        
        ValorAux.i = Gain[INTEGRAL][YAW];
        i2c_write16_multiples(_YAW_I, ValorAux.c, 2);
        delayms(100);
    
        printf("Params. saved");
    }
    else{
        printf("Params. not saved");
    }
    
    delayms(1000);
    lcd_clear(BLACK);
    
}

void draw_control_ref(void){
    int i = 0, k = 0, h = 0;
    //250 max 120 min
    if(DrawControlRefCheck == FALSE){
        lcd_clear(BLACK);
        lcd_goto(0,0);
        printf(" Control Reference ");
        lcd_goto(0,2);
        printf(" PITCH:\n ROLL:\n YAW:\n THROTTLE:"); // conferir a ordem
        DrawControlRefCheck = TRUE;
    }
    for(i = 0; i < 4; i++){
       
        lcd_goto(11,i+2);
        printf("%d  ", ControlRef[i]);
       
        k = 21 + (8 * i); //posicao inicial em y
        
        if(ControlRef[i] > 130){
            h=100;
        }
        else{
            if(i == 3){
                h = (ControlRef[i]) * 10 / 13;
            }
            else{
                h = (ControlRef[i] + 60) * 10 / 11;
            }
        }
        
        lcd_drawprogressbar(90,k,35,4,WHITE, BLUE, h);
    }
}

// consagred
void draw_rc_inputs(){
    int i = 0, k = 0, h = 0;
    if(DrawRCInputCheck == FALSE){
        lcd_goto(0,0);
        printf("     RC inputs");
        lcd_goto(0,2);
        for(i = 0; i < 8; i++){
            printf("ch[%d]:\n", i);
        }
        DrawRCInputCheck = TRUE;
    }
    
    for(i = 0; i < 8; i++){
        lcd_goto(7,i+2);
        printf("%d  ", PPMValue[i]);
  
        k = 21 + (8 * i); //posicao inicial em y
        
        if(PPMValue[i] > 3750){
            h=100;
        }
        else{
            if(PPMValue[i] < 1751){
                h = 0;
            }
            else{
                h = ((PPMValue[i] - 1750))/ 21;
            }
        }
        
        lcd_drawprogressbar(80,k,45,4,WHITE, BLUE, h);
    }    
}

// consagred
int find_first(int startaddress){
    for(; startaddress < 255; startaddress++){
        i2c_config(startaddress);
        if(!i2c_find_device()){
            return startaddress;
        }
    }   
    return 0;
}

void setup(){
    int i, j;
    
    clock_init();
    
    timer_a3_init();
    
    p1_init();
    
    analog_init();
    
    i2c_init();
    
    eint();
    
    lcd_init(BLACK);
    
    for(i = 0; i < 8; i++){
        for(j = 0; j<100; j++){
            AnalogGraph[i][j] = GRAPH_OFFSET;
        }
    }
    
    //printf("EEPROM ADDRESS: %dd\n", find_first(0)); // essa funcao disconfigura alguma coisa da i2c

    calibrate_radio();
    calibrate_gyro();
    
    lcd_clear(BLACK);
    lcd_goto(0,0);
    
    i2c_config(EEPROM_I2C_ADDR);
    EEPROMFound = !(i2c_find_device());
    
    //if(0){
    if(EEPROMFound){
        printf("EEPROM found\n");
        
        load_eeprom_values();

    }
    else{
        color_fore = RED;
        printf("EEPROM not found\n");
        color_fore = LIME;
        
        set_defaults();
    }
    analog_graph_clear_all();    
    
    // faz um x vermelho e azul para testar display e draw line
    lcd_drawline(10,80,90,90,RED);
    lcd_drawline(10,90,90,80,BLUE);
    lcd_drawline(45,85,45,50,BLUE);
    
    delayms(1000);
}

void analog_graph_clear_all(void){
    int i;
    for(i = 0; i < 8; i++){
        analog_graph_clear(i);
    }
}

void set_defaults(void){

    Gain[PROPORTIONAL][PITCH] = PITCH_P_DEFAULT;
    Gain[PROPORTIONAL][ROLL] = ROLL_P_DEFAULT;
    Gain[PROPORTIONAL][YAW] = YAW_P_DEFAULT;
    
    Gain[DERIVATIVE][PITCH] = PITCH_D_DEFAULT;
    Gain[DERIVATIVE][ROLL] = ROLL_D_DEFAULT;
    Gain[DERIVATIVE][YAW] = YAW_D_DEFAULT;
    
    Gain[INTEGRAL][PITCH] = PITCH_I_DEFAULT;
    Gain[INTEGRAL][ROLL] = ROLL_I_DEFAULT;
    Gain[INTEGRAL][YAW] = YAW_I_DEFAULT;
    
    save_current_gains();
}
