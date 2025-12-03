#include <Arduino.h>
#include "hal.h"

HAL hal;

enum CalibState {
    INIT,
    MENU,
    TEST_FORWARD,
    TEST_BACKWARD,
    TEST_TURN_LEFT,
    TEST_TURN_RIGHT,
    TEST_SPIN_CW,
    TEST_SPIN_CCW,
    TEST_CRAWL,
    TEST_RUN,
    TEST_MOTOR_A_ONLY,
    TEST_MOTOR_B_ONLY,
    AWAITING_INPUT,
    COMPLETE
};

struct CalibrationData {
    int baseSpeed = 150;
    int minSpeed = 80;
    int maxSpeed = 255;
    int crawlSpeed = 100;
    int turnDuration = 800;
    int moveDuration = 1500;
    bool motorA_inverted = false;
    bool motorB_inverted = false;
    int motorA_trim = 0;  // -50 to +50
    int motorB_trim = 0;
};

CalibState state = INIT;
CalibrationData calib;
unsigned long testStartTime = 0;
unsigned long testDuration = 0;
bool motorRunning = false;

// LED Status Colors
void showStatus(uint8_t r, uint8_t g, uint8_t b) {
    hal.setRGB(r, g, b);
}

void stopTest() {
    hal.stopMotors();
    motorRunning = false;
    showStatus(255, 255, 0); // Yellow = waiting
}

void printMenu() {
    Serial.println("\n╔════════════════════════════════════════╗");
    Serial.println("║   EMBER MOTOR CALIBRATION WIZARD      ║");
    Serial.println("╚════════════════════════════════════════╝");
    Serial.println();
    Serial.println("Basic Tests:");
    Serial.println("  1 - Forward           6 - Spin CCW");
    Serial.println("  2 - Backward          7 - Crawl (slow)");
    Serial.println("  3 - Turn Left         8 - Run (fast)");
    Serial.println("  4 - Turn Right        9 - Motor A only");
    Serial.println("  5 - Spin CW           0 - Motor B only");
    Serial.println();
    Serial.println("Adjustments:");
    Serial.println("  + - Increase speed    [ - Decrease duration");
    Serial.println("  - - Decrease speed    ] - Increase duration");
    Serial.println("  A - Invert Motor A    T - Trim Motor A (+)");
    Serial.println("  B - Invert Motor B    Y - Trim Motor B (+)");
    Serial.println("  R - Reset calibration G - Trim Motor A (-)");
    Serial.println("  S - Show settings     H - Trim Motor B (-)");
    Serial.println();
    Serial.println("Control:");
    Serial.println("  SPACE - Emergency stop");
    Serial.println("  D - Done (print config)");
    Serial.println();
    Serial.printf("Current: Speed=%d Duration=%dms Trim A=%d B=%d\n", 
                  calib.baseSpeed, calib.moveDuration, calib.motorA_trim, calib.motorB_trim);
    Serial.print("> ");
}

void runTest(CalibState test) {
    showStatus(0, 0, 255); // Blue = testing
    motorRunning = true;
    testStartTime = millis();
    
    int speedA = calib.baseSpeed + calib.motorA_trim;
    int speedB = calib.baseSpeed + calib.motorB_trim;
    
    speedA = constrain(speedA, calib.minSpeed, calib.maxSpeed);
    speedB = constrain(speedB, calib.minSpeed, calib.maxSpeed);
    
    switch (test) {
        case TEST_FORWARD:
            Serial.println("→ Moving FORWARD");
            hal.setMotorA(speedA, !calib.motorA_inverted);
            hal.setMotorB(speedB, !calib.motorB_inverted);
            testDuration = calib.moveDuration;
            break;
            
        case TEST_BACKWARD:
            Serial.println("← Moving BACKWARD");
            hal.setMotorA(speedA, calib.motorA_inverted);
            hal.setMotorB(speedB, calib.motorB_inverted);
            testDuration = calib.moveDuration;
            break;
            
        case TEST_TURN_LEFT:
            Serial.println("↺ Turning LEFT");
            hal.setMotorA(speedA, calib.motorA_inverted);    // Motor A (Left) backwards
            hal.setMotorB(speedB, !calib.motorB_inverted);   // Motor B (Right) forwards
            testDuration = calib.turnDuration;
            break;
            
        case TEST_TURN_RIGHT:
            Serial.println("↻ Turning RIGHT");
            hal.setMotorA(speedA, !calib.motorA_inverted);    // Motor A (Left) forwards
            hal.setMotorB(speedB, calib.motorB_inverted);    // Motor B (Right) backwards
            testDuration = calib.turnDuration;
            break;
            
        case TEST_SPIN_CW:
            Serial.println("⟳ Spinning CLOCKWISE");
            hal.setMotorA(speedA, !calib.motorA_inverted);
            hal.setMotorB(speedB, calib.motorB_inverted);
            testDuration = calib.moveDuration;
            break;
            
        case TEST_SPIN_CCW:
            Serial.println("⟲ Spinning COUNTER-CLOCKWISE");
            hal.setMotorA(speedA, calib.motorA_inverted);
            hal.setMotorB(speedB, !calib.motorB_inverted);
            testDuration = calib.moveDuration;
            break;
            
        case TEST_CRAWL:
            Serial.println("🐌 CRAWLING (slow)");
            hal.setMotorA(calib.crawlSpeed + calib.motorA_trim/2, !calib.motorA_inverted);
            hal.setMotorB(calib.crawlSpeed + calib.motorB_trim/2, !calib.motorB_inverted);
            testDuration = calib.moveDuration;
            break;
            
        case TEST_RUN:
            Serial.println("🏃 RUNNING (fast)");
            hal.setMotorA(calib.maxSpeed, !calib.motorA_inverted);
            hal.setMotorB(calib.maxSpeed, !calib.motorB_inverted);
            testDuration = calib.moveDuration;
            break;
            
        case TEST_MOTOR_A_ONLY:
            Serial.println("Motor A ONLY");
            hal.setMotorA(speedA, !calib.motorA_inverted);
            hal.setMotorB(0, true);
            testDuration = calib.moveDuration;
            break;
            
        case TEST_MOTOR_B_ONLY:
            Serial.println("Motor B ONLY");
            hal.setMotorA(0, true);
            hal.setMotorB(speedB, !calib.motorB_inverted);
            testDuration = calib.moveDuration;
            break;
            
        default:
            stopTest();
            break;
    }
}

void printConfiguration() {
    Serial.println("\n╔════════════════════════════════════════╗");
    Serial.println("║     CALIBRATION CONFIGURATION         ║");
    Serial.println("╚════════════════════════════════════════╝");
    Serial.println("\n// Add to your bot configuration:");
    Serial.println("struct MotorConfig {");
    Serial.printf("    int baseSpeed = %d;\n", calib.baseSpeed);
    Serial.printf("    int crawlSpeed = %d;\n", calib.crawlSpeed);
    Serial.printf("    int maxSpeed = %d;\n", calib.maxSpeed);
    Serial.printf("    int turnDuration = %d;\n", calib.turnDuration);
    Serial.printf("    bool motorA_inverted = %s;\n", calib.motorA_inverted ? "true" : "false");
    Serial.printf("    bool motorB_inverted = %s;\n", calib.motorB_inverted ? "true" : "false");
    Serial.printf("    int motorA_trim = %d;\n", calib.motorA_trim);
    Serial.printf("    int motorB_trim = %d;\n", calib.motorB_trim);
    Serial.println("};");
    Serial.println();
}

void processCommand(char cmd) {
    switch (cmd) {
        case '1': runTest(TEST_FORWARD); break;
        case '2': runTest(TEST_BACKWARD); break;
        case '3': runTest(TEST_TURN_LEFT); break;
        case '4': runTest(TEST_TURN_RIGHT); break;
        case '5': runTest(TEST_SPIN_CW); break;
        case '6': runTest(TEST_SPIN_CCW); break;
        case '7': runTest(TEST_CRAWL); break;
        case '8': runTest(TEST_RUN); break;
        case '9': runTest(TEST_MOTOR_A_ONLY); break;
        case '0': runTest(TEST_MOTOR_B_ONLY); break;
        
        case '+':
            calib.baseSpeed = min(calib.baseSpeed + 10, calib.maxSpeed);
            Serial.printf("Speed increased to %d\n", calib.baseSpeed);
            break;
        case '-':
            calib.baseSpeed = max(calib.baseSpeed - 10, calib.minSpeed);
            Serial.printf("Speed decreased to %d\n", calib.baseSpeed);
            break;
            
        case '[':
            calib.moveDuration = max(calib.moveDuration - 100, 500);
            Serial.printf("Duration decreased to %dms\n", calib.moveDuration);
            break;
        case ']':
            calib.moveDuration = min(calib.moveDuration + 100, 3000);
            Serial.printf("Duration increased to %dms\n", calib.moveDuration);
            break;
            
        case 'A': case 'a':
            calib.motorA_inverted = !calib.motorA_inverted;
            Serial.printf("Motor A inverted: %s\n", calib.motorA_inverted ? "YES" : "NO");
            break;
        case 'B': case 'b':
            calib.motorB_inverted = !calib.motorB_inverted;
            Serial.printf("Motor B inverted: %s\n", calib.motorB_inverted ? "YES" : "NO");
            break;
            
        case 'T': case 't':
            calib.motorA_trim = min(calib.motorA_trim + 5, 50);
            Serial.printf("Motor A trim: %+d\n", calib.motorA_trim);
            break;
        case 'G': case 'g':
            calib.motorA_trim = max(calib.motorA_trim - 5, -50);
            Serial.printf("Motor A trim: %+d\n", calib.motorA_trim);
            break;
        case 'Y': case 'y':
            calib.motorB_trim = min(calib.motorB_trim + 5, 50);
            Serial.printf("Motor B trim: %+d\n", calib.motorB_trim);
            break;
        case 'H': case 'h':
            calib.motorB_trim = max(calib.motorB_trim - 5, -50);
            Serial.printf("Motor B trim: %+d\n", calib.motorB_trim);
            break;
            
        case 'R': case 'r':
            calib = CalibrationData();
            Serial.println("Reset to defaults");
            break;
            
        case 'S': case 's':
            printConfiguration();
            break;
            
        case 'D': case 'd':
            stopTest();
            printConfiguration();
            showStatus(0, 255, 0); // Green = done
            state = COMPLETE;
            Serial.println("Calibration saved! Reset to run again.");
            return;
            
        case ' ':
            stopTest();
            Serial.println("⚠ EMERGENCY STOP");
            break;
            
        default:
            Serial.println("Unknown command");
            break;
    }
    
    if (cmd != ' ' && cmd != 'D' && cmd != 'd') {
        Serial.print("> ");
    }
}

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    showStatus(255, 0, 0); // Red = initializing
    
    if (!hal.init()) {
        Serial.println("❌ HAL initialization FAILED!");
        showStatus(255, 0, 0); // Stay red
        while (true) { delay(1000); }
    }
    
    showStatus(0, 255, 0); // Green = ready
    delay(500);
    
    printMenu();
    showStatus(255, 255, 0); // Yellow = awaiting command
    state = MENU;
}

void loop() {
    // Auto-stop after test duration
    if (motorRunning && (millis() - testStartTime >= testDuration)) {
        stopTest();
        Serial.println("✓ Test complete");
        Serial.print("> ");
    }
    
    // Process serial commands
    if (Serial.available()) {
        char cmd = Serial.read();
        
        // Clear any remaining newlines
        while (Serial.available() && (Serial.peek() == '\n' || Serial.peek() == '\r')) {
            Serial.read();
        }
        
        if (state == COMPLETE) {
            Serial.println("Reset device to calibrate again");
        } else {
            processCommand(cmd);
        }
    }
    
    delay(10);
}