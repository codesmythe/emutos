#include "config.h"

#define ENABLE_KDEBUG

#ifdef CONF_WITH_VT82C42

#include "emutos.h"
#include "ikbd.h"
#include "vectors.h"

#include <stdio.h>
#include <ctype.h>

#include "vt82c42.h"

#define PS2_BASE 0x00F7F200

#define VT82_DATA		  		0x00
#define VT82_CMD 				0x02
#define VT82_STATUS				0x02

void write_vt(UBYTE reg, UBYTE val);
UBYTE read_vt(UBYTE reg);

void vt8242_delay(unsigned long d);
void vt82_wait_status(UBYTE flag);
void vt82_wait_clear(UBYTE flag);

void write_vt(UBYTE reg, UBYTE val) {
    volatile UBYTE *vt_base = (volatile UBYTE *) PS2_BASE;
    vt_base[reg] = val;
}

UBYTE read_vt(UBYTE reg) {
    volatile UBYTE *vt_base = (volatile UBYTE *) PS2_BASE;
    return vt_base[reg];
}

static UBYTE g_key_mode = 0;

static const UBYTE st_make_code_map[] = {
    0 , 67 /*F9*/, 0 , 63 /*F5*/, 61 /*F3*/, 59 /*F1*/, 60 /*F2*/, 97 /*F12*/,
	0 , 68 /*F10*/, 66 /*F8*/, 64 /*F6*/, 62 /*F4*/, 15 /*Tab*/, 41 /*Backtick/Tilde (`~)*/, 0 , 
    0 , 56 /*Left Alt*/, 42 /*Left Shift*/, 0 , 29 /*Left Ctrl*/, 16 /*Q*/, 2 /*1*/, 0 ,
    0 , 0 , 44 /*Z*/, 31 /*S*/, 30 /*A*/, 17 /*W*/, 3 /*2*/, 0 ,
	0 , 46 /*C*/, 45 /*X*/, 32 /*D*/, 18 /*E*/, 5 /*4*/, 4 /*3*/, 0 ,
    0 , 57 /*Space*/, 47 /*V*/, 33 /*F*/, 20 /*T*/, 19 /*R*/, 6 /*5*/, 0 ,
    0 , 49 /*N*/, 48 /*B*/, 35 /*H*/, 34 /*G*/, 21 /*Y*/, 7 /*6*/, 0 ,
    0 , 0 , 50 /*M*/, 36 /*J*/, 22 /*U*/, 8 /*7*/, 9 /*8*/, 0 ,
    0 , 51 /*Comma (,<)*/, 37 /*K*/, 23 /*I*/, 24 /*O*/, 11 /*0*/, 10 /*9*/, 0 ,
    0 , 52 /*Period (.>)*/, 53 /*Slash (/?)*/, 38 /*L*/, 39 /*Semicolon (;:)*/, 25 /*P*/, 12 /*Minus (-_)*/, 0 ,
    0 , 0 , 40 /*Apostrophe ('")*/, 0 , 26 /*Left Bracket ([{)*/, 13 /*Equals (=+)*/, 0 , 0 , 58 /*CapsLock*/,
    54 /*Right Shift*/, 28 /*Enter*/, 27 /*Right Bracket (]})*/, 0 , 43 /*Backslash (\|)*/, 0 , 
    0 , 0 , 96 /*UK \| between left shift and Z*/, 0 , 0 , 0 , 0 , 14 /*Backspace*/, 0 , 0 , 109 /*Keypad 1/End*/,
    0 , 106 /*Keypad 4/Left*/, 103 /*Keypad 7/Home*/, 0 , 0 , 0 , 112 /*Keypad 0/Ins*/, 113 /*Keypad ./Del*/,
    110 /*Keypad 2/Down*/, 107 /*Keypad 5*/, 108 /*Keypad 6/Right*/, 104 /*Keypad 8/Up*/, 1 /*Escape*/,
    -1 /*NumLock*/, 98 /*F11*/, 78 /*Keypad +*/, 111 /*Keypad 3/PgDn*/, 74 /*Keypad -*/, 102 /*Keypad **/,
    105 /*Keypad 9/PgUp*/, -1 /*ScrollLock*/, 0 , 0 , 0 , 0 , 65 /*F7*/
};

static const UBYTE st_extended_make_code_map[] = {
    0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 
    56 /*Right Alt*/, 0 , 0 , 29 /*Right Ctrl*/, 0 , 0 , 0 , 0 , 0 , 0 ,
    0 , 0 , 0 , 0 , -1 /*Left GUI (Windows)*/, 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
    -1 /*Right GUI (Windows)*/, 0 , 0 , 0 , 0 , 0 , 0 , 0 , -1 /*Menu*/,
    0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
    0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 101 /*Keypad /*/, 0 , 0 , 0 ,
    0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 114 /*Keypad Enter*/,
    0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 79 /*End*/,
    0 , 75 /*Left Arrow*/, 71 /*Home*/, 0 , 0 , 0 , 82 /*Insert*/, 83 /*Delete*/,
    80 /*Down Arrow*/, 0 , 77 /*Right Arrow*/, 72 /*Up Arrow*/, 0 , 0 , 0 , 0 ,
    81 /*Page Down*/, 0 , 0 , 73 /*Page Up*/, 0 , 0
};

#define PS2_TIMEOUT 1000
#define WAIT_TIMEOUT 10000

//	keyboard interrupt handler
void __attribute__((interrupt)) vt8242_interrupt_handler(void)
{
    UBYTE register sc;
    sc = read_vt(VT82_DATA);
    vt8242_process_scancode(sc);
}

void vt8242_delay(unsigned long d)
{
    volatile unsigned long wait = d;
    while (wait--)
    {}
}

void vt82_wait_status(UBYTE flag)
{
    volatile ULONG timeout = WAIT_TIMEOUT;
    while (read_vt(VT82_STATUS) & flag)
    {
        timeout--;
        if (timeout == 0)
        {
            break;
        }
    }
}

 void vt82_wait_clear(UBYTE flag)
{
    volatile ULONG timeout = WAIT_TIMEOUT;
    while (!(read_vt(VT82_STATUS) & flag))
    {
        timeout--;
        if (timeout == 0)
            break;
    }
}

void vt8242_set_leds(UBYTE leds)
{
	vt82_wait_status(STATUS_IBF);
	write_vt(VT82_DATA, KBD_CMD_LED);
    vt82_wait_status(STATUS_OBF);
	write_vt(VT82_DATA, leds);
}

UBYTE vt8242_send_command(UBYTE cmd, UBYTE wait_response)
{
    vt82_wait_status(STATUS_IBF);
    write_vt(VT82_CMD, cmd);

    if (!wait_response)
        return 0;

    vt82_wait_clear(STATUS_OBF);
    return read_vt(VT82_DATA);
}

UBYTE vt8242_get_config_byte(void)
{
	vt82_wait_status(STATUS_IBF);
	write_vt(VT82_CMD, CMD_GET_BYTE);
    vt82_wait_clear(STATUS_OBF);
    return read_vt(VT82_DATA);
}

void vt8242_set_config_byte(UBYTE cfg_byte)
{
	vt82_wait_status(STATUS_IBF);
	write_vt(VT82_CMD, CMD_SET_BYTE);
    vt82_wait_status(STATUS_IBF);
	write_vt(VT82_DATA, cfg_byte);
}

void vt8242_disable_for_init(void)
{
	UBYTE cfg = vt8242_get_config_byte();
	vt8242_set_config_byte(cfg & ~(CMD_BYTE_KBD_INT | CMD_BYTE_AUX_INT | CMD_BYTE_TRANS));
}

UBYTE keyboard_send_command(UBYTE cmd)
{
    UBYTE res;

    vt82_wait_status(STATUS_IBF);
    write_vt(VT82_DATA, cmd);

    vt8242_delay(20);

    vt82_wait_clear(STATUS_OBF);
    res = read_vt(VT82_DATA);
	return res;
}

void vt8242_enable_port1_interrupt(void)
{
	UBYTE cfg = vt8242_get_config_byte();
	vt8242_set_config_byte(cfg | CMD_BYTE_KBD_INT);
}

void vt8242_enable_port2_interrupt(void)
{
	UBYTE cfg = vt8242_get_config_byte();
	vt8242_set_config_byte(cfg | CMD_BYTE_AUX_INT);
}

void vt8242_disable_port1_interrupt(void)
{
	UBYTE cfg = vt8242_get_config_byte();
	vt8242_set_config_byte(cfg & ~CMD_BYTE_KBD_INT);
}

void vt8242_disable_port2_interrupt(void)
{
	UBYTE cfg = vt8242_get_config_byte();
	vt8242_set_config_byte(cfg & ~CMD_BYTE_AUX_INT);
}

UBYTE vt8242_init(void)
{
    // NOTE: 
    // Most of this init code is disabled for now.
    // Seems to generate errors, could be based it's configured by DdraigDOS
    // before running EmuTOS.

    volatile PFVOID *vector_addr;

    // KDEBUG(("vt8242_init()\n"));

    // vt8242_set_config_byte(0);
	// vt8242_send_command(CMD_KBD_OFF, 0); // disable first port
	// vt8242_send_command(CMD_AUX_OFF, 0); // disable 2nd port

    // KDEBUG(("vt8242: reset controller\n"));
	// vt8242_disable_for_init();
    // KDEBUG(("vt8242: flush buffer\n"));
	// vt8242_flush();			 // flush buffer
    // KDEBUG(("vt8242: self test\n"));

    // KDEBUG(("vt8242: send self test command\n"));
	// if (vt8242_send_command(CMD_DIAG, 1) != KBD_STATUS_DIAG_OK)
    // {
	// 	KDEBUG(("ERROR: PS/2 keyboard controller failed.\n"));
    //     return 0;
	// }

    // KDEBUG(("vt8242: enable ports if present\n"));
	// vt8242_send_command(CMD_AUX_ON, 0); // enable 2nd port
	// if (!(vt8242_get_config_byte() & CMD_BYTE_AUX_OFF))
    // {
	// 	KDEBUG(("PS/2 controller has 2 channels.\n"));
	// 	vt8242_send_command(CMD_AUX_OFF, 0);
	// }

    // KDEBUG(("vt8242: test first PS/2 port\n"));
	// if (vt8242_send_command(CMD_KBD_TEST, 1) != 0x00)
    // {
	// 	KDEBUG(("ERROR: Check keyboard!\n"));
	// }
	// // enable first PS/2 port
    // KDEBUG(("vt8242: enable first PS/2 port\n"));
	// vt8242_send_command(CMD_KBD_ON, 0);
    // vt8242_flush();
    // KDEBUG (("vt8242: reset keyboard\n"));
    // int retries = 10;
    // UBYTE init_response;

    // while (retries--)
    // {
    //     init_response = keyboard_send_command(KBD_CMD_RST);
    //     if (init_response == KBD_STATUS_RESEND)
    //     {
    //         KDEBUG(("ERROR: Keyboard reset resending\n"));
    //         continue;
    //     }
    //     else if ((init_response != KBD_STATUS_RST_OK) && (init_response != KBD_STATUS_ACK))
    //     {
    //         KDEBUG(("ERROR: Keyboard reset error, resp = %02x\n", init_response));
    //     }
    // }

    // vt82_wait_clear(STATUS_OBF);

	// init_response = read_vt(VT82_DATA);
    // if ((init_response != KBD_STATUS_RST_OK) && (init_response != KBD_STATUS_ACK))
    // {
	// 	KDEBUG(("ERROR: Keyboard self test failed, resp = %02X\n", init_response));
	// }

    KDEBUG(("vt8242: install keyboard interrupt handler\n"));
    vector_addr = &VEC_LEVEL1 + (CONF_VT82C42_AUTOVECTOR - 1);
    *vector_addr = (PFVOID)vt8242_interrupt_handler;
    //(*((long *)0x74) = (long)vt8242_keyboard_interrupt);

    //vt8242_enable_port1_interrupt();
    return 1;
}

UBYTE vt8242_flush(void)
{
    int timeout = PS2_TIMEOUT;
    // Clear the Output Buffer
    while (timeout)
	{
        if ((read_vt(VT82_STATUS) & STATUS_OBF))
            read_vt(VT82_DATA);
        else
            break;
        timeout--;
    }

	if (timeout == 0)
	{
        KDEBUG(("keyboard output buffer flush timed out - Controller Failure?\n"));
		return -1;
	}
	return 0;
}

void vt8242_process_scancode(register UBYTE sc)
{
    static UBYTE key_break = 0;
    static UBYTE key_extended = 0;
    static UBYTE key_remaining = 0;

	UBYTE register chr;

    if (key_remaining > 0)
    {
        key_remaining--;
        return;
    }
    else if (sc == SCAN_CODE_BREAK)
        key_break = 1;
    else if (sc == SCAN_CODE_MODIFIER)
        key_extended  = 1;
    else if (sc == SCAN_CODE_PSBRK)
    {
        // Pause/Break keys extended sequence, ignore for now
        key_remaining = 7;
    }
    else
    {
        if (sc == SCAN_CODE_CAPLOCK)
        {
            g_key_mode ^= STATUS_CAPS_LOCK;
            vt8242_set_leds(g_key_mode);
        }
        else if (sc == SCAN_CODE_NUMLOCK)
        {
            g_key_mode ^= STATUS_NUM_LOCK;
            vt8242_set_leds(g_key_mode);
        }
        else if (sc == SCAN_CODE_SCRLOCK)
        {
            g_key_mode ^= STATUS_SCROLL_LOCK;
            vt8242_set_leds(g_key_mode);
        }

        sc &= 0x7f;

        if (key_extended)
            chr = st_extended_make_code_map[sc];
        else
            chr = st_make_code_map[sc];

        if (key_break)
            chr |= 0x80; // set break code

        
        KDEBUG(("call_ikbdraw 0x%02x\n", chr));
        call_ikbdraw(chr);
        key_extended = 0;
        key_break = 0;        
	}
}

#endif
