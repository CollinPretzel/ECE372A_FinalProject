#include "microphone.h"
#include "ADC.h"
#include <avr/io.h>

void initMic(){
  // Pretty sure it's unnecessary, A0 is initialized in ADC code
}

void freqID(unsigned char *note, unsigned char *octave, unsigned char freq){
  // Calculate note and octave using given frequency - reference MIDI Chart
}
