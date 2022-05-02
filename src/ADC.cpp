#include "ADC.h"
#include <Arduino.h>
#include <avr/io.h>



// v_k = (5/1024)*k
void initADC() {
    DDRF &= ~(1 << DDF0);
    // set reference voltage to 5Vcc
    ADMUX |= (1 << REFS0);
    ADMUX &= ~(1 << REFS1);

    // left justify mux output
    ADMUX |= (1 << ADLAR);

    // Set single ended input
    ADMUX &= ~((1 << MUX2) | (1 << MUX1) | (1 << MUX0));
    ADMUX &= ~((1 << MUX4) | (1 << MUX3));
    ADCSRB &= ~(1 << MUX5);

    // set free running mode
    ADCSRB &= ~((1 << ADTS2) | (1 << ADTS1) | (1 << ADTS0));

    // enable ADC
    ADCSRA |= (1 << ADEN);

    ADCSRA |= (1<<ADATE);

    ADCSRA |= (1 << ADIE);

    // do we need a prescalar? This is a prescalar of 2. Sample rate ??? kHz
    ADCSRA |= (1 << ADPS0);
    ADCSRA &= ~((1 << ADPS2) | (1 << ADPS1));

    // disable ADC0 pin digital input - pin A0 on board
    DIDR0 |= (1 << ADC0D);

    // start conversion (we're in free running mode so this just constantly converts)
    ADCSRA |= (1 << ADSC);
}

int getADCOutput() {
    int outputValue = 0;
    ADCSRA |= (1 << ADSC);
    while(1) {
        if (ADCSRA & (1 << ADIF)) {
            outputValue = ADCL;
            outputValue += ((unsigned int) ADCH) << 8; // this line definitely won't work
            return outputValue;
        }
    }
}