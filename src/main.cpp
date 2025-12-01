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
#include "globals.h"
#include "avoidance.h"
#include "seeking.h"
#include "cli.h"

using namespace HAL;

// ============================================================================
// GLOBAL INSTANCES
// ============================================================================

Genome genome;           // The bot's genetic code
Preferences preferences; // For saving/loading the genome
WebServer server(80);    // Web server instance

// ============================================================================
// LIFE STATE
// ============================================================================

float energy = 100.0f;
float light_level = 0.0f;
unsigned long alive_time_ms = 0;
unsigned long boot_time_ms = 0;
bool is_alive = true;
bool manual_override = false;
BehaviorState_t currentBehavior = IDLE;

LifeParams_t life_params;
BatteryMonitor_t battery;

// Timers for managing non-blocking updates
unsigned long last_life_update_ms = 0;
unsigned long last_stats_print_ms = 0;
unsigned long last_wifi_check_ms = 0;
const unsigned long WIFI_CHECK_INTERVAL_MS = 30000; // 30 seconds interval for WiFi check

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

// ============================================================================
// FORWARD DECLARATIONS
// ============================================================================
void updateLife(float dt);
void decideBehavior();
void executeBehavior();
void setupWebServer();
void updateBatteryStatus();
void checkWiFiConnection();
void showState();
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
 * @brief Periodically checks the WiFi connection and attempts to reconnect if lost.
 *
 * This function is non-blocking. It initiates a reconnect and allows the main
 * loop to continue running.
 */
void checkWiFiConnection() {
    // Only check periodically
    if (millis() - last_wifi_check_ms < WIFI_CHECK_INTERVAL_MS) {
        return;
    }
    last_wifi_check_ms = millis();

    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("[WiFi] Connection lost. Attempting to reconnect...");
        led.yellow(50); // Give a brief visual cue
        WiFi.reconnect();
        delay(100); // Allow LED to flash
        showState(); // Restore normal LED state
    }
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

        genome.light_threshold = 0.3f; // A more forgiving initial threshold
        genome.efficiency = 0.75f + (random(0, 100) / 100.0f); // Random efficiency 0.75 to 1.75
        genome.turn_sensitivity = 200 + random(0, 600); // Random sensitivity from 200 to 800
        genome.base_speed = 150 + random(0, 100); // Random base speed from 150 to 250
        genome.bot_id = 0; // Default to 0, should be set uniquely
        genome.generation = 0;
        
        // Immediately save the new random genome
        saveGenome();
    }
    preferences.end();
    Serial.printf("[Genome] ID: %d, Gen: %d, Threshold: %.2f, Efficiency: %.2f\n", genome.bot_id, genome.generation, genome.light_threshold, genome.efficiency);
}

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

    // Initialize runtime life parameters from config constants
    life_params.energy_decay = Config::Life::ENERGY_DECAY;
    life_params.energy_gain = Config::Life::ENERGY_GAIN;
    life_params.movement_cost_multiplier = Config::Life::MOVEMENT_COST_MULTIPLIER;

    // Load the bot's identity and traits from "EEPROM"
    loadGenome();

    // Take the first reading to initialize the light_level variable
    light_level = lightSensor.readAverage();

    // Give the bot a starting chance based on its initial environment
    updateLife(0.1f); // Simulate a small time-slice for the initial energy calculation

    HAL::motors.begin();

    // Set boot timestamp after all initial setup
    boot_time_ms = millis();

    setupAvoidance();

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
    // Base existence cost
    float energy_cost = life_params.energy_decay * dt;
    
    // Movement adds additional cost
    if (currentBehavior == SEEKING_LIGHT || currentBehavior == AVOIDING_OBSTACLE) {
        energy_cost *= life_params.movement_cost_multiplier;
    }
    
    energy -= energy_cost;
    
    // Energy gain from light (unchanged)
    if (light_level > genome.light_threshold) {
        float excess_light = light_level - genome.light_threshold;
        energy += life_params.energy_gain * excess_light * genome.efficiency * dt;
    }
    
    energy = constrain(energy, 0.0f, 100.0f);
    
    if (energy <= 0.0f) is_alive = false;
    if (!is_alive && energy > 0.0f) {
        is_alive = true;
        boot_time_ms = millis();
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
    // If manual override is active, don't change the behavior.
    if (manual_override) {
        return;
    }

    // --- Highest Priority: Obstacle Avoidance ---
    if (checkObstacle()) {
        currentBehavior = AVOIDING_OBSTACLE;
        return;
    }

    // Power state overrides all other decisions.
    if (battery.mode == POWER_CRITICAL || battery.mode == POWER_SHUTDOWN) {
        currentBehavior = IDLE; // Must conserve power
        return;
    }

    // The primary drive is survival. If energy is low, it must seek light.
    if (energy < 99.0f) { // Start seeking light almost immediately
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
            executeSeeking();
            break;
        }

        case AVOIDING_OBSTACLE: {
            executeAvoidance();
            break;
        }
    }
}

void loop() {
    ArduinoOTA.handle(); // Always handle network requests
    server.handleClient(); // Handle incoming web requests

    checkWiFiConnection();
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

// ============================================================================
// WEB SERVER IMPLEMENTATION
// ============================================================================

// HTML/CSS for the web interface. Using raw literals makes it easy to write.
const char* HTML_HEADER = R"rawliteral(
<!DOCTYPE html><html><head><title>EMBER Bot %d</title><meta http-equiv="refresh" content="2"><style>
body{font-family:monospace;background:#282c34;color:#abb2bf;padding:1em;}
.container{max-width:800px;margin:auto;}h1{color:#61afef;text-align:center;}
.grid{display:grid;grid-template-columns:repeat(auto-fit, minmax(300px, 1fr));gap:1em;}
.box{background:#323842;padding:1em;border-radius:8px;}
h2{color:#98c379;border-bottom:1px solid #444;padding-bottom:0.5em;margin-top:0;}
p{display:flex;justify-content:space-between;margin:0.5em 0;} span{color:#e5c07b;font-weight:bold;}
.actions{display:grid;grid-template-columns:1fr 1fr;gap:0.5em;}
.actions a{display:block;padding:0.8em;background:#61afef;color:#fff;text-decoration:none;text-align:center;border-radius:5px;}
.actions a.danger{background:#e06c75;}
</style></head><body><div class="container"><h1>&#128293; EMBER Bot %d</h1>
)rawliteral";

const char* HTML_FOOTER = R"rawliteral(
</div></body></html>
)rawliteral";

#include "web_server.h"