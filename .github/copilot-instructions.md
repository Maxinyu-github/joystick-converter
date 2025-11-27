# Copilot Instructions for Joystick Converter

This document provides instructions for GitHub Copilot when working on this repository.

## Project Overview

Joystick Converter is a gamepad/joystick converter based on the RP2350-PiZero development board. It converts gamepad inputs to compatible outputs for game consoles or PCs, with support for button remapping and mouse macros.

## Tech Stack

- **Firmware**: C (C11 standard) for RP2350 microcontroller
- **Build System**: CMake (minimum version 3.13)
- **SDK**: Raspberry Pi Pico SDK
- **Configuration Tool**: Python 3 with PyQt6
- **CI/CD**: GitHub Actions

## Project Structure

```
joystick-converter/
├── firmware/               # RP2350 firmware (C code)
│   ├── main.c             # Main entry point
│   ├── usb_host.c, .h     # USB Host functionality
│   ├── usb_device.c, .h   # USB Device output
│   ├── config.c, .h       # Configuration management
│   ├── remapping.c, .h    # Button remapping engine
│   └── macro.c, .h        # Macro system
├── config_software/       # PC configuration tool (Python)
│   ├── config_tool.py     # PyQt6 GUI application
│   └── requirements.txt   # Python dependencies
├── docs/                  # Documentation
└── CMakeLists.txt        # Root CMake configuration
```

## Build Commands

### Firmware

```bash
# Set Pico SDK path
export PICO_SDK_PATH=/path/to/pico-sdk

# Configure and build
mkdir build && cd build
cmake ..
make -j$(nproc)
```

### Configuration Tool

```bash
cd config_software
pip install -r requirements.txt
python config_tool.py
```

## Coding Standards

### C/C++ (Firmware)

- Follow the existing code style
- Use meaningful variable and function names
- Add comments for complex logic
- Keep functions focused and small
- Use `static` for internal functions
- Initialize all variables
- Check return values

Example:
```c
/**
 * Brief function description
 * 
 * @param input Description of input parameter
 * @return Description of return value
 */
static bool process_input(const gamepad_state_t *input) {
    if (!input) {
        return false;
    }
    
    // Implementation
    return true;
}
```

### Python (Configuration Software)

- Follow PEP 8 style guide
- Use type hints where appropriate
- Write docstrings for functions
- Keep functions focused
- Handle exceptions properly

Example:
```python
def configure_device(port: str, config: dict) -> bool:
    """
    Configure device with provided settings.
    
    Args:
        port: Serial port name
        config: Configuration dictionary
        
    Returns:
        True on success, False on failure
    """
    if not port or not config:
        return False
    
    # Implementation
    return True
```

## Boundaries

### Files to Avoid Modifying

- `pico_sdk_import.cmake` - SDK import script, managed externally
- `.github/workflows/` - CI configuration (modify only when specifically requested)
- `LICENSE` - License file

### Directories to Be Careful With

- `firmware/` - Core firmware code, test thoroughly after changes
- `config_software/` - GUI tool, ensure UI consistency

## Testing Guidelines

1. **Build Test**: Ensure firmware compiles without errors
   ```bash
   cd build && make clean && make
   ```

2. **No Unit Tests**: This project relies on hardware testing
   - Flash firmware to device
   - Test affected features manually
   - Test edge cases

## Commit Message Format

- Use present tense ("Add feature" not "Added feature")
- Keep first line under 50 characters
- Reference issues when applicable: "Fixes #123"

Examples:
- "Add support for Xbox Elite controller"
- "Fix button mapping not persisting after reboot"
- "Improve mouse sensitivity configuration"

## Additional Notes

- The firmware uses USB dual-role capabilities of RP2350
- Configuration is stored in the last flash sector
- Configuration limits are defined in `firmware/config.h` and `firmware/macro.h`:
  - `MAX_BUTTON_MAPPINGS`: Maximum button mappings
  - `MAX_MACROS`: Maximum number of macros
  - `MAX_MACRO_STEPS`: Maximum steps per macro
- Debug mode is available for testing gamepad inputs
