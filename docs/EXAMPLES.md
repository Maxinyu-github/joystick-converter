# Configuration Examples

This document provides practical examples for configuring the joystick converter.

## Example 1: FPS Game Setup

Configure gamepad for first-person shooter games:

### Button Mappings
- Left Trigger → Right Mouse Click (aim down sights)
- Right Trigger → Left Mouse Click (fire)
- A Button → Space (jump)
- B Button → C key (crouch)
- X Button → R key (reload)
- Y Button → E key (interact)
- Left Bumper → Q key (switch weapon)
- Right Bumper → F key (melee)

### Analog Sticks
- Right Stick → Mouse movement (for looking around)
- Left Stick → WASD keys (for movement)

### Configuration
```
Output Type: Keyboard+Mouse

Mappings:
Button 0x0001 (A) → Keyboard 0x2C (Space)
Button 0x0002 (B) → Keyboard 0x06 (C)
Button 0x0004 (X) → Keyboard 0x15 (R)
Button 0x0008 (Y) → Keyboard 0x08 (E)
Button 0x0010 (LB) → Keyboard 0x14 (Q)
Button 0x0020 (RB) → Keyboard 0x09 (F)
Left Trigger → Mouse Button 2
Right Trigger → Mouse Button 1
```

## Example 2: MOBA/RTS Game Setup

Configure for strategy games with complex macros:

### Macro 1: Quick Shop and Buy
```
Macro ID: 1
Steps:
key_press 0x05    # B key (open shop)
delay 200
key_release
delay 100
mouse_move 50 100  # Click on item
delay 50
mouse_button_press 1
delay 50
mouse_button_release
delay 100
key_press 0x29    # ESC (close shop)
delay 50
key_release
```

### Macro 2: Camera Lock Toggle
```
Macro ID: 2
Steps:
key_press 0x1C    # Y key
delay 50
key_release
```

### Button Mappings
- A Button → Macro 1 (quick buy)
- B Button → Macro 2 (camera lock)
- X Button → 1 key (ability 1)
- Y Button → 2 key (ability 2)
- LB → 3 key (ability 3)
- RB → 4 key (ultimate)

## Example 3: Accessibility Configuration

Setup for users with limited mobility:

### Auto-Fire Macro
```
Macro ID: 3
Steps:
mouse_button_press 1
delay 50
mouse_button_release
delay 50
mouse_button_press 1
delay 50
mouse_button_release
delay 50
mouse_button_press 1
delay 50
mouse_button_release
```

### Hold to Repeat Macro
```
Macro ID: 4
Steps:
key_press 0x04    # A key
delay 100
key_release
delay 50
key_press 0x04
delay 100
key_release
delay 50
key_press 0x04
delay 100
key_release
```

### Button Mappings
- Right Trigger → Macro 3 (auto-fire)
- A Button → Macro 4 (rapid A key)
- Right Stick → Mouse movement with reduced sensitivity

## Example 4: Retro Gaming

Configure for classic game emulators:

### Button Mappings (NES Layout)
- A Button → X key (A on keyboard)
- B Button → Z key (B on keyboard)
- Start → Enter
- Select → Shift
- D-Pad → Arrow Keys (automatic)

### Configuration
```
Output Type: Keyboard

Mappings:
Button 0x0001 → Keyboard 0x1B (X)
Button 0x0002 → Keyboard 0x1D (Z)
Button 0x0080 → Keyboard 0x28 (Enter)
Button 0x0040 → Keyboard 0xE1 (Shift)
```

## Example 5: Fighting Game Setup

Optimized for fighting games with combo macros:

### Combo Macro 1: Quarter Circle Forward + Punch
```
Macro ID: 5
Steps:
# Quarter circle forward motion
key_press 0x51    # Down arrow
delay 50
key_release
delay 30
key_press 0x4F    # Right arrow
delay 50
key_press 0x51    # Down arrow (diagonal)
delay 50
key_release
key_release
delay 30
key_press 0x04    # A key (light punch)
delay 50
key_release
```

### Combo Macro 2: Dragon Punch Motion
```
Macro ID: 6
Steps:
key_press 0x4F    # Right arrow
delay 50
key_release
delay 30
key_press 0x51    # Down arrow
delay 50
key_release
delay 30
key_press 0x4F    # Right arrow
delay 50
key_press 0x04    # A key
delay 50
key_release
key_release
```

### Button Mappings
- A Button → Macro 5 (special move 1)
- B Button → Macro 6 (special move 2)
- X Button → J key (light punch)
- Y Button → I key (heavy punch)
- LB → K key (light kick)
- RB → L key (heavy kick)

## Example 6: Productivity Setup

Use gamepad for desktop navigation:

### Button Mappings
- A Button → Enter
- B Button → ESC
- X Button → Alt+Tab (switch windows)
- Y Button → Windows key (start menu)
- LB → Ctrl+Z (undo)
- RB → Ctrl+Y (redo)
- Start → Ctrl+C (copy)
- Select → Ctrl+V (paste)

### Analog Sticks
- Left Stick → Arrow keys (navigation)
- Right Stick → Mouse movement

### Macro: Screenshot
```
Macro ID: 7
Steps:
key_press 0xE3    # GUI/Windows key
key_press 0x2C    # Print Screen
delay 50
key_release
key_release
```

## Tips for Creating Macros

1. **Timing is Important**: Adjust delay values based on game responsiveness
2. **Test Incrementally**: Test each step of your macro individually
3. **Keep it Simple**: Complex macros can be unreliable
4. **Document Your Macros**: Add comments to remember what each does
5. **Backup Configurations**: Save your working configs before making changes

## Common Key Codes

### Letters
- A: 0x04, B: 0x05, C: 0x06, D: 0x07
- E: 0x08, F: 0x09, G: 0x0A, H: 0x0B
- And so on...

### Numbers
- 1: 0x1E, 2: 0x1F, 3: 0x20, 4: 0x21
- 5: 0x22, 6: 0x23, 7: 0x24, 8: 0x25
- 9: 0x26, 0: 0x27

### Special Keys
- Enter: 0x28
- ESC: 0x29
- Backspace: 0x2A
- Tab: 0x2B
- Space: 0x2C
- Ctrl: 0xE0
- Shift: 0xE1
- Alt: 0xE2
- GUI/Win: 0xE3

### Arrow Keys
- Right: 0x4F
- Left: 0x50
- Down: 0x51
- Up: 0x52

For complete key code reference, see USB HID specification.
