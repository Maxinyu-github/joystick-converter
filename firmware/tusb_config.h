/**
 * TinyUSB Configuration
 * 
 * This file configures TinyUSB for the Joystick Converter project.
 * The project uses:
 * - USB Host for reading gamepad/keyboard input
 * - USB Device for outputting HID reports (gamepad/keyboard/mouse)
 * - CDC for serial communication with PC configuration tool
 */

#ifndef TUSB_CONFIG_H
#define TUSB_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

//--------------------------------------------------------------------
// Common Configuration
//--------------------------------------------------------------------

// CFG_TUSB_MCU is defined by the Pico SDK
#ifndef CFG_TUSB_MCU
#error CFG_TUSB_MCU must be defined
#endif

#ifndef CFG_TUSB_OS
#define CFG_TUSB_OS           OPT_OS_PICO
#endif

// Enable debug output (0 = off, 1 = errors, 2 = warnings, 3 = info)
#ifndef CFG_TUSB_DEBUG
#define CFG_TUSB_DEBUG        0
#endif

// Memory alignment for USB buffers
#ifndef CFG_TUSB_MEM_ALIGN
#define CFG_TUSB_MEM_ALIGN    __attribute__ ((aligned(4)))
#endif

//--------------------------------------------------------------------
// USB Host Configuration
//--------------------------------------------------------------------

// Enable USB Host stack
#define CFG_TUH_ENABLED       1

// RHPort number for host (0 or 1)
#ifndef BOARD_TUH_RHPORT
#define BOARD_TUH_RHPORT      0
#endif

// Maximum operational speed for host
#ifndef BOARD_TUH_MAX_SPEED
#define BOARD_TUH_MAX_SPEED   OPT_MODE_FULL_SPEED
#endif

#define CFG_TUH_MAX_SPEED     BOARD_TUH_MAX_SPEED

// Host memory section and alignment
#ifndef CFG_TUH_MEM_SECTION
#define CFG_TUH_MEM_SECTION
#endif

#ifndef CFG_TUH_MEM_ALIGN
#define CFG_TUH_MEM_ALIGN     __attribute__ ((aligned(4)))
#endif

// Size of buffer to hold descriptors and other data used for enumeration
#define CFG_TUH_ENUMERATION_BUFSIZE   256

// Number of hub devices supported
#define CFG_TUH_HUB           1

// Maximum device support (excluding hub device)
// Hub typically has 4 ports, so support 4 devices + 1 for direct connection
#define CFG_TUH_DEVICE_MAX    (CFG_TUH_HUB ? 4 : 1)

// Host class drivers
#define CFG_TUH_CDC           0  // CDC not needed for host
#define CFG_TUH_HID           4  // Support up to 4 HID devices (gamepads/keyboards)
#define CFG_TUH_MSC           0  // Mass storage not needed
#define CFG_TUH_VENDOR        0  // Vendor class not needed

// HID Host configuration
#define CFG_TUH_HID_EPIN_BUFSIZE    64
#define CFG_TUH_HID_EPOUT_BUFSIZE   64

//--------------------------------------------------------------------
// USB Device Configuration (currently disabled)
//--------------------------------------------------------------------

// Note: USB Device is disabled for now because the RP2040/RP2350 has
// only one USB controller. To use both Host and Device simultaneously,
// PIO-USB would be needed for one of them. For initial TinyUSB integration,
// we focus on USB Host functionality for reading gamepad input.
// 
// The stdio_usb (CDC serial) is handled separately by the Pico SDK
// when pico_enable_stdio_usb is enabled.

#define CFG_TUD_ENABLED       0

#ifdef __cplusplus
}
#endif

#endif /* TUSB_CONFIG_H */
