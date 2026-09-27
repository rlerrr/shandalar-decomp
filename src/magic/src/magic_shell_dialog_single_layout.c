#include "magic_shell_single_duel.h"
#include <string.h>

// FUNCTION: MAGIC 0x0045cd05
void shell_layout_single_duel_controls(HWND hwnd)
{
  struct

  {
    char text[100];
    RECT options_rect;
    HWND control;
    RECT control_rect;
    RECT buttons_rect;
    HDC dc;
    int available_height;
    int width;
    RECT lower_rect;
    int y;
    int x;
    int next_x;
    HGDIOBJ font;
    RECT page_rect;
    SIZE text_size;
    RECT difficulty_rect;
    int button_height;
    int line_height;
    int spacing;
    int character_width;
  } s;

  GetClientRect(hwnd, &s.page_rect);
  s.page_rect.left += 0x14;
  s.page_rect.right += -0x14;
  s.page_rect.top += 0x14;
  s.page_rect.bottom += -0x14;
  s.font = (HGDIOBJ)SendDlgItemMessageA(hwnd, 0x640, 0x31, 0, 0);
  s.dc = GetDC(hwnd);
  SelectObject(s.dc, s.font);
  GetTextExtentPoint32A(s.dc, "fun", 3, &s.text_size);
  s.button_height = s.text_size.cy * 2;
  s.line_height = s.text_size.cy;
  s.spacing = (s.text_size.cy * 3) / 2;
  s.control = GetDlgItem(hwnd, 0x640);
  GetWindowTextA(s.control, s.text, 100);
  GetTextExtentPoint32A(s.dc, s.text, strlen(s.text), &s.text_size);
  SetWindowPos(s.control, NULL, 0, 0, s.text_size.cx + s.spacing, s.line_height, 4);
  s.x = s.text_size.cx + s.spacing / 2;
  s.y = 0;
  s.control = GetDlgItem(hwnd, 0x641);
  GetWindowTextA(s.control, s.text, 100);
  GetTextExtentPoint32A(s.dc, s.text, strlen(s.text), &s.text_size);
  SetWindowPos(s.control, NULL, s.x, s.y, s.text_size.cx + s.spacing, s.line_height, 4);
  s.font = (HGDIOBJ)SendDlgItemMessageA(hwnd, 1, 0x31, 0, 0);
  s.dc = GetDC(hwnd);
  SelectObject(s.dc, s.font);
  GetTextExtentPoint32A(s.dc, "nn", 3, &s.text_size);
  s.button_height = s.text_size.cy * 2;
  s.line_height = s.text_size.cy;
  s.spacing = (s.text_size.cy * 3) / 2;
  s.character_width = s.text_size.cx;
  s.control = GetDlgItem(hwnd, 1);
  GetWindowTextA(s.control, s.text, 100);
  GetTextExtentPoint32A(s.dc, s.text, strlen(s.text), &s.text_size);
  s.width = s.text_size.cx;
  s.control = GetDlgItem(hwnd, 0x64f);
  GetWindowTextA(s.control, s.text, 100);
  GetTextExtentPoint32A(s.dc, s.text, strlen(s.text), &s.text_size);
  if (s.width < s.text_size.cx)
  {
    s.width = s.text_size.cx;
  }
  s.width += s.spacing;
  s.x = s.page_rect.right - s.width;
  s.y = s.page_rect.bottom - s.button_height;
  s.control = GetDlgItem(hwnd, 1);
  SetWindowPos(s.control, NULL, s.x, s.y, s.width, s.button_height, 4);
  s.y -= (s.button_height + s.button_height / 3);
  s.control = GetDlgItem(hwnd, 0x64f);
  SetWindowPos(s.control, NULL, s.x, s.y, s.width, s.button_height, 4);
  s.y -= (s.button_height + s.button_height * 3 / 4);
  s.control = GetDlgItem(hwnd, 0x650);
  GetWindowTextA(s.control, s.text, 100);
  GetTextExtentPoint32A(s.dc, s.text, strlen(s.text), &s.text_size);
  s.width = s.text_size.cx + s.spacing;
  SetWindowPos(s.control, NULL, (s.x + (s.page_rect.right - s.x) / 2) - s.width / 2, s.y, s.width,
      s.button_height, 4);
  SetRect(&s.buttons_rect, min((s.x + (s.page_rect.right - s.x) / 2) - s.width / 2, s.x), s.y,
      s.page_rect.right, s.page_rect.bottom);
  s.control = GetDlgItem(hwnd, 0x644);
  GetWindowTextA(s.control, s.text, 100);
  GetTextExtentPoint32A(s.dc, s.text, strlen(s.text), &s.text_size);
  GetWindowRect(GetDlgItem(hwnd, 0x641), &s.control_rect);
  MapWindowPoints(NULL, hwnd, (LPPOINT)&s.control_rect, 2);
  SetWindowPos(s.control, NULL, 0, s.control_rect.bottom + s.button_height, s.text_size.cx + s.spacing / 2,
      s.line_height, 4)
  ;
  s.control = GetDlgItem(hwnd, 0x645);
  GetClientRect(GetDlgItem(hwnd, 0x645), &s.control_rect);
  s.available_height = s.control_rect.bottom - s.control_rect.top;
  GetWindowRect(GetDlgItem(hwnd, 0x644), &s.control_rect);
  MapWindowPoints(NULL, hwnd, (LPPOINT)&s.control_rect, 2);
  s.width = s.page_rect.right - (s.control_rect.right + 10);
  SetWindowPos(s.control, NULL, s.control_rect.right + 10, s.control_rect.top, s.width, s.available_height, 4);
  s.control = GetDlgItem(hwnd, 0x642);
  GetWindowTextA(s.control, s.text, 100);
  GetTextExtentPoint32A(s.dc, s.text, strlen(s.text), &s.text_size);
  GetWindowRect(GetDlgItem(hwnd, 0x641), &s.control_rect);
  MapWindowPoints(NULL, hwnd, (LPPOINT)&s.control_rect, 2);
  SetWindowPos(s.control, NULL, 0, s.button_height * 3 + s.control_rect.bottom,
      s.text_size.cx + s.spacing / 2, s.line_height, 4);
  s.control = GetDlgItem(hwnd, 0x643);
  GetClientRect(GetDlgItem(hwnd, 0x643), &s.control_rect);
  s.available_height = s.control_rect.bottom - s.control_rect.top;
  GetWindowRect(GetDlgItem(hwnd, 0x642), &s.control_rect);
  MapWindowPoints(NULL, hwnd, (LPPOINT)&s.control_rect, 2);
  s.width = s.page_rect.right - (s.control_rect.right + 10);
  SetWindowPos(s.control, NULL, s.control_rect.right + 10, s.control_rect.top, s.width, s.available_height, 4);
  s.x = 0;
  s.y = s.buttons_rect.top - s.button_height / 3;
  SetRect(&s.lower_rect, s.x, s.y, s.buttons_rect.left - s.spacing, s.page_rect.bottom);
  s.width = s.lower_rect.right - s.lower_rect.left;
  SetRect(&s.options_rect, s.lower_rect.left, s.lower_rect.top, (s.lower_rect.left + s.width / 2) - s.spacing,
      s.lower_rect.bottom);
  SetRect(&s.difficulty_rect, s.lower_rect.left + s.width / 2, s.lower_rect.top, s.lower_rect.right,
      s.lower_rect.bottom);
  s.control = GetDlgItem(hwnd, 0x646);
  SetWindowPos(s.control, NULL, s.options_rect.left, s.options_rect.top,
      s.options_rect.right - s.options_rect.left, s.options_rect.bottom - s.options_rect.top, 4);
  s.x = s.options_rect.left + s.spacing;
  s.y = s.options_rect.top + s.button_height;
  s.available_height = s.options_rect.bottom - s.y;
  s.control = GetDlgItem(hwnd, 0x647);
  GetWindowTextA(s.control, s.text, 100);
  GetTextExtentPoint32A(s.dc, s.text, strlen(s.text), &s.text_size);
  SetWindowPos(s.control, NULL, s.x, s.y, s.text_size.cx + s.character_width, s.line_height, 4);
  s.y += s.available_height / 3;
  s.control = GetDlgItem(hwnd, 0x705);
  GetWindowTextA(s.control, s.text, 100);
  GetTextExtentPoint32A(s.dc, s.text, strlen(s.text), &s.text_size);
  s.width = s.text_size.cx + s.spacing / 2;
  SetWindowPos(s.control, NULL, s.x, s.y, s.width, s.line_height, 4);
  s.next_x = s.width + s.x;
  GetTextExtentPoint32A(s.dc, "0", 2, &s.text_size);
  s.width = s.text_size.cx + s.spacing;
  s.control = GetDlgItem(hwnd, 0x706);
  SetWindowPos(s.control, NULL, s.next_x, s.y, s.width, s.button_height, 4);
  s.next_x += s.width;
  GetTextExtentPoint32A(s.dc, "00", 2, &s.text_size);
  s.width = s.text_size.cx;
  s.control = GetDlgItem(hwnd, 0x707);
  SetWindowPos(s.control, NULL, s.next_x, s.y, s.width, s.button_height, 4);
  s.y += s.available_height / 3;
  s.control = GetDlgItem(hwnd, 0x6b5);
  GetWindowTextA(s.control, s.text, 100);
  GetTextExtentPoint32A(s.dc, s.text, strlen(s.text), &s.text_size);
  SetWindowPos(s.control, NULL, s.x, s.y, s.text_size.cx + s.character_width, s.line_height, 4);
  s.control = GetDlgItem(hwnd, 0x64a);
  SetWindowPos(s.control, NULL, s.difficulty_rect.left, s.difficulty_rect.top,
      s.difficulty_rect.right - s.difficulty_rect.left, s.difficulty_rect.bottom - s.difficulty_rect.top, 4);
  s.x = s.difficulty_rect.left + s.spacing;
  s.y = s.difficulty_rect.top + s.button_height;
  s.width = (s.difficulty_rect.right - s.x) - s.spacing / 4;
  s.width += s.character_width;
  s.available_height = s.difficulty_rect.bottom - s.y;
  s.control = GetDlgItem(hwnd, 0x64b);
  SetWindowPos(s.control, NULL, s.x, s.y, s.width, s.line_height, 4);
  s.y += s.available_height / 4;
  s.control = GetDlgItem(hwnd, 0x64c);
  SetWindowPos(s.control, NULL, s.x, s.y, s.width, s.line_height, 4);
  s.y += s.available_height / 4;
  s.control = GetDlgItem(hwnd, 0x64d);
  SetWindowPos(s.control, NULL, s.x, s.y, s.width, s.line_height, 4);
  s.y += s.available_height / 4;
  s.control = GetDlgItem(hwnd, 0x64e);
  SetWindowPos(s.control, NULL, s.x, s.y, s.width, s.line_height, 4);
  ReleaseDC(hwnd, s.dc);
}
