#pragma once
#include <Arduino.h>
#include "DualDigit_74HC595.h"

// Create dual digit display object with data, clock and latch pins
DualDigit_74HC595 dualDigit(DualDigit_74HC595::commonLedType::CATHODE);

namespace dd
{
// Pin definitions for the Dual Digit 7-seg display - any free ones can be used
#define dataPin 25
#define clkPin 32
#define latchPin 27

    void blankDisplay()
    {
        dualDigit.begin(dataPin, clkPin, latchPin);
        dualDigit.blankDigits();
    }

    void showValue(uint8_t value) {
        dualDigit.updateValue(value);
        dualDigit.sendOut();
    }
}