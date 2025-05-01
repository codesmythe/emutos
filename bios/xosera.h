#ifndef XOSERA_H
#define XOSERA_H

#include "portab.h"

#define XOSERA_SCREEN_WIDTH 640
#define XOSERA_SCREEN_HEIGHT 400

#define XOSERA_TEXT_START_ADDR 0xF6A0

typedef enum xosera_mode        // mode numbers for xosera_init
{
    XINIT_DETECT         = -1,        // detect only, do not configure
    XINIT_CONFIG_640x480 = 0,         // configure Xosera 640x480 VGA/DVI 4:3 (flash config #0)
    XINIT_CONFIG_848x480 = 1,         // configure Xosera 848x480 VGA/DVI 16:9 (flash config #1)
    XINIT_CONFIG_USER_2  = 2,         // configure Xosera flash config #2 (user defined/custom)
    XINIT_CONFIG_USER_3  = 3          // configure Xosera flash config #3 (user defined/custom)
} xosera_mode_t;

UWORD xm_getw(int reg_num);
void xm_setw(int reg_num, UWORD value);
void xosera_screen_init(void);

#endif // XOSERA_H
