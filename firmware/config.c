/**
 * Configuration Module Implementation
 */

#include "config.h"
#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/flash.h"
#include "hardware/sync.h"

#define CONFIG_MAGIC 0x4A435446  // "JCTF" - Joystick Converter Config

// Flash storage offset (use last sector of flash)
// Note: Actual offset should be calculated based on flash size
#define CONFIG_FLASH_OFFSET (PICO_FLASH_SIZE_BYTES - FLASH_SECTOR_SIZE)

static config_t current_config;

bool config_load(void) {
    printf("Config: Loading from flash...\n");
    
    // Read configuration from flash
    const uint8_t *flash_data = (const uint8_t *)(XIP_BASE + CONFIG_FLASH_OFFSET);
    memcpy(&current_config, flash_data, sizeof(config_t));
    
    // Validate magic number and version
    if (current_config.magic != CONFIG_MAGIC) {
        printf("Config: Invalid magic number, using defaults\n");
        return false;
    }
    
    if (current_config.version != CONFIG_VERSION) {
        printf("Config: Version mismatch, using defaults\n");
        return false;
    }
    
    printf("Config: Loaded %d mappings\n", current_config.num_mappings);
    return true;
}

bool config_save(void) {
    printf("Config: Saving to flash...\n");
    
    // Set magic and version
    current_config.magic = CONFIG_MAGIC;
    current_config.version = CONFIG_VERSION;
    
    // Disable interrupts during flash write
    uint32_t ints = save_and_disable_interrupts();
    
    // Erase sector
    flash_range_erase(CONFIG_FLASH_OFFSET, FLASH_SECTOR_SIZE);
    
    // Write configuration
    flash_range_program(CONFIG_FLASH_OFFSET, (const uint8_t*)&current_config, 
                        sizeof(config_t));
    
    // Restore interrupts
    restore_interrupts(ints);
    
    printf("Config: Saved successfully\n");
    return true;
}

void config_set_defaults(void) {
    printf("Config: Setting defaults\n");
    
    memset(&current_config, 0, sizeof(config_t));
    current_config.magic = CONFIG_MAGIC;
    current_config.version = CONFIG_VERSION;
    current_config.output_type = OUTPUT_TYPE_GAMEPAD;
    current_config.num_mappings = 0;
    
    // Add some default passthrough mappings
    // These just pass buttons through without modification
    // Users can customize via config software
}

config_t* config_get(void) {
    return &current_config;
}

bool config_add_mapping(uint16_t source_button, mapping_type_t type, 
                        uint16_t target_value, uint8_t macro_id) {
    if (current_config.num_mappings >= MAX_BUTTON_MAPPINGS) {
        printf("Config: Mapping table full\n");
        return false;
    }
    
    button_mapping_t *mapping = &current_config.mappings[current_config.num_mappings];
    mapping->source_button = source_button;
    mapping->type = type;
    mapping->target_value = target_value;
    mapping->macro_id = macro_id;
    
    current_config.num_mappings++;
    
    printf("Config: Added mapping for button 0x%04X\n", source_button);
    return true;
}

void config_clear_mappings(void) {
    current_config.num_mappings = 0;
    memset(current_config.mappings, 0, sizeof(current_config.mappings));
    printf("Config: Cleared all mappings\n");
}

button_mapping_t* config_find_mapping(uint16_t source_button) {
    for (uint8_t i = 0; i < current_config.num_mappings; i++) {
        if (current_config.mappings[i].source_button == source_button) {
            return &current_config.mappings[i];
        }
    }
    return NULL;
}
