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

void writeNote(unsigned char note){
   switch(note){
      case 0: // C
         writeToMatrix(0x01, 0b00000000);
         writeToMatrix(0x02, 0b00111100);
         writeToMatrix(0x03, 0b01000010);
         writeToMatrix(0x04, 0b10000000);
         writeToMatrix(0x05, 0b10000000);
         writeToMatrix(0x06, 0b10000000);
         writeToMatrix(0x07, 0b01000010);
         writeToMatrix(0x08, 0b00111100);
         break;
      case 1: // C#
         writeToMatrix(0x01, 0b00000001);
         writeToMatrix(0x02, 0b00111100);
         writeToMatrix(0x03, 0b01000010);
         writeToMatrix(0x04, 0b10000000);
         writeToMatrix(0x05, 0b10000000);
         writeToMatrix(0x06, 0b10000000);
         writeToMatrix(0x07, 0b01000010);
         writeToMatrix(0x08, 0b00111100);
         break;
      case 2: // D
         writeToMatrix(0x01, 0b00000000);
         writeToMatrix(0x02, 0b11111100);
         writeToMatrix(0x03, 0b01000010);
         writeToMatrix(0x04, 0b01000001);
         writeToMatrix(0x05, 0b01000001);
         writeToMatrix(0x06, 0b01000001);
         writeToMatrix(0x07, 0b01000010);
         writeToMatrix(0x08, 0b11111100);
         break;
      case 3: // D#
         writeToMatrix(0x01, 0b00000001);
         writeToMatrix(0x02, 0b11111100);
         writeToMatrix(0x03, 0b01000010);
         writeToMatrix(0x04, 0b01000001);
         writeToMatrix(0x05, 0b01000001);
         writeToMatrix(0x06, 0b01000001);
         writeToMatrix(0x07, 0b01000010);
         writeToMatrix(0x08, 0b11111100);
         break;
      case 4: // E
         writeToMatrix(0x01, 0b00000000);
         writeToMatrix(0x02, 0b11111111);
         writeToMatrix(0x03, 0b01000001);
         writeToMatrix(0x04, 0b01000000);
         writeToMatrix(0x05, 0b01110000);
         writeToMatrix(0x06, 0b01000000);
         writeToMatrix(0x07, 0b01000001);
         writeToMatrix(0x08, 0b11111111);
         break;
      case 5: // F
         writeToMatrix(0x01, 0b00000000);
         writeToMatrix(0x02, 0b11111111);
         writeToMatrix(0x03, 0b01000001);
         writeToMatrix(0x04, 0b01001000);
         writeToMatrix(0x05, 0b01111000);
         writeToMatrix(0x06, 0b01001000);
         writeToMatrix(0x07, 0b01000000);
         writeToMatrix(0x08, 0b11100000);
         break;
      case 6: // F#
         writeToMatrix(0x01, 0b00000001);
         writeToMatrix(0x02, 0b11111111);
         writeToMatrix(0x03, 0b01000001);
         writeToMatrix(0x04, 0b01001000);
         writeToMatrix(0x05, 0b01111000);
         writeToMatrix(0x06, 0b01001000);
         writeToMatrix(0x07, 0b01000000);
         writeToMatrix(0x08, 0b11100000);
         break;
      case 7: // G
         writeToMatrix(0x01, 0b00000000);
         writeToMatrix(0x02, 0b00111111);
         writeToMatrix(0x03, 0b01000001);
         writeToMatrix(0x04, 0b10000000);
         writeToMatrix(0x05, 0b10011111);
         writeToMatrix(0x06, 0b10001001);
         writeToMatrix(0x07, 0b01000001);
         writeToMatrix(0x08, 0b00111110);
         break;
      case 8: // G #
         writeToMatrix(0x01, 0b00000001);
         writeToMatrix(0x02, 0b00111111);
         writeToMatrix(0x03, 0b01000001);
         writeToMatrix(0x04, 0b10000000);
         writeToMatrix(0x05, 0b10011111);
         writeToMatrix(0x06, 0b10001001);
         writeToMatrix(0x07, 0b01000001);
         writeToMatrix(0x08, 0b00111110);
         break;
      case 9: // A
         writeToMatrix(0x01, 0b00000000);
         writeToMatrix(0x02, 0b00011000);
         writeToMatrix(0x03, 0b00100100);
         writeToMatrix(0x04, 0b01000010);
         writeToMatrix(0x05, 0b01000010);
         writeToMatrix(0x06, 0b01111110);
         writeToMatrix(0x07, 0b01000010);
         writeToMatrix(0x08, 0b11100111);
         break;
      case 10: //A#
         writeToMatrix(0x01, 0b00000001);
         writeToMatrix(0x02, 0b00011000);
         writeToMatrix(0x03, 0b00100100);
         writeToMatrix(0x04, 0b01000010);
         writeToMatrix(0x05, 0b01000010);
         writeToMatrix(0x06, 0b01111110);
         writeToMatrix(0x07, 0b01000010);
         writeToMatrix(0x08, 0b11100111);
         break;
      case 11: //B
         writeToMatrix(0x01, 0b00000000);
         writeToMatrix(0x02, 0b11111100);
         writeToMatrix(0x03, 0b01000010);
         writeToMatrix(0x04, 0b01000100);
         writeToMatrix(0x05, 0b01111110);
         writeToMatrix(0x06, 0b01000001);
         writeToMatrix(0x07, 0b01000001);
         writeToMatrix(0x08, 0b11111110);
         break;
      default:
         writeToMatrix(0x01, 0b00000000);
         writeToMatrix(0x02, 0b00000000);
         writeToMatrix(0x03, 0b00000000);
         writeToMatrix(0x04, 0b00000000);
         writeToMatrix(0x05, 0b00000000);
         writeToMatrix(0x06, 0b00000000);
         writeToMatrix(0x07, 0b00000000);
         writeToMatrix(0x08, 0b00000000);
   }
}
