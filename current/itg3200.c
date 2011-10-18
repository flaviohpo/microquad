#include "itg3200.h"
#include "i2c.h"

char ITGFound = 0;

//Initialize the i2c communication and set the gyro to full scale range and 100 Hz update rate
void ITG_init(void){
    i2c_change_address(ITG_I2C_ADDR);
    //Set internal clock to 1kHz with 42Hz LPF and Full Scale to 3 for proper operation
    i2c_write_byte(ITG_DLPF_FS, ITG_DLPF_FS_SEL_0|ITG_DLPF_FS_SEL_1|ITG_DLPF_CFG_0);

    //Set sample rate divider for 100 Hz operation
    i2c_write_byte(ITG_SMPLRT_DIV, 9);   //Fsample = Fint / (divider + 1) where Fint is 1kHz

    //Setup the interrupt to trigger when new data is ready.
    i2c_write_byte(ITG_INT_CFG, ITG_INT_CFG_RAW_RDY_EN | ITG_INT_CFG_ITG_RDY_EN);    // interruption em P2.0
        
    //Select X gyro PLL for clock source
    i2c_write_byte(ITG_PWR_MGM, ITG_PWR_MGM_CLK_SEL_0);
}

//Read a register value from the gyro
//pre: register_addr is the register address to read
//     value is a pointer to an integer
//post: value contains the value of the register that was read
//returns: 1-Success
//         TWSR-Failure (Check out twi.h for TWSR error codes)
//usage: status = gyro.read(DEVID, &value); //value is created as an 'int' in main.cpp
char ITG3200_read(char register_addr, char * value){
    char result = 0;
    
    return result;
}

//Write a value to a register
//pre: register_addre is the register to write to
//     value is the value to place in the register
//returns: 1-Success
//         TWSR- Failure
//usage status=gyro.write(register_addr, value);
char ITG3200_write(char register_addr, char value){
    char result = 0;
    
    return result;
}

/*
get functions return the g value of the specified axis
usage: printf("Xg = %1.3fg", (double)gyro.getX()
*/

int ITG3200_getX(void){
    int result = 0;
    /*
    xr = (float)x/14.375;
    */
    return result;
}

int ITG3200_getY(void){
    int result = 0;
    /*
    yr = (float)y/14.375;
    */
    return result;
}

int ITG3200_getZ(void){
    int result = 0;
    /*
    zr = (float)z/14.375;
    */
    return result;
}

int ITG3200_getTemp(void){
    int result = 0;
    /*
    temp = -13200-temp; //Get the offset temp
    tempr = (float)temp/280;    //Convert the offset to degree C
    tempr += 35;    //Add 35 degrees C to compensate for the offset
    */
    return result;
}