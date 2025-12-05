#include "sensors.h"
#include <algorithm> // For std::sort

// ============================================================================
// ULTRASONIC SENSOR IMPLEMENTATION
// ============================================================================

UltrasonicSensor::UltrasonicSensor(HAL& halRef) : hal(halRef) {
    // Initialize readings array to max distance
    for (int i = 0; i < FILTER_SIZE; i++) {
        readings[i] = 400;
    }
}

void UltrasonicSensor::update() {
    // Get new reading from HAL
    readings[readIndex] = hal.readUltrasonic();
    readIndex = (readIndex + 1) % FILTER_SIZE;
    
    // Update the filtered distance value
    filteredDistance = getMedianDistance();
    
    // Update stuck detection logic
    if (filteredDistance < STUCK_DISTANCE_THRESHOLD && abs(filteredDistance - lastDistance) < STUCK_STABILITY_THRESHOLD) {
        // If we are very close to an object AND the distance isn't changing, start a timer.
        if (stuckStartTime == 0) {
            stuckStartTime = millis();
        }
    } else {
        // If distance changes or we are far away, reset the timer.
        stuckStartTime = 0;
    }
    
    lastDistance = filteredDistance;
}

int UltrasonicSensor::getDistance() {
    return filteredDistance;
}

bool UltrasonicSensor::obstacleDetected() {
    return filteredDistance < stopDistance;
}

bool UltrasonicSensor::obstacleFar() {
    return filteredDistance < warnDistance;
}

bool UltrasonicSensor::isStuck() {
    // We are considered "stuck" if the stuck condition has persisted for the timeout duration.
    return (stuckStartTime != 0) && (millis() - stuckStartTime > STUCK_TIMEOUT);
}

void UltrasonicSensor::setStopDistance(int cm) {
    stopDistance = cm;
}

void UltrasonicSensor::setWarnDistance(int cm) {
    warnDistance = cm;
}

int UltrasonicSensor::getMedianDistance() {
    int sortedReadings[FILTER_SIZE];
    for (int i = 0; i < FILTER_SIZE; i++) {
        sortedReadings[i] = readings[i];
    }
    
    std::sort(sortedReadings, sortedReadings + FILTER_SIZE);
    
    // Return the middle value
    return sortedReadings[FILTER_SIZE / 2];
}


// ============================================================================
// LDR SENSOR IMPLEMENTATION
// ============================================================================


LDRSensor::LDRSensor(HAL& halRef) : hal(halRef), readIndex(0), leftBrightness(0.0f), rightBrightness(0.0f) {
    // Initialize filter arrays
    for (int i = 0; i < FILTER_SIZE; i++) {
        leftReadings[i] = 0.0f;
        rightReadings[i] = 0.0f;
    }
}

void LDRSensor::update() {
    // Read raw ADC values and normalize to 0.0-1.0
    float rawLeft = hal.readLDR_Left() / 4095.0f;
    float rawRight = hal.readLDR_Right() / 4095.0f;
    
    // Map to calibrated brightness (0.0 = dark, 1.0 = bright)
    float mappedLeft = mapBrightness(rawLeft, ADC::DARK_READING_LEFT, ADC::LIGHT_READING_LEFT);
    float mappedRight = mapBrightness(rawRight, ADC::DARK_READING_RIGHT, ADC::LIGHT_READING_RIGHT);
    
    // Store in filter array
    leftReadings[readIndex] = mappedLeft;
    rightReadings[readIndex] = mappedRight;
    
    readIndex = (readIndex + 1) % FILTER_SIZE;
    
    // Update filtered values
    leftBrightness = getAverageBrightness(leftReadings);
    rightBrightness = getAverageBrightness(rightReadings);
}

float LDRSensor::getLeftBrightness() {
    return leftBrightness;
}

float LDRSensor::getRightBrightness() {
    return rightBrightness;
}

float LDRSensor::getBrightnessDifference() {
    return leftBrightness - rightBrightness;
}

float LDRSensor::mapBrightness(float rawReading, float darkValue, float lightValue) {
    // Map from calibrated dark/light range to 0.0-1.0
    float brightness = (rawReading - darkValue) / (lightValue - darkValue);
    
    // Clamp to valid range
    if (brightness < 0.0f) brightness = 0.0f;
    if (brightness > 1.0f) brightness = 1.0f;
    
    return brightness;
}

float LDRSensor::getAverageBrightness(float readings[]) {
    float sum = 0.0f;
    for (int i = 0; i < FILTER_SIZE; i++) {
        sum += readings[i];
    }
    return sum / FILTER_SIZE;
}