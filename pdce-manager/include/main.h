#ifndef MAIN_H
#define MAIN_H

//#define DEBUG 0

// Functions prototypes
void buzz();
void checkBattery();
/*void wakeUpNow();
void wakeUpCoin();
void wakeUpDoor();
void timer_callback();
void resumeUSB();
void suspendUSB();
void startupCheck();
*/
void setupTimer1();
void goToSleep();

const int isawake = 21;
//int buzzerflag = 0;

// External /interrupts mapping
/*const int wakepin = 2;          // Activated on FALLING -- Touch GPIO State        -- 1- Touch 0- No Touch
const int coinpin = 3;          // Activated on FALLING -- Anti Pin Coin Presence  -- 1- Coin  0- No Coin
const int safedoorpin = 7;      // Activated on FALLING -- Safe Door State         -- 1- Close 0- Open
*/
const int batterycontrol = 10;  // Prevents battery from discharging below recommended levels       
const int spare1 = 12;   //HW_Button 
const int spare2 = 13;   // TO BE USED (1-Wire Protocol for RGB Led)


#endif