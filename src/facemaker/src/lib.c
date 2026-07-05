#include "defs.h"
#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include "inttypes.h"
#include "cardartlib/src/assert.h"
#include "drawcardlib/src/pic.h"
#include "facemaker_types.h"

extern HDC global_main_hdc;
extern int global_screen_width;
extern int global_screen_height;

DIBSurface *InitializeGraphicsSystemDefaultMode(void);
DIBSurface *CreateGraphicsPage(int page_number, int width, int height, int bits_per_pixel);
int ClearGraphicsPageWithPaletteColor(int page_number, int color_index);
void CopyBytesAsmCompat(double *dst, double *src, unsigned int size);

// GLOBAL: SHANDALAR 0x00581804
// GLOBAL: FACEMAKER 0x0040d084
int g_key_input_queue_count = 0;

typedef struct
{
  int8_t present; // nonzero => this mapping exists
  uint8_t unk;    // maybe flags, maybe pad
  uint16_t ch;    // resulting character / code
} keymap_variant_t;

typedef struct
{
  // [normal, shift, ctrl, alt]
  keymap_variant_t states[4];
} keymap_entry_t;

// GLOBAL: SHANDALAR 0x00581270
// GLOBAL: FACEMAKER 0x0040caf0
keymap_entry_t g_virtual_key_char_map[89] = {
#include "virtual_key_char_map_init.inc"
};

// GLOBAL: FACEMAKER 0x0040d0ec
// GLOBAL: SHANDALAR 0x005a166c
char s_D__NewMagic__sources__sidlib__lib_c_0040d0ec[] = "D:\\NewMagic\\sources\\sidlib\\lib.c";

// GLOBAL: FACEMAKER 0x0040d110
char s_Graphic_Page_number_out_of_range_0040d110[] = "Graphic Page number out of range: %d\n";

// GLOBAL: FACEMAKER 0x0040d140
char s_Cannot_explicitly_Deallocate_page_0_0040d140[] = "Cannot explicitly Deallocate page 0\n";

// GLOBAL: FACEMAKER 0x0040d090
int g_scanline_palette_needs_refresh = 1;

// GLOBAL: FACEMAKER 0x0040d168
char s_GetLine_not_implemented_for_page_0040d168[] = "GetLine not implemented for page 0\n";

// GLOBAL: SHANDALAR 0x005a7520
// GLOBAL: FACEMAKER 0x004128a0
unsigned int g_key_input_queue[50];

// GLOBAL: SHANDALAR 0x005a75e8
// GLOBAL: FACEMAKER 0x00412968
int g_keyboard_init_done;

// GLOBAL: FACEMAKER 0x00417188
BITMAPINFO *g_scanline_bitmap_info;

// GLOBAL: FACEMAKER 0x00417998
int g_scanline_bitmap_info_initialized;

// GLOBAL: FACEMAKER 0x0040d088
// GLOBAL: SHANDALAR 0x005a1608
int g_frontbuffer_direct_blit_enabled = 1;

// GLOBAL: FACEMAKER 0x00417180
// GLOBAL: SHANDALAR 0x00737fe0
BITMAPINFO *g_blit_bitmap_info;

// GLOBAL: FACEMAKER 0x0041799c
// GLOBAL: SHANDALAR 0x007387fc
int g_blit_bitmap_info_initialized;

// GLOBAL: FACEMAKER 0x004181b0
// GLOBAL: SHANDALAR 0x00739010
char g_copy_scratch_buffer[0x804];

// GLOBAL: FACEMAKER 0x0040d094
// GLOBAL: SHANDALAR 0x005a1614
char *g_copy_scratch_buffer_ptr = g_copy_scratch_buffer;

// GLOBAL: SHANDALAR 0x00739008
BITMAPINFO *g_copy_bitmap_info;

// GLOBAL: SHANDALAR 0x007357d0
int g_copy_bitmap_info_initialized;

// GLOBAL: SHANDALAR 0x007357d8
char g_copy_flip_scratch_buffer[0x804];

// GLOBAL: SHANDALAR 0x005a1618
char *g_copy_flip_scratch_buffer_ptr = g_copy_flip_scratch_buffer;

// GLOBAL: SHANDALAR 0x00737ff8
char g_copy_restore_scratch_buffer[0x804];

// GLOBAL: SHANDALAR 0x005a161c
char *g_copy_restore_scratch_buffer_ptr = g_copy_restore_scratch_buffer;

// GLOBAL: FACEMAKER 0x004189b4
// GLOBAL: SHANDALAR 0x00739814
int g_graphics_initialized;

// GLOBAL: FACEMAKER 0x00420df0
// GLOBAL: SHANDALAR 0x00981220
RpBitsPalettePacket g_palette_transition_source_words;

// GLOBAL: FACEMAKER 0x00425e10
// GLOBAL: SHANDALAR 0x00986220
HPALETTE g_palette_handle;

// GLOBAL: FACEMAKER 0x00425e20
// GLOBAL: SHANDALAR 0x00986230
RpBitsPalettePacket g_palette_data_words;

// GLOBAL: FACEMAKER 0x00426140
// GLOBAL: SHANDALAR 0x00986550
PALETTEENTRY g_palette_entries[256];

// GLOBAL: FACEMAKER 0x00426540
// GLOBAL: SHANDALAR 0x00986950
DIBSurface *g_graphics_pages[10];

// GLOBAL: FACEMAKER 0x00426568
// GLOBAL: SHANDALAR 0x00986978
int g_graphics_height;

// GLOBAL: FACEMAKER 0x0042656c
// GLOBAL: SHANDALAR 0x0098697c
int g_graphics_width;

// GLOBAL: FACEMAKER 0x00426570
// GLOBAL: SHANDALAR 0x00986980
int g_graphics_bpp;

// GLOBAL: FACEMAKER 0x00426580
// GLOBAL: SHANDALAR 0x00986990
RGBQUAD g_palette_rgb[256];

// GLOBAL: FACEMAKER 0x00426980
// GLOBAL: SHANDALAR 0x00986d90
LOGPALETTE *g_palette_layout;

// GLOBAL: FACEMAKER 0x00421110
// GLOBAL: SHANDALAR 0x00981540
int g_palette_transition_work_words[0x400];

// GLOBAL: FACEMAKER 0x0041f5f0
// GLOBAL: SHANDALAR 0x0097fa20
int g_palette_transition_hsv[0x301];

// GLOBAL: FACEMAKER 0x004201f4
// GLOBAL: SHANDALAR 0x00980624
int g_palette_transition_value_step[0x2ff];

// GLOBAL: FACEMAKER 0x0040d08c
// GLOBAL: SHANDALAR 0x005a160c
int g_graphics_internal_state = 0;

// GLOBAL: FACEMAKER 0x0040d2e4
// GLOBAL: SHANDALAR 0x005a1864
unsigned char *g_palette_rgb_bytes = g_palette_data_words.entry_data;

// GLOBAL: FACEMAKER 0x0041afb8
// GLOBAL: SHANDALAR 0x00748414
HPALETTE g_realized_palette_handle;

// Not optimized in shandalar interestingly
#ifdef FACEMAKER
#pragma intrinsic(memcpy)
#pragma optimize("gy", on)
#endif

// FUNCTION: SHANDALAR 0x0041cf20
// FUNCTION: FACEMAKER 0x004063f0
void QueueKeyInputFromMessage(WPARAM wparam, LPARAM lparam)
{
  int virtual_key;
  int modifier_mode = 0;
  if (g_keyboard_init_done == 0)
  {
    while (GetAsyncKeyState(VK_MENU) != 0)
    {
    }
    while (GetAsyncKeyState(VK_CONTROL) != 0)
    {
    }
    g_keyboard_init_done = 1;
  }

  if (g_key_input_queue_count == 49)
  {
    MessageBeep(-1);
    return;
  }

  virtual_key = (int)(((unsigned int)lparam & 0xff0000) >> 0x10);
  if (GetAsyncKeyState(VK_MENU))
  {
    modifier_mode = 3;
  }
  else if (GetAsyncKeyState(VK_CONTROL))
  {
    modifier_mode = 2;
  }
  else
  {
    if (isalpha(g_virtual_key_char_map[virtual_key].states[0].ch & 0xff))
    {
      modifier_mode = (GetAsyncKeyState(VK_CAPITAL) != 0) ^ (GetAsyncKeyState(VK_SHIFT) != 0);
    }
    else if ((virtual_key >= 'G') && (virtual_key <= 'S'))
    {
      modifier_mode = (GetAsyncKeyState(VK_NUMLOCK) != 0) ^ (GetAsyncKeyState(VK_SHIFT) != 0);
    }
    else if (GetAsyncKeyState(VK_SHIFT))
    {
      modifier_mode = 1;
    }
  }

  if (g_virtual_key_char_map[virtual_key].states[modifier_mode].present)
  {
    virtual_key = g_virtual_key_char_map[virtual_key].states[modifier_mode].ch;
  }
  else
  {
    return;
  }

  {
    int repeat_count = lparam & 0xffff;
    repeat_count = MIN(50 - g_key_input_queue_count, repeat_count);

    while (repeat_count--)
    {
      g_key_input_queue[g_key_input_queue_count++] = virtual_key;
    }
  }
}

// FUNCTION: SHANDALAR 0x0041d169
// FUNCTION: FACEMAKER 0x00406590
int HasQueuedKeyInput(void)
{
  return g_key_input_queue_count != 0;
}

// FUNCTION: SHANDALAR 0x0041d192
// FUNCTION: FACEMAKER 0x004065a0
int PopQueuedKeyInput(void)
{
  int queued_key;
  if (g_key_input_queue_count == 0)
  {
    return 0;
  }

  queued_key = g_key_input_queue[0];
  if (--g_key_input_queue_count != 0)
  {
    memcpy(g_key_input_queue, g_key_input_queue + 1, g_key_input_queue_count * 4);
  }
  return queued_key;
}

#pragma optimize("gy", on)
#pragma intrinsic(memset)
#pragma intrinsic(memcpy)

// FUNCTION: SHANDALAR 0x00578c90
// FUNCTION: FACEMAKER 0x004065f0
int LoadFontConfigIfPresent(char *executable_name, char *config_name)
{
  (void)executable_name;
  if (config_name != (char *)0)
  {
    return LoadFontCollection(config_name);
  }
  return 0;
}

// FUNCTION: FACEMAKER 0x004065e0
int LegacyInitNoop()
{
  return 0;
}

// FUNCTION: FACEMAKER 0x00406610
DIBSurface *GetPrimaryPage(int unused)
{
  return InitializeGraphicsSystemDefaultMode();
}

// FUNCTION: FACEMAKER 0x00406620
DIBSurface *GetPageByNumber(int page_number)
{
  if (page_number == 0)
  {
    return InitializeGraphicsSystemDefaultMode();
  }
  return CreateGraphicsPage(page_number, g_graphics_width, g_graphics_height, 8);
}

// FUNCTION: SHANDALAR 0x00578cc0
// FUNCTION: FACEMAKER 0x00406650
DIBSurface *InitializeGraphicsSystemDefaultMode(void)
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

  if (g_graphics_initialized != 0)
  {
    return g_graphics_pages[0];
  }

  surface = (DIBSurface *)malloc(0x30);
#ifdef MODERN_FIXES
  width = global_screen_width;
#else
  width = GetDeviceCaps(global_main_hdc, HORZRES);
#endif
  g_graphics_width = width;
  surface->width = width;
#ifdef MODERN_FIXES
  height = global_screen_height;
#else
  height = GetDeviceCaps(global_main_hdc, VERTRES);
#endif
  g_graphics_height = height;
  surface->height = height;
  bits_per_pixel_ptr = &bits_per_pixel_stack;
  *bits_per_pixel_ptr = GetDeviceCaps(global_main_hdc, BITSPIXEL);
  g_graphics_bpp = *bits_per_pixel_ptr;
  surface->bitsPerPixel = *bits_per_pixel_ptr;
  g_graphics_internal_state = 0;

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
  g_palette_layout = log_palette;

  palette_entry = g_palette_entries;
  do
  {
    palette_entry->peRed = 0;
    palette_entry->peGreen = 0;
    palette_entry->peBlue = 0;
    palette_entry->peFlags = 1;
    ++palette_entry;
  } while (palette_entry < (PALETTEENTRY *)g_graphics_pages);
  g_palette_entries[0].peFlags = 0;
  g_palette_entries[255].peFlags = 0;

  palette_handle = CreatePalette(log_palette);
  g_palette_handle = palette_handle;
  g_realized_palette_handle = palette_handle;
  surface->hPalette = palette_handle;
  SelectPalette(surface->hTempDC, palette_handle, FALSE);
  RealizePalette(surface->hTempDC);
  SetStretchBltMode(surface->hTempDC, 3);
  g_graphics_pages[0] = surface;
  g_graphics_initialized = 1;
  return surface;
}

// FUNCTION: SHANDALAR 0x00578e80
// FUNCTION: FACEMAKER 0x00406810
DIBSurface *InitializeGraphicsSystem(int width, int height, int bits_per_pixel)
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

  if (g_graphics_initialized != 0)
  {
    return g_graphics_pages[0];
  }

  surface = (DIBSurface *)malloc(0x30);
  g_graphics_width = width;
  surface->width = width;
  g_graphics_height = height;
  surface->height = height;
  if (bits_per_pixel == -1)
  {
    screen_dc = GetDC((HWND)0);
    bits_per_pixel = GetDeviceCaps(screen_dc, BITSPIXEL);
    g_graphics_bpp = bits_per_pixel;
    surface->bitsPerPixel = bits_per_pixel;
    ReleaseDC((HWND)0, screen_dc);
  }
  else
  {
    g_graphics_bpp = bits_per_pixel;
    surface->bitsPerPixel = bits_per_pixel;
  }

  image_size_bytes = bits_per_pixel * height * width;
  g_graphics_internal_state = 0;
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
  g_palette_layout = log_palette;

  palette_entry = g_palette_entries;
  do
  {
    palette_entry->peRed = 0;
    palette_entry->peGreen = 0;
    palette_entry->peBlue = 0;
    palette_entry->peFlags = 1;
    ++palette_entry;
  } while (palette_entry < (PALETTEENTRY *)g_graphics_pages);
  g_palette_entries[0].peFlags = 0;
  g_palette_entries[255].peFlags = 0;

  palette_handle = CreatePalette(log_palette);
  g_palette_handle = palette_handle;
  g_realized_palette_handle = palette_handle;
  surface->hPalette = palette_handle;
  SelectPalette(surface->hTempDC, palette_handle, FALSE);
  RealizePalette(surface->hTempDC);
  SetStretchBltMode(surface->hTempDC, 3);
  g_graphics_pages[0] = surface;
  g_graphics_initialized = 1;
  return surface;
}

// FUNCTION: SHANDALAR 0x00579050
// FUNCTION: FACEMAKER 0x004069e0
DIBSurface *CreateGraphicsPage(int page_number, int width, int height, int bits_per_pixel)
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
    return InitializeGraphicsSystem(width, height, bits_per_pixel);
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

  _itoa(page_number, mapping_name + 6, 10);
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
  page->hPalette = g_palette_handle;
  SelectPalette(page->hTempDC, page->hPalette, FALSE);
  RealizePalette(page->hTempDC);
  SetStretchBltMode(page->hTempDC, 3);

  memset(page->pBits, 0, image_size_bytes);
  return page;
}

// FUNCTION: SHANDALAR 0x00579240
int FreeGraphicsPage(int page_number)
{
  DIBSurface *page;
  HGDIOBJ stock_bitmap;

  assert((unsigned int)(page_number != 0), s_D__NewMagic__sources__sidlib__lib_c_0040d0ec, 0x156,
         s_Cannot_explicitly_Deallocate_page_0_0040d140);
  assert((unsigned int)(page_number < 10), s_D__NewMagic__sources__sidlib__lib_c_0040d0ec, 0x157,
         s_Graphic_Page_number_out_of_range_0040d110, page_number);
  page = g_graphics_pages[page_number];
  if (page == (DIBSurface *)0)
  {
    return 0;
  }
  SelectObject(page->hTempDC, page->hPreviousBitmap);
  DeleteObject(page->hBitmap);
  free(page->pBitmapInfo);
  CloseHandle(page->hMapping);
  stock_bitmap = GetStockObject(0xf);
  SelectObject(page->hTempDC, stock_bitmap);
  RealizePalette(page->hTempDC);
  DeleteDC(page->hTempDC);
  free(page);
  g_graphics_pages[page_number] = (DIBSurface *)0;
  return 0;
}

// FUNCTION: SHANDALAR 0x00579310
// FUNCTION: FACEMAKER 0x00406bd0
void SetGraphicsPage(int page_number, DIBSurface *page)
{
  if (page_number != 0)
  {
    if (g_graphics_pages[page_number] != (DIBSurface *)0)
    {
      assert((unsigned int)(page_number >= 1), s_D__NewMagic__sources__sidlib__lib_c_0040d0ec, 0x156,
             s_Cannot_explicitly_Deallocate_page_0_0040d140);
      assert((unsigned int)(page_number < 10), s_D__NewMagic__sources__sidlib__lib_c_0040d0ec, 0x157,
             s_Graphic_Page_number_out_of_range_0040d110, page_number);

      if (g_graphics_pages[page_number] != (DIBSurface *)0)
      {
        SelectObject(g_graphics_pages[page_number]->hTempDC, g_graphics_pages[page_number]->hPreviousBitmap);
        DeleteObject(g_graphics_pages[page_number]->hBitmap);
        free(g_graphics_pages[page_number]->pBitmapInfo);
        CloseHandle(g_graphics_pages[page_number]->hMapping);
        SelectObject(g_graphics_pages[page_number]->hTempDC, GetStockObject(15));
        RealizePalette(g_graphics_pages[page_number]->hTempDC);
        DeleteDC(g_graphics_pages[page_number]->hTempDC);
        free(g_graphics_pages[page_number]);
        g_graphics_pages[page_number] = (DIBSurface *)0;
      }
    }
  }
  g_graphics_pages[page_number] = page;
}

// FUNCTION: SHANDALAR 0x00579760
// FUNCTION: FACEMAKER 0x00407190
void PutGraphicsPixel(FacemakerWindowBounds *window_bounds, int x, int y, int color_index)
{
  DIBSurface *page;
  COLORREF color;
  int value;

  page = g_graphics_pages[window_bounds->page_number];
  value = color_index;

  if (value < 0)
  {
    value = -value;

    if (value == 0x00ffffff)
    {
      color = RGB(0xff, 0xff, 0xff);
    }
    else
    {
      color = PALETTERGB((BYTE)(value >> 8),
                         (BYTE)value,
                         (BYTE)(value >> 16));
    }
  }
  else
  {
    if (value == 0xff)
    {
      color = RGB(0xff, 0xff, 0xff);
    }
    else
    {
      color = PALETTEINDEX(value);
    }
  }

  SetPixelV(page->hTempDC, x, y, color);
}

// FUNCTION: SHANDALAR 0x00579400
// FUNCTION: FACEMAKER 0x00406cc0
void PresentGraphicsPage(int num)
{
  DIBSurface *page0 = g_graphics_pages[0];
  HDC src_dc = g_graphics_pages[num]->hTempDC;

  if (src_dc != (HDC)0 && page0->hTempDC != (HDC)0)
  {
    BitBlt(page0->hTempDC, 0, 0, page0->width, page0->height,
           src_dc, 0, 0, SRCCOPY);
  }
}

// FUNCTION: SHANDALAR 0x00579480
// FUNCTION: FACEMAKER 0x00406d00
int ClearGraphicsPageWithPaletteColor(int page_number, int color_index)
{
  DIBSurface *page;
  HBRUSH brush;
  RECT rect;
  LOGBRUSH brush_desc;
  COLORREF color;

  brush_desc.lbStyle = 0;
  page = g_graphics_pages[page_number];
  color = (COLORREF)((unsigned int)g_palette_entries[color_index].peRed |
                     ((unsigned int)g_palette_entries[color_index].peGreen << 8) |
                     ((unsigned int)g_palette_entries[color_index].peBlue << 16));
  brush_desc.lbColor = color;
  brush_desc.lbHatch = 0;
  brush = CreateBrushIndirect(&brush_desc);
  rect.top = 0;
  rect.left = 0;
  rect.right = page->width;
  rect.bottom = page->height;
  FillRect(page->hTempDC, &rect, brush);
  return DeleteObject(brush);
}

// FUNCTION: SHANDALAR 0x00579890
// FUNCTION: FACEMAKER 0x00407210
void BlitGraphicsRect(FacemakerWindowBounds *dst, unsigned int dst_x, int dst_y, unsigned int width, DWORD height,
                      FacemakerWindowBounds *src, int src_x, int src_y)
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
  double *top_row;
  double *bottom_row;
  int half_height;

  src_page_number = dst->page_number;
  dst_page_number = src->page_number;
  src_page = g_graphics_pages[src_page_number];
  dst_page = g_graphics_pages[dst_page_number];

  if (g_blit_bitmap_info_initialized == 0)
  {
    g_blit_bitmap_info = CreateBitmapInfo(1, 1, 8);
    g_blit_bitmap_info_initialized = 1;
  }

  g_blit_bitmap_info->bmiHeader.biWidth = src_page->width;
  if (dst_y == 0)
  {
    g_blit_bitmap_info->bmiHeader.biHeight = src_page->height;
  }
  else
  {
    g_blit_bitmap_info->bmiHeader.biHeight = src_page->height;
  }

  if (dst_page_number == 0)
  {
    if (((dst_x & 7) == 0) && g_frontbuffer_direct_blit_enabled != 0)
    {
      if (g_graphics_bpp != 8)
      {
        memcpy(g_blit_bitmap_info->bmiColors, g_palette_rgb, 0x400);
      }

      top_row = (double *)(dst_y * src_page->width + dst_x + (int)src_page->pBits);
      bottom_row = (double *)((dst_y + height - 1) * src_page->width + dst_x + (int)src_page->pBits);
      half_height = (int)height / 2;
      while (half_height > 0)
      {
        CopyBytesAsmCompat((double *)g_copy_scratch_buffer_ptr, top_row, width);
        CopyBytesAsmCompat(top_row, bottom_row, width);
        CopyBytesAsmCompat(bottom_row, (double *)g_copy_scratch_buffer_ptr, width);
        top_row = (double *)((int)top_row + src_page->width);
        bottom_row = (double *)((int)bottom_row - src_page->width);
        half_height--;
      }

      SetDIBitsToDevice(dst_page->hTempDC, src_x, src_y, width, height, dst_x, dst_y, 0, src_page->height,
                        src_page->pBits, g_blit_bitmap_info, (unsigned int)(g_graphics_bpp == 8));
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

// FUNCTION: SHANDALAR 0x00579e40
// FUNCTION: FACEMAKER 0x00407570
void StretchBlitGraphicsRect(FacemakerWindowBounds *dst, int dst_x, int dst_y,
                             int src_w, int src_h,
                             FacemakerWindowBounds *src, int src_x, int src_y,
                             int copy_w, int copy_h)
{
  DIBSurface *dst_page = g_graphics_pages[dst->page_number];
  DIBSurface *src_page = g_graphics_pages[src->page_number];
  HDC dst_dc = dst_page->hTempDC;
  HDC src_dc = src_page->hTempDC;

  StretchBlt(src_dc, src_x, src_y, copy_w, copy_h,
             dst_dc, dst_x, dst_y, src_w, src_h, SRCCOPY);
}

// FUNCTION: SHANDALAR 0x00579f10
// FUNCTION: FACEMAKER 0x004075d0
void WriteGraphicsScanline(unsigned int *scanline_data, int page_number, int dst_x, int dst_y,
                           unsigned int byte_count)
{
  DIBSurface *page;
  int palette_index;
  void *dst_ptr;

  if (g_scanline_bitmap_info_initialized == 0)
  {
    g_scanline_bitmap_info = (BITMAPINFO *)CreateBitmapInfo(1, 1, 8);
    g_scanline_bitmap_info_initialized = 1;
  }
  page = g_graphics_pages[page_number];
  g_scanline_bitmap_info->bmiHeader.biWidth = byte_count;
  if (page_number == 0)
  {
    if ((g_graphics_bpp != 8) && (g_scanline_palette_needs_refresh != 0))
    {
      memcpy(g_scanline_bitmap_info->bmiColors, g_palette_rgb, 0x400);
 
      g_scanline_palette_needs_refresh = 0;
    }
    SetDIBitsToDevice(page->hTempDC, dst_x, dst_y, byte_count, 1, 0, 0, 0, 1, scanline_data, g_scanline_bitmap_info,
                      (unsigned int)(g_graphics_bpp == 8));
    return;
  }
  dst_ptr = (char *)page->pBits + dst_x + (page->width + page->rowPadding) * dst_y;
  memcpy(dst_ptr, scanline_data, byte_count);
}

// FUNCTION: SHANDALAR 0x00579520
// FUNCTION: FACEMAKER 0x00406da0
unsigned int ReadGraphicsPixel(int param_1, int param_2, int param_3)
{
  DIBSurface *page;
  COLORREF CVar2;
  unsigned int uVar3;
  unsigned char *pbVar4;
  unsigned int uVar5;

  page = g_graphics_pages[param_1];
  if (param_1 == 0)
  {
    CVar2 = GetPixel(global_main_hdc, param_2, param_3);
    pbVar4 = (unsigned char *)&g_palette_entries;
    uVar3 = 0;
    uVar5 = (-(unsigned int)(g_graphics_bpp == 0x10) & 0xfffffff9) + 0xff;
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
    uVar3 = (unsigned int)*((unsigned char *)page->pBits + (page->rowPadding + page->width) * param_3 + param_2);
  }
  return uVar3;
}

// FUNCTION: SHANDALAR 0x0057a000
// FUNCTION: FACEMAKER 0x004076c0
void ReadGraphicsScanline(unsigned int *out_scanline, int page_number, int src_x, int src_y,
                          unsigned int byte_count)
{
  DIBSurface *page;
  void *src_ptr;

  assert((unsigned int)(page_number != 0), s_D__NewMagic__sources__sidlib__lib_c_0040d0ec, 0x503,
         s_GetLine_not_implemented_for_page_0040d168);
  page = g_graphics_pages[page_number];
  src_ptr = (char *)page->pBits + (page->rowPadding + page->width) * src_y + src_x;
  memcpy(out_scanline, src_ptr, byte_count);
}

// FUNCTION: SHANDALAR 0x0057da90
// FUNCTION: FACEMAKER 0x0040a1a0
LRESULT CALLBACK PaletteClassWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  short column;
  short row;
  HWND palette_hwnd;
  HDC desktop_hdc;
  HDC palette_hdc;
  HPEN pen_handle;
  HGDIOBJ old_pen_handle;
  int x;
  int y;
  int row_bottom;
  HWND hWndChildAfter;
  LPCSTR lpszClass;
  LPCSTR lpszWindow;
  LOGPEN pen;

  if (msg != WM_PAINT)
    return DefWindowProcA(hwnd, msg, wparam, lparam);

  DefWindowProcA(hwnd, msg, wparam, lparam);
  lpszWindow = (LPCSTR)0;
  lpszClass = "ShowPaletteClass";
  hWndChildAfter = (HWND)0;

  palette_hwnd = FindWindowExA(GetParent(hwnd), hWndChildAfter, lpszClass, lpszWindow);
  ShowWindow(palette_hwnd, SW_SHOW);
  ShowWindow(palette_hwnd, SW_SHOW);
  desktop_hdc = GetDC((HWND)0);
  palette_hdc = GetDC(palette_hwnd);
  SelectPalette(palette_hdc, g_palette_handle, FALSE);
  row_bottom = 0x10;
  RealizePalette(palette_hdc);
  row = 0;
  do
  {
    x = 0;
    column = 0;
    do
    {
      pen.lopnWidth.x = 0;
      pen.lopnColor = (unsigned short)(row * 0x10 + column) | 0x1000000;
      pen.lopnWidth.y = 0;
      pen.lopnStyle = 0;
      pen_handle = CreatePenIndirect(&pen);
      old_pen_handle = SelectObject(palette_hdc, pen_handle);
      DeleteObject(old_pen_handle);
      for (y = row_bottom - 0x10; y < row_bottom; y = y + 1)
      {
        MoveToEx(palette_hdc, x, y, (LPPOINT)0);
        LineTo(palette_hdc, x + 0x10, y);
      }
      x = x + 0x10;
      column = column + 1;
    } while (x < 0x100);
    row_bottom = row_bottom + 0x10;
    row = row + 1;
  } while (row_bottom < 0x110);
  ReleaseDC((HWND)0, desktop_hdc);
  ReleaseDC(palette_hwnd, palette_hdc);
  return 0;
}

// FUNCTION: SHANDALAR 0x0057dc20
// FUNCTION: FACEMAKER 0x0040a330
ATOM RegisterPaletteClass(HINSTANCE hInstance)
{
  WNDCLASSA wndclass;

  wndclass.style = 0x20;
  wndclass.hInstance = hInstance;
  wndclass.lpfnWndProc = PaletteClassWndProc;
  wndclass.cbClsExtra = 0;
  wndclass.cbWndExtra = 0;
  wndclass.hIcon = LoadIconA((HINSTANCE)0, (LPCSTR)0x7F00);
  wndclass.hCursor = LoadCursorA((HINSTANCE)0, (LPCSTR)0x7F00);
  wndclass.hbrBackground = CreateSolidBrush(0);
  wndclass.lpszMenuName = (LPCSTR)0;
  wndclass.lpszClassName = "ShowPaletteClass";

  return RegisterClassA(&wndclass);
}

// FUNCTION: SHANDALAR 0x0057dcb0
// FUNCTION: FACEMAKER 0x0040a3c0
HWND CreatePalettePopupWindow(HINSTANCE hInstance, HWND parent_hwnd)
{
  RECT rect;
  rect.left = 0;
  rect.right = 0x100;
  rect.top = 0;
  rect.bottom = 0x100;

  AdjustWindowRect(&rect, WS_CAPTION | WS_SYSMENU | WS_THICKFRAME, FALSE);

  return CreateWindowExA(0, "ShowPaletteClass", "Current Palette", WS_POPUP | WS_CAPTION | WS_SYSMENU,
                         100, 50,
                         rect.right - rect.left,
                         rect.bottom - rect.top,
                         parent_hwnd, (HMENU)0, hInstance, (LPVOID)0);
}

typedef struct
{
  int r;
  int g;
  int b;
  int a_or_unused; // this 4th field is suspicious in your snippet
} RGBLike;

typedef struct
{
  int hue;        // 0..0x59ff, maybe -1 for "undefined"
  int saturation; // 0..0x1000
  int value;      // appears to be scaled by 64
} HSVLike;

static __inline int div64_round_toward_zero(int x)
{
  // matches: mov ecx,eax / sar ecx,31 / and ecx,0x3f / add ecx,eax / sar ecx,6
  return (x + ((x >> 31) & 0x3f)) >> 6;
}

// FUNCTION: SHANDALAR 0x0057c890
// FUNCTION: FACEMAKER 0x0040a440
void ConvertRgbToHsv(HSVLike *out, RGBLike *rgb)
{
  RGBLike self_copy;
  int minv;
  int maxv;
  int delta;
  int sat;
  int hue;
  int r;
  int g;
  int b;

  self_copy = *rgb;

  b = self_copy.b;
  g = self_copy.g;

  minv = g;
  if (b < g)
  {
    minv = b;
  }
  r = self_copy.r;
  if (r < minv)
  {
    minv = r;
  }

  maxv = g;
  if (b > g)
  {
    maxv = b;
  }
  if (r > maxv)
  {
    maxv = r;
  }

  if (maxv == 0)
  {
    out->hue = -1;
    out->saturation = 0;
    out->value = 0;
    return;
  }

  delta = maxv - minv;
  sat = (delta << 12) / maxv;

  if (delta == 0)
  {
    out->hue = 0;
    out->saturation = sat;
    out->value = maxv << 6;
    return;
  }

  if (r == maxv)
  {
    hue = ((g - b) * 0x0f00) / delta;
  }
  else if (g == maxv)
  {
    hue = ((b - r) * 0x0f00) / delta + 0x1e00;
  }
  else
  {
    hue = ((r - g) * 0x0f00) / delta + 0x3c00;
  }

  if (hue < 0)
  {
    hue += 0x5a00;
  }

  out->hue = hue;
  out->saturation = sat;
  out->value = maxv << 6;
  return;
}

// FUNCTION: SHANDALAR 0x0057c9f0
// FUNCTION: FACEMAKER 0x0040a5a0
RGBLike *ConvertHsvToRgb(HSVLike *self, RGBLike *out)
{
  RGBLike tmp;
  int h, s, v;
  int p, q, t;
  int v6;

  s = self->saturation;

  // no saturation, or "undefined hue" sentinel -> grayscale
  if (s == 0)
  {
    if (self->hue == -1)
    {
      v6 = div64_round_toward_zero(self->value);

      tmp.r = v6;
      tmp.g = v6;
      tmp.b = v6;
      *out = tmp;
      return out;
    }
  }

  h = self->hue;

  // normalize one full turn sentinel?
  if (h == 0x5a00)
  {
    self->hue = 0;
    h = 0;
  }

  v6 = div64_round_toward_zero(self->value);

  // p = v * (1 - s)
  p = ((0x1000 - s) * v6 + ((((0x1000 - s) * v6) >> 31) & 0xfff)) >> 12;

  // remainder inside current hue sector
  {
    int h_rem = h % 0x0f00;

    // q = v * (1 - s * f)
    q = ((0x0f00000 - h_rem * s) * v6) / 0x0f00000;

    // t = v * (1 - s * (1 - f))
    t = ((((h_rem - 0x0f00) * s) + 0x0f00000) * v6) / 0x0f00000;
  }

  switch (h / 0x0f00)
  {
  default:
    tmp.r = 0;
    tmp.g = 0;
    tmp.b = 0;
    break;

  case 0:
    tmp.r = v6;
    tmp.g = t;
    tmp.b = p;
    break;

  case 1:
    tmp.r = q;
    tmp.g = v6;
    tmp.b = p;
    break;

  case 2:
    tmp.r = p;
    tmp.g = v6;
    tmp.b = t;
    break;

  case 3:
    tmp.r = p;
    tmp.g = q;
    tmp.b = v6;
    break;

  case 4:
    tmp.r = t;
    tmp.g = p;
    tmp.b = v6;
    break;

  case 5:
    tmp.r = v6;
    tmp.g = p;
    tmp.b = q;
    break;
  }

  *out = tmp;
  return out;
}

// FUNCTION: SHANDALAR 0x0057cb80
// FUNCTION: FACEMAKER 0x0040a730
int AnimatePaletteToColor(int gray, int steps)
{
  int value_step;
  int i;
  int frame;
  HSVLike target_hsv;
  RGBLike target_rgb;
  RGBLike rgb;
  HSVLike hsv;

  value_step = 0x4000 / steps;

  if (g_graphics_bpp != 8)
  {
    return 0;
  }

  memcpy(&g_palette_transition_source_words,
         &g_palette_data_words,
         0xC0 * sizeof(unsigned int));

  target_rgb.r = gray;
  target_rgb.g = gray;
  target_rgb.b = gray;
  ConvertRgbToHsv(&target_hsv, &target_rgb);

  RpBits_ApplyPalette(&g_palette_data_words);

  for (i = 0; i < 0x100; ++i)
  {
    int sat_step;
    int entry_offset;

    entry_offset = i * 3;

    rgb.r = (unsigned char)g_palette_rgb_bytes[entry_offset + 0];
    rgb.g = (unsigned char)g_palette_rgb_bytes[entry_offset + 1];
    rgb.b = (unsigned char)g_palette_rgb_bytes[entry_offset + 2];

    ConvertRgbToHsv(&hsv, &rgb);

    g_palette_transition_hsv[entry_offset + 0] = hsv.hue;
    g_palette_transition_hsv[entry_offset + 1] = hsv.saturation;
    g_palette_transition_hsv[entry_offset + 2] = hsv.value;

    sat_step = (target_hsv.saturation - hsv.saturation) / steps;
    g_palette_transition_value_step[entry_offset] = sat_step;

    if (target_hsv.saturation > hsv.saturation)
    {
      sat_step += 0x1000 / steps;
    }
    else
    {
      sat_step += -0x1000 / steps;
    }

    g_palette_transition_value_step[entry_offset] = sat_step;
  }

  if (steps > 0)
  {
    for (frame = 1; frame <= steps; ++frame)
    {
      int rgb_off;
      int work_off;
      int hsv_off;
      int step_off;

      rgb_off = 0;
      work_off = 0;
      hsv_off = 2; /* indexes the value field; -2 hue, -1 sat, 0 value */
      step_off = 0;

      while (1)
      {
        hsv.hue = g_palette_transition_hsv[hsv_off - 2];

        if (target_hsv.saturation == 0)
        {
          hsv.saturation = g_palette_transition_hsv[hsv_off - 1];
          hsv.value = g_palette_transition_hsv[hsv_off] - value_step;
          g_palette_transition_hsv[hsv_off] = hsv.value;
        }
        else
        {
          hsv.saturation =
              g_palette_transition_hsv[hsv_off - 1] +
              g_palette_transition_value_step[step_off] * frame;

          if (hsv.saturation > 0xFBF)
          {
            hsv.saturation = 0xFC0;
          }

          hsv.value = g_palette_transition_hsv[hsv_off];

          if (hsv.value > target_hsv.value)
          {
            hsv.value -= value_step;
          }
          else
          {
            hsv.value += value_step;
          }

          if (hsv.value > 0x3FBF)
          {
            hsv.value = 0x3FC0;
          }
          if (hsv.value < 0)
          {
            hsv.value = 0;
          }

          g_palette_transition_hsv[hsv_off - 1] = hsv.saturation;
          g_palette_transition_hsv[hsv_off] = hsv.value;
        }

        (void)ConvertHsvToRgb(&hsv, &rgb);

        g_palette_transition_work_words[work_off + 0] = rgb.r;
        g_palette_transition_work_words[work_off + 1] = rgb.g;
        g_palette_transition_work_words[work_off + 2] = rgb.b;
        g_palette_transition_work_words[work_off + 3] = rgb.a_or_unused;

        g_palette_rgb_bytes[rgb_off + 0] =
            (unsigned char)g_palette_transition_work_words[work_off + 0];
        g_palette_rgb_bytes[rgb_off + 1] =
            (unsigned char)g_palette_transition_work_words[work_off + 1];
        g_palette_rgb_bytes[rgb_off + 2] =
            (unsigned char)g_palette_transition_work_words[work_off + 2];

        step_off += 3;
        hsv_off += 3;
        work_off += 4;
        rgb_off += 3;

        if (work_off >= 0x400)
        {
          break;
        }
      }

      RpBits_ApplyPalette(&g_palette_data_words);
    }
  }

  RpBits_ApplyPalette(&g_palette_data_words);
  return ClearGraphicsPageWithPaletteColor(0, 0);
}

// FUNCTION: SHANDALAR 0x00566cea
// FUNCTION: FACEMAKER 0x0040aa20
void CopyBytesAsmCompat(double *dst, double *src, unsigned int num)
{
#ifdef MODERN_FIXES
  memcpy(dst, src, num);
#else
  // TODO: this looks like real inline asm but who knows
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
