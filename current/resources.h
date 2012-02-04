#include "shorttypes.h"

// STRINGS
const char* str_graph         = "GRPH";
const char* str_return        = "RETN";
const char* str_calibrate_throttle = "CATH";

const char* str_main_menu                = "MAIN MENU";
const char* str_analog_menu              = "ANALOG VIEW";
const char* str_radio_menu               = "RADIO VIEW";
const char* str_control_menu             = "CONTROL CONFIG.";
const char* str_motor_menu               = "MOTOR CONTROL";
const char* str_sensor_menu              = "SENSOR VIEW";
const char* str_option_menu              = "OPTIONS";
const char* str_vibration_analyzer_menu  = "VIBRATION TEST";
const char* str_voltage_drop_menu        = "BATTERY TEST";
const char* str_throttle_calibration_menu= "THROTTLE CALIBRATION";
const char* str_letsfly                  = "FLY!";

const char* str_rstdefault    = "RESET SET.";
const char* str_grava         = "SAVE  SET.";

const char* str_analogch0     = "ACH0";
const char* str_analogch1     = "ACH1";
const char* str_analogch2     = "ACH2";
const char* str_analogch3     = "ACH3";
const char* str_analogch4     = "ACH4";
const char* str_analogch5     = "ACH5";
const char* str_analogch6     = "ACH6";
const char* str_analogch7     = "ACH7";

const char* str_analogof0     = "OFF0";
const char* str_analogof1     = "OFF1";
const char* str_analogof2     = "OFF2";
const char* str_analogof3     = "OFF3";
const char* str_analogof4     = "OFF4";
const char* str_analogof5     = "OFF5";
const char* str_analogof6     = "OFF6";
const char* str_analogof7     = "OFF7";

const char* str_radioch0      = "CH0 ";
const char* str_radioch1      = "CH1 ";
const char* str_radioch2      = "CH2 ";
const char* str_radioch3      = "CH3 ";
const char* str_radioch4      = "CH4 ";
const char* str_radioch5      = "CH5 ";
const char* str_radioch6      = "CH6 ";
const char* str_radioch7      = "CH7 ";

const char* str_radioof0      = "OFF0";
const char* str_radioof1      = "OFF1";
const char* str_radioof2      = "OFF2";
const char* str_radioof3      = "OFF3";
const char* str_radioof4      = "OFF4";
const char* str_radioof5      = "OFF5";
const char* str_radioof6      = "OFF6";
const char* str_radioof7      = "OFF7";

const char* str_proportional  = "P   ";
const char* str_derivative    = "D   ";
const char* str_integral      = "I   ";

const char* str_yaw           = "YAW  ";
const char* str_pitch         = "PITCH";
const char* str_roll          = "ROLL ";

const char* str_motor1        = "ESC1";
const char* str_motor2        = "ESC2";
const char* str_motor3        = "ESC3";
const char* str_motor4        = "ESC4";
const char* str_motor5        = "ESC5";
const char* str_motor6        = "ESC6";
const char* str_calibrate     = "CALI";

const char* str_yawrefmul     = "YRMU";
const char* str_pitchrefmul   = "PRMU";
const char* str_rollrefmul    = "RRMU";

const char* str_yawrefdiv     = "YRDI";
const char* str_pitchrefdiv   = "PRDI";
const char* str_rollrefdiv    = "RRDI";

const char* str_yawpm         = "YPMU";
const char* str_pitchpm       = "PPMU";
const char* str_rollpm        = "RPMU";

const char* str_yawpd         = "YPDI";
const char* str_pitchpd       = "PPDI";
const char* str_rollpd        = "RPDI";

const char* str_accelx        = "ACCX";
const char* str_accely        = "ACCY";
const char* str_accelz        = "ACCZ";

const char* str_gyrox         = "GYRX";
const char* str_gyroy         = "GYRY";
const char* str_gyroz         = "GYRZ";

const char* str_throttle      = "THRO";

// accel max values during vibration test
const char* str_throttle_max  = "ThrH";
const char* str_throttle_min  = "ThrL";
const char* str_accelx_max    = "HacX";
const char* str_accely_max    = "HacY";
const char* str_accelz_max    = "HacZ";
// gyro max values during vibration test
const char* str_gyrox_max     = "HgyX";
const char* str_gyroy_max     = "HgyY";
const char* str_gyroz_max     = "HgyZ";
// motor output max values during vibration test
const char* str_motor1_max    = "HMo1";
const char* str_motor2_max    = "HMo2";
const char* str_motor3_max    = "HMo3";
const char* str_motor4_max    = "HMo4";
// control output
const char* str_roll_max      = "HCRo";
const char* str_pitch_max     = "HCPi";
const char* str_yaw_max       = "HCYa";
// accel max values during vibration test
const char* str_accelx_min    = "LacX";
const char* str_accely_min    = "LacY";
const char* str_accelz_min    = "LacZ";
// gyro max values during vibration test
const char* str_gyrox_min     = "LgyX";
const char* str_gyroy_min     = "LgyY";
const char* str_gyroz_min     = "LgyZ";
// motor output max values during vibration test
const char* str_motor1_min    = "LMo1";
const char* str_motor2_min    = "LMo2";
const char* str_motor3_min    = "LMo3";
const char* str_motor4_min    = "LMo4";
// vdrop menu
const char* str_battery_max   = "BatH";
const char* str_battery       = "Bat ";
const char* str_battery_min   = "BatL";
// control output
const char* str_roll_min      = "LCRo";
const char* str_pitch_min     = "LCPi";
const char* str_yaw_min       = "LCYa";

const char* _str_calibrando_esc = "CALIBRANDO ESC...";
const char* _str_programando_esc = "PROGRAMANDO ESC...";
const char* _str_inicializando = "INCIALIZANDO...\nNAO MEXA A IMU!\nO RADIO DEVE ESTAR\nLIGADO.\n";
const char* _str_pontinho = ".";

/*******************************/
// CONST VALUES
const int val_zero            = 0;
const int val_one             = 1;
const int val_max_control     = 255;
const int val_max_motor       = 4000;
const int val_min_motor       = 2000;
const int val_int_motor       = 25;
const int val_max_radio       = 4000;
const int val_min_radio       = 2000;
const int val_max_analog      = 4095;

const uchar val_janela_size_small     = 4;
const uchar val_janela_size_medium    = 8;
const uchar val_janela_size_large     = 12;
const uchar val_bar_lenght_small      = 15;
const uchar val_bar_lenght_medium     = 30;
const uchar val_bar_lenght_large      = 45;
const uchar val_arrow_left            = 6;
const uchar val_arrow_center          = 9;
const uchar val_arrow_right           = 13;
const uchar val_bar_position_left     = 30;
const uchar val_bar_position_center   = 50;
const uchar val_bar_position_right    = 70;
const uchar val_value_position_right  = 15;
const uchar val_value_position_center = 7;
const int val_max_int                 = 35767;
const int val_min_int                 = -35767;
const uint val_max_uint               = 65534;