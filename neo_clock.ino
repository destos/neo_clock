#include <Tween.h>
#include <Wire.h>
#include <RTClib.h>
#include <vcnl4000.h>
#include <Adafruit_NeoPixel.h>

// define pins
#define NEOPIN 4
#define PIXELS 60

#define MIN_BRIGHTNESS 100 // set max brightness
uint8_t target_brightness = MIN_BRIGHTNESS;
uint8_t current_brightness = target_brightness;

Tween_t brightness_chase;

RTC_DS1307 RTC; // Establish clock object
DateTime Clock; // Holds current clock time

// const uint8_t gamma[] = {
//       0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
//       0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  1,  1,  1,
//       1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,  2,  3,  3,  3,  3,
//       3,  3,  4,  4,  4,  4,  5,  5,  5,  5,  5,  6,  6,  6,  6,  7,
//       7,  7,  8,  8,  8,  9,  9,  9, 10, 10, 10, 11, 11, 11, 12, 12,
//      13, 13, 13, 14, 14, 15, 15, 16, 16, 17, 17, 18, 18, 19, 19, 20,
//      20, 21, 21, 22, 22, 23, 24, 24, 25, 25, 26, 27, 27, 28, 29, 29,
//      30, 31, 31, 32, 33, 34, 34, 35, 36, 37, 38, 38, 39, 40, 41, 42,
//      42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57,
//      58, 59, 60, 61, 62, 63, 64, 65, 66, 68, 69, 70, 71, 72, 73, 75,
//      76, 77, 78, 80, 81, 82, 84, 85, 86, 88, 89, 90, 92, 93, 94, 96,
//      97, 99,100,102,103,105,106,108,109,111,112,114,115,117,119,120,
//     122,124,125,127,129,130,132,134,136,137,139,141,143,145,146,148,
//     150,152,154,156,158,160,162,164,166,168,170,172,174,176,178,180,
//     182,184,186,188,191,193,195,197,199,202,204,206,209,211,213,215,
//     218,220,223,225,227,230,232,235,237,240,242,245,247,250,252,255
// };

Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXELS, NEOPIN, NEO_GRB + NEO_KHZ800); // strip object

vcnl4000 sensor = vcnl4000();

// Color scheme
uint32_t second_color = strip.Color( 84,  30,  0); //slightly yellower
uint32_t hour_color   = strip.Color( 44,  42,  0); //yellow
uint32_t minute_color = strip.Color(  0,   0,  90); //red
uint32_t off_color    = strip.Color(  0,   0,  4);


class ClockPositions
{
 public:

  uint8_t milli;
  uint8_t currentsecond;
  uint8_t currentminute;
  uint8_t currenthour;

  ClockPositions();
  void update();
  void setup();
};

ClockPositions::ClockPositions(){

}

void ClockPositions::setup(){
    if (! RTC.isrunning()) {
        Serial.println("RTC is NOT running!");
        // following line sets the RTC to the date & time this sketch was compiled
        RTC.adjust(DateTime(__DATE__, __TIME__));
    }
}
void ClockPositions::update(){

    Clock = RTC.now(); // get the RTC time
    // setTime(Clock.hour(), Clock.minute(), Clock.second(), Clock.day(), Clock.month(), Clock.year());
    // adjustTime(offset * SECS_PER_HOUR);

    // currentsecond = second();
    // currenthour   = hour() % 12;
    // currentminute = map(minute() % 60, 0, 60, 0, 59);
    currentsecond = Clock.second();
    currenthour   = Clock.hour() % 12;
    currentminute = map(Clock.minute() % 60, 0, 60, 0, 59);
    // currentminute = minute();
}

/* CLOCK VIEW */

class ClockSegments
{
    public:
        ClockPositions    &positions;
        Adafruit_NeoPixel &strip;

        ClockSegments(Adafruit_NeoPixel&, ClockPositions&);
        void draw ();
        void clear();
        void add_color(uint8_t position, uint32_t color, uint8_t pct=127);
        // uint32_t adjust_brightness(uint32_t color, uint8_t brightness);
        uint32_t blend(uint32_t color1, uint32_t color2, uint8_t pct=127);
};


ClockSegments::ClockSegments(Adafruit_NeoPixel& n_strip, ClockPositions& n_positions): strip(n_strip), positions(n_positions){}


void ClockSegments::draw(){
    clear();
    // minute arm slowly moves
    uint8_t adjust_by = map(positions.currentsecond, 0, 59, 0, 255);
    add_color(positions.currentminute, minute_color, adjust_by);
    int next_min_pos = (positions.currentminute < 59) ? positions.currentminute + 1 : 0;
    add_color(next_min_pos, minute_color, ~adjust_by);

    // Hour segment takes up entire hour span
    int hour_segment = strip.numPixels()/12;
    for(int i=0; i<hour_segment; i++){
        add_color(positions.currenthour*hour_segment+i, hour_color);
    }
    // second arm ticks
    add_color(positions.currentsecond, second_color);
    strip.show();
}


void ClockSegments::add_color(uint8_t position, uint32_t color, uint8_t pct){
    uint32_t blended_color = blend(strip.getPixelColor(position), color, pct);

    strip.setPixelColor(position, blended_color);
}

// uint32_t ClockSegments::adjust_brightness(uint32_t color, uint8_t brightness){
//     uint8_t r,b,g;

//     r = ((uint8_t)(color >> 16) * brightness) >> 8,
//     g = ((uint8_t)(color >>  8) * brightness) >> 8,
//     b = ((uint8_t)(color >>  0) * brightness) >> 8;

//     return strip.Color(constrain(r, 0, 255), constrain(g, 0, 255), constrain(b, 0, 255));
// }

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

    // Serial.println("----");
    // Serial.println(pct);
    // Serial.print(" r1");
    // Serial.print(r1);
    // Serial.print(" g1");
    // Serial.print(g1);
    // Serial.print(" b1");
    // Serial.println(b1);

    // Serial.print(" r2:");
    // Serial.print(r2);
    // Serial.print(" g2:");
    // Serial.print(g2);
    // Serial.print(" b2:");
    // Serial.println(b2);

    r3 = (uint8_t)(((uint16_t)r1*pct + (uint16_t)r2*(255-pct))/255);
    g3 = (uint8_t)(((uint16_t)g1*pct + (uint16_t)g2*(255-pct))/255);
    b3 = (uint8_t)(((uint16_t)b1*pct + (uint16_t)b2*(255-pct))/255);

    // Serial.print(" r3:");
    // Serial.print(r3);
    // Serial.print(" g3:");
    // Serial.print(g3);
    // Serial.print(" b3:");
    // Serial.println(b3);

    return strip.Color(r3, g3, b3);
}

void ClockSegments::clear()
{
    for(uint16_t i=0; i<strip.numPixels(); i++) {
        strip.setPixelColor(i, off_color);
    }
}

ClockPositions positions;
ClockSegments segments(strip, positions);

long brightness_start_ms;

void ambient_adjustments() {
    int ambient = sensor.readAmbient();
    Serial.println(ambient);
    // When bright in the room raise the brightness,
    target_brightness = constrain(map(ambient, 40, 250, MIN_BRIGHTNESS, 255), MIN_BRIGHTNESS, 255);

    brightness_start_ms = millis();
    // adjust over second
    Tween_line(&brightness_chase, (float) target_brightness, (float) 1000);
}

void setup () {
    Serial.begin(9600);

    Wire.begin();  // Begin I2C
    RTC.begin();   // begin clock
    positions.setup(); // get clock time
    sensor.setup();

    strip.begin();
    strip.show(); // Initialize all pixels to 'off'
    strip.setBrightness(MIN_BRIGHTNESS); // set overall brightness
}

// used to fire intermitent functions
long previous_time = 0;
long interval = 1000;

void loop () {

    long current_ms = millis();

    if(current_ms - previous_time > interval) {
        positions.update();

        previous_time = current_ms;
        ambient_adjustments();
    }

    // change brightness with tween
    current_brightness = (int) Tween_tick(&brightness_chase, (current_ms - brightness_start_ms));
    // Serial.println(current_brightness);

    segments.draw();
    strip.setBrightness(current_brightness);
    delay(20);
}
