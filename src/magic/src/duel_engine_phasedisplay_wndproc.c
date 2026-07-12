#include <windows.h>
#include <string.h>
#include <stdio.h>

#include "cardartlib/src/palette.h"
#include "drawcardlib/src/pic.h"
#include "magic/src/duel_engine.h"
#include "magic/src/game_support.h"
#include "magic/src/global_other.h"
#include "magic/src/global_state.h"
#include "magic/src/global_strings.h"
#include "magic/src/shared_startup.h"

#define PHASE_DISPLAY_SELECTED_CARD_OFFSET 0
#define PHASE_DISPLAY_SELECTED_PLAYER_OFFSET 4

#define PHASE_DISPLAY_COMMAND_HELP 0x64
#define PHASE_DISPLAY_COMMAND_STOP_BASE 0x96
#define PHASE_DISPLAY_COMMAND_TOGGLE_BASE 0xc8
#define PHASE_DISPLAY_COMMAND_HELP_BASE 0xfa

#define PHASE_DISPLAY_PLAYER_COMMAND_OFFSET 10
#define PHASE_DISPLAY_BITMAP_WIDTH 0x2f8

#ifndef SHANDALAR
extern HWND global_main_hwnd;
#define DUEL_MAIN_WINDOW_HWND global_main_hwnd
#else
extern HWND g_main_window_hwnd;
#define DUEL_MAIN_WINDOW_HWND g_main_window_hwnd
#endif

// GLOBAL: MAGIC 0x0057e49c
char s_MENU_PHASEBAR_0057e49c[0x10] = "MENU_PHASEBAR";

// GLOBAL: MAGIC 0x0057e4ac
char s__WINBK_Phase_pic_0057e4ac[0x14] = "\\WINBK_Phase.pic";

// GLOBAL: MAGIC 0x0057e4d8
char s__duel_hlp_0057e4d8[0xc] = "\\duel.hlp";

// GLOBAL: MAGIC 0x0057e4e4
char s__duel_hlp_0057e4e4[0xc] = "\\duel.hlp";

// GLOBAL: MAGIC 0x0057e4f0
char s__WINBK_Phase_pic_0057e4f0[0x14] = "\\WINBK_Phase.pic";

// GLOBAL: MAGIC 0x0055e1b4
int g_phase_display_selected_card_window_long_offset = 0;

// GLOBAL: MAGIC 0x0055e1b8
int g_phase_display_selected_player_window_long_offset = 4;

// GLOBAL: MAGIC 0x0055e1bc
int g_phase_display_window_extra_bytes = 8;

// GLOBAL: MAGIC 0x00708768
HANDLE g_magicgame_phase_display_pic;

// GLOBAL: MAGIC 0x00708770
char g_phase_display_menu_phase_help_text[0x38];

// GLOBAL: MAGIC 0x007087a8
char g_phase_display_menu_toggle_text[0x38];

// GLOBAL: MAGIC 0x007087e0
int g_phase_display_click_packet[3];

// GLOBAL: MAGIC 0x007087f0
char g_phase_display_menu_help_text[0x1c];

// GLOBAL: MAGIC 0x0070880c
HBRUSH g_phase_display_hatch_brush;

// GLOBAL: MAGIC 0x00708810
HMENU g_phase_display_menu;

// GLOBAL: MAGIC 0x00708818
int g_phase_display_menu_packet[3];

// GLOBAL: MAGIC 0x00708848
int g_phase_display_bitmap_divisor;

// GLOBAL: MAGIC 0x00708850
char g_phase_display_menu_stop_text[0x38];

extern char global_base_directory[];
extern char global_duelart_path[];
extern int g_duel_modal_action_active;
extern int DAT_00789710;
extern int DAT_007aa928;
extern int DAT_008b3400[0x26];
extern int DAT_00925360[0x26];

void get_current_duel_selection(int *selected_player, int *selected_card);
void save_duel_interface_options_to_registry(void);
unsigned int refresh_duel_display_cache(void);
LRESULT handle_duel_inactive_cursor(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
void delete_and_close_object(HANDLE obj);
int load_text_with_tab_escapes(char *filename, char *section_name);
int is_invalid_duel_player(int player);

static void get_phase_display_phase_rect(RECT *rect, int player, int phase, int width, int height);
static void hit_test_phase_display(POINT *point, RECT *client_rect, int *player, int *phase);
void copy_phase_stop_flags(int *phase_flags, int player);
void get_phase_display_selected_half(int *selected_half);
void get_phase_display_action_selection(int *player, int *phase);
void draw_phase_display_status_markers(HDC dc, RECT *client_rect);
void noop_phase_display_user_message(HWND hwnd, WPARAM wparam, LPARAM lparam);

// FUNCTION: MAGIC 0x00536e7e
static void hit_test_phase_display(POINT *point, RECT *client_rect, int *player, int *phase)
{
  struct
  {
    int found_player;
    int test_x;
    int test_y;
    RECT client_copy;
    RECT phase_rect;
    int found_phase;
  } s;

  CopyRect(&s.client_copy, client_rect);
  s.test_x = point->x;
  s.test_y = point->y;
  s.found_phase = -1;
  s.found_player = 1;

  get_phase_display_phase_rect(&s.phase_rect, 1, 1, s.client_copy.right, s.client_copy.bottom);
  if (PtInRect(&s.phase_rect, *(POINT *)&s.test_x) != 0)
    s.found_phase = 1;
  get_phase_display_phase_rect(&s.phase_rect, 1, 4, s.client_copy.right, s.client_copy.bottom);
  if (PtInRect(&s.phase_rect, *(POINT *)&s.test_x) != 0)
    s.found_phase = 4;
  get_phase_display_phase_rect(&s.phase_rect, 1, 0xa, s.client_copy.right, s.client_copy.bottom);
  if (PtInRect(&s.phase_rect, *(POINT *)&s.test_x) != 0)
    s.found_phase = 0xa;
  get_phase_display_phase_rect(&s.phase_rect, 1, 0x14, s.client_copy.right, s.client_copy.bottom);
  if (PtInRect(&s.phase_rect, *(POINT *)&s.test_x) != 0)
    s.found_phase = 0x14;
  get_phase_display_phase_rect(&s.phase_rect, 1, 0x16, s.client_copy.right, s.client_copy.bottom);
  if (PtInRect(&s.phase_rect, *(POINT *)&s.test_x) != 0)
    s.found_phase = 0x16;
  get_phase_display_phase_rect(&s.phase_rect, 1, 0x1e, s.client_copy.right, s.client_copy.bottom);
  if (PtInRect(&s.phase_rect, *(POINT *)&s.test_x) != 0)
    s.found_phase = 0x1e;
  get_phase_display_phase_rect(&s.phase_rect, 1, 0x1f, s.client_copy.right, s.client_copy.bottom);
  if (PtInRect(&s.phase_rect, *(POINT *)&s.test_x) != 0)
    s.found_phase = 0x1f;
  get_phase_display_phase_rect(&s.phase_rect, 1, 0x20, s.client_copy.right, s.client_copy.bottom);
  if (PtInRect(&s.phase_rect, *(POINT *)&s.test_x) != 0)
    s.found_phase = 0x20;

  if (s.found_phase == -1)
  {
    s.found_player = 0;
    get_phase_display_phase_rect(&s.phase_rect, 0, 1, s.client_copy.right, s.client_copy.bottom);
    if (PtInRect(&s.phase_rect, *(POINT *)&s.test_x) != 0)
      s.found_phase = 1;
    get_phase_display_phase_rect(&s.phase_rect, 0, 4, s.client_copy.right, s.client_copy.bottom);
    if (PtInRect(&s.phase_rect, *(POINT *)&s.test_x) != 0)
      s.found_phase = 4;
    get_phase_display_phase_rect(&s.phase_rect, 0, 0xa, s.client_copy.right, s.client_copy.bottom);
    if (PtInRect(&s.phase_rect, *(POINT *)&s.test_x) != 0)
      s.found_phase = 0xa;
    get_phase_display_phase_rect(&s.phase_rect, 0, 0x14, s.client_copy.right, s.client_copy.bottom);
    if (PtInRect(&s.phase_rect, *(POINT *)&s.test_x) != 0)
      s.found_phase = 0x14;
    get_phase_display_phase_rect(&s.phase_rect, 0, 0x15, s.client_copy.right, s.client_copy.bottom);
    if (PtInRect(&s.phase_rect, *(POINT *)&s.test_x) != 0)
      s.found_phase = 0x15;
    get_phase_display_phase_rect(&s.phase_rect, 0, 0x1e, s.client_copy.right, s.client_copy.bottom);
    if (PtInRect(&s.phase_rect, *(POINT *)&s.test_x) != 0)
      s.found_phase = 0x1e;
    get_phase_display_phase_rect(&s.phase_rect, 0, 0x1f, s.client_copy.right, s.client_copy.bottom);
    if (PtInRect(&s.phase_rect, *(POINT *)&s.test_x) != 0)
      s.found_phase = 0x1f;
    get_phase_display_phase_rect(&s.phase_rect, 0, 0x20, s.client_copy.right, s.client_copy.bottom);
    if (PtInRect(&s.phase_rect, *(POINT *)&s.test_x) != 0)
      s.found_phase = 0x20;
  }

  *player = s.found_player;
  *phase = s.found_phase;
}

// FUNCTION: MAGIC 0x0053726a
static void get_phase_display_phase_rect(RECT *rect, int player, int phase, int width, int height)
{
  struct
  {
    int top;
    int left;
  } s;

  if (player == -1 || phase == -1)
  {
    SetRect(rect, 0, 0, 0, 0);
    return;
  }

  s.top = (height * 40) / PHASE_DISPLAY_BITMAP_WIDTH;
  if (phase == 1)
    s.left = (height * 2) / PHASE_DISPLAY_BITMAP_WIDTH;
  else if (phase == 2 || phase == 3 || phase == 4 || phase == 5)
    s.left = (height * 43) / PHASE_DISPLAY_BITMAP_WIDTH;
  else if (phase == 0xa)
    s.left = (height * 84) / PHASE_DISPLAY_BITMAP_WIDTH;
  else if (phase == 0x14)
    s.left = (height * 125) / PHASE_DISPLAY_BITMAP_WIDTH;
  else if ((player == 1 && phase == 0x16) || (player == 0 && phase == 0x15))
    s.left = (height * 166) / PHASE_DISPLAY_BITMAP_WIDTH;
  else if (phase == 0x1e)
    s.left = (height * 207) / PHASE_DISPLAY_BITMAP_WIDTH;
  else if (phase == 0x1f)
    s.left = (height * 248) / PHASE_DISPLAY_BITMAP_WIDTH;
  else if (phase == 0x20 || phase == 0x21 || phase == 0x22 || phase == 0x25)
    s.left = (height * 289) / PHASE_DISPLAY_BITMAP_WIDTH;
  else
  {
    SetRect(rect, 0, 0, 0, 0);
    return;
  }

  if (player == 0)
    s.left += (height * 430) / PHASE_DISPLAY_BITMAP_WIDTH;

  SetRect(rect, 0, s.left, width, s.left + s.top);
}

// FUNCTION: MAGIC 0x0044a213
void copy_phase_stop_flags(int *phase_flags, int player)
{
  if (phase_flags == (int *)0 || is_invalid_duel_player(player) != 0)
    return;

  EnterCriticalSection(&g_duel_render_lock);
  if (player == 0)
    memcpy(phase_flags, DAT_00925360, 0x98);
  else
    memcpy(phase_flags, DAT_008b3400, 0x98);
  LeaveCriticalSection(&g_duel_render_lock);
}

// FUNCTION: MAGIC 0x0044a288
void get_phase_display_selected_half(int *selected_half)
{
  EnterCriticalSection(&g_duel_render_lock);
  if (selected_half != (int *)0)
    *selected_half = DAT_00789710;
  LeaveCriticalSection(&g_duel_render_lock);
}

// FUNCTION: MAGIC 0x0044a376
void get_phase_display_action_selection(int *player, int *phase)
{
  EnterCriticalSection(&g_duel_render_lock);
  if (player != (int *)0)
    *player = DAT_007aa928;
  if (phase != (int *)0)
    *phase = DAT_007abc74;
  LeaveCriticalSection(&g_duel_render_lock);
}

// FUNCTION: MAGIC 0x005374b3
void draw_phase_display_status_markers(HDC dc, RECT *client_rect)
{
  struct
  {
    int player;
    int phase;
    HBRUSH brush;
    HGDIOBJ old_brush;
    int phase_index;
    int player_index;
    RECT phase_rect;
    RECT ellipse_rect;
    int phase_flags[0x26];
  } s;

  get_phase_display_action_selection(&s.player, &s.phase);
  if (s.player == -1 || s.phase == -1)
  {
    s.brush = CreateSolidBrush(0xff);
    s.old_brush = SelectObject(dc, s.brush);
    for (s.player_index = 0; s.player_index < 2; ++s.player_index)
    {
      copy_phase_stop_flags(s.phase_flags, s.player_index);
      for (s.phase_index = 0; s.phase_index < 0x26; ++s.phase_index)
      {
        if (s.phase_flags[s.phase_index] != 0)
        {
          get_phase_display_phase_rect(&s.phase_rect, s.player_index, s.phase_index,
                                       client_rect->right, client_rect->bottom);
          if (IsRectEmpty(&s.phase_rect) == 0)
          {
            CopyRect(&s.ellipse_rect, &s.phase_rect);
            s.ellipse_rect.left = s.ellipse_rect.right -
                                  (s.phase_rect.right - s.phase_rect.left) / 3;
            s.ellipse_rect.top = s.ellipse_rect.bottom -
                                 ((s.ellipse_rect.right - s.ellipse_rect.left) *
                                  (s.phase_rect.bottom - s.phase_rect.top)) /
                                     (s.phase_rect.right - s.phase_rect.left);
            Ellipse(dc, s.ellipse_rect.left, s.ellipse_rect.top,
                    s.ellipse_rect.right, s.ellipse_rect.bottom);
          }
        }
      }
    }
    SelectObject(dc, s.old_brush);
    DeleteObject(s.brush);
  }

  if (s.player != -1 && s.phase != -1)
  {
    s.brush = CreateSolidBrush(0xff00);
    s.old_brush = SelectObject(dc, s.brush);
    for (s.player_index = 0; s.player_index < 2; ++s.player_index)
    {
      for (s.phase_index = 0; s.phase_index < 0x26; ++s.phase_index)
      {
        if (s.player_index == s.player && s.phase_index == s.phase)
        {
          get_phase_display_phase_rect(&s.phase_rect, s.player_index, s.phase_index,
                                       client_rect->right, client_rect->bottom);
          if (IsRectEmpty(&s.phase_rect) == 0)
          {
            CopyRect(&s.ellipse_rect, &s.phase_rect);
            s.ellipse_rect.right = s.ellipse_rect.left +
                                   (s.phase_rect.right - s.phase_rect.left) / 3;
            s.ellipse_rect.top = s.ellipse_rect.bottom -
                                 ((s.ellipse_rect.right - s.ellipse_rect.left) *
                                  (s.phase_rect.bottom - s.phase_rect.top)) /
                                     (s.phase_rect.right - s.phase_rect.left);
            Ellipse(dc, s.ellipse_rect.left, s.ellipse_rect.top,
                    s.ellipse_rect.right, s.ellipse_rect.bottom);
          }
        }
      }
    }
    SelectObject(dc, s.old_brush);
    DeleteObject(s.brush);
  }
}

// FUNCTION: MAGIC 0x005390d6
void noop_phase_display_user_message(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
}

// FUNCTION: MAGIC 0x00535650
// FUNCTION: SHANDALAR 0x00559e90
int register_MAGICGAME_PhaseDisplayClass(LPCSTR class_name)
{
  struct
  {
    char pic_path[264];
    int result;
    WNDCLASSA wndclass;
  } s;

  s.result = 1;
  s.wndclass.style = 0xb;
  s.wndclass.lpfnWndProc = wndproc_MAGICGAME_PhaseDisplayClass;
  s.wndclass.cbClsExtra = 0;
  s.wndclass.cbWndExtra = g_phase_display_window_extra_bytes;
  s.wndclass.hInstance = g_app_instance;
  s.wndclass.hIcon = LoadIconA((HINSTANCE)0, (LPCSTR)0x7f00);
  s.wndclass.hCursor = LoadCursorA((HINSTANCE)0, (LPCSTR)0x7f00);
  s.wndclass.hbrBackground = (HBRUSH)0x6;
  s.wndclass.lpszMenuName = (LPCSTR)0;
  s.wndclass.lpszClassName = class_name;
  if (RegisterClassA(&s.wndclass) == 0)
    s.result = 0;

  g_phase_display_menu = CreatePopupMenu();
  load_text_with_tab_escapes(global_ui_strings_filename, s_MENU_PHASEBAR_0057e49c);
  strcpy(g_phase_display_menu_stop_text, text_lines[0]);
  strcpy(g_phase_display_menu_toggle_text, text_lines[1]);
  strcpy(g_phase_display_menu_phase_help_text, text_lines[2]);
  strcpy(g_phase_display_menu_help_text, text_lines[3]);
  strcpy(s.pic_path, global_duelart_path);
  strcat(s.pic_path, s__WINBK_Phase_pic_0057e4ac);
  g_magicgame_phase_display_pic = load_pic(s.pic_path);
  g_phase_display_bitmap_divisor = 2;
  g_phase_display_hatch_brush = CreateHatchBrush(3, 0x808080);
  return s.result;
}

// FUNCTION: MAGIC 0x0053592b
// FUNCTION: SHANDALAR 0x0055a16b
LRESULT CALLBACK wndproc_MAGICGAME_PhaseDisplayClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  struct
  {
    int menu_remaining;
    int menu_count;
    int menu_hit_phase;
    int menu_values[0x26];
    POINT menu_cursor_point;
    int menu_help_command;
    int menu_player_offset;
    int menu_phase_index;
    int menu_stop_command;
    int menu_hit_player;
    int menu_toggle_command;
    RECT menu_client_rect;
    POINT popup_point;
    RECT popup_rect;
    int selected_half;
    char pic_path[264];
    HRGN paint_clip_region;
    HDC paint_dc;
    int paint_hit_phase;
    BITMAP paint_bitmap;
    PAINTSTRUCT paint;
    int paint_saved_dc;
    RECT paint_phase_rect;
    int paint_hit_player;
    RECT paint_client_rect;
    MSG peek_message;
    int click_hit_player;
    int peek_result;
    POINT click_point;
    RECT click_client_rect;
    int click_hit_phase;
    char phase_help_path[264];
    int phase_help_context;
    int toggle_phase_code;
    int toggle_player;
    RECT toggle_client_rect;
    RECT toggle_phase_rect;
    int stop_phase_code;
    int stop_player;
    int command_phase;
    int command_player;
    int command_base;
    int command_phase_index;
    char general_help_path[264];
    int general_help_context;
    int refresh_current_card;
    int refresh_current_player;
    char cue_text[100];
    int cue_hit_phase;
    POINT cue_point;
    int cue_valid;
    char opponent_name[100];
    int cue_hit_player;
    RECT cue_client_rect;
    int cached_card;
    int cached_player;
  } s;

  switch (msg)
  {
  case 0x437:
    s.cue_point.x = lparam & 0xffff;
    s.cue_point.y = HIWORD(lparam);
    GetClientRect(hwnd, &s.cue_client_rect);
    hit_test_phase_display(&s.cue_point, &s.cue_client_rect, &s.cue_hit_player, &s.cue_hit_phase);
    s.cue_valid = 1;
    copy_opponent_name_prefix(s.opponent_name);
    switch (s.cue_hit_phase)
    {
    case 1:
      if (s.cue_hit_player == 1)
        sprintf(s.cue_text, gs_phasebar_opponent_untap_008a8d80, s.opponent_name);
      else
        strcpy(s.cue_text, gs_phasebar_your_untap_008b3280);
      break;
    case 2:
    case 3:
    case 4:
    case 5:
      if (s.cue_hit_player == 1)
        sprintf(s.cue_text, gs_phasebar_opponent_upkeep_008a9b30, s.opponent_name);
      else
        strcpy(s.cue_text, gs_phasebar_your_upkeep_007a7c20);
      break;
    case 0xa:
      if (s.cue_hit_player == 1)
        sprintf(s.cue_text, gs_phasebar_opponent_draw_0091c490, s.opponent_name);
      else
        strcpy(s.cue_text, gs_phasebar_your_draw_0091b110);
      break;
    case 0x14:
      if (s.cue_hit_player == 1)
        sprintf(s.cue_text, gs_phasebar_opponent_main_precombat_00924790, s.opponent_name);
      else
        strcpy(s.cue_text, gs_phasebar_your_main_precombat_008b2840);
      break;
    case 0x15:
    case 0x16:
    case 0x17:
    case 0x18:
    case 0x19:
    case 0x1a:
    case 0x1b:
      if (s.cue_hit_player == 1)
        sprintf(s.cue_text, gs_phasebar_opponent_main_combat_00926380, s.opponent_name);
      else
        strcpy(s.cue_text, gs_phasebar_your_main_declare_combat_007a7cd0);
      break;
    case 0x1e:
      if (s.cue_hit_player == 1)
        sprintf(s.cue_text, gs_phasebar_opponent_main_postcombat_00925fa0, s.opponent_name);
      else
        strcpy(s.cue_text, gs_phasebar_your_main_postcombat_00926670);
      break;
    case 0x1f:
      if (s.cue_hit_player == 1)
        sprintf(s.cue_text, gs_phasebar_opponent_discard_00938e40, s.opponent_name);
      else
        strcpy(s.cue_text, gs_phasebar_your_discard_00939350);
      break;
    case 0x20:
    case 0x21:
    case 0x22:
    case 0x25:
      if (s.cue_hit_player == 1)
        sprintf(s.cue_text, gs_phasebar_opponent_cleanup_007aae40, s.opponent_name);
      else
        strcpy(s.cue_text, gs_phasebar_your_cleanup_00789b90);
      break;
    default:
      s.cue_valid = 0;
      break;
    }
    if (s.cue_valid != 0)
      strcpy((char *)wparam, s.cue_text);
    return s.cue_valid;

  case 0x432:
    s.cached_card = GetWindowLongA(hwnd, g_phase_display_selected_card_window_long_offset);
    s.cached_player = GetWindowLongA(hwnd, g_phase_display_selected_player_window_long_offset);
    get_current_duel_selection(&s.refresh_current_player, &s.refresh_current_card);
    if (s.cached_card != s.refresh_current_card || s.cached_player != s.refresh_current_player)
      InvalidateRect(hwnd, (RECT *)0, 0);
    return 0;

  case 0x400:
    noop_phase_display_user_message(hwnd, wparam, lparam);
    return 0;

  case WM_COMMAND:
    switch ((unsigned int)wparam & 0xffff)
    {
    case PHASE_DISPLAY_COMMAND_HELP:
      s.general_help_context = 0x7e5;
      strcpy(s.general_help_path, global_base_directory);
      strcat(s.general_help_path, s__duel_hlp_0057e4d8);
      WinHelpA(g_duel_window_hwnd, s.general_help_path, HELP_CONTEXT, s.general_help_context);
      break;

    default:
      s.command_phase = (unsigned int)wparam & 0xffff;
      if (s.command_phase >= PHASE_DISPLAY_COMMAND_HELP_BASE)
        s.command_base = PHASE_DISPLAY_COMMAND_HELP_BASE;
      else if (s.command_phase >= PHASE_DISPLAY_COMMAND_TOGGLE_BASE)
        s.command_base = PHASE_DISPLAY_COMMAND_TOGGLE_BASE;
      else
        s.command_base = PHASE_DISPLAY_COMMAND_STOP_BASE;

      s.command_phase -= s.command_base;
      if (s.command_phase >= PHASE_DISPLAY_PLAYER_COMMAND_OFFSET)
      {
        s.command_player = 1;
        s.command_phase -= PHASE_DISPLAY_PLAYER_COMMAND_OFFSET;
      }
      else
      {
        s.command_player = 0;
      }

      s.command_phase_index = s.command_phase;
      switch (s.command_base)
      {
      case PHASE_DISPLAY_COMMAND_STOP_BASE:
        if ((g_duel_network_flags & 2) != 0)
          break;
        s.stop_player = s.command_player;
        if (s.command_phase_index == 0)
          s.stop_phase_code = 1;
        else if (s.command_phase_index == 1)
          s.stop_phase_code = 4;
        else if (s.command_phase_index == 2)
          s.stop_phase_code = 0xa;
        else if (s.command_phase_index == 3)
          s.stop_phase_code = 0x14;
        else if (s.command_phase_index == 4)
        {
          if (s.stop_player == 0)
            s.stop_phase_code = 0x15;
          else
            s.stop_phase_code = 0x16;
        }
        else if (s.command_phase_index == 5)
          s.stop_phase_code = 0x1e;
        else if (s.command_phase_index == 6)
          s.stop_phase_code = 0x1f;
        else if (s.command_phase_index == 7)
          s.stop_phase_code = 0x20;
        unk_00716244 = s.stop_player;
        unk_00716248 = s.stop_phase_code;
        unk_00715fb0 = 0;
        g_phase_display_menu_packet[0] = -2;
        g_phase_display_menu_packet[1] = -1;
        g_phase_display_menu_packet[2] = -1;
        PostMessageA(g_duel_window_hwnd, 0x464, 0, (LPARAM)g_phase_display_menu_packet);
        break;

      case PHASE_DISPLAY_COMMAND_TOGGLE_BASE:
        s.toggle_player = s.command_player;
        if (s.command_phase_index == 3 && unk_008b35ec == s.toggle_player)
          break;
        if (s.command_phase_index == 0)
          s.toggle_phase_code = 1;
        else if (s.command_phase_index == 1)
          s.toggle_phase_code = 4;
        else if (s.command_phase_index == 2)
          s.toggle_phase_code = 0xa;
        else if (s.command_phase_index == 3)
          s.toggle_phase_code = 0x14;
        else if (s.command_phase_index == 4)
        {
          if (s.toggle_player == 0)
            s.toggle_phase_code = 0x15;
          else
            s.toggle_phase_code = 0x16;
        }
        else if (s.command_phase_index == 5)
          s.toggle_phase_code = 0x1e;
        else if (s.command_phase_index == 6)
          s.toggle_phase_code = 0x1f;
        else if (s.command_phase_index == 7)
          s.toggle_phase_code = 0x22;
        if (((int)(char)DAT_007abc90[s.toggle_player * 0x26 + s.toggle_phase_code] & 1) != 0)
          DAT_007abc90[s.toggle_player * 0x26 + s.toggle_phase_code] =
              (unsigned char)(((int)(char)DAT_007abc90[s.toggle_player * 0x26 + s.toggle_phase_code]) & 0xfe);
        else
          DAT_007abc90[s.toggle_player * 0x26 + s.toggle_phase_code] =
              (unsigned char)(((int)(char)DAT_007abc90[s.toggle_player * 0x26 + s.toggle_phase_code]) | 1);
        save_duel_interface_options_to_registry();
        refresh_duel_display_cache();
        GetClientRect(hwnd, &s.toggle_client_rect);
        get_phase_display_phase_rect(&s.toggle_phase_rect, s.toggle_player, s.toggle_phase_code,
                                     s.toggle_client_rect.right, s.toggle_client_rect.bottom);
        InvalidateRect(hwnd, &s.toggle_phase_rect, 0);
        break;

      case PHASE_DISPLAY_COMMAND_HELP_BASE:
        if (s.command_phase_index == 0)
          s.phase_help_context = 0x7da;
        else if (s.command_phase_index == 1)
          s.phase_help_context = 0x7db;
        else if (s.command_phase_index == 2)
          s.phase_help_context = 0x7dc;
        else if (s.command_phase_index == 3)
          s.phase_help_context = 0x7dd;
        else if (s.command_phase_index == 4)
          s.phase_help_context = 0x7dd;
        else if (s.command_phase_index == 5)
          s.phase_help_context = 0x7dd;
        else if (s.command_phase_index == 6)
          s.phase_help_context = 0x7de;
        else if (s.command_phase_index == 7)
          s.phase_help_context = 0x7df;
        strcpy(s.phase_help_path, global_base_directory);
        strcat(s.phase_help_path, s__duel_hlp_0057e4e4);
        WinHelpA(g_duel_window_hwnd, s.phase_help_path, HELP_CONTEXT, s.phase_help_context);
        break;
      }
      break;
    }
    return 0;

  case WM_CREATE:
    s.cached_card = 0;
    s.cached_player = 0;
    SetWindowLongA(hwnd, g_phase_display_selected_card_window_long_offset, s.cached_card);
    SetWindowLongA(hwnd, g_phase_display_selected_player_window_long_offset, s.cached_player);
    return 0;

  case WM_ERASEBKGND:
    return 1;

  case WM_LBUTTONDOWN:
    Sleep(GetDoubleClickTime());
    s.peek_result = PeekMessageA(&s.peek_message, hwnd, WM_LBUTTONDBLCLK, WM_LBUTTONDBLCLK, 0);
    s.click_point.x = lparam & 0xffff;
    s.click_point.y = HIWORD(lparam);
    GetClientRect(hwnd, &s.click_client_rect);
    hit_test_phase_display(&s.click_point, &s.click_client_rect, &s.click_hit_player, &s.click_hit_phase);
    if ((g_duel_network_flags & 2) == 0 && s.click_hit_phase != -1 && g_duel_modal_action_active != 0)
    {
      unk_00716244 = s.click_hit_player;
      unk_00716248 = s.click_hit_phase;
      unk_00715fb0 = s.peek_result;
      g_phase_display_click_packet[0] = -2;
      g_phase_display_click_packet[1] = -1;
      g_phase_display_click_packet[2] = -1;
      PostMessageA(g_duel_window_hwnd, 0x464, 0, (LPARAM)g_phase_display_click_packet);
    }
    return 0;

  case WM_PAINT:
    s.cached_card = GetWindowLongA(hwnd, g_phase_display_selected_card_window_long_offset);
    s.cached_player = GetWindowLongA(hwnd, g_phase_display_selected_player_window_long_offset);
    get_current_duel_selection(&s.paint_hit_player, &s.paint_hit_phase);
    if (s.paint_hit_phase != s.cached_card || s.paint_hit_player != s.cached_player)
      InvalidateRect(hwnd, (RECT *)0, 0);

    if (g_magicgame_phase_display_pic == (HANDLE)0)
    {
      strcpy(s.pic_path, global_duelart_path);
      strcat(s.pic_path, s__WINBK_Phase_pic_0057e4f0);
      g_magicgame_phase_display_pic = load_pic(s.pic_path);
    }

    EnterCriticalSection(&g_card_render_lock);
    s.paint_dc = g_shared_offscreen_dc;
    s.paint_saved_dc = SaveDC(g_shared_offscreen_dc);
    GetClientRect(hwnd, &s.paint_client_rect);
    if (g_magicgame_phase_display_pic != (HANDLE)0)
    {
      GetObjectA(g_magicgame_phase_display_pic, sizeof(BITMAP), &s.paint_bitmap);
      DrawBitmapSubrectToRect(s.paint_dc, &s.paint_client_rect, g_magicgame_phase_display_pic, 0, 0,
                              s.paint_bitmap.bmWidth / g_phase_display_bitmap_divisor,
                              s.paint_bitmap.bmHeight);
    }
    else
    {
      FillRect(s.paint_dc, &s.paint_client_rect, GetStockObject(4));
    }

    if (s.paint_hit_player != -1 && s.paint_hit_phase != -1)
    {
      get_phase_display_phase_rect(&s.paint_phase_rect, s.paint_hit_player, s.paint_hit_phase,
                                   s.paint_client_rect.right, s.paint_client_rect.bottom);
      s.paint_clip_region = CreateRectRgnIndirect(&s.paint_phase_rect);
      SelectClipRgn(s.paint_dc, s.paint_clip_region);
      if (g_magicgame_phase_display_pic != (HANDLE)0)
      {
        GetObjectA(g_magicgame_phase_display_pic, sizeof(BITMAP), &s.paint_bitmap);
        DrawBitmapSubrectToRect(s.paint_dc, &s.paint_client_rect, g_magicgame_phase_display_pic,
                                s.paint_bitmap.bmWidth -
                                    (s.paint_bitmap.bmWidth / g_phase_display_bitmap_divisor),
                                0, s.paint_bitmap.bmWidth / g_phase_display_bitmap_divisor,
                                s.paint_bitmap.bmHeight);
      }
      else
      {
        FillRect(s.paint_dc, &s.paint_client_rect, GetStockObject(2));
      }
      SelectClipRgn(s.paint_dc, (HRGN)0);
      DeleteObject(s.paint_clip_region);
    }

    RestoreDC(g_shared_offscreen_dc, s.paint_saved_dc);
    draw_phase_display_status_markers(s.paint_dc, &s.paint_client_rect);
    s.paint_dc = BeginPaint(hwnd, &s.paint);
    if (s.paint_dc != (HDC)0)
    {
      ApplyCardArtPaletteToDc(s.paint_dc);
      GetClientRect(hwnd, &s.paint_client_rect);
      if (g_duel_palette_refresh_pending != 0)
      {
        FillRect(s.paint_dc, &s.paint_client_rect, GetStockObject(WHITE_BRUSH));
        Sleep(200);
      }
      BitBlt(s.paint_dc, 0, 0, s.paint_client_rect.right, s.paint_client_rect.bottom,
             g_shared_offscreen_dc, 0, 0, SRCCOPY);
      get_phase_display_selected_half(&s.selected_half);
      if (s.selected_half != -1)
      {
        if (s.selected_half == 0)
          s.paint_client_rect.bottom -=
              (s.paint_client_rect.bottom - s.paint_client_rect.top) / 2;
        else
          s.paint_client_rect.top +=
              (s.paint_client_rect.bottom - s.paint_client_rect.top) / 2;
        SelectObject(s.paint_dc, g_phase_display_hatch_brush);
        SetBkMode(s.paint_dc, TRANSPARENT);
        Rectangle(s.paint_dc, s.paint_client_rect.left, s.paint_client_rect.top,
                  s.paint_client_rect.right, s.paint_client_rect.bottom);
      }
      EndPaint(hwnd, &s.paint);
      s.cached_card = s.paint_hit_phase;
      s.cached_player = s.paint_hit_player;
      SetWindowLongA(hwnd, g_phase_display_selected_card_window_long_offset, s.cached_card);
      SetWindowLongA(hwnd, g_phase_display_selected_player_window_long_offset, s.cached_player);
    }
    LeaveCriticalSection(&g_card_render_lock);
    return 0;

  case WM_RBUTTONDOWN:
    s.popup_point.x = lparam & 0xffff;
    s.popup_point.y = HIWORD(lparam);
    ClientToScreen(hwnd, &s.popup_point);
    SetRect(&s.popup_rect, s.popup_point.x, s.popup_point.y,
            s.popup_point.x + 1, s.popup_point.y + 1);
    TrackPopupMenu(g_phase_display_menu, TPM_RIGHTBUTTON, s.popup_point.x, s.popup_point.y,
                   0, hwnd, &s.popup_rect);
    return 0;

  case WM_INITMENU:
    GetCursorPos(&s.menu_cursor_point);
    ScreenToClient(hwnd, &s.menu_cursor_point);
    GetClientRect(hwnd, &s.menu_client_rect);
    hit_test_phase_display(&s.menu_cursor_point, &s.menu_client_rect,
                           &s.menu_hit_player, &s.menu_hit_phase);
    s.menu_player_offset = s.menu_hit_player == 1 ? PHASE_DISPLAY_PLAYER_COMMAND_OFFSET : 0;
    if (s.menu_hit_phase == 1)
      s.menu_phase_index = 0;
    else if (s.menu_hit_phase == 2 || s.menu_hit_phase == 3 ||
             s.menu_hit_phase == 4 || s.menu_hit_phase == 5)
      s.menu_phase_index = 1;
    else if (s.menu_hit_phase == 0xa)
      s.menu_phase_index = 2;
    else if (s.menu_hit_phase == 0x14)
      s.menu_phase_index = 3;
    else if (s.menu_hit_phase == 0x15 || s.menu_hit_phase == 0x16 ||
             s.menu_hit_phase == 0x17 || s.menu_hit_phase == 0x18 ||
             s.menu_hit_phase == 0x19 || s.menu_hit_phase == 0x1a ||
             s.menu_hit_phase == 0x1b)
      s.menu_phase_index = 4;
    else if (s.menu_hit_phase == 0x1e)
      s.menu_phase_index = 5;
    else if (s.menu_hit_phase == 0x1f)
      s.menu_phase_index = 6;
    else if (s.menu_hit_phase == 0x20 || s.menu_hit_phase == 0x21 ||
             s.menu_hit_phase == 0x22 || s.menu_hit_phase == 0x25)
      s.menu_phase_index = 7;
    else
      s.menu_phase_index = -1;
    if (s.menu_hit_phase != -1)
    {
      if (g_duel_modal_action_active != 0)
      {
        s.menu_stop_command =
            s.menu_phase_index + s.menu_player_offset + PHASE_DISPLAY_COMMAND_STOP_BASE;
        if ((g_duel_network_flags & 2) == 0)
          AppendMenuA(g_phase_display_menu, 0, s.menu_stop_command, g_phase_display_menu_stop_text);
      }
      if (GetMenuItemCount(g_phase_display_menu) != 0)
        AppendMenuA(g_phase_display_menu, MF_SEPARATOR, 0, (LPCSTR)0);
      s.menu_toggle_command =
          s.menu_phase_index + s.menu_player_offset + PHASE_DISPLAY_COMMAND_TOGGLE_BASE;
      AppendMenuA(g_phase_display_menu, 0, s.menu_toggle_command, g_phase_display_menu_toggle_text);
      copy_phase_stop_flags(s.menu_values, s.menu_hit_player);
      if (s.menu_values[s.menu_hit_phase] != 0)
        CheckMenuItem(g_phase_display_menu, s.menu_toggle_command, MF_CHECKED);
      s.menu_help_command =
          s.menu_phase_index + s.menu_player_offset + PHASE_DISPLAY_COMMAND_HELP_BASE;
      AppendMenuA(g_phase_display_menu, 0, s.menu_help_command, g_phase_display_menu_phase_help_text);
    }
    AppendMenuA(g_phase_display_menu, 0, PHASE_DISPLAY_COMMAND_HELP, g_phase_display_menu_help_text);
    return 0;

  case WM_MENUSELECT:
    if (HIWORD(wparam) == 0xffff && lparam == 0)
    {
      s.menu_count = GetMenuItemCount(g_phase_display_menu);
      s.menu_remaining = s.menu_count;
      --s.menu_count;
      while (s.menu_remaining != 0)
      {
        DeleteMenu(g_phase_display_menu, 0, MF_BYPOSITION);
        s.menu_remaining = s.menu_count;
        --s.menu_count;
      }
    }
    return 0;

  case WM_SETCURSOR:
    return handle_duel_inactive_cursor(hwnd, msg, wparam, lparam);

  case 0x30f:
  case 0x310:
  case 0x311:
    return FUN_10025b5e((int)hwnd, msg, (int)wparam, lparam);

  default:
    break;
  }

  return DefWindowProcA(hwnd, msg, wparam, lparam);
}
