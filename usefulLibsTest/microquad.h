#define PPM_P1MASK 0xFF

#define LOW_BAT 2000

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

#define TRUE 1
#define FALSE 0


void clock_init(void);
void p1_init(void);
void timer_a3_init(void);
void calibrate_radio(void);
void rx_get_channels(void);
void setup(void);
int main(void);

