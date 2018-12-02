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

// ServiceConnector.h

#ifndef _SERVICECONNECTOR_h
#define _SERVICECONNECTOR_h
#include "logger.h"
#include "application.h"
#include "Message.h"
#include "AFE.h"
#include "VOC.h"
#include "Sensor.h"
//#include "MirroringStream.h"
#include "StringStream.h"
#include "PhotonConfig.h"
#include "MQTT.h"
#include "NeuralNetwork.h"
#include "distributions.h"
#include <math.h>
//#include "httpsclient-particle.h"

void software_Reboot();

// #define UPLOAD_URL "4ddpn9qeb0.execute-api.us-west-2.amazonaws.com"
// #define UPLOAD_MSG_PATH "/Release/binary-msg-upload/"
// #define UPLOAD_BATCH_PATH "/Release/binary-batch-upload/"
// #define HEADER_KEY_NAME "x-api-key"
// #define HEADER_KEY_VALUE "g114c8FUxn2LLcJy0LuuXM4VHemAVVj7o144zIZ5"


#define BIN_BUF_MAX_ELEMS 20 //Max 60
#define CHUNK_MSG_SIZE 255

#define READINGS_HISTORY_SIZE 3

union BinMsgBuffer_t {
	binaryReadMessage_t BinMsgBuffer[BIN_BUF_MAX_ELEMS];
	binaryReadMessageEx_t BinMsgBufferEx[BIN_BUF_MAX_ELEMS];
};

typedef struct {
	float CO2;
	float S1A;
	float S1W;
	float S2A;
	float S2W;
	float S3A;
	float S3W;
	float humidity;
	float pressure;
	float temperature;
} Readings_History_t;

extern unsigned char BinMsgBufferPos;
extern union BinMsgBuffer_t MsgBuffer;

class ServiceConnector
{
  public:
    typedef enum {usb, ble, wifi} Msg_Source_t;
    ServiceConnector(Sensor& sensor, VOC& voc, CO2& co2, MQTT& mqtt, NeuralNetwork& nn);
		bool MQTTClientEnabled;
		bool RunNeuralNet;
    void begin();
    void processCommands();
    bool updateReadings();
    void processReadings();

    void setup_button_clicked(int times);

    void receiveMessageUSB(char* str);
    void receiveMessageBLE(char* str);
    void receiveMessageWiFi(char* str);

  private:
		Readings_History_t ReadingsHistory[READINGS_HISTORY_SIZE];
		int ReadingsHistoryPos = 0;
		Readings_History_t* UpdateHistory();
		void ComputeNeuralNetworkInputs(float inputs[]);
		float ReadingsHistoryMean(int offset);
		float ReadingsHistoryVar(int offset, float mean);

		float convertRawGasToVoltage(int rng, int rawValue);
    void processInMessage(InCmdMessage* msg, Msg_Source_t source);
    void processFlagBool(InCmdMessage* msg, const char* str, bool* value, Msg_Source_t source);
    void processReadFlagBool(InCmdMessage* msg, const char* str, bool value, Msg_Source_t source);
    void processFlagInt(InCmdMessage* msg, const char* str, int* value, Msg_Source_t source);
    void processReadFlagInt(InCmdMessage* msg, const char* str, int value, Msg_Source_t source);
    void processFlagLong(InCmdMessage* msg, const char* str, long* value, Msg_Source_t source);
    void processReadFlagLong(InCmdMessage* msg, const char* str, long value, Msg_Source_t source);
    void processReadFlagString(InCmdMessage* msg, const char* str, const char* value, Msg_Source_t source);
    void processFlagString(InCmdMessage* msg, const char* str, char* value, int len, Msg_Source_t source);
		void processFlagUChar(InCmdMessage* msg, const char* str, unsigned char* value, Msg_Source_t source);
    void processReadFlagUChar(InCmdMessage* msg, const char* str, unsigned char value, Msg_Source_t source);

    void applyWiFiStatus();

    unsigned long startSampleTime;
    unsigned long endSampleTime;

    bool serialBypass;
    //AFE& afe;
    Sensor& sensor;
    VOC& voc;
    CO2& co2;
		MQTT& mqttClient;
		NeuralNetwork& _nn;
    // bool streamBLE : 1;
    // bool streamWiFi : 1;
    // bool streamSD : 1;

    void execute_setup_command(int command);
    //unsigned long remainingWait();
    void performSensorRead();
    void doSendBatchToCloud(uint8_t *in, size_t input_size, bool sendDirect);
    void do_streamAll(uint32_t timestamp,
			AFE::Gas_Raw_t& gas, Sensor::Reading_Raw_t& bar_hum,
	  	AFE::Gas_Model_t& model,
    	VOC::VOC_Raw_t& vocRaw, VOC::VOC_Model_t& vocModel,
    	CO2::CO2_Raw_t& co2Raw, CO2::CO2_Model_t& co2Model,
    	bool wifi, bool sd, bool ble, bool usb, bool batch, bool mqtt);

		void uploadMsg(byte msg[], int binBufLen, const char* endpoint);

    void outMessage(const char* msg, Msg_Source_t dest);
    void ackMessage(const char* str, Msg_Source_t source);
		void outMsgToCloud(const char* msg);
		void outBMsgToCloud(const char* msg);
		void outMsgToMQTT(const char* msg);
		void outB64MsgToMQTT(const char* msg);
		void outBMsgToMQTT(const uint8_t* msg, unsigned int len);
    void logMessageToSD(const char* msg);
    void syncTimeWithWiFi();
    //void doReceiveCommand();
};

#endif
