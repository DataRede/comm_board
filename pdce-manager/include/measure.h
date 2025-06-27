#ifndef MEASURE_H
#define MEASURE_H

#include "main.h"
#include "Arduino.h"

// External variables
extern DeviceConfig config;
extern int vbat;
extern int vpv;
extern int ipv;

// Constants
const float VBAT_SCALE = 0.1;
const float VPV_SCALE = 0.1;
const float IPV_SCALE = 0.1;
const float FILTER_ALPHA = 0.2;
const float TEMP_SCALE = 36.28;
const int TEMP_OFFSET = 19292;
const int VPV_OFFSET = 2750;
const int VPV_MULTIPLIER = 10;

struct SensorReadings {
    int vbat;
    int vpv;
    int ipv;
    int temperature;
};

struct GPIOStatus {
    int wake;
    int coin;
    int door;
    int pmicStbyReq;
    int isAwake;
};

class Measure {
public:
    SensorReadings measureSensors();
    GPIOStatus measureGPIOs();

private:
    int VBAT();
    int VPV();
    int IPV();
    int temperature();

    int vbatsense, vpvsense, ipvsense, itemp, pdce_temp;
    float vbat_new, vpv_new, ipv_new;
    float vbat_ant = 13000, vpv_ant = 2750, ipv_ant = 0;
};

#endif
