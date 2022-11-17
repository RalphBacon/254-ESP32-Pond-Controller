#include <Arduino.h>
#include <max7219.h>
#include "time.h"
#include <WiFi.h>

// Local includes
#include "defsHelper.h"

void setup()
{
  Serial.begin(115200);

  // Clear and initialise the dual digit display
  dd::blankDisplay();
  dd::showValue(0);

  // Initialise and clear the 7-segment display
  sevenH::clearDisplay();

  // Start the LittleFS file system (used to show images on web pages)
  dd::showValue(10);
  if (!LittleFS.begin())
  {
    Serial.println("An Error has occurred while mounting LittleFS");
    while (1)
      ;
  }

  // Connect to Wi-Fi
  dd::showValue(15);
  WIFI::begin();
  if (!WIFI::connect())
  {
    log_e("Cannot connect to Wi-Fi");
    while (1)
      ;
  }

  // Initialise time zone etc
  dd::showValue(20);
  ntpH::setDaylightSavingOptions();
  ntpH::getCurrentTime();
  log_i("Current date/time is %02d/%02d/%02d at %02d:%02d:%02d",
        timeinfo.tm_mday, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900, hoursTime, minsTime, secsTime);

  // Allow various scheduling options
  clockTask.setSchedulingOption(TASK_SCHEDULE);

  // Initialise the task scheduler
  dd::showValue(25);
  schedule.init();
  schedule.addTask(clockTask);
  schedule.addTask(bubbleOffTask);
  schedule.addTask(allBackOnTask);

  // Once the task has been enabled it will keep real time
  dd::showValue(30);
  if (!clockTask.enable())
  {
    log_e("Unable to enable 'clockTask'");
    while (1)
      ;
  }

  // Initialise the relays, leds etc
  dd::showValue(30);
  relays::begin();

  // Various web pages to register
  dd::showValue(35);
  serverH::registerServerPages();

  // Blank the error display
  dd::blankDisplay();
}

void loop()
{
  // Check if OTA in progress
  static unsigned long OTAmillis = millis();
  if (Update.progress() > 0)
  {
    // Disable timer task(s)
    clockTask.disable();

    // Update in progress
    dd::showValue(99);

    // Occasional message showing progress
    size_t updateMaxBytes = Update.size();
    size_t progressUpdate = Update.progress();

    // Keep track of progress so we don't report same percentage twice
    static size_t prevPercentUploaded = 0;
    float_t percentUploaded = ((float)progressUpdate / (float)updateMaxBytes) * 100.0;

    if (percentUploaded != prevPercentUploaded)
    {
      if (millis() - OTAmillis > 1000 || percentUploaded > 95.0)
      {
        OTAmillis = millis();
        log_w("OTA update is running: uploaded %5.2f%%", percentUploaded);
      }
      prevPercentUploaded = percentUploaded;
    }

    // Do nothing else during OTA
    return;
  }

  // Ensure timer task execution
  schedule.execute();

  // If "quiet period" skip main process and check air only
  if (quietPeriod)
  {
    if (!prevQuietPeriod)
    {
      log_i("Quiet Period one-time processing starts");
      prevQuietPeriod = true;
      relays::waterOff();
      relays::airOff();
    }

    // Every 15 minutes during quietPeriod turn on AIR only (prevents water freezing)
    if (newHour || quarterPast || halfPast || quarterTo)
    {
      log_i("15 Minutes on the hour detected (15, 30, 45, 00)");
      newHour = false;
      quarterPast = false;
      halfPast = false;
      quarterTo = false;
      relays::airOn();

      // Schedule a one-shot task to switch off the air after X minute(s)
      bubbleOffTask.setIterations(1);
      if (!bubbleOffTask.enableDelayed(TASK_MINUTE))
      {
        dd::showValue(55);
        log_w("Unable to enable 'bubbleOffTask'");
      }
    }
  }
  else
  // Non "quiet-period" processing
  {
    // On the hour, every hour, turn off everything
    if (newHour)
    {
      log_v("On the non-quiet period hour, turn everything off");
      newHour = false;
      relays::waterOff();
      relays::airOff();
    }

    // At 15 mins past the hour switch it all back on
    if (quarterPast)
    {
      log_v("On the non-quiet 1/4 hour, turn everything on");
      quarterPast = false;
      relays::waterOn();
      relays::airOn();
    }
  }
}
