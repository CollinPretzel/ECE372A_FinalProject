// Author: 
// Net ID: 
// Date: 
// Assignment: Lab 3
//----------------------------------------------------------------------//

#include "timer.h"
//You many use any timer you wish for the microsecond delay and the millisecond delay


/* Initialize timer 1, you should not turn the timer on here. Use CTC mode  .*/
void initTimer1(){
	//Set CTC Mode w/ only 1 in WGM12
    TCCR1A&=~(1<<WGM10);
    TCCR1A&=~(1<<WGM11);
    TCCR1B|=(1<<WGM12);
    TCCR1B&=~(1<<WGM13);
}

/* This delays the program an amount of microseconds specified by unsigned int delay.
*/
void delayMs(unsigned int delay){
    unsigned int count = 0;
    // Start the clock w/ a prescaler of 1
    // (1ms*16MHz)/1 - 1 = 16000 - 1 = 15999 = OCR1A
    TCCR1B &= !((1<<CS12)|(1<<CS11));
    TCCR1B |= (1<<CS10);
    OCR1AH = 0x3E;
    OCR1AL = 0x7F;

    while (count < delay){
        TIFR1 |= (1<<OCF1A); // Initialize compare flag to start the timer
        TCNT1 = 0; // Restart timer to make sure it's at 0

        while(!(TIFR1 & (1<<OCF1A))); // While the flag is set, do nothing
        
        count++;    
    }

    TCCR1B &= ~((1<<CS12)|(1<<CS11)|(1<<CS10)); // Turn off clock until it's needed again
}
