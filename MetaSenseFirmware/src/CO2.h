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

// CO2.h

#ifndef _CO2_h
#define _CO2_h
#include "PhotonConfig.h"
#include "application.h"
#include "telaire_T6713.h"
//#include "JsonDeserializer.h"
#include "AFE.h"

class CO2
{
public:
  // Config has parameters for a linear calibration of a sensor with
  // temperature (F) and humidity (RH). This is currently not implemented.
	typedef struct {
    uint8_t ID;
    uint16_t slope_Q10;
    uint16_t offset;
    uint16_t pressure_mbar;
    uint16_t temp_F;
    uint16_t hum_RH;
	} CO2Config_t;

	typedef struct {
		uint16_t val;
    uint16_t status;
	} CO2_Raw_t;

	typedef struct {
		uint16_t CO2_ppm;
	} CO2_Model_t;

  // Initilization
	CO2(); // TODO Initialize with a desired address

  // Functions
	void begin();
	CO2_Raw_t* readAll();
	CO2_Model_t* convertModel(uint16_t temp_F, uint16_t hum_RH, uint16_t bar_mB);

  // Variables
	CO2_Raw_t lastReading;
	CO2_Model_t lastModel;
  CO2Config_t config;
private:
  telaire_T6713 CO2Sensor;

};

#endif
