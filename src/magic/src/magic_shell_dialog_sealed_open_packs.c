#include <stdio.h>
#include <string.h>
#include "cardartlib/src/palette.h"
#include "drawcardlib/Drawcardlib.h"
#include "global_strings.h"
#include "deckdll/src/card_db.h"
#include "magic_sealed_open_packs.h"
#include "magic_shell_dialogs.h"
#include "magic_shell.h"
#include "game_support.h"
#include "global_other.h"
#include "global_state.h"
#include "shared_startup.h"

extern char g_exp1_art_path[];
extern char global_base_directory[];
extern HWND global_main_hwnd;
void shell_set_map_mode(HDC dc, HWND hwnd, HBITMAP background);
static int shell_draw_sealed_open_pack_choices(HDC dc, int count, RECT *rects, int selected_pack,
    HBITMAP highlight);
static int shell_draw_sealed_open_pack(HDC dc, RECT *rect, sealed_deck_pack_t *pack);
static int shell_draw_sealed_revealed_cards(HDC dc, sealed_deck_pack_t *pack, RECT *cards_rect, RECT *clip_rect);
static void shell_draw_sealed_finish_button(DRAWITEMSTRUCT *item, HBRUSH brush, HPEN light_pen, HPEN dark_pen,
    COLORREF color, int focus);

BOOL DrawMaskedBitmapToRect(HDC dc, RECT *rect, HANDLE bitmap, int width, int height, int src_x, int src_y,
    int mask_x, int mask_y);
void draw_item(DRAWITEMSTRUCT *item, HBRUSH brush, HANDLE bitmap, HPEN light_pen, HPEN dark_pen,
    COLORREF color, int focus, int flags);

// FUNCTION: MAGIC 0x004955dd
static void shell_draw_sealed_finish_button(DRAWITEMSTRUCT *item, HBRUSH brush, HPEN light_pen, HPEN dark_pen,
    COLORREF color, int focus)
{
  draw_item(item, brush, NULL, light_pen, dark_pen, color, focus, 1);
}

// FUNCTION: MAGIC 0x004c3548
static int shell_draw_sealed_pack_highlight(HDC dc, RECT *rect, int is_starter, HBITMAP highlight)
{
  struct
  {
    BITMAP bitmap;
    int height;
    int width;
    int source_y;
    int source_x;
    int mask_y;
    RECT destination;
    int mask_x;
    RECT highlighted_rect;
  } s;

  if (dc == NULL || rect == NULL)
  return 0;
  if (highlight != NULL)

  {
    GetObjectA(highlight, sizeof(s.bitmap), &s.bitmap);
    s.width = s.bitmap.bmWidth / 4;
    s.height = s.bitmap.bmHeight;
    shell_fit_sealed_pack_rect(&s.destination, rect, s.width, s.height);
    if (is_starter != 0)

    {
      s.source_x = 0;
      s.source_y = 0;
      s.mask_x = s.width;
      s.mask_y = 0;
    }
    else

    {
      s.source_x = s.width * 2;
      s.source_y = 0;
      s.mask_x = s.width * 3;
      s.mask_y = 0;
    }
    CopyRect(&s.highlighted_rect, &s.destination);
    InflateRect(&s.highlighted_rect, ((s.destination.right - s.destination.left) * 35) / 1000,
        ((s.destination.bottom - s.destination.top) * 39) / 1000);
    DrawMaskedBitmapToRect(dc, &s.highlighted_rect, highlight, s.width, s.height, s.source_x, s.source_y,
        s.mask_x, s.mask_y);
  }
}

// FUNCTION: MAGIC 0x004c74ac
static int shell_draw_sealed_open_pack_choices(HDC dc, int count, RECT *rects, int selected_pack,
    HBITMAP highlight)
{
  struct
  {
    int pack_index;
    RECT rect;
  } s;

  for (s.pack_index = 0; s.pack_index < count; ++s.pack_index)

  {
    CopyRect(&s.rect, &rects[s.pack_index]);
    if (s.pack_index == selected_pack)

    {
      shell_draw_sealed_pack_highlight(dc, &s.rect, g_sealed_deck_open_packs[s.pack_index].is_starter, highlight);
      if (g_sealed_deck_open_packs[s.pack_index].is_starter != 0)
      shell_draw_sealed_starter_pack(dc, &s.rect, g_sealed_deck_open_packs[s.pack_index].pack_type,
          g_sealed_deck_open_packs[s.pack_index].variation, g_sealed_deck_open_packs[s.pack_index].opened);
      else
      shell_draw_sealed_booster_pack(dc, &s.rect, g_sealed_deck_open_packs[s.pack_index].pack_type,
          g_sealed_deck_open_packs[s.pack_index].variation, g_sealed_deck_open_packs[s.pack_index].opened);
    }
    else if (g_sealed_deck_open_packs[s.pack_index].is_starter != 0)
    shell_draw_sealed_starter_pack(dc, &s.rect, g_sealed_deck_open_packs[s.pack_index].pack_type,
        g_sealed_deck_open_packs[s.pack_index].variation, g_sealed_deck_open_packs[s.pack_index].opened);
    else
    shell_draw_sealed_booster_pack(dc, &s.rect, g_sealed_deck_open_packs[s.pack_index].pack_type,
        g_sealed_deck_open_packs[s.pack_index].variation, g_sealed_deck_open_packs[s.pack_index].opened);
  }
  return 0;
}

// FUNCTION: MAGIC 0x004c76a8
static int shell_draw_sealed_open_pack(HDC dc, RECT *rect, sealed_deck_pack_t *pack)
{
  RECT destination;

  CopyRect(&destination, rect);
  if (pack != NULL)

  {
    if (pack->is_starter != 0)
    shell_draw_large_sealed_starter(dc, &destination, pack->pack_type, pack->variation, pack->opened,
        pack->card_count - pack->revealed_card_index - 1);
    else
    shell_draw_large_sealed_booster(dc, &destination, pack->pack_type, pack->variation, pack->opened,
        pack->card_count - pack->revealed_card_index - 1);
  }
  return 0;
}

// FUNCTION: MAGIC 0x004c7758
static int shell_draw_sealed_revealed_cards(HDC dc, sealed_deck_pack_t *pack, RECT *cards_rect, RECT *clip_rect)
{
  struct
  {
    int stack_index;
    RECT device_rect;
    RECT hidden_rect;
    int card_index;
    RECT card_rect;
    int has_hidden_cards;
    RECT cards_rect;
    RECT clip_rect;
    int card_height;
  } s;

  CopyRect(&s.cards_rect, cards_rect);
  CopyRect(&s.clip_rect, clip_rect);
  if (pack != NULL && pack->opened != 0)

  {
    CopyRect(&s.device_rect, &s.clip_rect);
    LPtoDP(dc, (POINT *)&s.device_rect, 2);
    SetMapMode(dc, MM_TEXT);
    s.card_height = ((s.device_rect.bottom - s.device_rect.top) * 65) / 100;
    SetTextAlign(dc, 0);
    s.stack_index = pack->revealed_card_index;
    s.has_hidden_cards = 0;
    for (s.card_index = 0; s.card_index <= pack->revealed_card_index;
        ++s.card_index)

    {
      if (shell_sealed_open_card_rect(&s.card_rect, s.stack_index, pack->revealed_card_index + 1,
          &s.device_rect) != 0)
      {
        s.card_rect.bottom = s.card_rect.top + s.card_height;
        DrawFullCard(dc, &s.card_rect, &global_raw_cards_storage[pack->cards[s.card_index]], 0, 2,
            g_duel_interface_options.expand_text_box_on_big_card, gs_illus_00789130);
      }
      else
      {
        s.has_hidden_cards = 1;
        CopyRect(&s.hidden_rect, &s.card_rect);
      }
      --s.stack_index;
    }
  }
  return 0;
}

// GLOBAL: MAGIC 0x0064fac4
sealed_deck_pack_t *g_sealed_deck_open_packs;
// GLOBAL: MAGIC 0x0069b1dc
int g_sealed_deck_open_pack_count;

// GLOBAL: MAGIC 0x0069b3a4
static HBITMAP g_open_packs_background;
// GLOBAL: MAGIC 0x0069aa28
static HBITMAP g_open_packs_highlight;
// GLOBAL: MAGIC 0x0064fae4
static HDC g_open_packs_background_dc;
// GLOBAL: MAGIC 0x0064fae8
static HBRUSH g_open_packs_button_brush;
// GLOBAL: MAGIC 0x0069af30
static HPEN g_open_packs_light_pen;
// GLOBAL: MAGIC 0x0069b324
static HPEN g_open_packs_dark_pen;
// GLOBAL: MAGIC 0x0069b31c
static COLORREF g_open_packs_button_color;
// GLOBAL: MAGIC 0x0069aec0
static COLORREF g_open_packs_title_color;
// GLOBAL: MAGIC 0x0069b3a8
static COLORREF g_open_packs_text_color;
// GLOBAL: MAGIC 0x0069aebc
static COLORREF g_open_packs_help_color;
// GLOBAL: MAGIC 0x0064f9a8
static HFONT g_open_packs_title_font;
// GLOBAL: MAGIC 0x0064fa4c
static HFONT g_open_packs_description_font;
// GLOBAL: MAGIC 0x0069b3a0
static HFONT g_open_packs_help_font;
// GLOBAL: MAGIC 0x0069b314
static int g_open_packs_selected_pack;
// GLOBAL: MAGIC 0x0069b328
static int g_open_packs_reserved_69b328;
// GLOBAL: MAGIC 0x0069aec4
static int g_open_packs_reserved_69aec4;
// GLOBAL: MAGIC 0x0069aff0
static int g_open_packs_background_width;
// GLOBAL: MAGIC 0x0064fa58
static int g_open_packs_background_height;
// GLOBAL: MAGIC 0x0064fb08
static RECT g_open_packs_title_rect;
// GLOBAL: MAGIC 0x0069b330
static RECT g_open_packs_pack_rects[6];
// GLOBAL: MAGIC 0x0064faf0
static RECT g_open_packs_opened_pack_rect;
// GLOBAL: MAGIC 0x0064f998
static RECT g_open_packs_cards_rect;
// GLOBAL: MAGIC 0x0069ae40
static RECT g_open_packs_starters_rect;
// GLOBAL: MAGIC 0x0069aee8
static RECT g_open_packs_boosters_rect;
// GLOBAL: MAGIC 0x0069af08
static RECT g_open_packs_selection_help_rect;
// GLOBAL: MAGIC 0x0069aec8
static RECT g_open_packs_opening_help_rect;
// GLOBAL: MAGIC 0x0069af20
static RECT g_open_packs_cards_help_rect;
// GLOBAL: MAGIC 0x0069af88
static char g_open_packs_title[100];
// GLOBAL: MAGIC 0x0064f9b0
static char g_open_packs_starter_description[100];
// GLOBAL: MAGIC 0x0064fb20
static char g_open_packs_booster_description[100];
// GLOBAL: MAGIC 0x0069b1e0
static char g_open_packs_selection_help[100];
// GLOBAL: MAGIC 0x0069ae50
static char g_open_packs_opening_help[100];
// GLOBAL: MAGIC 0x0069b248
static char g_open_packs_cards_help[100];

// FUNCTION: MAGIC 0x004c5b89
BOOL CALLBACK shell_sealed_open_packs_dialog_proc(
    HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
  struct
  {
    HWND finish_button;
    CHAR button_text[100];
    RECT button_text_rect;
    LOGFONTA resize_font_description;
    HDC resize_dc;
    int button_height;
    HFONT resize_font;
    int button_width;
    int character_width;
    HGDIOBJ button_font;
    RECT resize_rect;
    SIZE text_size;
    HANDLE old_button_font;
    HDC paint_dc;
    HDC offscreen_dc;
    PAINTSTRUCT paint;
    int saved_dc;
    RECT update_rect;
    RECT client_rect;
    sealed_deck_pack_t *paint_pack;
    int help_card_index;
    char card_help_path[264];
    DWORD help_context;
    POINT right_click_point;
    HDC right_click_dc;
    RECT moved_rect;
    int clicked_card;
    int stack_index;
    int cards_to_hide;
    RECT stack_card_rect;
    WPARAM pack_control_id;
    int hit_pack_index;
    MSG double_click_message;
    POINT click_point;
    HDC click_dc;
    RECT all_packs_rect;
    int handled_click;
    BOOL double_click;
    DRAWITEMSTRUCT *draw_item;
    HWND colored_control;
    int colored_control_id;
    HDC control_dc;
    HBRUSH control_brush;
    HWND new_focus;
    HWND old_focus;
    HDC display_dc;
    RECT closed_rect;
    int command_id;
    RECT selection_rect;
    char help_path[264];
    int description_width;
    int description_margin;
    int starter_count;
    int pack_index;
    LOGFONTA font_description;
    int initialized;
    BITMAP bitmap;
    HANDLE dialog_font;
    HFONT finish_font;
    RECT owner_rect;
  } s;
  switch (message)

  {
  case WM_INITDIALOG:

    s.initialized = 1;
    SetWindowTextA(hwnd, gs_magic_the_gathering_title_00789460);
    shell_load_sealed_open_packs_resources(&g_open_packs_background, &g_open_packs_highlight,
        &g_open_packs_title_rect, g_open_packs_pack_rects, &g_open_packs_opened_pack_rect,
        &g_open_packs_cards_rect, &g_open_packs_title_color, &g_open_packs_text_color,
        &g_open_packs_help_color, &g_open_packs_button_brush, &g_open_packs_light_pen, &g_open_packs_dark_pen,
        &g_open_packs_button_color);
    if (g_open_packs_background != NULL)
    {
      GetObjectA(g_open_packs_background, sizeof(BITMAP), &s.bitmap);
      g_open_packs_background_width = s.bitmap.bmWidth;
      g_open_packs_background_height = s.bitmap.bmHeight;
      g_open_packs_background_dc = CreateCompatibleDC(NULL);
      SelectObject(g_open_packs_background_dc, g_open_packs_background);
    }
    s.dialog_font = (HANDLE)SendMessageA(hwnd, 0x31, 0, 0);
    GetObjectA(s.dialog_font, sizeof(LOGFONTA), &s.font_description);
    s.font_description.lfHeight = 0x16;
    s.font_description.lfItalic = '\x01';
    s.font_description.lfWeight = 700;
    g_open_packs_title_font = CreateFontIndirectA(&s.font_description);
    GetObjectA(s.dialog_font, sizeof(LOGFONTA), &s.font_description);
    s.font_description.lfHeight = 0x10;
    g_open_packs_description_font = CreateFontIndirectA(&s.font_description);
    GetObjectA(s.dialog_font, sizeof(LOGFONTA), &s.font_description);
    s.font_description.lfHeight = 0x16;
    g_open_packs_help_font = CreateFontIndirectA(&s.font_description);
    GetObjectA(s.dialog_font, sizeof(LOGFONTA), &s.font_description);
    s.finish_font = CreateFontIndirectA(&s.font_description);
    SendDlgItemMessageA(hwnd, 1, 0x30, (WPARAM)s.finish_font, 0);
    g_open_packs_selected_pack = -1;
    g_open_packs_reserved_69b328 = 3;
    g_open_packs_reserved_69aec4 = 2;
    LoadTextSectionLines(global_ui_strings_filename, "SEALEDDECK_FOILPACKSCREEN");
    strcpy(g_open_packs_title, g_text_lines[0]);
    for (s.pack_index = 0, s.starter_count = 0; s.pack_index < g_sealed_deck_open_pack_count; s.pack_index = s.pack_index + 1)
    {
      if (g_sealed_deck_open_packs[s.pack_index].is_starter != 0)
      {
        s.starter_count = s.starter_count + 1;
      }
    }
    sprintf(g_open_packs_starter_description, g_text_lines[1], s.starter_count);
    sprintf(g_open_packs_booster_description, g_text_lines[2], g_sealed_deck_open_pack_count - s.starter_count);
    for (s.pack_index = 0; s.pack_index < g_sealed_deck_open_pack_count; s.pack_index = s.pack_index + 1)
    {
      if (g_sealed_deck_open_packs[s.pack_index].is_starter != 0)
      {
        sprintf(g_open_packs_starter_description + strlen(g_open_packs_starter_description), "\n   \x95 %s",
            g_sealed_pack_definitions[g_sealed_deck_open_packs[s.pack_index].pack_type].name);
      }
      else
      {
        sprintf(g_open_packs_booster_description + strlen(g_open_packs_booster_description), "\n   \x95 %s",
            g_sealed_pack_definitions[g_sealed_deck_open_packs[s.pack_index].pack_type].name);
      }
    }
    strcpy(g_open_packs_selection_help, g_text_lines[3]);
    strcpy(g_open_packs_opening_help, g_text_lines[4]);
    strcpy(g_open_packs_cards_help, g_text_lines[5]);
    SetDlgItemTextA(hwnd, 1, g_text_lines[6]);
    SetRect(&g_open_packs_selection_help_rect, g_open_packs_pack_rects[0].left,
        g_open_packs_pack_rects[0].top + -0x19, g_open_packs_pack_rects[5].right,
        g_open_packs_pack_rects[0].top + -5
        );
    SetRect(&g_open_packs_opening_help_rect, g_open_packs_opened_pack_rect.left,
        g_open_packs_opened_pack_rect.top + -0x19, g_open_packs_opened_pack_rect.right,
        g_open_packs_opened_pack_rect.top + -5
        );
    SetRect(&g_open_packs_cards_help_rect, g_open_packs_title_rect.right + 10, g_open_packs_cards_rect.top,
        g_open_packs_cards_rect.left + -10, g_open_packs_cards_rect.bottom);
    s.description_width = ((g_open_packs_title_rect.right - g_open_packs_title_rect.left) * 0x28) / 100;
    s.description_margin = ((g_open_packs_title_rect.right - g_open_packs_title_rect.left) * 10) / 100;
    SetRect(&g_open_packs_starters_rect, g_open_packs_title_rect.left + s.description_margin,
        g_open_packs_title_rect.top + 0x19,
        (g_open_packs_title_rect.left + s.description_margin) + s.description_width,
        g_open_packs_title_rect.bottom);
    SetRect(&g_open_packs_boosters_rect,
        (g_open_packs_title_rect.right - s.description_margin) - s.description_width,
        g_open_packs_title_rect.top + 0x19, g_open_packs_title_rect.right - s.description_margin,
        g_open_packs_title_rect.bottom);
    SetFocus(GetDlgItem(hwnd, 1));
    SendMessageA(hwnd, 0x401, 1, 0);
    change_buttonclass_wndproc(hwnd);
    GetWindowRect(global_main_hwnd, &s.owner_rect);
    MoveWindow(hwnd, s.owner_rect.left, s.owner_rect.top, s.owner_rect.right - s.owner_rect.left,
        s.owner_rect.bottom - s.owner_rect.top, 1);
    if (s.initialized == 0)
    {
      EndDialog(hwnd, -1);
    }
    return FALSE;

  case WM_DESTROY:

    strcpy(s.help_path, global_base_directory);
    strcat(s.help_path, "\\duel.hlp");
    WinHelpA(global_main_hwnd, s.help_path, 2, 0);
    DeleteDC(g_open_packs_background_dc);
    shell_release_sealed_open_packs_resources(g_open_packs_background, g_open_packs_highlight,
        g_open_packs_button_brush, g_open_packs_light_pen, g_open_packs_dark_pen);
    DeleteObject(g_open_packs_title_font);
    DeleteObject(g_open_packs_description_font);
    DeleteObject(g_open_packs_help_font);
    return FALSE;

  case WM_COMMAND:

    s.command_id = (uint)wparam & 0xffff;
    if (s.command_id == 1)
    {
      GetWindowRect(hwnd, &s.closed_rect);
      ShowWindow(hwnd, 0);
      s.display_dc = CreateDCA("display", (LPCSTR)0x0, (LPCSTR)0x0, (DEVMODEA *)0x0);
      FillRect(s.display_dc, &s.closed_rect, GetStockObject(BLACK_BRUSH));
      DeleteDC(s.display_dc);
      EndDialog(hwnd, 0);
    }
    else if ((0x67f <= s.command_id) && ((int)s.command_id <= g_sealed_deck_open_pack_count + 0x67f))
    {
      if (g_open_packs_selected_pack != -1)
      {
        CopyRect(&s.selection_rect, &g_open_packs_pack_rects[g_open_packs_selected_pack]);
        InflateRect(&s.selection_rect, 10, 10);
        shell_invalidate_logical_rect(hwnd, g_open_packs_background, &s.selection_rect);
      }
      g_open_packs_selected_pack = s.command_id - 0x67f;
      CopyRect(&s.selection_rect, &g_open_packs_pack_rects[g_open_packs_selected_pack]);
      InflateRect(&s.selection_rect, 10, 10);
      shell_invalidate_logical_rect(hwnd, g_open_packs_background, &s.selection_rect);
      shell_invalidate_logical_rect(hwnd, g_open_packs_background, &g_open_packs_opened_pack_rect);
      shell_invalidate_logical_rect(hwnd, g_open_packs_background, &g_open_packs_cards_rect);
    }
    return TRUE;

  case 0x4c8:

    s.old_focus = (HWND)wparam;
    s.new_focus = (HWND)lparam;
    if (s.old_focus != NULL)
    {
      SendMessageA(hwnd, 0x401, (WPARAM)s.old_focus, 0);
    }
    if (s.old_focus != NULL)
    {
      InvalidateRect((HWND)s.old_focus, NULL, 1);
    }
    if (s.new_focus != NULL)
    {
      InvalidateRect(s.new_focus, NULL, 1);
    }
    return FALSE;

  case WM_CTLCOLORSTATIC:
  case WM_CTLCOLORBTN:

    s.control_dc = (HDC)wparam;
    ApplyCardArtPaletteToDc(s.control_dc);
    s.colored_control = (HWND)lparam;
    s.colored_control_id = GetDlgCtrlID(s.colored_control);
    if (s.colored_control_id == 0x67c)
    {
      SetTextColor(s.control_dc, g_open_packs_title_color);
    }
    else if ((s.colored_control_id == 0x682) || (s.colored_control_id == 0x681))
    {
      SetTextColor(s.control_dc, g_open_packs_help_color);
    }
    else
    {
      SetTextColor(s.control_dc, g_open_packs_text_color);
    }
    SetBkMode(s.control_dc, 1);
    s.control_brush = GetStockObject(NULL_BRUSH);
    return (BOOL)s.control_brush;

  case WM_DRAWITEM:

    s.draw_item = (DRAWITEMSTRUCT *)lparam;
    if (s.draw_item->CtlID == 1)
    {
      shell_draw_sealed_finish_button(s.draw_item, g_open_packs_button_brush, g_open_packs_light_pen,
          g_open_packs_dark_pen, g_open_packs_button_color, 0);
    }
    else
    {
      draw_owner_draw_button_centered(s.draw_item, g_open_packs_button_brush, g_open_packs_light_pen,
          g_open_packs_dark_pen, g_open_packs_button_color, 0);
    }
    return TRUE;

  case WM_QUERYNEWPALETTE:
  case WM_PALETTEISCHANGING:
  case WM_PALETTECHANGED:
    return handle_button_palette_message((int)hwnd, message, (int)wparam, lparam);
  case WM_ERASEBKGND:

    return TRUE;

  case WM_LBUTTONDOWN:

    s.click_point.x = (uint)lparam & 0xffff;
    s.click_point.y = (WORD)(((uint)lparam >> 16) & 0xffff);
    s.click_dc = GetDC(hwnd);
    shell_set_map_mode(s.click_dc, hwnd, g_open_packs_background);
    DPtoLP(s.click_dc, &s.click_point, 1);
    ReleaseDC(hwnd, s.click_dc);
    Sleep(GetDoubleClickTime());
    s.double_click = PeekMessageA(&s.double_click_message, hwnd, 0x203, 0x203, 1);
    SetRect(&s.all_packs_rect, g_open_packs_pack_rects[0].left, g_open_packs_pack_rects[0].top,
        g_open_packs_pack_rects[5].right, g_open_packs_pack_rects[5].bottom);
    InflateRect(&s.all_packs_rect, 10, 10);
    s.handled_click = 0;
    if (PtInRect(&s.all_packs_rect, s.click_point) != 0)
    {
      s.handled_click = 1;
      s.pack_control_id = 0;
      for (s.hit_pack_index = 0;
          s.hit_pack_index < g_sealed_deck_open_pack_count && s.pack_control_id == 0;
          ++s.hit_pack_index)
      {
        if (PtInRect(&g_open_packs_pack_rects[s.hit_pack_index], s.click_point) != 0)
        {
          s.pack_control_id = s.hit_pack_index + 0x67f;
        }
      }
      if (s.pack_control_id != 0)
      {
        SendMessageA(hwnd, 0x111, s.pack_control_id, 0);
      }
    }
    else
    {
      if ((PtInRect(&g_open_packs_opened_pack_rect, s.click_point) != 0) && (g_open_packs_selected_pack != -1))
      {
        s.handled_click = 1;
        if (g_sealed_deck_open_packs[g_open_packs_selected_pack].opened == 0)
        {
          g_sealed_deck_open_packs[g_open_packs_selected_pack].revealed_card_index = -1;
          g_sealed_deck_open_packs[g_open_packs_selected_pack].opened = 1;
          shell_invalidate_logical_rect(hwnd, g_open_packs_background,
              &g_open_packs_pack_rects[g_open_packs_selected_pack]);
          shell_invalidate_logical_rect(hwnd, g_open_packs_background, &g_open_packs_opened_pack_rect);
          if (g_sealed_deck_open_packs[g_open_packs_selected_pack].is_starter != 0)
          {
            play_sound_effect(0x42);
          }
          else
          {
            play_sound_effect(0x41);
          }
        }
        shell_reveal_sealed_pack_cards(hwnd, &g_sealed_deck_open_packs[g_open_packs_selected_pack], 0,
            s.double_click ? 4 : 1, g_open_packs_background, &g_open_packs_opened_pack_rect
            , &g_open_packs_cards_rect);
      }
      else
      {
        if ((PtInRect(&g_open_packs_cards_rect, s.click_point) != 0) &&
            ((g_open_packs_selected_pack != -1 && (g_sealed_deck_open_packs[g_open_packs_selected_pack].revealed_card_index != -1)))
            )
        {
          s.handled_click = 1;
          s.clicked_card = -1;
          for (s.stack_index = 0;
              s.stack_index < 8 && s.clicked_card == -1; ++s.stack_index)
          {
            if (shell_sealed_open_card_rect(&s.stack_card_rect, s.stack_index,
                g_sealed_deck_open_packs[g_open_packs_selected_pack].revealed_card_index + 1,
                &g_open_packs_cards_rect) != 0 && PtInRect(&s.stack_card_rect, s.click_point) != 0)
            {
              s.clicked_card = g_sealed_deck_open_packs[g_open_packs_selected_pack].revealed_card_index - s.stack_index;
            }
          }
          if (s.clicked_card == -1)
          {
            s.cards_to_hide = 4;
          }
          else if (g_sealed_deck_open_packs[g_open_packs_selected_pack].revealed_card_index == s.clicked_card)
          {
            s.cards_to_hide = 1;
          }
          else
          {
            s.cards_to_hide = g_sealed_deck_open_packs[g_open_packs_selected_pack].revealed_card_index - s.clicked_card;
          }
          shell_reveal_sealed_pack_cards(hwnd, &g_sealed_deck_open_packs[g_open_packs_selected_pack], 1,
              s.cards_to_hide, g_open_packs_background, &g_open_packs_opened_pack_rect, &g_open_packs_cards_rect);
        }
      }
    }
    if (s.handled_click == 0)
    {
      SendMessageA(hwnd, 0x112, 0xf012, 0);
      GetWindowRect(hwnd, &s.moved_rect);
      SetWindowPos(global_main_hwnd, NULL, s.moved_rect.left, s.moved_rect.top, 0, 0, 5);
    }
    return TRUE;

  case WM_RBUTTONDBLCLK:

    s.right_click_point.x = (uint)lparam & 0xffff;
    s.right_click_point.y = (WORD)(((uint)lparam >> 16) & 0xffff);
    s.right_click_dc = GetDC(hwnd);
    shell_set_map_mode(s.right_click_dc, hwnd, g_open_packs_background);
    DPtoLP(s.right_click_dc, &s.right_click_point, 1);
    ReleaseDC(hwnd, s.right_click_dc);
    if ((PtInRect(&g_open_packs_opened_pack_rect, s.right_click_point) != 0) && (g_open_packs_selected_pack != -1))
    {
      if (g_sealed_deck_open_packs[g_open_packs_selected_pack].opened == 0)
      {
        g_sealed_deck_open_packs[g_open_packs_selected_pack].revealed_card_index = -1;
        g_sealed_deck_open_packs[g_open_packs_selected_pack].opened = 1;
        shell_invalidate_logical_rect(hwnd, g_open_packs_background,
            &g_open_packs_pack_rects[g_open_packs_selected_pack]);
        shell_invalidate_logical_rect(hwnd, g_open_packs_background, &g_open_packs_opened_pack_rect);
      }
      shell_reveal_sealed_pack_cards(hwnd, &g_sealed_deck_open_packs[g_open_packs_selected_pack], 2, 0,
          g_open_packs_background, &g_open_packs_opened_pack_rect, &g_open_packs_cards_rect);
    }
    else
    {
      if ((PtInRect(&g_open_packs_cards_rect, s.right_click_point) != 0) &&
          ((g_open_packs_selected_pack != -1 && (g_sealed_deck_open_packs[g_open_packs_selected_pack].opened != 0))))
      {
        shell_reveal_sealed_pack_cards(hwnd, &g_sealed_deck_open_packs[g_open_packs_selected_pack], 3, 0,
            g_open_packs_background, &g_open_packs_opened_pack_rect, &g_open_packs_cards_rect);
      }
    }
    return TRUE;

  case WM_HELP:

    if (((g_open_packs_selected_pack != -1) && (g_sealed_deck_open_packs[g_open_packs_selected_pack].opened != 0)) &&
        (g_sealed_deck_open_packs[g_open_packs_selected_pack].revealed_card_index != -1))
    {
      s.help_card_index = g_sealed_deck_open_packs[g_open_packs_selected_pack].revealed_card_index;
      s.help_context = global_raw_cards_storage[g_sealed_deck_open_packs[g_open_packs_selected_pack].cards[s.help_card_index]].id;
      if (s.help_context != 0xffffffff)
      {
        strcpy(s.card_help_path, global_base_directory);
        strcat(s.card_help_path, "\\duel.hlp");
        WinHelpA(g_duel_window_hwnd, s.card_help_path, 1, s.help_context);
      }
    }
    return TRUE;

  case WM_PAINT:

    if (g_open_packs_selected_pack != -1)
    {
      s.paint_pack = &g_sealed_deck_open_packs[g_open_packs_selected_pack];
    }
    else
    {
      s.paint_pack = NULL;
    }
    LoadDuelInterfaceRegistryOptions();
    EnterCriticalSection(&g_card_render_lock);
    s.offscreen_dc = g_shared_offscreen_dc;
    s.saved_dc = SaveDC(g_shared_offscreen_dc);
    ApplyCardArtPaletteToDc(s.offscreen_dc);
    GetClientRect(hwnd, &s.client_rect);
    GetUpdateRect(hwnd, &s.update_rect, 0);
    IntersectClipRect(s.offscreen_dc, s.update_rect.left, s.update_rect.top, s.update_rect.right,
        s.update_rect.bottom);
    s.paint_dc = BeginPaint(hwnd, &s.paint);
    shell_draw_sealed_open_packs_background(s.offscreen_dc, &s.client_rect, g_open_packs_background_dc,
        g_open_packs_background, g_open_packs_background_width, g_open_packs_background_height);
    shell_set_map_mode(s.offscreen_dc, hwnd, g_open_packs_background);
    SetBkMode(s.offscreen_dc, 1);
    SelectObject(s.offscreen_dc, g_open_packs_title_font);
    SetTextColor(s.offscreen_dc, g_open_packs_title_color);
    DrawTextA(s.offscreen_dc, g_open_packs_title, -1, &g_open_packs_title_rect, 1);
    SelectObject(s.offscreen_dc, g_open_packs_description_font);
    SetTextColor(s.offscreen_dc, g_open_packs_text_color);
    DrawTextA(s.offscreen_dc, g_open_packs_starter_description, -1, &g_open_packs_starters_rect, 0);
    DrawTextA(s.offscreen_dc, g_open_packs_booster_description, -1, &g_open_packs_boosters_rect, 0);
    SelectObject(s.offscreen_dc, g_open_packs_help_font);
    SetTextColor(s.offscreen_dc, g_open_packs_text_color);
    DrawTextA(s.offscreen_dc, g_open_packs_selection_help, -1, &g_open_packs_selection_help_rect, 0x29);
    SetTextColor(s.offscreen_dc, g_open_packs_help_color);
    DrawTextA(s.offscreen_dc, g_open_packs_opening_help, -1, &g_open_packs_opening_help_rect, 0x29);
    DrawTextA(s.offscreen_dc, g_open_packs_cards_help, -1, &g_open_packs_cards_help_rect, 2);
    shell_draw_sealed_open_pack_choices(s.offscreen_dc, g_sealed_deck_open_pack_count,
        g_open_packs_pack_rects, g_open_packs_selected_pack, g_open_packs_highlight);
    shell_draw_sealed_open_pack(s.offscreen_dc, &g_open_packs_opened_pack_rect, s.paint_pack);
    shell_draw_sealed_revealed_cards(s.offscreen_dc, s.paint_pack, &g_open_packs_cards_rect,
        &g_open_packs_cards_rect);
    RestoreDC(g_shared_offscreen_dc, s.saved_dc);
    s.offscreen_dc = s.paint_dc;
    ApplyCardArtPaletteToDc(s.offscreen_dc);
    GetClientRect(hwnd, &s.client_rect);
    BitBlt(s.offscreen_dc, s.update_rect.left, s.update_rect.top, s.update_rect.right - s.update_rect.left,
        s.update_rect.bottom - s.update_rect.top, g_shared_offscreen_dc, s.update_rect.left,
        s.update_rect.top, 0xcc0020);
    EndPaint(hwnd, &s.paint);
    LeaveCriticalSection(&g_card_render_lock);
    return TRUE;

  case WM_SIZE:

    s.old_button_font = (HANDLE)SendDlgItemMessageA(hwnd, 1, 0x31, 0, 0);
    GetObjectA(s.old_button_font, sizeof(LOGFONTA), &s.resize_font_description);
    GetClientRect(hwnd, &s.resize_rect);
    s.resize_font_description.lfHeight = (s.resize_rect.bottom - s.resize_rect.top) / 0x28;
    if (s.resize_font_description.lfHeight % 2 != 0)
    {
      s.resize_font_description.lfHeight = s.resize_font_description.lfHeight + 1;
    }
    s.resize_font = CreateFontIndirectA(&s.resize_font_description);
    SendDlgItemMessageA(hwnd, 1, 0x30, (WPARAM)s.resize_font, 0);
    DeleteObject(s.old_button_font);
    s.resize_dc = GetDC(hwnd);
    s.button_font = (HGDIOBJ)SendDlgItemMessageA(hwnd, 1, 0x31, 0, 0);
    SelectObject(s.resize_dc, s.button_font);
    GetTextExtentPoint32A(s.resize_dc, "W", 1, &s.text_size);
    s.character_width = s.text_size.cx;
    s.finish_button = GetDlgItem(hwnd, 1);
    GetWindowTextA(s.finish_button, s.button_text, 100);
    s.button_width = s.character_width * 2;
    SetRect(&s.button_text_rect, 0, 0, s.button_width, 10);
    s.button_height = DrawTextA(s.resize_dc, s.button_text, -1, &s.button_text_rect, 0x401);
    shell_logical_rect_to_device(hwnd, g_open_packs_background, &g_open_packs_title_rect, &s.resize_rect);
    SetWindowPos(s.finish_button, NULL, s.resize_rect.left + 5, (s.resize_rect.bottom + -5) - s.button_height,
        s.button_width
        , s.button_height, 4);
    ReleaseDC(hwnd, s.resize_dc);
    InvalidateRect(hwnd, NULL, 1);
    GetWindowRect(hwnd, &s.resize_rect);
    MoveWindow(global_main_hwnd, s.resize_rect.left, s.resize_rect.top,
        s.resize_rect.right - s.resize_rect.left, s.resize_rect.bottom - s.resize_rect.top, 0);
    return TRUE;

  case WM_DISPLAYCHANGE:

    shell_release_sealed_open_packs_resources(g_open_packs_background, g_open_packs_highlight,
        g_open_packs_button_brush, g_open_packs_light_pen, g_open_packs_dark_pen);
    shell_load_sealed_open_packs_resources(&g_open_packs_background, &g_open_packs_highlight,
        &g_open_packs_title_rect, g_open_packs_pack_rects, &g_open_packs_opened_pack_rect,
        &g_open_packs_cards_rect, &g_open_packs_title_color, &g_open_packs_text_color,
        &g_open_packs_help_color, &g_open_packs_button_brush, &g_open_packs_light_pen, &g_open_packs_dark_pen,
        &g_open_packs_button_color);
    return FALSE;

  default:
    return FALSE;
  }
}

// FUNCTION: MAGIC 0x004c742e
int shell_draw_sealed_open_packs_background(HDC dc, RECT *bounds, HDC background_dc, HBITMAP background,
    int width, int height)
{
  RECT rect;

  CopyRect(&rect, bounds);
  if (background != NULL)
  StretchBlt(dc, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, background_dc, 0, 0,
      width, height, SRCCOPY);
  else
  FillRect(dc, &rect, GetStockObject(GRAY_BRUSH));
  return 0;
}

// FUNCTION: MAGIC 0x004c78ae
void shell_load_sealed_open_packs_resources(HBITMAP *background, HBITMAP *highlight, RECT *title, RECT *packs,
    RECT *opened_pack, RECT *cards, COLORREF *title_color, COLORREF *text_color, COLORREF *help_color,
    HBRUSH *button_brush, HPEN *light_pen, HPEN *dark_pen, COLORREF *button_color)
{
  char path[264];

  if (g_display_color_depth == 8)

  {
    sprintf(path, "%s\\WINBK_FoilPackScreen.pic", g_exp1_art_path);
    *background = load_pic(path);
  }
  else

  {
    sprintf(path, "%s\\WINBK_FoilPackScreen16.bmp", g_exp1_art_path);
    *background = shell_load_bitmap_file(path, NULL, 0);
  }
  sprintf(path, "%s\\WINBK_FoilPackHighlights.pic", g_exp1_art_path);
  *highlight = load_pic(path);
  *title_color = 0x1000023;
  *text_color = 0x1000090;
  *help_color = 0x100009e;
  *button_brush = CreateSolidBrush(0x10000cf);
  *light_pen = CreatePen(0, 0, 0x1000090);
  *dark_pen = CreatePen(0, 0, 0x1000005);
  *button_color = 0x100009e;
  SetRect(title, 0x15, 0x11, 0x1d1, 0x9e);
  SetRect(packs, 0x14, 0xd7, 0x6d, 0x173);
  SetRect(packs + 1, 0x80, 0xd7, 0xd9, 0x173);
  SetRect(packs + 2, 0xec, 0xd7, 0x145, 0x173);
  SetRect(packs + 3, 0x14, 0x19c, 0x6d, 0x238);
  SetRect(packs + 4, 0x80, 0x19c, 0xd9, 0x238);
  SetRect(packs + 5, 0xec, 0x19c, 0x145, 0x238);
  SetRect(opened_pack, 0x159, 0xca, 0x220, 0x243);
  SetRect(cards, 0x237, 2, 0x316, 0x24d);
  if (*button_brush == NULL)
  *button_brush = GetStockObject(GRAY_BRUSH);
  if (*light_pen == NULL)
  *light_pen = GetStockObject(WHITE_PEN);
  if (*dark_pen == NULL)
  *dark_pen = GetStockObject(BLACK_PEN);
}

// FUNCTION: MAGIC 0x004c7b0e
void shell_release_sealed_open_packs_resources(HBITMAP background, HBITMAP highlight, HBRUSH button_brush,
    HPEN light_pen, HPEN dark_pen)
{
  if (background != NULL)
  delete_and_close_object(background);
  if (highlight != NULL)
  delete_and_close_object(highlight);
  if (button_brush != NULL)
  DeleteObject(button_brush);
  if (light_pen != NULL)
  DeleteObject(light_pen);
  if (dark_pen != NULL)
  DeleteObject(dark_pen);
}

// FUNCTION: MAGIC 0x004c7b81
int shell_sealed_open_card_rect(RECT *result, int stack_index, int revealed_count, RECT *bounds)
{
  struct
  {
    int spacing[15];
    int width;
    int index;
    int top;
    int available_height;
    int visible;
    int card_height;
    RECT rect;
  } s;

  if (result == NULL)
  return 0;
  if (stack_index < 0 || revealed_count <= 0 || bounds == NULL)
  return 0;
  s.width = bounds->right - bounds->left;
  s.card_height = ((bounds->bottom - bounds->top) * 65) / 100;
  if (stack_index == 0)

  {
    SetRect(&s.rect, bounds->left, bounds->bottom - s.card_height, bounds->right, bounds->bottom);
    s.visible = 1;
  }
  else if (stack_index > 8)

  {
    s.available_height = (bounds->bottom - bounds->top) - s.card_height;
    SetRect(&s.rect, bounds->left, bounds->top, bounds->right, (s.available_height * 10) / 100 + bounds->top);
    s.visible = 0;
  }
  else

  {
    s.available_height = (bounds->bottom - bounds->top) - s.card_height;
    if (revealed_count <= 2)
    s.spacing[0] = 48;
    else if (revealed_count == 3)

    {
      s.spacing[0] = 48;
      s.spacing[1] = 40;
    }
    else if (revealed_count == 4)

    {
      s.spacing[0] = 42;
      s.spacing[1] = 32;
      s.spacing[2] = 21;
    }
    else if (revealed_count == 5)

    {
      s.spacing[0] = 37;
      s.spacing[1] = 27;
      s.spacing[2] = 20;
      s.spacing[3] = 15;
    }
    else if (revealed_count == 6)

    {
      s.spacing[0] = 30;
      s.spacing[1] = 21;
      s.spacing[2] = 18;
      s.spacing[3] = 15;
      s.spacing[4] = 15;
    }
    else if (revealed_count == 7)

    {
      s.spacing[0] = 25;
      s.spacing[1] = 19;
      s.spacing[2] = 16;
      s.spacing[3] = 15;
      s.spacing[4] = 14;
      s.spacing[5] = 12;
    }
    else if (revealed_count == 8)

    {
      s.spacing[0] = 21;
      s.spacing[1] = 18;
      s.spacing[2] = 14;
      s.spacing[3] = 14;
      s.spacing[4] = 14;
      s.spacing[5] = 12;
      s.spacing[6] = 7;
    }
    else if (revealed_count >= 9)

    {
      s.spacing[0] = 19;
      s.spacing[1] = 17;
      s.spacing[2] = 14;
      s.spacing[3] = 14;
      s.spacing[4] = 14;
      s.spacing[5] = 12;
      s.spacing[6] = 6;
      s.spacing[7] = 6;
    }
    s.top = bounds->bottom - s.card_height;
    for (s.index = 0; stack_index > s.index; ++s.index)
    s.top -= (s.spacing[s.index] * s.available_height) / 100;
    SetRect(&s.rect, bounds->left, s.top, bounds->right, s.top + s.card_height);
    s.visible = 1;
  }
  CopyRect(result, &s.rect);
  return s.visible;
}

// FUNCTION: MAGIC 0x004c7e96
void shell_reveal_sealed_pack_cards(HWND hwnd, sealed_deck_pack_t *pack, int direction, int card_count,
    HBITMAP background, RECT *pack_rect, RECT *cards_rect)
{
  int revealed_card;

  if (pack == NULL)
  return;
  if (pack->card_count == 0)
  return;
  if (card_count < 0 && direction == 0)

  {
    card_count = -card_count;
    direction = 1;
  }
  if (card_count < 0 && direction == 1)

  {
    card_count = -card_count;
    direction = 0;
  }
  revealed_card = pack->revealed_card_index;
  if (direction == 0)
  revealed_card += card_count;
  else if (direction == 1)
  revealed_card -= card_count;
  else if (direction == 2)
  revealed_card = 1000;
  else if (direction == 3)
  revealed_card = -1000;
  if (revealed_card < -1)
  revealed_card = -1;
  if (pack->card_count - 1 < revealed_card)
  revealed_card = pack->card_count - 1;
  if (direction == 0 || direction == 2)
  play_sound_effect(0x43);
  else
  play_sound_effect(0x44);
  if (pack->revealed_card_index != revealed_card)

  {
    pack->revealed_card_index = revealed_card;
    shell_invalidate_logical_rect(hwnd, background, pack_rect);
    shell_invalidate_logical_rect(hwnd, background, cards_rect);
  }
}
