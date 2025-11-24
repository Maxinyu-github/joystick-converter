/**
 * Configuration Module
 * 
 * Handles configuration storage and retrieval from flash memory.
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <stdbool.h>
#include <stdint.h>
#include "usb_device.h"

#define CONFIG_VERSION 1
#define MAX_BUTTON_MAPPINGS 32
#define MAX_MACRO_STEPS 128

// Button mapping types
typedef enum {
    MAPPING_TYPE_NONE,
    MAPPING_TYPE_BUTTON,      // Map to another button
    MAPPING_TYPE_KEY,         // Map to keyboard key
    MAPPING_TYPE_MOUSE_BUTTON, // Map to mouse button
    MAPPING_TYPE_MACRO        // Execute macro
} mapping_type_t;

// Button mapping entry
typedef struct {
    uint16_t source_button;   // Source button bit
    mapping_type_t type;      // Mapping type
    uint16_t target_value;    // Target button/key code
    uint8_t macro_id;         // Macro ID if type is MAPPING_TYPE_MACRO
} button_mapping_t;

// Configuration structure
typedef struct {
    uint32_t magic;           // Magic number for validation
    uint32_t version;         // Config version
    output_type_t output_type; // Output device type
    uint8_t num_mappings;     // Number of button mappings
    button_mapping_t mappings[MAX_BUTTON_MAPPINGS];
    // Add more configuration options as needed
} config_t;

/**
 * Load configuration from flash
 * @return true on success, false on failure
 */
bool config_load(void);

/**
 * Save configuration to flash
 * @return true on success, false on failure
 */
bool config_save(void);

/**
 * Set default configuration
 */
void config_set_defaults(void);

/**
 * Get current configuration
 * @return Pointer to current configuration
 */
config_t* config_get(void);

/**
 * Add button mapping
 * @param source_button Source button
 * @param type Mapping type
 * @param target_value Target value
 * @param macro_id Macro ID (if applicable)
 * @return true on success, false if mapping table is full
 */
bool config_add_mapping(uint16_t source_button, mapping_type_t type, 
                        uint16_t target_value, uint8_t macro_id);

/**
 * Clear all button mappings
 */
void config_clear_mappings(void);

/**
 * Find button mapping
 * @param source_button Source button to look up
 * @return Pointer to mapping, or NULL if not found
 */
button_mapping_t* config_find_mapping(uint16_t source_button);

#endif // CONFIG_H
