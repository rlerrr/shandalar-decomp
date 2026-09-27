#include <stdio.h>
#include "magic_sealed_player.h"
#include "magic_shell_dialogs.h"
#include "cardartlib/src/palette.h"
#include "game_support.h"
#include "global_other.h"
#include "shared_startup.h"

extern char g_exp1_art_path[];
int draw_masked_bitmap_left_half_to_rect(HDC dc, RECT *rect, HANDLE bitmap);
void draw_owner_draw_button_centered(DRAWITEMSTRUCT *item, HBRUSH brush,
    HPEN light_pen, HPEN dark_pen, COLORREF color, int focus);

#define PLAYER_POINT_IN_RECT(rect, x, y) \
  ((BOOL(WINAPI *)(const RECT *, LONG, LONG))PtInRect)((rect), (x), (y))

static void shell_load_sealed_player_resources(HBITMAP *background, COLORREF *name_color,
    COLORREF *description_color, HBRUSH *button_brush, HPEN *light_pen, HPEN *dark_pen,
    COLORREF *button_color);
static void shell_release_sealed_player_resources(HBITMAP background, HBRUSH button_brush,
    HPEN light_pen, HPEN dark_pen);

// GLOBAL: MAGIC 0x0069b394
static sealed_player_dialog_context_t *g_sealed_player_context;
// GLOBAL: MAGIC 0x0069aeb8
static int g_sealed_player_close_pending;
// GLOBAL: MAGIC 0x0064fb18
static HBITMAP g_sealed_player_background;
// GLOBAL: MAGIC 0x0069af48
static COLORREF g_sealed_player_name_color;
// GLOBAL: MAGIC 0x0069b1d8
static COLORREF g_sealed_player_description_color;
// GLOBAL: MAGIC 0x0064fb00
static HBRUSH g_sealed_player_button_brush;
// GLOBAL: MAGIC 0x0069aedc
static HPEN g_sealed_player_light_pen;
// GLOBAL: MAGIC 0x0064fa48
static HPEN g_sealed_player_dark_pen;
// GLOBAL: MAGIC 0x0064f92c
static COLORREF g_sealed_player_button_color;
// GLOBAL: MAGIC 0x0069aef8
static RECT g_sealed_player_face_rect;

// FUNCTION: MAGIC 0x004cbdf2
BOOL CALLBACK shell_sealed_player_dialog_proc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
  struct
  {
    HDC paint_dc;
    PAINTSTRUCT paint;
    RECT paint_rect;
    HDC erase_dc;
    RECT erase_rect;
    DRAWITEMSTRUCT *draw_item;
    HWND control;
    int control_id;
    HDC control_dc;
    HBRUSH control_brush;
    HWND old_focus;
    HWND new_focus;
    unsigned int command;
    DWORD command_thread_status;
    DWORD timer_thread_status;
    POINT cursor;
    RECT timer_rect;
    HWND text_control;
    HDC layout_dc;
    int text_y;
    int text_x;
    HGDIOBJ text_font;
    RECT layout_rect;
    SIZE text_size;
    int button_height;
    int text_height;
    int text_margin;
    BITMAP background_bitmap;
    RECT parent_rect;
  } s;

  switch (message)
  {
  case WM_INITDIALOG:
    g_sealed_player_context = (sealed_player_dialog_context_t *)lparam;
    g_sealed_player_close_pending = 0;
    shell_load_sealed_player_resources(&g_sealed_player_background, &g_sealed_player_name_color,
        &g_sealed_player_description_color, &g_sealed_player_button_brush,
        &g_sealed_player_light_pen, &g_sealed_player_dark_pen, &g_sealed_player_button_color);
    SetRect(&g_sealed_player_face_rect, 30, 30, 148, 175);
    if (g_sealed_player_background != NULL)
    {
      GetObjectA(g_sealed_player_background, sizeof(BITMAP), &s.background_bitmap);
      GetWindowRect(GetParent(hwnd), &s.parent_rect);
      MoveWindow(hwnd,
          s.parent_rect.left + ((s.parent_rect.right - s.parent_rect.left) - s.background_bitmap.bmWidth) / 2,
          s.parent_rect.top + ((s.parent_rect.bottom - s.parent_rect.top) - s.background_bitmap.bmHeight) / 2,
          s.background_bitmap.bmWidth, s.background_bitmap.bmHeight, FALSE);
    }
    SetDlgItemTextA(hwnd, 0x6eb, g_sealed_player_context->name);
    SetDlgItemTextA(hwnd, 0x6ec, g_sealed_player_context->description);
    GetClientRect(hwnd, &s.layout_rect);
    s.layout_rect.left += 20;
    s.layout_rect.right -= 20;
    s.layout_rect.top += 20;
    s.layout_rect.bottom -= 20;
    s.layout_dc = GetDC(hwnd);
    s.text_font = (HGDIOBJ)SendDlgItemMessageA(hwnd, 0x6eb, WM_GETFONT, 0, 0);
    SelectObject(s.layout_dc, s.text_font);
    GetTextExtentPoint32A(s.layout_dc, "fun", 3, &s.text_size);
    s.button_height = s.text_size.cy * 2;
    s.text_height = s.text_size.cy;
    s.text_margin = (s.text_size.cy * 3) / 2;
    s.text_x = g_sealed_player_face_rect.right + s.text_margin / 2;
    s.text_y = g_sealed_player_face_rect.top;
    s.text_control = GetDlgItem(hwnd, 0x6eb);
    SetWindowPos(s.text_control, NULL, s.text_x, s.text_y,
        s.layout_rect.right - 10 - s.text_x, s.text_height, SWP_NOZORDER);
    s.text_y += (s.text_height * 3) / 2;
    s.text_control = GetDlgItem(hwnd, 0x6ec);
    SetWindowPos(s.text_control, NULL, s.text_x, s.text_y,
        s.layout_rect.right - 10 - s.text_x, s.layout_rect.bottom - s.text_y, SWP_NOZORDER);
    ReleaseDC(hwnd, s.layout_dc);
    SetFocus(GetDlgItem(hwnd, IDOK));
    SendMessageA(hwnd, 0x401, 1, 0);
    change_buttonclass_wndproc(hwnd);
    SetFocus(hwnd);
    SetTimer(hwnd, 1, 5000, NULL);
    return FALSE;

  case WM_DESTROY:
    shell_release_sealed_player_resources(g_sealed_player_background, g_sealed_player_button_brush,
        g_sealed_player_light_pen, g_sealed_player_dark_pen);
    KillTimer(hwnd, 1);
    return FALSE;

  case WM_TIMER:
    if (g_sealed_player_context->build_thread != NULL &&
        GetExitCodeThread(g_sealed_player_context->build_thread, &s.timer_thread_status) != 0 &&
        s.timer_thread_status == STILL_ACTIVE)
    {
      SetTimer(hwnd, wparam, 200, NULL);
    }
    else if (g_sealed_player_close_pending != 0)
    {
      EndDialog(hwnd, 0);
    }
    else
    {
      GetCursorPos(&s.cursor);
      ScreenToClient(hwnd, &s.cursor);
      GetClientRect(hwnd, &s.timer_rect);
      if (g_sealed_player_context->keep_open_under_cursor == 0 ||
          PLAYER_POINT_IN_RECT(&s.timer_rect, s.cursor.x, s.cursor.y) == 0)
        EndDialog(hwnd, 0);
    }
    return TRUE;

  case WM_KEYDOWN:
  case WM_LBUTTONDOWN:
    SendMessageA(hwnd, WM_COMMAND, IDOK, 0);
    return TRUE;

  case WM_COMMAND:
    s.command = wparam & 0xffff;
    if (s.command == IDOK || s.command == IDCANCEL)
    {
      if (g_sealed_player_context->build_thread != NULL &&
          GetExitCodeThread(g_sealed_player_context->build_thread, &s.command_thread_status) != 0 &&
          s.command_thread_status == STILL_ACTIVE)
      {
        SetTimer(hwnd, 1, 100, NULL);
        g_sealed_player_close_pending = 1;
      }
      else
        EndDialog(hwnd, 0);
    }
    return TRUE;

  case 0x4c8:
    s.new_focus = (HWND)wparam;
    s.old_focus = (HWND)lparam;
    if (s.new_focus != NULL)
      SendMessageA(hwnd, 0x401, (WPARAM)s.new_focus, 0);
    if (s.new_focus != NULL)
      InvalidateRect(s.new_focus, NULL, TRUE);
    if (s.old_focus != NULL)
      InvalidateRect(s.old_focus, NULL, TRUE);
    return FALSE;

  case WM_CTLCOLORBTN:
  case WM_CTLCOLORSTATIC:
    s.control_dc = (HDC)wparam;
    ApplyCardArtPaletteToDc(s.control_dc);
    s.control = (HWND)lparam;
    s.control_id = GetDlgCtrlID(s.control);
    if (s.control_id == 0x6ec)
      SetTextColor(s.control_dc, g_sealed_player_description_color);
    else
      SetTextColor(s.control_dc, g_sealed_player_name_color);
    SetBkMode(s.control_dc, TRANSPARENT);
    s.control_brush = GetStockObject(NULL_BRUSH);
    return (BOOL)s.control_brush;

  case WM_DRAWITEM:
    s.draw_item = (DRAWITEMSTRUCT *)lparam;
    draw_owner_draw_button_centered(s.draw_item, g_sealed_player_button_brush,
        g_sealed_player_light_pen, g_sealed_player_dark_pen, g_sealed_player_button_color, 0);
    return TRUE;

  case WM_QUERYNEWPALETTE:
  case WM_PALETTEISCHANGING:
  case WM_PALETTECHANGED:
    return handle_button_palette_message((int)hwnd, message, (int)wparam, (long)lparam);

  case WM_ERASEBKGND:
    s.erase_dc = (HDC)wparam;
    ApplyCardArtPaletteToDc(s.erase_dc);
    GetClientRect(hwnd, &s.erase_rect);
    if (g_sealed_player_background != NULL)
      DrawBitmapToRect(s.erase_dc, &s.erase_rect, g_sealed_player_background);
    else
      FillRect(s.erase_dc, &s.erase_rect, GetStockObject(GRAY_BRUSH));
    return TRUE;

  case WM_PAINT:
    s.paint_dc = BeginPaint(hwnd, &s.paint);
    ApplyCardArtPaletteToDc(s.paint_dc);
    GetClientRect(hwnd, &s.paint_rect);
    if (g_sealed_player_context->face == NULL)
      g_sealed_player_context->face = load_pic(g_sealed_player_context->face_path);
    if (g_sealed_player_context->face != NULL)
      draw_masked_bitmap_left_half_to_rect(s.paint_dc, &g_sealed_player_face_rect,
          g_sealed_player_context->face);
    EndPaint(hwnd, &s.paint);
    return TRUE;

  default:
    return FALSE;
  }
}

// FUNCTION: MAGIC 0x004cc5ee
static void shell_load_sealed_player_resources(HBITMAP *background, COLORREF *name_color,
    COLORREF *description_color, HBRUSH *button_brush, HPEN *light_pen, HPEN *dark_pen,
    COLORREF *button_color)
{
  char path[264];

  if (g_display_color_depth == 8)
  {
    sprintf(path, "%s\\WINBK_Rogue.pic", g_exp1_art_path);
    *background = load_pic(path);
    *name_color = 0x100009c;
    *description_color = 0x10000cb;
    *button_brush = CreateSolidBrush(0x100009c);
    *light_pen = CreatePen(0, 0, 0x1000057);
    *dark_pen = CreatePen(0, 0, 0x100000f);
    *button_color = 0x100009e;
  }
  else
  {
    sprintf(path, "%s\\WINBK_Rogue16.bmp", g_exp1_art_path);
    *background = shell_load_bitmap_file(path, NULL, 0);
    *name_color = 0xafd1f3;
    *description_color = 0x98a7cd;
    *button_brush = CreateSolidBrush(0xafd1f3);
    *light_pen = CreatePen(0, 0, 0xedd4cd);
    *dark_pen = CreatePen(0, 0, 0x4a3230);
    *button_color = 0xb0eff9;
  }
  if (*button_brush == NULL)
    *button_brush = GetStockObject(GRAY_BRUSH);
  if (*light_pen == NULL)
    *light_pen = GetStockObject(WHITE_PEN);
  if (*dark_pen == NULL)
    *dark_pen = GetStockObject(BLACK_PEN);
}

// FUNCTION: MAGIC 0x004cc762
static void shell_release_sealed_player_resources(HBITMAP background, HBRUSH button_brush,
    HPEN light_pen, HPEN dark_pen)
{
  if (background != NULL)
    delete_and_close_object(background);
  if (button_brush != NULL)
    DeleteObject(button_brush);
  if (light_pen != NULL)
    DeleteObject(light_pen);
  if (dark_pen != NULL)
    DeleteObject(dark_pen);
}
