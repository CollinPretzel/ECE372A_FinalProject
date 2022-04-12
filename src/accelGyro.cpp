#include "accelGyro.h"
#include "i2c.h"
#include <Arduino.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#define SLA 0x68  // MPU_6050 address with PIN AD0 grounded
#define PWR_MGMT  0x6B
#define WAKEUP 0x00
#define ACCEL_X_HIGH  0x3B
#define ACCEL_X_LOW   0x3C
#define ACCEL_Y_HIGH  0x3D
#define ACCEL_Y_LOW   0x3E
#define ACCEL_Z_HIGH  0x3F
#define ACCEL_Z_LOW   0x40
#define GYRO_X_HIGH  0x43
#define GYRO_X_LOW   0x44
#define GYRO_Y_HIGH  0x45
#define GYRO_Y_LOW   0x46
#define GYRO_Z_HIGH  0x47
#define GYRO_Z_LOW   0x48


#define ACCEL_CONFIG 0x1C
#define GYRO_CONFIG 0x1B

void initAccelerometer(){
    // page 15 of data sheet
    Read_From(SLA,ACCEL_CONFIG);
    int prevValue = Read_Data();
    prevValue|=(1<<4);
    prevValue&=~(1<<3);
    startI2C_Trans(SLA);
    write(prevValue);
    stopI2C_Trans();
    //Set bits 4 and 3 to 10 for +- 8g range
}

void initGyroscope(){
    // page 14 of data sheet
    Read_From(SLA,GYRO_CONFIG);
    int prevValue = Read_Data();
    prevValue|=(1<<4);
    prevValue&=~(1<<3);
    startI2C_Trans(SLA);
    write(prevValue);
    stopI2C_Trans();
    //Set bits 4 and 3 to 10 for +- 1000 degree range
}

void wakeUp(){
    startI2C_Trans(SLA);
    write(PWR_MGMT);
    stopI2C_Trans();
    startI2C_Trans(SLA);
    write(WAKEUP);
    stopI2C_Trans();
}
int getAccelX(){
    int dat = 0;
    Read_From(SLA,ACCEL_X_HIGH);
    dat = Read_Data();
    Read_From(SLA,ACCEL_X_LOW);
    dat = (dat << 8)|Read_Data();
    return dat;
}
int getAccelY(){
    int dat = 0;
    Read_From(SLA,ACCEL_Y_HIGH);
    dat = Read_Data();
    Read_From(SLA,ACCEL_Y_LOW);
    dat = (dat << 8)|Read_Data();
    return dat;
}
int getAccelZ(){
    int dat = 0;
    Read_From(SLA,ACCEL_Z_HIGH);
    dat = Read_Data();
    Read_From(SLA,ACCEL_Z_LOW);
    dat = (dat << 8)|Read_Data();
    return dat;
}
int getGyroX(){
    int dat = 0;
    Read_From(SLA,GYRO_X_HIGH);
    dat = Read_Data();
    Read_From(SLA,GYRO_X_LOW);
    dat = (dat << 8)|Read_Data();
    return dat;
}
int getGyroY(){
    int dat = 0;
    Read_From(SLA,GYRO_Y_HIGH);
    dat = Read_Data();
    Read_From(SLA,GYRO_Y_LOW);
    dat = (dat << 8)|Read_Data();
    return dat;
}
int getGyroZ(){
    int dat = 0;
    Read_From(SLA,GYRO_Z_HIGH);
    dat = Read_Data();
    Read_From(SLA,GYRO_Z_LOW);
    dat = (dat << 8)|Read_Data();
    return dat;
}