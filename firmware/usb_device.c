/**
 * USB Device Module Implementation
 * 
 * This module provides USB device output functionality.
 * Note: USB Device functionality requires separate USB controller from USB Host.
 * On RP2040/RP2350 with single USB controller, full dual USB support would
 * require PIO-USB for one role. The stdio_usb (CDC serial) is handled by
 * the Pico SDK separately.
 */

#include "usb_device.h"
#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "tusb.h"

static output_type_t current_output_type = OUTPUT_TYPE_GAMEPAD;
static bool config_mode_request = false;

bool usb_device_init(void) {
    printf("USB Device: Initializing...\n");
    
    // Note: USB Device stack initialization is currently not enabled because
    // the RP2040/RP2350 has only one USB controller. When pico_enable_stdio_usb
    // is enabled, it uses the USB controller for CDC serial communication.
    // For full USB Device HID output, PIO-USB would be needed as a second
    // USB controller, or stdio would need to use UART instead.
    
    config_mode_request = false;
    
    printf("USB Device: Initialized (device output disabled, using CDC for serial)\n");
    return true;
}

void usb_device_task(void) {
    // USB device task is currently handled by stdio_usb in the Pico SDK
    // for CDC serial communication. Full HID device output would require
    // additional USB controller (e.g., PIO-USB).
}

void usb_device_send_gamepad(uint16_t buttons, int16_t *axes, uint8_t num_axes) {
    (void)buttons;
    (void)axes;
    (void)num_axes;
    
    // HID gamepad output requires USB Device stack which is currently not
    // enabled due to single USB controller limitation
    if (current_output_type != OUTPUT_TYPE_GAMEPAD) {
        return;
    }
}

void usb_device_send_keyboard(uint8_t modifiers, uint8_t *keycodes, uint8_t num_keys) {
    (void)modifiers;
    (void)keycodes;
    (void)num_keys;
    
    // HID keyboard output requires USB Device stack
    if (current_output_type != OUTPUT_TYPE_KEYBOARD && 
        current_output_type != OUTPUT_TYPE_COMBO) {
        return;
    }
}

void usb_device_send_mouse(uint8_t buttons, int8_t x, int8_t y, int8_t wheel) {
    (void)buttons;
    (void)x;
    (void)y;
    (void)wheel;
    
    // HID mouse output requires USB Device stack
    if (current_output_type != OUTPUT_TYPE_MOUSE && 
        current_output_type != OUTPUT_TYPE_COMBO) {
        return;
    }
}

bool usb_device_config_mode_requested(void) {
    bool request = config_mode_request;
    config_mode_request = false;  // Clear flag
    return request;
}

void usb_device_set_output_type(output_type_t type) {
    if (type != current_output_type) {
        printf("USB Device: Output type changed to %d\n", type);
        current_output_type = type;
    }
}

output_type_t usb_device_get_output_type(void) {
    return current_output_type;
}
