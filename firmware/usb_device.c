/**
 * USB Device Module Implementation
 * 
 * This module provides USB device output functionality using the native USB controller.
 * The device presents as a composite USB device with CDC (serial) and HID interfaces.
 * 
 * Hardware Configuration for Waveshare RP2350-PiZero:
 * - Native USB (Port 0): USB Device for HID output + CDC serial (connects to PC)
 * - PIO-USB (Port 1): USB Host for controller input (handled by usb_host.c)
 */

#include "usb_device.h"
#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "tusb.h"
#include "logging.h"

static output_type_t current_output_type = OUTPUT_TYPE_GAMEPAD;
static bool config_mode_request = false;

// HID Report IDs (must match usb_descriptors.c)
#define REPORT_ID_GAMEPAD   1
#define REPORT_ID_KEYBOARD  2
#define REPORT_ID_MOUSE     3

// Gamepad report structure (without report_id - TinyUSB adds it)
typedef struct __attribute__((packed)) {
    uint16_t buttons;
    int16_t left_x;
    int16_t left_y;
    int16_t right_x;
    int16_t right_y;
    uint8_t left_trigger;
    uint8_t right_trigger;
    uint8_t hat;  // D-pad hat switch (0-7 for directions, 8 for center)
} gamepad_report_t;

// Keyboard report structure (without report_id - TinyUSB adds it)
typedef struct __attribute__((packed)) {
    uint8_t modifiers;
    uint8_t reserved;
    uint8_t keycodes[6];
} keyboard_report_t;

// Mouse report structure (without report_id - TinyUSB adds it)
typedef struct __attribute__((packed)) {
    uint8_t buttons;
    int8_t x;
    int8_t y;
    int8_t wheel;
} mouse_report_t;

bool usb_device_init(void) {
    LOG_INFO("USB Device: Initializing native USB device stack...");
    
    // Initialize TinyUSB device stack on native USB (port 0)
    if (!tud_init(BOARD_TUD_RHPORT)) {
        LOG_ERROR("USB Device: Failed to initialize TinyUSB device");
        return false;
    }
    
    config_mode_request = false;
    
    LOG_INFO("USB Device: Native USB device stack initialized on port %d", BOARD_TUD_RHPORT);
    return true;
}

void usb_device_task(void) {
    // Process USB device events via TinyUSB
    tud_task();
}

void usb_device_send_gamepad(uint16_t buttons, int16_t *axes, uint8_t num_axes) {
    if (current_output_type != OUTPUT_TYPE_GAMEPAD) {
        return;
    }
    
    // Only send if device is ready
    if (!tud_hid_ready()) {
        return;
    }
    
    gamepad_report_t report = {0};
    report.buttons = buttons;
    
    // Copy axes if provided
    if (axes && num_axes >= 4) {
        report.left_x = axes[0];
        report.left_y = axes[1];
        report.right_x = axes[2];
        report.right_y = axes[3];
    }
    if (axes && num_axes >= 6) {
        report.left_trigger = (uint8_t)((axes[4] + 32768) >> 8);  // Convert from int16 to uint8
        report.right_trigger = (uint8_t)((axes[5] + 32768) >> 8);
    }
    
    // Hat switch is centered by default
    report.hat = 8;  // 8 = center/no direction
    
    tud_hid_report(REPORT_ID_GAMEPAD, &report, sizeof(report));
}

void usb_device_send_keyboard(uint8_t modifiers, uint8_t *keycodes, uint8_t num_keys) {
    if (current_output_type != OUTPUT_TYPE_KEYBOARD && 
        current_output_type != OUTPUT_TYPE_COMBO) {
        return;
    }
    
    if (!tud_hid_ready()) {
        return;
    }
    
    keyboard_report_t report = {0};
    report.modifiers = modifiers;
    report.reserved = 0;
    
    // Copy keycodes (up to 6)
    if (keycodes && num_keys > 0) {
        uint8_t copy_count = num_keys > 6 ? 6 : num_keys;
        memcpy(report.keycodes, keycodes, copy_count);
    }
    
    tud_hid_report(REPORT_ID_KEYBOARD, &report, sizeof(report));
}

void usb_device_send_mouse(uint8_t buttons, int8_t x, int8_t y, int8_t wheel) {
    if (current_output_type != OUTPUT_TYPE_MOUSE && 
        current_output_type != OUTPUT_TYPE_COMBO) {
        return;
    }
    
    if (!tud_hid_ready()) {
        return;
    }
    
    mouse_report_t report = {0};
    report.buttons = buttons;
    report.x = x;
    report.y = y;
    report.wheel = wheel;
    
    tud_hid_report(REPORT_ID_MOUSE, &report, sizeof(report));
}

bool usb_device_config_mode_requested(void) {
    bool request = config_mode_request;
    config_mode_request = false;  // Clear flag
    return request;
}

void usb_device_set_output_type(output_type_t type) {
    if (type != current_output_type) {
        LOG_INFO("USB Device: Output type changed to %d", type);
        current_output_type = type;
    }
}

output_type_t usb_device_get_output_type(void) {
    return current_output_type;
}

//--------------------------------------------------------------------
// TinyUSB HID Device Callbacks
//--------------------------------------------------------------------

// Invoked when received GET_REPORT control request
uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, 
                                hid_report_type_t report_type, uint8_t *buffer, uint16_t reqlen) {
    (void)instance;
    (void)report_id;
    (void)report_type;
    (void)buffer;
    (void)reqlen;
    
    return 0;
}

// Invoked when received SET_REPORT control request
void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, 
                           hid_report_type_t report_type, uint8_t const *buffer, uint16_t bufsize) {
    (void)instance;
    (void)report_id;
    (void)report_type;
    (void)buffer;
    (void)bufsize;
    
    // Could be used to receive LED status for keyboard, etc.
}
