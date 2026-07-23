#include <windows.h>
#include <stdio.h>
#include <string.h>

#include "cardartlib/src/palette.h"
#include "deckdll/src/shared_resources.h"
#include "drawcardlib/src/pic.h"
#include "magic/src/duel_engine.h"
#include "magic/src/game_support.h"
#include "magic/src/global_other.h"
#include "magic/src/global_state.h"
#include "magic/src/global_strings.h"
#include "magic/src/shared_startup.h"

extern int g_duel_cached_life_player_0;
extern int g_duel_cached_life_player_1;
extern int g_duel_cached_state_007abce0;
extern int g_duel_cached_state_007abce4;
extern int g_duel_cached_unk_008b44d0_player_0;
extern int g_duel_cached_unk_008b44d0_player_1;
extern int g_duel_modal_action_active;

BOOL DrawBitmapToRect(HDC dst_dc, const RECT *dst_rect, HBITMAP bitmap);
void delete_and_close_object(HANDLE obj);
int draw_masked_bitmap_left_half_to_rect(HDC dc, RECT *rect, HANDLE bitmap);
LRESULT handle_duel_inactive_cursor(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
int load_text_with_tab_escapes(char *filename, char *section_name);
void set_player_directive_value(int player, int value);
int is_face_directive_action_enabled(int player);
LRESULT CALLBACK wndproc_MAGICGAME_LifeClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
void post_life_directive_action(int player);
extern char global_base_directory[];

// GLOBAL: MAGIC 0x0055e148
int g_life_window_life_window_long_offset = 0;

// GLOBAL: MAGIC 0x0055e14c
int g_life_window_poison_window_long_offset = 4;

// GLOBAL: MAGIC 0x0055e150
int g_life_window_lich_window_long_offset = 8;

// GLOBAL: MAGIC 0x0055e154
int g_life_window_bitmap_window_long_offset = 0xc;

// GLOBAL: MAGIC 0x0055e158
int g_magicgame_life_window_extra_bytes = 0x10;

// GLOBAL: MAGIC 0x0057aa7c
char s_MENU_LIFE_0057aa7c[0xc] = "MENU_LIFE";

// GLOBAL: MAGIC 0x0057aa88
char s__s_Poison_pic_0057aa88[0x10] = "%s\\Poison.pic";

// GLOBAL: MAGIC 0x0057aa98
char s__s_LIFE_liched_pic_0057aa98[0x14] = "%s\\LIFE_liched.pic";

// GLOBAL: MAGIC 0x0057aaac
char s_Life_0057aaac[8] = "Life";

// GLOBAL: MAGIC 0x0057aab4
char s__duel_hlp_0057aab4[0xc] = "\\duel.hlp";

// GLOBAL: MAGIC 0x0057aac0
char s__d_0057aac0[4] = "%d";

// GLOBAL: MAGIC 0x0057aac4
char s__s_LIFE_liched_pic_0057aac4[0x14] = "%s\\LIFE_liched.pic";

// GLOBAL: MAGIC 0x0057aad8
char s__s_Poison_pic_0057aad8[0x10] = "%s\\Poison.pic";

// GLOBAL: MAGIC 0x0069e290
char g_life_menu_set_life_text[0x48];

// GLOBAL: MAGIC 0x0069e2c4
COLORREF g_life_text_color;

// GLOBAL: MAGIC 0x0069e2c8
COLORREF g_life_shadow_color;

// GLOBAL: MAGIC 0x0069e2cc
HFONT g_life_font;

// GLOBAL: MAGIC 0x0069e2d0
HBITMAP g_life_poison_bitmap;

// GLOBAL: MAGIC 0x0069e2d8
char g_life_menu_help_text[0x1c];

// GLOBAL: MAGIC 0x0069e2f4
HMENU g_life_popup_menu;

// GLOBAL: MAGIC 0x0069e2f8
char g_life_menu_directive_format[0x68];

// GLOBAL: MAGIC 0x0069e360
int g_life_directive_packet[3];

// GLOBAL: MAGIC 0x0069e370
char g_life_menu_directive_self_text[0x34];

// GLOBAL: MAGIC 0x0069e3a4
HBITMAP g_life_lich_bitmap;

// FUNCTION: MAGIC 0x00449bbf
int is_invalid_duel_player(int player)
{
  if (player != 0 && player != 1)
  {
    return 1;
  }

  return 0;
}

// FUNCTION: MAGIC 0x00449c4c
int get_displayed_life_total(int player)
{
  int result;

  if (is_invalid_duel_player(player) != 0)
  {
    return 0;
  }

  EnterCriticalSection(&g_duel_render_lock);
  if (player == 0)
  {
    result = g_duel_cached_life_player_0;
  }
  else
  {
    result = g_duel_cached_life_player_1;
  }
  LeaveCriticalSection(&g_duel_render_lock);

  return result;
}

// FUNCTION: MAGIC 0x00449cb2
int get_displayed_poison_total(int player)
{
  int result;

  if (is_invalid_duel_player(player) != 0)
  {
    return 0;
  }

  EnterCriticalSection(&g_duel_render_lock);
  if (player == 0)
  {
    result = g_duel_cached_state_007abce0;
  }
  else
  {
    result = g_duel_cached_state_007abce4;
  }
  LeaveCriticalSection(&g_duel_render_lock);

  return result;
}

// FUNCTION: MAGIC 0x00449d18
int get_displayed_lich_state(int player)
{
  int result;

  if (is_invalid_duel_player(player) != 0)
  {
    return 0;
  }

  EnterCriticalSection(&g_duel_render_lock);
  if (player == 0)
  {
    result = g_duel_cached_unk_008b44d0_player_0;
  }
  else
  {
    result = g_duel_cached_unk_008b44d0_player_1;
  }
  LeaveCriticalSection(&g_duel_render_lock);

  return result;
}

// FUNCTION: MAGIC 0x004e1e00
// FUNCTION: SHANDALAR 0x004ccc10
int register_MAGICGAME_LifeClass(LPCSTR class_name)
{
  struct
  {
    char path[264];
    int result;
    WNDCLASSA wndclass;
  } s;

  s.result = 1;
  s.wndclass.style = 0xb;
  s.wndclass.lpfnWndProc = wndproc_MAGICGAME_LifeClass;
  s.wndclass.cbClsExtra = 0;
  s.wndclass.cbWndExtra = g_magicgame_life_window_extra_bytes;
  s.wndclass.hInstance = g_app_instance;
  s.wndclass.hIcon = (HICON)0;
  s.wndclass.hCursor = LoadCursorA((HINSTANCE)0, (LPCSTR)0x7f00);
  s.wndclass.hbrBackground = (HBRUSH)0x6;
  s.wndclass.lpszMenuName = (LPCSTR)0;
  s.wndclass.lpszClassName = class_name;
  if (RegisterClassA(&s.wndclass) == 0)
  {
    s.result = 0;
  }

  g_life_popup_menu = CreatePopupMenu();
  load_text_with_tab_escapes(global_ui_strings_filename, s_MENU_LIFE_0057aa7c);
  strcpy(g_life_menu_directive_format, text_lines[0]);
  strcpy(g_life_menu_directive_self_text, text_lines[1]);
  strcpy(g_life_menu_set_life_text, text_lines[2]);
  strcpy(g_life_menu_help_text, text_lines[3]);

  sprintf(s.path, s__s_Poison_pic_0057aa88, global_cardart_path);
  g_life_poison_bitmap = load_pic(s.path);
  sprintf(s.path, s__s_LIFE_liched_pic_0057aa98, global_duelart_path);
  g_life_lich_bitmap = load_pic(s.path);

  g_life_font = CreateFontIndirectA(LoadFontFromIni(s_Life_0057aaac, 0));
  g_life_text_color = 0x100004a;
  g_life_shadow_color = 0x10000c9;
  return s.result;
}

// FUNCTION: MAGIC 0x004e1f8e
// FUNCTION: SHANDALAR 0x004ccd9e
void destroy_MAGICGAME_LifeClass(LPCSTR class_name)
{
  if (g_life_popup_menu != (HMENU)0)
  {
    DestroyMenu(g_life_popup_menu);
  }
  g_life_popup_menu = (HMENU)0;

  if (g_life_poison_bitmap != (HBITMAP)0)
  {
    delete_and_close_object(g_life_poison_bitmap);
  }
  g_life_poison_bitmap = (HBITMAP)0;

  if (g_life_lich_bitmap != (HBITMAP)0)
  {
    delete_and_close_object(g_life_lich_bitmap);
  }
  g_life_lich_bitmap = (HBITMAP)0;

  if (g_life_font != (HFONT)0)
  {
    DeleteObject(g_life_font);
  }
  g_life_font = (HFONT)0;
}

// FUNCTION: MAGIC 0x004e2029
// FUNCTION: SHANDALAR 0x004cce39
LRESULT CALLBACK wndproc_MAGICGAME_LifeClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  struct
  {
    int menu_item_count;
    char directive_text[100];
    unsigned int menu_player;
    char opponent_name[100];
    POINT menu_point;
    RECT popup_rect;
    char poison_path[264];
    char lich_path[264];
    HDC dc;
    int poison_height;
    PAINTSTRUCT paint;
    int poison_width;
    int saved_dc;
    int poison_y;
    int poison_index;
    int poison_total;
    int poison_x;
    int life_total;
    char life_text[12];
    RECT poison_rect;
    int lich_state;
    RECT client_rect;
    MSG peek_msg;
    unsigned int click_player;
    char help_path[264];
    DWORD help_context;
    unsigned int command_player;
    int updated_poison_total;
    int updated_life_total;
    int updated_lich_state;
    char cuecard_text[100];
    char cuecard_opponent_name[100];
    int cached_poison_total;
    HBITMAP background_bitmap;
    int cached_life_total;
    int cached_lich_state;
  } s;

  switch (msg)
  {
  case 0x437:
    s.cached_poison_total = GetWindowLongA(hwnd, g_life_window_poison_window_long_offset);
    s.cached_lich_state = GetWindowLongA(hwnd, g_life_window_lich_window_long_offset);
    if (hwnd == g_duel_life_status_window_1_hwnd)
    {
      if (s.cached_lich_state != 0)
      {
        if (s.cached_poison_total != 0)
        {
          strcpy(s.cuecard_text, gs_cuecard_your_life_points_and_poison_lich_008cf4f0);
        }
        else
        {
          strcpy(s.cuecard_text, gs_cuecard_your_life_points_lich_007a7810);
        }
      }
      else if (s.cached_poison_total != 0)
      {
        strcpy(s.cuecard_text, gs_cuecard_your_life_points_and_poison_008a9ba0);
      }
      else
      {
        strcpy(s.cuecard_text, gs_cuecard_your_life_points_009394a0);
      }
    }
    else
    {
      copy_opponent_name_prefix(s.cuecard_opponent_name);
      if (s.cached_lich_state != 0)
      {
        if (s.cached_poison_total != 0)
        {
          sprintf(s.cuecard_text, gs_cuecard_opponent_life_points_and_poison_lich_008b2890, s.cuecard_opponent_name);
        }
        else
        {
          sprintf(s.cuecard_text, gs_cuecard_opponent_life_points_lich_0091d1b0, s.cuecard_opponent_name);
        }
      }
      else if (s.cached_poison_total != 0)
      {
        sprintf(s.cuecard_text, gs_cuecard_opponent_life_points_and_poison_009252f0, s.cuecard_opponent_name);
      }
      else
      {
        sprintf(s.cuecard_text, gs_cuecard_opponent_life_points_008b30a0, s.cuecard_opponent_name);
      }
    }
    strcpy((char *)wparam, s.cuecard_text);
    return 1;

  case 0x432:
    s.cached_life_total = GetWindowLongA(hwnd, g_life_window_life_window_long_offset);
    if (hwnd == g_duel_life_status_window_1_hwnd)
    {
      s.updated_life_total = get_displayed_life_total(0);
    }
    else
    {
      s.updated_life_total = get_displayed_life_total(1);
    }

    s.cached_poison_total = GetWindowLongA(hwnd, g_life_window_poison_window_long_offset);
    if (hwnd == g_duel_life_status_window_1_hwnd)
    {
      s.updated_poison_total = get_displayed_poison_total(0);
    }
    else
    {
      s.updated_poison_total = get_displayed_poison_total(1);
    }

    s.cached_lich_state = GetWindowLongA(hwnd, g_life_window_lich_window_long_offset);
    if (hwnd == g_duel_life_status_window_1_hwnd)
    {
      s.updated_lich_state = get_displayed_lich_state(0);
    }
    else
    {
      s.updated_lich_state = get_displayed_lich_state(1);
    }

    if (s.cached_life_total != s.updated_life_total)
    {
      InvalidateRect(hwnd, (RECT *)0, 0);
    }
    if (s.cached_poison_total != s.updated_poison_total)
    {
      InvalidateRect(hwnd, (RECT *)0, 0);
    }
    if (s.cached_lich_state != s.updated_lich_state)
    {
      InvalidateRect(hwnd, (RECT *)0, 0);
    }
    return 0;

  case 0x439:
    s.background_bitmap = (HBITMAP)GetWindowLongA(hwnd, g_life_window_bitmap_window_long_offset);
    if (s.background_bitmap != (HBITMAP)0)
    {
      DeleteObject(s.background_bitmap);
    }
    s.background_bitmap = (HBITMAP)wparam;
    SetWindowLongA(hwnd, g_life_window_bitmap_window_long_offset, s.background_bitmap);
    InvalidateRect(hwnd, (RECT *)0, 1);
    return 0;

  case 0x438:
    return s.background_bitmap = GetWindowLongA(hwnd, g_life_window_bitmap_window_long_offset);

  case WM_COMMAND:
    switch ((unsigned int)wparam & 0xffff)
    {
    case 0x66:
      s.command_player = hwnd == g_duel_life_status_window_1_hwnd ? 0 : 1;
      unk_00715fb0 = 0;
      post_life_directive_action(s.command_player);
      break;

    case 100:
      set_player_directive_value(hwnd != g_duel_life_status_window_1_hwnd, 1);
      break;

    case 0x65:
      s.cached_lich_state = GetWindowLongA(hwnd, g_life_window_lich_window_long_offset);
      if (s.cached_lich_state != 0)
      {
        s.help_context = 0xbe1;
      }
      else
      {
        s.help_context = 0x7e8;
      }
      strcpy(s.help_path, global_base_directory);
      strcat(s.help_path, s__duel_hlp_0057aab4);
      WinHelpA(g_duel_window_hwnd, s.help_path, HELP_CONTEXT, s.help_context);
      break;
    default:
      break;
    }
    return 0;

  case WM_CREATE:
    s.cached_life_total = 0;
    SetWindowLongA(hwnd, g_life_window_life_window_long_offset, s.cached_life_total);
    s.cached_poison_total = 0;
    SetWindowLongA(hwnd, g_life_window_poison_window_long_offset, s.cached_poison_total);
    s.cached_lich_state = 0;
    SetWindowLongA(hwnd, g_life_window_lich_window_long_offset, s.cached_lich_state);
    s.background_bitmap = (HBITMAP)0;
    SetWindowLongA(hwnd, g_life_window_bitmap_window_long_offset, s.background_bitmap);
    return 0;

  case WM_DESTROY:
    s.background_bitmap = (HBITMAP)GetWindowLongA(hwnd, g_life_window_bitmap_window_long_offset);
    if (s.background_bitmap != (HBITMAP)0)
    {
      delete_and_close_object(s.background_bitmap);
    }
    return 0;

  case WM_ERASEBKGND:
    return 1;

  case WM_LBUTTONDOWN:
    s.click_player = hwnd == g_duel_life_status_window_1_hwnd ? 0 : 1;
    if (g_duel_modal_action_active != 0)
    {
      Sleep(GetDoubleClickTime());
      unk_00715fb0 = PeekMessageA(&s.peek_msg, hwnd, WM_LBUTTONDBLCLK, WM_LBUTTONDBLCLK, 0);
      post_life_directive_action(s.click_player);
    }
    return 0;

  case WM_PAINT:
    s.cached_life_total = GetWindowLongA(hwnd, g_life_window_life_window_long_offset);
    s.cached_poison_total = GetWindowLongA(hwnd, g_life_window_poison_window_long_offset);
    s.cached_lich_state = GetWindowLongA(hwnd, g_life_window_lich_window_long_offset);
    if (hwnd == g_duel_life_status_window_1_hwnd)
    {
      s.life_total = get_displayed_life_total(0);
    }
    else
    {
      s.life_total = get_displayed_life_total(1);
    }
    wsprintfA(s.life_text, s__d_0057aac0, s.life_total);
    if (hwnd == g_duel_life_status_window_1_hwnd)
    {
      s.poison_total = get_displayed_poison_total(0);
    }
    else
    {
      s.poison_total = get_displayed_poison_total(1);
    }
    if (hwnd == g_duel_life_status_window_1_hwnd)
    {
      s.lich_state = get_displayed_lich_state(0);
    }
    else
    {
      s.lich_state = get_displayed_lich_state(1);
    }
    if (s.cached_life_total != s.life_total)
    {
      InvalidateRect(hwnd, (RECT *)0, 0);
    }
    if (s.cached_poison_total != s.poison_total)
    {
      InvalidateRect(hwnd, (RECT *)0, 0);
    }
    if (s.cached_lich_state != s.lich_state)
    {
      InvalidateRect(hwnd, (RECT *)0, 0);
    }

    GetClientRect(hwnd, &s.client_rect);
    EnterCriticalSection(&g_card_render_lock);
    s.dc = g_shared_offscreen_dc;
    s.saved_dc = SaveDC(g_shared_offscreen_dc);
    s.background_bitmap = (HBITMAP)GetWindowLongA(hwnd, g_life_window_bitmap_window_long_offset);
    DrawBitmapToRect(s.dc, &s.client_rect, s.background_bitmap);

    if (s.lich_state != 0)
    {
      if (g_life_lich_bitmap == (HBITMAP)0)
      {
        sprintf(s.lich_path, s__s_LIFE_liched_pic_0057aac4, global_duelart_path);
        g_life_lich_bitmap = load_pic(s.lich_path);
      }
      DrawBitmapToRect(s.dc, &s.client_rect, g_life_lich_bitmap);
    }

    if (g_life_poison_bitmap == (HBITMAP)0)
    {
      sprintf(s.poison_path, s__s_Poison_pic_0057aad8, global_cardart_path);
      g_life_poison_bitmap = load_pic(s.poison_path);
    }

    s.poison_width = s.client_rect.right / 4;
    s.poison_height = s.client_rect.bottom / 3;
    s.poison_x = 0;
    s.poison_y = 0;
    for (s.poison_index = 0; s.poison_index < s.poison_total; s.poison_index++)
    {
      SetRect(&s.poison_rect, s.poison_x, s.poison_y, s.poison_x + s.poison_width, s.poison_y + s.poison_height);
      draw_masked_bitmap_left_half_to_rect(s.dc, &s.poison_rect, g_life_poison_bitmap);
      s.poison_x += s.poison_width;
      if (s.client_rect.right < s.poison_x + s.poison_width)
      {
        s.poison_x = 0;
        s.poison_y += s.poison_height;
      }
    }

    if (s.lich_state == 0)
    {
      SetMapMode(s.dc, MM_ANISOTROPIC);
      SetWindowExtEx(s.dc, 0x7d, 100, (LPSIZE)0);
      SetViewportExtEx(s.dc, s.client_rect.right - s.client_rect.left,
                       s.client_rect.bottom - s.client_rect.top, (LPSIZE)0);
      SelectObject(s.dc, g_life_font);
      SetBkMode(s.dc, TRANSPARENT);
      SetRect(&s.client_rect, 0, 0, 0x7d, 100);
      OffsetRect(&s.client_rect, 3, 3);
      SetTextColor(s.dc, g_life_shadow_color);
      DrawTextA(s.dc, s.life_text, -1, &s.client_rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
      OffsetRect(&s.client_rect, -3, -3);
      SetTextColor(s.dc, g_life_text_color);
      DrawTextA(s.dc, s.life_text, -1, &s.client_rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    }

    RestoreDC(g_shared_offscreen_dc, s.saved_dc);
    s.dc = BeginPaint(hwnd, &s.paint);
    if (s.dc != (HDC)0)
    {
      ApplyCardArtPaletteToDc(s.dc);
      GetClientRect(hwnd, &s.client_rect);
      if (g_duel_palette_refresh_pending != 0)
      {
        FillRect(s.dc, &s.client_rect, GetStockObject(WHITE_BRUSH));
        Sleep(200);
      }
      BitBlt(s.dc, 0, 0, s.client_rect.right, s.client_rect.bottom,
             g_shared_offscreen_dc, 0, 0, SRCCOPY);
      EndPaint(hwnd, &s.paint);
      s.cached_life_total = s.life_total;
      SetWindowLongA(hwnd, g_life_window_life_window_long_offset, s.cached_life_total);
      s.cached_poison_total = s.poison_total;
      SetWindowLongA(hwnd, g_life_window_poison_window_long_offset, s.cached_poison_total);
      s.cached_lich_state = s.lich_state;
      SetWindowLongA(hwnd, g_life_window_lich_window_long_offset, s.cached_lich_state);
    }
    LeaveCriticalSection(&g_card_render_lock);
    return 0;

  case WM_RBUTTONDOWN:
    s.menu_point.x = lparam & 0xffff;
    s.menu_point.y = HIWORD(lparam);
    ClientToScreen(hwnd, &s.menu_point);
    SetRect(&s.popup_rect, s.menu_point.x, s.menu_point.y, s.menu_point.x + 1, s.menu_point.y + 1);
    TrackPopupMenu(g_life_popup_menu, TPM_RIGHTBUTTON, s.menu_point.x, s.menu_point.y, 0, hwnd, &s.popup_rect);
    return 0;

  case WM_INITMENU:
    s.menu_player = hwnd == g_duel_life_status_window_1_hwnd ? 0 : 1;
    if (g_duel_modal_action_active != 0 && is_face_directive_action_enabled(s.menu_player) != 0)
    {
      if (s.menu_player == 1)
      {
        copy_opponent_name_prefix(s.opponent_name);
        sprintf(s.directive_text, g_life_menu_directive_format, s.opponent_name);
      }
      else
      {
        strcpy(s.directive_text, g_life_menu_directive_self_text);
      }
      AppendMenuA(g_life_popup_menu, 0, 0x66, s.directive_text);
    }
    if (GetMenuItemCount(g_life_popup_menu) > 0)
    {
      AppendMenuA(g_life_popup_menu, MF_SEPARATOR, 0, (LPCSTR)0);
    }
    AppendMenuA(g_life_popup_menu, 0, 100, g_life_menu_set_life_text);
    AppendMenuA(g_life_popup_menu, 0, 0x65, g_life_menu_help_text);
    return 0;

  case WM_INITMENUPOPUP:
    if (HIWORD(wparam) == 0xffff && lparam == 0)
    {
      s.menu_item_count = GetMenuItemCount(g_life_popup_menu);
      while (s.menu_item_count-- != 0)
      {
        DeleteMenu(g_life_popup_menu, 0, MF_BYPOSITION);
      }
    }
    return 0;

  case WM_SETCURSOR:
    return handle_duel_inactive_cursor(hwnd, msg, wparam, lparam);

  case 0x30f:
  case 0x310:
  case 0x311:
    return FUN_10025b5e((int)hwnd, msg, (int)wparam, lparam);
  default:
    return DefWindowProcA(hwnd, msg, wparam, lparam);
  }
  return 0;
}

// FUNCTION: MAGIC 0x004e2edd
void post_life_directive_action(int player)
{
  g_life_directive_packet[0] = 0;
  g_life_directive_packet[1] = player;
  g_life_directive_packet[2] = -1;
  PostMessageA(g_duel_window_hwnd, 0x464, 0, (LPARAM)g_life_directive_packet);
}
