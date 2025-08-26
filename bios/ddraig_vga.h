#ifndef _DDRAIG_VGA_H_
#define _DDRAIG_VGA_H_

#define DRVGA_TEXTBUF_SIZE  2400

#include <stdint.h>
#include "portab.h"

extern uint32_t ddraigvga_base;

#define DRVGA_REG_WRITE(x, y)  (*((volatile uint16_t *) (ddraigvga_base + (x))) = (y))
#define DRVGA_REG_READ(x)      (*((volatile uint16_t *) (ddraigvga_base + (x))))

#define	REG_STATUS              0x00    // Status register
#define REG_CONTROL             0x02    // Display control register
#define REG_INTERRUPT           0x04    // Interrupt control/ status
#define	REG_COMMAND             0x06    // Command control register
#define REG_DATA                0x08    // Command data register
#define	REG_PARAM_DATA0         0x0A    // Command parameter 0
#define	REG_PARAM_DATA1         0x0C    // Command parameter 1
#define	REG_PARAM_DATA2         0x0E    // Command parameter 2
#define	REG_PARAM_DATA3         0x10    // Command parameter 3
#define	REG_PARAM_DATA4         0x12    // Command parameter 4
#define	REG_PARAM_DATA5         0x14    // Command parameter 5
#define	REG_PARAM_DATA6         0x16    // Command parameter 6
#define	REG_PARAM_DATA7         0x18    // Command parameter 7

#define REG_PALETTE_IDX         0x20    // Palette index register
#define REG_PALETTE_DATA        0x22    // Palette data register
#define REG_PALETTE_CTL         0x24    // Palette control register

#define REG_BITMAP_PTR_L		0x30    // Bitmap data address location (L) 
#define REG_BITMAP_PTR_H		0x32    // Bitmap data address location (H)
#define REG_BITMAP_SCROLL_X 	0x34    // Bitmap horizontal scroll position
#define REG_BITMAP_SCROLL_Y 	0x36    // Bitmap vertical scroll position

#define REG_LAYER1_PTR_L	    0x38    // Tile layer 1 data address location (L)
#define REG_LAYER1_PTR_H	    0x3A    // Tile layer 1 data address location (H)
#define REG_LAYER1_SCROLL_X	    0x3C    // Tile later 1 horizontal scroll position77
#define REG_LAYER1_SCROLL_Y	    0x3E    // Tile later 1 vertical scroll position
#define REG_LAYER1_IDX		    0x40    // Tile later 1 map index
#define REG_LAYER2_PTR_L	    0x42    // Tile layer 2 data address location (L)
#define REG_LAYER2_PTR_H	    0x44    // Tile layer 2 data address location (H)
#define REG_LAYER2_SCROLL_X	    0x46    // Tile later 2 horizontal scroll position77
#define REG_LAYER2_SCROLL_Y	    0x48    // Tile later 2 vertical scroll position
#define REG_LAYER2_IDX		    0x4A    // Tile later 2 map index

#define REG_SPRITE_PTR_L		0x4C    // Sprite data address location (L)
#define REG_SPRITE_PRT_H		0x4E    // Sprite data address location (H)
#define REG_SPRITE_IDX			0x50    // Sprite index
#define REG_SPRITE_CONTROL		0x52    // Sprite control data
#define REG_SPRITE_POSITION_X	0x54    // Sprite horizontal position
#define REG_SPRITE_POSITION_Y	0x56    // Sprite vertical position

#define	REG_PARAM_COLOR         REG_PARAM_DATA0
#define	REG_PARAM_X0            REG_PARAM_DATA1
#define	REG_PARAM_Y0            REG_PARAM_DATA2
#define	REG_PARAM_X1            REG_PARAM_DATA3
#define	REG_PARAM_Y1            REG_PARAM_DATA4

// Status register masks
#define STATUS_READY            0x0001
#define STATUS_ERROR            0x0002
#define STATUS_HSYNC            0x0004
#define STATUS_VSYNC            0x0008

// Control register settings
// Display modes
#define DISPMODE_TEXT			0x0000      // Text mode, default
#define DISPMODE_BITMAP			0x0001      // Bitmap 320x240
#define DISPMODE_BITMAPHIRES	0x0002      // Bitmap 640x480
#define DISPMODE_TILE1			0x0003      // Tile layer 1 (320x240)
#define DISPMODE_TILE2			0x0004      // Tile layer 2 (320x240)
#define DISPMODE_BMAPTILE		0x0005      // Bitmap with Tile overlay (320x240)
#define DISPMODE_BGCOLOR		0x0006      // Background color only
#define DISPMODE_RESERVED		0x0007      // Reserved for future use

// Bitmap mode bits per pixel
#define DISP_DEPTH_12BPP		0x0000
#define DISP_DEPTH_8BPP			0x0008
#define DISP_DEPTH_4BPP			0x0009
#define DISP_DEPTH_2BPP			0x000A
#define DISP_DEPTH_1BPP			0x000B

#define DISP_WIDTH_512			0x0000
#define DISP_WIDTH_1024			0x0040
#define DISP_WIDTH_2048			0x0080
#define DISP_WIDTH_4096			0x00C0

#define CMD_CLEAR_SCREEN    0x0001
#define CMD_FILL_RECT       0x0002
#define CMD_DRAW_PIXEL      0x0003
#define CMD_DRAW_LINE       0x0004
#define CMD_SET_CHARACTER   0x0010
#define CMD_SET_TEXTCOLOR   0x0011
#define CMD_SET_TEXTAREA    0x0012
#define CMD_MEMORY_ACCESS   0x0020


void drvga_write_control_reg(uint16_t data);
void drvga_write_char(uint16_t address, uint16_t text);
uint16_t drvga_read_char(uint16_t address);
void drvga_copy_buffer(void);
void drvga_scroll_up(void);
void drvga_scroll_down(void);
void ddraigvga_screen_init(void);

#endif
