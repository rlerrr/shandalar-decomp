#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <windows.h>
#include "cardartlib/src/palette.h"
#include "deckdll/src/shared_resources.h"
#include "drawcardlib/Drawcardlib.h"
#include "game_support.h"
#include "global_state.h"
#include "global_strings.h"

typedef ptrdiff_t INT_PTR;

// GLOBAL: MAGIC 0x0055e0c0
// GLOBAL: SHANDALAR 0x0057f128
int g_showlist_card_csvid_window_long_offset = 0;
// GLOBAL: MAGIC 0x0055e0c4
// GLOBAL: SHANDALAR 0x0057f12c
int g_showlist_card_count_window_long_offset = 4;
// GLOBAL: MAGIC 0x0055e0c8
// GLOBAL: SHANDALAR 0x0057f130
int g_showlist_card_show_count_window_long_offset = 8;
// GLOBAL: MAGIC 0x0055e0cc
// GLOBAL: SHANDALAR 0x0057f134
int g_showlist_card_window_extra_bytes = 0xc;
// GLOBAL: MAGIC 0x008a9190
// GLOBAL: SHANDALAR 0x008bd390
int g_showlist_smallcard_width;
// GLOBAL: MAGIC 0x008cf1b0
// GLOBAL: SHANDALAR 0x008e3300
int g_showlist_smallcard_height;
// GLOBAL: MAGIC 0x00638b34
// GLOBAL: SHANDALAR 0x0065027c
int DAT_00638b34;
// GLOBAL: MAGIC 0x00638b48
int DAT_00638b48;
// GLOBAL: MAGIC 0x00638b68
// GLOBAL: SHANDALAR 0x006502b0
int DAT_00638b68;
// GLOBAL: MAGIC 0x00638b70
// GLOBAL: SHANDALAR 0x006502b8
int DAT_00638b70;
// GLOBAL: MAGIC 0x00638b80
int DAT_00638b80;
// GLOBAL: MAGIC 0x00638ba4
// GLOBAL: SHANDALAR 0x006502ec
int *DAT_00638ba4;
// GLOBAL: MAGIC 0x00638bf4
// GLOBAL: SHANDALAR 0x0065033c
int DAT_00638bf4;
// GLOBAL: MAGIC 0x00638c08
// GLOBAL: SHANDALAR 0x00650350
int DAT_00638c08;
// GLOBAL: MAGIC 0x00638c40
// GLOBAL: SHANDALAR 0x00650388
int DAT_00638c40;
// GLOBAL: MAGIC 0x00638c44
// GLOBAL: SHANDALAR 0x0065038c
int DAT_00638c44;
// GLOBAL: MAGIC 0x00638c6c
// GLOBAL: SHANDALAR 0x006503b4
int DAT_00638c6c;
// GLOBAL: MAGIC 0x00638c84
// GLOBAL: SHANDALAR 0x006503cc
int DAT_00638c84;

extern HPALETTE global_cart_art_hpalette;
extern card_ptr_t global_raw_cards_storage[2000];
extern CRITICAL_SECTION g_card_render_lock;
extern HDC g_shared_offscreen_dc;
extern HINSTANCE g_app_instance;

void FUN_0055b9f0(int dc, int *rect, int value);

// TODO: cleanup this bucket of shit
#ifdef SHANDALAR
#define SHOWLIST_CARD_BACK_CSVID (*(int *)&gs_phasebar_your_main_postcombat_00926670[0x60])
#else
#define SHOWLIST_CARD_BACK_CSVID unk_009266ac
#endif
#define SHOWLIST_MOUSE_MODE g_duel_interface_options.layout

// FUNCTION: MAGIC 0x0049fd0c
// FUNCTION: SHANDALAR 0x0053b6af
void FUN_0049fd0c(int *brush1, int *pen1, int *pen2, int *pen3, int *brush2, int *text_color)
{
  *brush1 = (int)CreateSolidBrush(0x10000c7);
  *pen1 = (int)CreatePen(0, 0, 0x1000086);
  *pen2 = (int)CreatePen(0, 0, 0x100001d);
  *pen3 = (int)CreatePen(0, 0, 0x10000c9);
  *brush2 = (int)CreateSolidBrush(0x100000f);
  *text_color = 0x1000090;

  if (*brush1 == 0)
    *brush1 = (int)GetStockObject(2);

  if (*pen1 == 0)
    *pen1 = (int)GetStockObject(6);

  if (*pen2 == 0)
    *pen2 = (int)GetStockObject(6);

  if (*pen3 == 0)
    *pen3 = (int)GetStockObject(7);

  if (*brush2 == 0)
    *brush2 = (int)GetStockObject(2);
}

// FUNCTION: MAGIC 0x0049fdf9
// FUNCTION: SHANDALAR 0x0053b79c
void FUN_0049fdf9(HGDIOBJ brush1, HGDIOBJ pen1, HGDIOBJ pen2, HGDIOBJ pen3, HGDIOBJ brush2)
{
  if (brush1)
    DeleteObject(brush1);

  if (pen1)
    DeleteObject(pen1);

  if (pen2)
    DeleteObject(pen2);

  if (pen3)
    DeleteObject(pen3);

  if (brush2)
    DeleteObject(brush2);
}

// FUNCTION: MAGIC 0x0049ebde
// FUNCTION: SHANDALAR 0x0053a586
INT_PTR CALLBACK dlgfunc_show_deck(HWND hwnd, UINT msg, WPARAM wparam_dc, LPARAM lparam_data)
{
  struct
  {
    int unused_160;   // ebp-0x160
    LONG long_result; // ebp-0x15c
    HDC screen_dc;
    int nc_calc_result;         // ebp-0x154
    RECT hit_rect;              // ebp-0x150
    SIZE text_extent;           // ebp-0x140
    int right_border_width;     // ebp-0x138
    RECT client_rect;           // ebp-0x134
    char title_text[100];       // ebp-0x124
    HDC dc;                     // ebp-0x0c0
    int row;                    // ebp-0x0bc
    int frame_width;            // ebp-0x0b8
    int bottom_border_height;   // ebp-0x0b4
    int other_frame_width;      // ebp-0x0b0
    LONG has_selection;         // ebp-0x0ac
    int current_button_state;   // ebp-0x0a8
    RECT title_rect;            // ebp-0x0a4
    RECT window_rect;           // ebp-0x094
    HDC erase_dc;               // ebp-0x084
    RECT erase_rect;            // ebp-0x080
    int scroll_pos;             // ebp-0x070
    int scroll_frame_width;     // ebp-0x06c
    int scroll_min;             // ebp-0x068
    int client_bottom;          // ebp-0x064
    RECT scroll_rect;           // ebp-0x060
    int scroll_max;             // ebp-0x050
    int new_scroll_pos;         // ebp-0x04c
    UINT command;               // ebp-0x048
    LONG command_has_selection; // ebp-0x044
    int *dialog_data;           // ebp-0x040
    int selection_index;        // ebp-0x03c
    int destroy_result;         // ebp-0x038
    HWND card_window;           // ebp-0x034
    int init_dummy_top;         // ebp-0x030
    int columns;                // ebp-0x02c
    int rows;                   // ebp-0x028
    int button_index;           // ebp-0x024
    int button_y;               // ebp-0x020
    int button_x;               // ebp-0x01c
    int client_top_limit;       // ebp-0x018
    int title_bar_height;       // ebp-0x014
    RECT rect;                  // ebp-0x010
  } s;

  switch (msg)
  {
  case WM_INITDIALOG:
    s.init_dummy_top = 1;
    DAT_00638ba4 = (int *)lparam_data;
    SetWindowLongA(hwnd, 8, DAT_00638ba4[0x5df]);
    DAT_00638c08 = 0;
    FUN_0049fd0c(&DAT_00638c40, &DAT_00638b68, &DAT_00638c44, &DAT_00638b70, &DAT_00638bf4, &DAT_00638c6c);
    SetWindowTextA(hwnd, (LPCSTR)*DAT_00638ba4);
    s.columns = DAT_00638ba4[0x5dd];
    DAT_00638b80 = (g_showlist_smallcard_width * 2) / 3;
    DAT_00638c84 = (g_showlist_smallcard_height * 2) / 3;
    DAT_00638b48 = 8;
    DAT_00638b34 = 8;
    s.rows = 6;
    if (s.columns % s.rows == 1)
    {
      --s.rows;
    }
    s.title_bar_height = s.columns / s.rows;
    if (s.columns % s.rows > 0)
    {
      ++s.title_bar_height;
    }
    else
    {
    }
    s.client_top_limit = 4;
    SetRect(&s.rect,
            0,
            0,
            (DAT_00638b48 + DAT_00638b80) * s.rows + DAT_00638b48,
            (DAT_00638c84 + DAT_00638b34) * s.title_bar_height + DAT_00638b34);
    SetWindowLongA(hwnd, GWL_STYLE, GetWindowLongA(hwnd, GWL_STYLE) & ~WS_VSCROLL);
    if (s.client_top_limit < s.title_bar_height)
    {
      SetWindowLongA(hwnd, GWL_STYLE, GetWindowLongA(hwnd, GWL_STYLE) | WS_VSCROLL);
      SetScrollRange(hwnd, 1, 0, s.rect.bottom - ((DAT_00638c84 + DAT_00638b34) * s.client_top_limit + DAT_00638b34), 1);
      SetScrollPos(hwnd, 1, 0, 1);
      s.rect.bottom = (DAT_00638c84 + DAT_00638b34) * s.client_top_limit + s.rect.top + DAT_00638b34;
      s.rect.right += GetSystemMetrics(2);
    }
    AdjustWindowRect(&s.rect, GetWindowLongA(hwnd, GWL_STYLE), 0);
    SetWindowPos(hwnd,
                 0,
                 (GetSystemMetrics(0) - (s.rect.right - s.rect.left)) / 2,
                 (GetSystemMetrics(1) - (s.rect.bottom - s.rect.top)) / 2,
                 s.rect.right - s.rect.left,
                 s.rect.bottom - s.rect.top,
                 4);
    s.button_x = DAT_00638b48;
    s.button_y = DAT_00638b34;
    GetClientRect(hwnd, &s.rect);
    for (s.button_index = 0; s.button_index < DAT_00638ba4[0x5dd]; ++s.button_index)
    {
      s.card_window = CreateWindowExA(0,  "ShowListCard", "List Card", 0x50000000, s.button_x, s.button_y, DAT_00638b80, DAT_00638c84, hwnd, (HMENU)(s.button_index + 10),
                                      g_app_instance, (LPVOID)DAT_00638ba4[s.button_index + 1]);
      if (DAT_00638ba4[0x5de] != 0)
      {
        SendMessageA(s.card_window, 0x414, 1, DAT_00638ba4[s.button_index + 0x1f5]);
      }
      s.button_x += DAT_00638b48 + DAT_00638b80;
      if (s.rect.right < s.button_x + DAT_00638b80)
      {
        s.button_y += DAT_00638c84 + DAT_00638b34;
        s.button_x = DAT_00638b48;
      }
    }
    SetFocus(hwnd);
    return 0;
  case WM_LBUTTONDOWN:
  case WM_CLOSE:

    s.destroy_result = GetWindowLongA(hwnd, 8);
    if (s.destroy_result == 0)
    {
      FUN_0049fdf9((HGDIOBJ)DAT_00638c40,
                   (HGDIOBJ)DAT_00638b68,
                   (HGDIOBJ)DAT_00638c44,
                   (HGDIOBJ)DAT_00638b70,
                   (HGDIOBJ)DAT_00638bf4);
      EndDialog(hwnd, -1);
    }
    return 1;

  case WM_COMMAND:
    s.command = (unsigned int)wparam_dc & 0xffff;
    s.dialog_data = (int *)lparam_data;
    s.command_has_selection = GetWindowLongA(hwnd, 8);
    if (s.command == 2 || s.command == 1)
    {
      if (s.command_has_selection == 0)
      {
        FUN_0049fdf9((HGDIOBJ)DAT_00638c40,
                     (HGDIOBJ)DAT_00638b68,
                     (HGDIOBJ)DAT_00638c44,
                     (HGDIOBJ)DAT_00638b70,
                     (HGDIOBJ)DAT_00638bf4);
        EndDialog(hwnd, -1);
      }
    }
    else if (s.dialog_data != 0)
    {
      s.selection_index = s.command - 10;
      if (DAT_00638ba4[s.selection_index + 0x3e9] == 0)
      {
      }
      else
      {
        FUN_0049fdf9((HGDIOBJ)DAT_00638c40,
                     (HGDIOBJ)DAT_00638b68,
                     (HGDIOBJ)DAT_00638c44,
                     (HGDIOBJ)DAT_00638b70,
                     (HGDIOBJ)DAT_00638bf4);
        EndDialog(hwnd, s.selection_index);
      }
    }
    return 1;

  case WM_KEYDOWN:
    if (wparam_dc == VK_NEXT)
    {
      SendMessageA(hwnd, WM_VSCROLL, SB_PAGEDOWN, 0);
    }
    else if (wparam_dc == VK_PRIOR)
    {
      SendMessageA(hwnd, WM_VSCROLL, SB_PAGEUP, 0);
    }
    else if (wparam_dc == VK_DOWN)
    {
      SendMessageA(hwnd, WM_VSCROLL, SB_LINEDOWN, 0);
    }
    else if (wparam_dc == VK_UP)
    {
      SendMessageA(hwnd, WM_VSCROLL, SB_LINEUP, 0);
    }
    else if (wparam_dc == VK_ESCAPE)
    {
      SendMessageA(hwnd, WM_CLOSE, 0, 0);
    }
    return 1;

  case WM_VSCROLL:
    s.scroll_pos = GetScrollPos(hwnd, SB_VERT);
    GetScrollRange(hwnd, SB_VERT, &s.scroll_min, &s.scroll_max);
    GetClientRect(hwnd, &s.scroll_rect);
    s.scroll_frame_width = DAT_00638c84 + DAT_00638b34;
    s.client_bottom = s.scroll_rect.bottom - DAT_00638b34;

    switch ((unsigned int)wparam_dc & 0xffff)
    {
    case SB_LINEUP:
      s.new_scroll_pos = s.scroll_pos - s.scroll_frame_width;
      break;
    case SB_PAGEUP:
      s.new_scroll_pos = s.scroll_pos - s.client_bottom;
    case SB_THUMBPOSITION:
    case SB_THUMBTRACK:
      s.new_scroll_pos = HIWORD(wparam_dc);
      break;
    case SB_PAGEDOWN:
      s.new_scroll_pos = s.scroll_pos + s.client_bottom;
      break;
    case SB_LINEDOWN:
      s.new_scroll_pos = s.scroll_pos + s.scroll_frame_width;
      break;
    default:
      s.new_scroll_pos = s.scroll_pos;
      break;
    }

    if (s.scroll_min > s.new_scroll_pos)
    {
      s.new_scroll_pos = s.scroll_min;
    }

    if (s.scroll_max < s.new_scroll_pos)
    {
      s.new_scroll_pos = s.scroll_max;
    }

    ScrollWindow(hwnd, 0, -(s.new_scroll_pos - s.scroll_pos), 0, 0);
    SetScrollPos(hwnd, SB_VERT, s.new_scroll_pos, TRUE);
    return 1;

  case WM_QUERYNEWPALETTE:
  case WM_PALETTEISCHANGING:
  case WM_PALETTECHANGED:
    return handle_button_palette_message((int)hwnd, msg, (int)wparam_dc, (int)lparam_data);

  case WM_ERASEBKGND:
    s.erase_dc = (HDC)wparam_dc;
    ApplyCardArtPaletteToDc(s.erase_dc);
    GetClientRect(hwnd, &s.erase_rect);
    FillRect(s.erase_dc, &s.erase_rect, (HBRUSH)DAT_00638c40);
    return 1;

  case WM_NCPAINT:
  case WM_NCACTIVATE:
    s.has_selection = GetWindowLongA(hwnd, 8);
    GetWindowRect(hwnd, &s.window_rect);
    OffsetRect(&s.window_rect, -s.window_rect.left, -s.window_rect.top);
    if (s.window_rect.right - s.window_rect.left == 0 || s.window_rect.bottom - s.window_rect.top == 0)
    {
      return DefWindowProcA(hwnd, msg, wparam_dc, lparam_data);
    }

    if (msg == WM_NCPAINT)
    {
      DefWindowProcA(hwnd, msg, wparam_dc, lparam_data);
      s.current_button_state = 0;
    }
    else
    {
      s.current_button_state = 1;
    }
    s.dc = GetWindowDC(hwnd);
    if (s.dc != 0)
    {
      ApplyCardArtPaletteToDc(s.dc);
      GetWindowRect(hwnd, &s.window_rect);
      GetClientRect(hwnd, &s.client_rect);
      MapWindowPoints(hwnd, 0, (LPPOINT)&s.client_rect, 2);
      OffsetRect(&s.client_rect, -s.window_rect.left, -s.window_rect.top);
      OffsetRect(&s.window_rect, -s.window_rect.left, -s.window_rect.top);
      GetWindowTextA(hwnd, s.title_text, 100);
      s.right_border_width = s.client_rect.left - s.window_rect.left;
      s.bottom_border_height = s.window_rect.bottom - s.client_rect.bottom;
      SelectObject(s.dc, (HGDIOBJ)DAT_00638c44);
      s.frame_width = 0;
      MoveToEx(s.dc, 0, s.frame_width, 0);
      LineTo(s.dc, s.window_rect.right - 1, s.frame_width);
      SelectObject(s.dc, (HGDIOBJ)DAT_00638b68);
      for (s.row = 1, s.frame_width = 1; s.row <= s.bottom_border_height - 2; ++s.row, ++s.frame_width)
      {
        MoveToEx(s.dc, 1, s.frame_width, 0);
        LineTo(s.dc, (s.window_rect.right - s.right_border_width) + 1, s.frame_width);
      }
      SelectObject(s.dc, (HGDIOBJ)DAT_00638c44);
      s.frame_width = s.bottom_border_height - 1;
      MoveToEx(s.dc, s.right_border_width - 1, s.frame_width, 0);
      LineTo(s.dc, s.window_rect.right - s.right_border_width, s.frame_width);
      SelectObject(s.dc, (HGDIOBJ)DAT_00638c44);
      s.other_frame_width = 0;
      MoveToEx(s.dc, s.other_frame_width, 0, 0);
      LineTo(s.dc, s.other_frame_width, s.window_rect.bottom - 1);
      SelectObject(s.dc, (HGDIOBJ)DAT_00638b68);
      for (s.row = 1, s.other_frame_width = 1; s.row <= s.right_border_width - 2; ++s.row, ++s.other_frame_width)
      {
        MoveToEx(s.dc, s.other_frame_width, 1, 0);
        LineTo(s.dc, s.other_frame_width, s.window_rect.bottom - 1);
      }
      SelectObject(s.dc, (HGDIOBJ)DAT_00638c44);
      s.other_frame_width = s.client_rect.left - 1;
      MoveToEx(s.dc, s.other_frame_width, s.bottom_border_height - 1, 0);
      LineTo(s.dc, s.other_frame_width, s.client_rect.bottom + 1);
      SelectObject(s.dc, GetStockObject(7));
      s.other_frame_width = s.window_rect.right - 1;
      MoveToEx(s.dc, s.other_frame_width, 0, 0);
      LineTo(s.dc, s.other_frame_width, s.window_rect.bottom);
      SelectObject(s.dc, (HGDIOBJ)DAT_00638b70);
      for (s.row = 1, s.other_frame_width = s.window_rect.right - 2; s.row <= s.right_border_width - 2; ++s.row, --s.other_frame_width)
      {
        MoveToEx(s.dc, s.other_frame_width, 1, 0);
        LineTo(s.dc, s.other_frame_width, s.window_rect.bottom - 1);
      }
      SelectObject(s.dc, (HGDIOBJ)DAT_00638c44);
      s.other_frame_width = s.window_rect.right - s.right_border_width;
      MoveToEx(s.dc, s.other_frame_width, s.bottom_border_height - 1, 0);
      LineTo(s.dc, s.other_frame_width, s.client_rect.bottom + 1);
      SelectObject(s.dc, GetStockObject(7));
      s.frame_width = s.window_rect.bottom - 1;
      MoveToEx(s.dc, 0, s.frame_width, 0);
      LineTo(s.dc, s.window_rect.right, s.frame_width);
      SelectObject(s.dc, (HGDIOBJ)DAT_00638b70);

      s.frame_width = s.window_rect.bottom - 2;
      s.row = 1;
      for (; s.row <= s.bottom_border_height - 2; --s.frame_width, ++s.row)
      {
        MoveToEx(s.dc, 1, s.frame_width, 0);
        LineTo(s.dc, s.window_rect.right - 1, s.frame_width);
      }
      SelectObject(s.dc, (HGDIOBJ)DAT_00638c44);
      s.frame_width = s.window_rect.bottom - s.bottom_border_height;
      MoveToEx(s.dc, s.right_border_width - 1, s.frame_width, 0);
      LineTo(s.dc, s.window_rect.right - 2, s.frame_width);
      SelectObject(s.dc, (HGDIOBJ)DAT_00638c44);
      s.frame_width = s.client_rect.top - 1;
      MoveToEx(s.dc, s.client_rect.left, s.frame_width, 0);
      LineTo(s.dc, s.window_rect.right - s.right_border_width, s.frame_width);
      SetRect(&s.title_rect, s.client_rect.left, s.bottom_border_height, s.window_rect.right - s.right_border_width, s.client_rect.top - 1);
      FillRect(s.dc, &s.title_rect, (HBRUSH)DAT_00638bf4);
      SetTextColor(s.dc, DAT_00638c6c);
      SetBkMode(s.dc, 1);
      s.title_rect.left += 5;
      DrawTextA(s.dc, s.title_text, -1, &s.title_rect, DT_SINGLELINE | DT_VCENTER);
      if (s.has_selection == 0)
      {
        DrawTextA(s.dc, (LPCSTR)((char *)DAT_00638ba4 + 0x1780), -1, &s.title_rect, DT_RIGHT | DT_SINGLELINE | DT_VCENTER);
      }
      ReleaseDC(hwnd, s.dc);
    }

    SetWindowLongA(hwnd, 0, s.current_button_state);
    return 1;

  case WM_NCHITTEST:
    s.nc_calc_result = DefWindowProcA(hwnd, msg, wparam_dc, lparam_data);
    if (s.nc_calc_result == HTMINBUTTON || s.nc_calc_result == HTCAPTION)
    {
      s.screen_dc = GetDC(0);
      GetTextExtentPoint32A(s.screen_dc, (LPCSTR)((char *)DAT_00638ba4 + 0x1780), strlen((char *)DAT_00638ba4 + 0x1780), &s.text_extent);
      ReleaseDC(0, s.screen_dc);
      GetClientRect(hwnd, &s.hit_rect);
      MapWindowPoints(hwnd, 0, (LPPOINT)&s.hit_rect, 2);
      if (s.hit_rect.right - s.text_extent.cx <= (int)((unsigned long)lparam_data & 0xffff))
      {
        s.nc_calc_result = 200;
      }
    }
    SetWindowLongA(hwnd, 0, s.nc_calc_result);
    return 1;

  case WM_NCLBUTTONDOWN:
    s.unused_160 = (int)wparam_dc;
    if (s.unused_160 == 0xc8)
    {
      SendMessageA(hwnd, WM_CLOSE, 0, 0);
      s.long_result = 0;
    }
    else
    {
      s.long_result = DefWindowProcA(hwnd, msg, wparam_dc, lparam_data);
    }
    SetWindowLongA(hwnd, 0, s.long_result);
    return 1;

  default:
    return 0;
  }
}

// FUNCTION: MAGIC 0x0049fe68
// FUNCTION: SHANDALAR 0x0053b80b
LRESULT CALLBACK wndproc_ShowListCard(HWND card_window, UINT message, WPARAM wparam_window, LPARAM lparam_data)
{
  struct
  {
    HDC paint_dc;         // ebp-0x60
    PAINTSTRUCT ps;       // ebp-0x5c
    RECT client_rect;     // ebp-0x1c
    LONG show_count_flag; // ebp-0x0c
    LONG count;           // ebp-0x08
    LONG csvid;           // ebp-0x04
  } s;

  switch (message)
  {
  case 0x437:
    s.csvid = GetWindowLongA(card_window, g_showlist_card_csvid_window_long_offset);
    if (SHOWLIST_MOUSE_MODE != 2)
    {
      SendMessageA(g_duel_card_preview_window_hwnd, 0x401, s.csvid, 0);
    }
    return 0;

  case 0x414:
    s.show_count_flag = (LONG)wparam_window;
    s.count = (LONG)lparam_data;
    SetWindowLongA(card_window, g_showlist_card_show_count_window_long_offset, s.show_count_flag);
    SetWindowLongA(card_window, g_showlist_card_count_window_long_offset, s.count);
    InvalidateRect(card_window, NULL, TRUE);
    return 0;

  case WM_CREATE:
    s.csvid = *(LONG *)lparam_data;
    SetWindowLongA(card_window, g_showlist_card_csvid_window_long_offset, s.csvid);
    s.show_count_flag = 0;
    s.count = 0;
    SetWindowLongA(card_window, g_showlist_card_show_count_window_long_offset, s.show_count_flag);
    SetWindowLongA(card_window, g_showlist_card_count_window_long_offset, s.count);
    return 0;

  case WM_GETDLGCODE:
    return 4;

  case WM_KEYDOWN:
    SendMessageA(GetParent(card_window), message, wparam_window, lparam_data);
    return 0;

  case WM_LBUTTONDOWN:
    SendMessageA(GetParent(card_window),
                 WM_COMMAND,
                 ((unsigned int)(unsigned short)GetDlgCtrlID(card_window)) | 0x10000,
                 (LPARAM)card_window);
    return 0;

  case WM_MOUSEMOVE:
  case WM_RBUTTONDOWN:
    if ((message == WM_MOUSEMOVE && SHOWLIST_MOUSE_MODE != 2) || (message == WM_RBUTTONDOWN && SHOWLIST_MOUSE_MODE == 2))
    {
      s.csvid = GetWindowLongA(card_window, g_showlist_card_csvid_window_long_offset);
      if ((int)DAT_00638c08 != (int)card_window)
      {
        SendMessageA(g_duel_card_preview_window_hwnd, 0x401, s.csvid, 0);
        DAT_00638c08 = (int)card_window;
      }
    }
    return 0;

  case WM_PAINT:
    s.csvid = GetWindowLongA(card_window, g_showlist_card_csvid_window_long_offset);
    s.show_count_flag = GetWindowLongA(card_window, g_showlist_card_show_count_window_long_offset);
    s.count = GetWindowLongA(card_window, g_showlist_card_count_window_long_offset);

    EnterCriticalSection(&g_card_render_lock);
    GetClientRect(card_window, &s.client_rect);
    FillRect(g_shared_offscreen_dc, &s.client_rect, (HBRUSH)GetStockObject(4));
    if (s.csvid == SHOWLIST_CARD_BACK_CSVID)
    {
      DrawCardBack(g_shared_offscreen_dc, &s.client_rect);
    }
    else
    {
      DrawSmallCard(g_shared_offscreen_dc, &s.client_rect, global_raw_cards_storage + s.csvid, 0, 0);
    }

    if (s.show_count_flag != 0)
    {
      FUN_0055b9f0((int)g_shared_offscreen_dc, (int *)&s.client_rect, s.count);
    }

    s.paint_dc = BeginPaint(card_window, &s.ps);
    if (s.paint_dc != 0)
    {
      ApplyCardArtPaletteToDc(s.paint_dc);
      BitBlt(s.paint_dc,
             0,
             0,
             s.client_rect.right,
             s.client_rect.bottom,
             g_shared_offscreen_dc,
             0,
             0,
             0xcc0020);
      EndPaint(card_window, &s.ps);
    }
    LeaveCriticalSection(&g_card_render_lock);
    return 0;

  case WM_QUERYNEWPALETTE:
  case WM_PALETTEISCHANGING:
  case WM_PALETTECHANGED:
    return handle_button_palette_message((int)card_window, message, (int)wparam_window, (int)lparam_data);

  default:
    return DefWindowProcA(card_window, message, wparam_window, lparam_data);
  }

  return 0;
}

// FUNCTION: MAGIC 0x0049e6aa
// FUNCTION: SHANDALAR 0x0053a054
int show_cardlist(int *graveyard,
                  int *card_counts,
                  int *available,
                  int count,
                  void *context,
                  unsigned int big_card_mode,
                  char *prompt)
{
  struct
  {
    void *dialog_context;
    int displayed_csvids[500];
    int card_counts[500];
    int selectable_cards[500];
    int item_count;
    unsigned int show_card_counts;
    int show_bigcard;
    char title[12];
    int index;
  } s;

  WNDCLASS wndclass;

  if (count < 1 || graveyard == NULL || graveyard[0] == -1)
  {
    return -1;
  }

  wndclass.style = 0;
  wndclass.lpfnWndProc = wndproc_ShowListCard;
  wndclass.cbClsExtra = 0;
  wndclass.cbWndExtra = g_showlist_card_window_extra_bytes;
  wndclass.hInstance = g_app_instance;
  wndclass.hIcon = LoadIconA(0, (const char *)0x7f00);
  wndclass.hCursor = LoadCursorA(0, (const char *)0x7f00);
  wndclass.hbrBackground = (HBRUSH)6;
  wndclass.lpszMenuName = 0;
  wndclass.lpszClassName = "ShowListCard";
  RegisterClassA(&wndclass);

  s.dialog_context = context;
  for (s.index = 0; s.index < count && graveyard[s.index] != -1; ++s.index)
  {
    s.displayed_csvids[s.index] = CardIDFromType(graveyard[s.index] & 0xfff);
  }

  s.item_count = s.index;
  s.show_card_counts = card_counts != 0;
  if (s.show_card_counts != 0)
  {
    for (s.index = 0; s.index < s.item_count; ++s.index)
    {
      s.card_counts[s.index] = card_counts[s.index];
    }
  }

  for (s.index = 0; s.index < s.item_count; ++s.index)
  {
    if (available == 0)
    {
      s.selectable_cards[s.index] = 1;
    }
    else
    {
      s.selectable_cards[s.index] = available[s.index];
    }
  }

  s.show_bigcard = big_card_mode;
  if (big_card_mode == 0)
  {
    strcpy(s.title, prompt);
  }
  else
  {
    strcpy(s.title, "");
  }

  return DialogBoxParam(g_app_instance, (const char *)0xe9, g_duel_window_hwnd, dlgfunc_show_deck, (long)&s.dialog_context);
}

// FUNCTION: MAGIC 0x0055b9f0
// FUNCTION: SHANDALAR 0x00571d0e
void FUN_0055b9f0(int dc, int *rect, int value)
{
  struct
  {
    char text[12];
    int text_y;
    int text_x;
    int saved_dc;
  } s;

  if (dc == 0 || rect == NULL)
    return;

  s.saved_dc = SaveDC((HDC)dc);
  SetMapMode((HDC)dc, 8);
  SetWindowExtEx((HDC)dc, 100, 0x8c, NULL);
  SetViewportExtEx((HDC)dc, rect[2] - rect[0], rect[3] - rect[1], NULL);
  SetWindowOrgEx((HDC)dc, 0, 0, NULL);
  SetViewportOrgEx((HDC)dc, rect[0], rect[1], NULL);
  sprintf(s.text, "%d", value);
  SelectObject((HDC)dc, (HGDIOBJ)global_smallcard_pt_font);
  SetTextAlign((HDC)dc, 10);
  SetBkMode((HDC)dc, 1);
  s.text_x = 100;
  s.text_y = 0x8c;
  SetTextColor((HDC)dc, global_palette_col_c9);
  TextOutA((HDC)dc, s.text_x - 1, s.text_y - 1, s.text, strlen(s.text));
  SetTextColor((HDC)dc, global_palette_col_9e_b);
  TextOutA((HDC)dc, s.text_x - 3, s.text_y - 3, s.text, strlen(s.text));
  RestoreDC((HDC)dc, s.saved_dc);
}
