#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <io.h>
#include <stdlib.h>
#include "inttypes.h"
#include "mystdbool.h"
#include "sidlib/pcxw.h"
#include "sidlib/pic.h"
#include "cardartlib/src/palette.h"

typedef unsigned short ushort;

/* Local helpers moved with load_pic(). */
int FUN_1000b745(char *param_1,int param_2);
void FUN_1000b778(int param_1);
void FUN_1000b7a2(undefined4 param_1);
int FUN_1000b7c9(void);
void FUN_1000b804(void);

// FUNCTION: DRAWCARDLIB 0x1000965b
undefined4 * FUN_1000965b(undefined4 param_1,int param_2,int param_3)

{
  int local_10;
  undefined4 *local_c;
  undefined4 *local_8;
  
  if (param_3 == 8) {
    local_8 = malloc(0x42c);
  }
  else if (param_3 == 0x18) {
    local_8 = malloc(0x2c);
  }
  else {
    local_8 = malloc(0x2c);
  }
  *local_8 = 0x28;
  local_8[1] = param_1;
  local_8[2] = -param_2;
  *(undefined2 *)(local_8 + 3) = 1;
  *(short *)((int)local_8 + 0xe) = (short)param_3;
  local_8[4] = 0;
  local_8[5] = 0;
  local_8[6] = 0;
  local_8[7] = 0;
  if (param_3 == 8) {
    local_8[8] = 0x100;
    local_8[9] = 0x100;
    local_c = local_8 + 10;
    for (local_10 = 0; local_10 < 0x100; local_10 = local_10 + 1) {
      *(short *)local_c = (short)local_10;
      local_c = (undefined4 *)((int)local_c + 2);
    }
  }
  else {
    local_8[8] = 0;
    local_8[9] = 0;
  }
  return local_8;
}

// FUNCTION: DRAWCARDLIB 0x1000977f
undefined4 FUN_1000977f(undefined4 param_1)

{
  free((void *)param_1);
  return 1;
}

// GLOBAL: DRAWCARDLIB 0x100f23b0
undefined4 DAT_100f23b0 = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x10022528
// GLOBAL: DECKDLL 0x1003a840
undefined1 *PTR_DAT_10022528 = &DAT_100f23b0;

// GLOBAL: DRAWCARDLIB 0x1002252c
ushort *PTR_DAT_1002252c = (ushort *)0x100F35A0;

// GLOBAL: DRAWCARDLIB 0x10022530
undefined4 DAT_10022530 = 0xFFFFFFFF;

// GLOBAL: DRAWCARDLIB 0x1002253c
char DAT_1002253c[] = ".pcx";

// GLOBAL: DRAWCARDLIB 0x10022544
char DAT_10022544[] = "rb";

// GLOBAL: DRAWCARDLIB 0x100f2394
undefined4 DAT_100f2394 = 0x00000000;
// GLOBAL: DRAWCARDLIB 0x100f239c
undefined4 DAT_100f239c = 0x00000000;
// GLOBAL: DRAWCARDLIB 0x100f23a0
undefined4 DAT_100f23a0 = 0x00000000;
// GLOBAL: DRAWCARDLIB 0x100f23a4
undefined4 DAT_100f23a4 = 0x00000000;
// GLOBAL: DRAWCARDLIB 0x100f23a8
undefined4 _DAT_100f23a8 = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x100f33a0
undefined1 DAT_100f33a0[0x200];

// GLOBAL: DRAWCARDLIB 0x100f3394
ushort *DAT_100f3394 = (ushort *)0x0;
// GLOBAL: DRAWCARDLIB 0x100f35a0
void (__cdecl *DAT_100f35a0)() = (void (__cdecl *)())0x0;

// FUNCTION: DRAWCARDLIB 0x10155000
void __fastcall FUN_10155000(undefined4 param_1,undefined4 param_2,ushort *param_3) {
  param_1 = param_1;
  param_2 = param_2;
  param_3 = param_3;
  //TODO: this needs to be decompiled into a separate static library.
  //Address space layout indicates that's how it was done at Microprose at least
}

// FUNCTION: DRAWCARDLIB 0x10155484
void FUN_10155484() {
  //TODO: this needs to be decompiled into a separate static library
  //Address space layout indicates that's how it was done at Microprose at least
}

// FUNCTION: DRAWCARDLIB 0x1000b1e0
undefined * FUN_1000b1e0(int param_1,int param_2,int param_3)
{
  int iVar1;
  DWORD dwMaximumSizeLow;
  undefined4 uVar2;
  HANDLE pvVar3;
  HDC pHVar4;
  HBITMAP pHVar5;
  undefined *puVar6;
  uint uVar7;
  
  *(int *)(PTR_DAT_10022528 + 0x20) = param_1;
  *(int *)(PTR_DAT_10022528 + 0x24) = param_2;
  *(int *)(PTR_DAT_10022528 + 0x28) = param_3;
  iVar1 = param_3 * param_1 + (param_3 * param_1 >> 0x1f & 7U);
  uVar7 = iVar1 >> 0x1f;
  if (((iVar1 >> 3 ^ uVar7) - uVar7 & 3 ^ uVar7) == uVar7) {
    *(undefined4 *)(PTR_DAT_10022528 + 0x2c) = 0;
  }
  else {
    iVar1 = param_3 * param_1 + (param_3 * param_1 >> 0x1f & 7U);
    uVar7 = iVar1 >> 0x1f;
    *(uint *)(PTR_DAT_10022528 + 0x2c) = 4 - (((iVar1 >> 3 ^ uVar7) - uVar7 & 3 ^ uVar7) - uVar7);
  }
  iVar1 = (param_1 + *(int *)(PTR_DAT_10022528 + 0x2c)) * param_3 * param_2;
  dwMaximumSizeLow = ((int)(iVar1 + (iVar1 >> 0x1f & 7U)) >> 3) + 0x10;
  *(DWORD *)(PTR_DAT_10022528 + 0x1c) = dwMaximumSizeLow;
  uVar2 = FUN_1000965b(param_1,param_2,param_3);
  *(undefined4 *)(PTR_DAT_10022528 + 0x10) = uVar2;
  if (*(int *)(PTR_DAT_10022528 + 0x10) == 0) {
    puVar6 = (undefined *)0x0;
  }
  else {
    pvVar3 = CreateFileMappingA((HANDLE)0xffffffff,(LPSECURITY_ATTRIBUTES)0x0,0x8000004,0,
                                dwMaximumSizeLow,(LPCSTR)0x0);
    *(HANDLE *)PTR_DAT_10022528 = pvVar3;
    if (*(int *)PTR_DAT_10022528 == 0) {
      FUN_1000977f(*(undefined4 *)(PTR_DAT_10022528 + 0x10));
      puVar6 = (undefined *)0x0;
    }
    else {
      pHVar4 = GetDC((HWND)0x0);
      *(HDC *)(PTR_DAT_10022528 + 4) = pHVar4;
      ApplyCardArtPaletteToDc(*(undefined4 *)(PTR_DAT_10022528 + 4));
      pHVar5 = CreateDIBSection(*(HDC *)(PTR_DAT_10022528 + 4),
                                *(BITMAPINFO **)(PTR_DAT_10022528 + 0x10),(uint)(param_3 == 8),
                                (void **)(PTR_DAT_10022528 + 0x18),*(HANDLE *)PTR_DAT_10022528,0);
      *(HBITMAP *)(PTR_DAT_10022528 + 8) = pHVar5;
      ReleaseDC((HWND)0x0,*(HDC *)(PTR_DAT_10022528 + 4));
      if (*(int *)(PTR_DAT_10022528 + 8) == 0) {
        FUN_1000977f(*(undefined4 *)(PTR_DAT_10022528 + 0x10));
        CloseHandle(*(HANDLE *)PTR_DAT_10022528);
        puVar6 = (undefined *)0x0;
      }
      else {
        FUN_1000977f(*(undefined4 *)(PTR_DAT_10022528 + 0x10));
        puVar6 = PTR_DAT_10022528;
      }
    }
  }
  return puVar6;
}

// FUNCTION: DRAWCARDLIB 0x1000b40f
// FUNCTION: DECKDLL 0x1002ceab
undefined4
FUN_1000b40f(int param_1,undefined4 param_2,undefined4 param_3,char *param_4,undefined1 *param_5)

{
  char *_Str2;
  int iVar1;
  int local_414;
  int local_410;
  undefined1 local_40c [1024];
  int local_c;
  int local_8;
  
  param_2 = param_2;
  param_3 = param_3;
  local_8 = 8;
  _Str2 = strchr(param_4,0x2e);
  iVar1 = strcmpi(DAT_1002253c,_Str2);
  if (iVar1 == 0) {
    gPcxInFile = fopen(param_4,DAT_10022544);
    if (gPcxInFile == (FILE *)0x0) {
      return 0;
    }
    gPcxPath = param_4;
    if (param_5 == (undefined1 *)0x1) {
      param_5 = local_40c;
    }
    if (param_5 == (undefined1 *)0x0) {
      PcxReadHeaderAndPalette(0);
      if (param_1 < 0) {
        gImageHeight = 0;
      }
      if ((int)gImageWidth % 3 == 0) {
        local_410 = 0;
      }
      else {
        local_410 = 4 - (int)gImageWidth % 3;
      }
      DAT_100f2394 = gImageWidth + local_410;
      FUN_1000b1e0(DAT_100f2394,gImageHeight,local_8);
      local_c = *(int *)(PTR_DAT_10022528 + 0x18);
      for (DAT_100f239c = 0; DAT_100f239c < gImageHeight; DAT_100f239c = DAT_100f239c + 1) {
        PcxReadScanlineRle(local_c);
        local_c = local_c + ((int)(local_8 + (local_8 >> 0x1f & 7U)) >> 3) * DAT_100f2394;
      }
      fclose(gPcxInFile);
    }
    else {
      PcxReadHeaderAndPalette(param_5 + 6);
      *param_5 = 0x4d;
      param_5[1] = 0x31;
      *(undefined2 *)(param_5 + 2) = 0x300;
      param_5[4] = 0;
      param_5[5] = 0xff;
    }
  }
  else {
    DAT_100f23a0 = FUN_1000b745(param_4,0x8000);
    if (DAT_100f23a0 == -1) {
      return 0;
    }
    FUN_1000b7a2(DAT_100f23a0);
    FUN_10155000(0,0,(ushort *)param_5);
    if ((gImageWidth & 3) == 0) {
      local_414 = 0;
    }
    else {
      local_414 = 4 - (gImageWidth & 3);
    }
    DAT_100f2394 = gImageWidth + local_414;
    iVar1 = FUN_1000b1e0(gImageWidth,gImageHeight,local_8);
    if (iVar1 == 0) {
      *(undefined4 *)(PTR_DAT_10022528 + 8) = 0;
    }
    else {
      local_c = *(int *)(PTR_DAT_10022528 + 0x18);
      DAT_100f239c = 0;
      while (DAT_100f239c < gImageHeight) {
        FUN_10155484(local_c,gImageWidth);
        DAT_100f239c = DAT_100f239c + 1;
        local_c = local_c + *(int *)(PTR_DAT_10022528 + 0x2c) +
                            ((int)(gImageWidth * local_8 +
                                  ((int)(gImageWidth * local_8) >> 0x1f & 7U)) >> 3);
      }
    }
    FUN_1000b778(DAT_100f23a0);
  }
  return *(undefined4 *)(PTR_DAT_10022528 + 8);
}

// MATCHING
// FUNCTION: DRAWCARDLIB 0x1000b700
// FUNCTION: DECKDLL 0x1002d1a1
int load_pic(char *filename)
{
  int iVar1;
  
  iVar1 = FUN_1000b40f(0,0,0,filename,0);
  if (iVar1 != 0) {
    CloseHandle(*(HANDLE *)PTR_DAT_10022528);
  }
  return iVar1;
}

// MATCHING
// FUNCTION: DRAWCARDLIB 0x1000b745
int FUN_1000b745(char *param_1,int param_2)
{
  int iVar1;
  
  iVar1 = _open(param_1,param_2);
  _DAT_100f23a8 = 0xffffffff;
  return iVar1;
}

// FUNCTION: DRAWCARDLIB 0x1000b778
void FUN_1000b778(int param_1)

{
  if (param_1 != DAT_10022530) {
    _close(param_1);
  }
  return;
}

// FUNCTION: DRAWCARDLIB 0x1000b7a2
void FUN_1000b7a2(undefined4 param_1)

{
  DAT_100f23a4 = param_1;
  DAT_100f3394 = PTR_DAT_1002252c;
  DAT_100f35a0 = (void (__cdecl *)())FUN_1000b7c9;
  return;
}

// MATCHING
// FUNCTION: DRAWCARDLIB 0x1000b7c9
int FUN_1000b7c9(void)
{
  int iVar1;
  
  iVar1 = _read(DAT_100f23a4,DAT_100f33a0,0x200);
  DAT_100f3394 = (ushort *)DAT_100f33a0;
  return iVar1;
}

// FUNCTION: DRAWCARDLIB 0x1000b804
void FUN_1000b804(void)
{
  return;
}
