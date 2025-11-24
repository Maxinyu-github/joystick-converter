/**
 * Joystick Converter - Main Entry Point
 * 
 * This firmware converts gamepad inputs to compatible outputs for various devices.
 * Supports button remapping and complex mouse macro functionality.
 * 
 * Hardware: RP2350-PiZero with PiPUSB and Type-C connectivity
 */

#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "pico/unique_id.h"
#include "hardware/gpio.h"
#include "hardware/watchdog.h"

#include "usb_host.h"
#include "usb_device.h"
#include "config.h"
#include "remapping.h"
#include "macro.h"

// LED pin for status indication
#define LED_PIN 25

// Application state
typedef enum {
    STATE_INIT,
    STATE_WAITING_FOR_INPUT,
    STATE_ACTIVE,
    STATE_CONFIG_MODE,
    STATE_ERROR
} app_state_t;

static app_state_t current_state = STATE_INIT;

/**
 * Initialize hardware peripherals
 */
static void hardware_init(void) {
    // Initialize stdio for debugging
    stdio_init_all();
    
    // Initialize LED
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, 1);
    
    printf("Joystick Converter Starting...\n");
    printf("Hardware: RP2350-PiZero\n");
}

/**
 * Blink LED to indicate status
 */
static void status_led_update(void) {
    static uint32_t last_blink = 0;
    static bool led_state = false;
    uint32_t now = to_ms_since_boot(get_absolute_time());
    
    uint32_t blink_interval;
    switch (current_state) {
        case STATE_INIT:
            blink_interval = 100;  // Fast blink during init
            break;
        case STATE_WAITING_FOR_INPUT:
            blink_interval = 500;  // Medium blink waiting for device
            break;
        case STATE_ACTIVE:
            blink_interval = 2000; // Slow blink when active
            break;
        case STATE_CONFIG_MODE:
            blink_interval = 200;  // Fast blink in config mode
            break;
        case STATE_ERROR:
            blink_interval = 100;  // Very fast blink on error
            break;
        default:
            blink_interval = 1000;
            break;
    }
    
    if (now - last_blink >= blink_interval) {
        led_state = !led_state;
        gpio_put(LED_PIN, led_state);
        last_blink = now;
    }
}

/**
 * Main application loop
 */
int main(void) {
    // Initialize hardware
    hardware_init();
    
    // Load configuration from flash
    if (!config_load()) {
        printf("Warning: Failed to load config, using defaults\n");
        config_set_defaults();
    }
    
    // Initialize USB host for gamepad input
    if (!usb_host_init()) {
        printf("Error: Failed to initialize USB host\n");
        current_state = STATE_ERROR;
    } else {
        current_state = STATE_WAITING_FOR_INPUT;
    }
    
    // Initialize USB device for output
    if (!usb_device_init()) {
        printf("Error: Failed to initialize USB device\n");
        current_state = STATE_ERROR;
    }
    
    // Initialize remapping engine
    remapping_init();
    
    // Initialize macro system
    macro_init();
    
    printf("Initialization complete. Waiting for gamepad...\n");
    
    // Main loop
    while (1) {
        // Update LED status
        status_led_update();
        
        // Process USB host (gamepad input)
        usb_host_task();
        
        // Process USB device (output)
        usb_device_task();
        
        // Process macro execution
        macro_task();
        
        // Update application state
        if (current_state == STATE_WAITING_FOR_INPUT) {
            if (usb_host_device_connected()) {
                current_state = STATE_ACTIVE;
                printf("Gamepad connected!\n");
            }
        } else if (current_state == STATE_ACTIVE) {
            if (!usb_host_device_connected()) {
                current_state = STATE_WAITING_FOR_INPUT;
                printf("Gamepad disconnected\n");
            }
        }
        
        // Check for config mode entry (could be via special button combo)
        if (usb_device_config_mode_requested()) {
            current_state = STATE_CONFIG_MODE;
            printf("Entering configuration mode\n");
        }
        
        // Small delay to prevent busy waiting and reduce power consumption
        sleep_ms(1);
    }
    
    return 0;
}
