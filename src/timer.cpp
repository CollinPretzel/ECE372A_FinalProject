// Author: Jackson Wood
// Net ID: jwwood3
// Date: 2/18/2022
// Assignment: Lab 3
//----------------------------------------------------------------------//

#include "timer.h"
//You many use any timer you wish for the microsecond delay and the millisecond delay

void initTimer3(){
	//Set Normal Mode
    TCCR3A&=~(1<<WGM30);
    TCCR3A&=~(1<<WGM31);
    TCCR3B&=~(1<<WGM32);
    TCCR3B&=~(1<<WGM33);
    // We want free-running mode so we can assess TCNT value and then number of overflow triggers
    // Starting clock with prescaler of 1, as high a resolution as possible.. theoretically
    TCCR3B &= ~((1 << CS32)|(1 << CS31));
    TCCR3B |= (1 << CS30);
}

/* Initialize timer 1, you should not turn the timer on here. Use CTC mode  .*/
void initTimer1() {
	// sets timer to CTC mode
    TCCR1A &= ~(1 << WGM10);
    TCCR1A &= ~(1 << WGM11);
    TCCR1B |= (1 << WGM12);
    TCCR1B &= ~(1 << WGM13);

    // sets prescalar to 1
    TCCR1B |= (1 << CS10);
    TCCR1B &= ~(1 << CS11);
    TCCR1B &= ~(1 << CS12);

    OCR1AH = 0x00;
    // counts from 0-f
    OCR1AL = 0x0f;
}

/* This delays the program an amount of microseconds specified by unsigned int delay.
*/
void delayUs(unsigned int delay){
    unsigned int usElapsed = 0;
    TCNT1 = 0;
    TIFR1 |= (1 << OCF1A);
    while(usElapsed < delay){
        if(TIFR1 & (1<<OCF1A)){
            usElapsed++;
            TIFR1 |= (1 << OCF1A);
        }
    }
}

/* Initialize timer 0, you should not turn the timer on here.
* You will need to use CTC mode */
void initTimer0(){
    TCCR0A |= (1 << WGM01); // Set bit for CTC mode
    TCCR0A &= ~(1 << WGM00); // Set bit for CTC mode
    TCCR0B &= ~(1 << WGM02); // Set bit for CTC mode
    TCCR0B |= (1 << CS01) | (1 << CS00); // Set bits for prescaler of 64
    TCCR0B &= ~(1 << CS02);
    OCR0A = 249;
    TCNT0 = 0;
}

/* This delays the program an amount specified by unsigned int delay.
* Use timer 0. Keep in mind that you need to choose your prescalar wisely
* such that your timer is precise to 1 millisecond and can be used for
* 100-2000 milliseconds
*/
void delayMs(unsigned int delay){
   unsigned int msElapsed = 0; // Keep track of the number of milliseconds passed
    TCNT0 = 0x00;
    TIFR0 |= (1 << OCF0A); // Start Timer
    while (msElapsed < delay){
        if(TIFR0 & (1 << OCF0A)){
            msElapsed++;
            TIFR0 |= (1 << OCF0A);
        }
    }
}
