/*
 * EMBER - Step 1: OTA Firmware
 *
 * This is the foundational sketch for the EMBER bot. Its sole purpose
 * is to connect to WiFi and enable Over-the-Air (OTA) updates.
 *
 * This allows for wireless code deployment, which is essential for
 * developing the bot without needing a physical USB connection for every change.
 */

#include <Arduino.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <ArduinoOTA.h>
#include <WebServer.h>
#include <Preferences.h> // For saving data to flash (EEPROM replacement)
#include "pins.h"

// Include Hardware Abstraction Layer, Genome definition, and credentials
#include "hal.h"
#include "genome.h"
#include "credentials_user.h"

using namespace HAL;

// ============================================================================
// GLOBAL INSTANCES
// ============================================================================

Genome genome;           // The bot's genetic code
Preferences preferences; // For saving/loading the genome

// ============================================================================
// LIFE STATE
// ============================================================================

float energy = 100.0f;            // Current energy level (0-100)
float light_level = 0.0f;         // Last read ambient light level (0.0-1.0)
unsigned long alive_time_ms = 0;  // How long the bot has been alive
unsigned long boot_time_ms = 0;   // Timestamp for when setup() finishes
bool is_alive = true;             // The primary state: living or dead

// Timers for managing non-blocking updates
unsigned long last_life_update_ms = 0;
unsigned long last_stats_print_ms = 0;

// ============================================================================
// BEHAVIOR STATE
// ============================================================================

typedef enum {
    IDLE,           // Conserving energy, not moving
    SEEKING_LIGHT,  // Actively moving towards a light source
    AVOIDING_OBSTACLE // Performing an avoidance maneuver
} BehaviorState_t;

BehaviorState_t currentBehavior = IDLE;

// ============================================================================
// POWER MANAGEMENT
// ============================================================================

typedef enum {
    POWER_NORMAL = 0,
    POWER_ECONOMY,
    POWER_LOW,
    POWER_CRITICAL,
    POWER_SHUTDOWN,
    POWER_USB_DEBUG // Special mode for when powered via USB, which interferes with readings
} PowerMode_t;

typedef struct {
    float voltage;
    float percentage;
    PowerMode_t mode;
    uint32_t lastUpdate;
} BatteryMonitor_t;

BatteryMonitor_t battery;

// ============================================================================
// WEB SERVER
// ============================================================================

WebServer server(80);

float readBatteryVoltage() {
    int adcValue = analogRead(Pins::BATTERY_SENSE);
    float v = ((float)adcValue / 4095.0f) * 3.3f; // Using constants directly for clarity
    return v / 0.3333f; // Apply voltage divider formula
}

float batteryToPercent(float voltage) {
    // 2S LiPo: 8.4V (100%) to 6.4V (0%)
    float percent = (voltage - 6.4f) / (8.4f - 6.4f) * 100.0f;
    return constrain(percent, 0.0f, 100.0f);
}

PowerMode_t getPowerMode(float voltage) {
    if (voltage >= 7.8f) return POWER_NORMAL;
    if (voltage >= 7.2f) return POWER_ECONOMY;
    if (voltage >= 6.8f) return POWER_LOW;
    if (voltage >= 6.4f) return POWER_CRITICAL;
    return POWER_SHUTDOWN;
}

void updateBatteryStatus() {
    battery.voltage = readBatteryVoltage();
    battery.percentage = batteryToPercent(battery.voltage);

    // Check for unrealistic voltage, which indicates USB power is connected
    if (battery.voltage > 9.0f) {
        battery.mode = POWER_USB_DEBUG;
        battery.percentage = 101.0f; // Use a special value to indicate USB power
        return;
    }

    battery.mode = getPowerMode(battery.voltage);
    battery.lastUpdate = millis();
}

// WIFI & OTA SETUP
/**
 * @brief Connects the ESP32 to the configured WiFi network.
 *
 * Retries connection until successful. Provides serial feedback.
 */
void setupWiFi() {
    led.blue(50); // Dim blue to indicate WiFi setup is starting
    Serial.println("\n[WiFi] Connecting to " + String(WIFI_SSID));
    WiFi.mode(WIFI_STA);
    WiFi.setHostname(OTA_HOSTNAME);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    // Wait for connection
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    led.green(150); // Bright green for success
    Serial.println("\n[WiFi] Connected successfully!");
    Serial.print("[WiFi] IP Address: ");
    Serial.println(WiFi.localIP());
    Serial.print("[WiFi] Hostname: http://");
    Serial.print(OTA_HOSTNAME);
    Serial.println(".local");
}



/**
 * @brief Configures and starts the ArduinoOTA service.
 *
 * Sets up handlers to provide feedback during the update process.
 */
void setupOTA() {
    // --- Configure OTA ---
    ArduinoOTA.setHostname(OTA_HOSTNAME);
    ArduinoOTA.setPassword(OTA_PASSWORD);

    // --- OTA Event Handlers ---
    ArduinoOTA.onStart([]() {
        String type;
        if (ArduinoOTA.getCommand() == U_FLASH) {
            HAL::led.magenta(200); // Magenta indicates an OTA update is in progress
            type = "sketch";
        } else { // U_SPIFFS
            type = "filesystem";
        }
        Serial.println("[OTA] Start updating " + type);
    });

    ArduinoOTA.onEnd([]() {
        Serial.println("\n[OTA] Update Complete!");
        // Green flash 5x for success
        for (int i = 0; i < 5; i++) { 
            HAL::led.green(255);
            delay(100);
            HAL::led.off();
            delay(100);
        }
    });

    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("[OTA] Progress: %u%%\r", (progress / (total / 100)));
        // Pulse magenta to show progress
        // Brightness cycles up and down
        int brightness = (millis() / 5) % 512;
        if (brightness > 255) brightness = 511 - brightness;
        HAL::led.setRGB(brightness, 0, brightness);
    });

    ArduinoOTA.onError([](ota_error_t error) {
        // Red flash 10x for error
        for (int i = 0; i < 10; i++) {
            HAL::led.red(255);
            delay(50);
            HAL::led.off();
            delay(50);
        }
        Serial.printf("[OTA] Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR) Serial.println("Authentication Failed");
        else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
        else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
        else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
        else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });

    // --- Start OTA Service ---
    ArduinoOTA.begin();
    Serial.println("[OTA] Service ready.");
}

/**
 * @brief Configures and starts the mDNS responder.
 *
 * This allows you to connect to the device using its hostname (e.g., "ember-bot-1.local")
 * instead of its IP address.
 */
void setupMDNS() {
    if (!MDNS.begin(OTA_HOSTNAME)) {
        Serial.println("[mDNS] Error setting up mDNS responder!");
        return;
    }
    Serial.println("[mDNS] Responder started.");
    // Announce the OTA service over mDNS
    MDNS.addService("arduino", "tcp", 3232);
}

// ============================================================================
// PERSISTENT STORAGE (EEPROM)
// ============================================================================

/**
 * @brief Saves the current genome to non-volatile flash storage.
 */
void saveGenome() {
    preferences.begin("ember-genome", false); // Open in read-write mode
    preferences.putBytes("genome", &genome, sizeof(genome));
    preferences.end();
    Serial.println("[Storage] Genome saved to flash.");
}

/**
 * @brief Loads the genome from flash storage. If no genome is found,
 *        it initializes with a random one.
 */
void loadGenome() {
    preferences.begin("ember-genome", true); // Open in read-only mode
    
    if (preferences.isKey("genome")) {
        preferences.getBytes("genome", &genome, sizeof(genome));
        Serial.println("[Storage] Genome loaded from flash.");
    } else {
        Serial.println("[Storage] No saved genome found. Generating a random one.");
        // Seed the random number generator from an unused analog pin
        randomSeed(analogRead(A0)); 

        genome.light_threshold = 0.05f; // Set a minimal, but non-zero, threshold
        genome.efficiency = 0.75f + (random(0, 100) / 100.0f); // Random efficiency 0.75 to 1.75
        genome.bot_id = 0; // Default to 0, should be set uniquely
        genome.generation = 0;
        
        // Immediately save the new random genome
        saveGenome();
    }
    preferences.end();
    Serial.printf("[Genome] ID: %d, Gen: %d, Threshold: %.2f, Efficiency: %.2f\n", genome.bot_id, genome.generation, genome.light_threshold, genome.efficiency);
}

// Forward declarations for functions used in the main loop
void updateLife(float dt);
void handleSerialCommands();
void decideBehavior();
void executeBehavior();
void setupWebServer();
void updateBatteryStatus();
void showState();

// ============================================================================
// SETUP
// ============================================================================
void setup() {
    Serial.begin(115200);
    Serial.println("\n\n--- EMBER OTA Firmware Booting ---");

    // Initialize hardware from the HAL
    lightSensor.begin();
    ultrasonic.begin();
    led.begin();
    led.yellow(50); // Dim yellow to indicate booting

    // Load the bot's identity and traits from "EEPROM"
    loadGenome();

    // Take the first reading to initialize the light_level variable
    light_level = lightSensor.readAverage();

    // Give the bot a starting chance based on its initial environment
    updateLife(0.1f); // Simulate a small time-slice for the initial energy calculation

    HAL::motors.begin();

    // Set boot timestamp after all initial setup
    boot_time_ms = millis();

    setupWiFi();
    setupMDNS();
    setupOTA();

    // Start the web server if WiFi is connected
    if (WiFi.status() == WL_CONNECTED) {
        setupWebServer();
    }

    updateBatteryStatus();

    led.off(); // Turn LED off after setup is complete
    Serial.println("\n--- Boot complete. Life cycle starting. ---");
}

// ============================================================================
// LOOP
// ============================================================================

/**
 * @brief Updates the bot's energy based on light level and genome.
 */
void updateLife(float dt) {
    // Universal cost of existence
    // The decay is now multiplied by delta time (dt) to be per-second.
    energy -= Config::Life::ENERGY_DECAY * dt;

    // Gain energy from light if the threshold is met
    if (light_level > genome.light_threshold) {
        float excess_light = light_level - genome.light_threshold;
        energy += Config::Life::ENERGY_GAIN * excess_light * genome.efficiency * dt;
    }

    // Clamp energy to its valid range
    energy = constrain(energy, Config::Life::MIN_ENERGY, Config::Life::MAX_ENERGY);

    // Check for death
    if (energy <= 0.0f) {
        is_alive = false;
    }

    // Check for resurrection
    if (!is_alive && energy > 0.0f) {
        is_alive = true;
        boot_time_ms = millis(); // Reset the 'alive' timer
    }
}

/**
 * @brief Updates the RGB LED to reflect the bot's current state.
 */
void showState() {
    if (!is_alive) {
        led.off(); // Dead
        return;
    }

    // Override color for low power warning
    if (battery.mode == POWER_LOW || battery.mode == POWER_CRITICAL) {
        // Flash yellow to indicate low battery
        bool led_on = (millis() % 1000) < 500;
        led.yellow(led_on ? 200 : 0);
        return;
    }

    if (light_level > genome.light_threshold) {
        led.green(150); // Thriving
    } else {
        // Dying - flash red, faster as energy gets lower
        int flash_period = 100 + (int)(energy * 9); // Period from 100ms to 1000ms
        bool led_on = (millis() % flash_period) < (flash_period / 2);
        if (led_on) {
            led.red(200);
        } else {
            led.off();
        }
    }
}

/**
 * @brief The "brain" of the bot. Decides what behavior to adopt.
 */
void decideBehavior() {
    // --- Highest Priority: Obstacle Avoidance ---
    float distance = ultrasonic.readDistance();
    if (distance > 0 && distance < Config::Behavior::OBSTACLE_DISTANCE_CM) {
        currentBehavior = AVOIDING_OBSTACLE;
        return; // Obstacle avoidance overrides all other behaviors
    }

    // Power state overrides all other decisions.
    if (battery.mode == POWER_CRITICAL || battery.mode == POWER_SHUTDOWN) {
        currentBehavior = IDLE; // Must conserve power
        return;
    }

    // The primary drive is survival. If energy is low, it must seek light.
    if (energy < 95.0f) {
        currentBehavior = SEEKING_LIGHT;
    } 
    // If energy is high, it can rest and conserve power.
    else {
        currentBehavior = IDLE;
    }
}

/**
 * @brief Executes the current behavior by controlling the motors.
 */
void executeBehavior() {
    if (!is_alive) {
        HAL::motors.stop();
        return;
    }

    switch (currentBehavior) {
        case IDLE: {
            HAL::motors.stop();
            break;
        }

        case SEEKING_LIGHT: {
            float leftLight = lightSensor.readLeft();
            float rightLight = lightSensor.readRight();
            
            // Simple phototaxis: turn towards the brighter light source.
            float error = leftLight - rightLight;
            
            // Adjust speed based on power mode
            int baseSpeed = (battery.mode == POWER_ECONOMY) ? 150 : 200;
            int turnSpeed = (int)(error * 500.0f);

            HAL::motors.setSpeeds(baseSpeed - turnSpeed, baseSpeed + turnSpeed);
            break;
        }

        case AVOIDING_OBSTACLE: {
            // Simple avoidance: back up and turn right.
            Serial.println("[Behavior] Obstacle detected! Avoiding...");
            HAL::motors.setSpeeds(-180, -180); // Back up
            delay(400);
            HAL::motors.setSpeeds(200, -200);  // Turn right
            delay(300);
            HAL::motors.stop();
            currentBehavior = IDLE; // Re-evaluate state on the next loop
            break;
        }
    }
}

void loop() {
    ArduinoOTA.handle(); // Always handle network requests
    server.handleClient(); // Handle incoming web requests
    handleSerialCommands(); // Check for user input
    
    // Calculate delta time (time since last loop)
    unsigned long now = millis();
    float dt = (now - last_life_update_ms) / 1000.0f;
    last_life_update_ms = now;
    
    // Read primary sensors once per loop for consistency
    light_level = lightSensor.readAverage();

    // Update the life state regardless of whether it's alive or dead.
    // This allows a "dead" bot to potentially gain energy and resurrect.
    updateLife(dt);

    if (is_alive) {
        // Decide what to do, then do it.
        decideBehavior();
        executeBehavior();
    }

    if (is_alive) {
        alive_time_ms = millis() - boot_time_ms;
    }

    // Always update the LED to reflect the current state
    showState();

    // Print stats to serial every second
    if (millis() - last_stats_print_ms > 1000) {
        last_stats_print_ms = millis();

        // Update battery status before printing
        updateBatteryStatus();

        // Build the battery status string
        char battery_str[30];
        if (battery.mode == POWER_USB_DEBUG) {
            snprintf(battery_str, sizeof(battery_str), "DEBUGGING (%.1fV)", battery.voltage);
        } else {
            snprintf(battery_str, sizeof(battery_str), "%.1f%% (%.1fV)", battery.percentage, battery.voltage);
        }

        Serial.printf("Light: %.3f | Energy: %5.1f | Batt: %-18s | Dist: %4.1fcm | Alive: %lus | Status: %s\n",
                      light_level, energy, battery_str, ultrasonic.readDistance(), alive_time_ms / 1000, is_alive ? "ALIVE" : "DEAD");
    }
}

/**
 * @brief Handles incoming commands from the Serial Monitor.
 */
void handleSerialCommands() {
    if (Serial.available() > 0) {
        String cmd = Serial.readStringUntil('\n');
        cmd.trim();

        if (cmd == "sensors") {
            Serial.println("\n--- Sensor Readings ---");
            Serial.printf("Light Left:  %.3f\n", lightSensor.readLeft());
            Serial.printf("Light Right: %.3f\n", lightSensor.readRight());
            Serial.printf("Light Avg:   %.3f\n", light_level);
            Serial.printf("Distance:    %.1f cm\n", ultrasonic.readDistance());
            Serial.println("----------------------\n");
        }
        else if (cmd == "reset") {
            energy = 100.0f;
            is_alive = true;
            boot_time_ms = millis();
            Serial.println("[System] Life has been reset.");
        }
        else if (cmd == "help") {
            Serial.println("\n--- Serial Commands ---");
            Serial.println("  sensors  - Show current sensor readings.");
            Serial.println("  reset    - Reset the bot's life and energy.");
            Serial.println("  forward, back, left, right, stop - Control motors.");
            Serial.println("  help     - Show this command list.");
            Serial.println("-----------------------\n");
        }
        else if (cmd.length() > 0) {
            Serial.println("Unknown command. Type 'help' for a list of commands.");
        }
    }
}

// ============================================================================
// WEB SERVER IMPLEMENTATION
// ============================================================================

void handleRoot() {
    char buffer[4096]; // A buffer to build the HTML page
    char temp[200];

    // Header
    snprintf(buffer, sizeof(buffer),
        "<!DOCTYPE html><html><head><title>EMBER Bot %d</title>"
        "<meta http-equiv='refresh' content='2'>"
        "<style>"
        "body{font-family:monospace;background:#282c34;color:#abb2bf;padding:1em;}"
        ".container{max-width:800px;margin:auto;}"
        "h1{color:#61afef;text-align:center;}"
        ".grid{display:grid;grid-template-columns:1fr 1fr;gap:1em;}"
        ".box{background:#323842;padding:1em;border-radius:8px;}"
        "h2{color:#98c379;border-bottom:1px solid #444;padding-bottom:0.5em;margin-bottom:0.5em;}"
        "p{display:flex;justify-content:space-between;margin:0.5em 0;} span{color:#e5c07b;}"
        ".actions a{display:block;padding:0.8em;margin:0.5em 0;background:#61afef;color:#fff;text-decoration:none;text-align:center;border-radius:5px;}"
        "</style></head><body><div class='container'><h1>&#128293; EMBER Bot %d</h1>",
        genome.bot_id, genome.bot_id);

    // Main Grid
    strcat(buffer, "<div class='grid'>");

    // --- Life Status Box ---
    strcat(buffer, "<div class='box'><h2>Life Status</h2>");
    snprintf(temp, sizeof(temp), "<p>Status: <span>%s</span></p>", is_alive ? "ALIVE" : "DEAD"); strcat(buffer, temp);
    snprintf(temp, sizeof(temp), "<p>Energy: <span>%.1f%%</span></p>", energy); strcat(buffer, temp);
    snprintf(temp, sizeof(temp), "<p>Alive Time: <span>%lu s</span></p>", alive_time_ms / 1000); strcat(buffer, temp);
    strcat(buffer, "</div>");

    // --- Environment Box ---
    strcat(buffer, "<div class='box'><h2>Environment</h2>");
    snprintf(temp, sizeof(temp), "<p>Light Level: <span>%.3f</span></p>", light_level); strcat(buffer, temp);
    snprintf(temp, sizeof(temp), "<p>Distance: <span>%.1f cm</span></p>", ultrasonic.readDistance()); strcat(buffer, temp);
    strcat(buffer, "</div>");

    // --- Genome Box ---
    strcat(buffer, "<div class='box'><h2>Genome</h2>");
    snprintf(temp, sizeof(temp), "<p>Generation: <span>%u</span></p>", genome.generation); strcat(buffer, temp);
    snprintf(temp, sizeof(temp), "<p>Light Threshold: <span>%.3f</span></p>", genome.light_threshold); strcat(buffer, temp);
    snprintf(temp, sizeof(temp), "<p>Efficiency: <span>%.3f</span></p>", genome.efficiency); strcat(buffer, temp);
    strcat(buffer, "</div>");

    // --- Power Box ---
    strcat(buffer, "<div class='box'><h2>Power</h2>");
    if (battery.mode == POWER_USB_DEBUG) {
        snprintf(temp, sizeof(temp), "<p>Status: <span>DEBUGGING</span></p><p>Voltage: <span>%.1fV</span></p>", battery.voltage);
    } else {
        snprintf(temp, sizeof(temp), "<p>Percentage: <span>%.1f%%</span></p><p>Voltage: <span>%.1fV</span></p>", battery.percentage, battery.voltage);
    }
    strcat(buffer, temp);
    strcat(buffer, "</div>");

    strcat(buffer, "</div>"); // End Grid

    // --- Actions Box ---
    strcat(buffer, "<div class='box actions'><h2>Controls</h2>");
    strcat(buffer, "<a href='/reset'>Reset Life</a>");
    strcat(buffer, "<a href='/api/stats' target='_blank'>JSON API</a>");
    strcat(buffer, "</div>");

    // Footer
    strcat(buffer, "</div></body></html>");

    server.send(200, "text/html", buffer);
}

void handleAPIStats() {
    char buffer[512];
    snprintf(buffer, sizeof(buffer),
        "{\"bot_id\":%d,\"generation\":%u,\"alive\":%s,\"energy\":%.1f,"
        "\"light_level\":%.3f,\"light_left\":%.3f,\"light_right\":%.3f,"
        "\"distance_cm\":%.1f,\"battery_v\":%.2f,\"battery_pct\":%.1f,"
        "\"power_mode\":%d,\"alive_time_s\":%lu}",
        genome.bot_id, genome.generation, is_alive ? "true" : "false", energy,
        light_level, lightSensor.readLeft(), lightSensor.readRight(),
        ultrasonic.readDistance(), battery.voltage, battery.percentage,
        battery.mode, alive_time_ms / 1000);

    server.send(200, "application/json", buffer);
}

void handleReset() {
    energy = 100.0f;
    is_alive = true;
    boot_time_ms = millis();
    Serial.println("[Web] Life has been reset via web.");
    server.sendHeader("Location", "/");
    server.send(303);
}

void handleNotFound() {
    server.send(404, "text/plain", "Not Found");
}

void setupWebServer() {
    server.on("/", HTTP_GET, handleRoot);
    server.on("/api/stats", HTTP_GET, handleAPIStats);
    server.on("/reset", HTTP_GET, handleReset);
    server.onNotFound(handleNotFound);

    server.begin();
    Serial.println("[Web] Server started.");
}