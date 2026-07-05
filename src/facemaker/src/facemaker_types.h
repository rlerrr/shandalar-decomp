#ifndef FACEMAKER_TYPES_H
#define FACEMAKER_TYPES_H

#include <windows.h>

typedef struct FacemakerWindowBounds
{
  int page_number;         // 0x0
  int clip_left;           // 0x4
  int clip_top;            // 0x8
  int max_x;               // 0xc
  int max_y;               // 0x10
  int draw_shadow_enabled; // 0x14
  int text_color;          // 0x18
  int unk_1c;              // 0x1c
  int font_slot;           // 0x20
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
  int total_size;         // 0x0
  short width;            // 0x4
  short height;           // 0x6
  short left_clip;        // 0x8
  short top_clip;         // 0xa
  short first_row;        // 0xc
  short row_count;        // 0xe
  unsigned char spans[1]; // 0x10
} EncodedImage;

typedef char FontSlot_size_must_be_0x2a4[(sizeof(FontSlot) == 0x2a4) ? 1 : -1];

#endif
