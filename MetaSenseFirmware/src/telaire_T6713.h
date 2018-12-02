// telaire_T6713.h
//
// Provides an interface for the Telaire T6713 CO2 monitoring module over I2C
// using the MODBUS protocol.
//

#ifndef _TELAIRE_T6713_h
#define _TELAIRE_T6713_h

#include "application.h"

#define CO2_ADDRESS       0x15
#define CO2_CMD_READ      0x04
#define CO2_CMD_WRITE     0x05
#define CO2_CMD_WRITEADDRESS  0x06

#define CO2_REG_FIRMWAREREV   0x1389  // Firmware revision (read-only)
#define CO2_REG_STATUS    0x138A      // Status register (read-only)
#define CO2_REG_GAS_PPM   0x138B      // CO2 measurement in PPM (read-only)
#define CO2_REG_RESET     0x03E8      // Reset device (write-only)
#define CO2_REG_STARTCAL  0x03EC      // Start a single-point calibration  (write-only)
#define CO2_REG_I2CADDRESS    0x0FA5  // Change the slave address (read/write)
#define CO2_REG_ABCCTRL   0x03EE      // Enable or diable ABC logic (read/write)

#define CO2_STATUS_ERROR      0x0001
#define CO2_STATUS_ERROR_FLASH  0x0002
#define CO2_STATUS_ERROR_CAL  0x0004
#define CO2_STATUS_MODE_RS232 0x0100
#define CO2_STATUS_MODE_RS485 0x0200
#define CO2_STATUS_MODE_I2C   0x0400
#define CO2_STATUS_WARMUP     0x0800
#define CO2_STATUS_CAL        0x8000

typedef struct {
  uint16_t firmwareRev;
  uint16_t status;
  uint16_t co2_ppm;
} data_CO2_t;

class telaire_T6713 {
  public:
    telaire_T6713();

    void begin();
    uint8_t readAll();
    uint8_t readData();
    uint8_t readStatus();
    void reset();
    uint16_t getCO2();
    uint16_t getStatus();
    uint16_t getFirmwareRev();
  private:
    uint16_t readReg(uint16_t reg);
    inline void setError();
    data_CO2_t prevReading;
    uint8_t readFirmwareRev();
};

#endif
