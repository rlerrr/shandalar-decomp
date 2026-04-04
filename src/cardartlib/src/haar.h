#ifndef HAAR_H
#define HAAR_H

#include "defs.h"
#include "inttypes.h"
#include "mystdbool.h"

int *Catalog_LoadWvlEntry(int catalog_id, char *wvl_path, int decode_haar);
uint *Wvl_DecodeToBgr24(byte *param_1, int *wvl_entry, int width, int height);
byte *Wvl_DecodeHaar(int *param_1, byte *param_2);

extern undefined4 global_dither_kernel_id;
extern undefined4 global_color_depth;

#endif
