# Build Instructions

## Prerequisites

### Required Tools

1. **CMake** (version 3.13 or higher)
   ```bash
   # Ubuntu/Debian
   sudo apt install cmake
   
   # macOS
   brew install cmake
   
   # Windows
   # Download from https://cmake.org/download/
   ```

2. **ARM GCC Toolchain**
   ```bash
   # Ubuntu/Debian
   sudo apt install gcc-arm-none-eabi libnewlib-arm-none-eabi libstdc++-arm-none-eabi-newlib
   
   # macOS
   brew install --cask gcc-arm-embedded
   
   # Windows
   # Download from https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm
   ```

3. **Pico SDK**
   ```bash
   # Clone Pico SDK
   git clone https://github.com/raspberrypi/pico-sdk.git
   cd pico-sdk
   git submodule update --init
   
   # Set environment variable
   export PICO_SDK_PATH=/path/to/pico-sdk
   ```

4. **Pico-PIO-USB Library** (for dual USB support)
   ```bash
   # Clone Pico-PIO-USB into SDK's lib directory
   git clone https://github.com/sekigon-gonnoc/Pico-PIO-USB.git $PICO_SDK_PATH/lib/Pico-PIO-USB
   ```

### Optional Tools

- **Build essentials**: `build-essential` (Linux), Xcode Command Line Tools (macOS)
- **Python 3**: For build scripts
- **minicom** or similar: For serial debugging

## Building the Firmware

### Step 1: Clone Repository

```bash
git clone https://github.com/Maxinyu-github/joystick-converter.git
cd joystick-converter
```

### Step 2: Configure Build

```bash
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake ..

# Or specify Pico SDK path explicitly
cmake -DPICO_SDK_PATH=/path/to/pico-sdk ..
```

### Step 3: Compile

```bash
# Build all targets
make

# Or build with verbose output
make VERBOSE=1

# Or build with multiple cores
make -j4
```

### Step 4: Flash Firmware

The build process creates `joystick_converter.uf2` in `build/firmware/`.

**Method 1: BOOTSEL Mode (Recommended for initial flash)**

1. Disconnect the RP2350 board from USB
2. Hold the BOOTSEL button on the board
3. Connect the board to your computer via USB while holding BOOTSEL
4. The board appears as a USB mass storage device
5. Copy `joystick_converter.uf2` to the mounted drive
6. The board automatically reboots and runs the firmware

**Method 2: Using picotool (for subsequent updates)**

```bash
# Install picotool
git clone https://github.com/raspberrypi/picotool.git
cd picotool
mkdir build && cd build
cmake ..
make
sudo make install

# Flash firmware
picotool load joystick_converter.uf2
picotool reboot
```

**Method 3: Using OpenOCD with Debug Probe**

```bash
openocd -f interface/cmsis-dap.cfg -f target/rp2040.cfg -c "program joystick_converter.elf verify reset exit"
```

## Building the Configuration Software

### Prerequisites

- Python 3.8 or higher
- pip (Python package manager)

### Installation

```bash
cd config_software

# Create virtual environment (recommended)
python3 -m venv venv
source venv/bin/activate  # On Windows: venv\Scripts\activate

# Install dependencies
pip install -r requirements.txt
```

### Running the Configuration Tool

```bash
python config_tool.py
```

### Creating Executable (Optional)

To create a standalone executable:

```bash
# Install PyInstaller
pip install pyinstaller

# Create executable
pyinstaller --onefile --windowed config_tool.py

# Executable will be in dist/config_tool (or dist/config_tool.exe on Windows)
```

## Build Options

### CMake Options

- `PICO_BOARD`: Set board type (default: `pico2`)
  ```bash
  cmake -DPICO_BOARD=pico2 ..
  ```

- `CMAKE_BUILD_TYPE`: Set build type
  ```bash
  cmake -DCMAKE_BUILD_TYPE=Debug ..    # Debug build with symbols
  cmake -DCMAKE_BUILD_TYPE=Release ..  # Optimized release build
  ```

- `PICO_SDK_FETCH_FROM_GIT`: Automatically fetch Pico SDK
  ```bash
  cmake -DPICO_SDK_FETCH_FROM_GIT=ON ..
  ```

## Debugging

### Serial Debug Output

The firmware outputs debug information via UART. Connect a USB-to-UART adapter to:
- **TX**: GPIO0
- **RX**: GPIO1
- **Baud rate**: 115200

**Note**: The native USB port is used for HID device output (gamepad/keyboard/mouse) and CDC serial communication with the PC configuration tool. Debug output goes to the UART pins.

```bash
# Linux (with USB-to-UART adapter)
minicom -b 115200 -D /dev/ttyUSB0

# macOS
screen /dev/tty.usbserial* 115200

# Windows (PuTTY)
# Use COM port with 115200 baud rate
```

### USB Port Configuration (Waveshare RP2350-PiZero)

The firmware uses dual USB:
- **Native USB (Type-C labeled "USB")**: Connects to PC/gaming machine as USB Device (HID + CDC)
- **PIO-USB (Type-C labeled "PIO-USB")**: Connects to controller as USB Host

When the device is properly connected:
1. Connect the controller to the PIO-USB port
2. Connect the USB port to your PC or gaming machine
3. Windows/Mac/Linux should recognize the device as a composite HID device

### SWD Debugging

Using a debug probe (e.g., Raspberry Pi Debug Probe):

```bash
# Start OpenOCD
openocd -f interface/cmsis-dap.cfg -f target/rp2040.cfg

# In another terminal, start GDB
arm-none-eabi-gdb build/firmware/joystick_converter.elf
(gdb) target remote localhost:3333
(gdb) load
(gdb) monitor reset init
(gdb) continue
```

## Troubleshooting

### Issue: CMake cannot find Pico SDK

**Solution**: Set the `PICO_SDK_PATH` environment variable:
```bash
export PICO_SDK_PATH=/path/to/pico-sdk
```

Or use CMake option:
```bash
cmake -DPICO_SDK_PATH=/path/to/pico-sdk ..
```

### Issue: Build fails with "arm-none-eabi-gcc not found"

**Solution**: Install ARM GCC toolchain and ensure it's in your PATH:
```bash
which arm-none-eabi-gcc
```

### Issue: UF2 file not created

**Solution**: Ensure you have all Pico SDK submodules:
```bash
cd $PICO_SDK_PATH
git submodule update --init --recursive
```

### Issue: Python dependencies fail to install

**Solution**: Upgrade pip and try again:
```bash
pip install --upgrade pip
pip install -r requirements.txt
```

### Issue: Serial port not accessible (Linux)

**Solution**: Add your user to the dialout group:
```bash
sudo usermod -a -G dialout $USER
# Log out and back in for changes to take effect
```

## Clean Build

To perform a clean build:

```bash
# Remove build directory
rm -rf build

# Recreate and build
mkdir build
cd build
cmake ..
make
```

## Cross-Platform Notes

### Linux
- Most straightforward platform for development
- Ensure dialout group membership for serial access

### macOS
- May need to approve security settings for ARM toolchain
- Use Homebrew for easy package installation

### Windows
- Use MSYS2 or WSL2 for best experience
- Alternative: Use CMake GUI and Visual Studio
- Serial port will be COMx instead of /dev/ttyACMx

## Continuous Integration

For automated builds, see `.github/workflows/` (if available) for CI/CD configuration examples.
