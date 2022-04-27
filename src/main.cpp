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
#include "microphone.h"

// definitions
#define LONG_DELAY 200
#define SHORT_DELAY 100

#define SAMPLE_RATE 325000
#define NUM_NOTES 20

unsigned char noteHistory[NUM_NOTES*2];

// ADC variables
byte newData = 0;
byte prevData = 0;
unsigned int time = 0;//keeps time and sends vales to store in timer[] occasionally
int timer[10];//sstorage for timing of events
int slope[10];//storage for slope of events
unsigned int totalTimer;//used to calculate period
volatile unsigned int period;//storage for period of wave
byte index = 0;//current storage index
float frequency;//storage for frequency calculations
int maxSlope = 0;//used to calculate max slope as trigger point
int newSlope;//storage for incoming slope data
// ADC slope match variables
byte noMatch = 0;//counts how many non-matches you've received to reset variables if it's been too long
byte slopeTol = 3;//slope tolerance (adjustable)
int timerTol = 10;//timer tolerance (adjustable)

//variables for amp detection
unsigned int ampTimer = 0;
byte maxAmp = 0;
byte checkMaxAmp;
byte ampThreshold = 30;//raise if you have a very noisy signal

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

byte prevData = 0, newData = 0;
boolean clipping = 0;

int main(){
  for(int i=0;i<NUM_NOTES*2;i++){
    noteHistory[i] = -1;
  }
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
  //initI2C();
  Serial.println("Initializing Accelerometer");
  //wakeUp();
  //initAccelerometer();
  Serial.println("Initializing Gyroscope");
  //initGyroscope();
  Serial.println("Done Initializing");
  sei(); // Enable global interrupts.
  bool playing = false;
  char count = 0;
// while loop
  while(1){
    Serial.flush();
    delayMs(1000);
    //eightBitCommandWithDelay(0b00011100,40);
    if(PINA & (1<<PB2)){//change to the pin for the replay button
      playing = true;
      char curNote = noteHistory[0];
      char curOctave = noteHistory[1];
      //Display current note on the LED array
      //Display note history on LCD
      //play current note on the speaker for a little bit
      for(int i=0;i<NUM_NOTES*2-2;i++){//remove the first note from the history
        noteHistory[i] = noteHistory[i+2];
      }
    } else {
      if(playing){
        for(int i=0;i<NUM_NOTES*2;i++){
          noteHistory[i] = -1;
        }
      }
      playing = false;
    }
    if(PINA&(1<<PB3) && !playing){//change to the pin for the record button
      frequency = SAMPLE_RATE/period;
      Serial.print("Frequency: ");
      Serial.println(frequency);
      unsigned char octave = 0;
      unsigned char note = 0;
      freqID(&note,&octave,frequency);
      Serial.print("Note: ");
      Serial.print(note);
      Serial.print(" Octave: ");
      Serial.println(octave);
      if(noteHistory[0] != note || noteHistory[1] != octave){//Save old notes to history if it's a new note
        for(int i=2;i<NUM_NOTES*2;i++){
          noteHistory[i] = noteHistory[i-2];
        }
        noteHistory[0]=note;
        noteHistory[1]=octave;
      }
      //Print note to the ledMatrix
      //Print note history to LCD
    }
    
    // needed if we want an indicator light that for when sound is out of range
    if (clipping){//if currently clipping
      PORTB &= B11011111;//turn off clipping indicator led
      clipping = 0;
    }

    return 0;
  }
}

/* Implement an Pin Change Interrupt which handles the switch being
* pressed and released. When the switch is pressed and released, the LEDs
* change at twice the original rate. If the LEDs are already changing at twice
* the original rate, it goes back to the original rate.
*/
ISR(ADC_vect) {//when new ADC value ready
  
  PORTB &= B11101111;//set pin 12 low
  prevData = newData;//store previous value
  newData = ADCH;//get value from A0
  if (prevData < 127 && newData >=127){//if increasing and crossing midpoint
    newSlope = newData - prevData;//calculate slope
    if (abs(newSlope-maxSlope)<slopeTol){//if slopes are ==
      //record new data and reset time
      slope[index] = newSlope;
      timer[index] = time;
      time = 0;
      if (index == 0){//new max slope just reset
        PORTB |= B00010000;//set pin 12 high
        noMatch = 0;
        index++;//increment index
      }
      else if (abs(timer[0]-timer[index])<timerTol && abs(slope[0]-newSlope)<slopeTol){//if timer duration and slopes match
        //sum timer values
        totalTimer = 0;
        for (byte i=0;i<index;i++){
          totalTimer+=timer[i];
        }
        period = totalTimer;//set period
        //reset new zero index values to compare with
        timer[0] = timer[index];
        slope[0] = slope[index];
        index = 1;//set index to 1
        PORTB |= B00010000;//set pin 12 high
        noMatch = 0;
      }
      else{//crossing midpoint but not match
        index++;//increment index
        if (index > 9){
          reset();
        }
      }
    }
    else if (newSlope>maxSlope){//if new slope is much larger than max slope
      maxSlope = newSlope;
      time = 0;//reset clock
      noMatch = 0;
      index = 0;//reset index
    }
    else{//slope not steep enough
      noMatch++;//increment no match counter
      if (noMatch>9){
        reset();
      }
    }
  }
    
  if (newData == 0 || newData == 1023){//if clipping
    PORTB |= B00100000;//set pin 13 high- turn on clipping indicator led
    clipping = 1;//currently clipping
  }
  
  time++;//increment timer at rate of 38.5kHz
  
  ampTimer++;//increment amplitude timer
  if (abs(127-ADCH)>maxAmp){
    maxAmp = abs(127-ADCH);
  }
  if (ampTimer==1000){
    ampTimer = 0;
    checkMaxAmp = maxAmp;
    maxAmp = 0;
  }
  
}

void reset(){//clea out some variables
  index = 0;//reset index
  noMatch = 0;//reset match couner
  maxSlope = 0;//reset slope
}

// can be used if we want it
void checkClipping(){//manage clipping indicator LED
  if (clipping){//if currently clipping
    PORTB &= B11011111;//turn off clipping indicator led
    clipping = 0;
  }
}
