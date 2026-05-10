#include <windows.h>
#include <stdio.h>
#include <io.h>
#include <string.h>
#include "cardartlib/src/assert.h"
#include "drawcardlib/src/pic.h"
#include "drawcardlib/src/pcxw.h"

extern void FUN_004075d0(unsigned int *param_1, int param_2, int param_3, int param_4,
                                 unsigned int param_5);
extern unsigned char gPcxScanlineBuffer[0x1000];
extern void RpBits_Setup(int fileDescriptor);
extern void RpBits_ReadTables(unsigned short *param_3);
extern void RpBits_DecodeImage(void *dst, int count);
extern void RpBits_ApplyPalette(short *palette_data_words);
extern char DAT_0040d224[];

void FUN_00409eb0(int page_number, int x, int y, char *path, void *opaque);

#pragma optimize("gy", on)

// GLOBAL: FACEMAKER 0x0040d29c
char s_D__NewMagic__sources__sidlib__Fileio_c_0040d29c[] = "D:\\NewMagic\\sources\\sidlib\\Fileio.c";

// GLOBAL: FACEMAKER 0x0040d2c0
char s_Error_Opening_File__s_0040d2c0[] = "Error Opening File %s\r\n";

// GLOBAL: FACEMAKER 0x0040d2d8
char DAT_0040d2d8[] = ".pcx";

// FUNCTION: FACEMAKER 0x0040a110
void FUN_0040a110(int page_number, char *path)
{
  FUN_00409eb0(page_number, 0, 0, path, (void *)1);
}

// FUNCTION: FACEMAKER 0x0040a130
void FUN_0040a130(int page_number, char *path)
{
  FUN_00409eb0(page_number, 0, 0, path, (void *)0);
}

// FUNCTION: FACEMAKER 0x0040a150
void FUN_0040a150(char *path, unsigned short *palette)
{
  FUN_00409eb0(-1, 0, 0, path, palette);
}

// FUNCTION: FACEMAKER 0x00409eb0
void FUN_00409eb0(int page_number, int x, int y, char *path, void *opaque)
{
  char *ext;
  int file_handle;
  int line;
  unsigned short *palette;
  unsigned short local_palette[0x200];

  ext = strchr(path, '.');
  if (_stricmp(DAT_0040d2d8, ext) != 0)
  {
    file_handle = _open(path, 0x8000);
    assert((unsigned int)(file_handle != -1), s_D__NewMagic__sources__sidlib__Fileio_c_0040d29c, 0x88,
           s_Error_Opening_File__s_0040d2c0, path);

    RpBits_Setup(file_handle);
    RpBits_ReadTables((unsigned short *)opaque);
    if (page_number < 0)
    {
      global_pcxw_image_height = 0;
    }
    line = 0;
    while (line < global_pcxw_image_height)
    {
      RpBits_DecodeImage(gPcxScanlineBuffer, global_pcxw_image_width);
      FUN_004075d0((unsigned int *)gPcxScanlineBuffer, page_number, x, y + line, global_pcxw_image_width);
      line = line + 1;
    }

    assert((unsigned int)(_close(file_handle) == 0), s_D__NewMagic__sources__sidlib__Fileio_c_0040d29c, 0xa8,
           (char *)0);
    return;
  }

  gPcxInFile = fopen(path, DAT_0040d224);
  assert((unsigned int)(gPcxInFile != (FILE *)0), s_D__NewMagic__sources__sidlib__Fileio_c_0040d29c, 0xf7,
         s_Error_Opening_File__s_0040d2c0, path);
  gPcxPath = path;
  palette = (unsigned short *)opaque;
  if (palette == (unsigned short *)1)
  {
    palette = local_palette;
  }
  if (palette == (unsigned short *)0)
  {
    PcxReadHeaderAndPalette((void *)0);
  }
  else
  {
    PcxReadHeaderAndPalette(palette + 3);
    *(char *)palette = 'M';
    *((char *)palette + 1) = '1';
    *(unsigned short *)((char *)palette + 2) = 0x300;
    *((char *)palette + 4) = '\0';
    *((char *)palette + 5) = '\xff';
    RpBits_ApplyPalette((short *)palette);
  }

  if (page_number < 0)
  {
    global_pcxw_image_height = 0;
  }
  line = 0;
  while (line < global_pcxw_image_height)
  {
    PcxReadScanlineRle((char *)gPcxScanlineBuffer);
    FUN_004075d0((unsigned int *)gPcxScanlineBuffer, page_number, x, y + line, global_pcxw_image_width);
    line = line + 1;
  }
  fclose(gPcxInFile);
}
