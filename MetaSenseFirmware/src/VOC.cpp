// BSD 3-Clause License
//
// Copyright (c) 2018, The Regents of the University of California.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// * Neither the name of the copyright holder nor the names of its
//   contributors may be used to endorse or promote products derived from
//   this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

// VOC.cpp
//
// Handles reading and processing data received from mounted VOC sensors.
//

#include "VOC.h"

VOC::VOC(uint8_t i2cAddress0) : ads0(i2cAddress0) {
	// Default to datasheet (Mocon piD-TECH eVx Red Label) config values if config
  // is not in EEPROM. Otherwise, load saved values.
  if (EEPROM.read(ADDRESS_VOCCONFIG) == 0xFF) {
    config.reading_PID_uV[0] = 45000;     // 0.045 V
    config.reading_PID_uV[1] = 2500000;   // 2.5 V
    config.reading_PID_ppb[0] = 0;        // 0 ppb
    config.reading_PID_ppb[1] = 200000;   // 200 ppm
    config.sensitivity_PID_ppb_per_uV_Q10 = 83;

    config.reading_IAQ_count[0] = 0;     // 0 ppb reading
    config.reading_IAQ_count[1] = 600;   // 600 ppb reading
    config.reading_IAQ_ppb[0] = 0;        // 0 ppb
    config.reading_IAQ_ppb[1] = 600;   // 600 ppb actual
    config.sensitivity_IAQ_ppb_per_uV_Q10 = (1 << 10);
  }
  else {
    EEPROM.get(ADDRESS_VOCCONFIG, config);
  }

}

// begin
// Initializes VOC interface board and associated hardware
void VOC::begin() {
  // Pull EN_N line on interface board low to allow power to flow
  pinMode(VOCEnablePin, OUTPUT);
  digitalWrite(VOCEnablePin, LOW);

  // Initialize I2C interface to ADC
  ads0.begin();

  // Initialize VOC MOS sensor
  iaq0.begin();
}

// readAll
// Performs a single read from ADC and updates the values from the IAQ VOC
// sensor over I2C. If there is an error or the sensor is warming up, the actual
// value (vIA) will be -1 and vIP will show the error code. If the read failed,
// then the status will be 0xFF and all other values will be 0.
VOC::VOC_Raw_t* VOC::readAll(){
  ads0.setGain(currentGain);
  lastReading.range = currentGain;
  lastReading.val_PID= ads0.readADC_SingleEnded(3);
  if (iaq0.readData()) {
    lastReading.val_IAQ = iaq0.getTVOC();
  }
  else {
    lastReading.val_IAQ = -1;
  }

  return &lastReading;
}

// getVoltage
// Converts LSBs from ADC (15 bit) to voltage (uV)
VOC::VOC_uV_t VOC::getVoltage(int16_t val_bits){
	int32_t gain_num = 2;         // fixed gain of 2. See notes under VOC.h for
	int32_t gain_denom = 1;       // additional details
	int32_t maxBit = 32767;       // (2^15 - 1) bits
	int32_t maxVolt_uV = 4096000; // ADS1115 uses a 4.096 V reference

	int32_t val_uV = ((maxVolt_uV * gain_denom + (1<<14)) / maxBit) * val_bits / gain_num;

	return val_uV;
}

// convertModel
// For PID, converts voltage (uV) to ppb reading for TVOC cocentration using the
// calibration values. For IAQ, applies a calibration factor between sensor ppb
// and actual ppb readings.
VOC::VOC_Model_t* VOC::convertModel(uint16_t temp_C, uint16_t hum_RH, uint16_t bar_mB){
  VOC_uV_t VOC_uV = getVoltage(lastReading.val_PID);
  VOC_uV = VOC_uV - config.reading_PID_uV[0];
  VOC_uV = (VOC_uV > 0) ? VOC_uV : 0;     // Calculation overflows for negatives

  lastModel.VOC_PID_ppb = ((VOC_uV*config.sensitivity_PID_ppb_per_uV_Q10 + (1<<9)) >> 10)
    + config.reading_PID_ppb[0];

  // If error in reading,
  // TODO Update to include calibration if required.
  if (lastReading.val_IAQ < 0) {
    lastModel.VOC_IAQ_ppb = iaq0.getStatus();
  }
  else {
    lastModel.VOC_IAQ_ppb = lastReading.val_IAQ;
  }

	return &lastModel;
}

// storeConfig
// Stores a configuration into non-volatile memory (EEPROM-esque). AFE
// configuration is stored in EEPROM starting at 0.
void VOC::storeConfig(VOCConfig_t* config_in) {
  EEPROM.put(ADDRESS_VOCCONFIG, config_in);
}

// getConfig
// Retrieves configuration from non-volatile memory
VOC::VOCConfig_t* VOC::getConfig() {
  VOCConfig_t config_out;
  EEPROM.get(ADDRESS_VOCCONFIG, config_out);

  return &config_out;
}

// clearConfig
// Resets the IDs of the two sensors to 0xFF, indicating that no data is present
// TODO Update to clear the stored configuration values to either default or all
// 0xFF, depending on what's appropriate for application.
void VOC::clearConfig() {
  VOCConfig_t temp;

  temp.PID_ID = 0xFF;
  temp.IAQ_ID = 0xFF;
  EEPROM.put(ADDRESS_VOCCONFIG, temp);
}
