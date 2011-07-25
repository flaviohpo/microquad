#define PPM_P1MASK 0xFF

#define YAW_CH 4
#define THROTTLE_CH 3
#define PITCH_CH 2
#define ROLL_CH 0
#define CH5_CH 5
#define CH6_CH 6
#define CH7_CH 7

#define TRUE 1
#define FALSE 0

#define CONTROL_PERIOD 5
#define RC_PERIOD 19
#define MENU_PERIOD 69

#define MENU_LENGHT 4
#define GRAPH_LENGHT 100
#define GRAPH_OFFSET 85
#define GRAPH_OFFSETX 10

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
    RADIO_RAW = 0,
    RADIO_CORRECT,
    ANALOG_MONITOR,
    READY,
    INITIAL_SETTINGS
}MENU_OPTION;


void clock_init(void);
void p1_init(void);
void timer_a3_init(void);
void calibrate_radio(void);
void load_transmitter_values(void);
void draw_rc_inputs(char raw);
void show_menu(void);
void process_menu(void);
void process_rc(void);
void main_loop(void);
void process_option(void);
void analog_graph_clear(int n);
void analog_graph_clear_all(void);
void draw_analog_graph(void);
void process_analog_menu(void);
void refresh_analog_menu(void);
void setup(void);
int find_first(int staraddress);
int main(void);

