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

#include "ServiceConnector.h"
#include "SdFat.h"
//#include <Time.h>
#include "heatshrink_encoder.h"
static heatshrink_encoder hse;
SdFat SD;
//Params Needed for direct https uploading of data
TCPClient client;
const bool g_https_trace = false;
bool g_https_complete;
uint32_t g_bytes_received;
const int g_port = 443;

//https://zdwx46g3uj.execute-api.us-west-2.amazonaws.com/Prod
const char host [] = "q229xyhbv1.execute-api.us-west-2.amazonaws.com";
// unsigned char httpRequestContent[] = "POST %s HTTP/1.0\r\n"
// 	"User-Agent: MatrixSSL/" MATRIXSSL_VERSION "\r\n"
// 	"x-api-key: g114c8FUxn2LLcJy0LuuXM4VHemAVVj7o144zIZ5\r\n"
// 	"Host: q229xyhbv1.execute-api.us-west-2.amazonaws.com\r\n"
// 	"Accept: */*\r\n"
// 	"Content-Type: application/octet-stream\r\n"
// 	"Content-Length: %d\r\n";

String msg_endpoint = "/Prod/binary-msg-upload/" + System.deviceID();
String batch_endpoint = "/Prod/binary-batch-upload/" + System.deviceID();

ServiceConnector::ServiceConnector(Sensor& _sensor ,
	VOC& _voc, CO2& _co2, MQTT& mqtt, NeuralNetwork& nn) :
	sensor(_sensor), voc(_voc), co2(_co2), mqttClient(mqtt),
	_nn(nn)
{
//
}

void ServiceConnector::begin()
{

	// HTTPSClientEnabled = httpsClientSetup(host, msg_endpoint.c_str()) == 0;

	serialBypass = false;

	if (init)
	{
		//do_sendConfig(sensor.getConfig(), false, true, false, false);
		StaticJsonBuffer<MSG_JSON_BUF_MAX_LEN> jsonBuffer;
		JsonObject& root = jsonBuffer.createObject();
		root[msg_String_Flag_AFESerial] = SensorConfig.MACAddress;
		root[msg_String_Flag_MAC] = SensorConfig.MACAddress;
		char buffer[MSG_JSON_MAX_LEN];
		root.printTo(buffer,sizeof(buffer));
		logMessageToSD(buffer);
	}
  M_MQTT_TRACE("streamMQTT value %d\n\r", SensorConfig.streamMQTT)
  if (SensorConfig.streamMQTT) {
		M_MQTT_TRACE("Connecting to server")
		mqttClient.connect("Particle_" + System.deviceID(), MQTT_Server_Username, MQTT_Server_Password);
	}

	#if (PLATFORM_ID == 6)
	// Photon code here
	if (sensor.isWiFiEnabled())
		WiFi.connect(WIFI_CONNECT_SKIP_LISTEN);
	#elif (PLATFORM_ID == 10)
	//Electron or other Particle device code here
	if (sensor.isWiFiEnabled())
		Cellular.connect();
	#endif
}

void visualConfirm(uint8_t red, uint8_t green, uint8_t blue, int times)
{
	RGB.control(true);
	RGB.color(red, green, blue);

	for (int i=0; i<times; i++) {
		RGB.brightness(255);
		delay(500);
		RGB.brightness(64);
		delay(500);
	}

	RGB.control(false);
}
void resetEEPROM(Sensor& sensor)
{
	SensorEEPROMConfig_t* cfg = sensor.getConfig();
	cfg->wifiEnabled  = false;
	cfg->sleepEnabled = false;
	cfg->vocInstalled = false;
	cfg->co2Installed = false;
	cfg->messageEncodingSD = json;
	cfg->messageEncodingBLE  = json;
	cfg->messageEncodingCloud = binB64;
	cfg->messageEncodingMQTT = MQTT_Streaming_Default_Format;
	cfg->streamBLE  = true;
	cfg->streamWiFi = true;
	cfg->streamMQTT = false;
	cfg->batchWiFi = true;
	cfg->streamSD = true;
	cfg->intervalTime = 5;
	strcpy(cfg->AFESerial, "UNDEFINED");
	strcpy(cfg->MACAddress, "UNKNOWN");
	cfg->BLEBindType=3;
	init = true;
}
bool testATCommand(int baud, int rep)
{
	Serial1.begin(baud);
	delay (100);
	for(int j=0; j<rep; j++)
		Serial1.print("ABCDEFGHIL");
	Serial1.flush();
	delay (100);
	while(Serial1.available())
		Serial1.read();
	Serial1.print("AT");
	Serial1.flush();
	delay(300);
	int v0 = Serial1.read();
	int v1 = Serial1.read();
	SC_TRACE("Read v0=%d v1=%d\r\n", v0, v1);
	if (v0!='O' || v1!='K')
		return false;
	return true;
}
void BLEPinReset()
{
		pinMode(BLE_KEY_PIN, OUTPUT);
		digitalWrite(BLE_KEY_PIN, LOW);
		delay(1050);
		digitalWrite(BLE_KEY_PIN, HIGH);
		delay(1000);
		digitalWrite(BLE_KEY_PIN, LOW);
		delay(50);
		digitalWrite(BLE_KEY_PIN, HIGH);
		delay(500);
		digitalWrite(BLE_KEY_PIN, LOW);
		delay(1050);
		digitalWrite(BLE_KEY_PIN, HIGH);
		delay(1000);
		digitalWrite(BLE_KEY_PIN, LOW);
		delay(50);
		digitalWrite(BLE_KEY_PIN, HIGH);
		delay(500);
		//pinMode(BLE_KEY_PIN, INPUT);
}
void configBLEBondType(int bond_type)
{
	digitalWrite(BLE_KEY_PIN, LOW);
	delay(50);
	digitalWrite(BLE_KEY_PIN, HIGH);
	delay(500);
	Serial1.print("AT+TYPE");
	Serial1.print(bond_type);
	Serial1.flush();
	delay(500);
}
void configBLE(int bond_type)
{
	//Reset BLE to factory (do it twice to make sure it works even if the BLE was
	// connected or sleeping)
	SC_TRACE("D4 Value %d, D6 Value %d\r\n",D4, D6);
	SC_TRACE("BLE_KEY_PIN Value %d\r\n", BLE_KEY_PIN);

	BLEPinReset();
	SC_TRACE("Ready to check 9600 BAUD\r\n");
	if (!testATCommand(9600,10)) {
		BLEPinReset();
		SC_TRACE("Ready to check 115200 BAUD\r\n");
		int ret = testATCommand(115200,10);
		SC_TRACE("Check 115200 resut: %d\r\n", ret);
	}

	Serial1.print("AT+MODE2");
	Serial1.flush();
	delay(500);
	Serial1.print("AT+TYPE");
	Serial1.print(bond_type);
	Serial1.flush();
	delay(500);
	Serial1.print("AT+PIO11");
	Serial1.flush();
	delay(500);

	//Get MAC Address and Store it in local Settings
	while(Serial1.available())
		Serial1.read();
	Serial1.print("AT+ADDR?");
	Serial1.flush();
	delay(500);
	//bool macStarted = false;
	while(Serial1.available())
	{
		char c = Serial1.read();
		if (c==':')
			break;
	}
	int pos = 0;
	while(Serial1.available() && pos<19)
	{
		char c = Serial1.read();
		SensorConfig.MACAddress[pos++] = c;
	}
	SensorConfig.MACAddress[pos] = 0;
	//------------------------------

	char renameCommand[20];
	strcpy(renameCommand, "AT+NAMEMSense-");
	renameCommand[14]=SensorConfig.MACAddress[8];
	renameCommand[15]=SensorConfig.MACAddress[9];
	renameCommand[16]=SensorConfig.MACAddress[10];
	renameCommand[17]=SensorConfig.MACAddress[11];
	renameCommand[18]=0;

	Serial1.print(renameCommand);
	Serial1.flush();
	delay(500);
		Serial1.print(AT_BAUD_BLE_CFG);
	Serial1.flush();
	delay(500);
	Serial1.print("AT+RESET");
	Serial1.flush();
	delay(500);
	SC_TRACE("AT_RESET Sent\r\n");
	Serial1.begin(serialSpeed);
}
void ServiceConnector::execute_setup_command(int command)
{
	SINGLE_THREADED_BLOCK() {
		scheduledSetupCommand = 0;
		//stream.printlnf("button was clicked %d times", times);
		SensorEEPROMConfig_t* cfg = sensor.getConfig();
		if (command==1) {
			cfg->sleepEnabled = !cfg->sleepEnabled;
			visualConfirm(255, 0, 0, 3);
		} else if (command==2) {
			cfg->wifiEnabled = !cfg->wifiEnabled;
			visualConfirm(0, 255, 0, 3);
		} else if (command==3) {
			resetEEPROM(sensor);
			visualConfirm(0, 0, 255, 3);
		} else if (command==4) {
			RGB.control(true);
			RGB.color(255, 0, 255);
			cfg->BLEBindType = 0;
			configBLE(0);
			visualConfirm(255, 0, 255, 3);
		} else {
			cfg->BLEBindType++;
			if (cfg->BLEBindType<0 || cfg->BLEBindType>3)
				cfg->BLEBindType=0;
			configBLEBondType(cfg->BLEBindType);
			visualConfirm(150,  160, 230, cfg->BLEBindType);
		}
		sensor.storeEEPROMConfig();
		init = true;
		System.reset();
	}
}
void ServiceConnector::setup_button_clicked(int times)
{
	scheduledSetupCommand = times;
}

void ServiceConnector::outMessage(const char* msg, Msg_Source_t dest)
{
	if (dest==wifi)
		outMsgToCloud(msg);
	else if (dest==ble)
		Serial1.println(msg);
	else if (dest==usb && sensor.isUSBSerialConnected())
		Serial.println(msg);
}
void ServiceConnector::ackMessage(const char* str, Msg_Source_t source)
{
	SC_TRACE("Send Ack for %s\r\n", str);
	OutMessage o_msg;
	const char* o_msg_str = o_msg.msgStringFlag(msgFieldVal_Ack, str);
	SC_TRACE("Response: %s\r\n", o_msg_str);
	outMessage(o_msg_str, source);
}
void ServiceConnector::processReadFlagBool(InCmdMessage* msg, const char* str, bool value, Msg_Source_t source)
{
	if(msg->requestFlag(str)) {
		SC_TRACE("Received request for %s\r\n", str);
		OutMessage o_msg;
		const char* o_msg_str = o_msg.msgBoolFlag(str, value);
		SC_TRACE("Response: %s\r\n", o_msg_str);
		outMessage(o_msg_str, source);
	}
}
void ServiceConnector::processFlagBool(InCmdMessage* msg, const char* str, bool* value, Msg_Source_t source)
{
	if (msg->hasFlag(str)) {
		*value = msg->getBoolFlag(str);
		SC_TRACE("Received %s value %d\r\n", str, *value);
		ackMessage(str, source);
	}
}
void ServiceConnector::processReadFlagInt(InCmdMessage* msg, const char* str, int value, Msg_Source_t source)
{
	if(msg->requestFlag(str)) {
		SC_TRACE("Received request for %s\r\n", str);
		OutMessage o_msg;
		const char* o_msg_str = o_msg.msgIntFlag(str, value);
		SC_TRACE("Response: %s\r\n", o_msg_str);
		outMessage(o_msg_str, source);
	}
}
void ServiceConnector::processFlagInt(InCmdMessage* msg, const char* str, int* value, Msg_Source_t source)
{
	if (msg->hasFlag(str)) {
		*value = msg->getIntFlag(str);
		SC_TRACE("Received %s value %d\r\n", str, *value);
		ackMessage(str, source);
	}
}
void ServiceConnector::processReadFlagUChar(InCmdMessage* msg, const char* str, unsigned char value, Msg_Source_t source){
	if(msg->requestFlag(str)) {
		SC_TRACE("Received request for %s\r\n", str);
		OutMessage o_msg;
		const char* o_msg_str = o_msg.msgUCharFlag(str, value);
		SC_TRACE("Response: %s\r\n", o_msg_str);
		outMessage(o_msg_str, source);
	}
}
void ServiceConnector::processFlagUChar(InCmdMessage* msg, const char* str, unsigned char* value, Msg_Source_t source){
	if (msg->hasFlag(str)) {
		*value = msg->getUCharFlag(str);
		SC_TRACE("Received %s value %d\r\n", str, *value);
		ackMessage(str, source);
	}
}
void ServiceConnector::processReadFlagLong(InCmdMessage* msg, const char* str, long value, Msg_Source_t source)
{
	if(msg->requestFlag(str)) {
		SC_TRACE("Received request for %s\r\n", str);
		OutMessage o_msg;
		const char* o_msg_str = o_msg.msgLongFlag(str, value);
		SC_TRACE("Response: %s\r\n", o_msg_str);
		outMessage(o_msg_str, source);
	}
}
void ServiceConnector::processFlagLong(InCmdMessage* msg, const char* str, long* value, Msg_Source_t source)
{
	if (msg->hasFlag(str)) {
		*value = msg->getLongFlag(str);
		SC_TRACE("Received %s value %d\r\n", str, *value);
		ackMessage(str, source);
	}
}
void ServiceConnector::processReadFlagString(InCmdMessage* msg, const char* str, const char* value, Msg_Source_t source)
{
	if(msg->requestFlag(str)) {
		SC_TRACE("Received request for %s\r\n", str);
		OutMessage o_msg;
		const char* o_msg_str = o_msg.msgStringFlag(str, value);
		SC_TRACE("Response: %s\r\n", o_msg_str);
		outMessage(o_msg_str, source);
	}
}
void ServiceConnector::processFlagString(InCmdMessage* msg, const char* str, char* value, int len, Msg_Source_t source)
{
	if (msg->hasFlag(str)) {
		strncpy(value, msg->getStringFlag(str), len);
		SC_TRACE("Received %s value %s\r\n", str, value);
		ackMessage(str, source);
	}
}

void ServiceConnector::processInMessage(InCmdMessage* msg, Msg_Source_t source)
{
	if(msg->requestFlag(msg_Exec_Cmd_Reset)) {
		SC_TRACE("Resetting\r\n");
		ackMessage(msg_Exec_Cmd_Reset, source);
		init = true;
		System.reset();
	}
	if(msg->requestFlag(msg_Exec_Cmd_StoreEEPROM)) {
		SC_TRACE("Saving current config to EEPROM\r\n");
		sensor.storeEEPROMConfig();
		ackMessage(msg_Exec_Cmd_StoreEEPROM, source);
	}
	if(msg->requestFlag(msg_Exec_Cmd_ClearWiFiCredentials)) {
		SC_TRACE("Removing all WiFi Credentials\r\n");
		#if (PLATFORM_ID == 6)
		// Photon code here
		WiFi.clearCredentials();
		WiFi.off();
		#elif (PLATFORM_ID == 10)
		//Electron or other Particle device code here
		#endif
		ackMessage(msg_Exec_Cmd_ClearWiFiCredentials, source);
	}
	if (msg->hasFlag(msg_Int_Flag_Setup)) {
		int val_tmp = msg->getIntFlag(msg_Int_Flag_Setup);
		SC_TRACE("Received %s with value %d\r\n", msg_String_Flag_SSID, val_tmp);
		if (val_tmp>0 && val_tmp<5)
			scheduledSetupCommand = val_tmp;
		ackMessage(msg_Int_Flag_Setup, source);
	}
	if (msg->hasFlag(msg_String_Flag_SSID)) {
		#if (PLATFORM_ID == 6)
		// Photon code here
		WiFi.on();
		SC_TRACE("Received %s with value %s\r\n", msg_String_Flag_SSID, msg->getStringFlag(msg_String_Flag_SSID));
		if(msg->hasFlag(msg_String_Flag_PASS)) {
			SC_TRACE("Received %s with value %s\r\n", msg_String_Flag_PASS, msg->getStringFlag(msg_String_Flag_PASS));
			WiFi.setCredentials(msg->getStringFlag(msg_String_Flag_SSID), msg->getStringFlag(msg_String_Flag_PASS));
		} else {
			WiFi.setCredentials(msg->getStringFlag(msg_String_Flag_SSID));
		}
		WiFi.off();
		delay(100);
		WiFi.on();
		#elif (PLATFORM_ID == 10)
		//Electron or other Particle device code here
		#endif
		ackMessage(msg_String_Flag_SSID, source);
	}

	processReadFlagBool(msg, msg_Bool_Flag_Streaming_SD, SensorConfig.streamSD, source);
	processReadFlagBool(msg, msg_Bool_Flag_Streaming_BLE, SensorConfig.streamBLE, source);
	processReadFlagBool(msg, msg_Bool_Flag_Streaming_WiFi, SensorConfig.streamWiFi, source);
	processReadFlagBool(msg, msg_Bool_Flag_Streaming_MQTT, SensorConfig.streamMQTT, source);
	processReadFlagBool(msg, msg_Bool_Flag_Batch_WiFi, SensorConfig.batchWiFi, source);
	processReadFlagBool(msg, msg_Bool_Flag_EnableVOC, SensorConfig.vocInstalled, source);
	processReadFlagBool(msg, msg_Bool_Flag_EnableCO2, SensorConfig.co2Installed, source);
	processReadFlagBool(msg, msg_Bool_Flag_WifiStatus, SensorConfig.wifiEnabled, source);
	processReadFlagBool(msg, msg_Bool_Flag_SleepStatus, SensorConfig.sleepEnabled, source);
	processReadFlagBool(msg, msg_Bool_Flag_MirrorToUSBStatus, usbMirror, source);
	processReadFlagBool(msg, msg_Bool_Flag_USBPassthroughStatus, usbPassthrough, source);

	processFlagBool(msg, msg_Bool_Flag_Streaming_SD, &SensorConfig.streamSD, source);
	processFlagBool(msg, msg_Bool_Flag_Streaming_BLE, &SensorConfig.streamBLE, source);
	processFlagBool(msg, msg_Bool_Flag_Streaming_WiFi, &SensorConfig.streamWiFi, source);
	processFlagBool(msg, msg_Bool_Flag_Streaming_MQTT, &SensorConfig.streamMQTT, source);
	processFlagBool(msg, msg_Bool_Flag_Batch_WiFi, &SensorConfig.batchWiFi, source);
	processFlagBool(msg, msg_Bool_Flag_EnableVOC, &SensorConfig.vocInstalled, source);
	processFlagBool(msg, msg_Bool_Flag_EnableCO2, &SensorConfig.co2Installed, source);

	processFlagBool(msg, msg_Bool_Flag_WifiStatus, &SensorConfig.wifiEnabled, source);
	if (msg->hasFlag(msg_Bool_Flag_WifiStatus)) {
		applyWiFiStatus();
	}

	processFlagBool(msg, msg_Bool_Flag_SleepStatus, &SensorConfig.sleepEnabled, source);
	processFlagBool(msg, msg_Bool_Flag_MirrorToUSBStatus, &usbMirror, source);
	processFlagBool(msg, msg_Bool_Flag_USBPassthroughStatus, &usbPassthrough, source);

	long power = sensor.getPowerLevel();
	processReadFlagLong(msg, msg_Long_Flag_ReadPower, power, source);
	processReadFlagLong(msg, msg_Long_Flag_StreamingInterval, SensorConfig.intervalTime, source);
	processFlagLong(msg, msg_Long_Flag_StreamingInterval, &SensorConfig.intervalTime, source);

	processReadFlagUChar(msg, msg_Int_Flag_SD_Reads_Format, (unsigned char)SensorConfig.messageEncodingSD, source);
	processReadFlagUChar(msg, msg_Int_Flag_BLE_Reads_Format, (unsigned char)SensorConfig.messageEncodingBLE, source);
	processReadFlagUChar(msg, msg_Int_Flag_WiFi_Reads_Format, (unsigned char)SensorConfig.messageEncodingCloud, source);
	processReadFlagUChar(msg, msg_Int_Flag_MQTT_Reads_Format, (unsigned char)SensorConfig.messageEncodingMQTT, source);

	processFlagUChar(msg, msg_Int_Flag_SD_Reads_Format, (unsigned char*)&SensorConfig.messageEncodingSD, source);
	processFlagUChar(msg, msg_Int_Flag_BLE_Reads_Format, (unsigned char*)&SensorConfig.messageEncodingBLE, source);
	processFlagUChar(msg, msg_Int_Flag_WiFi_Reads_Format, (unsigned char*)&SensorConfig.messageEncodingCloud, source);
	processFlagUChar(msg, msg_Int_Flag_MQTT_Reads_Format, (unsigned char*)&SensorConfig.messageEncodingMQTT, source);

	//String s(sizeof(int));
	//processReadFlagString(msg, msg_String_Flag_NodeID, s.c_str()/*System.deviceID()*/, source);
	processReadFlagString(msg, msg_String_Flag_NodeID, System.deviceID(), source);
	processReadFlagString(msg, msg_String_Flag_AFESerial, SensorConfig.AFESerial, source);
	processReadFlagString(msg, msg_String_Flag_MAC, SensorConfig.MACAddress, source);

	processFlagString(msg, msg_String_Flag_AFESerial, SensorConfig.AFESerial, 9, source);
	processFlagString(msg, msg_String_Flag_MAC, SensorConfig.MACAddress, 19, source);

	if (msg->hasTime()) {
		SC_TRACE("Has time ts: %d\r\n", msg->getTime());
		Time.setTime(msg->getTime());
	}
}

void ServiceConnector::receiveMessageUSB(char* str)
{
	/*char buf[MAX_MSG_LEN+1];
	strncpy(buf, str, MAX_MSG_LEN);*/
	SC_TRACE("Received USB %s\r\n",str);
	InCmdMessage msg(str);
	processInMessage(&msg, usb);
}
void ServiceConnector::receiveMessageBLE(char* str)
{
	/*char buf[MAX_MSG_LEN+1];
	strncpy(buf, str, MAX_MSG_LEN);*/
	SC_TRACE("Received BLE |%s|\r\n", str);
	InCmdMessage msg(str);
	processInMessage(&msg, ble);
	SC_TRACE("Done Processing BLE |%s|\r\n", str);
}
void ServiceConnector::receiveMessageWiFi(char* str)
{
	/*char buf[MAX_MSG_LEN+1];
	strncpy(buf, str, MAX_MSG_LEN);*/
	SC_TRACE("Received WiFi %s\r\n",str);
	InCmdMessage msg(str);
	processInMessage(&msg, wifi);
}

#if (PLATFORM_ID == 6)
// Photon code here
void ServiceConnector::applyWiFiStatus() {
	if (sensor.isWiFiEnabled()) {
		if (!WiFi.connecting() && !WiFi.ready()) {
			SC_TRACE("Enabling wifi\r\n");
			WiFi.on();
			WiFi.connect(WIFI_CONNECT_SKIP_LISTEN);
		}
		if (WiFi.ready() && Particle.connected() == false) {
			SC_TRACE("Connecting Particle Cloud\r\n");
			Particle.connect();
		}
		if (nextSyncTime < Time.now() && Particle.connected()) {
			SC_TRACE("Particle.syncTime\r\n");
			Particle.syncTime();
			nextSyncTime = Time.now() + 3600 * 24;
		}
	} else if (WiFi.connecting() || WiFi.ready()) {
		SC_TRACE("Turn off wifi\r\n");
		WiFi.off();
	}
}
#elif (PLATFORM_ID == 10)
//Electron or other Particle device code here
void ServiceConnector::applyWiFiStatus() {
	if (sensor.isWiFiEnabled()) {
		if (!Cellular.connecting() && !Cellular.ready()) {
			SC_TRACE("Enabling cellular\r\n");
			Cellular.on();
			Cellular.connect();
		}
		if (Cellular.ready() && Particle.connected() == false) {
			SC_TRACE("Connecting Particle Cloud\r\n");
			Particle.connect();
		}
		if (nextSyncTime < Time.now() && Particle.connected()) {
			SC_TRACE("Particle.syncTime\r\n");
			Particle.syncTime();
			nextSyncTime = Time.now() + 3600 * 24;
		}
	} else if (Cellular.connecting() || Cellular.ready()) {
		SC_TRACE("Turn off cellular\r\n");
		Cellular.off();
	}
}
#endif

void ServiceConnector::processCommands()
{
	//Execute setup configurations scheduled by the setup button
	if(scheduledSetupCommand>0) {
		execute_setup_command(scheduledSetupCommand);
	}
	sensor.checkWakeupPinStatus();

	applyWiFiStatus();

	//Do it only when the BLE controlled PIN is HIGH
	long remaining_wait =
				SensorConfig.intervalTime - (Time.now() - lastReadingTime);

	//Serial1.printlnf("remaining_wait %d", remaining_wait);

	if (sensor.isSleepEnabled() && remaining_wait > 0) {
		//Serial1.printlnf("going to sleep for %d!",remaining_wait);
		//if(sensor.isUSBSerialConnected())
		//	Serial.flush();
		Serial1.flush();
		System.sleep(SLEEP_MODE_DEEP, remaining_wait);
		//System.sleep(A7, CHANGE, sleep_sec); //This will not reset the sensor but less power save
	}
}

bool ServiceConnector::updateReadings()
{
	sensor.checkWakeupPinStatus();
	// When we control the BLE serial module directly form the USB
	// port we disable the reading of data... it is used just to configure
	// or debug the BLE module, not while in use
	if (serialBypass)
		return false;

	long nowTime = Time.now();
	long waited = nowTime - lastReadingTime;

	if (waited>=0 && waited<=SensorConfig.intervalTime)
		return false;
	//Reinitialize the reading time for a new sample
	//We use second precision that is supported by the real time clock
	lastReadingTime = nowTime; //original was using millis();
	//Read all raw values
	sensor.readAll(); 	// sensor.lastReading;
	sensor.afe.convertModel(sensor.lastReading.hum_T / 100.0, sensor.lastReading.hum_H / 100.0, sensor.lastReading.bar_P / 10.0);
	if (SensorConfig.vocInstalled) {
		sensor.checkWakeupPinStatus();
		voc.readAll(); 		// voc.lastReading;
		voc.convertModel(sensor.lastReading.hum_T / 100.0, sensor.lastReading.hum_H / 100.0, sensor.lastReading.bar_P / 10.0);
	}
	if (SensorConfig.co2Installed) {
		sensor.checkWakeupPinStatus();
		co2.readAll();
		co2.convertModel(sensor.lastReading.hum_T / 100.0, sensor.lastReading.hum_H / 100.0, sensor.lastReading.bar_P / 10.0);
	}
	sensor.checkWakeupPinStatus();
	return true;
}

float ServiceConnector::convertRawGasToVoltage(int rng, int rawValue) {
	float gain = rng;
  if (rng == 0)
  	gain = 2.0 / 3.0;
  float voltCalc = 4096.0 / (gain * 0x7FFF);
  return (rawValue * voltCalc);
}

Readings_History_t* ServiceConnector::UpdateHistory() {
	if (ReadingsHistoryPos>=READINGS_HISTORY_SIZE){
		ReadingsHistoryPos=0;
	}
	return &ReadingsHistory[ReadingsHistoryPos++];
}
float ServiceConnector::ReadingsHistoryMean(int offset) {
	float acc = 0;
	for (int i=0; i<READINGS_HISTORY_SIZE; i++)
		acc+=*(float*)(&ReadingsHistory[i]+offset);
	return acc / READINGS_HISTORY_SIZE;
}
float ServiceConnector::ReadingsHistoryVar(int offset, float mean){
	float acc = 0;
	for (int i=0; i<READINGS_HISTORY_SIZE; i++)
		acc+=pow(*(float*)(&ReadingsHistory[i]+offset)-mean, 2);
	return acc / (READINGS_HISTORY_SIZE-1);
}

void ServiceConnector::ComputeNeuralNetworkInputs(float inputs[]) {
	inputs[0] = ReadingsHistoryMean(offsetof(Readings_History_t, CO2));
	inputs[1] = ReadingsHistoryVar(offsetof(Readings_History_t, CO2),inputs[0]);
	inputs[2] = ReadingsHistoryMean(offsetof(Readings_History_t, S1A));
	inputs[3] = ReadingsHistoryVar(offsetof(Readings_History_t, S1A),inputs[2]);
	inputs[4] = ReadingsHistoryMean(offsetof(Readings_History_t, S1W));
	inputs[5] = ReadingsHistoryVar(offsetof(Readings_History_t, S1W),inputs[4]);
	inputs[6] = ReadingsHistoryMean(offsetof(Readings_History_t, S2A));
	inputs[7] = ReadingsHistoryVar(offsetof(Readings_History_t, S2A),inputs[6]);
	inputs[8] = ReadingsHistoryMean(offsetof(Readings_History_t, S2W));
	inputs[9] = ReadingsHistoryVar(offsetof(Readings_History_t, S2W),inputs[8]);
	inputs[10] = ReadingsHistoryMean(offsetof(Readings_History_t, S3A));
	inputs[11] = ReadingsHistoryVar(offsetof(Readings_History_t, S3A),inputs[10]);
	inputs[12] = ReadingsHistoryMean(offsetof(Readings_History_t, S3W));
	inputs[13] = ReadingsHistoryVar(offsetof(Readings_History_t, S3W),inputs[12]);
	inputs[14] = ReadingsHistoryMean(offsetof(Readings_History_t, humidity));
	inputs[15] = ReadingsHistoryVar(offsetof(Readings_History_t, humidity),inputs[14]);
	inputs[16] = ReadingsHistoryMean(offsetof(Readings_History_t, temperature));
	inputs[17] = ReadingsHistoryVar(offsetof(Readings_History_t, temperature),inputs[16]);

	inputs[0] = (inputs[0] - metasense_CO2_mean_mean)/metasense_CO2_mean_variance;
	inputs[1] = (inputs[1] - metasense_CO2_var_mean)/metasense_CO2_var_variance;
	inputs[2] = (inputs[2] - metasense_S1A_mean_mean)/metasense_S1A_mean_variance;
	inputs[3] = (inputs[3] - metasense_S1A_var_mean)/metasense_S1A_var_variance;
	inputs[4] = (inputs[4] - metasense_S1W_mean_mean)/metasense_S1W_mean_variance;
	inputs[5] = (inputs[5] - metasense_S1W_var_mean)/metasense_S1W_var_variance;
	inputs[6] = (inputs[6] - metasense_S2A_mean_mean)/metasense_S2A_mean_variance;
	inputs[7] = (inputs[7] - metasense_S2A_var_mean)/metasense_S2A_var_variance;
	inputs[8] = (inputs[8] - metasense_S2W_mean_mean)/metasense_S2W_mean_variance;
	inputs[9] = (inputs[9] - metasense_S2W_var_mean)/metasense_S2W_var_variance;
	inputs[10] = (inputs[10] - metasense_S3A_mean_mean)/metasense_S3A_mean_variance;
	inputs[11] = (inputs[11] - metasense_S3A_var_mean)/metasense_S3A_var_variance;
	inputs[12] = (inputs[12] - metasense_S3W_mean_mean)/metasense_S3W_mean_variance;
	inputs[13] = (inputs[13] - metasense_S3W_var_mean)/metasense_S3W_var_variance;
	inputs[14] = (inputs[14] - metasense_humidity_mean_mean)/metasense_humidity_mean_variance;
	inputs[15] = (inputs[15] - metasense_humidity_var_mean)/metasense_humidity_var_variance;
	inputs[16] = (inputs[16] - metasense_temperature_mean_mean)/metasense_temperature_mean_variance;
	inputs[17] = (inputs[17] - metasense_temperature_var_mean)/metasense_temperature_var_variance;
}

void ServiceConnector::processReadings() {
	//Convert and send as needed by the streaming mode
	uint32_t timestamp = Time.now();
	this->do_streamAll(timestamp, sensor.afe.lastReading, sensor.lastReading,
		sensor.afe.lastModel,
		voc.lastReading, voc.lastModel, co2.lastReading, co2.lastModel,
		SensorConfig.streamWiFi, SensorConfig.streamSD,
		SensorConfig.streamBLE, usbMirror,
		SensorConfig.batchWiFi,
		SensorConfig.streamMQTT);
		if(RunNeuralNet) {
			M_MQTT_TRACE("Running NeuralNetwork loop\r\n");
			//Get the history buffer (it's circular right now size 3)
			Readings_History_t* currentHistory = UpdateHistory();
			//Update the values of the buffer with current readings
			currentHistory->CO2 = co2.lastReading.val;
			currentHistory-> S1A = convertRawGasToVoltage(sensor.afe.lastReading.range, sensor.afe.lastReading.ADC0_0);
			currentHistory-> S1W=convertRawGasToVoltage(sensor.afe.lastReading.range, sensor.afe.lastReading.ADC0_1);
			currentHistory-> S2A=convertRawGasToVoltage(sensor.afe.lastReading.range, sensor.afe.lastReading.ADC1_2);
			currentHistory-> S2W=convertRawGasToVoltage(sensor.afe.lastReading.range, sensor.afe.lastReading.ADC1_3);
			currentHistory-> S3A=convertRawGasToVoltage(sensor.afe.lastReading.range, sensor.afe.lastReading.ADC1_0);
			currentHistory-> S3W=convertRawGasToVoltage(sensor.afe.lastReading.range, sensor.afe.lastReading.ADC1_1);
			currentHistory-> humidity=sensor.lastReading.hum_H / 100.0;
			currentHistory-> pressure=sensor.lastReading.bar_P / 10.0;
			currentHistory-> temperature=sensor.lastReading.bar_T  / 10.0;
			//Create the nn input processing the history buffer (it's currently size 20)
			float inputs[N_IN];
			ComputeNeuralNetworkInputs(inputs);
			//Create the json message
			char* msg =  _nn.Loop(timestamp, N_IN, inputs);

			M_MQTT_TRACE("outMsgToMQTT: isWiFiEnabled %d, updatesPending %d, MQTTClientEnabled %d\r\n",
					sensor.isWiFiEnabled(), System.updatesPending(), MQTTClientEnabled);
			if (sensor.isWiFiEnabled() && Particle.connected() && !System.updatesPending()
					&& MQTTClientEnabled) {
				M_MQTT_TRACE("Publishing MSG to MQTT: %s\r\n", msg);
				if (!mqttClient.isConnected()) {
					M_MQTT_TRACE("Not connected to MQTT... reconnecting\r\n");
					mqttClient.connect("Particle_" + System.deviceID(), MQTT_Server_Username, MQTT_Server_Password);
				}
				if (mqttClient.isConnected()) {
					M_MQTT_TRACE("Client is connected... publish to MetaSense/nnjson\r\n");
					mqttClient.publish("MetaSense/nnjson", msg);
				}
			}
		}
}

void ServiceConnector::outMsgToCloud(const char* str) {
	if (sensor.isWiFiEnabled() && Particle.connected() && !System.updatesPending()) {
		SC_TRACE("Publishing MSG to cloud: %s\r\n", str);
		Particle.publish("MSG", str, PRIVATE | WITH_ACK);
	}
}

void ServiceConnector::outBMsgToCloud(const char* str) {
	if (sensor.isWiFiEnabled() && Particle.connected() && !System.updatesPending()) {
		SC_TRACE("Publishing MSG to cloud: %s\r\n", str);
		Particle.publish("BMSG", str, PRIVATE | WITH_ACK);
	}
}

void ServiceConnector::outMsgToMQTT(const char* msg) {
	M_MQTT_TRACE("outMsgToMQTT: isWiFiEnabled %d, updatesPending %d, MQTTClientEnabled %d\r\n",
			sensor.isWiFiEnabled(), System.updatesPending(), MQTTClientEnabled);
	if (sensor.isWiFiEnabled() && Particle.connected() && !System.updatesPending()
			&& MQTTClientEnabled) {
		M_MQTT_TRACE("Publishing MSG to MQTT: %s\r\n", msg);
		if (!mqttClient.isConnected()) {
			M_MQTT_TRACE("Not connected to MQTT... reconnecting\r\n");
			mqttClient.connect("Particle_" + System.deviceID(), MQTT_Server_Username, MQTT_Server_Password);
    }
		if (mqttClient.isConnected()) {
			M_MQTT_TRACE("Client is connected... publish to MetaSense/msgjson\r\n");
			mqttClient.publish("MetaSense/msgjson", msg);
    }
	}
}

void ServiceConnector::outB64MsgToMQTT(const char* msg) {
	M_MQTT_TRACE("outB64MsgToMQTT: isWiFiEnabled %d, updatesPending %d, MQTTClientEnabled %d\r\n",
			sensor.isWiFiEnabled(), System.updatesPending(), MQTTClientEnabled);
	if (sensor.isWiFiEnabled() && !System.updatesPending()
			&& MQTTClientEnabled) {
		M_MQTT_TRACE("Publishing MSG to MQTT: %s\r\n", msg);
		if (!mqttClient.isConnected()) {
			M_MQTT_TRACE("Not connected to MQTT... reconnecting\r\n");
			mqttClient.connect("Particle_" + System.deviceID(), MQTT_Server_Username, MQTT_Server_Password);
    }
		if (mqttClient.isConnected()) {
			M_MQTT_TRACE("Client is connected... publish to MetaSense/msgb64\r\n");
			mqttClient.publish("MetaSense/msgb64", msg);
    }
	}
}

void ServiceConnector::outBMsgToMQTT(const uint8_t* msg, unsigned int len) {
	M_MQTT_TRACE("outBMsgToMQTT: isWiFiEnabled %d, updatesPending %d, MQTTClientEnabled %d\r\n",
			sensor.isWiFiEnabled(), System.updatesPending(), MQTTClientEnabled);
	if (sensor.isWiFiEnabled() && Particle.connected() && !System.updatesPending()
			&& MQTTClientEnabled) {
		M_MQTT_TRACE("Publishing binary MSG to MQTT msg len: %d\r\n", len);
		if (!mqttClient.isConnected()) {
			M_MQTT_TRACE("Not connected to MQTT... reconnecting\r\n");
			mqttClient.connect("Particle_" + System.deviceID(), MQTT_Server_Username, MQTT_Server_Password);
    }
		if (mqttClient.isConnected()) {
			M_MQTT_TRACE("Client is connected... publish to MetaSense/msgbin\r\n");
			mqttClient.publish("MetaSense/msgbin", msg, len);
    }
	}
}

void ServiceConnector::uploadMsg(byte msg[], int binBufLen, const char* endpoint) {
  if (sensor.isWiFiEnabled() && Particle.connected() &&
		!System.updatesPending() && MQTTClientEnabled) {
			//TODO: fix uploadMsg using MQTT
	}
}

void ServiceConnector::do_streamAll(uint32_t timestamp,
	AFE::Gas_Raw_t& gas, Sensor::Reading_Raw_t& bar_hum,
	AFE::Gas_Model_t& model,
	VOC::VOC_Raw_t& vocRaw, VOC::VOC_Model_t& vocModel,
	CO2::CO2_Raw_t& co2Raw, CO2::CO2_Model_t& co2Model,
	bool wifi, bool sd, bool ble, bool usb, bool batch, bool mqtt) {
  //Serial1.println("do_streamAll");
	OutMessage msg = OutMessage(timestamp);
	msg.setAFEGas(gas);
	msg.setSensorsReadings(bar_hum);
	msg.setGasModel(model);
	if(SensorConfig.vocInstalled) {
		SC_TRACE("vocInstalled\r\n");
		msg.setVOCRaw(vocRaw);
		msg.setVOCModel(vocModel);
	}
	if(SensorConfig.co2Installed) {
		SC_TRACE("co2Installed\r\n");
		msg.setCO2Raw(co2Raw);
		msg.setCO2Model(co2Model);
	}

	char* msg_str = msg.toString();
	char* msg_b64_str = msg.toStringB64();

	binaryReadMessageEx_t buf;
	int binBufLen = msg.binaryMsgLen();
	uint8_t* binBuf = msg.toBinaryMsg((uint8_t*)&buf);

	//M_OUT_MSG
	M_MQTT_TRACE("ble value %d, encoding %d\n\r", ble, sensor.getConfig()->messageEncodingBLE)
	if (ble) {
		switch(sensor.getConfig()->messageEncodingBLE) {
			case json:
			Serial1.println(msg_str);
			break;
			case binB64:
			Serial1.println(msg_b64_str);
			break;
			case bin:
			Serial1.write(binBuf, binBufLen);
			break;
		}
	}
	if (usb && sensor.isUSBSerialConnected()) {
		switch(sensor.getConfig()->messageEncodingBLE) {
			case json:
			Serial.println(msg_str);
			break;
			case binB64:
			Serial.println(msg_b64_str);
			break;
		}
	}
	if (wifi) {
		switch(sensor.getConfig()->messageEncodingCloud) {
			case json:
			outMsgToCloud(msg_str);
			break;
			case binB64:
			outBMsgToCloud(msg_b64_str);
			break;
			case bin:
			uploadMsg(binBuf, binBufLen, msg_endpoint);
			break;
		}
	}
	M_MQTT_TRACE("mqtt value %d, encoding %d\n\r", mqtt, sensor.getConfig()->messageEncodingMQTT)
	if (mqtt) {
		switch(sensor.getConfig()->messageEncodingMQTT) {
			case json:
			M_MQTT_TRACE("mqtt json message\n\r")
			outMsgToMQTT(msg_str);
			break;
			case binB64:
			M_MQTT_TRACE("mqtt B64 message\n\r")
			outB64MsgToMQTT(msg_b64_str);
			break;
			case bin:
			M_MQTT_TRACE("mqtt BIN message\n\r")
			outBMsgToMQTT(binBuf, binBufLen);
			break;
		}
	}
	if (sd) {
		switch(sensor.getConfig()->messageEncodingSD) {
			case json:
			logMessageToSD(msg_str);
			break;
			case binB64:
			logMessageToSD(msg_b64_str);
			break;
		}
	}
	if (batch) {
		if (msg.hasExtra())
		{
			msg.toB64Buffer(&MsgBuffer.BinMsgBufferEx[BinMsgBufferPos++]);
		} else {
			msg.toB64Buffer(&MsgBuffer.BinMsgBuffer[BinMsgBufferPos++]);
		}
		if (BinMsgBufferPos>=BIN_BUF_MAX_ELEMS){
			size_t input_size;
			uint8_t *in;
			if (msg.hasExtra())
			{
				input_size = sizeof(MsgBuffer.BinMsgBufferEx);
				in = (uint8_t *)&MsgBuffer.BinMsgBufferEx;
			} else {
				input_size = sizeof(MsgBuffer.BinMsgBuffer);
				in = (uint8_t *)&MsgBuffer.BinMsgBuffer;
			}
			switch(sensor.getConfig()->messageEncodingCloud) {
				case binB64:
				doSendBatchToCloud(in, input_size, false);
				break;
				case bin:
				doSendBatchToCloud(in, input_size, true);
				break;
			}
			BinMsgBufferPos=0;
		}
	}
}
void ServiceConnector::doSendBatchToCloud(uint8_t *in, size_t input_size, bool sendDirect){
	if (sensor.isWiFiEnabled() && Particle.connected() && !System.updatesPending()) {
		SC_TRACE("Publishing batch of %d messages to cloud\r\n", BinMsgBufferPos);
		//Serial.flush();
		//delay(200);
		int inputLen;
		int encodedLen;
		char *string;
		char *source;

		heatshrink_encoder_reset(&hse);
		SC_TRACE("Input buffer %d bytes.\r\n", input_size);
		size_t comp_sz = input_size + (input_size/2) + 4;
		uint8_t *out = (uint8_t*)malloc(comp_sz);
		SC_TRACE("Allocation of %d bytes for out returned 0x%X.\r\n", comp_sz, (int)out);
		if (out == NULL)
	  {
			SC_TRACE("Error allocationg memory for compression\r\n");
			 return; //Not enough memory
	  }
		memset(out, 0, comp_sz);

		size_t count = 0;
		uint32_t sunk = 0;
    uint32_t polled = 0;

    while (sunk < input_size) {
				HSE_sink_res res = heatshrink_encoder_sink(&hse, &in[sunk], input_size - sunk, &count);
        if ( res != HSER_SINK_OK ){
					SC_TRACE("Internal error - compression failed: %zd\r\n", res);
					return;
				}
        sunk += count;
        SC_TRACE("^^ sunk %d\r\n", count);
        if (sunk == input_size) {
					HSE_finish_res f_res = heatshrink_encoder_finish(&hse);
					if ( HSER_FINISH_MORE == f_res )
						SC_TRACE("Finish: %d\r\n", f_res);
        }
        HSE_poll_res pres;
        do {                    /* "turn the crank" */
            pres = heatshrink_encoder_poll(&hse, &out[polled], comp_sz - polled, &count);
            polled += count;
            SC_TRACE("^^ sunk %d\r\n", count);
        } while (pres == HSER_POLL_MORE);
    }
    //uint32_t compressed_size = polled;
		// int cmp_status = mz_compress(out, &cmp_len, (const unsigned char *)in, src_len);
		// if (cmp_status != MZ_OK)
  	// 	SC_TRACE("Internal error - compression failed: %d\r\n", cmp_status);
	  /* check for an incompressible block */
	  if (polled >= input_size)
			SC_TRACE("This block contains incompressible data.\r\n");
		SC_TRACE("Compression rate %f.\r\n", (float)polled / (float)input_size);
		inputLen = polled;

		if (sendDirect){
			uploadMsg(out, inputLen, batch_endpoint);
			free(out);
			return;
		}

		encodedLen = base64_enc_len(inputLen);

		string = (char*)malloc( encodedLen );
		MSG_TRACE("Originally had %d bytes, compressed to %d bytes and encoded to %d bytes in B64\r\n", input_size, inputLen, encodedLen);

		source = (char*)out;
		if (string == NULL){
			free(source);
			return;
		}
		base64_encode(string, source, inputLen);
		MSG_TRACE("Encoded to B64\r\n");
		free(source);

		MSG_TRACE("Encoded Message \r\n%s\r\n", string);
		char send_buf[CHUNK_MSG_SIZE+1];
		snprintf(send_buf,9, "%08X", Time.now());
		int message_chunk_size = CHUNK_MSG_SIZE-12;
		int chunks_number = encodedLen/message_chunk_size;
		int lastChunk_size = encodedLen%message_chunk_size;
		if (lastChunk_size!=0)
			chunks_number++;
		else
			lastChunk_size = message_chunk_size;
		for (int i=0; i<chunks_number; i++){
			MSG_TRACE("Sending chunk %d\r\n", i);
			char* start = string+i*message_chunk_size;
			int current_chunk_size;
			if (i==(chunks_number-1))
				current_chunk_size = lastChunk_size;
			else
				current_chunk_size = message_chunk_size;
			MSG_TRACE("Chunk message number %d of %d\r\n", i, chunks_number);
			snprintf(send_buf+8, 3, "%02d", i);
			snprintf(send_buf+10, 3, "%02d", chunks_number);
			MSG_TRACE("Chunk size %d\r\n", current_chunk_size);
			strncpy(send_buf+12, start, current_chunk_size);
			send_buf[current_chunk_size+12]=0;
			MSG_TRACE("Chunk to send %s\r\n", send_buf);
			Particle.publish("BATCH", send_buf);
		}
		free(string);
	}
}
void ServiceConnector::logMessageToSD(const char* msg) {
	sensor.selectSPI(Sensor::SDCARD);

	SC_TRACE("sensor.logFile: %d\r\n", (int)sensor.logFile);
	if (!sensor.logFile) {
		SC_TRACE("Init SD again\r\n");
		//SD.root.close();
		/*bool initCardResult = SD.card.init(SPI_FULL_SPEED, SDCSPin);
		bool initVolumeResult = SD.volume.init(SD.card);
	  bool initRootResult = SD.root.openRoot(SD.volume);

		bool reInitResult = initCardResult && initVolumeResult && initRootResult;
		M_TRACE("initCardResult %d, initVolumeResult %d, initRootResult %d, reInitResult: %d",
			initCardResult, initVolumeResult, initRootResult, reInitResult);*/
		bool reInitResult = SD.begin(SDCSPin);
		if (reInitResult) {
			SC_TRACE("Open file again\r\n");
			sensor.logFile = SD.open(sensor.filename, FILE_WRITE);
		}
	}

	SC_TRACE("sensor.logFile: %d\r\n", (int)sensor.logFile);

	if (sensor.logFile) {
		cid_t cid;
		SC_TRACE("Reading CID\r\n");
		if (!SD.card()->readCID(&cid)) {
			SC_TRACE("Error in reading CID\r\n");
			if (sensor.logFile) {
				SC_TRACE("Closing file\r\n");
				sensor.logFile.close();
				//SD.root.close();
			}
		}
	}

	if (sensor.logFile) {
		SC_TRACE("Log to file and flush start\r\n");
		sensor.logFile.println(msg);
		//msg->SerializeTo(*sensor.logFile); // contains println
		sensor.logFile.flush();
		SC_TRACE(" to file and flush done\r\n");
	}
	sensor.selectSPI(Sensor::NONE);
}

/*void ServiceConnector::do_sendConfig(SensorEEPROMConfig_t* configuration, bool wifi, bool sd, bool ble, bool usb) {
	OutMessage msg = OutMessage(Time.now());
	msg.setAFESerial(SensorConfig.AFESerial);

	char* msg_str = msg.toString();
	//M_OUT_MSG
	if (ble)
	{
		Serial1.println(msg_str);
	}
	if (usb) {
		Serial.println(msg_str);
	}

	if (wifi)
		outMsgToCloud(msg_str);
	if (sd)
		logMessageToSD(msg_str);
}*/
