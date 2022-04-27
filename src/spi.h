// Description: Function prototypes for switch.cpp
//----------------------------------------------------------------------//

#ifndef SPI_H
#define SPI_H

void initLEDSPI();
void initLEDMatrix();
void writeToMatrix(unsigned char addy, unsigned int data);
void writeNote(unsigned char);

#endif