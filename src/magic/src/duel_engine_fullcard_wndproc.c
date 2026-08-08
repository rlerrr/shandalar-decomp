#include <windows.h>
#include <stdlib.h>
#include <string.h>

#include "cardartlib/src/palette.h"
#include "drawcardlib/Drawcardlib.h"
#include "magic/src/duel_engine.h"
#include "magic/src/game_support.h"
#include "magic/src/global_duel_ui_ids.h"
#include "magic/src/global_other.h"
#include "magic/src/global_state.h"
#include "magic/src/global_strings.h"
#include "magic/src/shared_startup.h"

#define DISPLAYED_PLAYER_CARD_INSTANCE(player_, card_) global_displayed_card_instances[(player_)][(card_)]

#define DUEL_MAIN_WINDOW_HWND g_duel_window_hwnd

extern int global_available_slots;
extern card_ptr_t global_raw_cards_storage[2000];
extern char global_base_directory[];
extern CRITICAL_SECTION g_card_render_lock;

int load_text_with_tab_escapes(char *filename, char *section_name);
LRESULT handle_duel_inactive_cursor(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
unsigned int get_displayed_card_special_counters(int player, int card);
card_id_t get_displayed_card_id(int player, int card);
int get_displayed_card_internal_id(int player, int card);
int displayed_card_indices_invalid(int player, int card);
int get_displayed_card_color_flags(int player, int card);
void get_displayed_card_counter_power_toughness(int player, int card, int *power, int *toughness);
unsigned int draw_displayed_full_card(HDC dc, RECT *rect, card_ptr_t *raw_card, int player, int card, int draw_mode, int expand_text_box);
void draw_displayed_card_overlaid_full_card(HDC dc, RECT *rect, int player, int card);
void draw_special_effect_full_card(HDC dc, RECT *rect, card_id_t card_id, int player, int card);
void save_duel_interface_options_to_registry(void);

// GLOBAL: MAGIC 0x0055e1c8
// GLOBAL: SHANDALAR 0x0057f184
int g_full_card_card_id_window_long_offset = 0;

// GLOBAL: MAGIC 0x0055e1cc
// GLOBAL: SHANDALAR 0x0057f188
int g_full_card_player_window_long_offset = 4;

// GLOBAL: MAGIC 0x0055e1d0
// GLOBAL: SHANDALAR 0x0057f18c
int g_full_card_card_window_long_offset = 8;

// GLOBAL: MAGIC 0x0055e1d4
// GLOBAL: SHANDALAR 0x0057f190
int g_full_card_counters_window_long_offset = 0xc;

// GLOBAL: MAGIC 0x0055e1d8
// GLOBAL: SHANDALAR 0x0057f194
int g_full_card_color_window_long_offset = 0x10;

// GLOBAL: MAGIC 0x0055e1dc
// GLOBAL: SHANDALAR 0x0057f198
int g_full_card_window_extra_bytes = 0x14;

// GLOBAL: MAGIC 0x0057f440
// GLOBAL: SHANDALAR 0x005a0d94
int g_full_card_preview_repositioned = 0;

// GLOBAL: MAGIC 0x00708b10
// GLOBAL: SHANDALAR 0x00732188
char g_full_card_menu_help_text[0x38];

// GLOBAL: MAGIC 0x00708b48
// GLOBAL: SHANDALAR 0x007321c0
char g_full_card_menu_expand_text[0x38];

// GLOBAL: MAGIC 0x00708b80
// GLOBAL: SHANDALAR 0x007321f8
char g_full_card_menu_more_help_text[0x1c];

// GLOBAL: MAGIC 0x00708ba0
// GLOBAL: SHANDALAR 0x00732218
RECT g_full_card_saved_window_rect;

// FUNCTION: MAGIC 0x00557850
// FUNCTION: SHANDALAR 0x0056dbb0
int register_MAGICGAME_FullCardClass(LPCSTR class_name)
{
  struct
  {
    int result;
    WNDCLASSA wndclass;
  } s;

  s.result = 1;
  s.wndclass.style = 0x803;
  s.wndclass.lpfnWndProc = wndproc_MAGICGAME_FullCardClass;
  s.wndclass.cbClsExtra = 0;
  s.wndclass.cbWndExtra = g_full_card_window_extra_bytes;
  s.wndclass.hInstance = g_app_instance;
  s.wndclass.hIcon = (HICON)0;
  s.wndclass.hCursor = LoadCursorA((HINSTANCE)0, (LPCSTR)0x7f00);
  s.wndclass.hbrBackground = GetStockObject(4);
  s.wndclass.lpszMenuName = (LPCSTR)0;
  s.wndclass.lpszClassName = class_name;

  if (RegisterClassA(&s.wndclass) == 0)
  {
    s.result = 0;
  }
  g_magicgame_full_card_menu = CreatePopupMenu();
  load_text_with_tab_escapes(global_ui_strings_filename, "MENU_FULLCARD");
  strcpy(g_full_card_menu_expand_text, text_lines[0]);
  strcpy(g_full_card_menu_help_text, text_lines[1]);
  strcpy(g_full_card_menu_more_help_text, text_lines[2]);
  return s.result;
}

// FUNCTION: MAGIC 0x0055793c
// FUNCTION: SHANDALAR 0x0056dc9c
void destroy_MAGICGAME_FullCardClass(LPCSTR class_name)
{
  if (g_magicgame_full_card_menu != (HMENU)0)
  {
    DestroyMenu(g_magicgame_full_card_menu);
  }

  g_magicgame_full_card_menu = (HMENU)0;
}

// FUNCTION: MAGIC 0x004493ec
// FUNCTION: SHANDALAR 0x00452f69
int displayed_card_has_changed_color_words(int player, int card)
{
  struct
  {
    int result;
    int internal_card_id;
  } s;

  if (displayed_card_indices_invalid(player, card) != 0)
  {
    return 0;
  }

  s.internal_card_id = get_displayed_card_internal_id(player, card);
  if (s.internal_card_id == -1)
  {
    return 0;
  }

  EnterCriticalSection(&g_duel_render_lock);
  if ((DISPLAYED_PLAYER_CARD_INSTANCE(player, card).token_status & 6) != 0)
  {
    s.result = 1;
  }
  else
  {
    s.result = 0;
  }
  LeaveCriticalSection(&g_duel_render_lock);
  return s.result;
}

// FUNCTION: MAGIC 0x0055796a
// FUNCTION: SHANDALAR 0x0056dcca
LRESULT CALLBACK wndproc_MAGICGAME_FullCardClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  struct
  {
    POINT point;
    int windowpos_scaled_height;
    int windowpos_cx;
    int windowpos_scaled_width;
    WINDOWPOS *window_pos;
    POINT timer_cursor;
    int menu_item_count;
    POINT popup_point;
    RECT popup_rect;
    unsigned int draw_result;
    int paint_toughness;
    unsigned int paint_power;
    HDC paint_dc;
    PAINTSTRUCT paint;
    int paint_color;
    RECT client_rect;
    unsigned int paint_counters;
    DWORD paint_start_tick;
    unsigned int mouse_x;
    unsigned int mouse_y;
    RECT mouse_rect;
    int create_height;
    int adjusted_height;
    int create_width;
    int adjusted_width;
    CREATESTRUCTA *create_struct;
    char more_help_path[264];
    DWORD more_help_context;
    char help_path[264];
    DWORD help_context;
    HWND remove_packet;
    int changed;
    int *update_card;
    int displayed_toughness;
    unsigned int displayed_power;
    int color;
    unsigned int counters;
    HWND previous_window_card_id;
    int card;
    unsigned int cached_counters;
    int player;
    LONG cached_color;
    HWND window_card_id;
  } s;

  switch (msg)
  {
  case 0x437:
    return 0;

  case 0x433:
  case 0x434:
    s.previous_window_card_id = (HWND)wparam;
    s.previous_window_card_id = (HWND)GetWindowLongA(hwnd, g_full_card_card_id_window_long_offset);
    if (s.previous_window_card_id == s.previous_window_card_id)
    {
      InvalidateRect(hwnd, (RECT *)0, 0);
    }
    return 0;

  case 0x432:
    s.window_card_id = (HWND)GetWindowLongA(hwnd, g_full_card_card_id_window_long_offset);
    s.player = GetWindowLongA(hwnd, g_full_card_player_window_long_offset);
    s.card = GetWindowLongA(hwnd, g_full_card_card_window_long_offset);
    s.cached_counters = GetWindowLongA(hwnd, g_full_card_counters_window_long_offset);
    if (s.window_card_id == (HWND)unk_007a7d64)
    {
      s.counters = get_displayed_card_special_counters(s.player, s.card);
    }
    else if (s.window_card_id == (HWND)unk_00789734)
    {
      get_displayed_card_counter_power_toughness(s.player, s.card, (int *)&s.displayed_power, &s.displayed_toughness);
      s.counters = MAKELONG(s.displayed_power, s.displayed_toughness);
    }
    else
    {
      s.counters = 0;
    }

    s.cached_color = GetWindowLongA(hwnd, g_full_card_color_window_long_offset);
    if (s.player != -1 && s.card != -1)
    {
      s.color = single_color_test_bit_to_color_t(get_displayed_card_color_flags(s.player, s.card));
    }
    else
    {
      s.color = 0;
    }

    if (s.cached_counters != s.counters)
    {
      InvalidateRect(hwnd, (RECT *)0, 0);
    }
    if (s.cached_color != s.color)
    {
      InvalidateRect(hwnd, (RECT *)0, 0);
    }
    if (s.player != -1 && s.card != -1)
    {
      if (displayed_card_has_changed_color_words(s.player, s.card) != 0)
      {
        InvalidateRect(hwnd, (RECT *)0, 0);
      }
      if ((HWND)get_displayed_card_id(s.player, s.card) != s.window_card_id)
      {
        InvalidateRect(hwnd, (RECT *)0, 0);
      }
    }
    return 0;

  case 0x401:
    s.window_card_id = (HWND)wparam;
    s.update_card = (int *)lparam;
    if ((s.window_card_id == (HWND)unk_007a7d64 ||
         (HWND)unk_00789b80 == s.window_card_id ||
         (HWND)unk_008cf1ac == s.window_card_id ||
         s.window_card_id == (HWND)unk_00789734 ||
         s.window_card_id == (HWND)unk_008a8de8) &&
        (s.update_card == (int *)0 || s.update_card[0] == -1 || s.update_card[1] == -1))
    {
      return 0;
    }
    if (s.update_card != (int *)0 && s.update_card[0] != -1 && s.update_card[1] == -1)
    {
      return 0;
    }

    if (s.update_card != (int *)0)
    {
      s.player = s.update_card[0];
      s.card = s.update_card[1];
    }
    else
    {
      s.player = -1;
      s.card = -1;
    }

    s.changed = 0;
    if ((HWND)GetWindowLongA(hwnd, g_full_card_card_id_window_long_offset) != s.window_card_id)
    {
      s.changed = 1;
    }
    if (GetWindowLongA(hwnd, g_full_card_player_window_long_offset) != s.player ||
        GetWindowLongA(hwnd, g_full_card_card_window_long_offset) != s.card)
    {
      s.changed = 1;
    }

    if (s.changed != 0)
    {
      SetWindowLongA(hwnd, g_full_card_card_id_window_long_offset, (LONG)s.window_card_id);
      SetWindowLongA(hwnd, g_full_card_player_window_long_offset, s.player);
      SetWindowLongA(hwnd, g_full_card_card_window_long_offset, s.card);
      InvalidateRect(hwnd, (RECT *)0, 0);
    }
    else
    {
      SendMessageA(hwnd, 0x432, 0, 0);
    }

    if (g_duel_interface_options.layout == 2)
    {
      ShowWindow(hwnd, SW_SHOW);
      if (g_duel_card_preview_window_hwnd == hwnd)
      {
        SetTimer(hwnd, 1, 12000, (TIMERPROC)0);
      }
      BringWindowToTop(hwnd);
    }
    return 0;

  case 0x40b:
    s.remove_packet = (HWND)wparam;
    s.player = GetWindowLongA(hwnd, g_full_card_player_window_long_offset);
    s.card = GetWindowLongA(hwnd, g_full_card_card_window_long_offset);
    if (((int *)s.remove_packet)[0] == s.player && ((int *)s.remove_packet)[1] == s.card)
    {
      SendMessageA(hwnd, 0x401, 0xffffffff, 0);
      return 1;
    }
    else
    {
      return 0;
    }

  case WM_COMMAND:
    switch ((UINT)wparam & 0xffff)
    {
    case 1:
      g_duel_interface_options.expand_text_box_on_big_card = (g_duel_interface_options.expand_text_box_on_big_card == 0);
      save_duel_interface_options_to_registry();
      InvalidateRect(hwnd, (RECT *)0, 0);
      break;

    case 100:
      s.help_context = GetWindowLongA(hwnd, g_full_card_card_id_window_long_offset);
      if (s.help_context == (DWORD)unk_009266ac)
      {
        s.help_context = 0xc1b;
      }
      if (s.help_context != 0xffffffff)
      {
        strcpy(s.help_path, global_base_directory);
        strcat(s.help_path, "\\duel.hlp");
        WinHelpA(DUEL_MAIN_WINDOW_HWND, s.help_path, HELP_CONTEXT, s.help_context);
      }
      break;

    case 0x65:
      s.more_help_context = 0x7e7;
      strcpy(s.more_help_path, global_base_directory);
      strcat(s.more_help_path, "\\duel.hlp");
      WinHelpA(DUEL_MAIN_WINDOW_HWND, s.more_help_path, HELP_CONTEXT, s.more_help_context);
      break;

    default:
      break;
    }
    return 0;

  case WM_CREATE:
    s.window_card_id = (HWND)-1;
    SetWindowLongA(hwnd, g_full_card_card_id_window_long_offset, (LONG)s.window_card_id);
    s.player = -1;
    s.card = -1;
    SetWindowLongA(hwnd, g_full_card_player_window_long_offset, s.player);
    SetWindowLongA(hwnd, g_full_card_card_window_long_offset, s.card);
    s.cached_counters = 0;
    SetWindowLongA(hwnd, g_full_card_counters_window_long_offset, s.cached_counters);
    s.cached_color = 0;
    SetWindowLongA(hwnd, g_full_card_color_window_long_offset, s.cached_color);
    s.create_struct = (CREATESTRUCTA *)lparam;
    s.create_width = s.create_struct->cx;
    s.create_height = s.create_struct->cy;
    s.adjusted_height = (s.create_height * 200) / 300;
    s.adjusted_width = (s.create_width * 300) / 200;
    if (abs(s.adjusted_height - s.create_width) + abs(s.adjusted_width - s.create_height) > 4)
    {
      if (s.adjusted_height < s.create_width)
      {
        SetWindowPos(hwnd, (HWND)0, 0, 0, s.adjusted_height, s.create_height, 6);
      }
      else
      {
        SetWindowPos(hwnd, (HWND)0, 0, 0, s.create_width, s.adjusted_width, 6);
      }
    }
    return 0;

  case WM_ERASEBKGND:
    return 1;

  case WM_LBUTTONDOWN:
    if (g_duel_interface_options.layout == 2)
    {
      ShowWindow(hwnd, SW_HIDE);
      KillTimer(hwnd, 1);
    }
    return 0;

  case WM_MOUSEMOVE:
    if (GetParent(hwnd) != DUEL_MAIN_WINDOW_HWND)
    {
      return 0;
    }

    return 0;

    if (g_duel_cheats_state == 0)
    {
      return 0;
    }

    s.mouse_x = (unsigned int)lparam & 0xffff;
    s.mouse_y = HIWORD(lparam);
    GetClientRect(hwnd, &s.mouse_rect);
    if ((s.mouse_rect.right * 90) / 100 > (int)s.mouse_x &&
        (s.mouse_rect.bottom * 10) / 100 < (int)s.mouse_y &&
        (s.mouse_rect.bottom * 90) / 100 > (int)s.mouse_y)
    {
      if (g_full_card_preview_repositioned == 0)
      {
        GetWindowRect(hwnd, &g_full_card_saved_window_rect);
        if ((GetWindowLongA(hwnd, GWL_STYLE) & WS_CHILD) != 0)
        {
          MapWindowPoints((HWND)0, GetParent(hwnd), (LPPOINT)&g_full_card_saved_window_rect, 2);
        }
        g_full_card_preview_repositioned = 1;
        CopyRect(&s.mouse_rect, &g_full_card_saved_window_rect);
        if (g_duel_interface_options.layout == 2)
        {
          s.mouse_rect.bottom += ((s.mouse_rect.bottom - s.mouse_rect.top) * 30) / 100;
        }
        else
        {
          s.mouse_rect.top -= ((s.mouse_rect.bottom - s.mouse_rect.top) * 15) / 100;
          s.mouse_rect.bottom += ((s.mouse_rect.bottom - s.mouse_rect.top) * 15) / 100;
        }
        s.mouse_rect.right += ((s.mouse_rect.right - s.mouse_rect.left) * 20) / 100;

        if ((GetWindowLongA(hwnd, GWL_STYLE) & WS_CHILD) != 0)
        {
          MapWindowPoints((HWND)0, GetParent(hwnd), (LPPOINT)&s.mouse_rect, 2);
        }
        BringWindowToTop(hwnd);
        MoveWindow(hwnd,
                   s.mouse_rect.left,
                   s.mouse_rect.top,
                   s.mouse_rect.right - s.mouse_rect.left,
                   s.mouse_rect.bottom - s.mouse_rect.top,
                   1);
      }
    }
    else if (g_full_card_preview_repositioned != 0)
    {
      g_full_card_preview_repositioned = 0;
      SetWindowPos(hwnd, (HWND)1, 0, 0, 0, 0, 3);
      MoveWindow(hwnd,
                 g_full_card_saved_window_rect.left,
                 g_full_card_saved_window_rect.top,
                 g_full_card_saved_window_rect.right - g_full_card_saved_window_rect.left,
                 g_full_card_saved_window_rect.bottom - g_full_card_saved_window_rect.top,
                 1);
    }
    return 0;

  case WM_PAINT:
    s.window_card_id = (HWND)GetWindowLongA(hwnd, g_full_card_card_id_window_long_offset);
    s.player = GetWindowLongA(hwnd, g_full_card_player_window_long_offset);
    s.card = GetWindowLongA(hwnd, g_full_card_card_window_long_offset);
    s.cached_counters = GetWindowLongA(hwnd, g_full_card_counters_window_long_offset);
    s.cached_color = GetWindowLongA(hwnd, g_full_card_color_window_long_offset);
    if ((s.window_card_id == (HWND)-1 ||
         s.window_card_id == (HWND)unk_009266ac ||
         global_available_slots - 1 < (int)s.window_card_id) &&
        g_duel_interface_options.layout == 2 &&
        g_duel_card_preview_window_hwnd == hwnd)
    {
      SendMessageA(hwnd, WM_TIMER, 1, 0);
      return DefWindowProcA(hwnd, msg, wparam, lparam);
    }

    if (s.window_card_id == (HWND)unk_007a7d64)
    {
      s.paint_counters = get_displayed_card_special_counters(s.player, s.card);
    }
    else if (s.window_card_id == (HWND)unk_00789734)
    {
      get_displayed_card_counter_power_toughness(s.player, s.card, (int *)&s.paint_power, &s.paint_toughness);
      s.paint_counters = MAKELONG(s.paint_power, s.paint_toughness);
    }
    else
    {
      s.paint_counters = 0;
    }

    if (s.player != -1 && s.card != -1)
    {
      s.paint_color = single_color_test_bit_to_color_t(get_displayed_card_color_flags(s.player, s.card));
    }
    else
    {
      s.paint_color = 0;
    }
    if (s.cached_counters != s.paint_counters)
    {
      InvalidateRect(hwnd, (RECT *)0, 0);
    }
    if (s.cached_color != s.paint_color)
    {
      InvalidateRect(hwnd, (RECT *)0, 0);
    }

    EnterCriticalSection(&g_card_render_lock);
    s.paint_start_tick = GetTickCount();
    GetClientRect(hwnd, &s.client_rect);
    s.paint_dc = BeginPaint(hwnd, &s.paint);
    if (s.paint_dc != (HDC)0)
    {
      ApplyCardArtPaletteToDc(s.paint_dc);
      if (g_duel_palette_refresh_pending != 0)
      {
        FillRect(s.paint_dc, &s.client_rect, GetStockObject(0));
        Sleep(200);
      }

      if (global_available_slots - 1 < (int)s.window_card_id)
      {
        FillRect(g_shared_offscreen_dc, &s.client_rect, GetStockObject(4));
        DrawCardBack(g_shared_offscreen_dc, &s.client_rect);
        BitBlt(s.paint_dc, 0, 0, s.client_rect.right, s.client_rect.bottom,
               g_shared_offscreen_dc, 0, 0, SRCCOPY);
      }
      else if ((int)s.window_card_id < 0 || s.window_card_id == (HWND)unk_009266ac)
      {
        FillRect(g_shared_offscreen_dc, &s.client_rect, GetStockObject(4));
        DrawCardBack(g_shared_offscreen_dc, &s.client_rect);
        BitBlt(s.paint_dc, 0, 0, s.client_rect.right, s.client_rect.bottom,
               g_shared_offscreen_dc, 0, 0, SRCCOPY);
      }
      else if (s.window_card_id == (HWND)unk_007a7d64 ||
               (HWND)unk_00789b80 == s.window_card_id ||
               (HWND)unk_008cf1ac == s.window_card_id ||
               s.window_card_id == (HWND)unk_00789734 ||
               s.window_card_id == (HWND)unk_008a8de8)
      {
        FillRect(g_shared_offscreen_dc, &s.client_rect, GetStockObject(4));
        if (get_displayed_card_id(s.player, s.card) != -1)
        {
          draw_special_effect_full_card(g_shared_offscreen_dc, &s.client_rect, (card_id_t)s.window_card_id, s.player, s.card);
        }
        else
        {
          DrawCardBack(g_shared_offscreen_dc, &s.client_rect);
        }
        BitBlt(s.paint_dc, 0, 0, s.client_rect.right, s.client_rect.bottom,
               g_shared_offscreen_dc, 0, 0, SRCCOPY);
      }
      else if (s.window_card_id == (HWND)unk_0092666c)
      {
        FillRect(g_shared_offscreen_dc, &s.client_rect, GetStockObject(4));
        if (get_displayed_card_id(s.player, s.card) != -1)
        {
          draw_displayed_card_overlaid_full_card(g_shared_offscreen_dc, &s.client_rect, s.player, s.card);
        }
        else
        {
          DrawCardBack(g_shared_offscreen_dc, &s.client_rect);
        }
        BitBlt(s.paint_dc, 0, 0, s.client_rect.right, s.client_rect.bottom,
               g_shared_offscreen_dc, 0, 0, SRCCOPY);
      }
      else
      {
        FillRect(g_shared_offscreen_dc, &s.client_rect, GetStockObject(4));
        if (s.player != -1 && s.card != -1 && get_displayed_card_id(s.player, s.card) != -1)
        {
          s.draw_result = draw_displayed_full_card(g_shared_offscreen_dc,
                                                   &s.client_rect,
                                                   global_raw_cards_storage + (int)s.window_card_id,
                                                   s.player,
                                                   s.card,
                                                   0,
                                                   g_duel_interface_options.expand_text_box_on_big_card);
        }
        else if (s.window_card_id != (HWND)-1)
        {
          s.draw_result = DrawFullCard(g_shared_offscreen_dc,
                                       &s.client_rect,
                                       global_raw_cards_storage + (int)s.window_card_id,
                                       0,
                                       0,
                                       g_duel_interface_options.expand_text_box_on_big_card,
                                       gs_illus_00789130);
        }
        else
        {
          DrawCardBack(g_shared_offscreen_dc, &s.client_rect);
          s.draw_result = 1;
        }
        BitBlt(s.paint_dc, 0, 0, s.client_rect.right, s.client_rect.bottom,
               g_shared_offscreen_dc, 0, 0, SRCCOPY);
        if (s.draw_result == 0)
        {
          if (s.player != -1 && s.card != -1 && get_displayed_card_id(s.player, s.card) != -1)
          {
            draw_displayed_full_card(g_shared_offscreen_dc,
                                     &s.client_rect,
                                     global_raw_cards_storage + (int)s.window_card_id,
                                     s.player,
                                     s.card,
                                     2,
                                     g_duel_interface_options.expand_text_box_on_big_card);
          }
          else if (s.window_card_id != (HWND)-1)
          {
            DrawFullCard(g_shared_offscreen_dc,
                         &s.client_rect,
                         global_raw_cards_storage + (int)s.window_card_id,
                         0,
                         2,
                         g_duel_interface_options.expand_text_box_on_big_card,
                         gs_illus_00789130);
          }
          BitBlt(s.paint_dc, 0, 0, s.client_rect.right, s.client_rect.bottom,
                 g_shared_offscreen_dc, 0, 0, SRCCOPY);
        }
      }
      EndPaint(hwnd, &s.paint);
      s.cached_counters = s.paint_counters;
      SetWindowLongA(hwnd, g_full_card_counters_window_long_offset, s.cached_counters);
      s.cached_color = s.paint_color;
      SetWindowLongA(hwnd, g_full_card_color_window_long_offset, s.cached_color);
    }
    g_duel_tick_adjustment += (int)(GetTickCount() - s.paint_start_tick);
    LeaveCriticalSection(&g_card_render_lock);
    return 0;

  case WM_RBUTTONDOWN:
    s.popup_point.x = (unsigned int)lparam & 0xffff;
    s.popup_point.y = HIWORD(lparam);
    ClientToScreen(hwnd, &s.popup_point);
    SetRect(&s.popup_rect,
            s.popup_point.x,
            s.popup_point.y,
            s.popup_point.x + 1,
            s.popup_point.y + 1);
    TrackPopupMenu(g_magicgame_full_card_menu, 2, s.popup_point.x, s.popup_point.y, 0, hwnd, &s.popup_rect);
    return 0;

  case WM_INITMENU:
    AppendMenuA(g_magicgame_full_card_menu, 0, 1, g_full_card_menu_expand_text);
    if (g_duel_interface_options.expand_text_box_on_big_card != 0)
    {
      CheckMenuItem(g_magicgame_full_card_menu, 1, MF_CHECKED);
    }
    AppendMenuA(g_magicgame_full_card_menu, 0, 100, g_full_card_menu_help_text);
    AppendMenuA(g_magicgame_full_card_menu, 0, 0x65, g_full_card_menu_more_help_text);
    return 0;

  case WM_MENUSELECT:
    if (HIWORD(wparam) == 0xffff && lparam == 0)
    {
      s.menu_item_count = GetMenuItemCount(g_magicgame_full_card_menu);
      while ((s.point.x = s.menu_item_count--) != 0)
      {
        DeleteMenu(g_magicgame_full_card_menu, 0, MF_BYPOSITION);
      }
    }
    return 0;

  case WM_TIMER:
    if ((HWND)wparam == (HWND)1 && g_duel_interface_options.layout == 2)
    {
      GetCursorPos(&s.timer_cursor);
      if (WindowFromPoint(s.timer_cursor) != hwnd)
      {
        SendMessageA(hwnd, WM_LBUTTONDOWN, 1, 0);
      }
    }
    return 0;

  case WM_SETCURSOR:
    return handle_duel_inactive_cursor(hwnd, msg, wparam, lparam);

  case WM_WINDOWPOSCHANGING:
    DefWindowProcA(hwnd, msg, wparam, lparam);
    s.window_pos = (WINDOWPOS *)lparam;
    s.windowpos_cx = s.window_pos->cx;
    s.point.y = s.window_pos->cy;
    s.windowpos_scaled_height = (s.point.y * 200) / 300;
    s.windowpos_scaled_width = (s.windowpos_cx * 300) / 200;
    if (abs(s.windowpos_scaled_width - s.point.y) +
            abs(s.windowpos_scaled_height - s.windowpos_cx) <=
        4)
    {
      return 0;
    }
    if (s.windowpos_cx > s.windowpos_scaled_height)
    {
      s.window_pos->cx = s.windowpos_scaled_height;
    }
    else
    {
      s.window_pos->cy = s.windowpos_scaled_width;
    }
    return 0;

  case 0x30f:
  case 0x310:
  case 0x311:
    return handle_button_palette_message((int)hwnd, msg, (int)wparam, lparam);

  default:
    return DefWindowProcA(hwnd, msg, wparam, lparam);
  }
  return 0;
}
