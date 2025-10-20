# Apple IIc Keyboard Adapter Firmware

ESP32-C3 firmware for USB and Bluetooth keyboard adapter replacing the AY-3600-PRO-KEY encoder.

## Features

- ✅ AY-3600 emulation module with full signal generation
- ✅ Comprehensive unit tests
- ✅ Configurable debouncing and key repeat
- 🚧 USB Host support (coming soon)
- 🚧 Bluetooth HID support (coming soon)

## Project Structure

```
firmware/
├── platformio.ini          # PlatformIO configuration
├── src/
│   ├── main.c             # Main application entry point
│   ├── ay3600_emulator.h  # AY-3600 emulator header
│   └── ay3600_emulator.c  # AY-3600 emulator implementation
└── test/
    └── test_ay3600/       # Unit tests for AY-3600 emulator
        └── test_ay3600.c
```

## Building

### Prerequisites

- [PlatformIO](https://platformio.org/) installed
- ESP-IDF framework (automatically installed by PlatformIO)

### Build Commands

```bash
# Build for ESP32-C3
cd firmware
pio run

# Upload to ESP32-C3
pio run --target upload

# Monitor serial output
pio device monitor
```

## Testing

### Run Unit Tests

```bash
# Run tests on native platform (fast, no hardware needed)
cd firmware
pio test -e native

# Run tests on ESP32-C3 hardware
pio test -e esp32c3
```

### Test Coverage

The test suite includes:
- ✅ Initialization validation
- ✅ Key press/release handling
- ✅ Modifier key support (Control, Shift)
- ✅ Event handling
- ✅ Output signal verification
- ✅ State machine transitions
- ✅ Statistics tracking
- ✅ Edge case handling

## AY-3600 Emulator Module

### Overview

The `ay3600_emulator` module provides software emulation of the original AY-3600-PRO-KEY keyboard encoder chip.

### Key Features

- **Signal Generation**: D0-D4 (5-bit key codes), CONTROL, SHIFT, ANY-KEY, KSTRB
- **Debouncing**: Configurable debounce time (default 20ms)
- **Key Repeat**: Configurable initial delay (500ms) and repeat rate (50ms)
- **State Machine**: Proper state transitions for idle, debounce, pressed, and repeating states
- **Statistics**: Tracking for keypresses, repeats, and debounce events

### Usage Example

```c
#include "ay3600_emulator.h"

// Define output callback
void my_output_callback(const ay3600_output_t *output) {
    // Set GPIO pins based on output signals
    gpio_set_level(PIN_D0, (output->key_code >> 0) & 1);
    gpio_set_level(PIN_D1, (output->key_code >> 1) & 1);
    // ... etc
}

// Initialize emulator
ay3600_config_t config = {
    .output_callback = my_output_callback,
    .debounce_ms = 20,
    .repeat_delay_ms = 500,
    .repeat_rate_ms = 50,
};
ay3600_init(&config);

// In main loop
while (1) {
    // Process any pending events (debouncing, repeat timing)
    ay3600_process();

    // Handle keyboard input from USB/BT/matrix
    if (key_pressed) {
        ay3600_press_key(key_code, ctrl_pressed, shift_pressed);
    }
    if (key_released) {
        ay3600_release_key();
    }

    vTaskDelay(pdMS_TO_TICKS(1)); // 1ms tick
}
```

## GPIO Pin Assignment

| ESP32-C3 GPIO | Signal | Function |
|---------------|--------|----------|
| GPIO 0 | D0 | Key code bit 0 |
| GPIO 1 | D1 | Key code bit 1 |
| GPIO 2 | D2 | Key code bit 2 |
| GPIO 3 | D3 | Key code bit 3 |
| GPIO 4 | D4 | Key code bit 4 |
| GPIO 5 | CONTROL | Control key state |
| GPIO 6 | SHIFT | Shift key state |
| GPIO 7 | ANY-KEY | Any key pressed flag |
| GPIO 8 | KSTRB | Keyboard strobe pulse |

**Note:** These GPIO outputs are 3.3V. Level shifters (e.g., 74LVC245) are required to convert to 5V TTL for the Apple IIc.

## Configuration

Edit `platformio.ini` to customize:

```ini
[env:esp32c3]
build_flags =
    -DCONFIG_LOG_DEFAULT_LEVEL_DEBUG  # Log level
    -DAY3600_DEBOUNCE_MS=20            # Debounce time
    -DAY3600_REPEAT_DELAY_MS=500       # Initial repeat delay
    -DAY3600_REPEAT_RATE_MS=50         # Repeat rate
```

## Troubleshooting

### Build Errors

**Error:** `Cannot find esp-idf`
- Solution: Run `pio pkg install -e esp32c3` to install ESP-IDF framework

**Error:** `GPIO not defined`
- Solution: Check that GPIO numbers match your hardware setup in `main.c`

### Runtime Issues

**Problem:** Keys not registering
- Check level shifter connections (3.3V to 5V)
- Verify GPIO pin assignments
- Enable debug logging: `idf.py menuconfig` → Component config → Log output → Set to Debug

**Problem:** Keys repeating too fast/slow
- Adjust `repeat_delay_ms` and `repeat_rate_ms` in `ay3600_init()` config

## Contributing

1. Write unit tests for new features
2. Run all tests: `pio test -e native`
3. Format code: Follow ESP-IDF style guide
4. Document API changes in header files

## License

MIT License - See LICENSE file in repository root.
