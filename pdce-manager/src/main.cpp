//********************************************//
//*************PDCE FIRMWARE v1.0.0***********//
//********************************************//
#include "main.h"
#include "keys.h"
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include "Keyboard.h"
#include <avr/power.h>
#include "TimerOne.h"
#include "measure.h"
#include "rpcparser.h"
#include "memory.h"
#include "usbmanager.h"
#include "Adafruit_NeoPixel.h"

#define NUMPIXELS 1
#define DATA_PIN 13

// //DECLARE ALL THE GLOBAL USED VARS AND CONSTANTS

//VARIABLES MARIO
#define SAMPLE_INTERVAL_TICKS 250       // 1 ms

static uint16_t msCounter = 0;
volatile bool sleepNow = false;
volatile bool triggerFromINT0 = false;
volatile bool triggerFromINT1 = false;
volatile bool triggerFromINT6 = false;
volatile bool triggerFromPCINT4 = false;

const uint8_t pinPulseOut = 12;
const uint8_t pinToneOut = 11;

bool pulseActive = false;
bool toneActive = false;
unsigned long pulseStartTick = 0;
unsigned long toneStartTick = 0;
unsigned long lastToggleTick = 0;
unsigned long bootTick = 0;
bool toneState = false;

volatile unsigned long tickCounter = 0;
volatile unsigned long lastWakeTick = 0;

bool f12TriggerPending = false;
uint8_t f12Stage = 0;
unsigned long f12ActionStartTick = 0;

volatile bool wasAsleep = false;
bool playToneOnWake = false;


// /*****************VARIABLES AND CONSTANTS********************/
unsigned long startMillis;
unsigned long currentMillis;
unsigned long previousMillis = 0;

//Adding startup timer counter for SNR validation
unsigned long past_startup_time = 0;
unsigned long startup_time = 0;


//REMOVE AFTER TESTING
unsigned long testing_time;
unsigned long past_testing_time = 0;

int buzzerOnperiod = 25;
int buzzerOffperiod = 1000;

boolean isopen = false;


int lowbatth = 12000;
int highbatth = 12500;
int overvoltagebat = 14300;
int undervoltagebat = 11000;

int ipvfactor = 51;
int vpvfactor = 49;
int vbatfactor = 152;


// Digital pins mapping
const int sda = 4;              // I2C SDA Pin
const int scl = 5;              // I2C SCL Pin
const int chargestatus = 9;     // Get charger status
//const int maindoor = 8;         // Check if main door is open
const int chargeerror = 6;      // Get charger error state
const int buzzer = 11;          // Buzzer interface
//const int isawake = 21;         // Check if parkmeter is awake

//Analog pins mapping
// const int vbatadc = 18;       // Read battery voltage
// const int ipvadc = 19;        // Read PV current (ZXCT1110)
// const int vpvadc = 20;        // Read PV voltage
const int extranalogadc = 22;      // TO BE USED


//Variable and Flags initialization

//Flags

volatile int wakeStatus = 0;       // Variable to store a request for wake
volatile int goingtosleep = 0;     // Sleep mode control variable
volatile int sendkeyStatus = 0;    // Checks when and what key to send
int usbstatus = 0;        // Check if USB is on suspend or active mode
int buzzing = 0;          // Buzzer flag.
int buzzerstate = 0;      // Holds buzzer state
unsigned long int buzzerperiod = 1000;  // Buzzer variable period


//DigitalPins Status

int wakepinstatus = 0;
int coinpinstatus = 0;
int safedoorpinstatus = 0;
int maindoorstatus = 0;
volatile int isawakepinstatus = 0;
int spare1pinstatus = 0;

//Timer variables
int tcounter = 0;
int onecounter = 0;
int const slow_charging = 6;
int const fast_charging = 80;
int const sleep_charging = 5;
int chargecounter = 0;
bool makemeasure = 0;
int chgstatus = 0;

//Variables

//_________ADC Value_________
// int vbatsense = 0;
// int ipvsense = 0;
// int vpvsense = 0;

//_______Voltage Values_______
// int vbat_new = 0;
// int ipv_new = 0;
// int vpv_new = 0;
// int vbat_ant = 12600;
// int ipv_ant = 0;
// int vpv_ant = 0;
int vbat = 0;
int ipv = 0;
int vpv = 0;

int pdce_temp = 0;
int itemp = 0;

int fw = 0;

int i = 0;
byte incomingByte = 0;

int buzzerflag = 0;
int set_mode = 1; 
int buzzerbuttonflag = 0;
int wakeupflag = 0;

int failedcounter = 0;

bool boot_time = true;

long booting_time = 0;
long past_booting_time = 0;

bool human_interaction = false;
int boot_status = 0;

int fw_version;

int on_count = 0;
int max_on_time = 240; //multiplo de 5 = 720 = 3600 segundos  // Testing 240 15042025
int can_sleep = 0;

bool messageError = false;
int clearserialbuffer = 0;
int set_color = 1; // Green

// Define the array of leds
Adafruit_NeoPixel pixels(NUMPIXELS, DATA_PIN, NEO_GRB + NEO_KHZ800);


Measure measure;
USB usb;
extern struct MemStruct Memo; 

//=================================_________================================//
//================================|__SETUP__|===============================//
//==========================================================================//

void setup()
{
  delay(1000); // REMOVE AFTER PROGRAM IS CORRECT!!!!!!!
  //NOT USED PINS - "pinMode(pin, INPUT);"
  EEPROMConfig();

  pinMode(2, INPUT_PULLUP);  // INT0
  pinMode(3, INPUT_PULLUP);  // INT1
  pinMode(7, INPUT);         // INT6
  pinMode(8, INPUT);  // PCINT4
  pinMode(isawake, INPUT);

  pinMode(buzzer, OUTPUT);
  pinMode(batterycontrol, OUTPUT);
  //Starts low to prevent tablet turning ON - OFF - ON
  digitalWrite(batterycontrol, HIGH);
  pinMode(spare1,OUTPUT);
  digitalWrite(spare1, LOW);

    pinMode(pinPulseOut, OUTPUT);
  pinMode(pinToneOut, OUTPUT);

  digitalWrite(pinPulseOut, LOW);
  digitalWrite(pinToneOut, LOW);

  setupTimer1();

  EICRA |= (1 << ISC11) | (1 << ISC01);    // Falling edge INT1, INT0
  EICRA &= ~((1 << ISC10) | (1 << ISC00));
  EICRB &= ~((1 << ISC61) | (1 << ISC60)); // INT6 low level

  EIMSK |= (1 << INT0) | (1 << INT1) | (1 << INT6);

  PCICR |= (1 << PCIE0);
  PCMSK0 |= (1 << PCINT4);

  bootTick = tickCounter;

  sei();
  


  /*// External interrupts configuration
  attachInterrupt(digitalPinToInterrupt(wakepin), wakeUpNow, FALLING); // WakeButton Interrupt (LOW)
  attachInterrupt(digitalPinToInterrupt(coinpin), wakeUpCoin, FALLING); // WakeButton Interrupt (LOW)
  attachInterrupt(digitalPinToInterrupt(safedoorpin), wakeUpDoor, LOW); // WakeButton Interrupt (LOW)

  // Timer1 interrupt
  Timer1.initialize(1000);
  Timer1.attachInterrupt(timer_callback);
*/
  //HID and UART interfaces
  Keyboard.begin();
  Serial1.begin(19200);
  Serial1.setTimeout(500);

  EEPROMConfig(); // Reads config values from EEPROM.
  readEEPROM();
  methodRegistration();

  pixels.begin();

}


//=============================_____________________=============================//
//============================|_EXTERNAL INTERRUPTS_|============================//
//===============================================================================//



//=============================____________=============================//
//============================|_INTERRUPTS_|============================//
//======================================================================//

void timer_callback() {

  tcounter++;
  if (tcounter >= 5000) {
    makemeasure = 1;
    tcounter = 0;
    chargecounter = onecounter;
    onecounter = 0;
  }
  else {
    if (digitalRead(6) == 1) {
      onecounter++;
    }
  }
}

//================================*************================================//
//================================**FUNCTIONS**================================//
//================================*************================================//

void checkBattery() {

  int isawakepinstatus = digitalRead(isawake);

  //Deprecated. Is now used for wake coin signal. Controlled by analog circuit
  if (vbat >= highbatth) {
    digitalWrite(batterycontrol, HIGH);

  }
  else if (vbat < lowbatth && isawakepinstatus == 0) {
    digitalWrite(batterycontrol, HIGH); 
  }
}



void buzz(void) {
  currentMillis = millis();
  if (currentMillis - previousMillis >= buzzerperiod) {
    previousMillis = currentMillis;
    if (buzzerstate == 0) {
      tone(buzzer, 4000, 50);
      buzzerperiod = buzzerOnperiod;
      buzzerstate = 1;
    }
    else {
      digitalWrite(buzzer, LOW);
      buzzerstate = 0;
      buzzerperiod = buzzerOffperiod;
    }
  }
}

void chargeStatus() {

  //1 >> Charged || 2 >> Fast Charging ||  >> Slow Charging || 4 >> Sleep || 5 >> Charger Error //
  if (chargecounter >= 2000) {
    chgstatus = 1;
  }
  else if (chargecounter >= 80 && chargecounter <= 1000) {
    chgstatus = 2;
  }
  else if (chargecounter >= 6 && chargecounter <= 75) {
    chgstatus = 3;
  }
  else if (chargecounter < 6) {
    chgstatus = 4;
  }

  if (digitalRead(9) == 1) {
    chgstatus = 5;
  }
  else {

  }
}

/*void startupCheck(){
  
  int wake_status = digitalRead(isawake);
  if(wake_status==1){
    failedcounter = 0;
  }
  else{
    //Increment failed counter
    failedcounter++;
    wakeupflag=0;
    }
}*/

/*void checkGPIOBlock(){

  int isawake_gpio = digitalRead(isawakepinstatus);
  if(isawake_gpio==1){
    on_count++;
  }
 
  else if(isawake_gpio==0){
    on_count=0;
  }
}*/

void loop() {
  if ((tickCounter - bootTick >= 60000) && digitalRead(8) == HIGH && (tickCounter - lastWakeTick >= 1000)) {
    sleepNow = true;
  }

  if (sleepNow) {
    wasAsleep = true;
    goToSleep();
    sleepNow = false;
    lastWakeTick = tickCounter;
  }

  if (triggerFromINT0 || triggerFromINT1 || triggerFromINT6 || triggerFromPCINT4) {
    bool toneShouldPlay = false;

    if (triggerFromINT0) {
      triggerFromINT0 = false;
      if (playToneOnWake) {
        toneShouldPlay = true;
        playToneOnWake = false;
      }
    }
    if (triggerFromINT1) {
      triggerFromINT1 = false;
      toneShouldPlay = true;
    }
    if (triggerFromINT6) {
      triggerFromINT6 = false;
      toneShouldPlay = true;
    }
    if (triggerFromPCINT4) {
      triggerFromPCINT4 = false;
      toneShouldPlay = true;
    }

    digitalWrite(pinPulseOut, HIGH);
    pulseStartTick = tickCounter;
    pulseActive = true;

    if (toneShouldPlay) {
      toneStartTick = tickCounter;
      lastToggleTick = tickCounter;
      toneActive = true;
      toneState = false;
      digitalWrite(pinToneOut, LOW);
    }
  }

  if (pulseActive && (tickCounter - pulseStartTick >= 300)) {
    digitalWrite(pinPulseOut, LOW);
    pulseActive = false;
  }

  if (toneActive) {
    buzzerbuttonflag=1;
    if (tickCounter - toneStartTick >= 50) {
      //digitalWrite(pinToneOut, LOW);
      buzzerbuttonflag=0;
      toneActive = false;
    } else if (tickCounter - lastToggleTick >= 1) {
      toneState = !toneState;
      //digitalWrite(pinToneOut, toneState);
      buzzerbuttonflag=1;
      lastToggleTick = tickCounter;
    }
  }

  if (!(EIMSK & (1 << INT6)) && digitalRead(7) == HIGH) {
    EIMSK |= (1 << INT6);
  }

  if (f12TriggerPending) {
    switch (f12Stage) {
      case 1:
        Keyboard.press(KEY_F12);
        f12ActionStartTick = tickCounter;
        f12Stage = 2;
        break;
      case 2:
        if (tickCounter - f12ActionStartTick >= 250) {
          Keyboard.release(KEY_F12);
          f12ActionStartTick = tickCounter;
          f12Stage = 3;
        }
        break;
      case 3:
        if (tickCounter - f12ActionStartTick >= 100) {
          Keyboard.press(KEY_F12);
          f12ActionStartTick = tickCounter;
          f12Stage = 4;
        }
        break;
      case 4:
        if (tickCounter - f12ActionStartTick >= 250) {
          Keyboard.release(KEY_F12);
          f12TriggerPending = false;
          f12Stage = 0;
        }
        break;
    }
  }

  if (makemeasure) {
    
    switch(set_color){
      case 1:
        pixels.setPixelColor(0,0,25,0);
        pixels.show();
        break;
      case 2:
        pixels.setPixelColor(0,25,25,0);
        pixels.show();
        break;
      case 3:
        pixels.setPixelColor(0,25,0,0);
        pixels.show();
        break;
      case 4:
        pixels.setPixelColor(0,0,0,25);
        pixels.show();
        break;
      default:
        pixels.setPixelColor(0,0,15,0);
        pixels.show();
      break;
    }
    //delay(100);
    measure.VBAT();
    measure.VPV();
    measure.IPV();
    measure.temperature();
    checkBattery();
    chargeStatus();
    makemeasure = 0;
    pixels.setPixelColor(0,0,0,0);
    pixels.show();
    delay(100);
    //}
  }

    if (Serial1.available()) {

      parseJson();
    }

  if (buzzerflag) {
    buzz();
  }
  else {
    digitalWrite(buzzer, LOW);
  }

  //WakeUP Sound notification
  switch (buzzerbuttonflag){
    case 1:
      tone(11, 3136, 50);
      delay(100);
      //Serial1.println("Buzzing");
      tone(11, 3951, 50);
      delay(100);
      tone(11, 4699, 200);
      delay(200);
      buzzerbuttonflag = 0;
      break;
    case 2:
        tone(11, 4186, 50);
        delay(100);
        tone(11, 3729, 50);
        delay(100);
        tone(11, 3322, 50);
        delay(100);
        tone(11, 4978, 100);
        delay(200);
        buzzerbuttonflag = 0;
        break;
      default:
        break;
  }

}


void goToSleep() {
  digitalWrite(buzzer, LOW);  // Turns off buzzer.
  pixels.setPixelColor(0,0,0,0);
  pixels.show();
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  sleep_cpu();
  sleep_disable();
}

void setupTimer1() {
  noInterrupts();
  TCCR1A = 0;
  TCCR1B = 0;
  TCCR1B |= (1 << WGM12);
  TCCR1B |= (1 << CS11) | (1 << CS10); // Prescaler 64
  OCR1A = SAMPLE_INTERVAL_TICKS;
  //OCR1B = 1000; // 1 second = 1000 ms
  TIMSK1 |= (1 << OCIE1A);
  //TIMSK1 |= (1 << OCIE1B);
  interrupts();
}

ISR(TIMER1_COMPA_vect) {

  tickCounter++;
  msCounter++;

  if (msCounter >= 5000) {
    msCounter = 0;
    makemeasure = 1;
    chargecounter=onecounter;
    onecounter=0;
    }
    else{
      if (digitalRead(6) == 1) {
      onecounter++;
      }
  }
}


ISR(INT0_vect) {
  if (tickCounter - lastWakeTick >= 1000) {
    triggerFromINT0 = true;
    sleepNow = false;
    lastWakeTick = tickCounter;

    f12TriggerPending = true;
    f12Stage = 1;
    f12ActionStartTick = tickCounter;

    if (wasAsleep) {
      playToneOnWake = true;
    }
    wasAsleep = false;
  }
}

ISR(INT1_vect) {
  if (tickCounter - lastWakeTick >= 1000) {
    triggerFromINT1 = true;
    sleepNow = false;
    lastWakeTick = tickCounter;
  }
}

ISR(INT6_vect) {
  if (tickCounter - lastWakeTick >= 1000) {
    triggerFromINT6 = true;
    sleepNow = false;
    lastWakeTick = tickCounter;
  }
  EIMSK &= ~(1 << INT6);
}

ISR(PCINT0_vect) {
  if (!(PINB & (1 << PB4)) && (tickCounter - lastWakeTick >= 1000)) {
    triggerFromPCINT4 = true;
    sleepNow = false;
    lastWakeTick = tickCounter;
  }
}