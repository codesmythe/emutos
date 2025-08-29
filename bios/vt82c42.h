#ifndef _VT82C42__H_
#define _VT82C42__H_

#include "portab.h"

// 8542 Commands (Sent to the Command Port)
#define CMD_SET_BYTE		    0x60	// Set the command byte
#define CMD_GET_BYTE		    0x20	// Get the command byte
#define CMD_KBD_OBUFF		    0xD2	// Write to Keyboard Output Buffer
#define CMD_AUX_OBUFF		    0xD3	// Write to Mouse Output Buffer
#define CMD_AUX_WRITE		    0xD4	// Write to Mouse Port
#define CMD_AUX_OFF			    0xA7	// Disable Mouse Port
#define CMD_AUX_ON			    0xA8	// Re-Enable Mouse Port
#define CMD_AUX_TEST		    0xA9	// Test for the presence of a Mouse
#define CMD_DIAG			    0xAA	// Start Diagnostics
#define CMD_KBD_TEST		    0xAB	// Test for presence of a keyboard
#define CMD_KBD_OFF			    0xAD	// Disable Keyboard Port
#define CMD_KBD_ON			    0xAE	// Re-Enable Keyboard Port

// Command byte set by KBD_CMD_SET_BYTE and retrieved by KBD_CMD_GET_BYTE
#define CMD_BYTE_TRANS		    0x40
#define CMD_BYTE_AUX_OFF	    0x20	// 1 = mouse port disabled, 0 = enabled
#define CMD_BYTE_KBD_OFF	    0x10	// 1 = keyboard port disabled, 0 = enabled
#define CMD_BYTE_OVER		    0x08	// 1 = override keyboard lock
#define CMD_BYTE_RES		    0x04	// reserved
#define CMD_BYTE_AUX_INT	    0x02	// 1 = enable mouse interrupt
#define CMD_BYTE_KBD_INT	    0x01	// 1 = enable keyboard interrupt

// Keyboard Commands (Sent to the Data Port)
#define KBD_CMD_LED			    0xED	// Set Keyboard LEDS with next byte
#define KBD_CMD_ECHO		    0xEE	// Echo - we get 0xFA, 0xEE back
#define KBD_CMD_MODE		    0xF0	// set scan code mode with next byte
#define KBD_CMD_ID			    0xF2	// get keyboard/mouse ID
#define KBD_CMD_RPT			    0xF3	// Set Repeat Rate and Delay with Second Byte
#define KBD_CMD_ON			    0xF4	// Enable keyboard
#define KBD_CMD_OFF			    0xF5	// Disables Scanning and Resets to Defaults
#define KBD_CMD_DEF			    0xF6	// Reverts keyboard to default settings
#define KBD_CMD_RST			    0xFF	// Reset - we should get 0xFA, 0xAA back

// Set LED second bit defines
#define KBD_CMD_LED_SCROLL	    0x01	// Set SCROLL LOCK LED on
#define KBD_CMD_LED_NUM		    0x02	// Set NUM LOCK LED on
#define KBD_CMD_LED_CAPS	    0x04	// Set CAPS LOCK LED on

// Set Mode second byte defines
#define KBD_CMD_MODE_STATE	    0x00	// get current scan code mode
#define KBD_CMD_MODE_SCAN1	    0x01	// set mode to scan code 1 - PC/XT
#define KBD_CMD_MODE_SCAN2	    0x02	// set mode to scan code 2
#define KBD_CMD_MODE_SCAN3	    0x03	// set mode to scan code 3

// Keyboard/Mouse ID Codes
#define KBD_CMD_ID_1ST		    0xAB	// first byte is 0xAB, second is actual ID
#define KBD_CMD_ID_KBD		    0x83	// Keyboard
#define KBD_CMD_ID_MOUSE	    0x00	// Mouse

// Keyboard Data Return Defines
#define KBD_STATUS_OVER		    0x00	// Buffer Overrun
#define KBD_STATUS_DIAG_OK	    0x55	// Internal Self Test OK
#define KBD_STATUS_RST_OK		0xAA	// Reset Complete
#define KBD_STATUS_ECHO		    0xEE	// Echo Command Return
#define KBD_STATUS_BRK		    0xF0	// Prefix for Break Key Code
#define KBD_STATUS_ACK		    0xFA	// Received after all commands
#define KBD_STATUS_DIAG_FAIL	0xFD	// Internal Self Test Failed
#define KBD_STATUS_RESEND		0xFE	// Resend Last Command

// Status Register Bit Defines
#define STATUS_OBF			    0x01	// 1 = output buffer (controller to cpu) has data
#define STATUS_IBF			    0x02	// 1 = input buffer (cpu to controller) has data
#define STATUS_SYS			    0x04	// system flag - unused
#define STATUS_CMD			    0x08	// 1 = command in input buffer, 0 = data
#define STATUS_INH			    0x10	// 1 = Inhibit - unused
#define STATUS_TX				0x20	// 1 = Transmit Timeout has occured
#define STATUS_RX				0x40	// 1 = Receive Timeout has occured
#define STATUS_PERR			    0x80	// 1 = Parity Error from Keyboard

#define STATE_BREAK			    0x0001
#define STATE_MODIFIER		    0x0002
#define STATE_SHIFT_L		    0x0004
#define STATE_SHIFT_R		    0x0008
#define STATE_ALT_L		        0x0010
#define STATE_ALT_R		        0x0020
#define STATE_CTRL_L            0x0040
#define STATE_CTRL_R            0x0080
#define STATE_WIN_L             0x0100
#define STATE_WIN_R             0x0200
#define STATE_MENUS             0x0400

#define STATUS_SCROLL_LOCK	    0x01
#define STATUS_NUM_LOCK		    0x02
#define STATUS_CAPS_LOCK		0x04

#define SCAN_CODE_BREAK         0xF0
#define SCAN_CODE_MODIFIER      0xE0
#define SCAN_CODE_PSBRK         0xE1
#define SCAN_CODE_ALT           0x11
#define SCAN_CODE_SHIFTL        0x12
#define SCAN_CODE_SHIFTR        0x59
#define SCAN_CODE_CTRL          0x14
#define SCAN_CODE_WINL          0x1F
#define SCAN_CODE_WINR          0x27
#define SCAN_CODE_MENUS         0x2F
#define SCAN_CODE_CAPLOCK       0x58
#define SCAN_CODE_NUMLOCK       0x77
#define SCAN_CODE_SCRLOCK       0x7E

#define SCAN_CODE_SLASHF        0x4A
#define SCAN_CODE_ENTER         0x5A
#define SCAN_CODE_END           0x69
#define SCAN_CODE_ARROW_L       0x6B
#define SCAN_CODE_HOME          0x6C
#define SCAN_CODE_INSERT        0x70
#define SCAN_CODE_DELETE        0x71
#define SCAN_CODE_ARROW_D       0x72
#define SCAN_CODE_ARROW_R       0x74
#define SCAN_CODE_ARROW_U       0x75
#define SCAN_CODE_PAGEDOWN      0x7A
#define SCAN_CODE_PAGEUP        0x7D


void vt8242_interrupt_handler(void);

UBYTE vt8242_init(void);
UBYTE vt8242_flush(void);
void vt8242_disable_for_init(void);

void vt8242_set_config_byte(UBYTE cfg_byte);
UBYTE vt8242_get_config_byte(void);
void vt8242_process_scancode(UBYTE sc);

void vt8242_enable_port1_interrupt(void);
void vt8242_enable_port2_interrupt(void);
void vt8242_disable_port1_interrupt(void);
void vt8242_disable_port2_interrupt(void);

void vt8242_set_leds(UBYTE leds);
UBYTE vt8242_send_command(UBYTE cmd, UBYTE wait_response);
UBYTE keyboard_send_command(UBYTE cmd);

#endif
