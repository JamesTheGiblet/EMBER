/**
 * @file main.cpp
 * @brief Main test file for EMBER hardware.
 *
 * This file now uses the final, filtered `hal.readUltrasonic()` function.
 */

#include <Arduino.h>
#include "hal.h"

HAL hal;

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(10); // Wait for serial port to connect
  }

  Serial.println("\n--- Ultrasonic Sensor Test ---");

  hal.init();
  Serial.println("HAL Initialized.");
  Serial.println("Reading final, filtered distance from HAL...");
}

void loop() {
  int distance = hal.readUltrasonic();

  Serial.printf("Distance: %d cm\n", distance);

  delay(500); // Wait half a second between readings
}