#ifndef MAIN_H
#define MAIN_H

#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/power.h>

//#define DEBUG 0

// Functions prototypes
void buzz();
void setupTimer1();
void goToSleep();
void startWakeToneSequence();

// ADC pin mapping
const uint8_t vbatadc = 18;          // Read battery voltage
const uint8_t vpvadc = 20;           // Read PV voltage
const uint8_t ipvadc = 19;           // Read PV current (ZXCT1110)
const uint8_t analogtempadc = 23;    // Read analog temperature (LMT87)

// Digital pin Mapping
const uint8_t wakePin = 2;              // Activated on FALLING -- Touch GPIO State             -- 1- Touch 0- No Touch
const uint8_t coinPin = 3;              // Activated on FALLING -- Anti Pin Coin Presence       -- 1- Coin  0- No Coin
const uint8_t sda = 4;                  // I2C SDA Pin
const uint8_t scl = 5;                  // I2C SCL Pin
const uint8_t chgStatusPin = 6;         // Charger status LED
const uint8_t doorStatusPin = 7;        // Activated on FALLING -- Main and Safe Door State     -- 1- Close 0- Open
const uint8_t pmic_stby_req = 8;        // CPU PMIC_STBY_REQ used to check if tablet is awake   -- 1- STBY  0- Awake
const uint8_t chgErrorPin = 9;          // Charger error LED
const uint8_t coinWakeSignalPin = 10;   // Coin Wake Signal (connected to Tablet GPIO)          -- NOT USED YET
const uint8_t pinToneOut = 11;          // Buzzer interface
const uint8_t pinPulseOut = 12;         // HW Button
const uint8_t extGpio = 13;             // TO BE USED (connected to PIN 4 on flat cable)
const uint8_t isAwake = 21;             // OUT0 from Tablet                                     -- 1- Awake 0- Asleep
const uint8_t extAdc = 22;              // TO BE USED (not connected)

//Device config struct
struct DeviceConfig {
  int highbatth = 12600;
  int lowbatth = 12200;
  int buzzerOnperiod = 50;
  int buzzerOffperiod = 2500;
  int vpvfactor = 49;
  int ipvfactor = 49;
  int vbatfactor = 152;
  int fw = 0;
  int boot_status = 0;
  int set_mode = 1;
};

extern DeviceConfig config;

#endif