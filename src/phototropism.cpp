#include "phototropism.h"
#include "status.h"
#include "movement.h"
#include "hal.h"
#include "sensors.h"

Phototropism::Phototropism(HAL& halRef, Movement& movRef, StatusLED& statRef, LDRSensor& ldrRef)
    : hal(halRef), movement(movRef), status(statRef), ldrSensor(ldrRef) {}

void Phototropism::enable() {
    enabled = true;
    currentState = IDLE;
    Serial.println("🦋 Phototropism mode ENABLED (moth mode)");
}

void Phototropism::disable() {
    enabled = false;
    movement.stop();
    currentState = IDLE;
    status.setStatus(StatusLED::READY);
    Serial.println("🌑 Phototropism mode DISABLED");
}

bool Phototropism::isEnabled() {
    return enabled;
}

Phototropism::State Phototropism::getState() {
    return currentState;
}

void Phototropism::update() {
    if (!enabled) return;
    
    // Get calibrated brightness (0.0-1.0)
    float leftBright = ldrSensor.getLeftBrightness();
    float rightBright = ldrSensor.getRightBrightness();
    float avgBright = (leftBright + rightBright) / 2.0f;
    float difference = leftBright - rightBright;  // Positive = left brighter
    
    switch (currentState) {
        case IDLE:
            // Wait for bright light to appear
            if (avgBright > LIGHT_THRESHOLD) {
                Serial.printf("💡 Light detected! Avg brightness: %.3f\n", avgBright);
                currentState = SEEKING;
                stateStartTime = millis();
            }
            break;
            
        case SEEKING:
            status.setStatus(StatusLED::SEARCHING);  // CYAN LED
            
            // Turn toward brighter side
            if (abs(difference) > SEEK_DELTA) {
                if (difference > 0) {
                    // Left is brighter - spin left (CCW)
                    Serial.printf("↺ Turning LEFT (L=%.3f > R=%.3f)\n", leftBright, rightBright);
                    movement.spinCCW(seekSpeed);
                } else {
                    // Right is brighter - spin right (CW)
                    Serial.printf("↻ Turning RIGHT (R=%.3f > L=%.3f)\n", rightBright, leftBright);
                    movement.spinCW(seekSpeed);
                }
            } else if (abs(difference) < BALANCE_THRESHOLD) {
                // Sensors balanced - face light source!
                Serial.println("🎯 Light centered! Approaching...");
                movement.stop();
                delay(200);  // Brief pause
                currentState = APPROACHING;
                stateStartTime = millis();
            }
            
            // Timeout if seeking too long (stuck spinning)
            if (millis() - stateStartTime > 5000) {
                Serial.println("⏱️ Seek timeout - moving forward anyway");
                currentState = APPROACHING;
                stateStartTime = millis();
            }
            break;
            
        case APPROACHING:
            status.setStatus(StatusLED::SEARCHING);  // CYAN LED
            
            // Move toward light
            movement.forward(approachSpeed);
            
            // If light gets dim, go back to idle
            if (avgBright < LIGHT_THRESHOLD * 0.8f) {  // 80% of threshold
                Serial.printf("🌑 Light dimmed (%.3f). Stopping.\n", avgBright);
                movement.stop();
                currentState = IDLE;
                stateStartTime = millis();
            }
            
            // If light becomes unbalanced while approaching, go back to seeking
            if (abs(difference) > SEEK_DELTA * 1.5f) {  // 50% more sensitive
                Serial.println("🔄 Light shifted - re-seeking");
                currentState = SEEKING;
                stateStartTime = millis();
            }
            break;
    }
}