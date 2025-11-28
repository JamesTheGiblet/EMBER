# EMBER Evolution Experiment Log: [Your Experiment Title]

**Date:** YYYY-MM-DD
**Author:** [Your Name]

---

## 1. Experiment Setup

### Goal

*A clear, one-sentence goal for this experiment. Example: "To evolve a genome adapted to a dim, constant light environment."*

### Environment

- **Light Source:** (e.g., 60W desk lamp, natural window light, LED panel)
- **Light Intensity:** (e.g., ~400 lux, measured with a phone app)
- **Light Position:** (e.g., 50cm directly above the center of the arena)
- **Arena:** (e.g., 60cm x 60cm white foam board with black walls)

### Population

- **Number of Bots:** (e.g., 9 bots, IDs 0-8)
- **Initial State:** All bots starting with new random genomes.

### Methodology

- **Generation Time:** (e.g., 1800 seconds / 30 minutes)
- **Selection Pressure:** (e.g., Top 33% - the 3 fittest bots are selected as parents)
- **Breeding Strategy:** (e.g., Crossover - threshold from parent A, efficiency from parent B)
- **Mutation Rate:** (e.g., Default ±10% via `mutate` command)

---

## 2. Generation 0: Random Start

### Initial Genomes

*Record the starting genomes after randomizing all bots.*

| Bot ID | Light Threshold | Efficiency |
|:------:|:---------------:|:----------:|
| 0      | 0.XXX           | 1.XXX      |
| 1      | 0.XXX           | 1.XXX      |
| 2      | 0.XXX           | 1.XXX      |
| 3      | 0.XXX           | 1.XXX      |
| 4      | 0.XXX           | 1.XXX      |
| 5      | 0.XXX           | 1.XXX      |
| 6      | 0.XXX           | 1.XXX      |
| 7      | 0.XXX           | 1.XXX      |
| 8      | 0.XXX           | 1.XXX      |

### Predictions

*What do you expect to happen? Example: "Bots with a light_threshold below the ambient light level will survive. Bots with high efficiency will have an advantage."*

### Fitness Results

| Bot ID | Survival Time (s) | Status |
|:------:|:-----------------:|:------:|
| 0      |                   |        |
| 1      |                   |        |
| 2      |                   |        |
| 3      |                   |        |
| 4      |                   |        |
| 5      |                   |        |
| 6      |                   |        |
| 7      |                   |        |
| 8      |                   |        |

**Observations:** *Note any interesting behaviors. Which bots died first? Which thrived?*

---

## 3. Generation 1

### Selection & Breeding

- **Winners (Parents):** Bot #X, Bot #Y, Bot #Z
- **Breeding Notes:** *Describe how you created the new genomes. Example: "Copied genomes from winners to losers, then mutated all."*

### New Genomes

| Bot ID | Light Threshold | Efficiency |
|:------:|:---------------:|:----------:|
| 0      | 0.XXX           | 1.XXX      |
| ...    | ...             | ...        |

### Fitness Results

| Bot ID | Survival Time (s) | Status |
|:------:|:-----------------:|:------:|
| 0      |                   |        |
| ...    | ...               | ...    |

**Observations:** *Did average fitness increase? Is there less variation in survival times?*

---

## 4. Generation 2

*(Copy the template from Generation 1 and continue for each generation...)*

---

## 5. Final Results & Conclusion

### Final Adapted Genomes (Generation X)

| Bot ID | Light Threshold | Efficiency |
|:------:|:---------------:|:----------:|
| 0      | 0.XXX           | 1.XXX      |
| ...    | ...             | ...        |

### Summary of Adaptation

- **Initial Avg. Fitness (Gen 0):** XXX seconds
- **Final Avg. Fitness (Gen X):** YYY seconds
- **Fitness Improvement:** Z%
- **Initial Avg. Threshold:** 0.XXX
- **Final Avg. Threshold:** 0.YYY

### Conclusion

*What did you learn? Did the population adapt as predicted? What was the optimal genome that emerged for this environment? What were the key selective pressures?*

### Raw Data

- **Log File:** `[Link to evolution_log.csv]`
- **Analysis Plots:** `[Link to evolution_analysis.png]`
