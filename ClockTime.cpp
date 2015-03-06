#include "ClockTime.h"
#include <RTClib.h>

RTC_DS1307 RTC; // Establish clock object
DateTime Clock; // Holds current clock time

ClockTime::ClockTime(){
}

void ClockTime::setup(){
    RTC.begin();   // begin clock
    if (! RTC.isrunning()) {
        Serial.println("RTC is NOT running!");
        // following line sets the RTC to the date & time this sketch was compiled
        RTC.adjust(DateTime(__DATE__, __TIME__));
    }
}
void ClockTime::update(){
    Clock = RTC.now(); // get the RTC time
    currentsecond = Clock.second();
    currenthour   = Clock.hour() % 12;
    currentminute = Clock.minute() % 60;
}
