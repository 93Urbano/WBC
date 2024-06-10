//************************************//
//              RTC.h                 //
//************************************//

// Date and time functions using a DS3231 RTC connected via I2C and Wire lib
#include "RTClib.h"
#include "I2C.hpp"

RTC_DS3231 rtc;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
