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


} // namespace Pins
