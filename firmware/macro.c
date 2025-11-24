/**
 * Macro System Implementation
 */

#include "macro.h"
#include "usb_device.h"
#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"

static macro_t macros[MAX_MACROS];
static uint8_t num_macros = 0;

// Macro execution state
static struct {
    bool executing;
    uint8_t current_macro_id;
    uint8_t current_step;
    uint32_t step_start_time;
} macro_state = {0};

void macro_init(void) {
    printf("Macro: Initializing\n");
    num_macros = 0;
    memset(macros, 0, sizeof(macros));
    memset(&macro_state, 0, sizeof(macro_state));
}

bool macro_execute(uint8_t macro_id) {
    macro_t *macro = macro_get(macro_id);
    if (!macro) {
        printf("Macro: Macro %d not found\n", macro_id);
        return false;
    }
    
    if (macro_state.executing) {
        printf("Macro: Already executing macro %d, ignoring request for %d\n", 
               macro_state.current_macro_id, macro_id);
        return false;
    }
    
    printf("Macro: Executing macro %d with %d steps\n", macro_id, macro->num_steps);
    
    macro_state.executing = true;
    macro_state.current_macro_id = macro_id;
    macro_state.current_step = 0;
    macro_state.step_start_time = to_ms_since_boot(get_absolute_time());
    
    return true;
}

void macro_task(void) {
    if (!macro_state.executing) {
        return;
    }
    
    macro_t *macro = macro_get(macro_state.current_macro_id);
    if (!macro || macro_state.current_step >= macro->num_steps) {
        // Macro finished or invalid
        macro_state.executing = false;
        printf("Macro: Execution complete\n");
        return;
    }
    
    macro_step_t *step = &macro->steps[macro_state.current_step];
    uint32_t now = to_ms_since_boot(get_absolute_time());
    
    // Execute current step
    switch (step->action) {
        case MACRO_ACTION_KEY_PRESS: {
            uint8_t keycode = (uint8_t)step->param1;
            usb_device_send_keyboard(0, &keycode, 1);
            printf("Macro: Key press 0x%02X\n", keycode);
            macro_state.current_step++;
            macro_state.step_start_time = now;
            break;
        }
        
        case MACRO_ACTION_KEY_RELEASE: {
            usb_device_send_keyboard(0, NULL, 0);
            printf("Macro: Key release\n");
            macro_state.current_step++;
            macro_state.step_start_time = now;
            break;
        }
        
        case MACRO_ACTION_MOUSE_MOVE: {
            int8_t x = (int8_t)step->param2;
            int8_t y = (int8_t)step->param3;
            usb_device_send_mouse(0, x, y, 0);
            printf("Macro: Mouse move (%d, %d)\n", x, y);
            macro_state.current_step++;
            macro_state.step_start_time = now;
            break;
        }
        
        case MACRO_ACTION_MOUSE_BUTTON_PRESS: {
            uint8_t buttons = (uint8_t)step->param1;
            usb_device_send_mouse(buttons, 0, 0, 0);
            printf("Macro: Mouse button press 0x%02X\n", buttons);
            macro_state.current_step++;
            macro_state.step_start_time = now;
            break;
        }
        
        case MACRO_ACTION_MOUSE_BUTTON_RELEASE: {
            usb_device_send_mouse(0, 0, 0, 0);
            printf("Macro: Mouse button release\n");
            macro_state.current_step++;
            macro_state.step_start_time = now;
            break;
        }
        
        case MACRO_ACTION_DELAY: {
            uint32_t delay_ms = step->param1;
            if (now - macro_state.step_start_time >= delay_ms) {
                printf("Macro: Delay %d ms complete\n", delay_ms);
                macro_state.current_step++;
                macro_state.step_start_time = now;
            }
            break;
        }
        
        default:
            printf("Macro: Unknown action %d\n", step->action);
            macro_state.current_step++;
            break;
    }
}

bool macro_add(const macro_t *macro) {
    if (num_macros >= MAX_MACROS) {
        printf("Macro: Macro table full\n");
        return false;
    }
    
    // Check if macro with this ID already exists
    for (uint8_t i = 0; i < num_macros; i++) {
        if (macros[i].id == macro->id) {
            // Replace existing macro
            memcpy(&macros[i], macro, sizeof(macro_t));
            printf("Macro: Updated macro %d\n", macro->id);
            return true;
        }
    }
    
    // Add new macro
    memcpy(&macros[num_macros], macro, sizeof(macro_t));
    num_macros++;
    printf("Macro: Added macro %d with %d steps\n", macro->id, macro->num_steps);
    return true;
}

bool macro_remove(uint8_t macro_id) {
    for (uint8_t i = 0; i < num_macros; i++) {
        if (macros[i].id == macro_id) {
            // Shift remaining macros down
            for (uint8_t j = i; j < num_macros - 1; j++) {
                memcpy(&macros[j], &macros[j + 1], sizeof(macro_t));
            }
            num_macros--;
            printf("Macro: Removed macro %d\n", macro_id);
            return true;
        }
    }
    
    printf("Macro: Macro %d not found\n", macro_id);
    return false;
}

macro_t* macro_get(uint8_t macro_id) {
    for (uint8_t i = 0; i < num_macros; i++) {
        if (macros[i].id == macro_id) {
            return &macros[i];
        }
    }
    return NULL;
}

void macro_clear_all(void) {
    num_macros = 0;
    memset(macros, 0, sizeof(macros));
    printf("Macro: Cleared all macros\n");
}
