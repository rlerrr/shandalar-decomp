#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "defs.h"
#include "shandalar.h"
#include "magic/src/global_state.h"
#include "shandalar_global_strings.h"
#include "deckdll/src/card_db.h"
#include "facemaker/src/facemaker_types.h"
#include "cardartlib/src/palette.h"
#include "drawcardlib/src/pic.h"

typedef unsigned char byte;

typedef struct
{
  int progress_by_color[5];
  int duel_wins_by_color[5];
  unsigned char town_count_by_color[5];
  unsigned char highlighted_wizard_color;
  unsigned char highlighted_creature_type;
  unsigned char highlighted_lair_color;
} StatWinData;

typedef struct
{
  int wins;
  int losses;
} StatsCreatureJournalCount;

typedef union
{
  struct
  {
    EncodedImage *normal[12];
    EncodedImage *highlight[12];
    EncodedImage *pressed[12];
    EncodedImage *icon[12];
  } named;
  EncodedImage *by_group[4][12];
} WorldMagicChoiceButtonSpriteBank;

typedef struct
{
  EncodedImage *normal[4];
  EncodedImage *highlight[4];
  EncodedImage *pressed[4];
} StatsMenuButtonSpriteSet;

int RenderStatsTabButtonControl(AdvMenuControl *control, int mode);
int RenderStatsWorldMagicControl(AdvMenuControl *control, int mode);
static int ActivateStatsMenuControl(AdvMenuControl *control);
static int RenderStatsScrollbarHotspot(AdvMenuControl *control, int mode);
int RenderStatsCreatureScrollbar(AdvMenuControl *control);
static int DragStatsCreatureScrollbar(AdvMenuControl *control);
static int RenderStatsColorHotspot(AdvMenuControl *control, int mode);
static int RenderJournalExitButtonControl(AdvMenuControl *control, int mode);
static int ActivateJournalExitButtonControl(AdvMenuControl *control);
static int RenderJournalStepButtonControl(AdvMenuControl *control, int mode);
static int ActivateJournalStepButtonControl(AdvMenuControl *control);
static int RenderJournalScrollbar(AdvMenuControl *control);
static int SelectStatsJournalEntry(int journal_index);
static void BuildStatsJournalEntryMessage(int entry_type, unsigned int entry_arg, int tile_x, int tile_y);
static void AnimateStatsJournalMarkerToTile(unsigned int tile_x, unsigned int tile_y);
char *BuildTownDisplayName(int town_index);
char *BuildCreatureNameWithArticle(int creature_type);
char *__cdecl GetDungeonName(int dungeon_index);
DWORD FormatMessageFromStringStripCarriageReturns(char *dst, DWORD max_length, LPCVOID format, ...);
char *__cdecl FUN_005307c3(int param_1);
int FindNearestTownIndex(int world_x, int world_y);

extern int global_screen_width;
extern int global_screen_height;
extern int g_default_palette_fade_steps;
extern int g_statwin_exports_by_ordinal[3];
extern int g_world_magic_town_flags[5];
extern char g_ui_message_buffer[0x1000];
extern HWND g_main_window_hwnd;
extern HANDLE g_main_thread_handle;
extern HANDLE g_timer_thread_handle;
extern HPALETTE g_palette_handle;
extern DIBSurface *g_graphics_pages[10];
extern FacemakerWindowBounds *PTR_DAT_005832b4;
extern FacemakerWindowBounds *PTR_DAT_005832dc;
extern FacemakerWindowBounds *PTR_DAT_00583304;
extern int _DAT_007483e4;
extern card_data_t global_cards_data[];
extern char DAT_0093a870[0x20];
extern int g_menu_render_guard;
extern int g_mouse_x;
extern int g_mouse_y;
extern int g_reveal_all_world_info;
extern WorldMagicChoiceButtonSpriteBank g_world_magic_choice_button_sprite_bank;
extern int g_graphics_bpp;
extern RpBitsPalettePacket g_palette_data_words;
extern DIBSurface *g_facemaker_page4_dib;
extern HBITMAP g_facemaker_page4_bitmap;
extern int g_mouse_x_snapshot;
extern int g_mouse_y_snapshot;
extern int g_mouse_button_down_mask;
extern int Gold;
extern FacemakerWindowBounds *g_menu_control_draw_target_page;
extern EncodedImage *DAT_0073ea94;
extern EncodedImage *DAT_00746b04;
extern EncodedImage *DAT_0073eab0[5][3];
extern int DAT_00789938;
extern int DAT_0078df68;

// GLOBAL: SHANDALAR 0x00590618
static char s_statText_00590618[] = "statText";
// GLOBAL: SHANDALAR 0x005905f8
static char s_stats_card_tab_navigate_hotkeys_005905f8[4] = {'D', 'd', '\x1b', ' '};
// GLOBAL: SHANDALAR 0x00590600
static char s_stats_card_tab_activate_hotkeys_00590600[4] = {'\x1b', ' ', '\0', '\0'};

// GLOBAL: SHANDALAR 0x005905c8
static int g_stats_portrait_frame_colors[5][2] = {
    {4, 7},
    {245, 243},
    {207, 205},
    {190, 188},
    {127, 2}};

// GLOBAL: SHANDALAR 0x00650264
int DAT_00650264;
// GLOBAL: SHANDALAR 0x0073ea30
static StatWinData g_statwin_data;
// GLOBAL: SHANDALAR 0x00650630
static EncodedImage *g_stats_menu_color_sprites[5];
// GLOBAL: SHANDALAR 0x00650698
static StatsMenuButtonSpriteSet g_stats_menu_button_sprites;
// GLOBAL: SHANDALAR 0x006506d0
static EncodedImage *g_stats_menu_scrollbar_sprite;
// GLOBAL: SHANDALAR 0x006506d4
static EncodedImage *g_stats_menu_scroll_thumb_sprite;
// GLOBAL: SHANDALAR 0x00650650
static int g_stats_world_magic_unlocked[12];
// GLOBAL: SHANDALAR 0x00650458
static int g_stats_journal_world_magic_counts[6];
// GLOBAL: SHANDALAR 0x00650470
static StatsCreatureJournalCount g_stats_creature_journal_counts[0x38];
// GLOBAL: SHANDALAR 0x00650684
static int g_stats_journal_completed_quests;
// GLOBAL: SHANDALAR 0x00650644
static int g_stats_journal_failed_quests;
// GLOBAL: SHANDALAR 0x00650680
static int g_stats_journal_cities_saved;
// GLOBAL: SHANDALAR 0x00650690
static int g_stats_journal_lairs_explored;
// GLOBAL: SHANDALAR 0x006506cc
static int g_stats_journal_dungeons_explored;
// GLOBAL: SHANDALAR 0x00650688
static int g_stats_journal_spells_found;
// GLOBAL: SHANDALAR 0x0065068c
static int g_stats_journal_artifacts_found;
// GLOBAL: SHANDALAR 0x0058fe70
static int g_stats_creature_sheet_resolution = 1;
// GLOBAL: SHANDALAR 0x006506d8
static int g_stats_creature_page_index;
// GLOBAL: SHANDALAR 0x00650648
static int g_stats_menu_selection;
// GLOBAL: SHANDALAR 0x0065064c
static int *g_stats_text_table;
// GLOBAL: SHANDALAR 0x006506c8
static int g_stats_text_loaded;

// GLOBAL: SHANDALAR 0x0058fe78
static int g_stats_creature_sheet_cell_dims[3][2] = {
    {80, 64},
    {100, 80},
    {128, 102},
};

// GLOBAL: SHANDALAR 0x0058fe90
static AdvMenuControl g_stats_menu_controls[0x16] = {
    {0x193, 7, 0x54, 0x17, 0x193, 7, 0x54, 0x17, 1, (AdvMenuRenderCallback)RenderStatsTabButtonControl, (AdvMenuActivateCallback)ActivateStatsMenuControl, 1, 0, "Ww", (char *)0, 0, 0, {0, 0, 0, 0}},
    {0x1ec, 7, 0x48, 0x17, 0x1ec, 7, 0x48, 0x17, 1, (AdvMenuRenderCallback)RenderStatsTabButtonControl, (AdvMenuActivateCallback)ActivateStatsMenuControl, 2, 0, "Jj", (char *)0, 0, 0, {0, 0, 0, 0}},
    {0x23c, 7, 0x31, 0x17, 0x23c, 7, 0x31, 0x17, 1, (AdvMenuRenderCallback)RenderStatsTabButtonControl, (AdvMenuActivateCallback)ActivateStatsMenuControl, 3, 1, s_stats_card_tab_navigate_hotkeys_005905f8, s_stats_card_tab_activate_hotkeys_00590600, 0, 0, {0, 0, 0, 0}},
    {0xe3, 0x2b, 0xe, 0x96, 0xe3, 0x2b, 0xe, 0x96, 1, (AdvMenuRenderCallback)RenderStatsScrollbarHotspot, (AdvMenuActivateCallback)DragStatsCreatureScrollbar, 4, 0, (char *)0, (char *)0, 0, 0, {0, 0, 0, 0}},
    {0xe3, 0x2b, 0xe, 0x12, 0xe3, 0x2b, 0xe, 0x12, 1, (AdvMenuRenderCallback)RenderStatsScrollbarHotspot, (AdvMenuActivateCallback)DragStatsCreatureScrollbar, 5, 0, (char *)0, (char *)0, 0, 0, {0, 0, 0, 0}},
    {0x14d, 0x154, 0x30, 0x30, 0x14d, 0x154, 0x30, 0x30, 1, (AdvMenuRenderCallback)RenderStatsColorHotspot, (AdvMenuActivateCallback)0, 10, 0, (char *)0, (char *)0, 0, 0, {0, 0, 0, 0}},
    {0x186, 0x154, 0x30, 0x30, 0x186, 0x154, 0x30, 0x30, 1, (AdvMenuRenderCallback)RenderStatsColorHotspot, (AdvMenuActivateCallback)0, 11, 0, (char *)0, (char *)0, 0, 0, {0, 0, 0, 0}},
    {0x1bf, 0x154, 0x30, 0x30, 0x1bf, 0x154, 0x30, 0x30, 1, (AdvMenuRenderCallback)RenderStatsColorHotspot, (AdvMenuActivateCallback)0, 12, 0, (char *)0, (char *)0, 0, 0, {0, 0, 0, 0}},
    {0x1f8, 0x154, 0x30, 0x30, 0x1f8, 0x154, 0x30, 0x30, 1, (AdvMenuRenderCallback)RenderStatsColorHotspot, (AdvMenuActivateCallback)0, 13, 0, (char *)0, (char *)0, 0, 0, {0, 0, 0, 0}},
    {0x231, 0x154, 0x30, 0x30, 0x231, 0x154, 0x30, 0x30, 1, (AdvMenuRenderCallback)RenderStatsColorHotspot, (AdvMenuActivateCallback)0, 14, 0, (char *)0, (char *)0, 0, 0, {0, 0, 0, 0}},
    {0x14d, 0x154, 0x30, 0x30, 0x14d, 0x154, 0x30, 0x30, 1, (AdvMenuRenderCallback)RenderStatsWorldMagicControl, (AdvMenuActivateCallback)ActivateStatsMenuControl, 15, 0, (char *)0, (char *)0, 0, 0, {0, 0, 0, 0}},
    {0x186, 0x154, 0x30, 0x30, 0x186, 0x154, 0x30, 0x30, 1, (AdvMenuRenderCallback)RenderStatsWorldMagicControl, (AdvMenuActivateCallback)ActivateStatsMenuControl, 16, 0, (char *)0, (char *)0, 0, 0, {0, 0, 0, 0}},
    {0x1bf, 0x154, 0x30, 0x30, 0x1bf, 0x154, 0x30, 0x30, 1, (AdvMenuRenderCallback)RenderStatsWorldMagicControl, (AdvMenuActivateCallback)ActivateStatsMenuControl, 17, 0, (char *)0, (char *)0, 0, 0, {0, 0, 0, 0}},
    {0x1f8, 0x154, 0x30, 0x30, 0x1f8, 0x154, 0x30, 0x30, 1, (AdvMenuRenderCallback)RenderStatsWorldMagicControl, (AdvMenuActivateCallback)ActivateStatsMenuControl, 18, 0, (char *)0, (char *)0, 0, 0, {0, 0, 0, 0}},
    {0x231, 0x154, 0x30, 0x30, 0x231, 0x154, 0x30, 0x30, 1, (AdvMenuRenderCallback)RenderStatsWorldMagicControl, (AdvMenuActivateCallback)ActivateStatsMenuControl, 19, 0, (char *)0, (char *)0, 0, 0, {0, 0, 0, 0}},
    {0x14d, 0x154, 0x30, 0x30, 0x14d, 0x154, 0x30, 0x30, 1, (AdvMenuRenderCallback)RenderStatsWorldMagicControl, (AdvMenuActivateCallback)ActivateStatsMenuControl, 20, 0, (char *)0, (char *)0, 0, 0, {0, 0, 0, 0}},
    {0x186, 0x154, 0x30, 0x30, 0x186, 0x154, 0x30, 0x30, 1, (AdvMenuRenderCallback)RenderStatsWorldMagicControl, (AdvMenuActivateCallback)ActivateStatsMenuControl, 21, 0, (char *)0, (char *)0, 0, 0, {0, 0, 0, 0}},
    {0x1bf, 0x154, 0x30, 0x30, 0x1bf, 0x154, 0x30, 0x30, 1, (AdvMenuRenderCallback)RenderStatsWorldMagicControl, (AdvMenuActivateCallback)ActivateStatsMenuControl, 22, 0, (char *)0, (char *)0, 0, 0, {0, 0, 0, 0}},
    {0x1f8, 0x154, 0x30, 0x30, 0x1f8, 0x154, 0x30, 0x30, 1, (AdvMenuRenderCallback)RenderStatsWorldMagicControl, (AdvMenuActivateCallback)ActivateStatsMenuControl, 23, 0, (char *)0, (char *)0, 0, 0, {0, 0, 0, 0}},
    {0x231, 0x154, 0x30, 0x30, 0x231, 0x154, 0x30, 0x30, 1, (AdvMenuRenderCallback)RenderStatsWorldMagicControl, (AdvMenuActivateCallback)ActivateStatsMenuControl, 24, 0, (char *)0, (char *)0, 0, 0, {0, 0, 0, 0}},
    {0x14d, 0x154, 0x30, 0x30, 0x14d, 0x154, 0x30, 0x30, 1, (AdvMenuRenderCallback)RenderStatsWorldMagicControl, (AdvMenuActivateCallback)ActivateStatsMenuControl, 25, 0, (char *)0, (char *)0, 0, 0, {0, 0, 0, 0}},
    {0x186, 0x154, 0x30, 0x30, 0x186, 0x154, 0x30, 0x30, 1, (AdvMenuRenderCallback)RenderStatsWorldMagicControl, (AdvMenuActivateCallback)ActivateStatsMenuControl, 26, 0, (char *)0, (char *)0, 0, 0, {0, 0, 0, 0}},
};

// GLOBAL: SHANDALAR 0x00581b00
static AdvMenuControl g_stats_journal_menu_controls[3] = {
    {0x1f3, 5, 0x5e, 0x1c, 0x1f3, 5, 0x5e, 0x1c, 1, (AdvMenuRenderCallback)RenderJournalExitButtonControl, (AdvMenuActivateCallback)ActivateJournalExitButtonControl, 4, 0, " \rDd", " \rDd", 0, 0, {0, 0, 0, 0}},
    {1, 0x6e, 0x13, 0x117, 1, 0x6e, 0x13, 0x117, 1, (AdvMenuRenderCallback)RenderJournalStepButtonControl, (AdvMenuActivateCallback)ActivateJournalStepButtonControl, 1, 0, " \rDd", " \rDd", 0, 0, {0, 0, 0, 0}},
    {2, 0x6f, 0xf, 0x46, 2, 0x6f, 0xf, 0x46, 1, (AdvMenuRenderCallback)RenderJournalStepButtonControl, (AdvMenuActivateCallback)ActivateJournalStepButtonControl, 2, 0, " \rDd", " \rDd", 0, 0, {0, 0, 0, 0}},
};

// GLOBAL: SHANDALAR 0x005a82d0
static int g_stats_journal_marker_x;
// GLOBAL: SHANDALAR 0x005a82d4
static int g_stats_journal_marker_y;
// GLOBAL: SHANDALAR 0x005a82d8
static int g_stats_journal_menu_selection;
// GLOBAL: SHANDALAR 0x005a82dc
static int g_stats_journal_current_entry;
// GLOBAL: SHANDALAR 0x005a82e0
static int g_stats_journal_entry_count;
// GLOBAL: SHANDALAR 0x005a82e4
static int g_stats_journal_previous_marker_y;
// GLOBAL: SHANDALAR 0x005a82e8
static int g_stats_journal_previous_marker_x;

void AnimatePaletteToColor(int color_index, int steps);
void ClearGraphicsPageWithPaletteColor(int page_number, int color_index);
void LoadPcxIntoPage(int page_number, char *path);
void LoadPcxIntoPageNoPalette(char *path);
int *LoadIniEscapedStringTable(FILE *ini_file, char *section_name, int unk1, int unk2);
int FUN_0056302b(int param_1);
void ReadPalette(char *palette_text_path, char *palette_binary_path);
void DrawGraphicsLine(FacemakerWindowBounds *window_bounds, int x1, int y1, int x2, int y2, int color_index);
unsigned int WaitForInputEventUnlessBlocked(void);
int single_color_test_bit_to_color_t(int mask);
void DrawTextLineNoShadow(char *text, int x, int y, int color_index);
void DrawCenteredTextLineClamped(char *text, int center_x, int y, int color_index);
void DrawScaledCenteredTextNoShadow(char *text, int center_x, int y, int color_index);
int ScaleUiCoordinateFrom320(int value);
void BlitGraphicsRect(FacemakerWindowBounds *dst, unsigned int dst_x, int dst_y, unsigned int width, DWORD height,
                      FacemakerWindowBounds *src, int src_x, int src_y);
void StretchBlitGraphicsRect(FacemakerWindowBounds *dst, int dst_x, int dst_y, int src_w, int src_h,
                             FacemakerWindowBounds *src, int src_x, int src_y, int copy_w, int copy_h);
void DrawEncodedImageResampled(FacemakerWindowBounds *dst, int x, int y, int width, int height, EncodedImage *encoded_image);
void FillGraphicsRect(FacemakerWindowBounds *window_bounds, int x, int y, int width, int height, unsigned int color_index);
void DrawFormattedTextNoShadowCentered(FacemakerWindowBounds *dst, int text_color, int x, int y, char *format, ...);
void DrawFormattedTextShadowedCentered(FacemakerWindowBounds *window, int color_index, int x, int y, char *format, ...);
void DrawWorldUiFormattedText(FacemakerWindowBounds *window, int color_index, int x, int y, char *format, ...);
void FUN_0057b5f0(FacemakerWindowBounds *window, int color_index, int x, int y, char *format, ...);
unsigned int FUN_004bb458(int world_magic_slot_index);
int ScaleUiCoordinate(int value);
void DrawTextAt(FacemakerWindowBounds *dst, int text_color, int x, int y, char *text, ...);
void DrawUiScaledCenteredText(char *text, int center_x, int y, int color_index);
void ClearInputAndWaitForMouseRelease(void);
void BeginSpriteEncodeSession(void);
EncodedImage *EncodeSpriteFromPage(int page_number, int x, int y, int width, int height);
void FinalizeSpriteEncodeSession(void);
int SetFontStyleSize(int font_id, unsigned int style);
extern FILE *g_advbuttons_ini_file;
extern char g_ini_string_scratch[0x28];
void LoadPcxResource(int page_number, int x, int y, char *path, void *opaque);
int ApplyPortraitPaletteMap(FacemakerWindowBounds *page, int src_x, int src_y, unsigned int width, int height, char *palette_source_path,
                            char *portrait_path);
int CountDuelPoolEligibleTowns(void);
void RpBits_ApplyPalette(RpBitsPalettePacket *palette_data);
int FadeInPaletteFromGray(int gray, int steps);
void LoadPcxIntoPageOpaque(int page_number, char *path);
int BeginMenuContext(void);
int ResetMenuContext(int context_index);
int AddMenuControlsToContext(AdvMenuControl *controls, int control_count, int context_index);
int EndMenuContext(void);
void UpdateMouseSnapshot(void);
int UpdateMenuControlSelection(int mouse_x, int mouse_y, int allow_activate_on_click);
void FreeSpriteBlob(void *memory);
void PlaySoundEffectOnChannel(char *sound_path, int channel, int volume, int pitch_percent, int pan_percent);
extern char *g_world_magic_sound_paths[5];
int ShowStatsWindow(int mode, int highlight);
void ShowWorldMapScreen(int mode);
int ConsumeUiTickCount(void);
int GetUiTickCount(void);
int IsKeyInputQueueEmpty(void);
int PopQueuedKeyInput(void);
void FUN_00550164(int tile_x, int tile_y, int *out_x, int *out_y);
void DelayUiTicks(int ticks);
int SignNonZero(int value);
int ClampIntToRange(int value, int min_value, int max_value);
int MeasureMultilineTextWidth(FacemakerWindowBounds *dst, char *text);
int GetFontLineHeight(int font_slot);
void ApplyPortraitTintMap(FacemakerWindowBounds *dst, int x, int y, int w, int h, unsigned int tint, int maybe_shadow);

int RenderStatsCreatureGridPage(int page_index);

// FUNCTION: SHANDALAR 0x00500e30
void ScaleMenuControlsForScreen(AdvMenuControl *control, int control_count)
{
  int i;

  if ((global_screen_width != 0x280) && (control->x == control->base_x))
  {
    for (i = 0; i < control_count; i = i + 1, control = control + 1)
    {
      control->x = ScaleUiCoordinate(control->x);
      control->y = ScaleUiCoordinate(control->y);
      control->width = ScaleUiCoordinate(control->width);
      control->height = ScaleUiCoordinate(control->height);
    }
  }
}

// FUNCTION: SHANDALAR 0x004ff340
int RenderBasicImageMenuControl(AdvMenuControl *control, int mode)
{
  int mouse_inside;

  if (g_menu_render_guard == 0)
  {
    if ((g_mouse_x < control->x) || (control->width + control->x < g_mouse_x))
    {
      mouse_inside = 0;
    }
    else if ((g_mouse_y < control->y) || (control->y + control->height < g_mouse_y))
    {
      mouse_inside = 0;
    }
    else
    {
      mouse_inside = 1;
    }
    if (!mouse_inside)
    {
      return 0;
    }
  }
  if (control->state == 3)
  {
    return 0;
  }
  DrawEncodedImageResampled(g_menu_control_draw_target_page, control->x, control->y, control->width, control->height,
                            (EncodedImage *)control->mode_data[mode]);
  if ((mode == 2) && (control->on_activate != 0))
  {
    control->on_activate(control);
  }
  return 1;
}

// FUNCTION: SHANDALAR 0x00548eb1
void DrawStatsRectFrame(FacemakerWindowBounds *window, int x, int y, int width, int height, int color_index)
{
  struct
  {
    int x2;
    int y2;
  } s;

  s.x2 = width + x;
  s.y2 = height + y;
  DrawGraphicsLine(window, x, y, s.x2, y, color_index);
  DrawGraphicsLine(window, s.x2, y, s.x2, s.y2, color_index);
  DrawGraphicsLine(window, s.x2, s.y2, x, s.y2, color_index);
  DrawGraphicsLine(window, x, s.y2, x, y, color_index);
}

// FUNCTION: SHANDALAR 0x005487c0
int RenderStatsTabButtonControl(AdvMenuControl *control, int mode)
{
  struct
  {
    int mouse_inside;
    int y;
    int x;
    int height;
    int width;
    EncodedImage *highlight_image;
    EncodedImage *encoded_image;
  } s;

  if (g_menu_render_guard == 0)
  {
    if ((g_mouse_x < control->x) || (control->width + control->x < g_mouse_x))
    {
      s.mouse_inside = 0;
    }
    else if ((g_mouse_y < control->y) || (control->y + control->height < g_mouse_y))
    {
      s.mouse_inside = 0;
    }
    else
    {
      s.mouse_inside = 1;
    }
    if (!s.mouse_inside)
    {
      return 0;
    }
  }
  if (control->state == 3)
  {
    return 0;
  }
  s.encoded_image = *(EncodedImage **)((char *)&g_stats_menu_button_sprites + ((control->selection_value - 1) << 4) + mode * 4);
  s.highlight_image = *(EncodedImage **)((char *)&g_stats_menu_button_sprites + ((control->selection_value - 1) << 4) + 4);
  s.x = control->x;
  s.y = control->y;
  s.width = control->width;
  s.height = control->height;
  if (mode == 2)
  {
    DrawEncodedImageResampled(PTR_DAT_00583304, 0, s.y + 0x80, s.width, s.height, s.highlight_image);
    DrawEncodedImageResampled(PTR_DAT_00583304, 2, s.y + 0x82, s.width - 4, s.height - 4, s.encoded_image);
    BlitGraphicsRect(PTR_DAT_00583304, 0, s.y + 0x80, s.width, s.height, PTR_DAT_005832b4, s.x, s.y);

    if (mode == 2)
    {
      if (control->on_activate != 0)
      {
        control->on_activate(control);
      }
    }
  }
  else
  {
    DrawEncodedImageResampled(PTR_DAT_005832b4, s.x, s.y, s.width, s.height, s.encoded_image);
  }
  return 1;
}

// FUNCTION: SHANDALAR 0x00548b25
static int ActivateStatsMenuControl(AdvMenuControl *control)
{
  unsigned int world_magic_index = control->selection_value - 0xf;
  if (((control->selection_value >= 0xf) && ((int)world_magic_index >= 2)) && ((world_magic_index & 1) == 0))
  {
    PlaySoundEffectOnChannel(g_world_magic_sound_paths[(control->selection_value - 0x11) / 2], 0xf, 100, 100, 0);
  }
  else
  {
    PlaySoundEffectOnChannel("x:sound\\button2.wav", 0xf, 100, 100, 0);
  }
  g_stats_menu_selection = control->selection_value;
  return g_stats_menu_selection;
}

// FUNCTION: SHANDALAR 0x0054899f
int RenderStatsWorldMagicControl(AdvMenuControl *control, int mode)
{
  struct
  {
    int mouse_inside;
    int y;
    int x;
    int height;
    int width;
    EncodedImage *highlight_sprite;
    EncodedImage *sprite;
  } s;

  if (g_menu_render_guard == 0)
  {
    if ((g_mouse_x < control->x) || (control->width + control->x < g_mouse_x))
    {
      s.mouse_inside = 0;
    }
    else if ((g_mouse_y < control->y) || (control->y + control->height < g_mouse_y))
    {
      s.mouse_inside = 0;
    }
    else
    {
      s.mouse_inside = 1;
    }
    if (s.mouse_inside == 0)
    {
      return 0;
    }
  }
  if (control->state == 3)
  {
    return 0;
  }

  s.sprite = (EncodedImage *)control->mode_data[mode];
  s.highlight_sprite = (EncodedImage *)control->mode_data[1];
  s.x = control->x;
  s.y = control->y;
  s.width = control->width;
  s.height = control->height;

  if (mode == 2)
  {
    DrawEncodedImageResampled(PTR_DAT_005832b4, control->x + 2, control->y + 2, control->width - 4, control->height - 4,
                              s.highlight_sprite);

    if (mode == 2)
    {
      if (control->on_activate != 0)
      {
        control->on_activate(control);
      }
    }
  }
  else
  {
    DrawEncodedImageResampled(PTR_DAT_005832b4, s.x, s.y, s.width, s.height, s.sprite);
  }
  return 1;
}

// FUNCTION: SHANDALAR 0x00548bb5
static int RenderStatsScrollbarHotspot(AdvMenuControl *control, int mode)
{
  struct
  {
    int mouse_inside;
    int y;
    int x;
    int height;
    int width;
  } s;

  if (g_menu_render_guard == 0)
  {
    if ((g_mouse_x < control->x) || (control->width + control->x < g_mouse_x))
    {
      s.mouse_inside = 0;
    }
    else if ((g_mouse_y < control->y) || (control->y + control->height < g_mouse_y))
    {
      s.mouse_inside = 0;
    }
    else
    {
      s.mouse_inside = 1;
    }
    if (!s.mouse_inside)
    {
      return 0;
    }
  }
  if (control->state == 3)
  {
    return 0;
  }
  s.x = control->x;
  s.y = control->y;
  s.width = control->width;
  s.height = control->height;
  if ((mode == 2) && (control->on_activate != 0))
  {
    control->on_activate(control);
  }
  return 1;
}

// FUNCTION: SHANDALAR 0x00548f51
static int RenderStatsColorHotspot(AdvMenuControl *control, int mode)
{
  int mouse_inside;

  if ((g_mouse_x < control->x) || (control->width + control->x < g_mouse_x))
  {
    mouse_inside = 0;
  }
  else if ((g_mouse_y < control->y) || (control->y + control->height < g_mouse_y))
  {
    mouse_inside = 0;
  }
  else
  {
    mouse_inside = 1;
  }
  if (mouse_inside == 0)
  {
    return 0;
  }

  if (mode == 2)
  {
    g_stats_menu_selection = control->selection_value;
  }
  return 1;
}

static __inline void InitializeStatsFixedMenuControls(void)
{
  int i;

  memset(g_stats_menu_controls, 0, sizeof(g_stats_menu_controls));
  g_stats_menu_controls[0].base_x = 0x193;
  g_stats_menu_controls[0].base_y = 7;
  g_stats_menu_controls[0].base_width = 0x54;
  g_stats_menu_controls[0].base_height = 0x17;
  g_stats_menu_controls[0].x = 0x193;
  g_stats_menu_controls[0].y = 7;
  g_stats_menu_controls[0].width = 0x54;
  g_stats_menu_controls[0].height = 0x17;
  g_stats_menu_controls[0].enabled = 1;
  g_stats_menu_controls[0].on_render = (AdvMenuRenderCallback)RenderStatsTabButtonControl;
  g_stats_menu_controls[0].on_activate = (AdvMenuActivateCallback)ActivateStatsMenuControl;
  g_stats_menu_controls[0].selection_value = 1;

  g_stats_menu_controls[1] = g_stats_menu_controls[0];
  g_stats_menu_controls[1].base_x = 0x1ec;
  g_stats_menu_controls[1].x = 0x1ec;
  g_stats_menu_controls[1].base_width = 0x48;
  g_stats_menu_controls[1].width = 0x48;
  g_stats_menu_controls[1].selection_value = 2;

  g_stats_menu_controls[2] = g_stats_menu_controls[0];
  g_stats_menu_controls[2].base_x = 0x23c;
  g_stats_menu_controls[2].x = 0x23c;
  g_stats_menu_controls[2].base_width = 0x31;
  g_stats_menu_controls[2].width = 0x31;
  g_stats_menu_controls[2].selection_value = 3;

  g_stats_menu_controls[3].base_x = 0xe3;
  g_stats_menu_controls[3].base_y = 0x2b;
  g_stats_menu_controls[3].base_width = 0xe;
  g_stats_menu_controls[3].base_height = 0x96;
  g_stats_menu_controls[3].x = 0xe3;
  g_stats_menu_controls[3].y = 0x2b;
  g_stats_menu_controls[3].width = 0xe;
  g_stats_menu_controls[3].height = 0x96;
  g_stats_menu_controls[3].enabled = 1;
  g_stats_menu_controls[3].on_render = (AdvMenuRenderCallback)RenderStatsScrollbarHotspot;
  g_stats_menu_controls[3].on_activate = (AdvMenuActivateCallback)DragStatsCreatureScrollbar;
  g_stats_menu_controls[3].selection_value = 4;
  g_stats_menu_controls[3].mode_data[3] = 0x12;

  g_stats_menu_controls[4] = g_stats_menu_controls[3];
  g_stats_menu_controls[4].base_height = 0x12;
  g_stats_menu_controls[4].height = 0x12;
  g_stats_menu_controls[4].on_render = (AdvMenuRenderCallback)RenderStatsCreatureScrollbar;
  g_stats_menu_controls[4].selection_value = 5;

  for (i = 0; i < 5; i = i + 1)
  {
    g_stats_menu_controls[i + 5].base_x = i * 0x39 + 0x14d;
    g_stats_menu_controls[i + 5].base_y = 0x154;
    g_stats_menu_controls[i + 5].base_width = 0x30;
    g_stats_menu_controls[i + 5].base_height = 0x30;
    g_stats_menu_controls[i + 5].x = i * 0x39 + 0x14d;
    g_stats_menu_controls[i + 5].y = 0x154;
    g_stats_menu_controls[i + 5].width = 0x30;
    g_stats_menu_controls[i + 5].height = 0x30;
    g_stats_menu_controls[i + 5].enabled = 1;
    g_stats_menu_controls[i + 5].on_render = (AdvMenuRenderCallback)RenderStatsColorHotspot;
    g_stats_menu_controls[i + 5].selection_value = i + 10;
    g_stats_menu_controls[i + 5].state = g_world_magic_town_flags[i] ? 0 : 3;
  }
}

static __inline void InitializeStatsWorldMagicControls(void)
{
  int i;
  int x;
  int y;

  for (i = 0; i < 0xc; i = i + 1)
  {
    g_stats_menu_controls[i + 10].state = 3;
    g_stats_world_magic_unlocked[i] = 0;
    if ((i >= 2) && ((i & 1) == 0))
    {
      y = 0xd2;
      x = ((i - 2) / 2) * 0x35 + 0x16b;
      g_stats_menu_controls[i + 10].x = x;
      g_stats_menu_controls[i + 10].base_x = g_stats_menu_controls[i + 10].x;
      g_stats_menu_controls[i + 10].y = y;
      g_stats_menu_controls[i + 10].base_y = g_stats_menu_controls[i + 10].y;
      if ((g_world_magic_bitmap & (1 << (byte)i)) != 0)
      {
        g_stats_menu_controls[i + 10].state = 0;
        g_stats_world_magic_unlocked[i] = 1;
      }
    }
    if (((i < 2) || ((i & 1) != 0)) || (g_stats_world_magic_unlocked[i] == 0))
    {
      y = 0x10e;
      if (i < 2)
      {
        x = i * 0x35;
      }
      else
      {
        x = ((i - 2) / 2) * 0x35 + 0x6a;
      }
      x = x + 0xee;
      g_stats_menu_controls[i + 10].x = x;
      g_stats_menu_controls[i + 10].base_x = g_stats_menu_controls[i + 10].x;
      g_stats_menu_controls[i + 10].y = y;
      g_stats_menu_controls[i + 10].base_y = g_stats_menu_controls[i + 10].y;
      if ((g_world_magic_bitmap & (1 << (byte)i)) != 0)
      {
        g_stats_menu_controls[i + 10].state = 0;
        g_stats_world_magic_unlocked[i] = 1;
      }
    }
    g_stats_menu_controls[i + 10].height = 0x35;
    g_stats_menu_controls[i + 10].base_height = g_stats_menu_controls[i + 10].height;
    g_stats_menu_controls[i + 10].width = g_stats_menu_controls[i + 10].base_height;
    g_stats_menu_controls[i + 10].base_width = g_stats_menu_controls[i + 10].width;
    g_stats_menu_controls[i + 10].mode_data[0] = (int)g_world_magic_choice_button_sprite_bank.named.normal[i];
    g_stats_menu_controls[i + 10].mode_data[2] = (int)g_world_magic_choice_button_sprite_bank.named.highlight[i];
    g_stats_menu_controls[i + 10].mode_data[1] = g_stats_menu_controls[i + 10].mode_data[2];
  }
}

// FUNCTION: SHANDALAR 0x00548dfb
int RenderStatsCreatureScrollbar(AdvMenuControl *control)
{
  struct
  {
    int page_count;
    int y;
    EncodedImage *encoded_image;
    AdvMenuControl *thumb_control;
  } s;

  s.page_count = 0x11;
  s.thumb_control = control + 1;
  s.encoded_image = g_stats_menu_scroll_thumb_sprite;
  s.y = control->y + s.thumb_control->height / 2;
  FillGraphicsRect(PTR_DAT_005832b4, control->x, control->y, control->width, control->height, 0);
  DrawEncodedImageResampled(PTR_DAT_005832b4, control->x, s.y - s.thumb_control->height / 2, control->width,
                            ((int)s.encoded_image->height * control->width) / control->base_width, s.encoded_image);
  return 0;
}

// FUNCTION: SHANDALAR 0x00548cba
static int DragStatsCreatureScrollbar(AdvMenuControl *control)
{
  struct
  {
    int page_count;
    int y;
    EncodedImage *encoded_image;
    int page_index;
    AdvMenuControl *thumb_control;
  } s;

  s.page_count = 0x11;
  s.thumb_control = control + 1;
  s.encoded_image = g_stats_menu_scroll_thumb_sprite;
  UpdateMouseSnapshot();
  s.y = g_mouse_y_snapshot;
  s.y = MAX(control->y + s.thumb_control->height / 2, s.y);
  s.y = MIN(control->y + control->height - s.thumb_control->height / 2, s.y);
  s.page_index = ((s.y - control->y - s.thumb_control->height / 2) * s.page_count) / (control->height - s.thumb_control->height);
  FillGraphicsRect(PTR_DAT_005832b4, control->x, control->y, control->width, control->height, 0);
  DrawEncodedImageResampled(PTR_DAT_005832b4, control->x, s.y - s.thumb_control->height / 2, control->width,
                            ((int)s.encoded_image->height * control->width) / control->base_width, s.encoded_image);
  RenderStatsCreatureGridPage(s.page_index);
  g_stats_menu_selection = control->selection_value;
  return g_stats_menu_selection;
}

// FUNCTION: SHANDALAR 0x0054a4aa
int RebuildStatsJournalCounts(void)
{
  unsigned int entry_arg;
  int entry_type;
  int journal_index;
  int skipped_entry_count;

  skipped_entry_count = 0;
  memset(g_stats_creature_journal_counts, 0, sizeof(g_stats_creature_journal_counts));
  g_stats_journal_failed_quests =
      g_stats_journal_completed_quests =
          g_stats_journal_dungeons_explored =
              g_stats_journal_lairs_explored =
                  g_stats_journal_cities_saved =
                      g_stats_journal_spells_found =
                          g_stats_journal_artifacts_found = 0;

  for (journal_index = 0; journal_index < 10000; journal_index = journal_index + 1)
  {
    entry_type = g_journal_entries[journal_index].type;
    entry_arg = g_journal_entries[journal_index].arg;
    if (g_journal_entries[journal_index].type == JOURNAL_ENTRY_NONE)
    {
      break;
    }
    switch (entry_type)
    {
    case 2:
      if ((entry_arg & 0x80) != 0)
      {
        g_stats_journal_failed_quests = g_stats_journal_failed_quests + 1;
        g_stats_creature_journal_counts[entry_arg & 0x7f].wins = g_stats_creature_journal_counts[entry_arg & 0x7f].wins + 1;
      }
      else
      {
        g_stats_journal_completed_quests = g_stats_journal_completed_quests + 1;
        g_stats_creature_journal_counts[entry_arg & 0x7f].losses = g_stats_creature_journal_counts[entry_arg & 0x7f].losses + 1;
      }
      break;
    case 4:
      g_stats_journal_world_magic_counts[entry_arg & 0x7f] = g_stats_journal_world_magic_counts[entry_arg & 0x7f] + 1;
      break;
    case 7:
      g_stats_journal_cities_saved = g_stats_journal_cities_saved + 1;
      break;
    case 0xd:
      g_stats_journal_lairs_explored = g_stats_journal_lairs_explored + 1;
      break;
    case 0xf:
      skipped_entry_count = skipped_entry_count + 1;
      break;
    case 0x10:
      g_stats_journal_spells_found = g_stats_journal_spells_found + 1;
      break;
    case 0x11:
      g_stats_journal_artifacts_found = g_stats_journal_artifacts_found + 1;
      break;
    case 0x13:
      g_stats_journal_dungeons_explored = g_stats_journal_dungeons_explored + 1;
      break;
    }
  }
  return 0;
}

// FUNCTION: SHANDALAR 0x0054a099
int RenderStatsCreatureGridPage(int page_index)
{
  struct
  {
    int row_count;              // ebp - 0x34
    unsigned int creature_type; // ebp - 0x30
    int win_percent;            // ebp - 0x2c
    int text_colors[5];         // ebp - 0x28
    int losses;                 // ebp - 0x14
    int row;                    // ebp - 0x10
    int column;                 // ebp - 0xc
    int wins;                   // ebp - 0x8
    int color_index;            // ebp - 0x4
  } s;

  if (page_index == 0x12)
  {
    s.row_count = 1;
  }
  else
  {
    s.row_count = 2;
  }
  s.text_colors[0] = 0xbf;
  s.text_colors[1] = 0xbc;
  s.text_colors[2] = 0xb7;
  s.text_colors[3] = 0xf4;
  s.text_colors[4] = 0xf6;
  s.creature_type = page_index * 3;
  BlitGraphicsRect(PTR_DAT_00583304, 0, 0x154, ScaleUiCoordinate(0x173), ScaleUiCoordinate(0x7f), PTR_DAT_00583304, 0, 0x80);
  for (s.row = 0; s.row < s.row_count; s.row++)
  {
    s.column = 0;
    for (; (s.column < 3 && (int)s.creature_type < 0x37); s.column++, s.creature_type++)
    {
      if ((g_stats_creature_journal_counts[s.creature_type + 1].wins != 0) ||
          ((g_stats_creature_journal_counts[s.creature_type + 1].losses != 0 || (g_reveal_all_world_info != 0))))
      {
        s.wins = g_stats_creature_journal_counts[s.creature_type + 1].wins;
        s.losses = g_stats_creature_journal_counts[s.creature_type + 1].losses;

        s.win_percent = s.wins * 100 / MAX(s.wins + s.losses, 1);

        if (s.wins < s.losses)
        {
          if ((s.losses - s.wins > 5) && (s.win_percent <= 25))
          {
            s.color_index = 0;
          }
          else if ((s.wins + s.losses < 5) || (s.win_percent > 40))
          {
            s.color_index = 2;
          }
          else
          {
            s.color_index = 1;
          }
        }
        else if ((s.wins - s.losses > 5) && (s.win_percent >= 75))
        {
          s.color_index = 4;
        }
        else if ((s.wins + s.losses < 5) || (s.win_percent <= 60))
        {
          s.color_index = 2;
        }
        else
        {
          s.color_index = 3;
        }
        BlitGraphicsRect(PTR_DAT_005832dc,
                         g_stats_creature_sheet_cell_dims[g_stats_creature_sheet_resolution][0] * ((int)s.creature_type % 8) + 1,
                         g_stats_creature_sheet_cell_dims[g_stats_creature_sheet_resolution][1] * ((int)s.creature_type / 8) +
                             ScaleUiCoordinate(3) + 1,
                         g_stats_creature_sheet_cell_dims[g_stats_creature_sheet_resolution][0] - 2,
                         (g_stats_creature_sheet_cell_dims[g_stats_creature_sheet_resolution][1] - 2) - ScaleUiCoordinate(3),
                         PTR_DAT_00583304, ScaleUiCoordinate(0x7c) * s.column, ScaleUiCoordinate(s.row << 6) + 0x80);
        PTR_DAT_00583304->font_slot = 4;
        DrawFormattedTextShadowedCentered(PTR_DAT_00583304, s.text_colors[s.color_index],
                                          ScaleUiCoordinate(0x7c) * s.column + ScaleUiCoordinate(0x5e),
                                          ScaleUiCoordinate(s.row * 0x40 + -3) + ScaleUiCoordinate(0x20) + 0x80, "%d/%d",
                                          g_stats_creature_journal_counts[s.creature_type + 1].wins, g_stats_creature_journal_counts[s.creature_type + 1].losses);
        PTR_DAT_00583304->font_slot = 1;
        DrawFormattedTextShadowedCentered(PTR_DAT_00583304, 0xb7, ScaleUiCoordinate(0x7c) * s.column + ScaleUiCoordinate(0x3e),
                                          ScaleUiCoordinate(s.row << 6) + ScaleUiCoordinate(0x36) + 0x80, "%s",
                                          g_shandalar_monster_definitions[s.creature_type + 1].name);
      }
    }
  }
  BlitGraphicsRect(PTR_DAT_00583304, 0, 0x80, ScaleUiCoordinate(0x7c) * 3 - 2, ScaleUiCoordinate(0x40) * 2 - 2, PTR_DAT_005832b4,
                   ScaleUiCoordinate(0xf6), ScaleUiCoordinate(0x43));
  g_stats_creature_page_index = page_index;
  return page_index;
}

// FUNCTION: SHANDALAR 0x0054a668
int ShowWorldMagicStatsDetail(int world_magic_slot_index)
{
  struct
  {
    int icon_top;
    int icon_bottom;
    int detail_result;
    int y;
    int icon_right;
    int x;
    int icon_left;
    EncodedImage *encoded_image;
  } s;

  s.icon_left = 0x14f;
  s.icon_right = 0x19e;
  s.icon_top = 0x4c;
  s.icon_bottom = 0x9a;
  LoadPcxIntoPage(1, "worlbak1.pic");
  StretchBlitGraphicsRect(PTR_DAT_005832dc, 0, 0, 0x280, 0x1e0, PTR_DAT_005832b4, 0, 0, global_screen_width, global_screen_height);
  s.detail_result = FUN_004bb458(world_magic_slot_index);
  s.y = 0x28;
  s.y += 0x10;
  s.encoded_image = g_world_magic_choice_button_sprite_bank.named.normal[world_magic_slot_index];
  s.y = ((s.icon_bottom - s.icon_top) - s.encoded_image->height) / 2 + s.icon_top;
  s.x = ((s.icon_right - s.icon_left) - s.encoded_image->width) / 2 + s.icon_left;
  DrawEncodedImageResampled(PTR_DAT_005832b4, ScaleUiCoordinate(s.x), ScaleUiCoordinate(s.y),
                            ScaleUiCoordinate((int)s.encoded_image->width), ScaleUiCoordinate((int)s.encoded_image->height),
                            g_world_magic_choice_button_sprite_bank.named.normal[world_magic_slot_index]);
  s.y += (int)s.encoded_image->height + 0x10;
  s.y /= 2;
  s.x += (int)s.encoded_image->width / 2;
  s.x /= 2;
  PTR_DAT_005832b4->font_slot = 4;
  DrawTextAt(PTR_DAT_005832b4, 0x7b, 0x176, 0x4b, gs_worldmagic_title_0077e1d0);
  s.y -= 5;
  strcpy(g_ui_message_buffer, gs_worldmagic_names_00780660[world_magic_slot_index]);
  DrawUiScaledCenteredText(g_ui_message_buffer, s.x, s.y, 0x40);
  s.y += 8;
  strcpy(g_ui_message_buffer, gs_worldmagic_explains_0074b8f0[world_magic_slot_index]);
  DrawWorldUiFormattedText(PTR_DAT_005832b4, 0x7b, ScaleUiCoordinateFrom320(s.x), ScaleUiCoordinateFrom320(s.y), g_ui_message_buffer);
  s.y += 0x10;
  ClearInputAndWaitForMouseRelease();
  WaitForInputEventUnlessBlocked();
  return 0;
}

// Helper factored from RunAdventureStatsMenu (SHANDALAR 0x00549002); not a standalone original function.
static __inline void RenderAdventureStatsOverview(FacemakerWindowBounds *portrait_work_page, int *colors)
{
  int world_magic_slot_index;

  PTR_DAT_005832dc->font_slot = 4;
  PTR_DAT_00583304->font_slot = 4;
  g_graphics_pages[4] = g_facemaker_page4_dib;
  SelectObject(g_graphics_pages[4]->hTempDC, g_facemaker_page4_bitmap);
  if (g_graphics_bpp == 8)
  {
    LoadPcxResource(1, 0, global_screen_height - 0x1e0, "statbak.pic", (void *)1);
  }
  else
  {
    LoadPcxResource(1, 0, global_screen_height - 0x1e0, "statbak.pic", (void *)1);
  }
  DrawStatsRectFrame(PTR_DAT_005832dc, 0x26, global_screen_height - 0x1d1, 0x8c, 0xac,
                     g_stats_portrait_frame_colors[g_starting_color - 1][0]);
  DrawStatsRectFrame(PTR_DAT_005832dc, 0x27, global_screen_height - 0x1d0, 0x8a, 0xaa,
                     g_stats_portrait_frame_colors[g_starting_color - 1][1]);
  StretchBlitGraphicsRect(PTR_DAT_005832dc, 0, global_screen_height - 0x1e0, 0x280, 0x1e0, PTR_DAT_005832dc, 0, 0,
                          global_screen_width, global_screen_height);
  StretchBlitGraphicsRect(portrait_work_page, 0, 0, 0x89, 0xa9, PTR_DAT_005832dc, ScaleUiCoordinate(0x28),
                          ScaleUiCoordinate(0x11), ScaleUiCoordinate(0x89), ScaleUiCoordinate(0xa9));
  BlitGraphicsRect(PTR_DAT_005832dc, ScaleUiCoordinate(0xf7), ScaleUiCoordinate(0x43), ScaleUiCoordinate(0x173),
                   ScaleUiCoordinate(0x7f), PTR_DAT_00583304, 0, 0x154);
  ApplyPortraitPaletteMap(PTR_DAT_005832dc, ScaleUiCoordinate(0x28), ScaleUiCoordinate(0x11), ScaleUiCoordinate(0x89),
                          ScaleUiCoordinate(0xa9), "prdblk.pic", "advfac64.pic");
  SelectObject(g_graphics_pages[4]->hTempDC, g_graphics_pages[4]->hPreviousBitmap);
  g_graphics_pages[4] = (DIBSurface *)0;

  DrawTextAt(PTR_DAT_005832dc, colors[3], 0x4f, 0x102, "%d", Gold);
  DrawTextAt(PTR_DAT_005832dc, colors[3], 0xb7, 0x102, "%d", g_food);
  DrawTextAt(PTR_DAT_005832dc, colors[3], 0x4f, 0x138, "%d", CountDuelPoolEligibleTowns());
  DrawTextAt(PTR_DAT_005832dc, colors[3], 0xb7, 0x138, "%d/%d", DAT_0078df68,
             DAT_00789938);
  DrawTextAt(PTR_DAT_005832dc, colors[3], 0x29, 0x161, "%d", g_amulet_inventory[0]);
  DrawTextAt(PTR_DAT_005832dc, colors[3], 0x4e, 0x161, "%d", g_amulet_inventory[1]);
  DrawTextAt(PTR_DAT_005832dc, colors[3], 0x73, 0x161, "%d", g_amulet_inventory[2]);
  DrawTextAt(PTR_DAT_005832dc, colors[3], 0x98, 0x161, "%d", g_amulet_inventory[3]);
  DrawTextAt(PTR_DAT_005832dc, colors[3], 0xbd, 0x161, "%d", g_amulet_inventory[4]);
  DrawTextAt(PTR_DAT_005832dc, colors[4], 0x69, 0xc6, "%s",
             gs_difficultylevels_0077d130[g_shandalar_difficulty]);
  DrawTextAt(PTR_DAT_005832dc, colors[4], 0x69, 0xd6, "%s", g_name_entry_buffer);
  DrawTextAt(PTR_DAT_005832dc, colors[4], 0x10e, 0x16c, gs_stats_0077cfa0[0]);

  for (world_magic_slot_index = 0; (int)world_magic_slot_index < 5; world_magic_slot_index = world_magic_slot_index + 1)
  {
    if (g_world_magic_town_flags[world_magic_slot_index] != 0)
    {
      DrawEncodedImageResampled(PTR_DAT_005832dc, ScaleUiCoordinate(world_magic_slot_index * 0x39 + 0x14d), ScaleUiCoordinate(0x154),
                                ScaleUiCoordinate(0x30), ScaleUiCoordinate(0x30), g_stats_menu_color_sprites[world_magic_slot_index]);
    }
  }

  DrawTextAt(PTR_DAT_005832dc, colors[4], 0x118, 0xeb, gs_stats_0077cfa0[1]);
  InitializeStatsWorldMagicControls();
  ScaleMenuControlsForScreen(&g_stats_menu_controls[10], 0xc);
  RebuildStatsJournalCounts();
  FUN_0057b5f0(PTR_DAT_005832dc, colors[4], 0x14, 0x184, gs_stats_0077cfa0[2]);
  DrawTextAt(PTR_DAT_005832dc, colors[3], 0xb2, 0x184, "%d", g_stats_journal_lairs_explored);
  FUN_0057b5f0(PTR_DAT_005832dc, colors[4], 0x14, 0x196, gs_stats_0077cfa0[3]);
  DrawTextAt(PTR_DAT_005832dc, colors[3], 0xb2, 0x196, "%d", g_stats_journal_cities_saved);
  FUN_0057b5f0(PTR_DAT_005832dc, colors[4], 0x14, 0x1a8, gs_stats_0077cfa0[4]);
  DrawTextAt(PTR_DAT_005832dc, colors[3], 0xb2, 0x1a8, "%d/%d",
             g_stats_journal_spells_found, g_stats_journal_artifacts_found);
  FUN_0057b5f0(PTR_DAT_005832dc, colors[4], 0x14, 0x1ba, gs_stats_0077cfa0[5]);
  DrawTextAt(PTR_DAT_005832dc, colors[3], 0xb2, 0x1ba, "%d/%d",
             g_stats_journal_failed_quests, g_stats_journal_completed_quests);
  FUN_0057b5f0(PTR_DAT_005832dc, colors[4], 0xe9, 0x19a, gs_stats_0077cfa0[6]);
  for (world_magic_slot_index = 0; (int)world_magic_slot_index < 5; world_magic_slot_index = world_magic_slot_index + 1)
  {
    FUN_0057b5f0(PTR_DAT_005832dc, colors[3], world_magic_slot_index * 0x39 + 0x165, 0x19a, "%d",
                 g_castle_dungeon_slots[world_magic_slot_index].times_entered + 1);
  }
  FUN_0057b5f0(PTR_DAT_005832dc, colors[4], 0xe9, 0x1c0, gs_stats_0077cfa0[7]);
  DrawTextAt(PTR_DAT_005832dc, colors[3], 0x1a1, 0x1c0, "%d", g_stats_journal_dungeons_explored);
  DrawTextAt(PTR_DAT_005832dc, colors[4], 0x1b0, 0x36, gs_stats_0077cfa0[8]);

  if (g_graphics_bpp == 8)
  {
    memset(g_palette_data_words.entry_data, 0, 0x300);
    RpBits_ApplyPalette(&g_palette_data_words);
  }
  if (g_graphics_bpp == 8)
  {
    LoadPcxResource(-1, 0, 0, "advfac64.pic", &g_palette_data_words);
  }
  else
  {
    LoadPcxIntoPageNoPalette("advfac64.pic");
  }
  BlitGraphicsRect(PTR_DAT_005832dc, 0, 0, global_screen_width, global_screen_height, PTR_DAT_005832b4, 0, 0);
  FadeInPaletteFromGray(0, g_default_palette_fade_steps);
}

// FUNCTION: SHANDALAR 0x00428b90
static int RenderJournalExitButtonControl(AdvMenuControl *control, int mode)
{
  struct
  {
    int mouse_inside;    // ebp - 0x20
    int draw_state;      // ebp - 0x1c
    int draw_y;          // ebp - 0x18
    int draw_x;          // ebp - 0x14
    int draw_h;          // ebp - 0x10
    int draw_w;          // ebp - 0xc
    int old_page_number; // ebp - 0x8
    int selection_value; // ebp - 0x4
  } s;

  if (g_menu_render_guard == 0)
  {
    if ((g_mouse_x < control->x) || (control->width + control->x < g_mouse_x))
    {
      s.mouse_inside = 0;
    }
    else if ((g_mouse_y < control->y) || (control->y + control->height < g_mouse_y))
    {
      s.mouse_inside = 0;
    }
    else
    {
      s.mouse_inside = 1;
    }
    if (!s.mouse_inside)
    {
      return 0;
    }
  }
  if (control->state == 3)
  {
    return 0;
  }
  s.old_page_number = PTR_DAT_005832b4->page_number;
  s.selection_value = g_stats_journal_menu_controls[0].selection_value;
  switch (mode)
  {
  case 0:
    s.draw_state = 0;
    break;
  case 1:
    s.draw_state = 1;
    break;
  case 2:
    s.draw_state = 2;
    break;
  }

  s.draw_x = g_stats_journal_menu_controls[0].x;
  s.draw_y = g_stats_journal_menu_controls[0].y;
  s.draw_w = g_stats_journal_menu_controls[0].width;
  s.draw_h = g_stats_journal_menu_controls[0].height;
  DrawEncodedImageResampled(PTR_DAT_005832b4, s.draw_x, s.draw_y, s.draw_w, s.draw_h,
                            DAT_0073eab0[s.selection_value][s.draw_state]);
  if ((mode == 2) && (control->on_activate != 0))
  {
    control->on_activate(control);
  }
  return 1;
}

// FUNCTION: SHANDALAR 0x00428d2c
static int ActivateJournalExitButtonControl(AdvMenuControl *control)
{
  PlaySoundEffectOnChannel("x:sound\\button2.wav", 0xf, 100, 100, 0);
  g_stats_journal_menu_selection = control->selection_value;
  return 0;
}

// FUNCTION: SHANDALAR 0x00428d5e
static int RenderJournalStepButtonControl(AdvMenuControl *control, int mode)
{
  struct
  {
    int mouse_inside;
    int y;
    int x;
    int height;
    int width;
  } s;

  if (g_menu_render_guard == 0)
  {
    if ((g_mouse_x < control->x) || (control->width + control->x < g_mouse_x))
    {
      s.mouse_inside = 0;
    }
    else if ((g_mouse_y < control->y) || (control->y + control->height < g_mouse_y))
    {
      s.mouse_inside = 0;
    }
    else
    {
      s.mouse_inside = 1;
    }
    if (!s.mouse_inside)
    {
      return 0;
    }
  }
  if (control->state == 3)
  {
    return 0;
  }
  s.x = control->x;
  s.y = control->y;
  s.width = control->width;
  s.height = control->height;
  if ((mode == 2) && (control->on_activate != 0))
  {
    control->on_activate(control);
  }
  return 1;
}

// FUNCTION: SHANDALAR 0x00428e63
static int ActivateJournalStepButtonControl(AdvMenuControl *control)
{
  struct
  {
    int entry_count;
    int y;
    EncodedImage *encoded_image;
    int journal_index;
    AdvMenuControl *thumb_control;
  } s;

  s.entry_count = control->unk_30;
  s.thumb_control = control + 1;
  s.encoded_image = DAT_00746b04;
  UpdateMouseSnapshot();
  s.y = g_mouse_y_snapshot;
  s.y = MAX(control->y + s.thumb_control->height / 2, s.y);
  s.y = MIN(control->y + control->height - s.thumb_control->height / 2, s.y);
  s.journal_index = ((s.y - control->y - s.thumb_control->height / 2) * s.entry_count) / (control->height - s.thumb_control->height);
  DrawEncodedImageResampled(PTR_DAT_005832dc, control->x, control->y, control->width, control->height, DAT_0073ea94);
  DrawEncodedImageResampled(PTR_DAT_005832dc, control->x, s.y - s.thumb_control->height / 2, control->width,
                            ((int)s.encoded_image->height * control->width) / control->base_width, s.encoded_image);
  BlitGraphicsRect(PTR_DAT_005832dc, control->x, control->y, control->width, control->height, PTR_DAT_005832b4, control->x,
                   control->y);
  SelectStatsJournalEntry(s.journal_index);
  g_stats_journal_menu_selection = control->selection_value;
  return g_stats_journal_menu_selection;
}

// FUNCTION: SHANDALAR 0x00428fe8
static int RenderJournalScrollbar(AdvMenuControl *control)
{
  struct
  {
    int entry_count;
    int y;
    EncodedImage *encoded_image;
    AdvMenuControl *thumb_control;
  } s;

  s.entry_count = control->unk_30;
  s.thumb_control = control + 1;
  s.encoded_image = DAT_00746b04;
  s.y = control->y + s.thumb_control->height / 2;
  DrawEncodedImageResampled(PTR_DAT_005832dc, control->x, control->y, control->width, control->height, DAT_0073ea94);
  DrawEncodedImageResampled(PTR_DAT_005832dc, control->x, s.y - s.thumb_control->height / 2,
                            control->width, ((int)s.encoded_image->height * control->width) / control->base_width, s.encoded_image);
  BlitGraphicsRect(PTR_DAT_005832dc, control->x, control->y, control->width, control->height, PTR_DAT_005832b4, control->x,
                   control->y);
  return 0;
}

// FUNCTION: SHANDALAR 0x00429473
static int SelectStatsJournalEntry(int journal_index)
{
  struct
  {
    int requested_journal_index;
    unsigned int tile_y;
    unsigned int tile_x;
    int entry_type;
    int message_found;
    unsigned int entry_arg;
  } s;

  s.requested_journal_index = journal_index;
  s.message_found = 0;
  if (journal_index < 0)
  {
    journal_index = 0;
  }
  if (g_stats_journal_entry_count < journal_index)
  {
    journal_index = g_stats_journal_entry_count;
  }
  if (g_stats_journal_current_entry == journal_index)
  {
    return 0;
  }
  strcpy(g_ui_message_buffer, "");
  while (g_ui_message_buffer[0] == 0)
  {
    s.entry_type = g_journal_entries[journal_index].type;
    s.entry_arg = g_journal_entries[journal_index].arg;
    s.tile_x = g_journal_entries[journal_index].tile_x;
    s.tile_y = g_journal_entries[journal_index].tile_y;
    if (g_journal_entries[journal_index].type == JOURNAL_ENTRY_NONE)
    {
      return 0;
    }
    BlitGraphicsRect(PTR_DAT_005832dc, 0, 0, global_screen_width, global_screen_height, PTR_DAT_005832b4, 0, 0);
    strcpy(g_ui_message_buffer, "");
    BuildStatsJournalEntryMessage(s.entry_type, s.entry_arg, s.tile_x, s.tile_y);
    if (g_ui_message_buffer[0] != '\0')
    {
      s.message_found = 1;
    }
    journal_index = journal_index + 1;
  }
  g_stats_journal_previous_marker_x = -1;
  g_stats_journal_previous_marker_y = g_stats_journal_previous_marker_x;
  if (journal_index - 1 == g_stats_journal_current_entry)
  {
    return 0;
  }

  AnimateStatsJournalMarkerToTile(s.tile_x, s.tile_y);
  g_stats_journal_current_entry = journal_index - 1;
}

// FUNCTION: SHANDALAR 0x004295f7
static void BuildStatsJournalEntryMessage(int entry_type, unsigned int entry_arg, int tile_x, int tile_y)
{
  struct
  {
    char nearest_town_name[300];
    int card_id;
    char formatted_message[500];
  } s;

  switch (entry_type)
  {
  case 1:
    if ((entry_arg & 0x80) != 0)
    {
      sprintf(s.formatted_message, gs_logstrings_0077c9a0[0], BuildTownDisplayName(entry_arg & 0x7f));
    }
    else
    {
      sprintf(s.formatted_message, gs_logstrings_0077c9a0[1], BuildTownDisplayName(entry_arg & 0x7f));
    }
    break;
  case 7:
    sprintf(s.formatted_message, gs_logstrings_0077c9a0[2], BuildTownDisplayName(entry_arg));
    break;
  case 13:
    strcpy(g_ui_message_buffer, "");
    FindNearestTownIndex(tile_x, tile_y);
    strcpy(s.nearest_town_name, g_ui_message_buffer);
    sprintf(s.formatted_message, gs_logstrings_0077c9a0[3], s.nearest_town_name);
    break;
  case 2:
    if ((entry_arg & 0x80) != 0)
    {
      sprintf(s.formatted_message, gs_logstrings_0077c9a0[4], BuildCreatureNameWithArticle(entry_arg & 0x7f));
    }
    else
    {
      sprintf(s.formatted_message, gs_logstrings_0077c9a0[5], BuildCreatureNameWithArticle(entry_arg & 0x7f));
    }
    break;
  case 3:
    sprintf(s.formatted_message, gs_logstrings_0077c9a0[6], GetDungeonName(entry_arg & 0x7f));
    break;
  case 4:
    if ((entry_arg & 0x80) != 0)
    {
      FormatMessageFromStringStripCarriageReturns(s.formatted_message, 0x1000, gs_logstrings_0077c9a0[7], GetDungeonName(entry_arg & 0x7f),
                                                  gs_cityname_manacastle_0077de00[(entry_arg & 0x7f) + 1]);
    }
    else
    {
      FormatMessageFromStringStripCarriageReturns(s.formatted_message, 0x1000, gs_logstrings_0077c9a0[8], GetDungeonName(entry_arg & 0x7f),
                                                  gs_cityname_manacastle_0077de00[(entry_arg & 0x7f) + 1]);
    }
    break;
  case 5:
    if ((int)entry_arg < 5)
    {
      strcpy(s.formatted_message, "");
    }
    else
    {
      sprintf(s.formatted_message, gs_logstrings_0077c9a0[9], FUN_005307c3(entry_arg));
    }
    break;
  case 6:
    sprintf(s.formatted_message, gs_logstrings_0077c9a0[10], gs_worldmagic_names_00780660[entry_arg]);
    break;
  case 8:
  case 9:
  case 10:
  case 11:
  case 12:
    s.card_id = (entry_type - 8) * 0x100 + entry_arg;
    sprintf(s.formatted_message, gs_logstrings_0077c9a0[11], global_cards_data[s.card_id].name);
    break;
  case 18:
    switch (entry_arg & 0xffff)
    {
    case 1:
    case 2:
      strcpy(s.formatted_message, gs_logstrings_0077c9a0[12]);
      break;
    case 3:
      strcpy(s.formatted_message, gs_logstrings_0077c9a0[13]);
      break;
    case 4:
      sprintf(s.formatted_message, gs_logstrings_0077c9a0[14], BuildCreatureNameWithArticle((int)entry_arg >> 16));
      break;
    case 5:
      sprintf(s.formatted_message, gs_logstrings_0077c9a0[15], BuildTownDisplayName(FindNearestTownIndex(tile_x, tile_y)));
      break;
    default:
      strcpy(s.formatted_message, "");
      break;
    }
    break;
  default:
    strcpy(s.formatted_message, "");
    break;
  }
  strcpy(g_ui_message_buffer, s.formatted_message);
}

// FUNCTION: SHANDALAR 0x00429a62
static void AnimateStatsJournalMarkerToTile(unsigned int tile_x, unsigned int tile_y)
{
  struct
  {
    int line_height;
    int text_width;
    int delta_y;
    int delta_x;
    int target_y;
    int previous_y;
    int target_x;
    int previous_x;
  } s;

  if (g_stats_journal_previous_marker_y == -1)
  {
    g_stats_journal_previous_marker_y = tile_x;
    g_stats_journal_previous_marker_x = tile_y;
    FUN_00550164(tile_x, tile_y, &s.target_x, &s.target_y);
    s.target_x = (global_screen_width * s.target_x) / 0x280;
    s.target_y = (global_screen_height * s.target_y) / 0x1e0;
    s.target_y += ScaleUiCoordinate(0x40);
  }
  else
  {
    FUN_00550164(g_stats_journal_previous_marker_y, g_stats_journal_previous_marker_x, &s.previous_x, &s.previous_y);
    s.previous_x = (s.previous_x * global_screen_width) / 0x280;
    s.previous_y = (global_screen_height * s.previous_y) / 0x1e0;
    s.previous_y += ScaleUiCoordinate(0x40);
    FUN_00550164(tile_x, tile_y, &s.target_x, &s.target_y);
    s.target_x = (global_screen_width * s.target_x) / 0x280;
    s.target_y = (global_screen_height * s.target_y) / 0x1e0;
    s.target_y += ScaleUiCoordinate(0x40);
    g_stats_journal_previous_marker_y = tile_x;
    g_stats_journal_previous_marker_x = tile_y;
    s.delta_x = s.target_x - s.previous_x;
    s.delta_y = s.target_y - s.previous_y;
    tile_x = s.previous_x;
    tile_y = s.previous_y;

    if (abs(s.delta_y) < abs(s.delta_x))
    {
      while (tile_x != s.target_x)
      {
        FillGraphicsRect(PTR_DAT_005832b4, tile_x, tile_y, 2, 2, 0xff);
        DelayUiTicks(5);
        if ((tile_x & 1) != 0)
        {
          FillGraphicsRect(PTR_DAT_005832b4, tile_x, tile_y, 2, 2, 0);
        }
        else
        {
          BlitGraphicsRect(PTR_DAT_005832dc, tile_x, tile_y, 2, 2, PTR_DAT_005832b4, tile_x, tile_y);
        }
        tile_x += SignNonZero(s.delta_x);
        tile_y = s.previous_y + (abs(tile_x - s.previous_x) * s.delta_y) / abs(s.delta_x);
      }
    }
    else
    {
      while (tile_y != s.target_y)
      {
        FillGraphicsRect(PTR_DAT_005832b4, tile_x, tile_y, 2, 2, 0xff);
        DelayUiTicks(5);
        if ((tile_y & 1) != 0)
        {
          FillGraphicsRect(PTR_DAT_005832b4, tile_x, tile_y, 2, 2, 0);
        }
        else
        {
          BlitGraphicsRect(PTR_DAT_005832dc, tile_x, tile_y, 2, 2, PTR_DAT_005832b4, tile_x, tile_y);
        }
        tile_y += SignNonZero(s.delta_y);
        tile_x = s.previous_x + (abs(tile_y - s.previous_y) * s.delta_x) / abs(s.delta_y);
      }
    }
  }

  g_stats_journal_marker_x = s.target_x;
  g_stats_journal_marker_y = s.target_y;
  if (g_ui_message_buffer[0] == '\0')
  {
    return;
  }

  tile_x = ClampIntToRange(s.target_x - 0x50, 0, 0xa0);
  tile_y = ClampIntToRange(s.target_y - 10, 0, 0xbf);
  s.text_width = MeasureMultilineTextWidth(PTR_DAT_005832b4, g_ui_message_buffer);
  s.line_height = GetFontLineHeight(PTR_DAT_005832b4->font_slot);
  PTR_DAT_00583304->font_slot = PTR_DAT_005832b4->font_slot;
  BlitGraphicsRect(PTR_DAT_005832dc, (s.target_x - 2) - s.text_width / 2, s.target_y - 2, s.text_width + 2, s.line_height + 2,
                   PTR_DAT_00583304, 0, 0xa0);
  ApplyPortraitTintMap(PTR_DAT_00583304, 0, 0xa0, s.text_width + 2, s.line_height + 2, 0x3f3f3f, 1);
  DrawFormattedTextShadowedCentered(PTR_DAT_00583304, 0xff, s.text_width / 2 + 1, s.line_height / 2 + 0xa1, g_ui_message_buffer);
  BlitGraphicsRect(PTR_DAT_00583304, 0, 0xa0, s.text_width + 2, s.line_height + 2, PTR_DAT_005832b4,
                   (s.target_x - 2) - s.text_width / 2, s.target_y - 2);
}

// FUNCTION: SHANDALAR 0x00429161
void RunStatsWorldMapJournalMenu(void)
{
  struct
  {
    int key_code;
    int menu_context;
    int journal_index;
  } s;

  ShowWorldMapScreen(4);
  BlitGraphicsRect(PTR_DAT_005832b4, 0, 0, global_screen_width, global_screen_height, PTR_DAT_005832dc, 0, 0);
  g_stats_journal_previous_marker_x = -1;
  g_stats_journal_previous_marker_y = g_stats_journal_previous_marker_x;
  for (s.journal_index = 0; s.journal_index < 10000; s.journal_index = s.journal_index + 1)
  {
    if (g_journal_entries[s.journal_index].type == JOURNAL_ENTRY_NONE)
    {
      break;
    }
  }
  g_stats_journal_entry_count = s.journal_index;
  g_stats_journal_menu_controls[1].unk_30 = g_stats_journal_entry_count;
  g_stats_journal_current_entry = -1;
  ScaleMenuControlsForScreen(g_stats_journal_menu_controls, 3);
  s.menu_context = BeginMenuContext();
  ResetMenuContext(s.menu_context);
  g_stats_journal_menu_controls[2].state = 3;
  AddMenuControlsToContext(g_stats_journal_menu_controls, 3, s.menu_context);
  g_menu_render_guard = 1;
  PTR_DAT_005832b4->page_number = 1;
  RenderJournalExitButtonControl(&g_stats_journal_menu_controls[0], 0);
  RenderJournalScrollbar(&g_stats_journal_menu_controls[1]);
  g_menu_render_guard = 0;
  PTR_DAT_005832b4->page_number = 0;
  SelectStatsJournalEntry(0);

retry:
  ConsumeUiTickCount();
  g_stats_journal_menu_selection = -1;
  while (g_stats_journal_menu_selection == -1)
  {
    if ((((GetUiTickCount() % 0x14) == 0) & ((GetUiTickCount() % 10) == 0)) != 0)
    {
      FillGraphicsRect(PTR_DAT_005832b4, g_stats_journal_marker_x - 2, g_stats_journal_marker_y - 2, 5, 5, 0xff);
    }
    if ((((GetUiTickCount() % 10) == 0) & (GetUiTickCount() % 0x14)) != 0)
    {
      BlitGraphicsRect(PTR_DAT_005832dc, g_stats_journal_marker_x - 2, g_stats_journal_marker_y - 2, 5, 5, PTR_DAT_005832b4,
                       g_stats_journal_marker_x, g_stats_journal_marker_y);
    }
    UpdateMouseSnapshot();
    UpdateMenuControlSelection(g_mouse_x_snapshot, g_mouse_y_snapshot, g_mouse_button_down_mask);
    if ((g_stats_journal_menu_selection == -1) && ((g_mouse_button_down_mask != 0) || (IsKeyInputQueueEmpty() == 0)))
    {
      s.key_code = PopQueuedKeyInput();
      if (((g_mouse_button_down_mask & 1) != 0) || (s.key_code == 0x5000))
      {
        SelectStatsJournalEntry(g_stats_journal_current_entry + 1);
      }
      else
      {
        if (((g_mouse_button_down_mask & 2) != 0) || (s.key_code == 0x4800))
        {
          SelectStatsJournalEntry(g_stats_journal_current_entry - 1);
        }
      }
      ClearInputAndWaitForMouseRelease();
    }
  }

  switch (g_stats_journal_menu_selection)
  {
  case 4:
    ClearInputAndWaitForMouseRelease();
    EndMenuContext();
    FreeSpriteBlob(DAT_0073eab0[0][0]);
    break;
  case 1:
    goto retry;
    break;
  }
}

// FUNCTION: SHANDALAR 0x005227e3
void __cdecl ShowStatWinDialog(void *statwin_data, int mode)
{
  if (g_statwin_exports_by_ordinal[0] != 0)
  {
    ((void(__cdecl *)(void *, int))g_statwin_exports_by_ordinal[0])(statwin_data, mode);
  }
}

// FUNCTION: SHANDALAR 0x005344b6
void DrawDeckAnalysisPercentCentered(int value, int center_x, int y, int color_index)
{
  strcpy(g_ui_message_buffer, "");
  strcat(g_ui_message_buffer, _itoa((value * 100) / DAT_00650264, DAT_0093a870, 10));
  strcat(g_ui_message_buffer, "%");
  DrawScaledCenteredTextNoShadow(g_ui_message_buffer, center_x, y, color_index);
}

// FUNCTION: SHANDALAR 0x00431002
void DrawScaledTextNoShadow(char *text, int x, int y, int color_index)
{
  x = (x * global_screen_width) / 0x140;
  y = (y * global_screen_height) / 0xf0;
  DrawTextLineNoShadow(text, x, y, color_index);
}

// FUNCTION: SHANDALAR 0x00549002
int RunAdventureStatsMenu(void)
{
  int world_magic_slot_index;
  struct
  {
    FacemakerWindowBounds portrait_work_page;
    FacemakerWindowBounds *portrait_work_page_ptr;
    int menu_context;
    int colors[5];
    int button_index;
  } s;

  s.portrait_work_page.page_number = 4;
  s.portrait_work_page.clip_left = 0;
  s.portrait_work_page.clip_top = 0;
  s.portrait_work_page.max_x = 800;
  s.portrait_work_page.max_y = 600;
  s.portrait_work_page.draw_shadow_enabled = 1;
  s.portrait_work_page.text_color = 0xf;
  s.portrait_work_page.unk_1c = 4;
  s.portrait_work_page.font_slot = 0;
  s.portrait_work_page_ptr = &s.portrait_work_page;
  s.colors[3] = 0xb7;
  s.colors[4] = 0x1c;
  s.colors[0] = 10;
  s.colors[1] = 8;
  s.colors[2] = 8;

  if (g_stats_text_loaded == 0)
  {
    g_stats_text_table =
        ((int *(__cdecl *)(FILE *, char *, int))LoadIniEscapedStringTable)(g_advbuttons_ini_file, s_statText_00590618,
                                                                           (int)g_ini_string_scratch);
    g_stats_text_loaded = 1;
  }

  AnimatePaletteToColor(0, g_default_palette_fade_steps);
  FillGraphicsRect(PTR_DAT_005832b4, 0, 0, global_screen_width, global_screen_height, 0);
  LoadPcxIntoPageNoPalette("advfac64.pic");
  LoadPcxIntoPage(1, "statbut1.pic");
  BeginSpriteEncodeSession();
  for (world_magic_slot_index = 0; world_magic_slot_index < 5; world_magic_slot_index = world_magic_slot_index + 1)
  {
    g_stats_menu_color_sprites[world_magic_slot_index] =
        EncodeSpriteFromPage(1, world_magic_slot_index * 0x31 + 1, 0x4c, 0x30, 0x30);
  }

  PTR_DAT_005832dc->font_slot = 6;
  for (world_magic_slot_index = 0; world_magic_slot_index < 3; world_magic_slot_index = world_magic_slot_index + 1)
  {
    SetFontStyleSize(6, 9 + (((unsigned int)(world_magic_slot_index - 2) < 1) ? -1 : 0));
    DrawFormattedTextNoShadowCentered(PTR_DAT_005832dc, s.colors[world_magic_slot_index], world_magic_slot_index * 0x55 + 0x2b, 0xc,
                                      "%s", (char *)g_stats_text_table[0]);
    g_stats_menu_button_sprites.normal[world_magic_slot_index] =
        EncodeSpriteFromPage(1, world_magic_slot_index * 0x55 + 1, 1, 0x54, 0x18);
    DrawFormattedTextNoShadowCentered(PTR_DAT_005832dc, s.colors[world_magic_slot_index], world_magic_slot_index * 0x4a + 0x26, 0x25,
                                      "%s", (char *)g_stats_text_table[1]);
    g_stats_menu_button_sprites.highlight[world_magic_slot_index] =
        EncodeSpriteFromPage(1, world_magic_slot_index * 0x4a + 1, 0x1a, 0x49, 0x18);
    DrawFormattedTextNoShadowCentered(PTR_DAT_005832dc, s.colors[world_magic_slot_index], world_magic_slot_index * 0x32 + 0x1a, 0x3e,
                                      "%s", (char *)g_stats_text_table[2]);
    g_stats_menu_button_sprites.pressed[world_magic_slot_index] =
        EncodeSpriteFromPage(1, world_magic_slot_index * 0x32 + 1, 0x33, 0x31, 0x18);
  }
  g_stats_menu_scrollbar_sprite = EncodeSpriteFromPage(1, 1, 0x7d, 0x19, 0xa1);
  g_stats_menu_scroll_thumb_sprite = EncodeSpriteFromPage(1, 0x1b, 0x7d, 0xf, 0x12);
  FinalizeSpriteEncodeSession();
  ScaleMenuControlsForScreen(g_stats_menu_controls, 10);
  g_stats_menu_controls[4].state = 3;
  g_stats_creature_page_index = 0;

  while (1)
  {
    RenderAdventureStatsOverview(s.portrait_work_page_ptr, s.colors);

    switch (global_screen_width)
    {
    case 0x280:
      LoadPcxIntoPageOpaque(1, "creatures640.pic");
      g_stats_creature_sheet_resolution = 0;
      break;
    case 800:
      LoadPcxIntoPageOpaque(1, "creatures800.pic");
      g_stats_creature_sheet_resolution = 1;
      break;
    case 0x400:
      LoadPcxIntoPageOpaque(1, "creatures1024.pic");
      g_stats_creature_sheet_resolution = 2;
      break;
    }
    g_stats_creature_page_index = 0;
    RenderStatsCreatureGridPage(g_stats_creature_page_index);
    RenderStatsCreatureScrollbar(&g_stats_menu_controls[4]);

  retry:
    s.menu_context = BeginMenuContext();
    ResetMenuContext(s.menu_context);
    AddMenuControlsToContext(g_stats_menu_controls, 0x16, s.menu_context);
    g_menu_render_guard = 1;
    for (s.button_index = 0; s.button_index < 3; s.button_index = s.button_index + 1)
    {
      RenderStatsTabButtonControl(&g_stats_menu_controls[s.button_index], 0);
    }
    for (s.button_index = 10; s.button_index < 0x16; s.button_index = s.button_index + 1)
    {
      RenderBasicImageMenuControl(&g_stats_menu_controls[s.button_index], 0);
    }
    g_menu_render_guard = 0;
    g_stats_menu_selection = -1;
    while (g_stats_menu_selection == -1)
    {
      UpdateMouseSnapshot();
      UpdateMenuControlSelection(g_mouse_x_snapshot, g_mouse_y_snapshot, g_mouse_button_down_mask);
    }
    EndMenuContext();

    switch (g_stats_menu_selection)
    {
    case 1:
      ClearInputAndWaitForMouseRelease();
      ShowStatsWindow(0, -1);
      break;
    case 2:
      RunStatsWorldMapJournalMenu();
      break;
    case 3:
      FreeSpriteBlob(g_stats_menu_color_sprites[0]);
      AnimatePaletteToColor(0, g_default_palette_fade_steps);
      LoadPcxIntoPageNoPalette("advfac64.pic");
      return 0;
    case 4:
    case 5:
      goto retry;
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
      if (g_world_magic_town_flags[g_stats_menu_selection - 10] != 0)
      {
        AnimatePaletteToColor(0, g_default_palette_fade_steps);
        ShowStatsWindow(0x101, g_stats_menu_selection - 9);
      }
      ClearInputAndWaitForMouseRelease();
      if (g_world_magic_town_flags[g_stats_menu_selection - 10] == 0)
      {
        goto retry;
      }
      break;
    case 15:
    case 16:
    case 17:
    case 18:
    case 19:
    case 20:
    case 21:
    case 22:
    case 23:
    case 24:
    case 25:
    case 26:
      ShowWorldMagicStatsDetail(g_stats_menu_selection - 0xf);
      break;
    }
  }
}

// FUNCTION: SHANDALAR 0x0056335f
int ShowStatsWindow(int mode, int highlight)
{
  struct
  {
    unsigned int color_order[7];
    int mapped_color;              // ebp - 0x20
    int duel_win_count;            // ebp - 0x1c
    int scan_index;                // ebp - 0x18
    int wizard_color_index;        // ebp - 0x14
    int old_timer_thread_priority; // ebp - 0x10
    int required_duel_wins;        // ebp - 0xc
    int ruled_town_count;          // ebp - 0x8
    int old_main_thread_priority;  // ebp - 0x4
  } s;

  AnimatePaletteToColor(0, g_default_palette_fade_steps);
  ClearGraphicsPageWithPaletteColor(0, 0);
  LoadPcxIntoPageNoPalette("advfac64.pic");
  s.old_timer_thread_priority = GetThreadPriority(g_timer_thread_handle);

  for (s.wizard_color_index = 0; s.wizard_color_index < 5; s.wizard_color_index = s.wizard_color_index + 1)
  {
    s.duel_win_count = 0;
    s.ruled_town_count = 0;
    s.mapped_color = FUN_0056302b(s.wizard_color_index + 1);
    for (s.scan_index = 0; s.scan_index < 0x80; s.scan_index = s.scan_index + 1)
    {
      if ((g_town_slots[s.scan_index].status_and_ruling_wizard & 0xff00) &&
          (((int)g_town_slots[s.scan_index].status_and_ruling_wizard >> 8) - 1 == s.wizard_color_index))
      {
        s.ruled_town_count = s.ruled_town_count + 1;
      }
    }
    g_statwin_data.town_count_by_color[s.mapped_color] = (unsigned char)s.ruled_town_count;
    if (g_world_magic_town_flags[s.wizard_color_index] != 0)
    {
      g_statwin_data.progress_by_color[s.mapped_color] = 0;
    }
    else
    {
      s.required_duel_wins = s.color_order[6] = g_shandalar_difficulty * s.ruled_town_count + g_shandalar_difficulty * 5 + 0x1e;
      for (s.scan_index = 0; s.scan_index < 1000; s.scan_index = s.scan_index + 1)
      {
        if (g_duel_victory_log[s.scan_index] == '\0')
          break;

        if (((int)(char)g_duel_victory_log[s.scan_index] >> 4) == s.wizard_color_index + 1)
        {
          s.duel_win_count = s.duel_win_count + 1;
        }
      }
      g_statwin_data.duel_wins_by_color[s.mapped_color] = s.duel_win_count;
      s.color_order[6] = MAX(g_shandalar_difficulty * 5 + 0x14, s.required_duel_wins - s.duel_win_count);

      g_statwin_data.progress_by_color[s.mapped_color] = 0x1e - (s.required_duel_wins - s.color_order[6]);
    }
  }

  g_statwin_data.highlighted_wizard_color = 0;
  g_statwin_data.highlighted_creature_type = 0;
  g_statwin_data.highlighted_lair_color = 0;
  if ((mode & 0xff) == 1)
  {
    if (((unsigned int)mode & 0x100U) == 0)
    {
      g_world_magic_town_flags[highlight - 1] = 1;
    }
    g_statwin_data.highlighted_wizard_color = (unsigned char)FUN_0056302b(highlight);
    g_statwin_data.progress_by_color[g_statwin_data.highlighted_wizard_color] = 0;
    mode &= 0xff;
  }
  if (mode == 2)
  {
    s.color_order[0] = (unsigned int)highlight >> 0x10;
    s.color_order[1] = 4;
    s.color_order[2] = 2;
    s.color_order[3] = 3;
    s.color_order[4] = 1;
    s.color_order[5] = 0;
    highlight &= 0xffff;
    if (g_shandalar_monster_definitions[highlight].color_mask == -1)
    {
      mode = 0;
    }
    else
    {
      g_statwin_data.highlighted_creature_type = (unsigned char)highlight;
      g_statwin_data.highlighted_wizard_color = (unsigned char)s.color_order[s.color_order[0]];
    }
  }
  if (mode == 3)
  {
    g_statwin_data.highlighted_lair_color = (unsigned char)highlight;
  }
  ShowStatWinDialog(&g_statwin_data, mode);
  SetThreadPriority(g_timer_thread_handle, s.old_timer_thread_priority);
  SetThreadPriority(g_main_thread_handle, s.old_main_thread_priority);
  SetForegroundWindow(g_main_window_hwnd);
  BringWindowToTop(g_main_window_hwnd);
  SetFocus(g_main_window_hwnd);
  LoadPcxIntoPageNoPalette("advfac64.pic");
  ReadPalette("todpal.tr", (char *)0);
  SelectPalette(g_graphics_pages[0]->hTempDC, g_palette_handle, 0);
  RealizePalette(g_graphics_pages[0]->hTempDC);
  return 0;
}

// FUNCTION: SHANDALAR 0x00533ccd
void AnalyzeDeckAndMaybeShowReport(int show_ui)
{
  struct
  {
    int land_color_count[8];
    int card_type_count_by_color[7][8];
    int color_index;
    int column_index;
    int deck_index;
    int y;
    int x;
    unsigned int card_id_masked;
    int max_creature_count;
    int rarity_count[3];
  } s;

  DAT_00650264 = 0;
  for (s.color_index = 0; s.color_index <= 7; s.color_index = s.color_index + 1)
  {
    s.land_color_count[s.color_index] = 0;
    for (s.column_index = 0; s.column_index < 7; s.column_index = s.column_index + 1)
    {
      s.card_type_count_by_color[s.column_index][s.color_index] = 0;
    }
  }
  for (s.color_index = 0; s.color_index < 3; s.color_index = s.color_index + 1)
  {
    s.rarity_count[s.color_index] = 0;
  }
  for (s.deck_index = 0; s.deck_index < 500; s.deck_index = s.deck_index + 1)
  {
    if ((deck[s.deck_index] & 0x4000) == 0)
    {
      DAT_00650264 = DAT_00650264 + 1;
      s.card_id_masked = deck[s.deck_index] & 0xfff;
      s.color_index = (int)global_cards_data[s.card_id_masked].color;
      if (global_cards_data[s.card_id_masked].rarity == '\x03')
      {
        s.rarity_count[2] = s.rarity_count[2] + 1;
      }
      else if (global_cards_data[s.card_id_masked].rarity == '\x02')
      {
        s.rarity_count[1] = s.rarity_count[1] + 1;
      }
      else
      {
        s.rarity_count[0] = s.rarity_count[0] + 1;
      }
      if ((global_cards_data[s.card_id_masked].type & 1) != 0)
      {
        for (s.column_index = 1; s.column_index < 7; s.column_index = s.column_index + 1)
        {
          if (((1 << (byte)s.column_index) & (int)global_cards_data[s.card_id_masked].color) != 0)
          {
            s.land_color_count[s.column_index] = s.land_color_count[s.column_index] + 1;
            s.land_color_count[7] = s.land_color_count[7] + 1;
          }
        }
      }
      s.color_index = single_color_test_bit_to_color_t(s.color_index);
      switch (global_cards_data[s.card_id_masked].type)
      {
      case '\x01':
        s.card_type_count_by_color[0][s.color_index] = s.card_type_count_by_color[0][s.color_index] + 1;
        s.card_type_count_by_color[0][0] = s.card_type_count_by_color[0][0] + 1;
        break;
      case '\x02':
        if (4 < (int)global_cards_data[s.card_id_masked].power + (int)global_cards_data[s.card_id_masked].toughness)
        {
          s.card_type_count_by_color[2][s.color_index] = s.card_type_count_by_color[2][s.color_index] + 1;
          s.card_type_count_by_color[2][0] = s.card_type_count_by_color[2][0] + 1;
        }
        else
        {
          s.card_type_count_by_color[1][s.color_index] = s.card_type_count_by_color[1][s.color_index] + 1;
          s.card_type_count_by_color[1][0] = s.card_type_count_by_color[1][0] + 1;
        }
        break;
      case '\x04':
        s.card_type_count_by_color[3][s.color_index] = s.card_type_count_by_color[3][s.color_index] + 1;
        s.card_type_count_by_color[3][0] = s.card_type_count_by_color[3][0] + 1;
        break;
      case '\x08':
        s.card_type_count_by_color[4][s.color_index] = s.card_type_count_by_color[4][s.color_index] + 1;
        s.card_type_count_by_color[4][0] = s.card_type_count_by_color[4][0] + 1;
        break;
      case '\x10':
      case ' ':
        s.card_type_count_by_color[5][s.color_index] = s.card_type_count_by_color[5][s.color_index] + 1;
        s.card_type_count_by_color[5][0] = s.card_type_count_by_color[5][0] + 1;
        break;
      case '@':
      case 'B':
        s.card_type_count_by_color[6][s.color_index] = s.card_type_count_by_color[6][s.color_index] + 1;
      }
    }
  }
  if (show_ui != 0)
  {
    ClearGraphicsPageWithPaletteColor(0, 0);
    PTR_DAT_005832b4->font_slot = 4;
    s.x = 0x10;
    s.y = s.x;
    sprintf(g_ui_message_buffer, gs_analyze_0074b870[0], DAT_00650264);
    DrawScaledTextNoShadow(g_ui_message_buffer, s.x, s.y, 0xf6);
    DrawScaledCenteredTextNoShadow(gs_analyze_0074b870[1], 0x80, s.y, 0xff);
    DrawScaledCenteredTextNoShadow(gs_analyze_0074b870[2], 0xb0, s.y, 0xff);
    DrawScaledCenteredTextNoShadow(gs_analyze_0074b870[3], 0xd0, s.y, 0xff);
    DrawScaledCenteredTextNoShadow(gs_analyze_0074b870[4], 0xf0, s.y, 0xff);
    DrawScaledCenteredTextNoShadow(gs_analyze_0074b870[5], 0x110, s.y, 0xff);
    DrawScaledCenteredTextNoShadow(gs_analyze_0074b870[6], 0x130, s.y, 0xff);
    DrawGraphicsLine(PTR_DAT_005832b4, 0, s.y * 2 + 0x18, global_screen_width - 1, s.y * 2 + 0x18, 0xf4);
    DrawGraphicsLine(PTR_DAT_005832b4, 0x130, 0, 0x130, global_screen_height - 1, 0xf4);
    s.y = 0x20;
    DrawScaledTextNoShadow(gs_analyze_0074b870[7], s.x, s.y, 0xff);
    DrawDeckAnalysisPercentCentered(s.card_type_count_by_color[0][0], 0x80, s.y, 0xff);
    for (s.deck_index = 1; s.deck_index <= 5; s.deck_index = s.deck_index + 1)
    {
      DrawDeckAnalysisPercentCentered(s.land_color_count[s.deck_index], s.deck_index * 0x20 + 0x90, s.y, 0xff);
    }
    s.y = s.y + 0xc;
    for (s.column_index = 1; s.column_index < 6; s.column_index = s.column_index + 1)
    {
      switch (s.column_index)
      {
      case 1:
        strcpy(g_ui_message_buffer, gs_analyze_0074b870[8]);
        break;
      case 2:
        strcpy(g_ui_message_buffer, gs_analyze_0074b870[9]);
        break;
      case 3:
        strcpy(g_ui_message_buffer, gs_analyze_0074b870[10]);
        break;
      case 4:
        strcpy(g_ui_message_buffer, gs_analyze_0074b870[11]);
        break;
      case 5:
        strcpy(g_ui_message_buffer, gs_analyze_0074b870[12]);
        break;
      }
      DrawScaledTextNoShadow(g_ui_message_buffer, s.x, s.y, 0xff);
      DrawDeckAnalysisPercentCentered(s.card_type_count_by_color[s.column_index][0], 0x80, s.y, 0xff);
      for (s.deck_index = 1; s.deck_index <= 5; s.deck_index = s.deck_index + 1)
      {
        DrawDeckAnalysisPercentCentered(s.card_type_count_by_color[s.column_index][s.deck_index], s.deck_index * 0x20 + 0x90, s.y, 0xff);
      }
      s.y = s.y + 0xc;
    }
    DrawScaledTextNoShadow(gs_analyze_0074b870[0xd], s.x, s.y, 0xff);
    DrawDeckAnalysisPercentCentered(s.card_type_count_by_color[6][0], 0x80, s.y, 0xff);
    s.y = s.y + 0x18;
    s.x = 0x80;
    DrawScaledTextNoShadow(gs_analyze_0074b870[0xe], 0x80, s.y, 0xf6);
    strcat(g_ui_message_buffer, " ");
    DrawDeckAnalysisPercentCentered(s.rarity_count[0], s.x + 0x30, s.y, 0xf6);
    s.y = s.y + 8;
    DrawScaledTextNoShadow(gs_analyze_0074b870[0xf], s.x, s.y, 0xf6);
    strcat(g_ui_message_buffer, " ");
    DrawDeckAnalysisPercentCentered(s.rarity_count[1], s.x + 0x30, s.y, 0xf6);
    s.y = s.y + 8;
    DrawScaledTextNoShadow(gs_analyze_0074b870[0x10], s.x, s.y, 0xf6);
    strcat(g_ui_message_buffer, " ");
    DrawDeckAnalysisPercentCentered(s.rarity_count[2], s.x + 0x30, s.y, 0xf6);
    s.y = s.y + 8;
    WaitForInputEventUnlessBlocked();
  }
  _DAT_007483e4 = 0;
  s.max_creature_count = s.card_type_count_by_color[2][0];
  if (s.max_creature_count < s.card_type_count_by_color[3][0])
  {
    _DAT_007483e4 = 1;
    s.max_creature_count = s.card_type_count_by_color[3][0];
  }
  if (s.max_creature_count < s.card_type_count_by_color[5][0] + s.card_type_count_by_color[6][0])
  {
    _DAT_007483e4 = 2;
  }
}
