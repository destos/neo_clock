#include "ClockSegements.h"
#include "ClockTime.h"

ClockSegments::ClockSegments(Adafruit_NeoPixel& n_strip, ClockTime& n_time): strip(n_strip), clock_time(n_time){
    // Color scheme
    second_color = strip.Color( 84,  30,  0); // orange
    hour_color   = strip.Color( 44,  42,  0); // yellow
    minute_color = strip.Color(  0,   0, 90); // bright blue
    off_color    = strip.Color(  0,   0,  4); // light blue

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

    // minute arm slowly moves
    uint8_t adjust_by = map(clock_time.currentsecond, 0, 59, 0, 255);
    add_color(clock_time.currentminute, minute_color, adjust_by);
    int next_min_pos = (clock_time.currentminute < 59) ? clock_time.currentminute + 1 : 0;
    add_color(next_min_pos, minute_color, ~adjust_by);

    // Hour segment takes up entire hour span
    int hour_segment = strip.numPixels()/12;
    for(int i=0; i<hour_segment; i++){
        add_color(clock_time.currenthour*hour_segment+i, hour_color);
    }

    // second arm ticks
    add_color(clock_time.currentsecond, second_color);
}


void ClockSegments::add_color(uint8_t position, uint32_t color, uint8_t pct){
    uint32_t blended_color = blend(strip.getPixelColor(position), color, pct);
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
