// Description: This file implements the initialization of an external
// switch.
// PINS:
// - Switch 1: Pin 11 : PORTB5
// - Switch 2: Pin 13 : PORTB7
//----------------------------------------------------------------------//

#include "switch.h"
#include <avr/io.h>

/*
 * Initializes pull-up resistor on PB3 and sets it into input mode
 */
void initSwitches(){
  DDRB &= ~(1<<DDB7); // Set it to input mode
  DDRB &= ~(1<<DDB5); // Set it to input mode
  PORTB |= (1<<PORTB7); // Enable pull-up resistor for playback button
  PORTB |= (1<<PORTB5); // Enable pull-up resistor for recording button
}
