#include "PWM.h"

void initPWMTimer3(){
    // PE3 for output
    DDRE |= (1 << DDE3);
    //Set up timer 3 for pwm mode
	TCCR3A |= (1 << WGM30);
    TCCR3A |= (1 << WGM31);
    TCCR3B |= (1 << WGM32);
    TCCR3B &= ~(1 << WGM33);
    //Set up Prescaler of 1
    TCCR3B &= ~((1<<CS32)|(1<<CS31));
    TCCR3B |= (1 << CS30);
    //Set to non-inverting mode
    TCCR3A |=(1 << COM3A1);
    TCCR3A &=~(1<< COM3A0);
    //OCR3A = 
    OCR3A = 0x3FF;
    TCNT3 = 0; // Start count at 0
}

void initPWMTimer4(){
    //PH3 for output
    DDRH |= (1 << DDH3);
    //Set up timer 4 for pwm mode
	TCCR4A |= (1 << WGM40);
    TCCR4A |= (1 << WGM41);
    TCCR4B |= (1 << WGM42);
    TCCR4B &= ~(1 << WGM43);
    //Set up Prescaler of 1
    TCCR4B &= ~((1<<CS42)|(1<<CS41));
    TCCR4B |= (1 << CS40);
    //Set to non-inverting mode
    TCCR4A |=(1 << COM4A1);
    TCCR4A &=~(1<< COM4A0);
    //OCR4A = 
    OCR4A = 0x000;
    TCNT4 = 0; // Start count at 0
}

//newCycle is the 10 bit output from the ADC
void changeDutyCycle(int newCycle){
    bool clockwise = true;
    int newCycle10Bit = newCycle & 0x3FF;
    // Voltage is greater than 2.5 V
    if(newCycle10Bit>=512){
        newCycle10Bit = (newCycle10Bit - 512)*2; // Sets it to zero if voltage is 2.5 V
        clockwise = false;
    }
    else{
        newCycle10Bit = (511 - newCycle10Bit)*2;
        clockwise = true;
    }
    // Switch directions!
    if(clockwise){
        OCR3A = newCycle10Bit;
        OCR4A = 0x000;
    } else {
        OCR4A = newCycle10Bit;
        OCR3A = 0x000;
    }
}
