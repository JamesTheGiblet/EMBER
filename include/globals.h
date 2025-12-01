#pragma once

#include <stdint.h>
#include "config.h" // For BehaviorState_t

// WiFi check timer
extern unsigned long last_wifi_check_ms;
extern const unsigned long WIFI_CHECK_INTERVAL_MS;

// Life state
extern bool is_alive;
extern float energy;
extern unsigned long alive_time_ms;
extern unsigned long boot_time_ms;
extern bool manual_override;

// Life parameters that can be configured at runtime
typedef struct {
    float energy_decay;
    float energy_gain;
    float movement_cost_multiplier;
} LifeParams_t;
extern LifeParams_t life_params;

// Behavior state enum
typedef enum {
    IDLE,
    SEEKING_LIGHT,
    AVOIDING_OBSTACLE
} BehaviorState_t;

extern BehaviorState_t currentBehavior;

// Power management
typedef enum {
    POWER_NORMAL = 0,
    POWER_ECONOMY,
    POWER_LOW,
    POWER_CRITICAL,
    POWER_SHUTDOWN,
    POWER_USB_DEBUG
} PowerMode_t;

typedef struct {
    float voltage;
    float percentage;
    PowerMode_t mode;
    uint32_t lastUpdate;
} BatteryMonitor_t;
extern BatteryMonitor_t battery;
