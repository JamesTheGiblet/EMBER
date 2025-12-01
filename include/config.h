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
        constexpr int CH_MOTOR_A = 3;
        constexpr int CH_MOTOR_B = 4;
    }

    // ADC calibration for light sensors
    namespace ADC {
        // The normalized ADC reading in darkness/light. Calibrated automatically.
        constexpr float DARK_READING_LEFT  = 0.726f;
        constexpr float LIGHT_READING_LEFT = 1.000f;
        constexpr float DARK_READING_RIGHT = 0.729f;
        constexpr float LIGHT_READING_RIGHT= 1.000f;
    }

    // LIFE SIMULATION PARAMETERS
    // ========================================================================
    namespace Life {
        // The universal cost of existence (energy lost per second).
        constexpr float ENERGY_DECAY = 0.1f;
        // The rate at which energy is gained from excess light (per second).
        constexpr float ENERGY_GAIN = 2.0f;
        // The multiplier for energy cost when moving (e.g., 1.5 = 50% extra cost).
        constexpr float MOVEMENT_COST_MULTIPLIER = 1.5f;
        // The maximum and minimum possible energy levels.
        constexpr float MAX_ENERGY = 100.0f;
        constexpr float MIN_ENERGY = 0.0f;
    }

    // ========================================================================
    // BEHAVIOR PARAMETERS
    // ========================================================================
    namespace Behavior {
        // Distance in cm to trigger the obstacle avoidance behavior.
        constexpr float OBSTACLE_DISTANCE_CM = 15.0f;
    }
} // namespace Config
