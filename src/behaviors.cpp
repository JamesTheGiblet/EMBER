#include "behaviors.h"
ObstacleAvoidance::ObstacleAvoidance(HAL& halRef, Movement& movRef, 
                                     UltrasonicSensor& sensRef, StatusLED& statRef,
                                     MotorConfig& cfg)
    : hal(halRef), movement(movRef), sensor(sensRef), status(statRef), config(cfg) {
}

void ObstacleAvoidance::enable() {
    enabled = true;
    setState(EXPLORING);
    Serial.println("🤖 Autonomous mode ENABLED");
}

void ObstacleAvoidance::disable() {
    enabled = false;
    movement.stop();
    setState(IDLE);
    status.setStatus(StatusLED::READY);
    Serial.println("⏹ Autonomous mode DISABLED");
}

bool ObstacleAvoidance::isEnabled() {
    return enabled;
}

ObstacleAvoidance::State ObstacleAvoidance::getState() {
    return currentState;
}

void ObstacleAvoidance::setState(State newState) {
    currentState = newState;
    stateStartTime = millis();
}

void ObstacleAvoidance::update() {
    if (!enabled) return;
    
    // State machine
    switch (currentState) {
        case IDLE:
            // Should never be here if enabled
            break;
            
        case EXPLORING:
            handleExploring();
            break;
            
        case OBSTACLE_DETECTED:
            handleObstacleDetected();
            break;
            
        case BACKING_UP:
            handleBackingUp();
            break;
            
        case TURNING:
            handleTurning();
            break;
            
        case STUCK_ESCAPE:
            handleStuckEscape();
            break;
    }
}

void ObstacleAvoidance::handleExploring() {
    status.setStatus(StatusLED::MOVING);
    
    int distance = sensor.getDistance();
    
    // static unsigned long lastDebug = 0;
    // if (millis() - lastDebug > 1000) {
    //     Serial.printf("[DEBUG] Exploring: dist=%d, moving=%s, speed=%d\n", 
    //                   distance, 
    //                   movement.isMoving() ? "YES" : "NO",
    //                   movement.getCurrentSpeed());
    //     lastDebug = millis();
    // }

    // --- PRIORITY 0: OBSTACLE AVOIDANCE ---
    // This is the highest priority behavior. If an obstacle is detected,
    // we immediately change state and do not execute any lower-priority behaviors.
    if (sensor.obstacleDetected()) {
        Serial.printf("🛑 Obstacle detected at %d cm\n", distance);
        setState(OBSTACLE_DETECTED);
        return;
    }
    
    // --- PRIORITY 0.5: STUCK DETECTION ---
    // This is also a high-priority safety behavior.
    if (sensor.isStuck()) {
        Serial.println("⚠ STUCK - can't get away from obstacle!");
        setState(STUCK_ESCAPE);
        return;
    }
    
    // --- PRIORITY 1: PHOTOTROPISM (Light Seeking) ---
    // This behavior only runs if no obstacles are detected.
    int leftLDR = hal.readLDR_Left();
    int rightLDR = hal.readLDR_Right();
    int diff = leftLDR - rightLDR;
    const int LDR_THRESHOLD = 200; // How much difference is significant

    if (abs(diff) > LDR_THRESHOLD) {
        // There is a significant light difference. Turn towards it.
        int baseSpeed = config.baseSpeed;
        // The more the difference, the sharper the turn.
        int turnAmount = map(abs(diff), LDR_THRESHOLD, 4095, 20, baseSpeed);

        // diff > 0 means left is brighter, so we pass a negative turn amount to veer left.
        movement.setVeer(baseSpeed, -diff);
    } else {
        // --- PRIORITY 2: EXPLORATION ---
        // No obstacles and no significant light source. Just explore.
        // We can still use distance to modulate speed.
        if (sensor.obstacleFar()) {
            movement.crawl();
        } else {
            movement.smoothForward(config.baseSpeed);
        }
    }
}

void ObstacleAvoidance::handleObstacleDetected() {
    status.setStatus(StatusLED::OBSTACLE);
    movement.smoothStop();
    delay(200);
    
    // Start backing up
    Serial.println("← Backing up...");
    setState(BACKING_UP);
}

void ObstacleAvoidance::handleBackingUp() {
    status.setStatus(StatusLED::OBSTACLE);
    movement.backward(config.crawlSpeed);
    
    // Back up for 800ms or until clear
    if (millis() - stateStartTime > 800 || sensor.getDistance() > 50) {
        movement.stop();
        delay(100);
        
        // Alternate turn direction for variety
        turnDirection *= -1;
        
        Serial.printf("↻ Turning %s...\n", turnDirection > 0 ? "right" : "left");
        setState(TURNING);
    }
}

void ObstacleAvoidance::handleTurning() {
    status.setStatus(StatusLED::OBSTACLE);
    
    // Only do the scan once when we first enter turning state
    if (millis() - stateStartTime < 100) {
        // Just entered turning state, start the scan
        Serial.println("🔍 Scanning for clearer path...");
        
        // Look left
        movement.spinCCW(config.baseSpeed);
        delay(config.turnDuration / 2);
        movement.stop();
        delay(50);
        sensor.update();
        int leftDist = sensor.getDistance();
        
        // Return to center and look right
        movement.spinCW(config.baseSpeed);
        delay(config.turnDuration / 2); // Back to center
        delay(config.turnDuration / 2); // Now to the right
        movement.stop();
        delay(50);
        sensor.update();
        int rightDist = sensor.getDistance();
        
        Serial.printf("  [SCAN] Left: %d cm, Right: %d cm\n", leftDist, rightDist);
        
        // Return to center before final turn
        movement.spinCCW(config.baseSpeed);
        delay(config.turnDuration / 2);
        movement.stop();
        delay(100);
        
        // Decide which way to turn
        if (leftDist > rightDist) {
            turnDirection = -1;
            Serial.println("  ↺ LEFT is clearer");
        } else {
            turnDirection = 1;
            Serial.println("  ↻ RIGHT is clearer");
        }
        
        // Reset timer for the actual turn
        stateStartTime = millis();
    }
    
    // Perform the turn in the chosen direction
    if (millis() - stateStartTime < config.turnDuration) {
        // Still turning
        if (turnDirection > 0) {
            movement.spinCW(config.baseSpeed);
        } else {
            movement.spinCCW(config.baseSpeed);
        }
    } else {
        // Turn complete, check if clear
        movement.stop();
        delay(100);
        
        // Verify path
        sensor.update();
        delay(50);
        sensor.update();
        delay(50);
        sensor.update();
        
        int finalDist = sensor.getDistance();
        
        if (finalDist > 50) {
            Serial.printf("✓ Path clear (%d cm), resuming\n", finalDist);
            setState(EXPLORING);
            movement.smoothForward(config.baseSpeed);
        } else {
            Serial.printf("⚠ Still blocked (%d cm), turning 90° more\n", finalDist);
            // Turn another 90 degrees in same direction
            if (turnDirection > 0) {
                movement.spinCW(config.baseSpeed);
            } else {
                movement.spinCCW(config.baseSpeed);
            }
            delay(config.turnDuration);
            stateStartTime = millis(); // Check again next cycle
        }
    }
}

void ObstacleAvoidance::handleStuckEscape() {
    status.setStatus(StatusLED::ERROR);
    Serial.println("🆘 Executing stuck escape maneuver...");
    
    // Aggressive escape sequence
    movement.backward(config.maxSpeed);
    delay(1000);
    
    movement.stop();
    delay(200);
    
    // Spin 180 degrees
    movement.spinCW(config.maxSpeed);
    delay(config.turnDuration * 2);
    
    movement.stop();
    delay(200);
    
    // Resume exploring
    Serial.println("✓ Escape complete");
    setState(EXPLORING);
}