#include <windows.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "inttypes.h"
#include "mystdbool.h"
#include "sidlib/pcxw.h"

typedef void * pointer;
typedef char byte;
typedef unsigned short ushort;

/* External helpers (implemented elsewhere in the build). */
void FUN_1000c0f0(int ok, const char *file, int line, const char *fmt, ...);

/* Placeholder globals for missing decompiler labels (PCXW-only). */
char DAT_10021f18[] = "rb";
char DAT_10021f60[] = "rb";
char DAT_10021fd8[] = "w+b";

/* PCXW globals */
pointer PTR_s_D__Newmagic_sources_sidlib_Pcxw__10021ed0 = (pointer)0x10021EDC;
undefined4 gPcxBufferWidth = 0x00000000;
undefined4 gPcxBufferHeight = 0x00000000;
char s_D__Newmagic_sources_sidlib_Pcxw__10021edc[] = "D:\\Newmagic\\sources\\sidlib\\Pcxw.c";
char s_Error_Opening_File__s_10021f00[] = "Error Opening File %s\r\n";
char s__s_Not_a_256_color_palettized_pc_10021f1c[] = "%s Not a 256 color palettized pcx file\r\n";
char s_Error_Opening_File__s_10021f48[] = "Error Opening File %s\r\n";
char s__s_Not_a_pcx_file_10021f64[] = "%s Not a pcx file\r\n";
char s__s_Not_a_version_5_pcx_file_10021f78[] = "%s Not a version 5 pcx file\r\n";
char s__s_is_not_in_a_recognizable_form_10021f98[] = "%s is not in a recognizable format\r\n";
char s_Error_Opening_File__s_10021fc0[] = "Error Opening File %s\r\n";

undefined4 gImageWidth = 0x00000000;
undefined4 gImageHeight = 0x00000000;

FILE *gPcxInFile = (FILE *)0x0;
FILE *gPcxOutFile = (FILE *)0x0;
char *gPcxPath = (char *)0x0;
undefined2 gPcxBytesPerLine = 0x0000;

undefined4 DAT_10126d30 = 0;
undefined4 DAT_10127d42 = 0;
undefined4 DAT_10127d80 = 0;
undefined4 _DAT_10127d4c = 0;
undefined4 _DAT_10127d4e = 0;
undefined4 _DAT_10127d84 = 0;
undefined4 _DAT_10127d86 = 0;
undefined4 _DAT_10127d88 = 0;

undefined1 DAT_10127d40 = 0x00;
undefined1 DAT_10127d41 = 0x00;
undefined1 DAT_10127d43 = 0x00;
undefined2 DAT_10127d44 = 0x0000;
undefined2 DAT_10127d46 = 0x0000;
undefined2 DAT_10127d48 = 0x0000;
undefined2 DAT_10127d4a = 0x0000;
undefined1 DAT_10127d81 = 0x00;

// FUNCTION: DRAWCARDLIB 0x1000b80f
void FUN_1000b80f(void)
{
  return;
}

// FUNCTION: DRAWCARDLIB 0x10004050
undefined1 * PcxLoad8bppImage(char *path,undefined1 *imagePixels,void *paletteOut)
{
  undefined1 *puVar1;
  uint uVar2;
  int iVar3;
  HGLOBAL pvVar4;
  uint uVar5;
  undefined4 local_18;
  int local_14;
  int local_8;
  
  gPcxInFile = fopen(path,&DAT_10021f18);
  FUN_1000c0f0(gPcxInFile != (FILE *)0x0,PTR_s_D__Newmagic_sources_sidlib_Pcxw__10021ed0,0x69,
               s_Error_Opening_File__s_10021f00,path);
  gPcxPath = path;
  PcxReadHeaderAndPalette(paletteOut);
  if ((DAT_10127d43 == '\b') && (DAT_10127d81 == '\x01')) {
    local_18 = 1;
  }
  else {
    local_18 = 0;
  }
  FUN_1000c0f0(local_18,PTR_s_D__Newmagic_sources_sidlib_Pcxw__10021ed0,0x6f,
               s__s_Not_a_256_color_palettized_pc_10021f1c,gPcxPath);
  uVar2 = 4 - (gImageWidth & 3);
  uVar5 = (int)uVar2 >> 0x1f;
  iVar3 = ((uVar2 ^ uVar5) - uVar5 & 3 ^ uVar5) - uVar5;
  if ((gImageWidth == gPcxBufferWidth) && (gImageHeight == gPcxBufferHeight)) {
    memset(imagePixels,0,(gPcxBufferWidth + iVar3) * gPcxBufferHeight);
  }
  else {
    pvVar4 = GlobalHandle(imagePixels);
    GlobalUnlock(pvVar4);
    pvVar4 = GlobalHandle(imagePixels);
    GlobalUnlock(pvVar4);
    pvVar4 = GlobalHandle(imagePixels);
    GlobalFree(pvVar4);
    pvVar4 = GlobalAlloc(0x40,(gImageWidth + iVar3) * gImageHeight);
    imagePixels = GlobalLock(pvVar4);
    pvVar4 = GlobalHandle(imagePixels);
    GlobalLock(pvVar4);
    gPcxBufferWidth = gImageWidth;
    gPcxBufferHeight = gImageHeight;
  }
  puVar1 = imagePixels;
  for (local_8 = 0; local_8 < gImageHeight; local_8 = local_8 + 1) {
    PcxReadScanlineRle(&DAT_10126d30);
    for (local_14 = 0; local_14 < (int)gImageWidth; local_14 = local_14 + 1) {
      *imagePixels = (&DAT_10126d30)[local_14];
      imagePixels = imagePixels + 1;
    }
    imagePixels = imagePixels + iVar3;
  }
  fclose(gPcxInFile);
  return puVar1;
}

// FUNCTION: DRAWCARDLIB 0x10004267
bool PcxReadHeaderAndPaletteFromPath(char *path,void *paletteOut)

{
  int iVar1;
  
  gPcxInFile = fopen(path,&DAT_10021f60);
  FUN_1000c0f0(gPcxInFile != (FILE *)0x0,PTR_s_D__Newmagic_sources_sidlib_Pcxw__10021ed0,0x9c,
               s_Error_Opening_File__s_10021f48,path);
  gPcxPath = path;
  iVar1 = PcxReadHeaderAndPalette(paletteOut);
  if (iVar1 != 0) {
    fclose(gPcxInFile);
  }
  return iVar1 != 0;
}

// FUNCTION: DRAWCARDLIB 0x100042ea
undefined4 PcxReadHeaderAndPalette(void *paletteOut)

{
  int local_8;
  
  fread(&DAT_10127d40,0x80,1,gPcxInFile);
  FUN_1000c0f0(DAT_10127d40 == '\n',PTR_s_D__Newmagic_sources_sidlib_Pcxw__10021ed0,0xad,
               s__s_Not_a_pcx_file_10021f64,gPcxPath);
  FUN_1000c0f0(DAT_10127d41 == '\x05',PTR_s_D__Newmagic_sources_sidlib_Pcxw__10021ed0,0xae,
               s__s_Not_a_version_5_pcx_file_10021f78,gPcxPath);
  gImageWidth = ((uint)DAT_10127d48 - (uint)DAT_10127d44) + 1;
  gImageHeight = ((uint)DAT_10127d4a - (uint)DAT_10127d46) + 1;
  if (paletteOut != (void *)0x0) {
    if ((DAT_10127d81 == '\x01') && (DAT_10127d43 == '\b')) {
      fseek(gPcxInFile,-0x300,2);
      fread(paletteOut,1,0x300,gPcxInFile);
      fseek(gPcxInFile,0x80,0);
    }
    else if ((DAT_10127d81 == '\x04') && (DAT_10127d43 == '\x01')) {
      fseek(gPcxInFile,0x10,2);
      for (local_8 = 0; local_8 < 0x10; local_8 = local_8 + 1) {
        fread((void *)(local_8 * 4 + (int)paletteOut),1,3,gPcxInFile);
      }
      fseek(gPcxInFile,0x80,0);
    }
    else {
      FUN_1000c0f0(0,PTR_s_D__Newmagic_sources_sidlib_Pcxw__10021ed0,0xd4,
                   s__s_is_not_in_a_recognizable_form_10021f98,gPcxPath);
    }
  }
  return 1;
}

// FUNCTION: DRAWCARDLIB 0x100044d0
undefined4 PcxReadScanlineRle(byte *dstScanline)

{
  uint uVar1;
  int iVar2;
  int local_18;
  uint local_10;
  
  local_18 = (int)gPcxBytesPerLine;
  while (0 < local_18) {
    uVar1 = fgetc(gPcxInFile);
    if (((byte)uVar1 & 0xc0) == 0xc0) {
      uVar1 = uVar1 & 0x3f;
      iVar2 = fgetc(gPcxInFile);
      if (uVar1 < 2) {
        *dstScanline = (byte)iVar2;
        dstScanline = dstScanline + 1;
        local_18 = local_18 + -1;
      }
      else {
        for (local_10 = 0; local_10 < uVar1; local_10 = local_10 + 1) {
          *dstScanline = (byte)iVar2;
          dstScanline = dstScanline + 1;
        }
        local_18 = local_18 - uVar1;
      }
    }
    else {
      *dstScanline = (byte)uVar1;
      dstScanline = dstScanline + 1;
      local_18 = local_18 + -1;
    }
  }
  return 1;
}

// FUNCTION: DRAWCARDLIB 0x100045ac
undefined4
PcxSave8bppImage(undefined *srcPixels,char *path,void* palette,undefined4 unused_param_4,
            undefined4 unused_param_5,int width,int height)

{
  int local_100c;
  undefined1 local_1008 [4064];
  undefined4 uStackY_28;
  undefined1 *puStackY_24;
  undefined *puStackY_20;
  undefined4 uStackY_1c;
  
  uStackY_1c = 0x100045ce;
  gPcxOutFile = fopen(path,&DAT_10021fd8);
  uStackY_1c = 0x146;
  puStackY_20 = PTR_s_D__Newmagic_sources_sidlib_Pcxw__10021ed0;
  puStackY_24 = (undefined1 *)(uint)(gPcxOutFile != (FILE *)0x0);
  uStackY_28 = 0x100045fa;
  FUN_1000c0f0(               (uint)(gPcxOutFile != (FILE *)0x0),
               PTR_s_D__Newmagic_sources_sidlib_Pcxw__10021ed0,0x146,
               s_Error_Opening_File__s_10021fc0);
  gPcxPath = path;
  uStackY_1c = 0x10004612;
  PcxWriteHeader(width,(short)height);
  for (local_100c = 0; local_100c < height; local_100c = local_100c + 1) {
    uStackY_1c = unused_param_4;
    puStackY_20 = srcPixels;
    puStackY_24 = local_1008;
    uStackY_28 = 0x1000465b;
    FUN_1000b80f();
    uStackY_1c = 0x1000466e;
    PcxWriteScanlineRle(local_1008,width);
  }
  PcxWritePalette256(palette);
  fclose(gPcxOutFile);
  return 0;
}

// FUNCTION: DRAWCARDLIB 0x1000469c
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 PcxWriteHeader(undefined4 width,short height)

{
  ushort uVar1;
  ushort uVar2;
  
  DAT_10127d40 = 10;
  DAT_10127d41 = 5;
  DAT_10127d42 = 1;
  DAT_10127d43 = 8;
  DAT_10127d44 = 0;
  uVar1 = (ushort)width;
  DAT_10127d48 = uVar1 - 1;
  DAT_10127d46 = 0;
  DAT_10127d4a = height + -1;
  _DAT_10127d4c = 0;
  _DAT_10127d4e = 0;
  DAT_10127d80 = 0;
  DAT_10127d81 = 1;
  uVar2 = (ushort)((int)width >> 0x1f);
  gPcxBytesPerLine = uVar1 + (((uVar1 ^ uVar2) - uVar2 & 1 ^ uVar2) - uVar2);
  _DAT_10127d84 = 1;
  _DAT_10127d86 = 0;
  _DAT_10127d88 = 0;
  fwrite(&DAT_10127d40,0x80,1,gPcxOutFile);
  return 0;
}

// FUNCTION: DRAWCARDLIB 0x10004760
undefined4 PcxWriteScanlineRle(char *srcScanline,int widthBytes)

{
  int iVar1;
  int local_14;
  uint local_c;
  uint local_8;
  
  local_8 = 0;
  local_14 = 0;
  while (local_14 < widthBytes) {
    local_8 = (local_8 & 0xffffff00U) | (uint)(byte)*srcScanline;
    if ((widthBytes - local_14 == 1) || (srcScanline[1] != *srcScanline)) {
      PcxWriteRleByte(local_8);
      local_14 = local_14 + 1;
      srcScanline = srcScanline + 1;
    }
    else {
      iVar1 = widthBytes - local_14;
      if (0x3e < iVar1) {
        iVar1 = 0x3f;
      }
      local_c = CountRepeats(local_8,srcScanline,iVar1);
      local_14 = local_14 + local_c;
      srcScanline = srcScanline + local_c;
      local_c = local_c | 0xc0;
      fwrite(&local_c,1,1,gPcxOutFile);
      fwrite(&local_8,1,1,gPcxOutFile);
    }
  }
  if (local_14 < gPcxBytesPerLine) {
    local_8 = local_8 & 0xffffff00U;
    fwrite(&local_8,1,1,gPcxOutFile);
  }
  return 1;
}

// FUNCTION: DRAWCARDLIB 0x10004873
void PcxWriteRleByte(byte value)

{
  undefined1 local_8 [4];
  
  local_8[0] = 0xc1;
  if ((value & 0xc0) == 0xc0) {
    fwrite(local_8,1,1,gPcxOutFile);
  }
  fwrite(&value,1,1,gPcxOutFile);
  return;
}

// FUNCTION: DRAWCARDLIB 0x100048c0
int CountRepeats(char value,char *buffer,int maxCount)

{
  int local_8;
  
  local_8 = 0;
  while ((maxCount != 0 && (*buffer == value))) {
    local_8 = local_8 + 1;
    buffer = buffer + 1;
    maxCount = maxCount + -1;
  }
  return local_8;
}

// MATCHING
// FUNCTION: DRAWCARDLIB 0x1000491f
undefined4 PcxWritePalette256(void *palette)
{
  undefined1 local_8 [4];
  
  local_8[0] = 0xc;
  fwrite(local_8,1,1,gPcxOutFile);
  fwrite(palette,3,0x100,gPcxOutFile);
  return 0;
}
