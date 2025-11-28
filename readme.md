# EMBER v0.1 - Life From Light

**The Universal Life Pattern Implemented in Hardware**
```
╔═══════════════════════════════════════╗
║     EMBER - ARTIFICIAL LIFE v0.1      ║
║                                       ║
║  Simple rule: Light = Energy = Life  ║
║                                       ║
║  Existence costs energy               ║
║  Resources provide energy             ║
║  Survive                              ║
╚═══════════════════════════════════════╝
```

---

## What Is This?

EMBER is **not a robot**. It's a **minimal viable organism**.

A physical implementation of the fundamental equation that all life follows:
```cpp
energy -= EXISTENCE_COST;
energy += detectResource() * EFFICIENCY;
alive = (energy > 0);
```

Watch it live. Watch it die. Watch evolution happen in real-time.

---

## Quick Start

### If You Just Want to See It Work

1. **Flash the code** to ESP32 (change `bot_id` to unique number 0-8)
2. **Connect battery** and power on
3. **Place in light** and watch LED:
   - 🟢 **Green** = Thriving (gaining energy)
   - 🔴 **Red flashing** = Dying (losing energy)
   - ⚫ **Off** = Dead (energy = 0)
4. **Open Serial Monitor** (115200 baud) to see:
```
   Light: 0.512 | Energy: 73.2 | Alive: 342s | Status: ALIVE
```

### If You Want to Build One

1. **Read the BUILD_GUIDE.md** - complete assembly instructions
2. **Read the SPEC.md** - technical specifications and pin assignments
3. **Gather parts** - ~$30 in components per bot
4. **Assemble** - 3-4 hours for first bot
5. **Flash code** - Arduino IDE with ESP32 support
6. **Test** - follow calibration procedure in BUILD_GUIDE

### If You Want to Run Evolution

1. **Build 9 bots** (population size for meaningful selection)
2. **Read EVOLUTION_GUIDE.md** - how to run experiments
3. **Create arena** - controlled environment with light source
4. **Run experiment** - place bots, measure survival times
5. **Select winners** - copy genes from longest-surviving bots
6. **Breed next generation** - mutate and repeat

---

## Project Structure
```
/ember/
├── README.md                      ← You are here
├── ember_v0.1_light.ino           ← Main code (flash this)
│
├── /docs/
│   ├── EMBER_MANIFEST.md          ← Philosophy: what EMBER is and why
│   ├── EMBER_v0.1_SPEC.md         ← Technical spec: pins, circuits, behavior
│   ├── EMBER_BUILD_GUIDE.md       ← Assembly: how to build the hardware
│   ├── EMBER_EVOLUTION_GUIDE.md   ← Experiments: how to run evolution
│   └── EMBER_ROADMAP.md           ← Future: v0.2, v0.3, v0.4...
│
├── /hardware/
│   ├── circuit_diagram.png        ← Wiring schematic
│   ├── parts_list.md              ← Shopping list with links
│   ├── chassis_design.dxf         ← Laser-cut chassis (optional)
│   └── /photos/
│       ├── assembly_step1.jpg
│       ├── assembly_step2.jpg
│       └── ...
│
├── /experiments/
│   ├── log_template.md            ← Template for recording results
│   └── /results/
│       ├── experiment_001.md      ← Your evolution data goes here
│       └── ...
│
└── /tools/
    ├── serial_logger.py           ← Auto-log fitness data
    ├── genome_analyzer.py         ← Analyze which genes win
    └── batch_flasher.sh           ← Flash multiple bots quickly
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
- Four behaviors emerge (seek light, avoid darkness, compete for resources, adapt to environment)

Nothing is pre-programmed except the capacity to sense, the cost of existence, and the rule for energy gain.

**Everything else emerges.**

### 4. It's Universal

The same code works with ANY resource sensor:
- Swap LDR for microphone → survives near sound
- Swap LDR for thermistor → survives in warmth  
- Swap LDR for chemical sensor → survives near food
- **The life equation doesn't change**

EMBER isn't "a light-seeking robot." It's a **template for artificial life** that happens to use light in version 0.1.

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

**No two bots are identical** (genes randomized on first boot).

### The Life Cycle
```
BIRTH
  ↓
energy = 100
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
```

### Fitness

**Fitness = survival time**

The longer a bot stays alive in a given environment, the better its genes are suited to that environment.
```
Arena with desk lamp:
  Bot #3 (threshold=0.3, efficiency=1.2) → survives 3600s
  Bot #7 (threshold=0.7, efficiency=0.9) → survives 120s
  
Bot #3 has higher fitness → its genes should propagate
```

### Evolution
```
Generation 0: Random genomes
    ↓
Run experiment (measure fitness)
    ↓
Identify top 3 survivors
    ↓
Copy their genes to the 6 dead bots
    ↓
Add small mutations (±10%)
    ↓
Generation 1: Partially adapted genomes
    ↓
Repeat...
    ↓
Generation 10: Fully adapted genomes
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
| 18650 batteries | 2 | $8 |
| Battery holder | 1 | $2 |
| Breadboard/protoboard | 1 | $3 |
| Wires, connectors | misc | $2 |
| **Total per bot** | | **~$25** |

### Full Mobile Build (v0.2+)

Add to above:
- TT motors (2) - $4
- H-bridge motor driver - $3
- HC-SR04 ultrasonic - $2
- Wheels, chassis - $8
- **Total per bot: ~$42**

**v0.1 doesn't need motors** (stationary version to prove concept).

---

## Software Requirements

### Arduino IDE Setup

1. **Install Arduino IDE** (1.8.19+ or 2.x)
2. **Add ESP32 board support:**
```
   File → Preferences → Additional Board Manager URLs:
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
```
3. **Install ESP32 boards:**
```
   Tools → Board → Boards Manager → Search "ESP32" → Install
```
4. **Select board:**
```
   Tools → Board → ESP32 Dev Module
   Tools → Upload Speed → 115200
```

### Flashing the Code

1. Open `ember_v0.1_light.ino`
2. **Change bot_id** (critical - each bot needs unique ID):
```cpp
   Genome genome = {
       .light_threshold = 0.5,
       .efficiency = 1.0,
       .bot_id = 0,  // ← CHANGE THIS (0-8 for 9 bots)
       .generation = 0
   };
```
3. Connect ESP32 via USB
4. Click Upload (**→** button)
5. Wait for "Done uploading"
6. Open Serial Monitor (115200 baud)

---

## Serial Commands

Interact with your bot via Serial Monitor:
```
genome          → Display current genetic code
stats           → Show energy, light, alive time
mutate          → Apply random mutation (±10%)
reset           → Reset energy to 100 (keep genome)
randomize       → Generate new random genome
threshold X     → Manually set light_threshold (0.0-1.0)
efficiency X    → Manually set efficiency (0.5-1.5)
help            → Show all commands
```

### Example Session
```
> genome
=================================
Bot ID: 3
Generation: 0
Light Threshold: 0.347
Efficiency: 1.123
=================================

> mutate
Genome mutated!
=================================
Bot ID: 3
Generation: 1
Light Threshold: 0.381
Efficiency: 1.089
=================================

> stats
Light: 0.512 | Energy: 73.2 | Alive: 342s | Status: ALIVE
```

---

## Understanding LED States

| LED Color | Pattern | Meaning | Energy | Action |
|-----------|---------|---------|--------|--------|
| 🟢 Green | Solid bright | Thriving | 80-100 | Gaining energy fast |
| 🟢 Green | Solid dim | Healthy | 50-79 | Gaining energy slowly |
| 🔴 Red | Slow flash (1Hz) | Struggling | 20-49 | Losing energy slowly |
| 🔴 Red | Fast flash (5Hz) | Critical | 1-19 | About to die |
| ⚫ Off | No light | Dead | 0 | Energy depleted |
| ⚪ White | 3 flashes | Boot | 100 | System initialized |

**Flash rate increases as energy drops** - visual urgency signal.

---

## Example Experiments

### Experiment 1: Survival Test

**Goal:** Verify basic life functions work

**Setup:**
1. Fully charge bot
2. Place in room with stable light
3. Start timer

**Expected results:**
- Bright room (>0.6 light): survives indefinitely
- Dim room (0.3-0.6): depends on genome
- Dark (<0.1): dies in ~16 minutes

**Success criteria:** Bot behavior matches expectations based on genome

---

### Experiment 2: Threshold Test

**Goal:** Understand how `light_threshold` affects survival

**Setup:**
1. Set bot to known genome: `threshold 0.5` + `efficiency 1.0`
2. Place in adjustable lighting (dimmer switch)
3. Slowly decrease light level

**Expected results:**
- Light > 0.5: green LED, energy rises
- Light = 0.5: energy stable
- Light < 0.5: red LED, energy falls

**Success criteria:** Transition occurs exactly at threshold value

---

### Experiment 3: Simple Evolution

**Goal:** Demonstrate natural selection in 3 generations

**Setup:**
1. Build 3 bots with random genomes
2. Place all in same environment (desk lamp)
3. Record survival time for each
4. Copy best genome to other two bots
5. Mutate slightly
6. Repeat

**Expected results:**
- Gen 0: Wide variation in survival (random genes)
- Gen 1: Less variation (all from same parent)
- Gen 2: Population adapts to lamp brightness
- Gen 3: All three survive similarly (convergence)

**Success criteria:** Generation 3 survival time > Generation 0 average

---

### Experiment 4: Environmental Adaptation

**Goal:** Show same genome fails in different environments

**Setup:**
1. Breed bot optimized for bright light (threshold ≈ 0.7)
2. Test in three environments:
   - Bright: under desk lamp
   - Medium: room light
   - Dark: covered box

**Expected results:**
- Bright: survives well (optimized for this)
- Medium: struggles or dies (below threshold)
- Dark: dies quickly (no light at all)

**Success criteria:** Same genome, different outcomes - proves fitness is environmental

---

## Troubleshooting

### Bot Dies Immediately

**Problem:** Energy drops to 0 within seconds, even in bright light

**Most likely cause:** Random `light_threshold` higher than available light

**Fix:**
```
threshold 0.3
reset
```

---

### LED Stuck on One Color

**Problem:** LED only shows red, green, or blue - never changes

**Most likely cause:** One GPIO pin disconnected or wrong pin number in code

**Fix:**
1. Check wiring: GPIO23=red, GPIO22=green, GPIO21=blue
2. Test each color individually via serial commands
3. Verify common cathode connected to GND

---

### Light Sensor Not Responding

**Problem:** Light reading stuck at 0.0 or 1.0 regardless of lighting

**Most likely cause:** Voltage divider wired incorrectly

**Fix:**
1. Verify circuit: 3.3V → LDR → GPIO34/35 → 10kΩ → GND
2. Measure voltage at GPIO with multimeter (should be 0-3.3V)
3. If always 0V: check GND connection
4. If always 3.3V: check resistor to GND exists

---

### Serial Monitor Gibberish

**Problem:** Serial output shows random characters

**Most likely cause:** Wrong baud rate

**Fix:**
1. Set Serial Monitor to 115200 baud
2. Press reset button on ESP32
3. Should see clean boot message

---

### Bot Resets Randomly

**Problem:** Bot reboots unexpectedly, loses energy count

**Most likely cause:** Battery voltage too low or loose power connection

**Fix:**
1. Charge batteries fully (should read 7.4-8.4V)
2. Check all GND connections secure
3. Add 100µF capacitor across buck converter output

---

## FAQ

### Q: Why only light? Why not other sensors?

**A:** v0.1 uses light to prove the concept works. The same code works with ANY sensor - just swap the `readLight()` function. Future versions will have multi-sensory organisms.

### Q: Why can't it move in v0.1?

**A:** Movement adds complexity. v0.1 focuses on the absolute minimum: sense → energy → survival. Movement comes in v0.2 once the basic pattern is proven.

### Q: How is this different from a light-seeking robot?

**A:** A light-seeking robot is pre-programmed to seek light. EMBER bots have random genes - some will happen to survive, others won't. We don't tell them what to do, we create selection pressure and let evolution find the solution.

### Q: Do I need 9 bots?

**A:** For meaningful evolution experiments, yes. You need genetic diversity (multiple genomes), selection (some live, some die), and breeding (copy winning genes). With fewer bots, there's less variation and slower convergence.

### Q: Can I run this in simulation?

**A:** You *could*, but the point is to prove emergence works in **physical reality**, not just in code. Simulations make assumptions. Hardware doesn't lie.

### Q: What if all my bots die?

**A:** Then your environment is too harsh for ANY genome in the current population. Either:
1. Add more light
2. Lower ENERGY_DECAY constant in code
3. Increase ENERGY_GAIN constant in code
4. Randomize all genomes again (wider search space)

### Q: What if all my bots survive forever?

**A:** Then your environment has no selection pressure. Either:
1. Reduce light intensity
2. Create variable light (shade part of arena)
3. Increase ENERGY_DECAY constant
4. Proceed to v0.2 (movement + competition)

### Q: How long does a generation take?

**A:** Depends on selection pressure:
- High pressure (many deaths): 10-30 minutes
- Low pressure (slow energy drain): 1-2 hours
- No pressure (all survive): infinite (need to change environment)

### Q: What happens at Generation 100?

**A:** Population will be highly adapted to your specific environment. Changing lighting even slightly will cause deaths - they've specialized. This is **niche adaptation** emerging from simple rules.

### Q: Can I cross-breed bots?

**A:** Not in v0.1 (you manually copy genes via serial). v0.4 will have ESP-NOW communication for automatic gene sharing. But you can simulate it:
1. Take `threshold` from Bot A
2. Take `efficiency` from Bot B
3. Set new bot to combined genome
4. This is **genetic crossover**

### Q: Is this actually alive?

**A:** Philosophically? That's your call. Scientifically, it exhibits:
- Homeostasis (energy regulation)
- Metabolism (light → energy conversion)
- Growth (energy accumulation)
- Response to stimuli (phototaxis in v0.2+)
- Reproduction (copy genes to offspring)
- Evolution (adaptation over generations)

**It follows the same life equation as bacteria, plants, and you.**

---

## What You Can Learn From This

### For Students

- **Biology:** Evolution by natural selection (real, not simulated)
- **Physics:** Energy conservation, photovoltaic effect, thermodynamics
- **Computer Science:** Genetic algorithms, emergence, state machines
- **Engineering:** Embedded systems, sensor integration, power management
- **Philosophy:** What is life? What is intelligence? What is consciousness?

### For Researchers

- **Validation:** Does emergence work in hardware? (Answer: Yes)
- **Platform:** Test evolutionary theories with physical agents
- **Experiments:** Multi-sensory organisms, specialization, swarm intelligence
- **Extensions:** Add more sensors, more genes, more complexity - pattern holds

### For Makers

- **Framework:** Reusable life template for any sensor type
- **Learning:** Practical embedded systems project with deep concepts
- **Extension:** Build on this - add behaviors, communication, memory
- **Philosophy:** Anti-gatekeeping - all designs MIT licensed, share freely

---

## Next Steps

### After You Build One Bot

1. **Test basic functions** - verify sensing, energy, life/death works
2. **Try serial commands** - mutate, reset, observe changes
3. **Run survival tests** - different lighting, different thresholds
4. **Document your findings** - which genomes work in your environment

### After You Build Nine Bots

1. **Read EVOLUTION_GUIDE.md** - comprehensive evolution protocol
2. **Create test arena** - controlled environment with adjustable light
3. **Run generation 0** - random genomes, measure fitness
4. **Select and breed** - copy winners, mutate, repeat
5. **Track lineages** - which bots descended from which survivors
6. **Analyze results** - what genome emerged as optimal?

### After Evolution Works

1. **Modify environment** - change light intensity, observe re-adaptation
2. **Add constraints** - limited "food" (light spots), force competition
3. **Cross environments** - move adapted bots to new arena, watch them struggle
4. **Build v0.2** - enable movement, watch phototaxis emerge
5. **Publish results** - share your data, help build knowledge

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
- Document your experiments (PRs welcome)
- Share your modifications (especially sensor swaps)
- Report issues (GitHub issues)
- Improve documentation (always needed)
- Build tools (data logging, analysis, visualization)

**No permission needed. Just build and share.**

---

## Project Philosophy

### Build to Understand

EMBER isn't a simulation. It's physical hardware following real physics. The goal is to **prove emergence works in reality**, not just in theory.

You can't handwave thermodynamics. You can't skip circuit design. You can't pretend about battery voltage.

**Physical constraints force honesty.**

### Simple Rules, Complex Behavior

EMBER demonstrates the **Mavric Pattern** (three-layer emergence):

**Layer 1: Physics**
- Photons → voltage
- Transistors → current
- Battery → chemical energy

**Layer 2: Life**
- Sensor → perception
- Energy → survival
- Death → selection

**Layer 3: Evolution**
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
- `EMBER_v0.1_SPEC.md` - Technical specifications
- `EMBER_BUILD_GUIDE.md` - Assembly instructions
- `EMBER_EVOLUTION_GUIDE.md` - How to run experiments
- `EMBER_ROADMAP.md` - Future versions

### Code
- `ember_v0.1_light.ino` - Main firmware
- `/tools/` - Helper scripts for logging, analysis, batch flashing

### Hardware
- `/hardware/circuit_diagram.png` - Wiring schematic
- `/hardware/parts_list.md` - Component sourcing
- `/hardware/chassis_design.dxf` - Laser-cut chassis (optional)

### Community
- GitHub Issues - bug reports, feature requests
- Discussions - share experiments, ask questions
- Pull Requests - contribute improvements

---

## License

**MIT License** - Build freely, share openly
```
Copyright (c) 2024 James @ Giblets Creations

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

Build nine. Watch evolution happen in real-time.

Change the environment. Watch them adapt.

Swap the sensor. Watch the same pattern work with different resources.

**This is emergence. This is evolution. This is life.**

Not programmed. Not scripted. Not faked.

**Emergent.**

From simple rules. In silicon and photons.

---

**Ready?**

Flash the code. Connect the battery. Place it in light.

**Watch what emerges.**

---

*EMBER v0.1 - Life From Light*  
*Created 2024*  
*Part of the Forge Theory Project*  
*gibletscreations.com | shaped-maker-3d @ cults3d*
