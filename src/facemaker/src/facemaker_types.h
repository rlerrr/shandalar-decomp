#ifndef FACEMAKER_TYPES_H
#define FACEMAKER_TYPES_H

#include <windows.h>

typedef struct FacemakerWindowBounds
{
  int unk_00;
  int unk_04;
  int unk_08;
  int max_x;
  int max_y;
  int unk_14;
  int unk_18;
  int unk_1c;
  int unk_20;
  struct FacemakerWindowBounds *self_ptr;
} FacemakerWindowBounds;

typedef union FontSlotData
{
  struct
  {
    char unk_020[0x60];
    unsigned char glyph_advance[0x80];
    char unk_100[0x120];
  } bitmap;
  struct
  {
    char font_file[0x100];
    char font_name[0x100];
  } gdi;
} FontSlotData;

typedef struct FontSlot
{
  unsigned char first_char;
  unsigned char last_char;
  unsigned char glyph_width;
  unsigned char has_packed_widths;
  unsigned char point_size;
  unsigned char unk_05;
  unsigned char unk_06;
  unsigned char row_padding;
  HBITMAP bitmap_inverted;
  HBITMAP bitmap_normal;
  HDC hdc;
  unsigned char *bitmap_data;
  int font_loaded;
  HFONT hfont;
  FontSlotData data;
  int tm_min;
  int tm_max;
  LONG tm_leading;
  char unk_22c[0x78];
} FontSlot;

typedef struct EncodedImage
{
  int total_size;
  short width;
  short height;
  short left_clip;
  short top_clip;
  short first_row;
  short row_count;
  unsigned char spans[1];
} EncodedImage;

typedef char FontSlot_size_must_be_0x2a4[(sizeof(FontSlot) == 0x2a4) ? 1 : -1];

#endif
