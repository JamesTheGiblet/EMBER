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

// Include Hardware Abstraction Layer and credentials
#include "hal.h"
#include "credentials_user.h"

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
            led.magenta(200); // Magenta indicates an OTA update is in progress
            type = "sketch";
        } else { // U_SPIFFS
            type = "filesystem";
        }
        Serial.println("[OTA] Start updating " + type);
    });

    ArduinoOTA.onEnd([]() {
        led.green(255); // Bright green for a successful update
        Serial.println("\n[OTA] Update Complete!");
        delay(1000);
    });

    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("[OTA] Progress: %u%%\r", (progress / (total / 100)));
    });

    ArduinoOTA.onError([](ota_error_t error) {
        led.red(255); // Bright red for a failed update
        Serial.printf("[OTA] Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR) Serial.println("Authentication Failed");
        else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
        else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
        else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
        else if (error == OTA_END_ERROR) Serial.println("End Failed");
        delay(2000);
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
// SETUP
// ============================================================================
void setup() {
    Serial.begin(115200);
    Serial.println("\n\n--- EMBER OTA Firmware Booting ---");

    // Initialize hardware from the HAL
    led.begin();
    led.yellow(50); // Dim yellow to indicate booting

    setupWiFi();
    setupMDNS();
    setupOTA();

    led.off(); // Turn LED off after setup is complete
    Serial.println("\n--- Boot complete. Waiting for OTA upload. ---");
}

// ============================================================================
// LOOP
// ============================================================================
void loop() {
    // ArduinoOTA.handle() must be called regularly to listen for update requests.
    ArduinoOTA.handle();

    // You can add other non-blocking code here in the future.
    // For now, we just keep the device responsive to OTA.
    
    // Add a "heartbeat" pulse to show the main loop is running.
    // A slow blue pulse indicates it's idle and waiting for commands.
    long t = millis();
    int brightness = (t % 2000 < 1000) ? (t % 1000) / 10 : 100 - (t % 1000) / 10;
    led.blue(brightness);

    delay(10);
}