/*
 * seeking.cpp - Light-seeking behavior implementation
 */

#include "seeking.h"
#include "hal.h"
#include "genome.h"
#include "globals.h"

void executeSeeking() {
    float leftLight = HAL::lightSensor.readLeft();
    float rightLight = HAL::lightSensor.readRight();
    float error = leftLight - rightLight;
    float abs_error = abs(error);
    
    // Adjust speed based on power mode
    int baseSpeed = genome.base_speed;
    if (battery.mode == POWER_ECONOMY) baseSpeed = (baseSpeed * 3) / 4;
    
    // Only turn if difference is significant (reduces jitter)
    if (abs_error > 0.05f) {  // 5% threshold
        int turnSpeed = constrain((int)(error * genome.turn_sensitivity), -100, 100);
        HAL::motors.setSpeeds(baseSpeed - turnSpeed, baseSpeed + turnSpeed);
    } else {
        // Go straight if light is roughly equal
        HAL::motors.setSpeeds(baseSpeed, baseSpeed);
    }
}