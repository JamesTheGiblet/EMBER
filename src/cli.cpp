#include <Arduino.h>
#include "hal.h"

void processSerialCommands(HAL& hal) {
    if (Serial.available()) {
        String cmd = Serial.readStringUntil('\n');
        cmd.trim();
        if (cmd.equalsIgnoreCase("STOP")) {
            hal.stopMotors();
            Serial.println("Motors stopped.");
        } else {
            char motor = toupper(cmd.charAt(0));
            char dir = toupper(cmd.charAt(2));
            int speed = cmd.substring(4).toInt();
            bool forward = (dir == 'F');
            if (motor == 'A') {
                hal.setMotorA(speed, forward);
                Serial.printf("Motor A %s at %d\n", forward ? "forward" : "reverse", speed);
            } else if (motor == 'B') {
                hal.setMotorB(speed, forward);
                Serial.printf("Motor B %s at %d\n", forward ? "forward" : "reverse", speed);
            } else {
                Serial.println("Invalid command. Use: A/B F/R speed or STOP");
            }
        }
    }
}
