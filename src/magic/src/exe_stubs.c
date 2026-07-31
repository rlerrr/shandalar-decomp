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
int FUN_00447f59(void);
int FUN_004e1b84(void);
int get_duel_thread_time_ms(void);
void update_duel_thread_time_marker(void);
void StopWorldLocationMusic(void);
int activate(int who_activates, int player, int card);
int can_activate_mana_source_for_stop_prompt(int player, int card);
int FUN_0044b7d4(int player, int card);
int resolve_activated_ability(int player, int card);
void prompt_player_for_stop_action(int player, char *prompt);
void show_mana_burn_dialog(int player, int amount);
BOOL CALLBACK dlgproc_mana_burn(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
BOOL CALLBACK dlgproc_prompt_for_life_total(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
void delete_and_close_object(HANDLE obj);
HBITMAP load_pic(char *filename);
void ApplyCardArtPaletteToDc(HDC hdc);
BOOL DrawBitmapToRect(HDC dst_dc, const RECT *dst_rect, HBITMAP bitmap);
extern int DAT_00743098;
extern int DAT_00712544;
void __cdecl _assert(void *expr, void *filename, unsigned line);
void change_buttonclass_wndproc(HWND hwnd);
void save_duel_interface_options_to_registry(void);
void FUN_0044a796(int player, int card);
int card_ball_lightning(int player, int card, event_t event);
void FUN_004955ae(DRAWITEMSTRUCT *draw_item_struct,
                  HBRUSH brush,
                  HPEN pen1,
                  HPEN pen2,
                  COLORREF color,
                  int draw_focus);

// GLOBAL: MAGIC 0x00637f2c
static HBRUSH g_duel_options_dialog_button_brush;
// GLOBAL: MAGIC 0x00637f18
static HPEN g_duel_options_dialog_pen1;
// GLOBAL: MAGIC 0x00637f28
static HPEN g_duel_options_dialog_pen2;
// GLOBAL: MAGIC 0x00637f24
static COLORREF g_duel_options_dialog_focus_text_color;
// GLOBAL: MAGIC 0x00637f34
static COLORREF g_duel_options_dialog_unfocus_text_color;
// GLOBAL: MAGIC 0x00637f30
static COLORREF g_duel_options_dialog_text_color;
// GLOBAL: MAGIC 0x00637f1c
static COLORREF g_duel_options_dialog_title_color;
// GLOBAL: MAGIC 0x00637f20
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

// FUNCTION: MAGIC 0x00489f9f
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

    if (g_duel_interface_options.directive_tracks_mouse != 0)
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
    if (g_duel_interface_options.show_coin_flips != 0)
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
      return 1;
    }
    if ((wparam & 0xffff) == IDOK)
    {
      if (IsDlgButtonChecked(hwnd, 0x426) != 0)
      {
        g_duel_interface_options.layout = 1;
      }
      else
      {
        g_duel_interface_options.layout = 2;
      }
      g_duel_interface_options.directive_tracks_mouse = IsDlgButtonChecked(hwnd, 0x438);
      g_duel_interface_options.show_cue_cards = IsDlgButtonChecked(hwnd, 0x429);
      g_duel_interface_options.show_power_toughness_on_cards = IsDlgButtonChecked(hwnd, 0x42a);
      g_duel_interface_options.show_abilities_on_cards = IsDlgButtonChecked(hwnd, 0x42b);
      g_duel_interface_options.show_coin_flips = IsDlgButtonChecked(hwnd, 0x437);

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
      return 1;
    }
    return 0;

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
      s.brush = g_duel_options_dialog_pen2;
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
    FUN_004955ae(s.draw_item,
                 g_duel_options_dialog_button_brush,
                 g_duel_options_dialog_pen1,
                 g_duel_options_dialog_pen2,
                 s.text_color,
                 0);
    return 1;

  case WM_QUERYNEWPALETTE:
  case WM_PALETTEISCHANGING:
  case WM_PALETTECHANGED:
    return FUN_10025b5e((int)hwnd, msg, (int)wparam, lparam);

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
void reset_trigger_dispatch_state(void)
{
  trigger_dispatch_depth = max_trigger_dispatch_depth = 0;
  regenerate_or_graveyard_triggers_in_progress = 0;
}

// FUNCTION: MAGIC 0x0044af2d
int prompt_stop_phase_anyway(phase_t phase)
{
  if (g_duel_ai_mode_state != 1 &&
      (((int)(char)g_duel_phase_stop_settings[current_player].phase_flags[phase] & (PHASE_STOP_ENABLED | PHASE_STOP_OPPONENT)) != 0 ||
       (phase == stop_phase && current_player == stop_phase_player) ||
       (phase == previous_stop_phase && current_player == previous_stop_phase_player)))
  {
    load_text(global_ui_strings_filename, "PROMPT_STOPANYWAY");

    if (phase == 4)
    {
      strcpy(g_prompt_stop_phase_anyway_text, text_lines[1]);
    }
    else if (phase == 1)
    {
      strcpy(g_prompt_stop_phase_anyway_text, text_lines[2]);
    }
    else if (phase == 10)
    {
      strcpy(g_prompt_stop_phase_anyway_text, text_lines[3]);
    }
    else if (phase == 0x14)
    {
      strcpy(g_prompt_stop_phase_anyway_text, text_lines[4]);
    }
    else if (phase == 0x1f)
    {
      strcpy(g_prompt_stop_phase_anyway_text, text_lines[5]);
    }
    else if (phase == 0x22)
    {
      strcpy(g_prompt_stop_phase_anyway_text, text_lines[6]);
    }
    else if (phase == 0x19)
    {
      strcpy(g_prompt_stop_phase_anyway_text, text_lines[7]);
    }
    else if (phase == 0x1a)
    {
      strcpy(g_prompt_stop_phase_anyway_text, text_lines[8]);
    }
    else if (phase == 0x1b)
    {
      strcpy(g_prompt_stop_phase_anyway_text, text_lines[8]);
    }
    else
    {
      strcpy(g_prompt_stop_phase_anyway_text, text_lines[0]);
    }

    if ((g_duel_network_flags & 2) != 0)
    {
      if (active_player == current_player)
      {
        if (((int)(char)g_duel_phase_stop_settings[current_player].phase_flags[phase] & PHASE_STOP_ENABLED) != 0 ||
            (phase == stop_phase && current_player == stop_phase_player))
        {
          prompt_player_for_stop_action(active_player, g_prompt_stop_phase_anyway_text);
        }
        if (((int)(char)g_duel_phase_stop_settings[current_player].phase_flags[phase] & PHASE_STOP_OPPONENT) != 0 ||
            (phase == previous_stop_phase && current_player == previous_stop_phase_player))
        {
          prompt_player_for_stop_action(other_player, g_prompt_stop_phase_anyway_text);
        }
      }
      else
      {
        if (((int)(char)g_duel_phase_stop_settings[current_player].phase_flags[phase] & PHASE_STOP_OPPONENT) != 0 ||
            (phase == previous_stop_phase && current_player == previous_stop_phase_player))
        {
          prompt_player_for_stop_action(other_player, g_prompt_stop_phase_anyway_text);
        }
        if (((int)(char)g_duel_phase_stop_settings[current_player].phase_flags[phase] & PHASE_STOP_ENABLED) != 0 ||
            (phase == stop_phase && current_player == stop_phase_player))
        {
          prompt_player_for_stop_action(active_player, g_prompt_stop_phase_anyway_text);
        }
      }
    }
    else
    {
      prompt_player_for_stop_action(active_player, g_prompt_stop_phase_anyway_text);
    }
  }

  return -1;
}

// FUNCTION: MAGIC 0x0044b2b3
void prompt_player_for_stop_action(int player, char *prompt)
{
  int selected_card;
  int done;
  char prompt_copy[100];

  strcpy(prompt_copy, prompt);
  done = 0;

  while (done == 0)
  {
    if (active_player == current_player)
    {
      stop_phase_player = -1;
      stop_phase = -1;
    }
    else
    {
      previous_stop_phase_player = -1;
      previous_stop_phase = -1;
    }

    selected_card = select_card_for_action(player, player, player, 0xff, 0, prompt_copy, 2);
    if (g_target_selection_status_code != -3)
    {
      if (g_target_selection_status_code == -2)
      {
        done = 1;
      }
      else if (g_target_selection_status_code == 0 && selected_card != -1 &&
               can_activate_mana_source_for_stop_prompt(unk_00742fcc, selected_card) != 0)
      {
        FUN_0044b7d4(unk_00742fcc, selected_card);
      }
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
    if (raw_mana_available[other_player][7] != 0 && current_phase > 0x1a)
    {
      for (current_card = 0;
           current_card < active_cards_count[other_player];
           current_card++)
      {
        if (is_in_play(other_player, current_card) != 0)
        {
          dispatch_event_to_single_card(other_player, current_card, 0x8f, 1 - other_player, -1);
          if (last_single_card_event_result != 0)
          {
            if (activate(other_player, other_player, current_card) != 0)
            {
              resolve_activated_ability(other_player, current_card);
            }
            goto loop;
          }
        }
      }
    }
  }

  for (player = 0; player < 2; player = player + 1)
  {
    if (raw_mana_available[player][7] > 0)
    {
      if (g_duel_ai_mode_state != 1)
      {
        play_sound_effect(0x2a);
        show_mana_burn_dialog(player, raw_mana_available[player][7]);
      }
      life[player] = life[player] - raw_mana_available[player][7];
      for (current_card = 0; current_card <= 7; current_card = current_card + 1)
      {
        raw_mana_available[player][current_card] = 0;
      }
    }
  }
}

// FUNCTION: MAGIC 0x0044b646
int FUN_0044b646(int *card_pairs, int card_pair_count, int player, int card)
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
int allow_response_once(int param_1, int param_2, char *prompt, int param_4)
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

  s.saved_trigger_condition = trigger_condition;
  s.saved_DAT_007aadec = DAT_007aadec;
  trigger_condition = -1;
  s.saved_unk_008b3270 = unk_008b3270;

  ++trigger_dispatch_depth;
  if (trigger_dispatch_depth == 1)
  {
    max_trigger_dispatch_depth = 0;
  }
  else if (max_trigger_dispatch_depth < trigger_dispatch_depth && param_2 != 0x8e && param_2 != 0x70 && param_2 != 0xd3)
  {
    max_trigger_dispatch_depth = trigger_dispatch_depth;
  }

  s.result = 0;
  DAT_007aadec = param_4;
  s.saved_DAT_00777aa8 = DAT_00777aa8;
  s.saved_DAT_00561390 = DAT_00561390;

  if (param_1 == -2 && should_skip_phase(current_player) == 0)
  {
    DAT_00561390 = 1;
  }
  else
  {
    DAT_00561390 = 0;
  }

  if ((g_duel_network_flags & 2) == 0)
  {
    if (should_skip_phase(current_player) == 0 &&
        ((((int)(char)g_duel_phase_stop_settings[current_player].phase_flags[param_2] & PHASE_STOP_ENABLED) != 0) ||
         (current_player == stop_phase_player && stop_phase == param_2)))
    {
      DAT_00777aa8 = 1;
    }
    else
    {
      DAT_00777aa8 = 0;
    }
  }

  if (param_1 > -1)
  {
    if (DAT_00777aa8 == 0)
    {
      goto cleanup;
    }
  }

  strcpy(s.prompt_copy, prompt);

  if (current_phase == PHASE_UPKEEP && trigger_dispatch_depth == 1)
  {
    unk_00742f60 = current_player;
    reset_upkeep_costs();
  }

  while (1)
  {
  restart_response_window:
    do
    {
      DAT_00777aa8 = can_stop_for_phase(1);

      if ((g_duel_network_flags & 2) != 0)
      {
        DAT_00789714 = 1;
      }
      else
      {
        if (current_player != 0)
        {
          if (param_1 > -1)
          {
            DAT_00789714 = 0;
          }
          else
          {
            DAT_00789714 = 2;
          }
        }
        else
        {
          DAT_00789714 = 1;
        }
      }

      if (trigger_dispatch_depth <= 1)
      {
        _DAT_00743020 = 0xffffffff;
      }

      spell_fizzled = 0;
      unk_008b3270 = spell_fizzled;

      s.dispatch_result = (unsigned int)process_response_actions(current_player, s.prompt_copy);
      if (s.dispatch_result != 0)
      {
        response_action_taken = 1;
      }

      if (((active_player == current_player) ||
           (other_player == current_player && (g_duel_network_flags & 2) != 0)) &&
          s.dispatch_result != 0 && g_duel_ai_mode_state != 1)
      {
        s.result = 1;
      }

      if (trigger_dispatch_depth < max_trigger_dispatch_depth && stack_size >= 0)
      {
        s.dispatch_result = 0;
      }

    } while (s.dispatch_result != 0 || ((unk_008b3270 & 1) != 0 && trigger_dispatch_depth == 1));

    if (current_phase == PHASE_UPKEEP && trigger_dispatch_depth == 1)
    {
      unk_00742f60 = 1 - current_player;
      reset_upkeep_costs();
    }

    while (1)
    {
      s.saved_DAT_00777aa8_in_loop = DAT_00777aa8;
      s.saved_phase_stop_suppressed = phase_stop_suppressed;

      if ((g_duel_network_flags & 2) != 0)
      {
        DAT_00789714 = 1;
        phase_stop_suppressed = 0;

        if (stack_size == 0 && should_skip_phase(1 - current_player) != 0 && param_1 != -2)
        {
          phase_stop_suppressed = 1;
        }

        DAT_00777aa8 = can_stop_for_phase(0);
      }
      else
      {
        if (current_player != 0)
        {
          DAT_00789714 = 1;
        }
        else
        {
          if (param_1 > -1)
          {
            DAT_00789714 = 0;
          }
          else
          {
            DAT_00789714 = 2;
          }
        }
      }

      if (trigger_dispatch_depth <= 1)
      {
        _DAT_00743020 = 0xffffffff;
      }

      spell_fizzled = 0;
      unk_008b3270 = spell_fizzled;

      if ((trigger_dispatch_depth < max_trigger_dispatch_depth && stack_size >= 0))
      {
        goto cleanup;
      }

      s.dispatch_result = (unsigned int)process_response_actions(1 - current_player, s.prompt_copy);
      DAT_00777aa8 = s.saved_DAT_00777aa8_in_loop;
      phase_stop_suppressed = s.saved_phase_stop_suppressed;
      if (s.dispatch_result != 0)
      {
        response_action_taken = 1;

        if (g_duel_ai_mode_state != 1)
        {
          goto restart_response_window;
        }

        if (active_player == current_player)
        {
          goto restart_response_window;
        }
        if ((g_duel_network_flags & 2) != 0)
        {
          goto restart_response_window;
        }
        if ((unk_008b3270 & 1) != 0)
        {
          if (trigger_dispatch_depth == 1)
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
  --trigger_dispatch_depth;
  if (trigger_dispatch_depth != 0)
  {
    _DAT_00743020 = 0xffffffff;
  }
  else
  {
    DAT_00742f68 = 0xffffffff;
    _DAT_00743020 = DAT_00742f68;
    if (nested_trigger_depth == 0)
    {
      DAT_00742f64 = 0;
      if (g_duel_ai_mode_state != 1)
      {
        phase_stop_suppressed = 0;
      }
    }
  }

  DAT_00777aa8 = s.saved_DAT_00777aa8;
  DAT_00561390 = s.saved_DAT_00561390;
  unk_008b3270 = s.saved_unk_008b3270;
  trigger_condition = s.saved_trigger_condition;
  DAT_007aadec = s.saved_DAT_007aadec;

  if (s.result != 0)
  {
    ai_action_replay_available = 0;
  }

  return s.result;
}

// FUNCTION: MAGIC 0x004af6f4
int FUN_004af6f4(void)
{
  if (DAT_007aadec == 0x8e)
  {
    if (attacking_creature_count != 0)
      return 1;
    else
      return 0;
  }
  else if (DAT_007aadec == 0x6a || DAT_007aadec == 0x6b || DAT_007aadec == 0x6c ||
           DAT_007aadec == 0x6d || DAT_007aadec == 0x6e || DAT_007aadec == 0x6f ||
           DAT_007aadec == 0x70 || DAT_007aadec == 0x71 || DAT_007aadec == 0x72 ||
           DAT_007aadec == 0x73 || DAT_007aadec == 0x74 || DAT_007aadec == 0x75 ||
           DAT_007aadec == 0x76 || DAT_007aadec == 0x77 || DAT_007aadec == 0x78 ||
           DAT_007aadec == 0x79 || DAT_007aadec == 0x7a || DAT_007aadec == 0x7b ||
           DAT_007aadec == 0x7c || DAT_007aadec == 0x7d || DAT_007aadec == 0x7e ||
           DAT_007aadec == 0x7f || DAT_007aadec == 0x80 || DAT_007aadec == 0x81 ||
           DAT_007aadec == 0x82 || DAT_007aadec == 0x83 || DAT_007aadec == 0x84 ||
           DAT_007aadec == 0x85 || DAT_007aadec == 0x86 || DAT_007aadec == 0x87 ||
           DAT_007aadec == 0x88 || DAT_007aadec == 0x89 || DAT_007aadec == 0x8e ||
           DAT_007aadec == 0xc7 || DAT_007aadec == 0xc8 || DAT_007aadec == 0xc9 ||
           DAT_007aadec == 0xca || DAT_007aadec == 0xcb || DAT_007aadec == 0xcc ||
           DAT_007aadec == 0xcd || DAT_007aadec == 0xce || DAT_007aadec == 0xcf ||
           DAT_007aadec == 0xd2 || DAT_007aadec == 0xd3 || DAT_007aadec == 0xd4 ||
           DAT_007aadec == 0xd5 || DAT_007aadec == 0xd6 || DAT_007aadec == 0xd7 ||
           DAT_007aadec == 0xd8 || DAT_007aadec == 0xd9 || DAT_007aadec == 0xdc ||
           DAT_007aadec == 0xdb)
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
int player_can_stop_at_phase(int param_1, phase_t param_2)
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

  if (phase_stop_suppressed != 0)
  {
    return 0;
  }

  if (FUN_004af6f4() == 0)
  {
    return 0;
  }

  if (phase_response_window_open == 0)
  {
    return 0;
  }

  if (param_2 >= PHASE_BEGIN_UPKEEP && param_2 <= PHASE_UNKOWN5)
  {
    s.mapped_phase = PHASE_UPKEEP;
  }
  else if (param_2 >= PHASE_CLEANUP2 && param_2 <= PHASE_DAMAGE_PREVENTION)
  {
    s.mapped_phase = PHASE_CLEANUP2;
  }
  else
  {
    s.mapped_phase = param_2;
  }

  if ((g_duel_network_flags & 2) != 0)
  {
    if (current_player == param_1)
    {
      if ((active_player == param_1 &&
           (((int)(char)g_duel_phase_stop_settings[active_player].phase_flags[current_phase]) & PHASE_STOP_ENABLED) != 0) ||
          (other_player == param_1 &&
           (((int)(char)g_duel_phase_stop_settings[other_player].phase_flags[current_phase]) & PHASE_STOP_OPPONENT) != 0))
      {
        s.result = 1;
      }
    }
    else
    {
      if ((other_player == param_1 &&
           (((int)(char)g_duel_phase_stop_settings[active_player].phase_flags[current_phase]) & PHASE_STOP_OPPONENT) != 0) ||
          (active_player == param_1 &&
           (((int)(char)g_duel_phase_stop_settings[other_player].phase_flags[current_phase]) & PHASE_STOP_ENABLED) != 0))
      {
        s.result = 1;
      }
    }
  }
  else
  {
    if (stop_phase == s.mapped_phase)
    {
      s.result = 1;
    }

    if (stop_phase == -1 && active_player == param_1 &&
        (((int)(char)g_duel_phase_stop_settings[current_player].phase_flags[s.mapped_phase]) & PHASE_STOP_ENABLED) != 0)
    {
      s.result = 1;
    }
  }

  return s.result;
}

// FUNCTION: MAGIC 0x004e51a6
int get_recorded_action_count(void)
{
  return recorded_action_count;
}

// FUNCTION: MAGIC 0x004aec05
// FUNCTION: SHANDALAR 0x0046bbe0
unsigned int FUN_004aec05(int player)
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

  if (trigger_condition != -1 && current_turn == other_player)
  {
    for (s.other_player = 0; s.other_player < 2; ++s.other_player)
    {
      for (s.card = 0; (int)s.card < active_cards_count[s.other_player]; ++s.card)
      {
        if (is_in_play(s.other_player, (int)s.card) &&
            dispatch_card_trigger_event(s.other_player, (int)s.card, 0x7d, player) == 2)
        {
          unk_00742fcc = s.other_player;
          DAT_007ab2bc = 4;
          return s.card;
        }

        if (player == s.other_player &&
            (int)PLAYER_CARD_INSTANCE(s.other_player, (int)s.card).unknown0x14 == trigger_condition &&
            current_turn == s.other_player && trigger_condition != -1 &&
            PLAYER_CARD_INSTANCE(s.other_player, (int)s.card).internal_card_id != -1)
        {
          DAT_007ab2bc |= 4;
          ++DAT_00791410;
          unk_00742fcc = s.other_player;
          return s.card;
        }
      }
    }
  }

  if (((DAT_00789714 & 2) == 0) || (player == active_player) || (_DAT_00743020 == 0))
  {
    return 0xffffffffU;
  }

  unk_00742fcc = player;

  for (s.card = 0; (int)s.card < active_cards_count[player]; ++s.card)
  {
    s.internal_card_id = PLAYER_CARD_INSTANCE(player, (int)s.card).internal_card_id;
    if (s.internal_card_id != -1)
    {
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
  }

  if (DAT_007aadec == 4)
  {
    for (s.card = 0; (int)s.card < active_cards_count[1 - player]; ++s.card)
    {
      s.internal_card_id = PLAYER_CARD_INSTANCE(1 - player, (int)s.card).internal_card_id;
      if (s.internal_card_id != -1)
      {
        s.response_flags = (int)get_card_response_action_type(1 - player, (int)s.card);
        if (s.response_flags == 2)
        {
          unk_00742fcc = 1 - player;
          return s.card;
        }
      }
    }
  }

  if (DAT_007aadec == 4)
  {
    return 0xffffffffU;
  }

  s.candidates[s.num_candidates] = 0xffffffffU;
  ++s.num_candidates;

  if (g_duel_ai_mode_state == 1)
  {
    if (internal_rand(2) == 0 || get_recorded_action_count() == 0)
    {
      ai_recorded_choice = internal_rand(s.num_candidates);
    }
    else
    {
      ai_recorded_choice = s.num_candidates - 1;
    }

    if (ai_search_force_pass != 0)
    {
      ai_recorded_choice = s.num_candidates - 1;
      if (ai_search_force_pass == 1)
      {
        ai_search_force_pass = -1;
      }
    }

    ai_recorded_action = (((unsigned int)-(((PLAYER_CARD_INSTANCE(player, (int)s.candidates[ai_recorded_choice]).state & 2) == 0))) &
                    0xfffff000) +
                   0x2000;
    ai_recorded_action |= s.candidates[ai_recorded_choice];
    ai_recorded_action |= (((player == 0) - 1) & 0x100);

    ai_recorded_action_type = 4;
    record_ai_action_selection();
  }
  else
  {
    ai_recorded_action_type = 4;
    replay_ai_action_selection();

    if (s.num_candidates <= ai_recorded_choice)
    {
      ai_recorded_choice = s.num_candidates - 1;
    }
  }

  if (s.candidates[ai_recorded_choice] != 0xffffffffU)
  {
    if (DAT_008a8de4 > 0xf)
    {
      --DAT_008a8de4;
    }

    DAT_0091bf70[DAT_008a8de4] =
        PLAYER_CARD_INSTANCE(player, (int)s.candidates[ai_recorded_choice]).internal_card_id;
    DAT_007abc00[DAT_008a8de4] = s.candidates[ai_recorded_choice];
    ++DAT_008a8de4;
  }

  return s.candidates[ai_recorded_choice];
}

// FUNCTION: MAGIC 0x00440ad2
// FUNCTION: SHANDALAR 0x0040c352
int FUN_00440ad2(int player, int card, int arg_3)
{
  struct
  {
    int trace_counter;       /* iVar2 / local_324 */
    char dialog_prompt[300]; /* local_320 */
    char trace_text[500];    /* local_1f4 */
  } s;

  if ((g_duel_network_flags & 2) != 0)
  {
    s.trace_counter = duel_trace_counter;
    ++duel_trace_counter;
    sprintf(s.trace_text, "%d: Player #%d is processing %s(%d).\n",
            s.trace_counter, player,
            global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].name,
            card);
    append_to_trace_txt(s.trace_text);
  }

  unk_00777850 = 1;
  push_card_onto_stack(player, card, 0x7e, arg_3, 0);

  if (spell_fizzled == 1)
  {
    obliterate_top_card_of_stack();
    unk_00777850 = 0;
    return 0;
  }

  if (g_duel_ai_mode_state != 1)
  {
    set_stack_damage_targets();
    if (current_turn != active_player)
    {
      LoadTextSectionLines(global_ui_strings_filename, "PROMPT_PROC1");
      sprintf(s.dialog_prompt, text_lines[0], DAT_007a7c60);
      raw_do_dialog(player, card, -1, -1, s.dialog_prompt, 0);
    }
  }

  PLAYER_CARD_INSTANCE(player, card).state |= STATE_PROCESSING;
  resolve_top_card_on_stack();

  unk_00777850 = 0;
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
      ((global_cards_data[internal_card_id].extra_ability & EA_MANA_SOURCE) != 0 && produced_mana_color != -1);

  if (spell_fizzled == 1)
  {
    return 1;
  }

  if (!has_mana_source_and_color && unk_007a7c1c == 0)
  {
    sprintf(unk_00748770, gs_activate_0091b2d0, get_displayed_card_name(player, card));

    if (phase_stop_suppressed != 0 && (active_player == player || (g_duel_network_flags & 2) != 0))
    {
      response_player = -1;
    }
    else
    {
      response_player = -2;
    }

    allow_response(response_player, current_phase, unk_00748770, 0x6d);
  }

  resolve_top_card_on_stack();

  saved_trigger_cause_controller = trigger_cause_controller;
  saved_trigger_cause = trigger_cause;

  trigger_cause_controller = player;
  trigger_cause = card;
  dispatch_trigger_twice_once_with_each_player_as_reason(current_player, 0xd2, gs_tapping_00925d40, 0);

  trigger_cause_controller = saved_trigger_cause_controller;
  trigger_cause = saved_trigger_cause;

  return 1;
}

// FUNCTION: MAGIC 0x00440c61
// FUNCTION: SHANDALAR 0x0040c4e1
int FUN_00440c61(int player, int card)
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
    s.response_controller = active_player;
  }

  s.internal_card_id = PLAYER_CARD_INSTANCE(player, card).internal_card_id;

  if ((PLAYER_CARD_INSTANCE(player, card).state & STATE_IN_PLAY) != 0)
  {
    if ((PLAYER_CARD_INSTANCE(player, card).state & STATE_OUBLIETTED) != 0)
    {
      return 0;
    }

    if ((int)PLAYER_CARD_INSTANCE(player, card).unknown0x14 == trigger_condition)
    {
      if (trigger_condition != -1)
      {
        if (current_turn == player)
        {
          DAT_007ab2bc |= 4;
          DAT_00791410 += 1;
          return 2;
        }
        else
        {
          return 0;
        }
      }
    }

    if (trigger_condition != -1)
    {
      s.tmp = dispatch_card_trigger_event(player, card, 0x7d, player);
      if (s.tmp != 0)
      {
        DAT_007ab2bc |= 1 << ((char)s.tmp);
        DAT_00791410 += 1;

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
         (DAT_00742f68 & 0x10) != 0) ||
        ((global_cards_data[s.internal_card_id].extra_ability & 2) != 0 &&
         (DAT_00742f68 & 0x20) != 0))
    {
      if (DAT_00791418 != 0 &&
          (global_cards_data[s.internal_card_id].extra_ability & 2) == 0)
      {
        return 0;
      }

      if ((land_can_be_played & 4) == 0 ||
          ((land_can_be_played & 4) != 0 &&
           (global_cards_data[s.internal_card_id].extra_ability & 0x1004) != 0) ||
          (DAT_007aadec == 0xd3 &&
           (global_cards_data[s.internal_card_id].extra_ability & 0x4000) != 0))
      {
        unk_008b3270 &= ~2;

        if (((s.response_controller == player ||
              (_DAT_0064f0a8 & (int)(char)global_cards_data[s.internal_card_id].act_phases) != 0) &&
             (PLAYER_CARD_INSTANCE(player, card).state & STATE_INVISIBLE) == 0) &&
            dispatch_event_to_single_card(player, card, 0x73, 1 - player, -1) != 0)
        {

          if ((unk_008b3270 & 2U) != 0)
          {
            DAT_007ab2bc |= 4;
            return 2;
          }

          DAT_007ab2bc |= 2;
          return 3;
        }
      }
    }

    if (DAT_007aadec == 4 && (PLAYER_CARD_INSTANCE(player, card).upkeep_flags & 1) != 0)
    {
      unk_008b3270 |= 3;
      DAT_007ab2bc |= 4;
      return 2;
    }

    if (DAT_007aadec == 4 && player == unk_00742f60 &&
        (PLAYER_CARD_INSTANCE(player, card).upkeep_flags & 0x10) != 0 &&
        (PLAYER_CARD_INSTANCE(player, card).upkeep_flags & 0x88) == 0 &&
        can_pay_untap_cost(player, card) != 0)
    {
      DAT_007ab2bc |= 2;

      if (other_player == player && (g_duel_network_flags & 2) == 0)
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

    if (trigger_condition != -1 &&
        global_cards_data[s.internal_card_id].code_pointer != card_death_ward)
    {
      return 0;
    }

    if (DAT_00791418 != 0 && ((global_cards_data[s.internal_card_id].type & 0x20) == 0))
    {
      return 0;
    }

    if ((DAT_00742f68 & (unsigned char)global_cards_data[s.internal_card_id].type) != 0 &&
        can_pay_card_mana_cost(player, player, card) != 0 &&
        ((land_can_be_played & 4) == 0 ||
         ((land_can_be_played & 4) != 0 &&
          (global_cards_data[s.internal_card_id].extra_ability & 0x1004) != 0) ||
         (DAT_007aadec == 0xd3 &&
          (global_cards_data[s.internal_card_id].extra_ability & 0x2000) != 0)) &&
        ((s.response_controller == player ||
          (_DAT_0064f0a4 & (int)(char)global_cards_data[s.internal_card_id].act_phases) != 0) &&
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
int FUN_0044b7d4(int player, int card)
{
  unk_00938e2c = 0x3e;
  produced_mana_color = -1;

  dispatch_event_to_single_card(player, card, 0x6d, 1 - player, -1);

  if (spell_fizzled != 1)
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
    spell_fizzled = 0;
  }

  unk_00938e2c = 0;
  return produced_mana_color;
}

// FUNCTION: MAGIC 0x00445f61
// FUNCTION: SHANDALAR 0x004117fa
int FUN_00445f61(int player, int card)
{
  int internal_card_id;

  if (player == -1 || card == -1)
  {
    return 0;
  }

  internal_card_id = PLAYER_CARD_INSTANCE(player, card).internal_card_id;
  if (stack_proxy_internal_card_id == internal_card_id)
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
    int result;
    int who_pays; /* also who chooses in do_dialog */

    int was_tapped;
    int has_upkeep_costs;
    int upkeep_total;

    int upkeep_dialog_ai_choice;
    int upkeep_cost[7];

    int upkeep_uses_mana_source;

    char trace[500];
    char prompt[300];
    char upkeep_opt0[300];
    char upkeep_opt1[300];
    char upkeep_prompt[600];
  } s;

  int i;
  card_instance_t *instance;

  s.result = 1;

  instance = &PLAYER_CARD_INSTANCE(player, card);
  s.who_pays = player;

  if ((g_duel_network_flags & 2) != 0)
  {
    int trace_counter;

    trace_counter = duel_trace_counter;
    ++duel_trace_counter;
    sprintf(s.trace,
            "%d: Player #%d is tapping %s(%d).\n",
            trace_counter,
            player,
            global_cards_data[instance->internal_card_id].name,
            card);
    append_to_trace_txt(s.trace);
  }

  if (current_phase == EVENT_UPKEEP_PHASE)
  {
    s.who_pays = unk_00742f60;
  }

  if (s.who_pays == other_player && (g_duel_network_flags & 2) == 0 && g_duel_ai_mode_state != 1)
  {
    ai_recorded_action = -1;
    dispatch_event_to_single_card(player, card, EVENT_GET_SELECTED_CARD, 1 - player, -1);

    LoadTextSectionLines(global_ui_strings_filename, "PROMPT_TAP1");
    sprintf(s.prompt, text_lines[0], DAT_007a7c60);

    if (FUN_00445f61(player, card) != 0)
    {
      sprintf(s.prompt, text_lines[1], DAT_007a7c60, unk_00715fa8);
    }
    else if (((global_cards_data[instance->internal_card_id].extra_ability & (EA_INF_POWER | EA_INF_TOUGHNESS)) != 0) &&
             current_player == player && unk_00715fa8 != 0)
    {
      sprintf(s.prompt, text_lines[2], DAT_007a7c60, unk_00715fa8);
    }

    if (instance->number_of_targets == 0)
    {
      if (ai_recorded_action == -1)
      {
        raw_do_dialog(player, card, -1, -1, s.prompt, 0);
      }
      else
      {
        /* Unreachable in practice, but present in the original binary. */
        raw_do_dialog(player, card, (ai_recorded_action >> 8), (ai_recorded_action & 0xff), s.prompt, 0);
      }
    }
    else if (instance->number_of_targets == 1)
    {
      raw_do_dialog(player, card, instance->targets[0].player, instance->targets[0].card, s.prompt, 0);
    }
    else
    {
      raw_do_dialog(player, card, -1, -1, s.prompt, 0);
    }
  }

  push_card_onto_stack(player, card, EVENT_RESOLVE_ACTIVATION, player, 0);
  unk_007a7c1c = 0;

  if ((instance->upkeep_flags & UPKEEP_UPKEEP_TRIGGER) != 0)
  {
    s.has_upkeep_costs = 1;
    s.upkeep_total = 0;

    s.upkeep_cost[0] = (int)(char)instance->upkeep_colorless;
    s.upkeep_cost[1] = (int)(char)instance->upkeep_black;
    s.upkeep_cost[2] = (int)(char)instance->upkeep_blue;
    s.upkeep_cost[3] = (int)(char)instance->upkeep_green;
    s.upkeep_cost[4] = (int)(char)instance->upkeep_red;
    s.upkeep_cost[5] = (int)(char)instance->upkeep_white;
    s.upkeep_cost[6] = (int)(char)instance->upkeep_artmana;

    for (i = 0; i < 7; ++i)
    {
      mana_charge[i] = s.upkeep_cost[i];

      if (i > 0)
      {
        if (has_mana(player, (color_t)i, mana_charge[i]) == 0)
        {
          s.has_upkeep_costs = 0;
        }
      }

      s.upkeep_total += mana_charge[i];
    }

    if (has_mana(s.who_pays, COLOR_ANY, s.upkeep_total) == 0)
    {
      s.has_upkeep_costs = 0;
    }

    s.upkeep_dialog_ai_choice = (s.has_upkeep_costs == 0);

    load_text(global_ui_strings_filename, "PROMPT_PAYUPKEEP");
    strcpy(s.upkeep_opt0, text_lines[0]);
    strcpy(s.upkeep_opt1, text_lines[1]);
    sprintf(s.upkeep_prompt, " %s\n %s", s.upkeep_opt0, s.upkeep_opt1);

    if ((instance->upkeep_flags & UPKEEP_UPKEEP_NODIALOG) != 0 ||
        do_dialog(s.who_pays, player, card, -1, -1, s.upkeep_prompt, s.upkeep_dialog_ai_choice) == 0)
    {
      if (s.has_upkeep_costs == 0)
      {
        goto upkeep_cleanup;
      }

      charge_mana(s.who_pays, 0, 0);
      if (spell_fizzled == 0)
      {
        dispatch_event_to_single_card(player, card, EVENT_UPKEEP_PHASE, 1 - player, -1);
        if (last_single_card_event_result == 0)
        {
          instance->upkeep_flags |= UPKEEP_UPKEEP_UNPAID;
        }
      }
    }

    if (spell_fizzled != 0)
    {
      spell_fizzled = 0;
      obliterate_top_card_of_stack();
      return 0;
    }

  upkeep_cleanup:
    for (i = 0; i < 7; ++i)
    {
      mana_charge[i] = 0;
    }

    instance->upkeep_flags &= ~UPKEEP_UPKEEP_TRIGGER;
    instance->upkeep_flags |= UPKEEP_UPKEEP_CANTPAY;

    refresh_stack_proxy_card(1);
    return 1;
  }

  if ((instance->upkeep_flags & UPKEEP_UNTAP_TRIGGER) != 0 && can_pay_untap_cost(player, card) != 0)
  {
    for (i = 0; i < 7; ++i)
    {
      mana_charge[i] = (int)(char)instance->mana_to_untap[i];
    }

    charge_mana(player, 0, 0);
    if (spell_fizzled == 0)
    {
      dispatch_event_to_single_card(player, card, EVENT_UNTAP_PHASE, 1 - player, -1);
      if (last_single_card_event_result == 0)
      {
        instance->upkeep_flags |= UPKEEP_UNTAP_PAID;
      }
    }

    if (spell_fizzled != 0)
    {
      spell_fizzled = 0;
      obliterate_top_card_of_stack();
      return 0;
    }

    instance->upkeep_flags &= ~UPKEEP_UNTAP_TRIGGER;
    instance->upkeep_flags |= UPKEEP_UNTAP_DONE;
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
    if ((global_cards_data[instance->internal_card_id].extra_ability & EA_MANA_SOURCE) != 0)
    {
      produced_mana_color = -1;
    }

    begin_mana_payment_record();

    s.was_tapped = instance->state & 0x10;
    dispatch_event_to_single_card(player, card, EVENT_ACTIVATE, 1 - player, -1);

    if ((g_duel_network_flags & 2) != 0 && other_player == who_activates && DAT_007abc80 == 0 &&
        instance->internal_card_id != draw_card_placeholder_internal_card_id)
    {
      LoadTextSectionLines(global_ui_strings_filename, "PROMPT_TAP1");
      sprintf(s.prompt, text_lines[0], DAT_007a7c60);

      if (FUN_00445f61(player, card) != 0)
      {
        sprintf(s.prompt, text_lines[1], DAT_007a7c60, x_value);
      }
      else if (((global_cards_data[instance->internal_card_id].extra_ability & (EA_INF_POWER | EA_INF_TOUGHNESS)) != 0) &&
               current_player == player && x_value != 0)
      {
        sprintf(s.prompt, text_lines[2], DAT_007a7c60, x_value);
      }

      if (instance->number_of_targets == 1)
      {
        raw_do_dialog(player, card, instance->targets[0].player, instance->targets[0].card, s.prompt, 0);
      }
      else
      {
        raw_do_dialog(player, card, -1, -1, s.prompt, 0);
      }
    }

    if (spell_fizzled == 1)
    {
      restore_recorded_mana_to_pool(player);
      obliterate_top_card_of_stack();
      s.result = 0;
    }

    end_mana_payment_record();

    if (s.result != 0)
    {
      if (s.was_tapped == 0 && (instance->state & 0x10) != 0)
      {
        dispatch_event(player, card, EVENT_TAP_CARD);
      }
      else
      {
        dispatch_event(player, card, EVENT_PLAY_ABILITY);
      }

      i = instance->internal_card_id;
      if (i == -1)
      {
        i = instance->original_internal_card_id;
      }
      s.upkeep_uses_mana_source = global_cards_data[i].extra_ability & EA_MANA_SOURCE;

      if (s.upkeep_uses_mana_source == 0 || produced_mana_color == -1)
      {
        refresh_stack_proxy_card(1);
      }
      else
      {
        refresh_stack_proxy_card(0);
      }

      if (g_duel_ai_mode_state != 1)
      {
        if ((global_cards_data[instance->internal_card_id].extra_ability & EA_MANA_SOURCE) != 0 &&
            produced_mana_color != -1)
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
    unk_007a7c1c = 0;
  }

  return s.result;
}

// FUNCTION: MAGIC 0x0050aa68
void FUN_0050aa68(void)
{
  // This space intentionally left blank (empty function)
}

// FUNCTION: MAGIC 0x00446cb8
void FUN_00446cb8(char *text)
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

// FUNCTION: MAGIC 0x0050aa5d
void FUN_0050aa5d(void)
{
  // This space intentionally left blank (empty function)
}

// FUNCTION: MAGIC 0x004b5fc9
// FUNCTION: SHANDALAR 0x0056c7cb
void FUN_004b5fc9(char *text)
{
  if (g_duel_ai_mode_state == 1)
    return;

  FUN_0050aa68();
  FUN_00446cb8(text);
  text[0] = '\0';
  FUN_0050aa5d();
}

// FUNCTION: MAGIC 0x00444d48
// FUNCTION: SHANDALAR 0x004105e0
int allow_response(int param_1, int param_2, char *prompt, int param_4)
{
  int saved_allow_response_flag = phase_stop_suppressed;

  if (stack_size == 0)
  {
    if (should_skip_phase(current_player) != 0 && param_1 != -2 && g_duel_ai_mode_state != 1)
    {
      phase_stop_suppressed = 1;
    }
  }

  {
    int result;

    do
    {
      response_action_taken = 0;
      result = allow_response_once(param_1, param_2, prompt, param_4);
    } while (response_action_taken != 0 && stack_size <= 0);

    if (stack_size == 0)
    {
      if (g_duel_ai_mode_state != 1)
      {
        phase_stop_suppressed = 0;
      }
      {
        g_duel_phase_stop_settings[current_player].phase_flags[current_phase] = (int)(char)g_duel_phase_stop_settings[current_player].phase_flags[current_phase] & ~PHASE_STOP_SUPPRESSED;
      }
    }
    else if (g_duel_ai_mode_state != 1)
    {
      phase_stop_suppressed = saved_allow_response_flag;
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
    int tmp;                      // scratch/padding (kept at the start so other offsets stay stable)
    int pad;                      // unused/padding
    char prompt_format[300];      // local_29c
    unsigned int selected_card;   // local_170
    int saved_unk_00742f60;       // local_16c
    unsigned int maybe_card;      // local_168
    int some_flag;                // local_164
    unsigned int response_code;   // local_160
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

  if (life[active_player] <= 0)
  {
    g_duel_phase_stop_settings[current_player].phase_flags[current_phase] =
        (unsigned char)(((int)(char)g_duel_phase_stop_settings[current_player].phase_flags[current_phase]) | PHASE_STOP_SUPPRESSED);
  }

  s.interrupts_available = 0;
  if (g_duel_ai_mode_state != 1 && stop_phase_player == current_player && current_phase == stop_phase)
  {
    phase_stop_suppressed = 0;
  }

  s.can_respond = player_can_stop_at_phase(reason_for_trigger_controller, current_phase);

  strcpy(s.prompt_copy, prompt);

  s.saved_land_can_be_played = land_can_be_played;
  s.saved_unk_00742f60 = unk_00742f60;
  s.selected_player = reason_for_trigger_controller;
  unk_00742f60 = s.selected_player;
  s.player = unk_00742f60;

  _DAT_00742fbc = 1;
  DAT_00742f68 = _DAT_00743020 & 0x30;
  _DAT_00743088 = 0;

  ++DAT_007161d0;
  if (DAT_00742f64 > DAT_007161d0)
  {
    DAT_00742f64 = 0;
  }

  if ((g_duel_network_flags & 2) == 0)
  {
    _DAT_0064f0a4 = 2;
    _DAT_0064f0a8 = 0x20;

    if (current_phase >= PHASE_DECLARE_BLOCKERS)
    {
      _DAT_0064f0a4 = 4;
      _DAT_0064f0a8 = 0x40;
    }
    if (current_phase <= PHASE_MAIN1)
    {
      _DAT_0064f0a4 = 1;
      _DAT_0064f0a8 = 0x10;
    }
    if (current_phase >= PHASE_MAIN2)
    {
      _DAT_0064f0a4 = 8;
      _DAT_0064f0a8 = (int)0xffffff80;
    }
    if (current_phase == PHASE_DISCARD)
    {
      _DAT_0064f0a4 = 0xf;
      _DAT_0064f0a8 = (int)0xfffffff0;
    }
    if (other_player == reason_for_trigger_controller && global_stack_cards[0].player == active_player)
    {
      _DAT_0064f0a4 = 0xf;
      _DAT_0064f0a8 = (int)0xfffffff0;
    }
  }
  else
  {
    _DAT_0064f0a4 = 0xf;
    _DAT_0064f0a8 = (int)0xfffffff0;
  }

  if ((g_duel_network_flags & 2) == 0)
  {
    if (g_duel_ai_mode_state == 1 || ai_action_replay_available != 0 ||
        (trigger_condition != -1 && current_turn == other_player) || DAT_007aadec == 4 ||
        (other_player == unk_00742f60 && (land_can_be_played & 0x200) != 0))
    {
      s.selected_card = FUN_004aec05(s.player);
      s.selected_player = unk_00742fcc;
    }
    else
    {
      s.selected_card = 0xffffffffU;
    }

    s.result_flags = 0;

    if (s.selected_card != 0xffffffffU &&
        dispatch_card_trigger_event(s.selected_player, (int)s.selected_card, 0x7d, s.player) == 2)
    {
      FUN_00440ad2(s.selected_player, (int)s.selected_card, s.player);
      s.selected_card = 0xffffffffU;
      TENTATIVE_reassess_all_cards(0, 0xff);
    }

    if (s.selected_card != 0xffffffffU)
    {
      s.internal_card_id = PLAYER_CARD_INSTANCE(s.selected_player, (int)s.selected_card).internal_card_id;
      _DAT_00743030 = s.internal_card_id;

      if (is_in_play(s.selected_player, (int)s.selected_card))
      {
        if (PLAYER_CARD_INSTANCE(s.selected_player, (int)s.selected_card).unknown0x14 == (unsigned int)trigger_condition &&
            pending_killed_card_handler != NULL && trigger_condition != -1)
        {
          pending_killed_card_handler(s.selected_player, (int)s.selected_card);
        }
        else
        {
          dispatch_event_to_single_card(s.selected_player, (int)s.selected_card, 0x73, 1 - s.selected_player, -1);
          if (activate(s.player, s.selected_player, (int)s.selected_card))
          {
            resolve_activated_ability(s.selected_player, (int)s.selected_card);
          }
          spell_fizzled = 0;
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
            DAT_00742f68 = _DAT_00743020 & 0x20;
          }

          s.result_flags = 1;
          TENTATIVE_reassess_all_cards(0, 0xff);

          if (g_duel_ai_mode_state != 1 && internal_rand(3) == 0)
          {
            FUN_004b5fc9("Didn't expect that, did ya?");
          }
        }
      }
    }
  }
  else
  {
    s.result_flags = 0;
  }

  s.response_mask = 0;
  s.num_candidates = 0;
  s.maybe_card = 0xffffffffU;

  if (g_duel_ai_mode_state != 1 ||
      (trigger_condition != -1 &&
       (current_turn == active_player || (current_turn == other_player && (g_duel_network_flags & 2) != 0))))
  {
    if (((current_turn == active_player) || (current_turn == other_player && (g_duel_network_flags & 2) != 0)) &&
        trigger_condition != -1)
    {
      for (s.player = 0; s.player < 2; ++s.player)
      {
        for (s.card = 0; (int)s.card < active_cards_count[s.player]; ++s.card)
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
                s.response_mask |= 1U << ((unsigned char)s.response_code & 0x1f);
              }
            }
          }

          if ((DAT_00789714 & 1) != 0 &&
              PLAYER_CARD_INSTANCE(s.player, (int)s.card).unknown0x14 == (unsigned int)trigger_condition &&
              current_turn == s.player && trigger_condition != -1 &&
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
        if (trigger_condition == 0xca && (char)g_duel_phase_stop_settings[current_player].phase_flags[PHASE_UPKEEP] == 0)
        {
          DAT_00789714 = 0;
        }
        if (trigger_condition == 0xce && (char)g_duel_phase_stop_settings[current_player].phase_flags[PHASE_DRAW] == 0)
        {
          DAT_00789714 = 0;
        }
      }
    }

    if (s.maybe_card != 0xffffffffU || g_duel_ai_mode_state != 1)
    {
      if (s.maybe_card != 0xffffffffU || s.response_mask != 0 || ((DAT_00789714 & 1) != 0 && _DAT_00743020 != 0))
      {
        if ((g_duel_network_flags & 2) == 0)
        {
          s.player = active_player;
        }
        else
        {
          s.player = reason_for_trigger_controller;
        }

        unk_008b3270 = 0;
        DAT_00789714 = 0;
        s.some_flag = 0;

        for (s.card = 0; (int)s.card < active_cards_count[s.player]; ++s.card)
        {
          if (PLAYER_CARD_INSTANCE(s.player, (int)s.card).internal_card_id != -1 &&
              ((PLAYER_CARD_INSTANCE(s.player, (int)s.card).upkeep_flags & 1) != 0 ||
               (PLAYER_CARD_INSTANCE(s.player, (int)s.card).upkeep_flags & 0x10) != 0 ||
               (!is_nonactivated_mana_source(s.player, (int)s.card) && s.player == unk_00742f60)))
          {
            s.response_code = get_card_response_action_type(s.player, (int)s.card);

            if (((s.response_code >= 2) ||
                 (((DAT_00777aa8 != 0 || s.player != current_player) && (s.response_code & 2) != 0) ||
                  (unk_008b3270 & 2) != 0)) &&
                ((current_turn != s.player || trigger_condition == -1) ||
                 (s.response_code != 2 || (unk_008b3270 & 2) != 0)))
            {
              if ((unk_008b3270 & 2) == 0 && s.response_code != 2)
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

              unk_008b3270 &= ~2;
            }
          }
        }

        if (DAT_007aadec == 4)
        {
          for (s.card = 0; (int)s.card < active_cards_count[1 - s.player]; ++s.card)
          {
            if (PLAYER_CARD_INSTANCE(1 - s.player, (int)s.card).internal_card_id != -1 &&
                ((PLAYER_CARD_INSTANCE(1 - s.player, (int)s.card).upkeep_flags & 1) != 0 ||
                 (PLAYER_CARD_INSTANCE(1 - s.player, (int)s.card).upkeep_flags & 0x10) != 0 ||
                 !is_nonactivated_mana_source(1 - s.player, (int)s.card)))
            {
              s.response_code = get_card_response_action_type(1 - s.player, (int)s.card);
              if ((unk_008b3270 & 2) != 0)
              {
                s.maybe_card = s.card;
                s.maybe_player = 1 - s.player;
                ++s.num_candidates;

                unk_008b3270 &= ~2;

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

    if ((DAT_00742f68 & 0x10) == 0 || unk_008ce508 != -1)
    {
      if ((DAT_00742f68 & 0x20) != 0)
      {
        if (DAT_007a7d78 != 0)
        {
          DAT_007a7d78 = 1;
        }
        strcpy(s.prompt_format, gs_interrupts_0093a850);
      }
    }
    else
    {
      strcpy(s.prompt_format, gs_fast_effects_00926520);
    }

    if (trigger_condition != -1)
    {
      strcpy(s.prompt_format, gs_triggered_effects_009396a0);
    }

    sprintf(unk_00748770, s.prompt_format, s.prompt_copy);

    if (phase_stop_suppressed == 0 || (s.response_mask & 2) != 0)
    {
      if (((s.can_respond == 0 && (DAT_00777aa8 == 0 || trigger_condition != -1)) &&
           (s.interrupts_available == 0 || trigger_condition == -1) &&
           (DAT_00561390 == 0 || (s.response_mask & 6) == 0)) &&
          (s.num_candidates <= (int)((s.response_mask & 6) == 0) ||
           ((s.some_flag == 0 && should_skip_phase(reason_for_trigger_controller) != 0) || trigger_condition == 0xd6 ||
            (s.maybe_card != 0xffffffffU && DAT_00742f64 == DAT_007161d0))))
      {
        s.selected_card = s.maybe_card;
        s.selected_player = s.maybe_player;
        _DAT_00743088 = 1;
      }
      else
      {
        if (g_duel_ai_mode_state != 1)
        {
          phase_was_skipped = 1;
        }

        DAT_00742f64 = 0;

        s.tmp = 0;
        while (!s.tmp)
        {
          if (g_duel_ai_mode_state == 1)
          {
            s.selected_card = s.maybe_card;
            s.tmp = 1;
            g_target_selection_status_code = -1;
          }
          else
          {
            if ((g_duel_network_flags & 2) == 0)
            {
              s.player = active_player;
            }
            else
            {
              s.player = reason_for_trigger_controller;
            }

            if (active_player == s.player)
            {
              stop_phase_player = -1;
              stop_phase = -1;
            }
            else
            {
              previous_stop_phase_player = -1;
              previous_stop_phase = -1;
            }

            s.selected_card = select_card_for_action(s.player, -1, s.player, 0xff, 0, unk_00748770, 2);
            s.selected_player = unk_00742fcc;
            if ((int)s.selected_card >= 0)
            {
              g_duel_phase_stop_settings[current_player].phase_flags[current_phase] =
                  (unsigned char)(((int)(char)g_duel_phase_stop_settings[current_player].phase_flags[current_phase]) | PHASE_STOP_SUPPRESSED);
            }
          }

          if (g_target_selection_status_code == -3)
          {
            s.tmp = 0;
          }
          else if (g_target_selection_status_code == -2)
          {
            s.tmp = 1;
            s.selected_card = 0xffffffffU;

            DAT_007ab2bc &= ~2;
            if (trigger_condition != -1)
            {
              g_target_selection_status_code = 0;
            }

            if (s.num_candidates != 0)
            {
              DAT_00742f64 = DAT_007161d0;
              _DAT_00743088 = 1;
              if (g_duel_ai_mode_state != 1)
              {
                phase_stop_suppressed = 1;
              }
            }
          }
          else if (g_target_selection_status_code == 0)
          {
            if (s.selected_player != -1 && s.selected_card != 0xffffffffU)
            {
              s.tmp = 1;
            }
            else if (s.selected_player != -1 && s.selected_card == 0xffffffffU)
            {
              s.tmp = 0;
            }
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
        _DAT_00743088 = 1;
      }
    }

    /* original uses an empty string at 0x00573048 */
    unk_00748770[0] = '\0';

    if ((g_duel_network_flags & 2) == 0)
    {
      s.player = active_player;
    }
    else
    {
      s.player = reason_for_trigger_controller;
    }

    if (s.selected_card != 0xffffffffU &&
        (s.selected_player == s.player ||
         (((PLAYER_CARD_INSTANCE(s.selected_player, (int)s.selected_card).state & 2) != 0 &&
           dispatch_card_trigger_event(s.selected_player, (int)s.selected_card, 0x7d, s.player) != 0) ||
          DAT_007aadec == 4)))
    {
      s.internal_card_id = PLAYER_CARD_INSTANCE(s.selected_player, (int)s.selected_card).internal_card_id;

      if (get_card_response_action_type(s.selected_player, (int)s.selected_card) != 0)
      {
        if (is_in_play(s.selected_player, (int)s.selected_card))
        {
          if (dispatch_card_trigger_event(s.selected_player, (int)s.selected_card, 0x7d, s.player) != 0)
          {
            FUN_00440ad2(s.selected_player, (int)s.selected_card, s.player);
          }
          else
          {
            if (PLAYER_CARD_INSTANCE(s.selected_player, (int)s.selected_card).unknown0x14 == (unsigned int)trigger_condition &&
                pending_killed_card_handler != NULL && trigger_condition != -1)
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

                if (spell_fizzled != 1 && g_duel_ai_mode_state != 1)
                {
                  play_sound_effect(0x26);
                }
              }

              spell_fizzled = 0;
            }
          }
        }
        else
        {
          resolve_cast_card(s.selected_player, (int)s.selected_card);
          if (global_cards_data[s.internal_card_id].type == 0x20)
          {
            DAT_00742f68 = _DAT_00743020 & 0x20;
          }

          if (internal_rand(3) == 0)
          {
            FUN_004b5fc9("I knew that was coming.");
          }
        }

        TENTATIVE_reassess_all_cards(0, 0xff);
        s.result_flags |= 2;
      }
      else
      {
        if (can_activate_mana_source_for_stop_prompt(s.selected_player, (int)s.selected_card) != 0 && s.selected_player == reason_for_trigger_controller)
        {
          FUN_0044b7d4(s.selected_player, (int)s.selected_card);
        }
      }

      s.result_flags |= 2;
    }

    DAT_00789714 = 1;
    _DAT_00743088 = 0;
  }

  if (s.result_flags == 0)
  {
    DAT_00742f68 = _DAT_00743020 & 0x30;
  }

  _DAT_00742fbc = 0;
  unk_00742f60 = s.saved_unk_00742f60;
  land_can_be_played = s.saved_land_can_be_played;
  --DAT_007161d0;

  return (int)s.result_flags;
}

// FUNCTION: MAGIC 0x004e1c8c
void FUN_004e1c8c(unsigned int internal_card_id, int unk1, char *prompt, int unk2)
{
  (void)internal_card_id;
  (void)unk1;
  (void)prompt;
  (void)unk2;
}

// FUNCTION: MAGIC 0x00435c1f
// FUNCTION: SHANDALAR 0x004170f8
int begin_mana_payment_record(void)
{
  int color;

  if (mana_payment_record_depth < 10)
  {
    for (color = 0; color < 7; ++color)
    {
      recorded_mana_payments[mana_payment_record_depth][color] = 0;
    }
    ++mana_payment_record_depth;
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
  if (mana_payment_record_depth > 0)
  {
    --mana_payment_record_depth;
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

  if (mana_payment_record_depth > 0)
  {
    for (color = 0; color < 7; ++color)
    {
      raw_mana_available[player][color] += recorded_mana_payments[mana_payment_record_depth - 1][color];
      raw_mana_available[player][7] += recorded_mana_payments[mana_payment_record_depth - 1][color];
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
int refresh_stack_proxy_card(int param_1)
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

  s.stack_index = stack_size - 1;
  s.player = global_stack_cards[s.stack_index].player;
  s.card = global_stack_cards[s.stack_index].card;

  if (PLAYER_CARD_INSTANCE(s.player, s.card).internal_card_id == stack_proxy_internal_card_id)
  {
    s.saved_parent_card = PLAYER_CARD_INSTANCE(s.player, s.card).parent_card;
    s.saved_parent_controller = PLAYER_CARD_INSTANCE(s.player, s.card).parent_controller;
    s.saved_timestamp = PLAYER_CARD_INSTANCE(s.player, s.card).timestamp;

    memcpy(global_card_instances[s.player] + s.card,
           global_card_instances[s.saved_parent_controller] + s.saved_parent_card,
           300);

    PLAYER_CARD_INSTANCE(s.player, s.card).internal_card_id = stack_proxy_internal_card_id;
    PLAYER_CARD_INSTANCE(s.player, s.card).unknown0x14 = 0;
    PLAYER_CARD_INSTANCE(s.player, s.card).kill_code = 0;
    PLAYER_CARD_INSTANCE(s.player, s.card).state |= 2;

    PLAYER_CARD_INSTANCE(s.player, s.card).parent_controller = s.saved_parent_controller;
    PLAYER_CARD_INSTANCE(s.player, s.card).parent_card = s.saved_parent_card;
    PLAYER_CARD_INSTANCE(s.player, s.card).timestamp = s.saved_timestamp;

    if (PLAYER_CARD_INSTANCE(s.saved_parent_controller, s.saved_parent_card).internal_card_id != -1)
    {
      PLAYER_CARD_INSTANCE(s.player, s.card).original_internal_card_id =
          PLAYER_CARD_INSTANCE(s.saved_parent_controller, s.saved_parent_card).internal_card_id;
    }

    if ((int)PLAYER_CARD_INSTANCE(s.player, s.card).original_internal_card_id < unk_009266a4 ||
        (unk_009266a4 + 0x2d) <= (int)PLAYER_CARD_INSTANCE(s.player, s.card).original_internal_card_id)
    {
      unsigned int packed_id;

      packed_id =
          (unsigned int)global_cards_data[PLAYER_CARD_INSTANCE(s.saved_parent_controller, s.saved_parent_card)
                                              .original_internal_card_id]
              .id;
      PLAYER_CARD_INSTANCE(s.player, s.card).display_pic_csv_id = (unsigned short)packed_id;
      PLAYER_CARD_INSTANCE(s.player, s.card).display_pic_num = (unsigned short)(packed_id >> 16);
    }
  }

  if (g_duel_ai_mode_state != 1)
  {
    stack_prompt_flags[s.stack_index] = param_1;
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

  for (s.stack_index = 0; s.stack_index < stack_size; ++s.stack_index)
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
int count_colored_cards_in_hand(int param_1)
{
  struct
  {
    int count;
    int card;
    int internal_card_id;
  } s;

  s.count = 0;
  for (s.card = 0; s.card < active_cards_count[param_1]; ++s.card)
  {
    s.internal_card_id = PLAYER_CARD_INSTANCE(param_1, s.card).internal_card_id;
    if (s.internal_card_id != -1)
    {
      if ((PLAYER_CARD_INSTANCE(param_1, s.card).state & 2) == 0)
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

int FUN_00496128(int windows, int count);
int FUN_004961ab(HWND param_1, int param_2, int param_3);

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

  extern int DAT_007a7d74;

  s.windows[7] = (HWND)DAT_007a7d74;
  s.windows[6] = g_duel_prompt_context_hwnd;
  s.windows[5] = (HWND)g_duel_phase_display_window_hwnd;
  s.windows[4] = (HWND)g_duel_attack_phase_window_hwnd;
  s.windows[3] = DAT_0094ca30;
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
    s.found_index = FUN_00496128((int)&s.windows[0], s.count);
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

      FUN_004961ab(s.windows[s.found_index], (int)s.next, s.count - (s.found_index + 1));
    }
  }
}

// FUNCTION: MAGIC 0x00496128
int FUN_00496128(HWND *windows, int count)
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
int FUN_004961ab(HWND param_1, HWND *param_2, int param_3)
{
  struct
  {
    HWND *next; // ebp-0x8
    int i;      // ebp-0x4
  } s;

  if (param_2 == 0 || param_3 <= 0)
  {
    return 0;
  }

  s.i = FUN_00496128(param_2, param_3);
  if (s.i != -1)
  {
    SetWindowPos(param_2[s.i], param_1, 0, 0, 0, 0, 3);

    if (s.i + 1 >= param_3)
    {
      s.next = (HWND *)0;
    }
    else
    {
      s.next = &param_2[s.i + 1];
    }

    FUN_004961ab(param_2[s.i], (int)s.next, param_3 - (s.i + 1));
    return 1;
  }
  else
  {
    return 0;
  }
}

// FUNCTION: MAGIC 0x00447f59
int FUN_00447f59(void)
{
  unk_0093f4b4 = GetTickCount();
  g_duel_tick_adjustment = 0;
  return 0;
}

// FUNCTION: MAGIC 0x004e1b84
int FUN_004e1b84(void)
{
  return FUN_00447f59();
}

// FUNCTION: MAGIC 0x004a7c4a
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
void update_duel_thread_time_marker(void)
{
  DAT_0064ee98 = get_duel_thread_time_ms();
}

// FUNCTION: MAGIC 0x004a3662
void show_mana_burn_dialog(int player, int amount)
{
  struct
  {
    int player;
    int amount;
  } s;

  if (g_duel_ai_mode_state != 1)
  {
    s.player = player;
    s.amount = amount;
    DialogBoxParamA(g_app_instance, (LPCSTR)0xf3, g_duel_window_hwnd, dlgproc_mana_burn, (LPARAM)&s.player);
  }
}

// FUNCTION: MAGIC 0x004a36ac
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
    g_mana_burn_dialog_data = (int *)lparam;
    sprintf(s.path, "%s\\WINBK_ManaBurn.pic", global_duelart_path);
    g_mana_burn_dialog_background = load_pic(s.path);
    g_mana_burn_dialog_text_color = 0x100009a;
    g_mana_burn_dialog_shadow_color = 0x10000c9;
    load_text(global_ui_strings_filename, "DIALOG_MANABURN");
    SetDlgItemTextA(hwnd, 0x4bb, text_lines[0]);
    ShowWindow(GetDlgItem(hwnd, 0x4bb), 0);
    ShowWindow(GetDlgItem(hwnd, 0x4ba), 0);
    SetTimer(hwnd, 1, 3000, NULL);
    return 1;

  case 0x30f:
  case 0x310:
  case 0x311:
    return FUN_10025b5e((int)hwnd, msg, (int)wparam, lparam);

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
    if (g_mana_burn_dialog_data[0] == 1)
    {
      copy_opponent_name_prefix(s.message_text);
      sprintf(s.title_text, text_lines[1], s.message_text, g_mana_burn_dialog_data[1]);
    }
    else
    {
      sprintf(s.title_text, text_lines[2], g_mana_burn_dialog_data[1]);
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
int prompt_for_life_total(int player, char *prompt, int maxnum)
{
  struct
  {
    char *prompt;
    int maxnum;
    int show_reset_button;
  } s;
  int result;

  if (player != 0)
  {
    return maxnum;
  }
  else
  {
    s.prompt = prompt;
    s.maxnum = maxnum;
    s.show_reset_button = 0;
    result = DialogBoxParamA(g_app_instance, (LPCSTR)0xdc, g_duel_window_hwnd, dlgproc_prompt_for_life_total, (LPARAM)&s.prompt);
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
    HWND context;
  } s;

  switch (msg)
  {
  case WM_INITDIALOG:
    s.context = (HWND)lparam;
    SetWindowLongA(hwnd, 8, ((int *)s.context)[1]);
    setup_life_total_dialog_resources(&g_life_total_dialog_background,
                                      &g_life_total_dialog_text_color,
                                      &g_life_total_dialog_button_brush,
                                      &g_life_total_dialog_pen1,
                                      &g_life_total_dialog_pen2,
                                      &g_life_total_dialog_unfocus_text_color,
                                      &g_life_total_dialog_focus_text_color);
    SetDlgItemTextA(hwnd, IDOK, gs_ok_00924800);
    SetDlgItemTextA(hwnd, IDCANCEL, gs_cancel_008a8c20);
    SetDlgItemTextA(hwnd, 0x44d, (LPCSTR)((int *)s.context)[0]);
    if (((int *)s.context)[2] == 0)
    {
      ShowWindow(GetDlgItem(hwnd, 0x44e), 0);
    }
    SendMessageA(hwnd, 0x401, IDOK, 0);
    SetDlgItemInt(hwnd, 0x44c, ((int *)s.context)[1], 0);
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
    FUN_004955ae(s.draw_item,
                 g_life_total_dialog_button_brush,
                 g_life_total_dialog_pen1,
                 g_life_total_dialog_pen2,
                 s.text_color,
                 0);
    return 1;

  case WM_QUERYNEWPALETTE:
  case WM_PALETTEISCHANGING:
  case WM_PALETTECHANGED:
    return FUN_10025b5e((int)hwnd, msg, (int)wparam, lparam);

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
