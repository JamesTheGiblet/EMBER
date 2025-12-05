#ifndef SENSORS_H
#define SENSORS_H
#include "hal.h"
#include "config.h"  // NEW - for ADC calibration values

class UltrasonicSensor {
public:
    UltrasonicSensor(HAL& halRef);
    
    void update();              // Call in loop - updates readings
    int getDistance();          // Get filtered distance
    bool obstacleDetected();    // Is obstacle within stop distance?
    bool obstacleFar();         // Is obstacle in warning zone?
    bool isStuck();             // Stuck detection (distance unchanging)
    
    // Configuration
    void setStopDistance(int cm);
    void setWarnDistance(int cm);
    
private:
    HAL& hal;
    
    // Distance thresholds
    int stopDistance = 20;      // Stop if closer than this (cm)
    int warnDistance = 40;      // Slow down if closer than this (cm)
    
    // Filtering
    static const int FILTER_SIZE = 5;
    int readings[FILTER_SIZE];
    int readIndex = 0;
    int filteredDistance = 400;
    
    // Stuck detection thresholds
    static const int STUCK_DISTANCE_THRESHOLD = 15;    // Must be closer than 15cm
    static const int STUCK_STABILITY_THRESHOLD = 2;    // Distance can't vary more than 2cm
    static const unsigned long STUCK_TIMEOUT = 3000;   // Must persist for 3 seconds
    
    // Stuck detection state
    int lastDistance = 400;
    unsigned long stuckStartTime = 0;
    
    int getMedianDistance();
};

// ============================================================================
// LDR SENSOR CLASS (Phase 3A)
// ============================================================================

class LDRSensor {
public:
    LDRSensor(HAL& halRef);
    
    void update();                  // Call in loop - updates readings
    float getLeftBrightness();      // Get filtered left brightness (0.0-1.0)
    float getRightBrightness();     // Get filtered right brightness (0.0-1.0)
    float getBrightnessDifference(); // Get L-R difference (positive = left brighter)
    
private:
    HAL& hal;
    
    // Filtering (simple moving average)
    static const int FILTER_SIZE = 5;
    float leftReadings[FILTER_SIZE];
    float rightReadings[FILTER_SIZE];
    int readIndex = 0;
    
    // Filtered values
    float leftBrightness = 0.0f;
    float rightBrightness = 0.0f;
    
    // Helper functions
    float getAverageBrightness(float readings[]);
    float mapBrightness(float rawReading, float darkValue, float lightValue);
};

#endif