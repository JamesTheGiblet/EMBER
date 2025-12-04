#include "movement.h"

Movement::Movement(HAL& halRef, MotorConfig& configRef) 
    : hal(halRef), config(configRef) {
    state.speedA = 0;
    state.speedB = 0;
    state.directionA = true;
    state.directionB = true;
    state.moving = false;
}

// ============================================================================
// INTERNAL HELPERS
// ============================================================================

int Movement::getSpeed(int requested) {
    return (requested == -1) ? config.baseSpeed : requested;
}

void Movement::setMotors(int speedA, bool dirA, int speedB, bool dirB) {
    hal.setMotorA(speedA, dirA);
    hal.setMotorB(speedB, dirB);
    
    // Update state
    state.speedA = speedA;
    state.speedB = speedB;
    state.directionA = dirA;
    state.directionB = dirB;
    state.moving = (speedA > 0 || speedB > 0);
}

void Movement::rampSpeed(int fromSpeed, int toSpeed, bool forward) {
    if (fromSpeed < toSpeed) {
        // Ramp up
        for (int speed = fromSpeed; speed <= toSpeed; speed += 5) {
            hal.setMotorA(speed, forward);
            hal.setMotorB(speed, forward);
            delay(20);
        }
    } else {
        // Ramp down
        for (int speed = fromSpeed; speed >= toSpeed; speed -= 5) {
            hal.setMotorA(speed, forward);
            hal.setMotorB(speed, forward);
            delay(20);
        }
    }
    
    state.speedA = toSpeed;
    state.speedB = toSpeed;
    state.directionA = forward;
    state.directionB = forward;
    state.moving = (toSpeed > 0);
}

// ============================================================================
// BASIC MOVEMENTS
// ============================================================================

void Movement::stop() {
    hal.stopMotors();
    state.speedA = 0;
    state.speedB = 0;
    state.moving = false;
}

void Movement::forward(int speed) {
    speed = getSpeed(speed);
    setMotors(speed, true, speed, true);
}

void Movement::backward(int speed) {
    speed = getSpeed(speed);
    setMotors(speed, false, speed, false);
}

void Movement::turnLeft(int speed) {
    speed = getSpeed(speed);
    setMotors(speed / 2, true, speed, true);
}

void Movement::turnRight(int speed) {
    speed = getSpeed(speed);
    setMotors(speed, true, speed / 2, true);
}

void Movement::spinCW(int speed) {
    speed = getSpeed(speed);
    setMotors(speed, true, speed, false);
}

void Movement::spinCCW(int speed) {
    speed = getSpeed(speed);
    setMotors(speed, false, speed, true);
}

void Movement::crawl() { 
    forward(config.crawlSpeed); 
}

void Movement::run() { 
    forward(config.maxSpeed); 
}

// ============================================================================
// SMOOTH MOVEMENTS
// ============================================================================

void Movement::smoothStart(int targetSpeed) {
    if (targetSpeed == -1) targetSpeed = config.baseSpeed;
    
    // Only ramp if we're starting from stop or slow speed
    int currentMax = max(state.speedA, state.speedB);
    
    if (currentMax < targetSpeed) {
        rampSpeed(currentMax, targetSpeed, true);
    } else {
        // Already at speed, just set it
        forward(targetSpeed);
    }
}

void Movement::smoothStop() {
    // Get the current maximum speed and direction
    int currentSpeed = max(abs(state.speedA), abs(state.speedB));
    
    if (currentSpeed == 0) {
        stop();
        return;
    }
    
    // Check if both motors are moving in the same direction
    bool movingForward = state.directionA && state.directionB;
    bool movingBackward = !state.directionA && !state.directionB;
    
    if (movingForward || movingBackward) {
        // Simple case: both motors same direction
        rampSpeed(currentSpeed, 0, movingForward);
    } else {
        // Complex case: spinning or turning - just stop quickly
        for (int i = 0; i < 10; i++) {
            int newSpeedA = state.speedA * (10 - i) / 10;
            int newSpeedB = state.speedB * (10 - i) / 10;
            hal.setMotorA(newSpeedA, state.directionA);
            hal.setMotorB(newSpeedB, state.directionB);
            delay(20);
        }
    }
    
    stop();
}

void Movement::smoothForward(int speed) {
    speed = getSpeed(speed);
    
    // If we're currently stopped or moving forward, smooth ramp
    if (!state.moving || (state.directionA && state.directionB)) {
        int currentSpeed = max(state.speedA, state.speedB);
        rampSpeed(currentSpeed, speed, true);
    } else {
        // We're moving backward or spinning - stop first, then start
        smoothStop();
        delay(100);
        smoothStart(speed);
    }
}

void Movement::smoothBackward(int speed) {
    speed = getSpeed(speed);
    
    // If we're currently stopped or moving backward, smooth ramp
    if (!state.moving || (!state.directionA && !state.directionB)) {
        int currentSpeed = max(state.speedA, state.speedB);
        rampSpeed(currentSpeed, speed, false);
    } else {
        // We're moving forward or spinning - stop first, then start
        smoothStop();
        delay(100);
        rampSpeed(0, speed, false);
    }
}

// ============================================================================
// PROPORTIONAL TURNING
// ============================================================================

void Movement::setVeer(int baseSpeed, int turnAmount) {
    // This function allows for gentle, proportional turns (veering).
    // A negative turnAmount veers left (slowing the right motor).
    // A positive turnAmount veers right (slowing the left motor).
    int speedA = baseSpeed;
    int speedB = baseSpeed;

    if (turnAmount > 0) { // Veer right
        speedA -= turnAmount;
    } else { // Veer left
        speedB += turnAmount; // turnAmount is negative, so this is subtraction
    }
    setMotors(constrain(speedA, 0, 255), true, constrain(speedB, 0, 255), true);
}

// ============================================================================
// STATE QUERIES
// ============================================================================

bool Movement::isMoving() {
    return state.moving;
}

int Movement::getCurrentSpeed() {
    return max(state.speedA, state.speedB);
}