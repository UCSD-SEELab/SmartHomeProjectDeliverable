// telaire_T6713.cpp
//
// Provides an interface for the Telaire T6713 CO2 monitoring module over I2C
// using the MODBUS protocol.
//

#include "telaire_T6713.h"

telaire_T6713::telaire_T6713() {
  prevReading.status = 0xFFFF;
  prevReading.firmwareRev = 0x0000;
}


// begin
// Initializes I2C interface at correct speed.
void telaire_T6713::begin() {
  if (Wire.isEnabled())  {
    Wire.end();
  }
  Wire.setSpeed(CLOCK_SPEED_100KHZ);
  Wire.begin();

  delay(1);

  readFirmwareRev();
}


// readAll
// Reads all data from connected Telaire T6713 device over I2C
//
// Returns 1 for success, 0 for failure to read
// If system failed to read, status will read 0xFF and co2_ppm will be 0
uint8_t telaire_T6713::readAll() {
  readStatus(); // TODO For some reason, status is always returning 0

  return (readData());
}


// readData
// Reads CO2 data from connected Telaire T6713 device over I2C
//
// Returns 1 for success, 0 for failure to read
uint8_t telaire_T6713::readData() {
  prevReading.co2_ppm  = readReg(CO2_REG_GAS_PPM);

  return (prevReading.co2_ppm > 0);
}

// readFirmwareRev
// Reads firmware revision from connected Telaire T6713 device over I2C
//
// Returns 1 for success, 0 for failure to read
uint8_t telaire_T6713::readFirmwareRev() {
  prevReading.firmwareRev = readReg(CO2_REG_FIRMWAREREV);

  return (prevReading.firmwareRev > 0);
}


// readStatus
// Reads status from connected Telaire T6713 device over I2C
//
// Returns 1 for success, 0 for failure to read
uint8_t telaire_T6713::readStatus() {
  prevReading.status = readReg(CO2_REG_STATUS);

  return (prevReading.status > 0);
}


// readReg
// Reads the targetted register from connected Telaire T6713 device over I2C.
// Response is 2B. For status codes, see telaire_T6713.h
//
// Returns value for success, 0 for failure to read
// If system failed to read, status will read 0xFF and co2_ppm will be 0
uint16_t telaire_T6713::readReg(uint16_t reg) {
  uint16_t i_Loop = 0;

  Wire.beginTransmission(CO2_ADDRESS);
  Wire.write(CO2_CMD_READ);           // Function code
  Wire.write((uint8_t)(reg >> 8));    // Starting address (MSB)
  Wire.write((uint8_t)(reg & 0xFF)); // Starting address (LSB)
  Wire.write(0x00);                   // Number of registers to read (MSB)
  Wire.write(0x01);                   // Number of registers to read (LSB)
  Wire.endTransmission();

  Wire.requestFrom(CO2_ADDRESS, 4);

  while(Wire.available() < 4) {
    if (i_Loop > 5) {
      setError();
      return 0;
    }
    else {
      i_Loop++;
      delay(1);
    }
  }

//  if (!(Wire.read() == 0x04) && !(Wire.read() == 0x02)) {
//    return 0;
//  }
  Wire.read();    // Function response
  Wire.read();    // Number of bytes (always two since only 1 register read)
  i_Loop = Wire.read() << 8;    // byte 0
  i_Loop += Wire.read();        // byte 1

//  Serial.println(i_Loop, HEX);

  return (i_Loop);
}


// reset
// Resets the device. Upon receipt, the module will immediately reset.
void telaire_T6713::reset() {
  Wire.beginTransmission(CO2_ADDRESS);
  Wire.write(CO2_CMD_WRITE);          // Function code
  Wire.write((uint8_t)(CO2_REG_RESET >> 8));    // Starting address (MSB)
  Wire.write((uint8_t)(CO2_REG_RESET & 0xFF)); // Starting address (LSB)
  Wire.write(0xFF);
  Wire.write(0x00);
  Wire.endTransmission();
}


// setError
// Used when an error is encountered at any time during the Telaire
// communication process.
inline void telaire_T6713::setError() {
  prevReading.co2_ppm = 0;
  prevReading.status = 0xFFFF;
}


// getCO2
// Returns CO2 reading in ppm
uint16_t telaire_T6713::getCO2() {
  return prevReading.co2_ppm;
}


// getStatus
// Returns status.
// For full list of possible statuses, see helaire_T6713.h
uint16_t telaire_T6713::getStatus() {
  return prevReading.status;
}

// getFirmwareRev
// Returns firmware revision, which is read only in the begin() function.
uint16_t telaire_T6713::getFirmwareRev() {
  return prevReading.firmwareRev;
}
