#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include "cardartlib/src/assert.h"
#include "drawcardlib/src/pic.h"

extern HDC global_main_hdc;
DIBSurface * FUN_00406650(void);
DIBSurface * FUN_004069e0(int page_number, int width, int height, int bits_per_pixel);
int FUN_00406d00(int page_number, int color_index);
void FUN_0040aa20(double *dst, double *src, unsigned int size);

// GLOBAL: FACEMAKER 0x0040d0ec
char s_D__NewMagic__sources__sidlib__lib_c_0040d0ec[] = "D:\\NewMagic\\sources\\sidlib\\lib.c";

// GLOBAL: FACEMAKER 0x0040d110
char s_Graphic_Page_number_out_of_range_0040d110[] = "Graphic Page number out of range: %d\n";

// GLOBAL: FACEMAKER 0x0040d140
char s_Cannot_explicitly_Deallocate_page_0_0040d140[] = "Cannot explicitly Deallocate page 0\n";

// GLOBAL: FACEMAKER 0x0040d090
int DAT_0040d090 = 1;

// GLOBAL: FACEMAKER 0x0040d168
char s_GetLine_not_implemented_for_page_0040d168[] = "GetLine not implemented for page 0\n";

// GLOBAL: FACEMAKER 0x00417188
BITMAPINFO *DAT_00417188;

// GLOBAL: FACEMAKER 0x00417998
int DAT_00417998;

// GLOBAL: FACEMAKER 0x0040d088
int DAT_0040d088 = 1;

// GLOBAL: FACEMAKER 0x00417180
BITMAPINFO *DAT_00417180;

// GLOBAL: FACEMAKER 0x0041799c
int DAT_0041799c;

// GLOBAL: FACEMAKER 0x004181b0
char DAT_004181b0[0x804];

// GLOBAL: FACEMAKER 0x0040d094
char *PTR_DAT_0040d094 = DAT_004181b0;

// GLOBAL: FACEMAKER 0x004189b4
int DAT_004189b4;

// GLOBAL: FACEMAKER 0x00425e10
HPALETTE DAT_00425e10;

// GLOBAL: FACEMAKER 0x00425e20
unsigned int DAT_00425e20[0xc8];

// GLOBAL: FACEMAKER 0x00426140
PALETTEENTRY DAT_00426140[256];

// GLOBAL: FACEMAKER 0x00426540
DIBSurface *DAT_00426540[10];

// GLOBAL: FACEMAKER 0x00426568
int DAT_00426568;

// GLOBAL: FACEMAKER 0x0042656c
int DAT_0042656c;

// GLOBAL: FACEMAKER 0x00426570
int DAT_00426570;

// GLOBAL: FACEMAKER 0x00426580
RGBQUAD DAT_00426580[256];

// GLOBAL: FACEMAKER 0x00426980
LOGPALETTE *DAT_00426980;

// GLOBAL: FACEMAKER 0x00420df0
unsigned int DAT_00420df0[0xc8];

// GLOBAL: FACEMAKER 0x00421110
int DAT_00421110[0x400];

// GLOBAL: FACEMAKER 0x0041f5f0
int DAT_0041f5f0[0x301];

// GLOBAL: FACEMAKER 0x004201f4
int DAT_004201f4[0x2ff];

// GLOBAL: FACEMAKER 0x0040d08c
int DAT_0040d08c = 0;

// GLOBAL: FACEMAKER 0x0040d2e4
#if defined(FACEMAKER)
unsigned char *PTR_DAT_0040d2e4 = (unsigned char *)0x00425e26;
#else
unsigned char *PTR_DAT_0040d2e4 = (unsigned char *)DAT_00425e20 + 6;
#endif

// GLOBAL: FACEMAKER 0x0041afb8
HPALETTE DAT_0041afb8;

#pragma optimize("gy", on)

// FUNCTION: FACEMAKER 0x00406650
DIBSurface * FUN_00406650(void)
{
  char mapping_name[10] = "rpbits";
  DIBSurface *surface;
  int width;
  int height;
  int bits_per_pixel_stack;
  int *bits_per_pixel_ptr;
  int image_size_bytes;
  LOGPALETTE *log_palette;
  LOGPALETTE *log_palette_cursor;
  HPALETTE palette_handle;
  PALETTEENTRY *palette_entry;
  int palette_entries_left;

  if (DAT_004189b4 != 0)
  {
    return DAT_00426540[0];
  }

  surface = (DIBSurface *)malloc(0x30);
  width = GetDeviceCaps(global_main_hdc, 8);
  DAT_0042656c = width;
  surface->width = width;
  height = GetDeviceCaps(global_main_hdc, 10);
  DAT_00426568 = height;
  surface->height = height;
  bits_per_pixel_ptr = &bits_per_pixel_stack;
  *bits_per_pixel_ptr = GetDeviceCaps(global_main_hdc, 12);
  DAT_00426570 = *bits_per_pixel_ptr;
  surface->bitsPerPixel = *bits_per_pixel_ptr;
  DAT_0040d08c = 0;

  image_size_bytes = width * height * *bits_per_pixel_ptr;
  surface->imageSizeBytes = (image_size_bytes + ((image_size_bytes >> 31) & 7)) >> 3;
  _itoa(0, mapping_name, 10);
  surface->hTempDC = global_main_hdc;
  surface->pBitmapInfo = CreateBitmapInfo(width, height, *bits_per_pixel_ptr);
  surface->pBitmapInfo = CreateBitmapInfo(width, height, 8);
  surface->hMapping = 0;
  surface->hBitmap = 0;

  log_palette = (LOGPALETTE *)malloc(0x408);
  log_palette->palVersion = 0x300;
  palette_entries_left = 0x100;
  log_palette->palNumEntries = 0x100;
  log_palette_cursor = log_palette;
  do
  {
    log_palette_cursor->palPalEntry[0].peRed = 0;
    --palette_entries_left;
    log_palette_cursor->palPalEntry[0].peGreen = 0;
    log_palette_cursor->palPalEntry[0].peBlue = 0;
    log_palette_cursor->palPalEntry[0].peFlags = 1;
    log_palette_cursor = (LOGPALETTE *)log_palette_cursor->palPalEntry;
  } while (palette_entries_left != 0);
  log_palette->palPalEntry[0].peFlags = 0;
  log_palette->palPalEntry[255].peFlags = 0;
  DAT_00426980 = log_palette;

  palette_entry = DAT_00426140;
  do
  {
    palette_entry->peRed = 0;
    palette_entry->peGreen = 0;
    palette_entry->peBlue = 0;
    palette_entry->peFlags = 1;
    ++palette_entry;
  } while (palette_entry < (PALETTEENTRY *)DAT_00426540);
  DAT_00426140[0].peFlags = 0;
  DAT_00426140[255].peFlags = 0;

  palette_handle = CreatePalette(log_palette);
  DAT_00425e10 = palette_handle;
  DAT_0041afb8 = palette_handle;
  surface->hPalette = palette_handle;
  SelectPalette(surface->hTempDC, palette_handle, FALSE);
  RealizePalette(surface->hTempDC);
  SetStretchBltMode(surface->hTempDC, 3);
  DAT_00426540[0] = surface;
  DAT_004189b4 = 1;
  return surface;
}

// FUNCTION: SHANDALAR 0x00578e80
// FUNCTION: FACEMAKER 0x00406810
DIBSurface * FUN_00406810(int width, int height, int bits_per_pixel)
{
  char mapping_name[16] = "rpbits";

  DIBSurface *surface;
  HDC screen_dc;
  int image_size_bytes;
  LOGPALETTE *log_palette;
  LOGPALETTE *log_palette_cursor;
  HPALETTE palette_handle;
  PALETTEENTRY *palette_entry;
  int palette_entries_left;

  if (DAT_004189b4 != 0)
  {
    return DAT_00426540[0];
  }

  surface = (DIBSurface *)malloc(0x30);
  DAT_0042656c = width;
  surface->width = width;
  DAT_00426568 = height;
  surface->height = height;
  if (bits_per_pixel == -1)
  {
    screen_dc = GetDC((HWND)0);
    bits_per_pixel = GetDeviceCaps(screen_dc, BITSPIXEL);
    DAT_00426570 = bits_per_pixel;
    surface->bitsPerPixel = bits_per_pixel;
    ReleaseDC((HWND)0, screen_dc);
  }
  else
  {
    DAT_00426570 = bits_per_pixel;
    surface->bitsPerPixel = bits_per_pixel;
  }

  image_size_bytes = bits_per_pixel * height * width;
  DAT_0040d08c = 0;
  surface->imageSizeBytes = (image_size_bytes + ((image_size_bytes >> 31) & 7)) >> 3;
  _itoa(0, mapping_name, 10);
  surface->hTempDC = global_main_hdc;
  surface->pBitmapInfo = CreateBitmapInfo(width, height, bits_per_pixel);
  surface->pBitmapInfo = CreateBitmapInfo(width, height, 8);
  surface->hMapping = 0;
  surface->hBitmap = 0;

  log_palette = (LOGPALETTE *)malloc(0x408);
  log_palette->palVersion = 0x300;
  palette_entries_left = 0x100;
  log_palette->palNumEntries = 0x100;
  log_palette_cursor = log_palette;
  do
  {
    log_palette_cursor->palPalEntry[0].peRed = 0;
    --palette_entries_left;
    log_palette_cursor->palPalEntry[0].peGreen = 0;
    log_palette_cursor->palPalEntry[0].peBlue = 0;
    log_palette_cursor->palPalEntry[0].peFlags = 1;
    log_palette_cursor = (LOGPALETTE *)log_palette_cursor->palPalEntry;
  } while (palette_entries_left != 0);
  log_palette->palPalEntry[0].peFlags = 0;
  log_palette->palPalEntry[255].peFlags = 0;
  DAT_00426980 = log_palette;

  palette_entry = DAT_00426140;
  do
  {
    palette_entry->peRed = 0;
    palette_entry->peGreen = 0;
    palette_entry->peBlue = 0;
    palette_entry->peFlags = 1;
    ++palette_entry;
  } while (palette_entry < (PALETTEENTRY *)DAT_00426540);
  DAT_00426140[0].peFlags = 0;
  DAT_00426140[255].peFlags = 0;

  palette_handle = CreatePalette(log_palette);
  DAT_00425e10 = palette_handle;
  DAT_0041afb8 = palette_handle;
  surface->hPalette = palette_handle;
  SelectPalette(surface->hTempDC, palette_handle, FALSE);
  RealizePalette(surface->hTempDC);
  SetStretchBltMode(surface->hTempDC, 3);
  DAT_00426540[0] = surface;
  DAT_004189b4 = 1;
  return surface;
}

// FUNCTION: SHANDALAR 0x00579050
// FUNCTION: FACEMAKER 0x004069e0
DIBSurface * FUN_004069e0(int page_number, int width, int height, int bits_per_pixel)
{
  char mapping_name[16] = "rpbips";

  DIBSurface *page;
  int row_remainder;
  int total_bits;
  int image_size_bytes;
  DWORD *dword_ptr;
  unsigned int fill_count;

  if (page_number == 0)
  {
    return FUN_00406810(width, height, bits_per_pixel);
  }

  assert(page_number < 10 ? 1 : 0, s_D__NewMagic__sources__sidlib__lib_c_0040d0ec, 0x121,
         "Graphic Page number out of range: %d\n", page_number);

  page = (DIBSurface *)malloc(0x30);
  page->width = width;
  page->height = height;
  page->bitsPerPixel = bits_per_pixel;

  total_bits = bits_per_pixel * width;
  total_bits += (total_bits >> 31) & 7;
  row_remainder = total_bits >> 3;
  row_remainder = (row_remainder ^ (row_remainder >> 31)) - (row_remainder >> 31);
  row_remainder &= 3;
  if (row_remainder == 0)
  {
    page->rowPadding = 0;
  }
  else
  {
    page->rowPadding = 4 - row_remainder;
  }

  image_size_bytes = (page->rowPadding + width) * bits_per_pixel * height;
  image_size_bytes = (image_size_bytes + ((image_size_bytes >> 31) & 7)) >> 3;
  page->imageSizeBytes = image_size_bytes;

  _itoa(page_number, mapping_name, 10);
  page->hTempDC = CreateCompatibleDC((HDC)0);
  page->pBitmapInfo = CreateBitmapInfo(width, height, bits_per_pixel);
  page->hMapping =
      CreateFileMappingA((HANDLE)0xffffffff, (LPSECURITY_ATTRIBUTES)0, 0x8000004, 0, image_size_bytes,
                         mapping_name);
  assert((int)page->hMapping, s_D__NewMagic__sources__sidlib__lib_c_0040d0ec, 0x137,
         "Create File Mapping failed: page %d\n", page_number);

  page->hBitmap =
      CreateDIBSection(page->hTempDC, page->pBitmapInfo, (unsigned int)(bits_per_pixel == 8),
                       &page->pBits, page->hMapping, 0);
  assert((int)page->hBitmap, s_D__NewMagic__sources__sidlib__lib_c_0040d0ec, 0x13b,
         "WM_CREATE CreateDIBSection");

  page->hPreviousBitmap = SelectObject(page->hTempDC, page->hBitmap);
  page->hPalette = DAT_00425e10;
  SelectPalette(page->hTempDC, page->hPalette, FALSE);
  RealizePalette(page->hTempDC);
  SetStretchBltMode(page->hTempDC, 3);

  dword_ptr = (DWORD *)page->pBits;
  fill_count = image_size_bytes >> 2;
  while (fill_count != 0)
  {
    *dword_ptr = 0;
    ++dword_ptr;
    --fill_count;
  }

  fill_count = image_size_bytes & 3;
  while (fill_count != 0)
  {
    *(unsigned char *)dword_ptr = 0;
    dword_ptr = (DWORD *)((char *)dword_ptr + 1);
    --fill_count;
  }

  return page;
}

// FUNCTION: FACEMAKER 0x00406bd0
void FUN_00406bd0(int page_number, DIBSurface *page)
{
  if (page_number != 0)
  {
    if (DAT_00426540[page_number] != (DIBSurface *)0)
    {
      assert((unsigned int)(page_number >= 1), s_D__NewMagic__sources__sidlib__lib_c_0040d0ec, 0x156,
             s_Cannot_explicitly_Deallocate_page_0_0040d140);
      assert((unsigned int)(page_number < 10), s_D__NewMagic__sources__sidlib__lib_c_0040d0ec, 0x157,
             s_Graphic_Page_number_out_of_range_0040d110, page_number);

      if (DAT_00426540[page_number] != (DIBSurface *)0)
      {
        SelectObject(DAT_00426540[page_number]->hTempDC, DAT_00426540[page_number]->hPreviousBitmap);
        DeleteObject(DAT_00426540[page_number]->hBitmap);
        free(DAT_00426540[page_number]->pBitmapInfo);
        CloseHandle(DAT_00426540[page_number]->hMapping);
        SelectObject(DAT_00426540[page_number]->hTempDC, GetStockObject(15));
        RealizePalette(DAT_00426540[page_number]->hTempDC);
        DeleteDC(DAT_00426540[page_number]->hTempDC);
        free(DAT_00426540[page_number]);
        DAT_00426540[page_number] = (DIBSurface *)0;
      }
    }
  }
  DAT_00426540[page_number] = page;
}

// FUNCTION: FACEMAKER 0x00407190
void FUN_00407190(int *param_1, int param_2, int param_3, unsigned int param_4)
{
  DIBSurface *page;
  COLORREF color;
  unsigned int value;
  unsigned int rebuilt_color;
  unsigned int upper_byte;
  unsigned int middle_byte;
  unsigned char low_byte;
  unsigned char high_byte;

  page = DAT_00426540[*param_1];
  value = param_4;
  if ((int)value < 0)
  {
    value = -value;
    color = 0xffffff;
    if (value != color)
    {
      low_byte = (unsigned char)value;
      high_byte = (unsigned char)(value >> 8);
      upper_byte = (value >> 0x10) & 0xff;
      rebuilt_color = (unsigned int)high_byte;
      rebuilt_color = rebuilt_color | 0x20000;
      rebuilt_color = rebuilt_color << 8;
      middle_byte = upper_byte << 0x10;
      rebuilt_color = rebuilt_color | middle_byte;
      rebuilt_color = rebuilt_color | (unsigned int)low_byte;
      color = rebuilt_color;
    }
  }
  else
  {
    if (value == 0xff)
    {
      color = 0xffffff;
    }
    else
    {
      color = value & 0xffff;
      color = color | 0x1000000;
    }
  }
  SetPixelV(page->hTempDC, param_2, param_3, color);
}

// FUNCTION: FACEMAKER 0x00406cc0
void FUN_00406cc0(int enabled)
{
  if (DAT_00426540[enabled]->hTempDC != (HDC)0 && DAT_00426540[0]->hTempDC != (HDC)0)
  {
    BitBlt(DAT_00426540[0]->hTempDC, 0, 0, DAT_00426540[0]->width, DAT_00426540[0]->height,
           DAT_00426540[enabled]->hTempDC, 0, 0, 0xcc0020);
  }
}

// FUNCTION: FACEMAKER 0x00406d00
int FUN_00406d00(int page_number, int color_index)
{
  int page_ptr;
  HBRUSH brush;
  RECT rect;
  LOGBRUSH brush_desc;

  brush_desc.lbStyle = 0;
  page_ptr = (int)DAT_00426540[page_number];
  brush_desc.lbColor = (((unsigned int)DAT_00426140[color_index].peGreen | 0x20000) << 8) |
                       ((unsigned int)DAT_00426140[color_index].peBlue << 0x10) |
                       (unsigned int)DAT_00426140[color_index].peRed;
  brush = CreateBrushIndirect(&brush_desc);
  rect.top = 0;
  rect.left = 0;
  rect.right = *(LONG *)(page_ptr + 0x20);
  rect.bottom = *(LONG *)(page_ptr + 0x24);
  FillRect(*(HDC *)(page_ptr + 4), &rect, brush);
  return DeleteObject(brush);
}

// FUNCTION: FACEMAKER 0x00407210
void FUN_00407210(int *dst, unsigned int dst_x, int dst_y, unsigned int width, DWORD height,
                          int *src, int src_x, int src_y)
{
  int i;
  int src_bits_per_row;
  int dst_bits_per_row;
  int src_x_bits;
  int dst_x_bits;
  int copy_bits;
  int src_stride_bytes;
  int dst_stride_bytes;
  int copy_bytes;
  int src_page_number;
  int dst_page_number;
  DIBSurface *src_page;
  DIBSurface *dst_page;
  RGBQUAD *src_palette;
  RGBQUAD *dst_palette;
  double *top_row;
  double *bottom_row;
  int half_height;

  src_page_number = *dst;
  dst_page_number = *src;
  src_page = DAT_00426540[src_page_number];
  dst_page = DAT_00426540[dst_page_number];

  if (DAT_0041799c == 0)
  {
    DAT_00417180 = CreateBitmapInfo(1, 1, 8);
    DAT_0041799c = 1;
  }

  DAT_00417180->bmiHeader.biWidth = src_page->width;
  if (dst_y == 0)
  {
    DAT_00417180->bmiHeader.biHeight = src_page->height;
  }
  else
  {
    DAT_00417180->bmiHeader.biHeight = src_page->height;
  }

  if (dst_page_number == 0)
  {
    if (((dst_x & 7) == 0) && DAT_0040d088 != 0)
    {
      if (DAT_00426570 != 8)
      {
        src_palette = DAT_00426580;
        dst_palette = DAT_00417180->bmiColors;
        for (i = 0x100; i != 0; i--)
        {
          *dst_palette = *src_palette;
          src_palette++;
          dst_palette++;
        }
      }

      top_row = (double *)(dst_y * src_page->width + dst_x + (int)src_page->pBits);
      bottom_row = (double *)((dst_y + height - 1) * src_page->width + dst_x + (int)src_page->pBits);
      half_height = (int)height / 2;
      while (half_height > 0)
      {
        FUN_0040aa20((double *)PTR_DAT_0040d094, top_row, width);
        FUN_0040aa20(top_row, bottom_row, width);
        FUN_0040aa20(bottom_row, (double *)PTR_DAT_0040d094, width);
        top_row = (double *)((int)top_row + src_page->width);
        bottom_row = (double *)((int)bottom_row - src_page->width);
        half_height--;
      }

      SetDIBitsToDevice(dst_page->hTempDC, src_x, src_y, width, height, dst_x, dst_y, 0, src_page->height,
                        src_page->pBits, DAT_00417180, (unsigned int)(DAT_00426570 == 8));
      return;
    }
  }
  else if (src_page_number != 0 && src_page->bitsPerPixel == dst_page->bitsPerPixel)
  {
    if (src_page == dst_page && src_y < dst_y)
    {
      if ((int)height < 1)
      {
        return;
      }
      for (i = 0; i < (int)height; i++)
      {
        src_bits_per_row = src_page->width * src_page->bitsPerPixel;
        src_x_bits = dst_x * src_page->bitsPerPixel;
        dst_bits_per_row = dst_page->width * dst_page->bitsPerPixel;
        dst_x_bits = src_x * dst_page->bitsPerPixel;
        copy_bits = width * dst_page->bitsPerPixel;
        memmove((void *)((dst_page->rowPadding + ((dst_bits_per_row + ((dst_bits_per_row >> 0x1f) & 7U)) >> 3)) *
                            (src_y + i) +
                        ((dst_x_bits + ((dst_x_bits >> 0x1f) & 7U)) >> 3) + (int)dst_page->pBits),
                (void *)((src_page->rowPadding + ((src_bits_per_row + ((src_bits_per_row >> 0x1f) & 7U)) >> 3)) *
                            (dst_y + i) +
                        ((src_x_bits + ((src_x_bits >> 0x1f) & 7U)) >> 3) + (int)src_page->pBits),
                (int)(copy_bits + ((copy_bits >> 0x1f) & 7U)) >> 3);
      }
      return;
    }

    for (i = (int)height - 1; i >= 0; i--)
    {
      src_bits_per_row = src_page->width * src_page->bitsPerPixel;
      src_x_bits = dst_x * src_page->bitsPerPixel;
      dst_bits_per_row = dst_page->width * dst_page->bitsPerPixel;
      dst_x_bits = src_x * dst_page->bitsPerPixel;
      copy_bits = width * dst_page->bitsPerPixel;
      memmove((void *)((dst_page->rowPadding + ((dst_bits_per_row + ((dst_bits_per_row >> 0x1f) & 7U)) >> 3)) *
                          (src_y + i) +
                      ((dst_x_bits + ((dst_x_bits >> 0x1f) & 7U)) >> 3) + (int)dst_page->pBits),
              (void *)((src_page->rowPadding + ((src_bits_per_row + ((src_bits_per_row >> 0x1f) & 7U)) >> 3)) *
                          (dst_y + i) +
                      ((src_x_bits + ((src_x_bits >> 0x1f) & 7U)) >> 3) + (int)src_page->pBits),
              (int)(copy_bits + ((copy_bits >> 0x1f) & 7U)) >> 3);
    }
    return;
  }

  src_stride_bytes = src_page->width;
  dst_stride_bytes = dst_page->width;
  copy_bytes = width;
  (void)src_stride_bytes;
  (void)dst_stride_bytes;
  (void)copy_bytes;
  BitBlt(dst_page->hTempDC, src_x, src_y, width, height, src_page->hTempDC, dst_x, dst_y, SRCCOPY);
}

// FUNCTION: FACEMAKER 0x00407570
void FUN_00407570(void *dst, int dst_x, int dst_y, int src_w, int src_h, void *src,
                          int src_x, int src_y, int copy_w, int copy_h)
{
  StretchBlt(DAT_00426540[*(int *)src]->hTempDC, src_x, src_y, copy_w, copy_h,
             DAT_00426540[*(int *)dst]->hTempDC, dst_x, dst_y, src_w, src_h, 0xcc0020);
}

// FUNCTION: FACEMAKER 0x004075d0
void FUN_004075d0(unsigned int *param_1, int param_2, int param_3, int param_4,
                          unsigned int param_5)
{
  int iVar1;
  int iVar2;
  unsigned int uVar3;
  RGBQUAD *pRVar4;
  RGBQUAD *pRVar5;
  unsigned int *puVar6;

  if (DAT_00417998 == 0)
  {
    DAT_00417188 = (BITMAPINFO *)CreateBitmapInfo(1, 1, 8);
    DAT_00417998 = 1;
  }
  iVar1 = (int)DAT_00426540[param_2];
  DAT_00417188->bmiHeader.biWidth = param_5;
  if (param_2 == 0)
  {
    if ((DAT_00426570 != 8) && (DAT_0040d090 != 0))
    {
      pRVar4 = (RGBQUAD *)&DAT_00426580;
      pRVar5 = DAT_00417188->bmiColors;
      for (iVar2 = 0x100; iVar2 != 0; iVar2 = iVar2 - 1)
      {
        *pRVar5 = *pRVar4;
        pRVar4 = pRVar4 + 1;
        pRVar5 = pRVar5 + 1;
      }
      DAT_0040d090 = 0;
    }
    SetDIBitsToDevice(*(HDC *)(iVar1 + 4), param_3, param_4, param_5, 1, 0, 0, 0, 1, param_1, DAT_00417188,
                      (unsigned int)(DAT_00426570 == 8));
    return;
  }
  puVar6 = (unsigned int *)(param_3 + (*(int *)(iVar1 + 0x20) + *(int *)(iVar1 + 0x2c)) * param_4 +
                            *(int *)(iVar1 + 0x18));
  for (uVar3 = param_5 >> 2; uVar3 != 0; uVar3 = uVar3 - 1)
  {
    *puVar6 = *param_1;
    param_1 = param_1 + 1;
    puVar6 = puVar6 + 1;
  }
  for (uVar3 = param_5 & 3; uVar3 != 0; uVar3 = uVar3 - 1)
  {
    *(unsigned char *)puVar6 = *(unsigned char *)param_1;
    param_1 = (unsigned int *)((int)param_1 + 1);
    puVar6 = (unsigned int *)((int)puVar6 + 1);
  }
}

// FUNCTION: FACEMAKER 0x00406da0
unsigned int FUN_00406da0(int param_1, int param_2, int param_3)
{
  int iVar1;
  COLORREF CVar2;
  unsigned int uVar3;
  unsigned char *pbVar4;
  unsigned int uVar5;

  iVar1 = (int)DAT_00426540[param_1];
  if (param_1 == 0)
  {
    CVar2 = GetPixel(global_main_hdc, param_2, param_3);
    pbVar4 = (unsigned char *)&DAT_00426140;
    uVar3 = 0;
    uVar5 = (-(unsigned int)(DAT_00426570 == 0x10) & 0xfffffff9) + 0xff;
    while ((((*pbVar4 & uVar5) != (CVar2 & 0xff) || ((pbVar4[1] & uVar5) != (CVar2 >> 8 & 0xff))) ||
            ((pbVar4[2] & uVar5) != (CVar2 >> 0x10 & 0xff))))
    {
      uVar3 = uVar3 + 1;
      pbVar4 = pbVar4 + 4;
      if (0xff < (int)uVar3)
      {
        return 0xffffffff;
      }
    }
  }
  else
  {
    uVar3 = (unsigned int)*(unsigned char *)((*(int *)(iVar1 + 0x2c) + *(int *)(iVar1 + 0x20)) * param_3 +
                                             *(int *)(iVar1 + 0x18) + param_2);
  }
  return uVar3;
}

// FUNCTION: FACEMAKER 0x004076c0
void FUN_004076c0(unsigned int *param_1, int param_2, int param_3, int param_4, unsigned int param_5)
{
  int iVar1;
  unsigned int uVar2;
  unsigned int *puVar3;

  assert((unsigned int)(param_2 != 0), s_D__NewMagic__sources__sidlib__lib_c_0040d0ec, 0x503,
         s_GetLine_not_implemented_for_page_0040d168);
  iVar1 = (int)DAT_00426540[param_2];
  puVar3 = (unsigned int *)((*(int *)(iVar1 + 0x2c) + *(int *)(iVar1 + 0x20)) * param_4 + *(int *)(iVar1 + 0x18) +
                            param_3);
  for (uVar2 = param_5 >> 2; uVar2 != 0; uVar2 = uVar2 - 1)
  {
    *param_1 = *puVar3;
    puVar3 = puVar3 + 1;
    param_1 = param_1 + 1;
  }
  for (uVar2 = param_5 & 3; uVar2 != 0; uVar2 = uVar2 - 1)
  {
    *(unsigned char *)param_1 = *(unsigned char *)puVar3;
    puVar3 = (unsigned int *)((int)puVar3 + 1);
    param_1 = (unsigned int *)((int)param_1 + 1);
  }
}

// FUNCTION: FACEMAKER 0x0040aa20
void FUN_0040aa20(double *dst, double *src, unsigned int num)
{
#ifdef MODERN_FIXES
  memcpy(dst,src,num);
#else
  //TODO: this looks like real inline asm but who knows
  __asm {
    mov edi, dst
    mov esi, src
    mov ecx, num
    push ecx
    shr ecx, 3
  copy_bytes_qword_loop:
    fld qword ptr [esi]
    fstp qword ptr [edi]
    add esi, 8
    add edi, 8
    dec ecx
    jne copy_bytes_qword_loop
  copy_bytes_tail:
    pop ecx
    and ecx, 7
    rep movsb
  copy_bytes_done:
  }
#endif
}
