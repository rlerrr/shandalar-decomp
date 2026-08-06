#include <windows.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "defs.h"
#include "shandalar.h"
#include "magic/src/global_state.h"
#include "magic/src/global_strings.h"
#include "shandalar_global_strings.h"
#include "facemaker/src/facemaker_types.h"
#include "drawcardlib/src/pic.h"

/* Menu / input globals (owned by shandalar.c) */
extern int global_screen_width;
extern int global_screen_height;
extern int g_menu_render_guard;
extern int g_menu_context_index;
extern int g_menu_prev_control_index;
extern int g_menu_current_control_index;
extern int g_menu_allow_arrow_nav_by_context[50];
extern int g_menu_control_count_by_context[50];
extern AdvMenuControl *g_menu_controls_by_context[50][50];

extern WorldMagicSlotTimer g_world_magic_slot_timers[0xc];
extern int g_mouse_x;
extern int g_mouse_y;
extern int g_mouse_button_down_mask;
extern int g_mouse_x_snapshot;
extern int g_mouse_y_snapshot;

extern FILE *g_advbuttons_ini_file;
extern char g_ini_string_scratch[0x28];
extern char g_ui_message_buffer[0x1000];

extern FacemakerWindowBounds *g_page0_window_bounds;
extern FacemakerWindowBounds *g_page1_window_bounds;

extern RpBitsPalettePacket g_palette_data_words;

/* World state */
extern int g_world_player_tile_x;
extern int g_world_player_tile_y;
extern int g_world_scroll_cache_ready;

extern int g_neighbor_dx[9];
extern int g_neighbor_dy[9];

/* Debug toggle used by multiple screens */
extern int g_reveal_all_world_info;

/* Sprites loaded by startup code (owned by shandalar.c) */
extern EncodedImage *g_ttsprite_grid_sprite_entries[0x40];
extern EncodedImage *g_ttsprite_aux_sprite_entries[8];
extern EncodedImage *g_world_magic_avatar_sprites[5];

extern FontSlot g_font_slots[0x10];
extern char g_world_ui_text_scratch_buffer[0x1000];

/* Engine / UI helpers */
int *LoadIniEscapedStringTable(FILE *ini_file, char *section_name, char *scratch);
void LoadPcxIntoPageNoPalette(char *path);
void LoadPcxIntoPage(int page_number, char *path);
void LoadPcxResource(int page_number, int x, int y, char *path, void *opaque);
void BeginSpriteEncodeSession(void);
EncodedImage *EncodeSpriteFromPage(int page_number, int x, int y, int width, int height);
void FinalizeSpriteEncodeSession(void);
void FreeSpriteBlob(void *memory);

void ClearGraphicsPageWithPaletteColor(int page_number, int palette_index);
void StretchBlitGraphicsRect(FacemakerWindowBounds *dst, int dst_x, int dst_y, int src_w, int src_h,
                             FacemakerWindowBounds *src, int src_x, int src_y, int copy_w, int copy_h);
void BlitGraphicsRect(FacemakerWindowBounds *dst, int dst_x, int dst_y, int width, int height,
                      FacemakerWindowBounds *src, int src_x, int src_y);

int ScaleUiCoordinate(int value);
int SetFontStyleSize(int font_slot, unsigned int point_size);
void DrawFormattedTextShadowedCentered(FacemakerWindowBounds *window, int color_index, int x, int y, char *format, ...);
void DrawTextAt(FacemakerWindowBounds *dst, int text_id, int x, int y, char *text, ...);
int MeasureMultilineTextWidth(FacemakerWindowBounds *dst, char *text);
int GetFontLineHeight(int font_slot);
int GetFontStyleSize(int font_slot);
void ApplyPortraitTintMap(FacemakerWindowBounds *dst, int x, int y, int w, int h, unsigned int tint, int maybe_shadow);

void DrawEncodedImageResampled(FacemakerWindowBounds *dst, int x, int y, int width, int height, EncodedImage *encoded_image);
void DrawEncodedImageUnscaled(FacemakerWindowBounds *dst, int x, int y, EncodedImage *encoded_image);
void FillGraphicsRect(FacemakerWindowBounds *window_bounds, int x, int y, int width, int height, unsigned int color_index);
void DrawGraphicsLine(FacemakerWindowBounds *window_bounds, int x1, int y1, int x2, int y2, int color_index);

int BeginMenuContext(void);
int ResetMenuContext(int context_index);
int AddMenuControlsToContext(AdvMenuControl *controls, int control_count, int context_index);
int EndMenuContext(void);
int RenderCurrentMenuContextControls(void);
int UpdateMenuControlSelection(int mouse_x, int mouse_y, int allow_activate_on_click);

void UpdateMouseSnapshot(void);
int ConsumeUiTickCount(void);
int GetUiTickCount(void);
void PopNormalizedQueuedKeyInput(void);
void ClearInputAndWaitForMouseRelease(void);

void EnsureAdvfac64Loaded(int state);
void PlaySoundEffectOnChannel(char *sound_path, int channel, int volume, int pitch_percent, int pan_percent);

unsigned int GetWorldTileType(int x, int y);
unsigned int GetWorldMapPixelFlags(int x, int y);
unsigned int WorldRoadTileHasDirection(int tile_x, int tile_y, char direction_index);
void ConvertWorldTileToMapScreenCoords(int tile_x, int tile_y, int *out_x, int *out_y);
void ConvertMapScreenToWorldTileCoords(int x, int y, int *out_tile_x, int *out_tile_y);
void DrawEncodedImageUiScaled(FacemakerWindowBounds *dst, int x_320, int y_200, EncodedImage *sprite, int width_320, int height_200);
int single_color_test_bit_to_color_t(int mask);
unsigned int GetWorldTileMagicMask(unsigned int tile_mask);
int FindWorldMagicCardIndex(int world_magic_slot_index);
int FindNearestTownIndex(int world_x, int world_y);
int FindTownAtWorldCoordinates(int world_x, int world_y);
char *GetPluralCardClassNameFromColorMask(unsigned int color_bitmask);

void DrawCenteredTextLineClamped(char *text, int center_x, int y, int color_index);

void ShowCityInfoScreen(int unused);

void FormatMessageFromStringStripCarriageReturns(char *dst, int max_length, char *format, ...);

int RenderMapMenuControl(AdvMenuControl *control, int mode);
int ActivateMapMenuControl(AdvMenuControl *control);

// GLOBAL: SHANDALAR 0x00650f20
int g_map_button_labels_loaded;
// GLOBAL: SHANDALAR 0x006527a4
char **g_map_button_labels;
// GLOBAL: SHANDALAR 0x00650f1c
EncodedImage *g_map_amulet_panel_sprite;
// GLOBAL: SHANDALAR 0x006506e0
EncodedImage *g_map_summary_panel_sprite;
// GLOBAL: SHANDALAR 0x0073ea94
EncodedImage *g_adv_scrollbar_track_sprite;
// GLOBAL: SHANDALAR 0x00746b04
EncodedImage *g_adv_scrollbar_thumb_sprite;
// GLOBAL: SHANDALAR 0x006506e8
int g_map_menu_selection;

// GLOBAL: SHANDALAR 0x0073eab0
EncodedImage *g_map_button_sprites[5][3];

// GLOBAL: SHANDALAR 0x00590a30
int g_map_tile_variant_primes[6] = {2, 3, 5, 7, 11, 13};

// GLOBAL: SHANDALAR 0x00590850
int g_map_button_slot_left_x = 48;
// GLOBAL: SHANDALAR 0x00590854
int g_map_button_slot_left_y = 5;
// GLOBAL: SHANDALAR 0x00590858
int g_map_button_slot_right_x = 148;
// GLOBAL: SHANDALAR 0x0059085c
int g_map_button_slot_right_y = 5;

// GLOBAL: SHANDALAR 0x00590b08
char g_map_view_nav_hotkeys[] = "Mm";
// GLOBAL: SHANDALAR 0x00590b0c
char g_map_view_activate_hotkeys[] = "Mm";
// GLOBAL: SHANDALAR 0x00590b10
char g_planeswalk_nav_hotkeys[] = "Pp";
// GLOBAL: SHANDALAR 0x00590b14
char g_planeswalk_activate_hotkeys[] = "Pp";
// GLOBAL: SHANDALAR 0x00590b18
char g_info_view_nav_hotkeys[] = "Ii";
// GLOBAL: SHANDALAR 0x00590b1c
char g_info_view_activate_hotkeys[] = "Ii";
// GLOBAL: SHANDALAR 0x00590b20
char g_city_list_nav_hotkeys[] = "Cc";
// GLOBAL: SHANDALAR 0x00590b24
char g_city_list_activate_hotkeys[] = "Cc";
// GLOBAL: SHANDALAR 0x00590b28
char g_map_done_nav_hotkeys[] = " \rDd";
// GLOBAL: SHANDALAR 0x00590b30
char g_map_done_activate_hotkeys[] = " \rDd";

// GLOBAL: SHANDALAR 0x00590888
AdvMenuControl g_map_menu_controls_00590888[5] = {
    {48, 5, 94, 28, 48, 5, 94, 28, 1, RenderMapMenuControl, ActivateMapMenuControl, 0, 0, g_map_view_nav_hotkeys, g_map_view_activate_hotkeys, 0, 0, {0, 0, 0, 0}},
    {48, 5, 94, 28, 48, 5, 94, 28, 1, RenderMapMenuControl, ActivateMapMenuControl, 2, 0, g_planeswalk_nav_hotkeys, g_planeswalk_activate_hotkeys, 0, 0, {0, 0, 0, 0}},
    {48, 5, 94, 28, 48, 5, 94, 28, 1, RenderMapMenuControl, ActivateMapMenuControl, 1, 0, g_info_view_nav_hotkeys, g_info_view_activate_hotkeys, 0, 0, {0, 0, 0, 0}},
    {398, 5, 94, 28, 398, 5, 94, 28, 1, RenderMapMenuControl, ActivateMapMenuControl, 3, 0, g_city_list_nav_hotkeys, g_city_list_activate_hotkeys, 0, 0, {0, 0, 0, 0}},
    {499, 5, 94, 28, 499, 5, 94, 28, 1, RenderMapMenuControl, ActivateMapMenuControl, 4, 0, g_map_done_nav_hotkeys, g_map_done_activate_hotkeys, 0, 0, {0, 0, 0, 0}},
};

// FUNCTION: SHANDALAR 0x00550164
void ConvertWorldTileToMapScreenCoords(int tile_x, int tile_y, int *out_x, int *out_y)
{
  *out_x = (tile_y + tile_x) * 6 - 0x40;
  *out_y = (tile_y - tile_x) * 6 + 200;
}

// FUNCTION: SHANDALAR 0x00550197
void ConvertMapScreenToWorldTileCoords(int x, int y, int *out_tile_x, int *out_tile_y)
{
  *out_tile_x = ((x + 0x40) - (y - 200)) / 0xc;
  *out_tile_y = *out_tile_x + (y - 200) / 6;
}

// FUNCTION: SHANDALAR 0x00428783
char *GetPluralCardClassNameFromColorMask(unsigned int color_bitmask)
{
  char *result;

  switch (color_bitmask)
  {
  case 1:
    result = gs_cardclassnames_plural_0077e1f0[0];
    break;
  case 2:
    result = gs_cardclassnames_plural_0077e1f0[1];
    break;
  case 4:
    result = gs_cardclassnames_plural_0077e1f0[2];
    break;
  case 8:
    result = gs_cardclassnames_plural_0077e1f0[3];
    break;
  case 0x10:
    result = gs_cardclassnames_plural_0077e1f0[4];
    break;
  case 0x20:
    result = gs_cardclassnames_plural_0077e1f0[5];
    break;
  case 0x40:
    result = gs_cardclassnames_plural_0077e1f0[6];
    break;
  case 0x42:
    result = gs_cardclassnames_plural_0077e1f0[7];
    break;
  case 0x80:
    result = gs_cardclassnames_plural_0077e1f0[8];
    break;
  default:
    result = "";
    break;
  }

  return result;
}

// FUNCTION: SHANDALAR 0x004310a4
void DrawCenteredTextLineWithShadow(char *text, int center_x, int y, int color_index)
{
  if (color_index != 0)
  {
    DrawCenteredTextLineClamped(text, center_x, y + 1, 0);
  }
  DrawCenteredTextLineClamped(text, center_x, y, color_index);
}

// FUNCTION: SHANDALAR 0x0054e756
int RenderMapMenuControl(AdvMenuControl *control, int mode)
{
  struct
  {
    int in_bounds;
    int sprite_state_col;
    int button_y;
    int button_x;
    int button_height;
    int button_width;
    int saved_page_number;
    int mapped_button_row;
  } s;

  if (g_menu_render_guard == 0)
  {
    if ((g_mouse_x < control->x) || ((control->x + control->width) < g_mouse_x))
    {
      s.in_bounds = 0;
    }
    else if ((g_mouse_y < control->y) || ((control->y + control->height) < g_mouse_y))
    {
      s.in_bounds = 0;
    }
    else
    {
      s.in_bounds = 1;
    }

    if (s.in_bounds == 0)
    {
      return 0;
    }
  }

  if (control->state == 3)
  {
    return 0;
  }

  s.saved_page_number = g_page0_window_bounds->page_number;
  g_page0_window_bounds->page_number = 0;

  s.mapped_button_row = g_map_menu_controls_00590888[control->selection_value].selection_value;

  switch (mode)
  {
  case 0:
    s.sprite_state_col = 0;
    break;
  case 1:
    s.sprite_state_col = 1;
    break;
  case 2:
    s.sprite_state_col = 2;
    break;
  }

  s.button_x = g_map_menu_controls_00590888[s.mapped_button_row].x;
  s.button_y = g_map_menu_controls_00590888[s.mapped_button_row].y;
  s.button_width = g_map_menu_controls_00590888[s.mapped_button_row].width;
  s.button_height = g_map_menu_controls_00590888[s.mapped_button_row].height;
  DrawEncodedImageResampled(g_page0_window_bounds, s.button_x, s.button_y, s.button_width, s.button_height,
                            g_map_button_sprites[s.mapped_button_row][s.sprite_state_col]);

  g_page0_window_bounds->page_number = s.saved_page_number;

  if ((mode == 2) && (control->on_activate != (AdvMenuActivateCallback)0))
  {
    control->on_activate(control);
  }

  return 1;
}

// FUNCTION: SHANDALAR 0x0054e94c
int ActivateMapMenuControl(AdvMenuControl *control)
{
  PlaySoundEffectOnChannel("x:sound\\button2.wav", 0xf, 100, 100, 0);
  g_map_menu_selection = control->selection_value;
  return 0;
}

// FUNCTION: SHANDALAR 0x0054e97e
void SplitMapTooltipTextIntoTwoLines(char *src, char *dst_first, char *dst_second)
{
  struct
  {
    char *space_back;
    char *space_fwd;
    char *mid;
  } s;

  if (src == (char *)0)
  {
    if (dst_first != (char *)0)
    {
      strcpy(dst_first, "");
    }
    if (dst_second != (char *)0)
    {
      strcpy(dst_second, "");
    }
    return;
  }

  if (dst_first == (char *)0)
  {
    if (dst_second != (char *)0)
    {
      strcpy(dst_second, src);
    }
    return;
  }

  if (dst_second == (char *)0)
  {
    if (dst_first != (char *)0)
    {
      strcpy(dst_first, src);
    }
    return;
  }

  strcpy(dst_first, src);
  s.mid = dst_first + (strlen(dst_first) >> 1);

  for (s.space_fwd = s.mid; (*s.space_fwd != '\0' && (*s.space_fwd != ' ')); s.space_fwd++)
  {
  }
  if (*s.space_fwd != ' ')
  {
    s.space_fwd = (char *)0;
  }

  for (s.space_back = s.mid; (*s.space_back != ' ' && (dst_first <= s.space_back)); s.space_back--)
  {
  }
  if (*s.space_back != ' ')
  {
    s.space_back = (char *)0;
  }

  if ((s.space_fwd != (char *)0) &&
      ((s.space_back == (char *)0) || ((int)s.space_fwd - (int)s.mid <= (int)s.mid - (int)s.space_back)))
  {
    *s.space_fwd = '\0';
    strcpy(dst_second, s.space_fwd + 1);
  }
  else if ((s.space_back != (char *)0) &&
           ((s.space_fwd == (char *)0) || ((int)s.space_fwd - (int)s.mid >= (int)s.mid - (int)s.space_back)))
  {
    *s.space_back = '\0';
    strcpy(dst_second, s.space_back + 1);
  }
  else
  {
    strcpy(dst_second, "");
  }
}

// FUNCTION: SHANDALAR 0x00550314
char *__cdecl FormatWorldLocationName(int town_index, int mana_castle_index)
{
  struct
  {
    int first_half_index;
    int location_type;
    int second_half_index;
  } s;

  s.first_half_index = (town_index / 8) % 16;
  if (s.first_half_index < 0)
  {
    s.first_half_index = 0;
  }
  if (s.first_half_index > 0xf)
  {
    s.first_half_index = 0xf;
  }

  s.second_half_index = town_index % 16;
  if (s.second_half_index < 0)
  {
    s.second_half_index = 0;
  }
  if (s.second_half_index > 0xf)
  {
    s.second_half_index = 0xf;
  }

  s.location_type = g_town_slots[town_index].location_type;
  if ((s.location_type == 4) || (s.location_type == 5))
  {
    sprintf(g_world_ui_text_scratch_buffer, gs_cityname_manacastle_0077de00[mana_castle_index]);
  }
  else
  {
    FormatMessageFromStringStripCarriageReturns(g_world_ui_text_scratch_buffer, 0x1000, gs_cityname_format_left_0074c950, gs_citynames_firsthalf_0077e060[s.first_half_index],
                                                gs_citynames_secondhalf_007653e0[s.second_half_index]);
  }

  return g_world_ui_text_scratch_buffer;
}

// FUNCTION: SHANDALAR 0x0054eb6a
void ShowWorldMapScreen(int mode)
{
  struct
  {
    int saved_page_number;
    char line1[100];
    int tooltip_line_height;
    char line2[100];

    int text_width2;              // ebp - 0xbc
    int avatar_indices[5];        // ebp - 0xb8
    int world_magic_avatar_width; // ebp - 0xa4

    int world_magic_line_height; // ebp - 0xa0
    int text_width;              // ebp - 0x9c
    int wizard_count;            // ebp - 0x98
    int button_text_colors[4];   // ebp - 0x94
    int menu_context;            // ebp - 0x84

    int map_overlay_colors[8]; // ebp - 0x80
    int wizard_counts[7];      // ebp - 0x60

    unsigned int mana_mask;      // ebp - 0x44
    int original_mode;           // ebp - 0x40
    int world_magic_slot_result; // ebp - 0x3c
    int i;                       // ebp - 0x38
    int tile_y;                  // ebp - 0x34
    int dir;                     // ebp - 0x30
    int tile_x;                  // ebp - 0x2c
    int any_tooltip_drawn;       // ebp - 0x28
    int screen_y_unscaled_debug; // ebp - 0x24
    int screen_x_unscaled_debug; // ebp - 0x20
    unsigned int tile_class;     // ebp - 0x1c
    int text_color;              // ebp - 0x18
    int nearest_town_for_lair;   // ebp - 0x14
    int screen_y;                // ebp - 0x10
    int town_index;              // ebp - 0xc
    int blink_on;                // ebp - 0x8
    int screen_x;                // ebp - 0x4
  } s;

  s.blink_on = 1;
  s.any_tooltip_drawn = 0;
  s.original_mode = mode;

  s.map_overlay_colors[0] = 0xd2;
  s.map_overlay_colors[1] = 0;
  s.map_overlay_colors[2] = 0x40;
  s.map_overlay_colors[3] = 0xd0;
  s.map_overlay_colors[4] = 0xbe;
  s.map_overlay_colors[5] = 3;
  s.screen_x_unscaled_debug = -1;
  s.screen_y_unscaled_debug = -1;

  EnsureAdvfac64Loaded(0);
  LoadPcxIntoPageNoPalette("advfac64.pic");

  if (mode == 4)
  {
    mode = 0;
  }

  s.button_text_colors[0] = 0x48;
  s.button_text_colors[1] = 0x3f;
  s.button_text_colors[2] = 0x40;

  LoadPcxIntoPage(1, "mapbttns.pic");
  BeginSpriteEncodeSession();

  if (g_map_button_labels_loaded == 0)
  {
    g_map_button_labels = (char **)LoadIniEscapedStringTable(g_advbuttons_ini_file, "mapButtons", g_ini_string_scratch);
    g_map_button_labels_loaded = 1;
  }

  g_page1_window_bounds->font_slot = 7;

  for (s.i = 0; s.i < 5; s.i++)
  {
    for (s.dir = 0; s.dir < 3; s.dir++)
    {
      SetFontStyleSize(7, 9 + ((unsigned int)(s.dir - 2) < 1 ? -1 : 0));
      DrawFormattedTextShadowedCentered(g_page1_window_bounds, s.button_text_colors[s.dir], s.dir * 0x5e + 0x30, s.i * 0x1c + 0xe,
                                        "%s", g_map_button_labels[s.i]);
      g_map_button_sprites[s.i][s.dir] = EncodeSpriteFromPage(1, s.dir * 0x5e + 1, s.i * 0x1c + 1, 0x5d, 0x1b);
    }
  }

  g_map_amulet_panel_sprite = EncodeSpriteFromPage(1, 1, 0x8d, 0x2b, 0xe9);
  g_map_summary_panel_sprite = EncodeSpriteFromPage(1, 0x2d, 0x8d, 0xd1, 0x1c);
  g_adv_scrollbar_track_sprite = EncodeSpriteFromPage(1, 0xe8, 0xaa, 0x13, 0x117);
  g_adv_scrollbar_thumb_sprite = EncodeSpriteFromPage(1, 0xfc, 0xaa, 0xf, 0x46);

  FinalizeSpriteEncodeSession();

  /* One-time scaling of the x/y/width/height fields */
  if (g_map_menu_controls_00590888[0].base_x == g_map_menu_controls_00590888[0].x)
  {
    for (s.dir = 0; s.dir < 5; s.dir++)
    {
      g_map_menu_controls_00590888[s.dir].x = ScaleUiCoordinate(g_map_menu_controls_00590888[s.dir].x);
      g_map_menu_controls_00590888[s.dir].y = ScaleUiCoordinate(g_map_menu_controls_00590888[s.dir].y);
      g_map_menu_controls_00590888[s.dir].width = ScaleUiCoordinate(g_map_menu_controls_00590888[s.dir].width);
      g_map_menu_controls_00590888[s.dir].height = ScaleUiCoordinate(g_map_menu_controls_00590888[s.dir].height);
    }
  }

retry:

  g_page0_window_bounds->font_slot = 1;
  g_page0_window_bounds->page_number = 1;

  ClearGraphicsPageWithPaletteColor(g_page0_window_bounds->page_number, 0);
  LoadPcxResource(1, 0, global_screen_height - 0x1e0, "mapback.pic", &g_palette_data_words);

  if (global_screen_width != 0x1e0)
  {
    StretchBlitGraphicsRect(g_page1_window_bounds, 0, global_screen_height - 0x1e0, 0x280, 0x1e0, g_page0_window_bounds, 0, 0, global_screen_width, global_screen_height);
  }

  s.menu_context = BeginMenuContext();
  ResetMenuContext(s.menu_context);

  /* Position the two inactive mode-buttons into the two slots (x=48 and x=148). */
  switch (mode)
  {
  case 0:
    g_map_menu_controls_00590888[1].x = ScaleUiCoordinate(g_map_button_slot_left_x);
    g_map_menu_controls_00590888[1].y = ScaleUiCoordinate(g_map_button_slot_left_y);
    AddMenuControlsToContext(&g_map_menu_controls_00590888[1], 1, s.menu_context);

    g_map_menu_controls_00590888[2].x = ScaleUiCoordinate(g_map_button_slot_right_x);
    g_map_menu_controls_00590888[2].y = ScaleUiCoordinate(g_map_button_slot_right_y);
    AddMenuControlsToContext(&g_map_menu_controls_00590888[2], 1, s.menu_context);
    break;

  case 1:
    g_map_menu_controls_00590888[0].x = ScaleUiCoordinate(g_map_button_slot_left_x);
    g_map_menu_controls_00590888[0].y = ScaleUiCoordinate(g_map_button_slot_left_y);
    AddMenuControlsToContext(&g_map_menu_controls_00590888[0], 1, s.menu_context);

    g_map_menu_controls_00590888[1].x = ScaleUiCoordinate(g_map_button_slot_right_x);
    g_map_menu_controls_00590888[1].y = ScaleUiCoordinate(g_map_button_slot_right_y);
    AddMenuControlsToContext(&g_map_menu_controls_00590888[1], 1, s.menu_context);
    break;

  case 2:
    g_map_menu_controls_00590888[0].x = ScaleUiCoordinate(g_map_button_slot_left_x);
    g_map_menu_controls_00590888[0].y = ScaleUiCoordinate(g_map_button_slot_left_y);
    AddMenuControlsToContext(&g_map_menu_controls_00590888[0], 1, s.menu_context);

    g_map_menu_controls_00590888[2].x = ScaleUiCoordinate(g_map_button_slot_right_x);
    g_map_menu_controls_00590888[2].y = ScaleUiCoordinate(g_map_button_slot_right_y);
    AddMenuControlsToContext(&g_map_menu_controls_00590888[2], 1, s.menu_context);
    break;
  }

  AddMenuControlsToContext(&g_map_menu_controls_00590888[3], 1, s.menu_context);
  AddMenuControlsToContext(&g_map_menu_controls_00590888[4], 1, s.menu_context);

  s.map_overlay_colors[6] = 0;

  /* Draw terrain tiles and roads */
  for (s.tile_x = 0; s.tile_x < 0x40; s.tile_x++)
  {
    for (s.tile_y = 0; s.tile_y < 0x40; s.tile_y++)
    {
      s.map_overlay_colors[7] = GetWorldMapPixelFlags(s.tile_x, s.tile_y);
      s.tile_class = s.map_overlay_colors[7] & 0xf;

      if (((s.map_overlay_colors[7] & 0x80) == 0) && (g_reveal_all_world_info == 0))
      {
        continue;
      }

      ConvertWorldTileToMapScreenCoords(s.tile_x, s.tile_y, &s.screen_x, &s.screen_y);
      if (s.screen_x > global_screen_width - 8 || s.screen_x < 8)
      {
        continue;
      }

      if (s.screen_y > global_screen_height - 8 || s.screen_y < 8)
      {
        continue;
      }

      /* Draw tile sprite (uses a deterministic variant index) */
      s.screen_y += 0x40;
      if ((6 < s.screen_x) && (6 < s.screen_y))
      {
        DrawEncodedImageUiScaled(g_page0_window_bounds, s.screen_x - 6, s.screen_y - 6,
                                 g_ttsprite_grid_sprite_entries[((g_map_tile_variant_primes[(s.tile_y + s.tile_x) % 6] * g_map_tile_variant_primes[(s.tile_y * s.tile_x) % 6]) % 3) * 0x10 + (int)s.tile_class], 0xe, 0xe);
      }
      s.screen_y -= 0x40;

      s.screen_x = (s.screen_x * global_screen_width) / 0x280;
      s.screen_y = (s.screen_y * global_screen_height) / 0x1e0;
      s.screen_y += ScaleUiCoordinate(0x40);

      for (s.dir = 1; s.dir <= 8; s.dir++)
      {
        if (WorldRoadTileHasDirection(s.tile_x, s.tile_y, (char)((s.dir & 7) + 1)) != 0)
        {
          DrawGraphicsLine(g_page0_window_bounds, s.screen_x, s.screen_y, s.screen_x + ScaleUiCoordinate(g_neighbor_dx[s.dir] * 7),
                           s.screen_y + ScaleUiCoordinate(g_neighbor_dy[s.dir] * 7), 0xd2);
          DrawGraphicsLine(g_page0_window_bounds, s.screen_x - 1, s.screen_y,
                           (s.screen_x - 1) + ScaleUiCoordinate(g_neighbor_dx[s.dir] * 7),
                           s.screen_y + ScaleUiCoordinate(g_neighbor_dy[s.dir] * 7), 0xd2);
        }
      }
    }
  }

  for (s.dir = 0; s.dir < 7; s.dir++)
  {
    s.wizard_counts[s.dir] = 0;
  }

  s.nearest_town_for_lair = -1;
  if (g_lair_or_monster_slots[7].entry_type != SHANDALAR_ENTRY_NONE)
  {
    s.nearest_town_for_lair = FindNearestTownIndex(g_lair_or_monster_slots[7].world_x / 32,
                                                   g_lair_or_monster_slots[7].world_y / 32);
  }

  /* Draw town markers + optional tooltips */
  for (s.tile_x = 0; s.tile_x < 0x40; s.tile_x++)
  {
    for (s.tile_y = 0; s.tile_y < 0x40; s.tile_y++)
    {
      s.map_overlay_colors[7] = GetWorldMapPixelFlags(s.tile_x, s.tile_y);
      s.tile_class = s.map_overlay_colors[7] & 0xf;
      ConvertWorldTileToMapScreenCoords(s.tile_x, s.tile_y, &s.screen_x, &s.screen_y);

      s.screen_x = (s.screen_x * global_screen_width) / 0x280;
      s.screen_y = (s.screen_y * global_screen_height) / 0x1e0;
      s.screen_y = s.screen_y + ScaleUiCoordinate(0x40);

      if (s.screen_x > global_screen_width - 8 || s.screen_x < 0)
      {
        continue;
      }

      if (s.screen_y > global_screen_height - 8 || s.screen_y < 0)
      {
        continue;
      }

      if ((s.map_overlay_colors[7] & 0x10) != 0)
      {
        s.town_index = FindTownAtWorldCoordinates(s.tile_x, s.tile_y);

        if (g_town_slots[s.town_index].status_and_ruling_wizard & 0xff00)
        {
          s.mana_mask = g_town_slots[s.town_index].status_and_ruling_wizard >> 8;
          s.wizard_counts[s.mana_mask] = s.wizard_counts[s.mana_mask] + 1;
        }

        if ((mode == 0) && ((g_town_slots[s.town_index].status_and_ruling_wizard & 1) != 0) && (1 < g_town_slots[s.town_index].location_type))
        {
          s.map_overlay_colors[6] = s.map_overlay_colors[6] + 1;
        }

        if (((s.map_overlay_colors[7] & 0x80) == 0) && (g_reveal_all_world_info == 0))
        {
          continue;
        }

        if (g_town_slots[s.town_index].location_type == 1)
        {
          s.saved_page_number = 0;
        }
        else if ((g_town_slots[s.town_index].location_type == 4) || (g_town_slots[s.town_index].location_type == 5))
        {
          s.saved_page_number = 2;
        }
        else
        {
          s.saved_page_number = 1;
        }

        DrawEncodedImageResampled(g_page0_window_bounds, s.screen_x - ScaleUiCoordinate(7), s.screen_y - ScaleUiCoordinate(0x12), ScaleUiCoordinate(0xd), ScaleUiCoordinate(0x14),
                                  g_ttsprite_aux_sprite_entries[s.saved_page_number]);

        if (s.town_index != -1 && 1 < g_town_slots[s.town_index].location_type)
        {
          s.text_color = (g_town_slots[s.town_index].status_and_ruling_wizard & 1) ? 0xff : 0xe3;
          if (s.nearest_town_for_lair == s.town_index)
          {
            s.text_color = 0xbe;
          }
          if (g_town_slots[s.town_index].status_and_ruling_wizard & 0xff00)
          {
            s.mana_mask = g_town_slots[s.town_index].status_and_ruling_wizard >> 8;
            s.text_color = s.map_overlay_colors[s.mana_mask];
          }

          if (mode == 1)
          {
            s.wizard_count = 0;
            s.world_magic_avatar_width = (int)g_world_magic_avatar_sprites[0]->width;
            s.world_magic_line_height = GetFontStyleSize(g_page0_window_bounds->font_slot);

            s.mana_mask = GetWorldTileMagicMask(GetWorldTileType(g_town_slots[s.town_index].world_x, g_town_slots[s.town_index].world_y));
            for (s.dir = 1; s.dir < 6; s.dir++)
            {
              if ((s.mana_mask & (1U << (s.dir & 0xff))) != 0)
              {
                s.wizard_count++;
              }
            }

            s.button_text_colors[3] = (s.screen_x + ((s.wizard_count - 1) * s.world_magic_avatar_width) / 2) -
                                      s.world_magic_avatar_width / 2;
            for (s.dir = 1; s.dir < 6; s.dir++)
            {
              s.avatar_indices[0] = 2;
              s.avatar_indices[1] = 1;
              s.avatar_indices[2] = 4;
              s.avatar_indices[3] = 3;
              s.avatar_indices[4] = 0;

              if ((s.mana_mask & (1U << (s.dir & 0xff))) != 0)
              {
                DrawEncodedImageUnscaled(g_page0_window_bounds, s.button_text_colors[3], s.screen_y + 0xc, g_world_magic_avatar_sprites[s.avatar_indices[s.dir - 1]]);
                s.button_text_colors[3] -= s.world_magic_avatar_width;
              }
            }

            if (g_town_slots[s.town_index].trade_color_and_type & 0xff)
            {
              s.mana_mask = single_color_test_bit_to_color_t(g_town_slots[s.town_index].trade_color_and_type & 0xff);
              strcpy(g_ui_message_buffer, gs_colorcards_0077c5e0[s.mana_mask]);
            }
            else
            {
              strcpy(g_ui_message_buffer, GetPluralCardClassNameFromColorMask(1U << ((unsigned char)((unsigned int)g_town_slots[s.town_index].trade_color_and_type >> 8) - 1U)));
            }

            s.text_width = MeasureMultilineTextWidth(g_page0_window_bounds, g_ui_message_buffer);

            ApplyPortraitTintMap(g_page0_window_bounds, (s.screen_x - 2) - s.text_width / 2, s.screen_y - 2, s.text_width + 4, GetFontLineHeight(g_page0_window_bounds->font_slot) + 2, 0x3f3f3f,
                                 (s.any_tooltip_drawn == 0));
            s.any_tooltip_drawn = 1;
            DrawCenteredTextLineWithShadow(g_ui_message_buffer, s.screen_x, s.screen_y, s.text_color);

            for (s.dir = 0; s.dir < 0xc; s.dir++)
            {
              if ((s.town_index != 0) && (g_world_magic_slot_timers[s.dir].town_index == s.town_index))
              {
                s.world_magic_slot_result = FindWorldMagicCardIndex(s.dir);
                strcpy(g_ui_message_buffer, gs_worldmagic_names_00780660[s.dir]);
                s.text_width = MeasureMultilineTextWidth(g_page0_window_bounds, g_ui_message_buffer);

                ApplyPortraitTintMap(g_page0_window_bounds, (s.screen_x - 2) - s.text_width / 2,
                                     s.screen_y + s.world_magic_line_height,
                                     s.text_width + 4, GetFontLineHeight(g_page0_window_bounds->font_slot) + 2, 0x3f3f3f, (s.any_tooltip_drawn == 0));
                DrawCenteredTextLineWithShadow(g_ui_message_buffer, s.screen_x,
                                               s.screen_y + s.world_magic_line_height,
                                               s.text_color);
              }
            }
          }

          if ((mode == 0) || ((mode == 1) && (g_town_slots[s.town_index].location_type == 4)))
          {
            s.tooltip_line_height = (int)GetFontStyleSize(g_page0_window_bounds->font_slot);

            if ((g_town_slots[s.town_index].location_type == 4) || (g_town_slots[s.town_index].location_type == 5))
            {
              strcpy(g_ui_message_buffer, FormatWorldLocationName(s.town_index, single_color_test_bit_to_color_t((int)GetWorldTileMagicMask(GetWorldTileType(s.tile_x, s.tile_y)))));
            }
            else
            {
              strcpy(g_ui_message_buffer, FormatWorldLocationName(s.town_index, 0));
            }

            SplitMapTooltipTextIntoTwoLines(g_ui_message_buffer, s.line1, s.line2);

            strcpy(g_ui_message_buffer, s.line1);
            s.text_width2 = MeasureMultilineTextWidth(g_page0_window_bounds, g_ui_message_buffer);

            ApplyPortraitTintMap(g_page0_window_bounds, (s.screen_x - 2) - s.text_width2 / 2, s.screen_y - 2, s.text_width2 + 4, GetFontLineHeight(g_page0_window_bounds->font_slot) + 2, 0x4f4f4f,
                                 (s.any_tooltip_drawn == 0));
            s.any_tooltip_drawn = 1;
            DrawCenteredTextLineWithShadow(g_ui_message_buffer, s.screen_x, s.screen_y, s.text_color);

            strcpy(g_ui_message_buffer, s.line2);
            s.text_width2 = MeasureMultilineTextWidth(g_page0_window_bounds, g_ui_message_buffer);

            ApplyPortraitTintMap(g_page0_window_bounds, (s.screen_x - 2) - s.text_width2 / 2, s.screen_y + s.tooltip_line_height, s.text_width2 + 4, GetFontLineHeight(g_page0_window_bounds->font_slot), 0x4f4f4f, 0);
            DrawCenteredTextLineWithShadow(g_ui_message_buffer, s.screen_x, s.screen_y + s.tooltip_line_height, s.text_color);
          }
        }
      }

      if ((g_reveal_all_world_info != 0) && ((s.map_overlay_colors[7] & 0x40) != 0))
      {
        FillGraphicsRect(g_page0_window_bounds, s.screen_x + 1, s.screen_y + 1, 2, 2, 0xf6);
      }
    }
  }

  DrawEncodedImageResampled(g_page0_window_bounds, ScaleUiCoordinate(0xdc), ScaleUiCoordinate(0x36),
                            ScaleUiCoordinate((int)g_map_summary_panel_sprite->width), ScaleUiCoordinate((int)g_map_summary_panel_sprite->height), g_map_summary_panel_sprite);

  if (s.original_mode != 4)
  {
    DrawEncodedImageResampled(g_page0_window_bounds, ScaleUiCoordinate(0), ScaleUiCoordinate(0x85),
                              ScaleUiCoordinate((int)g_map_amulet_panel_sprite->width), ScaleUiCoordinate((int)g_map_amulet_panel_sprite->height), g_map_amulet_panel_sprite);
    for (s.dir = 0; s.dir < 5; s.dir++)
    {
      DrawTextAt(g_page0_window_bounds, 0xfe, 0x18, s.dir * 0x1a + 0xc6, "%d", g_amulet_inventory[s.dir]);
    }
  }

  g_page0_window_bounds->page_number = 0;
  BlitGraphicsRect(g_page1_window_bounds, 0, 0, global_screen_width, global_screen_height, g_page0_window_bounds, 0, 0);

  if (s.original_mode == 4)
  {
    EndMenuContext();
    return;
  }

  RenderCurrentMenuContextControls();

  ConvertWorldTileToMapScreenCoords(g_world_player_tile_x, g_world_player_tile_y, &s.screen_x, &s.screen_y);
  s.screen_x = (s.screen_x * global_screen_width) / 0x280;
  s.screen_y = ScaleUiCoordinate(0x40) + (s.screen_y * global_screen_height) / 0x1e0;

  g_map_menu_selection = -1;
  s.blink_on = 1;
  ConsumeUiTickCount();
  s.screen_x_unscaled_debug = s.screen_y_unscaled_debug = -1;

  do
  {
    if ((GetUiTickCount() % 0x14 < 10) && (s.blink_on != 0))
    {
      FillGraphicsRect(g_page0_window_bounds, s.screen_x - 1, s.screen_y - 1, 4, 4, 0xff);
      s.blink_on = 0;
    }
    else if ((10 <= (GetUiTickCount() % 0x14)) && (s.blink_on == 0))
    {
      FillGraphicsRect(g_page0_window_bounds, s.screen_x - 1, s.screen_y - 1, 4, 4, 0);
      s.blink_on = 1;
    }

    UpdateMouseSnapshot();

    if (g_mouse_button_down_mask != 0)
    {
      UpdateMenuControlSelection(g_mouse_x_snapshot, g_mouse_y_snapshot, g_mouse_button_down_mask);
      if ((g_mouse_button_down_mask != 0) && (g_reveal_all_world_info != 0) && (g_map_menu_selection < 0))
      {
        BlitGraphicsRect(g_page1_window_bounds, s.screen_x - 1, s.screen_y - 1, 4, 4, g_page0_window_bounds, s.screen_x - 1, s.screen_y - 1);
        s.screen_x = s.screen_x_unscaled_debug = g_mouse_x_snapshot;
        s.screen_y = s.screen_y_unscaled_debug = g_mouse_y_snapshot;

        s.screen_x_unscaled_debug = (s.screen_x_unscaled_debug * 0x280) / global_screen_width;
        s.screen_y_unscaled_debug = ((s.screen_y_unscaled_debug - ScaleUiCoordinate(0x40)) * 0x1e0) / global_screen_height;
        ConvertMapScreenToWorldTileCoords(s.screen_x_unscaled_debug, s.screen_y_unscaled_debug, &g_world_player_tile_x, &g_world_player_tile_y);
        g_world_player_x = g_world_player_tile_x * 0x20 + 0x10;
        g_world_player_y = g_world_player_tile_y * 0x20 + 0x10;

        ConvertWorldTileToMapScreenCoords(g_world_player_tile_x, g_world_player_tile_y, &s.screen_x, &s.screen_y);
        s.screen_x = (s.screen_x * global_screen_width) / 0x280;
        s.screen_y = ScaleUiCoordinate(0x40) + (s.screen_y * global_screen_height) / 0x1e0;

        g_world_scroll_cache_ready = 0;
      }
    }
    else
    {
      UpdateMenuControlSelection(g_mouse_x_snapshot, g_mouse_y_snapshot, g_mouse_button_down_mask);
    }
  } while (g_map_menu_selection == -1);

  if (g_map_menu_selection == 3)
  {
    ClearInputAndWaitForMouseRelease();
    ShowCityInfoScreen(1);
    mode = 0;
    EndMenuContext();
    goto retry;
  }

  if (g_map_menu_selection != 4)
  {
    mode = g_map_menu_selection;
    ClearInputAndWaitForMouseRelease();
    EndMenuContext();
    goto retry;
  }
  else
  {
    EndMenuContext();
    FreeSpriteBlob(g_map_button_sprites[0][0]);
    ClearInputAndWaitForMouseRelease();
  }
}
