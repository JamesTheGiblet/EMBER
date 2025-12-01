/*
 * genome.h - Defines the genetic structure and life state for an EMBER bot.
 */

#pragma once
#include <stdint.h>

// This struct holds the genetic traits that can be evolved and persisted.
struct Genome {
    float light_threshold;  // How much light is needed to gain energy (0.0-1.0)
    float efficiency;       // How well light converts to energy (0.5-1.5)

    // --- v0.2 Movement Genes ---
    // How aggressively the bot turns towards light.
    float turn_sensitivity;
    // How fast the bot moves in general.
    int base_speed;

    uint8_t bot_id;         // The unique ID of this bot (0-8)
    uint32_t generation;     // The evolutionary generation number
};

// --- Global Instances ---

#include <Preferences.h>
// Declare a global genome instance. It will be defined and managed in main.cpp.
extern Genome genome;

// Declare a global preferences instance for saving/loading the genome.
extern Preferences preferences;