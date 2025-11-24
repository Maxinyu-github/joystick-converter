# Joystick Converter

A versatile gamepad/joystick converter based on the RP2350-PiZero development board. Convert gamepad inputs to compatible outputs for game consoles or PCs, with support for button remapping and complex mouse macros.

[中文文档](docs/README_CN.md) | [Build Instructions](docs/BUILD.md)

## Features

- **USB Host Support**: Connect various USB gamepads via Type-C
- **USB Device Output**: Acts as HID device (gamepad/keyboard/mouse)
- **Flexible Button Mapping**:
  - Remap gamepad buttons to other buttons
  - Map buttons to keyboard keys
  - Map buttons to mouse actions
  - Trigger complex macros
- **Mouse Macro System**: Record and execute complex mouse and keyboard sequences
- **Multiple Output Modes**:
  - Gamepad mode
  - Keyboard mode
  - Mouse mode
  - Keyboard+Mouse combo mode
- **Debug Mode**: Real-time visualization of gamepad input for identifying special buttons
- **PC Configuration Tool**: GUI application for easy configuration
- **Persistent Configuration**: Settings saved to flash memory

## Hardware Requirements

- **Development Board**: RP2350-PiZero (or compatible RP2350 board)
- **Interface**: Type-C USB port (for gamepad and PC connection)
- **Optional**: Status LED (GPIO 25)

## Quick Start

### 1. Build Firmware

```bash
# Set Pico SDK path
export PICO_SDK_PATH=/path/to/pico-sdk

# Build
mkdir build && cd build
cmake ..
make
```

See [BUILD.md](docs/BUILD.md) for detailed instructions.

### 2. Flash Firmware

1. Hold BOOTSEL button on RP2350 board
2. Connect board to PC via USB
3. Copy `joystick_converter.uf2` to the mounted drive
4. Device will reboot automatically

### 3. Use Configuration Tool

```bash
cd config_software
pip install -r requirements.txt
python config_tool.py
```

## Project Structure

```
joystick-converter/
├── firmware/               # RP2350 firmware code
│   ├── main.c             # Main program
│   ├── usb_host.c/h       # USB Host (gamepad input)
│   ├── usb_device.c/h     # USB Device (output)
│   ├── config.c/h         # Configuration management
│   ├── remapping.c/h      # Button remapping engine
│   └── macro.c/h          # Macro system
├── config_software/       # PC configuration tool
│   ├── config_tool.py     # PyQt6 GUI application
│   └── requirements.txt   # Python dependencies
├── docs/                  # Documentation
└── CMakeLists.txt        # CMake build configuration
```

## LED Status Indicators

The onboard LED (GPIO 25) indicates device status:

- **Fast blink (100ms)**: Initializing or error
- **Medium blink (500ms)**: Waiting for gamepad
- **Slow blink (2000ms)**: Active and working
- **Fast blink (200ms)**: Configuration mode

## Examples

### Example 1: Basic Button Remapping

Map gamepad A button to keyboard Space key:

1. Open configuration tool
2. Click "Add Mapping"
3. Source: Button A
4. Type: Keyboard Key
5. Target: Space (0x2C)
6. Save to device

### Example 2: Mouse Macro

Create a click macro (move mouse then click):

```
Macro ID: 1
Steps:
mouse_move 100 50
delay 100
mouse_button_press 1
delay 50
mouse_button_release
```

Then map a gamepad button to trigger this macro.

### Example 3: Analog Stick to Mouse

Select "Mouse" or "Keyboard+Mouse" output mode. The right analog stick will automatically control mouse movement.

## Debug Mode

Debug mode helps you identify and test all gamepad inputs, including special buttons:

1. Open configuration tool and connect to device
2. Switch to "Debug Mode" tab
3. Connect your gamepad
4. Click "Start Debug Mode"
5. Press buttons and move sticks to see real-time data

The debug display shows:
- All standard buttons with visual indicators
- Analog stick positions with numeric values
- Trigger pressure levels
- D-pad direction
- Raw button data (hex) for identifying unmapped buttons

This is particularly useful for gamepads with special buttons like:
- Joystick press buttons (LS/RS)
- Back paddles on elite controllers
- Extra mode/turbo buttons
- Special function buttons

See [Debug Mode Guide](docs/DEBUG_MODE.md) for detailed instructions.

## Technical Details

### USB Dual Role

The project uses RP2350's USB capabilities to implement dual role:
- **USB Host**: Connect and read gamepad input
- **USB Device**: Output as HID device to PC/console

### Configuration Storage

Configuration stored in the last flash sector, including:
- Output device type
- Button mapping table (up to 32 mappings)
- Macro definitions (up to 16 macros, 128 steps each)

### Remapping Engine

Supports the following mapping types:
1. **Passthrough**: No mapping, direct gamepad output
2. **Button to Button**: Remap gamepad buttons
3. **Button to Keyboard**: Map to keyboard keys
4. **Button to Mouse**: Map to mouse buttons
5. **Button to Macro**: Trigger macro execution

## Contributing

Contributions welcome! Please open an issue or submit a pull request.

## License

MIT License - see LICENSE file for details.

## Support

For issues or questions, please create an issue on GitHub.