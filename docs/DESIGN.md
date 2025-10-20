# Apple IIc USB/Bluetooth Keyboard Adapter
## Proposal for AY-3600 Replacement using ESP32-C3

---

## Executive Summary

Replace the Apple IIc's AY-3600-PRO-KEY keyboard encoder with a modern ESP32-C3 microcontroller to add USB and Bluetooth keyboard support while maintaining compatibility with the original keyboard.

**Total Cost:** ~$20
**Assembly Difficulty:** Easy (no SMD soldering required)
**Features:** USB keyboards, Bluetooth keyboards, optional WiFi features

---

## System Architecture

```mermaid
graph TB
    subgraph "Input Devices"
        USB[USB Keyboard]
        BT[Bluetooth Keyboard]
        ORIG[Original IIc Keyboard<br/>18x6 Matrix<br/><i>optional fallback</i>]
    end

    subgraph "ESP32-C3 Module"
        USBH[USB Host Controller]
        BTH[Bluetooth LE Stack]
        SCAN[Matrix Scanner<br/><i>optional</i>]
        PARSE[HID Parser]
        TRANS[Key Translator]
        EMU[AY-3600 Emulator]

        USB --> USBH
        BT --> BTH
        ORIG -.-> SCAN

        USBH --> PARSE
        BTH --> PARSE
        SCAN -.-> PARSE

        PARSE --> TRANS
        TRANS --> EMU
    end

    subgraph "Level Shifting"
        LS1[TXB0108 Module #1<br/>8 channels]
        LS2[TXB0108 Module #2<br/>1 channel]
    end

    subgraph "Apple IIc Motherboard"
        IOU[IOU Chip<br/>Input/Output Unit]
        ROM[2516 ROM<br/><i>not used with ESP32</i>]
        MMU[Memory Management]
    end

    EMU -->|GPIO 0-7| LS1
    EMU -->|GPIO 8| LS2

    LS1 -->|D0-D4<br/>CONTROL<br/>SHIFT<br/>ANY-KEY| IOU
    LS2 -->|KSTRB| IOU

    IOU --> MMU
    MMU -->|$C000| CPU[65C02 CPU]

    style USB fill:#e1f5ff
    style BT fill:#e1f5ff
    style ORIG fill:#fff4e1
    style EMU fill:#ffe1e1
    style IOU fill:#e1ffe1
```

---

## Hardware Design

### Component Selection

```mermaid
graph LR
    subgraph "Main Controller"
        ESP[ESP32-C3-DevKitM-1<br/>$4<br/>RISC-V 160MHz<br/>USB OTG + BT 5.0]
    end

    subgraph "Level Shifters"
        LS1[Adafruit TXB0108<br/>Breakout #1<br/>$7.95<br/>8-channel 3.3V↔5V]
        LS2[Adafruit TXB0108<br/>Breakout #2<br/>$7.95<br/>8-channel 3.3V↔5V<br/><i>only use 1 channel</i>]
    end

    subgraph "Accessories"
        USB_PORT[USB-A Receptacle<br/>Breakout<br/>$2]
        WIRE[Jumper Wires<br/>$2]
        PROTO[Prototype Board<br/>$3<br/><i>optional</i>]
    end

    ESP --> LS1
    ESP --> LS2
    USB_PORT --> ESP
```

### Bill of Materials

| Component | Qty | Unit Price | Total | Source | Part Number |
|-----------|-----|------------|-------|--------|-------------|
| ESP32-C3-DevKitM-1 | 1 | $4.00 | $4.00 | AliExpress/Espressif | ESP32-C3-DevKitM-1 |
| Adafruit TXB0108 Breakout | 2 | $7.95 | $15.90 | Adafruit | #4245 |
| USB-A Breakout Board | 1 | $2.00 | $2.00 | Amazon/AliExpress | Generic |
| Jumper Wires M-F | 20 | $0.10 | $2.00 | Amazon | Generic |
| **TOTAL** | - | - | **$23.90** | - | - |

**Optional:**
- Prototype PCB: $3
- Header pins: $2
- Enclosure: $5-10

---

## Signal Interface

### AY-3600 Output Signals (to be emulated)

```mermaid
graph LR
    subgraph "ESP32-C3 GPIO Outputs"
        G0[GPIO 0]
        G1[GPIO 1]
        G2[GPIO 2]
        G3[GPIO 3]
        G4[GPIO 4]
        G5[GPIO 5]
        G6[GPIO 6]
        G7[GPIO 7]
        G8[GPIO 8]
    end

    subgraph "Level Shifters"
        LS1[TXB0108 #1<br/>3.3V → 5V]
        LS2[TXB0108 #2<br/>3.3V → 5V]
    end

    subgraph "Apple IIc Signals (5V TTL)"
        D0[D0 - Key Code Bit 0]
        D1[D1 - Key Code Bit 1]
        D2[D2 - Key Code Bit 2]
        D3[D3 - Key Code Bit 3]
        D4[D4 - Key Code Bit 4]
        CTRL[CONTROL - Ctrl Key]
        SHFT[SHIFT - Shift Key]
        ANY[ANY-KEY - Key Pressed]
        KSTR[KSTRB - Keyboard Strobe]
    end

    G0 --> LS1 --> D0
    G1 --> LS1 --> D1
    G2 --> LS1 --> D2
    G3 --> LS1 --> D3
    G4 --> LS1 --> D4
    G5 --> LS1 --> CTRL
    G6 --> LS1 --> SHFT
    G7 --> LS1 --> ANY
    G8 --> LS2 --> KSTR
```

### Pin Assignment Table

| ESP32-C3 GPIO | Signal Name | Direction | Apple IIc Function | 5V Level |
|---------------|-------------|-----------|-------------------|----------|
| GPIO 0 | D0 | Output | Key code bit 0 | Yes (via LS) |
| GPIO 1 | D1 | Output | Key code bit 1 | Yes (via LS) |
| GPIO 2 | D2 | Output | Key code bit 2 | Yes (via LS) |
| GPIO 3 | D3 | Output | Key code bit 3 | Yes (via LS) |
| GPIO 4 | D4 | Output | Key code bit 4 | Yes (via LS) |
| GPIO 5 | CONTROL | Output | Control key state | Yes (via LS) |
| GPIO 6 | SHIFT | Output | Shift key state | Yes (via LS) |
| GPIO 7 | ANY-KEY | Output | Any key pressed flag | Yes (via LS) |
| GPIO 8 | KSTRB | Output | Keyboard strobe pulse | Yes (via LS) |
| GPIO 18 | USB D+ | I/O | USB Host (built-in) | 3.3V |
| GPIO 19 | USB D- | I/O | USB Host (built-in) | 3.3V |
| GPIO 10 | STATUS_LED | Output | Status indicator (optional) | 3.3V |

---

## Software Architecture

### Firmware Flow

```mermaid
flowchart TD
    START([Power On]) --> INIT[Initialize Hardware]
    INIT --> INIT_USB[Initialize USB Host]
    INIT_USB --> INIT_BT[Initialize Bluetooth LE]
    INIT_BT --> INIT_GPIO[Configure GPIO Outputs]
    INIT_GPIO --> LOOP{Main Loop}

    LOOP --> CHECK_USB{USB Keyboard<br/>Connected?}
    CHECK_USB -->|Yes| READ_USB[Read USB HID Report]
    CHECK_USB -->|No| CHECK_BT{Bluetooth<br/>Keyboard<br/>Connected?}

    CHECK_BT -->|Yes| READ_BT[Read BT HID Report]
    CHECK_BT -->|No| CHECK_MATRIX{Original<br/>Matrix<br/>Enabled?}

    CHECK_MATRIX -->|Yes| SCAN_MATRIX[Scan Keyboard Matrix]
    CHECK_MATRIX -->|No| DELAY

    READ_USB --> PARSE[Parse HID Report]
    READ_BT --> PARSE
    SCAN_MATRIX --> PARSE

    PARSE --> TRANSLATE[Translate to<br/>Apple Key Code]
    TRANSLATE --> SET_D[Set D0-D4 Lines<br/>5-bit key code]
    SET_D --> SET_MODS[Set CONTROL/SHIFT]
    SET_MODS --> SET_ANY[Set ANY-KEY High]
    SET_ANY --> PULSE[Pulse KSTRB<br/>~1μs]
    PULSE --> WAIT[Wait for Key Release]
    WAIT --> CLEAR[Clear ANY-KEY]
    CLEAR --> DELAY[Delay 1ms]
    DELAY --> LOOP

    style READ_USB fill:#e1f5ff
    style READ_BT fill:#e1f5ff
    style SCAN_MATRIX fill:#fff4e1
    style PULSE fill:#ffe1e1
```

### Key Translation Example

```mermaid
graph LR
    subgraph "USB HID Scancode"
        USB_A[HID: 0x04<br/>'A' key]
        USB_CTRL[HID: 0xE0<br/>Left Control]
    end

    subgraph "Translation Table"
        LUT[Lookup Table<br/>USB → Apple IIc]
    end

    subgraph "Apple IIc Key Code"
        APPLE_A[Apple: 0x00<br/>'A' key]
        APPLE_CTRL[CONTROL flag<br/>separate signal]
    end

    subgraph "GPIO Output"
        D_LINES[D0-D4: 0x00<br/>CONTROL: HIGH<br/>SHIFT: LOW<br/>ANY-KEY: HIGH]
    end

    USB_A --> LUT --> APPLE_A --> D_LINES
    USB_CTRL --> LUT --> APPLE_CTRL --> D_LINES
```

---

## Implementation Phases

```mermaid
gantt
    title Implementation Timeline
    dateFormat YYYY-MM-DD
    section Phase 1
    Acquire Components           :p1a, 2024-01-01, 7d
    Basic GPIO Test             :p1b, after p1a, 3d
    Verify 5V Interface         :p1c, after p1b, 2d
    section Phase 2
    USB Host Setup              :p2a, after p1c, 5d
    HID Parser Implementation   :p2b, after p2a, 7d
    Key Translation Table       :p2c, after p2b, 3d
    section Phase 3
    Bluetooth Stack Setup       :p3a, after p2c, 7d
    BT HID Integration         :p3b, after p3a, 5d
    Multi-Input Priority        :p3c, after p3b, 3d
    section Phase 4
    Final Testing              :p4a, after p3c, 7d
    Documentation              :p4b, after p3c, 5d
    Optional PCB Design        :p4c, after p4a, 14d
```

### Phase 1: Proof of Concept (Week 1-2)

**Goal:** Verify hardware interface works

**Tasks:**
1. Wire ESP32-C3 to TXB0108 level shifters
2. Connect level shifters to Apple IIc keyboard connector
3. Write test firmware to output fixed key codes
4. Verify Apple IIc reads keys correctly from $C000

**Success Criteria:**
- ESP32 can set D0-D4, CONTROL, SHIFT, ANY-KEY
- KSTRB pulse detected by IOU
- Reading $C000 returns expected key code

### Phase 2: USB Keyboard Support (Week 3-4)

**Goal:** Connect and use USB keyboards

**Tasks:**
1. Configure ESP-IDF USB Host stack
2. Enumerate USB HID keyboards
3. Parse USB HID reports
4. Build USB scancode → Apple IIc translation table
5. Test with multiple USB keyboards

**Success Criteria:**
- USB keyboard detected on plug-in
- All keys generate correct Apple IIc codes
- Modifier keys (Ctrl, Shift) work correctly
- Key repeat functions properly

### Phase 3: Bluetooth Support (Week 5-6)

**Goal:** Add wireless keyboard support

**Tasks:**
1. Initialize Bluetooth LE stack
2. Implement HID-over-GATT profile
3. Add pairing/bonding support
4. Parse Bluetooth HID reports
5. Test with multiple BT keyboards

**Success Criteria:**
- BT keyboard pairs successfully
- All keys work wirelessly
- Reconnection after power cycle works
- Both USB and BT keyboards can be switched

### Phase 4: Polish & Enhancement (Week 7-8)

**Goal:** Production-ready device

**Tasks:**
1. Add status LED indicators (USB/BT/error)
2. Implement configuration storage (key remapping)
3. Add WiFi config webpage (optional)
4. Create user documentation
5. Design permanent PCB (optional)

**Success Criteria:**
- Reliable operation over extended testing
- User-friendly status feedback
- Complete documentation
- Optional: Professional PCB design

---

## Physical Installation Options

```mermaid
graph TB
    subgraph "Option A: External Adapter"
        EXT_CONN[Intercept Ribbon Cable<br/>Outside Case]
        EXT_BOARD[Adapter Board in<br/>External Enclosure]
        EXT_USB[USB Port Accessible<br/>No Case Modification]
    end

    subgraph "Option B: Internal Replacement"
        INT_REMOVE[Remove Original<br/>AY-3600 Chip]
        INT_MOUNT[Mount ESP32 Module<br/>in Original Location]
        INT_DRILL[Drill Hole for<br/>USB Port]
    end

    subgraph "Option C: Hybrid Design"
        HYB_BOARD[Small PCB Replaces<br/>AY-3600]
        HYB_CABLE[USB Port on<br/>Flying Lead]
        HYB_MOUNT[No Case Drilling<br/>Route Cable Out]
    end

    INSTALL{Choose Installation} --> EXT_CONN
    INSTALL --> INT_REMOVE
    INSTALL --> HYB_BOARD
```

### Recommended: Option C (Hybrid)

**Advantages:**
- No permanent case modification
- Reversible installation
- Professional appearance
- USB port accessible through ventilation slots

**Implementation:**
1. Design PCB to match AY-3600 footprint (or use proto board)
2. Ribbon cable connects to original keyboard connector
3. USB port on 6-inch flying lead
4. Route cable through existing case openings
5. Use velcro/mounting tape to secure USB port

---

## Testing Procedures

### Hardware Validation Tests

```mermaid
flowchart LR
    subgraph "Power Tests"
        T1[3.3V Rail Stable]
        T2[5V Rail Stable]
        T3[Current Draw OK]
    end

    subgraph "Signal Tests"
        T4[GPIO Output Levels<br/>3.3V @ ESP32]
        T5[Level Shifted Levels<br/>5V @ Apple IIc]
        T6[KSTRB Pulse Width<br/>~1μs]
    end

    subgraph "Interface Tests"
        T7[Read $C000<br/>Returns Key Code]
        T8[ANY-KEY Detected<br/>by IOU]
        T9[CONTROL/SHIFT<br/>Detected]
    end

    START([Begin Testing]) --> T1
    T1 --> T2 --> T3 --> T4 --> T5 --> T6 --> T7 --> T8 --> T9 --> PASS([Pass])
```

### Functional Tests

| Test Case | Expected Behavior | Pass/Fail |
|-----------|-------------------|-----------|
| USB keyboard plug-in | Detected within 2 seconds | ☐ |
| Type 'A' key | $C000 reads 0x00 (or correct code) | ☐ |
| Hold Shift + 'A' | SHIFT signal HIGH + 'A' code | ☐ |
| Hold Control + 'C' | CONTROL signal HIGH + 'C' code | ☐ |
| Key auto-repeat | Repeated key codes after 500ms | ☐ |
| BT keyboard pairing | Connects within 10 seconds | ☐ |
| BT keyboard typing | Same as USB keyboard | ☐ |
| Unplug USB keyboard | No spurious key events | ☐ |
| Power cycle | BT keyboard reconnects | ☐ |

---

## Cost Analysis

### Base Configuration (USB Only)

```mermaid
pie title Cost Breakdown - USB Only ($24)
    "ESP32-C3 Module" : 4
    "Level Shifters (2x)" : 16
    "USB Port Breakout" : 2
    "Wires & Misc" : 2
```

### Enhanced Configuration (USB + BT + PCB)

```mermaid
pie title Cost Breakdown - Full Featured ($32)
    "ESP32-C3 Module" : 4
    "Level Shifters (2x)" : 16
    "USB Port" : 2
    "Wires & Misc" : 2
    "Custom PCB" : 5
    "Enclosure" : 3
```

### Comparison vs. Alternatives

| Solution | Cost | USB | BT | Difficulty | Time |
|----------|------|-----|----|-----------||------|
| **This Project (Breadboard)** | $24 | ✅ | ✅ | Easy | 2 weeks |
| **This Project (PCB)** | $32 | ✅ | ✅ | Easy | 4 weeks |
| Original AY-3600 Repair | $50+ | ❌ | ❌ | Hard | N/A |
| Commercial USB Adapter | $80+ | ✅ | ❌ | N/A | N/A |
| Custom FPGA Solution | $100+ | ✅ | ❌ | Hard | 8 weeks |

---

## Risk Assessment & Mitigation

```mermaid
graph TD
    subgraph "Risks"
        R1[5V Signal Damage<br/>to ESP32]
        R2[Incorrect Timing<br/>IOU Won't Recognize]
        R3[Power Supply<br/>Issues]
        R4[USB Host Stack<br/>Compatibility]
    end

    subgraph "Mitigations"
        M1[Level Shifters<br/>Mandatory]
        M2[Logic Analyzer<br/>Verification]
        M3[Separate 3.3V<br/>Regulator]
        M4[Use ESP-IDF<br/>Tested Stack]
    end

    R1 --> M1
    R2 --> M2
    R3 --> M3
    R4 --> M4
```

| Risk | Probability | Impact | Mitigation Strategy |
|------|-------------|--------|---------------------|
| Damage ESP32 with 5V | Medium | High | Always use level shifters; verify voltages before connecting |
| IOU doesn't detect signals | Medium | High | Use logic analyzer; match original AY-3600 timing precisely |
| Insufficient power | Low | Medium | Add dedicated 3.3V regulator; monitor current draw |
| USB keyboard incompatibility | Low | Low | Test with multiple keyboards; use proven TinyUSB/ESP-IDF stack |
| BT pairing problems | Medium | Low | Implement standard HID-over-GATT; test with multiple devices |
| Mechanical fit issues | Low | Low | Prototype on breadboard first; measure carefully before PCB |

---

## Future Enhancements

### Software Features (WiFi Enabled)

```mermaid
mindmap
    root((Future Features))
        Network
            Telnet Keyboard Server
            VNC-style Input
            Network Config UI
            OTA Firmware Updates
        Advanced Input
            Macro Recording
            Key Remapping
            Multi-Language Layouts
            Programmable Function Keys
        Debugging
            Keystroke Logger
            Timing Analyzer
            Signal Monitor
            Remote Diagnostics
        Integration
            MQTT Control
            Smart Home Integration
            Automation Scripts
            Multiple IIc Support
```

### Hardware Variants

1. **Budget Version** ($15)
   - Single TXB0104 (4-channel level shifter)
   - USB only (no Bluetooth)
   - ESP32-C3 clone module

2. **Premium Version** ($50)
   - Custom PCB with SMD components
   - Integrated level shifters
   - Professional enclosure
   - OLED status display
   - Multiple USB ports

3. **Universal Adapter** ($60)
   - Support multiple vintage computers
   - Selectable output protocols
   - DIP switches for configuration
   - Compatible with: Apple IIc, IIe, II+, Commodore 64, etc.

---

## Conclusion & Recommendation

### Why This Design?

✅ **Lazy-Friendly:** No SMD soldering required
✅ **Cost-Effective:** Under $25 for full USB+BT support
✅ **Future-Proof:** ESP32 ecosystem has long-term support
✅ **Expandable:** WiFi opens doors to advanced features
✅ **Reversible:** Doesn't permanently modify Apple IIc
✅ **Well-Documented:** ESP32 has huge community & examples

### Next Steps

1. **Order components** (links provided above)
2. **Start with Phase 1** (hardware validation)
3. **Iterate quickly** (breadboard allows rapid testing)
4. **Document findings** (help future retro enthusiasts!)

### Success Metrics

- ✅ USB keyboard works with Apple IIc
- ✅ Bluetooth keyboard works wirelessly
- ✅ All 63 keys properly translated
- ✅ Reliable operation for 100+ hours
- ✅ Total cost under $30

---

## Appendix A: Shopping Links

### Core Components

- **ESP32-C3-DevKitM-1:**
  - [Espressif Official Store (AliExpress)](https://www.aliexpress.com/item/1005003989099547.html)
  - Search: "ESP32-C3-DevKitM-1"

- **Adafruit TXB0108 Level Shifter:**
  - [Adafruit Product #4245](https://www.adafruit.com/product/4245)
  - Alternative: Search Amazon for "TXB0108 breakout"

- **USB-A Breakout:**
  - Search: "USB-A female breakout board"
  - [SparkFun USB-A Female Breakout](https://www.sparkfun.com/products/12700)

### Tools & Accessories

- **Logic Analyzer** (for debugging):
  - Saleae Logic 8 ($400) or cheap clones ($10)
- **Multimeter:** Any basic model
- **Soldering Iron:** For header pins (optional)
- **Breadboard:** 830 tie-points
- **Jumper Wires:** M-F and M-M assortment

---

## Appendix B: Reference Documents

- [ESP32-C3 Technical Reference Manual](https://www.espressif.com/sites/default/files/documentation/esp32-c3_technical_reference_manual_en.pdf)
- [Apple IIc Technical Reference Manual](https://archive.org/details/a2ctrm/Apple2cTechRef01) - Section 11.7 "The Keyboard"
- [USB HID Usage Tables](https://usb.org/sites/default/files/hut1_21_0.pdf)
- [TXB0108 Datasheet](https://www.ti.com/lit/ds/symlink/txb0108.pdf)

---

## Appendix C: Contact & Support

**Project Author:** [Your Name]
**Last Updated:** 2025-10-20
**License:** Open Source Hardware (OSHW)
**Repository:** [GitHub link when available]

For questions or contributions, please open an issue on GitHub or contact via email.

---

*End of Proposal*
