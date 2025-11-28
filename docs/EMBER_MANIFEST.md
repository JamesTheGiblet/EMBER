# EMBER - The Universal Life Pattern

**Simple rule: Existence costs energy. Resources provide energy. Survive.**

---

## What Is EMBER?

EMBER is the fundamental pattern of life, reduced to its absolute minimum and implemented in hardware.

It is not a specific robot. It is a **life template** - the core equation that every living thing follows, whether biological or artificial:

```cpp
energy -= EXISTENCE_COST;
energy += detectResource() * EFFICIENCY;
alive = (energy > 0);
```

Everything else - movement, sensing, communication, cooperation, competition, speciation - emerges from this foundation when you add complexity. But the foundation itself never changes.

EMBER is where artificial life begins.

---

## The Core Equation

All life faces the same problem:

1. **Existence has a cost** - You lose energy just by being alive
2. **Resources exist in the environment** - But you must detect them
3. **Conversion efficiency varies** - Genes determine how well you extract energy
4. **Survival is mathematical** - `resource_gain > existence_cost`

This is true for:

- Plants (light → chemical energy)
- Animals (food → metabolic energy)  
- Bacteria (nutrients → survival)
- EMBER bots (sensors → electrical energy)

The **sensor changes**. The **equation doesn't**.

---

## Why This Matters

### 1. It's Universal

EMBER works with ANY resource detection mechanism:

- Light sensor → phototroph (survives near light)
- Sound sensor → phonotroph (survives near sound)
- Temperature sensor → thermotroph (survives near heat)
- Chemical sensor → chemotroph (survives near food)
- Proximity sensor → tactilotroph (survives near objects)

The life logic is identical. Only `detectResource()` changes.

### 2. It Enables Evolution

Because survival depends on the genome (threshold, efficiency), natural selection works:

- Random variation in genes
- Environmental selection pressure
- Fittest genomes survive longest
- Copy successful genes to next generation
- Add small mutations
- Repeat

**You can watch evolution happen in real-time.**

### 3. It Produces Emergence

When multiple EMBER bots with different genomes compete for the same resources:

- Specialists emerge (optimized for one resource)
- Generalists emerge (survive anywhere, excel nowhere)
- Niche partitioning emerges (different bots occupy different spaces)
- Communication strategies emerge (cooperation beats competition)
- **None of this is programmed. It emerges from the core equation.**

---

## The Mavric Pattern in Hardware

EMBER demonstrates the three-layer emergence architecture (Mavric Pattern) in physical form:

**Layer 1: Physics**

- Light hits sensor → voltage change
- Current flows through LED → photons emitted
- Energy stored in system → state maintained

**Layer 2: Life**

- Sensor reading → resource detection
- Energy management → survival state
- Genome parameters → individual variation

**Layer 3: Evolution**

- Fitness measurement → survival time
- Genetic variation → different parameters
- Selection pressure → environmental fit
- **Emergent behaviors:** specialization, cooperation, adaptation

Each layer follows **simple rules**. Complexity emerges at layer boundaries.

---

## EMBER Versions

### v0.1 - Life From Light (Current)

- **Hardware:** ESP32 + LDR + RGB LED + Motors (disabled)
- **Genome:** `light_threshold`, `efficiency`
- **Behavior:** Gains energy in light, loses energy in darkness, dies at zero
- **Emergence:** Shows which genomes survive in specific lighting conditions
- **Technology:** HAL + OTA updates + Web dashboard + Persistent storage
- **Status:** Production ready

### v0.2 - Multi-Sensory Life (Planned)

- **Hardware:** v0.1 + additional sensors (sound, temp, proximity)
- **Genome:** Efficiency values for each sensor type
- **Behavior:** Multiple resource pathways, metabolic diversity
- **Emergence:** Specialists vs generalists, niche partitioning

### v0.3 - Mobile Life (Planned)

- **Hardware:** v0.2 + motors enabled + wheels
- **Genome:** Movement strategies, exploration vs exploitation
- **Behavior:** Active resource seeking, spatial distribution
- **Emergence:** Territorial behavior, migration patterns

### v0.4 - Communicating Life (Planned)

- **Hardware:** v0.3 + ESP-NOW mesh network
- **Genome:** Communication willingness, signal interpretation
- **Behavior:** Resource sharing, danger warnings, coordination
- **Emergence:** Swarm intelligence, collective decisions, culture

### v0.5 - Specialized Life (Planned)

- **Hardware:** v0.4 + task-specific variants (SCOUT, HARVESTER, GUARD)
- **Genome:** Role preferences, cooperation strategies
- **Behavior:** Division of labor, symbiotic relationships
- **Emergence:** Eusocial structures, superorganism behavior

---

## Design Philosophy

### Anti-Gatekeeping

All EMBER designs, code, and documentation are MIT licensed. No secrets. No paywalls. Anyone can build, modify, and extend EMBER organisms.

### Minimal Viable Complexity

EMBER uses the **smallest possible rule set** that produces the desired emergence. If you can remove a rule without losing the core behavior, remove it.

### Build To Understand

EMBER isn't a simulation. It's physical hardware following real physical laws. The goal is to **prove emergence works in reality**, not just in theory.

### Evolution Over Programming

Don't program behaviors. Program the **capacity for behaviors to evolve**. If you find yourself writing `if (situation) then (specific_action)`, you're doing it wrong. Write the fitness function and let natural selection find the solution.

---

## The Experiment

### Basic Setup

1. Build 9 EMBER v0.1 bots
2. Give each a unique `bot_id` (0-8) and hostname
3. Flash with identical code but randomized genomes
4. Place in shared environment with light source
5. **Monitor via web dashboard or JSON API**
6. **Watch which ones survive**

### What You'll See

- Some bots thrive (green LED, high energy on dashboard)
- Some bots struggle (red flashing, declining energy graph)
- Some bots die (LED off, energy = 0, status: DEAD)

### The Pattern

- Bots with `light_threshold` **below** ambient light level → survive
- Bots with `light_threshold` **above** ambient light level → die
- Bots with high `efficiency` → gain more from same light
- **The environment selects which genomes survive**

### Evolution Loop (v0.1 HAL+OTA Edition)

1. Run for 10 minutes (or until deaths occur)
2. Check web dashboards or query JSON API for fitness data
3. Identify the 3 longest-surviving genomes
4. Copy their genes to the dead bots (via web interface or serial)
5. Add small mutations (±10%) using mutate button or command
6. Save all genomes to flash
7. Reset energy, restart experiment
8. **Repeat**

After 10 generations, your bot population will be **adapted to your specific light environment**. Different lighting = different optimal genomes.

**All genome changes persist across reboots** thanks to flash storage.

---

## Why "EMBER"?

An ember is:

- **Small** - Minimal viable life
- **Energy** - Glowing with stored heat
- **Potential** - Can ignite into raging fire
- **Fragile** - Dies without fuel
- **Reproductive** - One ember can create many

Like a real ember, EMBER organisms:

- Start simple and small
- Require constant energy to survive  
- Can grow into complex systems (swarms)
- Die if resources run out
- Can spread and multiply when conditions are right

From tiny ember → raging fire.  
From simple organism → artificial ecosystem.

---

## Technology Stack

### Hardware Abstraction Layer (HAL)

EMBER uses a clean HAL to separate hardware from logic:

```cpp
// Clean interfaces instead of raw GPIO
lightSensor.readAverage();  // vs analogRead() + math
led.green(255);             // vs ledcWrite() 
motors.forward(200);        // vs complex PWM setup
```

**Benefits:**

- Swap hardware without touching application code
- Support multiple motor driver types (L9110S, TB6612FNG)
- Portable across different ESP32 variants
- Testable and maintainable

**HAL Components:**

- `LightSensor` - Dual LDR with averaging, difference detection
- `RGBLED` - PWM-based color control with presets
- `MotorDriver` - Unified interface for different H-bridge chips
- `UltrasonicSensor` - Distance measurement (future use)

### Network Features

**Over-The-Air (OTA) Updates:**

- Upload code wirelessly to all bots
- Password protected (configurable)
- Update 9 bots in <5 minutes
- No chassis disassembly required
- Visual feedback via LED (purple = updating, green = success)

**Web Dashboard:**

- Real-time monitoring at `http://ember-bot-N.local/`
- Auto-refreshing live stats (2-second interval)
- Remote control (mutate, reset, save, randomize)
- Beautiful responsive design (works on mobile)
- Shows: energy, light levels, genome, network status

**Persistent Storage:**

- Genome saved to ESP32 NVS flash
- Survives reboots and power loss
- Automatic save on mutations
- Manual save via serial `save` command or web button
- Preferences API (key-value store)
- ~32KB available for application data

**mDNS Support:**

- Human-readable hostnames (`ember-bot-0.local`)
- No need to remember IP addresses
- Service discovery for automation
- Cross-platform (Windows/Mac/Linux with Bonjour/Avahi)

**JSON API:**

- RESTful endpoint: `http://ember-bot-N.local/api/stats`
- Real-time bot telemetry
- Enable swarm monitoring
- Data logging and graphing
- External control systems
- Returns: bot_id, generation, alive status, energy, light levels, genome, network stats

**WiFi Auto-Reconnect:**

- Checks connection every 30 seconds
- Automatic reconnection attempts
- Life continues normally if offline
- Visual feedback (yellow LED flash)

### Evolution Acceleration

The technology stack enables faster evolution cycles:

**Before (v0.1 basic):**

1. Manually check each bot via serial (9 USB connections)
2. Note survival times on paper
3. Manually flash new genomes via USB
4. Wait for each upload to complete

**After (v0.1 HAL+OTA):**

1. Open 9 browser tabs or query JSON API
2. See all fitness data instantly
3. Update genomes via web interface
4. OTA upload to all bots simultaneously
5. All genomes auto-saved to flash

**Result:** 10× faster evolution experiments

---

## Connection to Forge Theory

EMBER is **Forge Theory in silicon**.

Every Forge simulation demonstrates:

- Simple rules → complex behavior
- Local interactions → global patterns
- Random variation → selected adaptation
- Three-layer emergence (physics → life → evolution)

EMBER proves this **works in physical reality**, not just simulation.

When you watch an EMBER bot die in shadow and thrive in light, you're watching the same emergence pattern as:

- **TreeForge:** cells → branches → forest
- **EcoForge:** agents → populations → ecosystems
- **NeuroForge:** neurons → patterns → thought
- **LifeForge:** chemistry → cells → organisms

**The pattern is universal. EMBER is the hardware proof.**

The addition of network features (OTA, web dashboard, API) doesn't change the fundamental pattern - it just makes it easier to observe and study emergence at scale.

---

## Getting Started

See:

- `README.md` - Quick start and feature overview
- `EMBER_v0.1_SPEC.md` - Technical specification for version 0.1
- `EMBER_BUILD_GUIDE.md` - How to build the physical hardware
- `EMBER_EVOLUTION_GUIDE.md` - How to run evolution experiments

Or just:

1. Configure WiFi credentials (optional)
2. Flash the code
3. Place it near a window
4. Open `http://ember-bot-0.local/`
5. Watch it live or die

**That's the whole point.**

---

## The Question EMBER Answers

**Can artificial life emerge from simple rules in physical hardware?**

Build one. Watch it survive or die based on its genome and environment.

Monitor it via web dashboard. Track its evolution via JSON API.

The answer is glowing green in your browser.

---

*EMBER v0.1 HAL+OTA Edition - Created 2025*  
*Part of the Forge Theory Project*  
*MIT License - Build freely, share openly*
