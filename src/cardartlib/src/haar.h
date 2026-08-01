#ifndef HAAR_H
#define HAAR_H

#include "defs.h"
#include "inttypes.h"
#include "mystdbool.h"

typedef struct WvlEntry {
  int chroma_is_420;
  int unk_04;
  int unk_08;
  int unk_0c;
  int unk_10;
  int unk_14;
  int unk_18;
  int width;
  int height;
  int base_size;
  int pieces;
  int unk_2c;
  int unk_30;
  int unk_34;
  int unk_38;
  int unk_3c;
  int unk_40;
  int unk_44;
  int unk_48;
  int unk_4c;
  int unk_50;
  int unk_54;
  int unk_58;
  int huff_bytes_y[4];
  int huff_bytes_u[4];
  int huff_bytes_v[4];
  int unk_88;
  int decode_bytes;
  int unk_94;
  int unk_98;
  char wvl_path[0x104];
  byte *data_ptr;
  int data_size;
  int decoded_flag;
  byte *decoded_ptr;
} WvlEntry;

STATIC_ASSERT(sizeof(WvlEntry) == 0x1b0, WvlEntry_wrong_size);

WvlEntry *Catalog_LoadWvlEntry(int catalog_id, char *wvl_path, int decode_haar);
uint *Wvl_DecodeToBgr24(byte *out_bgr24, WvlEntry *wvl_entry, int width, int height);
byte *Wvl_DecodeHaar(WvlEntry *wvl, byte *dst);

extern undefined4 global_dither_kernel_id;
extern undefined4 global_color_depth;

#endif
