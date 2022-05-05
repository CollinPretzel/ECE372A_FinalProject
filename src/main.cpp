// Author: Collin Preszler
// Net ID: collinpreszler
// Date: 2/18/2022
// Assignment:     Lab 3
//
// Description: Automatic Pitch and Harmonic Identification (APHID)
//
//----------------------------------------------------------------------//

#include <Arduino.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "pwm.h"
#include "switch.h"
#include "timer.h"
#include "spi.h"
#include "microphone.h"
#include "lcd.h"
#include "ADC.h"

// definitions
#define LONG_DELAY 200
#define SHORT_DELAY 100

#define SAMPLE_RATE 500000
#define NUM_NOTES 20

unsigned char noteHistory[NUM_NOTES+1];
unsigned char octaveHistory[NUM_NOTES+1];

// ADC variables
volatile byte newData = 0;
volatile byte prevData = 0;
volatile unsigned int time = 0;//keeps time and sends vales to store in timer[] occasionally
volatile int timer[10];//sstorage for timing of events
volatile int slope[10];//storage for slope of events
volatile unsigned int totalTimer;//used to calculate period
volatile unsigned int period;//storage for period of wave
volatile unsigned int overflowCnt = 0; // Track the number of times Timer 3 overflows
volatile unsigned int initial, final; // Start and end of timer counting
volatile bool isCounting = true; // renamed for clarity
volatile byte index = 0;//current storage index
volatile float frequency;//storage for frequency calculations
volatile int maxSlope = 0;//used to calculate max slope as trigger point
volatile int newSlope;//storage for incoming slope data
volatile int prevSlope; // storage for last calculated slope
// ADC slope match variables
volatile byte noMatch = 0;//counts how many non-matches you've received to reset variables if it's been too long
volatile byte slopeTol = 3;//slope tolerance (adjustable)
volatile int timerTol = 10;//timer tolerance (adjustable)

//variables for amp detection
volatile unsigned int ampTimer = 0;
volatile byte maxAmp = 0;
volatile byte checkMaxAmp;
volatile byte ampThreshold = 30;//raise if you have a very noisy signal

volatile boolean clipping = 0; // In case the noise clips for future implementations

// C, C#, D, D#, E, F, F#, G, G#, A, A#, B
float octave20[] = {16.35,17.32,18.35,19.45,20.60,21.83,23.12,24.50,25.96,27.50,29.14,30.87};
float octave21[] = {32.70,34.65,36.71,38.89,41.20,43.65,46.25,49.00,51.91,55.00,58.27,61.74};
float octave22[] = {65.41,69.30,73.42,77.78,82.41,87.31,92.50,98.00,103.83,110.00,116.54,123.47};
float octave23[] = {130.81,138.59,146.83,155.56,164.81,174.61,185.00,196.00,207.65,220.00,233.08,246.94};
float octave24[] = {261.63,277.18,293.66,311.13,329.63,349.23,369.99,392.00,415.30,440.00,466.16,493.88};
float octave25[] = {523.25,554.37,587.33,622.25,659.26,698.46,739.99,783.99,830.61,880.00,932.33,987.77};
float octave26[] = {1046.50,1108.73,1174.66,1244.51,1318.51,1396.91,1479.98,1567.98,1661.22,1760.00,1864.66,1975.53};
float octave27[] = {2093.00,2217.46,2349.32,2489.02,2637.02,2793.83,2959.96,3135.96,3322.44,3520.00,3729.31,3951.07};
float octave28[] = {4186.01,4434.92,4698.64,4978.03,5274.04,5587.65,5919.91,6271.93,6644.88,7040.00,7458.62,7902.13};
float* octaves2[] = {octave20, octave21, octave22, octave23, octave24, octave25, octave26, octave27, octave28};

void reset(){//clea out some variables
  index = 0;//reset index
  noMatch = 0;//reset match couner
  maxSlope = 0;//reset slope
}

int readTCNT(){ // Reads the value from Timer 3's counter
  unsigned char sreg;
  sreg = SREG; // Store the current state of the interrupt status
  cli(); // Remove interrupt capability to properly read TCNT
  int i = TCNT3;
  SREG = sreg; // Replace all interrupt triggers
  return i;
}

int main(){
  for(int i=0;i<NUM_NOTES;i++){
    noteHistory[i] = 255;
    octaveHistory[i] = 255;
  }
  noteHistory[NUM_NOTES] = 0;
  octaveHistory[NUM_NOTES] = 0;
  DDRG&=~(1<<DDG5);
  PORTG|=(1<<PORTG5);
  Serial.begin(9600);
  Serial.println("Starting...");
  Serial.println("Initializing LEDs");
  initLEDSPI();
  // initialize 8 x 8 LED array (info from MAX7219 datasheet)
  writeToMatrix(0x0A, 0x08);  // brightness control
  writeToMatrix(0x0B, 0x07); // scanning all rows and columns
  writeToMatrix(0x0C,0x01); // set shutdown register to normal operation (0x01)
  writeToMatrix(0x0F, 0x00); // display test register - set to normal operation (0x01)
  Serial.println("Initializing timers");
  initTimer3();
  initTimer1();
  initTimer0();
  Serial.println("Initializing Switch");
  initSwitches();
  Serial.println("Initializing PWM");
  initPWM_Pins();
  Serial.println("Initializing LCD");
  initLCD();
  Serial.println("Initializing ADC");
  initADC();
  Serial.println("Done Initializing");
  sei(); // Enable global interrupts.
  bool playing = false;
// while loop
  while(1){
    Serial.flush();
    delayMs(1000);
    //eightBitCommandWithDelay(0b00011100,40);
    if(!(PINB & (1<<PINB7))){//change to the pin for the replay button
      Serial.println("Replay button pressed");
      playing = true;
      char curNote = noteHistory[0];
      char curOctave = octaveHistory[0];
      //Display current note on the LED array
      writeNote(curNote);
      //Display note history on LCD
      moveCursor(0,0);
      writeString(noteToString(noteHistory,NUM_NOTES));
      moveCursor(1,0);
      writeString(octaveToString(octaveHistory,NUM_NOTES));
      //play current note on the speaker for a little bit
      IncFrequency(octaves2[curOctave][curNote],1); // Play the current note in the history
      for(int i=0;i<NUM_NOTES-1;i++){//remove the first note from the history
        noteHistory[i] = noteHistory[i+1];
        octaveHistory[i] = octaveHistory[i+1];
      }
    } else {
      if(!playing){
        Serial.println("Playing is false");
      }
      IncFrequency(1200,0); // Turns of the piezo buzzer
      Serial.print("Playing=");
      Serial.println(playing);
      if(playing){
        for(int i=0;i<NUM_NOTES;i++){ // Clear note history
          noteHistory[i] = 0;
          octaveHistory[i] = 0;
        }
      }
      playing = false;
    }
    if(!(PINB&(1<<PINB5)) && !playing){//change to the pin for the record button
      // Serial monitor for debugging and verification
      Serial.print("Frequency: ");
      Serial.println(frequency);
      unsigned char octave = 0;
      unsigned char note = 0;
      freqID(&note,&octave,frequency); // Identifies the note and octave from the given frequency()
      Serial.print("Note: ");
      Serial.print(note);
      Serial.print(" Octave: ");
      Serial.println(octave);
      if(noteHistory[0] != note || octaveHistory[0] != octave){//Save old notes to history if it's a new note
        for(int i=1;i<NUM_NOTES;i++){
          noteHistory[i] = noteHistory[i-1];
          octaveHistory[i] = octaveHistory[i-1];
        }
        noteHistory[0]=note;
        octaveHistory[0]=octave;
      }
      //Print note to the ledMatrix
      Serial.print("Note: ");
      Serial.print(note);
      writeNote(note);
      //Print note history to LCD
      moveCursor(0,0);
      writeString(noteToString(noteHistory,NUM_NOTES));
      moveCursor(1,0);
      writeString(octaveToString(octaveHistory,NUM_NOTES));
    }
    
    // needed if we want an indicator light that for when sound is out of range
    if (clipping){//if currently clipping
      PORTH &= ~(1<<PORTH6);//turn off clipping indicator led
      clipping = 0;
    }

  }
  return 0;
}

ISR(TIMER3_OVF_vect){
  if(isCounting){
    // Supposed to determine how often the timer overflowed to calculate frequency
    // Ran into some consistency issues in implementation w/ the full code
    overflowCnt++;
  }
}

ISR(ADC_vect) {//when new ADC value ready
  
  prevData = newData;//store previous value
  newData = ADCH;//get value from A0
  if (prevData < 127 && newData >=127){//if increasing and crossing midpoint
    //PORTB &= ~(1 << PORTB4);
    newSlope = newData-prevData;
    if (abs(newSlope-maxSlope)<slopeTol){//if slopes are ==
      if(isCounting){
        final = readTCNT();
        // unneeded timePassed = (final + overflowCnt*65535 - initial)/16.0; // Calculation for number of counts passed, adjusted for prescaler
        frequency = 16000000.0/(final + overflowCnt*65535 - initial); // frequency calculated from using counts and timer 3's frequency
        isCounting = false;
      }
      else{
        // reset time
        overflowCnt = 0;
        initial = readTCNT();
        isCounting = true;
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
      if (noMatch>300){ // Range of 20 Hz to 5 kHz, could be improved w/ a Hi-Fi and Lo-Fi bool, rests slope values if there are no matches for a given time
        reset();
      }
    }
}