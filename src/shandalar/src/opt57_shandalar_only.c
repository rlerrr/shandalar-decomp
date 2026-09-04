// SHANDALAR-only optimized helpers from the 0x0057xxxx region.
//
// These live in SHANDALAR but not in FACEMAKER, even though SHANDALAR pulls in
// most of the facemaker sidlib sources. Keeping these here prevents FACEMAKER
// from accidentally linking in extra code.

#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include <string.h>

#include <windows.h>

#include "cardartlib/src/assert.h"
#include "drawcardlib/src/pic.h"
#include "facemaker/src/facemaker_types.h"
#include "shandalar_internal.h"

#undef _fileno

typedef struct RGBLike
{
  int r;
  int g;
  int b;
  int a_or_unused;
} RGBLike;

typedef struct HSVLike
{
  int hue;
  int saturation;
  int value;
} HSVLike;

extern void RpBits_ApplyPalette(RpBitsPalettePacket *palette_data);
extern void ConvertRgbToHsv(HSVLike *hsv, RGBLike *rgb);
extern RGBLike *ConvertHsvToRgb(HSVLike *hsv, RGBLike *rgb);
extern void CopyBytesAsmCompat(double *dst, double *src, unsigned int size);

extern RGBQUAD g_palette_rgb[256];
extern unsigned char *g_palette_rgb_bytes;

extern BITMAPINFO *g_copy_bitmap_info;
extern int g_copy_bitmap_info_initialized;
extern char *g_copy_flip_scratch_buffer_ptr;
extern char *g_copy_restore_scratch_buffer_ptr;

extern RpBitsPalettePacket g_palette_transition_source_words;
extern int g_palette_transition_work_words[0x400];
extern int g_palette_transition_hsv[0x301];
extern int g_palette_transition_value_step[0x2ff];

// GLOBAL: SHANDALAR 0x005a1868
unsigned char* g_palette_transition_source_bytes = g_palette_transition_source_words.entry_data;

#pragma optimize("gy", on)
#pragma intrinsic(memset)
#pragma intrinsic(memcpy)

// FUNCTION: SHANDALAR 0x00579bf0
void CopyGraphicsRect(FacemakerWindowBounds *src, int src_x, int src_y, unsigned int width, int height,
                      FacemakerWindowBounds *dst, int dst_x, int dst_y)
{
  DIBSurface *src_page;
  DIBSurface *dst_page;
  double *top_row;
  double *bottom_row;
  int half_height;
  int i;

  src_page = g_graphics_pages[src->page_number];
  dst_page = g_graphics_pages[dst->page_number];

  if (g_copy_bitmap_info_initialized == 0)
  {
    g_copy_bitmap_info = CreateBitmapInfo(1, 1, 8);
    g_copy_bitmap_info_initialized = 1;
  }

  g_copy_bitmap_info->bmiHeader.biWidth = src_page->width;
  if (src_y == 0)
  {
    g_copy_bitmap_info->bmiHeader.biHeight = src_page->height;
  }
  else
  {
    g_copy_bitmap_info->bmiHeader.biHeight = src_page->height;
  }

  if ((dst->page_number == 0) && ((src_x & 7) == 0) && (g_frontbuffer_direct_blit_enabled != 0))
  {
    if (g_graphics_bpp != 8)
    {
      memcpy(g_copy_bitmap_info->bmiColors, g_palette_rgb, 0x400);
    }

    top_row = (double *)((int)src_page->pBits + src_x + src_page->width * src_y);
    bottom_row = (double *)((int)src_page->pBits + src_x + (src_y + height - 1) * src_page->width);
    half_height = height / 2;
    for (i = half_height; i > 0; i--)
    {
      CopyBytesAsmCompat((double *)g_copy_flip_scratch_buffer_ptr, top_row, width);
      CopyBytesAsmCompat(top_row, bottom_row, width);
      CopyBytesAsmCompat(bottom_row, (double *)g_copy_flip_scratch_buffer_ptr, width);
      top_row = (double *)((int)top_row + src_page->width);
      bottom_row = (double *)((int)bottom_row - src_page->width);
    }

    SetDIBitsToDevice(dst_page->hTempDC, dst_x, dst_y, width, height, src_x, src_y, 0, src_page->height,
                      src_page->pBits, g_copy_bitmap_info, (unsigned int)(g_graphics_bpp == 8));

    top_row = (double *)((int)src_page->pBits + src_x + src_page->width * src_y);
    bottom_row = (double *)((int)src_page->pBits + src_x + (src_y + height - 1) * src_page->width);
    for (i = half_height; i > 0; i--)
    {
      CopyBytesAsmCompat((double *)g_copy_restore_scratch_buffer_ptr, top_row, width);
      CopyBytesAsmCompat(top_row, bottom_row, width);
      CopyBytesAsmCompat(bottom_row, (double *)g_copy_restore_scratch_buffer_ptr, width);
      top_row = (double *)((int)top_row + src_page->width);
      bottom_row = (double *)((int)bottom_row - src_page->width);
    }
    return;
  }

  BitBlt(dst_page->hTempDC, dst_x, dst_y, width, height, src_page->hTempDC, src_x, src_y, SRCCOPY);
}

// FUNCTION: SHANDALAR 0x0057b7a0
int ReadSpriteEntryPointersWithLimit(EncodedImage **out_entry_ptrs, char *sprite_path, int max_entries)
{
  FILE *_File;
  int iVar1;
  int _Size;
  int *_DstBuf;
  int iVar2;

  iVar2 = 0;
  _File = fopen(sprite_path, "rb");
  assert((int)_File, "D:\\NewMagic\\sources\\sidlib\\sprite.c", 0xc5,
         "Could not open Sprite File %s\n", sprite_path);
  iVar1 = _fileno(_File);
  _Size = _filelength(iVar1);
  _DstBuf = malloc(_Size);
  fread(_DstBuf, 1, _Size, _File);
  fclose(_File);

  iVar1 = *_DstBuf;
  if (iVar1 != -1)
  {
    do
    {
      if (max_entries <= iVar2)
      {
        break;
      }
      *out_entry_ptrs = (EncodedImage *)_DstBuf;
      out_entry_ptrs = out_entry_ptrs + 1;
      iVar2 = iVar2 + 1;
      _DstBuf = (int *)((int)_DstBuf + *_DstBuf);
      iVar1 = *_DstBuf;
    } while (iVar1 != -1);
  }
  return iVar2;
}

// FUNCTION: SHANDALAR 0x0057bd10
void DrawEncodedImageUnscaledClipped(FacemakerWindowBounds *dst, int x, int y, EncodedImage *encoded_image)
{
  int base_y;
  int page_number;
  DIBSurface *page;
  int draw_y;
  unsigned int span_offset;
  unsigned int clipped_span_length;
  int row_stride;
  unsigned int span_length;
  unsigned char *next_span;
  unsigned char *span_ptr;
  int row_count;
  int draw_start_x;
  int skip_bytes;
  int row_index;
  int dst_row_ptr;

  page_number = dst->page_number;
  if (((encoded_image != (EncodedImage *)0) && (x <= dst->max_x)) && (y <= dst->max_y))
  {
    page = g_graphics_pages[page_number];
    if (((dst->clip_left <= x) && (x + (int)encoded_image->width <= dst->max_x)) &&
        ((dst->clip_top <= y) && ((int)encoded_image->height + y <= dst->max_y)))
    {
      DrawEncodedImageUnscaled(dst, x, y, encoded_image);
      return;
    }
    base_y = y + (int)encoded_image->first_row;
    row_count = (int)encoded_image->row_count;
    if (((x <= dst->max_x) && (dst->clip_left <= (int)encoded_image->width + x)) &&
        ((dst->clip_top <= row_count + base_y) && (base_y <= dst->max_y)))
    {
      row_stride = page->rowPadding + page->width;
      row_index = 0;
      dst_row_ptr = (int)page->pBits + x + base_y * row_stride;
      span_ptr = encoded_image->spans;
      if (0 < row_count)
      {
        do
        {
          draw_y = row_index + base_y;
          if (dst->max_y <= draw_y)
          {
            return;
          }
          if (draw_y < dst->clip_top)
          {
            if (*span_ptr == 0xff)
            {
              next_span = span_ptr + 1;
            }
            else if (span_ptr[1] == 0xfe)
            {
              skip_bytes = (int)span_ptr[2] + 3;
next_span_ptr:
              next_span = span_ptr + skip_bytes;
            }
            else
            {
              next_span = span_ptr + span_ptr[1] + 2;
            }
          }
          else
          {
            span_offset = (unsigned int)*span_ptr;
            next_span = span_ptr + 1;
            if (span_offset != 0xff)
            {
              span_length = (unsigned int)*next_span;
              next_span = span_ptr + 2;
              if (span_length == 0xfe)
              {
                span_length = (unsigned int)span_ptr[2];
                next_span = span_ptr + 3;
              }
              span_ptr = next_span;
              draw_start_x = dst->max_x;
              if ((int)(x + span_offset) <= draw_start_x)
              {
                if (draw_start_x < (int)(x + span_length + span_offset))
                {
                  clipped_span_length = (unsigned int)((draw_start_x - (int)span_offset) - x);
                  skip_bytes = (int)(span_length - clipped_span_length);
                  span_length = clipped_span_length;
                }
                else
                {
                  skip_bytes = 0;
                }
                draw_start_x = dst->clip_left;
                if ((int)(x + span_offset) < draw_start_x)
                {
                  if ((int)(x + span_length + span_offset) < draw_start_x)
                  {
                    skip_bytes = skip_bytes + (int)span_length;
                    goto next_span_ptr;
                  }
                  draw_start_x = (draw_start_x - (int)span_offset) - x;
                  span_ptr = span_ptr + draw_start_x;
                }
                else
                {
                  draw_start_x = 0;
                }
                if (page_number == 0)
                {
                  for (; draw_start_x < (int)span_length; draw_start_x = draw_start_x + 1)
                  {
                    if (*span_ptr != 0)
                    {
                      PutGraphicsPixel(dst, x + draw_start_x + (int)span_offset, draw_y, (unsigned int)*span_ptr);
                    }
                    span_ptr = span_ptr + 1;
                  }
                }
                else
                {
                  for (; draw_start_x < (int)span_length; draw_start_x = draw_start_x + 1)
                  {
                    if (*span_ptr != 0)
                    {
                      *(unsigned char *)(dst_row_ptr + draw_start_x + (int)span_offset) = *span_ptr;
                    }
                    span_ptr = span_ptr + 1;
                  }
                }
                goto next_span_ptr;
              }
              next_span = span_ptr + span_length;
            }
          }
          row_index = row_index + 1;
          dst_row_ptr = dst_row_ptr + row_stride;
          span_ptr = next_span;
        } while (row_index < row_count);
      }
    }
  }
}

// FUNCTION: SHANDALAR 0x0057ce70
int FadeInPaletteFromGray(int gray, int steps)
{
  int value_step;
  RGBLike target_rgb;
  HSVLike target_hsv;
  RGBLike rgb;
  HSVLike hsv;
  int color_index;
  int entry_offset;
  int work_offset;
  int saturation_step;
  short frames_left;
  unsigned char *dst;
  unsigned char *src;
  int copy_count;
  unsigned short gray_word;

  value_step = 0x3fc0 / steps;

  if (g_graphics_bpp != 8)
  {
    return 0;
  }

  target_rgb.r = gray;
  target_rgb.g = gray;
  target_rgb.b = gray;

  src = g_palette_rgb_bytes;
  dst = g_palette_transition_source_bytes;
  copy_count = 0x2ff;
  do
  {
    *dst = *src;
    src = src + 1;
    dst = dst + 1;
  } while (copy_count-- != 0);

  gray_word = (unsigned short)((unsigned char)gray | ((unsigned short)(unsigned char)gray << 8));
  for (color_index = 0; color_index < 0x100; color_index = color_index + 1)
  {
    entry_offset = color_index * 3;
    *(unsigned short *)(g_palette_rgb_bytes + entry_offset) = gray_word;
    g_palette_rgb_bytes[entry_offset + 2] = (unsigned char)gray;
  }

  RpBits_ApplyPalette(&g_palette_data_words);
  ConvertRgbToHsv(&target_hsv, &target_rgb);

  for (color_index = 0; color_index < 0x100; color_index = color_index + 1)
  {
    entry_offset = color_index * 3;

    rgb.r = (unsigned char)g_palette_transition_source_bytes[entry_offset + 0];
    rgb.g = (unsigned char)g_palette_transition_source_bytes[entry_offset + 1];
    rgb.b = (unsigned char)g_palette_transition_source_bytes[entry_offset + 2];
    ConvertRgbToHsv(&hsv, &rgb);

    g_palette_transition_hsv[entry_offset + 0] = hsv.hue;
    g_palette_transition_hsv[entry_offset + 1] = hsv.saturation;
    g_palette_transition_hsv[entry_offset + 2] = hsv.value;

    saturation_step = (target_hsv.saturation - hsv.saturation) / steps;
    g_palette_transition_value_step[color_index] = saturation_step;

    if (hsv.saturation < target_hsv.saturation)
    {
      saturation_step = 0x1000 / steps;
    }
    else
    {
      saturation_step = -0x1000 / steps;
    }

    g_palette_transition_value_step[color_index] = saturation_step;
  }

  frames_left = (short)steps;
  if (0 < frames_left)
  {
    do
    {
      for (color_index = 0; color_index < 0x100; color_index = color_index + 1)
      {
        entry_offset = color_index * 3;

        hsv.hue = g_palette_transition_hsv[entry_offset + 0];
        if (target_hsv.value == 0)
        {
          hsv.saturation = g_palette_transition_hsv[entry_offset + 1];
          hsv.value = g_palette_transition_hsv[entry_offset + 2] - frames_left * value_step;
          if (hsv.value < 1)
          {
            hsv.value = 0;
          }
        }
        else
        {
          hsv.saturation =
              g_palette_transition_value_step[color_index] * (int)frames_left +
              g_palette_transition_hsv[entry_offset + 1];
          if (hsv.saturation > 0xfff)
          {
            hsv.saturation = 0x1000;
          }
          if (hsv.saturation < 1)
          {
            hsv.saturation = 0;
          }

          hsv.value = value_step * (int)frames_left + g_palette_transition_hsv[entry_offset + 2];
          if (hsv.value > 0x3fbf)
          {
            hsv.value = 0x3fc0;
          }
        }

        (void)ConvertHsvToRgb(&hsv, &rgb);

        work_offset = color_index * 4;
        g_palette_transition_work_words[work_offset + 0] = rgb.r;
        g_palette_transition_work_words[work_offset + 1] = rgb.g;
        g_palette_transition_work_words[work_offset + 2] = rgb.b;
        g_palette_transition_work_words[work_offset + 3] = rgb.a_or_unused;

        if (g_palette_transition_work_words[work_offset + 0] > 0xfe)
        {
          g_palette_rgb_bytes[entry_offset + 0] = 0xff;
        }
        else
        {
          g_palette_rgb_bytes[entry_offset + 0] =
              (unsigned char)g_palette_transition_work_words[work_offset + 0];
        }

        if (g_palette_transition_work_words[work_offset + 1] > 0xfe)
        {
          g_palette_rgb_bytes[entry_offset + 1] = 0xff;
        }
        else
        {
          g_palette_rgb_bytes[entry_offset + 1] =
              (unsigned char)g_palette_transition_work_words[work_offset + 1];
        }

        if (g_palette_transition_work_words[work_offset + 2] > 0xfe)
        {
          g_palette_rgb_bytes[entry_offset + 2] = 0xff;
        }
        else
        {
          g_palette_rgb_bytes[entry_offset + 2] =
              (unsigned char)g_palette_transition_work_words[work_offset + 2];
        }
      }

      RpBits_ApplyPalette(&g_palette_data_words);
      frames_left = frames_left - 1;
    } while (frames_left != 0);
  }

  src = g_palette_transition_source_bytes;
  dst = g_palette_rgb_bytes;
  copy_count = 0x2ff;
  do
  {
    *dst = *src;
    src = src + 1;
    dst = dst + 1;
  } while (copy_count-- != 0);
  RpBits_ApplyPalette(&g_palette_data_words);
  return 0;
}

#pragma optimize("", on)
