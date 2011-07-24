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

volatile unsigned int ControlCounter = 0;
volatile unsigned int RCCounter = 0;
volatile unsigned int MenuCounter = 0;
/*
    StatusFlags:
    ----------------------------------
    |FirstSetup | Reserved | Reserved|
    ----------------------------------*/
char StatusFlags[3] = {0, 0, 0};// ficam guardados na EEPROM
char MenuVisible = FALSE;       // indica se o menu deve ser processado ou nao
char SetupDone = FALSE;         // fica true quando acaba o setup, serve para saber se ja pode comecar a ler o radio
char MotorArmed = FALSE;        // seguranca, o loop de controle so eh feito quando esta variavei for TRUE

MENU_STEPSET MenuStep = DISPLAY;        // gerencia o menu principal
MENU_STEPSET MenuAnalog = DISPLAY;      // gerencia os canais analog visiveis
MENU_OPTION MenuOption = READY;         // guarda a opcao escolhida no menu principal

// variaveis usadas no menu analog graph
char AnalogSelect = 7;      // serve para selecionar a entrada no menu analog
char AnalogChecked = 0x00;  // verifica se a entrada X deve ser mostrada no grafico
char AnalogGraph[8][100];   // um vetor para cada entrada analogica, guarda as ultimas 100 leituras, total = 800B
char AnalogColours[8] = {BLUE, LIME, RED, YELLOW, ORANGE, MAGENTA, AQUA, WHITE}; 

// variaveis ajuste radio
float PPMSlope[8] = {1, 1, 1, 1, 1, 1, 1, 1};   // leitura do radio: leitura_ajustada = slope * leitura_real + offset
float PPMOffset[8] = {0, 0, 0, 0, 0, 0, 0, 0};  // ver funcao process_rc() calibrate_radio()

// variaveis leitura radio raw
volatile unsigned int TimeUpEdge[8] = {0,0,0,0,0,0,0,0};            // 
volatile unsigned int PPMValue[8] = {0,0,0,0,0,0,0,0};
volatile unsigned int ChannelInput[8] = {0, 0, 0, 0, 0, 0, 0, 0};

int main(){
    WDTCTL = WDTPW + WDTHOLD;   // desabilita watchdog

    INICIO: //loop main, vir para ca no caso de um reset
    setup();    // inicializa perifericos e verifica se precisa calibrar o radio

    lcd_clear(BLACK);
    
    MenuOption = RADIO_RAW;
    MenuStep = DISPLAY;
    MenuVisible = TRUE;     // inicializa mostrando o menu
        
    while(1){
        /* os Counters sao incrementados na interrution do timerA
        esta parte do codigo nao esta dentro da interruption do timerA porque ela eh muito extensa
        e a prioridade da interrupcao do timerA eh uma das mais altas, fazendo com que as interruption
        da P1 e P2 fiquem esperando. Isso prejudica a leitura do radio. Se a prioridade da P1 e P2 for
        maior que a do timerA todo o codigo a seguir poderia estar la dentro da interruption do timerA.
        Outra maneira eh delegar a leitura dos PPM para outro MSP, algum bem pequeno, so precisa de uma
        P1 completa. */
        if(MotorArmed == TRUE){
            if(ControlCounter >= CONTROL_PERIOD){
                main_loop();
                ControlCounter = 0;
            }
        }
        
        if(SetupDone == TRUE){
            if(RCCounter >= RC_PERIOD){
                process_rc();
                RCCounter = 0;
            }
        }
    
        if(MenuVisible == TRUE){
            if(MenuCounter >= MENU_PERIOD){
                process_menu();
                MenuCounter = 0;   
            }
        }
        else{
            if(ChannelInput[CH6_CH] > 3500){
                MenuVisible = TRUE;
                MotorArmed = FALSE;
            }
        }
    }
}

void process_menu(void){
    switch(MenuStep){
        case DISPLAY:
            lcd_goto(0,14);
            printf("DISPLAY             ");
            
            lcd_clear(BLACK);
            lcd_goto(0,0);
            printf("   MAIN MENU\n\n");
            printf("  Radio raw\n  Radio correct\n  Analog Monitor\n  Ready\n  Set init settings");
            MenuStep = SELECT;
            break;
        
        case SELECT: // fica neste step a maior parte do tempo
            lcd_goto(0,14);
            printf("SELECT            ");
            
            lcd_goto(0,(int)MenuOption + 2);
            printf(">");
            
            // movimento pra baixo e pra cima
            if(ChannelInput[PITCH_CH] < 2500){
                MenuStep = WAIT_PITCH_DOWN;
            }
            else{
                if(ChannelInput[PITCH_CH] > 3500){
                    MenuStep = WAIT_PITCH_UP;
                }
            }
            
            // select
            if(ChannelInput[ROLL_CH] > 3500){
                MenuStep = WAIT_ROLL_LEFT;
            }
            
            break;
        
        case WAIT_PITCH_DOWN:
            lcd_goto(0,14);
            printf("WAIT PITCH DOWN    ");
            
            if(ChannelInput[PITCH_CH] > 2500){
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

            if(ChannelInput[PITCH_CH] < 3500){
                lcd_goto(0,(int)MenuOption + 2);
                printf(" ");
                if((int)MenuOption < MENU_LENGHT){
                    MenuOption = (MENU_OPTION)(MenuOption + 1);
                }
                MenuStep = SELECT;
            }
            break;
        
        case WAIT_ROLL_LEFT:
            lcd_goto(0,14);
            printf("WAIT ROLL LEFT    ");
            
            if(ChannelInput[ROLL_CH] < 3500){
                lcd_clear(BLACK);
                lcd_goto(0,0);  
                MenuStep = PROCESS_OPTION;
            }
            break;
            
        case WAIT_CH7_BACK:
            lcd_goto(0,14);
            printf("WAIT CH7 BACK    ");
            
            if(ChannelInput[CH7_CH] < 2500){
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
            draw_rc_inputs(1);
            break;
        case RADIO_CORRECT:
            draw_rc_inputs(0);
            break;
        case ANALOG_MONITOR:
            process_analog_menu();
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
            
            // seta o flag initial setup
            break;          
    }
    
    if(ChannelInput[CH7_CH] > 3500){
        MenuStep = WAIT_CH7_BACK;
        MenuAnalog = DISPLAY;   // serve para colocar o menu analog na posicao inicial para quando for acessado denovo
        lcd_clear(BLACK);
    }
}

void draw_analog_graph(void){
    lcd_goto(4,4);
    printf("FAZER");
}

void process_analog_menu(void){
    switch(MenuAnalog){
        case DISPLAY:
            lcd_goto(0,14);
            printf("DISPLAY             ");

            refresh_analog_menu();
            MenuAnalog = SELECT;
            break;
        case SELECT:
            lcd_goto(0,14);
            printf("SELECT            ");

            // move para os lados
            if(ChannelInput[ROLL_CH] > 3500){
                MenuAnalog = WAIT_ROLL_LEFT;
            }
            else{
                if(ChannelInput[ROLL_CH] < 2500){
                    MenuAnalog = WAIT_ROLL_RIGHT;
                }
            }
            
            // check uncheck
            if(ChannelInput[PITCH_CH] < 2500){
                MenuAnalog = WAIT_PITCH_DOWN;
            }
            else{
                if(ChannelInput[PITCH_CH] > 3500){
                    MenuAnalog = WAIT_PITCH_UP;
                }
            }
            
            break;
        case WAIT_PITCH_DOWN:
            lcd_goto(0,14);
            printf("WAIT PITCH DOWN    ");

            if(ChannelInput[PITCH_CH] > 2500){
                AnalogChecked = AnalogChecked^(1<<AnalogSelect);
                MenuAnalog = DISPLAY;
            }
            break;
            
        case WAIT_PITCH_UP:
            lcd_goto(0,14);
            printf("WAIT PITCH UP    ");

            if(ChannelInput[PITCH_CH] < 3500){
                AnalogChecked = AnalogChecked^(1<<AnalogSelect);
                MenuAnalog = DISPLAY;
            }
            break;
            
        case WAIT_ROLL_RIGHT:
                lcd_goto(0,14);
                printf("WAIT ROLL RIGHT ");

                if(ChannelInput[PITCH_CH] < 3500){
                    if(AnalogSelect > 0){
                        AnalogSelect--;
                    }
                    MenuAnalog = DISPLAY;
                }
            break;
            
        case WAIT_ROLL_LEFT:
                lcd_goto(0,14);
                printf("WAIT ROLL LEFT   ");
        
                if(ChannelInput[PITCH_CH] > 2500){
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
    ControlCounter++;
    RCCounter++;
    MenuCounter++;
}

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
    TACCR0 = 4001;

    /* 
     * TACTL, Timer_A3 Control Register
     * 
     * TASSEL_2 -- SMCLK
     * ID_3 -- Divider - /8
     * MC_1 -- Up Mode
     */
    TACTL = TASSEL_2 + ID_3 + MC_1;
}

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
                    PPMValue[channel_num] = TACCR0 - TimeUpEdge[channel_num] + PPM_aux;
                }
                else{ // se nao deu overflow na contagem do timer
                    PPMValue[channel_num] = PPM_aux - TimeUpEdge[channel_num];
                }
                P1IES &= ~(PPM_ch_counter & PPM_P1MASK); // configure low to high      
            }
            P1IFG &= ~(PPM_ch_counter & PPM_P1MASK); // apaga o interruption flag da P1.7
        }
        channel_num++;
    }
}

void calibrate_radio(void){
    
    if(MotorArmed == 1 ){ // nao faz se os motores estiverem armados
        return;
    }

    int i = 0, k;    
    float PPMMin[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    float PPMMax[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    fourbytes ValorAux;        
    
    lcd_goto(2,5);
    color_fore = RED;
    printf("MOVE STICKS\nALL DIRECTIONS");
    color_fore = LIME;
    delayms(3000);
    lcd_clear(BLACK);

    for(i = 0; i < 8; i++){ // zera offset e slope
        PPMOffset[i] = 0;
        PPMSlope[i] = 1;
    }

    for(i = 0; i < 8; i++){ // coloca um valor inicial em max min
        PPMMin[i] = PPMValue[i];
        PPMMax[i] = PPMMin[i];
    }
    
    for(k = 120; k > 0; k--){
        draw_rc_inputs(1);
        lcd_goto(0, 14);
        printf("%d ", k);
        delayms(1);
        for(i = 0; i < 8; i++){ // pega max min
            if(PPMMax[i] < PPMValue[i]){ // nao ta pegando maximo
                PPMMax[i] = PPMValue[i];
            }
            if(PPMMin[i] > PPMValue[i]){
                PPMMin[i] = PPMValue[i];
            }
        }
    }
    
    lcd_clear(BLACK);
    
    for(i = 0; i < 8; i++){ // calcula offset e slope pra deixar na faixa de 1000 a 2000
        PPMSlope[i] = 2000.0 / (PPMMax[i] - PPMMin[i]);
        PPMOffset[i] = 2000.0 - (PPMSlope[i] * PPMMin[i]);
    }

    // grava dados na EEPROM
    if(EEPROMFound){
        i2c_change_address(EEPROM_I2C_ADDR);
        
        // grava yaw slope 
        ValorAux.f = PPMSlope[YAW_CH];
        i2c_write16_multiples(_RADIO_YAW_SLOPE, ValorAux.c, 4);
        
        delayms(100);
        
        // grava throttle slope 
        ValorAux.f = PPMSlope[THROTTLE_CH];
        i2c_write16_multiples(_RADIO_THROTTLE_SLOPE, ValorAux.c, 4);
        
        delayms(100);
        
        // grava pitch slope 
        ValorAux.f = PPMSlope[PITCH_CH];
        i2c_write16_multiples(_RADIO_PITCH_SLOPE, ValorAux.c, 4);
        
        delayms(100);
        
        // grava roll slope 
        ValorAux.f = PPMSlope[ROLL_CH];
        i2c_write16_multiples(_RADIO_ROLL_SLOPE, ValorAux.c, 4);
        
        delayms(100);
        
        // grava ch5 slope 
        ValorAux.f = PPMSlope[CH5_CH];
        i2c_write16_multiples(_RADIO_CH5_SLOPE, ValorAux.c, 4);
        
        delayms(100);
        
        // grava ch6 slope 
        ValorAux.f = PPMSlope[CH6_CH];
        i2c_write16_multiples(_RADIO_CH6_SLOPE, ValorAux.c, 4);
        
        delayms(100);
        
        // grava ch7 slope 
        ValorAux.f = PPMSlope[CH7_CH];
        i2c_write16_multiples(_RADIO_CH7_SLOPE, ValorAux.c, 4);
        
        delayms(100);
        
        // grava yaw offset 
        ValorAux.f = PPMOffset[YAW_CH];
        i2c_write16_multiples(_RADIO_YAW_OFFSET, ValorAux.c, 4);
        
        delayms(100);
        
        // grava throttle offset 
        ValorAux.f = PPMOffset[THROTTLE_CH];
        i2c_write16_multiples(_RADIO_THROTTLE_OFFSET, ValorAux.c, 4);
        
        delayms(100);
        
        // grava pitch offset 
        ValorAux.f = PPMOffset[PITCH_CH];
        i2c_write16_multiples(_RADIO_PITCH_OFFSET, ValorAux.c, 4);
        
        delayms(100);
        
        // grava roll offset 
        ValorAux.f = PPMOffset[ROLL_CH];
        i2c_write16_multiples(_RADIO_ROLL_OFFSET, ValorAux.c, 4);
        
        delayms(100);
        
        // grava ch5 offset 
        ValorAux.f = PPMOffset[CH5_CH];
        i2c_write16_multiples(_RADIO_CH5_OFFSET, ValorAux.c, 4);
        
        delayms(100);
        
        // grava ch6 offset 
        ValorAux.f = PPMOffset[CH6_CH];
        i2c_write16_multiples(_RADIO_CH6_OFFSET, ValorAux.c, 4);
        
        delayms(100);
        
        // grava ch7 offset 
        ValorAux.f = PPMOffset[CH7_CH];
        i2c_write16_multiples(_RADIO_CH7_OFFSET, ValorAux.c, 4);
        
        delayms(100);
        
        lcd_goto(2,2);
        printf("Params saved.");
        delayms(2000);
    }
}

void process_rc(void){
    ChannelInput[THROTTLE_CH] = PPMValue[THROTTLE_CH] * PPMSlope[THROTTLE_CH] + PPMOffset[THROTTLE_CH];
    ChannelInput[YAW_CH] = PPMValue[YAW_CH] * PPMSlope[YAW_CH] + PPMOffset[YAW_CH];
    ChannelInput[PITCH_CH] = PPMValue[PITCH_CH] * PPMSlope[PITCH_CH] + PPMOffset[PITCH_CH]; // pitch invertido
    ChannelInput[ROLL_CH] = PPMValue[ROLL_CH] * PPMSlope[ROLL_CH] + PPMOffset[ROLL_CH];
    ChannelInput[CH5_CH] = PPMValue[CH5_CH] * PPMSlope[CH5_CH] + PPMOffset[CH5_CH];
    ChannelInput[CH6_CH] = PPMValue[CH6_CH] * PPMSlope[CH6_CH] + PPMOffset[CH6_CH];
    ChannelInput[CH7_CH] = PPMValue[CH7_CH] * PPMSlope[CH7_CH] + PPMOffset[CH7_CH];
}

void load_transmitter_values(void){
    fourbytes ValorAux;

    if(EEPROMFound == 1){
        i2c_change_address(EEPROM_I2C_ADDR);    

        // load yaw slope e offset
        i2c_read16_multiples(_RADIO_YAW_SLOPE, ValorAux.c, 4);
        PPMSlope[YAW_CH] = ValorAux.f;
        i2c_read16_multiples(_RADIO_YAW_OFFSET, ValorAux.c, 4);
        PPMOffset[YAW_CH] = ValorAux.f;
        
        // load throttle slope  e offset
        i2c_read16_multiples(_RADIO_THROTTLE_SLOPE, ValorAux.c, 4);
        PPMSlope[THROTTLE_CH] = ValorAux.f;
        i2c_read16_multiples(_RADIO_THROTTLE_OFFSET, ValorAux.c, 4);
        PPMOffset[THROTTLE_CH] = ValorAux.f;
        
        // load pitch slope  e offset
        i2c_read16_multiples(_RADIO_PITCH_SLOPE, ValorAux.c, 4);
        PPMSlope[PITCH_CH] = ValorAux.f;
        i2c_read16_multiples(_RADIO_PITCH_OFFSET, ValorAux.c, 4);
        PPMOffset[PITCH_CH] = ValorAux.f;
        
        // load roll slope  e offset
        i2c_read16_multiples(_RADIO_ROLL_SLOPE, ValorAux.c, 4);
        PPMSlope[ROLL_CH] = ValorAux.f;
        i2c_read16_multiples(_RADIO_ROLL_OFFSET, ValorAux.c, 4);
        PPMOffset[ROLL_CH] = ValorAux.f;
    
        // load ch5 slope  e offset
        i2c_read16_multiples(_RADIO_CH5_SLOPE, ValorAux.c, 4);
        PPMSlope[CH5_CH] = ValorAux.f;
        i2c_read16_multiples(_RADIO_CH5_OFFSET, ValorAux.c, 4);
        PPMOffset[CH5_CH] = ValorAux.f;
    
        // load ch6 slope  e offset
        i2c_read16_multiples(_RADIO_CH6_SLOPE, ValorAux.c, 4);
        PPMSlope[CH6_CH] = ValorAux.f;
        i2c_read16_multiples(_RADIO_CH6_OFFSET, ValorAux.c, 4);
        PPMOffset[CH6_CH] = ValorAux.f;
        
        // load ch7 slope  e offset
        i2c_read16_multiples(_RADIO_CH7_SLOPE, ValorAux.c, 4);
        PPMSlope[CH7_CH] = ValorAux.f;
        i2c_read16_multiples(_RADIO_CH7_OFFSET, ValorAux.c, 4);
        PPMOffset[CH7_CH] = ValorAux.f;
    }
}

void draw_rc_inputs(char raw){
    unsigned int i = 0, k = 0, h = 0;
    lcd_goto(0,0);
    printf("     RC inputs");
    lcd_goto(0,2);
    
    for(i = 0; i < 8; i++){
        if(!raw){
            printf("ch[%d]: %d   \n", i, ChannelInput[i]);
        }
        else{
            printf("ch[%d]: %d   \n", i, PPMValue[i]);
        }
        k = 21 + (8 * i); //posicao inicial em y
        if(!raw){
            if(ChannelInput[i] > 4000){
                h=100;
            }
            else{
                if(ChannelInput[i] < 2001){
                    h = 0;
                }
                else{
                    h = ((ChannelInput[i] - 2000) * 10) / 200;
                }
            }
        }
        else{
            if(PPMValue[i] > 4000){
                h=100;
            }
            else{
                if(PPMValue[i] < 2001){
                    h = 0;
                }
                else{
                    h = ((PPMValue[i] - 2000) * 10) / 200;
                }
            }
        }
        lcd_drawprogressbar(80,k,45,4,WHITE, BLUE, h);
    }
}

int find_first(int startaddress){
    for(; startaddress < 255; startaddress++){
        i2c_config(startaddress);
        if(!i2c_find_device()){
            return startaddress;
        }
    }   
    return 0;
}


// loop de controle
void main_loop(){
    
    
}

void setup(){
    
    clock_init();
    
    timer_a3_init();
    
    p1_init();
    
    analog_init();
    
    i2c_init();
    
    eint();
    
    lcd_init(BLACK);
    
    //printf("EEPROM ADDRESS: %dd\n", find_first(0));

    i2c_config(EEPROM_I2C_ADDR);
    EEPROMFound = !(i2c_find_device());
    
    if(EEPROMFound){
        printf("EEPROM found\n");
        i2c_read16_multiples(_STATUS_FLAGS, StatusFlags, 3);
        printf("SF: %d %d %d\n", StatusFlags[0], StatusFlags[1], StatusFlags[2]);

        //if(1){
        if(StatusFlags[0] != 0){
            printf("Calibrando radio...\n");
            delayms(4000);
            calibrate_radio();
            StatusFlags[0] = 0;
            StatusFlags[1] = 0;
            StatusFlags[2] = 0;
            i2c_write16_multiples(_STATUS_FLAGS, StatusFlags, 3);
        }
        else{
            load_transmitter_values();
            printf("RC params loaded\n");
            //carregar os outros parametros
        }
    }
    else{
        color_fore = RED;
        printf("EEPROM not found\n");
        color_fore = LIME;
    }
    
    delayms(5000);
    
    SetupDone = 1;
}

void analog_graph_clear(void){
    int i, k;
    for(i = 0; i < 8; i++){
        for(k = 0; k < 100; k++){
            AnalogGraph[i][k] = 0;
        }
    }
}


