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

struct FaceControl;

typedef void(__cdecl *FaceControlCallback)(struct FaceControl *control);
typedef int(__cdecl *FaceControlModeCallback)(struct FaceControl *control, int mode);

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

extern DIBSurface *InitializeGraphicsSystem(int width, int height, int bits_per_pixel);
extern DIBSurface *CreateGraphicsPage(int page_number, int width, int height, int bits_per_pixel);
extern int LegacyInitNoop();
extern int LoadFontConfigIfPresent(char *executable_name, char *config_name);
extern DIBSurface *GetPrimaryPage(int unused);
extern DIBSurface *GetPageByNumber(int page_number);
extern DIBSurface *InitializeGraphicsSystemDefaultMode(void);
extern void SetGraphicsPage(int page_number, DIBSurface *page);
extern char s_D__NewMagic__sources__sidlib__lib_c_0040d0ec[];
extern int LoadFontCollection(char *path);
extern int LoadBitmapFontFromFile(int font_id, FILE *file);
extern void BlitGraphicsRect(FacemakerWindowBounds *dst, unsigned int dst_x, int dst_y, unsigned int width,
                             DWORD height, FacemakerWindowBounds *src, int src_x, int src_y);
extern void PutGraphicsPixel(FacemakerWindowBounds *dst, int x, int y, int char_code);
extern void DrawEncodedImageUnscaled(FacemakerWindowBounds *dst, int x, int y, EncodedImage *encoded_image);
extern void DrawEncodedImageResampled(FacemakerWindowBounds *dst, int x, int y, int width, int height,
                                      EncodedImage *encoded_image);
extern void LoadPcxResource(int page_number, int x, int y, char *path, void *opaque);
extern unsigned int ScaleUiCoordinate(int value);
extern void BlitScaledRect(FacemakerWindowBounds *dst, int dst_x, int dst_y, int width, int height, FacemakerWindowBounds *src,
                           int src_x, int src_y);
extern void DrawEncodedImageAutoScale(FacemakerWindowBounds *dst, int x, int y, EncodedImage *encoded_image);
extern int LoadSystemFont(int font_id, unsigned int point_size, char *font_file, char *font_name,
                          int weight, DWORD italic);
extern void PresentGraphicsPage(int num);
extern int InitializeLegacyVideoStub(void);
extern void LoadPcxIntoPage(int page_number, char *path);
extern size_t WriteSpriteBlob(void *sprite_blob, char *output_path);
extern int ReadSpriteEntryPointers(EncodedImage **out_entry_ptrs, char *sprite_path);
extern int DrawTextFormatted(FacemakerWindowBounds *dst, int text_id, int draw_shadow, int scale_to_screen, int center_x, int center_y,
                             int x, int y, int *format_and_args);
extern void StretchBlitGraphicsRect(FacemakerWindowBounds *dst, int dst_x, int dst_y, int src_w, int src_h, FacemakerWindowBounds *src,
                                    int src_x, int src_y, int copy_w, int copy_h);
extern int InitializeFaceMakerAssets(void);
extern void DrawFaceLayerUnscaled(FacemakerWindowBounds *page, int x, int y, EncodedImage *encoded_image);
extern void DrawTransparentOverlayMask(FacemakerWindowBounds *page, int x, int y, unsigned int width, int height);
extern int LoadSelectedFaceSet(int selected_face_index);
extern int LoadFaceSpriteSet(char *base_path, int *group_frame_counts, EncodedImage **group_entries,
                             EncodedImage **first_sprite_out);
extern int ShowSaveFaceDialog(void);
extern void QueueKeyInputFromMessage(WPARAM wparam, LPARAM lparam);
extern int DisableFaceControl(FaceControl *control);
extern int EnableFaceControl(FaceControl *control);
extern void AcceptFaceSelection(FaceControl *control);
extern int CycleFaceSet(FaceControl *control);
extern int ScrollFaceSetList(FaceControl *control);
extern int AdjustVisibleFaceVariant(FaceControl *control);
extern void ComposeSelectedFaceImage(FaceControl *control);
extern void DrawFaceSlotEntry(int index, int selected);
extern int DrawFaceControl(FaceControl *control, int mode);
extern void RedrawFaceCompositePreview(void);
extern void BeginSpriteEncodeSession(void);
extern void LoadPcxIntoPageNoPalette(int page_number, char *path);
extern int SetFontStyleSize(int font_id, unsigned int style);
extern void DrawTextAt(FacemakerWindowBounds *dst, int text_id, int x, int y, char *text, ...);
extern EncodedImage *EncodeSpriteFromPage(int page_number, int x, int y, unsigned int width,
                                          int height);
extern int ExportEncodedImage(int param_1, int param_2, int param_3, int param_4, int param_5, int param_6,
                              char *param_7);
extern void FinalizeSpriteEncodeSession(void);
extern void FreeSpriteBlob(void *memory);
extern int ConsumeUiTickCount(void);
extern int PeekUiTickCount(void);
extern int HasQueuedKeyInput(void);
extern int PopQueuedKeyInput(void);
extern int CleanupLegacyVideoStub(void);
extern void WriteGraphicsScanline(unsigned int *param_1, int param_2, int param_3, int param_4,
                                  unsigned int param_5);
extern void ReadGraphicsScanline(unsigned int *param_1, int param_2, int param_3, int param_4,
                                 unsigned int param_5);
extern ATOM RegisterPaletteClass(HINSTANCE hInstance);
extern HWND CreatePalettePopupWindow(HINSTANCE hInstance, HWND parent_hwnd);
extern int AnimatePaletteToColor(int enabled, int mode);
extern int ClearGraphicsPageWithPaletteColor(int page_number, int color_index);
extern void CopyBytesAsmCompat(double *dst, double *src, unsigned int size);
extern int g_frontbuffer_direct_blit_enabled;
extern int g_graphics_bpp;
extern RGBQUAD g_palette_rgb[256];
extern RpBitsPalettePacket g_palette_data_words;
extern unsigned int g_palette_transition_source_words[0xc8];
extern int g_palette_transition_work_words[0x400];
extern int g_palette_transition_hsv[0x301];
extern int g_palette_transition_value_step[0x2ff];
extern unsigned char *g_palette_rgb_bytes;
extern FontSlot g_font_slots[0x10];
extern char g_file_read_mode[];

// GLOBAL: FACEMAKER 0x0040c010
FacemakerWindowBounds g_face_preview_page_bounds = {0, 0, 0, 0x13f, 0xc7, 1, 0xf, 4, 0};

// GLOBAL: FACEMAKER 0x0040c038
FacemakerWindowBounds g_face_work_page_bounds = {1, 0, 0, 0x13f, 0xc7, 1, 0xf, 4, 0};

// GLOBAL: FACEMAKER 0x0040c060
FacemakerWindowBounds g_face_aux_page_bounds = {2, 0, 0, 0x13f, 0xc7, 1, 0xf, 4, 0};

// GLOBAL: FACEMAKER 0x0040c088
FacemakerWindowBounds g_face_fullscreen_page_bounds = {2, 0, 0, 0x320, 0x258, 1, 0x9f, 4, 0};

// GLOBAL: FACEMAKER 0x0040c034
FacemakerWindowBounds *g_face_preview_bounds = &g_face_preview_page_bounds;

// GLOBAL: FACEMAKER 0x0040c05c
FacemakerWindowBounds *g_face_work_bounds = &g_face_work_page_bounds;

// GLOBAL: FACEMAKER 0x0040c0ac
FacemakerWindowBounds *g_face_fullscreen_bounds = &g_face_fullscreen_page_bounds;

// GLOBAL: FACEMAKER 0x0040c9e4
int g_use_desktop_palette = 1;

// GLOBAL: FACEMAKER 0x0040c9e0
int g_ui_timer_interval_ms = 0x42;

// GLOBAL: FACEMAKER 0x0040c9e8
int g_ui_tick_count = 0;

// GLOBAL: FACEMAKER 0x0040c188
int g_legacy_video_state = 0;

// GLOBAL: FACEMAKER 0x0040c190
int g_face_slot_arrow_xy[6][2] = {
    {0x18b, 0xd9},
    {0x22a, 0xd9},
    {0x4d, 0x1b8},
    {0xec, 0x1b8},
    {0x18b, 0x1b8},
    {0x22a, 0x1b8}};

// GLOBAL: FACEMAKER 0x0040c1c0
int g_face_variant_arrow_xy[4][2] = {
    {221, 138},
    {270, 138},
    {213, 188},
    {278, 188}};

// GLOBAL: FACEMAKER 0x0040c1e0
int g_face_set_total = 1000;

// GLOBAL: FACEMAKER 0x0040c1e4
int g_selected_face_set_index = 0;

// GLOBAL: FACEMAKER 0x0040c1e8
FaceControl g_face_controls[0x15] = {
    {0, 0, 0x40, 0x20, DrawFaceControl, (FaceControlCallback)0, 0, 0, 0, {0, 0, 0, 0}},
    {0xce, 0x24, 0x56, 0x21, DrawFaceControl, ComposeSelectedFaceImage, 0, 0, 0, {0, 0, 0, 0}},
    {0xce, 0x56, 0x56, 0x21, DrawFaceControl, AcceptFaceSelection, 0, 0, 0, {0, 0, 0, 0}},
    {0xe0, 0x8a, 0x30, 0x21, DrawFaceControl, (FaceControlCallback)0, 0, 0, 0, {0, 0, 0, 0}},
    {0xd8, 0xbc, 0x40, 0x21, DrawFaceControl, (FaceControlCallback)0, 0, 0, 0, {0, 0, 0, 0}},
    {0xb9, 0x8a, 0x25, 0x1d, DrawFaceControl, (FaceControlCallback)CycleFaceSet, -1, 0, 0, {0, 0, 0, 0}},
    {0x111, 0x8a, 0x25, 0x1d, DrawFaceControl, (FaceControlCallback)CycleFaceSet, 1, 0, 0, {0, 0, 0, 0}},
    {0xb1, 0xbc, 0x25, 0x1d, DrawFaceControl, (FaceControlCallback)ScrollFaceSetList, -1, 0, 0, {0, 0, 0, 0}},
    {0x119, 0xbc, 0x25, 0x1d, DrawFaceControl, (FaceControlCallback)ScrollFaceSetList, 1, 0, 0, {0, 0, 0, 0}},
    {0x167, 0xd9, 0x25, 0x1d, DrawFaceControl, (FaceControlCallback)AdjustVisibleFaceVariant, 0, -1, 0, {0, 0, 0, 0}},
    {0x194, 0xd9, 0x25, 0x1d, DrawFaceControl, (FaceControlCallback)AdjustVisibleFaceVariant, 0, 1, 0, {0, 0, 0, 0}},
    {0x206, 0xd9, 0x25, 0x1d, DrawFaceControl, (FaceControlCallback)AdjustVisibleFaceVariant, 1, -1, 0, {0, 0, 0, 0}},
    {0x233, 0xd9, 0x25, 0x1d, DrawFaceControl, (FaceControlCallback)AdjustVisibleFaceVariant, 1, 1, 0, {0, 0, 0, 0}},
    {0x29, 0x1b8, 0x25, 0x1d, DrawFaceControl, (FaceControlCallback)AdjustVisibleFaceVariant, 2, -1, 0, {0, 0, 0, 0}},
    {0x56, 0x1b8, 0x25, 0x1d, DrawFaceControl, (FaceControlCallback)AdjustVisibleFaceVariant, 2, 1, 0, {0, 0, 0, 0}},
    {0xc8, 0x1b8, 0x25, 0x1d, DrawFaceControl, (FaceControlCallback)AdjustVisibleFaceVariant, 3, -1, 0, {0, 0, 0, 0}},
    {0xf5, 0x1b8, 0x25, 0x1d, DrawFaceControl, (FaceControlCallback)AdjustVisibleFaceVariant, 3, 1, 0, {0, 0, 0, 0}},
    {0x167, 0x1b8, 0x25, 0x1d, DrawFaceControl, (FaceControlCallback)AdjustVisibleFaceVariant, 4, -1, 0, {0, 0, 0, 0}},
    {0x194, 0x1b8, 0x25, 0x1d, DrawFaceControl, (FaceControlCallback)AdjustVisibleFaceVariant, 4, 1, 0, {0, 0, 0, 0}},
    {0x206, 0x1b8, 0x25, 0x1d, DrawFaceControl, (FaceControlCallback)AdjustVisibleFaceVariant, 5, -1, 0, {0, 0, 0, 0}},
    {0x233, 0x1b8, 0x25, 0x1d, DrawFaceControl, (FaceControlCallback)AdjustVisibleFaceVariant, 5, 1, 0, {0, 0, 0, 0}}};

// GLOBAL: FACEMAKER 0x0041afb4
HDC global_main_hdc;

// GLOBAL: FACEMAKER 0x0041b024
char g_original_working_dir[4];

// GLOBAL: FACEMAKER 0x0041af50
char g_default_face_filename[0x64];

// GLOBAL: FACEMAKER 0x0041afc0
char g_save_dialog_title[0x64];

// GLOBAL: FACEMAKER 0x0041b028
HWND g_main_hwnd;

// GLOBAL: FACEMAKER 0x0041b030
char g_face_output_path[0x105];

// GLOBAL: FACEMAKER 0x0041c140
HINSTANCE g_app_instance;

// GLOBAL: FACEMAKER 0x0041c144
int g_ui_ready;

// GLOBAL: FACEMAKER 0x00412488
HWND g_palette_hwnd;

// GLOBAL: FACEMAKER 0x0041248c
int g_palette_class_registered;

// GLOBAL: FACEMAKER 0x00412490
int g_main_timer_id;

// GLOBAL: FACEMAKER 0x0041c150
char *g_face_set_gender_tags[1000];

// GLOBAL: FACEMAKER 0x0041d110
int global_screen_width;

// GLOBAL: FACEMAKER 0x0041d114
int global_screen_height;

// GLOBAL: FACEMAKER 0x0041d120
char *g_face_set_display_names[1000];

// GLOBAL: FACEMAKER 0x0041d0f0
FaceSpriteRootStorage g_face_sprite_roots;

// GLOBAL: FACEMAKER 0x0041e3f0
EncodedImage *g_face_control_even_sprites[4];

// GLOBAL: FACEMAKER 0x0041e400
EncodedImage *g_face_control_odd_sprites[4];

// GLOBAL: FACEMAKER 0x0041e410
int g_face_group_count;

// GLOBAL: FACEMAKER 0x0041e420
char g_face_button_text_buffer[0x80];

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
int g_worker_should_exit;

// GLOBAL: FACEMAKER 0x0041e500
EncodedImage *g_face_control0_sprites[3];

// GLOBAL: FACEMAKER 0x0041e50c
int g_face_composite_ready;

// GLOBAL: FACEMAKER 0x0041e510
EncodedImage *g_face_control2_sprites[4];

// GLOBAL: FACEMAKER 0x0041e520
EncodedImage *g_face_control4_sprites[2];

// GLOBAL: FACEMAKER 0x0041e530
int g_face_group_frame_counts[10];

// GLOBAL: FACEMAKER 0x0041e558
EncodedImage *g_face_control3_sprites[2];

// GLOBAL: FACEMAKER 0x0041e560
int g_ignore_mouse_hit_test;

// GLOBAL: FACEMAKER 0x0041e570
char *g_face_set_names[1000];

// GLOBAL: FACEMAKER 0x0041e0c0
int g_face_list_scroll_offset;

// GLOBAL: FACEMAKER 0x0041e0d0
FaceSpriteEntryStorage g_face_sprite_entries;

// GLOBAL: FACEMAKER 0x0041e564
int g_worker_exit_code;

extern DIBSurface *g_graphics_pages[10];
extern int g_graphics_height;
extern int g_graphics_width;

// GLOBAL: FACEMAKER 0x00423338
int g_mouse_y;

// GLOBAL: FACEMAKER 0x0042333c
int g_mouse_x;

// GLOBAL: FACEMAKER 0x00423334
int g_mouse_button_down;

// GLOBAL: FACEMAKER 0x00423340
int g_mouse_button_mask;

// GLOBAL: FACEMAKER 0x0040c62c
char g_text_write_mode[] = "wt";

// GLOBAL: FACEMAKER 0x0040c658
char g_text_read_mode[] = "rt";

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
char g_text_read_mode_alt[] = "rt";

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

// GLOBAL: FACEMAKER 0x00412088
char g_open_file_filter_buffer[0x400];

// FUNCTION: FACEMAKER 0x00406253
void InitOpenFileDialog(HWND owner_hwnd, OPENFILENAMEA *open_file, char *initial_file, char *pattern,
                        char *file_buffer)
{
  memset(g_open_file_filter_buffer, 0, 0x400);
  strcpy(g_open_file_filter_buffer, initial_file);
  strcpy(g_open_file_filter_buffer + strlen(g_open_file_filter_buffer) + 1, pattern);
  open_file->lStructSize = 0x4c;
  open_file->hwndOwner = owner_hwnd;
  open_file->hInstance = (HINSTANCE)0;
  open_file->lpstrFilter = g_open_file_filter_buffer;
  open_file->lpstrCustomFilter = (LPSTR)0;
  open_file->nMaxCustFilter = 0;
  open_file->nFilterIndex = 0;
  open_file->lpstrFile = file_buffer;
  open_file->nMaxFile = 0x104;
  open_file->lpstrFileTitle = (LPSTR)0;
  open_file->nMaxFileTitle = 0x200;
  open_file->lpstrInitialDir = s_PlayFace_0040cad4;
  open_file->lpstrTitle = g_save_dialog_title;
  open_file->Flags = 0x6000c;
  open_file->nFileOffset = 0;
  open_file->nFileExtension = 0;
  open_file->lpstrDefExt = pattern + 2;
  open_file->lCustData = 0;
  open_file->lpfnHook = (LPOFNHOOKPROC)0;
  open_file->lpTemplateName = (LPCSTR)0;
}

// FUNCTION: FACEMAKER 0x00406369
int ShowSaveFaceDialog(void)
{
  struct
  {
    OPENFILENAMEA open_file;
    int save_result;
  } s;

  strcpy(g_face_output_path, s___pic_0040cae0);
  InitOpenFileDialog(g_main_hwnd, &s.open_file, g_default_face_filename, s___pic_0040cae8, g_face_output_path);
  s.save_result = GetSaveFileNameA(&s.open_file);
  g_mouse_button_down = 0;
  if (s.save_result != 0)
    return 1;
  else
    return 0;
}

// FUNCTION: FACEMAKER 0x00402600
unsigned int ScaleUiCoordinate(int value)
{
  return (unsigned int)((global_screen_width * value) / 0x280);
}

// FUNCTION: FACEMAKER 0x00402621
void BlitScaledRect(FacemakerWindowBounds *dst, int dst_x, int dst_y, int width, int height, FacemakerWindowBounds *src, int src_x,
                    int src_y)
{
  BlitGraphicsRect(dst, ScaleUiCoordinate(dst_x), ScaleUiCoordinate(dst_y), ScaleUiCoordinate(width),
                   ScaleUiCoordinate(height), src, ScaleUiCoordinate(src_x), ScaleUiCoordinate(src_y));
}

// FUNCTION: FACEMAKER 0x0040268a
void DrawEncodedImageAutoScale(FacemakerWindowBounds *dst, int x, int y, EncodedImage *encoded_image)
{
  EncodedImage *local_encoded_image;

  local_encoded_image = encoded_image;
  if (global_screen_width == 640)
  {
    DrawEncodedImageUnscaled(dst, x, y, encoded_image);
  }
  else
  {
    DrawEncodedImageResampled(dst, ScaleUiCoordinate(x), ScaleUiCoordinate(y),
                              ScaleUiCoordinate((int)local_encoded_image->width),
                              ScaleUiCoordinate((int)local_encoded_image->height), encoded_image);
  }
}

// FUNCTION: FACEMAKER 0x004093d0
#pragma optimize("gty", on)
int InitializeLegacyVideoStub(void)
{
  return -1;
}

// FUNCTION: FACEMAKER 0x004093e0
int CleanupLegacyVideoStub(void)
{
  return -1;
}
#pragma optimize("", on)

// FUNCTION: FACEMAKER 0x0040369c
int InitializeFaceMakerAssets(void)
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

  local.file = fopen(s_FaceButtons_txt_0040c65c, g_text_read_mode);
  fscanf(local.file, s________c_0040c66c, g_save_dialog_title);
  fscanf(local.file, s________c_0040c678, g_default_face_filename);
  for (local.loop_index = 0; local.loop_index < 4; local.loop_index = local.loop_index + 1)
  {
    fscanf(local.file, s________c_0040c684, g_face_button_text_buffer + local.loop_index * 0x20);
  }

  fscanf(local.file, s__d__d__d__d__d_0040c690, local.button_text_ids, local.style_values,
         local.style_values + 1, local.style_values + 2, local.style_values + 3);
  fclose(local.file);

  BeginSpriteEncodeSession();
  LoadPcxIntoPageNoPalette(2, s_FaceArt_facebut1_pcx_0040c6a0);
  g_face_fullscreen_bounds->font_slot = local.button_text_ids[0];

  for (local.loop_index = 0; local.loop_index < 4;
       local.loop_index = local.loop_index + 1, local.control_slot++)
  {
    local.face_slot_x = (local.control_slot % 5) * 0x70;
    local.style_values[5] = (local.control_slot / 5) * 0x60;
    SetFontStyleSize(local.button_text_ids[0], local.style_values[local.loop_index]);
    DrawTextAt(g_face_fullscreen_bounds, local.button_text_ids[local.loop_index + 1],
               local.face_slot_x + 0x2b, local.style_values[5] + 0xe, g_face_button_text_buffer + 0x20);
    g_face_control1_sprites[local.loop_index] =
        EncodeSpriteFromPage(2, local.face_slot_x, local.style_values[5], 0x56, 0x21);
    g_face_controls[1].page_by_mode[local.loop_index] = g_face_control1_sprites[local.loop_index];
  }

  for (local.loop_index = 0; local.loop_index < 4;
       local.loop_index = local.loop_index + 1, local.control_slot++)
  {
    local.face_slot_x = (local.control_slot % 5) * 0x70;
    local.style_values[5] = (local.control_slot / 5) * 0x60;
    SetFontStyleSize(local.button_text_ids[0], local.style_values[local.loop_index]);
    DrawTextAt(g_face_fullscreen_bounds, local.button_text_ids[local.loop_index + 1],
               local.face_slot_x + 0x2b, local.style_values[5] + 0xe, g_face_button_text_buffer);
    g_face_control2_sprites[local.loop_index] =
        EncodeSpriteFromPage(2, local.face_slot_x, local.style_values[5], 0x56, 0x21);
    g_face_controls[2].page_by_mode[local.loop_index] = g_face_control2_sprites[local.loop_index];
  }

  for (local.loop_index = 0; local.loop_index < 4;
       local.loop_index = local.loop_index + 1, local.control_slot++)
  {
    local.face_slot_x = (local.control_slot % 5) * 0x70;
    local.style_values[5] = (local.control_slot / 5) * 0x60;
    g_face_control_odd_sprites[local.loop_index] =
        EncodeSpriteFromPage(2, local.face_slot_x, local.style_values[5], 0x25, 0x1d);
    for (local.style_values[4] = 5; local.style_values[4] < 0x14;
         local.style_values[4] = local.style_values[4] + 2)
    {
      g_face_controls[local.style_values[4]].page_by_mode[local.loop_index] =
          g_face_control_odd_sprites[local.loop_index];
    }
  }

  for (local.loop_index = 0; local.loop_index < 4;
       local.loop_index = local.loop_index + 1, local.control_slot++)
  {
    local.face_slot_x = (local.control_slot % 5) * 0x70;
    local.style_values[5] = (local.control_slot / 5) * 0x60;
    g_face_control_even_sprites[local.loop_index] =
        EncodeSpriteFromPage(2, local.face_slot_x, local.style_values[5], 0x25, 0x1d);
    for (local.style_values[4] = 6; local.style_values[4] < 0x16;
         local.style_values[4] = local.style_values[4] + 2)
    {
      g_face_controls[local.style_values[4]].page_by_mode[local.loop_index] =
          g_face_control_even_sprites[local.loop_index];
    }
  }

  for (local.loop_index = 0; local.loop_index < 3;
       local.loop_index = local.loop_index + 1, local.control_slot++)
  {
    local.face_slot_x = (local.control_slot % 5) * 0x70;
    local.style_values[5] = (local.control_slot / 5) * 0x60;
    g_face_control0_sprites[local.loop_index] =
        EncodeSpriteFromPage(2, local.face_slot_x, local.style_values[5], 0x40, 0x20);
    g_face_controls[0].page_by_mode[local.loop_index] = g_face_control0_sprites[local.loop_index];
  }

  for (local.loop_index = 0; local.loop_index < 2;
       local.loop_index = local.loop_index + 1, local.control_slot++)
  {
    local.face_slot_x = (local.control_slot % 5) * 0x70;
    local.style_values[5] = (local.control_slot / 5) * 0x60;
    SetFontStyleSize(local.button_text_ids[0], local.style_values[local.loop_index]);
    DrawTextAt(g_face_fullscreen_bounds, local.button_text_ids[local.loop_index + 1],
               local.face_slot_x + 0x20, local.style_values[5] + 0xe, g_face_button_text_buffer + 0x40);
    g_face_control3_sprites[local.loop_index] =
        EncodeSpriteFromPage(2, local.face_slot_x, local.style_values[5], 0x40, 0x1d);
    g_face_controls[3].page_by_mode[local.loop_index] = g_face_control3_sprites[local.loop_index];
  }

  for (local.loop_index = 0; local.loop_index < 2;
       local.loop_index = local.loop_index + 1, local.control_slot++)
  {
    local.face_slot_x = (local.control_slot % 5) * 0x70;
    local.style_values[5] = (local.control_slot / 5) * 0x60;
    SetFontStyleSize(local.button_text_ids[0], local.style_values[local.loop_index]);
    DrawTextAt(g_face_fullscreen_bounds, local.button_text_ids[local.loop_index + 1],
               local.face_slot_x + 0x20, local.style_values[5] + 0xe, g_face_button_text_buffer + 0x60);
    g_face_control4_sprites[local.loop_index] =
        EncodeSpriteFromPage(2, local.face_slot_x, local.style_values[5], 0x40, 0x1d);
    g_face_controls[4].page_by_mode[local.loop_index] = g_face_control4_sprites[local.loop_index];
  }

  local.face_slot_x = (local.control_slot % 5) * 0x70;
  local.style_values[5] = (local.control_slot / 5) * 0x60;
  local.control_slot++;
  g_face_scroll_arrow_sprite = EncodeSpriteFromPage(2, local.face_slot_x, local.style_values[5], 9, 0x1d);

  local.face_slot_x = (local.control_slot % 5) * 0x70;
  local.style_values[5] = (local.control_slot / 5) * 0x60;
  local.control_slot++;
  g_face_scroll_divider_sprite =
      EncodeSpriteFromPage(2, local.face_slot_x, local.style_values[5], 3, 0x1d);

  LoadPcxIntoPageNoPalette(2, s_FaceArt_facfrsh2New_pcx_0040c6b8);
  g_face_slot_background_sprite = EncodeSpriteFromPage(2, 0, 0, 0x8a, 0xac);
  FinalizeSpriteEncodeSession();

  local.tmp_x = (int)fopen(s_FaceData_txt_0040c6d4, g_text_read_mode_alt);
  local.tmp_y = _filelength(_fileno((FILE *)local.tmp_x));
  g_face_set_display_names[0] = malloc(local.tmp_y);
  local.scan_ptr = g_face_set_display_names[0];
  fread(local.scan_ptr, local.tmp_y, 1, (FILE *)local.tmp_x);
  fclose((FILE *)local.tmp_x);

  g_face_set_total = 1;
  g_face_set_display_names[0] = local.scan_ptr;
  local.scan_ptr = local.scan_ptr + 1;
  for (local.loop_index = 1; local.loop_index < local.tmp_y;
       local.loop_index = local.loop_index + 1, local.scan_ptr = local.scan_ptr + 1)
  {
    if (*local.scan_ptr == '\n')
    {
      *local.scan_ptr = '\0';
      if (local.scan_ptr[1] != '\n')
      {
        g_face_set_display_names[g_face_set_total] = local.scan_ptr + 1;
        g_face_set_total = g_face_set_total + 1;
      }
    }
  }

  g_face_set_total = g_face_set_total - 1;
  for (local.loop_index = 0; local.loop_index < g_face_set_total; local.loop_index = local.loop_index + 1)
  {
    local.scan_ptr = g_face_set_display_names[local.loop_index];
    while (*local.scan_ptr != ',')
    {
      local.scan_ptr = local.scan_ptr + 1;
    }
    *local.scan_ptr = '\0';
    local.scan_ptr = local.scan_ptr + 1;
    while (*local.scan_ptr == ' ' || *local.scan_ptr == '\t')
    {
      local.scan_ptr = local.scan_ptr + 1;
    }
    g_face_set_names[local.loop_index] = local.scan_ptr;
    while (*local.scan_ptr != ',' && *local.scan_ptr != ' ' && *local.scan_ptr != '\t')
    {
      local.scan_ptr = local.scan_ptr + 1;
    }
    local.char_temp = *local.scan_ptr;
    *local.scan_ptr = '\0';
    local.scan_ptr = local.scan_ptr + 1;
    while (*local.scan_ptr == ' ' || *local.scan_ptr == '\t')
    {
      local.scan_ptr = local.scan_ptr + 1;
    }
    g_face_set_gender_tags[local.loop_index] = local.scan_ptr;
  }

  return 0;
}

// FUNCTION: FACEMAKER 0x00402717
void DrawFaceLayerUnscaled(FacemakerWindowBounds *page, int x, int y, EncodedImage *encoded_image)
{
  EncodedImage *tmp = encoded_image;
  DrawEncodedImageUnscaled(page, x, y, encoded_image);
}

// FUNCTION: FACEMAKER 0x00402743
void DrawTransparentOverlayMask(FacemakerWindowBounds *page, int x, int y, unsigned int width, int height)
{
  struct
  {
    int row;
    int pixel;
    unsigned char row_pixels[256];
  } local;

  for (local.row = 0; local.row < height; local.row = local.row + 1)
  {
    ReadGraphicsScanline((unsigned int *)local.row_pixels, page->page_number, x, local.row + y, width);
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
    WriteGraphicsScanline((unsigned int *)local.row_pixels, page->page_number, width + x, local.row + y, width);
  }
}

// FUNCTION: FACEMAKER 0x00402edc
int LoadSelectedFaceSet(int selected_face_index)
{
  int face_slot_x;
  int face_slot_y;
  int control_index;
  char face_art_path[260];

  if (g_face_sprite_roots.first != (EncodedImage *)0)
  {
    FreeSpriteBlob(g_face_sprite_roots.first);
  }

  strcpy(face_art_path, "FaceArt/");
  strcat(face_art_path, g_face_set_names[selected_face_index]);
  g_face_group_count =
      LoadFaceSpriteSet(face_art_path, g_face_group_frame_counts, g_face_sprite_entries.flat,
                        g_face_sprite_roots.flat);

  if (g_face_group_count <= 6)
  {
    DisableFaceControl(&g_face_controls[7]);
    DisableFaceControl(&g_face_controls[8]);
    for (control_index = 9; control_index < g_face_group_count * 2 + 9;
         control_index = control_index + 1)
    {
      if ((control_index & 1) != 0)
      {
        EnableFaceControl(&g_face_controls[control_index]);
      }
      else if (g_face_group_frame_counts[(control_index - 9) / 2] == 1)
      {
        DisableFaceControl(&g_face_controls[control_index]);
      }
      else
      {
        EnableFaceControl(&g_face_controls[control_index]);
      }
    }
    for (; control_index < 0x15; control_index = control_index + 1)
    {
      DisableFaceControl(&g_face_controls[control_index]);
    }
  }
  else
  {
    for (control_index = 9; control_index < 0x15; control_index = control_index + 1)
    {
      if ((control_index & 1) != 0)
      {
        EnableFaceControl(&g_face_controls[control_index]);
      }
      else if (g_face_group_frame_counts[(control_index - 9) / 2] == 1)
      {
        DisableFaceControl(&g_face_controls[control_index]);
      }
      else
      {
        EnableFaceControl(&g_face_controls[control_index]);
      }
    }
    DisableFaceControl(&g_face_controls[7]);
    EnableFaceControl(&g_face_controls[8]);
  }

  for (control_index = 0; control_index < 0x15; control_index = control_index + 1)
  {
    DrawFaceControl(&g_face_controls[control_index], 0);
  }
  memset(g_face_variant_index, 0, sizeof(g_face_variant_index));
  g_face_list_scroll_offset = 0;

  for (control_index = 0; control_index < MIN(g_face_group_count, 6);
       control_index = control_index + 1)
  {
    DrawFaceSlotEntry(control_index, 0);
  }

  for (; control_index < 6; control_index = control_index + 1)
  {
    face_slot_x = ((control_index + 2) % 4) * 0x9f + 0xd;
    face_slot_y = ((control_index + 2) / 4) * 0xdf + 0x24;
    DrawEncodedImageAutoScale(g_face_work_bounds, face_slot_x, face_slot_y,
                              g_face_slot_background_sprite);
    BlitScaledRect(g_face_work_bounds, face_slot_x, face_slot_y, 0x89, 0xac,
                   g_face_preview_bounds, face_slot_x, face_slot_y);
  }
  RedrawFaceCompositePreview();
  return selected_face_index;
}

// FUNCTION: FACEMAKER 0x00402841
void AcceptFaceSelection(FaceControl *control)
{
  FILE *recent_face_file;

  if (g_face_composite_ready == 0 && g_use_desktop_palette == 0)
  {
    ComposeSelectedFaceImage(control);
  }

  recent_face_file = fopen("FaceMostRecent.txt", g_text_write_mode);
  if (recent_face_file != (FILE *)0)
  {
    fprintf(recent_face_file, "%d, %s", g_selected_face_set_index,
            g_face_set_names[g_selected_face_set_index]);
    fclose(recent_face_file);
  }
  g_worker_should_exit = 1;
}

// FUNCTION: FACEMAKER 0x00402a42
int CycleFaceSet(FaceControl *control)
{
  g_selected_face_set_index =
      (control->value_index + g_selected_face_set_index + g_face_set_total) % g_face_set_total;
  LoadSelectedFaceSet(g_selected_face_set_index);
  return g_selected_face_set_index;
}

// FUNCTION: FACEMAKER 0x00402b24
int ScrollFaceSetList(FaceControl *control)
{
  int control_index;
  int visible_face_index;

  if (g_face_group_count <= 6)
  {
    return 0;
  }

  g_face_list_scroll_offset = g_face_list_scroll_offset + control->value_index;
  g_face_list_scroll_offset =
      (g_face_list_scroll_offset <= 0) ? 0 : g_face_list_scroll_offset;
  g_face_list_scroll_offset = MIN(g_face_group_count - 6, g_face_list_scroll_offset);

  for (control_index = 9; control_index < 0x15; control_index = control_index + 2)
  {
    visible_face_index = g_face_list_scroll_offset + (control_index - 9) / 2;
    if (g_face_variant_index[visible_face_index] == -1)
    {
      DisableFaceControl(&g_face_controls[control_index]);
    }
    else
    {
      EnableFaceControl(&g_face_controls[control_index]);
    }

    if (g_face_group_frame_counts[visible_face_index] - 1 == g_face_variant_index[visible_face_index])
    {
      DisableFaceControl(&g_face_controls[control_index + 1]);
    }
    else
    {
      EnableFaceControl(&g_face_controls[control_index + 1]);
    }
  }

  for (control_index = 0; control_index < 6; control_index = control_index + 1)
  {
    DrawFaceSlotEntry(control_index, 0);
  }

  if (g_face_list_scroll_offset == 0)
  {
    DisableFaceControl(&g_face_controls[7]);
    EnableFaceControl(&g_face_controls[8]);
  }
  else if (g_face_group_count - 6 == g_face_list_scroll_offset)
  {
    DisableFaceControl(&g_face_controls[8]);
    EnableFaceControl(&g_face_controls[7]);
  }
  else
  {
    EnableFaceControl(&g_face_controls[7]);
    EnableFaceControl(&g_face_controls[8]);
  }

  return g_face_list_scroll_offset;
}

// FUNCTION: FACEMAKER 0x00402d30
int AdjustVisibleFaceVariant(FaceControl *control)
{
  int visible_face_index = control->value_index + g_face_list_scroll_offset;
  int next_variant_index = control->value_delta;
  g_face_variant_index[visible_face_index] = g_face_variant_index[visible_face_index] + next_variant_index;

  g_face_variant_index[visible_face_index] = MAX(g_face_variant_index[visible_face_index], -1);

  g_face_variant_index[visible_face_index] = MIN(g_face_group_frame_counts[visible_face_index] - 1, g_face_variant_index[visible_face_index]);

  DrawFaceSlotEntry(visible_face_index - g_face_list_scroll_offset, 1);

  if (g_face_variant_index[visible_face_index] == -1)
  {
    FaceControl *sibling_control;
    DisableFaceControl(control);
    if (((int)(control - g_face_controls) & 1) != 0)
    {
      sibling_control = control + 1;
    }
    else
    {
      sibling_control = control - 1;
    }
    EnableFaceControl(sibling_control);
  }
  else if (g_face_group_frame_counts[visible_face_index] - 1 ==
           g_face_variant_index[visible_face_index])
  {
    FaceControl *sibling_control;
    DisableFaceControl(control);
    if (((int)(control - g_face_controls) & 1) != 0)
    {
      sibling_control = control + 1;
    }
    else
    {
      sibling_control = control - 1;
    }
    EnableFaceControl(sibling_control);
  }
  else
  {
    FaceControl *sibling_control;
    EnableFaceControl(control);
    if (((int)(control - g_face_controls) & 1) != 0)
    {
      sibling_control = control + 1;
    }
    else
    {
      sibling_control = control - 1;
    }
    EnableFaceControl(sibling_control);
  }

  return g_face_variant_index[visible_face_index];
}

// FUNCTION: FACEMAKER 0x004033c9
void ComposeSelectedFaceImage(FaceControl *control)
{
  int face_index;

  (void)control;

  ClearGraphicsPageWithPaletteColor(2, 0);
  DrawFaceLayerUnscaled(g_face_fullscreen_bounds, 0, 0, g_face_sprite_roots.first);
  for (face_index = 0; face_index < g_face_group_count; face_index = face_index + 1)
  {
    if (g_face_variant_index[face_index] >= 0 &&
        g_face_sprite_entries.groups[face_index][g_face_variant_index[face_index]] != (EncodedImage *)0)
    {
      DrawFaceLayerUnscaled(g_face_fullscreen_bounds, 0, 0,
                            g_face_sprite_entries.groups[face_index][g_face_variant_index[face_index]]);
    }
  }

  DrawTransparentOverlayMask(g_face_fullscreen_bounds, 0, 0, 0x8a, 0xaa);
  g_face_composite_ready = 1;
  if (g_use_desktop_palette != 0)
  {
    if (SendMessageA(g_main_hwnd, 0x111, 0x9c47, 0) != 0)
    {
      ExportEncodedImage(2, 0, 0, 0x114, 0xaa, 0, g_face_output_path);
    }
  }
  else
  {
    if (g_face_set_gender_tags[g_selected_face_set_index][0] == 'm')
    {
      PutGraphicsPixel(g_face_fullscreen_bounds, 0, 200, 0x6d);
    }
    else
    {
      PutGraphicsPixel(g_face_fullscreen_bounds, 0, 200, 0x66);
    }
    WriteGraphicsScanline((unsigned int *)g_face_set_display_names[g_selected_face_set_index], 2, 1, 200,
                          (unsigned int)strlen(g_face_set_display_names[g_selected_face_set_index]));
  }
}

// FUNCTION: FACEMAKER 0x00402a84
int DisableFaceControl(FaceControl *control)
{
  int old_mode;

  old_mode = control->state;
  g_ignore_mouse_hit_test = 1;
  control->state = 0;
  DrawFaceControl(control, 3);
  g_ignore_mouse_hit_test = 0;
  control->state = 3;
  return old_mode;
}

// FUNCTION: FACEMAKER 0x00402ad9
int EnableFaceControl(FaceControl *control)
{
  int old_mode;

  old_mode = control->state;
  control->state = 0;
  g_ignore_mouse_hit_test = 1;
  DrawFaceControl(control, 0);
  g_ignore_mouse_hit_test = 0;
  return old_mode;
}

// FUNCTION: FACEMAKER 0x004028d1
int DrawFaceControl(FaceControl *control, int mode)
{
  int in_bounds;

  if (g_ignore_mouse_hit_test == 0)
  {
    if (g_mouse_x < (int)ScaleUiCoordinate(control->x) ||
        (int)ScaleUiCoordinate(control->width + control->x) < g_mouse_x)
    {
      in_bounds = 0;
    }
    else if (g_mouse_y < (int)ScaleUiCoordinate(control->y) ||
             (int)ScaleUiCoordinate(control->y + control->height) < g_mouse_y)
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
    DrawEncodedImageAutoScale(g_face_preview_bounds, control->x, control->y,
                              control->page_by_mode[mode]);
  }

  if (control->page_by_mode[mode] != 0)
  {
    DrawEncodedImageAutoScale(g_face_work_bounds, control->x, control->y,
                              control->page_by_mode[mode]);
  }

  if (mode == 2 && control->on_activate != (FaceControlCallback)0)
  {
    control->on_activate(control);
  }
  return 1;
}

// FUNCTION: FACEMAKER 0x004032e4
void RedrawFaceCompositePreview(void)
{
  int face_index;

  DrawEncodedImageAutoScale(g_face_work_bounds, 0xd, 0x24, g_face_slot_background_sprite);
  DrawEncodedImageAutoScale(g_face_work_bounds, 0xd, 0x24, g_face_sprite_roots.first);
  for (face_index = 0; face_index < g_face_group_count; face_index++)
  {
    if (g_face_variant_index[face_index] >= 0 &&
        g_face_sprite_entries.groups[face_index][g_face_variant_index[face_index]] != (EncodedImage *)0)
    {
      DrawEncodedImageAutoScale(g_face_work_bounds, 0xd, 0x24,
                                g_face_sprite_entries.groups[face_index][g_face_variant_index[face_index]]);
    }
  }

  BlitScaledRect(g_face_work_bounds, 0xd, 0x24, 0x89, 0xa9, g_face_preview_bounds, 0xd, 0x24);
}

// FUNCTION: FACEMAKER 0x00403573
void DrawFaceSlotEntry(int index, int selected)
{
  int face_slot_x;
  int face_slot_y;

  face_slot_x = ((index + 2) % 4) * 0x9f + 0xd;
  face_slot_y = ((index + 2) / 4) * 0xdf + 0x24;
  DrawEncodedImageAutoScale(g_face_work_bounds, face_slot_x, face_slot_y,
                            g_face_slot_background_sprite);
  if (g_face_variant_index[index + g_face_list_scroll_offset] >= 0 &&
      g_face_sprite_entries.groups[index + g_face_list_scroll_offset]
                                  [g_face_variant_index[index + g_face_list_scroll_offset]] !=
          (EncodedImage *)0)
  {
    DrawEncodedImageAutoScale(g_face_work_bounds, face_slot_x, face_slot_y,
                              g_face_sprite_entries.groups[index + g_face_list_scroll_offset]
                                                          [g_face_variant_index[index + g_face_list_scroll_offset]]);
  }

  BlitScaledRect(g_face_work_bounds, face_slot_x, face_slot_y, 0x89, 0xab, g_face_preview_bounds,
                 face_slot_x, face_slot_y);
  if (selected != 0)
  {
    RedrawFaceCompositePreview();
  }
}

// FUNCTION: FACEMAKER 0x00403fd9
DWORD __cdecl FaceMakerWorkerThread(LPVOID unused)
{
  struct
  {
    int key_char;
    char *last_backslash;
    int page_number;
    int next_hover_button_index;
    int hover_button_index;
    int face_slot_y;
    int face_slot_x;
    char face_art_path[260];
    FILE *recent_face_file;
    char recent_face_name[256];
    DIBSurface *page;
    int button_index;
    int face_index;
  } local;

  (void)unused;

#ifdef _DEBUG
  // Allow debugging directly from output directory
  SetCurrentDirectoryA("C:\\Magic\\Program");
  if (getcwd(global_base_directory, sizeof(global_base_directory)) == NULL)
  {
    return 0;
  }
#else
  GetModuleFileNameA((HMODULE)0, global_base_directory, 0x105);
  SetCurrentDirectoryA(global_base_directory);
  local.last_backslash = strrchr(global_base_directory, '\\');
  *local.last_backslash = '\0';
#endif

  sprintf(g_face_output_path, "%s\\PlayFace\\*.pic", global_base_directory);
  LegacyInitNoop(LoadFontConfigIfPresent("mgraphic.exe", "fonts.cv"));
  GetPrimaryPage(0);

  local.face_index = 3;
  local.button_index = 0;
  for (; local.button_index < local.face_index; local.button_index++)
  {
    if (local.button_index == 1 && g_graphics_pages[0]->width < 800)
    {
      local.page = CreateGraphicsPage(local.button_index, 800, 600, 8);
    }
    else
    {
      local.page = GetPageByNumber(local.button_index);
    }
    SetGraphicsPage(local.button_index, local.page);
  }

  g_face_fullscreen_bounds->max_x = global_screen_width;
  g_face_work_bounds->max_x = g_face_fullscreen_bounds->max_x;
  g_face_preview_bounds->max_x = g_face_work_bounds->max_x;
  g_face_fullscreen_bounds->max_y = global_screen_height;
  g_face_work_bounds->max_y = g_face_fullscreen_bounds->max_y;
  g_face_preview_bounds->max_y = g_face_work_bounds->max_y;
  g_frontbuffer_direct_blit_enabled = 0;

  if (global_screen_width == 640)
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
  else if (global_screen_width == 1024)
  {
    LoadSystemFont(1, ScaleUiCoordinate(9), "tt0530m_.ttf", "Benguiat BkCn BT", 100, 0);
    LoadSystemFont(2, 10, "tt0298m_.ttf", "MPZurich Cn BT", 400, 0);
    LoadSystemFont(4, ScaleUiCoordinate(0xb), "tt0530m_.ttf", "Benguiat BkCn BT", 100, 0);
    LoadSystemFont(5, ScaleUiCoordinate(0x14), "tt0127m_.ttf", "Benguiat Bk BT", 700, 1);
    LoadSystemFont(6, ScaleUiCoordinate(0x1c), "tt0530m_.ttf", "Benguiat BkCn BT", 100, 0);
    LoadSystemFont(7, ScaleUiCoordinate(0xb), "tt0530m_.ttf", "Benguiat BkCn BT", 100, 0);
  }

  g_face_preview_bounds->font_slot = 1;
  PresentGraphicsPage(0);
  g_legacy_video_state = InitializeLegacyVideoStub();
  LoadPcxIntoPage(2, "FaceArt/facebk0New.pcx");
  StretchBlitGraphicsRect(g_face_fullscreen_bounds, 0, 0, 0x280, 0x1e0, g_face_work_bounds, 0, 0, global_screen_width,
                          global_screen_height);
  InitializeFaceMakerAssets();

  local.recent_face_file = fopen("FaceMostRecent.txt", "rt");
  if (local.recent_face_file != (FILE *)0)
  {
    fscanf(local.recent_face_file, "%d, %s", &g_selected_face_set_index,
           local.recent_face_name);
    if (_stricmp(g_face_set_names[g_selected_face_set_index],
                 local.recent_face_name) != 0)
    {
      g_selected_face_set_index = 0;
    }
    fclose(local.recent_face_file);
  }

  strcpy(local.face_art_path, "FaceArt/");
  strcat(local.face_art_path, g_face_set_names[g_selected_face_set_index]);
  g_face_group_count =
      LoadFaceSpriteSet(local.face_art_path, g_face_group_frame_counts,
                        g_face_sprite_entries.flat,
                        g_face_sprite_roots.flat);
  DisableFaceControl(&g_face_controls[7]);
  if (g_face_group_count <= 6)
  {
    DisableFaceControl(&g_face_controls[8]);
  }

  for (; local.button_index < 6; local.button_index++)
  {
    local.face_slot_x = ((local.button_index + 2) % 4) * 0x9f + 0xd;
    local.face_slot_y = ((local.button_index + 2) / 4) * 0xdf + 0x24;
    DrawEncodedImageAutoScale(g_face_work_bounds, local.face_slot_x,
                              local.face_slot_y,
                              g_face_slot_background_sprite);
    BlitScaledRect(g_face_work_bounds, local.face_slot_x, local.face_slot_y,
                   0x89, 0xa9, g_face_preview_bounds, local.face_slot_x,
                   local.face_slot_y);
  }

  for (local.button_index = 0; ((g_face_group_count < 6) ? g_face_group_count : 6) > local.button_index; local.button_index++)
  {
    DrawFaceSlotEntry(local.button_index, 0);
  }

  for (local.button_index = 0; local.button_index < 6; local.button_index++)
  {
    DrawEncodedImageAutoScale(g_face_work_bounds,
                              g_face_slot_arrow_xy[local.button_index][0],
                              g_face_slot_arrow_xy[local.button_index][1],
                              g_face_scroll_arrow_sprite);
  }

  for (local.button_index = 0; local.button_index < 4; local.button_index++)
  {
    DrawEncodedImageAutoScale(
        g_face_work_bounds,
        g_face_variant_arrow_xy[local.button_index][0],
        g_face_variant_arrow_xy[local.button_index][1],
        g_face_scroll_arrow_sprite);
  }

  g_face_preview_bounds->page_number = 1;
  g_ignore_mouse_hit_test = 1;

  for (local.button_index = 1; ((g_face_group_count < 6) ? g_face_group_count : 6) * 2 + 9 > local.button_index; local.button_index++)
  {
    DrawFaceControl(&g_face_controls[local.button_index], 0);
  }

  for (; local.button_index < 0x15; local.button_index++)
  {
    DrawFaceControl(&g_face_controls[local.button_index], 3);
  }

  DisableFaceControl(&g_face_controls[3]);
  DisableFaceControl(&g_face_controls[4]);
  g_face_controls[0].state = 3;
  g_ignore_mouse_hit_test = 0;
  g_face_preview_bounds->page_number = 0;
  PresentGraphicsPage(1);
  RedrawFaceCompositePreview();
  g_ui_ready = 1;

  local.next_hover_button_index = -1;
  local.hover_button_index = -1;
  do
  {
    if (local.hover_button_index >= 0)
    {
      if (g_mouse_x <
              (int)ScaleUiCoordinate(g_face_controls[local.hover_button_index]
                                         .x) ||
          (int)ScaleUiCoordinate(g_face_controls[local.hover_button_index].x +
                                 g_face_controls[local.hover_button_index]
                                     .width) < g_mouse_x)
      {
        local.key_char = 0;
      }
      else
      {
        if (g_mouse_y <
                (int)ScaleUiCoordinate(g_face_controls[local.hover_button_index]
                                           .y) ||
            (int)ScaleUiCoordinate(g_face_controls[local.hover_button_index].y +
                                   g_face_controls[local.hover_button_index]
                                       .height) < g_mouse_y)
        {
          local.key_char = 0;
        }
        else
        {
          local.key_char = 1;
        }
      }
      if (local.key_char == 0)
      {
        g_ignore_mouse_hit_test = 1;
        g_face_controls[local.hover_button_index].on_render(
            &g_face_controls[local.hover_button_index], 0);
        local.hover_button_index = -1;
        local.next_hover_button_index = local.hover_button_index;
        g_ignore_mouse_hit_test = 0;
      }
    }

    local.button_index = 0;
    for (; ((g_face_group_count < 6) ? g_face_group_count : 6) * 2 + 9 > local.button_index; local.button_index++)
    {
      if (local.hover_button_index != local.button_index &&
          g_face_controls[local.button_index].on_render(
              &g_face_controls[local.button_index], 0) != 0)
      {
        local.next_hover_button_index = local.button_index;
      }
    }

    if (local.hover_button_index != local.next_hover_button_index)
    {
      if (local.hover_button_index >= 0)
      {
        g_ignore_mouse_hit_test = 1;
        g_face_controls[local.hover_button_index].on_render(
            &g_face_controls[local.hover_button_index], 0);
        g_ignore_mouse_hit_test = 0;
      }
      g_face_controls[local.next_hover_button_index].on_render(
          &g_face_controls[local.next_hover_button_index], 1);
    }

    if (g_mouse_button_down == 1)
    {
      if (local.next_hover_button_index >= 0)
      {
        g_face_controls[local.next_hover_button_index].on_render(
            &g_face_controls[local.next_hover_button_index], 2);
      }
      ConsumeUiTickCount();
      while (g_mouse_button_down != 0)
      {
        if (PeekUiTickCount() >= 5)
        {
          if (local.next_hover_button_index >= 0)
          {
            g_face_controls[local.next_hover_button_index].on_render(
                &g_face_controls[local.next_hover_button_index], 2);
          }
          ConsumeUiTickCount();
        }
      }
      if (local.next_hover_button_index >= 0)
      {
        g_face_controls[local.next_hover_button_index].on_render(
            &g_face_controls[local.next_hover_button_index], 1);
      }
    }

    local.hover_button_index = local.next_hover_button_index;
    if (HasQueuedKeyInput() != 0)
    {
      local.page_number = PopQueuedKeyInput();
      if ((local.page_number & 0xff) == VK_ESCAPE)
      {
        g_worker_exit_code = -1;
        g_worker_should_exit = 1;
      }
    }
  } while (g_worker_should_exit == 0);

  if (g_legacy_video_state != 0)
  {
    CleanupLegacyVideoStub();
  }
  chdir(g_original_working_dir);
  SendMessageA(g_main_hwnd, 0x12, g_worker_exit_code, 0);
  ExitThread(g_worker_exit_code);
}

// FUNCTION: FACEMAKER 0x00404d70
int LoadFaceSpriteSet(char *base_path, int *group_frame_counts, EncodedImage **group_entries,
                      EncodedImage **first_sprite_out)
{
  struct
  {
    int tile_y;
    int tile_x;
    int frames_in_group;
    int tile_index;
    int group_count;
    long find_handle;
    char local_34c[260];
    int use_pcx_tiles;
    int saw_separator;
    long pcx_find_handle;
    unsigned char find_data[0x118];
    EncodedImage *current_sprite;
    char *group_suffix;
    int first_tile;
    unsigned char pcx_find_data[0x118];
  } face_loader;

  face_loader.group_count = 0;
  face_loader.saw_separator = 0;
  face_loader.frames_in_group = 0;
  face_loader.group_suffix = "1";
  face_loader.first_tile = 1;
  face_loader.use_pcx_tiles = 0;
  memset(group_entries, 0, 4);
  *face_loader.group_suffix = 'a';
  strcpy(face_loader.local_34c, base_path);
  face_loader.find_handle = _findfirst(face_loader.local_34c, (struct _finddata_t *)face_loader.find_data);
  strcpy(face_loader.local_34c, base_path);
  strcat(face_loader.local_34c, "*.pcx");
  face_loader.pcx_find_handle =
      _findfirst(face_loader.local_34c, (struct _finddata_t *)face_loader.pcx_find_data);
  if ((face_loader.pcx_find_handle == -1) && (face_loader.find_handle == -1))
  {
    return face_loader.group_count;
  }
  else if (face_loader.find_handle == -1)
  {
    face_loader.use_pcx_tiles = 1;
  }
  else if (face_loader.pcx_find_handle != -1)
  {
    do
    {
      if (*(int *)(face_loader.find_data + 0xc) < *(int *)(face_loader.pcx_find_data + 0xc))
      {
        face_loader.use_pcx_tiles = 1;
      }
    } while (_findnext(face_loader.pcx_find_handle, (struct _finddata_t *)face_loader.pcx_find_data) == 0);
  }
  if (face_loader.pcx_find_handle != -1)
  {
    _findclose(face_loader.pcx_find_handle);
  }
  if (face_loader.find_handle != -1)
  {
    _findclose(face_loader.find_handle);
  }
  strcpy(face_loader.local_34c, base_path);
  strcat(face_loader.local_34c, ".pcx");
  if (face_loader.use_pcx_tiles == 0)
  {
    return LoadSpriteGroupsFromFile(base_path, group_frame_counts, group_entries, first_sprite_out);
  }
  LoadPcxIntoPageNoPalette(2, face_loader.local_34c);
  ReplacePaletteIndexInRect(g_face_fullscreen_bounds, 0, 0, 0x22c, 0x158, 0x6d, 0);
  BeginSpriteEncodeSession();
  *first_sprite_out = EncodeSpriteFromPage(2, 0, 0, 0x89, 0xa9);
  face_loader.current_sprite = *first_sprite_out;
  face_loader.tile_index = 1;
  while (1)
  {
    do
    {
      face_loader.tile_x = (face_loader.tile_index % 4) * 0x8a;
      face_loader.tile_y = (face_loader.tile_index / 4) * 0xaa;
      if (IsSpriteTileEmpty(2, face_loader.tile_x, face_loader.tile_y, 0x8a, 0xaa) == 0)
      {
        ((EncodedImage * (*)[20]) group_entries)[face_loader.group_count][face_loader.frames_in_group] =
            EncodeSpriteFromPage(2, face_loader.tile_x, face_loader.tile_y, 0x89, 0xa9);
        face_loader.current_sprite = ((EncodedImage * (*)[20]) group_entries)[face_loader.group_count][face_loader.frames_in_group];
        face_loader.frames_in_group = face_loader.frames_in_group + 1;
        face_loader.current_sprite->top_clip = -1;
        face_loader.current_sprite->left_clip = face_loader.current_sprite->top_clip;
        face_loader.saw_separator = 0;
      }
      else if (face_loader.saw_separator != 0)
      {
        face_loader.current_sprite->top_clip = 0;
        goto DONE;
      }
      else if (face_loader.first_tile != 0)
      {
        face_loader.current_sprite->left_clip = 0;
        face_loader.tile_index++;
        face_loader.first_tile = 0;
        continue;
      }
      else
      {
        face_loader.current_sprite->left_clip = 0;
        group_frame_counts[face_loader.group_count] = face_loader.frames_in_group;
        face_loader.group_count = face_loader.group_count + 1;
        face_loader.frames_in_group = 0;
        face_loader.saw_separator = 1;
      }

      face_loader.first_tile = 0;
      face_loader.tile_index++;

    } while (face_loader.tile_index < 8);

    face_loader.tile_index = 0;
    strcpy(face_loader.local_34c, base_path);
    strcat(face_loader.local_34c, face_loader.group_suffix);
    strcat(face_loader.local_34c, ".pcx");
    ++*face_loader.group_suffix;
    face_loader.pcx_find_handle =
        _findfirst(face_loader.local_34c, (struct _finddata_t *)face_loader.pcx_find_data);
    if (face_loader.pcx_find_handle == -1)
    {
      break;
    }
    else
      _findclose(face_loader.pcx_find_handle);
    LoadPcxIntoPageNoPalette(2, face_loader.local_34c);
    ReplacePaletteIndexInRect(g_face_fullscreen_bounds, 0, 0, 0x22c, 0x158, 0x6d, 0);
  }

DONE:
  FinalizeSpriteEncodeSession();
  WriteSpriteBlob(*first_sprite_out, base_path);
  return face_loader.group_count;
}

// FUNCTION: FACEMAKER 0x00405261
int IsSpriteTileEmpty(int page_number, int x, int y, unsigned int width, int height)
{
  struct
  {
    int current_y;
    int remaining_height;
    int pixel_index;
    unsigned char row[1024];
  } tile_scan;

  (void)page_number;
  while (tile_scan.remaining_height = height--)
  {
    tile_scan.current_y = y;
    y = y + 1;
    ReadGraphicsScanline((unsigned int *)tile_scan.row, 2, x, tile_scan.current_y, width);
    for (tile_scan.pixel_index = 0; tile_scan.pixel_index < (int)width;
         tile_scan.pixel_index = tile_scan.pixel_index + 1)
    {
      if (tile_scan.row[tile_scan.pixel_index] != '\0')
      {
        return 0;
      }
    }
  }
  return 1;
}

// FUNCTION: FACEMAKER 0x0040530d
int LoadSpriteGroupsFromFile(char *sprite_path, int *group_frame_counts,
                             EncodedImage **group_entries, EncodedImage **first_sprite_out)
{
  struct
  {
    int group_count;
    int sprite_index;
    int frame_count;
    int table_total;
    EncodedImage *sprite_table[1000];
    EncodedImage *sprite_entry;
  } group_loader;

  group_loader.group_count = 0;
  group_loader.frame_count = 0;
  group_loader.table_total = ReadSpriteEntryPointers(group_loader.sprite_table, sprite_path);
  *first_sprite_out = group_loader.sprite_table[0];
  group_loader.sprite_index = 1;
  for (; group_loader.sprite_index < group_loader.table_total;
       group_loader.sprite_index = group_loader.sprite_index + 1)
  {
    group_loader.sprite_entry = group_loader.sprite_table[group_loader.sprite_index];
    ((EncodedImage * (*)[20]) group_entries)[group_loader.group_count][group_loader.frame_count] =
        group_loader.sprite_entry;
    group_loader.frame_count = group_loader.frame_count + 1;
    if (group_loader.sprite_entry->left_clip == 0)
    {
      if (group_loader.sprite_entry->top_clip == 0)
      {
        group_frame_counts[group_loader.group_count] = group_loader.frame_count;
        group_loader.group_count = group_loader.group_count + 1;
        break;
      }
    }
    if (group_loader.sprite_entry->left_clip == 0)
    {
      group_frame_counts[group_loader.group_count] = group_loader.frame_count;
      group_loader.group_count = group_loader.group_count + 1;
      group_loader.frame_count = 0;
    }
  }
  return group_loader.group_count;
}

// FUNCTION: FACEMAKER 0x0040542d
int ReplacePaletteIndexInRect(FacemakerWindowBounds *page, int x, int y, unsigned int width, int height,
                              unsigned int from_color, unsigned char to_color)
{
  int row_index;
  int pixel_index;
  unsigned char local_800[2048];

  for (row_index = 0; row_index < height; row_index = row_index + 1)
  {
    ReadGraphicsScanline((unsigned int *)local_800, page->page_number, x, row_index + y, width);
    for (pixel_index = 0; pixel_index < (int)width; pixel_index = pixel_index + 1)
    {
      if (local_800[pixel_index] == from_color)
      {
        local_800[pixel_index] = to_color;
      }
    }
    WriteGraphicsScanline((unsigned int *)local_800, page->page_number, x, row_index + y, width);
  }
  return height;
}

// FUNCTION: FACEMAKER 0x00405a49
LRESULT CALLBACK MainWindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  char face_art_path[528];

  switch (msg)
  {
  case WM_CREATE:
    g_main_timer_id = SetTimer(hwnd, 1, g_ui_timer_interval_ms, (TIMERPROC)0);
    break;

  case 0x12:
    AnimatePaletteToColor(0, 0x30);
    DestroyWindow(hwnd);
    PostQuitMessage(0);
    break;

  case WM_DESTROY:
    KillTimer(hwnd, g_main_timer_id);
    PostMessageA(hwnd, 0x12, 0, 0);
    break;

  case WM_TIMER:
    g_ui_tick_count = g_ui_tick_count + 1;
    break;

  case WM_SIZE:
    if (g_ui_ready != 0)
    {
      PresentGraphicsPage(1);
    }
    break;

  case WM_PAINT:
    if (g_ui_ready != 0)
    {
      PresentGraphicsPage(1);
    }
    break;

  case WM_MOUSEMOVE:
    g_mouse_x = (unsigned int)lparam & 0xffff;
    g_mouse_y = (unsigned short)(((unsigned int)lparam >> 0x10) & 0xffff);
    break;

  case WM_LBUTTONDOWN:
    g_mouse_button_mask = g_mouse_button_mask | 1;
    g_mouse_button_down = 1;
    g_mouse_x = (unsigned int)lparam & 0xffff;
    g_mouse_y = (unsigned short)(((unsigned int)lparam >> 0x10) & 0xffff);
    break;

  case WM_RBUTTONDOWN:
    g_mouse_button_mask = g_mouse_button_mask | 2;
    g_mouse_button_down = 2;
    g_mouse_x = (unsigned int)lparam & 0xffff;
    g_mouse_y = (unsigned short)(((unsigned int)lparam >> 0x10) & 0xffff);
    break;

  case WM_LBUTTONUP:
  case WM_RBUTTONUP:
    g_mouse_button_down = 0;
    break;

  case WM_KEYDOWN:
  case WM_SYSKEYDOWN:
    QueueKeyInputFromMessage(wparam, lparam);
    return DefWindowProcA(hwnd, msg, wparam, lparam);

  case WM_COMMAND:
    switch (wparam)
    {
    case 0x9c46:
      strcpy(face_art_path, s_FaceArt_evil_0040caa0);
      LoadFaceSpriteSet(face_art_path, g_face_group_frame_counts, g_face_sprite_entries.flat,
                        g_face_sprite_roots.flat);
      PresentGraphicsPage(1);
      break;

    case 0x9c47:
      return ShowSaveFaceDialog();

    case 0xffff:
      if (g_palette_class_registered == 0)
      {
        RegisterPaletteClass(g_app_instance);
        g_palette_class_registered = 1;
      }

      g_palette_hwnd = FindWindowExA((HWND)0, (HWND)0, "ShowPaletteClass", "Current Palette");
      if (g_palette_hwnd != (HWND)0)
      {
        BringWindowToTop(g_palette_hwnd);
      }
      else
      {
        g_palette_hwnd = CreatePalettePopupWindow(g_app_instance, (HWND)0);
        if (g_palette_hwnd == (HWND)0)
        {
          return 0;
        }
        ShowWindow(g_palette_hwnd, SW_SHOW);
      }
      UpdateWindow(g_palette_hwnd);
      break;
    }
    break;

  default:
    return DefWindowProcA(hwnd, msg, wparam, lparam);
  }

  return DefWindowProcA(hwnd, msg, wparam, lparam);
}

// FUNCTION: FACEMAKER 0x00405f25
void __stdcall IncrementUiTickCallback(int callback_arg_1, int callback_arg_2, int callback_arg_3,
                                       int callback_arg_4, int callback_arg_5)
{
  (void)callback_arg_1;
  (void)callback_arg_2;
  (void)callback_arg_3;
  (void)callback_arg_4;
  (void)callback_arg_5;
  g_ui_tick_count = g_ui_tick_count + 1;
}

// FUNCTION: FACEMAKER 0x00405f38
int ConsumeUiTickCount(void)
{
  int queued_ticks;

  queued_ticks = g_ui_tick_count;
  g_ui_tick_count = 0;
  return queued_ticks;
}

// FUNCTION: FACEMAKER 0x00405f60
int PeekUiTickCount(void)
{
  return g_ui_tick_count;
}

// FUNCTION: FACEMAKER 0x00405f75
int WaitForUiTicks(int wait_ticks)
{
  int start_tick_count;

  start_tick_count = g_ui_tick_count;
  while (g_ui_tick_count - start_tick_count < wait_ticks)
  {
  }
  return 0;
}

// FUNCTION: FACEMAKER 0x00405fa8
void RestoreSystemPalette(void)
{
  HDC hdc;

  if (g_use_desktop_palette == 0)
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

// FUNCTION: FACEMAKER 0x0040623c
void RestoreDisplayResolution(void)
{
  ChangeDisplayResolution(0, 0);
}

// FUNCTION: FACEMAKER 0x00405510
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int nShowCmd)
{
  char *slash;
  DWORD thread_id;
  MSG msg;
  // int horzres;
  //  unsigned int matched_mode;
  char module_path[256];
  int custom_mode_selected;
  WNDCLASSA wndclass;
  HWND main_hwnd;

  (void)prevInstance;

  custom_mode_selected = 0;
  g_main_hwnd = FindWindowA("FaceMakerMainClass", (LPCSTR)0);
  if (g_main_hwnd != (HWND)0)
  {
    ShowWindow(g_main_hwnd, nShowCmd);
    BringWindowToTop(g_main_hwnd);
    SetForegroundWindow(g_main_hwnd);
    return 0;
  }

  srand(GetTickCount());
  g_app_instance = hInstance;
  wndclass.style = 0x23;
  wndclass.lpfnWndProc = MainWindowProc;
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

  // lmfao it's a miracle this doesn't crash (nBufferLength == 0 so the write to address 0x100 is elided)
  GetCurrentDirectoryA(g_original_working_dir[0], 0x100);

  strcpy(module_path, (*__p___argv())[0]);
  slash = strrchr(module_path, '\\');
  *slash = '\0';
  if (_chdir(module_path) != 0)
  {
    assert_noabort(0, "D:\\NewMagic\\Sources\\1player\\FaceMaker\\code\\test.c", 0xa2,
                   "Path Set FAILED");
  }

  atexit(RestoreDisplayResolution);
  if (GetDeviceCaps(GetDC((HWND)0), HORZRES) > 1024)
  {
    ChangeDisplayResolution(1024, 768);
  }

LOOP:
  if (*cmdLine != '\0' && cmdLine[1] != '\0')
  {
    int matched_mode = 0;
    if (cmdLine[1] == 'S')
    {
      g_use_desktop_palette = 0;
    }

    if (cmdLine[1] == '6')
    {
      global_screen_width = 640;
      global_screen_height = 480;
      CreateGraphicsPage(0, global_screen_width, global_screen_height, -1);
      matched_mode = 1;
      custom_mode_selected = 1;
    }

    if (cmdLine[1] == '8')
    {
      global_screen_width = 800;
      global_screen_height = 600;
      CreateGraphicsPage(0, global_screen_width, global_screen_height, -1);
      matched_mode = 1;
      custom_mode_selected = 1;
    }

    if (cmdLine[1] == '1')
    {
      global_screen_width = 1024;
      global_screen_height = 768;
      CreateGraphicsPage(0, global_screen_width, global_screen_height, -1);
      matched_mode = 1;
      custom_mode_selected = 1;
    }

    if (matched_mode == 0)
    {
      *cmdLine = '\0';

      // TODO: did they actually use goto here? can't figure out a normal loop that mimics this
      goto LOOP;
    }

    g_face_preview_bounds->max_x = global_screen_width - 1;
    g_face_preview_bounds->max_y = global_screen_height - 1;
  }
  else
  {
    int horzres = GetDeviceCaps(GetDC((HWND)0), HORZRES);
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
#ifdef MODERN_FIXES
    default:
      global_screen_width = 1024; // GetDeviceCaps(GetDC((HWND)0), HORZRES) - 1;
      global_screen_height = 768; // GetDeviceCaps(GetDC((HWND)0), VERTRES) - 1;
      break;
#endif
    }
    g_face_preview_bounds->max_x = global_screen_width - 1;
    g_face_preview_bounds->max_y = global_screen_height - 1;
  }

  main_hwnd = CreateWindowExA(g_use_desktop_palette ? 0 : 8, "FacemakerMainClass",
                              "Magic: Facemaker", 0x80000000, 0, 0, global_screen_width,
                              global_screen_height, (HWND)0, (HMENU)0, hInstance, (LPVOID)0);
  g_main_hwnd = main_hwnd;
  ShowWindow(main_hwnd, nShowCmd);
  global_main_hdc = GetDC(main_hwnd);
  BringWindowToTop(g_main_hwnd);
  SetForegroundWindow(g_main_hwnd);
  SetFocus(g_main_hwnd);
  if (custom_mode_selected != 0)
  {
    g_graphics_pages[0]->hTempDC = global_main_hdc;
    SelectPalette(g_graphics_pages[0]->hTempDC, g_graphics_pages[0]->hPalette, FALSE);
    RealizePalette(g_graphics_pages[0]->hTempDC);
    SetStretchBltMode(g_graphics_pages[0]->hTempDC, 3);
  }

  SetSystemPaletteUse(global_main_hdc, 2);
  atexit(RestoreSystemPalette);
  CreateThread((LPSECURITY_ATTRIBUTES)0, 0, (LPTHREAD_START_ROUTINE)FaceMakerWorkerThread,
               (LPVOID)0, 0, &thread_id);
  while (GetMessageA(&msg, (HWND)0, 0, 0) != 0)
  {
    TranslateMessage(&msg);
    DispatchMessageA(&msg);
  }

  GdiFlush();
  return g_worker_exit_code;
}
