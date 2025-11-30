/*
 * actuators.h - Actuator interfaces for the EMBER project HAL.
 *
 * This file contains classes for controlling output devices like
 * RGB LEDs and motors.
 */

#pragma once

#include <Arduino.h>
#include "pins.h"
#include "config.h"

namespace HAL {

    class RGBLED {
    public:
        void begin() {
            ledcSetup(Config::PWM::CH_RED, Config::PWM::FREQ, Config::PWM::RESOLUTION);
            ledcSetup(Config::PWM::CH_GREEN, Config::PWM::FREQ, Config::PWM::RESOLUTION);
            ledcSetup(Config::PWM::CH_BLUE, Config::PWM::FREQ, Config::PWM::RESOLUTION);
            ledcAttachPin(Pins::LED_RED, Config::PWM::CH_RED);
            ledcAttachPin(Pins::LED_GREEN, Config::PWM::CH_GREEN);
            ledcAttachPin(Pins::LED_BLUE, Config::PWM::CH_BLUE);
        }
        void setRGB(uint8_t r, uint8_t g, uint8_t b) {
            ledcWrite(Config::PWM::CH_RED, r);
            ledcWrite(Config::PWM::CH_GREEN, g);
            ledcWrite(Config::PWM::CH_BLUE, b);
        }
        void red(uint8_t brightness = 255) { setRGB(brightness, 0, 0); }
        void green(uint8_t brightness = 255) { setRGB(0, brightness, 0); }
        void blue(uint8_t brightness = 255) { setRGB(0, 0, brightness); }
        void yellow(uint8_t brightness = 255) { setRGB(brightness, brightness, 0); }
        void magenta(uint8_t brightness = 255) { setRGB(brightness, 0, brightness); }
        void off() { setRGB(0, 0, 0); }
    };


} // namespace HAL