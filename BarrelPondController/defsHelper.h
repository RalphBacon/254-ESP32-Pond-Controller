#pragma once
#include <Arduino.h>

// Definitions for the 7-segment display
#define LEFT 0
#define RIGHT 1

// Output pins controlling the relays
#define AIR 26
#define WATER 13

// The ON/OFF state of the GPIO pins is "backwards" as the pins SINK the current
// so define some sanity-retaining names here
#define _AIR_OFF HIGH
#define _AIR_ON LOW
#define _WATER_OFF HIGH
#define _WATER_ON LOW

// LEDs for AIR and Water
#define redAir 16
#define greenAir 4
#define redWater 33
#define greenWater 17

// Macros
uint8_t QUIET_PERIOD_START = 17;
#define QUIET_PERIOD_END 8
#define AUTO_ON_MINS 10

// Current time structure
struct tm timeinfo;

// Counter for hours and minutes
uint8_t hoursTime = 0;
uint8_t minsTime = 0;
uint8_t secsTime = 0;

// Manual countdown timer
bool countDownTimerActive = false;
uint8_t minsCountDown = 0;
uint8_t secsCountDown = 0;

// Various time-specific flags
uint8_t prevHour = 99;
bool newHour = false;

bool prevQuarterPast = false;
bool quarterPast = false;

bool prevQuarterTo = false;
bool quarterTo = false;

bool prevHalfPast = false;
bool halfPast = false;

// First 15 minutes of every non-quiet period hour
bool feedingTime = false;

// No auto air/water during this period
bool prevQuietPeriod = false;
bool quietPeriod = false;

#define _TASK_SCHEDULING_OPTIONS
#include <TaskScheduler.h>
#include "ntpHelper.h"
#include "timerH.hpp"
#include "relayH.hpp"

// Scheduler
Scheduler schedule;

// Main Task
Task clockTask(1000, TASK_FOREVER, &timerH::timerCallBack);

// Switches off air pump after X minutes overnight (quiet period)
Task bubbleOffTask(0, 1, &relays::airOff);

// Switches it all on again after a 10 minute override
Task allBackOnTask(0, 1, &relays::airWaterOn);

#include "dualDigitHelper.h"
#include "wifiHelper.h"
#include "7H.hpp"
#include "serverH.hpp"
