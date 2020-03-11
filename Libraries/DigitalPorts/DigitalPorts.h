// @file DigitalPorts.h
// @author Eli Countrywood
// @date 3/2020
// 
#ifndef _DIGITALPORTS_h
#define _DIGITALPORTS_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif
/*	@brief Class to represent the digital output pins used to drive external relays
*	74HC595 shift register accepts a serial byte and raises the corresponding pins to 5V
*/
class DigitalPorts{
private:
	//Pin connected to ST_CP of 74HC595
	int latchPin = 33;
	//Pin connected to SH_CP of 74HC595
	int clockPin = 32;
	////Pin connected to DS of 74HC595
	int dataPin = 25;
public:
	// byte representing pins to raise e.g. 00110101
	byte data;
	// constructor
	DigitalPorts();
	// function to push data to shift register
	bool writeData(byte data);
};

#endif