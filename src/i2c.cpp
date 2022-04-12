


#include "i2c.h"
#include <avr/io.h>
#include <Arduino.h>

void initI2C(){
  // wake up from power reduction:   if the I2C is in power reduction mode (PRTWI bit is 1) 
  //then it needs to be restored by writing a zero to PRTWI.
  
  PRR0 &= ~(1 << PRTWI);  //On pg. 236 of ATmega2560 Datasheet   
  
  // generate a 100kHz clock rate
  // SCL freq = 16Mhz/(16 + 2(TWBR)*4)
  // Table 24-7 Prescaler value is 1 so TWPS0 = 0 and TWPS1 = 0.

  // The example code uses TWPS0 =1 and TWPS1 = 0 for a prescaler of 1
  // I checked the data sheet though and this seems to be correct. -Jackson
  TWSR &= ~((1 << TWPS1) | (1 << TWPS0));
  TWBR = 0x12; // bit rate generator = 100k  (TWBR = 18)

  TWCR |= (1 << TWINT | 1 << TWEN); // enable two wire interface
}

void startI2C_Trans(unsigned char SLA){
  // Someone check me on this -Patrick
  
  TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN); // Clear TWINT, initiate start condition, initiate enable
  while(!(TWCR & (1 << TWINT))); // wait for flag to change
  TWDR = (SLA << 1) & 0xFE; // load address and write bit
  TWCR = (1<<TWINT)|(1<<TWEN);  // clear flag and enable TWI
  while (!(TWCR & (1<<TWINT))); // wait for flag to set
  
}

void stopI2C_Trans(){
  // this function sends a stop condition to stop I2C transmission
  TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO); //trigger action:  send stop condition
  while (!(TWCR & (1<<TWINT))); // wait for flag to set
}

void write(unsigned char data){
  // this function loads the data passed into the I2C data register and transmits
  TWDR = data; //load data into TWDR register
  TWCR = (1<<TWINT)|(1<<TWEN);  // trigger action:  clear flag and enable TWI
  while (!(TWCR & (1<<TWINT))); // wait for flag to set
}

void Read_From(unsigned char SLA, unsigned char MEMADDRESS){
  // this function sets up reading from SLA at the SLA MEMADDRESS 
  startI2C_Trans(SLA);
  write(MEMADDRESS);
  TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN); // restart to switch to read mode
  while (!(TWCR & (1<<TWINT))); // wait for flag to set
  TWDR = (SLA << 1) | 0x01; // 7 bit address for slave plus read bit
  TWCR = (1 << TWINT) | (1 << TWEN)| (1 << TWEA);// trigger with master sending ack
  while (!(TWCR & (1<<TWINT))); // wait for flag to set
  TWCR = (1<< TWINT) | (1 << TWEN);  // master can send a nack now
  while (!(TWCR & (1<<TWINT))); // wait for flag to set
  TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO); // Stop condition
// after this function is executed the TWDR register has the data from SLA that Master wants to read
}

unsigned char Read_Data(){
    return TWDR;//11010001
}
