/*
 * pins.h - Centralized GPIO pin assignments for the EMBER project.
 *
 * This file serves as the single source of truth for all hardware wiring.
 * If you change a connection, you only need to update it here.
 */

#pragma once

namespace Pins {


    // -- Light Sensors (LDRs) --
    // Note: Must be ADC1 pins (GPIOs 32-39)
    constexpr int LDR_LEFT  = 35;
    constexpr int LDR_RIGHT = 34;

    // -- RGB LED --
    constexpr int LED_RED   = 14;
    constexpr int LED_GREEN = 33;
    constexpr int LED_BLUE  = 12;

    // -- Ultrasonic Sensor --
    constexpr int US_TRIGGER = 26;
    constexpr int US_ECHO    = 25;

    // -- Power Management --
    constexpr int BATTERY_SENSE = 32; // ADC pin for battery voltage

    // -- Motor Driver (supports both L9110S and TB6612FNG) --
    constexpr int MOTOR_A_IN1 = 15; // Left Motor IN1
    constexpr int MOTOR_A_IN2 = 16;  // Left Motor IN2
    constexpr int MOTOR_B_IN1 = 17; // Right Motor IN1
    constexpr int MOTOR_B_IN2 = 18; // Right Motor IN2
    constexpr int MOTOR_A_EN  = 5;  // Left Motor Speed (PWM - for TB6612FNG)
    constexpr int MOTOR_B_EN  = 4;  // Right Motor Speed (PWM - for TB6612FNG)
    constexpr int MOTOR_STBY  = 13; // Standby pin for TB6612FNG

} // namespace Pins
