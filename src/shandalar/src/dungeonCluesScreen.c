#include <windows.h>
#include <stdio.h>
#include <string.h>

#include "defs.h"
#include "shandalar.h"
#include "magic/src/global_state.h"
#include "magic/src/global_strings.h"
#include "shandalar_global_strings.h"
#include "facemaker/src/facemaker_types.h"
#include <drawcardlib/src/pic.h>
#include "drawcardlib/Drawcardlib.h"

#pragma intrinsic(memset)

// Dungeon clues screens:
// - List screen (SHANDALAR 0x00509244)
// - Detail screen (SHANDALAR 0x0050caa0)
// Kept together since the list screen directly calls the detail screen.

/* External engine state (owned by shandalar.c / other modules) */
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
extern int g_world_scroll_cache_ready;
extern int g_world_ui_top_offset;
extern FacemakerWindowBounds *g_page0_window_bounds;
extern FacemakerWindowBounds *g_page1_window_bounds;
extern FacemakerWindowBounds *g_page2_window_bounds;

extern DIBSurface *g_graphics_pages[10];

/* Shared selection/activation scratch used by multiple screens (defined in cityInfoScreen.c) */
extern int g_adv_menu_selected_value;

/* Debug toggle used by multiple screens (defined in cityInfoScreen.c) */
extern int g_reveal_all_world_info;

/* Palette helper data (defined in shandalar.c) */
extern int g_default_palette_fade_steps;

/* Sprites loaded by startup code (owned by shandalar.c) */
extern EncodedImage *g_location_marker_sprite_entries[0x9e];
extern EncodedImage *g_castles_sprite_entries[20];

typedef struct DungeonCluesListSpriteStorage
{
  EncodedImage *button_sprites[3][4];
  EncodedImage *scrollbar_sprite;
} DungeonCluesListSpriteStorage;

/* Forward decls for locals defined later in this module */
extern DungeonCluesListSpriteStorage g_dungeon_clues_list_sprite_storage;
#define g_dungeon_clues_list_button_sprites g_dungeon_clues_list_sprite_storage.button_sprites
#define g_dungeon_clues_list_done_label_sprites g_dungeon_clues_list_sprite_storage.button_sprites[2]
#define g_dungeon_clues_scrollbar_sprite g_dungeon_clues_list_sprite_storage.scrollbar_sprite

extern EncodedImage *g_dungeon_clue_detail_done_button_sprites[3];
extern EncodedImage *g_dungeon_clue_detail_button_icon_sprites[3];

extern char g_dungeon_clues_list_done_keys[];
extern char g_dungeon_clues_list_done_alt_keys[];
extern char g_dungeon_clue_detail_done_keys[];
extern char g_dungeon_clue_detail_done_alt_keys[];

void ShowDungeonClueDetailScreen(int dungeon_index);

/* External functions */
int *LoadIniEscapedStringTable(FILE *ini_file, char *section_name, int unk1, int unk2);
void LoadPcxIntoPageNoPalette(char *path);
void LoadPcxIntoPage(int page_number, char *path);
void LoadPcxResource(int page_number, int x, int y, char *path, void *opaque);
void BeginSpriteEncodeSession(void);
EncodedImage *EncodeSpriteFromPage(int page_number, int x, int y, int width, int height);
void FinalizeSpriteEncodeSession(void);
void FreeSpriteBlob(void *memory);

int BeginMenuContext(void);
int ResetMenuContext(int context_index);
int AddMenuControlsToContext(AdvMenuControl *controls, int control_count, int context_index);
int EndMenuContext(void);
int SetCurrentMenuContextArrowNavigation(int allow_arrow_nav);
int RenderCurrentMenuContextControls(void);

int ScaleUiCoordinateFrom320(int value);
int ScaleUiCoordinate(int value);
int SetFontStyleSize(int font_slot, unsigned int point_size);
int GetFontStyleSize(int font_slot);
int MeasureMultilineTextWidth(FacemakerWindowBounds *dst, char *text);
int MeasureTextLineWidth(char *text);
int GetFontLineHeight(int font_slot);
void DrawTextLine(FacemakerWindowBounds *dst, int x, int y, char *text);
int DrawTextFormatted(FacemakerWindowBounds *dst, int text_color, int draw_shadow, int scale_to_screen, int center_x, int center_y, int x,
                      int y, int *format_and_args);
void DrawTextLineClamped(char *text, int x, int y, int color_index);
void DrawCenteredTextLineClamped(char *text, int center_x, int y, int color_index);

void DrawEncodedImageResampled(FacemakerWindowBounds *dst, int x, int y, int width, int height, EncodedImage *encoded_image);
void DrawEncodedImageUnscaled(FacemakerWindowBounds *dst, int x, int y, EncodedImage *encoded_image);
void FillGraphicsRect(FacemakerWindowBounds *window_bounds, int x, int y, int width, int height, unsigned int color_index);
void StretchBlitGraphicsRect(FacemakerWindowBounds *dst, int dst_x, int dst_y, int src_w, int src_h,
                             FacemakerWindowBounds *src, int src_x, int src_y, int copy_w, int copy_h);
void CopyGraphicsRect(FacemakerWindowBounds *src, int src_x, int src_y, int width, int height,
                      FacemakerWindowBounds *dst, int dst_x, int dst_y);
void BlitGraphicsRect(FacemakerWindowBounds *dst, int dst_x, int dst_y, int width, int height,
                      FacemakerWindowBounds *src, int src_x, int src_y);
void DrawGraphicsLine(FacemakerWindowBounds *window_bounds, int x1, int y1, int x2, int y2, int color_index);
AdvMenuRect *PushGraphicsClipRect(AdvMenuRect *saved_clip_rect, FacemakerWindowBounds *page, int x, int y, int width, int height);

void UpdateMouseSnapshot(void);
void PopNormalizedQueuedKeyInput(void);
int UpdateMenuControlSelection(int mouse_x, int mouse_y, int allow_activate_on_click);
void ClearInputAndWaitForMouseRelease(void);

void AnimatePaletteToColor(int mode, int color_index);
void PlaySoundEffectOnChannel(char *sound_path, int channel, int volume, int pitch_percent, int pan_percent);
void DrawFormattedTextShadowedCentered(FacemakerWindowBounds *window, int color_index, int x, int y, char *format, ...);
void DrawFormattedTextNoShadow(FacemakerWindowBounds *dst, int text_color, int x, int y, char *format, ...);
void GetEncodedImageSpanXExtents(EncodedImage *image, unsigned int *out_min_x, int *out_max_x);
int DrawEncodedImageResampledFitBoxCentered(FacemakerWindowBounds *dst, int x, int y, int box_w, int box_h,
                                            EncodedImage *image);
char *BuildTownDisplayName(int town_index);
void ResetWorldDrawQueue(void);
void UpdateWorldViewportBuffer(int world_x, int world_y);
void DrawQueuedWorldSprites(void);
void DestroyCachedCardArt(void);
unsigned int FindCardIndexByCsvid(int csvid);

/* Card rendering (drawcardlib) */
extern unsigned char global_raw_cards_storage[];
extern card_data_t global_cards_data[];
extern EncodedImage *g_endtop_banner_sprite;

/* Other helpers referenced by clue detail screen */
void BlitGraphicsRectScaledFrom320x240(FacemakerWindowBounds *dst, int dst_x, int dst_y, int width, int height,
                                       FacemakerWindowBounds *src, int src_x, int src_y);
void ResetWorldDrawQueue(void);
void DrawQueuedWorldSprites(void);
void UpdateWorldViewportBuffer(int world_x, int world_y);

unsigned int MarkPathConnection(int world_x, int world_y, int unused);
char *BuildTownDisplayName(int town_index);
int LoadTextSectionLines(char *filename, char *section);
DWORD FormatMessageFromStringStripCarriageReturns(char *dst, DWORD dst_len, LPCVOID format, ...);
unsigned int GetWorldMapPixelFlags(int x, int y);

/*
 * These two helpers are used as enable/disable visuals for arrow buttons.
 * They are implemented (with these names) in cityInfoScreen.c.
 */
int RenderAdvMenuControlDisabled(AdvMenuControl *control);
int RenderAdvMenuControlNormally(AdvMenuControl *control);

/*
 * Local helpers (new in this module).
 */

// FUNCTION: SHANDALAR 0x00508bf3
int IsPointInRectExclusive(int x, int y, int rect_x, int rect_y, int rect_w, int rect_h)
{
  if ((rect_x < x) && (x < rect_x + rect_w) && (rect_y < y) && (y < rect_y + rect_h))
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

// FUNCTION: SHANDALAR 0x00508c4a
EncodedImage *__cdecl EncodeMergedDungeonIconSprite(FacemakerWindowBounds *unused_page, int unused_x, int unused_y, int unused_w, int unused_h,
                                                    EncodedImage **sprite_pair)
{
  struct
  {
    int max_y;
    int max_x_a;
    EncodedImage *sprite_b;
    EncodedImage *result;
    int min_x_b;
    int min_y;
    int min_x_a;
    EncodedImage *return_value;
    EncodedImage *sprite_a;
    int max_x_b;
  } s;

  (void)unused_page;
  (void)unused_x;
  (void)unused_y;
  (void)unused_w;
  (void)unused_h;

  s.sprite_a = sprite_pair[0];
  s.sprite_b = sprite_pair[1];
  if (s.sprite_a == (EncodedImage *)0)
  {
    return (EncodedImage *)0;
  }

  FillGraphicsRect(g_page2_window_bounds, 0, 0x80, (int)s.sprite_a->width + 1, (int)s.sprite_a->height + 1, 0);
  DrawEncodedImageUnscaled(g_page2_window_bounds, 0, 0x80, sprite_pair[0]);
  DrawEncodedImageUnscaled(g_page2_window_bounds, 0, 0x80, sprite_pair[1]);

  GetEncodedImageSpanXExtents(sprite_pair[0], (unsigned int *)&s.min_x_a, &s.max_x_a);
  GetEncodedImageSpanXExtents(sprite_pair[1], (unsigned int *)&s.min_x_b, &s.max_x_b);

  s.min_x_a = (s.min_x_a <= s.min_x_b) ? s.min_x_a : s.min_x_b;
  s.max_x_a = (s.max_x_b > s.max_x_a) ? s.max_x_b : s.max_x_a;

  if (0xfa < (int)(s.max_x_a - s.min_x_a))
  {
    s.max_x_a = s.min_x_a + 0xfa;
  }

  s.min_y = ((int)s.sprite_a->first_row <= (int)s.sprite_b->first_row) ? (int)s.sprite_a->first_row : (int)s.sprite_b->first_row;
  s.max_y = (((int)s.sprite_a->first_row + (int)s.sprite_a->row_count) < ((int)s.sprite_b->first_row + (int)s.sprite_b->row_count))
                ? ((int)s.sprite_b->first_row + (int)s.sprite_b->row_count)
                : ((int)s.sprite_a->first_row + (int)s.sprite_a->row_count);

  s.result = EncodeSpriteFromPage(g_page2_window_bounds->page_number, s.min_x_a, s.min_y + 0x80,
                                  (s.max_x_a - s.min_x_a) + 1, (s.max_y - s.min_y) + 1);
  s.return_value = s.result;
  return s.result;
}

// FUNCTION: SHANDALAR 0x005081e0
char *__cdecl GetDungeonName(int dungeon_index)
{
  return gs_dungeon_names_00780820[dungeon_index];
}

// FUNCTION: SHANDALAR 0x0050a56d
int GetDefeatedWizardCountPlusOne(void)
{
  struct
  {
    int wins;
    int i;
  } s;

  s.wins = 1;
  for (s.i = 0; s.i < 6; s.i++)
  {
    if ((g_defeated_wizards_bitmap & (1 << (unsigned char)s.i)) != 0)
    {
      s.wins = s.wins + 1;
    }
  }

  return s.wins;
}

// FUNCTION: SHANDALAR 0x00508f45
int RenderDungeonCluesListButton(AdvMenuControl *control, int mode)
{
  int hovered;

  if (g_menu_render_guard == 0)
  {
    if ((control->x > g_mouse_x) || (g_mouse_x > control->x + control->width))
    {
      hovered = 0;
    }
    else if ((control->y > g_mouse_y) || (g_mouse_y > control->y + control->height))
    {
      hovered = 0;
    }
    else
    {
      hovered = 1;
    }

    if (!hovered)
    {
      return 0;
    }
  }

  if (control->state == 3)
  {
    return 0;
  }

  DrawEncodedImageResampled(g_page0_window_bounds, control->x, control->y, control->width, control->height,
                            g_dungeon_clues_list_button_sprites[control->data_value][mode]);

  if ((mode == 2) && (control->on_activate != (AdvMenuActivateCallback)0))
  {
    control->on_activate(control);
  }

  return 1;
}

// FUNCTION: SHANDALAR 0x00509064
int ActivateDungeonCluesListButton(AdvMenuControl *control)
{
  PlaySoundEffectOnChannel("x:sound\\button2.wav", 0xf, 100, 100, 0);
  g_adv_menu_selected_value = control->selection_value;
  return 0;
}

// FUNCTION: SHANDALAR 0x005089cb
int RenderDungeonClueDetailDoneButton(AdvMenuControl *control, int mode)
{
  int hovered;

  if (g_menu_render_guard == 0)
  {
    if ((control->x > g_mouse_x) || (g_mouse_x > control->x + control->width))
    {
      hovered = 0;
    }
    else if ((control->y > g_mouse_y) || (g_mouse_y > control->y + control->height))
    {
      hovered = 0;
    }
    else
    {
      hovered = 1;
    }

    if (!hovered)
    {
      return 0;
    }
  }

  if (control->state == 3)
  {
    return 0;
  }

  if (mode == 2)
  {
    DrawEncodedImageResampled(g_page1_window_bounds, 1, 1, control->width - 2, control->height - 2, g_dungeon_clue_detail_done_button_sprites[mode]);
    BlitGraphicsRect(g_page1_window_bounds, 0, 0, control->width + 1, control->height + 1, g_page0_window_bounds, control->x, control->y);
  }
  else
  {
    DrawEncodedImageResampled(g_page0_window_bounds, control->x, control->y, control->width, control->height, g_dungeon_clue_detail_done_button_sprites[mode]);
  }

  if (mode == 2 && control->on_activate != (AdvMenuActivateCallback)0)
  {
    control->on_activate(control);
  }

  return 1;
}

// FUNCTION: SHANDALAR 0x00508b57
int ActivateDungeonClueDetailDoneButton(AdvMenuControl *control)
{
  PlaySoundEffectOnChannel("x:sound\\button2.wav", 0xf, 100, 100, 0);
  g_adv_menu_selected_value = control->selection_value;
  return 0;
}

// FUNCTION: SHANDALAR 0x004f2407
void DrawAdventureCard(int card_index, int x, int y, int full_card, char *banner_label)
{
  struct
  {
    int card_height;
    int card_width;
    RECT clip;
    int saved_dc;
    int text_w;
  } s;

  if (full_card != 0)
  {
    s.card_width = ScaleUiCoordinateFrom320(0x50);
    s.card_height = ScaleUiCoordinateFrom320(0x70);
    if (0xf0 < y + 0x70)
    {
      y = 0x7f;
    }
  }
  else
  {
    s.card_width = ScaleUiCoordinateFrom320(0x30);
    s.card_height = ScaleUiCoordinateFrom320(0x30);
  }

  x = ScaleUiCoordinateFrom320(x);
  y = ScaleUiCoordinateFrom320(y);
  SetRect(&s.clip, x, y, x + s.card_width, y + s.card_height);

  s.saved_dc = SaveDC(g_graphics_pages[g_page0_window_bounds->page_number]->hTempDC);
  IntersectClipRect(g_graphics_pages[g_page0_window_bounds->page_number]->hTempDC, s.clip.left, s.clip.top, s.clip.right, s.clip.bottom);

  if (full_card != 0)
  {
    DrawFullCard(g_graphics_pages[g_page0_window_bounds->page_number]->hTempDC, &s.clip,
                 (card_ptr_t *)(global_raw_cards_storage + global_cards_data[card_index].id * 0x98), 0, 1, 1, gs_illus_00789130);
  }
  else
  {
    DrawSmallCard(g_graphics_pages[g_page0_window_bounds->page_number]->hTempDC, &s.clip,
                  (card_ptr_t *)(global_raw_cards_storage + global_cards_data[card_index].id * 0x98), 0, 1);
  }

  RestoreDC(g_graphics_pages[g_page0_window_bounds->page_number]->hTempDC, s.saved_dc);

  if ((banner_label != (char *)0) && (*banner_label != '\0'))
  {
    g_page0_window_bounds->font_slot = 1;
    s.text_w = MeasureTextLineWidth(banner_label);
    DrawEncodedImageResampled(g_page0_window_bounds, (x + s.card_width / 2) - s.text_w / 2 - 10, y - 0x18, s.text_w + 0x14, 0x14,
                              g_endtop_banner_sprite);
    DrawCenteredTextLineClamped(banner_label, x + s.card_width / 2, y - 0x14, 0xff);
  }
}

// FUNCTION: SHANDALAR 0x004f263b
void DrawAdventureCardSized(int card_index, int x, int y, int width, int height, int full_card, char *banner_label)
{
  struct
  {
    RECT clip;
    int saved_dc;
    int text_w;
  } s;

  if (full_card != 0)
  {
    width = ScaleUiCoordinateFrom320(width);
    height = ScaleUiCoordinateFrom320(height);
    if (0xf0 < y + 0x70)
    {
      y = 0x7f;
    }
  }
  else
  {
    width = ScaleUiCoordinateFrom320(width);
    height = ScaleUiCoordinateFrom320(height);
  }

  x = ScaleUiCoordinateFrom320(x);
  y = ScaleUiCoordinateFrom320(y);
  SetRect(&s.clip, x, y, x + width, y + height);

  s.saved_dc = SaveDC(g_graphics_pages[g_page0_window_bounds->page_number]->hTempDC);
  IntersectClipRect(g_graphics_pages[g_page0_window_bounds->page_number]->hTempDC, s.clip.left, s.clip.top, s.clip.right, s.clip.bottom);

  if (full_card != 0)
  {
    DrawFullCard(g_graphics_pages[g_page0_window_bounds->page_number]->hTempDC, &s.clip,
                 (card_ptr_t *)(global_raw_cards_storage + global_cards_data[card_index].id * 0x98), 0, 1, 1, gs_illus_00789130);
  }
  else
  {
    DrawSmallCard(g_graphics_pages[g_page0_window_bounds->page_number]->hTempDC, &s.clip,
                  (card_ptr_t *)(global_raw_cards_storage + global_cards_data[card_index].id * 0x98), 0, 1);
  }

  RestoreDC(g_graphics_pages[g_page0_window_bounds->page_number]->hTempDC, s.saved_dc);

  if ((banner_label != (char *)0) && (*banner_label != '\0'))
  {
    g_page0_window_bounds->font_slot = 1;
    s.text_w = MeasureTextLineWidth(banner_label);
    DrawEncodedImageResampled(g_page0_window_bounds, (x + width / 2) - s.text_w / 2 - 10, y - 0x18, s.text_w + 0x14, 0x14,
                              g_endtop_banner_sprite);
    DrawCenteredTextLineClamped(banner_label, x + width / 2, y - 0x14, 0xff);
  }
}

/*
 * Globals used by the dungeon clues list screen.
 */

// GLOBAL: SHANDALAR 0x00603a3c
int g_dungeon_clues_list_strings_loaded;
// GLOBAL: SHANDALAR 0x00603a40
int *g_dungeon_clues_list_strings;

// GLOBAL: SHANDALAR 0x00746e20
DungeonCluesListSpriteStorage g_dungeon_clues_list_sprite_storage;

// GLOBAL: SHANDALAR 0x0058c708
AdvMenuControl g_dungeon_clues_list_controls[3] = {
    // Up
    {24, 219, 31, 36, 24, 219, 31, 36, 1, (AdvMenuRenderCallback)RenderDungeonCluesListButton, (AdvMenuActivateCallback)ActivateDungeonCluesListButton, -1, 0, (char *)0, (char *)0, 0x4800, 0, {0, 0, 0, 0}},
    // Down
    {24, 260, 31, 36, 24, 260, 31, 36, 1, (AdvMenuRenderCallback)RenderDungeonCluesListButton, (AdvMenuActivateCallback)ActivateDungeonCluesListButton, 1, 1, (char *)0, (char *)0, 0x5000, 0, {0, 0, 0, 0}},
    // Done
    {516, 29, 58, 26, 516, 29, 58, 26, 1, (AdvMenuRenderCallback)RenderDungeonCluesListButton, (AdvMenuActivateCallback)ActivateDungeonCluesListButton, 0, 2, g_dungeon_clues_list_done_keys, g_dungeon_clues_list_done_alt_keys, 0, 0, {0, 0, 0, 0}},
};

// GLOBAL: SHANDALAR 0x0058c804
int DAT_0058c804 = 1;

// GLOBAL: SHANDALAR 0x0058c808
unsigned char g_dungeon_clues_castle_sprite_lookup_by_tile_class[0x18] = {
    0x08,
    0x09,
    0x0a,
    0x0b,
    0x02,
    0x06,
    0x03,
    0x07,
    0x0c,
    0x10,
    0x0d,
    0x11,
    0x0e,
    0x12,
    0x0f,
    0x13,
    0x00,
    0x04,
    0x01,
    0x05,
    0x00,
    0x00,
    0x00,
    0x00,
};

// GLOBAL: SHANDALAR 0x0058c820
int g_dungeon_clues_marker_sprite_lookup[24] = {
    0x2a,
    0x30,
    0x3f,
    0x43,
    0x36,
    0x3c,
    0x35,
    0x3b,
    0x32,
    0x38,
    0x33,
    0x39,
    0x4e,
    0x54,
    0x50,
    0x56,
    0x4f,
    0x55,
    0x37,
    0x3d,
    0x4d,
    0x53,
    0x4c,
    0x52,
};

// GLOBAL: SHANDALAR 0x0058cac4
char g_dungeon_clues_list_done_keys[] = " Dd\r\x1b";
// GLOBAL: SHANDALAR 0x0058cacc
char g_dungeon_clues_list_done_alt_keys[] = " Dd\r\x1b";

/*
 * Globals used by the clue detail screen.
 */

// GLOBAL: SHANDALAR 0x00603a44
int *g_dungeon_clue_detail_strings;
// GLOBAL: SHANDALAR 0x00603a4c
int g_dungeon_clue_detail_strings_loaded;

// GLOBAL: SHANDALAR 0x00746e10
EncodedImage *g_dungeon_clue_detail_done_button_sprites[3];
// GLOBAL: SHANDALAR 0x00746eb0
EncodedImage *g_dungeon_clue_detail_button_icon_sprites[3];

// GLOBAL: SHANDALAR 0x0058c6b0
AdvMenuControl g_dungeon_clue_detail_controls[1] = {
    // Done
    {22, 418, 89, 35, 22, 418, 89, 35, 1, (AdvMenuRenderCallback)RenderDungeonClueDetailDoneButton, (AdvMenuActivateCallback)ActivateDungeonClueDetailDoneButton, 1, 0, g_dungeon_clue_detail_done_keys, g_dungeon_clue_detail_done_alt_keys, 0, 0, {0, 0, 0, 0}},
};

// GLOBAL: SHANDALAR 0x0058cab4
char g_dungeon_clue_detail_done_keys[] = " \rDd\x1b";
// GLOBAL: SHANDALAR 0x0058cabc
char g_dungeon_clue_detail_done_alt_keys[] = " \rDd\x1b";

char *WrapTextToWidthForDropCap(char *src, char *dst, int max_width);
void DrawFormattedTextShadowed(FacemakerWindowBounds *window, int color_index, int x, int y, char *format, ...);
int GetFontCharWidth(int font_slot, int ch);

// FUNCTION: SHANDALAR 0x004311d2
void BlitGraphicsRectScaledFrom320x240(FacemakerWindowBounds *dst, int dst_x, int dst_y, int width, int height,
                                       FacemakerWindowBounds *src, int src_x, int src_y)
{
  BlitGraphicsRect(dst, (dst_x * global_screen_width) / 0x140, (dst_y * global_screen_height) / 0xf0,
                   (width * global_screen_width) / 0x140, (height * global_screen_height) / 0xf0, src,
                   (src_x * global_screen_width) / 0x140, (src_y * global_screen_height) / 0xf0);
}

// FUNCTION: SHANDALAR 0x0050c973
int DrawDungeonClueTextLine(int y, int color_index)
{
  struct
  {
    char first_char_text[4];
    char wrapped_text[1000];
    int first_char_width;
  } s;

  if (ScaleUiCoordinate(0xbe) < MeasureMultilineTextWidth(g_page0_window_bounds, g_ui_message_buffer))
  {
    s.first_char_width = GetFontCharWidth(4, g_ui_message_buffer[0]);
    s.first_char_text[0] = g_ui_message_buffer[0];
    s.first_char_text[1] = '\0';
    DrawTextLineClamped(s.first_char_text, ScaleUiCoordinateFrom320(0x10), y, color_index);

    WrapTextToWidthForDropCap(g_ui_message_buffer + 1, s.wrapped_text, ScaleUiCoordinate(0xbe) - s.first_char_width);

    DrawFormattedTextShadowed(g_page0_window_bounds, color_index, s.first_char_width + ScaleUiCoordinateFrom320(0x10), y, s.wrapped_text);

    y += GetFontLineHeight(g_page0_window_bounds->font_slot) * 2;
  }
  else
  {
    DrawTextLineClamped(g_ui_message_buffer, ScaleUiCoordinateFrom320(0x10), y, color_index);
    y += GetFontLineHeight(g_page0_window_bounds->font_slot);
  }

  return y;
}

#define DUNGEON_CLUE_COLUMN_OFFSET(index) ((((index) & 1) == 0) ? -263 : 0)

// FUNCTION: SHANDALAR 0x00509244
void ShowDungeonCluesScreen(int unused)
{
  struct
  {
    int scroll_delta;              // ebp - 0x1538
    int previous_scroll_top_index; // ebp - 0x1534
    int hover_text_y;              // ebp - 0x1530
    int hover_text_x;              // ebp - 0x152c
    int hover_mouse_y;             // ebp - 0x1528
    int hover_mouse_x;             // ebp - 0x1524
    int click_text_y;              // ebp - 0x1520
    int click_text_x;              // ebp - 0x151c
    int click_mouse_y;             // ebp - 0x1518
    int click_mouse_x;             // ebp - 0x1514
    int castle_icon_y;             // ebp - 0x1510
    int castle_icon_x;             // ebp - 0x150c
    int temp_150c;                 // ebp - 0x1508
    int dungeon_icon_y;            // ebp - 0x1504
    int dungeon_icon_x;            // ebp - 0x1500
    int row_text_y;                // ebp - 0x14fc
    int row_text_x;                // ebp - 0x14f8
    int temp_14f8;                 // ebp - 0x14f4
    int temp_14f4;                 // ebp - 0x14f0
    int temp_14f0;                 // ebp - 0x14ec
    EncodedImage *sprite_pair[2];  // ebp - 0x14e8
    int title_text_width;          // ebp - 0x14e0
    unsigned int old_font_size;    // ebp - 0x14dc
    int title_color_by_state[4];   // ebp - 0x14d8
    int menu_context;              // ebp - 0x14c8
    int town_delta_x;              // ebp - 0x14c4
    int scroll_top_index;          // ebp - 0x14c0
    int hover_index_copy;          // ebp - 0x14bc
    int action_key;                // ebp - 0x14b8
    int draw_index;                // ebp - 0x14b4
    int spacing_accumulator;       // ebp - 0x14b0
    int unused_zero;               // ebp - 0x14ac
    int i;                         // ebp - 0x14a8
    EncodedImage *icon_blobs[50];  // ebp - 0x14a4
    unsigned int visible_dungeon_indices[15];
    unsigned int dungeon_index_by_row[50];
    int town_revealed;          // ebp - 0x12d8
    int visible_count;          // ebp - 0x12d4
    int hover_index;            // ebp - 0x12d0
    int redraw_background;      // ebp - 0x12cc
    unsigned int dungeon_index; // ebp - 0x12c8
    int entry_count;            // ebp - 0x12c4
    char name_buffer[50][0x30]; // ebp - 0x12c0
    char stack_padding[0x960];
  } s;

  (void)unused;

  s.visible_count = 0;
  s.entry_count = 0;
  s.scroll_top_index = 0;
  s.icon_blobs[0] = (EncodedImage *)0;
  memset(&s.icon_blobs[1], 0, sizeof(s.icon_blobs) - sizeof(s.icon_blobs[0]));

  s.title_color_by_state[0] = 0x48;
  s.title_color_by_state[1] = 0x3f;
  s.title_color_by_state[2] = 0x40;

  if (g_dungeon_clues_list_strings_loaded == 0)
  {
    g_dungeon_clues_list_strings = ((int *(__cdecl *)(FILE *, char *, int))LoadIniEscapedStringTable)(g_advbuttons_ini_file, "dunClues",
                                                                                                      (int)g_ini_string_scratch);
    g_dungeon_clues_list_strings_loaded = 1;
  }

  LoadPcxIntoPage(1, "dun_bar.pic");

  BeginSpriteEncodeSession();
  g_dungeon_clues_scrollbar_sprite = EncodeSpriteFromPage(1, 2, 1, 0xd, 0x70);

  g_page1_window_bounds->font_slot = 7;
  for (s.i = 0; s.i < 3; s.i++)
  {
    SetFontStyleSize(7, (unsigned int)(10 + (((unsigned int)(s.i - 2) < 1) ? -1 : 0)));
    DrawFormattedTextShadowedCentered(g_page1_window_bounds, s.title_color_by_state[s.i], s.i * 0x3c + 0x2e, 0xd, "%s",
                                      (char *)g_dungeon_clues_list_strings[1]);
    g_dungeon_clues_list_done_label_sprites[s.i] = EncodeSpriteFromPage(1, s.i * 0x3c + 0x10, 1, 0x3b, 0x1a);
  }

  for (s.draw_index = 0; s.draw_index < 2; s.draw_index++)
  {
    for (s.i = 0; s.i < 4; s.i++)
    {
      g_dungeon_clues_list_button_sprites[s.draw_index][s.i] = EncodeSpriteFromPage(1, s.i * 0x20 + s.draw_index * 0x80 + 0x10, 0x1c, 0x1f, 0x24);
    }
  }

  FinalizeSpriteEncodeSession();

  if (g_dungeon_clues_list_controls[0].x == g_dungeon_clues_list_controls[0].base_x)
  {
    for (s.i = 0; s.i < 3; s.i++)
    {
      g_dungeon_clues_list_controls[s.i].x = ScaleUiCoordinate(g_dungeon_clues_list_controls[s.i].x);
      g_dungeon_clues_list_controls[s.i].y = ScaleUiCoordinate(g_dungeon_clues_list_controls[s.i].y);
      g_dungeon_clues_list_controls[s.i].width = ScaleUiCoordinate(g_dungeon_clues_list_controls[s.i].width);
      g_dungeon_clues_list_controls[s.i].height = ScaleUiCoordinate(g_dungeon_clues_list_controls[s.i].height);
    }
  }

  LoadPcxResource(1, 0, global_screen_height - 0x1e0, "dung_bd.pic", (void *)0);
  StretchBlitGraphicsRect(g_page1_window_bounds, 0, global_screen_height - 0x1e0, 0x280, 0x1e0, g_page1_window_bounds, 0, 0, global_screen_width,
                          global_screen_height);

  s.old_font_size = GetFontStyleSize(6);
  g_page1_window_bounds->font_slot = 6;
  SetFontStyleSize(6, (unsigned int)ScaleUiCoordinate(0x18));
  s.title_text_width = MeasureMultilineTextWidth(g_page1_window_bounds, (char *)g_dungeon_clues_list_strings[0]);
  DrawFormattedTextShadowedCentered(g_page1_window_bounds, 0x42, ScaleUiCoordinate(0x1f7) / 2, ScaleUiCoordinate(0x26), "%s",
                                    (char *)g_dungeon_clues_list_strings[0]);
  SetFontStyleSize(6, s.old_font_size);

  BlitGraphicsRect(g_page1_window_bounds, 0, 0, global_screen_width, global_screen_height, g_page0_window_bounds, 0, 0);

  BeginSpriteEncodeSession();
  for (s.dungeon_index = 0; (int)s.dungeon_index < 0xf; s.dungeon_index++)
  {
    s.town_revealed = 0;
    if ((g_castle_dungeon_slots[s.dungeon_index].clues_bitmap == 0) && (g_reveal_all_world_info == 0))
    {
      continue;
    }
    if (((int)s.dungeon_index >= 5) && (g_castle_dungeon_slots[s.dungeon_index].card_slot_1 == -1) && (g_reveal_all_world_info == 0))
    {
      continue;
    }

    s.dungeon_index_by_row[s.entry_count] = s.dungeon_index;
    if (((g_castle_dungeon_slots[s.dungeon_index].clues_bitmap & 1) != 0) || (g_reveal_all_world_info != 0))
    {
      if ((int)s.dungeon_index < 5)
      {
        s.sprite_pair[0] = g_castles_sprite_entries[(char)g_dungeon_clues_castle_sprite_lookup_by_tile_class[s.dungeon_index * 4]];
        s.sprite_pair[1] = g_castles_sprite_entries[(char)g_dungeon_clues_castle_sprite_lookup_by_tile_class[s.dungeon_index * 4 + 1]];
        s.icon_blobs[s.entry_count] =
            EncodeMergedDungeonIconSprite(g_page1_window_bounds, 0, 0, ScaleUiCoordinate(0x3e), ScaleUiCoordinate(0x3e), s.sprite_pair);
      }
      else
      {
        s.temp_14f0 = (s.dungeon_index - 5) * 2;
        s.sprite_pair[0] = g_location_marker_sprite_entries[g_dungeon_clues_marker_sprite_lookup[s.temp_14f0]];
        s.sprite_pair[1] = g_location_marker_sprite_entries[g_dungeon_clues_marker_sprite_lookup[s.temp_14f0 + 1]];
        s.icon_blobs[s.entry_count] =
            EncodeMergedDungeonIconSprite(g_page1_window_bounds, 0, 0, ScaleUiCoordinate(0x3e), ScaleUiCoordinate(0x3e), s.sprite_pair);
      }
    }
    s.entry_count = s.entry_count + 1;
  }
  FinalizeSpriteEncodeSession();

  s.redraw_background = 1;
  s.menu_context = BeginMenuContext();
  ResetMenuContext(s.menu_context);
  AddMenuControlsToContext(g_dungeon_clues_list_controls, 3, s.menu_context);
  SetCurrentMenuContextArrowNavigation(0);

redraw_background:
  if (s.redraw_background == 0)
  {
    LoadPcxResource(1, 0, global_screen_height - 0x1e0, "dung_bd.pic", (void *)0);
    StretchBlitGraphicsRect(g_page1_window_bounds, 0, global_screen_height - 0x1e0, 0x280, 0x1e0, g_page1_window_bounds, 0, 0,
                            global_screen_width, global_screen_height);

    s.temp_14f4 = GetFontStyleSize(6);
    g_page1_window_bounds->font_slot = 6;
    SetFontStyleSize(6, (unsigned int)ScaleUiCoordinate(0x18));
    s.temp_14f8 = MeasureMultilineTextWidth(g_page1_window_bounds, (char *)g_dungeon_clues_list_strings[0]);
    DrawFormattedTextShadowedCentered(g_page1_window_bounds, 0x42, ScaleUiCoordinate(0x1f7) / 2, ScaleUiCoordinate(0x26), "%s",
                                      (char *)g_dungeon_clues_list_strings[0]);
    SetFontStyleSize(6, s.temp_14f4);

    BlitGraphicsRect(g_page1_window_bounds, 0, 0, global_screen_width, global_screen_height, g_page0_window_bounds, 0, 0);
  }

  s.redraw_background = 0;

redraw_list:
  if (s.entry_count > 0xc)
  {
    if (s.scroll_top_index == 0)
    {
      RenderAdvMenuControlDisabled(&g_dungeon_clues_list_controls[0]);
    }
    else
    {
      RenderAdvMenuControlNormally(&g_dungeon_clues_list_controls[0]);
    }

    if (s.scroll_top_index + 0xc >= s.entry_count)
    {
      RenderAdvMenuControlDisabled(&g_dungeon_clues_list_controls[1]);
    }
    else
    {
      RenderAdvMenuControlNormally(&g_dungeon_clues_list_controls[1]);
    }
  }
  else
  {
    RenderAdvMenuControlDisabled(&g_dungeon_clues_list_controls[0]);
    RenderAdvMenuControlDisabled(&g_dungeon_clues_list_controls[1]);
  }

  RenderCurrentMenuContextControls();

  LoadPcxResource(1, 0, global_screen_height - 0x1e0, "dung_bd.pic", (void *)0);
  StretchBlitGraphicsRect(g_page1_window_bounds, 0, global_screen_height - 0x1e0, 0x280, 0x1e0, g_page1_window_bounds, 0, 0, global_screen_width,
                          global_screen_height);

  g_page0_window_bounds->page_number = 1;
  s.visible_count = 0;
  g_page0_window_bounds->font_slot = 1;
  s.spacing_accumulator = 2;
  s.unused_zero = 0;
  for (s.draw_index = s.scroll_top_index;
       s.draw_index < ((s.entry_count <= s.scroll_top_index + 0xc) ? s.entry_count : s.scroll_top_index + 0xc); s.draw_index++)
  {
    s.dungeon_index = s.dungeon_index_by_row[s.draw_index];
    s.town_revealed = 0;

    if ((g_castle_dungeon_slots[s.dungeon_index].clues_bitmap == 0) && (g_reveal_all_world_info == 0))
    {
      continue;
    }
    if (((int)s.dungeon_index >= 5) && (g_castle_dungeon_slots[s.dungeon_index].card_slot_1 == -1) && (g_reveal_all_world_info == 0))
    {
      continue;
    }

    s.visible_dungeon_indices[s.visible_count] = s.dungeon_index;
    if ((int)s.dungeon_index < 5)
    {
      LoadTextSectionLines("ADVstrings.txt", "CAVELIST");
      FormatMessageFromStringStripCarriageReturns(
          g_ui_message_buffer, 0x1000, text_lines[0], GetDungeonName(s.dungeon_index),
          gs_cityname_manacastle_0077de00[s.dungeon_index + 1]);
    }
    else
    {
      strcpy(g_ui_message_buffer, GetDungeonName(s.dungeon_index));
    }

    s.row_text_x = ScaleUiCoordinate(DUNGEON_CLUE_COLUMN_OFFSET(s.visible_count) + 0x19b);
    s.row_text_y = ScaleUiCoordinate((s.visible_count / 2) * 0x3e + 0x69) - GetFontLineHeight(g_page0_window_bounds->font_slot) / 2;
    DrawFormattedTextNoShadow(g_page0_window_bounds, 0xff, s.row_text_x, s.row_text_y, g_ui_message_buffer);
    strcpy(s.name_buffer[s.visible_count], g_ui_message_buffer);

    s.spacing_accumulator = s.spacing_accumulator + 7;
    if (((g_castle_dungeon_slots[s.dungeon_index].clues_bitmap & 1) != 0) || (g_reveal_all_world_info != 0))
    {
      s.town_delta_x = g_castle_dungeon_slots[s.dungeon_index].world_x -
                       g_town_slots[g_castle_dungeon_slots[s.dungeon_index].north_of_town_index].world_x;
      s.title_color_by_state[3] = g_castle_dungeon_slots[s.dungeon_index].world_y -
                                  g_town_slots[g_castle_dungeon_slots[s.dungeon_index].north_of_town_index].world_y;
      if ((GetWorldMapPixelFlags(g_town_slots[g_castle_dungeon_slots[s.dungeon_index].north_of_town_index].world_x,
                        g_town_slots[g_castle_dungeon_slots[s.dungeon_index].north_of_town_index].world_y) &
           0x80) != 0)
      {
        s.town_revealed = 1;
      }

      if ((int)s.dungeon_index >= 5)
      {
        s.temp_150c = (s.dungeon_index - 5) * 2;
        s.dungeon_icon_x = ScaleUiCoordinate(DUNGEON_CLUE_COLUMN_OFFSET(s.visible_count) + 0x15b);
        s.dungeon_icon_y = ScaleUiCoordinate((s.visible_count / 2) * 0x3e + 0x49);
        DrawEncodedImageResampledFitBoxCentered(g_page0_window_bounds, s.dungeon_icon_x, s.dungeon_icon_y, ScaleUiCoordinate(0x3e), ScaleUiCoordinate(0x3e),
                                                s.icon_blobs[s.draw_index]);
      }
      else
      {
        s.castle_icon_x = ScaleUiCoordinate(DUNGEON_CLUE_COLUMN_OFFSET(s.visible_count) + 0x15b);
        s.castle_icon_y = ScaleUiCoordinate((s.visible_count / 2) * 0x3e + 0x49);
        DrawEncodedImageResampledFitBoxCentered(g_page0_window_bounds, s.castle_icon_x, s.castle_icon_y, ScaleUiCoordinate(0x3e), ScaleUiCoordinate(0x3e),
                                                s.icon_blobs[s.draw_index]);
      }
    }
    s.visible_count = s.visible_count + 1;
  }

  g_page0_window_bounds->page_number = 0;
  BlitGraphicsRect(g_page1_window_bounds, ScaleUiCoordinate(0x50), ScaleUiCoordinate(0x46), ScaleUiCoordinate(0x208), ScaleUiCoordinate(0x17a), g_page0_window_bounds,
                   ScaleUiCoordinate(0x50), ScaleUiCoordinate(0x46));

  g_adv_menu_selected_value = -5;
  s.hover_index = -1;
  s.hover_index_copy = s.hover_index;
  while (g_adv_menu_selected_value == -5)
  {
    s.dungeon_index = 0xffffffffU;
    UpdateMouseSnapshot();

    if (g_mouse_button_down_mask != 0)
    {
      s.click_mouse_y = (g_mouse_y_snapshot * 0x1e0) / global_screen_height;
      s.click_mouse_x = (g_mouse_x_snapshot * 0x280) / global_screen_width;
      if (IsPointInRectExclusive(s.click_mouse_x, s.click_mouse_y, 0x54, 0x49, 0xfb, 0x174) != 0)
      {
        s.dungeon_index = ((s.click_mouse_y - 0x49) / 0x3e) * 2;
      }
      else if (IsPointInRectExclusive(s.click_mouse_x, s.click_mouse_y, 0x15b, 0x49, 0xfd, 0x174) != 0)
      {
        s.dungeon_index = ((s.click_mouse_y - 0x49) / 0x3e) * 2 + 1;
      }

      if (((int)s.dungeon_index >= 0) && ((int)s.dungeon_index < s.visible_count))
      {
        s.click_text_x = ScaleUiCoordinate(DUNGEON_CLUE_COLUMN_OFFSET(s.dungeon_index) + 0x19b);
        s.click_text_y = ScaleUiCoordinate(((int)s.dungeon_index / 2) * 0x3e + 0x69) - GetFontLineHeight(g_page0_window_bounds->font_slot) / 2;
        DrawFormattedTextNoShadow(g_page0_window_bounds, 0xbe, s.click_text_x, s.click_text_y, s.name_buffer[s.dungeon_index]);
        ClearInputAndWaitForMouseRelease();
        ShowDungeonClueDetailScreen(s.visible_dungeon_indices[s.dungeon_index]);
        goto redraw_background;
      }
    }
    else
    {
      s.hover_mouse_y = (g_mouse_y_snapshot * 0x1e0) / global_screen_height;
      s.hover_mouse_x = (g_mouse_x_snapshot * 0x280) / global_screen_width;
      if (IsPointInRectExclusive(s.hover_mouse_x, s.hover_mouse_y, 0x54, 0x49, 0xfb, 0x174) != 0)
      {
        s.dungeon_index = ((s.hover_mouse_y - 0x49) / 0x3e) * 2;
      }
      else if (IsPointInRectExclusive(s.hover_mouse_x, s.hover_mouse_y, 0x15b, 0x49, 0xfd, 0x174) != 0)
      {
        s.dungeon_index = ((s.hover_mouse_y - 0x49) / 0x3e) * 2 + 1;
      }

      if (((int)s.dungeon_index >= 0) && ((int)s.dungeon_index < s.visible_count) && (s.dungeon_index != (unsigned int)s.hover_index))
      {
        if (s.hover_index != -1)
        {
          s.hover_text_x = ScaleUiCoordinate(DUNGEON_CLUE_COLUMN_OFFSET(s.hover_index) + 0x19b);
          s.hover_text_y = ScaleUiCoordinate((s.hover_index / 2) * 0x3e + 0x69) - GetFontLineHeight(g_page0_window_bounds->font_slot) / 2;
          DrawFormattedTextNoShadow(g_page0_window_bounds, 0xff, s.hover_text_x, s.hover_text_y, s.name_buffer[s.hover_index]);
        }

        s.hover_text_x = ScaleUiCoordinate(DUNGEON_CLUE_COLUMN_OFFSET(s.dungeon_index) + 0x19b);
        s.hover_text_y = ScaleUiCoordinate(((int)s.dungeon_index / 2) * 0x3e + 0x69) - GetFontLineHeight(g_page0_window_bounds->font_slot) / 2;
        DrawFormattedTextNoShadow(g_page0_window_bounds, 0xe3, s.hover_text_x, s.hover_text_y, s.name_buffer[s.dungeon_index]);
        s.hover_index = (int)s.dungeon_index;
      }
    }

    s.action_key = -1;
    UpdateMenuControlSelection(g_mouse_x_snapshot, g_mouse_y_snapshot, g_mouse_button_down_mask);
    if (g_adv_menu_selected_value != -5)
    {
      switch (g_adv_menu_selected_value)
      {
      case -1:
        s.action_key = 0x4800;
        break;
      case 1:
        s.action_key = 0x5000;
        break;
      }

      if (g_adv_menu_selected_value != 0)
      {
        g_adv_menu_selected_value = -5;
      }
    }

    if (s.entry_count > 0xc)
    {
      s.previous_scroll_top_index = s.scroll_top_index;
      if ((s.action_key == 0x4800) || (s.action_key == 0x5000))
      {
        if (s.action_key == 0x4800)
        {
          s.scroll_delta = -1;
        }
        else
        {
          s.scroll_delta = 1;
        }

        s.scroll_top_index += s.scroll_delta * 2;
        s.scroll_top_index = MAX(s.scroll_top_index, 0);
        s.scroll_top_index = MIN(s.scroll_top_index, ((s.entry_count + 2) & -2) - 0xc);
        if (s.previous_scroll_top_index != s.scroll_top_index)
        {
          goto redraw_list;
        }
        ClearInputAndWaitForMouseRelease();
      }
    }
  }

  ClearInputAndWaitForMouseRelease();
  EndMenuContext();
  FreeSpriteBlob(g_dungeon_clues_scrollbar_sprite);
  if ((s.entry_count != 0) && (s.icon_blobs[0] != (EncodedImage *)0))
  {
    FreeSpriteBlob(s.icon_blobs[0]);
  }
}

// FUNCTION: SHANDALAR 0x0050caa0
void ShowDungeonClueDetailScreen(int dungeon_index)
{
  struct
  {
    AdvMenuRect clip_restore_page4; // ebp - 0x98
    AdvMenuRect clip_restore_page0; // ebp - 0x88
    AdvMenuRect clip_save_page4;    // ebp - 0x78
    AdvMenuRect clip_save_page0;    // ebp - 0x68
    int saved_page0;                // ebp - 0x58
    AdvMenuRect saved_clip_page0;   // ebp - 0x54

    AdvMenuRect saved_clip_page4; // ebp - 0x44
    int town_delta_y;             // ebp - 0x34
    int town_delta_x;             // ebp - 0x30
    int relative_direction;       // ebp - 0x2c
    int dungeon_color;            // ebp - 0x28
    int menu_context;             // ebp - 0x24
    int title_colors[5];          // ebp - 0x20
    int text_y;                   // ebp - 0xc
    int i;                        // ebp - 0x8
    int line_color;               // ebp - 0x4
  } s;

  s.line_color = 0x98;
  s.title_colors[3] = 0xab;
  s.title_colors[4] = 0x3a;
  s.title_colors[0] = 0x3a;
  s.title_colors[1] = 0x3c;
  s.title_colors[2] = 0x3c;

  if (g_dungeon_clue_detail_strings_loaded == 0)
  {
    g_dungeon_clue_detail_strings = ((int *(__cdecl *)(FILE *, char *, int))LoadIniEscapedStringTable)(g_advbuttons_ini_file, "dunClues",
                                                                                                       (int)g_ini_string_scratch);
    g_dungeon_clue_detail_strings_loaded = 1;
  }

  AnimatePaletteToColor(0, g_default_palette_fade_steps);
  LoadPcxIntoPageNoPalette("advfac64.pic");
  LoadPcxIntoPage(1, "cluebutn.pic");

  BeginSpriteEncodeSession();
  g_page1_window_bounds->font_slot = 7;
  for (s.i = 0; s.i < 3; s.i++)
  {
    SetFontStyleSize(7, (s.i == 2) ? 12 : 14);
    DrawFormattedTextShadowedCentered(g_page1_window_bounds, s.title_colors[s.i], s.i * 0x5a + 0x2e, 0xf, "%s", (char *)g_dungeon_clue_detail_strings[1]);
    g_dungeon_clue_detail_done_button_sprites[s.i] = EncodeSpriteFromPage(1, s.i * 0x5a + 1, 1, 0x59, 0x23);
    g_dungeon_clue_detail_button_icon_sprites[s.i] = EncodeSpriteFromPage(1, s.i * 0x15 + 1, 0x25, 0x14, 0x24);
  }

  if (g_dungeon_clue_detail_controls[0].x == g_dungeon_clue_detail_controls[0].base_x)
  {
    g_dungeon_clue_detail_controls[0].x = ScaleUiCoordinate(g_dungeon_clue_detail_controls[0].x);
    g_dungeon_clue_detail_controls[0].y = ScaleUiCoordinate(g_dungeon_clue_detail_controls[0].y);
    g_dungeon_clue_detail_controls[0].width = ScaleUiCoordinate(g_dungeon_clue_detail_controls[0].width);
    g_dungeon_clue_detail_controls[0].height = ScaleUiCoordinate(g_dungeon_clue_detail_controls[0].height);
  }

  s.menu_context = BeginMenuContext();
  ResetMenuContext(s.menu_context);
  AddMenuControlsToContext(g_dungeon_clue_detail_controls, 1, s.menu_context);
  FinalizeSpriteEncodeSession();

  LoadPcxResource(1, 0, global_screen_height - 0x1e0, "clueback.pic", (void *)0);
  StretchBlitGraphicsRect(g_page1_window_bounds, 0, global_screen_height - 0x1e0, 0x280, 0x1e0, g_page1_window_bounds, 0, 0, global_screen_width,
                          global_screen_height);
  CopyGraphicsRect(g_page1_window_bounds, 0, 0, global_screen_width, global_screen_height, g_page0_window_bounds, 0, 0);

  /* Render top caption box from page 0 (matches original layout) */
  BlitGraphicsRect(g_page1_window_bounds, ScaleUiCoordinate(0x16), ScaleUiCoordinate(0x1a2), ScaleUiCoordinate(0x5a), ScaleUiCoordinate(0x24),
                   g_page1_window_bounds, 0, 0);
  RenderCurrentMenuContextControls();

  g_page0_window_bounds->font_slot = 4;

  /* Title */
  if (dungeon_index < 5)
  {
    sprintf(g_ui_message_buffer, "%s\n      (%s)", GetDungeonName(dungeon_index),
            (char *)gs_cityname_manacastle_0077de00[dungeon_index + 1]);
    DrawFormattedTextShadowed(g_page0_window_bounds, s.title_colors[4], 0x12, 0x1c, g_ui_message_buffer);
    s.text_y = ScaleUiCoordinateFrom320(0x24);
  }
  else
  {
    strcpy(g_ui_message_buffer, GetDungeonName(dungeon_index));
    s.text_y = DrawDungeonClueTextLine(0x10, s.title_colors[4]);
    s.text_y = ScaleUiCoordinateFrom320(0x1c);
  }

  for (s.i = 0; s.i < 3; s.i++)
  {
    if ((&g_castle_dungeon_slots[dungeon_index].card_slot_1)[s.i] == -1)
    {
      continue;
    }

    DrawAdventureCard((&g_castle_dungeon_slots[dungeon_index].card_slot_1)[s.i], s.i * 0x29 + 0xa0,
                      ((s.i == 0) ? 8 : 0) + s.i * 4 + 0x76, 1, "");
  }

  if (((g_castle_dungeon_slots[dungeon_index].clues_bitmap & 2) != 0) || (g_reveal_all_world_info != 0))
  {
    DrawTextLineClamped(gs_cave_showclues_0077efa0[0], ScaleUiCoordinateFrom320(0xc), s.text_y, s.line_color);
    s.text_y += GetFontLineHeight(g_page0_window_bounds->font_slot);
    s.dungeon_color = (int)(char)g_castle_dungeon_slots[dungeon_index].color;
    if (dungeon_index < 5)
    {
      g_castle_dungeon_slots[dungeon_index].monster_flags = GetDefeatedWizardCountPlusOne() | 0x80;
      sprintf(g_ui_message_buffer, gs_cave_showclues_0077efa0[1], gs_cave_showclues_0077efa0[s.dungeon_color + 1]);
    }
    else
    {
      sprintf(g_ui_message_buffer, gs_cave_showclues_0077efa0[1], gs_cave_showclues_0077efa0[s.dungeon_color + 6]);
    }
    s.text_y = DrawDungeonClueTextLine(s.text_y, s.title_colors[3]);
  }

  s.text_y += GetFontLineHeight(g_page0_window_bounds->font_slot);
  if (((g_castle_dungeon_slots[dungeon_index].clues_bitmap & 4) != 0) || (g_reveal_all_world_info != 0))
  {
    DrawTextLineClamped(gs_cave_showclues_0077efa0[0xc], ScaleUiCoordinateFrom320(0xc), s.text_y, s.line_color);
    s.text_y += GetFontLineHeight(g_page0_window_bounds->font_slot);
    if ((g_castle_dungeon_slots[dungeon_index].rules_bitmap & 0x10) != 0)
    {
      sprintf(g_ui_message_buffer, gs_cave_showclues_0077efa0[0xd], gs_colorcards_0077c5e0[(char)g_castle_dungeon_slots[dungeon_index].color]);
      s.text_y = DrawDungeonClueTextLine(s.text_y, s.title_colors[3]);
    }
    if ((g_castle_dungeon_slots[dungeon_index].rules_bitmap & 0x20) != 0)
    {
      strcpy(g_ui_message_buffer, gs_cave_showclues_0077efa0[0xe]);
      s.text_y = DrawDungeonClueTextLine(s.text_y, s.title_colors[3]);
    }
    if ((g_castle_dungeon_slots[dungeon_index].rules_bitmap & 0x40) != 0)
    {
      strcpy(g_ui_message_buffer, gs_cave_showclues_0077efa0[0xf]);
      s.text_y = DrawDungeonClueTextLine(s.text_y, s.title_colors[3]);
    }
    if ((g_castle_dungeon_slots[dungeon_index].rules_bitmap & 0x80) != 0)
    {
      strcpy(g_ui_message_buffer, gs_cave_showclues_0077efa0[0x10]);
      s.text_y = DrawDungeonClueTextLine(s.text_y, s.title_colors[3]);
    }
    if ((g_castle_dungeon_slots[dungeon_index].rules_bitmap & 1) != 0)
    {
      strcpy(g_ui_message_buffer, gs_cave_showclues_0077efa0[0x11]);
      s.text_y = DrawDungeonClueTextLine(s.text_y, s.title_colors[3]);
    }
    if ((g_castle_dungeon_slots[dungeon_index].rules_bitmap & 2) != 0)
    {
      strcpy(g_ui_message_buffer, gs_cave_showclues_0077efa0[0x12]);
      s.text_y = DrawDungeonClueTextLine(s.text_y, s.title_colors[3]);
    }
    if (g_castle_dungeon_slots[dungeon_index].card_in_effect != -1)
    {
      sprintf(g_ui_message_buffer, gs_cave_showclues_0077efa0[0x13],
              global_cards_data[FindCardIndexByCsvid(g_castle_dungeon_slots[dungeon_index].card_in_effect)].name);
      s.text_y = DrawDungeonClueTextLine(s.text_y, s.title_colors[3]);
    }
    else if (g_castle_dungeon_slots[dungeon_index].rules_bitmap == 0)
    {
      strcpy(g_ui_message_buffer, gs_cave_showclues_0077efa0[0x14]);
      s.text_y = DrawDungeonClueTextLine(s.text_y, s.title_colors[3]);
    }
    s.text_y += GetFontLineHeight(g_page0_window_bounds->font_slot);
  }

  if (((g_castle_dungeon_slots[dungeon_index].clues_bitmap & 1) != 0) || (g_reveal_all_world_info != 0))
  {
    DrawTextLineClamped(gs_cave_showclues_0077efa0[0x15], ScaleUiCoordinateFrom320(0xc), s.text_y, s.line_color);
    s.text_y += GetFontLineHeight(g_page0_window_bounds->font_slot);
    s.town_delta_x = g_castle_dungeon_slots[dungeon_index].world_x -
                     g_town_slots[g_castle_dungeon_slots[dungeon_index].north_of_town_index].world_x;
    s.town_delta_y = g_castle_dungeon_slots[dungeon_index].world_y -
                     g_town_slots[g_castle_dungeon_slots[dungeon_index].north_of_town_index].world_y;
    if (s.town_delta_y > 0)
    {
      if (s.town_delta_x > 0)
      {
        s.relative_direction = 1;
      }
      else
      {
        s.relative_direction = 2;
      }
    }
    else
    {
      if (s.town_delta_x > 0)
      {
        s.relative_direction = 0;
      }
      else
      {
        s.relative_direction = 3;
      }
    }

    FormatMessageFromStringStripCarriageReturns(g_ui_message_buffer, 0x1000,
                                                gs_cave_showclues_0077efa0[0x16],
                                                gs_directions_00765d50[s.relative_direction],
                                                BuildTownDisplayName(g_castle_dungeon_slots[dungeon_index].north_of_town_index));
    s.text_y = DrawDungeonClueTextLine(s.text_y, s.title_colors[3]);
  }

  if (((g_castle_dungeon_slots[dungeon_index].clues_bitmap & 1) != 0) || (g_reveal_all_world_info != 0))
  {
    BlitGraphicsRectScaledFrom320x240(g_page2_window_bounds, 0, 0, 0x50, 0x32, g_page2_window_bounds, 0x50, 0);
    g_town_dialog_callback(ScaleUiCoordinate(0xf4) + 10, ScaleUiCoordinate(0xc) + 10, ScaleUiCoordinate(0x17e) - 0x18,
                           ScaleUiCoordinate(0xe1) - 0x18, 0);
    g_page0_window_bounds->page_number = g_page1_window_bounds->page_number;
    ResetWorldDrawQueue();
    s.saved_page0 = g_page1_window_bounds->page_number;
    g_world_scroll_cache_ready = 0;
    s.saved_clip_page0 = *PushGraphicsClipRect(&s.clip_save_page0, g_page1_window_bounds, 0, 0x80, g_page0_window_bounds->max_x,
                                               g_page0_window_bounds->max_y - 0x80);

    s.saved_clip_page4 = *PushGraphicsClipRect(&s.clip_save_page4, g_page0_window_bounds, 0, 0x80, g_page0_window_bounds->max_x,
                                               g_page0_window_bounds->max_y - 0x80);

    UpdateWorldViewportBuffer(g_castle_dungeon_slots[dungeon_index].world_x * 0x20 + 0x10,
                              g_castle_dungeon_slots[dungeon_index].world_y * 0x20 + 0x10);
    DrawQueuedWorldSprites();
    g_world_scroll_cache_ready = 0;
    PushGraphicsClipRect(&s.clip_restore_page0, g_page1_window_bounds, s.saved_clip_page0.x, s.saved_clip_page0.y, s.saved_clip_page0.width,
                         s.saved_clip_page0.height);
    PushGraphicsClipRect(&s.clip_restore_page4, g_page0_window_bounds, s.saved_clip_page4.x, s.saved_clip_page4.y, s.saved_clip_page4.width,
                         s.saved_clip_page4.height);
    g_page1_window_bounds->page_number = s.saved_page0;
    g_page0_window_bounds->page_number = 0;
    BlitGraphicsRectScaledFrom320x240(g_page1_window_bounds, 0x40, g_world_ui_top_offset, 0xb3, 100, g_page0_window_bounds, 0x7f, 0xb);
    BlitGraphicsRectScaledFrom320x240(g_page2_window_bounds, 0x50, 0, 0x50, 0x32, g_page2_window_bounds, 0, 0);
  }

  g_adv_menu_selected_value = -1;
  while (g_adv_menu_selected_value == -1)
  {
    UpdateMouseSnapshot();
    UpdateMenuControlSelection(g_mouse_x_snapshot, g_mouse_y_snapshot, g_mouse_button_down_mask);
  }

  DestroyCachedCardArt();
  EndMenuContext();
  FreeSpriteBlob(g_dungeon_clue_detail_done_button_sprites[0]);
}
