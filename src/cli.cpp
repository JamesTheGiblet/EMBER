/*
 * cli.cpp - Command Line Interface implementation
 */

#include "cli.h"
#include "hal.h"
#include "genome.h"
#include "globals.h"

// Forward declaration for function in main.cpp
void saveGenome();

/**
 * @brief Handles incoming commands from the Serial Monitor.
 */
void handleSerialCommands() {
    if (Serial.available() > 0) {
        String cmd = Serial.readStringUntil('\n');
        cmd.trim();

        if (cmd == "sensors") {
            Serial.println("\n--- Sensor Readings ---");
            Serial.printf("Light Left:  %.3f\n", HAL::lightSensor.readLeft());
            Serial.printf("Light Right: %.3f\n", HAL::lightSensor.readRight());
            // light_level is not directly available here, but we can recalculate it.
            Serial.printf("Light Avg:   %.3f\n", (HAL::lightSensor.readLeft() + HAL::lightSensor.readRight()) / 2.0f);
            Serial.printf("Distance:    %.1f cm\n", HAL::ultrasonic.readDistance());
            Serial.println("----------------------\n");
        }
        else if (cmd == "status") {
            char battery_str[30];
            if (battery.mode == POWER_USB_DEBUG) {
                snprintf(battery_str, sizeof(battery_str), "DEBUGGING (%.1fV)", battery.voltage);
            } else {
                snprintf(battery_str, sizeof(battery_str), "%.1f%% (%.1fV)", battery.percentage, battery.voltage);
            }
            const char* behavior_str = "UNKNOWN";
            switch(currentBehavior) {
                case IDLE: behavior_str = "IDLE"; break;
                case SEEKING_LIGHT: behavior_str = "SEEKING"; break;
                case AVOIDING_OBSTACLE: behavior_str = "AVOIDING"; break;
            }
            if (manual_override) {
                behavior_str = "IDLE (FORCED)";
            }
            Serial.printf("Light: %.3f | Energy: %5.1f | Batt: %-18s | Dist: %4.1fcm | Alive: %lus | Behavior: %s\n",
                      (HAL::lightSensor.readLeft() + HAL::lightSensor.readRight()) / 2.0f, energy, battery_str, HAL::ultrasonic.readDistance(), alive_time_ms / 1000, behavior_str);
        }
        else if (cmd == "genome") {
            Serial.println("\n--- Current Genome ---");
            Serial.printf("  Bot ID:           %d\n", genome.bot_id);
            Serial.printf("  Generation:       %u\n", genome.generation);
            Serial.printf("  Light Threshold:  %.3f\n", genome.light_threshold);
            Serial.printf("  Efficiency:       %.3f\n", genome.efficiency);
            Serial.printf("  Turn Sensitivity: %.1f\n", genome.turn_sensitivity);
            Serial.printf("  Base Speed:       %d\n", genome.base_speed);
            Serial.println("----------------------\n");
        }
        else if (cmd == "battery") {
            Serial.println("\n--- Battery Status ---");
            Serial.printf("  Voltage:    %.2fV\n", battery.voltage);
            Serial.printf("  Percentage: %.1f%%\n", battery.percentage);
            Serial.printf("  Power Mode: %d\n", battery.mode);
            Serial.println("----------------------\n");
        }
        else if (cmd == "reset") {
            energy = 100.0f;
            is_alive = true;
            boot_time_ms = millis();
            Serial.println("[System] Life has been reset.");
        }
        else if (cmd.startsWith("threshold ")) {
            float val = cmd.substring(10).toFloat();
            genome.light_threshold = constrain(val, 0.01f, 1.0f);
            saveGenome();
            Serial.printf("[Genome] light_threshold set to %.3f\n", genome.light_threshold);
        }
        else if (cmd.startsWith("efficiency ")) {
            float val = cmd.substring(11).toFloat();
            genome.efficiency = constrain(val, 0.5f, 2.0f);
            saveGenome();
            Serial.printf("[Genome] efficiency set to %.3f\n", genome.efficiency);
        }
        else if (cmd.startsWith("turn_sensitivity ")) {
            int val = cmd.substring(16).toInt();
            genome.turn_sensitivity = constrain(val, 50, 2000);
            saveGenome();
            Serial.printf("[Genome] turn_sensitivity set to %d\n", (int)genome.turn_sensitivity);
        }
        else if (cmd.startsWith("base_speed ")) {
            int val = cmd.substring(11).toInt();
            genome.base_speed = constrain(val, 50, 255);
            saveGenome();
            Serial.printf("[Genome] base_speed set to %d\n", genome.base_speed);
        }
        else if (cmd == "mutate") {
            genome.light_threshold += random(-100, 100) / 2000.0f; // Smaller mutation
            genome.efficiency += random(-100, 100) / 2000.0f;
            genome.light_threshold = constrain(genome.light_threshold, 0.01f, 1.0f);
            genome.efficiency = constrain(genome.efficiency, 0.5f, 2.0f);
            genome.generation++;
            saveGenome();
            Serial.println("[Genome] Genome mutated and saved.");
        }
        else if (cmd == "randomize") {
            genome.light_threshold = 0.3f;
            genome.efficiency = 0.75f + (random(0, 100) / 100.0f);
            genome.turn_sensitivity = 200 + random(0, 600);
            genome.base_speed = 150 + random(0, 100);
            genome.generation = 0;
            saveGenome();
            energy = 100.0f; // Also reset life
            is_alive = true;
            boot_time_ms = millis();
            Serial.println("[Genome] New random genome generated, saved, and life reset.");
        }
        else if (cmd == "reboot") {
            Serial.println("[System] Rebooting now...");
            delay(100);
            ESP.restart();
        }
        else if (cmd == "force idle") {
            manual_override = true;
            currentBehavior = IDLE;
            Serial.println("[System] Forcing IDLE state. Use 'force auto' to release.");
        }
        else if (cmd == "force auto") {
            manual_override = false;
            Serial.println("[System] Releasing manual override. Bot is now autonomous.");
        }
        else if (cmd == "help") {
            Serial.println("\n--- Serial Commands ---");
            Serial.println("  status   - Show main status line.");
            Serial.println("  sensors  - Show raw sensor readings.");
            Serial.println("  battery  - Show battery status.");
            Serial.println("  genome   - Show current genetic traits.");
            Serial.println("");
            Serial.println("  reset    - Reset life and energy.");
            Serial.println("  mutate   - Mutate the current genome and save.");
            Serial.println("  randomize- Generate a new random genome and reset life.");
            Serial.println("  reboot   - Reboot the device.");
            Serial.println("");
            Serial.println("  force idle - Stop the bot and hold it in IDLE state.");
            Serial.println("  force auto - Allow the bot to decide its own behavior.");
            Serial.println("  set <gene> <value> - Set a gene value (e.g., 'set threshold 0.5').");
            Serial.println("  help     - Show this list.");
            Serial.println("-----------------------\n");
        }
        else if (cmd.length() > 0) {
            Serial.println("Unknown command. Type 'help' for a list of commands.");
        }
    }
}