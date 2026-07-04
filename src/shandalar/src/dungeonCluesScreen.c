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

extern FacemakerWindowBounds *PTR_DAT_005832b4;
extern FacemakerWindowBounds *PTR_DAT_005832dc;
extern FacemakerWindowBounds *PTR_DAT_00583304;

extern DIBSurface *g_graphics_pages[10];

/* Shared selection/activation scratch used by multiple screens (defined in cityInfoScreen.c) */
extern int DAT_00603a34;

/* Debug toggle used by multiple screens (defined in cityInfoScreen.c) */
extern int DAT_007894f4;

/* Palette helper data (defined in shandalar.c) */
extern int DAT_00589dec;

/* Sprites loaded by startup code (owned by shandalar.c) */
extern EncodedImage *g_location_marker_sprite_entries[0x9e];

/* Marker lookup table (defined in adventureWorldUi.c) */
extern int g_dungeon_marker_sprite_lookup[24];

/* Forward decls for locals defined later in this module */
extern EncodedImage *DAT_00746e20[8];
extern EncodedImage *DAT_00746e40[3];
extern EncodedImage *DAT_00746e50;

extern EncodedImage *DAT_00746e10[3];
extern EncodedImage *DAT_00746eb0[3];

extern char DAT_0058cac4[];
extern char DAT_0058cacc[];
extern char DAT_0058cab4[];
extern char DAT_0058cabc[];
extern char s_x_sound_button2_wav_0058cae4[];
extern char s_x_sound_button2_wav_0058caf8[];

void __cdecl FUN_0050caa0(int dungeon_index);

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
int FUN_00500129(int allow_arrow_nav);
int FUN_005001e3(void);

int ScaleUiCoordinate(int value);
int FUN_005501dc(int value);
int SetFontStyleSize(int font_slot, unsigned int point_size);
unsigned char GetFontStyleSize(int font_slot);
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

void UpdateMouseSnapshot(void);
void PopNormalizedQueuedKeyInput(void);
int UpdateMenuControlSelection(int mouse_x, int mouse_y, int allow_activate_on_click);
void ClearInputAndWaitForMouseRelease(void);

void AnimatePaletteToColor(int mode, int color_index);
void PlaySoundEffectOnChannel(char *sound_path, int channel, int volume, int pitch_percent, int pan_percent);
void DrawFormattedTextShadowedCentered(FacemakerWindowBounds *window, int color_index, int x, int y, char *format, ...);
void __cdecl DrawFormattedTextNoShadow(FacemakerWindowBounds *dst, int text_color, int x, int y, char *format, ...);
void __cdecl GetEncodedImageSpanXExtents(EncodedImage *image, unsigned int *out_min_x, int *out_max_x);
int __cdecl DrawEncodedImageResampledFitBoxCentered(FacemakerWindowBounds *dst, int x, int y, int box_w, int box_h,
                                                    EncodedImage *image);

/* Card rendering (drawcardlib) */
extern unsigned char global_raw_cards_storage[];
extern card_data_t global_cards_data[];
extern EncodedImage *g_endtop_banner_sprite;

/* Other helpers referenced by clue detail screen */
void FUN_004311d2(FacemakerWindowBounds *dst, int dst_x, int dst_y, int width, int height,
                  FacemakerWindowBounds *src, int src_x, int src_y);
void FUN_0054a880(void);
void FUN_0054a997(void);
void FUN_0054cee2(int world_x, int world_y);

unsigned int MarkPathConnection(int world_x, int world_y, int unused);
char *FUN_00550220(int town_index);
DWORD __cdecl FUN_00564e70(char *dst, DWORD dst_len, LPCVOID format, ...);

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
int __cdecl FUN_00508bf3(int x, int y, int rect_x, int rect_y, int rect_w, int rect_h)
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
EncodedImage *__cdecl FUN_00508c4a(FacemakerWindowBounds *unused_page, int unused_x, int unused_y, int unused_w, int unused_h,
                                   EncodedImage **sprite_pair)
{
  EncodedImage *sprite_a;
  EncodedImage *sprite_b;
  EncodedImage *result;
  int top_y;
  unsigned int min_x_a;
  unsigned int min_x_b;
  int max_x_a;
  int max_x_b;
  unsigned int min_x;
  int max_x;
  int min_y;
  int max_y;
  int a_y;
  int b_y;
  int a_rows;
  int b_rows;

  (void)unused_page;
  (void)unused_x;
  (void)unused_y;
  (void)unused_w;
  (void)unused_h;

  sprite_a = sprite_pair[0];
  sprite_b = sprite_pair[1];
  if (sprite_a == (EncodedImage *)0)
  {
    return (EncodedImage *)0;
  }

  // Clear scratch area then draw the two sprites into it.
  FillGraphicsRect(PTR_DAT_00583304, 0, 0x80, (int)sprite_a->width + 1, (int)sprite_a->height + 1, 0);
  DrawEncodedImageUnscaled(PTR_DAT_00583304, 0, 0x80, sprite_a);
  DrawEncodedImageUnscaled(PTR_DAT_00583304, 0, 0x80, sprite_b);

  GetEncodedImageSpanXExtents(sprite_a, &min_x_a, &max_x_a);
  GetEncodedImageSpanXExtents(sprite_b, &min_x_b, &max_x_b);

  min_x = (min_x_a <= min_x_b) ? min_x_a : min_x_b;
  max_x = (max_x_b <= max_x_a) ? max_x_a : max_x_b;
  if (0xfa < (int)(max_x - min_x))
  {
    max_x = (int)min_x + 0xfa;
  }

  a_y = (int)sprite_a->first_row;
  b_y = (int)sprite_b->first_row;
  a_rows = (int)sprite_a->row_count;
  b_rows = (int)sprite_b->row_count;

  // Compute Y extents similarly to the original (first_row/top_clip are signed shorts in the blob).
  min_y = (a_y <= b_y) ? a_y : b_y;
  max_y = ((a_y + a_rows) <= (b_y + b_rows)) ? (b_y + b_rows) : (a_y + a_rows);

  top_y = min_y + 0x80;
  result = EncodeSpriteFromPage(PTR_DAT_00583304->page_number, (int)min_x, top_y, (max_x - (int)min_x) + 1, (max_y - min_y) + 1);
  return result;
}

// FUNCTION: SHANDALAR 0x005081e0
char *__cdecl FUN_005081e0(int dungeon_index)
{
  return gs_dungeon_names_00780820[dungeon_index];
}

// FUNCTION: SHANDALAR 0x0050a56d
int FUN_0050a56d(void)
{
  int wins;
  int i;

  wins = 1;
  for (i = 0; i < 6; i++)
  {
    if ((g_defeated_wizards_bitmap & (1 << ((unsigned char)i & 0x1f))) != 0)
    {
      wins = wins + 1;
    }
  }

  return wins;
}

// FUNCTION: SHANDALAR 0x00508f45
int __cdecl FUN_00508f45(AdvMenuControl *control, int mode)
{
  int hovered;

  if (g_menu_render_guard == 0)
  {
    hovered = (control->x <= g_mouse_x) && (g_mouse_x <= control->x + control->width) && (control->y <= g_mouse_y) &&
              (g_mouse_y <= control->y + control->height);
    if (!hovered)
    {
      return 0;
    }
  }

  if (control->state == 3)
  {
    return 0;
  }

  DrawEncodedImageResampled(PTR_DAT_005832b4, control->x, control->y, control->width, control->height,
                            *(&DAT_00746e20[0] + mode + control->unk_30 * 4));

  if ((mode == 2) && (control->on_activate != (AdvMenuActivateCallback)0))
  {
    control->on_activate(control);
  }

  return 1;
}

// FUNCTION: SHANDALAR 0x00509064
int __cdecl FUN_00509064(AdvMenuControl *control)
{
  PlaySoundEffectOnChannel(s_x_sound_button2_wav_0058caf8, 0xf, 100, 100, 0);
  DAT_00603a34 = control->selection_value;
  return 0;
}

// FUNCTION: SHANDALAR 0x005089cb
int __cdecl FUN_005089cb(AdvMenuControl *control, int mode)
{
  int hovered;

  if (g_menu_render_guard == 0)
  {
    hovered = (control->x <= g_mouse_x) && (g_mouse_x <= control->x + control->width) && (control->y <= g_mouse_y) &&
              (g_mouse_y <= control->y + control->height);
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
    DrawEncodedImageResampled(PTR_DAT_005832dc, 1, 1, control->width - 2, control->height - 2, DAT_00746e10[2]);
    BlitGraphicsRect(PTR_DAT_005832dc, 0, 0, control->width + 1, control->height + 1, PTR_DAT_005832b4, control->x, control->y);
    if (control->on_activate != (AdvMenuActivateCallback)0)
    {
      control->on_activate(control);
    }
  }
  else
  {
    DrawEncodedImageResampled(PTR_DAT_005832b4, control->x, control->y, control->width, control->height, DAT_00746e10[mode]);
  }

  return 1;
}

// FUNCTION: SHANDALAR 0x00508b57
int __cdecl FUN_00508b57(AdvMenuControl *control)
{
  PlaySoundEffectOnChannel(s_x_sound_button2_wav_0058cae4, 0xf, 100, 100, 0);
  DAT_00603a34 = control->selection_value;
  return 0;
}

// FUNCTION: SHANDALAR 0x004f2407
void __cdecl FUN_004f2407(int card_index, int x, int y, int full_card, char *banner_label)
{
  int x_scaled;
  int y_scaled;
  int w_scaled;
  int h_scaled;
  RECT clip;
  int saved_dc;
  int text_w;

  if (full_card == 0)
  {
    w_scaled = FUN_005501dc(0x30);
    h_scaled = FUN_005501dc(0x30);
  }
  else
  {
    w_scaled = FUN_005501dc(0x50);
    h_scaled = FUN_005501dc(0x70);
    if (0xf0 < y + 0x70)
    {
      y = 0x7f;
    }
  }

  x_scaled = FUN_005501dc(x);
  y_scaled = FUN_005501dc(y);
  SetRect(&clip, x_scaled, y_scaled, x_scaled + w_scaled, y_scaled + h_scaled);

  saved_dc = SaveDC(g_graphics_pages[PTR_DAT_005832b4->page_number]->hTempDC);
  IntersectClipRect(g_graphics_pages[PTR_DAT_005832b4->page_number]->hTempDC, clip.left, clip.top, clip.right, clip.bottom);

  if (full_card == 0)
  {
    DrawSmallCard(g_graphics_pages[PTR_DAT_005832b4->page_number]->hTempDC, &clip,
                  (card_ptr_t *)(global_raw_cards_storage + global_cards_data[card_index].id * 0x98), 0, 1);
  }
  else
  {
    DrawFullCard(g_graphics_pages[PTR_DAT_005832b4->page_number]->hTempDC, &clip,
                 (card_ptr_t *)(global_raw_cards_storage + global_cards_data[card_index].id * 0x98), 0, 1, 1, gs_illus_00789130);
  }

  RestoreDC(g_graphics_pages[PTR_DAT_005832b4->page_number]->hTempDC, saved_dc);

  if ((banner_label != (char *)0) && (*banner_label != '\0'))
  {
    PTR_DAT_005832b4->font_slot = 1;
    text_w = MeasureTextLineWidth(banner_label);
    DrawEncodedImageResampled(PTR_DAT_005832b4, (x_scaled + w_scaled / 2) - text_w / 2 - 10, y_scaled - 0x18, text_w + 0x14, 0x14,
                              g_endtop_banner_sprite);
    DrawCenteredTextLineClamped(banner_label, x_scaled + w_scaled / 2, y_scaled - 0x14, 0xff);
  }
}

// FUNCTION: SHANDALAR 0x004f263b
void __cdecl FUN_004f263b(int card_index, int x, int y, int width, int height, int full_card, char *banner_label)
{
  int x_scaled;
  int y_scaled;
  int w_scaled;
  int h_scaled;
  RECT clip;
  int saved_dc;
  int text_w;

  w_scaled = FUN_005501dc(width);
  h_scaled = FUN_005501dc(height);
  if ((full_card != 0) && (0xf0 < y + 0x70))
  {
    y = 0x7f;
  }

  x_scaled = FUN_005501dc(x);
  y_scaled = FUN_005501dc(y);
  SetRect(&clip, x_scaled, y_scaled, x_scaled + w_scaled, y_scaled + h_scaled);

  saved_dc = SaveDC(g_graphics_pages[PTR_DAT_005832b4->page_number]->hTempDC);
  IntersectClipRect(g_graphics_pages[PTR_DAT_005832b4->page_number]->hTempDC, clip.left, clip.top, clip.right, clip.bottom);

  if (full_card == 0)
  {
    DrawSmallCard(g_graphics_pages[PTR_DAT_005832b4->page_number]->hTempDC, &clip,
                  (card_ptr_t *)(global_raw_cards_storage + global_cards_data[card_index].id * 0x98), 0, 1);
  }
  else
  {
    DrawFullCard(g_graphics_pages[PTR_DAT_005832b4->page_number]->hTempDC, &clip,
                 (card_ptr_t *)(global_raw_cards_storage + global_cards_data[card_index].id * 0x98), 0, 1, 1, gs_illus_00789130);
  }

  RestoreDC(g_graphics_pages[PTR_DAT_005832b4->page_number]->hTempDC, saved_dc);

  if ((banner_label != (char *)0) && (*banner_label != '\0'))
  {
    PTR_DAT_005832b4->font_slot = 1;
    text_w = MeasureTextLineWidth(banner_label);
    DrawEncodedImageResampled(PTR_DAT_005832b4, (x_scaled + w_scaled / 2) - text_w / 2 - 10, y_scaled - 0x18, text_w + 0x14, 0x14,
                              g_endtop_banner_sprite);
    DrawCenteredTextLineClamped(banner_label, x_scaled + w_scaled / 2, y_scaled - 0x14, 0xff);
  }
}

/*
 * Globals used by the dungeon clues list screen.
 */

// GLOBAL: SHANDALAR 0x00603a3c
int DAT_00603a3c = 0;
// GLOBAL: SHANDALAR 0x00603a40
int *DAT_00603a40 = (int *)0;

// GLOBAL: SHANDALAR 0x00746e20
EncodedImage *DAT_00746e20[8];
// GLOBAL: SHANDALAR 0x00746e40
EncodedImage *DAT_00746e40[3];
// GLOBAL: SHANDALAR 0x00746e50
EncodedImage *DAT_00746e50 = (EncodedImage *)0;

// GLOBAL: SHANDALAR 0x0058c708
AdvMenuControl DAT_0058c708[3] = {
    // Up
    {24, 219, 31, 36, 24, 219, 31, 36, 1, (AdvMenuRenderCallback)FUN_00508f45, (AdvMenuActivateCallback)FUN_00509064, -1, 0, (char *)0, (char *)0, 0x4800, 0, {0, 0, 0, 0}},
    // Down
    {24, 260, 31, 36, 24, 260, 31, 36, 1, (AdvMenuRenderCallback)FUN_00508f45, (AdvMenuActivateCallback)FUN_00509064, 1, 1, (char *)0, (char *)0, 0x5000, 0, {0, 0, 0, 0}},
    // Done
    {516, 29, 58, 26, 516, 29, 58, 26, 1, (AdvMenuRenderCallback)FUN_00508f45, (AdvMenuActivateCallback)FUN_00509064, 0, 2, DAT_0058cac4, DAT_0058cacc, 0, 0, {0, 0, 0, 0}},
};

// GLOBAL: SHANDALAR 0x0058cac4
char DAT_0058cac4[] = " Dd\r\x1b";
// GLOBAL: SHANDALAR 0x0058cacc
char DAT_0058cacc[] = " Dd\r\x1b";

// GLOBAL: SHANDALAR 0x0058cb0c
char s_dunClues_0058cb0c[] = "dunClues";
// GLOBAL: SHANDALAR 0x0058cb18
char s_dun_bar_pic_0058cb18[] = "dun_bar.pic";
// GLOBAL: SHANDALAR 0x0058cb24
char DAT_0058cb24[] = "%s";
// GLOBAL: SHANDALAR 0x0058cb28
char s_dung_bd_pic_0058cb28[] = "dung_bd.pic";
// GLOBAL: SHANDALAR 0x0058cb38
char s_dung_bd_pic_0058cb38[] = "dung_bd.pic";
// GLOBAL: SHANDALAR 0x0058cb48
char s_dung_bd_pic_0058cb48[] = "dung_bd.pic";
// GLOBAL: SHANDALAR 0x0058cb54
char s_CAVELIST_0058cb54[] = "CAVELIST";
// GLOBAL: SHANDALAR 0x0058cb60
char s_ADVstrings_txt_0058cb60[] = "ADVstrings.txt";

/*
 * Globals used by the clue detail screen.
 */

// GLOBAL: SHANDALAR 0x00603a44
int *DAT_00603a44 = (int *)0;
// GLOBAL: SHANDALAR 0x00603a4c
int DAT_00603a4c = 0;

// GLOBAL: SHANDALAR 0x00746e10
EncodedImage *DAT_00746e10[3];
// GLOBAL: SHANDALAR 0x00746eb0
EncodedImage *DAT_00746eb0[3];

// GLOBAL: SHANDALAR 0x0058c6b0
AdvMenuControl DAT_0058c6b0[1] = {
    // Done
    {22, 418, 89, 35, 22, 418, 89, 35, 1, (AdvMenuRenderCallback)FUN_005089cb, (AdvMenuActivateCallback)FUN_00508b57, 1, 0, DAT_0058cab4, DAT_0058cabc, 0, 0, {0, 0, 0, 0}},
};

// GLOBAL: SHANDALAR 0x0058cab4
char DAT_0058cab4[] = " \rDd\x1b";
// GLOBAL: SHANDALAR 0x0058cabc
char DAT_0058cabc[] = " \rDd\x1b";

// GLOBAL: SHANDALAR 0x0058cae4
char s_x_sound_button2_wav_0058cae4[] = "x:sound\\button2.wav";
// GLOBAL: SHANDALAR 0x0058caf8
char s_x_sound_button2_wav_0058caf8[] = "x:sound\\button2.wav";

// GLOBAL: SHANDALAR 0x0058ccac
char s_dunClues_0058ccac[] = "dunClues";
// GLOBAL: SHANDALAR 0x0058ccb8
char s_advfac64_pic_0058ccb8[] = "advfac64.pic";
// GLOBAL: SHANDALAR 0x0058ccc8
char s_cluebutn_pic_0058ccc8[] = "cluebutn.pic";
// GLOBAL: SHANDALAR 0x0058ccdc
char s_clueback_pic_0058ccdc[] = "clueback.pic";
// GLOBAL: SHANDALAR 0x0058ccec
char s__s____s__0058ccec[] = "%s\n      (%s)";
// GLOBAL: SHANDALAR 0x0058ccfc
char DAT_0058ccfc[] = "";

/*
 * Dungeon clue text draw helper.
 * This is a simplified version of SHANDALAR 0x0050c973: draw g_ui_message_buffer and advance Y.
 */

// FUNCTION: SHANDALAR 0x0050c973
int __cdecl FUN_0050c973(int y, int color_index)
{
  int x;
  int line_height;

  x = FUN_005501dc(0x10);
  DrawTextLineClamped(g_ui_message_buffer, x, y, color_index);

  line_height = GetFontLineHeight(PTR_DAT_005832b4->font_slot);
  return y + line_height;
}

// FUNCTION: SHANDALAR 0x00509244
void ShowDungeonCluesScreen(void)
{
  struct
  {
    int title_color_by_state[4];
    EncodedImage *icon_blobs[50];
    unsigned int dungeon_index_by_row[50];
    char name_buffer[50][0x30];

    int entry_count;
    int visible_count;
    int scroll_top_index;
    int menu_context;
    int hover_index;
    int prev_hover_index;
    int action_key;
  } s;

  int i;
  int j;

  s.visible_count = 0;
  s.entry_count = 0;
  s.scroll_top_index = 0;
  s.hover_index = -1;
  s.prev_hover_index = -1;
  s.action_key = -1;

  for (i = 0; i < 50; i++)
  {
    s.icon_blobs[i] = (EncodedImage *)0;
    s.dungeon_index_by_row[i] = 0xffffffffU;
    s.name_buffer[i][0] = '\0';
  }

  s.title_color_by_state[0] = 0x48;
  s.title_color_by_state[1] = 0x3f;
  s.title_color_by_state[2] = 0x40;

  if (DAT_00603a3c == 0)
  {
    DAT_00603a40 = LoadIniEscapedStringTable(g_advbuttons_ini_file, s_dunClues_0058cb0c, (int)g_ini_string_scratch, 0);
    DAT_00603a3c = 1;
  }

  LoadPcxIntoPage(1, s_dun_bar_pic_0058cb18);

  BeginSpriteEncodeSession();
  DAT_00746e50 = EncodeSpriteFromPage(1, 2, 1, 0xd, 0x70);

  /* Build 3-state Done button sprites into DAT_00746e40 */
  PTR_DAT_005832dc->font_slot = 7;
  for (i = 0; i < 3; i++)
  {
    SetFontStyleSize(7, (unsigned int)(10 - (i == 2)));
    if (DAT_00603a40 != (int *)0)
    {
      DrawFormattedTextShadowedCentered(PTR_DAT_005832dc, s.title_color_by_state[i], i * 0x3c + 0x2e, 0xd, DAT_0058cb24,
                                        (char *)DAT_00603a40[1]);
    }
    DAT_00746e40[i] = EncodeSpriteFromPage(1, i * 0x3c + 0x10, 1, 0x3b, 0x1a);
  }

  /* Build 2x4 sprites into DAT_00746e20: up/down button states */
  for (i = 0; i < 2; i++)
  {
    for (j = 0; j < 4; j++)
    {
      DAT_00746e20[j + i * 4] = EncodeSpriteFromPage(1, j * 0x20 + i * 0x80 + 0x10, 0x1c, 0x1f, 0x24);
    }
  }

  FinalizeSpriteEncodeSession();

  /* Scale controls once (x matches base_x before scaling) */
  if (DAT_0058c708[0].x == DAT_0058c708[0].base_x)
  {
    for (i = 0; i < 3; i++)
    {
      DAT_0058c708[i].x = ScaleUiCoordinate(DAT_0058c708[i].x);
      DAT_0058c708[i].y = ScaleUiCoordinate(DAT_0058c708[i].y);
      DAT_0058c708[i].width = ScaleUiCoordinate(DAT_0058c708[i].width);
      DAT_0058c708[i].height = ScaleUiCoordinate(DAT_0058c708[i].height);
    }
  }

  /* Background */
  LoadPcxResource(1, 0, global_screen_height - 0x1e0, s_dung_bd_pic_0058cb28, (void *)0);
  StretchBlitGraphicsRect(PTR_DAT_005832dc, 0, global_screen_height - 0x1e0, 0x280, 0x1e0, PTR_DAT_005832dc, 0, 0, global_screen_width,
                          global_screen_height);

  /* Title (advButtons [dunClues]) */
  PTR_DAT_005832dc->font_slot = 6;
  {
    unsigned char old_font_byte = GetFontStyleSize(6);
    SetFontStyleSize(6, (unsigned int)ScaleUiCoordinate(0x18));
    if (DAT_00603a40 != (int *)0)
    {
      MeasureMultilineTextWidth(PTR_DAT_005832dc, (char *)DAT_00603a40[0]);
      DrawFormattedTextShadowedCentered(PTR_DAT_005832dc, 0x42, ScaleUiCoordinate(0x1f7) / 2, ScaleUiCoordinate(0x26), DAT_0058cb24,
                                        (char *)DAT_00603a40[0]);
    }
    SetFontStyleSize(6, (unsigned int)old_font_byte);
  }

  BlitGraphicsRect(PTR_DAT_005832dc, 0, 0, global_screen_width, global_screen_height, PTR_DAT_005832b4, 0, 0);

  /* Build list of dungeon indices we can show */
  for (i = 0; i < 0xf; i++)
  {
    if (((g_castle_dungeon_slots[i].clues_bitmap != 0) || (DAT_007894f4 != 0)) &&
        ((i < 5) || (g_castle_dungeon_slots[i].card_slot_1 != -1) || (DAT_007894f4 != 0)))
    {
      s.dungeon_index_by_row[s.entry_count] = (unsigned int)i;
      s.entry_count = s.entry_count + 1;
    }
  }

  s.menu_context = BeginMenuContext();
  ResetMenuContext(s.menu_context);
  AddMenuControlsToContext(DAT_0058c708, 3, s.menu_context);
  FUN_00500129(0);

  for (;;)
  {
    /* Enable/disable arrows based on scroll position */
    if (s.entry_count < 0xd)
    {
      RenderAdvMenuControlDisabled(&DAT_0058c708[0]);
      RenderAdvMenuControlDisabled(&DAT_0058c708[1]);
    }
    else
    {
      if (s.scroll_top_index == 0)
      {
        RenderAdvMenuControlDisabled(&DAT_0058c708[0]);
      }
      else
      {
        RenderAdvMenuControlNormally(&DAT_0058c708[0]);
      }

      if (s.scroll_top_index + 0xc < s.entry_count)
      {
        RenderAdvMenuControlNormally(&DAT_0058c708[1]);
      }
      else
      {
        RenderAdvMenuControlDisabled(&DAT_0058c708[1]);
      }
    }

    FUN_005001e3();

    LoadPcxResource(1, 0, global_screen_height - 0x1e0, s_dung_bd_pic_0058cb48, (void *)0);
    StretchBlitGraphicsRect(PTR_DAT_005832dc, 0, global_screen_height - 0x1e0, 0x280, 0x1e0, PTR_DAT_005832dc, 0, 0, global_screen_width,
                            global_screen_height);

    PTR_DAT_005832b4->page_number = 1;
    PTR_DAT_005832b4->font_slot = 1;

    /* Draw visible entries (up to 12) */
    s.visible_count = 0;
    for (i = s.scroll_top_index; (i < s.entry_count) && (i < s.scroll_top_index + 0xc); i++)
    {
      int dungeon_index = (int)s.dungeon_index_by_row[i];
      int row_index = s.visible_count;
      int entry_x;
      int entry_y;

      /* Name */
      strcpy(s.name_buffer[row_index], FUN_005081e0(dungeon_index));

      entry_x = ScaleUiCoordinate((-(unsigned int)((row_index & 1) == 0) & 0xfffffef9) + 0x19b);
      entry_y = ScaleUiCoordinate((row_index / 2) * 0x3e + 0x69);
      DrawFormattedTextShadowedCentered(PTR_DAT_005832b4, 0xff, entry_x, entry_y, DAT_0058cb24, s.name_buffer[row_index]);

      /* Icon: combine the two marker sprites into a single blob like the original */
      if (((g_castle_dungeon_slots[dungeon_index].clues_bitmap & 1) != 0) || (DAT_007894f4 != 0))
      {
        EncodedImage *pair[2];
        int marker_offset = dungeon_index * 2 - 10;
        pair[0] = g_location_marker_sprite_entries[g_dungeon_marker_sprite_lookup[marker_offset]];
        pair[1] = g_location_marker_sprite_entries[g_dungeon_marker_sprite_lookup[marker_offset + 1]];
        s.icon_blobs[row_index] =
            FUN_00508c4a(PTR_DAT_005832dc, 0, 0, ScaleUiCoordinate(0x3e), ScaleUiCoordinate(0x3e), (EncodedImage **)&pair[0]);

        DrawEncodedImageResampledFitBoxCentered(
            PTR_DAT_005832b4, ScaleUiCoordinate((-(unsigned int)((row_index & 1) == 0) & 0xfffffef9) + 0x15b),
            ScaleUiCoordinate((row_index / 2) * 0x3e + 0x49), ScaleUiCoordinate(0x3e), ScaleUiCoordinate(0x3e),
            s.icon_blobs[row_index]);
      }

      s.visible_count = s.visible_count + 1;
    }

    /* Blit the list panel to the visible page */
    BlitGraphicsRect(PTR_DAT_005832dc, ScaleUiCoordinate(0x50), ScaleUiCoordinate(0x46), ScaleUiCoordinate(0x208), ScaleUiCoordinate(0x17a), PTR_DAT_005832b4,
                     ScaleUiCoordinate(0x50), ScaleUiCoordinate(0x46));

    /* Input loop */
    DAT_00603a34 = -5;
    for (;;)
    {
      UpdateMouseSnapshot();
      UpdateMenuControlSelection(g_mouse_x_snapshot, g_mouse_y_snapshot, g_mouse_button_down_mask);

      /* Scroll via arrow controls */
      s.action_key = -1;
      if (DAT_00603a34 != -5)
      {
        if (DAT_00603a34 == -1)
        {
          s.action_key = 0x4800;
        }
        else if (DAT_00603a34 == 1)
        {
          s.action_key = 0x5000;
        }

        if (DAT_00603a34 != 0)
        {
          DAT_00603a34 = -5;
        }
      }

      if (s.action_key == 0x4800)
      {
        s.scroll_top_index = s.scroll_top_index - 2;
        if (s.scroll_top_index < 0)
        {
          s.scroll_top_index = 0;
        }
        PopNormalizedQueuedKeyInput();
        break;
      }
      if (s.action_key == 0x5000)
      {
        int max_top = ((s.entry_count + 2) & ~1) - 0xc;
        s.scroll_top_index = s.scroll_top_index + 2;
        if (s.scroll_top_index < 0)
        {
          s.scroll_top_index = 0;
        }
        if (max_top < s.scroll_top_index)
        {
          s.scroll_top_index = max_top;
        }
        PopNormalizedQueuedKeyInput();
        break;
      }

      /* Done */
      if (DAT_00603a34 != -5)
      {
        PopNormalizedQueuedKeyInput();
        EndMenuContext();
        FreeSpriteBlob(DAT_00746e50);
        for (i = 0; i < 50; i++)
        {
          if (s.icon_blobs[i] != (EncodedImage *)0)
          {
            FreeSpriteBlob(s.icon_blobs[i]);
          }
        }
        return;
      }

      /* Click on list entries (maps to detail screen) */
      if (g_mouse_button_down_mask != 0)
      {
        int mouse_y_280 = (g_mouse_y_snapshot * 0x1e0) / global_screen_height;
        int mouse_x_280 = (g_mouse_x_snapshot * 0x280) / global_screen_width;
        int index = -1;

        if (FUN_00508bf3(mouse_x_280, mouse_y_280, 0x54, 0x49, 0xfb, 0x174) != 0)
        {
          index = ((mouse_y_280 - 0x49) / 0x3e) * 2;
        }
        else if (FUN_00508bf3(mouse_x_280, mouse_y_280, 0x15b, 0x49, 0xfd, 0x174) != 0)
        {
          index = ((mouse_y_280 - 0x49) / 0x3e) * 2 + 1;
        }

        if ((0 <= index) && (index < s.visible_count))
        {
          PopNormalizedQueuedKeyInput();
          FUN_0050caa0((int)s.dungeon_index_by_row[s.scroll_top_index + index]);
          break;
        }
      }
    }
  }
}

// FUNCTION: SHANDALAR 0x0050caa0
void __cdecl FUN_0050caa0(int dungeon_index)
{
  struct
  {
    int title_colors[6];
    int menu_context;
    int text_y;
    int text_color;
    int i;
  } s;

  (void)dungeon_index;

  s.title_colors[0] = 0x3a;
  s.title_colors[1] = 0x3c;
  s.title_colors[2] = 0x3c;
  s.title_colors[3] = 0xab;
  s.title_colors[4] = 0x3a;

  if (DAT_00603a4c == 0)
  {
    DAT_00603a44 = LoadIniEscapedStringTable(g_advbuttons_ini_file, s_dunClues_0058ccac, (int)g_ini_string_scratch, 0);
    DAT_00603a4c = 1;
  }

  AnimatePaletteToColor(0, DAT_00589dec);
  LoadPcxIntoPageNoPalette(s_advfac64_pic_0058ccb8);
  LoadPcxIntoPage(1, s_cluebutn_pic_0058ccc8);

  BeginSpriteEncodeSession();
  PTR_DAT_005832dc->font_slot = 7;
  for (s.i = 0; s.i < 3; s.i++)
  {
    SetFontStyleSize(7, (unsigned int)((-(unsigned int)(s.i == 2) & 0xfffffffe) + 0xe));
    if (DAT_00603a44 != (int *)0)
    {
      DrawFormattedTextShadowedCentered(PTR_DAT_005832dc, s.title_colors[s.i], s.i * 0x5a + 0x2e, 0xf, DAT_0058cb24,
                                        (char *)DAT_00603a44[0]);
    }
    DAT_00746e10[s.i] = EncodeSpriteFromPage(1, s.i * 0x5a + 1, 1, 0x59, 0x23);
    DAT_00746eb0[s.i] = EncodeSpriteFromPage(1, s.i * 0x15 + 1, 0x25, 0x14, 0x24);
  }

  if (DAT_0058c6b0[0].x == DAT_0058c6b0[0].base_x)
  {
    DAT_0058c6b0[0].x = ScaleUiCoordinate(DAT_0058c6b0[0].x);
    DAT_0058c6b0[0].y = ScaleUiCoordinate(DAT_0058c6b0[0].y);
    DAT_0058c6b0[0].width = ScaleUiCoordinate(DAT_0058c6b0[0].width);
    DAT_0058c6b0[0].height = ScaleUiCoordinate(DAT_0058c6b0[0].height);
  }

  s.menu_context = BeginMenuContext();
  ResetMenuContext(s.menu_context);
  AddMenuControlsToContext(DAT_0058c6b0, 1, s.menu_context);
  FinalizeSpriteEncodeSession();

  LoadPcxResource(1, 0, global_screen_height - 0x1e0, s_clueback_pic_0058ccdc, (void *)0);
  StretchBlitGraphicsRect(PTR_DAT_005832dc, 0, global_screen_height - 0x1e0, 0x280, 0x1e0, PTR_DAT_005832dc, 0, 0, global_screen_width,
                          global_screen_height);
  CopyGraphicsRect(PTR_DAT_005832dc, 0, 0, global_screen_width, global_screen_height, PTR_DAT_005832b4, 0, 0);

  /* Render top caption box from page 0 (matches original layout) */
  BlitGraphicsRect(PTR_DAT_005832dc, FUN_005501dc(0x16), FUN_005501dc(0x1a2), ScaleUiCoordinate(0x5a), ScaleUiCoordinate(0x24), PTR_DAT_005832dc, 0, 0);
  FUN_005001e3();

  PTR_DAT_005832b4->font_slot = 4;

  /* Title */
  if (dungeon_index < 5)
  {
    FUN_00564e70(g_ui_message_buffer, 0x1000, s__s____s__0058ccec, FUN_005081e0(dungeon_index),
                 (char *)(DAT_0077de00 + (dungeon_index * 4 + 4) * 0x19));
    DrawTextLineClamped(g_ui_message_buffer, 0x12, 0x1c, s.title_colors[4]);
    s.text_y = FUN_005501dc(0x24);
  }
  else
  {
    strcpy(g_ui_message_buffer, FUN_005081e0(dungeon_index));
    DrawTextLineClamped(g_ui_message_buffer, 0x12, 0x1c, s.title_colors[4]);
    s.text_y = FUN_005501dc(0x1c);
  }

  /* Show up to 3 card prizes */
  for (s.i = 0; s.i < 3; s.i++)
  {
    int card_slot = (&g_castle_dungeon_slots[dungeon_index].card_slot_1)[s.i];
    if (card_slot != -1)
    {
      FUN_004f2407(card_slot, s.i * 0x29 + 0xa0, (-(unsigned int)(s.i == 0) & 8) + s.i * 4 + 0x76, 1, DAT_0058ccfc);
    }
  }

  /* Draw the cave/rules text */
  s.text_color = s.title_colors[3];
  if (((g_castle_dungeon_slots[dungeon_index].clues_bitmap & 2) != 0) || (DAT_007894f4 != 0))
  {
    strcpy(g_ui_message_buffer, gs_cave_showclues_0077efa0[0]);
    s.text_y = FUN_0050c973(s.text_y, s.text_color);
  }

  if (((g_castle_dungeon_slots[dungeon_index].clues_bitmap & 1) != 0) || (DAT_007894f4 != 0))
  {
    strcpy(g_ui_message_buffer, gs_cave_showclues_0077efa0[0x15]);
    s.text_y = FUN_0050c973(s.text_y, s.text_color);
  }

  /* Wait for Done */
  DAT_00603a34 = -1;
  while (DAT_00603a34 == -1)
  {
    UpdateMouseSnapshot();
    UpdateMenuControlSelection(g_mouse_x_snapshot, g_mouse_y_snapshot, g_mouse_button_down_mask);
  }

  ClearInputAndWaitForMouseRelease();
  EndMenuContext();
  FreeSpriteBlob(DAT_00746e10[0]);
}
