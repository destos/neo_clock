#include "ClockSegements.h"
#include "ClockTime.h"

ClockSegments::ClockSegments(Adafruit_NeoPixel& n_strip, ClockTime& n_time): strip(n_strip), clock_time(n_time){
    // Color scheme
    second_color = strip.Color( 84,  30,  0); // orange
    hour_color   = strip.Color( 44,  42,  0); // yellow
    minute_color = strip.Color(  0,   0, 90); // bright blue
    off_color    = strip.Color(  0,   10,  4); // light green

    // segments[16]
}

void ClockSegments::draw(){
    // loops through strip and renders segments
    int strip_length = strip.numPixels();
    for (int i=0; i<strip_length; ++i){
        strip.setPixelColor(i, segments[i]);
    }
    strip.show();
}

void ClockSegments::calculate_colors(){
    // get latest time
    clock_time.update();

    // clear previous colors before adding our own
    clear();

    int minute = clock_time.currentminute;
    int second = clock_time.currentsecond;
    int hour = clock_time.currenthour;

    // minute arm slowly moves
    uint8_t adjust_by = map(second, 0, 59, 0, 255);
    add_color(minute, minute_color, adjust_by);
    int next_min_pos = (minute < 59) ? minute + 1 : 0;
    add_color(next_min_pos, minute_color, ~adjust_by);

    // Hour segment takes up entire hour span
    int hour_segment = strip.numPixels()/12;

#ifdef DEBUG
    Serial.print("minute:");
    Serial.println(minute);
#endif

    for(int i=0; i<hour_segment+1; i++){
        // current LED power, moves with minutes
        int hour_seg_to_seconds = map(i, 0, 5, 1, 60*60);
        int hour_seg_off = abs(hour_seg_to_seconds - ((minute * 60) + second));
        int hour_power = map(hour_seg_off, 0, 60*60, 30, 255);

#ifdef DEBUG
        Serial.print("seg:");
        Serial.print(hour*hour_segment+i);
        Serial.print(" seg_to_min:");
        Serial.print(hour_seg_to_seconds);
        Serial.print(" seg_off:");
        Serial.print(hour_seg_off);
        Serial.print(" power:");
        Serial.println(hour_power);
#endif
        add_color(hour*hour_segment+i, hour_color, hour_power);
    }

    // second arm tick
    add_color(second, second_color);
}


void ClockSegments::add_color(uint8_t position, uint32_t color, uint8_t pct){
    uint32_t blended_color = blend(segments[position], color, pct);
    segments[position] = blended_color;
}

void ClockSegments::clear(){
    for(uint16_t i=0; i<strip.numPixels(); i++) {
        segments[i] = off_color;
    }
}

uint32_t ClockSegments::blend(uint32_t color1, uint32_t color2, uint8_t pct){
    uint8_t r1,g1,b1;
    uint8_t r2,g2,b2;
    uint8_t r3,g3,b3;

    r1 = (uint8_t)(color1 >> 16),
    g1 = (uint8_t)(color1 >>  8),
    b1 = (uint8_t)(color1 >>  0);

    r2 = (uint8_t)(color2 >> 16),
    g2 = (uint8_t)(color2 >>  8),
    b2 = (uint8_t)(color2 >>  0);

    r3 = (uint8_t)(((uint16_t)r1*pct + (uint16_t)r2*(255-pct))/255);
    g3 = (uint8_t)(((uint16_t)g1*pct + (uint16_t)g2*(255-pct))/255);
    b3 = (uint8_t)(((uint16_t)b1*pct + (uint16_t)b2*(255-pct))/255);

#ifdef DEBUG
    Serial.println("----");
    Serial.println(pct);
    Serial.print(" r1");
    Serial.print(r1);
    Serial.print(" g1");
    Serial.print(g1);
    Serial.print(" b1");
    Serial.println(b1);

    Serial.print(" r2:");
    Serial.print(r2);
    Serial.print(" g2:");
    Serial.print(g2);
    Serial.print(" b2:");
    Serial.println(b2);

    Serial.print(" r3:");
    Serial.print(r3);
    Serial.print(" g3:");
    Serial.print(g3);
    Serial.print(" b3:");
    Serial.println(b3);
#endif

     return strip.Color(r3, g3, b3);
}
