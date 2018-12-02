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

// AFE.h

#ifndef _AFE_h
#define _AFE_h

#include "application.h"
// #if defined(ARDUINO) && ARDUINO >= 100
// #include "arduino.h"
// #else
// #include "WProgram.h"
// #endif

//#include <EEPROM.h>
#include "Adafruit_ADS1015.h"
//#include "JsonDeserializer.h"
#include "PhotonConfig.h"

#define	NO2_Std 100
#define	CO_Std 9
#define OX_Std 0.075

class AFE
{
public:
	typedef double Gas_PPB_t;
	typedef double Gas_PPM_t;
	typedef double Gas_AQI_t;
	typedef double Gas_mV_t;

	typedef enum { NO2, OX, CO, Temp, ERR } readingType_t;
	typedef struct {
		readingType_t type;
		int32_t WE_E0; // in mV
		int32_t WE_S0; // in mV
		int32_t AE_E0; // in mV
		int32_t AE_S0; // in mV
		int32_t Sensitivity; // in mV/ppb
		int32_t NO2_Sensitivity; // in mV/ppb
	} SensorCalibration_t;
	typedef struct {
		//uint8_t magic; //Must be 0x15 used to check that the strucutre is initialized correctly
		char serial[9];
		int32_t PT20C_V; // in mV /100
		SensorCalibration_t calibration[3];
		long wifiStatus;
	} AFEConfig_t;
	typedef struct {
		adsGain_t range;
		uint16_t ADC0_0; // SN1A NO2
		uint16_t ADC0_1;	// SN1W NO2
		uint16_t ADC0_2; // PT
		uint16_t ADC0_3;	// unused
		uint16_t ADC1_0; // SN3A CO
		uint16_t ADC1_1; // SN3W CO
		uint16_t ADC1_2; // SN2A OX
		uint16_t ADC1_3; // SN2W OX
	} Gas_Raw_t;
	typedef struct {
		Gas_PPB_t SN1_ppb;
		Gas_PPB_t SN2_ppm;
		Gas_PPB_t SN3_ppm;
		Gas_AQI_t AQI;
		uint16_t temp_F;
		uint16_t hum_RH;
	} Gas_Model_t;
	//typedef struct {
	//	adsGain_t range;
	//	int16_t ADC0_0_1;
	//	int16_t ADC0_2_3;
	//	int16_t ADC1_0_1;
	//	int16_t ADC1_2_3;
	//} Gas_Raw_Diff_t;

	AFE(uint8_t i2cAddress0, uint8_t i2cAddress1);
	void begin();
	//void ResetAFE();
	Gas_Raw_t* readAll();

	Gas_mV_t getVoltage(adsGain_t range, int16_t val_bits);
	Gas_Model_t* convertModel(uint16_t temp_F, uint16_t hum_RH, uint16_t bar_mB);

	Gas_PPB_t modelNO2(Gas_mV_t NO2A_mV, Gas_mV_t NO2W_mV, uint16_t temp_C, uint16_t hum_RH);
	Gas_PPM_t modelOX(Gas_mV_t OXA_mV, Gas_mV_t OXW_mV, uint16_t temp_C, uint16_t hum_RH, Gas_mV_t NO2A_mV, Gas_mV_t NO2W_mV);
	Gas_PPM_t modelCO(Gas_mV_t COA_mV, Gas_mV_t COW_mV, uint16_t temp_C, uint16_t hum_RH);
	Gas_AQI_t modelAQI(Gas_PPB_t NO2_ppb, Gas_PPM_t OX_ppm, Gas_PPM_t CO_ppm);

	AFEConfig_t configuration;
	Gas_Raw_t lastReading;
	Gas_Model_t lastModel;

	//Gas_Raw_t* getLastReading();
	//SensorCalibration_t* getSensorCalibration(readingType_t sensor);
	//AFEConfig_t* getAFEConfiguration();
private:
	//adsGain_t currentGain = GAIN_TWOTHIRDS;

	//bool storedConfig = true;

	Adafruit_ADS1115 ads0;
	Adafruit_ADS1115 ads1;

	void updateCurrentGain();
	adsGain_t incGain(adsGain_t currentGain);
  adsGain_t decGain(adsGain_t currentGain);
  //byte computeShift(adsGain_t currentGain);
	//AFEConfig_t* initAFEConfiguration();

};

#endif
