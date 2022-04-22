#include "microphone.h"
#include "ADC.h"
#include <avr/io.h>

float myAbs(float x){
  if(x<0){
    return -x;
  }
  return x;
}

void initMic(){
  // Pretty sure it's unnecessary, A0 is initialized in ADC code
}
float octave0[] = {16.35,17.32,18.35,19.45,20.60,21.83,23.12,24.50,25.96,27.50,29.14,30.87};
float octave1[] = {32.70,34.65,36.71,38.89,41.20,43.65,46.25,49.00,51.91,55.00,58.27,61.74};
float octave2[] = {65.41,69.30,73.42,77.78,82.41,87.31,92.50,98.00,103.83,110.00,116.54,123.47};
float octave3[] = {130.81,138.59,146.83,155.56,164.81,174.61,185.00,196.00,207.65,220.00,233.08,246.94};
float octave4[] = {261.63,277.18,293.66,311.13,329.63,349.23,369.99,392.00,415.30,440.00,466.16,493.88};
float octave5[] = {523.25,554.37,587.33,622.25,659.26,698.46,739.99,783.99,830.61,880.00,932.33,987.77};
float octave6[] = {1046.50,1108.73,1174.66,1244.51,1318.51,1396.91,1479.98,1567.98,1661.22,1760.00,1864.66,1975.53};
float octave7[] = {2093.00,2217.46,2349.32,2489.02,2637.02,2793.83,2959.96,3135.96,3322.44,3520.00,3729.31,3951.07};
float octave8[] = {4186.01,4434.92,4698.64,4978.03,5274.04,5587.65,5919.91,6271.93,6644.88,7040.00,7458.62,7902.13};
float* octaves[9] = {octave0, octave1, octave2, octave3, octave4, octave5, octave6, octave7, octave8};
unsigned char notes[] = {'C','C','D','D','E','F','F','G','G','A','A','B'};
void freqID(unsigned char *note, unsigned char *octave, float freq){
    for(int i = 0; i < 9; i++){
        if(freq < octaves[i][0] || freq > octaves[i][8]){
            continue;
        }
        if(myAbs(freq-octaves[i][0]) < myAbs(freq-octaves[i][1])){
            *note = notes[0];
            *octave = '0'+i;
            return;
        }
        for(int j = 1; j < 11; j++){
            if(myAbs(freq-octaves[i][j]) < myAbs(freq-octaves[i][j-1]) && myAbs(freq-octaves[i][j]) < myAbs(freq-octaves[i][j+1])){
                *note = notes[j];
                *octave = '0'+i;
                return;
            }
        }
        if(myAbs(freq-octaves[i][11]) < myAbs(freq-octaves[i][10])){
            *note = notes[11];
            *octave = '0'+i;
            return;
        }
    }
    *note = '?';
    *octave = '?';
}
