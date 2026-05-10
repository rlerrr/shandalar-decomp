#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <io.h>
#include <stdlib.h>
#include "inttypes.h"
#include "mystdbool.h"
#include "pcxw.h"
#include "pic.h"
#include "cardartlib/src/palette.h"

typedef unsigned short ushort;

/* Local helpers moved with load_pic(). */
int OpenPcxFile(char *param_1, int param_2);
void ClosePcxFile(int param_1);
void RpBits_Setup(int fileDescriptor);
int RpBitsRefill(void);
void RpBits_ApplyPalette(short *palette_data_words);

#if defined(FACEMAKER) || defined(SHANDALAR)
extern int g_graphics_bpp;
extern HPALETTE g_palette_handle;
extern PALETTEENTRY g_palette_entries[256];
extern RGBQUAD g_palette_rgb[256];
extern DIBSurface *g_graphics_pages[10];
extern int g_graphics_height;
extern int g_scanline_palette_needs_refresh;
#endif

typedef struct RpBitsPalettePacket
{
  unsigned short signature;
  unsigned short block_size;
  unsigned char first_index;
  unsigned char last_index;
  unsigned char entry_data[1];
} RpBitsPalettePacket;

#ifndef defined(DRAWCARDLIB)
// For some reason these are actually optimized in deckdll?
#pragma optimize("gy", on)
#endif

// FUNCTION: DRAWCARDLIB 0x1000965b
// FUNCTION: DECKDLL 0x100010a0
// FUNCTION: FACEMAKER 0x00407a90
// FUNCTION: SHANDALAR 0x0057a440
BITMAPINFO *CreateBitmapInfo(int width, int height, int bitsPerPixel)
{
  short *color;
  BITMAPINFO *bmi;
  int i;

  if (bitsPerPixel == 8)
  {
    bmi = malloc(sizeof(BITMAPINFO) + 256 * sizeof(RGBQUAD));
  }
  else if (bitsPerPixel == 24)
  {
    bmi = malloc(sizeof(BITMAPINFO));
  }
  else
  {
    bmi = malloc(sizeof(BITMAPINFO));
  }

  // Header
  bmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
  bmi->bmiHeader.biWidth = width;
  bmi->bmiHeader.biHeight = -height; // top-down DIB
  bmi->bmiHeader.biPlanes = 1;
  bmi->bmiHeader.biBitCount = (WORD)bitsPerPixel;
  bmi->bmiHeader.biCompression = BI_RGB;
  bmi->bmiHeader.biSizeImage = 0;
  bmi->bmiHeader.biXPelsPerMeter = 0;
  bmi->bmiHeader.biYPelsPerMeter = 0;

  if (bitsPerPixel == 8)
  {
    bmi->bmiHeader.biClrUsed = 256;
    bmi->bmiHeader.biClrImportant = 256;

    color = (short *)&bmi->bmiColors[0];
    for (i = 0; i < 256; i++)
    {
      *color++ = i;
    }
  }
  else
  {
    bmi->bmiHeader.biClrUsed = 0;
    bmi->bmiHeader.biClrImportant = 0;
  }

  return bmi;
}

// FUNCTION: DRAWCARDLIB 0x1000977f
// FUNCTION: DECKDLL 0x10001120
// FUNCTION: SHANDALAR 0x0057a4c0
BOOL FreeBitmapInfo(void *param_1)
{
  free(param_1);
  return 1;
}

#ifndef DRAWCARDLIB
#pragma optimize("", on)
#endif

// GLOBAL: DRAWCARDLIB 0x100f23b0
// GLOBAL: DECKDLL 0x10105338
DIBSurface DAT_100f23b0;

// GLOBAL: DRAWCARDLIB 0x10022528
// GLOBAL: DECKDLL 0x1003a840
// GLOBAL: MAGIC 0x00570f2c
DIBSurface *global_dibSurface = &DAT_100f23b0;

// GLOBAL: DRAWCARDLIB 0x100f35a0
// GLOBAL: DECKDLL 0x10105730
// GLOBAL: SHANDALAR 0x00748428
// GLOBAL: FACEMAKER 0x00422320
void(__cdecl *rpbits_stream_refill)();

extern byte rpbits_buffer[0x200];

// This points off the end of the array, which yields a reccmp warning but oh well
// GLOBAL: DRAWCARDLIB 0x1002252c
// GLOBAL: DECKDLL 0x1003a844
// GLOBAL: SHANDALAR 0x005a1814
// GLOBAL: FACEMAKER 0x0040d294
#if defined(FACEMAKER) || defined(SHANDALAR)
byte *rpbits_stream_end = (byte *)&rpbits_stream_refill;
#else
byte *rpbits_stream_end = rpbits_buffer + 0x200;
#endif

// GLOBAL: DRAWCARDLIB 0x10022530
// GLOBAL: DECKDLL 0x1003a848
undefined4 DAT_10022530 = 0xFFFFFFFF;

// GLOBAL: DRAWCARDLIB 0x100f2394
// GLOBAL: DECKDLL 0x1010531c
int DAT_100f2394;

// GLOBAL: DRAWCARDLIB 0x100f239c
// GLOBAL: DECKDLL 0x10105324
undefined4 global_pcx_lineNum;

// GLOBAL: DRAWCARDLIB 0x100f23a0
// GLOBAL: DECKDLL 0x10105328
int global_pcxFileDescriptor;

// GLOBAL: DRAWCARDLIB 0x100f23a4
// GLOBAL: DECKDLL 0x1010532c
// GLOBAL: SHANDALAR 0x005b7b6c
undefined4 rpbits_file_descriptor;

// GLOBAL: DRAWCARDLIB 0x100f23a8
// GLOBAL: DECKDLL 0x10105330
// GLOBAL: MAGIC 0x00637e20
undefined4 _DAT_100f23a8;

// GLOBAL: DRAWCARDLIB 0x100f33a0
// GLOBAL: DECKDLL 0x10105530
// GLOBAL: SHANDALAR 0x00982550
byte rpbits_buffer[0x200];

// GLOBAL: DRAWCARDLIB 0x100f3394
// GLOBAL: DECKDLL 0x1010552c
// GLOBAL: FACEMAKER 0x00422110
// GLOBAL: SHANDALAR 0x00982540
byte *rpbits_stream_ptr;

// LIBRARY: DRAWCARDLIB 0x10155000 SYMBOL
// LIBRARY: DECKDLL 0x101f7000 SYMBOL
// LIBRARY: MAGIC 0x00953000 SYMBOL
// LIBRARY: SHANDALAR 0x0098a000 SYMBOL
// LIBRARY: FACEMAKER 0x00428000 SYMBOL
// _RpBits_ReadTables
void RpBits_ReadTables(ushort *param_3);

// LIBRARY: DRAWCARDLIB 0x10155484 SYMBOL
// LIBRARY: DECKDLL 0x101f7484 SYMBOL
// LIBRARY: MAGIC 0x00953484 SYMBOL
// LIBRARY: SHANDALAR 0x0098a484 SYMBOL
// LIBRARY: FACEMAKER 0x00428484 SYMBOL
// _RpBits_DecodeImage
void RpBits_DecodeImage(void *dst, int count);

// LIBRARY: DRAWCARDLIB 0x10155245 SYMBOL
// LIBRARY: DECKDLL 0x101f7245 SYMBOL
// LIBRARY: MAGIC 0x00953245 SYMBOL
// LIBRARY: SHANDALAR 0x0098a245 SYMBOL
// LIBRARY: FACEMAKER 0x00428245 SYMBOL
// _RpBits_InitStream

// LIBRARY: DRAWCARDLIB 0x101552b5 SYMBOL
// LIBRARY: DECKDLL 0x101f72b5 SYMBOL
// LIBRARY: MAGIC 0x009532b5 SYMBOL
// LIBRARY: SHANDALAR 0x0098a2b5 SYMBOL
// LIBRARY: FACEMAKER 0x004282b5 SYMBOL
// _RpBits_ResetDecoder

// LIBRARY: DRAWCARDLIB 0x10155392 SYMBOL
// LIBRARY: DECKDLL 0x101f7392 SYMBOL
// LIBRARY: MAGIC 0x00953392 SYMBOL
// LIBRARY: SHANDALAR 0x0098a392 SYMBOL
// LIBRARY: FACEMAKER 0x00428392 SYMBOL
// _RpBits_DecodeSymbol

// LIBRARY: DRAWCARDLIB 0x10155300 SYMBOL
// LIBRARY: DECKDLL 0x101f7300 SYMBOL
// LIBRARY: MAGIC 0x00953300 SYMBOL
// LIBRARY: SHANDALAR 0x0098a300 SYMBOL
// LIBRARY: FACEMAKER 0x00428300 SYMBOL
// _RpBits_DecodeRun

// FUNCTION: DRAWCARDLIB 0x1000b1e0
// FUNCTION: DECKDLL 0x1002cc80
// FUNCTION: SHANDALAR 0x004c8600
DIBSurface *CreateDIBSurface(int width, int height, int bitsPerPixel)
{
  // Gotta be leftover debugging or something
  char unused[16] = "rpbits";
  struct
  {
    DWORD imageSize;
    int screenDC;
  } s;

  // Store basic parameters
  global_dibSurface->width = width;
  global_dibSurface->height = height;
  global_dibSurface->bitsPerPixel = bitsPerPixel;

  // Calculate row padding to DWORD-align scanlines
  if ((width * bitsPerPixel) / 8 % 4 != 0)
  {
    global_dibSurface->rowPadding = (4 - ((width * bitsPerPixel) / 8 % 4));
  }
  else
  {
    global_dibSurface->rowPadding = 0;
  }

  // Total image size (+ a little extra, matching original)
  s.screenDC = global_dibSurface->rowPadding;
  s.imageSize = (width + (int)s.screenDC) * bitsPerPixel * height / 8 + 0x10;
  global_dibSurface->imageSizeBytes = s.imageSize;

  // Create BITMAPINFO
  global_dibSurface->pBitmapInfo = CreateBitmapInfo(width, height, bitsPerPixel);
  if (!global_dibSurface->pBitmapInfo)
    return NULL;

  // Create shared memory for the DIB section
  global_dibSurface->hMapping = CreateFileMappingA(
      INVALID_HANDLE_VALUE,
      NULL,
      0x8000004,
      0,
      s.imageSize,
      NULL);

  if (!global_dibSurface->hMapping)
  {
    FreeBitmapInfo(global_dibSurface->pBitmapInfo);
    return NULL;
  }

  // Create the DIB section
  global_dibSurface->hTempDC = GetDC(NULL);
  ApplyCardArtPaletteToDc(global_dibSurface->hTempDC);

  global_dibSurface->hBitmap = CreateDIBSection(
      global_dibSurface->hTempDC,
      global_dibSurface->pBitmapInfo,
      (bitsPerPixel == 8) ? DIB_PAL_COLORS : DIB_RGB_COLORS,
      &global_dibSurface->pBits,
      global_dibSurface->hMapping,
      0);

  ReleaseDC(NULL, global_dibSurface->hTempDC);

  if (!global_dibSurface->hBitmap)
  {
    FreeBitmapInfo(global_dibSurface->pBitmapInfo);
    CloseHandle(global_dibSurface->hMapping);
    return 0;
  }

  FreeBitmapInfo(global_dibSurface->pBitmapInfo);
  return global_dibSurface;
}

// FUNCTION: DRAWCARDLIB 0x1000b40f
// FUNCTION: DECKDLL 0x1002ceab
// FUNCTION: MAGIC 0x004853bf
HBITMAP
LoadPicFile(int param_1, undefined4 param_2, undefined4 param_3, char *pcxFilename, undefined1 *palette)
{
  struct
  {
    int local_414;
    int local_410;
    undefined1 local_40c[1024];
    char *local_c;
    int bitsPerPixel;
  } s;

  param_2 = param_2;
  param_3 = param_3;
  s.bitsPerPixel = 8;

  if (strcmpi(".pcx", strchr(pcxFilename, '.')) == 0)
  {
    //.pcx file
    gPcxInFile = fopen(pcxFilename, "rb");
    if (gPcxInFile == (FILE *)0x0)
    {
      return 0;
    }
    gPcxPath = pcxFilename;
    if (palette == (undefined1 *)0x1)
    {
      palette = s.local_40c;
    }
    if (palette != (undefined1 *)0x0)
    {
      PcxReadHeaderAndPalette(palette + 6);
      *palette = 0x4d;
      palette[1] = 0x31;
      *(undefined2 *)(palette + 2) = 0x300;
      palette[4] = 0;
      palette[5] = 0xff;
    }
    else
    {
      PcxReadHeaderAndPalette(0);
      if (param_1 < 0)
      {
        global_pcxw_image_height = 0;
      }
      if ((global_pcxw_image_width & 3) != 0)
      {
        s.local_410 = 4 - (global_pcxw_image_width & 3);
      }
      else
      {
        s.local_410 = 0;
      }
      DAT_100f2394 = global_pcxw_image_width + s.local_410;
      CreateDIBSurface(DAT_100f2394, global_pcxw_image_height, s.bitsPerPixel);
      s.local_c = (char *)global_dibSurface->pBits;
      for (global_pcx_lineNum = 0; (int)global_pcx_lineNum < global_pcxw_image_height; global_pcx_lineNum++,
          s.local_c += (s.bitsPerPixel / 8 * DAT_100f2394))
      {
        PcxReadScanlineRle(s.local_c);
      }
      fclose(gPcxInFile);
    }
  }
  else
  {
    //.pic file (probably)
    global_pcxFileDescriptor = OpenPcxFile(pcxFilename, 0x8000);
    if (global_pcxFileDescriptor != -1)
    {
      RpBits_Setup(global_pcxFileDescriptor);
      RpBits_ReadTables((ushort *)palette);
      if ((global_pcxw_image_width & 3) != 0)
      {
        s.local_414 = 4 - (global_pcxw_image_width & 3);
      }
      else
      {
        s.local_414 = 0;
      }
      DAT_100f2394 = global_pcxw_image_width + s.local_414;

      if (CreateDIBSurface(global_pcxw_image_width, global_pcxw_image_height, s.bitsPerPixel) != 0)
      {
        s.local_c = (char *)global_dibSurface->pBits;

        for (global_pcx_lineNum = 0; (int)global_pcx_lineNum < global_pcxw_image_height; global_pcx_lineNum++,
            s.local_c += global_dibSurface->rowPadding + (global_pcxw_image_width * s.bitsPerPixel) / 8)
        {
          RpBits_DecodeImage(s.local_c, global_pcxw_image_width);
        }
      }
      else
        global_dibSurface->hBitmap = 0;
      ClosePcxFile(global_pcxFileDescriptor);
    }
    else
      return 0;
  }
  return global_dibSurface->hBitmap;
}

// MATCHING
// FUNCTION: DRAWCARDLIB 0x1000b700
// FUNCTION: DECKDLL 0x1002d1a1
// FUNCTION: MAGIC 0x004856b5
HBITMAP load_pic(char *filename)
{
  HBITMAP iVar1;

  iVar1 = LoadPicFile(0, 0, 0, filename, 0);
  if (iVar1 != 0)
  {
    CloseHandle(global_dibSurface->hMapping);
  }
  return iVar1;
}

// MATCHING
// FUNCTION: DRAWCARDLIB 0x1000b745
// FUNCTION: DECKDLL 0x1002d1e6
// FUNCTION: MAGIC 0x004856fa
int OpenPcxFile(char *param_1, int param_2)
{
  int iVar1;

  iVar1 = _open(param_1, param_2);
  _DAT_100f23a8 = 0xffffffff;
  return iVar1;
}

// FUNCTION: DRAWCARDLIB 0x1000b778
// FUNCTION: DECKDLL 0x1002d21a
// FUNCTION: MAGIC 0x0048572e
void ClosePcxFile(int param_1)
{
  if (param_1 == (int)DAT_10022530)
    return;
  _close(param_1);
}

// FUNCTION: DRAWCARDLIB 0x1000b7a2
// FUNCTION: DECKDLL 0x1002d245
// FUNCTION: MAGIC 0x00485759
// FUNCTION: SHANDALAR 0x004c8bc5
void RpBits_Setup(int fileDescriptor)
{
  rpbits_file_descriptor = fileDescriptor;
  rpbits_stream_ptr = (byte *)rpbits_stream_end;
  rpbits_stream_refill = (void(__cdecl *)())RpBitsRefill;
}

// MATCHING
// FUNCTION: DRAWCARDLIB 0x1000b7c9
// FUNCTION: DECKDLL 0x1002d26c
// FUNCTION: MAGIC 0x00485780
// FUNCTION: SHANDALAR 0x004c8bec
int RpBitsRefill(void)
{
  int iVar1;

  iVar1 = _read(rpbits_file_descriptor, rpbits_buffer, 0x200);
  rpbits_stream_ptr = rpbits_buffer;
  return iVar1;
}

// FUNCTION: DRAWCARDLIB 0x1000b804
// FUNCTION: DECKDLL 0x1002d2a8
// FUNCTION: SHANDALAR 0x0057a060
// FUNCTION: FACEMAKER 0x00407720
#if defined(FACEMAKER) || defined(SHANDALAR)
#pragma optimize("gty", on)
#endif
#if !defined(FACEMAKER) && !defined(SHANDALAR)
#pragma optimize("", off)
#endif
void RpBits_ApplyPalette(short *palette_data_words)
{
#if defined(FACEMAKER) || defined(SHANDALAR)
  short *word_src;
  unsigned int *dword_dst;
  unsigned int count;
  unsigned int first_index;
  unsigned int last_index;
  int palette_offset;
  int remaining_entries;
  unsigned int mask_value;
  unsigned int rgb_value;
  unsigned char *entry_data;
  unsigned char component;
  unsigned char mask;
  int *surface_ptr;
  static unsigned int palette_packet_words[0x320 / 4];
  HWND palette_window;

  word_src = palette_data_words;
  dword_dst = palette_packet_words;
  for (count = (unsigned int)(int)(short)(palette_data_words[1] + 2) >> 2; count != 0; count = count - 1)
  {
    *dword_dst = *(unsigned int *)word_src;
    word_src = word_src + 2;
    dword_dst = dword_dst + 1;
  }

  for (count = (int)(short)(palette_data_words[1] + 2) & 3; count != 0; count = count - 1)
  {
    *(char *)dword_dst = (char)*word_src;
    word_src = (short *)((int)word_src + 1);
    dword_dst = (unsigned int *)((int)dword_dst + 1);
  }

  first_index = (unsigned int)*(unsigned char *)(palette_data_words + 2);
  mask_value = (-(unsigned int)(g_graphics_bpp == 0x10) & 0xfffffff9) + 0xff;
  last_index = (unsigned int)*(unsigned char *)((int)palette_data_words + 5);
  mask = (unsigned char)mask_value;

  if (*(short *)"M1" == *palette_data_words)
  {
    if (first_index <= last_index)
    {
      palette_offset = first_index * 4;
      entry_data = (unsigned char *)((int)palette_data_words + first_index * 3 + 6);
      remaining_entries = (last_index - first_index) + 1;
      do
      {
        component = (unsigned char)(((unsigned int)entry_data[0] * 0xff) / 0x3f) & mask;
        *((unsigned char *)&g_palette_entries + palette_offset + 0) = component;
        *((unsigned char *)&g_palette_rgb + palette_offset + 2) = component;

        component = (unsigned char)(((unsigned int)entry_data[1] * 0xff) / 0x3f) & mask;
        *((unsigned char *)&g_palette_entries + palette_offset + 1) = component;
        *((unsigned char *)&g_palette_rgb + palette_offset + 1) = component;

        component = (unsigned char)(((unsigned int)entry_data[2] * 0xff) / 0x3f) & mask;
        *((unsigned char *)&g_palette_entries + palette_offset + 2) = component;
        *((unsigned char *)&g_palette_rgb + palette_offset + 0) = component;

        *((unsigned char *)&g_palette_entries + palette_offset + 3) = 1;
        *((unsigned char *)&g_palette_rgb + palette_offset + 3) = 0;
        if (*(unsigned int *)((int)&g_palette_rgb + palette_offset) == 0x00ffffff && palette_offset != 0x3fc)
        {
          rgb_value = (mask_value << 0x10) | (mask_value << 8) | mask_value;
          *(unsigned int *)((int)&g_palette_entries + palette_offset) = rgb_value & 0x01fefefe;
          *(unsigned int *)((int)&g_palette_rgb + palette_offset) = rgb_value & 0x00fefefe;
        }

        palette_offset = palette_offset + 4;
        entry_data = entry_data + 3;
        remaining_entries = remaining_entries - 1;
      } while (remaining_entries != 0);
    }
  }
  else if (*(short *)"M0" == *palette_data_words)
  {
    if (first_index <= last_index)
    {
      palette_offset = first_index * 4;
      entry_data = (unsigned char *)((int)palette_data_words + first_index * 3 + 6);
      remaining_entries = (last_index - first_index) + 1;
      do
      {
        component = entry_data[0];
        *((unsigned char *)&g_palette_entries + palette_offset + 0) = component & mask;
        *((unsigned char *)&g_palette_rgb + palette_offset + 2) = component & mask;

        component = entry_data[1];
        *((unsigned char *)&g_palette_entries + palette_offset + 1) = component & mask;
        *((unsigned char *)&g_palette_rgb + palette_offset + 1) = component & mask;

        component = entry_data[2];
        *((unsigned char *)&g_palette_entries + palette_offset + 2) = component & mask;
        *((unsigned char *)&g_palette_rgb + palette_offset + 0) = component & mask;

        *((unsigned char *)&g_palette_entries + palette_offset + 3) = 1;
        *((unsigned char *)&g_palette_rgb + palette_offset + 3) = 0;
        if (*(unsigned int *)((int)&g_palette_rgb + palette_offset) == 0x00ffffff && palette_offset != 0x3fc)
        {
          rgb_value = ((mask_value << 0x10) | (mask_value << 8) | mask_value) & 0x00fefefe;
          *(unsigned int *)((int)&g_palette_rgb + palette_offset) = rgb_value;
          *(unsigned int *)((int)&g_palette_entries + palette_offset) = rgb_value;
          *((unsigned char *)&g_palette_entries + palette_offset + 3) = 1;
        }

        palette_offset = palette_offset + 4;
        entry_data = entry_data + 3;
        remaining_entries = remaining_entries - 1;
      } while (remaining_entries != 0);
    }
  }

  *((unsigned char *)&g_palette_entries + 2) = 0;
  *((unsigned char *)&g_palette_entries + 1) = 0;
  *((unsigned char *)&g_palette_entries + 0) = 0;
  *((unsigned char *)&g_palette_entries + 3) = 0;
  *((unsigned char *)&g_palette_rgb + 0) = 0;
  *((unsigned char *)&g_palette_rgb + 1) = 0;
  *((unsigned char *)&g_palette_rgb + 2) = 0;
  *((unsigned char *)&g_palette_rgb + 3) = 0;
  *((unsigned char *)&g_palette_entries + 0x3f7) = 1;
  *((unsigned char *)&g_palette_entries + 0x3fb) = 1;
  *((unsigned char *)&g_palette_entries + 0x3ff) = 0;
  *((unsigned char *)&g_palette_rgb + 0x3f7) = 0;
  *((unsigned char *)&g_palette_rgb + 0x3fc) = 0xff;
  *((unsigned char *)&g_palette_rgb + 0x3fd) = 0xff;
  *((unsigned char *)&g_palette_rgb + 0x3fe) = 0xff;
  *((unsigned char *)&g_palette_rgb + 0x3ff) = 0;

  AnimatePalette(g_palette_handle, 0, 0x100, (PALETTEENTRY *)&g_palette_entries);
  if (g_graphics_pages[0] != (DIBSurface *)0)
  {
    RealizePalette(g_graphics_pages[0]->hTempDC);
  }

  surface_ptr = (int *)&g_graphics_pages[1];
  do
  {
    if (*surface_ptr != 0)
    {
      SetDIBColorTable(*(HDC *)(*surface_ptr + 4), 0, 0x100, (RGBQUAD *)&g_palette_rgb);
    }
    surface_ptr = surface_ptr + 1;
  } while (surface_ptr < &g_graphics_height);

  palette_window = FindWindowExA((HWND)0, (HWND)0, "ShowPaletteClass", "Current Palette");
  if (palette_window != (HWND)0)
  {
    InvalidateRect(palette_window, (RECT *)0, FALSE);
    UpdateWindow(palette_window);
  }
  g_scanline_palette_needs_refresh = 1;
#else
  (void)palette_data_words;
#endif
}
#if !defined(FACEMAKER) && !defined(SHANDALAR)
#pragma optimize("", on)
#endif
#if defined(FACEMAKER) || defined(SHANDALAR)
#pragma optimize("", off)
#endif
