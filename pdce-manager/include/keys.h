#ifndef KEYS_H
#define KEYS_H
#endif

//#include "Arduino.h"
//include "Keyboard.h"

extern int buzzerbuttonflag;
extern int wakeupflag;
extern unsigned long past_startup_time;
extern int set_mode;

/* Send keys functions */
void sendPwrKey();
void sendCoinKey();
void sendRestartKey();

