#define constrain(val,min,max) ((val)<(min)?(min):(val)>(max)?(max):val) // limita um valor entre min e max

//#define TEST_LOOP_PERIOD

//#define ITG3200

#define BUZZER                0x10
#define BUZZER_ON()          (P2OUT |= BUZZER)
#define BUZZER_OFF()         (P2OUT &= ~BUZZER)
#define BUZZER_TOOGLE()      (P2OUT ^= BUZZER)
#define ENABLE_BUZZER()      (P2DIR |= BUZZER)
#define DISABLE_BUZZER()     (P2DIR &=~ BUZZER)

#define MIN_BATTERY           2650
#define MAX_BATTERY           3300
// battery colors
#define BATTERY_YELLOW        3050
#define BATTERY_RED           2850
#define BATTERY_CHECK_DELAY   2000

#define YAW_PROPORTIONAL_MUL    2
#define PITCH_PROPORTIONAL_MUL  1
#define ROLL_PROPORTIONAL_MUL   1
#define YAW_PROPORTIONAL_DIV    2
#define PITCH_PROPORTIONAL_DIV  2
#define ROLL_PROPORTIONAL_DIV   2
#define YAW_REF_DIV             0
#define PITCH_REF_DIV           2
#define ROLL_REF_DIV            2

#define BATTERY_ACH             7 
#define GYRO_YAW_ACH            3
#define GYRO_PITCH_ACH          1
#define GYRO_ROLL_ACH           0
#define ACCELX_ACH              5
#define ACCELY_ACH              4
#define ACCELZ_ACH              3

#define LCD_MAX_BRIGHT          2500
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

#define ANALOG_MENU_INDEX       0
#define CONTROL_MENU_INDEX      3
#define RADIO_MENU_INDEX        1
#define MOTOR_MENU_INDEX        2
#define SENSOR_MENU_INDEX       4
#define OPTION_MENU_INDEX       5
#define LETSFLY_INDEX           6

#define CALIBR_INDEX            1
#define GYRO_CALIBR_INDEX       13
#define RETURN_INDEX            0

#define YAW_INDEX               0
#define PITCH_INDEX             1
#define ROLL_INDEX              2

#define SAVE_INDEX              1
#define RESET_INDEX             2

#define ACCEL_X_INDEX           0
#define ACCEL_Y_INDEX           1
#define ACCEL_Z_INDEX           2

#define GYRO_X_INDEX            0
#define GYRO_Y_INDEX            1
#define GYRO_Z_INDEX            2

typedef enum{
    PROCESS_MAIN_MENU           = 0xFF,
    PROCESS_ANALOG_MENU         = ANALOG_MENU_INDEX,
    PROCESS_MOTOR_MENU          = MOTOR_MENU_INDEX,
    PROCESS_SENSOR_MENU         = SENSOR_MENU_INDEX,
    PROCESS_RADIO_MENU          = RADIO_MENU_INDEX,
    PROCESS_OPTION_MENU         = OPTION_MENU_INDEX,
    PROCESS_CONTROL_MENU        = CONTROL_MENU_INDEX,
    PROCESS_CONTROL             = LETSFLY_INDEX
}PROGRAM_STEP;

unsigned char save_params(void);
unsigned char load_params(void);
unsigned char reset_defaults(void);
void menu_init(void);
void setup(void);
void timer_init(void);
void P2_init(void);
void P1_init(void);
void clock_init(void);
void set_motor_output(void);
void set_all_motors(unsigned int val);
void screen_flash(int Color, int interval, int times);
void calibrate_radio(void);
void ControlLoop(void);
int main(void);
ACTION get_radio_action(void);

