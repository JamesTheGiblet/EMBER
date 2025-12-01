# EMBER Development Roadmap

**From Simple Life to Complex Ecosystems**

---

## Vision

EMBER is not one robot. It's a **life template** that grows in complexity while maintaining the same fundamental equation:

```
energy -= EXISTENCE_COST;
energy += detectResource() * EFFICIENCY;
alive = (energy > 0);
```

This roadmap shows how we add layers of complexity **on top of this foundation** without changing the core pattern. Each version demonstrates new emergent behaviors while proving that the same life equation scales from bacteria to ecosystems.

---

## Development Philosophy

### What Changes

- **Hardware complexity** (sensors, actuators, capabilities)
- **Genome complexity** (more genes, more parameters)
- **Behavioral complexity** (movement, communication, cooperation)
- **Environmental complexity** (multiple resources, dynamic conditions)

### What Doesn't Change

- **The life equation** (always the same)
- **The Mavric Pattern** (physics → life → evolution)
- **Emergence over programming** (behaviors aren't coded, they evolve)
- **Anti-gatekeeping** (all designs remain MIT licensed)

### Core Principle

**If you can remove it without losing the fundamental behavior, remove it.**

Each version is the **minimum viable complexity** to demonstrate the target emergence.

---

## Version Overview

| Version | Status | Focus | Key Feature | Emergence Demonstrated |
|---------|--------|-------|-------------|----------------------|
| **v0.1** | ✅ Superseded | Stationary life | Light sensing, HAL+OTA | Genome selection |
| **v0.2** | ✅ Released | Mobile life | Movement + phototaxis | Resource seeking |
| **v0.3** | 🔄 In Design | Multi-sensory | Multiple resources | Metabolic diversity |
| **v0.4** | 📋 Planned | Communicating | ESP-NOW mesh | Swarm intelligence |
| **v0.5** | 📋 Planned | Specialized | Role variants | Division of labor |
| **v0.6+** | 💭 Conceptual | Reproduction | Self-replication | True artificial life |

---

## v0.1 - Life From Light ✅ RELEASED

**Goal:** Prove the fundamental life pattern works in hardware

### Features

- Stationary platform (motors disabled)
- Dual LDR light sensing (left/right)
- RGB LED state visualization
- Energy system (decay + light gain)
- Genome: `light_threshold`, `efficiency`
- Serial interface for evolution
- **HAL+OTA Edition:**
  - Hardware Abstraction Layer
  - Over-the-air updates
  - Web dashboard monitoring
  - JSON API for data logging
  - Persistent genome storage (NVS flash)
  - WiFi auto-reconnect

### What It Proves

- ✅ Life equation works in physical hardware
- ✅ Different genomes show different survival times
- ✅ Natural selection can optimize genomes
- ✅ Network features enable efficient evolution experiments
- ✅ Same code supports multiple motor driver types (HAL)

### Emergence Observed

- **Genome selection:** Environment favors low thresholds in bright light, high thresholds in dim light
- **Specialization:** Genomes converge on optimal values for specific conditions
- **Trade-offs:** Efficiency vs threshold optimization

### Hardware

```
ESP32 + 2×LDR + RGB LED + Motors (disabled) + Motor Driver + Power
Cost: ~$25 per bot
Build time: 2-3 hours
```

### Current Status

- ✅ Code complete and tested
- ✅ HAL implementation supports L9110S and TB6612FNG
- ✅ OTA updates working
- ✅ Web dashboard operational
- ✅ Documentation complete (README, SPEC, BUILD_GUIDE, EVOLUTION_GUIDE)
- ✅ Ready for evolution experiments

### Next Steps for Users

1. Build 9 bots with network features
2. Run basic evolution experiments (5-10 generations)
3. Use web dashboards and JSON API for monitoring
4. Share results and improvements
5. Prepare for v0.2 (movement)

---

## v0.2 - Mobile Life ✅ RELEASED

**Goal:** Add movement to enable active resource seeking

### New Features

- **Enable motors** (already wired, currently disabled)
- **Phototaxis:** Move toward light sources based on simple, hardcoded logic.
- **Collision avoidance:** Use ultrasonic sensor
- **Genome expansion:**

```cpp
  struct Genome {
      // v0.1 genes
      float light_threshold;
      float efficiency;
      
      // v0.2 Genes (Future Work)
      // float movement_threshold;    // How low before seeking light
      // float turn_sensitivity;      // How much to turn based on L/R difference
      // float exploration_rate;      // Random movement when energy high
      
      uint8_t bot_id;
      uint32_t generation;
  };
```

- **Enhanced fitness:**

```cpp
  fitness = survival_time; // Distance can be added later
```

- **Movement patterns emerge** from genome, not programmed

### What It Will Prove

- ✅ Phototaxis emerges from genome without explicit programming
- ✅ Exploration vs exploitation trade-offs
- ✅ Spatial distribution patterns emerge
- ✅ Movement strategies evolve based on environment

### Emergence Expected

- **Light-seeking behavior:** Bots with good `turn_sensitivity` reach light faster
- **Exploration strategies:** High `exploration_rate` finds distributed resources, low rate exploits known resources
- **Energy management:** Bots learn when to move (costs energy) vs when to stay still
- **Territorial behavior:** Competition for light sources creates spatial patterns

### Code Changes Required

**HAL already supports motors - just enable them:**

```cpp
// v0.1 (current)
void loop() {
    motors.disable();  // Explicitly disabled
    updateEnergy(deltaTime);
    showState();
}

// v0.2 (new)
void loop() {
    updateEnergy(deltaTime);
    
    if (is_alive) {
        updateMovement(deltaTime);  // NEW: Movement logic
    } else {
        motors.stop();
    }
    
    showState();
}

void updateMovement(float deltaTime) {
    float light_diff = lightSensor.readDifference();  // -1.0 to +1.0
    float avg_light = lightSensor.readAverage();
    
    // Should we seek light?
    if (energy < genome.movement_threshold * MAX_ENERGY) {
        // Low energy - seek light
        int turn_amount = (int)(light_diff * genome.turn_sensitivity * 255);
        
        if (abs(light_diff) > 0.1) {
            // Turn toward brighter side
            motors.setMotors(150 - turn_amount, 150 + turn_amount);
        } else {
            // Move forward
            motors.forward(150);
        }
    } else if (random(1000) < genome.exploration_rate * 10) {
        // High energy - explore randomly
        motors.setMotors(random(100, 200), random(100, 200));
    } else {
        // Stay still, conserve energy
        motors.stop();
    }
    
    // Collision avoidance
    float distance = ultrasonic.readDistance();
    if (distance > 0 && distance < 20) {  // 20cm
        motors.backward(100);
        delay(500);
        motors.turnLeft(150);
        delay(300);
    }
}
```

### Evolution Experiments (v0.2)

**Experiment 1: Light-Seeking Evolution**

- Arena with single light source in corner
- Measure: survival time + distance traveled
- Expected: Genomes evolve good `turn_sensitivity`
- Emergence: Efficient path-finding to light

**Experiment 2: Exploration vs Exploitation**

- Arena with multiple light sources
- Variable light intensity over time
- Expected: Some bots exploit (stay at one light), others explore (visit many)
- Emergence: Niche partitioning by strategy

**Experiment 3: Energy Budget Optimization**

- Limited light, movement costs energy
- Expected: Bots evolve optimal `movement_threshold`
- Emergence: When to move vs when to conserve

### Hardware (Same as v0.1)

```
ESP32 + 2×LDR + RGB LED + 2×Motors (ENABLED) + Motor Driver + HC-SR04 + Power
Cost: ~$25 per bot
No new components needed - just enable motors!
```

### Development Timeline

- **Q1 2025:** Prototype movement code
- **Q2 2025:** Test phototaxis algorithms
- **Q3 2025:** Run evolution experiments
- **Q4 2025:** Release v0.2 code

### Migration Path from v0.1

1. Existing hardware works as-is (motors already installed)
2. OTA update from v0.1 to v0.2 code
3. Genome auto-migrates (adds new genes with defaults)
4. Existing experiments can continue or restart

---

## v0.3 - Multi-Sensory Life 📋 PLANNED

**Goal:** Add multiple resource types to demonstrate metabolic diversity

### New Hardware

- **Sound sensor** (microphone module)
- **Temperature sensor** (DHT22 or DS18B20)
- **Optional:** Chemical sensor (MQ-series gas sensor)

### Genome Expansion

```cpp
struct Genome {
    // v0.1-v0.2 genes
    float light_threshold;
    float light_efficiency;
    float movement_threshold;
    float turn_sensitivity;
    float exploration_rate;
    
    // v0.3 NEW genes
    float sound_threshold;       // Minimum sound level
    float sound_efficiency;      // Energy from sound
    float temp_threshold;        // Optimal temperature
    float temp_efficiency;       // Energy from warmth
    
    // Future: chemical_threshold, chemical_efficiency
    
    uint8_t bot_id;
    uint32_t generation;
};
```

### Enhanced Life Equation

```cpp
void updateEnergy(float deltaTime) {
    // Universal decay
    energy -= ENERGY_DECAY * deltaTime;
    
    // Multiple resource pathways
    float light = lightSensor.readAverage();
    if (light > genome.light_threshold) {
        energy += ENERGY_GAIN * (light - genome.light_threshold) 
                  * genome.light_efficiency * deltaTime;
    }
    
    float sound = soundSensor.readLevel();
    if (sound > genome.sound_threshold) {
        energy += ENERGY_GAIN * (sound - genome.sound_threshold)
                  * genome.sound_efficiency * deltaTime;
    }
    
    float temp = tempSensor.readCelsius();
    float temp_diff = abs(temp - genome.temp_threshold);
    if (temp_diff < 5.0) {  // Within 5°C of optimal
        energy += ENERGY_GAIN * (1.0 - temp_diff/5.0)
                  * genome.temp_efficiency * deltaTime;
    }
    
    // ... clamp and death check
}
```

### What It Will Prove

- ✅ Same life equation works with ANY sensor type
- ✅ Metabolic diversity emerges from multi-resource environments
- ✅ Specialists vs generalists trade-off
- ✅ Resource partitioning reduces competition

### Emergence Expected

- **Metabolic specialists:** Bots optimize for one resource (high light_efficiency, low sound_efficiency)
- **Metabolic generalists:** Bots use all resources moderately (balanced efficiencies)
- **Niche partitioning:** Light specialists cluster near lamps, sound specialists near speakers, temp specialists near heaters
- **Resource competition:** Specialists outcompete generalists in resource-rich areas, generalists survive in mixed environments

### Evolution Experiments (v0.3)

**Experiment 1: Resource Abundance**

- Multiple resource types available
- Expected: Specialists emerge (one high efficiency, others low)
- Emergence: Niche partitioning by resource type

**Experiment 2: Resource Scarcity**

- Limited resources, varied types
- Expected: Generalists survive better
- Emergence: Metabolic flexibility as survival strategy

**Experiment 3: Environmental Shift**

- Start with abundant light
- Switch to abundant sound halfway through
- Expected: Population crashes, then re-adapts to sound
- Emergence: Extinction and re-emergence cycles

### Hardware

```
v0.2 hardware + Sound sensor + Temperature sensor
Cost: ~$30 per bot (+$5 for sensors)
```

### Development Timeline

- **2025-2026:** Design and prototyping
- **Release:** TBD based on v0.2 success

---

## v0.4 - Communicating Life 📋 PLANNED

**Goal:** Add inter-bot communication to enable swarm intelligence

### New Features

- **ESP-NOW mesh network** (already supported by ESP32)
- **Information sharing:** Bots broadcast resource locations
- **Coordination:** Bots cooperate to exploit resources
- **Genome expansion for social behavior:**

```cpp
  struct Genome {
      // v0.1-v0.3 genes...
      
      // v0.4 NEW genes
      float signal_threshold;      // When to broadcast (energy level)
      float signal_trust;          // How much to trust others' signals
      float cooperation_rate;      // Willingness to share resources
      
      uint8_t bot_id;
      uint32_t generation;
  };
```

### Communication Protocol

```cpp
struct ResourceSignal {
    uint8_t sender_id;
    uint8_t resource_type;  // 0=light, 1=sound, 2=temp
    float resource_level;   // How much resource
    float direction;        // Angle to resource
    float distance;         // Distance to resource
    uint32_t timestamp;
};

void broadcastResource() {
    if (energy > genome.signal_threshold * MAX_ENERGY) {
        ResourceSignal sig = {
            .sender_id = genome.bot_id,
            .resource_type = 0,  // Light
            .resource_level = lightSensor.readAverage(),
            .direction = calculateDirection(),
            .distance = 0,  // At source
            .timestamp = millis()
        };
        esp_now_send(BROADCAST_MAC, (uint8_t*)&sig, sizeof(sig));
    }
}

void onReceiveSignal(ResourceSignal sig) {
    if (random(1000) < genome.signal_trust * 1000) {
        // Trust the signal, adjust movement
        navigateToward(sig.direction, sig.distance);
    }
    // Else: ignore (skeptical genome)
}
```

### What It Will Prove

- ✅ Communication emerges when beneficial
- ✅ Cooperation vs competition trade-offs
- ✅ Information sharing creates swarm intelligence
- ✅ Trust and deception can co-evolve

### Emergence Expected

- **Cooperative signaling:** Bots broadcast resource locations, others follow
- **Selfish strategies:** Some bots ignore signals (free ride on others' exploration)
- **Deceptive signaling:** Bots broadcast false locations to reduce competition
- **Trust networks:** Bots learn which senders are reliable
- **Swarm behaviors:** Coordinated movement, resource allocation, danger avoidance
- **Cultural evolution:** Successful communication strategies spread non-genetically

### Evolution Experiments (v0.4)

**Experiment 1: Cooperation Emergence**

- Scattered resources
- Expected: Cooperative bots (high `cooperation_rate`) find resources faster
- Emergence: Information sharing as survival strategy

**Experiment 2: Deception Evolution**

- Limited resources, high competition
- Expected: Some bots evolve low `signal_trust` (skeptical)
- Emergence: Arms race between honest signaling and deception

**Experiment 3: Swarm Optimization**

- Complex environment with dynamic resources
- Expected: Coordinated search patterns emerge
- Emergence: Collective intelligence exceeds individual capability

### Hardware (Same as v0.3)

```
No new hardware needed - ESP32 has ESP-NOW built-in
Cost: ~$30 per bot
```

### Development Timeline

- **2026:** Prototyping communication protocols
- **Release:** TBD

---

## v0.5 - Specialized Life 📋 PLANNED

**Goal:** Hardware variants create division of labor

### Hardware Variants

**SCOUT:**

- Extra sensors (camera module?)
- No resource collection (minimal efficiency genes)
- Fast motors
- Role: Explore and broadcast locations

**HARVESTER:**

- High-efficiency resource sensors
- Slow motors
- Large energy capacity
- Role: Stay at resources, collect energy

**GUARD:**

- Proximity sensors
- Medium speed
- Territory defense behaviors
- Role: Protect resource areas

**NURSE:**

- Energy transfer capability (wireless charging coil)
- Social genes (cooperation, altruism)
- Role: Share energy with others

### Genome Per Role

```cpp
struct Genome {
    // All previous genes...
    
    // v0.5 NEW genes
    uint8_t role_preference;     // 0=scout, 1=harvester, 2=guard, 3=nurse
    float altruism_rate;         // Willingness to help others
    float territorial_radius;    // How far to defend
    
    uint8_t bot_id;
    uint32_t generation;
};
```

### What It Will Prove

- ✅ Division of labor emerges from specialization
- ✅ Eusocial structures possible in artificial life
- ✅ Superorganism behavior (colony acts as single entity)
- ✅ Role assignment can evolve

### Emergence Expected

- **Caste system:** Bots specialize into distinct roles
- **Symbiotic relationships:** Scouts + harvesters cooperate
- **Colony-level selection:** Groups with good role distribution survive better than groups with poor distribution
- **Altruistic behaviors:** Nurses sacrifice energy to help others
- **Emergent intelligence:** Colony solves problems no individual can solve

### Evolution Experiments (v0.5)

**Experiment 1: Role Optimization**

- Mixed hardware types
- Expected: Optimal ratio of scouts:harvesters:guards emerges
- Emergence: Division of labor maximizes group fitness

**Experiment 2: Colony Competition**

- Multiple colonies (groups of 5-10 bots each)
- Limited shared resources
- Expected: Colonies with better cooperation survive
- Emergence: Group-level selection

**Experiment 3: Eusocial Structures**

- Nurse bots can share energy
- Expected: Altruism emerges when kin selection possible
- Emergence: Sacrificial behaviors for colony benefit

### Hardware

```
Specialized variants of base platform:
- Scout: +camera, -efficiency
- Harvester: +battery, -speed  
- Guard: +proximity, standard
- Nurse: +wireless charging, +social
Cost: $30-50 per bot depending on variant
```

### Development Timeline

- **2026-2027:** Hardware variant design
- **Release:** TBD

---

## v0.6+ - Reproductive Life 💭 CONCEPTUAL

**Goal:** Self-replication and true artificial life

### The Ultimate Goal

True artificial life requires **reproduction**:

- Bots can create copies of themselves
- Genomes pass to offspring
- Evolution occurs through birth/death, not manual breeding

### Approaches Under Consideration

**Approach 1: Modular Assembly**

- Arena contains component bins (sensors, motors, chassis parts)
- Bots use manipulator arms to assemble offspring
- Genome stored on removable storage (SD card) transferred to offspring
- Energy cost for reproduction = high

**Approach 2: 3D Printing**

- Bot carries 3D printer head
- Prints body parts from feedstock
- Assembles electronics from component library
- Ultra slow but fully autonomous

**Approach 3: Hybrid Biological**

- Bots cultivate biological organisms (bacteria, algae)
- Biological organisms reproduce naturally
- Bots harvest energy from biological processes
- Blurs line between artificial and natural life

**Approach 4: Digital Reproduction**

- Bots "reproduce" by flashing code to new ESP32s
- Human provides blank hardware periodically
- Genome evolution purely software
- Least autonomous but most practical

### Challenges

**Technical:**

- Manipulator arms complex and expensive
- Component assembly requires high precision
- Energy budget for reproduction enormous
- Failure modes multiply

**Philosophical:**

- Is it still "artificial life" if humans provide components?
- Does digital reproduction count?
- Where is the line between life and machine?

### Timeline

- **2027+:** Exploration and experimentation
- **Release:** Unknown - may be perpetually experimental

---

## Cross-Version Features

### Persistent Across All Versions

**Hardware Abstraction Layer (HAL):**

- Clean interfaces for all hardware
- Swap sensors/actuators without code changes
- Support multiple variants (L9110S, TB6612FNG, future drivers)

**Network Infrastructure:**

- OTA updates for all versions
- Web dashboards evolve with features
- JSON API expands but stays backward compatible
- Data logging and monitoring

**Persistent Storage:**

- Genomes always saved to flash
- Evolution history tracked
- Migration between versions automatic

**Evolution Tools:**

- Serial commands consistent across versions
- Web interface for genome manipulation
- Automated evolution scripts work on all versions

### Growing Complexity

| Version | Genes | Sensors | Behaviors | Network Features |
|---------|-------|---------|-----------|-----------------|
| v0.1 | 2 | 2 (LDR×2) | Stationary | Web + API + OTA |
| v0.2 | 5 | 3 (+ultrasonic) | Movement | + Movement tracking |
| v0.3 | 9 | 5 (+sound +temp) | Multi-resource | + Resource maps |
| v0.4 | 12 | 5 (same) | Communication | + Mesh network |
| v0.5 | 15 | 5-8 (variant) | Specialization | + Colony dashboard |
| v0.6+ | ? | ? | Reproduction | ? |

---

## Research Questions By Version

### v0.1 Questions ✅

- Does natural selection work in hardware? **YES**
- Can genomes adapt to environments? **YES**
- Does HAL enable hardware flexibility? **YES**
- Do network features accelerate evolution? **Testing in progress**

### v0.2 Questions

- Does phototaxis emerge without programming?
- What movement strategies evolve?
- How do bots balance exploration vs exploitation?
- Can spatial patterns emerge from simple rules?

### v0.3 Questions

- Do specialists or generalists win in multi-resource environments?
- Does niche partitioning reduce competition?
- Can metabolic flexibility evolve?
- How do populations respond to resource shifts?

### v0.4 Questions

- When does communication evolve?
- Can deception emerge and persist?
- Do swarms show emergent intelligence?
- How does information sharing affect fitness?

### v0.5 Questions

- Can division of labor emerge?
- Do eusocial structures form?
- Is group selection possible?
- Can colonies act as superorganisms?

### v0.6+ Questions

- Can artificial life truly self-replicate?
- Where is the boundary between life and machine?
- Can evolution continue indefinitely without human intervention?

---

## Contributing to the Roadmap

### How You Can Help

**For v0.1 (Now):**

- Build bots and run evolution experiments
- Share results and findings
- Improve documentation
- Test edge cases
- Contribute HAL drivers for new motor controllers

**For v0.2 (Next):**

- Prototype movement algorithms
- Test phototaxis strategies
- Design evolution experiments
- Contribute to codebase

**For v0.3-v0.5 (Future):**

- Suggest sensors and experiments
- Design hardware variants
- Develop communication protocols
- Propose research questions

**For v0.6+ (Distant Future):**

- Explore reproduction mechanisms
- Push boundaries of what's possible
- Challenge assumptions

### Community Development

**Open Source:**

- All code MIT licensed
- All designs published freely
- No gatekeeping, no secrets

**Collaboration:**

- GitHub for code and issues
- Discord for real-time discussion (TBD)
- Forum for long-form discussion (TBD)
- Research papers welcomed

**Education:**

- Classroom use encouraged
- Educational materials developed
- Student projects supported
- Demonstrations at events

---

## Success Metrics By Version

### v0.1 ✅

- [x] Life equation proven in hardware
- [x] Natural selection demonstrated
- [x] HAL supports multiple motor drivers
- [x] Network features operational
- [ ] 100+ bots built by community (in progress)
- [ ] Published evolution results (pending)

### v0.2

- [ ] Phototaxis emergence proven
- [ ] Movement strategies documented
- [ ] 50+ bots running mobility experiments
- [ ] Spatial pattern analysis published

### v0.3

- [ ] Metabolic diversity observed
- [ ] Specialist/generalist trade-offs measured
- [ ] Multi-resource environments tested
- [ ] Niche partitioning documented

### v0.4

- [ ] Swarm intelligence demonstrated
- [ ] Communication protocols evolved
- [ ] Cooperation/competition dynamics mapped
- [ ] Cultural evolution observed

### v0.5

- [ ] Division of labor achieved
- [ ] Eusocial behaviors documented
- [ ] Group selection proven
- [ ] Superorganism emergence observed

### v0.6+

- [ ] Self-replication achieved (any method)
- [ ] Autonomous evolution demonstrated
- [ ] Published in peer-reviewed journal
- [ ] Recognition as true artificial life

---

## Timeline Summary

```
2024  ████████████████████ v0.1 Released (HAL+OTA Edition)
      │
2025  ├─────────────────── v0.2 Development
      │                    (Mobile life, phototaxis)
      │
2026  ├─────────────────── v0.3 Development
      │                    (Multi-sensory, metabolic diversity)
      │
      ├─────────────────── v0.4 Development
      │                    (Communication, swarm intelligence)
      │
2027  ├─────────────────── v0.5 Development
      │                    (Specialization, division of labor)
      │
      └─────────────────── v0.6+ Exploration
                           (Self-replication, true artificial life)
```

**Note:** Timeline is aspirational. Development pace depends on:

- Community contributions
- Research findings
- Hardware availability
- Funding (currently zero - all volunteer)
- Complexity of challenges

---

## The Long-Term Vision

**EMBER is proof that the universal life pattern works.**

By v0.5, we'll have:

- Artificial organisms with complex genomes
- Emergent behaviors at multiple scales
- Division of labor and cooperation
- Swarm intelligence
- **Artificial ecosystems**

By v0.6+, we might have:

- True self-replicating artificial life
- Open-ended evolution
- Artificial organisms we didn't design
- **Life we discovered by building the conditions for emergence**

**This is Forge Theory proven in silicon, plastic, and light.**

The pattern is universal. The emergence is real. The future is open.

---

## Get Involved

**Start with v0.1:**

1. Build the hardware
2. Run evolution experiments
3. Share your results
4. Contribute improvements

**Shape the future:**

- Suggest features for v0.2+
- Design experiments
- Build variants
- Push boundaries

**The roadmap isn't fixed - it's evolving based on what we learn.**

Just like the bots themselves.

---

*EMBER Development Roadmap*  
*From simple life to artificial ecosystems*  
*Part of the Forge Theory Project*  
*MIT License - Build the future openly*
