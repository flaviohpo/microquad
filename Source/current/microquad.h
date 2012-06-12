#ifndef __MINIQUAD_H
#define __MINIQUAD_H

// esse macro limita o valor entre o min e o max, pode ser int, float, long, etc
#define constrain(val, min, max) ((val)<(min)?(min):(val)>(max)?(max):val) // limita um valor entre min e max

// sample rate, nao eh usado no programa, soh informativo
#define SAMPLE_TIME           25 // * 100us = (2500ns) = 400Hz

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/

// CONTROL PARAMS

#define YAW_PROPORTIONAL_MUL    2
#define PITCH_PROPORTIONAL_MUL  1
#define ROLL_PROPORTIONAL_MUL   1
#define YAW_PROPORTIONAL_DIV    0
#define PITCH_PROPORTIONAL_DIV  0
#define ROLL_PROPORTIONAL_DIV   0

// control saturation
#define PITCH_ROLL_P_SATURATION 750
#define YAW_P_SATURATION        500 // not used

// default integral gain, soh tem DIV pro integral, pode ser 1, 2, 4, 8, 32, 64, 128, 256....
#define YAW_INTEGRAL_DIV        7   // = 2^DIV
#define PITCH_INTEGRAL_DIV      7   // 8 vai limitar o integral em 128, 7 vai limitar em 256
#define ROLL_INTEGRAL_DIV       7

#define YAW_INTEGRAL_LIMIT      32000
#define PITCH_INTEGRAL_LIMIT    32000
#define ROLL_INTEGRAL_LIMIT     32000

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/

// default filters
// esses filtros devem ser tais que (mul + 1) = 2^div
#define THROTTLE_LP_MUL         7
#define THROTTLE_LP_DIV         3
#define MOTOR_OUTPUT_LP_MUL     1
#define MOTOR_OUTPUT_LP_DIV     1
#define GYRO_LP_MUL             3
#define GYRO_LP_DIV             2
#define ACCEL_LP_MUL            3
#define ACCEL_LP_DIV            2         

#define BATTERY_ACH             7 
#define GYRO_YAW_ACH            2
#define GYRO_PITCH_ACH          1
#define GYRO_ROLL_ACH           0
#define ACCELX_ACH              4   // roll
#define ACCELY_ACH              3   // pitch
#define ACCELZ_ACH              5

/* arrays indexes */
#define RADIO_PITCH_CH          2
#define RADIO_YAW_CH            4
#define RADIO_THROTTLE_CH       3
#define RADIO_ROLL_CH           0

#define MOTOR_LEFT              0
#define MOTOR_BACK              1
#define MOTOR_FRONT             2
#define MOTOR_RIGHT             3

#define YAW_INDEX               0
#define PITCH_INDEX             1
#define ROLL_INDEX              2

#define ANALOG_CH0_INDEX        2
#define ANALOG_CH1_INDEX        3
#define ANALOG_CH2_INDEX        4
#define ANALOG_CH3_INDEX        5
#define ANALOG_CH4_INDEX        6
#define ANALOG_CH5_INDEX        7
#define ANALOG_CH6_INDEX        8
#define ANALOG_CH7_INDEX        9

/* menu indexes */
#define RETURN_INDEX            0
#define CALIBR_INDEX            1
#define THROTTLE_CALIBR_INDEX   2
#define SAVE_INDEX              1
#define RESET_INDEX             2

/* constantes e limites */
#define LCD_MAX_BRIGHT          2800

#define MIN_MOTOR               2000
#define MAX_MOTOR               4000
#define MIN_MOTOR_WORK          2450 // parece q nao esta sendo usado

#define STICK_UPPER_THRESHOLD   3000
#define STICK_LOWER_THRESHOLD   2300

#define GRAPH_LENGHT            56
#define GRAPH_OFFSETX           36
#define GRAPH_OFFSET            115
#define GRAPH_START             64
#define GRAPH_HEIGHT            52

#define LCD_CENTER_VIBRATION_X  64
#define LCD_CENTER_VIBRATION_Y  70

#define BATTERY_YELLOW          1720
#define BATTERY_RED             1590
#define BATTERY_CHECK_TIME      2000
#define MIN_BATTERY             1520
#define MAX_BATTERY             1850

/*************************************************************************/
/*************************************************************************/
/*************************************************************************/
// ENUMERATORS

typedef enum{
    FLYMODE_ACRO,
    FLYMODE_STABLE,
}FLIGHT_MODE;

enum{
    LETSFLY_INDEX               ,
    ANALOG_MENU_INDEX           ,
    CONTROL_MENU_INDEX          ,
    FILTER_MENU_INDEX           ,
    RADIO_MENU_INDEX            ,
    MOTOR_MENU_INDEX            ,
    SENSOR_MENU_INDEX           ,
    VOLTAGE_DROP_INDEX          ,
    VIBRATION_ANALYZER_INDEX    ,
    OPTION_MENU_INDEX           ,
}MENU_INDEX;

typedef enum{
    PROCESS_MAIN_MENU                   = 0xFF,
    PROCESS_ANALOG_MENU                 = ANALOG_MENU_INDEX,
    PROCESS_MOTOR_MENU                  = MOTOR_MENU_INDEX,
    PROCESS_SENSOR_MENU                 = SENSOR_MENU_INDEX,
    PROCESS_RADIO_MENU                  = RADIO_MENU_INDEX,
    PROCESS_OPTION_MENU                 = OPTION_MENU_INDEX,
    PROCESS_CONTROL_MENU                = CONTROL_MENU_INDEX,
    PROCESS_FILTER_MENU                 = FILTER_MENU_INDEX,
    PROCESS_VIBRATION_ANALYZER_MENU     = VIBRATION_ANALYZER_INDEX,
    PROCESS_VOLTAGE_DROP_MENU           = VOLTAGE_DROP_INDEX,
    PROCESS_CONTROL                     = LETSFLY_INDEX,
}PROGRAM_STEP;

/**************************************************************************/
/**************************************************************************/
/**************************************************************************/
// STRUCTURES

/*
sugestoes:
- Controller (Filter + Control + Sensor)
- ScrPatern ( screen patern ) talves deveria ir no menu.c
- RadioReceiver
*/

/**************************************************************************/
/**************************************************************************/
/**************************************************************************/
// function prototypes

int main(void);

unsigned char save_params(void);
unsigned char load_params(void);
unsigned char reset_defaults(void);

void adjust_readings(void);
void menu_init(void);
void setup(void);
void init_vars(void);
void timer_init(void);
void P1_init(void);
void clock_init(void);
void set_motor_output(void);
void set_all_motors(unsigned int val);
void screen_flash(int Color, int interval, int times);
void control_loop_acro(void);
void control_loop_stable(void);
void process_analog_graph(void);
void analog_graph_clear(int i);
void process_vibration_analyzer_menu(void);

ACTION get_radio_action(void);

#endif // __MINIQUAD_H

