#ifndef CLOCKSEGMENTS_H
#define CLOCKSEGMENTS_H

#include "ClockTime.h"
#include <Adafruit_NeoPixel.h>

#if (ARDUINO >= 100)
 #include <Arduino.h>
#else
 #include <WProgram.h>
 #include <pins_arduino.h>
#endif

class ClockSegments
{
    public:
        ClockTime    &clock_time;
        Adafruit_NeoPixel &strip;

        ClockSegments(Adafruit_NeoPixel&, ClockTime&);
        void draw();
        void clear();
        void calculate_colors();
        void add_color(uint8_t position, uint32_t color, uint8_t pct=127);
        uint32_t blend(uint32_t color1, uint32_t color2, uint8_t pct=127);
    private:
        // not sure how to specifty segent array length dynamically
        uint32_t segments[60];
        uint32_t second_color, hour_color, minute_color, off_color;
};

#endif
