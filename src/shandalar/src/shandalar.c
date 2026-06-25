#include <windows.h>
#include <mmsystem.h>
#include <direct.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>
#include <time.h>
#include <errno.h>

#include "defs.h"
#include "shandalar.h"
#include "magic/src/global_state.h"
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

void LoadPcxResource(int page_number, int x, int y, char *path, void *opaque);
extern DIBSurface *g_graphics_pages[10];

typedef struct
{
  int unk_00;
  int unk_04;
  int unk_08;
  int unk_0c;
  int unk_10;
  int unk_14;
  int unk_18;
  int unk_1c;
} ShandalarColorSlot;

typedef struct
{
  int first;
  int second;
} HintPair;


int FUN_004ff456(void *param_1, int param_2);
int FUN_004ff652(void *param_1, int param_2);
int FUN_004ff888(void *param_1, int param_2);
int FUN_004ffc5d(AdvMenuControl *param_1);
int FUN_004ffd56(AdvMenuControl *param_1);

// GLOBAL: SHANDALAR 0x007483f8
HDC global_main_hdc;
// GLOBAL: SHANDALAR 0x005863b8
int global_screen_width = 0x280;
// GLOBAL: SHANDALAR 0x005863bc
int global_screen_height = 0x1e0;
// GLOBAL: SHANDALAR 0x005862d8
int DAT_005862d8[9];
// GLOBAL: SHANDALAR 0x00586340
int DAT_00586340[9];
// GLOBAL: SHANDALAR 0x005863c8
ShandalarColorSlot DAT_005863c8[8];
// GLOBAL: SHANDALAR 0x00589de8
const char *PTR_s_advinter800_pic_00589de8;
// GLOBAL: SHANDALAR 0x00583290
FacemakerWindowBounds DAT_00583290 = {0, 0, 0, 0x27f, 0x1df, 1, 0x0f, 4, 0};
// GLOBAL: SHANDALAR 0x005832b4
FacemakerWindowBounds *PTR_DAT_005832b4 = &DAT_00583290;
// GLOBAL: SHANDALAR 0x005832b8
FacemakerWindowBounds DAT_005832b8 = {1, 0, 0, 0x320, 0x258, 1, 0x0f, 4, 0};
// GLOBAL: SHANDALAR 0x005832dc
FacemakerWindowBounds *PTR_DAT_005832dc = &DAT_005832b8;
// GLOBAL: SHANDALAR 0x005832e0
FacemakerWindowBounds DAT_005832e0 = {2, 0, 0, 0x27f, 0x1df, 1, 0x0f, 4, 0};
// GLOBAL: SHANDALAR 0x00583304
FacemakerWindowBounds *PTR_DAT_00583304 = &DAT_005832e0;
// GLOBAL: SHANDALAR 0x00583308
FacemakerWindowBounds DAT_00583308 = {3, 0, 0, 0x27f, 0x1df, 1, 0x0f, 4, 0};
// GLOBAL: SHANDALAR 0x0058332c
FacemakerWindowBounds *PTR_DAT_0058332c = &DAT_00583308;
// GLOBAL: SHANDALAR 0x00583330
FacemakerWindowBounds DAT_00583330 = {5, 0, 0, 0x27f, 0x1df, 1, 0x0f, 4, 0};
// GLOBAL: SHANDALAR 0x00583354
FacemakerWindowBounds *PTR_DAT_00583354 = &DAT_00583330;
// GLOBAL: SHANDALAR 0x00748420
HWND DAT_00748420;
// GLOBAL: SHANDALAR 0x00939160
HINSTANCE DAT_00939160;
// GLOBAL: SHANDALAR 0x005a1608
int DAT_005a1608 = 1;
// GLOBAL: SHANDALAR 0x005a6198
long DAT_005a6198[0x4e2];

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
// GLOBAL: SHANDALAR 0x0073c00c
int DAT_0073c00c;
// GLOBAL: SHANDALAR 0x0073e9d8
char DAT_0073e9d8;
// GLOBAL: SHANDALAR 0x0067a3b8
char *DAT_0067a3b8;
// GLOBAL: SHANDALAR 0x005919ac
int DAT_005919ac = 0x005c3a61;
// GLOBAL: SHANDALAR 0x0078cf08
FILE *DAT_0078cf08;
// GLOBAL: SHANDALAR 0x0078cf10
char DAT_0078cf10[0x1000];
// GLOBAL: SHANDALAR 0x0078df10
char DAT_0078df10[0x28];
// GLOBAL: SHANDALAR 0x0078df38
int DAT_0078df38;
// GLOBAL: SHANDALAR 0x00586494
int DAT_00586494;
// GLOBAL: SHANDALAR 0x009300f0
int DAT_009300f0;
// GLOBAL: SHANDALAR 0x007486d0
int DAT_007486d0;
// GLOBAL: SHANDALAR 0x007486e0
EncodedImage *DAT_007486e0[5];
// GLOBAL: SHANDALAR 0x00781728
EncodedImage *g_face_preview_sprite_selected;
// GLOBAL: SHANDALAR 0x0078172c
EncodedImage *g_face_preview_sprite_group[6];
// GLOBAL: SHANDALAR 0x0073e9dc
int DAT_0073e9dc;
// GLOBAL: SHANDALAR 0x00591214
int DAT_00591214 = 1;
// GLOBAL: SHANDALAR 0x0078990c
int DAT_0078990c[10];
#define DAT_00789910 (&DAT_0078990c[1])
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
// GLOBAL: SHANDALAR 0x0097db40
long DAT_0097db40[0x100];
// GLOBAL: SHANDALAR 0x0097df40
int DAT_0097df40[0x100];
// GLOBAL: SHANDALAR 0x0097e450
HintPair DAT_0097e450[0x100];
// GLOBAL: SHANDALAR 0x007898f4
int DAT_007898f4;
// GLOBAL: SHANDALAR 0x007898f8
int DAT_007898f8;
// GLOBAL: SHANDALAR 0x007898f0
int DAT_007898f0;
// GLOBAL: SHANDALAR 0x007490f0
int DAT_007490f0[100];
// GLOBAL: SHANDALAR 0x0058e050
HANDLE DAT_0058e050;
// GLOBAL: SHANDALAR 0x0058c038
char s_D_MAGIC0_SVE_0058c038[] = "D:MAGIC0.SVE";
// GLOBAL: SHANDALAR 0x0058c13c
int DAT_0058c13c = -1;
// GLOBAL: SHANDALAR 0x0058c5ec
int DAT_0058c5ec[0x20];
// GLOBAL: SHANDALAR 0x0058c5fc
int DAT_0058c5fc[0x100];
// GLOBAL: SHANDALAR 0x0058c620
int DAT_0058c620[0x20];
// GLOBAL: SHANDALAR 0x00746e00
int DAT_00746e00[3];
// GLOBAL: SHANDALAR 0x00747ee0
int DAT_00747ee0;
// GLOBAL: SHANDALAR 0x00747ef0
char DAT_00747ef0[0x2c0];
// GLOBAL: SHANDALAR 0x00746ec0
FacemakerWindowBounds DAT_00746ec0;
// GLOBAL: SHANDALAR 0x00746ef0
int DAT_00746ef0[50];
// GLOBAL: SHANDALAR 0x00746f40
AdvMenuControl *DAT_00746f40[50][50];
// GLOBAL: SHANDALAR 0x00748d10
int DAT_00748d10[4];
// GLOBAL: SHANDALAR 0x00748d20
int DAT_00748d20[4];
// GLOBAL: SHANDALAR 0x007491c0
int DAT_007491c0[12];
// GLOBAL: SHANDALAR 0x007491f0
int DAT_007491f0[12];
// GLOBAL: SHANDALAR 0x00749220
int DAT_00749220[12];
// GLOBAL: SHANDALAR 0x00749250
int DAT_00749250[0x20];
// GLOBAL: SHANDALAR 0x0058b584
int DAT_0058b584;
// GLOBAL: SHANDALAR 0x0058b588
AdvMenuControl DAT_0058b588[4] = {
    {3, 0x47, 0x39, 0x36, 3, 0x47, 0x39, 0x36, 1, FUN_004ff456, FUN_004ffd56, 1, 0, (char *)0, (char *)0, 0, 0, {0, 0, 0, 0}},
    {3, 0x7e, 0x39, 0x36, 3, 0x7e, 0x39, 0x36, 1, FUN_004ff456, FUN_004ffd56, 2, 0, (char *)0, (char *)0, 0, 0, {0, 0, 0, 0}},
    {3, 0xb5, 0x39, 0x36, 3, 0xb5, 0x39, 0x36, 1, FUN_004ff456, FUN_004ffd56, 4, 0, (char *)0, (char *)0, 0, 0, {0, 0, 0, 0}},
    {3, 0xec, 0x39, 0x36, 3, 0xec, 0x39, 0x36, 1, FUN_004ff652, FUN_004ffd56, 5, 0, (char *)0, (char *)0, 0, 0, {0, 0, 0, 0}},
};
// GLOBAL: SHANDALAR 0x0058b6d8
AdvMenuControl DAT_0058b6d8[5] = {
    {0x28, 0x17c, 0x42, 0x28, 0x28, 0x17c, 0x42, 0x28, 1, FUN_004ff888, FUN_004ffc5d, 0x31, 0, (char *)0, (char *)0, 0, 0, {0, 0, 0, 0}},
    {0x73, 0x17c, 0x48, 0x28, 0x73, 0x17c, 0x48, 0x28, 1, FUN_004ff888, FUN_004ffc5d, 0x32, 0, (char *)0, (char *)0, 0, 0, {0, 0, 0, 0}},
    {0xc5, 0x17c, 0x48, 0x28, 0xc5, 0x17c, 0x48, 0x28, 1, FUN_004ff888, FUN_004ffc5d, 0x33, 0, (char *)0, (char *)0, 0, 0, {0, 0, 0, 0}},
    {0x114, 0x17c, 0x48, 0x28, 0x114, 0x17c, 0x48, 0x28, 1, FUN_004ff888, FUN_004ffc5d, 0x34, 0, (char *)0, (char *)0, 0, 0, {0, 0, 0, 0}},
    {0x166, 0x17c, 0x48, 0x28, 0x166, 0x17c, 0x48, 0x28, 1, FUN_004ff888, FUN_004ffc5d, 0x35, 0, (char *)0, (char *)0, 0, 0, {0, 0, 0, 0}},
};
// GLOBAL: SHANDALAR 0x0058b580
FacemakerWindowBounds *DAT_0058b580 = &DAT_00583290;
// GLOBAL: SHANDALAR 0x0058b87c
char *DAT_0058b87c[0x40];
// GLOBAL: SHANDALAR 0x0058b880
AdvMenuRect DAT_0058b880[0x20];
// GLOBAL: SHANDALAR 0x0058b954
int DAT_0058b954 = -1;
// GLOBAL: SHANDALAR 0x0058b958
int DAT_0058b958 = -1;
// GLOBAL: SHANDALAR 0x005b80d8
int DAT_005b80d8;
// GLOBAL: SHANDALAR 0x005b80e0
int DAT_005b80e0[50];
// GLOBAL: SHANDALAR 0x008c84f0
BITMAPINFO DAT_008c84f0;
// GLOBAL: SHANDALAR 0x008c7408
int DAT_008c7408[7];
// GLOBAL: SHANDALAR 0x0073e990
jmp_buf DAT_0073e990;
// GLOBAL: SHANDALAR 0x0073e9e0
jmp_buf DAT_0073e9e0;
// GLOBAL: SHANDALAR 0x006527b0
int DAT_006527b0;
// GLOBAL: SHANDALAR 0x00602fb4
int DAT_00602fb4;
// GLOBAL: SHANDALAR 0x00590764
int DAT_00590764;
// GLOBAL: SHANDALAR 0x00590768
int DAT_00590768;
// GLOBAL: SHANDALAR 0x00650f28
int DAT_00650f28;
// GLOBAL: SHANDALAR 0x0073eaa0
int DAT_0073eaa0;
// GLOBAL: SHANDALAR 0x0073e880
int DAT_0073e880;
// GLOBAL: SHANDALAR 0x005a6074
int DAT_005a6074;
// GLOBAL: SHANDALAR 0x005a6078
int DAT_005a6078;
// GLOBAL: SHANDALAR 0x00580d80
int DAT_00580d80;
// GLOBAL: SHANDALAR 0x00580d84
int DAT_00580d84;
// GLOBAL: SHANDALAR 0x00580d88
int DAT_00580d88;
// GLOBAL: SHANDALAR 0x00580d8c
int DAT_00580d8c;
// GLOBAL: SHANDALAR 0x00580d90
int DAT_00580d90;
// GLOBAL: SHANDALAR 0x00580d94
int DAT_00580d94;
// GLOBAL: SHANDALAR 0x00580d98
int DAT_00580d98 = 8;
// GLOBAL: SHANDALAR 0x00580d9c
int DAT_00580d9c = 0x71;
// GLOBAL: SHANDALAR 0x00580da0
int DAT_00580da0 = 0xe3;
// GLOBAL: SHANDALAR 0x005863ac
int DAT_005863ac = 5;
// GLOBAL: SHANDALAR 0x005a5fe8
int DAT_005a5fe8[0x20];
// GLOBAL: SHANDALAR 0x005a6068
int DAT_005a6068;
// GLOBAL: SHANDALAR 0x005a606c
int DAT_005a606c;
// GLOBAL: SHANDALAR 0x005a6070
int DAT_005a6070;
// GLOBAL: SHANDALAR 0x005a607c
int DAT_005a607c;
// GLOBAL: SHANDALAR 0x007483f0
int DAT_007483f0;
// GLOBAL: SHANDALAR 0x007483f4
int _DAT_007483f4;
// GLOBAL: SHANDALAR 0x0078cef0
char DAT_0078cef0[0xc];
// GLOBAL: SHANDALAR 0x00789934
int DAT_00789934;
// GLOBAL: SHANDALAR 0x0097ec50
signed char DAT_0097ec50[0x20];
// GLOBAL: SHANDALAR 0x0097ec70
int DAT_0097ec70;
// GLOBAL: SHANDALAR 0x0097ec74
int DAT_0097ec74;
// GLOBAL: SHANDALAR 0x0097ec78
int DAT_0097ec78;
// GLOBAL: SHANDALAR 0x0097ec7c
int DAT_0097ec7c;
// GLOBAL: SHANDALAR 0x0097ec80
int DAT_0097ec80;
// GLOBAL: SHANDALAR 0x0097ec84
int DAT_0097ec84;
// GLOBAL: SHANDALAR 0x0097ec88
int DAT_0097ec88;
// GLOBAL: SHANDALAR 0x0097ec8c
int DAT_0097ec8c;
// GLOBAL: SHANDALAR 0x0097ec90
int DAT_0097ec90;
// GLOBAL: SHANDALAR 0x0097ec94
int DAT_0097ec94;
// GLOBAL: SHANDALAR 0x0097ec98
int DAT_0097ec98;

// GLOBAL: SHANDALAR 0x005aa414
int DAT_005aa414;
// GLOBAL: SHANDALAR 0x005873d4
int DAT_005873d4;

// GLOBAL: SHANDALAR 0x0074cfe4
int DAT_0074cfe4;

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
int MeasureMultilineTextWidth(FacemakerWindowBounds *param_1, char *param_2);
int SetFontStyleSize(int font_id, unsigned int style);
void LoadPcxIntoPageNoPalette(char *path);
void ClearGraphicsPageWithPaletteColor(int page_number, int color_index);
void FUN_00579bf0(FacemakerWindowBounds *src, int src_x, int src_y, unsigned int width, int height,
                  FacemakerWindowBounds *dst, int dst_x, int dst_y);
int *FUN_00579ea0(int *out_rect, FacemakerWindowBounds *page, int x, int y, int width, int height);
void BeginSpriteEncodeSession(void);
EncodedImage *EncodeSpriteFromPage(int page_number, int x, int y, int width, int height);
void FinalizeSpriteEncodeSession(void);
int DrawTextFormatted(FacemakerWindowBounds *dst, int text_id, int draw_shadow, int scale_to_screen, int center_x, int center_y, int x,
                      int y, int *format_and_args);
void ReadGraphicsScanline(unsigned int *param_1, int param_2, int param_3, int param_4, unsigned int param_5);
void FUN_005626b0(char *param_1, int param_2, int param_3, int param_4, int param_5);
void PutGraphicsPixel(FacemakerWindowBounds *window_bounds, int x, int y, int color_index);
unsigned int ReadGraphicsPixel(int page_number, int x, int y);
void BlitGraphicsRect(FacemakerWindowBounds *dst, unsigned int dst_x, int dst_y, unsigned int width, DWORD height,
                      FacemakerWindowBounds *src, int src_x, int src_y);
void DrawEncodedImageResampled(FacemakerWindowBounds *dst, int x, int y, int width, int height, EncodedImage *encoded_image);
void StretchBlitGraphicsRect(FacemakerWindowBounds *dst, int dst_x, int dst_y, int src_w, int src_h,
                             FacemakerWindowBounds *src, int src_x, int src_y, int copy_w, int copy_h);
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
void FreeSpriteBlob(void *memory);
void AnimatePaletteToColor(int color_index, int palette_id);
int RunOpeningMenu(void);
int RunDifficultyMenu(void);
int RunColorMenu(void);
int FUN_00522508(int param_1);
int RunFacemakerFlow(void);
void FUN_004bdd0a(void);
void FUN_004f6d90(void);
void FUN_004f8101(void);
int FUN_004f7fb9(int param_1, int param_2);
void FUN_004f7c7d(void);
int FUN_004f717a(void);
void FUN_004f78d3(void);
int FUN_004f7a3c(int param_1, int param_2, int param_3, int param_4);
void FUN_004f7eb2(int param_1, int param_2, unsigned int param_3);
void FUN_005081fa(void);
unsigned int FUN_0043146b(int param_1, int param_2);
unsigned int FUN_005611c8(unsigned int param_1);
unsigned int FUN_004314ca(int param_1, int param_2);
void FUN_00431526(unsigned int param_1, int param_2, int param_3);
void FUN_0043174d(int param_1, int param_2, int param_3);
void FUN_005019ad(int param_1);
int FUN_005031a8(void);
int FUN_00501760(int param_1);
int FUN_0040dffd(unsigned char param_1);
void FUN_004290e2(int param_1, int param_2);
int FUN_005226e0(void);
void FUN_004bbc50(void);
int FUN_004ffcb4(AdvMenuControl *param_1);
void FUN_004ffda4(void);
void FUN_005631ca(void);
void FUN_0056bff1(void);
void FUN_0054cdbd(void);
int ConsumeUiTickCount(void);
void FUN_0054ac08(int param_1, int param_2, int param_3);
int FUN_0055e1b2(void);
int FUN_0055e651(void);
void FUN_0055e808(void);
void FUN_0055fd27(void);
void FUN_00561647(void);
void FUN_0046eca4(void);
int FUN_00500321(void);
int FUN_005000fb(int param_1);
int FUN_0050014e(AdvMenuControl *param_1, int param_2, int param_3);
int FUN_0050035e(void);
int FUN_0057ce70(int param_1, int param_2);
void FUN_0057b530(FacemakerWindowBounds *param_1, int param_2, int param_3, int param_4, char *format, ...);
void FUN_0057b560(FacemakerWindowBounds *param_1, int param_2, int param_3, int param_4, char *format, ...);
unsigned char FUN_0057ae30(int param_1);
void FUN_0057b4d0(FacemakerWindowBounds *param_1, int param_2, int param_3, int param_4, char *format, ...);
void FUN_0057c7e0(int param_1, char *param_2);
unsigned int FUN_005597ca(void);
int FUN_005003b7(int param_1, int param_2, int param_3);
unsigned int FUN_0041d21d(void);
unsigned int FUN_004ed005(void);
int FUN_004ecf30(int param_1, int param_2);
void FUN_004ecfa2(void);
void FUN_004ecfe3(void);
void FUN_00522786(void);
int HasQueuedKeyInput(void);
int PopQueuedKeyInput(void);
int FUN_00414127(void);
void destroy_create_fonts_resources(void);
void FUN_005797e0(FacemakerWindowBounds *param_1, int param_2, int param_3, int param_4, int param_5, unsigned int param_6);
unsigned int __cdecl save_or_load_ver1(void);
void BuildFacemakerPortraitSprites(FacemakerWindowBounds *page);

void FUN_004ce992(int param_1);
int FUN_0056c705(int param_1);
unsigned int FUN_004bdccc(unsigned int param_1);
int FUN_004be0bf(unsigned int param_1, int param_2, int param_3, int param_4, int param_5, int param_6);
int FUN_0056bcf7(unsigned int param_1, unsigned int param_2);
int FUN_0056c0e5(int param_1, int param_2, int param_3);
int FUN_0056bd9d(unsigned int param_1);
int FUN_0056c5ea(int param_1);
int FUN_004bb1cf(unsigned int param_1);
unsigned int FUN_005795f0(FacemakerWindowBounds *param_1, int param_2, int param_3);
int RunLoadSaveMenu(int param_1);
int FUN_0052280c(void *param_1);
int FUN_0056302b(int param_1);
int FUN_00578c60(void);
int FUN_00412bff(char *param_1, int param_2, int param_3);
int FUN_00412c37(char *param_1, int param_2);
void FUN_00412bae(char *param_1, int param_2, unsigned int param_3);
int FUN_0041318b(char *param_1, int param_2);
void FUN_004136ab(int param_1, int param_2, int param_3, int param_4);
void FUN_00413796(int param_1, int param_2, int param_3, int param_4, int param_5);
void FUN_004138bd(int param_1, int param_2, int param_3, int param_4, int param_5);
void FUN_00430e00(int param_1, int param_2, int param_3, int param_4, int param_5);
void FUN_00430e2d(char *param_1, int param_2, int param_3, int param_4);
void FUN_00430ef4(char *param_1, int param_2, int param_3, int param_4);
void FUN_0043104f(char *param_1, int param_2, int param_3, int param_4);
int FUN_004310e8(char *param_1);
void FUN_00431351(FacemakerWindowBounds *param_1, int param_2, int param_3, EncodedImage *param_4, int param_5, int param_6);
int FUN_004bb458(int param_1);
void FUN_0041d24b(int param_1);
int FUN_0055e31f(int param_1, int param_2);
int FUN_0050027e(int param_1, int param_2);
int FUN_004ce97d(void);
int FUN_004ece9a(void);
int FUN_004ecec6(int param_1, int param_2, int param_3);
int FUN_004ece40(int param_1);
unsigned int FUN_005018e8(char *param_1, int param_2);
int FUN_00501aec(char *param_1);
int FUN_00501b7d(void);
int FUN_00501e44(char *param_1);
int FUN_005020fe(char *param_1);
int FUN_0057aa30(int param_1, char param_2);
int FUN_0057adf0(int param_1);
void FUN_005796c0(FacemakerWindowBounds *param_1, int param_2, int param_3, int param_4, int param_5, int param_6);
int DrawTextLine(FacemakerWindowBounds *param_1, int param_2, int param_3, char *param_4);
void DrawEncodedImageUnscaled(FacemakerWindowBounds *dst, int x, int y, EncodedImage *encoded_image);
extern char *global_base_txt;
extern HPALETTE global_cart_art_hpalette;
extern int Gold;
extern int g_graphics_bpp;
extern RpBitsPalettePacket g_palette_data_words;
extern FontSlot g_font_slots[0x10];
extern int g_key_input_queue_count;
extern unsigned int g_key_input_queue[50];
extern int g_menu_selection_value;
extern int g_color_menu_selection_to_deck_color[5];
extern card_data_t global_cards_data[];

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
  DAT_008cf6d0 = -1;
  unk_00789308 = -1;
  FUN_00559999();
  for (i = 0; i < 4; i = i + 1)
  {
    DAT_007a7d10[i] = 8;
  }

  FUN_004184d2();
  FUN_0041786e();
  FUN_00578c70(1, 1, DAT_007486d0);
  FUN_0046ed03();
  FUN_0046ed33();
  GdiGetBatchLimit();
  GdiSetBatchLimit(100);
  LoadPcxIntoPageNoPalette("advfac64.pic");
  ReadSpriteEntryPointers(DAT_007490f0, "dbox.spr");

  while (1)
  {
    game_setup_mode = RunOpeningMenu();
    AnimatePaletteToColor(0, DAT_00589dec);
    proceed_to_main_loop = 0;

    switch (game_setup_mode)
    {
    case 0:
      while (1)
      {
        g_shandalar_difficulty = RunDifficultyMenu();
        AnimatePaletteToColor(0, DAT_00589dec);
        if (g_shandalar_difficulty == -1)
        {
          break;
        }

        while (1)
        {
          g_selected_wizard_color = RunColorMenu();
          g_starting_color = g_selected_wizard_color;
          DAT_0091c99c = g_selected_wizard_color;
          DAT_0091c9a0 = FUN_00522508(3);
          AnimatePaletteToColor(0, DAT_00589dec);
          if (g_starting_color == -1)
          {
            break;
          }

          g_player_is_male = RunFacemakerFlow();
          AnimatePaletteToColor(0, DAT_00589dec);
          if (g_player_is_male == -1)
          {
            continue;
          }

          g_deck_color_bitmap = 1 << ((unsigned char)g_selected_wizard_color & 0x1f);
          LoadPcxIntoPageNoPalette("advfac64.pic");
          LoadPcxIntoPage(1, (char *)PTR_s_advinter800_pic_00589de8);
          BlitGraphicsRect((int *)PTR_DAT_005832dc, 0, 0, global_screen_width, global_screen_height, (int *)PTR_DAT_005832b4, 0, 0);
          DAT_00715f10 = 0;
          setup_shared_startup();
          initialized_world = 1;
          PTR_DAT_005832b4->font_slot = 5;
          FUN_0056cc4d("ADVstrings.txt", "STARTUP");
          DrawTextAt((int *)PTR_DAT_005832b4, 0xff, 0x140, 0xbc, text_lines[0]);
          DAT_0073e9dc = 1;
          FUN_004bdd0a();
          FUN_004f6d90();
          FUN_005081fa();
          Gold = (5 - g_shandalar_difficulty) * 0x32;
          DAT_00591214 = 0;
          do
          {
            do
            {
              random_value = FUN_00522508(0x40);
              g_world_player_x = random_value * 0x20 + 0x10;
              random_value = FUN_00522508(0x40);
              g_world_player_y = random_value * 0x20 + 0x10;
              tile_mask =
                  FUN_0043146b((g_world_player_x + ((int)g_world_player_x >> 0x1f & 0x1fU)) >> 5,
                               (g_world_player_y + ((int)g_world_player_y >> 0x1f & 0x1fU)) >> 5);
              tile_mask = FUN_005611c8(tile_mask);
            } while ((g_deck_color_bitmap & tile_mask) == 0);

            tile_mask =
                FUN_004314ca((g_world_player_x + ((int)g_world_player_x >> 0x1f & 0x1fU)) >> 5,
                             (g_world_player_y + ((int)g_world_player_y >> 0x1f & 0x1fU)) >> 5);
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
      DAT_00715f10 = 0;
      random_value = FUN_005031a8();
      FUN_00501760(random_value);
      proceed_to_main_loop = 1;
      break;

    case 2:
      DAT_00715f10 = 0;
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
      if (g_town_slots[i].location_type == 5)
      {
        tile_mask = FUN_0043146b(g_town_slots[i].world_x, g_town_slots[i].world_y);
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
      ConsumeUiTickCount();
      FUN_0054ac08(g_world_player_x, g_world_player_y, DAT_00669710);
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
      g_monster_timer = g_monster_timer + 1;
      FUN_004ecfa2();
      FUN_004ecfe3();
      _setjmp3(&DAT_0073e9e0, 0);
    }

    AnimatePaletteToColor(0, DAT_00589dec);
    FUN_00522786();
    return FUN_00469099();
  }
}

// FUNCTION: SHANDALAR 0x00412bff
int FUN_00412bff(char *param_1, int param_2, int param_3)
{
  DAT_005a6074 = param_2;
  DAT_005a6078 = param_3;
  DAT_00580d80 = -1;
  return FUN_00412c37(param_1, 1);
}

// FUNCTION: SHANDALAR 0x00412bae
void FUN_00412bae(char *param_1, int param_2, unsigned int param_3)
{
  FUN_00412bff(param_1, (global_screen_width * param_2) / 0x140,
               (int)((param_3 - (param_3 & 1)) * global_screen_height) / 200 + (param_3 & 1));
  return;
}

// FUNCTION: SHANDALAR 0x00412c37
int FUN_00412c37(char *param_1, int param_2)
{
  int done;
  int selected_menu_entry;
  int next_menu_entry;
  int current_menu_entry;
  int timer_seconds_left;
  int mouse_active;
  int previous_menu_entry;
  int line_height;
  int last_timer_seconds;
  int key_code;
  char local_18[4];
  char *seconds_text;

  if ((param_2 != 0) && (DAT_0097ec7c == 0))
  {
    FUN_004ecfa2();
  }

  previous_menu_entry = -1;
  DAT_00580d90 = 0;
  DAT_00580d84 = 0;
  DAT_005a606c = 0;
  selected_menu_entry = 0;
  if (DAT_00580d80 != -1)
  {
    selected_menu_entry = DAT_00580d80;
  }

  DAT_0097ec90 = -1;
  DAT_0097ec78 = 1;
  DAT_005a607c = -1;
  DAT_00580d98 = FUN_0057adf0(PTR_DAT_005832b4->font_slot);
  line_height = 0x16;
  DAT_005a6068 = 0xff;
  if (DAT_00789934 == 0)
  {
    line_height = -1;
  }
  (void)line_height;

  mouse_active = 0;
  done = 0;
  FUN_0046ed33();
  FUN_0041318b(param_1, DAT_00580d80);
  FUN_0046ed03();

  if (DAT_0097ec7c != 0)
  {
    DAT_0097ec7c = 0;
    return -1;
  }

  if (DAT_00580d94 != -1)
  {
    ConsumeUiTickCount();
    last_timer_seconds = -1;
  }

  while (!done)
  {
    DAT_0097ec78 = 0;
    DAT_007898f0 = 0;
    if (DAT_007483f0 == 0)
    {
      FUN_004ece9a();
    }

    current_menu_entry = DAT_00580d94;
    if (DAT_00580d94 != -1)
    {
      timer_seconds_left = current_menu_entry - FUN_004ce97d() / (DAT_005863ac * 0x3c);
      if (timer_seconds_left == 0)
      {
        selected_menu_entry = -1;
        done = 1;
      }
      if (last_timer_seconds != timer_seconds_left)
      {
        seconds_text = _itoa(timer_seconds_left, DAT_0078cef0, 10);
        strcpy(local_18, seconds_text);
        FUN_005797e0(PTR_DAT_005832b4, DAT_0097ec84 - 0xe, DAT_005a6078 + 4, 0xc, 7, 0xbc);
        FUN_0043104f(local_18, DAT_0097ec84 - 8, DAT_005a6078 + 5, 0xff);
        last_timer_seconds = timer_seconds_left;
      }
    }

    FUN_0046eca4();
    if ((DAT_007898f0 == 0) && (mouse_active == 0))
    {
      if (HasQueuedKeyInput() != 0)
      {
        key_code = FUN_00414127();
        current_menu_entry = selected_menu_entry;
        if (key_code < 0x1c)
        {
          if (key_code == 0x1b)
          {
            selected_menu_entry = -1;
            done = 1;
          }
          else if (key_code == 0xd)
          {
            if ((DAT_0097ec98 & (1 << ((unsigned char)selected_menu_entry & 0x1f))) == 0)
            {
              done = 1;
            }
          }
          else
          {
            next_menu_entry = current_menu_entry + 1;
            while (next_menu_entry <= 0x1f)
            {
              if ((DAT_0097ec50[next_menu_entry] != -1) &&
                  ((((int)(char)DAT_0097ec50[next_menu_entry] ^ key_code) & 0x1f) == 0))
              {
                selected_menu_entry = next_menu_entry;
                break;
              }
              next_menu_entry = next_menu_entry + 1;
            }
          }
        }
        else if (key_code == 0x20)
        {
          if ((DAT_0097ec98 & (1 << ((unsigned char)selected_menu_entry & 0x1f))) == 0)
          {
            done = 1;
          }
        }
        else if (key_code == 0x4800)
        {
          if (0 < selected_menu_entry)
          {
            selected_menu_entry = selected_menu_entry - 1;
          }
        }
        else if (key_code == 0x5000)
        {
          if (selected_menu_entry < DAT_0097ec80 - 1)
          {
            selected_menu_entry = selected_menu_entry + 1;
          }
        }
        else
        {
          next_menu_entry = current_menu_entry + 1;
          while (next_menu_entry <= 0x1f)
          {
            if ((DAT_0097ec50[next_menu_entry] != -1) &&
                ((((int)(char)DAT_0097ec50[next_menu_entry] ^ key_code) & 0x1f) == 0))
            {
              selected_menu_entry = next_menu_entry;
              break;
            }
            next_menu_entry = next_menu_entry + 1;
          }
        }
      }
    }
    else
    {
      DAT_00580d90 = 1;
      mouse_active = 1;
      if (DAT_007898f0 == 2)
      {
        DAT_00580d84 = 1;
      }

      selected_menu_entry = ((DAT_007898f8 - DAT_005a6078) - 4) / DAT_00580d98 - DAT_0097ec90;
      if ((DAT_007898f4 < DAT_005a6074) || (DAT_0097ec84 < DAT_007898f4))
      {
        selected_menu_entry = -1;
      }

      if ((DAT_0097ec98 & (1 << ((unsigned char)selected_menu_entry & 0x1f))) == 0)
      {
        if (DAT_007898f0 == 0)
        {
          done = 1;
        }
      }
      else
      {
        selected_menu_entry = previous_menu_entry;
      }
    }

    if ((selected_menu_entry < 0) || (DAT_0097ec80 <= selected_menu_entry) || (DAT_0097ec8c != 0) || (DAT_0097ec74 != 0))
    {
      selected_menu_entry = -1;
    }

    if (selected_menu_entry != previous_menu_entry)
    {
      FUN_0041318b(param_1, selected_menu_entry);
      previous_menu_entry = selected_menu_entry;
      DAT_005a607c = selected_menu_entry;
    }
  }

  DAT_005a606c = 1;
  if (selected_menu_entry == -1)
  {
    DAT_00580d84 = 0;
  }
  else
  {
    FUN_0046ed33();
    FUN_0041318b(param_1, selected_menu_entry);
    FUN_004ce992(0x14);
    FUN_0046ed03();
  }
  DAT_00580d94 = -1;
  DAT_00580d88 = -1;
  DAT_00580d80 = -1;
  DAT_00580d8c = 0;
  DAT_0097ec74 = 0;
  DAT_0097ec98 = 0;
  DAT_0097ec70 = 0;
  return selected_menu_entry;
}

// FUNCTION: SHANDALAR 0x0057adf0
int FUN_0057adf0(int param_1)
{
  FontSlot *font;

  font = &g_font_slots[param_1];
  if (font->font_loaded != 0)
  {
    return (unsigned int)font->point_size + font->tm_leading;
  }
  return (unsigned int)font->point_size + (unsigned int)font->unk_06;
}

// FUNCTION: SHANDALAR 0x0057aa30
int FUN_0057aa30(int param_1, char param_2)
{
  FontSlot *font;
  HDC hdc;
  ABC abc;
  unsigned int ch;

  font = &g_font_slots[param_1];
  ch = (unsigned int)(unsigned char)param_2;
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

// FUNCTION: SHANDALAR 0x004ecec6
int FUN_004ecec6(int param_1, int param_2, int param_3)
{
  if (param_1 < param_2)
  {
    param_1 = param_2;
  }
  if (param_3 < param_1)
  {
    param_1 = param_3;
  }
  return param_1;
}

// FUNCTION: SHANDALAR 0x005796c0
void FUN_005796c0(FacemakerWindowBounds *param_1, int param_2, int param_3, int param_4, int param_5, int param_6)
{
  DIBSurface *page;
  HPEN pen;
  HGDIOBJ old_pen;
  COLORREF color;
  unsigned char *palette_entry;

  page = g_graphics_pages[param_1->page_number];
  if (param_6 < 0)
  {
    color = (COLORREF)(-param_6);
  }
  else
  {
    palette_entry = &g_palette_data_words.entry_data[param_6 * 4];
    color = ((unsigned int)palette_entry[2] << 0x10) | ((unsigned int)palette_entry[1] << 8) | (unsigned int)palette_entry[0];
  }
  pen = CreatePen(0, 1, color);
  old_pen = SelectObject(page->hTempDC, pen);
  MoveToEx(page->hTempDC, param_2, param_3, (LPPOINT)0);
  LineTo(page->hTempDC, param_4, param_5);
  SelectObject(page->hTempDC, old_pen);
  DeleteObject(pen);
}

// FUNCTION: SHANDALAR 0x00430e00
void FUN_00430e00(int param_1, int param_2, int param_3, int param_4, int param_5)
{
  FUN_005796c0(PTR_DAT_005832b4, param_1, param_2, param_3, param_4, param_5);
}

// FUNCTION: SHANDALAR 0x004138bd
void FUN_004138bd(int param_1, int param_2, int param_3, int param_4, int param_5)
{
  int tile_columns;
  int tile_rows;
  int tile_origin_x;
  int tile_origin_y;
  int y;
  int x;
  EncodedImage **box_sprites;

  tile_columns = (param_3 + 0x17 + ((param_3 + 0x17 >> 0x1f) & 0xf)) >> 4;
  tile_rows = (param_4 + 0x17 + ((param_4 + 0x17 >> 0x1f) & 0xf)) >> 4;
  tile_origin_x = (param_1 - 4) - ((tile_columns * 0x10) - (param_3 + 8)) / 2;
  tile_origin_y = (param_2 - 4) - ((tile_rows * 0x10) - (param_4 + 8)) / 2;
  box_sprites = ((EncodedImage **)DAT_007490f0) + param_5 * 9;

  PTR_DAT_005832b4->page_number = 1;
  FUN_005797e0(PTR_DAT_005832b4, tile_origin_x, tile_origin_y, tile_columns << 4, tile_rows << 4, 0xe3);

  for (y = 0; y < tile_rows; y = y + 1)
  {
    for (x = 0; x < tile_columns; x = x + 1)
    {
      DrawEncodedImageUnscaled(PTR_DAT_005832b4, x * 0x10 + tile_origin_x, y * 0x10 + tile_origin_y, box_sprites[0]);
    }
  }

  for (x = 0; x < tile_columns; x = x + 1)
  {
    DrawEncodedImageUnscaled(PTR_DAT_005832b4, x * 0x10 + tile_origin_x, tile_origin_y - 10, box_sprites[5]);
    DrawEncodedImageUnscaled(PTR_DAT_005832b4, x * 0x10 + tile_origin_x, tile_rows * 0x10 + tile_origin_y - 6, box_sprites[7]);
  }

  for (y = 0; y < tile_rows; y = y + 1)
  {
    DrawEncodedImageUnscaled(PTR_DAT_005832b4, tile_origin_x - 10, y * 0x10 + tile_origin_y, box_sprites[8]);
    DrawEncodedImageUnscaled(PTR_DAT_005832b4, tile_columns * 0x10 + tile_origin_x - 6, y * 0x10 + tile_origin_y, box_sprites[6]);
  }

  DrawEncodedImageUnscaled(PTR_DAT_005832b4, tile_origin_x - 10, tile_origin_y - 10, box_sprites[1]);
  DrawEncodedImageUnscaled(PTR_DAT_005832b4, tile_columns * 0x10 + tile_origin_x - 6, tile_origin_y - 10, box_sprites[2]);
  DrawEncodedImageUnscaled(PTR_DAT_005832b4, tile_origin_x - 10, tile_rows * 0x10 + tile_origin_y - 6, box_sprites[3]);
  DrawEncodedImageUnscaled(PTR_DAT_005832b4, tile_columns * 0x10 + tile_origin_x - 6, tile_rows * 0x10 + tile_origin_y - 6, box_sprites[4]);

  PTR_DAT_005832b4->page_number = 0;
  BlitGraphicsRect(PTR_DAT_005832dc, tile_origin_x - 10, tile_origin_y - 10, tile_columns * 0x10 + 0x14, tile_rows * 0x10 + 0x14,
                   PTR_DAT_005832b4, tile_origin_x - 10, tile_origin_y - 10);
}

// FUNCTION: SHANDALAR 0x004136ab
void FUN_004136ab(int param_1, int param_2, int param_3, int param_4)
{
  if (DAT_0097ec94 == 0)
  {
    FUN_004138bd(param_1, param_2, param_3, param_4, (param_2 + ((param_2 >> 0x1f) & 7)) >> 3 & 3);
  }
  else
  {
    FUN_004138bd(param_1, param_2, param_3, param_4, DAT_0097ec94);
    DAT_0097ec94 = 0;
  }
}

// FUNCTION: SHANDALAR 0x00430e2d
void FUN_00430e2d(char *param_1, int param_2, int param_3, int param_4)
{
  int text_width;
  int font_height;
  int right_limit;
  int bottom_limit;

  if (param_2 < 0)
  {
    param_2 = 0;
  }
  if (param_3 < 0)
  {
    param_3 = 0;
  }

  text_width = FUN_004310e8(param_1);
  if (global_screen_width <= param_2 + text_width)
  {
    right_limit = global_screen_width - 1;
    text_width = FUN_004310e8(param_1);
    param_2 = right_limit - text_width;
  }
  font_height = FUN_0057adf0(*(int *)((char *)PTR_DAT_005832b4 + 0x20));
  if (global_screen_height <= param_3 + font_height)
  {
    bottom_limit = global_screen_height - 1;
    font_height = FUN_0057adf0(*(int *)((char *)PTR_DAT_005832b4 + 0x20));
    param_3 = bottom_limit - font_height;
  }

  PTR_DAT_005832b4->text_color = param_4;
  DrawTextLine(PTR_DAT_005832b4, param_2, param_3, param_1);
}

// FUNCTION: SHANDALAR 0x00413796
void FUN_00413796(int param_1, int param_2, int param_3, int param_4, int param_5)
{
  FUN_00430e00(param_1, param_2, param_3 + param_1, param_2, param_5);
  FUN_00430e00(param_1, param_4 + param_2, param_3 + param_1, param_4 + param_2, param_5);
  FUN_00430e00(param_3 + param_1, param_2, param_3 + param_1, param_4 + param_2, param_5);
  FUN_00430e00(param_1, param_2, param_1, param_4 + param_2, param_5);
}

// FUNCTION: SHANDALAR 0x00430ef4
void FUN_00430ef4(char *param_1, int param_2, int param_3, int param_4)
{
  PTR_DAT_005832b4->unk_14 = 0;
  FUN_00430e2d(param_1, param_2, param_3, param_4);
  PTR_DAT_005832b4->unk_14 = 1;
}

// FUNCTION: SHANDALAR 0x004310e8
int FUN_004310e8(char *param_1)
{
  int font_slot;
  int text_width;
  char *cursor;

  cursor = param_1;
  font_slot = PTR_DAT_005832b4->font_slot;
  text_width = 0;
  while (*cursor != '\0')
  {
    text_width = text_width + FUN_0057aa30(font_slot, *cursor);
    cursor = cursor + 1;
  }
  return text_width;
}

// FUNCTION: SHANDALAR 0x0043104f
void FUN_0043104f(char *param_1, int param_2, int param_3, int param_4)
{
  int text_width;

  text_width = FUN_004310e8(param_1);
  PTR_DAT_005832b4->unk_14 = 0;
  FUN_00430e2d(param_1, param_2 - text_width / 2, param_3, param_4);
  PTR_DAT_005832b4->unk_14 = 1;
}

// FUNCTION: SHANDALAR 0x004ce97d
int FUN_004ce97d(void)
{
  return DAT_00589df0;
}

// FUNCTION: SHANDALAR 0x004ce955
int ConsumeUiTickCount(void)
{
  int queued_ticks;

  queued_ticks = DAT_00589df0;
  DAT_00589df0 = 0;
  return queued_ticks;
}

// FUNCTION: SHANDALAR 0x004ece9a
int FUN_004ece9a(void)
{
  _DAT_007483f4 = GetTickCount() & 0x7fff;
  DAT_007483f0 = 1;
  return 0;
}

// FUNCTION: SHANDALAR 0x0041318b
int FUN_0041318b(char *param_1, int param_2)
{
  size_t text_len;
  int char_width;
  int text_color;
  int highlight_color;
  int line_idx;
  int menu_bottom;
  int menu_option_idx;
  int line_width;

  DAT_005a5fe8[0] = 0;
  if (DAT_0097ec78 == 1)
  {
    DAT_00580d98 = FUN_0057adf0(PTR_DAT_005832b4->font_slot);
    for (line_idx = 0; line_idx < 0x20; line_idx = line_idx + 1)
    {
      DAT_0097ec50[line_idx] = -1;
    }

    menu_option_idx = 0;
    DAT_005a6070 = 0;
    line_width = 0;
    DAT_0097ec88 = 0;
    for (line_idx = 0, text_len = strlen(param_1); line_idx < (int)text_len; line_idx = line_idx + 1)
    {
      if (param_1[line_idx] == '\n')
      {
        if (DAT_0097ec88 < line_width)
        {
          DAT_0097ec88 = line_width;
        }
        line_width = 0;
        DAT_005a6070 = DAT_005a6070 + 1;
        DAT_005a5fe8[DAT_005a6070] = line_idx + 1;
      }
      else
      {
        if ((line_width == 0) && ((param_1[line_idx] == ' ') || (param_1[line_idx] == '_')))
        {
          if (menu_option_idx < 0x20)
          {
            DAT_0097ec50[menu_option_idx] = param_1[line_idx + 1];
          }
          if (DAT_0097ec90 == -1)
          {
            DAT_0097ec90 = DAT_005a6070;
          }
          menu_option_idx = menu_option_idx + 1;
        }
        char_width = FUN_0057aa30(PTR_DAT_005832b4->font_slot, param_1[line_idx]);
        line_width = line_width + char_width;
      }
    }

    DAT_005a6070 = FUN_004ecec6(DAT_005a6070, 0, (global_screen_height - DAT_005a6078) / DAT_00580d98);
    if (DAT_005a6074 == -1)
    {
      DAT_005a6074 = 0xa0 - (DAT_0097ec88 + 8) / 2;
    }
    DAT_0097ec84 = DAT_0097ec88 + DAT_005a6074 + 8;
    menu_bottom = DAT_005a6070 * DAT_00580d98 + DAT_005a6078 + 6;
    if (DAT_00580d8c != 0)
    {
      menu_bottom = DAT_005a6070 * DAT_00580d98 + DAT_005a6078 + 8;
    }
    text_len = strlen(param_1);
    if (param_1[text_len - 1] != '\n')
    {
      menu_option_idx = menu_option_idx - 1;
    }
    DAT_0097ec80 = menu_option_idx;
    FUN_004136ab(DAT_005a6074, DAT_005a6078, DAT_0097ec84 - DAT_005a6074, menu_bottom - DAT_005a6078);
    if (DAT_0097ec8c != 0)
    {
      FUN_00430e2d("OK", DAT_0097ec84 - 0x11, menu_bottom - 8, 0xfe);
      FUN_00413796(DAT_0097ec84 - 0x14, menu_bottom - 10, 0x14, 10, 0xfe);
    }
  }

  if ((*param_1 == ' ') || (*param_1 == '_'))
  {
    menu_option_idx = 0;
  }
  else
  {
    menu_option_idx = -1;
  }

  DAT_005a6068 = DAT_00580d9c;
  PTR_DAT_005832b4->text_color = DAT_00580d9c;
  for (line_idx = 0; line_idx < DAT_005a6070; line_idx = line_idx + 1)
  {
    if ((DAT_0097ec78 != 0) || (menu_option_idx == DAT_005a607c) || (menu_option_idx == param_2))
    {
      param_1[DAT_005a5fe8[line_idx + 1] - 1] = '\0';
      if ((menu_option_idx < 0) || ((DAT_0097ec70 & (1 << ((unsigned char)menu_option_idx & 0x1f))) == 0))
      {
        if (menu_option_idx < 0)
        {
          text_color = DAT_005a6068;
        }
        else if (menu_option_idx == param_2)
        {
          text_color = DAT_00580da0;
        }
        else
        {
          text_color = DAT_00580d9c;
        }
        FUN_00430ef4(param_1 + DAT_005a5fe8[line_idx], DAT_005a6074 + 5, line_idx * DAT_00580d98 + DAT_005a6078 + 5, text_color);
      }
      else
      {
        param_1[DAT_005a5fe8[line_idx]] = '^';
        if ((DAT_005a606c == 0) || (menu_option_idx != param_2))
        {
          if (menu_option_idx < 0)
          {
            highlight_color = DAT_005a6068;
          }
          else if (menu_option_idx == param_2)
          {
            highlight_color = DAT_00580da0;
          }
          else
          {
            highlight_color = DAT_00580d9c;
          }
          FUN_00430ef4(param_1 + DAT_005a5fe8[line_idx], DAT_005a6074 + 5, line_idx * DAT_00580d98 + DAT_005a6078 + 5, highlight_color);
        }
        else
        {
          FUN_00430ef4(param_1 + DAT_005a5fe8[line_idx], DAT_005a6074 + 5, line_idx * DAT_00580d98 + DAT_005a6078 + 5, 0xff);
        }
        param_1[DAT_005a5fe8[line_idx]] = ' ';
      }
      param_1[DAT_005a5fe8[line_idx + 1] - 1] = '\n';
    }

    if ((param_1[DAT_005a5fe8[line_idx + 1]] == ' ') || (param_1[DAT_005a5fe8[line_idx + 1]] == '_'))
    {
      menu_option_idx = menu_option_idx + 1;
    }
  }
  return param_2;
}

// FUNCTION: SHANDALAR 0x0057a9f0
BOOL FUN_0057a9f0(int param_1)
{
  FontSlot *font;

  font = &g_font_slots[param_1];
  if (font->font_loaded == 0)
  {
    return FALSE;
  }

  DeleteObject(font->hfont);
  return RemoveFontResourceA(font->data.gdi.font_file);
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
void FUN_004184d2(void)
{
  struct
  {
    int local_124;
    char local_120[8];
    int local_118;
    int local_114;
    int local_110;
    char local_10c;
    char local_10b[255];
    FILE *local_c;
    int local_8;
  } locals;

  locals.local_c = fopen("hints.txt", "rt");
  locals.local_118 = 0;
  do
  {
    locals.local_8 = fscanf(locals.local_c, "%[^\n]", &locals.local_10c);
    if (locals.local_10c == '.')
    {
      sscanf(locals.local_10b, "%d %d %s", &locals.local_124, &locals.local_114, locals.local_120);
      DAT_0097e450[locals.local_118].first = locals.local_124;
      DAT_0097e450[locals.local_118].second = locals.local_114;
      DAT_0097df40[locals.local_118] = 0;

      if (strchr(locals.local_120, 'A') != (char *)0)
      {
        DAT_0097df40[locals.local_118] = DAT_0097df40[locals.local_118] | 1;
      }

      if (strchr(locals.local_120, 'B') != (char *)0)
      {
        DAT_0097df40[locals.local_118] = DAT_0097df40[locals.local_118] | 2;
      }

      if (strchr(locals.local_120, 'C') != (char *)0)
      {
        DAT_0097df40[locals.local_118] = DAT_0097df40[locals.local_118] | 4;
      }

      if (strchr(locals.local_120, 'D') != (char *)0)
      {
        DAT_0097df40[locals.local_118] = DAT_0097df40[locals.local_118] | 8;
      }

      locals.local_110 = FUN_0056c705(locals.local_124);
      locals.local_110 = FUN_0056c705(locals.local_114);
      locals.local_8 = fscanf(locals.local_c, "%[\n]", &locals.local_10c);
      DAT_0097db40[locals.local_118] = ftell(locals.local_c);
      locals.local_118 = locals.local_118 + 1;
    }
    else
    {
      locals.local_8 = fscanf(locals.local_c, "%[\n]", &locals.local_10c);
    }
  } while ((locals.local_118 < 0x100) && (locals.local_8 != -1));

  do
  {
    DAT_0097e450[locals.local_118].second = -1;
    DAT_0097e450[locals.local_118].first = DAT_0097e450[locals.local_118].second;
    locals.local_118 = locals.local_118 + 1;
  } while (locals.local_118 < 0x100);

  fclose(locals.local_c);
}
// FUNCTION: SHANDALAR 0x0041786e
void FUN_0041786e(void)
{
  struct
  {
    int local_1c;
    int local_18;
    long local_14;
    int local_10;
    int local_c;
    FILE *local_8;
    int local_4;
  } locals;

  for (locals.local_10 = 0; locals.local_10 < 0x4e2; locals.local_10 = locals.local_10 + 1)
  {
    DAT_005a6198[locals.local_10] = -1;
  }

  locals.local_8 = fopen("concise.csv", "rt");
  locals.local_c = 0;

  for (locals.local_10 = 0; locals.local_10 < DAT_0073c00c; locals.local_10 = locals.local_10 + 1)
  {
    locals.local_c = global_cards_data[locals.local_10].id;
    locals.local_4 = fscanf(locals.local_8, "%d %d %ld\n", &locals.local_18, &locals.local_1c, &locals.local_14);
    global_cards_data[locals.local_10].rarity = (unsigned char)locals.local_1c;
    DAT_005a6198[locals.local_c] = locals.local_14;
  }

  fclose(locals.local_8);
}
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

// FUNCTION: SHANDALAR 0x0057b530
void FUN_0057b530(FacemakerWindowBounds *param_1, int param_2, int param_3, int param_4, char *format, ...)
{
  DrawTextFormatted(param_1, param_2, 1, 0, 0, 1, param_3, param_4, (int *)&format);
}

// FUNCTION: SHANDALAR 0x0057b560
void FUN_0057b560(FacemakerWindowBounds *param_1, int param_2, int param_3, int param_4, char *format, ...)
{
  DrawTextFormatted(param_1, param_2, 1, 0, 1, 1, param_3, param_4, (int *)&format);
}

// FUNCTION: SHANDALAR 0x0057ae30
unsigned char FUN_0057ae30(int param_1)
{
  return g_font_slots[param_1].point_size;
}

// FUNCTION: SHANDALAR 0x0057b4d0
void FUN_0057b4d0(FacemakerWindowBounds *param_1, int param_2, int param_3, int param_4, char *format, ...)
{
  DrawTextFormatted(param_1, param_2, 1, 0, 0, 0, param_3, param_4, (int *)&format);
}

// FUNCTION: SHANDALAR 0x0057c7e0
void FUN_0057c7e0(int param_1, char *param_2)
{
  LoadPcxResource(param_1, 0, 0, param_2, (RpBitsPalettePacket *)1);
}

// FUNCTION: SHANDALAR 0x00579bf0
void FUN_00579bf0(FacemakerWindowBounds *src, int src_x, int src_y, unsigned int width, int height,
                  FacemakerWindowBounds *dst, int dst_x, int dst_y)
{
  BlitGraphicsRect(dst, (unsigned int)dst_x, dst_y, width, (DWORD)height, src, src_x, src_y);
}

// FUNCTION: SHANDALAR 0x00579ea0
int *FUN_00579ea0(int *out_rect, FacemakerWindowBounds *page, int x, int y, int width, int height)
{
  out_rect[0] = page->unk_04;
  out_rect[1] = page->unk_08;
  out_rect[2] = page->max_x;
  out_rect[3] = page->max_y;
  page->unk_04 = x;
  page->unk_08 = y;
  page->max_x = width;
  page->max_y = height;
  return out_rect;
}

// FUNCTION: SHANDALAR 0x00500321
int FUN_00500321(void)
{
  DAT_0058b584 = DAT_0058b584 + 1;
  DAT_0058b954 = -1;
  DAT_0058b958 = -1;
  FUN_005000fb(DAT_0058b584);
  return DAT_0058b584;
}

// FUNCTION: SHANDALAR 0x005000fb
int FUN_005000fb(int param_1)
{
  DAT_00746ef0[param_1] = 0;
  DAT_005b80e0[param_1] = 1;
  return 0;
}

// FUNCTION: SHANDALAR 0x0050014e
int FUN_0050014e(AdvMenuControl *param_1, int param_2, int param_3)
{
  int local_c;
  int local_8;

  DAT_00747ee0 = 1;
  local_c = DAT_00746ef0[param_3];
  for (local_8 = 0; local_8 < param_2; local_c++, local_8++)
  {
    DAT_00746f40[param_3][local_c] = &param_1[local_8];
  }
  DAT_00746ef0[param_3] = DAT_00746ef0[param_3] + param_2;
  DAT_00747ee0 = 0;
  return DAT_00746ef0[param_3];
}

// FUNCTION: SHANDALAR 0x0050027e
int FUN_0050027e(int param_1, int param_2)
{
  struct
  {
    int i;
    int limit;
  } locals;

  if (DAT_00746ef0[DAT_0058b584] < param_1 + param_2)
  {
    locals.limit = DAT_00746ef0[DAT_0058b584];
  }
  else
  {
    locals.limit = param_1 + param_2;
  }

  DAT_00747ee0 = 1;
  for (locals.i = param_1; locals.i < locals.limit; locals.i = locals.i + 1)
  {
    DAT_00746f40[DAT_0058b584][locals.i]->on_render(DAT_00746f40[DAT_0058b584][locals.i], 0);
  }
  DAT_00747ee0 = 0;
  return 1;
}

// FUNCTION: SHANDALAR 0x0050035e
int FUN_0050035e(void)
{
  FUN_005000fb(DAT_0058b584);
  if (DAT_0058b584 == 0)
  {
    DAT_0058b584 = 0;
  }
  else
  {
    DAT_0058b584 = DAT_0058b584 - 1;
  }
  DAT_0058b954 = -1;
  DAT_0058b958 = -1;
  return DAT_0058b584;
}

// FUNCTION: SHANDALAR 0x00522508
int FUN_00522508(int param_1)
{
  return (param_1 > 1) ? rand() % param_1 : 0;
}

// FUNCTION: SHANDALAR 0x004ce992
void FUN_004ce992(int param_1)
{
  (void)param_1;
}

// FUNCTION: SHANDALAR 0x005797e0
void FUN_005797e0(FacemakerWindowBounds *param_1, int param_2, int param_3, int param_4, int param_5, unsigned int param_6)
{
  COLORREF color;
  unsigned int value;
  HBRUSH brush;
  RECT fill_rect;
  DIBSurface *page;

  fill_rect.left = param_2;
  fill_rect.right = param_2 + param_4;
  fill_rect.top = param_3;
  fill_rect.bottom = param_3 + param_5;

  page = g_graphics_pages[param_1->page_number];

  if ((int)param_6 < 0)
  {
    value = (unsigned int)(-(int)param_6);
    color = RGB(0xff, 0xff, 0xff);
    if (param_6 != 0xff000001)
    {
      color = PALETTERGB((BYTE)(value >> 8), (BYTE)value, (BYTE)(value >> 16));
    }
  }
  else if (param_6 == 0xff)
  {
    color = RGB(0xff, 0xff, 0xff);
  }
  else
  {
    color = PALETTEINDEX((WORD)param_6);
  }

  brush = CreateSolidBrush(color);
  FillRect(page->hTempDC, &fill_rect, brush);
  DeleteObject(brush);
}

// FUNCTION: SHANDALAR 0x004bdd0a
void FUN_004bdd0a(void)
{
  struct
  {
    int local_1c;
    int local_18;
    unsigned int uVar3;
    unsigned int uVar2;
    int local_10;
    int local_8;
    int local_4;
  } locals;

  if (DAT_008bd200 == 0)
  {
    for (locals.local_10 = 0; locals.local_10 < 500; locals.local_10 = locals.local_10 + 1)
    {
      deck[locals.local_10] = -1;
    }

    if (*(int *)&DAT_0078df10[0x24] == 0)
    {
      for (locals.local_10 = 0; locals.local_10 < 7; locals.local_10 = locals.local_10 + 1)
      {
        DAT_008b3240[locals.local_10] = 0;
        DAT_008c7408[locals.local_10] = -1;
      }

      g_world_magic_bitmap = g_world_magic_bitmap | (1 << (g_selected_wizard_color * 2));
      DAT_005863c8[g_selected_wizard_color].unk_00 = 0;

      locals.uVar2 = 1 << (BYTE)g_selected_wizard_color;
      locals.local_8 = g_shandalar_difficulty + 1;
      locals.local_4 = locals.local_8;
      if (g_shandalar_difficulty == 3)
      {
        locals.local_8 = 1;
        locals.local_8 = 3;
      }

      locals.local_1c = g_shandalar_difficulty;
      switch (locals.local_1c)
      {
      case 0:
        if ((((locals.uVar2 < 1) ? 1 : 0) & 0x20) != 0)
        {
          FUN_004be0bf(locals.uVar2, 0xd, 7, 0xf, 1, 1);
        }
        else
        {
          FUN_004be0bf(locals.uVar2, 0xd, 0xc, 10, 1, 1);
        }
        break;
      case 1:
        FUN_004be0bf(locals.uVar2, 0xb, 4, 0xc, 1, 1);
        FUN_004be0bf(FUN_004bdccc(locals.uVar2), 4, 3, 4, 0, 1);
        break;
      case 2:
        FUN_004be0bf(locals.uVar2, 9, 3, 9, 1, 1);
        locals.uVar3 = FUN_004bdccc(locals.uVar2);
        FUN_004be0bf(locals.uVar3, 5, 3, 4, 0, 1);
        FUN_004be0bf(FUN_004bdccc(locals.uVar2 | locals.uVar3), 4, 3, 3, 0, 1);
        break;
      case 3:
        FUN_004be0bf(locals.uVar2, 6, 3, 5, 1, 1);
        FUN_004be0bf(1, 0xb, 5, 0xe, 0, 1);
        break;
      }

      g_journal_entry_count = 0;
      for (locals.local_10 = 0; locals.local_10 < 5; locals.local_10 = locals.local_10 + 1)
      {
        DAT_00789910[locals.local_10] = 0;
      }

      DAT_0078990c[g_selected_wizard_color] = DAT_0078990c[g_selected_wizard_color] + 1;
      for (locals.local_10 = 0; locals.local_10 < 3 - g_shandalar_difficulty; locals.local_10 = locals.local_10 + 1)
      {
        locals.local_18 = FUN_00522508(5);
        DAT_00789910[locals.local_18] = DAT_00789910[locals.local_18] + 1;
      }

      for (locals.local_10 = 0; locals.local_10 < 0x80; locals.local_10 = locals.local_10 + 1)
      {
        g_town_slots[locals.local_10].location_type = -1;
      }

      for (locals.local_10 = 0; locals.local_10 < 8; locals.local_10 = locals.local_10 + 1)
      {
        g_lair_or_monster_slots[locals.local_10].entry_type = -1;
      }

      for (locals.local_10 = 0; locals.local_10 < 1000; locals.local_10 = locals.local_10 + 1)
      {
        g_duel_victory_log[0] = 0;
      }

      for (locals.local_10 = 0; locals.local_10 < 4; locals.local_10 = locals.local_10 + 1)
      {
        DAT_007a7d10[locals.local_10] = 8;
      }

      for (locals.local_10 = 0; locals.local_10 < 0x96; locals.local_10 = locals.local_10 + 1)
      {
        deck[locals.local_10] = deck[locals.local_10] | 0x10000;
      }
      g_selected_wizard_color = -1;
    }
  }
}

// FUNCTION: SHANDALAR 0x004bdccc
unsigned int FUN_004bdccc(unsigned int param_1)
{
  int iVar1;
  unsigned char local_8;

  do
  {
    iVar1 = FUN_00522508(5);
    local_8 = (unsigned char)iVar1 + 1;
  } while ((param_1 & (1U << (local_8 & 0x1f))) != 0);

  return 1U << (local_8 & 0x1f);
}

// FUNCTION: SHANDALAR 0x004be0bf
int FUN_004be0bf(unsigned int param_1, int param_2, int param_3, int param_4, int param_5, int param_6)
{
  int bVar1;
  int iVar2;
  unsigned int local_20;
  unsigned int local_1c;
  unsigned int local_18;
  int local_14;
  int local_10;
  int local_c;

  for (local_10 = 0; local_10 < param_2; local_10 = local_10 + 1)
  {
    local_14 = FUN_0056bcf7(1, param_1);
    iVar2 = FUN_0056c0e5((int)(char)global_cards_data[local_14].color, param_1, 0);
    if ((iVar2 == 0) || (4 < local_14) || ((global_cards_data[local_14].expansion & 0xc1) == 0))
    {
      local_10 = local_10 - 1;
    }
    else
    {
      FUN_0056bd9d(local_14);
    }
  }

  for (local_10 = 0; local_10 < param_3; local_10 = local_10 + 1)
  {
    if (param_6 == 0)
    {
      local_18 = 0;
    }
    else
    {
      local_18 = rand();
      local_18 = local_18 & 1;
    }

    if (local_18 == 0)
    {
      local_1c = param_1;
    }
    else
    {
      local_1c = 1;
    }

    local_14 = FUN_0056bcf7((-(unsigned int)(local_18 == 0) & 0xffffffc4) + 0x40, local_1c);
    if (((g_shandalar_difficulty == 0) && ((global_cards_data[local_14].static_ability & 3) != 0)) ||
        ((global_cards_data[local_14].extra_ability & 0x900) != 0))
    {
      local_10 = local_10 - 1;
    }
    else
    {
      if ((global_cards_data[local_14].type & 4) != 0)
      {
        local_18 = 0;
      }

      if (local_18 == 0)
      {
        local_20 = param_1;
      }
      else
      {
        local_20 = 1;
      }

      iVar2 = FUN_0056c0e5((int)(char)global_cards_data[local_14].color, local_20, 0);
      if ((iVar2 == 0) || ((iVar2 = FUN_0056c5ea(local_14), (int)((~local_10 & 1) + 1) < iVar2)) ||
          ((global_cards_data[local_14].expansion & 0xc1) == 0))
      {
        local_10 = local_10 - 1;
      }
      else
      {
        FUN_0056bd9d(local_14);
      }
    }
  }

  local_c = 0;
  for (local_10 = 0; local_10 < param_4; local_10 = local_10 + 1)
  {
    local_14 = FUN_0056bcf7(2, param_1);
    if (((g_shandalar_difficulty < 4) && ((global_cards_data[local_14].static_ability & 3) != 0)) ||
        ((global_cards_data[local_14].extra_ability & 0x900) != 0))
    {
      local_10 = local_10 - 1;
      local_c = local_c - 1;
    }
    else
    {
      if (local_c < 1000)
      {
        iVar2 = FUN_004bb1cf(local_14);
        bVar1 = 0 < iVar2;
      }
      else
      {
        bVar1 = 1;
      }

      iVar2 = FUN_0056c0e5((int)(char)global_cards_data[local_14].color, param_1, 0);
      if ((iVar2 == 0) || ((iVar2 = FUN_0056c5ea(local_14), (int)((~local_10 & 1) + 1) < iVar2)) ||
          ((!bVar1 || ((global_cards_data[local_14].expansion & 0xc1) == 0))))
      {
        local_10 = local_10 - 1;
      }
      else
      {
        FUN_0056bd9d(local_14);
      }
    }
    local_c = local_c + 1;
  }

  if (param_5 != 0)
  {
    do
    {
      do
      {
        local_14 = FUN_0056bcf7(0xe, 1);
        iVar2 = FUN_0056c0e5((int)(char)global_cards_data[local_14].color, param_1, 1);
      } while (iVar2 == 0);
      iVar2 = FUN_0056c5ea(local_14);
    } while ((iVar2 < 3) || ((iVar2 = FUN_004bb1cf(local_14), iVar2 < 1)) ||
             ((g_shandalar_difficulty == 0 && ((global_cards_data[local_14].static_ability & 3) != 0))) ||
             ((global_cards_data[local_14].extra_ability & 0x900) != 0) ||
             ((global_cards_data[local_14].expansion & 0xc1) == 0));
  }

  FUN_0056bd9d(local_14);
  return 0;
}

// FUNCTION: SHANDALAR 0x0056c705
int FUN_0056c705(int param_1)
{
  int ret;
  int local_8;

  if (param_1 == -1)
  {
    return -1;
  }

  ret = -1;
  for (local_8 = 0; local_8 < DAT_0073c00c + 0x10; local_8 = local_8 + 1)
  {
    if (global_cards_data[local_8].id == param_1)
    {
      return local_8;
    }
  }
  return ret;
}

// FUNCTION: SHANDALAR 0x0056bcf7
int FUN_0056bcf7(unsigned int param_1, unsigned int param_2)
{
  int bVar1;
  int iVar2;
  int local_10;

  local_10 = 0;
  do
  {
    bVar1 = 0;
    iVar2 = FUN_00522508(DAT_0073c00c - 0x39);
    if (((param_1 == 0) || ((param_1 & global_cards_data[iVar2].type) != 0)) &&
        ((param_2 == 1) || ((param_2 & (int)(char)global_cards_data[iVar2].color) != 0)))
    {
      bVar1 = 1;
    }
    if (bVar1 == 0)
    {
      local_10 = local_10 + 1;
    }
  } while ((bVar1 == 0) && (local_10 < 999));

  return iVar2;
}

// FUNCTION: SHANDALAR 0x0056c0e5
int FUN_0056c0e5(int param_1, int param_2, int param_3)
{
  static const signed char DAT_00593e20[18] = {
      0, 0, 0,
      1, 4, 2,
      2, 1, 5,
      3, 5, 4,
      4, 3, 1,
      5, 2, 5};
  int iVar2;
  int iVar3;

  if ((param_1 == 1) || (param_2 == 1))
  {
    return 1;
  }

  iVar2 = FUN_0040dffd((unsigned char)param_1);
  iVar3 = FUN_0040dffd((unsigned char)param_2);
  if (DAT_00593e20[iVar3 * 3] == iVar2)
  {
    return 1;
  }
  if ((param_3 < 2) || (DAT_00593e20[iVar3 * 3 + 1] != iVar2))
  {
    if ((param_3 < 3) || (DAT_00593e20[iVar3 * 3 + 2] != iVar2))
    {
      if (param_3 < 4)
      {
        return 0;
      }
      return 1;
    }
    return 1;
  }
  return 1;
}

// FUNCTION: SHANDALAR 0x0056c5ea
int FUN_0056c5ea(int param_1)
{
  int local_8;

  if (((global_cards_data[param_1].extra_ability & 0x180U) != 0) || (global_cards_data[param_1].expansion == '@'))
  {
    global_cards_data[param_1].rarity = 4;
  }
  if (global_cards_data[param_1].rarity == 0xff)
  {
    local_8 = 1;
    global_cards_data[param_1].rarity = (unsigned char)local_8;
  }
  else
  {
    local_8 = (int)(char)global_cards_data[param_1].rarity;
  }
  return local_8;
}

// FUNCTION: SHANDALAR 0x004bb1cf
int FUN_004bb1cf(unsigned int param_1)
{
  int iVar1;
  int local_30;
  int local_2c;
  int local_28;
  unsigned int local_24;
  int aiStack_20[7];

  if ((int)param_1 < 5)
  {
    local_30 = 99;
  }
  else
  {
    for (local_28 = 0; local_28 < 7; local_28 = local_28 + 1)
    {
      aiStack_20[local_28] = 0;
    }
    local_2c = 0;
    local_30 = 0;
    for (local_28 = 0; local_28 < 500; local_28 = local_28 + 1)
    {
      if ((deck[local_28] != -1) && ((DAT_0097f1c1[local_28 * 4] & 0x40) == 0))
      {
        local_2c = local_2c + 1;
        iVar1 = FUN_0040dffd(global_cards_data[(unsigned int)deck[local_28] & 0xfff].color);
        aiStack_20[iVar1] = aiStack_20[iVar1] + 1;
      }
      if (((unsigned int)deck[local_28] & 0xffff7fff) == param_1)
      {
        local_30 = local_30 + 1;
      }
    }

    local_24 = (unsigned int)-1;
    for (local_28 = 1; local_28 < 7; local_28 = local_28 + 1)
    {
      if ((int)local_24 < aiStack_20[local_28])
      {
        local_24 = aiStack_20[local_28];
      }
    }

    g_deck_color_bitmap = 0;
    for (local_28 = 1; local_28 < 7; local_28 = local_28 + 1)
    {
      if ((int)(local_24 * 2) / 3 <= aiStack_20[local_28])
      {
        g_deck_color_bitmap = g_deck_color_bitmap | (1 << ((unsigned char)local_28 & 0x1f));
      }
    }

    local_24 = 1;
    if (0x27 < local_2c)
    {
      local_24 = 2;
    }
    if (0x3b < local_2c)
    {
      local_24 = 3;
    }
    if ((g_world_magic_bitmap & 0x20) != 0)
    {
      local_24 = local_24 + 1;
    }
    if ((global_cards_data[param_1].extra_ability & 0x100) == 0)
    {
      if ((global_cards_data[param_1].expansion & 0xc1) == 0)
      {
        local_24 = local_24 / 2;
      }
    }
    else
    {
      local_24 = (unsigned int)(g_shandalar_difficulty <= (int)local_24);
    }
    local_30 = (int)local_24 - local_30;
  }

  return local_30;
}

// FUNCTION: SHANDALAR 0x004bb458
int FUN_004bb458(int param_1)
{
  int local_8;
  int iVar1;

  for (local_8 = 0; local_8 < DAT_0073c00c + 0x10; local_8 = local_8 + 1)
  {
    if (global_cards_data[local_8].id == *(int *)((char *)Scards + param_1 * 0x10))
    {
      return local_8;
    }
  }

  iVar1 = DAT_0073c00c + 0x10;
  return iVar1;
}

// FUNCTION: SHANDALAR 0x004290e2
void FUN_004290e2(int param_1, int param_2)
{
  if (g_journal_entry_count < 2000)
  {
    g_journal_entries[g_journal_entry_count][0] = param_1;
    g_journal_entries[g_journal_entry_count][1] = param_2;
    g_journal_entries[g_journal_entry_count][2] = (g_world_player_x + ((g_world_player_x >> 0x1f) & 0x1fU)) >> 5;
    g_journal_entries[g_journal_entry_count][3] = (g_world_player_y + ((g_world_player_y >> 0x1f) & 0x1fU)) >> 5;
    g_journal_entry_count = g_journal_entry_count + 1;
  }
}

// FUNCTION: SHANDALAR 0x0056bd9d
int FUN_0056bd9d(unsigned int param_1)
{

  struct
  {
    int local_14;
    int local_10;
    unsigned int local_c;
    int local_8;
    int local_4;
  } s;

  if (FUN_0056c5ea(param_1) >= 3)
  {
    FUN_004290e2((int)param_1 / 0x100 + 8, param_1 & 0xff);
  }
  s.local_8 = FUN_0040dffd(global_cards_data[param_1].color) * 0x20 + (unsigned int)global_cards_data[param_1].type * 0x100 +
              (int)global_cards_data[param_1].name[0];
  s.local_14 = 0;
  for (s.local_10 = 0; s.local_10 < 500; s.local_10 = s.local_10 + 1)
  {
    if (deck[s.local_10] == -1)
    {
      s.local_14 = 1;
    }
  }
  if (s.local_14 == 0)
  {
    return -1;
  }
  for (s.local_10 = 0x1f2; s.local_10 >= 0; s.local_10 = s.local_10 - 1)
  {
    if (deck[s.local_10] != -1)
    {
      s.local_c = (unsigned int)deck[s.local_10] & 0xfff;
      s.local_4 = FUN_0040dffd(global_cards_data[s.local_c].color) * 0x20 + (int)global_cards_data[s.local_c].name[0] +
                  (unsigned int)global_cards_data[s.local_c].type * 0x100;
      if (s.local_4 >= s.local_8)
      {
        *(unsigned int *)(DAT_0097f1c1 + s.local_10 * 4 + 3) = param_1;
        param_1 = (unsigned int)-1;
        return s.local_10 + 1;
      }
      *(unsigned int *)(DAT_0097f1c1 + s.local_10 * 4 + 3) = (unsigned int)deck[s.local_10];
    }
  }
  deck[0] = param_1;
  return 0;
}

// FUNCTION: SHANDALAR 0x005795f0
unsigned int FUN_005795f0(FacemakerWindowBounds *param_1, int param_2, int param_3)
{
  (void)param_1;
  return (unsigned int)GetPixel(global_main_hdc, param_2, param_3);
}

// FUNCTION: SHANDALAR 0x0052280c
int FUN_0052280c(void *param_1)
{
  int uVar1;

  if (DAT_00746e00[1] == 0)
  {
    uVar1 = 0;
  }
  else
  {
    uVar1 = ((int(__cdecl *)(void *))DAT_00746e00[1])(param_1);
  }

  return uVar1;
}

// FUNCTION: SHANDALAR 0x0056302b
int FUN_0056302b(int param_1)
{
  int uVar1;

  switch (param_1)
  {
  case 1:
    uVar1 = 4;
    break;
  case 2:
    uVar1 = 2;
    break;
  case 3:
    uVar1 = 3;
    break;
  case 4:
    uVar1 = 1;
    break;
  case 5:
    uVar1 = 0;
    break;
  case 6:
    uVar1 = 5;
    break;
  default:
    uVar1 = -1;
  }
  return uVar1;
}

// FUNCTION: SHANDALAR 0x00578c60
int FUN_00578c60(void)
{
  int iVar1;

  iVar1 = DAT_00986da0;
  DAT_00986da0 = 0;
  return iVar1;
}

// FUNCTION: SHANDALAR 0x004f6d90
void FUN_004f6d90(void)
{
  int local_24;
  int local_18;
  int local_14;
  int local_10;
  int local_c;
  unsigned int local_8;
  unsigned int uVar1;
  int neighbor_is_open;
  static const int s_neighbor_dx[9] = {0, -1, 0, 1, -1, 1, 1, -1, 0};
  static const int s_neighbor_dy[9] = {0, 0, -1, 0, 1, 1, -1, 1, 0};

  while (1)
  {
    FUN_004f8101();
    FUN_005797e0(PTR_DAT_00583304, 0, 0, 0x140, 200, 0);
    local_24 = 0;
    for (local_10 = 0; local_10 < 0x40; local_10 = local_10 + 1)
    {
      for (local_18 = 0; local_18 < 0x40; local_18 = local_18 + 1)
      {
        if (((local_10 < 2) || (local_18 < 2)) || ((0x3d < local_10) || (0x3d < local_18)))
        {
          PutGraphicsPixel(PTR_DAT_00583304, local_10, local_18, 0);
        }
        else
        {
          local_14 = (local_10 + local_18) * 3 - 0x20;
          local_c = (local_18 - local_10) * 3 + 100;
          if ((local_14 < 4) || (local_c < 4) || ((0x13b < local_14) || (0xc3 < local_c)))
          {
            PutGraphicsPixel(PTR_DAT_00583304, local_10, local_18, 0);
          }
          else
          {
            local_14 = FUN_004f7fb9(local_10, local_18);
            switch ((local_14 + ((unsigned int)local_14 >> 0x1f & 7)) >> 3)
            {
            case 0:
            case 1:
              local_8 = 0;
              break;
            case 2:
              local_8 = 1;
              if (0x15 < local_14)
              {
                local_8 = 8;
              }
              break;
            case 3:
              local_8 = 3;
              break;
            case 4:
              local_8 = 6;
              if (local_14 < 0x22)
              {
                local_8 = 0xd;
              }
              break;
            case 5:
              if (0x2a < local_14)
              {
                local_8 = 2;
              }
              else
              {
                local_8 = 10;
              }
              break;
            case 6:
              local_8 = 2;
              break;
            case 7:
              if (local_14 < 0x3c)
              {
                local_8 = 0xf;
              }
              else
              {
                local_8 = 5;
              }
              break;
            case 8:
            case 9:
            case 10:
            case 0xb:
              local_8 = 5;
              break;
            default:
              local_8 = 0;
              break;
            }

            PutGraphicsPixel(PTR_DAT_00583304, local_10, local_18, (int)local_8);
            if (local_8 != 0)
            {
              local_24 = local_24 + 1;
            }
          }
        }
      }
    }

    if (0x6d5 < local_24)
    {
      for (local_10 = 1; local_10 < 0x3f; local_10 = local_10 + 1)
      {
        for (local_18 = 1; local_18 < 0x3f; local_18 = local_18 + 1)
        {
          local_8 = FUN_0043146b(local_10, local_18);
          if (local_8 == 0)
          {
            neighbor_is_open = 0;
            for (local_14 = 1; local_14 < 9; local_14 = local_14 + 2)
            {
              uVar1 = FUN_0043146b(local_10 + s_neighbor_dx[local_14], local_18 + s_neighbor_dy[local_14]);
              if (uVar1 == 0)
              {
                neighbor_is_open = 1;
                break;
              }
            }
            if (neighbor_is_open == 0)
            {
              PutGraphicsPixel(PTR_DAT_00583304, local_10, local_18, 6);
            }
          }
        }
      }

      FUN_004f7c7d();
      if (FUN_004f717a() != 0)
      {
        FUN_004f78d3();
        for (local_14 = 0; local_14 < 7; local_14 = local_14 + 1)
        {
          g_lair_or_monster_slots[local_14].respawn_timestamp = (4 - g_shandalar_difficulty) * local_14 * -100;
        }
        return;
      }
    }
  }
}

// FUNCTION: SHANDALAR 0x004f717a
int FUN_004f717a(void)
{
  return 1;
}

// FUNCTION: SHANDALAR 0x004f78d3
void FUN_004f78d3(void)
{
  int iVar1;
  int iVar2;
  int local_28;
  int local_24;
  int local_1c;
  int local_18;
  int local_10;
  int local_c;
  int local_8;
  WorldNode *location_data;
  WorldNode *candidate_data;

  for (local_1c = 0; local_1c < 0x80; local_1c = local_1c + 1)
  {
    location_data = &g_town_slots[local_1c];
    for (local_28 = 0; local_28 < location_data->location_type; local_28 = local_28 + 1)
    {
      local_8 = 0;
      do
      {
        local_24 = 0x7fff;
        for (local_18 = 0; local_18 < 0x2a; local_18 = local_18 + 1)
        {
          iVar1 = FUN_00522508(0x80);
          candidate_data = &g_town_slots[iVar1];
          iVar2 = FUN_004ecf30(location_data->world_x - candidate_data->world_x, location_data->world_y - candidate_data->world_y);
          if (iVar2 < local_24)
          {
            local_10 = local_c;
            local_24 = iVar2;
            local_c = iVar1;
          }
        }
        candidate_data = &g_town_slots[local_10];
        iVar1 = FUN_004f7a3c(location_data->world_x, location_data->world_y, candidate_data->world_x, candidate_data->world_y);
      } while ((iVar1 == 0) && (local_8 = local_8 + 1, local_8 < 3));
    }
  }
}

// FUNCTION: SHANDALAR 0x004f7a3c
int FUN_004f7a3c(int param_1, int param_2, int param_3, int param_4)
{
  int iVar1;
  int iVar2;
  int param1;
  unsigned int uVar3;
  int local_34;
  int local_30;
  int local_2c;
  int local_24;
  int local_20;
  int local_1c;
  int local_8;

  local_1c = param_1;
  local_20 = param_2;
  local_34 = 0;
  BlitGraphicsRect(PTR_DAT_00583304, 0, 0, 0x40, 0x80, PTR_DAT_00583304, 0x40, 0);
  FUN_00431526(0x20, param_1, param_2);
  do
  {
    iVar1 = FUN_004ecf30(param_3 - local_1c, param_4 - local_20);
    local_30 = -1;
    local_2c = 0x7fff;
    for (local_24 = 1; local_24 < 9; local_24 = local_24 + 1)
    {
      iVar2 = DAT_005862d8[local_24] + local_1c;
      param1 = DAT_00586340[local_24] + local_20;
      uVar3 = FUN_0043146b(iVar2, param1);
      if ((uVar3 != 0) && ((local_8 = FUN_004ecf30(param_3 - iVar2, param_4 - param1), local_8 < iVar1)))
      {
        if ((uVar3 == 2) || (uVar3 == 0xb))
        {
          local_8 = local_8 + 1;
        }
        if ((uVar3 == 4) || (uVar3 == 5))
        {
          local_8 = local_8 + 4;
        }
        uVar3 = FUN_004314ca(iVar2, param1);
        if (((uVar3 & 0x20) != 0) && (0 < local_34))
        {
          local_8 = local_8 - 4;
        }
        if (local_8 < local_2c)
        {
          local_2c = local_8;
          local_30 = local_24;
        }
      }
    }
    if (local_30 == -1)
    {
      BlitGraphicsRect(PTR_DAT_00583304, 0x40, 0, 0x40, 0x80, PTR_DAT_00583304, 0, 0);
      return 0;
    }
    iVar1 = DAT_005862d8[local_30] + local_1c;
    iVar2 = DAT_00586340[local_30] + local_20;
    uVar3 = FUN_004314ca(iVar1, iVar2);
    FUN_0043174d(local_1c, local_20, local_30);
    if (((uVar3 & 0x20) != 0) && (0 < local_34))
    {
      return 1;
    }
    local_34 = local_34 + 1;
    local_20 = iVar2;
    local_1c = iVar1;
  } while ((iVar1 != param_3) || (iVar2 != param_4));
  return 1;
}

// FUNCTION: SHANDALAR 0x004f7c7d
void FUN_004f7c7d(void)
{
  int bVar1;
  unsigned int uVar2;
  int local_18;
  int local_14;
  int local_10;
  int local_8;

  PutGraphicsPixel(PTR_DAT_00583304, 0xa8, 0x58, 0xff);
  do
  {
    bVar1 = 0;
    for (local_10 = 4; local_10 < 0x40; local_10 = local_10 + 4)
    {
      for (local_14 = 4; local_14 < 0x40; local_14 = local_14 + 4)
      {
        uVar2 = ReadGraphicsPixel(PTR_DAT_00583304->page_number, local_10 + 0x80, local_14 + 0x40);
        if (uVar2 != 0)
        {
          FUN_005797e0(PTR_DAT_00583304, 0x40, 0, 0x40, 0x40, 0);
          FUN_004f7eb2(local_10, local_14, 8);
          PutGraphicsPixel(PTR_DAT_00583304, local_10 + 0x80, local_14 + 0x40, 0);
          for (local_18 = 0; local_18 < 0x40; local_18 = local_18 + 1)
          {
            for (local_8 = 0; local_8 < 0x40; local_8 = local_8 + 1)
            {
              uVar2 = ReadGraphicsPixel(PTR_DAT_00583304->page_number, local_18 + 0x40, local_8);
              if ((uVar2 != 0) && ((uVar2 = ReadGraphicsPixel(PTR_DAT_00583304->page_number, local_18 + 0x80, local_8), uVar2 == 0)))
              {
                bVar1 = 1;
                PutGraphicsPixel(PTR_DAT_00583304, local_18 + 0x80, local_8, 0xff);
                PutGraphicsPixel(PTR_DAT_00583304, local_18 + 0x80, local_8 + 0x40, 0xff);
              }
            }
          }
        }
      }
    }
  } while (bVar1 != 0);

  for (local_10 = 0; local_10 < 0x40; local_10 = local_10 + 1)
  {
    for (local_14 = 0; local_14 < 0x40; local_14 = local_14 + 1)
    {
      uVar2 = ReadGraphicsPixel(PTR_DAT_00583304->page_number, local_10 + 0x80, local_14);
      if (uVar2 == 0)
      {
        PutGraphicsPixel(PTR_DAT_00583304, local_10, local_14, 0);
      }
    }
  }
}

// FUNCTION: SHANDALAR 0x004f7eb2
void FUN_004f7eb2(int param_1, int param_2, unsigned int param_3)
{
  char cVar1;
  char cVar2;
  unsigned int uVar3;
  char local_10;

  PutGraphicsPixel(PTR_DAT_00583304, param_1 + 0x40, param_2, param_3);
  if (1 < (int)param_3)
  {
    for (local_10 = 1; local_10 < 9; local_10 = local_10 + 2)
    {
      cVar1 = (char)DAT_005862d8[(char)local_10] + (char)param_1;
      cVar2 = (char)DAT_00586340[(char)local_10] + (char)param_2;
      uVar3 = ReadGraphicsPixel(PTR_DAT_00583304->page_number, cVar1 + 0x40, cVar2);
      if (((int)(char)uVar3 < (int)param_3) && ((uVar3 = ReadGraphicsPixel(PTR_DAT_00583304->page_number, cVar1, cVar2), uVar3 != 0)))
      {
        FUN_004f7eb2(cVar1, cVar2, param_3 - 1);
      }
    }
  }
}

// FUNCTION: SHANDALAR 0x004f7fb9
int FUN_004f7fb9(int param_1, int param_2)
{
  return (param_1 * 3 + param_2 * 5 + g_monster_timer) & 0x3f;
}

// FUNCTION: SHANDALAR 0x004f8101
void FUN_004f8101(void)
{
  int iVar1;
  int local_10;
  int local_c;
  int local_8;

  for (local_8 = 0; local_8 < 0x12; local_8 = local_8 + 1)
  {
    for (local_10 = 0; local_10 < 0x12; local_10 = local_10 + 1)
    {
      iVar1 = FUN_00522508(0x10);
      DAT_00747ef0[local_10 + local_8 * 0x13] = (char)iVar1;
    }
    DAT_00747ef0[local_8 * 0x13 + 0x12] = DAT_00747ef0[local_8 * 0x13];
  }
  for (local_10 = 0; local_10 < 0x12; local_10 = local_10 + 1)
  {
    DAT_00747ef0[local_10 + 0x156] = DAT_00747ef0[local_10];
  }
  for (local_8 = 0; local_8 < 0x11; local_8 = local_8 + 1)
  {
    for (local_10 = 0; local_10 < 0x11; local_10 = local_10 + 1)
    {
      for (local_c = 1; local_c < 9; local_c = local_c + 1)
      {
      }
      DAT_00747ef0[local_10 + local_8 * 0x13 + 0x170] = DAT_00747ef0[local_10 + local_8 * 0x13];
    }
  }
  for (local_8 = 0; local_8 < 0x11; local_8 = local_8 + 1)
  {
    DAT_00747ef0[local_8 * 0x13 + 0x180] = DAT_00747ef0[local_8 * 0x13 + 0x170];
  }
  for (local_10 = 0; local_10 < 0x11; local_10 = local_10 + 1)
  {
    DAT_00747ef0[local_10 + 0x2a0] = DAT_00747ef0[local_10 + 0x170];
  }
}

// FUNCTION: SHANDALAR 0x005081fa
void FUN_005081fa(void)
{
  unsigned char bVar1;
  unsigned char bVar2;
  int iVar3;
  int param1;
  unsigned int uVar4;
  int iVar5;
  int local_2c;
  int local_28;
  int local_24;
  int local_20;
  int local_10;
  int local_c;
  int local_8;
  ShandalarEncounterSlot *slot;

  for (local_8 = 0; local_8 < 0xf; local_8 = local_8 + 1)
  {
    g_castle_dungeon_slots[local_8].card_slot_3 = -1;
    g_castle_dungeon_slots[local_8].card_slot_2 = g_castle_dungeon_slots[local_8].card_slot_3;
    g_castle_dungeon_slots[local_8].card_slot_1 = g_castle_dungeon_slots[local_8].card_slot_2;
    g_castle_dungeon_slots[local_8].times_entered = -1;
    g_castle_dungeon_slots[local_8].reserved_2c = g_castle_dungeon_slots[local_8].times_entered;
  }

  for (local_2c = 0; local_2c < DAT_0073c00c - 0x39; local_2c = local_2c + 1)
  {
    if ((global_cards_data[local_2c].extra_ability & 0x100) != 0)
    {
      do
      {
        iVar3 = FUN_00522508(10);
        iVar3 = iVar3 + 5;
      } while (g_castle_dungeon_slots[iVar3].card_slot_3 != -1);

      if (g_castle_dungeon_slots[iVar3].card_slot_1 == -1)
      {
        g_castle_dungeon_slots[iVar3].card_slot_1 = local_2c;
      }
      else if (g_castle_dungeon_slots[iVar3].card_slot_2 == -1)
      {
        g_castle_dungeon_slots[iVar3].card_slot_2 = local_2c;
      }
      else
      {
        g_castle_dungeon_slots[iVar3].card_slot_3 = local_2c;
      }
    }
  }

  local_8 = 0;
  do
  {
    if (0xe < local_8)
    {
      return;
    }

    do
    {
      do
      {
        do
        {
          iVar3 = FUN_00522508(0x40);
          param1 = FUN_00522508(0x40);
          uVar4 = FUN_0043146b(iVar3, param1);
        } while (uVar4 == 0);
        uVar4 = FUN_004314ca(iVar3, param1);
      } while ((uVar4 & 0x30) != 0);

      local_28 = 0xff;
      for (local_20 = 0; local_20 < 0x80; local_20 = local_20 + 1)
      {
        if ((local_8 < 5) && (g_town_slots[local_20].location_type == 4))
        {
          uVar4 = FUN_0043146b(g_town_slots[local_20].world_x, g_town_slots[local_20].world_y);
          uVar4 = FUN_005611c8(uVar4);
          if (uVar4 == 1 << (((char)local_8 + 1U) & 0x1f))
          {
            local_10 = local_20;
          }
        }

        if (((1 < g_town_slots[local_20].location_type) && (g_town_slots[local_20].location_type != 4)) &&
            ((iVar5 = FUN_004ecf30(g_town_slots[local_20].world_x - iVar3,
                                   g_town_slots[local_20].world_y - param1),
              iVar5 < local_28)))
        {
          local_24 = local_20;
          local_28 = iVar5;
        }
      }

      for (local_20 = 0; local_20 < local_8; local_20 = local_20 + 1)
      {
        iVar5 = FUN_004ecf30(g_castle_dungeon_slots[local_20].world_x - iVar3, g_castle_dungeon_slots[local_20].world_y - param1);
        if (iVar5 < local_28)
        {
          local_28 = iVar5;
        }
      }
    } while (local_28 < 4);

    FUN_00431526(0x40, iVar3, param1);
    slot = g_castle_dungeon_slots + local_8;
    slot->world_x = iVar3;
    slot->world_y = param1;
    slot->north_of_town_index = local_24;
    iVar3 = FUN_00522508(5);
    slot->color = (unsigned char)iVar3 + 1;
    slot->monster_flags = 2;

    if (local_8 < 5)
    {
      slot->color = (unsigned char)local_8 + 1;
      slot->monster_flags = 0x81;
      slot->world_x = g_town_slots[local_10].world_x;
      slot->world_y = g_town_slots[local_10].world_y;
    }

    bVar1 = slot->monster_flags;
    iVar3 = FUN_00522508(2);
    if (iVar3 + 1 < (int)(unsigned int)bVar1)
    {
      local_c = 0;
      for (local_20 = 0; local_20 < (int)(unsigned int)(unsigned char)slot->monster_flags; local_20 = local_20 + 1)
      {
        local_c = local_c + local_20 * 2 + 4;
      }
    }
    else
    {
      if ((unsigned char)slot->monster_flags < 2)
      {
        local_c = 0x10;
      }
      else
      {
        local_c = 0x1c;
      }
      slot->monster_flags = slot->monster_flags | 0x80;
    }

    if (slot->card_slot_2 == -1)
    {
      local_c = (local_c * 3) / 2;
    }
    if (slot->card_slot_3 != -1)
    {
      local_c = (local_c * 2) / 3;
    }

    bVar1 = slot->monster_flags;
    bVar2 = slot->monster_flags;
    iVar3 = FUN_00522508(2);
    slot->card_in_effect = DAT_0058c5fc[((((bVar1 & 0xc0) == 0) - 1 & 4) + (bVar2 & 0x7f) + iVar3)];
    slot->rules_bitmap = 1;

    switch ((int)(local_c + (local_c >> 0x1f & 3U)) >> 2)
    {
    case 0:
    case 1:
    case 2:
      slot->monster_flags = slot->monster_flags + 1;
    case 3:
      slot->card_in_effect = DAT_0058c5ec[(char)slot->color];
      iVar3 = FUN_00522508(5);
      slot->rules_bitmap = slot->rules_bitmap | 1 << (((char)iVar3 + 4U) & 0x1f);
      break;
    case 4:
      slot->card_in_effect = DAT_0058c5ec[(char)slot->color];
      break;
    case 5:
      iVar3 = FUN_00522508(5);
      slot->rules_bitmap = slot->rules_bitmap | 1 << (((char)iVar3 + 4U) & 0x1f);
      slot->card_in_effect = -1;
      break;
    case 6:
      slot->card_in_effect = -1;
      break;
    }

    if (((slot->rules_bitmap & 0x100) != 0) || (((slot->monster_flags & 0x3f) < 2) && ((slot->rules_bitmap & 0x20) != 0)))
    {
      slot->card_in_effect = DAT_0058c5ec[(char)slot->color];
      slot->rules_bitmap = slot->rules_bitmap & 0xfffffedf;
    }

    if ((slot->monster_flags & 0x7f) == 1)
    {
      slot->rules_bitmap = slot->rules_bitmap & 0xfffffffe;
    }

    if (local_8 < 5)
    {
      slot->rules_bitmap = slot->rules_bitmap | 1;
      slot->rules_bitmap = slot->rules_bitmap | 2;
      slot->card_in_effect = DAT_0058c620[g_shandalar_difficulty + local_8 * 4];
    }
    local_8 = local_8 + 1;
  } while (1);
}

// FUNCTION: SHANDALAR 0x00431351
void FUN_00431351(FacemakerWindowBounds *param_1, int param_2, int param_3, EncodedImage *param_4, int param_5, int param_6)
{
  DrawEncodedImageResampled(param_1, (param_2 * global_screen_width) / 0x280, (global_screen_height * param_3) / 0x1e0,
                            (param_5 * global_screen_width) / 0x280, (param_6 * global_screen_height) / 0x1e0, param_4);
}

// FUNCTION: SHANDALAR 0x0043146b
unsigned int FUN_0043146b(int param_1, int param_2)
{
  if ((0x3f < param_1) || (param_1 < 0))
  {
    param_1 = 0;
  }
  if ((0x3f < param_2) || (param_2 < 0))
  {
    param_2 = 0;
  }
  return FUN_005795f0(PTR_DAT_00583304, param_1, param_2) & 0xf;
}

// FUNCTION: SHANDALAR 0x005611c8
unsigned int FUN_005611c8(unsigned int param_1)
{
  unsigned int local_8;

  switch (param_1)
  {
  case 1:
    local_8 = 4;
    break;
  case 2:
    local_8 = 8;
    break;
  case 3:
    local_8 = 2;
    break;
  case 4:
    local_8 = 0x30;
    break;
  case 5:
    local_8 = 0x10;
    break;
  case 6:
    local_8 = 0x20;
    break;
  case 7:
    local_8 = 0x24;
    break;
  case 8:
    local_8 = 6;
    break;
  case 9:
    local_8 = 0x14;
    break;
  case 10:
    local_8 = 0x28;
    break;
  case 0xb:
    local_8 = 10;
    break;
  case 0xc:
    local_8 = 0x12;
    break;
  case 0xd:
    local_8 = 0x22;
    break;
  case 0xe:
    local_8 = 0xc;
    break;
  case 0xf:
    local_8 = 0x18;
    break;
  default:
    local_8 = 0;
  }

  return local_8;
}

// FUNCTION: SHANDALAR 0x004314ca
unsigned int FUN_004314ca(int param_1, int param_2)
{
  unsigned int uVar1;

  if ((param_1 < 0x40) && (-1 < param_1))
  {
    if ((param_2 < 0x40) && (-1 < param_2))
    {
      uVar1 = FUN_005795f0(PTR_DAT_00583304, param_1, param_2);
    }
    else
    {
      uVar1 = 0;
    }
  }
  else
  {
    uVar1 = 0;
  }

  return uVar1;
}

// FUNCTION: SHANDALAR 0x00431526
void FUN_00431526(unsigned int param_1, int param_2, int param_3)
{
  if ((((param_2 < 0x40) && (-1 < param_2)) && (param_3 < 0x40)) && (-1 < param_3))
  {
    PutGraphicsPixel(PTR_DAT_00583304, param_2, param_3, FUN_005795f0(PTR_DAT_00583304, param_2, param_3) | param_1);
  }
}

// FUNCTION: SHANDALAR 0x0043174d
void FUN_0043174d(int param_1, int param_2, int param_3)
{
  int param1;
  int iVar1;
  unsigned int uVar2;

  if ((((param_1 < 0x40) && (-1 < param_1)) && (param_2 < 0x40)) && (-1 < param_2))
  {
    uVar2 = FUN_005795f0(PTR_DAT_00583304, param_1, param_2 + 0x40);
    PutGraphicsPixel(PTR_DAT_00583304, param_1, param_2 + 0x40, uVar2 | 1 << (((char)param_3 - 1U) & 0x1f));
    FUN_00431526(0x20, param_1, param_2);
    param1 = DAT_005862d8[param_3] + param_1;
    iVar1 = DAT_00586340[param_3] + param_2;
    uVar2 = FUN_005795f0(PTR_DAT_00583304, param1, iVar1 + 0x40);
    PutGraphicsPixel(PTR_DAT_00583304, param1, iVar1 + 0x40, uVar2 | 1 << (((char)param_3 + 3U) & 7));
    FUN_00431526(0x20, param1, iVar1);
  }
}

// FUNCTION: SHANDALAR 0x005019ad
void FUN_005019ad(int param_1)
{
  int tmp;
  DAT_00602fb4 = 0;
  global_saveload_loading = DAT_00602fb4;
  FUN_0046ed33();
  tmp = FUN_00501b7d();
  if (tmp != -1)
  {
    if (param_1 == -1)
    {
      FUN_0046ed03();
      param_1 = FUN_00412bff(DAT_0078cf10, 0x30, 0x20);
      FUN_0046ed33();
    }

    if (param_1 != -1)
    {
      DAT_0074cfe4 = param_1;
      s_D_MAGIC0_SVE_0058c038[7] = (char)FUN_004ece40(param_1);
      if (FUN_00501aec(s_D_MAGIC0_SVE_0058c038) != 0)
      {
        if (DAT_00602fb4 == 0)
        {
          strcpy(DAT_0078cf10, " Game has been saved.\n");
        }
        else
        {
          strcpy(DAT_0078cf10, " Game NOT saved.\n");
          FUN_005797e0(PTR_DAT_005832b4, 0x40, 0x7f, 0xc0, 0x22, 0xc);
        }

        if (DAT_00602fb4 == 0xd)
        {
          strcat(DAT_0078cf10, " Write access denied.\n");
        }

        if (DAT_00602fb4 == 0x1c)
        {
          strcat(DAT_0078cf10, " Disk Full.\n");
        }
        strcat(DAT_0078cf10, " Press key to continue.\n");
      }
    }
  }
  FUN_0046ed03();
}

// FUNCTION: SHANDALAR 0x005031a8
int FUN_005031a8(void)
{
  return RunLoadSaveMenu(0);
}

// FUNCTION: SHANDALAR 0x00501760
int FUN_00501760(int param_1)
{
  int ret;
  unsigned int local_4;
  char save_filename[0xd];

  global_saveload_loading = 1;
  FUN_0046ed33();
  ret = FUN_00501b7d();
  if (ret != -1)
  {
    if (param_1 == -1)
    {
      strcpy(DAT_0078cf10, "\x8cSelect Load File...\n");
      *(int *)(DAT_0077f190 + 0x38) = 0;

      for (local_4 = 0; local_4 < 10; local_4 = local_4 + 1)
      {
        s_D_MAGIC0_SVE_0058c038[7] = (char)FUN_004ece40(local_4);
        if (FUN_005018e8(s_D_MAGIC0_SVE_0058c038, 1) != 0)
        {
          *(unsigned int *)(DAT_0077f190 + 0x38) = *(unsigned int *)(DAT_0077f190 + 0x38) | (1 << (unsigned char)local_4);
        }
      }

      FUN_0046ed03();
      DAT_0074cfe4 = FUN_00412bff(DAT_0078cf10, 0x30, 0x40);
      FUN_0046ed33();
      if ((*(unsigned int *)(DAT_0077f190 + 0x38) & (1 << (unsigned char)DAT_0074cfe4)) == 0)
      {
        DAT_0074cfe4 = -1;
      }
    }
    else
    {
      DAT_0074cfe4 = param_1;
    }

    if (DAT_0074cfe4 != -1)
    {
      s_D_MAGIC0_SVE_0058c038[7] = (char)FUN_004ece40(DAT_0074cfe4);
      if (FUN_005018e8(s_D_MAGIC0_SVE_0058c038, 0) == 0)
      {
        DAT_0074cfe4 = -1;
      }
    }

    if (DAT_0074cfe4 == -1)
    {
      FUN_00412bff("Error Loading Save Game File:EXITING\n ", 100, 0x50);
      exit(1);
    }

    FUN_0046ed03();
    return DAT_0074cfe4;
  }
  else
  {
    FUN_0046ed03();
    return -1;
  }
}

// FUNCTION: SHANDALAR 0x004ece40
int FUN_004ece40(int param_1)
{
  int iVar1;

  if ((param_1 < 0) || (9 < param_1))
  {
    if ((param_1 < 10) || (0xf < param_1))
    {
      iVar1 = 0;
    }
    else
    {
      iVar1 = param_1 + 0x57;
    }
  }
  else
  {
    iVar1 = param_1 + 0x30;
  }

  return iVar1;
}

// FUNCTION: SHANDALAR 0x00501b7d
int FUN_00501b7d(void)
{
  char local_108[0x104];

  if (DAT_0058c13c == -1)
  {
    GetCurrentDirectoryA(0x100, local_108);
    s_D_MAGIC0_SVE_0058c038[0] = local_108[0];
  }

  return tolower(s_D_MAGIC0_SVE_0058c038[0]) - 0x61;
}

// FUNCTION: SHANDALAR 0x00501e44
int FUN_00501e44(char *param_1)
{
  struct
  {
    int pad_00;
    int saved_rect[4];
    FacemakerWindowBounds page4_bounds;
    FacemakerWindowBounds *page4_bounds_ptr;
    DIBSurface *page4_dib;
    int image_height;
    int image_width;
    int slot_index;
    int player_index;
  } s;

  strcpy(param_1 + 9, "SVE");
  DAT_006abe38 = _open(param_1, 0x8000);
  if (DAT_006abe38 == -1)
  {
    strcpy(DAT_0078cf10, "File Error: ");
    strcat(DAT_0078cf10, param_1);
    strcat(DAT_0078cf10, "\n");
    FUN_00412bff(DAT_0078cf10, 100, 0x50);
    return 0;
  }

  global_saveload_loading = 1;
  save_or_load_ver1();
  _close(DAT_006abe38);
  for (s.player_index = 0; s.player_index < 2; s.player_index = s.player_index + 1)
  {
    for (s.slot_index = 0; s.slot_index < 0x96; s.slot_index = s.slot_index + 1)
    {
      if (*(int *)(s.player_index * 0xb0f4 + 0x008ca2dc + s.slot_index * 300) != -1)
      {
        active_cards_count[s.player_index] = s.slot_index;
      }
    }
  }

  if (unk_00742fc4 == 0)
  {
    FUN_0046ed33();
    strcpy(param_1 + 9, "map");
    LoadPcxIntoPage(2, param_1);
    FUN_0046ed03();
  }

  s.page4_bounds.page_number = 4;
  s.page4_bounds.unk_04 = 0;
  s.page4_bounds.unk_08 = 0;
  s.page4_bounds.max_x = 800;
  s.page4_bounds.max_y = 600;
  s.page4_bounds.unk_14 = 1;
  s.page4_bounds.text_color = 0xf;
  s.page4_bounds.unk_1c = 4;
  s.page4_bounds.font_slot = 0;
  s.page4_bounds_ptr = &s.page4_bounds;
  s.image_width = 0x89;
  s.image_height = 0xa9;
  if (DAT_0078990c[6] == 0)
  {
    s.page4_dib = (DIBSurface *)CreateGraphicsPage(4, s.image_width * 2, s.image_height, 8);
    SetGraphicsPage(4, s.page4_dib);
  }
  else
  {
    g_graphics_pages[4] = (DIBSurface *)DAT_0078990c[6];
  }

  FUN_00579ea0(s.saved_rect, s.page4_bounds_ptr, 0, 0, s.image_width * 2, s.image_height);
  FUN_005797e0(s.page4_bounds_ptr, 0, 0, s.image_width * 2, s.image_height, 0);
  strcpy(param_1 + 9, "fce");
  LoadPcxIntoPage(4, param_1);
  BuildFacemakerPortraitSprites(&s.page4_bounds);
  LoadPcxIntoPage(4, param_1);
  DAT_0078990c[7] = (int)g_graphics_pages[4]->hBitmap;
  DAT_0078990c[6] = (int)g_graphics_pages[4];
  SelectObject(g_graphics_pages[4]->hTempDC, g_graphics_pages[4]->hPreviousBitmap);
  g_graphics_pages[4] = (DIBSurface *)0;
  return 1;
}

// FUNCTION: SHANDALAR 0x005018e8
unsigned int FUN_005018e8(char *param_1, int param_2)
{
  strcpy(param_1 + 9, "SVE");
  if (param_2 != 0)
  {
    DAT_006abe38 = open(param_1, 0x8000);
    if (DAT_006abe38 != -1)
    {
      strcat(DAT_0078cf10, "OK\n");
    }
    else
    {
      sprintf(DAT_0078cf10, "%s\n", gs_loadsave_0077d1b0[2]);
    }

    close(DAT_006abe38);
    return (DAT_006abe38 != -1) ? 1 : 0;
  }

  return (unsigned int)FUN_00501e44(param_1);

  return 1;
}

// FUNCTION: SHANDALAR 0x005020fe
int FUN_005020fe(char *param_1)
{
  strcpy(param_1 + 9, "SVE");
  DAT_006abe38 = _open(param_1, 0x8301, 0x80);
  if (DAT_006abe38 == -1)
  {
    if (errno == ENOSPC)
    {
      DAT_00602fb4 = 0x1c;
    }
    else
    {
      DAT_00602fb4 = 0xd;
    }
    return 0;
  }

  _close(DAT_006abe38);
  DAT_00602fb4 = 0;
  return 1;
}

// FUNCTION: SHANDALAR 0x00501aec
int FUN_00501aec(char *param_1)
{
  strcpy(DAT_0078cf10, " ");
  strcat(DAT_0078cf10, "\n ... save in progress.\n");
  FUN_005020fe(param_1);
  return 1;
}

// FUNCTION: SHANDALAR 0x0040dffd
int FUN_0040dffd(int param_1)
{
  if (param_1 & 2)
    return 1;

  if (param_1 & 4)
    return 2;

  if (param_1 & 8)
    return 3;

  if (param_1 & 0x10)
    return 4;

  if (param_1 & 0x20)
    return 5;

  return 0;
}

// FUNCTION: SHANDALAR 0x005226e0
int FUN_005226e0(void)
{
  int i;

  for (i = 0; i < 3; i = i + 1)
  {
    DAT_00746e00[i] = 0;
  }

  DAT_0058e050 = LoadLibraryA("statwin.dll");
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

// FUNCTION: SHANDALAR 0x004ffcb4
int FUN_004ffcb4(AdvMenuControl *param_1)
{
  int previous_state;

  previous_state = param_1->state;
  DAT_00747ee0 = 1;
  param_1->state = 0;
  param_1->on_render(param_1, 3);
  DAT_00747ee0 = 0;
  param_1->state = 3;
  return previous_state;
}

// FUNCTION: SHANDALAR 0x004ff456
int FUN_004ff456(void *param_1, int param_2)
{
  AdvMenuControl *control;
  int iVar1;
  int iVar2;

  control = (AdvMenuControl *)param_1;
  if (DAT_00747ee0 == 0)
  {
    if ((DAT_00986d9c < control->x) || (control->x + control->width < DAT_00986d9c))
    {
      return 0;
    }
    if ((DAT_00986d98 < control->y) || (control->y + control->height < DAT_00986d98))
    {
      return 0;
    }
  }
  if (control->state == 3)
  {
    return 0;
  }
  if (param_2 == 2)
  {
    iVar1 = ScaleUiCoordinate(4);
    iVar2 = ScaleUiCoordinate(8);
    BlitGraphicsRect(PTR_DAT_00583354, (unsigned int)control->x, control->y, (unsigned int)control->width, (DWORD)control->height, PTR_DAT_005832dc,
                     control->x, control->y);
    DrawEncodedImageResampled(PTR_DAT_005832dc, control->x + iVar1, control->y + iVar1, control->width - iVar2, control->height - iVar2,
                              (EncodedImage *)control->mode_data[3]);
    BlitGraphicsRect(PTR_DAT_005832dc, (unsigned int)control->x, control->y, (unsigned int)control->width, (DWORD)control->height, PTR_DAT_005832b4,
                     control->x, control->y);
    if (control->on_activate != (AdvMenuActivateCallback)0)
    {
      control->on_activate(control);
    }
  }
  else
  {
    DrawEncodedImageResampled(DAT_0058b580, control->x, control->y, control->width, control->height, (EncodedImage *)control->mode_data[param_2]);
  }
  return 1;
}

// FUNCTION: SHANDALAR 0x004ff652
int FUN_004ff652(void *param_1, int param_2)
{
  AdvMenuControl *control;
  EncodedImage *sprite;
  int iVar1;
  int iVar2;
  int iVar3;
  unsigned int local_width;
  DWORD local_height;
  unsigned int local_x;
  int local_y;

  control = (AdvMenuControl *)param_1;
  if (param_2 == 0)
  {
    sprite = g_face_preview_sprite_selected;
  }
  else
  {
    sprite = g_face_preview_sprite_group[0];
  }
  if (DAT_00747ee0 == 0)
  {
    if ((DAT_00986d9c < control->x) || (control->x + control->width < DAT_00986d9c))
    {
      return 0;
    }
    if ((DAT_00986d98 < control->y) || (control->y + control->height < DAT_00986d98))
    {
      return 0;
    }
  }
  if (control->state == 3)
  {
    return 0;
  }
  local_height = (DWORD)ScaleUiCoordinate(0x30);
  local_width = (unsigned int)ScaleUiCoordinate((sprite->width * 0x30) / (int)sprite->height);
  iVar1 = ScaleUiCoordinate(0x20);
  local_x = (unsigned int)(iVar1 - (int)local_width / 2);
  iVar1 = ScaleUiCoordinate(0x106);
  local_y = iVar1 - (int)local_height / 2;
  if (param_2 == 2)
  {
    iVar2 = ScaleUiCoordinate(4);
    iVar3 = ScaleUiCoordinate(8);
    BlitGraphicsRect(PTR_DAT_00583354, local_x, local_y, local_width, local_height, PTR_DAT_005832dc, local_x, local_y);
    DrawEncodedImageResampled(PTR_DAT_005832dc, iVar2 + local_x, iVar2 + local_y, local_width - iVar3, (int)local_height - iVar3, sprite);
    BlitGraphicsRect(PTR_DAT_005832dc, local_x, local_y, local_width, local_height, PTR_DAT_005832b4, local_x, local_y);
    if (control->on_activate != (AdvMenuActivateCallback)0)
    {
      control->on_activate(control);
    }
  }
  else
  {
    DrawEncodedImageResampled(DAT_0058b580, local_x, local_y, local_width, (int)local_height, sprite);
  }
  return 1;
}

// FUNCTION: SHANDALAR 0x004ff888
int FUN_004ff888(void *param_1, int param_2)
{
  AdvMenuControl *control;
  int local_58[6];
  int local_3c[6];
  int local_40;
  int local_24;
  unsigned int local_20;
  DWORD local_1c;
  unsigned int local_18;
  EncodedImage *local_14;
  int local_10;
  int local_c;
  int local_8;
  int iVar1;
  int iVar2;

  control = (AdvMenuControl *)param_1;
  local_8 = control->selection_value * 2 + -0x60;
  if (DAT_00747ee0 == 0)
  {
    if ((DAT_00986d9c < control->x) || (control->x + control->width < DAT_00986d9c))
    {
      return 0;
    }
    if ((DAT_00986d98 < control->y) || (control->y + control->height < DAT_00986d98))
    {
      return 0;
    }
  }
  if (control->state == 3)
  {
    return 0;
  }
  if (((g_world_magic_bitmap & (1 << ((unsigned char)local_8 & 0x1f))) != 0) && (DAT_0078990c[local_8 / 2] != 0))
  {
    local_c = FUN_004bb458(local_8);
    if (param_2 == 2)
    {
      local_14 = (EncodedImage *)control->mode_data[3];
      local_20 = (unsigned int)ScaleUiCoordinate(DAT_0058b880[local_8].x);
      local_24 = ScaleUiCoordinate(DAT_0058b880[local_8].y);
      local_18 = (unsigned int)ScaleUiCoordinate(DAT_0058b880[local_8].width);
      local_1c = (DWORD)ScaleUiCoordinate(DAT_0058b880[local_8].height);
      local_10 = ScaleUiCoordinate(4);
      iVar1 = ScaleUiCoordinate(0x148);
      BlitGraphicsRect(PTR_DAT_0058332c, local_20, local_24 - iVar1, local_18, local_1c, PTR_DAT_005832dc, local_20, local_24);
      FUN_00431351(PTR_DAT_005832dc, DAT_0058b880[local_8].x, DAT_0058b880[local_8].y, (EncodedImage *)DAT_00749250[local_8],
                   DAT_0058b880[local_8].width, DAT_0058b880[local_8].height);
      DrawEncodedImageResampled(PTR_DAT_005832dc, local_10 + local_20, local_10 + local_24, local_18 - local_10 * 2, (int)local_1c - local_10 * 2, local_14);

      local_3c[0] = 0x6c;
      local_3c[1] = 0xbf;
      local_3c[2] = 0x10e;
      local_3c[3] = 0x15e;
      local_3c[4] = 0x1b1;
      local_58[0] = 2;
      local_58[1] = 1;
      local_58[2] = 4;
      local_58[3] = 3;
      local_58[4] = 0;
      local_58[5] = local_8 / 2 - 1;
      PTR_DAT_005832b4->font_slot = 4;
      local_40 = 400 - (int)DAT_007486e0[0]->height / 2;
      iVar2 = local_58[local_58[5]];
      FUN_00431351(PTR_DAT_005832dc, local_3c[local_58[5]] - 0x1e, local_40, DAT_007486e0[iVar2], (int)DAT_007486e0[0]->width,
                   (int)DAT_007486e0[0]->height);
      BlitGraphicsRect(PTR_DAT_005832dc, local_20, local_24, local_18, local_1c, PTR_DAT_005832b4, local_20, local_24);
      if (control->on_activate != (AdvMenuActivateCallback)0)
      {
        control->on_activate(control);
      }
    }
    else
    {
      FUN_00431351(PTR_DAT_005832b4, DAT_0058b880[local_8].x, DAT_0058b880[local_8].y, (EncodedImage *)control->mode_data[param_2],
                   DAT_0058b880[local_8].width, DAT_0058b880[local_8].height);
    }
  }
  return 1;
}

// FUNCTION: SHANDALAR 0x004ffc5d
int FUN_004ffc5d(AdvMenuControl *control)
{
  FUN_004ecfa2();
  if (DAT_0073e880 == 0)
  {
    DAT_0073e880 = control->selection_value;
  }
  DAT_00986d94 = 0;
  FUN_005626b0(DAT_0058b87c[control->selection_value], 0xf, 100, 100, 0);
  return 0;
}

// FUNCTION: SHANDALAR 0x004ffd56
int FUN_004ffd56(AdvMenuControl *control)
{
  FUN_004ecfa2();
  if (DAT_0073e880 == 0)
  {
    DAT_0073e880 = control->selection_value;
  }
  DAT_00986d94 = 0;
  FUN_005626b0("x_sound_button2.wav", 0xf, 100, 100, 0);
  return 0;
}

// FUNCTION: SHANDALAR 0x004ffda4
void FUN_004ffda4(void)
{
  int i;
  int sprite_index;

  DAT_00746ec0 = *PTR_DAT_005832b4;

  for (i = 0; i < 4; i = i + 1)
  {
    DAT_0058b588[i].mode_data[0] = DAT_00748d10[i];
    DAT_0058b588[i].mode_data[1] = DAT_00748d20[i];
    DAT_0058b588[i].mode_data[2] = DAT_00748d20[i];
    DAT_0058b588[i].mode_data[3] = DAT_00748d10[i];
  }

  for (i = 0; i < 5; i = i + 1)
  {
    sprite_index = i * 2 + 2;
    DAT_0058b6d8[i].mode_data[0] = DAT_007491c0[sprite_index];
    DAT_0058b6d8[i].mode_data[1] = DAT_007491f0[sprite_index];
    DAT_0058b6d8[i].mode_data[2] = DAT_007491f0[sprite_index];
    DAT_0058b6d8[i].mode_data[3] = DAT_00749220[sprite_index];
  }

  if (global_screen_width != 0x280)
  {
    for (i = 0; i < 4; i = i + 1)
    {
      DAT_0058b588[i].x = (DAT_0058b588[i].x * global_screen_width) / 0x280;
      DAT_0058b588[i].y = (DAT_0058b588[i].y * global_screen_width) / 0x280;
      DAT_0058b588[i].width = (DAT_0058b588[i].width * global_screen_width) / 0x280;
      DAT_0058b588[i].height = (DAT_0058b588[i].height * global_screen_width) / 0x280;
    }
    for (i = 0; i < 5; i = i + 1)
    {
      DAT_0058b6d8[i].x = (DAT_0058b6d8[i].x * global_screen_width) / 0x280;
      DAT_0058b6d8[i].y = (DAT_0058b6d8[i].y * global_screen_width) / 0x280;
      DAT_0058b6d8[i].width = (DAT_0058b6d8[i].width * global_screen_width) / 0x280;
      DAT_0058b6d8[i].height = (DAT_0058b6d8[i].height * global_screen_width) / 0x280;
    }
  }

  FUN_0050014e(DAT_0058b588, 4, 0);
  FUN_0050014e(DAT_0058b6d8, 5, 0);
}

// FUNCTION: SHANDALAR 0x005631ca
void FUN_005631ca(void)
{
  int iVar1;
  int iVar2;
  int iVar3;
  int local_4c;
  int local_48;
  int local_44;
  int local_3c[5];
  int aiStack_28[5];
  unsigned char auStack_14[5];
  unsigned char local_f;
  unsigned char local_e;
  unsigned char local_d;
  int local_8;

  for (local_44 = 0; local_44 < 5; local_44 = local_44 + 1)
  {
    local_4c = 0;
    local_8 = 0;
    iVar1 = FUN_0056302b(local_44 + 1);
    for (local_48 = 0; local_48 < 0x80; local_48 = local_48 + 1)
    {
      if ((((unsigned int)g_town_slots[local_48].status_and_ruling_wizard & 0xff00U) != 0) &&
          (((g_town_slots[local_48].status_and_ruling_wizard >> 8) - 1) == local_44))
      {
        local_8 = local_8 + 1;
      }
    }
    auStack_14[iVar1] = (unsigned char)local_8;
    if (DAT_0073ea70[local_44] == 0)
    {
      iVar2 = g_shandalar_difficulty * local_8 + g_shandalar_difficulty * 5 + 0x1e;
      for (local_48 = 0; (local_48 < 1000) && (((unsigned char *)&g_duel_victory_log)[local_48] != '\0'); local_48 = local_48 + 1)
      {
        if ((((int)(char)((unsigned char *)&g_duel_victory_log)[local_48]) >> 4) == local_44 + 1)
        {
          local_4c = local_4c + 1;
        }
      }
      aiStack_28[iVar1] = local_4c;
      iVar3 = g_shandalar_difficulty * 5 + 0x14;
      local_4c = iVar2 - local_4c;
      if (iVar3 <= local_4c)
      {
        iVar3 = local_4c;
      }
      local_3c[iVar1] = 0x1e - (iVar2 - iVar3);
    }
    else
    {
      local_3c[iVar1] = 0;
    }
  }
  local_f = 0;
  local_e = 0;
  local_d = 0;
  FUN_0052280c(local_3c);
}

// FUNCTION: SHANDALAR 0x0056bff1
void FUN_0056bff1(void)
{
  int iVar1;
  int local_7dc;
  unsigned int auStack_7d8[500];
  int local_8;

  for (local_7dc = 0; local_7dc < 500; local_7dc = local_7dc + 1)
  {
    auStack_7d8[local_7dc] = deck[local_7dc];
    deck[local_7dc] = -1;
  }
  local_8 = g_journal_entry_count;
  for (local_7dc = 0; local_7dc < 500; local_7dc = local_7dc + 1)
  {
    if (auStack_7d8[local_7dc] != 0xffffffff)
    {
      iVar1 = FUN_0056bd9d(auStack_7d8[local_7dc] & 0xfff);
      deck[iVar1] = deck[iVar1] | (auStack_7d8[local_7dc] & 0xfffff000);
    }
  }
  g_journal_entry_count = local_8;
}

// FUNCTION: SHANDALAR 0x0054cdbd
void FUN_0054cdbd(void)
{
  int viewport_height;
  int viewport_top;

  DAT_006527b0 = -1;
  DAT_00590764 = -1;
  DAT_00590768 = 1;
  if (DAT_008bd200 == 0)
  {
    LoadPcxIntoPage(1, (char *)PTR_s_advinter800_pic_00589de8);
    BlitGraphicsRect(PTR_DAT_005832dc, 0, 0, global_screen_width, global_screen_height, PTR_DAT_005832b4, 0, 0);
    FUN_0050027e(0, 4);
  }

  DAT_0073eaa0 = 0;
  if (DAT_00650f28 == 0)
  {
    viewport_height = ScaleUiCoordinate(0x1e0) - ScaleUiCoordinate(0x148);
    viewport_top = ScaleUiCoordinate(0x148);
    BlitGraphicsRect(PTR_DAT_005832dc, 0, viewport_top, ScaleUiCoordinate(0x280), viewport_height, PTR_DAT_0058332c, 0, 0);
    BlitGraphicsRect(PTR_DAT_005832dc, 0, 0, ScaleUiCoordinate(0x40), ScaleUiCoordinate(0x148), PTR_DAT_00583354, 0, 0);
    DAT_00650f28 = 1;
  }
}

// FUNCTION: SHANDALAR 0x0054ac08
void FUN_0054ac08(int param_1, int param_2, int param_3)
{
  (void)param_1;
  (void)param_2;
  (void)param_3;
}

// FUNCTION: SHANDALAR 0x0055e1b2
int FUN_0055e1b2(void)
{
  int menu_width;
  int menu_height;
  int old_page_mode;

  old_page_mode = PTR_DAT_005832b4->font_slot;
  FUN_0056cc4d("ADVstrings.txt", "STARTUP");
  PTR_DAT_005832b4->font_slot = 4;
  menu_width = ScaleUiCoordinate(0x40);
  menu_height = ScaleUiCoordinate(0x50);

  switch (FUN_00412bff(text_lines[1], menu_height, menu_width))
  {
  case 0:
    FUN_0041d24b(0x53);
    break;
  case 1:
    FUN_0041d24b(0x4c);
    break;
  case 2:
    FUN_0041d24b(0x51);
    break;
  case 3:
    FUN_0041d24b(0x3b00);
    break;
  case 4:
    FUN_0041d24b(0x3c00);
    break;
  case 5:
    FUN_0041d24b(0x3d00);
    break;
  case 6:
    FUN_0041d24b(0x3e00);
    break;
  case 7:
    FUN_0041d24b(0x3f00);
    break;
  case 8:
    FUN_0041d24b(0x4000);
    break;
  default:
    FUN_0054cdbd();
    break;
  }

  PTR_DAT_005832b4->font_slot = old_page_mode;
}

// FUNCTION: SHANDALAR 0x0055e651
int FUN_0055e651(void)
{
  int mapped_key;

  mapped_key = -1;
  switch (DAT_0073e880)
  {
  case 1:
    FUN_0041d24b(0x3b00);
    break;
  case 2:
    FUN_0041d24b(0x3c00);
    break;
  case 3:
    FUN_0041d24b(0x3d00);
    break;
  case 4:
    FUN_0041d24b(0x3e00);
    break;
  case 5:
    FUN_0041d24b(0x3f00);
    break;
  case 6:
    FUN_0041d24b(0x4000);
    break;
  case 0x31:
    FUN_0041d24b(0x31);
    break;
  case 0x32:
    FUN_0041d24b(0x32);
    break;
  case 0x33:
    FUN_0041d24b(0x33);
    break;
  case 0x34:
    FUN_0041d24b(0x34);
    break;
  case 0x35:
    FUN_0041d24b(0x35);
    break;
  default:
    FUN_0046eca4();
    if (DAT_007898f0 != 0)
    {
      mapped_key = FUN_0055e31f(DAT_007898f4, DAT_007898f8);
    }
    if (mapped_key != -1)
    {
      FUN_0041d24b(mapped_key);
    }
    break;
  }

  DAT_0073e880 = 0;
  return 0;
}

// FUNCTION: SHANDALAR 0x0055e31f
int FUN_0055e31f(int param_1, int param_2)
{
  struct
  {
    int local_18;
    int local_14;
    int local_10;
    int local_c;
    unsigned int direction;
  } locals;
  int keycode_map[10];

  keycode_map[0] = 0x4800;
  keycode_map[1] = 0x4900;
  keycode_map[2] = 0x4d00;
  keycode_map[3] = 0x5100;
  keycode_map[4] = 0x5000;
  keycode_map[5] = 0x4f00;
  keycode_map[6] = 0x4b00;
  keycode_map[7] = 0x4700;
  keycode_map[8] = 0x4800;

  if ((param_1 < ScaleUiCoordinate(0x40) || ScaleUiCoordinate(0x240) < param_1) || (param_2 < ScaleUiCoordinate(0x30) || ScaleUiCoordinate(0x148) < param_2))
  {
    return -1;
  }

  if ((ScaleUiCoordinate(0x130) < param_1 && param_1 < ScaleUiCoordinate(0x158)) &&
      (ScaleUiCoordinate(0xa8) < param_2 && param_2 < ScaleUiCoordinate(0xdd)))
  {
    return 0x20;
  }

  locals.local_14 = ScaleUiCoordinate(0x140);
  locals.local_18 = ScaleUiCoordinate(0xbc);
  param_1 = param_1 - locals.local_14;
  param_2 = locals.local_18 - param_2;
  locals.local_c = abs(param_1);
  locals.local_10 = abs(param_2);

  if ((param_1 < 0) || (param_2 < 0))
  {
    if ((param_1 < 0) || (-1 < param_2))
    {
      if ((param_1 < 0) && (param_2 < 0))
      {
        locals.direction = 4;
      }
      else if ((param_1 < 0) && (-1 < param_2))
      {
        locals.direction = 6;
      }
    }
    else
    {
      locals.direction = 2;
    }
  }
  else
  {
    locals.direction = 0;
  }

  if ((((locals.direction & 2) == 0) && (locals.local_10 < locals.local_c)) || (((locals.direction & 2) != 0) && (locals.local_c < locals.local_10)))
  {
    locals.direction = locals.direction + 1;
  }

  switch (locals.direction)
  {
  case 0:
  case 3:
  case 4:
  case 7:
    keycode_map[9] = (locals.local_c * 0x9a85) >> 0xe;
    if (param_2 < 0)
    {
      keycode_map[9] = -keycode_map[9];
    }
    break;
  case 1:
  case 2:
  case 5:
  case 6:
    keycode_map[9] = (locals.local_c * 0x1a82) >> 0xe;
    if (param_2 < 0)
    {
      keycode_map[9] = -keycode_map[9];
    }
    break;
  }

  switch (locals.direction)
  {
  case 0:
  case 1:
  case 2:
  case 3:
    if (param_2 < keycode_map[9])
    {
      locals.direction = locals.direction + 1;
    }
    break;
  case 4:
  case 5:
  case 6:
  case 7:
    if (keycode_map[9] < param_2)
    {
      locals.direction = locals.direction + 1;
    }
    break;
  }

  return keycode_map[locals.direction];
}

// FUNCTION: SHANDALAR 0x0055e808
void FUN_0055e808(void) {}

// FUNCTION: SHANDALAR 0x0055fd27
void FUN_0055fd27(void) {}

// FUNCTION: SHANDALAR 0x00561647
void FUN_00561647(void)
{
  int local_8;
  int *timer;

  for (local_8 = 0; local_8 < 0xc; local_8 = local_8 + 1)
  {
    timer = (int *)((char *)&DAT_005863c8 + 4 + local_8 * 0x10);
    if (0 < *timer)
    {
      *timer = *timer - 1;
      if (*timer == 0)
      {
        FUN_0054cdbd();
      }
    }
  }
}

// FUNCTION: SHANDALAR 0x0046eca4
void FUN_0046eca4(void)
{
  unsigned int uVar1;

  if (DAT_00586494 == 0)
  {
    DAT_007898f8 = 0;
    DAT_007898f4 = 0;
    DAT_007898f0 = 0;
  }
  else
  {
    uVar1 = FUN_00578c60();
    DAT_007898f0 = uVar1 | DAT_00986d94;
    DAT_007898f4 = DAT_00986d9c;
    DAT_007898f8 = DAT_00986d98;
  }
}

// FUNCTION: SHANDALAR 0x0041d21d
unsigned int FUN_0041d21d(void)
{
  if (g_key_input_queue_count == 0)
  {
    return 0;
  }
  else
  {
    return g_key_input_queue[0];
  }
}

// FUNCTION: SHANDALAR 0x0041d24b
void FUN_0041d24b(int param_1)
{
  memmove(g_key_input_queue + 1, g_key_input_queue, (size_t)g_key_input_queue_count * sizeof(g_key_input_queue[0]));
  g_key_input_queue_count = g_key_input_queue_count + 1;
  g_key_input_queue[0] = (unsigned int)param_1;
}

// FUNCTION: SHANDALAR 0x005003b7
int FUN_005003b7(int param_1, int param_2, int param_3)
{
  int i;
  int queued_key;
  int full_key;

  (void)param_1;
  (void)param_2;

  DAT_005b80d8 = 1;
  if (HasQueuedKeyInput() != 0)
  {
    queued_key = FUN_0041d21d();
    if ((queued_key == 0xf09) || (queued_key == 0xf0f) ||
        ((DAT_005b80e0[DAT_0058b584] != 0) && ((queued_key == 0x4800) || (queued_key == 0x5000))))
    {
      int direction;

      PopQueuedKeyInput();
      full_key = (int)queued_key;
      switch (full_key)
      {
      case 0xf0f:
      case 0x4800:
        direction = -1;
        break;
      case 0xf09:
      case 0x5000:
        direction = 1;
        break;
      default:
        break;
      }

      if (DAT_0058b954 == -1)
      {
        if (direction <= 0)
        {
          DAT_0058b954 = DAT_00746ef0[DAT_0058b584] - 1;
        }
        else
        {
          DAT_0058b954 = 0;
        }
      }
      else
      {
        DAT_0058b954 = (DAT_00746ef0[DAT_0058b584] + DAT_0058b954 + direction) % DAT_00746ef0[DAT_0058b584];
      }

      while (DAT_00746f40[DAT_0058b584][DAT_0058b954]->state == 3)
      {
        DAT_0058b954 = (DAT_00746ef0[DAT_0058b584] + DAT_0058b954 + direction) % DAT_00746ef0[DAT_0058b584];
      }

      if (DAT_0058b958 != -1)
      {
        DAT_00746f40[DAT_0058b584][DAT_0058b958]->on_render(DAT_00746f40[DAT_0058b584][DAT_0058b958], 0);
      }

      if (DAT_00746f40[DAT_0058b584][DAT_0058b954]->x != -1)
      {
        SetCursorPos(DAT_00746f40[DAT_0058b584][DAT_0058b954]->x + DAT_00746f40[DAT_0058b584][DAT_0058b954]->width / 2,
                     DAT_00746f40[DAT_0058b584][DAT_0058b954]->y + DAT_00746f40[DAT_0058b584][DAT_0058b954]->height / 2);
      }
      DAT_00746f40[DAT_0058b584][DAT_0058b954]->on_render(DAT_00746f40[DAT_0058b584][DAT_0058b954], 1);
    }

    full_key = (int)queued_key;
    queued_key &= 0xff;
    if ((queued_key != 0) || (DAT_005b80e0[DAT_0058b584] == 0))
    {
      int next_search_index;
      int control_index;

      i = 0;
      if (DAT_0058b954 == -1)
      {
        next_search_index = 0;
      }
      else
      {
        next_search_index = DAT_0058b954 + 1;
      }

      for (; i < DAT_00746ef0[DAT_0058b584]; i = i + 1)
      {
        control_index = (next_search_index + i) % DAT_00746ef0[DAT_0058b584];
        if ((DAT_00746f40[DAT_0058b584][control_index]->state != 3) &&
            ((DAT_00746f40[DAT_0058b584][control_index]->direct_hotkey == full_key) ||
             ((queued_key != 0) && (DAT_00746f40[DAT_0058b584][control_index]->navigate_hotkeys != (char *)0) &&
              (strchr(DAT_00746f40[DAT_0058b584][control_index]->navigate_hotkeys, queued_key) != (char *)0))))
        {
          DAT_0058b954 = control_index;
          if (DAT_00746f40[DAT_0058b584][control_index]->x != -1)
          {
            SetCursorPos(
                DAT_00746f40[DAT_0058b584][control_index]->x + DAT_00746f40[DAT_0058b584][control_index]->width / 2,
                DAT_00746f40[DAT_0058b584][control_index]->y + DAT_00746f40[DAT_0058b584][control_index]->height / 2);
          }

          if (DAT_0058b958 != -1)
          {
            DAT_00746f40[DAT_0058b584][DAT_0058b958]->on_render(DAT_00746f40[DAT_0058b584][DAT_0058b958], 0);
          }

          DAT_00746f40[DAT_0058b584][DAT_0058b954]->on_render(DAT_00746f40[DAT_0058b584][DAT_0058b954], 1);
          if ((DAT_00746f40[DAT_0058b584][control_index]->direct_hotkey == full_key) ||
              ((queued_key != 0) && (DAT_00746f40[DAT_0058b584][control_index]->activate_hotkeys != (char *)0) &&
               (strchr(DAT_00746f40[DAT_0058b584][control_index]->activate_hotkeys, queued_key) != (char *)0)))
          {
            DAT_00747ee0 = 1;
            DAT_00746f40[DAT_0058b584][DAT_0058b954]->on_render(DAT_00746f40[DAT_0058b584][DAT_0058b954], 2);
            DAT_00747ee0 = 0;
            DAT_0058b958 = DAT_0058b954;
            PopQueuedKeyInput();
            DAT_005b80d8 = 0;
            return DAT_0058b954;
          }
          PopQueuedKeyInput();
          break;
        }
      }

      if ((queued_key == 0xd) && (DAT_0058b954 != -1))
      {
        if (DAT_0058b958 != -1)
        {
          DAT_00746f40[DAT_0058b584][DAT_0058b954]->on_render(DAT_00746f40[DAT_0058b584][DAT_0058b958], 0);
        }
        DAT_00746f40[DAT_0058b584][DAT_0058b954]->on_render(DAT_00746f40[DAT_0058b584][DAT_0058b954], 2);
        DAT_0058b958 = DAT_0058b954;
        PopQueuedKeyInput();
        DAT_005b80d8 = 0;
        return DAT_0058b954;
      }

      DAT_0058b958 = DAT_0058b954;
    }
  }

  PopQueuedKeyInput();
  if (DAT_0058b958 >= 0)
  {
    int in_bounds;

    if ((DAT_00986d9c < DAT_00746f40[DAT_0058b584][DAT_0058b958]->x) ||
        (DAT_00746f40[DAT_0058b584][DAT_0058b958]->x + DAT_00746f40[DAT_0058b584][DAT_0058b958]->width < DAT_00986d9c))
    {
      in_bounds = 0;
    }
    else if ((DAT_00986d98 < DAT_00746f40[DAT_0058b584][DAT_0058b958]->y) ||
             (DAT_00746f40[DAT_0058b584][DAT_0058b958]->height + DAT_00746f40[DAT_0058b584][DAT_0058b958]->y < DAT_00986d98))
    {
      in_bounds = 0;
    }
    else
    {
      in_bounds = 1;
    }

    if (in_bounds == 0)
    {
      DAT_00747ee0 = 1;
      DAT_00746f40[DAT_0058b584][DAT_0058b958]->on_render(DAT_00746f40[DAT_0058b584][DAT_0058b958], 0);
      DAT_0058b958 = -1;
      DAT_0058b954 = -1;
      DAT_00747ee0 = 0;
    }
  }

  for (i = 0; i < DAT_00746ef0[DAT_0058b584]; i = i + 1)
  {
    if (DAT_0058b958 != i)
    {
      if (DAT_00746f40[DAT_0058b584][i]->on_render(DAT_00746f40[DAT_0058b584][i], 0) != 0)
      {
        DAT_0058b954 = i;
      }
    }
  }

  if (DAT_0058b958 != DAT_0058b954)
  {
    if (DAT_0058b958 >= 0)
    {
      DAT_00747ee0 = 1;
      DAT_00746f40[DAT_0058b584][DAT_0058b958]->on_render(DAT_00746f40[DAT_0058b584][DAT_0058b958], 0);
      DAT_00747ee0 = 0;
    }

    DAT_00746f40[DAT_0058b584][DAT_0058b954]->on_render(DAT_00746f40[DAT_0058b584][DAT_0058b954], 1);
  }

  if ((param_3 == 1) && (DAT_0058b954 >= 0))
  {
    DAT_00746f40[DAT_0058b584][DAT_0058b954]->on_render(DAT_00746f40[DAT_0058b584][DAT_0058b954], 2);
  }

  DAT_0058b958 = DAT_0058b954;
  DAT_005b80d8 = 0;
  return DAT_0058b954;
}

// FUNCTION: SHANDALAR 0x004ed005
unsigned int FUN_004ed005(void)
{
  int iVar1;
  unsigned int local_8;

  if (DAT_008bd200 == 0)
  {
    do
    {
      FUN_0046eca4();
      if (DAT_007898f0 != 0)
      {
        break;
      }
      iVar1 = HasQueuedKeyInput();
    } while (iVar1 == 0);

    local_8 = DAT_007898f0;
    if (DAT_007898f0 == 0)
    {
      do
      {
        local_8 = PopQueuedKeyInput();
      } while (local_8 == 0);
    }

    FUN_004ecfa2();
  }
  else
  {
    local_8 = 0;
  }

  return local_8;
}

// FUNCTION: SHANDALAR 0x005597ca
unsigned int FUN_005597ca(void)
{
  unsigned int ret;

  if (DAT_008bd200 == 1)
  {
    ret = 0;
  }
  else if (unk_00742fc4 == 0)
  {
    ret = FUN_004ed005();
  }
  else
  {
    ret = 0;
  }

  return ret;
}

// FUNCTION: SHANDALAR 0x004ecf30
int FUN_004ecf30(int param_1, int param_2)
{
  int local_8;

  if (param_1 < 0)
  {
    param_1 = -param_1;
  }
  if (param_2 < 0)
  {
    param_2 = -param_2;
  }
  if (param_2 < param_1)
  {
    local_8 = param_1 * 2 + param_2;
  }
  else
  {
    local_8 = param_2 * 2 + param_1;
  }
  if (local_8 < 0)
  {
    local_8 = 0x7ffe;
  }
  return local_8;
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
int FUN_00414127(void)
{
  int queued_key;

  do
  {
    queued_key = PopQueuedKeyInput();
  } while (queued_key == 0);
  return queued_key;
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
                       DAT_0077d610 + sizeof(gs_city_text_cluster_0077d610.citynames_buf_0077d610), &s.local_8);
  s.ok &= FUN_00565c7e(filename, "CITYNAMES_SECONDHALF", gs_citynames_secondhalf_007653e0, 0x10, s.local_8,
                       DAT_0077d610 + sizeof(gs_city_text_cluster_0077d610.citynames_buf_0077d610), (char **)0);

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
  s.ok &= FUN_00565c7e(filename, "BROWSE", gs_browse_0074da20, 0x11,
                       DAT_0077d610 + sizeof(gs_city_text_cluster_0077d610.citynames_buf_0077d610),
                       DAT_0077d610 + sizeof(gs_city_text_cluster_0077d610.citynames_buf_0077d610) + sizeof(gs_city_text_cluster_0077d610.browse_buf_0077dc50),
                       (char **)0);
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

// FUNCTION: SHANDALAR 0x005626b0
void FUN_005626b0(char *param_1, int param_2, int param_3, int param_4, int param_5)
{
  Sound local_24;

  sound_unload(param_2);
  FUN_00562f92(param_1, param_2, 0);
  memset(&local_24, 0, sizeof(local_24));
  local_24.volume = param_3 << 2;
  local_24.sampleRate = (param_4 * 0x5622) / 100;
  local_24.pan = param_5 << 2;
  local_24.flags = local_24.flags & 0xffffffee;
  sound_play(param_2, &local_24);
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
    if ((i == 1) && (g_graphics_pages[0]->width < 0x401))
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

#ifdef _DEBUG
  // Annoying trying to debug with window always on top
  main_hwnd = CreateWindowExA(0, "ShandalarMainClass", "Magic: Shandalar", 0x80000000, 0, 0,
                              global_screen_width, global_screen_height, (HWND)0, (HMENU)0, hInstance, (LPVOID)0);
#else
  main_hwnd = CreateWindowExA(8, "ShandalarMainClass", "Magic: Shandalar", 0x80000000, 0, 0,
                              global_screen_width, global_screen_height, (HWND)0, (HMENU)0, hInstance, (LPVOID)0);
#endif
  DAT_00748420 = main_hwnd;
  ShowWindow(main_hwnd, nShowCmd);
  global_main_hdc = GetDC(main_hwnd);

  if (show_shell_on_exit)
  {
    g_graphics_pages[0]->hTempDC = global_main_hdc;
    SelectPalette(g_graphics_pages[0]->hTempDC, g_graphics_pages[0]->hPalette, FALSE);
    RealizePalette(g_graphics_pages[0]->hTempDC);
    SetStretchBltMode(g_graphics_pages[0]->hTempDC, 3);
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
