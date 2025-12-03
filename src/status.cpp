#include "status.h"

StatusLED::StatusLED(HAL& halRef) 
    : hal(halRef), currentStatus(OFF), lastBlink(0), blinkState(false) {
}

void StatusLED::setStatus(Status newStatus) {
    currentStatus = newStatus;
    blinkState = false;
    lastBlink = millis();
    
    // Apply color immediately
    updateColor();
}

void StatusLED::update() {
    // Blink for ERROR and OTA_UPDATE states
    if (currentStatus == ERROR || currentStatus == OTA_UPDATE) {
        if (millis() - lastBlink >= 500) {
            blinkState = !blinkState;
            lastBlink = millis();
            
            if (blinkState) {
                updateColor(); // Re-apply the state's color
            } else {
                applyColor(0, 0, 0);
            }
        }
    }
}

StatusLED::Status StatusLED::getCurrentStatus() {
    return currentStatus;
}

void StatusLED::applyColor(uint8_t r, uint8_t g, uint8_t b) {
    hal.setRGB(r, g, b);
}

void StatusLED::updateColor() {
    switch (currentStatus) {
        case OFF:
            applyColor(0, 0, 0);
            break;
        case BOOTING:
            applyColor(255, 0, 0); // Red
            break;
        case READY:
            applyColor(0, 255, 0); // Green
            break;
        case MOVING:
            applyColor(0, 0, 255); // Blue
            break;
        case OBSTACLE:
            applyColor(255, 255, 0); // Yellow
            break;
        case SEARCHING:
            applyColor(0, 255, 255); // Cyan
            break;
        case ERROR:
            applyColor(255, 0, 0); // Red (will blink)
            break;
        case CALIBRATING:
            applyColor(128, 0, 128); // Purple
            break;
        case OTA_UPDATE:
            applyColor(255, 255, 255); // White (will blink)
            break;
    }
}
