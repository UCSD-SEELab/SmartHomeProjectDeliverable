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

// Sensor.h

#ifndef _SENSOR_h
#define _SENSOR_h
#include "logger.h"
#include "application.h"

#include "SdFat.h"

#include "PhotonConfig.h"
#include "SHT1x.h"

#include "AFE.h"
#include "PowerManager.h"
extern PowerManager PM;
extern SdFat SD;

class Sensor
{
public:
	typedef struct {
		//float bar_T; //Temperature of the barometeric sensor in Celsius
		//float bar_P; //Barometric pressure in millibar
		//float hum_T; //Temperature of the humidity sensor in Celsius
		//float hum_H; //Humidity in percent
		int16_t bar_T; //Temperature of the barometeric sensor in Celsius
		uint16_t bar_P; //Barometric pressure in millibar
		int16_t hum_T; //Temperature of the humidity sensor in Celsius
		uint16_t hum_H; //Humidity in percent
	} Reading_Raw_t;

	Sensor(byte humClkPin, byte humDataPin, byte barCSPin,
		byte sdCSPin, byte ucCSPin, uint8_t i2cAddress0, uint8_t i2cAddress1);

	void storeEEPROMConfig();
	SensorEEPROMConfig_t* getConfig();

	Reading_Raw_t* readAll();
	Reading_Raw_t lastReading;
	AFE afe;

	bool isBluetoothSerialConnected();
	bool lastBluetoothSerialConnected;

	void begin();
	File logFile;

	// unsigned long now();
	// void setTime(unsigned long time);

	//SPI stuff
	enum spiDevice { SDCARD, BAR, UC_SPI, NONE };
	void selectSPI(spiDevice spiID);
	char const *filename = "MS000000.log";

	bool isWiFiEnabled();
	bool isSDAvail();
	bool isSleepEnabled();
	bool isSleepEnabledByConfig();
	bool isUSBSerialConnected();
	bool isUSBChargerConnected();
	double getPowerLevel();
	double getInVoltage();

	void initWakeupPinStatus();
	void checkWakeupPinStatus();
private:
	bool wakeupPinStatus;

	bool SDAvail;
	unsigned long _now;
	unsigned long _lastMillis;

	union {
		int16_t num;
		uint8_t buf[2];
	} fileNum;

	SHT1x sht1x;
	byte _humClkPin;
	byte _humDataPin;

	byte _barCSPin;
	byte _sdCSPin;
	byte _ucCSPin;

	void sdcard_Init(void);
	//Barometric sensor stuff
	uint16_t c1, c2, c3, c4, c5, c6;
	uint16_t c1plus24576, c2mul4, c5mul8, c6plus50;
	int16_t c4minus512;
	uint16_t ut1;
	void barometer_Init(void);
	void barometer_reset(void);
	uint16_t convertWtoC(uint8_t index, uint16_t W1, uint16_t W2, uint16_t W3, uint16_t W4);
	uint16_t getW(uint8_t index);
	uint16_t getD(uint8_t index);
	uint16_t get16(uint8_t type, uint8_t cmd1, uint8_t cmd2);
};

#endif
