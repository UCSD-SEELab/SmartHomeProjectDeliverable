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

#include <stdlib.h>
// #include "application.h"
//#include <SPI.h>
#include "Sensor.h"

#define W	0
#define D	1

//char measurement[MEASUREMENT_MESSAGE_LENGHT+1];
#define SD_CONFIG_FILE_NAME "MS.CFG"

Sensor::Sensor(byte humClkPin, byte humDataPin, byte barCSPin, byte sdCSPin,
	byte ucCSPin, uint8_t i2cAddress0, uint8_t i2cAddress1) :
	_humClkPin(humClkPin), _humDataPin(humDataPin), _barCSPin(barCSPin),
	_sdCSPin(sdCSPin), _ucCSPin(ucCSPin), _now(0), _lastMillis(0),
	afe(i2cAddress0, i2cAddress1), sht1x(humDataPin, humClkPin)
{
	wakeupPinStatus = false;
	//	sht1x = new SHT1x(humDataPin, humClkPin);
}

/*unsigned long Sensor::now() {
	_now = Time.now();
	_lastMillis = millis();
	return _now;
}
void Sensor::setTime(unsigned long time) {
	Time.setTime(time);
	_now = time;
}*/

void Sensor::begin()
{
	pinMode(WAKEUP_Pin, INPUT);
	initWakeupPinStatus();

	pinMode(_barCSPin, OUTPUT);
	pinMode(_sdCSPin, OUTPUT);
	pinMode(_ucCSPin, OUTPUT);

	pinMode(PowerConnectedPin, INPUT);

	// Bluetooth System Key
	pinMode(BTConnectedPin, INPUT);	//For this to work we need to add a pulldown resistor
	pinMode(BLE_KEY_PIN, OUTPUT);
	digitalWrite(BLE_KEY_PIN, HIGH);

	//selectSPI(SDCARD);
	// SPI.setDataMode(SPI_MODE0);
	// SPI.setClockDivider(SPI_FULL_SPEED);
	// SPI.setBitOrder(MSBFIRST);

	SPI.begin();
	sdcard_Init();
	barometer_Init();
	afe.begin();
	if(init) {
		EEPROM.get(0, SensorConfig);
		SensorConfig.AFESerial[9]=0;
		SensorConfig.MACAddress[19]=0;
	}
}
double Sensor::getPowerLevel()
{
	int8_t val = PM.getFuelLevel();
	LOG_TRACE("Fuel level %d", val)
	return val; // getInVoltage();
}

void Sensor::storeEEPROMConfig() {
	EEPROM.put(0, SensorConfig);
}
SensorEEPROMConfig_t* Sensor::getConfig() {
	return &SensorConfig;
}
bool Sensor::isWiFiEnabled() {
	//We disable WiFi when sleep is enabled
	return SensorConfig.wifiEnabled && !isSleepEnabled() && !temporarlyDisableSleep;
}
bool Sensor::isSDAvail() {
	//TODO Fix the code to manage SD card errors and detect when the card is missing
	return SDAvail;
}
void Sensor::initWakeupPinStatus()
{
	wakeupPinStatus = (HIGH == digitalRead(WAKEUP_Pin));
	//LOG_TRACE("wakeupPinStatus %d\r\n", wakeupPinStatus);
}
void Sensor::checkWakeupPinStatus()
{
	if (!wakeupPinStatus)
		wakeupPinStatus = (HIGH == digitalRead(WAKEUP_Pin));
}

bool Sensor::isSleepEnabledByConfig() {
	// Sleep is enable only if the sleepEnabled variable in retained memory is true
	// And if the wakeup pin is low (so that going up will wakeup the cpu)
	// And if the USB serial is not connecte (to avoid continuous connection/disconnections)
	bool wkupLow = !wakeupPinStatus;
	bool noUsb = !isUSBChargerConnected();
	bool sleepEnabled = getConfig()->sleepEnabled;
	//LOG_TRACE("Sleep configuration: wkupLow %d, noUsb %d, sleepEnabled %d\r\n", wkupLow, noUsb, sleepEnabled);
	return sleepEnabled && wkupLow && noUsb;
}
bool Sensor::isSleepEnabled() {
	return isSleepEnabledByConfig() && !temporarlyDisableSleep;
}
bool Sensor::isUSBSerialConnected() {
	//Available for Write happens to be -1 when the USB serial is disconnected
	int aw = Serial.availableForWrite();
	return aw>=0;
	//Not quick enough to relibly stop the system from sleeping lets ignore for now.
	//return false;
}
double Sensor::getInVoltage() {
	//Available for Write happens to be -1 when the USB serial is disconnected
	int val = analogRead(PowerConnectedPin);
	double voltage= (((double)val*(3.3/4095.0))/2.24)*5.0;
	return val;
	//Not quick enough to relibly stop the system from sleeping lets ignore for now.
	//return false;
}
bool Sensor::isUSBChargerConnected() {
	//Available for Write happens to be -1 when the USB serial is disconnected
	return PM.isChargingOrTrickling(); // getInVoltage()>2500;
	//Not quick enough to relibly stop the system from sleeping lets ignore for now.
	//return false;
}
boolean Sensor::isBluetoothSerialConnected() {
	//To discover if bluetooth is connected we make sure to configure the module
	// AT+PIO11
	// When connected BTConnectedPin is HIGH when unconnected is LOW
	return digitalRead(BTConnectedPin) == HIGH;
}

Sensor::Reading_Raw_t* Sensor::readAll()
{

	/*unsigned long startSampleTime;
  unsigned long endSampleTime;
	startSampleTime = millis();*/

	//Initializing the humidity seonsor asking for a read of tempreature
	//While waiting for the measure we will get data from the barometric sensor.

	//Modified to enable sleeping instead of active waiting for reading results
	sht1x.sleep_enabled = isSleepEnabled();
	sht1x.beginReadTemperatureC();

	//----- Begin Barometric sensor reading -----
	int16_t temp;
	uint16_t baro;

	uint16_t d1, d2;
	int16_t off;
	int32_t dt, sens, x;

	selectSPI(BAR);

	//We try to reset to solve some erroneous readings
	barometer_reset();

	//TODO Double check this
	//	d1 = getD(1);            // to avoid problems with SD

	d1 = getD(1);            // read pressure value
	d2 = getD(2);            // read temperature value

	selectSPI(NONE);

	dt = (int32_t)d2 - ut1;  // dut=0 for T=20 degrC
							  // can be positive or neg
	temp = 200 + (int16_t)(dt * c6plus50 / 1024);

	off = c2mul4 + (int16_t)((int32_t)c4minus512 * dt / 4096);
	sens = c1 + (c3 * dt) / 1024 + 24576;
	x = (sens * (d1 - 7168)) / 16384 - off;

	baro = (uint16_t)(x * 10 / 32 + 2500);   // pressure in 0.1 mbar

	lastReading.bar_T = temp; // / 10.0;
	lastReading.bar_P = baro; // / 10.0;
	//----- End Barometric sensor reading -----

	// Most of the wait was used up while reading the barometric sensor.
	checkWakeupPinStatus();
	float hum_T = sht1x.endReadTemperatureC();
	lastReading.hum_T = hum_T * 100;

	checkWakeupPinStatus();
	sht1x.beginReadHumidity();
	//While waiting for the humidity measurement to be ready we read the
	//afe to save time
	afe.readAll();
	checkWakeupPinStatus();
	float hum_H = sht1x.endReadHumidity(hum_T);
	lastReading.hum_H = hum_H * 100;
	checkWakeupPinStatus();
	/*endSampleTime = millis();
	M_TRACE("Sampling Duration BAR- HUM %dms", endSampleTime-startSampleTime);
	*/

	return &lastReading;
}

void Sensor::barometer_reset(void)
{
	//SPI.beginTransaction(SPISettings(20000000, MSBFIRST, SPI_MODE0));
	SPI.setDataMode(SPI_MODE0);

	SPI.transfer(0x15);
	SPI.transfer(0x55);
	SPI.transfer(0x40);
	//SPI.endTransaction();
}

void Sensor::barometer_Init(void)
{
	uint16_t w1, w2, w3, w4;

	selectSPI(BAR);

	barometer_reset();

	w1 = getW(1);
	w2 = getW(2);
	w3 = getW(3);
	w4 = getW(4);

	c1 = convertWtoC(1, w1, w2, w3, w4);
	c2 = convertWtoC(2, w1, w2, w3, w4);
	c3 = convertWtoC(3, w1, w2, w3, w4);
	c4 = convertWtoC(4, w1, w2, w3, w4);
	c5 = convertWtoC(5, w1, w2, w3, w4);
	c6 = convertWtoC(6, w1, w2, w3, w4);

	c1plus24576 = c1 + 24576;
	c2mul4 = c2 * 4;
	c4minus512 = (int16_t)c4 - 512;
	c5mul8 = c5 * 8;
	c6plus50 = c6 + 50;

	ut1 = c5mul8 + 20224;

	selectSPI(NONE);
}

void Sensor::sdcard_Init(void)
{
	selectSPI(SDCARD);
	M_LOG("Accessing SD card");
	SDAvail = SD.begin(SDCSPin);

	if (isSDAvail()){
		M_LOG("SD Found");
		logFile = SD.open(filename, FILE_WRITE);
		M_LOG("SD Log File Opened");
	}
	selectSPI(NONE);
}

uint16_t Sensor::convertWtoC(uint8_t index, uint16_t W1, uint16_t W2, uint16_t W3, uint16_t W4) {
	uint16_t c;
	uint16_t x, y;

	c = 0;
	switch (index)
	{
	case 1:
		c = (W1 >> 1) & 0x7FFF;
		break;
	case 2:
		x = (W3 << 6) & 0x0FC0;
		y = W4 & 0x003F;
		c = x | y;
		break;
	case 3:
		c = (W4 >> 6) & 0x03FF;
		break;
	case 4:
		c = (W3 >> 6) & 0x03FF;
		break;
	case 5:
		x = (W1 << 10) & 0x0400;
		y = (W2 >> 6) & 0x03FF;
		c = x | y;
		break;
	case 6:
		c = W2 & 0x003F;
		break;
	}
	return c;
}

uint16_t Sensor::getW(uint8_t index) {
	uint16_t data = 0;

	switch (index) {

	case 1:
		data = get16(W, 0x1D, 0x50);
		break;

	case 2:
		data = get16(W, 0x1D, 0x60);
		break;

	case 3:
		data = get16(W, 0x1D, 0x90);
		break;

	case 4:
		data = get16(W, 0x1D, 0xA0);
		break;

	default:
		break;
	}

	return data;
}

uint16_t Sensor::getD(uint8_t index) {
	uint16_t data = 0;

	switch (index) {

	case 1:
		data = get16(D, 0x0F, 0x40);
		break;

	case 2:
		data = get16(D, 0x0F, 0x20);
		break;

	default:
		break;
	}

	return data;
}

uint16_t Sensor::get16(uint8_t type, uint8_t cmd1, uint8_t cmd2) {
	uint8_t data8 = 0;
	uint8_t dummy = 0;
	uint16_t data16 = 0;

	SPI.setDataMode(SPI_MODE0);
	//TODO just in case check needed to avoid problems with SPI and SD
	delay(1);
	SPI.transfer(cmd1);
	SPI.transfer(cmd2);

	SPI.setDataMode(SPI_MODE1);
	if (type == D) {
		delay(100);
	}
	else if (type == W) {
		delay(1);
		// This delay is not mentioned in MS5534 Datasheet.
		// However, reading W values has problem without this delay
		// when UART1 port is used for SPI communication.
	}

	data8 = SPI.transfer(dummy);
	data16 = data8 << 8;
	data8 = SPI.transfer(dummy);
	data16 += data8;

	SPI.setDataMode(SPI_MODE0);

	return data16;
}

// in general, SPI is active low: select one device at a time
// except BAR is "selected" by powering on (enabling the mclock)
void Sensor::selectSPI(spiDevice spiID) {
	switch (spiID) {
	case SDCARD:
		digitalWrite(_barCSPin, HIGH);  // disable
		digitalWrite(_sdCSPin, LOW);   // select
		digitalWrite(_ucCSPin, HIGH);  // de-select
		break;
	case BAR:
		digitalWrite(_barCSPin, LOW); // enable
		digitalWrite(_sdCSPin, HIGH);  // de-select
		digitalWrite(_ucCSPin, HIGH);  // de-select
		break;
	case UC_SPI:
		digitalWrite(_barCSPin, HIGH); // disable
		digitalWrite(_sdCSPin, HIGH); // de-select
		digitalWrite(_ucCSPin, LOW);  // select
		break;
	default:
		digitalWrite(_barCSPin, HIGH); // disable
		digitalWrite(_sdCSPin, HIGH); // de-select
		digitalWrite(_ucCSPin, HIGH); // de-select
	}
	M_TRACE("_barCSPin=%d | %d, _sdCSPin=%d | %d, _ucCSPin=%d | %d",_barCSPin, digitalRead(_barCSPin), _sdCSPin, digitalRead(_sdCSPin), _ucCSPin, digitalRead(_ucCSPin));
}
