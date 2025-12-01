// avoidance.cpp - IMPROVED VERSION

#include "avoidance.h"
#include "hal.h"
#include "config.h"
#include "globals.h"

enum AvoidancePhase_t { AVOID_NONE, AVOID_BACKUP, AVOID_TURN, AVOID_VERIFY };
static AvoidancePhase_t avoidance_phase = AVOID_NONE;
static unsigned long avoidance_start_ms = 0;

unsigned long last_avoidance_ms = 0;
const unsigned long AVOIDANCE_COOLDOWN_MS = 500;  // Reduced cooldown

void setupAvoidance() {
    avoidance_phase = AVOID_NONE;
    last_avoidance_ms = 0;
}

bool checkObstacle() {
    static float last_valid_distance = 999.0f;
    float distance = HAL::ultrasonic.readDistance();
    
    // Filter invalid readings
    if (distance < 2.0f || distance > 400.0f) {
        distance = last_valid_distance;
    } else {
        last_valid_distance = distance;
    }
    
    if (distance < Config::Behavior::OBSTACLE_DISTANCE_CM) {
        if (millis() - last_avoidance_ms > AVOIDANCE_COOLDOWN_MS) {
            last_avoidance_ms = millis();
            return true;
        }
    }
    return false;
}

void executeAvoidance() {
    unsigned long now = millis();

    // If we are commanded to be idle (e.g. manual override), stop immediately.
    if (currentBehavior == IDLE) {
        avoidance_phase = AVOID_NONE;
        return;
    }
    
    // Energy-aware speeds
    int backupSpeed = 180;
    int turnSpeed = 200;
    
    if (energy < 30.0f) {
        backupSpeed = 120;
        turnSpeed = 140;
    } else if (battery.mode == POWER_ECONOMY) {
        backupSpeed = 140;
        turnSpeed = 160;
    }

    if (avoidance_phase == AVOID_NONE) {
        Serial.println("[Behavior] Obstacle! Starting avoidance...");
        avoidance_phase = AVOID_BACKUP;
        avoidance_start_ms = now;
        // Motor control removed
    }
    else if (avoidance_phase == AVOID_BACKUP) {
        if (now - avoidance_start_ms > 800) {  // Longer backup
            avoidance_phase = AVOID_TURN;
            avoidance_start_ms = now;
            bool turn_right = random(0, 2);
            Serial.printf("[Behavior] Turning %s...\n", turn_right ? "right" : "left");
            // Motor control removed
        }
    }
    else if (avoidance_phase == AVOID_TURN) {
        if (now - avoidance_start_ms > 600) {  // Longer turn
            avoidance_phase = AVOID_VERIFY;
            avoidance_start_ms = now;
            // Motor control removed
            Serial.println("[Behavior] Verifying clearance...");
        }
    }
    else if (avoidance_phase == AVOID_VERIFY) {
        // Brief pause to let ultrasonic settle
        if (now - avoidance_start_ms > 200) {
            float distance = HAL::ultrasonic.readDistance();
            
            Serial.printf("[Behavior] Verification distance: %.1fcm\n", distance);
            if (distance < Config::Behavior::OBSTACLE_DISTANCE_CM + 5.0f) {
                // Still too close - turn more!
                Serial.println("[Behavior] Still blocked, turning more...");
                avoidance_phase = AVOID_TURN;
                avoidance_start_ms = now;
                bool turn_right = random(0, 2);
                // Motor control removed
            } else {
                // Clear!
                avoidance_phase = AVOID_NONE;
                currentBehavior = IDLE;
                Serial.printf("[Behavior] Path clear (%.1fcm). Resuming.\n", distance);
            }
        }
    }
}