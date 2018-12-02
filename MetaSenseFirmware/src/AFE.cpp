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

#include "AFE.h"

AFE::AFE(uint8_t i2cAddress0, uint8_t i2cAddress1) : ads0(i2cAddress0), ads1(i2cAddress1) {
	//initAFEConfiguration();
	//if (configuration.magic != 'M') {
	//	storedConfig = false;
		//should get this from EPROM specific for each node and AFE
		//memcpy(configuration.serial, "19054690", 8);
		//configuration.serial[8] = 0;
		//configuration.PT20C_V = 0.321766; // 0.32176628;
		//configuration.magic = 'M';
		//configuration.calibration[0].type = NO2;
		//configuration.calibration[0].WE_E0 = 285;
		//configuration.calibration[0].WE_S0 = -46;
		//configuration.calibration[0].AE_E0 = 295;
		//configuration.calibration[0].AE_S0 = -20;
		//configuration.calibration[0].Sensitivity = 201;
		//configuration.calibration[0].NO2_Sensitivity = 201;
		//configuration.calibration[1].type = OX;
		//configuration.calibration[1].WE_E0 = 420;
		//configuration.calibration[1].WE_S0 = -48;
		//configuration.calibration[1].AE_E0 = 419;
		//configuration.calibration[1].AE_S0 = -32;
		//configuration.calibration[1].Sensitivity = 268;
		//configuration.calibration[1].NO2_Sensitivity = 273;
		//configuration.calibration[2].type = CO;
		//configuration.calibration[2].WE_E0 = 269;
		//configuration.calibration[2].WE_S0 = 47;
		//configuration.calibration[2].AE_E0 = 278;
		//configuration.calibration[2].AE_S0 = 0;
		//configuration.calibration[2].Sensitivity = 212;
		//configuration.calibration[2].NO2_Sensitivity = 0;
	//}
}

void AFE::begin() {
  ads0.begin();
  ads1.begin();
}

//AFE::SensorCalibration_t* AFE::getSensorCalibration(readingType_t sensor){
//	if (sensor == NO2)
//		return &configuration.calibration[0];
//	if (sensor == OX)
//		return &configuration.calibration[1];
//	return &configuration.calibration[2];
//}
//
//byte AFE::computeShift(adsGain_t currentGain) {
//	byte ret = 0;
//	if (currentGain == GAIN_TWO)
//		ret = 1;
//	else if (currentGain == GAIN_FOUR)
//		ret = 2;
//	else if (currentGain == GAIN_EIGHT)
//		ret = 3;
//	else if (currentGain == GAIN_SIXTEEN)
//		ret = 4;
//	return ret;
//}

adsGain_t AFE::incGain(adsGain_t currentGain) {
  adsGain_t ret = GAIN_SIXTEEN;
  if (currentGain == GAIN_TWOTHIRDS)
    ret = GAIN_ONE;
  if (currentGain == GAIN_ONE)
    ret = GAIN_TWO;
  else if (currentGain == GAIN_TWO)
    ret = GAIN_FOUR;
  else if (currentGain == GAIN_FOUR)
    ret = GAIN_EIGHT;
  else if (currentGain == GAIN_EIGHT)
    ret = GAIN_SIXTEEN;
  return ret;
}

adsGain_t AFE::decGain(adsGain_t currentGain) {
  adsGain_t ret = GAIN_TWOTHIRDS;
  if (currentGain == GAIN_TWO)
    ret = GAIN_ONE;
  else if (currentGain == GAIN_FOUR)
    ret = GAIN_TWO;
  else if (currentGain == GAIN_EIGHT)
    ret = GAIN_FOUR;
  else if (currentGain == GAIN_SIXTEEN)
    ret = GAIN_EIGHT;
  return ret;
}


void AFE::updateCurrentGain() {
	int16_t maxVal = 0;
	if (maxVal < lastReading.ADC0_0)
		maxVal = lastReading.ADC0_0;
	if (maxVal < lastReading.ADC0_1)
		maxVal = lastReading.ADC0_1;
	if (maxVal < lastReading.ADC0_2)
		maxVal = lastReading.ADC0_2;
	if (maxVal < lastReading.ADC0_3)
		maxVal = lastReading.ADC0_3;
	if (maxVal < lastReading.ADC1_0)
		maxVal = lastReading.ADC1_0;
	if (maxVal < lastReading.ADC1_1)
		maxVal = lastReading.ADC1_1;
	if (maxVal < lastReading.ADC1_2)
		maxVal = lastReading.ADC1_2;
	if (maxVal < lastReading.ADC1_3)
		maxVal = lastReading.ADC1_3;


  if (maxVal>30000)
    currentGain = decGain(currentGain);
  else if (maxVal<10000)
    currentGain = incGain(currentGain);


//	maxVal = maxVal >> computeShift(currentGain);
//
//	if (1 > 0x7FFF / maxVal)
//		currentGain = GAIN_TWOTHIRDS;
//	else if (2 > 0x7FFF / maxVal)
//		currentGain = GAIN_ONE;
//	else if (4 > 0x7FFF / maxVal)
//		currentGain = GAIN_TWO;
//	else if (8 > 0x7FFF / maxVal)
//		currentGain = GAIN_FOUR;
//	else if (16 > 0x7FFF / maxVal)
//		currentGain = GAIN_EIGHT;
//	else
//		currentGain == GAIN_SIXTEEN;
}

//AFE::Gas_Raw_t* AFE::getLastReading() {
//	return &lastReading;
//}

AFE::Gas_Raw_t* AFE::readAll(){
  ads0.setGain(currentGain);
  ads1.setGain(currentGain);
  lastReading.range = currentGain;
  lastReading.ADC0_0 = ads0.readADC_SingleEnded(0);
  lastReading.ADC0_1 = ads0.readADC_SingleEnded(1);
  lastReading.ADC0_2 = ads0.readADC_SingleEnded(2);
  lastReading.ADC0_3 = ads0.readADC_SingleEnded(3); // unused by AFE
  lastReading.ADC1_0 = ads1.readADC_SingleEnded(0);
  lastReading.ADC1_1 = ads1.readADC_SingleEnded(1);
  lastReading.ADC1_2 = ads1.readADC_SingleEnded(2);
  lastReading.ADC1_3 = ads1.readADC_SingleEnded(3);
  updateCurrentGain();
  return &lastReading;
}

// Reads a 16 bit output from the ADC and outputs the voltage detected
AFE::Gas_mV_t AFE::getVoltage(adsGain_t range, int16_t val_bits){
	// range is type of adsGain_t, which is an enum representing 2/3, 1, 2, 4...
	float gain = 2.0 / 3.0;

	if (range > 0){
		gain = range;
	}

    float maxBit = 32767;
    float maxVolt = 4.096 * 1000;
    float val_mv = maxVolt * ( val_bits / (gain * maxBit) );

	return val_mv;
}

AFE::Gas_PPB_t AFE::modelNO2(AFE::Gas_mV_t NO2A_mV, AFE::Gas_mV_t NO2W_mV, uint16_t temp_C, uint16_t hum_RH){
	Gas_PPB_t NO2_ppb;

	// NO2 ppb model MDL4 based on AFE729 (Temperatures in Celsius, RH in percent)
	// Note: may display negative values
	double x		= 10.85858796;
	double x1	= -2.042332131;
	double x2	= 0.9053671045;
	double x3	= 9.70E-03;
	double x4	= -4.01E-02;
	double x5	= 2.59E-02;
	double x6	= 5.76E-03;
	double x7	= 0.01932777913;
	double x8	= -0.02070809566;
	double x9	= -0.007235449385;

	NO2_ppb = x + x1*(NO2W_mV - NO2A_mV) + x2*hum_RH + x3*(NO2W_mV - NO2A_mV)*NO2A_mV + x4*(NO2W_mV - NO2A_mV)*temp_C + x5*(NO2W_mV - NO2A_mV)*hum_RH + x6*temp_C*hum_RH + x7*(NO2W_mV - NO2A_mV)*(NO2W_mV - NO2A_mV) + x8*(temp_C * temp_C) + x9*( hum_RH * hum_RH );

	/*
	// NO2 ppb model #4 from preliminary model set
    float c1 =    12.818;
    float c2 =    12.346;
    float c3 =    -0.13832;
    float c4 =    -0.8486;
    float c5 =    -0.15373;
    float c6 =    -0.070753;
    float c7 =    0.034065;

    NO2_ppb     = c1 \
                + c2*(NO2W_mV - NO2A_mV)  \
                + c3*(temp_F)  \
                + c4*(hum_RH)  \
                + c5*((NO2W_mV - NO2A_mV)*temp_F)  \
                + c6*((NO2W_mV - NO2A_mV)*hum_RH)  \
                + c7*(temp_F*hum_RH);
	*/

	return NO2_ppb;
}

AFE::Gas_PPM_t AFE::modelOX(AFE::Gas_mV_t OXA_mV, AFE::Gas_mV_t OXW_mV, \
							uint16_t temp_C, uint16_t hum_RH, \
							AFE::Gas_mV_t NO2A_mV, AFE::Gas_mV_t NO2W_mV) {
	Gas_PPM_t OX_ppm;

	// OX ppm model MDL2 based on AFE729 (Temperatures in Celsius, RH in percent)
	// Note: may display negative values
	double x		= 0.01742770311;
	double x1	= 0.0007640929485;
	double x2	= -0.001646046129;
	double x3	= -0.0001142313228;
	double x4	= -0.0002805577051;

	OX_ppm = x + x1*(OXW_mV - OXA_mV) + x2*(NO2W_mV - NO2A_mV) + x3*temp_C + x4*hum_RH	;

	/*
	// OX ppb model #3 from preliminary model set
    float c1 =    0.043652;
    float c2 =    0.00034356;
    float c3 =    -0.00069622;
    float c4 =    -2.31E-05;
    float c5 =    -0.00033241;

    OX_ppb 		= c1 \
				+ c2*(OXW_mV - OXA_mV)  \
				+ c3*(NO2_ppb) \
				+ c4*(temp_F) \
				+ c5*(hum_RH);
	*/

	return OX_ppm;
}

AFE::Gas_PPM_t AFE::modelCO(AFE::Gas_mV_t COA_mV, AFE::Gas_mV_t COW_mV, uint16_t temp_C, uint16_t hum_RH){
	Gas_PPM_t CO_ppm;

	// CO ppm model MDL3 based on AFE729 (Temperatures in Celsius, RH in percent)
	// Note: may display negative values
  // TODO: Check CO model since other models use (gasW - gasA)
	double x		= -0.9893169905;
	double x1	= 0.003416631473;
	double x2	= 0.02870125247;
	double x3	= -2.44E-03;
	double x4	= -1.06E-04;
	double x5	= 8.87E-06;
	double x6	= 1.65E-06;
	double x7	= 0.000253121356;

	CO_ppm = x + x1*COW_mV + x2*temp_C + x3*hum_RH + x4*(COW_mV*temp_C) + x5*(COW_mV*hum_RH) + x6*( COW_mV * COW_mV ) + x7* ( temp_C * temp_C );

	/*
	// CO ppm model #4 from preliminary model set (Temperatures in Fahrenheit)
    float c1 =    -0.064883;
    float c2 =    0.0088855;
    float c3 =    0.0044046;
    float c4 =    0.0052814;
    float c5 =    -7.09E-05;
    float c6 =    -0.00017777;

    CO_ppm     = c1 \
                + c2*(COW_mV - COA_mV)  \
                + c3*(temp_F)  \
                + c4*(hum_RH)  \
                + c5*((COW_mV - COA_mV)*temp_F)  \
                + c6*(temp_F*hum_RH);
	*/

	return CO_ppm;
}

// Generate Air Quality Index based on ppb/ppm values of various gases
// Reference: https://www3.epa.gov/airnow/aqi_brochure_02_14.pdf
// https://airnow.gov/index.cfm?action=resources.conc_aqi_calc
// Note: Minimum is set at 0, negative values are simply dropped

// An AQI of 100 for ozone corresponds to an ozone level of 0.075 parts per million (averaged over 8 hours).
// An AQI of 100 for carbon monoxide corresponds to a level of 9 parts per million (averaged over 8 hours).
// NO2 100 - 100

AFE::Gas_AQI_t AFE::modelAQI(AFE::Gas_PPB_t NO2_ppb, AFE::Gas_PPM_t OX_ppm, AFE::Gas_PPM_t CO_ppm){
	Gas_AQI_t aqi = 0;

	int NO2_aqi = NO2_ppb;
	int CO_aqi 	= CO_ppm * 100 / CO_Std;
	int OX_aqi 	= OX_ppm * 100 / OX_Std;

	aqi = NO2_aqi;

	if (CO_aqi > aqi){
		aqi = CO_aqi;
	}
	if (OX_aqi > aqi){
		aqi = OX_aqi;
	}

	if (aqi < 0)
		aqi = 0;

	return aqi;
}

AFE::Gas_Model_t* AFE::convertModel(uint16_t temp_C, uint16_t hum_RH, uint16_t bar_mB){
	// SN1: NO2, SN2: OX, SN3: CO
	//TODO model should read AFE temperature instead of humidity sensor temperature
	uint16_t temp_F = temp_C * 1.8+32;

	Gas_mV_t SN1A_mV = getVoltage(lastReading.range, lastReading.ADC0_0);
	Gas_mV_t SN1W_mV = getVoltage(lastReading.range, lastReading.ADC0_1);
	Gas_mV_t SN2A_mV = getVoltage(lastReading.range, lastReading.ADC1_2);
	Gas_mV_t SN2W_mV = getVoltage(lastReading.range, lastReading.ADC1_3);
	Gas_mV_t SN3A_mV = getVoltage(lastReading.range, lastReading.ADC1_0);
	Gas_mV_t SN3W_mV = getVoltage(lastReading.range, lastReading.ADC1_1);

  lastModel.SN1_ppb = modelNO2(SN1A_mV, SN1W_mV, temp_C, hum_RH);
  lastModel.SN2_ppm = modelOX(SN2A_mV, SN2W_mV, temp_C, hum_RH, SN1A_mV, SN1W_mV); // in PPM
  lastModel.SN3_ppm = modelCO(SN3A_mV, SN3W_mV, temp_C, hum_RH); // in PPM
  lastModel.temp_F = temp_F;
  lastModel.hum_RH = hum_RH;
  lastModel.AQI = modelAQI(lastModel.SN1_ppb, lastModel.SN2_ppm, lastModel.SN3_ppm);

  return &lastModel;
}

//AFE::AFEConfig_t* AFE::getAFEConfiguration(){
//  return &configuration;
//}

/*AFE::AFEConfig_t* AFE::initAFEConfiguration() {
	EEPROM.get(0, configuration);
	//if (configuration.magic!='M')
	//	memset(&configuration, 0, sizeof(configuration));
	return &configuration;
}

void AFE::ResetAFE() {
	//memcpy(&configuration, cfg, sizeof(configuration));
	//configuration.magic = 'M';
	EEPROM.put(0, configuration);
}*/
