#pragma once

#include <Arduino.h>

#include "pins.h"
#include "config.h"
#include "sensors.h"
#include "actuators.h"
#include "motors.h"

namespace HAL {
    // Declare global instances of the HAL objects.
    // The 'extern' keyword tells the compiler that these variables are defined in another file (hal.cpp).
    extern HAL::RGBLED led;
    extern HAL::LightSensor lightSensor;
    extern HAL::UltrasonicSensor ultrasonic;
    extern HAL::MotorDriver motors;
} // namespace HAL
