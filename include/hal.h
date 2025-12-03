#ifndef HAL_H
#define HAL_H

#include <Arduino.h>

class HAL {
public:
    HAL();
    bool init();
    
    void setLED(bool state);
    void setRGB(uint8_t r, uint8_t g, uint8_t b);
    
    void setMotorA(int speed, bool forward);
    void setMotorB(int speed, bool forward);
    void stopMotors();
    
    // Brake modes
    void brakeMotors();      // Active brake (short circuit)
    void coastMotors();      // Coast to stop
    
private:
    // PWM channels for ESP32
    static const int MOTOR_A_PWM_CHANNEL = 0;
    static const int MOTOR_B_PWM_CHANNEL = 1;
    static const int RGB_R_PWM_CHANNEL = 2;
    static const int RGB_G_PWM_CHANNEL = 3;
    static const int RGB_B_PWM_CHANNEL = 4;
    
    static const int PWM_FREQ = 20000;  // 20kHz - above human hearing
    static const int PWM_RESOLUTION = 8; // 8-bit (0-255)
};

#endif