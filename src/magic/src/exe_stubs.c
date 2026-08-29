#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <windows.h>

#include "game_support.h"
#include "global_strings.h"
#include "shared_startup.h"

// This file holds temporary stub implementations for exe functions that are
// referenced by decompiled code but not yet matched/decompiled elsewhere.

int dispatch_trigger_twice_once_with_each_player_as_reason(int reason_for_trig, trigger_t trig, const char *prompt, int a4);
int set_stack_damage_targets(void);
int raw_do_dialog(int bigcard_player,
                  int bigcard_card,
                  int smallcard_player,
                  int smallcard_card,
                  char *prompt,
                  int dialog_mode);
void FUN_00464bb0(void);
int reset_duel_tick_timer(void);
int reset_duel_tick_timer_indirect(void);
int get_duel_thread_time_ms(void);
void update_duel_thread_time_marker(void);
void StopWorldLocationMusic(void);
int activate(int who_activates, int player, int card);
int can_activate_mana_source_for_stop_prompt(int player, int card);
int activate_mana_source_card(int player, int card);
int resolve_activated_ability(int player, int card);
void prompt_player_for_stop_action(int player, char *prompt);
void show_mana_burn_dialog(int player, int amount);
BOOL CALLBACK dlgproc_mana_burn(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
BOOL CALLBACK dlgproc_prompt_for_life_total(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
void delete_and_close_object(HANDLE obj);
HBITMAP load_pic(char *filename);
void ApplyCardArtPaletteToDc(HDC hdc);
BOOL DrawBitmapToRect(HDC dst_dc, const RECT *dst_rect, HBITMAP bitmap);
extern int g_ai_response_state;
extern int g_reveal_all_world_info;
void __cdecl _assert(void *expr, void *filename, unsigned line);
void change_buttonclass_wndproc(HWND hwnd);
void save_duel_interface_options_to_registry(void);
void FUN_0044a796(int player, int card);
int card_ball_lightning(int player, int card, event_t event);
void draw_owner_draw_button_centered(DRAWITEMSTRUCT *draw_item_struct,
                                     HBRUSH brush,
                                     HPEN pen1,
                                     HPEN pen2,
                                     COLORREF color,
                                     int draw_focus);

// GLOBAL: MAGIC 0x00637f2c
// GLOBAL: SHANDALAR 0x005aa460
static HBRUSH g_duel_options_dialog_button_brush;
// GLOBAL: MAGIC 0x00637f18
// GLOBAL: SHANDALAR 0x005aa44c
static HPEN g_duel_options_dialog_pen1;
// GLOBAL: MAGIC 0x00637f28
// GLOBAL: SHANDALAR 0x005aa45c
static HPEN g_duel_options_dialog_pen2;
// GLOBAL: MAGIC 0x00637f24
// GLOBAL: SHANDALAR 0x005aa458
static COLORREF g_duel_options_dialog_focus_text_color;
// GLOBAL: MAGIC 0x00637f34
// GLOBAL: SHANDALAR 0x005aa468
static COLORREF g_duel_options_dialog_unfocus_text_color;
// GLOBAL: MAGIC 0x00637f30
// GLOBAL: SHANDALAR 0x005aa464
static COLORREF g_duel_options_dialog_text_color;
// GLOBAL: MAGIC 0x00637f1c
// GLOBAL: SHANDALAR 0x005aa450
static COLORREF g_duel_options_dialog_title_color;
// GLOBAL: MAGIC 0x00637f20
// GLOBAL: SHANDALAR 0x005aa454
static HBITMAP g_duel_options_dialog_background;

// GLOBAL: MAGIC 0x00638c8c
static HBITMAP g_life_total_dialog_background;
// GLOBAL: MAGIC 0x00638c14
static COLORREF g_life_total_dialog_text_color;
// GLOBAL: MAGIC 0x00638b50
static HBRUSH g_life_total_dialog_button_brush;
// GLOBAL: MAGIC 0x00638bec
static HPEN g_life_total_dialog_pen1;
// GLOBAL: MAGIC 0x00638bd8
static HPEN g_life_total_dialog_pen2;
// GLOBAL: MAGIC 0x00638b58
static COLORREF g_life_total_dialog_unfocus_text_color;
// GLOBAL: MAGIC 0x00638c2c
static COLORREF g_life_total_dialog_focus_text_color;
// GLOBAL: MAGIC 0x0077651c
static WNDPROC g_life_total_edit_wndproc;

static void setup_life_total_dialog_resources(HBITMAP *background,
                                              COLORREF *text_color,
                                              HBRUSH *button_brush,
                                              HPEN *pen1,
                                              HPEN *pen2,
                                              COLORREF *unfocus_text_color,
                                              COLORREF *focus_text_color);
static void cleanup_life_total_dialog_resources(HBITMAP background,
                                                HBRUSH button_brush,
                                                HPEN pen1,
                                                HPEN pen2);
static LRESULT CALLBACK life_total_edit_wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

typedef struct life_total_dialog_context_struct
{
  char *prompt;
  int initial_life_total;
  int show_reset_button;
} life_total_dialog_context_t;
STATIC_ASSERT(sizeof(life_total_dialog_context_t) == 0xc, life_total_dialog_context_wrong_size);

// FUNCTION: MAGIC 0x00489f9f
// FUNCTION: SHANDALAR 0x004a5c2f
static void setup_duel_options_dialog_resources(HBITMAP *background,
                                                COLORREF *title_color,
                                                COLORREF *text_color,
                                                HBRUSH *button_brush,
                                                HPEN *pen1,
                                                HPEN *pen2,
                                                COLORREF *unfocus_text_color,
                                                COLORREF *focus_text_color)
{
  char path[264];

  sprintf(path, "%s\\WINBK_Options.pic", global_duelart_path);
  *background = load_pic(path);
  *title_color = 0x1000007;
  *text_color = 0x1000001;
  *button_brush = CreateSolidBrush(0x1000036);
  *pen1 = CreatePen(PS_SOLID, 0, 0x10000ba);
  *pen2 = CreatePen(PS_SOLID, 0, 0x1000016);
  *unfocus_text_color = 0x1000001;
  *focus_text_color = 0x10000bf;
  if (*button_brush == (HBRUSH)0)
  {
    *button_brush = GetStockObject(2);
  }
  if (*pen1 == (HPEN)0)
  {
    *pen1 = GetStockObject(6);
  }
  if (*pen2 == (HPEN)0)
  {
    *pen2 = GetStockObject(7);
  }
}

// FUNCTION: MAGIC 0x0048a085
// FUNCTION: SHANDALAR 0x004a5d15
static void cleanup_duel_options_dialog_resources(HBITMAP background,
                                                  HBRUSH button_brush,
                                                  HPEN pen1,
                                                  HPEN pen2)
{
  if (background != (HBITMAP)0)
  {
    delete_and_close_object(background);
  }
  if (button_brush != (HBRUSH)0)
  {
    DeleteObject(button_brush);
  }
  if (pen1 != (HPEN)0)
  {
    DeleteObject(pen1);
  }
  if (pen2 != (HPEN)0)
  {
    DeleteObject(pen2);
  }
}

// FUNCTION: MAGIC 0x0048958a
// FUNCTION: SHANDALAR 0x004a521a
BOOL CALLBACK dlgproc_duel_interface_options(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  struct
  {
    HDC erase_hdc;
    RECT rect;
    COLORREF text_color;
    DRAWITEMSTRUCT *draw_item;
    HWND ctl_hwnd;
    int ctl_id;
    HDC ctl_hdc;
    HBRUSH brush;
    int previous_control;
    int current_control;
    int selected_control;
  } s;

  switch (msg)
  {
  case WM_INITDIALOG:
    setup_duel_options_dialog_resources(&g_duel_options_dialog_background,
                                        &g_duel_options_dialog_title_color,
                                        &g_duel_options_dialog_text_color,
                                        &g_duel_options_dialog_button_brush,
                                        &g_duel_options_dialog_pen1,
                                        &g_duel_options_dialog_pen2,
                                        &g_duel_options_dialog_unfocus_text_color,
                                        &g_duel_options_dialog_focus_text_color);

    SetDlgItemTextA(hwnd, 0x424, gs_duel_option_008b35f0[0]);
    SetDlgItemTextA(hwnd, 0x425, gs_duel_option_008b35f0[1]);
    SetDlgItemTextA(hwnd, 0x426, gs_duel_option_008b35f0[2]);
    SetDlgItemTextA(hwnd, 0x427, gs_duel_option_008b35f0[3]);
    SetDlgItemTextA(hwnd, 0x438, gs_duel_option_008b35f0[4]);
    SetDlgItemTextA(hwnd, 0x429, gs_duel_option_008b35f0[5]);
    SetDlgItemTextA(hwnd, 0x42b, gs_duel_option_008b35f0[6]);
    SetDlgItemTextA(hwnd, 0x42a, gs_duel_option_008b35f0[7]);
    SetDlgItemTextA(hwnd, 0x437, gs_duel_option_008b35f0[8]);
    SetDlgItemTextA(hwnd, 0x433, gs_duel_option_008b35f0[9]);
    SetDlgItemTextA(hwnd, 0x42d, gs_duel_option_008b35f0[10]);
    SetDlgItemTextA(hwnd, 0x42e, gs_duel_option_008b35f0[11]);
    SetDlgItemTextA(hwnd, 0x42f, gs_duel_option_008b35f0[12]);
    SetDlgItemTextA(hwnd, 0x430, gs_duel_option_008b35f0[13]);
    SetDlgItemTextA(hwnd, 0x431, gs_duel_option_008b35f0[14]);
    SetDlgItemTextA(hwnd, 0x432, gs_duel_option_008b35f0[15]);
    SetDlgItemTextA(hwnd, 0x434, gs_duel_option_008b35f0[16]);
    SetDlgItemTextA(hwnd, 0x435, gs_duel_option_008b35f0[17]);
    SetDlgItemTextA(hwnd, 0x436, gs_duel_option_008b35f0[18]);
    SetDlgItemTextA(hwnd, IDOK, gs_ok_00924800);
    SetDlgItemTextA(hwnd, IDCANCEL, gs_cancel_008a8c20);

    LoadDuelInterfaceRegistryOptions();

    if (g_duel_interface_options.layout == 1)
    {
      s.selected_control = 0x426;
    }
    else
    {
      s.selected_control = 0x427;
    }
    CheckDlgButton(hwnd, s.selected_control, 1);
    CheckRadioButton(hwnd, 0x426, 0x427, s.selected_control);

    if (g_duel_interface_options.show_coin_flips != 0)
    {
      CheckDlgButton(hwnd, 0x438, 1);
    }
    if (g_duel_interface_options.show_cue_cards != 0)
    {
      CheckDlgButton(hwnd, 0x429, 1);
    }
    if (g_duel_interface_options.show_power_toughness_on_cards != 0)
    {
      CheckDlgButton(hwnd, 0x42a, 1);
    }
    if (g_duel_interface_options.show_abilities_on_cards != 0)
    {
      CheckDlgButton(hwnd, 0x42b, 1);
    }
    if (g_duel_interface_options.see_next_draws_at_end_of_duel != 0)
    {
      CheckDlgButton(hwnd, 0x437, 1);
    }
    if ((g_duel_mode_flags & 1) == 0)
    {
      EnableWindow(GetDlgItem(hwnd, 0x437), 0);
      CheckDlgButton(hwnd, 0x437, 1);
    }

    if (g_duel_interface_options.player_territory_color == 1)
    {
      s.selected_control = 0x42f;
    }
    else if (g_duel_interface_options.player_territory_color == 2)
    {
      s.selected_control = 0x42e;
    }
    else if (g_duel_interface_options.player_territory_color == 3)
    {
      s.selected_control = 0x431;
    }
    else if (g_duel_interface_options.player_territory_color == 5)
    {
      s.selected_control = 0x42d;
    }
    else if (g_duel_interface_options.player_territory_color == 4)
    {
      s.selected_control = 0x430;
    }
    else
    {
      s.selected_control = 0x432;
    }
    CheckDlgButton(hwnd, s.selected_control, 1);
    CheckRadioButton(hwnd, 0x42d, 0x432, s.selected_control);

    if (g_duel_interface_options.player_territory_type == 0)
    {
      s.selected_control = 0x434;
    }
    else if (g_duel_interface_options.player_territory_type == 1)
    {
      s.selected_control = 0x435;
    }
    else
    {
      s.selected_control = 0x436;
    }
    CheckDlgButton(hwnd, s.selected_control, 1);
    CheckRadioButton(hwnd, 0x434, 0x436, s.selected_control);

    SetFocus(GetDlgItem(hwnd, IDOK));
    SendMessageA(hwnd, 0x401, 1, 0);
    change_buttonclass_wndproc(hwnd);
    return 0;

  case WM_COMMAND:
    if ((wparam & 0xffff) == IDCANCEL)
    {
      cleanup_duel_options_dialog_resources(g_duel_options_dialog_background,
                                            g_duel_options_dialog_button_brush,
                                            g_duel_options_dialog_pen1,
                                            g_duel_options_dialog_pen2);
      EndDialog(hwnd, 0);
    }
    else if ((wparam & 0xffff) == IDOK)
    {
      if (IsDlgButtonChecked(hwnd, 0x426) != 0)
      {
        g_duel_interface_options.layout = 1;
      }
      else
      {
        g_duel_interface_options.layout = 2;
      }
      g_duel_interface_options.show_coin_flips = IsDlgButtonChecked(hwnd, 0x438);
      g_duel_interface_options.show_cue_cards = IsDlgButtonChecked(hwnd, 0x429);
      g_duel_interface_options.show_power_toughness_on_cards = IsDlgButtonChecked(hwnd, 0x42a);
      g_duel_interface_options.show_abilities_on_cards = IsDlgButtonChecked(hwnd, 0x42b);
      g_duel_interface_options.see_next_draws_at_end_of_duel = IsDlgButtonChecked(hwnd, 0x437);

      if (IsDlgButtonChecked(hwnd, 0x42f) != 0)
      {
        g_duel_interface_options.player_territory_color = 1;
      }
      else if (IsDlgButtonChecked(hwnd, 0x42e) != 0)
      {
        g_duel_interface_options.player_territory_color = 2;
      }
      else if (IsDlgButtonChecked(hwnd, 0x42d) != 0)
      {
        g_duel_interface_options.player_territory_color = 5;
      }
      else if (IsDlgButtonChecked(hwnd, 0x431) != 0)
      {
        g_duel_interface_options.player_territory_color = 3;
      }
      else if (IsDlgButtonChecked(hwnd, 0x430) != 0)
      {
        g_duel_interface_options.player_territory_color = 4;
      }
      else
      {
        g_duel_interface_options.player_territory_color = -1;
      }

      if (IsDlgButtonChecked(hwnd, 0x434) != 0)
      {
        g_duel_interface_options.player_territory_type = 0;
      }
      else if (IsDlgButtonChecked(hwnd, 0x435) != 0)
      {
        g_duel_interface_options.player_territory_type = 1;
      }
      else
      {
        g_duel_interface_options.player_territory_type = 2;
      }

      save_duel_interface_options_to_registry();
      cleanup_duel_options_dialog_resources(g_duel_options_dialog_background,
                                            g_duel_options_dialog_button_brush,
                                            g_duel_options_dialog_pen1,
                                            g_duel_options_dialog_pen2);
      EndDialog(hwnd, 1);
    }

    return 1;

  case 0x4c8:
    s.current_control = (int)wparam;
    s.previous_control = (int)lparam;
    if (GetDlgItem(hwnd, IDCANCEL) == (HWND)s.current_control)
    {
      SendMessageA(hwnd, 0x401, IDCANCEL, 0);
    }
    else
    {
      SendMessageA(hwnd, 0x401, IDOK, 0);
    }
    if (s.current_control != 0)
    {
      InvalidateRect((HWND)s.current_control, NULL, 1);
    }
    if (s.previous_control != 0)
    {
      InvalidateRect((HWND)s.previous_control, NULL, 1);
    }
    return 0;

  case WM_CTLCOLORBTN:
  case WM_CTLCOLORSTATIC:
    s.ctl_hdc = (HDC)wparam;
    ApplyCardArtPaletteToDc(s.ctl_hdc);
    s.ctl_hwnd = (HWND)lparam;
    s.ctl_id = GetDlgCtrlID(s.ctl_hwnd);
    if (s.ctl_id == 0x425 || s.ctl_id == 0x433)
    {
      SetTextColor(s.ctl_hdc, g_duel_options_dialog_text_color);
      SetBkMode(s.ctl_hdc, TRANSPARENT);
      s.brush = g_duel_options_dialog_button_brush;
    }
    else if (s.ctl_id == 0x424)
    {
      SetTextColor(s.ctl_hdc, g_duel_options_dialog_title_color);
      SetBkMode(s.ctl_hdc, TRANSPARENT);
      s.brush = GetStockObject(NULL_BRUSH);
    }
    else
    {
      if (GetFocus() == s.ctl_hwnd)
      {
        SetTextColor(s.ctl_hdc, g_duel_options_dialog_focus_text_color);
      }
      else
      {
        SetTextColor(s.ctl_hdc, g_duel_options_dialog_text_color);
      }
      SetBkMode(s.ctl_hdc, TRANSPARENT);
      s.brush = GetStockObject(NULL_BRUSH);
    }
    return (BOOL)s.brush;

  case WM_DRAWITEM:
    s.draw_item = (DRAWITEMSTRUCT *)lparam;
    if (GetFocus() == s.draw_item->hwndItem)
    {
      s.text_color = g_duel_options_dialog_focus_text_color;
    }
    else
    {
      s.text_color = g_duel_options_dialog_unfocus_text_color;
    }
    draw_owner_draw_button_centered(s.draw_item,
                                    g_duel_options_dialog_button_brush,
                                    g_duel_options_dialog_pen1,
                                    g_duel_options_dialog_pen2,
                                    s.text_color,
                                    0);
    return 1;

  case WM_QUERYNEWPALETTE:
  case WM_PALETTEISCHANGING:
  case WM_PALETTECHANGED:
    return handle_button_palette_message((int)hwnd, msg, (int)wparam, lparam);

  case WM_ERASEBKGND:
    s.erase_hdc = (HDC)wparam;
    ApplyCardArtPaletteToDc(s.erase_hdc);
    GetClientRect(hwnd, &s.rect);
    if (g_duel_options_dialog_background != (HBITMAP)0)
    {
      DrawBitmapToRect(s.erase_hdc, &s.rect, g_duel_options_dialog_background);
    }
    else
    {
      FillRect(s.erase_hdc, &s.rect, GetStockObject(2));
    }
    return 1;

  case WM_LBUTTONDOWN:
    SendMessageA(hwnd, WM_SYSCOMMAND, 0xf012, 0);
    return 0;

  default:
    return 0;
  }
}

// NOTE: Even though this file is named "exe_stubs.c", functions removed from
// here as they get fully decompiled elsewhere. A few UI helpers live here
// temporarily until their proper home is decided.

// FUNCTION: MAGIC 0x00444d1f
// FUNCTION: SHANDALAR 0x004105b7
void reset_trigger_dispatch_state(void)
{
  g_trigger_dispatch_depth = g_max_trigger_dispatch_depth = 0;
  g_regenerate_or_graveyard_triggers_in_progress = 0;
}

// FUNCTION: MAGIC 0x0044af2d
// FUNCTION: SHANDALAR 0x004038a8
int prompt_stop_phase_anyway(phase_t phase)
{
  if (g_duel_ai_mode_state != 1 &&
      (((int)(char)g_duel_phase_stop_settings[g_current_player].phase_flags[phase] & (PHASE_STOP_ENABLED | PHASE_STOP_OPPONENT)) != 0 ||
       (phase == g_stop_phase && g_current_player == g_stop_phase_player) ||
       (phase == g_previous_stop_phase && g_current_player == g_previous_stop_phase_player)))
  {
    load_text(global_ui_strings_filename, "PROMPT_STOPANYWAY");

    if (phase == 4)
    {
      strcpy(g_prompt_stop_phase_anyway_text, g_text_lines[1]);
    }
    else if (phase == 1)
    {
      strcpy(g_prompt_stop_phase_anyway_text, g_text_lines[2]);
    }
    else if (phase == 10)
    {
      strcpy(g_prompt_stop_phase_anyway_text, g_text_lines[3]);
    }
    else if (phase == 0x14)
    {
      strcpy(g_prompt_stop_phase_anyway_text, g_text_lines[4]);
    }
    else if (phase == 0x1f)
    {
      strcpy(g_prompt_stop_phase_anyway_text, g_text_lines[5]);
    }
    else if (phase == 0x22)
    {
      strcpy(g_prompt_stop_phase_anyway_text, g_text_lines[6]);
    }
    else if (phase == 0x19)
    {
      strcpy(g_prompt_stop_phase_anyway_text, g_text_lines[7]);
    }
    else if (phase == 0x1a)
    {
      strcpy(g_prompt_stop_phase_anyway_text, g_text_lines[8]);
    }
    else if (phase == 0x1b)
    {
      strcpy(g_prompt_stop_phase_anyway_text, g_text_lines[8]);
    }
    else
    {
      strcpy(g_prompt_stop_phase_anyway_text, g_text_lines[0]);
    }

    if ((g_duel_network_flags & 2) != 0)
    {
      if (g_active_player == g_current_player)
      {
        if (((int)(char)g_duel_phase_stop_settings[g_current_player].phase_flags[phase] & PHASE_STOP_ENABLED) != 0 ||
            (phase == g_stop_phase && g_current_player == g_stop_phase_player))
        {
          prompt_player_for_stop_action(g_active_player, g_prompt_stop_phase_anyway_text);
        }
        if (((int)(char)g_duel_phase_stop_settings[g_current_player].phase_flags[phase] & PHASE_STOP_OPPONENT) != 0 ||
            (phase == g_previous_stop_phase && g_current_player == g_previous_stop_phase_player))
        {
          prompt_player_for_stop_action(g_other_player, g_prompt_stop_phase_anyway_text);
        }
      }
      else
      {
        if (((int)(char)g_duel_phase_stop_settings[g_current_player].phase_flags[phase] & PHASE_STOP_OPPONENT) != 0 ||
            (phase == g_previous_stop_phase && g_current_player == g_previous_stop_phase_player))
        {
          prompt_player_for_stop_action(g_other_player, g_prompt_stop_phase_anyway_text);
        }
        if (((int)(char)g_duel_phase_stop_settings[g_current_player].phase_flags[phase] & PHASE_STOP_ENABLED) != 0 ||
            (phase == g_stop_phase && g_current_player == g_stop_phase_player))
        {
          prompt_player_for_stop_action(g_active_player, g_prompt_stop_phase_anyway_text);
        }
      }
    }
    else
    {
      prompt_player_for_stop_action(g_active_player, g_prompt_stop_phase_anyway_text);
    }
  }

  return -1;
}

// FUNCTION: MAGIC 0x0044b2b3
// FUNCTION: SHANDALAR 0x00403c34
void prompt_player_for_stop_action(int player, char *prompt)
{
  struct
  {
    int selected_card;
    char prompt_copy[100];
    int done;
  } s;

  strcpy(s.prompt_copy, prompt);
  s.done = 0;

  while (s.done == 0)
  {
    if (g_active_player == g_current_player)
    {
      g_stop_phase = g_stop_phase_player = -1;
    }
    else
    {
      g_previous_stop_phase = g_previous_stop_phase_player = -1;
    }

    s.selected_card = select_card_for_action(player, player, player, 0xff, 0, s.prompt_copy, 2);
    switch (g_target_selection_status_code)
    {
    case 0:
      if (s.selected_card == -1)
      {
      }
      else
      {
        if (can_activate_mana_source_for_stop_prompt(g_target_player_choice, s.selected_card) != 0)
        {
          activate_mana_source_card(g_target_player_choice, s.selected_card);
        }
        else
        {
        }
      }
      break;

    case -2:
      s.done = 1;
      break;

    case -3:
      break;
    }
  }
}

// FUNCTION: MAGIC 0x0044b3d4
// FUNCTION: SHANDALAR 0x00403d55
void resolve_mana_burn(void)
{
  int current_card;
  int player;

  if ((g_duel_network_flags & 2) == 0)
  {
  loop:
    if (g_raw_mana_available[g_other_player][7] != 0 && g_current_phase > 0x1a)
    {
      for (current_card = 0;
           current_card < g_active_cards_count[g_other_player];
           current_card++)
      {
        if (is_in_play(g_other_player, current_card) != 0)
        {
          dispatch_event_to_single_card(g_other_player, current_card, 0x8f, 1 - g_other_player, -1);
          if (g_last_single_card_event_result != 0)
          {
            if (activate(g_other_player, g_other_player, current_card) != 0)
            {
              resolve_activated_ability(g_other_player, current_card);
            }
            goto loop;
          }
        }
      }
    }
  }

  for (player = 0; player < 2; player = player + 1)
  {
    if (g_raw_mana_available[player][7] > 0)
    {
      if (g_duel_ai_mode_state != 1)
      {
        play_sound_effect(0x2a);
        show_mana_burn_dialog(player, g_raw_mana_available[player][7]);
      }
      g_life[player] = g_life[player] - g_raw_mana_available[player][7];
      for (current_card = 0; current_card <= 7; current_card = current_card + 1)
      {
        g_raw_mana_available[player][current_card] = 0;
      }
    }
  }
}

// FUNCTION: MAGIC 0x0044b646
// FUNCTION: SHANDALAR 0x00403fc7
int contains_player_card_pair(int *card_pairs, int card_pair_count, int player, int card)
{
  struct
  {
    int card_pair_index;
    int result;
  } s;

  s.result = 0;
  for (s.card_pair_index = 0; s.card_pair_index < card_pair_count; s.card_pair_index = s.card_pair_index + 1)
  {
    if ((card_pairs[s.card_pair_index * 2] == player) &&
        (card_pairs[s.card_pair_index * 2 + 1] == card))
    {
      s.result = 1;
    }
  }
  return s.result;
}

// FUNCTION: MAGIC 0x00444e5f
// FUNCTION: SHANDALAR 0x004106f7
int allow_response_once(int response_player, int phase, char *prompt, int event_code)
{
  /* Stack layout is sensitive here; keep locals in one struct to match /Od codegen. */
  struct
  {
    int saved_DAT_007aadec;          /* ebp-0xa4 */
    int saved_unk_008b3270;          /* ebp-0xa0 */
    unsigned int dispatch_result;    /* ebp-0x9c */
    int result;                      /* ebp-0x98 */
    int saved_DAT_00777aa8_in_loop;  /* ebp-0x94 */
    int saved_DAT_00777aa8;          /* ebp-0x90 */
    int saved_phase_stop_suppressed; /* ebp-0x8c */
    char prompt_copy[128];           /* ebp-0x88 */
    int saved_trigger_condition;     /* ebp-0x8 */
    int saved_DAT_00561390;          /* ebp-0x4 */
  } s;

  s.saved_trigger_condition = g_trigger_condition;
  s.saved_DAT_007aadec = g_current_action_event_code;
  g_trigger_condition = -1;
  s.saved_unk_008b3270 = g_activation_event_flags;

  ++g_trigger_dispatch_depth;
  if (g_trigger_dispatch_depth == 1)
  {
    g_max_trigger_dispatch_depth = 0;
  }
  else if (g_max_trigger_dispatch_depth < g_trigger_dispatch_depth && phase != 0x8e && phase != 0x70 && phase != 0xd3)
  {
    g_max_trigger_dispatch_depth = g_trigger_dispatch_depth;
  }

  s.result = 0;
  g_current_action_event_code = event_code;
  s.saved_DAT_00777aa8 = g_ai_decision_parameter;
  s.saved_DAT_00561390 = g_affected_card_context_flags;

  if (response_player == -2 && should_skip_phase(g_current_player) == 0)
  {
    g_affected_card_context_flags = 1;
  }
  else
  {
    g_affected_card_context_flags = 0;
  }

  if ((g_duel_network_flags & 2) == 0)
  {
    if (should_skip_phase(g_current_player) == 0 &&
        ((((int)(char)g_duel_phase_stop_settings[g_current_player].phase_flags[phase] & PHASE_STOP_ENABLED) != 0) ||
         (g_current_player == g_stop_phase_player && g_stop_phase == phase)))
    {
      g_ai_decision_parameter = 1;
    }
    else
    {
      g_ai_decision_parameter = 0;
    }
  }

  if (response_player > -1)
  {
    if (g_ai_decision_parameter == 0)
    {
      goto cleanup;
    }
  }

  strcpy(s.prompt_copy, prompt);

  if (g_current_phase == PHASE_UPKEEP && g_trigger_dispatch_depth == 1)
  {
    g_event_player = g_current_player;
    reset_upkeep_costs();
  }

  while (1)
  {
  restart_response_window:
    do
    {
      g_ai_decision_parameter = can_stop_for_phase(1);

      if ((g_duel_network_flags & 2) != 0)
      {
        g_response_processing_flags = 1;
      }
      else
      {
        if (g_current_player != 0)
        {
          if (response_player > -1)
          {
            g_response_processing_flags = 0;
          }
          else
          {
            g_response_processing_flags = 2;
          }
        }
        else
        {
          g_response_processing_flags = 1;
        }
      }

      if (g_trigger_dispatch_depth <= 1)
      {
        g_response_action_mask = 0xffffffff;
      }

      g_spell_fizzled = 0;
      g_activation_event_flags = g_spell_fizzled;

      s.dispatch_result = (unsigned int)process_response_actions(g_current_player, s.prompt_copy);
      if (s.dispatch_result != 0)
      {
        g_response_action_taken = 1;
      }

      if (((g_active_player == g_current_player) ||
           (g_other_player == g_current_player && (g_duel_network_flags & 2) != 0)) &&
          s.dispatch_result != 0 && g_duel_ai_mode_state != 1)
      {
        s.result = 1;
      }

      if (g_trigger_dispatch_depth < g_max_trigger_dispatch_depth && g_stack_size >= 0)
      {
        s.dispatch_result = 0;
      }

    } while (s.dispatch_result != 0 || ((g_activation_event_flags & 1) != 0 && g_trigger_dispatch_depth == 1));

    if (g_current_phase == PHASE_UPKEEP && g_trigger_dispatch_depth == 1)
    {
      g_event_player = 1 - g_current_player;
      reset_upkeep_costs();
    }

    while (1)
    {
      s.saved_DAT_00777aa8_in_loop = g_ai_decision_parameter;
      s.saved_phase_stop_suppressed = g_phase_stop_suppressed;

      if ((g_duel_network_flags & 2) != 0)
      {
        g_response_processing_flags = 1;
        g_phase_stop_suppressed = 0;

        if (g_stack_size == 0 && should_skip_phase(1 - g_current_player) != 0 && response_player != -2)
        {
          g_phase_stop_suppressed = 1;
        }

        g_ai_decision_parameter = can_stop_for_phase(0);
      }
      else
      {
        if (g_current_player != 0)
        {
          g_response_processing_flags = 1;
        }
        else
        {
          if (response_player > -1)
          {
            g_response_processing_flags = 0;
          }
          else
          {
            g_response_processing_flags = 2;
          }
        }
      }

      if (g_trigger_dispatch_depth <= 1)
      {
        g_response_action_mask = 0xffffffff;
      }

      g_spell_fizzled = 0;
      g_activation_event_flags = g_spell_fizzled;

      if ((g_trigger_dispatch_depth < g_max_trigger_dispatch_depth && g_stack_size >= 0))
      {
        goto cleanup;
      }

      s.dispatch_result = (unsigned int)process_response_actions(1 - g_current_player, s.prompt_copy);
      g_ai_decision_parameter = s.saved_DAT_00777aa8_in_loop;
      g_phase_stop_suppressed = s.saved_phase_stop_suppressed;
      if (s.dispatch_result != 0)
      {
        g_response_action_taken = 1;

        if (g_duel_ai_mode_state != 1)
        {
          goto restart_response_window;
        }

        if (g_active_player == g_current_player)
        {
          goto restart_response_window;
        }
        if ((g_duel_network_flags & 2) != 0)
        {
          goto restart_response_window;
        }
        if ((g_activation_event_flags & 1) != 0)
        {
          if (g_trigger_dispatch_depth == 1)
          {
            goto restart_response_window;
          }
        }
        goto cleanup;
      }
      else
      {
        goto cleanup;
      }
    }
  }

cleanup:
  --g_trigger_dispatch_depth;
  if (g_trigger_dispatch_depth != 0)
  {
    g_response_action_mask = 0xffffffff;
  }
  else
  {
    g_response_card_type_mask = 0xffffffff;
    g_response_action_mask = g_response_card_type_mask;
    if (g_nested_trigger_depth == 0)
    {
      g_response_window_serial = 0;
      if (g_duel_ai_mode_state != 1)
      {
        g_phase_stop_suppressed = 0;
      }
    }
  }

  g_ai_decision_parameter = s.saved_DAT_00777aa8;
  g_affected_card_context_flags = s.saved_DAT_00561390;
  g_activation_event_flags = s.saved_unk_008b3270;
  g_trigger_condition = s.saved_trigger_condition;
  g_current_action_event_code = s.saved_DAT_007aadec;

  if (s.result != 0)
  {
    g_ai_action_replay_available = 0;
  }

  return s.result;
}

// FUNCTION: MAGIC 0x004af6f4
// FUNCTION: SHANDALAR 0x0046c6cf
int is_phase_stop_allowed_for_current_event(void)
{
  if (g_current_action_event_code == 0x8e)
  {
    if (g_attacking_creature_count != 0)
      return 1;
    else
      return 0;
  }
  else if (g_current_action_event_code == 0x6a || g_current_action_event_code == 0x6b || g_current_action_event_code == 0x6c ||
           g_current_action_event_code == 0x6d || g_current_action_event_code == 0x6e || g_current_action_event_code == 0x6f ||
           g_current_action_event_code == 0x70 || g_current_action_event_code == 0x71 || g_current_action_event_code == 0x72 ||
           g_current_action_event_code == 0x73 || g_current_action_event_code == 0x74 || g_current_action_event_code == 0x75 ||
           g_current_action_event_code == 0x76 || g_current_action_event_code == 0x77 || g_current_action_event_code == 0x78 ||
           g_current_action_event_code == 0x79 || g_current_action_event_code == 0x7a || g_current_action_event_code == 0x7b ||
           g_current_action_event_code == 0x7c || g_current_action_event_code == 0x7d || g_current_action_event_code == 0x7e ||
           g_current_action_event_code == 0x7f || g_current_action_event_code == 0x80 || g_current_action_event_code == 0x81 ||
           g_current_action_event_code == 0x82 || g_current_action_event_code == 0x83 || g_current_action_event_code == 0x84 ||
           g_current_action_event_code == 0x85 || g_current_action_event_code == 0x86 || g_current_action_event_code == 0x87 ||
           g_current_action_event_code == 0x88 || g_current_action_event_code == 0x89 || g_current_action_event_code == 0x8e ||
           g_current_action_event_code == 0xc7 || g_current_action_event_code == 0xc8 || g_current_action_event_code == 0xc9 ||
           g_current_action_event_code == 0xca || g_current_action_event_code == 0xcb || g_current_action_event_code == 0xcc ||
           g_current_action_event_code == 0xcd || g_current_action_event_code == 0xce || g_current_action_event_code == 0xcf ||
           g_current_action_event_code == 0xd2 || g_current_action_event_code == 0xd3 || g_current_action_event_code == 0xd4 ||
           g_current_action_event_code == 0xd5 || g_current_action_event_code == 0xd6 || g_current_action_event_code == 0xd7 ||
           g_current_action_event_code == 0xd8 || g_current_action_event_code == 0xd9 || g_current_action_event_code == 0xdc ||
           g_current_action_event_code == 0xdb)
  {
    return 0;
  }
  else
  {
    return 1;
  }
}

// FUNCTION: MAGIC 0x0044ac96
// FUNCTION: SHANDALAR 0x00403615
int player_can_stop_at_phase(int player, phase_t phase)
{
  struct
  {
    phase_t mapped_phase; /* ebp-0x8 */
    int result;           /* ebp-0x4 */
  } s;

  s.result = 0;

  if (g_duel_ai_mode_state == 1)
  {
    return 0;
  }

  if (g_phase_stop_suppressed != 0)
  {
    return 0;
  }

  if (is_phase_stop_allowed_for_current_event() == 0)
  {
    return 0;
  }

  if (g_phase_response_window_open == 0)
  {
    return 0;
  }

  if (phase >= PHASE_BEGIN_UPKEEP && phase <= PHASE_UNKOWN5)
  {
    s.mapped_phase = PHASE_UPKEEP;
  }
  else if (phase >= PHASE_CLEANUP2 && phase <= PHASE_DAMAGE_PREVENTION)
  {
    s.mapped_phase = PHASE_CLEANUP2;
  }
  else
  {
    s.mapped_phase = phase;
  }

  if ((g_duel_network_flags & 2) != 0)
  {
    if (g_current_player == player)
    {
      if ((g_active_player == player &&
           (((int)(char)g_duel_phase_stop_settings[g_active_player].phase_flags[g_current_phase]) & PHASE_STOP_ENABLED) != 0) ||
          (g_other_player == player &&
           (((int)(char)g_duel_phase_stop_settings[g_other_player].phase_flags[g_current_phase]) & PHASE_STOP_OPPONENT) != 0))
      {
        s.result = 1;
      }
    }
    else
    {
      if ((g_other_player == player &&
           (((int)(char)g_duel_phase_stop_settings[g_active_player].phase_flags[g_current_phase]) & PHASE_STOP_OPPONENT) != 0) ||
          (g_active_player == player &&
           (((int)(char)g_duel_phase_stop_settings[g_other_player].phase_flags[g_current_phase]) & PHASE_STOP_ENABLED) != 0))
      {
        s.result = 1;
      }
    }
  }
  else
  {
    if (g_stop_phase == s.mapped_phase)
    {
      s.result = 1;
    }

    if (g_stop_phase == -1 && g_active_player == player &&
        (((int)(char)g_duel_phase_stop_settings[g_current_player].phase_flags[s.mapped_phase]) & PHASE_STOP_ENABLED) != 0)
    {
      s.result = 1;
    }
  }

  return s.result;
}

// FUNCTION: MAGIC 0x004e51a6
// FUNCTION: SHANDALAR 0x004c4415
int get_recorded_action_count(void)
{
  return g_recorded_action_count;
}

// FUNCTION: MAGIC 0x004aec05
// FUNCTION: SHANDALAR 0x0046bbe0
unsigned int choose_response_card(int player)
{
  struct
  {
    unsigned int candidates[20];
    int response_flags;   /* local_18 */
    unsigned int card;    /* local_14 */
    int other_player;     /* local_10 */
    int internal_card_id; /* local_c */
    int num_candidates;   /* local_8 */
  } s;

  s.num_candidates = 0;

  if (g_trigger_condition != -1 && g_current_turn == g_other_player)
  {
    for (s.other_player = 0; s.other_player < 2; ++s.other_player)
    {
      for (s.card = 0; (int)s.card < g_active_cards_count[s.other_player]; ++s.card)
      {
        if (is_in_play(s.other_player, (int)s.card) &&
            dispatch_card_trigger_event(s.other_player, (int)s.card, 0x7d, player) == 2)
        {
          g_activation_dispatch_flags = 4;
          g_target_player_choice = s.other_player;
          return s.card;
        }

        if (player == s.other_player &&
            (int)PLAYER_CARD_INSTANCE(s.other_player, (int)s.card).unknown0x14 == g_trigger_condition &&
            g_current_turn == s.other_player && g_trigger_condition != -1 &&
            PLAYER_CARD_INSTANCE(s.other_player, (int)s.card).internal_card_id != -1)
        {
          g_activation_dispatch_flags |= 4;
          ++g_duel_state_current_card;
          g_target_player_choice = s.other_player;
          return s.card;
        }
      }
    }
  }

  if (((g_response_processing_flags & 2) == 0) || (player == g_active_player) || (g_response_action_mask == 0))
  {
    return 0xffffffffU;
  }

  g_target_player_choice = player;

  for (s.card = 0; (int)s.card < g_active_cards_count[player]; ++s.card)
  {
    s.internal_card_id = PLAYER_CARD_INSTANCE(player, (int)s.card).internal_card_id;
    if (s.internal_card_id == -1)
      continue;

    s.response_flags = (int)get_card_response_action_type(player, (int)s.card);
    if (s.response_flags != 0)
    {
      s.candidates[s.num_candidates] = s.card;
      ++s.num_candidates;

      if (s.response_flags == 2)
      {
        return s.card;
      }
    }
  }

  if (g_current_action_event_code == 4)
  {
    for (s.card = 0; (int)s.card < g_active_cards_count[1 - player]; ++s.card)
    {
      s.internal_card_id = PLAYER_CARD_INSTANCE(1 - player, (int)s.card).internal_card_id;
      if (s.internal_card_id == -1)
        continue;

      s.response_flags = (int)get_card_response_action_type(1 - player, (int)s.card);
      if (s.response_flags != 0 && s.response_flags == 2)
      {
        g_target_player_choice = 1 - player;
        return s.card;
      }
    }
  }

  if (g_current_action_event_code == 4)
  {
    return 0xffffffffU;
  }

  s.candidates[s.num_candidates] = 0xffffffffU;
  ++s.num_candidates;

  if (g_duel_ai_mode_state == 1)
  {
    if (internal_rand(2) != 0 && get_recorded_action_count() != 0)
    {
      g_ai_recorded_choice = s.num_candidates - 1;
    }
    else
    {
      g_ai_recorded_choice = internal_rand(s.num_candidates);
    }

    if (g_ai_search_force_pass != 0)
    {
      g_ai_recorded_choice = s.num_candidates - 1;
      if (g_ai_search_force_pass == 1)
      {
        g_ai_search_force_pass = -1;
      }
    }

    g_ai_recorded_action = ((PLAYER_CARD_INSTANCE(player, (int)s.candidates[g_ai_recorded_choice]).state & STATE_IN_PLAY) ? 0x2000 : 0x1000) | s.candidates[g_ai_recorded_choice] | ((player != 0) ? 0x100 : 0);

    g_ai_recorded_action_type = 4;
    record_ai_action_selection();
  }
  else
  {
    g_ai_recorded_action_type = 4;
    replay_ai_action_selection();

    if (s.num_candidates <= g_ai_recorded_choice)
    {
      g_ai_recorded_choice = s.num_candidates - 1;
    }
  }

  if (s.candidates[g_ai_recorded_choice] != 0xffffffffU)
  {
    if (g_duel_state_008a8de4 >= 0x10)
    {
      --g_duel_state_008a8de4;
    }

    g_nested_trigger_stack[g_duel_state_008a8de4] =
        PLAYER_CARD_INSTANCE(player, (int)s.candidates[g_ai_recorded_choice]).internal_card_id;
    g_ai_action_score_table[g_duel_state_008a8de4] = s.candidates[g_ai_recorded_choice];
    ++g_duel_state_008a8de4;
  }

  return s.candidates[g_ai_recorded_choice];
}

// FUNCTION: MAGIC 0x00440ad2
// FUNCTION: SHANDALAR 0x0040c352
int resolve_card_immediately(int player, int card, int arg_3)
{
  struct
  {
    int trace_counter;       /* iVar2 / local_324 */
    char dialog_prompt[300]; /* local_320 */
    char trace_text[500];    /* local_1f4 */
  } s;

  if ((g_duel_network_flags & 2) != 0)
  {
    s.trace_counter = g_duel_trace_counter;
    ++g_duel_trace_counter;
    sprintf(s.trace_text, "%d: Player #%d is processing %s(%d).\n",
            s.trace_counter, player,
            global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].name,
            card);
    append_to_trace_txt(s.trace_text);
  }

  g_ai_search_result = 1;
  push_card_onto_stack(player, card, 0x7e, arg_3, 0);

  if (g_spell_fizzled == 1)
  {
    obliterate_top_card_of_stack();
    g_ai_search_result = 0;
    return 0;
  }

  if (g_duel_ai_mode_state != 1)
  {
    set_stack_damage_targets();
    if (g_current_turn != g_active_player)
    {
      LoadTextSectionLines(global_ui_strings_filename, "PROMPT_PROC1");
      sprintf(s.dialog_prompt, g_text_lines[0], g_duel_message_text);
      raw_do_dialog(player, card, -1, -1, s.dialog_prompt, 0);
    }
  }

  PLAYER_CARD_INSTANCE(player, card).state |= STATE_PROCESSING;
  resolve_top_card_on_stack();

  g_ai_search_result = 0;
}

// FUNCTION: MAGIC 0x0044096f
// FUNCTION: SHANDALAR 0x0040c1ef
int resolve_activated_ability(int player, int card)
{
  int internal_card_id;
  int has_mana_source_and_color;
  int response_player;
  int saved_trigger_cause_controller;
  int saved_trigger_cause;

  internal_card_id = PLAYER_CARD_INSTANCE(player, card).internal_card_id;
  has_mana_source_and_color =
      ((global_cards_data[internal_card_id].extra_ability & EA_MANA_SOURCE) != 0 && g_produced_mana_color != -1);

  if (g_spell_fizzled == 1)
  {
    return 1;
  }

  if (!has_mana_source_and_color && g_upkeep_payment_completed == 0)
  {
    sprintf(g_ui_message_buffer, gs_activate_0091b2d0, get_displayed_card_name(player, card));

    if (g_phase_stop_suppressed != 0 && (g_active_player == player || (g_duel_network_flags & 2) != 0))
    {
      response_player = -1;
    }
    else
    {
      response_player = -2;
    }

    allow_response(response_player, g_current_phase, g_ui_message_buffer, 0x6d);
  }

  resolve_top_card_on_stack();

  saved_trigger_cause_controller = g_trigger_cause_controller;
  saved_trigger_cause = g_trigger_cause;

  g_trigger_cause_controller = player;
  g_trigger_cause = card;
  dispatch_trigger_twice_once_with_each_player_as_reason(g_current_player, 0xd2, gs_tapping_00925d40, 0);

  g_trigger_cause_controller = saved_trigger_cause_controller;
  g_trigger_cause = saved_trigger_cause;

  return 1;
}

// FUNCTION: MAGIC 0x00440c61
// FUNCTION: SHANDALAR 0x0040c4e1
int is_card_pending_resolution(int player, int card)
{
  if (PLAYER_CARD_INSTANCE(player, card).internal_card_id == -1)
  {
    return 0;
  }

  return ((PLAYER_CARD_INSTANCE(player, card).state & (STATE_IN_PLAY | STATE_INVISIBLE)) != 0) ? 0 : 1;
}

// FUNCTION: MAGIC 0x00442cf9
// FUNCTION: SHANDALAR 0x0040e593
int is_nonactivated_mana_source(int player, int card)
{
  int result;

  if ((global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].extra_ability & EA_MANA_SOURCE) != 0)
  {
    if ((global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].extra_ability & EA_ACT_ABILITY) != 0)
    {
      result = 0;
    }
    else
    {
      result = 1;
    }
  }
  else
  {
    result = 0;
  }

  return result;
}

// FUNCTION: MAGIC 0x004af0ec
// FUNCTION: SHANDALAR 0x0046c0c8
int get_card_response_action_type(int player, int card)
{
  struct
  {
    int tmp;                 /* ebp-0x0c */
    int response_controller; /* ebp-0x08 */
    int internal_card_id;    /* ebp-0x04 */
  } s;

  if ((g_duel_network_flags & 2) != 0)
  {
    s.response_controller = player;
  }
  else
  {
    s.response_controller = g_active_player;
  }

  s.internal_card_id = PLAYER_CARD_INSTANCE(player, card).internal_card_id;

  if ((PLAYER_CARD_INSTANCE(player, card).state & STATE_IN_PLAY) != 0)
  {
    if ((PLAYER_CARD_INSTANCE(player, card).state & STATE_OUBLIETTED) != 0)
    {
      return 0;
    }

    if ((int)PLAYER_CARD_INSTANCE(player, card).unknown0x14 == g_trigger_condition)
    {
      if (g_trigger_condition != -1)
      {
        if (g_current_turn == player)
        {
          g_activation_dispatch_flags |= 4;
          g_duel_state_current_card += 1;
          return 2;
        }
        else
        {
          return 0;
        }
      }
    }

    if (g_trigger_condition != -1)
    {
      s.tmp = dispatch_card_trigger_event(player, card, 0x7d, player);
      if (s.tmp != 0)
      {
        g_activation_dispatch_flags |= 1 << ((char)s.tmp);
        g_duel_state_current_card += 1;

        if (s.tmp >= 2)
        {
          return 2;
        }
        else
        {
          return 3;
        }
      }
      return 0;
    }

    if (((PLAYER_CARD_INSTANCE(player, card).upkeep_flags & 1) == 0 &&
         (global_cards_data[s.internal_card_id].extra_ability & 1) != 0 &&
         (g_response_card_type_mask & 0x10) != 0) ||
        ((global_cards_data[s.internal_card_id].extra_ability & 2) != 0 &&
         (g_response_card_type_mask & 0x20) != 0))
    {
      if (g_duel_state_00791418 != 0 &&
          (global_cards_data[s.internal_card_id].extra_ability & 2) == 0)
      {
        return 0;
      }

      if ((g_land_can_be_played & 4) == 0 ||
          ((g_land_can_be_played & 4) != 0 &&
           (global_cards_data[s.internal_card_id].extra_ability & 0x1004) != 0) ||
          (g_current_action_event_code == 0xd3 &&
           (global_cards_data[s.internal_card_id].extra_ability & 0x4000) != 0))
      {
        g_activation_event_flags &= ~2;

        if (((s.response_controller == player ||
              (g_response_source_phase_mask & (int)(char)global_cards_data[s.internal_card_id].act_phases) != 0) &&
             (PLAYER_CARD_INSTANCE(player, card).state & STATE_INVISIBLE) == 0) &&
            dispatch_event_to_single_card(player, card, 0x73, 1 - player, -1) != 0)
        {

          if ((g_activation_event_flags & 2U) != 0)
          {
            g_activation_dispatch_flags |= 4;
            return 2;
          }

          g_activation_dispatch_flags |= 2;
          return 3;
        }
      }
    }

    if (g_current_action_event_code == 4 && (PLAYER_CARD_INSTANCE(player, card).upkeep_flags & 1) != 0)
    {
      g_activation_event_flags |= 3;
      g_activation_dispatch_flags |= 4;
      return 2;
    }

    if (g_current_action_event_code == 4 && player == g_event_player &&
        (PLAYER_CARD_INSTANCE(player, card).upkeep_flags & 0x10) != 0 &&
        (PLAYER_CARD_INSTANCE(player, card).upkeep_flags & 0x88) == 0 &&
        can_pay_untap_cost(player, card) != 0)
    {
      g_activation_dispatch_flags |= 2;

      if (g_other_player == player && (g_duel_network_flags & 2) == 0)
      {
        return 2;
      }
      else
      {
        return 3;
      }
    }
  }
  else
  {
    if ((PLAYER_CARD_INSTANCE(player, card).state & (STATE_INVISIBLE | STATE_SUMMONSICK)) != 0)
    {
      return 0;
    }

    if (g_trigger_condition != -1 &&
        global_cards_data[s.internal_card_id].code_pointer != card_death_ward)
    {
      return 0;
    }

    if (g_duel_state_00791418 != 0 && ((global_cards_data[s.internal_card_id].type & 0x20) == 0))
    {
      return 0;
    }

    if ((g_response_card_type_mask & (unsigned char)global_cards_data[s.internal_card_id].type) != 0 &&
        can_pay_card_mana_cost(player, player, card) != 0 &&
        ((g_land_can_be_played & 4) == 0 ||
         ((g_land_can_be_played & 4) != 0 &&
          (global_cards_data[s.internal_card_id].extra_ability & 0x1004) != 0) ||
         (g_current_action_event_code == 0xd3 &&
          (global_cards_data[s.internal_card_id].extra_ability & 0x2000) != 0)) &&
        ((s.response_controller == player ||
          (g_response_activation_phase_mask & (int)(char)global_cards_data[s.internal_card_id].act_phases) != 0) &&
         dispatch_event_to_single_card(player, card, 0x74, 1 - player, -1) != 0))
    {
      return 3;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0044b6af
// FUNCTION: SHANDALAR 0x00404030
int can_activate_mana_source_for_stop_prompt(int player, int card)
{
  if (is_in_play(player, card) != 0 &&
      (global_cards_data[(global_card_instances[player])[card].internal_card_id].extra_ability & EA_MANA_SOURCE) != 0 &&
      ((global_card_instances[player])[card].state & STATE_TAPPED) == 0 &&
      (((global_card_instances[player])[card].state & STATE_SUMMONSICK_BOTH) == 0 ||
       (global_cards_data[(global_card_instances[player])[card].internal_card_id].type & TYPE_CREATURE) == 0))
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

// FUNCTION: MAGIC 0x0044b7d4
// FUNCTION: SHANDALAR 0x00404155
int activate_mana_source_card(int player, int card)
{
  g_required_mana_color_mask = 0x3e;
  g_produced_mana_color = -1;

  dispatch_event_to_single_card(player, card, 0x6d, 1 - player, -1);

  if (g_spell_fizzled != 1)
  {
    if ((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) != 0)
    {
      dispatch_event(player, card, 0x81);
    }

    process_damage_prevention(player);
    regenerate_or_graveyard_triggers();
  }
  else
  {
    g_spell_fizzled = 0;
  }

  g_required_mana_color_mask = 0;
  return g_produced_mana_color;
}

// FUNCTION: MAGIC 0x00445f61
// FUNCTION: SHANDALAR 0x004117fa
int card_activation_uses_x(int player, int card)
{
  int internal_card_id;

  if (player == -1 || card == -1)
  {
    return 0;
  }

  internal_card_id = PLAYER_CARD_INSTANCE(player, card).internal_card_id;
  if (g_stack_proxy_internal_card_id == internal_card_id)
  {
    internal_card_id = PLAYER_CARD_INSTANCE(player, card).original_internal_card_id;
  }

  if (internal_card_id == -1)
  {
    return 0;
  }

  if ((global_cards_data[internal_card_id].extra_ability & EA_ACT_USE_X) != 0)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

// FUNCTION: MAGIC 0x0043ff32
// FUNCTION: SHANDALAR 0x0040b7b2
int activate(int who_activates, int player, int card)
{
  struct
  {
    int upkeep_uses_mana_source;
    int untap_color;
    int upkeep_total;
    char upkeep_prompt[600];
    int has_upkeep_costs;
    int i;
    int upkeep_dialog_ai_choice;
    char upkeep_opt0[300];
    char upkeep_opt1[300];
    card_instance_t *instance;
    char prompt[300];
    int result;
    int who_pays; /* also who chooses in do_dialog */
    int was_tapped;
    char trace[500];
  } s;

  s.result = 1;

  s.instance = &PLAYER_CARD_INSTANCE(player, card);
  s.who_pays = player;

  if ((g_duel_network_flags & 2) != 0)
  {
    int trace_counter;

    trace_counter = g_duel_trace_counter;
    ++g_duel_trace_counter;
    sprintf(s.trace,
            "%d: Player #%d is tapping %s(%d).\n",
            trace_counter,
            player,
            global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].name,
            card);
    append_to_trace_txt(s.trace);
  }
  if (g_current_phase == EVENT_UPKEEP_PHASE)
  {
    s.who_pays = g_event_player;
  }
  if (s.who_pays == g_other_player && (g_duel_network_flags & 2) == 0 && g_duel_ai_mode_state != 1)
  {
    g_ai_recorded_action = -1;
    dispatch_event_to_single_card(player, card, EVENT_GET_SELECTED_CARD, 1 - player, -1);

    LoadTextSectionLines(global_ui_strings_filename, "PROMPT_TAP1");
    sprintf(s.prompt, g_text_lines[0], g_duel_message_text);

    if (card_activation_uses_x(player, card) != 0)
    {
      sprintf(s.prompt, g_text_lines[1], g_duel_message_text, g_loaded_recorded_action_code);
    }
    else if (((global_cards_data[s.instance->internal_card_id].extra_ability & (EA_INF_POWER | EA_INF_TOUGHNESS)) != 0) &&
             g_current_player == player && g_loaded_recorded_action_code != 0)
    {
      sprintf(s.prompt, g_text_lines[2], g_duel_message_text, g_loaded_recorded_action_code);
    }

    if (s.instance->number_of_targets != 0)
    {
      if (s.instance->number_of_targets == 1)
      {
        raw_do_dialog(player, card, s.instance->targets[0].player, s.instance->targets[0].card, s.prompt, 0);
      }
      else
      {
        raw_do_dialog(player, card, -1, -1, s.prompt, 0);
      }
    }
    else
    {
      if (g_ai_recorded_action != -1)
      {
        /* Unreachable in practice, but present in the original binary. */
        raw_do_dialog(player, card, (g_ai_recorded_action >> 8), (g_ai_recorded_action & 0xff), s.prompt, 0);
      }
      else
      {
        raw_do_dialog(player, card, -1, -1, s.prompt, 0);
      }
    }
  }

  push_card_onto_stack(player, card, EVENT_RESOLVE_ACTIVATION, player, 0);
  g_upkeep_payment_completed = 0;

  if ((s.instance->upkeep_flags & UPKEEP_UPKEEP_TRIGGER) != 0)
  {
    s.has_upkeep_costs = 1;
    s.upkeep_total = 0;

    for (s.i = 0; s.i < 7; s.i++)
    {
      g_mana_charge[s.i] = ((char *)&s.instance->upkeep_colorless)[s.i];

      if (s.i > 0)
      {
        if (has_mana(player, (color_t)s.i, g_mana_charge[s.i]) == 0)
        {
          s.has_upkeep_costs = 0;
        }
      }

      s.upkeep_total += g_mana_charge[s.i];
    }

    if (has_mana(s.who_pays, COLOR_ANY, s.upkeep_total) == 0)
    {
      s.has_upkeep_costs = 0;
    }

    if (s.has_upkeep_costs != 0)
      s.upkeep_dialog_ai_choice = 0;
    else
      s.upkeep_dialog_ai_choice = 1;

    load_text(global_ui_strings_filename, "PROMPT_PAYUPKEEP");
    strcpy(s.upkeep_opt0, g_text_lines[0]);
    strcpy(s.upkeep_opt1, g_text_lines[1]);
    sprintf(s.upkeep_prompt, " %s\n %s", s.upkeep_opt0, s.upkeep_opt1);

    if ((s.instance->upkeep_flags & UPKEEP_UPKEEP_NODIALOG) != 0 ||
        do_dialog(s.who_pays, player, card, -1, -1, s.upkeep_prompt, s.upkeep_dialog_ai_choice) == 0)
    {
      if (s.has_upkeep_costs == 0)
      {
        goto upkeep_cleanup;
      }

      charge_mana(s.who_pays, 0, 0);
      if (g_spell_fizzled == 0)
      {
        dispatch_event_to_single_card(player, card, EVENT_UPKEEP_PHASE, 1 - player, -1);
        if (g_last_single_card_event_result == 0)
        {
          s.instance->upkeep_flags |= UPKEEP_UPKEEP_UNPAID;
        }
      }
    }

    if (g_spell_fizzled != 0)
    {
      g_spell_fizzled = 0;
      obliterate_top_card_of_stack();
      return 0;
    }

  upkeep_cleanup:
    for (s.i = 0; s.i < 7; s.i++)
    {
      g_mana_charge[s.i] = 0;
    }

    s.instance->upkeep_flags &= ~UPKEEP_UPKEEP_TRIGGER;
    s.instance->upkeep_flags |= UPKEEP_UPKEEP_CANTPAY;

    refresh_stack_proxy_card(1);
    return 1;
  }

  if ((s.instance->upkeep_flags & UPKEEP_UNTAP_TRIGGER) != 0 && can_pay_untap_cost(player, card) != 0)
  {
    for (s.untap_color = 0; s.untap_color < 7; ++s.untap_color)
    {
      g_mana_charge[s.untap_color] = (int)(char)s.instance->mana_to_untap[s.untap_color];
    }

    charge_mana(player, 0, 0);
    if (g_spell_fizzled == 0)
    {
      dispatch_event_to_single_card(player, card, EVENT_UNTAP_PHASE, 1 - player, -1);
      if (g_last_single_card_event_result == 0)
      {
        s.instance->upkeep_flags |= UPKEEP_UNTAP_PAID;
      }
    }

    if (g_spell_fizzled != 0)
    {
      g_spell_fizzled = 0;
      obliterate_top_card_of_stack();
      return 0;
    }

    s.instance->upkeep_flags &= ~UPKEEP_UNTAP_TRIGGER;
    s.instance->upkeep_flags |= UPKEEP_UNTAP_DONE;
    refresh_stack_proxy_card(1);
    return 1;
  }

  if (dispatch_event(player, card, EVENT_UNKNOWN80) != 0)
  {
    obliterate_top_card_of_stack();
    s.result = 0;
  }
  else
  {
    if ((global_cards_data[s.instance->internal_card_id].extra_ability & EA_MANA_SOURCE) != 0)
    {
      g_produced_mana_color = -1;
    }

    begin_mana_payment_record();

    s.was_tapped = s.instance->state & 0x10;
    dispatch_event_to_single_card(player, card, EVENT_ACTIVATE, 1 - player, -1);

    if ((g_duel_network_flags & 2) != 0 && g_other_player == who_activates && g_battlefield_ui_flags == 0 &&
        s.instance->internal_card_id != g_draw_card_placeholder_internal_card_id)
    {
      LoadTextSectionLines(global_ui_strings_filename, "PROMPT_TAP1");
      sprintf(s.prompt, g_text_lines[0], g_duel_message_text);

      if (card_activation_uses_x(player, card) != 0)
      {
        sprintf(s.prompt, g_text_lines[1], g_duel_message_text, g_x_value);
      }
      else if (((global_cards_data[s.instance->internal_card_id].extra_ability & (EA_INF_POWER | EA_INF_TOUGHNESS)) != 0) &&
               g_current_player == player && g_x_value != 0)
      {
        sprintf(s.prompt, g_text_lines[2], g_duel_message_text, g_x_value);
      }

      if (s.instance->number_of_targets == 1)
      {
        raw_do_dialog(player, card, s.instance->targets[0].player, s.instance->targets[0].card, s.prompt, 0);
      }
      else
      {
        raw_do_dialog(player, card, -1, -1, s.prompt, 0);
      }
    }

    if (g_spell_fizzled == 1)
    {
      restore_recorded_mana_to_pool(player);
      obliterate_top_card_of_stack();
      s.result = 0;
    }

    end_mana_payment_record();

    if (s.result != 0)
    {
      if (s.was_tapped == 0 && (s.instance->state & 0x10) != 0)
      {
        dispatch_event(player, card, EVENT_TAP_CARD);
      }
      else
      {
        dispatch_event(player, card, EVENT_PLAY_ABILITY);
      }

      if (s.instance->internal_card_id != -1)
      {
        s.upkeep_uses_mana_source = global_cards_data[s.instance->internal_card_id].extra_ability & EA_MANA_SOURCE;
      }
      else
      {
        s.upkeep_uses_mana_source = global_cards_data[s.instance->original_internal_card_id].extra_ability & EA_MANA_SOURCE;
      }

      if (s.upkeep_uses_mana_source != 0 && g_produced_mana_color != -1)
      {
        refresh_stack_proxy_card(0);
      }
      else
      {
        refresh_stack_proxy_card(1);
      }

      if (g_duel_ai_mode_state != 1)
      {
        if ((global_cards_data[s.instance->internal_card_id].extra_ability & EA_MANA_SOURCE) != 0 &&
            g_produced_mana_color != -1)
        {
          play_sound_effect(0x12);
        }
        else
        {
          play_sound_effect(0x26);
        }
      }

      if (g_duel_ai_mode_state != 1)
      {
        set_stack_damage_targets();
      }
    }
  }

  if (s.result == 0)
  {
    g_upkeep_payment_completed = 0;
  }

  return s.result;
}

// FUNCTION: MAGIC 0x00446cb8
// FUNCTION: SHANDALAR 0x00450836
void show_opponent_taunt_text_stub(char *text)
{
  RECT rect;
  int x;
  int y;

  if (0)
  {
    if (text == NULL || text[0] == '\0')
    {
      return;
    }

    play_sound_effect(0x25);

    SetWindowTextA(global_opponent_chat_hwnd, text);
    GetWindowRect(global_opponent_chat_hwnd, &rect);

    SetWindowPos(global_opponent_chat_hwnd,
                 (HWND)0,
                 rect.left,
                 -(rect.bottom - rect.top),
                 0,
                 0,
                 5);
    ShowWindow(global_opponent_chat_hwnd, 5);
    BringWindowToTop(global_opponent_chat_hwnd);

    x = rect.left;

    for (y = -(rect.bottom - rect.top); y < rect.top; ++y)
    {
      SetWindowPos(global_opponent_chat_hwnd, (HWND)0, x, y, 0, 0, 5);
      UpdateWindow(global_opponent_chat_hwnd);
      Sleep(0x19);
    }
  }
}

#ifndef SHANDALAR

// FUNCTION: MAGIC 0x0050aa68
void HideMouseCursorNested(void)
{
  // This space intentionally left blank (empty function)
}

// FUNCTION: MAGIC 0x0050aa5d
void ShowMouseCursorNested(void)
{
  // This space intentionally left blank (empty function)
}

#endif

// FUNCTION: MAGIC 0x004b5fc9
// FUNCTION: SHANDALAR 0x0056c7cb
void show_opponent_taunt(char *text)
{
  if (g_duel_ai_mode_state == 1)
    return;

  HideMouseCursorNested();

  show_opponent_taunt_text_stub(text);
#ifndef MODERN_FIXES
  // TODO: why the fuck did they do this??
  text[0] = '\0';
#endif

  ShowMouseCursorNested();
}

// FUNCTION: MAGIC 0x00444d48
// FUNCTION: SHANDALAR 0x004105e0
int allow_response(int response_player, int phase, char *prompt, int event_code)
{
  int saved_allow_response_flag = g_phase_stop_suppressed;

  if (g_stack_size == 0)
  {
    if (should_skip_phase(g_current_player) != 0 && response_player != -2 && g_duel_ai_mode_state != 1)
    {
      g_phase_stop_suppressed = 1;
    }
  }

  {
    int result;

    do
    {
      g_response_action_taken = 0;
      result = allow_response_once(response_player, phase, prompt, event_code);
    } while (g_response_action_taken != 0 && g_stack_size <= 0);

    if (g_stack_size == 0)
    {
      if (g_duel_ai_mode_state != 1)
      {
        g_phase_stop_suppressed = 0;
      }
      {
        g_duel_phase_stop_settings[g_current_player].phase_flags[g_current_phase] = (int)(char)g_duel_phase_stop_settings[g_current_player].phase_flags[g_current_phase] & ~PHASE_STOP_SUPPRESSED;
      }
    }
    else if (g_duel_ai_mode_state != 1)
    {
      g_phase_stop_suppressed = saved_allow_response_flag;
    }

    return result;
  }
}

// FUNCTION: MAGIC 0x004ad7e0
// FUNCTION: SHANDALAR 0x0046a7c0
int process_response_actions(int reason_for_trigger_controller, const char *prompt)
{
  struct
  {
    int selection_done;           // local_29c
    char prompt_format[300];      // local_29c
    unsigned int selected_card;   // local_170
    int saved_unk_00742f60;       // local_16c
    unsigned int maybe_card;      // local_168
    int some_flag;                // local_164
    int response_code;            // local_160
    unsigned int card;            // local_15c
    int saved_land_can_be_played; // local_158
    unsigned int response_mask;   // local_154
    int player;                   // local_150
    unsigned int result_flags;    // local_14c
    char prompt_copy[300];        // local_148
    int internal_card_id;         // local_1c
    int interrupts_available;     // local_18
    int selected_player;          // local_14
    int can_respond;              // local_10
    int maybe_player;             // local_c
    int num_candidates;           // local_8
  } s;

  if (g_life[g_active_player] <= 0)
  {
    g_duel_phase_stop_settings[g_current_player].phase_flags[g_current_phase] =
        (unsigned char)(((int)(char)g_duel_phase_stop_settings[g_current_player].phase_flags[g_current_phase]) | PHASE_STOP_SUPPRESSED);
  }

  s.interrupts_available = 0;
  if (g_duel_ai_mode_state != 1 && g_stop_phase_player == g_current_player && g_current_phase == g_stop_phase)
  {
    g_phase_stop_suppressed = 0;
  }

  s.can_respond = player_can_stop_at_phase(reason_for_trigger_controller, g_current_phase);

  strcpy(s.prompt_copy, prompt);

  s.saved_land_can_be_played = g_land_can_be_played;
  s.saved_unk_00742f60 = g_event_player;
  s.selected_player = reason_for_trigger_controller;
  g_event_player = s.selected_player;
  s.player = g_event_player;

  g_response_selection_in_progress = 1;
  g_response_card_type_mask = g_response_action_mask & 0x30;
  g_response_action_selected = 0;

  ++g_response_window_depth;
  if (g_response_window_serial > g_response_window_depth)
  {
    g_response_window_serial = 0;
  }

  if ((g_duel_network_flags & 2) == 0)
  {
    g_response_activation_phase_mask = 2;
    g_response_source_phase_mask = 0x20;

    if (g_current_phase >= PHASE_DECLARE_BLOCKERS)
    {
      g_response_activation_phase_mask = 4;
      g_response_source_phase_mask = 0x40;
    }
    if (g_current_phase <= PHASE_MAIN1)
    {
      g_response_activation_phase_mask = 1;
      g_response_source_phase_mask = 0x10;
    }
    if (g_current_phase >= PHASE_MAIN2)
    {
      g_response_activation_phase_mask = 8;
      g_response_source_phase_mask = (int)0xffffff80;
    }
    if (g_current_phase == PHASE_DISCARD)
    {
      g_response_activation_phase_mask = 0xf;
      g_response_source_phase_mask = (int)0xfffffff0;
    }
    if (g_other_player == reason_for_trigger_controller && global_stack_cards[0].player == g_active_player)
    {
      g_response_activation_phase_mask = 0xf;
      g_response_source_phase_mask = (int)0xfffffff0;
    }
  }
  else
  {
    g_response_activation_phase_mask = 0xf;
    g_response_source_phase_mask = (int)0xfffffff0;
  }

  if ((g_duel_network_flags & 2) == 0)
  {
    if (g_duel_ai_mode_state == 1 || g_ai_action_replay_available != 0 ||
        (g_trigger_condition != -1 && g_current_turn == g_other_player) || g_current_action_event_code == 4 ||
        (g_other_player == g_event_player && (g_land_can_be_played & 0x200) != 0))
    {
      s.selected_card = choose_response_card(s.player);
      s.selected_player = g_target_player_choice;
    }
    else
    {
      s.selected_card = 0xffffffffU;
    }

    s.result_flags = 0;

    if (s.selected_card != 0xffffffffU &&
        dispatch_card_trigger_event(s.selected_player, (int)s.selected_card, 0x7d, s.player) == 2)
    {
      resolve_card_immediately(s.selected_player, (int)s.selected_card, s.player);
      s.selected_card = 0xffffffffU;
      TENTATIVE_reassess_all_cards(0, 0xff);
    }

    if (s.selected_card != 0xffffffffU)
    {
      s.internal_card_id = PLAYER_CARD_INSTANCE(s.selected_player, (int)s.selected_card).internal_card_id;
      g_selected_response_internal_card_id = s.internal_card_id;

      if (is_in_play(s.selected_player, (int)s.selected_card))
      {
        if (PLAYER_CARD_INSTANCE(s.selected_player, (int)s.selected_card).unknown0x14 == (unsigned int)g_trigger_condition &&
            pending_killed_card_handler != NULL && g_trigger_condition != -1)
        {
          if (pending_killed_card_handler != NULL)
          {
            pending_killed_card_handler(s.selected_player, (int)s.selected_card);
          }
        }
        else
        {
          dispatch_event_to_single_card(s.selected_player, (int)s.selected_card, 0x73, 1 - s.selected_player, -1);
          if (activate(s.player, s.selected_player, (int)s.selected_card))
          {
            resolve_activated_ability(s.selected_player, (int)s.selected_card);
          }
          g_spell_fizzled = 0;
        }

        s.result_flags = 1;
        TENTATIVE_reassess_all_cards(0, 0xff);
      }
      else
      {
        if (resolve_cast_card(s.selected_player, (int)s.selected_card))
        {
          if (global_cards_data[s.internal_card_id].type == 0x20)
          {
            g_response_card_type_mask = g_response_action_mask & 0x20;
          }

          s.result_flags = 1;
          TENTATIVE_reassess_all_cards(0, 0xff);

          if (g_duel_ai_mode_state != 1 && internal_rand(3) == 0)
          {
            show_opponent_taunt("Didn't expect that, did ya?");
          }
        }
      }
    }
  }
  else
  {
    s.result_flags = 0;
  }

  s.num_candidates = s.response_mask = 0;
  s.maybe_card = 0xffffffffU;

  if (g_duel_ai_mode_state != 1 ||
      (g_trigger_condition != -1 &&
       (g_current_turn == g_active_player || (g_current_turn == g_other_player && (g_duel_network_flags & 2) != 0))))
  {
    if (((g_current_turn == g_active_player) || (g_current_turn == g_other_player && (g_duel_network_flags & 2) != 0)) &&
        g_trigger_condition != -1)
    {
      for (s.player = 0; s.player <= 1; ++s.player)
      {
        for (s.card = 0; (int)s.card < g_active_cards_count[s.player]; ++s.card)
        {
          if (is_in_play(s.player, (int)s.card))
          {
            s.response_code = dispatch_card_trigger_event(s.player, (int)s.card, 0x7d, reason_for_trigger_controller);
            if (s.response_code)
            {
              if (s.response_code == 2)
              {
                s.maybe_card = s.card;
                s.maybe_player = s.player;
                s.selected_player = s.player;
                ++s.num_candidates;
              }
              else
              {
                s.response_mask |= 1U << (unsigned char)s.response_code;
              }
            }
          }

          if ((g_response_processing_flags & 1) != 0 &&
              PLAYER_CARD_INSTANCE(s.player, (int)s.card).unknown0x14 == (unsigned int)g_trigger_condition &&
              g_current_turn == s.player && g_trigger_condition != -1 &&
              PLAYER_CARD_INSTANCE(s.player, (int)s.card).internal_card_id != -1)
          {
            s.response_mask |= 4;
            s.maybe_card = s.card;
            s.maybe_player = s.player;
            s.selected_player = s.player;
            ++s.num_candidates;
          }
        }
      }
    }

    if (s.maybe_card == 0xffffffffU)
    {
      if (g_duel_ai_mode_state != 1)
      {
        if (g_trigger_condition == 0xca && (char)g_duel_phase_stop_settings[g_current_player].phase_flags[PHASE_UPKEEP] == 0)
        {
          g_response_processing_flags = 0;
        }
        if (g_trigger_condition == 0xce && (char)g_duel_phase_stop_settings[g_current_player].phase_flags[PHASE_DRAW] == 0)
        {
          g_response_processing_flags = 0;
        }
      }
    }

    if (s.maybe_card != 0xffffffffU || g_duel_ai_mode_state != 1)
    {
      if (s.maybe_card != 0xffffffffU || s.response_mask != 0 || ((g_response_processing_flags & 1) != 0 && g_response_action_mask != 0))
      {
        if ((g_duel_network_flags & 2) == 0)
        {
          s.player = g_active_player;
        }
        else
        {
          s.player = reason_for_trigger_controller;
        }

        g_response_processing_flags = g_activation_event_flags = 0;
        s.some_flag = 0;

        for (s.card = 0; (int)s.card < g_active_cards_count[s.player]; ++s.card)
        {
          if (PLAYER_CARD_INSTANCE(s.player, (int)s.card).internal_card_id != -1 &&
              ((PLAYER_CARD_INSTANCE(s.player, (int)s.card).upkeep_flags & 1) != 0 ||
               (PLAYER_CARD_INSTANCE(s.player, (int)s.card).upkeep_flags & 0x10) != 0 ||
               (!is_nonactivated_mana_source(s.player, (int)s.card) && s.player == g_event_player)))
          {
            s.response_code = get_card_response_action_type(s.player, (int)s.card);

            if (((s.response_code >= 2) ||
                 (((g_ai_decision_parameter != 0 || s.player != g_current_player) && (s.response_code & 2) != 0) ||
                  (g_activation_event_flags & 2) != 0)) &&
                ((g_current_turn != s.player || g_trigger_condition == -1) ||
                 (s.response_code != 2 || (g_activation_event_flags & 2) != 0)))
            {
              if ((g_activation_event_flags & 2) == 0 && s.response_code != 2)
              {
                if (s.response_code == 2)
                {
                  s.response_mask |= 4;
                }
                else
                {
                  s.response_mask |= 2;
                }
              }
              else
              {
                s.maybe_card = s.card;
                s.maybe_player = s.player;
                ++s.num_candidates;
              }

              g_activation_event_flags &= ~2;
            }
          }
        }

        if (g_current_action_event_code == 4)
        {
          for (s.card = 0; (int)s.card < g_active_cards_count[1 - s.player]; ++s.card)
          {
            if (PLAYER_CARD_INSTANCE(1 - s.player, (int)s.card).internal_card_id != -1 &&
                ((PLAYER_CARD_INSTANCE(1 - s.player, (int)s.card).upkeep_flags & 1) != 0 ||
                 (PLAYER_CARD_INSTANCE(1 - s.player, (int)s.card).upkeep_flags & 0x10) != 0 ||
                 !is_nonactivated_mana_source(1 - s.player, (int)s.card)))
            {
              s.response_code = get_card_response_action_type(1 - s.player, (int)s.card);
              if ((g_activation_event_flags & 2) != 0)
              {
                s.maybe_card = s.card;
                s.maybe_player = 1 - s.player;
                ++s.num_candidates;

                g_activation_event_flags &= ~2;

                if (s.response_code == 2)
                {
                  s.response_mask |= 4;
                }
                else
                {
                  s.response_mask |= 2;
                }

                if ((g_duel_network_flags & 2) == 0)
                {
                  break;
                }
              }
            }
          }
        }

        if ((s.response_mask & 2) != 0)
        {
          s.interrupts_available = 1;
        }
      }
    }
  }

  if (s.interrupts_available || s.can_respond || s.num_candidates)
  {
    strcpy(s.prompt_format, gs_triggered_effects_009396a0);

    if ((g_response_card_type_mask & 0x10) != 0 && g_current_spell_player == -1)
    {
      strcpy(s.prompt_format, gs_fast_effects_00926520);
    }
    else if ((g_response_card_type_mask & 0x20) != 0)
    {
      if (g_duel_state_007a7d78 != 0)
      {
        g_duel_state_007a7d78 = 1;
      }
      strcpy(s.prompt_format, gs_interrupts_0093a850);
    }

    if (g_trigger_condition != -1)
    {
      strcpy(s.prompt_format, gs_triggered_effects_009396a0);
    }

    sprintf(g_ui_message_buffer, s.prompt_format, s.prompt_copy);

    if (g_phase_stop_suppressed == 0 || (s.response_mask & 2) != 0)
    {
      if (((s.can_respond == 0 && (g_ai_decision_parameter == 0 || g_trigger_condition != -1)) &&
           (s.interrupts_available == 0 || g_trigger_condition == -1) &&
           (g_affected_card_context_flags == 0 || (s.response_mask & 6) == 0)) &&
          ((s.num_candidates <= (int)!(s.response_mask & 2) && (s.response_mask & 4) == 0) ||
           ((s.some_flag == 0 && should_skip_phase(reason_for_trigger_controller) != 0) || g_trigger_condition == 0xd6 ||
            (s.maybe_card != 0xffffffffU && g_response_window_serial == g_response_window_depth))))
      {
        s.selected_card = s.maybe_card;
        s.selected_player = s.maybe_player;
        g_response_action_selected = 1;
      }
      else
      {
        if (g_duel_ai_mode_state != 1)
        {
          g_phase_was_skipped = 1;
        }

        g_response_window_serial = 0;

        s.selection_done = g_response_window_serial;
        while (!s.selection_done)
        {
          if (g_duel_ai_mode_state == 1)
          {
            s.selected_card = s.maybe_card;
            s.selection_done = 1;
            g_target_selection_status_code = -1;
          }
          else
          {
            if ((g_duel_network_flags & 2) == 0)
            {
              s.player = g_active_player;
            }
            else
            {
              s.player = reason_for_trigger_controller;
            }

            if (g_active_player == s.player)
            {
              g_stop_phase = g_stop_phase_player = -1;
            }
            else
            {
              g_previous_stop_phase = g_previous_stop_phase_player = -1;
            }

            s.selected_card = select_card_for_action(s.player, -1, s.player, 0xff, 0, g_ui_message_buffer, 2);
            s.selected_player = g_target_player_choice;
            if ((int)s.selected_card >= 0)
            {
              g_duel_phase_stop_settings[g_current_player].phase_flags[g_current_phase] =
                  (unsigned char)(((int)(char)g_duel_phase_stop_settings[g_current_player].phase_flags[g_current_phase]) | PHASE_STOP_SUPPRESSED);
            }
          }

          switch (g_target_selection_status_code)
          {
          case 0:
            if (s.selected_player != -1 && s.selected_card != 0xffffffffU)
            {
              s.selection_done = 1;
            }
            else if (s.selected_player != -1 && s.selected_card == 0xffffffffU)
            {
              s.selection_done = 0;
            }
            break;

          case -3:
            s.selection_done = 0;
            break;

          case -2:
            s.selection_done = 1;
            s.selected_card = 0xffffffffU;

            g_activation_dispatch_flags &= ~2;
            if (g_trigger_condition != -1)
            {
              g_target_selection_status_code = 0;
            }

            if (s.num_candidates != 0)
            {
              g_response_window_serial = g_response_window_depth;
              g_response_action_selected = 1;
              if (g_duel_ai_mode_state != 1)
              {
                g_phase_stop_suppressed = 1;
              }
            }
            break;
          }
        }
      }
    }
    else
    {
      s.selected_card = s.maybe_card;
      s.selected_player = s.maybe_player;
      if (s.maybe_card != 0xffffffffU)
      {
        g_response_action_selected = 1;
      }
    }

    strcpy(g_ui_message_buffer, "");

    if ((g_duel_network_flags & 2) == 0)
    {
      s.player = g_active_player;
    }
    else
    {
      s.player = reason_for_trigger_controller;
    }

    if (s.selected_card != 0xffffffffU &&
        (s.selected_player == s.player ||
         (((PLAYER_CARD_INSTANCE(s.selected_player, (int)s.selected_card).state & 2) != 0 &&
           dispatch_card_trigger_event(s.selected_player, (int)s.selected_card, 0x7d, s.player) != 0) ||
          g_current_action_event_code == 4)))
    {
      s.internal_card_id = PLAYER_CARD_INSTANCE(s.selected_player, (int)s.selected_card).internal_card_id;

      if (get_card_response_action_type(s.selected_player, (int)s.selected_card) != 0)
      {
        if (is_in_play(s.selected_player, (int)s.selected_card))
        {
          if (dispatch_card_trigger_event(s.selected_player, (int)s.selected_card, 0x7d, s.player) != 0)
          {
            resolve_card_immediately(s.selected_player, (int)s.selected_card, s.player);
          }
          else
          {
            if (PLAYER_CARD_INSTANCE(s.selected_player, (int)s.selected_card).unknown0x14 == (unsigned int)g_trigger_condition &&
                pending_killed_card_handler != NULL && g_trigger_condition != -1)
            {
              if (pending_killed_card_handler != NULL)
              {
                pending_killed_card_handler(s.selected_player, (int)s.selected_card);
              }
            }
            else
            {
              if (activate(s.player, s.selected_player, (int)s.selected_card))
              {
                resolve_activated_ability(s.selected_player, (int)s.selected_card);

                if (g_spell_fizzled != 1 && g_duel_ai_mode_state != 1)
                {
                  play_sound_effect(0x26);
                }
              }

              g_spell_fizzled = 0;
            }
          }
        }
        else
        {
          resolve_cast_card(s.selected_player, (int)s.selected_card);
          if (global_cards_data[s.internal_card_id].type == 0x20)
          {
            g_response_card_type_mask = g_response_action_mask & 0x20;
          }

          if (internal_rand(3) == 0)
          {
            show_opponent_taunt("I knew that was coming.");
          }
        }

        TENTATIVE_reassess_all_cards(0, 0xff);
        s.result_flags |= 2;
      }
      else
      {
        if (can_activate_mana_source_for_stop_prompt(s.selected_player, (int)s.selected_card) != 0 && s.selected_player == reason_for_trigger_controller)
        {
          activate_mana_source_card(s.selected_player, (int)s.selected_card);
        }
      }

      s.result_flags |= 2;
    }

    g_response_processing_flags = 1;
    g_response_action_selected = 0;
  }

  if (s.result_flags == 0)
  {
    g_response_card_type_mask = g_response_action_mask & 0x30;
  }

  g_response_selection_in_progress = 0;
  g_event_player = s.saved_unk_00742f60;
  g_land_can_be_played = s.saved_land_can_be_played;
  --g_response_window_depth;

  return (int)s.result_flags;
}

#ifndef SHANDALAR
// FUNCTION: MAGIC 0x004e1c8c
void show_card_image_dialog(unsigned int internal_card_id, int unk1, char *prompt, int unk2)
{
  (void)internal_card_id;
  (void)unk1;
  (void)prompt;
  (void)unk2;
}
#endif

// FUNCTION: MAGIC 0x00435c1f
// FUNCTION: SHANDALAR 0x004170f8
int begin_mana_payment_record(void)
{
  int color;

  if (g_mana_payment_record_depth < 10)
  {
    for (color = 0; color < 7; ++color)
    {
      g_recorded_mana_payments[g_mana_payment_record_depth][color] = 0;
    }
    ++g_mana_payment_record_depth;
    return 1;
  }
  else
  {
    return 0;
  }
}

// FUNCTION: MAGIC 0x00435cdd
// FUNCTION: SHANDALAR 0x004171b7
int end_mana_payment_record(void)
{
  if (g_mana_payment_record_depth > 0)
  {
    --g_mana_payment_record_depth;
    return 1;
  }
  else
  {
    return 0;
  }
}

// FUNCTION: MAGIC 0x00435d11
// FUNCTION: SHANDALAR 0x004171eb
int restore_recorded_mana_to_pool(int player)
{
  int color;

  if (g_mana_payment_record_depth > 0)
  {
    for (color = 0; color < 7; ++color)
    {
      g_raw_mana_available[player][color] += g_recorded_mana_payments[g_mana_payment_record_depth - 1][color];
      g_raw_mana_available[player][7] += g_recorded_mana_payments[g_mana_payment_record_depth - 1][color];
    }

    return 1;
  }
  else
  {
    return 0;
  }
}

// FUNCTION: MAGIC 0x004438cb
// FUNCTION: SHANDALAR 0x0040f165
int refresh_stack_proxy_card(int prompt_flag)
{
  struct
  {
    int card;                    /* ebp-0x18 */
    int saved_parent_controller; /* ebp-0x14 */
    int stack_index;             /* ebp-0x10 */
    int saved_parent_card;       /* ebp-0x0c */
    int saved_timestamp;         /* ebp-0x08 */
    int player;                  /* ebp-0x04 */
  } s;

  s.stack_index = g_stack_size - 1;
  s.player = global_stack_cards[s.stack_index].player;
  s.card = global_stack_cards[s.stack_index].card;

  if (PLAYER_CARD_INSTANCE(s.player, s.card).internal_card_id == g_stack_proxy_internal_card_id)
  {
    s.saved_parent_card = PLAYER_CARD_INSTANCE(s.player, s.card).parent_card;
    s.saved_parent_controller = PLAYER_CARD_INSTANCE(s.player, s.card).parent_controller;
    s.saved_timestamp = PLAYER_CARD_INSTANCE(s.player, s.card).timestamp;

    memcpy(global_card_instances[s.player] + s.card,
           global_card_instances[PLAYER_CARD_INSTANCE(s.player, s.card).parent_controller] +
               PLAYER_CARD_INSTANCE(s.player, s.card).parent_card,
           300);

    PLAYER_CARD_INSTANCE(s.player, s.card).internal_card_id = g_stack_proxy_internal_card_id;
    PLAYER_CARD_INSTANCE(s.player, s.card).unknown0x14 = 0;
    PLAYER_CARD_INSTANCE(s.player, s.card).kill_code = 0;
    PLAYER_CARD_INSTANCE(s.player, s.card).state |= 2;

    PLAYER_CARD_INSTANCE(s.player, s.card).parent_controller = s.saved_parent_controller;
    PLAYER_CARD_INSTANCE(s.player, s.card).parent_card = s.saved_parent_card;
    PLAYER_CARD_INSTANCE(s.player, s.card).timestamp = s.saved_timestamp;

    if (PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(s.player, s.card).parent_controller,
                             PLAYER_CARD_INSTANCE(s.player, s.card).parent_card)
            .internal_card_id != -1)
    {
      PLAYER_CARD_INSTANCE(s.player, s.card).original_internal_card_id =
          PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(s.player, s.card).parent_controller,
                               PLAYER_CARD_INSTANCE(s.player, s.card).parent_card)
              .internal_card_id;
    }

    if ((int)PLAYER_CARD_INSTANCE(s.player, s.card).original_internal_card_id < g_damage_card_internal_card_id ||
        (g_damage_card_internal_card_id + 0x2d) <= (int)PLAYER_CARD_INSTANCE(s.player, s.card).original_internal_card_id)
    {
      *(unsigned int *)&PLAYER_CARD_INSTANCE(s.player, s.card).display_pic_csv_id =
          (unsigned int)global_cards_data[PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(s.player, s.card).parent_controller,
                                                               PLAYER_CARD_INSTANCE(s.player, s.card).parent_card)
                                              .original_internal_card_id]
              .id;
    }
  }

  if (g_duel_ai_mode_state != 1)
  {
    g_stack_prompt_flags[s.stack_index] = prompt_flag;
  }

  global_stack_damage_targets[s.stack_index].player =
      (int)(char)PLAYER_CARD_INSTANCE(s.player, s.card).damage_target_player;
  global_stack_damage_targets[s.stack_index].card =
      PLAYER_CARD_INSTANCE(s.player, s.card).damage_target_card;
  return 0;
}

// FUNCTION: MAGIC 0x00444443
// FUNCTION: SHANDALAR 0x0040fcdd
int set_stack_damage_targets(void)
{
  struct
  {
    int card;
    int stack_index;
    int player;
  } s;

  for (s.stack_index = 0; s.stack_index < g_stack_size; ++s.stack_index)
  {
    s.player = global_stack_cards[s.stack_index].player;
    s.card = global_stack_cards[s.stack_index].card;
    global_stack_damage_targets[s.stack_index].player =
        (int)(char)PLAYER_CARD_INSTANCE(s.player, s.card).damage_target_player;
    global_stack_damage_targets[s.stack_index].card =
        PLAYER_CARD_INSTANCE(s.player, s.card).damage_target_card;
  }
  return 0;
}

// FUNCTION: MAGIC 0x004427d9
// FUNCTION: SHANDALAR 0x0040e073
int count_colored_cards_in_hand(int player)
{
  struct
  {
    int count;
    int card;
    int internal_card_id;
  } s;

  s.count = 0;
  for (s.card = 0; s.card < g_active_cards_count[player]; ++s.card)
  {
    s.internal_card_id = PLAYER_CARD_INSTANCE(player, s.card).internal_card_id;
    if (s.internal_card_id != -1)
    {
      if ((PLAYER_CARD_INSTANCE(player, s.card).state & 2) == 0)
      {
        if ((global_cards_data[s.internal_card_id].type & 1) != 0)
        {
          if ((char)global_cards_data[s.internal_card_id].color != 0)
          {
            ++s.count;
          }
        }
      }
    }
  }

  return s.count;
}

int find_first_visible_window_index(HWND *windows, int count);
int restack_visible_windows_after(HWND previous_window, HWND *windows, int window_count);

// FUNCTION: MAGIC 0x00495fa1
// FUNCTION: SHANDALAR 0x00466a34
void restack_duel_child_windows(void)
{
  struct
  {
    HWND *next;      // ebp-0x3c
    int count;       // ebp-0x38
    int best_index;  // ebp-0x34
    int i;           // ebp-0x30
    HWND wnd;        // ebp-0x2c
    int found_index; // ebp-0x28
    int changed;     // ebp-0x24
    HWND windows[8]; // ebp-0x20
  } s;

  extern int g_duel_tooltip_window;

  s.windows[7] = (HWND)g_duel_tooltip_window;
  s.windows[6] = g_duel_prompt_context_hwnd;
  s.windows[5] = (HWND)g_duel_phase_display_window_hwnd;
  s.windows[4] = (HWND)g_duel_attack_phase_window_hwnd;
  s.windows[3] = g_attack_phase_window_hwnd;
  s.windows[2] = (HWND)g_duel_full_card_window_hwnd;
  s.windows[1] = (HWND)g_duel_life_window_hwnd;
  s.windows[0] = (HWND)g_duel_card_preview_window_hwnd;

  s.count = 8;
  s.changed = 0;
  s.wnd = (HWND)g_duel_window_hwnd;
  s.best_index = -1;

  while (s.wnd != (HWND)0)
  {
    s.wnd = GetWindow(s.wnd, 3);
    s.i = 0;
    s.found_index = -1;

    for (;;)
    {
      if (s.i >= s.count)
      {
        break;
      }
      if (s.found_index != -1)
      {
        break;
      }
      if (s.windows[s.i] == s.wnd)
      {
        s.found_index = s.i;
      }
      ++s.i;
    }

    if (s.found_index != -1)
    {
      if (IsWindowVisible(s.wnd))
      {
        if (s.found_index < s.best_index)
        {
          s.changed = 1;
        }
        s.best_index = s.found_index;
      }
    }
  }

  if (s.changed != 0)
  {
    s.found_index = find_first_visible_window_index(&s.windows[0], s.count);
    if (s.found_index != -1)
    {
      SetWindowPos(s.windows[s.found_index], (HWND)1, 0, 0, 0, 0, 3);

      if (s.found_index + 1 >= s.count)
      {
        s.next = (HWND *)0;
      }
      else
      {
        s.next = &s.windows[s.found_index + 1];
      }

      restack_visible_windows_after(s.windows[s.found_index], s.next, s.count - (s.found_index + 1));
    }
  }
}

// FUNCTION: MAGIC 0x00496128
// FUNCTION: SHANDALAR 0x00466bbb
int find_first_visible_window_index(HWND *windows, int count)
{
  int result;
  int i;

  result = -1;
  if (windows == 0 || count == 0)
  {
    return -1;
  }

  for (i = 0; i < count && result == -1; ++i)
  {
    if (IsWindowVisible(windows[i]))
    {
      result = i;
    }
  }

  return result;
}

// FUNCTION: MAGIC 0x004961ab
// FUNCTION: SHANDALAR 0x00466c3e
int restack_visible_windows_after(HWND previous_window, HWND *windows, int window_count)
{
  struct
  {
    HWND *next; // ebp-0x8
    int i;      // ebp-0x4
  } s;

  if (windows == 0 || window_count <= 0)
  {
    return 0;
  }

  s.i = find_first_visible_window_index(windows, window_count);
  if (s.i != -1)
  {
    SetWindowPos(windows[s.i], previous_window, 0, 0, 0, 0, 3);

    if (s.i + 1 >= window_count)
    {
      s.next = (HWND *)0;
    }
    else
    {
      s.next = &windows[s.i + 1];
    }

    restack_visible_windows_after(windows[s.i], s.next, window_count - (s.i + 1));
    return 1;
  }
  else
  {
    return 0;
  }
}

// FUNCTION: MAGIC 0x00447f59
int reset_duel_tick_timer(void)
{
  g_affected_card_context_flags_0093f4b4 = GetTickCount();
  g_duel_tick_adjustment = 0;
  return 0;
}

// FUNCTION: MAGIC 0x004e1b84
int reset_duel_tick_timer_indirect(void)
{
  return reset_duel_tick_timer();
}

// FUNCTION: MAGIC 0x004a7c4a
// FUNCTION: SHANDALAR 0x00559dcd
int get_duel_thread_time_ms(void)
{
  struct
  {
    unsigned int elapsed_time;
    FILETIME scratch_time;
    FILETIME user_time;
  } s;

  if (g_is_win9x != 0)
  {
    DeviceIoControl(g_mpstime_vxd_handle, 2, NULL, 0, &s.elapsed_time, 4, NULL, NULL);
  }
  else
  {
    GetThreadTimes(GetCurrentThread(), &s.scratch_time, &s.scratch_time, &s.scratch_time, &s.user_time);
    s.elapsed_time = s.user_time.dwLowDateTime / 10000;
  }

  return s.elapsed_time;
}

// FUNCTION: MAGIC 0x004a7cba
// FUNCTION: SHANDALAR 0x00559e3d
void update_duel_thread_time_marker(void)
{
  g_ai_trace_enabled = get_duel_thread_time_ms();
}

// FUNCTION: MAGIC 0x004a3662
// FUNCTION: SHANDALAR 0x0053eff0
void show_mana_burn_dialog(int player, int amount)
{
  struct
  {
    mana_burn_dialog_context_t dialog_context;
    int dialog_result;
  } s;

  if (g_duel_ai_mode_state != 1)
  {
    s.dialog_context.player = player;
    s.dialog_context.amount = amount;
    s.dialog_result = DialogBoxParamA(g_app_instance,
                                      (LPCSTR)0xf3,
                                      g_duel_window_hwnd,
                                      dlgproc_mana_burn,
                                      (LPARAM)&s.dialog_context);
  }
}

// FUNCTION: MAGIC 0x004a36ac
// FUNCTION: SHANDALAR 0x0053f03a
BOOL CALLBACK dlgproc_mana_burn(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  struct
  {
    char title_text[200];
    HDC dc;
    HGDIOBJ font;
    RECT rect;
    char message_text[200];
    char path[264];
  } s;

  switch (msg)
  {
  case WM_INITDIALOG:
    g_mana_burn_dialog_context = (mana_burn_dialog_context_t *)lparam;
    sprintf(s.path, "%s\\WINBK_ManaBurn.pic", global_duelart_path);
    g_mana_burn_dialog_background = load_pic(s.path);
    g_mana_burn_dialog_text_color = 0x100009a;
    g_mana_burn_dialog_shadow_color = 0x10000c9;
    load_text(global_ui_strings_filename, "DIALOG_MANABURN");
    SetDlgItemTextA(hwnd, 0x4bb, g_text_lines[0]);
    ShowWindow(GetDlgItem(hwnd, 0x4bb), 0);
    ShowWindow(GetDlgItem(hwnd, 0x4ba), 0);
    SetTimer(hwnd, 1, 3000, NULL);
    return 1;

  case 0x30f:
  case 0x310:
  case 0x311:
    return handle_button_palette_message((int)hwnd, msg, (int)wparam, lparam);

  case WM_ERASEBKGND:
    s.dc = (HDC)wparam;
    ApplyCardArtPaletteToDc(s.dc);
    GetClientRect(hwnd, &s.rect);
    if (g_mana_burn_dialog_background != NULL)
    {
      DrawBitmapToRect(s.dc, &s.rect, g_mana_burn_dialog_background);
    }
    else
    {
      FillRect(s.dc, &s.rect, GetStockObject(2));
    }
    s.font = (HGDIOBJ)SendDlgItemMessageA(hwnd, 0x4bb, WM_GETFONT, 0, 0);
    SelectObject(s.dc, s.font);
    SetBkMode(s.dc, TRANSPARENT);
    SetTextColor(s.dc, g_mana_burn_dialog_text_color);
    GetDlgItemTextA(hwnd, 0x4bb, s.title_text, 200);
    GetWindowRect(GetDlgItem(hwnd, 0x4bb), &s.rect);
    MapWindowPoints(NULL, hwnd, (LPPOINT)&s.rect, 2);
    SetTextColor(s.dc, g_mana_burn_dialog_shadow_color);
    DrawTextA(s.dc, s.title_text, -1, &s.rect, 1);
    OffsetRect(&s.rect, -2, -2);
    SetTextColor(s.dc, g_mana_burn_dialog_text_color);
    DrawTextA(s.dc, s.title_text, -1, &s.rect, 1);
    GetWindowRect(GetDlgItem(hwnd, 0x4ba), &s.rect);
    MapWindowPoints(NULL, hwnd, (LPPOINT)&s.rect, 2);
    load_text(global_ui_strings_filename, "DIALOG_MANABURN");
    if (g_mana_burn_dialog_context->player == 1)
    {
      copy_opponent_name_prefix(s.message_text);
      sprintf(s.title_text, g_text_lines[1], s.message_text, g_mana_burn_dialog_context->amount);
    }
    else
    {
      sprintf(s.title_text, g_text_lines[2], g_mana_burn_dialog_context->amount);
    }
    SetTextColor(s.dc, g_mana_burn_dialog_shadow_color);
    DrawTextA(s.dc, s.title_text, -1, &s.rect, 1);
    OffsetRect(&s.rect, -2, -2);
    SetTextColor(s.dc, g_mana_burn_dialog_text_color);
    DrawTextA(s.dc, s.title_text, -1, &s.rect, 1);
    return 1;

  case WM_KEYDOWN:
  case WM_COMMAND:
    delete_and_close_object(g_mana_burn_dialog_background);
    EndDialog(hwnd, 0);
    return 1;

  case WM_LBUTTONDOWN:
  case WM_RBUTTONDOWN:
    delete_and_close_object(g_mana_burn_dialog_background);
    EndDialog(hwnd, 0);
    return 1;

  case WM_TIMER:
    delete_and_close_object(g_mana_burn_dialog_background);
    EndDialog(hwnd, 0);
    return 1;

  default:
    return 0;
  }
}

// FUNCTION: MAGIC 0x004a09c6
// FUNCTION: SHANDALAR 0x0053c35e
int prompt_for_life_total(int player, char *prompt, int initial_life_total)
{
  life_total_dialog_context_t context;
  int result;

  if (player != 0)
  {
    return initial_life_total;
  }
  else
  {
    context.prompt = prompt;
    context.initial_life_total = initial_life_total;
    context.show_reset_button = 0;
    result = DialogBoxParamA(g_app_instance,
                             (LPCSTR)0xdc,
                             g_duel_window_hwnd,
                             dlgproc_prompt_for_life_total,
                             (LPARAM)&context);
    return result;
  }
}

// FUNCTION: MAGIC 0x004a0a29
BOOL CALLBACK dlgproc_prompt_for_life_total(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  struct
  {
    HDC erase_hdc;
    int saved_dc;
    RECT rect;
    COLORREF text_color;
    DRAWITEMSTRUCT *draw_item;
    HWND ctl_hwnd;
    int ctl_id;
    HDC ctl_hdc;
    HWND previous_control;
    HWND current_control;
    UINT reset_life;
    UINT selected_life;
    BOOL was_translated;
    UINT command_id;
    life_total_dialog_context_t *context;
  } s;

  switch (msg)
  {
  case WM_INITDIALOG:
    s.context = (life_total_dialog_context_t *)lparam;
    SetWindowLongA(hwnd, 8, s.context->initial_life_total);
    setup_life_total_dialog_resources(&g_life_total_dialog_background,
                                      &g_life_total_dialog_text_color,
                                      &g_life_total_dialog_button_brush,
                                      &g_life_total_dialog_pen1,
                                      &g_life_total_dialog_pen2,
                                      &g_life_total_dialog_unfocus_text_color,
                                      &g_life_total_dialog_focus_text_color);
    SetDlgItemTextA(hwnd, IDOK, gs_ok_00924800);
    SetDlgItemTextA(hwnd, IDCANCEL, gs_cancel_008a8c20);
    SetDlgItemTextA(hwnd, 0x44d, s.context->prompt);
    if (s.context->show_reset_button == 0)
    {
      ShowWindow(GetDlgItem(hwnd, 0x44e), 0);
    }
    SendMessageA(hwnd, 0x401, IDOK, 0);
    SetDlgItemInt(hwnd, 0x44c, s.context->initial_life_total, 0);
    g_life_total_edit_wndproc = (WNDPROC)SetWindowLongA(GetDlgItem(hwnd, 0x44c), GWL_WNDPROC, (LONG)life_total_edit_wndproc);
    SetFocus(GetDlgItem(hwnd, 0x44c));
    SendDlgItemMessageA(hwnd, 0x44c, EM_SETSEL, 0, -1);
    change_buttonclass_wndproc(hwnd);
    return 0;

  case WM_COMMAND:
    s.command_id = (UINT)(wparam & 0xffff);
    switch (s.command_id)
    {
    case IDOK:
      s.selected_life = GetDlgItemInt(hwnd, 0x44c, &s.was_translated, 0);
      if (s.was_translated != 0)
      {
        cleanup_life_total_dialog_resources(g_life_total_dialog_background,
                                            g_life_total_dialog_button_brush,
                                            g_life_total_dialog_pen1,
                                            g_life_total_dialog_pen2);
        EndDialog(hwnd, s.selected_life);
      }
      else
      {
        SetFocus(GetDlgItem(hwnd, 0x44c));
        SendDlgItemMessageA(hwnd, 0x44c, EM_SETSEL, 0, -1);
      }
      break;

    case IDCANCEL:
      cleanup_life_total_dialog_resources(g_life_total_dialog_background,
                                          g_life_total_dialog_button_brush,
                                          g_life_total_dialog_pen1,
                                          g_life_total_dialog_pen2);
      EndDialog(hwnd, -1);
      break;

    case 0x44e:
      s.reset_life = GetWindowLongA(hwnd, 8);
      SetDlgItemInt(hwnd, 0x44c, s.reset_life, 0);
      SetFocus(GetDlgItem(hwnd, 0x44c));
      break;
    }
    return 1;

  case 0x4c8:
    s.current_control = (HWND)wparam;
    s.previous_control = (HWND)lparam;
    if (GetDlgItem(hwnd, IDCANCEL) == s.current_control)
    {
      SendMessageA(hwnd, 0x401, IDCANCEL, 0);
    }
    else
    {
      SendMessageA(hwnd, 0x401, IDOK, 0);
    }
    if (s.current_control != NULL)
    {
      InvalidateRect(s.current_control, NULL, 1);
    }
    if (s.previous_control != NULL)
    {
      InvalidateRect(s.previous_control, NULL, 1);
    }
    return 0;

  case WM_CTLCOLORSTATIC:
    s.ctl_hdc = (HDC)wparam;
    ApplyCardArtPaletteToDc(s.ctl_hdc);
    s.ctl_hwnd = (HWND)lparam;
    s.ctl_id = GetDlgCtrlID(s.ctl_hwnd);
    SetBkMode(s.ctl_hdc, TRANSPARENT);
    SetTextColor(s.ctl_hdc, g_life_total_dialog_text_color);
    return (BOOL)GetStockObject(NULL_BRUSH);

  case WM_DRAWITEM:
    s.draw_item = (DRAWITEMSTRUCT *)lparam;
    if (GetFocus() == s.draw_item->hwndItem)
    {
      s.text_color = g_life_total_dialog_focus_text_color;
    }
    else
    {
      s.text_color = g_life_total_dialog_unfocus_text_color;
    }
    draw_owner_draw_button_centered(s.draw_item,
                                    g_life_total_dialog_button_brush,
                                    g_life_total_dialog_pen1,
                                    g_life_total_dialog_pen2,
                                    s.text_color,
                                    0);
    return 1;

  case WM_QUERYNEWPALETTE:
  case WM_PALETTEISCHANGING:
  case WM_PALETTECHANGED:
    return handle_button_palette_message((int)hwnd, msg, (int)wparam, lparam);

  case WM_ERASEBKGND:
    s.erase_hdc = (HDC)wparam;
    ApplyCardArtPaletteToDc(s.erase_hdc);
    GetClientRect(hwnd, &s.rect);
    EnterCriticalSection(&g_card_render_lock);
    s.saved_dc = SaveDC(g_shared_offscreen_dc);
    s.erase_hdc = g_shared_offscreen_dc;
    if (g_life_total_dialog_background != (HBITMAP)0)
    {
      DrawBitmapToRect(s.erase_hdc, &s.rect, g_life_total_dialog_background);
    }
    else
    {
      FillRect(s.erase_hdc, &s.rect, GetStockObject(2));
    }
    RestoreDC(g_shared_offscreen_dc, s.saved_dc);
    s.erase_hdc = (HDC)wparam;
    GetClientRect(hwnd, &s.rect);
    BitBlt(s.erase_hdc,
           0,
           0,
           s.rect.right,
           s.rect.bottom,
           g_shared_offscreen_dc,
           0,
           0,
           SRCCOPY);
    LeaveCriticalSection(&g_card_render_lock);
    return 1;

  case WM_LBUTTONDOWN:
    SendMessageA(hwnd, WM_SYSCOMMAND, 0xf012, 0);
    return 0;

  default:
    return 0;
  }
}

// FUNCTION: MAGIC 0x004a0f95
static LRESULT CALLBACK life_total_edit_wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  switch (msg)
  {
  case WM_CHAR:
    if ((wparam >= '0' && wparam <= '9') || wparam == VK_BACK)
    {
      return CallWindowProcA(g_life_total_edit_wndproc, hwnd, msg, wparam, lparam);
    }
    else
    {
      return 0;
    }

  default:
    return CallWindowProcA(g_life_total_edit_wndproc, hwnd, msg, wparam, lparam);
  }
}

// FUNCTION: MAGIC 0x004a1033
static void setup_life_total_dialog_resources(HBITMAP *background,
                                              COLORREF *text_color,
                                              HBRUSH *button_brush,
                                              HPEN *pen1,
                                              HPEN *pen2,
                                              COLORREF *unfocus_text_color,
                                              COLORREF *focus_text_color)
{
  char path[264];

  sprintf(path, "%s\\WINBK_QuestN.pic", global_duelart_path);
  *background = load_pic(path);
  *text_color = 0x100009a;
  *button_brush = CreateSolidBrush(0x100001c);
  *pen1 = CreatePen(PS_SOLID, 0, 0x1000072);
  *pen2 = CreatePen(PS_SOLID, 0, 0x10000ca);
  *unfocus_text_color = 0x100009a;
  *focus_text_color = 0x10000bf;
  if (*button_brush == (HBRUSH)0)
  {
    *button_brush = GetStockObject(2);
  }
  if (*pen1 == (HPEN)0)
  {
    *pen1 = GetStockObject(6);
  }
  if (*pen2 == (HPEN)0)
  {
    *pen2 = GetStockObject(7);
  }
}

// FUNCTION: MAGIC 0x004a1110
static void cleanup_life_total_dialog_resources(HBITMAP background,
                                                HBRUSH button_brush,
                                                HPEN pen1,
                                                HPEN pen2)
{
  if (background != (HBITMAP)0)
  {
    delete_and_close_object(background);
  }
  if (button_brush != (HBRUSH)0)
  {
    DeleteObject(button_brush);
  }
  if (pen1 != (HPEN)0)
  {
    DeleteObject(pen1);
  }
  if (pen2 != (HPEN)0)
  {
    DeleteObject(pen2);
  }
}
