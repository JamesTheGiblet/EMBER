/*
 * avoidance.h - Obstacle avoidance behavior for EMBER
 */

#pragma once

#include <Arduino.h>
#include "globals.h"

// Public functions for avoidance behavior
void setupAvoidance();
void executeAvoidance();
bool checkObstacle();

// Cooldown for obstacle avoidance to prevent re-triggering
extern unsigned long last_avoidance_ms;
extern const unsigned long AVOIDANCE_COOLDOWN_MS;