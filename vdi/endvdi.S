/*
 * endvdi.S - Last source of VDI
 *
 * Copyright (C) 2016-2019 The EmuTOS development team.
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#include "asmdefs.h"

        .bss

// This label is referred by the OSHEADER
        .globl  __endvdibss
__endvdibss:
