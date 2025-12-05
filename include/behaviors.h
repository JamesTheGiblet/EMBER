#ifndef BEHAVIORS_H
#define BEHAVIORS_H

#include "hal.h"
#include "movement.h"
#include "sensors.h"
#include "status.h"
#include "config.h"

class ObstacleAvoidance {
public:
    enum State {
        IDLE,
        EXPLORING,
        OBSTACLE_DETECTED,
        BACKING_UP,
        TURNING,
        STUCK_ESCAPE
    };
    
    ObstacleAvoidance(HAL& halRef, Movement& movRef, UltrasonicSensor& sensRef, 
                      StatusLED& statRef, MotorConfig& cfg);
    
    void enable();              // Start autonomous mode
    void disable();             // Stop autonomous mode
    void update();              // Call in loop
    bool isEnabled();
    State getState();
    
private:
    HAL& hal;
    Movement& movement;
    UltrasonicSensor& sensor;
    StatusLED& status;
    MotorConfig& config;
    
    bool enabled = false;
    State currentState = IDLE;
    unsigned long stateStartTime = 0;
    int turnDirection = 1; // 1 = right, -1 = left
    
    void setState(State newState);
    void handleExploring();
    void handleObstacleDetected();
    void handleBackingUp();
    void handleTurning();
    void handleStuckEscape();
};

// ============================================================================
// PHOTOTROPISM BEHAVIOR (Phase 3B)
// ============================================================================
class Phototropism {
public:
    enum State { IDLE, SEEKING, APPROACHING };
    
    Phototropism(HAL& halRef, Movement& movRef, StatusLED& statRef, LDRSensor& ldrRef);
    
    void enable();
    void disable();
    bool isEnabled();
    void update();
    State getState();
    
private:
    HAL& hal;
    Movement& movement;
    StatusLED& status;
    LDRSensor& ldrSensor;
    
    State currentState = IDLE;
    unsigned long stateStartTime = 0;
    bool enabled = false;
    
    const float LIGHT_THRESHOLD = 0.7f;
    const float SEEK_DELTA = 0.15f;
    const float BALANCE_THRESHOLD = 0.05f;
    
    const int seekSpeed = 100;
    const int approachSpeed = 120;
};

#endif