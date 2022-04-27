#ifndef MIC_H
#define MIC_H

void initMic(); // Initiale analog input
void freqID(unsigned char *note, unsigned char *octave, float freq); // Involves timer and may be triggered by ADC interrupt or include it

#endif
