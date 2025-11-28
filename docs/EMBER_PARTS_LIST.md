# EMBER v0.1 - Hardware Parts List

**Complete Component Guide for Building EMBER Bots**

This document provides a comprehensive list of all components required to build an EMBER bot, based on the **HAL+OTA Edition** architecture. It covers the minimum requirements for v0.1 (Stationary) and the recommended components for v0.2 (Mobile) readiness.

---

## 🏗️ Build Strategy

**Recommended approach:** Build with all components now (including motors and sensors), even though v0.1 code disables movement. This allows you to evolve your bot from v0.1 (Stationary Life) to v0.2 (Mobile Life) via an **Over-The-Air (OTA) software update** without disassembling the chassis.

---

## 💰 Cost Summary

| Build Tier | Components Only | Notes |
|------------|----------------|-------|
| **v0.1 Minimum** | ~$25 | Stationary, light sensing only |
| **v0.2 Ready (Recommended)** | ~$42 | Includes motors, wheels, and ultrasonic |
| **9-Bot Swarm** | ~$225 - $380 | For full evolution experiments |

---

## 🔌 Electronics Core

These components are required for the bot to function, think, and manage energy.

| Qty | Component | Specification | Notes |
|-----|-----------|---------------|-------|
| 1 | **ESP32 DevKit** | 30-pin version | **CRITICAL:** Must be the 30-pin version to match wiring guides. |
| 1 | **Buck Converter** | 7-12V In, 5V 3A Out | Steps down battery voltage to safe 5V for ESP32. **Must be adjusted to 5V before use.** |
| 2 | **18650 Battery** | 3.7V 2000mAh+ | Wired in series for ~7.4V total. High capacity recommended for WiFi features. |
| 1 | **Battery Holder** | 2-slot 18650 Series | Must connect batteries in series (7.4V), not parallel. |
| 1 | **Charging Board** | TP4056 Dual Cell | For recharging the LiPo batteries. |
| 1 | **Switch** | Toggle/Rocker | Optional but recommended for cutting power. |

---

## 👁️ Sensors & Output

These allow the bot to sense the environment and communicate its state.

| Qty | Component | Specification | Notes |
|-----|-----------|---------------|-------|
| 2 | **LDR** | GL5516 Photoresistor | The "eyes". 10kΩ-1MΩ resistance range. |
| 1 | **RGB LED** | 5mm Common Cathode | **CRITICAL:** Must be Common Cathode. Visualizes energy state and WiFi status. |
| 1 | **Ultrasonic** | HC-SR04 | Distance sensor. Wired in v0.1 but not active until v0.2 updates. |

---

## ⚙️ Locomotion (v0.2 Readiness)

These components enable movement. While disabled in v0.1 code, installing them now makes the bot "evolution ready."

| Qty | Component | Specification | Notes |
|-----|-----------|---------------|-------|
| 1 | **Motor Driver** | L9110S **OR** TB6612FNG | **Do not use L298N** (too bulky). The HAL software supports both listed drivers. |
| 2 | **TT Motors** | 3-6V DC Gearmotor | Standard "yellow" hobby motors. |
| 2 | **Wheels** | 65mm Diameter | Compatible with TT motor shafts. |
| 1 | **Caster** | Universal Wheel | Front support for chassis stability. |

---

## 🛡️ Passive Components

Essential small parts for circuit logic and protection.

| Qty | Component | Value | Purpose |
|-----|-----------|-------|---------|
| 2 | **Resistor** | 10kΩ | Voltage divider for LDRs (Light Sensors). |
| 3 | **Resistor** | 220Ω | Current limiting for RGB LED channels (Red, Green, Blue). |
| 1 | **Capacitor** | 100µF 16V | Power smoothing for the buck converter (optional but recommended). |

---

## 🔧 Hardware & Mechanical

| Qty | Item | Notes |
|-----|------|-------|
| 1 | **Chassis** | Laser-cut acrylic or 3D printed. Design files included in repo. |
| 1 | **Breadboard** | Mini 170-point size. Used for signal wiring distribution. |
| 10m | **Wire** | 22AWG solid core. Multiple colors helps with troubleshooting. |
| 20 | **Jumpers** | Male-to-Female wires for connecting sensors to ESP32. |
| 1 | **USB Cable** | Micro-USB. **Must be data capable** for flashing code. |
| 1 | **Velcro** | Strap or tape to secure batteries to chassis. |

---

## 🛠️ Tools Required

You do not need a full lab, but these specific tools are necessary for assembly:

* **Soldering Iron:** Temperature controlled (350°C)
* **Multimeter:** Critical for setting Buck Converter voltage to 5.0V *before* connecting the ESP32.
* **Wire Strippers & Pliers**
* **Hot Glue Gun:** For mounting sensors and the LED.
* **Screwdrivers:** Phillips and flat head.

---

## ⚠️ Important Sourcing Notes

* **Motor Driver:** The `Technical Specification` mentions the L298N, but the `BUILD_GUIDE` explicitly warns **AGAINST** it due to size. Stick to the **L9110S** or **TB6612FNG**.
* **ESP32:** Ensure you buy the **30-pin** version. The 38-pin versions will not fit the wiring diagrams provided in the manual.
* **Batteries:** 18650 LiPo batteries can be dangerous. Use a dedicated charger or the TP4056 board and never leave them charging unattended.
