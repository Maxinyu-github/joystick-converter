add_executable(joystick_converter
    main.c
    usb_host.c
    usb_device.c
    config.c
    macro.c
    remapping.c
)

# Pull in common dependencies
target_link_libraries(joystick_converter
    pico_stdlib
    pico_unique_id
    hardware_flash
    tinyusb_host
    tinyusb_device
)

# Enable USB output, disable UART output
pico_enable_stdio_usb(joystick_converter 0)
pico_enable_stdio_uart(joystick_converter 1)

# Create map/bin/hex/uf2 files
pico_add_extra_outputs(joystick_converter)
