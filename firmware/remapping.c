/**
 * Remapping Engine Implementation
 */

#include "remapping.h"
#include "config.h"
#include "usb_device.h"
#include "macro.h"
#include <stdio.h>
#include <string.h>

static uint16_t previous_buttons = 0;
static gamepad_state_t last_input = {0};

void remapping_init(void) {
    printf("Remapping: Initializing\n");
    previous_buttons = 0;
    memset(&last_input, 0, sizeof(last_input));
}

void remapping_process_input(const gamepad_state_t *input) {
    if (!input) {
        return;
    }
    
    config_t *cfg = config_get();
    
    // Process button events
    uint16_t button_changes = input->buttons ^ previous_buttons;
    
    // Check each button for changes
    for (uint8_t i = 0; i < 16; i++) {
        uint16_t button_bit = (1 << i);
        
        if (button_changes & button_bit) {
            // Button state changed
            bool pressed = (input->buttons & button_bit) != 0;
            
            // Look up mapping for this button
            button_mapping_t *mapping = config_find_mapping(button_bit);
            
            if (mapping) {
                // Apply mapping
                switch (mapping->type) {
                    case MAPPING_TYPE_BUTTON:
                        // Map to different button
                        printf("Remapping: Button 0x%04X -> Button 0x%04X (%s)\n", 
                               button_bit, mapping->target_value, pressed ? "pressed" : "released");
                        break;
                        
                    case MAPPING_TYPE_KEY:
                        // Map to keyboard key
                        if (pressed) {
                            uint8_t keycode = (uint8_t)mapping->target_value;
                            usb_device_send_keyboard(0, &keycode, 1);
                            printf("Remapping: Button 0x%04X -> Key 0x%02X\n", 
                                   button_bit, keycode);
                        } else {
                            // Release all keys - pass empty array
                            uint8_t no_keys = 0;
                            usb_device_send_keyboard(0, &no_keys, 0);
                        }
                        break;
                        
                    case MAPPING_TYPE_MOUSE_BUTTON:
                        // Map to mouse button
                        if (pressed) {
                            usb_device_send_mouse((uint8_t)mapping->target_value, 0, 0, 0);
                            printf("Remapping: Button 0x%04X -> Mouse Button 0x%02X\n", 
                                   button_bit, mapping->target_value);
                        } else {
                            usb_device_send_mouse(0, 0, 0, 0);
                        }
                        break;
                        
                    case MAPPING_TYPE_MACRO:
                        // Execute macro
                        if (pressed) {
                            macro_execute(mapping->macro_id);
                            printf("Remapping: Button 0x%04X -> Macro %d\n", 
                                   button_bit, mapping->macro_id);
                        }
                        break;
                        
                    default:
                        break;
                }
            } else {
                // No mapping, pass through
                if (cfg->output_type == OUTPUT_TYPE_GAMEPAD) {
                    // Passthrough mode - just send the original input
                }
            }
        }
    }
    
    // If no mappings are active, send gamepad data in passthrough mode
    if (cfg->output_type == OUTPUT_TYPE_GAMEPAD && cfg->num_mappings == 0) {
        int16_t axes[6] = {
            input->left_x,
            input->left_y,
            input->right_x,
            input->right_y,
            (int16_t)input->left_trigger,
            (int16_t)input->right_trigger
        };
        usb_device_send_gamepad(input->buttons, axes, 6);
    }
    
    // Process analog sticks for mouse emulation if needed
    if (cfg->output_type == OUTPUT_TYPE_MOUSE || cfg->output_type == OUTPUT_TYPE_COMBO) {
        // Convert right stick to mouse movement
        int8_t mouse_x = (int8_t)(input->right_x / 256);
        int8_t mouse_y = (int8_t)(input->right_y / 256);
        
        if (mouse_x != 0 || mouse_y != 0) {
            usb_device_send_mouse(0, mouse_x, mouse_y, 0);
        }
    }
    
    // Update state
    previous_buttons = input->buttons;
    memcpy(&last_input, input, sizeof(gamepad_state_t));
}

bool remapping_is_button_pressed(uint16_t buttons, uint16_t button) {
    return (buttons & button) != 0;
}

bool remapping_button_pressed(uint16_t button) {
    return (last_input.buttons & button) && !(previous_buttons & button);
}

bool remapping_button_released(uint16_t button) {
    return !(last_input.buttons & button) && (previous_buttons & button);
}
