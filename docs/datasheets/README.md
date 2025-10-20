# AY-3600 Datasheet Information

## About the AY-3600-PRO-KEY

The AY-3600-PRO-KEY is a keyboard encoder IC manufactured by General Instrument (GI) in the late 1970s/early 1980s. It was used in various Apple II computers including the Apple II, II+, IIe, and IIc.

### Known Specifications

Based on the Apple IIc Technical Reference Manual (Section 11.7):

**Function:**
- Keyboard matrix scanner and encoder
- Scans an 18×6 keyboard matrix
- Outputs 5-bit key codes (D0-D4)
- Provides CONTROL, SHIFT modifier outputs
- Generates ANY-KEY and KSTRB (keyboard strobe) signals

**Interface:**
- Input: 18×6 passive keyboard matrix
- Output:
  - D0-D4: 5-bit encoded key value (0-31)
  - CONTROL: Control key state
  - SHIFT: Shift key state
  - ANY-KEY: High when any key is pressed
  - KSTRB: Pulses on key press/repeat

**Power:**
- 5V TTL logic levels
- Power consumption: ~100mA (estimated)

**Features:**
- Built-in key debouncing
- Automatic key repeat after delay
- Two-key rollover support

### Datasheet Availability

**Unfortunately, the original AY-3600 datasheet appears to be lost to time.**

General Instrument was acquired by Microchip Technology, and the AY-3600 line was discontinued decades ago. Extensive searches of:
- Bitsavers.org (major datasheet archive)
- Internet Archive
- Vintage computing forums
- Microchip archives

...have not yielded a complete datasheet.

### Alternative Documentation Sources

1. **Apple IIc Technical Reference Manual** (best source)
   - Section 11.7: "The Keyboard" (pages 237-240)
   - Includes circuit diagram showing AY-3600 connections
   - Describes signal behavior and timing
   - Available at: https://archive.org/details/a2ctrm/Apple2cTechRef01

2. **Apple II Reference Manual** (1979)
   - Contains similar but less detailed information
   - Available at: https://archive.org/details/apple-ii-reference-manual-1979

3. **Community Reverse Engineering**
   - Various projects have reverse-engineered the AY-3600 behavior
   - See: https://github.com/topics/apple-ii-keyboard
   - AppleWin emulator source code

### Our Emulation Approach

Since the datasheet is unavailable, this project emulates the AY-3600 based on:

1. **Observed behavior** from the Apple IIc Technical Reference Manual
2. **Signal timing** requirements from the IOU (Input/Output Unit) chip
3. **Testing** with actual Apple IIc hardware

The emulator in `firmware/src/ay3600_emulator.c` provides functionally equivalent behavior for use with modern keyboards.

## Contributing

If you have access to an original AY-3600 datasheet or additional technical documentation, please:
1. Open an issue on this repository
2. Contact the maintainer
3. Consider donating it to Bitsavers.org for preservation

## References

- Apple IIc Technical Reference Manual: Section 11.7
- Understanding the Apple II, by Jim Sather (1983)
- Apple II Circuit Description, by Winston Gayler (1983)
