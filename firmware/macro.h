/**
 * Macro System
 * 
 * Handles complex mouse and keyboard macros.
 */

#ifndef MACRO_H
#define MACRO_H

#include <stdbool.h>
#include <stdint.h>

#define MAX_MACROS 16
#define MAX_MACRO_STEPS 128

// Macro action types
typedef enum {
    MACRO_ACTION_KEY_PRESS,
    MACRO_ACTION_KEY_RELEASE,
    MACRO_ACTION_MOUSE_MOVE,
    MACRO_ACTION_MOUSE_BUTTON_PRESS,
    MACRO_ACTION_MOUSE_BUTTON_RELEASE,
    MACRO_ACTION_DELAY
} macro_action_type_t;

// Macro step
typedef struct {
    macro_action_type_t action;
    uint16_t param1;  // Key code, mouse button, or delay ms
    int16_t param2;   // Mouse X or 0
    int16_t param3;   // Mouse Y or 0
} macro_step_t;

// Macro definition
typedef struct {
    uint8_t id;
    uint8_t num_steps;
    macro_step_t steps[MAX_MACRO_STEPS];
} macro_t;

/**
 * Initialize macro system
 */
void macro_init(void);

/**
 * Execute a macro
 * @param macro_id Macro ID to execute
 * @return true on success, false if macro not found
 */
bool macro_execute(uint8_t macro_id);

/**
 * Add a macro
 * @param macro Macro definition
 * @return true on success, false if macro table is full
 */
bool macro_add(const macro_t *macro);

/**
 * Remove a macro
 * @param macro_id Macro ID to remove
 * @return true on success, false if macro not found
 */
bool macro_remove(uint8_t macro_id);

/**
 * Get macro by ID
 * @param macro_id Macro ID
 * @return Pointer to macro, or NULL if not found
 */
macro_t* macro_get(uint8_t macro_id);

/**
 * Clear all macros
 */
void macro_clear_all(void);

/**
 * Macro task - must be called regularly to execute pending macros
 */
void macro_task(void);

#endif // MACRO_H
