/*
    hardware:
    - colocar um rele para ligar e desligar o ESC
    - ver qual celula de carga da pra usar
    * eeprom guarda parametros
    * analog para celula de carga
    * sensor luminosidade para rotacao
    * sensor hall para corrente
    
    setup:
    - desliga rele
    - serial default 34600
    - calibra analog
    - liga rele
    - calibra esc
    - init display
    
    mostra menu principal:
    - testar motor
    - config
    - manual
    
    menu config:
    - tempo de subida
    - numero de pontos no graf

    
    loop teste:
    se throttle <= 100
        sobe throttle
        espera estabilizar
        manda valores lidos
    manda throttle minimo
    desliga rele
    
    */

#include "stdio.h"
#include "stdlib.h"
#include "legacymsp430.h"
#include "msp430f2618.h"
#include "delay.h"
#include "analog.h"
#include "eeprom.h"
#include "i2c.h"
#include "resources.h"
#include "uart.h"

#define constrain(val,min,max) ((val)<(min)?(min):(val)>(max)?(max):val) // limita um valor entre min e max

#define DELAY_MOTOR_INC     0
#define DEFAULT_STAB        200

#define LOAD_CELL_ACH       3
#define CURRENT_ACH         4

#define DEFAULT_MOTOR_INC   100
#define MIN_THROTTLE        2000

typedef enum{
    START               = 'A',
    STOP                = 'B',
    CALIBRATE_ANALOG    = 'C',
    CALIBRATE_ESC       = 'D',
    SET_MOTOR_INC       = 'E',
    SET_MOTOR_STAB      = 'F',
    SAVE_PARAMS         = 'G',
    NONE                = 'H'
}COMMAND;

typedef enum{
    SHOW_MENU,
    IDLE,
    TESTING
}STATE;

void clock_init(void);
void timer_init(void);
void send_data(int valor);
void calibrate_esc(void);
unsigned char load_params(void);
unsigned char save_params(void);

// printf na uart:
int putchar(int c){
    return (int)(UART_putchar((char)c));
}

int main(void){
    WDTCTL = WDTPW + WDTHOLD;   // desabilita watchdog
    
    unsigned int MotorInc = DEFAULT_MOTOR_INC;   // 5%
    unsigned int MotorStab = DEFAULT_STAB;  
    unsigned int GPCounter = 0;

    int ValorBalanca = 0;
        
    STATE State = SHOW_MENU;
    COMMAND Cmd = STOP;  
    
    clock_init();
    UART_init();
    timer_init();
    analog_init();
    analog_calibrate_channel(LOAD_CELL_ACH); // offset
    analog_calibrate_channel(CURRENT_ACH); // offset
    //load_params();
       
    eint();
 
    delayms(300);
    
    printf(startup_txt);
    
    while(1){
        
        if(UC0IFG & UCA0RXIFG){
            Cmd = UCA0RXBUF;        
        }
        
        switch(State){
            case SHOW_MENU:
                printf(showmenu_txt);
                State = IDLE;
            break;
            
            case IDLE:
                TBCCR1 = MIN_THROTTLE;
                switch(Cmd){                    
                    case START:     
                        printf(testing_txt);   
                        State = TESTING;              
                    break;

                    case CALIBRATE_ANALOG:
                        for(GPCounter = 0; GPCounter < 8; GPCounter++){
                            analog_calibrate_channel(GPCounter);
                        }
                        State = SHOW_MENU;
                    break;
                    
                    case CALIBRATE_ESC:
                        printf(calibrate_esc_txt);
                        calibrate_esc();
                        State = SHOW_MENU;
                    break;
                                  
                    case SET_MOTOR_INC:
                        printf(motor_inc_txt);
                        while(!(UC0IFG & UCA0RXIFG));
                        MotorInc = UCA0RXBUF;        
                        MotorInc = constrain(MotorInc * 10, 10, 2000);
                        State = SHOW_MENU;
                    break;
                    
                    case SET_MOTOR_STAB:     
                        printf(motor_stab_txt);               
                        while(!(UC0IFG & UCA0RXIFG));
                        MotorStab = UCA0RXBUF;        
                        MotorStab = constrain(MotorStab * 10, 50, 1000);
                        State = SHOW_MENU;
                    break;

                    case SAVE_PARAMS:
                        if(save_params()){
                            printf(params_saved_txt);
                        }
                        else{
                            // diz q deu erro
                        }
                        State = SHOW_MENU;
                    break;
                    case NONE:
                    case STOP:
                    break;
                }   
                Cmd = NONE;
            break;
                        
            case TESTING:
                switch(Cmd){
                    case STOP:
                        State = SHOW_MENU;
                    break;
                    case START:     
                    case CALIBRATE_ANALOG:
                    case CALIBRATE_ESC:
                    case SET_MOTOR_INC:
                    case SET_MOTOR_STAB:     
                    case SAVE_PARAMS:
                    case NONE:
                    break;
                }

                analog_refresh_all();
                                
                ValorBalanca = 9 * ValorBalanca + (AnalogValue[LOAD_CELL_ACH] - AnalogOffset[LOAD_CELL_ACH]);
                ValorBalanca /= 10;
                
                if(get_delay(DELAY_MOTOR_INC)){
                    set_delay(DELAY_MOTOR_INC, MotorStab);
                    if(TBCCR1 == 4000){
                        State = SHOW_MENU;
                    }

                    send_data(ValorBalanca); // envia os dados antes de aumentar a rotacao
                    
                    TBCCR1 = constrain(TBCCR1 + MotorInc, 2000, 4000); // aumenta a rotacao
                }                                
            break;
        }
    }
}

unsigned char load_params(void){
    unsigned char c[1];
    printf(".");
    i2c_config(EEPROM_I2C_ADDR);
        printf(".");
    if(i2c_find_device(EEPROM_I2C_ADDR)){
        printf("FUCK");
    }
    else{
        printf("SUCK");
    }
        printf(".");
    i2c_wread_two_data(0x00, 0x00, c);
    printf("eeprom 0x00: %c", c[0]);
    i2c_wread_two_data(0x00, 0x01, c);
    printf("eeprom 0x01: %c", c[0]);
    return 1;
}

unsigned char save_params(void){
    // eeprom init
    i2c_config(EEPROM_I2C_ADDR);
    if(i2c_find_device(EEPROM_I2C_ADDR)){
        printf(eeprom_not_found_txt);
    }
    else{
        // teste
        i2c_write_one_data(0x00, 'A');
        i2c_write_one_data(0x01, 'B');
    }
    return 1;
}

void clock_init(void){
    BCSCTL2 = SELM_0 + DIVM_0 + DIVS_0;
    DCOCTL = 0x77;      
    BCSCTL1 = 0xCF;     
    BCSCTL3 = 0xA0;
}

void send_data(int valor){
    printf("%d ", valor);
}

void calibrate_esc(void){
    // desliga rele 
    // delay um segundo
    // set throttle 1 max
    // liga rele
    // espera 4s
    // throttle min
    // espera 4s
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

    TACTL = TASSEL_1 + ID_3 + MC_1 + TAIE;
    TACCR1 = 0;

    P2SEL |= 0x08;
    P2DIR |= 0x08;
}

interrupt (TIMERA1_VECTOR) TIMERA1_ISR_HOOK(void){ // 1ms
    int i = TIMEDELAY_LEN;
 
    do{
        i--;
        if(TimeDelay[i] > 0){
            TimeDelay[i]--;
        }
    }while(i);

    TACTL &= ~0x01;
}



