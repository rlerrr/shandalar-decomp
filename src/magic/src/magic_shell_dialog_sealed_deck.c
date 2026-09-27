#include "magic_shell_dialogs.h"
#include <commdlg.h>
#include <stdio.h>
#include <string.h>
#include "cardartlib/src/palette.h"
#include "deckdll/src/shared_resources.h"
#include "game_support.h"
#include "global_duel_ui_ids.h"
#include "global_other.h"
#include "global_state.h"
#include "global_strings.h"
#include "magic_shell.h"
#include "magic_sealed_tournament.h"
#include "magic_sealed_build.h"
#include "shared_startup.h"

extern HWND global_main_hwnd;
extern char global_base_directory[];
extern char g_exp1_art_path[];
extern char *s_sealed_deck_registry_path_005710e0;
void __stdcall LoadSealedDeckRegistryOptions(void);
unsigned int HasExpansion(unsigned char expansion_mask);
void shell_open_duel_interface_options(HWND hwnd);
BOOL DrawMaskedBitmapToRect(HDC dc, RECT *rect, HANDLE bitmap,
                            int width, int height, int src_x, int src_y,
                            int mask_x, int mask_y);

// GLOBAL: MAGIC 0x00637a98
static int g_sealed_deck_selected_starters[2];
// GLOBAL: MAGIC 0x00637ab0
static int g_sealed_deck_selected_boosters[4];
// GLOBAL: MAGIC 0x00637aa8
static HFONT g_sealed_deck_low_resolution_font;
// GLOBAL: MAGIC 0x00637b0c
static HFONT g_sealed_deck_dialog_font;
// GLOBAL: MAGIC 0x00637ac8
static char g_sealed_deck_card_count_format[68];
// GLOBAL: MAGIC 0x00637b10
static char g_sealed_deck_rounds_format[100];

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
         card_index < ((sealed_deck_tournament_t *)g_savegame_data_pointer)->player_count;
         ++card_index)
    {
      if (((sealed_deck_tournament_t *)g_savegame_data_pointer)->players[card_index].art != NULL)
      {
        delete_and_close_object(
            ((sealed_deck_tournament_t *)g_savegame_data_pointer)->players[card_index].art);
        ((sealed_deck_tournament_t *)g_savegame_data_pointer)->players[card_index].art = NULL;
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

// FUNCTION: MAGIC 0x004c233c
int shell_sealed_starter_pack_available(int pack_type)
{
  return HasExpansion((unsigned char)g_sealed_pack_definitions[pack_type].starter_count);
}

// FUNCTION: MAGIC 0x004c2362
int shell_sealed_booster_pack_available(int pack_type)
{
  return g_sealed_pack_definitions[pack_type].booster_count;
}

// FUNCTION: MAGIC 0x00493e1e
void shell_fit_sealed_pack_rect(RECT *result, RECT *bounds,
                                int pack_width, int pack_height)
{
  struct
  {
    int candidate_width;
    int candidate_height;
    int fitted_height;
    int fitted_width;
  } s;

  if (result == NULL || bounds == NULL)
    return;
  s.candidate_width = (bounds->bottom - bounds->top) * pack_width /
                      pack_height;
  s.candidate_height = (bounds->right - bounds->left) * pack_height /
                       pack_width;
  if (s.candidate_width < bounds->right - bounds->left)
  {
    s.fitted_width = s.candidate_width;
    s.fitted_height = bounds->bottom - bounds->top;
  }
  else
  {
    s.fitted_height = s.candidate_height;
    s.fitted_width = bounds->right - bounds->left;
  }
  result->left = bounds->left + (bounds->right - bounds->left) / 2 -
                 s.fitted_width / 2;
  result->right = result->left + s.fitted_width;
  result->top = bounds->top + (bounds->bottom - bounds->top) / 2 -
                s.fitted_height / 2;
  result->bottom = result->top + s.fitted_height;
}

// FUNCTION: MAGIC 0x004c237f
int shell_draw_sealed_pack_label(HDC dc, RECT *rect, int pack_type)
{
  if (dc == NULL || rect == NULL)
    return 0;
  if (pack_type < 0 || pack_type > 8)
    return 0;
  ExtTextOutA(dc, rect->left, rect->top, ETO_OPAQUE | ETO_CLIPPED,
              rect, g_sealed_pack_definitions[pack_type].name,
              strlen(g_sealed_pack_definitions[pack_type].name), NULL);
  return 1;
}

// FUNCTION: MAGIC 0x004c2d78
int shell_draw_sealed_starter_pack(HDC dc, RECT *rect, int pack_type,
                                  int variation, int selected)
{
  struct
  {
    BITMAP bitmap;
    int tile_height;
    int tile_width;
    int source_y;
    int source_x;
    int mask_y;
    RECT destination;
    int mask_x;
    char path[264];
    int saved_dc;
  } s;

  if (dc == NULL || rect == NULL)
    return 0;
  if (pack_type > 8)
    return 0;
  if (pack_type != -1)
  {
    if (g_sealed_pack_definitions[pack_type].booster_count == 0)
      return 0;
    if (variation <= 0 ||
        g_sealed_pack_definitions[pack_type].rounds - 1 < variation)
      variation = g_sealed_pack_definitions[pack_type].rounds - 1;
  }
  if (g_sealed_deck_art_4 == NULL)
  {
    if (g_display_color_depth == 8)
    {
      sprintf(s.path, "%s\\PACKSTARTERSsmall.pic", g_exp1_art_path);
      g_sealed_deck_art_4 = load_pic(s.path);
    }
    else
    {
      sprintf(s.path, "%s\\PACKSTARTERSsmall16.bmp", g_exp1_art_path);
      g_sealed_deck_art_4 = shell_load_bitmap_file(s.path, NULL, 0);
    }
  }
  if (g_sealed_deck_art_4 != NULL)
  {
    GetObjectA(g_sealed_deck_art_4, sizeof(s.bitmap), &s.bitmap);
    s.tile_width = s.bitmap.bmWidth / 4;
    s.tile_height = s.bitmap.bmHeight / 2;
    shell_fit_sealed_pack_rect(&s.destination, rect,
                               s.tile_width, s.tile_height);
    if (pack_type == -1)
    {
      s.source_x = s.bitmap.bmWidth - s.tile_width;
      s.source_y = s.tile_height;
    }
    else
    {
      if (pack_type == 2)
        s.source_x = 0;
      else if (pack_type == 1)
        s.source_x = s.tile_width;
      else if (pack_type == 0)
        s.source_x = s.tile_width * 2;
      else
        s.source_x = 0;
      if (selected != 0)
        s.source_y = s.tile_height;
      else
        s.source_y = 0;
    }
    s.mask_x = s.bitmap.bmWidth - s.tile_width;
    s.mask_y = 0;
    DrawMaskedBitmapToRect(dc, &s.destination, g_sealed_deck_art_4,
                           s.tile_width, s.tile_height, s.source_x,
                           s.source_y, s.mask_x, s.mask_y);
  }
  else
  {
    s.saved_dc = SaveDC(dc);
    SelectObject(dc, GetStockObject(DKGRAY_BRUSH));
    SetTextColor(dc, 0x10101);
    SetBkMode(dc, TRANSPARENT);
    SelectObject(dc, GetStockObject(SYSTEM_FONT));
    Rectangle(dc, rect->left, rect->top, rect->right, rect->bottom);
    TextOutA(dc, rect->left, rect->top,
             g_sealed_pack_definitions[pack_type].name, 3);
    if (selected != 0)
      TextOutA(dc, rect->left, rect->top + 20, "X", 1);
    RestoreDC(dc, s.saved_dc);
  }
  return 1;
}

// FUNCTION: MAGIC 0x004c2410
int shell_draw_sealed_booster_pack(HDC dc, RECT *rect, int pack_type,
                                  int variation, int selected)
{
  struct
  {
    BITMAP bitmap;
    int tile_height;
    int tile_width;
    int source_y;
    int source_x;
    int mask_y;
    RECT destination;
    int mask_x;
    char path[264];
    int saved_dc;
  } s;

  if (dc == NULL || rect == NULL)
    return 0;
  if (pack_type > 8)
    return 0;
  if (pack_type != -1 &&
      (variation <= 0 ||
       g_sealed_pack_definitions[pack_type].color_count - 1 < variation))
    variation = g_sealed_pack_definitions[pack_type].color_count - 1;
  if (g_sealed_deck_art_1 == NULL)
  {
    if (g_display_color_depth == 8)
    {
      sprintf(s.path, "%s\\PACKBOOSTERSsmall.pic", g_exp1_art_path);
      g_sealed_deck_art_1 = load_pic(s.path);
    }
    else
    {
      sprintf(s.path, "%s\\PACKBOOSTERSsmall16.bmp", g_exp1_art_path);
      g_sealed_deck_art_1 = shell_load_bitmap_file(s.path, NULL, 0);
    }
  }
  if (g_sealed_deck_art_1 != NULL)
  {
    GetObjectA(g_sealed_deck_art_1, sizeof(s.bitmap), &s.bitmap);
    s.tile_width = s.bitmap.bmWidth / 10;
    s.tile_height = s.bitmap.bmHeight / 2;
    shell_fit_sealed_pack_rect(&s.destination, rect,
                               s.tile_width, s.tile_height);
    if (pack_type == -1)
    {
      s.source_x = s.bitmap.bmWidth - s.tile_width;
      s.source_y = s.tile_height;
    }
    else
    {
      if (pack_type == 2)
        s.source_x = s.tile_width * variation;
      else if (pack_type == 4)
        s.source_x = s.tile_width * 5;
      else if (pack_type == 3)
        s.source_x = s.tile_width * 6;
      else if (pack_type == 0)
        s.source_x = s.tile_width * 7;
      else if (pack_type == 1)
        s.source_x = s.tile_width * 8;
      else
        s.source_x = 0;
      if (selected != 0)
        s.source_y = s.tile_height;
      else
        s.source_y = 0;
    }
    s.mask_x = s.bitmap.bmWidth - s.tile_width;
    s.mask_y = 0;
    DrawMaskedBitmapToRect(dc, &s.destination, g_sealed_deck_art_1,
                           s.tile_width, s.tile_height, s.source_x,
                           s.source_y, s.mask_x, s.mask_y);
  }
  else
  {
    s.saved_dc = SaveDC(dc);
    SelectObject(dc, GetStockObject(GRAY_BRUSH));
    SetTextColor(dc, 0x10101);
    SetBkMode(dc, TRANSPARENT);
    SelectObject(dc, GetStockObject(SYSTEM_FONT));
    Rectangle(dc, rect->left, rect->top, rect->right, rect->bottom);
    TextOutA(dc, rect->left, rect->top,
             g_sealed_pack_definitions[pack_type].name, 3);
    if (selected != 0)
      TextOutA(dc, rect->left, rect->top + 20, "X", 1);
    RestoreDC(dc, s.saved_dc);
  }
  return 1;
}

// FUNCTION: MAGIC 0x0048bfc8
static void shell_save_sealed_deck_registry_options(void)
{
  struct
  {
    HKEY options_key;
    char value_buffer[52];
    DWORD disposition;
    int pack_index;
  } s;

  if (RegCreateKeyExA(HKEY_CURRENT_USER,
                      s_sealed_deck_registry_path_005710e0, 0, NULL, 0,
                      KEY_ALL_ACCESS, NULL, &s.options_key,
                      &s.disposition) == ERROR_SUCCESS)
  {
    wsprintfA(s.value_buffer, "%d", g_sealed_deck_options.rounds);
    RegSetValueExA(s.options_key, "Rounds", 0, REG_SZ,
                   (BYTE *)s.value_buffer, strlen(s.value_buffer) + 1);
    wsprintfA(s.value_buffer, "%d", g_sealed_deck_options.starter_count);
    for (s.pack_index = 0;
         s.pack_index < g_sealed_deck_options.starter_count;
         ++s.pack_index)
      wsprintfA(s.value_buffer + strlen(s.value_buffer), " %d",
                g_sealed_deck_options.starter_packs[s.pack_index]);
    RegSetValueExA(s.options_key, "Starters", 0, REG_SZ,
                   (BYTE *)s.value_buffer, strlen(s.value_buffer) + 1);
    wsprintfA(s.value_buffer, "%d", g_sealed_deck_options.booster_count);
    for (s.pack_index = 0;
         s.pack_index < g_sealed_deck_options.booster_count;
         ++s.pack_index)
      wsprintfA(s.value_buffer + strlen(s.value_buffer), " %d",
                g_sealed_deck_options.booster_packs[s.pack_index]);
    RegSetValueExA(s.options_key, "Boosters", 0, REG_SZ,
                   (BYTE *)s.value_buffer, strlen(s.value_buffer) + 1);
    wsprintfA(s.value_buffer, "%d", g_sealed_deck_options.free_lands);
    RegSetValueExA(s.options_key, "FreeLands", 0, REG_SZ,
                   (BYTE *)s.value_buffer, strlen(s.value_buffer) + 1);
    wsprintfA(s.value_buffer, "%d", g_sealed_deck_options.best_of);
    RegSetValueExA(s.options_key, "BestOf", 0, REG_SZ,
                   (BYTE *)s.value_buffer, strlen(s.value_buffer) + 1);
    wsprintfA(s.value_buffer, "%d", g_sealed_deck_options.ante);
    RegSetValueExA(s.options_key, "Ante", 0, REG_SZ,
                   (BYTE *)s.value_buffer, strlen(s.value_buffer) + 1);
    wsprintfA(s.value_buffer, "%d",
              g_sealed_deck_options.minimum_deck_size);
    RegSetValueExA(s.options_key, "MinDeckSize", 0, REG_SZ,
                   (BYTE *)s.value_buffer, strlen(s.value_buffer) + 1);
    RegFlushKey(s.options_key);
    RegCloseKey(s.options_key);
  }
}

// FUNCTION: MAGIC 0x004cce74
static int shell_write_sealed_deck_build_report(void)
{
  /* TODO: build the sealed deck report and launch the text viewer. */
  return 0;
}

// FUNCTION: MAGIC 0x00462bde
static void shell_layout_sealed_deck_dialog(HWND hwnd)
{
  struct
  {
    char control_text[100];
    RECT ante_region;
    HWND control;
    RECT button_region;
    HDC dc;
    int row_height;
    int button_width;
    RECT options_region;
    int y;
    int x;
    int next_x;
    HGDIOBJ font;
    RECT rounds_region;
    RECT client_rect;
    SIZE text_size;
    int button_height;
    HCURSOR old_cursor;
    int text_height;
    int margin;
    int radio_width;
  } s;

  s.old_cursor = GetCursor();
  SetCursor(LoadCursorA(NULL, IDC_WAIT));
  GetClientRect(hwnd, &s.client_rect);
  s.client_rect.left += 20;
  s.client_rect.right -= 20;
  s.client_rect.top += 20;
  s.client_rect.bottom -= 20;

  s.font = (HGDIOBJ)SendDlgItemMessageA(hwnd, 0x65e, WM_GETFONT, 0, 0);
  s.dc = GetDC(hwnd);
  SelectObject(s.dc, s.font);
  GetTextExtentPoint32A(s.dc, "fun", 3, &s.text_size);
  s.button_height = s.text_size.cy * 2;
  s.text_height = s.text_size.cy;
  s.margin = s.text_size.cy * 3 / 2;
  s.control = GetDlgItem(hwnd, 0x65e);
  GetWindowTextA(s.control, s.control_text, 100);
  GetTextExtentPoint32A(s.dc, s.control_text, strlen(s.control_text),
                        &s.text_size);
  SetWindowPos(s.control, NULL, 0, 0, s.text_size.cx + s.margin,
               s.text_height, SWP_NOZORDER);
  s.x = s.text_size.cx + s.margin / 2;
  s.y = 0;
  s.control = GetDlgItem(hwnd, 0x65f);
  GetWindowTextA(s.control, s.control_text, 100);
  GetTextExtentPoint32A(s.dc, s.control_text, strlen(s.control_text),
                        &s.text_size);
  SetWindowPos(s.control, NULL, s.x, s.y, s.text_size.cx,
               s.text_height, SWP_NOZORDER);

  s.font = (HGDIOBJ)SendDlgItemMessageA(hwnd, IDOK, WM_GETFONT, 0, 0);
  s.dc = GetDC(hwnd);
  SelectObject(s.dc, s.font);
  GetTextExtentPoint32A(s.dc, "fun", 3, &s.text_size);
  s.button_height = s.text_size.cy * 2;
  s.text_height = s.text_size.cy;
  s.margin = s.text_size.cy * 3 / 2;
  s.radio_width = s.text_size.cx;
  s.control = GetDlgItem(hwnd, IDOK);
  GetWindowTextA(s.control, s.control_text, 100);
  GetTextExtentPoint32A(s.dc, s.control_text, strlen(s.control_text),
                        &s.text_size);
  s.button_width = s.text_size.cx;
  s.control = GetDlgItem(hwnd, 0x66f);
  GetWindowTextA(s.control, s.control_text, 100);
  GetTextExtentPoint32A(s.dc, s.control_text, strlen(s.control_text),
                        &s.text_size);
  if (s.button_width < s.text_size.cx)
    s.button_width = s.text_size.cx;
  s.button_width += s.margin;
  s.x = s.client_rect.right - s.button_width;
  s.y = s.client_rect.bottom - s.button_height;
  s.control = GetDlgItem(hwnd, IDOK);
  SetWindowPos(s.control, NULL, s.x, s.y, s.button_width,
               s.button_height, SWP_NOZORDER);
  s.y -= s.button_height + s.button_height / 3;
  s.control = GetDlgItem(hwnd, 0x66f);
  SetWindowPos(s.control, NULL, s.x, s.y, s.button_width,
               s.button_height, SWP_NOZORDER);
  s.y -= s.button_height + s.button_height * 3 / 4;
  s.control = GetDlgItem(hwnd, 0x664);
  GetWindowTextA(s.control, s.control_text, 100);
  GetTextExtentPoint32A(s.dc, s.control_text, strlen(s.control_text),
                        &s.text_size);
  s.button_width = s.text_size.cx + s.margin;
  SetWindowPos(s.control, NULL,
               s.x + (s.client_rect.right - s.x) / 2 - s.button_width / 2,
               s.y, s.button_width, s.button_height, SWP_NOZORDER);
  s.next_x = s.x + (s.client_rect.right - s.x) / 2 -
             s.button_width / 2;
  if (s.x <= s.next_x)
    s.next_x = s.x;
  SetRect(&s.button_region, s.next_x, s.y, s.client_rect.right,
          s.client_rect.bottom);
  SetRect(&s.options_region, 0, s.button_height, s.client_rect.right,
          s.button_region.top - s.button_height -
              s.text_height * 75 / 100);
  s.button_width = s.options_region.right - s.options_region.left;
  SetRect(&s.rounds_region, s.options_region.left, s.options_region.top,
          s.options_region.left + s.button_width * 2 / 3 - s.margin,
          s.options_region.bottom);
  SetRect(&s.ante_region, s.options_region.left + s.button_width * 2 / 3,
          s.options_region.top, s.options_region.right,
          s.options_region.top +
              (s.options_region.bottom - s.options_region.top) * 5 / 6);
  s.control = GetDlgItem(hwnd, 0x70b);
  SetWindowPos(s.control, NULL, s.rounds_region.left, s.rounds_region.top,
               s.rounds_region.right - s.rounds_region.left,
               s.rounds_region.bottom - s.rounds_region.top, SWP_NOZORDER);
  s.x = s.rounds_region.left + s.margin;
  s.y = s.rounds_region.top + s.button_height * 75 / 100;
  s.row_height = s.rounds_region.bottom - s.y -
                 s.button_height * 25 / 100;

  s.control = GetDlgItem(hwnd, 0x660);
  GetWindowTextA(s.control, s.control_text, 100);
  GetTextExtentPoint32A(s.dc, s.control_text, strlen(s.control_text),
                        &s.text_size);
  s.button_width = s.text_size.cx + s.margin / 2;
  SetWindowPos(s.control, NULL, s.x, s.y, s.button_width,
               s.text_height, SWP_NOZORDER);
  s.next_x = s.button_width + s.x;
  GetTextExtentPoint32A(s.dc, "0", 2, &s.text_size);
  s.button_width = s.text_size.cx + s.margin;
  s.control = GetDlgItem(hwnd, 0x661);
  SetWindowPos(s.control, NULL, s.next_x, s.y, s.button_width,
               s.button_height, SWP_NOZORDER);
  s.next_x += s.button_width;
  GetTextExtentPoint32A(s.dc, "00", 2, &s.text_size);
  s.button_width = s.text_size.cx;
  s.control = GetDlgItem(hwnd, 0x662);
  SetWindowPos(s.control, NULL, s.next_x, s.y, s.button_width,
               s.button_height, SWP_NOZORDER);
  s.next_x += s.button_width + s.margin / 2;
  s.control = GetDlgItem(hwnd, 0x663);
  GetWindowTextA(s.control, s.control_text, 100);
  GetTextExtentPoint32A(s.dc, s.control_text, strlen(s.control_text),
                        &s.text_size);
  s.button_width = s.text_size.cx + s.margin;
  SetWindowPos(s.control, NULL, s.next_x, s.y, s.button_width,
               s.text_height, SWP_NOZORDER);

  s.y += s.row_height / 3;
  s.control = GetDlgItem(hwnd, 0x66c);
  GetWindowTextA(s.control, s.control_text, 100);
  GetTextExtentPoint32A(s.dc, s.control_text, strlen(s.control_text),
                        &s.text_size);
  s.button_width = s.text_size.cx + s.margin / 2;
  SetWindowPos(s.control, NULL, s.x, s.y, s.button_width,
               s.text_height, SWP_NOZORDER);
  s.next_x = s.button_width + s.x;
  GetTextExtentPoint32A(s.dc, "000", 3, &s.text_size);
  s.button_width = s.text_size.cx + s.margin;
  s.control = GetDlgItem(hwnd, 0x66d);
  SetWindowPos(s.control, NULL, s.next_x, s.y, s.button_width,
               s.button_height, SWP_NOZORDER);
  s.next_x += s.button_width;
  GetTextExtentPoint32A(s.dc, "00", 2, &s.text_size);
  s.button_width = s.text_size.cx;
  s.control = GetDlgItem(hwnd, 0x66e);
  SetWindowPos(s.control, NULL, s.next_x, s.y, s.button_width,
               s.button_height, SWP_NOZORDER);

  s.y += s.row_height / 3;
  s.control = GetDlgItem(hwnd, 0x670);
  GetWindowTextA(s.control, s.control_text, 100);
  GetTextExtentPoint32A(s.dc, s.control_text, strlen(s.control_text),
                        &s.text_size);
  s.button_width = s.text_size.cx + s.margin / 2;
  SetWindowPos(s.control, NULL, s.x, s.y, s.button_width,
               s.text_height, SWP_NOZORDER);
  s.control = GetDlgItem(hwnd, 0x673);
  GetWindowTextA(s.control, s.control_text, 100);
  GetTextExtentPoint32A(s.dc, s.control_text, strlen(s.control_text),
                        &s.text_size);
  SetWindowPos(s.control, NULL, s.button_width + s.x, s.y,
               s.text_size.cx + s.radio_width, s.text_height,
               SWP_NOZORDER);
  s.next_x = s.text_size.cx + s.margin / 2 + s.button_width +
             s.x + s.radio_width;
  s.control = GetDlgItem(hwnd, 0x674);
  GetWindowTextA(s.control, s.control_text, 100);
  GetTextExtentPoint32A(s.dc, s.control_text, strlen(s.control_text),
                        &s.text_size);
  SetWindowPos(s.control, NULL, s.button_width + s.x, s.text_height + s.y,
               s.text_size.cx + s.radio_width, s.text_height,
               SWP_NOZORDER);

  s.control = GetDlgItem(hwnd, 0x665);
  SetWindowPos(s.control, NULL, s.ante_region.left, s.ante_region.top,
               s.ante_region.right - s.ante_region.left,
               s.ante_region.bottom - s.ante_region.top, SWP_NOZORDER);
  s.x = s.ante_region.left + s.margin;
  s.y = s.ante_region.top + s.button_height;
  s.row_height = s.ante_region.bottom - s.y;
  s.control = GetDlgItem(hwnd, 0x666);
  GetWindowTextA(s.control, s.control_text, 100);
  GetTextExtentPoint32A(s.dc, s.control_text, strlen(s.control_text),
                        &s.text_size);
  SetWindowPos(s.control, NULL, s.x, s.y,
               s.text_size.cx + s.radio_width, s.text_height,
               SWP_NOZORDER);
  s.y += s.row_height / 3;
  s.control = GetDlgItem(hwnd, 0x671);
  GetWindowTextA(s.control, s.control_text, 100);
  GetTextExtentPoint32A(s.dc, s.control_text, strlen(s.control_text),
                        &s.text_size);
  s.button_width = s.text_size.cx + s.margin / 2;
  SetWindowPos(s.control, NULL, s.x, s.y, s.button_width,
               s.text_height, SWP_NOZORDER);
  s.next_x = s.button_width + s.x;
  GetTextExtentPoint32A(s.dc, "0", 2, &s.text_size);
  s.button_width = s.text_size.cx + s.margin;
  s.control = GetDlgItem(hwnd, 0x672);
  SetWindowPos(s.control, NULL, s.next_x, s.y, s.button_width,
               s.button_height, SWP_NOZORDER);
  s.next_x += s.button_width;
  GetTextExtentPoint32A(s.dc, "00", 2, &s.text_size);
  s.button_width = s.text_size.cx;
  s.control = GetDlgItem(hwnd, 0x675);
  SetWindowPos(s.control, NULL, s.next_x, s.y, s.button_width,
               s.button_height, SWP_NOZORDER);
  s.control = GetDlgItem(hwnd, 0x66a);
  s.x = 0;
  s.y = s.button_region.top - s.button_height;
  SetWindowPos(s.control, NULL, s.x, s.y,
               s.button_region.left - s.margin - s.x,
               s.client_rect.bottom - s.y, SWP_NOZORDER);
  ReleaseDC(hwnd, s.dc);
  SetCursor(s.old_cursor);
}

// FUNCTION: MAGIC 0x0046032a
BOOL CALLBACK shell_sealed_deck_dialog_proc(HWND hwnd, UINT message,
                                           WPARAM wparam, LPARAM lparam)
{
  struct
  {
    LOGFONTA resize_font;
    HFONT resize_replacement_font;
    int resize_group_index;
    int resize_group_count;
    int resize_group_last[5];
    int resize_control_index;
    int resize_base_height[5];
    HWND resize_controls[50];
    int resize_group_first[5];
    LOGFONTA *resize_font_description;
    int resize_control_count;
    RECT resize_client_rect;
    HFONT resize_old_fonts[5];
    int gap;
    HDC paint_dc;
    COLORREF paint_background;
    PAINTSTRUCT paint;
    int paint_index;
    int label_height;
    HFONT pack_font;
    RECT pack_label_rect;
    LOGFONTA *pack_font_description;
    RECT paint_client_rect;
    HBRUSH hatch_brush;
    RECT pack_region;
    RECT pack_image_rect;
    SIZE text_extent;
    int pack_width;
    HDC erase_dc;
    RECT erase_rect;
    COLORREF draw_color;
    DRAWITEMSTRUCT *draw_item;
    HWND control;
    int control_id;
    HDC control_dc;
    HGDIOBJ control_brush;
    HWND new_focus;
    HWND old_focus;
    char click_text[100];
    int compact_index;
    POINT click_point;
    int booster_start;
    int click_index;
    BOOL click_result;
    RECT clickable_region;
    RECT pack_cell;
    int next_pack;
    int changed_pack;
    int cell_width;
    WPARAM notification_control;
    int best_of_value;
    unsigned int best_of_error;
    int free_lands_value;
    unsigned int free_lands_error;
    int rounds_value;
    char rounds_text[100];
    HWND rounds_label;
    unsigned int rounds_error;
    RECT rounds_rect;
    char autosave_path[264];
    unsigned int save_load_result;
    MSG pending_message;
    int user_cancelled;
    int autosave_queued;
    int resumed_duel_result;
    int new_duel_result;
    int command_id;
    HWND command_control;
    unsigned int notification;
    HGDIOBJ destroy_font;
    char init_text[100];
    int init_pack_index;
    int pack_type;
    int init_booster_index;
    HFONT current_font;
    LOGFONTA *font_description;
    RECT *dialog_rect;
  } s;

  switch (message)
  {
  case WM_INITDIALOG:
    ShowWindow(GetDlgItem(hwnd, 0x703), SW_HIDE);
    g_savegame_data_pointer = NULL;
    shell_prepare_save_game_dialog(3);
    LoadSealedDeckRegistryOptions();
    g_sealed_deck_selected_starters[0] = g_sealed_deck_options.starter_packs[0];
    if (g_sealed_deck_options.starter_count == 1)
      g_sealed_deck_selected_starters[1] = -1;
    else
      g_sealed_deck_selected_starters[1] = g_sealed_deck_options.starter_packs[1];
    for (s.init_booster_index = 0; s.init_booster_index < g_sealed_deck_options.booster_count;
         ++s.init_booster_index)
      g_sealed_deck_selected_boosters[s.init_booster_index] =
          g_sealed_deck_options.booster_packs[s.init_booster_index];
    for (; s.init_booster_index < 4; ++s.init_booster_index)
      g_sealed_deck_selected_boosters[s.init_booster_index] = -1;

    s.font_description = LoadFontFromIni("ShellDialog", 0);
    g_sealed_deck_dialog_font = CreateFontIndirectA(s.font_description);
    s.font_description = LoadFontFromIni("ShellDialog_lores", 0);
    g_sealed_deck_low_resolution_font =
        CreateFontIndirectA(s.font_description);
    s.current_font = g_sealed_deck_dialog_font;
    SendDlgItemMessageA(hwnd, 0x660, WM_SETFONT,
                        (WPARAM)s.current_font, 0);
    SendDlgItemMessageA(hwnd, 0x661, WM_SETFONT, (WPARAM)s.current_font, 0);
    SendDlgItemMessageA(hwnd, 0x663, WM_SETFONT, (WPARAM)s.current_font, 0);
    SendDlgItemMessageA(hwnd, 0x670, WM_SETFONT, (WPARAM)s.current_font, 0);
    SendDlgItemMessageA(hwnd, 0x673, WM_SETFONT, (WPARAM)s.current_font, 0);
    SendDlgItemMessageA(hwnd, 0x674, WM_SETFONT, (WPARAM)s.current_font, 0);
    SendDlgItemMessageA(hwnd, 0x66c, WM_SETFONT, (WPARAM)s.current_font, 0);
    SendDlgItemMessageA(hwnd, 0x66d, WM_SETFONT, (WPARAM)s.current_font, 0);
    SendDlgItemMessageA(hwnd, 0x666, WM_SETFONT, (WPARAM)s.current_font, 0);
    SendDlgItemMessageA(hwnd, 0x671, WM_SETFONT, (WPARAM)s.current_font, 0);
    SendDlgItemMessageA(hwnd, 0x672, WM_SETFONT, (WPARAM)s.current_font, 0);
    SendDlgItemMessageA(hwnd, 0x664, WM_SETFONT, (WPARAM)s.current_font, 0);
    SendDlgItemMessageA(hwnd, 0x66f, WM_SETFONT, (WPARAM)s.current_font, 0);
    SendDlgItemMessageA(hwnd, IDOK, WM_SETFONT, (WPARAM)s.current_font, 0);
    s.font_description = LoadFontFromIni("ShellDialogLabel", 0);
    s.current_font = CreateFontIndirectA(s.font_description);
    SendDlgItemMessageA(hwnd, 0x65f, WM_SETFONT, (WPARAM)s.current_font, 0);
    SendDlgItemMessageA(hwnd, 0x65e, WM_SETFONT, (WPARAM)s.current_font, 0);
    SendDlgItemMessageA(hwnd, 0x665, WM_SETFONT, (WPARAM)s.current_font, 0);
    SendDlgItemMessageA(hwnd, 0x66a, WM_SETFONT, (WPARAM)s.current_font, 0);
    SendDlgItemMessageA(hwnd, 0x70b, WM_SETFONT, (WPARAM)s.current_font, 0);

    load_active_screen_name_profile();
    SetDlgItemTextA(hwnd, 0x65f, g_screen_name_profile.screen_name);
    sprintf((char *)g_duel_state_block_008b3fc0, "%s\\PlayFace\\%s.PIC",
            global_base_directory, g_screen_name_profile.playface_name);
    SendDlgItemMessageA(hwnd, 0x675, 0x465, 0, 0x10005);
    SendDlgItemMessageA(hwnd, 0x675, 0x467, 0,
                        (unsigned short)g_sealed_deck_options.best_of);
    SendDlgItemMessageA(hwnd, 0x672, EM_LIMITTEXT, 1, 0);
    if (g_sealed_deck_options.ante != 0)
      CheckDlgButton(hwnd, 0x666, TRUE);
    SendDlgItemMessageA(hwnd, 0x662, 0x465, 0, 0x30005);
    SendDlgItemMessageA(hwnd, 0x662, 0x467, 0,
                        (unsigned short)g_sealed_deck_options.rounds);
    SendDlgItemMessageA(hwnd, 0x661, EM_LIMITTEXT, 1, 0);
    SendDlgItemMessageA(hwnd, 0x66e, 0x465, 0, 10);
    SendDlgItemMessageA(hwnd, 0x66e, 0x467, 0,
                        (unsigned short)g_sealed_deck_options.free_lands);
    SendDlgItemMessageA(hwnd, 0x66d, EM_LIMITTEXT, 2, 0);
    if (g_sealed_deck_options.free_lands == 0)
      SetDlgItemInt(hwnd, 0x66d, 0, FALSE);
    if (g_sealed_deck_options.minimum_deck_size == 60)
      CheckRadioButton(hwnd, 0x673, 0x674, 0x674);
    else
      CheckRadioButton(hwnd, 0x673, 0x674, 0x673);

    for (s.init_pack_index = 0; s.init_pack_index < 2; ++s.init_pack_index)
    {
      s.pack_type = g_sealed_deck_selected_starters[s.init_pack_index];
      if (s.pack_type >= 0)
      {
        while (shell_sealed_booster_pack_available(s.pack_type) == 0 ||
               shell_sealed_starter_pack_available(s.pack_type) == 0)
          s.pack_type = (s.pack_type + 1) % 9;
        g_sealed_deck_selected_starters[s.init_pack_index] = s.pack_type;
      }
    }
    for (s.init_pack_index = 0; s.init_pack_index < 4; ++s.init_pack_index)
    {
      s.pack_type = g_sealed_deck_selected_boosters[s.init_pack_index];
      if (s.pack_type >= 0)
      {
        while (shell_sealed_starter_pack_available(s.pack_type) == 0)
          s.pack_type = (s.pack_type + 1) % 9;
        g_sealed_deck_selected_boosters[s.init_pack_index] = s.pack_type;
      }
    }
    shell_save_sealed_deck_registry_options();
    load_text(global_ui_strings_filename, "SHELLPAGE_SEALEDDECK");
    SetDlgItemTextA(hwnd, 0x65e, g_text_lines[0]);
    SetDlgItemTextA(hwnd, 0x664, g_text_lines[1]);
    SetDlgItemTextA(hwnd, 0x70b, g_text_lines[2]);
    SetDlgItemTextA(hwnd, 0x660, g_text_lines[3]);
    strcpy(g_sealed_deck_rounds_format, g_text_lines[4]);
    sprintf(s.init_text, g_sealed_deck_rounds_format,
            1 << (unsigned char)g_sealed_deck_options.rounds);
    SetDlgItemTextA(hwnd, 0x663, s.init_text);
    strcpy(g_sealed_deck_card_count_format, g_text_lines[5]);
    sprintf(s.init_text, g_sealed_deck_card_count_format,
            shell_sealed_deck_card_count());
    SetDlgItemTextA(hwnd, 0x66a, s.init_text);
    SetDlgItemTextA(hwnd, 0x66c, g_text_lines[6]);
    SetDlgItemTextA(hwnd, 0x670, g_text_lines[7]);
    SetDlgItemTextA(hwnd, 0x673, g_text_lines[8]);
    SetDlgItemTextA(hwnd, 0x674, g_text_lines[9]);
    SetDlgItemTextA(hwnd, 0x665, g_text_lines[10]);
    SetDlgItemTextA(hwnd, 0x666, g_text_lines[11]);
    SetDlgItemTextA(hwnd, 0x671, g_text_lines[12]);
    SetDlgItemTextA(hwnd, IDOK, g_text_lines[13]);
    SetDlgItemTextA(hwnd, 0x66f, g_text_lines[14]);
    SetFocus(GetDlgItem(hwnd, IDOK));
    SendMessageA(hwnd, 0x401, 1, 0);
    change_buttonclass_wndproc(hwnd);
    s.dialog_rect = (RECT *)lparam;
    MoveWindow(hwnd, s.dialog_rect->left, s.dialog_rect->top,
               s.dialog_rect->right - s.dialog_rect->left,
               s.dialog_rect->bottom - s.dialog_rect->top, TRUE);
    return FALSE;

  case WM_DESTROY:
    s.destroy_font = (HGDIOBJ)SendDlgItemMessageA(hwnd, 0x65f, WM_GETFONT, 0, 0);
    SendDlgItemMessageA(hwnd, 0x65f, WM_SETFONT, 0, 0);
    SendDlgItemMessageA(hwnd, 0x65e, WM_SETFONT, 0, 0);
    SendDlgItemMessageA(hwnd, 0x665, WM_SETFONT, 0, 0);
    SendDlgItemMessageA(hwnd, 0x66a, WM_SETFONT, 0, 0);
    SendDlgItemMessageA(hwnd, 0x70b, WM_SETFONT, 0, 0);
    DeleteObject(s.destroy_font);
    s.destroy_font = (HGDIOBJ)SendDlgItemMessageA(hwnd, 0x660, WM_GETFONT, 0, 0);
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
    DeleteObject(s.destroy_font);
    shell_release_sealed_deck_art_resources();
    return FALSE;

  case WM_COMMAND:
    s.command_id = wparam & 0xffff;
    s.notification = HIWORD(wparam);
    s.command_control = (HWND)lparam;
    switch (s.command_id)
    {
    case 0x703:
      g_sealed_deck_build_report_enabled = 0;
      g_sealed_deck_build_report_extra_enabled = 0;
      shell_enable_animation(0);
      ShowWindow(global_main_hwnd, SW_HIDE);
      shell_write_sealed_deck_build_report();
      ShowWindow(global_main_hwnd, SW_SHOW);
      shell_enable_animation(1);
      g_sealed_deck_build_report_enabled = 0;
      g_sealed_deck_build_report_extra_enabled = 0;

      break;

    case IDOK:
      GetDlgItemTextA(hwnd, 0x65f, g_player_name, 100);
      g_duel_network_flags = 1;
      shell_enable_animation(0);
      PostMessageA(global_main_hwnd, 0x496, 0, 0);
      s.new_duel_result = shell_run_sealed_deck_duel(0);
      PostMessageA(global_main_hwnd, 0x496, 1, 0);
      SetFocus(GetDlgItem(hwnd, IDOK));
      shell_enable_animation(1);

      break;

    case 0x66f:
      g_duel_network_flags = 1;
      Sleep(GetDoubleClickTime());
      s.autosave_queued = PeekMessageA(
          &s.pending_message, NULL, WM_LBUTTONDBLCLK,
          WM_LBUTTONDBLCLK, PM_REMOVE);
      shell_prepare_save_game_dialog(3);
      g_duel_save_game_openfilename.hwndOwner = global_main_hwnd;
      load_text(global_ui_strings_filename, "WINDOWTITLES");
      g_duel_save_game_openfilename.lpstrTitle = g_text_lines[6];
      g_duel_save_game_openfilename.Flags = 0x2a100c;
      if (s.autosave_queued != 0)
      {
        strcpy(s.autosave_path, global_savegame_path);
        strcat(s.autosave_path, "\\AUTOSAVE.");
        strcat(s.autosave_path,
               g_duel_save_game_openfilename.lpstrDefExt);
        strcpy(g_duel_save_game_openfilename.lpstrFile, s.autosave_path);
        InitializeDuelUiGlobalIds();
        s.save_load_result = load_duel_run_mode_3_save(s.autosave_path);
        s.user_cancelled = 0;
      }
      else
      {
        if (GetOpenFileNameA(&g_duel_save_game_openfilename) != 0)
        {
          InitializeDuelUiGlobalIds();
          s.save_load_result = load_duel_run_mode_3_save(
              g_duel_save_game_openfilename.lpstrFile);
          s.user_cancelled = 0;
        }
        else
        {
          s.save_load_result = 0;
          s.user_cancelled = 1;
        }
      }
      shell_enable_animation(0);
      if (s.save_load_result != 0)
      {
        SendDlgItemMessageA(hwnd, 0x675, 0x467, 0,
                            (unsigned short)g_sealed_deck_options.best_of);
        PostMessageA(global_main_hwnd, 0x496, 0, 0);
        s.resumed_duel_result = shell_run_sealed_deck_duel(1);
        SendMessageA(global_main_hwnd, 0x496, 1, 0);
      }
      else
      {
        if (s.user_cancelled == 0)
          MessageBoxA(
              hwnd,
              "Couldn't load the save game; corrupt file or from a different version",
              "Load saved game", 0);
      }
      shell_enable_animation(1);

      break;

    case 0x664:
      shell_enable_animation(0);
      shell_open_duel_interface_options(global_main_hwnd);
      shell_enable_animation(1);
      SetFocus(GetDlgItem(hwnd, IDOK));

      break;

    case 0x661:
      if (s.notification == EN_KILLFOCUS)
      {
        s.rounds_value = (int)SendDlgItemMessageA(hwnd, 0x662, 0x468, 0, 0);
        s.rounds_error = HIWORD(s.rounds_value) != 0;
        if (s.rounds_error != 0)
        {
          SendDlgItemMessageA(hwnd, 0x662, 0x467, 0,
                              (unsigned short)g_sealed_deck_options.rounds);
        }
        else
        {
          if (g_sealed_deck_options.rounds != s.rounds_value)
          {
            g_sealed_deck_options.rounds = s.rounds_value;
            shell_save_sealed_deck_registry_options();
            s.rounds_label = GetDlgItem(hwnd, 0x663);
            ShowWindow(s.rounds_label, SW_HIDE);
            GetWindowRect(s.rounds_label, &s.rounds_rect);
            MapWindowPoints(NULL, hwnd, (LPPOINT)&s.rounds_rect, 2);
            InvalidateRect(hwnd, &s.rounds_rect, TRUE);
            sprintf(s.rounds_text, g_sealed_deck_rounds_format,
                    1 << (unsigned char)g_sealed_deck_options.rounds);
            SetDlgItemTextA(hwnd, 0x663, s.rounds_text);
            ShowWindow(s.rounds_label, SW_SHOW);
          }
        }

      }
      break;

    case 0x66d:
      if (s.notification == EN_KILLFOCUS)
      {
        s.free_lands_value = (int)SendDlgItemMessageA(hwnd, 0x66e, 0x468, 0, 0);
        s.free_lands_error = HIWORD(s.free_lands_value) != 0;
        if (s.free_lands_error != 0)
        {
          SendDlgItemMessageA(hwnd, 0x66e, 0x467, 0,
                              (unsigned short)g_sealed_deck_options.free_lands);
        }
        else
        {
          if (g_sealed_deck_options.free_lands != s.free_lands_value)
          {
            g_sealed_deck_options.free_lands = s.free_lands_value;
            shell_save_sealed_deck_registry_options();
          }
        }

      }
      break;

    case 0x673:
    case 0x674:
      if (IsDlgButtonChecked(hwnd, 0x673) != 0)
        g_sealed_deck_options.minimum_deck_size = 40;
      else if (IsDlgButtonChecked(hwnd, 0x674) != 0)
        g_sealed_deck_options.minimum_deck_size = 60;
      shell_save_sealed_deck_registry_options();

      break;

    case 0x672:
      if (s.notification == EN_KILLFOCUS)
      {
        s.best_of_value = (int)SendDlgItemMessageA(hwnd, 0x675, 0x468, 0, 0);
        s.best_of_error = HIWORD(s.best_of_value) != 0;
        if (s.best_of_error != 0)
        {
          SendDlgItemMessageA(hwnd, 0x675, 0x467, 0,
                              (unsigned short)g_sealed_deck_options.best_of);
        }
        else
        {
          if (s.best_of_value % 2 == 0)
          {
            if (g_sealed_deck_options.best_of < s.best_of_value)
              ++s.best_of_value;
            else
              --s.best_of_value;
            SendDlgItemMessageA(hwnd, 0x675, 0x467, 0,
                                s.best_of_value & 0xffff);
            s.best_of_value = (int)SendDlgItemMessageA(hwnd, 0x675,
                                                   0x468, 0, 0);
          }
          if (g_sealed_deck_options.best_of != s.best_of_value)
          {
            g_sealed_deck_options.best_of = s.best_of_value;
            shell_save_sealed_deck_registry_options();
          }
        }

      }
      break;

    case 0x666:
      g_sealed_deck_options.ante = IsDlgButtonChecked(hwnd, 0x666);
      shell_save_sealed_deck_registry_options();

      break;

    }
    return TRUE;

  case WM_NOTIFY:
    s.notification_control = wparam;
    if (s.notification_control == 0x66e)
      PostMessageA(hwnd, WM_COMMAND, 0x200066d,
                   (LPARAM)GetDlgItem(hwnd, 0x66d));
    else if (s.notification_control == 0x662)
      PostMessageA(hwnd, WM_COMMAND, 0x2000661,
                   (LPARAM)GetDlgItem(hwnd, 0x661));
    else if (s.notification_control == 0x675)
      PostMessageA(hwnd, WM_COMMAND, 0x2000672,
                   (LPARAM)GetDlgItem(hwnd, 0x672));
    return FALSE;

  case WM_LBUTTONDOWN:
  case WM_LBUTTONDBLCLK:
    s.click_point.x = lparam & 0xffff;
    s.click_point.y = HIWORD(lparam);
    GetWindowRect(GetDlgItem(hwnd, 0x66a), &s.clickable_region);
    MapWindowPoints(NULL, hwnd, (LPPOINT)&s.clickable_region, 2);
    s.clickable_region.top += GetSystemMetrics(SM_CYCAPTION);
    s.changed_pack = 0;
    if (PtInRect(&s.clickable_region, s.click_point))
    {
      s.cell_width = (s.clickable_region.right - s.clickable_region.left) / 6;
      SetRect(&s.pack_cell, s.clickable_region.left, s.clickable_region.top,
              s.clickable_region.left + s.cell_width,
              s.clickable_region.bottom);
      if (PtInRect(&s.pack_cell, s.click_point))
      {
        s.next_pack = (g_sealed_deck_selected_starters[0] + 1) % 9;
        while (!shell_sealed_booster_pack_available(s.next_pack) ||
               !shell_sealed_starter_pack_available(s.next_pack))
          s.next_pack = (s.next_pack + 1) % 9;
        g_sealed_deck_selected_starters[0] = s.next_pack;
        InvalidateRect(hwnd, &s.pack_cell, TRUE);
        s.changed_pack = 1;
      }
      OffsetRect(&s.pack_cell, s.cell_width, 0);
      if (PtInRect(&s.pack_cell, s.click_point) && !s.changed_pack)
      {
        s.next_pack = g_sealed_deck_selected_starters[1] + 1;
        while (s.next_pack < 9 &&
               (!shell_sealed_booster_pack_available(s.next_pack) ||
                !shell_sealed_starter_pack_available(s.next_pack)))
          ++s.next_pack;
        if (s.next_pack == 9)
          s.next_pack = -1;
        g_sealed_deck_selected_starters[1] = s.next_pack;
        InvalidateRect(hwnd, &s.pack_cell, TRUE);
        if (g_sealed_deck_selected_starters[1] == -1)
        {
          s.compact_index = 0;
          for (s.click_index = 0; s.click_index < 4; ++s.click_index)
          {
            if (g_sealed_deck_selected_boosters[s.click_index] != -1)
            {
              if (s.compact_index != s.click_index)
              {
                g_sealed_deck_selected_boosters[s.compact_index] =
                    g_sealed_deck_selected_boosters[s.click_index];
                g_sealed_deck_selected_boosters[s.click_index] = -1;
                InvalidateRect(hwnd, &s.clickable_region, TRUE);
              }
              ++s.compact_index;
            }
          }
          for (s.click_index = s.compact_index; s.click_index < 2;
               ++s.click_index)
          {
            g_sealed_deck_selected_boosters[s.click_index] = 2;
            InvalidateRect(hwnd, &s.clickable_region, TRUE);
          }
        }
        s.changed_pack = 1;
      }
      OffsetRect(&s.pack_cell, s.cell_width, 0);
      s.booster_start = g_sealed_deck_selected_starters[1] == -1 ? 2 : 0;
      for (s.click_index = 0; s.click_index < 4 && !s.changed_pack;
           ++s.click_index)
      {
        if (PtInRect(&s.pack_cell, s.click_point))
        {
          if (s.click_index < s.booster_start)
          {
            s.next_pack =
                (g_sealed_deck_selected_boosters[s.click_index] + 1) % 9;
            while (!shell_sealed_starter_pack_available(s.next_pack))
              s.next_pack = (s.next_pack + 1) % 9;
          }
          else
          {
            s.next_pack =
                g_sealed_deck_selected_boosters[s.click_index] + 1;
            while (s.next_pack < 9 &&
                   !shell_sealed_starter_pack_available(s.next_pack))
              ++s.next_pack;
            if (s.next_pack == 9)
              s.next_pack = -1;
          }
          g_sealed_deck_selected_boosters[s.click_index] = s.next_pack;
          InvalidateRect(hwnd, &s.pack_cell, TRUE);
          s.changed_pack = 1;
        }
        OffsetRect(&s.pack_cell, s.cell_width, 0);
      }
      if (s.changed_pack)
      {
        g_sealed_deck_options.starter_packs[0] =
            g_sealed_deck_selected_starters[0];
        if (g_sealed_deck_selected_starters[1] == -1)
          g_sealed_deck_options.starter_count = 1;
        else
        {
          g_sealed_deck_options.starter_packs[1] =
              g_sealed_deck_selected_starters[1];
          g_sealed_deck_options.starter_count = 2;
        }
        s.compact_index = 0;
        for (s.click_index = 0; s.click_index < 4; ++s.click_index)
        {
          if (g_sealed_deck_selected_boosters[s.click_index] != -1)
            g_sealed_deck_options.booster_packs[s.compact_index++] =
                g_sealed_deck_selected_boosters[s.click_index];
        }
        g_sealed_deck_options.booster_count = s.compact_index;
        shell_save_sealed_deck_registry_options();
        sprintf(s.click_text, g_sealed_deck_card_count_format,
                shell_sealed_deck_card_count());
        SetDlgItemTextA(hwnd, 0x66a, s.click_text);
      }
      s.click_result = TRUE;
    }
    else
      s.click_result = FALSE;
    return s.click_result;

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
      s.control_brush = shell_draw_resources.light_brush;
    }
    else if (s.control_id == 0x661 || s.control_id == 0x66d ||
             s.control_id == 0x672)
    {
      SetTextColor(s.control_dc, shell_draw_resources.background_color);
      SetBkMode(s.control_dc, OPAQUE);
      SetBkColor(s.control_dc, shell_draw_resources.panel_color);
      s.control_brush = shell_draw_resources.panel_brush;
    }
    else
    {
      if (GetFocus() == s.control)
        SetTextColor(s.control_dc,
                     shell_draw_resources.selected_button_color);
      else if (s.control_id == 0x65f)
        SetTextColor(s.control_dc,
                     shell_draw_resources.selected_button_color);
      else
        SetTextColor(s.control_dc,
                     shell_draw_resources.button_text_color);
      SetBkMode(s.control_dc, TRANSPARENT);
      s.control_brush = GetStockObject(NULL_BRUSH);
    }
    return (BOOL)s.control_brush;

  case WM_DRAWITEM:
    s.draw_item = (DRAWITEMSTRUCT *)lparam;
    if (GetFocus() == s.draw_item->hwndItem)
      s.draw_color = shell_draw_resources.selected_button_color;
    else if ((s.draw_item->itemState & ODS_GRAYED) != 0)
      s.draw_color = 0x10000c6;
    else
      s.draw_color = shell_draw_resources.regular_button_color;
    if (shell_draw_resources.button_face != NULL)
      shell_draw_bitmap_button(
          s.draw_item, shell_draw_resources.button_face,
          shell_draw_resources.light_pen, shell_draw_resources.dark_pen,
          s.draw_color, 0);
    else
      draw_owner_draw_button_centered(
          s.draw_item, GetStockObject(GRAY_BRUSH),
          shell_draw_resources.light_pen, shell_draw_resources.dark_pen,
          s.draw_color, 0);
    return TRUE;

  case 0x30f:
  case 0x310:
  case 0x311:
    return handle_button_palette_message((int)hwnd, message,
                                         (int)wparam, (int)lparam);

  case WM_ERASEBKGND:
    s.erase_dc = (HDC)wparam;
    ApplyCardArtPaletteToDc(s.erase_dc);
    GetClientRect(hwnd, &s.erase_rect);
    MapWindowPoints(hwnd, GetParent(hwnd), (LPPOINT)&s.erase_rect, 2);
    SendMessageA(GetParent(hwnd), 0x498, (WPARAM)s.erase_dc,
                 (LPARAM)&s.erase_rect);
    return TRUE;

  case WM_PAINT:
    s.paint_background = 0;
    s.paint_dc = BeginPaint(hwnd, &s.paint);
    ApplyCardArtPaletteToDc(s.paint_dc);
    GetClientRect(hwnd, &s.paint_client_rect);
    s.hatch_brush = CreateHatchBrush(HS_FDIAGONAL, 0xfefefe);
    SelectObject(s.paint_dc, s.hatch_brush);
    SelectObject(s.paint_dc, GetStockObject(NULL_PEN));
    SetBkMode(s.paint_dc, TRANSPARENT);
    s.pack_font_description = LoadFontFromIni("ShellPackLabel", 0);
    s.pack_font = CreateFontIndirectA(s.pack_font_description);
    SelectObject(s.paint_dc, s.pack_font);
    GetTextExtentPoint32A(s.paint_dc, "fun", 3, &s.text_extent);
    s.label_height = s.text_extent.cy;
    SelectObject(s.paint_dc, s.hatch_brush);
    SelectObject(s.paint_dc, GetStockObject(NULL_PEN));
    SetBkMode(s.paint_dc, TRANSPARENT);
    SetBkColor(s.paint_dc, s.paint_background);
    SetTextColor(s.paint_dc, shell_draw_resources.button_text_color);
    GetWindowRect(GetDlgItem(hwnd, 0x66a), &s.pack_region);
    MapWindowPoints(NULL, hwnd, (LPPOINT)&s.pack_region, 2);
    s.pack_region.top += GetSystemMetrics(SM_CYCAPTION);
    s.gap = 5;
    s.pack_width = (s.pack_region.right - s.pack_region.left - s.gap) / 6;
    s.pack_width -= s.gap;
    SetRect(&s.pack_image_rect,
            s.pack_region.left + s.gap, s.pack_region.top + 5,
            s.pack_region.left + s.gap + s.pack_width,
            s.pack_region.bottom - 5 - s.label_height - 5);
    SetRect(&s.pack_label_rect,
            s.pack_image_rect.left, s.pack_image_rect.bottom + 5,
            s.pack_image_rect.right, s.pack_region.bottom - 5);
    for (s.paint_index = 0; s.paint_index < 2; ++s.paint_index)
    {
      if (g_sealed_deck_selected_starters[s.paint_index] == -1)
      {
        shell_draw_sealed_starter_pack(s.paint_dc, &s.pack_image_rect,
                                       -1, 0, 0);
        FillRect(s.paint_dc, &s.pack_label_rect,
                 GetStockObject(BLACK_BRUSH));
      }
      else
      {
        shell_draw_sealed_starter_pack(
            s.paint_dc, &s.pack_image_rect,
            g_sealed_deck_selected_starters[s.paint_index], 0, 0);
        if (shell_sealed_starter_pack_available(
                g_sealed_deck_selected_starters[s.paint_index]) == 0)
          Rectangle(s.paint_dc, s.pack_image_rect.left,
                    s.pack_image_rect.top, s.pack_image_rect.right,
                    s.pack_image_rect.bottom);
        shell_draw_sealed_pack_label(
            s.paint_dc, &s.pack_label_rect,
            g_sealed_deck_selected_starters[s.paint_index]);
      }
      OffsetRect(&s.pack_image_rect, s.gap + s.pack_width, 0);
      OffsetRect(&s.pack_label_rect, s.gap + s.pack_width, 0);
    }
    for (s.paint_index = 0; s.paint_index < 4; ++s.paint_index)
    {
      if (g_sealed_deck_selected_boosters[s.paint_index] == -1)
      {
        shell_draw_sealed_booster_pack(s.paint_dc, &s.pack_image_rect,
                                       -1, 0, 0);
        FillRect(s.paint_dc, &s.pack_label_rect,
                 GetStockObject(BLACK_BRUSH));
      }
      else
      {
        shell_draw_sealed_booster_pack(
            s.paint_dc, &s.pack_image_rect,
            g_sealed_deck_selected_boosters[s.paint_index], 0, 0);
        if (shell_sealed_starter_pack_available(
                g_sealed_deck_selected_boosters[s.paint_index]) == 0)
          Rectangle(s.paint_dc, s.pack_image_rect.left,
                    s.pack_image_rect.top, s.pack_image_rect.right,
                    s.pack_image_rect.bottom);
        shell_draw_sealed_pack_label(
            s.paint_dc, &s.pack_label_rect,
            g_sealed_deck_selected_boosters[s.paint_index]);
      }
      OffsetRect(&s.pack_image_rect, s.gap + s.pack_width, 0);
      OffsetRect(&s.pack_label_rect, s.gap + s.pack_width, 0);
    }
    EndPaint(hwnd, &s.paint);
    DeleteObject(s.hatch_brush);
    DeleteObject(s.pack_font);
    return FALSE;

  case WM_SIZE:
    s.resize_group_count = 0;
    s.resize_control_count = 0;
    s.resize_group_first[s.resize_group_count] = s.resize_control_count;
    s.resize_controls[s.resize_control_count++] = GetDlgItem(hwnd, 0x65e);
    s.resize_controls[s.resize_control_count++] = GetDlgItem(hwnd, 0x65f);
    s.resize_controls[s.resize_control_count++] = GetDlgItem(hwnd, 0x70b);
    s.resize_controls[s.resize_control_count++] = GetDlgItem(hwnd, 0x665);
    s.resize_controls[s.resize_control_count++] = GetDlgItem(hwnd, 0x66a);
    s.resize_group_last[s.resize_group_count] = s.resize_control_count - 1;
    s.resize_old_fonts[s.resize_group_count] = (HFONT)SendMessageA(
        s.resize_controls[s.resize_control_count - 1], WM_GETFONT, 0, 0);
    s.resize_font_description = LoadFontFromIni("ShellDialogLabel", 0);
    s.resize_base_height[s.resize_group_count] = s.resize_font_description->lfHeight;
    ++s.resize_group_count;

    s.resize_group_first[s.resize_group_count] = s.resize_control_count;
    s.resize_controls[s.resize_control_count++] = GetDlgItem(hwnd, 0x660);
    s.resize_controls[s.resize_control_count++] = GetDlgItem(hwnd, 0x661);
    s.resize_controls[s.resize_control_count++] = GetDlgItem(hwnd, 0x662);
    s.resize_controls[s.resize_control_count++] = GetDlgItem(hwnd, 0x663);
    s.resize_controls[s.resize_control_count++] = GetDlgItem(hwnd, 0x670);
    s.resize_controls[s.resize_control_count++] = GetDlgItem(hwnd, 0x673);
    s.resize_controls[s.resize_control_count++] = GetDlgItem(hwnd, 0x674);
    s.resize_controls[s.resize_control_count++] = GetDlgItem(hwnd, 0x66c);
    s.resize_controls[s.resize_control_count++] = GetDlgItem(hwnd, 0x66d);
    s.resize_controls[s.resize_control_count++] = GetDlgItem(hwnd, 0x66e);
    s.resize_controls[s.resize_control_count++] = GetDlgItem(hwnd, 0x666);
    s.resize_controls[s.resize_control_count++] = GetDlgItem(hwnd, 0x671);
    s.resize_controls[s.resize_control_count++] = GetDlgItem(hwnd, 0x672);
    s.resize_controls[s.resize_control_count++] = GetDlgItem(hwnd, 0x675);
    s.resize_controls[s.resize_control_count++] = GetDlgItem(hwnd, 0x664);
    s.resize_controls[s.resize_control_count++] = GetDlgItem(hwnd, 0x66f);
    s.resize_controls[s.resize_control_count++] = GetDlgItem(hwnd, IDOK);
    s.resize_group_last[s.resize_group_count] = s.resize_control_count - 1;
    GetClientRect(global_main_hwnd, &s.resize_client_rect);
    if (s.resize_client_rect.right < 0x2ee ||
        s.resize_client_rect.bottom < 0x226)
    {
      s.resize_font_description = LoadFontFromIni("ShellDialog_lores", 0);
      s.resize_old_fonts[s.resize_group_count] = g_sealed_deck_low_resolution_font;
    }
    else
    {
      s.resize_font_description = LoadFontFromIni("ShellDialog", 0);
      s.resize_old_fonts[s.resize_group_count] = g_sealed_deck_dialog_font;
    }
    s.resize_base_height[s.resize_group_count] = s.resize_font_description->lfHeight;
    ++s.resize_group_count;
    LockWindowUpdate(hwnd);
    for (s.resize_group_index = 0;
         s.resize_group_index < s.resize_group_count;
         ++s.resize_group_index)
    {
      GetObjectA(s.resize_old_fonts[s.resize_group_index],
                 sizeof(s.resize_font), &s.resize_font);
      GetClientRect(global_main_hwnd, &s.resize_client_rect);
      s.resize_font.lfHeight =
          (s.resize_client_rect.bottom - s.resize_client_rect.top) /
          s.resize_base_height[s.resize_group_index];
      if (s.resize_font.lfHeight % 2 != 0)
        ++s.resize_font.lfHeight;
      s.resize_replacement_font = CreateFontIndirectA(&s.resize_font);
      if (s.resize_old_fonts[s.resize_group_index] ==
          g_sealed_deck_low_resolution_font)
        g_sealed_deck_low_resolution_font = s.resize_replacement_font;
      else if (s.resize_old_fonts[s.resize_group_index] ==
               g_sealed_deck_dialog_font)
        g_sealed_deck_dialog_font = s.resize_replacement_font;
      SendMessageA(hwnd, WM_SETFONT,
                   (WPARAM)s.resize_replacement_font, 0);
      for (s.resize_control_index =
               s.resize_group_first[s.resize_group_index];
           s.resize_control_index <=
               s.resize_group_last[s.resize_group_index];
           ++s.resize_control_index)
        SendMessageA(s.resize_controls[s.resize_control_index],
                     WM_SETFONT, (WPARAM)s.resize_replacement_font, 0);
      DeleteObject(s.resize_old_fonts[s.resize_group_index]);
    }
    shell_layout_sealed_deck_dialog(hwnd);
    LockWindowUpdate(NULL);
    return TRUE;

  case 0x499:
    shell_release_sealed_deck_art_resources();
    return FALSE;

  default:
    return FALSE;
  }
}

// FUNCTION: MAGIC 0x004c27ac
int shell_draw_large_sealed_booster(HDC dc, RECT *rect, int pack_type, int variation, int opened,
    int cards_remaining)
{
  struct
  {
    int overlay_height;
    int overlay_width;
    RECT overlay;
    BITMAP bitmap;
    int tile_height;
    int tile_width;
    int source_y;
    int source_x;
    int mask_y;
    RECT destination;
    int mask_x;
    char path[264];
    int saved_dc;
  } s;

  if ((dc == NULL) || (rect == NULL))
  {
    return 0;
  }
  if ((pack_type < 0) || (8 < pack_type))
  {
    return 0;
  }
  if ((variation <= 0) || (g_sealed_pack_definitions[pack_type].color_count - 1 < variation))
  {
    variation = g_sealed_pack_definitions[pack_type].color_count - 1;
  }
  if (g_sealed_deck_art_2 == NULL)
  {
    if (g_display_color_depth == 8)
    {
      sprintf(s.path, "%s\\PACKBOOSTERSlarge.pic", g_exp1_art_path);
      g_sealed_deck_art_2 = load_pic(s.path);
    }
    else
    {
      sprintf(s.path, "%s\\PACKBOOSTERSlarge16.bmp", g_exp1_art_path);
      g_sealed_deck_art_2 = shell_load_bitmap_file(s.path, NULL, 0);
    }
  }
  if ((opened != 0) && (g_sealed_deck_art_3 == NULL))
  {
    if (g_display_color_depth == 8)
    {
      sprintf(s.path, "%s\\PACKBOOSTERSlargeopened.pic", g_exp1_art_path);
      g_sealed_deck_art_3 = load_pic(s.path);
    }
    else
    {
      sprintf(s.path, "%s\\PACKBOOSTERSlargeopened16.bmp", g_exp1_art_path);
      g_sealed_deck_art_3 = shell_load_bitmap_file(s.path, NULL, 0);
    }
  }
  if (g_sealed_deck_art_2 != NULL)
  {
    GetObjectA(g_sealed_deck_art_2, sizeof(s.bitmap), &s.bitmap);
    s.tile_width = s.bitmap.bmWidth / 5;
    s.tile_height = s.bitmap.bmHeight / 2;
    shell_fit_sealed_pack_rect(&s.destination, rect, s.tile_width, s.tile_height);
    if (pack_type == 2)
    {
      s.source_x = s.tile_width * variation;
      s.source_y = 0;
    }
    else if (pack_type == 0)
    {
      s.source_x = 0;
      s.source_y = s.tile_height;
    }
    else if (pack_type == 1)
    {
      s.source_x = s.tile_width;
      s.source_y = s.tile_height;
    }
    else if (pack_type == 3)
    {
      s.source_x = s.tile_width * 2;
      s.source_y = s.tile_height;
    }
    else if (pack_type == 4)
    {
      s.source_x = s.tile_width * 3;
      s.source_y = s.tile_height;
    }
    else
    {
      s.source_x = 0;
      s.source_y = 0;
    }
    s.mask_x = s.bitmap.bmWidth - s.tile_width;
    s.mask_y = s.bitmap.bmHeight - s.tile_height;
    DrawMaskedBitmapToRect(dc, &s.destination, g_sealed_deck_art_2, s.tile_width, s.tile_height, s.source_x,
        s.source_y, s.mask_x, s.mask_y);
    if ((opened != 0) && (g_sealed_deck_art_3 != NULL))
    {
      GetObjectA(g_sealed_deck_art_3, sizeof(s.bitmap), &s.bitmap);
      s.overlay_width = s.bitmap.bmWidth / 6;
      s.overlay_height = s.bitmap.bmHeight / 2;
      if (pack_type == 2)
      {
        s.source_y = s.overlay_height;
      }
      else
      {
        s.source_y = 0;
      }
      if (cards_remaining <= 0)
      {
        s.source_x = s.overlay_width << 2;
      }
      else if (cards_remaining == 1)
      {
        s.source_x = s.overlay_width * 3;
      }
      else if (cards_remaining == 2)
      {
        s.source_x = s.overlay_width * 2;
      }
      else if (cards_remaining == 3)
      {
        s.source_x = s.overlay_width;
      }
      else
      {
        s.source_x = 0;
      }
      s.mask_x = s.bitmap.bmWidth - s.overlay_width;
      s.mask_y = 0;
      s.overlay.right = s.destination.right;
      s.overlay.left = s.overlay.right - ((s.destination.right - s.destination.left) * s.overlay_width) / s.tile_width;
      s.overlay.top = s.destination.top;
      s.overlay.bottom =
      s.overlay.top + ((s.destination.bottom - s.destination.top) * s.overlay_height) / s.tile_height;
      DrawMaskedBitmapToRect(dc, &s.overlay, g_sealed_deck_art_3, s.overlay_width, s.overlay_height,
          s.source_x, s.source_y, s.mask_x, s.mask_y);
    }
  }
  else
  {
    s.saved_dc = SaveDC(dc);
    SelectObject(dc, GetStockObject(GRAY_BRUSH));
    SetTextColor(dc, 0x10101);
    SetBkMode(dc, TRANSPARENT);
    SelectObject(dc, GetStockObject(SYSTEM_FONT));
    Rectangle(dc, rect->left, rect->top, rect->right, rect->bottom);
    TextOutA(dc, rect->left, rect->top, g_sealed_pack_definitions[pack_type].name, 3);
    if (opened != 0)
    {
      TextOutA(dc, rect->left, rect->top + 0x14, "X", 1);
    }
    RestoreDC(dc, s.saved_dc);
  }
  return 1;
}

// FUNCTION: MAGIC 0x004c30e4
int shell_draw_large_sealed_starter(HDC dc, RECT *rect, int pack_type, int variation, int opened,
    int cards_remaining)
{
  struct
  {
    int overlay_height;
    int overlay_width;
    RECT overlay;
    BITMAP bitmap;
    int tile_height;
    int tile_width;
    int source_y;
    int source_x;
    int mask_y;
    RECT destination;
    int mask_x;
    char path[264];
    int saved_dc;
  } s;

  if ((dc == NULL) || (rect == NULL))
  {
    return 0;
  }
  if ((pack_type < 0) || (8 < pack_type))
  {
    return 0;
  }
  if (variation <= 0 ||
      g_sealed_pack_definitions[pack_type].rounds - 1 < variation)
  variation = g_sealed_pack_definitions[pack_type].rounds - 1;
  if (g_sealed_deck_art_5 == NULL)
  {
    sprintf(s.path, "%s\\PACKSTARTERSlarge.pic", g_exp1_art_path);
    g_sealed_deck_art_5 = load_pic(s.path);
  }
  if ((opened != 0) && (g_sealed_deck_art_6 == NULL))
  {
    sprintf(s.path, "%s\\PACKSTARTERSlargeopened.pic", g_exp1_art_path);
    g_sealed_deck_art_6 = load_pic(s.path);
  }
  if (g_sealed_deck_art_5 != NULL)
  {
    GetObjectA(g_sealed_deck_art_5, sizeof(s.bitmap), &s.bitmap);
    s.tile_width = s.bitmap.bmWidth / 4;
    s.tile_height = s.bitmap.bmHeight;
    shell_fit_sealed_pack_rect(&s.destination, rect, s.tile_width, s.tile_height);
    if (pack_type == 2)
    {
      s.source_x = 0;
    }
    else if (pack_type == 1)
    {
      s.source_x = s.tile_width;
    }
    else if (pack_type == 0)
    {
      s.source_x = s.tile_width * 2;
    }
    else
    {
      s.source_x = 0;
    }
    s.source_y = 0;
    s.mask_x = s.bitmap.bmWidth - s.tile_width;
    s.mask_y = 0;
    DrawMaskedBitmapToRect(dc, &s.destination, g_sealed_deck_art_5, s.tile_width, s.tile_height, s.source_x,
        s.source_y, s.mask_x, s.mask_y);
    if ((opened != 0) && (g_sealed_deck_art_6 != NULL))
    {
      GetObjectA(g_sealed_deck_art_6, sizeof(s.bitmap), &s.bitmap);
      s.overlay_width = s.bitmap.bmWidth / 4;
      s.overlay_height = s.bitmap.bmHeight;
      if (cards_remaining <= 0)
      {
        s.source_x = s.overlay_width * 2;
      }
      else if (cards_remaining == 1)
      {
        s.source_x = s.overlay_width;
      }
      else
      {
        s.source_x = 0;
      }
      s.source_y = 0;
      s.mask_x = s.bitmap.bmWidth - s.tile_width;
      s.mask_y = 0;
      s.overlay.right = s.destination.right;
      s.overlay.left = s.overlay.right - ((s.destination.right - s.destination.left) * s.overlay_width) / s.tile_width;
      s.overlay.top = s.destination.top;
      s.overlay.bottom =
      s.overlay.top + ((s.destination.bottom - s.destination.top) * s.overlay_height) / s.tile_height;
      DrawMaskedBitmapToRect(dc, &s.overlay, g_sealed_deck_art_6, s.overlay_width, s.overlay_height,
          s.source_x, s.source_y, s.mask_x, s.mask_y);
    }
  }
  else
  {
    s.saved_dc = SaveDC(dc);
    SelectObject(dc, GetStockObject(DKGRAY_BRUSH));
    SetTextColor(dc, 0x10101);
    SetBkMode(dc, TRANSPARENT);
    SelectObject(dc, GetStockObject(SYSTEM_FONT));
    Rectangle(dc, rect->left, rect->top, rect->right, rect->bottom);
    TextOutA(dc, rect->left, rect->top, g_sealed_pack_definitions[pack_type].name, 3);
    if (opened != 0)
    {
      TextOutA(dc, rect->left, rect->top + 0x14, "X", 1);
    }
    RestoreDC(dc, s.saved_dc);
  }
  return 1;
}
