#pragma once
#include "Arduino.h"
#include "defsHelper.h"

namespace relays
{
    // Forward declarations
    void updateStatusLEDs();
    void airOn();
    void airOff();
    void waterOn();
    void waterOff();

    // Set pin modes and initial states of relays
    void begin()
    {
        // Initialise relays & set to initial value
        pinMode(AIR, OUTPUT);
        pinMode(WATER, OUTPUT);
        digitalWrite(AIR, _AIR_OFF);
        digitalWrite(WATER, _WATER_OFF);

        // Default water and air are ON
        waterOn();
        airOn();

        // Status LEDs (bidirectional)
        pinMode(redAir, OUTPUT);
        pinMode(greenAir, OUTPUT);
        pinMode(redWater, OUTPUT);
        pinMode(greenWater, OUTPUT);
        updateStatusLEDs();
    }

    /*
        IMPORTANT NOTE: THE AIR AND WATER PINS ARE LOW WHEN ON AND HIGH WHEN OFF.

        FOR CLARITY USE THE DEFINITIONS OF _WATER_ON, _AIR_ON (AND THE 'OFF' VERSIONS)
    */
    void airOn()
    {
        if (digitalRead(AIR) == _AIR_ON)
            log_w("Air already ON");

        digitalWrite(AIR, _AIR_ON);
        updateStatusLEDs();
        log_v("AIR on");
    }

    void waterOn()
    {
        if (digitalRead(WATER) == _WATER_ON)
            log_w("Water already ON");

        digitalWrite(WATER, _WATER_ON);
        updateStatusLEDs();
        log_v("WATER on");
    }

    void airOff()
    {
        if (digitalRead(AIR) == _AIR_OFF)
            log_w("Air already OFF");

        digitalWrite(AIR, _AIR_OFF);
        updateStatusLEDs();
        log_v("AIR off");
    }

    void waterOff()
    {
        if (digitalRead(WATER) == _WATER_OFF)
            log_w("Water already OFF");

        digitalWrite(WATER, _WATER_OFF);
        updateStatusLEDs();
        log_v("WATER off");
    }

    // Switch it all back on, initiated by task.
    void airWaterOn()
    {
        // If not within 15 mins of an hour, or quiet period, switch all back on
        if (!quietPeriod && minsTime > 15)
        {
            airOn();
            waterOn();
            log_v("Water/Air auto-resumed");
        }
        else
        {
            log_w("Water/Air NOT auto-resumed because now is the %s", quietPeriod ? "Quiet Period" : "first 15 mins of an hour");
        }
    }

    bool isWaterOn() {
        return (digitalRead(WATER) == _WATER_ON);
    }

    bool isAirOn() {
        return (digitalRead(AIR) == _AIR_ON);
    }

    // Two bidirectional LEDs show status of AIR & WATER relays (red=off, green=on)
    void updateStatusLEDs()
    {
        // LEDs to reflect current relay statuses
        digitalWrite(greenAir, digitalRead(AIR));
        digitalWrite(redAir, !digitalRead(greenAir));

        digitalWrite(greenWater, digitalRead(WATER));
        digitalWrite(redWater, !digitalRead(greenWater));
    }
}