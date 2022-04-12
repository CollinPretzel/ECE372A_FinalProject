#ifndef MIC_H
#define MIC_H

void initMic(); // Initiale analog input
void freqID(char *note, unsigned int *octave); // Involves timer and may be triggered by ADC interrupt or include it

#endif MIC_H
