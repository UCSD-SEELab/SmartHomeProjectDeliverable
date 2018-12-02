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

#ifndef PowerManager_h
#define PowerManager_h

#include "logger.h"
#include "application.h"
#include "STC3105.h"

#define R_cg			0.02 	// 20 mOhm
#define trickleCurrent	-8.0 	// mA // FIXME: seen up to 7.06mA when CO2 attached
#define SoHThreshold	0.8 	// when full charge falls to 80% of spec, soh = 0

/*
When the battery voltage falls below the undervoltage lockout threshold of 2.7 V, the monitoring functions are stopped and the STC3105 stays in power-down mode with minimum quiescent current. All registers are maintained down to 2.0 V. Below 2.0 V, the registers are reset to their default power-on values.
*/

// different active states the battey can be in
typedef enum { UNKNOWN, DISCHARGING, CHARGING, TRICKLE } state_t;

typedef struct{
	bool 		isAvail;
	bool 		isCal;
	float 	charge;
	time_t 	lastInit;
	time_t 	lastCal;
	int8_t 	soc;
	int 		canary;
	int 		ccounter;
} PowerEEPROMState_t;


class PowerManager
{
  public:
    PowerManager(void);
		void 		begin(PowerEEPROMState_t* inPS);
		int8_t 	setLowBatteryThreshold(int8_t);
		void 		setBatteryCapacity(int8_t);

		void 		printPowerReport(void);
		float 	getBatteryVoltage();
		int8_t	getFuelLevel();
		int8_t	getBattSoH(); // NOT READY FIXME

		bool 		isAvail();
		bool 		isCalibrated();
		bool 		isBatteryLow(); // NOT READY FIXME
		bool 		isCharging();
		bool 		isChargingOrTrickling();
		//Moved to public. Need to call this periodiclly in the main loop.
		void 		updateReadings(void);

  private:
		PowerEEPROMState_t* PS;

		STC3105 fuelGauge;
		int			minInterval; // millisecond
		time_t	lastUpdate;
		void 		reset();
		void 		calibrateFuelGauge(void);
		bool 		gaugeAvail;
		bool 		gaugeCal;
		int8_t	SOCAlarmThreshold;
		state_t	batteryState;
		time_t	lastInit;
		time_t	lastCal;

		float		voltage;
		float		current;
		float		rawAcc;
		int			capacity;		// measured capacity in mAh
		int			capacitySpec; 	// stated capacity in mAh
		int			baseSoC;
		int			_ccounter;
		int8_t	soh;			// integer 0-100
		int8_t	_soc;			// integer 0-100
};

#endif
