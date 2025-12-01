/*
 * web_server.cpp - EMBER Web Server Implementation
 */

#include <WebServer.h>
#include "hal.h"
#include "genome.h"
#include "globals.h"

extern WebServer server;
extern Genome genome;
extern float light_level;

// Forward declaration for the function defined in main.cpp
void saveGenome();

void handleRoot();
void handleAPIStats();
void handleReset();
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
    snprintf(temp, sizeof(temp), "<p>Distance: <span>%.1f cm</span></p>", HAL::ultrasonic.readDistance()); strcat(buffer, temp);
    strcat(buffer, "</div>");
    strcat(buffer, "<div class='box'><h2>Genome</h2>");
    snprintf(temp, sizeof(temp), "<p>Generation: <span>%u</span></p>", genome.generation); strcat(buffer, temp);
    snprintf(temp, sizeof(temp), "<p>Light Threshold: <span>%.3f</span></p>", genome.light_threshold); strcat(buffer, temp);
    snprintf(temp, sizeof(temp), "<p>Efficiency: <span>%.3f</span></p>", genome.efficiency); strcat(buffer, temp);
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
        genome.bot_id, genome.generation, is_alive ? "true" : "false", energy, //
        light_level, HAL::lightSensor.readLeft(), HAL::lightSensor.readRight(), //
        HAL::ultrasonic.readDistance(), battery.voltage, battery.percentage,   //
        battery.mode, alive_time_ms / 1000);                                    //
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
