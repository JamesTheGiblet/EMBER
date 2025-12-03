#include "hal.h"
#include "pins.h"

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
    
    // Initialize motors stopped
    stopMotors();
    
    // Test RGB (quick flash)
    setRGB(255, 0, 0);
    delay(100);
    setRGB(0, 0, 0);
    
    return true;
}

void HAL::setLED(bool state) {
    ledcWrite(RGB_R_PWM_CHANNEL, state ? 255 : 0);
}

void HAL::setRGB(uint8_t r, uint8_t g, uint8_t b) {
    ledcWrite(RGB_R_PWM_CHANNEL, r);
    ledcWrite(RGB_G_PWM_CHANNEL, g);
    ledcWrite(RGB_B_PWM_CHANNEL, b);
}

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