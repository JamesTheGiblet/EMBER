#include <Arduino.h>
#include "pins.h"

// --- Configuration ---
const int SAMPLING_DURATION_MS = 3000; // How long to sample for each reading.

// --- State Machine ---
enum CalibrationState {
    START,
    WAIT_FOR_DARK,
    SAMPLING_DARK,
    WAIT_FOR_LIGHT,
    SAMPLING_LIGHT,
    DONE
};
CalibrationState currentState = START;

// --- Data Storage ---
float darkLeft, darkRight;
float lightLeft, lightRight;

// --- Timing & Sampling ---
unsigned long stateStartTime;
float maxLeft, maxRight;

void setup() {
    Serial.begin(115200);
    pinMode(Pins::LDR_LEFT, INPUT);
    pinMode(Pins::LDR_RIGHT, INPUT);
}

void loop() {
    switch (currentState) {
        case START:
            Serial.println("\n\n--- Interactive LDR Calibration Wizard ---");
            Serial.println("\nStep 1: Calibrate DARKNESS");
            Serial.println(">>> Please completely cover both LDR sensors.");
            Serial.println(">>> When you are ready, type 'ready' and press Enter.");
            currentState = WAIT_FOR_DARK;
            break;

        case WAIT_FOR_DARK:
            if (Serial.available() > 0) {
                String cmd = Serial.readStringUntil('\n');
                cmd.trim();
                if (cmd.equalsIgnoreCase("ready")) {
                    Serial.println("\nStarting DARKNESS sampling for 3 seconds... Please wait.");
                    stateStartTime = millis();
                    maxLeft = 0.0f;
                    maxRight = 0.0f;
                    currentState = SAMPLING_DARK;
                }
            }
            break;

        case SAMPLING_DARK:
            if (millis() - stateStartTime < SAMPLING_DURATION_MS) {
                // During sampling, find the highest normalized value (which corresponds to darkness)
                float normLeft = analogRead(Pins::LDR_LEFT) / 4095.0f;
                float normRight = analogRead(Pins::LDR_RIGHT) / 4095.0f;
                if (normLeft > maxLeft) maxLeft = normLeft;
                if (normRight > maxRight) maxRight = normRight;
                delay(10); // Sample frequently
            } else {
                darkLeft = maxLeft;
                darkRight = maxRight;
                Serial.printf("Darkness values recorded: L=%.3f, R=%.3f\n", darkLeft, darkRight);
                
                Serial.println("\nStep 2: Calibrate LIGHT");
                Serial.println(">>> Please shine a BRIGHT, DIRECT light on both sensors.");
                Serial.println(">>> When you are ready, type 'ready' and press Enter.");
                currentState = WAIT_FOR_LIGHT;
            }
            break;

        case WAIT_FOR_LIGHT:
            if (Serial.available() > 0) {
                String cmd = Serial.readStringUntil('\n');
                cmd.trim();
                if (cmd.equalsIgnoreCase("ready")) {
                    Serial.println("\nStarting LIGHT sampling for 3 seconds... Please wait.");
                    stateStartTime = millis();
                    maxLeft = 0.0f;
                    maxRight = 0.0f;
                    currentState = SAMPLING_LIGHT;
                }
            }
            break;

        case SAMPLING_LIGHT:
            if (millis() - stateStartTime < SAMPLING_DURATION_MS) {
                // During sampling, find the highest normalized value (which corresponds to brightness)
                float normLeft = analogRead(Pins::LDR_LEFT) / 4095.0f;
                float normRight = analogRead(Pins::LDR_RIGHT) / 4095.0f;
                if (normLeft > maxLeft) maxLeft = normLeft;
                if (normRight > maxRight) maxRight = normRight;
                delay(10);
            } else {
                lightLeft = maxLeft;
                lightRight = maxRight;
                Serial.printf("Light values recorded: L=%.3f, R=%.3f\n", lightLeft, lightRight);
                
                Serial.println("\n\n✅ Calibration Complete!");
                Serial.println("Copy the following block into your 'include/config.h' file, replacing the existing ADC namespace:");
                Serial.println("\n------------------- COPY BELOW -------------------");
                Serial.println("    namespace ADC {");
                Serial.println("        // The normalized ADC reading in darkness/light. Calibrated automatically.");
                Serial.printf("        constexpr float DARK_READING_LEFT  = %.3ff;\n", darkLeft);
                Serial.printf("        constexpr float LIGHT_READING_LEFT = %.3ff;\n", lightLeft);
                Serial.printf("        constexpr float DARK_READING_RIGHT = %.3ff;\n", darkRight);
                Serial.printf("        constexpr float LIGHT_READING_RIGHT= %.3ff;\n", lightRight);
                Serial.println("    }");
                Serial.println("-------------------- END COPY --------------------\n");
                Serial.println("Next steps:");
                Serial.println("1. Paste the code block into 'include/config.h'.");
                Serial.println("2. Restore your original 'src/main.cpp' file.");
                Serial.println("3. Upload the final, calibrated firmware to your bot.");
                
                currentState = DONE;
            }
            break;

        case DONE:
            // Do nothing. The process is complete.
            delay(1000);
            break;
    }
}