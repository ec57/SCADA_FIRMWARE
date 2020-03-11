// AuxillaryVoltageArray.cpp
// @author Eli Countrywood

#include "AuxillaryVoltageArray.h"

// class constructor
AuxillaryVoltageArray::AuxillaryVoltageArray()
{
	// Attach a pin to ADC (also clears 
	// any other analog mode that could be on). 
	// Returns TRUE or FALSE result.
	adcAttachPin(analogVoltagePin);

	///////// initial configuration for esp32  //////////////
	gpio_config_t io_conf;
	io_conf.intr_type = GPIO_INTR_DISABLE;
	io_conf.mode = GPIO_MODE_OUTPUT;
	io_conf.pin_bit_mask = GPIO_BIT_MASK;
	io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
	io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
	gpio_config(&io_conf);

	////////// Set the number of cycles per sample.
	//// Default is 8. Range: 1 to 255.
	// analogSetCycles(8);
}

// function to read the 8 auxillary voltage pins
bool AuxillaryVoltageArray::readAuxVoltage()
{
	// enable 12,13,14 as outputs
	REG_WRITE(GPIO_ENABLE_REG, 0x7000);

	for (int i = 0; i < 8; i++) {
		// write bits high
		REG_WRITE(GPIO_OUT_W1TS_REG, H[i]);
		delay(100);
		// analog read logic here...
		Voltage[i] = ReadVoltage(analogVoltagePin)*divider_ratio;
		// write bits low
		REG_WRITE(GPIO_OUT_W1TC_REG, H[i]);
	}
	return true;
}

/** brief ReadVoltage - Read the voltage of a pin
 * correction is performed on the initial reading for more accuracy
 * param pin - GPIO pin to read 
 * return double indicating the voltage level at the pin
 *
 */
double AuxillaryVoltageArray::ReadVoltage(byte pin)
{
	// Reference voltage is 3v3 so maximum reading is 3v3 = 4095 in range 0 to 4095
	// https://github.com/G6EJD/ESP32-ADC-Accuracy-Improvement-function/blob/master/ESP32_ADC_Read_Voltage_Accurate.ino
	double reading = analogRead(pin);
	if (reading < 1 || reading > 4095) return 0;
	return -0.000000000000016 * pow(reading, 4)
		+ 0.000000000118171 * pow(reading, 3)
		- 0.000000301211691 * pow(reading, 2)
		+ 0.001109019271794 * reading
		+ 0.034143524634089;
}

/** brief Stub for getter function to access the auxillary voltage array values.
 * ret-value to be decided
 * param none
 * return double
 *
 */
double AuxillaryVoltageArray::getVoltage(){
	return 0.0;
}

/** @brief Print out the values in the voltage vector
 * ret-value to be decided
 * @param none
 * @return String representing the float values on each pin of the 
 * auxillary voltage array
 *
 */

String AuxillaryVoltageArray::toString(){
	String data = "[";
	for (size_t i = 0; i < 8; i++)
	{		
		data = data + this->Voltage[i];
		data = data + ", ";
	}
	data = data + "]";
	return data;
}
