#pragma once
#include <Arduino.h>
#include <max7219.h>

// 8-digit 7-segment display
MAX7219 max7219;

namespace sevenH
{
    void clearDisplay()
    {
        max7219.Begin();
        max7219.Clear();
        max7219.MAX7219_SetBrightness(1);
    }

    // Send out the countdown timer (AUTO_ON_MINS on last 4 digits) to the display
    void displayCountDown(uint8_t mins, uint8_t secs)
    {
        // Count down the seconds (always starts at 9:99)
        if (secs == 0)
        {
            if (mins == 0)
            {
                // No more time to show, blank the display
                max7219.DisplayText((char *)"    ", RIGHT);
                countDownTimerActive = false;
                log_v("Countdown timer cleared.");
                return;
            }
            mins--;
            secs = 59;
        }
        else
        {
            secs--;
        }

        char countDownStr[6];
        sprintf(countDownStr, "%5.2f", (float)(mins) + (secs / 100.0));

        // Output the countdown, right justified
        max7219.DisplayText((char *)countDownStr, RIGHT);

        // Update global variables to match local values
        minsCountDown = mins;
        secsCountDown = secs;
    }

    void displayTime()
    {
        // Convert time 12:45 to 12.45 so we can display it as a float
        char timeStr[6];
        sprintf(timeStr, "%05.2f", (float)(hoursTime) + (minsTime / 100.0));

        // Output the time, left justified
        max7219.DisplayText((char *)timeStr, LEFT);
        max7219.MAX7219_SetBrightness(1);

        // If we are still in a countdown period (manually stopped pump via web page) show remaining time here
        if (countDownTimerActive)
        {
            sevenH::displayCountDown(minsCountDown, secsCountDown);
        }
    }
}