#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <windows.h>

#include "cardartlib/src/assert.h"
#include "cardartlib/src/palette.h"
#include "deckdll/src/card_db.h"
#include "deckdll/src/magsnd.h"
#include "deckdll/src/shared_resources.h"
#include "drawcardlib/Drawcardlib.h"
#include "duel_engine.h"
#include "game_support.h"
#include "global_strings.h"
#include "shared_startup.h"

typedef ptrdiff_t INT_PTR;

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

typedef struct life_total_dialog_context_struct
{
  char *prompt;
  int initial_life_total;
  int show_reset_button;
} life_total_dialog_context_t;
STATIC_ASSERT(sizeof(life_total_dialog_context_t) == 0xc, life_total_dialog_context_wrong_size);

static void setup_duel_options_dialog_resources(HBITMAP *background,
                                                COLORREF *title_color,
                                                COLORREF *text_color,
                                                HBRUSH *button_brush,
                                                HPEN *pen1,
                                                HPEN *pen2,
                                                COLORREF *unfocus_text_color,
                                                COLORREF *focus_text_color);
static void cleanup_duel_options_dialog_resources(HBITMAP background,
                                                  HBRUSH button_brush,
                                                  HPEN pen1,
                                                  HPEN pen2);
static LRESULT CALLBACK life_total_edit_wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
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

// FUNCTION: MAGIC 0x0044331f
// FUNCTION: SHANDALAR 0x0040ebb9
int play_sound_effect(wav_t sound_id)
{
  struct
  {
    char path[264];
    int replacement_result;
    int sound_num;
    Sound sound;
  } s;

  s.sound.volume = 300;
  s.sound.sampleRate = 0;
  s.sound.pan = 0;
  s.sound.field_C = 0;
  s.sound.field_10 = 0;
  s.sound.field_14 = 0;
  s.sound.loadId = sound_id;
  s.sound.flags = 0;

  if (g_duel_ai_mode_state == 1)
  {
    return 0;
  }

  s.sound_num = sound_id;
  if (s.sound_num <= 0x13)
  {
    sound_play(s.sound_num, 0);
  }
  else if (s.sound_num <= 0x26)
  {
    if (sound_is_loaded(sound_id, &s.sound_num) == 0)
    {
      s.replacement_result = sound_get_lru(&s.sound_num, 0x14, 0x16);
      if (s.replacement_result == 0)
      {
        sound_unload(s.sound_num);
      }
      else if (s.replacement_result != 1)
      {
        return 0;
      }

      strcpy(s.path, global_duelsounds_path);
      strcat(s.path, "\\");
      strcat(s.path, g_duel_sound_filenames[sound_id]);
      sound_load(s.path, s.sound_num, &s.sound);
    }

    sound_play(s.sound_num, 0);
  }
  else if (s.sound_num <= 0x2d)
  {
    if (sound_is_loaded(sound_id, &s.sound_num) == 0)
    {
      s.replacement_result = sound_get_lru(&s.sound_num, 0x27, 0x27);
      if (s.replacement_result == 0)
      {
        sound_unload(s.sound_num);
      }
      else if (s.replacement_result != 1)
      {
        return 0;
      }

      strcpy(s.path, global_duelsounds_path);
      strcat(s.path, "\\");
      strcat(s.path, g_duel_sound_filenames[sound_id + 1]);
      sound_load(s.path, s.sound_num, &s.sound);
    }

    sound_play(s.sound_num, 0);
  }
  else if (s.sound_num <= 0x3b)
  {
    s.sound.volume = 400;
    if (sound_is_loaded(sound_id, &s.sound_num) == 0)
    {
      if (sound_id != WAV_CATATAP)
      {
        s.sound.flags |= 4U;
      }
      else
      {
        s.sound.field_14 = -1;
      }

      strcpy(s.path, global_duelsounds_path);
      strcat(s.path, "\\");
      strcat(s.path, g_duel_sound_filenames[sound_id + 2]);
      sound_load(s.path, s.sound_num, &s.sound);
      sound_play(s.sound_num, &s.sound);
    }
    else
    {
      if (sound_id != WAV_CATATAP)
      {
        s.sound.flags |= 4U;
      }
      else
      {
        s.sound.field_14 = -1;
      }

      sound_play(s.sound_num, &s.sound);
    }
  }
  else if (s.sound_num <= 0x40)
  {
    if (sound_is_loaded(sound_id, &s.sound_num) == 0)
    {
      s.replacement_result = sound_get_lru(&s.sound_num, 0x3c, 0x40);
      if (s.replacement_result == 0)
      {
        sound_unload(s.sound_num);
      }
      else if (s.replacement_result != 1)
      {
        return 0;
      }

      strcpy(s.path, global_duelsounds_path);
      strcat(s.path, "\\");
      strcat(s.path, g_duel_sound_filenames[sound_id + 2]);
      sound_load(s.path, s.sound_num, &s.sound);
    }

    sound_play(s.sound_num, 0);
  }
  else if (s.sound_num <= 0x44)
  {
    if (sound_is_loaded(sound_id, &s.sound_num) == 0)
    {
      s.replacement_result = sound_get_lru(&s.sound_num, 0x41, 0x44);
      if (s.replacement_result == 0)
      {
        sound_unload(s.sound_num);
      }
      else if (s.replacement_result != 1)
      {
        return 0;
      }

      strcpy(s.path, global_duelsounds_path);
      strcat(s.path, "\\");
      strcat(s.path, g_duel_sound_filenames[sound_id + 3]);
      sound_load(s.path, s.sound_num, &s.sound);
    }

    sound_play(s.sound_num, 0);
  }
  else
  {
    return 0;
  }

  return 1;
}

// FUNCTION: MAGIC 0x00446c95
// FUNCTION: SHANDALAR 0x00450813
void request_duel_display_refresh(int player, int card, int unk1, int unk2)
{
  (void)player;
  (void)card;
  (void)unk1;
  (void)unk2;
  SendMessageA((HWND)g_duel_window_hwnd, 0x464, 0xff, 0);
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

// FUNCTION: MAGIC 0x00446da7
// FUNCTION: SHANDALAR 0x00450925
void show_duel_card_preview(unsigned int internal_card_id, int player, int card, int unk1, char *prompt, int unk2)
{
  (void)unk1;
  (void)prompt;
  (void)unk2;

  // The original stores these params to locals even if it won't use them.
  {
    int preview_player;
    int preview_card;

    preview_player = player;
    preview_card = card;

    if (IsWindowVisible((HWND)g_duel_card_preview_window_hwnd))
    {
      if (player != -1 && card != -1)
      {
        SendMessageA((HWND)g_duel_card_preview_window_hwnd, 0x401, CardIDFromType(internal_card_id), (LPARAM)&preview_player);
      }
      else
      {
        SendMessageA((HWND)g_duel_card_preview_window_hwnd, 0x401, CardIDFromType(internal_card_id), 0);
      }
    }
  }
}

// FUNCTION: MAGIC 0x00447f59
int reset_duel_tick_timer(void)
{
  g_affected_card_context_flags_0093f4b4 = GetTickCount();
  g_duel_tick_adjustment = 0;
  return 0;
}

// FUNCTION: MAGIC 0x004480a6
// FUNCTION: SHANDALAR 0x00451c24
void display_duel_prompt_text(char *text)
{
  char *msg;

  if (text != NULL)
  {
    msg = text;
  }
  else
  {
    msg = g_empty_duel_prompt_text;
  }

  set_duel_prompt_context(g_duel_prompt_context_hwnd, msg, 0);
}

// FUNCTION: MAGIC 0x0044837e
// FUNCTION: SHANDALAR 0x00451efb
void notify_library_shuffled(int deck_owner)
{
  HWND hwnd;

  if (deck_owner == 0)
  {
    hwnd = g_duel_main_window_hwnd;
  }
  else
  {
    hwnd = g_duel_status_window_hwnd;
  }

  SendMessageA(hwnd, 0x400, 0, 0);
}

// FUNCTION: MAGIC 0x00449bef
// FUNCTION: SHANDALAR 0x00453770
void copy_opponent_name_prefix(char *name)
{
  char *source;

  if (name == 0)
  {
    return;
  }

  source = g_saved_player_name;
  while (*source != '\0' && *source != '-')
  {
    *name = *source;
    ++source;
    ++name;
  }
  *name = '\0';
}

void default_target_definition(int player, int card, target_definition_t *td, int type)
{
  memset(td, 0, sizeof(*td));
  td->who_chooses = player;
  td->allowed_controller = ANYBODY;
  td->preferred_controller = 1 - player;
  td->zone = TARGET_ZONE_IN_PLAY;
  td->required_type = type;
  td->player = player;
  td->card = card;
}

// FUNCTION: MAGIC 0x0048958a
// FUNCTION: SHANDALAR 0x004a521a
INT_PTR CALLBACK dlgproc_duel_interface_options(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
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
    return (INT_PTR)s.brush;

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

// FUNCTION: MAGIC 0x004964dd
// FUNCTION: SHANDALAR 0x00466f70
int get_card_display_pic_num(card_id_t card_id, int player, int card)
{
  int num_pics;

  if (card_id == -1)
  {
    return 0;
  }
  if (player == -1 || card == -1)
  {
    return 0;
  }
  num_pics = global_raw_cards_storage[card_id].num_pics;
  if (num_pics <= 1)
  {
    return 0;
  }
  else
  {
    return (player + card) % num_pics;
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
INT_PTR CALLBACK dlgproc_prompt_for_life_total(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
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
    return (INT_PTR)GetStockObject(NULL_BRUSH);

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
INT_PTR CALLBACK dlgproc_mana_burn(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
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

// FUNCTION: MAGIC 0x004a616d
// FUNCTION: SHANDALAR 0x005582ed
int update_duel_selection_display_if_human(int player, int phase)
{
  if (g_duel_ai_mode_state == 1)
  {
    return 0;
  }
  return update_duel_selection_display(player, phase);
}

// FUNCTION: MAGIC 0x004a61a1
// FUNCTION: SHANDALAR 0x00558321
void request_duel_display_refresh_if_human(int player, int card, int unk1, int unk2)
{
  if (g_duel_ai_mode_state == 1)
    return;

  request_duel_display_refresh(player, card, unk1, unk2);
}

// FUNCTION: MAGIC 0x004a61d6
// FUNCTION: SHANDALAR 0x00558356
void set_duel_prompt_text(char *text)
{
  strcpy(g_current_duel_prompt_text, text);
  display_duel_prompt_text(text);
}

// FUNCTION: MAGIC 0x004a61fe
int show_cardlist_if_human(int *cards,
                           int count,
                           void *window_title,
                           unsigned int require_selection,
                           char *prompt)
{
  if (g_duel_ai_mode_state == 1)
  {
    return 1;
  }

  return show_cardlist(cards, 0, 0, count, window_title, require_selection, prompt);
}

// FUNCTION: MAGIC 0x004a6245
// FUNCTION: SHANDALAR 0x005583c5
INT_PTR show_damage_assignment_cardlist(int *internal_card_ids, int *damage_amounts, int count, int title, int allow_cancel, char *prompt)
{
  if (g_duel_ai_mode_state == 1)
  {
    return 1;
  }

  return show_cardlist(internal_card_ids, damage_amounts, 0, count, (void *)title, (unsigned int)allow_cancel, prompt);
}

// FUNCTION: MAGIC 0x004a628e
// FUNCTION: SHANDALAR 0x0055840e
INT_PTR show_selectable_cardlist(int *graveyard,
                                 int *available,
                                 int count,
                                 void *window_title,
                                 unsigned int require_selection,
                                 char *prompt)
{
  if (g_duel_ai_mode_state == 1)
  {
    return 1;
  }

  return show_cardlist(graveyard, 0, available, count, window_title, require_selection, prompt);
}

// FUNCTION: MAGIC 0x004a6355
// FUNCTION: SHANDALAR 0x005584d5
void show_card_preview_if_human(unsigned int internal_card_id, int unk1, char *prompt, int unk2)
{
  if (g_duel_ai_mode_state == 1)
  {
    return;
  }

  if (g_duel_active != 0)
  {
    show_duel_card_preview(internal_card_id, -1, -1, unk1, prompt, unk2);
  }
  else
  {
    show_card_image_dialog(internal_card_id, unk1, prompt, unk2);
  }
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

// FUNCTION: MAGIC 0x004b6002
// FUNCTION: SHANDALAR 0x0056c804
void show_duel_system_error(const char *err_msg)
{
#ifdef SHANDALAR
  assert_noabort(0, "D:\\Newmagic\\multiplayer\\sid\\Minit.c", 0x76b, err_msg);
#else
  char buf[500];

  if (g_duel_ai_mode_state != 1)
  {
    LoadTextSectionLines(global_ui_strings_filename, "PROMPT_SYSERR");
    sprintf(buf, g_text_lines[1], err_msg);
    MessageBoxA((HWND)g_duel_window_hwnd, buf, g_text_lines[0], 0);
  }
#endif
}

// FUNCTION: MAGIC 0x004e1b84
int reset_duel_tick_timer_indirect(void)
{
  return reset_duel_tick_timer();
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

// FUNCTION: MAGIC 0x004e1cd1
// FUNCTION: SHANDALAR 0x00426f3c
void redraw_shandalar_duel_screen(int view_player, int present_after_draw)
{
  /*
   * MAGIC keeps Shandalar's two-argument screen redraw hook, but compiles it
   * as a no-op. SHANDALAR's legacy body is only the no-active-duel fallback;
   * active duels redraw through the duel window message path instead.
   */
  (void)view_player;
  (void)present_after_draw;
}

// FUNCTION: MAGIC 0x004ec223
// FUNCTION: SHANDALAR 0x00469f23
void set_duel_prompt_context(HWND prompt_hwnd, char *text, unsigned int button_flags)
{
  struct
  {
    HDC dc2;                 // ebp-0x74
    RECT rect1;              // ebp-0x70
    HDC dc;                  // ebp-0x60
    RECT rect2;              // ebp-0x5c
    SIZE text_size;          // ebp-0x4c
    int border;              // ebp-0x44
    int icon_width;          // ebp-0x40
    RECT rect3;              // ebp-0x3c
    int extra_width;         // ebp-0x2c
    int gap;                 // ebp-0x28
    int y;                   // ebp-0x24
    unsigned int text_width; // ebp-0x20
    int padding;             // ebp-0x1c
    int x;                   // ebp-0x18
    int client_h;            // ebp-0x14
    RECT rect4;              // ebp-0x10
  } s;

  s.border = 4;
  s.padding = 4;
  s.gap = 7;

  if (text == NULL || strlen(text) == 0)
  {
    ShowWindow(prompt_hwnd, 0);
  }

  if (button_flags == 0 || (button_flags & 1) == 0)
  {
    ShowWindow(g_duel_client_window, 0);
  }

  if (button_flags == 0 || (button_flags & 2) == 0)
  {
    ShowWindow(g_duel_main_window, 0);
  }

  s.extra_width = 0;
  s.dc = GetDC(prompt_hwnd);
  GetClientRect(prompt_hwnd, &s.rect2);

  s.client_h = (s.rect2.bottom - s.rect2.top) - s.padding * 2;
  setup_duel_prompt_context_text_dc(prompt_hwnd, s.dc, (int *)&s.rect2.left);

  GetTextExtentPoint32A(s.dc, g_duel_status_text, strlen(g_duel_status_text), &s.text_size);
  s.icon_width = s.text_size.cx + s.client_h / 2;

  s.rect2.right = 2000;
  s.text_width = (unsigned int)(unsigned short)CalcDrawManaText(s.dc, &s.rect2, text);
  ReleaseDC(prompt_hwnd, s.dc);

  if ((button_flags & 2) != 0 || (button_flags & 1) != 0)
  {
    SetWindowPos(g_duel_main_window, (HWND)0, 0, 0, s.icon_width, s.client_h, 6);
    s.extra_width = s.extra_width + s.icon_width + s.gap;
  }

  if ((button_flags & 1) != 0)
  {
    SetWindowPos(g_duel_client_window, (HWND)0, 0, 0, s.icon_width, s.client_h, 6);
    s.extra_width = s.extra_width + s.icon_width + s.gap;
  }

  if (text != NULL && strlen(text) != 0)
  {
    s.dc2 = GetDC(prompt_hwnd);
    GetClientRect(prompt_hwnd, &s.rect1);
    setup_duel_prompt_context_text_dc(prompt_hwnd, s.dc2, (int *)&s.rect1.left);
    s.rect1.right = 2000;
    s.text_width = (unsigned int)(unsigned short)CalcDrawManaText(s.dc2, &s.rect1, text);
    ReleaseDC(prompt_hwnd, s.dc2);
    SetWindowTextA(prompt_hwnd, text);
  }
  else
  {
    s.text_width = 0;
    // STRING: MAGIC 0x0057b054
    SetWindowTextA(prompt_hwnd, "");
  }

  GetWindowRect(prompt_hwnd, &s.rect4);
  SetWindowPos(prompt_hwnd,
               (HWND)0,
               0,
               0,
               s.border * 2 + s.extra_width + s.text_width + 0x19,
               s.rect4.bottom - s.rect4.top,
               6);

  GetClientRect(prompt_hwnd, &s.rect4);
  s.x = s.rect4.left + s.border;

  if ((button_flags & 2) != 0)
  {
    GetWindowRect(g_duel_main_window, &s.rect3);
    s.x = s.x + s.gap;
    s.y = (s.rect4.bottom - s.rect4.top) / 2 - (s.rect3.bottom - s.rect3.top) / 2;
    SetWindowPos(g_duel_main_window, (HWND)0, s.x, s.y, 0, 0, 5);
  }

  if ((button_flags & 1) != 0)
  {
    GetWindowRect(g_duel_client_window, &s.rect3);
    s.x = s.x + (s.rect3.right - s.rect3.left) + s.gap;
    s.y = (s.rect4.bottom - s.rect4.top) / 2 - (s.rect3.bottom - s.rect3.top) / 2;
    SetWindowPos(g_duel_client_window, (HWND)0, s.x, s.y, 0, 0, 5);
  }

  position_duel_prompt_context_window(prompt_hwnd);
  InvalidateRect(g_duel_client_window, (RECT *)0, 1);
  InvalidateRect(g_duel_main_window, (RECT *)0, 1);
  InvalidateRect(prompt_hwnd, (RECT *)0, 1);

  if ((button_flags & 1) != 0)
  {
    ShowWindow(g_duel_client_window, 5);
  }

  if ((button_flags & 2) != 0)
  {
    ShowWindow(g_duel_main_window, 5);
  }

  if (text != NULL)
  {
    if (strlen(text) != 0)
    {
      ShowWindow(prompt_hwnd, 5);
    }
  }

  restack_duel_child_windows();
  UpdateWindow(prompt_hwnd);
}

// FUNCTION: MAGIC 0x004ec616
// FUNCTION: SHANDALAR 0x0046a315
void position_duel_prompt_context_window(HWND prompt_hwnd)
{
  struct
  {
    RECT rect1; // ebp-0x20
    RECT rect2; // ebp-0x10
  } s;

  if (IsWindowVisible((HWND)g_duel_phase_display_window_hwnd))
  {
    GetWindowRect((HWND)g_duel_phase_display_window_hwnd, &s.rect1);
  }
  else if (IsWindowVisible((HWND)g_duel_attack_phase_window_hwnd))
  {
    GetWindowRect(g_attack_phase_window_hwnd, &s.rect1);
  }
  else
  {
    GetWindowRect(g_duel_help_owner_hwnd, &s.rect1);
    GetWindowRect(prompt_hwnd, &s.rect2);
    s.rect1.bottom -= (s.rect2.bottom - s.rect2.top) / 2;
  }

  SetWindowPos(prompt_hwnd, (HWND)0, s.rect1.left, s.rect1.bottom, 0, 0, 5);
}

// FUNCTION: MAGIC 0x004ec6c3
// FUNCTION: SHANDALAR 0x0046a3c1
void setup_duel_prompt_context_text_dc(HWND prompt_hwnd, HDC dc, int *rect_values)
{
  struct
  {
    RECT rect;        // ebp-0x4c
    HGDIOBJ wnd_font; // ebp-0x3c
    TEXTMETRICA tm;   // ebp-0x38
  } s;

  s.wnd_font = (HGDIOBJ)GetWindowLongA(prompt_hwnd, g_duel_ui_initialized);
  if (s.wnd_font != (HGDIOBJ)0)
  {
    SelectObject(dc, s.wnd_font);
  }

  GetTextMetricsA(dc, &s.tm);

  *rect_values += s.tm.tmHeight / 2;
  rect_values[1] += 4;
  rect_values[3] -= 3;

  if (IsWindowVisible(g_duel_client_window))
  {
    GetWindowRect(g_duel_client_window, &s.rect);
    MapWindowPoints((HWND)0, prompt_hwnd, (LPPOINT)&s.rect, 2);
    *rect_values = max(s.rect.right + s.tm.tmHeight / 2, *rect_values);
  }

  if (IsWindowVisible(g_duel_main_window))
  {
    GetWindowRect(g_duel_main_window, &s.rect);
    MapWindowPoints((HWND)0, prompt_hwnd, (LPPOINT)&s.rect, 2);
    *rect_values = max(s.rect.right + s.tm.tmHeight / 2, *rect_values);
  }

  SetMapMode(dc, 8);
  SetWindowExtEx(dc, rect_values[2] - *rect_values, 0x14, (LPSIZE)0);
  SetViewportExtEx(dc, rect_values[2] - *rect_values, rect_values[3] - rect_values[1], (LPSIZE)0);
}

#ifndef SHANDALAR
// FUNCTION: MAGIC 0x0050aa5d
void ShowMouseCursorNested(void)
{
  // This space intentionally left blank (empty function)
}

// FUNCTION: MAGIC 0x0050aa68
void HideMouseCursorNested(void)
{
  // This space intentionally left blank (empty function)
}

#endif
