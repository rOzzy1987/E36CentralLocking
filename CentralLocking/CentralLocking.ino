#include <PinChangeInterrupt.h>
#include <PinChangeInterruptBoards.h>
#include <PinChangeInterruptPins.h>
#include <PinChangeInterruptSettings.h>
#include <Arduino.h>
#include <avr/sleep.h>
#include "lockSignal.h"

#define LOCK_SIGNAL_PIN 0
#define KEY_SIGNAL_PIN 2

// Number of seconds between lock signals to indicate window closing intent
#define DOUBLE_PRESS_SECONDS 5
// Number of seconds to keep key signal on for raising windows
#define WINDOW_SECONDS 15
// Number of milliseconds to keep key signal on to lock the doors fully
#define KEYLOCK_MILLIS 500

//#define LDBG

///////////////// DO NOT REDEFINE /////////////////

#define WINDOW_MILLIS WINDOW_SECONDS * 1000
#define DOUBLE_PRESS_MILLIS DOUBLE_PRESS_SECONDS * 1000


#define STATE_WAKING 0
#define STATE_WAITINGFORLOCK 1
#define STATE_LOCKING 2
#define STATE_LOCKINGENDED 3
#define STATE_WINDOWUP 4
#define STATE_WINDOWUPENDED 5
#define STATE_GOTOSLEEP 6

uint8_t state = STATE_WAKING;

unsigned long wut;

#ifdef LDBG
uint8_t c;
#endif

void setup() {
  // Handling USB pins

  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  digitalWrite(3, 0);
  digitalWrite(4, 0);

  initLockPin(LOCK_SIGNAL_PIN);
  pinMode(KEY_SIGNAL_PIN, OUTPUT);

  #ifdef LDBG
  pinMode(1, OUTPUT);
  #endif
}


void loop() {
    switch (state) {
        case STATE_WAKING:
            initWakeUp();
            break;
        case STATE_WAITINGFORLOCK: 
            waitForLock();
            break;
        case STATE_LOCKING:
            waitForLockEnd();
            break;
        case STATE_LOCKINGENDED:
            waitForWindow(); 
            break;
        case STATE_WINDOWUP:
            waitForWindowEnd();
            break;
        case STATE_WINDOWUPENDED:
            sleepIfTimeout();
            break;
        default:
            goToSleep();
            break;
    }
    delay(25);

    #ifdef LDBG
    c++;
    digitalWrite(1, (c & 5) > 0);
    #endif
}

void sendKeySignal(unsigned int length) {
    unsigned long s = millis();
    digitalWrite(KEY_SIGNAL_PIN, HIGH);
    delay(length);
    digitalWrite(KEY_SIGNAL_PIN,LOW);
}

void sleepIfTimeout() {
    if (millis() - wut > DOUBLE_PRESS_MILLIS) {
        state = STATE_GOTOSLEEP;
    }
}

void initWakeUp() {
    wut = millis();
    state = STATE_WAITINGFORLOCK;
    resetLockBuffer();
}

void waitForLock() {
    if (isLocking()) {
        sendKeySignal(KEYLOCK_MILLIS);
        state = STATE_LOCKING;    
    } else {
        sleepIfTimeout();
    }
}

void waitForLockEnd() {
    if (!isLocking()) {
        state = STATE_LOCKINGENDED;
    } else {
        sleepIfTimeout();
    }
}

void waitForWindow() {
    if (isLocking()) {
        sendKeySignal(WINDOW_MILLIS);
        state = STATE_WINDOWUP;    
    } else {
        sleepIfTimeout();
    }
}

void waitForWindowEnd() {
    if (!isLocking()) {
        state = STATE_WINDOWUPENDED;
    } else {
        sleepIfTimeout();
    }
}

void goToSleep() {         
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);   // sleep mode is set here
    sleep_enable();                        // enables the sleep bit in the mcucr register so sleep is possible
    attachPCINT(digitalPinToPCINT(LOCK_SIGNAL_PIN), wakeUpNow, RISING);

    #ifdef LDBG
    c = 0;
    digitalWrite(1, 0);
    #endif

    sleep_mode();

    // Actual sleep //

    sleep_disable(); 
    detachPCINT(digitalPinToPCINT(LOCK_SIGNAL_PIN));
}

void wakeUpNow() {
    state=STATE_WAKING;
}