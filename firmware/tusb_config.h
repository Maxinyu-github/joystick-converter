/**
 * TinyUSB Configuration
 * 
 * This file configures TinyUSB for the Joystick Converter project.
 * 
 * The project uses dual USB:
 * - Native USB (Port 0): USB Device for HID output (gamepad/keyboard/mouse) + CDC serial
 * - PIO-USB (Port 1): USB Host for reading gamepad/keyboard input from controller
 * 
 * This configuration is designed for Waveshare RP2350-PiZero which has:
 * - Type-C USB port connected to PC/gaming machine (native USB device)
 * - PIO-USB port connected to controller (USB host via PIO)
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
#ifndef CFG_TUSB_MEM_SECTION
#define CFG_TUSB_MEM_SECTION
#endif

#ifndef CFG_TUSB_MEM_ALIGN
#define CFG_TUSB_MEM_ALIGN    __attribute__ ((aligned(4)))
#endif

//--------------------------------------------------------------------
// USB Device Configuration (Native USB - Port 0)
// Connected to PC/gaming machine
//--------------------------------------------------------------------

// Enable USB Device stack
#define CFG_TUD_ENABLED       1

// RHPort number for device (0 = native USB controller)
#define BOARD_TUD_RHPORT      0

// Device uses Full Speed
#define BOARD_TUD_MAX_SPEED   OPT_MODE_FULL_SPEED
#define CFG_TUD_MAX_SPEED     BOARD_TUD_MAX_SPEED

// Device endpoint 0 size
#define CFG_TUD_ENDPOINT0_SIZE    64

// Device class configuration
#define CFG_TUD_CDC           1  // CDC for serial communication
#define CFG_TUD_HID           1  // HID for gamepad/keyboard/mouse output
#define CFG_TUD_MSC           0
#define CFG_TUD_MIDI          0
#define CFG_TUD_VENDOR        0

// CDC FIFO size
#define CFG_TUD_CDC_RX_BUFSIZE    256
#define CFG_TUD_CDC_TX_BUFSIZE    256
#define CFG_TUD_CDC_EP_BUFSIZE    64

// HID buffer size
#define CFG_TUD_HID_EP_BUFSIZE    64

//--------------------------------------------------------------------
// USB Host Configuration (PIO-USB - Port 1)
// Connected to controller via PIO-USB
//--------------------------------------------------------------------

// Enable USB Host stack
#define CFG_TUH_ENABLED       1

// Enable PIO-USB for host
#define CFG_TUH_RPI_PIO_USB   1

// RHPort number for host (1 = PIO-USB)
#define BOARD_TUH_RHPORT      1

// Maximum operational speed for host (Full Speed for PIO-USB)
#define BOARD_TUH_MAX_SPEED   OPT_MODE_FULL_SPEED
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
// PIO-USB Configuration
// Default GPIO pins for PIO-USB D+/D-
//--------------------------------------------------------------------

// PIO-USB D+ pin (D- is D+ + 1, so GPIO0 for D+, GPIO1 for D-)
// These pins connect to the controller via the PIO-USB Type-C port
#ifndef PIO_USB_DP_PIN_DEFAULT
#define PIO_USB_DP_PIN_DEFAULT    0
#endif

#ifdef __cplusplus
}
#endif

#endif /* TUSB_CONFIG_H */
