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

#include "PowerManager.h"

PowerManager::PowerManager(void)
{
	// Set default values
	minInterval					= 1;
	SOCAlarmThreshold 	= 5;
	batteryState 				= UNKNOWN;
	capacitySpec 				= 1800;
	capacity						= capacitySpec;
}

void PowerManager::begin(PowerEEPROMState_t* inPS){
	PM_TRACE("----PowerManager::begin------\n");
	PM_TRACE("Before PM begin, saved isAvail was: %d and saved isCal: %d\n",
			inPS->isAvail, inPS->isCal);

	bool wasAvail = inPS->isAvail; // was fuel gauge running in a previous life?

	PS = inPS;

	if (Wire.isEnabled())  {
		Wire.end();
	}
	Wire.setSpeed(CLOCK_SPEED_100KHZ);
	Wire.begin();


	// This checks I2C response but doesn't reset anything... I think.
	PS->isAvail = fuelGauge.init(R_cg);
	PM_TRACE("PM init RETURNED: %d\n", PS->isAvail);
	// If gauge is not available now, there is nothing to do
	if (!PS->isAvail){
		voltage 	= -1;
		current 	= -1;
		PS->charge	= -1;
		_ccounter 	= -1;
		baseSoC 	= -1;
		batteryState = UNKNOWN;
		PS->soc 	= -1;
		PS->lastCal = 0;
		PS->lastInit= 0;
		PS->isCal 	= false;

		return;
	}

	if (wasAvail){
		PM_TRACE("capacity: %d\n", capacity);
		PM_TRACE("Saved charge: %d\n", PS->charge);
		PM_TRACE("Set base SOC: %d %d %d\n",
			(capacity + PS->charge),
			(capacity + PS->charge) * 100 ,
			(capacity + PS->charge) * 100 /capacity);
		//fuelGauge.setBaseSOC((capacity + PS->charge)*100/capacity);
		fuelGauge.setBaseSOC(0);
	}else{
		reset();
	}

	// some incongruency in the coulomb counter (maybe there was a reset)
	// dscount integer overflow because that's going to happen too
	_ccounter = fuelGauge.readCounter();
	if ( (PS->ccounter > _ccounter) && !(PS->ccounter>0 && _ccounter <0) ){
		PM_TRACE("----Noticed a reset!\n");
		PM_TRACE("Saved cc: %d\n", PS->ccounter);
		PM_TRACE(" Current cc: %d\n", _ccounter);
		PM_TRACE("Uncalibrating.\n");
		PS->isCal = false;
		PS->lastCal = 0;
	}

	PS->lastInit = Time.now();
	fuelGauge.enableAlarm();

	PM_TRACE("After PM begin: PS isAvail: %d and new isCal: %d\n",
		PS->isAvail, inPS->isCal);
}


void PowerManager::reset(){
	PM_TRACE("PM:reset\n");
	fuelGauge.resetAccumulator();
	fuelGauge.enableOperation();
	fuelGauge.setBaseSOC(0);
	fuelGauge.clearSOCAlarm();
	fuelGauge.clearVoltageAlarm();
	PS->lastInit = Time.now();
	PS->lastCal = 0;
	PS->isCal = false;
}

void PowerManager::printPowerReport(void){
	//Serial1.println("PM:power report");
	//updateReadings();
	char tmpStr[30];
	Time.timeStr().toCharArray(tmpStr,30);
	PM_TRACE("POWER_REPORT: %s ", tmpStr);
	PM_TRACE(", I_mA: %4.2f", current);
	PM_TRACE(", V_V: %4.2f", voltage/1000);
	PM_TRACE(", BaseSOC: %d", baseSoC);

	PM_TRACE(", CC: %d", PS->ccounter);

	PM_TRACE(", RawAcc_uVh: %4.2f", rawAcc);

	PM_TRACE(", charge_mAh: %4.2f", PS->charge);

	PM_TRACE(", SOC: %d", PS->soc);

	PM_TRACE(", STAT: %d", batteryState);

	PM_TRACE(", ALMLevel: %d", SOCAlarmThreshold);

	uint16_t ctrlReg = fuelGauge.readCTRL();

	PM_TRACE(", ALM_SOC: %d", ctrlReg & STC_3105_ALM_SOC);

	PM_TRACE(", ALM_MODE: %d", ctrlReg & STC_3105_IO0DATA);

	//Serial1.print(", PORDET, ");
	//Serial1.print(String(ctrlReg & STC_3105_PORDET));

	PM_TRACE(", Canary: %d", PS->canary);
	(PS->canary)++;

	switch(batteryState){
			case CHARGING:
				PM_TRACE(", batteryState: CHARGING");
				break;
			case DISCHARGING:
				PM_TRACE(", batteryState: DISCHARGING");
				break;
			case TRICKLE:
				PM_TRACE(", batteryState: TRICKLE");
				break;
			case UNKNOWN:
				PM_TRACE(", batteryState: UNKNOWN");
				break;
	}


	PM_TRACE(", isCal: %d", PS->isCal);

	PM_TRACE(", isCal: %d", PS->isCal);

	String x = Time.format(PS->lastCal, "%a %h %e %H:%M:%S %Y");
	x.toCharArray(tmpStr,30);
  PM_TRACE(", lastCal: %s", tmpStr);

	PM_TRACE("\n");
}

// calibrate fuel gauge WHEN FULL to get accurate SoC readings
void PowerManager::calibrateFuelGauge(void){
	PM_TRACE("---------Calibrating fuel gauge--------\n");

	// fuelGauge.setBaseSOC(1800*1000*R_cg);
	PM_TRACE("Setting SOC to 100%\n");
	fuelGauge.setBaseSOC(100);
	PM_TRACE("Resetting accumulator\n");
	fuelGauge.resetAccumulator();

	capacity = capacitySpec; // FIXME
	soh = ( (capacity / capacitySpec) -SoHThreshold) * 100 /(1 - SoHThreshold);

	PM_TRACE("Setting calibration time\n");
	PS->lastCal = Time.now();
	PM_TRACE("Setting isCal to true\n");
	PS->isCal = true;
}

void PowerManager::setBatteryCapacity(int8_t mAh){
	capacitySpec = mAh;
}

bool PowerManager::isAvail(){
	return PS->isAvail;
}

bool PowerManager::isCalibrated(){
	return PS->isCal;
}

float PowerManager::getBatteryVoltage(){
	updateReadings();
	return voltage;
}

bool PowerManager::isCharging(){
	// charging: +300/400mA
	// done charging but plugged in: -1 to -3mA
	// discharging: -50mA (for bare code, probably higher magnitude for actual)
	//updateReadings();
	return (batteryState == CHARGING);
}
bool PowerManager::isChargingOrTrickling(){
	// charging: +300/400mA
	// done charging but plugged in: -1 to -3mA
	// discharging: -50mA (for bare code, probably higher magnitude for actual)
	//updateReadings();
	return (batteryState == TRICKLE || batteryState == CHARGING);
}

int8_t PowerManager::getFuelLevel(){
	//updateReadings();
	return PS->soc;
}

bool PowerManager::isBatteryLow(){
	//updateReadings();
	bool valid = PS->isAvail && PS->isCal;
//TODO: Make sure this return false if the battery is charging
	return (valid && (PS->soc < SOCAlarmThreshold));
}

int8_t PowerManager::setLowBatteryThreshold(int8_t threshold){
	int8_t newAlarm = SOCAlarmThreshold;
	PM_TRACE("Requested alarm level: %d\n", threshold);

	if ( threshold>=0 && threshold<=100 ){
		SOCAlarmThreshold = threshold;
		PM_TRACE("Set new threshold\n");
		newAlarm = (int8_t) fuelGauge.setSOCAlarm(SOCAlarmThreshold);
	}

	fuelGauge.enableAlarm();
	return newAlarm;
}

void PowerManager::updateReadings(){
	// Limit the number of I2C requests you send to get new power numbers
	if (Time.now()>lastUpdate && //Let's make sure that the clock was not reset
		 (Time.now() - lastUpdate) < minInterval ) {
		return;
	}

	// For compatability with boards missing STC3105 chip
	if ( !(PS->isAvail) ){
		PM_TRACE("STC3105 not available\n");
		voltage 	= -1;
		current 	= -1;
		PS->charge	= -1;
		PS->ccounter = -1;
		baseSoC 	= -1;
		PS->soc 	= -1;
		batteryState = UNKNOWN;
		PS->lastCal 	= 0;
		PS->lastInit 	= 0;
		PS->isCal 	= false;

		return;
	}

	voltage 	= fuelGauge.readVoltage();
	current 	= fuelGauge.readCurrent();
	// readCharge returns value in uVh. [uVh]/R_cg/1000 = [mAh]
	rawAcc		= fuelGauge.readCharge();
	PS->charge 	= rawAcc/R_cg/1000;
	_ccounter 	= fuelGauge.readCounter();
	baseSoC 	= fuelGauge.readBaseSOC();

	PS->ccounter = _ccounter;

	if (PS->isCal){
		PS->soc = (capacity + PS->charge) / capacity * 100;

		if (PS->soc < 0){
			PS->soc = 0;
		}else if (PS->soc > 100){
			PS->soc = 100;
		}
	}else{
		PS->soc = -1;
	}

	// intermittent bug at brownout?: if getting 0 readings, reinitialize the fuel gauge
	if (current==0.00 && voltage== 0.00){
		PM_TRACE("Fuel gauge giving 0 readings, reinitialize...\n");
		PS->isAvail = fuelGauge.init(R_cg);

		if (PS->isAvail){
			reset();
		}
	}

	state_t prevBatteryState = batteryState;

	batteryState = DISCHARGING;
	if ( current > 0 ){
		batteryState = CHARGING;
	} else if ( current > trickleCurrent) {
		batteryState = TRICKLE;
	}

	// reached a full charge!
	//if ( (prevBatteryState == CHARGING && batteryState == TRICKLE) || (batteryState == TRICKLE && !(PS->isCal)) ) {
	if (prevBatteryState == CHARGING && batteryState == TRICKLE) {
		PM_TRACE("Charging -> Full charge!\n");
	}
	if ( batteryState == TRICKLE && !(PS->isCal) )  {
		PM_TRACE("Trickling and uncalibrated\n");
		calibrateFuelGauge();
	}

	// alarm-on is auto, alarm-off is manual
	// FIXME do i really want to read this every time? seems heavy...
	uint16_t ctrlReg = fuelGauge.readCTRL();

	if ( (PS->soc > SOCAlarmThreshold) && (ctrlReg & STC_3105_ALM_SOC) ){
		PM_TRACE("Turning off the SOC alarm\n");
		fuelGauge.clearSOCAlarm();
	}

	// not sure how alarm gets disabled but sometimes it happens
	if ( !(ctrlReg & STC_3105_IO0DATA) ){
		PM_TRACE("Reenable alarm\n");
		fuelGauge.enableAlarm();
	}

	lastUpdate = Time.now();
}
