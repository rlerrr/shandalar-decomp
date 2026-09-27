#include "magic_shell_network_match.h"
#include <string.h>

// GLOBAL: MAGIC 0x0093eda0
char g_multiplayer_button_text[6][300];

// FUNCTION: MAGIC 0x0045a076
void shell_layout_multiplayer_controls(HWND hwnd)
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
  s.font = (HGDIOBJ)SendDlgItemMessageA(hwnd, 0x722, 0x31, 0, 0);
  s.dc = GetDC(hwnd);
  SelectObject(s.dc, s.font);
  GetTextExtentPoint32A(s.dc, "fun", 3, &s.text_size);
  s.button_height = s.text_size.cy * 2;
  s.line_height = s.text_size.cy;
  s.spacing = (s.text_size.cy * 3) / 2;
  s.x = s.text_size.cx + s.spacing / 2;
  s.y = 0;
  s.control = GetDlgItem(hwnd, 0x721);
  GetWindowTextA(s.control, s.text, 100);
  GetTextExtentPoint32A(s.dc, s.text, strlen(s.text), &s.text_size);
  SetWindowPos(s.control, NULL, s.x, s.y, (s.page_rect.right - s.page_rect.left) * 3 / 4,
      (s.line_height << 2) / 3, 4);
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
  s.control = GetDlgItem(hwnd, 0x72e);
  GetWindowTextA(s.control, s.text, 100);
  GetTextExtentPoint32A(s.dc, g_multiplayer_button_text[3], strlen(g_multiplayer_button_text[3]), &s.text_size);
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
  s.control = GetDlgItem(hwnd, 0x72e);
  SetWindowPos(s.control, NULL, s.x, s.y, s.width, s.button_height, 4);
  s.y -= (s.button_height + s.button_height * 3 / 4);
  s.control = GetDlgItem(hwnd, 0x72d);
  GetWindowTextA(s.control, s.text, 100);
  GetTextExtentPoint32A(s.dc, s.text, strlen(s.text), &s.text_size);
  s.width = s.text_size.cx + s.spacing;
  SetWindowPos(s.control, NULL, (s.x + (s.page_rect.right - s.x) / 2) - s.width / 2, s.y, s.width,
      s.button_height, 4);
  SetRect(&s.buttons_rect, min((s.x + (s.page_rect.right - s.x) / 2) - s.width / 2, s.x), s.y,
      s.page_rect.right, s.page_rect.bottom);
  s.control = GetDlgItem(hwnd, 0x722);
  GetWindowTextA(s.control, s.text, 100);
  GetTextExtentPoint32A(s.dc, s.text, strlen(s.text), &s.text_size);
  GetWindowRect(GetDlgItem(hwnd, 0x721), &s.control_rect);
  MapWindowPoints(NULL, hwnd, (LPPOINT)&s.control_rect, 2);
  SetWindowPos(s.control, NULL, 0, s.control_rect.bottom + s.button_height, s.text_size.cx + s.spacing / 2,
      s.line_height, 4);
  s.control = GetDlgItem(hwnd, 0x723);
  GetClientRect(GetDlgItem(hwnd, 0x723), &s.control_rect);
  s.available_height = s.control_rect.bottom - s.control_rect.top;
  GetWindowRect(GetDlgItem(hwnd, 0x722), &s.control_rect);
  MapWindowPoints(NULL, hwnd, (LPPOINT)&s.control_rect, 2);
  s.width = s.page_rect.right - (s.control_rect.right + 10);
  SetWindowPos(s.control, NULL, s.control_rect.right + 10, s.control_rect.top, s.width, s.available_height, 4);
  s.x = 0;
  s.y = (s.control_rect.top + s.button_height / 3) + s.available_height * 4;
  SetRect(&s.lower_rect, s.x, s.y, s.buttons_rect.left - s.spacing, s.page_rect.bottom);
  s.width = s.lower_rect.right - s.lower_rect.left;
  SetRect(&s.options_rect, s.lower_rect.left, s.lower_rect.top, s.lower_rect.left + s.width, s.lower_rect.bottom);
  s.control = GetDlgItem(hwnd, 0x724);
  SetWindowPos(s.control, NULL, s.options_rect.left, s.options_rect.top,
      s.options_rect.right - s.options_rect.left, s.options_rect.bottom - s.options_rect.top, 4);
  s.x = s.options_rect.left + s.spacing;
  s.y = s.options_rect.top + s.button_height;
  s.available_height = s.options_rect.bottom - s.y;
  s.control = GetDlgItem(hwnd, 0x725);
  GetWindowTextA(s.control, s.text, 100);
  GetTextExtentPoint32A(s.dc, s.text, strlen(s.text), &s.text_size);
  SetWindowPos(s.control, NULL, s.x, s.y, s.text_size.cx + s.character_width, s.line_height, 4);
  s.y += s.available_height / 5;
  s.control = GetDlgItem(hwnd, 0x732);
  GetWindowTextA(s.control, s.text, 100);
  GetTextExtentPoint32A(s.dc, s.text, strlen(s.text), &s.text_size);
  s.width = s.text_size.cx + s.spacing / 2;
  SetWindowPos(s.control, NULL, s.x, s.y, s.width, s.line_height, 4);
  s.y += s.line_height + s.button_height / 4;
  s.control = GetDlgItem(hwnd, 0x731);
  GetWindowTextA(s.control, s.text, 100);
  GetTextExtentPoint32A(s.dc, s.text, strlen(s.text), &s.text_size);
  s.width = s.text_size.cx + s.spacing;
  SetWindowPos(s.control, NULL, s.x, s.y, s.width, s.line_height, 4);
  s.next_x = s.width + s.x;
  GetTextExtentPoint32A(s.dc, "0", 2, &s.text_size);
  s.width = s.text_size.cx + s.spacing;
  s.control = GetDlgItem(hwnd, 0x727);
  SetWindowPos(s.control, NULL, s.next_x, s.y, s.width, s.button_height, 4);
  s.next_x += s.width;
  GetTextExtentPoint32A(s.dc, "00", 2, &s.text_size);
  s.width = s.text_size.cx;
  s.control = GetDlgItem(hwnd, 0x72f);
  SetWindowPos(s.control, NULL, s.next_x, s.y, s.width, s.button_height, 4);
  s.y += (s.available_height + s.button_height) / 4;
  s.control = GetDlgItem(hwnd, 0x733);
  GetWindowTextA(s.control, s.text, 100);
  GetTextExtentPoint32A(s.dc, s.text, strlen(s.text), &s.text_size);
  SetWindowPos(s.control, NULL, s.x, s.y, s.text_size.cx + s.character_width, s.line_height, 4);
  s.next_x = s.text_size.cx + s.spacing;
  s.control = GetDlgItem(hwnd, 0x734);
  GetWindowTextA(s.control, s.text, 100);
  GetTextExtentPoint32A(s.dc, s.text, strlen(s.text), &s.text_size);
  SetWindowPos(s.control, NULL, s.next_x, s.y, s.text_size.cx + s.character_width, s.line_height, 4);
  s.y += s.line_height + s.button_height / 4;
  s.control = GetDlgItem(hwnd, 0x735);
  GetWindowTextA(s.control, s.text, 100);
  GetTextExtentPoint32A(s.dc, s.text, strlen(s.text), &s.text_size);
  SetWindowPos(s.control, NULL, s.next_x, s.y, s.text_size.cx + s.character_width, s.line_height, 4);
  s.y += s.available_height / 5;
  s.control = GetDlgItem(hwnd, 0x728);
  GetWindowTextA(s.control, s.text, 100);
  GetTextExtentPoint32A(s.dc, s.text, strlen(s.text), &s.text_size);
  SetWindowPos(s.control, NULL, s.x, s.y, s.text_size.cx + s.character_width, s.line_height, 4);
  s.control = GetDlgItem(hwnd, 0x724);
  s.x = s.lower_rect.left + (s.lower_rect.right - s.lower_rect.left) / 2 + s.spacing;
  s.y = s.lower_rect.top + s.button_height;
  s.width = (s.lower_rect.right - s.x) - s.spacing / 5;
  s.width += s.character_width;
  s.available_height = s.lower_rect.bottom - s.y;
  s.control = GetDlgItem(hwnd, 0x729);
  SetWindowPos(s.control, NULL, s.x, s.y, s.width, s.line_height, 4);
  s.y += s.available_height / 8;
  s.control = GetDlgItem(hwnd, 0x72b);
  SetWindowPos(s.control, NULL, s.x, s.y, s.width, s.line_height, 4);
  s.y += s.available_height / 8;
  s.control = GetDlgItem(hwnd, 0x72c);
  SetWindowPos(s.control, NULL, s.x, s.y, s.width, s.line_height, 4);
  s.y += s.available_height / 8;
  s.control = GetDlgItem(hwnd, 0x730);
  SetWindowPos(s.control, NULL, s.x, s.y, s.width, s.line_height, 4);
  s.y += s.available_height / 8;
  s.control = GetDlgItem(hwnd, 0x72a);
  SetWindowPos(s.control, NULL, s.x, s.y, s.width, s.line_height, 4);
  ReleaseDC(hwnd, s.dc);
}
