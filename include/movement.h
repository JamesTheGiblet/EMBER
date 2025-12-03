#ifndef MOVEMENT_H
#define MOVEMENT_H

#include "hal.h"
#include "config.h"

class Movement {
public:
    Movement(HAL& halRef, MotorConfig& cfg);
    
    // Basic movements
    void forward(int speed = -1);
    void backward(int speed = -1);
    void turnLeft(int speed = -1);
    void turnRight(int speed = -1);
    void spinCW(int speed = -1);
    void spinCCW(int speed = -1);
    void stop();
    
    // Speed presets
    void crawl();
    void run();
    
    // Smooth acceleration/deceleration
    void smoothStart(int targetSpeed);
    void smoothStop();
    void smoothForward(int speed = -1);
    void smoothBackward(int speed = -1);
    
    // State queries
    bool isMoving();
    int getCurrentSpeed();
    
private:
    HAL& hal;
    MotorConfig& config;
    
    // Motor state tracking
    struct MotorState {
        int speedA = 0;
        int speedB = 0;
        bool directionA = true;  // true = forward
        bool directionB = true;
        bool moving = false;
    } state;
    
    // Internal helpers
    void setMotors(int speedA, bool dirA, int speedB, bool dirB);
    void rampSpeed(int fromSpeed, int toSpeed, bool forward);
    int getSpeed(int requested);
};

#endif