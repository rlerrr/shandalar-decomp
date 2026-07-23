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

int load_text_with_tab_escapes(char *filename, char *section_name);
LRESULT handle_duel_inactive_cursor(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
unsigned int get_displayed_card_special_counters(int player, int card);
card_id_t get_displayed_card_id(int player, int card);
int get_displayed_card_internal_id(int player, int card);
int FUN_004483be(int player, int card);
int FUN_00449057(int player, int card);
void FUN_00449249(int player, int card, int *power, int *toughness);
unsigned int FUN_00559999(int dc, int rect, int raw_card, int player, int card, int param_6, int param_7);
void FUN_00559bc1(int dc, int rect, int player, int card);
void draw_special_effect_full_card(int dc, int rect, card_id_t card_id, int player, int card);
void save_duel_interface_options_to_registry(void);

// GLOBAL: MAGIC 0x0057f444
char s_MENU_FULLCARD_0057f444[0x10] = "MENU_FULLCARD";

// GLOBAL: MAGIC 0x0057f454
char s__duel_hlp_0057f454[0xc] = "\\duel.hlp";

// GLOBAL: MAGIC 0x0057f460
char s__duel_hlp_0057f460[0xc] = "\\duel.hlp";

// GLOBAL: MAGIC 0x0055e1c8
int g_full_card_card_id_window_long_offset = 0;

// GLOBAL: MAGIC 0x0055e1cc
int g_full_card_player_window_long_offset = 4;

// GLOBAL: MAGIC 0x0055e1d0
int g_full_card_card_window_long_offset = 8;

// GLOBAL: MAGIC 0x0055e1d4
int g_full_card_counters_window_long_offset = 0xc;

// GLOBAL: MAGIC 0x0055e1d8
int g_full_card_color_window_long_offset = 0x10;

// GLOBAL: MAGIC 0x0057f440
int g_full_card_preview_repositioned;

// GLOBAL: MAGIC 0x00708b10
char g_full_card_menu_help_text[0x38];

// GLOBAL: MAGIC 0x00708b48
char g_full_card_menu_expand_text[0x38];

// GLOBAL: MAGIC 0x00708b80
char g_full_card_menu_more_help_text[0x1c];

// GLOBAL: MAGIC 0x00708ba0
RECT g_full_card_saved_window_rect;

// FUNCTION: MAGIC 0x00557850
// FUNCTION: SHANDALAR 0x0056dbb0
int register_MAGICGAME_FullCardClass(LPCSTR class_name)
{
  ATOM atom;
  WNDCLASSA wndclass;

  wndclass.style = 0x803;
  wndclass.lpfnWndProc = wndproc_MAGICGAME_FullCardClass;
  wndclass.cbClsExtra = 0;
  wndclass.cbWndExtra = 0x14;
  wndclass.hInstance = g_app_instance;
  wndclass.hIcon = (HICON)0;
  wndclass.hCursor = LoadCursorA((HINSTANCE)0, (LPCSTR)0x7f00);
  wndclass.hbrBackground = GetStockObject(4);
  wndclass.lpszMenuName = (LPCSTR)0;
  wndclass.lpszClassName = class_name;

  atom = RegisterClassA(&wndclass);
  g_magicgame_full_card_menu = CreatePopupMenu();
  load_text_with_tab_escapes(global_ui_strings_filename, s_MENU_FULLCARD_0057f444);
  strcpy(g_full_card_menu_expand_text, text_lines[0]);
  strcpy(g_full_card_menu_help_text, text_lines[1]);
  strcpy(g_full_card_menu_more_help_text, text_lines[2]);
  return atom != 0;
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
int FUN_004493ec(int player, int card)
{
  int result;
  int internal_card_id;

  result = FUN_004483be(player, card);
  if (result == 0)
  {
    internal_card_id = get_displayed_card_internal_id(player, card);
    if (internal_card_id == -1)
    {
      result = 0;
    }
    else
    {
      EnterCriticalSection(&g_duel_render_lock);
      result = (DISPLAYED_PLAYER_CARD_INSTANCE(player, card).token_status & 6) != 0;
      LeaveCriticalSection(&g_duel_render_lock);
    }
  }
  else
  {
    result = 0;
  }

  return result;
}

// FUNCTION: MAGIC 0x0055796a
// FUNCTION: SHANDALAR 0x0056dcca
LRESULT CALLBACK wndproc_MAGICGAME_FullCardClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  struct
  {
    POINT point;
    int keep_width_delta;
    int keep_height_delta;
    int new_height;
    int new_width;
    WINDOWPOS *window_pos;
    RECT mouse_rect;
    unsigned int mouse_y;
    unsigned int mouse_x;
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
    int create_height;
    int adjusted_height;
    int create_width;
    int adjusted_width;
    int *create_struct_words;
    char more_help_path[264];
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
    InvalidateRect(hwnd, (RECT *)0, 0);
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
      FUN_00449249(s.player, s.card, (int *)&s.displayed_power, &s.displayed_toughness);
      s.counters = (s.displayed_toughness << 0x10) | (s.displayed_power & 0xffff);
    }
    else
    {
      s.counters = 0;
    }

    s.cached_color = GetWindowLongA(hwnd, g_full_card_color_window_long_offset);
    if (s.player != -1 && s.card != -1)
    {
      s.color = single_color_test_bit_to_color_t(FUN_00449057(s.player, s.card));
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
      if (FUN_004493ec(s.player, s.card) != 0)
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
         s.window_card_id == (HWND)unk_00789b80 ||
         s.window_card_id == (HWND)unk_008cf1ac ||
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
    return 0;

  case WM_COMMAND:
    switch ((UINT)wparam & 0xffff)
    {
    case 1:
      g_duel_interface_options.expand_text_box_on_big_card =
          (g_duel_interface_options.expand_text_box_on_big_card == 0);
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
        strcat(s.help_path, s__duel_hlp_0057f454);
        WinHelpA(DUEL_MAIN_WINDOW_HWND, s.help_path, HELP_CONTEXT, s.help_context);
      }
      break;

    case 0x65:
      s.help_context = 0x7e7;
      strcpy(s.more_help_path, global_base_directory);
      strcat(s.more_help_path, s__duel_hlp_0057f460);
      WinHelpA(DUEL_MAIN_WINDOW_HWND, s.more_help_path, HELP_CONTEXT, s.help_context);
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
    s.create_struct_words = (int *)lparam;
    s.create_height = s.create_struct_words[4];
    s.create_width = s.create_struct_words[5];
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
    if (g_duel_startup_state == 0)
    {
      return 0;
    }

    s.mouse_x = (unsigned int)lparam & 0xffff;
    s.mouse_y = ((unsigned int)lparam >> 0x10) & 0xffff;
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
      }
      CopyRect(&s.mouse_rect, &g_full_card_saved_window_rect);
      if (g_duel_interface_options.layout == 2)
      {
        s.mouse_rect.bottom += ((s.mouse_rect.bottom - s.mouse_rect.top) * 30) / 100;
      }
      else
      {
        s.mouse_rect.top -= ((s.mouse_rect.bottom - s.mouse_rect.top) * 15) / 100;
        s.mouse_rect.bottom += ((s.mouse_rect.bottom - s.mouse_rect.top) * 15) / 100;
        s.mouse_rect.right += ((s.mouse_rect.right - s.mouse_rect.left) * 20) / 100;
      }
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
      return DefWindowProcA(hwnd, WM_PAINT, wparam, lparam);
    }

    if (s.window_card_id == (HWND)unk_007a7d64)
    {
      s.paint_counters = get_displayed_card_special_counters(s.player, s.card);
    }
    else if (s.window_card_id == (HWND)unk_00789734)
    {
      FUN_00449249(s.player, s.card, (int *)&s.paint_power, &s.paint_toughness);
      s.paint_counters = (s.paint_toughness << 0x10) | (s.paint_power & 0xffff);
    }
    else
    {
      s.paint_counters = 0;
    }

    if (s.player != -1 && s.card != -1)
    {
      s.paint_color = single_color_test_bit_to_color_t(FUN_00449057(s.player, s.card));
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

    EnterCriticalSection(&g_duel_render_lock);
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
               s.window_card_id == (HWND)unk_00789b80 ||
               s.window_card_id == (HWND)unk_008cf1ac ||
               s.window_card_id == (HWND)unk_00789734 ||
               s.window_card_id == (HWND)unk_008a8de8)
      {
        FillRect(g_shared_offscreen_dc, &s.client_rect, GetStockObject(4));
        s.changed = get_displayed_card_id(s.player, s.card);
        if (s.changed != -1)
        {
          draw_special_effect_full_card((int)g_shared_offscreen_dc, (int)&s.client_rect, (card_id_t)s.window_card_id, s.player, s.card);
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
        s.changed = get_displayed_card_id(s.player, s.card);
        if (s.changed != -1)
        {
          FUN_00559bc1((int)g_shared_offscreen_dc, (int)&s.client_rect, s.player, s.card);
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
          s.draw_result = FUN_00559999((int)g_shared_offscreen_dc,
                                       (int)&s.client_rect,
                                       (int)(global_raw_cards_storage + (int)s.window_card_id),
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
            FUN_00559999((int)g_shared_offscreen_dc,
                         (int)&s.client_rect,
                         (int)(global_raw_cards_storage + (int)s.window_card_id),
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
    g_duel_tick_adjustment += GetTickCount() - s.paint_start_tick;
    LeaveCriticalSection(&g_duel_render_lock);
    return 0;

  case WM_RBUTTONDOWN:
    s.popup_point.x = (unsigned int)lparam & 0xffff;
    s.popup_point.y = (unsigned int)lparam >> 0x10;
    ClientToScreen(hwnd, &s.popup_point);
    SetRect(&s.popup_rect,
            s.popup_point.x,
            s.popup_point.y,
            s.popup_point.x + 1,
            s.popup_point.y + 1);
    TrackPopupMenu(g_magicgame_full_card_menu, 2, s.popup_point.x, s.popup_point.y, 0, hwnd, &s.popup_rect);
    return 0;

  case WM_INITMENUPOPUP:
    AppendMenuA(g_magicgame_full_card_menu, 0, 1, g_full_card_menu_expand_text);
    if (g_duel_interface_options.expand_text_box_on_big_card != 0)
    {
      CheckMenuItem(g_magicgame_full_card_menu, 1, MF_CHECKED);
    }
    AppendMenuA(g_magicgame_full_card_menu, 0, 100, g_full_card_menu_help_text);
    AppendMenuA(g_magicgame_full_card_menu, 0, 0x65, g_full_card_menu_more_help_text);
    return 0;

  case WM_MENUSELECT:
    if (((UINT)wparam >> 0x10) == 0xffff && lparam == 0)
    {
      s.menu_item_count = GetMenuItemCount(g_magicgame_full_card_menu);
      while (s.menu_item_count != 0)
      {
        s.menu_item_count--;
        DeleteMenu(g_magicgame_full_card_menu, 0, MF_BYPOSITION);
      }
    }
    return 0;

  case WM_TIMER:
    if ((HWND)wparam == (HWND)1 && g_duel_interface_options.layout == 2)
    {
      GetCursorPos(&s.timer_cursor);
      s.point.x = s.timer_cursor.x;
      s.point.y = s.timer_cursor.y;
      if (WindowFromPoint(s.point) != hwnd)
      {
        SendMessageA(hwnd, WM_LBUTTONDOWN, 1, 0);
      }
    }
    return 0;

  case WM_SETCURSOR:
    return handle_duel_inactive_cursor(hwnd, WM_SETCURSOR, wparam, lparam);

  case WM_WINDOWPOSCHANGING:
    DefWindowProcA(hwnd, WM_WINDOWPOSCHANGING, wparam, lparam);
    s.window_pos = (WINDOWPOS *)lparam;
    s.new_width = s.window_pos->cx;
    s.new_height = (s.window_pos->cy * 200) / 300;
    s.adjusted_width = (s.new_width * 300) / 200;
    s.keep_height_delta = abs(s.adjusted_width - s.window_pos->cy);
    s.keep_width_delta = abs(s.new_height - s.new_width);
    if (s.keep_height_delta + s.keep_width_delta < 5)
    {
      return 0;
    }
    if (s.new_height < s.new_width)
    {
      s.window_pos->cx = s.new_height;
    }
    else
    {
      s.window_pos->cy = s.adjusted_width;
    }
    return 0;

  case 0x30f:
  case 0x310:
  case 0x311:
    return FUN_10025b5e((int)hwnd, msg, (int)wparam, lparam);

  default:
    return DefWindowProcA(hwnd, msg, wparam, lparam);
  }
  return 0;
}
