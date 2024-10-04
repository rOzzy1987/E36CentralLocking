#ifndef LOCKSIGNAL_H
#define LOCKSIGNAL_H

#include <Arduino.h>
#define LOCK_BUF_SIZE 8

uint8_t _lockPinBuffer[LOCK_BUF_SIZE];
uint8_t _lockPinIdx = 0;
uint8_t _lockPinState = 0;
uint8_t _lockPin;

void initLockPin(uint8_t pin){
    pinMode(pin, INPUT);
    _lockPin = pin;
}

void resetLockBuffer() {
  for(uint8_t i = 0; i < LOCK_BUF_SIZE; i++){
    _lockPinBuffer[i] = 0;
  }
}

uint8_t isLocking(){
    _lockPinBuffer[_lockPinIdx] = digitalRead(_lockPin);
    _lockPinIdx = (_lockPinIdx + 1) % LOCK_BUF_SIZE;

    uint8_t result = 0;
    for(uint8_t i = 0; i < LOCK_BUF_SIZE; i++){
        result += _lockPinBuffer[i];
    }
    if (result == 0)
        _lockPinState = 0;
    if (result == LOCK_BUF_SIZE)
        _lockPinState = 1;
    
    return _lockPinState;
}

#endif