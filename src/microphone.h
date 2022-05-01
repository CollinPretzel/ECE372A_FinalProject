#ifndef MIC_H
#define MIC_H

void initMic(); // Initiale analog input
void freqID(unsigned char *note, unsigned char *octave, float freq); // Involves timer and may be triggered by ADC interrupt or include it
float myAbs(float x); // Returns absolute value of x
unsigned char* noteToString(unsigned char* notes, int size); // Returns string of notes
unsigned char* octaveToString(unsigned char* octaves, int size); // Returns string of octaves
#endif
