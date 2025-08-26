
#include "config.h"

/* #define ENABLE_KDEBUG */

#if CONF_WITH_DDRAIGVGA_CONSOLE

#include <stdbool.h>
#include <stdint.h>

#include "ddraig_vga.h"

#include "emutos.h"
#include "portab.h"
#include "conout.h"
#include "lineavars.h"

#define RGB_BLACK          0x0000
#define RGB_BLUE           0x0100
#define RGB_GREEN          0x0200
#define RGB_CYAN           0x0300
#define RGB_RED            0x0400
#define RGB_MAGENTA        0x0500
#define RGB_BROWN          0x0600
#define RGB_LIGHTGRAY      0x0700
#define RGB_GRAY           0x0800
#define RGB_LIGHTBLUE      0x0900
#define RGB_LIGHTGREEN     0x0A00
#define RGB_LIGHTCYAN      0x0B00
#define RGB_LIGHTRED       0x0C00
#define RGB_LIGHTMAGENTA   0x0D00
#define RGB_YELLOW         0x0E00
#define RGB_WHITE          0x0F00

static const UWORD dflt_palette[] = {
    RGB_WHITE, RGB_RED, RGB_GREEN, RGB_YELLOW,
    RGB_BLUE, RGB_MAGENTA, RGB_CYAN, RGB_LIGHTGRAY,
    RGB_GRAY, RGB_LIGHTRED, RGB_LIGHTGREEN, RGB_BROWN,
    RGB_LIGHTBLUE, RGB_LIGHTMAGENTA, RGB_LIGHTCYAN, RGB_BLACK
};

static UWORD cell_addr(const UWORD x, const UWORD y)
{
    return (v_cel_mx + 1) * y + x;
}

static void ddraig_write_char(const uint16_t addr, const int ch)
{
    const uint16_t color = v_stat_0 & M_REVID ? 
        dflt_palette[v_col_bg] | (dflt_palette[v_col_fg] << 4) : dflt_palette[v_col_fg] | (dflt_palette[v_col_bg] << 4);
    KDEBUG(("ddraig_write_char: addr=%u, ch=0x%02X, color=0x%04X\n", addr, ch & 0xFF, color));
    drvga_write_char(addr, color | (ch & 0xFF));
}

static void neg_cell(const UWORD cell_addr)
{
    // Get the word at the given cell address.
    const uint16_t ch = drvga_read_char(cell_addr);

    // Swap foreground and background colors.
    const uint16_t new_bg = (ch & 0x0F00) << 4;
    const uint16_t new_fg = (ch & 0xF000) >> 4;
    KDEBUG(("neg_cell: addr=%u, orig_char=%04X, new_col=0x%4X\n", cell_addr, ch, new_bg | new_fg));
    // Set the updated word at the given cell address.
    drvga_write_char(cell_addr, new_bg | new_fg | (ch & 0x00FF));
}

/*
 * invert_cell - negates the cells bits
 *
 * This routine negates the contents of an arbitrarily-tall byte-wide cell
 * composed of an arbitrary number of (Atari-style) bit-planes.
 *
 * Wrapper for neg_cell().
 *
 * in:
 * x - cell X coordinate
 * y - cell Y coordinate
 */

void invert_cell(int x, int y)
{
    /* fetch x and y coords and invert cursor. */
    neg_cell(cell_addr(x, y));
}

/*
 * move_cursor - move the cursor.
 *
 * move the cursor and update global parameters
 * erase the old cursor (if necessary) and draw new cursor (if necessary)
 *
 * in:
 * d0.w    new cell X coordinate
 * d1.w    new cell Y coordinate
 */

void move_cursor(int x, int y)
{
    /* update cell position */
    KDEBUG(("move_cursor(%d, %d)\n", x, y));
    /* clamp x,y to valid ranges */
    if (x < 0) x = 0;
    else if (x > v_cel_mx) x = v_cel_mx;

    if (y < 0) y = 0;
    else if (y > v_cel_my) y = v_cel_my;

    /* is cursor visible? */
    if (!(v_stat_0 & M_CVIS)) {
        /* Not visible, so just set new position and return. */
        v_cur_cx = x;
        v_cur_cy = y;
        return;
    }
    /* is cursor flashing? */
    if (v_stat_0 & M_CFLASH) {
        v_stat_0 &= ~M_CVIS;                    /* yes, make invisible...semaphore. */

        /* is cursor presently displayed ? */
        if (!(v_stat_0 & M_CSTATE)) {
            /* not displayed */
            v_cur_cx = x;
            v_cur_cy = y;
            /* show the cursor when it moves */
            neg_cell(cell_addr(x, y));                         /* complement cursor. */
            v_stat_0 |= M_CSTATE;
            v_cur_tim = v_period;               /* reset the timer. */
            v_stat_0 |= M_CVIS;                 /* end of critical section. */
            return;
        }
    }

    /* move the cursor after all special checks failed */
    neg_cell(cell_addr(v_cur_cx, v_cur_cy));                               /* erase present cursor */
    v_cur_cx = x;
    v_cur_cy = y;
    neg_cell(cell_addr(v_cur_cx, v_cur_cy));                                /* complement cursor. */

    /* do not flash the cursor when it moves */
    v_cur_tim = v_period;                       /* reset the timer. */
    v_stat_0 |= M_CVIS;                         /* end of critical section. */
}

/*
 * blank_out - Fills region with the background color.
 *
 */

void blank_out(const int top_x, const int top_y, const int bottom_x, const int bottom_y)
{
    const uint16_t color = dflt_palette[v_col_bg] << 4 | dflt_palette[v_col_bg];
    int x, y;
    for (y = top_y; y <= bottom_y; y++) {
        uint16_t addr = cell_addr(0, y);
        for (x = top_x; x < bottom_x; x++) {
            ddraig_write_char(addr, color | ' ');
            addr++;
        }
    }
}

/*
 * scroll_up - Scroll upwards
 */

void scroll_up(const UWORD top_line)
{
    drvga_scroll_up();
    blank_out(0, v_cel_my, v_cel_mx, v_cel_my);
}

/*
 * scroll_down - Scroll (partially) downwards
 */

void scroll_down(const UWORD start_line)
{
    drvga_scroll_down();
    blank_out(0, start_line, v_cel_mx, start_line);
}

static bool next_cell(void)
{
    if (v_cur_cx == v_cel_mx) {
        // We have reached the end of the line. Decide whether not to wrap.
        if (!(v_stat_0 & M_CEOL)) {
            /* Overwrite is in effect, don't move the cursor. */
            return false;
        }

        /* call carriage return routine */
        /* call line feed routine */
        return true;                       /* indicate that CR LF is required */
    }

    v_cur_cx++;
    return false;
}

/*
 * ascii_out - prints an ascii character on the screen
 *
 * in:
 *
 * ch.w      ascii code for character
 */

void ascii_out(const int ch)
{
    const bool visible = v_stat_0 & M_CVIS;        /* test visibility bit */
    //KDEBUG(("ascii_out: ch=0x%02X, cursor at (%u,%u), visible=%d\n", ch & 0xFF, v_cur_cx, v_cur_cy, visible));
    if (visible) {
        v_stat_0 &= ~M_CVIS;                    /* start of critical section */
    }

    /* put the cell out (this covers the cursor) */
    ddraig_write_char(cell_addr(v_cur_cx, v_cur_cy), ch);

    if (next_cell()) {
        /* Need to do a carriage return / line feed */
        v_cur_cx = 0;
        if (v_cur_cy < v_cel_my) v_cur_cy++;
        else scroll_up(0);
    }
    if (visible) {
        neg_cell(cell_addr(v_cur_cx, v_cur_cy));                 /* display cursor. */
        v_stat_0 |= M_CSTATE;           /* set state flag (cursor on). */
        v_stat_0 |= M_CVIS;             /* end of critical section. */

        /* do not flash the cursor when it moves */
        if (v_stat_0 & M_CFLASH) {
            v_cur_tim = v_period;       /* reset the timer. */
        }
    }
}

#endif