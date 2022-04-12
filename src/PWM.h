// Author:         
// Net ID:         
// Date:           
// Assignment:     Lab 4
//----------------------------------------------------------------------//

#ifndef PWM_H
#define PWM_H

#include <avr/io.h>

void initPWMTimer3();
void initPWMTimer4();
void changeDutyCycle(int newCycle);

#endif