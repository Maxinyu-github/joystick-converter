/**
 * USB Host Module Implementation
 */

#include "usb_host.h"
#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "remapping.h"
#include "logging.h"

// Current gamepad state
static gamepad_state_t current_gamepad_state = {0};
// Current keyboard state
static keyboard_state_t current_keyboard_state = {0};
// Connected device info
static usb_device_info_t device_info = {0};
static bool device_connected = false;
static bool gamepad_state_valid = false;
static bool keyboard_state_valid = false;
static input_type_t current_input_type = INPUT_TYPE_UNKNOWN;

// Known controller VID/PID for logging
typedef struct {
    uint16_t vid;
    uint16_t pid;
    const char *name;
} known_device_t;

// Boot protocol keyboard report constants
#define KEYBOARD_REPORT_SIZE 8
#define KEYBOARD_REPORT_KEY_START 2

static const known_device_t known_devices[] = {
    // Nintendo controllers
    {0x057E, 0x2009, "Nintendo Switch Pro Controller"},
    {0x057E, 0x200E, "Nintendo Switch Pro Controller 2"},  // Second generation Switch Pro Controller
    {0x057E, 0x2017, "Nintendo Switch SNES Controller"},
    {0x057E, 0x2019, "Nintendo Switch N64 Controller"},
    {0x057E, 0x201E, "Nintendo Switch Online Controller"},
    // Xbox controllers
    {0x045E, 0x028E, "Xbox 360 Controller"},
    {0x045E, 0x02FF, "Xbox One Controller"},
    {0x045E, 0x0B12, "Xbox Series X Controller"},
    // Sony controllers
    {0x054C, 0x0268, "PlayStation 3 Controller"},
    {0x054C, 0x05C4, "PlayStation 4 Controller"},
    {0x054C, 0x09CC, "PlayStation 4 Controller v2"},
    {0x054C, 0x0CE6, "PlayStation 5 DualSense"},
    // Generic/Other
    {0x0000, 0x0000, NULL}  // End marker
};

// Helper function to get device name from VID/PID
static const char* get_device_name(uint16_t vid, uint16_t pid) {
    for (size_t i = 0; known_devices[i].name != NULL; i++) {
        if (known_devices[i].vid == vid && known_devices[i].pid == pid) {
            return known_devices[i].name;
        }
    }
    return "Unknown Device";
}

// Helper function to get input type name
static const char* get_input_type_name(input_type_t type) {
    switch (type) {
        case INPUT_TYPE_GAMEPAD:
            return "Gamepad";
        case INPUT_TYPE_KEYBOARD:
            return "Keyboard";
        default:
            return "Unknown";
    }
}

bool usb_host_init(void) {
    LOG_INFO("USB Host: Initializing...");
    
    // TODO: Initialize TinyUSB host stack
    // This would involve setting up USB host controller,
    // HID class driver for gamepad devices
    
    memset(&current_gamepad_state, 0, sizeof(current_gamepad_state));
    memset(&current_keyboard_state, 0, sizeof(current_keyboard_state));
    memset(&device_info, 0, sizeof(device_info));
    device_connected = false;
    gamepad_state_valid = false;
    keyboard_state_valid = false;
    current_input_type = INPUT_TYPE_UNKNOWN;
    
    LOG_INFO("USB Host: Initialized (placeholder)");
    return true;
}

void usb_host_task(void) {
    // TODO: Process USB host events
    // This would involve:
    // 1. Polling USB host controller
    // 2. Processing HID reports from gamepad or keyboard
    // 3. Updating current state
    // 4. Calling remapping engine with new state (for gamepad)
    
    if (device_connected) {
        if (current_input_type == INPUT_TYPE_GAMEPAD && gamepad_state_valid) {
            // Process the gamepad state through remapping
            remapping_process_input(&current_gamepad_state);
        }
        // Keyboard input is handled separately for debug purposes
    }
}

bool usb_host_device_connected(void) {
    return device_connected;
}

bool usb_host_get_gamepad_state(gamepad_state_t *state) {
    if (!gamepad_state_valid || current_input_type != INPUT_TYPE_GAMEPAD) {
        return false;
    }
    
    memcpy(state, &current_gamepad_state, sizeof(gamepad_state_t));
    return true;
}

bool usb_host_get_keyboard_state(keyboard_state_t *state) {
    if (!keyboard_state_valid || current_input_type != INPUT_TYPE_KEYBOARD) {
        return false;
    }
    
    memcpy(state, &current_keyboard_state, sizeof(keyboard_state_t));
    return true;
}

input_type_t usb_host_get_input_type(void) {
    return current_input_type;
}

bool usb_host_get_device_info(usb_device_info_t *info) {
    if (!device_connected || !info) {
        return false;
    }
    
    memcpy(info, &device_info, sizeof(usb_device_info_t));
    return true;
}

// Callback for when device is mounted (called by TinyUSB)
void tuh_hid_mount_cb(uint8_t dev_addr, uint8_t instance, uint8_t const* desc_report, uint16_t desc_len) {
    (void)desc_report;
    (void)desc_len;
    
    // Store device address
    device_info.dev_addr = dev_addr;
    device_info.interface_num = instance;
    
    // Log basic mount information
    LOG_INFO("USB Host: Device mounted - addr=%d, instance=%d", dev_addr, instance);
    
    // TODO: When TinyUSB is integrated, get VID/PID from device descriptor
    // For now, log with placeholder values
    // tuh_vid_pid_get(dev_addr, &device_info.vid, &device_info.pid);
    
    // Log detailed device info
    // NOTE: VID/PID will be populated when TinyUSB is integrated
    // For now they will be 0 until tuh_vid_pid_get() is called
    LOG_DEBUG("USB Host: VID=0x%04X, PID=0x%04X", device_info.vid, device_info.pid);
    if (device_info.vid == 0 && device_info.pid == 0) {
        LOG_DEBUG("USB Host: Device info pending TinyUSB integration");
    } else {
        LOG_DEBUG("USB Host: Device name: %s", get_device_name(device_info.vid, device_info.pid));
    }
    
    // TODO: Determine input type from interface descriptor
    // For now, default to gamepad; keyboard detection would be based on
    // interface class (0x03 for HID), subclass (0x01 for boot interface),
    // and protocol (0x01 for keyboard, 0x02 for mouse)
    
    // Example HID class values for reference:
    // Keyboard: class=0x03, subclass=0x01, protocol=0x01
    // Mouse: class=0x03, subclass=0x01, protocol=0x02
    // Gamepad: class=0x03, subclass=0x00, protocol=0x00 (typically)
    
    // When TinyUSB is integrated:
    // uint8_t itf_class = tuh_hid_interface_protocol(dev_addr, instance);
    // if (itf_class == HID_ITF_PROTOCOL_KEYBOARD) {
    //     current_input_type = INPUT_TYPE_KEYBOARD;
    // } else {
    //     current_input_type = INPUT_TYPE_GAMEPAD;
    // }
    
    // For now, default to gamepad
    current_input_type = INPUT_TYPE_GAMEPAD;
    device_info.input_type = current_input_type;
    
    LOG_INFO("USB Host: Input type detected: %s", get_input_type_name(current_input_type));
    
    device_connected = true;
    
    // Request first HID report
    // tuh_hid_receive_report(dev_addr, instance);
}

// Callback for when device is unmounted (called by TinyUSB)
void tuh_hid_umount_cb(uint8_t dev_addr, uint8_t instance) {
    LOG_INFO("USB Host: Device unmounted - addr=%d, instance=%d", dev_addr, instance);
    
    // Log device info before clearing
    if (device_info.vid != 0 || device_info.pid != 0) {
        LOG_DEBUG("USB Host: Disconnected device: %s (VID=0x%04X, PID=0x%04X)", 
                  get_device_name(device_info.vid, device_info.pid),
                  device_info.vid, device_info.pid);
    }
    
    device_connected = false;
    gamepad_state_valid = false;
    keyboard_state_valid = false;
    current_input_type = INPUT_TYPE_UNKNOWN;
    memset(&current_gamepad_state, 0, sizeof(current_gamepad_state));
    memset(&current_keyboard_state, 0, sizeof(current_keyboard_state));
    memset(&device_info, 0, sizeof(device_info));
}

// Callback for HID report received (called by TinyUSB)
void tuh_hid_report_received_cb(uint8_t dev_addr, uint8_t instance, uint8_t const* report, uint16_t len) {
    (void)dev_addr;
    (void)instance;
    
    if (current_input_type == INPUT_TYPE_KEYBOARD) {
        // Parse keyboard HID report (standard boot protocol keyboard)
        // Boot protocol keyboard report format (8 bytes):
        // Byte 0: Modifier keys (Ctrl, Shift, Alt, GUI)
        // Byte 1: Reserved
        // Bytes 2-7: Key codes (up to 6 keys)
        if (len >= KEYBOARD_REPORT_SIZE) {
            current_keyboard_state.modifiers = report[0];
            current_keyboard_state.num_keys = 0;
            
            for (int i = KEYBOARD_REPORT_KEY_START; i < KEYBOARD_REPORT_SIZE && current_keyboard_state.num_keys < MAX_KEYBOARD_KEYS; i++) {
                if (report[i] != 0) {
                    current_keyboard_state.keys[current_keyboard_state.num_keys++] = report[i];
                }
            }
            
            keyboard_state_valid = true;
            
            // Log keyboard state for debugging
            if (current_keyboard_state.num_keys > 0 || current_keyboard_state.modifiers != 0) {
                LOG_DEBUG("Keyboard: mod=0x%02X, keys=%d", 
                          current_keyboard_state.modifiers, 
                          current_keyboard_state.num_keys);
            }
        }
    } else {
        // Parse HID report and update gamepad state
        // This is a simplified parser - actual implementation would need to
        // handle different gamepad types and report formats
        
        if (len >= 8) {
            // Example parsing for standard gamepad report
            current_gamepad_state.buttons = report[0] | (report[1] << 8);
            current_gamepad_state.left_x = (int16_t)(report[2] | (report[3] << 8));
            current_gamepad_state.left_y = (int16_t)(report[4] | (report[5] << 8));
            
            if (len >= 12) {
                current_gamepad_state.right_x = (int16_t)(report[6] | (report[7] << 8));
                current_gamepad_state.right_y = (int16_t)(report[8] | (report[9] << 8));
                current_gamepad_state.left_trigger = report[10];
                current_gamepad_state.right_trigger = report[11];
            }
            
            gamepad_state_valid = true;
        }
    }
    
    // Request next report
    // tuh_hid_receive_report(dev_addr, instance);
}
