/*!
 * @file Adafruit_INA219.cpp
 *
 * @mainpage Adafruit INA219 current/power monitor IC
 *
 * @section intro_sec Introduction
 *
 *  Driver for the INA219 current sensor
 *
 *  This is a library for the Adafruit INA219 breakout
 *  ----> https://www.adafruit.com/products/904
 *
 *  Adafruit invests time and resources providing this open source code,
 *  please support Adafruit and open-source hardware by purchasing
 *  products from Adafruit!
 *
 * @section author Author
 *
 * Written by Kevin "KTOWN" Townsend for Adafruit Industries.
 *
 * @section license License
 *
 * BSD license, all text here must be included in any redistribution.
 *
 Added a configuration for 10 Amp and 0.003 ohm sense resistor.
 */

#include "Arduino.h"
#include <Wire.h>
#include "Adafruit_INA219.h"

 /*!
  *  @brief  Sends a single command byte over I2C
  *  @param  reg
  *          register address
  *  @param  value
  *          value to write
  */
void Adafruit_INA219::wireWriteRegister(uint8_t reg, uint16_t value) {
    _i2c->beginTransmission(ina219_i2caddr);
    _i2c->write(reg);                 // Register
    _i2c->write((value >> 8) & 0xFF); // Upper 8-bits
    _i2c->write(value & 0xFF);        // Lower 8-bits
    _i2c->endTransmission();
}

/*!
 *  @brief  Reads a 16 bit values over I2C
 *  @param  reg
 *          register address
 *  @param  *value
 *          read value
 */
void Adafruit_INA219::wireReadRegister(uint8_t reg, uint16_t* value) {

    _i2c->beginTransmission(ina219_i2caddr);
    _i2c->write(reg); // Register
    _i2c->endTransmission();

    delay(1); // Max 12-bit conversion time is 586us per sample

    _i2c->requestFrom(ina219_i2caddr, (uint8_t)2);
    // Shift values to create properly formed integer
    *value = ((_i2c->read() << 8) | _i2c->read());
}

/*!
 *  @brief  Set power save mode according to parameters
 *  @param  on
 *          boolean value
 */
void Adafruit_INA219::powerSave(bool on) {
    uint16_t current;
    wireReadRegister(INA219_REG_CONFIG, &current);
    uint8_t next;
    if (on) {
        next = current | INA219_CONFIG_MODE_POWERDOWN;
    }
    else {
        next = current & ~INA219_CONFIG_MODE_POWERDOWN;
    }
    wireWriteRegister(INA219_REG_CONFIG, next);
}

/*!
 *  @brief  Configures the INA219 to measure up to 32V and 10A of current. See datasheet for details.  
 *  @note   These calculations assume a 0.003 ohm resistor is present
 *  @author Modified by Eli C. and Jose T. 1/17/2020 for SCADA PCB project
 *  MATLAB CODE TO CALCULATE CALIBRATION VALUE:
        % 0.04096 is an internal fixed value used to ensure scaling is maintained properly
        SCALING_VALUE = 0.04096;        % a constant        
        MAXIMUM_EXPECTED_CURRENT = 10;  % current in Amperes
        R_SHUNT = 3e-3;                 % 3 milliOhm shunt resistor
        Current_LSB = MAXIMUM_EXPECTED_CURRENT / (2^15)                     % equation 2
        Cal = floor((SCALING_VALUE)/(Current_LSB * R_SHUNT))                % equation 1
        Calibration_Register = dec2hex(Cal) % HEX value for register
        dec2bin(Cal)                        % BIN value for register
        Power_LSB = 20 * Current_LSB                                        % equation 3
 */

void Adafruit_INA219::setCalibration_32V_10A() {
    
    ina219_calValue =  0xAEC3;          // This value can be calculated with the MATLAB code above
	ina219_currentLSB = 0.000305;       // Each unit of current corresponds to 305uA
	ina219_shuntVoltageLSB  = 0.000010; // 10 uV
	
    // Power Register content is multiplied by Power LSB which is 
    // 20 times the Current_LSB for a power value in watts.
    // Each unit of power corresponds to 6.1mW.
    ina219_powerMultiplier_mW = 20 * ina219_currentLSB;     
	
    // Set Config register to take into account the settings above
    // These are the settings used for the SCADA board
    // uint16_t config = 0x27FF -> This is the value obtained by filling the 
    // config register using the data sheeet. It matches below.
    uint16_t config = INA219_CONFIG_BVOLTAGERANGE_32V |
        INA219_CONFIG_GAIN_1_40MV | 
        INA219_CONFIG_BADCRES_12BIT |
        INA219_CONFIG_SADCRES_12BIT_128S_69MS |
        INA219_CONFIG_MODE_SANDBVOLT_CONTINUOUS;

    // Set Configuration register
    wireWriteRegister(INA219_REG_CONFIG, config);
    // Set Calibration register to 'Cal' calculated above
    wireWriteRegister(INA219_REG_CALIBRATION, ina219_calValue);
}

/*!
 *  @brief  Instantiates a new INA219 class
 *  @param addr the I2C address the device can be found on. Default is 0x40
 */
Adafruit_INA219::Adafruit_INA219(uint8_t addr) {
    ina219_i2caddr = addr;
}

/*!
 *  @brief  Setups the HW (defaults to 32V and 10A for calibration values)
 *  @param theWire the TwoWire object to use
 */
void Adafruit_INA219::begin(TwoWire* theWire) {
    _i2c = theWire;
    init();
}

/*!
 *  @brief  begin I2C and set up the hardware
 */
void Adafruit_INA219::init() {
    _i2c->begin();
    // Set chip to 10A range config values to start
    setCalibration_32V_10A();
}

/*!
 *  @brief  Gets the raw bus voltage (16-bit signed integer, so +-32767)
 *  @return the raw bus voltage reading
 */
int16_t Adafruit_INA219::getBusVoltage_raw() {
    uint16_t value;
    wireReadRegister(INA219_REG_BUSVOLTAGE, &value);
    // Shift to the right 3
    return (int16_t)((value >> 3) * 4);
}

/*!
 *  @brief  Gets the raw shunt voltage (16-bit signed integer, so +-32767)
 *  @return the raw shunt voltage reading
 */
int16_t Adafruit_INA219::getShuntVoltage_raw() {
    uint16_t value;
    wireReadRegister(INA219_REG_SHUNTVOLTAGE, &value);
    return (int16_t)value;
}

/*!
 *  @brief  Gets the raw current value (16-bit signed integer, so +-32767)
 *  @return the raw current reading
 */
int16_t Adafruit_INA219::getCurrent_raw() {
    uint16_t value;

    // Sometimes a sharp load will reset the INA219, which will
    // reset the cal register, meaning CURRENT and POWER will
    // not be available ... avoid this by always setting a cal
    // value even if it's an unfortunate extra step
    wireWriteRegister(INA219_REG_CALIBRATION, ina219_calValue);

    // Now we can safely read the CURRENT register!
    wireReadRegister(INA219_REG_CURRENT, &value);

    return (int16_t)value;
}

/*!
 *  @brief  Gets the raw power value (16-bit signed integer, so +-32767)
 *  @return raw power reading
 */
int16_t Adafruit_INA219::getPower_raw() {
    uint16_t value;

    // Sometimes a sharp load will reset the INA219, which will
    // reset the cal register, meaning CURRENT and POWER will
    // not be available ... avoid this by always setting a cal
    // value even if it's an unfortunate extra step
    wireWriteRegister(INA219_REG_CALIBRATION, ina219_calValue);

    // Now we can safely read the POWER register!
    wireReadRegister(INA219_REG_POWER, &value);

    return (int16_t)value;
}

/*!
 *  @brief  Gets the shunt voltage in mV (so +-327mV)
 *  @return the shunt voltage converted to millivolts
 *	Shunt Voltage Register LSB: 10uV
 */
float Adafruit_INA219::getShuntVoltage_mV() {
    int16_t value;
    value = getShuntVoltage_raw();
    return value * ina219_shuntVoltageLSB; // 10uV * (1000mV/1V)
}

/*!
 *  @brief  Gets the shunt voltage in volts
 *  @return the bus voltage converted to milivolts
 */
float Adafruit_INA219::getBusVoltage_V() {
    int16_t value = getBusVoltage_raw();
    return value *0.001; // basic conversion to V
}

/*!
 *  @brief  Gets the current value in mA, taking into account the
 *  config settings and current LSB to obtain a value in 
 *  amperes the Current register value is multiplied by the programmed Current_LSB.
 *  @return the current reading convereted to milliamps
 */
float Adafruit_INA219::getCurrent_mA() {
    float valueDec = getCurrent_raw();
	valueDec *= ina219_currentLSB; 
    return valueDec;
}

/*!
 *  @brief  Gets the power value in mW, taking into account the
 *          config settings and current LSB
 *  @return power reading converted to milliwatts
 */
float Adafruit_INA219::getPower_mW() {
    float valueDec = getPower_raw();
    valueDec *= ina219_powerMultiplier_mW;
    return valueDec;
}

/*!
 *  @brief  read the status of the power monitor
 *  @return void
 */
void Adafruit_INA219::readDevice(){
    busVoltage = this->getBusVoltage_V();
    current_mA = this->getCurrent_mA();
    power_mW = this->getPower_mW();
    overCurrent = (current_mA > 10000); // greater than 10 Amps
    overVoltage = (busVoltage > 31.9);  
}

/*!
 *  @brief  return the status of the power monitor as a string
 *  @return string representing status of device
 */
String Adafruit_INA219::toString(){

    String str = "[" 
    + String(this->busVoltage) + "," 
    + this->current_mA + ","
    + this->power_mW + ","
    + this->online + ","
    + this->overCurrent + ","
    + this->overVoltage + ","
    + "]";
    return String(str);
}