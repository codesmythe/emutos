#include "config.h"

#if CONF_WITH_DDRAIGVGA_CONSOLE

#include <stdbool.h>
#include <stdint.h>

#include "ddraig_vga.h"
#include "lineavars.h"
#include "tosvars.h"

typedef volatile uint8_t *DDRAIGVGA_PTR;

// Hard codeed for now, should be detected
volatile uint16_t *ddraigvga_base = (uint16_t *)0xF7F500;

void drvga_write_control_reg(uint16_t data)
{
    DRVGA_REG_WRITE(REG_CONTROL, data);
}


/* Sets system variables so that EmuTOS will use the graphics card */
static void init_system_vars(void)
{
    KDEBUG(("init_system_vars()\n"));

    /* Screen address */
    //v_bas_ad = (UBYTE *)novamembase;
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
    drvga_write_control_reg(DISPMODE_TEXT);
    init_system_vars();
}



#endif
