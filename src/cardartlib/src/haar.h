#ifndef HAAR_H
#define HAAR_H

#include "defs.h"
#include "inttypes.h"
#include "mystdbool.h"

uint *Wvl_DecodeToBgr24(byte *param_1, int *wvl_entry, int width, int height);
byte *Wvl_DecodeHaar(int *param_1, byte *param_2);

extern undefined4 global_dither_kernel_id;
extern undefined4 global_color_depth;

#endif
