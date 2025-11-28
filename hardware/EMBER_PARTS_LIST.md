# EMBER v0.1 - Hardware Parts List

**Complete Component Guide for Building EMBER Bots**

This document provides a comprehensive list of all components required to build an EMBER bot, with current pricing, sourcing recommendations, and build options.

---

## Build Versions

This parts list covers both the **v0.1 HAL+OTA Edition** (stationary) and preparation for **v0.2 Mobile** (movement enabled).

**Recommended approach:** Build with all components now (including motors), run v0.1 experiments (motors disabled), then OTA update to v0.2 when ready. This avoids disassembly.

---

## Quick Cost Summary

| Build Type | Component Cost | Tool Cost (if needed) | Total |
|------------|----------------|----------------------|-------|
| **v0.1 Minimum** | ~$20-25 | ~$30-50 | ~$50-75 |
| **v0.1 + Motors (recommended)** | ~$25-30 | ~$30-50 | ~$55-80 |
| **v0.2 Ready** | ~$25-30 | ~$30-50 | ~$55-80 |
| **Per additional bot** | ~$25-30 | $0 | ~$25-30 |

**For 9-bot swarm:** ~$225-270 (components only) + one-time tool cost

---

## Core Electronics

### Microcontroller & Power

| Qty | Component | Specification | Est. Cost | Example Sources | Notes |
|-----|-----------|---------------|-----------|----------------|-------|
| 1 | **ESP32 DevKit** | 30-pin version, CP2102/CH340 | $4-8 | Amazon, AliExpress, Adafruit | **CRITICAL:** Must be 30-pin. Verify WiFi antenna present. |
| 1 | **Buck Converter** | MP1584EN or LM2596 module | $1-3 | Amazon, AliExpress | **CRITICAL:** Adjustable output, 3A minimum. Test before connecting ESP32! |
| 2 | **18650 Battery** | 3.7V, 2000mAh+, protected | $6-12 | Battery junction, AliExpress | Protected cells recommended. **High-quality brands only** (Samsung, Panasonic, LG). |
| 1 | **Battery Holder** | 2×18650, series connection | $2-4 | Amazon, AliExpress | Must have series wiring (7.4V output). |
| 1 | **TP4056 Charging Board** | Dual 18650, micro-USB | $2-5 | Amazon, AliExpress | **CRITICAL:** Get 2S version (two cells). Protection circuit recommended. |
| 1 | **Power Switch** | SPST toggle, mini | $1-2 | Amazon, AliExpress | Optional but highly recommended. |

**Subtotal (Power):** $16-34

### Sensors

| Qty | Component | Specification | Est. Cost | Example Sources | Notes |
|-----|-----------|---------------|-----------|----------------|-------|
| 2 | **LDR (Photoresistor)** | GL5516 or GL5528 | $0.50-2 | Amazon, AliExpress, Adafruit | The bot's "eyes". Get extras (cheap, sometimes DOA). |
| 1 | **RGB LED** | 5mm, common cathode | $0.50-2 | Amazon, AliExpress, Adafruit | **CRITICAL:** Must be common cathode, not common anode. Get extras. |
| 1 | **HC-SR04** | Ultrasonic distance sensor | $1-3 | Amazon, AliExpress | Wired in v0.1 but not used until v0.2+. |

**Subtotal (Sensors):** $2-7

### Motor System (Recommended Now for v0.2 Readiness)

| Qty | Component | Specification | Est. Cost | Example Sources | Notes |
|-----|-----------|---------------|-----------|----------------|-------|
| 2 | **TT Gear Motors** | 3-6V, 1:48 ratio | $2-6 | Amazon, AliExpress | Yellow/blue hobby motors. Get 1:48 or 1:90 ratio (not 1:20 - too fast). |
| 1 | **Motor Driver** | L9110S or TB6612FNG | $2-5 | Amazon, AliExpress | **L9110S:** Cheaper, works. **TB6612FNG:** Better quality. HAL supports both! |
| 2 | **Wheels** | 65mm diameter, TT shaft | $2-4 | Amazon, AliExpress | Must fit TT motor D-shaft (3mm). |
| 1 | **Caster Wheel** | Ball caster, 15-20mm | $1-3 | Amazon, AliExpress | Front support. Get metal ball version. |

**Subtotal (Motors):** $7-18

**Note:** Skip motors for v0.1-only build, but you'll need to disassemble later to add them for v0.2.

---

## Passive Components

### Resistors & Capacitors

| Qty | Component | Value | Est. Cost | Example Sources | Purpose |
|-----|-----------|-------|-----------|----------------|---------|
| 2 | **Resistor** | 10kΩ, 1/4W | $0.10-0.50 | Amazon, AliExpress, Adafruit | LDR voltage dividers |
| 3 | **Resistor** | 220Ω, 1/4W | $0.10-0.50 | Amazon, AliExpress, Adafruit | RGB LED current limiting |
| 1 | **Capacitor** | 100µF, 16V+ electrolytic | $0.20-1 | Amazon, AliExpress | Optional but recommended for power stability |

**Subtotal (Passives):** $0.50-2

**Note:** Buy resistor assortment kits (~$5-10 for 500+ resistors). Much cheaper and useful for future projects.

---

## Mechanical & Hardware

### Chassis Options

| Option | Material | Cost | Build Time | Pros | Cons |
|--------|----------|------|------------|------|------|
| **Laser Cut Acrylic** | 3mm acrylic sheet | $5-10 | 30 min | Professional look, precise | Requires laser cutter access |
| **3D Printed** | PLA filament | $2-5 | 2-4 hours | Customizable, complex shapes | Requires 3D printer |
| **Cardboard** | Cardboard + hot glue | $0-2 | 1 hour | Cheap, rapid prototyping | Not durable |
| **Protoboard Platform** | Large protoboard | $3-5 | 15 min | No cutting needed, rigid | Less aesthetic |

**Recommended for beginners:** Protoboard platform or cardboard for first bot, then laser cut for swarm.

### Hardware & Wiring

| Qty | Item | Specification | Est. Cost | Example Sources | Notes |
|-----|------|---------------|-----------|----------------|-------|
| 1 | **Breadboard** | Mini 170-point or 400-point | $2-5 | Amazon, AliExpress | For prototyping before soldering |
| 1 | **Protoboard** | 7×9cm or larger (optional) | $2-4 | Amazon, AliExpress | For permanent soldered version |
| 5m | **Hookup Wire** | 22 AWG, solid core | $3-8 | Amazon, AliExpress | Get multiple colors (red, black, yellow, green, blue) |
| 20 | **Jumper Wires** | M-M, M-F, F-F assortment | $3-8 | Amazon, AliExpress | Pre-made jumpers save time |
| 1 | **USB Cable** | Micro-USB, data-capable | $2-5 | Amazon | For programming ESP32. **Must support data**, not just charging! |
| 10 | **M3 Screws** | 10mm length | $2-5 | Amazon, hardware store | For mounting components |
| 10 | **M3 Standoffs** | 10mm length | $2-5 | Amazon | Raises boards above chassis |
| - | **Zip Ties** | Small, assorted | $2-5 | Amazon, hardware store | Cable management |
| - | **Heat Shrink Tubing** | Assorted sizes | $5-10 | Amazon | Wire insulation (or use electrical tape) |
| - | **Double-Sided Tape** | Foam or standard | $2-5 | Amazon, hardware store | Quick component mounting |

**Subtotal (Mechanical):** $25-65 depending on choices

---

## Tools & Consumables

### Essential Tools (One-Time Investment)

| Tool | Est. Cost | Notes |
|------|-----------|-------|
| **Soldering Iron** | $15-50 | Temperature controlled (350°C) recommended. Cheaper irons work but harder to use. |
| **Solder** | $5-10 | 60/40 tin/lead or lead-free. Get 0.8mm diameter. |
| **Wire Strippers** | $5-15 | Self-adjusting recommended. Makes wire prep much faster. |
| **Flush Cutters** | $5-10 | For trimming component leads and wires. |
| **Multimeter** | $10-30 | **CRITICAL:** Must have to verify voltages. Cheap ones work fine. |
| **Hot Glue Gun** | $5-15 | For mounting sensors and securing components. Get high-temp version. |
| **Screwdriver Set** | $10-20 | Precision set with Phillips and flat. |
| **Helping Hands** | $5-15 | Third hand with clips for soldering. Very helpful. |

**Total Essential Tools:** $60-165 (one-time cost, used for all bots)

### Nice-to-Have Tools

| Tool | Est. Cost | Notes |
|------|-----------|-------|
| Desoldering Pump | $5-10 | For fixing mistakes |
| Solder Wick | $3-5 | Alternative to pump |
| Digital Calipers | $10-20 | For precise measurements |
| Label Maker | $15-30 | For marking bots clearly |
| Component Storage | $10-20 | Organizer boxes for parts |

### Consumables

| Item | Est. Cost | Notes |
|------|-----------|-------|
| Electrical Tape | $2-5 | Wire insulation |
| Isopropyl Alcohol | $3-5 | Flux cleanup |
| Solder Flux | $3-8 | Improves solder flow (optional) |
| Safety Glasses | $5-10 | **MANDATORY** when soldering |
| Paper Towels | $2-5 | General cleanup |

---

## Detailed Shopping List by Vendor

### Budget Build (AliExpress - Slow Shipping)

**Advantages:** Lowest cost, huge selection  
**Disadvantages:** 2-6 week shipping, variable quality  
**Best for:** Building swarms (9+ bots), have time to wait

| Component Bundle | Est. Cost | Ships From |
|------------------|-----------|------------|
| 5× ESP32 DevKit 30-pin | $15-20 | China |
| 10× GL5516 LDR | $2-5 | China |
| 10× RGB LED common cathode | $2-5 | China |
| 5× MP1584EN buck converter | $3-8 | China |
| 5× L9110S motor driver | $5-10 | China |
| 10× TT motor + wheel set | $15-25 | China |
| 5× HC-SR04 ultrasonic | $5-10 | China |
| 10× 18650 battery (protected) | $15-30 | China |
| Resistor assortment (500pc) | $5-8 | China |
| Jumper wire kit | $5-10 | China |
| Hookup wire spool set | $8-15 | China |
| **Total for 5 bots:** | **$80-146** | 2-6 weeks |

### Fast Build (Amazon - Quick Shipping)

**Advantages:** 1-2 day shipping, easy returns  
**Disadvantages:** Higher cost  
**Best for:** Single bot or small batches, need it now

| Component Bundle | Est. Cost | Ships From |
|------------------|-----------|------------|
| 3× ESP32 DevKit | $20-30 | US |
| 10× LDR pack | $5-10 | US |
| 10× RGB LED pack | $5-10 | US |
| 3× Buck converter | $10-15 | US |
| 3× Motor driver (L9110S or TB6612) | $12-20 | US |
| 6× TT motor + 6× wheel | $15-25 | US |
| 3× HC-SR04 | $10-15 | US |
| 6× 18650 battery + charger | $25-40 | US |
| Resistor kit | $10-15 | US |
| Jumper wire kit | $8-12 | US |
| Wire spool | $10-15 | US |
| **Total for 3 bots:** | **$130-207** | 1-2 days |

### Quality Build (Adafruit/SparkFun - Best Quality)

**Advantages:** Verified quality, great documentation, support education  
**Disadvantages:** Highest cost  
**Best for:** Educational institutions, when reliability critical

| Component | Est. Cost | Notes |
|-----------|-----------|-------|
| ESP32 Feather or Thing | $20-25 | Different pinout - verify compatibility |
| Quality LDRs (2×) | $2-4 | Matched pair |
| RGB LED diffused | $2-3 | Higher quality |
| Pololu buck regulator | $5-8 | Better efficiency |
| Motor driver (DRV8833) | $5-10 | Better than L9110S |
| Premium TT motors | $8-15 | Metal gears |
| Quality batteries | $15-25 | Name brand cells |
| **Total per bot:** | **$57-90** | Higher reliability |

---

## Purchasing Strategy

### For First Bot (Learning Build)

**Goal:** Minimize cost while learning

```
Amazon Prime (1-2 day shipping):
- 1× ESP32 DevKit ($8)
- 1× Buck converter ($3)
- 2× 18650 battery ($8)
- 1× TP4056 charger ($3)
- Breadboard + jumper kit ($8)
- Resistor assortment ($8)

Local Electronics Store:
- 2× LDR ($2)
- 1× RGB LED ($1)
- 3× 220Ω resistor ($1)
- 2× 10kΩ resistor ($1)

Already Have:
- USB cable
- Wire from old projects

Total: ~$43 + tools if needed
```

### For 9-Bot Swarm (Optimal)

**Goal:** Best cost/performance ratio

**Order 1 (AliExpress, 4-6 weeks before experiment):**

- 10× ESP32 DevKit
- 20× GL5516 LDR
- 10× RGB LED common cathode
- 10× L9110S motor driver
- 20× TT motors + wheels
- 10× HC-SR04 ultrasonic
- 20× 18650 batteries (extras for testing)
- Resistor/capacitor assortment
- Wire assortment
- Total: ~$120-150

**Order 2 (Amazon, 1 week before build day):**

- 5× TP4056 charging boards
- Extra USB cables
- Zip ties, standoffs, screws
- Hot glue sticks
- Total: ~$30-40

**Order 3 (Local Makerspace/University Shop):**

- Laser cut 9× chassis from provided DXF
- Total: ~$20-40

**Grand Total for 9 bots:** ~$170-230 (average $19-26 per bot)

---

## Component Sourcing Tips

### Batteries - Critical Safety Notes

**⚠️ IMPORTANT:** Lithium batteries are dangerous if mishandled.

**DO:**

- ✅ Buy from reputable sellers (Samsung, Panasonic, LG cells)
- ✅ Get protected cells (built-in protection circuit)
- ✅ Use TP4056 charging board (has overcharge protection)
- ✅ Check cells with multimeter before use (should be 3.7-4.2V)
- ✅ Never leave charging unattended

**DON'T:**

- ❌ Buy ultra-cheap cells from unknown brands
- ❌ Charge above 4.2V per cell
- ❌ Discharge below 2.5V per cell
- ❌ Short-circuit terminals
- ❌ Puncture or disassemble cells

**Red flags:** Claimed capacity >3000mAh in 18650 size is fake. Real maximum is ~3500mAh.

### ESP32 - Verify Before Purchase

**What to check:**

- 30-pin version (not 36-pin or 38-pin)
- WiFi antenna visible on board (required for network features)
- USB chip: CP2102 or CH340 (both work, CH340 needs driver)
- Adequate pin labels (some cheap boards have tiny/missing labels)

**Common issues:**

- Clone boards may need CH340 driver installed
- Some boards ship without header pins (requires soldering)
- Verify pinout matches EMBER spec before ordering in bulk

### Motor Drivers - Which to Choose

**L9110S (Budget):**

- ✅ Cheaper ($1-2)
- ✅ Simple wiring
- ✅ HAL fully supports
- ❌ Lower efficiency
- ❌ Gets warm under load
- **Good for:** v0.1, learning, budget builds

**TB6612FNG (Recommended):**

- ✅ Better efficiency
- ✅ Higher current (1.2A continuous)
- ✅ Standby mode (power saving)
- ✅ HAL fully supports
- ❌ More expensive ($3-5)
- ❌ Slightly more complex wiring
- **Good for:** v0.2+, serious experiments, long runtime

**DRV8833 (Premium):**

- ✅ Best efficiency
- ✅ Tiny footprint
- ❌ Most expensive ($5-8)
- ❌ Not yet in HAL (contribution welcome!)
- **Good for:** Advanced builds, custom PCB

---

## Verification Checklist Before Building

Before starting assembly, verify you have:

### Critical Components

```
□ ESP32 DevKit (30-pin, WiFi antenna visible)
□ Buck converter (adjustable, tested at 5.0V)
□ 2× 18650 batteries (protected, tested >3.7V each)
□ TP4056 charger (2S version for two cells)
□ 2× LDR (tested: resistance changes with light)
□ 1× RGB LED (common cathode verified with multimeter)
□ Motor driver (L9110S or TB6612FNG)
□ Resistors: 2× 10kΩ, 3× 220Ω
```

### Tools Ready

```
□ Soldering iron (tested, temperature controlled)
□ Multimeter (calibrated, fresh battery)
□ Wire strippers
□ Flush cutters
□ Hot glue gun
□ Safety glasses
```

### Software Ready

```
□ Arduino IDE installed
□ ESP32 board support installed
□ USB cable tested (data, not just power)
□ Driver installed (if using CH340 chip)
□ EMBER code downloaded
```

### Network Setup Ready (Optional but Recommended)

```
□ WiFi SSID and password known
□ Router allows mDNS/Bonjour
□ Computer on same 2.4GHz network
□ Bonjour installed (Windows) or avahi running (Linux)
```

---

## Cost Optimization Strategies

### Minimum Viable Bot (~$20)

**Skip:**

- Motors and wheels (add later for v0.2)
- Ultrasonic sensor (not used in v0.1)
- Chassis (use cardboard)
- Protoboard (breadboard only)

**Cheap alternatives:**

- Generic ESP32 from AliExpress
- Basic buck converter
- Salvaged LDRs from old electronics
- RGB LED from dead Christmas lights
- Old phone batteries instead of 18650 (dangerous - not recommended)

**Result:** Functional v0.1 bot for ~$15-20, but longer shipping and more reliability issues.

### Optimal Value Bot (~$25-30)

**Include everything:**

- All components from main list
- Motors (disabled in v0.1, ready for v0.2 OTA update)
- Quality components with warranty

**Result:** Reliable bot ready for v0.1 and v0.2, $25-30 per bot in bulk (9+).

### Premium Bot (~$50-70)

**Upgrades:**

- ESP32 Feather (better quality, different pinout)
- TB6612FNG motor driver
- Metal gear TT motors
- Name-brand batteries
- Professional laser-cut chassis
- All permanent soldering (no breadboard)

**Result:** Exhibition-quality bot, more durable, better for long-term experiments.

---

## Bulk Ordering Discounts

### Education/Makerspaces

**Contact these suppliers for education discounts:**

- Adafruit: 10% education discount
- SparkFun: Bulk/education pricing available
- Digi-Key: Educational institution accounts
- Mouser: Volume pricing

**Typical savings:** 10-20% on orders >$100

### Group Buys

**If building with others:**

1. Pool orders for 10+ bots
2. Buy 15-20× each component (extras for failures)
3. Share shipping costs
4. Buy tools once, share among group

**Savings:** 30-40% compared to individual purchases

---

## What's Included in EMBER Repository

**You don't need to buy:**

- ✅ Code (free, MIT license)
- ✅ Chassis design files (DXF for laser cutting)
- ✅ Documentation
- ✅ Build guide
- ✅ Evolution guide
- ✅ Python monitoring scripts

**You do need to buy:**

- Hardware components (this list)
- Tools (if you don't have them)
- Materials for chassis

---

## Regional Sourcing Alternatives

### United States

- Amazon (fast, higher cost)
- Adafruit (quality, education-friendly)
- SparkFun (quality, great tutorials)
- Digi-Key, Mouser (professional, bulk)
- Micro Center (in-store pickup, good prices)

### Europe

- RS Components
- Farnell
- Conrad Electronic
- Local makerspaces/fab labs

### Asia-Pacific

- AliExpress (cheapest, slow)
- Taobao (China domestic)
- Element14 (Australia)
- Local electronics markets

### Everywhere

- eBay (hit or miss on quality)
- AliExpress (global shipping)
- Local university surplus stores (treasure hunt!)

---

## Sample Order (Complete Single Bot)

**For your first EMBER bot - ready to build in 1-2 days:**

### Amazon Cart Example

```
1. ESP32 DevKit 30-pin (HiLetgo) - $9
2. LDR 5-pack (Chanzon) - $6
3. RGB LED 10-pack (common cathode) - $6
4. Buck converter 3-pack (DAOKI) - $9
5. L9110S motor driver 2-pack - $7
6. TT motor + wheel kit (2 sets) - $10
7. 18650 batteries 2-pack + charger - $12
8. HC-SR04 ultrasonic - $5
9. Breadboard + jumper wire kit - $10
10. Resistor assortment - $9
11. Hookup wire spool - $8

Subtotal: $91 (covers first bot + extras for 2-3 more)
```

**What you can build with these purchases:**

- 1 complete bot now
- Components for 2-3 more bots (need more batteries, ESP32s)
- Resistors/wire for 10+ bots

---

## Next Steps After Ordering

1. **While waiting for parts:**
   - Read BUILD_GUIDE.md
   - Install Arduino IDE and ESP32 support
   - Set up WiFi network if using network features
   - Plan your workspace

2. **When parts arrive:**
   - Verify all components present
   - Test critical components (ESP32, buck converter, batteries)
   - Organize into kit boxes (one per bot)

3. **Before building:**
   - Review pin assignments in SPEC
   - Print out BUILD_GUIDE for reference
   - Prepare chassis (cut/print)
   - Set up soldering station

---

## Frequently Asked Questions

**Q: Can I use different batteries?**  
A: Yes, but maintain 7.4V (2S configuration). LiPo pouches work but need different charging.

**Q: Will regular AA batteries work?**  
A: Yes for testing, but runtime will be very short (4×AA = 6V, ~2-4 hours).

**Q: Can I skip the buck converter?**  
A: NO. ESP32 needs 5V regulated. Battery voltage (7.4V) will damage it.

**Q: Do I need both motor driver types?**  
A: No, choose one. L9110S is cheaper, TB6612FNG is better. HAL supports both.

**Q: Can I use a different ESP32 board?**  
A: Yes, but verify pinout. ESP32-CAM, Feather, etc. have different pins.

**Q: Where can I get the chassis cut?**  
A: Local makerspace, university fab lab, online laser cutting service, or 3D print it.

**Q: Can I build without soldering?**  
A: Yes initially (breadboard), but permanent builds require soldering for reliability.

**Q: What if a component arrives broken?**  
A: This is why we recommend buying extras and from retailers with good return policies.

---

## Summary

**For your first EMBER bot:**

- Budget: $20-25 (AliExpress, 4-6 week wait)
- Standard: $30-40 (Amazon, 1-2 day shipping)
- Premium: $50-70 (Adafruit/SparkFun, highest quality)

**For 9-bot swarm:**

- Budget: $170-230 ($19-26 per bot)
- Standard: $270-360 ($30-40 per bot)
- Premium: $450-630 ($50-70 per bot)

**Plus one-time tool cost:** $60-165

**Start building. Start evolving. Start discovering emergence in silicon.**

---

*EMBER v0.1 Parts List - HAL+OTA Edition*  
*Updated for network features and dual motor driver support*  
*Part of the Forge Theory Project*  
*MIT License*
