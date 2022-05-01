// Author: 
// Net ID: 
// Date: 
// Assignment: Lab 3
//
// PINS:
// - VSS - Ground
// - VDD - 5V
// - V0 - 10k Potentiometer between 0V-5V for contrast dial
// - RS - Pin 12 : PORTB6
// - RW - Ground
// - E - Pin 10 : PORTB4
// - D0 - N/A
// - D1 - N/A
// - D2 - N/A
// - D3 - N/A
// - D4 - Pin 22 : PORTA0
// - D5 - Pin 23 : PORTA1
// - D6 - Pin 24 : PORTA2
// - D7 - Pin 25 : PORTA3
// - A - 5V                Backlight Power
// - K - Ground            Backlight Power
//----------------------------------------------------------------------//
#include "lcd.h"
#include "timer.h"
#include <util/delay.h>

/*
 * Initializes all pins related to the LCD to be outputs
 */
void initLCDPins(){
  // RS and Enable pins
  DDRB |= (1<<DDB4)|(1<<DDB6); // Initialize as outputs PORT B4 and B6 for LCD enable and RS
  PORTB &= ~((1<<PORTB4)|(1<<PORTB6)); // Set these pins to zero to begin with, they'll be changed as needed
  // LED pins
  DDRA |= (1<<DDA0)|(1<<DDA1)|(1<<DDA2)|(1<<DDA3); // Initialize as output pins
  PORTA &= ~((1<<PORTA0)|(1<<PORTA1)|(1<<PORTA2)|(1<<PORTA3)); // Start with the pins turned off
}



/* 1. Assert bits to pins connected to DB[7:4] on the LCD screen
 *      a. data is an unsigned char which has 8 bits. Therefore, you
 *         need assign the bottom 4 bits of "data" to the appropriate bits in
 *         PORTA
 *  2. This is a "command" signal, meaning RS should be Low
 *  3. Assert high on enable pin, delay, and asset low on enable pin
 *  4. delay the provided number in MICROseconds.
 */
void fourBitCommandWithDelay(unsigned char data, unsigned int delay){
  PORTA = (PORTA & 0xF0) | (data & 0x0F); // data;
  PORTB &= ~(1<<PORTB6); // Set RS as low (0)
  PORTB |= (1<<PORTB4); // Set enable pin to high
  delayUs(1);
  PORTB &= ~(1<<PORTB4); // Set enable pin back to low
  delayUs(delay);
}



/* Similar to fourBitCommandWithDelay except that now all eight bits of command are
 * sent.
 * 1. Assert bits to pins connected to DB[7:4] on the LCD screen
 *       a. unlike fourBitCommandWithDelay, you need to send the TOP four bits of
 *          "command" first. These should be assigned to appropriate bits in PORTA
 * 2. This is a command signal, meaning RS should be low
 * 3. Assert high on enable pin, delay, and asset low on enable pin
 * 4. Now set the lower four bits of command to appropriate bits in PORTA
 * 5. Assert high on enable pin, delay, and asset low on enable pin
 * 6. delay the provided number in MICROseconds.
 */
void eightBitCommandWithDelay(unsigned char command, unsigned int delay){
  PORTA = (PORTA & 0xF0) | (command >> 4); // Moves the top four bits into the bottom four, allowing them to be assigned to PORTA
  PORTB &= ~(1<<PORTB6); // Set RS as low (0)
  PORTB |= (1<<PORTB4); // Set enable pin to high
  delayUs(1);
  PORTB &= ~(1<<PORTB4); // Set enable pin back to low
  PORTA = (PORTA & 0xF0) | (command & 0x0F); // data;
  PORTB |= (1<<PORTB4); // Set enable pin to high
  delayUs(1);
  PORTB &= ~(1<<PORTB4); // Set enable pin back to low
  delayUs(delay);
}



/* Similar to eightBitCommandWithDelay except that now RS should be high
 * 1. Assert bits to pins connected to DB[7:4] on the LCD screen
 * 2. This is a "data" signal, meaning RS should be high
 * 3. Assert high on enable pin, delay, and asset low on enable pin
 * 4. Now set the lower four bits of character to appropriate bits in PORTA
 * 5. Assert high on enable pin, delay, and asset low on enable pin
 * 6. delay is always 46 MICROseconds for a character write
 */
void writeCharacter(unsigned char character){
  PORTA = (PORTA & 0xF0) | (character >> 4); // Moves the top four bits into the bottom four, allowing them to be assigned to PORTA
  PORTB |= (1<<PORTB6); // Set RS as high (1)
  PORTB |= (1<<PORTB4); // Set enable pin to high
  delayUs(1);
  PORTB &= ~(1<<PORTB4); // Set enable pin back to low
  PORTA = (PORTA & 0xF0) | (character & 0x0F); // data;
  PORTB |= (1<<PORTB4); // Set enable pin to high
  delayUs(1);
  PORTB &= ~(1<<PORTB4); // Set enable pin back to low
  delayUs(46);
}




/*
 * Writes a provided string such as "Hello!" to the LCD screen. You should
 * remember that a c string always ends with the '\0' character and
 * that this should just call writeCharacter multiple times.
 */
void writeString(unsigned char *string){
  while(*string != '\0'){
    writeCharacter(*string);
    string++;
  }
}



/*
 * This moves the LCD cursor to a specific place on the screen.
 * This can be done using the eightBitCommandWithDelay with correct arguments
 * Row is x, column is y
 */
void moveCursor(unsigned char x, unsigned char y){
  char command = 0x80|(x<<6)|y; // Sets B7 to high, B6 to x (0=1st line), and B3-B0 to y (lateral 0-15)
  eightBitCommandWithDelay(command, 40);
}


/* This is the procedure outline on the LCD datasheet page 4 out of 9.
 * This should be the last function you write as it largely depends on all other
 * functions working.
 */
void initLCDProcedure(){
  // Delay 15 milliseconds
  delayMs(50); // Allow the LCD to power up
  // Write 0b0011 to DB[7:4] and delay 4100 microseconds
  fourBitCommandWithDelay(0b0011,4100); // special case of function set, unknown purpose
  // Write 0b0011 to DB[7:4] and delay 100 microseconds
  fourBitCommandWithDelay(0b0011,100); // special case of function set, unknown function
  // The data sheet does not make this clear, but at this point you are issuing
  // commands in two sets of four bits. You must delay after each command
  // (which is the second set of four bits) an amount specified on page 3 of
  // the data sheet.
  // write 0b0011 to DB[7:4] and 100us delay
  fourBitCommandWithDelay(0b0011,100); // special case again, reset recognition
  // write 0b0010 to DB[7:4] and 100us delay.
  fourBitCommandWithDelay(0b0010,100); // REAL function set, switch to four-bit mode
  // Function set in the command table with 53us delay
  eightBitCommandWithDelay(0b00101000,53); // data length = 4-bit, 2 lines, DNM but 5x7 dots
  // Display off in the command table with 53us delay
  eightBitCommandWithDelay(0b00001000,53); // Initialize display control w/ everything off
  // Clear display in the command table. Remember the delay is longer!!!
  eightBitCommandWithDelay(0b00000001,3000); // Clear display, even longer delay for peace of mind
  // Entry Mode Set in the command table.
  eightBitCommandWithDelay(0b00000110,53); //110 Set cursor to increment (write forward?) and no shifting display
  // Display ON/OFF Control in the command table. (Yes, this is not specified),
  // in the data sheet, but you have to do it to get this to work. Yay datasheets!)
  eightBitCommandWithDelay(0b00001100,53); // turn display on but keep cursor and blinking off
}



/* Initializes Tri-state for LCD pins and calls initialization procedure.
* This function is made so that it's possible to test initLCDPins separately
* from initLCDProcedure which will likely be necessary.
*/
void initLCD(){
  initLCDPins();
  initLCDProcedure();
}