/*
 * avoidance.cpp - Obstacle avoidance behavior implementation
 */

#include "avoidance.h"
#include "hal.h"
#include "config.h"

// Internal state for the avoidance state machine
enum AvoidancePhase_t { AVOID_NONE, AVOID_BACKUP, AVOID_TURN };
static AvoidancePhase_t avoidance_phase = AVOID_NONE;
static unsigned long avoidance_start_ms = 0;

// Cooldown for obstacle avoidance to prevent re-triggering
unsigned long last_avoidance_ms = 0;
const unsigned long AVOIDANCE_COOLDOWN_MS = 1000;

void setupAvoidance() {
    avoidance_phase = AVOID_NONE;
    last_avoidance_ms = 0;
}

bool checkObstacle() {
    float distance = HAL::ultrasonic.readDistance();
    if (distance > 0 && distance < Config::Behavior::OBSTACLE_DISTANCE_CM) {
        // Only trigger if not in cooldown
        if (millis() - last_avoidance_ms > AVOIDANCE_COOLDOWN_MS) {
            last_avoidance_ms = millis(); // Set cooldown
            return true;
        }
    }
    return false;
}

void executeAvoidance() {
    unsigned long now = millis();

    // Start avoidance sequence
    if (avoidance_phase == AVOID_NONE) {
        Serial.println("[Behavior] Obstacle detected! Starting avoidance...");
        avoidance_phase = AVOID_BACKUP;
        avoidance_start_ms = now;
        HAL::motors.setSpeeds(-180, -180); // Start backing up
    }
    // Execute backup phase
    else if (avoidance_phase == AVOID_BACKUP) {
        if (now - avoidance_start_ms > 400) {
            // Backup complete, start turn
            avoidance_phase = AVOID_TURN;
            avoidance_start_ms = now;
            bool turn_right = random(0, 2); // 0 for left, 1 for right
            Serial.printf("[Behavior] Turning %s to avoid obstacle...\n", turn_right ? "right" : "left");
            HAL::motors.setSpeeds(turn_right ? 200 : -200, turn_right ? -200 : 200);
        }
    }
    // Execute turn phase
    else if (avoidance_phase == AVOID_TURN) {
        if (now - avoidance_start_ms > 300) {
            HAL::motors.stop();
            avoidance_phase = AVOID_NONE;
            currentBehavior = IDLE; // Allow re-evaluation
            Serial.println("[Behavior] Avoidance complete.");
        }
    }
}