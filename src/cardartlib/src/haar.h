#ifndef HAAR_H
#define HAAR_H

#include "CardArtLib.h"
#include "defs.h"
#include "inttypes.h"
#include "mystdbool.h"

uint *Wvl_DecodeToBgr24(byte *param_1, int *wvl_entry, int width, int height);

extern undefined4 global_dither_kernel_id;
extern undefined4 global_color_depth;

#endif
