// ams_IAQCORE.cpp
//
// Provides an interface for the ams iAQ-CORE C and P products. The iAQ-CORE is
// MOS-based VOC sensor with an I2C interface.
//

#include "ams_IAQCORE.h"

ams_IAQCORE::ams_IAQCORE() {
  prevReading.status = 0xFF;
}

// begin
// Initializes I2C interface at correct speed.
void ams_IAQCORE::begin() {
  if (Wire.isEnabled())  {
    Wire.end();
  }
  Wire.setSpeed(CLOCK_SPEED_100KHZ);
  Wire.begin();
}

// readData
// Reads all data from connected ams iAQ-CORE device over I2C
// 9 B are expected (see ams_IAQCORE.h for more info)
//
// Returns 1 for success, 0 for failure to read
// If system failed to read, status will read 0xFF and all others will be 0
uint8_t ams_IAQCORE::readData() {
  uint8_t i_Loop = 0;

  Wire.requestFrom(IAQ_ADDRESS, 9);

  while(Wire.available() < 9) {
    if (i_Loop > 10) {
      prevReading.co2_ppm = 0;
      prevReading.status = 0xFF;
      prevReading.resistance = 0;
      prevReading.tvoc_ppb = 0;
      return 0;
    }
    else {
      i_Loop++;
      delay(1);
    }
  }

  prevReading.co2_ppm = Wire.read() << 8;       // byte 0
  prevReading.co2_ppm += Wire.read();           // byte 1
  prevReading.status = Wire.read();             // byte 2
  prevReading.resistance = Wire.read() << 24;   // byte 3
  prevReading.resistance += (Wire.read() << 16);// byte 4
  prevReading.resistance += (Wire.read() << 8); // byte 5
  prevReading.resistance += Wire.read();        // byte 6
  prevReading.tvoc_ppb = Wire.read() << 8;      // byte 7
  prevReading.tvoc_ppb += Wire.read();          // byte 8

  return !(prevReading.status);
}

// getCO2
// Returns CO2 reading in ppm
uint16_t ams_IAQCORE::getCO2() {
  return prevReading.co2_ppm;
}

// getStatus
// Returns status.
//   OK (0x00)
//   WARMUP (0x10)
//   BUSY (0x01)
//   ERROR (0x80)
uint8_t ams_IAQCORE::getStatus() {
  return prevReading.status;
}

// getResistance
// Returns resistance of heating element
int32_t ams_IAQCORE::getResistance() {
  return prevReading.resistance;
}

// getTVOC
// Returns TVOC in ppb
uint16_t ams_IAQCORE::getTVOC() {
  return prevReading.tvoc_ppb;
}
