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

extern int g_mouse_x;
extern int g_mouse_y;
extern int g_mouse_button_down_mask;
extern int g_mouse_x_snapshot;
extern int g_mouse_y_snapshot;

extern FILE *g_advbuttons_ini_file;
extern char g_ini_string_scratch[0x28];
extern char g_ui_message_buffer[0x1000];

extern FacemakerWindowBounds *PTR_DAT_005832b4;
extern FacemakerWindowBounds *PTR_DAT_005832dc;

extern RpBitsPalettePacket g_palette_data_words;

/* World state */
extern int g_world_player_tile_x;
extern int g_world_player_tile_y;
extern int g_world_scroll_cache_ready;

extern int g_neighbor_dx[9];
extern int g_neighbor_dy[9];

extern unsigned char Scards[0xc0];

/* Debug toggle used by multiple screens */
extern int DAT_007894f4;

/* Sprites loaded by startup code (owned by shandalar.c) */
extern EncodedImage *g_ttsprite_grid_sprite_entries[0x40];
extern EncodedImage *g_ttsprite_aux_sprite_entries[0x18];
extern EncodedImage *g_world_magic_avatar_sprites[5];

extern FontSlot g_font_slots[0x10];
extern char g_world_ui_text_scratch_buffer[0x1000];

/* Engine / UI helpers */
int *LoadIniEscapedStringTable(FILE *ini_file, char *section_name, int unk1, int unk2);
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
void DrawTextAt(FacemakerWindowBounds *dst, int text_id, int x, int y, char *text);
int MeasureMultilineTextWidth(FacemakerWindowBounds *dst, char *text);
int GetFontLineHeight(int font_slot);
unsigned char GetFontStyleSize(int font_slot);
void ApplyPortraitTintMap(FacemakerWindowBounds *dst, int x, int y, int w, int h, unsigned int tint, int maybe_shadow);

void DrawEncodedImageResampled(FacemakerWindowBounds *dst, int x, int y, int width, int height, EncodedImage *encoded_image);
void DrawEncodedImageUnscaled(FacemakerWindowBounds *dst, int x, int y, EncodedImage *encoded_image);
void FillGraphicsRect(FacemakerWindowBounds *window_bounds, int x, int y, int width, int height, unsigned int color_index);
void DrawGraphicsLine(FacemakerWindowBounds *window_bounds, int x1, int y1, int x2, int y2, int color_index);

int BeginMenuContext(void);
int ResetMenuContext(int context_index);
int AddMenuControlsToContext(AdvMenuControl *controls, int control_count, int context_index);
int EndMenuContext(void);
int FUN_005001e3(void);
int UpdateMenuControlSelection(int mouse_x, int mouse_y, int allow_activate_on_click);

void UpdateMouseSnapshot(void);
int ConsumeUiTickCount(void);
int GetUiTickCount(void);
void PopNormalizedQueuedKeyInput(void);

void EnsureAdvfac64Loaded(int state);
void PlaySoundEffectOnChannel(char *sound_path, int channel, int volume, int pitch_percent, int pan_percent);

unsigned int FUN_0043146b(int x, int y);
unsigned int FUN_004314ca(int x, int y);
unsigned int WorldRoadTileHasDirection(int tile_x, int tile_y, char direction_index);
void FUN_00550164(int tile_x, int tile_y, int *out_x, int *out_y);
void FUN_00550197(int x, int y, int *out_tile_x, int *out_tile_y);
void DrawEncodedImageUiScaled(FacemakerWindowBounds *dst, int x_320, int y_200, EncodedImage *sprite, int width_320, int height_200);
int FUN_0040dffd(int mask);
unsigned int FUN_005611c8(unsigned int tile_mask);
int FUN_004bb458(int world_magic_slot_index);
int FindNearestTownIndex(int world_x, int world_y);
int FUN_004bb040(int world_x, int world_y);
char *FUN_00428783(unsigned int color_bitmask);

void DrawCenteredTextLineClamped(char *text, int center_x, int y, int color_index);

void ShowCityInfoScreen(int param_1);

void FUN_00564e70(char *dst, int max_length, char *format, ...);

int __cdecl RenderMapMenuControl(AdvMenuControl *control, int mode);
int __cdecl ActivateMapMenuControl(AdvMenuControl *control);

// GLOBAL: SHANDALAR 0x00650f20
int DAT_00650f20;
// GLOBAL: SHANDALAR 0x006527a4
char **DAT_006527a4;
// GLOBAL: SHANDALAR 0x00650f1c
EncodedImage *DAT_00650f1c;
// GLOBAL: SHANDALAR 0x006506e0
EncodedImage *DAT_006506e0;
// GLOBAL: SHANDALAR 0x0073ea94
EncodedImage *DAT_0073ea94;
// GLOBAL: SHANDALAR 0x00746b04
EncodedImage *DAT_00746b04;
// GLOBAL: SHANDALAR 0x006506e8
int DAT_006506e8;

// GLOBAL: SHANDALAR 0x0073eab0
EncodedImage *DAT_0073eab0[15];

// GLOBAL: SHANDALAR 0x00590a30
int DAT_00590a30[6] = {2, 3, 5, 7, 11, 13};

// GLOBAL: SHANDALAR 0x00590850
int DAT_00590850 = 48;
// GLOBAL: SHANDALAR 0x00590854
int DAT_00590854 = 5;
// GLOBAL: SHANDALAR 0x00590858
int DAT_00590858 = 148;
// GLOBAL: SHANDALAR 0x0059085c
int DAT_0059085c = 5;

// GLOBAL: SHANDALAR 0x00590b08
char DAT_00590b08[] = "Mm";
// GLOBAL: SHANDALAR 0x00590b0c
char DAT_00590b0c[] = "Mm";
// GLOBAL: SHANDALAR 0x00590b10
char DAT_00590b10[] = "Pp";
// GLOBAL: SHANDALAR 0x00590b14
char DAT_00590b14[] = "Pp";
// GLOBAL: SHANDALAR 0x00590b18
char DAT_00590b18[] = "Ii";
// GLOBAL: SHANDALAR 0x00590b1c
char DAT_00590b1c[] = "Ii";
// GLOBAL: SHANDALAR 0x00590b20
char DAT_00590b20[] = "Cc";
// GLOBAL: SHANDALAR 0x00590b24
char DAT_00590b24[] = "Cc";
// GLOBAL: SHANDALAR 0x00590b28
char DAT_00590b28[] = " \rDd\x1b";
// GLOBAL: SHANDALAR 0x00590b30
char DAT_00590b30[] = " \rDd\x1b";

// GLOBAL: SHANDALAR 0x00590b6c
char s_x_sound_button2_wav_00590b6c[] = "x:sound\\button2.wav";

// GLOBAL: SHANDALAR 0x00590b8c
char s_advfac64_pic_00590b8c[] = "advfac64.pic";
// GLOBAL: SHANDALAR 0x00590b9c
char s_mapbttns_pic_00590b9c[] = "mapbttns.pic";
// GLOBAL: SHANDALAR 0x00590bac
char s_mapButtons_00590bac[] = "mapButtons";
// GLOBAL: SHANDALAR 0x00590bbc
char s_mapback_pic_00590bbc[] = "mapback.pic";

// GLOBAL: SHANDALAR 0x00590b80
char DAT_00590b80[] = "";
// GLOBAL: SHANDALAR 0x00590b84
char DAT_00590b84[] = "";
// GLOBAL: SHANDALAR 0x00590b88
char DAT_00590b88[] = "";

// GLOBAL: SHANDALAR 0x00590bc8
char DAT_00590bc8[] = "%d";

// GLOBAL: SHANDALAR 0x00581af8
char DAT_00581af8[] = "";

// GLOBAL: SHANDALAR 0x00590888
AdvMenuControl g_map_menu_controls_00590888[5] = {
    {48, 5, 94, 28, 48, 5, 94, 28, 1, RenderMapMenuControl, ActivateMapMenuControl, 0, 0, DAT_00590b08, DAT_00590b0c, 0, 0, {0, 0, 0, 0}},
    {48, 5, 94, 28, 48, 5, 94, 28, 1, RenderMapMenuControl, ActivateMapMenuControl, 2, 0, DAT_00590b10, DAT_00590b14, 0, 0, {0, 0, 0, 0}},
    {48, 5, 94, 28, 48, 5, 94, 28, 1, RenderMapMenuControl, ActivateMapMenuControl, 1, 0, DAT_00590b18, DAT_00590b1c, 0, 0, {0, 0, 0, 0}},
    {398, 5, 94, 28, 398, 5, 94, 28, 1, RenderMapMenuControl, ActivateMapMenuControl, 3, 0, DAT_00590b20, DAT_00590b24, 0, 0, {0, 0, 0, 0}},
    {499, 5, 94, 28, 499, 5, 94, 28, 1, RenderMapMenuControl, ActivateMapMenuControl, 4, 0, DAT_00590b28, DAT_00590b30, 0, 0, {0, 0, 0, 0}},
};

// FUNCTION: SHANDALAR 0x00550164
void FUN_00550164(int tile_x, int tile_y, int *out_x, int *out_y)
{
  *out_x = (tile_y + tile_x) * 6 - 0x40;
  *out_y = (tile_y - tile_x) * 6 + 200;
}

// FUNCTION: SHANDALAR 0x00550197
void FUN_00550197(int x, int y, int *out_tile_x, int *out_tile_y)
{
  *out_tile_x = ((x + 0x40) - (y - 200)) / 0xc;
  *out_tile_y = *out_tile_x + (y - 200) / 6;
}

// FUNCTION: SHANDALAR 0x00428783
char *FUN_00428783(unsigned int color_bitmask)
{
  char **p;

  p = (char **)(gs_worldmagic_title_0077e1d0 + 0x20);
  switch (color_bitmask)
  {
  case 1:
    return p[0];
  case 2:
    return p[1];
  case 4:
    return p[2];
  case 8:
    return p[3];
  case 0x10:
    return p[4];
  case 0x20:
    return p[5];
  case 0x40:
    return p[6];
  case 0x42:
    return p[7];
  case 0x80:
    return p[8];
  default:
    return DAT_00581af8;
  }
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
int __cdecl RenderMapMenuControl(AdvMenuControl *control, int mode)
{
  int in_bounds;
  int saved_page_number;
  int mapped_button_row;
  int sprite_state_col;

  if (g_menu_render_guard == 0)
  {
    if ((g_mouse_x < control->x) || ((control->x + control->width) < g_mouse_x))
    {
      in_bounds = 0;
    }
    else if ((g_mouse_y < control->y) || ((control->y + control->height) < g_mouse_y))
    {
      in_bounds = 0;
    }
    else
    {
      in_bounds = 1;
    }

    if (in_bounds == 0)
    {
      return 0;
    }
  }

  if (control->state == 3)
  {
    return 0;
  }

  saved_page_number = PTR_DAT_005832b4->page_number;
  PTR_DAT_005832b4->page_number = 0;

  /* Map selection_value to the row index used by the 5x3 sprite bank. */
  mapped_button_row = g_map_menu_controls_00590888[control->selection_value].selection_value;

  if (mode == 0)
  {
    sprite_state_col = 0;
  }
  else if (mode == 1)
  {
    sprite_state_col = 1;
  }
  else
  {
    sprite_state_col = 2;
  }

  DrawEncodedImageResampled(PTR_DAT_005832b4, g_map_menu_controls_00590888[mapped_button_row].x, g_map_menu_controls_00590888[mapped_button_row].y,
                            g_map_menu_controls_00590888[mapped_button_row].width, g_map_menu_controls_00590888[mapped_button_row].height,
                            DAT_0073eab0[sprite_state_col + mapped_button_row * 3]);

  PTR_DAT_005832b4->page_number = saved_page_number;

  if ((mode == 2) && (control->on_activate != (AdvMenuActivateCallback)0))
  {
    control->on_activate(control);
  }

  return 1;
}

// FUNCTION: SHANDALAR 0x0054e94c
int __cdecl ActivateMapMenuControl(AdvMenuControl *control)
{
  PlaySoundEffectOnChannel(s_x_sound_button2_wav_00590b6c, 0xf, 100, 100, 0);
  DAT_006506e8 = control->selection_value;
  return 0;
}

// FUNCTION: SHANDALAR 0x0054e97e
void __cdecl FUN_0054e97e(char *src, char *dst_first, char *dst_second)
{
  size_t len;
  char *mid;
  char *space_fwd;
  char *space_back;

  if (src == (char *)0)
  {
    if (dst_first != (char *)0)
    {
      strcpy(dst_first, DAT_00590b80);
    }
    if (dst_second != (char *)0)
    {
      strcpy(dst_second, DAT_00590b84);
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
  len = strlen(dst_first);
  mid = dst_first + (len >> 1);

  for (space_fwd = mid; (*space_fwd != '\0' && (*space_fwd != ' ')); space_fwd++)
  {
  }
  if (*space_fwd != ' ')
  {
    space_fwd = (char *)0;
  }

  for (space_back = mid; (*space_back != ' ' && (dst_first <= space_back)); space_back--)
  {
  }
  if (*space_back != ' ')
  {
    space_back = (char *)0;
  }

  if ((space_fwd == (char *)0) ||
      ((space_back != (char *)0) && ((int)mid - (int)space_fwd < (int)space_back - (int)mid)))
  {
    if ((space_back == (char *)0) ||
        ((space_fwd != (char *)0) && ((int)space_back - (int)mid < (int)mid - (int)space_fwd)))
    {
      strcpy(dst_second, DAT_00590b88);
    }
    else
    {
      *space_back = '\0';
      strcpy(dst_second, space_back + 1);
    }
  }
  else
  {
    *space_fwd = '\0';
    strcpy(dst_second, space_fwd + 1);
  }
}

// FUNCTION: SHANDALAR 0x00550314
char * __cdecl FUN_00550314(int town_index, int mana_castle_index)
{
  int first_half_index;
  int second_half_index;
  int sign_adjust;
  int location_type;

  /* Matches the original's signed /8 and clamp behavior. */
  first_half_index = town_index + ((town_index >> 0x1f) & 7U);
  sign_adjust = first_half_index >> 0x1f;
  first_half_index = (((first_half_index >> 3) ^ sign_adjust) - sign_adjust & 0xf ^ sign_adjust) - sign_adjust;
  if (first_half_index < 0)
  {
    first_half_index = 0;
  }
  if (first_half_index > 0xf)
  {
    first_half_index = 0xf;
  }

  second_half_index = town_index;
  sign_adjust = second_half_index >> 0x1f;
  second_half_index = (((second_half_index ^ sign_adjust) - sign_adjust) & 0xf ^ sign_adjust) - sign_adjust;
  if (second_half_index < 0)
  {
    second_half_index = 0;
  }
  if (second_half_index > 0xf)
  {
    second_half_index = 0xf;
  }

  location_type = g_town_slots[town_index].location_type;
  if ((location_type == 4) || (location_type == 5))
  {
    sprintf(g_world_ui_text_scratch_buffer, gs_city_text_cluster_0077d610.cityname_manacastle_0077de00 + mana_castle_index * 100);
  }
  else
  {
    FUN_00564e70(g_world_ui_text_scratch_buffer, 0x1000, gs_cityname_format_left_0074c950, gs_citynames_firsthalf_0077e060[first_half_index],
                 gs_citynames_secondhalf_007653e0[second_half_index]);
  }

  return g_world_ui_text_scratch_buffer;
}

// FUNCTION: SHANDALAR 0x0054eb6a
void ShowWorldMapScreen(int mode)
{
  struct
  {
    int blink_on;
    int any_tooltip_drawn;
    int original_mode;
    int menu_context;

    int button_text_colors[3];
    char line1[100];
    char line2[100];

    int tile_x;
    int tile_y;
    int screen_x_unscaled;
    int screen_y_unscaled;
    int screen_x;
    int screen_y;

    unsigned int tile_flags;
    unsigned int tile_class;

    int nearest_town_for_lair;
    int town_index;
    int i;
    int j;
    int dir;

    int text_width;
    int line_height;
    int tooltip_line_height;
    int text_color;
    int mana_castle_index;
    int saved_page_number;
    int variant;
    int tick;
    int unused_town_counter;

    int wizard_counts[7];
    int wizard_count;
    int avatar_width;
    int avatar_x;

    unsigned int mana_mask;
  } s;

  s.blink_on = 1;
  s.any_tooltip_drawn = 0;
  s.original_mode = mode;

  EnsureAdvfac64Loaded(0);
  LoadPcxIntoPageNoPalette(s_advfac64_pic_00590b8c);

  if (mode == 4)
  {
    mode = 0;
  }

  s.button_text_colors[0] = 0x48;
  s.button_text_colors[1] = 0x3f;
  s.button_text_colors[2] = 0x40;

  LoadPcxIntoPage(1, s_mapbttns_pic_00590b9c);
  BeginSpriteEncodeSession();

  if (DAT_00650f20 == 0)
  {
    DAT_006527a4 = (char **)LoadIniEscapedStringTable(g_advbuttons_ini_file, s_mapButtons_00590bac, (int)g_ini_string_scratch, 0);
    DAT_00650f20 = 1;
  }

  PTR_DAT_005832dc->font_slot = 7;

  for (s.i = 0; s.i < 5; s.i++)
  {
    for (s.j = 0; s.j < 3; s.j++)
    {
      SetFontStyleSize(7, 9 - (s.j == 2));
      DrawFormattedTextShadowedCentered(PTR_DAT_005832dc, s.button_text_colors[s.j], s.j * 0x5e + 0x30, s.i * 0x1c + 0xe,
                                        DAT_006527a4[s.i]);
      DAT_0073eab0[s.i * 3 + s.j] = EncodeSpriteFromPage(1, s.j * 0x5e + 1, s.i * 0x1c + 1, 0x5d, 0x1b);
    }
  }

  DAT_00650f1c = EncodeSpriteFromPage(1, 1, 0x8d, 0x2b, 0xe9);
  DAT_006506e0 = EncodeSpriteFromPage(1, 0x2d, 0x8d, 0xd1, 0x1c);
  DAT_0073ea94 = EncodeSpriteFromPage(1, 0xe8, 0xaa, 0x13, 0x117);
  DAT_00746b04 = EncodeSpriteFromPage(1, 0xfc, 0xaa, 0xf, 0x46);

  FinalizeSpriteEncodeSession();

  /* One-time scaling of the x/y/width/height fields */
  if (g_map_menu_controls_00590888[0].x == g_map_menu_controls_00590888[0].base_x)
  {
    for (s.i = 0; s.i < 5; s.i++)
    {
      g_map_menu_controls_00590888[s.i].x = ScaleUiCoordinate(g_map_menu_controls_00590888[s.i].x);
      g_map_menu_controls_00590888[s.i].y = ScaleUiCoordinate(g_map_menu_controls_00590888[s.i].y);
      g_map_menu_controls_00590888[s.i].width = ScaleUiCoordinate(g_map_menu_controls_00590888[s.i].width);
      g_map_menu_controls_00590888[s.i].height = ScaleUiCoordinate(g_map_menu_controls_00590888[s.i].height);
    }
  }

  while (1)
  {
    s.unused_town_counter = 0;

    PTR_DAT_005832b4->font_slot = 1;
    PTR_DAT_005832b4->page_number = 1;

    ClearGraphicsPageWithPaletteColor(PTR_DAT_005832b4->page_number, 0);
    LoadPcxResource(1, 0, global_screen_height - 0x1e0, s_mapback_pic_00590bbc, &g_palette_data_words);

    if (global_screen_width != 0x1e0)
    {
      StretchBlitGraphicsRect(PTR_DAT_005832dc, 0, global_screen_height - 0x1e0, 0x280, 0x1e0, PTR_DAT_005832b4, 0, 0, global_screen_width, global_screen_height);
    }

    s.menu_context = BeginMenuContext();
    ResetMenuContext(s.menu_context);

    /* Position the two inactive mode-buttons into the two slots (x=48 and x=148). */
    if (mode == 0)
    {
      g_map_menu_controls_00590888[1].x = ScaleUiCoordinate(DAT_00590850);
      g_map_menu_controls_00590888[1].y = ScaleUiCoordinate(DAT_00590854);
      AddMenuControlsToContext(&g_map_menu_controls_00590888[1], 1, s.menu_context);

      g_map_menu_controls_00590888[2].x = ScaleUiCoordinate(DAT_00590858);
      g_map_menu_controls_00590888[2].y = ScaleUiCoordinate(DAT_0059085c);
      AddMenuControlsToContext(&g_map_menu_controls_00590888[2], 1, s.menu_context);
    }
    else if (mode == 1)
    {
      g_map_menu_controls_00590888[0].x = ScaleUiCoordinate(DAT_00590850);
      g_map_menu_controls_00590888[0].y = ScaleUiCoordinate(DAT_00590854);
      AddMenuControlsToContext(&g_map_menu_controls_00590888[0], 1, s.menu_context);

      g_map_menu_controls_00590888[1].x = ScaleUiCoordinate(DAT_00590858);
      g_map_menu_controls_00590888[1].y = ScaleUiCoordinate(DAT_0059085c);
      AddMenuControlsToContext(&g_map_menu_controls_00590888[1], 1, s.menu_context);
    }
    else if (mode == 2)
    {
      g_map_menu_controls_00590888[0].x = ScaleUiCoordinate(DAT_00590850);
      g_map_menu_controls_00590888[0].y = ScaleUiCoordinate(DAT_00590854);
      AddMenuControlsToContext(&g_map_menu_controls_00590888[0], 1, s.menu_context);

      g_map_menu_controls_00590888[2].x = ScaleUiCoordinate(DAT_00590858);
      g_map_menu_controls_00590888[2].y = ScaleUiCoordinate(DAT_0059085c);
      AddMenuControlsToContext(&g_map_menu_controls_00590888[2], 1, s.menu_context);
    }

    AddMenuControlsToContext(&g_map_menu_controls_00590888[3], 1, s.menu_context);
    AddMenuControlsToContext(&g_map_menu_controls_00590888[4], 1, s.menu_context);

    /* Draw terrain tiles and roads */
    for (s.tile_x = 0; s.tile_x < 0x40; s.tile_x++)
    {
      for (s.tile_y = 0; s.tile_y < 0x40; s.tile_y++)
      {
        s.tile_flags = FUN_004314ca(s.tile_x, s.tile_y);
        s.tile_class = s.tile_flags & 0xf;

        if (((s.tile_flags & 0x80) == 0) && (DAT_007894f4 == 0))
        {
          continue;
        }

        FUN_00550164(s.tile_x, s.tile_y, &s.screen_x_unscaled, &s.screen_y_unscaled);
        if ((s.screen_x_unscaled <= global_screen_width - 8) && (7 < s.screen_x_unscaled) && (s.screen_y_unscaled <= global_screen_height - 8) && (7 < s.screen_y_unscaled))
        {
          /* Draw tile sprite (uses a deterministic variant index) */
          if ((6 < s.screen_x_unscaled) && (6 < (s.screen_y_unscaled + 0x40)))
          {
            s.variant = ((DAT_00590a30[(s.tile_y * s.tile_x) % 6] * DAT_00590a30[(s.tile_y + s.tile_x) % 6]) % 3) * 0x10 + (int)s.tile_class;
            DrawEncodedImageUiScaled(PTR_DAT_005832b4, s.screen_x_unscaled - 6, (s.screen_y_unscaled + 0x3a),
                                     g_ttsprite_grid_sprite_entries[s.variant], 0xe, 0xe);
          }

          s.screen_x = (s.screen_x_unscaled * global_screen_width) / 0x280;
          s.screen_y = ((s.screen_y_unscaled * global_screen_height) / 0x1e0) + ScaleUiCoordinate(0x40);

          for (s.dir = 1; s.dir < 9; s.dir++)
          {
            if (WorldRoadTileHasDirection(s.tile_x, s.tile_y, (char)(((unsigned char)s.dir & 7) + 1)) != 0)
            {
              int dx = ScaleUiCoordinate(g_neighbor_dx[s.dir] * 7);
              int dy = ScaleUiCoordinate(g_neighbor_dy[s.dir] * 7);
              DrawGraphicsLine(PTR_DAT_005832b4, s.screen_x, s.screen_y, s.screen_x + dx, s.screen_y + dy, 0xd2);
              DrawGraphicsLine(PTR_DAT_005832b4, s.screen_x - 1, s.screen_y, (s.screen_x - 1) + dx, s.screen_y + dy, 0xd2);
            }
          }
        }
      }
    }

    for (s.i = 0; s.i < 7; s.i++)
    {
      s.wizard_counts[s.i] = 0;
    }

    s.nearest_town_for_lair = -1;
    if (g_lair_or_monster_slots[7].entry_type != -1)
    {
      s.nearest_town_for_lair = FindNearestTownIndex((int)(g_lair_or_monster_slots[7].world_x + ((g_lair_or_monster_slots[7].world_x >> 0x1f) & 0x1fU)) >> 5,
                                                     (int)(g_lair_or_monster_slots[7].world_y + ((g_lair_or_monster_slots[7].world_y >> 0x1f) & 0x1fU)) >> 5);
    }

    /* Draw town markers + optional tooltips */
    for (s.tile_x = 0; s.tile_x < 0x40; s.tile_x++)
    {
      for (s.tile_y = 0; s.tile_y < 0x40; s.tile_y++)
      {
        s.tile_flags = FUN_004314ca(s.tile_x, s.tile_y);
        FUN_00550164(s.tile_x, s.tile_y, &s.screen_x_unscaled, &s.screen_y_unscaled);

        s.screen_x = (s.screen_x_unscaled * global_screen_width) / 0x280;
        s.screen_y = (s.screen_y_unscaled * global_screen_height) / 0x1e0;
        s.screen_y = s.screen_y + ScaleUiCoordinate(0x40);

        if ((s.screen_x < 0) || ((global_screen_width - 8) < s.screen_x) || (s.screen_y < 0) || ((global_screen_height - 8) < s.screen_y))
        {
          continue;
        }

        if ((s.tile_flags & 0x10) != 0)
        {
          s.town_index = FUN_004bb040(s.tile_x, s.tile_y);
          if (s.town_index < 0)
          {
            continue;
          }

          if (*(unsigned char *)((char *)&g_town_slots[s.town_index].status_and_ruling_wizard + 1) != 0)
          {
            int wiz = (int)((unsigned int)g_town_slots[s.town_index].status_and_ruling_wizard >> 8);
            s.wizard_counts[wiz] = s.wizard_counts[wiz] + 1;
          }

          if ((mode == 0) && ((g_town_slots[s.town_index].status_and_ruling_wizard & 1) != 0) && (1 < g_town_slots[s.town_index].location_type))
          {
            s.unused_town_counter = s.unused_town_counter + 1;
          }

          if (((s.tile_flags & 0x80) == 0) && (DAT_007894f4 == 0))
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

          DrawEncodedImageResampled(PTR_DAT_005832b4, s.screen_x - ScaleUiCoordinate(7), s.screen_y - ScaleUiCoordinate(0x12), ScaleUiCoordinate(0xd), ScaleUiCoordinate(0x14),
                                    g_ttsprite_aux_sprite_entries[s.saved_page_number]);

          if (1 < g_town_slots[s.town_index].location_type)
          {
            s.text_color = ((g_town_slots[s.town_index].status_and_ruling_wizard & 1) == 0) ? 0xe3 : 0xff;
            if (s.nearest_town_for_lair == s.town_index)
            {
              s.text_color = 0xbe;
            }
            if (*(unsigned char *)((char *)&g_town_slots[s.town_index].status_and_ruling_wizard + 1) != 0)
            {
              int wiz = (int)((unsigned int)g_town_slots[s.town_index].status_and_ruling_wizard >> 8);
              s.text_color = s.button_text_colors[wiz];
            }

            if (mode == 1)
            {
              s.wizard_count = 0;
              s.avatar_width = (int)g_world_magic_avatar_sprites[0]->width;

              s.mana_mask = FUN_005611c8(FUN_0043146b(g_town_slots[s.town_index].world_x, g_town_slots[s.town_index].world_y));
              for (s.dir = 1; s.dir < 6; s.dir++)
              {
                if ((s.mana_mask & (1U << (s.dir & 0x1f))) != 0)
                {
                  s.wizard_count++;
                }
              }

              s.avatar_x = (s.screen_x + ((s.wizard_count - 1) * s.avatar_width) / 2) - s.avatar_width / 2;
              for (s.dir = 1; s.dir < 6; s.dir++)
              {
                static int sprite_index_by_wizbit[6] = {0, 2, 1, 4, 3, 0};

                if ((s.mana_mask & (1U << (s.dir & 0x1f))) != 0)
                {
                  DrawEncodedImageUnscaled(PTR_DAT_005832b4, s.avatar_x, s.screen_y + 0xc, g_world_magic_avatar_sprites[sprite_index_by_wizbit[s.dir]]);
                  s.avatar_x -= s.avatar_width;
                }
              }

              if ((char)g_town_slots[s.town_index].trade_color_and_type == 0)
              {
                strcpy(g_ui_message_buffer, FUN_00428783(1U << (((unsigned char)((unsigned int)g_town_slots[s.town_index].trade_color_and_type >> 8) - 1U) & 0x1f)));
              }
              else
              {
                s.mana_castle_index = FUN_0040dffd(g_town_slots[s.town_index].trade_color_and_type & 0xff);
                strcpy(g_ui_message_buffer, gs_colorcards_0077c5e0 + s.mana_castle_index * 0x19);
              }

              s.text_width = MeasureMultilineTextWidth(PTR_DAT_005832b4, g_ui_message_buffer);
              s.line_height = GetFontLineHeight(PTR_DAT_005832b4->font_slot);

              ApplyPortraitTintMap(PTR_DAT_005832b4, (s.screen_x - 2) - s.text_width / 2, s.screen_y - 2, s.text_width + 4, s.line_height + 2, 0x3f3f3f,
                                   (s.any_tooltip_drawn == 0));
              s.any_tooltip_drawn = 1;
              DrawCenteredTextLineWithShadow(g_ui_message_buffer, s.screen_x, s.screen_y, s.text_color);

              for (s.dir = 0; s.dir < 0xc; s.dir++)
              {
                if ((s.town_index != 0) && (*(int *)(Scards + s.dir * 0x10 + 8) == s.town_index))
                {
                  (void)FUN_004bb458(s.dir);
                  strcpy(g_ui_message_buffer, gs_worldmagic_names_00780660[s.dir]);
                  s.text_width = MeasureMultilineTextWidth(PTR_DAT_005832b4, g_ui_message_buffer);
                  s.line_height = GetFontLineHeight(PTR_DAT_005832b4->font_slot);

                  ApplyPortraitTintMap(PTR_DAT_005832b4, (s.screen_x - 2) - s.text_width / 2,
                                       s.screen_y + (int)GetFontStyleSize(PTR_DAT_005832b4->font_slot),
                                       s.text_width + 4, s.line_height + 2, 0x3f3f3f, (s.any_tooltip_drawn == 0));
                  DrawCenteredTextLineWithShadow(g_ui_message_buffer, s.screen_x,
                                                 s.screen_y + (int)GetFontStyleSize(PTR_DAT_005832b4->font_slot),
                                                 s.text_color);
                }
              }
            }

            if ((mode == 0) || ((mode == 1) && (g_town_slots[s.town_index].location_type == 4)))
            {
              s.tooltip_line_height = (int)GetFontStyleSize(PTR_DAT_005832b4->font_slot);

              if ((g_town_slots[s.town_index].location_type == 4) || (g_town_slots[s.town_index].location_type == 5))
              {
                s.mana_castle_index = FUN_0040dffd((int)FUN_005611c8(FUN_0043146b(s.tile_x, s.tile_y)));
                strcpy(g_ui_message_buffer, FUN_00550314(s.town_index, s.mana_castle_index));
              }
              else
              {
                strcpy(g_ui_message_buffer, FUN_00550314(s.town_index, 0));
              }

              FUN_0054e97e(g_ui_message_buffer, s.line1, s.line2);

              strcpy(g_ui_message_buffer, s.line1);
              s.text_width = MeasureMultilineTextWidth(PTR_DAT_005832b4, g_ui_message_buffer);
              s.line_height = GetFontLineHeight(PTR_DAT_005832b4->font_slot);
              ApplyPortraitTintMap(PTR_DAT_005832b4, (s.screen_x - 2) - s.text_width / 2, s.screen_y - 2, s.text_width + 4, s.line_height + 2, 0x4f4f4f,
                                   (s.any_tooltip_drawn == 0));
              s.any_tooltip_drawn = 1;
              DrawCenteredTextLineWithShadow(g_ui_message_buffer, s.screen_x, s.screen_y, s.text_color);

              strcpy(g_ui_message_buffer, s.line2);
              s.text_width = MeasureMultilineTextWidth(PTR_DAT_005832b4, g_ui_message_buffer);
              s.line_height = GetFontLineHeight(PTR_DAT_005832b4->font_slot);
              ApplyPortraitTintMap(PTR_DAT_005832b4, (s.screen_x - 2) - s.text_width / 2, s.screen_y + s.tooltip_line_height, s.text_width + 4, s.line_height + 2, 0x4f4f4f, 0);
              DrawCenteredTextLineWithShadow(g_ui_message_buffer, s.screen_x, s.screen_y + s.tooltip_line_height, s.text_color);
            }
          }
        }

        if ((DAT_007894f4 != 0) && ((s.tile_flags & 0x40) != 0))
        {
          FillGraphicsRect(PTR_DAT_005832b4, s.screen_x + 1, s.screen_y + 1, 2, 2, 0xf6);
        }
      }
    }

    DrawEncodedImageResampled(PTR_DAT_005832b4, ScaleUiCoordinate(0xdc), ScaleUiCoordinate(0x36),
                              ScaleUiCoordinate((int)DAT_006506e0->width), ScaleUiCoordinate((int)DAT_006506e0->height), DAT_006506e0);

    if (s.original_mode != 4)
    {
      DrawEncodedImageResampled(PTR_DAT_005832b4, ScaleUiCoordinate(0), ScaleUiCoordinate(0x85),
                                ScaleUiCoordinate((int)DAT_00650f1c->width), ScaleUiCoordinate((int)DAT_00650f1c->height), DAT_00650f1c);
      for (s.i = 0; s.i < 5; s.i++)
      {
        DrawTextAt(PTR_DAT_005832b4, 0xfe, 0x18, s.i * 0x1a + 0xc6, DAT_00590bc8);
      }
    }

    PTR_DAT_005832b4->page_number = 0;
    BlitGraphicsRect(PTR_DAT_005832dc, 0, 0, global_screen_width, global_screen_height, PTR_DAT_005832b4, 0, 0);

    if (s.original_mode == 4)
    {
      EndMenuContext();
      return;
    }

    FUN_005001e3();

    FUN_00550164(g_world_player_tile_x, g_world_player_tile_y, &s.screen_x_unscaled, &s.screen_y_unscaled);
    s.screen_x = (s.screen_x_unscaled * global_screen_width) / 0x280;
    s.screen_y = ScaleUiCoordinate(0x40) + (s.screen_y_unscaled * global_screen_height) / 0x1e0;

    DAT_006506e8 = -1;
    s.blink_on = 1;
    ConsumeUiTickCount();

    while (DAT_006506e8 == -1)
    {
      s.tick = GetUiTickCount();
      if ((s.tick % 0x14 < 10) && (s.blink_on != 0))
      {
        FillGraphicsRect(PTR_DAT_005832b4, s.screen_x - 1, s.screen_y - 1, 4, 4, 0xff);
        s.blink_on = 0;
      }
      else if ((9 < (s.tick % 0x14)) && (s.blink_on == 0))
      {
        FillGraphicsRect(PTR_DAT_005832b4, s.screen_x - 1, s.screen_y - 1, 4, 4, 0);
        s.blink_on = 1;
      }

      UpdateMouseSnapshot();

      if (g_mouse_button_down_mask == 0)
      {
        UpdateMenuControlSelection(g_mouse_x_snapshot, g_mouse_y_snapshot, 0);
      }
      else
      {
        UpdateMenuControlSelection(g_mouse_x_snapshot, g_mouse_y_snapshot, g_mouse_button_down_mask);
        if ((g_mouse_button_down_mask != 0) && (DAT_007894f4 != 0) && (DAT_006506e8 < 0))
        {
          BlitGraphicsRect(PTR_DAT_005832dc, s.screen_x - 1, s.screen_y - 1, 4, 4, PTR_DAT_005832b4, s.screen_x - 1, s.screen_y - 1);
          s.screen_x = g_mouse_x_snapshot;
          s.screen_y = g_mouse_y_snapshot;

          s.screen_x_unscaled = (g_mouse_x_snapshot * 0x280) / global_screen_width;
          s.screen_y_unscaled = ((g_mouse_y_snapshot - ScaleUiCoordinate(0x40)) * 0x1e0) / global_screen_height;
          FUN_00550197(s.screen_x_unscaled, s.screen_y_unscaled, &g_world_player_tile_x, &g_world_player_tile_y);
          g_world_player_x = g_world_player_tile_x * 0x20 + 0x10;
          g_world_player_y = g_world_player_tile_y * 0x20 + 0x10;

          FUN_00550164(g_world_player_tile_x, g_world_player_tile_y, &s.screen_x_unscaled, &s.screen_y_unscaled);
          s.screen_x = (s.screen_x_unscaled * global_screen_width) / 0x280;
          s.screen_y = ScaleUiCoordinate(0x40) + (s.screen_y_unscaled * global_screen_height) / 0x1e0;

          g_world_scroll_cache_ready = 0;
        }
      }
    }

    if (DAT_006506e8 == 3)
    {
      PopNormalizedQueuedKeyInput();
      ShowCityInfoScreen(1);
      mode = 0;
      EndMenuContext();
    }
    else if (DAT_006506e8 == 4)
    {
      EndMenuContext();
      FreeSpriteBlob(DAT_0073eab0);
      PopNormalizedQueuedKeyInput();
      return;
    }
    else
    {
      mode = DAT_006506e8;
      PopNormalizedQueuedKeyInput();
      EndMenuContext();
    }
  }
}
