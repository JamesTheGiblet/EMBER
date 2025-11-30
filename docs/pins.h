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
    constexpr int LDR_LEFT  = 34;
    constexpr int LDR_RIGHT = 35;

    // -- RGB LED --
    constexpr int LED_RED   = 23;
    constexpr int LED_GREEN = 22;
    constexpr int LED_BLUE  = 21;

    // -- Motor Driver (supports both L9110S and TB6612FNG) --
    constexpr int MOTOR_A_IN1 = 15; // Left Motor IN1
    constexpr int MOTOR_A_IN2 = 2;  // Left Motor IN2
    constexpr int MOTOR_B_IN1 = 16; // Right Motor IN1
    constexpr int MOTOR_B_IN2 = 17; // Right Motor IN2
    constexpr int MOTOR_A_EN  = 5;  // Left Motor Speed (PWM - for TB6612FNG)
    constexpr int MOTOR_B_EN  = 4;  // Right Motor Speed (PWM - for TB6612FNG)
    constexpr int MOTOR_STBY  = 13; // Standby pin for TB6612FNG

} // namespace Pins