#include <windows.h>
#include <stdio.h>

#include "defs.h"
#include "cardartlib/src/palette.h"
#include "magic/src/global_state.h"
#include "magic/src/global_strings.h"
#include "magic/src/shared_startup.h"

#define CHOOSE_COLOR_ICON_WHITE 0x460
#define CHOOSE_COLOR_ICON_GREEN 0x461
#define CHOOSE_COLOR_ICON_BLUE 0x462
#define CHOOSE_COLOR_ICON_RED 0x463
#define CHOOSE_COLOR_ICON_BLACK 0x464
#define CHOOSE_COLOR_ICON_COLORLESS 0x465
#define CHOOSE_COLOR_LABEL_WHITE 0x466
#define CHOOSE_COLOR_LABEL_BLUE 0x467
#define CHOOSE_COLOR_LABEL_BLACK 0x468
#define CHOOSE_COLOR_LABEL_RED 0x469
#define CHOOSE_COLOR_LABEL_GREEN 0x46a
#define CHOOSE_COLOR_LABEL_COLORLESS 0x46b
#define CHOOSE_COLOR_BUTTON_RESET 0x46c
#define CHOOSE_COLOR_LABEL_PROMPT 0x46d
#define WM_CHOOSE_COLOR_BUTTON_FOCUS 0x4c8

typedef struct choose_color_dialog_context_struct
{
  const char *prompt;
  int initial_color;
  unsigned int has_initial_color;
  int use_color_names_instead_of_land;
  unsigned int available_colors;
} choose_color_dialog_context_t;

extern HINSTANCE g_app_instance;

HBITMAP load_pic(char *filename);
void delete_and_close_object(HANDLE obj);
void change_buttonclass_wndproc(HWND hwnd);
void draw_owner_draw_button_centered(DRAWITEMSTRUCT *draw_item, HBRUSH brush, HPEN pen1, HPEN pen2, COLORREF color, int draw_focus);
int handle_button_palette_message(int hwnd, unsigned int msg, int wparam, int lparam);
int draw_masked_bitmap_left_half_to_rect(HDC dc, RECT *rect, HANDLE bitmap);

BOOL WINAPI dlgproc_choose_color(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
void setup_choose_color_dialog_resources(HBITMAP *background, COLORREF *prompt_text_color, HBITMAP *selection,
                                         HBITMAP color_bitmaps[6], COLORREF *label_text_color,
                                         HBRUSH *button_brush, HPEN *button_pen1, HPEN *button_pen2,
                                         COLORREF *button_unfocus_text_color, COLORREF *button_focus_text_color);
void cleanup_choose_color_dialog_resources(HBITMAP background, HBITMAP selection, HBITMAP color_bitmaps[6],
                                           HBRUSH button_brush, HPEN button_pen1, HPEN button_pen2);
void get_choose_color_selection_rect(RECT *rect, HWND hwnd, int color);

// GLOBAL: MAGIC 0x00638b5c
COLORREF g_choose_color_button_focus_text_color;

// GLOBAL: MAGIC 0x00638b88
HBITMAP g_choose_color_mana_symbol_bitmaps[6];

// GLOBAL: MAGIC 0x00638bcc
COLORREF g_choose_color_prompt_text_color;

// GLOBAL: MAGIC 0x00638be4
HBITMAP g_choose_color_selection_bitmap;

// GLOBAL: MAGIC 0x00638be8
HPEN g_choose_color_button_pen2;

// GLOBAL: MAGIC 0x00638c20
HPEN g_choose_color_button_pen1;

// GLOBAL: MAGIC 0x00638c50
HBITMAP g_choose_color_background_bitmap;

// GLOBAL: MAGIC 0x00638c54
HBRUSH g_choose_color_button_brush;

// GLOBAL: MAGIC 0x00638c90
COLORREF g_choose_color_label_text_color;

// GLOBAL: MAGIC 0x00638c98
int g_choose_color_selected_color;

// GLOBAL: MAGIC 0x00638ca0
COLORREF g_choose_color_button_unfocus_text_color;

// FUNCTION: MAGIC 0x004a116d
int choose_a_color_dialog(int player, const char *prompt, int use_color_names_instead_of_land, int ai_choice, unsigned int available_colors)
{
  struct
  {
    int color_count;
    int selected_color;
    choose_color_dialog_context_t dialog_context;
    int dialog_result;
  } s;

  s.color_count = 0;
  if ((available_colors & COLOR_TEST_BLACK) != 0)
  {
    s.color_count++;
  }
  if ((available_colors & COLOR_TEST_WHITE) != 0)
  {
    s.color_count++;
  }
  if ((available_colors & COLOR_TEST_GREEN) != 0)
  {
    s.color_count++;
  }
  if ((available_colors & COLOR_TEST_RED) != 0)
  {
    s.color_count++;
  }
  if ((available_colors & COLOR_TEST_BLUE) != 0)
  {
    s.color_count++;
  }
  if ((available_colors & COLOR_TEST_COLORLESS) != 0)
  {
    s.color_count++;
  }

  if (s.color_count == 0)
  {
    return -1;
  }
  else
  {
    if (s.color_count == 1)
    {
      if ((available_colors & COLOR_TEST_BLACK) != 0)
      {
        s.dialog_result = COLOR_BLACK;
      }
      else if ((available_colors & COLOR_TEST_WHITE) != 0)
      {
        s.dialog_result = COLOR_WHITE;
      }
      else if ((available_colors & COLOR_TEST_GREEN) != 0)
      {
        s.dialog_result = COLOR_GREEN;
      }
      else if ((available_colors & COLOR_TEST_RED) != 0)
      {
        s.dialog_result = COLOR_RED;
      }
      else if ((available_colors & COLOR_TEST_BLUE) != 0)
      {
        s.dialog_result = COLOR_BLUE;
      }
      else if ((available_colors & COLOR_TEST_COLORLESS) != 0)
      {
        s.dialog_result = COLOR_COLORLESS;
      }
      return s.dialog_result;
    }
  }

  if ((ai_choice == COLOR_BLACK && (available_colors & COLOR_TEST_BLACK) == 0) ||
      (ai_choice == COLOR_WHITE && (available_colors & COLOR_TEST_WHITE) == 0) ||
      (ai_choice == COLOR_GREEN && (available_colors & COLOR_TEST_GREEN) == 0) ||
      (ai_choice == COLOR_RED && (available_colors & COLOR_TEST_RED) == 0) ||
      (ai_choice == COLOR_BLUE && (available_colors & COLOR_TEST_BLUE) == 0) ||
      (ai_choice == COLOR_COLORLESS && (available_colors & COLOR_TEST_COLORLESS) == 0) ||
      (ai_choice < COLOR_COLORLESS || COLOR_WHITE < ai_choice))
  {
    if ((available_colors & COLOR_TEST_WHITE) != 0)
    {
      ai_choice = COLOR_WHITE;
    }
    else if ((available_colors & COLOR_TEST_BLUE) != 0)
    {
      ai_choice = COLOR_BLUE;
    }
    else if ((available_colors & COLOR_TEST_BLACK) != 0)
    {
      ai_choice = COLOR_BLACK;
    }
    else if ((available_colors & COLOR_TEST_RED) != 0)
    {
      ai_choice = COLOR_RED;
    }
    else if ((available_colors & COLOR_TEST_GREEN) != 0)
    {
      ai_choice = COLOR_GREEN;
    }
    else if ((available_colors & COLOR_TEST_COLORLESS) != 0)
    {
      ai_choice = COLOR_COLORLESS;
    }
  }

  if (player != 0)
  {
    s.selected_color = ai_choice;
  }
  else
  {
    s.dialog_context.prompt = prompt;
    s.dialog_context.initial_color = ai_choice;
    if (ai_choice == -1)
    {
      s.dialog_context.has_initial_color = 0;
    }
    else
    {
      s.dialog_context.has_initial_color = 1;
    }
    s.dialog_context.use_color_names_instead_of_land = use_color_names_instead_of_land;
    s.dialog_context.available_colors = available_colors;
    s.dialog_result = DialogBoxParamA(g_app_instance, (LPCSTR)0xde, g_duel_window_hwnd,
                                      dlgproc_choose_color, (LPARAM)&s.dialog_context);
    if (s.dialog_result == -1)
    {
      s.selected_color = -1;
    }
    else if (s.dialog_result == -2)
    {
      s.selected_color = -1;
    }
    else
    {
      s.selected_color = s.dialog_result;
    }
  }

  return s.selected_color;
}

// FUNCTION: MAGIC 0x004a142c
BOOL WINAPI dlgproc_choose_color(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  struct
  {
    HDC erase_hdc;
    int saved_dc;
    HBRUSH selection_brush;
    RECT paint_rect;
    COLORREF button_text_color;
    DRAWITEMSTRUCT *draw_item;
    HWND ctl_hwnd;
    int ctl_id;
    HDC ctl_hdc;
    HWND focus_lost;
    HWND focus_gained;
    int command_id;
    RECT selection_rect;
    choose_color_dialog_context_t *context;
  } s;

  switch (msg)
  {
  case WM_INITDIALOG:
    s.context = (choose_color_dialog_context_t *)lparam;
    SetWindowLongA(hwnd, 8, s.context->initial_color);
    setup_choose_color_dialog_resources(&g_choose_color_background_bitmap,
                                        &g_choose_color_prompt_text_color,
                                        &g_choose_color_selection_bitmap,
                                        g_choose_color_mana_symbol_bitmaps,
                                        &g_choose_color_label_text_color,
                                        &g_choose_color_button_brush,
                                        &g_choose_color_button_pen1,
                                        &g_choose_color_button_pen2,
                                        &g_choose_color_button_unfocus_text_color,
                                        &g_choose_color_button_focus_text_color);
    SetDlgItemTextA(hwnd, IDOK, gs_ok_00924800);
    SetDlgItemTextA(hwnd, IDCANCEL, gs_cancel_008a8c20);
    SetDlgItemTextA(hwnd, CHOOSE_COLOR_LABEL_PROMPT, s.context->prompt);
    if (s.context->has_initial_color == 0)
    {
      ShowWindow(GetDlgItem(hwnd, CHOOSE_COLOR_BUTTON_RESET), 0);
    }
    SetFocus(GetDlgItem(hwnd, IDOK));
    SendMessageA(hwnd, 0x401, IDOK, 0);
    g_choose_color_selected_color = s.context->initial_color;
    load_text(global_ui_strings_filename, "COLORLESSMANA");
    if (s.context->use_color_names_instead_of_land != 0)
    {
      SetDlgItemTextA(hwnd, CHOOSE_COLOR_LABEL_BLACK, gs_color_word_capitalized_00709390[COLOR_BLACK]);
      SetDlgItemTextA(hwnd, CHOOSE_COLOR_LABEL_BLUE, gs_color_word_capitalized_00709390[COLOR_BLUE]);
      SetDlgItemTextA(hwnd, CHOOSE_COLOR_LABEL_GREEN, gs_color_word_capitalized_00709390[COLOR_GREEN]);
      SetDlgItemTextA(hwnd, CHOOSE_COLOR_LABEL_RED, gs_color_word_capitalized_00709390[COLOR_RED]);
      SetDlgItemTextA(hwnd, CHOOSE_COLOR_LABEL_WHITE, gs_color_word_capitalized_00709390[COLOR_WHITE]);
      SetDlgItemTextA(hwnd, CHOOSE_COLOR_LABEL_COLORLESS, text_lines[0]);
    }
    else
    {
      SetDlgItemTextA(hwnd, CHOOSE_COLOR_LABEL_BLACK, gs_land_word_capitalized_00709290[COLOR_BLACK]);
      SetDlgItemTextA(hwnd, CHOOSE_COLOR_LABEL_BLUE, gs_land_word_capitalized_00709290[COLOR_BLUE]);
      SetDlgItemTextA(hwnd, CHOOSE_COLOR_LABEL_GREEN, gs_land_word_capitalized_00709290[COLOR_GREEN]);
      SetDlgItemTextA(hwnd, CHOOSE_COLOR_LABEL_RED, gs_land_word_capitalized_00709290[COLOR_RED]);
      SetDlgItemTextA(hwnd, CHOOSE_COLOR_LABEL_WHITE, gs_land_word_capitalized_00709290[COLOR_WHITE]);
      SetDlgItemTextA(hwnd, CHOOSE_COLOR_LABEL_COLORLESS, text_lines[0]);
    }
    if ((s.context->available_colors & COLOR_TEST_BLACK) == 0)
    {
      ShowWindow(GetDlgItem(hwnd, CHOOSE_COLOR_ICON_BLACK), 0);
      ShowWindow(GetDlgItem(hwnd, CHOOSE_COLOR_LABEL_BLACK), 0);
    }
    if ((s.context->available_colors & COLOR_TEST_BLUE) == 0)
    {
      ShowWindow(GetDlgItem(hwnd, CHOOSE_COLOR_ICON_BLUE), 0);
      ShowWindow(GetDlgItem(hwnd, CHOOSE_COLOR_LABEL_BLUE), 0);
    }
    if ((s.context->available_colors & COLOR_TEST_GREEN) == 0)
    {
      ShowWindow(GetDlgItem(hwnd, CHOOSE_COLOR_ICON_GREEN), 0);
      ShowWindow(GetDlgItem(hwnd, CHOOSE_COLOR_LABEL_GREEN), 0);
    }
    if ((s.context->available_colors & COLOR_TEST_RED) == 0)
    {
      ShowWindow(GetDlgItem(hwnd, CHOOSE_COLOR_ICON_RED), 0);
      ShowWindow(GetDlgItem(hwnd, CHOOSE_COLOR_LABEL_RED), 0);
    }
    if ((s.context->available_colors & COLOR_TEST_WHITE) == 0)
    {
      ShowWindow(GetDlgItem(hwnd, CHOOSE_COLOR_ICON_WHITE), 0);
      ShowWindow(GetDlgItem(hwnd, CHOOSE_COLOR_LABEL_WHITE), 0);
    }
    if ((s.context->available_colors & COLOR_TEST_COLORLESS) == 0)
    {
      ShowWindow(GetDlgItem(hwnd, CHOOSE_COLOR_ICON_COLORLESS), 0);
      ShowWindow(GetDlgItem(hwnd, CHOOSE_COLOR_LABEL_COLORLESS), 0);
    }
    change_buttonclass_wndproc(hwnd);
    return 0;

  case WM_COMMAND:
    s.command_id = (int)(wparam & 0xffff);

    switch (s.command_id)
    {
    case 1:
    case 2:
      cleanup_choose_color_dialog_resources(g_choose_color_background_bitmap,
                                            g_choose_color_selection_bitmap,
                                            g_choose_color_mana_symbol_bitmaps,
                                            g_choose_color_button_brush,
                                            g_choose_color_button_pen1,
                                            g_choose_color_button_pen2);
      if (s.command_id == IDOK)
      {
        EndDialog(hwnd, g_choose_color_selected_color);
      }
      else
      {
        EndDialog(hwnd, -2);
      }
      break;

    case CHOOSE_COLOR_BUTTON_RESET:
      get_choose_color_selection_rect(&s.selection_rect, hwnd, g_choose_color_selected_color);
      InvalidateRect(hwnd, &s.selection_rect, 1);
      g_choose_color_selected_color = GetWindowLongA(hwnd, 8);
      get_choose_color_selection_rect(&s.selection_rect, hwnd, g_choose_color_selected_color);
      InvalidateRect(hwnd, &s.selection_rect, 0);
      SetFocus(GetDlgItem(hwnd, IDOK));
      break;
      
    case CHOOSE_COLOR_ICON_BLACK:
      get_choose_color_selection_rect(&s.selection_rect, hwnd, g_choose_color_selected_color);
      InvalidateRect(hwnd, &s.selection_rect, 1);
      g_choose_color_selected_color = COLOR_BLACK;
      get_choose_color_selection_rect(&s.selection_rect, hwnd, g_choose_color_selected_color);
      InvalidateRect(hwnd, &s.selection_rect, 0);
      if (HIWORD(wparam) == 5)
      {
        SendMessageA(hwnd, WM_COMMAND, 0x10000, (LPARAM)GetDlgItem(hwnd, IDOK));
      }
      break;

    case CHOOSE_COLOR_ICON_BLUE:
      get_choose_color_selection_rect(&s.selection_rect, hwnd, g_choose_color_selected_color);
      InvalidateRect(hwnd, &s.selection_rect, 1);
      g_choose_color_selected_color = COLOR_BLUE;
      get_choose_color_selection_rect(&s.selection_rect, hwnd, g_choose_color_selected_color);
      InvalidateRect(hwnd, &s.selection_rect, 0);
      if (HIWORD(wparam) == 5)
      {
        SendMessageA(hwnd, WM_COMMAND, 0x10000, (LPARAM)GetDlgItem(hwnd, IDOK));
      }
      break;

    case CHOOSE_COLOR_ICON_GREEN:
      get_choose_color_selection_rect(&s.selection_rect, hwnd, g_choose_color_selected_color);
      InvalidateRect(hwnd, &s.selection_rect, 1);
      g_choose_color_selected_color = COLOR_GREEN;
      get_choose_color_selection_rect(&s.selection_rect, hwnd, g_choose_color_selected_color);
      InvalidateRect(hwnd, &s.selection_rect, 0);
      if (HIWORD(wparam) == 5)
      {
        SendMessageA(hwnd, WM_COMMAND, 0x10000, (LPARAM)GetDlgItem(hwnd, IDOK));
      }
      break;

    case CHOOSE_COLOR_ICON_RED:
      get_choose_color_selection_rect(&s.selection_rect, hwnd, g_choose_color_selected_color);
      InvalidateRect(hwnd, &s.selection_rect, 1);
      g_choose_color_selected_color = COLOR_RED;
      get_choose_color_selection_rect(&s.selection_rect, hwnd, g_choose_color_selected_color);
      InvalidateRect(hwnd, &s.selection_rect, 0);
      if (HIWORD(wparam) == 5)
      {
        SendMessageA(hwnd, WM_COMMAND, 0x10000, (LPARAM)GetDlgItem(hwnd, IDOK));
      }
      break;

    case CHOOSE_COLOR_ICON_WHITE:
      get_choose_color_selection_rect(&s.selection_rect, hwnd, g_choose_color_selected_color);
      InvalidateRect(hwnd, &s.selection_rect, 1);
      g_choose_color_selected_color = COLOR_WHITE;
      get_choose_color_selection_rect(&s.selection_rect, hwnd, g_choose_color_selected_color);
      InvalidateRect(hwnd, &s.selection_rect, 0);
      if (HIWORD(wparam) == 5)
      {
        SendMessageA(hwnd, WM_COMMAND, 0x10000, (LPARAM)GetDlgItem(hwnd, IDOK));
      }
      break;

    case CHOOSE_COLOR_ICON_COLORLESS:
      get_choose_color_selection_rect(&s.selection_rect, hwnd, g_choose_color_selected_color);
      InvalidateRect(hwnd, &s.selection_rect, 1);
      g_choose_color_selected_color = COLOR_COLORLESS;
      get_choose_color_selection_rect(&s.selection_rect, hwnd, g_choose_color_selected_color);
      InvalidateRect(hwnd, &s.selection_rect, 0);
      if (HIWORD(wparam) == 5)
      {
        SendMessageA(hwnd, WM_COMMAND, 0x10000, (LPARAM)GetDlgItem(hwnd, IDOK));
      }
      break;
    }

    return 1;

  case WM_CHOOSE_COLOR_BUTTON_FOCUS:
    s.focus_gained = (HWND)wparam;
    s.focus_lost = (HWND)lparam;
    if (GetDlgItem(hwnd, IDCANCEL) == s.focus_gained)
    {
      SendMessageA(hwnd, 0x401, IDCANCEL, 0);
    }
    else
    {
      SendMessageA(hwnd, 0x401, IDOK, 0);
    }
    if (s.focus_gained != (HWND)0)
    {
      InvalidateRect(s.focus_gained, (RECT *)0, 1);
    }
    if (s.focus_lost != (HWND)0)
    {
      InvalidateRect(s.focus_lost, (RECT *)0, 1);
    }
    return 0;

  case WM_CTLCOLORSTATIC:
    s.ctl_hdc = (HDC)wparam;
    ApplyCardArtPaletteToDc(s.ctl_hdc);
    s.ctl_hwnd = (HWND)lparam;
    s.ctl_id = GetDlgCtrlID(s.ctl_hwnd);
    SetBkMode(s.ctl_hdc, TRANSPARENT);
    if (s.ctl_id == CHOOSE_COLOR_LABEL_PROMPT)
    {
      SetTextColor(s.ctl_hdc, g_choose_color_prompt_text_color);
    }
    else
    {
      SetTextColor(s.ctl_hdc, g_choose_color_label_text_color);
    }
    return (BOOL)GetStockObject(NULL_BRUSH);

  case WM_DRAWITEM:
    s.draw_item = (DRAWITEMSTRUCT *)lparam;
    if (GetFocus() == s.draw_item->hwndItem)
    {
      s.button_text_color = g_choose_color_button_focus_text_color;
    }
    else
    {
      s.button_text_color = g_choose_color_button_unfocus_text_color;
    }
    draw_owner_draw_button_centered(s.draw_item,
                 g_choose_color_button_brush,
                 g_choose_color_button_pen1,
                 g_choose_color_button_pen2,
                 s.button_text_color,
                 0);
    return 1;

  case WM_QUERYNEWPALETTE:
  case WM_PALETTEISCHANGING:
  case WM_PALETTECHANGED:
    return handle_button_palette_message((int)hwnd, msg, (int)wparam, lparam);

  case WM_ERASEBKGND:
    s.erase_hdc = (HDC)wparam;
    ApplyCardArtPaletteToDc(s.erase_hdc);
    GetClientRect(hwnd, &s.paint_rect);
    EnterCriticalSection(&g_card_render_lock);
    s.saved_dc = SaveDC(g_shared_offscreen_dc);
    s.erase_hdc = g_shared_offscreen_dc;
    if (g_choose_color_background_bitmap != (HBITMAP)0)
    {
      DrawBitmapToRect(s.erase_hdc, &s.paint_rect, g_choose_color_background_bitmap);
    }
    else
    {
      FillRect(s.erase_hdc, &s.paint_rect, GetStockObject(2));
    }
    get_choose_color_selection_rect(&s.paint_rect, hwnd, g_choose_color_selected_color);
    if (g_choose_color_selection_bitmap != (HBITMAP)0)
    {
      DrawBitmapToRect(s.erase_hdc, &s.paint_rect, g_choose_color_selection_bitmap);
    }
    else
    {
      s.selection_brush = CreateSolidBrush(0x2908c52);
      FrameRect(s.erase_hdc, &s.paint_rect, s.selection_brush);
      DeleteObject(s.selection_brush);
    }
    if (IsWindowVisible(GetDlgItem(hwnd, CHOOSE_COLOR_LABEL_WHITE)) != 0)
    {
      GetWindowRect(GetDlgItem(hwnd, CHOOSE_COLOR_ICON_WHITE), &s.paint_rect);
      MapWindowPoints((HWND)0, hwnd, (LPPOINT)&s.paint_rect, 2);
      draw_masked_bitmap_left_half_to_rect(s.erase_hdc, &s.paint_rect, g_choose_color_mana_symbol_bitmaps[COLOR_WHITE]);
    }
    if (IsWindowVisible(GetDlgItem(hwnd, CHOOSE_COLOR_LABEL_BLUE)) != 0)
    {
      GetWindowRect(GetDlgItem(hwnd, CHOOSE_COLOR_ICON_BLUE), &s.paint_rect);
      MapWindowPoints((HWND)0, hwnd, (LPPOINT)&s.paint_rect, 2);
      draw_masked_bitmap_left_half_to_rect(s.erase_hdc, &s.paint_rect, g_choose_color_mana_symbol_bitmaps[COLOR_BLUE]);
    }
    if (IsWindowVisible(GetDlgItem(hwnd, CHOOSE_COLOR_LABEL_BLACK)) != 0)
    {
      GetWindowRect(GetDlgItem(hwnd, CHOOSE_COLOR_ICON_BLACK), &s.paint_rect);
      MapWindowPoints((HWND)0, hwnd, (LPPOINT)&s.paint_rect, 2);
      draw_masked_bitmap_left_half_to_rect(s.erase_hdc, &s.paint_rect, g_choose_color_mana_symbol_bitmaps[COLOR_BLACK]);
    }
    if (IsWindowVisible(GetDlgItem(hwnd, CHOOSE_COLOR_LABEL_RED)) != 0)
    {
      GetWindowRect(GetDlgItem(hwnd, CHOOSE_COLOR_ICON_RED), &s.paint_rect);
      MapWindowPoints((HWND)0, hwnd, (LPPOINT)&s.paint_rect, 2);
      draw_masked_bitmap_left_half_to_rect(s.erase_hdc, &s.paint_rect, g_choose_color_mana_symbol_bitmaps[COLOR_RED]);
    }
    if (IsWindowVisible(GetDlgItem(hwnd, CHOOSE_COLOR_LABEL_GREEN)) != 0)
    {
      GetWindowRect(GetDlgItem(hwnd, CHOOSE_COLOR_ICON_GREEN), &s.paint_rect);
      MapWindowPoints((HWND)0, hwnd, (LPPOINT)&s.paint_rect, 2);
      draw_masked_bitmap_left_half_to_rect(s.erase_hdc, &s.paint_rect, g_choose_color_mana_symbol_bitmaps[COLOR_GREEN]);
    }
    if (IsWindowVisible(GetDlgItem(hwnd, CHOOSE_COLOR_LABEL_COLORLESS)) != 0)
    {
      GetWindowRect(GetDlgItem(hwnd, CHOOSE_COLOR_ICON_COLORLESS), &s.paint_rect);
      MapWindowPoints((HWND)0, hwnd, (LPPOINT)&s.paint_rect, 2);
      draw_masked_bitmap_left_half_to_rect(s.erase_hdc, &s.paint_rect, g_choose_color_mana_symbol_bitmaps[COLOR_COLORLESS]);
    }
    RestoreDC(g_shared_offscreen_dc, s.saved_dc);
    s.erase_hdc = (HDC)wparam;
    GetClientRect(hwnd, &s.paint_rect);
    BitBlt(s.erase_hdc, 0, 0, s.paint_rect.right, s.paint_rect.bottom, g_shared_offscreen_dc, 0, 0, SRCCOPY);
    LeaveCriticalSection(&g_card_render_lock);
    return 1;

  case WM_LBUTTONDOWN:
    SendMessageA(hwnd, WM_SYSCOMMAND, 0xf012, 0);
    return 0;

  default:
    return 0;
  }
}

// FUNCTION: MAGIC 0x004a226c
void setup_choose_color_dialog_resources(HBITMAP *background, COLORREF *prompt_text_color, HBITMAP *selection,
                                         HBITMAP color_bitmaps[6], COLORREF *label_text_color,
                                         HBRUSH *button_brush, HPEN *button_pen1, HPEN *button_pen2,
                                         COLORREF *button_unfocus_text_color, COLORREF *button_focus_text_color)
{
  struct
  {
    char filename[264];
  } s;

  sprintf(s.filename, "%s\\WINBK_QuestMana.pic", global_duelart_path);
  *background = load_pic(s.filename);
  *prompt_text_color = 0x1000031;
  sprintf(s.filename, "%s\\WINBK_QuestManaSelection.pic", global_duelart_path);
  *selection = load_pic(s.filename);
  sprintf(s.filename, "%s\\QUESTMANA_Black.pic", global_duelart_path);
  color_bitmaps[COLOR_BLACK] = load_pic(s.filename);
  sprintf(s.filename, "%s\\QUESTMANA_White.pic", global_duelart_path);
  color_bitmaps[COLOR_WHITE] = load_pic(s.filename);
  sprintf(s.filename, "%s\\QUESTMANA_Green.pic", global_duelart_path);
  color_bitmaps[COLOR_GREEN] = load_pic(s.filename);
  sprintf(s.filename, "%s\\QUESTMANA_Blue.pic", global_duelart_path);
  color_bitmaps[COLOR_BLUE] = load_pic(s.filename);
  sprintf(s.filename, "%s\\QUESTMANA_Red.pic", global_duelart_path);
  color_bitmaps[COLOR_RED] = load_pic(s.filename);
  sprintf(s.filename, "%s\\QUESTMANA_Gray.pic", global_duelart_path);
  color_bitmaps[COLOR_COLORLESS] = load_pic(s.filename);
  *label_text_color = 0x1000031;
  *button_brush = CreateSolidBrush(0x10000c6);
  *button_pen1 = CreatePen(PS_SOLID, 0, 0x10000c2);
  *button_pen2 = CreatePen(PS_SOLID, 0, 0x10000c8);
  *button_unfocus_text_color = 0x1000031;
  *button_focus_text_color = 0x10000bf;
  if (*button_brush == (HBRUSH)0)
  {
    *button_brush = (HBRUSH)GetStockObject(2);
  }
  if (*button_pen1 == (HPEN)0)
  {
    *button_pen1 = (HPEN)GetStockObject(6);
  }
  if (*button_pen2 == (HPEN)0)
  {
    *button_pen2 = (HPEN)GetStockObject(7);
  }
}

// FUNCTION: MAGIC 0x004a2499
void cleanup_choose_color_dialog_resources(HBITMAP background, HBITMAP selection, HBITMAP color_bitmaps[6],
                                           HBRUSH button_brush, HPEN button_pen1, HPEN button_pen2)
{
  int color_index;

  if (background != (HBITMAP)0)
  {
    delete_and_close_object(background);
  }
  if (selection != (HBITMAP)0)
  {
    delete_and_close_object(selection);
  }
  for (color_index = 0; color_index < 6; color_index++)
  {
    if (color_bitmaps[color_index] != (HBITMAP)0)
    {
      delete_and_close_object(color_bitmaps[color_index]);
    }
  }
  if (button_brush != (HBRUSH)0)
  {
    DeleteObject(button_brush);
  }
  if (button_pen1 != (HPEN)0)
  {
    DeleteObject(button_pen1);
  }
  if (button_pen2 != (HPEN)0)
  {
    DeleteObject(button_pen2);
  }
}

// FUNCTION: MAGIC 0x004a254f
void get_choose_color_selection_rect(RECT *rect, HWND hwnd, int color)
{
  struct
  {
    RECT icon_rect;
    RECT label_rect;
  } s;

  if (rect == (RECT *)0)
  {
  }
  else
  {
    if (color == COLOR_WHITE)
    {
      GetWindowRect(GetDlgItem(hwnd, CHOOSE_COLOR_ICON_WHITE), &s.icon_rect);
      GetWindowRect(GetDlgItem(hwnd, CHOOSE_COLOR_LABEL_WHITE), &s.label_rect);
    }
    if (color == COLOR_BLUE)
    {
      GetWindowRect(GetDlgItem(hwnd, CHOOSE_COLOR_ICON_BLUE), &s.icon_rect);
      GetWindowRect(GetDlgItem(hwnd, CHOOSE_COLOR_LABEL_BLUE), &s.label_rect);
    }
    if (color == COLOR_BLACK)
    {
      GetWindowRect(GetDlgItem(hwnd, CHOOSE_COLOR_ICON_BLACK), &s.icon_rect);
      GetWindowRect(GetDlgItem(hwnd, CHOOSE_COLOR_LABEL_BLACK), &s.label_rect);
    }
    if (color == COLOR_RED)
    {
      GetWindowRect(GetDlgItem(hwnd, CHOOSE_COLOR_ICON_RED), &s.icon_rect);
      GetWindowRect(GetDlgItem(hwnd, CHOOSE_COLOR_LABEL_RED), &s.label_rect);
    }
    if (color == COLOR_GREEN)
    {
      GetWindowRect(GetDlgItem(hwnd, CHOOSE_COLOR_ICON_GREEN), &s.icon_rect);
      GetWindowRect(GetDlgItem(hwnd, CHOOSE_COLOR_LABEL_GREEN), &s.label_rect);
    }
    if (color == COLOR_COLORLESS)
    {
      GetWindowRect(GetDlgItem(hwnd, CHOOSE_COLOR_ICON_COLORLESS), &s.icon_rect);
      GetWindowRect(GetDlgItem(hwnd, CHOOSE_COLOR_LABEL_COLORLESS), &s.label_rect);
    }
    UnionRect(rect, &s.icon_rect, &s.label_rect);
    InflateRect(rect, 0, 10);
    MapWindowPoints((HWND)0, hwnd, (LPPOINT)rect, 2);
  }
}
