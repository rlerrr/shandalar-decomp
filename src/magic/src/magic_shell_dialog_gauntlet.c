#include "magic_shell_dialogs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <direct.h>
#include <commdlg.h>
#include "cardartlib/src/palette.h"
#include "deckdll/src/shared_resources.h"
#include "game_support.h"
#include "global_duel_ui_ids.h"
#include "global_other.h"
#include "magic_shell.h"
#include "shared_startup.h"

extern HWND global_main_hwnd;
extern HINSTANCE g_app_instance;
extern char global_base_directory[];
extern void *g_duel_player_face_pic;
void __stdcall LoadGauntletRegistryOptions(void);
void save_gauntlet_options_to_registry(void);
int IsCardAvailable(int card_id, int expansion);
int check_ante(int card_id);

// GLOBAL: MAGIC 0x00637b00
static HFONT shell_gauntlet_dialog_font;
// GLOBAL: MAGIC 0x00637aa0
static HFONT shell_gauntlet_dialog_low_resolution_font;

// GLOBAL: MAGIC 0x0093b170
char shell_gauntlet_player_deck_path[264];

// FUNCTION: MAGIC 0x00489556
void shell_open_duel_interface_options(HWND hwnd)
{
  if (DialogBoxParamA(g_app_instance, (LPCSTR)0xe1, hwnd,
                      dlgproc_duel_interface_options, 0) != 0)
    save_duel_interface_options_to_registry();
}

// FUNCTION: MAGIC 0x004e07f0
static int shell_deck_is_available(const char *deck_path, int allow_ante)
{
  struct
  {
    int lines_seen;
    int ignored_lines;
    int card_id;
    int quantity;
    int total_cards;
    int result;
    char line[500];
    FILE *deck_file;
    int scan_result;
  } s;

  s.result = 1;
  s.lines_seen = s.ignored_lines = 0;
  s.deck_file = fopen(deck_path, "rt");
  if (s.deck_file == NULL)
    return 0;

  s.scan_result = fscanf(s.deck_file, "%[^\n]", s.line);
  s.scan_result = fscanf(s.deck_file, "%[\n]", s.line);
  s.total_cards = 0;
  s.quantity = -1;
  s.card_id = s.quantity;
  do
  {
    s.scan_result = fscanf(s.deck_file, "%[^\n]", s.line);
    if (s.line[0] == '.')
    {
      if (s.line[1] != 'v')
      {
        sscanf(s.line + 1, "%d %d", &s.card_id, &s.quantity);
        if (IsCardAvailable(s.card_id, 0) == 0 &&
            IsCardAvailable(s.card_id, 1) == 0 &&
            IsCardAvailable(s.card_id, 2) == 0)
          s.result = 0;
        if (allow_ante == 0 && check_ante(s.card_id) != 0)
          s.result = 0;
        s.total_cards += s.quantity;
      }
      ++s.lines_seen;
    }
    else if (++s.ignored_lines == 5)
    {
    }
    else if (s.ignored_lines == 6)
    {
    }
    s.scan_result = fscanf(s.deck_file, "%[\n]", s.line);
  } while (s.lines_seen < 200 && s.scan_result != -1 &&
           (s.card_id != 0 || s.quantity != 0));
  fclose(s.deck_file);
  if (s.total_cards < 40)
    s.result = 0;

  return s.result;
}

// FUNCTION: MAGIC 0x004dff64
static LRESULT shell_populate_deck_choices(HWND hwnd, int first_control,
                                           int second_control, int allow_ante)
{
  struct
  {
    char deck_path[264];
    char deck_filename[264];
    HWND listbox;
    char previous_directory[264];
    char first_line[100];
    WPARAM index;
    FILE *deck_file;
    char display_name[100];
    char *cursor;
  } s;
  LRESULT deck_count;

  s.listbox = CreateWindowExA(0, "LISTBOX", "", 0x40a00003,
                              0, 0, 0, 0, hwnd, NULL,
                              g_app_instance, NULL);
  if (s.listbox == NULL)
    return -1;
  GetCurrentDirectoryA(0x104, s.previous_directory);
  _chdir(global_base_directory);
  strcpy(s.deck_filename, "playdeck\\*.DCK");
  SendMessageA(s.listbox, LB_DIR, 0, (LPARAM)s.deck_filename);
  deck_count = SendMessageA(s.listbox, LB_GETCOUNT, 0, 0);
  for (s.index = 0; (int)s.index < deck_count; ++s.index)
  {
    SendMessageA(s.listbox, LB_GETTEXT, s.index,
                 (LPARAM)s.deck_filename);
    strcpy(s.deck_path, "playdeck\\");
    strcat(s.deck_path, s.deck_filename);
    if (shell_deck_is_available(s.deck_path, allow_ante))
    {
      s.deck_file = fopen(s.deck_path, "r");
      if (s.deck_file != NULL)
      {
        s.first_line[0] = '\0';
        s.cursor = s.first_line;
        while (1)
        {
          *s.cursor = (char)fgetc(s.deck_file);
          if (*s.cursor == '\n')
            break;
          if (*s.cursor != ';')
            ++s.cursor;
        }
        *s.cursor = '\0';
        strcpy(s.display_name, s.first_line);
        strcat(s.display_name, ".    (");
        s.cursor = s.display_name + strlen(s.display_name);
        while (1)
        {
          *s.cursor = (char)fgetc(s.deck_file);
          if (*s.cursor == '\n')
            break;
          if (*s.cursor != ';')
            ++s.cursor;
        }
        *s.cursor = '\0';
        strcat(s.display_name, ")");
        fclose(s.deck_file);
        if (first_control != -1)
          SendDlgItemMessageA(hwnd, first_control, CB_ADDSTRING, 0,
                              (LPARAM)s.display_name);
        if (second_control != -1)
          SendDlgItemMessageA(hwnd, second_control, CB_ADDSTRING, 0,
                              (LPARAM)s.display_name);
      }
    }
  }
  _chdir(s.previous_directory);
  DestroyWindow(s.listbox);
  return deck_count;
}
#include "global_state.h"
#include "global_strings.h"

// FUNCTION: MAGIC 0x0045fe6e
static int shell_gauntlet_total_games(void)
{
  int total_games;

  total_games = (5 - g_gauntlet_options.best_of + 1) *
                g_gauntlet_options.gauntlet_length;
  return total_games;
}

// FUNCTION: MAGIC 0x0045fe9a
static void shell_format_gauntlet_status(char *output, int line_index)
{
  struct
  {
    int total_games;
    char format[100];
  } s;
  int tier;

  load_text(global_ui_strings_filename, "SHELLPAGE_GAUNTLET");
  s.total_games = shell_gauntlet_total_games();
  strcpy(s.format, g_text_lines[line_index]);
  tier = s.total_games / 50 + 1;
  if (tier > 5)
    tier = 5;
  sprintf(output, s.format, s.total_games, g_text_lines[line_index + tier]);
}

// FUNCTION: MAGIC 0x0045f555
static void shell_layout_gauntlet_controls(HWND hwnd)
{
  struct
  {
    char text[100];
    RECT status_rect;
    HWND control;
    RECT control_rect;
    RECT buttons_rect;
    HDC dc;
    int control_height;
    int width;
    int y;
    int x;
    int control_width;
    HGDIOBJ font;
    RECT content_rect;
    SIZE text_size;
    int button_height;
    int line_height;
    int margin;
    int digit_width;
  } s;

  GetClientRect(hwnd, &s.content_rect);
  s.content_rect.left += 20;
  s.content_rect.right -= 20;
  s.content_rect.top += 20;
  s.content_rect.bottom -= 20;
  s.font = (HGDIOBJ)SendDlgItemMessageA(hwnd, 0x6a4, WM_GETFONT, 0, 0);
  s.dc = GetDC(hwnd);
  SelectObject(s.dc, s.font);
  GetTextExtentPoint32A(s.dc, "fun", 3, &s.text_size);
  s.button_height = s.text_size.cy * 2;
  s.line_height = s.text_size.cy;
  s.margin = s.text_size.cy * 3 / 2;
  s.control = GetDlgItem(hwnd, 0x6a4);
  GetWindowTextA(s.control, s.text, 100);
  GetTextExtentPoint32A(s.dc, s.text, strlen(s.text), &s.text_size);
  SetWindowPos(s.control, NULL, 0, 0, s.text_size.cx + s.margin,
               s.line_height, 4);
  s.x = s.text_size.cx + s.margin / 2;
  s.y = 0;
  s.control = GetDlgItem(hwnd, 0x6a5);
  GetWindowTextA(s.control, s.text, 100);
  GetTextExtentPoint32A(s.dc, s.text, strlen(s.text), &s.text_size);
  SetWindowPos(s.control, NULL, s.x, s.y, s.text_size.cx + s.margin,
               s.line_height, 4);
  s.font = (HGDIOBJ)SendDlgItemMessageA(hwnd, IDOK, WM_GETFONT, 0, 0);
  s.dc = GetDC(hwnd);
  SelectObject(s.dc, s.font);
  GetTextExtentPoint32A(s.dc, "fun", 3, &s.text_size);
  s.button_height = s.text_size.cy * 2;
  s.line_height = s.text_size.cy;
  s.margin = s.text_size.cy * 3 / 2;
  s.digit_width = s.text_size.cx;
  s.control = GetDlgItem(hwnd, IDOK);
  GetWindowTextA(s.control, s.text, 100);
  GetTextExtentPoint32A(s.dc, s.text, strlen(s.text), &s.text_size);
  s.width = s.text_size.cx;
  s.control = GetDlgItem(hwnd, 0x6b3);
  GetWindowTextA(s.control, s.text, 100);
  GetTextExtentPoint32A(s.dc, s.text, strlen(s.text), &s.text_size);
  if (s.width < s.text_size.cx)
    s.width = s.text_size.cx;
  s.width += s.margin;
  s.x = s.content_rect.right - s.width;
  s.y = s.content_rect.bottom - s.button_height;
  s.control = GetDlgItem(hwnd, IDOK);
  SetWindowPos(s.control, NULL, s.x, s.y, s.width, s.button_height, 4);
  s.y -= s.button_height + s.button_height / 3;
  s.control = GetDlgItem(hwnd, 0x6b3);
  SetWindowPos(s.control, NULL, s.x, s.y, s.width, s.button_height, 4);
  s.y -= s.button_height + s.button_height * 3 / 4;
  s.control = GetDlgItem(hwnd, 0x6ad);
  GetWindowTextA(s.control, s.text, 100);
  GetTextExtentPoint32A(s.dc, s.text, strlen(s.text), &s.text_size);
  s.width = s.text_size.cx + s.margin;
  SetWindowPos(s.control, NULL,
               s.x + (s.content_rect.right - s.x) / 2 - s.width / 2,
               s.y, s.width, s.button_height, 4);
  SetRect(&s.buttons_rect,
          s.x < s.x + (s.content_rect.right - s.x) / 2 - s.width / 2
              ? s.x
              : s.x + (s.content_rect.right - s.x) / 2 - s.width / 2,
          s.y, s.content_rect.right, s.content_rect.bottom);
  s.control = GetDlgItem(hwnd, 0x6a6);
  GetWindowTextA(s.control, s.text, 100);
  GetTextExtentPoint32A(s.dc, s.text, strlen(s.text), &s.text_size);
  GetWindowRect(GetDlgItem(hwnd, 0x6a5), &s.control_rect);
  MapWindowPoints(NULL, hwnd, (LPPOINT)&s.control_rect, 2);
  SetWindowPos(s.control, NULL, 0,
               s.control_rect.bottom + s.button_height * 3 / 2,
               s.text_size.cx + s.margin / 2, s.line_height, 4);
  s.control = GetDlgItem(hwnd, 0x6a7);
  GetClientRect(GetDlgItem(hwnd, 0x6a7), &s.control_rect);
  s.control_height = s.control_rect.bottom - s.control_rect.top;
  GetWindowRect(GetDlgItem(hwnd, 0x6a6), &s.control_rect);
  MapWindowPoints(NULL, hwnd, (LPPOINT)&s.control_rect, 2);
  s.width = s.content_rect.right - (s.control_rect.right + 10);
  SetWindowPos(s.control, NULL, s.control_rect.right + 10,
               s.control_rect.top, s.width, s.control_height, 4);
  s.x = 0;
  s.y = s.buttons_rect.top - s.button_height * 2;
  SetRect(&s.status_rect, s.x, s.y,
          s.buttons_rect.left - s.margin * 3,
          s.content_rect.bottom);
  s.control = GetDlgItem(hwnd, 0x6af);
  SetWindowPos(s.control, NULL, s.status_rect.left, s.status_rect.top,
               s.status_rect.right - s.status_rect.left,
               s.status_rect.bottom - s.status_rect.top, 4);
  s.x = s.status_rect.left + s.margin;
  s.y = s.status_rect.top + s.button_height;
  s.control_height = s.status_rect.bottom - s.y;
  s.control = GetDlgItem(hwnd, 0x6a9);
  GetWindowTextA(s.control, s.text, 100);
  GetTextExtentPoint32A(s.dc, s.text, strlen(s.text), &s.text_size);
  SetWindowPos(s.control, NULL, s.x, s.y,
               s.text_size.cx + s.digit_width, s.line_height, 4);
  s.y += s.control_height / 4;
  s.control = GetDlgItem(hwnd, 0x708);
  GetWindowTextA(s.control, s.text, 100);
  GetTextExtentPoint32A(s.dc, s.text, strlen(s.text), &s.text_size);
  s.width = s.text_size.cx + s.margin / 2;
  SetWindowPos(s.control, NULL, s.x, s.y, s.width, s.line_height, 4);
  s.control_width = s.width + s.x;
  GetTextExtentPoint32A(s.dc, "0", 2, &s.text_size);
  s.width = s.text_size.cx + s.margin;
  s.control = GetDlgItem(hwnd, 0x709);
  SetWindowPos(s.control, NULL, s.control_width, s.y, s.width,
               s.button_height, 4);
  s.control_width += s.width;
  GetTextExtentPoint32A(s.dc, "00", 2, &s.text_size);
  s.width = s.text_size.cx;
  s.control = GetDlgItem(hwnd, 0x70a);
  SetWindowPos(s.control, NULL, s.control_width, s.y, s.width,
               s.button_height, 4);
  s.y += s.control_height / 4;
  s.control = GetDlgItem(hwnd, 0x6b0);
  GetWindowTextA(s.control, s.text, 100);
  GetTextExtentPoint32A(s.dc, s.text, strlen(s.text), &s.text_size);
  s.width = s.text_size.cx + s.margin / 2;
  SetWindowPos(s.control, NULL, s.x, s.y, s.width, s.line_height, 4);
  s.control_width = s.width + s.x;
  GetTextExtentPoint32A(s.dc, "000", 3, &s.text_size);
  s.width = s.text_size.cx + s.margin;
  s.control = GetDlgItem(hwnd, 0x6b1);
  SetWindowPos(s.control, NULL, s.control_width, s.y, s.width,
               s.button_height, 4);
  s.control_width += s.width;
  GetTextExtentPoint32A(s.dc, "00", 2, &s.text_size);
  s.width = s.text_size.cx;
  s.control = GetDlgItem(hwnd, 0x6b2);
  SetWindowPos(s.control, NULL, s.control_width, s.y, s.width,
               s.button_height, 4);
  s.y += s.control_height / 4;
  s.control = GetDlgItem(hwnd, 0x6ac);
  GetWindowTextA(s.control, s.text, 100);
  GetTextExtentPoint32A(s.dc, s.text, strlen(s.text), &s.text_size);
  SetWindowPos(s.control, NULL, s.x, s.y, s.text_size.cx + s.margin,
               s.line_height, 4);
  ReleaseDC(hwnd, s.dc);
}

// FUNCTION: MAGIC 0x0045d89e
BOOL CALLBACK shell_gauntlet_dialog_proc(HWND hwnd, UINT message,
                                         WPARAM wparam, LPARAM lparam)
{
  struct
  {
    LOGFONTA resized_font_description;
    HFONT replacement_font;
    int group_index;
    int group_count;
    int group_last[5];
    int control_index;
    int base_heights[4];
    HWND unused_control;
    HWND controls[50];
    int group_first[5];
    LOGFONTA *resize_font_description;
    int control_count;
    RECT shell_rect;
    HFONT old_fonts[5];
    HDC erase_dc;
    RECT erase_rect;
    COLORREF button_color;
    DRAWITEMSTRUCT *draw_item;
    HWND color_control;
    int control_id;
    HDC color_dc;
    int color_gap;
    HWND previous_focus;
    HWND focus_control;
    HWND notify_control;
    int length_position;
    int length_error;
    int best_position;
    int best_error;
    LRESULT selection;
    char autosave_path[264];
    unsigned int save_load_result;
    MSG pending_message;
    char saved_text[100];
    int user_cancelled;
    BOOL autosave_queued;
    WPARAM selected_deck_index;
    int random_selection;
    unsigned int command_id;
    HWND command_hwnd;
    char command_text[100];
    unsigned int notification;
    HFONT destroy_font;
    char deck_name[100];
    int deck_count;
    WPARAM deck_index;
    HFONT init_font;
    LOGFONTA *init_font_description;
    RECT *initial_rect;
  } s;

  switch (message)
  {
  case WM_INITDIALOG:
    shell_prepare_save_game_dialog(2);
    LoadGauntletRegistryOptions();
    s.init_font_description = LoadFontFromIni("ShellDialog", 0);
    shell_gauntlet_dialog_font = CreateFontIndirectA(s.init_font_description);
    s.init_font_description = LoadFontFromIni("ShellDialog_lores", 0);
    shell_gauntlet_dialog_low_resolution_font =
        CreateFontIndirectA(s.init_font_description);
    s.init_font = shell_gauntlet_dialog_font;
    SendDlgItemMessageA(hwnd, 0x6a6, WM_SETFONT,
                        (WPARAM)s.init_font, 0);
    SendDlgItemMessageA(hwnd, 0x6a7, WM_SETFONT, (WPARAM)s.init_font, 0);
    SendDlgItemMessageA(hwnd, 0x6a9, WM_SETFONT, (WPARAM)s.init_font, 0);
    SendDlgItemMessageA(hwnd, 0x708, WM_SETFONT, (WPARAM)s.init_font, 0);
    SendDlgItemMessageA(hwnd, 0x709, WM_SETFONT, (WPARAM)s.init_font, 0);
    SendDlgItemMessageA(hwnd, 0x6b0, WM_SETFONT, (WPARAM)s.init_font, 0);
    SendDlgItemMessageA(hwnd, 0x6b1, WM_SETFONT, (WPARAM)s.init_font, 0);
    SendDlgItemMessageA(hwnd, 0x6ac, WM_SETFONT, (WPARAM)s.init_font, 0);
    SendDlgItemMessageA(hwnd, 0x6ad, WM_SETFONT, (WPARAM)s.init_font, 0);
    SendDlgItemMessageA(hwnd, 0x6b3, WM_SETFONT, (WPARAM)s.init_font, 0);
    SendDlgItemMessageA(hwnd, IDOK, WM_SETFONT, (WPARAM)s.init_font, 0);
    s.init_font_description = LoadFontFromIni("ShellDialogLabel", 0);
    s.init_font = CreateFontIndirectA(s.init_font_description);
    SendDlgItemMessageA(hwnd, 0x6a5, WM_SETFONT, (WPARAM)s.init_font, 0);
    SendDlgItemMessageA(hwnd, 0x6a4, WM_SETFONT, (WPARAM)s.init_font, 0);
    SendDlgItemMessageA(hwnd, 0x6af, WM_SETFONT, (WPARAM)s.init_font, 0);
    load_active_screen_name_profile();
    SetDlgItemTextA(hwnd, 0x6a5, g_screen_name_profile.screen_name);
    sprintf((char *)g_duel_state_block_008b3fc0,
            "%s\\PlayFace\\%s.PIC", global_base_directory,
            g_screen_name_profile.playface_name);
    SendDlgItemMessageA(hwnd, 0x6a7, CB_RESETCONTENT, 0, 0);
    shell_populate_deck_choices(hwnd, 0x6a7, 0, g_gauntlet_options.ante);
    SendDlgItemMessageA(hwnd, 0x6a7, CB_SETCURSEL, 0, 0);
    load_text(global_ui_strings_filename, "SHELLPAGE_GAUNTLET");
    SendDlgItemMessageA(hwnd, 0x6a7, CB_INSERTSTRING, 0,
                        (LPARAM)g_text_lines[2]);
    SendDlgItemMessageA(hwnd, 0x6a7, CB_SETCURSEL, 0, 0);
    if (g_gauntlet_options.player_random == 0)
      SendDlgItemMessageA(hwnd, 0x6a7, CB_SELECTSTRING, 0,
                          (LPARAM)g_gauntlet_options.player_deck);
    s.deck_count = SendDlgItemMessageA(hwnd, 0x6a7, CB_GETCOUNT, 0, 0);
    for (s.deck_index = 1; (int)s.deck_index < s.deck_count;
         ++s.deck_index)
    {
      SendDlgItemMessageA(hwnd, 0x6a7, CB_GETLBTEXT,
                          s.deck_index, (LPARAM)s.deck_name);
      *strchr(s.deck_name, '.') = '\0';
      SendDlgItemMessageA(hwnd, 0x6b4, CB_ADDSTRING, 0,
                          (LPARAM)s.deck_name);
    }
    ShowWindow(GetDlgItem(hwnd, 0x6b4), SW_HIDE);
    if (g_gauntlet_options.ante != 0)
      CheckDlgButton(hwnd, 0x6a9, BST_CHECKED);
    SendDlgItemMessageA(hwnd, 0x70a, 0x465, 0, 0x10005);
    SendDlgItemMessageA(hwnd, 0x70a, 0x467, 0,
                        (WORD)g_gauntlet_options.best_of);
    SendDlgItemMessageA(hwnd, 0x709, EM_SETLIMITTEXT, 1, 0);
    if (g_gauntlet_options.allow_sideboarding != 0)
      CheckDlgButton(hwnd, 0x6ac, BST_CHECKED);
    if (g_gauntlet_options.best_of == 1)
      EnableWindow(GetDlgItem(hwnd, 0x6ac), FALSE);
    g_gauntlet_options.allow_sideboarding = 0;
    ShowWindow(GetDlgItem(hwnd, 0x6ac), SW_HIDE);
    SendDlgItemMessageA(hwnd, 0x6b2, 0x465, 0, 0x50032);
    SendDlgItemMessageA(hwnd, 0x6b2, 0x467, 0,
                        (WORD)g_gauntlet_options.gauntlet_length);
    SendDlgItemMessageA(hwnd, 0x6b1, EM_SETLIMITTEXT, 2, 0);
    load_text(global_ui_strings_filename, "SHELLPAGE_GAUNTLET");
    SetDlgItemTextA(hwnd, 0x6a4, g_text_lines[0]);
    SetDlgItemTextA(hwnd, 0x6a6, g_text_lines[1]);
    shell_format_gauntlet_status(s.deck_name, 3);
    SetDlgItemTextA(hwnd, 0x6af, s.deck_name);
    SetDlgItemTextA(hwnd, 0x6b0, g_text_lines[9]);
    SetDlgItemTextA(hwnd, 0x6a9, g_text_lines[10]);
    SetDlgItemTextA(hwnd, 0x708, g_text_lines[11]);
    SetDlgItemTextA(hwnd, 0x6ac, g_text_lines[12]);
    SetDlgItemTextA(hwnd, 0x6ad, g_text_lines[13]);
    SetDlgItemTextA(hwnd, IDOK, g_text_lines[14]);
    SetDlgItemTextA(hwnd, 0x6b3, g_text_lines[15]);
    SetFocus(GetDlgItem(hwnd, IDOK));
    SendMessageA(hwnd, 0x401, 1, 0);
    change_buttonclass_wndproc(hwnd);
    s.initial_rect = (RECT *)lparam;
    MoveWindow(hwnd, s.initial_rect->left, s.initial_rect->top,
               s.initial_rect->right - s.initial_rect->left,
               s.initial_rect->bottom - s.initial_rect->top, TRUE);
    return FALSE;

  case WM_DESTROY:
    s.destroy_font = (HFONT)SendDlgItemMessageA(hwnd, 0x6a5,
                                                WM_GETFONT, 0, 0);
    SendDlgItemMessageA(hwnd, 0x6a5, WM_SETFONT, 0, 0);
    SendDlgItemMessageA(hwnd, 0x6a4, WM_SETFONT, 0, 0);
    SendDlgItemMessageA(hwnd, 0x6af, WM_SETFONT, 0, 0);
    DeleteObject(s.destroy_font);
    s.destroy_font = (HFONT)SendDlgItemMessageA(hwnd, 0x6a6,
                                                WM_GETFONT, 0, 0);
    SendDlgItemMessageA(hwnd, 0x6a6, WM_SETFONT, 0, 0);
    SendDlgItemMessageA(hwnd, 0x6a7, WM_SETFONT, 0, 0);
    SendDlgItemMessageA(hwnd, 0x6a9, WM_SETFONT, 0, 0);
    SendDlgItemMessageA(hwnd, 0x708, WM_SETFONT, 0, 0);
    SendDlgItemMessageA(hwnd, 0x709, WM_SETFONT, 0, 0);
    SendDlgItemMessageA(hwnd, 0x6b0, WM_SETFONT, 0, 0);
    SendDlgItemMessageA(hwnd, 0x6b1, WM_SETFONT, 0, 0);
    SendDlgItemMessageA(hwnd, 0x6ac, WM_SETFONT, 0, 0);
    SendDlgItemMessageA(hwnd, 0x6ad, WM_SETFONT, 0, 0);
    SendDlgItemMessageA(hwnd, 0x6b3, WM_SETFONT, 0, 0);
    SendDlgItemMessageA(hwnd, IDOK, WM_SETFONT, 0, 0);
    DeleteObject(s.destroy_font);
    return FALSE;

  case WM_COMMAND:
    s.command_id = wparam & 0xffff;
    s.notification = HIWORD(wparam);
    s.command_hwnd = (HWND)lparam;
    switch (s.command_id)
    {
    case IDOK:
      g_duel_network_flags = 1;
      GetDlgItemTextA(hwnd, 0x6a5, g_player_name, 100);
      g_opponent_starting_card_id_1 = -1;
      g_opponent_starting_card_id_2 = g_opponent_starting_card_id_1;
      InitializeDuelUiGlobalIds();
      g_duel_player_face_pic = NULL;
      g_duel_state_00789104 = IsDlgButtonChecked(hwnd, 0x6a9);
      s.random_selection = 0;
      s.selected_deck_index =
          SendDlgItemMessageA(hwnd, 0x6a7, CB_GETCURSEL, 0, 0);
      if (s.selected_deck_index == 0)
      {
        s.selected_deck_index =
            SendDlgItemMessageA(hwnd, 0x6a7, CB_GETCOUNT, 0, 0);
        if ((int)s.selected_deck_index > 1)
        {
          s.selected_deck_index =
              rand() % (int)(s.selected_deck_index - 1) + 1;
          SendDlgItemMessageA(hwnd, 0x6a7, CB_SETCURSEL,
                              s.selected_deck_index, 0);
          s.random_selection = 1;
        }
        else
          s.selected_deck_index = (WPARAM)-1;
      }
      if (s.selected_deck_index == (WPARAM)-1)
        strcpy(shell_gauntlet_player_deck_path, "");
      else
      {
        SendDlgItemMessageA(hwnd, 0x6a7, CB_GETLBTEXT,
                            s.selected_deck_index,
                            (LPARAM)s.command_text);
        *strchr(s.command_text, '.') = '\0';
        strcpy(g_gauntlet_options.player_deck, s.command_text);
        save_gauntlet_options_to_registry();
        sprintf(shell_gauntlet_player_deck_path, "%s\\%s.dck",
                global_playdeck_path, g_gauntlet_options.player_deck);
      }
      shell_enable_animation(0);
      if (shell_load_gauntlet_decks(shell_gauntlet_player_deck_path,
                                    NULL) != 0)
        shell_run_gauntlet(0, g_gauntlet_options.gauntlet_length,
                           GetDlgItem(hwnd, 0x6b4));
      if (s.random_selection != 0)
        SendDlgItemMessageA(hwnd, 0x6a7, CB_SETCURSEL, 0, 0);
      SetFocus(GetDlgItem(hwnd, IDOK));
      SendMessageA(global_main_hwnd, 0x496, 1, 0);
      shell_enable_animation(1);
      break;

    case 0x6b3:
      g_duel_network_flags = 1;
      Sleep(GetDoubleClickTime());
      s.autosave_queued =
          PeekMessageA(&s.pending_message, NULL, WM_LBUTTONDBLCLK,
                       WM_LBUTTONDBLCLK, PM_REMOVE);
      shell_prepare_save_game_dialog(2);
      g_duel_save_game_openfilename.hwndOwner = global_main_hwnd;
      load_text(global_ui_strings_filename, "WINDOWTITLES");
      g_duel_save_game_openfilename.lpstrTitle = g_text_lines[6];
      g_duel_save_game_openfilename.Flags = 0x2a100c;
      if (s.autosave_queued != 0)
      {
        strcpy(s.autosave_path, global_savegame_path);
        strcat(s.autosave_path, "\\AUTOSAVE.");
        strcat(s.autosave_path,
               g_duel_save_game_openfilename.lpstrDefExt);
        strcpy(g_duel_save_game_openfilename.lpstrFile,
               s.autosave_path);
        InitializeDuelUiGlobalIds();
        s.save_load_result = load_duel_run_mode_2_save(s.autosave_path);
        s.user_cancelled = 0;
      }
      else
      {
        if (GetOpenFileNameA(&g_duel_save_game_openfilename) != 0)
        {
          InitializeDuelUiGlobalIds();
          s.save_load_result = load_duel_run_mode_2_save(
              g_duel_save_game_openfilename.lpstrFile);
          s.user_cancelled = 0;
        }
        else
        {
          s.save_load_result = 0;
          s.user_cancelled = 1;
        }
      }
      shell_enable_animation(0);
      if (s.save_load_result != 0)
      {
        SendDlgItemMessageA(hwnd, 0x70a, 0x467, 0,
                            (WORD)g_gauntlet_options.best_of);
        if (g_gauntlet_options.allow_sideboarding != 0)
          CheckDlgButton(hwnd, 0x6ac, BST_CHECKED);
        if (g_gauntlet_options.best_of == 1)
          EnableWindow(GetDlgItem(hwnd, 0x6ac), FALSE);
        SendDlgItemMessageA(hwnd, 0x6b2, 0x467, 0,
                            (WORD)g_gauntlet_options.gauntlet_length);
        shell_format_gauntlet_status(s.saved_text, 3);
        SetDlgItemTextA(hwnd, 0x6af, s.saved_text);
        InvalidateRect(GetDlgItem(hwnd, 0x6af), NULL, TRUE);
        PostMessageA(global_main_hwnd, 0x496, 0, 0);
        shell_run_gauntlet(1, g_gauntlet_options.gauntlet_length,
                           GetDlgItem(hwnd, 0x6b4));
        SendMessageA(global_main_hwnd, 0x496, 1, 0);
      }
      else if (s.user_cancelled == 0)
        MessageBoxA(hwnd,
                    "Couldn't load the save game; corrupt file or from a different version",
                    "Load saved game", 0);
      shell_enable_animation(1);
      break;

    case 0x6ad:
      shell_enable_animation(0);
      shell_open_duel_interface_options(global_main_hwnd);
      shell_enable_animation(1);
      SetFocus(GetDlgItem(hwnd, IDOK));
      break;

    case 0x6a7:
      if (s.notification == CBN_SELCHANGE)
      {
        s.selection = SendMessageA(s.command_hwnd, CB_GETCURSEL, 0, 0);
        g_gauntlet_options.player_random = s.selection == 0;
        save_gauntlet_options_to_registry();
      }
      if (s.notification == CBN_SELENDOK)
      {
        s.selection = SendMessageA(s.command_hwnd, CB_GETCURSEL, 0, 0);
        if (s.selection != 0)
          GetDlgItemTextA(hwnd, 0x6a7,
                          g_gauntlet_options.player_deck, 0x32);
        save_gauntlet_options_to_registry();
      }
      break;

    case 0x6a9:
      g_gauntlet_options.ante = IsDlgButtonChecked(hwnd, 0x6a9);
      save_gauntlet_options_to_registry();
      shell_format_gauntlet_status(s.command_text, 3);
      SetDlgItemTextA(hwnd, 0x6af, s.command_text);
      InvalidateRect(GetDlgItem(hwnd, 0x6af), NULL, TRUE);
      SendDlgItemMessageA(hwnd, 0x6a7, CB_RESETCONTENT, 0, 0);
      shell_populate_deck_choices(hwnd, 0x6a7, 0,
                                  g_gauntlet_options.ante);
      load_text(global_ui_strings_filename, "SHELLPAGE_GAUNTLET");
      SendDlgItemMessageA(hwnd, 0x6a7, CB_INSERTSTRING, 0,
                          (LPARAM)g_text_lines[2]);
      SendDlgItemMessageA(hwnd, 0x6a7, CB_SETCURSEL, 0, 0);
      break;

    case 0x709:
      if (s.notification == 0x200)
      {
        s.best_position =
            SendDlgItemMessageA(hwnd, 0x70a, 0x468, 0, 0);
        s.best_error = HIWORD(s.best_position) != 0;
        if (s.best_error != 0)
          SendDlgItemMessageA(hwnd, 0x70a, 0x467, 0,
                              (WORD)g_gauntlet_options.best_of);
        else
        {
          if (s.best_position % 2 == 0)
          {
            if (g_gauntlet_options.best_of < (int)s.best_position)
              ++s.best_position;
            else
              --s.best_position;
            SendDlgItemMessageA(hwnd, 0x70a, 0x467, 0,
                                s.best_position & 0xffff);
            s.best_position =
                SendDlgItemMessageA(hwnd, 0x70a, 0x468, 0, 0);
          }
          if (g_gauntlet_options.best_of != (int)s.best_position)
          {
            g_gauntlet_options.best_of = s.best_position;
            save_gauntlet_options_to_registry();
          }
          EnableWindow(GetDlgItem(hwnd, 0x6ac),
                       g_gauntlet_options.best_of > 1);
          shell_format_gauntlet_status(s.command_text, 3);
          SetDlgItemTextA(hwnd, 0x6af, s.command_text);
          InvalidateRect(GetDlgItem(hwnd, 0x6af), NULL, TRUE);
        }
      }
      break;
    case 0x6ac:
      g_gauntlet_options.allow_sideboarding =
          IsDlgButtonChecked(hwnd, 0x6ac);
      save_gauntlet_options_to_registry();
      shell_format_gauntlet_status(s.command_text, 3);
      SetDlgItemTextA(hwnd, 0x6af, s.command_text);
      InvalidateRect(GetDlgItem(hwnd, 0x6af), NULL, TRUE);
      break;

    case 0x6b1:
      if (s.notification == 0x200)
      {
        s.length_position =
            SendDlgItemMessageA(hwnd, 0x6b2, 0x468, 0, 0);
        s.length_error = HIWORD(s.length_position) != 0;
        if (s.length_error != 0)
          SendDlgItemMessageA(hwnd, 0x6b2, 0x467, 0,
                              (WORD)g_gauntlet_options.gauntlet_length);
        else
        {
          if (g_gauntlet_options.gauntlet_length != (int)s.length_position)
          {
            g_gauntlet_options.gauntlet_length = s.length_position;
            save_gauntlet_options_to_registry();
            shell_format_gauntlet_status(s.command_text, 3);
            SetDlgItemTextA(hwnd, 0x6af, s.command_text);
            InvalidateRect(GetDlgItem(hwnd, 0x6af), NULL, TRUE);
          }
        }
      }
      break;
    }
    return TRUE;

  case WM_NOTIFY:
    s.notify_control = (HWND)wparam;
    if (s.notify_control == (HWND)0x6b2)
      PostMessageA(hwnd, WM_COMMAND, 0x20006b1,
                   (LPARAM)GetDlgItem(hwnd, 0x6b1));
    else if (s.notify_control == (HWND)0x70a)
      PostMessageA(hwnd, WM_COMMAND, 0x2000709,
                   (LPARAM)GetDlgItem(hwnd, 0x709));
    return FALSE;

  case 0x4c8:
    s.focus_control = (HWND)wparam;
    s.previous_focus = (HWND)lparam;
    if (s.focus_control == GetDlgItem(hwnd, IDOK) ||
        s.focus_control == GetDlgItem(hwnd, 0x6b3) ||
        s.focus_control == GetDlgItem(hwnd, 0x6ad))
      SendMessageA(hwnd, 0x401, GetDlgCtrlID(s.focus_control), 0);
    else
      SendMessageA(hwnd, 0x401, 1, 0);
    if (s.focus_control != NULL)
      InvalidateRect(s.focus_control, NULL, TRUE);
    if (s.previous_focus != NULL)
      InvalidateRect(s.previous_focus, NULL, TRUE);
    return FALSE;
  case WM_CTLCOLOREDIT:
  case WM_CTLCOLORBTN:
  case WM_CTLCOLORSTATIC:
    s.color_dc = (HDC)wparam;
    ApplyCardArtPaletteToDc(s.color_dc);
    s.color_control = (HWND)lparam;
    s.control_id = GetDlgCtrlID(s.color_control);
    if (s.control_id == 0x6af)
    {
      SetTextColor(s.color_dc, shell_draw_resources.button_text_color);
      SetBkMode(s.color_dc, TRANSPARENT);
      s.color_gap = (int)shell_draw_resources.light_brush;
    }
    else if (s.control_id == 0x6a7 || s.control_id == 0x6b1 ||
             s.control_id == 0x709)
    {
      SetTextColor(s.color_dc, shell_draw_resources.background_color);
      SetBkMode(s.color_dc, OPAQUE);
      SetBkColor(s.color_dc, shell_draw_resources.panel_color);
      s.color_gap = (int)shell_draw_resources.panel_brush;
    }
    else
    {
      if (GetFocus() == s.color_control)
        SetTextColor(s.color_dc, shell_draw_resources.selected_button_color);
      else if (s.control_id == 0x6a5)
        SetTextColor(s.color_dc, shell_draw_resources.selected_button_color);
      else
        SetTextColor(s.color_dc, shell_draw_resources.button_text_color);
      SetBkMode(s.color_dc, TRANSPARENT);
      s.color_gap = (int)GetStockObject(NULL_BRUSH);
    }
    return s.color_gap;

  case WM_DRAWITEM:
    s.draw_item = (DRAWITEMSTRUCT *)lparam;
    if (GetFocus() == s.draw_item->hwndItem)
      s.button_color = shell_draw_resources.selected_button_color;
    else if (s.draw_item->itemState & ODS_GRAYED)
      s.button_color = 0x10000c6;
    else
      s.button_color = shell_draw_resources.regular_button_color;
    if (shell_draw_resources.button_face != NULL)
      shell_draw_bitmap_button(
          s.draw_item, shell_draw_resources.button_face,
          shell_draw_resources.light_pen, shell_draw_resources.dark_pen,
          s.button_color, 0);
    else
      draw_owner_draw_button_centered(
          s.draw_item, (HBRUSH)GetStockObject(2),
          shell_draw_resources.light_pen, shell_draw_resources.dark_pen,
          s.button_color, 0);
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
    MapWindowPoints(hwnd, GetParent(hwnd), (LPPOINT)&s.erase_rect, 2);
    SendMessageA(GetParent(hwnd), 0x498, (WPARAM)s.erase_dc,
                 (LPARAM)&s.erase_rect);
    return TRUE;

  case WM_SIZE:
    s.group_count = 0;
    s.control_count = 0;
    s.group_first[s.group_count] = s.control_count;
    s.controls[s.control_count++] = GetDlgItem(hwnd, 0x6a4);
    s.controls[s.control_count++] = GetDlgItem(hwnd, 0x6a5);
    s.controls[s.control_count++] = GetDlgItem(hwnd, 0x6af);
    s.group_last[s.group_count] = s.control_count - 1;
    s.old_fonts[s.group_count] =
        (HFONT)SendMessageA(s.controls[s.control_count - 1],
                            WM_GETFONT, 0, 0);
    s.resize_font_description = LoadFontFromIni("ShellDialogLabel", 0);
    s.base_heights[s.group_count] = s.resize_font_description->lfHeight;
    ++s.group_count;
    s.group_first[s.group_count] = s.control_count;
    s.controls[s.control_count++] = GetDlgItem(hwnd, 0x6a6);
    s.controls[s.control_count++] = GetDlgItem(hwnd, 0x6a7);
    s.controls[s.control_count++] = GetDlgItem(hwnd, 0x6a9);
    s.controls[s.control_count++] = GetDlgItem(hwnd, 0x708);
    s.controls[s.control_count++] = GetDlgItem(hwnd, 0x709);
    s.controls[s.control_count++] = GetDlgItem(hwnd, 0x70a);
    s.controls[s.control_count++] = GetDlgItem(hwnd, 0x6b0);
    s.controls[s.control_count++] = GetDlgItem(hwnd, 0x6b1);
    s.controls[s.control_count++] = GetDlgItem(hwnd, 0x6b2);
    s.controls[s.control_count++] = GetDlgItem(hwnd, 0x6ac);
    s.controls[s.control_count++] = GetDlgItem(hwnd, 0x6ad);
    s.controls[s.control_count++] = GetDlgItem(hwnd, 0x6b3);
    s.controls[s.control_count++] = GetDlgItem(hwnd, IDOK);
    s.group_last[s.group_count] = s.control_count - 1;
    GetClientRect(global_main_hwnd, &s.shell_rect);
    if (s.shell_rect.right < 0x2ee || s.shell_rect.bottom < 0x226)
    {
      s.resize_font_description = LoadFontFromIni("ShellDialog_lores", 0);
      s.old_fonts[s.group_count] =
          shell_gauntlet_dialog_low_resolution_font;
    }
    else
    {
      s.resize_font_description = LoadFontFromIni("ShellDialog", 0);
      s.old_fonts[s.group_count] = shell_gauntlet_dialog_font;
    }
    s.base_heights[s.group_count] = s.resize_font_description->lfHeight;
    ++s.group_count;
    LockWindowUpdate(hwnd);
    for (s.group_index = 0; s.group_index < s.group_count;
         ++s.group_index)
    {
      GetObjectA(s.old_fonts[s.group_index],
                 sizeof(s.resized_font_description),
                 &s.resized_font_description);
      GetClientRect(global_main_hwnd, &s.shell_rect);
      s.resized_font_description.lfHeight =
          (s.shell_rect.bottom - s.shell_rect.top) /
          s.base_heights[s.group_index];
      if (s.resized_font_description.lfHeight % 2 != 0)
        ++s.resized_font_description.lfHeight;
      s.replacement_font = CreateFontIndirectA(
          &s.resized_font_description);
      if (s.old_fonts[s.group_index] ==
          shell_gauntlet_dialog_low_resolution_font)
        shell_gauntlet_dialog_low_resolution_font = s.replacement_font;
      else if (s.old_fonts[s.group_index] == shell_gauntlet_dialog_font)
        shell_gauntlet_dialog_font = s.replacement_font;
      SendMessageA(hwnd, WM_SETFONT, (WPARAM)s.replacement_font, 0);
      for (s.control_index = s.group_first[s.group_index];
           s.control_index <= s.group_last[s.group_index];
           ++s.control_index)
        SendMessageA(s.controls[s.control_index], WM_SETFONT,
                     (WPARAM)s.replacement_font, 0);
      DeleteObject(s.old_fonts[s.group_index]);
    }
    shell_layout_gauntlet_controls(hwnd);
    LockWindowUpdate(NULL);
    return TRUE;

  case 0x499:
    return FALSE;

  default:
    return FALSE;
  }
}
