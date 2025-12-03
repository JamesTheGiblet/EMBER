#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

struct MotorConfig {
    int baseSpeed = 150;
    int crawlSpeed = 100;
    int maxSpeed = 255;
    int turnDuration = 800;
    bool motorA_inverted = false;
    bool motorB_inverted = false;
    int motorA_trim = 0;
    int motorB_trim = 0;
};

// Status LED Colors (for reference)
namespace StatusColors {
    const uint32_t OFF       = 0x000000;
    const uint32_t BOOTING   = 0xFF0000; // Red
    const uint32_t READY     = 0x00FF00; // Green
    const uint32_t MOVING    = 0x0000FF; // Blue
    const uint32_t OBSTACLE  = 0xFFFF00; // Yellow
    const uint32_t SEARCHING = 0x00FFFF; // Cyan
    const uint32_t ERROR     = 0xFF0000; // Red (blink)
    const uint32_t CALIBRATE = 0x8000FF; // Purple
    const uint32_t OTA       = 0xFFFFFF; // White (blink)
}

// Global instance
extern MotorConfig motorConfig;

#endif // CONFIG_H
