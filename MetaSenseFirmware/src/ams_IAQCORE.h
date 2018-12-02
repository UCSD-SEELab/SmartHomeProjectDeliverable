// ams_IAQCORE.h
//
// Provides an interface for the ams iAQ-CORE C and P products. The iAQ-CORE is
// MOS-based VOC sensor with an I2C interface.
//

#ifndef _AMS_IAQCORE_h
#define _AMS_IAQCORE_h

#include "application.h"

#define IAQ_STATUS_OK     0x00
#define IAQ_STATUS_WARMUP 0x10
#define IAQ_STATUS_BUSY   0x01
#define IAQ_STATUS_ERROR  0x80

#define IAQ_ADDRESS   0x5A
#define IAQ_READCMD   0x01

typedef struct {
  uint16_t co2_ppm;
  uint8_t  status;
  int32_t resistance;
  uint16_t tvoc_ppb;
} data_IAQ_t;

class ams_IAQCORE {
  public:
    ams_IAQCORE();

    void begin();
    uint8_t readData();
    uint16_t getCO2();
    uint8_t getStatus();
    uint16_t getTVOC();
    int32_t getResistance();
  private:
    data_IAQ_t prevReading;
};


#endif
