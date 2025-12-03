#pragma once

#include <Arduino.h>

class HAL {
public:
    HAL();
    bool init();
    
    // LED Control
    void setLED(bool state);
    void setRGB(uint8_t r, uint8_t g, uint8_t b);
    
    // Motor Control
    void setMotorA(int speed, bool forward);
    void setMotorB(int speed, bool forward);
    void stopMotors();
    void brakeMotors();      // Active brake (short circuit)
    void coastMotors();      // Coast to stop
    
    // Ultrasonic Sensor
    int readUltrasonic();    // Returns distance in cm (0-400)
    
    // Battery Monitoring
    float readBatteryVoltage(); // Returns voltage
    
    // LDR Sensors
    int readLDR_Left();      // Returns ADC value (0-4095)
    int readLDR_Right();     // Returns ADC value (0-4095)
    
private:
    // PWM channels for ESP32
    static const int MOTOR_A_PWM_CHANNEL = 0;
    static const int MOTOR_B_PWM_CHANNEL = 1;
    static const int RGB_R_PWM_CHANNEL = 2;
    static const int RGB_G_PWM_CHANNEL = 3;
    static const int RGB_B_PWM_CHANNEL = 4;
    
    static const int PWM_FREQ = 20000;  // 20kHz - above human hearing
    static const int PWM_RESOLUTION = 8; // 8-bit (0-255)
    
    // Ultrasonic timing
    static const unsigned long US_TIMEOUT = 30000; // 30ms timeout (≈5m range)
    static const int US_MAX_DISTANCE = 400;        // Max valid distance (cm)
    
    // Helper for ultrasonic
    long measurePulse();
};