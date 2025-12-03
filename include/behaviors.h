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

#endif