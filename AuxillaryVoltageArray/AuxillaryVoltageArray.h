// AuxillaryVoltageArray.h
// @author Eli Countrywood 

#ifndef _AUXILLARYVOLTAGEARRAY_h
#define _AUXILLARYVOLTAGEARRAY_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

// use pins 12 = 0x1000, 13 = 0x2000, 14 = 0x4000 to drive mux 
#define GPIO_BIT_MASK  ((1ULL<<GPIO_NUM_12) | (1ULL<<GPIO_NUM_13) | (1ULL<<GPIO_NUM_14))

/** class AuxillaryVoltageArray 
*	breif class to represent the 8 analog voltage measurement terminals
*			on the SCADA board. 
*	details These terminals are connected to MUX via a voltage divider
*			which devides the voltage by about 8. The GPIO pins 12,13,14 control the mux. 
*			The mux output is connected to GPIO pin 27 where the analog voltage is read.			
*/
class AuxillaryVoltageArray
{
public:
	//Array to hold voltage values of Aux Bus
	double Voltage[8];
	double divider_ratio = 7.8;
private:
	// use pin 35 to read analog voltage
	byte analogVoltagePin = 35;
	// 32 bit vector to set or clear digital signal from pins 12-14
	// e.g. REG_WRITE(GPIO_OUT_W1TS_REG, H[1]);
	// sets bit 13 and 14 HIGH.
	int D14 = 0x00004000;
	int D13 = 0x00002000;
	int D12 = 0x00001000;
	// H corresponds to the multiplexing order required to sequentially read
	// auxillary voltages from  V0 - V7 as laid out on PCB.
	// H[1]: pin 13, 14 are set high --> output pin Z is muxed to Y0:
	// = aux_voltage_0 on the PCB
	int H[8] = {(D13 | D14), (D12 | D13 | D14),
		(D12 | D13), (D13),(D12 | D14),
		0,(D14), (D12) };
public:
	// Constructor for AuxillaryVoltageArray
	AuxillaryVoltageArray();
	// function to read the 8 auxillary voltage pins
	bool readAuxVoltage();
	// Read voltage from a single pin with polynomial correction
	double ReadVoltage(byte pin);
	// getter function may be used later
	double getVoltage();
	// return a string containing the voltage values
	String toString();
};
#endif

