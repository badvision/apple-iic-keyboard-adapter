# Apple IIc Technical Reference Manual - Keyboard Section

This directory contains screenshots from the Apple IIc Technical Reference Manual, specifically Section 11.7: "The Keyboard" which documents the AY-3600-PRO-KEY keyboard encoder chip.

## Contents

### Page 1: Section Introduction (page 237, n270)
**File:** `apple-iic-section-11.7-page1.png`

This page introduces Section 11.7 and describes:
- The keyboard as an 18×6 matrix of keyswitches
- Connection via ribbon cable and 16-pin connector
- AY-3600 type keyboard decoder
- ROM timing diagram showing signal relationships

**Key Quote:**
> "The Apple IIc's keyboard is a matrix of keyswitches connected to an AY-3600 type keyboard decoder via a ribbon cable and a 16-pin connector."

### Page 2: Keyboard Circuit Diagram (page 238, n271)
**File:** `apple-iic-section-11.7-page2.png`

**Figure 11.16: Keyboard Circuit Diagram**

Shows the complete circuit including:
- 18×6 Keyboard Grid (X0-X17 columns, Y0-Y5 rows)
- **AY-3600-PRO-KEY** chip (the main encoder IC)
- CONTROL signal
- Keyboard Address Bus connections
- KSTRB (Keyboard Strobe) signal
- Connection to 2716 ROM for character translation
- 16 x 8 Coordinates table

This is the most important diagram showing exactly how the AY-3600 interfaces with:
- The keyboard matrix (input)
- The IOU/MMU (output via KSTRB and data lines)
- The character ROM (for ASCII translation)

### Page 3: Signal Timing (page 239, n272)
**File:** `apple-iic-section-11.7-page3.png`

Describes the operational behavior:
- Signal timing and relationships
- Key scan operation
- CONTROL and SHIFT handling
- ANY-KEY-DOWN signal generation
- KSTRB pulse generation
- Connection to IOU soft switches
- ROM character code translation

**Figure 11.17: Keyboard Signals**

Timing diagram showing:
- Key pressed event
- Key released event
- Decode timing (when key code is valid)
- KSTRB pulse timing and duration
- ANY-KEY signal relationship

**Key Technical Details:**
- Key codes are 5 bits (D0-D4) = 32 possible values
- CONTROL and SHIFT are separate signals
- KSTRB pulses on each key press/repeat
- ANY-KEY-DOWN signal indicates keyboard activity
- Reading location $C000 retrieves the character code

## Source

These images are from:
- **Title:** Apple IIc Technical Reference Manual
- **Publisher:** Apple Computer Inc.
- **Date:** 1985
- **Archive:** Internet Archive
- **URL:** https://archive.org/details/a2ctrm/Apple2cTechRef01
- **Pages:** 237-239 (archive notation: n270-n272)

## How to Use These Images

1. **Hardware Design:**
   - Reference the circuit diagram to understand signal connections
   - Match pin assignments when designing replacement hardware
   - Verify signal levels (5V TTL)

2. **Software Emulation:**
   - Study timing diagrams for correct signal sequencing
   - Implement debouncing based on documented behavior
   - Match key repeat timing characteristics

3. **Testing:**
   - Compare emulator output to documented signals
   - Verify KSTRB pulse timing (~1μs)
   - Test with logic analyzer against reference waveforms

## Related Documentation

- **Complete manual section:** See archive.org link above
- **Circuit analysis:** See `../DESIGN.md` for detailed breakdown
- **Emulator implementation:** See `../../firmware/src/ay3600_emulator.c`

## Copyright Notice

These images are included for technical reference and educational purposes under fair use. The Apple IIc Technical Reference Manual is a historical document (1985) for a discontinued product. Original copyright: Apple Computer Inc.

For preservation and research purposes, the complete manual is available at Internet Archive.
