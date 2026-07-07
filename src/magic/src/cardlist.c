#include <stddef.h>
#include <string.h>
#include <windows.h>
#include "cardartlib/src/palette.h"
#include "deckdll/src/shared_resources.h"
#include "drawcardlib/Drawcardlib.h"
#include "game_support.h"
#include "global_strings.h"

typedef ptrdiff_t INT_PTR;

// GLOBAL: MAGIC 0x00572920
// GLOBAL: SHANDALAR 0x0058f448
char s_ShowListCard_00572920[16] = "ShowListCard";
// GLOBAL: MAGIC 0x0055e0c0
int unk_0055e0c0;
// GLOBAL: MAGIC 0x0055e0c4
int unk_0055e0c4;
// GLOBAL: MAGIC 0x0055e0c8
int unk_0055e0c8;
// GLOBAL: MAGIC 0x0055e0cc
int unk_0055e0cc;
// GLOBAL: MAGIC 0x00572930
// GLOBAL: SHANDALAR 0x0058f438
char unk_00572930[16] = "\0\0\0\0List Card";

// GLOBAL: MAGIC 0x008a9190
// GLOBAL: SHANDALAR 0x008bd390
int g_showlist_smallcard_width;

// GLOBAL: MAGIC 0x008cf1b0
// GLOBAL: SHANDALAR 0x008e3300
int g_showlist_smallcard_height;

// GLOBAL: MAGIC 0x00638b34
int DAT_00638b34;
// GLOBAL: MAGIC 0x00638b48
int DAT_00638b48;
// GLOBAL: MAGIC 0x00638b68
int DAT_00638b68;
// GLOBAL: MAGIC 0x00638b70
int DAT_00638b70;
// GLOBAL: MAGIC 0x00638b80
int DAT_00638b80;
// GLOBAL: MAGIC 0x00638ba4
int *DAT_00638ba4;
// GLOBAL: MAGIC 0x00638bf4
int DAT_00638bf4;
// GLOBAL: MAGIC 0x00638c08
int DAT_00638c08;
// GLOBAL: MAGIC 0x00638c40
int DAT_00638c40;
// GLOBAL: MAGIC 0x00638c44
int DAT_00638c44;
// GLOBAL: MAGIC 0x00638c6c
int DAT_00638c6c;
// GLOBAL: MAGIC 0x00638c84
int DAT_00638c84;

// GLOBAL: MAGIC 0x00896714
// GLOBAL: SHANDALAR 0x008aa914
HWND DAT_00896714;

extern HPALETTE global_cart_art_hpalette;
extern card_ptr_t global_raw_cards_storage[2000];
extern CRITICAL_SECTION g_card_render_lock;
extern HDC g_shared_offscreen_dc;
extern HINSTANCE g_app_instance;
extern HWND g_main_window_hwnd;

void FUN_0055b9f0(int dc, int *rect, int value);

// TODO: cleanup this bucket of shit
#ifdef SHANDALAR
#define SHOWLIST_CARD_BACK_CSVID (*(int *)&gs_phasebar_your_main_postcombat_00926670[0x60])
#define SHOWLIST_MOUSE_MODE g_duel_interface_options.layout
#else
extern int DAT_0091c998;
#define SHOWLIST_CARD_BACK_CSVID unk_009266ac
#define SHOWLIST_MOUSE_MODE DAT_0091c998
#endif

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
  UINT command;
  int button_index;
  int button_x;
  int button_y;
  int columns;
  int rows;
  int title_bar_height;
  int client_bottom;
  int frame_width;
  int row;
  int right_border_width;
  int bottom_border_height;
  int window_style;
  int scroll_pos;
  int scroll_min;
  int scroll_max;
  int new_scroll_pos;
  int current_button_state;
  int destroy_result;
  int nc_calc_result;
  int client_top_limit;
  int width;
  int height;
  LONG long_result;
  RECT rect;
  RECT client_rect;
  RECT window_rect;
  RECT title_rect;
  RECT scroll_rect;
  RECT erase_rect;
  SIZE text_extent;
  HDC dc;
  HDC screen_dc;
  HGDIOBJ old_obj;
  HWND card_window;
  LONG has_selection;
  DWORD style;
  UINT flags;
  BOOL has_menu;
  char title_text[100];
  int *dialog_data;

  if (msg < 0x15)
  {
    if (msg == 0x14)
    {
      dc = (HDC)wparam_dc;
      ApplyCardArtPaletteToDc(dc);
      GetClientRect(hwnd, &erase_rect);
      FillRect(dc, &erase_rect, (HBRUSH)DAT_00638c40);
      return 1;
    }
    if (msg == 0x10)
    {
    destroy_window:
      destroy_result = GetWindowLongA(hwnd, 8);
      if (destroy_result == 0)
      {
        FUN_0049fdf9(DAT_00638c40, DAT_00638b68, DAT_00638c44, DAT_00638b70, DAT_00638bf4);
        EndDialog(hwnd, -1);
      }
      return 1;
    }
  }
  else if (msg < 0xa2)
  {
    if (msg == 0xa1)
    {
      if ((HDC)wparam_dc == (HDC)0xc8)
      {
        SendMessageA(hwnd, 0x10, 0, 0);
        long_result = 0;
      }
      else
      {
        long_result = DefWindowProcA(hwnd, 0xa1, wparam_dc, lparam_data);
      }
      SetWindowLongA(hwnd, 0, long_result);
      return 1;
    }
    if (msg == 0x84)
    {
      nc_calc_result = DefWindowProcA(hwnd, 0x84, wparam_dc, lparam_data);
      if (nc_calc_result == 8 || nc_calc_result == 2)
      {
        screen_dc = GetDC(0);
        GetTextExtentPoint32A(screen_dc, (LPCSTR)((char *)DAT_00638ba4 + 0x1780), strlen((char *)DAT_00638ba4 + 0x1780), &text_extent);
        ReleaseDC(0, screen_dc);
        GetClientRect(hwnd, &rect);
        MapWindowPoints(hwnd, 0, (LPPOINT)&rect, 2);
        if (rect.right - text_extent.cx <= (int)((unsigned long)lparam_data & 0xffff))
        {
          nc_calc_result = 200;
        }
      }
      SetWindowLongA(hwnd, 0, nc_calc_result);
      return 1;
    }
    if (msg > 0x84 && msg < 0x87)
    {
      has_selection = GetWindowLongA(hwnd, 8);
      GetWindowRect(hwnd, &window_rect);
      OffsetRect(&window_rect, -window_rect.left, -window_rect.top);
      if (window_rect.right != window_rect.left && window_rect.bottom != window_rect.top)
      {
        if (msg == 0x85)
        {
          DefWindowProcA(hwnd, 0x85, wparam_dc, lparam_data);
        }
        current_button_state = (msg != 0x85);
        dc = GetWindowDC(hwnd);
        if (dc != 0)
        {
          ApplyCardArtPaletteToDc(dc);
          GetWindowRect(hwnd, &window_rect);
          GetClientRect(hwnd, &client_rect);
          MapWindowPoints(hwnd, 0, (LPPOINT)&client_rect, 2);
          OffsetRect(&client_rect, -window_rect.left, -window_rect.top);
          OffsetRect(&window_rect, -window_rect.left, -window_rect.top);
          GetWindowTextA(hwnd, title_text, 100);
          right_border_width = client_rect.left - window_rect.left;
          bottom_border_height = window_rect.bottom - client_rect.bottom;
          SelectObject(dc, (HGDIOBJ)DAT_00638c44);
          MoveToEx(dc, 0, 0, 0);
          LineTo(dc, window_rect.right - 1, 0);
          SelectObject(dc, (HGDIOBJ)DAT_00638b68);
          for (row = 1; row <= bottom_border_height - 2; ++row)
          {
            MoveToEx(dc, 1, row, 0);
            LineTo(dc, (window_rect.right - right_border_width) + 1, row);
          }
          SelectObject(dc, (HGDIOBJ)DAT_00638c44);
          MoveToEx(dc, right_border_width - 1, bottom_border_height - 1, 0);
          LineTo(dc, window_rect.right - right_border_width, bottom_border_height - 1);
          SelectObject(dc, (HGDIOBJ)DAT_00638c44);
          MoveToEx(dc, 0, 0, 0);
          LineTo(dc, 0, window_rect.bottom - 1);
          SelectObject(dc, (HGDIOBJ)DAT_00638b68);
          for (row = 1; row <= right_border_width - 2; ++row)
          {
            MoveToEx(dc, row, 1, 0);
            LineTo(dc, row, window_rect.bottom - 1);
          }
          SelectObject(dc, (HGDIOBJ)DAT_00638c44);
          MoveToEx(dc, client_rect.left - 1, bottom_border_height - 1, 0);
          LineTo(dc, client_rect.left - 1, client_rect.bottom + 1);
          old_obj = GetStockObject(7);
          SelectObject(dc, old_obj);
          MoveToEx(dc, window_rect.right - 1, 0, 0);
          LineTo(dc, window_rect.right - 1, window_rect.bottom);
          SelectObject(dc, (HGDIOBJ)DAT_00638b70);
          for (row = 1, frame_width = window_rect.right - 2; row <= right_border_width - 2; ++row, --frame_width)
          {
            MoveToEx(dc, frame_width, 1, 0);
            LineTo(dc, frame_width, window_rect.bottom - 1);
          }
          SelectObject(dc, (HGDIOBJ)DAT_00638c44);
          MoveToEx(dc, window_rect.right - right_border_width, bottom_border_height - 1, 0);
          LineTo(dc, window_rect.right - right_border_width, client_rect.bottom + 1);
          old_obj = GetStockObject(7);
          SelectObject(dc, old_obj);
          MoveToEx(dc, 0, window_rect.bottom - 1, 0);
          LineTo(dc, window_rect.right, window_rect.bottom - 1);
          SelectObject(dc, (HGDIOBJ)DAT_00638b70);
          for (row = 1, frame_width = window_rect.bottom - 2; row <= bottom_border_height - 2; ++row, --frame_width)
          {
            MoveToEx(dc, 1, frame_width, 0);
            LineTo(dc, window_rect.right - 1, frame_width);
          }
          SelectObject(dc, (HGDIOBJ)DAT_00638c44);
          MoveToEx(dc, right_border_width - 1, window_rect.bottom - bottom_border_height, 0);
          LineTo(dc, window_rect.right - 2, window_rect.bottom - bottom_border_height);
          SelectObject(dc, (HGDIOBJ)DAT_00638c44);
          MoveToEx(dc, client_rect.left, client_rect.top - 1, 0);
          LineTo(dc, window_rect.right - right_border_width, client_rect.top - 1);
          SetRect(&title_rect, client_rect.left, bottom_border_height, window_rect.right - right_border_width, client_rect.top - 1);
          FillRect(dc, &title_rect, (HBRUSH)DAT_00638bf4);
          SetTextColor(dc, DAT_00638c6c);
          SetBkMode(dc, 1);
          title_rect.left += 5;
          DrawTextA(dc, title_text, -1, &title_rect, 0x24);
          if (has_selection == 0)
          {
            DrawTextA(dc, (LPCSTR)((char *)DAT_00638ba4 + 0x1780), -1, &title_rect, 0x26);
          }
          ReleaseDC(hwnd, dc);
        }
        SetWindowLongA(hwnd, 0, current_button_state);
        return 1;
      }
      return DefWindowProcA(hwnd, msg, wparam_dc, lparam_data);
    }
  }
  else if (msg < 0x111)
  {
    if (msg == 0x110)
    {
      dialog_data = (int *)lparam_data;
      DAT_00638ba4 = dialog_data;
      SetWindowLongA(hwnd, 8, dialog_data[0x5df]);
      DAT_00638c08 = 0;
      FUN_0049fd0c(&DAT_00638c40, &DAT_00638b68, &DAT_00638c44, &DAT_00638b70, &DAT_00638bf4, &DAT_00638c6c);
      SetWindowTextA(hwnd, (LPCSTR)*DAT_00638ba4);
      columns = DAT_00638ba4[0x5dd];
      DAT_00638b80 = (g_showlist_smallcard_width * 2) / 3;
      DAT_00638c84 = (g_showlist_smallcard_height * 2) / 3;
      DAT_00638b48 = 8;
      DAT_00638b34 = 8;
      rows = 6;
      if (columns % 6 == 1)
      {
        rows = 5;
      }
      title_bar_height = columns / rows;
      if (columns % rows > 0)
      {
        ++title_bar_height;
      }
      client_top_limit = 4;
      SetRect(&rect, 0, 0, (DAT_00638b80 + 8) * rows + 8, (DAT_00638c84 + 8) * title_bar_height + 8);
      style = GetWindowLongA(hwnd, -0x10);
      SetWindowLongA(hwnd, -0x10, style & 0xffdfffff);
      if (client_top_limit < title_bar_height)
      {
        style = GetWindowLongA(hwnd, -0x10);
        SetWindowLongA(hwnd, -0x10, style | 0x200000);
        SetScrollRange(hwnd, 1, 0, rect.bottom - ((DAT_00638c84 + DAT_00638b34) * client_top_limit + DAT_00638b34), 1);
        SetScrollPos(hwnd, 1, 0, 1);
        rect.bottom = (DAT_00638c84 + DAT_00638b34) * client_top_limit + rect.top + DAT_00638b34;
        rect.right += GetSystemMetrics(2);
      }
      has_menu = 0;
      window_style = GetWindowLongA(hwnd, -0x10);
      AdjustWindowRect(&rect, window_style, has_menu);
      flags = 4;
      height = rect.bottom - rect.top;
      width = rect.right - rect.left;
      button_y = (GetSystemMetrics(1) - (rect.bottom - rect.top)) / 2;
      button_x = (GetSystemMetrics(0) - (rect.right - rect.left)) / 2;
      SetWindowPos(hwnd, 0, button_x, button_y, width, height, flags);
      button_x = DAT_00638b48;
      button_y = DAT_00638b34;
      GetClientRect(hwnd, &rect);
      for (button_index = 0; button_index < DAT_00638ba4[0x5dd]; ++button_index)
      {
        card_window = CreateWindowExA(0, s_ShowListCard_00572920, unk_00572930 + 4, 0x50000000, button_x, button_y, DAT_00638b80, DAT_00638c84, hwnd, (HMENU)(button_index + 10),
                                      g_app_instance, (LPVOID)DAT_00638ba4[button_index + 1]);
        if (DAT_00638ba4[0x5de] != 0)
        {
          SendMessageA(card_window, 0x414, 1, DAT_00638ba4[button_index + 0x1f5]);
        }
        button_x += DAT_00638b48 + DAT_00638b80;
        if (rect.right < button_x + DAT_00638b80)
        {
          button_y += DAT_00638c84 + DAT_00638b34;
          button_x = DAT_00638b48;
        }
      }
      SetFocus(hwnd);
      return 0;
    }
    if (msg == 0x100)
    {
      if ((HDC)wparam_dc == (HDC)0x22)
      {
        SendMessageA(hwnd, 0x115, 3, 0);
      }
      else if ((HDC)wparam_dc == (HDC)0x21)
      {
        SendMessageA(hwnd, 0x115, 2, 0);
      }
      else if ((HDC)wparam_dc == (HDC)0x28)
      {
        SendMessageA(hwnd, 0x115, 1, 0);
      }
      else if ((HDC)wparam_dc == (HDC)0x26)
      {
        SendMessageA(hwnd, 0x115, 0, 0);
      }
      else if ((HDC)wparam_dc == (HDC)0x1b)
      {
        SendMessageA(hwnd, 0x10, 0, 0);
      }
      return 1;
    }
  }
  else if (msg < 0x116)
  {
    if (msg == 0x115)
    {
      scroll_pos = GetScrollPos(hwnd, 1);
      GetScrollRange(hwnd, 1, &scroll_min, &scroll_max);
      GetClientRect(hwnd, &scroll_rect);
      frame_width = DAT_00638c84 + DAT_00638b34;
      client_bottom = scroll_rect.bottom - DAT_00638b34;
      switch ((unsigned int)wparam_dc & 0xffff)
      {
      case 0:
        new_scroll_pos = scroll_pos - frame_width;
        break;
      case 1:
        new_scroll_pos = scroll_pos + frame_width;
        break;
      case 2:
      case 4:
      case 5:
        new_scroll_pos = (unsigned int)wparam_dc >> 16;
        break;
      case 3:
        new_scroll_pos = scroll_pos + client_bottom;
        break;
      default:
        new_scroll_pos = scroll_pos;
        break;
      }
      if (new_scroll_pos < scroll_min)
      {
        new_scroll_pos = scroll_min;
      }
      if (new_scroll_pos > scroll_max)
      {
        new_scroll_pos = scroll_max;
      }
      ScrollWindow(hwnd, 0, -(new_scroll_pos - scroll_pos), 0, 0);
      SetScrollPos(hwnd, 1, new_scroll_pos, 1);
      return 1;
    }
    if (msg == 0x111)
    {
      command = (unsigned int)wparam_dc & 0xffff;
      dialog_data = (int *)lparam_data;
      has_selection = GetWindowLongA(hwnd, 8);
      if (command == 2 || command == 1)
      {
        if (has_selection == 0)
        {
          FUN_0049fdf9(DAT_00638c40, DAT_00638b68, DAT_00638c44, DAT_00638b70, DAT_00638bf4);
          EndDialog(hwnd, -1);
        }
      }
      else if (dialog_data != 0 && *((int *)DAT_00638ba4 + command + 0x3df) != 0)
      {
        button_index = command - 10;
        FUN_0049fdf9(DAT_00638c40, DAT_00638b68, DAT_00638c44, DAT_00638b70, DAT_00638bf4);
        EndDialog(hwnd, button_index);
      }
      return 1;
    }
  }
  else
  {
    if (msg == 0x201)
    {
      goto destroy_window;
    }
    if (msg > 0x30e && msg < 0x312)
    {
      return FUN_10025b5e((int)hwnd, msg, (int)wparam_dc, (int)lparam_data);
    }
  }

  return 0;
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
  case WM_PAINT:
    s.csvid = GetWindowLongA(card_window, unk_0055e0c0);
    s.show_count_flag = GetWindowLongA(card_window, unk_0055e0c8);
    s.count = GetWindowLongA(card_window, unk_0055e0c4);

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
      FUN_0055b9f0(g_shared_offscreen_dc, (int *)&s.client_rect, s.count);
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

  case WM_CREATE:
    s.csvid = *(LONG *)lparam_data;
    SetWindowLongA(card_window, unk_0055e0c0, s.csvid);
    s.show_count_flag = 0;
    s.count = 0;
    SetWindowLongA(card_window, unk_0055e0c8, s.show_count_flag);
    SetWindowLongA(card_window, unk_0055e0c4, s.count);
    return 0;

  case WM_GETDLGCODE:
    return 4;

  case WM_KEYDOWN:
    SendMessageA(GetParent(card_window), message, wparam_window, lparam_data);
    return 0;

  case WM_QUERYNEWPALETTE:
  case WM_PALETTEISCHANGING:
  case WM_PALETTECHANGED:
    return FUN_10025b5e((int)card_window, message, (int)wparam_window, (int)lparam_data);

  case WM_MOUSEMOVE:
  case WM_RBUTTONDOWN:
    if ((message == WM_MOUSEMOVE && SHOWLIST_MOUSE_MODE != 2) || (message == WM_RBUTTONDOWN && SHOWLIST_MOUSE_MODE == 2))
    {
      s.csvid = GetWindowLongA(card_window, unk_0055e0c0);
      if ((int)DAT_00638c08 != (int)card_window)
      {
        SendMessageA(DAT_00896714, 0x401, s.csvid, 0);
        DAT_00638c08 = (int)card_window;
      }
    }
    return 0;

  case WM_LBUTTONDOWN:
    SendMessageA(GetParent(card_window),
                 WM_COMMAND,
                 ((unsigned int)(unsigned short)GetDlgCtrlID(card_window)) | 0x10000,
                 (LPARAM)card_window);
    return 0;

  case 0x414:
    s.show_count_flag = (LONG)wparam_window;
    s.count = (LONG)lparam_data;
    SetWindowLongA(card_window, unk_0055e0c8, s.show_count_flag);
    SetWindowLongA(card_window, unk_0055e0c4, s.count);
    InvalidateRect(card_window, NULL, TRUE);
    return 0;

  case 0x437:
    s.csvid = GetWindowLongA(card_window, unk_0055e0c0);
    if (SHOWLIST_MOUSE_MODE != 2)
    {
      SendMessageA(DAT_00896714, 0x401, s.csvid, 0);
    }
    return 0;

  default:
    return DefWindowProcA(card_window, message, wparam_window, lparam_data);
  }
}

// FUNCTION: MAGIC 0x0049e6aa
// FUNCTION: SHANDALAR 0x0053a054
int show_cardlist(int *graveyard,
                  int *alternate_csvids,
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
    int available_cards[500];
    int item_count;
    unsigned int copy_alternate_csvids;
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
  wndclass.cbWndExtra = unk_0055e0cc;
  wndclass.hInstance = g_app_instance;
  wndclass.hIcon = LoadIconA(0, (const char *)0x7f00);
  wndclass.hCursor = LoadCursorA(0, (const char *)0x7f00);
  wndclass.hbrBackground = 6;
  wndclass.lpszMenuName = 0;
  wndclass.lpszClassName = s_ShowListCard_00572920;
  RegisterClassA(&wndclass);

  s.dialog_context = context;
  for (s.index = 0; s.index < count && graveyard[s.index] != -1; ++s.index)
  {
    s.displayed_csvids[s.index] = CardIDFromType(graveyard[s.index] & 0xfff);
  }

  s.item_count = s.index;
  s.copy_alternate_csvids = alternate_csvids != 0;
  if (s.copy_alternate_csvids != 0)
  {
    for (s.index = 0; s.index < s.item_count; ++s.index)
    {
      s.displayed_csvids[s.index] = alternate_csvids[s.index];
    }
  }

  for (s.index = 0; s.index < s.item_count; ++s.index)
  {
    if (available == 0)
    {
      s.available_cards[s.index] = 1;
    }
    else
    {
      s.available_cards[s.index] = available[s.index];
    }
  }

  s.show_bigcard = big_card_mode;
  if (big_card_mode == 0)
  {
    strcpy(s.title, prompt);
  }
  else
  {
    strcpy(s.title, unk_00572930);
  }

  return DialogBoxParam(g_app_instance, (const char *)0xe9, g_main_window_hwnd, dlgfunc_show_deck, (long)&s.dialog_context);
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
