#include "hal.h"
#include "pins.h"
#include <algorithm> // For std::sort

HAL::HAL() {}

bool HAL::init() {
    // Configure motor PWM channels (20kHz, 8-bit)
    ledcSetup(MOTOR_A_PWM_CHANNEL, PWM_FREQ, PWM_RESOLUTION);
    ledcSetup(MOTOR_B_PWM_CHANNEL, PWM_FREQ, PWM_RESOLUTION);
    ledcAttachPin(Pins::MOTOR_A_EN, MOTOR_A_PWM_CHANNEL);
    ledcAttachPin(Pins::MOTOR_B_EN, MOTOR_B_PWM_CHANNEL);
    
    // Configure RGB LED PWM channels (5kHz is fine for LEDs)
    ledcSetup(RGB_R_PWM_CHANNEL, 5000, PWM_RESOLUTION);
    ledcSetup(RGB_G_PWM_CHANNEL, 5000, PWM_RESOLUTION);
    ledcSetup(RGB_B_PWM_CHANNEL, 5000, PWM_RESOLUTION);
    ledcAttachPin(Pins::LED_RED, RGB_R_PWM_CHANNEL);
    ledcAttachPin(Pins::LED_GREEN, RGB_G_PWM_CHANNEL);
    ledcAttachPin(Pins::LED_BLUE, RGB_B_PWM_CHANNEL);
    
    // Motor direction pins (digital)
    pinMode(Pins::MOTOR_A_IN1, OUTPUT);
    pinMode(Pins::MOTOR_A_IN2, OUTPUT);
    pinMode(Pins::MOTOR_B_IN1, OUTPUT);
    pinMode(Pins::MOTOR_B_IN2, OUTPUT);
    
    // TB6612FNG Standby pin - HIGH to enable motor driver
    pinMode(Pins::MOTOR_STBY, OUTPUT);
    digitalWrite(Pins::MOTOR_STBY, HIGH); // Enable motor driver
    
    // Ultrasonic sensor pins
    pinMode(Pins::US_TRIGGER, OUTPUT);
    pinMode(Pins::US_ECHO, INPUT);
    digitalWrite(Pins::US_TRIGGER, LOW);
    
    // LDR pins (ADC - no pinMode needed on ESP32)
    // ADC is configured automatically when analogRead is called
    
    // Battery sense pin (ADC)
    // ADC is configured automatically when analogRead is called
    
    // Initialize motors stopped
    stopMotors();
    
    // Test RGB (quick flash to show init complete)
    setRGB(255, 0, 0);
    delay(100);
    setRGB(0, 0, 0);
    
    return true;
}

// ============================================================================
// LED CONTROL
// ============================================================================

void HAL::setLED(bool state) {
    ledcWrite(RGB_R_PWM_CHANNEL, state ? 255 : 0);
}

void HAL::setRGB(uint8_t r, uint8_t g, uint8_t b) {
    ledcWrite(RGB_R_PWM_CHANNEL, r);
    ledcWrite(RGB_G_PWM_CHANNEL, g);
    ledcWrite(RGB_B_PWM_CHANNEL, b);
}

// ============================================================================
// MOTOR CONTROL
// ============================================================================

void HAL::setMotorA(int speed, bool forward) {
    speed = constrain(speed, 0, 255);
    
    // Set direction
    digitalWrite(Pins::MOTOR_A_IN1, forward ? HIGH : LOW);
    digitalWrite(Pins::MOTOR_A_IN2, forward ? LOW : HIGH);
    
    // Set speed with hardware PWM
    ledcWrite(MOTOR_A_PWM_CHANNEL, speed);
}

void HAL::setMotorB(int speed, bool forward) {
    speed = constrain(speed, 0, 255);
    
    // Set direction
    digitalWrite(Pins::MOTOR_B_IN1, forward ? HIGH : LOW);
    digitalWrite(Pins::MOTOR_B_IN2, forward ? LOW : HIGH);
    
    // Set speed with hardware PWM
    ledcWrite(MOTOR_B_PWM_CHANNEL, speed);
}

void HAL::stopMotors() {
    // Stop with active brake (both pins LOW = short circuit)
    ledcWrite(MOTOR_A_PWM_CHANNEL, 0);
    ledcWrite(MOTOR_B_PWM_CHANNEL, 0);
    digitalWrite(Pins::MOTOR_A_IN1, LOW);
    digitalWrite(Pins::MOTOR_A_IN2, LOW);
    digitalWrite(Pins::MOTOR_B_IN1, LOW);
    digitalWrite(Pins::MOTOR_B_IN2, LOW);
}

void HAL::brakeMotors() {
    // Active brake - both direction pins HIGH
    // This shorts the motor terminals, providing strong braking
    ledcWrite(MOTOR_A_PWM_CHANNEL, 255);
    ledcWrite(MOTOR_B_PWM_CHANNEL, 255);
    digitalWrite(Pins::MOTOR_A_IN1, HIGH);
    digitalWrite(Pins::MOTOR_A_IN2, HIGH);
    digitalWrite(Pins::MOTOR_B_IN1, HIGH);
    digitalWrite(Pins::MOTOR_B_IN2, HIGH);
}

void HAL::coastMotors() {
    // Coast - just disable PWM, let motors spin down naturally
    ledcWrite(MOTOR_A_PWM_CHANNEL, 0);
    ledcWrite(MOTOR_B_PWM_CHANNEL, 0);
}

// ============================================================================
// ULTRASONIC SENSOR
// ============================================================================

long HAL::measurePulse() {
    // Send 10us trigger pulse
    digitalWrite(Pins::US_TRIGGER, LOW);
    delayMicroseconds(2);
    digitalWrite(Pins::US_TRIGGER, HIGH);
    delayMicroseconds(10);
    digitalWrite(Pins::US_TRIGGER, LOW);
    
    // Measure echo pulse duration (timeout after 30ms)
    return pulseIn(Pins::US_ECHO, HIGH, US_TIMEOUT);
}

int HAL::readUltrasonic() {
    // Use a median filter to reject noise and get a stable reading.
    const int numReadings = 5;
    long readings[numReadings];

    for (int i = 0; i < numReadings; ++i) {
        readings[i] = measurePulse();
        delay(20); // Small delay between pings to prevent interference
    }

    // Sort the readings to find the median
    std::sort(readings, readings + numReadings);

    // The median is the middle value
    long medianDuration = readings[numReadings / 2];
    
    // If the median is 0, it means most readings timed out
    if (medianDuration == 0) {
        return US_MAX_DISTANCE;
    }
    
    int distance = medianDuration / 58;
    
    return constrain(distance, 2, US_MAX_DISTANCE);
}

// ============================================================================
// BATTERY MONITORING
// ============================================================================

float HAL::readBatteryVoltage() {
    // Read ADC value (0-4095 for 12-bit ADC)
    int adcValue = analogRead(Pins::BATTERY_SENSE);
    
    // This ratio is calculated by measuring the actual battery voltage with a
    // multimeter and dividing it by the raw ADC value reported by the calibration
    // sketch. This accounts for all hardware variations.
    // Example: 7.65V / 2350 ADC = 0.003255
    const float ADC_TO_VOLTAGE_RATIO = 0.001788; // Recalibrated: 7.32V / 4095 ADC
    
    float voltage = adcValue * ADC_TO_VOLTAGE_RATIO;
    
    return voltage;
}

// ============================================================================
// LDR SENSORS (Light Dependent Resistors)
// ============================================================================

int HAL::readLDR_Left() {
    return analogRead(Pins::LDR_LEFT);
}

int HAL::readLDR_Right() {
    return analogRead(Pins::LDR_RIGHT);
}