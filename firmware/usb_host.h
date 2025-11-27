/**
 * USB Host Module
 * 
 * Handles USB host functionality for receiving gamepad input.
 * Uses TinyUSB host stack for USB HID device support.
 */

#ifndef USB_HOST_H
#define USB_HOST_H

#include <stdbool.h>
#include <stdint.h>

// Gamepad button definitions (using JC_ prefix to avoid conflict with TinyUSB)
#define JC_BUTTON_A       (1 << 0)
#define JC_BUTTON_B       (1 << 1)
#define JC_BUTTON_X       (1 << 2)
#define JC_BUTTON_Y       (1 << 3)
#define JC_BUTTON_LB      (1 << 4)
#define JC_BUTTON_RB      (1 << 5)
#define JC_BUTTON_BACK    (1 << 6)
#define JC_BUTTON_START   (1 << 7)
#define JC_BUTTON_LS      (1 << 8)
#define JC_BUTTON_RS      (1 << 9)

// Input device types
typedef enum {
    INPUT_TYPE_UNKNOWN = 0,
    INPUT_TYPE_GAMEPAD,
    INPUT_TYPE_KEYBOARD
} input_type_t;

// Maximum number of keys that can be pressed simultaneously
#define MAX_KEYBOARD_KEYS 6

// Gamepad state structure
typedef struct {
    uint16_t buttons;      // Button state bitmap
    int16_t left_x;        // Left stick X (-32768 to 32767)
    int16_t left_y;        // Left stick Y
    int16_t right_x;       // Right stick X
    int16_t right_y;       // Right stick Y
    uint8_t left_trigger;  // Left trigger (0-255)
    uint8_t right_trigger; // Right trigger (0-255)
    int8_t dpad_x;         // D-pad X (-1, 0, 1)
    int8_t dpad_y;         // D-pad Y (-1, 0, 1)
} gamepad_state_t;

// Keyboard state structure for debug input
typedef struct {
    uint8_t modifiers;                    // Modifier keys (Ctrl, Alt, Shift, etc.)
    uint8_t keys[MAX_KEYBOARD_KEYS];      // Key codes currently pressed
    uint8_t num_keys;                     // Number of keys currently pressed
} keyboard_state_t;

// USB device info structure for detailed logging
typedef struct {
    uint16_t vid;              // Vendor ID
    uint16_t pid;              // Product ID
    uint8_t dev_addr;          // Device address
    uint8_t interface_num;     // Interface number
    uint8_t interface_class;   // Interface class
    uint8_t interface_subclass; // Interface subclass
    uint8_t interface_protocol; // Interface protocol
    input_type_t input_type;   // Detected input type
} usb_device_info_t;

/**
 * Initialize USB host
 * @return true on success, false on failure
 */
bool usb_host_init(void);

/**
 * USB host task - must be called regularly in main loop
 */
void usb_host_task(void);

/**
 * Check if a gamepad device is connected
 * @return true if connected, false otherwise
 */
bool usb_host_device_connected(void);

/**
 * Get current gamepad state
 * @param state Pointer to store gamepad state
 * @return true if state is valid, false otherwise
 */
bool usb_host_get_gamepad_state(gamepad_state_t *state);

/**
 * Get current keyboard state (for debug input)
 * @param state Pointer to store keyboard state
 * @return true if state is valid, false otherwise
 */
bool usb_host_get_keyboard_state(keyboard_state_t *state);

/**
 * Get current input device type
 * @return Current input device type
 */
input_type_t usb_host_get_input_type(void);

/**
 * Get connected device info
 * @param info Pointer to store device info
 * @return true if device is connected, false otherwise
 */
bool usb_host_get_device_info(usb_device_info_t *info);

#endif // USB_HOST_H
