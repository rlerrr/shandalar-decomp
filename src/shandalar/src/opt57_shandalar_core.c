// Optimized helpers from the 0x0057xxxx region that are implemented only in
// SHANDALAR. These were previously in shandalar.c but are kept in a dedicated
// optimized TU.

#include <windows.h>
#include <io.h>

#include "defs.h"
#include "shandalar.h"
#include "drawcardlib/src/pic.h"
#include "facemaker/src/facemaker_types.h"

extern HDC global_main_hdc;
extern DIBSurface *g_graphics_pages[10];
extern RpBitsPalettePacket g_palette_data_words;
extern PALETTEENTRY g_palette_entries[256];
extern FontSlot g_font_slots[0x10];

extern int g_mouse_button_released_mask;
extern int g_graphics_bpp;

int DrawTextFormatted(FacemakerWindowBounds *dst, int text_color, int draw_shadow, int scale_to_screen,
                      int center_x, int center_y, int x, int y, int *format_and_args);
void LoadPcxResource(int page_number, int x, int y, char *path, void *opaque);
int ExportEncodedImage(int param_1, int param_2, int param_3, int param_4, int param_5, int param_6, char *param_7);

typedef void(__cdecl *EncodeRpBitsImage_Callback)(unsigned int *scanline, int page_number, int x, int y, unsigned int width);
int EncodeRpBitsImage(int fd, EncodeRpBitsImage_Callback scanline_cb, int page_number, int x, int y, int width, int height);
void ReadGraphicsScanline(unsigned int *out_scanline, int page_number, int src_x, int src_y, unsigned int byte_count);
extern int g_export_write_palette;

#pragma optimize("gy", on)

// FUNCTION: SHANDALAR 0x00578c20
int FUN_00578c20(void)
{
  return -1;
}

// FUNCTION: SHANDALAR 0x00578c30
int FUN_00578c30(void)
{
  return -1;
}

// FUNCTION: SHANDALAR 0x00578c40
void ShowMouseCursor(void)
{
  ShowCursor(1);
}

// FUNCTION: SHANDALAR 0x00578c50
void HideMouseCursor(void)
{
  ShowCursor(0);
}

// FUNCTION: SHANDALAR 0x00578c60
int ConsumeMouseButtonReleaseMask(void)
{
  int released_mask;

  released_mask = g_mouse_button_released_mask;
  g_mouse_button_released_mask = 0;
  return released_mask;
}

// FUNCTION: SHANDALAR 0x00578c70
void FUN_00578c70(int param_1, int param_2, int param_3)
{
  (void)param_1;
  (void)param_2;
  (void)param_3;
}

// FUNCTION: SHANDALAR 0x00578c80
int FUN_00578c80(int param_1)
{
  return 0;
}

// FUNCTION: SHANDALAR 0x005795f0
unsigned int GetGraphicsPixelColorRef(FacemakerWindowBounds *window, int x, int y)
{
  int page_number;
  DIBSurface *page;

  page_number = window->page_number;
  page = g_graphics_pages[page_number];
  if (page_number != 0)
  {
    int stride = page->rowPadding + page->width;
    unsigned char *bits = (unsigned char *)page->pBits;
    return (unsigned int)bits[stride * y + x];
  }
  else
  {
    COLORREF color;
    int r;
    int g;
    int b;
    unsigned int i;
    unsigned int mask_value;
    unsigned int mask;
    unsigned char *pal;

    color = GetPixel(global_main_hdc, x, y);
    r = (int)(color & 0xff);
    g = (int)((color >> 8) & 0xff);
    b = (int)((color >> 0x10) & 0xff);

    mask_value = (-(unsigned int)(g_graphics_bpp == 0x10) & 0xfffffff9) + 0xff;
    mask = (unsigned int)(unsigned char)mask_value;

    pal = (unsigned char *)&g_palette_entries[0].peRed;
    for (i = 0; i < 0x100; i = i + 1)
    {
      if ((((unsigned int)pal[0]) & mask) == (unsigned int)r)
      {
        if ((((unsigned int)pal[1]) & mask) == (unsigned int)g)
        {
          if ((((unsigned int)pal[2]) & mask) == (unsigned int)b)
          {
            return i;
          }
        }
      }
      pal = pal + 4;
    }
    return 0xffffffff;
  }
}

// FUNCTION: SHANDALAR 0x005796c0
void DrawGraphicsLine(FacemakerWindowBounds *window_bounds, int x1, int y1, int x2, int y2, int color_index)
{
  DIBSurface *page;
  HPEN pen;
  HGDIOBJ old_pen;
  COLORREF color;
  unsigned char *palette_entry;

  page = g_graphics_pages[window_bounds->page_number];
  if (color_index < 0)
  {
    color = (COLORREF)(-color_index);
  }
  else
  {
    palette_entry = &g_palette_data_words.entry_data[color_index * 4];
    color = ((unsigned int)palette_entry[2] << 0x10) | ((unsigned int)palette_entry[1] << 8) | (unsigned int)palette_entry[0];
  }
  pen = CreatePen(0, 1, color);
  old_pen = SelectObject(page->hTempDC, pen);
  MoveToEx(page->hTempDC, x1, y1, (LPPOINT)0);
  LineTo(page->hTempDC, x2, y2);
  SelectObject(page->hTempDC, old_pen);
  DeleteObject(pen);
}

// FUNCTION: SHANDALAR 0x005797e0
void FillGraphicsRect(FacemakerWindowBounds *window_bounds, int x, int y, int width, int height, int color_index)
{
  COLORREF color;
  HBRUSH brush;
  RECT fill_rect;
  DIBSurface *page;

  fill_rect.left = x;
  fill_rect.right = x + width;
  fill_rect.top = y;
  fill_rect.bottom = y + height;

  page = g_graphics_pages[window_bounds->page_number];

  if (color_index >= 0)
  {
    if (color_index == 0xff)
    {
      color = RGB(0xff, 0xff, 0xff);
    }
    else
    {
      color = PALETTEINDEX((WORD)color_index);
    }
  }
  else
  {
    color_index = -color_index;
    color = RGB(0xff, 0xff, 0xff);
    if (color_index != 0x00ffffff)
    {
      color = PALETTERGB((BYTE)(color_index >> 8), (BYTE)color_index, (BYTE)(color_index >> 16));
    }
  }

  brush = CreateSolidBrush(color);
  FillRect(page->hTempDC, &fill_rect, brush);
  DeleteObject(brush);
}

// FUNCTION: SHANDALAR 0x00579ea0
AdvMenuRect *PushGraphicsClipRect(AdvMenuRect *saved_clip_rect, FacemakerWindowBounds *page, int x, int y, int width, int height)
{
  AdvMenuRect tmp;

  tmp.x = page->clip_left;
  tmp.y = page->clip_top;
  tmp.width = page->max_x - tmp.x;
  tmp.height = page->max_y - tmp.y;

  page->clip_top = y;
  page->clip_left = x;
  page->max_x = x + width;
  page->max_y = y + height;

  *saved_clip_rect = tmp;
  return saved_clip_rect;
}

// FUNCTION: SHANDALAR 0x0057a9f0
BOOL UnloadFontSlot(int font_slot)
{
  FontSlot *font;

  font = &g_font_slots[font_slot];
  if (font->font_loaded == 0)
  {
    return FALSE;
  }

  DeleteObject(font->hfont);
  return RemoveFontResourceA(font->data.gdi.font_file);
}

// FUNCTION: SHANDALAR 0x0057aa30
int GetFontCharWidth(int font_slot, unsigned char ch_value)
{
  FontSlot *font;
  HDC hdc;
  ABC abc;
  unsigned int ch;

  font = &g_font_slots[font_slot];
  ch = (unsigned int)(unsigned char)ch_value;
  if (font->font_loaded != 0)
  {
    hdc = GetDC((HWND)0);
    SelectObject(hdc, font->hfont);
    GetCharABCWidthsA(hdc, ch, ch, &abc);
    ReleaseDC((HWND)0, hdc);
    return abc.abcA + abc.abcB + abc.abcC;
  }
  if (font->has_packed_widths != 0)
  {
    return (unsigned int)font->has_packed_widths + (unsigned int)font->unk_05;
  }
  return (unsigned int)font->unk_05 + (unsigned int)font->data.bitmap.glyph_advance[ch];
}

// FUNCTION: SHANDALAR 0x0057acb0
int MeasureTextSpanWidth(FacemakerWindowBounds *window, char *text, int length)
{
  char ch;
  unsigned char glyph_width;
  unsigned char glyph_spacing;
  FontSlot *font;
  HGDIOBJ old_font;
  HDC hdc;
  int char_width;
  int text_width;
  ABC abc;

  text_width = 0;
  font = &g_font_slots[window->font_slot];
  if (font->font_loaded == 0)
  {
    ch = *text;
    while ((ch != '\0') && (length-- != 0))
    {
      ch = *text;
      text++;
      if (font->font_loaded == 0)
      {
        glyph_width = font->glyph_width;
        if (glyph_width == 0)
        {
          glyph_width = font->data.bitmap.glyph_advance[(unsigned char)ch];
          glyph_spacing = font->unk_05;
        }
        else
        {
          glyph_spacing = font->unk_05;
        }
        char_width = (unsigned int)glyph_spacing + (unsigned int)glyph_width;
      }
      else
      {
        hdc = GetDC((HWND)0);
        SelectObject(hdc, font->hfont);
        GetCharABCWidthsA(hdc, ch, ch, &abc);
        ReleaseDC((HWND)0, hdc);
        char_width = abc.abcB + abc.abcC + abc.abcA;
      }
      text_width += char_width;
      ch = *text;
    }
    return text_width;
  }

  hdc = g_graphics_pages[window->page_number]->hTempDC;
  old_font = SelectObject(hdc, font->hfont);
  GetTextExtentPoint32A(hdc, text, length, (LPSIZE)&abc);
  SelectObject(hdc, old_font);
  return abc.abcA;
}

// FUNCTION: SHANDALAR 0x0057adf0
int GetFontLineHeight(int font_slot)
{
  FontSlot *font;

  font = &g_font_slots[font_slot];
  if (font->font_loaded != 0)
  {
    return (unsigned int)font->point_size + font->tm_leading;
  }
  return (unsigned int)font->point_size + (unsigned int)font->unk_06;
}

// FUNCTION: SHANDALAR 0x0057b4d0
void DrawFormattedTextShadowed(FacemakerWindowBounds *window, int color_index, int x, int y, char *format, ...)
{
  DrawTextFormatted(window, color_index, 1, 0, 0, 0, x, y, (int *)&format);
}

// FUNCTION: SHANDALAR 0x0057b530
void DrawFormattedTextShadowedCenterY(FacemakerWindowBounds *window, int color_index, int x, int y, char *format, ...)
{
  DrawTextFormatted(window, color_index, 1, 0, 0, 1, x, y, (int *)&format);
}

// FUNCTION: SHANDALAR 0x0057b560
void DrawFormattedTextShadowedCentered(FacemakerWindowBounds *window, int color_index, int x, int y, char *format, ...)
{
  DrawTextFormatted(window, color_index, 1, 0, 1, 1, x, y, (int *)&format);
}

// FUNCTION: SHANDALAR 0x0057b590
void FUN_0057b590(FacemakerWindowBounds *window, int color_index, int x, int y, char *text)
{
  DrawTextFormatted(window, color_index, 0, 1, 1, 1, x, y, (int *)&text);
}

// FUNCTION: SHANDALAR 0x0057b5f0
void FUN_0057b5f0(FacemakerWindowBounds *window, int color_index, int x, int y, char *format, ...)
{
  DrawTextFormatted(window, color_index, 1, 1, 0, 1, x, y, (int *)&format);
}

// FUNCTION: SHANDALAR 0x0057c7e0
void LoadPcxIntoPageOpaque(int page_number, char *path)
{
  LoadPcxResource(page_number, 0, 0, path, (RpBitsPalettePacket *)1);
}

// FUNCTION: SHANDALAR 0x0057dd30
int ExportGraphicsPage(int page_number, char *path)
{
  int fd;
  int result;

  fd = _open(path, 0x8302, 0x80);
  if (fd == -1)
  {
    return fd;
  }

  g_export_write_palette = 0;
  result = EncodeRpBitsImage(fd, ReadGraphicsScanline, page_number, 0, 0, 0x140, 0xc8);
  _close(fd);
  return result;
}

// FUNCTION: SHANDALAR 0x0057e826
char *FUN_0057e826(char *dst, char *src)
{
  char *p;

  p = dst;
  while (*p != '\0')
  {
    p = p + 1;
  }
  while (1)
  {
    *p = *src;
    if (*src == '\0')
    {
      break;
    }
    p = p + 1;
    src = src + 1;
  }

  return dst;
}

#pragma optimize("", on)
