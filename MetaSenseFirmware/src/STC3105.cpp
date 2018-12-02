// Reference sources:
//	https://github.com/ahmadexp/STC3105-Arduino
// 	https://github.com/egorf/STC3105_driver
// Edited by CSC

#include <STC3105.h>

// Defines ///////////////////////////////////////////////////////////

// The Arduino two-wire interface uses a 7-bit number for the address,
// and sets the last bit correctly based on reads and writes

#define STC_3105_ADDRESS 0b1110000


// Constructors //////////////////////////////////////////////////////

STC3105::STC3105()
{
  address = STC_3105_ADDRESS;
}

// Public Methods ////////////////////////////////////////////////////

// sets or detects slave address; returns bool indicating success
uint8_t STC3105::init(float R_cg)
{
  address 		= STC_3105_ADDRESS;
  resistor_cg = R_cg;
  return testWhoAmI();
}

// turns on the fuel guage
void STC3105::enableOperation(void)
{
  uint8_t tmp=readReg(STC_3105_REG_MODE);
  writeReg16(STC_3105_REG_MODE, tmp|STC_3105_GG_RUN);
}

// writes register
void STC3105::writeReg(uint8_t reg, uint8_t value)
{
  Wire.beginTransmission(address);
  Wire.write(reg);
  Wire.write(value);
  Wire.endTransmission();
}

// reads register
uint8_t STC3105::readReg(uint8_t reg)
{
  uint8_t value;

  Wire.beginTransmission(address);
  Wire.write(reg);
  Wire.endTransmission(false); // restart
  Wire.requestFrom(address, (uint8_t)1);
  value = Wire.read();
  Wire.endTransmission();
  return value;
}

void STC3105::writeReg16(uint8_t reg, int16_t value)
{
  Wire.beginTransmission(address);
  Wire.write(reg);
  Wire.write(value&0xFF);
  Wire.write(value>>8);
  Wire.endTransmission();
}

// reads register
int16_t STC3105::readReg16(uint8_t reg)
{
  uint16_t value;

  Wire.beginTransmission(address);
  Wire.write(reg);
  Wire.endTransmission(false); // restart
  Wire.requestFrom(address, (uint8_t)2);
  value = Wire.read();
  value |= Wire.read()<<8;
  Wire.endTransmission();
  return value;
}

// reads Charge in uVh unit
float STC3105::readCharge(void)
{
  return (float)readReg16(STC_3105_REG_CHARGE_LOW)*CHARGE_RESOLUTION;
}

// reads coulomb counter - number of conversions completed
uint16_t STC3105::readCounter(void)
{
  return readReg16(STC_3105_REG_COUNTER_LOW);
}

// reads Current in mA unit
float STC3105::readCurrent(void)
{
  // assuming the register gives voltage drop
  float voltageCG = (float)readReg16(STC_3105_REG_CURRENT_LOW)*CURRENT_RESOLUTION; // in uV
  float currentCG = voltageCG / resistor_cg;
  return currentCG / 1000;
}

// reads voltage in mV unit
float STC3105::readVoltage(void)
{
  return (float)readReg16(STC_3105_REG_VOLTAGE_LOW)*VOLTAGE_RESOLUTION;
}

// read the state of charge base offset
uint16_t STC3105::readBaseSOC(void)
{
  return readReg16(STC_3105_REG_SOC_BASE_LOW);
}

// read the state of charge base offset
uint16_t STC3105::setBaseSOC(uint16_t baseSOCLevel)
{
  writeReg16(STC_3105_REG_SOC_BASE_LOW, baseSOCLevel);
  return readReg16(STC_3105_REG_SOC_BASE_LOW);
}

void STC3105::resetAccumulator(void)
{
  uint8_t tmp=readReg(STC_3105_REG_CTRL);
  writeReg(STC_3105_REG_CTRL, tmp|STC_3105_GG_RST);
}

void STC3105::POR(uint8_t flag)
{
  uint8_t tmp=readReg(STC_3105_REG_CTRL);
  if(flag)
    writeReg16(STC_3105_REG_CTRL, tmp&(~STC_3105_PORDET));
  else
    writeReg16(STC_3105_REG_CTRL, tmp|STC_3105_PORDET);
}

// Writing the IO0DATA bit to 0 forces the ALM output low; writing the IO0DATA bit to 1 lets the ALM output reflect the battery condition. Reading the IO0DATA bit gives the state of the ALM pin.
// The ALM pin remains low (even if the conditions disappear) until the software writes the ALM_VOLT and ALM_SOC bits to 0 to clear the interrupt.

void STC3105::enableAlarm(){
	uint16_t tmp;

	// Enable alarm status
	tmp = readReg(STC_3105_REG_MODE);
	writeReg(STC_3105_REG_MODE, tmp|STC_3105_ALM_ENA);

	// Make the alarm pin reflect ALM status
	tmp = readReg(STC_3105_REG_CTRL);
	writeReg(STC_3105_REG_CTRL, tmp|STC_3105_IO0DATA);
}

uint16_t STC3105::setSOCAlarm(uint16_t socLevel){
	writeReg16(STC_3105_REG_ALARM_SOC_LOW, socLevel);
	readReg16(STC_3105_REG_ALARM_SOC_LOW);
	return socLevel;
}

void STC3105::clearSOCAlarm(){
	uint16_t tmp = readReg16(STC_3105_REG_CTRL);
	writeReg16(STC_3105_REG_CTRL, tmp&(~STC_3105_ALM_SOC));
}

uint16_t STC3105::readCTRL(){
	return readReg16(STC_3105_REG_CTRL);
}

void STC3105::clearVoltageAlarm(){
	uint16_t tmp = readReg16(STC_3105_REG_CTRL);
	writeReg16(STC_3105_REG_CTRL, tmp&(~STC_3105_ALM_VOLT));
}

// Private Methods ///////////////////////////////////////////////////
uint8_t STC3105::testWhoAmI(void)
{
  return (readReg(STC_3105_REG_ID) == STC_3105_REG_ID_VALUE);
}
