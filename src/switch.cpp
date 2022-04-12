// Description: This file implements the initialization of an external
// switch.
//----------------------------------------------------------------------//

#include "switch.h"
#include <avr/io.h>

/*
 * Initializes pull-up resistor on PB3 and sets it into input mode
 */
void initSwitchPB3(){
  DDRB &= ~(1<<DDB3); // Set it to input mode
  PORTB |= (1<<PORTB3); // Enable pull-up resistor
  PCICR |= (1<<PCIE0); // Enables interrupt for PortB pins, e.g. PB3
  PCMSK0 |= (1<<PCINT3); // PB3 is tagged with PCINT3
}
