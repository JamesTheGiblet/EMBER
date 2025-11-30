#include "hal.h"
#include "actuators.h"
#include "sensors.h"
#include "motors.h"

// Define the global instances that were declared as 'extern' in hal.h
namespace HAL {
	RGBLED led;
	LightSensor lightSensor;
	UltrasonicSensor ultrasonic;
	MotorDriver motors;
}