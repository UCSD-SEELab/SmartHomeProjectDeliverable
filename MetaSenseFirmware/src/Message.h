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

// Message.h

#ifndef _MESSAGE_h
#define _MESSAGE_h

#include "application.h"

// #if defined(ARDUINO) && ARDUINO >= 100
// #include "arduino.h"
// #else
// #include "WProgram.h"
// #endif

//#include <Time.h>
#include "ArduinoJson.h"
#include "PhotonConfig.h"
//TODO Need to remove these files and use the more advanced ArduinoJson
//#include "JsonSerializer.h"
//#include "JsonDeserializer.h"
#include "AFE.h"
#include "VOC.h"
#include "CO2.h"
#include "Sensor.h"
#include "Base64.h"

#define msg_FieldName "msg"

#define msgFieldVal_Command "cmd"
#define msgFieldVal_Reading "read"
#define msgFieldVal_Ack "ack"
#define msgFieldVal_Error "err"
#define msgFieldVal_Request "req"
#define msgFieldVal_Requests "reqs"

//Boolean Flags
#define msg_Bool_Flag_Streaming_SD "s_sd"
#define msg_Bool_Flag_Streaming_WiFi "s_wifi"
#define msg_Bool_Flag_Streaming_MQTT "s_mqtt"
#define msg_Bool_Flag_Batch_WiFi "b_wifi"
#define msg_Bool_Flag_Streaming_BLE "s_ble"
#define msg_Bool_Flag_WifiStatus "wifi_en"
#define msg_Bool_Flag_SleepStatus "sleep_en"
#define msg_Bool_Flag_MirrorToUSBStatus "usb_en"
#define msg_Bool_Flag_USBPassthroughStatus "usb_pass"
#define msg_Bool_Flag_EnableCO2 "co2_en"
#define msg_Bool_Flag_EnableVOC "voc_en"

//Long Flags
#define msg_Long_Flag_ReadPower "power"
#define msg_Long_Flag_StreamingInterval "s_inter"

//Int (and enum) flags
#define msg_Int_Flag_SD_Reads_Format "f_sd"
#define msg_Int_Flag_WiFi_Reads_Format "f_wifi"
#define msg_Int_Flag_MQTT_Reads_Format "f_mqtt"
#define msg_Int_Flag_BLE_Reads_Format "f_ble"
#define msg_Int_Flag_Setup "setup"

//String Flags
#define msg_String_Flag_SSID "ssid"
#define msg_String_Flag_PASS "pass"
#define msg_String_Flag_NodeID "node_id"
#define msg_String_Flag_AFESerial "afe_ser"
#define msg_String_Flag_MAC "mac_addr"

//Execute Command
#define msg_Exec_Cmd_Reset "reset"
#define msg_Exec_Cmd_ClearWiFiCredentials "cl_wifi"
#define msg_Exec_Cmd_StoreEEPROM "st_rom"

//Other Fields
#define msgField_time "ts"


#define msgFieldRead_realTime "rt"
#define msgFieldRead_realTimeString "rts"
#define msgFieldRead_mV "mV"
#define msgFieldRead_ppb "ppb"
#define msgFieldRead_raw "raw"
#define msgFieldRead_raw_diff "rawD"
#define msgFieldRead_HumBar "hu_pr"
#define msgFieldRead_VOC "voc"
#define msgFieldRead_CO2 "co2"
#define msgFieldReadRaw_VOC "vocR"
#define msgFieldReadRaw_CO2 "co2R"

// #define msgField_Config "conf"
// #define cfgField_PT20C_V "PTV"
// #define cfgField_Calibration "cal"
// #define cfgCalibration_SensorType "Typ"
// #define cfgCalibration_SensorType_NO2 "NO2"
// #define cfgCalibration_SensorType_OX "OX"
// #define cfgCalibration_SensorType_CO "CO"
// #define cfgCalibration_SensorType_ERR "?"
// #define cfgCalibration_WE_E0 "WE0"
// #define cfgCalibration_WE_S0 "WS0"
// #define cfgCalibration_AE_E0 "AE0"
// #define cfgCalibration_AE_S0 "AS0"
// #define cfgCalibration_Sensitivity "Sns"
// #define cfgCalibration_NO2_Sensitivity "NO2Sns"

#define gas_mV_Field_Range "Rng"
#define gas_mV_Field_SN1_W "S1W"
#define gas_mV_Field_SN1_A "S1A"
#define gas_mV_Field_SN2_W "S2W"
#define gas_mV_Field_SN2_A "S2A"
#define gas_mV_Field_SN3_W "S3W"
#define gas_mV_Field_SN3_A "S3A"
#define gas_mV_Field_PT "PT"
#define gas_mV_Field_NC "NC"

#define gas_ppb_Field_SN1 "NO2"
#define gas_ppb_Field_SN2 "OX"
#define gas_ppb_Field_SN3 "CO"
#define gas_ppb_Field_AQI "AQI"

#define voc_range_raw_Field "Rng"
#define voc_pid_raw_Field "vPA"
#define voc_pid_ppb_Field "vPP"
#define voc_iaq_raw_Field "vIA"
#define voc_iaq_ppb_Field "vIP"

#define co2_ppm_Field "CO2"
#define co2_val_Field "val"
#define co2_status_Field "status"

#define bar_hum_Field_bar_T "bT"
#define bar_hum_Field_bar_P "bP"
#define bar_hum_Field_hum_T "hT"
#define bar_hum_Field_hum_H "hH"

#define MSG_CONT_CO2		0b00000001
#define MSG_CONT_CONC		0b00000010
#define MSG_CONT_CONF		0b00000100
#define MSG_CONT_HUPR 	0b00001000
#define MSG_CONT_RAW 		0b00010000
#define MSG_CONT_TSTR		0b00100000
#define MSG_CONT_VOC 		0b01000000

#define MSG_JSON_MAX_LEN 400
#define MSG_B64_MAX_LEN 80
#define MSG_JSON_BUF_MAX_LEN 1000
#define MAX_STR_FLAG_LEN 30

typedef struct {
	uint32_t messageTime;
	uint8_t msgType : 2;
	bool hasTime : 1;
	bool hasRawGas : 1;
	bool hasHumBar : 1;
	bool hasVoc : 1;
	bool hasCO2 : 1;
	Sensor::Reading_Raw_t humBar;
	AFE::Gas_Raw_t rawGas;
} binaryReadMessage_t;
typedef struct {
	uint32_t messageTime;
	uint8_t msgType : 2;
	bool hasTime : 1;
	bool hasRawGas : 1;
	bool hasHumBar : 1;
	bool hasVoc : 1;
	bool hasCO2 : 1;
	Sensor::Reading_Raw_t humBar;
	AFE::Gas_Raw_t rawGas;
	uint16_t CO2_ppm;
	VOC::VOC_ppb_t voc_val_IAQ;
	VOC::VOC_ppb_t voc_val_PID;
} binaryReadMessageEx_t;
class OutMessage
{
	public:
		OutMessage();
		OutMessage(unsigned long time);
		// Call to get a JSON serialization
		bool hasExtra();
		//void SerializeTo(Stream& communicationStream);
		char* toString();
		char* toString(uint8_t mask);
		char* toStringB64();
		void  toB64Buffer(binaryReadMessage_t *bm);
		void  toB64Buffer(binaryReadMessageEx_t *bm);

		int binaryMsgLen();
		uint8_t* toBinaryMsg(uint8_t* buf);

		char* MsgAck();
		char* MsgError();

		const char* msgBoolFlag(const char* name, bool value);
		const char* msgUCharFlag(const char* name, unsigned char value);
		const char* msgIntFlag(const char* name, int value);
		const char* msgLongFlag(const char* name, long value);
		const char* msgStringFlag(const char* name, const char* value);

		// char* MsgInterval(long interval);
		// char* MsgPower(int level);
		// char* MsgStreamingWiFi(bool value);
		// char* MsgStreamingSD(bool value);
		// char* MsgStreamingBLE(bool value);
		// char* MsgWifiStatus(bool on);
		// char* MsgSleepStatus(bool on);
		// char* MsgMirrorToUSBStatus(bool on);
		// char* MsgVOCStatus(bool on);
		// char* MsgCO2Status(bool on);


		//void setAFEConfig(AFE::AFEConfig_t& config);
		//void setAFESerial(const char* serial);
		void setAFEGas(AFE::Gas_Raw_t& gas);
		void setGasModel(AFE::Gas_Model_t& model);
		void setVOCRaw(VOC::VOC_Raw_t& voc);
		void setVOCModel(VOC::VOC_Model_t& model);
		void setCO2Raw(CO2::CO2_Raw_t& co2);
		void setCO2Model(CO2::CO2_Model_t& model);
		void setSensorsReadings(Sensor::Reading_Raw_t& hum_bar);
		//void setMSGContent(uint8_t mask);

	private:
		//char* MsgRequestValueBool(char* str, bool value);
		void copyDataTo(binaryReadMessage_t& binReadMsg, uint8_t mask);
		void copyDataTo(binaryReadMessageEx_t& binReadMsg, uint8_t mask);
		char* toString(JsonObject& root);
		void outTimeTo(JsonObject& root);
		// void outConfigTo(JsonObject& root);
		void outGas_mVTo(JsonObject& root);
		void outGas_RawTo(JsonObject& root);
		void outGas_ModelTo(JsonObject& root);
		void outVOC_ModelTo(JsonObject& root);
	  void outCO2_ModelTo(JsonObject& root);
		void outVOC_RawTo(JsonObject& root);
	  void outCO2_RawTo(JsonObject& root);
		void outGas_RawDiffTo(JsonObject& root);
		void outHum_BarTo(JsonObject& root);

		int encodeScale(adsGain_t currentGain);

		char buffer[MSG_JSON_MAX_LEN];
		char bufferB64[MSG_B64_MAX_LEN];
		//uint8_t content_mask;

		//unsigned long msgRealTime;
		//String msgRealTimeString;

		boolean hasTime;
		unsigned long msgTime;
		//AFE::AFEConfig_t* _config;
		//const char* _serial;
		AFE::Gas_Raw_t* gas_Raw;
		AFE::Gas_Model_t* gas_Model;
		VOC::VOC_Raw_t* voc_Raw;
		VOC::VOC_Model_t* voc_Model;
	  CO2::CO2_Raw_t* co2_Raw;
		CO2::CO2_Model_t* co2_Model;
		Sensor::Reading_Raw_t* _hum_bar;
};

class InCmdMessage
{
	public:
		bool hasFlag(const char* name);
		bool requestFlag(const char* name);
		bool getBoolFlag(const char* name);
		unsigned char getUCharFlag(const char* name);
		int getIntFlag(const char* name);
		long getLongFlag(const char* name);
		const char* getStringFlag(const char* name);



		//InCmdMessage(Stream& jsonStream, long timeout);
		InCmdMessage(char* str); //str will get modified
		//~InCmdMessage();

		bool isError();

		// bool hasStreamingSD();
		// bool getStreamingSD();
		// bool hasStreamingWiFi();
		// bool getStreamingWiFi();
		// bool hasStreamingBLE();
		// bool getStreamingBLE();

		bool hasTime();
		unsigned long getTime();

		// bool hasInterval();
		// long getInterval();
		//
		// bool hasSSID();
		// const char* getSSID();
		// bool hasPASS();
		// const char* getPASS();
		//
		// //boolean isAck();
		// bool hasLogToUSB();
		// long getLogToUSB();
		// bool isDisableLoggingToUSB();
		// bool isEnableLoggingToUSB();
		// bool isForwardSerialToUSB();
		//
		// bool hasTemporaryEnableWiFi();
		// long IntervalEnableWiFi();
		//
		// bool storeConfigInEEPROM();
		// bool clearWiFiCredentials();
		// bool requestStreamingSD();
		// bool requestStreamingWiFi();
		// bool requestStreamingBLE();
		// bool requestInterval();
		// bool requestReset();
		// bool requestConfig();
		// bool requestPower();
		//
		// bool requestWifiStatus();
		// bool requestSleepStatus();
		// bool requestMirrorToUSBStatus();
		//
		// bool requestVOCStatus();
		// bool requestCO2Status();
		//
		// bool hasVOCStatus();
		// bool getVOCStatus();
		// bool hasCO2Status();
		// bool getCO2Status();
		//
		// bool hasSleepStatus();
		// bool getSleepStatus();
		//
		// bool hasUpdateConfig();
		// bool hasAFESerial();
		// const char* getAFESerial();
		// bool hasSensorName();
		// const char* getSensorName();

	private:
		StaticJsonBuffer<MSG_JSON_BUF_MAX_LEN> jsonBuffer;
		JsonObject& root;
		String getReqType();
};

#endif
