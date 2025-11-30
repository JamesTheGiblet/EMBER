# EMBER v0.1 Build Guide

## Assembling Your First Artificial Life Form

-----

## What You're Building

A stationary robot that:

- Senses light with two eyes (LDRs)
- Gains energy from light above its genetic threshold
- Loses energy just by existing
- Shows its state with an RGB LED (green=thriving, red=dying, off=dead)
- Lives or dies based on its genome and environment
- **Connects to WiFi** for a web dashboard, JSON API, and OTA updates

**This is not a toy. This is a minimal viable organism.**

-----

## Before You Start

### Required Skills

- Basic soldering
- Wire stripping and crimping
- Using a multimeter
- Flashing ESP32 via Arduino IDE
- Basic WiFi network knowledge

### Time Required

- First build: 3-4 hours
- Subsequent builds: 1-2 hours
- WiFi configuration: +15 minutes

### Safety Equipment

- Safety glasses (for soldering)
- Well-ventilated area
- Fire-safe work surface

-----

## Parts List

### Electronics

| Qty | Component | Specification | Notes |
|-----|-----------|---------------|-------|
| 1 | ESP32 DevKit | 30-pin version | Main brain |
| 1 | H-Bridge Motor Driver | 4-channel digital (IN1/IN2/IN3/IN4) | Small board, not L298N |
| 2 | TT Motor | 3-6V DC gearmotor | Yellow/blue common type |
| 2 | LDR | GL5516 photoresistor | Light sensors |
| 1 | Ultrasonic Sensor | HC-SR04 | Distance sensing |
| 1 | RGB LED | 5mm common cathode | State indicator |
| 1 | Buck Converter | 7-12V in, 5V 3A out | Voltage regulation |
| 2 | LiPo Battery | 3.7V 2000mAh | Power source |
| 1 | Charging Board | TP4056 dual cell | USB charging |
| 1 | Velcro Strap | - | For securing batteries |

### Passive Components

| Qty | Component | Value | Purpose |
|-----|-----------|-------|---------|
| 2 | Resistor | 10kΩ | LDR voltage divider |
| 3 | Resistor | 220Ω | RGB LED current limiting |
| 1 | Capacitor | 100µF 16V electrolytic | Power smoothing (optional) |

### Hardware & Mechanical

| Qty | Item | Specification |
|-----|------|---------------|
| 1 | Chassis | Laser-cut acrylic or 3D printed |
| 2 | Wheels | 65mm diameter for TT motors |
| 1 | Caster wheel | Front support |
| 10m | Wire | 22AWG solid core, multiple colors |
| 1 | Breadboard | Mini 170-point (for prototyping) |
| 20 | Jumper wires | Male-to-female, various lengths |
| 1 | USB cable | Micro-USB for ESP32 programming |

### Tools Required

- Soldering iron (temperature controlled, 350°C)
- Solder (60/40 or lead-free)
- Wire strippers
- Needle-nose pliers
- Multimeter
- Hot glue gun
- Screwdrivers (Phillips + flat)
- Helping hands/PCB holder

-----

## Understanding Your H-Bridge Motor Driver

### NOT the L298N Module

You have a **compact digital H-bridge** with 4 control pins (IN1/IN2/IN3/IN4), not the large L298N board.

**Common types:**

- L9110S dual motor driver (small blue board)
- TB6612FNG motor driver (small purple/green board)
- DRV8833 motor driver (tiny breakout)

### Identifying Your Driver

**L9110S (most common cheap version):**

```txt
Pins: B-1A, B-1B, GND, VCC, A-1A, A-1B
- VCC: 2.5-12V (connect to 7.4V battery)
- GND: Ground
- A-1A/A-1B: Motor A control (both digital)
- B-1A/B-1B: Motor B control (both digital)
```txt

**TB6612FNG (better quality):**

```txt
Pins: VM, VCC, GND, AO1, AO2, BO1, BO2, AIN1, AIN2, PWMA, STBY, PWMB, BIN1, BIN2
- VM: Motor voltage (7.4V battery)
- VCC: Logic voltage (3.3V from ESP32)
- PWMA/PWMB: Speed control (connect to ESP32 PWM pins)
- AIN1/AIN2: Motor A direction
- BIN1/BIN2: Motor B direction
- STBY: Standby (pull HIGH to enable)
```

**We'll cover both types below.**

-----

## Pin Assignments

This section provides the practical wiring connections for the components. For the definitive pinout map and logical pin definitions within the code, please refer to the **`EMBER_v0.1_SPEC.md`** document.

### L9110S Motor Driver Wiring

**Connection Table:**

| ESP32 Pin | L9110S Pin | Wire Color Suggestion |
|-----------|--------------------|----------------------|
| GPIO15 | A-1A (Motor A IN1) | Orange |
| GPIO2 | A-1B (Motor A IN2) | Yellow |
| GPIO16 | B-1A (Motor B IN1) | Green |
| GPIO17 | B-1B (Motor B IN2) | Blue |
| 7.4V Battery+ | VCC | Red (thick wire) |
| GND | GND | Black (thick wire) |

### TB6612FNG Motor Driver Wiring

**Connection Table:**

| ESP32 Pin | TB6612 Pin | Wire Color |
|-----------|------------|------------|
| GPIO15 | AIN1 | Orange |
| GPIO2 | AIN2 | Yellow |
| GPIO5 | PWMA | Red |
| GPIO16 | BIN1 | Green |
| GPIO17 | BIN2 | Blue |
| GPIO4 | PWMB | Purple |
| GPIO13 | STBY | White |
| 3.3V | VCC | Red (thin) |
| 7.4V Battery+ | VM | Red (thick) |
| GND | GND | Black |

### Other Components (Same for Both)

-----

## Step-by-Step Assembly

### Step 1: Prepare the Chassis

**If using laser-cut acrylic:**

1. Remove protective film from all pieces
2. Test-fit all components before gluing
3. Mark mounting holes with pencil

**If using 3D printed:**

1. Clean up support material
2. Test-fit threaded inserts if using
3. Sand any rough edges

**Component layout:**

```txt
        [Front]
          ___
         |US |  ← Ultrasonic
         |___|

   ●           ●   ← LDRs at front corners
   
┌─────────────────────┐
│    [ESP32]          │
│                     │
│  [Buck]  [H-Brdg]   │
│                     │
│ [Batt][Batt]        │
│       ⚫             │ ← RGB LED (top, visible)
└─────────────────────┘

   ◐           ◐   ← Motors/wheels
```

### Step 2: Mount the Motors

1. **Attach motors to chassis:**
       - Use M3 screws (usually included with motors)
       - Ensure motor shafts point outward
       - Motors should be parallel and level

2. **Attach wheels:**
       - Press wheels onto motor shafts
       - Ensure tight fit (add tape if loose)
       - Wheels should not rub chassis

3. **Add caster wheel:**
       - Mount at front center
       - Adjust height so chassis is level
       - Should rotate freely

### Step 3: Install Power System

-----

## Power Management System (Battery Monitoring)

To protect your batteries and maximize runtime, EMBER uses a voltage divider to monitor battery voltage and automatically adjust performance.

### Wiring the Voltage Divider

- **Purpose:** Safely measure battery voltage (2S LiPo, 6.0V–8.4V) using ESP32 ADC pin.
- **Resistors:** 20kΩ (R1, high side) and 10kΩ (R2, low side)
- **ADC Pin:** Use GPIO32 (right side, pin 21) for battery sense (do not use GPIO34/35, those are for LDRs)

**Wiring:**

1. Battery + → one leg of 20kΩ resistor (R1)
2. Other leg of 20kΩ resistor joins one leg of 10kΩ resistor (R2) and also connects to ESP32 GPIO32
3. Other leg of 10kΩ resistor → Battery - (GND)

**Diagram:**

Battery + → 20kΩ → (join) → 10kΩ → Battery - (GND)
                        │
                        └→ ESP32 GPIO32 (ADC)

**Component List:**

| Component | Value | Purpose |
|-----------|-------|---------|
| R1        | 20kΩ  | High side |
| R2        | 10kΩ  | Low side  |
| ADC Pin   | GPIO32| Battery sense |

### Power Modes (Automatic)

EMBER automatically switches between 5 power modes based on battery voltage:

| Mode      | Voltage Range | LED         | Features           |
|-----------|--------------|-------------|--------------------|
| NORMAL    | 7.8–8.4V     | Green       | Full performance   |
| ECONOMY   | 7.2–7.8V     | Blue        | Reduced features   |
| LOW       | 6.8–7.2V     | Yellow      | Essential only     |
| CRITICAL  | 6.4–6.8V     | Red flash   | Survival mode      |
| SHUTDOWN  | <6.4V        | Red solid   | Battery protection |

The robot will reduce speed, disable non-critical features, and eventually shut down to prevent battery damage.

**Tip:** Always use quality resistors (±1%) and double-check wiring before powering up.

-----

## CRITICAL: Test before connecting ESP32

A. **Mount batteries:**
       - Secure the two LiPo batteries to the chassis bottom with a velcro strap or double-sided tape.
       - Ensure batteries won't shift during movement.
       - Place them where they won't interfere with wheels or wiring.

B. **Wire batteries in series:**

[Image of two 18650 batteries wired in series diagram]

```txt
   Battery 1: [+] ─────┬────→ TO LOADS (+7.4V)
              [-] ──┐  │
                    │  │
   Battery 2: [+] ──┘  │
              [-] ─────┴────→ TO LOADS (GND)
```

C. **Add power switch (recommended):**
       - Cut positive wire from battery
       - Install toggle switch inline
       - Use heat shrink to insulate

D. **Connect buck converter:**

[Image of LM2596 buck converter wiring diagram]
![LM2596 buck converter wiring diagram](../hardware/photos/LM2596%20buck%20converter%20wiring%20diagram.jpg)

```txt
   Buck Converter:
   IN+  ← Battery positive (7.4V)
   IN-  ← Battery negative (GND)
   OUT+ → ESP32 VIN (5V regulated)
   OUT- → ESP32 GND
```

E. **Adjust buck converter output:**
       - **DO THIS BEFORE CONNECTING ESP32**
       - Connect multimeter to OUT+ and OUT-
       - Turn adjustment screw until multimeter reads 5.0V
       - Verify multiple times (incorrect voltage will damage ESP32)

F. **Test power system:**

<!-- end list -->

```txt
   ✓ Measure battery voltage: should be 7.4-8.4V
   ✓ Measure buck output: should be 5.0V ±0.1V
   ✓ Connect LED to buck output: should light dimly
```

### Step 4: Mount ESP32

A. **Create a secure mount:**
       - Use standoffs or hot glue
       - USB port must be accessible
       - Leave space above for heat dissipation

B. **Connect power:**

<!-- end list -->

```txt
   Buck OUT+ → ESP32 VIN pin
   Buck OUT- → ESP32 GND pin
```

C. **Test ESP32:**
       - Plug in USB cable (while battery connected is fine)
       - Onboard LED should light
       - ESP32 should be warm, not hot

### Step 5: Wire the Light Sensors (LDRs)

**Each LDR needs a voltage divider circuit:**

![LDR voltage divider circuit schematic](../hardware/photos/LDR%20voltage%20divider%20circuit%20schematic.png)

```txt
3.3V ──┬─── [LDR] ───┬─── [10kΩ] ─── GND
       │             │
     (exposed)    TO ESP32
                 (GPIO 34/21)
```

**Left LDR:**

1. Solder the **right leg** of the 10kΩ resistor to the **left leg** of the LDR. This connection point is the "junction".
2. Solder a wire from this junction to the **ESP32 GPIO34** pin.
3. Solder a wire from the free (right) leg of the LDR to the **ESP32 3.3V** pin.
4. Solder a wire from the free (left) leg of the resistor to an **ESP32 GND** pin.

**Right LDR:**

1. Repeat the exact same process, but connect the junction wire to **ESP32 GPIO21**.

**Mounting LDRs:**

- Hot glue to front corners of chassis
- Face forward/outward at \~45° angle
- Ensure nothing blocks light path
- Keep legs short to reduce noise

**Testing:**

```cpp
// Upload this test sketch:
void setup() {
  Serial.begin(115200);
  pinMode(34, INPUT);
  pinMode(35, INPUT);
}

void loop() {
  Serial.print("Left: ");
  Serial.print(analogRead(34));
  Serial.print(" Right: ");
  Serial.println(analogRead(35));
  delay(500);
}

// Cover each LDR - values should drop
// Shine light - values should rise
// Values should be: 0-4095
```

### Step 6: Wire the RGB LED

**Circuit for common cathode LED:**

```
GPIO14 ──[220Ω]── LED RED ──┐
GPIO33 ──[220Ω]── LED GRN ──┼── GND
GPIO12 ──[220Ω]── LED BLU ──┘
```

**Assembly:**

1. **Identify LED pins:**

<!-- end list -->

```
   Looking at LED (flat side = cathode side):
   
   Longest pin = Common cathode (to GND)
   
   [R] [G] [-] [B]   ← Typical pinout
    │   │   │   │
```

2. **Solder resistors:**
      - Cut resistor legs to \~5mm
      - Solder 220Ω to each color pin (not cathode)
      - Leave cathode leg bare

3. **Solder wires:**
      - Red wire to RED resistor → GPIO14
      - Green wire to GREEN resistor → GPIO33
      - Blue wire to BLUE resistor → GPIO12
      - Black wire to cathode → GND

4. **Mount LED:**
      - Drill 5mm hole in top of chassis
      - Push LED through (resistors should be below)
      - Secure with hot glue from underneath
      - LED should be visible from all angles

**Testing:**

```cpp
void setup() {
  pinMode(14, OUTPUT);
  pinMode(33, OUTPUT);
  pinMode(12, OUTPUT);
}

void loop() {
  digitalWrite(14, HIGH); delay(1000); // Red
  digitalWrite(14, LOW);
  digitalWrite(33, HIGH); delay(1000); // Green
  digitalWrite(33, LOW);
  digitalWrite(12, HIGH); delay(1000); // Blue
  digitalWrite(12, LOW);
}
```

### Step 7: Wire the Motor Driver

**Note:** The HAL code automatically detects and supports both driver types. Just wire according to your hardware.

#### For L9110S Driver

**Power connections:**

<!-- end list -->

```txt
   Battery 7.4V → VCC
   GND → GND (shared with ESP32 GND)
```

**Control connections:**

<!-- end list -->

```txt
   ESP32 GPIO15 → A-1A (Motor A)
   ESP32 GPIO16  → A-1B (Motor A)
   ESP32 GPIO17 → B-1A (Motor B)
   ESP32 GPIO18 → B-1B (Motor B)
```

**Motor connections:**

<!-- end list -->

```txt
   Left Motor:  Connect to Motor A output
   Right Motor: Connect to Motor B output
```

   **Note:** Polarity determines direction. If motor spins wrong way, swap wires.

#### For TB6612FNG Driver

**Power connections:**

<!-- end list -->

```txt
   Battery 7.4V → VM
   ESP32 3.3V → VCC
   GND → GND (shared)
```

**Control connections:**

<!-- end list -->

```txt
   ESP32 GPIO15 → AIN1
   ESP32 GPIO16 → AIN2
   ESP32 GPIO5  → PWMA
   ESP32 GPIO17 → BIN1
   ESP32 GPIO18 → BIN2
   ESP32 GPIO4  → PWMB
   ESP32 GPIO13 → STBY (or tie to VCC)
```

**Motor connections:**

<!-- end list -->

```txt
   Left Motor:  AO1, AO2
   Right Motor: BO1, BO2
```

**The HAL code handles the differences automatically\!**

### Step 8: Wire the Ultrasonic Sensor

**HC-SR04 connections:**

```txt
VCC  → 5V (from buck converter OUT+)
TRIG → ESP32 GPIO25
ECHO → ESP32 GPIO26
GND  → GND
```

**Mounting:**

- Hot glue to front center of chassis
- Face forward, level with ground
- Nothing blocking the "eyes" (transmitter/receiver)

**Not used in v0.1, but wire it for future versions.**

### Step 9: Final Assembly

1. **Cable management:**
       - Bundle wires with zip ties
       - Keep power wires separate from signal wires
       - Ensure nothing touches motor shafts
       - Leave slack for vibration

2. **Secure all components:**
       - Hot glue any loose boards
       - Ensure ESP32 can't short against metal
       - Batteries should not move

3. **Label your bot:**
       - Write bot\_id (0-8) on chassis with marker
       - Write hostname (`ember-bot-N`) on bottom
       - This is critical for evolution experiments and network access

4. **Final inspection:**

<!-- end list -->

```txt
   ✓ No exposed wire touching chassis
   ✓ All components secure
   ✓ Battery polarity correct
   ✓ Motors can't touch circuit boards
   ✓ USB port accessible
   ✓ Power switch accessible
   ✓ LED visible from top
   ✓ LDRs unobstructed
```

-----

## Programming with PlatformIO

This project uses **PlatformIO** for a streamlined build and upload process.

### Step 1: Install VS Code & PlatformIO

1. **Install Visual Studio Code:** If you don't have it, download it from code.visualstudio.com.
2. **Install PlatformIO Extension:**
    - In VS Code, open the **Extensions** view (the icon with four squares).
    - Search for `PlatformIO IDE` and click **Install**.
    - Restart VS Code after installation.

### Step 2: Configure Bot Identity and WiFi

This is the most critical step for enabling network features and running swarm experiments. You must edit the `ember_v0.1_hal_ota.ino` file.

1. **Open Project Folder:** In VS Code, go to `File > Open Folder...` and select the root `EMBER` project directory. PlatformIO will automatically recognize the `platformio.ini` file.

2. **Edit Configuration:** Open the `ember_v0.1_hal_ota.ino` file in the project root.

3. **Set WiFi Credentials:** Near the top of the file, find the `CONFIGURATION` block and enter your WiFi network's SSID and password.

    ```cpp
    // WiFi credentials - CHANGE THESE
    const char* WIFI_SSID = "YourNetworkName";
    const char* WIFI_PASSWORD = "YourPassword";
    ```

4. **Set a Unique Hostname:** In the same block, define a unique hostname for this specific bot. This is used for accessing the web dashboard (e.g., `http://ember-bot-0.local/`).

    ```cpp
    // OTA configuration
    const char* OTA_HOSTNAME = "ember-bot-0";  // Change for each bot
    const char* OTA_PASSWORD = "ember2025";     // OTA update password
    ```

    **IMPORTANT:** Every bot on your network **must** have a unique hostname. The convention is `ember-bot-N`, where N is the bot's ID number.

**Set the Bot's Internal ID:** A little further down, in the `GENETIC CODE` section, set the `bot_id`. This number should match the number in the hostname.

```cpp
    Genome genome = {
        .light_threshold = 0.5,
        .efficiency = 1.0,
        .bot_id = 0, // <-- CHANGE THIS FOR EACH BOT (0, 1, 2, etc.)
        .generation = 0
    };
    ```

    **Example for the first three bots:**

    | Bot | `OTA_HOSTNAME` | `genome.bot_id` |
    |:---:|:----------------|:---------------:|
    | #0  | `"ember-bot-0"` | `0`             |
    | #1  | `"ember-bot-1"` | `1`             |
    | #2  | `"ember-bot-2"` | `2`             |

### Step 3: First Upload via USB

1. **Connect the Bot:** Connect the ESP32 to your computer with a data-capable Micro-USB cable.
2. **Click Upload:** In the VS Code status bar at the bottom, click the **right-arrow icon (→)** to build and upload the project.
3. **Wait for Completion:** PlatformIO will compile and upload the code. This first upload must be done via USB.

### Step 4: Verify via Serial Monitor

1. **Open Serial Monitor:** In the VS Code status bar, click the **plug icon (🔌)** to open the PlatformIO Serial Monitor. The baud rate is set automatically from `platformio.ini`.
2. **Check the Output:** You should see the boot sequence, including the WiFi connection status and the bot's assigned IP and `.local` address. This confirms your setup is working.

    `[WiFi] Connected!     [WiFi] IP: 192.168.1.50     [mDNS] Access at: http://ember-bot-0.local/     ...     Light: 0.512 | Energy: 73.2 | Alive: 5s | Status: ALIVE | IP: 192.168.1.50`

### Step 5: Wireless (OTA) Uploads

After the first USB flash, all future updates can be wireless.

1. **Find the Bot's IP:** Note the IP address from the Serial Monitor or the bot's web dashboard.
2. **Open the PlatformIO CLI:** In VS Code, select `Terminal > New Terminal` to open a command line.
3. **Run the OTA Upload Command:**

    ```bash
    pio run -t upload --upload-port <IP_ADDRESS_OF_BOT>
    ```

    For example:

    ```bash
    pio run -t upload --upload-port 192.168.1.50
    ```

The bot's LED will turn purple during the update and flash green upon success.

-----

## First Power-On Checklist

**BEFORE connecting battery:**

```

□ All solder joints inspected (no bridges)
□ No exposed wires touching
□ Buck converter set to 5.0V
□ Multimeter tested all connections
□ Code uploaded successfully (via USB)
□ WiFi credentials configured (if using network features)
□ Bot_id is unique for this bot (0-8)
□ Hostname is unique (ember-bot-N)

```txt

**Power-on sequence:**

1. Turn on power switch
2. White LED flash 3× (boot complete)
3. Blue pulse (WiFi connecting) - or skip if no WiFi
4. Blue flash 3× (WiFi connected) - or yellow if failed
5. Green or red LED (life state)
6. Open Serial Monitor (115200 baud)
7. Verify stats printing every second
8. If WiFi connected, open web dashboard

**If LED doesn't light:**

- Check buck converter 5V output
- Check ESP32 GND connection
- Check LED wiring and polarity

**If code doesn't upload:**

- Press and hold BOOT button during upload
- Try different USB cable
- Check drivers installed (CP210x or CH340)

-----

## Calibration Procedure

### Step 1: Dark Calibration

1. Place bot in completely dark box
2. Open Serial Monitor or web dashboard
3. Note light level reading (should be \<0.05)
4. If too high, check LDR circuit wiring
5. Use `sensors` command for detailed readings

### Step 2: Bright Calibration

1. Shine flashlight directly at LDRs
2. Note light level reading (should be \>0.95)
3. If too low, check LDR isn't damaged
4. Use web dashboard for real-time monitoring
5. If readings differ \>50% between left/right, check wiring

### Step 3: Survival Test

1. Place bot in normal room light
2. Watch for 5 minutes (use web dashboard or serial)
3. Energy should stay relatively stable (±10%)
4. **If dying:**
       - Web: Click randomize or adjust threshold
       - Serial: `threshold 0.3` then `reset`
5. **If capped at 100:** Genome well-suited (this is good\!)

### Step 4: Network Test (If WiFi Enabled)

1. **Web dashboard:**
       - Should auto-refresh every 2 seconds
       - Try clicking "Mutate" button
       - Verify genome changes and saves

2. **JSON API:**
       - Open `http://ember-bot-0.local/api/stats`
       - Should see JSON data
       - Save for data logging scripts

3. **OTA Update:**
       - Make code change
       - Upload via network port
       - Verify purple LED and success flash

### Step 5: Motor Test (Even Though Disabled)

## IMPORTANT: Test motors work before evolution experiments

The HAL automatically handles your motor driver type, so this test works for both L9110S and TB6612FNG:

1. Temporarily enable motors in code:

<!-- end list -->

```cpp
   // In loop(), add:
   motors.forward(100);   // Both forward
   delay(2000);
   motors.backward(100);  // Both backward
   delay(2000);
   motors.stop();
   delay(2000);
```

1. Each motor should:
       - Spin forward smoothly
       - Spin backward smoothly
       - Stop completely

2. If motor spins wrong direction, swap motor wires at driver

3. After testing, remove test code and re-upload EMBER v0.1 (motors disabled)

-----

## Troubleshooting

### Network Issues

**Problem:** Bot shows yellow flash (WiFi failed)

**Cause:** Can't connect to configured network

**Fix:**

1. Bot works fine offline - this is OK
2. Check SSID/password in code
3. Ensure 2.4GHz network
4. Move closer to router
5. Serial command `wifi` shows connection status

-----

**Problem:** Web dashboard doesn't load

**Cause:** Network configuration or firewall

**Fix:**

1. Try IP address instead of .local hostname
2. Check bot is on same network as computer
3. Ping the bot: `ping ember-bot-0.local` or `ping 192.168.1.50`
4. Check firewall settings
5. Restart browser

-----

**Problem:** OTA upload fails

**Cause:** Network issue or insufficient power

**Fix:**

1. Check battery voltage (should be \>7V)
2. Move bot closer to router
3. Try upload again (network ports can be flaky)
4. If repeated failures, use USB upload
5. Check Serial Monitor for error messages

-----

### Hardware Issues

**Problem:** LED stuck on one color

**Fix:**

1. Serial command: `led` to test all colors
2. Web interface: Should show live LED state
3. Check wiring: GPIO23=red, GPIO22=green, GPIO21=blue
4. Verify common cathode connected to GND

-----

**Problem:** Light sensor not responding

**Fix:**

1. Serial command: `sensors` for detailed readings
2. Web dashboard shows live light levels
3. Verify circuit: 3.3V → LDR → GPIO34/35 → 10kΩ → GND
4. Measure voltage at GPIO (should vary 0-3.3V with light)

-----

**Problem:** Bot resets randomly

**Fix:**

1. Check battery voltage under load (\>6.5V)
2. Charge batteries fully
3. Add 100µF capacitor across buck converter output
4. Check all GND connections
5. WiFi can draw power - disable if battery weak

-----

**Problem:** Genome doesn't save across reboots

**Cause:** Preferences not being written to flash

**Fix:**

1. Serial command: `save` to manually save
2. Web interface: Click "Save Genome" button
3. Mutations via web/serial should auto-save
4. Serial command: `genome` to verify after reboot
5. If still failing, check ESP32 flash isn't full

-----

## Building Multiple Bots

### Batch Production Tips

**Build 3 at a time:**

1. Do all mechanical assembly first (chassis, motors, wheels)
2. Do all power wiring second (batteries, buck, charging)
3. Do all sensor wiring third (LDRs, LED)
4. Do all motor wiring last (easiest to troubleshoot)
5. **Program all at once** (USB first, then OTA for updates)

**Network setup for batch:**

1. Flash Bot 0 with `ember-bot-0`, verify it works
2. Flash Bot 1 with `ember-bot-1`, verify it works
3. Continue for all bots
4. **Once all working:** Use OTA to update all simultaneously\!

**Quality control checklist per bot:**

```txt
□ Unique bot_id (0-8)
□ Unique hostname (ember-bot-N)
□ WiFi connects successfully
□ Web dashboard accessible
□ OTA updates work
□ Genome saves/loads correctly
□ All sensors calibrated
□ Motors tested (even if disabled)
```

**Recommended build order:**

1. Bot 0, 1, 2 - first batch, expect issues, learn process
2. Bot 3, 4, 5 - second batch, faster, apply lessons
3. Bot 6, 7, 8 - third batch, fastest, production quality

**Total time for 9 bots:**

- Hardware assembly: 15-20 hours
- Network setup: +2-3 hours
- Testing/calibration: +2-3 hours
- **Total: \~24-26 hours** (spread over several days)

-----

## What Success Looks Like

### After First Power-On

**Hardware success indicators:**

1. White LED flash 3× (boot complete)
2. Blue flash 3× (WiFi connected) OR yellow 3× (offline mode)
3. LED shows life state (green/red)
4. Serial output every second with stats
5. Energy rising in light, falling in dark
6. Bot "dies" (LED off) in darkness after \~16 minutes

**Network success indicators:**

1. Web dashboard loads at `http://ember-bot-N.local/`
2. Dashboard auto-refreshes every 2 seconds
3. Control buttons work (mutate, reset, save)
4. JSON API returns data
5. OTA updates work (via network ports in Arduino IDE)
6. Genome persists across reboots

### After Calibration

**In bright light:**

- LED solid green
- Energy near 100 (stable)
- Light reading \>0.6
- Web dashboard shows "Thriving"

**In room light:**

- LED green or slow red flash
- Energy 40-80 (stable or slowly changing)
- Light reading 0.3-0.6
- Genome determines survival

**In darkness:**

- LED fast red flash
- Energy dropping steadily (-0.1/second)
- Light reading \<0.1
- Death after \~1000 seconds
- Web dashboard shows "Critical" then "DEAD"

### Ready for Evolution

**All 9 bots should:**

- Boot successfully with network connectivity
- Read light independently with calibrated sensors
- Show different genomes (randomized, saved to flash)
- Survive or die based on genome + environment
- Respond to web interface controls
- Respond to serial commands
- Allow OTA updates
- Persist genome across reboots

**Network dashboard shows:**

- All 9 bots accessible via `http://ember-bot-N.local/`
- Live fitness tracking (survival time)
- Real-time energy levels
- Genome for each bot
- Easy mutation/selection via web interface

**If all criteria met, you're ready for evolution experiments\!**

-----

## Next Steps

1. **Read the EVOLUTION\_GUIDE.md** - Running selection experiments
2. **Set up monitoring** - Web dashboards or API scripts
3. **Build your arena** - Controlled environment with adjustable light
4. **Start with simple tests** - Bright vs dim vs dark
5. **Use web interface** - Track fitness, copy genomes, mutate
6. **Log results via API** - Automate data collection
7. **Run evolution** - Select winners, breed, repeat

**You've built networked artificial life. Now watch it evolve while monitoring it in real-time.**

-----

## Common Mistakes to Avoid

❌ **Don't skip the buck converter voltage adjustment** - will damage ESP32  
❌ **Don't mount LDRs facing up** - chassis shadow affects readings  
❌ **Don't forget unique bot\_id AND hostname** - ruins tracking  
❌ **Don't use same hostname for multiple bots** - network conflicts  
❌ **Don't power motors from buck converter** - needs too much current  
❌ **Don't leave battery charging unattended** - fire risk  
❌ **Don't forget to test OTA before final assembly** - hard to debug later  
❌ **Don't skip WiFi if you want evolution experiments** - manual tracking is tedious  
❌ **Don't forget to save genome changes** - use `save` command or web button  
❌ **Don't cross signal and power wires** - causes noise in sensors  

-----

## Support and Resources

### If You Get Stuck

1. Check troubleshooting sections above
2. Verify against pin assignments
3. Test each subsystem individually
4. Use `sensors` and `wifi` serial commands for diagnostics
5. Check web dashboard for live feedback
6. Compare to a working bot if you have one

### Component Datasheets

- ESP32: espressif.com
- L9110S: GitHub datasheets
- TB6612FNG: [sparkfun.com/products/14450](https://www.google.com/search?q=https://sparkfun.com/products/14450)
- GL5516: common LDR datasheet
- HC-SR04: multiple sources online

### Network Resources

- mDNS/Bonjour: Support varies by OS
- OTA: ArduinoOTA library documentation
- Web Server: ESP32 WebServer examples
- Preferences: ESP32 NVS documentation

-----

*EMBER v0.1 Build Guide - HAL+OTA Edition*  
*For digital H-bridge motor drivers*  
*With network features and persistent storage*  
*Part of the Forge Theory Project*  
*MIT License*
