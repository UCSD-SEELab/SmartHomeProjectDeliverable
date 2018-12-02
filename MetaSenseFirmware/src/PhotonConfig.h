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

#ifndef _PHOTONCONFIG_h
#define _PHOTONCONFIG_h

#include "application.h"
#include "Adafruit_ADS1015.h"

#define WATCHDOG_TIMEOUT 60000  //If the sensor is stuck for more than 1 min
#define BOARD_VERSION 2.2
#define MAX_MSG_LEN 350

#define Experimental_MQTTClientEnabled true
#define Experimental_RunNeuralNet true
#define MQTT_Server_Address "192.168.2.38"
//#define MQTT_Server_Address "192.168.10.6"
#define MQTT_Server_Port 61613
#define MQTT_Server_Username "admin"
#define MQTT_Server_Password "SamsungProject"
#define MQTT_Streaming_Default_Format json;

typedef enum { json, binB64, bin } MessageEncodingType_t;

typedef struct {
	bool wifiEnabled;
	bool sleepEnabled;
	bool vocInstalled;
	bool co2Installed;
	MessageEncodingType_t messageEncodingSD;
	MessageEncodingType_t messageEncodingBLE;
	MessageEncodingType_t messageEncodingCloud;
	MessageEncodingType_t messageEncodingMQTT;
	bool streamBLE;
	bool streamWiFi;
	bool streamMQTT;
	bool batchWiFi;
	bool streamSD;
	long intervalTime;
	char AFESerial[10];
	char MACAddress[20];
	char BLEBindType; // Set between 0 and 3
} SensorEEPROMConfig_t;

//const unsigned long defaultTimeInterval = 5000; // in milliseconds
const bool defaultMirrorToUSB = false;
//const bool forceSyncAtStartup = false;  		// blocks at setup until cloud connection succeeds
const bool stealthMode = false;					// turn off RGB LED during normal operation
const String wifiSSID = "littlesegway"; 		// CSC mobile hotspot
const String wifiPassword = "gogogogo"; 		// CSC mobile hotspot
const int8_t UTCOffset = -7;  					// -7 for Pacific
const uint8_t wifiAttempts = 1;					// how many times to look for wifi at setup
const bool deepSleepEnabled = false;			// requires setup() after wakeup
//const long init_wifiOnUntil = -1; // 0 disables , -1 on forever
//------------------
//retained variables
//------------------
//Access to variable in retained memory
extern bool usbMirror;
extern unsigned long lastSetupTime;
extern unsigned long lastReadingTime;
extern unsigned long nextSyncTime;
extern bool init;
extern SensorEEPROMConfig_t SensorConfig;
extern adsGain_t currentGain;
extern int ResetSequenceLen;
extern int BLE_KEY_PIN;
extern int UNCONNECTED_CS_PIN;

//------------------
//normal variables
//------------------
extern bool temporarlyDisableSleep;
extern bool usbPassthrough;
extern int scheduledSetupCommand;

/*=========================================================================
	Bluetooth Serial Connection Parameters
	-----------------------------------------------------------------------*/
//This is actually the system Key pin that will reset the module if LOW
//for more than a second

	//BOARD Version <2.2
	//#define BTEnablePin    D6 //NOTE: switched with D4 in board 2.2 (to support debug)
	//BOARD Version >=2.2
	//#define BTEnablePin    D4

// #if BOARD_VERSION_MAJ >= 2 && BOARD_VERSION_MIN >= 2
// 	 #define BTEnablePin    D4
// #else
// 	 #define BTEnablePin    D6
// #endif


  #define BTConnectedPin D5 // aka COMM_LED

	#define serialSpeed  9600  //115200 //38400
	#define AT_BAUD_BLE_CFG  "AT+BAUD0" //115200 = "AT+BAUD4"

/*=========================================================================*/

/*=========================================================================
	Temperature+Humidity (SHT11)
	-----------------------------------------------------------------------*/
	#define HumDataPin D2
	#define HumSckPin  D3
/*=========================================================================*/

/*=========================================================================
	Volatile Organic Compounds (VOC) sensor
	-----------------------------------------------------------------------*/
	#define VOCEnablePin A6
/*=========================================================================*/

/*=========================================================================
	SPI: Used by Barometric pressure (MS5534C) and MicroSD card
	-----------------------------------------------------------------------*/
	#define SPIDinPin A5 	// MOSI
	#define SPIDoutPin A4 	// MISO
	#define SPISckPin A3

	#define BarCSPin A0		// Bar chip select
	#define SDCSPin  A2 	// SD chip select

	//#define SDCDPin A1		// SD card detect
	#define PowerConnectedPin A1


	//BOARD Version <2.2
	//	#define UCCSPin  D4 	// unconnected SPI device
	//BOARD Version >=2.2
	//#define UCCSPin  D6 	// unconnected SPI device
// #if  BOARD_VERSION_MAJ >= 2 && BOARD_VERSION_MIN >= 2
// 	#define UCCSPin    D6
// #else
// 	#define UCCSPin    D4
// #endif


/*=========================================================================*/

/*=========================================================================
	I2C ADDRESS/BITS
	-----------------------------------------------------------------------*/
	#define ADS1115_ADDRESS_0                 (0x48)
	#define ADS1115_ADDRESS_1                 (0x49)
/*=========================================================================*/

	#define WAKEUP_Pin	A7

/*=========================================================================
	UNASSIGNED/DEBUG
	-----------------------------------------------------------------------*/
	#define DebugLED D7
	//Will need to free also D6 for supporting debug
/*=========================================================================*/

#endif
