#include "magic_shell_dialogs.h"
#include "cardartlib/src/palette.h"
#include "game_support.h"
#include "global_state.h"

typedef struct sealed_deck_cached_card_t
{
  char reserved_0000[0x2fc];
  HGDIOBJ art;
  char reserved_0300[0x12a8 - 0x300];
} sealed_deck_cached_card_t;

typedef struct sealed_deck_card_cache_t
{
  sealed_deck_cached_card_t cards[64];
  int count;
} sealed_deck_card_cache_t;

typedef struct sealed_deck_pack_definition_t
{
  char name[0x34];
  int starter_count;
  int booster_count;
  int free_lands;
  int rounds;
  int color_count;
} sealed_deck_pack_definition_t;

extern sealed_deck_pack_definition_t g_sealed_pack_definitions[9];

// GLOBAL: MAGIC 0x0074b628
static HGDIOBJ g_sealed_deck_art_1;
// GLOBAL: MAGIC 0x0074b624
static HGDIOBJ g_sealed_deck_art_2;
// GLOBAL: MAGIC 0x0074b5d8
static HGDIOBJ g_sealed_deck_art_3;
// GLOBAL: MAGIC 0x0074b620
static HGDIOBJ g_sealed_deck_art_4;
// GLOBAL: MAGIC 0x0074b5e8
static HGDIOBJ g_sealed_deck_art_5;
// GLOBAL: MAGIC 0x0074b5dc
static HGDIOBJ g_sealed_deck_art_6;

// FUNCTION: MAGIC 0x004c21a3
static int shell_release_sealed_deck_art_resources(void)
{
  int card_index;

  if (g_sealed_deck_art_1 != NULL)
  {
    delete_and_close_object(g_sealed_deck_art_1);
    g_sealed_deck_art_1 = NULL;
  }
  if (g_sealed_deck_art_2 != NULL)
  {
    delete_and_close_object(g_sealed_deck_art_2);
    g_sealed_deck_art_2 = NULL;
  }
  if (g_sealed_deck_art_3 != NULL)
  {
    delete_and_close_object(g_sealed_deck_art_3);
    g_sealed_deck_art_3 = NULL;
  }
  if (g_sealed_deck_art_4 != NULL)
  {
    delete_and_close_object(g_sealed_deck_art_4);
    g_sealed_deck_art_4 = NULL;
  }
  if (g_sealed_deck_art_5 != NULL)
  {
    delete_and_close_object(g_sealed_deck_art_5);
    g_sealed_deck_art_5 = NULL;
  }
  if (g_sealed_deck_art_6 != NULL)
  {
    delete_and_close_object(g_sealed_deck_art_6);
    g_sealed_deck_art_6 = NULL;
  }
  if (g_savegame_data_pointer != NULL)
  {
    for (card_index = 0;
         card_index < ((sealed_deck_card_cache_t *)g_savegame_data_pointer)->count;
         ++card_index)
    {
      if (((sealed_deck_card_cache_t *)g_savegame_data_pointer)->cards[card_index].art != NULL)
      {
        delete_and_close_object(
            ((sealed_deck_card_cache_t *)g_savegame_data_pointer)->cards[card_index].art);
        ((sealed_deck_card_cache_t *)g_savegame_data_pointer)->cards[card_index].art = NULL;
      }
    }
  }
  return 1;
}

// FUNCTION: MAGIC 0x004c36e5
int shell_sealed_deck_card_count(void)
{
  struct
  {
    int card_count;
    int pack_index;
    int pack_type;
  } s;

  s.card_count = 0;
  for (s.pack_index = 0; s.pack_index < g_sealed_deck_options.starter_count;
       ++s.pack_index)
    s.card_count += 60;
  for (s.pack_index = 0; s.pack_index < g_sealed_deck_options.booster_count;
       ++s.pack_index)
  {
    s.pack_type = g_sealed_deck_options.booster_packs[s.pack_index];
    if (g_sealed_pack_definitions[s.pack_type].free_lands == 0)
      s.card_count += 15;
    else if (g_sealed_pack_definitions[s.pack_type].free_lands == 1)
      s.card_count += 12;
    else if (g_sealed_pack_definitions[s.pack_type].free_lands == 2)
      s.card_count += 8;
  }
  return s.card_count;
}

// FUNCTION: MAGIC 0x0046032a
BOOL CALLBACK shell_sealed_deck_dialog_proc(HWND hwnd, UINT message,
                                           WPARAM wparam, LPARAM lparam)
{
  struct
  {
    HGDIOBJ label_font;
    HGDIOBJ dialog_font;
    HDC erase_dc;
    RECT erase_rect;
    DRAWITEMSTRUCT *draw_item;
    COLORREF draw_color;
    HDC control_dc;
    HWND control;
    int control_id;
    HWND old_focus;
    HWND new_focus;
  } s;

  switch (message)
  {
  case WM_DESTROY:
    s.label_font = (HGDIOBJ)SendDlgItemMessageA(hwnd, 0x65f, WM_GETFONT, 0, 0);
    SendDlgItemMessageA(hwnd, 0x65f, WM_SETFONT, 0, 0);
    SendDlgItemMessageA(hwnd, 0x65e, WM_SETFONT, 0, 0);
    SendDlgItemMessageA(hwnd, 0x665, WM_SETFONT, 0, 0);
    SendDlgItemMessageA(hwnd, 0x66a, WM_SETFONT, 0, 0);
    SendDlgItemMessageA(hwnd, 0x70b, WM_SETFONT, 0, 0);
    DeleteObject(s.label_font);
    s.dialog_font = (HGDIOBJ)SendDlgItemMessageA(hwnd, 0x660, WM_GETFONT, 0, 0);
    SendDlgItemMessageA(hwnd, 0x660, WM_SETFONT, 0, 0);
    SendDlgItemMessageA(hwnd, 0x661, WM_SETFONT, 0, 0);
    SendDlgItemMessageA(hwnd, 0x663, WM_SETFONT, 0, 0);
    SendDlgItemMessageA(hwnd, 0x670, WM_SETFONT, 0, 0);
    SendDlgItemMessageA(hwnd, 0x673, WM_SETFONT, 0, 0);
    SendDlgItemMessageA(hwnd, 0x674, WM_SETFONT, 0, 0);
    SendDlgItemMessageA(hwnd, 0x66c, WM_SETFONT, 0, 0);
    SendDlgItemMessageA(hwnd, 0x66d, WM_SETFONT, 0, 0);
    SendDlgItemMessageA(hwnd, 0x666, WM_SETFONT, 0, 0);
    SendDlgItemMessageA(hwnd, 0x671, WM_SETFONT, 0, 0);
    SendDlgItemMessageA(hwnd, 0x672, WM_SETFONT, 0, 0);
    SendDlgItemMessageA(hwnd, 0x664, WM_SETFONT, 0, 0);
    SendDlgItemMessageA(hwnd, 0x66f, WM_SETFONT, 0, 0);
    SendDlgItemMessageA(hwnd, IDOK, WM_SETFONT, 0, 0);
    DeleteObject(s.dialog_font);
    shell_release_sealed_deck_art_resources();
    return FALSE;

  case WM_ERASEBKGND:
    s.erase_dc = (HDC)wparam;
    ApplyCardArtPaletteToDc(s.erase_dc);
    GetClientRect(hwnd, &s.erase_rect);
    MapWindowPoints(hwnd, GetParent(hwnd), (LPPOINT)&s.erase_rect, 2);
    SendMessageA(GetParent(hwnd), 0x498, (WPARAM)s.erase_dc,
                 (LPARAM)&s.erase_rect);
    return TRUE;

  case WM_DRAWITEM:
    s.draw_item = (DRAWITEMSTRUCT *)lparam;
    if (GetFocus() == s.draw_item->hwndItem)
      s.draw_color = shell_draw_resources.selected_button_color;
    else if ((s.draw_item->itemState & ODS_SELECTED) == 0)
      s.draw_color = shell_draw_resources.regular_button_color;
    else
      s.draw_color = 0x10000c6;
    if (shell_draw_resources.button_face == NULL)
      draw_owner_draw_button_centered(
          s.draw_item, GetStockObject(LTGRAY_BRUSH),
          shell_draw_resources.light_pen, shell_draw_resources.dark_pen,
          s.draw_color, 0);
    else
      shell_draw_bitmap_button(
          s.draw_item, shell_draw_resources.button_face,
          shell_draw_resources.light_pen, shell_draw_resources.dark_pen,
          s.draw_color, 0);
    return TRUE;

  case WM_CTLCOLOREDIT:
  case WM_CTLCOLORBTN:
  case WM_CTLCOLORSTATIC:
    s.control_dc = (HDC)wparam;
    ApplyCardArtPaletteToDc(s.control_dc);
    s.control = (HWND)lparam;
    s.control_id = GetDlgCtrlID(s.control);
    if (s.control_id == 0x66a || s.control_id == 0x665 ||
        s.control_id == 0x70b)
    {
      SetTextColor(s.control_dc, shell_draw_resources.button_text_color);
      SetBkMode(s.control_dc, TRANSPARENT);
      return (BOOL)shell_draw_resources.light_brush;
    }
    if (s.control_id != 0x661 && s.control_id != 0x66d &&
        s.control_id != 0x672)
    {
      if (GetFocus() == s.control || s.control_id == 0x65f)
        SetTextColor(s.control_dc,
                     shell_draw_resources.selected_button_color);
      else
        SetTextColor(s.control_dc,
                     shell_draw_resources.button_text_color);
      SetBkMode(s.control_dc, TRANSPARENT);
      return (BOOL)GetStockObject(NULL_BRUSH);
    }
    SetTextColor(s.control_dc, shell_draw_resources.background_color);
    SetBkMode(s.control_dc, OPAQUE);
    SetBkColor(s.control_dc, shell_draw_resources.panel_color);
    return (BOOL)shell_draw_resources.panel_brush;

  case WM_NOTIFY:
    if (wparam == 0x66e)
      PostMessageA(hwnd, WM_COMMAND, 0x200066d,
                   (LPARAM)GetDlgItem(hwnd, 0x66d));
    else if (wparam == 0x662)
      PostMessageA(hwnd, WM_COMMAND, 0x2000661,
                   (LPARAM)GetDlgItem(hwnd, 0x661));
    else if (wparam == 0x675)
      PostMessageA(hwnd, WM_COMMAND, 0x2000672,
                   (LPARAM)GetDlgItem(hwnd, 0x672));
    return FALSE;

  case 0x4c8:
    s.old_focus = (HWND)wparam;
    s.new_focus = (HWND)lparam;
    if (GetDlgItem(hwnd, IDOK) == s.old_focus ||
        GetDlgItem(hwnd, 0x66f) == s.old_focus ||
        GetDlgItem(hwnd, 0x664) == s.old_focus)
      SendMessageA(hwnd, 0x401, GetDlgCtrlID(s.old_focus), 0);
    else
      SendMessageA(hwnd, 0x401, 1, 0);
    if (s.old_focus != NULL)
      InvalidateRect(s.old_focus, NULL, TRUE);
    if (s.new_focus != NULL)
      InvalidateRect(s.new_focus, NULL, TRUE);
    return FALSE;

  case 0x30f:
  case 0x310:
  case 0x311:
    return handle_button_palette_message((int)hwnd, message,
                                         (int)wparam, (int)lparam);

  default:
    /* TODO: decompile the remaining messages for MAGIC 0x0046032a,
     * including WM_INITDIALOG, WM_COMMAND, WM_PAINT, and pack selection. */
    return FALSE;
  }
}
