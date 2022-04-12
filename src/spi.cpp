#include <Arduino.h>
#include "spi.h"
#include <avr/interrupt.h>
#include <avr/io.h>

// Data Direction Register on ATMEGA2560 for SPI is DDRB
// SS Chip Select data direction bit B0 of ATMEGA2560 is DDB0
// Clock pin connection data direction bit B1 on ATMEGA2560 is DDB1
// MOSI pin datadirection on ATMEGA2560 is DDB0
// PortB for SPI on ATMEGA2560 is PORTB
// Port B register Bit B0 of Chip Select on ATMEGA2560 is PORTB0


void initLEDSPI(){
   // set MOSI,SCK,and SS direction to outputs
    DDRB = (1 << DDB2)| (1 << DDB1) | (1 << DDB0);

    // note this program does not use MISO line. IT only writes to device

    // set SS high initially (chip select off)
    PORTB  |= (1 << PORTB0);

    // enable SPI, master mode, CPOL, CPHA, default clock and fosc/128
    // datasheet says sample on rising edge CPOL = 1 CPHA =1
    SPCR  |= (1 <<SPE)| (1 << MSTR) | (1 << CPOL) | (1 << CPHA ) | (1 << SPR1) | (1 << SPR0);
}

void writeToMatrix(unsigned char addy, unsigned int data){
    PORTB &= ~(1 << PORTB0);  // enable chip select bit to begin SPI frame
    SPDR = addy; // load the CMD address into register
    while(!(SPSR & (1 << SPIF))); // wait for flag to raise
    SPDR = data; // load the data into register
    while(!(SPSR & (1 << SPIF))); // wait for flag to raise
    PORTB |= (1 << PORTB0); // disable chip select to end SPI frame
}
