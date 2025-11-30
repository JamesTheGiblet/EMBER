/*
 * sensors.h - Sensor interfaces for the EMBER project HAL.
 *
 * This file contains the classes for interacting with various sensors
 * like light-dependent resistors (LDRs) and ultrasonic distance sensors.
 */

#pragma once

#include <Arduino.h>
#include "pins.h"
#include "config.h"

namespace HAL {

    class LightSensor {
    public:
        void begin() {
            pinMode(Pins::LDR_LEFT, INPUT);
            pinMode(Pins::LDR_RIGHT, INPUT);
        }

        // Reads the left sensor and returns a calibrated 0.0-1.0 value.
        float readLeft() {
            float raw_normalized = analogRead(Pins::LDR_LEFT) / 4095.0f;
            float mapped = map(raw_normalized * 1000, Config::ADC::DARK_READING_LEFT * 1000, Config::ADC::LIGHT_READING_LEFT * 1000, 0, 1000) / 1000.0f;
            return constrain(mapped, 0.0f, 1.0f);
        }

        // Reads the right sensor and returns a calibrated 0.0-1.0 value.
        float readRight() {
            float raw_normalized = analogRead(Pins::LDR_RIGHT) / 4095.0f;
            float mapped = map(raw_normalized * 1000, Config::ADC::DARK_READING_RIGHT * 1000, Config::ADC::LIGHT_READING_RIGHT * 1000, 0, 1000) / 1000.0f;
            return constrain(mapped, 0.0f, 1.0f);
        }

        // Returns the average of the two calibrated sensor readings.
        float readAverage() {
            return (readLeft() + readRight()) / 2.0f;
        }
    };

    class UltrasonicSensor {
    public:
        UltrasonicSensor() {}

        void begin() {
            pinMode(Pins::US_TRIGGER, OUTPUT);
            pinMode(Pins::US_ECHO, INPUT);
            digitalWrite(Pins::US_TRIGGER, LOW);
        }

        // Returns distance in cm, or -1.0 on timeout.
        float readDistance() {
            // Trigger a 10-microsecond pulse to start the measurement.
            digitalWrite(Pins::US_TRIGGER, HIGH);
            delayMicroseconds(10);
            digitalWrite(Pins::US_TRIGGER, LOW);
            
            // Read the echo pulse duration (in microseconds).
            // A timeout of 30000us is ~5 meters, which is more than enough.
            long duration = pulseIn(Pins::US_ECHO, HIGH, 30000);
            
            // Calculate distance: Speed of sound is ~0.0343 cm/us.
            // The sound travels to the object and back, so we divide the total time by 2.
            return (duration > 0) ? (duration * 0.0343f / 2.0f) : -1.0f;
        }
    };

} // namespace HAL