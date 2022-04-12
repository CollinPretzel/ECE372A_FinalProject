// Description: Function prototypes for switch.cpp
//----------------------------------------------------------------------//

#ifndef I2C_H
#define I2C_H

void initI2C();
void startI2C_Trans(unsigned char SLA);
void stopI2C_Trans();
void write(unsigned char data);
void Read_From(unsigned char SLA, unsigned char MEMADDRESS);
unsigned char Read_Data();

#endif