#define Constrain(val,min,max) ((val)<(min)?(min):(val)>(max)?(max):val) // limita um valor entre min e max

//#define ITG3200

#define LCD_MAX_BRIGHT          2500
#define PPM_P1MASK              0xFF

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
#define LETSFLY_INDEX           5

#define RETURN_INDEX            0

#define YAW_INDEX               0
#define PITCH_INDEX             1
#define ROLL_INDEX              2

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
    PROCESS_CONTROL_MENU        = CONTROL_MENU_INDEX,
    PROCESS_CONTROL             = LETSFLY_INDEX
}PROGRAM_STEP;

void menu_init(void);
void setup(void);
void timer_init(void);
void P2_init(void);
void P1_init(void);
void clock_init(void);
int main(void);
ACTION get_radio_action(void);
