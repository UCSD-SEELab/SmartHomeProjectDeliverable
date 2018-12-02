/**
 * SHT1x Library
 *
 * Copyright 2009 Jonathan Oxer <jon@oxer.com.au> / <www.practicalarduino.com>
 * Based on previous work by:
 *    Maurice Ribble: <www.glacialwanderer.com/hobbyrobotics/?p=5>
 *    Wayne ?: <ragingreality.blogspot.com/2008/01/ardunio-and-sht15.html>
 *
 * Manages communication with SHT1x series (SHT10, SHT11, SHT15)
 * temperature / humidity sensors from Sensirion (www.sensirion.com).'
 *
 * Ported to Spark Core by Anurag Chugh (https://github.com/lithiumhead) on 2014-10-15
 */

#include "SHT1x.h"

SHT1x::SHT1x(byte dataPin, byte clockPin)
{
    _dataPin = dataPin;
    _clockPin = clockPin;
    sleep_enabled = false;
}


/* ================  Public methods ================ */

//Split the methods in 2 so that we may do something during the wait time
void SHT1x::beginReadHumidity() {
  startReadHumidityRaw();
}
void SHT1x::beginReadTemperatureC() {
  startReadTemperatureRaw();
}
float SHT1x::endReadHumidity(float tempC)
{
    int _valRaw;                    // Raw humidity value returned from sensor

	_valRaw = endReadHumidityRaw();
	// CSC humidity correction
        const float _C1 = -2.0468;
        const float _C2 = 0.0367;
        const float _C3 = -1.5955E-6;
        const float T1 = 0.01;
        const float T2 = 0.00008;
        const float TAdj = 25.0;


		// Apply linear conversion to raw value
        float linearHumidity = _C1 + _C2 * _valRaw + _C3 * _valRaw * _valRaw;

    	// Correct humidity value for current temperature
        float _correctedHumidity = ( tempC - TAdj) * (T1 + T2 * _valRaw) + linearHumidity;

	// return _valRaw;
	return _correctedHumidity;
}
float SHT1x::endReadTemperatureC()
{
    int _val;                // Raw value returned from sensor
    float _temperature;      // Temperature derived from raw value

    // Conversion coefficients from SHT15 datasheet
    //const int Dat1 = -40.0;  // for 14 Bit @ 5V
    //const float Dat2 =   0.01; // for 14 Bit DEGC

    // Fetch raw value
    _val = endReadTemperatureRaw();

    // Convert raw value to degrees Celsius
	//_temperature = (_val * Dat2) + Dat1;
//	_temperature = _val -4000;


    const float Dat1 = -39.36;
    const float Dat2 = 0.01;
    _temperature = _val * Dat2 + Dat1;

	return (_temperature);
}


/**
 * Reads the current temperature in degrees Celsius
 */
float SHT1x::readTemperatureC()
{
    int _val;                // Raw value returned from sensor
    float _temperature;      // Temperature derived from raw value

    // Conversion coefficients from SHT15 datasheet
    //const int Dat1 = -40.0;  // for 14 Bit @ 5V
    //const float Dat2 =   0.01; // for 14 Bit DEGC

    // Fetch raw value
    _val = readTemperatureRaw();

    // Convert raw value to degrees Celsius
	//_temperature = (_val * Dat2) + Dat1;
//	_temperature = _val -4000;


    const float Dat1 = -39.36;
    const float Dat2 = 0.01;
    _temperature = _val * Dat2 + Dat1;

	return (_temperature);
}

/**
 * Reads current temperature-corrected relative humidity
 */
float SHT1x::readHumidity(float tempC)
{
    int _valRaw;                    // Raw humidity value returned from sensor

	_valRaw = readHumidityRaw();

	// Get current temperature for humidity correction
	//_temperatureRaw = readTemperatureC();

	//long long _val = ((long long)_valRaw) * 10000000;
	//long long _temperature = ((long long)_temperatureRaw) * 10000000;
	//long long _linearHumidity;       // Humidity with linear correction applied
	//long long  _correctedHumidity;    // Temperature-corrected humidity

	//const long long C1 = -40000000;       // for 12 Bit
	//const long long C2 = 405000;    // for 12 Bit
	//const long long C3 = -28; // for 12 Bit
	//const long long T1 = 100000;      // for 14 Bit @ 5V
	//const long long T2 = 800;   // for 14 Bit @ 5V
	//const long long TAdj = -250000000;   // for 14 Bit @ 5V

 //   // Conversion coefficients from SHT15 datasheet
 //   const float C1 = -4.0;       // for 12 Bit
 //   const float C2 =  0.0405;    // for 12 Bit
 //   const float C3 = -0.0000028; // for 12 Bit
 //   const float T1 =  0.01;      // for 14 Bit @ 5V
 //   const float T2 =  0.00008;   // for 14 Bit @ 5V

//	long _result = _correctedHumidity / 100000;

//    return (_result);

	// CSC humidity correction

        const float _C1 = -2.0468;
        const float _C2 = 0.0367;
        const float _C3 = -1.5955E-6;
        const float T1 = 0.01;
        const float T2 = 0.00008;
        const float TAdj = 25.0;

		// Apply linear conversion to raw value
        float linearHumidity = _C1 + _C2 * _valRaw + _C3 * _valRaw * _valRaw;

    	// Correct humidity value for current temperature
        float _correctedHumidity = ( tempC - TAdj) * (T1 + T2 * _valRaw) + linearHumidity;

	// return _valRaw;
	return _correctedHumidity;

}


/* ================  Private methods ================ */

void SHT1x::startReadTemperatureRaw()
{
    // Command to send to the SHT1x to request Temperature
    int _gTempCmd  = 0b00000011;
    sendCommandSHT(_gTempCmd, _dataPin, _clockPin);
}

void SHT1x::startReadHumidityRaw()
{
	// Command to send to the SHT1x to request humidity
	int _gHumidCmd = 0b00000101;
	// Fetch the value from the sensor
	sendCommandSHT(_gHumidCmd, _dataPin, _clockPin);
}

int SHT1x::endReadTemperatureRaw()
{
    int _val;
    waitForResultSHT(_dataPin);
    _val = getData16SHT(_dataPin, _clockPin);
    skipCrcSHT(_dataPin, _clockPin);
    return (_val);
}
int SHT1x::endReadHumidityRaw()
{
	int _val;
	waitForResultSHT(_dataPin);
	_val = getData16SHT(_dataPin, _clockPin);
	skipCrcSHT(_dataPin, _clockPin);
	return (_val);
}


/**
 * Reads the current raw temperature value
 */
int SHT1x::readTemperatureRaw()
{
    int _val;

    // Command to send to the SHT1x to request Temperature
    int _gTempCmd  = 0b00000011;

    sendCommandSHT(_gTempCmd, _dataPin, _clockPin);
    waitForResultSHT(_dataPin);
    _val = getData16SHT(_dataPin, _clockPin);
    skipCrcSHT(_dataPin, _clockPin);

    return (_val);
}

/**
* Reads the current raw humidity value
*/
int SHT1x::readHumidityRaw()
{
	int _val;

	// Command to send to the SHT1x to request humidity
	int _gHumidCmd = 0b00000101;

	// Fetch the value from the sensor
	sendCommandSHT(_gHumidCmd, _dataPin, _clockPin);
	waitForResultSHT(_dataPin);
	_val = getData16SHT(_dataPin, _clockPin);
	skipCrcSHT(_dataPin, _clockPin);

	return (_val);
}
/**
 */
int SHT1x::shiftIn(byte _dataPin, byte _clockPin, byte _numBits)
{
    int ret = 0;
    int i;
    for (i=0; i<_numBits; ++i)
    {
        digitalWrite(_clockPin, HIGH);
        delay(10);  // I don't know why I need this, but without it I don't get my 8 lsb of temp
        ret = ret*2 + digitalRead(_dataPin);
        digitalWrite(_clockPin, LOW);
    }
    return(ret);
}

/**
 */
void SHT1x::sendCommandSHT(int _command, byte _dataPin, byte _clockPin)
{
    int ack;

    // Transmission Start
    pinMode(_dataPin, OUTPUT);
    pinMode(_clockPin, OUTPUT);
    digitalWrite(_dataPin, HIGH);
    digitalWrite(_clockPin, HIGH);
    digitalWrite(_dataPin, LOW);
    digitalWrite(_clockPin, LOW);
    digitalWrite(_clockPin, HIGH);
    digitalWrite(_dataPin, HIGH);
    digitalWrite(_clockPin, LOW);

    // The command (3 msb are address and must be 000, and last 5 bits are command)
    shiftOut(_dataPin, _clockPin, MSBFIRST, _command);

    // Verify we get the correct ack
    digitalWrite(_clockPin, HIGH);
    pinMode(_dataPin, INPUT);
    ack = digitalRead(_dataPin);
    if (ack != LOW) {
      M_ERROR("Ack Error 0");
    }
    digitalWrite(_clockPin, LOW);
    ack = digitalRead(_dataPin);
    if (ack != HIGH) {
      M_ERROR("Ack Error 1");
    }
}

/**
 */
void SHT1x::waitForResultSHT(byte _dataPin)
{
    int i;
    int ack;
    pinMode(_dataPin, INPUT);
    //TODO Fix this sleep if needed
//    if (!sleep_enabled) {
      //REDUCED from 100 because the maximum delay is 320ms
    	//This improves with the large delay if the sensor is not working.
      for(i= 0; i < 35; ++i)
        {
            delay(10);
            ack = digitalRead(_dataPin);
            if (ack == LOW) {
                break;
            }
        }
      // timeout error
      if (ack == HIGH) {
        M_ERROR("Ack Error 2"); // Can't do serial stuff here, need another way of reporting errors
      }
    /*} else {
      //This is slower if the sensor is not present (timeout of 1 second but sleep during the delay
      System.sleep(_dataPin, FALLING, 1);
    }*/
}

/**
 */
int SHT1x::getData16SHT(byte _dataPin, byte _clockPin)
{
    int val;

    // Get the most significant bits
    pinMode(_dataPin, INPUT);
    pinMode(_clockPin, OUTPUT);
    val = shiftIn(_dataPin, _clockPin, 8);
    val *= 256;

    // Send the required ack
    pinMode(_dataPin, OUTPUT);
    digitalWrite(_dataPin, HIGH);
    digitalWrite(_dataPin, LOW);
    digitalWrite(_clockPin, HIGH);
    digitalWrite(_clockPin, LOW);

    // Get the least significant bits
    pinMode(_dataPin, INPUT);
    val |= shiftIn(_dataPin, _clockPin, 8);

    return val;
}

/**
 */
void SHT1x::skipCrcSHT(byte _dataPin, byte _clockPin)
{
    // Skip acknowledge to end trans (no CRC)
    pinMode(_dataPin, OUTPUT);
    pinMode(_clockPin, OUTPUT);

    digitalWrite(_dataPin, HIGH);
    digitalWrite(_clockPin, HIGH);
    digitalWrite(_clockPin, LOW);
}
