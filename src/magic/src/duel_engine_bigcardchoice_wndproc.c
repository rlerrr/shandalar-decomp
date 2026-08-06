#include <windows.h>
#include <stdlib.h>
#include <string.h>

#include "cardartlib/src/palette.h"
#include "drawcardlib/Drawcardlib.h"
#include "magic/src/duel_engine.h"
#include "magic/src/global_other.h"
#include "magic/src/shared_startup.h"

HBITMAP load_pic(char *filename);
LRESULT handle_button_palette_message(int hwnd, unsigned int msg, int wparam, int lparam);

extern int g_big_card_choice_timeout_ms;

#define BIGCARD_CHOICE_FONT_WINDOW_LONG_OFFSET g_big_card_choice_font_window_long_offset
#define BIGCARD_CHOICE_ITEMS_WINDOW_LONG_OFFSET g_big_card_choice_items_window_long_offset
#define BIGCARD_CHOICE_COUNT_WINDOW_LONG_OFFSET g_big_card_choice_count_window_long_offset
#define BIGCARD_CHOICE_SELECTED_WINDOW_LONG_OFFSET g_big_card_choice_selected_window_long_offset
#define BIGCARD_CHOICE_MODE_WINDOW_LONG_OFFSET g_big_card_choice_mode_window_long_offset

// GLOBAL: MAGIC 0x0055e19c
// GLOBAL: SHANDALAR 0x0057f0b8
int g_big_card_choice_font_window_long_offset = 0;

// GLOBAL: MAGIC 0x0055e1a0
// GLOBAL: SHANDALAR 0x0057f0bc
int g_big_card_choice_items_window_long_offset = 4;

// GLOBAL: MAGIC 0x0055e1a4
// GLOBAL: SHANDALAR 0x0057f0c0
int g_big_card_choice_count_window_long_offset = 8;

// GLOBAL: MAGIC 0x0055e1a8
// GLOBAL: SHANDALAR 0x0057f0c4
int g_big_card_choice_selected_window_long_offset = 0xc;

// GLOBAL: MAGIC 0x0055e1ac
// GLOBAL: SHANDALAR 0x0057f0c8
int g_big_card_choice_mode_window_long_offset = 0x10;

// GLOBAL: MAGIC 0x0055e1b0
// GLOBAL: SHANDALAR 0x0057f0cc
int g_big_card_choice_window_extra_bytes = 0x14;

extern char g_big_card_choice_window_text_buffer[0xc8];

int big_card_choice_item_is_enabled(HWND hwnd, int item);

// FUNCTION: MAGIC 0x00507e37
// FUNCTION: SHANDALAR 0x004c6b35
int register_MAGICGAME_BigCardChoiceClass(LPCSTR class_name)
{
  struct
  {
    char path[264];
    int registered;
    WNDCLASSA wndclass;
  } s;

  s.registered = 1;
  SET_DUEL_WNDCLASS(s.wndclass, 0, wndproc_MAGICGAME_BigCardChoiceClass,
                    g_big_card_choice_window_extra_bytes,
                    LoadIconA((HINSTANCE)0, (LPCSTR)0x7f00), (HBRUSH)0x6, class_name);
  if (RegisterClassA(&s.wndclass) == 0)
  {
    s.registered = 0;
  }

  strcpy(s.path, global_duelart_path);
  strcat(s.path, "\\WINBK_BigCard.pic");
  g_magicgame_big_card_choice_pic = load_pic(s.path);
  g_big_card_choice_timeout_ms = 9000;
  return s.registered;
}

// FUNCTION: MAGIC 0x00507f42
// FUNCTION: SHANDALAR 0x004c6c40
LRESULT CALLBACK wndproc_MAGICGAME_BigCardChoiceClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  struct
  {
    int strlen_result;
    int any_enabled;
    int found_choices;
    int text_index;
    LRESULT text_result;
    char *source;
    RECT selection_rect;
    SIZE text_size;
    COLORREF color;
    HDC paint_dc;
    int line_height;
    PAINTSTRUCT paint;
    int item_y;
    int index;
    COLORREF title_color;
    COLORREF normal_color;
    COLORREF selected_text_color;
    COLORREF selected_color;
    char text[500];
    TEXTMETRICA metrics;
    COLORREF disabled_color;
    RECT rect; // ebp - 0xd0
    COLORREF shadow_color;
    char *item_text; // ebp - 0xbc
    int title_height;
    RECT before_drag_rect;
    int mouse_line_height; // ebp - 0xa4
    RECT mouse_selection_rect;
    int mouse_x;
    int mouse_y;
    HDC mouse_dc;
    int new_selected; // ebp - 0x84
    int mouse_index;  // ebp - 0x80
    int mouse_item_y; // ebp - 0x7c
    int was_dragged;
    TEXTMETRICA mouse_metrics;
    RECT after_drag_rect;
    RECT mouse_rect;
    int command_selected;
    union
    {
      char *create_name;
      int enabled;
    } u18;
    int count;    // ebp - 0x14
    int selected; // ebp - 0x10
    int mode;     // ebp - 0xc
    HFONT font;   // ebp - 0x8
    char *items;  // ebp - 0x4
  } s;

  switch (msg)
  {
  case 0x400:
    s.count = GetWindowLongA(hwnd, BIGCARD_CHOICE_COUNT_WINDOW_LONG_OFFSET);
    return s.count;

  case 0x401:
    s.mode = (int)wparam;
    SetWindowLongA(hwnd, BIGCARD_CHOICE_MODE_WINDOW_LONG_OFFSET, s.mode);
    return 0;

  case WM_CHAR:
    SendMessageA(GetParent(hwnd), WM_COMMAND, MAKELONG(GetDlgCtrlID(hwnd), 0), (LPARAM)hwnd);
    return 0;

  case WM_CREATE:
    SetWindowLongA(hwnd, BIGCARD_CHOICE_FONT_WINDOW_LONG_OFFSET, (LONG)(s.font = 0));
    s.items = (char *)malloc(0x3e8);
    s.count = 0;
    s.selected = -1;
    SetWindowLongA(hwnd, BIGCARD_CHOICE_COUNT_WINDOW_LONG_OFFSET, s.count);
    SetWindowLongA(hwnd, BIGCARD_CHOICE_ITEMS_WINDOW_LONG_OFFSET, (LONG)s.items);
    SetWindowLongA(hwnd, BIGCARD_CHOICE_SELECTED_WINDOW_LONG_OFFSET, s.selected);
    s.mode = (int)((CREATESTRUCTA *)lparam)->lpCreateParams;
    SetWindowLongA(hwnd, BIGCARD_CHOICE_MODE_WINDOW_LONG_OFFSET, s.mode);
    s.u18.create_name = (char *)((CREATESTRUCTA *)lparam)->lpszName;
    if (s.u18.create_name != NULL)
    {
      PostMessageA(hwnd, WM_SETTEXT, 0, (LPARAM)s.u18.create_name);
    }
    return 0;

  case WM_DESTROY:
    s.items = (char *)GetWindowLongA(hwnd, BIGCARD_CHOICE_ITEMS_WINDOW_LONG_OFFSET);
    if (s.items != NULL)
    {
      free(s.items);
    }
    return 0;

  case WM_ERASEBKGND:
    return 1;

  case WM_GETDLGCODE:
    return 4;

  case WM_GETFONT:
    s.font = (HFONT)GetWindowLongA(hwnd, BIGCARD_CHOICE_FONT_WINDOW_LONG_OFFSET);
    return (LRESULT)s.font;

  case WM_KEYDOWN:
    s.count = GetWindowLongA(hwnd, BIGCARD_CHOICE_COUNT_WINDOW_LONG_OFFSET);
    s.selected = GetWindowLongA(hwnd, BIGCARD_CHOICE_SELECTED_WINDOW_LONG_OFFSET);
    s.mode = GetWindowLongA(hwnd, BIGCARD_CHOICE_MODE_WINDOW_LONG_OFFSET);
    if (s.mode != 0)
    {
      if (wparam == VK_RETURN || wparam == VK_SPACE)
      {
        s.command_selected = s.selected + 1;
        SendMessageA(GetParent(hwnd),
                     WM_COMMAND,
                     MAKELONG(GetDlgCtrlID(hwnd), s.command_selected),
                     (LPARAM)hwnd);
      }
      else
      {
        if (wparam == VK_TAB)
        {
          if ((GetKeyState(VK_SHIFT) & 0x8000) != 0)
          {
            wparam = VK_UP;
          }
          else
          {
            wparam = VK_DOWN;
          }
        }

        switch (wparam)
        {
        case VK_HOME:
          s.selected = 0;
          while (!big_card_choice_item_is_enabled(hwnd, s.selected))
          {
            s.selected++;
          }
          break;

        case VK_END:
          s.selected = s.count - 1;
          while (!big_card_choice_item_is_enabled(hwnd, s.selected))
          {
            s.selected--;
          }
          break;

        case VK_UP:
          if (--s.selected < 0)
          {
            s.selected = s.count - 1;
          }

          while (!big_card_choice_item_is_enabled(hwnd, s.selected))
          {
            if (--s.selected < 0)
            {
              s.selected = s.count - 1;
            }
          }

          break;

        case VK_DOWN:
          if (s.count - 1 < ++s.selected)
          {
            s.selected = 0;
          }
          while (!big_card_choice_item_is_enabled(hwnd, s.selected))
          {
            if (s.count - 1 < ++s.selected)
            {
              s.selected = 0;
            }
          }
          break;
        }

        SetWindowLongA(hwnd, BIGCARD_CHOICE_SELECTED_WINDOW_LONG_OFFSET, s.selected);
        InvalidateRect(hwnd, (RECT *)0, 0);
      }
    }
    return 0;

  case WM_LBUTTONDOWN:
  case WM_MOUSEMOVE:
  case WM_LBUTTONUP:
    s.count = GetWindowLongA(hwnd, BIGCARD_CHOICE_COUNT_WINDOW_LONG_OFFSET);
    s.font = (HFONT)GetWindowLongA(hwnd, BIGCARD_CHOICE_FONT_WINDOW_LONG_OFFSET);
    s.selected = GetWindowLongA(hwnd, BIGCARD_CHOICE_SELECTED_WINDOW_LONG_OFFSET);
    s.mode = GetWindowLongA(hwnd, BIGCARD_CHOICE_MODE_WINDOW_LONG_OFFSET);
    if (msg == WM_LBUTTONDOWN)
    {
      GetWindowRect(GetParent(hwnd), &s.before_drag_rect);
      SendMessageA(GetParent(hwnd), WM_SYSCOMMAND, SC_MOVE | HTCAPTION, 0);
      msg = WM_LBUTTONUP;
      GetWindowRect(GetParent(hwnd), &s.after_drag_rect);
      if (abs(s.before_drag_rect.top - s.after_drag_rect.top) +
              abs(s.before_drag_rect.left - s.after_drag_rect.left) <
          5)
      {
        s.was_dragged = 0;
      }
      else
      {
        s.was_dragged = 1;
      }
    }
    else
    {
      s.was_dragged = 0;
    }

    if (s.was_dragged == 0)
    {
      s.new_selected = s.selected + 1;
      if (s.mode == 0)
      {
        if (msg == WM_LBUTTONUP)
        {
          SendMessageA(GetParent(hwnd),
                       WM_COMMAND,
                       MAKELONG(GetDlgCtrlID(hwnd), s.new_selected),
                       (LPARAM)hwnd);
        }
      }
      else if (s.count < 1)
      {
        if (msg == WM_LBUTTONUP)
        {
          SendMessageA(GetParent(hwnd), WM_COMMAND, MAKELONG(GetDlgCtrlID(hwnd), 0), (LPARAM)hwnd);
        }
      }
      else
      {
        s.mouse_x = lparam & 0xffff;
        s.mouse_y = HIWORD(lparam);
        s.mouse_dc = GetDC(hwnd);
        ApplyCardArtPaletteToDc(s.mouse_dc);
        SelectObject(s.mouse_dc, s.font);
        GetTextMetricsA(s.mouse_dc, &s.mouse_metrics);
        s.mouse_line_height = s.mouse_metrics.tmHeight + s.mouse_metrics.tmExternalLeading;
        ReleaseDC(hwnd, s.mouse_dc);
        s.new_selected = 0;
        GetClientRect(hwnd, &s.mouse_rect);
        s.mouse_item_y = s.mouse_rect.bottom - s.mouse_line_height;
        for (s.mouse_index = s.count; s.mouse_index > 0 && s.new_selected == 0; s.mouse_item_y -= s.mouse_line_height, s.mouse_index--)
        {
          if (s.mouse_item_y < s.mouse_y)
          {
            s.new_selected = s.mouse_index;
            SetRect(&s.mouse_selection_rect, 0, s.mouse_item_y,
                    s.mouse_rect.right, s.mouse_item_y + s.mouse_line_height);
          }
        }
        if (big_card_choice_item_is_enabled(hwnd, s.new_selected - 1))
        {
          s.selected = s.new_selected - 1;
          SetWindowLongA(hwnd, BIGCARD_CHOICE_SELECTED_WINDOW_LONG_OFFSET, s.selected);
          InvalidateRect(hwnd, (RECT *)0, 0);
          UpdateWindow(hwnd);
          if (msg == WM_LBUTTONUP)
          {
            Sleep(750);
            SendMessageA(GetParent(hwnd),
                         WM_COMMAND,
                         MAKELONG(GetDlgCtrlID(hwnd), s.new_selected),
                         (LPARAM)hwnd);
          }
        }
      }
    }
    return 0;

  case WM_PAINT:
    s.font = (HFONT)GetWindowLongA(hwnd, BIGCARD_CHOICE_FONT_WINDOW_LONG_OFFSET);
    s.count = GetWindowLongA(hwnd, BIGCARD_CHOICE_COUNT_WINDOW_LONG_OFFSET);
    s.items = (char *)GetWindowLongA(hwnd, BIGCARD_CHOICE_ITEMS_WINDOW_LONG_OFFSET);
    s.selected = GetWindowLongA(hwnd, BIGCARD_CHOICE_SELECTED_WINDOW_LONG_OFFSET);
    s.mode = GetWindowLongA(hwnd, BIGCARD_CHOICE_MODE_WINDOW_LONG_OFFSET);
    GetWindowTextA(hwnd, s.text, sizeof(s.text));

    s.item_text = s.items;
    s.title_color = 0x01000097;
    s.normal_color = 0x01000097;
    s.selected_text_color = 0x01000097;
    s.disabled_color = 0x010000c3;
    s.selected_color = 0x010000bf;
    s.shadow_color = 0x010000c9;
    s.paint_dc = BeginPaint(hwnd, &s.paint);
    if (s.paint_dc != (HDC)0)
    {
      ApplyCardArtPaletteToDc(s.paint_dc);
      GetClientRect(hwnd, &s.rect);
      SetBkMode(s.paint_dc, TRANSPARENT);
      SelectObject(s.paint_dc, s.font);
      SetTextColor(s.paint_dc, s.shadow_color);
      OffsetRect(&s.rect, 1, 1);
      DrawTextA(s.paint_dc, s.text, -1, &s.rect, DT_WORDBREAK);
      OffsetRect(&s.rect, -1, -1);
      SetTextColor(s.paint_dc, s.title_color);
      DrawTextA(s.paint_dc, s.text, -1, &s.rect, DT_WORDBREAK);
      s.title_height = DrawTextA(s.paint_dc, s.text, -1, &s.rect, DT_CALCRECT | DT_WORDBREAK);
      if (s.count != 0)
      {
        s.item_y = s.rect.top + s.title_height + 20;
        GetTextMetricsA(s.paint_dc, &s.metrics);
        s.line_height = s.metrics.tmHeight + s.metrics.tmExternalLeading +
                        (s.metrics.tmHeight * 10) / 100;
        SetBkMode(s.paint_dc, TRANSPARENT);
        for (s.index = 0; s.index < s.count; s.index++)
        {
          if (big_card_choice_item_is_enabled(hwnd, s.index) == 0)
          {
            s.item_text++;
            if (*s.item_text == ' ')
            {
              s.item_text++;
            }
          }

          if (s.mode != 0)
          {
            if (big_card_choice_item_is_enabled(hwnd, s.index) == 0)
            {
              s.color = s.disabled_color;
            }
            else if (s.index == s.selected)
            {
              s.color = s.selected_color;
            }
            else
            {
              s.color = s.normal_color;
            }
          }
          else
          {
            if (s.index == s.selected)
            {
              GetTextExtentPointA(s.paint_dc, s.item_text, strlen(s.item_text), &s.text_size);
              SetRect(&s.selection_rect,
                      s.rect.left - 5,
                      s.item_y,
                      s.rect.left + s.text_size.cx + 5,
                      s.item_y + s.line_height);
              FillRect(s.paint_dc, &s.selection_rect, GetStockObject(0));
            }

            if (big_card_choice_item_is_enabled(hwnd, s.index) == 0)
            {
              s.color = s.disabled_color;
            }
            else
            {
              s.color = s.selected_text_color;
            }
          }

          SetTextColor(s.paint_dc, s.shadow_color);
          TextOutA(s.paint_dc, s.rect.left + 1, s.item_y + 1, s.item_text, strlen(s.item_text));
          SetTextColor(s.paint_dc, s.color);
          TextOutA(s.paint_dc, s.rect.left, s.item_y, s.item_text, strlen(s.item_text));
          s.item_y += s.line_height;
          s.item_text += strlen(s.item_text) + 1;
        }
      }
      if (s.count != 0)
      {
        GetClientRect(hwnd, &s.rect);
        s.rect.bottom = s.item_y;
        SetWindowPos(hwnd, (HWND)0, 0, 0,
                     s.rect.right - s.rect.left,
                     s.rect.bottom - s.rect.top,
                     SWP_NOZORDER | SWP_NOMOVE);
      }
      EndPaint(hwnd, &s.paint);
    }
    return 0;

  case WM_SETFONT:
    s.font = (HFONT)GetWindowLongA(hwnd, BIGCARD_CHOICE_FONT_WINDOW_LONG_OFFSET);
    if ((HFONT)wparam != s.font)
    {
      s.font = (HFONT)wparam;
      SetWindowLongA(hwnd, BIGCARD_CHOICE_FONT_WINDOW_LONG_OFFSET, (LONG)s.font);
      InvalidateRect(hwnd, (RECT *)0, 1);
    }
    return 0;

  case WM_SETTEXT:
    s.source = (char *)lparam;
    s.text_index = 0;
    s.found_choices = 0;
    while (*s.source != '\0' && s.found_choices == 0)
    {
      if (*s.source == ' ' || *s.source == '>')
      {
        g_big_card_choice_window_text_buffer[s.text_index] = '\0';
        s.found_choices = 1;
      }
      else
      {
        while (*s.source != '\0' && *s.source != '\n')
        {
          g_big_card_choice_window_text_buffer[s.text_index] = *s.source;
          s.source++;
          s.text_index++;
        }
        if (*s.source != '\0')
        {
          g_big_card_choice_window_text_buffer[s.text_index] = *s.source;
          s.source++;
          s.text_index++;
        }
      }
    }

    g_big_card_choice_window_text_buffer[s.text_index] = '\0';
    while (g_big_card_choice_window_text_buffer[strlen(g_big_card_choice_window_text_buffer) - 2] == '\n')
    {
      s.strlen_result = strlen(g_big_card_choice_window_text_buffer);
      g_big_card_choice_window_text_buffer[s.strlen_result - 2] = '\0';
    }

    s.text_result = DefWindowProcA(hwnd, msg, wparam, (LPARAM)g_big_card_choice_window_text_buffer);
    s.items = (char *)GetWindowLongA(hwnd, BIGCARD_CHOICE_ITEMS_WINDOW_LONG_OFFSET);
    s.count = 0;
    s.selected = -1;
    if (*s.source != '\0')
    {
      s.text_index = 0;
      while (*s.source != '\0')
      {
        while (*s.source == ' ')
        {
          s.source++;
        }

        if (*s.source == '>')
        {
          s.selected = s.count;
          s.source++;
        }

        while (*s.source == '\n')
        {
          s.source++;
        }

        while (*s.source != '\0' && *s.source != '\n')
        {
          s.items[s.text_index] = *s.source;
          s.source++;
          s.text_index++;
        }
        s.items[s.text_index] = '\0';
        s.text_index++;
        if (*s.source != '\0')
        {
          s.source++;
        }
        s.count++;
      }

      s.items[s.text_index] = '\0';
    }

    if (s.count != 0 && s.selected == -1)
    {
      s.selected = 0;
    }

    s.any_enabled = 0;
    for (s.text_index = 0; s.text_index < s.count; s.text_index++)
    {
      s.any_enabled |= big_card_choice_item_is_enabled(hwnd, s.text_index);
    }
    if (s.any_enabled == 0)
    {
      s.mode = 0;
      SetWindowLongA(hwnd, BIGCARD_CHOICE_MODE_WINDOW_LONG_OFFSET, s.mode);
    }
    SetWindowLongA(hwnd, BIGCARD_CHOICE_COUNT_WINDOW_LONG_OFFSET, s.count);
    SetWindowLongA(hwnd, BIGCARD_CHOICE_ITEMS_WINDOW_LONG_OFFSET, (LONG)s.items);
    SetWindowLongA(hwnd, BIGCARD_CHOICE_SELECTED_WINDOW_LONG_OFFSET, s.selected);
    return s.text_result;

  case WM_QUERYNEWPALETTE:
  case WM_PALETTEISCHANGING:
  case WM_PALETTECHANGED:
    return handle_button_palette_message((int)hwnd, msg, (int)wparam, lparam);

  default:
    return DefWindowProcA(hwnd, msg, wparam, lparam);
  }
}

// FUNCTION: MAGIC 0x00509154
// FUNCTION: SHANDALAR 0x004c7e59
int big_card_choice_item_is_enabled(HWND hwnd, int item)
{
  struct
  {
    int result;
    int count;
    int index;
    char *items;
    char *item_text;
  } s;

  if (hwnd == (HWND)0)
  {
    return 0;
  }
  s.items = (char *)GetWindowLongA(hwnd, BIGCARD_CHOICE_ITEMS_WINDOW_LONG_OFFSET);
  s.count = GetWindowLongA(hwnd, BIGCARD_CHOICE_COUNT_WINDOW_LONG_OFFSET);
  if (s.count - 1 < item)
  {
    return 0;
  }

  s.item_text = s.items;

  for (s.index = 0; s.index < item; s.index++)
  {
    s.item_text += strlen(s.item_text) + 1;
  }

  if (*s.item_text == '_')
  {
    s.result = 0;
  }
  else
  {
    s.result = 1;
  }
  return s.result;
}
