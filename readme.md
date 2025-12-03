# EMBER v0.2 - Mobile Artificial Life

**Simple rule: Existence costs energy. Resources provide energy. Survive.**
**A physical implementation of artificial life. Simple rule: Existence costs energy. Resources provide energy. Survive.**

---

## What We Have Accomplished: The v0.2 Milestone

This moment marks the successful completion of the v0.2 "Mobile Life" milestone. We have transformed the EMBER bots from stationary organisms into a dynamic, mobile population capable of interacting with their environment in a meaningful way.

The codebase has been completely refactored from a single `.ino` file into a robust, modular, and scalable PlatformIO project. This professional structure was essential for managing the complexity of movement, networking, and state management.

### Key Features Now Implemented

- **🏃‍♂️ Mobile Platform:** Bots now move, actively seeking light to replenish energy using their motors.
- **🧠 Simple Behaviors:** The core logic for `decideBehavior()` and `executeBehavior()` is in place, enabling phototaxis (seeking light) and basic obstacle avoidance.
- **🌐 Advanced Web Dashboard:** A rich, auto-refreshing web interface provides real-time monitoring and control (mutate, reset, save genome) for each bot.
- **🏗️ Mature Codebase:**
  - **Hardware Abstraction Layer (HAL):** All hardware is cleanly separated, making the code easy to maintain and adapt.
  - **Modular Structure:** Logic is split into `main.cpp`, `hal.cpp`, and `web_server.cpp`, managed with headers in the `include/` directory.
  - **Professional Build System:** The project is now fully managed by PlatformIO, supporting different environments for USB and OTA uploads.
- **📡 Robust Networking:**
  - **OTA Updates:** Firmware can be updated wirelessly.
  - **Persistent Genome:** Genetic traits are saved to flash memory and survive reboots.
  - **WiFi Auto-Reconnect:** Bots are resilient to network dropouts.
- **🔋 Power Management:** A 5-level power management system monitors battery voltage and adjusts performance to maximize runtime and protect the hardware.

### Emergence We Can Now Observe

- **Resource Seeking:** Bots with effective turning logic will find light sources faster and survive longer.
- **Exploration vs. Exploitation:** The trade-off between moving to find light (costing energy) and staying still to conserve it becomes a key survival factor.
- **Spatial Competition:** Multiple bots competing for a single light source will physically jostle for position, creating emergent territorial behavior.

---

## What's Next: The Path to v0.3

With the v0.2 foundation firmly in place, the next steps focus on running experiments and expanding the bot's sensory capabilities.

1. **Run Mobile Evolution Experiments:**
    - Use the updated `EMBER_EVOLUTION_GUIDE.md` to run experiments where movement is a factor.
    - Evolve new genetic traits related to movement strategy (e.g., `turn_sensitivity`, `exploration_rate`).
    - Analyze how mobility affects the fitness landscape and the speed of evolution.

2. **Refine Behavior and Genome:**
    - Expand the `Genome` struct to include genes that control movement behavior.
    - Refine the `decideBehavior()` logic to create more nuanced actions based on these new genes.

3. **Begin v0.3 - Multi-Sensory Life:**
    - **Goal:** Add new resource types (e.g., sound, temperature) to demonstrate metabolic diversity.
    - **Hardware:** Integrate new sensors like microphones or thermistors into the HAL.
    - **Software:** Expand the `updateLife()` function to account for energy gain from multiple resource types.
    - **Emergence:** Evolve "specialist" bots that are highly efficient at harvesting one resource versus "generalist" bots that can use many.

---

## Current Project Structure

```
EMBER/
├── include/              # Header files (.h)
│   ├── config.h          # Core constants (energy, behavior)
│   ├── credentials_user.h  # User's WiFi credentials (ignored by Git)
│   ├── genome.h          # Genome struct definition
│   ├── globals.h         # Global variables and type definitions
│   ├── hal.h             # Hardware Abstraction Layer interface
│   └── web_server.h      # Web server function prototypes
├── src/                  # Source files (.cpp)
│   ├── hal.cpp           # HAL implementation
│   ├── main.cpp          # Main application logic (setup, loop, life)
│   └── web_server.cpp    # Web server handlers
├── docs/                 # Project documentation
├── tools/                # Python scripts for experiments
└── platformio.ini        # PlatformIO project configuration
```

### How to Build and Upload

### How to Build and Upload

- **Configure WiFi:** Copy `include/credentials_template.h` to `include/credentials_user.h` and fill in your network details.

- **First upload via USB:**

    ```sh
    pio run -e esp32-usb -t upload
    ```

- **Subsequent uploads via OTA:**

    ```sh
    pio run -e esp32-ota -t upload --upload-port <BOT_IP_ADDRESS>
    ```

- **Configure WiFi and bot ID** in your main firmware file before first upload.

### How to Monitor and Control

- **Web dashboard:** Live stats, genome, battery, and controls (mutate, reset, save) at `http://ember-bot-N.local/`.
- **JSON API:** Real-time telemetry at `/api/stats` endpoint.
- **Serial monitor:** Diagnostic output and manual commands.

### Evolution Experiments

- Build 9 bots, each with a unique hostname and bot ID.
- Use the web dashboard and JSON API to track fitness and survival.
- Mutate and breed genomes via web or serial commands.
- All changes persist across reboots and code updates.

---

## Power Management System

EMBER v0.1 includes a battery monitoring and power management system:

- **Voltage divider circuit** for safe battery voltage sensing (20kΩ/10kΩ resistors)
- **ADC pin GPIO32** for battery sense
- **Automatic power modes**: NORMAL, ECONOMY, LOW, CRITICAL, SHUTDOWN (LED color and feature scaling)
- Protects batteries and maximizes runtime

See the [Build Guide](docs/EMBER_BUILD_GUIDE.md#power-management-system-battery-monitoring) for wiring instructions, resistor values, and details on power mode logic.

## The Universal Life Pattern Implemented in Hardware

```txt
╔═══════════════════════════════════════╗
║     EMBER - ARTIFICIAL LIFE v0.1      ║
║         HAL + OTA Edition             ║
║                                       ║
║  Simple rule: Light = Energy = Life  ║
║                                       ║
║  Existence costs energy               ║
║  Resources provide energy             ║
║  Survive                              ║
╚═══════════════════════════════════════╝
```

---

## 🆕 NEW in HAL+OTA Edition

This version adds powerful new features while keeping the core life pattern unchanged:

- **Web Dashboard** - Monitor bots at `http://ember-bot-0.local/` with live stats
- **📡 OTA Updates** - Upload code wirelessly (no USB after first flash)
- **💾 Persistent Storage** - Genome survives reboots and power loss
- **🔄 Auto-Reconnect** - WiFi resilience with automatic recovery
- **📊 JSON API** - Automate swarm monitoring and data logging
- **🏗️ Hardware Abstraction** - Clean HAL for easy hardware swaps

---

## What Is This?

EMBER is **not a robot**. It's a **minimal viable organism**.

A physical implementation of the fundamental equation that all life follows:

```cpp
energy -= EXISTENCE_COST;
energy += detectResource() * EFFICIENCY;
alive = (energy > 0);
energy -= cost_of_living;
energy += gain_from_resources;
is_alive = (energy > 0);
```

Watch it live. Watch it die. Watch evolution happen in real-time.

---

## Quick Start

### If You Just Want to See It Work

1. **Configure WiFi** in `include/credentials_user.h`.

2. **Flash the code** to your ESP32 using the PlatformIO "Upload" task for the `esp32-usb` environment.

3. **Connect battery** and power on

4. **Watch LED sequence**:
   - Yellow = Booting
   - Blue = Connecting to WiFi
   - Green flash = WiFi connected
   - Then: Green (thriving) / Red (dying) / Off (dead)

5. **Access web dashboard** (if WiFi connected):
   - Browser: `http://ember-bot-0.local/`
   - Auto-refreshes every 2 seconds with live stats

6. **Or use the Serial Monitor** (115200 baud):

```
   Light: 0.453 | Energy: 88.1 | Batt: 98.7% (8.3V) | Dist: 45.1cm | Alive: 123s | Status: ALIVE
```

### If You Want to Build One

1. **Read `docs/EMBER_BUILD_GUIDE.md`** - complete assembly instructions
2. **Read `docs/EMBER_v0.1_SPEC.md`** - technical specifications and pin assignments
3. **Gather parts** - ~$30 in components per bot
4. **Assemble** - 3-4 hours for first bot
5. **Flash code** - Arduino IDE with ESP32 support
6. **Test** - follow calibration procedure in BUILD_GUIDE

### If You Want to Run Evolution

1. **Build 9 bots** (population size for meaningful selection)
2. **Read `docs/EMBER_EVOLUTION_GUIDE.md`** - how to run experiments
3. **Create arena** - controlled environment with light source
4. **Run experiment** - place bots, measure survival times
5. **Select winners** - copy genes from longest-surviving bots
6. **Breed next generation** - mutate and repeat
7. **Or automate it all** - use the `evolution_experiment.py` script

---

## Network Features

### Web Interface

Access your bot's dashboard via browser at: `http://ember-bot-N.local/`

**Dashboard displays:**

- 🟢🔴⚫ Live LED indicator (pulses with actual bot state)
- 📊 Life status (alive/dead, energy bar, survival time, uptime)
- 🌞 Environment (all light sensor readings)
- 🧬 Genome (complete genetic code)
- 📡 Network info (hostname, IP, WiFi signal, free memory)

**Control buttons:**

- **🧬 Mutate** - Apply random mutation to genome
- **🔄 Reset Life** - Reset energy to 100, keep genome
- **🎲 Randomize** - Generate completely new random genome
- **💾 Save Genome** - Write current genome to flash immediately

**Auto-refresh:** Page updates every 2 seconds automatically

### Over-The-Air (OTA) Updates

**First upload requires USB**, then all future uploads are wireless:

1. Make code changes
2. Arduino IDE → Tools → Port → **Network Ports** → ember-bot-0
3. In PlatformIO, run the OTA upload task: `pio run -e esp32-ota -t upload --upload-port <IP>`
4. Watch LED turn purple (updating)
5. Green flash 5× = success!

**Update all 9 bots in <5 minutes** - no chassis disassembly required.

**LED indicators during OTA:**

- 🟣 Purple pulse = Uploading
- 🟢 Green flash 5× = Upload successful
- 🔴 Red flash 10× = Upload failed

### Persistent Storage

**Genome survives reboots:**

- Saved to ESP32 NVS flash (non-volatile storage)
- Automatic save on mutations via web interface
- Manual save via serial command `save` or web button
- Survives power loss, reboots, code re-uploads
- Clear storage with serial command `clear`

**What gets saved:**

```cpp
- light_threshold
- efficiency
- bot_id
- generation
```

### WiFi Configuration

**Create and edit `include/credentials_user.h` before first upload:**

```cpp
const char* WIFI_SSID = "YourNetworkName";   // Your 2.4GHz WiFi network name
const char* WIFI_PASSWORD = "YourPassword";    // Your WiFi password
const char* OTA_HOSTNAME = "ember-bot-0";   // Unique hostname for each bot
const char* OTA_PASSWORD = "ember2025";      // OTA update security
```

**Auto-reconnect:** The bot checks its connection every 30 seconds and attempts to reconnect if dropped. Life continues normally even if WiFi is down.

### JSON API

**Endpoint:** `http://ember-bot-N.local/api/stats`

**Returns real-time data:**

```json
{
  "bot_id": 0,
  "generation": 5,
  "alive": true,
  "energy": 73.45,
  "light_level": 0.512,
  "light_left": 0.498, // New in v0.2
  "light_right": 0.526, // New in v0.2
  "distance_cm": 25.3, // New in v0.2
  "threshold": 0.347,
  "efficiency": 1.123,
  "alive_time": 342,
  "uptime": 450,
  "wifi_rssi": -67,
  "free_heap": 234567,
  "battery_v": 8.1,
  "battery_pct": 95.5
}
```

**Perfect for:**

- Data logging scripts
- Swarm monitoring dashboards
- Evolution tracking
- Automated experiments
- External control systems

**Example Python monitoring script:**

```python
import requests
import time

bots = [f"ember-bot-{i}.local" for i in range(9)]

while True:
    for bot in bots:
        try:
            r = requests.get(f"http://{bot}/api/stats", timeout=2)
            data = r.json()
            print(f"{bot}: Energy={data['energy']:.1f}% Gen={data['generation']} {'ALIVE' if data['alive'] else 'DEAD'}")
        except:
            print(f"{bot}: OFFLINE")
    print("-" * 50)
    time.sleep(5)
```

---

## What Makes EMBER Different?

### 1. It's Real, Not Simulated

This isn't software pretending to be life. It's physical hardware following real physical laws:

- Light photons hit silicon photodiodes
- Current flows through transistors
- Energy dissipates as heat
- Death is permanent (until you reset it)

### 2. It Actually Evolves

Not "AI learning" or "neural network training." Actual Darwinian evolution:

- **Random variation** - each bot has different genes
- **Environmental selection** - your lighting determines who lives
- **Inheritance** - copy genes from survivors to next generation
- **Mutation** - small random changes each generation
- **Adaptation emerges** - after 10+ generations, population adapts to your specific environment

### 3. It's Minimal

EMBER uses the **smallest possible rule set** that produces emergence:

- One sensor type (light)
- Two genes (threshold, efficiency)
- Three states (thriving, dying, dead)
- Behaviors emerge (seek light, avoid obstacles, compete for resources)

Nothing is pre-programmed except the capacity to sense, the cost of existence, and the rule for energy gain.

**Everything else emerges.**

### 4. It's Universal

The same code works with ANY resource sensor:

- Swap LDR for microphone → survives near sound
- Swap LDR for thermistor → survives in warmth  
- Swap LDR for chemical sensor → survives near food
- **The life equation doesn't change**

EMBER isn't "a light-seeking robot." It's a **template for artificial life** that happens to use light in version 0.1.

### 5. Hardware Abstraction Layer

Clean interfaces separate hardware from logic:

```cpp
// Instead of raw GPIO:
analogRead(34);
ledcWrite(0, 255);

// Use HAL:
lightSensor.readAverage();
led.green(255);
motors.forward(200);
```

**Benefits:**

- Swap hardware without touching application code
- Support multiple motor driver types automatically
- Portable across different ESP32 variants
- Maintainable and testable

---

## Core Concepts

### The Genome

Each bot has **genetic code** that determines its behavior:

```cpp
struct Genome {
    float light_threshold;  // How much light needed to survive (0.0-1.0)
    float efficiency;       // How well it converts light to energy (0.5-1.5)
    uint8_t bot_id;        // Which physical bot (0-8)
    uint32_t generation;    // Evolutionary generation number
};
```

**These genes determine everything:**

- Bot with `threshold = 0.2` thrives in dim light
- Bot with `threshold = 0.8` needs bright light to survive
- Bot with `efficiency = 1.5` extracts more energy from same photons
- Bot with `efficiency = 0.5` is inefficient, struggles even in good light

**No two bots are identical** (genes randomized on first boot, then saved to flash).

### The Life Cycle

```txt
BIRTH
  ↓
energy = 100
genome loaded from flash (or randomized if new)
  ↓
┌─────────────────────┐
│                     │
│  Read light level   │ ───→ light_level
│        ↓            │
│  if light > threshold:
│    energy += gain   │ ───→ Thriving (green LED)
│  else:               │
│    energy -= decay  │ ───→ Dying (red LED)
│        ↓            │
│  if energy <= 0:    │
│    DEATH            │ ───→ Off (black LED)
│                     │
└─────────────────────┘
       ↑ │
       └─┘ (loop while alive)

(genome persists in flash across reboots)
```

### Fitness

## Fitness = survival time

The longer a bot stays alive in a given environment, the better its genes are suited to that environment.

```txt
Arena with desk lamp:
  Bot #3 (threshold=0.3, efficiency=1.2) → survives 3600s
  Bot #7 (threshold=0.7, efficiency=0.9) → survives 120s
  
Bot #3 has higher fitness → its genes should propagate
```

### Evolution

```txt
Generation 0: Random genomes (saved to flash)
    ↓
Run experiment (measure fitness via web dashboard or API)
    ↓
Identify top 3 survivors
    ↓
Copy their genes to the 6 dead bots (via web or serial)
    ↓
Add small mutations (±10%)
    ↓
Save all genomes to flash
    ↓
Generation 1: Partially adapted genomes
    ↓
Repeat...
    ↓
Generation 10: Fully adapted genomes (all saved)
```

**You will literally watch the population adapt to your specific lighting conditions.**

Different environments produce different optimal genomes:

- **Bright lab** → low thresholds, any efficiency
- **Dim office** → very low thresholds, high efficiency required
- **Variable window light** → medium thresholds, high efficiency (generalists)

---

## Hardware Requirements

### Minimum Viable Build

| Component | Quantity | Approx Cost |
|-----------|----------|-------------|
| ESP32 DevKit (30-pin) | 1 | $6 |
| GL5516 LDR | 2 | $1 |
| RGB LED (common cathode) | 1 | $0.50 |
| Resistors (10kΩ, 220Ω) | 5 | $0.50 |
| Buck converter | 1 | $2 |
| 3.7V 2000mAh LiPo Battery | 2 | $10 |
| Velcro strap/tape | 1 | $1 |
| Breadboard/protoboard | 1 | $3 |
| Wires, connectors | misc | $2 |
| **Total per bot** | | **~$25** |

### Full Mobile Build (v0.2+)

This is now the standard build.

- TT motors (2) - $4
- H-bridge motor driver - $3
- HC-SR04 ultrasonic - $2
- Wheels, chassis - $8
- **Total per bot: ~$35-40**

---

## Software Setup (PlatformIO)

This project uses **PlatformIO** with Visual Studio Code for a more robust development experience.

### 1. Install VS Code and PlatformIO

1. **Install Visual Studio Code:** Download it from [code.visualstudio.com](https://code.visualstudio.com/).
2. **Install PlatformIO IDE Extension:**
    - Open VS Code.
    - Go to the Extensions view (Ctrl+Shift+X).
    - Search for `PlatformIO IDE` and click "Install".
    - Restart VS Code when prompted.

### 2. Build and Upload

1. **Open Project:** In VS Code, go to `File > Open Folder...` and select the root `EMBER` directory.
2. **First Upload (USB):**
    - Connect the ESP32 via USB.
    - Use the PlatformIO "Upload" task for the `esp32-usb` environment.
3. **Wireless Upload (OTA):**
    - After the first USB flash, you can upload wirelessly.
    - Find the bot's IP address from the Serial Monitor.
    - In the PlatformIO CLI terminal, run: `pio run -e esp32-ota -t upload --upload-port <IP_ADDRESS>`
    - Example: `pio run -t upload --upload-port 192.168.1.50`

All libraries are included with ESP32 core:

- WiFi (built-in)
- ESPmDNS (built-in)
- WebServer (built-in)
- ArduinoOTA (built-in)
- Preferences (built-in)

**PlatformIO will handle all dependencies automatically.**

---

## Serial Commands

Interact with your bot via **Serial Monitor** or **Web Interface**:

### Genome Commands

```
genome          → Display current genetic code
mutate          → Apply random mutation (±10%) and save to flash
randomize       → Generate new random genome and save
threshold X     → Manually set light_threshold (0.0-1.0)
efficiency X    → Manually set efficiency (0.5-1.5)
id X            → Set bot_id (0-8)
```

### Life Commands

```
reset           → Reset energy to 100 (keep genome)
```

### Storage Commands (New)

```txt
save            → Save current genome to flash immediately
clear           → Clear all saved preferences (genome reset on next boot)
```

### Sensor Commands (New)

```txt
sensors         → Show all sensor readings (light L/R, ultrasonic, raw ADC)
```

### Network Commands (New)

```txt
wifi            → Show WiFi status, IP, signal strength, hostname, web URL
```

### Utility Commands

```txt
led             → Test LED colors (cycles through R/G/B/W)
help            → Show all commands
```

### Example Session

```
> genome
=================================
Bot ID: 3
Generation: 5
Light Threshold: 0.347
Efficiency: 1.123
=================================

> mutate
Genome mutated and saved!
=================================
Bot ID: 3
Generation: 6
Light Threshold: 0.381
Efficiency: 1.089
=================================

> wifi
--- WiFi Status ---
Connected: Yes
SSID: MyNetwork
IP: 192.168.1.50
RSSI: -67 dBm
Hostname: ember-bot-3.local
Web: http://ember-bot-3.local/
-------------------

> save
Genome saved to flash!
```

---

## Understanding LED States

| LED Color | Pattern | Meaning | Energy | Context |
|-----------|---------|---------|--------|---------|
| 🟡 Yellow | Solid | Booting | - | System initializing |
| 🔵 Blue | Solid | Connecting | - | WiFi connecting |
| 🟢 Green | Solid | Connected | - | WiFi successful |
| 🟡 Yellow | Flashing | Low Battery | < 25% | Power saving warning |
| 🟡 Yellow | 1 flash | Reconnecting | - | WiFi reconnect attempt |
| 🟣 Purple | Pulse | OTA Update | - | Uploading new code |
| 🟢 Green | Flashing | OTA Success | - | Upload complete |
| 🔴 Red | 10 flashes | OTA Failed | - | Upload error |
| 🟢 Green | Solid bright | Thriving | 80-100 | Gaining energy fast |
| 🟢 Green | Solid dim | Healthy | 50-79 | Gaining energy slowly |
| 🔴 Red | Slow flash (1Hz) | Struggling | 20-49 | Losing energy slowly |
| 🔴 Red | Fast flash (5Hz) | Critical | 1-19 | About to die |
| ⚫ Off | No light | Dead | 0 | Energy depleted |

**Flash rate increases as energy drops** - visual urgency signal.

---

## Troubleshooting

### WiFi Issues

**Problem:** Bot won't connect to WiFi

**Fixes:**

1. Check SSID/password are correct (case-sensitive)
2. Ensure 2.4GHz network (ESP32 doesn't support 5GHz)
3. Router must allow mDNS/Bonjour
4. Try moving closer to router
5. Check Serial Monitor for error messages

---

**Problem:** Can't access `ember-bot-0.local`

**Fixes:**

1. Try IP address instead (shown in Serial Monitor)
2. **Windows:** Install [Bonjour Print Services](https://support.apple.com/kb/DL999)
3. **Linux:** Install `avahi-daemon` (`sudo apt install avahi-daemon`)
4. **Mac:** Should work out of box
5. Check firewall isn't blocking port 80

---

**Problem:** OTA not showing in Arduino IDE

**Fixes:**

1. Wait 30 seconds after bot boots
2. Check bot is on same network as computer
3. Refresh port list (close and reopen Tools → Port)
4. Check firewall isn't blocking port 3232
5. Try IP address: `ember-bot-0.local` or `192.168.1.50`

---

### Hardware Issues

**Problem:** Bot dies immediately even in bright light

**Fix:**

1. Use the `sensors` serial command to check readings.
2. Ensure LDRs are wired correctly.
3. Use the web interface to lower the `light_threshold` and click "Reset Life".

Or use web interface: Set threshold lower, click "Reset Life"

---

**Problem:** LED stuck on one color

**Fix:**

1. Check wiring: GPIO14=red, GPIO33=green, GPIO12=blue
2. Serial command: `led` to test colors
3. Verify common cathode connected to GND

---

**Problem:** Light sensor not responding

**Fix:**

1. Verify circuit: 3.3V → LDR → GPIO34/35 → 10kΩ → GND.
2. Serial command: `sensors` to check raw values
3. Measure voltage at GPIO (should be 0-3.3V)

---

**Problem:** Bot resets randomly

**Fix:**

1. Charge batteries (should be 7.4-8.4V)
2. Check all GND connections
3. Add 100µF capacitor across buck converter output
4. Check Serial Monitor for brownout messages

---

## FAQ

### Q: Do I need WiFi for the bot to work?

**A:** No. WiFi is completely optional. If WiFi connection fails or is disabled, the bot continues functioning normally with all core life functions. You just won't have web interface, OTA updates, or remote monitoring. Serial commands still work via USB.

### Q: Can I update the genome via the web interface?

**A:** Yes! The web dashboard has control buttons:

- **🧬 Mutate** = Apply random mutation
- **🎲 Randomize** = New random genome
- **💾 Save** = Write current genome to flash immediately

All changes are automatically saved to flash.

### Q: What happens if WiFi drops during an experiment?

**A:** Life continues completely normally. The bot checks WiFi every 30 seconds and attempts to reconnect automatically. All core functions (sensing, energy management, LED display) are independent of network status. You'll see a brief yellow LED flash when reconnecting.

### Q: How do I monitor all 9 bots at once?

**A:** Three options:

1. **Browser tabs:** Open 9 tabs to each bot's dashboard (auto-refreshes every 2s)
2. **JSON API:** Use a script to poll `/api/stats` endpoint
3. **Serial Monitor:** Connect to each bot via USB (displays stats every second)

See the JSON API section above for a Python monitoring example.

### Q: Can I change WiFi password without USB?

**A:** Yes, via OTA:

1. Edit code with new credentials
2. Upload via network port
3. Bot reboots and connects to new network

**Warning:** If new credentials are wrong, you'll need USB to fix it.

### Q: Does the genome persist across reboots?

**A:** Yes! Every mutation (via web or serial `mutate` command) automatically saves to ESP32 flash memory. Manual save is also available via serial `save` command or web "Save Genome" button. The genome survives:

- Power loss
- Reboots
- Code re-uploads (unless you use `clear` command)
- Battery changes

### Q: Why only light? Why not other sensors?

**A:** v0.1 uses light to prove the concept works. The HAL makes it trivial to swap sensors - just replace the `LightSensor` class. Future versions will have multi-sensory organisms. The life equation stays the same.

### Q: Why can't it move in v0.1?

**A:** It can! This is v0.2. The motors are enabled and the bot will actively seek light and avoid obstacles.

### Q: How is this different from a light-seeking robot?

**A:** A light-seeking robot is pre-programmed: "if light_detected, move_toward_light". EMBER bots have random genes - some will happen to survive, others won't. We create selection pressure and let evolution find the solution. The difference is **emergence vs programming**.

### Q: Do I need 9 bots?

**A:** For meaningful evolution, yes. You need:

- Genetic diversity (multiple genomes)
- Selection (some live, some die)
- Breeding (copy winning genes)

With fewer bots, there's less variation and slower convergence. 3 bots is minimum, 9 is ideal, 20+ is even better.

### Q: Can I run this in simulation?

**A:** You *could*, but you'd miss the point. EMBER proves emergence works in **physical reality**. Simulations make assumptions and skip constraints. Hardware follows real physics - photons, thermodynamics, battery chemistry. **Physical constraints force honesty.**

### Q: What if all my bots die?

**A:** Environment is too harsh. Either:

1. Add more light
2. Lower `ENERGY_DECAY` in code
3. Increase `ENERGY_GAIN` in code
4. Use web interface to randomize all genomes (wider search)

### Q: What if all my bots survive forever?

**A:** No selection pressure. Either:

1. Reduce light intensity
2. Create variable light (shade part of arena)
3. Increase `ENERGY_DECAY`
4. Add competition (limited "food" spots)
5. Proceed to v0.2 (movement + resource seeking)

### Q: How long does a generation take?

**A:** Depends on selection pressure:

- High pressure (harsh environment): 10-30 minutes
- Medium pressure (balanced): 1-2 hours
- Low pressure (easy environment): hours to never

Monitor via web dashboard to see fitness in real-time.

### Q: What happens at Generation 100?

**A:** Population becomes highly specialized to your exact environment. Even small lighting changes cause deaths. This is **niche adaptation** - the same thing that makes pandas dependent on bamboo or koalas on eucalyptus. It emerges from simple rules.

### Q: Can I cross-breed bots?

**A:** Manual crossover in v0.1:

1. View genome of Bot A via web interface
2. View genome of Bot B
3. Take `threshold` from A, `efficiency` from B
4. Use serial commands to set combined genome on Bot C
5. Save to flash

v0.4 will have ESP-NOW communication for automatic genetic exchange.

### Q: Is this actually alive?

**A:** Scientifically, it exhibits:

- **Homeostasis** (energy regulation)
- **Metabolism** (light → energy conversion)
- **Growth** (energy accumulation)
- **Response to stimuli** (light detection, phototaxis in v0.2+)
- **Reproduction** (gene copying to offspring)
- **Evolution** (adaptation over generations)

It follows the same life equation as bacteria, plants, and you.

Philosophically? **You decide.**

---

## What You Can Learn From This

### For Students

- **Biology:** Evolution by natural selection (real, not simulated)
- **Physics:** Energy conservation, photovoltaic effect, thermodynamics
- **Computer Science:** Genetic algorithms, emergence, HAL design, web APIs
- **Engineering:** Embedded systems, sensor integration, OTA updates
- **Philosophy:** What is life? What is intelligence? What is consciousness?

### For Researchers

- **Validation:** Does emergence work in hardware? (Answer: Yes)
- **Platform:** Test evolutionary theories with physical agents
- **Experiments:** Multi-sensory organisms, specialization, swarm intelligence
- **Extensions:** Add sensors/genes/complexity - pattern holds
- **Data:** JSON API enables rigorous data collection

### For Makers

- **Framework:** Reusable life template for any sensor type
- **Learning:** Practical embedded systems with deep concepts
- **HAL Design:** Clean abstraction layer patterns
- **Network Integration:** OTA + web interface + API
- **Philosophy:** Anti-gatekeeping - all designs MIT licensed

---

## Next Steps

### After You Build One Bot

1. **Test basic functions** - sensing, energy, life/death
2. **Try serial commands** - mutate, reset, observe changes
3. **Access web dashboard** - see live stats
4. **Run survival tests** - different lighting, different thresholds
5. **Test OTA** - make code change, upload wirelessly
6. **Document findings** - which genomes work in your environment

### After You Build Nine Bots

1. **Read EVOLUTION_GUIDE.md** - comprehensive evolution protocol
2. **Create test arena** - controlled environment with adjustable light
3. **Set up monitoring** - web dashboards or API script
4. **Run generation 0** - random genomes, measure fitness
5. **Select and breed** - copy winners (via web or serial), mutate, save
6. **Track lineages** - which bots descended from which survivors
7. **Analyze results** - what genome emerged as optimal?
8. **Export data** - JSON API enables long-term tracking

### After Evolution Works

1. **Modify environment** - change light, observe re-adaptation
2. **Add constraints** - limited "food" (light spots), force competition
3. **Cross environments** - move adapted bots to new arena
4. **Build v0.2** - enable movement, watch phototaxis emerge
5. **Publish results** - share your data, help build knowledge
6. **Extend platform** - swap sensors, add genes, explore

---

## Contributing

EMBER is **open source** (MIT License) and **anti-gatekeeping**.

**You can:**

- Build it
- Modify it
- Extend it
- Sell it
- Teach it
- Share it
- Improve it

**Please contribute:**

- Document experiments (PRs welcome)
- Share modifications (sensor swaps, new features)
- Report issues (GitHub)
- Improve documentation
- Build tools (monitoring, analysis, visualization)
- Share your data (evolution results)

**No permission needed. Just build and share.**

---

## Project Philosophy

### Build to Understand

EMBER isn't a simulation. It's physical hardware following real physics. The goal is to **prove emergence works in reality**, not just in theory.

You can't handwave thermodynamics. You can't skip circuit design. You can't pretend about battery voltage.

**Physical constraints force honesty.**

### Simple Rules, Complex Behavior

EMBER demonstrates the **Mavric Pattern** (three-layer emergence):

## Layer 1: Physics

- Photons → voltage
- Transistors → current
- Battery → chemical energy

## Layer 2: Life

- Sensor → perception
- Energy → survival
- Death → selection

## Layer 3: Evolution

- Variation → diversity
- Selection → pressure
- Inheritance → adaptation

**Each layer emerges from the one below. Nothing is pre-programmed except the rules.**

### Anti-Gatekeeping

All EMBER designs are MIT licensed. No secrets. No paywalls. No "contact me for details."

**Why?** Because hoarding knowledge slows progress. Sharing accelerates it.

Build this. Improve it. Share your improvements. Someone will improve those. The project evolves like the bots.

**That's the point.**

---

## Connection to Forge Theory

EMBER is **Forge Theory in silicon**.

Every Forge simulation demonstrates the same pattern:

- Simple rules → complex behavior
- Local interactions → global patterns
- Random variation → selected adaptation
- Three-layer emergence architecture

**EMBER proves this works in physical reality.**

When you watch an EMBER bot die in shadow and thrive in light, you're watching the same emergence as:

- **TreeForge:** cells → branches → forest
- **EcoForge:** agents → populations → ecosystems
- **NeuroForge:** neurons → patterns → thought
- **LifeForge:** chemistry → cells → organisms

**The pattern is universal. EMBER is the hardware proof.**

---

## Resources

### Documentation

- `EMBER_MANIFEST.md` - Philosophy and core concepts
- `docs/EMBER_v0.1_SPEC.md` - Technical specifications (for historical reference)
- `EMBER_BUILD_GUIDE.md` - Assembly instructions
- `EMBER_EVOLUTION_GUIDE.md` - How to run experiments
- `EMBER_ROADMAP.md` - Future versions

### Code

- `src/main.cpp` - Main firmware for the current version.
- `tools/` - Helper scripts for logging, analysis, monitoring

### Hardware

- `/hardware/circuit_diagram.png` - Wiring schematic
- `/hardware/parts_list.md` - Component sourcing
- `/hardware/chassis_design.dxf` - Laser-cut chassis

### Community

- GitHub Issues - bug reports, feature requests
- Discussions - share experiments, ask questions
- Pull Requests - contribute improvements

---

## License

**MIT License** - Build freely, share openly

```txt
Copyright (c) 2025 James @ Giblets Creations

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND.
```

**Translation:** Do whatever you want. No permission needed. Just don't sue me if your bot becomes sentient and takes over.

---

## Credits

**Created by:** James Dearing (ShapedMaker3D / Giblets Creations)  
**Part of:** The Forge Theory Project  
**Philosophy:** Build to understand, share to accelerate  
**Inspiration:** 8 years of WHEELIE project + decades of cross-domain pattern recognition

**Standing on the shoulders of:**

- Darwin (natural selection)
- Von Neumann (self-replicating machines)
- Brooks (behavior-based robotics)
- Holland (genetic algorithms)
- Langton (artificial life)

**But mostly:** Trial and error. Thousands of hours of building, breaking, and learning.

---

## Final Words

**EMBER is where artificial life begins.**

Not in simulation. Not in theory. In physical hardware following real physical laws.

Build one. Watch it live or die based on its genome and environment.

Build nine. Watch evolution happen in real-time. Monitor them via web dashboard.

Change the environment. Watch them adapt. Track it with the JSON API.

Swap the sensor. Watch the same pattern work with different resources.

**This is emergence. This is evolution. This is life.**

Not programmed. Not scripted. Not faked.

**Emergent.**

From simple rules. In silicon and photons.

---

## Ready to Build?

1. Configure WiFi (or skip for offline mode)

2. Flash the code

3. Connect the battery

4. Open `http://ember-bot-0.local/`

5. Place it in light

**Watch what emerges.**

*EMBER v0.2 Mobile Life Edition*  

*Create

*Created by James Gilbert @ Giblets Creations
