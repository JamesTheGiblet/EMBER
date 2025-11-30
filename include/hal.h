#pragma once

// Minimal HAL for LED control (expand as needed)
#include <Arduino.h>

class RGBLED {
public:
    static constexpr int PIN_RED = 14;
    static constexpr int PIN_GREEN = 33;
    static constexpr int PIN_BLUE = 12;

    void begin() {
        pinMode(PIN_RED, OUTPUT);
        pinMode(PIN_GREEN, OUTPUT);
        pinMode(PIN_BLUE, OUTPUT);
    }
    void setRGB(uint8_t r, uint8_t g, uint8_t b) {
        analogWrite(PIN_RED, r);
        analogWrite(PIN_GREEN, g);
        analogWrite(PIN_BLUE, b);
    }
    void red(uint8_t brightness = 255) { setRGB(brightness, 0, 0); }
    void green(uint8_t brightness = 255) { setRGB(0, brightness, 0); }
    void blue(uint8_t brightness = 255) { setRGB(0, 0, brightness); }
    void yellow(uint8_t brightness = 255) { setRGB(brightness, brightness, 0); }
    void magenta(uint8_t brightness = 255) { setRGB(brightness, 0, brightness); }
    void off() { setRGB(0, 0, 0); }
};

extern RGBLED led;
