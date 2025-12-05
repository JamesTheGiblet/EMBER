#pragma once
#include "hal.h"
#include "movement.h"
#include "status.h"
#include "sensors.h"  // NEW - need LDRSensor class

class Phototropism {
public:
    enum State { IDLE, SEEKING, APPROACHING };
    
    Phototropism(HAL& halRef, Movement& movRef, StatusLED& statRef, LDRSensor& ldrRef);  // Added LDRSensor
    
    void enable();
    void disable();
    bool isEnabled();
    void update();
    State getState();
    
private:
    HAL& hal;
    Movement& movement;
    StatusLED& status;
    LDRSensor& ldrSensor;  // NEW - reference to LDRSensor
    
    State currentState = IDLE;
    unsigned long stateStartTime = 0;
    bool enabled = false;
    
    // Thresholds (using calibrated 0.0-1.0 values, not raw ADC)
    const float LIGHT_THRESHOLD = 0.7f;      // Must be brighter than 70% to activate
    const float SEEK_DELTA = 0.15f;          // 15% difference triggers turning
    const float BALANCE_THRESHOLD = 0.05f;   // Within 5% = balanced
    
    const int seekSpeed = 100;
    const int approachSpeed = 120;
};