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

extern FacemakerWindowBounds *PTR_DAT_005832b4;
extern FacemakerWindowBounds *PTR_DAT_005832dc;
extern FacemakerWindowBounds *PTR_DAT_00583304;

extern int g_world_scroll_cache_ready;

// External functions
int *LoadIniEscapedStringTable(FILE *ini_file, char *section_name, int unk1, int unk2);
void LoadPcxIntoPage(int page_number, char *path);
void LoadPcxResource(int page_number, int x, int y, char *path, void *opaque);
void BeginSpriteEncodeSession(void);
EncodedImage *EncodeSpriteFromPage(int page_number, int x, int y, int width, int height);
void FinalizeSpriteEncodeSession(void);

int ScaleUiCoordinate(int value);
int SetFontStyleSize(int font_slot, unsigned int point_size);
void DrawTextAt(FacemakerWindowBounds *dst, int text_id, int x, int y, char *text);
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
extern unsigned char Scards[0xc0];

void FUN_005616cb(int param_1);
void PlaySoundEffectOnChannel(char *sound_path, int channel, int volume, int pitch_percent, int pan_percent);

unsigned int FUN_0043146b(int x, int y);
unsigned int FUN_005611c8(unsigned int tile_mask);
int FUN_004bb458(int world_magic_slot_index);
char *FUN_004f2e17(int town_index);

// From city-info helpers
DWORD __cdecl FUN_00564e70(char *dst, DWORD dst_len, LPCVOID format, ...);
char *__cdecl FUN_00550220(int town_index);
int __cdecl FUN_0050bb6d(FacemakerWindowBounds *dst, int town_index, int x, int y);
void __cdecl FUN_0057b4a0(FacemakerWindowBounds *dst, int text_color, int x, int y, char *format, ...);
void __cdecl FUN_0057b560(FacemakerWindowBounds *window, int color_index, int x, int y, char *format, ...);

// GLOBAL: SHANDALAR 0x00603a34
int DAT_00603a34 = 0;
// GLOBAL: SHANDALAR 0x00603a38
int DAT_00603a38 = 0;
// GLOBAL: SHANDALAR 0x00603a48
int *DAT_00603a48 = (int *)0;

// GLOBAL: SHANDALAR 0x00746e60
EncodedImage *DAT_00746e60[3];
// GLOBAL: SHANDALAR 0x00746e70
EncodedImage *DAT_00746e70[0x10];

// GLOBAL: SHANDALAR 0x007894f4
int DAT_007894f4 = 0;

// GLOBAL: SHANDALAR 0x00652740
char DAT_00652740[0x1000];

// GLOBAL: SHANDALAR 0x00581918
int DAT_00581918[0x10] = {
    0x000000d2, 0x00000000, 0x000000f4, 0x000000ce, 0x000000bc, 0x00000003, 0x00000000, 0x0000001b,
    0x00000002, 0x0000000d, 0x00000003, 0x00000018, 0x00000004, 0x00000001, 0x00000005, 0x0000001d,
};

// GLOBAL: SHANDALAR 0x0058cad4
char DAT_0058cad4[] = " \rDd\x1b";
// GLOBAL: SHANDALAR 0x0058cadc
char DAT_0058cadc[] = " \rDd\x1b";

// GLOBAL: SHANDALAR 0x0058cb98
char s_x_sound_button2_wav_0058cb98[] = "x:sound\\button2.wav";
// GLOBAL: SHANDALAR 0x0058cbac
char s_x_sound_button2_wav_0058cbac[] = "x:sound\\button2.wav";

// GLOBAL: SHANDALAR 0x0058cbc0
char s_cityInfo_0058cbc0[] = "cityInfo";
// GLOBAL: SHANDALAR 0x0058cbcc
char s_infobar_pic_0058cbcc[] = "infobar.pic";
// GLOBAL: SHANDALAR 0x0058cbd8
char s_cityinfo_pic_0058cbd8[] = "cityinfo.pic";
// GLOBAL: SHANDALAR 0x0058cc00
char s_cinfopce_pic_0058cc00[] = "cinfopce.pic";

// GLOBAL: SHANDALAR 0x0058cbe8
char DAT_0058cbe8[] = "%s";
// GLOBAL: SHANDALAR 0x0058cbec
char DAT_0058cbec[] = "%s";
// GLOBAL: SHANDALAR 0x0058cbf0
char DAT_0058cbf0[] = "%s";
// GLOBAL: SHANDALAR 0x0058cbf4
char DAT_0058cbf4[] = "%s";
// GLOBAL: SHANDALAR 0x0058cbf8
char DAT_0058cbf8[] = "%s";
// GLOBAL: SHANDALAR 0x0058cbfc
char DAT_0058cbfc[] = "%s";

// GLOBAL: SHANDALAR 0x0058cc10
char DAT_0058cc10[] = "%d";
// GLOBAL: SHANDALAR 0x0058cc14
char DAT_0058cc14[] = "\n";
// GLOBAL: SHANDALAR 0x0058cc18
char DAT_0058cc18[] = "X";
// GLOBAL: SHANDALAR 0x0058cc1c
char DAT_0058cc1c[] = "";

// Forward decls
int __cdecl FUN_0050a709(AdvMenuControl *control, int mode);
int __cdecl FUN_0050a828(AdvMenuControl *control);
int __cdecl FUN_0050a85a(AdvMenuControl *control, int mode);
int __cdecl FUN_0050a970(AdvMenuControl *control);

// GLOBAL: SHANDALAR 0x0058c8f8
AdvMenuControl DAT_0058c8f8[5] = {
    // PageUp
    {27, 171, 17, 58, 27, 171, 17, 58, 1, (AdvMenuRenderCallback)FUN_0050a709, (AdvMenuActivateCallback)FUN_0050a828, -2, 0, (char *)0, (char *)0, 0x4900, 0, {0, 0, 0, 0}},
    // Up
    {27, 230, 17, 58, 27, 230, 17, 58, 1, (AdvMenuRenderCallback)FUN_0050a709, (AdvMenuActivateCallback)FUN_0050a828, -1, 1, (char *)0, (char *)0, 0x4800, 0, {0, 0, 0, 0}},
    // Down
    {27, 289, 17, 58, 27, 289, 17, 58, 1, (AdvMenuRenderCallback)FUN_0050a709, (AdvMenuActivateCallback)FUN_0050a828, 1, 2, (char *)0, (char *)0, 0x5000, 0, {0, 0, 0, 0}},
    // PageDown
    {27, 348, 17, 58, 27, 348, 17, 58, 1, (AdvMenuRenderCallback)FUN_0050a709, (AdvMenuActivateCallback)FUN_0050a828, 2, 3, (char *)0, (char *)0, 0x5100, 0, {0, 0, 0, 0}},
    // Done
    {544, 28, 58, 26, 544, 28, 58, 26, 1, (AdvMenuRenderCallback)FUN_0050a85a, (AdvMenuActivateCallback)FUN_0050a970, 0, 0, DAT_0058cad4, DAT_0058cadc, 0, 0, {0, 0, 0, 0}},
};

// FUNCTION: SHANDALAR 0x004ffcb4
int FUN_004ffcb4(AdvMenuControl *control)
{
  int old_state;

  old_state = control->state;
  g_menu_render_guard = 1;
  control->state = 0;
  control->on_render(control, 3);
  g_menu_render_guard = 0;
  control->state = 3;
  return old_state;
}

// FUNCTION: SHANDALAR 0x004ffd0a
int FUN_004ffd0a(AdvMenuControl *control)
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
int __cdecl FUN_00500129(int allow_arrow_nav)
{
  g_menu_allow_arrow_nav_by_context[g_menu_context_index] = allow_arrow_nav;
  return 1;
}

// FUNCTION: SHANDALAR 0x005001e3
int FUN_005001e3(void)
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
int __cdecl FUN_0050a709(AdvMenuControl *control, int mode)
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

  DrawEncodedImageResampled(PTR_DAT_005832b4, control->x, control->y, control->width, control->height,
                            DAT_00746e70[control->unk_30 * 4 + mode]);

  if ((mode == 2) && (control->on_activate != (AdvMenuActivateCallback)0))
  {
    control->on_activate(control);
  }

  return 1;
}

// FUNCTION: SHANDALAR 0x0050a828
int __cdecl FUN_0050a828(AdvMenuControl *control)
{
  PlaySoundEffectOnChannel(s_x_sound_button2_wav_0058cb98, 0xf, 100, 100, 0);
  DAT_00603a34 = control->selection_value;
  return 0;
}

// FUNCTION: SHANDALAR 0x0050a85a
int __cdecl FUN_0050a85a(AdvMenuControl *control, int mode)
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

  DrawEncodedImageResampled(PTR_DAT_005832b4, control->x, control->y, control->width, control->height, DAT_00746e60[mode]);

  if ((mode == 2) && (control->on_activate != (AdvMenuActivateCallback)0))
  {
    control->on_activate(control);
  }

  return 1;
}

// FUNCTION: SHANDALAR 0x0050a970
int __cdecl FUN_0050a970(AdvMenuControl *control)
{
  PlaySoundEffectOnChannel(s_x_sound_button2_wav_0058cbac, 0xf, 100, 100, 0);
  DAT_00603a34 = control->selection_value;
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
      FUN_0050bb6d(PTR_DAT_005832dc, town_indices_1based[list_index + 1], 0x30, y0 + i * row_height_scaled);
    }
  }
}

// FUNCTION: SHANDALAR 0x0050a9a2
void ShowCityInfoScreen(int param_1)
{
  struct
  {
    int scroll_top_index;
    int town_count;
    int town_indices_1based[0x81];
    int menu_context;

    int list_panel_x;
    int list_top_y;
    int list_width_scaled;
    int row_height_scaled;
  } s;

  int i;

  (void)param_1;

  s.scroll_top_index = 0;
  s.town_count = 0;

  if (DAT_00603a38 == 0)
  {
    DAT_00603a48 = LoadIniEscapedStringTable(g_advbuttons_ini_file, s_cityInfo_0058cbc0, (int)g_ini_string_scratch, 0);
    DAT_00603a38 = 1;
  }

  LoadPcxIntoPage(1, s_infobar_pic_0058cbcc);

  BeginSpriteEncodeSession();

  // Scale/encode the 4 scroll buttons
  for (i = 0; i < 4; i++)
  {
    int j;
    if (DAT_0058c8f8[i].x == DAT_0058c8f8[i].base_x)
    {
      DAT_0058c8f8[i].x = ScaleUiCoordinate(DAT_0058c8f8[i].x);
      DAT_0058c8f8[i].y = ScaleUiCoordinate(DAT_0058c8f8[i].y);
      DAT_0058c8f8[i].width = ScaleUiCoordinate(DAT_0058c8f8[i].width);
      DAT_0058c8f8[i].height = ScaleUiCoordinate(DAT_0058c8f8[i].height);
    }

    for (j = 0; j < 4; j++)
    {
      DAT_00746e70[i * 4 + j] = EncodeSpriteFromPage(1, j * 0x12 + 0x2b, i * 0x31 + 0x1c, 0x11, 0x2f);
    }
  }

  // Scale the Done button
  if (DAT_0058c8f8[4].x == DAT_0058c8f8[4].base_x)
  {
    DAT_0058c8f8[4].x = ScaleUiCoordinate(DAT_0058c8f8[4].x);
    DAT_0058c8f8[4].y = ScaleUiCoordinate(DAT_0058c8f8[4].y);
    DAT_0058c8f8[4].width = ScaleUiCoordinate(DAT_0058c8f8[4].width);
    DAT_0058c8f8[4].height = ScaleUiCoordinate(DAT_0058c8f8[4].height);
  }

  // Build 3-state Done button sprites into DAT_00746e60
  PTR_DAT_005832dc->font_slot = 7;
  SetFontStyleSize(7, (unsigned int)ScaleUiCoordinate(10));
  for (i = 0; i < 3; i++)
  {
    int x_table[3];
    x_table[0] = 0x48;
    x_table[1] = 0x3f;
    x_table[2] = 0x40;

    SetFontStyleSize(7, (unsigned int)(10 - i / 2));
    FUN_0057b4a0(PTR_DAT_005832dc, x_table[i], i * 0x3c + 0x49, 0xe, (char *)g_done_text_table_entry);
  }

  for (i = 0; i < 3; i++)
  {
    DAT_00746e60[i] = EncodeSpriteFromPage(1, i * 0x3c + 0x2b, 1, 0x3a, 0x18);
  }

  FinalizeSpriteEncodeSession();

  s.menu_context = BeginMenuContext();
  ResetMenuContext(s.menu_context);
  AddMenuControlsToContext(DAT_0058c8f8, 5, s.menu_context);
  FUN_00500129(0);

  // Prepare background
  g_world_scroll_cache_ready = 1;
  LoadPcxResource(1, 0, global_screen_height - 0x1e0, s_cityinfo_pic_0058cbd8, (void *)0);
  StretchBlitGraphicsRect(PTR_DAT_005832dc, 0, global_screen_height - 0x1e0, 0x280, 0x1e0, PTR_DAT_005832dc, 0, 0, global_screen_width,
                          global_screen_height);

  // Headings, loaded from advButtons [cityInfo] (format strings in .rdata are \"%s\")
  PTR_DAT_005832dc->font_slot = 7;
  SetFontStyleSize(7, (unsigned int)ScaleUiCoordinate(10));
  if (DAT_00603a48 != (int *)0)
  {
    ((void(__cdecl *)(FacemakerWindowBounds *, int, int, int, char *, ...))DrawTextAt)(PTR_DAT_005832dc, 200, 0x69, 0x28, DAT_0058cbe8,
                                                                                       (char *)DAT_00603a48[0]);
    ((void(__cdecl *)(FacemakerWindowBounds *, int, int, int, char *, ...))DrawTextAt)(PTR_DAT_005832dc, 200, 0x54, 0x45, DAT_0058cbec,
                                                                                       (char *)DAT_00603a48[1]);
    ((void(__cdecl *)(FacemakerWindowBounds *, int, int, int, char *, ...))DrawTextAt)(PTR_DAT_005832dc, 200, 0xb3, 0x45, DAT_0058cbf0,
                                                                                       (char *)DAT_00603a48[2]);
    ((void(__cdecl *)(FacemakerWindowBounds *, int, int, int, char *, ...))DrawTextAt)(PTR_DAT_005832dc, 200, 0x130, 0x45, DAT_0058cbf4,
                                                                                       (char *)DAT_00603a48[3]);
    ((void(__cdecl *)(FacemakerWindowBounds *, int, int, int, char *, ...))DrawTextAt)(PTR_DAT_005832dc, 200, 0x1cc, 0x45, DAT_0058cbf8,
                                                                                       (char *)DAT_00603a48[4]);
    ((void(__cdecl *)(FacemakerWindowBounds *, int, int, int, char *, ...))DrawTextAt)(PTR_DAT_005832dc, 200, 0x23f, 0x45, DAT_0058cbfc,
                                                                                       (char *)DAT_00603a48[5]);
  }

  FUN_005616cb(1);

  // Reset font slots
  PTR_DAT_005832b4->font_slot = 1;
  PTR_DAT_005832dc->font_slot = 1;
  PTR_DAT_00583304->font_slot = 1;

  LoadPcxResource(2, 0, PTR_DAT_00583304->max_y - 0x46, s_cinfopce_pic_0058cc00, (void *)0);

  // List panel geometry
  s.list_panel_x = (global_screen_width / 2 + global_screen_width * 0x30) / 0x280;
  s.list_top_y = ScaleUiCoordinate(0x52);
  s.list_width_scaled = ScaleUiCoordinate(0x231);
  s.row_height_scaled = ScaleUiCoordinate(0x2a);

  // Build repeating row background on the offscreen page
  StretchBlitGraphicsRect(PTR_DAT_00583304, 0, PTR_DAT_00583304->max_y - 0x2c, 0x231, 0x2a, PTR_DAT_00583304, 0, 0x80, s.list_width_scaled,
                          s.row_height_scaled);
  for (i = 0; i < 9; i++)
  {
    CopyGraphicsRect(PTR_DAT_00583304, 0, 0x80, s.list_width_scaled, s.row_height_scaled, PTR_DAT_005832dc, s.list_panel_x,
                     i * s.row_height_scaled + s.list_top_y);
  }

  CopyGraphicsRect(PTR_DAT_005832dc, 0, 0, global_screen_width, global_screen_height, PTR_DAT_005832b4, 0, 0);

  // Build town list (1-based indexing like the original)
  for (i = 0; i < 0x80; i++)
  {
    if ((((g_town_slots[i].status_and_ruling_wizard & 2) != 0) || (DAT_007894f4 != 0)) && (g_town_slots[i].location_type != 1))
    {
      s.town_indices_1based[s.town_count + 1] = i;
      s.town_count = s.town_count + 1;
    }
  }

  // Main loop
  for (;;)
  {
    // Rebuild row backgrounds
    for (i = 0; i < 9; i++)
    {
      CopyGraphicsRect(PTR_DAT_00583304, 0, 0x80, s.list_width_scaled, s.row_height_scaled, PTR_DAT_005832dc, s.list_panel_x,
                       i * s.row_height_scaled + s.list_top_y);
    }

    CityInfo_DrawVisibleRows(s.town_indices_1based, s.town_count, s.scroll_top_index, s.list_top_y, s.row_height_scaled);

    // Copy list area to the visible page
    CopyGraphicsRect(PTR_DAT_005832dc, s.list_panel_x, s.list_top_y, s.list_width_scaled, s.row_height_scaled * 9, PTR_DAT_005832b4, s.list_panel_x,
                     s.list_top_y);

    // Enable/disable scroll buttons
    if (s.scroll_top_index == 0)
    {
      FUN_004ffcb4(&DAT_0058c8f8[0]);
      FUN_004ffcb4(&DAT_0058c8f8[1]);
    }
    else
    {
      FUN_004ffd0a(&DAT_0058c8f8[0]);
      FUN_004ffd0a(&DAT_0058c8f8[1]);
    }

    if (s.town_count - 9 <= s.scroll_top_index)
    {
      FUN_004ffcb4(&DAT_0058c8f8[2]);
      FUN_004ffcb4(&DAT_0058c8f8[3]);
    }
    else
    {
      FUN_004ffd0a(&DAT_0058c8f8[2]);
      FUN_004ffd0a(&DAT_0058c8f8[3]);
    }

    FUN_005001e3();

    DAT_00603a34 = -5;
    for (;;)
    {
      UpdateMouseSnapshot();
      UpdateMenuControlSelection(g_mouse_x_snapshot, g_mouse_y_snapshot, g_mouse_button_down_mask);
      if (DAT_00603a34 != -5)
      {
        break;
      }
      if (HasQueuedKeyInput() != 0)
      {
        break;
      }
    }

    if (DAT_00603a34 == 0)
    {
      EndMenuContext();
      if (DAT_00746e70[0] != (EncodedImage *)0)
      {
        FreeSpriteBlob(DAT_00746e70[0]);
      }
      return;
    }

    if (s.town_count < 10)
    {
      EndMenuContext();
      if (DAT_00746e70[0] != (EncodedImage *)0)
      {
        FreeSpriteBlob(DAT_00746e70[0]);
      }
      return;
    }

    switch (DAT_00603a34)
    {
    case -2: // PageUp
      s.scroll_top_index = s.scroll_top_index - 8;
      if (s.scroll_top_index < 0)
      {
        s.scroll_top_index = 0;
      }
      break;
    case -1: // Up
      s.scroll_top_index = s.scroll_top_index - 1;
      if (s.scroll_top_index < 0)
      {
        s.scroll_top_index = 0;
      }
      break;
    case 1: // Down
      s.scroll_top_index = s.scroll_top_index + 1;
      if ((s.town_count - 9) < s.scroll_top_index)
      {
        s.scroll_top_index = s.town_count - 9;
      }
      break;
    case 2: // PageDown
      s.scroll_top_index = s.scroll_top_index + 8;
      if ((s.town_count - 9) < s.scroll_top_index)
      {
        s.scroll_top_index = s.town_count - 9;
      }
      break;
    default:
      break;
    }
  }
}

// FUNCTION: SHANDALAR 0x00550220
char *__cdecl FUN_00550220(int town_index)
{
  int first_half_index;
  int second_half_index;

  // town_index is non-negative in practice; this mirrors the original's clamping
  first_half_index = town_index / 8;
  if (first_half_index < 0)
  {
    first_half_index = 0;
  }
  else if (first_half_index > 0xf)
  {
    first_half_index = 0xf;
  }

  second_half_index = town_index & 0xf;
  if (second_half_index < 0)
  {
    second_half_index = 0;
  }
  else if (second_half_index > 0xf)
  {
    second_half_index = 0xf;
  }

  if (g_town_slots[town_index].location_type == 1)
  {
    FUN_00564e70(DAT_00652740, 0x1000, gs_cityname_format_right_0077f190, gs_citynames_firsthalf_0077e060[first_half_index]);
  }
  else
  {
    FUN_00564e70(DAT_00652740, 0x1000, gs_cityname_format_left_0074c950, gs_citynames_firsthalf_0077e060[first_half_index],
                 gs_citynames_secondhalf_007653e0[second_half_index]);
  }

  return DAT_00652740;
}

// FUNCTION: SHANDALAR 0x0050bb6d
int __cdecl FUN_0050bb6d(FacemakerWindowBounds *dst, int town_index, int x, int y)
{
  size_t len;
  char *mid;
  char *space_fwd;
  char *space_back;
  unsigned int tile_mask;
  unsigned int tile_class;
  int text_color;
  int i;
  int wizard_count;
  int wizard_sprite_x;
  int sprite_width;
  int sprite_height;
  EncodedImage *sprite;
  int sprite_index_by_wizard_bit[6];

  (void)dst;

  // Map wizard-bit positions 1..5 to avatar sprite indices 0..4
  sprite_index_by_wizard_bit[0] = 0;
  sprite_index_by_wizard_bit[1] = 2;
  sprite_index_by_wizard_bit[2] = 1;
  sprite_index_by_wizard_bit[3] = 4;
  sprite_index_by_wizard_bit[4] = 3;
  sprite_index_by_wizard_bit[5] = 0;

  if (g_town_slots[town_index].location_type == 4)
  {
    strcpy(g_ui_message_buffer, gs_cityname_castle_00765dc0);
  }
  else
  {
    strcpy(g_ui_message_buffer, FUN_00550220(town_index));
  }

  len = strlen(g_ui_message_buffer);
  mid = g_ui_message_buffer + (len >> 1);

  for (space_fwd = mid; (*space_fwd != '\0') && (*space_fwd != ' '); space_fwd++)
  {
  }
  if (*space_fwd != ' ')
  {
    space_fwd = (char *)0;
  }

  for (space_back = mid; (space_back > g_ui_message_buffer) && (*space_back != ' '); space_back--)
  {
  }
  if ((space_back <= g_ui_message_buffer) || (*space_back != ' '))
  {
    space_back = (char *)0;
  }

  if ((space_fwd == (char *)0) || ((space_back != (char *)0) && ((mid - space_fwd) < (space_back - mid))))
  {
    if ((space_back != (char *)0) && ((space_fwd == (char *)0) || ((mid - space_fwd) <= (space_back - mid))))
    {
      *space_back = '\n';
    }
  }
  else
  {
    *space_fwd = '\n';
  }

  strcat(g_ui_message_buffer, DAT_0058cc14);

  tile_mask = FUN_0043146b(g_town_slots[town_index].world_x, g_town_slots[town_index].world_y);
  FUN_005611c8(tile_mask);

  if ((g_town_slots[town_index].status_and_ruling_wizard & 1) == 0)
  {
    text_color = 0xe3;
  }
  else
  {
    text_color = 0xff;
  }

  if (((g_town_slots[town_index].status_and_ruling_wizard & 0xffffff00U) >> 8) != 0)
  {
    text_color = DAT_00581918[(unsigned char)(g_town_slots[town_index].status_and_ruling_wizard >> 8)];
  }

  FUN_0057b560(PTR_DAT_005832dc, text_color, ScaleUiCoordinate(x + 0x2a), y, g_ui_message_buffer);
  if ((g_town_slots[town_index].status_and_ruling_wizard & 1) != 0)
  {
    FUN_0057b560(PTR_DAT_005832dc, text_color, ScaleUiCoordinate(x + 0x20c), y, g_ui_message_buffer);
  }

  tile_mask = FUN_0043146b(g_town_slots[town_index].world_x, g_town_slots[town_index].world_y);
  tile_class = FUN_005611c8(tile_mask);

  wizard_count = 0;
  for (i = 1; i < 6; i++)
  {
    if ((tile_class & (1U << ((unsigned char)i & 0x1f))) != 0)
    {
      wizard_count++;
    }
  }

  sprite_width = (int)g_world_magic_avatar_sprites[0]->width;
  sprite_height = (int)g_world_magic_avatar_sprites[0]->height;
  wizard_sprite_x = ScaleUiCoordinate(((x + 0x80) - (sprite_width * wizard_count) / 2) - (wizard_count * 5 - 5));

  for (i = 1; i < 6; i++)
  {
    if ((tile_class & (1U << ((unsigned char)i & 0x1f))) != 0)
    {
      sprite = g_world_magic_avatar_sprites[sprite_index_by_wizard_bit[i]];
      DrawEncodedImageResampled(PTR_DAT_005832dc, wizard_sprite_x, y - ScaleUiCoordinate(sprite_height / 2), ScaleUiCoordinate(sprite_width),
                                ScaleUiCoordinate(sprite_height), sprite);
      wizard_sprite_x += ScaleUiCoordinate(sprite_width + 5);
    }
  }

  strcpy(g_ui_message_buffer, FUN_004f2e17(town_index));
  FUN_0057b560(PTR_DAT_005832dc, text_color, ScaleUiCoordinate(x + 0xff), y, g_ui_message_buffer);

  strcpy(g_ui_message_buffer, DAT_0058cc1c);
  for (i = 0; i < 0xc; i++)
  {
    if ((town_index != 0) && (*(int *)(Scards + i * 0x10 + 8) == town_index))
    {
      FUN_004bb458(i);
      strcat(g_ui_message_buffer, gs_worldmagic_names_00780660[i]);
    }
  }
  FUN_0057b560(PTR_DAT_005832dc, text_color, ScaleUiCoordinate(x + 0x19c), y, g_ui_message_buffer);

  return 0;
}

// FUNCTION: SHANDALAR 0x0057b4a0
void __cdecl FUN_0057b4a0(FacemakerWindowBounds *dst, int text_color, int x, int y, char *format, ...)
{
  DrawTextFormatted(dst, text_color, 0, 0, 1, 1, x, y, (int *)&format);
}
