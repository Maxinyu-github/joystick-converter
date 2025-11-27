# Pico-PIO-USB library path
# The library can be provided via CMake variable, or will be auto-fetched
if(NOT DEFINED PICO_PIO_USB_PATH)
    set(PICO_PIO_USB_PATH ${PICO_SDK_PATH}/lib/Pico-PIO-USB)
endif()

# Track whether we need to create our own target
set(PICO_PIO_USB_CREATE_TARGET TRUE)

# Check if Pico-PIO-USB exists, if not fetch it automatically
if(NOT EXISTS ${PICO_PIO_USB_PATH}/src/pio_usb.h)
    message(STATUS "Pico-PIO-USB not found at ${PICO_PIO_USB_PATH}, fetching from GitHub...")
    include(FetchContent)
    FetchContent_Declare(
        pico_pio_usb_fetch
        GIT_REPOSITORY https://github.com/sekigon-gonnoc/Pico-PIO-USB.git
        GIT_TAG main
        SOURCE_DIR ${PICO_PIO_USB_PATH}
    )
    FetchContent_MakeAvailable(pico_pio_usb_fetch)
    # FetchContent already created the pico_pio_usb target
    set(PICO_PIO_USB_CREATE_TARGET FALSE)
endif()

# TinyUSB path (from Pico SDK)
set(PICO_TINYUSB_PATH ${PICO_SDK_PATH}/lib/tinyusb)

# Only create pico_pio_usb target if it doesn't exist (e.g., when library was pre-installed)
if(PICO_PIO_USB_CREATE_TARGET AND NOT TARGET pico_pio_usb)
    add_library(pico_pio_usb STATIC
        ${PICO_PIO_USB_PATH}/src/pio_usb.c
        ${PICO_PIO_USB_PATH}/src/pio_usb_host.c
        ${PICO_PIO_USB_PATH}/src/usb_crc.c
    )

    target_include_directories(pico_pio_usb PUBLIC
        ${PICO_PIO_USB_PATH}/src
    )

    target_link_libraries(pico_pio_usb PUBLIC
        pico_stdlib
        hardware_pio
        hardware_dma
    )

    # Use TinyUSB implementation for PIO-USB
    target_compile_definitions(pico_pio_usb PUBLIC
        PIO_USB_USE_TINYUSB
    )
endif()

# Main executable
add_executable(joystick_converter
    main.c
    usb_host.c
    usb_device.c
    usb_descriptors.c
    config.c
    macro.c
    remapping.c
    logging.c
    # PIO-USB HCD and DCD implementations for TinyUSB
    ${PICO_TINYUSB_PATH}/src/portable/raspberrypi/pio_usb/hcd_pio_usb.c
    ${PICO_TINYUSB_PATH}/src/portable/raspberrypi/pio_usb/dcd_pio_usb.c
)

# Include the firmware directory for tusb_config.h and PIO-USB headers
target_include_directories(joystick_converter PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${PICO_PIO_USB_PATH}/src
)

# Use TinyUSB implementation for PIO-USB
target_compile_definitions(joystick_converter PRIVATE
    PIO_USB_USE_TINYUSB
)

# Pull in common dependencies
# Note: We use both tinyusb_device (native USB) and tinyusb_host (PIO-USB)
target_link_libraries(joystick_converter
    pico_stdlib
    pico_unique_id
    pico_multicore
    hardware_flash
    hardware_pio
    hardware_dma
    tinyusb_device    # TinyUSB device for HID output on native USB
    tinyusb_host      # TinyUSB host for gamepad input on PIO-USB
    pico_pio_usb      # PIO-USB driver
)

# Disable USB stdio - TinyUSB takes over the USB controller
# Serial communication for config tool is handled by TinyUSB CDC in usb_descriptors.c
# Debug output goes to UART (GPIO0/GPIO1 - requires USB-to-UART adapter)
pico_enable_stdio_usb(joystick_converter 0)
pico_enable_stdio_uart(joystick_converter 1)

# Print memory usage
target_link_options(joystick_converter PRIVATE -Xlinker --print-memory-usage)

# Enable all warnings
target_compile_options(joystick_converter PRIVATE -Wall -Wextra)

# Create map/bin/hex/uf2 files
pico_add_extra_outputs(joystick_converter)
