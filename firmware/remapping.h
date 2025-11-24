/**
 * Remapping Engine
 * 
 * Handles input remapping and processing logic.
 */

#ifndef REMAPPING_H
#define REMAPPING_H

#include <stdbool.h>
#include <stdint.h>
#include "usb_host.h"

/**
 * Initialize remapping engine
 */
void remapping_init(void);

/**
 * Process input from gamepad and generate output
 * @param input Gamepad input state
 */
void remapping_process_input(const gamepad_state_t *input);

/**
 * Check if a button is pressed (with debouncing)
 * @param buttons Current button state
 * @param button Button to check
 * @return true if button is pressed and debounced
 */
bool remapping_is_button_pressed(uint16_t buttons, uint16_t button);

/**
 * Get button press event (rising edge detection)
 * @param button Button to check
 * @return true if button was just pressed
 */
bool remapping_button_pressed(uint16_t button);

/**
 * Get button release event (falling edge detection)
 * @param button Button to check
 * @return true if button was just released
 */
bool remapping_button_released(uint16_t button);

#endif // REMAPPING_H
