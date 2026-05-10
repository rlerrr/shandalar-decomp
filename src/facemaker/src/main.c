#include <process.h>
#include <windows.h>
#include <direct.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cardartlib/src/assert.h"
#include "defs.h"

typedef struct FacemakerWindowBounds
{
  char pad_0[12];
  int max_x;
  int max_y;
} FacemakerWindowBounds;

typedef struct FacemakerSurface
{
  HANDLE mapping_handle;
  HDC hdc;
  HBITMAP bitmap;
  HGDIOBJ previous_bitmap;
  BITMAPINFO *bitmap_info;
  HPALETTE palette;
  void *bits;
  int image_size_bytes;
  int width;
  int height;
  int bits_per_pixel;
  int row_padding;
} FacemakerSurface;

typedef struct FacemakerSeedString
{
  DWORD head;
  WORD mid;
  char tail;
} FacemakerSeedString;

extern FacemakerSurface *__cdecl FUN_00406810(int width, int height, int bits_per_pixel);
extern FacemakerSurface *__cdecl FUN_004069e0(int page_number, int width, int height, int bits_per_pixel);
extern unsigned int __cdecl FUN_00402600(int value);
extern void __cdecl FUN_00402621(int *dst, int dst_x, int dst_y, int width, int height, int *src,
                                 int src_x, int src_y);
extern void __cdecl FUN_0040268a(int *dst, int x, int y, int src_page);
extern void __cdecl FUN_00407e20(int font_id, unsigned int point_size, char *font_file, char *font_name,
                                 int weight, DWORD italic);
extern void __cdecl FUN_00406cc0(int enabled);
extern int __cdecl FUN_004093d0(void);
extern void __cdecl FUN_0040a110(int page_number, char *path);
extern void __cdecl FUN_00407570(void *dst, int dst_x, int dst_y, int src_w, int src_h, void *src,
                                 int src_x, int src_y, int copy_w, int copy_h);
extern void __cdecl FUN_0040369c(void);
extern int __cdecl FUN_00404d70(char *path, int counts_out, void *entries_out, int *first_entry_out);
extern void __cdecl FUN_00402a84(void *control);
extern void __cdecl FUN_00403573(int index, int selected);
extern void __cdecl FUN_004028d1(void *control, int mode);
extern void __cdecl FUN_004032e4(void);

// GLOBAL: FACEMAKER 0x0040c010
FacemakerWindowBounds DAT_0040c010;

// GLOBAL: FACEMAKER 0x0040c034
FacemakerWindowBounds *PTR_DAT_0040c034 = &DAT_0040c010;

// GLOBAL: FACEMAKER 0x0040c05c
FacemakerWindowBounds *PTR_DAT_0040c05c = &DAT_0040c010;

// GLOBAL: FACEMAKER 0x0040c0ac
FacemakerWindowBounds *PTR_DAT_0040c0ac = &DAT_0040c010;

// GLOBAL: FACEMAKER 0x0040c9e4
int DAT_0040c9e4 = 1;

// GLOBAL: FACEMAKER 0x0040c188
int DAT_0040c188;

// GLOBAL: FACEMAKER 0x0040c190
int DAT_0040c190;

// GLOBAL: FACEMAKER 0x0040c194
int DAT_0040c194;

// GLOBAL: FACEMAKER 0x0040c1c0
int DAT_0040c1c0;

// GLOBAL: FACEMAKER 0x0040c1c4
int DAT_0040c1c4;

// GLOBAL: FACEMAKER 0x0040c1e4
int DAT_0040c1e4;

// GLOBAL: FACEMAKER 0x0040c1e8
char DAT_0040c1e8[0x444];

// GLOBAL: FACEMAKER 0x0040c208
int DAT_0040c208;

// GLOBAL: FACEMAKER 0x0040c284
char DAT_0040c284[0x34];

// GLOBAL: FACEMAKER 0x0040c2b8
char DAT_0040c2b8[0x34];

// GLOBAL: FACEMAKER 0x0040c354
char DAT_0040c354[0x34];

// GLOBAL: FACEMAKER 0x0040c388
char DAT_0040c388[0x34];

// GLOBAL: FACEMAKER 0x0040d0a0
char s_rpbits_0040d0a0[] = "rpbits";

// GLOBAL: FACEMAKER 0x0040d0ec
char s_D__NewMagic__sources__sidlib__lib_c_0040d0ec[] = "D:\\NewMagic\\sources\\sidlib\\lib.c";

// GLOBAL: FACEMAKER 0x0040d138
char s_rpbips_0040d138[] = "rpbips";

// GLOBAL: FACEMAKER 0x0041afb4
HDC DAT_0041afb4;

// GLOBAL: FACEMAKER 0x0041ae40
char DAT_0041ae40[0x105];

// GLOBAL: FACEMAKER 0x0041b024
char DAT_0041b024[256];

// GLOBAL: FACEMAKER 0x0041b028
HWND DAT_0041b028;

// GLOBAL: FACEMAKER 0x0041b030
char DAT_0041b030[0x105];

// GLOBAL: FACEMAKER 0x0041c140
HINSTANCE DAT_0041c140;

// GLOBAL: FACEMAKER 0x0041c144
int DAT_0041c144;

// GLOBAL: FACEMAKER 0x0041d110
int global_screen_width;

// GLOBAL: FACEMAKER 0x0041d114
int global_screen_height;

// GLOBAL: FACEMAKER 0x0041d0f0
char DAT_0041d0f0[0x420];

// GLOBAL: FACEMAKER 0x0041e410
int DAT_0041e410;

// GLOBAL: FACEMAKER 0x0041e4a0
int DAT_0041e4a0;

// GLOBAL: FACEMAKER 0x0041e4dc
int DAT_0041e4dc;

// GLOBAL: FACEMAKER 0x0041e560
int DAT_0041e560;

// GLOBAL: FACEMAKER 0x0041e570
char *DAT_0041e570[1];

// GLOBAL: FACEMAKER 0x0041e0d0
char DAT_0041e0d0[0x340];

// GLOBAL: FACEMAKER 0x0041e564
int DAT_0041e564;

// GLOBAL: FACEMAKER 0x004189b4
int DAT_004189b4;

// GLOBAL: FACEMAKER 0x00425e10
HPALETTE DAT_00425e10;

// GLOBAL: FACEMAKER 0x00426140
PALETTEENTRY DAT_00426140[256];

// GLOBAL: FACEMAKER 0x00426540
FacemakerSurface *DAT_00426540[10];

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

// GLOBAL: FACEMAKER 0x0040d088
int DAT_0040d088;

// GLOBAL: FACEMAKER 0x0041afb8
HPALETTE DAT_0041afb8;

// FUNCTION: FACEMAKER 0x004065f0
int __cdecl FUN_004065f0(char *executable_name, char *config_name)
{
  (void)executable_name;
  (void)config_name;
  return 0;
}

// FUNCTION: FACEMAKER 0x004065e0
void __cdecl FUN_004065e0(int config_handle)
{
  (void)config_handle;
}

// FUNCTION: FACEMAKER 0x00406610
void __cdecl FUN_00406610(int unused)
{
  (void)unused;
}

// FUNCTION: FACEMAKER 0x00406620
FacemakerSurface *__cdecl FUN_00406620(int page_number)
{
  (void)page_number;
  return 0;
}

// FUNCTION: FACEMAKER 0x00406bd0
void __cdecl FUN_00406bd0(int page_number, FacemakerSurface *page)
{
  (void)page_number;
  (void)page;
}

// FUNCTION: FACEMAKER 0x00402600
unsigned int __cdecl FUN_00402600(int value)
{
  return (unsigned int)value;
}

// FUNCTION: FACEMAKER 0x00402621
void __cdecl FUN_00402621(int *dst, int dst_x, int dst_y, int width, int height, int *src, int src_x,
                          int src_y)
{
  (void)dst;
  (void)dst_x;
  (void)dst_y;
  (void)width;
  (void)height;
  (void)src;
  (void)src_x;
  (void)src_y;
}

// FUNCTION: FACEMAKER 0x0040268a
void __cdecl FUN_0040268a(int *dst, int x, int y, int src_page)
{
  (void)dst;
  (void)x;
  (void)y;
  (void)src_page;
}

// FUNCTION: FACEMAKER 0x00407e20
void __cdecl FUN_00407e20(int font_id, unsigned int point_size, char *font_file, char *font_name,
                          int weight, DWORD italic)
{
  (void)font_id;
  (void)point_size;
  (void)font_file;
  (void)font_name;
  (void)weight;
  (void)italic;
}

// FUNCTION: FACEMAKER 0x00406cc0
void __cdecl FUN_00406cc0(int enabled)
{
  (void)enabled;
}

// FUNCTION: FACEMAKER 0x004093d0
int __cdecl FUN_004093d0(void)
{
  return 0;
}

// FUNCTION: FACEMAKER 0x0040a110
void __cdecl FUN_0040a110(int page_number, char *path)
{
  (void)page_number;
  (void)path;
}

// FUNCTION: FACEMAKER 0x00407570
void __cdecl FUN_00407570(void *dst, int dst_x, int dst_y, int src_w, int src_h, void *src,
                          int src_x, int src_y, int copy_w, int copy_h)
{
  (void)dst;
  (void)dst_x;
  (void)dst_y;
  (void)src_w;
  (void)src_h;
  (void)src;
  (void)src_x;
  (void)src_y;
  (void)copy_w;
  (void)copy_h;
}

// FUNCTION: FACEMAKER 0x0040369c
void __cdecl FUN_0040369c(void)
{
}

// FUNCTION: FACEMAKER 0x00404d70
int __cdecl FUN_00404d70(char *path, int counts_out, void *entries_out, int *first_entry_out)
{
  (void)path;
  (void)counts_out;
  (void)entries_out;
  (void)first_entry_out;
  return 0;
}

// FUNCTION: FACEMAKER 0x00402a84
void __cdecl FUN_00402a84(void *control)
{
  (void)control;
}

// FUNCTION: FACEMAKER 0x004028d1
void __cdecl FUN_004028d1(void *control, int mode)
{
  (void)control;
  (void)mode;
}

// FUNCTION: FACEMAKER 0x004032e4
void __cdecl FUN_004032e4(void)
{
}

// FUNCTION: FACEMAKER 0x00403573
void __cdecl FUN_00403573(int index, int selected)
{
  (void)index;
  (void)selected;
}

// FUNCTION: FACEMAKER 0x00403fd9
DWORD WINAPI FUN_00403fd9(LPVOID unused)
{
  char *last_backslash;
  int page_count;
  int page_number;
  int face_index;
  int face_slot_x;
  int face_slot_y;
  int visible_face_count;
  int button_index;
  FacemakerSurface *page;
  unsigned int sign_bits;
  FILE *recent_face_file;
  char recent_face_name[256];
  char face_art_path[260];

  (void)unused;

  GetModuleFileNameA((HMODULE)0, DAT_0041ae40, 0x105);
  SetCurrentDirectoryA(DAT_0041ae40);
  last_backslash = strrchr(DAT_0041ae40, '\\');
  *last_backslash = '\0';
  sprintf(DAT_0041b030, "%s\\PlayFace\\*.pic", DAT_0041ae40);
  FUN_004065e0(FUN_004065f0("mgraphic.exe", "fonts.cv"));
  FUN_00406610(0);

  page_count = 3;
  for (page_number = 0; page_number < page_count; ++page_number)
  {
    if (page_number == 1 && DAT_00426540[0]->width < 800)
    {
      page = FUN_004069e0(page_number, 800, 600, 8);
    }
    else
    {
      page = FUN_00406620(page_number);
    }
    FUN_00406bd0(page_number, page);
  }

  PTR_DAT_0040c0ac->max_x = global_screen_width;
  PTR_DAT_0040c05c->max_x = PTR_DAT_0040c0ac->max_x;
  PTR_DAT_0040c034->max_x = PTR_DAT_0040c05c->max_x;
  PTR_DAT_0040c0ac->max_y = global_screen_height;
  PTR_DAT_0040c05c->max_y = PTR_DAT_0040c0ac->max_y;
  PTR_DAT_0040c034->max_y = PTR_DAT_0040c05c->max_y;
  DAT_0040d088 = 0;

  if (global_screen_width == 640)
  {
    FUN_00407e20(1, 0xb, "tt0300m_.ttf", "MPZurich Cn BT", 400, 0);
    FUN_00407e20(2, 9, "tt0298m_.ttf", "MPZurich Cn BT", 400, 0);
    FUN_00407e20(4, FUN_00402600(10), "tt0530m_.ttf", "Benguiat BkCn BT", 100, 0);
    FUN_00407e20(5, FUN_00402600(0x14), "tt0127m_.ttf", "Benguiat Bk BT", 700, 1);
    FUN_00407e20(6, FUN_00402600(0x1c), "tt0530m_.ttf", "Benguiat BkCn BT", 100, 0);
    FUN_00407e20(7, FUN_00402600(0xb), "tt0530m_.ttf", "Benguiat BkCn BT", 100, 0);
  }
  else if (global_screen_width == 800)
  {
    FUN_00407e20(1, FUN_00402600(8), "tt0530m_.ttf", "Benguiat BkCn BT", 100, 0);
    FUN_00407e20(2, 9, "tt0298m_.ttf", "MPZurich Cn BT", 400, 0);
    FUN_00407e20(4, FUN_00402600(10), "tt0530m_.ttf", "Benguiat BkCn BT", 100, 0);
    FUN_00407e20(5, FUN_00402600(0x14), "tt0127m_.ttf", "Benguiat Bk BT", 700, 1);
    FUN_00407e20(6, FUN_00402600(0x1c), "tt0530m_.ttf", "Benguiat BkCn BT", 100, 0);
    FUN_00407e20(7, FUN_00402600(0xb), "tt0530m_.ttf", "Benguiat BkCn BT", 100, 0);
  }
  else if (global_screen_width == 1024)
  {
    FUN_00407e20(1, FUN_00402600(9), "tt0530m_.ttf", "Benguiat BkCn BT", 100, 0);
    FUN_00407e20(2, 10, "tt0298m_.ttf", "MPZurich Cn BT", 400, 0);
    FUN_00407e20(4, FUN_00402600(0xb), "tt0530m_.ttf", "Benguiat BkCn BT", 100, 0);
    FUN_00407e20(5, FUN_00402600(0x14), "tt0127m_.ttf", "Benguiat Bk BT", 700, 1);
    FUN_00407e20(6, FUN_00402600(0x1c), "tt0530m_.ttf", "Benguiat BkCn BT", 100, 0);
    FUN_00407e20(7, FUN_00402600(0xb), "tt0530m_.ttf", "Benguiat BkCn BT", 100, 0);
  }

  *(int *)((char *)PTR_DAT_0040c034 + 0x20) = 1;
  FUN_00406cc0(0);
  DAT_0040c188 = FUN_004093d0();
  FUN_0040a110(2, "FaceArt/facebk0New.pcx");
  FUN_00407570(PTR_DAT_0040c0ac, 0, 0, 0x280, 0x1e0, PTR_DAT_0040c05c, 0, 0, global_screen_width,
               global_screen_height);
  FUN_0040369c();

  recent_face_file = fopen("FaceMostRecent.txt", "rt");
  if (recent_face_file != (FILE *)0)
  {
    fscanf(recent_face_file, "%d, %s", &DAT_0040c1e4, recent_face_name);
    if (_stricmp(DAT_0041e570[DAT_0040c1e4], recent_face_name) != 0)
    {
      DAT_0040c1e4 = 0;
    }
    fclose(recent_face_file);
  }

  strcpy(face_art_path, "FaceArt/");
  strcat(face_art_path, DAT_0041e570[DAT_0040c1e4]);
  DAT_0041e410 = FUN_00404d70(face_art_path, 0x41e530, &DAT_0041e0d0, (int *)DAT_0041d0f0);
  FUN_00402a84(&DAT_0040c354);
  if (DAT_0041e410 < 7)
  {
    FUN_00402a84(&DAT_0040c388);
  }

  for (face_index = 0; face_index < 6; face_index++)
  {
    sign_bits = (int)(face_index + 2U) >> 0x1f;
    face_slot_x = ((((face_index + 2U ^ sign_bits) - sign_bits & 3U) ^ sign_bits) - sign_bits) * 0x9f +
                  0xd;
    face_slot_y = ((int)(face_index + 2 + ((face_index + 2 >> 0x1f) & 3U)) >> 2) * 0xdf + 0x24;
    FUN_0040268a((int *)PTR_DAT_0040c05c, face_slot_x, face_slot_y, DAT_0041e4dc);
    FUN_00402621((int *)PTR_DAT_0040c05c, face_slot_x, face_slot_y, 0x89, 0xa9,
                 (int *)PTR_DAT_0040c034, face_slot_x, face_slot_y);
  }

  face_index = 0;
  while (1)
  {
    visible_face_count = DAT_0041e410;
    if (5 < DAT_0041e410)
    {
      visible_face_count = 6;
    }
    if (visible_face_count <= face_index)
    {
      break;
    }
    FUN_00403573(face_index, 0);
    face_index++;
  }

  for (face_index = 0; face_index < 6; face_index++)
  {
    FUN_0040268a((int *)PTR_DAT_0040c05c, *(int *)((char *)&DAT_0040c190 + face_index * 8),
                 *(int *)((char *)&DAT_0040c194 + face_index * 8), DAT_0041e4a0);
  }

  for (face_index = 0; face_index < 4; face_index++)
  {
    FUN_0040268a((int *)PTR_DAT_0040c05c, *(int *)((char *)&DAT_0040c1c0 + face_index * 8),
                 ((int *)&DAT_0040c1c4)[face_index * 2], DAT_0041e4a0);
  }

  *(int *)PTR_DAT_0040c034 = 1;
  DAT_0041e560 = 1;
  button_index = 1;
  while (1)
  {
    visible_face_count = DAT_0041e410;
    if (5 < DAT_0041e410)
    {
      visible_face_count = 6;
    }
    if (visible_face_count * 2 + 9 <= button_index)
    {
      break;
    }
    FUN_004028d1(&DAT_0040c1e8[button_index * 0x34], 0);
    button_index++;
  }

  while (button_index < 0x15)
  {
    FUN_004028d1(&DAT_0040c1e8[button_index * 0x34], 3);
    button_index++;
  }

  FUN_00402a84(&DAT_0040c284);
  FUN_00402a84(&DAT_0040c2b8);
  DAT_0040c208 = 3;
  DAT_0041e560 = 0;
  *(int *)PTR_DAT_0040c034 = 0;
  FUN_00406cc0(1);
  FUN_004032e4();
  DAT_0041c144 = 1;

  return 0;
}

// FUNCTION: FACEMAKER 0x00405a49
LRESULT CALLBACK FUN_00405a49(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  return DefWindowProcA(hwnd, msg, wparam, lparam);
}

// FUNCTION: FACEMAKER 0x00405fa8
void FUN_00405fa8(void)
{
  HDC hdc;

  if (DAT_0040c9e4 == 0)
  {
    SetSystemPaletteUse(DAT_0041afb4, 1);
  }
  else
  {
    hdc = GetDC((HWND)0);
    SelectPalette(hdc, GetStockObject(15), FALSE);
    SetSystemPaletteUse(hdc, 1);
    RealizePalette(hdc);
    ReleaseDC((HWND)0, hdc);
  }
}

// FUNCTION: FACEMAKER 0x004061bd
LONG __cdecl FUN_004061bd(DWORD width, DWORD height)
{
  DEVMODEA dev_mode;

  if (height == 0 && width == 0)
  {
    return ChangeDisplaySettingsA((DEVMODEA *)0, 0);
  }

  if (height == 0 || width == 0)
  {
    return -1;
  }

  dev_mode.dmSize = 0x9c;
  dev_mode.dmFields = 0x180000;
  dev_mode.dmPelsWidth = width;
  dev_mode.dmPelsHeight = height;
  return ChangeDisplaySettingsA(&dev_mode, 0);
}

// FUNCTION: FACEMAKER 0x0040623c
void FUN_0040623c(void)
{
  FUN_004061bd(0, 0);
}

#ifdef FACEMAKER
// These two functions appear to have been built with different codegen.
#pragma optimize("gy", on)
#endif

// FUNCTION: FACEMAKER 0x00406810
FacemakerSurface *__cdecl FUN_00406810(int width, int height, int bits_per_pixel)
{
  struct
  {
    char mapping_name[16];
  } s;
  FacemakerSurface *surface;
  HDC screen_dc;
  int image_size_bytes;
  LOGPALETTE *log_palette;
  LOGPALETTE *log_palette_cursor;
  HPALETTE palette_handle;
  PALETTEENTRY *palette_entry;
  int palette_entries_left;

  *(DWORD *)&s.mapping_name[0] = *(DWORD *)&s_rpbits_0040d0a0[0];
  *(WORD *)&s.mapping_name[4] = *(WORD *)&s_rpbits_0040d0a0[4];
  s.mapping_name[6] = s_rpbits_0040d0a0[6];
  *(DWORD *)&s.mapping_name[7] = 0;
  *(DWORD *)&s.mapping_name[11] = 0;
  s.mapping_name[15] = '\0';

  if (DAT_004189b4 != 0)
  {
    return DAT_00426540[0];
  }

  surface = (FacemakerSurface *)malloc(0x30);
  DAT_0042656c = width;
  surface->width = width;
  DAT_00426568 = height;
  surface->height = height;
  if (bits_per_pixel == -1)
  {
    screen_dc = GetDC((HWND)0);
    bits_per_pixel = GetDeviceCaps(screen_dc, BITSPIXEL);
    DAT_00426570 = bits_per_pixel;
    surface->bits_per_pixel = bits_per_pixel;
    ReleaseDC((HWND)0, screen_dc);
  }
  else
  {
    DAT_00426570 = bits_per_pixel;
    surface->bits_per_pixel = bits_per_pixel;
  }

  image_size_bytes = bits_per_pixel * height * width;
  DAT_0040d08c = 0;
  surface->image_size_bytes = (image_size_bytes + ((image_size_bytes >> 31) & 7)) >> 3;
  _itoa(0, s.mapping_name, 10);
  surface->hdc = DAT_0041afb4;
  surface->bitmap_info = CreateBitmapInfo(width, height, bits_per_pixel);
  surface->bitmap_info = CreateBitmapInfo(width, height, 8);
  surface->mapping_handle = 0;
  surface->bitmap = 0;

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
  surface->palette = palette_handle;
  SelectPalette(surface->hdc, palette_handle, FALSE);
  RealizePalette(surface->hdc);
  SetStretchBltMode(surface->hdc, 3);
  DAT_00426540[0] = surface;
  DAT_004189b4 = 1;
  return surface;
}

// FUNCTION: FACEMAKER 0x004069e0
FacemakerSurface *__cdecl FUN_004069e0(int page_number, int width, int height, int bits_per_pixel)
{
  struct
  {
    char mapping_name[16];
  } s;
  FacemakerSeedString *mapping_seed;
  FacemakerSurface *page;
  int row_remainder;
  int total_bits;
  int image_size_bytes;
  DWORD *dword_ptr;
  unsigned int fill_count;

  mapping_seed = (FacemakerSeedString *)s_rpbips_0040d138;
  *(DWORD *)&s.mapping_name[0] = mapping_seed->head;
  *(WORD *)&s.mapping_name[4] = mapping_seed->mid;
  s.mapping_name[6] = mapping_seed->tail;
  *(DWORD *)&s.mapping_name[7] = 0;
  *(DWORD *)&s.mapping_name[11] = 0;
  s.mapping_name[15] = '\0';

  if (page_number == 0)
  {
    return FUN_00406810(width, height, bits_per_pixel);
  }

  assert(page_number < 10 ? 1 : 0, s_D__NewMagic__sources__sidlib__lib_c_0040d0ec, 0x121,
         "Graphic Page number out of range: %d\n", page_number);

  page = (FacemakerSurface *)malloc(0x30);
  page->width = width;
  page->height = height;
  page->bits_per_pixel = bits_per_pixel;

  total_bits = bits_per_pixel * width;
  total_bits += (total_bits >> 31) & 7;
  row_remainder = total_bits >> 3;
  row_remainder = (row_remainder ^ (row_remainder >> 31)) - (row_remainder >> 31);
  row_remainder &= 3;
  if (row_remainder == 0)
  {
    page->row_padding = 0;
  }
  else
  {
    page->row_padding = 4 - row_remainder;
  }

  image_size_bytes = (page->row_padding + width) * bits_per_pixel * height;
  image_size_bytes = (image_size_bytes + ((image_size_bytes >> 31) & 7)) >> 3;
  page->image_size_bytes = image_size_bytes;

  _itoa(page_number, s.mapping_name, 10);
  page->hdc = CreateCompatibleDC((HDC)0);
  page->bitmap_info = CreateBitmapInfo(width, height, bits_per_pixel);
  page->mapping_handle =
      CreateFileMappingA((HANDLE)0xffffffff, (LPSECURITY_ATTRIBUTES)0, 0x8000004, 0, image_size_bytes,
                         s.mapping_name);
  assert((int)page->mapping_handle, s_D__NewMagic__sources__sidlib__lib_c_0040d0ec, 0x137,
         "Create File Mapping failed: page %d\n", page_number);

  page->bitmap =
      CreateDIBSection(page->hdc, page->bitmap_info, (unsigned int)(bits_per_pixel == 8),
                       &page->bits, page->mapping_handle, 0);
  assert((int)page->bitmap, s_D__NewMagic__sources__sidlib__lib_c_0040d0ec, 0x13b,
         "WM_CREATE CreateDIBSection");

  page->previous_bitmap = SelectObject(page->hdc, page->bitmap);
  page->palette = DAT_00425e10;
  SelectPalette(page->hdc, page->palette, FALSE);
  RealizePalette(page->hdc);
  SetStretchBltMode(page->hdc, 3);

  dword_ptr = (DWORD *)page->bits;
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

#ifdef FACEMAKER
#pragma optimize("", on)
#endif

// FUNCTION: FACEMAKER 0x00405510
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int nShowCmd)
{
  char *slash;
  DWORD thread_id;
  int horzres;
  unsigned int matched_mode;
  char module_path[256];
  MSG msg;
  int custom_mode_selected;
  WNDCLASSA wndclass;
  HWND main_hwnd;

  (void)prevInstance;

  custom_mode_selected = 0;
  DAT_0041b028 = FindWindowA("FaceMakerMainClass", (LPCSTR)0);
  if (DAT_0041b028 != (HWND)0)
  {
    ShowWindow(DAT_0041b028, nShowCmd);
    BringWindowToTop(DAT_0041b028);
    SetForegroundWindow(DAT_0041b028);
    return 0;
  }

  srand(GetTickCount());
  DAT_0041c140 = hInstance;
  wndclass.style = 0x23;
  wndclass.lpfnWndProc = FUN_00405a49;
  wndclass.cbClsExtra = 0;
  wndclass.cbWndExtra = 0;
  wndclass.hInstance = hInstance;
  wndclass.hIcon = (HICON)0;
  wndclass.hCursor = LoadCursorA((HINSTANCE)0, (LPCSTR)0x7f00);
  wndclass.hbrBackground = GetStockObject(4);
  wndclass.lpszMenuName = (LPCSTR)0;
  wndclass.lpszClassName = "FaceMakerMainClass";
  if (RegisterClassA(&wndclass) == 0)
  {
    MessageBoxA((HWND)0, "Couldn't register the classes", (LPCSTR)0, 0x1010);
    return 0;
  }

#ifdef MODERN_FIXES
  GetCurrentDirectoryA(0x100, DAT_0041b024);  
#else
  // lmfao it's a miracle this doesn't crash (nBufferLength == 0 so the write to address 0x100 is elided)
  GetCurrentDirectoryA(DAT_0041b024[0], 0x100);
#endif

  strcpy(module_path, (*__p___argv())[0]);
  slash = strrchr(module_path, '\\');
  *slash = '\0';
  if (_chdir(module_path) != 0)
  {
    assert_noabort(0, "D:\\NewMagic\\Sources\\1player\\FaceMaker\\code\\test.c", 0xa2,
                   "Path Set FAILED");
  }

  atexit(FUN_0040623c);
  if (GetDeviceCaps(GetDC((HWND)0), HORZRES) > 1024)
  {
    FUN_004061bd(1024, 768);
  }

  while (*cmdLine != '\0' && cmdLine[1] != '\0')
  {
    matched_mode = 0;
    if (cmdLine[1] == 'S')
    {
      DAT_0040c9e4 = 0;
    }

    if (cmdLine[1] == '6')
    {
      global_screen_width = 640;
      global_screen_height = 480;
      FUN_004069e0(0, global_screen_width, global_screen_height, -1);
      custom_mode_selected = 1;
    }

    matched_mode = (unsigned int)(cmdLine[1] == '6');
    if (cmdLine[1] == '8')
    {
      global_screen_width = 800;
      global_screen_height = 600;
      FUN_004069e0(0, global_screen_width, global_screen_height, -1);
      matched_mode = 1;
      custom_mode_selected = 1;
    }

    if (cmdLine[1] == '1')
    {
      global_screen_width = 1024;
      global_screen_height = 768;
      FUN_004069e0(0, global_screen_width, global_screen_height, -1);
      matched_mode = 1;
      custom_mode_selected = 1;
    }

    if (matched_mode != 0)
    {
      PTR_DAT_0040c034->max_x = global_screen_width - 1;
      PTR_DAT_0040c034->max_y = global_screen_height - 1;
      break;
    }

    *cmdLine = '\0';
  }

  if (matched_mode == 0)
  {
    horzres = GetDeviceCaps(GetDC((HWND)0), HORZRES);
    switch (horzres)
    {
    case 640:
      global_screen_width = 640;
      global_screen_height = 480;
      break;
    case 800:
      global_screen_width = 800;
      global_screen_height = 600;
      break;
    case 1024:
      global_screen_width = 1024;
      global_screen_height = 768;
      break;
    }

    PTR_DAT_0040c034->max_x = global_screen_width - 1;
    PTR_DAT_0040c034->max_y = global_screen_height - 1;
  }

  DAT_0041b028 = CreateWindowExA(-(unsigned int)(DAT_0040c9e4 == 0) & 8, "FacemakerMainClass",
                                 "Magic: Facemaker", 0x80000000, 0, 0, global_screen_width,
                                 global_screen_height, (HWND)0, (HMENU)0, hInstance, (LPVOID)0);
  main_hwnd = DAT_0041b028;
  ShowWindow(DAT_0041b028, nShowCmd);
  DAT_0041afb4 = GetDC(main_hwnd);
  BringWindowToTop(DAT_0041b028);
  SetForegroundWindow(DAT_0041b028);
  SetFocus(DAT_0041b028);
  if (custom_mode_selected != 0)
  {
    DAT_00426540[0]->hdc = DAT_0041afb4;
    SelectPalette(DAT_00426540[0]->hdc, DAT_00426540[0]->palette, FALSE);
    RealizePalette(DAT_00426540[0]->hdc);
    SetStretchBltMode(DAT_00426540[0]->hdc, 3);
  }

  SetSystemPaletteUse(DAT_0041afb4, 2);
  atexit(FUN_00405fa8);
  CreateThread((LPSECURITY_ATTRIBUTES)0, 0, FUN_00403fd9, (LPVOID)0, 0, &thread_id);
  while (GetMessageA(&msg, (HWND)0, 0, 0) != 0)
  {
    TranslateMessage(&msg);
    DispatchMessageA(&msg);
  }

  GdiFlush();
  return DAT_0041e564;
}
