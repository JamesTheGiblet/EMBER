# EMBER v0.1 Technical Specification

## Universal Life From Light - Hardware Implementation with HAL+OTA

-----

## Overview

EMBER v0.1 is the first implementation of the universal life pattern. This version uses light as the primary resource, with ultrasonic sensing for future expansion.

**Core Behavior:** Bot gains energy in light, loses energy in darkness, dies when energy reaches zero.

**Hardware Platform:** A networked, mobile-ready platform. It features dual light sensors, an RGB LED for rich status visualization, and integrated WiFi for a web dashboard, JSON API, and Over-The-Air (OTA) updates. Motors are included but disabled until v0.2.

**Software Architecture:** Hardware Abstraction Layer (HAL) for clean hardware/software separation, OTA update capability, web dashboard, persistent storage, and JSON API.

-----

## Hardware Components

### Primary Components

| Component | Specification | Quantity | Purpose |
|-----------|--------------|----------|---------|
| ESP32 DevKit | 30-pin version | 1 | Main controller |
| H-Bridge Motor Driver | Compact digital (L9110S/TB6612FNG) | 1 | Motor control |
| TT Motor | 3-6V DC gearmotor | 2 | Locomotion (v0.2+) |
| LDR | GL5516 photoresistor | 2 | Light sensing (left/right) |
| Ultrasonic Sensor | HC-SR04 | 1 | Proximity detection (future) |
| RGB LED | Common cathode | 1 | State display |
| Buck Converter | 7-12V in, 5V 3A out | 1 | Power regulation |
| LiPo Battery | 3.7V 2000mAh | 2 | Power (series = 7.4V) |
| USB Charging Board | TP4056 dual | 1 | Battery charging |

### Passive Components

| Component | Value | Quantity | Purpose |
|-----------|-------|----------|---------|
| Resistor | 10kΩ | 2 | LDR voltage divider |
| Resistor | 220Ω | 3 | RGB LED current limiting |
| Capacitor | 100µF 16V | 1 | Power smoothing (optional) |

-----

## Power Architecture

[Image of two LiPo batteries wired in series diagram]

````TXT
[Battery 1: 3.7V] ──┐
                    ├─── 7.4V ─┬─→ [Motor Driver] → Motors
[Battery 2: 3.7V] ──┘          │
                               ├─→ [Buck Converter] → 5V ─┬─→ ESP32 (VIN)
                               │                          ├─→ HC-SR04
                               │                          └─→ RGB LED
                               │
                               └─→ [TP4056 Charging Board]


[Image of LM2596 buck converter wiring diagram]


### Power Budget

| Component | Current Draw | Notes |
|-----------|-------------|-------|
| ESP32 (idle) | 80mA | No WiFi activity |
| ESP32 (WiFi active) | 150-240mA | During web serving or OTA |
| Motors (both) | 0-400mA | Disabled in v0.1 |
| RGB LED | 0-60mA | Max brightness, all colors |
| HC-SR04 | 15mA | During ping (not used v0.1) |
| LDRs | <1mA | Negligible |
| **Total (offline)** | ~100mA | ~40 hours runtime |
| **Total (WiFi connected)** | ~150mA | ~26 hours runtime |
| **Total (active WiFi)** | ~250mA | ~16 hours runtime |

**Runtime Notes:**

- v0.1 stationary (motors disabled)
- WiFi auto-reconnects consume brief power spikes
- OTA updates consume peak power temporarily
- Web dashboard serving is lightweight

---

## Network Architecture

### WiFi Configuration

```cpp
const char* WIFI_SSID = "YourNetwork";      // 2.4GHz WiFi SSID
const char* WIFI_PASSWORD = "YourPassword";  // Network password
const char* OTA_HOSTNAME = "ember-bot-0";    // Unique per bot (0-8)
const char* OTA_PASSWORD = "ember2025";      // OTA security
````

### Network Services

| Service | Port | Protocol | Purpose | Access |
|---------|------|----------|---------|--------|
| HTTP Server | 80 | TCP | Web dashboard | `http://ember-bot-N.local/` |
| OTA Updates | 3232 | TCP | Wireless code upload | Arduino IDE network ports |
| mDNS | 5353 | UDP | Hostname resolution | Automatic |

### Auto-Reconnect Behavior

```cpp
const unsigned long WIFI_RECONNECT_INTERVAL = 30000;  // 30 seconds

// Periodic connection check
if (WiFi.status() != WL_CONNECTED) {
    // Attempt reconnection
    // Life continues normally during disconnection
    // Yellow LED flash indicates reconnection attempt
}
```

**Key Features:**

- Non-blocking reconnection (life continues)
- Configurable interval (default 30s)
- Visual feedback via LED
- Graceful degradation (operates offline)

### Persistent Storage (NVS Flash)

**Storage Namespace:** `ember`

**Stored Data:**

```cpp
preferences.putFloat("threshold", genome.light_threshold);
preferences.putFloat("efficiency", genome.efficiency);
preferences.putUChar("bot_id", genome.bot_id);
preferences.putUInt("generation", genome.generation);
```

**Storage Characteristics:**

- Location: ESP32 NVS partition
- Capacity: \~32KB available
- Persistence: Survives reboots, power loss, code updates
- Write cycles: \~100,000 per sector (adequate for experiments)
- Access: Preferences API (key-value store)

### Web Dashboard

**Endpoint:** `http://ember-bot-N.local/`

**Features:**

- Auto-refresh every 2 seconds
- Real-time energy/light graphs
- Live LED indicator (pulses with bot state)
- Genome display
- Network statistics
- Control buttons (mutate, reset, randomize, save)

**Dashboard Sections:**

1. **Life Status:** alive/dead, energy bar, survival time, uptime
2. **Environment:** light levels (left/right/average/diff)
3. **Genome:** all genetic parameters, generation number
4. **Network:** hostname, IP, WiFi RSSI, free heap

### JSON API

**Endpoint:** `http://ember-bot-N.local/api/stats`

**Response Schema:**

```json
{
  "bot_id": 0,
  "generation": 5,
  "alive": true,
  "energy": 73.45,
  "light_level": 0.512,
  "light_left": 0.498,
  "light_right": 0.526,
  "threshold": 0.347,
  "efficiency": 1.123,
  "alive_time": 342,
  "uptime": 450,
  "wifi_rssi": -67,
  "free_heap": 234567
}
```

**Content-Type:** `application/json`

**Use Cases:**

- Automated data logging
- Swarm monitoring dashboards
- Evolution tracking scripts
- External control systems
- Long-term fitness analysis

-----

## Pin Assignments

### ESP32 30-Pin Pinout

```txt
                ESP32 DevKit (30-pin)
                    
    3V3     [ 1]        │        [30] VIN
    GND     [ 2] RGB GND│        [29] GND
    GPIO15  [ 3] INT1   │        [28] GPIO13
    GPIO2   [ 4]        │  RGB B [27] GPIO12
    GPIO4   [ 5] INT2   │  RGB R [26] GPIO14
    GPIO16  [ 6] INT3   │        [25] GPIO27
    GPIO17  [ 7] INT4   │US_TRIG [24] GPIO26
    GPIO5   [ 8]        │US_ECHO [23] GPIO25
    GPIO18  [ 9]        │  RGB G [22] GPIO33
    GPIO19  [10]        │        [21] GPIO32
    GPIO21  [11]        │LDR lhs [20] GPIO35
    RX0     [12]        │LDR rhs [19] GPIO34
    TX0     [13]        │        [18] VN
    GPIO22  [14]        │        [17] VP
    GPIO23  [15]        │        [16] EN
```

### Detailed Pin Mapping

#### Light Sensors (Input)

```cpp
namespace HAL::Pins {
    constexpr int LDR_LEFT = 35;   // Left light sensor (ADC1_CH7)
    constexpr int LDR_RIGHT = 34;  // Right light sensor (ADC1_CH6)
}
```

**Circuit per LDR:**

[Image of LDR voltage divider circuit schematic]

3.3V ──┬─── LDR ───┬─── 10kΩ ─── GND
│           │
│           └─── GPIO34/35 (ADC input)
│
(voltage divider: 0V in darkness, 3.3V in bright light)

```txt

**HAL Interface:**

```cpp
HAL::LightSensor lightSensor(34, 35);

float left = lightSensor.readLeft();      // 0.0-1.0
float right = lightSensor.readRight();    // 0.0-1.0
float avg = lightSensor.readAverage();    // 0.0-1.0
float diff = lightSensor.readDifference(); // -1.0 to +1.0
````

#### RGB LED (Output)

```cpp
namespace HAL::Pins {
    constexpr int LED_RED = 14;    // GPIO14 (RGB R)
    constexpr int LED_GREEN = 33;  // GPIO33 (RGB G)
    constexpr int LED_BLUE = 12;   // GPIO12 (RGB B)
}
```

**Circuit:**

```txt
GPIO14 ── 220Ω ──┬── LED RED ──┐
GPIO33 ── 220Ω ──┼── LED GRN ──┼── GND (common cathode)
GPIO12 ── 220Ω ──┴── LED BLU ──┘
```

**HAL Interface:**

```cpp
HAL::RGBLED led(14, 33, 12);

led.red(255);           // Solid red
led.green(128);         // Dim green
led.blue(255);          // Solid blue
led.white(200);         // White
led.setRGB(r, g, b);    // Custom color
led.off();              // All off
```

**PWM Configuration:**

```cpp
namespace HAL::PWM {
    constexpr int FREQ = 5000;      // 5kHz
    constexpr int RESOLUTION = 8;   // 8-bit (0-255)
    constexpr int CH_RED = 0;       // LEDC channel 0
    constexpr int CH_GREEN = 1;     // LEDC channel 1
    constexpr int CH_BLUE = 2;      // LEDC channel 2
}
```

#### Motor Driver (Configurable)

**Supports both L9110S and TB6612FNG via HAL:**

```cpp
namespace HAL::Pins {
    constexpr int MOTOR_A_IN1 = 15;   // GPIO15
    constexpr int MOTOR_A_IN2 = 2;    // GPIO2
    constexpr int MOTOR_B_IN1 = 16;   // GPIO16
    constexpr int MOTOR_B_IN2 = 17;   // GPIO17
    constexpr int MOTOR_A_EN = 5;     // GPIO5 (PWM or unused)
    constexpr int MOTOR_B_EN = 4;     // GPIO4 (PWM or unused)
    constexpr int MOTOR_STBY = 13;    // GPIO13 (standby or unused)
}
```

**HAL Interface:**

```cpp
HAL::MotorDriver motors(15, 2, 16, 17, 5, 4, 13);

motors.setMotors(100, 100);   // Both forward at speed 100
motors.forward(150);          // Both forward
motors.backward(150);         // Both backward
motors.turnLeft(100);         // Rotate left
motors.turnRight(100);        // Rotate right
motors.stop();                // Stop both
motors.disable();             // Power down (v0.1 default)
```

#### Ultrasonic Sensor (Future)

```cpp
namespace HAL::Pins {
    constexpr int US_TRIGGER = 25;   // GPIO25
    constexpr int US_ECHO = 26;     // GPIO26
}
```

**HAL Interface:**

```cpp
HAL::UltrasonicSensor ultrasonic(25, 26);

float distance = ultrasonic.readDistance();  // Returns cm or -1.0 if no echo
```

-----

## Genome Structure

```cpp
struct Genome {
    // Core survival parameters
    float light_threshold;      // Minimum light needed (0.0-1.0)
    float efficiency;           // Energy conversion rate (0.5-1.5)
    
    // Identity
    uint8_t bot_id;            // Hardware ID (0-8)
    uint32_t generation;        // Evolutionary generation
    
    // Future expansion (v0.2+)
    float movement_threshold;   // When to seek light
    float exploration_rate;     // How much to wander
};
```

### v0.1 Active Genes

| Gene | Range | Default | Storage | Purpose |
|------|-------|---------|---------|---------|
| `light_threshold` | 0.0 - 1.0 | random | Flash | Minimum light level to gain energy |
| `efficiency` | 0.5 - 1.5 | 1.0 | Flash | How well light converts to energy |
| `bot_id` | 0 - 8 | manual | Flash | Physical bot identifier |
| `generation` | 0 - ∞ | 0 | Flash | Tracks evolutionary lineage |

**Genetic Variation:**

- Each bot starts with randomized genes (or loads from flash)
- Mutations are ±10% of current value
- All changes saved to NVS flash
- Survives reboots and code updates
- Manual gene copying via web interface or serial

**Persistence:**

```cpp
// Automatic save on mutations (web or serial)
void mutateGenome() {
    // Apply mutations...
    saveGenome();  // Auto-save to flash
}

// Manual save available
void saveGenome() {
    preferences.begin("ember", false);
    preferences.putFloat("threshold", genome.light_threshold);
    preferences.putFloat("efficiency", genome.efficiency);
    preferences.putUChar("bot_id", genome.bot_id);
    preferences.putUInt("generation", genome.generation);
    preferences.end();
}

// Auto-load on boot
void loadGenome() {
    preferences.begin("ember", true);
    if (preferences.isKey("threshold")) {
        genome.light_threshold = preferences.getFloat("threshold");
        genome.efficiency = preferences.getFloat("efficiency");
        genome.bot_id = preferences.getUChar("bot_id");
        genome.generation = preferences.getUInt("generation");
    }
    preferences.end();
}
```

-----

## Energy System

### Energy Dynamics

```cpp
const float ENERGY_DECAY = 0.1;  // Energy lost per second (existence cost)
const float ENERGY_GAIN = 2.0;   // Energy gained per second in good light
const float MAX_ENERGY = 100.0;
const float MIN_ENERGY = 0.0;
```

### Update Algorithm

```cpp
void updateEnergy(float deltaTime) {
    // Universal cost of existence
    energy -= ENERGY_DECAY * deltaTime;
    
    // Resource extraction via HAL
    float light_level = lightSensor.readAverage();
    
    if (light_level > genome.light_threshold) {
        float excess = light_level - genome.light_threshold;
        energy += ENERGY_GAIN * excess * genome.efficiency * deltaTime;
    }
    
    // Clamp to valid range
    energy = constrain(energy, MIN_ENERGY, MAX_ENERGY);
    
    // Death condition
    if (energy <= 0.0) {
        is_alive = false;
    }
}
```

### Energy States

| Energy Level | State | LED Behavior | Web Dashboard |
|-------------|-------|--------------|---------------|
| 80-100 | Thriving | Solid green, bright | Green bar, "ALIVE" |
| 50-79 | Healthy | Solid green, dimmer | Yellow bar, "ALIVE" |
| 20-49 | Struggling | Red flashing (slow) | Orange bar, "ALIVE" |
| 1-19 | Critical | Red flashing (fast) | Red bar, "ALIVE" |
| 0 | Dead | LED off | Gray bar, "DEAD" |

**LED Flash Rate Formula:**

```cpp
int flash_period = 100 + (int)(energy * 9);  // 100ms (critical) to 1000ms (struggling)
```

-----

## LED State Display

### Complete LED Status Codes

| LED Color | Pattern | Meaning | Context |
|-----------|---------|---------|---------|
| ⚪ White | 3 flashes | Boot complete | System initialized |
| 🔵 Blue | Slow pulse | WiFi connecting | Network setup |
| 🔵 Blue | 3 flashes | WiFi connected | Network ready |
| 🟡 Yellow | 3 flashes | WiFi failed | Offline mode |
| 🟡 Yellow | 1 flash | WiFi reconnecting | Auto-reconnect |
| 🟣 Purple | Pulse | OTA updating | Code upload |
| 🟢 Green | 5 flashes | OTA success | Upload complete |
| 🔴 Red | 10 flashes | OTA failed | Upload error |
| 🟢 Green | Solid bright | Thriving | Energy 80-100 |
| 🟢 Green | Solid dim | Healthy | Energy 50-79 |
| 🔴 Red | Slow flash | Struggling | Energy 20-49 |
| 🔴 Red | Fast flash | Critical | Energy 1-19 |
| ⚫ Off | No light | Dead | Energy 0 |

### HAL LED Implementation

```cpp
void showState() {
    if (!is_alive) {
        led.off();
        return;
    }
    
    float light = lightSensor.readAverage();
    int brightness = (int)(energy * 2.55);  // Map 0-100 to 0-255
    
    if (light > genome.light_threshold) {
        led.green(brightness);  // Thriving
    } else {
        // Dying - flash rate increases as energy drops
        int flash_period = 100 + (int)(energy * 9);
        int flash = (millis() / flash_period) % 2;
        if (flash) {
            led.red(brightness);
        } else {
            led.off();
        }
    }
}
```

-----

## Sensor Calibration

### Light Sensor Characteristics

**GL5516 LDR Specifications:**

- Dark resistance: 1MΩ
- Bright resistance: 10kΩ (10 lux)
- Response time: \~20ms

**Voltage Divider Output:**

```txt
         3.3V
          │
        [LDR]  ← Varies with light
          │
          ├─── To ADC (GPIO34/35)
          │
        [10kΩ] ← Fixed
          │
         GND

V_out = 3.3V × (10kΩ / (R_ldr + 10kΩ))

Dark:   ~0.03V  (ADC: ~40,   normalized: 0.01)
Dim:    ~0.5V   (ADC: ~600,  normalized: 0.15)
Room:   ~1.5V   (ADC: ~1850, normalized: 0.45)
Bright: ~3.0V   (ADC: ~3700, normalized: 0.90)
```

### HAL Sensor Access

```cpp
// Via HAL (recommended)
float light = lightSensor.readAverage();     // 0.0-1.0 normalized
float left = lightSensor.readLeft();         // 0.0-1.0
float right = lightSensor.readRight();       // 0.0-1.0
float diff = lightSensor.readDifference();   // -1.0 to +1.0

// Raw ADC access (for debugging)
int raw_left = lightSensor.readRawLeft();    // 0-4095
int raw_right = lightSensor.readRawRight();  // 0-4095
```

### Calibration Procedure

**Automated via Serial Commands:**

```txt
sensors    → Shows all sensor readings (normalized + raw)
```

**Manual Calibration Steps:**

1. **Dark:** Cover LDRs → expect reading \<0.05
2. **Bright:** Flashlight → expect reading \>0.95
3. **Ambient:** Typical room → record baseline (e.g., 0.45)
4. **Set Threshold:** Via web or serial to \~0.5 × ambient

**Web Dashboard Calibration:**

- Real-time sensor readings visible
- Adjust threshold with buttons
- Save to flash immediately
- Visual feedback via live LED

-----

## Fitness Measurement

### Primary Metric: Survival Time

```cpp
unsigned long alive_time = 0;  // Milliseconds alive
unsigned long boot_time = 0;   // System uptime reference

void loop() {
    if (is_alive) {
        alive_time = millis() - boot_time;
    }
    // alive_time freezes at death
}
```

### Fitness Function

```txt
fitness = alive_time (in seconds)

Higher fitness = better adapted genome for environment
```

### Fitness Tracking

**Via Web Dashboard:**

- Real-time survival time display
- Uptime vs alive time comparison
- Generation number tracking
- Visual energy bar

**Via JSON API:**

```json
{
  "alive_time": 342,     // seconds alive
  "uptime": 450,         // total uptime (includes dead time)
  "generation": 5,       // evolutionary lineage
  "alive": true          // current status
}
```

### Secondary Metrics (Logged)

**Available via API but not stored:**

- `average_energy`: Real-time only
- `peak_energy`: Not tracked (always 100)
- `time_above_50`: Calculable from logs

**For Long-Term Analysis:**
Use JSON API polling to log:

```python
import requests
import time
import csv

with open('fitness_log.csv', 'w') as f:
    writer = csv.writer(f)
    writer.writerow(['timestamp', 'bot_id', 'generation', 
                     'alive', 'alive_time', 'energy'])
    
    while True:
        for bot_id in range(9):
            data = requests.get(f'http://ember-bot-{bot_id}.local/api/stats').json()
            writer.writerow([time.time(), data['bot_id'], data['generation'],
                           data['alive'], data['alive_time'], data['energy']])
        time.sleep(5)
```

-----

## Serial Interface

### Commands

```cpp
// Genome inspection
"genome"          → Display current genetic code
"stats"           → Show energy, light, alive time, IP address

// Evolution controls  
"mutate"          → Apply random mutation (±10%) and save to flash
"reset"           → Reset energy to 100 (keep genome)
"randomize"       → Generate new random genome and save

// Manual gene editing
"threshold X"     → Set light_threshold to X (0.0-1.0)
"efficiency X"    → Set efficiency to X (0.5-1.5)
"id X"            → Set bot_id to X (0-8)

// Storage commands
"save"            → Manually save genome to flash
"clear"           → Clear all saved preferences

// Sensor diagnostics
"sensors"         → Show all sensor readings (light L/R, ultrasonic, raw ADC)

// Network commands
"wifi"            → Show WiFi status, IP, RSSI, hostname, web URL

// Utility
"led"             → Test LED colors (cycles R/G/B/W)
"help"            → List all commands
```

### Output Format

```txt
=================================
Bot ID: 3
Generation: 5
Light Threshold: 0.347
Efficiency: 1.123
=================================

Light: 0.512 | Energy: 73.2 | Alive: 342s | Status: ALIVE | IP: 192.168.1.50

--- WiFi Status ---
Connected: Yes
SSID: MyNetwork
IP: 192.168.1.50
RSSI: -67 dBm
Hostname: ember-bot-3.local
Web: http://ember-bot-3.local/
-------------------
```

-----

## Physical Assembly

### Chassis Layout

```txt
                    [Front]
                      ___
                     |US |  ← Ultrasonic sensor
                     |___|
          
    [LDR_L]                   [LDR_R]
       ●                         ●
       
    ┌─────────────────────────────┐
    │                             │
    │        [ESP32]              │
    │                             │
    │   [Buck]    [Motor Driver]  │
    │                             │
    │  [Batt1]      [Batt2]       │
    │                             │
    │        [RGB LED]            │  ← Visible from top/front
    │                             │
    └─────────────────────────────┘
    
    [Motor_L]               [Motor_R]
         ◐                       ◐
```

### Mounting Positions

1. **LDRs:** Front corners, unobstructed, 45° outward
2. **RGB LED:** Top center, visible from all angles
3. **Ultrasonic:** Front center, level, unobstructed
4. **ESP32:** Center, USB accessible, WiFi antenna clear
5. **Batteries:** Low for stability
6. **Buck converter:** Near ESP32 power input
7. **Motor driver:** Near motors, short wire runs

-----

## Expected Behaviors (v0.1)

### Scenario 1: Bright Constant Light + WiFi

**Setup:** Bot under desk lamp with WiFi configured

**Expected:**

- Energy rises to 100 quickly
- LED solid bright green
- Web dashboard shows "Thriving"
- Survives indefinitely
- Genome auto-saved to flash

**Genome Selection:** Favors LOW `light_threshold`

-----

### Scenario 2: Dim Light + Network Monitoring

**Setup:** Room light, all bots on WiFi

**Expected:**

- Some survive (low threshold genomes)
- Some struggle (medium threshold genomes)
- Some die (high threshold genomes)
- Web dashboards show different states
- JSON API enables batch monitoring

**Genome Selection:** Favors `threshold` \< ambient

-----

### Scenario 3: WiFi Dropout During Experiment

**Setup:** Disable router mid-experiment

**Expected:**

- Bots continue living normally
- Yellow LED flash indicates reconnect attempts
- Serial monitoring still works via USB
- Life/death unaffected by network status
- Genomes remain saved in flash

**Robustness Test:** Network is optional, life is not

-----

### Scenario 4: OTA Evolution Cycle

**Setup:** 9 bots, generation 0, web monitoring

**Expected:**

1. View all dashboards in browser tabs
2. Identify survivors after 30 minutes
3. Use web interface to copy genomes
4. Mutate via web buttons (auto-saves)
5. Reset life via web
6. Update all bot code via OTA
7. Start generation 1

**Efficiency:** 10× faster than USB-based evolution

-----

## Troubleshooting

### Network Issues

**Problem:** Bot shows yellow flash (WiFi failed)

**Checks:**

1. SSID/password correct (case-sensitive)
2. 2.4GHz network (ESP32 doesn't support 5GHz)
3. Router within range
4. Bot continues functioning (this is OK)

**Fix:**

```cpp
// Serial command:
wifi  // Shows detailed connection status
```

-----

**Problem:** Web dashboard doesn't load

**Checks:**

1. WiFi connected (blue flash 3×)
2. Same network as computer
3. Try IP instead of .local hostname
4. Firewall not blocking port 80

**Fix:**

- Use IP from Serial Monitor
- Install Bonjour (Windows) or avahi (Linux)

-----

**Problem:** Genome doesn't persist across reboots

**Checks:**

1. Mutations auto-save (check Serial for "[Storage] Genome saved")
2. Manual save works (`save` command)
3. NVS partition not full

**Fix:**

```cpp
// Serial commands:
save     // Manual save
genome   // Verify after reboot
clear    // If corrupted, clear and start fresh
```

-----

### Hardware Issues

(Unchanged from previous spec - HAL handles hardware variations)

-----

## Component Sourcing

### Critical Specifications

| Component | Critical Spec | Why It Matters |
|-----------|--------------|----------------|
| ESP32 | 30-pin DevKit with WiFi | Network features require WiFi |
| LDR | GL5516 or similar | 10kΩ-1MΩ range |
| RGB LED | Common cathode | HAL assumes this polarity |
| Motor Driver | L9110S or TB6612FNG | HAL supports both |
| Buck converter | 5V 3A output | WiFi increases peak current |
| Batteries | LiPo 2000mAh+ | Runtime with WiFi |

-----

## Safety Notes

(Unchanged from previous spec)

-----

## Version History

### v0.1 HAL+OTA Edition - Current

- Hardware Abstraction Layer (HAL)
- Over-The-Air (OTA) updates
- Web dashboard (`http://ember-bot-N.local/`)
- JSON API (`/api/stats`)
- Persistent storage (NVS flash)
- WiFi auto-reconnect
- mDNS support
- Stationary platform (motors disabled)
- Dual LDR light sensing
- RGB LED state display with network indicators
- Enhanced serial command interface
- Remote evolution experiments via web

### v0.2 - Planned

- Enable motor control via HAL
- Light-seeking behavior (phototaxis)
- Web interface shows movement
- Enhanced fitness: survival time + distance

### v0.3 - Planned

- Ultrasonic collision avoidance via HAL
- Exploration vs exploitation
- Real-time movement tracking via API

-----

## Files in This Release

```txt
/ember/v0.1/
  ├── ember_v0.1_hal_ota.ino     # Main code (HAL+OTA edition)
  ├── EMBER_v0.1_SPEC.md         # This file
  ├── EMBER_BUILD_GUIDE.md       # Assembly instructions
  ├── EMBER_MANIFEST.md          # Philosophy
  └── README.md                   # Quick start guide
```

-----

## Success Criteria

✅ **v0.1 HAL+OTA Edition is successful when:**

### Core Life Functions

1. Bot survives indefinitely in bright light (energy stable at 100)
2. Bot dies predictably in darkness (\~1000 seconds)
3. Different genomes show different survival times
4. LED clearly indicates energy state + network status

### Network Features

1. WiFi connects successfully (blue flash 3×)
2. Web dashboard accessible and auto-refreshes
3. JSON API returns valid data
4. OTA updates work (purple LED, green success flash)
5. Genome persists across reboots (NVS flash)
6. Bot operates normally when WiFi offline

### Evolution Capabilities

1. Serial commands allow gene manipulation and save to flash
2. Web interface enables remote genome control
3. Nine bots show genetic diversity
4. JSON API enables automated fitness tracking
5. OTA allows simultaneous code updates to all bots

**If all 15 criteria met, EMBER v0.1 HAL+OTA is production ready.**

Evolution experiments can scale efficiently.

-----

*EMBER v0.1 Technical Specification - HAL+OTA Edition* *Updated: 2025* *Part of the Forge Theory Project*
