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
void save_ai_search_state(void);
void restore_ai_search_state(void);
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
int get_ai_search_elapsed_time(void);
void delete_and_close_object(HANDLE obj);
HBITMAP load_pic(char *filename);
void ApplyCardArtPaletteToDc(HDC hdc);
BOOL DrawBitmapToRect(HDC dst_dc, const RECT *dst_rect, HBITMAP bitmap);
extern int DAT_00743098;
extern int DAT_00712544;
void __cdecl _assert(void *expr, void *filename, unsigned line);
void change_buttonclass_wndproc(HWND hwnd);
void save_duel_interface_options_to_registry(void);
int ai_opinion_of_gamestate_continued(int player, int score);
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

typedef struct
{
  card_instance_t card_instances[2][151];
  card_data_t extra_card_data[16];
  int library[2][500];
  int graveyard[2][500];
  int exile[2][500];
  int special_mana[2][8];
  int raw_mana[2][8];
  int basic_lands[2][8];
  int raw_mana_hex[2][51];
  int life_totals[2];
  int DAT_007abce0_pair[2];
  unsigned int card_types[2];
  int graveyard_type_flags[2];
  unsigned char DAT_008cfd70_copy[0x60];
  int ante_cards[2][16];
  int timestamp_player[500];
  int timestamp_card[500];
  int land_play_flags;
  phase_t phase;
  int current_casting_internal_card_id_value;
  int combat_count;
  int spell_fizzled_value;
  int stack_events[32];
  target_t stack_cards[32];
  target_t stack_damage_targets[32];
  int active_counts[2];
  int stack_count;
  int(__cdecl *DAT_007ab2cc_value)(int, int);
  int DAT_0093a848_value;
  int damage_matrix[2][151][2][4];
  int cost_mod[8];
  int mana_charge[8];
  int x_value_copy;
  int max_x_value_copy;
  int extra_turn_player;
  phase_t stop_phase_copy;
  int stop_phase_player_copy;
  int battlefield_flags;
  int unk_008b44d0_copy[2];
  int unk_007a79b0_copy[2];
} AiSearchBackup;

static AiSearchBackup g_ai_search_backup;

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
      (((int)(char)g_duel_phase_stop_settings[human_player].phase_flags[phase] & (PHASE_STOP_ENABLED | PHASE_STOP_OPPONENT)) != 0 ||
       (phase == stop_phase && human_player == stop_phase_player) ||
       (phase == previous_stop_phase && human_player == previous_stop_phase_player)))
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
      if (nonactive_player == human_player)
      {
        if (((int)(char)g_duel_phase_stop_settings[human_player].phase_flags[phase] & PHASE_STOP_ENABLED) != 0 ||
            (phase == stop_phase && human_player == stop_phase_player))
        {
          prompt_player_for_stop_action(nonactive_player, g_prompt_stop_phase_anyway_text);
        }
        if (((int)(char)g_duel_phase_stop_settings[human_player].phase_flags[phase] & PHASE_STOP_OPPONENT) != 0 ||
            (phase == previous_stop_phase && human_player == previous_stop_phase_player))
        {
          prompt_player_for_stop_action(active_player, g_prompt_stop_phase_anyway_text);
        }
      }
      else
      {
        if (((int)(char)g_duel_phase_stop_settings[human_player].phase_flags[phase] & PHASE_STOP_OPPONENT) != 0 ||
            (phase == previous_stop_phase && human_player == previous_stop_phase_player))
        {
          prompt_player_for_stop_action(active_player, g_prompt_stop_phase_anyway_text);
        }
        if (((int)(char)g_duel_phase_stop_settings[human_player].phase_flags[phase] & PHASE_STOP_ENABLED) != 0 ||
            (phase == stop_phase && human_player == stop_phase_player))
        {
          prompt_player_for_stop_action(nonactive_player, g_prompt_stop_phase_anyway_text);
        }
      }
    }
    else
    {
      prompt_player_for_stop_action(nonactive_player, g_prompt_stop_phase_anyway_text);
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
    if (nonactive_player == human_player)
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
void resolve_mana_burn(void)
{
  int current_card;
  int player;

  if ((g_duel_network_flags & 2) == 0)
  {
    while (raw_mana_available[active_player][7] != 0 && current_phase > 0x1a)
    {
      current_card = 0;
      while (current_card < active_cards_count[active_player])
      {
        if (is_in_play(active_player, current_card) != 0)
        {
          dispatch_event_to_single_card(active_player, current_card, 0x8f, 1 - active_player, -1);
          if (DAT_008cf1b8 != 0)
          {
            break;
          }
        }
        current_card = current_card + 1;
      }
      if (active_cards_count[active_player] <= current_card)
      {
        goto burn_remaining_mana;
      }
      if (activate(active_player, active_player, current_card) != 0)
      {
        resolve_activated_ability(active_player, current_card);
      }
    }
  }

burn_remaining_mana:
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
int allow_response_once(int param_1, int param_2, char *param_3, int param_4)
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

  if (param_1 == -2 && should_skip_phase(human_player) == 0)
  {
    DAT_00561390 = 1;
  }
  else
  {
    DAT_00561390 = 0;
  }

  if ((g_duel_network_flags & 2) == 0)
  {
    if (should_skip_phase(human_player) == 0 &&
        ((((int)(char)g_duel_phase_stop_settings[human_player].phase_flags[param_2] & PHASE_STOP_ENABLED) != 0) ||
         (human_player == stop_phase_player && stop_phase == param_2)))
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

  strcpy(s.prompt_copy, param_3);

  if (current_phase == PHASE_UPKEEP && trigger_dispatch_depth == 1)
  {
    unk_00742f60 = human_player;
    FUN_004460d3();
  }

  while (1)
  {
  restart_response_window:
    do
    {
      DAT_00777aa8 = FUN_0044541f(1);

      if ((g_duel_network_flags & 2) != 0)
      {
        DAT_00789714 = 1;
      }
      else
      {
        if (human_player != 0)
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

      s.dispatch_result = (unsigned int)process_response_actions(human_player, s.prompt_copy);
      if (s.dispatch_result != 0)
      {
        response_action_taken = 1;
      }

      if (((nonactive_player == human_player) ||
           (active_player == human_player && (g_duel_network_flags & 2) != 0)) &&
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
      unk_00742f60 = 1 - human_player;
      FUN_004460d3();
    }

    while (1)
    {
      s.saved_DAT_00777aa8_in_loop = DAT_00777aa8;
      s.saved_phase_stop_suppressed = phase_stop_suppressed;

      if ((g_duel_network_flags & 2) != 0)
      {
        DAT_00789714 = 1;
        phase_stop_suppressed = 0;

        if (stack_size == 0 && should_skip_phase(1 - human_player) != 0 && param_1 != -2)
        {
          phase_stop_suppressed = 1;
        }

        DAT_00777aa8 = FUN_0044541f(0);
      }
      else
      {
        if (human_player != 0)
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

      s.dispatch_result = (unsigned int)process_response_actions(1 - human_player, s.prompt_copy);
      DAT_00777aa8 = s.saved_DAT_00777aa8_in_loop;
      phase_stop_suppressed = s.saved_phase_stop_suppressed;
      if (s.dispatch_result != 0)
      {
        response_action_taken = 1;

        if (g_duel_ai_mode_state != 1)
        {
          goto restart_response_window;
        }

        if (nonactive_player == human_player)
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
    DAT_0072c8e4 = 0;
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
    if (human_player == param_1)
    {
      if ((nonactive_player == param_1 &&
           (((int)(char)g_duel_phase_stop_settings[nonactive_player].phase_flags[current_phase]) & PHASE_STOP_ENABLED) != 0) ||
          (active_player == param_1 &&
           (((int)(char)g_duel_phase_stop_settings[active_player].phase_flags[current_phase]) & PHASE_STOP_OPPONENT) != 0))
      {
        s.result = 1;
      }
    }
    else
    {
      if ((active_player == param_1 &&
           (((int)(char)g_duel_phase_stop_settings[nonactive_player].phase_flags[current_phase]) & PHASE_STOP_OPPONENT) != 0) ||
          (nonactive_player == param_1 &&
           (((int)(char)g_duel_phase_stop_settings[active_player].phase_flags[current_phase]) & PHASE_STOP_ENABLED) != 0))
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

    if (stop_phase == -1 && nonactive_player == param_1 &&
        (((int)(char)g_duel_phase_stop_settings[human_player].phase_flags[s.mapped_phase]) & PHASE_STOP_ENABLED) != 0)
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

  if (trigger_condition != -1 && current_turn == active_player)
  {
    for (s.other_player = 0; s.other_player < 2; ++s.other_player)
    {
      for (s.card = 0; (int)s.card < active_cards_count[s.other_player]; ++s.card)
      {
        if (is_in_play(s.other_player, (int)s.card) &&
            FUN_004b082f(s.other_player, (int)s.card, 0x7d, player) == 2)
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

  if (((DAT_00789714 & 2) == 0) || (player == nonactive_player) || (_DAT_00743020 == 0))
  {
    return 0xffffffffU;
  }

  unk_00742fcc = player;

  for (s.card = 0; (int)s.card < active_cards_count[player]; ++s.card)
  {
    s.internal_card_id = PLAYER_CARD_INSTANCE(player, (int)s.card).internal_card_id;
    if (s.internal_card_id != -1)
    {
      s.response_flags = (int)FUN_004af0ec(player, (int)s.card);
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
        s.response_flags = (int)FUN_004af0ec(1 - player, (int)s.card);
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
      unk_00939340 = internal_rand(s.num_candidates);
    }
    else
    {
      unk_00939340 = s.num_candidates - 1;
    }

    if (DAT_008a8d74 != 0)
    {
      unk_00939340 = s.num_candidates - 1;
      if (DAT_008a8d74 == 1)
      {
        DAT_008a8d74 = -1;
      }
    }

    unk_00925bb8 = (((unsigned int)-(((PLAYER_CARD_INSTANCE(player, (int)s.candidates[unk_00939340]).state & 2) == 0))) &
                    0xfffff000) +
                   0x2000;
    unk_00925bb8 |= s.candidates[unk_00939340];
    unk_00925bb8 |= (((player == 0) - 1) & 0x100);

    unk_0057aae8 = 4;
    FUN_004e4f11();
  }
  else
  {
    unk_0057aae8 = 4;
    FUN_004e5089();

    if (s.num_candidates <= unk_00939340)
    {
      unk_00939340 = s.num_candidates - 1;
    }
  }

  if (s.candidates[unk_00939340] != 0xffffffffU)
  {
    if (DAT_008a8de4 > 0xf)
    {
      --DAT_008a8de4;
    }

    DAT_0091bf70[DAT_008a8de4] =
        PLAYER_CARD_INSTANCE(player, (int)s.candidates[unk_00939340]).internal_card_id;
    DAT_007abc00[DAT_008a8de4] = s.candidates[unk_00939340];
    ++DAT_008a8de4;
  }

  return s.candidates[unk_00939340];
}

// FUNCTION: MAGIC 0x00440ad2
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
    if (current_turn != nonactive_player)
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
    sprintf(unk_00748770, gs_activate_0091b2d0, FUN_0044a3bf(player, card));

    if (phase_stop_suppressed != 0 && (nonactive_player == player || (g_duel_network_flags & 2) != 0))
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
  dispatch_trigger_twice_once_with_each_player_as_reason(human_player, 0xd2, gs_tapping_00925d40, 0);

  trigger_cause_controller = saved_trigger_cause_controller;
  trigger_cause = saved_trigger_cause;

  return 1;
}

// FUNCTION: MAGIC 0x00440c61
int FUN_00440c61(int player, int card)
{
  if (PLAYER_CARD_INSTANCE(player, card).internal_card_id == -1)
  {
    return 0;
  }

  return ((PLAYER_CARD_INSTANCE(player, card).state & (STATE_IN_PLAY | STATE_INVISIBLE)) != 0) ? 0 : 1;
}

// FUNCTION: MAGIC 0x00442cf9
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
int FUN_004af0ec(int player, int card)
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
    s.response_controller = nonactive_player;
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
      s.tmp = FUN_004b082f(player, card, 0x7d, player);
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

      if ((((land_can_be_played & 4) == 0 ||
            (global_cards_data[s.internal_card_id].extra_ability & 0x1004) != 0) ||
           (DAT_007aadec == 0xd3 &&
            (global_cards_data[s.internal_card_id].extra_ability & 0x4000) != 0)))
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
        FUN_00445b56(player, card) != 0)
    {
      DAT_007ab2bc |= 2;

      if (active_player == player && (g_duel_network_flags & 2) == 0)
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
        FUN_0043fdb3(player, player, card) != 0 &&
        (((land_can_be_played & 4) == 0 ||
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

  if (s.who_pays == active_player && (g_duel_network_flags & 2) == 0 && g_duel_ai_mode_state != 1)
  {
    unk_00925bb8 = -1;
    dispatch_event_to_single_card(player, card, EVENT_GET_SELECTED_CARD, 1 - player, -1);

    LoadTextSectionLines(global_ui_strings_filename, "PROMPT_TAP1");
    sprintf(s.prompt, text_lines[0], DAT_007a7c60);

    if (FUN_00445f61(player, card) != 0)
    {
      sprintf(s.prompt, text_lines[1], DAT_007a7c60, unk_00715fa8);
    }
    else if (((global_cards_data[instance->internal_card_id].extra_ability & (EA_INF_POWER | EA_INF_TOUGHNESS)) != 0) &&
             human_player == player && unk_00715fa8 != 0)
    {
      sprintf(s.prompt, text_lines[2], DAT_007a7c60, unk_00715fa8);
    }

    if (instance->number_of_targets == 0)
    {
      if (unk_00925bb8 == -1)
      {
        raw_do_dialog(player, card, -1, -1, s.prompt, 0);
      }
      else
      {
        /* Unreachable in practice, but present in the original binary. */
        raw_do_dialog(player, card, (unk_00925bb8 >> 8), (unk_00925bb8 & 0xff), s.prompt, 0);
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
        if (DAT_008cf1b8 == 0)
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

  if ((instance->upkeep_flags & UPKEEP_UNTAP_TRIGGER) != 0 && FUN_00445b56(player, card) != 0)
  {
    for (i = 0; i < 7; ++i)
    {
      mana_charge[i] = (int)(char)instance->mana_to_untap[i];
    }

    charge_mana(player, 0, 0);
    if (spell_fizzled == 0)
    {
      dispatch_event_to_single_card(player, card, EVENT_UNTAP_PHASE, 1 - player, -1);
      if (DAT_008cf1b8 == 0)
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

    FUN_00435c1f();

    s.was_tapped = instance->state & 0x10;
    dispatch_event_to_single_card(player, card, EVENT_ACTIVATE, 1 - player, -1);

    if ((g_duel_network_flags & 2) != 0 && active_player == who_activates && DAT_007abc80 == 0 &&
        instance->internal_card_id != unk_008b28f8)
    {
      LoadTextSectionLines(global_ui_strings_filename, "PROMPT_TAP1");
      sprintf(s.prompt, text_lines[0], DAT_007a7c60);

      if (FUN_00445f61(player, card) != 0)
      {
        sprintf(s.prompt, text_lines[1], DAT_007a7c60, x_value);
      }
      else if (((global_cards_data[instance->internal_card_id].extra_ability & (EA_INF_POWER | EA_INF_TOUGHNESS)) != 0) &&
               human_player == player && x_value != 0)
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
      FUN_00435d11(player);
      obliterate_top_card_of_stack();
      s.result = 0;
    }

    FUN_00435cdd();

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
int allow_response(int param_1, int param_2, char *param_3, int param_4)
{
  int saved_allow_response_flag = phase_stop_suppressed;

  if (stack_size == 0)
  {
    if (should_skip_phase(human_player) != 0 && param_1 != -2 && g_duel_ai_mode_state != 1)
    {
      phase_stop_suppressed = 1;
    }
  }

  {
    int result;

    do
    {
      response_action_taken = 0;
      result = allow_response_once(param_1, param_2, param_3, param_4);
    } while (response_action_taken != 0 && stack_size <= 0);

    if (stack_size == 0)
    {
      if (g_duel_ai_mode_state != 1)
      {
        phase_stop_suppressed = 0;
      }
      {
        g_duel_phase_stop_settings[human_player].phase_flags[current_phase] = (int)(char)g_duel_phase_stop_settings[human_player].phase_flags[current_phase] & ~PHASE_STOP_SUPPRESSED;
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

  if (life[nonactive_player] <= 0)
  {
    g_duel_phase_stop_settings[human_player].phase_flags[current_phase] =
        (unsigned char)(((int)(char)g_duel_phase_stop_settings[human_player].phase_flags[current_phase]) | PHASE_STOP_SUPPRESSED);
  }

  s.interrupts_available = 0;
  if (g_duel_ai_mode_state != 1 && stop_phase_player == human_player && current_phase == stop_phase)
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
    if (active_player == reason_for_trigger_controller && global_stack_cards[0].player == nonactive_player)
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
    if (g_duel_ai_mode_state == 1 || DAT_0072c8e4 != 0 ||
        (trigger_condition != -1 && current_turn == active_player) || DAT_007aadec == 4 ||
        (active_player == unk_00742f60 && (land_can_be_played & 0x200) != 0))
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
        FUN_004b082f(s.selected_player, (int)s.selected_card, 0x7d, s.player) == 2)
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
            DAT_007ab2cc != NULL && trigger_condition != -1)
        {
          DAT_007ab2cc(s.selected_player, (int)s.selected_card);
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
        if (FUN_0043ece1(s.selected_player, (int)s.selected_card))
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
       (current_turn == nonactive_player || (current_turn == active_player && (g_duel_network_flags & 2) != 0))))
  {
    if (((current_turn == nonactive_player) || (current_turn == active_player && (g_duel_network_flags & 2) != 0)) &&
        trigger_condition != -1)
    {
      for (s.player = 0; s.player < 2; ++s.player)
      {
        for (s.card = 0; (int)s.card < active_cards_count[s.player]; ++s.card)
        {
          if (is_in_play(s.player, (int)s.card))
          {
            s.response_code = FUN_004b082f(s.player, (int)s.card, 0x7d, reason_for_trigger_controller);
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
        if (trigger_condition == 0xca && (char)g_duel_phase_stop_settings[human_player].phase_flags[PHASE_UPKEEP] == 0)
        {
          DAT_00789714 = 0;
        }
        if (trigger_condition == 0xce && (char)g_duel_phase_stop_settings[human_player].phase_flags[PHASE_DRAW] == 0)
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
          s.player = nonactive_player;
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
            s.response_code = FUN_004af0ec(s.player, (int)s.card);

            if (((s.response_code >= 2) ||
                 (((DAT_00777aa8 != 0 || s.player != human_player) && (s.response_code & 2) != 0) ||
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
              s.response_code = FUN_004af0ec(1 - s.player, (int)s.card);
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
              s.player = nonactive_player;
            }
            else
            {
              s.player = reason_for_trigger_controller;
            }

            if (nonactive_player == s.player)
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
              g_duel_phase_stop_settings[human_player].phase_flags[current_phase] =
                  (unsigned char)(((int)(char)g_duel_phase_stop_settings[human_player].phase_flags[current_phase]) | PHASE_STOP_SUPPRESSED);
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
      s.player = nonactive_player;
    }
    else
    {
      s.player = reason_for_trigger_controller;
    }

    if (s.selected_card != 0xffffffffU &&
        (s.selected_player == s.player ||
         (((PLAYER_CARD_INSTANCE(s.selected_player, (int)s.selected_card).state & 2) != 0 &&
           FUN_004b082f(s.selected_player, (int)s.selected_card, 0x7d, s.player) != 0) ||
          DAT_007aadec == 4)))
    {
      s.internal_card_id = PLAYER_CARD_INSTANCE(s.selected_player, (int)s.selected_card).internal_card_id;

      if (FUN_004af0ec(s.selected_player, (int)s.selected_card) != 0)
      {
        if (is_in_play(s.selected_player, (int)s.selected_card))
        {
          if (FUN_004b082f(s.selected_player, (int)s.selected_card, 0x7d, s.player) != 0)
          {
            FUN_00440ad2(s.selected_player, (int)s.selected_card, s.player);
          }
          else
          {
            if (PLAYER_CARD_INSTANCE(s.selected_player, (int)s.selected_card).unknown0x14 == (unsigned int)trigger_condition &&
                DAT_007ab2cc != NULL && trigger_condition != -1)
            {
              if (DAT_007ab2cc != NULL)
              {
                DAT_007ab2cc(s.selected_player, (int)s.selected_card);
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
          FUN_0043ece1(s.selected_player, (int)s.selected_card);
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
int FUN_00435c1f(void)
{
  int color;

  if (unk_005607e0 < 10)
  {
    for (color = 0; color < 7; ++color)
    {
      unk_00637558[unk_005607e0][color] = 0;
    }
    ++unk_005607e0;
    return 1;
  }
  else
  {
    return 0;
  }
}

// FUNCTION: MAGIC 0x00435cdd
int FUN_00435cdd(void)
{
  if (unk_005607e0 > 0)
  {
    --unk_005607e0;
    return 1;
  }
  else
  {
    return 0;
  }
}

// FUNCTION: MAGIC 0x00435d11
int FUN_00435d11(int player)
{
  int color;

  if (unk_005607e0 > 0)
  {
    for (color = 0; color < 7; ++color)
    {
      raw_mana_available[player][color] += unk_00637558[unk_005607e0 - 1][color];
      raw_mana_available[player][7] += unk_00637558[unk_005607e0 - 1][color];
    }

    return 1;
  }
  else
  {
    return 0;
  }
}

// FUNCTION: MAGIC 0x004438cb
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
int FUN_004427d9(int param_1)
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
void FUN_00495fa1(void)
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

// FUNCTION: MAGIC 0x00441cf2
void start_ai_decision_search(int decision_code, int time_scale)
{
  DAT_0089652c = -9999;
  g_duel_ai_mode_state = 1;
  ai_decision_code = decision_code;
  DAT_0091c500 = ((g_shandalar_difficulty + 1) * time_scale) / 4;

  DAT_00775d3c = 1 << (unsigned char)(internal_rand(5) + 1);

  save_ai_search_state();
  FUN_004e1b84();
  update_duel_thread_time_marker();

  unk_008cc840 = 0;
  DAT_008a8d74 = 1;
  unk_00712938 = 0;
}

// FUNCTION: MAGIC 0x004e430e
void save_ai_search_state(void)
{
  saved_recorded_action_count = 0;
  memcpy(g_ai_search_backup.card_instances, global_card_instances, sizeof(g_ai_search_backup.card_instances));
  memcpy(g_ai_search_backup.extra_card_data, global_cards_data + g_card_count, sizeof(g_ai_search_backup.extra_card_data));
  memcpy(g_ai_search_backup.library, global_library, sizeof(g_ai_search_backup.library));
  memcpy(g_ai_search_backup.graveyard, global_graveyard_slots, sizeof(g_ai_search_backup.graveyard));
  memcpy(g_ai_search_backup.exile, global_exile, sizeof(g_ai_search_backup.exile));
  memcpy(g_ai_search_backup.special_mana, unk_00742f70, sizeof(g_ai_search_backup.special_mana));
  memcpy(g_ai_search_backup.raw_mana, raw_mana_available, sizeof(g_ai_search_backup.raw_mana));
  memcpy(g_ai_search_backup.basic_lands, basiclandtypes_controlled, sizeof(g_ai_search_backup.basic_lands));
  memcpy(g_ai_search_backup.raw_mana_hex, raw_mana_available_hex, sizeof(g_ai_search_backup.raw_mana_hex));
  memcpy(g_ai_search_backup.life_totals, life, sizeof(g_ai_search_backup.life_totals));
  memcpy(g_ai_search_backup.DAT_007abce0_pair, &DAT_007abce0, sizeof(g_ai_search_backup.DAT_007abce0_pair));
  memcpy(g_ai_search_backup.card_types, card_types_in_play, sizeof(g_ai_search_backup.card_types));
  memcpy(g_ai_search_backup.graveyard_type_flags, unk_007a7c58, sizeof(g_ai_search_backup.graveyard_type_flags));
  memcpy(g_ai_search_backup.DAT_008cfd70_copy, DAT_008cfd70, sizeof(g_ai_search_backup.DAT_008cfd70_copy));
  memcpy(g_ai_search_backup.ante_cards, global_ante_cards, sizeof(g_ai_search_backup.ante_cards));
  memcpy(g_ai_search_backup.timestamp_player, TENTATIVE_timestamp_player, sizeof(g_ai_search_backup.timestamp_player));
  memcpy(g_ai_search_backup.timestamp_card, TENTATIVE_timestamp_card, sizeof(g_ai_search_backup.timestamp_card));
  g_ai_search_backup.land_play_flags = land_can_be_played;
  g_ai_search_backup.phase = current_phase;
  DAT_00743098 = g_ai_search_backup.phase;
  g_ai_search_backup.current_casting_internal_card_id_value = current_casting_internal_card_id;
  g_ai_search_backup.combat_count = attacking_creature_count;
  g_ai_search_backup.spell_fizzled_value = spell_fizzled;
  memcpy(g_ai_search_backup.stack_events, stack_data, sizeof(g_ai_search_backup.stack_events));
  memcpy(g_ai_search_backup.stack_cards, global_stack_cards, sizeof(g_ai_search_backup.stack_cards));
  memcpy(g_ai_search_backup.stack_damage_targets, global_stack_damage_targets, sizeof(g_ai_search_backup.stack_damage_targets));
  memcpy(g_ai_search_backup.active_counts, active_cards_count, sizeof(g_ai_search_backup.active_counts));
  if (stack_size < 0)
  {
    _assert("ScWilly>=0", "D:\\Newmagic\\multiplayer\\sid\\Ai.c", 0x1cb);
  }
  g_ai_search_backup.stack_count = stack_size;
  g_ai_search_backup.DAT_007ab2cc_value = DAT_007ab2cc;
  g_ai_search_backup.DAT_0093a848_value = DAT_0093a848;
  memcpy(g_ai_search_backup.damage_matrix, unk_0093b280, sizeof(g_ai_search_backup.damage_matrix));
  memcpy(g_ai_search_backup.cost_mod, unk_0072c440, sizeof(g_ai_search_backup.cost_mod));
  memcpy(g_ai_search_backup.mana_charge, mana_charge, sizeof(g_ai_search_backup.mana_charge));
  g_ai_search_backup.x_value_copy = x_value;
  g_ai_search_backup.max_x_value_copy = max_x_value;
  g_ai_search_backup.extra_turn_player = g_duel_extra_turn_player;
  g_ai_search_backup.DAT_0093a848_value = DAT_0093a848;
  g_ai_search_backup.stop_phase_copy = stop_phase;
  g_ai_search_backup.stop_phase_player_copy = stop_phase_player;
  g_ai_search_backup.battlefield_flags = battlefield_extra_ability_flags;
  memcpy(g_ai_search_backup.unk_008b44d0_copy, unk_008b44d0, sizeof(g_ai_search_backup.unk_008b44d0_copy));
  memcpy(g_ai_search_backup.unk_007a79b0_copy, unk_007a79b0, sizeof(g_ai_search_backup.unk_007a79b0_copy));
  StopWorldLocationMusic();
}

// FUNCTION: MAGIC 0x004e4632
void restore_ai_search_state(void)
{
  memcpy(global_card_instances, g_ai_search_backup.card_instances, sizeof(g_ai_search_backup.card_instances));
  memcpy(global_cards_data + g_card_count, g_ai_search_backup.extra_card_data, sizeof(g_ai_search_backup.extra_card_data));
  memcpy(global_library, g_ai_search_backup.library, sizeof(g_ai_search_backup.library));
  memcpy(global_graveyard_slots, g_ai_search_backup.graveyard, sizeof(g_ai_search_backup.graveyard));
  memcpy(global_exile, g_ai_search_backup.exile, sizeof(g_ai_search_backup.exile));
  memcpy(unk_00742f70, g_ai_search_backup.special_mana, sizeof(g_ai_search_backup.special_mana));
  memcpy(raw_mana_available, g_ai_search_backup.raw_mana, sizeof(g_ai_search_backup.raw_mana));
  memcpy(basiclandtypes_controlled, g_ai_search_backup.basic_lands, sizeof(g_ai_search_backup.basic_lands));
  memcpy(raw_mana_available_hex, g_ai_search_backup.raw_mana_hex, sizeof(g_ai_search_backup.raw_mana_hex));
  memcpy(life, g_ai_search_backup.life_totals, sizeof(g_ai_search_backup.life_totals));
  memcpy(&DAT_007abce0, g_ai_search_backup.DAT_007abce0_pair, sizeof(g_ai_search_backup.DAT_007abce0_pair));
  memcpy(card_types_in_play, g_ai_search_backup.card_types, sizeof(g_ai_search_backup.card_types));
  memcpy(unk_007a7c58, g_ai_search_backup.graveyard_type_flags, sizeof(g_ai_search_backup.graveyard_type_flags));
  memcpy(DAT_008cfd70, g_ai_search_backup.DAT_008cfd70_copy, sizeof(g_ai_search_backup.DAT_008cfd70_copy));
  memcpy(global_ante_cards, g_ai_search_backup.ante_cards, sizeof(g_ai_search_backup.ante_cards));
  memcpy(TENTATIVE_timestamp_player, g_ai_search_backup.timestamp_player, sizeof(g_ai_search_backup.timestamp_player));
  memcpy(TENTATIVE_timestamp_card, g_ai_search_backup.timestamp_card, sizeof(g_ai_search_backup.timestamp_card));
  land_can_be_played = g_ai_search_backup.land_play_flags;
  current_phase = g_ai_search_backup.phase;
  current_casting_internal_card_id = g_ai_search_backup.current_casting_internal_card_id_value;
  attacking_creature_count = g_ai_search_backup.combat_count;
  spell_fizzled = g_ai_search_backup.spell_fizzled_value;
  memcpy(stack_data, g_ai_search_backup.stack_events, sizeof(g_ai_search_backup.stack_events));
  memcpy(global_stack_cards, g_ai_search_backup.stack_cards, sizeof(g_ai_search_backup.stack_cards));
  memcpy(global_stack_damage_targets, g_ai_search_backup.stack_damage_targets, sizeof(g_ai_search_backup.stack_damage_targets));
  memcpy(active_cards_count, g_ai_search_backup.active_counts, sizeof(g_ai_search_backup.active_counts));
  stack_size = g_ai_search_backup.stack_count;
  DAT_007ab2cc = g_ai_search_backup.DAT_007ab2cc_value;
  DAT_0093a848 = g_ai_search_backup.DAT_0093a848_value;
  memcpy(unk_0093b280, g_ai_search_backup.damage_matrix, sizeof(g_ai_search_backup.damage_matrix));
  memcpy(unk_0072c440, g_ai_search_backup.cost_mod, sizeof(g_ai_search_backup.cost_mod));
  memcpy(mana_charge, g_ai_search_backup.mana_charge, sizeof(g_ai_search_backup.mana_charge));
  x_value = g_ai_search_backup.x_value_copy;
  max_x_value = g_ai_search_backup.max_x_value_copy;
  g_duel_extra_turn_player = g_ai_search_backup.extra_turn_player;
  DAT_0093a848 = g_ai_search_backup.DAT_0093a848_value;
  stop_phase = g_ai_search_backup.stop_phase_copy;
  stop_phase_player = g_ai_search_backup.stop_phase_player_copy;
  battlefield_extra_ability_flags = g_ai_search_backup.battlefield_flags;
  memcpy(unk_008b44d0, g_ai_search_backup.unk_008b44d0_copy, sizeof(g_ai_search_backup.unk_008b44d0_copy));
  memcpy(unk_007a79b0, g_ai_search_backup.unk_007a79b0_copy, sizeof(g_ai_search_backup.unk_007a79b0_copy));
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

// FUNCTION: MAGIC 0x004e4e9a
void __stdcall FUN_004e4e9a(void)
{
  int i;

  DAT_0093d850 = 0;
  recorded_action_count = DAT_0093d850;
  _DAT_00743020 = -1;
  for (i = 0; i < 0x100; ++i)
  {
    unk_006a6ee8[i] = 0x63;
  }
  restore_ai_search_state();
  if (g_duel_ai_mode_state != 1)
  {
    ai_decision_code = -1;
  }
}

// FUNCTION: MAGIC 0x004e50f5
void __stdcall save_recorded_ai_actions(void)
{
  int action_index;

  for (action_index = 0; action_index < recorded_action_count; action_index = action_index + 1)
  {
    recorded_action_codes[action_index] = unk_006a6ee8[action_index];
    unk_006a1db8[action_index] = unk_006a21b8[action_index];
    saved_recorded_action_internal_card_ids[action_index] = unk_006a8258[action_index];
    unk_006a5f18[action_index] = unk_006a8660[action_index];
  }

  recorded_action_codes[recorded_action_count] = 99;
  if (recorded_action_codes[0] == 99)
  {
    saved_recorded_action_count = recorded_action_count;
  }
  DAT_0072c8e4 = 1;
}

// FUNCTION: MAGIC 0x004e51e8
int ai_opinion_of_gamestate(int player)
{
  struct
  {
    int block_result;
    int power_score;
    int toughness;
    unsigned int abilities;
    unsigned int landwalk_masks[2];
    char attached_to_card[300];
    int card_value;
    int opponent;
    int loop2;
    int life_score;
    int card;
    int damage_power;
    int internal_card_id;
    int side_score;
    int score;
    int current_player;
  } s;

  unk_00712938 = 1;
  s.score = 0;
  s.opponent = 1 - player;
  get_landwalk_evasion_masks(&s.landwalk_masks[0], &s.landwalk_masks[1]);
  memset(s.attached_to_card, 0, 0x12c);

  s.life_score = 0;
  s.score = s.life_score;
  for (s.card = 1; s.card <= life[player]; s.card = s.card + 1)
  {
    s.life_score += 0x18 / s.card + 0xc;
  }
  s.score += (DAT_007a7d10[player] * s.life_score) / 8;

  s.life_score = 0;
  for (s.card = 1; s.card <= life[s.opponent]; s.card = s.card + 1)
  {
    s.life_score += 0x18 / s.card + 0xc;
  }
  s.score -= (DAT_007a7d10[s.opponent] * s.life_score) / 8;

  if (life[player] <= 0)
  {
    s.score += (life[player] * 4 - 8) * 0x4b;
  }
  if (life[s.opponent] <= 0)
  {
    s.score -= (life[s.opponent] - 2) * 0x100;
  }

  if (DAT_00712544 != 0)
  {
    strcpy(unk_00748770, "");
  }

  for (s.current_player = 0; s.current_player < 2; s.current_player = s.current_player + 1)
  {
    s.opponent = 1 - s.current_player;
    s.side_score = -(((active_player == s.current_player ? 0x30 : 0) + 0x18) *
                     raw_mana_available[s.current_player][7]);

    for (s.card = 1; s.card < 6; s.card = s.card + 1)
    {
      for (s.loop2 = 1; s.loop2 <= basiclandtypes_controlled[s.current_player][s.card]; s.loop2 = s.loop2 + 1)
      {
        s.side_score += 0x30 / s.loop2;
      }
    }

    for (s.card = 0; s.card < active_cards_count[s.current_player]; s.card = s.card + 1)
    {
      if (global_card_instances[s.current_player][s.card].internal_card_id == -1)
      {
        continue;
      }
      if ((global_card_instances[s.current_player][s.card].state & STATE_OUBLIETTED) != 0)
      {
        continue;
      }

      s.internal_card_id = global_card_instances[s.current_player][s.card].internal_card_id;
      if ((global_cards_data[s.internal_card_id].type & TYPE_EFFECT) != 0)
      {
        continue;
      }

      s.card_value = 1;
      if ((global_cards_data[s.internal_card_id].type & TYPE_CREATURE) != 0)
      {
        s.abilities = C_get_abilities(s.current_player, s.card, EVENT_ABILITIES, -1);
        s.power_score = C_get_abilities(s.current_player, s.card, EVENT_POWER, -1) & ~0x4000;
        s.power_score *= 2;
        s.damage_power = s.power_score;
        if (global_cards_data[s.internal_card_id].subtype == 0)
        {
          s.damage_power = 0;
          s.power_score = s.damage_power;
        }
        s.toughness = C_get_abilities(s.current_player, s.card, EVENT_TOUGHNESS, -1) & ~0x4000;
        s.card_value = ((s.power_score + 3) * (s.toughness + 4)) / 2;

        if ((global_card_instances[s.current_player][s.card].state & STATE_TAPPED) != 0 &&
            human_player == s.current_player)
        {
          s.card_value--;
        }
        if ((s.abilities & 0x80) != 0)
        {
          s.card_value = (s.card_value * 3) / 2;
        }
        if ((s.abilities & 0x100) != 0)
        {
          s.card_value = (s.card_value * 3) / 2;
        }
        if ((global_cards_data[s.internal_card_id].extra_ability & 3) != 0)
        {
          s.card_value = (s.card_value * 3) / 2;
        }
        if ((s.abilities & 0x40) != 0)
        {
          s.card_value = ((s.toughness + 1) * s.card_value) / 2;
        }
        if ((s.abilities & 0x200) != 0)
        {
          s.card_value = (s.card_value * 3) / 2;
        }

        if (DAT_0093f4b0 == 0 &&
            player != s.current_player &&
            player == human_player &&
            (global_card_instances[s.current_player][s.card].state & STATE_IN_PLAY) != 0)
        {
          s.block_result = 0;
          s.abilities = C_get_abilities(s.current_player, s.card, EVENT_ABILITIES, -1);
          for (s.loop2 = 0; s.loop2 < active_cards_count[s.opponent]; s.loop2 = s.loop2 + 1)
          {
            if (can_block_attacker_with_abilities(s.opponent,
                                                  s.loop2,
                                                  s.current_player,
                                                  s.card,
                                                  s.abilities,
                                                  s.landwalk_masks[s.current_player]) != 0)
            {
              s.block_result |= 1;
              if (C_get_abilities(s.opponent, s.loop2, EVENT_TOUGHNESS, s.card) > s.power_score ||
                  C_get_abilities(s.opponent, s.loop2, EVENT_POWER, s.card) >= s.toughness)
              {
                s.block_result |= 2;
                break;
              }
            }
          }
          if ((s.block_result & 2) == 0)
          {
            s.side_score += (DAT_007a7d10[s.opponent] * s.damage_power * 0x18) / 0x10;
            if (s.block_result == 0 && life[s.opponent] <= s.power_score)
            {
              s.side_score += 0x100;
            }
          }
        }

        if ((global_card_instances[s.current_player][s.card].state & STATE_IN_PLAY) != 0)
        {
          s.card_value *= 3;
        }
        else if (global_cards_data[s.internal_card_id].code_pointer == card_ball_lightning)
        {
          s.card_value = 1;
        }

        s.card_value = (unk_007a7d18[s.current_player] * s.card_value) / 8;
      }

      if ((global_cards_data[s.internal_card_id].type & TYPE_LAND) != 0)
      {
        if ((global_card_instances[s.current_player][s.card].state & STATE_IN_PLAY) == 0)
        {
          s.card_value = 2;
        }
        else if ((global_card_instances[s.current_player][s.card].state & STATE_TAPPED) == 0)
        {
          s.card_value = 1;
        }
        else
        {
          s.card_value = 0;
        }
      }

      if (global_cards_data[s.internal_card_id].type == TYPE_ARTIFACT &&
          (global_card_instances[s.current_player][s.card].state & STATE_IN_PLAY) != 0)
      {
        s.card_value = ((global_cards_data[s.internal_card_id].cc[1] * 3 + 3) * 4) / 2;
      }

      if (global_cards_data[s.internal_card_id].type == TYPE_ENCHANTMENT &&
          (global_card_instances[s.current_player][s.card].state & STATE_IN_PLAY) != 0 &&
          (char)global_card_instances[s.current_player][s.card].damage_target_player != -1 &&
          global_card_instances[s.current_player][s.card].damage_target_card != -1)
      {
        s.attached_to_card[(int)(char)global_card_instances[s.current_player][s.card].damage_target_player * 150 +
                           global_card_instances[s.current_player][s.card].damage_target_card] |=
            (char)(1 << (unsigned char)s.current_player);
      }

      if ((global_cards_data[s.internal_card_id].type & TYPE_SPELL) != 0 &&
          (global_card_instances[s.current_player][s.card].state & STATE_IN_PLAY) == 0)
      {
        s.card_value = GetCardRarity(s.internal_card_id) * 0xc;
      }

      if ((global_cards_data[s.internal_card_id].type & TYPE_ENCHANTMENT) != 0 &&
          (global_card_instances[s.current_player][s.card].state & STATE_IN_PLAY) == 0)
      {
        s.card_value = 3;
      }

      s.side_score += s.card_value;
      if ((DAT_00712544 & 2) != 0 && s.current_player + 2 == DAT_00712544)
      {
        FUN_0044a796(s.current_player, s.card);
        strcat(unk_00748770, " ");
        strcat(unk_00748770, _itoa(s.card_value, ai_action_dialog_number_buffer, 10));
        strcat(unk_00748770, "\n");
      }
    }

    if (s.current_player == 0)
    {
      DAT_008cdab0 = s.side_score;
    }
    else
    {
      DAT_008cdab4 = s.side_score;
    }

    if (player == s.current_player)
    {
      s.score += s.side_score;
    }
    else
    {
      s.score -= s.side_score;
    }
  }

  for (s.current_player = 0; s.current_player < 2; s.current_player = s.current_player + 1)
  {
    for (s.card = 0; s.card < active_cards_count[s.current_player]; s.card = s.card + 1)
    {
      if (((int)(char)s.attached_to_card[s.card + s.current_player * 150] & (1 << (unsigned char)player)) != 0)
      {
        s.score += 2;
      }
      if (((int)(char)s.attached_to_card[s.card + s.current_player * 150] & (1 << (unsigned char)(1 - player))) != 0)
      {
        s.score -= 2;
      }
    }
  }

  if (DAT_0093f4b0 == 0 && player == human_player)
  {
    s.score = ai_opinion_of_gamestate_continued(player, s.score);
  }
  unk_00712938 = 0;
  return s.score;
}

// FUNCTION: MAGIC 0x004e5db4
int ai_opinion_of_gamestate_continued(int player, int score)
{
  struct
  {
    int attacker_power;
    int attacker_toughness;
    unsigned int block_result;
    int best_blocker;
    int opponent_player;
    int test_card;
    unsigned int landwalk_mask;
    int expected_damage;
    int card;
    int internal_card_id;
    unsigned int attacker_abilities;
    int blocker_toughness[16];
    int ranked_attackers[24];
    int creature_index;
    int blocker_power[16];
    int test_toughness;
    char creature_index_by_card[152];
    int smallest_blocker_score;
    int blocker_scores[16];
    int test_power;
    int ranked_index;
    int creature_count;
    char saved_special_mana[8];
  } s;

  s.opponent_player = 1 - player;
  get_landwalk_evasion_masks(&s.landwalk_mask, NULL);

  for (s.card = 0; s.card < 8; s.card = s.card + 1)
  {
    s.saved_special_mana[s.card] = (char)unk_00742f70[s.opponent_player][s.card];
    unk_00742f70[s.opponent_player][s.card] = basiclandtypes_controlled[s.opponent_player][s.card];
  }

  for (s.ranked_index = 0; s.ranked_index < 8; s.ranked_index = s.ranked_index + 1)
  {
    s.ranked_attackers[s.ranked_index * 3 + 1] = -1;
  }

  s.creature_count = 0;
  for (s.test_card = 0; s.test_card < active_cards_count[player]; s.test_card = s.test_card + 1)
  {
    if (global_card_instances[player][s.test_card].internal_card_id != -1 &&
        (global_card_instances[player][s.test_card].state & (STATE_IN_PLAY | STATE_PLAYED_FROM_HAND)) != 0 &&
        (global_card_instances[player][s.test_card].state & STATE_OUBLIETTED) == 0 &&
        (global_cards_data[global_card_instances[player][s.test_card].internal_card_id].type & TYPE_CREATURE) != 0)
    {
      s.creature_index_by_card[s.test_card] = (char)s.creature_count;
      s.blocker_power[s.creature_count] = C_get_abilities(player, s.test_card, EVENT_POWER, -1);
      s.blocker_toughness[s.creature_count] = C_get_abilities(player, s.test_card, EVENT_TOUGHNESS, -1);
      s.blocker_scores[s.creature_count] = (int)global_card_instances[player][s.test_card].mana_color;
      s.creature_count++;
    }
  }

  for (s.card = 0; s.card < active_cards_count[s.opponent_player]; s.card = s.card + 1)
  {
    s.internal_card_id = global_card_instances[s.opponent_player][s.card].internal_card_id;
    if (is_in_play(s.opponent_player, s.card) != 0 &&
        (global_cards_data[s.internal_card_id].type & TYPE_CREATURE) != 0 &&
        (global_cards_data[s.internal_card_id].subtype != 0 ||
         (global_card_instances[s.opponent_player][s.card].token_status & 0x800) != 0))
    {
      s.attacker_power = C_get_abilities(s.opponent_player, s.card, EVENT_POWER, -1);
      s.attacker_toughness = C_get_abilities(s.opponent_player, s.card, EVENT_TOUGHNESS, -1);
      if (nonactive_player == s.opponent_player)
      {
        if ((global_cards_data[s.internal_card_id].extra_ability & EA_INF_POWER) != 0)
        {
          s.attacker_power += (*global_cards_data[s.internal_card_id].code_pointer)(s.opponent_player, s.card, 0x39);
        }
        if ((global_cards_data[s.internal_card_id].extra_ability & EA_INF_TOUGHNESS) != 0)
        {
          s.attacker_toughness += (*global_cards_data[s.internal_card_id].code_pointer)(s.opponent_player, s.card, 0x3a);
        }
      }

      s.test_card = 0;
      while (s.test_card < 8)
      {
        if (s.attacker_power > s.ranked_attackers[s.test_card * 3 + 1])
        {
          for (s.ranked_index = 7; s.test_card < s.ranked_index; s.ranked_index = s.ranked_index - 1)
          {
            s.ranked_attackers[s.ranked_index * 3] = s.ranked_attackers[s.ranked_index * 3 - 3];
            s.ranked_attackers[s.ranked_index * 3 + 1] = s.ranked_attackers[s.ranked_index * 3 - 2];
            s.ranked_attackers[s.ranked_index * 3 + 2] = s.ranked_attackers[s.ranked_index * 3 - 1];
          }
          s.ranked_attackers[s.test_card * 3] = s.card;
          s.ranked_attackers[s.test_card * 3 + 1] = s.attacker_power;
          s.ranked_attackers[s.test_card * 3 + 2] = s.attacker_toughness;
          break;
        }
        s.test_card++;
      }
    }
  }

  s.expected_damage = 0;
  s.ranked_index = 0;
  while (s.ranked_index < 8 && s.ranked_attackers[s.ranked_index * 3 + 1] != -1)
  {
    s.card = s.ranked_attackers[s.ranked_index * 3];
    s.attacker_abilities = C_get_abilities(s.opponent_player, s.card, EVENT_ABILITIES, -1);
    s.attacker_power = s.ranked_attackers[s.ranked_index * 3 + 1];
    s.attacker_toughness = s.ranked_attackers[s.ranked_index * 3 + 2];
    s.best_blocker = 0;
    s.block_result = 0;
    s.creature_index = 0;
    s.smallest_blocker_score = 0x7fff;

    for (s.test_card = 0; s.test_card < active_cards_count[player]; s.test_card = s.test_card + 1)
    {
      if (is_in_play(player, s.test_card) != 0 &&
          (global_cards_data[global_card_instances[player][s.test_card].internal_card_id].type & TYPE_CREATURE) != 0)
      {
        s.creature_index = (int)s.creature_index_by_card[s.test_card];
        s.test_power = s.blocker_power[s.creature_index];
        s.test_toughness = s.blocker_toughness[s.creature_index];
        if ((global_card_instances[player][s.test_card].state &
             ((has_vigilance(player, s.test_card) == 0 ? STATE_ATTACKING : 0) + STATE_BLOCKING)) == 0 &&
            can_block_attacker_with_abilities(player,
                                              s.test_card,
                                              s.opponent_player,
                                              s.card,
                                              s.attacker_abilities,
                                              s.landwalk_mask) != 0)
        {
          s.block_result = 1;
          if (s.attacker_power < s.test_toughness || s.attacker_toughness <= s.test_power)
          {
            s.block_result = 3;
            global_card_instances[player][s.test_card].state |= STATE_BLOCKING;
            break;
          }
          if (s.blocker_scores[s.creature_index] < s.smallest_blocker_score)
          {
            s.smallest_blocker_score = s.blocker_scores[s.creature_index];
            s.best_blocker = s.test_card;
          }
        }
      }
    }

    if ((s.block_result & 2) == 0)
    {
      s.test_toughness = (DAT_007a7d10[player] * s.attacker_power * 0x18) / ClampIntToRange(life[player] + 1, 1, 99);
      s.test_power = (DAT_007a7d10[player + 2] * s.smallest_blocker_score) / 0x10;
      if (s.block_result == 0 || (s.test_toughness < s.test_power && s.expected_damage + s.attacker_power < life[player]))
      {
        s.expected_damage += s.attacker_power;
        score -= s.test_toughness;
      }
      else
      {
        score -= s.test_power;
        global_card_instances[player][s.best_blocker].state |= STATE_BLOCKING;
      }
    }
    s.ranked_index++;
  }

  if (life[player] <= s.expected_damage && life[s.opponent_player] > 0)
  {
    score -= 0x100;
  }

  for (s.card = 0; s.card < 8; s.card = s.card + 1)
  {
    unk_00742f70[s.opponent_player][s.card] = (int)s.saved_special_mana[s.card];
  }

  return score;
}

// FUNCTION: MAGIC 0x004e6754
int show_ai_action_log_dialog(int use_saved_actions, int score)
{
  struct
  {
    int internal_card_id;
    int action_count;
    unsigned int action_flags;
    int action_index;
  } s;

  strcpy(unk_00748770, "AI:");
  strcat(unk_00748770, _itoa(score, ai_action_dialog_number_buffer, 10));
  strcat(unk_00748770, " L:");
  strcat(unk_00748770, _itoa(life[0], ai_action_dialog_number_buffer, 10));
  strcat(unk_00748770, "/");
  strcat(unk_00748770, _itoa(life[1], ai_action_dialog_number_buffer, 10));
  strcat(unk_00748770, " ...\n");

  s.action_index = 0;
  while (1)
  {
    if (use_saved_actions != 0)
    {
      s.action_count = saved_recorded_action_count;
    }
    else
    {
      s.action_count = recorded_action_count;
    }

    if (s.action_count <= s.action_index)
    {
      break;
    }

    if (use_saved_actions != 0)
    {
      s.action_flags = unk_006a1db8[s.action_index];
    }
    else
    {
      s.action_flags = unk_006a21b8[s.action_index];
    }

    if (s.action_flags != (unsigned int)-1)
    {
      if ((s.action_flags & 0x1000) != 0)
      {
        strcat(unk_00748770, "Cast ");
      }
      if ((s.action_flags & 0x2000) != 0)
      {
        strcat(unk_00748770, "Tap ");
      }
      if ((s.action_flags & 0x4000) != 0)
      {
        strcat(unk_00748770, "...target ");
      }
      if ((s.action_flags & 0x100) == 0)
      {
        strcat(unk_00748770, "e");
      }
      if ((unsigned char)s.action_flags == 0xff)
      {
        strcat(unk_00748770, "Player");
      }
      else
      {
        if (use_saved_actions != 0)
        {
          s.internal_card_id = saved_recorded_action_internal_card_ids[s.action_index];
        }
        else
        {
          s.internal_card_id = unk_006a8258[s.action_index];
        }
        strcat(unk_00748770, global_cards_data[s.internal_card_id].name);
      }
      strcat(unk_00748770, "\n");
    }
    s.action_index = s.action_index + 1;
  }

  do_dialog(0, 0, 0, -1, -1, unk_00748770, 0);
  return 0;
}

// FUNCTION: MAGIC 0x004a7ccf
int get_ai_search_elapsed_time(void)
{
  unsigned int current_time;

  current_time = get_duel_thread_time_ms();
  return (int)((current_time - DAT_0064ee98) * 100) / 0x151d;
}

// FUNCTION: MAGIC 0x004a09c6
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
