#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <process.h>

#include "defs.h"
#include "shandalar.h"
#include "shandalar_internal.h"
#include "magic/src/global_state.h"
#include "facemaker/src/facemaker_types.h"
#include "drawcardlib/src/pic.h"

extern char *gs_loadsave_0077d1b0[3];
extern HWND g_main_window_hwnd;
extern char g_text_lines[249][300];


void LoadPcxResource(int page_number, int x, int y, char *path, void *opaque);
void LoadPcxIntoPage(int page_number, char *path);
void set_global_base_directory(char *path);
void __cdecl DrawFormattedTextNoShadowCentered(FacemakerWindowBounds *dst, int text_color, int x, int y, char *format, ...);
int Palette_FindNearestEntryIndex(int target_r, int target_g, int target_b, unsigned char *palette_bytes);
void WriteGraphicsScanline(unsigned int *scanline_data, int page_number, int dst_x, int dst_y, unsigned int byte_count);
void DrawLoadSaveButtonText(FacemakerWindowBounds *window, int color, int x, int y, ...);
int RenderAdvMenuControlNormally(AdvMenuControl *control);
int ShowHallBackgroundScreen(void);
int RunNameEntryDialog(char *name_buffer);
int BlitTransparentRuns(FacemakerWindowBounds *src, int src_x, int src_y, unsigned int width, int height, FacemakerWindowBounds *dst, int dst_x, int dst_y);
int ApplyNameEntryKey(char *name_buffer, int key_code, int max_len);
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

int SetSelectedMenuValue(AdvMenuControl *control, int event_type);
int HandleEscControlEvent(AdvMenuControl *control, int event_type);
int HandleDifficultyMenuControlEvent(AdvMenuControl *control, int event_type);
int DrawDifficultyMenuEntry(int difficulty_index, int visual_state);
int HandleDifficultyMenuControlActivate(AdvMenuControl *control);
int DrawColorMenuEntry(int color_index, int visual_state);
int HandleColorMenuControlEvent(AdvMenuControl *control, int event_type);
int HandleColorMenuControlActivate(AdvMenuControl *control);

// GLOBAL: SHANDALAR 0x005aa0c8
EncodedImage *g_opening_menu_unused_sprite_group_d[2];
// GLOBAL: SHANDALAR 0x005aa3f8
EncodedImage *g_opening_menu_unused_sprite_group_a[3];
// GLOBAL: SHANDALAR 0x005aa408
EncodedImage *g_opening_menu_unused_sprite_group_b[2];
// GLOBAL: SHANDALAR 0x005aa410
EncodedImage *g_opening_menu_sprite_blob_handle[2];
// GLOBAL: SHANDALAR 0x005aa424
int g_opening_menu_strings_loaded;
// GLOBAL: SHANDALAR 0x005aa430
EncodedImage *g_opening_menu_icon_state_sprites[4];
// GLOBAL: SHANDALAR 0x005aa440
EncodedImage *g_opening_menu_unused_sprite_group_c[3];
// GLOBAL: SHANDALAR 0x005a9d8c
char **g_opening_menu_text_table;
// GLOBAL: SHANDALAR 0x005a9d90
EncodedImage *g_opening_menu_unused_sprite_group_e[2];
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
// GLOBAL: SHANDALAR 0x00587d30
char g_opening_menu_esc_hotkeys[] = "Ee\x1b";
// GLOBAL: SHANDALAR 0x00587d34
char g_opening_menu_esc_activate_hotkeys[4] = "\x1b";
// GLOBAL: SHANDALAR 0x00587d38
char g_difficulty_menu_esc_navigate_hotkeys[4] = "\x1b";
// GLOBAL: SHANDALAR 0x00587d3c
char g_difficulty_menu_esc_activate_hotkeys[4] = "\x1b";
// GLOBAL: SHANDALAR 0x00587d40
char g_color_menu_esc_navigate_hotkeys[4] = "\x1b";
// GLOBAL: SHANDALAR 0x00587d44
char g_color_menu_esc_activate_hotkeys[4] = "\x1b";
// GLOBAL: SHANDALAR 0x00781778
int g_name_entry_cursor;
// GLOBAL: SHANDALAR 0x00781774
int g_name_entry_insert_mode;
// GLOBAL: SHANDALAR 0x00789924
DIBSurface *g_facemaker_page4_dib;
// GLOBAL: SHANDALAR 0x00789928
HBITMAP g_facemaker_page4_bitmap;
extern char s_portrait_palette_map_empty_palette_path_0058d3bc[];
extern char s_portrait_palette_map_empty_portrait_path_0058d3c0[];
// GLOBAL: SHANDALAR 0x0058d3b4
char *g_portrait_palette_map_cached_palette_path = s_portrait_palette_map_empty_palette_path_0058d3bc;
// GLOBAL: SHANDALAR 0x0058d3b8
char *g_portrait_palette_map_cached_portrait_path = s_portrait_palette_map_empty_portrait_path_0058d3c0;
// GLOBAL: SHANDALAR 0x0058d3bc
char s_portrait_palette_map_empty_palette_path_0058d3bc[4] = "";
// GLOBAL: SHANDALAR 0x0058d3c0
char s_portrait_palette_map_empty_portrait_path_0058d3c0[4] = "";
// GLOBAL: SHANDALAR 0x00603ef8
RpBitsPalettePacket g_portrait_palette_source_palette;
// GLOBAL: SHANDALAR 0x00603bd0
RpBitsPalettePacket g_portrait_palette_target_palette;
// GLOBAL: SHANDALAR 0x00604218
unsigned char *g_portrait_palette_scan_cursor;
// GLOBAL: SHANDALAR 0x00604220
unsigned char g_portrait_palette_remap_table[0x100];
// GLOBAL: SHANDALAR 0x00604320
unsigned char g_portrait_tint_remap_table[0x100];
// GLOBAL: SHANDALAR 0x00587220
AdvMenuControl g_opening_menu_controls[4] = {
    {0xb8, 0x121, 0x115, 0x28, 0xb8, 0x121, 0x115, 0x28, 1, HandleOpeningMenuControlEvent, HandleOpeningMenuControlActivate, 1, 0, "Ss", (char *)0, 0, 0, {0, 0, 0, 0}},
    {0xb8, 0x14a, 0x115, 0x28, 0xb8, 0x14a, 0x115, 0x28, 1, HandleOpeningMenuControlEvent, HandleOpeningMenuControlActivate, 2, 0, "Ll", (char *)0, 0, 0, {0, 0, 0, 0}},
    {0xb8, 0x173, 0x115, 0x28, 0xb8, 0x173, 0x115, 0x28, 1, HandleOpeningMenuControlEvent, HandleOpeningMenuControlActivate, 3, 0, "rR", (char *)0, 0, 0, {0, 0, 0, 0}},
    {0xb8, 0x19c, 0x115, 0x28, 0xb8, 0x19c, 0x115, 0x28, 1, HandleOpeningMenuControlEvent, HandleOpeningMenuControlActivate, 5, 1, g_opening_menu_esc_hotkeys, g_opening_menu_esc_activate_hotkeys, 0, 0, {0, 0, 0, 0}}};

// GLOBAL: SHANDALAR 0x005873f0
AdvMenuControl g_difficulty_menu_controls[5] = {
    {0x118, 0x50, 0x10e, 0x54, 0x118, 0x50, 0x10e, 0x54, 1, HandleDifficultyMenuControlEvent, HandleDifficultyMenuControlActivate, 1, 0, (char *)0, (char *)0, 0, 0, {0, 0, 0, 0}},
    {0x118, 0xb4, 0x10e, 0x54, 0x118, 0xb4, 0x10e, 0x54, 1, HandleDifficultyMenuControlEvent, HandleDifficultyMenuControlActivate, 2, 0, (char *)0, (char *)0, 0, 0, {0, 0, 0, 0}},
    {0x118, 0x118, 0x10e, 0x54, 0x118, 0x118, 0x10e, 0x54, 1, HandleDifficultyMenuControlEvent, HandleDifficultyMenuControlActivate, 3, 0, (char *)0, (char *)0, 0, 0, {0, 0, 0, 0}},
    {0x118, 0x17c, 0x10e, 0x54, 0x118, 0x17c, 0x10e, 0x54, 1, HandleDifficultyMenuControlEvent, HandleDifficultyMenuControlActivate, 4, 0, (char *)0, (char *)0, 0, 0, {0, 0, 0, 0}},
    {-1, -1, -1, -1, -1, -1, -1, -1, 1, HandleEscControlEvent, (AdvMenuActivateCallback)SetSelectedMenuValue, 0, 0, g_difficulty_menu_esc_navigate_hotkeys, g_difficulty_menu_esc_activate_hotkeys, 0, 0, {0, 0, 0, 0}}};

// GLOBAL: SHANDALAR 0x005875d8
AdvMenuControl g_color_menu_controls[6] = {
    {0x33, 0x43, 0x10e, 0x4b, 0x33, 0x43, 0x10e, 0x4b, 1, HandleColorMenuControlEvent, HandleColorMenuControlActivate, 1, 0, (char *)0, (char *)0, 0, 0, {0, 0, 0, 0}},
    {0x33, 0x8e, 0x10e, 0x4b, 0x33, 0x8e, 0x10e, 0x4b, 1, HandleColorMenuControlEvent, HandleColorMenuControlActivate, 2, 0, (char *)0, (char *)0, 0, 0, {0, 0, 0, 0}},
    {0x33, 0xd9, 0x10e, 0x4b, 0x33, 0xd9, 0x10e, 0x4b, 1, HandleColorMenuControlEvent, HandleColorMenuControlActivate, 3, 0, (char *)0, (char *)0, 0, 0, {0, 0, 0, 0}},
    {0x33, 0x124, 0x10e, 0x4b, 0x33, 0x124, 0x10e, 0x4b, 1, HandleColorMenuControlEvent, HandleColorMenuControlActivate, 4, 0, (char *)0, (char *)0, 0, 0, {0, 0, 0, 0}},
    {0x33, 0x16f, 0x10e, 0x4b, 0x33, 0x16f, 0x10e, 0x4b, 1, HandleColorMenuControlEvent, HandleColorMenuControlActivate, 5, 0, (char *)0, (char *)0, 0, 0, {0, 0, 0, 0}},
    {-1, -1, -1, -1, -1, -1, -1, -1, 1, HandleEscControlEvent, (AdvMenuActivateCallback)SetSelectedMenuValue, 0, 0, g_color_menu_esc_navigate_hotkeys, g_color_menu_esc_activate_hotkeys, 0, 0, {0, 0, 0, 0}}};

// GLOBAL: SHANDALAR 0x00587598
AdvMenuRect g_difficulty_menu_preview_rects[4] = {{0x1c7, 0x4b, 0x7a, 0x5c}, {0x1c7, 0xaf, 0x7a, 0x5c}, {0x1c7, 0x113, 0x7a, 0x5c}, {0x1c7, 0x177, 0x7a, 0x5c}};

// GLOBAL: SHANDALAR 0x00781710
EncodedImage *g_color_menu_normal_swatch_sprites[5];
// GLOBAL: SHANDALAR 0x007817d0
EncodedImage *g_color_menu_pressed_swatch_sprites[5];
// GLOBAL: SHANDALAR 0x005aa0c4
int g_difficulty_menu_strings_loaded;
// GLOBAL: SHANDALAR 0x005aa3f0
int g_color_menu_strings_loaded;
// GLOBAL: SHANDALAR 0x005aa420
char **g_difficulty_caption_table;
// GLOBAL: SHANDALAR 0x005aa428
char **g_color_menu_flavor_table;
// GLOBAL: SHANDALAR 0x005aa42c
char **g_color_menu_color_name_table;
// GLOBAL: SHANDALAR 0x005a9d88
char **g_difficulty_option_table;
// GLOBAL: SHANDALAR 0x005a9d98
char **g_color_caption_table;
// GLOBAL: SHANDALAR 0x00587370
int g_menu_selection_value = -1;

// GLOBAL: SHANDALAR 0x005877d0
AdvMenuRect g_color_menu_swatch_rects[5] = {{0x33, 0x43, 0x46, 0x4b}, {0x33, 0x8e, 0x46, 0x4b}, {0x33, 0xd9, 0x46, 0x4b}, {0x33, 0x124, 0x46, 0x4b}, {0x33, 0x16f, 0x46, 0x4b}};
// GLOBAL: SHANDALAR 0x00587820
int g_color_menu_selection_to_deck_color[5] = {4, 5, 1, 3, 2};

// GLOBAL: SHANDALAR 0x00588a18
int g_loadsave_insert_mode = 1;
// GLOBAL: SHANDALAR 0x005aa470
int g_loadsave_menu_selection;
// GLOBAL: SHANDALAR 0x005aa478
EncodedImage *g_loadsave_frame_sprites[9];
// GLOBAL: SHANDALAR 0x005aa4a0
int g_loadsave_slot_has_data[10];
// GLOBAL: SHANDALAR 0x005aa4c8
EncodedImage *g_loadsave_slot_button_state_sprites[2][3];
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
  // This param is real but unused
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

  g_page0_window_bounds->page_number = 1;
  BlitGraphicsRect(g_page2_window_bounds, g_opening_menu_entry_rects[entry_index].x, g_opening_menu_entry_rects[entry_index].y,
                   global_screen_width - g_opening_menu_entry_rects[entry_index].x, ScaleUiCoordinate(0x2d), g_page1_window_bounds,
                   g_opening_menu_entry_rects[entry_index].x, g_opening_menu_entry_rects[entry_index].y);

  switch (entry_index)
  {
  case 0:
    s.unused_button_sprite_group = g_opening_menu_sprite_blob_handle;
    break;
  case 1:
    s.unused_button_sprite_group = g_opening_menu_unused_sprite_group_a;
    break;
  case 2:
    s.unused_button_sprite_group = g_opening_menu_unused_sprite_group_c;
    break;
  case 3:
    s.unused_button_sprite_group = g_opening_menu_unused_sprite_group_d;
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
    DrawEncodedImageResampled(g_page0_window_bounds, (int)(*(volatile int *)&s.pressed_offset_x) + s.icon_x,
                              s.icon_y + s.pressed_offset_y, g_opening_menu_icon_size, g_opening_menu_icon_size,
                              g_opening_menu_icon_state_sprites[s.icon_sprite_index]);
  }
  else
  {
    DrawEncodedImageResampled(g_page0_window_bounds, s.icon_x, s.icon_y, g_opening_menu_icon_size, g_opening_menu_icon_size,
                              g_opening_menu_icon_state_sprites[s.icon_sprite_index]);
  }

  g_page0_window_bounds->font_slot = 6;
  DrawFormattedTextShadowed(g_page0_window_bounds, (&s.label_x_0)[s.icon_sprite_index], s.icon_x + ScaleUiCoordinate(0x24),
                            (s.icon_y + g_opening_menu_icon_size / 2) - (GetFontStyleSize(6) / 2),
                            "%s",
                            g_opening_menu_text_table[s.text_table_index]);

  g_page0_window_bounds->page_number = 0;
  BlitGraphicsRect(g_page1_window_bounds, g_opening_menu_entry_rects[entry_index].x, g_opening_menu_entry_rects[entry_index].y,
                   global_screen_width - g_opening_menu_entry_rects[entry_index].x, ScaleUiCoordinate(0x2d), g_page0_window_bounds,
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
    EncodedImage *sprite_entry_pointers[100];
    int menu_context;
#ifdef MODERN_FIXES
    char map_name[100];
#else
    char *map_name;
#endif
    int loop_index;
  } s;

#ifdef MODERN_FIXES
  strcpy(s.map_name, "magic3.map");
#else
  s.map_name = "magic3.map";
#endif
  if (g_opening_menu_strings_loaded == 0)
  {
    g_opening_menu_text_table = LoadIniEscapedStringTable(g_advbuttons_ini_file, "openingMenu", g_ini_string_scratch);
    g_opening_menu_strings_loaded = 1;
  }

  SetFontStyleSize(6, ScaleUiCoordinate(0x14));

loop:
  strcpy(s.map_name, "magic3.map");
  g_menu_selection_value = -1;

  AnimatePaletteToColor(0, g_default_palette_fade_steps);
  LoadPcxResource(1, 0, 0, "menubak.pic",
                  (g_graphics_bpp == 8) ? &g_palette_data_words : (RpBitsPalettePacket *)1);

  StretchBlitGraphicsRect(g_page1_window_bounds, 0, 0, 0x280, 0x1e0, g_page0_window_bounds, 0, 0, global_screen_width,
                          global_screen_height);
  StretchBlitGraphicsRect(g_page1_window_bounds, 0, 0, 0x280, 0x1e0, g_page2_window_bounds, 0, 0, global_screen_width,
                          global_screen_height);

  FadeInPaletteFromGray(0, g_default_palette_fade_steps);
  g_page0_window_bounds->font_slot = 5;
  g_page0_window_bounds->font_slot = 1;

  g_opening_menu_entry_enabled[2] = FileExists(s.map_name);
  for (s.loop_index = 4; s.loop_index < 0xe; s.loop_index = s.loop_index + 1)
  {
    if (s.loop_index >= 10)
    {
      s.map_name[5] = (char)(s.loop_index + 'W');
    }
    else
    {
      s.map_name[5] = (char)(s.loop_index + '0');
    }

    if (FileExists(s.map_name) != 0)
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
    g_opening_menu_sprite_blob_handle[s.sprite_group_index] = s.sprite_entry_pointers[s.sprite_entry_index];
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

  s.menu_context = BeginMenuContext();
  ResetMenuContext(s.menu_context);
  AddMenuControlsToContext(g_opening_menu_controls, 4, s.menu_context);

  for (s.loop_index = 0; s.loop_index < 4; s.loop_index = s.loop_index + 1)
  {
    DrawOpeningMenuEntry(s.loop_index, ((g_opening_menu_entry_enabled[s.loop_index] == 0) ? 3 : 0));
    if (g_opening_menu_entry_enabled[s.loop_index] == 0)
    {
      RenderAdvMenuControlDisabled(&g_opening_menu_controls[s.loop_index]);
    }
  }

  while (g_menu_selection_value == -1)
  {
    UpdateMenuControlSelection(g_mouse_x_snapshot, g_mouse_y_snapshot, g_mouse_button_down_mask);
  }

  EndMenuContext();
  ResetMenuContext(s.menu_context);
  FreeSpriteBlob((void *)g_opening_menu_sprite_blob_handle[0]);
  g_page0_window_bounds->font_slot = 1;

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

  if (g_menu_render_guard == 0)
  {
    if ((g_mouse_x < control->x) || (control->width + control->x < g_mouse_x))
    {
      is_inside_control_bounds = 0;
    }
    else if ((g_mouse_y < control->y) || (control->y + control->height < g_mouse_y))
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
  PlaySoundEffectOnChannel("x:sound\\button2.wav", 0xf, 100, 100, 0);
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
    s.source_page = g_page1_window_bounds;
    break;
  case 1:
    s.source_page = g_page2_window_bounds;
    break;
  case 2:
    s.source_page = g_page2_window_bounds;
  default:
    break;
  }
  s.preview_x = g_difficulty_menu_preview_rects[difficulty_index].x;
  s.preview_y = g_difficulty_menu_preview_rects[difficulty_index].y;
  s.preview_width = (unsigned int)g_difficulty_menu_preview_rects[difficulty_index].width;
  s.preview_height = g_difficulty_menu_preview_rects[difficulty_index].height;

  if (visual_state == 2)
  {
    BlitGraphicsRect(g_page1_window_bounds, 200, s.preview_y - ScaleUiCoordinate(0x43), s.preview_width, s.preview_height, g_page1_window_bounds, 0, 0);
    StretchBlitGraphicsRect(s.source_page, s.preview_x, s.preview_y, s.preview_width, s.preview_height, g_page1_window_bounds, 4, 4, s.preview_width - 4,
                            s.preview_height - 4);
    CopyGraphicsRect(g_page1_window_bounds, 0, 0, s.preview_width, s.preview_height, g_page0_window_bounds, s.preview_x, s.preview_y);
  }
  else
  {
    CopyGraphicsRect(s.source_page, s.preview_x, s.preview_y, s.preview_width, s.preview_height, g_page0_window_bounds, s.preview_x, s.preview_y);
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
    g_difficulty_caption_table = LoadIniEscapedStringTable(g_advbuttons_ini_file, "diffCaption", g_ini_string_scratch);
    g_difficulty_option_table = LoadIniEscapedStringTable(g_advbuttons_ini_file, "diffs", g_ini_string_scratch);
    g_difficulty_menu_strings_loaded = 1;
  }

  AnimatePaletteToColor(0, g_default_palette_fade_steps);
  LoadPcxResource(1, 0, 0, "menu2.pic",
                  (g_graphics_bpp == 8) ? &g_palette_data_words : (RpBitsPalettePacket *)1);
  StretchBlitGraphicsRect(g_page1_window_bounds, 0, 0, 0x280, 0x1e0, g_page0_window_bounds, 0, 0, global_screen_width,
                          global_screen_height);

  g_page0_window_bounds->font_slot = 7;
  SetFontStyleSize(7, ScaleUiCoordinate(0x1b));
  DrawFormattedTextShadowedCentered(g_page0_window_bounds, 0x76, ScaleUiCoordinate(0x140), ScaleUiCoordinate(0x2d), "%s",
                                    g_difficulty_caption_table[0]);

  SetFontStyleSize(7, ScaleUiCoordinate(0x16));
  {
    int text_width;

    for (difficulty_index = 0; difficulty_index < 4; difficulty_index = difficulty_index + 1)
    {
      text_width = MeasureMultilineTextWidth(g_page0_window_bounds, g_difficulty_option_table[difficulty_index]);
      DrawFormattedTextShadowedCenterY(g_page0_window_bounds, 0x7b, ScaleUiCoordinate(0x1b0) - text_width,
                                       ScaleUiCoordinate(difficulty_index * 100 + 0x75),
                                       "%s", g_difficulty_option_table[difficulty_index]);
    }
  }

  StretchBlitGraphicsRect(g_page1_window_bounds, 0x1b1, 0x43, 0xa4, 0x19d, g_page1_window_bounds, 200, 0, ScaleUiCoordinate(0xa4),
                          ScaleUiCoordinate(0x19d));

  FadeInPaletteFromGray(0, g_default_palette_fade_steps);

  LoadPcxIntoPage(1, "menu2-norm.pic");
  StretchBlitGraphicsRect(g_page1_window_bounds, 0, 0, 0xa4, 0x19d, g_page1_window_bounds, ScaleUiCoordinate(0x1b1),
                          ScaleUiCoordinate(0x43), ScaleUiCoordinate(0xa4), ScaleUiCoordinate(0x19d));

  LoadPcxIntoPage(2, "menu2-hi.pic");
  StretchBlitGraphicsRect(g_page2_window_bounds, 0, 0, 0xa4, 0x19d, g_page2_window_bounds, ScaleUiCoordinate(0x1b1),
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

  menu_context = BeginMenuContext();
  ResetMenuContext(menu_context);
  AddMenuControlsToContext(g_difficulty_menu_controls, 5, menu_context);

  for (difficulty_index = 0; difficulty_index < 4; difficulty_index = difficulty_index + 1)
  {
    DrawDifficultyMenuEntry(difficulty_index, 0);
  }

  g_menu_selection_value = -1;
  while (g_menu_selection_value == -1)
  {
    UpdateMenuControlSelection(g_mouse_x_snapshot, g_mouse_y_snapshot, g_mouse_button_down_mask);
  }

  EndMenuContext();
  ResetMenuContext(menu_context);
  return g_menu_selection_value - 1;
}

// FUNCTION: SHANDALAR 0x004a168a
int HandleDifficultyMenuControlEvent(AdvMenuControl *control, int event_type)
{
  int is_inside_control_bounds;

  if (g_menu_render_guard == 0)
  {
    if ((g_mouse_x < *(int *)((int)control + 0x10)) || (*(int *)((int)control + 0x18) + *(int *)((int)control + 0x10) < g_mouse_x))
    {
      is_inside_control_bounds = 0;
    }
    else if ((g_mouse_y < control->y) || (control->y + control->height < g_mouse_y))
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
  PlaySoundEffectOnChannel("x:sound\\button2.wav", 0xf, 100, 100, 0);
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
    DrawEncodedImageResampled(g_page2_window_bounds, s.swatch_x + 2, s.swatch_y + 2, s.swatch_width - 4, s.swatch_height - 4,
                              *(s.swatch_sprite_table + color_index));
    BlitGraphicsRect(g_page2_window_bounds, s.swatch_x, s.swatch_y, s.swatch_width, s.swatch_height, g_page0_window_bounds, s.swatch_x, s.swatch_y);
  }
  else
  {
    DrawEncodedImageResampled(g_page0_window_bounds, s.swatch_x, s.swatch_y, s.swatch_width, s.swatch_height, *(s.swatch_sprite_table + color_index));
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
    g_color_caption_table = LoadIniEscapedStringTable(g_advbuttons_ini_file, "colorCaption", g_ini_string_scratch);
    g_color_menu_color_name_table = LoadIniEscapedStringTable(g_advbuttons_ini_file, "colorMenuColors", g_ini_string_scratch);
    g_color_menu_flavor_table = LoadIniEscapedStringTable(g_advbuttons_ini_file, "colorMenuFlavor", g_ini_string_scratch);
    g_color_menu_strings_loaded = 1;
  }

  AnimatePaletteToColor(0, g_default_palette_fade_steps);
  if (g_graphics_bpp == 8)
  {
    ClearGraphicsPageWithPaletteColor(0, 0);
  }

  LoadPcxResource(1, 0, 0, "menu3.pic", (g_graphics_bpp == 8) ? &g_palette_data_words : (RpBitsPalettePacket *)1);
  LoadPcxIntoPageNoPalette("menu3.pic");
  StretchBlitGraphicsRect(g_page1_window_bounds, 0, 0, 0x280, 0x1e0, g_page2_window_bounds, 0, 0, global_screen_width, global_screen_height);
  BlitGraphicsRect(g_page2_window_bounds, 0, 0, global_screen_width, global_screen_height, g_page1_window_bounds, 0, 0);

  g_page1_window_bounds->font_slot = 7;
  SetFontStyleSize(7, ScaleUiCoordinate(0x1b));
  DrawFormattedTextShadowedCentered(g_page1_window_bounds, 0x9b, ScaleUiCoordinate(0x140), ScaleUiCoordinate(0x22), "%s",
                                    g_color_caption_table[0]);

  SetFontStyleSize(7, ScaleUiCoordinate(0x12));
  for (s.color_index = 0; s.color_index < 5; s.color_index = s.color_index + 1)
  {
    DrawFormattedTextShadowed(g_page1_window_bounds, 0xa3, ScaleUiCoordinate(0x7d), ScaleUiCoordinate(s.color_index * 0x4c + 0x4c), "%s",
                              g_color_menu_color_name_table[s.color_index]);
  }

  SetFontStyleSize(7, ScaleUiCoordinate(10));
  for (s.color_index = 0; s.color_index < 5; s.color_index = s.color_index + 1)
  {
    DrawFormattedTextShadowed(g_page1_window_bounds, 0x71, ScaleUiCoordinate(0x85), ScaleUiCoordinate(s.color_index * 0x4c + 100), "%s",
                              g_color_menu_flavor_table[s.color_index]);
  }

  AnimatePaletteToColor(0, 4);
  BlitGraphicsRect(g_page1_window_bounds, 0, 0, global_screen_width, global_screen_height, g_page0_window_bounds, 0, 0);
  LoadPcxResource(-1, 0, 0, "menu3.pic", &g_palette_data_words);
  FadeInPaletteFromGray(0, g_default_palette_fade_steps);

  *(AdvMenuRect *)&s.restore_rect_x = *PushGraphicsClipRect((AdvMenuRect *)s.temp_rect_buffer, g_page2_window_bounds, 0, 0, global_screen_width, global_screen_height);

  BeginSpriteEncodeSession();
  LoadPcxIntoPageOpaque(1, "menu3-but1.pic");
  for (s.color_index = 0; s.color_index < 5; s.color_index = s.color_index + 1)
  {
    g_color_menu_normal_swatch_sprites[s.color_index] = EncodeSpriteFromPage(1, 0, s.color_index * 0x4b, 0x46, 0x4b);
  }

  LoadPcxIntoPageOpaque(1, "menu3but.pic");
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

  s.menu_context = BeginMenuContext();
  ResetMenuContext(s.menu_context);
  AddMenuControlsToContext(g_color_menu_controls, 6, s.menu_context);
  for (s.color_index = 0; s.color_index < 5; s.color_index = s.color_index + 1)
  {
    DrawColorMenuEntry(s.color_index, 0);
  }

  g_menu_selection_value = -1;
  while (g_menu_selection_value == -1)
  {
    UpdateMenuControlSelection(g_mouse_x_snapshot, g_mouse_y_snapshot, g_mouse_button_down_mask);
  }

  EndMenuContext();
  ResetMenuContext(s.menu_context);
  FreeSpriteBlob(g_color_menu_normal_swatch_sprites[0]);
  PushGraphicsClipRect((AdvMenuRect *)s.restore_rect_buffer, g_page2_window_bounds, s.restore_rect_x, s.restore_rect_y, s.restore_rect_width, s.restore_rect_height);

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

  if (g_menu_render_guard == 0)
  {
    if ((g_mouse_x < control->x) || (control->x + control->width < g_mouse_x))
    {
      is_inside_control_bounds = 0;
    }
    else if ((g_mouse_y < control->y) || (control->y + control->height < g_mouse_y))
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
  PlaySoundEffectOnChannel("x:sound\\button2.wav", 0xf, 100, 100, 0);
  g_menu_selection_value = control->selection_value;
  return 0;
}

// FUNCTION: SHANDALAR 0x004a21ad
int RunFacemakerFlow(void)
{
  struct
  {
    int old_page_rect[3];
    char *pedestal_images[6];
    int parse_result;
    FacemakerWindowBounds page4_bounds;
    FacemakerWindowBounds *page4_bounds_ptr;
    int y;
    DIBSurface *page4_dib;
    int x;
    int image_height;
    int image_width;
    char *pixel;
    EncodedImage *selected_menu_entry;
    char facemaker_path[MAX_PATH];
    int spawn_result;
    EncodedImage *page2_sprite;
    unsigned int is_topmost_window;
  } s;

  DelayUiTicks(0x14);
  set_global_base_directory(s.facemaker_path);
  strcat(s.facemaker_path, "\\Facemaker.exe");

  do
  {
    s.is_topmost_window = (unsigned int)((GetWindowLongA(g_main_window_hwnd, -0x14) & 8) != 0);
    if (s.is_topmost_window != 0)
    {
      SetWindowPos(g_main_window_hwnd, (HWND)-2, 0, 0, 0, 0, 3);
    }

    s.spawn_result = (int)_spawnl(0, s.facemaker_path, s.facemaker_path, "/S", 0);
    BringWindowToTop(g_main_window_hwnd);
    SetForegroundWindow(g_main_window_hwnd);
    SetFocus(g_main_window_hwnd);
    if (s.spawn_result == -1)
    {
      return -1;
    }

    s.page4_bounds.page_number = 4;
    s.page4_bounds.clip_left = 0;
    s.page4_bounds.clip_top = 0;
    s.page4_bounds.max_x = 800;
    s.page4_bounds.max_y = 600;
    s.page4_bounds.draw_shadow_enabled = 1;
    s.page4_bounds.text_color = 0x0f;
    s.page4_bounds.unk_1c = 4;
    s.page4_bounds.font_slot = 0;
    s.page4_bounds_ptr = &s.page4_bounds;

    s.selected_menu_entry = g_face_preview_sprite_group[g_menu_selection_value];
    (void)s.selected_menu_entry;

    BeginSpriteEncodeSession();
    s.page2_sprite = EncodeSpriteFromPage(2, 0, 0, 0x89, 0xa9);
    FinalizeSpriteEncodeSession();

    s.image_width = 0x89;
    s.image_height = 0xa9;
    s.page4_dib = (DIBSurface *)CreateGraphicsPage(4, s.image_width * 2, s.image_height, 8);
    SetGraphicsPage(4, s.page4_dib);
    PushGraphicsClipRect((AdvMenuRect *)s.old_page_rect, s.page4_bounds_ptr, 0, 0, s.image_width * 2, s.image_height);
    LoadPcxIntoPageNoPalette("menu4.pic");
    FillGraphicsRect(s.page4_bounds_ptr, 0, 0, s.image_width, s.image_height, 0);
    BlitGraphicsRect(g_page2_window_bounds, 0, 0, 0x8a, 0xaa, s.page4_bounds_ptr, 0, 0);

    for (s.y = 0; s.y < s.image_height; s.y = s.y + 1)
    {
      s.pixel = (char *)g_graphics_pages[4]->pBits + (g_graphics_pages[4]->width + g_graphics_pages[4]->rowPadding) * s.y;
      for (s.x = 0; s.image_width > s.x; s.x = s.x + 1, s.pixel = s.pixel + 1)
      {
        if ((unsigned char)*s.pixel == '\0')
        {
          s.pixel[s.image_width] = (char)0xff;
        }
      }
    }

    g_facemaker_page4_bitmap = g_graphics_pages[4]->hBitmap;
    g_facemaker_page4_dib = g_graphics_pages[4];
    SelectObject(g_graphics_pages[4]->hTempDC, g_graphics_pages[4]->hPreviousBitmap);
    BuildFacemakerPortraitSprites(g_page2_window_bounds);
    g_graphics_pages[4] = (DIBSurface *)0;

    s.pedestal_images[1] = "prdblk.pic";
    s.pedestal_images[2] = "prdblu.pic";
    s.pedestal_images[3] = "prdgrn.pic";
    s.pedestal_images[4] = "prdrd.pic";
    s.pedestal_images[5] = "prdwt.pic";

    LoadPcxIntoPageNoPalette("pedstls.pic");
    LoadPcxResource(1, global_screen_width - 0x280, global_screen_height - 0x1e0,
                    s.pedestal_images[((int *)g_color_menu_selection_to_deck_color - 1)[g_menu_selection_value]],
                    (g_graphics_bpp == 8) ? &g_palette_data_words : 1);
    StretchBlitGraphicsRect(g_page1_window_bounds, global_screen_width - 0x280, global_screen_height - 0x1e0, 0x280, 0x1e0,
                            g_page1_window_bounds, 0, 0, global_screen_width, global_screen_height);

    DrawEncodedImageResampled(g_page1_window_bounds, global_screen_width / 2 - ScaleUiCoordinate(0x89), ScaleUiCoordinate(0x14),
                              ScaleUiCoordinate(0x112), ScaleUiCoordinate(0x152), s.page2_sprite);
    BlitGraphicsRect(g_page1_window_bounds, 0, 0, global_screen_width, global_screen_height, g_page0_window_bounds, 0, 0);

    memset(g_name_entry_buffer, 0, 0x40);
    ReadGraphicsScanline((unsigned int *)g_name_entry_buffer, 2, 0, 200, 0x40);
    g_player_is_male = (g_name_entry_buffer[0] == 'm');
    strcpy(g_name_entry_buffer, g_name_entry_buffer + 1);
    g_name_entry_cursor = (int)strlen(g_name_entry_buffer);
    g_page0_window_bounds->font_slot = 1;
    ReadGraphicsScanline((unsigned int *)g_name_entry_buffer, 2, 1, 200, 0x3f);

    s.parse_result = RunNameEntryDialog(g_name_entry_buffer);
  } while (s.parse_result == -1);

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
  LoadPcxIntoPageOpaque(1, "namepick.pic");
  g_page1_window_bounds->font_slot = g_page0_window_bounds->font_slot;
  LoadTextSectionLines("ADVstrings.txt", "STARTUP");
  DrawFormattedTextNoShadowCentered(g_page1_window_bounds, 0xed, 0x8b, 0x19, &g_text_lines[2][0]);
  DrawFormattedTextNoShadowCentered(g_page1_window_bounds, 0xb4, 0x8a, 0x18, &g_text_lines[2][0]);
  BlitGraphicsRect(g_page1_window_bounds, 0, 0, 0x114, 0x6a, g_page1_window_bounds, 0, 200);
  DrawFormattedTextNoShadowCentered(g_page1_window_bounds, 0xb4, 0x8a, 0x100, "%s", name_buffer);
  BlitTransparentRuns(g_page1_window_bounds, 0, 200, 0x114, 0x6a, g_page0_window_bounds, (global_screen_width - 0x114) / 2, ScaleUiCoordinate(dialog_top));
  do
  {
    key_code = PopQueuedKeyInput();
    if (key_code == 0x1c0d)
    {
      break;
    }
    text_width = MeasureMultilineTextWidth(g_page1_window_bounds, name_buffer);
    if (key_code != 0)
    {
      if (key_code == 0x11b)
      {
        return -1;
      }
      ApplyNameEntryKey(name_buffer, (unsigned int)key_code, 0x19);
      BlitGraphicsRect(g_page1_window_bounds, 0, 0, 0x114, 0x6a, g_page1_window_bounds, 0, 200);
      DrawFormattedTextNoShadowCentered(g_page1_window_bounds, 0xb4, 0x8a, 0x100, "%s", name_buffer);
      BlitTransparentRuns(g_page1_window_bounds, 0, 200, 0x114, 0x6a, g_page0_window_bounds, (global_screen_width - 0x114) / 2, ScaleUiCoordinate(dialog_top));
    }
    DrawBlinkingNameCaret(g_page0_window_bounds, 0xb4, ScaleUiCoordinate(0x140) - text_width / 2, ScaleUiCoordinate(dialog_top) + 0x30, name_buffer,
                          g_name_entry_cursor);
  } while (1);

  WaitForInputEventUnlessBlocked();
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
  g_face_preview_sprite_group[0] = EncodeSpriteFromPage(page->page_number, 0, 0, 0x89, 0xa9);
  ApplyPortraitTintMap(page, 0, 0, 0x89, 0xa9, 0, 1);
  g_face_preview_sprite_selected = EncodeSpriteFromPage(page->page_number, 0, 0, 0x89, 0xa9);
  FinalizeSpriteEncodeSession();
}

// FUNCTION: SHANDALAR 0x00521ffa
int ApplyPortraitPaletteMap(FacemakerWindowBounds *page, int src_x, int src_y, unsigned int width, int height, char *palette_source_path, char *portrait_path)
{
  struct
  {
    unsigned int scanline_buffer[256];
    int y;
    int x;
    unsigned int green;
    unsigned int red;
    unsigned int blue;
  } s;

  if ((strcmp(g_portrait_palette_map_cached_palette_path, palette_source_path) != 0) || (strcmp(g_portrait_palette_map_cached_portrait_path, portrait_path) != 0))
  {
    LoadPcxResource(-1, 0, 0, palette_source_path, &g_portrait_palette_source_palette);
    LoadPcxResource(-1, 0, 0, portrait_path, &g_portrait_palette_target_palette);
    g_portrait_palette_scan_cursor = g_portrait_palette_source_palette.entry_data;
    g_portrait_palette_map_cached_palette_path = palette_source_path;
    g_portrait_palette_map_cached_portrait_path = portrait_path;
    for (s.x = 0; s.x < 0x100; s.x = s.x + 1)
    {
      s.red = (unsigned int)*g_portrait_palette_scan_cursor;
      g_portrait_palette_scan_cursor = g_portrait_palette_scan_cursor + 1;
      s.green = (unsigned int)*g_portrait_palette_scan_cursor;
      g_portrait_palette_scan_cursor = g_portrait_palette_scan_cursor + 1;
      s.blue = (unsigned int)*g_portrait_palette_scan_cursor;
      g_portrait_palette_scan_cursor = g_portrait_palette_scan_cursor + 1;
      g_portrait_palette_remap_table[s.x] =
          (unsigned char)Palette_FindNearestEntryIndex((int)s.red, (int)s.green, (int)s.blue, g_portrait_palette_target_palette.entry_data);
    }
  }

  for (s.y = src_y; s.y < src_y + height; s.y = s.y + 1)
  {
    ReadGraphicsScanline(s.scanline_buffer, page->page_number, src_x, s.y, width);
    for (s.x = 0; s.x < (int)width; s.x = s.x + 1)
    {
      ((unsigned char *)s.scanline_buffer)[s.x] = g_portrait_palette_remap_table[((unsigned char *)s.scanline_buffer)[s.x]];
    }
    WriteGraphicsScanline(s.scanline_buffer, page->page_number, src_x, s.y, width);
  }

  return 0;
}

// FUNCTION: SHANDALAR 0x00521e80
int ApplyPortraitTintMap(FacemakerWindowBounds *page, int src_x, int src_y, unsigned int width, int height, unsigned int tint_mask, int tint_mode)
{
  struct
  {
    unsigned int scanline_buffer[256];
    unsigned int tint_green;
    unsigned int tint_red;
    unsigned int tint_blue;
    int y;
    int x;
    int mixed_green;
    unsigned char *palette_entry;
    int mixed_red;
    int mixed_blue;
  } s;

  s.palette_entry = g_palette_data_words.entry_data;
  if (tint_mode != 0)
  {
    s.tint_red = tint_mask & 0xff;
    s.tint_green = *((unsigned char *)&tint_mask + 1);
    s.tint_blue = (tint_mask & 0xff0000) >> 0x10;
    for (s.x = 0; s.x < 0x100; s.x = s.x + 1)
    {
      s.mixed_red = ((int)*s.palette_entry + (int)s.tint_red) / 2;
      s.palette_entry = s.palette_entry + 1;
      s.mixed_green = ((int)*s.palette_entry + (int)s.tint_green) / 2;
      s.palette_entry = s.palette_entry + 1;
      s.mixed_blue = ((int)*s.palette_entry + (int)s.tint_blue) / 2;
      s.palette_entry = s.palette_entry + 1;
      g_portrait_tint_remap_table[s.x] =
          (unsigned char)Palette_FindNearestEntryIndex(s.mixed_red, s.mixed_green, s.mixed_blue, g_palette_data_words.entry_data);
    }
  }

  for (s.y = src_y; s.y < src_y + height; s.y = s.y + 1)
  {
    ReadGraphicsScanline(s.scanline_buffer, page->page_number, src_x, s.y, width);
    for (s.x = 0; s.x < (int)width; s.x = s.x + 1)
    {
      ((unsigned char *)s.scanline_buffer)[s.x] = g_portrait_tint_remap_table[((unsigned char *)s.scanline_buffer)[s.x]];
    }
    WriteGraphicsScanline(s.scanline_buffer, page->page_number, src_x, s.y, width);
  }

  return 0;
}

// FUNCTION: SHANDALAR 0x004a2713
int BlitTransparentRuns(FacemakerWindowBounds *src, int src_x, int src_y, unsigned int width, int height, FacemakerWindowBounds *dst, int dst_x, int dst_y)
{
  struct
  {
    int row;
    int x;
    unsigned int scanline_buffer[250];
    unsigned int *run_start;
    unsigned int run_length;
  } s;

  for (s.row = 0; s.row < height; s.row = s.row + 1)
  {
    ReadGraphicsScanline(s.scanline_buffer, src->page_number, src_x, src_y + s.row, width);
    if ((unsigned char)s.scanline_buffer[0] != '\0')
    {
      s.run_start = s.scanline_buffer;
    }
    else
    {
      s.run_start = (unsigned int *)0;
    }

    s.run_length = 0;
    for (s.x = 0; s.x < (int)width; s.x = s.x + 1)
    {
      if (*((unsigned char *)s.scanline_buffer + s.x) == '\0')
      {
        if (s.run_length != 0)
        {
          WriteGraphicsScanline(s.run_start, dst->page_number, ((int)s.run_start - (int)s.scanline_buffer) + dst_x, dst_y + s.row, s.run_length);
          s.run_length = 0;
          s.run_start = (unsigned int *)((char *)s.scanline_buffer + s.x);
        }
        else
        {
          s.run_start = (unsigned int *)((char *)s.scanline_buffer + s.x);
        }
      }
      else
      {
        s.run_length = s.run_length + 1;
      }
    }

    if (s.run_length != 0)
    {
      WriteGraphicsScanline(s.run_start, dst->page_number, ((int)s.run_start - (int)s.scanline_buffer) + dst_x, dst_y + s.row, s.run_length);
    }
  }

  return 0;
}

// FUNCTION: SHANDALAR 0x004a2e33
int ApplyNameEntryKey(char *name_buffer, int key_code, int max_len)
{
  switch (key_code)
  {

  case 0x4b00:
    if (g_name_entry_cursor >= 0)
    {
      g_name_entry_cursor--;
    }
    break;

  case 0x4d00:
    if (max_len > g_name_entry_cursor)
    {
      if (strlen(name_buffer) == (size_t)g_name_entry_cursor)
      {
        strcat(name_buffer, " ");
      }
      g_name_entry_cursor++;
    }
    break;

  case 0x1c0d:
    return 1;
    break;

  case 0xf09:
    g_name_entry_cursor += 8;
    g_name_entry_cursor = MIN(g_name_entry_cursor, max_len);
    break;

  case 0xf0f:
    g_name_entry_cursor -= 8;
    g_name_entry_cursor = MAX(g_name_entry_cursor, 0);
    break;

  case 0xe08:
    if (g_name_entry_cursor == 0)
    {
      break;
    }

    g_name_entry_cursor--;
    strcpy(name_buffer + g_name_entry_cursor, name_buffer + g_name_entry_cursor + 1);
    break;

  case 0x5300:
    if (g_name_entry_cursor < (int)strlen(name_buffer))
    {
      strcpy(name_buffer + g_name_entry_cursor, name_buffer + g_name_entry_cursor + 1);
    }
    break;

  case 0x5200:
    g_name_entry_insert_mode = g_name_entry_insert_mode ^ 1;
    break;

  case 0x4700:
    g_name_entry_cursor = 0;
    break;

  case 0x4f00:
    g_name_entry_cursor = (int)strlen(name_buffer);
    break;

  default:

    if ((int)strlen(name_buffer) < max_len)
    {
      key_code = key_code & 0xff;
      if (((key_code >= 'A' && key_code <= 'Z') ||
           (key_code >= 'a' && key_code <= 'z') ||
           (key_code >= '0' && key_code <= '9') ||
           (key_code == ' ')))
      {
        if (g_name_entry_insert_mode != 0)
        {
          memmove(name_buffer + g_name_entry_cursor + 1, name_buffer + g_name_entry_cursor, (max_len - g_name_entry_cursor) - 1);
        }
        name_buffer[g_name_entry_cursor++] = (char)key_code;
      }
    }
  }

  return 0;
}

// FUNCTION: SHANDALAR 0x004a8470
int HandleLoadSaveEscControlEvent(AdvMenuControl *control, int event_type)
{
  (void)control;
  if (event_type == 2)
  {
    PlaySoundEffectOnChannel("x:sound\\button2.wav", 0xf, 100, 100, 0);
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
  struct
  {
    size_t string_len;
    int i;
    int caret_width;
  } s;

  s.string_len = strlen(name_buffer);
  if ((GetUiTickCount() & 7) == 0)
  {
    return 0;
  }

  for (s.i = 0; s.i < cursor_pos; s.i = s.i + 1)
  {
    if (s.i < (int)s.string_len)
    {
      caret_x = caret_x + GetFontCharWidth(window->font_slot, name_buffer[s.i]);
    }
    else
    {
      caret_x = caret_x + GetFontCharWidth(window->font_slot, ' ');
    }
  }

  if (s.i < (int)s.string_len)
  {
    s.caret_width = GetFontCharWidth(window->font_slot, name_buffer[cursor_pos]);
  }
  else
  {
    s.caret_width = GetFontCharWidth(window->font_slot, ' ');
  }

  caret_y = caret_y + GetFontLineHeight(window->font_slot) / 2;
  caret_y = caret_y - 1;
  DrawGraphicsLine(window, caret_x, caret_y, caret_x + s.caret_width, caret_y, color_index);
  DrawGraphicsLine(window, caret_x, caret_y + 1, caret_x + s.caret_width, caret_y + 1, color_index);
}

// FUNCTION: SHANDALAR 0x004a85fc
int DrawLoadSaveButton(FacemakerWindowBounds *window, EncodedImage **button_sprites, int button_width, int button_height, char *button_text, int font_slot,
                       int visual_state)
{
  struct
  {
    size_t text_len;
    int text_width;
    int text_y;
    int x_cursor;
    int line_height;
    int text_color;
    EncodedImage *sprite;
  } s;

  s.x_cursor = 0;
  button_width = ScaleUiCoordinate(button_width);
  button_height = ScaleUiCoordinate(button_height);

  s.sprite = button_sprites[0];
  DrawEncodedImageResampled(window, s.x_cursor, 0, ScaleUiCoordinate(s.sprite->width), ScaleUiCoordinate(s.sprite->height), s.sprite);
  s.x_cursor += ScaleUiCoordinate(s.sprite->width);

  s.sprite = button_sprites[1];
  while (s.x_cursor < button_width)
  {
    DrawEncodedImageResampled(window, s.x_cursor, 0, ScaleUiCoordinate(s.sprite->width), ScaleUiCoordinate(s.sprite->height), s.sprite);
    s.x_cursor += ScaleUiCoordinate(s.sprite->width);
  }

  s.sprite = button_sprites[2];
  s.x_cursor = button_width - ScaleUiCoordinate(s.sprite->width);
  DrawEncodedImageResampled(window, s.x_cursor, 0, ScaleUiCoordinate(s.sprite->width), ScaleUiCoordinate(s.sprite->height), s.sprite);

  window->font_slot = font_slot;
  s.line_height = GetFontLineHeight(font_slot);
  s.text_width = MeasureMultilineTextWidth(window, button_text);
  while (s.text_width > button_width)
  {
    s.text_len = strlen(button_text);
    button_text[s.text_len - 1] = '\0';
    s.text_width = MeasureMultilineTextWidth(window, button_text);
  }

  s.x_cursor = 10;
  s.text_y = button_height / 2;
  s.text_color = 0x25;
  if (visual_state == 1)
  {
    s.text_color = 0x67;
  }
  if (visual_state == 2)
  {
    s.text_color = 0x67;
    s.text_y += 2;
    s.x_cursor -= 2;
  }
  if (visual_state == 3)
  {
    s.text_color = 0xf0;
  }

  DrawLoadSaveButtonText(window, s.text_color, s.x_cursor, s.text_y, button_text);
  if (g_loadsave_edit_active != 0)
  {
    DrawBlinkingNameCaret(window, s.text_color, s.x_cursor, s.text_y, button_text, g_loadsave_cursor_index);
  }
  return 0;
}

// FUNCTION: SHANDALAR 0x004a884f
int DrawLoadSaveSlotEntry(int slot_index, int visual_state)
{
  struct
  {
    int slot_y;
    int slot_x;
    int slot_h;
    int slot_w;
    EncodedImage **slot_state_sprites;
  } s;

  switch (visual_state)
  {
  case 0:
    s.slot_state_sprites = &g_loadsave_slot_button_state_sprites[0][0];
    break;
  case 1:
    s.slot_state_sprites = &g_loadsave_slot_button_state_sprites[0][0];
    break;
  case 2:
    s.slot_state_sprites = &g_loadsave_slot_button_state_sprites[1][0];
    break;
  case 3:
    s.slot_state_sprites = &g_loadsave_slot_button_state_sprites[0][0];
    break;
  }

  s.slot_x = g_loadsave_menu_controls[slot_index].x;
  s.slot_y = g_loadsave_menu_controls[slot_index].y;
  s.slot_w = g_loadsave_menu_controls[slot_index].width;
  s.slot_h = g_loadsave_menu_controls[slot_index].height;

  DrawLoadSaveButton(g_page1_window_bounds, s.slot_state_sprites, 0x168, 0x1b, g_loadsave_slot_descriptions[slot_index], 4, visual_state);
  StretchBlitGraphicsRect(g_page1_window_bounds, 0, 0, ScaleUiCoordinate(0x168), ScaleUiCoordinate(0x1a), g_page0_window_bounds,
                          s.slot_x, s.slot_y, s.slot_w, s.slot_h);
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
  struct
  {
    int cursor_y;
    int i;
    int right_span_x;
    EncodedImageHeader sprite_headers[9];
    int cursor_x;
    EncodedImage **sprite_table;
  } s;

  s.sprite_table = g_loadsave_frame_sprites;
  for (s.i = 0; s.i < 9; s.i = s.i + 1)
  {
    s.sprite_headers[s.i] = *(EncodedImageHeader *)s.sprite_table[s.i];
    s.sprite_headers[s.i].width = (short)ScaleUiCoordinate((int)s.sprite_headers[s.i].width);
    s.sprite_headers[s.i].height = (short)ScaleUiCoordinate((int)s.sprite_headers[s.i].height);
  }

  x = ScaleUiCoordinate(x);
  y = ScaleUiCoordinate(y);
  width = ScaleUiCoordinate(width);
  height = ScaleUiCoordinate(height);

  DrawEncodedImageResampled(window, (x + width / 2) - (int)s.sprite_headers[0].width / 2,
                            y - ((int)s.sprite_headers[0].height - (int)s.sprite_headers[5].height),
                            (int)s.sprite_headers[0].width, (int)s.sprite_headers[0].height, s.sprite_table[0]);

  s.cursor_x = ((x + width / 2) - (int)s.sprite_headers[0].width / 2) - (int)s.sprite_headers[5].width;
  s.right_span_x = ((int)s.sprite_headers[0].width / 2) + (width / 2) + x;
  while (x < s.cursor_x)
  {
    DrawEncodedImageResampled(window, s.cursor_x, y, (int)s.sprite_headers[5].width, (int)s.sprite_headers[5].height, s.sprite_table[5]);
    DrawEncodedImageResampled(window, s.right_span_x, y, (int)s.sprite_headers[5].width, (int)s.sprite_headers[5].height, s.sprite_table[5]);
    s.cursor_x -= (int)s.sprite_headers[5].width;
    s.right_span_x += (int)s.sprite_headers[5].width;
  }

  DrawEncodedImageResampled(window, x, y, (int)s.sprite_headers[1].width, (int)s.sprite_headers[1].height, s.sprite_table[1]);
  DrawEncodedImageResampled(window, (width + x) - (int)s.sprite_headers[2].width, y, (int)s.sprite_headers[2].width,
                            (int)s.sprite_headers[2].height, s.sprite_table[2]);

  s.cursor_y = (int)s.sprite_headers[1].height + y;
  s.right_span_x = (width + x) - (int)s.sprite_headers[8].width;
  while (s.cursor_y < (height + y) - (int)s.sprite_headers[3].height)
  {
    DrawEncodedImageResampled(window, x, s.cursor_y, (int)s.sprite_headers[8].width, (int)s.sprite_headers[8].height, s.sprite_table[8]);
    DrawEncodedImageResampled(window, s.right_span_x, s.cursor_y,
                              (int)s.sprite_headers[6].width, (int)s.sprite_headers[6].height, s.sprite_table[6]);
    s.cursor_y += (int)s.sprite_headers[8].height;
  }

  DrawEncodedImageResampled(window, x, (height + y) - (int)s.sprite_headers[3].height, (int)s.sprite_headers[3].width,
                            (int)s.sprite_headers[3].height, s.sprite_table[3]);
  DrawEncodedImageResampled(window, (width + x) - (int)s.sprite_headers[4].width, (height + y) - (int)s.sprite_headers[4].height,
                            (int)s.sprite_headers[4].width, (int)s.sprite_headers[4].height, s.sprite_table[4]);

  s.cursor_x = (int)s.sprite_headers[3].width + x;
  s.right_span_x = ((width + x) - (int)s.sprite_headers[4].width) - (int)s.sprite_headers[7].width;
  y = y + (height - (int)s.sprite_headers[7].height);
  while (s.cursor_x < x + width / 2)
  {
    DrawEncodedImageResampled(window, s.cursor_x, y, (int)s.sprite_headers[7].width, (int)s.sprite_headers[7].height, s.sprite_table[7]);
    DrawEncodedImageResampled(window, s.right_span_x, y, (int)s.sprite_headers[7].width, (int)s.sprite_headers[7].height, s.sprite_table[7]);
    s.cursor_x += (int)s.sprite_headers[7].width;
    s.right_span_x -= (int)s.sprite_headers[7].width;
  }

  return 0;
}

// FUNCTION: SHANDALAR 0x004a8da8
int RunLoadSaveMenu(int save_mode)
{
  struct
  {
    int menu_control_count;
    size_t text_index;
    int key_code;
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
  } s;

  s.map_path = "magic4.map";
  AnimatePaletteToColor(0, g_default_palette_fade_steps);
  LoadPcxResource(1, 0, 0, "menopt.pic", (g_graphics_bpp == 8) ? &g_palette_data_words : (void *)1);
  StretchBlitGraphicsRect(g_page1_window_bounds, 0, 0, 0x280, 0x1e0, g_page0_window_bounds, 0, 0, global_screen_width, global_screen_height);
  FadeInPaletteFromGray(0, g_default_palette_fade_steps);
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
  for (s.row = 0; s.row < 2; s.row = s.row + 1)
  {
    for (s.slot_index = 0; s.slot_index < 3; s.slot_index = s.slot_index + 1)
    {
      g_loadsave_slot_button_state_sprites[s.row][s.slot_index] =
          EncodeSpriteFromPage(1, s.slot_index * 0x41 + 1, s.row * 0x1c + 0x55, 0x1e, 0x1b);
    }
  }
  FinalizeSpriteEncodeSession();

  s.frame_x = 0x1c;
  s.frame_y = 0x45;
  s.frame_width = 0x1a4 - s.frame_x;
  s.frame_height = 0x18e - s.frame_y;
  DrawLoadSaveFrame(g_page0_window_bounds, s.frame_x, s.frame_y, s.frame_width, s.frame_height);

  if (save_mode != 0)
  {
    strcpy(g_ui_message_buffer, gs_loadsave_0077d1b0[0]);
  }
  else
  {
    strcpy(g_ui_message_buffer, gs_loadsave_0077d1b0[1]);
  }

  s.map_path[5] = '4';
  s.save_desc_file = fopen("saveDescs", "r+t");
  for (s.slot_index = 0; s.slot_index < 10; s.slot_index = s.slot_index + 1)
  {
    fgets(g_loadsave_slot_descriptions[s.slot_index], 0x40, s.save_desc_file);
    s.text_index = strlen(g_loadsave_slot_descriptions[s.slot_index]);
    (&g_loadsave_slot_descriptions[s.slot_index][0])[-1 + s.text_index] = '\0';
    s.map_path[5] = (char)int_to_hex_digit(s.slot_index + 4);
    g_loadsave_slot_has_data[s.slot_index] = FileExists(s.map_path);
    if (g_loadsave_slot_has_data[s.slot_index] == 0)
    {
      strcpy(g_loadsave_slot_descriptions[s.slot_index], gs_loadsave_0077d1b0[2]);
    }
    if (save_mode != 0)
    {
      g_loadsave_slot_has_data[s.slot_index] = 1;
    }
  }

  if (g_loadsave_menu_controls[0].base_x == g_loadsave_menu_controls[0].x)
  {
    for (s.slot_index = 0; s.slot_index < 10; s.slot_index = s.slot_index + 1)
    {
      g_loadsave_menu_controls[s.slot_index].x = ScaleUiCoordinate(g_loadsave_menu_controls[s.slot_index].x);
      g_loadsave_menu_controls[s.slot_index].y = ScaleUiCoordinate(g_loadsave_menu_controls[s.slot_index].y);
      g_loadsave_menu_controls[s.slot_index].width = ScaleUiCoordinate(g_loadsave_menu_controls[s.slot_index].width);
      g_loadsave_menu_controls[s.slot_index].height = ScaleUiCoordinate(g_loadsave_menu_controls[s.slot_index].height);
    }
  }

restart_menu_loop:
  s.menu_context = BeginMenuContext();
  ResetMenuContext(s.menu_context);
  if ((save_mode != 0) || (g_loadsave_skip_esc != 0))
  {
    s.menu_control_count = 0xb;
  }
  else
  {
    s.menu_control_count = 10;
  }
  AddMenuControlsToContext(g_loadsave_menu_controls, s.menu_control_count, s.menu_context);
  for (s.slot_index = 0; s.slot_index < 10; s.slot_index = s.slot_index + 1)
  {
    if (g_loadsave_slot_has_data[s.slot_index] == 0)
    {
      RenderAdvMenuControlDisabled(&g_loadsave_menu_controls[s.slot_index]);
      DrawLoadSaveSlotEntry(s.slot_index, 3);
    }
    else
    {
      RenderAdvMenuControlNormally(&g_loadsave_menu_controls[s.slot_index]);
      DrawLoadSaveSlotEntry(s.slot_index, 0);
    }
  }

  g_loadsave_menu_selection = -1;
  while (g_loadsave_menu_selection == -1)
  {
    UpdateMenuControlSelection(g_mouse_x_snapshot, g_mouse_y_snapshot, g_mouse_button_down_mask);
  }
  EndMenuContext();
  ResetMenuContext(s.menu_context);
  if (g_loadsave_menu_selection == 0xe)
  {
    AnimatePaletteToColor(0, g_default_palette_fade_steps);
    return -1;
  }
  if (save_mode != 0)
  {

    s.selected_slot = g_loadsave_menu_selection - 4;
    strcpy(s.saved_slot_name, g_loadsave_slot_descriptions[s.selected_slot]);
    g_loadsave_edit_active = 1;
    memset(s.edit_buffer + 1, 0, 0x100);
    strcpy(s.edit_buffer + 1, g_loadsave_slot_descriptions[s.selected_slot]);
    if (strcmp(s.edit_buffer + 1, gs_loadsave_0077d1b0[2]) == 0)
    {
      s.edit_buffer[1] = '\0';
    }
    g_loadsave_cursor_index = strlen(s.edit_buffer + 1);
    DrawLoadSaveSlotEntry(s.selected_slot, 2);

    while ((s.key_code = PopQueuedKeyInput()) != 0x1c0d)
    {
      switch (s.key_code)
      {
      case 0x4b00:
        if (g_loadsave_cursor_index >= 0)
        {
          g_loadsave_cursor_index = g_loadsave_cursor_index - 1;
        }
        break;

      case 0x4d00:
        if (strlen(s.edit_buffer + 1) == g_loadsave_cursor_index)
        {
          strcat(s.edit_buffer + 1, " ");
        }
        g_loadsave_cursor_index++;
        break;

      case 0xf09:
        g_loadsave_cursor_index += 8;
        break;

      case 0xf0f:
        g_loadsave_cursor_index -= 8;
        g_loadsave_cursor_index = MAX(g_loadsave_cursor_index, 0);
        break;

      case 0xe08:
        if (g_loadsave_cursor_index == 0)
        {
          break;
        }

        g_loadsave_cursor_index--;
        strcpy(s.edit_buffer + g_loadsave_cursor_index + 1, s.edit_buffer + g_loadsave_cursor_index + 2);
        break;

      case 0x5300:
        if (g_loadsave_cursor_index < (int)strlen(s.edit_buffer + 1))
        {
          strcpy(s.edit_buffer + g_loadsave_cursor_index + 1, s.edit_buffer + g_loadsave_cursor_index + 2);
        }
        break;

      case 0x5200:
        g_loadsave_insert_mode = g_loadsave_insert_mode ^ 1;
        break;

      case 0x4700:
        g_loadsave_cursor_index = 0;
        break;

      case 0x4f00:
        g_loadsave_cursor_index = strlen(s.edit_buffer + 1);
        break;

      case 0x11b:
        g_loadsave_edit_active = 0;
        strcpy(g_loadsave_slot_descriptions[s.selected_slot], s.saved_slot_name);

      case 0x1c0d:
        goto restart_menu_loop;

      default:
        s.key_code = s.key_code & 0xff;
        if ((((s.key_code >= 0x41) && (s.key_code <= 0x5a)) || ((s.key_code >= 0x61) && (s.key_code <= 0x7a))) ||
            (((s.key_code >= 0x30) && (s.key_code <= 0x39)) || (s.key_code == 0x20)))
        {
          if (g_loadsave_insert_mode != 0)
          {
            memmove(s.edit_buffer + g_loadsave_cursor_index + 2, s.edit_buffer + g_loadsave_cursor_index + 1,
                    (0x100 - g_loadsave_cursor_index) - 1);
          }
          s.edit_buffer[g_loadsave_cursor_index + 1] = (char)s.key_code;
          g_loadsave_cursor_index = g_loadsave_cursor_index + 1;
        }
      }

      strcpy(g_loadsave_slot_descriptions[s.selected_slot], s.edit_buffer + 1);
      DrawLoadSaveSlotEntry(s.selected_slot, 2);
    }

    g_loadsave_edit_active = 0;
    fseek(s.save_desc_file, 0, 0);
    for (s.slot_index = 0; s.slot_index < 10; s.slot_index = s.slot_index + 1)
    {
      fprintf(s.save_desc_file, "%s\n", g_loadsave_slot_descriptions[s.slot_index]);
    }
  }

  fclose(s.save_desc_file);
  FreeSpriteBlob(g_loadsave_frame_sprites[0]);
  AnimatePaletteToColor(0, g_default_palette_fade_steps);
  return g_loadsave_menu_selection;
}

// FUNCTION: SHANDALAR 0x004a97e7
int HandleLoadSaveSlotControlEvent(AdvMenuControl *control, int event_type)
{
  int is_inside_control_bounds;

  if (g_menu_render_guard == 0)
  {
    if ((g_mouse_x < control->x) || (control->x + control->width < g_mouse_x))
    {
      is_inside_control_bounds = 0;
    }
    else if ((g_mouse_y < control->y) || (control->y + control->height < g_mouse_y))
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
  PlaySoundEffectOnChannel("x:sound\\button2.wav", 0xf, 100, 100, 0);
  g_loadsave_menu_selection = control->selection_value;
  return 0;
}

// FUNCTION: SHANDALAR 0x0056c9b0
int ShowHallBackgroundScreen(void)
{
  LoadPcxIntoPageOpaque(1, "hallback.pic");
  StretchBlitGraphicsRect(g_page1_window_bounds, 0, 0, 0x280, 0x1e0, g_page0_window_bounds, 0, 0, global_screen_width,
                          global_screen_height);
  WaitForInputEventUnlessBlocked();
  return 0;
}
