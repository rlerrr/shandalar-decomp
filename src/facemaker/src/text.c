#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include "cardartlib/src/assert.h"
#include "drawcardlib/src/pic.h"
#include "facemaker_types.h"

extern int __cdecl FUN_00407c20(int font_id, FILE *file);
extern int __cdecl FUN_00407f30(int font_id, unsigned int style);
extern int __cdecl FUN_004086c0(int *dst, int text_id, int a3, int a4, int a5, int a6, int x, int y, int *arg9);
extern void __cdecl FUN_00407190(int *param_1, int param_2, int param_3, unsigned int param_4);
extern void __cdecl FUN_004075d0(unsigned int *param_1, int param_2, int param_3, int param_4, unsigned int param_5);
extern char s_D__NewMagic__sources__sidlib__lib_c_0040d0ec[];
extern DIBSurface *DAT_00426540[10];
extern PALETTEENTRY DAT_00426140[256];
extern RGBQUAD DAT_00426580[256];
extern int DAT_00426570;
extern HDC global_main_hdc;

// GLOBAL: FACEMAKER 0x004199c8
int *DAT_004199c8;

// GLOBAL: FACEMAKER 0x004199cc
int *DAT_004199cc;

// GLOBAL: FACEMAKER 0x004189b8
int DAT_004189b8;

// GLOBAL: FACEMAKER 0x004189bc
int DAT_004189bc;

// GLOBAL: FACEMAKER 0x004189c0
int DAT_004189c0;

// GLOBAL: FACEMAKER 0x004189c8
int DAT_004189c8[0x400];

// GLOBAL: FACEMAKER 0x004199d0
int DAT_004199d0;

// GLOBAL: FACEMAKER 0x004199d4
int DAT_004199d4;

// GLOBAL: FACEMAKER 0x004199d8
int DAT_004199d8;

// GLOBAL: FACEMAKER 0x004199dc
int DAT_004199dc;

// GLOBAL: FACEMAKER 0x004199e0
int DAT_004199e0;

// GLOBAL: FACEMAKER 0x004199e4
int DAT_004199e4;

// GLOBAL: FACEMAKER 0x004199e8
int DAT_004199e8[0x400];

// GLOBAL: FACEMAKER 0x0041a9e8
int DAT_0041a9e8;

// GLOBAL: FACEMAKER 0x00423344
int DAT_00423344;

// GLOBAL: FACEMAKER 0x004233d0
FontSlot DAT_004233d0[0x10];

// GLOBAL: FACEMAKER 0x0040d224
char DAT_0040d224[] = "rb";

// GLOBAL: FACEMAKER 0x0040d228
char s_D__NewMagic__sources__sidlib__text_c_0040d228[] = "D:\\NewMagic\\sources\\sidlib\\text.c";

// GLOBAL: FACEMAKER 0x0040d200
char s_Can_not_load_more_than__d_fonts_0040d200[] = "Can not load more than %d fonts\n";

// GLOBAL: FACEMAKER 0x0040c0b4
char s_File__s_could_not_be_opened__EXITING_0040c0b4[] = "File %s could not be opened: EXITING\r\n";

// GLOBAL: FACEMAKER 0x0040c0b0
char *PTR_s_File__s_could_not_be_opened__EXI_0040c0b0 = s_File__s_could_not_be_opened__EXITING_0040c0b4;

// FUNCTION: FACEMAKER 0x00407b10
#pragma optimize("gty", on)
int __cdecl FUN_00407b10(char *path)
{
  FILE *font_file;
  long font_offsets[16];
  int font_count;
  int font_index;

  font_count = 0;
  font_file = fopen(path, DAT_0040d224);
  assert((unsigned int)(font_file != (FILE *)0), s_D__NewMagic__sources__sidlib__text_c_0040d228, 0x83,
         PTR_s_File__s_could_not_be_opened__EXI_0040c0b0, path);
  fread(&font_count, 2, 1, font_file);
  DAT_00423344 = font_count;
  font_index = 1;
  assert((unsigned int)(font_count < 0x10), s_D__NewMagic__sources__sidlib__text_c_0040d228, 0x8a,
         s_Can_not_load_more_than__d_fonts_0040d200, 0x10);
  if (font_count > 0)
  {
    do
    {
      font_offsets[font_index] = 0;
      fread(&font_offsets[font_index], 2, 1, font_file);
      font_index = font_index + 1;
    } while (font_index <= font_count);
  }

  font_index = 1;
  if (font_count > 0)
  {
    do
    {
      fseek(font_file, font_offsets[font_index], 0);
      FUN_00407c20(font_index, font_file);
      font_index = font_index + 1;
    } while (font_index <= font_count);
  }

  fclose(font_file);
  return font_count;
}

// FUNCTION: FACEMAKER 0x00407c20
int __cdecl FUN_00407c20(int font_id, FILE *file)
{
  int row_bytes;
  int bitmap_size;
  int remaining;
  unsigned int glyph_count;
  unsigned int height;
  unsigned int row_padding;
  unsigned int rows_remaining;
  FontSlot *font;
  unsigned char *bitmap_data;
  HBITMAP bitmap;

  font = &DAT_004233d0[font_id];
  fseek(file, -8, 1);
  fread(font, 1, 8, file);
  glyph_count = ((unsigned int)font->last_char - (unsigned int)font->first_char) + 1;
  if (font->has_packed_widths == '\0')
  {
    fseek(file, -8 - glyph_count, 1);
    fread(font->data.bitmap.glyph_advance + (unsigned int)font->first_char, 1, glyph_count, file);
    fseek(file, 8, 1);
  }

  height = (unsigned int)font->point_size;
  row_bytes = (unsigned int)font->glyph_width * glyph_count;
  row_padding = row_bytes & 1;
  font->row_padding = (unsigned char)row_padding;
  if (font->bitmap_data != (unsigned char *)0)
  {
    free(font->bitmap_data);
  }
  if (font->bitmap_normal != (HGDIOBJ)0)
  {
    DeleteObject(font->bitmap_normal);
  }
  if (font->bitmap_inverted != (HGDIOBJ)0)
  {
    DeleteObject(font->bitmap_inverted);
  }

  bitmap_size = (row_padding + row_bytes) * height;
  bitmap_data = malloc(bitmap_size + 0x2710);
  font->bitmap_data = bitmap_data;
  rows_remaining = height;
  while (rows_remaining != 0)
  {
    fread(bitmap_data, 1, row_bytes, file);
    bitmap_data = bitmap_data + row_padding + row_bytes;
    rows_remaining = rows_remaining - 1;
  }

  bitmap = CreateBitmap(row_bytes * 8, height, 1, 1, font->bitmap_data);
  font->bitmap_normal = bitmap;
  bitmap_data = font->bitmap_data;
  remaining = bitmap_size;
  while (remaining > 0)
  {
    *bitmap_data = (unsigned char)~*bitmap_data;
    bitmap_data = bitmap_data + 1;
    remaining = remaining - 1;
  }

  bitmap = CreateBitmap(row_bytes * 8, height, 1, 1, font->bitmap_data);
  font->bitmap_inverted = bitmap;
  bitmap_data = font->bitmap_data;
  while (bitmap_size > 0)
  {
    *bitmap_data = (unsigned char)~*bitmap_data;
    bitmap_data = bitmap_data + 1;
    bitmap_size = bitmap_size - 1;
  }

  if (font->hdc == (HDC)0)
  {
    font->hdc = CreateCompatibleDC((HDC)0);
  }
  font->font_loaded = 0;
  return 0;
}

// FUNCTION: FACEMAKER 0x00407e20
int __cdecl FUN_00407e20(int font_id, unsigned int point_size, char *font_file, char *font_name,
                         int weight, DWORD italic)
{
  char c;
  FontSlot *font;
  HFONT font_handle;
  HDC hdc;
  unsigned int ulen;
  unsigned int block_count;
  char *src;
  char *dst;
  unsigned int dword_count;
  TEXTMETRICA text_metrics;

  AddFontResourceA(font_file);
  font = &DAT_004233d0[font_id];
  font_handle = CreateFontA(100, 0, 0, 0, weight, italic, 0, 0, 0, 0, 0, 0, 0, font_name);
  font->hfont = font_handle;

  ulen = 0xffffffff;
  do
  {
    src = font_file;
    if (ulen == 0)
    {
      break;
    }
    ulen--;
    src = font_file + 1;
    c = *font_file;
    font_file = src;
  } while (c != '\0');
  ulen = ~ulen;
  src = src - ulen;
  dst = font->data.gdi.font_file;
  for (block_count = ulen >> 2; block_count != 0; block_count--)
  {
    *(unsigned int *)dst = *(unsigned int *)src;
    src += 4;
    dst += 4;
  }
  for (dword_count = ulen & 3; dword_count != 0; dword_count--)
  {
    *dst = *src;
    src++;
    dst++;
  }

  ulen = 0xffffffff;
  do
  {
    src = font_name;
    if (ulen == 0)
    {
      break;
    }
    ulen--;
    src = font_name + 1;
    c = *font_name;
    font_name = src;
  } while (c != '\0');
  ulen = ~ulen;
  src = src - ulen;
  dst = font->data.gdi.font_name;
  for (block_count = ulen >> 2; block_count != 0; block_count--)
  {
    *(unsigned int *)dst = *(unsigned int *)src;
    src += 4;
    dst += 4;
  }
  for (dword_count = ulen & 3; dword_count != 0; dword_count--)
  {
    *dst = *src;
    src++;
    dst++;
  }

  font->font_loaded = 1;

  hdc = GetDC((HWND)0);
  SelectObject(hdc, font->hfont);
  GetTextMetricsA(hdc, &text_metrics);
  ReleaseDC((HWND)0, hdc);
  font->tm_max = text_metrics.tmAscent;
  font->tm_min = text_metrics.tmInternalLeading;

  FUN_00407f30(font_id, point_size);
  return 1;
}

// FUNCTION: FACEMAKER 0x00407f30
int __cdecl FUN_00407f30(int font_id, unsigned int style)
{
  FontSlot *font;
  HFONT current_font;
  HFONT new_font;
  HDC hdc;
  int(__stdcall *get_object_fn)(HANDLE, int, LPVOID);
  HDC(__stdcall *get_dc_fn)(HWND);
  TEXTMETRICA text_metrics;
  LOGFONTA log_font;

  font = &DAT_004233d0[font_id];
  if (font->font_loaded == 0)
  {
    return 0;
  }
  if ((unsigned int)font->point_size == style)
  {
    return (int)font->hfont;
  }

  current_font = font->hfont;
  get_object_fn = GetObjectA;
  get_object_fn(current_font, 0x3c, &log_font);
  DeleteObject(current_font);
  font->point_size = (unsigned char)style;
  log_font.lfHeight = (int)(style * 100) / (font->tm_max - font->tm_min);
  new_font = CreateFontIndirectA(&log_font);
  font->hfont = new_font;
  get_dc_fn = GetDC;
  hdc = get_dc_fn((HWND)0);
  SelectObject(hdc, font->hfont);
  GetTextMetricsA(hdc, &text_metrics);
  ReleaseDC((HWND)0, hdc);
  font->tm_leading = text_metrics.tmInternalLeading + text_metrics.tmExternalLeading;
  return (int)font->hfont;
}

// FUNCTION: FACEMAKER 0x00408da0
void __cdecl FUN_00408da0(int *param_1, int param_2, int param_3, int param_4)
{
  short sVar1;
  int iVar2;
  int iVar3;
  unsigned int uVar4;
  unsigned int uVar5;
  unsigned int uVar6;
  int iVar7;
  unsigned char *pbVar8;
  unsigned char *pbVar9;
  int iVar10;
  int iVar11;
  unsigned char *pbVar12;
  int bVar13;
  int local_10;
  int local_c;

  iVar2 = *param_1;
  if (param_4 != 0)
  {
    iVar11 = (int)DAT_00426540[iVar2];
    sVar1 = *(short *)(param_4 + 0xe);
    iVar7 = param_3 + *(short *)(param_4 + 0xc);
    iVar3 = *(int *)(iVar11 + 0x2c) + *(int *)(iVar11 + 0x20);
    local_c = 0;
    local_10 = param_2 + iVar3 * iVar7 + *(int *)(iVar11 + 0x18);
    pbVar8 = (unsigned char *)(param_4 + 0x10);
    if (0 < sVar1)
    {
      do
      {
        uVar4 = (unsigned int)*pbVar8;
        pbVar9 = pbVar8 + 1;
        if (uVar4 != 0xff)
        {
          uVar6 = (unsigned int)*pbVar9;
          pbVar9 = pbVar8 + 2;
          bVar13 = uVar6 != 0xfe;
          if (!bVar13)
          {
            uVar6 = (unsigned int)*pbVar9;
            pbVar9 = pbVar8 + 3;
          }
          iVar11 = iVar7 + local_c;
          if (iVar11 < 0)
          {
            pbVar9 = pbVar9 + uVar6;
          }
          else if (bVar13)
          {
            if (iVar2 == 0)
            {
              iVar10 = 0;
              if (uVar6 != 0)
              {
                do
                {
                  if (*pbVar9 != 0)
                  {
                    FUN_00407190(param_1, param_2 + uVar4 + iVar10, iVar11, (unsigned int)*pbVar9);
                  }
                  iVar10 = iVar10 + 1;
                  pbVar9 = pbVar9 + 1;
                } while (iVar10 < (int)uVar6);
              }
            }
            else
            {
              iVar11 = 0;
              if (uVar6 != 0)
              {
                do
                {
                  if (*pbVar9 != 0)
                  {
                    *(unsigned char *)(uVar4 + iVar11 + local_10) = *pbVar9;
                  }
                  iVar11 = iVar11 + 1;
                  pbVar9 = pbVar9 + 1;
                } while (iVar11 < (int)uVar6);
              }
            }
          }
          else if (iVar2 == 0)
          {
            FUN_004075d0((unsigned int *)pbVar9, 0, param_2 + uVar4, iVar11, uVar6);
            pbVar9 = pbVar9 + uVar6;
          }
          else
          {
            pbVar8 = pbVar9;
            pbVar12 = (unsigned char *)(local_10 + uVar4);
            for (uVar5 = uVar6 >> 2; uVar5 != 0; uVar5 = uVar5 - 1)
            {
              *(unsigned int *)pbVar12 = *(unsigned int *)pbVar8;
              pbVar8 = pbVar8 + 4;
              pbVar12 = pbVar12 + 4;
            }
            pbVar9 = pbVar9 + uVar6;
            for (uVar6 = uVar6 & 3; uVar6 != 0; uVar6 = uVar6 - 1)
            {
              *pbVar12 = *pbVar8;
              pbVar8 = pbVar8 + 1;
              pbVar12 = pbVar12 + 1;
            }
          }
        }
        local_c = local_c + 1;
        local_10 = local_10 + iVar3;
        pbVar8 = pbVar9;
      } while (local_c < sVar1);
    }
  }
}

// FUNCTION: FACEMAKER 0x00408f20
void __cdecl FUN_00408f20(int *dst, int x, int y, int width, int height, int src_page)
{
  short row_count;
  int page_number;
  int source_width;
  int source_height;
  int i;
  int j;
  int k;
  int row_stride;
  int row_base;
  int row_index;
  int top_clip;
  int run_start;
  int run_end;
  int source_row;
  int next_source_row;
  int row_delta;
  int draw_y;
  int mapping_index;
  int run_offset;
  unsigned int run_x;
  unsigned int run_length;
  unsigned int segment_length;
  unsigned char *row_data;
  unsigned char *next_row_data;
  unsigned char tmp_line[0x400];
  DIBSurface *surface;
  int *lookup_ptr;
  int use_raw_copy;

  page_number = *dst;
  if (src_page != 0 && x <= dst[3] && y <= dst[4])
  {
    surface = DAT_00426540[page_number];
    source_width = (int)*(short *)(src_page + 4);
    source_height = (int)*(short *)(src_page + 6);
    if (((DAT_004199e4 != width) || (height != DAT_004199dc)) ||
        (source_width != DAT_0041a9e8 || source_height != DAT_004199d0))
    {
      DAT_004189b8 = (source_width << 0x10) / width;
      DAT_004189bc = (source_height << 0x10) / height;
      for (i = 0; i < 0x400; i = i + 1)
      {
        DAT_004189c8[i] = -1;
      }
      i = 0;
      DAT_004199d4 = 0;
      if (-1 < width + 2)
      {
        do
        {
          j = DAT_004199d4 >> 0x10;
          DAT_004199e8[i] = j;
          if (DAT_004189c8[j] == -1)
          {
            DAT_004189c8[j] = i;
          }
          i = i + 1;
          DAT_004199d4 = DAT_004199d4 + DAT_004189b8;
        } while (i <= width + 2);
      }
      if (width < source_width && -1 < source_width)
      {
        lookup_ptr = DAT_004189c8;
        i = source_width + 1;
        do
        {
          if (*lookup_ptr == -1)
          {
            *lookup_ptr = lookup_ptr[-1];
          }
          lookup_ptr = lookup_ptr + 1;
          i = i - 1;
        } while (i != 0);
      }
      DAT_004199e4 = width;
      DAT_004199dc = height;
      DAT_004199d0 = source_height;
      DAT_0041a9e8 = source_width;
    }
    if (x < dst[1])
    {
      DAT_004199e0 = dst[1] - x;
    }
    else
    {
      DAT_004199e0 = 0;
    }
    if (dst[3] < x + width)
    {
      DAT_004189c0 = dst[3] - x;
    }
    else
    {
      DAT_004189c0 = width;
    }

    top_clip = (int)*(short *)(src_page + 0xc);
    DAT_004199d8 = 0;
    if (0 < top_clip)
    {
      do
      {
        DAT_004199d8 = DAT_004199d8 + DAT_004189bc;
        y = y + 1;
      } while (DAT_004199d8 >> 0x10 < top_clip);
    }

    row_data = (unsigned char *)(src_page + 0x10);
    row_count = *(short *)(src_page + 0xe);
    row_stride = surface->width + surface->rowPadding;
    row_index = 0;
    row_base = (int)surface->pBits + y * row_stride + x;
    if ((DAT_004199d8 >> 0x10) < row_count + top_clip)
    {
      do
      {
        source_row = DAT_004199d8 >> 0x10;
        next_row_data = row_data + 1;
        next_source_row = (DAT_004189bc + DAT_004199d8) >> 0x10;
        run_x = (unsigned int)*row_data;
        if (run_x != 0xff)
        {
          segment_length = (unsigned int)*next_row_data;
          next_row_data = row_data + 2;
          use_raw_copy = segment_length == 0xfe;
          if (use_raw_copy)
          {
            next_row_data = row_data + 3;
            segment_length = (unsigned int)row_data[2];
          }
          draw_y = row_index + y;
          if (dst[2] <= draw_y)
          {
            if (dst[4] < draw_y)
            {
              return;
            }
            run_start = DAT_004189c8[run_x];
            if (run_start <= DAT_004199e0)
            {
              run_start = DAT_004199e0;
            }
            run_end = DAT_004189c8[segment_length + run_x];
            if (DAT_004189c0 <= DAT_004189c8[segment_length + run_x])
            {
              run_end = DAT_004189c0;
            }
            if (DAT_004199e8[run_start] - (int)run_x < 0)
            {
              run_start = run_start + 1;
            }
            if (page_number == 0)
            {
              k = run_start;
              if (use_raw_copy)
              {
                while (k < run_end)
                {
                  tmp_line[k] = next_row_data[DAT_004199e8[k] - run_x];
                  k = k + 1;
                }
                FUN_004075d0((unsigned int *)(tmp_line + run_start), 0, x + run_start, draw_y,
                             run_end - run_start);
              }
              else
              {
                while (run_start < run_end)
                {
                  mapping_index = DAT_004199e8[run_start] - run_x;
                  if (next_row_data[mapping_index] != 0)
                  {
                    FUN_00407190(dst, x + run_start, draw_y, (unsigned int)next_row_data[mapping_index]);
                  }
                  run_start = run_start + 1;
                }
              }
            }
            else
            {
              while (run_start < run_end)
              {
                mapping_index = DAT_004199e8[run_start] - run_x;
                if (next_row_data[mapping_index] != 0)
                {
                  *(unsigned char *)(row_base + run_start) = next_row_data[mapping_index];
                }
                run_start = run_start + 1;
              }
            }
          }
          next_row_data = next_row_data + segment_length;
        }

        if (next_source_row != source_row)
        {
          row_delta = next_source_row - source_row;
          row_data = next_row_data;
          if (row_delta != 1)
          {
            run_offset = row_delta - 2;
            if (row_delta != 1)
            {
              do
              {
                row_data = next_row_data + 1;
                if (*next_row_data != 0xff)
                {
                  run_length = (unsigned int)*row_data;
                  row_data = next_row_data + 2;
                  if (run_length == 0xfe)
                  {
                    run_length = (unsigned int)*row_data;
                    row_data = next_row_data + 3;
                  }
                  row_data = row_data + run_length;
                }
                next_row_data = row_data;
                run_offset = run_offset - 1;
              } while (run_offset != -1);
            }
          }
        }

        row_index = row_index + 1;
        DAT_004199d8 = DAT_004199d8 + DAT_004189bc;
        row_base = row_base + row_stride;
      } while ((DAT_004199d8 >> 0x10) < row_count + top_clip);
    }
  }
}

// FUNCTION: FACEMAKER 0x004080d0
int __cdecl FUN_004080d0(int *param_1, char *param_2)
{
  char *line_ptr;
  char c;
  FontSlot *font;
  int line_width;
  int max_width;
  int char_width;
  HDC hdc;
  HGDIOBJ old_object;
  unsigned int ulen;
  SIZE size;

  line_width = 0;
  max_width = -1;
  font = &DAT_004233d0[param_1[8]];
  if (font->font_loaded == 0)
  {
    c = *param_2;
    while (c != '\0')
    {
      c = *param_2;
      if (c == '\n')
      {
        if (max_width < line_width)
        {
          max_width = line_width;
        }
        line_width = 0;
      }
      else
      {
        if (font->font_loaded == 0)
        {
          if (font->has_packed_widths == 0)
          {
            char_width =
                (int)font->data.bitmap.glyph_advance[(unsigned char)c] + (int)(unsigned char)font->unk_05;
          }
          else
          {
            char_width = (int)font->has_packed_widths + (int)(unsigned char)font->unk_05;
          }
        }
        else
        {
          char_width = (int)font->glyph_width + (int)(unsigned char)font->unk_05;
        }
        line_width = line_width + char_width;
      }
      param_2 = param_2 + 1;
      c = *param_2;
    }
    if (max_width <= line_width)
    {
      max_width = line_width;
    }
    return max_width;
  }

  hdc = DAT_00426540[*param_1]->hTempDC;
  old_object = SelectObject(hdc, font->hfont);
  c = *param_2;
  line_ptr = param_2;
  while (c != '\0')
  {
    if (*param_2 == '\n')
    {
      ulen = 0xffffffff;
      *param_2 = '\0';
      do
      {
        if (ulen == 0)
        {
          break;
        }
        ulen = ulen - 1;
        c = *line_ptr;
        line_ptr = line_ptr + 1;
      } while (c != '\0');
      GetTextExtentPoint32A(hdc, line_ptr - (~ulen), ~ulen - 1, &size);
      if (max_width < size.cx + 2)
      {
        max_width = size.cx + 2;
      }
      line_ptr = param_2 + 1;
      *param_2 = '\n';
    }
    param_2 = param_2 + 1;
    c = *param_2;
  }

  ulen = 0xffffffff;
  do
  {
    if (ulen == 0)
    {
      break;
    }
    ulen = ulen - 1;
    c = *line_ptr;
    line_ptr = line_ptr + 1;
  } while (c != '\0');
  GetTextExtentPoint32A(hdc, line_ptr - (~ulen), ~ulen - 1, &size);
  if (max_width < size.cx + 2)
  {
    max_width = size.cx + 2;
  }
  SelectObject(hdc, old_object);
  return max_width;
}

// FUNCTION: FACEMAKER 0x004082e0
int __cdecl FUN_004082e0(int *param_1, int param_2, int param_3, char *param_4)
{
  char c;
  FontSlot *font;
  int line_top;
  HDC page_hdc;
  HDC font_hdc;
  HGDIOBJ old_page_object;
  unsigned int text_color;
  unsigned int ch;
  int char_width;
  ABC abc;
  char *start;

  start = param_4;
  line_top = param_2;
  if (*param_4 == '\0')
  {
    return 0;
  }
  if (param_1[2] <= param_3)
  {
    font = &DAT_004233d0[param_1[8]];
    if (font->font_loaded == 0)
    {
      if ((int)((unsigned int)font->point_size + (unsigned int)font->unk_06 + param_3) <= param_1[4])
      {
        page_hdc = DAT_00426540[*param_1]->hTempDC;
        font_hdc = font->hdc;
        SelectObject(font_hdc, font->bitmap_inverted);
        SetTextColor(page_hdc, 0x1000000);
        SetBkColor(page_hdc, 0xffffff);
        while (*param_4 != '\0')
        {
          ch = (unsigned int)(unsigned char)*param_4;
          if (font->font_loaded == 0)
          {
            if (font->has_packed_widths == 0)
            {
              char_width = (int)font->data.bitmap.glyph_advance[ch] + (int)(unsigned char)font->unk_05;
            }
            else
            {
              char_width = (int)font->has_packed_widths + (int)(unsigned char)font->unk_05;
            }
          }
          else
          {
            char_width = (int)font->glyph_width + (int)(unsigned char)font->unk_05;
          }
          BitBlt(page_hdc, param_2, param_3, char_width, (unsigned int)font->point_size, font_hdc,
                 (ch - (unsigned int)font->first_char) * (unsigned int)font->glyph_width * 8, 0, 0x8800c6);
          param_2 = param_2 + char_width;
          param_4 = param_4 + 1;
        }

        param_2 = line_top;
        text_color = 0xfe;
        if (param_1[6] != 0xff)
        {
          text_color = (unsigned int)param_1[6];
        }
        SelectObject(font_hdc, font->bitmap_normal);
        SetTextColor(page_hdc, 0x1000000);
        SetBkColor(page_hdc, text_color & 0xffff | 0x1000000);
        do
        {
          ch = (unsigned int)(unsigned char)*start;
          if (font->font_loaded == 0)
          {
            if (font->has_packed_widths == 0)
            {
              char_width = (int)font->data.bitmap.glyph_advance[ch] + (int)(unsigned char)font->unk_05;
            }
            else
            {
              char_width = (int)font->has_packed_widths + (int)(unsigned char)font->unk_05;
            }
          }
          else
          {
            font_hdc = GetDC((HWND)0);
            SelectObject(font_hdc, font->hfont);
            GetCharABCWidthsA(font_hdc, ch, ch, &abc);
            ReleaseDC((HWND)0, font_hdc);
            char_width = abc.abcB + abc.abcC + abc.abcA;
          }
          BitBlt(page_hdc, param_2, param_3, char_width, (unsigned int)font->point_size, font_hdc,
                 (ch - (unsigned int)font->first_char) * (unsigned int)font->glyph_width * 8, 0, 0xee0086);
          param_2 = param_2 + char_width;
          start = start + 1;
        } while (*start != '\0');
        return (int)start - (int)param_4;
      }
    }
    else if ((int)((unsigned int)font->point_size + font->tm_leading + param_3) <= param_1[4])
    {
      page_hdc = DAT_00426540[*param_1]->hTempDC;
      old_page_object = SelectObject(page_hdc, font->hfont);
      text_color = (unsigned int)param_1[6];
      if (0xfd < (int)text_color)
      {
        text_color = 0xfe;
      }
      SetTextColor(page_hdc, text_color & 0xffff | 0x1000000);
      SetBkMode(page_hdc, 1);
      TextOutA(page_hdc, param_2, param_3 - font->tm_leading, param_4, lstrlenA(param_4));
      SelectObject(page_hdc, old_page_object);
      return 1;
    }
  }
  c = *start;
  (void)c;
  return 0;
}

// FUNCTION: FACEMAKER 0x004086c0
int __cdecl FUN_004086c0(int *param_1, int param_2, int param_3, int param_4, int param_5, int param_6,
                         int param_7, int param_8, int *param_9)
{
  char c;
  int iVar2;
  char *pcVar3;
  int iVar4;
  int line_has_more;
  char *line_ptr;
  int line_count;
  int saved_color;
  char local_400[1024];

  iVar2 = _vsnprintf(local_400, 0x400, (char *)*param_9, (va_list)(param_9 + 1));
  pcVar3 = local_400;
  line_count = 0;
  while (local_400[0] != '\0')
  {
    c = *pcVar3;
    pcVar3 = pcVar3 + 1;
    if (c == '\n')
    {
      line_count = line_count + 1;
    }
    local_400[0] = *pcVar3;
  }
  if (pcVar3[-1] != '\n')
  {
    line_count = line_count + 1;
  }
  if (param_4 != 0)
  {
    param_7 = (DAT_00426540[0]->width * param_7) / 0x280;
    param_8 = (DAT_00426540[0]->height * param_8) / 0x1e0;
  }
  if (param_6 != 0)
  {
    param_8 = param_8 - ((int)DAT_004233d0[param_1[8]].point_size * line_count) / 2;
  }
  if (-1 < param_2)
  {
    saved_color = param_1[6];
    param_1[6] = param_2;
  }
  line_ptr = local_400;
  while (line_count != 0)
  {
    line_has_more = line_count != 0;
    line_count = line_count - 1;
    if (!line_has_more)
    {
      break;
    }
    c = *line_ptr;
    for (pcVar3 = line_ptr; c != '\0' && *pcVar3 != '\n'; pcVar3 = pcVar3 + 1)
    {
      c = pcVar3[1];
    }
    *pcVar3 = '\0';
    iVar4 = param_7;
    if (param_5 != 0)
    {
      iVar4 = FUN_004080d0(param_1, line_ptr);
      iVar4 = param_7 - iVar4 / 2;
    }
    if (param_3 != 0)
    {
      saved_color = param_1[6];
      param_1[6] = 0;
      FUN_004082e0(param_1, iVar4 + 1, param_8 + 1, line_ptr);
      param_1[6] = saved_color;
    }
    FUN_004082e0(param_1, iVar4, param_8, line_ptr);
    *pcVar3 = '\n';
    line_ptr = pcVar3 + 1;
    if (DAT_004233d0[param_1[8]].font_loaded == 0)
    {
      param_8 = param_8 + (int)DAT_004233d0[param_1[8]].point_size + (int)DAT_004233d0[param_1[8]].unk_06;
    }
    else
    {
      param_8 = param_8 + (int)DAT_004233d0[param_1[8]].point_size + DAT_004233d0[param_1[8]].tm_leading;
    }
  }
  if (-1 < param_2)
  {
    param_1[6] = saved_color;
  }
  return iVar2;
}

// FUNCTION: FACEMAKER 0x004088d0
void __cdecl FUN_004088d0(int *dst, int text_id, int x, int y, char *text)
{
  FUN_004086c0(dst, text_id, 0, 0, 1, 1, x, y, (int *)&text);
}
#pragma optimize("", off)
