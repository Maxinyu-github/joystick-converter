# Debug Mode Guide

## Overview

Debug mode is a feature that allows you to view real-time input device data from the configuration software. This is particularly useful for:
- Identifying which buttons your gamepad supports
- Detecting special buttons (joystick presses, back buttons, extra buttons)
- Troubleshooting button mapping issues
- Understanding the raw data from different gamepad models
- Testing input functionality with a keyboard

## Features

The debug mode displays:
- **Button States**: Visual indicators for all standard buttons (A, B, X, Y, LB, RB, Back, Start, LS, RS)
- **Analog Sticks**: Real-time X/Y position values for left and right sticks
- **Triggers**: Pressure values for left and right triggers (0-255)
- **D-Pad**: Current direction indicator
- **Raw Button Data**: Hexadecimal button bitmap for identifying unmapped buttons
- **Keyboard Input**: Support for keyboard as input device for debugging
- **Device Info**: Display connected device VID/PID and device type

## Supported Input Devices

### Gamepads
- Nintendo Switch Pro Controller
- Nintendo Switch Pro Controller 2
- Xbox 360/One/Series Controllers
- PlayStation 3/4/5 Controllers
- Generic USB Gamepads

### Keyboards
Keyboards can be used as debug input devices to test if input functionality is working correctly. When a keyboard is connected:
- Modifier key states are displayed (Ctrl, Alt, Shift, GUI)
- Currently pressed key codes are shown (up to 6)
- Useful for verifying that input channels are working properly

## How to Use Debug Mode

### 1. Connect Your Device

1. Open the configuration tool:
   ```bash
   cd config_software
   python config_tool.py
   ```

2. Select your device's serial port from the dropdown
3. Click "Connect"

### 2. Start Debug Mode

1. Click on the "Debug Mode" tab
2. Connect your gamepad to the converter device
3. Click "Start Debug Mode"
4. The status should change to "Active"

### 3. Test Your Gamepad

- Press buttons on your gamepad and watch them highlight in green
- Move the analog sticks and see the values change
- Pull the triggers and observe the progress bars
- Use the D-pad and see the direction indicator

### 4. Identify Special Buttons

The **Raw Button Data (Hex)** display shows the complete button bitmap. Each bit represents a button:

| Bit | Mask   | Button |
|-----|--------|--------|
| 0   | 0x0001 | A      |
| 1   | 0x0002 | B      |
| 2   | 0x0004 | X      |
| 3   | 0x0008 | Y      |
| 4   | 0x0010 | LB     |
| 5   | 0x0020 | RB     |
| 6   | 0x0040 | Back   |
| 7   | 0x0080 | Start  |
| 8   | 0x0100 | LS     |
| 9   | 0x0200 | RS     |
| 10+ | 0x0400+ | Special/Extra buttons |

If you see the hex value change when pressing a button that isn't shown in the standard button list, you've found a special button! Note the hex value and the bit position to add support for it.

### 5. Stop Debug Mode

Click "Stop Debug Mode" when you're done. The device will return to normal operation mode.

## Understanding the Data

### Analog Stick Values
- Range: -32768 to 32767
- Center position: 0
- Negative values: Left (X-axis) or Down (Y-axis)
- Positive values: Right (X-axis) or Up (Y-axis)

### Trigger Values
- Range: 0 to 255
- 0: Not pressed
- 255: Fully pressed

### D-Pad Values
- dpad_x: -1 (Left), 0 (Center), 1 (Right)
- dpad_y: -1 (Up), 0 (Center), 1 (Down)

## Troubleshooting

### "Not connected to device" error
- Make sure you've clicked "Connect" in the Connection section
- Verify the correct serial port is selected
- Check that the device is powered on and plugged in

### No data showing
- Ensure your gamepad is connected to the converter device
- Some gamepads may need to be activated by pressing a button
- Check the serial connection is stable

### Values seem incorrect
- Some gamepads have different center calibration
- Analog sticks may not center exactly at 0
- This is normal and the firmware handles small deadzones

### Debug mode won't start
- Make sure the device firmware supports debug mode
- Check that you're using the latest firmware version
- Verify serial communication is working (check Connection status)

## Technical Details

### Communication Protocol

Debug mode uses a simple text-based protocol over serial (115200 baud):

**Commands** (PC → Device):
```
DEBUG_START\n    - Enable debug mode
DEBUG_STOP\n     - Disable debug mode
DEBUG_GET\n      - Request current input state (gamepad or keyboard)
DEBUG_INFO\n     - Request connected device info
```

**Responses** (Device → PC):
```
DEBUG_MODE_STARTED\n                              - Confirmation
DEBUG_MODE_STOPPED\n                              - Confirmation
DEBUG:buttons,lx,ly,rx,ry,lt,rt,dx,dy\n          - Gamepad state data
DEBUG_KB:modifiers,num_keys,k0,k1,k2,k3,k4,k5\n  - Keyboard state data
DEBUG_INFO:vid,pid,addr,type\n                   - Device info
DEBUG_INFO:NO_DEVICE\n                           - No device connected
```

**Gamepad State Data Format**:
- `buttons`: 16-bit button bitmap (unsigned integer)
- `lx`, `ly`: Left stick X/Y (-32768 to 32767)
- `rx`, `ry`: Right stick X/Y (-32768 to 32767)
- `lt`, `rt`: Left/Right triggers (0-255)
- `dx`, `dy`: D-pad X/Y (-1, 0, 1)

**Keyboard State Data Format**:
- `modifiers`: Modifier key bitmap (Ctrl=0x01, Shift=0x02, Alt=0x04, GUI=0x08)
- `num_keys`: Number of keys currently pressed
- `k0-k5`: Key codes (USB HID key codes)

**Device Info Format**:
- `vid`: Vendor ID (hexadecimal)
- `pid`: Product ID (hexadecimal)
- `addr`: USB device address
- `type`: Input type (0=Unknown, 1=Gamepad, 2=Keyboard)

### Polling Rate

The configuration tool polls for gamepad state every 50ms (20Hz) when debug mode is active. This provides smooth visual feedback while not overwhelming the serial connection.

## Adding Support for Special Buttons

Once you've identified special buttons using debug mode:

1. Note the bit position of the special button in the raw data
2. Update `firmware/usb_host.h` to add a new button definition:
   ```c
   #define GAMEPAD_BUTTON_SPECIAL  (1 << 10)  // Bit 10
   ```
3. Update the config tool to display the new button
4. Recompile and flash the firmware

## Example Scenarios

### Scenario 1: Xbox Controller with Back Paddles
Some Xbox Elite controllers have back paddles. In debug mode, you might see:
- Standard buttons work normally
- Pressing back paddle shows hex value change (e.g., 0x0400)
- This indicates bit 10 is the back paddle
- Add mapping for this button in firmware

### Scenario 2: PlayStation Controller with Touchpad Click
The PS4/PS5 touchpad can be clicked:
- Standard buttons work normally
- Clicking touchpad shows hex change (e.g., 0x0800)
- This indicates bit 11 is the touchpad button
- Add support and mapping

### Scenario 3: Generic Controller with Extra Buttons
Budget controllers may have turbo or mode buttons:
- These appear as additional bits in the bitmap
- Use debug mode to identify which bit each button uses
- Add definitions and mappings as needed

## See Also

- [API Documentation](API.md) - For programming interface details
- [Examples](EXAMPLES.md) - For button mapping examples
- [Troubleshooting](TROUBLESHOOTING.md) - For common issues
