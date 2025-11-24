/**
 * USB Host Module
 * 
 * Handles USB host functionality for receiving gamepad input.
 */

#ifndef USB_HOST_H
#define USB_HOST_H

#include <stdbool.h>
#include <stdint.h>

// Gamepad button definitions
#define GAMEPAD_BUTTON_A       (1 << 0)
#define GAMEPAD_BUTTON_B       (1 << 1)
#define GAMEPAD_BUTTON_X       (1 << 2)
#define GAMEPAD_BUTTON_Y       (1 << 3)
#define GAMEPAD_BUTTON_LB      (1 << 4)
#define GAMEPAD_BUTTON_RB      (1 << 5)
#define GAMEPAD_BUTTON_BACK    (1 << 6)
#define GAMEPAD_BUTTON_START   (1 << 7)
#define GAMEPAD_BUTTON_LS      (1 << 8)
#define GAMEPAD_BUTTON_RS      (1 << 9)

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

#endif // USB_HOST_H
