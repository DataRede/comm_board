#include "measure.h"

SensorReadings Measure::measureSensors() {
    SensorReadings readings;
    readings.vbat = VBAT();
    readings.vpv = VPV();
    readings.ipv = IPV();
    readings.temperature = temperature();
    return readings;
}

GPIOStatus Measure::measureGPIOs() {
    GPIOStatus status;
    status.wake = digitalRead(wakePin);
    status.coin = digitalRead(coinPin);
    status.door = digitalRead(doorStatusPin);
    status.pmicStbyReq = digitalRead(pmic_stby_req);
    status.isAwake = digitalRead(isAwake);
    return status;
}

int Measure::VBAT() {
    vbatsense = analogRead(vbatadc);
    vbat_new = vbatsense * (config.vbatfactor * VBAT_SCALE);
    vbat = FILTER_ALPHA * vbat_ant + (1 - FILTER_ALPHA) * vbat_new;
    vbat_ant = vbat;
    return vbat;
}

int Measure::VPV() {
    vpvsense = analogRead(vpvadc);
    vpv_new = vpvsense * (config.vpvfactor * VPV_SCALE);
    vpv = FILTER_ALPHA * vpv_ant + (1 - FILTER_ALPHA) * vpv_new;
    vpv_ant = vpv;
    vpv = (vpv - VPV_OFFSET) * VPV_MULTIPLIER + vbat;
    return vpv;
}

int Measure::IPV() {
    ipvsense = analogRead(ipvadc);
    ipv_new = ipvsense * (config.ipvfactor * IPV_SCALE);
    ipv = FILTER_ALPHA * ipv_ant + (1 - FILTER_ALPHA) * ipv_new;
    ipv_ant = ipv;
    return ipv;
}

int Measure::temperature() {
    itemp = analogRead(analogtempadc);
    itemp = itemp * TEMP_SCALE;
    pdce_temp = TEMP_OFFSET - itemp;
    return pdce_temp;
}
