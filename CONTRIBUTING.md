# Contributing to Apple IIc Keyboard Adapter

Thank you for your interest in contributing! This project welcomes contributions of all kinds.

## How to Contribute

### Reporting Bugs

1. Check if the bug has already been reported in [Issues](https://github.com/badvision/apple-iic-keyboard-adapter/issues)
2. If not, create a new issue with:
   - Clear description of the problem
   - Steps to reproduce
   - Expected vs actual behavior
   - Hardware setup (ESP32 board, level shifters, etc.)
   - Firmware version or commit hash

### Suggesting Features

1. Open an issue with the "enhancement" label
2. Describe the feature and its use case
3. Explain why it would be valuable

### Code Contributions

#### Setup Development Environment

```bash
# Clone the repository
git clone https://github.com/badvision/apple-iic-keyboard-adapter.git
cd apple-iic-keyboard-adapter

# Install PlatformIO
pip install platformio

# Build and test
cd firmware
pio run
pio test -e native
```

#### Making Changes

1. **Fork** the repository
2. **Create a branch** for your feature: `git checkout -b feature/amazing-feature`
3. **Write tests** for new functionality
4. **Make your changes**
5. **Run tests**: `pio test -e native` (all tests must pass)
6. **Commit** with clear messages:
   ```
   Add USB keyboard enumeration support

   - Implement USB Host HID driver
   - Add device detection and initialization
   - Parse USB HID reports for keyboard events

   Fixes #42
   ```
7. **Push** to your fork: `git push origin feature/amazing-feature`
8. **Open a Pull Request**

#### Code Style

- Follow [ESP-IDF Style Guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/contribute/style-guide.html)
- Use meaningful variable and function names
- Add Doxygen comments for public API functions
- Keep functions focused and under 50 lines when possible
- Add unit tests for all new functionality

#### Testing Requirements

All code contributions must include:

1. **Unit Tests** in `firmware/test/`
   - Cover normal operation
   - Cover error cases
   - Cover edge cases
   - All tests must pass on native platform

2. **Hardware Testing** (when applicable)
   - Test on actual ESP32-C3 hardware
   - Verify with Apple IIc (if available)
   - Document test procedure in PR

3. **Documentation**
   - Update README if changing features
   - Add Doxygen comments to new functions
   - Update examples if API changes

### Documentation Contributions

Documentation improvements are always welcome!

- Fix typos or unclear explanations
- Add examples or tutorials
- Improve circuit diagrams
- Add photos of working setups

### Hardware Contributions

Have you built this adapter? Share your experience!

- Post photos of your build
- Share PCB designs
- Document modifications or improvements
- Report compatibility with different keyboards

## Project Structure

```
apple-iic-keyboard-adapter/
├── firmware/              # ESP32-C3 firmware
│   ├── src/              # Source code
│   ├── test/             # Unit tests
│   └── platformio.ini    # Build configuration
├── hardware/             # PCB designs (future)
├── docs/                 # Documentation
│   ├── manuals/          # Apple IIc manual excerpts
│   ├── datasheets/       # Chip datasheets
│   └── DESIGN.md         # Design documentation
└── test/                 # Integration tests (future)
```

## Module: AY-3600 Emulator

The core emulator module (`firmware/src/ay3600_emulator.c`) is well-tested and stable. Changes to this module require:

1. Clear justification (bug fix, feature request, etc.)
2. Comprehensive unit tests
3. Verification against Apple IIc Technical Reference Manual
4. Hardware testing with actual Apple IIc

## Git Workflow

1. Keep commits focused and atomic
2. Write clear commit messages
3. Rebase on main before submitting PR
4. Squash fixup commits before merge

## Communication

- **GitHub Issues:** Bug reports, feature requests
- **Pull Requests:** Code review and discussion
- **Discussions:** General questions, ideas, showcase

## Code of Conduct

- Be respectful and inclusive
- Provide constructive feedback
- Help others learn
- Give credit where due

## Recognition

All contributors will be acknowledged in the project README. Significant contributions may receive dedicated credit in documentation.

## License

By contributing, you agree that your contributions will be licensed under the MIT License.

## Questions?

Open an issue with the "question" label or start a discussion!

---

**Thank you for contributing to retrocomputing! 🍎✨**
