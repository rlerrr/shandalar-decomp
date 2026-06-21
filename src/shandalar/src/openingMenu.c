#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <process.h>

#include "defs.h"
#include "shandalar.h"
#include "facemaker/src/facemaker_types.h"
#include "drawcardlib/src/pic.h"

extern int global_screen_width;
extern int global_screen_height;
extern int g_graphics_bpp;
extern RpBitsPalettePacket g_palette_data_words;
extern int DAT_00589dec;
extern FILE *DAT_0078cf08;
extern char DAT_0078cf10[0x1000];
extern char DAT_0078df10[0x28];
extern int DAT_007898f4;
extern int DAT_007898f8;
extern int DAT_00986d94;
extern int DAT_00986d98;
extern int DAT_00986d9c;
extern int DAT_00747ee0;
extern char *gs_loadsave_0077d1b0[3];
extern HWND DAT_00748420;
extern int g_player_is_male;
extern int g_face_preview_sprite_group[6];
extern EncodedImage *g_face_preview_sprite_selected;
extern DIBSurface *g_graphics_pages[10];
extern char text_lines[249][300];

extern FacemakerWindowBounds *PTR_DAT_005832b4;
extern FacemakerWindowBounds *PTR_DAT_005832dc;
extern FacemakerWindowBounds *PTR_DAT_00583304;

int *LoadIniEscapedStringTable(FILE *ini_file, char *section_name, int unk1, int unk2);
int MeasureMultilineTextWidth(FacemakerWindowBounds *window, char *text);
int SetFontStyleSize(int font_id, unsigned int style);
int DrawTextFormatted(FacemakerWindowBounds *dst, int text_id, int draw_shadow, int scale_to_screen, int center_x, int center_y, int x,
                      int y, int *format_and_args);
int FUN_0056cc4d(const char *filename, const char *section);
int PopQueuedKeyInput(void);
void LoadPcxResource(int page_number, int x, int y, char *path, void *opaque);
void LoadPcxIntoPage(int page_number, char *path);
void LoadPcxIntoPageNoPalette(char *path);
void ClearGraphicsPageWithPaletteColor(int page_number, int color_index);
void FUN_00579bf0(FacemakerWindowBounds *src, int src_x, int src_y, unsigned int width, int height,
                  FacemakerWindowBounds *dst, int dst_x, int dst_y);
int *FUN_00579ea0(int *out_rect, FacemakerWindowBounds *page, int x, int y, int width, int height);
void BeginSpriteEncodeSession(void);
EncodedImage *EncodeSpriteFromPage(int page_number, int x, int y, int width, int height);
void FinalizeSpriteEncodeSession(void);
void FUN_005626b0(char *sound_path, int channel, int volume, int pan, int pitch);
void BlitGraphicsRect(FacemakerWindowBounds *dst, unsigned int dst_x, int dst_y, unsigned int width, DWORD height,
                      FacemakerWindowBounds *src, int src_x, int src_y);
void DrawEncodedImageResampled(FacemakerWindowBounds *dst, int x, int y, int width, int height, EncodedImage *encoded_image);
void StretchBlitGraphicsRect(FacemakerWindowBounds *dst, int dst_x, int dst_y, int src_w, int src_h,
                             FacemakerWindowBounds *src, int src_x, int src_y, int copy_w, int copy_h);
int FUN_00417dc6(const char *filename);
void ReadSpriteEntryPointers(void *out_sprite_entries, const char *sprite_table_name);
void FreeSpriteBlob(void *memory);
void AnimatePaletteToColor(int color_index, int palette_id);
void set_global_base_directory(char *path);
int FUN_00500321(void);
int FUN_005000fb(int menu_context);
int FUN_0050014e(AdvMenuControl *controls, int control_count, int menu_context);
int FUN_0050035e(void);
int FUN_0057ce70(int color_index, int palette_id);
void FUN_0057b530(FacemakerWindowBounds *window, int color, int x, int y, char *format, ...);
void FUN_0057b560(FacemakerWindowBounds *window, int color, int x, int y, char *format, ...);
int FUN_0057ae30(int font_slot);
void DrawLocalizedText(FacemakerWindowBounds *window, int color_index, int x, int y, ...);
void FUN_0057b4d0(FacemakerWindowBounds *window, int color, int x, int y, char *format, ...);
void FUN_0057c7e0(int page_number, char *path);
unsigned int FUN_005597ca(void);
int FUN_005003b7(int mouse_x, int mouse_y, int mouse_state);
int ScaleUiCoordinate(int value);
void *CreateGraphicsPage(int page_number, int width, int height, int bits_per_pixel);
void SetGraphicsPage(int page_number, void *page);
void ReadGraphicsScanline(unsigned int *param_1, int param_2, int param_3, int param_4, unsigned int param_5);
void WriteGraphicsScanline(unsigned int *param_1, int param_2, int param_3, int param_4, unsigned int param_5);
void FUN_005797e0(FacemakerWindowBounds *param_1, int param_2, int param_3, int param_4, int param_5, unsigned int param_6);
void FUN_005796c0(FacemakerWindowBounds *param_1, int param_2, int param_3, int param_4, int param_5, int param_6);
void FUN_004ce992(int param_1);
int FUN_004ce97d(void);
int FUN_0057aa30(int param_1, char param_2);
int FUN_0057adf0(int param_1);
void DrawLoadSaveButtonText(FacemakerWindowBounds *window, int color, int x, int y, ...);
int FUN_004ffcb4(AdvMenuControl *control);
int RenderAdvMenuControlNormally(AdvMenuControl *control);
int FUN_004ece40(int param_1);
int ShowHallBackgroundScreen(void);
int RunNameEntryDialog(char *name_buffer);
void BuildFacemakerPortraitSprites(FacemakerWindowBounds *page);
int BlitTransparentRuns(FacemakerWindowBounds *src, int src_x, int src_y, unsigned int width, int height, FacemakerWindowBounds *dst, int dst_x, int dst_y);
int ApplyNameEntryKey(char *name_buffer, unsigned int key_code, size_t max_len);
int DrawBlinkingNameCaret(FacemakerWindowBounds *window, int color_index, int caret_x, int caret_y, char *name_buffer, int cursor_pos);
int ApplyPortraitPaletteMap(FacemakerWindowBounds *page, int src_x, int src_y, unsigned int width, int height, char *palette_source_path, char *portrait_path);
int ApplyPortraitTintMap(FacemakerWindowBounds *page, int src_x, int src_y, unsigned int width, int height, unsigned int tint_mask, int tint_mode);
int HandleLoadSaveEscControlEvent(AdvMenuControl *control, int event_type);
int ActivateLoadSaveEscControl(AdvMenuControl *control);
int DrawLoadSaveButton(FacemakerWindowBounds *window, EncodedImage **button_sprites, int button_width, int button_height, char *button_text,
                 int font_slot, int visual_state);
int HandleLoadSaveSlotControlEvent(AdvMenuControl *control, int event_type);
int ActivateLoadSaveSlotControl(AdvMenuControl *control);

int HandleOpeningMenuControlEvent(AdvMenuControl *control, int event_type);
int HandleOpeningMenuControlActivate(AdvMenuControl *control);

int SetSelectedMenuValue(AdvMenuControl *control);
int HandleEscControlEvent(AdvMenuControl *control, int event_type);
int HandleDifficultyMenuControlEvent(AdvMenuControl *control, int event_type);
int DrawDifficultyMenuEntry(int difficulty_index, int visual_state);
int HandleDifficultyMenuControlActivate(AdvMenuControl *control);
int DrawColorMenuEntry(int color_index, int visual_state);
int HandleColorMenuControlEvent(AdvMenuControl *control, int event_type);
int HandleColorMenuControlActivate(AdvMenuControl *control);

// GLOBAL: SHANDALAR 0x005aa0c8
int g_opening_menu_unused_sprite_group_d[2];
// GLOBAL: SHANDALAR 0x005aa3f8
int g_opening_menu_unused_sprite_group_a[3];
// GLOBAL: SHANDALAR 0x005aa408
int g_opening_menu_unused_sprite_group_b[2];
// GLOBAL: SHANDALAR 0x005aa410
int g_opening_menu_sprite_blob_handle;
// GLOBAL: SHANDALAR 0x005aa424
int g_opening_menu_strings_loaded;
// GLOBAL: SHANDALAR 0x005aa430
int g_opening_menu_icon_state_sprites[4];
// GLOBAL: SHANDALAR 0x005aa440
int g_opening_menu_unused_sprite_group_c[3];
// GLOBAL: SHANDALAR 0x005a9d8c
int *g_opening_menu_text_table;
// GLOBAL: SHANDALAR 0x005a9d90
int g_opening_menu_unused_sprite_group_e[2];
// GLOBAL: SHANDALAR 0x00587378
AdvMenuRect g_opening_menu_entry_rects[4] = {{0xb8, 0x121, 0x109, 0x29}, {0xb8, 0x14a, 0x109, 0x29}, {0xb8, 0x173, 0x109, 0x29}, {0xb8, 0x19c, 0x109, 0x29}};
// GLOBAL: SHANDALAR 0x005873c8
int g_opening_menu_icon_size = 0x18;
// GLOBAL: SHANDALAR 0x005873cc
int g_opening_menu_entry_height = 0x29;
// GLOBAL: SHANDALAR 0x005873d0
int g_opening_menu_icon_label_spacing = 0x8c;
// GLOBAL: SHANDALAR 0x005873d8
int g_opening_menu_entry_enabled[4] = {1, 1, 1, 1};
// GLOBAL: SHANDALAR 0x00591228
char g_name_entry_buffer[0x40];
// GLOBAL: SHANDALAR 0x00781778
int g_name_entry_cursor;
// GLOBAL: SHANDALAR 0x00781774
int g_name_entry_insert_mode;
// GLOBAL: SHANDALAR 0x00789924
DIBSurface *g_facemaker_page4_dib;
// GLOBAL: SHANDALAR 0x00789928
HBITMAP g_facemaker_page4_bitmap;
// GLOBAL: SHANDALAR 0x007a0770
char g_player_name[0x40];

// GLOBAL: SHANDALAR 0x00587220
AdvMenuControl g_opening_menu_controls[4] = {
    {0xb8, 0x121, 0x115, 0x28, 0xb8, 0x121, 0x115, 0x28, 1, HandleOpeningMenuControlEvent, HandleOpeningMenuControlActivate, 1, 0, "Ss", (char *)0, 0, 0, {0, 0, 0, 0}},
    {0xb8, 0x14a, 0x115, 0x28, 0xb8, 0x14a, 0x115, 0x28, 1, HandleOpeningMenuControlEvent, HandleOpeningMenuControlActivate, 2, 0, "Ll", (char *)0, 0, 0, {0, 0, 0, 0}},
    {0xb8, 0x173, 0x115, 0x28, 0xb8, 0x173, 0x115, 0x28, 1, HandleOpeningMenuControlEvent, HandleOpeningMenuControlActivate, 3, 0, "rR", (char *)0, 0, 0, {0, 0, 0, 0}},
    {0xb8, 0x19c, 0x115, 0x28, 0xb8, 0x19c, 0x115, 0x28, 1, HandleOpeningMenuControlEvent, HandleOpeningMenuControlActivate, 5, 1, "Ee\x1b", "\x1b", 0, 0, {0, 0, 0, 0}}};

// GLOBAL: SHANDALAR 0x005873f0
AdvMenuControl g_difficulty_menu_controls[5] = {
    {0x118, 0x50, 0x10e, 0x54, 0x118, 0x50, 0x10e, 0x54, 1, HandleDifficultyMenuControlEvent, HandleDifficultyMenuControlActivate, 1, 0, (char *)0, (char *)0, 0, 0, {0, 0, 0, 0}},
    {0x118, 0xb4, 0x10e, 0x54, 0x118, 0xb4, 0x10e, 0x54, 1, HandleDifficultyMenuControlEvent, HandleDifficultyMenuControlActivate, 2, 0, (char *)0, (char *)0, 0, 0, {0, 0, 0, 0}},
    {0x118, 0x118, 0x10e, 0x54, 0x118, 0x118, 0x10e, 0x54, 1, HandleDifficultyMenuControlEvent, HandleDifficultyMenuControlActivate, 3, 0, (char *)0, (char *)0, 0, 0, {0, 0, 0, 0}},
    {0x118, 0x17c, 0x10e, 0x54, 0x118, 0x17c, 0x10e, 0x54, 1, HandleDifficultyMenuControlEvent, HandleDifficultyMenuControlActivate, 4, 0, (char *)0, (char *)0, 0, 0, {0, 0, 0, 0}},
    {-1, -1, -1, -1, -1, -1, -1, -1, 1, HandleEscControlEvent, SetSelectedMenuValue, 0, 0, "\x1b", "\x1b", 0, 0, {0, 0, 0, 0}}};

// GLOBAL: SHANDALAR 0x005875d8
AdvMenuControl g_color_menu_controls[6] = {
    {0x33, 0x43, 0x10e, 0x4b, 0x33, 0x43, 0x10e, 0x4b, 1, HandleColorMenuControlEvent, HandleColorMenuControlActivate, 1, 0, (char *)0, (char *)0, 0, 0, {0, 0, 0, 0}},
    {0x33, 0x8e, 0x10e, 0x4b, 0x33, 0x8e, 0x10e, 0x4b, 1, HandleColorMenuControlEvent, HandleColorMenuControlActivate, 2, 0, (char *)0, (char *)0, 0, 0, {0, 0, 0, 0}},
    {0x33, 0xd9, 0x10e, 0x4b, 0x33, 0xd9, 0x10e, 0x4b, 1, HandleColorMenuControlEvent, HandleColorMenuControlActivate, 3, 0, (char *)0, (char *)0, 0, 0, {0, 0, 0, 0}},
    {0x33, 0x124, 0x10e, 0x4b, 0x33, 0x124, 0x10e, 0x4b, 1, HandleColorMenuControlEvent, HandleColorMenuControlActivate, 4, 0, (char *)0, (char *)0, 0, 0, {0, 0, 0, 0}},
    {0x33, 0x16f, 0x10e, 0x4b, 0x33, 0x16f, 0x10e, 0x4b, 1, HandleColorMenuControlEvent, HandleColorMenuControlActivate, 5, 0, (char *)0, (char *)0, 0, 0, {0, 0, 0, 0}},
    {-1, -1, -1, -1, -1, -1, -1, -1, 1, HandleEscControlEvent, SetSelectedMenuValue, 0, 0, "\x1b", "\x1b", 0, 0, {0, 0, 0, 0}}};

// GLOBAL: SHANDALAR 0x00587598
AdvMenuRect g_difficulty_menu_preview_rects[4] = {{0x1c7, 0x4b, 0x7a, 0x5c}, {0x1c7, 0xaf, 0x7a, 0x5c}, {0x1c7, 0x113, 0x7a, 0x5c}, {0x1c7, 0x177, 0x7a, 0x5c}};

// GLOBAL: SHANDALAR 0x005877d0
AdvMenuRect g_color_menu_swatch_rects[5] = {{0x33, 0x43, 0x46, 0x4b}, {0x33, 0x8e, 0x46, 0x4b}, {0x33, 0xd9, 0x46, 0x4b}, {0x33, 0x124, 0x46, 0x4b}, {0x33, 0x16f, 0x46, 0x4b}};
// GLOBAL: SHANDALAR 0x00781710
EncodedImage *g_color_menu_normal_swatch_sprites[5];
// GLOBAL: SHANDALAR 0x007817d0
EncodedImage *g_color_menu_pressed_swatch_sprites[5];
// GLOBAL: SHANDALAR 0x005aa0c4
int g_difficulty_menu_strings_loaded;
// GLOBAL: SHANDALAR 0x005aa3f0
int g_color_menu_strings_loaded;
// GLOBAL: SHANDALAR 0x005aa420
int *g_difficulty_caption_table;
// GLOBAL: SHANDALAR 0x005aa428
int *g_color_menu_flavor_table;
// GLOBAL: SHANDALAR 0x005aa42c
int *g_color_menu_color_name_table;
// GLOBAL: SHANDALAR 0x005a9d88
int *g_difficulty_option_table;
// GLOBAL: SHANDALAR 0x005a9d98
int *g_color_caption_table;
// GLOBAL: SHANDALAR 0x00587370
int g_menu_selection_value = -1;
// GLOBAL: SHANDALAR 0x00587820
int g_color_menu_selection_to_deck_color[5] = {4, 5, 1, 3, 2};

// GLOBAL: SHANDALAR 0x00588a18
int g_loadsave_insert_mode;
// GLOBAL: SHANDALAR 0x005aa470
int g_loadsave_menu_selection;
// GLOBAL: SHANDALAR 0x005aa478
EncodedImage *g_loadsave_frame_sprites[9];
// GLOBAL: SHANDALAR 0x005aa4a0
int g_loadsave_slot_has_data[10];
// GLOBAL: SHANDALAR 0x005aa4c8
EncodedImage *g_loadsave_slot_button_state_sprites[6];
// GLOBAL: SHANDALAR 0x005aa4e0
int g_loadsave_cursor_index;
// GLOBAL: SHANDALAR 0x005aa4e4
int g_loadsave_edit_active;
// GLOBAL: SHANDALAR 0x0074ad20
int g_loadsave_skip_esc;
// GLOBAL: SHANDALAR 0x0074ad30
char g_loadsave_slot_descriptions[10][0x40];
// GLOBAL: SHANDALAR 0x00588dbc
char g_loadsave_esc_key[] = "\x1b";
// GLOBAL: SHANDALAR 0x00588dc0
char g_loadsave_esc_key_2[] = "\x1b";
// GLOBAL: SHANDALAR 0x00588dd8
char g_loadsave_magic_map_path[] = "magic4.map";
// GLOBAL: SHANDALAR 0x00588dfc
char g_loadsave_file_open_mode[] = "r+t";
// GLOBAL: SHANDALAR 0x00588e00
char g_loadsave_description_file_path[] = "saveDescs";
// GLOBAL: SHANDALAR 0x00588e0c
char g_loadsave_space_string[] = " ";
// GLOBAL: SHANDALAR 0x00588e10
char g_loadsave_description_line_format[] = "%s\n";

// GLOBAL: SHANDALAR 0x00588a20
AdvMenuControl g_loadsave_menu_controls[11] = {
    {0x2c, 0x55, 0x168, 0x1b, 0x2c, 0x55, 0x168, 0x1b, 1, HandleLoadSaveSlotControlEvent, ActivateLoadSaveSlotControl, 4, 0, (char *)0, (char *)0, 0, 0, {0, 0, 0, 0}},
    {0x2c, 0x73, 0x168, 0x1b, 0x2c, 0x73, 0x168, 0x1b, 1, HandleLoadSaveSlotControlEvent, ActivateLoadSaveSlotControl, 5, 0, (char *)0, (char *)0, 0, 0, {0, 0, 0, 0}},
    {0x2c, 0x91, 0x168, 0x1b, 0x2c, 0x91, 0x168, 0x1b, 1, HandleLoadSaveSlotControlEvent, ActivateLoadSaveSlotControl, 6, 0, (char *)0, (char *)0, 0, 0, {0, 0, 0, 0}},
    {0x2c, 0xaf, 0x168, 0x1b, 0x2c, 0xaf, 0x168, 0x1b, 1, HandleLoadSaveSlotControlEvent, ActivateLoadSaveSlotControl, 7, 0, (char *)0, (char *)0, 0, 0, {0, 0, 0, 0}},
    {0x2c, 0xcd, 0x168, 0x1b, 0x2c, 0xcd, 0x168, 0x1b, 1, HandleLoadSaveSlotControlEvent, ActivateLoadSaveSlotControl, 8, 0, (char *)0, (char *)0, 0, 0, {0, 0, 0, 0}},
    {0x2c, 0xeb, 0x168, 0x1b, 0x2c, 0xeb, 0x168, 0x1b, 1, HandleLoadSaveSlotControlEvent, ActivateLoadSaveSlotControl, 9, 0, (char *)0, (char *)0, 0, 0, {0, 0, 0, 0}},
    {0x2c, 0x109, 0x168, 0x1b, 0x2c, 0x109, 0x168, 0x1b, 1, HandleLoadSaveSlotControlEvent, ActivateLoadSaveSlotControl, 0xa, 0, (char *)0, (char *)0, 0, 0, {0, 0, 0, 0}},
    {0x2c, 0x127, 0x168, 0x1b, 0x2c, 0x127, 0x168, 0x1b, 1, HandleLoadSaveSlotControlEvent, ActivateLoadSaveSlotControl, 0xb, 0, (char *)0, (char *)0, 0, 0, {0, 0, 0, 0}},
    {0x2c, 0x145, 0x168, 0x1b, 0x2c, 0x145, 0x168, 0x1b, 1, HandleLoadSaveSlotControlEvent, ActivateLoadSaveSlotControl, 0xc, 0, (char *)0, (char *)0, 0, 0, {0, 0, 0, 0}},
    {0x2c, 0x163, 0x168, 0x1b, 0x2c, 0x163, 0x168, 0x1b, 1, HandleLoadSaveSlotControlEvent, ActivateLoadSaveSlotControl, 0xd, 0, (char *)0, (char *)0, 0, 0, {0, 0, 0, 0}},
    {-1, -1, -1, -1, -1, -1, -1, -1, 1, HandleLoadSaveEscControlEvent, ActivateLoadSaveEscControl, 0xe, 0, g_loadsave_esc_key, g_loadsave_esc_key_2, 0, 0, {0, 0, 0, 0}}};

// FUNCTION: SHANDALAR 0x004a04a0
int SetSelectedMenuValue(AdvMenuControl *control, int event_type)
{
  //This param is real but unused
  (void)event_type;

  g_menu_selection_value = control->selection_value;
  return 0;
}

// FUNCTION: SHANDALAR 0x004a04bd
int HandleEscControlEvent(AdvMenuControl *control, int event_type)
{
  if (event_type == 2)
  {
    SetSelectedMenuValue(control, event_type);
  }

  return 0;
}

// FUNCTION: SHANDALAR 0x004a04e9
int DrawOpeningMenuEntry(int entry_index, int visual_state)
{
  struct
  {
    int pressed_offset_y;
    int pressed_offset_x;
    int unused_label_offset_index;
    int icon_sprite_index;
    int icon_y;
    int icon_x;
    int label_x_0;
    int label_x_1;
    int label_x_2;
    int label_x_3;
    EncodedImage **unused_button_sprite_group;
    int text_table_index;
  } s;

  s.label_x_0 = 0x9c;
  s.label_x_1 = 0xbb;
  s.label_x_2 = 0xbb;
  s.label_x_3 = 0x40;

  PTR_DAT_005832b4->page_number = 1;
  BlitGraphicsRect(PTR_DAT_00583304, g_opening_menu_entry_rects[entry_index].x, g_opening_menu_entry_rects[entry_index].y,
                   global_screen_width - g_opening_menu_entry_rects[entry_index].x, ScaleUiCoordinate(0x2d), PTR_DAT_005832dc,
                   g_opening_menu_entry_rects[entry_index].x, g_opening_menu_entry_rects[entry_index].y);

  switch (entry_index)
  {
  case 0:
    s.unused_button_sprite_group = (EncodedImage **)&g_opening_menu_sprite_blob_handle;
    break;
  case 1:
    s.unused_button_sprite_group = (EncodedImage **)&g_opening_menu_unused_sprite_group_a;
    break;
  case 2:
    s.unused_button_sprite_group = (EncodedImage **)&g_opening_menu_unused_sprite_group_c;
    break;
  case 3:
    s.unused_button_sprite_group = (EncodedImage **)&g_opening_menu_unused_sprite_group_d;
    break;
  }

  if (3 <= entry_index)
  {
    s.text_table_index = entry_index + 1;
  }
  else
  {
    s.text_table_index = entry_index;
  }

  switch (visual_state)
  {
  case 0:
    s.unused_label_offset_index = 0;
    s.icon_sprite_index = 0;
    break;
  case 1:
    s.unused_label_offset_index = 1;
    s.icon_sprite_index = 1;
    break;
  case 2:
    s.unused_label_offset_index = 1;
    s.icon_sprite_index = 2;
    break;
  case 3:
    s.unused_label_offset_index = 2;
    s.icon_sprite_index = 3;
    break;
  }

  s.icon_x = g_opening_menu_entry_rects[entry_index].x + (g_opening_menu_entry_height - g_opening_menu_icon_size) / 2;
  s.icon_y = g_opening_menu_entry_rects[entry_index].y + (g_opening_menu_entry_height - g_opening_menu_icon_size) / 2;
  s.pressed_offset_x = ScaleUiCoordinate(2) / 2;
  s.pressed_offset_y = ScaleUiCoordinate(2) / 2;
  if (visual_state == 2)
  {
    DrawEncodedImageResampled(PTR_DAT_005832b4, (int)(*(volatile int *)&s.pressed_offset_x) + s.icon_x,
                              s.icon_y + s.pressed_offset_y, g_opening_menu_icon_size, g_opening_menu_icon_size,
                              (EncodedImage *)g_opening_menu_icon_state_sprites[s.icon_sprite_index]);
  }
  else
  {
    DrawEncodedImageResampled(PTR_DAT_005832b4, s.icon_x, s.icon_y, g_opening_menu_icon_size, g_opening_menu_icon_size,
                              (EncodedImage *)g_opening_menu_icon_state_sprites[s.icon_sprite_index]);
  }

  PTR_DAT_005832b4->font_slot = 6;
  FUN_0057b4d0(PTR_DAT_005832b4, (&s.label_x_0)[s.icon_sprite_index], s.icon_x + ScaleUiCoordinate(0x24),
               (s.icon_y + g_opening_menu_icon_size / 2) - (FUN_0057ae30(6) / 2),
               "%s",
               (char *)g_opening_menu_text_table[s.text_table_index]);

  PTR_DAT_005832b4->page_number = 0;
  BlitGraphicsRect(PTR_DAT_005832dc, g_opening_menu_entry_rects[entry_index].x, g_opening_menu_entry_rects[entry_index].y,
                   global_screen_width - g_opening_menu_entry_rects[entry_index].x, ScaleUiCoordinate(0x2d), PTR_DAT_005832b4,
                   g_opening_menu_entry_rects[entry_index].x, g_opening_menu_entry_rects[entry_index].y);
  (void)s.unused_button_sprite_group;
  return 0;
}

// FUNCTION: SHANDALAR 0x004a0834
int RunOpeningMenu(void)
{
  struct
  {
    int sprite_entry_index;
    int sprite_group_index;
    int copied_entry_count;
    int sprite_entry_pointers[100];
    int menu_context;
    char map_name_buffer[10];
    int loop_index;
  } s;

  if (g_opening_menu_strings_loaded == 0)
  {
    g_opening_menu_text_table = ((int *(__cdecl *)(FILE *, char *, int))LoadIniEscapedStringTable)(DAT_0078cf08, "openingMenu",
                                                                                       (int)DAT_0078df10);
    g_opening_menu_strings_loaded = 1;
  }

  SetFontStyleSize(6, ScaleUiCoordinate(0x14));

loop:
  strcpy(s.map_name_buffer, "magic3.map");
  g_menu_selection_value = -1;

  AnimatePaletteToColor(0, DAT_00589dec);
  LoadPcxResource(1, 0, 0, "menubak.pic",
                  (g_graphics_bpp == 8) ? &g_palette_data_words : (RpBitsPalettePacket *)1);

  StretchBlitGraphicsRect(PTR_DAT_005832dc, 0, 0, 0x280, 0x1e0, PTR_DAT_005832b4, 0, 0, global_screen_width,
                          global_screen_height);
  StretchBlitGraphicsRect(PTR_DAT_005832dc, 0, 0, 0x280, 0x1e0, PTR_DAT_00583304, 0, 0, global_screen_width,
                          global_screen_height);

  FUN_0057ce70(0, DAT_00589dec);
  PTR_DAT_005832b4->font_slot = 5;
  PTR_DAT_005832b4->font_slot = 1;

  g_opening_menu_entry_enabled[2] = FUN_00417dc6(s.map_name_buffer);
  for (s.loop_index = 4; s.loop_index < 0xe; s.loop_index = s.loop_index + 1)
  {
    if (s.loop_index >= 10)
    {
      s.map_name_buffer[5] = (char)(s.loop_index + 'W');
    }
    else
    {
      s.map_name_buffer[5] = (char)(s.loop_index + '0');
    }

    if (FUN_00417dc6(s.map_name_buffer) != 0)
    {
      break;
    }
  }

  if (s.loop_index == 0xe)
  {
    g_opening_menu_entry_enabled[1] = 0;
  }

  s.sprite_entry_index = 0;
  ReadSpriteEntryPointers(s.sprite_entry_pointers, "begin.spr");

  s.copied_entry_count = 0;
  for (s.sprite_group_index = 0; s.sprite_group_index < 2;
       s.copied_entry_count = s.copied_entry_count + 1, s.sprite_group_index = s.sprite_group_index + 1)
  {
    (&g_opening_menu_sprite_blob_handle)[s.sprite_group_index] = s.sprite_entry_pointers[s.sprite_entry_index];
    s.sprite_entry_index = s.sprite_entry_index + 1;
  }
  for (s.sprite_group_index = 0; s.sprite_group_index < 3;
       s.copied_entry_count = s.copied_entry_count + 1, s.sprite_group_index = s.sprite_group_index + 1)
  {
    g_opening_menu_unused_sprite_group_a[s.sprite_group_index] = s.sprite_entry_pointers[s.sprite_entry_index];
    s.sprite_entry_index = s.sprite_entry_index + 1;
  }
  for (s.sprite_group_index = 0; s.sprite_group_index < 3;
       s.copied_entry_count = s.copied_entry_count + 1, s.sprite_group_index = s.sprite_group_index + 1)
  {
    g_opening_menu_unused_sprite_group_c[s.sprite_group_index] = s.sprite_entry_pointers[s.sprite_entry_index];
    s.sprite_entry_index = s.sprite_entry_index + 1;
  }
  for (s.sprite_group_index = 0; s.sprite_group_index < 2;
       s.copied_entry_count = s.copied_entry_count + 1, s.sprite_group_index = s.sprite_group_index + 1)
  {
    g_opening_menu_unused_sprite_group_b[s.sprite_group_index] = s.sprite_entry_pointers[s.sprite_entry_index];
    s.sprite_entry_index = s.sprite_entry_index + 1;
  }
  for (s.sprite_group_index = 0; s.sprite_group_index < 2;
       s.copied_entry_count = s.copied_entry_count + 1, s.sprite_group_index = s.sprite_group_index + 1)
  {
    g_opening_menu_unused_sprite_group_d[s.sprite_group_index] = s.sprite_entry_pointers[s.sprite_entry_index];
    s.sprite_entry_index = s.sprite_entry_index + 1;
  }
  for (s.sprite_group_index = 0; s.sprite_group_index < 2;
       s.copied_entry_count = s.copied_entry_count + 1, s.sprite_group_index = s.sprite_group_index + 1)
  {
    g_opening_menu_unused_sprite_group_e[s.sprite_group_index] = s.sprite_entry_pointers[s.sprite_entry_index];
    s.sprite_entry_index = s.sprite_entry_index + 1;
  }
  for (s.sprite_group_index = 0; s.sprite_group_index < 4;
       s.copied_entry_count = s.copied_entry_count + 1, s.sprite_group_index = s.sprite_group_index + 1)
  {
    g_opening_menu_icon_state_sprites[s.sprite_group_index] = s.sprite_entry_pointers[s.sprite_entry_index];
    s.sprite_entry_index = s.sprite_entry_index + 1;
  }

  if (g_opening_menu_controls[0].x == g_opening_menu_controls[0].base_x)
  {
    for (s.loop_index = 0; s.loop_index < 4; s.loop_index = s.loop_index + 1)
    {
      g_opening_menu_entry_rects[s.loop_index].x = ScaleUiCoordinate(g_opening_menu_entry_rects[s.loop_index].x);
      g_opening_menu_entry_rects[s.loop_index].y = ScaleUiCoordinate(g_opening_menu_entry_rects[s.loop_index].y);
      g_opening_menu_entry_rects[s.loop_index].width = ScaleUiCoordinate(g_opening_menu_entry_rects[s.loop_index].width);
      g_opening_menu_entry_rects[s.loop_index].height = ScaleUiCoordinate(g_opening_menu_entry_rects[s.loop_index].height);
      g_opening_menu_controls[s.loop_index].x = ScaleUiCoordinate(g_opening_menu_controls[s.loop_index].x);
      g_opening_menu_controls[s.loop_index].y = ScaleUiCoordinate(g_opening_menu_controls[s.loop_index].y);
      g_opening_menu_controls[s.loop_index].width = ScaleUiCoordinate(g_opening_menu_controls[s.loop_index].width);
      g_opening_menu_controls[s.loop_index].height = ScaleUiCoordinate(g_opening_menu_controls[s.loop_index].height);
    }

    g_opening_menu_icon_size = ScaleUiCoordinate(g_opening_menu_icon_size);
    g_opening_menu_entry_height = ScaleUiCoordinate(g_opening_menu_entry_height);
    g_opening_menu_icon_label_spacing = ScaleUiCoordinate(g_opening_menu_icon_label_spacing);
  }

  s.menu_context = FUN_00500321();
  FUN_005000fb(s.menu_context);
  FUN_0050014e(g_opening_menu_controls, 4, s.menu_context);

  for (s.loop_index = 0; s.loop_index < 4; s.loop_index = s.loop_index + 1)
  {
    DrawOpeningMenuEntry(s.loop_index, ((g_opening_menu_entry_enabled[s.loop_index] == 0) ? 3 : 0));
    if (g_opening_menu_entry_enabled[s.loop_index] == 0)
    {
      FUN_004ffcb4(&g_opening_menu_controls[s.loop_index]);
    }
  }

  while (g_menu_selection_value == -1)
  {
    FUN_005003b7(DAT_007898f4, DAT_007898f8, DAT_00986d94);
  }

  FUN_0050035e();
  FUN_005000fb(s.menu_context);
  FreeSpriteBlob((void *)g_opening_menu_sprite_blob_handle);
  PTR_DAT_005832b4->font_slot = 1;

  if (g_menu_selection_value == 4)
  {
    ShowHallBackgroundScreen();

    // TODO: can you accomplish this w/o goto?
    goto loop;
  }
  else
  {
    return g_menu_selection_value - 1;
  }
}

// FUNCTION: SHANDALAR 0x004a0eef
int HandleOpeningMenuControlEvent(AdvMenuControl *control, int event_type)
{
  int is_inside_control_bounds;

  if (DAT_00747ee0 == 0)
  {
    if ((DAT_00986d9c < control->x) || (control->width + control->x < DAT_00986d9c))
    {
      is_inside_control_bounds = 0;
    }
    else if ((DAT_00986d98 < control->y) || (control->y + control->height < DAT_00986d98))
    {
      is_inside_control_bounds = 0;
    }
    else
    {
      is_inside_control_bounds = 1;
    }

    if (is_inside_control_bounds == 0)
    {
      return 0;
    }
  }

  if (control->state == 3)
  {
    return 0;
  }

  DrawOpeningMenuEntry((int)(control - g_opening_menu_controls), event_type);
  if ((event_type == 2) && (control->on_activate != (AdvMenuActivateCallback)0))
  {
    control->on_activate(control);
  }
  return 1;
}

// FUNCTION: SHANDALAR 0x004a0fed
int HandleOpeningMenuControlActivate(AdvMenuControl *control)
{
  FUN_005626b0("x:sound\\button2.wav", 0xf, 100, 100, 0);
  g_menu_selection_value = control->selection_value;
  return 0;
}

// FUNCTION: SHANDALAR 0x004a101f
int DrawDifficultyMenuEntry(int difficulty_index, int visual_state)
{
  struct
  {
    FacemakerWindowBounds *source_page;
    int preview_y;
    int preview_x;
    int preview_height;
    unsigned int preview_width;
  } s;

  switch (visual_state)
  {
  case 0:
    s.source_page = PTR_DAT_005832dc;
    break;
  case 1:
    s.source_page = PTR_DAT_00583304;
    break;
  case 2:
    s.source_page = PTR_DAT_00583304;
  default:
    break;
  }
  s.preview_x = g_difficulty_menu_preview_rects[difficulty_index].x;
  s.preview_y = g_difficulty_menu_preview_rects[difficulty_index].y;
  s.preview_width = (unsigned int)g_difficulty_menu_preview_rects[difficulty_index].width;
  s.preview_height = g_difficulty_menu_preview_rects[difficulty_index].height;

  if (visual_state == 2)
  {
    BlitGraphicsRect(PTR_DAT_005832dc, 200, s.preview_y - ScaleUiCoordinate(0x43), s.preview_width, s.preview_height, PTR_DAT_005832dc, 0, 0);
    StretchBlitGraphicsRect(s.source_page, s.preview_x, s.preview_y, s.preview_width, s.preview_height, PTR_DAT_005832dc, 4, 4, s.preview_width - 4,
                            s.preview_height - 4);
    FUN_00579bf0(PTR_DAT_005832dc, 0, 0, s.preview_width, s.preview_height, PTR_DAT_005832b4, s.preview_x, s.preview_y);
  }
  else
  {
    FUN_00579bf0(s.source_page, s.preview_x, s.preview_y, s.preview_width, s.preview_height, PTR_DAT_005832b4, s.preview_x, s.preview_y);
  }
  return 0;
}

// FUNCTION: SHANDALAR 0x004a1194
int RunDifficultyMenu(void)
{
  int menu_context;
  int difficulty_index;

  if (g_difficulty_menu_strings_loaded == 0)
  {
    g_difficulty_caption_table = ((int *(__cdecl *)(FILE *, char *, int))LoadIniEscapedStringTable)(DAT_0078cf08, "diffCaption",
                                                                                       (int)DAT_0078df10);
    g_difficulty_option_table = ((int *(__cdecl *)(FILE *, char *, int))LoadIniEscapedStringTable)(DAT_0078cf08, "diffs",
                                                                                       (int)DAT_0078df10);
    g_difficulty_menu_strings_loaded = 1;
  }

  AnimatePaletteToColor(0, DAT_00589dec);
  LoadPcxResource(1, 0, 0, "menu2.pic",
                  (g_graphics_bpp == 8) ? &g_palette_data_words : (RpBitsPalettePacket *)1);
  StretchBlitGraphicsRect(PTR_DAT_005832dc, 0, 0, 0x280, 0x1e0, PTR_DAT_005832b4, 0, 0, global_screen_width,
                          global_screen_height);

  PTR_DAT_005832b4->font_slot = 7;
  SetFontStyleSize(7, ScaleUiCoordinate(0x1b));
  FUN_0057b560(PTR_DAT_005832b4, 0x76, ScaleUiCoordinate(0x140), ScaleUiCoordinate(0x2d), "%s",
               (char *)g_difficulty_caption_table[0]);

  SetFontStyleSize(7, ScaleUiCoordinate(0x16));
  {
    int text_width;

    for (difficulty_index = 0; difficulty_index < 4; difficulty_index = difficulty_index + 1)
    {
      text_width = MeasureMultilineTextWidth(PTR_DAT_005832b4, (char *)g_difficulty_option_table[difficulty_index]);
      FUN_0057b530(PTR_DAT_005832b4, 0x7b, ScaleUiCoordinate(0x1b0) - text_width, ScaleUiCoordinate(difficulty_index * 100 + 0x75),
                   "%s", (char *)g_difficulty_option_table[difficulty_index]);
    }
  }

  StretchBlitGraphicsRect(PTR_DAT_005832dc, 0x1b1, 0x43, 0xa4, 0x19d, PTR_DAT_005832dc, 200, 0, ScaleUiCoordinate(0xa4),
                          ScaleUiCoordinate(0x19d));

  FUN_0057ce70(0, DAT_00589dec);

  LoadPcxIntoPage(1, "menu2-norm.pic");
  StretchBlitGraphicsRect(PTR_DAT_005832dc, 0, 0, 0xa4, 0x19d, PTR_DAT_005832dc, ScaleUiCoordinate(0x1b1),
                          ScaleUiCoordinate(0x43), ScaleUiCoordinate(0xa4), ScaleUiCoordinate(0x19d));

  LoadPcxIntoPage(2, "menu2-hi.pic");
  StretchBlitGraphicsRect(PTR_DAT_00583304, 0, 0, 0xa4, 0x19d, PTR_DAT_00583304, ScaleUiCoordinate(0x1b1),
                          ScaleUiCoordinate(0x43), ScaleUiCoordinate(0xa4), ScaleUiCoordinate(0x19d));

  if (g_difficulty_menu_controls[0].x == g_difficulty_menu_controls[0].base_x)
  {
    for (difficulty_index = 0; difficulty_index < 4; difficulty_index = difficulty_index + 1)
    {
      g_difficulty_menu_preview_rects[difficulty_index].x = ScaleUiCoordinate(g_difficulty_menu_preview_rects[difficulty_index].x);
      g_difficulty_menu_preview_rects[difficulty_index].y = ScaleUiCoordinate(g_difficulty_menu_preview_rects[difficulty_index].y);
      g_difficulty_menu_preview_rects[difficulty_index].width = ScaleUiCoordinate(g_difficulty_menu_preview_rects[difficulty_index].width);
      g_difficulty_menu_preview_rects[difficulty_index].height = ScaleUiCoordinate(g_difficulty_menu_preview_rects[difficulty_index].height);
      g_difficulty_menu_controls[difficulty_index].x = ScaleUiCoordinate(g_difficulty_menu_controls[difficulty_index].x);
      g_difficulty_menu_controls[difficulty_index].y = ScaleUiCoordinate(g_difficulty_menu_controls[difficulty_index].y);
      g_difficulty_menu_controls[difficulty_index].width = ScaleUiCoordinate(g_difficulty_menu_controls[difficulty_index].width);
      g_difficulty_menu_controls[difficulty_index].height = ScaleUiCoordinate(g_difficulty_menu_controls[difficulty_index].height);
    }
  }

  menu_context = FUN_00500321();
  FUN_005000fb(menu_context);
  FUN_0050014e(g_difficulty_menu_controls, 5, menu_context);

  for (difficulty_index = 0; difficulty_index < 4; difficulty_index = difficulty_index + 1)
  {
    DrawDifficultyMenuEntry(difficulty_index, 0);
  }

  g_menu_selection_value = -1;
  while (g_menu_selection_value == -1)
  {
    FUN_005003b7(DAT_007898f4, DAT_007898f8, DAT_00986d94);
  }

  FUN_0050035e();
  FUN_005000fb(menu_context);
  return g_menu_selection_value - 1;
}

// FUNCTION: SHANDALAR 0x004a168a
int HandleDifficultyMenuControlEvent(AdvMenuControl *control, int event_type)
{
  int is_inside_control_bounds;

  if (DAT_00747ee0 == 0)
  {
    if ((DAT_00986d9c < *(int *)((int)control + 0x10)) || (*(int *)((int)control + 0x18) + *(int *)((int)control + 0x10) < DAT_00986d9c))
    {
      is_inside_control_bounds = 0;
    }
    else if ((DAT_00986d98 < control->y) || (control->y + control->height < DAT_00986d98))
    {
      is_inside_control_bounds = 0;
    }
    else
    {
      is_inside_control_bounds = 1;
    }

    if (is_inside_control_bounds == 0)
    {
      return 0;
    }
  }

  if (control->state == 3)
  {
    return 0;
  }

  DrawDifficultyMenuEntry(control->selection_value - 1, event_type);
  if ((event_type == 2) && (control->on_activate != (AdvMenuActivateCallback)0))
  {
    control->on_activate(control);
  }
  return 1;
}

// FUNCTION: SHANDALAR 0x004a177f
int HandleDifficultyMenuControlActivate(AdvMenuControl *control)
{
  FUN_005626b0("x:sound\\button2.wav", 0xf, 100, 100, 0);
  g_menu_selection_value = control->selection_value;
  return 0;
}

// FUNCTION: SHANDALAR 0x004a17b1
int DrawColorMenuEntry(int color_index, int visual_state)
{
  struct
  {
    int swatch_y;
    unsigned int swatch_x;
    unsigned int swatch_height;
    unsigned int swatch_width;
    EncodedImage **swatch_sprite_table;
  } s;

  switch (visual_state)
  {
  case 0:
    s.swatch_sprite_table = g_color_menu_normal_swatch_sprites;
    break;
  case 1:
    s.swatch_sprite_table = g_color_menu_pressed_swatch_sprites;
    break;
  case 2:
    s.swatch_sprite_table = g_color_menu_pressed_swatch_sprites;
    break;
  }

  s.swatch_x = (unsigned int)g_color_menu_swatch_rects[color_index].x;
  s.swatch_y = g_color_menu_swatch_rects[color_index].y;
  s.swatch_width = (unsigned int)g_color_menu_swatch_rects[color_index].width;
  s.swatch_height = (unsigned int)g_color_menu_swatch_rects[color_index].height;

  if (visual_state == 2)
  {
    DrawEncodedImageResampled(PTR_DAT_00583304, s.swatch_x + 2, s.swatch_y + 2, s.swatch_width - 4, s.swatch_height - 4,
                              *(s.swatch_sprite_table + color_index));
    BlitGraphicsRect(PTR_DAT_00583304, s.swatch_x, s.swatch_y, s.swatch_width, s.swatch_height, PTR_DAT_005832b4, s.swatch_x, s.swatch_y);
  }
  else
  {
    DrawEncodedImageResampled(PTR_DAT_005832b4, s.swatch_x, s.swatch_y, s.swatch_width, s.swatch_height, *(s.swatch_sprite_table + color_index));
  }
  return 0;
}

// FUNCTION: SHANDALAR 0x004a18f0
int RunColorMenu(void)
{
  struct
  {
    int restore_rect_buffer[4];
    int temp_rect_buffer[4];
    int menu_context;
    int color_index;
    int restore_rect_x;
    int restore_rect_y;
    int restore_rect_width;
    int restore_rect_height;
  } s;

  if (g_color_menu_strings_loaded == 0)
  {
    g_color_caption_table = ((int *(__cdecl *)(FILE *, char *, int))LoadIniEscapedStringTable)(DAT_0078cf08, "colorCaption", (int)DAT_0078df10);
    g_color_menu_color_name_table = ((int *(__cdecl *)(FILE *, char *, int))LoadIniEscapedStringTable)(DAT_0078cf08, "colorMenuColors", (int)DAT_0078df10);
    g_color_menu_flavor_table = ((int *(__cdecl *)(FILE *, char *, int))LoadIniEscapedStringTable)(DAT_0078cf08, "colorMenuFlavor", (int)DAT_0078df10);
    g_color_menu_strings_loaded = 1;
  }

  AnimatePaletteToColor(0, DAT_00589dec);
  if (g_graphics_bpp == 8)
  {
    ClearGraphicsPageWithPaletteColor(0, 0);
  }

  LoadPcxResource(1, 0, 0, "menu3.pic", (g_graphics_bpp == 8) ? &g_palette_data_words : (RpBitsPalettePacket *)1);
  LoadPcxIntoPageNoPalette("menu3.pic");
  StretchBlitGraphicsRect(PTR_DAT_005832dc, 0, 0, 0x280, 0x1e0, PTR_DAT_00583304, 0, 0, global_screen_width, global_screen_height);
  BlitGraphicsRect(PTR_DAT_00583304, 0, 0, global_screen_width, global_screen_height, PTR_DAT_005832dc, 0, 0);

  PTR_DAT_005832dc->font_slot = 7;
  SetFontStyleSize(7, ScaleUiCoordinate(0x1b));
  FUN_0057b560(PTR_DAT_005832dc, 0x9b, ScaleUiCoordinate(0x140), ScaleUiCoordinate(0x22), "%s", (char *)g_color_caption_table[0]);

  SetFontStyleSize(7, ScaleUiCoordinate(0x12));
  for (s.color_index = 0; s.color_index < 5; s.color_index = s.color_index + 1)
  {
    FUN_0057b4d0(PTR_DAT_005832dc, 0xa3, ScaleUiCoordinate(0x7d), ScaleUiCoordinate(s.color_index * 0x4c + 0x4c), "%s",
                 (char *)g_color_menu_color_name_table[s.color_index]);
  }

  SetFontStyleSize(7, ScaleUiCoordinate(10));
  for (s.color_index = 0; s.color_index < 5; s.color_index = s.color_index + 1)
  {
    FUN_0057b4d0(PTR_DAT_005832dc, 0x71, ScaleUiCoordinate(0x85), ScaleUiCoordinate(s.color_index * 0x4c + 100), "%s",
                 (char *)g_color_menu_flavor_table[s.color_index]);
  }

  AnimatePaletteToColor(0, 4);
  BlitGraphicsRect(PTR_DAT_005832dc, 0, 0, global_screen_width, global_screen_height, PTR_DAT_005832b4, 0, 0);
  LoadPcxResource(-1, 0, 0, "menu3.pic", &g_palette_data_words);
  FUN_0057ce70(0, DAT_00589dec);

  *(AdvMenuRect *)&s.restore_rect_x = *(AdvMenuRect *)FUN_00579ea0(s.temp_rect_buffer, PTR_DAT_00583304, 0, 0, global_screen_width, global_screen_height);

  BeginSpriteEncodeSession();
  FUN_0057c7e0(1, "menu3-but1.pic");
  for (s.color_index = 0; s.color_index < 5; s.color_index = s.color_index + 1)
  {
    g_color_menu_normal_swatch_sprites[s.color_index] = EncodeSpriteFromPage(1, 0, s.color_index * 0x4b, 0x46, 0x4b);
  }

  FUN_0057c7e0(1, "menu3but.pic");
  for (s.color_index = 0; s.color_index < 5; s.color_index = s.color_index + 1)
  {
    g_color_menu_pressed_swatch_sprites[s.color_index] = EncodeSpriteFromPage(1, 0, s.color_index * 0x4b, 0x46, 0x4b);
  }
  FinalizeSpriteEncodeSession();

  if (g_color_menu_controls[0].x == g_color_menu_controls[0].base_x)
  {
    for (s.color_index = 0; s.color_index < 5; s.color_index = s.color_index + 1)
    {
      g_color_menu_swatch_rects[s.color_index].x = ScaleUiCoordinate(g_color_menu_swatch_rects[s.color_index].x);
      g_color_menu_swatch_rects[s.color_index].y = ScaleUiCoordinate(g_color_menu_swatch_rects[s.color_index].y);
      g_color_menu_swatch_rects[s.color_index].width = ScaleUiCoordinate(g_color_menu_swatch_rects[s.color_index].width);
      g_color_menu_swatch_rects[s.color_index].height = ScaleUiCoordinate(g_color_menu_swatch_rects[s.color_index].height);
      g_color_menu_controls[s.color_index].x = ScaleUiCoordinate(g_color_menu_controls[s.color_index].x);
      g_color_menu_controls[s.color_index].y = ScaleUiCoordinate(g_color_menu_controls[s.color_index].y);
      g_color_menu_controls[s.color_index].width = ScaleUiCoordinate(g_color_menu_controls[s.color_index].width);
      g_color_menu_controls[s.color_index].height = ScaleUiCoordinate(g_color_menu_controls[s.color_index].height);
    }
  }

  s.menu_context = FUN_00500321();
  FUN_005000fb(s.menu_context);
  FUN_0050014e(g_color_menu_controls, 6, s.menu_context);
  for (s.color_index = 0; s.color_index < 5; s.color_index = s.color_index + 1)
  {
    DrawColorMenuEntry(s.color_index, 0);
  }

  g_menu_selection_value = -1;
  while (g_menu_selection_value == -1)
  {
    FUN_005003b7(DAT_007898f4, DAT_007898f8, DAT_00986d94);
  }

  FUN_0050035e();
  FUN_005000fb(s.menu_context);
  FreeSpriteBlob(g_color_menu_normal_swatch_sprites[0]);
  FUN_00579ea0(s.restore_rect_buffer, PTR_DAT_00583304, s.restore_rect_x, s.restore_rect_y, s.restore_rect_width, s.restore_rect_height);

  if (g_menu_selection_value == 0)
  {
    return -1;
  }
  else
  {
    return g_color_menu_selection_to_deck_color[g_menu_selection_value - 1];
  }
}

// FUNCTION: SHANDALAR 0x004a1f2a
int HandleColorMenuControlEvent(AdvMenuControl *control, int event_type)
{
  int is_inside_control_bounds;

  if (DAT_00747ee0 == 0)
  {
    if ((DAT_00986d9c < control->x) || (control->x + control->width < DAT_00986d9c))
    {
      is_inside_control_bounds = 0;
    }
    else if ((DAT_00986d98 < control->y) || (control->y + control->height < DAT_00986d98))
    {
      is_inside_control_bounds = 0;
    }
    else
    {
      is_inside_control_bounds = 1;
    }

    if (is_inside_control_bounds == 0)
    {
      return 0;
    }
  }

  if (control->state == 3)
  {
    return 0;
  }

  DrawColorMenuEntry(control->selection_value - 1, event_type);
  if ((event_type == 2) && (control->on_activate != (AdvMenuActivateCallback)0))
  {
    control->on_activate(control);
  }
  return 1;
}

// FUNCTION: SHANDALAR 0x004a201f
int HandleColorMenuControlActivate(AdvMenuControl *control)
{
  FUN_005626b0("x:sound\\button2.wav", 0xf, 100, 100, 0);
  g_menu_selection_value = control->selection_value;
  return 0;
}

// FUNCTION: SHANDALAR 0x004a21ad
int RunFacemakerFlow(void)
{
  unsigned int is_topmost_window;
  int selected_menu_entry;
  int x;
  int y;
  int spawn_result;
  int image_width;
  int image_height;
  int scaled_face_x;
  int screen_center_x;
  int parse_result;
  int old_page_rect[4];
  char *pedestal_images[6];
  char *pixel;
  char facemaker_path[MAX_PATH];
  FacemakerWindowBounds page4_bounds;
  FacemakerWindowBounds *page4_bounds_ptr;
  DIBSurface *page4_dib;
  EncodedImage *page2_sprite;
  void *load_palette_data;

  FUN_004ce992(0x14);
  set_global_base_directory(facemaker_path);
  strcat(facemaker_path, "\\Facemaker.exe");

  while (1)
  {
    is_topmost_window = (unsigned int)((GetWindowLongA(DAT_00748420, -0x14) & 8) != 0);
    if (is_topmost_window != 0)
    {
      SetWindowPos(DAT_00748420, (HWND)-2, 0, 0, 0, 0, 3);
    }

    spawn_result = (int)_spawnl(0, facemaker_path, facemaker_path, "/S", 0);
    BringWindowToTop(DAT_00748420);
    SetForegroundWindow(DAT_00748420);
    SetFocus(DAT_00748420);
    if (spawn_result == -1)
    {
      return -1;
    }

    page4_bounds.page_number = 4;
    page4_bounds.unk_04 = 0;
    page4_bounds.unk_08 = 0;
    page4_bounds.max_x = 800;
    page4_bounds.max_y = 600;
    page4_bounds.unk_14 = 1;
    page4_bounds.text_color = 0x0f;
    page4_bounds.unk_1c = 4;
    page4_bounds.font_slot = 0;
    page4_bounds_ptr = &page4_bounds;

    selected_menu_entry = g_face_preview_sprite_group[g_menu_selection_value];
    (void)selected_menu_entry;

    BeginSpriteEncodeSession();
    page2_sprite = EncodeSpriteFromPage(2, 0, 0, 0x89, 0xa9);
    FinalizeSpriteEncodeSession();

    image_width = 0x89;
    image_height = 0xa9;
    page4_dib = (DIBSurface *)CreateGraphicsPage(4, image_width * 2, image_height, 8);
    SetGraphicsPage(4, page4_dib);
    FUN_00579ea0(old_page_rect, page4_bounds_ptr, 0, 0, image_width * 2, image_height);
    LoadPcxIntoPageNoPalette("menu4.pic");
    FUN_005797e0(page4_bounds_ptr, 0, 0, image_width, image_height, 0);
    BlitGraphicsRect(PTR_DAT_00583304, 0, 0, 0x8a, 0xaa, page4_bounds_ptr, 0, 0);

    for (y = 0; y < image_height; y = y + 1)
    {
      pixel = (char *)page4_dib->pBits + (page4_dib->width + page4_dib->rowPadding) * y;
      for (x = 0; x < image_width; x = x + 1)
      {
        if (*pixel == '\0')
        {
          pixel[image_width] = (char)0xff;
        }
        pixel = pixel + 1;
      }
    }

    g_facemaker_page4_bitmap = page4_dib->hBitmap;
    g_facemaker_page4_dib = page4_dib;
    SelectObject(page4_dib->hTempDC, page4_dib->hPreviousBitmap);
    BuildFacemakerPortraitSprites(PTR_DAT_00583304);
    g_graphics_pages[4] = (DIBSurface *)0;

    pedestal_images[1] = "prdblk.pic";
    pedestal_images[2] = "prdblu.pic";
    pedestal_images[3] = "prdgrn.pic";
    pedestal_images[4] = "prdrd.pic";
    pedestal_images[5] = "prdwt.pic";

    LoadPcxIntoPageNoPalette("pedstls.pic");
    load_palette_data = (g_graphics_bpp == 8) ? (void *)&g_palette_data_words : (void *)1;
    LoadPcxResource(1, global_screen_width - 0x280, global_screen_height - 0x1e0,
                    pedestal_images[((int *)g_color_menu_selection_to_deck_color - 1)[g_menu_selection_value]],
                    load_palette_data);
    StretchBlitGraphicsRect(PTR_DAT_005832dc, global_screen_width - 0x280, global_screen_height - 0x1e0, 0x280, 0x1e0,
                            PTR_DAT_005832dc, 0, 0, global_screen_width, global_screen_height);

    screen_center_x = global_screen_width / 2;
    scaled_face_x = ScaleUiCoordinate(0x89);
    DrawEncodedImageResampled(PTR_DAT_005832dc, screen_center_x - scaled_face_x, ScaleUiCoordinate(0x14),
                              ScaleUiCoordinate(0x112), ScaleUiCoordinate(0x152), page2_sprite);
    BlitGraphicsRect(PTR_DAT_005832dc, 0, 0, global_screen_width, global_screen_height, PTR_DAT_005832b4, 0, 0);

    memset(g_name_entry_buffer, 0, 0x40);
    ReadGraphicsScanline((unsigned int *)g_name_entry_buffer, 2, 0, 200, 0x40);
    g_player_is_male = (g_name_entry_buffer[0] == 'm');
    strcpy(g_name_entry_buffer, g_name_entry_buffer + 1);
    g_name_entry_cursor = (int)strlen(g_name_entry_buffer);
    PTR_DAT_005832b4->font_slot = 1;
    ReadGraphicsScanline((unsigned int *)g_name_entry_buffer, 2, 1, 200, 0x3f);

    parse_result = RunNameEntryDialog(g_name_entry_buffer);
    if (parse_result != -1)
    {
      break;
    }
  }

  strcpy(g_player_name, g_name_entry_buffer);
  return g_player_is_male;
}

// FUNCTION: SHANDALAR 0x004a288d
int RunNameEntryDialog(char *name_buffer)
{
  int dialog_top;
  int key_code;
  int text_width;

  dialog_top = 0x12c;
  FUN_0057c7e0(1, "namepick.pic");
  PTR_DAT_005832dc->font_slot = PTR_DAT_005832b4->font_slot;
  FUN_0056cc4d("ADVstrings.txt", "STARTUP");
  DrawLocalizedText(PTR_DAT_005832dc, 0xed, 0x8b, 0x19, &text_lines[2][0]);
  DrawLocalizedText(PTR_DAT_005832dc, 0xb4, 0x8a, 0x18, &text_lines[2][0]);
  BlitGraphicsRect(PTR_DAT_005832dc, 0, 0, 0x114, 0x6a, PTR_DAT_005832dc, 0, 200);
  DrawLocalizedText(PTR_DAT_005832dc, 0xb4, 0x8a, 0x100, "%s", name_buffer);
  BlitTransparentRuns(PTR_DAT_005832dc, 0, 200, 0x114, 0x6a, PTR_DAT_005832b4, (global_screen_width - 0x114) / 2, ScaleUiCoordinate(dialog_top));
  do
  {
    key_code = PopQueuedKeyInput();
    if (key_code == 0x1c0d)
    {
      break;
    }
    text_width = MeasureMultilineTextWidth(PTR_DAT_005832dc, name_buffer);
    if (key_code != 0)
    {
      if (key_code == 0x11b)
      {
        return -1;
      }
      ApplyNameEntryKey(name_buffer, (unsigned int)key_code, 0x19);
      BlitGraphicsRect(PTR_DAT_005832dc, 0, 0, 0x114, 0x6a, PTR_DAT_005832dc, 0, 200);
      DrawLocalizedText(PTR_DAT_005832dc, 0xb4, 0x8a, 0x100, "%s", name_buffer);
      BlitTransparentRuns(PTR_DAT_005832dc, 0, 200, 0x114, 0x6a, PTR_DAT_005832b4, (global_screen_width - 0x114) / 2, ScaleUiCoordinate(dialog_top));
    }
    DrawBlinkingNameCaret(PTR_DAT_005832b4, 0xb4, ScaleUiCoordinate(0x140) - text_width / 2, ScaleUiCoordinate(dialog_top) + 0x30, name_buffer,
                          g_name_entry_cursor);
  } while (1);

  FUN_005597ca();
  return 0;
}

// FUNCTION: SHANDALAR 0x004a2c10
void BuildFacemakerPortraitSprites(FacemakerWindowBounds *page)
{
  if (g_face_preview_sprite_group[0] != 0)
  {
    FreeSpriteBlob((void *)g_face_preview_sprite_group[0]);
  }
  ApplyPortraitPaletteMap(page, 0, 0, 0x89, 0xa9, "pedstls.pic", "advfac64.pic");
  LoadPcxIntoPageNoPalette("advfac64.pic");
  BeginSpriteEncodeSession();
  g_face_preview_sprite_group[0] = (int)EncodeSpriteFromPage(page->page_number, 0, 0, 0x89, 0xa9);
  ApplyPortraitTintMap(page, 0, 0, 0x89, 0xa9, 0, 1);
  g_face_preview_sprite_selected = EncodeSpriteFromPage(page->page_number, 0, 0, 0x89, 0xa9);
  FinalizeSpriteEncodeSession();
}

// FUNCTION: SHANDALAR 0x00521ffa
int ApplyPortraitPaletteMap(FacemakerWindowBounds *page, int src_x, int src_y, unsigned int width, int height, char *palette_source_path, char *portrait_path)
{
  (void)page;
  (void)src_x;
  (void)src_y;
  (void)width;
  (void)height;
  (void)portrait_path;
  LoadPcxIntoPageNoPalette(palette_source_path);
  return 0;
}

// FUNCTION: SHANDALAR 0x00521e80
int ApplyPortraitTintMap(FacemakerWindowBounds *page, int src_x, int src_y, unsigned int width, int height, unsigned int tint_mask, int tint_mode)
{
  (void)page;
  (void)src_x;
  (void)src_y;
  (void)width;
  (void)height;
  (void)tint_mask;
  (void)tint_mode;
  return 0;
}

// FUNCTION: SHANDALAR 0x0057b4a0
void DrawLocalizedText(FacemakerWindowBounds *window, int color_index, int x, int y, ...)
{
  DrawTextFormatted(window, color_index, 0, 0, 1, 1, x, y, (int *)(&y + 1));
}

// FUNCTION: SHANDALAR 0x004a2713
int BlitTransparentRuns(FacemakerWindowBounds *src, int src_x, int src_y, unsigned int width, int height, FacemakerWindowBounds *dst, int dst_x, int dst_y)
{
  int row;
  int x;
  unsigned int local_3f4[250];
  unsigned int *run_start;
  unsigned int run_length;

  for (row = 0; row < height; row = row + 1)
  {
    ReadGraphicsScanline(local_3f4, src->page_number, src_x, src_y + row, width);
    if ((char)local_3f4[0] == '\0')
    {
      run_start = (unsigned int *)0;
    }
    else
    {
      run_start = local_3f4;
    }

    run_length = 0;
    for (x = 0; x < (int)width; x = x + 1)
    {
      if (*((char *)local_3f4 + x) == '\0')
      {
        if (run_length == 0)
        {
          run_start = (unsigned int *)((char *)local_3f4 + x);
        }
        else
        {
          WriteGraphicsScanline(run_start, dst->page_number, (int)run_start + (dst_x - (int)local_3f4), dst_y + row, run_length);
          run_length = 0;
          run_start = (unsigned int *)((char *)local_3f4 + x);
        }
      }
      else
      {
        run_length = run_length + 1;
      }
    }

    if (run_length != 0)
    {
      WriteGraphicsScanline(run_start, dst->page_number, (int)run_start + (dst_x - (int)local_3f4), dst_y + row, run_length);
    }
  }

  return 0;
}

// FUNCTION: SHANDALAR 0x004a2e33
int ApplyNameEntryKey(char *name_buffer, unsigned int key_code, size_t max_len)
{
  unsigned int ch;
  size_t string_len;
  size_t cursor_pos;

  cursor_pos = (size_t)g_name_entry_cursor;
  if ((int)key_code < 0xf0a)
  {
    if (key_code == 0xf09)
    {
      g_name_entry_cursor = g_name_entry_cursor + 8;
      if ((int)g_name_entry_cursor >= (int)max_len)
      {
        g_name_entry_cursor = (int)max_len;
      }
      return 0;
    }
    if (key_code == 0xe08)
    {
      if (g_name_entry_cursor == 0)
      {
        return 0;
      }
      g_name_entry_cursor = g_name_entry_cursor - 1;
      strcpy(name_buffer + g_name_entry_cursor, name_buffer + cursor_pos);
      return 0;
    }
  }
  else if ((int)key_code < 0x1c0e)
  {
    if (key_code == 0x1c0d)
    {
      return 1;
    }
    if (key_code == 0xf0f)
    {
      g_name_entry_cursor = g_name_entry_cursor - 8;
      if (g_name_entry_cursor < 0)
      {
        g_name_entry_cursor = 0;
      }
      return 0;
    }
  }
  else if ((int)key_code < 0x4b01)
  {
    if (key_code == 0x4b00)
    {
      if (g_name_entry_cursor > 0)
      {
        g_name_entry_cursor = g_name_entry_cursor - 1;
      }
      return 0;
    }
    if (key_code == 0x4700)
    {
      g_name_entry_cursor = 0;
      return 0;
    }
  }
  else if ((int)key_code < 0x4f01)
  {
    if (key_code == 0x4f00)
    {
      g_name_entry_cursor = (int)strlen(name_buffer);
      return 0;
    }
    if (key_code == 0x4d00)
    {
      if ((int)max_len <= g_name_entry_cursor)
      {
        return 0;
      }
      string_len = strlen(name_buffer);
      if ((int)string_len == g_name_entry_cursor)
      {
        strcat(name_buffer, " ");
      }
      g_name_entry_cursor = g_name_entry_cursor + 1;
      return 0;
    }
  }
  else
  {
    if (key_code == 0x5200)
    {
      g_name_entry_insert_mode = g_name_entry_insert_mode ^ 1;
      return 0;
    }
    if (key_code == 0x5300)
    {
      string_len = strlen(name_buffer);
      if ((int)string_len <= g_name_entry_cursor)
      {
        return 0;
      }
      strcpy(name_buffer + g_name_entry_cursor, name_buffer + g_name_entry_cursor + 1);
      return 0;
    }
  }

  string_len = strlen(name_buffer);
  ch = key_code & 0xff;
  if (((int)string_len < (int)max_len) &&
      (((0x40 < ch && ch < 0x5b) || (0x60 < ch && ch < 0x7b)) || ((0x2f < ch && ch < 0x3a) || ch == 0x20)))
  {
    if (g_name_entry_insert_mode != 0)
    {
      memmove(name_buffer + g_name_entry_cursor + 1, name_buffer + g_name_entry_cursor, (max_len - g_name_entry_cursor) - 1);
    }
    name_buffer[g_name_entry_cursor] = (char)key_code;
    g_name_entry_cursor = g_name_entry_cursor + 1;
  }
  return 0;
}

// FUNCTION: SHANDALAR 0x004a8470
int HandleLoadSaveEscControlEvent(AdvMenuControl *control, int event_type)
{
  (void)control;
  if (event_type == 2)
  {
    FUN_005626b0("x:sound\\button2.wav", 0xf, 100, 100, 0);
    g_loadsave_menu_selection = 0xe;
  }
  return 0;
}

// FUNCTION: SHANDALAR 0x004a84ab
int ActivateLoadSaveEscControl(AdvMenuControl *control)
{
  (void)control;
  g_loadsave_menu_selection = 0xe;
  return 0;
}

// FUNCTION: SHANDALAR 0x004a84c7
int DrawBlinkingNameCaret(FacemakerWindowBounds *window, int color_index, int caret_x, int caret_y, char *name_buffer, int cursor_pos)
{
  size_t string_len;
  unsigned int frame_counter;
  int char_width;
  int i;
  int caret_width;
  int baseline_y;

  string_len = strlen(name_buffer);
  frame_counter = FUN_004ce97d();
  if ((frame_counter & 7) == 0)
  {
    return 0;
  }

  for (i = 0; i < cursor_pos; i = i + 1)
  {
    if (i < (int)string_len)
    {
      char_width = FUN_0057aa30(window->font_slot, name_buffer[i]);
    }
    else
    {
      char_width = FUN_0057aa30(window->font_slot, ' ');
    }
    caret_x = caret_x + char_width;
  }

  if (i < (int)string_len)
  {
    caret_width = FUN_0057aa30(window->font_slot, name_buffer[cursor_pos]);
  }
  else
  {
    caret_width = FUN_0057aa30(window->font_slot, ' ');
  }

  baseline_y = caret_y + FUN_0057adf0(window->font_slot) / 2;
  FUN_005796c0(window, caret_x, baseline_y - 1, caret_x + caret_width, baseline_y - 1, color_index);
  FUN_005796c0(window, caret_x, baseline_y, caret_x + caret_width, baseline_y, color_index);
  return 0;
}

// FUNCTION: SHANDALAR 0x0057b470
void DrawLoadSaveButtonText(FacemakerWindowBounds *window, int color, int x, int y, ...)
{
  DrawTextFormatted(window, color, 0, 0, 0, 1, x, y, (int *)(&y + 1));
}

// FUNCTION: SHANDALAR 0x004a85fc
int DrawLoadSaveButton(FacemakerWindowBounds *window, EncodedImage **button_sprites, int button_width, int button_height, char *button_text, int font_slot,
                 int visual_state)
{
  EncodedImage *sprite;
  int x_cursor;
  int text_width;
  size_t text_len;
  int text_x;
  int text_y;
  int text_color;

  button_width = ScaleUiCoordinate(button_width);
  button_height = ScaleUiCoordinate(button_height);

  sprite = button_sprites[0];
  DrawEncodedImageResampled(window, 0, 0, ScaleUiCoordinate(sprite->width), ScaleUiCoordinate(sprite->height), sprite);
  x_cursor = ScaleUiCoordinate(sprite->width);

  sprite = button_sprites[1];
  while (x_cursor < button_width)
  {
    DrawEncodedImageResampled(window, x_cursor, 0, ScaleUiCoordinate(sprite->width), ScaleUiCoordinate(sprite->height), sprite);
    x_cursor = x_cursor + ScaleUiCoordinate(sprite->width);
  }

  sprite = button_sprites[2];
  x_cursor = button_width;
  x_cursor = x_cursor - ScaleUiCoordinate(sprite->width);
  DrawEncodedImageResampled(window, x_cursor, 0, ScaleUiCoordinate(sprite->width), ScaleUiCoordinate(sprite->height), sprite);

  window->font_slot = font_slot;
  FUN_0057adf0(font_slot);
  text_width = MeasureMultilineTextWidth(window, button_text);
  while (button_width < text_width)
  {
    text_len = strlen(button_text);
    button_text[text_len - 1] = '\0';
    text_width = MeasureMultilineTextWidth(window, button_text);
  }

  text_x = 10;
  text_y = button_height / 2;
  text_color = 0x25;
  if (visual_state == 1)
  {
    text_color = 0x67;
  }
  if (visual_state == 2)
  {
    text_color = 0x67;
    text_y = text_y + 2;
    text_x = 8;
  }
  if (visual_state == 3)
  {
    text_color = 0xf0;
  }

  DrawLoadSaveButtonText(window, text_color, text_x, text_y, button_text);
  if (g_loadsave_edit_active != 0)
  {
    DrawBlinkingNameCaret(window, text_color, text_x, text_y, button_text, g_loadsave_cursor_index);
  }
  return 0;
}

// FUNCTION: SHANDALAR 0x004a884f
int DrawLoadSaveSlotEntry(int slot_index, int visual_state)
{
  EncodedImage **slot_state_sprites;
  int slot_y;
  int slot_w;
  int slot_h;

  switch (visual_state)
  {
  case 0:
    slot_state_sprites = g_loadsave_slot_button_state_sprites;
    break;
  case 1:
    slot_state_sprites = g_loadsave_slot_button_state_sprites;
    break;
  case 2:
    slot_state_sprites = g_loadsave_slot_button_state_sprites + 3;
    break;
  case 3:
    slot_state_sprites = g_loadsave_slot_button_state_sprites;
    break;
  default:
    slot_state_sprites = g_loadsave_slot_button_state_sprites;
    break;
  }

  slot_y = g_loadsave_menu_controls[slot_index].y;
  slot_w = g_loadsave_menu_controls[slot_index].width;
  slot_h = g_loadsave_menu_controls[slot_index].height;

  DrawLoadSaveButton(PTR_DAT_005832dc, slot_state_sprites, 0x168, 0x1b, g_loadsave_slot_descriptions[slot_index], 4, visual_state);
  StretchBlitGraphicsRect(PTR_DAT_005832dc, 0, 0, ScaleUiCoordinate(0x168), ScaleUiCoordinate(0x1a), PTR_DAT_005832b4,
                          g_loadsave_menu_controls[slot_index].x, slot_y, slot_w,
                          slot_h);
  return 0;
}

// FUNCTION: SHANDALAR 0x004a898c
int DrawLoadSaveFrame(FacemakerWindowBounds *window, int x, int y, int width, int height)
{
  typedef struct
  {
    int total_size;
    short width;
    short height;
    short left_clip;
    short top_clip;
    short first_row;
    short row_count;
  } EncodedImageHeader;
  EncodedImageHeader sprite_headers[9];
  EncodedImage **sprite_table;
  int i;
  int right_span_x;
  int cursor_x;
  int cursor_y;
  int right_edge_strip_width;

  sprite_table = g_loadsave_frame_sprites;
  for (i = 0; i < 9; i = i + 1)
  {
    sprite_headers[i] = *(EncodedImageHeader *)sprite_table[i];
    sprite_headers[i].width = (short)ScaleUiCoordinate((int)sprite_headers[i].width);
    sprite_headers[i].height = (short)ScaleUiCoordinate((int)sprite_headers[i].height);
  }

  x = ScaleUiCoordinate(x);
  y = ScaleUiCoordinate(y);
  width = ScaleUiCoordinate(width);
  height = ScaleUiCoordinate(height);

  DrawEncodedImageResampled(window, (x + width / 2) - (int)sprite_headers[0].width / 2,
                            y - ((int)sprite_headers[0].height - (int)sprite_headers[5].height),
                            (int)sprite_headers[0].width, (int)sprite_headers[0].height, sprite_table[0]);

  right_span_x = ((int)sprite_headers[0].width / 2) + (width / 2) + x;
  for (cursor_x = ((x + width / 2) - (int)sprite_headers[0].width / 2) - (int)sprite_headers[5].width; x < cursor_x;
       cursor_x = cursor_x - (int)sprite_headers[5].width)
  {
    DrawEncodedImageResampled(window, cursor_x, y, (int)sprite_headers[5].width, (int)sprite_headers[5].height, sprite_table[5]);
    DrawEncodedImageResampled(window, right_span_x, y, (int)sprite_headers[5].width, (int)sprite_headers[5].height, sprite_table[5]);
    right_span_x = right_span_x + (int)sprite_headers[5].width;
  }

  DrawEncodedImageResampled(window, x, y, (int)sprite_headers[1].width, (int)sprite_headers[1].height, sprite_table[1]);
  DrawEncodedImageResampled(window, (width + x) - (int)sprite_headers[2].width, y, (int)sprite_headers[2].width,
                            (int)sprite_headers[2].height, sprite_table[2]);

  right_edge_strip_width = (int)sprite_headers[8].width;
  for (cursor_y = (int)sprite_headers[1].height + y; cursor_y < (height + y) - (int)sprite_headers[3].height;
       cursor_y = cursor_y + (int)sprite_headers[8].height)
  {
    DrawEncodedImageResampled(window, x, cursor_y, (int)sprite_headers[8].width, (int)sprite_headers[8].height, sprite_table[8]);
    DrawEncodedImageResampled(window, (width + x) - right_edge_strip_width, cursor_y,
                              (int)sprite_headers[6].width, (int)sprite_headers[6].height, sprite_table[6]);
  }

  DrawEncodedImageResampled(window, x, (height + y) - (int)sprite_headers[3].height, (int)sprite_headers[3].width,
                            (int)sprite_headers[3].height, sprite_table[3]);
  DrawEncodedImageResampled(window, (width + x) - (int)sprite_headers[4].width, (height + y) - (int)sprite_headers[4].height,
                            (int)sprite_headers[4].width, (int)sprite_headers[4].height, sprite_table[4]);

  right_span_x = ((width + x) - (int)sprite_headers[4].width) - (int)sprite_headers[7].width;
  y = y + (height - (int)sprite_headers[7].height);
  for (cursor_x = (int)sprite_headers[3].width + x; cursor_x < x + width / 2; cursor_x = cursor_x + (int)sprite_headers[7].width)
  {
    DrawEncodedImageResampled(window, cursor_x, y, (int)sprite_headers[7].width, (int)sprite_headers[7].height, sprite_table[7]);
    DrawEncodedImageResampled(window, right_span_x, y, (int)sprite_headers[7].width, (int)sprite_headers[7].height, sprite_table[7]);
    right_span_x = right_span_x - (int)sprite_headers[7].width;
  }

  return 0;
}

// FUNCTION: SHANDALAR 0x004a8da8
int RunLoadSaveMenu(int param_1)
{
  struct
  {
    size_t text_index;
    int menu_control_count;
    int temp_int;
    unsigned int key_code;
    char saved_slot_name[0xff];
    char edit_buffer[0x101];
    int selected_slot;
    int frame_y;
    int frame_x;
    int frame_height;
    int frame_width;
    int menu_context;
    int row;
    char *map_path;
    int slot_index;
    FILE *save_desc_file;
  } locals;
  EncodedImage *sprite;

  locals.map_path = g_loadsave_magic_map_path;
  AnimatePaletteToColor(0, DAT_00589dec);
  LoadPcxResource(1, 0, 0, "menopt.pic", (g_graphics_bpp == 8) ? &g_palette_data_words : (void *)1);
  StretchBlitGraphicsRect(PTR_DAT_005832dc, 0, 0, 0x280, 0x1e0, PTR_DAT_005832b4, 0, 0, global_screen_width, global_screen_height);
  FUN_0057ce70(0, DAT_00589dec);
  LoadPcxIntoPage(1, "optbox.pic");
  BeginSpriteEncodeSession();
  g_loadsave_frame_sprites[0] = EncodeSpriteFromPage(1, 1, 1, 0x40, 0x19);
  g_loadsave_frame_sprites[1] = EncodeSpriteFromPage(1, 0x42, 1, 0xe, 0xe);
  g_loadsave_frame_sprites[2] = EncodeSpriteFromPage(1, 0x83, 1, 0xe, 0xe);
  g_loadsave_frame_sprites[3] = EncodeSpriteFromPage(1, 1, 0x1d, 0xe, 0xe);
  g_loadsave_frame_sprites[4] = EncodeSpriteFromPage(1, 0x42, 0x1d, 0xe, 0xe);
  g_loadsave_frame_sprites[5] = EncodeSpriteFromPage(1, 0x83, 0x1d, 8, 0xb);
  g_loadsave_frame_sprites[6] = EncodeSpriteFromPage(1, 1, 0x39, 0xb, 8);
  g_loadsave_frame_sprites[7] = EncodeSpriteFromPage(1, 0x42, 0x39, 8, 0xb);
  g_loadsave_frame_sprites[8] = EncodeSpriteFromPage(1, 0x83, 0x39, 0xb, 8);
  for (locals.row = 0; locals.row < 2; locals.row = locals.row + 1)
  {
    for (locals.slot_index = 0; locals.slot_index < 3; locals.slot_index = locals.slot_index + 1)
    {
      *(EncodedImage **)((char *)g_loadsave_slot_button_state_sprites + locals.row * 0xc + locals.slot_index * 4) =
          EncodeSpriteFromPage(1, locals.slot_index * 0x41 + 1, locals.row * 0x1c + 0x55, 0x1e, 0x1b);
    }
  }
  FinalizeSpriteEncodeSession();

  locals.frame_x = 0x1c;
  locals.frame_y = 0x45;
  locals.frame_width = 0x1a4 - locals.frame_x;
  locals.frame_height = 0x18e - locals.frame_y;
  DrawLoadSaveFrame(PTR_DAT_005832b4, locals.frame_x, locals.frame_y, locals.frame_width, locals.frame_height);

  if (param_1 != 0)
  {
    strcpy(DAT_0078cf10, gs_loadsave_0077d1b0[0]);
  }
  else
  {
    strcpy(DAT_0078cf10, gs_loadsave_0077d1b0[1]);
  }

  locals.map_path[5] = '4';
  locals.save_desc_file = fopen(g_loadsave_description_file_path, g_loadsave_file_open_mode);
  for (locals.slot_index = 0; locals.slot_index < 10; locals.slot_index = locals.slot_index + 1)
  {
    fgets(g_loadsave_slot_descriptions[locals.slot_index], 0x40, locals.save_desc_file);
    locals.text_index = strlen(g_loadsave_slot_descriptions[locals.slot_index]);
    (&g_loadsave_slot_descriptions[locals.slot_index][0])[-1 + locals.text_index] = '\0';
    locals.temp_int = FUN_004ece40(locals.slot_index + 4);
    locals.map_path[5] = (char)locals.temp_int;
    locals.temp_int = FUN_00417dc6(locals.map_path);
    g_loadsave_slot_has_data[locals.slot_index] = locals.temp_int;
    if (g_loadsave_slot_has_data[locals.slot_index] == 0)
    {
      strcpy(g_loadsave_slot_descriptions[locals.slot_index], gs_loadsave_0077d1b0[2]);
    }
    if (param_1 != 0)
    {
      g_loadsave_slot_has_data[locals.slot_index] = 1;
    }
  }

  if (g_loadsave_menu_controls[0].base_x == g_loadsave_menu_controls[0].x)
  {
    for (locals.slot_index = 0; locals.slot_index < 10; locals.slot_index = locals.slot_index + 1)
    {
      locals.temp_int = ScaleUiCoordinate(g_loadsave_menu_controls[locals.slot_index].x);
      g_loadsave_menu_controls[locals.slot_index].x = locals.temp_int;
      locals.temp_int = ScaleUiCoordinate(g_loadsave_menu_controls[locals.slot_index].y);
      g_loadsave_menu_controls[locals.slot_index].y = locals.temp_int;
      locals.temp_int = ScaleUiCoordinate(g_loadsave_menu_controls[locals.slot_index].width);
      g_loadsave_menu_controls[locals.slot_index].width = locals.temp_int;
      locals.temp_int = ScaleUiCoordinate(g_loadsave_menu_controls[locals.slot_index].height);
      g_loadsave_menu_controls[locals.slot_index].height = locals.temp_int;
    }
  }

restart_menu_loop:
  locals.menu_context = FUN_00500321();
  FUN_005000fb(locals.menu_context);
  if ((param_1 == 0) && (g_loadsave_skip_esc == 0))
  {
    locals.menu_control_count = 10;
  }
  else
  {
    locals.menu_control_count = 0xb;
  }
  FUN_0050014e(g_loadsave_menu_controls, locals.menu_control_count, locals.menu_context);
  for (locals.slot_index = 0; locals.slot_index < 10; locals.slot_index = locals.slot_index + 1)
  {
    if (g_loadsave_slot_has_data[locals.slot_index] == 0)
    {
      FUN_004ffcb4(&g_loadsave_menu_controls[locals.slot_index]);
      DrawLoadSaveSlotEntry(locals.slot_index, 3);
    }
    else
    {
      RenderAdvMenuControlNormally(&g_loadsave_menu_controls[locals.slot_index]);
      DrawLoadSaveSlotEntry(locals.slot_index, 0);
    }
  }

  g_loadsave_menu_selection = -1;
  while (g_loadsave_menu_selection == -1)
  {
    FUN_005003b7(DAT_007898f4, DAT_007898f8, DAT_00986d94);
  }
  FUN_0050035e();
  FUN_005000fb(locals.menu_context);
  if (g_loadsave_menu_selection == 0xe)
  {
    AnimatePaletteToColor(0, DAT_00589dec);
    return -1;
  }
  if (param_1 == 0)
  {
save_and_return:
    fclose(locals.save_desc_file);
    FreeSpriteBlob(g_loadsave_frame_sprites[0]);
    AnimatePaletteToColor(0, DAT_00589dec);
    return g_loadsave_menu_selection;
  }

  locals.selected_slot = g_loadsave_menu_selection - 4;
  strcpy(locals.saved_slot_name, g_loadsave_slot_descriptions[locals.selected_slot]);
  g_loadsave_edit_active = 1;
  memset(locals.edit_buffer + 1, 0, 0x100);
  strcpy(locals.edit_buffer + 1, g_loadsave_slot_descriptions[locals.selected_slot]);
  locals.temp_int = strcmp(locals.edit_buffer + 1, gs_loadsave_0077d1b0[2]);
  if (locals.temp_int == 0)
  {
    locals.edit_buffer[1] = '\0';
  }
  g_loadsave_cursor_index = strlen(locals.edit_buffer + 1);
  DrawLoadSaveSlotEntry(locals.selected_slot, 2);
  do
  {
    locals.key_code = PopQueuedKeyInput();
    locals.text_index = g_loadsave_cursor_index;
    if (locals.key_code == 0x1c0d)
    {
      g_loadsave_edit_active = 0;
      fseek(locals.save_desc_file, 0, 0);
      for (locals.slot_index = 0; locals.slot_index < 10; locals.slot_index = locals.slot_index + 1)
      {
        fprintf(locals.save_desc_file, g_loadsave_description_line_format, g_loadsave_slot_descriptions[locals.slot_index]);
      }
      goto save_and_return;
    }
    if ((int)locals.key_code < 0xe09)
    {
      if (locals.key_code == 0xe08)
      {
        if (g_loadsave_cursor_index != 0)
        {
          g_loadsave_cursor_index = g_loadsave_cursor_index - 1;
          strcpy(locals.edit_buffer + locals.text_index, locals.edit_buffer + locals.text_index + 1);
        }
      }
      else
      {
        if (locals.key_code == 0x11b)
        {
          break;
        }
        goto handle_name_char;
      }
    }
    else if ((int)locals.key_code < 0xf10)
    {
      if (locals.key_code == 0xf0f)
      {
        g_loadsave_cursor_index = g_loadsave_cursor_index - 8;
        if ((int)g_loadsave_cursor_index < 1)
        {
          g_loadsave_cursor_index = 0;
        }
      }
      else
      {
        if (locals.key_code != 0xf09)
        {
          goto handle_name_char;
        }
        g_loadsave_cursor_index = g_loadsave_cursor_index + 8;
      }
    }
    else if ((int)locals.key_code < 0x4701)
    {
      if (locals.key_code == 0x4700)
      {
        g_loadsave_cursor_index = 0;
      }
      else if (locals.key_code != 0x1c0d)
      {
        goto handle_name_char;
      }
    }
    else if ((int)locals.key_code < 0x4d01)
    {
      if (locals.key_code == 0x4d00)
      {
        locals.text_index = strlen(locals.edit_buffer + 1);
        if (locals.text_index == g_loadsave_cursor_index)
        {
          strcat(locals.edit_buffer + 1, g_loadsave_space_string);
        }
        g_loadsave_cursor_index = g_loadsave_cursor_index + 1;
      }
      else
      {
        if (locals.key_code != 0x4b00)
        {
          goto handle_name_char;
        }
        if (-1 < (int)g_loadsave_cursor_index)
        {
          g_loadsave_cursor_index = g_loadsave_cursor_index - 1;
        }
      }
    }
    else if (locals.key_code == 0x4f00)
    {
      g_loadsave_cursor_index = strlen(locals.edit_buffer + 1);
    }
    else if (locals.key_code == 0x5200)
    {
      g_loadsave_insert_mode = g_loadsave_insert_mode ^ 1;
    }
    else
    {
      if (locals.key_code != 0x5300)
      {
        goto handle_name_char;
      }
      locals.text_index = strlen(locals.edit_buffer + 1);
      if ((int)g_loadsave_cursor_index < (int)locals.text_index)
      {
        strcpy(locals.edit_buffer + g_loadsave_cursor_index + 1, locals.edit_buffer + g_loadsave_cursor_index + 2);
      }
    }
    goto redraw_edited_slot;

handle_name_char:
    locals.key_code = locals.key_code & 0xff;
    if ((((0x40 < locals.key_code) && (locals.key_code < 0x5b)) || ((0x60 < locals.key_code) && (locals.key_code < 0x7b))) ||
        (((0x2f < locals.key_code) && (locals.key_code < 0x3a)) || (locals.key_code == 0x20)))
    {
      if (g_loadsave_insert_mode != 0)
      {
        memmove(locals.edit_buffer + g_loadsave_cursor_index + 2, locals.edit_buffer + g_loadsave_cursor_index + 1,
                0xff - g_loadsave_cursor_index);
      }
      locals.temp_int = (char)locals.key_code;
      locals.edit_buffer[g_loadsave_cursor_index + 1] = (char)locals.temp_int;
      g_loadsave_cursor_index = g_loadsave_cursor_index + 1;
    }

redraw_edited_slot:
    strcpy(g_loadsave_slot_descriptions[locals.selected_slot], locals.edit_buffer + 1);
    DrawLoadSaveSlotEntry(locals.selected_slot, 2);
  } while (1);

  g_loadsave_edit_active = 0;
  strcpy(g_loadsave_slot_descriptions[locals.selected_slot], locals.saved_slot_name);
  goto restart_menu_loop;
}

// FUNCTION: SHANDALAR 0x004a97e7
int HandleLoadSaveSlotControlEvent(AdvMenuControl *control, int event_type)
{
  int is_inside_control_bounds;

  if (DAT_00747ee0 == 0)
  {
    if ((DAT_00986d9c < control->x) || (control->x + control->width < DAT_00986d9c))
    {
      is_inside_control_bounds = 0;
    }
    else if ((DAT_00986d98 < control->y) || (control->y + control->height < DAT_00986d98))
    {
      is_inside_control_bounds = 0;
    }
    else
    {
      is_inside_control_bounds = 1;
    }

    if (is_inside_control_bounds == 0)
    {
      return 0;
    }
  }

  if (control->state == 3)
  {
    return 0;
  }

  DrawLoadSaveSlotEntry(control->selection_value - 4, event_type);
  if ((event_type == 2) && (control->on_activate != (AdvMenuActivateCallback)0))
  {
    control->on_activate(control);
  }
  return 1;
}

// FUNCTION: SHANDALAR 0x004a98de
int ActivateLoadSaveSlotControl(AdvMenuControl *control)
{
  FUN_005626b0("x:sound\\button2.wav", 0xf, 100, 100, 0);
  g_loadsave_menu_selection = control->selection_value;
  return 0;
}

// FUNCTION: SHANDALAR 0x004ffd0a
int RenderAdvMenuControlNormally(AdvMenuControl *control)
{
  int old_state;

  old_state = control->state;
  control->state = 0;
  DAT_00747ee0 = 1;
  control->on_render(control, 0);
  DAT_00747ee0 = 0;
  return old_state;
}

// FUNCTION: SHANDALAR 0x0056c9b0
int ShowHallBackgroundScreen(void)
{
  FUN_0057c7e0(1, "hallback.pic");
  StretchBlitGraphicsRect(PTR_DAT_005832dc, 0, 0, 0x280, 0x1e0, PTR_DAT_005832b4, 0, 0, global_screen_width,
                          global_screen_height);
  FUN_005597ca();
  return 0;
}
