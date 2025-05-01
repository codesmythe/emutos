
#include "config.h"

#if CONF_WITH_XOSERA_CONSOLE

#include <stdbool.h>
#include <stdint.h>

#include "xosera_defs.h"
#include "xosera.h"
#include "lineavars.h"
#include "tosvars.h"

/* palette color definitions (from screen.h) */

#define RGB_BLACK     0x0000            /* ST(e) palette */
#define RGB_BLUE      0x000f
#define RGB_GREEN     0x00f0
#define RGB_CYAN      0x00ff
#define RGB_RED       0x0f00
#define RGB_MAGENTA   0x0f0f
#define RGB_LTGRAY    0x0555
#define RGB_GRAY      0x0333
#define RGB_LTBLUE    0x033f
#define RGB_LTGREEN   0x03f3
#define RGB_LTCYAN    0x03ff
#define RGB_LTRED     0x0f33
#define RGB_LTMAGENTA 0x0f3f
#define RGB_YELLOW    0x0ff0
#define RGB_LTYELLOW  0x0ff3
#define RGB_WHITE     0x0fff


typedef volatile uint8_t *XOSERA_PTR;

#define XM_BASEADDR 0xFF81C1
XOSERA_PTR xosbase = (XOSERA_PTR) XM_BASEADDR;

// TODO: This is less than ideal (tuned for ~10MHz)
__attribute__((noinline)) static void my_cpu_delay(int ms)
{
    __asm__ __volatile__(
            "    lsl.l   #8,%[temp]\n"
            "    add.l   %[temp],%[temp]\n"
            "0:  sub.l   #1,%[temp]\n"
            "    tst.l   %[temp]\n"
            "    bne.s   0b\n"
            : [temp] "+d"(ms));
}

void xm_setw(const int reg_num, const uint16_t value)
{
    *(xosbase + reg_num) = value >> 8; // write the upper part
    *(xosbase + reg_num + 2) = value & 0xff; // write the lower part
}

static void xm_setbh(const int reg_num, const uint8_t value)
{
    *(xosbase + reg_num) = value ; // write the upper part
}

uint16_t xm_getw(const int reg_num)
{
    const uint8_t upper = *(xosbase + reg_num);
    const uint8_t lower = *(xosbase + reg_num + 2);
    uint16_t result = upper << 8;
    result |= lower;
    return result;
}

// uint16_t xreg_getw(xreg_name) - get word val from XR_<xreg_name>
static uint16_t xreg_getw(const uint8_t xreg_num)
{
    xm_setw(XM_RD_XADDR, xreg_num);
    return xm_getw(XM_XDATA);
}

static void xreg_setw(int xreg_num, const uint16_t value)
{
    xm_setw(XM_WR_XADDR, xreg_num);
    xm_setw(XM_XDATA, value);
}

#if 0

#include <stdio.h>

static void read_build_info(void)
{
    uint8_t buffer[XV_INFO_BYTES];
    xm_setw(XM_RD_XADDR, XV_INFO_ADDR);
    for (int i = 0; i < 64; i++) {
        const uint16_t v = xm_getw(XM_XDATA);
        buffer[i*2] = (v >> 8) & 0xFF;
        buffer[i*2+1] = v & 0xFF;
    }
    printf("%s\n", (const char *) buffer);
}

#endif

static bool xosera_sync(XOSERA_PTR xosbase)
{
    uint16_t rd_incr = xm_getw(XM_RD_INCR);
    uint16_t test_incr = rd_incr ^ 0xF5FA;
    xm_setw(XM_RD_INCR, test_incr);
    if (xm_getw(XM_RD_INCR) != test_incr) {
        return false;        // not detected
    }
    xm_setw(XM_RD_INCR, rd_incr);

    return true;
}

#define SYNC_RETRIES 250

// wait for Xosera to respond after reconfigure
static bool xosera_wait_sync(XOSERA_PTR xosbase)
{
    // check for Xosera presense (retry in case it is reconfiguring)
    short r;
    for (r = SYNC_RETRIES; r != 0; --r) {
        if (xosera_sync(xosbase)) {
            return true;
        }
        my_cpu_delay(10);
    }
    return false;
}

// Wait while VBLANK is clear, returning when it is set.
static void xwait_not_vblank(void)
{
    uint8_t sys_ctrl = *(xosbase + XM_SYS_CTRL);
    while ((sys_ctrl & SYS_CTRL_VBLANK_F) == 0) {
        sys_ctrl = *(xosbase + XM_SYS_CTRL);
    }
}

// Wait while VBLANK is set, returning when it is clear.
static void xwait_vblank(void)
{
    uint8_t sys_ctrl = *(xosbase + XM_SYS_CTRL);
    while (sys_ctrl & SYS_CTRL_VBLANK_F) {
        sys_ctrl = *(xosbase + XM_SYS_CTRL);
    }
}

// reconfigure or sync Xosera and return true if it is responsive
// NOTE: May BUS ERROR if no hardware present
static bool xosera_init(xosera_mode_t init_mode)
{
    bool detected = xosera_wait_sync(xosbase);

    if (detected) {
        xwait_not_vblank();
        xwait_vblank();
        if (init_mode >= XINIT_CONFIG_640x480) {
            xm_setbh(XM_INT_CTRL, 0x80 | (init_mode & 0x03));        // reconfig FPGA to init_mode 0-3
            detected = xosera_wait_sync(xosbase);                        // wait for detect
            if (detected) {
                // wait for initial copper program to disable itself (or timeout)
                uint16_t timeout = 100;
                do {
                    my_cpu_delay(1);
                } while ((xreg_getw(XR_COPP_CTRL) & COPP_CTRL_COPP_EN_F) && --timeout);
            }
        }
    }
    return detected;
}

static const UWORD dflt_palette[] = {
    RGB_WHITE, RGB_RED, RGB_GREEN, RGB_YELLOW,
    RGB_BLUE, RGB_MAGENTA, RGB_CYAN, RGB_LTGRAY,
    RGB_GRAY, RGB_LTRED, RGB_LTGREEN, RGB_LTYELLOW,
    RGB_LTBLUE, RGB_LTMAGENTA, RGB_LTCYAN, RGB_BLACK
};

static void setup_xosera_text_screen(uint16_t disp_vram_start)
{
    // Video registers
    xreg_setw(XR_VID_CTRL, 0); // Border color 0 (black), no playfield color swap.
    xreg_setw(XR_VID_RIGHT, XOSERA_SCREEN_WIDTH); // always 640
    xreg_setw(XR_PA_GFX_CTRL, 0x0080); // Blank playfield A

    // Setup playfield B to be a tiled screen in 1 bit per pixel plus attribute mode.
    // Use TILEMEM as opposed to VRAM for tile data.
    uint16_t pb_gfx_ctrl = (GFX_1_BPP << GFX_CTRL_BPP_B);
    xreg_setw(XR_PB_GFX_CTRL, pb_gfx_ctrl);
    xreg_setw(XR_PB_LINE_LEN, 80);
    xreg_setw(XR_PB_TILE_CTRL, 15); // tile height = 16
    xreg_setw(XR_PB_DISP_ADDR, disp_vram_start);


    // Setup color palette.
    xm_setw(XM_WR_XADDR, XR_COLOR_B_ADDR);
    int i;
    for (i = 0; i < 16; i++) {
        uint16_t value = dflt_palette[i];
        if (value != RGB_BLACK) value |= 0xF000; // Set the alpha for non-black colors.
        xm_setw(XM_XDATA, value);
    }

    // Some of the main registers.
    xm_setw(XM_WR_INCR, 1);
    xm_setw(XM_RD_INCR, 1);
    xm_setw(XM_WR_ADDR, 0);
    xm_setw(XM_RD_ADDR, 0);
}

/* Sets system variables so that EmuTOS will use the graphics card */
static void init_system_vars(void)
{
    /* Screen address */
    //v_bas_ad = (UBYTE *)novamembase;
    /* Fake 640x200x2 video mode (ST medium) */
    sshiftmod = 1;

    /* Line A vars */
    /* Number of bitplanes */
    v_planes = 1;
    /* Bytes per scan-line */
    BYTES_LIN = v_lin_wr = 160;
    /* Vertical resolution */
    V_REZ_VT = 240;
    /* Horizontal resolution */
    V_REZ_HZ = 640;
}

void xosera_screen_init(void)
{
    const bool detected = xosera_init(XINIT_CONFIG_640x480);

    if (detected) {
        setup_xosera_text_screen(XOSERA_TEXT_START_ADDR);
        init_system_vars();
    }
}

#endif