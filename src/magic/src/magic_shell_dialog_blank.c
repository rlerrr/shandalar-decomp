#include "magic_shell_dialogs.h"
#include "cardartlib/src/palette.h"
#include "deckdll/src/shared_resources.h"
#include "game_support.h"
#include "global_strings.h"

extern HWND global_main_hwnd;

// GLOBAL: MAGIC 0x00637b04
static HFONT shell_blank_dialog_font;
// GLOBAL: MAGIC 0x00637b08
static HFONT shell_blank_dialog_low_resolution_font;

// FUNCTION: MAGIC 0x0045ff30
BOOL CALLBACK shell_blank_dialog_proc(HWND hwnd, UINT message,
                                     WPARAM wparam, LPARAM lparam)
{
  struct
  {
    LOGFONTA font;
    HFONT replacement_font;
    HDC paint_dc;
    PAINTSTRUCT paint;
    int line_height;
    LOGFONTA *paint_font_description;
    RECT shell_rect;
    HFONT old_font;
    HDC erase_dc;
    RECT erase_rect;
    HDC color_dc;
    HFONT control_font;
    HBRUSH color_brush;
    LOGFONTA *initial_font_description;
    RECT *initial_rect;
  } s;

  switch (message)
  {
  case WM_INITDIALOG:
    s.initial_font_description = LoadFontFromIni("ShellDialog", 0);
    shell_blank_dialog_font = CreateFontIndirectA(s.initial_font_description);
    s.initial_font_description = LoadFontFromIni("ShellDialog_lores", 0);
    shell_blank_dialog_low_resolution_font =
        CreateFontIndirectA(s.initial_font_description);
    s.control_font = shell_blank_dialog_font;
    SendDlgItemMessageA(hwnd, 0x747, WM_SETFONT, (WPARAM)s.control_font, 0);
    load_text("MP_UIStrings.txt", "SHELLPAGE_BLANK");
    SetDlgItemTextA(hwnd, 0x747, g_text_lines[0]);
    s.initial_rect = (RECT *)lparam;
    MoveWindow(hwnd, s.initial_rect->left, s.initial_rect->top,
               s.initial_rect->right - s.initial_rect->left,
               s.initial_rect->bottom - s.initial_rect->top, TRUE);
    return FALSE;

  case WM_DESTROY:
    s.control_font = (HFONT)SendDlgItemMessageA(hwnd, 0x747, WM_GETFONT, 0, 0);
    SendDlgItemMessageA(hwnd, 0x747, WM_SETFONT, 0, 0);
    DeleteObject(s.control_font);
    DeleteObject(shell_blank_dialog_low_resolution_font);
    DeleteObject(shell_blank_dialog_font);
    return FALSE;

  case WM_CTLCOLOREDIT:
  case WM_CTLCOLORBTN:
  case WM_CTLCOLORSTATIC:
    s.color_dc = (HDC)wparam;
    ApplyCardArtPaletteToDc(s.color_dc);
    SetTextColor(s.color_dc, shell_draw_resources.selected_button_color);
    SetBkMode(s.color_dc, TRANSPARENT);
    s.color_brush = (HBRUSH)GetStockObject(NULL_BRUSH);
    return (BOOL)s.color_brush;

  case 0x30f:
  case 0x310:
  case 0x311:
    return handle_button_palette_message((int)hwnd, message, (int)wparam,
                                         lparam);

  case WM_ERASEBKGND:
    s.erase_dc = (HDC)wparam;
    ApplyCardArtPaletteToDc(s.erase_dc);
    GetClientRect(hwnd, &s.erase_rect);
    MapWindowPoints(hwnd, GetParent(hwnd), (LPPOINT)&s.erase_rect, 2);
    SendMessageA(GetParent(hwnd), 0x498, (WPARAM)s.erase_dc,
                 (LPARAM)&s.erase_rect);
    return TRUE;

  case WM_PAINT:
    s.paint_dc = BeginPaint(hwnd, &s.paint);
    GetClientRect(global_main_hwnd, &s.shell_rect);
    if (s.shell_rect.right < 0x2ee || s.shell_rect.bottom < 0x226)
    {
      s.paint_font_description = LoadFontFromIni("ShellDialogLabel", 0);
      s.old_font = shell_blank_dialog_low_resolution_font;
    }
    else
    {
      s.paint_font_description = LoadFontFromIni("ShellDialog", 0);
      s.old_font = shell_blank_dialog_font;
    }
    GetObjectA(s.old_font, sizeof(s.font), &s.font);
    s.line_height = s.paint_font_description->lfHeight * 2 / 3;
    s.font.lfHeight = (s.shell_rect.bottom - s.shell_rect.top) / s.line_height;
    if (s.font.lfHeight % 2 != 0)
      ++s.font.lfHeight;
    s.replacement_font = CreateFontIndirectA(&s.font);
    if (s.old_font == shell_blank_dialog_low_resolution_font)
      shell_blank_dialog_low_resolution_font = s.replacement_font;
    else
      shell_blank_dialog_font = s.replacement_font;
    SendMessageA(GetDlgItem(hwnd, 0x747), WM_SETFONT,
                 (WPARAM)s.replacement_font, 0);
    DeleteObject(s.old_font);
    EndPaint(hwnd, &s.paint);
    return TRUE;

  }
#ifdef MODERN_FIXES
  return message;
#endif
}
