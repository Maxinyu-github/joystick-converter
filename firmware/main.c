/**
 * Joystick Converter - Main Entry Point
 * 
 * This firmware converts gamepad inputs to compatible outputs for various devices.
 * Supports button remapping and complex mouse macro functionality.
 * 
 * Hardware: Waveshare RP2350-PiZero with dual USB:
 * - Native USB (Type-C): USB Device for HID output + CDC serial (connects to PC)
 * - PIO-USB (Type-C): USB Host for controller input
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pico/stdlib.h"
#include "pico/unique_id.h"
#include "pico/multicore.h"
#include "hardware/gpio.h"
#include "hardware/watchdog.h"
#include "hardware/clocks.h"

#include "usb_host.h"
#include "usb_device.h"
#include "config.h"
#include "remapping.h"
#include "macro.h"
#include "logging.h"

// LED pin for status indication
#define LED_PIN 25

// Application state
typedef enum {
    STATE_INIT,
    STATE_WAITING_FOR_INPUT,
    STATE_ACTIVE,
    STATE_CONFIG_MODE,
    STATE_DEBUG_MODE,
    STATE_ERROR
} app_state_t;

static app_state_t current_state = STATE_INIT;
static bool debug_mode_enabled = false;

/**
 * Initialize hardware peripherals
 */
static void hardware_init(void) {
    // Set system clock to 120MHz for PIO-USB compatibility
    // PIO-USB requires the system clock to be a multiple of 12MHz
    set_sys_clock_khz(120000, true);
    
    // Small delay for clock stabilization
    sleep_ms(10);
    
    // Initialize stdio for debugging (uses USB CDC)
    stdio_init_all();
    
    // Initialize LED
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, 1);
    
    printf("Joystick Converter Starting...\n");
    printf("Hardware: Waveshare RP2350-PiZero (Dual USB)\n");
    printf("System clock: %lu Hz\n", clock_get_hz(clk_sys));
}

// Command buffer size for serial commands
#define CMD_BUFFER_SIZE 32

// Buffer for log output (shared across log commands)
static char log_output_buffer[LOG_BUFFER_SIZE];

/**
 * Handle serial commands for debug mode and logging
 */
static void handle_serial_commands(void) {
    static char cmd_buffer[CMD_BUFFER_SIZE];
    static int cmd_pos = 0;
    
    // Check if data is available
    int c = getchar_timeout_us(0);
    if (c == PICO_ERROR_TIMEOUT) {
        return;
    }
    
    // Validate character is printable or newline
    if (c < 0 || (c > 127 && c != '\n' && c != '\r')) {
        // Invalid character, reset buffer
        cmd_pos = 0;
        return;
    }
    
    // Add character to buffer
    if (c == '\n' || c == '\r') {
        if (cmd_pos > 0) {
            cmd_buffer[cmd_pos] = '\0';
            
            // Process command
            if (strcmp(cmd_buffer, "DEBUG_START") == 0) {
                debug_mode_enabled = true;
                if (usb_host_device_connected()) {
                    current_state = STATE_DEBUG_MODE;
                }
                printf("DEBUG_MODE_STARTED\n");
            } else if (strcmp(cmd_buffer, "DEBUG_STOP") == 0) {
                debug_mode_enabled = false;
                if (current_state == STATE_DEBUG_MODE) {
                    current_state = STATE_ACTIVE;
                }
                printf("DEBUG_MODE_STOPPED\n");
            } else if (strcmp(cmd_buffer, "DEBUG_GET") == 0) {
                // Send current input state based on input type
                if (debug_mode_enabled && usb_host_device_connected()) {
                    input_type_t input_type = usb_host_get_input_type();
                    
                    if (input_type == INPUT_TYPE_KEYBOARD) {
                        keyboard_state_t state;
                        if (usb_host_get_keyboard_state(&state)) {
                            // Format: "DEBUG_KB:modifiers,num_keys,key0,key1,key2,key3,key4,key5"
                            printf("DEBUG_KB:%u,%u,%u,%u,%u,%u,%u,%u\n",
                                   state.modifiers,
                                   state.num_keys,
                                   state.keys[0], state.keys[1],
                                   state.keys[2], state.keys[3],
                                   state.keys[4], state.keys[5]);
                        }
                    } else {
                        gamepad_state_t state;
                        if (usb_host_get_gamepad_state(&state)) {
                            // Format: "DEBUG:buttons,lx,ly,rx,ry,lt,rt,dx,dy"
                            printf("DEBUG:%u,%d,%d,%d,%d,%u,%u,%d,%d\n",
                                   state.buttons,
                                   state.left_x, state.left_y,
                                   state.right_x, state.right_y,
                                   state.left_trigger, state.right_trigger,
                                   state.dpad_x, state.dpad_y);
                        }
                    }
                }
            } else if (strcmp(cmd_buffer, "DEBUG_INFO") == 0) {
                // Send connected device info
                if (usb_host_device_connected()) {
                    usb_device_info_t info;
                    if (usb_host_get_device_info(&info)) {
                        // Format: "DEBUG_INFO:vid,pid,addr,type"
                        printf("DEBUG_INFO:0x%04X,0x%04X,%u,%u\n",
                               info.vid, info.pid, 
                               info.dev_addr, (unsigned)info.input_type);
                    }
                } else {
                    printf("DEBUG_INFO:NO_DEVICE\n");
                }
            } else if (strcmp(cmd_buffer, "LOG_GET") == 0) {
                // Get and send all logs
                uint16_t len = logging_get_logs(log_output_buffer, sizeof(log_output_buffer));
                if (len > 0) {
                    printf("LOG_START\n");
                    printf("%s", log_output_buffer);
                    printf("LOG_END\n");
                } else {
                    printf("LOG_EMPTY\n");
                }
            } else if (strcmp(cmd_buffer, "LOG_CLEAR") == 0) {
                // Clear all logs
                logging_clear();
                printf("LOG_CLEARED\n");
            } else if (strcmp(cmd_buffer, "LOG_COUNT") == 0) {
                // Get log entry count
                printf("LOG_COUNT:%u\n", logging_get_count());
            } else if (strncmp(cmd_buffer, "LOG_LEVEL ", 10) == 0) {
                // Set log level (LOG_LEVEL 0-3), ensure there's a character after space
                if (cmd_buffer[10] != '\0') {
                    int level = cmd_buffer[10] - '0';
                    if (level >= 0 && level <= 3) {
                        logging_set_level((log_level_t)level);
                        printf("LOG_LEVEL_SET:%d\n", level);
                    } else {
                        printf("LOG_LEVEL_ERROR:invalid\n");
                    }
                } else {
                    printf("LOG_LEVEL_ERROR:invalid\n");
                }
            } else if (strcmp(cmd_buffer, "LOG_STATUS") == 0) {
                // Get logging status
                printf("LOG_STATUS:level=%d,count=%u,overflow=%d\n",
                       logging_get_level(),
                       logging_get_count(),
                       logging_has_overflow() ? 1 : 0);
            }
            
            cmd_pos = 0;
        }
    } else if (cmd_pos < CMD_BUFFER_SIZE - 1) {
        // Only add printable characters
        if (c >= 32 && c < 127) {
            cmd_buffer[cmd_pos++] = (char)c;
        }
    } else {
        // Buffer full, reset to prevent overflow
        cmd_pos = 0;
    }
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
        case STATE_DEBUG_MODE:
            blink_interval = 300;  // Medium-fast blink in debug mode
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
    
    // Initialize logging system
    logging_init();
    LOG_INFO("Joystick Converter starting...");
    
    // Load configuration from flash
    if (!config_load()) {
        LOG_WARN("Failed to load config, using defaults");
        config_set_defaults();
    } else {
        LOG_INFO("Configuration loaded successfully");
    }
    
    // Initialize USB host for gamepad input
    if (!usb_host_init()) {
        LOG_ERROR("Failed to initialize USB host");
        current_state = STATE_ERROR;
    } else {
        LOG_INFO("USB host initialized");
        current_state = STATE_WAITING_FOR_INPUT;
    }
    
    // Initialize USB device for output
    if (!usb_device_init()) {
        LOG_ERROR("Failed to initialize USB device");
        current_state = STATE_ERROR;
    } else {
        LOG_INFO("USB device initialized");
    }
    
    // Initialize remapping engine
    remapping_init();
    LOG_INFO("Remapping engine initialized");
    
    // Initialize macro system
    macro_init();
    LOG_INFO("Macro system initialized");
    
    LOG_INFO("Initialization complete. Waiting for gamepad...");
    
    // Main loop
    while (1) {
        // Update LED status
        status_led_update();
        
        // Process USB host (gamepad input)
        usb_host_task();
        
        // Process USB device (output)
        usb_device_task();
        
        // Process macro execution (skip in debug mode)
        if (!debug_mode_enabled) {
            macro_task();
        }
        
        // Handle serial commands for debug mode
        handle_serial_commands();
        
        // Update application state
        if (current_state == STATE_WAITING_FOR_INPUT) {
            if (usb_host_device_connected()) {
                if (debug_mode_enabled) {
                    current_state = STATE_DEBUG_MODE;
                } else {
                    current_state = STATE_ACTIVE;
                }
                LOG_INFO("Gamepad connected");
            }
        } else if (current_state == STATE_ACTIVE || current_state == STATE_DEBUG_MODE) {
            if (!usb_host_device_connected()) {
                current_state = STATE_WAITING_FOR_INPUT;
                LOG_INFO("Gamepad disconnected");
            }
        }
        
        // Check for config mode entry (could be via special button combo)
        if (usb_device_config_mode_requested()) {
            current_state = STATE_CONFIG_MODE;
            LOG_INFO("Entering configuration mode");
        }
        
        // Small delay to prevent busy waiting and reduce power consumption
        sleep_ms(1);
    }
    
    return 0;
}
