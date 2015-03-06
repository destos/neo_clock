#include <Wire.h>
#include <RTClib.h>
#include <vcnl4000.h>
#include <Adafruit_NeoPixel.h>
#include "ClockTime.h"
#include "ClockSegements.h"
#include <Tween.h>

// define pins
#define NEOPIN 4
#define PIXELS 60

// #define DEBUG

// Brightness adjust settings and variables
#define MIN_BRIGHTNESS 100 // set minimum brightness
#define MAX_BRIGHTNESS 255// set max brightness
#define LOW_AMBIENT 40
#define HIGH_AMBIENT 350
#define TWEEN_LENGTH 4000 // tween length in ms
uint8_t target_brightness = MIN_BRIGHTNESS;
uint8_t current_brightness = target_brightness;

Tween_t brightness_chase;

// neopixel object
Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXELS, NEOPIN, NEO_GRB + NEO_KHZ800);

// Initialize our proximity/luminosity sensor
vcnl4000 sensor = vcnl4000();

ClockTime clock_time;
ClockSegments segments(strip, clock_time);

long brightness_start_ms;

void ambient_adjustments() {
    int ambient = sensor.readAmbient();
    // When bright in the room raise the brightness,
    target_brightness = constrain(
        map(ambient, LOW_AMBIENT, HIGH_AMBIENT, MIN_BRIGHTNESS, MAX_BRIGHTNESS), 0, 255);

    // Serial.print("ambient:");
    // Serial.println(ambient);
    // only start changing brightness when it moves widely
    if(target_brightness > (current_brightness + 5) || target_brightness < (current_brightness - 5) ){
        brightness_start_ms = millis();
        // Serial.println("starting tween");
        // Serial.print("target b:");
        // Serial.println(target_brightness);
        // tween
        Tween_line(&brightness_chase, (float) target_brightness, (float) TWEEN_LENGTH);
    }
}

void startup_animation(){
    for(int i=0; i<60; i++){
        strip.setPixelColor(i, strip.Color(0,0,4));
        strip.setPixelColor(i+1, strip.Color(0,4,8));
        strip.show();
        delay(40);
    }
}

void setup () {
    Serial.begin(9600);
    Wire.begin();  // Begin I2C
    // Serial.println("connected");
    clock_time.setup(); // get clock time
    // Serial.println("clock setup");
    sensor.setup();
    // Serial.println("sensor setup");

    // Tween_setType(&brightness_chase, TWEEN_EASEINOUTCUBIC);
    // run once so it starts off at target
    Tween_line(&brightness_chase, (float) target_brightness, (float) 1);
    current_brightness = Tween_tick(&brightness_chase, 1);

    strip.begin();
    strip.show(); // Initialize all pixels to 'off'

    strip.setBrightness(current_brightness); // set start overall brightness
    startup_animation();
}

// used to fire intermittent functions
long previous_time = 0;
long interval = 1000;

void loop () {

    long current_ms = millis();

    if(current_ms - previous_time > interval) {
        segments.calculate_colors();
        previous_time = current_ms;
        ambient_adjustments();
    }

    // Adjust the brightness!
    if(current_brightness != target_brightness){
        // change brightness with tween
        long adjust_diff = current_ms - brightness_start_ms;
        current_brightness = Tween_tick(&brightness_chase, adjust_diff);
        // Serial.print("current b:");
        // Serial.println(current_brightness);
        strip.setBrightness(current_brightness);
        // 25 fps
        delay(40);
    }else{
        // 1 fps
        delay(1000);
    }

    segments.draw();
}
