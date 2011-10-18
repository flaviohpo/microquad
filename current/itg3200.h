#define ITG_I2C_ADDR 0xD0 //0xD0 if tied low, 0xD2 if tied high

#define ITG_WHO_AM_I    0x00
#define ITG_SMPLRT_DIV  0x15
#define ITG_DLPF_FS     0x16
#define ITG_INT_CFG     0x17
#define ITG_INT_STATUS  0x1A
#define ITG_TEMP_OUT_H  0x1B
#define ITG_TEMP_OUT_L  0x1C
#define ITG_GYRO_XOUT_H 0x1D
#define ITG_GYRO_XOUT_L 0x1E
#define ITG_GYRO_YOUT_H 0x1F
#define ITG_GYRO_YOUT_L 0x20
#define ITG_GYRO_ZOUT_H 0x21
#define ITG_GYRO_ZOUT_L 0x22
#define ITG_PWR_MGM     0x3E

//Sample Rate Divider
//Fsample = Fint / (divider + 1) where Fint is either 1kHz or 8kHz
//Fint is set to 1kHz
//Set divider to 9 for 100 Hz sample rate

//DLPF, Full Scale Register Bits
//FS_SEL must be set to 3 for proper operation
//Set DLPF_CFG to 3 for 1kHz Fint and 42 Hz Low Pass Filter
#define ITG_DLPF_CFG_0  (1<<0)
#define ITG_DLPF_CFG_1  (1<<1)
#define ITG_DLPF_CFG_2  (1<<2)
#define ITG_DLPF_FS_SEL_0   (1<<3)
#define ITG_DLPF_FS_SEL_1   (1<<4)

//Power Management Register Bits
//Recommended to set CLK_SEL to 1,2 or 3 at startup for more stable clock
#define ITG_PWR_MGM_CLK_SEL_0   (1<<0)
#define ITG_PWR_MGM_CLK_SEL_1   (1<<1)
#define ITG_PWR_MGM_CLK_SEL_2   (1<<2)
#define ITG_PWR_MGM_STBY_Z  (1<<3)
#define ITG_PWR_MGM_STBY_Y  (1<<4)
#define ITG_PWR_MGM_STBY_X  (1<<5)
#define ITG_PWR_MGM_SLEEP   (1<<6)
#define ITG_PWR_MGM_H_RESET (1<<7)

//Interrupt Configuration Bist
#define ITG_INT_CFG_ACTL    (1<<7)
#define ITG_INT_CFG_OPEN    (1<<6)
#define ITG_INT_CFG_LATCH_INT_EN    (1<<5)
#define ITG_INT_CFG_INT_ANYRD   (1<<4)
#define ITG_INT_CFG_ITG_RDY_EN  (1<<2)
#define ITG_INT_CFG_RAW_RDY_EN  (1<<0)

extern char ITGFound;

void ITG_init();
char ITG_read(char register_addr, char *value);
char ITG_write(char register_addr, char value);

//Return the values in engineering units (degrees/second and degrees C)
int ITG_getX(void);
int ITG_getY(void);
int ITG_getZ(void);
int ITG_getTemp(void);
