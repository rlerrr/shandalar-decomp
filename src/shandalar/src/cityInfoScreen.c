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

// External engine state (defined in shandalar.c / other modules)
extern int global_screen_width;
extern int global_screen_height;
extern int g_menu_render_guard;
extern int g_menu_context_index;
extern int g_menu_prev_control_index;
extern int g_menu_allow_arrow_nav_by_context[50];
extern int g_menu_control_count_by_context[50];
extern AdvMenuControl *g_menu_controls_by_context[50][50];

extern int g_mouse_x;
extern int g_mouse_y;
extern int g_mouse_button_down_mask;
extern int g_mouse_x_snapshot;
extern int g_mouse_y_snapshot;
extern int g_mouse_button_mask_snapshot;

extern FILE *g_advbuttons_ini_file;
extern char g_ini_string_scratch[0x28];
extern char g_ui_message_buffer[0x1000];
extern int g_done_text_table_entry;

extern FacemakerWindowBounds *g_page0_window_bounds;
extern FacemakerWindowBounds *g_page1_window_bounds;
extern FacemakerWindowBounds *g_page2_window_bounds;

extern int g_world_scroll_cache_ready;
extern WorldMagicSlotTimer g_world_magic_slot_timers[0xc];

// External functions
int *LoadIniEscapedStringTable(FILE *ini_file, char *section_name, char *scratch);
void LoadPcxIntoPage(int page_number, char *path);
void LoadPcxResource(int page_number, int x, int y, char *path, void *opaque);
void BeginSpriteEncodeSession(void);
EncodedImage *EncodeSpriteFromPage(int page_number, int x, int y, int width, int height);
void FinalizeSpriteEncodeSession(void);

int ScaleUiCoordinate(int value);
int SetFontStyleSize(int font_slot, unsigned int point_size);
void DrawTextAt(FacemakerWindowBounds *dst, int text_id, int x, int y, char *text, ...);
int DrawTextFormatted(FacemakerWindowBounds *dst, int text_color, int draw_shadow, int scale_to_screen, int center_x, int center_y, int x,
                      int y, int *format_and_args);
void DrawEncodedImageResampled(FacemakerWindowBounds *dst, int x, int y, int width, int height, EncodedImage *encoded_image);
void StretchBlitGraphicsRect(FacemakerWindowBounds *dst, int dst_x, int dst_y, int src_w, int src_h,
                             FacemakerWindowBounds *src, int src_x, int src_y, int copy_w, int copy_h);
void CopyGraphicsRect(FacemakerWindowBounds *src, int src_x, int src_y, int width, int height,
                      FacemakerWindowBounds *dst, int dst_x, int dst_y);
void FreeSpriteBlob(void *memory);

int BeginMenuContext(void);
int ResetMenuContext(int context_index);
int AddMenuControlsToContext(AdvMenuControl *controls, int control_count, int context_index);
int EndMenuContext(void);

void UpdateMouseSnapshot(void);
int UpdateMenuControlSelection(int mouse_x, int mouse_y, int allow_activate_on_click);
int HasQueuedKeyInput(void);
extern EncodedImage *g_world_magic_avatar_sprites[5];

void EnsureAdvfac64Loaded(int state);
void PlaySoundEffectOnChannel(char *sound_path, int channel, int volume, int pitch_percent, int pan_percent);

unsigned int GetWorldTileType(int x, int y);
unsigned int GetWorldTileMagicMask(unsigned int tile_mask);
int FindWorldMagicCardIndex(int world_magic_slot_index);
char *GetTownCardDescription(int town_index);

// From city-info helpers
DWORD __cdecl FormatMessageFromStringStripCarriageReturns(char *dst, DWORD dst_len, LPCVOID format, ...);
char *BuildTownDisplayName(int town_index);
int __cdecl DrawCityInfoTownRow(FacemakerWindowBounds *dst, int town_index, int x, int y);
void __cdecl DrawFormattedTextNoShadowCentered(FacemakerWindowBounds *dst, int text_color, int x, int y, char *format, ...);
void __cdecl DrawFormattedTextShadowedCentered(FacemakerWindowBounds *window, int color_index, int x, int y, char *format, ...);
int RenderAdvMenuControlDisabled(AdvMenuControl *control);

// GLOBAL: SHANDALAR 0x00603a34
int g_adv_menu_selected_value;
// GLOBAL: SHANDALAR 0x00603a38
int g_city_info_strings_loaded;
// GLOBAL: SHANDALAR 0x00603a48
int *g_city_info_heading_strings;

// GLOBAL: SHANDALAR 0x00746e60
EncodedImage *g_city_info_done_button_sprites[3];
// GLOBAL: SHANDALAR 0x00746e70
EncodedImage *g_city_info_scroll_button_sprites[0x10];

extern int g_reveal_all_world_info;

// GLOBAL: SHANDALAR 0x00581918
int g_wizard_text_colors[0x10] = {
    0x000000d2,
    0x00000000,
    0x000000f4,
    0x000000ce,
    0x000000bc,
    0x00000003,
    0x00000000,
    0x0000001b,
    0x00000002,
    0x0000000d,
    0x00000003,
    0x00000018,
    0x00000004,
    0x00000001,
    0x00000005,
    0x0000001d,
};

// Forward decls
int __cdecl RenderCityInfoScrollButton(AdvMenuControl *control, int mode);
int __cdecl ActivateCityInfoScrollButton(AdvMenuControl *control);
int __cdecl RenderCityInfoDoneButton(AdvMenuControl *control, int mode);
int __cdecl ActivateCityInfoDoneButton(AdvMenuControl *control);
extern char g_city_info_done_navigate_hotkeys[];
extern char g_city_info_done_activate_hotkeys[];

// GLOBAL: SHANDALAR 0x0058c8f8
AdvMenuControl g_city_info_menu_controls[5] = {
    // PageUp
    {27, 171, 17, 58, 27, 171, 17, 58, 1, (AdvMenuRenderCallback)RenderCityInfoScrollButton, (AdvMenuActivateCallback)ActivateCityInfoScrollButton, -2, 0, (char *)0, (char *)0, 0x4900, 0, {0, 0, 0, 0}},
    // Up
    {27, 230, 17, 58, 27, 230, 17, 58, 1, (AdvMenuRenderCallback)RenderCityInfoScrollButton, (AdvMenuActivateCallback)ActivateCityInfoScrollButton, -1, 1, (char *)0, (char *)0, 0x4800, 0, {0, 0, 0, 0}},
    // Down
    {27, 289, 17, 58, 27, 289, 17, 58, 1, (AdvMenuRenderCallback)RenderCityInfoScrollButton, (AdvMenuActivateCallback)ActivateCityInfoScrollButton, 1, 2, (char *)0, (char *)0, 0x5000, 0, {0, 0, 0, 0}},
    // PageDown
    {27, 348, 17, 58, 27, 348, 17, 58, 1, (AdvMenuRenderCallback)RenderCityInfoScrollButton, (AdvMenuActivateCallback)ActivateCityInfoScrollButton, 2, 3, (char *)0, (char *)0, 0x5100, 0, {0, 0, 0, 0}},
    // Done
    {544, 28, 58, 26, 544, 28, 58, 26, 1, (AdvMenuRenderCallback)RenderCityInfoDoneButton, (AdvMenuActivateCallback)ActivateCityInfoDoneButton, 0, 0, g_city_info_done_navigate_hotkeys, g_city_info_done_activate_hotkeys, 0, 0, {0, 0, 0, 0}},
};

// GLOBAL: SHANDALAR 0x0058cad4
char g_city_info_done_navigate_hotkeys[8] = " \rDd\x1b";

// GLOBAL: SHANDALAR 0x0058cadc
char g_city_info_done_activate_hotkeys[8] = " \rDd\x1b";

// FUNCTION: SHANDALAR 0x004ffd0a
int RenderAdvMenuControlNormally(AdvMenuControl *control)
{
  int old_state;

  old_state = control->state;
  control->state = 0;
  g_menu_render_guard = 1;
  control->on_render(control, 0);
  g_menu_render_guard = 0;
  return old_state;
}

// FUNCTION: SHANDALAR 0x00500129
int __cdecl SetCurrentMenuContextArrowNavigation(int allow_arrow_nav)
{
  g_menu_allow_arrow_nav_by_context[g_menu_context_index] = allow_arrow_nav;
  return 1;
}

// FUNCTION: SHANDALAR 0x005001e3
int RenderCurrentMenuContextControls(void)
{
  int i;

  g_menu_render_guard = 1;
  for (i = 0; i < g_menu_control_count_by_context[g_menu_context_index]; i++)
  {
    g_menu_controls_by_context[g_menu_context_index][i]->on_render(g_menu_controls_by_context[g_menu_context_index][i],
                                                                   (g_menu_prev_control_index == i));
  }
  g_menu_render_guard = 0;
  return 1;
}

// FUNCTION: SHANDALAR 0x0050a709
int __cdecl RenderCityInfoScrollButton(AdvMenuControl *control, int mode)
{
  int in_bounds;

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

  DrawEncodedImageResampled(g_page0_window_bounds, control->x, control->y, control->width, control->height,
                            g_city_info_scroll_button_sprites[control->data_value * 4 + mode]);

  if ((mode == 2) && (control->on_activate != (AdvMenuActivateCallback)0))
  {
    control->on_activate(control);
  }

  return 1;
}

// FUNCTION: SHANDALAR 0x0050a828
int __cdecl ActivateCityInfoScrollButton(AdvMenuControl *control)
{
  PlaySoundEffectOnChannel("x:sound\\button2.wav", 0xf, 100, 100, 0);
  g_adv_menu_selected_value = control->selection_value;
  return 0;
}

// FUNCTION: SHANDALAR 0x0050a85a
int __cdecl RenderCityInfoDoneButton(AdvMenuControl *control, int mode)
{
  int in_bounds;

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

  DrawEncodedImageResampled(g_page0_window_bounds, control->x, control->y, control->width, control->height, g_city_info_done_button_sprites[mode]);

  if ((mode == 2) && (control->on_activate != (AdvMenuActivateCallback)0))
  {
    control->on_activate(control);
  }

  return 1;
}

// FUNCTION: SHANDALAR 0x0050a970
int __cdecl ActivateCityInfoDoneButton(AdvMenuControl *control)
{
  PlaySoundEffectOnChannel("x:sound\\button2.wav", 0xf, 100, 100, 0);
  g_adv_menu_selected_value = control->selection_value;
  return 0;
}

static void CityInfo_DrawVisibleRows(int *town_indices_1based, int town_count, int first_index, int list_top_y, int row_height_scaled)
{
  int i;
  int y0;

  y0 = list_top_y + ScaleUiCoordinate(0x15);
  for (i = 0; i < 9; i++)
  {
    int list_index = first_index + i;
    if (list_index < town_count)
    {
      DrawCityInfoTownRow(g_page1_window_bounds, town_indices_1based[list_index + 1], 0x30, y0 + i * row_height_scaled);
    }
  }
}

// FUNCTION: SHANDALAR 0x0050a9a2
void ShowCityInfoScreen(int unused)
{
  struct
  {
    int list_buffer_y;       // ebp - 0x444
    int x_table[3];          // ebp - 0x440
    int list_panel_x;        // ebp - 0x434
    int row_height_scaled;   // ebp - 0x430
    int menu_context;        // ebp - 0x42c
    int list_width_scaled;   // ebp - 0x428
    int scroll_top_index;    // ebp - 0x424
    int j;                   // ebp - 0x420
    int scroll_input;        // ebp - 0x41c
    int row_y;               // ebp - 0x418
    int i;                   // ebp - 0x414
    int visible_town_index;  // ebp - 0x410
    int town_indices[0x100]; // ebp - 0x40c
    int town_count;          // ebp - 0xc
    int town_scan_index;     // ebp - 0x8
    int list_top_y;          // ebp - 0x4
  } s;

  (void)unused;

  s.town_count = 0;
  s.scroll_top_index = 0;

  if (g_city_info_strings_loaded == 0)
  {
    g_city_info_heading_strings = LoadIniEscapedStringTable(g_advbuttons_ini_file, "cityInfo", g_ini_string_scratch);
    g_city_info_strings_loaded = 1;
  }

  LoadPcxIntoPage(1, "infobar.pic");

  BeginSpriteEncodeSession();

  // Scale/encode the 4 scroll buttons
  for (s.j = 0; s.j < 4; s.j++)
  {
    if (g_city_info_menu_controls[s.j].x == g_city_info_menu_controls[s.j].base_x)
    {
      g_city_info_menu_controls[s.j].x = ScaleUiCoordinate(g_city_info_menu_controls[s.j].x);
      g_city_info_menu_controls[s.j].y = ScaleUiCoordinate(g_city_info_menu_controls[s.j].y);
      g_city_info_menu_controls[s.j].width = ScaleUiCoordinate(g_city_info_menu_controls[s.j].width);
      g_city_info_menu_controls[s.j].height = ScaleUiCoordinate(g_city_info_menu_controls[s.j].height);
    }

    for (s.i = 0; s.i < 4; s.i++)
    {
      *(EncodedImage **)((char *)g_city_info_scroll_button_sprites + s.j * 0x10 + s.i * 4) = EncodeSpriteFromPage(1, s.i * 0x12 + 0x2b, s.j * 0x31 + 0x1c, 0x11, 0x2f);
    }
  }

  // Scale the Done button
  if (g_city_info_menu_controls[4].x == g_city_info_menu_controls[4].base_x)
  {
    g_city_info_menu_controls[4].x = ScaleUiCoordinate(g_city_info_menu_controls[4].x);
    g_city_info_menu_controls[4].y = ScaleUiCoordinate(g_city_info_menu_controls[4].y);
    g_city_info_menu_controls[4].width = ScaleUiCoordinate(g_city_info_menu_controls[4].width);
    g_city_info_menu_controls[4].height = ScaleUiCoordinate(g_city_info_menu_controls[4].height);
  }

  // Build 3-state Done button sprites into g_city_info_done_button_sprites
  g_page1_window_bounds->font_slot = 7;
  SetFontStyleSize(7, (unsigned int)ScaleUiCoordinate(10));
  for (s.i = 0; s.i < 3; s.i++)
  {
    s.x_table[0] = 0x48;
    s.x_table[1] = 0x3f;
    s.x_table[2] = 0x40;

    SetFontStyleSize(7, (unsigned int)(10 - s.i / 2));
    DrawFormattedTextNoShadowCentered(g_page1_window_bounds, s.x_table[s.i], s.i * 0x3c + 0x49, 0xe, (char *)g_done_text_table_entry);
  }

  for (s.i = 0; s.i < 3; s.i++)
  {
    g_city_info_done_button_sprites[s.i] = EncodeSpriteFromPage(1, s.i * 0x3c + 0x2b, 1, 0x3a, 0x18);
  }

  FinalizeSpriteEncodeSession();

  s.menu_context = BeginMenuContext();
  ResetMenuContext(s.menu_context);
  AddMenuControlsToContext(g_city_info_menu_controls, 5, s.menu_context);
  SetCurrentMenuContextArrowNavigation(0);

  // Prepare background
  g_world_scroll_cache_ready = 1;
  LoadPcxResource(1, 0, global_screen_height - 0x1e0, "cityinfo.pic", (void *)0);
  StretchBlitGraphicsRect(g_page1_window_bounds, 0, global_screen_height - 0x1e0, 0x280, 0x1e0, g_page1_window_bounds, 0, 0, global_screen_width,
                          global_screen_height);

  // Headings, loaded from advButtons [cityInfo] (format strings in .rdata are \"%s\")
  g_page1_window_bounds->font_slot = 7;
  SetFontStyleSize(7, (unsigned int)ScaleUiCoordinate(10));
  DrawTextAt(g_page1_window_bounds, 200, 0x69, 0x28, "%s", (char *)g_city_info_heading_strings[0]);
  DrawTextAt(g_page1_window_bounds, 200, 0x54, 0x45, "%s", (char *)g_city_info_heading_strings[1]);
  DrawTextAt(g_page1_window_bounds, 200, 0xb3, 0x45, "%s", (char *)g_city_info_heading_strings[2]);
  DrawTextAt(g_page1_window_bounds, 200, 0x130, 0x45, "%s", (char *)g_city_info_heading_strings[3]);
  DrawTextAt(g_page1_window_bounds, 200, 0x1cc, 0x45, "%s", (char *)g_city_info_heading_strings[4]);
  DrawTextAt(g_page1_window_bounds, 200, 0x23f, 0x45, "%s", (char *)g_city_info_heading_strings[5]);

  EnsureAdvfac64Loaded(1);

  // Reset font slots
  g_page0_window_bounds->font_slot = 1;
  g_page1_window_bounds->font_slot = 1;
  g_page2_window_bounds->font_slot = 1;

  LoadPcxResource(2, 0, g_page2_window_bounds->max_y - 0x46, "cinfopce.pic", (void *)0);

  // List panel geometry
  s.list_panel_x = (global_screen_width * 0x30 + global_screen_width / 2) / 0x280;
  s.list_top_y = ScaleUiCoordinate(0x52);
  s.list_width_scaled = ScaleUiCoordinate(0x231);
  s.row_height_scaled = ScaleUiCoordinate(0x2a);

  // Build repeating row background on the offscreen page
  StretchBlitGraphicsRect(g_page2_window_bounds, 0, g_page2_window_bounds->max_y - 0x2c, 0x231, 0x2a, g_page2_window_bounds, 0, 0x80, s.list_width_scaled,
                          s.row_height_scaled);
  for (s.i = 0; s.i < 9; s.i++)
  {
    CopyGraphicsRect(g_page2_window_bounds, 0, 0x80, s.list_width_scaled, s.row_height_scaled, g_page1_window_bounds, s.list_panel_x,
                     s.i * s.row_height_scaled + s.list_top_y);
  }

  CopyGraphicsRect(g_page1_window_bounds, 0, 0, global_screen_width, global_screen_height, g_page0_window_bounds, 0, 0);

  // Build town list (1-based indexing like the original)
  s.row_y = 2;
  for (s.town_scan_index = 0; s.town_scan_index < 0x80; s.town_scan_index++)
  {
    if ((((g_town_slots[s.town_scan_index].status_and_ruling_wizard & 2) == 0) && (g_reveal_all_world_info == 0)) ||
        (g_town_slots[s.town_scan_index].location_type == 1))
    {
      continue;
    }

    s.town_indices[s.town_count++] = s.town_scan_index;
  }

  for (s.i = 0; s.i < 5; s.i++)
  {
    DrawTextAt(g_page0_window_bounds, 0xfe, s.i * 0x2a + 0xcc, 0x2a, "%d", g_amulet_inventory[s.i]);
  }

main_loop:
  // Rebuild row backgrounds
  for (s.i = 0; s.i < 9; s.i++)
  {
    CopyGraphicsRect(g_page2_window_bounds, 0, 0x80, s.list_width_scaled, s.row_height_scaled, g_page1_window_bounds, s.list_panel_x,
                     s.i * s.row_height_scaled + s.list_top_y);
  }

  s.visible_town_index = s.scroll_top_index;
  for (s.j = 0; ((s.town_count - s.scroll_top_index < 9) ? s.town_count - s.scroll_top_index : 9) > s.j; s.j++, s.visible_town_index++)
  {
    s.town_scan_index = s.town_indices[s.visible_town_index];
    s.row_y = ScaleUiCoordinate(0x68) + s.j * s.row_height_scaled;
    DrawCityInfoTownRow(g_page1_window_bounds, s.town_scan_index, 0x30, s.row_y);
  }

  // Copy list area to the visible page
  CopyGraphicsRect(g_page1_window_bounds, s.list_panel_x, s.list_top_y, s.list_width_scaled, s.row_height_scaled * 9, g_page0_window_bounds, s.list_panel_x,
                   s.list_top_y);

  if (s.town_count > 9)
  {
    if (s.scroll_top_index == 0)
    {
      s.list_buffer_y = 0;
    }
    else
    {
      s.list_buffer_y = s.row_height_scaled;
    }

    CopyGraphicsRect(g_page1_window_bounds, s.list_panel_x, s.list_top_y, s.list_width_scaled, s.row_height_scaled * 9, g_page1_window_bounds, s.list_panel_x,
                     s.list_buffer_y);

    if (s.scroll_top_index == 0)
    {
      CopyGraphicsRect(g_page2_window_bounds, 0, 0x80, s.list_width_scaled, s.row_height_scaled, g_page1_window_bounds, s.list_panel_x,
                       s.row_height_scaled * 9);
      DrawCityInfoTownRow(g_page1_window_bounds, s.town_indices[s.scroll_top_index + 9], 0x30, ScaleUiCoordinate(0x15) + s.row_height_scaled * 9);

      CopyGraphicsRect(g_page2_window_bounds, 0, 0x80, s.list_width_scaled, s.row_height_scaled, g_page1_window_bounds, s.list_panel_x,
                       s.row_height_scaled * 10);

      if (s.scroll_top_index + 10 < s.town_count)
      {
        DrawCityInfoTownRow(g_page1_window_bounds, s.town_indices[s.scroll_top_index + 10], 0x30, ScaleUiCoordinate(0x15) + s.row_height_scaled * 10);
      }
    }
    else
    {
      CopyGraphicsRect(g_page2_window_bounds, 0, 0x80, s.list_width_scaled, s.row_height_scaled, g_page1_window_bounds, s.list_panel_x, 0);
      DrawCityInfoTownRow(g_page1_window_bounds, s.town_indices[s.scroll_top_index - 1], 0x30, ScaleUiCoordinate(0x15));

      if (s.scroll_top_index + 10 < s.town_count)
      {
        CopyGraphicsRect(g_page2_window_bounds, 0, 0x80, s.list_width_scaled, s.row_height_scaled, g_page1_window_bounds, s.list_panel_x,
                         s.row_height_scaled * 10);
      }

      if (s.scroll_top_index + 10 < s.town_count)
      {
        DrawCityInfoTownRow(g_page1_window_bounds, s.town_indices[s.scroll_top_index + 9], 0x30, ScaleUiCoordinate(0x15) + s.row_height_scaled * 10);
      }
    }
  }
  if (s.scroll_top_index == 0)
  {
    s.row_y = 0;
  }
  else
  {
    s.row_y = s.row_height_scaled;
  }

page_loop:
  // Enable/disable scroll buttons
  if (s.town_count > 9)
  {
    if (s.scroll_top_index == 0)
    {
      RenderAdvMenuControlDisabled(&g_city_info_menu_controls[0]);
      RenderAdvMenuControlDisabled(&g_city_info_menu_controls[1]);
    }
    else
    {
      RenderAdvMenuControlNormally(&g_city_info_menu_controls[0]);
      RenderAdvMenuControlNormally(&g_city_info_menu_controls[1]);
    }

    if (s.town_count - 9 == s.scroll_top_index)
    {
      RenderAdvMenuControlDisabled(&g_city_info_menu_controls[2]);
      RenderAdvMenuControlDisabled(&g_city_info_menu_controls[3]);
    }
    else
    {
      RenderAdvMenuControlNormally(&g_city_info_menu_controls[2]);
      RenderAdvMenuControlNormally(&g_city_info_menu_controls[3]);
    }
  }
  else
  {
    RenderAdvMenuControlDisabled(&g_city_info_menu_controls[0]);
    RenderAdvMenuControlDisabled(&g_city_info_menu_controls[1]);
    RenderAdvMenuControlDisabled(&g_city_info_menu_controls[2]);
    RenderAdvMenuControlDisabled(&g_city_info_menu_controls[3]);
  }

  RenderCurrentMenuContextControls();

  g_adv_menu_selected_value = -5;
  do
  {
    UpdateMouseSnapshot();
    UpdateMenuControlSelection(g_mouse_x_snapshot, g_mouse_y_snapshot, g_mouse_button_down_mask);
  } while ((g_adv_menu_selected_value == -5) && (HasQueuedKeyInput() == 0));

  if (g_adv_menu_selected_value == 0)
  {
    EndMenuContext();
    FreeSpriteBlob(g_city_info_scroll_button_sprites[0]);
    return;
  }

  switch (g_adv_menu_selected_value)
  {
  case -2: // PageUp
    s.scroll_input = 0x4900;
    break;
  case -1: // Up
    s.scroll_input = 0x4800;
    break;
  case 1: // Down
    s.scroll_input = 0x5000;
    break;
  case 2: // PageDown
    s.scroll_input = 0x5100;
    break;
  default:
    goto page_loop;
  }

  if (s.town_count > 9)
  {
    if (s.scroll_input == 0x4800)
    {
      s.scroll_top_index--;
      if (s.scroll_top_index < 0)
      {
        s.scroll_top_index = 0;
      }
      else
      {

        for (s.i = 0; ScaleUiCoordinate(0x2b) > s.i; s.i += 3)
        {
          CopyGraphicsRect(g_page1_window_bounds, s.list_panel_x, s.row_y - s.i + ScaleUiCoordinate(3), s.list_width_scaled,
                           s.row_height_scaled * 9 - ScaleUiCoordinate(5), g_page0_window_bounds, s.list_panel_x, s.list_top_y + ScaleUiCoordinate(3));
        }

        CopyGraphicsRect(g_page1_window_bounds, s.list_panel_x, 0, s.list_width_scaled, s.row_height_scaled * 9 - ScaleUiCoordinate(5), g_page0_window_bounds,
                         s.list_panel_x, s.list_top_y);

        if (s.scroll_top_index >= 1)
        {
          CopyGraphicsRect(g_page1_window_bounds, s.list_panel_x, 0, s.list_width_scaled, s.row_height_scaled * 10, g_page1_window_bounds, s.list_panel_x,
                           s.row_height_scaled);
          s.row_y = s.row_height_scaled;
          CopyGraphicsRect(g_page2_window_bounds, 0, 0x80, s.list_width_scaled, s.row_height_scaled, g_page1_window_bounds, s.list_panel_x, 0);
          DrawCityInfoTownRow(g_page1_window_bounds, s.town_indices[s.scroll_top_index - 1], 0x30, ScaleUiCoordinate(0x15));
        }
        else
        {
          s.row_y = 0;
        }
      }
      goto page_loop;
    }
    else if (s.scroll_input == 0x5000)
    {
      if (s.scroll_top_index != s.town_count - 9)
      {
        for (s.i = 0; ScaleUiCoordinate(0x2b) > s.i; s.i += 3)
        {
          CopyGraphicsRect(g_page1_window_bounds, s.list_panel_x, s.row_y + s.i + ScaleUiCoordinate(3), s.list_width_scaled,
                           s.row_height_scaled * 9 - ScaleUiCoordinate(5), g_page0_window_bounds, s.list_panel_x, s.list_top_y + ScaleUiCoordinate(3));
        }

        CopyGraphicsRect(g_page1_window_bounds, s.list_panel_x, s.row_y + s.row_height_scaled, s.list_width_scaled, s.row_height_scaled * 9 - ScaleUiCoordinate(5),
                         g_page0_window_bounds, s.list_panel_x, s.list_top_y);

        s.row_y = s.row_height_scaled;
        if ((s.scroll_top_index >= 1) && (s.scroll_top_index < s.town_count - 9))
        {
          CopyGraphicsRect(g_page1_window_bounds, s.list_panel_x, s.row_height_scaled, s.list_width_scaled, s.row_height_scaled * 10, g_page1_window_bounds,
                           s.list_panel_x, 0);
          if (s.scroll_top_index + 10 < s.town_count)
          {
            CopyGraphicsRect(g_page2_window_bounds, 0, 0x80, s.list_width_scaled, s.row_height_scaled, g_page1_window_bounds, s.list_panel_x,
                             s.row_height_scaled * 10);
          }

          if (s.scroll_top_index + 10 < s.town_count)
          {
            DrawCityInfoTownRow(g_page1_window_bounds, s.town_indices[s.scroll_top_index + 10], 0x30,
                                ScaleUiCoordinate(0x15) + s.row_height_scaled * 10);
          }
        }
      }

      s.scroll_top_index++;

      if (s.town_count - 9 < s.scroll_top_index)
      {
        s.scroll_top_index = s.town_count - 9;
      }

      goto page_loop;
    }
    else if (s.scroll_input == 0x4900)
    {
      s.scroll_top_index -= 8;
      s.scroll_top_index = MAX(s.scroll_top_index, 0);
      goto main_loop;
    }
    else if (s.scroll_input == 0x5100)
    {
      s.scroll_top_index += 8;
      s.scroll_top_index = MIN(s.scroll_top_index, s.town_count - 9);
      goto main_loop;
    }
    else
    {
      goto page_loop;
    }
  }

  EndMenuContext();
  FreeSpriteBlob(g_city_info_scroll_button_sprites[0]);
}

// FUNCTION: SHANDALAR 0x0050bb6d
int __cdecl DrawCityInfoTownRow(FacemakerWindowBounds *dst, int town_index, int x, int y)
{
  struct
  {
    int avatar_indices[5];
    char *space_fwd;
    char *space_back;
    unsigned int tile_class;
    int world_magic_slot_result;
    int wizard_sprite_x;
    int i;
    char *mid;
    int sprite_height;
    int sprite_width;
    int text_color;
    int wizard_count;
  } s;

  (void)dst;

  if (g_town_slots[town_index].location_type != 4)
  {
    strcpy(g_ui_message_buffer, BuildTownDisplayName(town_index));
  }
  else
  {
    strcpy(g_ui_message_buffer, gs_cityname_castle_00765dc0);
  }

  s.mid = g_ui_message_buffer + (strlen(g_ui_message_buffer) >> 1);

  for (s.space_fwd = s.mid; (*s.space_fwd != '\0') && (*s.space_fwd != ' '); s.space_fwd++)
  {
  }
  if (*s.space_fwd != ' ')
  {
    s.space_fwd = (char *)0;
  }

  for (s.space_back = s.mid; (*s.space_back != ' ') && (s.space_back >= g_ui_message_buffer); s.space_back--)
  {
  }
  if (*s.space_back != ' ')
  {
    s.space_back = (char *)0;
  }

  if ((s.space_fwd != (char *)0) && ((s.space_back == (char *)0) || ((s.space_back - s.mid) <= (s.mid - s.space_fwd))))
  {
    *s.space_fwd = '\n';
  }
  else if ((s.space_back != (char *)0) && ((s.space_fwd == (char *)0) || ((s.space_back - s.mid) >= (s.mid - s.space_fwd))))
  {
    *s.space_back = '\n';
  }

  strcat(g_ui_message_buffer, "\n");

  s.tile_class = GetWorldTileMagicMask(GetWorldTileType(g_town_slots[town_index].world_x, g_town_slots[town_index].world_y));

  if ((g_town_slots[town_index].status_and_ruling_wizard & 1) != 0)
  {
    s.text_color = 0xff;
  }
  else
  {
    s.text_color = 0xe3;
  }

  if (g_town_slots[town_index].status_and_ruling_wizard & 0xff00)
  {
    s.text_color = g_wizard_text_colors[(((int)(g_town_slots[town_index].status_and_ruling_wizard & 0xffffff00)) >> 8)];
  }

  DrawFormattedTextShadowedCentered(g_page1_window_bounds, s.text_color, ScaleUiCoordinate(x + 0x2a), y, g_ui_message_buffer);
  if ((g_town_slots[town_index].status_and_ruling_wizard & 1) != 0)
  {
    DrawFormattedTextShadowedCentered(g_page1_window_bounds, s.text_color, ScaleUiCoordinate(x + 0x20c), y, "X");
  }

  s.tile_class = GetWorldTileMagicMask(GetWorldTileType(g_town_slots[town_index].world_x, g_town_slots[town_index].world_y));

  s.wizard_count = 0;
  for (s.i = 1; s.i < 6; s.i++)
  {
    if ((s.tile_class & (1U << (unsigned char)s.i)) != 0)
    {
      s.wizard_count++;
    }
  }

  s.sprite_width = (int)g_world_magic_avatar_sprites[0]->width;
  s.sprite_height = (int)g_world_magic_avatar_sprites[0]->height;
  s.wizard_sprite_x = ScaleUiCoordinate(((x + 0x80) - (s.sprite_width * s.wizard_count) / 2) - ((s.wizard_count - 1) * 5));

  for (s.i = 1; s.i < 6; s.i++)
  {
    s.avatar_indices[0] = 2;
    s.avatar_indices[1] = 1;
    s.avatar_indices[2] = 4;
    s.avatar_indices[3] = 3;
    s.avatar_indices[4] = 0;

    if ((s.tile_class & (1U << (unsigned char)s.i)) != 0)
    {
      DrawEncodedImageResampled(g_page1_window_bounds, s.wizard_sprite_x, y - ScaleUiCoordinate(s.sprite_height / 2), ScaleUiCoordinate(s.sprite_width),
                                ScaleUiCoordinate(s.sprite_height), g_world_magic_avatar_sprites[s.avatar_indices[s.i - 1]]);
      s.wizard_sprite_x += ScaleUiCoordinate(s.sprite_width + 5);
    }
  }

  strcpy(g_ui_message_buffer, GetTownCardDescription(town_index));
  DrawFormattedTextShadowedCentered(g_page1_window_bounds, s.text_color, ScaleUiCoordinate(x + 0xff), y, g_ui_message_buffer);

  strcpy(g_ui_message_buffer, "");
  for (s.i = 0; s.i < 0xc; s.i++)
  {
    if ((town_index != 0) && (g_world_magic_slot_timers[s.i].town_index == town_index))
    {
      s.world_magic_slot_result = FindWorldMagicCardIndex(s.i);
      strcat(g_ui_message_buffer, gs_worldmagic_names_00780660[s.i]);
    }
  }
  DrawFormattedTextShadowedCentered(g_page1_window_bounds, s.text_color, ScaleUiCoordinate(x + 0x19c), y, g_ui_message_buffer);

  return 0;
}
