#include "config.h"

/* #define ENABLE_KDEBUG */

#if CONF_WITH_DDRAIGVGA_CONSOLE

#include <stdbool.h>

#include "ddraig_vga.h"
#include "emutos.h"
#include "lineavars.h"
#include "tosvars.h"

#define VGA_CHAR_WIDTH  80
#define VGA_CHAR_HEIGHT 30

// Hard codeed for now, should be detected
uint32_t ddraigvga_base = 0xF7F500;

static uint16_t ddraigvga_screenbuf[DRVGA_TEXTBUF_SIZE];;

void drvga_write_control_reg(uint16_t data)
{
    DRVGA_REG_WRITE(REG_CONTROL, data);
}

void drvga_write_char(uint16_t address, uint16_t text)
{
    DRVGA_REG_WRITE(REG_PARAM_DATA0, address);
    DRVGA_REG_WRITE(REG_PARAM_DATA1, text);
    DRVGA_REG_WRITE(REG_COMMAND, CMD_SET_CHARACTER);
    if (address < DRVGA_TEXTBUF_SIZE) {
        ddraigvga_screenbuf[address] = text;
    }
}

uint16_t drvga_read_char(uint16_t address)
{
    if (address < DRVGA_TEXTBUF_SIZE) {
        return ddraigvga_screenbuf[address];
    } else {
        return 0;
    }
}

void drvga_copy_buffer(void)
{
    uint16_t i;

    DRVGA_REG_WRITE(REG_PARAM_DATA0, 0);
    for (i = 0; i < DRVGA_TEXTBUF_SIZE; i++)
    {
        DRVGA_REG_WRITE(REG_PARAM_DATA1, ddraigvga_screenbuf[i]);
        DRVGA_REG_WRITE(REG_COMMAND, CMD_SET_CHARACTER);
    }
}

void drvga_scroll_up(void)
{
    uint16_t *src = &ddraigvga_screenbuf[VGA_CHAR_WIDTH];
    uint16_t *dst = &ddraigvga_screenbuf[0];
    uint16_t i;

    for (i = 0; i < (VGA_CHAR_HEIGHT - 1) * 80; i++) {
        *dst++ = *src++;
    }
    // blank out last line
    dst = &ddraigvga_screenbuf[(VGA_CHAR_HEIGHT - 1) * VGA_CHAR_WIDTH];
    for (i = 0; i < VGA_CHAR_WIDTH; i++) {
        *dst++ = ' ';
    }
    drvga_copy_buffer();
}

void drvga_scroll_down(void)
{
    uint16_t *src = &ddraigvga_screenbuf[(VGA_CHAR_HEIGHT - 2) * VGA_CHAR_WIDTH];
    uint16_t *dst = &ddraigvga_screenbuf[(VGA_CHAR_HEIGHT - 1) * VGA_CHAR_WIDTH];
    uint16_t i;

    // Move lines down
    for (i = 0; i < (VGA_CHAR_HEIGHT - 1) * VGA_CHAR_WIDTH; i++) {
        *dst-- = *src--;
    }
    // Blank out first line
    dst = &ddraigvga_screenbuf[0];
    for (i = 0; i < VGA_CHAR_WIDTH; i++) {
        *dst++ = ' ';
    }
    drvga_copy_buffer();
}

/* Sets system variables so that EmuTOS will use the graphics card */
static void init_system_vars(void)
{
    KDEBUG(("init_system_vars()\n"));
    /* Screen address */
    v_bas_ad = 0;
    /* Fake 640x400x2 video mode (ST high) */
    sshiftmod = 2;

    /* Line A vars */
    /* Number of bitplanes */
    v_planes = 1;
    /* Bytes per scan-line */
    BYTES_LIN = 80;
    /* Vertical resolution */
    V_REZ_VT = 480;
    /* Horizontal resolution */
    V_REZ_HZ = 640;
}

void ddraigvga_screen_init(void)
{
    int i;

    KDEBUG(("ddraigvga_screen_init()\n"));

    drvga_write_control_reg(DISPMODE_TEXT);
    for (i = 0; i < DRVGA_TEXTBUF_SIZE; i++) {
        ddraigvga_screenbuf[i] = ' ';
    }
    drvga_copy_buffer();
    init_system_vars();
}



#endif
