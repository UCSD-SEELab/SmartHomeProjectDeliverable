// Reference sources:
//	https://github.com/ahmadexp/STC3105-Arduino
// 	https://github.com/egorf/STC3105_driver
// Edited by CSC

#ifndef STC3105_h
#define STC3105_h

#include "application.h"

//Control Registers
#define STC_3105_REG_MODE           	  0x00
#define STC_3105_REG_CTRL           	  0x01
#define STC_3105_REG_CHARGE_LOW       	0x02
#define STC_3105_REG_CHARGE_HIGH       	0x03
#define STC_3105_REG_COUNTER_LOW      	0x04
#define STC_3105_REG_COUNTER_HIGH      	0x05
#define STC_3105_REG_CURRENT_LOW      	0x06
#define STC_3105_REG_CURRENT_HIGH      	0x07
#define STC_3105_REG_VOLTAGE_LOW      	0x08
#define STC_3105_REG_VOLTAGE_HIGH      	0x09
#define STC_3105_REG_SOC_BASE_LOW     	0x0A
#define STC_3105_REG_SOC_BASE_HIGH     	0x0B
#define STC_3105_REG_ALARM_SOC_LOW    	0x0C
#define STC_3105_REG_ALARM_SOC_HIGH    	0x0D
#define STC_3105_REG_ALARM_VOLTAGE     	0x0E
#define STC_3105_CURRENT_THRES  		    0x0F
#define STC_3105_RELAX_COUNT    		    0x10

//Device ID Registers
#define STC_3105_REG_ID         		    0x18
#define STC_3105_REG_ID_VALUE           0x12

//REG_Mode masks
#define STC_3105_PWR_SAVE               0x04
#define STC_3105_ALM_ENA                0x08
#define STC_3105_GG_RUN                 0x10

//REG_Ctrl masks
#define STC_3105_IO0DATA            	  0x01
#define STC_3105_GG_RST                 0x02
#define STC_3105_GG_EOC                 0x04
#define STC_3105_VM_EOC                 0x08
#define STC_3105_PORDET                 0x10
#define STC_3105_ALM_SOC                0x20
#define STC_3105_ALM_VOLT               0x40

//Resolutions
#define VOLTAGE_RESOLUTION              2.44 			// Voltage range is 0-5V, LSB = 2.44 mV
// #define CURRENT_RESOLUTION              0.01177/0.01
#define CURRENT_RESOLUTION              11.77			// Current sensing 14-bits in 2's comp, LSB = 11.77 uV
#define CHARGE_RESOLUTION               6.70			// LSB = Cur_res*2^12*0.5/3600 = 6.7 uVh

class STC3105
{
  public:
    STC3105(void);
    uint8_t init(float R_cg);
    void    enableOperation(void);

    void 	  writeReg(uint8_t, uint8_t);
    uint8_t readReg(uint8_t);
    void 	  writeReg16(uint8_t, int16_t);
    int16_t readReg16(uint8_t);

    float    readCharge(void);
    uint16_t readCounter(void);
    uint16_t readBaseSOC(void);
    float    readCurrent(void);
    float    readVoltage(void);
    void     resetAccumulator(void);
    void     POR(uint8_t);
	  uint16_t readCTRL(void);

	  void 	   enableAlarm();
	  uint16_t setBaseSOC(uint16_t);
	  uint16_t setSOCAlarm(uint16_t);
	  void 	   clearSOCAlarm();
    void 	   clearVoltageAlarm();

  private:
    uint8_t  address;
	float	 resistor_cg;
    uint8_t  testWhoAmI(void);
};

#endif
