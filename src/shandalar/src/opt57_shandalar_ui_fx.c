// Optimized helpers from the 0x0057xxxx region that are implemented only in
// SHANDALAR, but were originally scattered across multiple UI modules.

#include <windows.h>
#include <stdlib.h>

#include "facemaker/src/facemaker_types.h"

int DrawTextFormatted(FacemakerWindowBounds *dst, int text_color, int draw_shadow, int scale_to_screen, int center_x, int center_y,
                      int x, int y, int *format_and_args);
void DrawEncodedImageResampled(FacemakerWindowBounds *dst, int x, int y, int width, int height, EncodedImage *encoded_image);

extern FontSlot g_font_slots[0x10];

// From visitLocation.c (used by BlitRectByStaggeredRandomTileOrder)
extern int g_ui_fx_lcg_multipliers[0x14];

// FUNCTION: SHANDALAR 0x0057ae30
#pragma optimize("gy", on)
int GetFontStyleSize(int font_slot)
{
  return g_font_slots[font_slot].point_size;
}

// FUNCTION: SHANDALAR 0x0057b440
void __cdecl DrawFormattedTextNoShadow(FacemakerWindowBounds *dst, int text_color, int x, int y, char *format, ...)
{
  DrawTextFormatted(dst, text_color, 0, 0, 0, 0, x, y, (int *)&format);
}

// FUNCTION: SHANDALAR 0x0057b470
void DrawLoadSaveButtonText(FacemakerWindowBounds *window, int color, int x, int y, ...)
{
  DrawTextFormatted(window, color, 0, 0, 0, 1, x, y, (int *)(&y + 1));
}

// FUNCTION: SHANDALAR 0x0057b4a0
void __cdecl DrawFormattedTextNoShadowCentered(FacemakerWindowBounds *dst, int text_color, int x, int y, char *format, ...)
{
  DrawTextFormatted(dst, text_color, 0, 0, 1, 1, x, y, (int *)&format);
}

// FUNCTION: SHANDALAR 0x0057b500
void DrawWorldUiFormattedText(FacemakerWindowBounds *window, int color_index, int x, int y, char *format, ...)
{
  DrawTextFormatted(window, color_index, 1, 0, 1, 0, x, y, (int *)&format);
}


// FUNCTION: SHANDALAR 0x0057b5c0
void __cdecl DrawUiScaledCenteredTextWithShadow(FacemakerWindowBounds *window, int color, int x, int y, char *format, ...)
{
  DrawTextFormatted(window, color, 1, 1, 1, 0, x, y, (int *)&format);
}
// FUNCTION: SHANDALAR 0x0057c450
void __cdecl GetEncodedImageSpanXExtents(EncodedImage *image, unsigned int *out_min_x, int *out_max_x)
{
  int rows;
  unsigned char *cursor;

  *out_min_x = 0x7fffffff;
  *out_max_x = 0;

  if (image == (EncodedImage *)0)
  {
    return;
  }

  rows = (int)image->row_count;
  cursor = (unsigned char *)image->spans;
  while (rows > 0)
  {
    unsigned int span_x = (unsigned int)*cursor;
    unsigned int span_len;

    cursor++;
    if (span_x != 0xff)
    {
      span_len = (unsigned int)*cursor;
      cursor++;
      if (span_len == 0xfe)
      {
        span_len = (unsigned int)*cursor;
        cursor++;
      }

      if ((int)span_x < (int)*out_min_x)
      {
        *out_min_x = span_x;
      }
      if (*out_max_x < (int)(span_x + span_len))
      {
        *out_max_x = (int)(span_x + span_len);
      }

      cursor += span_len;
    }

    rows--;
  }
}

// FUNCTION: SHANDALAR 0x0057c4c0
int __cdecl DrawEncodedImageResampledFitBoxCentered(FacemakerWindowBounds *dst, int x, int y, int box_w, int box_h,
                                                    EncodedImage *image)
{
  int src_w;
  int src_h;
  int draw_w;
  int draw_h;

  if (image == (EncodedImage *)0)
  {
    return 0;
  }

  src_w = (int)image->width;
  src_h = (int)image->height;

  if (((box_h << 8) / src_h) < ((box_w << 8) / src_w))
  {
    draw_w = (src_w * box_h) / src_h;
    DrawEncodedImageResampled(dst, x + (box_w - draw_w) / 2, y, draw_w, box_h, image);
    return 0;
  }

  draw_h = (src_h * box_w) / src_w;
  DrawEncodedImageResampled(dst, x, y + (box_h - draw_h) / 2, box_w, draw_h, image);
  return 0;
}

// FUNCTION: SHANDALAR 0x0057d180
unsigned int BlitRectByRandomTileOrderInPlace(HDC dst, int dst_x, int dst_y, int width, int height, int tile_w, int tile_h,
                                              HDC src)
{
  int blocks_x;
  int blocks_y;
  int bit_count;
  int block_y;
  COLORREF color;
  unsigned int bit_cursor;
  int dst_x_00;
  int src_block_x;
  int dst_y_00;
  int copy_h;
  unsigned int masked_block_count;
  unsigned int result;
  unsigned int modulus;
  unsigned int lcg_mask;

  blocks_x = (unsigned int)(width % tile_w != 0) + width / tile_w;
  blocks_y = (unsigned int)(height % tile_h != 0) + height / tile_h;
  bit_cursor = 0x40000000;
  modulus = 0;
  masked_block_count = (blocks_y + 1) * (blocks_x + 1);
  bit_count = 0;
  do
  {
    if ((bit_cursor & masked_block_count) != 0)
    {
      if (modulus == 0)
      {
        modulus = bit_cursor;
      }
      bit_count = bit_count + 1;
    }
    bit_cursor = (int)bit_cursor >> 1;
  } while (bit_cursor != 0);
  if (bit_count != 1)
  {
    modulus = modulus * 2;
  }

  lcg_mask = 0;
  bit_cursor = 1;
  if (1 < (int)modulus)
  {
    do
    {
      lcg_mask = lcg_mask | bit_cursor;
      bit_cursor = bit_cursor * 2;
    } while ((int)bit_cursor < (int)modulus);
  }

  bit_count = rand();
  bit_cursor = bit_count % (int)modulus;
  result = bit_count / (int)modulus;

  while (masked_block_count != 0)
  {
    bit_cursor = (bit_cursor * 0x21 + 1) & lcg_mask;
    result = bit_cursor;
    if ((int)bit_cursor <= blocks_y * blocks_x)
    {
      block_y = (int)bit_cursor / blocks_x;
      src_block_x = (int)bit_cursor % blocks_x;
      dst_x_00 = src_block_x * tile_w + dst_x;
      dst_y_00 = block_y * tile_h + dst_y;
      bit_count = tile_w;
      if (width + dst_x <= tile_w + dst_x_00)
      {
        bit_count = (dst_x - dst_x_00) + width;
      }
      copy_h = tile_h;
      if (height + dst_y <= tile_h + dst_y_00)
      {
        copy_h = (dst_y - dst_y_00) + height;
      }
      if ((tile_w == 1) && (tile_h == 1))
      {
        color = GetPixel(src, src_block_x, block_y);
        result = SetPixelV(dst, src_block_x, block_y, color);
      }
      else
      {
        result = BitBlt(dst, dst_x_00, dst_y_00, bit_count, copy_h, src, dst_x_00, dst_y_00, 0xcc0020);
      }
      masked_block_count = masked_block_count - 1;
    }
  }

  return result;
}

// FUNCTION: SHANDALAR 0x0057d330
unsigned int BlitRectByRandomTileOrder(HDC dst_hdc, int dst_x, int dst_y, int width, int height, int block_w, int block_h,
                                       HDC src_hdc, int src_x, int src_y)
{
  int tiles_x;
  int tiles_y;
  unsigned int bit;
  int bit_count;
  int tile_x;
  int tile_y;
  COLORREF pixel;
  int draw_x;
  int draw_y;
  int copy_w;
  int copy_h;
  unsigned int tile_count;
  unsigned int lfsr_mask;
  unsigned int lfsr;
  unsigned int lfsr_limit;
  unsigned int result;

  tiles_x = width / block_w + (width % block_w != 0);
  tiles_y = height / block_h + (height % block_h != 0);
  bit_count = 0;
  lfsr_limit = 0;
  tile_count = (unsigned int)((tiles_y + 1) * (tiles_x + 1));
  bit = 0x40000000;
  do
  {
    if ((tile_count & bit) != 0)
    {
      if (lfsr_limit == 0)
      {
        lfsr_limit = bit;
      }
      bit_count = bit_count + 1;
    }
    bit = (unsigned int)((int)bit >> 1);
  } while (bit != 0);
  if (bit_count != 1)
  {
    lfsr_limit = lfsr_limit * 2;
  }

  lfsr_mask = 0;
  bit = 1;
  while (bit < lfsr_limit)
  {
    lfsr_mask = lfsr_mask | bit;
    bit = bit * 2;
  }

  bit_count = rand();
  lfsr = bit_count % lfsr_limit;
  result = bit_count / lfsr_limit;
  while (tile_count != 0)
  {
    lfsr = (lfsr * 0x21 + 1) & lfsr_mask;
    result = lfsr;
    if ((int)lfsr <= tiles_y * tiles_x)
    {
      tile_y = (int)lfsr / tiles_x;
      tile_x = (int)lfsr % tiles_x;
      draw_x = dst_x + tile_x * block_w;
      draw_y = dst_y + tile_y * block_h;

      copy_w = block_w;
      if (dst_x + width <= draw_x + block_w)
      {
        copy_w = dst_x + width - draw_x;
      }

      copy_h = block_h;
      if (dst_y + height <= draw_y + block_h)
      {
        copy_h = dst_y + height - draw_y;
      }

      if ((block_w == 1) && (block_h == 1))
      {
        pixel = GetPixel(src_hdc, tile_x, tile_y);
        result = SetPixelV(dst_hdc, tile_x, tile_y, pixel);
      }
      else
      {
        result = BitBlt(dst_hdc, draw_x, draw_y, copy_w, copy_h, src_hdc,
                        src_x + tile_x * block_w, src_y + tile_y * block_h, SRCCOPY);
      }
      tile_count = tile_count - 1;
    }
  }

  return result;
}
// FUNCTION: SHANDALAR 0x0057d500
void BlitRectByStaggeredRandomTileOrder(HDC dst_hdc, int x, int y, int w, int h, int strip_width, int active_strip_count,
                                       int tile_width, int tile_height, HDC src_hdc)
{
  int dst_block_y;
  int iVar1;
  int block_count_x;
  int total_block_count;
  void *state_storage;
  DWORD tick;
  int iVar3;
  int dst_block_x;
  COLORREF color;
  unsigned int bit_cursor;
  int src_block_x;
  int rand_value;
  int copy_w;
  int copy_h;
  unsigned int masked_block_count;
  unsigned int modulus;
  int *state;
  int state_index;
  int rng_list_index;
  int rng_step;
  int x_offset;
  int y_loop_counter;
  int dst_x;
  int iStack_1c;
  int y_00;
  int current_strip_index;
  int iStack_30;
  int *piStack_2c;
  int iStack_20;
  int iStack_c;

  current_strip_index = -active_strip_count + 1;
  block_count_x = (unsigned int)(strip_width % tile_width != 0) + strip_width / tile_width;
  bit_cursor = 0x40000000;
  total_block_count = ((unsigned int)(h % tile_height != 0) + h / tile_height) * block_count_x;
  iVar3 = (unsigned int)(w % strip_width != 0) + w / strip_width;
  modulus = 0;
  iVar1 = 0;
  do
  {
    if ((bit_cursor & (unsigned int)total_block_count) != 0)
    {
      if (modulus == 0)
      {
        modulus = bit_cursor;
      }
      iVar1 = iVar1 + 1;
    }
    bit_cursor = (int)bit_cursor >> 1;
  } while (bit_cursor != 0);
  if (iVar1 != 1)
  {
    modulus = modulus * 2;
  }

  state_storage = malloc(((active_strip_count + iVar3) * 4 + 0x14) * 5);
  tick = GetTickCount();
  rng_list_index = -active_strip_count + 2;
  *(int *)((int)state_storage + current_strip_index * 0x14 + active_strip_count * 0x14) = (int)tick % (int)modulus;
  piStack_2c = (int *)((int)state_storage + current_strip_index * 0x14 + active_strip_count * 0x14);
  piStack_2c[1] = 5;
  piStack_2c[2] = 1;
  piStack_2c[3] = modulus;
  piStack_2c[4] = modulus;

  if (rng_list_index < iVar3)
  {
    state = (int *)((int)state_storage + rng_list_index * 0x14 + active_strip_count * 0x14);
    do
    {
      *state = (state[-5] * 5 + 1) % (int)modulus;
      rng_step = rng_list_index % 0x14;
      rng_list_index = rng_list_index + 1;
      state[1] = g_ui_fx_lcg_multipliers[rng_step] * 4 + 1;
      state[2] = 1;
      state[3] = modulus;
      state[4] = modulus;
      state = state + 5;
    } while (rng_list_index < iVar3);
  }

  if (current_strip_index < iVar3)
  {
    x_offset = strip_width * current_strip_index;
    do
    {
      iStack_1c = active_strip_count;
      if (current_strip_index < active_strip_count + current_strip_index)
      {
        iStack_c = active_strip_count;
        do
        {
          iStack_30 = current_strip_index;
          iVar1 = current_strip_index + iStack_1c;
          if (iVar3 <= current_strip_index + iStack_1c)
          {
            iVar1 = iVar3;
          }
          if (current_strip_index < iVar1)
          {
            iStack_20 = x_offset;
            state = piStack_2c;
            do
            {
              rand_value = (state[1] * state[0] + state[2]) % state[3];
              state[0] = rand_value;
              state[4] = state[4] + -1;
              if (rand_value < total_block_count)
              {
                dst_block_y = rand_value / block_count_x;
                src_block_x = rand_value % block_count_x;
                dst_x = src_block_x * tile_width + x + iStack_20;
                dst_block_x = dst_block_y * tile_height + y;
                copy_w = tile_width;
                if (w + x <= tile_width + dst_x)
                {
                  copy_w = (w - dst_x) + x;
                }
                copy_h = tile_height;
                if (h + y <= tile_height + dst_block_x)
                {
                  copy_h = y + (h - dst_block_x);
                }
                if (-1 < iStack_30)
                {
                  if ((tile_width == 1) && (tile_height == 1))
                  {
                    color = GetPixel(src_hdc, src_block_x, dst_block_y);
                    SetPixel(dst_hdc, src_block_x, dst_block_y, color);
                  }
                  else
                  {
                    BitBlt(dst_hdc, dst_x, dst_block_x, copy_w, copy_h, src_hdc, dst_x, dst_block_x, 0xcc0020);
                  }
                }
              }
              iStack_20 = iStack_20 + strip_width;
              state = state + 5;
              iStack_30 = iStack_30 + 1;
            } while (iStack_30 < iVar1);
          }
          iStack_1c = iStack_1c + -1;
          iStack_c = iStack_c + -1;
        } while (iStack_c != 0);
      }
      if (piStack_2c[4] < 1)
      {
        piStack_2c = piStack_2c + 5;
        x_offset = x_offset + strip_width;
        current_strip_index = current_strip_index + 1;
      }
    } while (current_strip_index < iVar3);
  }

  free(state_storage);
}
#pragma optimize("", on)
