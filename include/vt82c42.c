#include <stdio.h>
#include <printf.h>
#include <ctype.h>
#include <stdbool.h>

#include "vt8242.h"
#include "keycodes.h"
#include "drivers/keyboard.h"

uint8_t vt8242_flush();

static uint8_t g_key_mode = 0;
static uint16_t g_key_state = 0;

#define SCANCODE_BUFFER_SIZE 64
#define KEYPRESS_BUFFER_SIZE 64

uint16_t g_scancode_data[SCANCODE_BUFFER_SIZE];
uint16_t g_keypress_data[KEYPRESS_BUFFER_SIZE];

ringbuffer_t g_buf_scancode;
ringbuffer_t g_buf_keypress;

//#define PS2_DEBUG 1
#define PS2_TIMEOUT 1000
#define WAIT_TIMEOUT 10000

//	keyboard interrupt handler
void __attribute__((interrupt)) vt8242_keyboard_interrupt()
{
    uint8_t register sc;
    sc = VT82_REG(VT82_DATA);
    ring_buf_put(&g_buf_scancode, sc);
}

static void vt8242_delay(unsigned long d)
{
    volatile unsigned long wait = d;
    while (wait--)
    {}
}

static inline void vt82_wait_status(uint8_t flag)
{
    uint32_t timeout = WAIT_TIMEOUT;
    while (VT82_REG(VT82_STATUS) & flag)
    {
        timeout--;
        if (timeout == 0)
            break;
    }
}

static inline void vt82_wait_clear(uint8_t flag)
{
    uint32_t timeout = WAIT_TIMEOUT;
    while (!(VT82_REG(VT82_STATUS) & flag))
    {
        timeout--;
        if (timeout == 0)
            break;
    }
}

void vt8242_set_leds(uint8_t leds)
{
	vt82_wait_status(STATUS_IBF);
	VT82_REG(VT82_DATA) = KBD_CMD_LED;
    vt82_wait_status(STATUS_OBF);
	VT82_REG(VT82_DATA) = leds;
}

uint8_t vt8242_send_command(uint8_t cmd, bool wait_response)
{
    vt82_wait_status(STATUS_IBF);
    VT82_REG(VT82_CMD) = cmd;

    if (!wait_response)
        return 0;

    vt82_wait_clear(STATUS_OBF);
    return VT82_REG(VT82_DATA);
}

uint8_t vt8242_get_config_byte(void)
{
	vt82_wait_status(STATUS_IBF);
	VT82_REG(VT82_CMD) = CMD_GET_BYTE;
    vt82_wait_clear(STATUS_OBF);
    return VT82_REG(VT82_DATA);
}

void vt8242_set_config_byte(uint8_t cfg_byte)
{
	vt82_wait_status(STATUS_IBF);
	VT82_REG(VT82_CMD) = CMD_SET_BYTE;
    vt82_wait_status(STATUS_IBF);
	VT82_REG(VT82_DATA) = cfg_byte;
}

void vt8242_disable_for_init(void)
{
	uint8_t cfg = vt8242_get_config_byte();
	vt8242_set_config_byte(cfg & ~(CMD_BYTE_KBD_INT | CMD_BYTE_AUX_INT | CMD_BYTE_TRANS));
}

uint8_t keyboard_send_command(uint8_t cmd)
{
    uint8_t res;

    vt82_wait_status(STATUS_IBF);
    VT82_REG(VT82_DATA) = cmd;

    vt8242_delay(20);

    vt82_wait_clear(STATUS_OBF);
    res = VT82_REG(VT82_DATA);
	return res;
}

void vt8242_enable_port1_interrupt(void)
{
	uint8_t cfg = vt8242_get_config_byte();
	vt8242_set_config_byte(cfg | CMD_BYTE_KBD_INT);
}

void vt8242_enable_port2_interrupt(void)
{
	uint8_t cfg = vt8242_get_config_byte();
	vt8242_set_config_byte(cfg | CMD_BYTE_AUX_INT);
}

void vt8242_disable_port1_interrupt(void)
{
	uint8_t cfg = vt8242_get_config_byte();
	vt8242_set_config_byte(cfg & ~CMD_BYTE_KBD_INT);
}

void vt8242_disable_port2_interrupt(void)
{
	uint8_t cfg = vt8242_get_config_byte();
	vt8242_set_config_byte(cfg & ~CMD_BYTE_AUX_INT);
}

bool vt8242_init()
{
    vt8242_disable_port1_interrupt();
    vt8242_disable_port2_interrupt();

	vt8242_send_command(CMD_KBD_OFF, false); // disable first port
	vt8242_send_command(CMD_AUX_OFF, false); // disable 2nd port

	ISR_VECT_PS2(vt8242_keyboard_interrupt);

	vt8242_disable_for_init();
	vt8242_flush();			 // flush buffer

	if (vt8242_send_command(CMD_DIAG, true) != KBD_STATUS_DIAG_OK)
    {
#ifdef PS2_DEBUG
		printf("ERROR: PS/2 keyboard controller failed.\n");
#endif
        return 0;
	}

	vt8242_send_command(CMD_AUX_ON, false); // enable 2nd port
	if (!(vt8242_get_config_byte() & CMD_BYTE_AUX_OFF))
    {
#ifdef PS2_DEBUG
		printf("PS/2 controller has 2 channels.\n");
#endif
		vt8242_send_command(CMD_AUX_OFF, false);
	}

	if (vt8242_send_command(CMD_KBD_TEST, true) != 0x00)
    {
#ifdef PS2_DEBUG
		printf("ERROR: Check keyboard!\n");
#endif
	}
	// enable first PS/2 port
	vt8242_send_command(CMD_KBD_ON, false);
    vt8242_flush();

    int retries = 30;
    uint8_t init_response;

    while (retries--)
    {
        init_response = keyboard_send_command(KBD_CMD_RST);
        if (init_response == KBD_STATUS_RESEND)
        {
    #ifdef PS2_DEBUG
            printf("ERROR: Keyboard reset resending\n");
    #endif
            continue;
        }
        else if ((init_response != KBD_STATUS_RST_OK) && (init_response != KBD_STATUS_ACK))
        {
    #ifdef PS2_DEBUG
            printf("ERROR: Keyboard reset error, resp = %02x\n", init_response);
    #endif
        }
    }

    vt82_wait_clear(STATUS_OBF);

	init_response = VT82_REG(VT82_DATA);
    if ((init_response != KBD_STATUS_RST_OK) && (init_response != KBD_STATUS_ACK))
    {
#ifdef PS2_DEBUG
		printf("ERROR: Keyboard self test failed, resp = %02X\n", init_response);
		if (init_response == 0xFC)
        {
			printf("Basic assurance test failed (0xFC)\n");
		}
#endif
		return 0;
	}

    return 1;
}

uint8_t vt8242_flush()
{
    int timeout = PS2_TIMEOUT;
    // Clear the Output Buffer
    while (timeout)
	{
        if ((VT82_REG(VT82_STATUS) & STATUS_OBF))
            VT82_REG(VT82_DATA);
        else
            break;
        timeout--;
    }

	if (timeout == 0)
	{
        printf("keyboard output buffer flush timed out - Controller Failure?\n");
		return -1;
	}
	return 0;
}

bool vt8242_has_data()
{
    process_keyboard();
    if (is_ring_buf_empty(&g_buf_keypress))
        return false;

    return true;
}

uint16_t vt8242_get_key()
{
    process_keyboard();
    if (is_ring_buf_empty(&g_buf_keypress))
        return 0;

    uint16_t key = ring_buf_get(&g_buf_keypress);
    return key;
}

void vt8242_process_scancode(register uint8_t sc)
{
	uint8_t register chr;

    if (sc == SCAN_CODE_BREAK)
        g_key_state |= STATE_BREAK;
    else if (sc == SCAN_CODE_MODIFIER)
    {
        g_key_state |= STATE_MODIFIER;
    }
    else
    {
        if (g_key_state & STATE_BREAK)
        {
            if (sc == SCAN_CODE_ALT)
            {
                if (g_key_state & STATE_MODIFIER)
                    g_key_state &= ~STATE_ALT_R;
                else
                    g_key_state &= ~STATE_ALT_L;
            }
            else if (sc == SCAN_CODE_SHIFTL)
            {
                g_key_state &= ~STATE_SHIFT_L;
            }
            else if (sc == SCAN_CODE_SHIFTR)
            {
                g_key_state &= ~STATE_SHIFT_R;
            }
            else if (sc == SCAN_CODE_CTRL)
            {
                if (g_key_state & STATE_MODIFIER)
                    g_key_state &= ~STATE_CTRL_R;
                else
                    g_key_state &= ~STATE_CTRL_L;
            }
            else if (sc == SCAN_CODE_WINL && (g_key_state & STATE_MODIFIER))
            {
                g_key_state &= ~STATE_WIN_L;
            }
            else if (sc == SCAN_CODE_WINR && (g_key_state & STATE_MODIFIER))
            {
                g_key_state &= ~STATE_WIN_R;
            }
            else if (sc == SCAN_CODE_MENUS && (g_key_state & STATE_MODIFIER))
            {
                g_key_state &= ~STATE_MENUS;
            }

            g_key_state &= ~(STATE_BREAK | STATE_MODIFIER);
            return;
        }

        if (sc == SCAN_CODE_ALT)
        {
            if (g_key_state & STATE_MODIFIER)
                g_key_state |= STATE_ALT_R;
            else
                g_key_state |= STATE_ALT_L;
        }
        else if (sc == SCAN_CODE_SHIFTL)
        {
            g_key_state |= STATE_SHIFT_L;
        }
        else if (sc == SCAN_CODE_SHIFTR)
        {
            g_key_state |= STATE_SHIFT_R;
        }
        else if (sc == SCAN_CODE_CTRL)
        {
            if (g_key_state & STATE_MODIFIER)
                g_key_state |= STATE_CTRL_R;
            else
                g_key_state |= STATE_CTRL_L;
        }
        else if (sc == SCAN_CODE_WINL && (g_key_state & STATE_MODIFIER))
        {
            g_key_state |= STATE_WIN_L;
        }
        else if (sc == SCAN_CODE_WINR && (g_key_state & STATE_MODIFIER))
        {
            g_key_state |= STATE_WIN_R;
        }
        else if (sc == SCAN_CODE_MENUS && (g_key_state & STATE_MODIFIER))
        {
            g_key_state |= STATE_MENUS;
        }

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

        chr = 0;
        if (g_key_state & STATE_MODIFIER)
        {
            switch (sc)
            {
                case SCAN_CODE_SLASHF:
                    chr = '/';
                    break;
                case SCAN_CODE_ENTER:
                    chr = KEY_ENTER;
                    break;
                case SCAN_CODE_END:
                    chr = KEY_END;
                    break;
                case SCAN_CODE_ARROW_L:
                    chr = KEY_LEFTARROW;
                    break;
                case SCAN_CODE_HOME:
                    chr = KEY_HOME;
                    break;
                case SCAN_CODE_INSERT:
                    chr = KEY_INSERT;
                    break;
                case SCAN_CODE_DELETE:
                    chr = KEY_DELETE;
                    break;
                case SCAN_CODE_ARROW_D:
                    chr = KEY_DOWNARROW;
                    break;
                case SCAN_CODE_ARROW_R:
                    chr = KEY_RIGHTARROW;
                    break;
                case SCAN_CODE_ARROW_U:
                    chr = KEY_UPARROW;
                    break;
                case SCAN_CODE_PAGEDOWN:
                    chr = KEY_PAGEDOWN;
                    break;
                case SCAN_CODE_PAGEUP:
                    chr = KEY_PAGEUP;
                    break;
                default: break;
            }

        }
        else if (g_key_state & (STATE_SHIFT_L | STATE_SHIFT_R))
        {
            if (sc < PS2_KEYMAP_SIZE)
                chr = g_ps2_keymap_shift[sc];
        }
        else
        {
            if (sc < PS2_KEYMAP_SIZE)
                chr = g_ps2_keymap[sc];
        }

        if (g_key_mode & STATUS_CAPS_LOCK)
            chr = toupper(chr);

        g_key_state &= ~(STATE_BREAK | STATE_MODIFIER);
        uint16_t res = chr;
        // If there are any modifier keys, add them as flags
        if (g_key_state & (STATE_SHIFT_L | STATE_SHIFT_R))
            res |= MODKEY_SHIFT;
        if (g_key_state & (STATE_CTRL_L | STATE_CTRL_R))
            res |= MODKEY_CTRL;
        if (g_key_state & (STATE_ALT_L | STATE_ALT_R))
            res |= MODKEY_ALT;
        if (g_key_state & (STATE_WIN_L | STATE_WIN_R))
            res |= MODKEY_WIN;
        if (g_key_state & STATE_MENUS)
            res |= MODKEY_MENU;

        if (chr != 0)
        {
            ring_buf_put(&g_buf_keypress, res);
        }
	}
}
