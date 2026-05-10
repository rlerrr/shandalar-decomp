#include <process.h>
#include <windows.h>
#include <direct.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <io.h>
#include <string.h>
#include <commdlg.h>
#include <ctype.h>
#include "cardartlib/src/assert.h"
#include "drawcardlib/src/pic.h"
#include "defs.h"
#include "facemaker_types.h"

#ifdef _fileno
#undef _fileno
#endif

typedef struct FaceControl FaceControl;

typedef void(__cdecl *FaceControlCallback)(FaceControl *control);
typedef int(__cdecl *FaceControlModeCallback)(FaceControl *control, int mode);

typedef struct FaceControl
{
  int x;
  int y;
  int width;
  int height;
  FaceControlModeCallback on_render;
  FaceControlCallback on_activate;
  int value_index;
  int value_delta;
  int state;
  EncodedImage *page_by_mode[4];
} FaceControl;

typedef struct RgbTriplet
{
  int red;
  int green;
  int blue;
  int unknown_0c;
} RgbTriplet;

typedef struct HsvTriplet
{
  int hue;
  int saturation;
  int value;
  int unknown_0c;
} HsvTriplet;

typedef union FaceSpriteEntryStorage
{
  EncodedImage *groups[10][20];
  EncodedImage *flat[0xd0];
} FaceSpriteEntryStorage;

typedef union FaceSpriteRootStorage
{
  EncodedImage *first;
  EncodedImage *flat[0x108];
} FaceSpriteRootStorage;

extern DIBSurface *FUN_00406810(int width, int height, int bits_per_pixel);
extern DIBSurface *FUN_004069e0(int page_number, int width, int height, int bits_per_pixel);
extern int FUN_004065e0();
extern int FUN_004065f0(char *executable_name, char *config_name);
extern DIBSurface *FUN_00406610(void);
extern DIBSurface *FUN_00406620(int page_number);
extern DIBSurface *FUN_00406650(void);
extern void FUN_00406bd0(int page_number, DIBSurface *page);
extern char s_D__NewMagic__sources__sidlib__lib_c_0040d0ec[];
extern int FUN_00407b10(char *path);
extern int FUN_00407c20(int font_id, FILE *file);
extern void FUN_00407210(int *dst, unsigned int dst_x, int dst_y, unsigned int width,
                         DWORD height, int *src, int src_x, int src_y);
extern void FUN_00407190(int *dst, int x, int y, int char_code);
extern void DrawEncodedImageUnscaled(int *dst, int x, int y, EncodedImage *encoded_image);
extern void DrawEncodedImageResampled(int *dst, int x, int y, int width, int height,
                                      EncodedImage *encoded_image);
extern void FUN_00409eb0(int page_number, int x, int y, char *path, void *opaque);
extern unsigned int FUN_00402600(int value);
extern void FUN_00402621(int *dst, int dst_x, int dst_y, int width, int height, int *src,
                         int src_x, int src_y);
extern void DrawEncodedImageAutoScale(int *dst, int x, int y, EncodedImage *encoded_image);
extern int FUN_00407e20(int font_id, unsigned int point_size, char *font_file, char *font_name,
                        int weight, DWORD italic);
extern void FUN_00406cc0(int enabled);
extern int FUN_004093d0(void);
extern void FUN_0040a110(int page_number, char *path);
extern size_t WriteSpriteBlob(void *sprite_blob, char *output_path);
extern int ReadSpriteEntryPointers(int *out_entry_ptrs, char *sprite_path);
extern int FUN_004086c0(int *dst, int text_id, int a3, int a4, int a5, int a6, int x, int y,
                        int *arg9);
extern void FUN_00407570(void *dst, int dst_x, int dst_y, int src_w, int src_h, void *src,
                         int src_x, int src_y, int copy_w, int copy_h);
extern int FUN_0040369c(void);
extern void FUN_00402717(int *page, int x, int y, EncodedImage *encoded_image);
extern void FUN_00402743(int *page, int x, int y, unsigned int width, int height);
extern int FUN_00402edc(int selected_face_index);
extern int LoadFaceSpriteSet(char *base_path, int *group_frame_counts, EncodedImage **group_entries,
                             EncodedImage **first_sprite_out);
extern int FUN_00406369(void);
extern void FUN_004063f0(WPARAM wparam, LPARAM lparam);
extern int FUN_00402a84(FaceControl *control);
extern int FUN_00402ad9(FaceControl *control);
extern void FUN_00402841(FaceControl *control);
extern int FUN_00402a42(FaceControl *control);
extern int FUN_00402b24(FaceControl *control);
extern int FUN_00402d30(FaceControl *control);
extern void FUN_004033c9(FaceControl *control);
extern void FUN_00403573(int index, int selected);
extern int FUN_004028d1(FaceControl *control, int mode);
extern void FUN_004032e4(void);
extern void BeginSpriteEncodeSession(void);
extern void FUN_0040a130(int page_number, char *path);
extern int FUN_00407f30(int font_id, unsigned int style);
extern void FUN_004088d0(int *dst, int text_id, int x, int y, char *text);
extern EncodedImage *EncodeSpriteFromPage(int page_number, int x, int y, unsigned int width,
                                          int height);
extern int FUN_00409420(int param_1, int param_2, int param_3, int param_4, int param_5, int param_6,
                        char *param_7);
extern void FinalizeSpriteEncodeSession(void);
extern void FUN_00408950(void *memory);
extern int FUN_00405f38(void);
extern int FUN_00405f60(void);
extern int FUN_00406590(void);
extern int FUN_004065a0(void);
extern int FUN_004093e0(void);
extern void FUN_004075d0(unsigned int *param_1, int param_2, int param_3, int param_4,
                         unsigned int param_5);
extern void FUN_004076c0(unsigned int *param_1, int param_2, int param_3, int param_4,
                         unsigned int param_5);
extern ATOM FUN_0040a330(HINSTANCE hInstance);
extern HWND FUN_0040a3c0(HINSTANCE hInstance, HWND parent_hwnd);
extern int FUN_0040a730(int enabled, int mode);
extern int FUN_00406d00(int page_number, int color_index);
extern void RpBits_ApplyPalette(short *palette_data_words);
extern void FUN_0040aa20(double *dst, double *src, unsigned int size);
extern int DAT_0040d088;
extern int DAT_00426570;
extern RGBQUAD DAT_00426580[256];
extern unsigned int DAT_00425e20[0xc8];
extern unsigned int DAT_00420df0[0xc8];
extern int DAT_00421110[0x400];
extern int DAT_0041f5f0[0x301];
extern int DAT_004201f4[0x2ff];
extern unsigned char *PTR_DAT_0040d2e4;
extern FontSlot DAT_004233d0[0x10];
extern char DAT_0040d224[];
extern __declspec(dllimport) int *__p___mb_cur_max(void);
extern __declspec(dllimport) unsigned short **__p__pctype(void);
extern __declspec(dllimport) int _isctype(int c, int type);

// GLOBAL: FACEMAKER 0x0040c010
FacemakerWindowBounds DAT_0040c010 = {0, 0, 0, 0x13f, 0xc7, 1, 0xf, 4, 0, &DAT_0040c010};

// GLOBAL: FACEMAKER 0x0040c038
FacemakerWindowBounds DAT_0040c038 = {1, 0, 0, 0x13f, 0xc7, 1, 0xf, 4, 0, &DAT_0040c038};

// GLOBAL: FACEMAKER 0x0040c060
FacemakerWindowBounds DAT_0040c060 = {2, 0, 0, 0x13f, 0xc7, 1, 0xf, 4, 0, &DAT_0040c060};

// GLOBAL: FACEMAKER 0x0040c088
FacemakerWindowBounds DAT_0040c088 = {2, 0, 0, 0x320, 0x258, 1, 0x9f, 4, 0, &DAT_0040c088};

// GLOBAL: FACEMAKER 0x0040c034
FacemakerWindowBounds *PTR_DAT_0040c034 = &DAT_0040c010;

// GLOBAL: FACEMAKER 0x0040c05c
FacemakerWindowBounds *PTR_DAT_0040c05c = &DAT_0040c038;

// GLOBAL: FACEMAKER 0x0040c0ac
FacemakerWindowBounds *PTR_DAT_0040c0ac = &DAT_0040c088;

// GLOBAL: FACEMAKER 0x0040c9e4
int DAT_0040c9e4 = 1;

// GLOBAL: FACEMAKER 0x0040c9e0
int DAT_0040c9e0 = 0x42;

// GLOBAL: FACEMAKER 0x0040c9e8
int DAT_0040c9e8 = 0;

// GLOBAL: FACEMAKER 0x0040c188
int DAT_0040c188 = 0;

// GLOBAL: FACEMAKER 0x0040c190
int DAT_0040c190 = 0x18b;

// GLOBAL: FACEMAKER 0x0040c194
int DAT_0040c194 = 0xd9;

// GLOBAL: FACEMAKER 0x0040c1c0
int DAT_0040c1c0 = 0xdd;

// GLOBAL: FACEMAKER 0x0040c1c4
int DAT_0040c1c4 = 0x8a;

// GLOBAL: FACEMAKER 0x0040c1e0
int DAT_0040c1e0 = 1000;

// GLOBAL: FACEMAKER 0x0040c1e4
int DAT_0040c1e4 = 0;

// GLOBAL: FACEMAKER 0x0040c1e8
FaceControl DAT_0040c1e8[0x15] = {
    {0, 0, 0x40, 0x20, FUN_004028d1, (FaceControlCallback)0, 0, 0, 0, {0, 0, 0, 0}},
    {0xce, 0x24, 0x56, 0x21, FUN_004028d1, FUN_004033c9, 0, 0, 0, {0, 0, 0, 0}},
    {0xce, 0x56, 0x56, 0x21, FUN_004028d1, FUN_00402841, 0, 0, 0, {0, 0, 0, 0}},
    {0xe0, 0x8a, 0x30, 0x21, FUN_004028d1, (FaceControlCallback)0, 0, 0, 0, {0, 0, 0, 0}},
    {0xd8, 0xbc, 0x40, 0x21, FUN_004028d1, (FaceControlCallback)0, 0, 0, 0, {0, 0, 0, 0}},
    {0xb9, 0x8a, 0x25, 0x1d, FUN_004028d1, (FaceControlCallback)FUN_00402a42, -1, 0, 0, {0, 0, 0, 0}},
    {0x111, 0x8a, 0x25, 0x1d, FUN_004028d1, (FaceControlCallback)FUN_00402a42, 1, 0, 0, {0, 0, 0, 0}},
    {0xb1, 0xbc, 0x25, 0x1d, FUN_004028d1, (FaceControlCallback)FUN_00402b24, -1, 0, 0, {0, 0, 0, 0}},
    {0x119, 0xbc, 0x25, 0x1d, FUN_004028d1, (FaceControlCallback)FUN_00402b24, 1, 0, 0, {0, 0, 0, 0}},
    {0x167, 0xd9, 0x25, 0x1d, FUN_004028d1, (FaceControlCallback)FUN_00402d30, 0, -1, 0, {0, 0, 0, 0}},
    {0x194, 0xd9, 0x25, 0x1d, FUN_004028d1, (FaceControlCallback)FUN_00402d30, 0, 1, 0, {0, 0, 0, 0}},
    {0x206, 0xd9, 0x25, 0x1d, FUN_004028d1, (FaceControlCallback)FUN_00402d30, 1, -1, 0, {0, 0, 0, 0}},
    {0x233, 0xd9, 0x25, 0x1d, FUN_004028d1, (FaceControlCallback)FUN_00402d30, 1, 1, 0, {0, 0, 0, 0}},
    {0x29, 0x1b8, 0x25, 0x1d, FUN_004028d1, (FaceControlCallback)FUN_00402d30, 2, -1, 0, {0, 0, 0, 0}},
    {0x56, 0x1b8, 0x25, 0x1d, FUN_004028d1, (FaceControlCallback)FUN_00402d30, 2, 1, 0, {0, 0, 0, 0}},
    {0xc8, 0x1b8, 0x25, 0x1d, FUN_004028d1, (FaceControlCallback)FUN_00402d30, 3, -1, 0, {0, 0, 0, 0}},
    {0xf5, 0x1b8, 0x25, 0x1d, FUN_004028d1, (FaceControlCallback)FUN_00402d30, 3, 1, 0, {0, 0, 0, 0}},
    {0x167, 0x1b8, 0x25, 0x1d, FUN_004028d1, (FaceControlCallback)FUN_00402d30, 4, -1, 0, {0, 0, 0, 0}},
    {0x194, 0x1b8, 0x25, 0x1d, FUN_004028d1, (FaceControlCallback)FUN_00402d30, 4, 1, 0, {0, 0, 0, 0}},
    {0x206, 0x1b8, 0x25, 0x1d, FUN_004028d1, (FaceControlCallback)FUN_00402d30, 5, -1, 0, {0, 0, 0, 0}},
    {0x233, 0x1b8, 0x25, 0x1d, FUN_004028d1, (FaceControlCallback)FUN_00402d30, 5, 1, 0, {0, 0, 0, 0}}};

// GLOBAL: FACEMAKER 0x0040c208
int DAT_0040c208 = 0;

// GLOBAL: FACEMAKER 0x0040c284
FaceControl DAT_0040c284 = {
    0xe0, 0x8a, 0x30, 0x21, FUN_004028d1, (FaceControlCallback)0, 0, 0, 0, {0, 0, 0, 0}};

// GLOBAL: FACEMAKER 0x0040c2b8
FaceControl DAT_0040c2b8 = {
    0xd8, 0xbc, 0x40, 0x21, FUN_004028d1, (FaceControlCallback)0, 0, 0, 0, {0, 0, 0, 0}};

// GLOBAL: FACEMAKER 0x0040c354
FaceControl DAT_0040c354 = {
    0xb1, 0xbc, 0x25, 0x1d, FUN_004028d1, (FaceControlCallback)FUN_00402b24, -1, 0, 0, {0, 0, 0, 0}};

// GLOBAL: FACEMAKER 0x0040c388
FaceControl DAT_0040c388 = {
    0x119, 0xbc, 0x25, 0x1d, FUN_004028d1, (FaceControlCallback)FUN_00402b24, 1, 0, 0, {0, 0, 0, 0}};

// GLOBAL: FACEMAKER 0x0041afb4
HDC global_main_hdc;

// GLOBAL: FACEMAKER 0x0041ae40
char DAT_0041ae40[0x105];

// GLOBAL: FACEMAKER 0x0041b024
char DAT_0041b024[4];

// GLOBAL: FACEMAKER 0x0041af50
char DAT_0041af50[0x64];

// GLOBAL: FACEMAKER 0x0041afc0
char DAT_0041afc0[0x64];

// GLOBAL: FACEMAKER 0x0041b028
HWND DAT_0041b028;

// GLOBAL: FACEMAKER 0x0041b030
char DAT_0041b030[0x105];

// GLOBAL: FACEMAKER 0x0041c140
HINSTANCE DAT_0041c140;

// GLOBAL: FACEMAKER 0x0041c144
int DAT_0041c144;

// GLOBAL: FACEMAKER 0x00412488
HWND DAT_00412488;

// GLOBAL: FACEMAKER 0x0041248c
int DAT_0041248c;

// GLOBAL: FACEMAKER 0x00412490
int DAT_00412490;

// GLOBAL: FACEMAKER 0x004128a0
unsigned int DAT_004128a0[0x32];

// GLOBAL: FACEMAKER 0x00412968
int DAT_00412968;

// GLOBAL: FACEMAKER 0x0041c150
char *DAT_0041c150[1000];

// GLOBAL: FACEMAKER 0x0041d110
int global_screen_width;

// GLOBAL: FACEMAKER 0x0041d114
int global_screen_height;

// GLOBAL: FACEMAKER 0x0041d120
char *DAT_0041d120[1000];

// GLOBAL: FACEMAKER 0x0041d0f0
FaceSpriteRootStorage g_face_sprite_roots;

// GLOBAL: FACEMAKER 0x0041e3f0
EncodedImage *g_face_control_even_sprites[4];

// GLOBAL: FACEMAKER 0x0041e400
EncodedImage *g_face_control_odd_sprites[4];

// GLOBAL: FACEMAKER 0x0041e410
int g_face_group_count;

// GLOBAL: FACEMAKER 0x0041e420
char DAT_0041e420[0x80];

// GLOBAL: FACEMAKER 0x0041e4a0
EncodedImage *g_face_scroll_arrow_sprite;

// GLOBAL: FACEMAKER 0x0041e4a4
EncodedImage *g_face_scroll_divider_sprite;

// GLOBAL: FACEMAKER 0x0041e4b0
int g_face_variant_index[10];

// GLOBAL: FACEMAKER 0x0041e4dc
EncodedImage *g_face_slot_background_sprite;

// GLOBAL: FACEMAKER 0x0041e4e0
EncodedImage *g_face_control1_sprites[4];

// GLOBAL: FACEMAKER 0x0041e4f0
int DAT_0041e4f0;

// GLOBAL: FACEMAKER 0x0041e500
EncodedImage *g_face_control0_sprites[3];

// GLOBAL: FACEMAKER 0x0041e50c
int DAT_0041e50c;

// GLOBAL: FACEMAKER 0x0041e510
EncodedImage *g_face_control2_sprites[4];

// GLOBAL: FACEMAKER 0x0041e520
EncodedImage *g_face_control4_sprites[2];

// GLOBAL: FACEMAKER 0x0041e530
int g_face_group_frame_counts[10];

// GLOBAL: FACEMAKER 0x0041e558
EncodedImage *g_face_control3_sprites[2];

// GLOBAL: FACEMAKER 0x0041e560
int DAT_0041e560;

// GLOBAL: FACEMAKER 0x0041e570
char *DAT_0041e570[1000];

// GLOBAL: FACEMAKER 0x0041e0c0
int DAT_0041e0c0;

// GLOBAL: FACEMAKER 0x0041e0d0
FaceSpriteEntryStorage g_face_sprite_entries;

// GLOBAL: FACEMAKER 0x0041e564
int DAT_0041e564;

extern DIBSurface *DAT_00426540[10];
extern int DAT_00426568;
extern int DAT_0042656c;

// GLOBAL: FACEMAKER 0x00423338
int DAT_00423338;

// GLOBAL: FACEMAKER 0x0042333c
int DAT_0042333c;

// GLOBAL: FACEMAKER 0x00423334
int DAT_00423334;

// GLOBAL: FACEMAKER 0x00423340
int DAT_00423340;

// GLOBAL: FACEMAKER 0x0040c62c
char DAT_0040c62c[] = "wt";

// GLOBAL: FACEMAKER 0x0040c630
char s_FaceMostRecent_txt_0040c630[] = "FaceMostRecent.txt";

// GLOBAL: FACEMAKER 0x0040c644
char s__d___s_0040c644[] = "%d, %s";

// GLOBAL: FACEMAKER 0x0040c64c
char s_FaceArt__0040c64c[] = "FaceArt/";

// GLOBAL: FACEMAKER 0x0040c658
char DAT_0040c658[] = "rt";

// GLOBAL: FACEMAKER 0x0040c65c
char s_FaceButtons_txt_0040c65c[] = "FaceButtons.txt";

// GLOBAL: FACEMAKER 0x0040c66c
char s________c_0040c66c[] = "%[^\n]%*c";

// GLOBAL: FACEMAKER 0x0040c678
char s________c_0040c678[] = "%[^\n]%*c";

// GLOBAL: FACEMAKER 0x0040c684
char s________c_0040c684[] = "%[^\n]%*c";

// GLOBAL: FACEMAKER 0x0040c690
char s__d__d__d__d__d_0040c690[] = "%d %d,%d,%d,%d";

// GLOBAL: FACEMAKER 0x0040c6a0
char s_FaceArt_facebut1_pcx_0040c6a0[] = "FaceArt/facebut1.pcx";

// GLOBAL: FACEMAKER 0x0040c6b8
char s_FaceArt_facfrsh2New_pcx_0040c6b8[] = "FaceArt/facfrsh2New.pcx";

// GLOBAL: FACEMAKER 0x0040c6d0
char DAT_0040c6d0[] = "rt";

// GLOBAL: FACEMAKER 0x0040c6d4
char s_FaceData_txt_0040c6d4[] = "FaceData.txt";

// GLOBAL: FACEMAKER 0x0040caa0
char s_FaceArt_evil_0040caa0[] = "FaceArt\\evil";

// GLOBAL: FACEMAKER 0x0040cad4
char s_PlayFace_0040cad4[] = "PlayFace";

// GLOBAL: FACEMAKER 0x0040cae0
char s___pic_0040cae0[] = "*.pic";

// GLOBAL: FACEMAKER 0x0040cae8
char s___pic_0040cae8[] = "*.pic";

// GLOBAL: FACEMAKER 0x0040caf0
unsigned char DAT_0040caf0[0x1000] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    2, 0, 27, 1, 2, 0, 27, 1, 2, 0, 27, 1, 0, 0, 0, 0,
    2, 0, 49, 2, 2, 0, 33, 2, 0, 0, 0, 0, 2, 0, 0, 120,
    2, 0, 50, 3, 2, 0, 64, 3, 0, 0, 0, 0, 2, 0, 0, 121};

// GLOBAL: FACEMAKER 0x0040cab0
char s_Current_Palette_0040cab0[] = "Current Palette";

// GLOBAL: FACEMAKER 0x0040cac0
char s_ShowPaletteClass_0040cac0[] = "ShowPaletteClass";

// GLOBAL: FACEMAKER 0x0040d084
int DAT_0040d084 = 0;

// GLOBAL: FACEMAKER 0x00412088
char DAT_00412088[0x400];

// FUNCTION: FACEMAKER 0x00406253
void FUN_00406253(HWND owner_hwnd, OPENFILENAMEA *open_file, char *initial_file, char *pattern,
                  char *file_buffer)
{
  memset(DAT_00412088, 0, 0x400);
  strcpy(DAT_00412088, initial_file);
  strcpy(DAT_00412088 + strlen(DAT_00412088) + 1, pattern);
  open_file->lStructSize = 0x4c;
  open_file->hwndOwner = owner_hwnd;
  open_file->hInstance = (HINSTANCE)0;
  open_file->lpstrFilter = DAT_00412088;
  open_file->lpstrCustomFilter = (LPSTR)0;
  open_file->nMaxCustFilter = 0;
  open_file->nFilterIndex = 0;
  open_file->lpstrFile = file_buffer;
  open_file->nMaxFile = 0x104;
  open_file->lpstrFileTitle = (LPSTR)0;
  open_file->nMaxFileTitle = 0x200;
  open_file->lpstrInitialDir = s_PlayFace_0040cad4;
  open_file->lpstrTitle = DAT_0041afc0;
  open_file->Flags = 0x6000c;
  open_file->nFileOffset = 0;
  open_file->nFileExtension = 0;
  open_file->lpstrDefExt = pattern + 2;
  open_file->lCustData = 0;
  open_file->lpfnHook = (LPOFNHOOKPROC)0;
  open_file->lpTemplateName = (LPCSTR)0;
}

// FUNCTION: FACEMAKER 0x00406369
int FUN_00406369(void)
{
  struct
  {
    OPENFILENAMEA open_file;
    int save_result;
  } s;

  strcpy(DAT_0041b030, s___pic_0040cae0);
  FUN_00406253(DAT_0041b028, &s.open_file, DAT_0041af50, s___pic_0040cae8, DAT_0041b030);
  s.save_result = GetSaveFileNameA(&s.open_file);
  DAT_00423334 = 0;
  if (s.save_result != 0)
    return 1;
  else
    return 0;
}

#pragma optimize("gy", on)

// FUNCTION: FACEMAKER 0x004063f0
void FUN_004063f0(WPARAM wparam, LPARAM lparam)
{
  (void)wparam;
  {
    int *mb_cur_max;
    unsigned short **pctype_table;
    int virtual_key;
    int modifier_mode;
    int repeat_count;
    int ctype_mask;
    int table_offset;
    int tmp_state;
    unsigned short translated_char;

    modifier_mode = 0;
    tmp_state = DAT_00412968;
    if (tmp_state == 0)
    {
      while (GetAsyncKeyState(0x12) != 0)
      {
      }
      while (GetAsyncKeyState(0x11) != 0)
      {
      }
      DAT_00412968 = 1;
    }

    tmp_state = DAT_0040d084;
    if (tmp_state == 0x31)
    {
      MessageBeep(-1);
      return;
    }

    virtual_key = (int)(((unsigned int)lparam & 0xff0000) >> 0x10);
    if (GetAsyncKeyState(0x12) != 0)
    {
      modifier_mode = 3;
    }
    else if (GetAsyncKeyState(0x11) != 0)
    {
      modifier_mode = 2;
    }
    else
    {
      mb_cur_max = __p___mb_cur_max();
      if (*mb_cur_max > 1)
      {
        ctype_mask = _isctype(*(unsigned short *)(DAT_0040caf0 + virtual_key * 0x10 + 2) & 0xff, 0x103);
      }
      else
      {
        repeat_count = (int)(unsigned int)*(unsigned short *)(DAT_0040caf0 + virtual_key * 0x10 + 2);
        pctype_table = __p__pctype();
        ctype_mask = (int)((*pctype_table)[repeat_count & 0xff] & 0x103);
      }

      if (ctype_mask != 0)
      {
        modifier_mode = (GetAsyncKeyState(0x14) == 1);
        modifier_mode = modifier_mode ^ (GetAsyncKeyState(0x10) == 1);
      }
      else if ((virtual_key < 0x47) || (0x53 < virtual_key))
      {
        if (GetAsyncKeyState(0x10) != 0)
        {
          modifier_mode = 1;
        }
      }
      else
      {
        modifier_mode = (GetAsyncKeyState(0x90) == 1);
        modifier_mode = modifier_mode ^ (GetAsyncKeyState(0x10) == 1);
      }
    }

    table_offset = (int)(modifier_mode + virtual_key * 4) * 4;
    if (DAT_0040caf0[table_offset] != '\0')
    {
      translated_char = *(unsigned short *)(DAT_0040caf0 + table_offset + 2);
      repeat_count = (int)((unsigned int)lparam & 0xffff);
      if ((int)repeat_count >= (0x32 - DAT_0040d084))
      {
        repeat_count = (0x32 - DAT_0040d084);
      }

      if (repeat_count != 0)
      {
        unsigned int *dst_char;

        dst_char = DAT_004128a0 + DAT_0040d084;
        do
        {
          repeat_count = repeat_count - 1;
          *dst_char = (unsigned int)translated_char;
          dst_char = dst_char + 1;
          DAT_0040d084 = DAT_0040d084 + 1;
        } while (repeat_count != 0);
      }
    }
  }
}

// FUNCTION: FACEMAKER 0x0040a330
ATOM FUN_0040a330(HINSTANCE hInstance)
{
  WNDCLASSA wndclass;

  wndclass.style = 0x20;
  wndclass.lpfnWndProc = DefWindowProcA;
  wndclass.cbClsExtra = 0;
  wndclass.cbWndExtra = 0;
  wndclass.hInstance = hInstance;
  wndclass.hIcon = LoadIconA((HINSTANCE)0, (LPCSTR)0x7f00);
  wndclass.hCursor = LoadCursorA((HINSTANCE)0, (LPCSTR)0x7f00);
  wndclass.hbrBackground = CreateSolidBrush(0);
  wndclass.lpszMenuName = (LPCSTR)0;
  wndclass.lpszClassName = s_ShowPaletteClass_0040cac0;
  return RegisterClassA(&wndclass);
}

// FUNCTION: FACEMAKER 0x0040a3c0
HWND FUN_0040a3c0(HINSTANCE hInstance, HWND parent_hwnd)
{
  return CreateWindowExA(0, s_ShowPaletteClass_0040cac0, s_Current_Palette_0040cab0, 0x80c80000,
                         100, 0x32, 0x100, 0x100, parent_hwnd, (HMENU)0, hInstance, (LPVOID)0);
}

// FUNCTION: FACEMAKER 0x0040a440
int *FUN_0040a440(int *dst, int *src)
{
  int green;
  int blue;
  int max_component;
  int min_component;
  int saturation;
  int hue;

  green = src[1];
  blue = src[2];
  max_component = green;
  if (green <= blue)
  {
    max_component = blue;
  }
  if (max_component <= src[0])
  {
    max_component = src[0];
  }

  min_component = green;
  if (blue <= green)
  {
    min_component = blue;
  }
  if (src[0] <= min_component)
  {
    min_component = src[0];
  }

  if (max_component != 0)
  {
    min_component = max_component - min_component;
    saturation = (min_component * 0x1000) / max_component;
    if (min_component == 0)
    {
      dst[0] = 0;
      dst[1] = saturation;
      dst[2] = max_component << 6;
      return dst;
    }

    if (max_component == src[0])
    {
      hue = ((green - blue) * 0xf00) / min_component;
    }
    else if (max_component == green)
    {
      hue = ((blue - src[0]) * 0xf00) / min_component + 0x1e00;
    }
    else
    {
      hue = ((src[0] - green) * 0xf00) / min_component + 0x3c00;
    }

    if (hue < 0)
    {
      hue = hue + 0x5a00;
    }
    dst[0] = hue;
    dst[1] = saturation;
    dst[2] = max_component << 6;
    return dst;
  }

  dst[0] = -1;
  dst[1] = 0;
  dst[2] = 0;
  return dst;
}

// FUNCTION: FACEMAKER 0x0040a5a0
unsigned int *FUN_0040a5a0(unsigned int *dst, int *src)
{
  int saturation;
  unsigned int value_8bit;
  int low_saturation_value;
  unsigned int base_component;
  unsigned int rising_component;
  unsigned int falling_component;
  unsigned int rgb[4];
  int section;

  saturation = src[1];
  if ((saturation == 0) && (src[0] == -1))
  {
    value_8bit = (unsigned int)((src[2] + ((src[2] >> 0x1f) & 0x3f)) >> 6);
    rgb[0] = value_8bit;
    rgb[1] = value_8bit;
    rgb[2] = value_8bit;
    dst[0] = rgb[0];
    dst[1] = rgb[1];
    dst[2] = rgb[2];
    dst[3] = rgb[3];
    return dst;
  }

  if (src[0] == 0x5a00)
  {
    src[0] = 0;
  }

  value_8bit = (unsigned int)((src[2] + ((src[2] >> 0x1f) & 0x3f)) >> 6);
  low_saturation_value = (0x1000 - saturation) * (int)value_8bit;
  base_component = (unsigned int)((low_saturation_value + ((low_saturation_value >> 0x1f) & 0xfff)) >> 0xc);
  section = src[0] % 0xf00;
  rising_component = (unsigned int)(((0xf00000 - saturation * section) * (int)value_8bit) / 0xf00000);
  falling_component =
      (unsigned int)((((section - 0xf00) * saturation + 0xf00000) * (int)value_8bit) / 0xf00000);

  switch (src[0] / 0xf00)
  {
  case 0:
    rgb[0] = value_8bit;
    rgb[1] = falling_component;
    rgb[2] = base_component;
    break;

  case 1:
    rgb[0] = rising_component;
    rgb[1] = value_8bit;
    rgb[2] = base_component;
    break;

  case 2:
    rgb[0] = base_component;
    rgb[1] = value_8bit;
    rgb[2] = falling_component;
    break;

  case 3:
    rgb[0] = base_component;
    rgb[1] = rising_component;
    rgb[2] = value_8bit;
    break;

  case 4:
    rgb[0] = falling_component;
    rgb[1] = base_component;
    rgb[2] = value_8bit;
    break;

  case 5:
    rgb[0] = value_8bit;
    rgb[1] = base_component;
    rgb[2] = rising_component;
    break;
  }

  dst[0] = rgb[0];
  dst[1] = rgb[1];
  dst[2] = rgb[2];
  dst[3] = rgb[3];
  return dst;
}

// FUNCTION: FACEMAKER 0x0040a730
int FUN_0040a730(int param_1, int param_2)
{
  unsigned char *puVar1;
  int iVar2;
  int *piVar3;
  unsigned char *pbVar4;
  int iVar5;
  int iVar7;
  int iVar8;
  short sVar9;
  int *piVar10;
  int iVar11;
  short local_3e;
  int local_38;
  int local_34;
  int local_30;
  int local_2c;
  int local_28;
  int local_24;
  int local_1c;
  int local_18;
  int local_14;
  unsigned int local_10[4];

  iVar2 = 0x4000 / param_2;
  if (DAT_00426570 != 8)
  {
    return 0;
  }

  piVar3 = (int *)DAT_00425e20;
  piVar10 = (int *)DAT_00420df0;
  for (iVar7 = 0xc0; iVar7 != 0; iVar7 = iVar7 - 1)
  {
    *piVar10 = *piVar3;
    piVar3 = piVar3 + 1;
    piVar10 = piVar10 + 1;
  }

  local_2c = param_1;
  local_28 = param_1;
  local_24 = param_1;
  piVar3 = FUN_0040a440((int *)local_10, &local_2c);
  local_1c = *piVar3;
  local_18 = piVar3[1];
  local_14 = piVar3[2];
  sVar9 = 0;
  RpBits_ApplyPalette((short *)DAT_00425e20);
  iVar7 = local_18;

  do
  {
    iVar11 = (int)sVar9;
    iVar8 = iVar11 * 4;
    pbVar4 = PTR_DAT_0040d2e4 + iVar11 * 3;
    DAT_00421110[iVar8] = (unsigned int)*pbVar4;
    DAT_00421110[iVar8 + 1] = (unsigned int)pbVar4[1];
    DAT_00421110[iVar8 + 2] = (unsigned int)pbVar4[2];
    piVar3 = FUN_0040a440((int *)local_10, DAT_00421110 + iVar8);
    DAT_0041f5f0[iVar11 * 3] = *piVar3;
    DAT_0041f5f0[iVar11 * 3 + 1] = piVar3[1];
    DAT_0041f5f0[iVar11 * 3 + 2] = piVar3[2];
    iVar8 = (iVar7 - DAT_0041f5f0[iVar11 * 3 + 1]) / param_2;
    DAT_004201f4[iVar11 * 3] = iVar8;
    if (DAT_0041f5f0[iVar11 * 3 + 1] < iVar7)
    {
      iVar5 = 0x1000;
    }
    else
    {
      iVar5 = -0x1000;
    }
    sVar9 = sVar9 + 1;
    DAT_004201f4[iVar11 * 3] = iVar5 / param_2 + iVar8;
  } while (sVar9 < 0x100);

  local_3e = 1;
  if (0 < param_2)
  {
    do
    {
      sVar9 = 0;
      do
      {
        iVar7 = (int)sVar9;
        if (local_14 == 0)
        {
          local_38 = DAT_0041f5f0[iVar7 * 3];
          local_34 = DAT_0041f5f0[iVar7 * 3 + 1];
          local_30 = DAT_0041f5f0[iVar7 * 3 + 2] - iVar2;
          DAT_0041f5f0[iVar7 * 3 + 2] = local_30;
        }
        else
        {
          local_38 = DAT_0041f5f0[iVar7 * 3];
          local_34 = DAT_004201f4[iVar7 * 3] * (int)local_3e + DAT_0041f5f0[iVar7 * 3 + 1];
          if (0xfbf < local_34)
          {
            local_34 = 0xfc0;
          }
          if (local_34 < 1)
          {
            local_34 = 0;
          }

          iVar8 = iVar2;
          if (local_14 < DAT_0041f5f0[iVar7 * 3 + 2])
          {
            iVar8 = -iVar2;
          }
          DAT_0041f5f0[iVar7 * 3 + 2] = DAT_0041f5f0[iVar7 * 3 + 2] + iVar8;
          local_30 = DAT_0041f5f0[iVar7 * 3 + 2];
          if (0x3fbf < local_30)
          {
            local_30 = 0x3fc0;
          }
        }

        if (local_30 < 1)
        {
          local_30 = 0;
        }

        iVar11 = (int)sVar9;
        sVar9 = sVar9 + 1;
        iVar8 = iVar11 * 4;
        piVar3 = (int *)FUN_0040a5a0(local_10, &local_38);
        DAT_00421110[iVar8] = *piVar3;
        DAT_00421110[iVar8 + 1] = piVar3[1];
        iVar7 = iVar11 * 3;
        DAT_00421110[iVar8 + 2] = piVar3[2];
        puVar1 = PTR_DAT_0040d2e4;
        DAT_00421110[iVar8 + 3] = piVar3[3];
        puVar1[iVar7] = (unsigned char)DAT_00421110[iVar8];
        PTR_DAT_0040d2e4[iVar7 + 1] = (unsigned char)DAT_00421110[iVar8 + 1];
        PTR_DAT_0040d2e4[iVar7 + 2] = (unsigned char)DAT_00421110[iVar8 + 2];
      } while (sVar9 < 0x100);
      RpBits_ApplyPalette((short *)DAT_00425e20);
      local_3e = local_3e + 1;
    } while (local_3e <= param_2);
  }

  sVar9 = 0;
  do
  {
    iVar2 = (int)sVar9;
    sVar9 = sVar9 + 1;
    iVar2 = iVar2 * 3;
    PTR_DAT_0040d2e4[iVar2] = (unsigned char)local_2c;
    PTR_DAT_0040d2e4[iVar2 + 1] = (unsigned char)local_28;
    PTR_DAT_0040d2e4[iVar2 + 2] = (unsigned char)local_24;
  } while (sVar9 < 0x100);

  RpBits_ApplyPalette((short *)DAT_00425e20);
  return FUN_00406d00(0, 0);
}

// FUNCTION: FACEMAKER 0x004065f0
int FUN_004065f0(char *executable_name, char *config_name)
{
  (void)executable_name;
  if (config_name != (char *)0)
  {
    return FUN_00407b10(config_name);
  }
  return 0;
}

#pragma optimize("gty", on)
#pragma intrinsic(memcpy)

// FUNCTION: FACEMAKER 0x00406590
int FUN_00406590(void)
{
  return DAT_0040d084 != 0;
}

// FUNCTION: FACEMAKER 0x004065a0
int FUN_004065a0(void)
{
  int queue_value;
  int queue_index;

  if (DAT_0040d084 == 0)
  {
    return 0;
  }

  queue_value = DAT_004128a0[0];
  DAT_0040d084 = DAT_0040d084 - 1;
  if (DAT_0040d084 != 0)
  {
    queue_index = DAT_0040d084;
    memcpy(DAT_004128a0, DAT_004128a0 + 1, queue_index * 4);
  }
  return queue_value;
}

#pragma optimize("gy", on)

// FUNCTION: FACEMAKER 0x004065e0
int FUN_004065e0()
{
  return 0;
}

// FUNCTION: FACEMAKER 0x00406610
DIBSurface *FUN_00406610(void)
{
  return FUN_00406650();
}

// FUNCTION: FACEMAKER 0x00406620
DIBSurface *FUN_00406620(int page_number)
{
  if (page_number == 0)
  {
    return FUN_00406650();
  }
  return FUN_004069e0(page_number, DAT_0042656c, DAT_00426568, 8);
}

#pragma optimize("", off)

// FUNCTION: FACEMAKER 0x00402600
unsigned int FUN_00402600(int value)
{
  return (unsigned int)((global_screen_width * value) / 0x280);
}

// FUNCTION: FACEMAKER 0x00402621
void FUN_00402621(int *dst, int dst_x, int dst_y, int width, int height, int *src, int src_x,
                  int src_y)
{
  FUN_00407210(dst, FUN_00402600(dst_x), FUN_00402600(dst_y), FUN_00402600(width),
               FUN_00402600(height), src, FUN_00402600(src_x), FUN_00402600(src_y));
}

// FUNCTION: FACEMAKER 0x0040268a
void DrawEncodedImageAutoScale(int *dst, int x, int y, EncodedImage *encoded_image)
{
  EncodedImage *local_encoded_image;

  local_encoded_image = encoded_image;
  if (global_screen_width == 640)
  {
    DrawEncodedImageUnscaled(dst, x, y, encoded_image);
  }
  else
  {
    DrawEncodedImageResampled(dst, FUN_00402600(x), FUN_00402600(y),
                              FUN_00402600((int)local_encoded_image->width),
                              FUN_00402600((int)local_encoded_image->height), encoded_image);
  }
}

// FUNCTION: FACEMAKER 0x004093d0
#pragma optimize("gty", on)
int FUN_004093d0(void)
{
  return -1;
}

// FUNCTION: FACEMAKER 0x004093e0
int FUN_004093e0(void)
{
  return -1;
}
#pragma optimize("", on)

// FUNCTION: FACEMAKER 0x0040369c
int FUN_0040369c(void)
{
  struct
  {
    char char_temp;
    char pad_001[3];
    char *scan_ptr;
    int tmp_y;
    int tmp_x;
    char pad_010[0x100];
    FILE *file;
    int style_values[6];
    int loop_index;
    int face_slot_x;
    int control_slot;
    int button_text_ids[5];
  } local;

  local.control_slot = 0;
  local.button_text_ids[1] = 0xce;
  local.button_text_ids[2] = 0x3b;
  local.button_text_ids[3] = 0x3b;
  local.button_text_ids[4] = 0x3b;
  local.style_values[0] = 10;
  local.style_values[1] = 10;
  local.style_values[2] = 8;
  local.style_values[3] = 10;
  local.button_text_ids[0] = 2;

  local.file = fopen(s_FaceButtons_txt_0040c65c, DAT_0040c658);
  fscanf(local.file, s________c_0040c66c, DAT_0041afc0);
  fscanf(local.file, s________c_0040c678, DAT_0041af50);
  for (local.loop_index = 0; local.loop_index < 4; local.loop_index = local.loop_index + 1)
  {
    fscanf(local.file, s________c_0040c684, DAT_0041e420 + local.loop_index * 0x20);
  }

  fscanf(local.file, s__d__d__d__d__d_0040c690, local.button_text_ids, local.style_values,
         local.style_values + 1, local.style_values + 2, local.style_values + 3);
  fclose(local.file);

  BeginSpriteEncodeSession();
  FUN_0040a130(2, s_FaceArt_facebut1_pcx_0040c6a0);
  *(int *)((char *)PTR_DAT_0040c0ac + 0x20) = local.button_text_ids[0];

  for (local.loop_index = 0; local.loop_index < 4;
       local.loop_index = local.loop_index + 1, local.control_slot = local.control_slot + 1)
  {
    local.face_slot_x = (local.control_slot % 5) * 0x70;
    local.style_values[5] = (local.control_slot / 5) * 0x60;
    FUN_00407f30(local.button_text_ids[0], local.style_values[local.loop_index]);
    FUN_004088d0((int *)PTR_DAT_0040c0ac, local.button_text_ids[local.loop_index + 1],
                 local.face_slot_x + 0x2b, local.style_values[5] + 0xe, DAT_0041e420 + 0x20);
    g_face_control1_sprites[local.loop_index] =
        EncodeSpriteFromPage(2, local.face_slot_x, local.style_values[5], 0x56, 0x21);
    DAT_0040c1e8[1].page_by_mode[local.loop_index] = g_face_control1_sprites[local.loop_index];
  }

  for (local.loop_index = 0; local.loop_index < 4;
       local.loop_index = local.loop_index + 1, local.control_slot = local.control_slot + 1)
  {
    local.face_slot_x = (local.control_slot % 5) * 0x70;
    local.style_values[5] = (local.control_slot / 5) * 0x60;
    FUN_00407f30(local.button_text_ids[0], local.style_values[local.loop_index]);
    FUN_004088d0((int *)PTR_DAT_0040c0ac, local.button_text_ids[local.loop_index + 1],
                 local.face_slot_x + 0x2b, local.style_values[5] + 0xe, DAT_0041e420);
    g_face_control2_sprites[local.loop_index] =
        EncodeSpriteFromPage(2, local.face_slot_x, local.style_values[5], 0x56, 0x21);
    DAT_0040c1e8[2].page_by_mode[local.loop_index] = g_face_control2_sprites[local.loop_index];
  }

  for (local.loop_index = 0; local.loop_index < 4;
       local.loop_index = local.loop_index + 1, local.control_slot = local.control_slot + 1)
  {
    local.face_slot_x = (local.control_slot % 5) * 0x70;
    local.style_values[5] = (local.control_slot / 5) * 0x60;
    g_face_control_odd_sprites[local.loop_index] =
        EncodeSpriteFromPage(2, local.face_slot_x, local.style_values[5], 0x25, 0x1d);
    for (local.style_values[4] = 5; local.style_values[4] < 0x14;
         local.style_values[4] = local.style_values[4] + 2)
    {
      DAT_0040c1e8[local.style_values[4]].page_by_mode[local.loop_index] =
          g_face_control_odd_sprites[local.loop_index];
    }
  }

  for (local.loop_index = 0; local.loop_index < 4;
       local.loop_index = local.loop_index + 1, local.control_slot = local.control_slot + 1)
  {
    local.face_slot_x = (local.control_slot % 5) * 0x70;
    local.style_values[5] = (local.control_slot / 5) * 0x60;
    g_face_control_even_sprites[local.loop_index] =
        EncodeSpriteFromPage(2, local.face_slot_x, local.style_values[5], 0x25, 0x1d);
    for (local.style_values[4] = 6; local.style_values[4] < 0x16;
         local.style_values[4] = local.style_values[4] + 2)
    {
      DAT_0040c1e8[local.style_values[4]].page_by_mode[local.loop_index] =
          g_face_control_even_sprites[local.loop_index];
    }
  }

  for (local.loop_index = 0; local.loop_index < 3;
       local.loop_index = local.loop_index + 1, local.control_slot = local.control_slot + 1)
  {
    local.face_slot_x = (local.control_slot % 5) * 0x70;
    local.style_values[5] = (local.control_slot / 5) * 0x60;
    g_face_control0_sprites[local.loop_index] =
        EncodeSpriteFromPage(2, local.face_slot_x, local.style_values[5], 0x40, 0x20);
    DAT_0040c1e8[0].page_by_mode[local.loop_index] = g_face_control0_sprites[local.loop_index];
  }

  for (local.loop_index = 0; local.loop_index < 2;
       local.loop_index = local.loop_index + 1, local.control_slot = local.control_slot + 1)
  {
    local.face_slot_x = (local.control_slot % 5) * 0x70;
    local.style_values[5] = (local.control_slot / 5) * 0x60;
    FUN_00407f30(local.button_text_ids[0], local.style_values[local.loop_index]);
    FUN_004088d0((int *)PTR_DAT_0040c0ac, local.button_text_ids[local.loop_index + 1],
                 local.face_slot_x + 0x20, local.style_values[5] + 0xe, DAT_0041e420 + 0x40);
    g_face_control3_sprites[local.loop_index] =
        EncodeSpriteFromPage(2, local.face_slot_x, local.style_values[5], 0x40, 0x1d);
    DAT_0040c1e8[3].page_by_mode[local.loop_index] = g_face_control3_sprites[local.loop_index];
  }

  for (local.loop_index = 0; local.loop_index < 2;
       local.loop_index = local.loop_index + 1, local.control_slot = local.control_slot + 1)
  {
    local.face_slot_x = (local.control_slot % 5) * 0x70;
    local.style_values[5] = (local.control_slot / 5) * 0x60;
    FUN_00407f30(local.button_text_ids[0], local.style_values[local.loop_index]);
    FUN_004088d0((int *)PTR_DAT_0040c0ac, local.button_text_ids[local.loop_index + 1],
                 local.face_slot_x + 0x20, local.style_values[5] + 0xe, DAT_0041e420 + 0x60);
    g_face_control4_sprites[local.loop_index] =
        EncodeSpriteFromPage(2, local.face_slot_x, local.style_values[5], 0x40, 0x1d);
    DAT_0040c1e8[4].page_by_mode[local.loop_index] = g_face_control4_sprites[local.loop_index];
  }

  local.face_slot_x = (local.control_slot % 5) * 0x70;
  local.style_values[5] = (local.control_slot / 5) * 0x60;
  local.control_slot = local.control_slot + 1;
  g_face_scroll_arrow_sprite = EncodeSpriteFromPage(2, local.face_slot_x, local.style_values[5], 9, 0x1d);

  local.face_slot_x = (local.control_slot % 5) * 0x70;
  local.style_values[5] = (local.control_slot / 5) * 0x60;
  local.control_slot = local.control_slot + 1;
  g_face_scroll_divider_sprite =
      EncodeSpriteFromPage(2, local.face_slot_x, local.style_values[5], 3, 0x1d);

  FUN_0040a130(2, s_FaceArt_facfrsh2New_pcx_0040c6b8);
  g_face_slot_background_sprite = EncodeSpriteFromPage(2, 0, 0, 0x8a, 0xac);
  FinalizeSpriteEncodeSession();

  local.file = fopen(s_FaceData_txt_0040c6d4, DAT_0040c6d0);
  local.face_slot_x = _fileno(local.file);
  local.style_values[5] = _filelength(local.face_slot_x);
  local.scan_ptr = malloc(local.style_values[5]);
  DAT_0041d120[0] = local.scan_ptr;
  local.scan_ptr = DAT_0041d120[0];
  fread(local.scan_ptr, local.style_values[5], 1, local.file);
  fclose(local.file);

  DAT_0040c1e0 = 1;
  DAT_0041d120[0] = local.scan_ptr;
  local.scan_ptr = local.scan_ptr + 1;
  local.loop_index = 1;
  while (local.loop_index < local.style_values[5])
  {
    if (*local.scan_ptr == '\n')
    {
      *local.scan_ptr = '\0';
      if (local.scan_ptr[1] != '\n')
      {
        DAT_0041d120[DAT_0040c1e0] = local.scan_ptr + 1;
        DAT_0040c1e0 = DAT_0040c1e0 + 1;
      }
    }
    local.loop_index = local.loop_index + 1;
    local.scan_ptr = local.scan_ptr + 1;
  }

  DAT_0040c1e0 = DAT_0040c1e0 - 1;
  for (local.loop_index = 0; local.loop_index < DAT_0040c1e0; local.loop_index = local.loop_index + 1)
  {
    local.scan_ptr = DAT_0041d120[local.loop_index];
    while (*local.scan_ptr != ',')
    {
      local.scan_ptr = local.scan_ptr + 1;
    }
    *local.scan_ptr = '\0';
    do
    {
      local.scan_ptr = local.scan_ptr + 1;
    } while (*local.scan_ptr == ' ' || *local.scan_ptr == '\t');
    DAT_0041e570[local.loop_index] = local.scan_ptr;
    while (*local.scan_ptr != ',' && *local.scan_ptr != ' ' && *local.scan_ptr != '\t')
    {
      local.scan_ptr = local.scan_ptr + 1;
    }
    local.char_temp = *local.scan_ptr;
    *local.scan_ptr = '\0';
    do
    {
      local.scan_ptr = local.scan_ptr + 1;
    } while (*local.scan_ptr == ' ' || *local.scan_ptr == '\t');
    DAT_0041c150[local.loop_index] = local.scan_ptr;
  }

  return 0;
}

// FUNCTION: FACEMAKER 0x00402717
void FUN_00402717(int *page, int x, int y, EncodedImage *encoded_image)
{
  EncodedImage *tmp = encoded_image;
  DrawEncodedImageUnscaled(page, x, y, encoded_image);
}

// FUNCTION: FACEMAKER 0x00402743
void FUN_00402743(int *page, int x, int y, unsigned int width, int height)
{
  struct
  {
    int row;
    int pixel;
    unsigned char row_pixels[256];
  } local;

  for (local.row = 0; local.row < height; local.row = local.row + 1)
  {
    FUN_004076c0((unsigned int *)local.row_pixels, *page, x, local.row + y, width);
    for (local.pixel = 0; local.pixel < (int)width; local.pixel = local.pixel + 1)
    {
      if (local.row_pixels[local.pixel] == '\0')
      {
        local.row_pixels[local.pixel] = 0xff;
      }
      else
      {
        local.row_pixels[local.pixel] = '\0';
      }
    }
    FUN_004075d0((unsigned int *)local.row_pixels, *page, width + x, local.row + y, width);
  }
}

// FUNCTION: FACEMAKER 0x00402edc
int FUN_00402edc(int selected_face_index)
{
  int face_slot_x;
  int face_slot_y;
  int control_index;
  char face_art_path[260];

  if (g_face_sprite_roots.first != (EncodedImage *)0)
  {
    FUN_00408950(g_face_sprite_roots.first);
  }

  strcpy(face_art_path, s_FaceArt__0040c64c);
  strcat(face_art_path, DAT_0041e570[selected_face_index]);
  g_face_group_count =
      LoadFaceSpriteSet(face_art_path, g_face_group_frame_counts, g_face_sprite_entries.flat,
                        g_face_sprite_roots.flat);

  if (g_face_group_count <= 6)
  {
    FUN_00402a84((FaceControl *)((int)DAT_0040c1e8 + 0x16c));
    FUN_00402a84((FaceControl *)((int)DAT_0040c1e8 + 0x1a0));
    for (control_index = 9; control_index < g_face_group_count * 2 + 9;
         control_index = control_index + 1)
    {
      if ((control_index & 1) != 0)
      {
        FUN_00402ad9(&DAT_0040c1e8[control_index]);
      }
      else if (g_face_group_frame_counts[(control_index - 9) / 2] == 1)
      {
        FUN_00402a84(&DAT_0040c1e8[control_index]);
      }
      else
      {
        FUN_00402ad9(&DAT_0040c1e8[control_index]);
      }
    }
    for (; control_index < 0x15; control_index = control_index + 1)
    {
      FUN_00402a84(&DAT_0040c1e8[control_index]);
    }
  }
  else
  {
    for (control_index = 9; control_index < 0x15; control_index = control_index + 1)
    {
      if ((control_index & 1) != 0)
      {
        FUN_00402ad9(&DAT_0040c1e8[control_index]);
      }
      else if (g_face_group_frame_counts[(control_index - 9) / 2] == 1)
      {
        FUN_00402a84(&DAT_0040c1e8[control_index]);
      }
      else
      {
        FUN_00402ad9(&DAT_0040c1e8[control_index]);
      }
    }
    FUN_00402a84((FaceControl *)((int)DAT_0040c1e8 + 0x16c));
    FUN_00402ad9((FaceControl *)((int)DAT_0040c1e8 + 0x1a0));
  }

  for (control_index = 0; control_index < 0x15; control_index = control_index + 1)
  {
    FUN_004028d1(&DAT_0040c1e8[control_index], 0);
  }
  memset(g_face_variant_index, 0, sizeof(g_face_variant_index));
  DAT_0041e0c0 = 0;

  for (control_index = 0; control_index < MIN(g_face_group_count, 6);
       control_index = control_index + 1)
  {
    FUN_00403573(control_index, 0);
  }

  for (; control_index < 6; control_index = control_index + 1)
  {
    face_slot_x = ((control_index + 2) % 4) * 0x9f + 0xd;
    face_slot_y = ((control_index + 2) / 4) * 0xdf + 0x24;
    DrawEncodedImageAutoScale((int *)PTR_DAT_0040c05c, face_slot_x, face_slot_y,
                              g_face_slot_background_sprite);
    FUN_00402621((int *)PTR_DAT_0040c05c, face_slot_x, face_slot_y, 0x89, 0xac,
                 (int *)PTR_DAT_0040c034, face_slot_x, face_slot_y);
  }
  FUN_004032e4();
  return selected_face_index;
}

// FUNCTION: FACEMAKER 0x00402841
void FUN_00402841(FaceControl *control)
{
  FILE *recent_face_file;

  if (DAT_0041e50c == 0 && DAT_0040c9e4 == 0)
  {
    FUN_004033c9(control);
  }

  recent_face_file = fopen(s_FaceMostRecent_txt_0040c630, DAT_0040c62c);
  if (recent_face_file != (FILE *)0)
  {
    fprintf(recent_face_file, s__d___s_0040c644, DAT_0040c1e4, DAT_0041e570[DAT_0040c1e4]);
    fclose(recent_face_file);
  }
  DAT_0041e4f0 = 1;
}

// FUNCTION: FACEMAKER 0x00402a42
int FUN_00402a42(FaceControl *control)
{
  DAT_0040c1e4 = (control->value_index + DAT_0040c1e4 + DAT_0040c1e0) % DAT_0040c1e0;
  FUN_00402edc(DAT_0040c1e4);
  return DAT_0040c1e4;
}

// FUNCTION: FACEMAKER 0x00402b24
int FUN_00402b24(FaceControl *control)
{
  int control_index;
  int visible_face_index;

  if (g_face_group_count <= 6)
  {
    return 0;
  }

  DAT_0041e0c0 = DAT_0041e0c0 + control->value_index;
  DAT_0041e0c0 = (DAT_0041e0c0 <= 0) ? 0 : DAT_0041e0c0;
  DAT_0041e0c0 = MIN(g_face_group_count - 6, DAT_0041e0c0);

  for (control_index = 9; control_index < 0x15; control_index = control_index + 2)
  {
    visible_face_index = DAT_0041e0c0 + (control_index - 9) / 2;
    if (g_face_variant_index[visible_face_index] == -1)
    {
      FUN_00402a84(&DAT_0040c1e8[control_index]);
    }
    else
    {
      FUN_00402ad9(&DAT_0040c1e8[control_index]);
    }

    if (g_face_group_frame_counts[visible_face_index] - 1 == g_face_variant_index[visible_face_index])
    {
      FUN_00402a84(&DAT_0040c1e8[control_index + 1]);
    }
    else
    {
      FUN_00402ad9(&DAT_0040c1e8[control_index + 1]);
    }
  }

  for (control_index = 0; control_index < 6; control_index = control_index + 1)
  {
    FUN_00403573(control_index, 0);
  }

  if (DAT_0041e0c0 == 0)
  {
    FUN_00402a84(&DAT_0040c1e8[7]);
    FUN_00402ad9(&DAT_0040c1e8[8]);
  }
  else if (g_face_group_count - 6 == DAT_0041e0c0)
  {
    FUN_00402a84(&DAT_0040c1e8[8]);
    FUN_00402ad9(&DAT_0040c1e8[7]);
  }
  else
  {
    FUN_00402ad9(&DAT_0040c1e8[7]);
    FUN_00402ad9(&DAT_0040c1e8[8]);
  }

  return DAT_0041e0c0;
}

// FUNCTION: FACEMAKER 0x00402d30
int FUN_00402d30(FaceControl *control)
{
  struct
  {
    FaceControl *sibling_control_third;
    int button_index;
    int temp_0c;
    int face_index;
    int value_delta;
  } local;

  local.face_index = control->value_index + DAT_0041e0c0;
  local.value_delta = control->value_delta;
  g_face_variant_index[local.face_index] = g_face_variant_index[local.face_index] + local.value_delta;
  local.value_delta = g_face_variant_index[local.face_index];
  if (local.value_delta <= -1)
  {
    local.value_delta = -1;
  }
  g_face_variant_index[local.face_index] = local.value_delta;

  local.value_delta = g_face_group_frame_counts[local.face_index] - 1;
  local.temp_0c = g_face_variant_index[local.face_index];
  if (local.value_delta >= local.temp_0c)
  {
    local.value_delta = local.temp_0c;
  }
  g_face_variant_index[local.face_index] = local.value_delta;
  FUN_00403573(local.face_index - DAT_0041e0c0, 1);

  if (g_face_variant_index[local.face_index] == -1)
  {
    FUN_00402a84(control);
    if ((((int)control - (int)DAT_0040c1e8) / 0x34 & 1) != 0)
    {
      local.temp_0c = (int)(control + 1);
    }
    else
    {
      local.temp_0c = (int)(control - 1);
    }
    FUN_00402ad9((FaceControl *)local.temp_0c);
  }
  else if (g_face_group_frame_counts[local.face_index] - 1 == g_face_variant_index[local.face_index])
  {
    FUN_00402a84(control);
    if ((((int)control - (int)DAT_0040c1e8) / 0x34 & 1) != 0)
    {
      local.button_index = (int)(control + 1);
    }
    else
    {
      local.button_index = (int)(control - 1);
    }
    FUN_00402ad9((FaceControl *)local.button_index);
  }
  else
  {
    FUN_00402ad9(control);
    if ((((int)control - (int)DAT_0040c1e8) / 0x34 & 1) != 0)
    {
      local.sibling_control_third = control + 1;
    }
    else
    {
      local.sibling_control_third = control - 1;
    }
    FUN_00402ad9(local.sibling_control_third);
  }

  return g_face_variant_index[local.face_index];
}

// FUNCTION: FACEMAKER 0x004033c9
void FUN_004033c9(FaceControl *control)
{
  int face_index;

  (void)control;

  FUN_00406d00(2, 0);
  FUN_00402717((int *)PTR_DAT_0040c0ac, 0, 0, g_face_sprite_roots.first);
  for (face_index = 0; face_index < g_face_group_count; face_index = face_index + 1)
  {
    if (g_face_variant_index[face_index] >= 0 &&
        g_face_sprite_entries.groups[face_index][g_face_variant_index[face_index]] != (EncodedImage *)0)
    {
      FUN_00402717((int *)PTR_DAT_0040c0ac, 0, 0,
                   g_face_sprite_entries.groups[face_index][g_face_variant_index[face_index]]);
    }
  }

  FUN_00402743((int *)PTR_DAT_0040c0ac, 0, 0, 0x8a, 0xaa);
  DAT_0041e50c = 1;
  if (DAT_0040c9e4 != 0)
  {
    if (SendMessageA(DAT_0041b028, 0x111, 0x9c47, 0) != 0)
    {
      FUN_00409420(2, 0, 0, 0x114, 0xaa, 0, DAT_0041b030);
    }
  }
  else
  {
    if (DAT_0041c150[DAT_0040c1e4][0] == 'm')
    {
      FUN_00407190((int *)PTR_DAT_0040c0ac, 0, 200, 0x6d);
    }
    else
    {
      FUN_00407190((int *)PTR_DAT_0040c0ac, 0, 200, 0x66);
    }
    FUN_004075d0((unsigned int *)DAT_0041d120[DAT_0040c1e4], 2, 1, 200,
                 (unsigned int)strlen(DAT_0041d120[DAT_0040c1e4]));
  }
}

// FUNCTION: FACEMAKER 0x00402a84
int FUN_00402a84(FaceControl *control)
{
  int old_mode;

  old_mode = control->state;
  DAT_0041e560 = 1;
  control->state = 0;
  FUN_004028d1(control, 3);
  DAT_0041e560 = 0;
  control->state = 3;
  return old_mode;
}

// FUNCTION: FACEMAKER 0x00402ad9
int FUN_00402ad9(FaceControl *control)
{
  int old_mode;

  old_mode = control->state;
  control->state = 0;
  DAT_0041e560 = 1;
  FUN_004028d1(control, 0);
  DAT_0041e560 = 0;
  return old_mode;
}

// FUNCTION: FACEMAKER 0x004028d1
int FUN_004028d1(FaceControl *control, int mode)
{
  int in_bounds;

  if (DAT_0041e560 == 0)
  {
    if (DAT_0042333c < (int)FUN_00402600(control->x) ||
        (int)FUN_00402600(control->width + control->x) < DAT_0042333c)
    {
      in_bounds = 0;
    }
    else if (DAT_00423338 < (int)FUN_00402600(control->y) ||
             (int)FUN_00402600(control->y + control->height) < DAT_00423338)
    {
      in_bounds = 0;
    }
    else
    {
      in_bounds = 1;
    }
    if (!in_bounds)
    {
      return 0;
    }
  }

  if (control->state == 3)
  {
    return 0;
  }

  if (control->page_by_mode[mode] != 0)
  {
    DrawEncodedImageAutoScale((int *)PTR_DAT_0040c034, control->x, control->y,
                              control->page_by_mode[mode]);
  }

  if (control->page_by_mode[mode] != 0)
  {
    DrawEncodedImageAutoScale((int *)PTR_DAT_0040c05c, control->x, control->y,
                              control->page_by_mode[mode]);
  }

  if (mode == 2 && control->on_activate != (FaceControlCallback)0)
  {
    control->on_activate(control);
  }
  return 1;
}

// FUNCTION: FACEMAKER 0x004032e4
void FUN_004032e4(void)
{
  int face_index;

  DrawEncodedImageAutoScale((int *)PTR_DAT_0040c05c, 0xd, 0x24, g_face_slot_background_sprite);
  DrawEncodedImageAutoScale((int *)PTR_DAT_0040c05c, 0xd, 0x24, g_face_sprite_roots.first);
  for (face_index = 0; face_index < *(volatile int *)&g_face_group_count; face_index++)
  {
    if (g_face_variant_index[face_index] >= 0 &&
        g_face_sprite_entries.groups[face_index][g_face_variant_index[face_index]] != (EncodedImage *)0)
    {
      DrawEncodedImageAutoScale((int *)PTR_DAT_0040c05c, 0xd, 0x24,
                                g_face_sprite_entries.groups[face_index][g_face_variant_index[face_index]]);
    }
  }

  FUN_00402621((int *)PTR_DAT_0040c05c, 0xd, 0x24, 0x89, 0xa9, (int *)PTR_DAT_0040c034, 0xd, 0x24);
}

// FUNCTION: FACEMAKER 0x00403573
void FUN_00403573(int index, int selected)
{
  int face_slot_x;
  int face_slot_y;

  face_slot_x = ((index + 2) % 4) * 0x9f + 0xd;
  face_slot_y = ((index + 2) / 4) * 0xdf + 0x24;
  DrawEncodedImageAutoScale((int *)PTR_DAT_0040c05c, face_slot_x, face_slot_y,
                            g_face_slot_background_sprite);
  if (g_face_variant_index[index + DAT_0041e0c0] >= 0 &&
      g_face_sprite_entries.groups[index + DAT_0041e0c0][g_face_variant_index[index + DAT_0041e0c0]] !=
          (EncodedImage *)0)
  {
    DrawEncodedImageAutoScale((int *)PTR_DAT_0040c05c, face_slot_x, face_slot_y,
                              g_face_sprite_entries.groups[index + DAT_0041e0c0]
                                                          [g_face_variant_index[index + DAT_0041e0c0]]);
  }

  FUN_00402621((int *)PTR_DAT_0040c05c, face_slot_x, face_slot_y, 0x89, 0xab, (int *)PTR_DAT_0040c034,
               face_slot_x, face_slot_y);
  if (selected != 0)
  {
    FUN_004032e4();
  }
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
  int hover_button_index;
  int next_hover_button_index;
  int in_bounds;
  int key_char;
  DIBSurface *page;
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
  FUN_00406610();

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

  recent_face_file = fopen(s_FaceMostRecent_txt_0040c630, DAT_0040c62c);
  if (recent_face_file != (FILE *)0)
  {
    fscanf(recent_face_file, s__d___s_0040c644, &DAT_0040c1e4, recent_face_name);
    if (_stricmp(DAT_0041e570[DAT_0040c1e4], recent_face_name) != 0)
    {
      DAT_0040c1e4 = 0;
    }
    fclose(recent_face_file);
  }

  strcpy(face_art_path, s_FaceArt__0040c64c);
  strcat(face_art_path, DAT_0041e570[DAT_0040c1e4]);
  g_face_group_count =
      LoadFaceSpriteSet(face_art_path, g_face_group_frame_counts, g_face_sprite_entries.flat,
                        g_face_sprite_roots.flat);
  FUN_00402a84(&DAT_0040c354);
  if (g_face_group_count < 7)
  {
    FUN_00402a84(&DAT_0040c388);
  }

  for (face_index = 0; face_index < 6; face_index++)
  {
    sign_bits = (int)(face_index + 2U) >> 0x1f;
    face_slot_x = ((((face_index + 2U ^ sign_bits) - sign_bits & 3U) ^ sign_bits) - sign_bits) * 0x9f +
                  0xd;
    face_slot_y = ((int)(face_index + 2 + ((face_index + 2 >> 0x1f) & 3U)) >> 2) * 0xdf + 0x24;
    DrawEncodedImageAutoScale((int *)PTR_DAT_0040c05c, face_slot_x, face_slot_y,
                              g_face_slot_background_sprite);
    FUN_00402621((int *)PTR_DAT_0040c05c, face_slot_x, face_slot_y, 0x89, 0xa9,
                 (int *)PTR_DAT_0040c034, face_slot_x, face_slot_y);
  }

  face_index = 0;
  while (1)
  {
    visible_face_count = g_face_group_count;
    if (5 < g_face_group_count)
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
    DrawEncodedImageAutoScale((int *)PTR_DAT_0040c05c, *(int *)((char *)&DAT_0040c190 + face_index * 8),
                              *(int *)((char *)&DAT_0040c194 + face_index * 8),
                              g_face_scroll_arrow_sprite);
  }

  for (face_index = 0; face_index < 4; face_index++)
  {
    DrawEncodedImageAutoScale((int *)PTR_DAT_0040c05c, *(int *)((char *)&DAT_0040c1c0 + face_index * 8),
                              ((int *)&DAT_0040c1c4)[face_index * 2], g_face_scroll_arrow_sprite);
  }

  *(int *)PTR_DAT_0040c034 = 1;
  DAT_0041e560 = 1;
  button_index = 1;
  while (1)
  {
    visible_face_count = g_face_group_count;
    if (5 < g_face_group_count)
    {
      visible_face_count = 6;
    }
    if (visible_face_count * 2 + 9 <= button_index)
    {
      break;
    }
    FUN_004028d1(&DAT_0040c1e8[button_index], 0);
    button_index++;
  }

  while (button_index < 0x15)
  {
    FUN_004028d1(&DAT_0040c1e8[button_index], 3);
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

  hover_button_index = -1;
  do
  {
    if (hover_button_index >= 0)
    {
      face_slot_x = (int)FUN_00402600(DAT_0040c1e8[hover_button_index].x);
      if (DAT_0042333c < face_slot_x ||
          (int)FUN_00402600(DAT_0040c1e8[hover_button_index].x +
                            DAT_0040c1e8[hover_button_index].width) < DAT_0042333c)
      {
        in_bounds = 0;
      }
      else
      {
        face_slot_y = (int)FUN_00402600(DAT_0040c1e8[hover_button_index].y);
        if (DAT_00423338 < face_slot_y ||
            (int)FUN_00402600(DAT_0040c1e8[hover_button_index].y +
                              DAT_0040c1e8[hover_button_index].height) < DAT_00423338)
        {
          in_bounds = 0;
        }
        else
        {
          in_bounds = 1;
        }
      }

      if (in_bounds == 0)
      {
        DAT_0041e560 = 1;
        FUN_004028d1(&DAT_0040c1e8[hover_button_index], 0);
        hover_button_index = -1;
        DAT_0041e560 = 0;
      }
    }

    button_index = 0;
    next_hover_button_index = hover_button_index;
    while (1)
    {
      visible_face_count = g_face_group_count;
      if (5 < g_face_group_count)
      {
        visible_face_count = 6;
      }
      if (visible_face_count * 2 + 9 <= button_index)
      {
        break;
      }
      if (hover_button_index != button_index && FUN_004028d1(&DAT_0040c1e8[button_index], 0) != 0)
      {
        next_hover_button_index = button_index;
      }
      button_index = button_index + 1;
    }

    if (hover_button_index != next_hover_button_index)
    {
      if (hover_button_index >= 0)
      {
        DAT_0041e560 = 1;
        FUN_004028d1(&DAT_0040c1e8[hover_button_index], 0);
        DAT_0041e560 = 0;
      }
      FUN_004028d1(&DAT_0040c1e8[next_hover_button_index], 1);
    }

    if (DAT_00423334 == 1)
    {
      if (next_hover_button_index >= 0)
      {
        FUN_004028d1(&DAT_0040c1e8[next_hover_button_index], 2);
      }
      FUN_00405f38();
      while (DAT_00423334 != 0)
      {
        if (4 < FUN_00405f60())
        {
          if (next_hover_button_index >= 0)
          {
            FUN_004028d1(&DAT_0040c1e8[next_hover_button_index], 2);
          }
          FUN_00405f38();
        }
      }
      if (next_hover_button_index >= 0)
      {
        FUN_004028d1(&DAT_0040c1e8[next_hover_button_index], 1);
      }
    }

    hover_button_index = next_hover_button_index;
    if (FUN_00406590() != 0)
    {
      key_char = FUN_004065a0();
      if ((char)key_char == '\x1b')
      {
        DAT_0041e564 = -1;
        DAT_0041e4f0 = 1;
      }
    }
  } while (DAT_0041e4f0 == 0);

  if (DAT_0040c188 != 0)
  {
    FUN_004093e0();
  }
  _chdir(DAT_0041b024);
  SendMessageA(DAT_0041b028, 0x12, DAT_0041e564, 0);
  ExitThread(DAT_0041e564);
  return DAT_0041e564;
}

// FUNCTION: FACEMAKER 0x00405a49
LRESULT CALLBACK FUN_00405a49(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  char face_art_path[528];

  switch (msg)
  {
  case WM_CREATE:
    DAT_00412490 = SetTimer(hwnd, 1, DAT_0040c9e0, (TIMERPROC)0);
    break;

  case 0x12:
    FUN_0040a730(0, 0x30);
    DestroyWindow(hwnd);
    PostQuitMessage(0);
    break;

  case WM_DESTROY:
    KillTimer(hwnd, DAT_00412490);
    PostMessageA(hwnd, 0x12, 0, 0);
    break;

  case WM_TIMER:
    DAT_0040c9e8 = DAT_0040c9e8 + 1;
    break;

  case WM_SIZE:
    if (DAT_0041c144 != 0)
    {
      FUN_00406cc0(1);
    }
    break;

  case WM_PAINT:
    if (DAT_0041c144 != 0)
    {
      FUN_00406cc0(1);
    }
    break;

  case WM_MOUSEMOVE:
    DAT_0042333c = (unsigned int)lparam & 0xffff;
    DAT_00423338 = (unsigned short)(((unsigned int)lparam >> 0x10) & 0xffff);
    break;

  case WM_LBUTTONDOWN:
    DAT_00423340 = DAT_00423340 | 1;
    DAT_00423334 = 1;
    DAT_0042333c = (unsigned int)lparam & 0xffff;
    DAT_00423338 = (unsigned short)(((unsigned int)lparam >> 0x10) & 0xffff);
    break;

  case WM_RBUTTONDOWN:
    DAT_00423340 = DAT_00423340 | 2;
    DAT_00423334 = 2;
    DAT_0042333c = (unsigned int)lparam & 0xffff;
    DAT_00423338 = (unsigned short)(((unsigned int)lparam >> 0x10) & 0xffff);
    break;

  case WM_LBUTTONUP:
  case WM_RBUTTONUP:
    DAT_00423334 = 0;
    break;

  case WM_KEYDOWN:
  case WM_SYSKEYDOWN:
    FUN_004063f0(wparam, lparam);
    return DefWindowProcA(hwnd, msg, wparam, lparam);

  case WM_COMMAND:
    switch (wparam)
    {
    case 0x9c46:
      strcpy(face_art_path, s_FaceArt_evil_0040caa0);
      LoadFaceSpriteSet(face_art_path, g_face_group_frame_counts, g_face_sprite_entries.flat,
                        g_face_sprite_roots.flat);
      FUN_00406cc0(1);
      break;

    case 0x9c47:
      return FUN_00406369();

    case 0xffff:
      if (DAT_0041248c == 0)
      {
        FUN_0040a330(DAT_0041c140);
        DAT_0041248c = 1;
      }

      DAT_00412488 = FindWindowExA((HWND)0, (HWND)0, s_ShowPaletteClass_0040cac0,
                                   s_Current_Palette_0040cab0);
      if (DAT_00412488 != (HWND)0)
      {
        BringWindowToTop(DAT_00412488);
      }
      else
      {
        DAT_00412488 = FUN_0040a3c0(DAT_0041c140, (HWND)0);
        if (DAT_00412488 == (HWND)0)
        {
          return 0;
        }
        ShowWindow(DAT_00412488, SW_SHOW);
      }
      UpdateWindow(DAT_00412488);
      break;
    }
    break;

  default:
    return DefWindowProcA(hwnd, msg, wparam, lparam);
  }

  return DefWindowProcA(hwnd, msg, wparam, lparam);
}

// FUNCTION: FACEMAKER 0x00405f25
void __stdcall FUN_00405f25(int param_1, int param_2, int param_3, int param_4, int param_5)
{
  (void)param_1;
  (void)param_2;
  (void)param_3;
  (void)param_4;
  (void)param_5;
  DAT_0040c9e8 = DAT_0040c9e8 + 1;
}

// FUNCTION: FACEMAKER 0x00405f38
int FUN_00405f38(void)
{
  int elapsed_ticks;

  elapsed_ticks = DAT_0040c9e8;
  DAT_0040c9e8 = 0;
  return elapsed_ticks;
}

// FUNCTION: FACEMAKER 0x00405f60
int FUN_00405f60(void)
{
  return DAT_0040c9e8;
}

// FUNCTION: FACEMAKER 0x00405f75
int FUN_00405f75(int wait_ticks)
{
  int start_ticks;

  start_ticks = DAT_0040c9e8;
  while (DAT_0040c9e8 - start_ticks < wait_ticks)
  {
  }
  return 0;
}

// FUNCTION: FACEMAKER 0x00405fa8
void FUN_00405fa8(void)
{
  HDC hdc;

  if (DAT_0040c9e4 == 0)
  {
    SetSystemPaletteUse(global_main_hdc, 1);
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
LONG FUN_004061bd(DWORD width, DWORD height)
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
  global_main_hdc = GetDC(main_hwnd);
  BringWindowToTop(DAT_0041b028);
  SetForegroundWindow(DAT_0041b028);
  SetFocus(DAT_0041b028);
  if (custom_mode_selected != 0)
  {
    DAT_00426540[0]->hTempDC = global_main_hdc;
    SelectPalette(DAT_00426540[0]->hTempDC, DAT_00426540[0]->hPalette, FALSE);
    RealizePalette(DAT_00426540[0]->hTempDC);
    SetStretchBltMode(DAT_00426540[0]->hTempDC, 3);
  }

  SetSystemPaletteUse(global_main_hdc, 2);
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
