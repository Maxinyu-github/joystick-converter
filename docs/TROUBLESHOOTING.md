# Troubleshooting Guide

This guide helps resolve common issues with the Joystick Converter.

## Device Connection Issues

### Device Not Recognized by PC

**Symptoms**: 
- Device doesn't show up in device manager
- No serial port appears

**Solutions**:
1. Check USB cable - ensure it's a data cable, not power-only
2. Try a different USB port
3. Re-flash the firmware:
   - Hold BOOTSEL and connect to PC
   - Copy UF2 file again
4. Check if LED is blinking (indicates firmware is running)
5. Verify power supply is adequate (5V, at least 500mA)

**Windows-specific Notes**:
- After flashing, a new COM port should appear automatically
- Check Device Manager > Ports (COM & LPT) for the serial port
- The device appears as a USB Serial Device (CDC) 
- If no port appears after firmware flash, ensure you're using the latest firmware build

### Serial Port Access Denied (Linux)

**Symptoms**: 
- Permission denied when trying to access /dev/ttyACM0

**Solutions**:
```bash
# Add user to dialout group
sudo usermod -a -G dialout $USER

# Log out and back in, or use:
newgrp dialout

# Or change permissions temporarily:
sudo chmod 666 /dev/ttyACM0
```

## Gamepad Connection Issues

### No Log Output When Connecting Keyboard/Gamepad

**Symptoms**:
- No log output when connecting USB devices
- LED continues waiting mode blink pattern
- Devices work on PC but not detected by the converter

**Solutions**:
1. **USB Host Integration Status**: The current firmware uses placeholder implementations for USB Host functionality. The TinyUSB host library integration is pending. The USB callbacks (`tuh_hid_mount_cb`, `tuh_hid_umount_cb`, etc.) are defined but not yet called because TinyUSB host is not linked in the build.
2. **PIO-USB Note**: For RP2350-PiZero boards using PIO-USB (software USB via GPIO pins), additional configuration is required:
   - PIO-USB uses GPIO pins instead of the hardware USB controller
   - The firmware needs to be configured with the correct GPIO pins for your board
   - Ensure you're connecting to the PIO-USB pins, not the hardware USB port
3. **Cable Requirements**: Use a proper USB data cable, not a power-only cable
4. **Check CMakeLists.txt**: Verify that `tinyusb_host` is uncommented in `firmware/CMakeLists.txt` when the integration is complete

### Gamepad Not Detected

**Symptoms**: 
- LED blinks at medium speed (waiting for device)
- Serial output shows "Waiting for gamepad"

**Solutions**:
1. Verify gamepad is a standard USB HID device
2. Try a different USB cable
3. Test gamepad on another computer to verify it works
4. Check serial output for error messages
5. Some gamepads may require specific drivers - check TinyUSB compatibility

### Gamepad Disconnects Randomly

**Symptoms**: 
- Gamepad works but disconnects intermittently

**Solutions**:
1. Check power supply - insufficient current can cause disconnects
2. Verify USB cable quality
3. Reduce USB cable length if possible
4. Check for electromagnetic interference
5. Update firmware to latest version

## Input/Output Issues

### Buttons Not Responding

**Symptoms**: 
- Gamepad connected but button presses don't work

**Solutions**:
1. Check output mode is correct (gamepad/keyboard/mouse)
2. Verify button mappings in configuration
3. Test in passthrough mode (no mappings)
4. Check serial output for button press detection
5. Try loading default configuration

### Wrong Keys Being Sent

**Symptoms**: 
- Correct buttons pressed but wrong output

**Solutions**:
1. Review button mapping configuration
2. Check for conflicting mappings
3. Verify key codes match expected values (see USB HID spec)
4. Reset to default configuration and reconfigure
5. Check if CAPS LOCK or other modifiers are active

### Analog Sticks Not Working

**Symptoms**: 
- Buttons work but analog sticks don't respond

**Solutions**:
1. Check output mode supports analog input
2. Verify stick calibration
3. Check deadzone settings
4. Test sticks with serial monitor to see values
5. Some games may require additional configuration

### Mouse Movement Too Fast/Slow

**Symptoms**: 
- Mouse sensitivity is not comfortable

**Solutions**:
1. Adjust sensitivity in configuration software
2. Modify mouse movement scaling in firmware
3. Use mouse acceleration settings in OS
4. Consider adding sensitivity adjustment to mappings

## Macro Issues

### Macro Not Executing

**Symptoms**: 
- Button mapped to macro but nothing happens

**Solutions**:
1. Verify macro is properly saved to device
2. Check macro ID matches in button mapping
3. Review macro syntax for errors
4. Test macro steps individually
5. Check serial output for macro execution messages

### Macro Executes Incorrectly

**Symptoms**: 
- Macro starts but doesn't complete or does wrong actions

**Solutions**:
1. Increase delay values between steps
2. Verify key codes are correct
3. Check if application has focus when macro runs
4. Simplify macro to identify problematic step
5. Test in a text editor to see actual output

### Macro Too Slow/Fast

**Symptoms**: 
- Macro timing is not right

**Solutions**:
1. Adjust delay values in macro definition
2. Consider game/application response time
3. Test with incremental delay adjustments
4. Some applications may need longer delays

## Configuration Software Issues

### Cannot Connect to Device

**Symptoms**: 
- Configuration software can't establish connection

**Solutions**:
1. Verify correct serial port is selected
2. Close other applications using the serial port
3. Try disconnecting and reconnecting device
4. Check serial port permissions (Linux)
5. Restart configuration software

### Configuration Not Saving

**Symptoms**: 
- Settings changed but not persisted after power cycle

**Solutions**:
1. Click "Save to Device" button
2. Wait for confirmation message
3. Check flash memory is not corrupted
4. Try saving simpler configuration first
5. Re-flash firmware if persistent

### Configuration Software Crashes

**Symptoms**: 
- GUI application crashes or freezes

**Solutions**:
1. Update Python and PyQt6 to latest versions
2. Check system compatibility
3. Review error logs/stack traces
4. Try running from command line to see errors
5. Report bug with error details

## Build Issues

### CMake Cannot Find Pico SDK

**Symptoms**: 
```
CMake Error: PICO_SDK_PATH not defined
```

**Solutions**:
```bash
# Set environment variable
export PICO_SDK_PATH=/path/to/pico-sdk

# Or use CMake option
cmake -DPICO_SDK_PATH=/path/to/pico-sdk ..
```

### Compiler Not Found

**Symptoms**: 
```
arm-none-eabi-gcc: command not found
```

**Solutions**:
```bash
# Install ARM toolchain
# Ubuntu/Debian:
sudo apt install gcc-arm-none-eabi

# macOS:
brew install --cask gcc-arm-embedded

# Verify installation:
arm-none-eabi-gcc --version
```

### Build Fails with Undefined References

**Symptoms**: 
```
undefined reference to 'tuh_init'
```

**Solutions**:
1. Ensure Pico SDK submodules are initialized:
   ```bash
   cd $PICO_SDK_PATH
   git submodule update --init --recursive
   ```
2. Clean build directory and rebuild:
   ```bash
   rm -rf build
   mkdir build && cd build
   cmake .. && make
   ```

### UF2 File Not Generated

**Symptoms**: 
- Build succeeds but no .uf2 file

**Solutions**:
1. Check if elf2uf2 tool is built in Pico SDK
2. Manually convert:
   ```bash
   elf2uf2 joystick_converter.elf joystick_converter.uf2
   ```
3. Verify CMakeLists.txt has `pico_add_extra_outputs()`

## Performance Issues

### Lag or Latency

**Symptoms**: 
- Noticeable delay between button press and action

**Solutions**:
1. Reduce number of active mappings
2. Simplify complex macros
3. Check USB polling rate
4. Verify no background processes causing delays
5. Use passthrough mode to establish baseline

### Device Runs Hot

**Symptoms**: 
- RP2350 gets warm during use

**Solutions**:
1. Normal operation - RP2350 can run warm
2. Ensure adequate ventilation
3. Check for infinite loops in custom code
4. Monitor power consumption
5. Add heatsink if needed (optional)

## Debugging Tips

### Enable Verbose Logging

Connect via serial terminal to see debug output:
```bash
minicom -b 115200 -D /dev/ttyACM0
```

### Check LED Status

LED blink patterns indicate device state:
- Fast (100ms): Initializing or error
- Medium (500ms): Waiting for gamepad
- Slow (2000ms): Active and working
- Config mode (200ms): Configuration mode

### Test in Stages

1. Test without mappings (passthrough)
2. Add one mapping at a time
3. Test macros individually
4. Combine gradually

### Use Serial Monitor

Monitor serial output to see:
- Device initialization
- Gamepad connection events
- Button press detection
- Mapping execution
- Macro steps
- Error messages

### Factory Reset

If all else fails, reset to factory defaults:
1. Hold specific button combination on gamepad (if implemented)
2. Or re-flash firmware
3. Or manually erase flash:
   ```c
   // In code, call:
   config_set_defaults();
   config_save();
   ```

## Getting Help

If you can't resolve your issue:

1. **Check Documentation**: Review all docs in the `docs/` directory
2. **Search Issues**: Look through GitHub issues for similar problems
3. **Collect Information**:
   - Firmware version
   - Hardware model
   - Serial debug output
   - Configuration settings
   - Steps to reproduce
4. **Create Issue**: Open a new GitHub issue with all details

## Common Error Messages

### "USB Host: Failed to initialize"
- USB host hardware issue
- Check USB connections
- Verify RP2350 USB configuration

### "Config: Invalid magic number"
- Corrupted flash configuration
- Reset to defaults: `config_set_defaults(); config_save();`

### "Macro: Macro table full"
- Too many macros defined (max 16)
- Remove unused macros

### "Config: Mapping table full"
- Too many button mappings (max 32)
- Consolidate or remove mappings

### "Flash write error"
- Flash memory issue
- Try reflashing firmware
- Check for hardware fault
