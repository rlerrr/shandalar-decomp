#include <stddef.h>
#include <string.h>
#include <windows.h>
#include "cardartlib/src/palette.h"
#include "game_support.h"

typedef ptrdiff_t INT_PTR;

// GLOBAL: MAGIC 0x00572920
char s_ShowListCard_00572920[16] = "ShowListCard";
// GLOBAL: MAGIC 0x0055e0cc
int unk_0055e0cc;
// GLOBAL: MAGIC 0x00572930
char unk_00572930[16];

// GLOBAL: MAGIC 0x008a9190
int DAT_008a9190;
// GLOBAL: MAGIC 0x008cf1b0
int DAT_008cf1b0;

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

extern HPALETTE global_cart_art_hpalette;

// FUNCTION: MAGIC 0x0049fd0c
void FUN_0049fd0c(int *brush1, int *pen1, int *pen2, int *pen3, int *brush2, int *text_color)
{
  if (brush1 != NULL)
  {
    *brush1 = 2;
  }
  if (pen1 != NULL)
  {
    *pen1 = 6;
  }
  if (pen2 != NULL)
  {
    *pen2 = 6;
  }
  if (pen3 != NULL)
  {
    *pen3 = 7;
  }
  if (brush2 != NULL)
  {
    *brush2 = 2;
  }
  if (text_color != NULL)
  {
    *text_color = 0x1000090;
  }
}

// FUNCTION: MAGIC 0x0049fdf9
void FUN_0049fdf9(int brush1, int pen1, int pen2, int pen3, int brush2)
{
  (void)brush1;
  (void)pen1;
  (void)pen2;
  (void)pen3;
  (void)brush2;
}


// FUNCTION: MAGIC 0x0049ebde
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
  int* dialog_data;

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
        GetTextExtentPoint32A(screen_dc, (LPCSTR)((char*)DAT_00638ba4 + 0x1780), strlen((char*)DAT_00638ba4 + 0x1780), &text_extent);
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
            DrawTextA(dc, (LPCSTR)((char*)DAT_00638ba4 + 0x1780), -1, &title_rect, 0x26);
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
      dialog_data = (int*)lparam_data;
      DAT_00638ba4 = dialog_data;
      SetWindowLongA(hwnd, 8, dialog_data[0x5df]);
      DAT_00638c08 = 0;
      FUN_0049fd0c(&DAT_00638c40, &DAT_00638b68, &DAT_00638c44, &DAT_00638b70, &DAT_00638bf4, &DAT_00638c6c);
      SetWindowTextA(hwnd, (LPCSTR)*DAT_00638ba4);
      columns = DAT_00638ba4[0x5dd];
      DAT_00638b80 = (DAT_008a9190 * 2) / 3;
      DAT_00638c84 = (DAT_008cf1b0 * 2) / 3;
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
                                      global_hinstance, (LPVOID)DAT_00638ba4[button_index + 1]);
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
      dialog_data = (int*)lparam_data;
      has_selection = GetWindowLongA(hwnd, 8);
      if (command == 2 || command == 1)
      {
        if (has_selection == 0)
        {
          FUN_0049fdf9(DAT_00638c40, DAT_00638b68, DAT_00638c44, DAT_00638b70, DAT_00638bf4);
          EndDialog(hwnd, -1);
        }
      }
      else if (dialog_data != 0 && *((int*)DAT_00638ba4 + command + 0x3df) != 0)
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
      return FUN_004962cc((int)hwnd, msg, (int)wparam_dc, (int)lparam_data);
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x00496489
BOOL CALLBACK FUN_10025d1b(HWND child_hwnd, LPARAM lparam)
{
  if (GetParent(child_hwnd) == *(HWND*)lparam)
  {
    SendMessageA(child_hwnd, *(UINT*)(lparam + 4), *(WPARAM*)(lparam + 8), *(LPARAM*)(lparam + 12));
  }

  return 1;
}

// FUNCTION: MAGIC 0x004962cc
int FUN_004962cc(int window, unsigned int message, int other_window, int data)
{
  HDC dc;
  DWORD source_process_id;
  DWORD target_process_id;
  HWND hwnd;
  HWND other_hwnd;
  struct
  {
    HWND hwnd;
    UINT message;
    HWND other_window;
    LPARAM data;
  } forwarded_message;

  hwnd = (HWND)window;
  other_hwnd = (HWND)other_window;
  if (message == 0x30f)
  {
    UnrealizeObject((HGDIOBJ)global_cart_art_hpalette);
    dc = GetDC(hwnd);
    SelectPalette(dc, (HPALETTE)global_cart_art_hpalette, 0);
    if (RealizePalette(dc) != 0)
    {
      InvalidateRect(hwnd, 0, 1);
    }
    ReleaseDC(hwnd, dc);
    return 1;
  }

  if (message < 0x310 || message > 0x311)
  {
    return 0;
  }

  if (other_hwnd != hwnd)
  {
    GetWindowThreadProcessId(other_hwnd, &source_process_id);
    GetWindowThreadProcessId(hwnd, &target_process_id);
    if (source_process_id == target_process_id)
    {
      if (GetParent(hwnd) == 0)
      {
        dc = GetDC(hwnd);
        SelectPalette(dc, (HPALETTE)global_cart_art_hpalette, 1);
        InvalidateRect(hwnd, 0, 1);
        ReleaseDC(hwnd, dc);
      }
      else if ((GetWindowLongA(hwnd, -0x10) & 0x40000000) == 0)
      {
        InvalidateRect(hwnd, 0, 1);
      }
    }
    else
    {
      InvalidateRect(hwnd, 0, 1);
    }
  }

  if (message == 0x311)
  {
    forwarded_message.hwnd = hwnd;
    forwarded_message.message = message;
    forwarded_message.other_window = other_hwnd;
    forwarded_message.data = data;
    EnumChildWindows(hwnd, FUN_10025d1b, (LPARAM)&forwarded_message);
  }

  return 0;
}

// FUNCTION: MAGIC 0x0049fe68
LRESULT CALLBACK wndproc_ShowListCard(HWND card_window, UINT message, WPARAM wparam_window, LPARAM lparam_data)
{
  (void)card_window;
  (void)wparam_window;
  (void)lparam_data;

  if (message < 0x10)
  {
    if (message == 1)
    {
      return 0;
    }
  }
  else if (message < 0x101)
  {
    if (message == 0x100)
    {
      return 0;
    }
    if (message == 0x87)
    {
      return 4;
    }
  }
  else
  {
    if (message < 0x312)
    {
      if (message == 0x201)
      {
        return 0;
      }
      if (message > 0x30e)
      {
        return 0;
      }
    }
    else
    {
      if (message == 0x414)
      {
        return 0;
      }
      if (message == 0x437)
      {
        return 0;
      }
    }
  }

  return 0;
}


// FUNCTION: MAGIC 0x0049e6aa
int show_cardlist(int *graveyard,
                 int *alternate_csvids,
                 int *available,
                 int count,
                 void *context,
                 unsigned int big_card_mode,
                 char *prompt)
{
  typedef struct
  {
    void *context;
    int displayed_csvids[500];
    int available_cards[500];
    int item_count;
    unsigned int copy_alternate_csvids;
    int show_bigcard;
    char title[12];
    int index;
  } dialog_box_data_t;

  dialog_box_data_t locals;
  WNDCLASS wndclass;

  if (count < 1 || graveyard == NULL || graveyard[0] == -1)
  {
    return -1;
  }

  wndclass.style = 0;
  wndclass.lpfnWndProc = wndproc_ShowListCard;
  wndclass.cbClsExtra = 0;
  wndclass.cbWndExtra = unk_0055e0cc;
  wndclass.hInstance = global_hinstance;
  wndclass.hIcon = LoadIconA(0, (const char *)0x7f00);
  wndclass.hCursor = LoadCursorA(0, (const char *)0x7f00);
  wndclass.hbrBackground = 6;
  wndclass.lpszMenuName = 0;
  wndclass.lpszClassName = s_ShowListCard_00572920;
  RegisterClassA(&wndclass);

  locals.context = context;
  for (locals.index = 0; locals.index < count && graveyard[locals.index] != -1; ++locals.index)
  {
    locals.displayed_csvids[locals.index] = CardIDFromType(graveyard[locals.index] & 0xfff);
  }

  locals.item_count = locals.index;
  locals.copy_alternate_csvids = alternate_csvids != 0;
  if (locals.copy_alternate_csvids != 0)
  {
    for (locals.index = 0; locals.index < locals.item_count; ++locals.index)
    {
      locals.displayed_csvids[locals.index] = alternate_csvids[locals.index];
    }
  }

  for (locals.index = 0; locals.index < locals.item_count; ++locals.index)
  {
    if (available == 0)
    {
      locals.available_cards[locals.index] = 1;
    }
    else
    {
      locals.available_cards[locals.index] = available[locals.index];
    }
  }

  locals.show_bigcard = big_card_mode;
  if (big_card_mode == 0)
  {
    strcpy(locals.title, prompt);
  }
  else
  {
    strcpy(locals.title, unk_00572930);
  }

  return DialogBoxParam(global_hinstance, (const char *)0xe9, unk_008cf1b4, dlgfunc_show_deck, (long)&locals.context);
}
