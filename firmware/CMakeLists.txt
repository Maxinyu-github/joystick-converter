add_executable(joystick_converter
    main.c
    usb_host.c
    usb_device.c
    config.c
    macro.c
    remapping.c
    logging.c
)

# Include the firmware directory for tusb_config.h
target_include_directories(joystick_converter PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Pull in common dependencies
target_link_libraries(joystick_converter
    pico_stdlib
    pico_unique_id
    hardware_flash
    tinyusb_host      # TinyUSB host for gamepad input
)

# Use UART for serial communication when using TinyUSB host
# Note: USB stdio is disabled because TinyUSB host uses the USB controller
pico_enable_stdio_usb(joystick_converter 0)
pico_enable_stdio_uart(joystick_converter 1)

# Create map/bin/hex/uf2 files
pico_add_extra_outputs(joystick_converter)
