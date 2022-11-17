#include "Arduino.h"
//#undef STRICT_ANSI

// This just gets rid of Intellisense squiggles for setenv() and tzset()
//_VOID _EXFUN(tzset, (_VOID));
// int _EXFUN(setenv, (const char *__string, const char *__value, int __overwrite));
// see: https://community.platformio.org/t/identifier-is-undefined-setenv-tzset/16162/8 for another fix
extern tm timeinfo;

namespace ntpH
{
    /*
        See https://github.com/G6EJD/ESP32-Time-Services-and-SETENV-variable/blob/master/ESP32_Time_and_SETENV.ino
        for details of setenv() and how to specify daylight saving months
    */

    // Time settings
    const char *ntpServer = "europe.pool.ntp.org";
    const long gmtOffset_sec = 0;
    const int daylightOffset_sec = 3600;

    /* This is the time info retrieved:

    struct tm:
        int	tm_sec;
        int	tm_min;
        int	tm_hour;
        int	tm_mday;
        int	tm_mon;
        int	tm_year;
        int	tm_wday;
        int	tm_yday;
        int	tm_isdst;

        %A	Full weekday name
        %B	Full month name
        %d	Day of the month
        %Y	Year
        %H	Hour in 24h format
        %I	Hour in 12h format
        %M	Minute
        %S	Second
        %b  Abbreviated month name
        %a  Abbreviated weekday name
        See http://www.cplusplus.com/reference/ctime/strftime/ for more

    */

    // Excuted just once in the setup to avoid memory leaks
    void setDaylightSavingOptions()
    {
        // Set the UK timezone (when the DST changes, March and October)
        configTime(0, 0, ntpServer);

        // Set the environment variable TZ (timezone) with the daylight settings
        // See: https://www.gnu.org/software/libc/manual/html_node/TZ-Variable.html
        // and https://github.com/G6EJD/ESP32-Time-Services-and-SETENV-variable
        setenv("TZ", "GMT0BST,M3.5.0/01,M10.5.0/02", 1);
        tzset();
    }

    // Gets the time from internal ESP32 clock
    uint8_t getTime()
    {
        if (!getLocalTime(&timeinfo))
        {
            log_e("Failed to obtain local time");
            return 0;
        }

        return 1;
    }

    // Retrieve time from "local time"
    bool getCurrentTime()
    {
        // Several attempts at getting the time
        for (auto cnt = 0; cnt < 5; cnt++)
        {
            if (ntpH::getTime())
            {
                hoursTime = timeinfo.tm_hour;
                minsTime = timeinfo.tm_min;
                secsTime = timeinfo.tm_sec;
                return true;
            }
            log_v("Re-trying to get local time, attempt: %d", cnt + 1);
            delay(1000);
        }
        return false;
    }

} // namespace ntpH