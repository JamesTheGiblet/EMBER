/*
 * EMBER v0.1 - LIFE FROM LIGHT
 * Hardware Abstraction Layer (HAL) + OTA + Web Server + Preferences Edition
 * 
 * The absolute minimum: Can it stay alive?
 * Now with: Clean hardware abstraction + OTA + Web interface + Persistent storage
 * 
 * Hardware: ESP32 + Light Sensor (LDR) + RGB LED + Motors (disabled v0.1)
 * 
 * Rule: Light = Energy = Life
 * Gene: How much light does it need to survive?
 * 
 * This is where it ALL begins.
 */

#include <Arduino.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <WebServer.h>
#include <Preferences.h>

// ============================================================================
// CONFIGURATION
// ============================================================================

// WiFi credentials - CHANGE THESE
const char* WIFI_SSID = "YourNetworkName";
const char* WIFI_PASSWORD = "YourPassword";

// OTA configuration
const char* OTA_HOSTNAME = "ember-bot-0";  // Change for each bot
const char* OTA_PASSWORD = "ember2024";     // OTA update password

// Web server
const int WEB_SERVER_PORT = 80;

// WiFi reconnection
const unsigned long WIFI_RECONNECT_INTERVAL = 30000;  // 30 seconds
unsigned long lastWiFiCheck = 0;

// ============================================================================
// HARDWARE ABSTRACTION LAYER (HAL)
// ============================================================================

namespace HAL {
    // Pin definitions - centralized hardware config
    namespace Pins {
        // Light sensors
        constexpr int LDR_LEFT = 34;
        constexpr int LDR_RIGHT = 35;
        
        // RGB LED
        constexpr int LED_RED = 23;
        constexpr int LED_GREEN = 22;
        constexpr int LED_BLUE = 21;
        
        // Ultrasonic sensor (future use)
        constexpr int US_TRIGGER = 25;
        constexpr int US_ECHO = 26;
        
        // Motor driver pins
        constexpr int MOTOR_A_IN1 = 15;
        constexpr int MOTOR_A_IN2 = 2;
        constexpr int MOTOR_B_IN1 = 16;
        constexpr int MOTOR_B_IN2 = 17;
        
        // Optional: motor enable pins (depends on driver type)
        constexpr int MOTOR_A_EN = 5;
        constexpr int MOTOR_B_EN = 4;
        constexpr int MOTOR_STBY = 13;  // For TB6612FNG
    }
    
    // PWM configuration for LED
    namespace PWM {
        constexpr int FREQ = 5000;      // 5kHz
        constexpr int RESOLUTION = 8;   // 8-bit (0-255)
        
        // Channel assignments
        constexpr int CH_RED = 0;
        constexpr int CH_GREEN = 1;
        constexpr int CH_BLUE = 2;
        constexpr int CH_MOTOR_A = 3;
        constexpr int CH_MOTOR_B = 4;
    }
    
    // ADC configuration
    namespace ADC {
        constexpr int RESOLUTION = 12;   // 12-bit (0-4095)
        constexpr int MAX_VALUE = 4095;
    }
    
    // ========================================================================
    // SENSOR INTERFACE
    // ========================================================================
    
    class LightSensor {
    private:
        int _pinLeft;
        int _pinRight;
        
    public:
        LightSensor(int pinLeft, int pinRight) 
            : _pinLeft(pinLeft), _pinRight(pinRight) {}
        
        void begin() {
            pinMode(_pinLeft, INPUT);
            pinMode(_pinRight, INPUT);
            analogReadResolution(ADC::RESOLUTION);
        }
        
        float readLeft() {
            int raw = analogRead(_pinLeft);
            return raw / (float)ADC::MAX_VALUE;  // Normalize to 0.0-1.0
        }
        
        float readRight() {
            int raw = analogRead(_pinRight);
            return raw / (float)ADC::MAX_VALUE;
        }
        
        float readAverage() {
            return (readLeft() + readRight()) / 2.0f;
        }
        
        float readDifference() {
            return readLeft() - readRight();  // Positive = light on left
        }
        
        // Raw ADC values for debugging
        int readRawLeft() { return analogRead(_pinLeft); }
        int readRawRight() { return analogRead(_pinRight); }
    };
    
    // ========================================================================
    // LED INTERFACE
    // ========================================================================
    
    class RGBLED {
    private:
        int _pinRed, _pinGreen, _pinBlue;
        int _chRed, _chGreen, _chBlue;
        
    public:
        RGBLED(int pinRed, int pinGreen, int pinBlue)
            : _pinRed(pinRed), _pinGreen(pinGreen), _pinBlue(pinBlue) {
            _chRed = PWM::CH_RED;
            _chGreen = PWM::CH_GREEN;
            _chBlue = PWM::CH_BLUE;
        }
        
        void begin() {
            // Setup PWM channels
            ledcSetup(_chRed, PWM::FREQ, PWM::RESOLUTION);
            ledcSetup(_chGreen, PWM::FREQ, PWM::RESOLUTION);
            ledcSetup(_chBlue, PWM::FREQ, PWM::RESOLUTION);
            
            // Attach pins to channels
            ledcAttachPin(_pinRed, _chRed);
            ledcAttachPin(_pinGreen, _chGreen);
            ledcAttachPin(_pinBlue, _chBlue);
            
            // Initialize to off
            setRGB(0, 0, 0);
        }
        
        void setRGB(uint8_t r, uint8_t g, uint8_t b) {
            ledcWrite(_chRed, r);
            ledcWrite(_chGreen, g);
            ledcWrite(_chBlue, b);
        }
        
        void setColor(uint32_t color) {
            uint8_t r = (color >> 16) & 0xFF;
            uint8_t g = (color >> 8) & 0xFF;
            uint8_t b = color & 0xFF;
            setRGB(r, g, b);
        }
        
        // Predefined colors
        void red(uint8_t brightness = 255) { setRGB(brightness, 0, 0); }
        void green(uint8_t brightness = 255) { setRGB(0, brightness, 0); }
        void blue(uint8_t brightness = 255) { setRGB(0, 0, brightness); }
        void white(uint8_t brightness = 255) { setRGB(brightness, brightness, brightness); }
        void yellow(uint8_t brightness = 255) { setRGB(brightness, brightness, 0); }
        void cyan(uint8_t brightness = 255) { setRGB(0, brightness, brightness); }
        void magenta(uint8_t brightness = 255) { setRGB(brightness, 0, brightness); }
        void off() { setRGB(0, 0, 0); }
    };
    
    // ========================================================================
    // MOTOR INTERFACE
    // ========================================================================
    
    class MotorDriver {
    private:
        int _pinA1, _pinA2, _pinB1, _pinB2;
        int _pinAEn, _pinBEn, _pinStby;
        int _chA, _chB;
        bool _hasEnablePins;
        bool _hasStandby;
        
    public:
        MotorDriver(int pinA1, int pinA2, int pinB1, int pinB2,
                   int pinAEn = -1, int pinBEn = -1, int pinStby = -1)
            : _pinA1(pinA1), _pinA2(pinA2), _pinB1(pinB1), _pinB2(pinB2),
              _pinAEn(pinAEn), _pinBEn(pinBEn), _pinStby(pinStby) {
            _chA = PWM::CH_MOTOR_A;
            _chB = PWM::CH_MOTOR_B;
            _hasEnablePins = (pinAEn >= 0 && pinBEn >= 0);
            _hasStandby = (pinStby >= 0);
        }
        
        void begin() {
            pinMode(_pinA1, OUTPUT);
            pinMode(_pinA2, OUTPUT);
            pinMode(_pinB1, OUTPUT);
            pinMode(_pinB2, OUTPUT);
            
            if (_hasEnablePins) {
                ledcSetup(_chA, PWM::FREQ, PWM::RESOLUTION);
                ledcSetup(_chB, PWM::FREQ, PWM::RESOLUTION);
                ledcAttachPin(_pinAEn, _chA);
                ledcAttachPin(_pinBEn, _chB);
            }
            
            if (_hasStandby) {
                pinMode(_pinStby, OUTPUT);
                digitalWrite(_pinStby, HIGH);
            }
            
            stop();
        }
        
        void setMotors(int speedA, int speedB) {
            if (_hasEnablePins) {
                // TB6612FNG style
                if (speedA > 0) {
                    digitalWrite(_pinA1, HIGH);
                    digitalWrite(_pinA2, LOW);
                    ledcWrite(_chA, speedA);
                } else if (speedA < 0) {
                    digitalWrite(_pinA1, LOW);
                    digitalWrite(_pinA2, HIGH);
                    ledcWrite(_chA, -speedA);
                } else {
                    digitalWrite(_pinA1, LOW);
                    digitalWrite(_pinA2, LOW);
                    ledcWrite(_chA, 0);
                }
                
                if (speedB > 0) {
                    digitalWrite(_pinB1, HIGH);
                    digitalWrite(_pinB2, LOW);
                    ledcWrite(_chB, speedB);
                } else if (speedB < 0) {
                    digitalWrite(_pinB1, LOW);
                    digitalWrite(_pinB2, HIGH);
                    ledcWrite(_chB, -speedB);
                } else {
                    digitalWrite(_pinB1, LOW);
                    digitalWrite(_pinB2, LOW);
                    ledcWrite(_chB, 0);
                }
            }
        }
        
        void stop() { setMotors(0, 0); }
        void disable() {
            stop();
            if (_hasStandby) digitalWrite(_pinStby, LOW);
        }
    };
    
    // ========================================================================
    // ULTRASONIC SENSOR INTERFACE
    // ========================================================================
    
    class UltrasonicSensor {
    private:
        int _pinTrigger;
        int _pinEcho;
        
    public:
        UltrasonicSensor(int pinTrigger, int pinEcho)
            : _pinTrigger(pinTrigger), _pinEcho(pinEcho) {}
        
        void begin() {
            pinMode(_pinTrigger, OUTPUT);
            pinMode(_pinEcho, INPUT);
            digitalWrite(_pinTrigger, LOW);
        }
        
        float readDistance() {
            digitalWrite(_pinTrigger, HIGH);
            delayMicroseconds(10);
            digitalWrite(_pinTrigger, LOW);
            
            long duration = pulseIn(_pinEcho, HIGH, 30000);
            if (duration == 0) return -1.0f;
            
            return duration * 0.0343f / 2.0f;
        }
    };
}

// ============================================================================
// HARDWARE INSTANCES
// ============================================================================

HAL::LightSensor lightSensor(HAL::Pins::LDR_LEFT, HAL::Pins::LDR_RIGHT);
HAL::RGBLED led(HAL::Pins::LED_RED, HAL::Pins::LED_GREEN, HAL::Pins::LED_BLUE);
HAL::MotorDriver motors(HAL::Pins::MOTOR_A_IN1, HAL::Pins::MOTOR_A_IN2,
                        HAL::Pins::MOTOR_B_IN1, HAL::Pins::MOTOR_B_IN2,
                        HAL::Pins::MOTOR_A_EN, HAL::Pins::MOTOR_B_EN,
                        HAL::Pins::MOTOR_STBY);
HAL::UltrasonicSensor ultrasonic(HAL::Pins::US_TRIGGER, HAL::Pins::US_ECHO);

// ============================================================================
// PERSISTENT STORAGE
// ============================================================================

Preferences preferences;

void saveGenome();
void loadGenome();

// ============================================================================
// GENETIC CODE - This is what evolves
// ============================================================================

struct Genome {
    float light_threshold;
    float efficiency;
    uint8_t bot_id;
    uint32_t generation;
};

Genome genome = {
    .light_threshold = 0.5,
    .efficiency = 1.0,
    .bot_id = 0,
    .generation = 0
};

// ============================================================================
// LIFE STATE
// ============================================================================

float energy = 100.0;
float light_level = 0.0;
unsigned long alive_time = 0;
unsigned long boot_time = 0;
bool is_alive = true;

const float ENERGY_DECAY = 0.1;
const float ENERGY_GAIN = 2.0;

// ============================================================================
// WEB SERVER
// ============================================================================

WebServer server(WEB_SERVER_PORT);

// HTML/CSS for web interface
const char* HTML_HEADER = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>EMBER Bot %d</title>
    <style>
        * { margin: 0; padding: 0; box-sizing: border-box; }
        body {
            font-family: 'Courier New', monospace;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            color: #fff;
            padding: 20px;
            min-height: 100vh;
        }
        .container {
            max-width: 800px;
            margin: 0 auto;
            background: rgba(0, 0, 0, 0.3);
            border-radius: 20px;
            padding: 30px;
            backdrop-filter: blur(10px);
            box-shadow: 0 8px 32px rgba(0, 0, 0, 0.3);
        }
        h1 {
            text-align: center;
            margin-bottom: 10px;
            font-size: 2.5em;
            text-shadow: 2px 2px 4px rgba(0, 0, 0, 0.5);
        }
        .subtitle {
            text-align: center;
            margin-bottom: 30px;
            opacity: 0.8;
            font-size: 1.1em;
        }
        .status-box {
            background: rgba(255, 255, 255, 0.1);
            border-radius: 15px;
            padding: 20px;
            margin: 20px 0;
            border: 2px solid rgba(255, 255, 255, 0.2);
        }
        .status-header {
            font-size: 1.3em;
            margin-bottom: 15px;
            border-bottom: 2px solid rgba(255, 255, 255, 0.3);
            padding-bottom: 10px;
        }
        .stat-row {
            display: flex;
            justify-content: space-between;
            padding: 10px 0;
            border-bottom: 1px solid rgba(255, 255, 255, 0.1);
        }
        .stat-row:last-child { border-bottom: none; }
        .stat-label {
            opacity: 0.8;
            font-weight: bold;
        }
        .stat-value {
            font-family: 'Courier New', monospace;
            font-size: 1.1em;
        }
        .energy-bar {
            width: 100%;
            height: 30px;
            background: rgba(0, 0, 0, 0.3);
            border-radius: 15px;
            overflow: hidden;
            margin: 10px 0;
            border: 2px solid rgba(255, 255, 255, 0.2);
        }
        .energy-fill {
            height: 100%;
            transition: width 0.3s ease;
            display: flex;
            align-items: center;
            justify-content: center;
            font-weight: bold;
            text-shadow: 1px 1px 2px rgba(0, 0, 0, 0.8);
        }
        .status-alive { background: linear-gradient(90deg, #00ff00, #00cc00); }
        .status-dead { background: linear-gradient(90deg, #ff0000, #cc0000); }
        .led-indicator {
            width: 60px;
            height: 60px;
            border-radius: 50%;
            margin: 20px auto;
            box-shadow: 0 0 20px currentColor;
            animation: pulse 2s ease-in-out infinite;
        }
        @keyframes pulse {
            0%, 100% { opacity: 1; }
            50% { opacity: 0.6; }
        }
        .button-grid {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(150px, 1fr));
            gap: 15px;
            margin: 20px 0;
        }
        .btn {
            background: rgba(255, 255, 255, 0.2);
            border: 2px solid rgba(255, 255, 255, 0.3);
            color: white;
            padding: 15px;
            border-radius: 10px;
            cursor: pointer;
            font-size: 1em;
            font-weight: bold;
            transition: all 0.3s ease;
            text-decoration: none;
            display: block;
            text-align: center;
        }
        .btn:hover {
            background: rgba(255, 255, 255, 0.3);
            transform: translateY(-2px);
            box-shadow: 0 4px 12px rgba(0, 0, 0, 0.3);
        }
        .btn-danger { border-color: #ff4444; }
        .btn-success { border-color: #44ff44; }
        .btn-info { border-color: #4444ff; }
        .footer {
            text-align: center;
            margin-top: 30px;
            opacity: 0.6;
            font-size: 0.9em;
        }
        .refresh-note {
            text-align: center;
            margin: 20px 0;
            opacity: 0.7;
            font-style: italic;
        }
    </style>
    <meta http-equiv="refresh" content="2">
</head>
<body>
)rawliteral";

const char* HTML_FOOTER = R"rawliteral(
    <div class="footer">
        <p>EMBER v0.1 - Life From Light</p>
        <p>Part of the Forge Theory Project</p>
    </div>
</body>
</html>
)rawliteral";

void handleRoot() {
    String html = String(HTML_HEADER);
    html.replace("%d", String(genome.bot_id));
    
    html += "<div class='container'>";
    html += "<h1>🔥 EMBER Bot " + String(genome.bot_id) + "</h1>";
    html += "<div class='subtitle'>Artificial Life - Generation " + String(genome.generation) + "</div>";
    
    // LED Indicator
    String ledColor = is_alive ? (light_level > genome.light_threshold ? "#00ff00" : "#ff0000") : "#000000";
    html += "<div class='led-indicator' style='background: " + ledColor + ";'></div>";
    
    // Life Status
    html += "<div class='status-box'>";
    html += "<div class='status-header'>💚 LIFE STATUS</div>";
    html += "<div class='stat-row'><span class='stat-label'>Status:</span><span class='stat-value'>";
    html += is_alive ? "🟢 ALIVE" : "⚫ DEAD";
    html += "</span></div>";
    html += "<div class='stat-row'><span class='stat-label'>Energy:</span><span class='stat-value'>" + String(energy, 1) + "%</span></div>";
    html += "<div class='energy-bar'><div class='energy-fill " + String(is_alive ? "status-alive" : "status-dead") + "' style='width: " + String(energy) + "%'>" + String(energy, 0) + "%</div></div>";
    html += "<div class='stat-row'><span class='stat-label'>Alive Time:</span><span class='stat-value'>" + String(alive_time / 1000) + "s</span></div>";
    html += "<div class='stat-row'><span class='stat-label'>Uptime:</span><span class='stat-value'>" + String((millis() - boot_time) / 1000) + "s</span></div>";
    html += "</div>";
    
    // Environment
    html += "<div class='status-box'>";
    html += "<div class='status-header'>🌞 ENVIRONMENT</div>";
    html += "<div class='stat-row'><span class='stat-label'>Light Level:</span><span class='stat-value'>" + String(light_level, 3) + "</span></div>";
    html += "<div class='stat-row'><span class='stat-label'>Light Left:</span><span class='stat-value'>" + String(lightSensor.readLeft(), 3) + "</span></div>";
    html += "<div class='stat-row'><span class='stat-label'>Light Right:</span><span class='stat-value'>" + String(lightSensor.readRight(), 3) + "</span></div>";
    html += "<div class='stat-row'><span class='stat-label'>Light Diff:</span><span class='stat-value'>" + String(lightSensor.readDifference(), 3) + "</span></div>";
    html += "</div>";
    
    // Genome
    html += "<div class='status-box'>";
    html += "<div class='status-header'>🧬 GENOME</div>";
    html += "<div class='stat-row'><span class='stat-label'>Bot ID:</span><span class='stat-value'>" + String(genome.bot_id) + "</span></div>";
    html += "<div class='stat-row'><span class='stat-label'>Generation:</span><span class='stat-value'>" + String(genome.generation) + "</span></div>";
    html += "<div class='stat-row'><span class='stat-label'>Light Threshold:</span><span class='stat-value'>" + String(genome.light_threshold, 3) + "</span></div>";
    html += "<div class='stat-row'><span class='stat-label'>Efficiency:</span><span class='stat-value'>" + String(genome.efficiency, 3) + "</span></div>";
    html += "</div>";
    
    // Network Info
    html += "<div class='status-box'>";
    html += "<div class='status-header'>📡 NETWORK</div>";
    html += "<div class='stat-row'><span class='stat-label'>Hostname:</span><span class='stat-value'>" + String(OTA_HOSTNAME) + ".local</span></div>";
    html += "<div class='stat-row'><span class='stat-label'>IP Address:</span><span class='stat-value'>" + WiFi.localIP().toString() + "</span></div>";
    html += "<div class='stat-row'><span class='stat-label'>WiFi RSSI:</span><span class='stat-value'>" + String(WiFi.RSSI()) + " dBm</span></div>";
    html += "<div class='stat-row'><span class='stat-label'>Free Heap:</span><span class='stat-value'>" + String(ESP.getFreeHeap() / 1024) + " KB</span></div>";
    html += "</div>";
    
    // Control Buttons
    html += "<div class='status-box'>";
    html += "<div class='status-header'>🎮 CONTROLS</div>";
    html += "<div class='button-grid'>";
    html += "<a href='/mutate' class='btn btn-info'>🧬 Mutate</a>";
    html += "<a href='/reset' class='btn btn-success'>🔄 Reset Life</a>";
    html += "<a href='/randomize' class='btn btn-danger'>🎲 Randomize</a>";
    html += "<a href='/save' class='btn btn-success'>💾 Save Genome</a>";
    html += "<a href='/api/stats' class='btn btn-info'>📊 JSON API</a>";
    html += "</div>";
    html += "</div>";
    
    html += "<div class='refresh-note'>↻ Auto-refreshing every 2 seconds</div>";
    html += "</div>";
    html += HTML_FOOTER;
    
    server.send(200, "text/html", html);
}

void handleMutate() {
    genome.light_threshold += (random(-100, 100) / 1000.0) * 0.1;
    genome.efficiency += (random(-100, 100) / 1000.0) * 0.1;
    genome.light_threshold = constrain(genome.light_threshold, 0.0, 1.0);
    genome.efficiency = constrain(genome.efficiency, 0.5, 1.5);
    genome.generation++;
    
    saveGenome();
    
    server.sendHeader("Location", "/");
    server.send(303);
}

void handleReset() {
    energy = 100.0;
    is_alive = true;
    alive_time = 0;
    
    server.sendHeader("Location", "/");
    server.send(303);
}

void handleRandomize() {
    genome.light_threshold = random(0, 100) / 100.0;
    genome.efficiency = 0.5 + (random(0, 100) / 100.0);
    genome.generation = 0;
    energy = 100.0;
    is_alive = true;
    alive_time = 0;
    
    saveGenome();
    
    server.sendHeader("Location", "/");
    server.send(303);
}

void handleSave() {
    saveGenome();
    server.sendHeader("Location", "/");
    server.send(303);
}

void handleAPIStats() {
    String json = "{";
    json += "\"bot_id\":" + String(genome.bot_id) + ",";
    json += "\"generation\":" + String(genome.generation) + ",";
    json += "\"alive\":" + String(is_alive ? "true" : "false") + ",";
    json += "\"energy\":" + String(energy, 2) + ",";
    json += "\"light_level\":" + String(light_level, 3) + ",";
    json += "\"light_left\":" + String(lightSensor.readLeft(), 3) + ",";
    json += "\"light_right\":" + String(lightSensor.readRight(), 3) + ",";
    json += "\"threshold\":" + String(genome.light_threshold, 3) + ",";
    json += "\"efficiency\":" + String(genome.efficiency, 3) + ",";
    json += "\"alive_time\":" + String(alive_time / 1000) + ",";
    json += "\"uptime\":" + String((millis() - boot_time) / 1000) + ",";
    json += "\"wifi_rssi\":" + String(WiFi.RSSI()) + ",";
    json += "\"free_heap\":" + String(ESP.getFreeHeap());
    json += "}";
    
    server.send(200, "application/json", json);
}

void handleNotFound() {
    server.send(404, "text/plain", "404: Not Found");
}

void setupWebServer() {
    server.on("/", handleRoot);
    server.on("/mutate", handleMutate);
    server.on("/reset", handleReset);
    server.on("/randomize", handleRandomize);
    server.on("/save", handleSave);
    server.on("/api/stats", handleAPIStats);
    server.onNotFound(handleNotFound);
    
    server.begin();
    Serial.println("[Web] Server started on port " + String(WEB_SERVER_PORT));
    Serial.println("[Web] Access at: http://" + String(OTA_HOSTNAME) + ".local/");
}

// ============================================================================
// PERSISTENT STORAGE FUNCTIONS
// ============================================================================

void saveGenome() {
    preferences.begin("ember", false);  // false = read/write
    preferences.putFloat("threshold", genome.light_threshold);
    preferences.putFloat("efficiency", genome.efficiency);
    preferences.putUChar("bot_id", genome.bot_id);
    preferences.putUInt("generation", genome.generation);
    preferences.end();
    
    Serial.println("[Storage] Genome saved to flash");
}

void loadGenome() {
    preferences.begin("ember", true);  // true = read-only
    
    // Check if we have saved data
    if (preferences.isKey("threshold")) {
        genome.light_threshold = preferences.getFloat("threshold", 0.5);
        genome.efficiency = preferences.getFloat("efficiency", 1.0);
        genome.bot_id = preferences.getUChar("bot_id", 0);
        genome.generation = preferences.getUInt("generation", 0);
        
        Serial.println("[Storage] Genome loaded from flash");
        Serial.printf("[Storage] Threshold: %.3f, Efficiency: %.3f, Gen: %d\n",
                     genome.light_threshold, genome.efficiency, genome.generation);
    } else {
        Serial.println("[Storage] No saved genome found - using defaults");
    }
    
    preferences.end();
}

void clearStorage() {
    preferences.begin("ember", false);
    preferences.clear();
    preferences.end();
    Serial.println("[Storage] All preferences cleared");
}

// ============================================================================
// WIFI & OTA FUNCTIONS
// ============================================================================

void setupWiFi() {
    Serial.println("\n[WiFi] Connecting to " + String(WIFI_SSID) + "...");
    WiFi.mode(WIFI_STA);
    WiFi.setHostname(OTA_HOSTNAME);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
        led.blue(50);
        delay(250);
        led.off();
        delay(250);
        Serial.print(".");
        attempts++;
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\n[WiFi] Connected!");
        Serial.print("[WiFi] IP: ");
        Serial.println(WiFi.localIP());
        Serial.print("[WiFi] Hostname: ");
        Serial.println(OTA_HOSTNAME);
        
        // Success flash - blue
        for (int i = 0; i < 3; i++) {
            led.blue(255);
            delay(100);
            led.off();
            delay(100);
        }
    } else {
        Serial.println("\n[WiFi] Failed to connect - continuing without network features");
        for (int i = 0; i < 3; i++) {
            led.yellow(255);
            delay(100);
            led.off();
            delay(100);
        }
    }
}

void checkWiFiConnection() {
    // Only check periodically
    if (millis() - lastWiFiCheck < WIFI_RECONNECT_INTERVAL) {
        return;
    }
    lastWiFiCheck = millis();
    
    // Check if disconnected
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("[WiFi] Connection lost - attempting reconnect...");
        
        // Brief visual indication
        led.yellow(50);
        delay(100);
        led.off();
        
        // Try to reconnect
        WiFi.disconnect();
        WiFi.reconnect();
        
        // Wait up to 10 seconds
        int attempts = 0;
        while (WiFi.status() != WL_CONNECTED && attempts < 40) {
            delay(250);
            attempts++;
        }
        
        if (WiFi.status() == WL_CONNECTED) {
            Serial.println("[WiFi] Reconnected successfully");
            Serial.print("[WiFi] IP: ");
            Serial.println(WiFi.localIP());
        } else {
            Serial.println("[WiFi] Reconnection failed - will retry later");
        }
    }
}

void setupOTA() {
    ArduinoOTA.setHostname(OTA_HOSTNAME);
    ArduinoOTA.setPassword(OTA_PASSWORD);
    
    ArduinoOTA.onStart([]() {
        String type = (ArduinoOTA.getCommand() == U_FLASH) ? "sketch" : "filesystem";
        Serial.println("\n[OTA] Start updating " + type);
        motors.disable();
        led.magenta(128);
    });
    
    ArduinoOTA.onEnd([]() {
        Serial.println("\n[OTA] Update complete!");
        for (int i = 0; i < 5; i++) {
            led.green(255);
            delay(100);
            led.off();
            delay(100);
        }
    });
    
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        static unsigned long lastPrint = 0;
        if (millis() - lastPrint > 1000) {
            Serial.printf("[OTA] Progress: %u%%\n", (progress / (total / 100)));
            lastPrint = millis();
            
            int brightness = (millis() / 10) % 256;
            if (brightness > 128) brightness = 255 - brightness;
            led.setRGB(brightness, 0, brightness);
        }
    });
    
    ArduinoOTA.onError([](ota_error_t error) {
        Serial.printf("[OTA] Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
        else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
        else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
        else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
        else if (error == OTA_END_ERROR) Serial.println("End Failed");
        
        for (int i = 0; i < 10; i++) {
            led.red(255);
            delay(50);
            led.off();
            delay(50);
        }
    });
    
    ArduinoOTA.begin();
    Serial.println("[OTA] Ready");
}

void setupMDNS() {
    if (MDNS.begin(OTA_HOSTNAME)) {
        Serial.println("[mDNS] Responder started");
        Serial.printf("[mDNS] Access at: http://%s.local/\n", OTA_HOSTNAME);
        
        // Advertise services
        MDNS.addService("http", "tcp", 80);
        MDNS.addService("arduino", "tcp", 3232);  // OTA service
    } else {
        Serial.println("[mDNS] Failed to start");
    }
}

// ============================================================================
// CORE LIFE FUNCTIONS
// ============================================================================

void updateEnergy(float deltaTime) {
    energy -= ENERGY_DECAY * deltaTime;
    
    if (light_level > genome.light_threshold) {
        float excess_light = light_level - genome.light_threshold;
        energy += ENERGY_GAIN * excess_light * genome.efficiency * deltaTime;
    }
    
    if (energy > 100.0) energy = 100.0;
    if (energy < 0.0) {
        energy = 0.0;
        is_alive = false;
    }
}

void showState() {
    if (!is_alive) {
        led.off();
        return;
    }
    
    if (light_level > genome.light_threshold) {
        int brightness = (int)(energy * 2.55);
        led.green(brightness);
    } else {
        int brightness = (int)(energy * 2.55);
        int flash_period = 100 + (int)(energy * 9);
        int flash = (millis() / flash_period) % 2;
        if (flash) {
            led.red(brightness);
        } else {
            led.off();
        }
    }
}

void printStats() {
    Serial.printf("Light: %.3f | Energy: %.1f | Alive: %ds | Status: %s | IP: %s\n",
                  light_level,
                  energy,
                  alive_time / 1000,
                  is_alive ? "ALIVE" : "DEAD",
                  WiFi.localIP().toString().c_str());
}

// ============================================================================
// SETUP
// ============================================================================

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    boot_time = millis();
    
    Serial.println("\n\n");
    Serial.println("╔═══════════════════════════════════════╗");
    Serial.println("║     EMBER v0.1 - LIFE FROM LIGHT      ║");
    Serial.println("║   HAL + OTA + Web + Storage Edition   ║");
    Serial.println("║                                       ║");
    Serial.println("║  Simple rule: Light = Energy = Life  ║");
    Serial.println("║                                       ║");
    Serial.println("║  Watch what emerges...                ║");
    Serial.println("╚═══════════════════════════════════════╝");
    Serial.println();
    
    // Initialize hardware
    Serial.println("[HAL] Initializing hardware...");
    lightSensor.begin();
    led.begin();
    motors.begin();
    ultrasonic.begin();
    motors.disable();
    Serial.println("[HAL] Motors disabled (v0.1 - stationary)");
    
    // Load saved genome from flash
    loadGenome();
    
    // Setup WiFi and network services
    setupWiFi();
    if (WiFi.status() == WL_CONNECTED) {
        setupMDNS();
        setupOTA();
        setupWebServer();
    }
    
    // If no saved genome, create random one
    if (genome.generation == 0 && genome.light_threshold == 0.5) {
        randomSeed(analogRead(35) + genome.bot_id * 1000);
        genome.light_threshold = random(0, 100) / 100.0;
        genome.efficiency = 0.5 + (random(0, 100) / 100.0);
        saveGenome();
    }
    
    // Print genome
    Serial.println("\n=================================");
    Serial.printf("Bot ID: %d\n", genome.bot_id);
    Serial.printf("Generation: %d\n", genome.generation);
    Serial.printf("Light Threshold: %.3f\n", genome.light_threshold);
    Serial.printf("Efficiency: %.3f\n", genome.efficiency);
    Serial.println("=================================\n");
    
    // Boot complete flash
    for (int i = 0; i < 3; i++) {
        led.white(255);
        delay(100);
        led.off();
        delay(100);
    }
    
    Serial.println("[EMBER] Ready!");
    Serial.println("[EMBER] Serial commands: genome, stats, sensors, save, clear, help");
    if (WiFi.status() == WL_CONNECTED) {
        Serial.printf("[EMBER] Web interface: http://%s.local/\n", OTA_HOSTNAME);
        Serial.printf("[EMBER] JSON API: http://%s.local/api/stats\n", OTA_HOSTNAME);
    }
    Serial.println();
}

// ============================================================================
// MAIN LOOP
// ============================================================================

unsigned long lastUpdate = 0;
unsigned long lastPrint = 0;

void loop() {
    // Handle network services
    if (WiFi.status() == WL_CONNECTED) {
        ArduinoOTA.handle();
        server.handleClient();
    }
    
    // Check WiFi connection periodically
    checkWiFiConnection();
    
    unsigned long now = millis();
    float deltaTime = (now - lastUpdate) / 1000.0;
    lastUpdate = now;
    
    // Read environment
    light_level = lightSensor.readAverage();
    
    // Update life state
    if (is_alive) {
        updateEnergy(deltaTime);
        alive_time = now;
    }
    
    // Show current state
    showState();
    
    // Print stats every second
    if (now - lastPrint > 1000) {
        printStats();
        lastPrint = now;
    }
    
    delay(50);
}

// ============================================================================
// SERIAL COMMANDS
// ============================================================================

void serialEvent() {
    if (Serial.available()) {
        String cmd = Serial.readStringUntil('\n');
        cmd.trim();
        
        if (cmd == "genome") {
            Serial.println("\n=================================");
            Serial.printf("Bot ID: %d\n", genome.bot_id);
            Serial.printf("Generation: %d\n", genome.generation);
            Serial.printf("Light Threshold: %.3f\n", genome.light_threshold);
            Serial.printf("Efficiency: %.3f\n", genome.efficiency);
            Serial.println("=================================\n");
        }
        else if (cmd == "stats") {
            printStats();
        }
        else if (cmd == "mutate") {
            genome.light_threshold += (random(-100, 100) / 1000.0) * 0.1;
            genome.efficiency += (random(-100, 100) / 1000.0) * 0.1;
            genome.light_threshold = constrain(genome.light_threshold, 0.0, 1.0);
            genome.efficiency = constrain(genome.efficiency, 0.5, 1.5);
            genome.generation++;
            saveGenome();
            Serial.println("Genome mutated and saved!");
        }
        else if (cmd == "reset") {
            energy = 100.0;
            is_alive = true;
            alive_time = 0;
            Serial.println("Life reset!");
        }
        else if (cmd == "randomize") {
            genome.light_threshold = random(0, 100) / 100.0;
            genome.efficiency = 0.5 + (random(0, 100) / 100.0);
            genome.generation = 0;
            energy = 100.0;
            is_alive = true;
            alive_time = 0;
            saveGenome();
            Serial.println("New random genome created and saved!");
        }
        else if (cmd.startsWith("threshold ")) {
            float val = cmd.substring(10).toFloat();
            genome.light_threshold = constrain(val, 0.0, 1.0);
            Serial.printf("Threshold set to: %.3f\n", genome.light_threshold);
        }
        else if (cmd.startsWith("efficiency ")) {
            float val = cmd.substring(11).toFloat();
            genome.efficiency = constrain(val, 0.5, 1.5);
            Serial.printf("Efficiency set to: %.3f\n", genome.efficiency);
        }
        else if (cmd.startsWith("id ")) {
            int val = cmd.substring(3).toInt();
            genome.bot_id = constrain(val, 0, 8);
            Serial.printf("Bot ID set to: %d\n", genome.bot_id);
        }
        else if (cmd == "save") {
            saveGenome();
            Serial.println("Genome saved to flash!");
        }
        else if (cmd == "clear") {
            clearStorage();
            Serial.println("Storage cleared! Restart to use defaults.");
        }
        else if (cmd == "sensors") {
            Serial.println("\n--- Sensor Readings ---");
            Serial.printf("Light Left:  %.3f (raw: %d)\n", 
                         lightSensor.readLeft(), lightSensor.readRawLeft());
            Serial.printf("Light Right: %.3f (raw: %d)\n", 
                         lightSensor.readRight(), lightSensor.readRawRight());
            Serial.printf("Light Avg:   %.3f\n", lightSensor.readAverage());
            Serial.printf("Light Diff:  %.3f\n", lightSensor.readDifference());
            Serial.printf("Distance:    %.1f cm\n", ultrasonic.readDistance());
            Serial.println("----------------------\n");
        }
        else if (cmd == "wifi") {
            Serial.println("\n--- WiFi Status ---");
            Serial.printf("Connected: %s\n", WiFi.status() == WL_CONNECTED ? "Yes" : "No");
            if (WiFi.status() == WL_CONNECTED) {
                Serial.printf("SSID: %s\n", WiFi.SSID().c_str());
                Serial.printf("IP: %s\n", WiFi.localIP().toString().c_str());
                Serial.printf("RSSI: %d dBm\n", WiFi.RSSI());
                Serial.printf("Hostname: %s.local\n", OTA_HOSTNAME);
                Serial.printf("Web: http://%s.local/\n", OTA_HOSTNAME);
            }
            Serial.println("-------------------\n");
        }
        else if (cmd == "led") {
            Serial.println("LED test - cycling colors...");
            led.red(255); delay(500);
            led.green(255); delay(500);
            led.blue(255); delay(500);
            led.white(255); delay(500);
            led.off();
            Serial.println("LED test complete");
        }
        else if (cmd == "help") {
            Serial.println("\nSerial Commands:");
            Serial.println("  genome       - Show current genetic code");
            Serial.println("  stats        - Show energy, light, fitness");
            Serial.println("  sensors      - Show all sensor readings");
            Serial.println("  mutate       - Mutate genome and save");
            Serial.println("  reset        - Reset energy/life (keep genome)");
            Serial.println("  randomize    - Create new random genome");
            Serial.println("  save         - Save current genome to flash");
            Serial.println("  clear        - Clear all saved preferences");
            Serial.println("  threshold X  - Set light threshold (0.0-1.0)");
            Serial.println("  efficiency X - Set efficiency (0.5-1.5)");
            Serial.println("  id X         - Set bot ID (0-8)");
            Serial.println("  wifi         - Show WiFi/network status");
            Serial.println("  led          - Test LED colors");
            Serial.println("  help         - Show this help");
            Serial.println();
        }
        else {
            Serial.println("Unknown command. Type 'help' for command list.");
        }
    }
}