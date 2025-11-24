/**
 * USB Device Module
 * 
 * Handles USB device functionality for outputting converted gamepad/keyboard/mouse data.
 */

#ifndef USB_DEVICE_H
#define USB_DEVICE_H

#include <stdbool.h>
#include <stdint.h>

// Output device types
typedef enum {
    OUTPUT_TYPE_GAMEPAD,
    OUTPUT_TYPE_KEYBOARD,
    OUTPUT_TYPE_MOUSE,
    OUTPUT_TYPE_COMBO  // Combined keyboard + mouse
} output_type_t;

/**
 * Initialize USB device
 * @return true on success, false on failure
 */
bool usb_device_init(void);

/**
 * USB device task - must be called regularly in main loop
 */
void usb_device_task(void);

/**
 * Send gamepad report
 * @param buttons Button state bitmap
 * @param axes Array of axis values
 * @param num_axes Number of axes
 */
void usb_device_send_gamepad(uint16_t buttons, int16_t *axes, uint8_t num_axes);

/**
 * Send keyboard report
 * @param modifiers Modifier keys (Ctrl, Alt, Shift, etc.)
 * @param keycodes Array of key codes (up to 6)
 * @param num_keys Number of keys pressed
 */
void usb_device_send_keyboard(uint8_t modifiers, uint8_t *keycodes, uint8_t num_keys);

/**
 * Send mouse report
 * @param buttons Mouse button state
 * @param x X movement (-127 to 127)
 * @param y Y movement (-127 to 127)
 * @param wheel Wheel movement (-127 to 127)
 */
void usb_device_send_mouse(uint8_t buttons, int8_t x, int8_t y, int8_t wheel);

/**
 * Check if config mode is requested (e.g., via USB control transfer)
 * @return true if config mode is requested
 */
bool usb_device_config_mode_requested(void);

/**
 * Set output device type
 * @param type Output type
 */
void usb_device_set_output_type(output_type_t type);

/**
 * Get current output device type
 * @return Current output type
 */
output_type_t usb_device_get_output_type(void);

#endif // USB_DEVICE_H
