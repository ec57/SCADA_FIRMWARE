// DigitalPorts.cpp
// @author Eli Countrywood
// 
#include "DigitalPorts.h"

/** @brief Digital Port Class constructor
 * represents an array of 8 digital output pins
 * connected to and driven by the 595 shift register
 */
DigitalPorts::DigitalPorts()
{
	pinMode(latchPin, OUTPUT);  //Pin connected to ST_CP of 74HC595
	pinMode(clockPin, OUTPUT);  //Pin connected to SH_CP of 74HC595
	pinMode(dataPin, OUTPUT);   //Pin connected to DS of 74HC595
}

/** brief Write a byte vector to the digital output pins
 * param byte of digital data 
 * 0x0 is all off, and 0x1 = [00000001]. 
 * Labels on board: [76543210]
 * return boolean indicating the function actually ran
 *
 */
bool DigitalPorts::writeData(byte data)
{
    digitalWrite(latchPin, LOW);
	shiftOut(dataPin, clockPin, MSBFIRST, data);
	digitalWrite(latchPin, HIGH);
	return true;
}