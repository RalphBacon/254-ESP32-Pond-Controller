#pragma once
#include <Arduino.h>
#include "defsHelper.h"
#include "7H.hpp"

namespace timerH
{
    // Foward declarations
    void updateLocalTimeManually();

    // Called exactly once per second
    void timerCallBack()
    {
        // Built-in ESP32 function to get the time
        if (!ntpH::getCurrentTime())
        {
            // If it fails, update the time ourselves
            timerH::updateLocalTimeManually();
        }

        // If we are in the darker months, adjust the QUIET TIME so pumps switch off earlier in the afternoon
        static uint8_t currentMonth = 99;
        if (timeinfo.tm_mon != currentMonth)
        {
            if ((timeinfo.tm_mon + 1) >= 10 || (timeinfo.tm_mon + 1) <= 4)
            {
                QUIET_PERIOD_START = 16;
                log_i("Current month OCT-APR so pumps will stop at 16:00");
            }
            else
            {
                QUIET_PERIOD_START = 17;
                log_i("Current month MAY-SEP so pumps will stop at 17:00");
            }
            currentMonth = timeinfo.tm_mon;
        }

        /*
            Set various time-related flags so main loop can do some work
        */

        // Change of hour?
        if (hoursTime != prevHour && minsTime == 0)
        {
            log_v("Hour has changed from %d to %d", prevHour, hoursTime);
            prevHour = hoursTime;
            newHour = true;

            // Feeding time starts on the hour
            if (!quietPeriod)
            {
                feedingTime = true;
            }
            else
            {
                feedingTime = true;
            }
        }

        // Quarter past the hour?
        if (minsTime == 15)
        {
            // This quarter-past not yet processed
            if (!prevQuarterPast)
            {
                log_v("Quarter Past the hour detected");
                prevQuarterPast = true;
                quarterPast = true;
            }

            // Feeding time is over
            feedingTime = false;
        }
        else if (prevQuarterPast)
        {
            // Reset the quarter past only when time not on 15 mins
            prevQuarterPast = false;
            log_v("Quarter Past the hour reset");
        }

        // Half-past the hour?
        if (minsTime == 30)
        {
            // This half-past not yet processed
            if (!prevHalfPast)
            {
                log_v("Half Past the hour detected");
                prevHalfPast = true;
                halfPast = true;
            }
        }
        else if (prevHalfPast)
        {
            // Reset the half-past only when time not on 30 mins
            prevHalfPast = false;
            log_v("Half Past the hour reset");
        }

        // Quarter-to the hour?
        if (minsTime == 45)
        {
            // This quarter-to not yet processed
            if (!prevQuarterTo)
            {
                log_v("Quarter To the hour detected");
                prevQuarterTo = true;
                quarterTo = true;
            }
        }
        else if (prevQuarterTo)
        {
            // Reset the quarter-to only when time not on 45 mins
            prevQuarterTo = false;
            log_v("Quarter To the hour reset");
        }

        // Are we in Quiet-period?
        quietPeriod = (hoursTime >= QUIET_PERIOD_START || hoursTime < QUIET_PERIOD_END) ? true : false;
        if (quietPeriod)
        {
        }
        else
        {
            if (prevQuietPeriod)
            {
                prevQuietPeriod = false;
                log_v("Quiet period reset");
            }
        }

        // Display the time and/or countdown timer
        sevenH::displayTime();
    }

    // If the call to the local ESP32 function to get the time fails, do it here
    // Might fail the first time (on startup) as it must do a call to the NTP server
    void updateLocalTimeManually()
    {
        // Increment seconds counter
        secsTime++;

        // Adjust seconds/minutes/hours overflow
        if (secsTime > 59)
        {
            minsTime++;
            secsTime = 0;

            // Increment the minutes and check for overflow
            if (minsTime > 59)
            {
                // Our local time variables
                minsTime = 0;
                hoursTime++;
                newHour = true;

                // New day dawning?
                if (hoursTime > 23)
                {
                    hoursTime = 0;
                }
            }
        }
        log_i("Time updated manually");
    }
}