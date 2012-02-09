#include "itg3200.h"
#include "i2c.h"

unsigned char ITG3200Found = 0;
unsigned char ITG3200Done = 0;

unsigned char ITG3200_init(void){
    unsigned char status = 0;
    
    i2c_change_slaveaddress(ITG_I2C_ADDR);
    
    status |= i2c_write_one_data(ITG_DLPF_FS, ITG_DLPF_FS_SEL_0|ITG_DLPF_FS_SEL_1);

    status |= i2c_write_one_data(ITG_SMPLRT_DIV, 12); // 8000 / 12 = 666Hz sample rate

    status |= i2c_write_one_data(ITG_INT_CFG, ITG_INT_CFG_RAW_RDY_EN);    // interruption em P2.0
        
    status |= i2c_write_one_data(ITG_PWR_MGM, ITG_PWR_MGM_CLK_SEL_0);
    
    ITG3200Done = !status;
    
    return status;
}

unsigned char ITG3200_read(unsigned char register_addr, unsigned char * value){
    i2c_change_slaveaddress(ITG_I2C_ADDR);    
    return i2c_wread_one_data(register_addr, value);
}

unsigned char ITG3200_write(unsigned char register_addr, unsigned char value){
    return i2c_write_one_data(register_addr, value);
}

unsigned char ITG3200_getX(int *valor){
    unsigned char status = 0, aux1, aux2;
        
    status = i2c_wread_one_data(ITG_GYRO_XOUT_H, &aux1);
    status |= i2c_wread_one_data(ITG_GYRO_XOUT_L, &aux2);
    *valor = ((char)aux1<<8)|aux2;
    
    return status;
}

unsigned char ITG3200_getY(int *valor){
    unsigned char status = 0, aux1, aux2;
        
    status = i2c_wread_one_data(ITG_GYRO_YOUT_H, &aux1);
    status |= i2c_wread_one_data(ITG_GYRO_YOUT_L, &aux2);
    *valor = ((char)aux1<<8)|aux2;
    
    return status;
}

unsigned char ITG3200_getZ(int *valor){
    unsigned char status = 0, aux1, aux2;
        
    status = i2c_wread_one_data(ITG_GYRO_ZOUT_H, &aux1);
    status |= i2c_wread_one_data(ITG_GYRO_ZOUT_L, &aux2);
    *valor = ((char)aux1<<8)|aux2;
    
    return status;
}

unsigned char ITG3200_getTemp(int *valor){
    unsigned char status = 0, aux1, aux2;
        
    status = i2c_wread_one_data(ITG_TEMP_OUT_H, &aux1);
    status |= i2c_wread_one_data(ITG_TEMP_OUT_L, &aux2);
    *valor = ((char)aux1<<8)|aux2;
    
    return status;
}