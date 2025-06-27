//********************************************//
//*************PDCE FIRMWARE v1.0.0***********//
//********************************************//
#include "main.h"
#include "Keyboard.h"
//#include "TimerOne.h"
#include "measure.h"
#include "rpcparser.h"
#include "memory.h"
#include "Adafruit_NeoPixel.h"
#include <avr/wdt.h>

#define NUMPIXELS 1
#define DATA_PIN 13
#define SAMPLE_INTERVAL_TICKS 250       // 1 ms

// Define the array of leds
Adafruit_NeoPixel pixels(NUMPIXELS, DATA_PIN, NEO_GRB + NEO_KHZ800);

// Timer and sleep control
static uint16_t msCounter = 0;
volatile bool sleepNow = false;
volatile bool triggerFromINT0 = false;
volatile bool triggerFromINT1 = false;
volatile bool triggerFromINT6 = false;
volatile bool triggerFromPCINT4 = false;

volatile unsigned long tickCounter = 0;
volatile unsigned long lastWakeTick = 0;
volatile unsigned long watchdogWakeTick = 0;

bool pulseActive = false;
unsigned long pulseStartTick = 0;
unsigned long bootTick = 0;

bool f12TriggerPending = false;
uint8_t f12Stage = 0;
unsigned long f12ActionStartTick = 0;

volatile bool wasAsleep = false;
bool playToneOnWake = false;

// Tone sequence state
enum ToneState { TONE_IDLE, TONE_STEP1, TONE_STEP2, TONE_STEP3, TONE_DONE };
ToneState toneState = TONE_IDLE;
unsigned long toneStartTick = 0;

// Measurement LED state
enum LEDState { LED_IDLE, LED_START, LED_DONE };
LEDState ledState = LED_IDLE;
unsigned long ledFlashStartTick = 0;

unsigned long startMillis;
unsigned long currentMillis;
unsigned long previousMillis = 0;

//Variable and Flags initialization

//Flags

int buzzerstate = 0;      // Holds buzzer state
unsigned long int buzzerperiod = 1000;  // Buzzer variable period
volatile bool watchdogTriggered = false;

//Timer variables
int onecounter = 0;
int chargecounter = 0;
bool makemeasure = 0;
int chgstatus = 0;

//Charging status constants
int const slow_charging = 6;
int const fast_charging = 80;
int const charged = 2000;

//Variables

int vbat = 0;
int ipv = 0;
int vpv = 0;

int buzzerflag = 0;
int buzzerbuttonflag = 0;

int clearserialbuffer = 0;
int set_color = 1; // Green

Measure measure;
SensorReadings sensors;
GPIOStatus gpio;
DeviceConfig config;
extern struct MemStruct Memo; 

//=================================_________================================//
//================================|__SETUP__|===============================//
//==========================================================================//

void setup()
{
  EEPROMConfig();

  pinMode(wakePin, INPUT_PULLUP);   // INT0
  pinMode(coinPin, INPUT_PULLUP);   // INT1
  pinMode(doorStatusPin, INPUT);    // INT6
  pinMode(pmic_stby_req, INPUT);    // PCINT4
  pinMode(isAwake, INPUT);
  pinMode(chgErrorPin, INPUT);
  pinMode(chgStatusPin, INPUT);

  pinMode(coinWakeSignalPin, OUTPUT);
  digitalWrite(coinWakeSignalPin, HIGH);

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
  
  //HID and UART interfaces
  Keyboard.begin();
  Serial1.begin(19200);
  Serial1.setTimeout(500);

  EEPROMConfig(); // Reads config values from EEPROM.
  readEEPROM();
  methodRegistration();

  pixels.begin();
  pixels.setPixelColor(0, 0, 0, 0);
  pixels.show();

  MCUSR &= ~(1<<WDRF); // Clear watchdog reset flag
  WDTCSR |= (1<<WDCE) | (1<<WDE); // Start timed sequence
  WDTCSR = (1<<WDIE) | (1<<WDP3) | (1<<WDP0); // 8s watchdog interrupt
}

//================================*************================================//
//================================**FUNCTIONS**================================//
//================================*************================================//


void buzz(void) {
  currentMillis = tickCounter;
  if (currentMillis - previousMillis >= buzzerperiod) {
    previousMillis = currentMillis;
    if (buzzerstate == 0) {
      tone(pinToneOut, 4000, config.buzzerOnperiod);
      buzzerperiod = config.buzzerOnperiod;
      buzzerstate = 1;
    }
    else {
      digitalWrite(pinToneOut, LOW);
      buzzerstate = 0;
      buzzerperiod = config.buzzerOffperiod;
    }
  }
}

void chargeStatus()
{

  // 1 >> Charged || 2 >> Fast Charging || 3 >> Slow Charging || 4 >> Sleep || 5 >> Charger Error //
  if (chargecounter >= charged)
  {
    chgstatus = 1;
  }
  else if (chargecounter >= fast_charging)
  {
    chgstatus = 2;
  }
  else if (chargecounter >= slow_charging)
  {
    chgstatus = 3;
  }
  else if (chargecounter < slow_charging)
  {
    chgstatus = 4;
  }

  if (digitalRead(chgErrorPin) == 1)
  {
    chgstatus = 5;
  }
}

void loop() {
  if ((tickCounter - bootTick >= 60000) && digitalRead(pmic_stby_req) == HIGH && (tickCounter - lastWakeTick >= 4000) && (tickCounter - watchdogWakeTick >= 4000)) {
    sleepNow = true;
  }

  if (sleepNow) {
    wasAsleep = true;
    goToSleep();
    sleepNow = false;
    if(!watchdogTriggered) {
      lastWakeTick = tickCounter;
      watchdogWakeTick = tickCounter;
    }
    else {
      watchdogWakeTick = tickCounter;
    }
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

    // WakeUP Sound notification
    if (toneShouldPlay) startWakeToneSequence();
  }

  if (pulseActive && (tickCounter - pulseStartTick >= 300)) {
    digitalWrite(pinPulseOut, LOW);
    pulseActive = false;
  }

  if (!(EIMSK & (1 << INT6)) && digitalRead(doorStatusPin) == HIGH) {
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

  switch (toneState) {
    case TONE_STEP1:
      tone(pinToneOut, 3136, 50);
      toneStartTick = tickCounter;
      toneState = TONE_STEP2;
      break;
    case TONE_STEP2:
      if (tickCounter - toneStartTick >= 100) {
        tone(pinToneOut, 3951, 50);
        toneStartTick = tickCounter;
        toneState = TONE_STEP3;
      }
      break;
    case TONE_STEP3:
      if (tickCounter - toneStartTick >= 100) {
        tone(pinToneOut, 4699, 200);
        toneStartTick = tickCounter;
        toneState = TONE_DONE;
      }
      break;
    case TONE_DONE:
      if (tickCounter - toneStartTick >= 200) {
        digitalWrite(pinToneOut, LOW);
        toneState = TONE_IDLE;
      }
      break;
    default:
      if (!buzzerflag) {
        digitalWrite(pinToneOut, LOW);
      }
      break;
  }

  if (makemeasure){
    makemeasure = 0;
    sensors = measure.measureSensors();
    chargeStatus();
  }

  if(watchdogTriggered){
    ledState = LED_START;
    ledFlashStartTick = tickCounter;
    watchdogTriggered = false;
  }

  switch (ledState) {
    case LED_START:
      switch (set_color) {
        case 1: pixels.setPixelColor(0, 0, 25, 0); break;     // Green
        case 2: pixels.setPixelColor(0, 25, 25, 0); break;    // Yellow
        case 3: pixels.setPixelColor(0, 25, 0, 0); break;     // Red
        case 4: pixels.setPixelColor(0, 0, 0, 25); break;     // Blue
        default: pixels.setPixelColor(0, 0, 0, 0); break;     // No color
      }
      pixels.show();
      ledFlashStartTick = tickCounter;
      ledState = LED_DONE;
      break;
    case LED_DONE:
      if (tickCounter - ledFlashStartTick >= 100) {
        pixels.setPixelColor(0, 0, 0, 0);
        pixels.show();
        ledState = LED_IDLE;

        if (digitalRead(pmic_stby_req) == HIGH && !pulseActive){
          // Now go to sleep after flashing
          sleepNow = true;
          return; // Skip the rest of loop
        }
      }
      break;
    default:
      break;
  }

  if (Serial1.available()){
    parseJson();
  }

  if (buzzerflag) buzz();

}

void startWakeToneSequence() {
  toneState = TONE_STEP1;
  toneStartTick = tickCounter;
}

void goToSleep() {
  // Turn off buzzer and LED
  digitalWrite(pinToneOut, LOW);
  pixels.setPixelColor(0,0,0,0);
  pixels.show();

  // Disable UART
  Serial1.end();

  // Disable ADC
  ADCSRA &= ~(1 << ADEN);

  // Enable sleep mode
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  sleep_cpu();

  // Wakes up here
  sleep_disable();

  // Re-enable ADC
  ADCSRA |= (1 << ADEN);

  // Re-enable UART
  Serial1.begin(19200);
  Serial1.setTimeout(500);
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
      if (digitalRead(chgStatusPin) == 1) {
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
    wasAsleep = false;
  }
}

ISR(INT6_vect) {
  if (tickCounter - lastWakeTick >= 1000) {
    triggerFromINT6 = true;
    sleepNow = false;
    lastWakeTick = tickCounter;
    wasAsleep = false;
  }
  EIMSK &= ~(1 << INT6);
}

ISR(PCINT0_vect) {
  if (!(PINB & (1 << PB4)) && (tickCounter - lastWakeTick >= 1000)) {
    triggerFromPCINT4 = true;
    sleepNow = false;
    lastWakeTick = tickCounter;
    wasAsleep = false;
  }
}

ISR(WDT_vect) {
  watchdogTriggered = true;
}