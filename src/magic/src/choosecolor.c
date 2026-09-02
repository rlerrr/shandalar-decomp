#include <windows.h>
#include <stdio.h>

#include "defs.h"
#include "cardartlib/src/palette.h"
#include "magic/src/game_support.h"
#include "magic/src/global_other.h"
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

#define MAGICAL_HACK_CARD_DISPLAY 0x492
#define MAGICAL_HACK_PROMPT_LINE_1 0x493
#define MAGICAL_HACK_PROMPT_LINE_2 0x494
#define MAGICAL_HACK_FIRST_WHITE 0x495
#define MAGICAL_HACK_FIRST_BLUE 0x496
#define MAGICAL_HACK_FIRST_BLACK 0x497
#define MAGICAL_HACK_FIRST_RED 0x498
#define MAGICAL_HACK_FIRST_GREEN 0x499
#define MAGICAL_HACK_SECOND_WHITE 0x49a
#define MAGICAL_HACK_SECOND_BLUE 0x49b
#define MAGICAL_HACK_SECOND_BLACK 0x49c
#define MAGICAL_HACK_SECOND_RED 0x49d
#define MAGICAL_HACK_SECOND_GREEN 0x49e

typedef struct choose_color_dialog_context_struct
{
  const char *prompt;
  int initial_color;
  unsigned int has_initial_color;
  int use_color_names_instead_of_land;
  unsigned int available_colors;
} choose_color_dialog_context_t;

typedef struct magical_hack_dialog_context_struct
{
  const char *prompt;
  int initial_color_mask;
  int has_initial_color;
  int target_player;
  int target_card;
  int dialog_result;
} magical_hack_dialog_context_t;

extern HINSTANCE g_app_instance;
extern card_ptr_t global_raw_cards_storage[2000];

HBITMAP load_pic(char *filename);
void delete_and_close_object(HANDLE obj);
void change_buttonclass_wndproc(HWND hwnd);
void draw_owner_draw_button_centered(DRAWITEMSTRUCT *draw_item, HBRUSH brush, HPEN pen1, HPEN pen2, COLORREF color, int draw_focus);
int handle_button_palette_message(int hwnd, unsigned int msg, int wparam, int lparam);
int draw_masked_bitmap_left_half_to_rect(HDC dc, RECT *rect, HANDLE bitmap);
card_id_t get_displayed_card_id(int player, int card);
unsigned int draw_displayed_full_card(HDC dc, RECT *rect, card_ptr_t *raw_card, int player, int card,
                                      int draw_mode, int expand_text_box);

BOOL WINAPI dlgproc_choose_color(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
BOOL WINAPI dlgproc_magical_hack(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
void setup_magical_hack_dialog_resources(HBITMAP *background, COLORREF *label_text_color,
                                         HBRUSH *button_brush, HPEN *button_pen1, HPEN *button_pen2,
                                         COLORREF *button_unfocus_text_color,
                                         COLORREF *button_focus_text_color);
void cleanup_magical_hack_dialog_resources(HBITMAP background, HBRUSH button_brush,
                                           HPEN button_pen1, HPEN button_pen2);
void setup_choose_color_dialog_resources(HBITMAP *background, COLORREF *prompt_text_color, HBITMAP *selection,
                                         HBITMAP color_bitmaps[6], COLORREF *label_text_color,
                                         HBRUSH *button_brush, HPEN *button_pen1, HPEN *button_pen2,
                                         COLORREF *button_unfocus_text_color, COLORREF *button_focus_text_color);
void cleanup_choose_color_dialog_resources(HBITMAP background, HBITMAP selection, HBITMAP color_bitmaps[6],
                                           HBRUSH button_brush, HPEN button_pen1, HPEN button_pen2);
void get_choose_color_selection_rect(RECT *rect, HWND hwnd, int color);

// GLOBAL: MAGIC 0x00638b5c
// GLOBAL: SHANDALAR 0x006502a4
COLORREF g_choose_color_button_focus_text_color;

// GLOBAL: MAGIC 0x00638b88
// GLOBAL: SHANDALAR 0x006502d0
HBITMAP g_choose_color_mana_symbol_bitmaps[6];

// GLOBAL: MAGIC 0x00638bcc
// GLOBAL: SHANDALAR 0x00650314
COLORREF g_choose_color_prompt_text_color;

// GLOBAL: MAGIC 0x00638be4
// GLOBAL: SHANDALAR 0x0065032c
HBITMAP g_choose_color_selection_bitmap;

// GLOBAL: MAGIC 0x00638be8
// GLOBAL: SHANDALAR 0x00650330
HPEN g_choose_color_button_pen2;

// GLOBAL: MAGIC 0x00638c20
// GLOBAL: SHANDALAR 0x00650368
HPEN g_choose_color_button_pen1;

// GLOBAL: MAGIC 0x00638c50
// GLOBAL: SHANDALAR 0x00650398
HBITMAP g_choose_color_background_bitmap;

// GLOBAL: MAGIC 0x00638c54
// GLOBAL: SHANDALAR 0x0065039c
HBRUSH g_choose_color_button_brush;

// GLOBAL: MAGIC 0x00638c90
// GLOBAL: SHANDALAR 0x006503d8
COLORREF g_choose_color_label_text_color;

// GLOBAL: MAGIC 0x00638c98
// GLOBAL: SHANDALAR 0x006503e0
int g_choose_color_selected_color;

// GLOBAL: MAGIC 0x00638ca0
// GLOBAL: SHANDALAR 0x006503e8
COLORREF g_choose_color_button_unfocus_text_color;

// GLOBAL: MAGIC 0x00638bb8
// GLOBAL: SHANDALAR 0x00650300
int g_magical_hack_displayed_player;

// GLOBAL: MAGIC 0x00638bbc
// GLOBAL: SHANDALAR 0x00650304
int g_magical_hack_displayed_card;

// GLOBAL: MAGIC 0x00638bc0
// GLOBAL: SHANDALAR 0x00650308
HBITMAP g_magical_hack_background_bitmap;

// GLOBAL: MAGIC 0x00638bc4
// GLOBAL: SHANDALAR 0x0065030c
HPEN g_magical_hack_button_pen1;

// GLOBAL: MAGIC 0x00638bc8
// GLOBAL: SHANDALAR 0x00650310
int g_magical_hack_displayed_card_id;

// GLOBAL: MAGIC 0x00638bd4
// GLOBAL: SHANDALAR 0x0065031c
COLORREF g_magical_hack_button_unfocus_text_color;

// GLOBAL: MAGIC 0x00638bfc
// GLOBAL: SHANDALAR 0x00650344
HPEN g_magical_hack_button_pen2;

// GLOBAL: MAGIC 0x00638c1c
// GLOBAL: SHANDALAR 0x00650364
HBRUSH g_magical_hack_button_brush;

// GLOBAL: MAGIC 0x00638c70
// GLOBAL: SHANDALAR 0x006503b8
COLORREF g_magical_hack_label_text_color;

// GLOBAL: MAGIC 0x00638c9c
// GLOBAL: SHANDALAR 0x006503e4
COLORREF g_magical_hack_button_focus_text_color;

// FUNCTION: MAGIC 0x004a116d
// FUNCTION: SHANDALAR 0x0053cb05
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
// FUNCTION: SHANDALAR 0x0053cdc4
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
      SetDlgItemTextA(hwnd, CHOOSE_COLOR_LABEL_COLORLESS, g_text_lines[0]);
    }
    else
    {
      SetDlgItemTextA(hwnd, CHOOSE_COLOR_LABEL_BLACK, gs_land_word_capitalized_00709290[COLOR_BLACK]);
      SetDlgItemTextA(hwnd, CHOOSE_COLOR_LABEL_BLUE, gs_land_word_capitalized_00709290[COLOR_BLUE]);
      SetDlgItemTextA(hwnd, CHOOSE_COLOR_LABEL_GREEN, gs_land_word_capitalized_00709290[COLOR_GREEN]);
      SetDlgItemTextA(hwnd, CHOOSE_COLOR_LABEL_RED, gs_land_word_capitalized_00709290[COLOR_RED]);
      SetDlgItemTextA(hwnd, CHOOSE_COLOR_LABEL_WHITE, gs_land_word_capitalized_00709290[COLOR_WHITE]);
      SetDlgItemTextA(hwnd, CHOOSE_COLOR_LABEL_COLORLESS, g_text_lines[0]);
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
// FUNCTION: SHANDALAR 0x0053dbf2
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
// FUNCTION: SHANDALAR 0x0053de1f
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
// FUNCTION: SHANDALAR 0x0053ded5
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

// FUNCTION: MAGIC 0x004a2712
// FUNCTION: SHANDALAR 0x0053e098
int choose_magical_hack_colors(int player, target_t *target, const char *prompt, int initial_color_mask,
                               int has_initial_color)
{
  magical_hack_dialog_context_t context;

  if (player == g_other_player && (g_duel_network_flags & 2) == 0)
  {
    return ((unsigned short)has_initial_color != 0 || initial_color_mask != 0) ? 1 : 0;
  }

  context.prompt = prompt;
  context.initial_color_mask = initial_color_mask;
  context.has_initial_color = has_initial_color;
  context.target_player = target->player;
  context.target_card = target->card;

  if ((g_duel_network_flags & 2) != 0 && player == g_other_player)
  {
    TENTATIVE_wait_for_network_result(player, 0x17);
    context.dialog_result = g_network_result_packet.result;
  }
  else
  {
    context.dialog_result = DialogBoxParamA(g_app_instance, (LPCSTR)0xea, g_duel_window_hwnd,
                                            dlgproc_magical_hack, (LPARAM)&context);
    if ((g_duel_network_flags & 2) != 0)
    {
      g_network_result_packet.result = context.dialog_result;
      g_network_result_packet.packet_type = 0x17;
      TENTATIVE_send_network_result(player, 0x17);
    }
  }

  if (context.dialog_result == -1)
  {
    return -1;
  }
  else
  {
    if (context.dialog_result == -2)
    {
      return -1;
    }
    else
    {
      return context.dialog_result;
    }
  }
}

// FUNCTION: MAGIC 0x004a2855
// FUNCTION: SHANDALAR 0x0053e1d9
BOOL WINAPI dlgproc_magical_hack(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  struct
  {
    POINT preview_point;
    RECT preview_rect;
    HDC paint_dc;
    int saved_dc;
    RECT paint_rect;
    COLORREF button_text_color;
    DRAWITEMSTRUCT *draw_item;
    HWND ctl_hwnd;
    int ctl_id;
    HDC ctl_hdc;
    HBRUSH ctl_brush;
    HWND focus_lost;
    HWND focus_gained;
    unsigned int command_id;
    LONG selection;
    magical_hack_dialog_context_t *context;
  } s;

  switch (msg)
  {
  case WM_INITDIALOG:
    s.context = (magical_hack_dialog_context_t *)lparam;
    setup_magical_hack_dialog_resources(&g_magical_hack_background_bitmap,
                                        &g_magical_hack_label_text_color,
                                        &g_magical_hack_button_brush,
                                        &g_magical_hack_button_pen1,
                                        &g_magical_hack_button_pen2,
                                        &g_magical_hack_button_unfocus_text_color,
                                        &g_magical_hack_button_focus_text_color);
    SetDlgItemTextA(hwnd, IDOK, gs_ok_00924800);
    SetDlgItemTextA(hwnd, IDCANCEL, gs_cancel_008a8c20);
    load_text(global_ui_strings_filename, "DIALOG_CHANGETEXT");
    SetDlgItemTextA(hwnd, MAGICAL_HACK_PROMPT_LINE_1, g_text_lines[0]);
    SetDlgItemTextA(hwnd, MAGICAL_HACK_PROMPT_LINE_2, g_text_lines[1]);
    SetDlgItemTextA(hwnd, IDOK, gs_ok_00924800);
    SetDlgItemTextA(hwnd, IDCANCEL, gs_cancel_008a8c20);
    SetWindowTextA(hwnd, s.context->prompt);
    g_magical_hack_displayed_player = s.context->target_player;
    g_magical_hack_displayed_card = s.context->target_card;
    g_magical_hack_displayed_card_id =
        get_displayed_card_id(g_magical_hack_displayed_player, g_magical_hack_displayed_card);
    ShowWindow(GetDlgItem(hwnd, MAGICAL_HACK_CARD_DISPLAY), SW_HIDE);
    if (s.context->has_initial_color != 0)
    {
      SetDlgItemTextA(hwnd, MAGICAL_HACK_FIRST_BLACK, gs_land_word_capitalized_00709290[COLOR_BLACK]);
      SetDlgItemTextA(hwnd, MAGICAL_HACK_FIRST_BLUE, gs_land_word_capitalized_00709290[COLOR_BLUE]);
      SetDlgItemTextA(hwnd, MAGICAL_HACK_FIRST_GREEN, gs_land_word_capitalized_00709290[COLOR_GREEN]);
      SetDlgItemTextA(hwnd, MAGICAL_HACK_FIRST_RED, gs_land_word_capitalized_00709290[COLOR_RED]);
      SetDlgItemTextA(hwnd, MAGICAL_HACK_FIRST_WHITE, gs_land_word_capitalized_00709290[COLOR_WHITE]);
      SetDlgItemTextA(hwnd, MAGICAL_HACK_SECOND_BLACK, gs_land_word_capitalized_00709290[COLOR_BLACK]);
      SetDlgItemTextA(hwnd, MAGICAL_HACK_SECOND_BLUE, gs_land_word_capitalized_00709290[COLOR_BLUE]);
      SetDlgItemTextA(hwnd, MAGICAL_HACK_SECOND_GREEN, gs_land_word_capitalized_00709290[COLOR_GREEN]);
      SetDlgItemTextA(hwnd, MAGICAL_HACK_SECOND_RED, gs_land_word_capitalized_00709290[COLOR_RED]);
      SetDlgItemTextA(hwnd, MAGICAL_HACK_SECOND_WHITE, gs_land_word_capitalized_00709290[COLOR_WHITE]);
    }
    else
    {
      SetDlgItemTextA(hwnd, MAGICAL_HACK_FIRST_BLACK, gs_color_word_capitalized_00709390[COLOR_BLACK]);
      SetDlgItemTextA(hwnd, MAGICAL_HACK_FIRST_BLUE, gs_color_word_capitalized_00709390[COLOR_BLUE]);
      SetDlgItemTextA(hwnd, MAGICAL_HACK_FIRST_GREEN, gs_color_word_capitalized_00709390[COLOR_GREEN]);
      SetDlgItemTextA(hwnd, MAGICAL_HACK_FIRST_RED, gs_color_word_capitalized_00709390[COLOR_RED]);
      SetDlgItemTextA(hwnd, MAGICAL_HACK_FIRST_WHITE, gs_color_word_capitalized_00709390[COLOR_WHITE]);
      SetDlgItemTextA(hwnd, MAGICAL_HACK_SECOND_BLACK, gs_color_word_capitalized_00709390[COLOR_BLACK]);
      SetDlgItemTextA(hwnd, MAGICAL_HACK_SECOND_BLUE, gs_color_word_capitalized_00709390[COLOR_BLUE]);
      SetDlgItemTextA(hwnd, MAGICAL_HACK_SECOND_GREEN, gs_color_word_capitalized_00709390[COLOR_GREEN]);
      SetDlgItemTextA(hwnd, MAGICAL_HACK_SECOND_RED, gs_color_word_capitalized_00709390[COLOR_RED]);
      SetDlgItemTextA(hwnd, MAGICAL_HACK_SECOND_WHITE, gs_color_word_capitalized_00709390[COLOR_WHITE]);
    }
    CheckDlgButton(hwnd, MAGICAL_HACK_FIRST_WHITE, BST_CHECKED);
    CheckRadioButton(hwnd, MAGICAL_HACK_FIRST_WHITE, MAGICAL_HACK_FIRST_GREEN, MAGICAL_HACK_FIRST_WHITE);
    CheckDlgButton(hwnd, MAGICAL_HACK_SECOND_GREEN, BST_CHECKED);
    CheckRadioButton(hwnd, MAGICAL_HACK_SECOND_WHITE, MAGICAL_HACK_SECOND_GREEN, MAGICAL_HACK_SECOND_GREEN);
    SetWindowLongA(hwnd, DWL_USER, ((unsigned int)(unsigned short)s.context->has_initial_color << 16) | 0x820);
    SetFocus(GetDlgItem(hwnd, IDOK));
    SendMessageA(hwnd, 0x401, IDOK, 0);
    change_buttonclass_wndproc(hwnd);
    return 1;

  case WM_COMMAND:
    s.command_id = LOWORD(wparam);
    s.selection = GetWindowLongA(hwnd, DWL_USER);
    if (s.command_id == IDOK)
    {
      cleanup_magical_hack_dialog_resources(g_magical_hack_background_bitmap,
                                            g_magical_hack_button_brush,
                                            g_magical_hack_button_pen1,
                                            g_magical_hack_button_pen2);
      EndDialog(hwnd, s.selection);
    }
    else if (s.command_id == IDCANCEL)
    {
      cleanup_magical_hack_dialog_resources(g_magical_hack_background_bitmap,
                                            g_magical_hack_button_brush,
                                            g_magical_hack_button_pen1,
                                            g_magical_hack_button_pen2);
      EndDialog(hwnd, -2);
    }
    else
    {
      if (s.command_id == MAGICAL_HACK_FIRST_BLACK)
        s.selection = (s.selection & 0xffffff02) | 2;
      else if (s.command_id == MAGICAL_HACK_FIRST_WHITE)
        s.selection = (s.selection & 0xffffff20) | 0x20;
      else if (s.command_id == MAGICAL_HACK_FIRST_GREEN)
        s.selection = (s.selection & 0xffffff08) | 8;
      else if (s.command_id == MAGICAL_HACK_FIRST_BLUE)
        s.selection = (s.selection & 0xffffff04) | 4;
      else if (s.command_id == MAGICAL_HACK_FIRST_RED)
        s.selection = (s.selection & 0xffffff10) | 0x10;
      else if (s.command_id == MAGICAL_HACK_SECOND_BLACK)
        s.selection = (s.selection & 0xffff02ff) | 0x200;
      else if (s.command_id == MAGICAL_HACK_SECOND_WHITE)
        s.selection = (s.selection & 0xffff20ff) | 0x2000;
      else if (s.command_id == MAGICAL_HACK_SECOND_GREEN)
        s.selection = (s.selection & 0xffff08ff) | 0x800;
      else if (s.command_id == MAGICAL_HACK_SECOND_BLUE)
        s.selection = (s.selection & 0xffff04ff) | 0x400;
      else if (s.command_id == MAGICAL_HACK_SECOND_RED)
        s.selection = (s.selection & 0xffff10ff) | 0x1000;
      SetWindowLongA(hwnd, DWL_USER, s.selection);
      if (((s.selection & 0xff) == 0) || ((s.selection & 0xff00) == 0) ||
          (((s.selection >> 8) & 0xff) == (s.selection & 0xff)))
      {
        EnableWindow(GetDlgItem(hwnd, IDOK), 0);
      }
      else
      {
        EnableWindow(GetDlgItem(hwnd, IDOK), 1);
      }
    }
    return 1;

  case WM_DRAWITEM:
    s.draw_item = (DRAWITEMSTRUCT *)lparam;
    if (GetFocus() == s.draw_item->hwndItem)
      s.button_text_color = g_magical_hack_button_focus_text_color;
    else if ((s.draw_item->itemState & ODS_SELECTED) == 0)
      s.button_text_color = g_magical_hack_button_unfocus_text_color;
    else
      s.button_text_color = 0x10000c6;
    draw_owner_draw_button_centered(s.draw_item,
                                    g_magical_hack_button_brush,
                                    g_magical_hack_button_pen1,
                                    g_magical_hack_button_pen2,
                                    s.button_text_color,
                                    0);
    return 1;

  case WM_ERASEBKGND:
    s.paint_dc = (HDC)wparam;
    ApplyCardArtPaletteToDc(s.paint_dc);
    GetClientRect(hwnd, &s.paint_rect);
    EnterCriticalSection(&g_card_render_lock);
    s.saved_dc = SaveDC(g_shared_offscreen_dc);
    s.paint_dc = g_shared_offscreen_dc;
    if (g_magical_hack_background_bitmap == (HBITMAP)0)
      FillRect(s.paint_dc, &s.paint_rect, (HBRUSH)GetStockObject(2));
    else
      DrawBitmapToRect(g_shared_offscreen_dc, &s.paint_rect, g_magical_hack_background_bitmap);
    if (g_magical_hack_displayed_card_id != -1)
    {
      GetWindowRect(GetDlgItem(hwnd, MAGICAL_HACK_CARD_DISPLAY), &s.paint_rect);
      MapWindowPoints((HWND)0, hwnd, (LPPOINT)&s.paint_rect, 2);
      draw_displayed_full_card(s.paint_dc,
                               &s.paint_rect,
                               global_raw_cards_storage + g_magical_hack_displayed_card_id,
                               g_magical_hack_displayed_player,
                               g_magical_hack_displayed_card,
                               0x11,
                               g_duel_interface_options.expand_text_box_on_big_card);
    }
    RestoreDC(g_shared_offscreen_dc, s.saved_dc);
    s.paint_dc = (HDC)wparam;
    GetClientRect(hwnd, &s.paint_rect);
    BitBlt(s.paint_dc, 0, 0, s.paint_rect.right, s.paint_rect.bottom,
           g_shared_offscreen_dc, 0, 0, SRCCOPY);
    LeaveCriticalSection(&g_card_render_lock);
    return 1;

  case WM_CTLCOLORBTN:
  case WM_CTLCOLORSTATIC:
    s.ctl_hdc = (HDC)wparam;
    ApplyCardArtPaletteToDc(s.ctl_hdc);
    s.ctl_hwnd = (HWND)lparam;
    s.ctl_id = GetDlgCtrlID(s.ctl_hwnd);
    if ((s.ctl_id != MAGICAL_HACK_PROMPT_LINE_1) &&
        (s.ctl_id != MAGICAL_HACK_PROMPT_LINE_2))
    {
      if (GetFocus() == s.ctl_hwnd)
        SetTextColor(s.ctl_hdc, g_magical_hack_button_focus_text_color);
      else
        SetTextColor(s.ctl_hdc, g_magical_hack_label_text_color);
      SetBkMode(s.ctl_hdc, TRANSPARENT);
      s.ctl_brush = (HBRUSH)GetStockObject(NULL_BRUSH);
      return (BOOL)s.ctl_brush;
    }
    SetTextColor(s.ctl_hdc, g_magical_hack_label_text_color);
    SetBkMode(s.ctl_hdc, TRANSPARENT);
    s.ctl_brush = g_magical_hack_button_brush;
    return (BOOL)s.ctl_brush;

  case WM_MOUSEMOVE:
  case WM_RBUTTONDOWN:
    if (((msg == WM_MOUSEMOVE) && (g_duel_interface_options.layout != 2)) ||
        ((msg == WM_RBUTTONDOWN) && (g_duel_interface_options.layout == 2)))
    {
      GetWindowRect(GetDlgItem(hwnd, MAGICAL_HACK_CARD_DISPLAY), &s.preview_rect);
      MapWindowPoints((HWND)0, hwnd, (LPPOINT)&s.preview_rect, 2);
      s.preview_point.x = (short)LOWORD(lparam);
      s.preview_point.y = (short)HIWORD(lparam);
      if ((g_magical_hack_displayed_card_id != -1) &&
          PtInRect(&s.preview_rect, s.preview_point))
      {
        SendMessageA(g_duel_card_preview_window_hwnd, 0x401,
                     g_magical_hack_displayed_card_id, 0);
      }
    }
    return 0;

  case WM_LBUTTONDOWN:
    SendMessageA(hwnd, WM_SYSCOMMAND, 0xf012, 0);
    return 0;

  case WM_QUERYNEWPALETTE:
  case WM_PALETTEISCHANGING:
  case WM_PALETTECHANGED:
    return handle_button_palette_message((int)hwnd, msg, (int)wparam, lparam);

  case WM_CHOOSE_COLOR_BUTTON_FOCUS:
    s.focus_gained = (HWND)wparam;
    s.focus_lost = (HWND)lparam;
    if (GetDlgItem(hwnd, IDCANCEL) == s.focus_gained)
      SendMessageA(hwnd, 0x401, IDCANCEL, 0);
    else
      SendMessageA(hwnd, 0x401, IDOK, 0);
    if (s.focus_gained != (HWND)0)
      InvalidateRect(s.focus_gained, (RECT *)0, 1);
    if (s.focus_lost != (HWND)0)
      InvalidateRect(s.focus_lost, (RECT *)0, 1);
    return 0;

  default:
    return 0;
  }
}

// FUNCTION: MAGIC 0x004a333c
// FUNCTION: SHANDALAR 0x0053ecca
void setup_magical_hack_dialog_resources(HBITMAP *background, COLORREF *label_text_color,
                                         HBRUSH *button_brush, HPEN *button_pen1, HPEN *button_pen2,
                                         COLORREF *button_unfocus_text_color,
                                         COLORREF *button_focus_text_color)
{
  struct
  {
    char filename[264];
  } s;

  sprintf(s.filename, "%s\\WINBK_ChangeText.pic", global_duelart_path);
  *background = load_pic(s.filename);
  *label_text_color = 0x1000098;
  *button_brush = CreateSolidBrush(0x1000076);
  *button_pen1 = CreatePen(PS_SOLID, 0, 0x10000b3);
  *button_pen2 = CreatePen(PS_SOLID, 0, 0x100004e);
  *button_unfocus_text_color = 0x1000098;
  *button_focus_text_color = 0x10000bf;
  if (*button_brush == (HBRUSH)0)
    *button_brush = (HBRUSH)GetStockObject(2);
  if (*button_pen1 == (HPEN)0)
    *button_pen1 = (HPEN)GetStockObject(6);
  if (*button_pen2 == (HPEN)0)
    *button_pen2 = (HPEN)GetStockObject(7);
}

// FUNCTION: MAGIC 0x004a3419
// FUNCTION: SHANDALAR 0x0053eda7
void cleanup_magical_hack_dialog_resources(HBITMAP background, HBRUSH button_brush,
                                           HPEN button_pen1, HPEN button_pen2)
{
  if (background != (HBITMAP)0)
    delete_and_close_object(background);
  if (button_brush != (HBRUSH)0)
    DeleteObject(button_brush);
  if (button_pen1 != (HPEN)0)
    DeleteObject(button_pen1);
  if (button_pen2 != (HPEN)0)
    DeleteObject(button_pen2);
}
