#include "magic_shell_dialogs.h"
#include <commdlg.h>
#include <stdio.h>
#include <string.h>
#include "cardartlib/src/palette.h"
#include "global_strings.h"
#include "game_support.h"
#include "shared_startup.h"

extern HINSTANCE g_app_instance;
extern char g_shell_art_directory[];
int draw_masked_bitmap_left_half_to_rect(HDC dc, RECT *rect, HANDLE bitmap);

typedef struct
{
  const char *title;
  const char *player_name;
  const char *player_deck_label;
  const char *player_face;
  const char *opponent_name;
  const char *opponent_deck_label;
  const char *opponent_face;
  int flags;
  HANDLE thread;
} shell_gauntlet_matchup_context_t;

// FUNCTION: MAGIC 0x0050e515
static void shell_initialize_matchup_resources(HBITMAP *background,
                                               COLORREF *background_color,
                                               COLORREF *text_color,
                                               HBRUSH *button_brush,
                                               HPEN *light_pen,
                                               HPEN *dark_pen,
                                               COLORREF *button_text_color)
{
  char path[264];

  if (g_display_color_depth == 8)
  {
    sprintf(path, "%s\\WINBK_Versus.pic", g_shell_art_directory);
    *background = load_pic(path);
    *background_color = 0x100009c;
    *text_color = 0x1000090;
    *button_brush = CreateSolidBrush(0x100009c);
    *light_pen = CreatePen(0, 0, 0x1000057);
    *dark_pen = CreatePen(0, 0, 0x100000f);
    *button_text_color = 0x100009e;
  }
  else
  {
    sprintf(path, "%s\\WINBK_Versus16.bmp", g_shell_art_directory);
    *background = shell_load_bitmap_file(path, NULL, 0);
    *background_color = 0xafd1f3;
    *text_color = 0x8fb0cd;
    *button_brush = CreateSolidBrush(0xafd1f3);
    *light_pen = CreatePen(0, 0, 0xedd4cd);
    *dark_pen = CreatePen(0, 0, 0x4a3230);
    *button_text_color = 0xb0eff9;
  }
  if (*button_brush == NULL)
    *button_brush = GetStockObject(GRAY_BRUSH);
  if (*light_pen == NULL)
    *light_pen = GetStockObject(WHITE_PEN);
  if (*dark_pen == NULL)
    *dark_pen = GetStockObject(BLACK_PEN);
}

// FUNCTION: MAGIC 0x0050e689
static void shell_release_matchup_resources(HBITMAP background,
                                            HBRUSH button_brush,
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

// GLOBAL: MAGIC 0x00707b98
static HPEN shell_matchup_light_pen;
// GLOBAL: MAGIC 0x00707b9c
static int shell_matchup_deferred_close;
// GLOBAL: MAGIC 0x00707ba0
static COLORREF shell_matchup_text_color;
// GLOBAL: MAGIC 0x00707ba4
static HPEN shell_matchup_dark_pen;
// GLOBAL: MAGIC 0x00707ba8
static COLORREF shell_matchup_button_text_color;
// GLOBAL: MAGIC 0x00707bb0
static HBITMAP shell_matchup_opponent_face;
// GLOBAL: MAGIC 0x00707bb8
static HBITMAP shell_matchup_background;
// GLOBAL: MAGIC 0x00707bc0
static shell_gauntlet_matchup_context_t *shell_matchup_context;
// GLOBAL: MAGIC 0x00707bc4
static HBRUSH shell_matchup_button_brush;
// GLOBAL: MAGIC 0x00707bc8
static HBITMAP shell_matchup_player_face;
// GLOBAL: MAGIC 0x00707bd0
static RECT shell_matchup_opponent_rect;
// GLOBAL: MAGIC 0x00707be4
static COLORREF shell_matchup_background_color;
// GLOBAL: MAGIC 0x00707be8
static RECT shell_matchup_player_rect;

// FUNCTION: MAGIC 0x0050da81
static BOOL CALLBACK shell_gauntlet_matchup_dialog_proc(HWND hwnd, UINT message,
                                                         WPARAM wparam,
                                                         LPARAM lparam)
{
  struct
  {
    HDC paint_dc;
    PAINTSTRUCT paint;
    HDC erase_dc;
    RECT erase_rect;
    DRAWITEMSTRUCT *draw_item;
    HWND color_control;
    int control_id;
    HDC color_dc;
    HGDIOBJ color_brush;
    HWND update_second;
    HWND update_first;
    UINT command_id;
    DWORD command_exit_code;
    DWORD timer_exit_code;
    POINT cursor;
    RECT cursor_rect;
    HWND control;
    char button_text[100];
    HDC init_dc;
    int label_width;
    int vertical_position;
    HFONT font;
    RECT face_rect;
    RECT client_rect;
    SIZE text_size;
    int double_line_height;
    int line_height;
    int half_triple_line_height;
    BITMAP bitmap;
    RECT window_rect;
  } s;

  switch (message)
  {
    case WM_INITDIALOG:
      shell_matchup_context = (shell_gauntlet_matchup_context_t *)lparam;
      shell_matchup_deferred_close = 0;
      shell_initialize_matchup_resources(
          &shell_matchup_background, &shell_matchup_background_color,
          &shell_matchup_text_color, &shell_matchup_button_brush,
          &shell_matchup_light_pen, &shell_matchup_dark_pen,
          &shell_matchup_button_text_color);
      shell_matchup_player_face = load_pic(
          (char *)shell_matchup_context->player_face);
      shell_matchup_opponent_face = load_pic(
          (char *)shell_matchup_context->opponent_face);
      SetRect(&shell_matchup_player_rect, 0x3e, 0x48, 200, 0xf2);
      SetRect(&shell_matchup_opponent_rect, 0x126, 0x48, 0x1b0, 0xf2);
      if (shell_matchup_background != NULL)
      {
        GetObjectA(shell_matchup_background, sizeof(s.bitmap), &s.bitmap);
        SetWindowPos(hwnd, NULL, 0, 0, s.bitmap.bmWidth, s.bitmap.bmHeight, 6);
      }
      load_text(global_ui_strings_filename, "DIALOG_STARTEXP1MATCH");
      SetDlgItemTextA(hwnd, 0x71a, g_text_lines[0]);
      SetDlgItemTextA(hwnd, 0x71f, shell_matchup_context->title);
      SetDlgItemTextA(hwnd, 0x71b, shell_matchup_context->player_name);
      SetDlgItemTextA(hwnd, 0x71e,
                      shell_matchup_context->player_deck_label);
      SetDlgItemTextA(hwnd, 0x71c, shell_matchup_context->opponent_name);
      SetDlgItemTextA(hwnd, 0x71d,
                      shell_matchup_context->opponent_deck_label);
      GetWindowRect(hwnd, &s.window_rect);
      SetWindowPos(hwnd, NULL, s.window_rect.left,
                   (GetSystemMetrics(SM_CYSCREEN) -
                    (s.window_rect.bottom - s.window_rect.top)) / 2,
                   0, 0, 5);
      GetClientRect(hwnd, &s.client_rect);
      s.client_rect.left += 10;
      s.client_rect.right -= 10;
      s.client_rect.top += 10;
      s.client_rect.bottom -= 10;
      s.init_dc = GetDC(hwnd);
      s.font = (HFONT)SendDlgItemMessageA(hwnd, 0x71f, WM_GETFONT, 0, 0);
      SelectObject(s.init_dc, s.font);
      GetTextExtentPoint32A(s.init_dc, "fun", 3, &s.text_size);
      s.double_line_height = s.text_size.cy * 2;
      s.line_height = s.text_size.cy;
      s.half_triple_line_height = (s.text_size.cy * 3) / 2;
      s.control = GetDlgItem(hwnd, 0x71f);
      SetWindowPos(s.control, NULL, s.client_rect.left, s.client_rect.top,
                   s.client_rect.right - s.client_rect.left, s.line_height, 4);
      s.control = GetDlgItem(hwnd, 0x71a);
      GetWindowTextA(s.control, s.button_text, 100);
      GetTextExtentPoint32A(s.init_dc, s.button_text, strlen(s.button_text), &s.text_size);
      s.label_width = s.text_size.cx + s.half_triple_line_height;
      SetWindowPos(s.control, NULL,
                   s.client_rect.left + (s.client_rect.right - s.client_rect.left) / 2 -
                       s.label_width / 2,
                   s.client_rect.top + (s.client_rect.bottom - s.client_rect.top) / 2 -
                       s.line_height / 2,
                   s.label_width, s.line_height, 4);
      CopyRect(&s.face_rect, &shell_matchup_player_rect);
      s.vertical_position = s.face_rect.bottom + s.line_height;
      s.control = GetDlgItem(hwnd, 0x71b);
      SetWindowPos(s.control, NULL, s.face_rect.left, s.vertical_position,
                   s.face_rect.right - s.face_rect.left, s.line_height * 5 / 2, 4);
      s.vertical_position = s.face_rect.bottom + s.line_height * 3;
      s.control = GetDlgItem(hwnd, 0x71e);
      SetWindowPos(s.control, NULL, s.face_rect.left, s.vertical_position,
                   s.client_rect.left + (s.client_rect.right - s.client_rect.left) / 2 -
                       s.face_rect.left,
                   s.client_rect.bottom - s.vertical_position, 4);
      CopyRect(&s.face_rect, &shell_matchup_opponent_rect);
      s.vertical_position = s.face_rect.bottom + s.line_height;
      s.control = GetDlgItem(hwnd, 0x71c);
      SetWindowPos(s.control, NULL, s.face_rect.left, s.vertical_position,
                   s.face_rect.right - s.face_rect.left, s.line_height * 5 / 2, 4);
      s.vertical_position = s.face_rect.bottom + s.line_height * 3;
      s.control = GetDlgItem(hwnd, 0x71d);
      SetWindowPos(s.control, NULL, s.face_rect.left, s.vertical_position,
                   s.client_rect.right - s.face_rect.left,
                   s.client_rect.bottom - s.vertical_position, 4);
      ReleaseDC(hwnd, s.init_dc);
      SetFocus(GetDlgItem(hwnd, IDOK));
      SendMessageA(hwnd, 0x401, 1, 0);
      change_buttonclass_wndproc(hwnd);
      SetFocus(hwnd);
      SetTimer(hwnd, 1, 8000, NULL);
      return FALSE;
    case WM_DESTROY:
      shell_release_matchup_resources(
          shell_matchup_background, shell_matchup_button_brush,
          shell_matchup_light_pen, shell_matchup_dark_pen);
      if (shell_matchup_player_face != NULL)
        delete_and_close_object(shell_matchup_player_face);
      if (shell_matchup_opponent_face != NULL)
        delete_and_close_object(shell_matchup_opponent_face);
      KillTimer(hwnd, 1);
      return FALSE;
    case WM_TIMER:
      if (shell_matchup_context->thread != NULL &&
          GetExitCodeThread(shell_matchup_context->thread,
                            &s.timer_exit_code) &&
          s.timer_exit_code == STILL_ACTIVE)
        SetTimer(hwnd, wparam, 200, NULL);
      else
      {
        if (shell_matchup_deferred_close != 0)
          EndDialog(hwnd, 0);
        else
        {
          GetCursorPos(&s.cursor);
          ScreenToClient(hwnd, &s.cursor);
          GetClientRect(hwnd, &s.cursor_rect);
          if (shell_matchup_context->flags == 0 ||
              !PtInRect(&s.cursor_rect, s.cursor))
            EndDialog(hwnd, 0);
        }
      }
      return TRUE;
    case WM_KEYDOWN:
    case WM_LBUTTONDOWN:
      SendMessageA(hwnd, WM_COMMAND, IDOK, 0);
      return TRUE;
    case WM_COMMAND:
      s.command_id = wparam & 0xffff;
      if (s.command_id == IDOK)
      {
        if (shell_matchup_context->thread != NULL &&
            GetExitCodeThread(shell_matchup_context->thread,
                              &s.command_exit_code) &&
            s.command_exit_code == STILL_ACTIVE)
        {
          SetTimer(hwnd, 1, 100, NULL);
          shell_matchup_deferred_close = 1;
        }
        else
          EndDialog(hwnd, 0);
      }
      return TRUE;
    case 0x4c8:
      s.update_first = (HWND)wparam;
      s.update_second = (HWND)lparam;
      if (s.update_first != NULL)
        SendMessageA(hwnd, 0x401, (WPARAM)s.update_first, 0);
      if (s.update_first != NULL)
        InvalidateRect(s.update_first, NULL, TRUE);
      if (s.update_second != NULL)
        InvalidateRect(s.update_second, NULL, TRUE);
      return FALSE;
    case WM_CTLCOLORBTN:
    case WM_CTLCOLORSTATIC:
      s.color_dc = (HDC)wparam;
      ApplyCardArtPaletteToDc(s.color_dc);
      s.color_control = (HWND)lparam;
      s.control_id = GetDlgCtrlID(s.color_control);
      if (s.control_id == 0x71b || s.control_id == 0x71c)
        SetTextColor(s.color_dc, shell_matchup_background_color);
      else
        SetTextColor(s.color_dc, shell_matchup_text_color);
      SetBkMode(s.color_dc, TRANSPARENT);
      s.color_brush = GetStockObject(NULL_BRUSH);
      return (BOOL)(int)s.color_brush;
    case WM_DRAWITEM:
      s.draw_item = (DRAWITEMSTRUCT *)lparam;
      draw_owner_draw_button_centered(s.draw_item,
                                      shell_matchup_button_brush,
                                      shell_matchup_light_pen,
                                      shell_matchup_dark_pen,
                                      shell_matchup_button_text_color, 0);
      return TRUE;
    case 0x30f:
    case 0x310:
    case 0x311:
      return handle_button_palette_message((int)hwnd, message,
                                           (int)wparam, lparam);
    case WM_ERASEBKGND:
      s.erase_dc = (HDC)wparam;
      ApplyCardArtPaletteToDc(s.erase_dc);
      GetClientRect(hwnd, &s.erase_rect);
      if (shell_matchup_background != NULL)
        DrawBitmapToRect(s.erase_dc, &s.erase_rect, shell_matchup_background);
      else
        FillRect(s.erase_dc, &s.erase_rect, GetStockObject(GRAY_BRUSH));
      return TRUE;
    case WM_PAINT:
      s.paint_dc = BeginPaint(hwnd, &s.paint);
      ApplyCardArtPaletteToDc(s.paint_dc);
      if (shell_matchup_player_face != NULL)
        draw_masked_bitmap_left_half_to_rect(
            s.paint_dc, &shell_matchup_player_rect, shell_matchup_player_face);
      if (shell_matchup_opponent_face != NULL)
        draw_masked_bitmap_left_half_to_rect(
            s.paint_dc, &shell_matchup_opponent_rect, shell_matchup_opponent_face);
      EndPaint(hwnd, &s.paint);
      return TRUE;
  }
  return FALSE;
}

// FUNCTION: MAGIC 0x0050d8af
void shell_show_gauntlet_matchup(HWND owner, const char *title,
                                const char *player_name,
                                const char *player_deck,
                                const char *player_face,
                                const char *opponent_name,
                                const char *opponent_deck,
                                const char *opponent_face,
                                HANDLE match_thread)
{
  struct
  {
    size_t opponent_title_length;
    size_t title_length;
    char file_title[100];
    char player_deck_label[100];
    shell_gauntlet_matchup_context_t context;
    char opponent_deck_label[100];
  } s;

  load_text(global_ui_strings_filename, "DIALOG_STARTEXP1MATCH");
  if (player_deck != NULL)
  {
    GetFileTitleA(player_deck, s.file_title, 100);
    if (_stricmp(s.file_title + strlen(s.file_title) - 4, ".dck") == 0)
    {
      s.title_length = strlen(s.file_title);
      s.file_title[s.title_length - 4] = '\0';
    }
    sprintf(s.player_deck_label, g_text_lines[1], s.file_title);
  }
  else
    strcpy(s.player_deck_label, "");
  if (opponent_deck != NULL)
  {
    GetFileTitleA(opponent_deck, s.file_title, 100);
    if (_stricmp(s.file_title + strlen(s.file_title) - 4, ".dck") == 0)
    {
      s.opponent_title_length = strlen(s.file_title);
      s.file_title[s.opponent_title_length - 4] = '\0';
    }
    sprintf(s.opponent_deck_label, g_text_lines[1], s.file_title);
  }
  else
    strcpy(s.opponent_deck_label, "");
  s.context.title = title;
  s.context.player_name = player_name;
  s.context.player_deck_label = s.player_deck_label;
  s.context.player_face = player_face;
  s.context.opponent_name = opponent_name;
  s.context.opponent_deck_label = s.opponent_deck_label;
  s.context.opponent_face = opponent_face;
  s.context.thread = match_thread;
  s.context.flags = 0;
  DialogBoxParamA(g_app_instance, "Versus", owner,
                  shell_gauntlet_matchup_dialog_proc, (LPARAM)&s.context);
}
