#include <windows.h>
#include <stdio.h>
#include <string.h>

#include "cardartlib/src/palette.h"
#include "drawcardlib/Drawcardlib.h"
#include "drawcardlib/src/pic.h"
#include "magic/src/duel_engine.h"
#include "magic/src/game_support.h"
#include "magic/src/global_other.h"
#include "magic/src/global_state.h"
#include "magic/src/global_strings.h"
#include "magic/src/shared_startup.h"

#ifndef SHANDALAR
extern HWND global_main_hwnd;
#define DUEL_SHELL_WINDOW_HWND global_main_hwnd
#else
extern HWND g_main_window_hwnd;
#define DUEL_SHELL_WINDOW_HWND g_main_window_hwnd
#endif

#define CLASS_EXPANDED_GRAVEYARD "ExpandedGraveyard"
#define CLASS_GRAVEYARD_CARDS "GraveyardCards"

extern card_ptr_t global_raw_cards_storage[2000];
extern char global_base_directory[];
extern int DAT_007ab430[500];
extern int DAT_0091a940[500];
extern int DAT_00924820[500];
extern int DAT_00924ff0[16];
extern int DAT_009397d0[16];
extern int g_duel_cached_graveyard_count_player_0;
extern int g_duel_cached_graveyard_count_player_1;
extern int g_duel_cached_exile_count_player_0;
extern int g_duel_cached_exile_count_player_1;
extern int DAT_008966d0;
extern int DAT_0093d84c;
extern int is_invalid_duel_player(int player);
extern LRESULT handle_duel_inactive_cursor(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
void delete_and_close_object(HANDLE obj);
int load_text_with_tab_escapes(char *filename, char *section_name);
LRESULT CALLBACK wndproc_ExpandedGraveyard(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
LRESULT CALLBACK wndproc_GraveyardCards(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
LRESULT CALLBACK wndproc_MAGICGAME_GraveyardClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
BOOL CALLBACK dlgproc_ViewAntes(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

// GLOBAL: MAGIC 0x0055e00c
int DAT_0055e00c;

// GLOBAL: MAGIC 0x0055e010
// GLOBAL: SHANDALAR 0x0057f10c
int g_graveyard_card_window_long_offset = 0;

// GLOBAL: MAGIC 0x0055e014
// GLOBAL: SHANDALAR 0x0057f110
int g_graveyard_expanded_window_long_offset = 4;

// GLOBAL: MAGIC 0x0055e018
// GLOBAL: SHANDALAR 0x0057f114
int g_graveyard_bitmap_window_long_offset = 8;

// GLOBAL: MAGIC 0x0055e01c
int g_graveyard_window_extra_bytes = 0xc;

// GLOBAL: MAGIC 0x0055e020
int g_expanded_graveyard_window_extra_bytes = 0;

// GLOBAL: MAGIC 0x0055e024
// GLOBAL: SHANDALAR 0x0057f120
int DAT_0055e024;

// GLOBAL: MAGIC 0x0055e028
int g_graveyard_cards_window_extra_bytes = 4;

// GLOBAL: MAGIC 0x0056e9e0
char s_MENU_GRAVEYARD_0056e9e0[0x10] = "MENU_GRAVEYARD";

// GLOBAL: MAGIC 0x0056e9f0
char s_DIALOG_VIEWANTES_0056e9f0[0x18] = "DIALOG_VIEWANTES";

// GLOBAL: MAGIC 0x0056ea04
char s__duel_hlp_0056ea04[0xc] = "\\duel.hlp";

// GLOBAL: MAGIC 0x0056ea7c
char s__s_WINBK_Ante_pic_0056ea7c[0x14] = "%s\\WINBK_Ante.pic";

// GLOBAL: MAGIC 0x0056ea90
char s__s_WINBK_AnteLabel_pic_0056ea90[0x18] = "%s\\WINBK_AnteLabel.pic";

// GLOBAL: MAGIC 0x00637918
HBITMAP g_view_antes_background_bitmap;

// GLOBAL: MAGIC 0x0063791c
COLORREF g_view_antes_text_color;

// GLOBAL: MAGIC 0x00637958
// GLOBAL: SHANDALAR 0x00603a90
char g_graveyard_menu_view_text[0x34];

// GLOBAL: MAGIC 0x0063798c
HBITMAP g_view_antes_label_bitmap;

// GLOBAL: MAGIC 0x00637990
// GLOBAL: SHANDALAR 0x00603ac8
char g_graveyard_menu_help_text[0x1c];

// GLOBAL: MAGIC 0x006379ac
// GLOBAL: SHANDALAR 0x00603ae4
HMENU g_graveyard_popup_menu;

// GLOBAL: MAGIC 0x006379c0
// GLOBAL: SHANDALAR 0x00603af8
char g_graveyard_view_antes_opponent_text[0x68];

// GLOBAL: MAGIC 0x00637920
// GLOBAL: SHANDALAR 0x00603a58
char g_graveyard_view_antes_player_text[0x38];

// GLOBAL: MAGIC 0x00637a28
// GLOBAL: SHANDALAR 0x00603b60
char g_graveyard_menu_view_antes_text[0x38];

// GLOBAL: MAGIC 0x00637a60
// GLOBAL: SHANDALAR 0x00603b98
char g_graveyard_menu_view_exile_text[0x34];

extern int g_graveyard_card_window_long_offset;
extern int g_graveyard_expanded_window_long_offset;
extern int g_graveyard_bitmap_window_long_offset;
extern int DAT_0055e024;

// FUNCTION: MAGIC 0x00449ea5
// FUNCTION: SHANDALAR 0x00453a26
int copy_cached_graveyard_cards_and_get_count(void *cards, int player)
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
    result = g_duel_cached_graveyard_count_player_0;
  }
  else
  {
    result = g_duel_cached_graveyard_count_player_1;
  }
  memcpy(cards, (player == 0) ? DAT_007ab430 : DAT_0091a940, 2000);
  LeaveCriticalSection(&g_duel_render_lock);

  return result;
}

// FUNCTION: MAGIC 0x00449f48
// FUNCTION: SHANDALAR 0x00453ac9
int copy_cached_exile_cards_and_get_count(void *cards, int player)
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
    result = g_duel_cached_exile_count_player_0;
  }
  else
  {
    result = g_duel_cached_exile_count_player_1;
  }
  memcpy(cards, (player == 0) ? DAT_00924820 : global_exile[1], 2000);
  LeaveCriticalSection(&g_duel_render_lock);

  return result;
}

// FUNCTION: MAGIC 0x00450a70
// FUNCTION: SHANDALAR 0x0050d5d0
int register_MAGICGAME_GraveyardClass(LPCSTR class_name)
{
  ATOM atom1;
  ATOM atom2;
  ATOM atom3;
  WNDCLASSA wndclass;

  SET_DUEL_WNDCLASS(wndclass, 0xb, wndproc_MAGICGAME_GraveyardClass, g_graveyard_window_extra_bytes,
                    LoadIconA((HINSTANCE)0, (LPCSTR)0x7f00), (HBRUSH)GetStockObject(4), class_name);
  atom1 = RegisterClassA(&wndclass);
  SET_DUEL_WNDCLASS(wndclass, 8, wndproc_ExpandedGraveyard, g_expanded_graveyard_window_extra_bytes,
                    LoadIconA((HINSTANCE)0, (LPCSTR)0x7f00), (HBRUSH)GetStockObject(4), CLASS_EXPANDED_GRAVEYARD);
  atom2 = RegisterClassA(&wndclass);
  SET_DUEL_WNDCLASS(wndclass, 0xb, wndproc_GraveyardCards, g_graveyard_cards_window_extra_bytes,
                    LoadIconA((HINSTANCE)0, (LPCSTR)0x7f00), (HBRUSH)0x6, CLASS_GRAVEYARD_CARDS);
  atom3 = RegisterClassA(&wndclass);
  g_graveyard_popup_menu = CreatePopupMenu();
  load_text_with_tab_escapes(global_ui_strings_filename, s_MENU_GRAVEYARD_0056e9e0);
  strcpy(g_graveyard_menu_view_text, text_lines[0]);
  strcpy(g_graveyard_menu_view_exile_text, text_lines[1]);
  strcpy(g_graveyard_menu_view_antes_text, text_lines[2]);
  strcpy(g_graveyard_menu_help_text, text_lines[3]);
  load_text_with_tab_escapes(global_ui_strings_filename, s_DIALOG_VIEWANTES_0056e9f0);
  strcpy(g_graveyard_view_antes_opponent_text, text_lines[0]);
  strcpy(g_graveyard_view_antes_player_text, text_lines[1]);
  return atom1 != 0 && atom2 != 0 && atom3 != 0;
}

// FUNCTION: MAGIC 0x00451fd8
int get_cached_top_graveyard_card_id(int player)
{
  int cards[500];
  int count;

  count = copy_cached_graveyard_cards_and_get_count(cards, player);
  if (count == 0)
  {
    return -1;
  }

  return cards[count - 1];
}

// FUNCTION: MAGIC 0x00451fc3
// FUNCTION: SHANDALAR 0x0050eb13
void destroy_expanded_graveyard_window(HWND hwnd)
{
  DestroyWindow(hwnd);
}

// FUNCTION: MAGIC 0x00451bf9
// FUNCTION: SHANDALAR 0x0050e749
HWND create_expanded_graveyard_window(HWND hwnd, int show_graveyard)
{
  struct
  {
    int row_count;
    int card_x;
    int card_count;
    int card_index;
    WPARAM cards[500];
    int x_pad;
    int y_pad;
    int cards_per_row;
    int card_y;
    HWND card_window;
    HWND expanded_window;
    int card_height;
    unsigned int player;
    RECT client_rect;
    RECT expanded_rect;
  } s;
  int x_step;
  int last_row_cards;

  GetWindowRect(hwnd, &s.client_rect);
  s.expanded_rect.left = s.client_rect.left;
  s.expanded_rect.top = s.client_rect.top;
  GetClientRect(DUEL_SHELL_WINDOW_HWND, &s.client_rect);
  s.expanded_rect.right = (s.client_rect.right * 0x4b) / 100;
  s.expanded_rect.bottom = s.client_rect.bottom;
  GetClientRect(hwnd, &s.client_rect);
  s.card_height = s.client_rect.bottom;
  x_step = (s.client_rect.right * 0x3c) / 100;
  s.x_pad = 5;
  s.y_pad = 5;
  s.cards_per_row = (((s.expanded_rect.right - s.expanded_rect.left) - 10) - s.client_rect.right) / x_step + 1;
  s.player = hwnd != g_duel_player_graveyard_window_hwnd;
  s.expanded_window = CreateWindowExA(0, CLASS_EXPANDED_GRAVEYARD, "", WS_POPUP, 0, 0, 0, 0,
                                      DUEL_SHELL_WINDOW_HWND, (HMENU)0, g_app_instance, (LPVOID)0);
  if (s.expanded_window == (HWND)0)
  {
    return (HWND)0;
  }

  if (show_graveyard == 0)
  {
    SetClassLongA(s.expanded_window, GCL_HBRBACKGROUND, (LONG)GetStockObject(BLACK_BRUSH));
    s.card_count = copy_cached_exile_cards_and_get_count(s.cards, s.player);
  }
  else
  {
    SetClassLongA(s.expanded_window, GCL_HBRBACKGROUND, (LONG)GetStockObject(LTGRAY_BRUSH));
    s.card_count = copy_cached_graveyard_cards_and_get_count(s.cards, s.player);
  }

  s.card_x = s.x_pad;
  s.card_y = s.y_pad;
  s.row_count = 0;
  s.card_index = s.card_count;
  while (--s.card_index >= 0)
  {
    s.card_window = CreateWindowExA(0, CLASS_GRAVEYARD_CARDS, "", WS_CHILD | WS_VISIBLE,
                                    s.card_x, s.card_y, s.client_rect.right, s.card_height,
                                    s.expanded_window, (HMENU)1, g_app_instance, (LPVOID)0);
    if (s.card_window != (HWND)0)
    {
      s.row_count++;
      SendMessageA(s.card_window, 0x401, s.cards[s.card_index], 0);
      s.card_x += x_step;
      if (s.row_count % s.cards_per_row == 0)
      {
        s.card_x = s.x_pad;
        s.card_y += s.card_height + s.y_pad;
      }
    }
  }

  if (s.card_count == 0)
  {
    DestroyWindow(s.expanded_window);
    return (HWND)0;
  }

  BringWindowToTop(s.expanded_window);
  last_row_cards = s.cards_per_row - 1;
  if (s.card_count - 1 <= s.cards_per_row - 1)
  {
    last_row_cards = s.card_count - 1;
  }
  s.expanded_rect.right = last_row_cards * x_step + s.x_pad * 2 + s.expanded_rect.left + s.client_rect.right;
  s.row_count = s.card_count / s.cards_per_row;
  if (s.card_count % s.cards_per_row != 0)
  {
    s.row_count++;
  }
  s.expanded_rect.bottom =
      (s.card_height + s.y_pad) * (s.row_count - 1) + s.expanded_rect.top + s.y_pad * 2 + s.card_height;
  GetClientRect(DUEL_SHELL_WINDOW_HWND, &s.client_rect);
  if (s.client_rect.bottom < s.expanded_rect.bottom)
  {
    OffsetRect(&s.expanded_rect, 0, -(s.expanded_rect.bottom - s.client_rect.bottom));
  }
  MoveWindow(s.expanded_window, s.expanded_rect.left, s.expanded_rect.top,
             s.expanded_rect.right - s.expanded_rect.left,
             s.expanded_rect.bottom - s.expanded_rect.top, 1);
  ShowWindow(s.expanded_window, SW_SHOW);
  return s.expanded_window;
}

// FUNCTION: MAGIC 0x0044a0e9
void copy_cached_ante_cards(int *opponent_antes, int *opponent_count, int *player_antes, int *player_count)
{
  int index;

  if (opponent_antes == NULL || opponent_count == NULL || player_antes == NULL || player_count == NULL)
    return;

  EnterCriticalSection(&g_duel_render_lock);
  *opponent_count = DAT_008966d0;
  for (index = 0; index < DAT_008966d0; index++)
  {
    opponent_antes[index] = CardIDFromType(DAT_00924ff0[index]);
  }
  *player_count = DAT_0093d84c;
  for (index = 0; index < DAT_0093d84c; index++)
  {
    player_antes[index] = CardIDFromType(DAT_009397d0[index]);
  }
  LeaveCriticalSection(&g_duel_render_lock);
}

// FUNCTION: MAGIC 0x00452a75
void get_view_antes_card_rect(RECT *rect, HWND hwnd, int opponent, int index)
{
  struct
  {
    int opponent_antes[16];
    int player_antes[16];
    int card_width;
    int x_step;
    RECT opponent_rect;
    RECT player_rect;
    int player_count;
    int opponent_count;
    RECT client_rect;
  } s;

  copy_cached_ante_cards(s.opponent_antes, &s.opponent_count, s.player_antes, &s.player_count);
  GetWindowRect(GetDlgItem(hwnd, 0x4b3), &s.opponent_rect);
  MapWindowPoints(NULL, hwnd, (LPPOINT)&s.opponent_rect, 2);
  GetWindowRect(GetDlgItem(hwnd, 0x4b0), &s.player_rect);
  MapWindowPoints(NULL, hwnd, (LPPOINT)&s.player_rect, 2);
  s.card_width = s.opponent_rect.right - s.opponent_rect.left;
  GetClientRect(hwnd, &s.client_rect);
  s.client_rect.left = s.opponent_rect.left;

  if (opponent == 0)
  {
    if (s.player_count < 1 || s.player_count <= index)
    {
      SetRect(rect, 0, 0, 0, 0);
    }
    else
    {
      s.x_step = (s.card_width * 60) / 100;
      while (s.client_rect.right - s.client_rect.left < (s.player_count - 1) * s.x_step + s.card_width &&
             (s.card_width * 10) / 100 < s.x_step)
      {
        --s.x_step;
      }
      CopyRect(rect, &s.player_rect);
      OffsetRect(rect, s.x_step * index, 0);
    }
  }
  else if (s.opponent_count < 1 || s.opponent_count <= index)
  {
    SetRect(rect, 0, 0, 0, 0);
  }
  else
  {
    s.x_step = (s.card_width * 60) / 100;
    while (s.client_rect.right - s.client_rect.left < (s.opponent_count - 1) * s.x_step + s.card_width &&
           (s.card_width * 10) / 100 < s.x_step)
    {
      --s.x_step;
    }
    CopyRect(rect, &s.opponent_rect);
    OffsetRect(rect, s.x_step * index, 0);
  }
}

// FUNCTION: MAGIC 0x00452059
BOOL CALLBACK dlgproc_ViewAntes(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  struct
  {
    int opponent_antes[16];
    int player_antes[16];
    HDC paint_dc;
    PAINTSTRUCT paint;
    int player_count;
    int index;
    RECT card_rect;
    int opponent_count;
    int card_id;
    HDC erase_dc;
    HGDIOBJ font;
    RECT text_rect;
    RECT client_rect;
    char text[100];
    HWND ctl_hwnd;
    int ctl_id;
    HDC ctl_dc;
    WPARAM hover_opponent_antes[16];
    WPARAM hover_player_antes[16];
    POINT mouse_point;
    int hover_player_count;
    int hover_index;
    RECT hover_rect;
    int hover_opponent_count;
    WPARAM hover_card_id;
    HDC measure_dc;
    int label_height;
    int label_width;
    HGDIOBJ label_font;
    SIZE label_size;
    char path[264];
    char label_text[200];
    char opponent_name[100];
    RECT window_rect;
  } s;

  switch (msg)
  {
  case WM_INITDIALOG:
    ShowWindow(GetDlgItem(hwnd, 0x4b3), 0);
    ShowWindow(GetDlgItem(hwnd, 0x4b0), 0);
    ShowWindow(GetDlgItem(hwnd, 0x4b1), 0);
    ShowWindow(GetDlgItem(hwnd, 0x4b2), 0);
    sprintf(s.path, s__s_WINBK_Ante_pic_0056ea7c, global_duelart_path);
    g_view_antes_background_bitmap = load_pic(s.path);
    sprintf(s.path, s__s_WINBK_AnteLabel_pic_0056ea90, global_duelart_path);
    g_view_antes_label_bitmap = load_pic(s.path);
    g_view_antes_text_color = 0;
    copy_opponent_name_prefix(s.opponent_name);
    sprintf(s.label_text, g_graveyard_view_antes_opponent_text, s.opponent_name);
    SetDlgItemTextA(hwnd, 0x4b1, s.label_text);
    strcpy(s.label_text, g_graveyard_view_antes_player_text);
    SetDlgItemTextA(hwnd, 0x4b2, s.label_text);
    s.measure_dc = GetDC(hwnd);
    ApplyCardArtPaletteToDc(s.measure_dc);
    s.label_font = (HGDIOBJ)SendDlgItemMessageA(hwnd, 0x4b1, WM_GETFONT, 0, 0);
    SelectObject(s.measure_dc, s.label_font);
    GetDlgItemTextA(hwnd, 0x4b1, s.label_text, 200);
    GetTextExtentPoint32A(s.measure_dc, s.label_text, strlen(s.label_text), &s.label_size);
    s.label_width = s.label_size.cx + s.label_size.cy;
    s.label_height = (s.label_size.cy * 3) / 2;
    SetWindowPos(GetDlgItem(hwnd, 0x4b1), NULL, 0, 0, s.label_width, s.label_height, SWP_NOZORDER | SWP_NOMOVE);
    SetWindowPos(GetDlgItem(hwnd, 0x4b2), NULL, 0, 0, s.label_width, s.label_height, SWP_NOZORDER | SWP_NOMOVE);
    ReleaseDC(hwnd, s.measure_dc);
    GetWindowRect(hwnd, &s.window_rect);
    SetWindowPos(hwnd, NULL, (GetSystemMetrics(SM_CXSCREEN) * 20) / 100, s.window_rect.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
    SetFocus(hwnd);
    return 0;

  case WM_KEYDOWN:
  case WM_COMMAND:
    delete_and_close_object(g_view_antes_background_bitmap);
    delete_and_close_object(g_view_antes_label_bitmap);
    EndDialog(hwnd, 0);
    return 1;

  case WM_MOUSEMOVE:
  case WM_RBUTTONDOWN:
    copy_cached_ante_cards((int *)s.hover_opponent_antes, &s.hover_opponent_count, (int *)s.hover_player_antes, &s.hover_player_count);
    s.mouse_point.x = (unsigned int)lparam & 0xffff;
    s.mouse_point.y = (unsigned short)(((unsigned int)lparam >> 16) & 0xffff);
    if ((msg == WM_MOUSEMOVE && g_duel_interface_options.layout != 2) || (msg == WM_RBUTTONDOWN && g_duel_interface_options.layout == 2))
    {
      s.hover_card_id = 0xffffffff;
      if (s.hover_player_count != 0)
      {
        for (s.hover_index = s.hover_player_count - 1; s.hover_index >= 0 && s.hover_card_id == 0xffffffff; --s.hover_index)
        {
          get_view_antes_card_rect(&s.hover_rect, hwnd, 0, s.hover_index);
          if (PtInRect(&s.hover_rect, s.mouse_point) != 0)
          {
            s.hover_card_id = s.hover_player_antes[s.hover_index];
          }
        }
      }
      if (s.hover_opponent_count != 0)
      {
        for (s.hover_index = s.hover_opponent_count - 1; s.hover_index >= 0 && s.hover_card_id == 0xffffffff; --s.hover_index)
        {
          get_view_antes_card_rect(&s.hover_rect, hwnd, 1, s.hover_index);
          if (PtInRect(&s.hover_rect, s.mouse_point) != 0)
          {
            s.hover_card_id = s.hover_opponent_antes[s.hover_index];
          }
        }
      }
      if (s.hover_card_id != 0xffffffff)
      {
        SendMessageA(g_duel_card_preview_window_hwnd, 0x401, s.hover_card_id, 0);
      }
    }
    return 0;

  case WM_LBUTTONDOWN:
    delete_and_close_object(g_view_antes_background_bitmap);
    delete_and_close_object(g_view_antes_label_bitmap);
    EndDialog(hwnd, 0);
    return 1;

  case WM_CTLCOLORSTATIC:
    s.ctl_dc = (HDC)wparam;
    ApplyCardArtPaletteToDc(s.ctl_dc);
    s.ctl_hwnd = (HWND)lparam;
    s.ctl_id = GetDlgCtrlID(s.ctl_hwnd);
    SetBkMode(s.ctl_dc, TRANSPARENT);
    SetTextColor(s.ctl_dc, g_view_antes_text_color);
    return (BOOL)GetStockObject(NULL_BRUSH);

  case WM_QUERYNEWPALETTE:
  case WM_PALETTECHANGED:
  case WM_PALETTEISCHANGING:
    return FUN_10025b5e((int)hwnd, msg, (int)wparam, lparam);

  case WM_ERASEBKGND:
    s.erase_dc = (HDC)wparam;
    ApplyCardArtPaletteToDc(s.erase_dc);
    GetClientRect(hwnd, &s.client_rect);
    s.font = (HGDIOBJ)SendDlgItemMessageA(hwnd, 0x4b1, WM_GETFONT, 0, 0);
    SelectObject(s.erase_dc, s.font);
    SetTextColor(s.erase_dc, 0);
    SetBkMode(s.erase_dc, TRANSPARENT);
    if (g_view_antes_background_bitmap != NULL)
    {
      DrawBitmapToRect(s.erase_dc, &s.client_rect, g_view_antes_background_bitmap);
    }
    else
    {
      FillRect(s.erase_dc, &s.client_rect, GetStockObject(2));
    }
    GetWindowRect(GetDlgItem(hwnd, 0x4b1), &s.text_rect);
    MapWindowPoints(NULL, hwnd, (LPPOINT)&s.text_rect, 2);
    DrawBitmapToRect(s.erase_dc, &s.text_rect, g_view_antes_label_bitmap);
    GetDlgItemTextA(hwnd, 0x4b1, s.text, 100);
    s.text_rect.left += (s.text_rect.bottom - s.text_rect.top) / 4;
    DrawTextA(s.erase_dc, s.text, -1, &s.text_rect, DT_SINGLELINE | DT_VCENTER);
    GetWindowRect(GetDlgItem(hwnd, 0x4b2), &s.text_rect);
    MapWindowPoints(NULL, hwnd, (LPPOINT)&s.text_rect, 2);
    DrawBitmapToRect(s.erase_dc, &s.text_rect, g_view_antes_label_bitmap);
    GetDlgItemTextA(hwnd, 0x4b2, s.text, 100);
    s.text_rect.left += (s.text_rect.bottom - s.text_rect.top) / 4;
    DrawTextA(s.erase_dc, s.text, -1, &s.text_rect, DT_SINGLELINE | DT_VCENTER);
    return 1;

  case WM_PAINT:
    s.paint_dc = BeginPaint(hwnd, &s.paint);
    if (s.paint_dc != NULL)
    {
      ApplyCardArtPaletteToDc(s.paint_dc);
      copy_cached_ante_cards(s.opponent_antes, &s.opponent_count, s.player_antes, &s.player_count);
      if (s.opponent_count != 0)
      {
        for (s.index = 0; s.index < s.opponent_count; ++s.index)
        {
          get_view_antes_card_rect(&s.card_rect, hwnd, 1, s.index);
          s.card_id = s.opponent_antes[s.index];
          DrawFullCard(s.paint_dc, &s.card_rect, global_raw_cards_storage + s.card_id, 0, 0x12, 0,
                       gs_illus_00789130);
        }
      }
      if (s.player_count != 0)
      {
        for (s.index = 0; s.player_count > s.index; ++s.index)
        {
          get_view_antes_card_rect(&s.card_rect, hwnd, 0, s.index);
          s.card_id = s.player_antes[s.index];
          DrawFullCard(s.paint_dc, &s.card_rect, global_raw_cards_storage + s.card_id, 0, 0x12, 0,
                       gs_illus_00789130);
        }
      }
      EndPaint(hwnd, &s.paint);
    }
    return 1;

  default:
    return 0;
  }
}

// FUNCTION: MAGIC 0x00452030
// FUNCTION: SHANDALAR 0x0050eb80
void show_view_antes_dialog(void)
{
  DialogBoxParamA(g_app_instance, (LPCSTR)0xeb, g_duel_window_hwnd, dlgproc_ViewAntes, 0);
}

// FUNCTION: MAGIC 0x00451926
// FUNCTION: SHANDALAR 0x0050e476
LRESULT CALLBACK wndproc_GraveyardCards(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  struct
  {
    HDC dc;
    PAINTSTRUCT paint;
    RECT client_rect;
    int card_id;
  } s;

  switch (msg)
  {

  case 0x437:
    s.card_id = GetWindowLongA(hwnd, DAT_0055e024);
    if (g_duel_interface_options.layout != 2 || IsWindowVisible(g_duel_card_preview_window_hwnd) != 0)
    {
      SendMessageA(g_duel_card_preview_window_hwnd, 0x401, s.card_id, 0);
    }
    return 0;

  case 0x400:
    return s.card_id = GetWindowLongA(hwnd, DAT_0055e024);

  case 0x401:
    s.card_id = GetWindowLongA(hwnd, DAT_0055e024);
    if (s.card_id != (int)wparam)
    {
      s.card_id = wparam;
      SetWindowLongA(hwnd, DAT_0055e024, s.card_id);
      InvalidateRect(hwnd, NULL, FALSE);
    }
    return 0;

  case WM_CREATE:
    s.card_id = -1;
    SetWindowLongA(hwnd, DAT_0055e024, s.card_id);
    return 0;

  case WM_PAINT:
    s.card_id = GetWindowLongA(hwnd, DAT_0055e024);
    GetClientRect(hwnd, &s.client_rect);
    EnterCriticalSection(&g_card_render_lock);
    FillRect(g_shared_offscreen_dc, &s.client_rect, GetStockObject(4));
    DrawFullCard(g_shared_offscreen_dc, &s.client_rect, global_raw_cards_storage + s.card_id, 0, 0x11, 0,
                 gs_illus_00789130);
    s.dc = BeginPaint(hwnd, &s.paint);
    if (s.dc != (HDC)0)
    {
      ApplyCardArtPaletteToDc(s.dc);
      BitBlt(s.dc, 0, 0, s.client_rect.right, s.client_rect.bottom,
             g_shared_offscreen_dc, 0, 0, SRCCOPY);
      EndPaint(hwnd, &s.paint);
    }
    LeaveCriticalSection(&g_card_render_lock);
    return 0;

  case WM_RBUTTONDBLCLK:
    s.card_id = GetWindowLongA(hwnd, DAT_0055e024);
    if (g_duel_interface_options.layout == 2)
    {
      SendMessageA(g_duel_card_preview_window_hwnd, 0x401, s.card_id, 0);
    }
    return 0;

  case WM_QUERYNEWPALETTE:
  case WM_PALETTECHANGED:
  case WM_PALETTEISCHANGING:
    return FUN_10025b5e((int)hwnd, msg, (int)wparam, lparam);

  default:
    return DefWindowProcA(hwnd, msg, wparam, lparam);
  }

  return 0;
}

// FUNCTION: MAGIC 0x00450cd7
// FUNCTION: SHANDALAR 0x0050d837
LRESULT CALLBACK wndproc_MAGICGAME_GraveyardClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  struct
  {
    int local_a70;
    int local_a6c;
    int local_a68;
    int menu_item_count;
    int cards[500];
    unsigned int player;
    MSG peek_msg;
    int show_popup;
    POINT popup_point;
    RECT popup_rect;
    HDC paint_dc;
    PAINTSTRUCT paint;
    RECT client_rect;
    int card_id;
    char help_path[264];
    DWORD help_context;
    unsigned int expand_mode;
    HWND expanded_window;
    int new_card_id;
    WPARAM hover_card_id;
    char cuecard_text[100];
    char opponent_name[100];
    int previous_card_id;
    HBITMAP background_bitmap;
    HWND captured_window;
    int local_padding_4;
    int local_padding_0;
  } s;

  switch (msg)
  {
  case 0x437:
    if (hwnd == g_duel_player_graveyard_window_hwnd)
    {
      strcpy(s.cuecard_text, gs_cuecard_your_graveyard_0091c9b0);
    }
    else
    {
      copy_opponent_name_prefix(s.opponent_name);
      sprintf(s.cuecard_text, gs_cuecard_opponent_graveyard_0091c7b0, s.opponent_name);
    }
    strcpy((char *)wparam, s.cuecard_text);
    s.hover_card_id = get_cached_top_graveyard_card_id(hwnd != g_duel_player_graveyard_window_hwnd);
    if (s.hover_card_id != 0xffffffff &&
        (g_duel_interface_options.layout != 2 || IsWindowVisible(g_duel_card_preview_window_hwnd) != 0))
    {
      SendMessageA(g_duel_card_preview_window_hwnd, 0x401, s.hover_card_id, 0);
    }
    return 1;

  case 0x433:
  case 0x434:
    s.new_card_id = get_cached_top_graveyard_card_id(hwnd != g_duel_player_graveyard_window_hwnd);
    if ((int)wparam == s.new_card_id)
    {
      InvalidateRect(hwnd, NULL, FALSE);
    }
    return 0;

  case 0x432:
    s.previous_card_id = GetWindowLongA(hwnd, g_graveyard_card_window_long_offset);
    s.new_card_id = get_cached_top_graveyard_card_id(hwnd != g_duel_player_graveyard_window_hwnd);
    SendMessageA(hwnd, 0x400, 0, 0);
    if (s.new_card_id != s.previous_card_id)
    {
      InvalidateRect(hwnd, NULL, FALSE);
    }
    return 0;

  case 0x439:
    s.background_bitmap = (HBITMAP)GetWindowLongA(hwnd, g_graveyard_bitmap_window_long_offset);
    if (s.background_bitmap != (HBITMAP)0)
    {
      DeleteObject(s.background_bitmap);
    }
    s.background_bitmap = (HBITMAP)wparam;
    SetWindowLongA(hwnd, g_graveyard_bitmap_window_long_offset, (LONG)wparam);
    InvalidateRect(hwnd, NULL, TRUE);
    return 0;

  case 0x438:
    return GetWindowLongA(hwnd, g_graveyard_bitmap_window_long_offset);

  case 0x400:
    s.captured_window = (HWND)GetWindowLongA(hwnd, g_graveyard_expanded_window_long_offset);
    s.expanded_window = (HWND)wparam;
    s.expand_mode = lparam;
    if (wparam != 0)
    {
      if (s.captured_window == (HWND)0)
      {
        s.captured_window = create_expanded_graveyard_window(hwnd, lparam);
      }
      SetWindowLongA(hwnd, g_graveyard_expanded_window_long_offset, (LONG)s.captured_window);
      if (s.captured_window != (HWND)0)
      {
        SetCapture(s.captured_window);
      }
    }
    else if (s.captured_window != (HWND)0)
    {
      ReleaseCapture();
      destroy_expanded_graveyard_window(s.captured_window);
      s.captured_window = (HWND)0;
      SetWindowLongA(hwnd, g_graveyard_expanded_window_long_offset, 0);
    }
    return 0;

  case WM_COMMAND:
    switch ((UINT)wparam & 0xffff)
    {
    case 100:
      SendMessageA(hwnd, 0x400, 1, 1);
      break;
    case 0x65:
      SendMessageA(hwnd, 0x400, 1, 0);
      break;
    case 0x66:
      show_view_antes_dialog();
      break;
    case 0x67:
      s.help_context = 0x7e6;
      strcpy(s.help_path, global_base_directory);
      strcat(s.help_path, s__duel_hlp_0056ea04);
      WinHelpA(g_duel_window_hwnd, s.help_path, HELP_CONTEXT, s.help_context);
      break;
    }
    return 0;

  case WM_CREATE:
    s.previous_card_id = -1;
    SetWindowLongA(hwnd, g_graveyard_card_window_long_offset, -1);
    s.captured_window = (HWND)0;
    SetWindowLongA(hwnd, g_graveyard_expanded_window_long_offset, 0);
    s.background_bitmap = (HBITMAP)0;
    SetWindowLongA(hwnd, g_graveyard_bitmap_window_long_offset, 0);
    return 0;

  case WM_DESTROY:
    s.background_bitmap = (HBITMAP)GetWindowLongA(hwnd, g_graveyard_bitmap_window_long_offset);
    if (s.background_bitmap != (HBITMAP)0)
    {
      delete_and_close_object(s.background_bitmap);
    }
    return 0;

  case WM_ERASEBKGND:
    return 1;

  case WM_LBUTTONDOWN:
    SendMessageA(hwnd, 0x400, 1, 1);
    return 0;

  case WM_PAINT:
    s.previous_card_id = GetWindowLongA(hwnd, g_graveyard_card_window_long_offset);
    s.background_bitmap = (HBITMAP)GetWindowLongA(hwnd, g_graveyard_bitmap_window_long_offset);
    s.card_id = get_cached_top_graveyard_card_id(hwnd != g_duel_player_graveyard_window_hwnd);
    if (s.card_id != s.previous_card_id)
    {
      InvalidateRect(hwnd, NULL, FALSE);
    }
    EnterCriticalSection(&g_card_render_lock);
    GetClientRect(hwnd, &s.client_rect);
    FillRect(g_shared_offscreen_dc, &s.client_rect, GetStockObject(4));
    if (s.card_id == -1)
    {
      if (s.background_bitmap != (HBITMAP)0)
      {
        DrawBitmapToRect(g_shared_offscreen_dc, &s.client_rect, s.background_bitmap);
      }
    }
    else
    {
      DrawFullCard(g_shared_offscreen_dc, &s.client_rect, global_raw_cards_storage + s.card_id, 0, 0x11, 0,
                   gs_illus_00789130);
    }
    s.paint_dc = BeginPaint(hwnd, &s.paint);
    if (s.paint_dc != (HDC)0)
    {
      ApplyCardArtPaletteToDc(s.paint_dc);
      if (g_duel_palette_refresh_pending != 0)
      {
        FillRect(s.paint_dc, &s.client_rect, GetStockObject(BLACK_BRUSH));
        Sleep(200);
      }
      BitBlt(s.paint_dc, 0, 0, s.client_rect.right, s.client_rect.bottom,
             g_shared_offscreen_dc, 0, 0, SRCCOPY);
      EndPaint(hwnd, &s.paint);
      s.previous_card_id = s.card_id;
      SetWindowLongA(hwnd, g_graveyard_card_window_long_offset, s.card_id);
    }
    LeaveCriticalSection(&g_card_render_lock);
    return 0;

  case WM_RBUTTONDOWN:
    s.show_popup = 1;
    if (g_duel_interface_options.layout == 2)
    {
      Sleep(GetDoubleClickTime());
      if (PeekMessageA(&s.peek_msg, hwnd, WM_RBUTTONDBLCLK, WM_RBUTTONDBLCLK, 0) != 0)
      {
        s.show_popup = 0;
      }
    }
    if (s.show_popup != 0)
    {
      s.popup_point.x = (unsigned int)lparam & 0xffff;
      s.popup_point.y = HIWORD(lparam);
      ClientToScreen(hwnd, &s.popup_point);
      SetRect(&s.popup_rect, s.popup_point.x, s.popup_point.y, s.popup_point.x + 1, s.popup_point.y + 1);
      TrackPopupMenu(g_graveyard_popup_menu, TPM_RIGHTBUTTON, s.popup_point.x, s.popup_point.y, 0, hwnd,
                     &s.popup_rect);
    }
    return 0;

  case WM_INITMENUPOPUP:
    s.player = hwnd != g_duel_player_graveyard_window_hwnd;
    AppendMenuA(g_graveyard_popup_menu, MF_STRING, 100, g_graveyard_menu_view_text);
    if (copy_cached_graveyard_cards_and_get_count(s.cards, s.player) == 0)
    {
      EnableMenuItem(g_graveyard_popup_menu, 100, MF_GRAYED);
    }
    AppendMenuA(g_graveyard_popup_menu, MF_STRING, 0x65, g_graveyard_menu_view_exile_text);
    if (copy_cached_exile_cards_and_get_count(s.cards, s.player) == 0)
    {
      EnableMenuItem(g_graveyard_popup_menu, 0x65, MF_GRAYED);
    }
    AppendMenuA(g_graveyard_popup_menu, MF_STRING, 0x66, g_graveyard_menu_view_antes_text);
    AppendMenuA(g_graveyard_popup_menu, MF_SEPARATOR, 0, NULL);
    AppendMenuA(g_graveyard_popup_menu, MF_STRING, 0x67, g_graveyard_menu_help_text);
    return 0;

  case WM_MENUSELECT:
    if (((UINT)wparam >> 16) == 0xffff && lparam == 0)
    {
      s.menu_item_count = GetMenuItemCount(g_graveyard_popup_menu);
      while (s.menu_item_count != 0)
      {
        DeleteMenu(g_graveyard_popup_menu, 0, MF_BYPOSITION);
        s.menu_item_count--;
      }
    }
    return 0;

  case WM_RBUTTONDBLCLK:
    s.hover_card_id = get_cached_top_graveyard_card_id(hwnd != g_duel_player_graveyard_window_hwnd);
    if (s.hover_card_id != 0xffffffff && g_duel_interface_options.layout == 2)
    {
      SendMessageA(g_duel_card_preview_window_hwnd, 0x401, s.hover_card_id, 0);
    }
    return 0;

  case WM_SETCURSOR:
    return handle_duel_inactive_cursor(hwnd, msg, wparam, lparam);

  case WM_QUERYNEWPALETTE:
  case WM_PALETTECHANGED:
  case WM_PALETTEISCHANGING:
    return FUN_10025b5e((int)hwnd, msg, (int)wparam, lparam);

  default:
    return DefWindowProcA(hwnd, msg, wparam, lparam);
  }

  return 0;
}
