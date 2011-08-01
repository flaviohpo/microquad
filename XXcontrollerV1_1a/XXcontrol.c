// XXcopter software
// =================
// Version 1.1a
// inspired by KKmulticopter
// based on assembly code by Rolf R Bakke
//
// converted by Mike Barton
// additional code suggestions by manuLRK

// used Quad V4.3 and Tri V1.2
// compatible with KKcontroller boards
// supports 88,168 and 328 
// ATmega48 can be used but has limited compatibility
//
// Copter Types*: TRI_COPTER, QUAD_COPTER, QUAD_X_COPTER and HEX_COPTER, Y6_COPTER
// * only QUAD & QUAD-X tested
//
// Additional Features:
// Stick centering - allows zero position of sticks to be saved and used in calculations
// Adjustable (re-compile) ESC rate

// Config Modes (at startup)
// =========================

// Stick Centering
// ---------------
// set Pitch Gain pot to zero
// set Tx trims to centre
// power on
// led flashes 3 times
// restore gain pot
// use as normal

// Gyro direction reversing
// ------------------------
// set Roll Gain pot to zero
// power on
// led flashes 3 times
// move stick Up/Left for Normal, Down/Right for reverse
// eg: to set Roll Gyro Normal, move Tx Roll stick left, or to reverse it move stick right

// ESC throttle calibration
// ------------------------
// set Yaw Gain pot to zero
// put throttle stick to full
// power on
// led flashes 3 times
// wait for motor signal (double bleep)
// throttle to zero
// wait for motor confirm signal
// power off
// restore gain pot

// Clear all settings (gyro & stick centering)
// -------------------------------------------
// set Roll, Pitch & Yaw Gain pots to zero
// power on, wait a few seconds, power off
// restore gain pots


// Other Notes
// ===========
//
// differences compared to Rolfs code
// ----------------------------------
// if gyro reversed, Gyro gain pots work normally (ie full ccw = 0)
// gyro direction reversing works differently
// Gyro gain pots are checked at the same time as gyro calibration
//   (ie changes to pots while props spinning will have no effect)

// Version History
// ===============
// V1.1a	used V1.0i as base point
//				code re-worked to optimise PWM output (re manuLRK)
//				Stick arming now standard (not optional) 
//				added Stick Scaling
//				removed TRI stick scaling from gyro gain pot.

/*


Tri   
 		  
	 	M1 CW      M2 CCW
		  \       / 
       \ --- /
			  |   |
				 --- 
				  |  
					|
					M3    M4=Tail Servo  


Quad
            M1 CW
						|
						|
						|
          +---+
CCW M2----|   |----M3 CCW
		      +---+
					  |
						|
						|
						M4 CW

Quad-X
 		  
	  	M1 CW     M2 CCW
	 	   \       / 
        \ --- /
         |   |
 	      / --- \
 		   /       \ 
 	   M4	CCW 	 M3 CW
 
Hex
            M1 CW
						|
		 M6  		|      M2 CCW
			 \		|     /
        \ +---+  /
         -|   |- 
		    / +---+  \
			 /	  |     \
		 M5			|      M3 CW
						|
						M4

Y6
 		  
	 	M1/4        M2/5		M1->3 = CW
		  \       / 			 	M4->6 = CCW
       \ --- /
			  |   |
				 --- 
				  |  
					|
					M3/6

*/

/* ----------- Configuration -----------  */
// Don't forget to set your device in Project/Configuration Options
// 
// enable ONE of these lines
//#define TRI_COPTER 
#define QUAD_COPTER
//#define QUAD_X_COPTER
//#define HEX_COPTER
//#define Y6_COPTER


// defines output rate to ESC/Servo
// either define by setting ESC_RATE (Max is approx 495Hz)

#define ESC_RATE 450	// in Hz		
#define PWM_LOW_PULSE_INTERVAL  ((1000000 / ESC_RATE) - 2000)/10

//or define by setting PWM_LOW_PULSE_INTERVAL (minimum is 1)

//#define PWM_LOW_PULSE_INTERVAL 50		// this equates to 400Hz
//#define ESC_RATE = 10000000 / ( (PWM_LOW_PULSE_INTERVAL * 10) + 20000 )

//PWM_LOW_PULSE_INTERVAL is the number of 10uS that PWM signal is low


// Adjust these:
// 		down if you have too much gyro assistance
// 		up if you have maxxed your gyro gain 
#define ROLL_GAIN_MULTIPLIER 3
#define PITCH_GAIN_MULTIPLIER 3
#define YAW_GAIN_MULTIPLIER 3

// Stick scaling
// 'normal' is 1
// to double stick effect, subtract by 1 (but do not go -ve!)
// to half, add 1
#define ROLL_STICK_DIVIDER 1
#define PITCH_STICK_DIVIDER 1
#define YAW_STICK_DIVIDER 1

// Stick Arming
// if you cannot either arm or disarm, lower this value
#define STICKARM_POINT 60	// defines how far stick must be moved
// set one of the following lines
#define STICKARM_LEFT
//#define STICKARM_RIGHT

// enable this line if you don't have Yaw gyro connected
//#define EXTERNAL_YAW_GYRO

// Max Collective
// limits the maximum stick collective (range 80->100  100=Off)
// this allows gyros to stabilise better when full throttle applied
#define MAX_COLLECTIVE 95

/* ----------- Main Code -----------  */

#include <avr/io.h>  
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <util/delay.h>
#include <avr/interrupt.h> 
#include <avr/eeprom.h>

#include "typedefs.h"
#include "io_cfg.h"


#define EEPROM_DATA_START_POS 10		// allows moving (not really necessary)

// Max ADC
#define UC_ADC_MAX 1023			// used to invert ADC reading of uC

enum GyroDirection { GYRO_NORMAL = 0, GYRO_REVERSED };

enum GyroArrayIndex { ROLL = 0, PITCH, YAW };

// eeProm data structure
typedef struct Config_Struct CONFIG_STRUCT;
struct Config_Struct
{
	uint8_t	setup;					// byte to identify if already setup

	uint8_t RollGyroDirection;
	uint8_t PitchGyroDirection;
	uint8_t YawGyroDirection;

	// allows setting to zero
	uint16_t RxChannel1ZeroOffset;
	uint16_t RxChannel2ZeroOffset;
	uint16_t RxChannel3ZeroOffset;  // currently fixed
	uint16_t RxChannel4ZeroOffset;

};

CONFIG_STRUCT Config;		// Holds configuration (from eeProm)

bool GyroCalibrated;
volatile BOOL RxChannelsUpdatingFlag;

bool Armed;

uint16_t GainInADC[3];	// ADC result
uint16_t GainIn[3];			// = GainInADC[3]/10

#ifdef TRI_COPTER
int16_t GainAttenuator;
uint16_t ServoPPMRateDivider = 0;
int8_t RxInYawAdjust;
#endif

volatile uint16_t RxChannel1;		// ISR vars
volatile uint16_t RxChannel2;
volatile uint16_t RxChannel3;
volatile uint16_t RxChannel4;

uint16_t RxChannel1Start;				// ISR vars (could be local static)
uint16_t RxChannel2Start;
uint16_t RxChannel3Start;
uint16_t RxChannel4Start;

int16_t RxInRoll;								// program vars
int16_t RxInPitch;
int16_t RxInCollective;
int16_t RxInYaw;

int16_t gyroADC[3];							// Holds Gyro ADC's
int16_t gyroZero[3] = {0,0,0};	// used for calibrating Gyros on ground


bool output_motor_high = false;
uint16_t PWM_Low_Pulse_Interval = PWM_LOW_PULSE_INTERVAL;		// non-const version of PWM_LOW_PULSE_INTERVAL

int16_t MotorOut1;
int16_t MotorOut2;
int16_t MotorOut3;
int16_t MotorOut4;
#if defined(HEX_COPTER) || defined(Y6_COPTER)
int16_t MotorOut5;
int16_t MotorOut6;
#endif

void setup(void);
void loop(void);

void Init_ADC(void);
void ReadGyros(bool calibrate);
void CalibrateGyros(void);

void ReadGainPots(void);
void RxGetChannels(void);
void read_adc(uint8_t channel);

void output_motor_ppm(void);

void Initial_EEPROM_Config_Load(void);
void Save_Config_to_EEPROM(void);
void Set_EEPROM_Default_Config(void);

void eeprom_write_byte_changed( uint8_t *  addr, uint8_t value );
void eeprom_write_block_changes( const uint8_t * src, void * dest, size_t size );


// RX_ROLL
ISR(PCINT2_vect)
{

	if ( RX_ROLL )		// rising
	{
		RxChannel1Start = TCNT1;

	} else {				// falling
		RxChannelsUpdatingFlag = 1;
		RxChannel1 = TCNT1 - RxChannel1Start;
		RxChannelsUpdatingFlag = 0;
	}
}

// RX_PITCH
ISR(INT0_vect)
{
	if (RX_PITCH)		
	{
		RxChannel2Start = TCNT1;

	} else {				// falling
		RxChannelsUpdatingFlag = 1;
		RxChannel2 = TCNT1 - RxChannel2Start;
		RxChannelsUpdatingFlag = 0;
	}
}

// RX_COLL
ISR(INT1_vect)
{
	if (RX_COLL)		
	{
		RxChannel3Start = TCNT1;

	} else {				// falling
		RxChannelsUpdatingFlag = 1;
		RxChannel3 = TCNT1 - RxChannel3Start;
		RxChannelsUpdatingFlag = 0;
	}
}

// RX_YAW
ISR(PCINT0_vect)
{

	if ( RX_YAW )		// rising
	{
		RxChannel4Start = TCNT1;

	} else {				// falling
		RxChannelsUpdatingFlag = 1;
		RxChannel4 = TCNT1 - RxChannel4Start;
		RxChannelsUpdatingFlag = 0;
	}
}



int main(void)
{

	setup();

	while (1)
	{
		loop();
	}

	return 1;
}

void setup(void)
{
	uint16_t i;	// nb was uint8_t, must be uint16_t for TRI
	uint16_t RxChannel1ZeroOffset, RxChannel2ZeroOffset, RxChannel4ZeroOffset;

	MCUCR |= (1<<PUD);	// Pull-up Disable

	RX_ROLL_DIR 			= INPUT;
	RX_PITCH_DIR 			= INPUT;
	RX_COLL_DIR   		= INPUT;
	RX_YAW_DIR   	 		= INPUT;

	GYRO_YAW_DIR 	 		= INPUT;
	GYRO_PITCH_DIR 	 	= INPUT;
	GYRO_ROLL_DIR  		= INPUT;
	GAIN_YAW_DIR 	 		= INPUT;
	GAIN_PITCH_DIR  	= INPUT;
	GAIN_ROLL_DIR  		= INPUT;

	M1_DIR 					 	= OUTPUT;
	M2_DIR 				 		= OUTPUT;
	M3_DIR 			 			= OUTPUT;
	M4_DIR 			 			= OUTPUT;
#if defined(HEX_COPTER) || defined(Y6_COPTER)
	M5_DIR 				 		= OUTPUT;
	M6_DIR 				 		= OUTPUT;
#endif
	LED_DIR 			 		= OUTPUT;

	LED 			= 0;
	RX_ROLL 	= 0;
	RX_PITCH 	= 0;
	RX_COLL  	= 0;
	RX_YAW   	= 0;

	// pin change interrupt enables
	PCICR |= (1 << PCIE0);			// PCINT0..7		
	PCICR |= (1 << PCIE2);			// PCINT16..23

	// pin change masks
	PCMSK0 |= (1 << PCINT7);		// PB7
	PCMSK2 |= (1 << PCINT17);		// PD1
	// external interrupts
	EICRA  = (1 << ISC00) | (1 << ISC10);	// Any change INT0, INT1
	EIMSK  = (1 << INT0) | (1 << INT1);		// External Interrupt Mask Register
	EIFR |= (1 << INTF0) | (1 << INTF1);

	// timer0 (8bit) - run @ 8MHz
	// used to control ESC/servo pulse length
	TCCR0A = 0;						// normal operation
	TCCR0B = (1 << CS00);	// clk/0
	TIMSK0 = 0; 					// no interrupts

	// timer1 (16bit) - run @ 1Mhz
	// used to measure Rx Signals & control ESC/servo output rate
	TCCR1A = 0;
	TCCR1B = (1 << CS11);

	// timer2 8bit - run @ 8MHz / 1024 = 7812.5KHz
	// and Stick-Arming
	TCCR2A = 0;	
	TCCR2B = (1 << CS22) | (1 << CS21) | (1 << CS20);	// /1024
	TIMSK2 = 0;
	TIFR2  = 0;
	TCNT2 = 0;		// reset counter

#ifdef TRI_COPTER
	// calculate Servo Rate divider
	ServoPPMRateDivider = 0;
	do {
		ServoPPMRateDivider++;
		i = ESC_RATE / ServoPPMRateDivider;
	} while (i>50);
#endif

	Initial_EEPROM_Config_Load();	// loads config at start-up 

	Init_ADC();

	GyroCalibrated = false;

	Armed = false;

	RxChannelsUpdatingFlag = 0;

	RxChannel1 = Config.RxChannel1ZeroOffset;		// prime the channels 1520;
	RxChannel2 = Config.RxChannel2ZeroOffset;		// 1520;
	RxChannel3 = Config.RxChannel3ZeroOffset;		// 1120;
	RxChannel4 = Config.RxChannel4ZeroOffset;		// 1520;

	// flash LED
	LED = 1;
	_delay_ms(150);
	LED = 0;

	sei();								// Global Interrupts 

	// 2 second delay
	_delay_ms(1500);
	
	ReadGainPots();
	ReadGainPots();

	// clear config
	if (GainInADC[PITCH] < (UC_ADC_MAX*5)/100 && 
			GainInADC[ROLL]  < (UC_ADC_MAX*5)/100 &&
			GainInADC[YAW]   < (UC_ADC_MAX*5)/100 )
	{
		Set_EEPROM_Default_Config();

		while ( 1 );
	} 

	// Stick Centering
	if (GainInADC[PITCH] < (UC_ADC_MAX*5)/100)		// less than 5%
	{
    // set offsets to zero (otherwise we affect what we want to calibrate !!)
    Config.RxChannel1ZeroOffset  = 0;
    Config.RxChannel2ZeroOffset  = 0;
    Config.RxChannel4ZeroOffset  = 0;

		// flash LED 3 times
		for (i=0;i<3;i++)
		{
			LED = 1;
			_delay_ms(25);
			LED = 0;
			_delay_ms(25);
		}
		RxChannel1ZeroOffset = RxChannel2ZeroOffset = RxChannel4ZeroOffset = 0;
		
		for (i=0;i<4;i++)
		{
	 		RxGetChannels();

	    RxChannel1ZeroOffset += RxInRoll;
	    RxChannel2ZeroOffset += RxInPitch;
	    RxChannel4ZeroOffset += RxInYaw;

			_delay_ms(100);
		}
		// nb RxGetChannels() divides RxInXXX by 4 so we won't here
    Config.RxChannel1ZeroOffset  = RxChannel1ZeroOffset;
    Config.RxChannel2ZeroOffset  = RxChannel2ZeroOffset;
    Config.RxChannel3ZeroOffset  = 1120;
    Config.RxChannel4ZeroOffset  = RxChannel4ZeroOffset;

		// Store gyro direction to EEPROM
		Save_Config_to_EEPROM();

	}

	// Gyro direction reversing
	if (GainInADC[ROLL] < (UC_ADC_MAX*5)/100)		// less than 5% (5/100) * 1023 = 51 
	{
		// flash LED 3 times
		for (i=0;i<3;i++)
		{
			LED = 1;
			_delay_ms(25);
			LED = 0;
			_delay_ms(25);
		}

		while(1)
		{
			RxGetChannels();

			if (RxInRoll < -30) {	// normal(left)
				Config.RollGyroDirection = GYRO_NORMAL;
				Save_Config_to_EEPROM();
				LED = 1;
			} if (RxInRoll > 30) {	// reverse(right)
				Config.RollGyroDirection = GYRO_REVERSED;
				Save_Config_to_EEPROM();
				LED = 1;
			} else if (RxInPitch < -30) { // normal(up)
				Config.PitchGyroDirection = GYRO_NORMAL;
				Save_Config_to_EEPROM();
				LED = 1;
			} else if (RxInPitch > 30) { // reverse(down)
				Config.PitchGyroDirection = GYRO_REVERSED;
				Save_Config_to_EEPROM();
				LED = 1;
			} else if (RxInYaw < -30) { // normal(left)
				Config.YawGyroDirection = GYRO_NORMAL;
				Save_Config_to_EEPROM();
				LED = 1;
			} else if (RxInYaw > 30) { // reverse(right)
				Config.YawGyroDirection = GYRO_REVERSED;
				Save_Config_to_EEPROM();
				LED = 1;
			}

			_delay_ms(50);
			LED = 0;

		}
	}

	// ESC throttle calibration
	if (GainInADC[YAW] < (UC_ADC_MAX*5)/100)		// less than 5%
	{
		// flash LED 3 times
		for (i=0;i<3;i++)
		{
			LED = 1;
			_delay_ms(25);
			LED = 0;
			_delay_ms(25);
		}

		Armed = true;	// override so that output_motor_pwm() won't quit early

		PWM_Low_Pulse_Interval = ((1000000UL / 50) - 2000)/10;	// set to 50Hz

#ifdef TRI_COPTER
		ServoPPMRateDivider = 1;	// since we have already set to 50Hz
#endif

		while (1)	// loop forever
		{
	
			RxGetChannels();

			MotorOut1 = RxInCollective;
			MotorOut2 = RxInCollective;
			MotorOut3 = RxInCollective;
#ifdef TRI_COPTER
			ServoPPMRateDivider = 1;	// This section already outputs @ 50Hz
			MotorOut4 = RxInYaw;
#else
			MotorOut4 = RxInCollective;
#endif
#if defined(HEX_COPTER) || defined(Y6_COPTER)
			MotorOut5 = RxInCollective;
			MotorOut6 = RxInCollective;
#endif
			output_motor_ppm();	// this regulates rate at which we output signals
		}
	}
}

void loop(void)
{
	static uint16_t Change_Arming=0;
	static uint8_t Arming_TCNT2=0;

	RxGetChannels();

	if ( RxInCollective < 0) {
		// check for stick arming (Timer2 @ 8MHz/1024 = 7812.5KHz)
		// arm: yaw right (>60), dis-arm: yaw left (<-60)
		Change_Arming += (uint8_t) (TCNT2 - Arming_TCNT2);
		Arming_TCNT2 = TCNT2;

#ifdef STICKARM_LEFT
		if (Armed) {
			if (RxInYaw<STICKARM_POINT) 	Change_Arming = 0;		// re-set count
		} else {
			if (RxInYaw>-STICKARM_POINT) 	Change_Arming = 0;		// re-set count
		}
#else
		if (!Armed) {	
			if (RxInYaw<STICKARM_POINT) 	Change_Arming = 0;		// re-set count
		} else {
			if (RxInYaw>-STICKARM_POINT) 	Change_Arming = 0;		// re-set count
		}
#endif
		// 3Sec / 0.000128 = 23437 = 0x5B8D or 
		// 2.5Sec / 0.000128 = 19531 = 0x4C4B
		if (Change_Arming>0x4C4B)
		{
			Armed = ! Armed;
			LED = 0;
			if (Armed) {
				output_motor_high = false;	// re-set 1st time flag
				CalibrateGyros();
				LED = 1;
			}	else if (output_motor_high) {
				output_motor_ppm();					// turn off
			}
			return;
		}
	}


	//--- Read gyros ---
	ReadGyros(false);

	//--- Start mixing by setting collective to motor input 1,2,3 and 4 ---
	if (RxInCollective > MAX_COLLECTIVE) RxInCollective = MAX_COLLECTIVE;
	MotorOut1 = RxInCollective;
	MotorOut2 = RxInCollective;
	MotorOut3 = RxInCollective;
#ifndef TRI_COPTER
	MotorOut4 = RxInCollective;
#endif
#if defined(HEX_COPTER) || defined(Y6_COPTER)
	MotorOut5 = RxInCollective;
	MotorOut6 = RxInCollective;
#endif
	//--- Calculate roll gyro output ---
	// nb IF YOU CHANGE THIS CODE, YOU MUST REMOVE PROPS BEFORE TESTING !!!
	gyroADC[ROLL] = gyroADC[ROLL] * GainIn[ROLL] * ROLL_GAIN_MULTIPLIER;
	gyroADC[ROLL] /=100;

#if ROLL_STICK_DIVIDER > 0
	RxInRoll = (RxInRoll >> ROLL_STICK_DIVIDER);		// scale stick output
#endif

	if (Config.RollGyroDirection == GYRO_NORMAL) {
		RxInRoll += gyroADC[ROLL];
	} else {
		RxInRoll -= gyroADC[ROLL];
	}

	//--- (Add)Adjust roll gyro output to motors
#ifdef TRI_COPTER
	RxInRoll	= (RxInRoll * 20)/23;	//RxInRollSine 60= 0.866 ~ 20/23 or possibly 7/8
	MotorOut1 += RxInRoll;
	MotorOut2 -= RxInRoll;
#elif defined(QUAD_COPTER)
	MotorOut2 += RxInRoll;
	MotorOut3 -= RxInRoll;
#elif defined(QUAD_X_COPTER)
	RxInRoll	= (RxInRoll >> 1);	//was:RxInRoll	= (RxInRoll * 20)/23;
	MotorOut1 += RxInRoll;
	MotorOut2 -= RxInRoll;
	MotorOut3 -= RxInRoll;
	MotorOut4 += RxInRoll;
#elif defined(HEX_COPTER)
	RxInRoll	= (RxInRoll * 20)/23;	//RxInRoll  *= 0.866;	// Sine 60
	MotorOut2 -= RxInRoll;
	MotorOut3 -= RxInRoll;
	MotorOut5 += RxInRoll;
	MotorOut6 += RxInRoll;
#elif defined(Y6_COPTER)
	RxInRoll	= (RxInRoll * 20)/23;	//RxInRoll  *= 0.866;	// Sine 60
	MotorOut1 += RxInRoll;
	MotorOut4 += RxInRoll;
	MotorOut2 -= RxInRoll;
	MotorOut5 -= RxInRoll;
#else
#error No Copter configuration defined !!!!
#endif

	//--- Calculate pitch gyro output ---
	// nb IF YOU CHANGE THIS CODE, YOU MUST REMOVE PROPS BEFORE TESTING !!!
  gyroADC[PITCH] = gyroADC[PITCH] * GainIn[PITCH] * PITCH_GAIN_MULTIPLIER;  
  gyroADC[PITCH] /=100;

#if PITCH_STICK_DIVIDER > 0
	RxInPitch = (RxInPitch >> PITCH_STICK_DIVIDER);		// scale stick output
#endif

	if (Config.PitchGyroDirection == GYRO_NORMAL) {	
		RxInPitch += gyroADC[PITCH];
	} else {	
		RxInPitch -= gyroADC[PITCH];
	}

	//--- (Add)Adjust pitch gyro output to motors
#ifdef TRI_COPTER
	MotorOut3 -= RxInPitch;
	RxInPitch = (RxInPitch >> 1);	// cosine of 60
	MotorOut1 += RxInPitch;
	MotorOut2 += RxInPitch;
#elif defined(QUAD_COPTER)
	MotorOut1 += RxInPitch;
	MotorOut4 -= RxInPitch;
#elif defined(QUAD_X_COPTER)
	RxInPitch	= (RxInPitch >> 1);	//23;	//Sine 60 = 0.866 20/23 = 0.869
	MotorOut1 += RxInPitch;
	MotorOut2 += RxInPitch;
	MotorOut3 -= RxInPitch;
	MotorOut4 -= RxInPitch;
#elif defined(HEX_COPTER)
	MotorOut1 += RxInPitch;
	MotorOut4 -= RxInPitch;
	RxInPitch = (RxInPitch >> 2);
	MotorOut2 += RxInPitch;	
	MotorOut3 -= RxInPitch;
	MotorOut5 -= RxInPitch;
	MotorOut6 += RxInPitch;
#elif defined(Y6_COPTER)
	MotorOut3 -= RxInPitch;
	MotorOut6 -= RxInPitch;
	RxInPitch = (RxInPitch >> 1);	// cosine of 60
	MotorOut1 += RxInPitch;
	MotorOut4 += RxInPitch;
	MotorOut2 += RxInPitch;
	MotorOut5 += RxInPitch;
#else
#error No Copter configuration defined !!!!
#endif


	//--- Calculate yaw gyro output ---
#ifdef TRI_COPTER

	GainAttenuator = RxInYaw;
	if (GainAttenuator < 0) GainAttenuator = - GainAttenuator;
	GainAttenuator -= 5;
	if (GainAttenuator < 0) GainAttenuator = 0;
	GainAttenuator = (GainAttenuator >> 2);				// div by 4
	GainAttenuator = 64 - GainAttenuator;
	if (GainAttenuator < 0) GainAttenuator = 0;
	GainAttenuator = (GainAttenuator >> 6);				// div by 64

	GainAttenuator *= GainIn[YAW];						// was 0->1023 now 0->102
	GainAttenuator *= YAW_GAIN_MULTIPLIER;		// x Gain (was 0.003) = x3/1000
	gyroADC[YAW] *= GainAttenuator;

	gyroADC[YAW] /= 100;						// so now /100 rather than /1000
	if (Config.YawGyroDirection == GYRO_NORMAL)	{	// scale gyro output
		RxInYaw += gyroADC[YAW];
	} else {
		RxInYaw -= gyroADC[YAW];
	}
	MotorOut4 = 50 + RxInYaw;
#else
	// nb IF YOU CHANGE THIS CODE, YOU MUST REMOVE PROPS BEFORE TESTING !!!
	gyroADC[YAW] = (gyroADC[YAW] * GainIn[YAW] * YAW_GAIN_MULTIPLIER);
	gyroADC[YAW] /=100;

#if YAW_STICK_DIVIDER > 0
	RxInYaw = (RxInYaw >> YAW_STICK_DIVIDER);		// scale stick output
#endif

	if (Config.YawGyroDirection == GYRO_NORMAL)	{		// scale gyro output
		RxInYaw += gyroADC[YAW];
	} else {
		RxInYaw -= gyroADC[YAW];
	}

	//--- (Add)Adjust yaw gyro output to motors
#ifdef QUAD_COPTER
	MotorOut1 -= RxInYaw;
	MotorOut2 += RxInYaw;
	MotorOut3 += RxInYaw;
	MotorOut4 -= RxInYaw;
#elif defined(QUAD_X_COPTER)
	MotorOut1 -= RxInYaw;
	MotorOut2 += RxInYaw;
	MotorOut3 -= RxInYaw;
	MotorOut4 += RxInYaw;
#elif defined(HEX_COPTER)
	MotorOut1 -= RxInYaw;
	MotorOut2 += RxInYaw;
	MotorOut3 -= RxInYaw;
	MotorOut4 += RxInYaw;
	MotorOut5 -= RxInYaw;
	MotorOut6 += RxInYaw;
#elif defined(Y6_COPTER)
	MotorOut1 -= RxInYaw;
	MotorOut2 -= RxInYaw;
	MotorOut3 -= RxInYaw;
	MotorOut4 += RxInYaw;
	MotorOut5 += RxInYaw;
	MotorOut6 += RxInYaw;
#else
#error No Copter configuration defined !!!!
#endif

#endif

	//--- Limit the lowest value to avoid stopping of motor if motor value is under-saturated ---
	if ( MotorOut1 < 10 )	MotorOut1 = 10;		// this is the motor idle level
	if ( MotorOut2 < 10 )	MotorOut2 = 10;	
	if ( MotorOut3 < 10 )	MotorOut3 = 10;
#ifndef TRI_COPTER
	if ( MotorOut4 < 10 )	MotorOut4 = 10;	
#endif
#if defined(HEX_COPTER) || defined(Y6_COPTER)
	if ( MotorOut5 < 10 )	MotorOut5 = 10;	
	if ( MotorOut6 < 10 )	MotorOut6 = 10;	
#endif	
	//--- Output to motor ESC's ---

	if (RxInCollective < 1 || !Armed || !GyroCalibrated)	// turn off motors if collective below 1% ???
	{																									// or  if gyros not calibrated
		MotorOut1 = 0;
		MotorOut2 = 0;
		MotorOut3 = 0;
#ifndef TRI_COPTER
		MotorOut4 = 0;
#else
		MotorOut4 = 50;
#endif
#if defined(HEX_COPTER) || defined(Y6_COPTER)
		MotorOut5 = 0;
		MotorOut6 = 0;
#endif
	}

	if (Armed) output_motor_ppm();		// output ESC signal
}

void Init_ADC(void)
{
	DIDR0 	= 0b00111111;		// Digital Input Disable Register - ADC5..0 Digital Input Disable
	ADCSRB 	= 0b00000000; 	// ADC Control and Status Register B - ADTS2:0
}

/*
 * This fuction populates following vars:
 * GainInADC[pot-name] holds the raw ADC values
 * GainIn[pot-name] = GainInADC[pot-name] * Gain+/-
 * 
 */
void ReadGainPots(void)
{
	read_adc( 3 );			// read roll gain ADC3
	GainInADC[ROLL] = ADCL;
	GainInADC[ROLL] += ((uint16_t) ADCH <<8);
	GainIn[ROLL] = GainInADC[ROLL] / 10;

	read_adc( 4 );			// read pitch gain ADC4
	GainInADC[PITCH] = ADCL;
	GainInADC[PITCH] += ((uint16_t) ADCH <<8);
	GainIn[PITCH] = GainInADC[PITCH] / 10;

	read_adc( 5 );			// read yaw gain ADC5
	GainInADC[YAW] = ADCL;
	GainInADC[YAW] += ((uint16_t) ADCH <<8);
	GainIn[YAW] = GainInADC[YAW] / 10;
}

void read_adc(uint8_t channel)
{
	ADMUX 	= channel;						// set channel
	ADCSRA 	= 0b11000110;					// ADEN, ADSC, ADPS1,2

	while (ADCSRA & (1 << ADSC));	// wait to complete

}

void ReadGyros(bool calibrate)
{
	read_adc( 2 );			// read roll gyro ADC2
	gyroADC[ROLL] = ADCL;
	gyroADC[ROLL] += ((uint16_t) ADCH <<8);
	if (!calibrate)	gyroADC[ROLL] 	-= gyroZero[ROLL];			//remove offset from gyro output

	read_adc( 1 );			// read pitch gyro ADC1
	gyroADC[PITCH] = ADCL;
	gyroADC[PITCH] += ((uint16_t) ADCH <<8);
	if (!calibrate)	gyroADC[PITCH] -= gyroZero[PITCH];

#ifdef EXTERNAL_YAW_GYRO
	gyroADC[YAW] = 0;
#else
	read_adc( 0 );			// read yaw gyro ADC0
	gyroADC[YAW] = ADCL;
	gyroADC[YAW] += ((uint16_t) ADCH <<8);
	if (!calibrate)	gyroADC[YAW] 	-= gyroZero[YAW];
#endif
}

void CalibrateGyros(void)
{
	uint8_t i;

	ReadGainPots();	// about time we did this !

	// get/set gyro zero value (average of 32 readings)
	gyroZero[ROLL] 	= 0;						
	gyroZero[PITCH] = 0;	
	gyroZero[YAW] 	= 0;

	for (i=0;i<32;i++)
	{
		ReadGyros(true);
	
		gyroZero[ROLL] 	+= gyroADC[ROLL];						
		gyroZero[PITCH] += gyroADC[PITCH];	
		gyroZero[YAW] 	+= gyroADC[YAW];
	}

	gyroZero[ROLL] 	= (gyroZero[ROLL] >> 5);						
	gyroZero[PITCH] = (gyroZero[PITCH] >> 5);
	gyroZero[YAW] 	= (gyroZero[YAW] 	>> 5);

	GyroCalibrated = true;

}


//--- Get and scale RX channel inputs ---
void RxGetChannels(void)
{
	static int16_t RxChannel;

	while ( RxChannelsUpdatingFlag );

	RxChannel = RxChannel1;
	RxChannel -= Config.RxChannel1ZeroOffset;				// normalise
	RxInRoll = (RxChannel >> 2);                    //     "

	while ( RxChannelsUpdatingFlag );

	RxChannel = RxChannel2;
	RxChannel -= Config.RxChannel2ZeroOffset;				// normalise
	RxInPitch = (RxChannel >> 2);                   //     "

	while ( RxChannelsUpdatingFlag );

	RxChannel = RxChannel3;
	RxChannel -= Config.RxChannel3ZeroOffset;				// scale 0->100
	RxInCollective = (RxChannel >> 3);              // 

	while ( RxChannelsUpdatingFlag );

	RxChannel = RxChannel4;
	RxChannel -= Config.RxChannel4ZeroOffset;				// normalise
	RxInYaw = (RxChannel >> 2);                     //     "

}

void output_motor_ppm(void)
{
	static uint8_t i;
	static uint16_t MotorStartTCNT1, ElapsedTCNT1;
	static int16_t MotorAdjust;
	static uint16_t PWM_Low_Count;
	static int8_t num_of_10uS;	
#ifdef TRI_COPTER
	static uint8_t ServoPPMRateCount;
#endif


	// if ESC's are high, we need to turn them off
	if (output_motor_high)
	{

		// set motor limits (0 -> 100)
		if ( MotorOut1 < 0 ) MotorOut1 = 0;
		else if ( MotorOut1 > 100 ) MotorOut1 = 100;
		if ( MotorOut2 < 0 ) MotorOut2 = 0;
		else if ( MotorOut2 > 100 ) MotorOut2 = 100;
		if ( MotorOut3 < 0 ) MotorOut3 = 0;
		else if ( MotorOut3 > 100 ) MotorOut3 = 100;
		if ( MotorOut4 < 0 ) MotorOut4 = 0;
		else if ( MotorOut4 > 100 ) MotorOut4 = 100;
#if defined(HEX_COPTER) || defined(Y6_COPTER)
		if ( MotorOut5 < 0 ) MotorOut5 = 0;
		else if ( MotorOut5 > 100 ) MotorOut5 = 100;
		if ( MotorOut6 < 0 ) MotorOut6 = 0;
		else if ( MotorOut6 > 100 ) MotorOut6 = 100;
#endif

		// now calculate the time already passed that Motors were HIGH
		ElapsedTCNT1 = (TCNT1 - MotorStartTCNT1);

		// start output timer
		TIFR0 &= ~(1 << TOV0);	// clr overflow
		TCNT0 = 0;							// reset counter

		// convert into 10uS intervals
		num_of_10uS = (ElapsedTCNT1 / 10) + 1;
		MotorAdjust = 100 - num_of_10uS;

		// add adjustment (1mS - time already gone) to all channels
		MotorOut1 += MotorAdjust;
		MotorOut2 += MotorAdjust;
		MotorOut3 += MotorAdjust;
		MotorOut4 += MotorAdjust;
#if defined(HEX_COPTER) || defined(Y6_COPTER)
		MotorOut5 += MotorAdjust;
		MotorOut6 += MotorAdjust;
#endif

		// keep signal on for correct time
		// MotorOutX = 100 -> 200
		// Pulse len = 1   -> 2    mS

		TIFR0 &= ~(1 << TOV0);	// clr overflow
		TCNT0 = 0;							// reset counter

		for (i=num_of_10uS;i<200;i++)	
		{
			while (TCNT0 < 80);		// 10uS @ 8MHz = 80 // 10 @ 1MHz = 10uS
			TCNT0 -= 80;

			if (MotorOut1) 
			{
				MotorOut1--;
				if (MotorOut1==0) M1 = 0;
			}
			if (MotorOut2) 
			{
				MotorOut2--;
				if (MotorOut2==0) M2 = 0;
			}
			if (MotorOut3) 
			{
				MotorOut3--;
				if (MotorOut3==0) M3 = 0;
			}
			if (MotorOut4) 
			{
				MotorOut4--;
				if (MotorOut4==0) M4 = 0;
			}
	#if defined(HEX_COPTER) || defined(Y6_COPTER)
			if (MotorOut5) 
			{
				MotorOut5--;
				if (MotorOut5==0) M5 = 0;
			}
			if (MotorOut6) 
			{
				MotorOut6--;
				if (MotorOut6==0) M6 = 0;
			}
	#endif
		}

		//Now wait low signal interval
		PWM_Low_Count = PWM_Low_Pulse_Interval - 1;

		TIFR0 &= ~(1 << TOV0);	// clr overflow
		TCNT0 = 0;							// reset counter

		while (PWM_Low_Count--)
		{
			while (TCNT0 < 80);		// 20 @ 2MHz = 10uS
			TCNT0 -= 80;
		}
	}

	if (! Armed) return;

	// Log PWM signal HIGH	
  MotorStartTCNT1 = TCNT1;
	output_motor_high = true;

	// turn on pins
	M1 = 1;
	M2 = 1;
	M3 = 1;
#ifdef TRI_COPTER
	if(ServoPPMRateCount==ServoPPMRateDivider)
	{
		M4 = 1;
		ServoPPMRateCount = 1;
	} else {
		ServoPPMRateCount++;
	}
#else
	M4 = 1;
#endif
#if defined(HEX_COPTER) || defined(Y6_COPTER)
	M5 = 1;
	M6 = 1;
#endif

}

void eeprom_write_byte_changed( uint8_t * addr, uint8_t value )
{ 
	if(eeprom_read_byte(addr) != value)
	{
		eeprom_write_byte( addr, value );
	}
}

void eeprom_write_block_changes( const uint8_t * src, void * dest, size_t size )
{ 
	size_t len;

	for(len=0;len<size;len++)
	{
		eeprom_write_byte_changed( dest,  *src );

		src++;
		dest++;
	}
}

void Initial_EEPROM_Config_Load(void)
{
	// load up last settings from EEPROM
	if(eeprom_read_byte((uint8_t*) EEPROM_DATA_START_POS )!=0x47)
	{
		Config.setup = 0x47;
		Set_EEPROM_Default_Config();
		// write to eeProm
		Save_Config_to_EEPROM();
	} else {
		// read eeprom
		eeprom_read_block(&Config, (void*) EEPROM_DATA_START_POS, sizeof(CONFIG_STRUCT)); 
	}
}

void Set_EEPROM_Default_Config(void)
{
	Config.RollGyroDirection 		= GYRO_REVERSED;
	Config.PitchGyroDirection		= GYRO_REVERSED;
	Config.YawGyroDirection			= GYRO_NORMAL;

	Config.RxChannel1ZeroOffset	= 1520;
	Config.RxChannel2ZeroOffset	= 1520;
	Config.RxChannel3ZeroOffset	= 1120;
	Config.RxChannel4ZeroOffset	= 1520;
}

void Save_Config_to_EEPROM(void)
{
	// write to eeProm
	cli();
	eeprom_write_block_changes( (const void*) &Config, (void*) EEPROM_DATA_START_POS, sizeof(CONFIG_STRUCT));	//current_config CONFIG_STRUCT
	sei();
}
