#include "magic_shell_screen_name.h"
#include "magic_shell_dialogs.h"
#include "game_support.h"
#include "global_other.h"
#include "global_strings.h"
#include "shared_startup.h"
#include "cardartlib/src/palette.h"
#include <stdio.h>
#include <string.h>

extern HWND global_main_hwnd;

// FUNCTION: MAGIC 0x00490474
BOOL CALLBACK shell_enter_screen_name_dialog_proc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
  struct
  {
    COLORREF button_color;
    DRAWITEMSTRUCT *draw_item;
    HWND color_control;
    int color_id;
    HDC color_dc;
    int command_id;
    char name[100];
    char *cursor;
    char background_path[264];
    HDC paint_dc;
    PAINTSTRUCT paint;
  } s;
  switch (message)
  {
  case WM_INITDIALOG:
    load_text("MP_UIStrings.txt", "SHELLPAGE_ENTERSCREENNAME");
    SetDlgItemTextA(hwnd, 0x745, g_text_lines[0]);
    SendDlgItemMessageA(hwnd, 0x746, EM_LIMITTEXT, 13, 0);
    sprintf(s.background_path, "%s\\WINBK_attack.pic", global_duelart_path);
    g_screen_name_entry_background = load_pic(s.background_path);
    SetFocus(GetDlgItem(hwnd, 0x746));
    SendDlgItemMessageA(hwnd, 0x746, EM_SETSEL, 0, -1);
    BringWindowToTop(hwnd);
    return FALSE;

  case WM_COMMAND:
    s.command_id = wparam & 0xffff;
    switch (s.command_id)
    {
    case IDOK:
      if (g_screen_name_entry_background != NULL)
        delete_and_close_object(g_screen_name_entry_background);
      SendMessageA(GetDlgItem(hwnd, 0x746), WM_GETTEXT, 100, (LPARAM)s.name);
      s.cursor = s.name;
      while (*s.cursor != '\0')
      {
        if (IsCharAlphaNumericA(*s.cursor))
          ++s.cursor;
        else
          strcpy(s.cursor, s.cursor + 1);
      }
      if (strcmp(s.name, "") == 0)
        break;
      strcpy(g_screen_name_profile.screen_name, s.name);
      EndDialog(hwnd, -1);
      break;
    case IDCANCEL:
      if (g_screen_name_entry_background != NULL)
        delete_and_close_object(g_screen_name_entry_background);
      EndDialog(hwnd, -1);
      break;
    }
    return TRUE;

  case WM_CTLCOLORSTATIC:
    s.color_dc = (HDC)wparam;
    ApplyCardArtPaletteToDc(s.color_dc);
    s.color_control = (HWND)lparam;
    s.color_id = GetDlgCtrlID(s.color_control);
    SetBkMode(s.color_dc, TRANSPARENT);
    SetTextColor(s.color_dc, g_screen_name_entry_text_color);
    return (BOOL)GetStockObject(NULL_BRUSH);

  case WM_DRAWITEM:
    s.draw_item = (DRAWITEMSTRUCT *)lparam;
    if (GetFocus() == s.draw_item->hwndItem)
      s.button_color = g_screen_name_playface_resources.selected_button_color;
    else
      s.button_color = g_screen_name_entry_button_color;
    draw_owner_draw_button_centered(s.draw_item, g_screen_name_entry_brush, g_screen_name_entry_light_pen,
        g_screen_name_entry_dark_pen, s.button_color, 0);
    return TRUE;

  case 0x30f:
  case 0x310:
  case 0x311:
    return handle_button_palette_message((int)hwnd, message, (int)wparam, lparam);

  case WM_PAINT:
    s.paint_dc = BeginPaint(hwnd, &s.paint);
    if (s.paint_dc != NULL)
    {
      ApplyCardArtPaletteToDc(s.paint_dc);
      if (g_screen_name_entry_background != NULL)
        DrawBitmapToRect(s.paint_dc, &s.paint.rcPaint, g_screen_name_entry_background);
      else
        FillRect(s.paint_dc, &s.paint.rcPaint, (HBRUSH)GetStockObject(GRAY_BRUSH));
      EndPaint(hwnd, &s.paint);
    }
    return TRUE;

  case WM_LBUTTONDOWN:
    SendMessageA(hwnd, WM_SYSCOMMAND, 0xf012, 0);
    return FALSE;

  default:
    return FALSE;
  }
}
