# API Documentation

This document describes the internal APIs and data structures used in the joystick converter firmware.

## USB Host API

### Functions

#### `bool usb_host_init(void)`
Initialize USB host functionality.

**Returns**: `true` on success, `false` on failure

#### `void usb_host_task(void)`
Main USB host processing task. Must be called regularly in the main loop.

#### `bool usb_host_device_connected(void)`
Check if a gamepad is currently connected.

**Returns**: `true` if device is connected, `false` otherwise

#### `bool usb_host_get_gamepad_state(gamepad_state_t *state)`
Get the current gamepad state.

**Parameters**:
- `state`: Pointer to gamepad_state_t structure to fill

**Returns**: `true` if state is valid, `false` otherwise

### Data Structures

#### `gamepad_state_t`
```c
typedef struct {
    uint16_t buttons;       // Button state bitmap
    int16_t left_x;         // Left stick X (-32768 to 32767)
    int16_t left_y;         // Left stick Y
    int16_t right_x;        // Right stick X
    int16_t right_y;        // Right stick Y
    uint8_t left_trigger;   // Left trigger (0-255)
    uint8_t right_trigger;  // Right trigger (0-255)
    int8_t dpad_x;          // D-pad X (-1, 0, 1)
    int8_t dpad_y;          // D-pad Y (-1, 0, 1)
} gamepad_state_t;
```

### Button Definitions
```c
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
```

## USB Device API

### Functions

#### `bool usb_device_init(void)`
Initialize USB device functionality.

**Returns**: `true` on success, `false` on failure

#### `void usb_device_task(void)`
Main USB device processing task. Must be called regularly in the main loop.

#### `void usb_device_send_gamepad(uint16_t buttons, int16_t *axes, uint8_t num_axes)`
Send a gamepad HID report.

**Parameters**:
- `buttons`: Button state bitmap
- `axes`: Array of axis values
- `num_axes`: Number of axes in the array

#### `void usb_device_send_keyboard(uint8_t modifiers, uint8_t *keycodes, uint8_t num_keys)`
Send a keyboard HID report.

**Parameters**:
- `modifiers`: Modifier key bitmask (Ctrl, Alt, Shift, etc.)
- `keycodes`: Array of key codes (up to 6 keys)
- `num_keys`: Number of keys pressed

#### `void usb_device_send_mouse(uint8_t buttons, int8_t x, int8_t y, int8_t wheel)`
Send a mouse HID report.

**Parameters**:
- `buttons`: Mouse button state
- `x`: X movement (-127 to 127)
- `y`: Y movement (-127 to 127)
- `wheel`: Wheel movement (-127 to 127)

#### `void usb_device_set_output_type(output_type_t type)`
Set the output device type.

**Parameters**:
- `type`: Output type (OUTPUT_TYPE_GAMEPAD, OUTPUT_TYPE_KEYBOARD, OUTPUT_TYPE_MOUSE, OUTPUT_TYPE_COMBO)

#### `output_type_t usb_device_get_output_type(void)`
Get the current output device type.

**Returns**: Current output type

### Data Types

#### `output_type_t`
```c
typedef enum {
    OUTPUT_TYPE_GAMEPAD,    // Gamepad output
    OUTPUT_TYPE_KEYBOARD,   // Keyboard output
    OUTPUT_TYPE_MOUSE,      // Mouse output
    OUTPUT_TYPE_COMBO       // Combined keyboard + mouse
} output_type_t;
```

## Configuration API

### Functions

#### `bool config_load(void)`
Load configuration from flash memory.

**Returns**: `true` on success, `false` on failure

#### `bool config_save(void)`
Save current configuration to flash memory.

**Returns**: `true` on success, `false` on failure

#### `void config_set_defaults(void)`
Reset configuration to default values.

#### `config_t* config_get(void)`
Get pointer to current configuration.

**Returns**: Pointer to configuration structure

#### `bool config_add_mapping(uint16_t source_button, mapping_type_t type, uint16_t target_value, uint8_t macro_id)`
Add a button mapping.

**Parameters**:
- `source_button`: Source button bit
- `type`: Mapping type
- `target_value`: Target button/key code
- `macro_id`: Macro ID (if type is MAPPING_TYPE_MACRO)

**Returns**: `true` on success, `false` if mapping table is full

#### `void config_clear_mappings(void)`
Clear all button mappings.

#### `button_mapping_t* config_find_mapping(uint16_t source_button)`
Find button mapping for a specific button.

**Parameters**:
- `source_button`: Button to look up

**Returns**: Pointer to mapping, or NULL if not found

### Data Structures

#### `config_t`
```c
typedef struct {
    uint32_t magic;                    // Magic number for validation
    uint32_t version;                  // Config version
    output_type_t output_type;         // Output device type
    uint8_t num_mappings;              // Number of button mappings
    button_mapping_t mappings[MAX_BUTTON_MAPPINGS];
} config_t;
```

#### `button_mapping_t`
```c
typedef struct {
    uint16_t source_button;   // Source button bit
    mapping_type_t type;      // Mapping type
    uint16_t target_value;    // Target button/key code
    uint8_t macro_id;         // Macro ID if type is MAPPING_TYPE_MACRO
} button_mapping_t;
```

#### `mapping_type_t`
```c
typedef enum {
    MAPPING_TYPE_NONE,
    MAPPING_TYPE_BUTTON,       // Map to another button
    MAPPING_TYPE_KEY,          // Map to keyboard key
    MAPPING_TYPE_MOUSE_BUTTON, // Map to mouse button
    MAPPING_TYPE_MACRO         // Execute macro
} mapping_type_t;
```

## Remapping API

### Functions

#### `void remapping_init(void)`
Initialize the remapping engine.

#### `void remapping_process_input(const gamepad_state_t *input)`
Process gamepad input and generate output according to current mappings.

**Parameters**:
- `input`: Pointer to gamepad state

#### `bool remapping_is_button_pressed(uint16_t buttons, uint16_t button)`
Check if a specific button is pressed.

**Parameters**:
- `buttons`: Current button state
- `button`: Button to check

**Returns**: `true` if button is pressed

#### `bool remapping_button_pressed(uint16_t button)`
Check if a button was just pressed (rising edge).

**Parameters**:
- `button`: Button to check

**Returns**: `true` if button was just pressed

#### `bool remapping_button_released(uint16_t button)`
Check if a button was just released (falling edge).

**Parameters**:
- `button`: Button to check

**Returns**: `true` if button was just released

## Macro API

### Functions

#### `void macro_init(void)`
Initialize the macro system.

#### `bool macro_execute(uint8_t macro_id)`
Execute a macro.

**Parameters**:
- `macro_id`: Macro ID to execute

**Returns**: `true` on success, `false` if macro not found or already executing

#### `void macro_task(void)`
Macro execution task. Must be called regularly in the main loop.

#### `bool macro_add(const macro_t *macro)`
Add or update a macro.

**Parameters**:
- `macro`: Pointer to macro definition

**Returns**: `true` on success, `false` if macro table is full

#### `bool macro_remove(uint8_t macro_id)`
Remove a macro.

**Parameters**:
- `macro_id`: Macro ID to remove

**Returns**: `true` on success, `false` if not found

#### `macro_t* macro_get(uint8_t macro_id)`
Get a macro by ID.

**Parameters**:
- `macro_id`: Macro ID

**Returns**: Pointer to macro, or NULL if not found

#### `void macro_clear_all(void)`
Clear all macros.

### Data Structures

#### `macro_t`
```c
typedef struct {
    uint8_t id;                          // Macro ID
    uint8_t num_steps;                   // Number of steps
    macro_step_t steps[MAX_MACRO_STEPS]; // Macro steps
} macro_t;
```

#### `macro_step_t`
```c
typedef struct {
    macro_action_type_t action;  // Action type
    uint16_t param1;             // Key code, mouse button, or delay ms
    int16_t param2;              // Mouse X or 0
    int16_t param3;              // Mouse Y or 0
} macro_step_t;
```

#### `macro_action_type_t`
```c
typedef enum {
    MACRO_ACTION_KEY_PRESS,
    MACRO_ACTION_KEY_RELEASE,
    MACRO_ACTION_MOUSE_MOVE,
    MACRO_ACTION_MOUSE_BUTTON_PRESS,
    MACRO_ACTION_MOUSE_BUTTON_RELEASE,
    MACRO_ACTION_DELAY
} macro_action_type_t;
```

## Constants

### Limits
```c
#define MAX_BUTTON_MAPPINGS 32   // Maximum button mappings
#define MAX_MACROS 16            // Maximum number of macros
#define MAX_MACRO_STEPS 128      // Maximum steps per macro
```

### Flash Configuration
```c
#define CONFIG_MAGIC 0x4A435446  // "JCTF" - validation magic
#define CONFIG_VERSION 1         // Current config version
```

## Usage Example

```c
// Initialize all systems
usb_host_init();
usb_device_init();
config_load();
remapping_init();
macro_init();

// Main loop
while (1) {
    // Process USB
    usb_host_task();
    usb_device_task();
    
    // Process macros
    macro_task();
    
    // Get gamepad state
    gamepad_state_t state;
    if (usb_host_get_gamepad_state(&state)) {
        // Process through remapping engine
        remapping_process_input(&state);
    }
}
```

## Communication Protocol (Configuration Software)

The configuration software communicates with the device via serial port using a simple text-based protocol:

### Commands

- `GET_CONFIG` - Request current configuration
- `SET_CONFIG <data>` - Upload new configuration
- `GET_VERSION` - Get firmware version
- `SAVE_CONFIG` - Save config to flash
- `RESET_CONFIG` - Reset to defaults

### Response Format

All responses are JSON formatted:
```json
{
    "status": "ok",
    "data": { ... }
}
```

Error response:
```json
{
    "status": "error",
    "message": "Error description"
}
```
