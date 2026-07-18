#include <windows.h>
#include <commdlg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "cardartlib/src/assert.h"
#include "cardartlib/src/palette.h"
#include "defs.h"
#include "magic/src/duel_engine.h"
#include "magic/src/game_support.h"
#include "magic/src/global_duel_ui_ids.h"
#include "magic/src/global_other.h"
#include "magic/src/global_state.h"
#include "magic/src/global_strings.h"
#include "magic/src/shared_startup.h"
#include "drawcardlib/src/pic.h"
#ifdef SHANDALAR
#include "deckdll/src/magsnd.h"
#include "shandalar/src/shandalar_global_strings.h"
#endif

#ifndef SHANDALAR
extern HWND global_main_hwnd;
#define DUEL_SHELL_WINDOW_HWND g_duel_parent_window_hwnd
#define DUEL_MAIN_WINDOW_HWND global_main_hwnd
#else
extern HWND g_main_window_hwnd;
#define DUEL_SHELL_WINDOW_HWND g_main_window_hwnd
#define DUEL_MAIN_WINDOW_HWND g_main_window_hwnd
#endif

extern int life[2];
extern HINSTANCE g_app_instance;
extern HWND DAT_008a8dec;
extern HWND DAT_008a8d78;
extern int DAT_007a7d74;
extern int g_showlist_smallcard_width;
extern int g_showlist_smallcard_height;
extern card_ptr_t global_raw_cards_storage[2000];
extern CRITICAL_SECTION g_card_render_lock;
void checked_DeleteDC_DeleteObject(HDC dc, HGDIOBJ obj);
void set_duel_prompt_context(HWND hwnd, char *prompt, unsigned int mode);
int prompt_for_life_total(int player, char *prompt, int maxnum);
int get_battlefield_card_stagger_offset(HWND hwnd);
LONG get_card_window_hidden_flag(HWND hwnd);
void reset_battlefield_layout_positions(HWND hwnd);
void get_next_battlefield_card_position(HWND parent, int *rect, int value, int *x, int *y, int flag);
void set_duel_tooltip_text(char *text);
void resize_battlefield_child_card_windows(HWND hwnd);
void layout_attack_phase_window(HWND hwnd);
void layout_phase_display_window(HWND hwnd, LPRECT rect);
void resize_duel_hand_window(HWND hwnd);
void set_player_directive_value(int player, int value);
void get_current_duel_selection(int *selected_player, int *selected_card);
unsigned int load_gametype0(char *path);
void copy_autosave_to_save_file(LPCSTR save_path);
DWORD WINAPI start_duel_thread(LPVOID unused);
void StopWorldLocationMusic(void);
void reset_duel_globals(void);
void reset_timestamp_players(void);
void load_selected_duel_save_slot(int player);
void load_duel_run_mode_1_save(char *path);
void load_duel_run_mode_2_save(char *path);
void load_duel_run_mode_3_save(char *path);
int check_duel_finished(void);
void run_duel_turn(unsigned int player);
int CountDuelPoolEligibleTowns(void);
char *GetCreatureName(int creature_type);
#ifdef SHANDALAR
int SeedRandomFromTickCount(void);
#else
void SeedRandomFromTickCount(void);
#endif

int AddCardToDeckSorted(int card_id);
int draw_random_unmarked_deck_card(void);
void copy_initial_library_to_player_zero(int wizard_color);
void remove_iid_from_initial_library(int player, int internal_card_id);
int DrawRandomCardFromInitialLibrary(int library_index);
void perform_player_mulligan(int player, int wizard_color);
void evaluate_opening_hand_land_counts(int *redraw_result, int *ante_result, int *ante_info);
void TENTATIVE_reassess_all_cards();
void append_to_trace_txt(char *text);
void AddCardToCLPacket(int card_in_packet);
int GetCardFromCLPacket(int packet_index);
unsigned int refresh_duel_display_cache(void);
unsigned int get_displayed_card_special_counters(int player, int card);
int get_displayed_card_internal_id(int player, int card);
int get_displayed_card_id(int player, int card);
int get_displayed_card_zone(int player, int card);
unsigned int get_displayed_card_ui_flags(int player, int card);
void get_displayed_card_attachment(int *player_and_card, int player, int card);
unsigned int get_displayed_card_display_flags(int player, int card);
int is_attack_phase_window_enabled(void);
int find_attack_phase_card_window(HWND hwnd, int *player_and_card, int *unused1, HWND *child_hwnd, int *unused2);
int find_battlefield_card_window(HWND hwnd, int *player_and_card, int *unused, HWND *child_hwnd);
extern int DAT_008ced00[16];
#ifdef SHANDALAR
int DAT_007a79b8;
#else
extern int DAT_007a79b8;
#endif
#ifndef SHANDALAR
extern HANDLE global_mutex_GameInit;
#endif
#ifdef SHANDALAR
extern int g_world_location_music_active;
extern int DAT_007483f0;
extern int _DAT_007483f4;
int single_color_test_bit_to_color_t(int color_mask);
void AddJournalEntry(int entry_type, int entry_arg);
#endif
int GetCardRarity(int card_id);

// GLOBAL: MAGIC 0x008a9140
OPENFILENAMEA g_duel_save_game_openfilename;

// GLOBAL: MAGIC 0x00746368
void *g_duel_player_face_pic;

// GLOBAL: MAGIC 0x00579f28
// GLOBAL: SHANDALAR 0x0058f658
char s_Congratulations_00579f28[0x14] = "Congratulations!";

// GLOBAL: MAGIC 0x00579f3c
// GLOBAL: SHANDALAR 0x0058f66c
char s_Too_bad_00579f3c[8] = "Too bad";

// GLOBAL: MAGIC 0x00579f44
// GLOBAL: SHANDALAR 0x0058f674
char s_Oh_well_00579f44[0xc] = "Oh well...";

// GLOBAL: MAGIC 0x00579f50
// GLOBAL: SHANDALAR 0x0058f680
char s_Want_to_play_again_00579f50[0x18] = "\n\nWant to play again?";

// GLOBAL: MAGIC 0x00579f68
// GLOBAL: SHANDALAR 0x0058f698
char s_End_of_duel_00579f68[0xc] = "End of duel";

// GLOBAL: MAGIC 0x0091c9a4
// GLOBAL: SHANDALAR 0x00930ae4
HANDLE g_duel_thread_handle;

// GLOBAL: MAGIC 0x00579f24
// GLOBAL: SHANDALAR 0x0058f654
int g_duel_main_window_minimized;

// GLOBAL: MAGIC 0x008b3bcc
// GLOBAL: SHANDALAR 0x008c7d4c
HWND g_duel_palette_window_hwnd;

// GLOBAL: MAGIC 0x008cc700
// GLOBAL: SHANDALAR 0x008e0880
int g_duel_modal_action_active;

// GLOBAL: MAGIC 0x008ce504
int DAT_008ce504;

// GLOBAL: MAGIC 0x00777aa4
// GLOBAL: SHANDALAR 0x0078e834
int g_duel_selected_player_card;

// GLOBAL: MAGIC 0x008a8d70
// GLOBAL: SHANDALAR 0x008bcf70
int g_duel_selected_opponent_card;

// GLOBAL: MAGIC 0x008b294c
// GLOBAL: SHANDALAR 0x008c6afc
int g_duel_selection_pending;

extern int g_duel_cached_life_player_1;
extern int g_duel_cached_life_player_0;
extern int g_duel_cached_state_007abce4;
extern int g_duel_cached_state_007abce0;
extern int g_duel_cached_unk_008b44d0_player_1;
extern int g_duel_cached_unk_008b44d0_player_0;

// GLOBAL: MAGIC 0x0094ca2c
HWND g_duel_toggleable_status_window_hwnd;

extern int g_duel_cached_library_count_player_1;
extern int g_duel_cached_library_count_player_0;
extern int g_duel_cached_graveyard_count_player_1;
extern int g_duel_cached_graveyard_count_player_0;
extern int g_duel_cached_exile_count_player_1;
extern int g_duel_cached_exile_count_player_0;
extern HWND g_duel_prompt_context_hwnd;

// GLOBAL: MAGIC 0x008b4370
// GLOBAL: SHANDALAR 0x008c84f0
BITMAPINFO g_duel_backbuffer_bmi;

// GLOBAL: MAGIC 0x00925ad0
// GLOBAL: SHANDALAR 0x00939c00
char g_duel_action_request_copy[0xe8];

// GLOBAL: MAGIC 0x008961b0
// GLOBAL: SHANDALAR 0x008aa3b0
int g_duel_current_selection_forced;

// GLOBAL: MAGIC 0x0074a230
// GLOBAL: SHANDALAR 0x00746b08
int g_duel_main_window_closing;

// GLOBAL: MAGIC 0x00579f20
// GLOBAL: SHANDALAR 0x0058f650
int g_duel_main_window_deferred_resize;

// GLOBAL: MAGIC 0x0055e168
int g_duel_window_userdata_player_offset;

// GLOBAL: MAGIC 0x0055e16c
int g_duel_window_userdata_card_offset = 4;

// GLOBAL: MAGIC 0x00939508
int DAT_00939508;

// GLOBAL: MAGIC 0x00789710
int DAT_00789710;

// FUNCTION: MAGIC 0x00447627
// FUNCTION: SHANDALAR 0x004511a5
void notify_duel_action(int player, unsigned int value)
{
  struct
  {
    HWND other_life_window;
    HWND life_window;
    int selected_card;
    HWND primary_battlefield_window;
    HWND child_window;
    HWND secondary_battlefield_window;
    int attached_player_and_card[2];
    int action_player;
    int action_card;
    int card_id;
    int card_index;
    int zone;
    int internal_card_id;
    unsigned int display_flags;
    int player_index;
    unsigned int need_redraw;
    unsigned int card_flags;
    WPARAM attack_phase_refresh_flag;
  } s;

  KillTimer(g_duel_window_hwnd, g_duel_timer_id);
  EnterCriticalSection(&g_duel_render_lock);
  if (g_duel_extra_turn_player != DAT_00789710)
  {
    DAT_00789710 = g_duel_extra_turn_player;
    InvalidateRect(DAT_008a8dec, (RECT *)0, 0);
  }
  LeaveCriticalSection(&g_duel_render_lock);

  s.need_redraw = refresh_duel_display_cache();

  EnterCriticalSection(&g_duel_render_lock);
  for (s.player_index = 0; s.player_index < 2; s.player_index++)
  {
    for (s.card_index = 0; s.card_index < 0x96; s.card_index++)
    {
      if (get_displayed_card_id(s.player_index, s.card_index) == unk_009266ac)
      {
        global_displayed_card_instances[s.player_index][s.card_index].state &= ~STATE_IN_PLAY;
      }
    }
  }
  LeaveCriticalSection(&g_duel_render_lock);

  if ((value == 0) || ((value & 0x30) != 0))
  {
    if (s.need_redraw != 0)
    {
      for (s.player_index = 0; s.player_index < 2; s.player_index++)
      {
        for (s.card_index = 0; s.card_index < 0x96; s.card_index++)
        {
          s.action_player = s.player_index;
          s.action_card = s.card_index;
          s.zone = get_displayed_card_zone(s.player_index, s.card_index);
          s.internal_card_id = get_displayed_card_internal_id(s.player_index, s.card_index);
          s.card_id = get_displayed_card_id(s.player_index, s.card_index);
          s.display_flags = get_displayed_card_display_flags(s.player_index, s.card_index);
          s.card_flags = get_displayed_card_ui_flags(s.player_index, s.card_index);

          if (((s.internal_card_id == -1) ||
               (((s.display_flags & 0x10000) != 0) && (g_duel_interface_options.show_invisible_effect_cards == 0))) ||
              ((unk_009266a4 == s.internal_card_id) && (get_displayed_card_special_counters(s.player_index, s.card_index) == 0)))
          {
            SendMessageA(g_duel_full_card_window_hwnd, 0x40b, (WPARAM)&s.action_player, 0);
            SendMessageA(g_duel_life_window_hwnd, 0x40b, (WPARAM)&s.action_player, 0);
            SendMessageA(g_duel_player_battlefield_window_hwnd, 0x40b, (WPARAM)&s.action_player, 0);
            SendMessageA(g_duel_help_owner_hwnd, 0x40b, (WPARAM)&s.action_player, 0);
            SendMessageA(g_duel_card_preview_window_hwnd, 0x40b, (WPARAM)&s.action_player, 0);
            if ((IsWindowVisible(g_duel_attack_phase_window_hwnd) != 0) || (is_attack_phase_window_enabled() != 0))
            {
              SendMessageA(g_duel_attack_phase_window_hwnd, 0x403, (WPARAM)&s.action_player, 0);
              SendMessageA(g_duel_attack_phase_window_hwnd, 0x402, (WPARAM)&s.action_player, 0);
            }
          }
          else if (s.zone == 2)
          {
            SendMessageA(g_duel_full_card_window_hwnd, 0x40b, (WPARAM)&s.action_player, 0);
            SendMessageA(g_duel_life_window_hwnd, 0x40b, (WPARAM)&s.action_player, 0);
            SendMessageA(g_duel_player_battlefield_window_hwnd, 0x40b, (WPARAM)&s.action_player, 0);
            SendMessageA(g_duel_help_owner_hwnd, 0x40b, (WPARAM)&s.action_player, 0);
          }
          else if (unk_0091a80c == s.internal_card_id)
          {
          }
          else
          {
            if (s.zone == 1)
            {
              SendMessageA(g_duel_full_card_window_hwnd, 0x40b, (WPARAM)&s.action_player, 0);
              SendMessageA(g_duel_life_window_hwnd, 0x40b, (WPARAM)&s.action_player, 0);
              if (((s.card_flags & 0x10) != 0) &&
                  ((unk_007a7d64 <= s.card_id ||
                    ((global_raw_cards_storage[s.card_id].db_card_type_2 == 2 &&
                      (*(int *)&global_raw_cards_storage[s.card_id].rarity != 0xd3))))))
              {
                get_displayed_card_attachment(s.attached_player_and_card, s.player_index, s.card_index);
                while (((get_displayed_card_ui_flags(s.attached_player_and_card[0], s.attached_player_and_card[1]) & 0x10) != 0 &&
                        ((s.card_id = get_displayed_card_id(s.attached_player_and_card[0], s.attached_player_and_card[1])) != -1)) &&
                       (unk_007a7d64 <= s.card_id ||
                        ((global_raw_cards_storage[s.card_id].db_card_type_2 == 2 &&
                          (*(int *)&global_raw_cards_storage[s.card_id].rarity != 0xd3)))))
                {
                  get_displayed_card_attachment(s.attached_player_and_card, s.attached_player_and_card[0], s.attached_player_and_card[1]);
                }
                if (s.attached_player_and_card[0] == 0)
                {
                  s.secondary_battlefield_window = g_duel_player_battlefield_window_hwnd;
                }
                else
                {
                  s.secondary_battlefield_window = g_duel_help_owner_hwnd;
                }
                if (s.attached_player_and_card[0] == 0)
                {
                  s.primary_battlefield_window = g_duel_help_owner_hwnd;
                }
                else
                {
                  s.primary_battlefield_window = g_duel_player_battlefield_window_hwnd;
                }
                SendMessageA(s.primary_battlefield_window, 0x40b, (WPARAM)&s.action_player, 0);
                SendMessageA(s.secondary_battlefield_window, 0x40a, (WPARAM)&s.action_player, 0);
                if (find_attack_phase_card_window(g_duel_attack_phase_window_hwnd, s.attached_player_and_card, (int *)0, &s.child_window, (int *)0) != 0)
                {
                  SendMessageA(g_duel_attack_phase_window_hwnd, 0x406, (WPARAM)&s.action_player, (LPARAM)s.child_window);
                  SendMessageA(g_duel_attack_phase_window_hwnd, 0x410, (WPARAM)s.child_window, 0);
                  s.attack_phase_refresh_flag = 1;
                  if (((find_battlefield_card_window(s.secondary_battlefield_window, s.attached_player_and_card, (int *)0, &s.child_window) != 0) &&
                       (IsWindowVisible(s.child_window) == 0)) &&
                      (find_battlefield_card_window(s.secondary_battlefield_window, &s.action_player, (int *)0, &s.child_window) != 0))
                  {
                    ShowWindow(s.child_window, SW_HIDE);
                  }
                }
              }
              else
              {
                if (s.player_index == 0)
                {
                  s.secondary_battlefield_window = g_duel_player_battlefield_window_hwnd;
                }
                else
                {
                  s.secondary_battlefield_window = g_duel_help_owner_hwnd;
                }
                if (s.player_index == 0)
                {
                  s.primary_battlefield_window = g_duel_help_owner_hwnd;
                }
                else
                {
                  s.primary_battlefield_window = g_duel_player_battlefield_window_hwnd;
                }
                SendMessageA(s.primary_battlefield_window, 0x40b, (WPARAM)&s.action_player, 0);
                SendMessageA(s.secondary_battlefield_window, 0x40a, (WPARAM)&s.action_player, 0);
              }
            }
            else if (s.zone == 0)
            {
              SendMessageA(g_duel_player_battlefield_window_hwnd, 0x40b, (WPARAM)&s.action_player, 0);
              SendMessageA(g_duel_help_owner_hwnd, 0x40b, (WPARAM)&s.action_player, 0);
              if (s.player_index == 0)
              {
                s.life_window = g_duel_life_window_hwnd;
              }
              else
              {
                s.life_window = g_duel_full_card_window_hwnd;
              }
              SendMessageA(s.life_window, 0x40b, (WPARAM)&s.action_player, 0);
              if (s.player_index == 0)
              {
                s.other_life_window = g_duel_full_card_window_hwnd;
              }
              else
              {
                s.other_life_window = g_duel_life_window_hwnd;
              }
              SendMessageA(s.other_life_window, 0x40a, (WPARAM)&s.action_player, 0);
            }
          }
        }
      }
    }

    SendMessageA(g_duel_player_battlefield_window_hwnd, 0x400, 0, 0);
    SendMessageA(g_duel_help_owner_hwnd, 0x400, 0, 0);
    get_current_duel_selection((int *)0, &s.selected_card);
    if ((0x14 < s.selected_card) && (s.selected_card < 0x1e))
    {
      SendMessageA(g_duel_attack_phase_window_hwnd, 0x412, s.attack_phase_refresh_flag, 0);
    }
    else
    {
      SendMessageA(g_duel_attack_phase_window_hwnd, 0x40c, 0, 0);
    }
    SendMessageA(g_duel_phase_display_window_hwnd, 0x412, 0, 0);
  }

  SendMessageA(g_duel_player_battlefield_window_hwnd, 0x412, 0, 0);
  SendMessageA(g_duel_help_owner_hwnd, 0x412, 0, 0);
  if ((value == 0) || ((value & 0x20) != 0))
  {
    SendMessageA(g_duel_full_card_window_hwnd, 0x432, 0, 0);
    SendMessageA(g_duel_life_window_hwnd, 0x432, 0, 0);
    SendMessageA(g_duel_player_battlefield_window_hwnd, 0x432, 0, 0);
    SendMessageA(g_duel_help_owner_hwnd, 0x432, 0, 0);
    if (IsWindowVisible(g_duel_attack_phase_window_hwnd) != 0)
    {
      SendMessageA(g_duel_attack_phase_window_hwnd, 0x432, 0, 0);
      UpdateWindow(g_duel_attack_phase_window_hwnd);
    }
    if (IsWindowVisible(g_duel_phase_display_window_hwnd) != 0)
    {
      SendMessageA(g_duel_phase_display_window_hwnd, 0x432, 0, 0);
      UpdateWindow(g_duel_phase_display_window_hwnd);
    }
    SendMessageA(g_duel_card_preview_window_hwnd, 0x432, 0, 0);
  }
  SendMessageA(g_duel_life_status_window_1_hwnd, 0x432, 0, 0);
  SendMessageA(g_duel_life_status_window_2_hwnd, 0x432, 0, 0);
  SendMessageA(unk_008ce534, 0x432, 0, 0);
  SendMessageA(unk_00939344, 0x432, 0, 0);
  SendMessageA(DAT_0091ce30, 0x432, 0, 0);
  SendMessageA(DAT_0092680c, 0x432, 0, 0);
  SendMessageA(g_duel_player_graveyard_window_hwnd, 0x432, 0, 0);
  SendMessageA(g_phasebar_your_untap_window_hwnd, 0x432, 0, 0);
  UpdateWindow(g_duel_window_hwnd);
}

// FUNCTION: MAGIC 0x004488c5
int get_displayed_card_zone(int player, int card)
{
  card_instance_t *instance;
  int result;

  if (((player == 0) || (player == 1)) && (card >= 0) && (card <= 0x96))
  {
    EnterCriticalSection(&g_duel_render_lock);
    instance = &global_displayed_card_instances[player][card];
    if ((instance->state & STATE_IN_PLAY) == 0)
    {
      if ((instance->state & STATE_INVISIBLE) == 0)
      {
        result = 0;
      }
      else
      {
        result = 2;
      }
    }
    else
    {
      result = 1;
    }
    LeaveCriticalSection(&g_duel_render_lock);
  }
  else
  {
    result = 0;
  }
  return result;
}

// FUNCTION: MAGIC 0x00448999
unsigned int get_displayed_card_ui_flags(int player, int card)
{
  card_instance_t *instance;
  unsigned char result;

  if (((player == 0) || (player == 1)) && (card >= 0) && (card <= 0x96))
  {
    EnterCriticalSection(&g_duel_render_lock);
    instance = &global_displayed_card_instances[player][card];
    result = (unsigned char)((instance->regen_status & 3) != 0);
    if (((instance->state & STATE_TAPPED) != 0) &&
        ((global_cards_data[instance->internal_card_id].type & 0x47) != TYPE_ENCHANTMENT))
    {
      result |= 2;
    }
    if ((instance->state & STATE_ATTACKING) != 0)
    {
      result |= 4;
    }
    if (instance->damage_target_player != -1)
    {
      result |= 8;
    }
    if ((instance->parent_controller != -1) && (instance->damage_target_card != -1))
    {
      result |= 0x10;
    }
    if ((instance->regen_status & 0x800000) != 0)
    {
      result |= 0x20;
    }
    LeaveCriticalSection(&g_duel_render_lock);
  }
  else
  {
    result = 0;
  }
  return result;
}

// FUNCTION: MAGIC 0x00448bc6
void get_displayed_card_attachment(int *player_and_card, int player, int card)
{
  card_instance_t *instance;

  if (player_and_card != (int *)0)
  {
    if (((player == 0) || (player == 1)) && (card >= 0) && (card <= 0x96))
    {
      EnterCriticalSection(&g_duel_render_lock);
      instance = &global_displayed_card_instances[player][card];
      player_and_card[0] = instance->damage_target_player;
      player_and_card[1] = instance->damage_target_card;
      LeaveCriticalSection(&g_duel_render_lock);
    }
    else
    {
      player_and_card[0] = -1;
      player_and_card[1] = -1;
    }
  }
}

// FUNCTION: MAGIC 0x0044a342
int is_attack_phase_window_enabled(void)
{
  int result;

  EnterCriticalSection(&g_duel_render_lock);
  result = g_duel_selection_pending;
  LeaveCriticalSection(&g_duel_render_lock);
  return result;
}

// FUNCTION: MAGIC 0x004d8879
int find_attack_phase_card_window(HWND hwnd, int *player_and_card, int *unused1, HWND *child_hwnd, int *unused2)
{
  (void)hwnd;
  (void)player_and_card;
  (void)unused1;
  (void)child_hwnd;
  (void)unused2;
  return 0;
}

// FUNCTION: MAGIC 0x004e994f
int find_battlefield_card_window(HWND hwnd, int *player_and_card, int *unused, HWND *child_hwnd)
{
  (void)hwnd;
  (void)player_and_card;
  (void)unused;
  (void)child_hwnd;
  return 0;
}

// FUNCTION: MAGIC 0x004d25fe
LONG get_card_window_hidden_flag(HWND hwnd)
{
  return GetWindowLongA(hwnd, 8);
}

// FUNCTION: MAGIC 0x004d153b
int get_battlefield_card_stagger_offset(HWND hwnd)
{
  int card;
  int card_id;
  int player;
  unsigned int sign;

  player = GetWindowLongA(hwnd, 0);
  card = GetWindowLongA(hwnd, 4);
  card_id = get_displayed_card_id(player, card);
  sign = (int)card_id >> 0x1f;
  return (((card_id ^ sign) - sign & 3 ^ sign) - sign) * 2 + (card % 3) * 3;
}

// FUNCTION: MAGIC 0x004e915a
void reset_battlefield_layout_positions(HWND hwnd)
{
  (void)hwnd;
}

// FUNCTION: MAGIC 0x004e9256
void get_next_battlefield_card_position(HWND parent, int *rect, int value, int *x, int *y, int flag)
{
  (void)parent;
  (void)rect;
  (void)value;
  (void)x;
  (void)y;
  (void)flag;
}

// FUNCTION: MAGIC 0x004e8e42
void resize_battlefield_child_card_windows(HWND hwnd)
{
  (void)hwnd;
}

// FUNCTION: MAGIC 0x004d6c6d
void layout_attack_phase_window(HWND hwnd)
{
  (void)hwnd;
}

// FUNCTION: MAGIC 0x0048894d
void layout_phase_display_window(HWND hwnd, LPRECT rect)
{
  (void)hwnd;
  (void)rect;
}

// FUNCTION: MAGIC 0x004dc512
// FUNCTION: SHANDALAR 0x005431cd
void layout_duel_child_windows(HWND hwnd, int layout)
{
  struct
  {
    char chat_text2[200];
    unsigned int chat_text2_length;
    CHAR tooltip_text2[100];
    char chat_text1[200];
    unsigned int chat_text1_length;
    CHAR tooltip_text1[100];
    int local_f0;
    int preview_height;
    int opponent_chat_y;
    int lower_life_y;
    int phasebar_width;
    int opponent_chat_x;
    int center_column_width;
    int preview_x;
    int preview_y;
    int half_phasebar_height;
    int chat_y;
    int left_phasebar_width;
    int bottom_half_y;
    int client_height;       // ebp - 0xb8
    POINT point;             // ebp - 0xb4
    int lower_middle_height; // ebp - 0xac
    int local_ac;            // ebp - 0xa8
    int divider_width;       // ebp - 0xa4
    int chat_height;         // ebp - 0xa0
    int client_width;        // ebp - 0x9c
    int top_middle_height;   // ebp - 0x98
    int face_y;              // ebp - 0x94
    int face_height;         // ebp - 0x90
    int player_life_x;       // ebp - 0x8c
    int preview_left;        // ebp - 0x88
    int preview_top;         // ebp - 0x84
    int right_column_y;      // ebp - 0x80
    int local_80;            // ebp - 0x7c
    int lower_phasebar_y;
    int right_chat_x;
    int player_chat_y;
    int opponent_life_y;
    int local_70;
    int lower_phasebar_height;
    int small_chat_height;
    int battlefield_width;
    int player_face_x;
    int lower_middle_y;
    int face_x;
    int opponent_phasebar_x;
    int face_top;
    int right_column_x;
    int preview_width;
    int prompt_x;
    int top_battlefield_height;
    int bottom_battlefield_height;
    int top_face_height;
    int right_column_width;
    RECT rect;
    int player_life_height;
    int phasebar_height;
    int right_phasebar_x;
    int player_battlefield_width;
    int opponent_life_x;
    int right_battlefield_x;
  } s;

#ifdef MODERN_FIXES
  memset(&s, 0, sizeof(s));
#endif

  GetClientRect(hwnd, &s.rect);
  g_showlist_smallcard_width = s.rect.right / 8;
  g_showlist_smallcard_height = g_showlist_smallcard_width;
  DAT_00939508 = (g_showlist_smallcard_height * 0x21) / 0x118;

  if (layout == 1)
  {
    GetWindowTextA((HWND)DAT_007a7d74, s.tooltip_text1, 100);
    set_duel_tooltip_text(s.tooltip_text1);
    GetClientRect(hwnd, &s.rect);
    s.client_width = s.rect.right;
    s.client_height = s.rect.bottom;
    s.preview_width = s.client_width / 4;
    s.preview_height = s.client_height / 2;
    s.preview_left = s.rect.left;
    s.preview_top = s.rect.top + (s.client_height - s.preview_height) / 2;
    s.point.x = s.preview_left;
    s.point.y = s.preview_top;
    ClientToScreen(hwnd, &s.point);
    MoveWindow(g_duel_card_preview_window_hwnd, s.point.x, s.point.y, s.preview_width, s.preview_height, 1);
    GetWindowRect(g_duel_card_preview_window_hwnd, &s.rect);
    s.preview_width = s.rect.right - s.rect.left;
    s.preview_height = s.rect.bottom - s.rect.top;
    GetClientRect(hwnd, &s.rect);
    s.client_width = s.rect.right;
    s.client_height = s.rect.bottom;
    s.left_phasebar_width = s.preview_width / 2;
    s.preview_y = s.rect.top;
    s.player_face_x = s.preview_height + s.preview_top;
    s.battlefield_width = (s.preview_width + s.preview_left) - s.left_phasebar_width;
    s.preview_x = s.battlefield_width;
    s.top_face_height = s.preview_top - s.preview_y;
    s.lower_middle_height = s.rect.bottom - s.player_face_x;
    s.phasebar_width = s.preview_width / 4;
    s.lower_phasebar_y = (s.preview_width - s.left_phasebar_width) - s.phasebar_width;
    s.half_phasebar_height = s.top_face_height / 2;
    s.phasebar_height = s.half_phasebar_height;
    s.face_x = s.preview_x - s.phasebar_width;
    s.right_phasebar_x = s.face_x;
    s.local_f0 = s.right_phasebar_x - s.lower_phasebar_y;
    s.local_ac = s.local_f0;
    s.opponent_phasebar_x = s.preview_top - s.half_phasebar_height;
    s.lower_middle_y = s.opponent_phasebar_x;
    s.local_70 = s.preview_height + s.preview_top;
    s.local_80 = s.local_70;
    s.small_chat_height = s.preview_width - s.left_phasebar_width;
    s.player_life_x = s.rect.left;
    s.opponent_life_x = s.player_life_x;
    s.opponent_life_y = s.rect.top;
    s.lower_life_y = s.half_phasebar_height + s.local_70;
    s.top_middle_height = s.opponent_phasebar_x - s.opponent_life_y;
    s.player_life_height = s.rect.bottom - s.lower_life_y;
    s.face_top = s.preview_width + s.preview_left;
    s.face_y = s.rect.top;
    s.divider_width = s.small_chat_height / 3;
    s.face_height = s.client_height;
    s.player_battlefield_width = (s.client_width - s.preview_width) - s.divider_width;
    s.top_battlefield_height = s.client_height / 2;
    s.bottom_battlefield_height = s.client_height - s.top_battlefield_height;
    s.right_battlefield_x = s.divider_width + s.face_top;
    s.prompt_x = s.right_battlefield_x;
    s.bottom_half_y = s.rect.bottom - s.bottom_battlefield_height;
    s.right_column_y = s.rect.top;
    s.right_column_width = s.player_battlefield_width;
    s.chat_height = (s.bottom_battlefield_height * 9) / 100;
    s.right_column_x = s.prompt_x;
    s.chat_y = s.bottom_half_y - s.chat_height / 2;
    s.center_column_width = s.player_battlefield_width;
    s.lower_phasebar_height = s.client_height / 0x28;
    s.opponent_chat_x = s.right_battlefield_x;
    s.opponent_chat_y = s.right_column_y;
    s.right_chat_x = s.prompt_x;
    s.player_chat_y = s.bottom_half_y - s.lower_phasebar_height;

    ShowWindow(g_duel_card_preview_window_hwnd, SW_SHOW);
    MoveWindow(g_duel_life_status_window_2_hwnd, s.opponent_life_x, s.opponent_life_y, s.small_chat_height, s.top_middle_height, 1);
    MoveWindow(g_duel_life_status_window_1_hwnd, s.player_life_x, s.lower_life_y, s.small_chat_height, s.player_life_height, 1);
    MoveWindow(unk_00939344, s.preview_x, s.preview_y, s.left_phasebar_width, s.top_face_height, 1);
    MoveWindow(unk_008ce534, s.battlefield_width, s.player_face_x, s.left_phasebar_width, s.lower_middle_height, 1);
    MoveWindow(DAT_0092680c, s.local_ac, s.lower_middle_y, s.lower_phasebar_y, s.phasebar_height, 1);
    MoveWindow(DAT_0091ce30, s.local_f0, s.local_80, s.lower_phasebar_y, s.phasebar_height, 1);
    MoveWindow(g_phasebar_your_untap_window_hwnd, s.right_phasebar_x, s.opponent_phasebar_x, s.phasebar_width, s.half_phasebar_height, 1);
    MoveWindow(g_duel_player_graveyard_window_hwnd, s.face_x, s.local_70, s.phasebar_width, s.half_phasebar_height, 1);
    s.point.x = s.preview_left;
    s.point.y = s.preview_top;
    ClientToScreen(hwnd, &s.point);
    MoveWindow(g_duel_card_preview_window_hwnd, s.point.x, s.point.y, s.preview_width, s.preview_height, 1);
    MoveWindow(g_duel_help_owner_hwnd, s.right_battlefield_x, s.right_column_y, s.player_battlefield_width, s.top_battlefield_height, 1);
    SendMessageA(g_duel_help_owner_hwnd, 0x412, 0, 0);
    MoveWindow(g_duel_player_battlefield_window_hwnd, s.prompt_x, s.bottom_half_y, s.player_battlefield_width, s.bottom_battlefield_height, 1);
    SendMessageA(g_duel_player_battlefield_window_hwnd, 0x412, 0, 0);
    s.point.x = s.right_column_x;
    s.point.y = s.chat_y;
    ClientToScreen(hwnd, &s.point);
    s.right_column_x = s.point.x;
    s.chat_y = s.point.y;
    MoveWindow(g_duel_prompt_context_hwnd, s.right_column_x, s.chat_y, s.right_column_width, s.chat_height, 1);
    s.chat_text1_length = SendMessageA(g_duel_prompt_context_hwnd, 0x402, (WPARAM)s.chat_text1, 0);
    set_duel_prompt_context(g_duel_prompt_context_hwnd, s.chat_text1, s.chat_text1_length);
    MoveWindow(DAT_008a8dec, s.face_top, s.face_y, s.divider_width, s.face_height, 1);
    MoveWindow(DAT_008a8d78, s.face_top, s.face_y, s.divider_width, s.face_height, 1);
    MoveWindow(g_duel_opponent_face_window_hwnd, s.opponent_life_x, s.opponent_life_y, s.left_phasebar_width + s.small_chat_height, s.top_face_height, 1);
    MoveWindow(g_duel_player_face_window_hwnd, s.player_life_x, s.local_80, s.left_phasebar_width + s.small_chat_height, s.lower_middle_height, 1);
    s.point.x = s.opponent_chat_x;
    s.point.y = s.opponent_chat_y;
    ClientToScreen(hwnd, &s.point);
    s.opponent_chat_x = s.point.x;
    s.opponent_chat_y = s.point.y;
    ShowWindow(global_opponent_chat_hwnd, SW_HIDE);
    MoveWindow(global_opponent_chat_hwnd, s.opponent_chat_x, s.opponent_chat_y, s.center_column_width, s.lower_phasebar_height, 1);
    s.point.x = s.right_chat_x;
    s.point.y = s.player_chat_y;
    ClientToScreen(hwnd, &s.point);
    s.right_chat_x = s.point.x;
    s.player_chat_y = s.point.y;
    ShowWindow(g_duel_player_chat_window_hwnd, SW_HIDE);
    MoveWindow(g_duel_player_chat_window_hwnd, s.right_chat_x, s.player_chat_y, s.center_column_width, s.lower_phasebar_height, 1);
  }
  else if (layout == 2)
  {
    GetWindowTextA((HWND)DAT_007a7d74, s.tooltip_text2, 100);
    set_duel_tooltip_text(s.tooltip_text2);
    GetClientRect(hwnd, &s.rect);
    s.client_width = s.rect.right;
    s.client_height = s.rect.bottom;
    s.preview_width = (s.client_width * 0x23) / 100;
    s.preview_height = (s.client_height * 0x3c) / 100;
    s.preview_left = s.rect.left;
    s.preview_top = s.rect.top;
    MoveWindow(g_duel_card_preview_window_hwnd, s.rect.left, s.rect.top, s.preview_width, s.preview_height, 0);
    GetWindowRect(g_duel_card_preview_window_hwnd, &s.rect);
    s.preview_width = s.rect.right - s.rect.left;
    s.preview_height = s.rect.bottom - s.rect.top;
    GetClientRect(hwnd, &s.rect);
    s.client_width = s.rect.right;
    s.client_height = s.rect.bottom;
    s.small_chat_height = s.client_width / 8;
    s.player_life_height = s.client_height / 8;
    s.top_middle_height = s.player_life_height;
    s.player_life_x = s.rect.left;
    s.opponent_life_x = s.player_life_x;
    s.opponent_life_y = s.client_height / 2 - s.top_middle_height;
    s.lower_life_y = s.client_height / 2;
    s.lower_phasebar_y = s.small_chat_height / 2;
    s.phasebar_width = s.small_chat_height - s.lower_phasebar_y;
    s.half_phasebar_height = s.client_height / 8;
    s.phasebar_height = s.half_phasebar_height;
    s.face_x = (s.small_chat_height + s.opponent_life_x) - s.phasebar_width;
    s.right_phasebar_x = s.face_x;
    s.local_f0 = s.opponent_life_x;
    s.local_ac = s.local_f0;
    s.opponent_phasebar_x = s.opponent_life_y - s.phasebar_height;
    s.lower_middle_y = s.opponent_phasebar_x;
    s.local_70 = s.player_life_height + s.lower_life_y;
    s.local_80 = s.local_70;
    s.left_phasebar_width = s.small_chat_height;
    s.battlefield_width = s.opponent_life_x;
    s.preview_x = s.battlefield_width;
    s.preview_y = s.rect.top;
    s.player_face_x = s.local_80 + s.phasebar_height;
    s.top_face_height = s.lower_middle_y - s.rect.top;
    s.lower_middle_height = s.rect.bottom - s.player_face_x;
    s.face_top = s.left_phasebar_width + s.preview_x;
    s.face_y = s.rect.top;
    s.divider_width = s.left_phasebar_width / 3;
    s.face_height = s.client_height;
    s.bottom_half_y = s.lower_life_y;
    s.right_column_y = s.rect.top;
    s.top_battlefield_height = (s.top_middle_height + s.opponent_life_y) - s.rect.top;
    s.bottom_battlefield_height = s.rect.bottom - s.bottom_half_y;
    s.right_battlefield_x = s.divider_width + s.face_top;
    s.prompt_x = s.right_battlefield_x;
    s.player_battlefield_width = (s.client_width - s.left_phasebar_width) - s.divider_width;
    s.right_column_width = s.player_battlefield_width;
    s.chat_height = (s.bottom_battlefield_height * 9) / 100;
    s.right_column_x = s.prompt_x;
    s.chat_y = s.bottom_half_y - s.chat_height / 2;
    s.center_column_width = s.player_battlefield_width;
    s.lower_phasebar_height = s.client_height / 0x28;
    s.opponent_chat_x = s.right_battlefield_x;
    s.opponent_chat_y = s.right_column_y;
    s.right_chat_x = s.prompt_x;
    s.player_chat_y = s.bottom_half_y - s.lower_phasebar_height;

    ShowWindow(g_duel_card_preview_window_hwnd, SW_HIDE);
    MoveWindow(g_duel_life_status_window_2_hwnd, s.opponent_life_x, s.opponent_life_y, s.small_chat_height, s.top_middle_height, 1);
    MoveWindow(g_duel_life_status_window_1_hwnd, s.player_life_x, s.lower_life_y, s.small_chat_height, s.player_life_height, 1);
    MoveWindow(unk_00939344, s.preview_x, s.preview_y, s.left_phasebar_width, s.top_face_height, 1);
    MoveWindow(unk_008ce534, s.battlefield_width, s.player_face_x, s.left_phasebar_width, s.lower_middle_height, 1);
    MoveWindow(DAT_0092680c, s.local_ac, s.lower_middle_y, s.lower_phasebar_y, s.phasebar_height, 1);
    MoveWindow(DAT_0091ce30, s.local_f0, s.local_80, s.lower_phasebar_y, s.phasebar_height, 1);
    MoveWindow(g_phasebar_your_untap_window_hwnd, s.right_phasebar_x, s.opponent_phasebar_x, s.phasebar_width, s.half_phasebar_height, 1);
    MoveWindow(g_duel_player_graveyard_window_hwnd, s.face_x, s.local_70, s.phasebar_width, s.half_phasebar_height, 1);
    MoveWindow(g_duel_card_preview_window_hwnd, s.preview_left, s.preview_top, s.preview_width, s.preview_height, 1);
    MoveWindow(g_duel_help_owner_hwnd, s.right_battlefield_x, s.right_column_y, s.player_battlefield_width, s.top_battlefield_height, 1);
    SendMessageA(g_duel_help_owner_hwnd, 0x412, 0, 0);
    MoveWindow(g_duel_player_battlefield_window_hwnd, s.prompt_x, s.bottom_half_y, s.player_battlefield_width, s.bottom_battlefield_height, 1);
    SendMessageA(g_duel_player_battlefield_window_hwnd, 0x412, 0, 0);
    s.point.x = s.right_column_x;
    s.point.y = s.chat_y;
    ClientToScreen(hwnd, &s.point);
    s.right_column_x = s.point.x;
    s.chat_y = s.point.y;
    MoveWindow(g_duel_prompt_context_hwnd, s.right_column_x, s.chat_y, s.right_column_width, s.chat_height, 1);
    s.chat_text2_length = SendMessageA(g_duel_prompt_context_hwnd, 0x402, (WPARAM)s.chat_text2, 0);
    set_duel_prompt_context(g_duel_prompt_context_hwnd, s.chat_text2, s.chat_text2_length);
    MoveWindow(DAT_008a8dec, s.face_top, s.face_y, s.divider_width, s.face_height, 1);
    MoveWindow(DAT_008a8d78, s.face_top, s.face_y, s.divider_width, s.face_height, 1);
    MoveWindow(g_duel_opponent_face_window_hwnd, s.opponent_life_x, s.opponent_life_y, s.small_chat_height, s.top_middle_height, 1);
    MoveWindow(g_duel_player_face_window_hwnd, s.player_life_x, s.lower_life_y, s.small_chat_height, s.player_life_height, 1);
    s.point.x = s.opponent_chat_x;
    s.point.y = s.opponent_chat_y;
    ClientToScreen(hwnd, &s.point);
    s.opponent_chat_x = s.point.x;
    s.opponent_chat_y = s.point.y;
    ShowWindow(global_opponent_chat_hwnd, SW_HIDE);
    MoveWindow(global_opponent_chat_hwnd, s.opponent_chat_x, s.opponent_chat_y, s.center_column_width, s.lower_phasebar_height, 1);
    s.point.x = s.right_chat_x;
    s.point.y = s.player_chat_y;
    ClientToScreen(hwnd, &s.point);
    s.right_chat_x = s.point.x;
    s.player_chat_y = s.point.y;
    ShowWindow(g_duel_player_chat_window_hwnd, SW_HIDE);
    MoveWindow(g_duel_player_chat_window_hwnd, s.right_chat_x, s.player_chat_y, s.center_column_width, s.lower_phasebar_height, 1);
  }

  resize_duel_hand_window(g_duel_full_card_window_hwnd);
  resize_duel_hand_window(g_duel_life_window_hwnd);
  resize_battlefield_child_card_windows(g_duel_player_battlefield_window_hwnd);
  resize_battlefield_child_card_windows(g_duel_help_owner_hwnd);
  layout_attack_phase_window(g_duel_attack_phase_window_hwnd);
  layout_phase_display_window(g_duel_phase_display_window_hwnd, (LPRECT)0);
  UpdateWindow(g_duel_card_preview_window_hwnd);
  UpdateWindow(hwnd);
}

// FUNCTION: MAGIC 0x004e8f49
// FUNCTION: SHANDALAR 0x00506d61
void refresh_duel_window(HWND hwnd)
{
  struct
  {
    int child_rect[2];
    LONG user_data_4;
    int child_count;
    int child_index;
    int y;
    int x;
    LONG user_data_0;
    HWND child_window;
    HWND child_windows[200];
    RECT client_rect;
  } s;

  s.user_data_4 = GetWindowLongA(hwnd, g_duel_window_userdata_card_offset);
  s.user_data_0 = GetWindowLongA(hwnd, g_duel_window_userdata_player_offset);
  GetClientRect(hwnd, &s.client_rect);
  reset_battlefield_layout_positions(hwnd);
  s.child_window = GetWindow(GetWindow(hwnd, 5), 1);
  s.child_count = 0;
  while (s.child_window != (HWND)0)
  {
    if (get_card_window_hidden_flag(s.child_window) == 0)
    {
      s.child_windows[s.child_count] = s.child_window;
      s.child_count++;
    }
    s.child_window = GetWindow(s.child_window, 3);
  }
  for (s.child_index = 0; s.child_index < s.child_count; s.child_index++)
  {
    s.child_window = s.child_windows[s.child_index];
    if (get_card_window_hidden_flag(s.child_window) == 0)
    {
      SendMessageA(s.child_window, 0x401, (WPARAM)s.child_rect, 0);
      get_next_battlefield_card_position(hwnd, s.child_rect, get_battlefield_card_stagger_offset(s.child_window), &s.x, &s.y, 1);
      SetWindowPos(s.child_window, (HWND)0, s.x, s.y, 0, 0, 5);
      BringWindowToTop(s.child_window);
    }
  }
  for (s.child_index = 0; s.child_index < s.child_count; s.child_index++)
  {
    s.child_window = s.child_windows[s.child_index];
    SendMessageA(hwnd, 0x410, (WPARAM)s.child_window, 0);
  }
  SendMessageA(hwnd, 0x412, 0, 0);
}

// FUNCTION: MAGIC 0x004ddbf4
// FUNCTION: SHANDALAR 0x005448a5
void run_duel_timer_tick(void)
{
}

// FUNCTION: MAGIC 0x0049d77b
// FUNCTION: SHANDALAR 0x00539128
void show_post_duel_draws(int duel_result)
{
}

// FUNCTION: MAGIC 0x0044a1ca
// FUNCTION: SHANDALAR 0x00453d4d
void get_current_duel_selection(int *selected_player, int *selected_card)
{
  EnterCriticalSection(&g_duel_render_lock);
  if (selected_player != NULL)
  {
    *selected_player = g_duel_selected_player_card;
  }
  if (selected_card != NULL)
  {
    *selected_card = g_duel_selected_opponent_card;
  }
  LeaveCriticalSection(&g_duel_render_lock);
}

// FUNCTION: MAGIC 0x0044a2bd
// FUNCTION: SHANDALAR 0x00453e40
int can_use_current_duel_selection(void)
{
  return 0;
}

// FUNCTION: MAGIC 0x00506240
int pick_internal_card_from_list_dialog(char *prompt, int initial_card_id, int filter)
{
  (void)prompt;
  (void)initial_card_id;
  (void)filter;
  return -1;
}

// FUNCTION: MAGIC 0x004649ef
void show_opponent_library_window(int unused_color)
{
  (void)unused_color;
}

// FUNCTION: MAGIC 0x00464a28
void show_player_library_window(int unused_color)
{
  (void)unused_color;
}

// FUNCTION: MAGIC 0x0048a8ee
void save_duel_interface_options_to_registry(void)
{
}

// FUNCTION: MAGIC 0x004e1b99
#ifndef SHANDALAR
void AddJournalEntry(int entry_type, int entry_arg)
{
}
#endif

// FUNCTION: MAGIC 0x004d91c9
// FUNCTION: SHANDALAR 0x00417946
void ReadCsvFieldByCsvid(char *out, int csvid, int field, const char *csv_name)
{
  struct
  {
    char in_quotes;
    char pad_01_to_03[3];
    int current_id;
    char delim[11];
    char pad_13;
    char token[512];
    char field_idx;
    char pad_215_to_217[3];
    FILE *fp;
    int fscanf_ret;
  } s;

  s.fp = fopen(csv_name, "rb");
  s.in_quotes = 0;
  s.field_idx = s.in_quotes;
  *out = '\0';

  if ((master_csv_offsets[csvid] != -1) && (strcmp(csv_name, "master.csv") == 0))
  {
    fseek(s.fp, (long)master_csv_offsets[csvid], 0);
  }

  do
  {
    s.fscanf_ret = fscanf(s.fp, "%[^,\n] %[,\n]", s.token, s.delim);
    if (s.fscanf_ret == 0)
    {
      break;
    }

    if (s.token[0] == '0')
    {
      s.current_id = atoi(s.token);
    }

    if (s.current_id == csvid)
    {
      ++s.field_idx;
      if ((s.field_idx == field) && s.in_quotes)
      {
        strcat(out, ",");
      }
      if (s.token[0] == '\"')
      {
        s.in_quotes = 1;
      }
      if (s.field_idx == field)
      {
        strcat(out, s.token);
      }
      if (s.token[strlen(s.token) - 1] == '\"')
      {
        s.in_quotes = 0;
      }
      if (s.in_quotes)
      {
        --s.field_idx;
      }
    }

  } while ((s.fscanf_ret != -1) && ((s.field_idx == '\0') || (s.current_id == csvid)));

  fclose(s.fp);
}

// FUNCTION: MAGIC 0x004b5de8
// FUNCTION: SHANDALAR 0x0056c5ea
int GetCardRarity(int param_1)
{
  struct
  {
    char rarity_str[32];
    int rarity;
  } s;

  if (((global_cards_data[param_1].extra_ability & 0x180U) != 0) || (global_cards_data[param_1].expansion == '@'))
  {
    global_cards_data[param_1].rarity = 4;
  }

  if ((signed char)global_cards_data[param_1].rarity != -1)
  {
    s.rarity = (int)(signed char)global_cards_data[param_1].rarity;

    return s.rarity;
  }

  ReadCsvFieldByCsvid(s.rarity_str, global_cards_data[param_1].id, 9, "info.csv");

  s.rarity = 1;

  if (strcmp(s.rarity_str, "Special") == 0)
  {
    s.rarity = 3;
  }

  if (strcmp(s.rarity_str, "Rare") == 0)
  {
    s.rarity = 3;
  }

  if (strcmp(s.rarity_str, "Uncommon") == 0)
  {
    s.rarity = 2;
  }

  global_cards_data[param_1].rarity = (unsigned char)s.rarity;

  return s.rarity;
}

// FUNCTION: MAGIC 0x004dd413
// FUNCTION: SHANDALAR 0x005440c4
int get_primary_color_from_duel_deck(int player)
{
  (void)player;
  return 0;
}

// FUNCTION: MAGIC 0x004dd719
void apply_duel_backdrop_art(int player, int color, int variant)
{
  struct
  {
    HWND hand_window;
    HWND graveyard_window;
    HWND life_window;
    HWND battlefield_window;
    char path[264];
    HBITMAP bitmap;
    char asset_name[52];
    BITMAP bitmap_info;
    RECT hand_rect;
  } s;

  if (color == 1)
  {
    strcpy(s.asset_name, "TERR_BLACK");
  }
  else if (color == 5)
  {
    strcpy(s.asset_name, "TERR_WHITE");
  }
  else if (color == 3)
  {
    strcpy(s.asset_name, "TERR_GREEN");
  }
  else if (color == 2)
  {
    strcpy(s.asset_name, "TERR_BLUE");
  }
  else
  {
    strcpy(s.asset_name, "TERR_RED");
  }
  if (variant == 0)
  {
    strcat(s.asset_name, "pict");
  }
  else if (variant == 1)
  {
    strcat(s.asset_name, "patt");
  }
  else
  {
    strcat(s.asset_name, "mana");
  }
  sprintf(s.path, "%s\\%s.pic", global_duelart_path, s.asset_name);
  s.bitmap = load_pic(s.path);
  if (player == 0)
  {
    s.battlefield_window = g_duel_player_battlefield_window_hwnd;
  }
  else
  {
    s.battlefield_window = g_duel_help_owner_hwnd;
  }
  SendMessageA(s.battlefield_window, 0x439, (WPARAM)s.bitmap, 0);

  if (color == 1)
  {
    strcpy(s.asset_name, "LIFE_BLACK");
  }
  else if (color == 5)
  {
    strcpy(s.asset_name, "LIFE_WHITE");
  }
  else if (color == 3)
  {
    strcpy(s.asset_name, "LIFE_GREEN");
  }
  else if (color == 2)
  {
    strcpy(s.asset_name, "LIFE_BLUE");
  }
  else
  {
    strcpy(s.asset_name, "LIFE_RED");
  }
  if (variant == 0)
  {
    strcat(s.asset_name, "pict");
  }
  else if (variant == 1)
  {
    strcat(s.asset_name, "patt");
  }
  else
  {
    strcat(s.asset_name, "mana");
  }
  sprintf(s.path, "%s\\%s.pic", global_duelart_path, s.asset_name);
  s.bitmap = load_pic(s.path);
  if (player == 0)
  {
    s.life_window = g_duel_life_status_window_1_hwnd;
  }
  else
  {
    s.life_window = g_duel_life_status_window_2_hwnd;
  }
  SendMessageA(s.life_window, 0x439, (WPARAM)s.bitmap, 0);

  if (color == 1)
  {
    strcpy(s.asset_name, "GRAVE_BLACK");
  }
  else if (color == 5)
  {
    strcpy(s.asset_name, "GRAVE_WHITE");
  }
  else if (color == 3)
  {
    strcpy(s.asset_name, "GRAVE_GREEN");
  }
  else if (color == 2)
  {
    strcpy(s.asset_name, "GRAVE_BLUE");
  }
  else
  {
    strcpy(s.asset_name, "GRAVE_RED");
  }
  sprintf(s.path, "%s\\%s.pic", global_duelart_path, s.asset_name);
  s.bitmap = load_pic(s.path);
  if (player == 0)
  {
    s.graveyard_window = g_duel_player_graveyard_window_hwnd;
  }
  else
  {
    s.graveyard_window = g_phasebar_your_untap_window_hwnd;
  }
  SendMessageA(s.graveyard_window, 0x439, (WPARAM)s.bitmap, 0);

  if (color == 1)
  {
    strcpy(s.asset_name, "HAND_BLACK");
  }
  else if (color == 5)
  {
    strcpy(s.asset_name, "HAND_WHITE");
  }
  else if (color == 3)
  {
    strcpy(s.asset_name, "HAND_GREEN");
  }
  else if (color == 2)
  {
    strcpy(s.asset_name, "HAND_BLUE");
  }
  else
  {
    strcpy(s.asset_name, "HAND_RED");
  }
  sprintf(s.path, "%s\\%s.pic", global_duelart_path, s.asset_name);
  s.bitmap = load_pic(s.path);
  GetObjectA(s.bitmap, sizeof(BITMAP), &s.bitmap_info);
  s.hand_rect.top = 0xb;
  s.hand_rect.left = s.bitmap_info.bmHeight - s.hand_rect.top;
  s.hand_rect.right = 7;
  s.hand_rect.bottom = 4;
  if (player == 0)
  {
    s.hand_window = g_duel_full_card_window_hwnd;
  }
  else
  {
    s.hand_window = g_duel_life_window_hwnd;
  }
  SendMessageA(s.hand_window, 0x439, (WPARAM)s.bitmap, (LPARAM)&s.hand_rect);
}

// FUNCTION: MAGIC 0x004ee26e
unsigned int load_gametype0(char *path)
{
  (void)path;
  return 0;
}

void dispatch_duel_engine_message(MSG *message);

// FUNCTION: MAGIC 0x004da411
// FUNCTION: SHANDALAR 0x005410d1
LRESULT CALLBACK wndproc_MAGICGAME_MainClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  struct
  {
    int minmaxinfo;
    char pad_0004[0x108];
    int command_player;
    int command_selected_card;
    int command_saved_phase_flags;
    int enemy_backdrop_result;
    int player_backdrop_color;
    int enemy_backdrop_color;
    int player_backdrop_result;

    int backdrop_player_color;
    int backdrop_enemy_color;
    int backdrop_player_variant;
    char backdrop_path[252];

    int slot_index;
    BOOL visible;
    int screen_height;
    int screen_width;
    int has_replay_prompt;
    char replay_prompt[100];
    int should_quit;
    HWND duel_result;
    int *posted_action_result;
    int selected_card;
    int selected_player;
    POINT cursor_pos;
    unsigned int hit_test;
    HWND cursor_window;
    int *action_request;
    MSG modal_msg;
    DWORD thread_exit_code;
    int *action_result;
    BOOL have_message;
    unsigned int action_result_ok;
    int done;
  } s;

  if (msg == 0x464)
  {
    notify_duel_action(0, (unsigned int)wparam);
    return 0;
  }

  switch (msg)
  {
  case 0x433:
  case 0x434:
    return 0;

  case 0x403:
    if (g_your_attack_window_hwnd != (HWND)0)
    {
      SendMessageA(g_your_attack_window_hwnd, WM_CLOSE, 0, 0);
    }
    KillTimer(hwnd, g_duel_timer_id);
    s.action_request = (int *)wparam;
    s.action_result = (int *)lparam;
    g_duel_modal_action_active = 1;
    memcpy(g_duel_action_request_copy, s.action_request, 0xe8);
    GetCursorPos(&s.cursor_pos);
    s.cursor_window = WindowFromPoint(s.cursor_pos);
    s.hit_test = SendMessageA(s.cursor_window, WM_NCHITTEST, 0,
                              ((s.cursor_pos.y << 0x10) & 0xffffU) | (s.cursor_pos.x & 0xffffU));
    SendMessageA(s.cursor_window, WM_SETCURSOR, (WPARAM)s.cursor_window, (s.hit_test & 0xffff) | 0x2000000);
    set_player_directive_value(1, s.action_request[0x38]);
    set_player_directive_value(0, s.action_request[0x39]);
    set_duel_prompt_context(g_duel_prompt_context_hwnd, (char *)(s.action_request + 6), s.action_request[5]);
    get_current_duel_selection(&s.selected_player, &s.selected_card);
    if (((s.selected_card == 0x15) && (s.selected_player == 1)) && (can_use_current_duel_selection() != 0))
    {
      g_duel_current_selection_forced = 1;
    }
    s.done = 0;
    while (s.done == 0)
    {
      GetExitCodeThread(g_duel_thread_handle, &s.thread_exit_code);
      if (s.thread_exit_code != STILL_ACTIVE)
      {
        s.done = 1;
      }
      s.have_message = PeekMessageA(&s.modal_msg, (HWND)0, 0, 0, PM_REMOVE);
      if (g_duel_main_window_closing != 0)
      {
        if ((s.have_message != 0) && (s.modal_msg.message == 0x464))
        {
          s.have_message = 0;
        }
        s.done = 1;
        s.action_result[0] = -5;
        s.action_result[1] = -1;
        s.action_result[2] = -1;
        if (s.action_result[0] == 0)
        {
          s.action_result_ok = 1;
        }
        else
        {
          s.action_result_ok = 0;
        }
      }
      if (s.have_message != 0)
      {
        if (s.modal_msg.message == 0x464)
        {
          s.posted_action_result = (int *)s.modal_msg.lParam;
          s.done = 1;
          s.action_result_ok = (unsigned int)(s.posted_action_result[0] == 0);
          memcpy(s.action_result, s.posted_action_result, 0x10);
        }
        else if (s.modal_msg.message == WM_QUIT)
        {
          PostQuitMessage(s.modal_msg.wParam);
          s.done = 1;
          s.action_result_ok = 0;
          s.action_result[0] = -2;
        }
        else
        {
          dispatch_duel_engine_message(&s.modal_msg);
        }
      }
    }
    set_duel_prompt_context(g_duel_prompt_context_hwnd, (char *)0, 0);
    set_player_directive_value(1, 0);
    set_player_directive_value(0, 0);
    g_duel_modal_action_active = 0;
    SetTimer(hwnd, g_duel_timer_id, 45000, (TIMERPROC)0);
    return s.action_result_ok;

  case 0x401:
    s.duel_result = (HWND)wparam;
    s.should_quit = 1;
    if (g_duel_thread_handle != (HANDLE)0)
    {
      WaitForSingleObject(g_duel_thread_handle, INFINITE);
      CloseHandle(g_duel_thread_handle);
      g_duel_thread_handle = (HANDLE)0;
    }
    if ((g_duel_mode_flags & 1) != 0)
    {
      if (g_duel_interface_options.see_next_draws_at_end_of_duel != 0)
      {
        show_post_duel_draws((int)s.duel_result);
      }
    }
    else if ((g_duel_mode_flags & 2) != 0)
    {
      s.has_replay_prompt = 1;
      if (s.duel_result == (HWND)1)
      {
        strcpy(s.replay_prompt, s_Congratulations_00579f28);
      }
      else if (s.duel_result == (HWND)0)
      {
        strcpy(s.replay_prompt, s_Too_bad_00579f3c);
      }
      else if (s.duel_result == (HWND)-1)
      {
        strcpy(s.replay_prompt, s_Oh_well_00579f44);
      }
      else
      {
        s.has_replay_prompt = 0;
      }
      if (s.has_replay_prompt != 0)
      {
        strcat(s.replay_prompt, s_Want_to_play_again_00579f50);
        if (MessageBoxA(hwnd, s.replay_prompt, s_End_of_duel_00579f68, MB_YESNO) == IDYES)
        {
          s.should_quit = 0;
          SendMessageA(hwnd, 0x400, 0, 0);
        }
      }
    }
    if (s.should_quit != 0)
    {
      g_duel_exit_code = (int)s.duel_result;
      PostQuitMessage((int)s.duel_result);
    }
    return 0;

  case 0x400:
    g_duel_main_window_closing = 0;
    if (g_duel_interface_options.layout == 2)
    {
      ShowWindow(g_duel_card_preview_window_hwnd, SW_HIDE);
    }
    else
    {
      SendMessageA(g_duel_card_preview_window_hwnd, 0x401, (WPARAM)-1, 0);
    }
    SendMessageA(g_duel_full_card_window_hwnd, 0x40c, 0, 0);
    SendMessageA(g_duel_life_window_hwnd, 0x40c, 0, 0);
    SendMessageA(g_duel_player_battlefield_window_hwnd, 0x40c, 0, 0);
    SendMessageA(g_duel_help_owner_hwnd, 0x40c, 0, 0);
    g_duel_selected_player_card = -1;
    g_duel_selected_opponent_card = -1;
    g_duel_selection_pending = 0;
    g_duel_current_selection_forced = -1;
    SendMessageA(DAT_008a8dec, 0x432, 0, 0);
    ShowWindow(DAT_008a8dec, SW_SHOW);
    ShowWindow(DAT_008a8d78, SW_HIDE);
    g_duel_cached_life_player_1 = 0;
    g_duel_cached_life_player_0 = g_duel_cached_life_player_1;
    g_duel_cached_state_007abce4 = 0;
    g_duel_cached_state_007abce0 = g_duel_cached_state_007abce4;
    g_duel_cached_unk_008b44d0_player_1 = 0;
    g_duel_cached_unk_008b44d0_player_0 = g_duel_cached_unk_008b44d0_player_1;
    SendMessageA(g_duel_life_status_window_1_hwnd, 0x432, 0, 0);
    SendMessageA(g_duel_life_status_window_2_hwnd, 0x432, 0, 0);
    for (s.screen_width = 0; s.screen_width < 7; s.screen_width = s.screen_width + 1)
    {
      g_duel_cached_raw_mana_player_1[s.screen_width] = 0;
      g_duel_cached_raw_mana_player_0[s.screen_width] = g_duel_cached_raw_mana_player_1[s.screen_width];
    }
    SendMessageA(unk_008ce534, 0x432, 0, 0);
    SendMessageA(unk_00939344, 0x432, 0, 0);
    g_duel_cached_library_count_player_1 = 1;
    g_duel_cached_library_count_player_0 = g_duel_cached_library_count_player_1;
    SendMessageA(DAT_0091ce30, 0x432, 0, 0);
    SendMessageA(DAT_0092680c, 0x432, 0, 0);
    g_duel_cached_graveyard_count_player_1 = 0;
    g_duel_cached_graveyard_count_player_0 = g_duel_cached_graveyard_count_player_1;
    g_duel_cached_exile_count_player_1 = 0;
    g_duel_cached_exile_count_player_0 = g_duel_cached_exile_count_player_1;
    SendMessageA(g_duel_player_graveyard_window_hwnd, 0x432, 0, 0);
    SendMessageA(g_phasebar_your_untap_window_hwnd, 0x432, 0, 0);
    SendMessageA(g_duel_attack_phase_window_hwnd, 0x40c, 0, 0);
    g_multiblock_creature_count = 0;
    global_stack_cards[0].player = -1;
    SendMessageA(g_duel_phase_display_window_hwnd, 0x40c, 0, 0);
    g_duel_modal_action_active = 0;
    ShowWindow(g_duel_opponent_face_window_hwnd, SW_HIDE);
    ShowWindow(g_duel_player_face_window_hwnd, SW_HIDE);
    UpdateWindow(hwnd);
    SetFocus(hwnd);
    if (g_duel_ai_mode_state == -10 && g_duel_run_mode > 0)
    {
    }
    else
    {
      if (g_duel_ai_mode_state == -10)
      {
        load_gametype0(g_duel_save_game_openfilename.lpstrFile);
      }
      else if ((g_duel_mode_flags & 4) != 0)
      {
        g_last_duel_enemy_primary_color = get_primary_color_from_duel_deck(DAT_0057a750);
        g_last_duel_player_primary_color = get_primary_color_from_duel_deck(g_selected_wizard_color);
      }
      else if ((g_duel_mode_flags & 1) != 0)
      {
        g_last_duel_enemy_primary_color = get_primary_color_from_duel_deck(DAT_0057a750);
        g_last_duel_player_primary_color = get_primary_color_from_duel_deck(g_selected_wizard_color);
      }
    }
    if ((g_duel_mode_flags & 0x10) != 0)
    {
      SendMessageA(g_duel_opponent_face_window_hwnd, 0x439, 0, 0);
      SendMessageA(g_duel_player_face_window_hwnd, 0x439, 0, 0);
    }
    else if ((g_duel_mode_flags & 1) != 0)
    {
      if (g_duel_creature_type != -1)
      {
        sprintf(s.backdrop_path, "%s\\%03d.pic", global_faces_path, g_duel_creature_type);
        s.screen_height = (int)load_pic(s.backdrop_path);
      }
      else
      {
        s.screen_height = 0;
      }
      SendMessageA(g_duel_opponent_face_window_hwnd, 0x439, (WPARAM)s.screen_height, 0);
      s.screen_height = (int)g_duel_player_face_pic;
      SendMessageA(g_duel_player_face_window_hwnd, 0x439, (WPARAM)s.screen_height, 1);
    }
    else
    {
      s.screen_height = (int)load_pic((char *)DAT_008ce570);
      SendMessageA(g_duel_opponent_face_window_hwnd, 0x439, (WPARAM)s.screen_height, 0);
      s.screen_height = (int)load_pic((char *)DAT_008b3fc0);
      SendMessageA(g_duel_player_face_window_hwnd, 0x439, (WPARAM)s.screen_height, 0);
    }
    if ((g_duel_mode_flags & 0x10) != 0)
    {
      s.backdrop_player_color = 1;
      s.backdrop_enemy_color = s.backdrop_player_color;
      s.backdrop_player_variant = 2;
      s.player_backdrop_result = s.backdrop_player_variant;
    }
    else if (g_duel_use_previous_backdrop_colors != 0)
    {
      s.backdrop_enemy_color = g_last_duel_enemy_primary_color;
      s.player_backdrop_result = g_last_duel_result_state;
      if (DAT_0091c99c == -1)
      {
        s.backdrop_player_color = g_last_duel_player_primary_color;
      }
      else
      {
        s.backdrop_player_color = DAT_0091c99c;
      }
      s.backdrop_player_variant = DAT_0091c9a0;
    }
    else
    {
      s.backdrop_enemy_color = g_last_duel_enemy_primary_color;
      s.player_backdrop_result = rand() % 3;
      if (DAT_0091c99c == -1)
      {
        s.backdrop_player_color = g_last_duel_player_primary_color;
      }
      else
      {
        s.backdrop_player_color = DAT_0091c99c;
      }
      s.backdrop_player_variant = DAT_0091c9a0;
      if ((s.backdrop_player_color == s.backdrop_enemy_color) &&
          (s.backdrop_player_variant == s.player_backdrop_result))
      {
        s.player_backdrop_result = (s.player_backdrop_result + 1) % 3;
      }
      g_last_duel_result_state = s.player_backdrop_result;
    }
    apply_duel_backdrop_art(1, s.backdrop_enemy_color, s.player_backdrop_result);
    apply_duel_backdrop_art(0, s.backdrop_player_color, s.backdrop_player_variant);
    if (IsWindowVisible(hwnd) == 0)
    {
      ShowWindow(hwnd, SW_SHOW);
      SetForegroundWindow(hwnd);
      UpdateWindow(hwnd);
      ShowWindow(g_duel_full_card_window_hwnd, SW_SHOW);
      ShowWindow(g_duel_life_window_hwnd, SW_SHOW);
    }
    if ((g_duel_mode_flags & 0x10) != 0)
    {
      g_duel_thread_handle = (HANDLE)0;
      PostMessageA(hwnd, 0x401, 0, 0);
    }
    else
    {
      g_duel_thread_handle = CreateThread((LPSECURITY_ATTRIBUTES)0,
                                          0,
                                          start_duel_thread,
                                          (LPVOID)0,
                                          0,
                                          (LPDWORD)&s.enemy_backdrop_color);
      SetThreadPriority(g_duel_thread_handle, THREAD_PRIORITY_NORMAL);
    }
    return 0;

  case WM_CLOSE:
    life[0] = 0;
    g_duel_main_window_closing = 1;
    return 0;

  case WM_COMMAND:
    switch ((unsigned int)wparam & 0xffff)
    {
    case 0x279:
      g_duel_interface_options.show_id_tags_on_cards =
          (g_duel_interface_options.show_id_tags_on_cards == 0);
      save_duel_interface_options_to_registry();
      SendMessageA(g_duel_player_battlefield_window_hwnd, 0x435, 0, 0);
      SendMessageA(g_duel_help_owner_hwnd, 0x435, 0, 0);
      SendMessageA(g_duel_attack_phase_window_hwnd, 0x435, 0, 0);
      SendMessageA(g_duel_phase_display_window_hwnd, 0x435, 0, 0);
      break;

    case 0x27a:
      g_duel_interface_options.show_invisible_effect_cards =
          (g_duel_interface_options.show_invisible_effect_cards == 0);
      save_duel_interface_options_to_registry();
      notify_duel_action(0, 0xff);
      break;

    case 0x27c:
      g_duel_interface_options.show_all_cards_summon_sickness =
          (g_duel_interface_options.show_all_cards_summon_sickness == 0);
      save_duel_interface_options_to_registry();
      SendMessageA(g_duel_player_battlefield_window_hwnd, 0x435, 0, 0);
      SendMessageA(g_duel_help_owner_hwnd, 0x435, 0, 0);
      SendMessageA(g_duel_attack_phase_window_hwnd, 0x435, 0, 0);
      SendMessageA(g_duel_phase_display_window_hwnd, 0x435, 0, 0);
      break;

    case 0x27b:
      if ((g_duel_network_flags & 2) == 0)
      {
        g_duel_save_game_openfilename.hwndOwner = g_duel_window_hwnd;
        g_duel_save_game_openfilename.lpstrTitle = gs_window_title_save_game_008951d0;
        g_duel_save_game_openfilename.Flags = 0x2a000c;
        if (GetSaveFileNameA(&g_duel_save_game_openfilename) != 0)
        {
          copy_autosave_to_save_file(g_duel_save_game_openfilename.lpstrFile);
        }
      }
      break;

    case 599:
      if (g_duel_startup_state != 0)
      {
        g_duel_startup_state = 0;
      }
      else
      {
        g_duel_startup_state = 1;
      }
      if (DAT_008ce504 == 0)
      {
        g_duel_startup_state = 0;
      }
      break;

    case 0x25c:
      if (g_duel_startup_state != 0)
      {
        g_duel_surface_reset_state = (g_duel_surface_reset_state == 0);
        resize_duel_hand_window(g_duel_life_window_hwnd);
      }
      break;

    case 0x25d:
      if (g_duel_startup_state != 0)
      {
        show_opponent_library_window(DAT_0057a750);
      }
      break;

    case 0x25e:
      if (g_duel_startup_state != 0)
      {
        show_player_library_window(DAT_0057a750);
      }
      break;

    case 0x26b:
    case 0x26c:
      if (g_duel_startup_state != 0)
      {
        if (((unsigned int)wparam & 0xffff) == 0x26b)
        {
          s.player_backdrop_color = 0;
        }
        else
        {
          s.player_backdrop_color = 1;
        }
        s.enemy_backdrop_result = pick_internal_card_from_list_dialog("Pick a card to put into play", -1, -1);
        s.command_selected_card = (int)(char)g_duel_phase_stop_settings[human_player].phase_flags[current_phase];
        g_duel_phase_stop_settings[human_player].phase_flags[current_phase] =
            (unsigned char)((int)(char)g_duel_phase_stop_settings[human_player].phase_flags[current_phase] & ~PHASE_STOP_ENABLED);
        s.command_saved_phase_flags = add_card_to_hand(s.player_backdrop_color, s.enemy_backdrop_result);
        if (s.command_saved_phase_flags != -1)
        {
          process_card_enters_play(s.player_backdrop_color, s.command_saved_phase_flags);
        }
        g_duel_phase_stop_settings[human_player].phase_flags[current_phase] = (unsigned char)s.command_selected_card;
        notify_duel_action(0, 0xff);
      }
      break;

    case 0x26d:
    case 0x26e:
      if (g_duel_startup_state != 0)
      {
        if (((unsigned int)wparam & 0xffff) == 0x26d)
        {
          s.player_backdrop_color = 0;
        }
        else
        {
          s.player_backdrop_color = 1;
        }
        s.enemy_backdrop_result = pick_internal_card_from_list_dialog("Pick a card to put into hand", -1, -1);
        s.command_saved_phase_flags = add_card_to_hand(s.player_backdrop_color, s.enemy_backdrop_result);
        hand_count[s.player_backdrop_color] += 1;
        if (((g_duel_network_flags & 2) != 0) && (nonactive_player == s.player_backdrop_color))
        {
          g_network_result_packet_type = 0x12;
          g_network_result_value = s.enemy_backdrop_result;
          TENTATIVE_send_network_result(nonactive_player, 0x12);
        }
        notify_duel_action(0, 0xff);
      }
      break;

    case 0x269:
    case 0x26a:
      if (g_duel_startup_state != 0)
      {
        if (((unsigned int)wparam & 0xffff) == 0x269)
        {
          s.player_backdrop_color = 0;
        }
        else
        {
          s.player_backdrop_color = 1;
        }
        draw_card_for_player(s.player_backdrop_color);
        notify_duel_action(0, 0xff);
      }
      break;

    case 0x26f:
    case 0x270:
      if (g_duel_startup_state != 0)
      {
        if (((unsigned int)wparam & 0xffff) == 0x26f)
        {
          s.player_backdrop_color = 0;
        }
        else
        {
          s.player_backdrop_color = 1;
        }
        life[s.player_backdrop_color] = prompt_for_life_total(0,
                                                     s.player_backdrop_color == 0 ? "Set player lives to:" : "Set opponent lives tp:",
                                                     life[s.player_backdrop_color]);
        notify_duel_action(0, 0xff);
      }
      break;

    case 0x267:
    case 0x268:
      if (g_duel_startup_state != 0)
      {
        if (((unsigned int)wparam & 0xffff) == 0x267)
        {
          s.player_backdrop_color = 1;
        }
        else
        {
          s.player_backdrop_color = 0;
        }
        life[s.player_backdrop_color] = 0;
        SendMessageA(g_duel_life_status_window_1_hwnd, 0x432, 0, 0);
        SendMessageA(g_duel_life_status_window_2_hwnd, 0x432, 0, 0);
      }
      break;

    case 0x263:
      if (g_duel_startup_state != 0)
      {
        life[0] = 0;
        life[1] = 0;
        SendMessageA(g_duel_life_status_window_1_hwnd, 0x432, 0, 0);
        SendMessageA(g_duel_life_status_window_2_hwnd, 0x432, 0, 0);
      }
      break;

    case 0x271:
      if (g_duel_startup_state != 0)
      {
        ShowWindow(g_duel_palette_window_hwnd, SW_SHOW);
      }
      break;

    case 0x275:
      break;

    case 0x276:
      if (g_duel_startup_state != 0)
      {
        if (g_duel_palette_refresh_pending == 0)
        {
          g_duel_palette_refresh_pending = 1;
        }
        else
        {
          g_duel_palette_refresh_pending = 0;
        }
      }
      break;

    case 0x277:
      if (g_duel_startup_state != 0)
      {
        ShowWindow(g_duel_toggleable_status_window_hwnd,
                   -((IsWindowVisible(g_duel_toggleable_status_window_hwnd) == 0)) & SW_SHOW);
      }
      break;

    case 0x273:
      if (g_duel_startup_state != 0)
      {
        g_shared_startup_completed = (g_shared_startup_completed == 0);
        SendMessageA(g_duel_player_battlefield_window_hwnd, 0x435, 0, 0);
        SendMessageA(g_duel_help_owner_hwnd, 0x435, 0, 0);
        SendMessageA(g_duel_full_card_window_hwnd, 0x435, 0, 0);
        SendMessageA(g_duel_life_window_hwnd, 0x435, 0, 0);
        InvalidateRect(g_duel_card_preview_window_hwnd, (RECT *)0, 0);
      }
      break;

    case 0x274:
      if (g_duel_startup_state != 0)
      {
        g_duel_network_state = 0;
      }
      break;

    case 0x272:
      if (g_duel_startup_state != 0)
      {
        g_duel_dialog_refresh_state = (g_duel_dialog_refresh_state == 0);
        SendMessageA(g_duel_player_battlefield_window_hwnd, 0x435, 0, 0);
        SendMessageA(g_duel_help_owner_hwnd, 0x435, 0, 0);
        SendMessageA(g_duel_full_card_window_hwnd, 0x435, 0, 0);
        SendMessageA(g_duel_life_window_hwnd, 0x435, 0, 0);
        InvalidateRect(DAT_0091ce30, (RECT *)0, 1);
        InvalidateRect(DAT_0092680c, (RECT *)0, 1);
      }
      break;
    }
    return 0;

  case WM_CREATE:
    g_duel_thread_handle = (HANDLE)0;
    g_duel_selected_player_card = -1;
    g_duel_selected_opponent_card = -1;
    g_duel_selection_pending = 0;
    g_duel_cached_life_player_1 = 0;
    g_duel_cached_life_player_0 = g_duel_cached_life_player_1;
    g_duel_cached_state_007abce4 = 0;
    g_duel_cached_state_007abce0 = g_duel_cached_state_007abce4;
    g_duel_cached_unk_008b44d0_player_1 = 0;
    g_duel_cached_unk_008b44d0_player_0 = g_duel_cached_unk_008b44d0_player_1;
    for (s.command_player = 0; s.command_player < 7; s.command_player = s.command_player + 1)
    {
      g_duel_cached_raw_mana_player_1[s.command_player] = 0;
      g_duel_cached_raw_mana_player_0[s.command_player] = g_duel_cached_raw_mana_player_1[s.command_player];
    }
    g_duel_cached_library_count_player_1 = 1;
    g_duel_cached_library_count_player_0 = g_duel_cached_library_count_player_1;
    g_duel_cached_graveyard_count_player_1 = 0;
    g_duel_cached_graveyard_count_player_0 = g_duel_cached_graveyard_count_player_1;
    g_duel_cached_exile_count_player_1 = 0;
    g_duel_cached_exile_count_player_0 = g_duel_cached_exile_count_player_1;
    g_multiblock_creature_count = 0;
    g_duel_modal_action_active = 0;
    if (create_duel_child_windows(hwnd) == 0)
    {
      return -1;
    }
    g_duel_palette_window_hwnd = CreateWindowExA(0, "MAGIC_PaletteClass", "Palette", 0x80cc0000,
                                                 0x14, 0x14, 300, 0x15e, hwnd, (HMENU)0,
                                                 g_app_instance, (LPVOID)0);
    g_your_attack_window_hwnd = (HWND)0;
    g_duel_timer_id = 0x14;
    return 0;

  case WM_DESTROY:
    strcpy(s.pad_0004, global_base_directory);
    strcat(s.pad_0004, "\\duel.hlp");
    WinHelpA(g_duel_window_hwnd, s.pad_0004, HELP_QUIT, 0);
    KillTimer(hwnd, g_duel_timer_id);
    return 0;

  case WM_DISPLAYCHANGE:
    g_display_color_depth = (int)wparam;
    EnterCriticalSection(&g_card_render_lock);
    checked_DeleteDC_DeleteObject(g_shared_offscreen_dc, g_shared_offscreen_bitmap);
    if (CreateOffscreen32bppDibSection(GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN),
                                       &g_shared_offscreen_dc, &g_duel_backbuffer_bmi,
                                       &g_shared_offscreen_bitmap, &g_shared_offscreen_prev_object,
                                       &g_shared_offscreen_bits) == 0)
    {
      MessageBoxA(hwnd, DAT_007898b0, gs_magic_the_gathering_title_00789460, MB_ICONEXCLAMATION);
      ShowWindow(hwnd, SW_HIDE);
    }
    else
    {
      ShowWindow(hwnd, SW_SHOW);
    }
    LeaveCriticalSection(&g_card_render_lock);
    if (IsIconic(hwnd) == 0)
    {
      MoveWindow(hwnd, 1, 0, ((unsigned int)lparam & 0xffff) - 1, HIWORD(lparam), 1);
    }
    else
    {
      g_duel_main_window_deferred_resize = 1;
    }
    return 0;

  case WM_ERASEBKGND:
    return 1;

  case 0x501:
    g_duel_main_window_deferred_resize = 0;
    MoveWindow(hwnd, 1, 0, GetSystemMetrics(SM_CXSCREEN) - 1, GetSystemMetrics(SM_CYSCREEN), 1);
    return 0;

  case WM_QUERYOPEN:
    if (g_duel_main_window_deferred_resize != 0)
    {
      PostMessageA(hwnd, 0x501, 0, 0);
    }

    if (DUEL_SHELL_WINDOW_HWND != (HWND)0)
    {
      ShowWindow(DUEL_SHELL_WINDOW_HWND, SW_SHOW);
    }

    return 1;

  case WM_GETMINMAXINFO:
    s.minmaxinfo = (int)lparam;
    ((int *)s.minmaxinfo)[6] = 0x32;
    ((int *)s.minmaxinfo)[7] = 0x32;
    return 0;

  case WM_SIZE:
    if ((wparam == SIZE_RESTORED) && (g_duel_main_window_minimized == 0))
    {
      LockWindowUpdate(hwnd);
      layout_duel_child_windows(hwnd, g_duel_interface_options.layout);
      refresh_duel_window(g_duel_player_battlefield_window_hwnd);
      refresh_duel_window(g_duel_help_owner_hwnd);
      LockWindowUpdate((HWND)0);
    }

    if (wparam == SIZE_MINIMIZED)
    {
      g_duel_main_window_minimized = 1;
    }
    else if (wparam == SIZE_RESTORED)
    {
      g_duel_main_window_minimized = 0;
    }

    if ((wparam == SIZE_MINIMIZED) && (DUEL_SHELL_WINDOW_HWND != (HWND)0))
    {
      ShowWindow(DUEL_SHELL_WINDOW_HWND, SW_SHOW);
    }
    return 0;

  case 0x30f:
  case 0x310:
  case 0x311:
    return FUN_10025b5e((int)hwnd, msg, (int)wparam, lparam);

  case WM_TIMER:
    if ((int)wparam == g_duel_timer_id)
    {
      if (g_duel_modal_action_active == 0)
      {
        if (g_your_attack_window_hwnd != (HWND)0)
        {
        }
        else
        {
          run_duel_timer_tick();
        }
      }
    }
    return 0;

  default:
    return DefWindowProcA(hwnd, msg, wparam, lparam);
  }

  return 0;
}
