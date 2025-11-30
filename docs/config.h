/*
 * config.h - Software configuration and tuning parameters for EMBER.
 *
 * This file contains constants that define the bot's behavior, such as
 * PWM settings, energy dynamics, and network intervals.
 */

#pragma once

// ========================================================================
// PWM & ADC CONFIGURATION
// ========================================================================
namespace Config {
    // Configuration for LEDC (LED Control) peripheral
    namespace PWM {
        constexpr int FREQ = 5000;    // 5kHz PWM frequency
        constexpr int RESOLUTION = 8; // 8-bit resolution (0-255)

        // PWM channel assignments
        constexpr int CH_RED   = 0;
        constexpr int CH_GREEN = 1;
        constexpr int CH_BLUE  = 2;
    }
} // namespace Config