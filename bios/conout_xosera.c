
#include "config.h"


#if CONF_WITH_XOSERA_CONSOLE

#include <stdbool.h>
#include <stdint.h>

#include "xosera_defs.h"
#include "xosera.h"

#include "portab.h"
#include "conout.h"
#include "lineavars.h"


static UWORD cell_addr(const UWORD x, const UWORD y)
{
    return XOSERA_TEXT_START_ADDR + (v_cel_mx + 1) * y + x;
}

static void xosera_write_char(const uint16_t vram_addr, const int ch)
{
    const uint16_t color = v_stat_0 & M_REVID ? v_col_fg << 12 | v_col_bg << 8 : v_col_bg << 12 | v_col_fg << 8;
    xm_setw(XM_WR_ADDR, vram_addr);
    xm_setw(XM_DATA, color | ch);
}

static void neg_cell(const UWORD cell_addr)
{
    // Get the word at the given cell address.
    xm_setw(XM_RD_ADDR, cell_addr);
    const uint16_t ch = xm_getw(XM_DATA);

    // Swap foreground and background colors.
    const uint16_t new_bg = (ch >>  8 & 0xF) << 12;
    const uint16_t new_fg = (ch >> 12 & 0xF) <<  8;

    // Set the updated word at the given cell address.
    xm_setw(XM_WR_ADDR, cell_addr);
    xm_setw(XM_DATA, new_bg | new_fg | (ch & 0xFF));
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
    const uint16_t color = v_col_bg << 12 | v_col_bg << 8;
    int x, y;
    xm_setw(XM_WR_INCR, 1);
    for (y = top_y; y <= bottom_y; y++) {
        uint16_t addr = cell_addr(0, y);
        for (x = top_x; x < bottom_x; x++) {
            xosera_write_char(addr, color | ' ');
            addr++;
        }
    }
}

/*
 * scroll_up - Scroll upwards
 */

void scroll_up(const UWORD top_line)
{
    const uint16_t dest_vram = cell_addr(0, top_line);
    const uint16_t src_vram  = dest_vram + (v_cel_mx + 1); // one row below dest
    const uint16_t count = (v_cel_my + 1 - top_line) * (v_cel_mx + 1);
    xm_setw(XM_RD_ADDR, src_vram);
    xm_setw(XM_WR_ADDR, dest_vram);
    xm_setw(XM_RD_INCR, 1);
    xm_setw(XM_WR_INCR, 1);
    int i;
    for (i = 0; i< count; i++) {
        const uint16_t val = xm_getw(XM_DATA);
        xm_setw(XM_DATA, val);
    }

    blank_out(0, v_cel_my, v_cel_mx, v_cel_my);
}

/*
 * scroll_down - Scroll (partially) downwards
 */

void scroll_down(const UWORD start_line)
{
    xm_setw(XM_RD_INCR, 1);
    xm_setw(XM_WR_INCR, 1);
    int row, i;
    for (row = v_cel_my; row > start_line; row--) {
        const uint16_t dst_vram = cell_addr(0, row);
        const uint16_t src_vram = dst_vram - (v_cel_mx + 1);
        xm_setw(XM_RD_ADDR, src_vram);
        xm_setw(XM_WR_ADDR, dst_vram);
        for (i = 0; i < v_cel_mx + 1; i++) {
            const uint16_t val = xm_getw(XM_DATA);
            xm_setw(XM_DATA, val);
        }
    }
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
    if (visible) {
        v_stat_0 &= ~M_CVIS;                    /* start of critical section */
    }

    /* put the cell out (this covers the cursor) */
    xosera_write_char(cell_addr(v_cur_cx, v_cur_cy), ch);

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