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

// CO2.cpp
//
// Handles reading and processing data received from mounted CO2 sensors.
//

#include "CO2.h"

CO2::CO2() {
  config.ID = 0xFF;
  config.slope_Q10 = 1024;
  config.offset = 0;
  config.pressure_mbar = 1000;
  config.temp_F = 73;
  config.hum_RH = 50;
}

// begin
// Initializes Telaire CO2 sensor
void CO2::begin() {
  CO2Sensor.begin();
}

// readAll
// Since there is only one sensor, read the data from Telaire T6713
CO2::CO2_Raw_t* CO2::readAll(){
  if (CO2Sensor.readAll()) {
    lastReading.val = CO2Sensor.getCO2();
  }
  else {
    lastReading.val = 0;
  }

  return &lastReading;
}

// convertModel
// For Telaire T6713, calibration is performed on the module itself. If needed
// in the future, a linear calibration offset can be applied.
CO2::CO2_Model_t* CO2::convertModel(uint16_t temp_C, uint16_t hum_RH, uint16_t bar_mB){
  lastModel.CO2_ppm = lastReading.val;

	return &lastModel;
}
