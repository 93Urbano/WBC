//************************************//
//              RTC.hpp               //
//************************************//

// Date and time functions using a DS3231 RTC connected via I2C and Wire lib
#include "RTClib.h"
#include <Arduino.h>

#ifndef RTCMOD_HPP
#define RTCMOD_HPP

RTC_DS3231 rtc;

#endif

class RTCmodule
{
    private:
        //No private methods

    public:
        static void RTCInit();
        static void RTCGetDateTime();
        static String GetTimeString();
        static String GetDateFileName();
};