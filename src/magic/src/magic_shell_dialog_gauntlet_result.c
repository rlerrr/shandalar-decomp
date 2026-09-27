#include <windows.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cardartlib/src/palette.h"
#include "deckdll/src/card_db.h"
#include "drawcardlib/Drawcardlib.h"
#include "duel_engine.h"
#include "game_support.h"
#include "global_other.h"
#include "global_state.h"
#include "global_strings.h"
#include "shared_startup.h"

typedef ptrdiff_t INT_PTR;

int copy_cached_library_cards_and_get_count(void *cards, int player);
void get_current_duel_selection(int *player, int *card);
void draw_owner_draw_button_centered(DRAWITEMSTRUCT *draw_item, HBRUSH brush,
                                     HPEN pen1, HPEN pen2, COLORREF color,
                                     int draw_focus);

// GLOBAL: MAGIC 0x00638c00
static HBITMAP g_shell_duel_result_background;
// GLOBAL: MAGIC 0x00638c38
static COLORREF g_shell_duel_result_text_color;
// GLOBAL: MAGIC 0x00638b28
static COLORREF g_shell_duel_result_button_text_color;
// GLOBAL: MAGIC 0x00638b78
static HBRUSH g_shell_duel_result_button_brush;
// GLOBAL: MAGIC 0x00638b6c
static HPEN g_shell_duel_result_button_pen1;
// GLOBAL: MAGIC 0x00638c4c
static HPEN g_shell_duel_result_button_pen2;
// GLOBAL: MAGIC 0x00638b38
static COLORREF g_shell_duel_result_button_unfocus_color;
// GLOBAL: MAGIC 0x00638bf8
static COLORREF g_shell_duel_result_button_focus_color;
// GLOBAL: MAGIC 0x00638c7c
static HFONT g_shell_duel_result_button_font;

// FUNCTION: MAGIC 0x0049d638
void setup_shell_duel_result_dialog_resources(HBITMAP *background,
                                              COLORREF *text_color,
                                              COLORREF *button_text_color,
                                              HBRUSH *button_brush,
                                              HPEN *pen1,
                                              HPEN *pen2,
                                              COLORREF *button_unfocus_color,
                                              COLORREF *button_focus_color)
{
  char path[264];

  sprintf(path, "%s\\WINBK_EndDuel.pic", global_duelart_path);
  *background = load_pic(path);
  *text_color = 0x1000040;
  *button_text_color = 0x1000040;
  *button_brush = CreateSolidBrush(0x100001a);
  *pen1 = CreatePen(0, 0, 0x100008c);
  *pen2 = CreatePen(0, 0, 0x1000001);
  *button_unfocus_color = 0x1000040;
  *button_focus_color = 0x10000bf;
  if (*button_brush == (HBRUSH)0)
    *button_brush = GetStockObject(GRAY_BRUSH);
  if (*pen1 == (HPEN)0)
    *pen1 = GetStockObject(6);
  if (*pen2 == (HPEN)0)
    *pen2 = GetStockObject(7);
}

// FUNCTION: MAGIC 0x0049d71e
void cleanup_shell_duel_result_dialog_resources(HBITMAP background,
                                                HBRUSH button_brush,
                                                HPEN pen1, HPEN pen2)
{
  if (background != (HBITMAP)0)
    delete_and_close_object(background);
  if (button_brush != (HBRUSH)0)
    DeleteObject(button_brush);
  if (pen1 != (HPEN)0)
    DeleteObject(pen1);
  if (pen2 != (HPEN)0)
    DeleteObject(pen2);
}

typedef struct shell_duel_result_dialog_params_struct
{
  int player_top_card;
  int opponent_top_card;
  char outcome_text[300];
  char match_progress_text[300];
  int match_finished;
  char sideboard_button_text[52];
  void (*sideboard_callback)(HWND);
  char save_button_text[52];
  void (*save_callback)(HWND);
} shell_duel_result_dialog_params_t;

// FUNCTION: MAGIC 0x0049c22c
INT_PTR CALLBACK shell_duel_result_dialog_proc(HWND hwnd, UINT msg, WPARAM wparam,
                                              LPARAM lparam)
{
  struct
  {
    HDC paint_dc;
    PAINTSTRUCT paint;
    int card_control_id;
    RECT card_rect;
    int selected_player;
    int card_id;
    int opponent_hover_card;
    POINT mouse_point;
    RECT player_card_rect;
    RECT opponent_card_rect;
    int player_hover_card;
    HDC erase_dc;
    RECT erase_rect;
    COLORREF draw_color;
    DRAWITEMSTRUCT *draw_item;
    HWND colored_control;
    int colored_control_id;
    HDC color_dc;
    HBRUSH color_brush;
    HWND old_focus;
    HWND new_focus;
    int command;
    HFONT destroy_font;
    char control_text[100];
    HWND text_window;
    RECT control_rect;
    HDC layout_dc;
    int wrapped_text_height;
    int button_spacing;
    int widest_button;
    int button_y;
    int button_x;
    HFONT layout_font;
    RECT layout_rect;
    SIZE text_extent;
    int button_height;
    int button_horizontal_padding;
    char opponent_text[200];
    LOGFONTA button_font;
    HFONT initial_font;
    RECT window_rect;
    shell_duel_result_dialog_params_t *dialog;
  } s;

  switch (msg)
  {
  case WM_INITDIALOG:
    s.dialog = (shell_duel_result_dialog_params_t *)lparam;
    SetWindowLongA(hwnd, DWL_USER, (LONG)s.dialog);
    setup_shell_duel_result_dialog_resources(
        &g_shell_duel_result_background,
        &g_shell_duel_result_text_color,
        &g_shell_duel_result_button_text_color,
        &g_shell_duel_result_button_brush,
        &g_shell_duel_result_button_pen1,
        &g_shell_duel_result_button_pen2,
        &g_shell_duel_result_button_unfocus_color,
        &g_shell_duel_result_button_focus_color);
    s.initial_font = (HFONT)SendDlgItemMessageA(hwnd, 0x6d1, WM_GETFONT, 0, 0);
    GetObjectA(s.initial_font, sizeof(s.button_font), &s.button_font);
    if (s.button_font.lfHeight > 0)
      s.button_font.lfHeight -= 2;
    else
      s.button_font.lfHeight += 2;
    g_shell_duel_result_button_font = CreateFontIndirectA(&s.button_font);
    SendDlgItemMessageA(hwnd, 0x6d2, WM_SETFONT,
                        (WPARAM)g_shell_duel_result_button_font, 0);
    SendDlgItemMessageA(hwnd, 0x6d5, WM_SETFONT,
                        (WPARAM)g_shell_duel_result_button_font, 0);
    GetWindowRect(hwnd, &s.window_rect);
    SetWindowPos(hwnd, (HWND)0, s.window_rect.left,
                 (GetSystemMetrics(SM_CYSCREEN) -
                  (s.window_rect.bottom - s.window_rect.top)) / 2,
                 0, 0, SWP_NOSIZE | SWP_NOZORDER);
    ShowWindow(GetDlgItem(hwnd, 0x6cd), SW_HIDE);
    ShowWindow(GetDlgItem(hwnd, 0x6cf), SW_HIDE);
    if (s.dialog->opponent_top_card == -1)
      ShowWindow(GetDlgItem(hwnd, 0x6cc), SW_HIDE);
    if (s.dialog->player_top_card == -1)
      ShowWindow(GetDlgItem(hwnd, 0x6ce), SW_HIDE);
    SetDlgItemTextA(hwnd, IDOK, gs_ok_00924800);
    load_text(global_ui_strings_filename, "DIALOG_ENDEXP1DUEL");
    sprintf(s.opponent_text, g_text_lines[0], g_saved_player_name);
    SetDlgItemTextA(hwnd, 0x6cc, s.opponent_text);
    SetDlgItemTextA(hwnd, 0x6ce, g_text_lines[1]);
    SetDlgItemTextA(hwnd, 0x6d3, s.dialog->outcome_text);
    SetDlgItemTextA(hwnd, 0x6d4, s.dialog->match_progress_text);
    SetDlgItemTextA(hwnd, 0x6d2, s.dialog->sideboard_button_text);
    if (s.dialog->sideboard_callback == 0)
      ShowWindow(GetDlgItem(hwnd, 0x6d2), SW_HIDE);
    SetDlgItemTextA(hwnd, 0x6d5, s.dialog->save_button_text);
    if (s.dialog->save_callback == 0)
      ShowWindow(GetDlgItem(hwnd, 0x6d5), SW_HIDE);
    if (s.dialog->match_finished != 0)
    {
      SetDlgItemTextA(hwnd, 0x6d0, g_text_lines[4]);
      ShowWindow(GetDlgItem(hwnd, 0x6d1), SW_HIDE);
      ShowWindow(GetDlgItem(hwnd, 0x6d2), SW_HIDE);
      ShowWindow(GetDlgItem(hwnd, 0x6d5), SW_HIDE);
    }
    else
    {
      SetDlgItemTextA(hwnd, 0x6d0, g_text_lines[2]);
      SetDlgItemTextA(hwnd, 0x6d1, g_text_lines[3]);
    }

    GetClientRect(hwnd, &s.layout_rect);
    s.layout_rect.left += 0x14;
    s.layout_rect.right -= 0x14;
    s.layout_rect.top += 0x14;
    s.layout_rect.bottom -= 0x14;
    s.layout_font = (HFONT)SendDlgItemMessageA(hwnd, 0x6d1, WM_GETFONT, 0, 0);
    s.layout_dc = GetDC(hwnd);
    SelectObject(s.layout_dc, s.layout_font);
    GetTextExtentPoint32A(s.layout_dc, "fun", 3, &s.text_extent);
    s.button_height = s.text_extent.cy * 2;
    s.button_horizontal_padding = (s.text_extent.cy * 3) / 2;
    s.button_spacing = s.button_height / 4;
    s.text_window = GetDlgItem(hwnd, 0x6d3);
    GetWindowTextA(s.text_window, s.control_text, 100);
    GetClientRect(s.text_window, &s.control_rect);
    s.wrapped_text_height = DrawTextA(s.layout_dc, s.control_text, -1,
                                    &s.control_rect, DT_WORDBREAK | DT_CALCRECT);
    GetWindowRect(s.text_window, &s.control_rect);
    MapWindowPoints((HWND)0, hwnd, (LPPOINT)&s.control_rect, 2);
    SetWindowPos(s.text_window, (HWND)0, 0, 0,
                 s.control_rect.right - s.control_rect.left,
                 s.button_height + s.wrapped_text_height, 6);
    s.text_window = GetDlgItem(hwnd, 0x6d4);
    SetWindowPos(s.text_window, (HWND)0, s.control_rect.left,
                 s.control_rect.top + s.button_height + s.button_height +
                     s.wrapped_text_height,
                 s.control_rect.right - s.control_rect.left,
                 s.wrapped_text_height, 4);
    s.text_window = GetDlgItem(hwnd, 0x6d4);
    GetWindowTextA(s.text_window, s.control_text, 100);
    GetClientRect(s.text_window, &s.control_rect);
    s.wrapped_text_height = DrawTextA(s.layout_dc, s.control_text, -1,
                                    &s.control_rect, DT_WORDBREAK | DT_CALCRECT);
    GetWindowRect(s.text_window, &s.control_rect);
    MapWindowPoints((HWND)0, hwnd, (LPPOINT)&s.control_rect, 2);
    SetWindowPos(s.text_window, (HWND)0, 0, 0,
                 s.control_rect.right - s.control_rect.left,
                 s.button_height + s.wrapped_text_height, 6);

    s.widest_button = 0;
    s.text_window = GetDlgItem(hwnd, 0x6d0);
    GetWindowTextA(s.text_window, s.control_text, 100);
    GetTextExtentPoint32A(s.layout_dc, s.control_text, strlen(s.control_text),
                          &s.text_extent);
    if (s.widest_button < s.text_extent.cx)
      s.widest_button = s.text_extent.cx;
    s.text_window = GetDlgItem(hwnd, 0x6d1);
    GetWindowTextA(s.text_window, s.control_text, 100);
    GetTextExtentPoint32A(s.layout_dc, s.control_text, strlen(s.control_text),
                          &s.text_extent);
    if (s.widest_button < s.text_extent.cx)
      s.widest_button = s.text_extent.cx;
    s.button_x = s.layout_rect.left + 10;
    s.button_y = s.layout_rect.bottom - s.button_spacing - s.button_height;
    s.text_window = GetDlgItem(hwnd, 0x6d1);
    SetWindowPos(s.text_window, (HWND)0, s.button_x, s.button_y,
                 s.button_horizontal_padding + s.widest_button, s.button_height, 4);
    s.button_y -= s.button_spacing + s.button_height;
    s.text_window = GetDlgItem(hwnd, 0x6d0);
    SetWindowPos(s.text_window, (HWND)0, s.button_x, s.button_y,
                 s.button_horizontal_padding + s.widest_button, s.button_height, 4);
    s.button_y -= s.button_spacing + s.button_spacing + s.button_height;

    s.layout_font = (HFONT)SendDlgItemMessageA(hwnd, 0x6d2, WM_GETFONT, 0, 0);
    SelectObject(s.layout_dc, s.layout_font);
    GetTextExtentPoint32A(s.layout_dc, "fun", 3, &s.text_extent);
    s.button_height = s.text_extent.cy * 2;
    s.button_horizontal_padding = (s.text_extent.cy * 3) / 2;
    s.button_spacing = s.button_height / 4;
    s.widest_button = 0;
    s.text_window = GetDlgItem(hwnd, 0x6d2);
    GetWindowTextA(s.text_window, s.control_text, 100);
    GetTextExtentPoint32A(s.layout_dc, s.control_text, strlen(s.control_text),
                          &s.text_extent);
    if (s.widest_button < s.text_extent.cx)
      s.widest_button = s.text_extent.cx;
    s.text_window = GetDlgItem(hwnd, 0x6d5);
    GetWindowTextA(s.text_window, s.control_text, 100);
    GetTextExtentPoint32A(s.layout_dc, s.control_text, strlen(s.control_text),
                          &s.text_extent);
    if (s.widest_button < s.text_extent.cx)
      s.widest_button = s.text_extent.cx;
    GetWindowRect(GetDlgItem(hwnd, 0x6d0), &s.control_rect);
    MapWindowPoints((HWND)0, hwnd, (LPPOINT)&s.control_rect, 2);
    s.button_x = s.control_rect.left +
               (s.control_rect.right - s.control_rect.left) / 2 -
               (s.button_horizontal_padding + s.widest_button) / 2;
    s.text_window = GetDlgItem(hwnd, 0x6d5);
    SetWindowPos(s.text_window, (HWND)0, s.button_x, s.button_y,
                 s.button_horizontal_padding + s.widest_button, s.button_height, 4);
    s.button_y -= s.button_spacing + s.button_height;
    s.text_window = GetDlgItem(hwnd, 0x6d2);
    SetWindowPos(s.text_window, (HWND)0, s.button_x, s.button_y,
                 s.button_horizontal_padding + s.widest_button, s.button_height, 4);
    ReleaseDC(hwnd, s.layout_dc);
    SetFocus(GetDlgItem(hwnd, 0x6d0));
    SendMessageA(hwnd, 0x401, 0x6d0, 0);
    change_buttonclass_wndproc(hwnd);
    return FALSE;

  case WM_DESTROY:
    cleanup_shell_duel_result_dialog_resources(
        g_shell_duel_result_background, g_shell_duel_result_button_brush,
        g_shell_duel_result_button_pen1, g_shell_duel_result_button_pen2);
    s.destroy_font = (HFONT)SendDlgItemMessageA(hwnd, 0x6d2, WM_GETFONT, 0, 0);
    SendDlgItemMessageA(hwnd, 0x6d2, WM_SETFONT, 0, 0);
    SendDlgItemMessageA(hwnd, 0x6d5, WM_SETFONT, 0, 0);
    DeleteObject(s.destroy_font);
    return FALSE;

  case WM_COMMAND:
    s.command = wparam & 0xffff;
    s.dialog = (shell_duel_result_dialog_params_t *)GetWindowLongA(hwnd, DWL_USER);
    switch (s.command)
    {
    case 0x6d0:
    case IDOK:
      EndDialog(hwnd, 1);
      break;
    case 0x6d1:
    case IDCANCEL:
      if (s.dialog->match_finished != 0)
        EndDialog(hwnd, 1);
      else
        EndDialog(hwnd, 0);
      break;
    case 0x6d2:
      if (s.dialog->sideboard_callback != 0)
        s.dialog->sideboard_callback(hwnd);
      break;
    case 0x6d5:
      if (s.dialog->save_callback != 0)
        s.dialog->save_callback(hwnd);
      break;
    }
    return TRUE;

  case 0x4c8:
    s.new_focus = (HWND)wparam;
    s.old_focus = (HWND)lparam;
    if (s.new_focus != 0)
      SendMessageA(hwnd, 0x401, (WPARAM)s.new_focus, 0);
    if (s.new_focus != 0)
      InvalidateRect(s.new_focus, (RECT *)0, TRUE);
    if (s.old_focus != 0)
      InvalidateRect(s.old_focus, (RECT *)0, TRUE);
    return FALSE;

  case WM_CTLCOLORBTN:
  case WM_CTLCOLORSTATIC:
    s.color_dc = (HDC)wparam;
    ApplyCardArtPaletteToDc(s.color_dc);
    s.colored_control = (HWND)lparam;
    s.colored_control_id = GetDlgCtrlID(s.colored_control);
    if (s.colored_control_id == 0x6d0 || s.colored_control_id == 0x6d1 ||
        s.colored_control_id == 0x6d2 || s.colored_control_id == 0x6d5)
    {
      if (GetFocus() == s.colored_control)
        SetTextColor(s.color_dc, g_shell_duel_result_button_focus_color);
      else
        SetTextColor(s.color_dc, g_shell_duel_result_button_text_color);
      SetBkMode(s.color_dc, TRANSPARENT);
      s.color_brush = (HBRUSH)GetStockObject(NULL_BRUSH);
    }
    else
    {
      SetTextColor(s.color_dc, g_shell_duel_result_text_color);
      SetBkMode(s.color_dc, TRANSPARENT);
      s.color_brush = (HBRUSH)GetStockObject(NULL_BRUSH);
    }
    return (INT_PTR)s.color_brush;

  case WM_DRAWITEM:
    s.draw_item = (DRAWITEMSTRUCT *)lparam;
    if (GetFocus() == s.draw_item->hwndItem)
      s.draw_color = g_shell_duel_result_button_focus_color;
    else
      s.draw_color = g_shell_duel_result_button_unfocus_color;
    if (*(int *)&gs_window_title_your_hand_00777bf0[20] == 0)
      s.draw_color = g_shell_duel_result_button_unfocus_color;
    draw_owner_draw_button_centered(s.draw_item,
                                    g_shell_duel_result_button_brush,
                                    g_shell_duel_result_button_pen1,
                                    g_shell_duel_result_button_pen2,
                                    s.draw_color, 0);
    return TRUE;

  case WM_QUERYNEWPALETTE:
  case WM_PALETTECHANGED:
  case WM_PALETTEISCHANGING:
    return handle_button_palette_message((int)hwnd, msg, (int)wparam,
                                         (int)lparam);

  case WM_ERASEBKGND:
    s.erase_dc = (HDC)wparam;
    ApplyCardArtPaletteToDc(s.erase_dc);
    GetClientRect(hwnd, &s.erase_rect);
    if (g_shell_duel_result_background != (HBITMAP)0)
      DrawBitmapToRect(s.erase_dc, &s.erase_rect, g_shell_duel_result_background);
    else
      FillRect(s.erase_dc, &s.erase_rect, GetStockObject(GRAY_BRUSH));
    return TRUE;

  case WM_LBUTTONDOWN:
    SendMessageA(hwnd, WM_SYSCOMMAND, 0xf012, 0);
    return TRUE;

  case WM_MOUSEMOVE:
  case WM_RBUTTONDOWN:
    s.mouse_point.x = lparam & 0xffff;
    s.mouse_point.y = HIWORD(lparam);
    s.dialog = (shell_duel_result_dialog_params_t *)GetWindowLongA(hwnd, DWL_USER);
    if ((msg == WM_MOUSEMOVE && g_duel_interface_options.layout != 2) ||
        (msg == WM_RBUTTONDOWN && g_duel_interface_options.layout == 2))
    {
      s.player_hover_card = s.dialog->player_top_card;
      GetWindowRect(GetDlgItem(hwnd, 0x6cf), &s.player_card_rect);
      MapWindowPoints((HWND)0, hwnd, (LPPOINT)&s.player_card_rect, 2);
      s.opponent_hover_card = s.dialog->opponent_top_card;
      GetWindowRect(GetDlgItem(hwnd, 0x6cd), &s.opponent_card_rect);
      MapWindowPoints((HWND)0, hwnd, (LPPOINT)&s.opponent_card_rect, 2);
      if (s.player_hover_card != -1 &&
          PtInRect(&s.player_card_rect, s.mouse_point) != 0)
        SendMessageA(g_duel_card_preview_window_hwnd, 0x401,
                     s.player_hover_card, 0);
      else if (s.opponent_hover_card != -1 &&
               PtInRect(&s.opponent_card_rect, s.mouse_point) != 0)
        SendMessageA(g_duel_card_preview_window_hwnd, 0x401,
                     s.opponent_hover_card, 0);
    }
    return FALSE;

  case WM_PAINT:
    s.dialog = (shell_duel_result_dialog_params_t *)GetWindowLongA(hwnd, DWL_USER);
    UpdateWindow(GetDlgItem(hwnd, 0x6d3));
    get_current_duel_selection(&s.selected_player, (int *)0);
    if (s.selected_player == 0)
      UpdateWindow(GetDlgItem(hwnd, 0x6ce));
    else
      UpdateWindow(GetDlgItem(hwnd, 0x6cc));
    s.paint_dc = BeginPaint(hwnd, &s.paint);
    if (s.paint_dc != (HDC)0)
    {
      ApplyCardArtPaletteToDc(s.paint_dc);
      if (s.selected_player == 0)
        s.card_id = s.dialog->player_top_card;
      else
        s.card_id = s.dialog->opponent_top_card;
      if (s.selected_player == 0)
        s.card_control_id = 0x6cf;
      else
        s.card_control_id = 0x6cd;
      if (s.card_id != -1)
      {
        GetWindowRect(GetDlgItem(hwnd, s.card_control_id), &s.card_rect);
        MapWindowPoints((HWND)0, hwnd, (LPPOINT)&s.card_rect, 2);
        DrawFullCard(s.paint_dc, &s.card_rect, global_raw_cards_storage + s.card_id,
                     0, 0x12, 0, gs_illus_00789130);
      }
      if (s.selected_player == 0)
        s.card_id = s.dialog->opponent_top_card;
      else
        s.card_id = s.dialog->player_top_card;
      if (s.selected_player == 0)
        s.card_control_id = 0x6cd;
      else
        s.card_control_id = 0x6cf;
      if (s.card_id != -1)
      {
        GetWindowRect(GetDlgItem(hwnd, s.card_control_id), &s.card_rect);
        MapWindowPoints((HWND)0, hwnd, (LPPOINT)&s.card_rect, 2);
        DrawFullCard(s.paint_dc, &s.card_rect, global_raw_cards_storage + s.card_id,
                     0, 0x12, 0, gs_illus_00789130);
      }
      EndPaint(hwnd, &s.paint);
    }
    return TRUE;

  default:
    return FALSE;
  }
}

// FUNCTION: MAGIC 0x0049be8d
INT_PTR end_the_game(int duel_result, char *match_progress, int match_finished,
                     char *sideboard_button,
                     void (*sideboard_callback)(HWND),
                     char *save_button, void (*save_callback)(HWND))
{
  struct
  {
    shell_duel_result_dialog_params_t dialog;
    int random_line;
    char formatted_text[252];
    int text_line_count;
    int cached_library_cards[500];
    int opponent_card;
    INT_PTR dialog_result;
    int player_card;
  } s;

  KillTimer(g_duel_window_hwnd, g_duel_timer_id);
  if (duel_result == 0 || duel_result == 1 || duel_result == -1)
  {
    if (copy_cached_library_cards_and_get_count(s.cached_library_cards, 0) != 0)
      s.player_card = s.cached_library_cards[0];
    else
      s.player_card = -1;
    if (copy_cached_library_cards_and_get_count(s.cached_library_cards, 1) != 0)
      s.opponent_card = s.cached_library_cards[0];
    else
      s.opponent_card = -1;
    s.dialog.player_top_card = s.player_card;
    s.dialog.opponent_top_card = s.opponent_card;
  }
  else
  {
    s.dialog.player_top_card = -1;
    s.dialog.opponent_top_card = -1;
  }

  if (duel_result == 0 || duel_result == 1 || duel_result == -1)
  {
    if (duel_result == 0)
      s.text_line_count = load_text(global_ui_strings_filename, "DIALOG_ENDEXP1DUEL_LOSE");
    else if (duel_result == 1)
      s.text_line_count = load_text(global_ui_strings_filename, "DIALOG_ENDEXP1DUEL_WIN");
    else if (duel_result == -1)
      s.text_line_count = load_text(global_ui_strings_filename, "DIALOG_ENDEXP1DUEL_TIE");
    else
      s.text_line_count = -1;

    if ((g_duel_network_flags & 2) == 0 && s.text_line_count > 0)
    {
      s.random_line = rand() % s.text_line_count;
      strcpy(s.dialog.outcome_text, g_text_lines[s.random_line]);
    }
    else if ((g_duel_network_flags & 2) != 0)
    {
      load_text(global_ui_strings_filename, "DIALOG_SHANDALARENDDUEL");
      if (duel_result == 1)
        strcpy(s.dialog.outcome_text, g_text_lines[1]);
      else if (duel_result == 0)
      {
        sprintf(s.formatted_text, g_text_lines[0], g_saved_player_name);
        strcpy(s.dialog.outcome_text, s.formatted_text);
      }
      else if (duel_result == -1)
        strcpy(s.dialog.outcome_text, g_text_lines[2]);
      else
        strcpy(s.dialog.outcome_text, "");
    }
    else
    {
      if (duel_result == 0)
        strcpy(s.dialog.outcome_text, "You lost.");
      else if (duel_result == 1)
        strcpy(s.dialog.outcome_text, "You won!");
      else if (duel_result == -1)
        strcpy(s.dialog.outcome_text, "We tied.");
      else
        strcpy(s.dialog.outcome_text, "");
    }
  }
  else
    strcpy(s.dialog.outcome_text, "");

  strcpy(s.dialog.match_progress_text, match_progress);
  s.dialog.match_finished = match_finished;
  strcpy(s.dialog.sideboard_button_text, sideboard_button);
  s.dialog.sideboard_callback = sideboard_callback;
  strcpy(s.dialog.save_button_text, save_button);
  s.dialog.save_callback = save_callback;
  s.dialog_result = DialogBoxParamA(g_app_instance, (LPCSTR)0xfa,
                                     g_duel_window_hwnd,
                                     shell_duel_result_dialog_proc,
                                     (LPARAM)&s.dialog);
  return s.dialog_result;
}
