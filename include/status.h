#ifndef STATUS_H
#define STATUS_H

#include "hal.h"

class StatusLED {
public:
    enum Status {
        OFF,
        BOOTING,      // Red
        READY,        // Green
        MOVING,       // Blue
        OBSTACLE,     // Yellow
        SEARCHING,    // Cyan (light seeking)
        ERROR,        // Red blink
        CALIBRATING,  // Purple
        OTA_UPDATE    // White blink
    };
    
    StatusLED(HAL& halRef);
    
    void setStatus(Status newStatus);
    void update(); // Call in loop for animations
    Status getCurrentStatus();
    
private:
    HAL& hal;
    Status currentStatus;
    unsigned long lastBlink;
    bool blinkState;
    
    void applyColor(uint8_t r, uint8_t g, uint8_t b);
    void updateColor();
};

#endif // STATUS_H
