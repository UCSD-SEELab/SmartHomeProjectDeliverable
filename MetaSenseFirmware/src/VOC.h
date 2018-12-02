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

// VOC.h

#ifndef _VOC_h
#define _VOC_h
#include "PhotonConfig.h"
#include "application.h"
#include "Adafruit_ADS1015.h"
//#include "JsonDeserializer.h"
#include "ams_IAQCORE.h"
#include "AFE.h"

#define ADDRESS_VOCCONFIG sizeof(AFE::AFEConfig_t)

class VOC
{
public:
	typedef int32_t VOC_ppb_t;
	typedef int32_t VOC_uV_t;  // Single LSB results in 62 uV change

  // Config has parameters for a two-point calibration of the sensors with
  // temperature (F) and humidity (RH). Temp and humidity are not currently used
  // but sensors are affected and params may be incorporated in future.
	typedef struct {
    uint8_t PID_ID;
    uint8_t IAQ_ID;
	  int32_t reading_PID_uV[2];    // Output of sensor (uV)
    int32_t reading_PID_ppb[2];   // Actual ppb of cal gas
    int16_t reading_IAQ_count[2]; // Output of sensor (ppb)
    int16_t reading_IAQ_ppb[2];   // Actual ppb of cal gas
    uint16_t pressure_mbar;
    uint16_t temp_F;
    uint16_t hum_RH;
    int32_t sensitivity_PID_ppb_per_uV_Q10;
    int16_t sensitivity_IAQ_ppb_per_uV_Q10;
	} VOCConfig_t;

	typedef struct {
		adsGain_t range;
		int16_t val_PID;
    VOC_ppb_t val_IAQ;
	} VOC_Raw_t;

	typedef struct {
		VOC_ppb_t VOC_PID_ppb;
    VOC_ppb_t VOC_IAQ_ppb;
	} VOC_Model_t;

  // Initilization
	VOC(uint8_t i2cAddress0);

  // Functions
	void begin();
	VOC_Raw_t* readAll();
	VOC_uV_t getVoltage(int16_t val_bits);
	VOC_Model_t* convertModel(uint16_t temp_F, uint16_t hum_RH, uint16_t bar_mB);
  void storeConfig(VOCConfig_t* config_in);
  VOCConfig_t* getConfig();
  void clearConfig();

  // Variables
	VOC_Raw_t lastReading;
	VOC_Model_t lastModel;
  VOCConfig_t config;
private:
  // PID VOC sensors output an analog voltage between 40 mV and 2.5 V.
  // The ADC has an adjustable gain that effectively scales the measurable range:
  // Gain  Vmax
  // 1.0   4.096 V
  // 2.0   2.048 V
	adsGain_t currentGain = GAIN_TWO;

	Adafruit_ADS1115 ads0;
  ams_IAQCORE iaq0;
};


#endif
