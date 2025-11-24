/**
 * USB Device Module Implementation
 */

#include "usb_device.h"
#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"

static output_type_t current_output_type = OUTPUT_TYPE_GAMEPAD;
static bool config_mode_request = false;

bool usb_device_init(void) {
    printf("USB Device: Initializing...\n");
    
    // TODO: Initialize TinyUSB device stack
    // This would involve:
    // 1. Setting up USB device descriptors
    // 2. Configuring HID class for gamepad/keyboard/mouse
    // 3. Starting USB device
    
    config_mode_request = false;
    
    printf("USB Device: Initialized (placeholder)\n");
    return true;
}

void usb_device_task(void) {
    // TODO: Process USB device events
    // This would involve handling TinyUSB device task
    // tud_task();
}

void usb_device_send_gamepad(uint16_t buttons, int16_t *axes, uint8_t num_axes) {
    // TODO: Send HID gamepad report
    // This would format and send a gamepad HID report via TinyUSB
    
    if (current_output_type != OUTPUT_TYPE_GAMEPAD) {
        return;
    }
    
    // Example implementation would look like:
    // uint8_t report[report_size];
    // format_gamepad_report(report, buttons, axes, num_axes);
    // tud_hid_report(REPORT_ID_GAMEPAD, report, sizeof(report));
}

void usb_device_send_keyboard(uint8_t modifiers, uint8_t *keycodes, uint8_t num_keys) {
    // TODO: Send HID keyboard report
    
    if (current_output_type != OUTPUT_TYPE_KEYBOARD && 
        current_output_type != OUTPUT_TYPE_COMBO) {
        return;
    }
    
    // Example implementation:
    // uint8_t report[8] = {modifiers};
    // memcpy(&report[2], keycodes, min(num_keys, 6));
    // tud_hid_keyboard_report(REPORT_ID_KEYBOARD, modifiers, keycodes);
}

void usb_device_send_mouse(uint8_t buttons, int8_t x, int8_t y, int8_t wheel) {
    // TODO: Send HID mouse report
    
    if (current_output_type != OUTPUT_TYPE_MOUSE && 
        current_output_type != OUTPUT_TYPE_COMBO) {
        return;
    }
    
    // Example implementation:
    // tud_hid_mouse_report(REPORT_ID_MOUSE, buttons, x, y, wheel, 0);
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
        
        // TODO: Reconfigure USB device descriptors if needed
    }
}

output_type_t usb_device_get_output_type(void) {
    return current_output_type;
}

// TinyUSB callbacks (would be implemented when TinyUSB is fully integrated)

// Invoked when received GET_REPORT control request
uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, 
                                hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen) {
    (void) instance;
    (void) report_id;
    (void) report_type;
    (void) buffer;
    (void) reqlen;
    
    return 0;
}

// Invoked when received SET_REPORT control request
void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, 
                            hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize) {
    (void) instance;
    (void) report_id;
    (void) report_type;
    
    // Check if this is a config mode request
    if (bufsize > 0 && buffer[0] == 0xFF) {
        config_mode_request = true;
    }
}
