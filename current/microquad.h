#define constrain(val, min, max) ((val)<(min)?(min):(val)>(max)?(max):val) // limita um valor entre min e max

#define LIGHTS_ON_I2C
#ifdef LIGHTS_ON_I2C
#define LED_RIGHT_TOGGLE() (P3OUT^=0x02)
#define LED_BACK_TOGGLE() (P3OUT^=0x04)
#endif

#define LIGHTS_ON_UART
#ifdef LIGHTS_ON_UART
#define LED_LEFT_TOGGLE()   (P3OUT^=0x20)
#endif
//#define TEST_LOOP_PERIOD
//#define ITG3200
#define SAMPLE_TIME           20 // * 100us (2000ns)

#define BUZZER                0x10
#define BUZZER_ON()          (P2OUT |= BUZZER)
#define BUZZER_OFF()         (P2OUT &= ~BUZZER)
#define BUZZER_TOOGLE()      (P2OUT ^= BUZZER)
#define ENABLE_BUZZER()      (P2DIR |= BUZZER)
#define DISABLE_BUZZER()     (P2DIR &=~ BUZZER)

#define LCD_CENTER_VIBRATION_X 64
#define LCD_CENTER_VIBRATION_Y 70

#define MIN_BATTERY           1520
#define MAX_BATTERY           1850
// battery colors
#define BATTERY_YELLOW        1720
#define BATTERY_RED           1590
#define BATTERY_CHECK_TIME    2000
// default proportional gain
#define YAW_PROPORTIONAL_MUL    4
#define PITCH_PROPORTIONAL_MUL  6
#define ROLL_PROPORTIONAL_MUL   6
#define YAW_PROPORTIONAL_DIV    1
#define PITCH_PROPORTIONAL_DIV  2
#define ROLL_PROPORTIONAL_DIV   2
// default integral gain
#define YAW_INTEGRAL_MUL        1
#define PITCH_INTEGRAL_MUL      1
#define ROLL_INTEGRAL_MUL       1
#define YAW_INTEGRAL_DIV        1
#define PITCH_INTEGRAL_DIV      1
#define ROLL_INTEGRAL_DIV       1
// default proportional radio influence
#define YAW_REF_MUL             3
#define PITCH_REF_MUL           4
#define ROLL_REF_MUL            4
#define YAW_REF_DIV             1
#define PITCH_REF_DIV           3
#define ROLL_REF_DIV            3

// default filters
#define THROTTLE_LP_MUL         7
#define THROTTLE_LP_DIV         3
#define MOTOR_OUTPUT_LP_MUL     1
#define MOTOR_OUTPUT_LP_DIV     1
#define GYRO_LP_MUL             1
#define GYRO_LP_DIV             1
#define ACCEL_LP_MUL            7
#define ACCEL_LP_DIV            3         

#define BATTERY_ACH             7 
#define GYRO_YAW_ACH            2
#define GYRO_PITCH_ACH          1
#define GYRO_ROLL_ACH           0
#define ACCELX_ACH              4
#define ACCELY_ACH              3
#define ACCELZ_ACH              5

#define LCD_MAX_BRIGHT          2800
#define PPM_P1MASK              0xFF

#define MIN_MOTOR               2000
#define MAX_MOTOR               4000
#define MIN_MOTOR_WORK          2450

#define MOTOR_FRONT             2
#define MOTOR_BACK              1
#define MOTOR_RIGHT             3
#define MOTOR_LEFT              0

#define STICK_UPPER_THRESHOLD   3200
#define STICK_LOWER_THRESHOLD   2300

#define RADIO_PITCH_CH          2
#define RADIO_YAW_CH            4
#define RADIO_THROTTLE_CH       3
#define RADIO_ROLL_CH           0

#define GRAPH_LENGHT            56
#define GRAPH_OFFSETX           36
#define GRAPH_OFFSET            115
#define GRAPH_START             64
#define GRAPH_HEIGHT            52

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
    OPTION_MENU_INDEX           
}MENU_INDEX;

#define RETURN_INDEX            0
#define CALIBR_INDEX            1
#define THROTTLE_CALIBR_INDEX   2

#define YAW_INDEX               0
#define PITCH_INDEX             1
#define ROLL_INDEX              2

#define SAVE_INDEX              1
#define RESET_INDEX             2

#define ACCEL_X_INDEX           0
#define ACCEL_Y_INDEX           1
#define ACCEL_Z_INDEX           2

#define GYRO_X_INDEX            2
#define GYRO_Y_INDEX            1
#define GYRO_Z_INDEX            0

#define ANALOG_CH0_INDEX        2
#define ANALOG_CH1_INDEX        3
#define ANALOG_CH2_INDEX        4
#define ANALOG_CH3_INDEX        5
#define ANALOG_CH4_INDEX        6
#define ANALOG_CH5_INDEX        7
#define ANALOG_CH6_INDEX        8
#define ANALOG_CH7_INDEX        9

typedef struct{
    int AccelHigher[3];
    int GyroHigher[3];
    int AccelLower[3];
    int GyroLower[3];
    int ControlResultHigher[3];
    int ControlResultLower[3];
    int MotorOutputHigher[6];
    int MotorOutputLower[6];
}BlackBox;

typedef enum{
    RADIO_MENU_VISUALIZANDO             ,
    RADIO_MENU_CALIBRANDO_THROTTLE
}RADIO_MENU_STEP;

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
    PROCESS_CONTROL                     = LETSFLY_INDEX
}PROGRAM_STEP;

int main(void);

unsigned char save_params(void);
unsigned char load_params(void);
unsigned char reset_defaults(void);

void adjust_readings(void);
void menu_init(void);
void setup(void);
void init_vars(void);
void timer_init(void);
void P2_init(void);
void P1_init(void);
void clock_init(void);
void set_motor_output(void);
void set_all_motors(unsigned int val);
void screen_flash(int Color, int interval, int times);
void calibrate_radio(void);
void control_loop(void);
void process_analog_graph(void);
void analog_graph_clear(int i);
void process_vibration_analyzer_menu(void);

ACTION get_radio_action(void);

