#include <Arduino.h>
#include "PWM.h"
// use timer 4 for calculatonsof variable freq

void initPWM_Pins(){
    //table 17-2, we need fast PWM, mode 15 for variable frequency
    TCCR4A |= (1<<WGM40) | (1<<WGM41);
    TCCR4B |= (1<<WGM42) | (1<<WGM43);
    // table 17-4, Fast PWM non-inverting mode
    TCCR4A &= ~(1<<COM4C0);
    TCCR4A |= (1<<COM4C1);
    //table 17-6, set prescalerto 1
    TCCR4B |= (1<<CS40);
    DDRH |= (1<<DDH5); //Set Pin 8 on board to output, which is OC4C, PORTH5, PWM
}

void IncFrequency(float frequency, bool on){
    if(on){
        OCR4A = 16000000 / frequency;
        OCR4C = OCR4A >> 1;
    } else {
        OCR4C = 0;
    }
    PORTB|=(1<<PORTB7);
}
