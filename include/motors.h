/*
 * motors.h - Motor driver abstraction for EMBER
 *
 * This file provides a simple interface for controlling the left and right motors.
 */

#pragma once

#include <Arduino.h>
#include "pins.h"
#include "config.h"

namespace HAL {
    class MotorDriver {
    public:
        // Set speed for left and right motors. Speed is -255 to 255.
        void setSpeeds(int leftSpeed, int rightSpeed) {
            setLeft(leftSpeed);
            setRight(rightSpeed);
        }
        void begin() {
            pinMode(Pins::MOTOR_A_IN1, OUTPUT);
            pinMode(Pins::MOTOR_A_IN2, OUTPUT);
            pinMode(Pins::MOTOR_B_IN1, OUTPUT);
            pinMode(Pins::MOTOR_B_IN2, OUTPUT);
            pinMode(Pins::MOTOR_A_EN, OUTPUT);
            pinMode(Pins::MOTOR_B_EN, OUTPUT);
            pinMode(Pins::MOTOR_STBY, OUTPUT);
            digitalWrite(Pins::MOTOR_STBY, HIGH); // Enable driver
        }

        void setLeft(int speed) {
            // speed: -255 (reverse) to 255 (forward)
            if (speed >= 0) {
                digitalWrite(Pins::MOTOR_A_IN1, HIGH);
                digitalWrite(Pins::MOTOR_A_IN2, LOW);
            } else {
                digitalWrite(Pins::MOTOR_A_IN1, LOW);
                digitalWrite(Pins::MOTOR_A_IN2, HIGH);
            }
            ledcWrite(Pins::MOTOR_A_EN, abs(speed));
        }

        void setRight(int speed) {
            // speed: -255 (reverse) to 255 (forward)
            if (speed >= 0) {
                digitalWrite(Pins::MOTOR_B_IN1, HIGH);
                digitalWrite(Pins::MOTOR_B_IN2, LOW);
            } else {
                digitalWrite(Pins::MOTOR_B_IN1, LOW);
                digitalWrite(Pins::MOTOR_B_IN2, HIGH);
            }
            ledcWrite(Pins::MOTOR_B_EN, abs(speed));
        }

        void stop() {
            setLeft(0);
            setRight(0);
        }
    };
}
