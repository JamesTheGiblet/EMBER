#pragma once

#include <Arduino.h>

#include "pins.h"
#include "config.h"
#include "sensors.h"
#include "actuators.h"

namespace HAL {
    // Declare global instances of the HAL objects (motors removed).
    extern HAL::RGBLED led;
    extern HAL::LightSensor lightSensor;
    extern HAL::UltrasonicSensor ultrasonic;
} // namespace HAL
