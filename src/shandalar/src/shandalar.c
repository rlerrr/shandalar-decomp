#include <windows.h>
#include <mmsystem.h>
#include <direct.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>
#include <time.h>

#include "defs.h"
#include "magic/src/global_strings.h"
#include "magic/src/shared_startup.h"
#include "shandalar_global_strings.h"
#include "cardartlib/src/assert.h"
#include "cardartlib/src/palette.h"
#include "deckdll/src/card_db.h"
#include "deckdll/src/magsnd.h"
#include "deckdll/src/shared_resources.h"
#include "facemaker/src/facemaker_types.h"
#include <drawcardlib/src/pic.h>

// GLOBAL: SHANDALAR 0x007483f8
HDC global_main_hdc;
// GLOBAL: SHANDALAR 0x005863b8
int global_screen_width = 0x280;
// GLOBAL: SHANDALAR 0x005863bc
int global_screen_height = 0x1e0;
// GLOBAL: SHANDALAR 0x00589de8
const char *PTR_s_advinter800_pic_00589de8 = "advinter800.pic";
// GLOBAL: SHANDALAR 0x00583290
FacemakerWindowBounds DAT_00583290;
// GLOBAL: SHANDALAR 0x005832b4
FacemakerWindowBounds *PTR_DAT_005832b4 = &DAT_00583290;
// GLOBAL: SHANDALAR 0x00748420
HWND DAT_00748420;
// GLOBAL: SHANDALAR 0x00939160
HINSTANCE DAT_00939160;
// GLOBAL: SHANDALAR 0x005a1608
int DAT_005a1608 = 1;
// GLOBAL: SHANDALAR 0x00986950
DIBSurface DAT_00986950;
// GLOBAL: SHANDALAR 0x00591210
int DAT_00591210;
// GLOBAL: SHANDALAR 0x00748418
UINT DAT_00748418;
// GLOBAL: SHANDALAR 0x007483fc
UINT DAT_007483fc;
// GLOBAL: SHANDALAR 0x0078cefc
int DAT_0078cefc;
// GLOBAL: SHANDALAR 0x00589de4
UINT DAT_00589de4 = 0x21;
// GLOBAL: SHANDALAR 0x00589df0
int DAT_00589df0;
// GLOBAL: SHANDALAR 0x00589dec
int DAT_00589dec = 0x30;
// GLOBAL: SHANDALAR 0x00748400
int DAT_00748400;
// GLOBAL: SHANDALAR 0x00748404
HANDLE DAT_00748404;
// GLOBAL: SHANDALAR 0x00748424
HANDLE DAT_00748424;
// GLOBAL: SHANDALAR 0x00748408
int DAT_00748408;
// GLOBAL: SHANDALAR 0x0074840c
int _DAT_0074840c;
// GLOBAL: SHANDALAR 0x00748410
HANDLE _DAT_00748410;
// GLOBAL: SHANDALAR 0x005b7d90
int DAT_005b7d90;
// GLOBAL: SHANDALAR 0x005b7d94
int DAT_005b7d94;
// GLOBAL: SHANDALAR 0x005b7d98
HWND DAT_005b7d98;
// GLOBAL: SHANDALAR 0x00986d94
int DAT_00986d94;
// GLOBAL: SHANDALAR 0x00986d98
int DAT_00986d98;
// GLOBAL: SHANDALAR 0x00986d9c
int DAT_00986d9c;
// GLOBAL: SHANDALAR 0x00986da0
int DAT_00986da0;

// GLOBAL: SHANDALAR 0x00669704
int DAT_00669704;
// GLOBAL: SHANDALAR 0x0066970c
int DAT_0066970c;
// GLOBAL: SHANDALAR 0x0073e890
char DAT_0073e890[0x100];
// GLOBAL: SHANDALAR 0x0073e9d8
char DAT_0073e9d8;
// GLOBAL: SHANDALAR 0x0067a3b8
char *DAT_0067a3b8;
// GLOBAL: SHANDALAR 0x005919ac
int DAT_005919ac = 0x005c3a61;
// GLOBAL: SHANDALAR 0x0078cf08
FILE *DAT_0078cf08;
// GLOBAL: SHANDALAR 0x0078df10
char DAT_0078df10[0x28];
// GLOBAL: SHANDALAR 0x0078df38
int DAT_0078df38;
// GLOBAL: SHANDALAR 0x00586494
int DAT_00586494;
// GLOBAL: SHANDALAR 0x009300f0
int DAT_009300f0;
// GLOBAL: SHANDALAR 0x008e3820
int DAT_008e3820;
// GLOBAL: SHANDALAR 0x007a0098
int DAT_007a0098;
// GLOBAL: SHANDALAR 0x007beaa0
int DAT_007beaa0[4];
// GLOBAL: SHANDALAR 0x007486d0
int DAT_007486d0;
// GLOBAL: SHANDALAR 0x0078cf04
int DAT_0078cf04;
// GLOBAL: SHANDALAR 0x00591204
int DAT_00591204;
// GLOBAL: SHANDALAR 0x00591208
int DAT_00591208;
// GLOBAL: SHANDALAR 0x0073ea88
int DAT_0073ea88;
// GLOBAL: SHANDALAR 0x00930adc
int DAT_00930adc;
// GLOBAL: SHANDALAR 0x00930ae0
int DAT_00930ae0;
// GLOBAL: SHANDALAR 0x0094d634
int DAT_0094d634;
// GLOBAL: SHANDALAR 0x0078df34
int DAT_0078df34;
// GLOBAL: SHANDALAR 0x0073e9dc
int DAT_0073e9dc;
// GLOBAL: SHANDALAR 0x00591214
int DAT_00591214;
// GLOBAL: SHANDALAR 0x005911f8
int DAT_005911f8;
// GLOBAL: SHANDALAR 0x005911fc
int DAT_005911fc;
// GLOBAL: SHANDALAR 0x00789940
unsigned char DAT_00789940[0x3200];
// GLOBAL: SHANDALAR 0x0073ea70
int DAT_0073ea70[8];
// GLOBAL: SHANDALAR 0x00669700
int DAT_00669700;
// GLOBAL: SHANDALAR 0x00669710
int DAT_00669710;
// GLOBAL: SHANDALAR 0x00789938
int DAT_00789938;
// GLOBAL: SHANDALAR 0x0078df68
int DAT_0078df68;
// GLOBAL: SHANDALAR 0x0097f1c1
unsigned char DAT_0097f1c1[2000];
// GLOBAL: SHANDALAR 0x0078cf00
int DAT_0078cf00;
// GLOBAL: SHANDALAR 0x007898f4
int DAT_007898f4;
// GLOBAL: SHANDALAR 0x007898f8
int DAT_007898f8;
// GLOBAL: SHANDALAR 0x007898f0
int DAT_007898f0;
// GLOBAL: SHANDALAR 0x007490f0
int DAT_007490f0;
// GLOBAL: SHANDALAR 0x0058e050
HANDLE DAT_0058e050;
// GLOBAL: SHANDALAR 0x0058e054
char s_statwin_dll_0058e054[] = "statwin.dll";
// GLOBAL: SHANDALAR 0x00746e00
int DAT_00746e00[3];
// GLOBAL: SHANDALAR 0x008c84f0
BITMAPINFO DAT_008c84f0;
// GLOBAL: SHANDALAR 0x0073e990
jmp_buf DAT_0073e990;
// GLOBAL: SHANDALAR 0x0073e9e0
jmp_buf DAT_0073e9e0;
static FacemakerWindowBounds s_ui_rect_005832dc;
static FacemakerWindowBounds s_ui_rect_00583304;
static FacemakerWindowBounds s_ui_rect_0058332c;
static FacemakerWindowBounds s_ui_rect_00583354;
// GLOBAL: SHANDALAR 0x005832dc
FacemakerWindowBounds *PTR_DAT_005832dc = &s_ui_rect_005832dc;
// GLOBAL: SHANDALAR 0x00583304
FacemakerWindowBounds *PTR_DAT_00583304 = &s_ui_rect_00583304;
// GLOBAL: SHANDALAR 0x0058332c
FacemakerWindowBounds *PTR_DAT_0058332c = &s_ui_rect_0058332c;
// GLOBAL: SHANDALAR 0x00583354
FacemakerWindowBounds *PTR_DAT_00583354 = &s_ui_rect_00583354;

/* 0x00589de4: hardcoded timer interval in original binary. */

int InitLicenseSecretsFromRegistry(void);
int FUN_00564ee7(const char *filename);
int FUN_0056cc4d(const char *filename, const char *section);
int FUN_00565c7e(const char *filename, const char *section, char **out_table, int max_entries, char *string_buf,
                 char *string_buf_end, char **out_next_buf);
int FUN_00565dbc(const char *filename);
int FUN_00565fdb(char *param_1, char *param_2, int *param_3, int *param_4);
void FUN_00559999(void);
void *CreateGraphicsPage(int page_number, int width, int height, int bits_per_pixel);
void FUN_00562d03(void);
void FUN_00565faa(void);
void QueueKeyInputFromMessage(WPARAM wparam, LPARAM lparam);
ATOM RegisterPaletteClass(HINSTANCE hinst);
HWND CreatePalettePopupWindow(HINSTANCE hinst, HWND parent_hwnd);
int *LoadIniEscapedStringTable(FILE *ini_file, char *section_name, int unk1, int unk2);
int FUN_00578c80(int param_1);
int FUN_00578c20(void);
int FUN_00578c30(void);
void *InitializeGraphicsSystemDefaultMode(void);
void SetGraphicsPage(int page_number, void *page);
void PresentGraphicsPage(int page_number);
int ReadPalette(char *palette_text_path, char *palette_binary_path);
int ScaleUiCoordinate(int value);
int LoadFontConfigIfPresent(char *executable_name, char *config_name);
int LoadSystemFont(int font_id, unsigned int point_size, char *font_file, char *font_name, int weight, DWORD italic);
int FUN_0055db50(void);
BOOL FUN_0057a9f0(int param_1);
int FUN_00417dc6(const char *filename);
unsigned int FUN_00562e0d(char *filename);
char FUN_00562ed0(void);
unsigned int FUN_00562f92(char *filename, int param_2, int param_3);
LONG ChangeDisplayResolution(DWORD width, DWORD height);
void RestoreDisplayResolution(void);
DWORD WINAPI FUN_0046e6f0(LPVOID);
int FUN_00578c70(int param_1, int param_2, int param_3);
void FUN_004184d2(void);
void FUN_0041786e(void);
void FUN_0046ed03(void);
void FUN_0046ed33(void);
void FUN_00578c40(void);
void FUN_00578c50(void);
void ReadSpriteEntryPointers(void *param_1, const char *param_2);
void AnimatePaletteToColor(int color_index, int palette_id);
int FUN_004a0834(void);
int FUN_004a1194(void);
int FUN_004a18f0(void);
int FUN_00522508(int param_1);
int FUN_004a21ad(void);
void FUN_004bdd0a(void);
void FUN_004f6d90(void);
void FUN_005081fa(void);
unsigned int FUN_0043146b(int param_1, int param_2);
unsigned int FUN_005611c8(unsigned int param_1);
unsigned int FUN_004314ca(int param_1, int param_2);
void FUN_005019ad(int param_1);
int FUN_005031a8(void);
void FUN_00501760(int param_1);
int FUN_0040dffd(unsigned char param_1);
int FUN_005226e0(void);
void FUN_004bbc50(void);
void FUN_004ffda4(void);
void FUN_005631ca(void);
void FUN_0056bff1(void);
void FUN_0054cdbd(void);
void FUN_004ce955(void);
void FUN_0054ac08(int param_1, int param_2, int param_3);
void FUN_0055e1b2(void);
void FUN_0055e651(void);
void FUN_0055e808(void);
void FUN_0055fd27(void);
void FUN_00561647(void);
void FUN_0046eca4(void);
void FUN_005003b7(int param_1, int param_2, int param_3);
void FUN_004ecfa2(void);
void FUN_004ecfe3(void);
void FUN_00522786(void);
int HasQueuedKeyInput(void);
void FUN_00414127(void);
void destroy_create_fonts_resources(void);
int FUN_0057c580(int param_1, int param_2, int param_3, const char *param_4, int param_5);
extern char *global_base_txt;
extern HPALETTE global_cart_art_hpalette;
extern int deck[500];
extern int Gold;

// FUNCTION: SHANDALAR 0x00578c80
int FUN_00578c80(int param_1)
{
  return 0;
}

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

// FUNCTION: SHANDALAR 0x005501fe
int ScaleUiCoordinate(int value)
{
  return (value * global_screen_width) / 0x280;
}

// FUNCTION: SHANDALAR 0x0055db50
int FUN_0055db50(void)
{
  int initialized_world;
  int game_setup_mode;
  unsigned int tile_mask;
  int random_value;
  clock_t t;
  int i;
  int proceed_to_main_loop;

  initialized_world = 0;
  DAT_008e3820 = -1;
  DAT_007a0098 = -1;
  FUN_00559999();
  for (i = 0; i < 4; i = i + 1)
  {
    DAT_007beaa0[i] = 8;
  }

  FUN_004184d2();
  FUN_0041786e();
  FUN_00578c70(1, 1, DAT_007486d0);
  FUN_0046ed03();
  FUN_0046ed33();
  GdiGetBatchLimit();
  GdiSetBatchLimit(100);
  LoadPcxIntoPageNoPalette("advfac64.pic");
  ReadSpriteEntryPointers(&DAT_007490f0, "dbox.spr");

  while (1)
  {
    game_setup_mode = FUN_004a0834();
    AnimatePaletteToColor(0, DAT_00589dec);
    proceed_to_main_loop = 0;

    switch (game_setup_mode)
    {
    case 0:
      while (1)
      {
        DAT_0078cf04 = FUN_004a1194();
        AnimatePaletteToColor(0, DAT_00589dec);
        if (DAT_0078cf04 == -1)
        {
          break;
        }

        while (1)
        {
          DAT_00591204 = FUN_004a18f0();
          DAT_0073ea88 = DAT_00591204;
          DAT_00930adc = DAT_00591204;
          DAT_00930ae0 = FUN_00522508(3);
          AnimatePaletteToColor(0, DAT_00589dec);
          if (DAT_0073ea88 == -1)
          {
            break;
          }

          DAT_0094d634 = FUN_004a21ad();
          AnimatePaletteToColor(0, DAT_00589dec);
          if (DAT_0094d634 == -1)
          {
            continue;
          }

          DAT_00591208 = 1 << ((unsigned char)DAT_00591204 & 0x1f);
          LoadPcxIntoPageNoPalette("advfac64.pic");
          LoadPcxIntoPage(1, PTR_s_advinter800_pic_00589de8);
          BlitGraphicsRect((int *)PTR_DAT_005832dc, 0, 0, global_screen_width, global_screen_height, (int *)PTR_DAT_005832b4, 0, 0);
          DAT_0078df34 = 0;
          setup_shared_startup();
          initialized_world = 1;
          PTR_DAT_005832b4->font_slot = 5;
          FUN_0056cc4d("ADVstrings.txt", "STARTUP");
          DrawTextAt((int *)PTR_DAT_005832b4, 0xff, 0x140, 0xbc);
          DAT_0073e9dc = 1;
          FUN_004bdd0a();
          FUN_004f6d90();
          FUN_005081fa();
          Gold = (5 - DAT_0078cf04) * 0x32;
          DAT_00591214 = 0;
          do
          {
            do
            {
              random_value = FUN_00522508(0x40);
              DAT_005911f8 = random_value * 0x20 + 0x10;
              random_value = FUN_00522508(0x40);
              DAT_005911fc = random_value * 0x20 + 0x10;
              tile_mask =
                  FUN_0043146b((DAT_005911f8 + ((int)DAT_005911f8 >> 0x1f & 0x1fU)) >> 5,
                               (DAT_005911fc + ((int)DAT_005911fc >> 0x1f & 0x1fU)) >> 5);
              tile_mask = FUN_005611c8(tile_mask);
            } while ((DAT_00591208 & tile_mask) == 0);

            tile_mask =
                FUN_004314ca((DAT_005911f8 + ((int)DAT_005911f8 >> 0x1f & 0x1fU)) >> 5,
                             (DAT_005911fc + ((int)DAT_005911fc >> 0x1f & 0x1fU)) >> 5);
          } while ((tile_mask & 0x10) != 0);

          FUN_005019ad(3);
          proceed_to_main_loop = 1;
          break;
        }

        if (proceed_to_main_loop != 0)
        {
          break;
        }
      }
      break;

    case 1:
      DAT_0078df34 = 0;
      random_value = FUN_005031a8();
      FUN_00501760(random_value);
      proceed_to_main_loop = 1;
      break;

    case 2:
      DAT_0078df34 = 0;
      FUN_00501760(3);
      proceed_to_main_loop = 1;
      break;

    case 3:
      proceed_to_main_loop = 1;
      break;

    case 4:
      DAT_009300f0 = 1;
      return 0;

    default:
      proceed_to_main_loop = 1;
      break;
    }

    if (proceed_to_main_loop == 0)
    {
      continue;
    }

    for (i = 0; i < 0x80; i = i + 1)
    {
      if (*(int *)(DAT_00789940 + i * 100) == 5)
      {
        tile_mask = FUN_0043146b(*(int *)(DAT_00789940 + i * 100 + 4), *(int *)(DAT_00789940 + i * 100 + 8));
        tile_mask = FUN_005611c8(tile_mask);
        random_value = FUN_0040dffd((unsigned char)tile_mask);
        DAT_0073ea70[random_value - 1] = 1;
      }
    }

    FUN_005226e0();
    if (!initialized_world)
    {
      setup_shared_startup();
    }
    FUN_004bbc50();
    FUN_004ffda4();
    FUN_005631ca();
    if (game_setup_mode != 0)
    {
      FUN_0056bff1();
    }

    LoadPcxIntoPageNoPalette("advfac64.pic");
    FUN_0054cdbd();
    DAT_00669700 = 0;
    DAT_00789938 = 0;
    DAT_0078df68 = 0;
    for (i = 0; i < 500; i = i + 1)
    {
      if ((deck[i] != -1) && ((DAT_00789938 = DAT_00789938 + 1), (DAT_0097f1c1[i * 4] & 0x40) == 0))
      {
        DAT_0078df68 = DAT_0078df68 + 1;
      }
    }

    _setjmp3(&DAT_0073e990, 0);
    _setjmp3(&DAT_0073e9e0, 0);
    while (DAT_009300f0 == 0)
    {
      FUN_004ce955();
      FUN_0054ac08(DAT_005911f8, DAT_005911fc, DAT_00669710);
      DAT_00669710 = 0;
      do
      {
        t = clock();
      } while (t < 0x3c);
      clock();

      if ((DAT_00986d94 & 2U) != 0)
      {
        FUN_0055e1b2();
      }
      FUN_0055e651();
      FUN_0055e808();
      FUN_0055fd27();
      FUN_00561647();
      FUN_0046eca4();
      FUN_005003b7(DAT_007898f4, DAT_007898f8, DAT_00986d94);
      DAT_0078cf00 = DAT_0078cf00 + 1;
      FUN_004ecfa2();
      FUN_004ecfe3();
      _setjmp3(&DAT_0073e9e0, 0);
    }

    AnimatePaletteToColor(0, DAT_00589dec);
    FUN_00522786();
    return FUN_00469099();
  }
}

// FUNCTION: SHANDALAR 0x0057a9f0
BOOL FUN_0057a9f0(int param_1)
{
  (void)param_1;
  return FALSE;
}

// FUNCTION: SHANDALAR 0x00578c70
int FUN_00578c70(int param_1, int param_2, int param_3)
{
  (void)param_1;
  (void)param_2;
  (void)param_3;
  return 0;
}

// FUNCTION: SHANDALAR 0x004184d2
void FUN_004184d2(void) {}
// FUNCTION: SHANDALAR 0x0041786e
void FUN_0041786e(void) {}
// FUNCTION: SHANDALAR 0x00578c40
void FUN_00578c40(void)
{
  ShowCursor(1);
}

// FUNCTION: SHANDALAR 0x00578c50
void FUN_00578c50(void)
{
  ShowCursor(0);
}

// FUNCTION: SHANDALAR 0x0046ed03
void FUN_0046ed03(void)
{
  DAT_0078cefc = DAT_0078cefc + 1;
  if ((DAT_00586494 != 0) && (DAT_0078cefc == 1))
  {
    FUN_00578c40();
  }
}

// FUNCTION: SHANDALAR 0x0046ed33
void FUN_0046ed33(void)
{
  if ((DAT_00586494 != 0) && (DAT_0078cefc == 1))
  {
    FUN_00578c50();
  }
  DAT_0078cefc = DAT_0078cefc - 1;
}

// FUNCTION: SHANDALAR 0x004a0834
int FUN_004a0834(void) { return 0; }
// FUNCTION: SHANDALAR 0x004a1194
int FUN_004a1194(void) { return -1; }
// FUNCTION: SHANDALAR 0x004a18f0
int FUN_004a18f0(void) { return -1; }
// FUNCTION: SHANDALAR 0x00522508
int FUN_00522508(int param_1)
{
  return (param_1 > 1) ? rand() % param_1 : 0;
}

// FUNCTION: SHANDALAR 0x004a21ad
int FUN_004a21ad(void) { return -1; }

// FUNCTION: SHANDALAR 0x004bdd0a
void FUN_004bdd0a(void) {}

// FUNCTION: SHANDALAR 0x004f6d90
void FUN_004f6d90(void) {}

// FUNCTION: SHANDALAR 0x005081fa
void FUN_005081fa(void) {}

// FUNCTION: SHANDALAR 0x0043146b
unsigned int FUN_0043146b(int param_1, int param_2)
{
  (void)param_1;
  (void)param_2;
  return 0;
}

// FUNCTION: SHANDALAR 0x005611c8
unsigned int FUN_005611c8(unsigned int param_1) { return param_1; }

// FUNCTION: SHANDALAR 0x004314ca
unsigned int FUN_004314ca(int param_1, int param_2)
{
  (void)param_1;
  (void)param_2;
  return 0;
}

// FUNCTION: SHANDALAR 0x005019ad
void FUN_005019ad(int param_1) { (void)param_1; }

// FUNCTION: SHANDALAR 0x005031a8
int FUN_005031a8(void) { return 0; }

// FUNCTION: SHANDALAR 0x00501760
void FUN_00501760(int param_1) { (void)param_1; }

// FUNCTION: SHANDALAR 0x0040dffd
int FUN_0040dffd(unsigned char param_1)
{
  (void)param_1;
  return 1;
}

// FUNCTION: SHANDALAR 0x005226e0
int FUN_005226e0(void)
{
  int i;

  for (i = 0; i < 3; i = i + 1)
  {
    DAT_00746e00[i] = 0;
  }

  DAT_0058e050 = LoadLibraryA(s_statwin_dll_0058e054);
  if (DAT_0058e050 == (HANDLE)0)
  {
    return 1;
  }

  for (i = 0; i < 3; i = i + 1)
  {
    DAT_00746e00[i] = (int)GetProcAddress((HMODULE)DAT_0058e050, (LPCSTR)((i + 1U) & 0xffff));
  }

  return 0;
}

// FUNCTION: SHANDALAR 0x004bbc50
void FUN_004bbc50(void) {}

// FUNCTION: SHANDALAR 0x004ffda4
void FUN_004ffda4(void) {}

// FUNCTION: SHANDALAR 0x005631ca
void FUN_005631ca(void) {}

// FUNCTION: SHANDALAR 0x0056bff1
void FUN_0056bff1(void) {}

// FUNCTION: SHANDALAR 0x0054cdbd
void FUN_0054cdbd(void) {}

// FUNCTION: SHANDALAR 0x004ce955
void FUN_004ce955(void) {}

// FUNCTION: SHANDALAR 0x0054ac08
void FUN_0054ac08(int param_1, int param_2, int param_3)
{
  (void)param_1;
  (void)param_2;
  (void)param_3;
}

// FUNCTION: SHANDALAR 0x0055e1b2
void FUN_0055e1b2(void) {}

// FUNCTION: SHANDALAR 0x0055e651
void FUN_0055e651(void) {}

// FUNCTION: SHANDALAR 0x0055e808
void FUN_0055e808(void) {}

// FUNCTION: SHANDALAR 0x0055fd27
void FUN_0055fd27(void) {}

// FUNCTION: SHANDALAR 0x00561647
void FUN_00561647(void) {}

// FUNCTION: SHANDALAR 0x0046eca4
void FUN_0046eca4(void) {}

// FUNCTION: SHANDALAR 0x005003b7
void FUN_005003b7(int param_1, int param_2, int param_3)
{
  (void)param_1;
  (void)param_2;
  (void)param_3;
}

// FUNCTION: SHANDALAR 0x004ecfa2
void FUN_004ecfa2(void)
{
  if (DAT_00586494 != 0)
  {
    do
    {
      FUN_0046eca4();
    } while (DAT_007898f0 != 0);
  }

  while (HasQueuedKeyInput() != 0)
  {
    FUN_00414127();
  }
}

// FUNCTION: SHANDALAR 0x004ecfe3
void FUN_004ecfe3(void)
{
  while (HasQueuedKeyInput() != 0)
  {
    FUN_00414127();
  }
}

// FUNCTION: SHANDALAR 0x00522786
void FUN_00522786(void)
{
  int i;

  if (DAT_0058e050 != (HANDLE)0)
  {
    FreeLibrary((HMODULE)DAT_0058e050);
    DAT_0058e050 = (HANDLE)0;
  }

  for (i = 0; i < 3; i = i + 1)
  {
    DAT_00746e00[i] = 0;
  }
}

// FUNCTION: SHANDALAR 0x00414127
void FUN_00414127(void) {}

// FUNCTION: SHANDALAR 0x0057c580
int FUN_0057c580(int param_1, int param_2, int param_3, const char *param_4, int param_5)
{
  (void)param_1;
  (void)param_2;
  (void)param_3;
  (void)param_4;
  (void)param_5;
  return 0;
}

// FUNCTION: SHANDALAR 0x00564ee7
int FUN_00564ee7(const char *filename)
{
  struct
  {
    int ok;
    int local_c;
    char *local_8;
    int iVar36;
  } s;

  s.ok = 1;
  s.ok &= FUN_00565c7e(filename, "PLAYERNAMES", gs_playernames_0077c5a0, 0xe, DAT_0077e2c0, DAT_0077e2c0 + 0x2bc, (char **)0);
  s.ok &= FUN_00565c7e(filename, "DIFFICULTYLEVELS", gs_difficultylevels_0077d130, 4, DAT_0074d790, DAT_0074d790 + 0x64, (char **)0);
  s.ok &= FUN_00565c7e(filename, "DUNGEON_NAMES", gs_dungeon_names_00780820, 0x11, DAT_0074c970, DAT_0074c970 + 0x352, (char **)0);
  s.ok &= FUN_00565c7e(filename, "LAIR_NAMES", gs_lair_names_0077c020, 0x13, DAT_0074bd30, DAT_0074bd30 + 0x3b6, (char **)0);

  FUN_0056cc4d(filename, "CITYNAMES_FORMAT");
  strcpy(DAT_0074c950, text_lines[0]);
  strcpy(DAT_0077f190, text_lines[1]);

  FUN_0056cc4d(filename, "CITYNAME_VILLAGE");
  strcpy(DAT_0077cfd0, text_lines[0]);

  FUN_0056cc4d(filename, "CITYNAME_CASTLE");
  strcpy(DAT_00765dc0, text_lines[0]);

  FUN_0056cc4d(filename, "CITYNAME_MANACASTLE");

  for (s.local_c = 0; s.local_c < 5; s.local_c = s.local_c + 1)
  {
    s.iVar36 = s.local_c * 4 + 4;
    s.iVar36 = s.iVar36 + s.iVar36 * 4;
    s.iVar36 = s.iVar36 + s.iVar36 * 4;
    strcpy(DAT_0077de00 + s.iVar36, text_lines[s.local_c]);
  }
  strcpy(DAT_0077de00, DAT_00765dc0);

  s.local_8 = DAT_0077d610;
  s.ok &= FUN_00565c7e(filename, "CITYNAMES_FIRSTHALF", gs_citynames_firsthalf_0077e060, 0x10, DAT_0077d610,
                       DAT_0077d610 + sizeof(gs_citynames_buf_0077d610), &s.local_8);
  s.ok &= FUN_00565c7e(filename, "CITYNAMES_SECONDHALF", gs_citynames_secondhalf_007653e0, 0x10, s.local_8,
                       DAT_0077d610 + sizeof(gs_citynames_buf_0077d610), (char **)0);

  s.local_8 = DAT_0074b160;
  FUN_0056cc4d(filename, "WORLDMAGIC");
  strcpy(DAT_0077e1d0, text_lines[0]);
  s.ok &= FUN_00565c7e(filename, "WORLDMAGIC_NAMES", gs_worldmagic_names_00780660, 0xc, DAT_0074b160,
                       DAT_0074b160 + sizeof(gs_worldmagic_buf_0074b160), &s.local_8);
  s.ok &= FUN_00565c7e(filename, "WORLDMAGIC_EXPLAINS", gs_worldmagic_explains_0074b8f0, 0xc, s.local_8,
                       DAT_0074b160 + sizeof(gs_worldmagic_buf_0074b160), (char **)0);

  s.ok &= FUN_00565c7e(filename, "LOGSTRINGS", gs_logstrings_0077c9a0, 0x10, DAT_0077c680,
                       DAT_0077c680 + sizeof(gs_logstrings_buf_0077c680), (char **)0);

  FUN_0056cc4d(filename, "WIZARDNAMES");
  for (s.local_c = 0; s.local_c < 5; s.local_c = s.local_c + 1)
  {
    strcpy(DAT_0077ee70 + (s.local_c * 5 + 5) * 10, text_lines[s.local_c]);
  }
  strcpy(DAT_0077ee70, "");

  s.iVar36 = FUN_0056cc4d(filename, "CREATURENAMES");
  if ((int)DAT_00593934 <= s.iVar36)
  {
    s.iVar36 = DAT_00593934;
  }
  for (s.local_c = 0; s.local_c < s.iVar36; s.local_c = s.local_c + 1)
  {
    strcpy(DAT_00591a12 + s.local_c * 0x8c, text_lines[s.local_c]);
  }
  while (s.local_c = s.iVar36, s.local_c < (int)DAT_00593934)
  {
    strcpy(DAT_00591a12 + s.local_c * 0x8c, "");
    s.iVar36 = s.local_c + 1;
  }

  s.iVar36 = FUN_0056cc4d(filename, "CREATURENAME_ARTICLES");
  if ((int)DAT_00593934 <= s.iVar36)
  {
    s.iVar36 = DAT_00593934;
  }
  for (s.local_c = 0; s.local_c < s.iVar36; s.local_c = s.local_c + 1)
  {
    strcpy(DAT_00591a08 + s.local_c * 0x8c, text_lines[s.local_c]);
  }
  while (s.local_c = s.iVar36, s.local_c < (int)DAT_00593934)
  {
    strcpy(DAT_00591a08 + s.local_c * 0x8c, "");
    s.iVar36 = s.local_c + 1;
  }

  s.iVar36 = FUN_0056cc4d(filename, "CREATURENAMES_PLURAL");
  if ((int)DAT_00593934 <= s.iVar36)
  {
    s.iVar36 = DAT_00593934;
  }
  for (s.local_c = 0; s.local_c < s.iVar36; s.local_c = s.local_c + 1)
  {
    strcpy(DAT_00591a44 + s.local_c * 0x8c, text_lines[s.local_c]);
  }
  while (s.local_c = s.iVar36, s.local_c < (int)DAT_00593934)
  {
    strcpy(DAT_00591a44 + s.local_c * 0x8c, "");
    s.iVar36 = s.local_c + 1;
  }

  FUN_0056cc4d(filename, "DIRECTIONS");
  for (s.local_c = 0; s.local_c < 4; s.local_c = s.local_c + 1)
  {
    strcpy(DAT_00765d50 + s.local_c * 0x19, text_lines[s.local_c]);
  }

  s.local_8 = DAT_0074c5c0;
  s.ok &= FUN_00565c7e(filename, "CARDCLASSNAMES", gs_cardclassnames_0077cf70, 9, DAT_0074c5c0,
                       DAT_0074c5c0 + sizeof(gs_cardclassnames_buf_0074c5c0), &s.local_8);
  s.ok &= FUN_00565c7e(filename, "CARDCLASSNAMES_PLURAL", gs_cardclassnames_plural_0077e1f0, 9, s.local_8,
                       DAT_0074c5c0 + sizeof(gs_cardclassnames_buf_0074c5c0), (char **)0);

  FUN_0056cc4d(filename, "SPELLNAMES");
  strcpy(DAT_0077e6e0, text_lines[0]);
  for (s.local_c = 1; s.local_c < 5; s.local_c = s.local_c + 1)
  {
    strcpy(DAT_0077e220 + s.local_c * 0x19, text_lines[s.local_c]);
  }
  strcpy(DAT_0077e220, text_lines[6]);

  FUN_0056cc4d(filename, "MANANAMES");
  for (s.local_c = 0; s.local_c < 5; s.local_c = s.local_c + 1)
  {
    strcpy(DAT_0074d980 + (s.local_c * 5 + 5) * 5, text_lines[s.local_c]);
  }
  strcpy(DAT_0074d980, "");

  FUN_0056cc4d(filename, "LANDWALKS");
  for (s.local_c = 0; s.local_c < 5; s.local_c = s.local_c + 1)
  {
    strcpy(DAT_007806f0 + (s.local_c * 5 + 5) * 10, text_lines[s.local_c]);
  }
  strcpy(DAT_007806f0, "");

  FUN_0056cc4d(filename, "AMULETNAMES");
  for (s.local_c = 0; s.local_c < 6; s.local_c = s.local_c + 1)
  {
    strcpy(DAT_0077d090 + s.local_c * 0x19, text_lines[s.local_c]);
  }

  FUN_0056cc4d(filename, "AMULETNAMES_PLURAL");
  for (s.local_c = 0; s.local_c < 6; s.local_c = s.local_c + 1)
  {
    strcpy(DAT_0077edd0 + s.local_c * 0x19, text_lines[s.local_c]);
  }

  FUN_0056cc4d(filename, "COLORCARDS");
  for (s.local_c = 0; s.local_c < 6; s.local_c = s.local_c + 1)
  {
    strcpy(DAT_0077c5e0 + s.local_c * 0x19, text_lines[s.local_c]);
  }

  s.ok &= FUN_00565c7e(filename, "CAVE_SHOWCLUES", gs_cave_showclues_0077efa0, 0x17, DAT_007658d0,
                       DAT_007658d0 + sizeof(gs_cave_showclues_buf_007658d0), (char **)0);

  s.local_8 = DAT_0074da70;
  s.ok &= FUN_00565c7e(filename, "ENCOUNTER_PREDUEL", gs_encounter_preduel_0077f0d0, 0x30, DAT_0074da70,
                       DAT_0074da70 + sizeof(gs_encounter_buf_0074da70), &s.local_8);
  s.ok &= FUN_00565c7e(filename, "ENCOUNTER_POSTDUEL", gs_encounter_postduel_0077f050, 0x20, s.local_8,
                       DAT_0074da70 + sizeof(gs_encounter_buf_0074da70), (char **)0);

  s.local_8 = DAT_0077f610;
  s.ok &= FUN_00565c7e(filename, "VISIT", gs_visit_0077c4f0, 0x23, DAT_0077f610, DAT_0077f610 + sizeof(gs_visit_buf_0077f610),
                       &s.local_8);
  s.ok &= FUN_00565c7e(filename, "VISIT_CITYBUY", gs_visit_citybuy_0077f1d0, 4, s.local_8,
                       DAT_0077f610 + sizeof(gs_visit_buf_0077f610), (char **)0);

  s.ok &= FUN_00565c7e(filename, "CASTLEWIN", gs_castlewin_0074b8c0, 0xb, DAT_0077e700,
                       DAT_0077e700 + sizeof(gs_castlewin_buf_0077e700), (char **)0);
  s.ok &= FUN_00565c7e(filename, "DUNGEON", gs_dungeon_0077f000, 0x14, DAT_00780870, DAT_00780870 + sizeof(gs_dungeon_buf_00780870),
                       (char **)0);
  s.ok &= FUN_00565c7e(filename, "HINTTEXT", gs_hinttext_0077e580, 2, DAT_0077e5a0, DAT_0077e5a0 + sizeof(gs_hinttext_buf_0077e5a0),
                       (char **)0);
  s.ok &= FUN_00565c7e(filename, "QUESTFAILED", gs_questfailed_0077c580, 6, DAT_0074b930,
                       DAT_0074b930 + sizeof(gs_questfailed_buf_0074b930), (char **)0);
  s.ok &= FUN_00565c7e(filename, "MONSTERLAIR", gs_monsterlair_0074cff0, 6, DAT_0074d010,
                       DAT_0074d010 + sizeof(gs_monsterlair_buf_0074d010), (char **)0);
  s.ok &= FUN_00565c7e(filename, "BUYANYCARD", gs_buyanycard_0074ccd0, 6, DAT_0077f1e0,
                       DAT_0077f1e0 + sizeof(gs_buyanycard_buf_0077f1e0), (char **)0);
  s.ok &= FUN_00565c7e(filename, "QUESTSTATUS", gs_queststatus_0077e0a0, 0x1a, DAT_0074d270,
                       DAT_0074d270 + sizeof(gs_queststatus_buf_0074d270), (char **)0);
  s.ok &= FUN_00565c7e(filename, "NEWSFLASH", gs_newsflash_0077d140, 0xb, DAT_0077d1c0,
                       DAT_0077d1c0 + sizeof(gs_newsflash_buf_0077d1c0), (char **)0);
  s.ok &= FUN_00565c7e(filename, "CITYCARDTEXT", gs_citycardtext_0074ccf0, 0x31, DAT_0074c0f0,
                       DAT_0074c0f0 + sizeof(gs_citycardtext_buf_0074c0f0), (char **)0);
  s.ok &= FUN_00565c7e(filename, "BROWSE", gs_browse_0074da20, 0x11, DAT_0077d610 + sizeof(gs_citynames_buf_0077d610),
                       DAT_0077d610 + sizeof(gs_citynames_buf_0077d610) + sizeof(gs_browse_buf_0077dc50), (char **)0);
  s.ok &= FUN_00565c7e(filename, "SHOWDECK", gs_showdeck_0074b920, 3, DAT_0074d890,
                       DAT_0074d890 + sizeof(gs_showdeck_buf_0074d890), (char **)0);
  s.ok &= FUN_00565c7e(filename, "LOADSAVE", gs_loadsave_0077d1b0, 3, DAT_0077d040, DAT_0077d040 + sizeof(gs_loadsave_buf_0077d040),
                       (char **)0);
  s.ok &= FUN_00565c7e(filename, "STATS", gs_stats_0077cfa0, 9, DAT_0074ce20, DAT_0074ce20 + sizeof(gs_stats_buf_0074ce20),
                       (char **)0);
  s.ok &= FUN_00565c7e(filename, "ANALYZE", gs_analyze_0074b870, 0x11, DAT_0074afb0, DAT_0074afb0 + sizeof(gs_analyze_buf_0074afb0),
                       (char **)0);
  s.ok &= FUN_00565c7e(filename, "RIDDLE", gs_riddle_0077cf20, 0x11, DAT_0077ca20, DAT_0077ca20 + sizeof(gs_riddle_buf_0077ca20),
                       (char **)0);
  s.ok &= FUN_00565c7e(filename, "LAIR", gs_lair_0077e180, 0x13, DAT_007800c0, DAT_007800c0 + sizeof(gs_lair_buf_007800c0),
                       (char **)0);

  s.local_8 = DAT_00780c60;
  s.ok &= FUN_00565c7e(filename, "WISEMAN", gs_wiseman_0074d840, 0x13, DAT_00780c60, DAT_00780c60 + sizeof(gs_wiseman_buf_00780c60),
                       &s.local_8);
  s.ok &= FUN_00565c7e(filename, "CITYWISEMAN", gs_citywiseman_0074d800, 0xd, s.local_8, DAT_00780c60 + sizeof(gs_wiseman_buf_00780c60),
                       (char **)0);
  s.ok &= FUN_00565c7e(filename, "CITYSCREEN_BUTTONS", gs_cityscreen_buttons_0077f5e0, 8, DAT_0077f450,
                       DAT_0077f450 + sizeof(gs_cityscreen_buttons_buf_0077f450), (char **)0);

  FUN_0056cc4d(filename, "SHOWLIST");
  strcpy(DAT_0077e110, text_lines[0]);
  strcpy(DAT_0077e142, text_lines[1]);

  FUN_0056cc4d(filename, "SHOWLIBRARY");
  strcpy(DAT_0074bcc0, text_lines[0]);
  strcpy(DAT_0074bcf2, text_lines[1]);

  return s.ok;
}

// FUNCTION: SHANDALAR 0x0056cc4d
int FUN_0056cc4d(const char *filename, const char *section)
{
  int x;
  int i;
  int j;
  size_t len;
  int out_pos;

  if (DAT_008bd200 == 1)
  {
    x = 0;
  }
  else
  {
    x = load_text(filename, section);
    i = 0;
    while (i < abs(x))
    {
      len = strlen(text_lines[i]);
      out_pos = 0;
      j = 0;
      while (j <= (int)len)
      {
        if (text_lines[i][j] == '\\' && text_lines[i][j + 1] == 'n')
        {
          text_lines[i][out_pos] = '\n';
          j = j + 1;
        }
        else
        {
          text_lines[i][out_pos] = text_lines[i][j];
        }
        out_pos = out_pos + 1;
        j = j + 1;
      }
      text_lines[i][out_pos] = '\0';
      i = i + 1;
    }
  }

  return x;
}

// FUNCTION: SHANDALAR 0x00565c7e
int FUN_00565c7e(const char *filename, const char *section, char **out_table, int max_entries, char *string_buf,
                 char *string_buf_end, char **out_next_buf)
{
  int overflow;
  int count;
  size_t line_len;
  int i;
  char *cursor;

  overflow = 0;
  count = FUN_0056cc4d(filename, section);
  if (max_entries <= count)
  {
    count = max_entries;
  }

  cursor = string_buf;
  i = 0;
  while (i < count && !overflow)
  {
    line_len = strlen(text_lines[i]);
    if (cursor + line_len < string_buf_end)
    {
      strcpy(cursor, text_lines[i]);
      out_table[i] = cursor;
      cursor = cursor + line_len + 1;
    }
    else
    {
      overflow = 1;
    }
    ++i;
  }

  while (i = count, i < max_entries)
  {
    out_table[i] = (char *)"";
    count = i + 1;
  }

  if (out_next_buf != (char **)0)
  {
    *out_next_buf = cursor;
  }

  return !overflow;
}

// FUNCTION: SHANDALAR 0x00565fdb
int FUN_00565fdb(char *param_1, char *param_2, int *param_3, int *param_4)
{
  if (param_1 == (char *)0 || param_2 == (char *)0 || param_2 <= param_1 || param_3 == (int *)0 || param_4 == (int *)0)
  {
    return 0;
  }

  while (*param_1 != '\0' && param_1 < param_2 && strncmp(param_1, "STARTBLOCK", 10))
  {
    ++param_1;
  }

  if (strncmp(param_1, "STARTBLOCK", 10) == 0)
  {
    param_1 += 0xc;
    if (param_3 != (int *)0)
    {
      *param_3 = (int)param_1;
    }

    while (*param_1 != '\0' && param_1 < param_2 && strncmp(param_1, "ENDBLOCK", 8))
    {
      ++param_1;
    }

    if (strncmp(param_1, "ENDBLOCK", 8) == 0)
    {
      *param_1 = '\0';
      if (param_4 != (int *)0)
      {
        *param_4 = (int)(param_1 + 8);
      }
    }
    else if (param_4 != (int *)0)
    {
      *param_4 = 0;
    }
    return 1;
  }
  else
  {
    return 0;
  }
}

// FUNCTION: SHANDALAR 0x00565dbc
int FUN_00565dbc(const char *filename)
{
  HANDLE file;
  int ok;
  DWORD bytes_read;
  DWORD file_size;
  int block_start;
  int i;
  char *buffer_end;
  char *cr;
  char *cursor;
  char *next;

  ok = 1;
  file = CreateFileA(filename, 0x80000000, 1, (LPSECURITY_ATTRIBUTES)0, 3, 0x8000080, (HANDLE)0);
  if (file == (HANDLE)-1)
  {
    ok = 0;
  }
  else
  {
    file_size = GetFileSize(file, (LPDWORD)0);
    DAT_0067a3b8 = (char *)malloc(file_size + 1);
    if (DAT_0067a3b8 == (char *)0)
    {
      ok = 0;
    }
    else
    {
      ReadFile(file, (void *)DAT_0067a3b8, file_size, &bytes_read, (LPOVERLAPPED)0);
      cursor = DAT_0067a3b8;
      buffer_end = DAT_0067a3b8 + bytes_read;

      for (i = 0; i < 4; ++i)
      {
        if (FUN_00565fdb(cursor, buffer_end, &block_start, (int *)&next) == 0)
        {
          ok = 0;
        }
        else
        {
          DAT_0074c930[i] = (char *)block_start;
          cursor = next;
          while (cr = strchr(DAT_0074c930[i], 0xd), cr != (char *)0)
          {
            strcpy(cr, cr + 1);
          }
        }
      }

      for (i = 0; i < 0xc; ++i)
      {
        if (FUN_00565fdb(cursor, buffer_end, &block_start, (int *)&next) == 0)
        {
          ok = 0;
        }
        else
        {
          DAT_0077c9e0[i] = (char *)block_start;
          cursor = next;
          while (cr = strchr(DAT_0077c9e0[i], 0xd), cr != (char *)0)
          {
            strcpy(cr, cr + 1);
          }
        }
      }
    }

    CloseHandle(file);
  }

  return ok;
}

// FUNCTION: SHANDALAR 0x00559999
void FUN_00559999(void)
{
}

// FUNCTION: SHANDALAR 0x00562d03
void FUN_00562d03(void)
{
  FUN_00562f92("x:sound\\kwalkl.wav", 0, 0);
  FUN_00562f92("x:sound\\kwalkr.wav", 1, 0);
  FUN_00562f92("x:sound\\bwalkl.wav", 2, 0);
  FUN_00562f92("x:sound\\bwalkr.wav", 3, 0);
  FUN_00562f92("x:sound\\gwalkl.wav", 4, 0);
  FUN_00562f92("x:sound\\gwalkr.wav", 5, 0);
  FUN_00562f92("x:sound\\rwalkl.wav", 6, 0);
  FUN_00562f92("x:sound\\rwalkr.wav", 7, 0);
  FUN_00562f92("x:sound\\wwalkl.wav", 8, 0);
  FUN_00562f92("x:sound\\wwalkr.wav", 9, 0);
  FUN_00562f92("x:sound\\kbird1.wav", 10, 0);
  FUN_00562f92("x:sound\\bbird1.wav", 0xb, 0);
  FUN_00562f92("x:sound\\gbird1.wav", 0xc, 0);
  FUN_00562f92("x:sound\\rbird1.wav", 0xd, 0);
  FUN_00562f92("x:sound\\wbird1.wav", 0xe, 0);
}

// FUNCTION: SHANDALAR 0x00565faa
void FUN_00565faa(void)
{
  if (DAT_0067a3b8 != 0)
  {
    free(DAT_0067a3b8);
    DAT_0067a3b8 = 0;
  }
}

// FUNCTION: SHANDALAR 0x00562e0d
unsigned int FUN_00562e0d(char *filename)
{
  UINT drive_type;
  FILE *file;
  int i;
  DWORD *buf;
  char drive_string[260];
  DWORD scratch[63];

  *(DWORD *)drive_string = (DWORD)DAT_005919ac;

  buf = scratch;
  for (i = 0x3f; i != 0; --i)
  {
    *buf++ = 0;
  }

  for (;;)
  {
    if ('z' < drive_string[0])
    {
      return (unsigned int)drive_string[0];
    }

    drive_type = GetDriveTypeA(drive_string);
    if (drive_type == 5)
    {
      strcat(drive_string, filename);
      file = fopen(drive_string, "rb");
      if (file != (FILE *)0)
      {
        fclose(file);
        return (unsigned int)drive_string[0];
      }

      drive_string[3] = 0;
    }

    ++drive_string[0];
  }
}

// FUNCTION: SHANDALAR 0x00562ed0
char FUN_00562ed0(void)
{
  unsigned int drive;
  char cwd[256];

  if (DAT_0066970c == 0)
  {
    drive = FUN_00562e0d("sound\\locmus1.wav");
    DAT_0073e9d8 = (char)drive;
    DAT_0066970c = 1;
  }

  if (DAT_00748408 == 0)
  {
    _getcwd(cwd, 0x100);
    return cwd[0];
  }

  return DAT_0073e9d8;
}

// FUNCTION: SHANDALAR 0x00562f92
unsigned int FUN_00562f92(char *filename, int param_2, int param_3)
{
  if (DAT_00669704 == 0)
  {
    _getcwd(DAT_0073e890, 0x100);
    DAT_00669704 = 1;
  }

  if (filename[0] == 'x')
  {
    filename[0] = DAT_0073e890[0];
    if (FUN_00417dc6(filename) == 0)
    {
      filename[0] = FUN_00562ed0();
    }
  }

  while (DAT_00748400 != 0)
  {
  }

  sound_load(filename, param_2, param_3);
  return 0;
}

// FUNCTION: SHANDALAR 0x00417dc6
int FUN_00417dc6(const char *filename)
{
  FILE *file;

  file = fopen(filename, "rt");
  if (file == (FILE *)0)
  {
    return 0;
  }

  fclose(file);
  return 1;
}

// FUNCTION: SHANDALAR 0x0046e6f0
DWORD WINAPI FUN_0046e6f0(LPVOID param_1)
{
  int *font_cfg_entry;
  int i;
  void *page;

  (void)param_1;

  DAT_0078cf08 = fopen("advButtons.txt", "rt");
  strcpy(DAT_0078df10, "");
  DAT_0078df38 = LoadIniEscapedStringTable(DAT_0078cf08, "done", DAT_0078df10, 0)[0];

  FUN_00578c80(LoadFontConfigIfPresent("misc.exe", (char *)0));
  FUN_00578c80(LoadFontConfigIfPresent("mgraphic.exe", "fonts.cv"));
  FUN_00578c80(LoadFontConfigIfPresent("nsound.cvl", (char *)0));

  if (global_screen_width == 0x280)
  {
    LoadSystemFont(1, 0xb, "tt0300m_.ttf", "MPZurich Cn BT", 400, 0);
    LoadSystemFont(2, 9, "tt0298m_.ttf", "MPZurich Cn BT", 400, 0);
    LoadSystemFont(4, ScaleUiCoordinate(10), "tt0530m_.ttf", "Benguiat BkCn BT", 100, 0);
    LoadSystemFont(5, ScaleUiCoordinate(0x14), "tt0127m_.ttf", "Benguiat Bk BT", 700, 1);
    LoadSystemFont(6, ScaleUiCoordinate(0x1c), "tt0530m_.ttf", "Benguiat BkCn BT", 100, 0);
    LoadSystemFont(7, ScaleUiCoordinate(0xb), "tt0530m_.ttf", "Benguiat BkCn BT", 100, 0);
  }
  else if (global_screen_width == 800)
  {
    LoadSystemFont(1, ScaleUiCoordinate(8), "tt0530m_.ttf", "Benguiat BkCn BT", 100, 0);
    LoadSystemFont(2, 9, "tt0298m_.ttf", "MPZurich Cn BT", 400, 0);
    LoadSystemFont(4, ScaleUiCoordinate(10), "tt0530m_.ttf", "Benguiat BkCn BT", 100, 0);
    LoadSystemFont(5, ScaleUiCoordinate(0x14), "tt0127m_.ttf", "Benguiat Bk BT", 700, 1);
    LoadSystemFont(6, ScaleUiCoordinate(0x1c), "tt0530m_.ttf", "Benguiat BkCn BT", 100, 0);
    LoadSystemFont(7, ScaleUiCoordinate(0xb), "tt0530m_.ttf", "Benguiat BkCn BT", 100, 0);
  }
  else if (global_screen_width == 0x400)
  {
    LoadSystemFont(1, ScaleUiCoordinate(9), "tt0530m_.ttf", "Benguiat BkCn BT", 100, 0);
    LoadSystemFont(2, 10, "tt0298m_.ttf", "MPZurich Cn BT", 400, 0);
    LoadSystemFont(4, ScaleUiCoordinate(0xb), "tt0530m_.ttf", "Benguiat BkCn BT", 100, 0);
    LoadSystemFont(5, ScaleUiCoordinate(0x14), "tt0127m_.ttf", "Benguiat Bk BT", 700, 1);
    LoadSystemFont(6, ScaleUiCoordinate(0x1c), "tt0530m_.ttf", "Benguiat BkCn BT", 100, 0);
    LoadSystemFont(7, ScaleUiCoordinate(0xb), "tt0530m_.ttf", "Benguiat BkCn BT", 100, 0);
  }

  InitializeGraphicsSystemDefaultMode();
  ReadPalette("todpal.tr", (char *)0);

  for (i = 0; i < 3; i = i + 1)
  {
    if ((i == 1) && (DAT_00986950.width < 0x401))
    {
      page = CreateGraphicsPage(1, 0x400, 800, 8);
    }
    else
    {
      page = CreateGraphicsPage(i, global_screen_width, global_screen_height + 1, 8);
    }
    SetGraphicsPage(i, page);
  }

  page = CreateGraphicsPage(5, ScaleUiCoordinate(0x40), ScaleUiCoordinate(0x148), 8);
  SetGraphicsPage(5, page);

  page = CreateGraphicsPage(3, ScaleUiCoordinate(0x280),
                            (ScaleUiCoordinate(0x1e0) - ScaleUiCoordinate(0x148)) + 3, 8);
  SetGraphicsPage(3, page);

  PTR_DAT_0058332c->max_x = ScaleUiCoordinate(0x280);
  PTR_DAT_00583304->max_x = PTR_DAT_0058332c->max_x;
  PTR_DAT_005832dc->max_x = PTR_DAT_00583304->max_x;
  PTR_DAT_005832b4->max_x = PTR_DAT_005832dc->max_x;
  PTR_DAT_00583304->max_x = ScaleUiCoordinate(0x1e0);
  PTR_DAT_005832dc->max_y = PTR_DAT_00583304->max_x;
  PTR_DAT_005832b4->max_y = PTR_DAT_005832dc->max_y;
  PTR_DAT_0058332c->max_y = ScaleUiCoordinate(0x1e0) - ScaleUiCoordinate(0x148);
  PTR_DAT_00583354->max_y = ScaleUiCoordinate(0x148);
  PTR_DAT_00583354->max_x = ScaleUiCoordinate(0x40);
  PTR_DAT_005832b4->font_slot = 1;

  PresentGraphicsPage(0);
  DAT_00586494 = FUN_00578c20();
  do
  {
    FUN_0055db50();
  } while (DAT_009300f0 == 0);

  FUN_0057a9f0(5);
  if (DAT_00586494 != 0)
  {
    FUN_00578c30();
  }

  PostMessageA(DAT_00748420, 0x10, 0, 0);
}

// FUNCTION: SHANDALAR 0x004cea4c
LONG ChangeDisplayResolution(DWORD width, DWORD height)
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

// FUNCTION: SHANDALAR 0x004ceacb
void RestoreDisplayResolution(void)
{
  ChangeDisplayResolution(0, 0);
}

// FUNCTION: SHANDALAR 0x004cea02
unsigned int FUN_004cea02(void)
{
  DAT_00748408 = (unsigned int)(FUN_00417dc6("sound\\locmus1.wav") == 0);
  _DAT_0074840c = 0;
  return 0;
}

// FUNCTION: SHANDALAR 0x004ce9c6
void FUN_004ce9c6(void)
{
  timeKillEvent(DAT_007483fc);
  timeEndPeriod(DAT_00748418);
}

// FUNCTION: SHANDALAR 0x004ce9e9
void FUN_004ce9e9(void)
{
  SetSystemPaletteUse(global_main_hdc, 1);
}

// FUNCTION: SHANDALAR 0x004ce8cd
void CALLBACK FUN_004ce8cd(UINT uID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2)
{
  (void)uID;
  (void)uMsg;
  (void)dwUser;
  (void)dw1;
  (void)dw2;

  if (DAT_005b7d94 == 0)
  {
    DAT_005b7d94 = 1;
    DAT_00748424 = GetCurrentThread();
    DuplicateHandle(GetCurrentProcess(), DAT_00748424, GetCurrentProcess(), &DAT_00748424, 0x1f03ff, FALSE, 0);
  }

  ++DAT_00589df0;

  if (DAT_00748400 != 0)
  {
  }
  else
  {
    DAT_00748400 = 1;
    update_snd();
    DAT_00748400 = 0;
  }
}

// FUNCTION: SHANDALAR 0x004ce61a
LRESULT CALLBACK FUN_004ce61a(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  switch (msg)
  {
  case 0x10101010:
    update_snd();
    return 0;
  case 2:
    sound_close();
    PostQuitMessage(0);
    return DefWindowProcA(hwnd, 2, wparam, lparam);
  case 0x100:
  case 0x104:
    QueueKeyInputFromMessage(wparam, lparam);
    if (wparam != 0x7a)
    {
      return DefWindowProcA(hwnd, msg, wparam, lparam);
    }

    if (DAT_005b7d90 == 0)
    {
      RegisterPaletteClass(DAT_00939160);
      DAT_005b7d90 = 1;
    }

    DAT_005b7d98 = FindWindowExA((HWND)0, (HWND)0, "ShowPaletteClass", "Current Palette");
    if (DAT_005b7d98 == (HWND)0)
    {
      DAT_005b7d98 = CreatePalettePopupWindow(DAT_00939160, (void *)0);
      if (DAT_005b7d98 == (HWND)0)
      {
        return 0;
      }
      ShowWindow(DAT_005b7d98, 5);
    }
    else
    {
      BringWindowToTop(DAT_005b7d98);
    }

    UpdateWindow(DAT_005b7d98);
    return DefWindowProcA(hwnd, msg, 0x7a, lparam);
  case 0x200:
    DAT_00986d9c = (int)((unsigned int)lparam & 0xffff);
    DAT_00986d98 = (int)(((unsigned int)lparam >> 0x10) & 0xffff);
    break;
  case 0x201:
    DAT_00986d94 = 1;
    DAT_00986d9c = (int)((unsigned int)lparam & 0xffff);
    DAT_00986d98 = (int)(((unsigned int)lparam >> 0x10) & 0xffff);
    break;
  case 0x202:
    DAT_00986da0 |= 2;
    DAT_00986d94 = 0;
    break;
  case 0x204:
    DAT_00986d94 = 2;
    DAT_00986d9c = (int)((unsigned int)lparam & 0xffff);
    DAT_00986d98 = (int)(((unsigned int)lparam >> 0x10) & 0xffff);
    break;
  case 0x205:
    DAT_00986da0 |= 1;
    DAT_00986d94 = 0;
    break;
  }

  return DefWindowProcA(hwnd, msg, wparam, lparam);
}

/*
 * Main entrypoint for shandalar.exe.
 * Decompilation/matching work will replace/refine this as we recurse into callees.
 */
// FUNCTION: SHANDALAR 0x004cdd70
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int nShowCmd)
{
  int custom_mode_selected;
  HWND existing_main;
  HWND magic_shell_hwnd;
  int horzres;
  char module_path[100];
  char game_dir[256];
  char cmd_copy[100];
  char *slash;
  WNDCLASSA wndclass;
  HWND main_hwnd;
  MSG msg;
  DWORD thread_id;
  DWORD sectors_per_cluster;
  DWORD bytes_per_sector;
  DWORD number_of_free_clusters;
  DWORD total_number_of_clusters;
  int disk_free_bytes;
  int tick;
  int show_shell_on_exit;
  int resolution_option_matched;
  HDC screen_hdc;
  int event_timer_ok;
  HPALETTE stock_palette;

  (void)prevInstance;

  custom_mode_selected = 0;
  strcpy(cmd_copy, cmdLine);
  DAT_00715fa0 = 0;

  existing_main = FindWindowA("ShandalarMainClass", (LPCSTR)0);
  if (existing_main != (HWND)0)
  {
    ShowWindow(existing_main, 9);
    SetForegroundWindow(existing_main);
    return 0;
  }

  magic_shell_hwnd = FindWindowA("Magic Shell", (LPCSTR)0);
  if (magic_shell_hwnd != (HWND)0 && _strnicmp(cmdLine, "/MTGshell", 9) != 0)
  {
    PostMessageA(magic_shell_hwnd, 0x400, 2, 0);
    return 0;
  }

  if (InitLicenseSecretsFromRegistry() != 0)
  {
    return 0;
  }

  GetWindowsDirectoryA(module_path, 0x100);
  module_path[3] = 0;
  GetDiskFreeSpaceA(module_path, &sectors_per_cluster, &bytes_per_sector, &number_of_free_clusters,
                    &total_number_of_clusters);

  disk_free_bytes = total_number_of_clusters * sectors_per_cluster * number_of_free_clusters;
  tick = disk_free_bytes + ((unsigned int)disk_free_bytes >> 0x1f & 0x1fU);
  tick = (tick >> 5) + ((unsigned int)tick >> 0x1f & 0x3ffU);

  if (((tick >> 10) + ((unsigned int)tick >> 0x1f & 0x3ffU)) >> 10 == 0)
  {
    MessageBoxA((HWND)0, "We have determined that you may not have enough free swap file space to play Magic: The Gathering reliably.\nThis is just a warning, and you will be able to continue from here, but you may want to free space on your windows drive before you play.", "Low Memory/Swap File Space Warning",
                0x30);
  }

  srand(GetTickCount());
  set_global_base_directory(global_base_directory);
  _chdir(global_base_directory);

  if (FUN_00564ee7("AdvStrings.txt") == 0)
  {
    MessageBoxA((HWND)0, "Couldn't load the strings from the AdvStrings.TXT file", (LPCSTR)0, 0x1010);
    return 0;
  }

  if (FUN_00565dbc("AdvBlocks.txt") == 0)
  {
    MessageBoxA((HWND)0, "Couldn't load the strings from the AdvBlocks.TXT file", (LPCSTR)0, 0x1010);
    return 0;
  }

  FUN_00559999();

  /* Switch to executable directory */
#ifndef _DEBUG
  strcpy(game_dir, (*__p___argv())[0]);
  slash = strrchr(game_dir, '\\');
  if (slash)
  {
    *slash = 0;
  }
  _chdir(game_dir);
#endif

  DAT_005a1608 = 0;
  DAT_00939160 = hInstance;

  memset(&wndclass, 0, sizeof(wndclass));
  wndclass.style = 0x23;
  wndclass.lpfnWndProc = FUN_004ce61a;
  wndclass.hInstance = hInstance;
  wndclass.hIcon = LoadIconA(hInstance, (LPCSTR)0x65);
  wndclass.hCursor = LoadCursorA((HINSTANCE)0, (LPCSTR)0x7f00);
  wndclass.hbrBackground = GetStockObject(4);
  wndclass.lpszClassName = "ShandalarMainClass";

  if (RegisterClassA(&wndclass) == 0)
  {
    MessageBoxA((HWND)0, "Couldn't register the classes", (LPCSTR)0, 0x1010);
    return 0;
  }

  atexit(RestoreDisplayResolution);

  screen_hdc = GetDC((HWND)0);
  horzres = GetDeviceCaps(screen_hdc, 8);
  if (0x400 < horzres)
  {
    ChangeDisplayResolution(0x400, 0x300);
  }

  /* Command line resolution override parsing (still being refined for matching) */
  resolution_option_matched = 0;
  show_shell_on_exit = 0;
  if (cmdLine && cmdLine[0] && cmdLine[1])
  {
    if (cmdLine[1] == '6')
    {
      global_screen_width = 0x280;
      global_screen_height = 0x1e0;
      PTR_s_advinter800_pic_00589de8 = "advinter.pic";
      CreateGraphicsPage(0, 0x280, 0x1e0, -1);
      show_shell_on_exit = 1;
      resolution_option_matched = 1;
    }
    else if (cmdLine[1] == '8')
    {
      global_screen_width = 800;
      global_screen_height = 600;
      PTR_s_advinter800_pic_00589de8 = "advinter800.pic";
      CreateGraphicsPage(0, 800, 600, -1);
      show_shell_on_exit = 1;
      resolution_option_matched = 1;
    }
    else if (cmdLine[1] == '1')
    {
      global_screen_width = 0x400;
      global_screen_height = 0x300;
      PTR_s_advinter800_pic_00589de8 = "advinter1024.pic";
      CreateGraphicsPage(0, 0x400, 0x300, -1);
      show_shell_on_exit = 1;
      resolution_option_matched = 1;
    }
  }

  if (!resolution_option_matched)
  {
    screen_hdc = GetDC((HWND)0);
    horzres = GetDeviceCaps(screen_hdc, 8);
    if (horzres == 0x280)
    {
      global_screen_width = 0x280;
      global_screen_height = 0x1e0;
      PTR_s_advinter800_pic_00589de8 = "advinter.pic";
    }
    else if (horzres == 800)
    {
      global_screen_width = 800;
      global_screen_height = 600;
      PTR_s_advinter800_pic_00589de8 = "advinter800.pic";
    }
    else
    {
      global_screen_width = 0x400;
      global_screen_height = 0x300;
      PTR_s_advinter800_pic_00589de8 = "advinter1024.pic";
    }
  }

  if (PTR_DAT_005832b4)
  {
    PTR_DAT_005832b4->max_x = global_screen_width - 1;
    PTR_DAT_005832b4->max_y = global_screen_height - 1;
  }

  main_hwnd = CreateWindowExA(8, "ShandalarMainClass", "Magic: Shandalar", 0x80000000, 0, 0,
                              global_screen_width, global_screen_height, (HWND)0, (HMENU)0, hInstance, (LPVOID)0);
  DAT_00748420 = main_hwnd;
  ShowWindow(main_hwnd, nShowCmd);
  global_main_hdc = GetDC(main_hwnd);

  if (show_shell_on_exit)
  {
    DAT_00986950.hTempDC = global_main_hdc;
    SelectPalette(DAT_00986950.hTempDC, DAT_00986950.hPalette, FALSE);
    RealizePalette(DAT_00986950.hTempDC);
    SetStretchBltMode(DAT_00986950.hTempDC, 3);
  }

  FUN_004cea02();
  init_sound_dll(main_hwnd, 0, 1);

  if (DAT_00591210 == 0)
  {
    FUN_00562d03();
  }

  timeBeginPeriod(DAT_00748418);
  DAT_007483fc = timeSetEvent(DAT_00589de4, DAT_00748418, (LPTIMECALLBACK)FUN_004ce8cd, 0, 1);
  if (DAT_007483fc == 0)
  {
    assert(0, "D:\\Newmagic\\multiplayer\\sid\\Test.c", 0x15a, "Timer failed to initialize!\n");
  }
  atexit(FUN_004ce9c6);
  assert((unsigned int)(DAT_007483fc != 0xffffffff), "D:\\Newmagic\\multiplayer\\sid\\Test.c", 0x15c,
         "Could not start timer\n");

  SetSystemPaletteUse(global_main_hdc, 2);
  atexit(FUN_004ce9e9);

  DAT_00748404 = GetCurrentThread();
  DuplicateHandle(GetCurrentProcess(), DAT_00748404, GetCurrentProcess(), &DAT_00748404, 0x1f03ff, FALSE, 0);

  if (DAT_00715fa0 == 0)
  {
    InitializeCriticalSection(&DAT_00926910);
    DAT_00715fa0 = 1;
  }

  _DAT_00748410 = CreateThread((LPSECURITY_ATTRIBUTES)0, 0x2000, FUN_0046e6f0, (LPVOID)0, 0, &thread_id);

  while (GetMessageA(&msg, (HWND)0, 0, 0))
  {
    TranslateMessage(&msg);
    DispatchMessageA(&msg);
  }

  if (DAT_00715fa0 != 0)
  {
    DeleteCriticalSection(&DAT_00926910);
    DAT_00715fa0 = 0;
  }

  FUN_00565faa();
  ChangeDisplayResolution(0, 0);

  screen_hdc = GetDC((HWND)0);
  stock_palette = GetStockObject(0xf);
  SelectPalette(screen_hdc, stock_palette, FALSE);
  SetSystemPaletteUse(screen_hdc, 1);
  RealizePalette(screen_hdc);
  ReleaseDC((HWND)0, screen_hdc);

  if (_strnicmp(cmd_copy, "/MTGshell", 8) == 0)
  {
    char exe_cmd[264];

    strcpy(exe_cmd, global_base_directory);
    strcat(exe_cmd, "\\Magic.exe");
    strcat(exe_cmd, " /start3,1");
    WinExec(exe_cmd, 5);
  }

  return (int)msg.wParam;
}
