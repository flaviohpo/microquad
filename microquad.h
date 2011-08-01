#define PPM_P1MASK 0xFF

// descobrir valor bom
#define PITCH_P_DEFAULT     1
#define YAW_P_DEFAULT       1
#define ROLL_P_DEFAULT      1

#define PITCH_D_DEFAULT     0
#define YAW_D_DEFAULT       0
#define ROLL_D_DEFAULT      0

#define PITCH_I_DEFAULT     0
#define YAW_I_DEFAULT       0
#define ROLL_I_DEFAULT      0

#define PITCH_RC_GAIN     0
#define YAW_RC_GAIN       0
#define ROLL_RC_GAIN      0

#define FRONT 0
#define LEFT 1
#define RIGHT 2
#define BACK 3

#define MIN_COMMAND 20

#define GYRO_ROLL 0
#define GYRO_PITCH 1
#define GYRO_YAW 2

#define PROPORTIONAL 0
#define DERIVATIVE 1
#define INTEGRAL 2

#define ROLL 0
#define PITCH 1
#define YAW 2
#define THROTTLE 3

#define YAW_CH 4
#define THROTTLE_CH 3
#define PITCH_CH 2
#define ROLL_CH 0
#define CH5_CH 5
#define CH6_CH 6
#define CH7_CH 7

#define STICK_UP_THRESHOLD (3250)
#define STICK_DOWN_THRESHOLD (2500)

#define TRUE 1
#define FALSE 0

#define MENU_LENGHT 11

#define GRAPH_LENGHT 75
#define GRAPH_OFFSET 65
#define GRAPH_OFFSETX 25

char FlightModeNames[3][10] = {"Normal", "Acro", "UFO"};
char MenuOptionNames[MENU_LENGHT][18] = {"Ready", "Change mode", "Control Ref", "Radio raw", "Analog monitor", "Change PID", "Recalibrate gyro", "Recalibrate radio", "Show settings", "Default settings", "Save current PID"};
char PIDMenuOptionNames[9][8] = {"P Roll", "P Yaw", "P Pitch", "D Roll", "D Yaw", "D Pitch", "I Roll", "I Yaw", "I Pitch"};

typedef enum{
    NORMAL = 0,
    ACRO,
    UFO
}FLIGHT_MODE;

// MENU OPTIONS
typedef enum{
    DISPLAY,
    SELECT,
    WAIT_PITCH_DOWN,
    WAIT_PITCH_UP,
    WAIT_CH7_BACK,
    WAIT_ROLL_RIGHT,
    WAIT_ROLL_LEFT,
    PROCESS_OPTION
}MENU_STEPSET;

typedef enum{
    READY = 0,
    CHANGE_FLIGHT_MODE,
    CONTROL_REF,
    RADIO_RAW,
    ANALOG_MONITOR,
    CHANGE_PID,
    RE_CALIBRATE_GYRO,
    RE_CALIBRATE_RADIO,
    SHOW_SETTINGS,
    INITIAL_SETTINGS,
    SAVE_CURRENT_GAINS
}MENU_OPTION;

typedef enum{
    CHANGE_P_ROLL = 0x00 & ROLL,
    CHANGE_P_YAW = 0x00 & YAW,
    CHANGE_P_PITCH = 0x00 & PITCH,
    CHANGE_D_ROLL = 0x10 & ROLL,
    CHANGE_D_YAW = 0x10 & YAW,
    CHANGE_D_PITCH = 0x10 & PITCH,
    CHANGE_I_ROLL = 0x20 & ROLL,
    CHANGE_I_YAW = 0x20 & YAW,
    CHANGE_I_PITCH = 0x20 & PITCH
}PID_TUNING_OPT;

void clock_init(void);
void p1_init(void);
void timer_a3_init(void);
void calibrate_radio(void);
void calibrate_gyro(void);
void load_eeprom_values(void);
void draw_rc_inputs(void);
void show_menu(void);
void process_menu(void);
void rx_get_channels(void);
void main_loop(void);
void process_option(void);
void process_PID_option(void);
void analog_graph_clear(int n);
void analog_graph_clear_all(void);
void draw_analog_graph(void);
void process_analog_menu(void);
void refresh_analog_menu(void);
void setup(void);
void set_defaults(void);
void show_settings(void);
void draw_control_ref(void);
void process_PID_menu(void);
void save_current_gains(void);
int find_first(int staraddress);
int main(void);

