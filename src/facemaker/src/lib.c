#include <windows.h>
#include <stdlib.h>
#include "cardartlib/src/assert.h"
#include "drawcardlib/src/pic.h"

extern HDC global_main_hdc;

// GLOBAL: FACEMAKER 0x0040d0ec
char s_D__NewMagic__sources__sidlib__lib_c_0040d0ec[] = "D:\\NewMagic\\sources\\sidlib\\lib.c";

// GLOBAL: FACEMAKER 0x004189b4
int DAT_004189b4;

// GLOBAL: FACEMAKER 0x00425e10
HPALETTE DAT_00425e10;

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

// GLOBAL: FACEMAKER 0x00426980
LOGPALETTE *DAT_00426980;

// GLOBAL: FACEMAKER 0x0040d08c
int DAT_0040d08c;

// GLOBAL: FACEMAKER 0x0041afb8
HPALETTE DAT_0041afb8;

#pragma optimize("gy", on)

// FUNCTION: SHANDALAR 0x00578e80
// FUNCTION: FACEMAKER 0x00406810
DIBSurface *__cdecl FUN_00406810(int width, int height, int bits_per_pixel)
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
DIBSurface *__cdecl FUN_004069e0(int page_number, int width, int height, int bits_per_pixel)
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

#pragma optimize("", on)