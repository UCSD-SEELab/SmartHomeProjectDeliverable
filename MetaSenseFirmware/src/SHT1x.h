/**
 * SHT1x Library
 *
 * Copyright 2009 Jonathan Oxer <jon@oxer.com.au> / <www.practicalarduino.com>
 * Based on previous work by:
 *    Maurice Ribble: <www.glacialwanderer.com/hobbyrobotics/?p=5>
 *    Wayne ?: <ragingreality.blogspot.com/2008/01/ardunio-and-sht15.html>
 *
 * Manages communication with SHT1x series (SHT10, SHT11, SHT15)
 * temperature / humidity sensors from Sensirion (www.sensirion.com).
 *
 * Ported to Spark Core by Anurag Chugh (https://github.com/lithiumhead) on 2014-10-15
 */
#ifndef SHT1x_h
#define SHT1x_h

#include "application.h"
#include "logger.h"


class SHT1x
{
    public:
      bool sleep_enabled;
      SHT1x(byte dataPin, byte clockPin);
      void beginReadHumidity();
  		void beginReadTemperatureC();
      float endReadHumidity(float tempC);
  		float endReadTemperatureC();
      float readHumidity(float tempC);
  		float readTemperatureC();
    private:
      byte _dataPin;
  		byte _clockPin;
  		byte _numBits;
      void startReadTemperatureRaw();
      void startReadHumidityRaw();
      int endReadTemperatureRaw();
      int endReadHumidityRaw();
  		int readTemperatureRaw();
  		int readHumidityRaw();
  		int shiftIn(byte _dataPin, byte _clockPin, byte _numBits);
      void sendCommandSHT(int _command, byte _dataPin, byte _clockPin);
      void waitForResultSHT(byte _dataPin);
      int getData16SHT(byte _dataPin, byte _clockPin);
      void skipCrcSHT(byte _dataPin, byte _clockPin);
};

#endif
