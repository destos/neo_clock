#ifndef CLOCKTIME_H
#define CLOCKTIME_H

#if (ARDUINO >= 100)
 #include <Arduino.h>
#else
 #include <WProgram.h>
 #include <pins_arduino.h>
#endif

#include <RTClib.h>

class ClockTime
{
    public:
        ClockTime();
        uint8_t currentsecond;
        uint8_t currentminute;
        uint8_t currenthour;

        void update();
        void setup();
};

#endif
