# Getting Started Guide

Complete step-by-step instructions for building and installing the Apple IIc USB/Bluetooth Keyboard Adapter.

---

## Table of Contents

1. [What You'll Need](#what-youll-need)
2. [Step 1: Order Components](#step-1-order-components)
3. [Step 2: Set Up Development Environment](#step-2-set-up-development-environment)
4. [Step 3: Build the Firmware](#step-3-build-the-firmware)
5. [Step 4: Flash the ESP32-C3](#step-4-flash-the-esp32-c3)
6. [Step 5: Wire the Hardware](#step-5-wire-the-hardware)
7. [Step 6: Test Before Installation](#step-6-test-before-installation)
8. [Step 7: Install in Apple IIc](#step-7-install-in-apple-iic)
9. [Troubleshooting](#troubleshooting)

---

## What You'll Need

### Required Tools

- Computer with USB port (for programming ESP32)
- Soldering iron (optional, for header pins)
- Multimeter (for testing voltages)
- Small screwdrivers (for opening Apple IIc case)

### Skills Required

- **Beginner friendly!** No programming required to use pre-built firmware
- Basic electronics knowledge helpful
- Ability to follow wiring diagrams

### Time Estimate

- First-time build: **2-3 hours**
- With experience: **30-60 minutes**

---

## Step 1: Order Components

### Shopping List (USB Only - $24)

| Component | Qty | Price | Where to Buy |
|-----------|-----|-------|--------------|
| ESP32-C3-DevKitM-1 | 1 | $4 | [AliExpress](https://www.aliexpress.com/w/wholesale-esp32-c3-devkitm-1.html), [Amazon](https://www.amazon.com/s?k=esp32-c3-devkitm-1) |
| Adafruit TXB0108 Level Shifter Breakout | 2 | $16 | [Adafruit #4245](https://www.adafruit.com/product/4245), [Amazon](https://www.amazon.com/s?k=TXB0108+breakout) |
| USB-A Female Breakout Board | 1 | $2 | [Amazon](https://www.amazon.com/s?k=usb-a+female+breakout), [AliExpress](https://www.aliexpress.com/w/wholesale-usb-a-breakout.html) |
| Breadboard (830 tie-points) | 1 | $2 | Any electronics supplier |
| Jumper Wires (M-M, M-F) | 1 pack | $2 | Any electronics supplier |
| **Total** | - | **~$24** | - |

### Optional Components

- **Prototype PCB** ($3) - For permanent installation
- **Header pins** ($2) - If your boards don't have them pre-soldered
- **Enclosure** ($5-10) - 3D printed or project box
- **Logic analyzer** ($10-20) - For debugging (optional but helpful)

### Wait Time

- **Domestic shipping:** 3-7 days
- **International (AliExpress):** 2-4 weeks (but cheaper!)

**💡 Pro Tip:** Order from multiple suppliers to get parts faster, or pay for expedited shipping if you're impatient!

---

## Step 2: Set Up Development Environment

### Option A: PlatformIO (Recommended)

PlatformIO provides the easiest setup with automatic dependency management.

**Install via pip:**
```bash
pip install platformio
```

**Or install VS Code extension:**
1. Open VS Code
2. Go to Extensions (Ctrl+Shift+X)
3. Search for "PlatformIO IDE"
4. Click Install
5. Restart VS Code

### Option B: Arduino IDE (Alternative)

If you prefer Arduino IDE:
1. Download from https://www.arduino.cc/en/software
2. Install ESP32 board support
3. Install required libraries (see firmware/README.md)

### Verify Installation

```bash
# Check PlatformIO version
pio --version

# Should see something like: PlatformIO Core, version 6.x.x
```

---

## Step 3: Build the Firmware

### Clone the Repository

```bash
# Clone from GitHub
git clone https://github.com/badvision/apple-iic-keyboard-adapter.git
cd apple-iic-keyboard-adapter/firmware
```

### Run Tests (Optional but Recommended)

```bash
# Run unit tests on your computer (no hardware needed!)
pio test -e native

# You should see: "17 Tests 0 Failures 0 Ignored"
```

### Build the Firmware

```bash
# Build for ESP32-C3
pio run -e esp32c3

# Look for: "SUCCESS" at the end
```

The compiled firmware will be in `.pio/build/esp32c3/firmware.bin`

### Pre-Built Firmware (Coming Soon)

Don't want to build? Pre-compiled firmware will be available in GitHub Releases.

---

## Step 4: Flash the ESP32-C3

### Connect ESP32-C3 to Computer

1. Plug ESP32-C3 into computer via USB-C cable
2. The board should be recognized as a serial device

**Check device detection:**

**macOS/Linux:**
```bash
ls /dev/tty.usb* # or /dev/ttyUSB*
```

**Windows:**
- Open Device Manager
- Look under "Ports (COM & LPT)"
- Note the COM port number (e.g., COM3)

### Flash the Firmware

```bash
# PlatformIO will auto-detect the port
pio run -e esp32c3 --target upload

# Or specify port manually
pio run -e esp32c3 --target upload --upload-port /dev/tty.usbserial-XXXXX
```

### Verify Flash

```bash
# Open serial monitor to see boot messages
pio device monitor

# You should see:
# "Apple IIc Keyboard Adapter starting..."
# "GPIO initialized"
# "AY3600 emulator initialized"
```

Press Ctrl+C to exit the monitor.

---

## Step 5: Wire the Hardware

### Breadboard Wiring Diagram

```
ESP32-C3          TXB0108 #1        TXB0108 #2        Apple IIc
                  (8-channel)       (1-channel)       Connector
------------------------------------------------------------------------
3.3V      ─────>  VA                VA
GND       ─────>  GND               GND               GND
          ┌────>  VB                VB        ─────>  +5V
          │
5V source ┘

GPIO 0    ─────>  A1 ───> B1  ──────────────────────> D0 (pin 1)
GPIO 1    ─────>  A2 ───> B2  ──────────────────────> D1 (pin 2)
GPIO 2    ─────>  A3 ───> B3  ──────────────────────> D2 (pin 3)
GPIO 3    ─────>  A4 ───> B4  ──────────────────────> D3 (pin 4)
GPIO 4    ─────>  A5 ───> B5  ──────────────────────> D4 (pin 5)
GPIO 5    ─────>  A6 ───> B6  ──────────────────────> CONTROL (pin 6)
GPIO 6    ─────>  A7 ───> B7  ──────────────────────> SHIFT (pin 7)
GPIO 7    ─────>  A8 ───> B8  ──────────────────────> ANY-KEY (pin 8)

GPIO 8    ─────>  A1 ───> B1  ──────────────────────> KSTRB (pin 9)

USB D+/D- ─────>  [USB-A Breakout] ────> USB Keyboard
```

### Step-by-Step Wiring

1. **Insert components into breadboard**
   - ESP32-C3 on one side
   - Both TXB0108 boards in the middle
   - Leave space for jumper wires

2. **Connect power rails**
   - ESP32-C3 3.3V → TXB0108 VA pins
   - ESP32-C3 GND → TXB0108 GND
   - 5V source → TXB0108 VB pins (both boards)

3. **Connect GPIO pins (ESP32 → TXB0108 A-side)**
   - Use the pinout table above
   - Double-check each connection!

4. **Connect to Apple IIc (TXB0108 B-side → Apple IIc)**
   - You'll need to identify the keyboard connector pins
   - See `docs/manuals/` for pin diagrams

5. **Connect USB port**
   - USB-A breakout D+ → ESP32 GPIO 18
   - USB-A breakout D- → ESP32 GPIO 19
   - USB-A VBUS → 5V (with protection)

### Power Supply Notes

**Option 1: Power from Apple IIc**
- Tap 5V from the keyboard connector
- Add 3.3V LDO regulator (AMS1117-3.3)
- Most integrated approach

**Option 2: External power**
- Power ESP32 via USB
- Level shifters need 5V from somewhere
- Less elegant but easier for testing

---

## Step 6: Test Before Installation

### Visual Inspection

- [ ] Check all connections with multimeter
- [ ] Verify no short circuits
- [ ] Confirm 3.3V at ESP32
- [ ] Confirm 5V at level shifter outputs

### Signal Testing

**With a logic analyzer or oscilloscope:**

1. Power on the adapter
2. Connect logic analyzer to D0-D4, KSTRB
3. Look for test pattern in serial output
4. Verify 5V TTL levels on output

**Without test equipment:**

Just check that:
- ESP32 boots (check serial output)
- No magic smoke 💨
- Power LED on ESP32 lights up

---

## Step 7: Install in Apple IIc

### Before You Begin

**⚠️ WARNING: Improper installation can damage your Apple IIc!**

- Discharge static electricity (touch metal case)
- Work on non-conductive surface
- Take photos before disconnecting anything
- Label all connections

### Opening the Apple IIc

1. **Unplug everything** from the Apple IIc
2. **Remove the case screws** (usually 4-6 screws on bottom)
3. **Carefully lift the top case**
4. **Locate the AY-3600 chip** (see manual photos in `docs/manuals/`)

### Installation Options

**Option A: Quick Test (Reversible)**
- Disconnect keyboard ribbon cable from motherboard
- Connect your adapter instead
- Test functionality
- Easy to reverse if problems occur

**Option B: Permanent Replacement**
- Remove the AY-3600 chip from motherboard
- Socket the replacement adapter
- Requires desoldering (advanced!)
- Route USB port through case opening

### First Boot

1. Connect adapter to keyboard port
2. Plug in USB keyboard
3. Power on Apple IIc
4. Try typing in BASIC prompt
5. Test all keys, modifiers, special keys

**Expected behavior:**
- Keys should register immediately
- Auto-repeat after 500ms
- Control and Shift work correctly

---

## Troubleshooting

### No Response from Keyboard

**Check:**
- Power LED on ESP32 (should be lit)
- Serial monitor for error messages
- Level shifter connections
- 5V supply voltage

**Try:**
- Replug USB keyboard
- Power cycle everything
- Check firmware flashed correctly

### Keys Repeat Too Fast/Slow

**Solution:**
Edit `firmware/src/main.c` and adjust:
```c
ay3600_config_t config = {
    .debounce_ms = 20,
    .repeat_delay_ms = 500,  // Change this
    .repeat_rate_ms = 50,     // Or this
};
```

Rebuild and re-flash firmware.

### Some Keys Don't Work

**Possible causes:**
- Key mapping incomplete (USB/BT support in progress)
- Hardware connection issue
- Level shifter problem

**Debug:**
- Check which keys work/don't work
- Look for pattern (e.g., all in one row/column)
- Test with serial monitor logging enabled

### Apple IIc Shows Garbage Characters

**Likely causes:**
- Incorrect voltage levels (not 5V TTL)
- Timing issues with KSTRB
- Interference on data lines

**Fix:**
- Verify 5V on outputs with multimeter
- Check signal quality with scope
- Add decoupling capacitors (0.1µF)

### ESP32 Won't Boot

**Check:**
- USB cable is data-capable (not power-only)
- ESP32 not in boot mode (BOOT button)
- Correct firmware for ESP32-C3 (not ESP32 or ESP32-S3)

### Need More Help?

- **Open an issue:** https://github.com/badvision/apple-iic-keyboard-adapter/issues
- **Check discussions:** https://github.com/badvision/apple-iic-keyboard-adapter/discussions
- **Read firmware docs:** `firmware/README.md`

---

## Next Steps

Once your adapter is working:

1. **Share your build!** Post photos in Discussions
2. **Test different keyboards** and report compatibility
3. **Design a PCB** for cleaner installation
4. **Contribute improvements** to the project

**Congratulations! You now have a modern keyboard on your Apple IIc! 🎉**

---

## Additional Resources

- [Hardware Design Details](DESIGN.md)
- [Firmware Documentation](../firmware/README.md)
- [Apple IIc Manual Excerpts](manuals/)
- [Contributing Guide](../CONTRIBUTING.md)
