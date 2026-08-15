#include <windows.h>
#include <stdio.h>
#include <string.h>

#include "cardartlib/src/palette.h"
#include "drawcardlib/Drawcardlib.h"
#include "magic/src/duel_engine.h"
#include "magic/src/game_support.h"
#include "magic/src/global_other.h"
#include "magic/src/global_state.h"
#include "magic/src/global_strings.h"
#include "magic/src/shared_startup.h"

#define DUEL_MAIN_WINDOW_HWND g_duel_window_hwnd

#define CLASS_SHUFFLE_CARD "ShuffleCard"

extern int DAT_008cdac0[500];
extern int DAT_0091b400[500];
extern int g_duel_cached_library_count_player_0;
extern int g_duel_cached_library_count_player_1;
extern char global_base_directory[];
extern int is_invalid_duel_player(int player);
extern LRESULT handle_duel_inactive_cursor(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

// GLOBAL: MAGIC 0x0055e0b8
// GLOBAL: SHANDALAR 0x0057f098
int g_library_window_long_offset = 0;

// GLOBAL: MAGIC 0x0055e0bc
// GLOBAL: SHANDALAR 0x0057f09c
int g_library_window_extra_bytes = 4;

// GLOBAL: MAGIC 0x00638ac8
// GLOBAL: SHANDALAR 0x005aa630
char g_library_menu_help_text[0x1c];

// GLOBAL: MAGIC 0x00638ae4
// GLOBAL: SHANDALAR 0x005aa64c
HMENU g_library_popup_menu;

// GLOBAL: MAGIC 0x00638ae8
// GLOBAL: SHANDALAR 0x005aa650
HMENU g_library_submenu;

// GLOBAL: MAGIC 0x00638af0
// GLOBAL: SHANDALAR 0x005aa658
char g_library_menu_count_text[0x20];

// FUNCTION: MAGIC 0x00449feb
// FUNCTION: SHANDALAR 0x00453b6c
int copy_cached_library_cards_and_get_count(void *cards, int player)
{
  int result;

  if (cards == NULL)
  {
    return 0;
  }

  if (is_invalid_duel_player(player) != 0)
  {
    return 0;
  }

  EnterCriticalSection(&g_duel_render_lock);
  if (player == 0)
  {
    result = g_duel_cached_library_count_player_0;
  }
  else
  {
    result = g_duel_cached_library_count_player_1;
  }
  memcpy(cards, (player == 0) ? DAT_008cdac0 : DAT_0091b400, 2000);
  LeaveCriticalSection(&g_duel_render_lock);

  return result;
}

// FUNCTION: MAGIC 0x00498ca9
int get_cached_library_count(int player)
{
  char cards[2000];

  return copy_cached_library_cards_and_get_count(cards, player);
}

// FUNCTION: MAGIC 0x00497fd0
// FUNCTION: SHANDALAR 0x004c1830
int register_MAGICGAME_LibraryClass(LPCSTR class_name)
{
  struct
  {
    int result;
    WNDCLASSA wndclass;
  } s;

  s.result = 1;
  s.wndclass.style = 3;
  s.wndclass.lpfnWndProc = wndproc_MAGICGAME_LibraryClass;
  s.wndclass.cbClsExtra = 0;
  s.wndclass.cbWndExtra = g_library_window_extra_bytes;
  s.wndclass.hInstance = g_app_instance;
  s.wndclass.hIcon = LoadIconA((HINSTANCE)0, (LPCSTR)0x7f00);
  s.wndclass.hCursor = LoadCursorA((HINSTANCE)0, (LPCSTR)0x7f00);
  s.wndclass.hbrBackground = (HBRUSH)GetStockObject(4);
  s.wndclass.lpszMenuName = (LPCSTR)0;
  s.wndclass.lpszClassName = class_name;
  if (RegisterClassA(&s.wndclass) == 0)
    s.result = 0;

  s.wndclass.style = 0;
  s.wndclass.lpfnWndProc = wndproc_ShuffleCard;
  s.wndclass.cbClsExtra = 0;
  s.wndclass.cbWndExtra = 0;
  s.wndclass.hInstance = g_app_instance;
  s.wndclass.hIcon = LoadIconA((HINSTANCE)0, (LPCSTR)0x7f00);
  s.wndclass.hCursor = LoadCursorA((HINSTANCE)0, (LPCSTR)0x7f00);
  s.wndclass.hbrBackground = (HBRUSH)0;
  s.wndclass.lpszMenuName = (LPCSTR)0;
  s.wndclass.lpszClassName = CLASS_SHUFFLE_CARD;
  if (RegisterClassA(&s.wndclass) == 0)
    s.result = 0;

  g_library_popup_menu = CreatePopupMenu();
  g_library_submenu = CreatePopupMenu();
  AppendMenuA(g_library_submenu, 0, 0x65, "");
  load_text_with_tab_escapes(global_ui_strings_filename, "MENU_LIBRARY");
  strcpy(g_library_menu_count_text, g_text_lines[0]);
  strcpy(g_library_menu_help_text, g_text_lines[1]);
  return s.result;
}

// FUNCTION: MAGIC 0x00498193
// FUNCTION: SHANDALAR 0x004c19f3
LRESULT CALLBACK wndproc_MAGICGAME_LibraryClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  struct
  {
    int unused;
    int menu_item_count;
    char menu_count_text[12];
    POINT menu_point;
    RECT menu_rect;
    char count_overlay_text[12];
    RECT stack_card_rect;
    int card_y_step;       // ebp - 0x41c
    int card_x_step;       // ebp - 0x418
    int card_width_delta;  // ebp - 0x414
    int stack_card_bottom; // ebp - 0x410
    RECT top_card_rect;
    int stack_card_right;
    int card_height_delta;
    int library_count;
    HDC paint_dc;
    PAINTSTRUCT paint_struct;
    RECT client_rect;
    char help_path[264];
    DWORD help_context;
    int shuffle_card_width_delta;
    int shuffle_card_height_delta;
    int shuffle_count;
    int shuffle_card_height;
    int shuffle_card_width;
    int shuffle_index;
    int shuffle_y;
    int shuffle_x;
    RECT shuffle_rect;
    HWND shuffle_windows[100];
    int cached_count;
    char cuecard_text[100];
    char opponent_name[100];
    int previous_count;
  } s;

  switch (msg)
  {
  case 0x437:
    if (DAT_0091ce30 == hwnd)
    {
      strcpy(s.cuecard_text, gs_cuecard_your_library_0091ca20);
    }
    else
    {
      copy_opponent_name_prefix(s.opponent_name);
      sprintf(s.cuecard_text, gs_cuecard_opponent_library_00789bd0, s.opponent_name);
    }
    strcpy((char *)wparam, s.cuecard_text);
    return 1;

  case 0x432:
    s.previous_count = GetWindowLongA(hwnd, g_library_window_long_offset);
    s.cached_count = get_cached_library_count(1 <= (unsigned int)((int)hwnd - (int)DAT_0091ce30));
    if (s.previous_count != s.cached_count)
    {
      InvalidateRect(hwnd, NULL, TRUE);
    }
    return 0;

  case 0x400:
    s.shuffle_count = get_cached_library_count(1 <= (unsigned int)((int)hwnd - (int)DAT_0091ce30));
    GetClientRect(hwnd, &s.shuffle_rect);
    s.shuffle_card_width_delta = (((s.shuffle_rect.right * 0x28) / 100) *
                                  (s.shuffle_count < 0x4b ? s.shuffle_count : 0x4b)) /
                                 0x4b;
    s.shuffle_card_height_delta = (s.shuffle_card_width_delta * s.shuffle_rect.bottom) / s.shuffle_rect.right;
    s.shuffle_card_width = s.shuffle_rect.right - s.shuffle_card_width_delta;
    s.shuffle_card_height = s.shuffle_rect.bottom - s.shuffle_card_height_delta;
    GetWindowRect(hwnd, &s.shuffle_rect);
    s.shuffle_x = s.shuffle_rect.left;
    s.shuffle_y = s.shuffle_rect.top;
    if (s.shuffle_count > 100)
    {
      s.shuffle_count = 100;
    }
    if (GetSystemMetrics(SM_SLOWMACHINE) != 0)
    {
      if (s.shuffle_count > 4)
      {
        s.shuffle_count = MAX(s.shuffle_count / 5, 4);
      }
    }
    else if (s.shuffle_count > 5)
    {
      s.shuffle_count = MAX(s.shuffle_count / 3, 5);
    }

    for (s.shuffle_index = 0; s.shuffle_count > s.shuffle_index; s.shuffle_index++)
    {
      s.shuffle_windows[s.shuffle_index] =
          CreateWindowExA(0, CLASS_SHUFFLE_CARD, "", 0x90000000, s.shuffle_x, s.shuffle_y,
                          s.shuffle_card_width, s.shuffle_card_height, DUEL_MAIN_WINDOW_HWND,
                          (HMENU)0, g_app_instance, (LPVOID)0);
      if (s.shuffle_windows[s.shuffle_index] != (HWND)0)
      {
        UpdateWindow(s.shuffle_windows[s.shuffle_index]);
        s.shuffle_x += (s.shuffle_card_width * 0xf) / 100;
      }
    }
    for (s.shuffle_index = s.shuffle_count - 1; s.shuffle_index >= 0; s.shuffle_index--)
    {
      if (s.shuffle_windows[s.shuffle_index] != (HWND)0)
      {
        DestroyWindow(s.shuffle_windows[s.shuffle_index]);
      }
      UpdateWindow(DUEL_MAIN_WINDOW_HWND);
    }
    return 0;

  case WM_COMMAND:
    switch (wparam & 0xffff)
    {
    case 100:
      s.help_context = 0x7e9;
      strcpy(s.help_path, global_base_directory);
      strcat(s.help_path, "\\duel.hlp");
      WinHelpA(DUEL_MAIN_WINDOW_HWND, s.help_path, HELP_CONTEXT, s.help_context);
      break;

    default:
      break;
    }
    return 0;

  case WM_CREATE:
    s.previous_count = 0;
    SetWindowLongA(hwnd, g_library_window_long_offset, s.previous_count);
    return 0;

  case WM_ERASEBKGND:
    return 1;

  case WM_LBUTTONDOWN:
    return 0;

  case WM_PAINT:
    s.previous_count = GetWindowLongA(hwnd, g_library_window_long_offset);
    s.library_count = get_cached_library_count(1 <= (unsigned int)((int)hwnd - (int)DAT_0091ce30));
    if (s.previous_count != s.library_count)
    {
      InvalidateRect(hwnd, NULL, FALSE);
    }
    EnterCriticalSection(&g_card_render_lock);
    s.paint_dc = BeginPaint(hwnd, &s.paint_struct);
    if (s.paint_dc != (HDC)0)
    {
      ApplyCardArtPaletteToDc(s.paint_dc);
      GetClientRect(hwnd, &s.client_rect);
      if (g_duel_palette_refresh_pending != 0)
      {
        FillRect(s.paint_dc, &s.client_rect, GetStockObject(WHITE_BRUSH));
        Sleep(200);
      }
      if (s.library_count == 0)
      {
        FillRect(s.paint_dc, &s.client_rect, GetStockObject(BLACK_BRUSH));
      }
      else if (s.library_count == 1)
      {
        DrawCardBack(s.paint_dc, &s.client_rect);
      }
      else
      {
        FillRect(g_shared_offscreen_dc, &s.client_rect, GetStockObject(BLACK_BRUSH));
        s.card_width_delta = (((s.client_rect.right * 0x28) / 100) *
                              (s.library_count < 0x4b ? s.library_count : 0x4b)) /
                             0x4b;
        s.card_height_delta = (s.card_width_delta * s.client_rect.bottom) / s.client_rect.right;
        SetRect(&s.top_card_rect, s.client_rect.left, s.client_rect.top,
                s.client_rect.right - s.card_width_delta, s.client_rect.bottom - s.card_height_delta);
        SelectObject(s.paint_dc, GetStockObject(GRAY_BRUSH));
        SelectObject(s.paint_dc, GetStockObject(BLACK_PEN));

        s.card_x_step = MAX(s.card_width_delta / MAX(s.library_count / 5, 1), 2);

        s.card_y_step = MAX(s.card_height_delta / MAX(s.library_count / 5, 1), 3);

        s.stack_card_right = s.client_rect.right;
        s.stack_card_bottom = s.client_rect.bottom;
        for (; s.stack_card_right > s.top_card_rect.right;
             s.stack_card_right -= s.card_x_step, s.stack_card_bottom -= s.card_y_step)
        {
          SetRect(&s.stack_card_rect, s.stack_card_right - (s.top_card_rect.right - s.top_card_rect.left),
                  s.stack_card_bottom - (s.top_card_rect.bottom - s.top_card_rect.top),
                  s.stack_card_right, s.stack_card_bottom);
          DrawCardBack(g_shared_offscreen_dc, &s.stack_card_rect);
        }
        DrawCardBack(g_shared_offscreen_dc, &s.top_card_rect);
        BitBlt(s.paint_dc, 0, 0, s.client_rect.right, s.client_rect.bottom,
               g_shared_offscreen_dc, 0, 0, SRCCOPY);
        if (g_duel_dialog_refresh_state != 0)
        {
          sprintf(s.count_overlay_text, "%d", s.library_count);
          SetBkMode(s.paint_dc, TRANSPARENT);
          SetTextColor(s.paint_dc, 0xffffff);
          TextOutA(s.paint_dc, 0, 0, s.count_overlay_text, strlen(s.count_overlay_text));
        }
      }
      EndPaint(hwnd, &s.paint_struct);
      s.previous_count = s.library_count;
      SetWindowLongA(hwnd, g_library_window_long_offset, s.previous_count);
    }
    LeaveCriticalSection(&g_card_render_lock);
    return 0;

  case WM_RBUTTONDOWN:
    s.menu_point.x = lparam & 0xffff;
    s.menu_point.y = HIWORD(lparam);
    ClientToScreen(hwnd, &s.menu_point);
    SetRect(&s.menu_rect, s.menu_point.x, s.menu_point.y, s.menu_point.x + 1, s.menu_point.y + 1);
    TrackPopupMenu(g_library_popup_menu, TPM_RIGHTBUTTON, s.menu_point.x, s.menu_point.y, 0, hwnd, &s.menu_rect);
    return 0;

  case WM_INITMENU:
    s.previous_count = GetWindowLongA(hwnd, g_library_window_long_offset);
    sprintf(s.menu_count_text, "%d", s.previous_count);
    AppendMenuA(g_library_popup_menu, MF_POPUP, (UINT)g_library_submenu, g_library_menu_count_text);
    ModifyMenuA(g_library_submenu, 0x65, MF_BYCOMMAND, 0x65, s.menu_count_text);
    AppendMenuA(g_library_popup_menu, MF_STRING, 100, g_library_menu_help_text);
    return 0;

  case WM_MENUSELECT:
    if (HIWORD(wparam) == 0xffff && lparam == 0)
    {
      s.menu_item_count = GetMenuItemCount(g_library_popup_menu);
      while (s.unused = s.menu_item_count--)
      {
        RemoveMenu(g_library_popup_menu, 0, MF_BYPOSITION);
      }
    }
    return 0;

  case WM_SETCURSOR:
    return handle_duel_inactive_cursor(hwnd, msg, wparam, lparam);

  case WM_QUERYNEWPALETTE:
  case WM_PALETTECHANGED:
  case WM_PALETTEISCHANGING:
    return handle_button_palette_message((int)hwnd, msg, (int)wparam, lparam);

  default:
    return DefWindowProcA(hwnd, msg, wparam, lparam);
  }

  return 0;
}
