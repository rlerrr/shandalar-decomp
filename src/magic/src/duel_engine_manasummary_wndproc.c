#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "cardartlib/src/palette.h"
#include "drawcardlib/src/pic.h"
#include "magic/src/duel_engine.h"
#include "magic/src/game_support.h"
#include "magic/src/global_other.h"
#include "magic/src/global_state.h"
#include "magic/src/global_strings.h"
#include "magic/src/shared_startup.h"

#define CLASS_MAGICGAME_MANA_SUMMARY "MAGICGAME_ManaSummaryClass"

#ifndef SHANDALAR
extern HWND global_main_hwnd;
#define DUEL_MAIN_WINDOW_HWND global_main_hwnd
#else
#define DUEL_MAIN_WINDOW_HWND g_main_window_hwnd
#endif

extern char global_base_directory[];
extern int g_duel_modal_action_active;
extern target_selection_request_t g_duel_action_request_copy;

typedef struct mana_summary_values_t
{
  int colorless;
  int black;
  int blue;
  int green;
  int red;
  int white;
  int artifact;
} mana_summary_values_t;

BOOL DrawBitmapToRect(HDC dst_dc, const RECT *dst_rect, HBITMAP bitmap);
void delete_and_close_object(HANDLE obj);
LRESULT handle_duel_inactive_cursor(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
int is_invalid_duel_player(int player);
int load_text_with_tab_escapes(char *filename, char *section_name);
void copy_opponent_name_prefix(char *name);
LOGFONTA *LoadFontFromIni(char *section, int must_load);
LRESULT CALLBACK wndproc_MAGICGAME_ManaSummaryClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
int get_displayed_mana_pool(mana_summary_values_t *mana_pool, int player);
void get_mana_pool_text_rect(LPRECT rect, HWND hwnd, int mana_color);

// GLOBAL: MAGIC 0x0055e1c4
// GLOBAL: SHANDALAR 0x0057f13c
int g_magicgame_mana_summary_window_extra_bytes = 4;

// GLOBAL: MAGIC 0x00708888
// GLOBAL: SHANDALAR 0x00652fc0
int g_mana_summary_click_packet[3];

// GLOBAL: MAGIC 0x00708898
// GLOBAL: SHANDALAR 0x00652fd0
char g_mana_summary_menu_blue_text[0x48];

// GLOBAL: MAGIC 0x007088d0
// GLOBAL: SHANDALAR 0x00653008
int g_mana_summary_menu_packet[3];

// GLOBAL: MAGIC 0x007088e0
// GLOBAL: SHANDALAR 0x00653018
char g_mana_summary_menu_colorless_text[0x38];

// GLOBAL: MAGIC 0x00708918
// GLOBAL: SHANDALAR 0x00653050
char g_mana_summary_menu_white_text[0x38];

// GLOBAL: MAGIC 0x00708950
// GLOBAL: SHANDALAR 0x00653088
char g_mana_summary_menu_red_text[0x38];

// GLOBAL: MAGIC 0x00708988
// GLOBAL: SHANDALAR 0x006530c0
char g_mana_summary_menu_help_text[0x1c];

// GLOBAL: MAGIC 0x007089a4
// GLOBAL: SHANDALAR 0x006530dc
HMENU g_mana_summary_popup_menu;

// GLOBAL: MAGIC 0x007089a8
// GLOBAL: SHANDALAR 0x006530e0
char g_mana_summary_menu_black_text[0x34];

// GLOBAL: MAGIC 0x007089dc
// GLOBAL: SHANDALAR 0x00653114
HFONT g_mana_summary_font;

// GLOBAL: MAGIC 0x007089e0
// GLOBAL: SHANDALAR 0x00653118
char g_mana_summary_menu_green_text[0x34];

// GLOBAL: MAGIC 0x00708a14
// GLOBAL: SHANDALAR 0x0065314c
HBITMAP g_mana_summary_background_bitmap;

// GLOBAL: MAGIC 0x00708a18
// GLOBAL: SHANDALAR 0x00653150
char g_mana_summary_menu_artifact_text[0x4c];

// FUNCTION: MAGIC 0x00539320
// FUNCTION: SHANDALAR 0x00550c00
int register_MAGICGAME_ManaSummaryClass(LPCSTR class_name)
{
  struct
  {
    char path[264];
    int result;
    WNDCLASSA wndclass;
  } s;

  s.result = 1;
  s.wndclass.style = 0xb;
  s.wndclass.lpfnWndProc = wndproc_MAGICGAME_ManaSummaryClass;
  s.wndclass.cbClsExtra = 0;
  s.wndclass.cbWndExtra = g_magicgame_mana_summary_window_extra_bytes;
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

  g_mana_summary_popup_menu = CreatePopupMenu();
  load_text_with_tab_escapes(global_ui_strings_filename, "MENU_MANAPOOL");
  strcpy(g_mana_summary_menu_black_text, text_lines[0]);
  strcpy(g_mana_summary_menu_blue_text, text_lines[1]);
  strcpy(g_mana_summary_menu_green_text, text_lines[2]);
  strcpy(g_mana_summary_menu_red_text, text_lines[3]);
  strcpy(g_mana_summary_menu_white_text, text_lines[4]);
  strcpy(g_mana_summary_menu_colorless_text, text_lines[5]);
  strcpy(g_mana_summary_menu_artifact_text, text_lines[6]);
  strcpy(g_mana_summary_menu_help_text, text_lines[7]);

  strcpy(s.path, global_duelart_path);
  strcat(s.path, "\\WINBK_ManaPool.pic");
  g_mana_summary_background_bitmap = load_pic(s.path);
  g_mana_summary_font = CreateFontIndirectA(LoadFontFromIni("ManaPool", 0));
  return s.result;
}

// FUNCTION: MAGIC 0x005394da
// FUNCTION: SHANDALAR 0x00550dba
void destroy_MAGICGAME_ManaSummaryClass(LPCSTR class_name)
{
  if (g_mana_summary_popup_menu != (HMENU)0)
  {
    DestroyMenu(g_mana_summary_popup_menu);
  }
  g_mana_summary_popup_menu = (HMENU)0;

  if (g_mana_summary_background_bitmap != (HBITMAP)0)
  {
    delete_and_close_object(g_mana_summary_background_bitmap);
  }
  g_mana_summary_background_bitmap = (HBITMAP)0;

  if (g_mana_summary_font != (HFONT)0)
  {
    DeleteObject(g_mana_summary_font);
  }
  g_mana_summary_font = (HFONT)0;
}

// FUNCTION: MAGIC 0x00539550
// FUNCTION: SHANDALAR 0x00550e30
LRESULT CALLBACK wndproc_MAGICGAME_ManaSummaryClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  struct
  {
    int menu_item_count;
    POINT cursor_point;
    char menu_text[100];
    RECT menu_hit_rect;
    int menu_color;
    int menu_index;
    POINT menu_point;
    RECT popup_rect;
    mana_summary_values_t paint_mana_pool;
    char background_path[264];
    char amount_text[8];
    int text_x_offset; // ebp - 0x31c
    COLORREF mana_text_colors[7];
    HDC dc;
    PAINTSTRUCT paint;
    int saved_dc;
    int color_index;
    RECT text_rect;
    RECT client_rect;
    COLORREF shadow_color;
    int clicked_mana_pool[7];
    unsigned int click_x;
    unsigned int click_y;
    MSG peek_msg;
    RECT click_rect;
    int peek_result;
    int unused_row_height; // ebp - 0x238
    int click_color;       // ebp - 0x234
    int unused_row_bottom; // ebp - 0x230
    RECT click_client_rect;
    unsigned int command_id;
    char help_path[264];
    DWORD help_context;
    mana_summary_values_t updated_mana_pool;
    char cuecard_text[100];
    int hit_color;
    unsigned int cuecard_x;
    unsigned int cuecard_y;
    int has_cuecard;
    unsigned int cuecard_player;
    RECT cuecard_rect;
    char opponent_name[100];
    mana_summary_values_t *cached_mana_pool;
  } s;

  switch (msg)
  {
  case 0x437:
    s.cuecard_x = lparam & 0xffff;
    s.cuecard_y = HIWORD(lparam);
    s.hit_color = -2;
    get_mana_pool_text_rect(&s.cuecard_rect, hwnd, 1);
    if (PtInRect(&s.cuecard_rect, *(POINT *)&s.cuecard_x) != 0)
    {
      s.hit_color = 1;
    }
    get_mana_pool_text_rect(&s.cuecard_rect, hwnd, 5);
    if (PtInRect(&s.cuecard_rect, *(POINT *)&s.cuecard_x) != 0)
    {
      s.hit_color = 5;
    }
    get_mana_pool_text_rect(&s.cuecard_rect, hwnd, 2);
    if (PtInRect(&s.cuecard_rect, *(POINT *)&s.cuecard_x) != 0)
    {
      s.hit_color = 2;
    }
    get_mana_pool_text_rect(&s.cuecard_rect, hwnd, 3);
    if (PtInRect(&s.cuecard_rect, *(POINT *)&s.cuecard_x) != 0)
    {
      s.hit_color = 3;
    }
    get_mana_pool_text_rect(&s.cuecard_rect, hwnd, 4);
    if (PtInRect(&s.cuecard_rect, *(POINT *)&s.cuecard_x) != 0)
    {
      s.hit_color = 4;
    }
    get_mana_pool_text_rect(&s.cuecard_rect, hwnd, 0);
    if (PtInRect(&s.cuecard_rect, *(POINT *)&s.cuecard_x) != 0)
    {
      s.hit_color = 0;
    }
    get_mana_pool_text_rect(&s.cuecard_rect, hwnd, 6);
    if (PtInRect(&s.cuecard_rect, *(POINT *)&s.cuecard_x) != 0)
    {
      s.hit_color = 6;
    }
    s.cuecard_player = unk_008ce534 == hwnd ? 0 : 1;
    if (s.hit_color == 0 || s.hit_color == 1 || s.hit_color == 5 ||
        s.hit_color == 3 || s.hit_color == 4 || s.hit_color == 2 || s.hit_color == 6)
    {
      if (s.cuecard_player == 0)
      {
        strcpy(s.cuecard_text, gs_cuecard_your_mana_pool_008cc2e0[s.hit_color]);
      }
      else
      {
        copy_opponent_name_prefix(s.opponent_name);
        sprintf(s.cuecard_text, gs_cuecard_opponent_mana_pool_008cff20[s.hit_color], s.opponent_name);
      }
      s.has_cuecard = 1;
    }
    else
    {
      s.has_cuecard = 0;
    }
    if (s.has_cuecard != 0)
    {
      strcpy((char *)wparam, s.cuecard_text);
      return s.has_cuecard;
    }
    break;

  case 0x432:
    s.cached_mana_pool = (mana_summary_values_t *)GetWindowLongA(hwnd, 0);
    get_displayed_mana_pool(&s.updated_mana_pool, (unsigned int)(unk_008ce534 != hwnd));
    if (s.cached_mana_pool->colorless != s.updated_mana_pool.colorless ||
        s.cached_mana_pool->black != s.updated_mana_pool.black ||
        s.cached_mana_pool->blue != s.updated_mana_pool.blue ||
        s.cached_mana_pool->red != s.updated_mana_pool.red ||
        s.cached_mana_pool->green != s.updated_mana_pool.green ||
        s.cached_mana_pool->white != s.updated_mana_pool.white ||
        s.cached_mana_pool->artifact != s.updated_mana_pool.artifact)
    {
      InvalidateRect(hwnd, (RECT *)0, 0);
    }
    return 0;

  case WM_COMMAND:
    switch ((unsigned int)wparam & 0xffff)
    {
    case 100:
      s.help_context = 0x7ea;
      strcpy(s.help_path, global_base_directory);
      strcat(s.help_path, "\\duel.hlp");
      WinHelpA(g_duel_window_hwnd, s.help_path, HELP_CONTEXT, s.help_context);
      break;

    default:
      s.command_id = (unsigned int)wparam & 0xffff;
      if (100 < s.command_id)
      {
        DAT_00715fa4 = (int)(unk_008ce534 != hwnd);
        DAT_0072c8e0 = s.command_id - 0x65;
        unk_00715fb0 = 0;
        g_mana_summary_menu_packet[0] = -3;
        g_mana_summary_menu_packet[1] = -1;
        g_mana_summary_menu_packet[2] = -1;
        PostMessageA(DUEL_MAIN_WINDOW_HWND, 0x464, 0, (LPARAM)g_mana_summary_menu_packet);
      }
      break;
    }
    return 0;

  case WM_CREATE:
    s.cached_mana_pool = malloc(sizeof(mana_summary_values_t));
    s.cached_mana_pool->artifact = 0;
    s.cached_mana_pool->white = s.cached_mana_pool->artifact;
    s.cached_mana_pool->green = s.cached_mana_pool->white;
    s.cached_mana_pool->red = s.cached_mana_pool->green;
    s.cached_mana_pool->blue = s.cached_mana_pool->red;
    s.cached_mana_pool->black = s.cached_mana_pool->blue;
    s.cached_mana_pool->colorless = s.cached_mana_pool->black;
    SetWindowLongA(hwnd, 0, (LONG)s.cached_mana_pool);
    if (s.cached_mana_pool == (mana_summary_values_t *)0)
    {
      return -1;
    }
    return 0;

  case WM_DESTROY:
    s.cached_mana_pool = (mana_summary_values_t *)GetWindowLongA(hwnd, 0);
    free(s.cached_mana_pool);
    return 0;

  case WM_ERASEBKGND:
    return 1;

  case WM_LBUTTONDOWN:
    if (g_duel_modal_action_active != 0)
    {
      Sleep(GetDoubleClickTime());
      s.peek_result = PeekMessageA(&s.peek_msg, hwnd, WM_LBUTTONDBLCLK, WM_LBUTTONDBLCLK, 0);
      get_displayed_mana_pool((mana_summary_values_t *)s.clicked_mana_pool, (unsigned int)(unk_008ce534 != hwnd));
      GetClientRect(hwnd, &s.click_client_rect);
      s.click_x = lparam & 0xffff;
      s.click_y = HIWORD(lparam);
      s.unused_row_height = s.unused_row_bottom = s.click_client_rect.bottom / 6;
      DAT_00715fa4 = unk_008ce534 == hwnd ? 0 : 1;
      DAT_0072c8e0 = -1;
      for (s.click_color = 0; s.click_color < 7; s.click_color++)
      {
        get_mana_pool_text_rect(&s.click_rect, hwnd, s.click_color);
        if (PtInRect(&s.click_rect, *(POINT *)&s.click_x) != 0 && 0 < s.clicked_mana_pool[s.click_color])
        {
          DAT_0072c8e0 = s.click_color;
        }
      }
      if (DAT_0072c8e0 != -1 && g_duel_modal_action_active != 0 &&
          (g_duel_action_request_copy.player == -1 || g_duel_action_request_copy.player == DAT_00715fa4) &&
          (g_duel_action_request_copy.zone_flags == -1 || (g_duel_action_request_copy.zone_flags & 1) != 0))
      {
        unk_00715fb0 = s.peek_result;
        g_mana_summary_click_packet[0] = -3;
        g_mana_summary_click_packet[1] = -1;
        g_mana_summary_click_packet[2] = -1;
        PostMessageA(g_duel_window_hwnd, 0x464, 0, (LPARAM)g_mana_summary_click_packet);
      }
    }
    return 0;

  case WM_PAINT:
    s.cached_mana_pool = (mana_summary_values_t *)GetWindowLongA(hwnd, 0);
    get_displayed_mana_pool(&s.paint_mana_pool, (unsigned int)(unk_008ce534 != hwnd));
    if (s.cached_mana_pool->colorless != s.paint_mana_pool.colorless ||
        s.cached_mana_pool->black != s.paint_mana_pool.black ||
        s.cached_mana_pool->blue != s.paint_mana_pool.blue ||
        s.cached_mana_pool->red != s.paint_mana_pool.red ||
        s.cached_mana_pool->green != s.paint_mana_pool.green ||
        s.cached_mana_pool->white != s.paint_mana_pool.white ||
        s.cached_mana_pool->artifact != s.paint_mana_pool.artifact)
    {
      InvalidateRect(hwnd, (RECT *)0, 0);
    }

    EnterCriticalSection(&g_card_render_lock);
    GetClientRect(hwnd, &s.client_rect);
    s.dc = g_shared_offscreen_dc;
    s.saved_dc = SaveDC(g_shared_offscreen_dc);
    if (g_mana_summary_background_bitmap == (HBITMAP)0)
    {
      strcpy(s.background_path, global_duelart_path);
      strcat(s.background_path, "\\WINBK_ManaPool.pic");
      g_mana_summary_background_bitmap = load_pic(s.background_path);
    }
    if (g_mana_summary_background_bitmap != (HBITMAP)0)
    {
      DrawBitmapToRect(s.dc, &s.client_rect, g_mana_summary_background_bitmap);
    }
    else
    {
      FillRect(s.dc, &s.client_rect, GetStockObject(LTGRAY_BRUSH));
    }
    SelectObject(s.dc, g_mana_summary_font);
    SetBkMode(s.dc, TRANSPARENT);
    SetTextAlign(s.dc, TA_CENTER);
    s.text_x_offset = (s.client_rect.right * 0x28) / 100;
    SetMapMode(s.dc, MM_ANISOTROPIC);
    get_mana_pool_text_rect(&s.text_rect, hwnd, 1);
    SetWindowExtEx(s.dc, s.text_rect.right - s.text_rect.left, 0x28, (LPSIZE)0);
    SetViewportExtEx(s.dc, s.text_rect.right - s.text_rect.left,
                     s.text_rect.bottom - s.text_rect.top, (LPSIZE)0);
    s.shadow_color = 0x10000c9;
    s.mana_text_colors[1] = 0x10000c8;
    s.mana_text_colors[2] = 0x100005d;
    s.mana_text_colors[3] = 0x1000026;
    s.mana_text_colors[4] = 0x100001e;
    s.mana_text_colors[5] = 0x10000bf;
    s.mana_text_colors[0] = 0x10000c5;
    s.mana_text_colors[6] = 0x10000d0;

    for (s.color_index = 0; s.color_index < 7; s.color_index++)
    {
      wsprintfA(s.amount_text, "%d", ((int *)&s.paint_mana_pool)[s.color_index]);
      get_mana_pool_text_rect(&s.text_rect, hwnd, s.color_index);
      DPtoLP(s.dc, (LPPOINT)&s.text_rect, 2);
      if (s.color_index != 6)
      {
        s.text_rect.left += s.text_x_offset;
      }
      if (s.color_index == 6 && IsRectEmpty(&s.text_rect))
      {
      }
      else
      {
        SetTextColor(s.dc, s.shadow_color);
        TextOutA(s.dc, s.text_rect.left + 1, s.text_rect.top + 1, s.amount_text, lstrlenA(s.amount_text));
        SetTextColor(s.dc, s.mana_text_colors[s.color_index]);
        TextOutA(s.dc, s.text_rect.left, s.text_rect.top, s.amount_text, lstrlenA(s.amount_text));
      }
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
      s.cached_mana_pool->colorless = s.paint_mana_pool.colorless;
      s.cached_mana_pool->black = s.paint_mana_pool.black;
      s.cached_mana_pool->blue = s.paint_mana_pool.blue;
      s.cached_mana_pool->red = s.paint_mana_pool.red;
      s.cached_mana_pool->green = s.paint_mana_pool.green;
      s.cached_mana_pool->white = s.paint_mana_pool.white;
      s.cached_mana_pool->artifact = s.paint_mana_pool.artifact;
    }
    LeaveCriticalSection(&g_card_render_lock);
    return 0;

  case WM_RBUTTONDOWN:
    s.menu_point.x = lparam & 0xffff;
    s.menu_point.y = HIWORD(lparam);
    ClientToScreen(hwnd, &s.menu_point);
    SetRect(&s.popup_rect, s.menu_point.x, s.menu_point.y, s.menu_point.x + 1, s.menu_point.y + 1);
    TrackPopupMenu(g_mana_summary_popup_menu, TPM_RIGHTBUTTON, s.menu_point.x, s.menu_point.y, 0, hwnd, &s.popup_rect);
    return 0;

  case WM_SETCURSOR:
    return handle_duel_inactive_cursor(hwnd, WM_SETCURSOR, wparam, lparam);

  case WM_INITMENU:
    s.has_cuecard = 0;
    for (s.menu_index = 0; s.menu_index < 7; s.menu_index++)
    {
      if (mana_charge[s.menu_index] != 0)
      {
        s.has_cuecard = 1;
      }
    }
    if (g_duel_modal_action_active != 0 && s.has_cuecard &&
        (g_duel_action_request_copy.player == -1 || g_duel_action_request_copy.player == DAT_00715fa4) &&
        (g_duel_action_request_copy.type_flags == -1 ||
         g_duel_action_request_copy.type_flags == 0 ||
         g_duel_action_request_copy.type_flags == 1) &&
        (g_duel_action_request_copy.color_flags == -1 ||
         g_duel_action_request_copy.color_flags == 0 ||
         g_duel_action_request_copy.color_flags == DAT_0072c8e0) &&
        (g_duel_action_request_copy.owner == -1 || g_duel_action_request_copy.owner == DAT_00715fa4) &&
        (g_duel_action_request_copy.zone_flags == -1 || (g_duel_action_request_copy.zone_flags & 1) != 0))
    {
      GetCursorPos(&s.cursor_point);
      MapWindowPoints((HWND)0, hwnd, &s.cursor_point, 1);
      s.menu_color = -1;
      get_mana_pool_text_rect(&s.menu_hit_rect, hwnd, 1);
      if (PtInRect(&s.menu_hit_rect, s.cursor_point) != 0)
      {
        s.menu_color = 1;
        strcpy(s.menu_text, g_mana_summary_menu_black_text);
      }
      get_mana_pool_text_rect(&s.menu_hit_rect, hwnd, 5);
      if (PtInRect(&s.menu_hit_rect, s.cursor_point) != 0)
      {
        s.menu_color = 5;
        strcpy(s.menu_text, g_mana_summary_menu_white_text);
      }
      get_mana_pool_text_rect(&s.menu_hit_rect, hwnd, 2);
      if (PtInRect(&s.menu_hit_rect, s.cursor_point) != 0)
      {
        s.menu_color = 2;
        strcpy(s.menu_text, g_mana_summary_menu_blue_text);
      }
      get_mana_pool_text_rect(&s.menu_hit_rect, hwnd, 3);
      if (PtInRect(&s.menu_hit_rect, s.cursor_point) != 0)
      {
        s.menu_color = 3;
        strcpy(s.menu_text, g_mana_summary_menu_green_text);
      }
      get_mana_pool_text_rect(&s.menu_hit_rect, hwnd, 4);
      if (PtInRect(&s.menu_hit_rect, s.cursor_point) != 0)
      {
        s.menu_color = 4;
        strcpy(s.menu_text, g_mana_summary_menu_red_text);
      }
      get_mana_pool_text_rect(&s.menu_hit_rect, hwnd, 0);
      if (PtInRect(&s.menu_hit_rect, s.cursor_point) != 0)
      {
        s.menu_color = 0;
        strcpy(s.menu_text, g_mana_summary_menu_colorless_text);
      }
      get_mana_pool_text_rect(&s.menu_hit_rect, hwnd, 6);
      if (PtInRect(&s.menu_hit_rect, s.cursor_point) != 0)
      {
        s.menu_color = 0;
        strcpy(s.menu_text, g_mana_summary_menu_artifact_text);
      }
      if (s.menu_color != -1)
      {
        AppendMenuA(g_mana_summary_popup_menu, 0, s.menu_color + 0x65, s.menu_text);
      }
    }
    if (0 < GetMenuItemCount(g_mana_summary_popup_menu))
    {
      AppendMenuA(g_mana_summary_popup_menu, MF_SEPARATOR, 0, (LPCSTR)0);
    }
    AppendMenuA(g_mana_summary_popup_menu, 0, 100, g_mana_summary_menu_help_text);
    return 0;

  case WM_MENUSELECT:
    if (((unsigned int)wparam >> 0x10) == 0xffff && lparam == 0)
    {
      s.menu_item_count = GetMenuItemCount(g_mana_summary_popup_menu);
      while (s.menu_item_count-- != 0)
      {
        DeleteMenu(g_mana_summary_popup_menu, 0, MF_BYPOSITION);
      }
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

// FUNCTION: MAGIC 0x00449d7e
// FUNCTION: SHANDALAR 0x004538ff
int get_displayed_mana_pool(mana_summary_values_t *mana_pool, int player)
{
  if (mana_pool == (mana_summary_values_t *)0)
  {
    return 0;
  }

  if (is_invalid_duel_player(player))
  {
    return 0;
  }

  EnterCriticalSection(&g_duel_render_lock);
  if (player == 0)
  {
    memcpy(mana_pool, g_duel_cached_raw_mana_player_0, sizeof(mana_summary_values_t));
  }
  else
  {
    memcpy(mana_pool, g_duel_cached_raw_mana_player_1, sizeof(mana_summary_values_t));
  }
  LeaveCriticalSection(&g_duel_render_lock);
  return 1;
}

// FUNCTION: MAGIC 0x0053a876
// FUNCTION: SHANDALAR 0x00552144
void get_mana_pool_text_rect(LPRECT rect, HWND hwnd, int mana_color)
{
  struct
  {
    mana_summary_values_t mana_pool;
    int row_index;
    int row_height;
    int row_spacing;
    int top_margin;
    int bottom;
    RECT client_rect;
    int top;
  } s;

  get_displayed_mana_pool(&s.mana_pool, (unsigned int)(hwnd != unk_008ce534));
  GetClientRect(hwnd, &s.client_rect);
  s.top_margin = (s.client_rect.bottom * 5) / 100;
  s.row_height = (s.client_rect.bottom * 0x91) / 1000;
  s.row_spacing = (s.client_rect.bottom * 0x14) / 1000;
  if (mana_color == 1)
  {
    s.row_index = 0;
  }
  else if (mana_color == 2)
  {
    s.row_index = 1;
  }
  else if (mana_color == 3)
  {
    s.row_index = 2;
  }
  else if (mana_color == 4)
  {
    s.row_index = 3;
  }
  else if (mana_color == 5)
  {
    s.row_index = 4;
  }
  else if (mana_color == 0)
  {
    s.row_index = 5;
  }
  else if (mana_color == 6)
  {
    s.row_index = 5;
  }
  else
  {
    s.row_index = -1;
  }
  if (s.row_index == -1)
  {
    SetRect(rect, 0, 0, 0, 0);
  }
  else
  {
    s.top = (s.row_spacing + s.row_height) * s.row_index + s.top_margin;
    s.bottom = s.top + s.row_height;
    SetRect(rect, s.client_rect.left, s.top, s.client_rect.right, s.bottom);
    if (s.mana_pool.artifact != 0)
    {
      if (mana_color == 0)
      {
        rect->right -= (s.client_rect.right - s.client_rect.left) / 2;
      }
      else if (mana_color == 6)
      {
        rect->left += ((s.client_rect.right - s.client_rect.left) * 2) / 3;
      }
    }
    else if (mana_color == 6)
    {
      SetRect(rect, 0, 0, 0, 0);
    }
  }
}
