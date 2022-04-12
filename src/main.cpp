// Author: Collin Preszler
// Net ID: collinpreszler
// Date: 2/18/2022
// Assignment:     Lab 3
//
// Description: This file contains a programmatic overall description of the
// program. It should never contain assignments to special function registers
// for the exception key one-line code such as checking the state of the pin.
//
// Requirements:
//----------------------------------------------------------------------//

#include <Arduino.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "pwm.h"
#include "switch.h"
#include "timer.h"
#include "spi.h"
#include "i2c.h"
#include "accelGyro.h"

// definitions
#define LONG_DELAY 200
#define SHORT_DELAY 100

/*
 * Define a set of states that can be used in the state machine using an enum.
 * States: Waiting for press, Press debounce, Waiting for release, Release debounce
 */
enum state{wp,pd,wr,rd};
/*
  * Define a set of states for the LED array output using an enum.
  * States: Smiley face, Frowny face
  */
enum LEDState{smile,frown};
// Initialize states.  Remember to use volatile 
volatile state button = wp;
volatile LEDState led = smile;

int main(){
  DDRB&=~(1<<DDB7);
  PORTB|=(1<<PORTB7);
  Serial.begin(9600);
  Serial.println("Starting...");
  Serial.println("Initializing LEDs");
  initLEDSPI();
  // initialize 8 x 8 LED array (info from MAX7219 datasheet)
  writeToMatrix(0x0A, 0x08);  // brightness control
  writeToMatrix(0x0B, 0x07); // scanning all rows and columns
  writeToMatrix(0x0C,0x01); // set shutdown register to normal operation (0x01)
  writeToMatrix(0x0F, 0x00); // display test register - set to normal operation (0x01)
  Serial.println("Initializing timer 1");
  initTimer1();
  Serial.println("Initializing Switch");
  initSwitchPB3();
  Serial.println("Initializing PWM");
  initPWM_Pins();
  Serial.println("new Initializing I2C");
  initI2C();
  Serial.println("Initializing Accelerometer");
  //wakeUp();
  //initAccelerometer();
  Serial.println("Initializing Gyroscope");
  //initGyroscope();
  Serial.println("Done Initializing");
  sei(); // Enable global interrupts.

  char count = 0;
// while loop
  while(1){
    Serial.flush();
    delayMs(1000);
    //eightBitCommandWithDelay(0b00011100,40);
    switch(button){
      case wp: // Don't change anything if the program is waiting for a button press
        break;
      case pd:
        delayMs(1); // Give some time to debounce and then change the state
        button = wr;
        break;
      case wr:
        break; // Again, if the button is still pressed we don't really want to change anything
      case rd:
        delayMs(1); // More debouncing time before saying the button is fully unreleased
        button = wp;
        // Turn off buzzer
	      IncFrequency(1); // Can't divide by zero
        break;
    }
    switch(led){
      case smile:
        //Make LEDs smile
	writeToMatrix(0x01, 0b00000000);
	writeToMatrix(0x02, 0b00100100);
	writeToMatrix(0x03, 0b00100100);
	writeToMatrix(0x04, 0b00000000);
	writeToMatrix(0x05, 0b00000000);
	writeToMatrix(0x06, 0b01000010);
	writeToMatrix(0x07, 0b00111100);
	writeToMatrix(0x08, 0b00000000);
      if(PINB & (1<<PORTB7)){
        led = frown;
      }
        break;
      case frown:
        //Make LEDs frown
	writeToMatrix(0x01, 0b00000000);
	writeToMatrix(0x02, 0b00100100);
	writeToMatrix(0x03, 0b00100100);
	writeToMatrix(0x04, 0b00000000);
	writeToMatrix(0x05, 0b00000000);
	writeToMatrix(0x06, 0b00111100);
	writeToMatrix(0x07, 0b01000010);
	writeToMatrix(0x08, 0b00000000);
      if(!(PINB & (1<<PORTB7))){
        led = smile;
      }
        break;
    }
    count++;
    //print the gyro values
    Serial.print("Gyroscope: x=");
    Serial.print(getGyroX());
    Serial.print(" y=");
    Serial.print(getGyroY());
    Serial.print(" z=");
    Serial.println(getGyroZ());
    Serial.print("Accelerometer: x=");
    Serial.print(getAccelX());
    Serial.print(" y=");
    Serial.print(getAccelY());
    Serial.print(" z=");
    Serial.println(getAccelZ());
	}
  return 0;
}

/* Implement an Pin Change Interrupt which handles the switch being
* pressed and released. When the switch is pressed and released, the LEDs
* change at twice the original rate. If the LEDs are already changing at twice
* the original rate, it goes back to the original rate.
*/
ISR(PCINT0_vect){
  // Interrupt triggered while main loop is in state waiting for press, need to debounce
  // so put state into debounce to allow a 1 ms debounce state
  if (button == wp){  
    button = pd;
  }
  // Interrupt triggered when waiting for release of button, so first enter debouncing
  // state again and change the delay
  else if (button == wr){
    button = rd;
  }
}
