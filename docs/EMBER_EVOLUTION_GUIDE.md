# EMBER v0.1 Evolution Guide

**Running Natural Selection Experiments with Networked Artificial Life**

---

## What This Guide Covers

This guide teaches you how to run evolution experiments with EMBER bots. You'll learn:

1. How to set up a controlled environment
2. How to measure fitness accurately
3. How to select and breed winners
4. How to track lineages across generations
5. How to use network features for efficient experiments
6. How to analyze results and understand emergence

**By the end, you'll watch artificial life adapt to your specific environment through natural selection.**

---

## Prerequisites

Before starting evolution experiments, ensure:

```
✓ You have built at least 3 bots (9 recommended)
✓ All bots are functional (pass hardware tests)
✓ All bots have unique bot_id (0-8) and hostnames
✓ WiFi is configured (optional but highly recommended)
✓ Web dashboards are accessible (test each bot)
✓ Genomes save/load correctly (test with reboot)
✓ You have a controlled environment (arena with light source)
✓ You have a way to log data (notebook, spreadsheet, or API script)
```

**Minimum:** 3 bots for basic selection  
**Recommended:** 9 bots for meaningful diversity  
**Optimal:** 20+ bots for complex dynamics

---

## Understanding Evolution

### The Four Requirements

For evolution to occur, you need:

1. **Variation** - Different genomes (random or mutated)
2. **Selection** - Environment favors some genomes over others
3. **Inheritance** - Successful genes copied to next generation
4. **Mutation** - Small random changes create new variation

**EMBER provides all four automatically.**

### What You'll Observe

**Generation 0 (Random):**

- Wide variation in survival times
- Some bots die quickly, some survive long
- No clear pattern yet

**Generations 1-5 (Selection):**

- Average fitness increases
- Less variation (converging on solution)
- Dominant genome emerges

**Generations 10+ (Adapted):**

- Population optimized for environment
- Minimal variation (all similar genomes)
- Changing environment causes mass death (over-specialization)

**This is exactly how real evolution works.**

---

## Evolution Experiment Types

### Type 1: Basic Selection (Beginner)

**Goal:** Demonstrate natural selection in 5 generations

**Setup:**

- 3-9 bots with random genomes
- Single constant light source
- 30-minute generations

**Method:**

1. Generation 0: Randomize all genomes
2. Run experiment, record survival times
3. Copy best genome to all bots
4. Add small mutations
5. Repeat

**Expected Result:** Convergence on optimal threshold for your light level

**Time Required:** 2-3 hours

---

### Type 2: Environmental Adaptation (Intermediate)

**Goal:** Show same genome fails in different environments

**Setup:**

- 9 bots, single evolved genome
- Three different light environments
- 20-minute tests per environment

**Method:**

1. Evolve bots to bright environment (5 generations)
2. Test evolved genome in dim environment → expect deaths
3. Test evolved genome in variable environment → expect struggle
4. Re-evolve in dim environment → different optimal genome

**Expected Result:** Different environments select different genomes

**Time Required:** 4-6 hours

---

### Type 3: Niche Partitioning (Advanced)

**Goal:** Observe specialization and generalists emerging

**Setup:**

- 9 bots
- Arena with multiple light zones (bright, medium, dim)
- 60-minute generations

**Method:**

1. Generation 0: Random genomes in mixed environment
2. Let bots "find" their optimal zones naturally
3. Record which zones each genome survives in
4. Breed separately by zone
5. Test if specialists emerged (optimized for one zone) vs generalists (survive everywhere)

**Expected Result:** Specialists in uniform zones, generalists in variable zones

**Time Required:** 6-8 hours

---

### Type 4: Long-Term Evolution (Expert)

**Goal:** Track evolution over 50+ generations, observe drift and speciation

**Setup:**

- 20+ bots (or run multiple cohorts)
- Constant environment
- Automated data logging via JSON API
- 20-minute generations

**Method:**

1. Set up automated logging script (see below)
2. Run generations continuously
3. Track genetic changes over time
4. Look for convergence, drift, mutation accumulation
5. Introduce environmental change at generation 30
6. Observe re-adaptation

**Expected Result:** Complex evolutionary dynamics, lineage trees, adaptation/re-adaptation cycles

**Time Required:** 2-3 days (mostly automated)

---

## Setting Up Your Arena

### Basic Arena (3-9 Bots)

```
┌─────────────────────────────────────┐
│                                     │
│         [Desk Lamp]                 │
│              │                      │
│              ↓                      │
│         ┌─────────┐                 │
│         │ ● ● ● ● │  ← 9 bots      │
│         │ ● ● ● ● │  ← evenly      │
│         │   ●     │  ← spaced      │
│         └─────────┘                 │
│                                     │
│  [Flat surface, controlled light]  │
└─────────────────────────────────────┘
```

**Requirements:**

- Consistent light source (desk lamp, LED panel)
- Flat, stable surface
- No shadows or obstructions on LDRs
- Minimal ambient light interference
- WiFi coverage (if using network features)

### Advanced Arena (Multi-Zone)

```
┌─────────────────────────────────────┐
│ [Bright Zone]  [Medium]  [Dim Zone] │
│   ■■■■■■■       ■■■■       ■■       │
│   ● ● ●         ● ●         ● ●     │
│   Bots 0-2      3-4         5-6     │
│                                     │
│   [Variable Zone: Window Light]     │
│         ■■■■■■■■                    │
│         ● ●                         │
│         Bots 7-8                    │
└─────────────────────────────────────┘
```

**Enables:**

- Niche partitioning experiments
- Specialist vs generalist comparison
- Complex fitness landscapes

---

## Running an Evolution Experiment: Two Methods

There are two ways to run an evolution experiment with EMBER:

1. **Method 1: Fully Automated (Recommended)**
    - Uses the `evolution_experiment.py` script.
    - The script handles everything: running generations, logging data, selecting winners, breeding, and mutating.
    - **Best for:** Serious experiments, long-term studies, and hands-off operation.

2. **Method 2: Manual / Semi-Automated**
    - You perform each step of the cycle manually using the web dashboards or other scripts.
    - **Best for:** Educational purposes, understanding the process step-by-step, and short, controlled tests.

---

## Method 1: Fully Automated Experiment (Recommended)

The `tools/evolution_experiment.py` script is the most powerful way to run experiments. It automates the entire evolutionary loop, allowing you to run multi-generation experiments for hours or days without any manual intervention.

### How it Works

The script performs the following loop for each generation:

1. **Resets** all bots to 100% energy.
2. **Runs the generation** for a specified amount of time, printing live status updates.
3. **Logs Fitness:** At the end of the generation, it queries each bot's API and records its final fitness (`alive_time`) and genome into a CSV log file.
4. **Selects Winners:** It identifies the top-performing bots based on survival time.
5. **Breeds:** It creates the next generation by performing a genetic crossover using the winners as parents.
6. **Mutates:** It applies a small random mutation to every bot in the new population.
7. **Repeats** for the specified number of generations.

### How to Use

1. **Ensure all bots are online** and connected to the same WiFi network as your computer.
2. Open a terminal and navigate to the `tools/` directory.
3. Run the script with your desired parameters.

**To run a default experiment** (10 generations, 30 minutes each, 9 bots):

```bash
python evolution_experiment.py
```

**To run a shorter test experiment** (3 generations, 5 minutes each, for a swarm of 4 bots):

```bash
python evolution_experiment.py --generations 3 --time 300 --count 4
```

### Command-Line Arguments

| Argument | Default | Description |
|:---|:---|:---|
| `--generations` | 10 | The total number of generations to run. |
| `--time` | 1800 | The duration of each generation in seconds (1800s = 30 min). |
| `--count` | 9 | The number of bots in your population (e.g., for `ember-bot-0` to `ember-bot-8`). |
| `--pressure` | 0.33 | The selection pressure (e.g., 0.33 means the top 33% are selected as parents). |
| `--log-file` | `evolution_log_[timestamp].csv` | The name of the CSV file where results will be saved. |
| `--host` | `ember-bot` | The base hostname for your bots. |

### What to Expect

The script will print detailed updates at every stage. You can leave it running and come back later. When it's finished, you will have a comprehensive `evolution_log.csv` file in the `tools/` directory.

### Next Step: Analyze the Data

After the experiment is complete, use the `genome_analyzer.py` script to visualize the results:

```bash
python genome_analyzer.py evolution_log_20251128_231045.csv
```

---

## Method 2: Manual / Semi-Automated Experiment

This method is excellent for learning the evolutionary process step-by-step. You will act as the "hand of god," performing the selection and breeding manually.

### Step 1: Prepare the Bots

**Randomize all genomes:**

**Option A: Via Serial (each bot individually)**

```
> randomize
New random genome created and saved!
=================================
Bot ID: 0
Generation: 0
Light Threshold: 0.423
Efficiency: 1.087
=================================
```

**Option B: Via Web Interface (faster for multiple bots)**

1. Open browser tabs to all 9 bots
2. Click "Randomize" button on each
3. Verify different genomes (check dashboards)

**Option C: Via Script (fastest)**

```python
import requests

for i in range(9):
    url = f"http://ember-bot-{i}.local/randomize"
    requests.get(url)
    print(f"Bot {i} randomized")
```

**Result:** All bots have random, unique genomes saved to flash

---

### Step 2: Record Initial State

**Create a log file:** `experiment_001.md`

```markdown
# Evolution Experiment 001
## Basic Selection in Constant Light

**Date:** 2024-11-28
**Environment:** Desk lamp, ~60cm above bots, 800 lumens
**Population:** 9 bots (IDs 0-8)
**Goal:** Demonstrate convergence over 5 generations

### Generation 0 - Random Genomes

| Bot ID | Threshold | Efficiency | Notes |
|--------|-----------|------------|-------|
| 0 | 0.423 | 1.087 | |
| 1 | 0.678 | 0.912 | |
| 2 | 0.234 | 1.234 | |
| 3 | 0.567 | 0.789 | |
| 4 | 0.345 | 1.123 | |
| 5 | 0.789 | 0.654 | |
| 6 | 0.123 | 1.345 | |
| 7 | 0.890 | 0.567 | |
| 8 | 0.456 | 1.001 | |

**Predictions:**
- Bots 1, 5, 7 will die first (high threshold)
- Bots 2, 6 will survive longest (low threshold)
```

**Or use automated logging:**

```python
import requests
import csv
from datetime import datetime

with open('gen_0_initial.csv', 'w') as f:
    writer = csv.writer(f)
    writer.writerow(['bot_id', 'threshold', 'efficiency', 'generation'])
    
    for i in range(9):
        data = requests.get(f'http://ember-bot-{i}.local/api/stats').json()
        writer.writerow([data['bot_id'], data['threshold'], 
                        data['efficiency'], data['generation']])
```

---

### Step 3: Run Generation 0

**Start the experiment:**

1. **Reset all bots to full energy:**
   - Web: Click "Reset Life" on all dashboards
   - Serial: Send `reset` to each bot
   - Script: `requests.get(f'http://ember-bot-{i}.local/reset')`

2. **Start timer**

3. **Monitor via web dashboards** (open all 9 in browser tabs)
   - Dashboards auto-refresh every 2 seconds
   - Watch energy bars rise/fall
   - Note which LEDs turn red first

4. **Or monitor via API script:**

```python
   import requests
   import time
   
   while True:
       for i in range(9):
           data = requests.get(f'http://ember-bot-{i}.local/api/stats').json()
           status = "ALIVE" if data['alive'] else "DEAD"
           print(f"Bot {i}: {status:5} Energy={data['energy']:5.1f}% Time={data['alive_time']:4}s")
       print("-" * 60)
       time.sleep(5)
```

5. **Wait for deaths or time limit**
   - Option A: Run until 50% die (adaptive)
   - Option B: Run for fixed time (30 min)
   - Option C: Run until all stable (energy not changing)

---

### Step 4: Record Fitness Data

**After generation completes, record survival times:**

**Via Web Dashboards:**

- Note "Alive Time" from each dashboard
- Dead bots show time of death
- Living bots show current time (ongoing)

**Via JSON API:**

```python
import requests
import csv

with open('gen_0_fitness.csv', 'w') as f:
    writer = csv.writer(f)
    writer.writerow(['bot_id', 'alive_time', 'alive', 'threshold', 'efficiency'])
    
    for i in range(9):
        data = requests.get(f'http://ember-bot-{i}.local/api/stats').json()
        writer.writerow([data['bot_id'], data['alive_time'], data['alive'],
                        data['threshold'], data['efficiency']])
```

**Example results:**

```
Bot 0: 1243s ALIVE  (threshold=0.423, efficiency=1.087)
Bot 1: 234s  DEAD   (threshold=0.678, efficiency=0.912) ← High threshold
Bot 2: 1800s ALIVE  (threshold=0.234, efficiency=1.234) ← Low threshold, HIGH FITNESS
Bot 3: 891s  ALIVE  (threshold=0.567, efficiency=0.789)
Bot 4: 1456s ALIVE  (threshold=0.345, efficiency=1.123)
Bot 5: 123s  DEAD   (threshold=0.789, efficiency=0.654) ← High threshold
Bot 6: 1800s ALIVE  (threshold=0.123, efficiency=1.345) ← Low threshold, HIGH FITNESS
Bot 7: 67s   DEAD   (threshold=0.890, efficiency=0.567) ← Highest threshold
Bot 8: 1112s ALIVE  (threshold=0.456, efficiency=1.001)
```

**Analysis:**

- Winners: Bots 2, 6 (low threshold, high efficiency)
- Losers: Bots 1, 5, 7 (high threshold)
- **Pattern:** Threshold < 0.5 correlates with survival

---

### Step 5: Selection (Choose Winners)

**Identify top performers:**

**Manual method:**

1. Sort bots by alive_time
2. Select top 3 (or top 33%)
3. Note their genomes

**Automated method:**

```python
import requests

# Get all bot data
bots = []
for i in range(9):
    data = requests.get(f'http://ember-bot-{i}.local/api/stats').json()
    bots.append(data)

# Sort by alive_time (fitness)
bots.sort(key=lambda x: x['alive_time'], reverse=True)

# Top 3 winners
winners = bots[:3]
print("Winners:")
for bot in winners:
    print(f"  Bot {bot['bot_id']}: {bot['alive_time']}s (threshold={bot['threshold']:.3f}, eff={bot['efficiency']:.3f})")
```

**Example winners:**

```
Winners:
  Bot 2: 1800s (threshold=0.234, efficiency=1.234)
  Bot 6: 1800s (threshold=0.123, efficiency=1.345)
  Bot 4: 1456s (threshold=0.345, efficiency=1.123)
```

---

### Step 6: Breeding (Copy Winning Genes)

**Copy winner genomes to losers:**

**Option A: Simple Cloning (all get same genome)**

```python
# Copy Bot 2's genome to all others
winner_genome = {
    'threshold': 0.234,
    'efficiency': 1.234
}

for i in range(9):
    if i != 2:  # Don't overwrite winner
        requests.get(f'http://ember-bot-{i}.local/threshold', 
                    params={'value': winner_genome['threshold']})
        requests.get(f'http://ember-bot-{i}.local/efficiency',
                    params={'value': winner_genome['efficiency']})
        requests.get(f'http://ember-bot-{i}.local/save')  # Save to flash
```

**Option B: Crossover Breeding (mix winner genes)**

```python
import random

# Winners
winner_genomes = [
    {'threshold': 0.234, 'efficiency': 1.234},  # Bot 2
    {'threshold': 0.123, 'efficiency': 1.345},  # Bot 6
    {'threshold': 0.345, 'efficiency': 1.123},  # Bot 4
]

# Assign to all bots
for i in range(9):
    # Pick random winner genes
    parent1 = random.choice(winner_genomes)
    parent2 = random.choice(winner_genomes)
    
    # Crossover (take threshold from one, efficiency from other)
    new_genome = {
        'threshold': parent1['threshold'],
        'efficiency': parent2['efficiency']
    }
    
    # Apply to bot
    requests.get(f'http://ember-bot-{i}.local/threshold',
                params={'value': new_genome['threshold']})
    requests.get(f'http://ember-bot-{i}.local/efficiency',
                params={'value': new_genome['efficiency']})
    requests.get(f'http://ember-bot-{i}.local/save')
```

**Option C: Via Web Interface (manual)**

1. Note winner genomes from dashboards
2. For each loser bot:
   - Navigate to dashboard
   - Manually set threshold/efficiency values
   - Click "Save Genome"

---

### Step 7: Mutation (Add Variation)

**Apply small random changes:**

**Via Web Interface:**

- Click "Mutate" button on each dashboard
- Genome changes by ±10% automatically
- Auto-saves to flash

**Via Serial:**

```
> mutate
Genome mutated and saved!
```

**Via Script:**

```python
for i in range(9):
    requests.get(f'http://ember-bot-{i}.local/mutate')
    print(f"Bot {i} mutated")
```

**Result:** Generation 1 has genomes similar to winners but with small variations

---

### Step 8: Repeat for Generation 1

1. Record new genomes (after mutation)
2. Reset all bots to full energy
3. Run experiment
4. Record fitness
5. Select winners
6. Breed
7. Mutate
8. **Repeat**

**After 5 generations, you should see:**

- Increasing average fitness
- Decreasing variation (convergence)
- Optimal genome for your environment emerges

---

## Analyzing Results

### Basic Analysis (Manual)

**Look for these patterns in your log:**

1. **Increasing Average Fitness**

```
   Gen 0: Avg survival = 687s
   Gen 1: Avg survival = 943s
   Gen 2: Avg survival = 1234s
   Gen 3: Avg survival = 1456s
   Gen 4: Avg survival = 1589s
```

   **Meaning:** Population is adapting

2. **Decreasing Variation**

```
   Gen 0: Genomes range 0.123-0.890 (wide)
   Gen 5: Genomes range 0.234-0.298 (narrow)
```

   **Meaning:** Convergence on optimal genome

3. **Threshold Convergence**

```
   Gen 0: Average threshold = 0.512
   Gen 5: Average threshold = 0.267
```

   **Meaning:** Optimal threshold for your light level is ~0.27

### Advanced Analysis (Python)

```python
import pandas as pd
import matplotlib.pyplot as plt

# Load data
df = pd.read_csv('evolution_log.csv')

# Plot fitness over generations
plt.figure(figsize=(12, 8))

# Subplot 1: Average fitness per generation
plt.subplot(2, 2, 1)
avg_fitness = df.groupby('generation')['alive_time'].mean()
plt.plot(avg_fitness.index, avg_fitness.values, marker='o')
plt.xlabel('Generation')
plt.ylabel('Average Survival Time (s)')
plt.title('Average Fitness Over Time')
plt.grid(True)

# Subplot 2: Fitness variance
plt.subplot(2, 2, 2)
fitness_std = df.groupby('generation')['alive_time'].std()
plt.plot(fitness_std.index, fitness_std.values, marker='o', color='orange')
plt.xlabel('Generation')
plt.ylabel('Std Dev of Survival Time (s)')
plt.title('Genetic Diversity Over Time')
plt.grid(True)

# Subplot 3: Threshold evolution
plt.subplot(2, 2, 3)
for bot_id in range(9):
    bot_data = df[df['bot_id'] == bot_id]
    plt.plot(bot_data['generation'], bot_data['threshold'], 
             marker='o', label=f'Bot {bot_id}', alpha=0.7)
plt.xlabel('Generation')
plt.ylabel('Light Threshold')
plt.title('Threshold Evolution by Bot')
plt.legend(bbox_to_anchor=(1.05, 1), loc='upper left')
plt.grid(True)

# Subplot 4: Efficiency evolution
plt.subplot(2, 2, 4)
for bot_id in range(9):
    bot_data = df[df['bot_id'] == bot_id]
    plt.plot(bot_data['generation'], bot_data['efficiency'],
             marker='o', label=f'Bot {bot_id}', alpha=0.7)
plt.xlabel('Generation')
plt.ylabel('Efficiency')
plt.title('Efficiency Evolution by Bot')
plt.legend(bbox_to_anchor=(1.05, 1), loc='upper left')
plt.grid(True)

plt.tight_layout()
plt.savefig('evolution_analysis.png', dpi=300, bbox_inches='tight')
plt.show()

# Statistical summary
print("\n" + "="*60)
print("EVOLUTION SUMMARY")
print("="*60)
print(f"\nGeneration 0:")
gen0 = df[df['generation'] == 0]
print(f"  Mean fitness: {gen0['alive_time'].mean():.1f}s")
print(f"  Std dev: {gen0['alive_time'].std():.1f}s")
print(f"  Mean threshold: {gen0['threshold'].mean():.3f}")

final_gen = df['generation'].max()
print(f"\nGeneration {final_gen}:")
genF = df[df['generation'] == final_gen]
print(f"  Mean fitness: {genF['alive_time'].mean():.1f}s")
print(f"  Std dev: {genF['alive_time'].std():.1f}s")
print(f"  Mean threshold: {genF['threshold'].mean():.3f}")

improvement = ((genF['alive_time'].mean() - gen0['alive_time'].mean()) / 
               gen0['alive_time'].mean() * 100)
print(f"\nFitness Improvement: {improvement:.1f}%")
print("="*60)
```

**This produces:**

- Graphs showing fitness increase
- Visualization of convergence
- Statistical proof of adaptation

---

## Common Patterns and What They Mean

### Pattern 1: Rapid Convergence (Generations 1-3)

**What you see:**

- Gen 0: Wide variation
- Gen 1-3: Narrow convergence
- Gen 4+: Almost identical genomes

**Meaning:**

- Strong selection pressure
- Clear optimal solution
- Environment is stable and simple

**What to try:**

- Increase environment complexity
- Add variable light
- Introduce competition

---

### Pattern 2: Slow Convergence (10+ generations)

**What you see:**

- Fitness improving slowly
- Variation remains high
- No clear winner emerges

**Meaning:**

- Weak selection pressure
- Multiple viable strategies
- Environment is complex or variable

**What to try:**

- Increase selection pressure (harsher environment)
- Longer generation times
- More extreme fitness differences

---

### Pattern 3: Convergence Then Divergence

**What you see:**

- Gen 0-5: Convergence
- Gen 6-10: Variation increases again

**Meaning:**

- Optimal genome found
- Mutation creating variation faster than selection removes it
- Population may be "exploring" fitness landscape

**What to try:**

- Reduce mutation rate
- Increase selection pressure
- Check for environmental changes

---

### Pattern 4: No Improvement

**What you see:**

- Fitness stays constant across generations
- Random variation in genomes

**Meaning:**

- No selection pressure (all genomes survive equally)
- Environment too easy or too hard

**What to try:**

- Adjust light intensity
- Reduce ENERGY_GAIN or increase ENERGY_DECAY in code
- Create variable environment

---

## Advanced Techniques

### Technique 1: Lineage Tracking

**Track which bots descended from which:**

```python
# Add to breeding function
lineages = {i: [i] for i in range(NUM_BOTS)}  # Track ancestry

def breed_with_lineage(generation, winners, lineages):
    for i in range(NUM_BOTS):
        parent = random.choice(winners)
        parent_id = parent['bot_id']
        
        # Copy genome
        # ... (same as before)
        
        # Track lineage
        lineages[i].append(parent_id)
        
    return lineages

# After experiment, visualize lineages
import matplotlib.pyplot as plt
import numpy as np

plt.figure(figsize=(14, 8))
for bot_id, ancestors in lineages.items():
    plt.plot(range(len(ancestors)), ancestors, marker='o', label=f'Bot {bot_id}')

plt.xlabel('Generation')
plt.ylabel('Parent Bot ID')
plt.title('Lineage Tree - Who Descended From Whom')
plt.legend()
plt.grid(True)
plt.savefig('lineage_tree.png')
```

**Result:** Visual tree showing which bots' genes dominated

---

### Technique 2: Fitness Landscape Mapping

**Map which genomes survive in which conditions:**

```python
import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

# Test grid of genomes
thresholds = np.linspace(0.1, 0.9, 20)
efficiencies = np.linspace(0.5, 1.5, 20)

fitness_map = np.zeros((20, 20))

for i, thresh in enumerate(thresholds):
    for j, eff in enumerate(efficiencies):
        # Set genome
        requests.get(f'http://ember-bot-0.local/threshold', params={'value': thresh})
        requests.get(f'http://ember-bot-0.local/efficiency', params={'value': eff})
        requests.get(f'http://ember-bot-0.local/reset')
        
        # Run for 5 minutes
        time.sleep(300)
        
        # Get fitness
        data = get_bot_data(0)
        fitness_map[i, j] = data['alive_time']
        
        print(f"Tested threshold={thresh:.2f}, efficiency={eff:.2f}, fitness={data['alive_time']}s")

# Visualize
fig = plt.figure(figsize=(12, 8))
ax = fig.add_subplot(111, projection='3d')
T, E = np.meshgrid(thresholds, efficiencies)
ax.plot_surface(T, E, fitness_map.T, cmap='viridis')
ax.set_xlabel('Threshold')
ax.set_ylabel('Efficiency')
ax.set_zlabel('Fitness (seconds)')
ax.set_title('Fitness Landscape')
plt.savefig('fitness_landscape.png')
```

**Result:** 3D map showing fitness peaks and valleys

---

### Technique 3: Environmental Shift Experiment

**Test re-adaptation after environment changes:**

```python
def run_shift_experiment():
    # Phase 1: Evolve in bright light (30 min generations)
    print("Phase 1: Evolving in BRIGHT light...")
    for gen in range(10):
        run_generation(1800)  # 30 min
        select_and_breed(gen)
        mutate_all_bots()
    
    # Record adapted population
    bright_adapted = [get_bot_data(i) for i in range(NUM_BOTS)]
    
    # CHANGE ENVIRONMENT (reduce light intensity)
    input("Reduce light intensity to 50%, then press Enter...")
    
    # Phase 2: Continue evolution in dim light
    print("Phase 2: Re-adapting to DIM light...")
    for gen in range(10, 20):
        run_generation(1800)
        select_and_breed(gen)
        mutate_all_bots()
    
    # Record re-adapted population
    dim_adapted = [get_bot_data(i) for i in range(NUM_BOTS)]
    
    # Compare genomes
    print("\nGenome Comparison:")
    print(f"Bright-adapted avg threshold: {np.mean([b['threshold'] for b in bright_adapted]):.3f}")
    print(f"Dim-adapted avg threshold: {np.mean([b['threshold'] for b in dim_adapted]):.3f}")
```

**Expected Result:** Different optimal genomes for different environments

---

## Troubleshooting Evolution Experiments

### Problem: No Selection Occurring

**Symptoms:**

- All bots survive every generation
- No fitness difference
- Random variation persists

**Causes:**

- Environment too easy (lots of light)
- ENERGY_DECAY too low
- Generation time too short

**Solutions:**

1. Reduce light intensity
2. Increase ENERGY_DECAY in code
3. Run longer generations
4. Make environment variable

---

### Problem: All Bots Die Every Generation

**Symptoms:**

- No survivors
- Can't breed next generation
- Experiment stalls

**Causes:**

- Environment too harsh (not enough light)
- All genomes unsuitable
- Generation time too long

**Solutions:**

1. Increase light intensity
2. Reduce ENERGY_DECAY or increase ENERGY_GAIN
3. Manually set some bots to viable genomes
4. Shorten generation time

---

### Problem: Network Issues During Experiment

**Symptoms:**

- Can't access web dashboards
- API calls failing
- Script crashes

**Solutions:**

1. Check WiFi connectivity (all bots show blue flash?)
2. Use IP addresses instead of .local hostnames
3. Add error handling to scripts:

```python
   try:
       data = get_bot_data(i)
   except Exception as e:
       print(f"Bot {i} unreachable: {e}")
       continue  # Skip this bot
```

4. Fall back to serial monitoring via USB

---

### Problem: Genomes Not Saving

**Symptoms:**

- After reboot, genomes reset to defaults
- Breeding doesn't persist

**Solutions:**

1. Use `save` command or web "Save" button after changes
2. Verify Serial shows "[Storage] Genome saved"
3. Check NVS partition not full (unlikely)
4. Use `clear` then `save` if corruption suspected

---

## Publishing Your Results

### What to Document

1. **Experimental Setup**
   - Number of bots
   - Environment description (light source, intensity)
   - Generation time
   - Selection method

2. **Initial Conditions**
   - Generation 0 genomes
   - Environmental parameters

3. **Results**
   - Fitness data (CSV or graphs)
   - Final adapted genomes
   - Number of generations to convergence

4. **Analysis**
   - Fitness improvement percentage
   - Genome convergence patterns
   - Unexpected observations

5. **Raw Data**
   - evolution_log.csv
   - Scripts used
   - Photos/videos of setup

### Sharing Format

```markdown
# EMBER Evolution Experiment: [Your Title]

## Summary
- **Bots:** 9
- **Generations:** 10
- **Environment:** Desk lamp, 800 lumens, 60cm above arena
- **Result:** 127% fitness improvement, convergence at generation 5

## Methodology
[Describe your method]

## Results
![Fitness Graph](fitness_over_time.png)

**Generation 0:**
- Mean fitness: 687s
- Mean threshold: 0.512

**Generation 10:**
- Mean fitness: 1560s
- Mean threshold: 0.267

**Improvement:** 127%

## Observations
[What you learned]

## Data
[Link to CSV files and scripts]
```

---

## Next Steps After Basic Evolution

### 1. Try Different Environments

- Bright vs dim
- Constant vs variable
- Multi-zone arenas

### 2. Modify Fitness Function

- Add distance traveled (v0.2+)
- Multi-objective fitness
- Time-weighted fitness

### 3. Increase Complexity

- More genes (movement parameters in v0.2+)
- Larger populations (20+ bots)
- Longer experiments (50+ generations)

### 4. Add Communication (v0.4+)

- Gene sharing between bots
- Cooperative behaviors
- Cultural evolution

### 5. Compare to Theory

- Do results match population genetics predictions?
- Test Hardy-Weinberg equilibrium
- Measure genetic drift

---

## Conclusion

**You now have everything you need to run evolution experiments with EMBER bots.**

Key takeaways:

- Evolution requires variation, selection, inheritance, mutation
- Network features enable efficient large-scale experiments
- Proper logging and analysis reveals emergence
- Real evolution is messy, noisy, and surprising
- **Your results will vary** - that's the point!

**The goal isn't to get specific numbers - it's to observe natural selection working in physical hardware.**

Start with basic selection (Type 1), then expand to more complex experiments as you gain experience.

**Watch artificial life evolve in your hands.**

---

*EMBER v0.1 Evolution Guide - HAL+OTA Edition*  
*Part of the Forge Theory Project*  
*MIT License - Share your results openly*
