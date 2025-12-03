#include "sensors.h"
#include <algorithm> // For std::sort

UltrasonicSensor::UltrasonicSensor(HAL& halRef) : hal(halRef) {
    // Initialize readings array
    for (int i = 0; i < FILTER_SIZE; i++) {
        readings[i] = 400;
    }
}

void UltrasonicSensor::update() {
    // Get new reading
    int newReading = hal.readUltrasonic();
    
    // Add to circular buffer
    readings[readIndex] = newReading;
    readIndex = (readIndex + 1) % FILTER_SIZE;
    
    // Update filtered distance (median filter - removes noise spikes)
    filteredDistance = getMedianDistance();
    
    // Stuck detection logic:
    // If we are very close to an object AND the distance isn't changing,
    // start a timer. If that condition persists, we are stuck.
    if (filteredDistance < STUCK_DISTANCE_THRESHOLD && abs(filteredDistance - lastDistance) < STUCK_STABILITY_THRESHOLD) {
        if (stuckStartTime == 0) { // Start timer on first detection
            stuckStartTime = millis();
        }
    } else {
        stuckStartTime = 0; // Reset stuck timer
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
    if (stuckStartTime == 0) return false;
    return (millis() - stuckStartTime) > STUCK_TIMEOUT;
}

void UltrasonicSensor::setStopDistance(int cm) {
    stopDistance = constrain(cm, 5, 100);
}

void UltrasonicSensor::setWarnDistance(int cm) {
    warnDistance = constrain(cm, stopDistance + 5, 200);
}

int UltrasonicSensor::getMedianDistance() {
    // Copy array for sorting
    int sorted[FILTER_SIZE];
    for (int i = 0; i < FILTER_SIZE; i++) {
        sorted[i] = readings[i];
    }
    
    // Use the standard library's sort function. It's efficient and reliable.
    std::sort(sorted, sorted + FILTER_SIZE);
    
    // Return median (middle value)
    return sorted[FILTER_SIZE / 2];
}