/*
 * web_server.cpp - EMBER Web Server Implementation
 */

#include <WebServer.h>
#include "hal.h"
#include "genome.h"
#include "globals.h"

extern LifeParams_t life_params;
extern WebServer server;
extern Genome genome;
extern float light_level;

// Forward declaration for the function defined in main.cpp
void saveGenome();

void handleRoot();
void handleAPIStats();
void handleReset();
void handleMutate();
void handleRandomize();
void handleSave();
void handleNotFound();
void setupWebServer();

void handleRoot() {
    char buffer[4096];
    char temp[200];
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
    strcat(buffer, "<div class='grid'>");
    strcat(buffer, "<div class='box'><h2>Life Status</h2>");
    snprintf(temp, sizeof(temp), "<p>Status: <span>%s</span></p>", is_alive ? "ALIVE" : "DEAD"); strcat(buffer, temp);
    snprintf(temp, sizeof(temp), "<p>Energy: <span>%.1f%%</span></p>", energy); strcat(buffer, temp);
    snprintf(temp, sizeof(temp), "<p>Alive Time: <span>%lu s</span></p>", alive_time_ms / 1000); strcat(buffer, temp);
    strcat(buffer, "</div>");
    strcat(buffer, "<div class='box'><h2>Environment</h2>");
    snprintf(temp, sizeof(temp), "<p>Light Level: <span>%.3f</span></p>", light_level); strcat(buffer, temp);
    snprintf(temp, sizeof(temp), "<form action='/set/decay' method='get'><p>Energy Decay: <input name='v' type='number' step='0.01' min='0.0' max='5.0' value='%.3f'><button type='submit'>Set</button></p></form>", life_params.energy_decay); strcat(buffer, temp);
    snprintf(temp, sizeof(temp), "<p>Distance: <span>%.1f cm</span></p>", HAL::ultrasonic.readDistance()); strcat(buffer, temp);
    strcat(buffer, "</div>");
    strcat(buffer, "<div class='box'><h2>Genome</h2>");
    snprintf(temp, sizeof(temp), "<p>Generation: <span>%u</span></p>", genome.generation); strcat(buffer, temp);
    snprintf(temp, sizeof(temp), "<form action='/set/threshold' method='get'><p>Light Threshold: <input name='v' type='number' step='0.01' min='0.01' max='1.0' value='%.3f'><button type='submit'>Set</button></p></form>", genome.light_threshold); strcat(buffer, temp);
    snprintf(temp, sizeof(temp), "<form action='/set/efficiency' method='get'><p>Efficiency: <input name='v' type='number' step='0.01' min='0.5' max='2.0' value='%.3f'><button type='submit'>Set</button></p></form>", genome.efficiency); strcat(buffer, temp);
    snprintf(temp, sizeof(temp), "<form action='/set/turn_sensitivity' method='get'><p>Turn Sensitivity: <input name='v' type='number' step='1' min='50' max='2000' value='%d'><button type='submit'>Set</button></p></form>", (int)genome.turn_sensitivity); strcat(buffer, temp);
    snprintf(temp, sizeof(temp), "<form action='/set/base_speed' method='get'><p>Base Speed: <input name='v' type='number' step='1' min='50' max='255' value='%d'><button type='submit'>Set</button></p></form>", genome.base_speed); strcat(buffer, temp);
    strcat(buffer, "</div>");
    strcat(buffer, "<div class='box'><h2>Power</h2>");
    if (battery.mode == POWER_USB_DEBUG) {
        snprintf(temp, sizeof(temp), "<p>Status: <span>DEBUGGING</span></p><p>Voltage: <span>%.1fV</span></p>", battery.voltage);
    } else {
        snprintf(temp, sizeof(temp), "<p>Percentage: <span>%.1f%%</span></p><p>Voltage: <span>%.1fV</span></p>", battery.percentage, battery.voltage);
    }
    strcat(buffer, temp);
    strcat(buffer, "</div>");
    strcat(buffer, "</div>");
    strcat(buffer, "<div class='box actions'><h2>Controls</h2>");
    strcat(buffer, "<a href='/reset'>Reset Life</a>");
    strcat(buffer, "<a href='/mutate'>Mutate Genome</a>");
    strcat(buffer, "<a href='/save'>Save Genome</a>");
    strcat(buffer, "<a href='/randomize' class='danger'>Randomize Genome</a>");
    strcat(buffer, "<a href='/api/stats' target='_blank'>JSON API</a>");
    strcat(buffer, "</div>");
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
        light_level, HAL::lightSensor.readLeft(), HAL::lightSensor.readRight(),
        HAL::ultrasonic.readDistance(), battery.voltage, battery.percentage,
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

void handleMutate() {
    genome.light_threshold += random(-100, 100) / 2000.0f; // Smaller mutation
    genome.efficiency += random(-100, 100) / 2000.0f;
    genome.light_threshold = constrain(genome.light_threshold, 0.01f, 1.0f);
    genome.efficiency = constrain(genome.efficiency, 0.5f, 2.0f);
    genome.generation++;
    saveGenome();
    Serial.println("[Web] Genome mutated via web.");
    server.sendHeader("Location", "/");
    server.send(303);
}

void handleRandomize() {
    genome.light_threshold = random(10, 500) / 1000.0f;
    genome.efficiency = 0.75f + (random(0, 100) / 100.0f);
    genome.turn_sensitivity = 200 + random(0, 600);
    genome.base_speed = 150 + random(0, 100);
    genome.generation = 0;
    saveGenome();
    handleReset(); // Also reset life state
    Serial.println("[Web] Genome randomized via web.");
}

void handleSave() {
    saveGenome();
    server.sendHeader("Location", "/");
    server.send(303);
}

void handleNotFound() {
    server.send(404, "text/plain", "Not Found");
}

void setupWebServer() {
    server.on("/", HTTP_GET, handleRoot);
    server.on("/mutate", HTTP_GET, handleMutate);
    server.on("/randomize", HTTP_GET, handleRandomize);
    server.on("/save", HTTP_GET, handleSave);
    server.on("/api/stats", HTTP_GET, handleAPIStats);
    server.on("/reset", HTTP_GET, handleReset);
    server.on("/set/threshold", HTTP_GET, []() {
        if (server.hasArg("v")) {
            float val = server.arg("v").toFloat();
            genome.light_threshold = constrain(val, 0.01f, 1.0f);
            saveGenome();
            server.send(200, "text/plain", "light_threshold set");
        } else {
            server.send(400, "text/plain", "Missing value");
        }
    });
    server.on("/set/efficiency", HTTP_GET, []() {
        if (server.hasArg("v")) {
            float val = server.arg("v").toFloat();
            genome.efficiency = constrain(val, 0.5f, 2.0f);
            saveGenome();
            server.send(200, "text/plain", "efficiency set");
        } else {
            server.send(400, "text/plain", "Missing value");
        }
    });
    server.on("/set/turn_sensitivity", HTTP_GET, []() {
        if (server.hasArg("v")) {
            int val = server.arg("v").toInt();
            genome.turn_sensitivity = constrain(val, 50, 2000);
            saveGenome();
            server.send(200, "text/plain", "turn_sensitivity set");
        } else {
            server.send(400, "text/plain", "Missing value");
        }
    });
    server.on("/set/base_speed", HTTP_GET, []() {
        if (server.hasArg("v")) {
            int val = server.arg("v").toInt();
            genome.base_speed = constrain(val, 50, 255);
            saveGenome();
            server.send(200, "text/plain", "base_speed set");
        } else {
            server.send(400, "text/plain", "Missing value");
        }
    });
    server.on("/set/decay", HTTP_GET, []() {
        if (server.hasArg("v")) {
            float val = server.arg("v").toFloat();
            life_params.energy_decay = constrain(val, 0.0f, 5.0f);
            // Note: This is a runtime change and is not saved to flash.
            server.send(200, "text/plain", "energy_decay set for this session");
        } else {
            server.send(400, "text/plain", "Missing value");
        }
    });
    server.onNotFound(handleNotFound);
    server.begin();
    Serial.println("[Web] Server started.");
}
