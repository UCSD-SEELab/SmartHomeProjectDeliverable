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

#include "Message.h"

OutMessage::OutMessage() :
	hasTime(false), gas_Raw(NULL), gas_Model(NULL),
	voc_Raw(NULL), voc_Model(NULL), co2_Raw(NULL), co2_Model(NULL), _hum_bar(NULL)
	/*gas_RawDiff(NULL), gas_mV(NULL), _serial(NULL),*/ {
	//content_mask = MSG_CONT_RAW & MSG_CONT_HUPR;
}
OutMessage::OutMessage(unsigned long time) : OutMessage() {
	hasTime = true;
	msgTime = time;
}

char* OutMessage::toString() {
	uint8_t mask =
			MSG_CONT_CO2  |
			MSG_CONT_CONC |
			MSG_CONT_CONF |
			MSG_CONT_HUPR |
			MSG_CONT_RAW  |
			MSG_CONT_VOC;
			//MSG_CONT_TSTR |
	return toString(mask);
}
char* OutMessage::toString(uint8_t mask) {
	StaticJsonBuffer<MSG_JSON_BUF_MAX_LEN> jsonBuffer;
	JsonObject& root = jsonBuffer.createObject();
	root[msg_FieldName] = msgFieldVal_Reading;
	if (hasTime)
		root[msgField_time] = msgTime;
	if (hasTime && (mask & MSG_CONT_TSTR)) {
		String timeSTR =  Time.format(msgTime, TIME_FORMAT_ISO8601_FULL);
		root[msgFieldRead_realTimeString] = (const char*)timeSTR.c_str();
	}
	if (gas_Model != NULL && (mask & MSG_CONT_CONC)) {
		JsonObject& gas_ModelObject = root.createNestedObject(msgFieldRead_ppb);
		outGas_ModelTo(gas_ModelObject);
	}
	if (gas_Raw != NULL && (mask & MSG_CONT_RAW)) {
		JsonObject& gas_RawObject = root.createNestedObject(msgFieldRead_raw);
		outGas_RawTo(gas_RawObject);
	}
	if (_hum_bar != NULL && (mask & MSG_CONT_HUPR)) {
		JsonObject& _hum_barObject = root.createNestedObject(msgFieldRead_HumBar);
		outHum_BarTo(_hum_barObject);
	}
	if (co2_Model != NULL && (mask & MSG_CONT_CO2)) {
		JsonObject& co2_ModelObject = root.createNestedObject(msgFieldRead_CO2);
		outCO2_ModelTo(co2_ModelObject);
	}
	if (voc_Model != NULL && (mask & MSG_CONT_VOC)) {
		JsonObject& voc_ModelObject = root.createNestedObject(msgFieldRead_VOC);
		outVOC_ModelTo(voc_ModelObject);
	}
	if (co2_Raw != NULL && (mask & MSG_CONT_CO2)) {
		JsonObject& co2_rawObject = root.createNestedObject(msgFieldReadRaw_CO2);
		outCO2_RawTo(co2_rawObject);
	}
	if (voc_Raw != NULL && (mask & MSG_CONT_VOC)) {
		JsonObject& voc_rawObject = root.createNestedObject(msgFieldReadRaw_VOC);
		outVOC_RawTo(voc_rawObject);
	}
	return toString(root);
}
char* OutMessage::toString(JsonObject& root) {
	root.printTo(buffer,sizeof(buffer));
	return buffer;
}
void OutMessage::copyDataTo(binaryReadMessageEx_t& binReadMsg, uint8_t mask) {
	binReadMsg.msgType = 0x02;
	if (hasTime) {
		binReadMsg.hasTime = true;
		binReadMsg.messageTime = msgTime;
	} else binReadMsg.hasTime = false;
	if (gas_Raw != NULL && (mask & MSG_CONT_RAW)) {
		binReadMsg.hasRawGas = true;
		binReadMsg.rawGas.range = gas_Raw->range;
		binReadMsg.rawGas.ADC0_0 = gas_Raw->ADC0_0;
		binReadMsg.rawGas.ADC0_1 = gas_Raw->ADC0_1;
		binReadMsg.rawGas.ADC0_2 = gas_Raw->ADC0_2;
		binReadMsg.rawGas.ADC0_3 = gas_Raw->ADC0_3;
		binReadMsg.rawGas.ADC1_0 = gas_Raw->ADC1_0;
		binReadMsg.rawGas.ADC1_1 = gas_Raw->ADC1_1;
		binReadMsg.rawGas.ADC1_2 = gas_Raw->ADC1_2;
		binReadMsg.rawGas.ADC1_3 = gas_Raw->ADC1_3;
	} else binReadMsg.hasRawGas = false;
	if (_hum_bar != NULL && (mask & MSG_CONT_HUPR)) {
		binReadMsg.hasHumBar = true;
		binReadMsg.humBar.bar_T = _hum_bar->bar_T;
		binReadMsg.humBar.bar_P = _hum_bar->bar_P;
		binReadMsg.humBar.hum_T = _hum_bar->hum_T;
		binReadMsg.humBar.hum_H = _hum_bar->hum_H;
	} else binReadMsg.hasHumBar = false;
	if (co2_Model != NULL && (mask & MSG_CONT_CO2)) {
		binReadMsg.hasCO2 = true;
		binReadMsg.CO2_ppm = co2_Model->CO2_ppm;
	} else binReadMsg.hasCO2 = false;
	if (voc_Model != NULL && (mask & MSG_CONT_VOC)) {
		binReadMsg.hasVoc = true;
		binReadMsg.voc_val_IAQ = voc_Model->VOC_IAQ_ppb;
		binReadMsg.voc_val_PID = voc_Model->VOC_PID_ppb;
	} else binReadMsg.hasVoc = false;
}
void OutMessage::copyDataTo(binaryReadMessage_t& binReadMsg, uint8_t mask) {
	binReadMsg.msgType = 0x01;
	if (hasTime) {
		binReadMsg.hasTime = true;
		binReadMsg.messageTime = msgTime;
	} else binReadMsg.hasTime = false;
	if (gas_Raw != NULL && (mask & MSG_CONT_RAW)) {
		binReadMsg.hasRawGas = true;
		binReadMsg.rawGas.range = gas_Raw->range;
		binReadMsg.rawGas.ADC0_0 = gas_Raw->ADC0_0;
		binReadMsg.rawGas.ADC0_1 = gas_Raw->ADC0_1;
		binReadMsg.rawGas.ADC0_2 = gas_Raw->ADC0_2;
		binReadMsg.rawGas.ADC0_3 = gas_Raw->ADC0_3;
		binReadMsg.rawGas.ADC1_0 = gas_Raw->ADC1_0;
		binReadMsg.rawGas.ADC1_1 = gas_Raw->ADC1_1;
		binReadMsg.rawGas.ADC1_2 = gas_Raw->ADC1_2;
		binReadMsg.rawGas.ADC1_3 = gas_Raw->ADC1_3;
	} else binReadMsg.hasRawGas = false;
	if (_hum_bar != NULL && (mask & MSG_CONT_HUPR)) {
		binReadMsg.hasHumBar = true;
		binReadMsg.humBar.bar_T = _hum_bar->bar_T;;
		binReadMsg.humBar.bar_P = _hum_bar->bar_P;
		binReadMsg.humBar.hum_T = _hum_bar->hum_T;
		binReadMsg.humBar.hum_H = _hum_bar->hum_H;
	} else binReadMsg.hasHumBar = false;
}
void OutMessage::toB64Buffer(binaryReadMessage_t *bm) {
	copyDataTo(*bm, MSG_CONT_CONC | MSG_CONT_CONF |	MSG_CONT_HUPR | MSG_CONT_RAW);
}
void OutMessage::toB64Buffer(binaryReadMessageEx_t *bm) {
	copyDataTo(*bm, MSG_CONT_CONC | MSG_CONT_CONF |	MSG_CONT_HUPR | MSG_CONT_RAW
		 | MSG_CONT_CO2 | MSG_CONT_VOC);
}
char* OutMessage::toStringB64() {
	int inputLen;
	if (hasExtra())
	{
		binaryReadMessageEx_t bm;
		toB64Buffer(&bm);
		inputLen = sizeof(binaryReadMessageEx_t);
		int encodedLen = base64_enc_len(inputLen);
		MSG_TRACE("Encoding to B64 Ex: inputLen = %d, encodedLen = %d\r\n", inputLen, encodedLen);
		if (encodedLen>MSG_B64_MAX_LEN)
			return NULL;
		base64_encode(bufferB64,(char*)&bm, inputLen);
		return bufferB64;
	} else {
		binaryReadMessage_t bm;
		toB64Buffer(&bm);
		inputLen = sizeof(binaryReadMessage_t);
		int encodedLen = base64_enc_len(inputLen);
		MSG_TRACE("Encoding to B64: inputLen = %d, encodedLen = %d\r\n", inputLen, encodedLen);
		if (encodedLen>MSG_B64_MAX_LEN)
			return NULL;
		base64_encode(bufferB64,(char*)&bm, inputLen);
		return bufferB64;
	}
}

int OutMessage::binaryMsgLen() {
	if (hasExtra())
		return sizeof(binaryReadMessageEx_t);
	return sizeof(binaryReadMessage_t);
}

uint8_t* OutMessage::toBinaryMsg(uint8_t* buf) {
	if (hasExtra())
		toB64Buffer((binaryReadMessageEx_t*)buf);
	else
		toB64Buffer((binaryReadMessage_t*)buf);
	return buf;
}

bool OutMessage::hasExtra() {
	return SensorConfig.vocInstalled || SensorConfig.co2Installed;
	//return co2_Model != NULL || voc_Model != NULL;
}
char* OutMessage::MsgAck() {
	StaticJsonBuffer<MSG_JSON_BUF_MAX_LEN> jsonBuffer;
	JsonObject& root = jsonBuffer.createObject();
	root[msg_FieldName] = msgFieldVal_Ack;
	return toString(root);
}
char* OutMessage::MsgError() {
	StaticJsonBuffer<MSG_JSON_BUF_MAX_LEN> jsonBuffer;
	JsonObject& root = jsonBuffer.createObject();
	root[msg_FieldName] = msgFieldVal_Error;
	return toString(root);
}
const char* OutMessage::msgBoolFlag(const char* name, bool value)
{
	StaticJsonBuffer<MSG_JSON_BUF_MAX_LEN> jsonBuffer;
	JsonObject& root = jsonBuffer.createObject();
	root[name] = value;
	return toString(root);
}
const char* OutMessage::msgUCharFlag(const char* name, unsigned char value)
{
	StaticJsonBuffer<MSG_JSON_BUF_MAX_LEN> jsonBuffer;
	JsonObject& root = jsonBuffer.createObject();
	root[name] = value;
	return toString(root);
}
const char* OutMessage::msgIntFlag(const char* name, int value)
{
	StaticJsonBuffer<MSG_JSON_BUF_MAX_LEN> jsonBuffer;
	JsonObject& root = jsonBuffer.createObject();
	root[name] = value;
	return toString(root);
}
const char* OutMessage::msgLongFlag(const char* name, long value)
{
	StaticJsonBuffer<MSG_JSON_BUF_MAX_LEN> jsonBuffer;
	JsonObject& root = jsonBuffer.createObject();
	root[name] = value;
	return toString(root);
}
const char* OutMessage::msgStringFlag(const char* name, const char* value)
{
	StaticJsonBuffer<MSG_JSON_BUF_MAX_LEN> jsonBuffer;
	JsonObject& root = jsonBuffer.createObject();
	root[name] = value;
	return toString(root);
}


/*char* OutMessage::MsgInterval(long interval) {
	StaticJsonBuffer<MSG_JSON_BUF_MAX_LEN> jsonBuffer;
	JsonObject& root = jsonBuffer.createObject();
	root[msg_FieldName] = msgFieldVal_Reading;
	long val = interval;
	root[msgFieldCmdType_StreamingInterval] = val;
	return toString(root);
}
char* OutMessage::MsgPower(int level) {
	StaticJsonBuffer<MSG_JSON_BUF_MAX_LEN> jsonBuffer;
	JsonObject& root = jsonBuffer.createObject();
	root[msg_FieldName] = msgFieldVal_Reading;
	root[msgFieldCmdType_ReadPower] = level;
	return toString(root);
}

char* OutMessage::MsgRequestValueBool(char* str, bool value) {
	StaticJsonBuffer<MSG_JSON_BUF_MAX_LEN> jsonBuffer;
	JsonObject& root = jsonBuffer.createObject();
	root[msg_FieldName] = msgFieldVal_Reading;
	root[str] = value;
	return toString(root);
}

char* OutMessage::MsgStreamingWiFi(bool value) { return MsgRequestValueBool(msgFieldCmdType_Streaming_WiFi, value); }
char* OutMessage::MsgStreamingSD(bool value) { return MsgRequestValueBool(msgFieldCmdType_Streaming_SD, value); }
char* OutMessage::MsgStreamingBLE(bool value) { return MsgRequestValueBool(msgFieldCmdType_Streaming_BLE, value); }

char* OutMessage::MsgWifiStatus(bool on) { return MsgRequestValueBool(msgFieldCmdType_WifiStatus, on); }
char* OutMessage::MsgSleepStatus(bool on) { return MsgRequestValueBool(msgFieldCmdType_SleepStatus, on); }
char* OutMessage::MsgMirrorToUSBStatus(bool on) { return MsgRequestValueBool(msgFieldCmdType_MirrorToUSBStatus, on); }

char* OutMessage::MsgVOCStatus(bool on) { return MsgRequestValueBool(msgFieldCmdType_VOCStatus, on); }
char* OutMessage::MsgCO2Status(bool on) { return MsgRequestValueBool(msgFieldCmdType_CO2Status, on); }

void OutMessage::setAFESerial(const char* serial) {
		_serial = serial;
		//strncpy ( _serial, serial, 9);
}*/
void OutMessage::setAFEGas(AFE::Gas_Raw_t& gas) {
	gas_Raw = &gas;
}
void OutMessage::setGasModel(AFE::Gas_Model_t& model) {
	gas_Model = &model;
}
void OutMessage::setVOCRaw(VOC::VOC_Raw_t& voc) {
	voc_Raw = &voc;
}
void OutMessage::setVOCModel(VOC::VOC_Model_t& model) {
	voc_Model = &model;
}
void OutMessage::setCO2Raw(CO2::CO2_Raw_t& co2) {
	co2_Raw = &co2;
}
void OutMessage::setCO2Model(CO2::CO2_Model_t& model) {
  co2_Model = &model;
}
void OutMessage::setSensorsReadings(Sensor::Reading_Raw_t& hum_bar) {
	_hum_bar = &hum_bar;
}
/*void OutMessage::outConfigTo(JsonObject& nestedObject) {
	nestedObject[cfgField_Serial] = (const char*) _serial;
	String ID =  System.deviceID();
	nestedObject[cfgField_NodeName] = (const char*)ID.c_str();
}*/

int OutMessage::encodeScale(adsGain_t currentGain) {
	if (currentGain == GAIN_TWOTHIRDS)
		return 0;
	else if (currentGain == GAIN_ONE)
		return 1;
	else if (currentGain == GAIN_TWO)
		return 2;
	else if (currentGain == GAIN_FOUR)
		return 4;
	else if (currentGain == GAIN_EIGHT)
		return 8;
	else if (currentGain == GAIN_SIXTEEN)
		return 16;
	return 0;
}

void OutMessage::outGas_RawTo(JsonObject& nestedObject) {
	nestedObject[gas_mV_Field_Range] = encodeScale(gas_Raw->range);
	nestedObject[gas_mV_Field_SN1_A] = gas_Raw->ADC0_0;
	nestedObject[gas_mV_Field_SN1_W] = gas_Raw->ADC0_1;
	nestedObject[gas_mV_Field_SN2_A] = gas_Raw->ADC1_2;
	nestedObject[gas_mV_Field_SN2_W] = gas_Raw->ADC1_3;
	nestedObject[gas_mV_Field_SN3_A] = gas_Raw->ADC1_0;
	nestedObject[gas_mV_Field_SN3_W] = gas_Raw->ADC1_1;
	nestedObject[gas_mV_Field_PT] = gas_Raw->ADC0_2;
	nestedObject[gas_mV_Field_NC] = gas_Raw->ADC0_3;
}

void OutMessage::outGas_ModelTo(JsonObject& nestedObject) {
	nestedObject[gas_ppb_Field_SN1] = gas_Model->SN1_ppb;
	nestedObject[gas_ppb_Field_SN2] = gas_Model->SN2_ppm;
	nestedObject[gas_ppb_Field_SN3] = gas_Model->SN3_ppm;
	nestedObject[gas_ppb_Field_AQI] = gas_Model->AQI;
}

void OutMessage::outVOC_ModelTo(JsonObject& nestedObject) {
	nestedObject[voc_pid_ppb_Field] = voc_Model->VOC_PID_ppb;
	nestedObject[voc_iaq_ppb_Field] = voc_Model->VOC_IAQ_ppb;
}

void OutMessage::outCO2_ModelTo(JsonObject& nestedObject) {
	nestedObject[co2_ppm_Field] = co2_Model->CO2_ppm;
}

void OutMessage::outVOC_RawTo(JsonObject& nestedObject) {
	nestedObject[voc_range_raw_Field] = voc_Raw->range;
	nestedObject[voc_pid_raw_Field] = voc_Raw->val_PID;
	nestedObject[voc_iaq_raw_Field] = voc_Raw->val_IAQ;
}

void OutMessage::outCO2_RawTo(JsonObject& nestedObject) {
	nestedObject[co2_val_Field] = co2_Raw->val;
	nestedObject[co2_status_Field] = co2_Raw->status;
}

void OutMessage::outHum_BarTo(JsonObject& nestedObject) {
	nestedObject[bar_hum_Field_bar_T] = _hum_bar->bar_T  / 10.0;
	nestedObject[bar_hum_Field_bar_P] = _hum_bar->bar_P / 10.0;
	nestedObject[bar_hum_Field_hum_T] = _hum_bar->hum_T / 100.0;
	nestedObject[bar_hum_Field_hum_H] = _hum_bar->hum_H / 100.0;
}


//--------------------------------------------------

InCmdMessage::InCmdMessage(char* str) : root(jsonBuffer.parseObject(str))
{}

bool InCmdMessage::isError() { return !root.success(); }

bool InCmdMessage::hasFlag(const char* name)
{
	return root.containsKey(name);
}
bool InCmdMessage::requestFlag(const char* name)
{
	if (!(root.containsKey(msgFieldVal_Request) ||
				root.containsKey(msgFieldVal_Requests))) return false;
	if (root[msgFieldVal_Requests].is<JsonArray&>())
	{
		JsonArray& nestedArray = root[msgFieldVal_Requests].as<JsonArray>();
		for (auto value : nestedArray) {
			const char* vs = value.as<const char*>();
			if (String(vs).equalsIgnoreCase(name)) {
				MSG_TRACE("Array requestFlag found %s checking str %s",vs, name);
	    	return true;
			}
		}
		return false;
	}
	const char* str = root[msgFieldVal_Request].as<char*>();
	MSG_TRACE("requestFlag str: %s",str);
	if (str==NULL)
		return false;
	return String(str).equalsIgnoreCase(name);
}
bool InCmdMessage::getBoolFlag(const char* name)
{
	bool val = root[name].as<bool>();
	MSG_TRACE("getBoolFlag %s: %d\r\n", val);
	return val;
}
unsigned char InCmdMessage::getUCharFlag(const char* name){
	unsigned char val = root[name].as<unsigned char>();
	MSG_TRACE("getMessageEncodingTypeFlag %s: %d\r\n", name, val);
	return val;
}
int InCmdMessage::getIntFlag(const char* name)
{
	int val = root[name].as<int>();
	MSG_TRACE("getIntFlag %s: %d\r\n", name, val);
	return val;
}
long InCmdMessage::getLongFlag(const char* name)
{
	long val = root[name].as<long>();
	MSG_TRACE("getLongFlag %s: %d\r\n", name, val);
	return val;
}
const char* InCmdMessage::getStringFlag(const char* name)
{
	const char* val = root[name].as<char*>();
	MSG_TRACE("getStringFlag %s: %s\r\n", name, val);
	return val;
}

boolean InCmdMessage::hasTime() { return root.containsKey(msgField_time); }
unsigned long InCmdMessage::getTime() {
	return root[msgField_time].as<unsigned long>();
}

String InCmdMessage::getReqType(){
	const char* str = root[msgFieldVal_Request].as<char*>();
	MSG_TRACE("getReqType str: %s",str);
	if (str==NULL)
		return "";
	return str;
}

/*bool InCmdMessage::hasStreamingSD() { return root.containsKey(msgFieldCmdType_Streaming_SD); }
bool InCmdMessage::getStreamingSD() { return root[msgFieldCmdType_Streaming_SD].as<bool>(); }
bool InCmdMessage::hasStreamingWiFi() { return root.containsKey(msgFieldCmdType_Streaming_WiFi); }
bool InCmdMessage::getStreamingWiFi() { return root[msgFieldCmdType_Streaming_WiFi].as<bool>(); }
bool InCmdMessage::hasStreamingBLE() { return root.containsKey(msgFieldCmdType_Streaming_BLE); }
bool InCmdMessage::getStreamingBLE() { return root[msgFieldCmdType_Streaming_BLE].as<bool>(); }

bool InCmdMessage::requestStreamingSD() { return getReqType().equalsIgnoreCase(msgFieldCmdType_Streaming_SD); }
bool InCmdMessage::requestStreamingWiFi() { return getReqType().equalsIgnoreCase(msgFieldCmdType_Streaming_WiFi); }
bool InCmdMessage::requestStreamingBLE() { return getReqType().equalsIgnoreCase(msgFieldCmdType_Streaming_BLE); }

bool InCmdMessage::requestWifiStatus() { return getReqType().equalsIgnoreCase(msgFieldCmdType_WifiStatus); }
bool InCmdMessage::requestSleepStatus() { return getReqType().equalsIgnoreCase(msgFieldCmdType_SleepStatus); }
bool InCmdMessage::requestMirrorToUSBStatus() { return getReqType().equalsIgnoreCase(msgFieldCmdType_MirrorToUSBStatus); }
bool InCmdMessage::requestVOCStatus() { return getReqType().equalsIgnoreCase(msgFieldCmdType_VOCStatus); }
bool InCmdMessage::requestCO2Status() { return getReqType().equalsIgnoreCase(msgFieldCmdType_CO2Status); }



boolean InCmdMessage::hasInterval() {  return root.containsKey(msgFieldCmdType_StreamingInterval); }
long InCmdMessage::getInterval() {
	return root[msgFieldCmdType_StreamingInterval].as<long>();
}

bool InCmdMessage::hasSSID() { return root.containsKey(msgFieldCmdType_SSID);  }
const char* InCmdMessage::getSSID() {
	return root[msgFieldCmdType_SSID].asString();
}
bool InCmdMessage::hasPASS() { return root.containsKey(msgFieldCmdType_PASS);  }
const char* InCmdMessage::getPASS() {
	return root[msgFieldCmdType_PASS].asString();
}

bool InCmdMessage::hasVOCStatus() { return root.containsKey(msgFieldCmdType_EnableVOC); }
bool InCmdMessage::getVOCStatus() { return root[msgFieldCmdType_EnableVOC].as<bool>(); }
bool InCmdMessage::hasCO2Status() { return root.containsKey(msgFieldCmdType_EnableCO2); }
bool InCmdMessage::getCO2Status() { return root[msgFieldCmdType_EnableCO2].as<bool>(); }

bool InCmdMessage::hasLogToUSB() { return root.containsKey(msgFieldCmdType_LogToUSB); }
long InCmdMessage::getLogToUSB() { return root[msgFieldCmdType_LogToUSB].as<long>(); }
bool InCmdMessage::isDisableLoggingToUSB() { return root[msgFieldCmdType_LogToUSB].as<long>() == 0; }
bool InCmdMessage::isEnableLoggingToUSB() { return root[msgFieldCmdType_LogToUSB].as<long>() == 1; }
bool InCmdMessage::isForwardSerialToUSB() { return root[msgFieldCmdType_LogToUSB].as<long>() == 2; }

bool InCmdMessage::hasTemporaryEnableWiFi() { return root.containsKey(msgFieldCmdType_EnableWiFi); }
long InCmdMessage::IntervalEnableWiFi() { return root[msgFieldCmdType_EnableWiFi].as<long>(); }

bool InCmdMessage::hasSleepStatus() { return root.containsKey(msgFieldCmdType_SleepStatus); }
bool InCmdMessage::getSleepStatus() { return root[msgFieldCmdType_SleepStatus].as<bool>(); }

//boolean InCmdMessage::isAck() { return _isAck; }

bool InCmdMessage::clearWiFiCredentials() { return root[msgFieldCmdType_ClearWiFiCredentials]; }
bool InCmdMessage::requestConfig() { return getReqType().equalsIgnoreCase(msgFieldCmdType_Config); }
bool InCmdMessage::requestReset() {  return getReqType().equalsIgnoreCase(msgFieldCmdType_Reset); }
bool InCmdMessage::requestInterval() {  return getReqType().equalsIgnoreCase(msgFieldCmdType_StreamingInterval); }
bool InCmdMessage::requestPower() {  return getReqType().equalsIgnoreCase(msgFieldCmdType_ReadPower);  }
bool InCmdMessage::storeConfigInEEPROM()  {  return getReqType().equalsIgnoreCase(msgFieldCmdType_StoreEEPROM);  }

bool InCmdMessage::hasUpdateConfig() { return root.containsKey(msgFieldRead_Config); }
bool InCmdMessage::hasAFESerial() { return root[msgFieldRead_Config].asObject().containsKey(cfgField_Serial); }
const char* InCmdMessage::getAFESerial() { return root[msgFieldRead_Config][cfgField_Serial].asString(); }
bool InCmdMessage::hasSensorName() { return root[msgFieldRead_Config].asObject().containsKey(cfgField_NodeName); }
const char* InCmdMessage::getSensorName() { return root[msgFieldRead_Config][cfgField_NodeName].asString(); }*/
