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

// TODO: Refine the dialog's stack layout and message dispatch to match assembly.
// FUNCTION: MAGIC 0x0049c22c
INT_PTR CALLBACK shell_duel_result_dialog_proc(HWND hwnd, UINT msg, WPARAM wparam,
                                                LPARAM lparam)
{
  shell_duel_result_dialog_params_t *dialog;
  LOGFONTA button_font;
  RECT window_rect;
  HWND control;
  HFONT old_font;
  char opponent_text[200];
  int command;
  HDC dc;
  RECT rect;
  DRAWITEMSTRUCT *draw_item;
  HWND colored_control;
  int colored_control_id;
  COLORREF draw_color;
  int selected_player;
  int card_id;
  int card_control_id;
  PAINTSTRUCT paint;
  POINT mouse_point;
  RECT opponent_card_rect;
  RECT player_card_rect;
  RECT layout_rect;
  RECT control_rect;
  SIZE text_extent;
  HDC layout_dc;
  HWND text_window;
  char control_text[100];
  int button_height;
  int button_horizontal_padding;
  int button_spacing;
  int wrapped_text_height;
  int widest_button;
  int button_x;
  int button_y;

  switch (msg)
  {
  case WM_INITDIALOG:
    dialog = (shell_duel_result_dialog_params_t *)lparam;
    SetWindowLongA(hwnd, DWL_USER, (LONG)dialog);
    setup_shell_duel_result_dialog_resources(
        &g_shell_duel_result_background,
        &g_shell_duel_result_text_color,
        &g_shell_duel_result_button_text_color,
        &g_shell_duel_result_button_brush,
        &g_shell_duel_result_button_pen1,
        &g_shell_duel_result_button_pen2,
        &g_shell_duel_result_button_unfocus_color,
        &g_shell_duel_result_button_focus_color);
    old_font = (HFONT)SendDlgItemMessageA(hwnd, 0x6d1, WM_GETFONT, 0, 0);
    GetObjectA(old_font, sizeof(button_font), &button_font);
    if (button_font.lfHeight < 1)
      button_font.lfHeight += 2;
    else
      button_font.lfHeight -= 2;
    g_shell_duel_result_button_font = CreateFontIndirectA(&button_font);
    SendDlgItemMessageA(hwnd, 0x6d2, WM_SETFONT,
                        (WPARAM)g_shell_duel_result_button_font, 0);
    SendDlgItemMessageA(hwnd, 0x6d5, WM_SETFONT,
                        (WPARAM)g_shell_duel_result_button_font, 0);
    GetWindowRect(hwnd, &window_rect);
    SetWindowPos(hwnd, (HWND)0, window_rect.left,
                 (GetSystemMetrics(SM_CYSCREEN) -
                  (window_rect.bottom - window_rect.top)) / 2,
                 0, 0, SWP_NOSIZE | SWP_NOZORDER);
    ShowWindow(GetDlgItem(hwnd, 0x6cd), SW_HIDE);
    ShowWindow(GetDlgItem(hwnd, 0x6cf), SW_HIDE);
    if (dialog->opponent_top_card == -1)
      ShowWindow(GetDlgItem(hwnd, 0x6cc), SW_HIDE);
    if (dialog->player_top_card == -1)
      ShowWindow(GetDlgItem(hwnd, 0x6ce), SW_HIDE);
    SetDlgItemTextA(hwnd, IDOK, gs_ok_00924800);
    load_text(global_ui_strings_filename, "DIALOG_ENDEXP1DUEL");
    sprintf(opponent_text, g_text_lines[0], g_saved_player_name);
    SetDlgItemTextA(hwnd, 0x6cc, opponent_text);
    SetDlgItemTextA(hwnd, 0x6ce, g_text_lines[1]);
    SetDlgItemTextA(hwnd, 0x6d3, dialog->outcome_text);
    SetDlgItemTextA(hwnd, 0x6d4, dialog->match_progress_text);
    SetDlgItemTextA(hwnd, 0x6d2, dialog->sideboard_button_text);
    if (dialog->sideboard_callback == 0)
      ShowWindow(GetDlgItem(hwnd, 0x6d2), SW_HIDE);
    SetDlgItemTextA(hwnd, 0x6d5, dialog->save_button_text);
    if (dialog->save_callback == 0)
      ShowWindow(GetDlgItem(hwnd, 0x6d5), SW_HIDE);
    if (dialog->match_finished == 0)
    {
      SetDlgItemTextA(hwnd, 0x6d0, g_text_lines[2]);
      SetDlgItemTextA(hwnd, 0x6d1, g_text_lines[3]);
    }
    else
    {
      SetDlgItemTextA(hwnd, 0x6d0, g_text_lines[4]);
      ShowWindow(GetDlgItem(hwnd, 0x6d1), SW_HIDE);
      ShowWindow(GetDlgItem(hwnd, 0x6d2), SW_HIDE);
      ShowWindow(GetDlgItem(hwnd, 0x6d5), SW_HIDE);
    }

    GetClientRect(hwnd, &layout_rect);
    layout_rect.left += 0x14;
    layout_rect.right -= 0x14;
    layout_rect.top += 0x14;
    layout_rect.bottom -= 0x14;
    old_font = (HFONT)SendDlgItemMessageA(hwnd, 0x6d1, WM_GETFONT, 0, 0);
    layout_dc = GetDC(hwnd);
    SelectObject(layout_dc, old_font);
    GetTextExtentPoint32A(layout_dc, "fun", 3, &text_extent);
    button_height = text_extent.cy * 2;
    button_horizontal_padding = (text_extent.cy * 3) / 2;
    button_spacing = (button_height + (button_height >> 31 & 3)) >> 2;
    text_window = GetDlgItem(hwnd, 0x6d3);
    GetWindowTextA(text_window, control_text, 100);
    GetClientRect(text_window, &control_rect);
    wrapped_text_height = DrawTextA(layout_dc, control_text, -1,
                                    &control_rect, DT_WORDBREAK | DT_CALCRECT);
    GetWindowRect(text_window, &control_rect);
    MapWindowPoints((HWND)0, hwnd, (LPPOINT)&control_rect, 2);
    SetWindowPos(text_window, (HWND)0, 0, 0,
                 control_rect.right - control_rect.left,
                 button_height + wrapped_text_height, 6);
    text_window = GetDlgItem(hwnd, 0x6d4);
    SetWindowPos(text_window, (HWND)0, control_rect.left,
                 control_rect.top + button_height * 2 + wrapped_text_height,
                 control_rect.right - control_rect.left,
                 wrapped_text_height, 4);
    text_window = GetDlgItem(hwnd, 0x6d4);
    GetWindowTextA(text_window, control_text, 100);
    GetClientRect(text_window, &control_rect);
    wrapped_text_height = DrawTextA(layout_dc, control_text, -1,
                                    &control_rect, DT_WORDBREAK | DT_CALCRECT);
    GetWindowRect(text_window, &control_rect);
    MapWindowPoints((HWND)0, hwnd, (LPPOINT)&control_rect, 2);
    SetWindowPos(text_window, (HWND)0, 0, 0,
                 control_rect.right - control_rect.left,
                 button_height + wrapped_text_height, 6);

    widest_button = 0;
    text_window = GetDlgItem(hwnd, 0x6d0);
    GetWindowTextA(text_window, control_text, 100);
    GetTextExtentPoint32A(layout_dc, control_text, strlen(control_text),
                          &text_extent);
    if (widest_button < text_extent.cx)
      widest_button = text_extent.cx;
    text_window = GetDlgItem(hwnd, 0x6d1);
    GetWindowTextA(text_window, control_text, 100);
    GetTextExtentPoint32A(layout_dc, control_text, strlen(control_text),
                          &text_extent);
    if (widest_button < text_extent.cx)
      widest_button = text_extent.cx;
    button_x = layout_rect.left + 10;
    button_y = layout_rect.bottom - button_spacing - button_height;
    text_window = GetDlgItem(hwnd, 0x6d1);
    SetWindowPos(text_window, (HWND)0, button_x, button_y,
                 button_horizontal_padding + widest_button, button_height, 4);
    button_y -= button_spacing + button_height;
    text_window = GetDlgItem(hwnd, 0x6d0);
    SetWindowPos(text_window, (HWND)0, button_x, button_y,
                 button_horizontal_padding + widest_button, button_height, 4);
    button_y -= button_spacing * 2 + button_height;

    old_font = (HFONT)SendDlgItemMessageA(hwnd, 0x6d2, WM_GETFONT, 0, 0);
    SelectObject(layout_dc, old_font);
    GetTextExtentPoint32A(layout_dc, "fun", 3, &text_extent);
    button_height = text_extent.cy * 2;
    button_horizontal_padding = (text_extent.cy * 3) / 2;
    button_spacing = (button_height + (button_height >> 31 & 3)) >> 2;
    widest_button = 0;
    text_window = GetDlgItem(hwnd, 0x6d2);
    GetWindowTextA(text_window, control_text, 100);
    GetTextExtentPoint32A(layout_dc, control_text, strlen(control_text),
                          &text_extent);
    if (widest_button < text_extent.cx)
      widest_button = text_extent.cx;
    text_window = GetDlgItem(hwnd, 0x6d5);
    GetWindowTextA(text_window, control_text, 100);
    GetTextExtentPoint32A(layout_dc, control_text, strlen(control_text),
                          &text_extent);
    if (widest_button < text_extent.cx)
      widest_button = text_extent.cx;
    text_window = GetDlgItem(hwnd, 0x6d0);
    GetWindowRect(text_window, &control_rect);
    MapWindowPoints((HWND)0, hwnd, (LPPOINT)&control_rect, 2);
    button_x = control_rect.left +
               (control_rect.right - control_rect.left) / 2 -
               (button_horizontal_padding + widest_button) / 2;
    text_window = GetDlgItem(hwnd, 0x6d5);
    SetWindowPos(text_window, (HWND)0, button_x, button_y,
                 button_horizontal_padding + widest_button, button_height, 4);
    button_y -= button_spacing + button_height;
    text_window = GetDlgItem(hwnd, 0x6d2);
    SetWindowPos(text_window, (HWND)0, button_x, button_y,
                 button_horizontal_padding + widest_button, button_height, 4);
    ReleaseDC(hwnd, layout_dc);
    control = GetDlgItem(hwnd, 0x6d0);
    SetFocus(control);
    SendMessageA(hwnd, 0x401, 0x6d0, 0);
    change_buttonclass_wndproc(hwnd);
    return FALSE;

  case WM_COMMAND:
    command = LOWORD(wparam);
    dialog = (shell_duel_result_dialog_params_t *)GetWindowLongA(hwnd, DWL_USER);
    if (command == 0x6d0 || command == IDOK)
    {
      EndDialog(hwnd, 1);
      return TRUE;
    }
    if (command == 0x6d2)
    {
      if (dialog->sideboard_callback != 0)
        dialog->sideboard_callback(hwnd);
      return TRUE;
    }
    if (command == 0x6d5)
    {
      if (dialog->save_callback != 0)
        dialog->save_callback(hwnd);
      return TRUE;
    }
    if (command == 0x6d1 || command == IDCANCEL)
    {
      EndDialog(hwnd, dialog->match_finished != 0);
      return TRUE;
    }
    return TRUE;

  case WM_DESTROY:
    cleanup_shell_duel_result_dialog_resources(
        g_shell_duel_result_background, g_shell_duel_result_button_brush,
        g_shell_duel_result_button_pen1, g_shell_duel_result_button_pen2);
    old_font = (HFONT)SendDlgItemMessageA(hwnd, 0x6d2, WM_GETFONT, 0, 0);
    SendDlgItemMessageA(hwnd, 0x6d2, WM_SETFONT, 0, 0);
    SendDlgItemMessageA(hwnd, 0x6d5, WM_SETFONT, 0, 0);
    DeleteObject(old_font);
    return FALSE;

  case WM_ERASEBKGND:
    dc = (HDC)wparam;
    ApplyCardArtPaletteToDc(dc);
    GetClientRect(hwnd, &rect);
    if (g_shell_duel_result_background == (HBITMAP)0)
      FillRect(dc, &rect, GetStockObject(GRAY_BRUSH));
    else
      DrawBitmapToRect(dc, &rect, g_shell_duel_result_background);
    return TRUE;

  case WM_CTLCOLORBTN:
  case WM_CTLCOLORSTATIC:
    dc = (HDC)wparam;
    ApplyCardArtPaletteToDc(dc);
    colored_control = (HWND)lparam;
    colored_control_id = GetDlgCtrlID(colored_control);
    if (colored_control_id == 0x6d0 || colored_control_id == 0x6d1 ||
        colored_control_id == 0x6d2 || colored_control_id == 0x6d5)
    {
      if (GetFocus() == colored_control)
        SetTextColor(dc, g_shell_duel_result_button_focus_color);
      else
        SetTextColor(dc, g_shell_duel_result_button_text_color);
    }
    else
      SetTextColor(dc, g_shell_duel_result_text_color);
    SetBkMode(dc, TRANSPARENT);
    return (INT_PTR)GetStockObject(NULL_BRUSH);

  case WM_DRAWITEM:
    draw_item = (DRAWITEMSTRUCT *)lparam;
    if (GetFocus() == draw_item->hwndItem)
      draw_color = g_shell_duel_result_button_focus_color;
    else
      draw_color = g_shell_duel_result_button_unfocus_color;
    if (*(int *)&gs_window_title_your_hand_00777bf0[20] == 0)
      draw_color = g_shell_duel_result_button_unfocus_color;
    draw_owner_draw_button_centered(draw_item,
                                    g_shell_duel_result_button_brush,
                                    g_shell_duel_result_button_pen1,
                                    g_shell_duel_result_button_pen2,
                                    draw_color, 0);
    return TRUE;

  case WM_QUERYNEWPALETTE:
  case WM_PALETTECHANGED:
  case WM_PALETTEISCHANGING:
    return handle_button_palette_message((int)hwnd, msg, (int)wparam,
                                         (int)lparam);

  case 0x4c8:
    if (wparam != 0)
      SendMessageA(hwnd, 0x401, wparam, 0);
    if (wparam != 0)
      InvalidateRect((HWND)wparam, (RECT *)0, TRUE);
    if (lparam != 0)
      InvalidateRect((HWND)lparam, (RECT *)0, TRUE);
    return FALSE;

  case WM_PAINT:
    dialog = (shell_duel_result_dialog_params_t *)GetWindowLongA(hwnd, DWL_USER);
    UpdateWindow(GetDlgItem(hwnd, 0x6d3));
    get_current_duel_selection(&selected_player, (int *)0);
    if (selected_player == 0)
      UpdateWindow(GetDlgItem(hwnd, 0x6ce));
    else
      UpdateWindow(GetDlgItem(hwnd, 0x6cc));
    dc = BeginPaint(hwnd, &paint);
    if (dc != (HDC)0)
    {
      ApplyCardArtPaletteToDc(dc);
      if (selected_player == 0)
      {
        card_id = dialog->player_top_card;
        card_control_id = 0x6cf;
      }
      else
      {
        card_id = dialog->opponent_top_card;
        card_control_id = 0x6cd;
      }
      if (card_id != -1)
      {
        GetWindowRect(GetDlgItem(hwnd, card_control_id), &rect);
        MapWindowPoints((HWND)0, hwnd, (LPPOINT)&rect, 2);
        DrawFullCard(dc, &rect, global_raw_cards_storage + card_id,
                     0, 0x12, 0, gs_illus_00789130);
      }
      if (selected_player == 0)
      {
        card_id = dialog->opponent_top_card;
        card_control_id = 0x6cd;
      }
      else
      {
        card_id = dialog->player_top_card;
        card_control_id = 0x6cf;
      }
      if (card_id != -1)
      {
        GetWindowRect(GetDlgItem(hwnd, card_control_id), &rect);
        MapWindowPoints((HWND)0, hwnd, (LPPOINT)&rect, 2);
        DrawFullCard(dc, &rect, global_raw_cards_storage + card_id,
                     0, 0x12, 0, gs_illus_00789130);
      }
      EndPaint(hwnd, &paint);
    }
    return TRUE;

  case WM_MOUSEMOVE:
  case WM_RBUTTONDOWN:
    mouse_point.x = LOWORD(lparam);
    mouse_point.y = HIWORD(lparam);
    dialog = (shell_duel_result_dialog_params_t *)GetWindowLongA(hwnd, DWL_USER);
    if ((msg == WM_MOUSEMOVE && g_duel_interface_options.layout != 2) ||
        (msg == WM_RBUTTONDOWN && g_duel_interface_options.layout == 2))
    {
      GetWindowRect(GetDlgItem(hwnd, 0x6cf), &player_card_rect);
      MapWindowPoints((HWND)0, hwnd, (LPPOINT)&player_card_rect, 2);
      GetWindowRect(GetDlgItem(hwnd, 0x6cd), &opponent_card_rect);
      MapWindowPoints((HWND)0, hwnd, (LPPOINT)&opponent_card_rect, 2);
      if (dialog->player_top_card != -1 &&
          PtInRect(&player_card_rect, mouse_point) != 0)
        SendMessageA(g_duel_card_preview_window_hwnd, 0x401,
                     dialog->player_top_card, 0);
      else if (dialog->opponent_top_card != -1 &&
               PtInRect(&opponent_card_rect, mouse_point) != 0)
        SendMessageA(g_duel_card_preview_window_hwnd, 0x401,
                     dialog->opponent_top_card, 0);
    }
    return FALSE;

  case WM_LBUTTONDOWN:
    SendMessageA(hwnd, WM_SYSCOMMAND, 0xf012, 0);
    return TRUE;
  }
  return FALSE;
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
