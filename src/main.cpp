#include <Arduino.h>
#include "hal.h"
#include "config.h"
#include "movement.h"
#include "status.h"
#include "sensors.h"
#include "behaviors.h"
#include "pins.h"

// ============================================================================
// GLOBAL OBJECTS
// ============================================================================

HAL hal;
MotorConfig motorConfig;
Movement movement(hal, motorConfig);
StatusLED status(hal);
UltrasonicSensor sensor(hal);
LDRSensor ldrSensor(hal);
ObstacleAvoidance autonomousMode(hal, movement, sensor, status, motorConfig);
Phototropism phototropismMode(hal, movement, status, ldrSensor);  // 

// ============================================================================
// TEST SEQUENCES
// ============================================================================

void runTestSequence() {
    Serial.println("\n→ Starting basic test sequence...");
    
    status.setStatus(StatusLED::MOVING);
    
    Serial.println("  Forward...");
    movement.forward(motorConfig.baseSpeed);
    delay(1500);
    
    Serial.println("  Backward...");
    movement.backward(motorConfig.baseSpeed);
    delay(1500);
    
    Serial.println("  Turn Right...");
    movement.turnRight(motorConfig.baseSpeed);
    delay(motorConfig.turnDuration);
    
    Serial.println("  Spin CW...");
    movement.spinCW(motorConfig.baseSpeed);
    delay(1000);
    
    Serial.println("  Spin CCW...");
    movement.spinCCW(motorConfig.baseSpeed);
    delay(1000);
    
    Serial.println("  Crawl...");
    movement.crawl();
    delay(1500);
    
    Serial.println("  Run...");
    movement.run();
    delay(1500);
    
    movement.stop();
    status.setStatus(StatusLED::READY);
    Serial.println("✓ Test sequence complete\n");
}

void runSmoothTestSequence() {
    Serial.println("\n→ Starting smooth movement test...");
    
    status.setStatus(StatusLED::MOVING);
    
    Serial.println("  Gentle acceleration from stop...");
    movement.smoothForward(motorConfig.crawlSpeed);
    delay(1500);
    
    Serial.println("  Ramping to full speed...");
    movement.smoothForward(motorConfig.maxSpeed);
    delay(2000);
    
    Serial.println("  Gentle deceleration to stop...");
    movement.smoothStop();
    delay(500);
    
    Serial.println("  Smooth backward start...");
    movement.smoothBackward(motorConfig.baseSpeed);
    delay(1500);
    
    Serial.println("  Smooth stop from backward...");
    movement.smoothStop();
    delay(500);
    
    Serial.println("  Testing direction change...");
    movement.smoothForward(motorConfig.baseSpeed);
    delay(1000);
    Serial.println("    (Stopping first...)");
    movement.smoothStop();
    delay(200);
    Serial.println("    (Now backward...)");
    movement.smoothBackward(motorConfig.baseSpeed);
    delay(1000);
    movement.smoothStop();
    
    status.setStatus(StatusLED::READY);
    Serial.println("✓ Smooth test complete\n");
}

void runRGBTest() {
    Serial.println("\n→ Testing RGB LED...");
    
    Serial.println("  Red...");
    status.setStatus(StatusLED::ERROR);
    delay(1000);
    
    Serial.println("  Green...");
    status.setStatus(StatusLED::READY);
    delay(1000);
    
    Serial.println("  Blue...");
    status.setStatus(StatusLED::MOVING);
    delay(1000);
    
    Serial.println("  Yellow...");
    status.setStatus(StatusLED::OBSTACLE);
    delay(1000);
    
    Serial.println("  Cyan...");
    status.setStatus(StatusLED::SEARCHING);
    delay(1000);
    
    Serial.println("  Purple...");
    status.setStatus(StatusLED::CALIBRATING);
    delay(1000);
    
    Serial.println("  Blinking (error)...");
    status.setStatus(StatusLED::ERROR);
    delay(3000);
    
    Serial.println("  Blinking (OTA)...");
    status.setStatus(StatusLED::OTA_UPDATE);
    delay(3000);
    
    status.setStatus(StatusLED::READY);
    Serial.println("✓ RGB test complete\n");
}

void printHelp() {
    Serial.println("\n╔════════════════════════════════════════╗");
    Serial.println("║          EMBER COMMAND HELP           ║");
    Serial.println("╚════════════════════════════════════════╝");
    Serial.println();
    Serial.println("Basic Movement:");
    Serial.println("  f/F - Forward       b/B - Backward");
    Serial.println("  r/R - Turn Right");
    Serial.println("  < - Spin CCW        > - Spin CW");
    Serial.println("  c/C - Crawl (slow)  m/M - Run (fast)");
    Serial.println();
    Serial.println("Control:");
    Serial.println("  s/S - Stop (disables autonomous)");
    Serial.println("  SPACE - Emergency stop");
    Serial.println();
    Serial.println("Smooth Movement:");
    Serial.println("  w/W - Smooth Forward");
    Serial.println("  x/X - Smooth Backward");
    Serial.println("  q/Q - Smooth Stop");
    Serial.println();
    Serial.println("Test Sequences:");
    Serial.println("  t/T - Basic movement test");
    Serial.println("  y/Y - Smooth movement test");
    Serial.println("  g/G - RGB LED test");
    Serial.println();
    Serial.println("Information:");
    Serial.println("  h/H - This help menu");
    Serial.println("  i/I - Show system info");
    Serial.println();
    Serial.println("Sensors:");
    Serial.println("  u/U - Read ultrasonic");
    Serial.println("  l/L - Read LDR sensors (light)");
    Serial.println("  p/P - Show sensor status (dist, stuck, batt)");
    Serial.println("  j/J - Show motor driver pin status");
    Serial.println();
    Serial.println("Autonomous:");
    Serial.println("  a/A - Toggle autonomous mode");
    Serial.println("  k/K - Toggle phototropism mode (light seeking)"); 
}

void printSystemInfo() {
    Serial.println("\n╔════════════════════════════════════════╗");
    Serial.println("║         EMBER SYSTEM INFO             ║");
    Serial.println("╚════════════════════════════════════════╝");
    Serial.println();
    Serial.println("Hardware:");
    Serial.printf("  Chip: %s\n", ESP.getChipModel());
    Serial.printf("  Cores: %d\n", ESP.getChipCores());
    Serial.printf("  CPU Freq: %d MHz\n", ESP.getCpuFreqMHz());
    Serial.printf("  Flash: %d MB\n", ESP.getFlashChipSize() / 1048576);
    Serial.printf("  Free Heap: %d KB\n", ESP.getFreeHeap() / 1024);
    Serial.println();
    Serial.println("Motor Configuration:");
    Serial.printf("  Base Speed: %d\n", motorConfig.baseSpeed);
    Serial.printf("  Crawl Speed: %d\n", motorConfig.crawlSpeed);
    Serial.printf("  Max Speed: %d\n", motorConfig.maxSpeed);
    Serial.printf("  Turn Duration: %d ms\n", motorConfig.turnDuration);
    Serial.printf("  Motor A Inverted: %s\n", motorConfig.motorA_inverted ? "Yes" : "No");
    Serial.printf("  Motor B Inverted: %s\n", motorConfig.motorB_inverted ? "Yes" : "No");
    Serial.printf("  Motor A Trim: %+d\n", motorConfig.motorA_trim);
    Serial.printf("  Motor B Trim: %+d\n", motorConfig.motorB_trim);
    Serial.println();
    Serial.println("Current State:");
    Serial.printf("  Moving: %s\n", movement.isMoving() ? "Yes" : "No");
    Serial.printf("  Speed: %d\n", movement.getCurrentSpeed());
    Serial.printf("  Status LED: ");
    switch(status.getCurrentStatus()) {
        case StatusLED::OFF: Serial.println("Off"); break;
        case StatusLED::BOOTING: Serial.println("Booting (Red)"); break;
        case StatusLED::READY: Serial.println("Ready (Green)"); break;
        case StatusLED::MOVING: Serial.println("Moving (Blue)"); break;
        case StatusLED::OBSTACLE: Serial.println("Obstacle (Yellow)"); break;
        case StatusLED::SEARCHING: Serial.println("Searching (Cyan)"); break;
        case StatusLED::ERROR: Serial.println("Error (Red Blink)"); break;
        case StatusLED::CALIBRATING: Serial.println("Calibrating (Purple)"); break;
        case StatusLED::OTA_UPDATE: Serial.println("OTA Update (White Blink)"); break;
    }
    Serial.println();
}

void printMotorDriverStatus() {
    Serial.println("\n--- Motor Driver Pin Status ---");
    // Read the actual digital state of the output pins
    Serial.println("Direction Pins:");
    Serial.printf("  Motor A (Left):  AIN1(15)=%d, AIN2(2)=%d\n", digitalRead(Pins::MOTOR_A_IN1), digitalRead(Pins::MOTOR_A_IN2));
    Serial.printf("  Motor B (Right): BIN1(16)=%d, BIN2(17)=%d\n", digitalRead(Pins::MOTOR_B_IN1), digitalRead(Pins::MOTOR_B_IN2));
    Serial.println("\nSpeed (PWM) and Standby:");
    // Note: We can't directly read the PWM value, but we can check STBY
    Serial.printf("  STBY Pin (13): %s\n", digitalRead(Pins::MOTOR_STBY) ? "HIGH (Enabled)" : "LOW (DISABLED!)");
    Serial.println("\nHidden State (from Movement class):");
    Serial.printf("  Current Speed: %d\n", movement.getCurrentSpeed());
    Serial.printf("  Is Moving: %s\n", movement.isMoving() ? "Yes" : "No");
    Serial.println();
}

// ============================================================================
// SETUP
// ============================================================================

void setup() {
    Serial.begin(115200);
    delay(1000); // Give serial time to stabilize
    
    // Show we're booting
    status.setStatus(StatusLED::BOOTING);
    
    Serial.println("\n\n");
    Serial.println("╔════════════════════════════════════════╗");
    Serial.println("║      EMBER v0.3 - Mobile Life         ║");
    Serial.println("║     Phase 3B: Phototropism (Moth)     ║");
    Serial.println("╚════════════════════════════════════════╝");
    Serial.println();
    
    if (!hal.init()) {
        Serial.println("❌ HAL initialization FAILED!");
        status.setStatus(StatusLED::ERROR);
        while (true) { 
            delay(1000); 
        }
    }
    
    Serial.println("✓ HAL initialized");
    Serial.println("✓ PWM configured (Motors: 20kHz, RGB: 5kHz)");
    Serial.println();
    
    Serial.println("Motor Configuration:");
    Serial.printf("  Base Speed: %d\n", motorConfig.baseSpeed);
    Serial.printf("  Crawl Speed: %d\n", motorConfig.crawlSpeed);
    Serial.printf("  Max Speed: %d\n", motorConfig.maxSpeed);
    Serial.printf("  Turn Duration: %d ms\n", motorConfig.turnDuration);
    Serial.println();
    
    movement.stop();
    status.setStatus(StatusLED::READY);
    
    Serial.println("✓ Robot Ready");
    Serial.println();
    Serial.println("Press 'h' for help, 'l' to read LDR sensors");
    Serial.print("> ");
}

// ============================================================================
// MAIN LOOP
// ============================================================================

void loop() {
    // Check for serial commands
    if (Serial.available()) {
        char cmd = Serial.read();
        
        // Clear buffer
        while (Serial.available() && (Serial.peek() == '\n' || Serial.peek() == '\r')) {
            Serial.read();
        }
        
        switch (cmd) {
            // ================================================================
            // BASIC MOVEMENT
            // ================================================================
            case 'f': case 'F':
                Serial.println("→ Forward");
                status.setStatus(StatusLED::MOVING);
                movement.forward(motorConfig.baseSpeed);
                break;
                
            case 'b': case 'B':
                Serial.println("← Backward");
                status.setStatus(StatusLED::MOVING);
                movement.backward(motorConfig.baseSpeed);
                break;
                
            case 'r': case 'R':
                Serial.println("↻ Turn Right");
                status.setStatus(StatusLED::MOVING);
                movement.turnRight(motorConfig.baseSpeed);
                delay(motorConfig.turnDuration);
                movement.stop();
                status.setStatus(StatusLED::READY);
                Serial.println("  (Turn complete)");
                break;
                
            case '<':
                Serial.println("⟲ Spin CCW");
                status.setStatus(StatusLED::MOVING);
                movement.spinCCW(motorConfig.baseSpeed);
                break;
                
            case '>':
                Serial.println("⟳ Spin CW");
                status.setStatus(StatusLED::MOVING);
                movement.spinCW(motorConfig.baseSpeed);
                break;
                
            case 'c': case 'C':
                Serial.println("🐌 Crawl");
                status.setStatus(StatusLED::MOVING);
                movement.crawl();
                break;
                
            case 'm': case 'M':
                Serial.println("🏃 Run");
                status.setStatus(StatusLED::MOVING);
                movement.run();
                break;
                
            case 's': case 'S':
                Serial.println("⏹ Stop");
                // Disable autonomous mode if running
                if (autonomousMode.isEnabled()) {
                    autonomousMode.disable();
                } else {
                    movement.stop();
                    status.setStatus(StatusLED::READY);
                }
                break;
            
            // ================================================================
            // SMOOTH MOVEMENT
            // ================================================================
            case 'w': case 'W':
                Serial.println("→ Smooth Forward");
                status.setStatus(StatusLED::MOVING);
                movement.smoothForward(motorConfig.baseSpeed);
                break;
                
            case 'x': case 'X':
                Serial.println("← Smooth Backward");
                status.setStatus(StatusLED::MOVING);
                movement.smoothBackward(motorConfig.baseSpeed);
                break;
                
            case 'q': case 'Q':
                Serial.println("⏹ Smooth Stop");
                movement.smoothStop();
                status.setStatus(StatusLED::READY);
                break;
            
            // ================================================================
            // TEST SEQUENCES
            // ================================================================
            case 't': case 'T':
                runTestSequence();
                break;
                
            case 'y': case 'Y':
                runSmoothTestSequence();
                break;
                
            case 'g': case 'G':
                runRGBTest();
                break;
            
            // ================================================================
            // INFORMATION
            // ================================================================
            case 'h': case 'H':
                printHelp();
                break;
                
            case 'i': case 'I':
                printSystemInfo();
                break;

            // ================================================================
            // ULTRASONIC READING
            // ================================================================                
            case 'u': case 'U':
                {
                    int dist = hal.readUltrasonic();
                    Serial.printf("Distance: %d cm\n", dist);
                }
                break;

            // ================================================================
            // LDR READING (Phase 3A)
            // ================================================================
            case 'l': case 'L':
                {
                    float leftBright = ldrSensor.getLeftBrightness();
                    float rightBright = ldrSensor.getRightBrightness();
                    Serial.println("--- LDR Sensors ---");
                    Serial.printf("  Left:  %.3f (0=dark, 1=bright)\n", leftBright);
                    Serial.printf("  Right: %.3f (0=dark, 1=bright)\n", rightBright);
                    Serial.printf("  Difference: %.3f ", abs(leftBright - rightBright));
                    if (leftBright > rightBright + 0.1f) {
                        Serial.println("(LEFT brighter)");
                    } else if (rightBright > leftBright + 0.1f) {
                        Serial.println("(RIGHT brighter)");
                    } else {
                        Serial.println("(balanced)");
                    }
                }
                break;

            // ================================================================
            // SENSOR DIAGNOSTICS
            // ================================================================
            case 'p': case 'P':
                {
                    int dist = sensor.getDistance();
                    bool stuck = sensor.isStuck();
                    float voltage = hal.readBatteryVoltage();
                    Serial.println("--- Sensor Status ---");
                    Serial.printf("  Filtered Distance: %d cm\n", dist);
                    Serial.printf("  Is Stuck: %s\n", stuck ? "YES" : "No");
                    Serial.printf("  Battery Voltage: %.2f V\n", voltage);
                }
                break;

            // ================================================================
            // MOTOR DIAGNOSTICS
            // ================================================================
            case 'j': case 'J':
                printMotorDriverStatus();
                break;

            // ================================================================
            // AUTONOMOUS MODE
            // ================================================================
            case 'a': case 'A':
                if (autonomousMode.isEnabled()) {
                    autonomousMode.disable();
                } else {
                    autonomousMode.enable();
                }
                break;                

            // ================================================================
            // PHOTOTROPISM MODE (Phase 3B)
            // ================================================================
            case 'k': case 'K':
                if (phototropismMode.isEnabled()) {
                    phototropismMode.disable();
                } else {
                    phototropismMode.enable();
                }
                break;
                                
            // ================================================================
            // EMERGENCY STOP
            // ================================================================

            case ' ': // Spacebar for emergency stop
                Serial.println("🛑 EMERGENCY STOP");
                autonomousMode.disable();
                movement.stop();
                status.setStatus(StatusLED::READY);
                break;

            // ================================================================
            // UNKNOWN
            // ================================================================
            default:
                Serial.println("❓ Unknown command. Press 'h' for help.");
                break;
        }
        
        // Print prompt
        Serial.print("> ");
    }
    
    // Update status LED (for animations/blinking)
    status.update();

    // Always update sensors so diagnostics are live
    sensor.update();
    ldrSensor.update();

    // Update autonomous mode
    autonomousMode.update();
    // Update phototropism mode
    phototropismMode.update();
    
    delay(5);
}