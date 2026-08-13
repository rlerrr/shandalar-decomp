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
#include "drawcardlib/Drawcardlib.h"
#include "drawcardlib/src/pic.h"
#ifdef SHANDALAR
#include "deckdll/src/magsnd.h"
#include "shandalar/src/shandalar_global_strings.h"
#endif

#ifndef SHANDALAR
extern HWND global_main_hwnd;
#define DUEL_MAIN_WINDOW_HWND global_main_hwnd
#else
#define DUEL_MAIN_WINDOW_HWND g_main_window_hwnd
#endif
#define DUEL_SHELL_WINDOW_HWND g_main_window_hwnd

extern int life[2];
extern HINSTANCE g_app_instance;
extern HWND DAT_008a8dec;
extern HWND DAT_008a8d78;
extern int DAT_007a7d74;
extern int g_showlist_smallcard_width;
extern int g_showlist_smallcard_height;
extern int global_available_slots;
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
void copy_autosave_to_save_file(LPCSTR save_path);
DWORD WINAPI start_duel_thread(LPVOID unused);
void StopWorldLocationMusic(void);
void reset_duel_globals(void);
void reset_timestamp_players(void);
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
void TENTATIVE_reassess_all_cards(int view_player, int present_after_draw);
void append_to_trace_txt(char *text);
void AddCardToCLPacket(int card_in_packet);
int GetCardFromCLPacket(int packet_index);
unsigned int refresh_duel_display_cache(void);
unsigned int get_displayed_card_special_counters(int player, int card);
int get_displayed_card_internal_id(int player, int card);
card_id_t get_displayed_card_id(int player, int card);
int get_displayed_card_zone(int player, int card);
int get_displayed_card_type(int player, int card);
unsigned int get_displayed_card_ui_flags(int player, int card);
void get_displayed_card_attachment(int *player_and_card, int player, int card);
unsigned int get_displayed_card_display_flags(int player, int card);
int is_attack_phase_window_enabled(void);
int find_attack_phase_card_window(HWND hwnd, int *player_and_card, int *unused1, HWND *child_hwnd, int *unused2);
int find_battlefield_card_window(HWND hwnd, int *player_and_card, int *unused, HWND *child_hwnd);
int card_window_matches_player_and_card(HWND hwnd, int *player_and_card);
card_id_t get_card_window_displayed_card_id(HWND hwnd);
int count_hidden_battlefield_descendants(HWND hwnd, HWND hidden_parent);
int count_hidden_battlefield_descendants_for_card(HWND hwnd, int *player_and_card);
int displayed_card_indices_invalid(int player, int card);
extern int g_manalink_is_host;
#ifndef SHANDALAR
extern HANDLE global_mutex_GameInit;
#endif
#ifdef SHANDALAR
extern int g_world_location_music_active;
extern int random_seed_initialized;
extern int _DAT_007483f4;
// GLOBAL: SHANDALAR 0x0058e048
// GLOBAL: SHANDALAR 0x0058e04c
int g_showlibrary_menu_selection = 0;
int single_color_test_bit_to_color_t(int color_mask);
void AddJournalEntry(int entry_type, int entry_arg);
int SelectAdventureListCardIndex(int player, int *card_ids, int card_count, char *title, int require_card_click, int *out_selection);
#endif
int GetCardRarity(int card_id);
int IsCardAvailable(csvid_t csvid, int expansion);
int copy_cached_library_cards_and_get_count(void *cards, int player);
void copy_opponent_name_prefix(char *name);
void delete_and_close_object(HANDLE obj);
void change_buttonclass_wndproc(HWND hwnd);
void draw_owner_draw_button_centered(DRAWITEMSTRUCT *draw_item, HBRUSH brush, HPEN pen1, HPEN pen2, COLORREF color, int draw_focus);
BOOL CALLBACK post_duel_draws_dialog_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
BOOL CALLBACK still_thinking_dialog_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

#define PICK_CARD_FULLCARD_ID 1030
#define PICK_CARD_LISTBOX_ID 1031
#define PICK_CARD_COLOR_BLACK_ID 1032
#define PICK_CARD_COLOR_BLUE_ID 1033
#define PICK_CARD_COLOR_GREEN_ID 1034
#define PICK_CARD_COLOR_RED_ID 1035
#define PICK_CARD_COLOR_WHITE_ID 1036
#define PICK_CARD_COLOR_GOLD_ID 1864
#define PICK_CARD_TYPE_CREATURE_ID 1037
#define PICK_CARD_TYPE_ARTIFACT_ID 1038
#define PICK_CARD_TYPE_ENCHANTMENT_ID 1039
#define PICK_CARD_TYPE_SORCERY_ID 1040
#define PICK_CARD_TYPE_INSTANT_ID 1041
#define PICK_CARD_TYPE_LAND_ID 1042

#define PICK_CARD_COLOR_BLACK 0x2
#define PICK_CARD_COLOR_BLUE 0x4
#define PICK_CARD_COLOR_GREEN 0x8
#define PICK_CARD_COLOR_RED 0x10
#define PICK_CARD_COLOR_WHITE 0x20
#define PICK_CARD_COLOR_GOLD 0x400

#define PICK_CARD_TYPE_LAND 0x1
#define PICK_CARD_TYPE_CREATURE 0x2
#define PICK_CARD_TYPE_ENCHANTMENT 0x4
#define PICK_CARD_TYPE_SORCERY 0x8
#define PICK_CARD_TYPE_INSTANT 0x10
#define PICK_CARD_TYPE_INTERRUPT 0x20
#define PICK_CARD_TYPE_ARTIFACT 0x40

typedef struct
{
  unsigned int color_filter;
  unsigned int type_filter;
  char *prompt;
} PickCardDialogContext;

// GLOBAL: MAGIC 0x0057d8a8
// GLOBAL: SHANDALAR 0x00580d70
PickCardDialogContext g_pick_card_dialog_context = {PICK_CARD_COLOR_BLACK, PICK_CARD_TYPE_CREATURE, NULL};

// GLOBAL: MAGIC 0x006f6df4
PickCardDialogContext *g_pick_card_dialog_context_ptr;

// GLOBAL: MAGIC 0x008a9140
// GLOBAL: SHANDALAR 0x008bd340
OPENFILENAMEA g_duel_save_game_openfilename;

// GLOBAL: MAGIC 0x00746368
void *g_duel_player_face_pic;

// GLOBAL: MAGIC 0x0091c9a4
// GLOBAL: SHANDALAR 0x00930ae4
HANDLE g_duel_thread_handle;

// GLOBAL: MAGIC 0x00579f24
// GLOBAL: SHANDALAR 0x0058f654
int g_duel_main_window_minimized = 0;

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
// GLOBAL: SHANDALAR 0x00951be0
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
target_selection_request_t g_duel_action_request_copy;

// GLOBAL: MAGIC 0x008961b0
// GLOBAL: SHANDALAR 0x008aa3b0
int g_duel_current_selection_forced;

// GLOBAL: MAGIC 0x0074a230
// GLOBAL: SHANDALAR 0x00746b08
int g_duel_main_window_closing;

// GLOBAL: MAGIC 0x00579f20
// GLOBAL: SHANDALAR 0x0058f650
int g_duel_main_window_deferred_resize = 0;

// GLOBAL: MAGIC 0x0055e168
// GLOBAL: SHANDALAR 0x0057f0f8
int g_duel_window_userdata_player_offset = 0;

// GLOBAL: MAGIC 0x0055e16c
// GLOBAL: SHANDALAR 0x0057f0fc
int g_duel_window_userdata_card_offset = 4;

// GLOBAL: MAGIC 0x0055e170
// GLOBAL: SHANDALAR 0x0057f100
int g_duel_window_userdata_8_offset = 8;

// GLOBAL: MAGIC 0x0055e174
// GLOBAL: SHANDALAR 0x0057f104
int g_duel_window_userdata_snapshot_offset = 0xc;

// GLOBAL: MAGIC 0x00939508
// GLOBAL: SHANDALAR 0x0094d638
int DAT_00939508;

// GLOBAL: MAGIC 0x00789710
// GLOBAL: SHANDALAR 0x007a04a0
int DAT_00789710;

// GLOBAL: MAGIC 0x0057ab6c
// GLOBAL: SHANDALAR 0x0058c37c
int DAT_0057ab6c = 0;

// GLOBAL: MAGIC 0x008b2878
// GLOBAL: SHANDALAR 0x008c6a28
int g_battlefield_land_x[2];

// GLOBAL: MAGIC 0x007ab2b0
// GLOBAL: SHANDALAR 0x007bf4b0
int g_battlefield_creature_y[2];

// GLOBAL: MAGIC 0x008cff18
// GLOBAL: SHANDALAR 0x008e4064
int g_battlefield_creature_x_spacing;

// GLOBAL: MAGIC 0x007aaee8
// GLOBAL: SHANDALAR 0x007bf0e8
int g_battlefield_creature_y_margin;

// GLOBAL: MAGIC 0x008b3498
// GLOBAL: SHANDALAR 0x008c7638
int g_battlefield_noncreature_y[2];

// GLOBAL: MAGIC 0x008cfd28
// GLOBAL: SHANDALAR 0x008e3e78
int g_battlefield_draw_placeholder_x[2];

// GLOBAL: MAGIC 0x0093a800
// GLOBAL: SHANDALAR 0x0094e920
int g_battlefield_draw_placeholder_y[2];

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
    card_id_t card_id;
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
              ((damage_card_internal_card_id == s.internal_card_id) && (get_displayed_card_special_counters(s.player_index, s.card_index) == 0)))
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
          else if (stack_proxy_internal_card_id == s.internal_card_id)
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
                    ((global_raw_cards_storage[s.card_id].card_type == 2 &&
                      (global_raw_cards_storage[s.card_id].subtype != 0xd3))))))
              {
                get_displayed_card_attachment(s.attached_player_and_card, s.player_index, s.card_index);
                while (((get_displayed_card_ui_flags(s.attached_player_and_card[0], s.attached_player_and_card[1]) & 0x10) != 0 &&
                        ((s.card_id = get_displayed_card_id(s.attached_player_and_card[0], s.attached_player_and_card[1])) != -1)) &&
                       (unk_007a7d64 <= s.card_id ||
                        ((global_raw_cards_storage[s.card_id].card_type == 2 &&
                          (global_raw_cards_storage[s.card_id].subtype != 0xd3)))))
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
// FUNCTION: SHANDALAR 0x00452442
int get_displayed_card_zone(int player, int card)
{
  int result;

  if (displayed_card_indices_invalid(player, card) != 0)
    return 0;

  EnterCriticalSection(&g_duel_render_lock);
  if ((global_displayed_card_instances[player][card].state & STATE_IN_PLAY) != 0)
  {
    result = 1;
  }
  else
  {
    if ((global_displayed_card_instances[player][card].state & STATE_INVISIBLE) != 0)
    {
      result = 2;
    }
    else
    {
      result = 0;
    }
  }
  LeaveCriticalSection(&g_duel_render_lock);

  return result;
}

// FUNCTION: MAGIC 0x00448999
// FUNCTION: SHANDALAR 0x00452516
unsigned int get_displayed_card_ui_flags(int player, int card)
{
  unsigned int result;
  int card_type;

  result = 0;
  if (displayed_card_indices_invalid(player, card) != 0)
  {
    return 0;
  }

  EnterCriticalSection(&g_duel_render_lock);
  card_type = global_cards_data[global_displayed_card_instances[player][card].internal_card_id].type;
  if ((global_displayed_card_instances[player][card].state & STATE_SUMMONSICK_BOTH) != 0)
  {
    result |= 1;
  }
  if (((global_displayed_card_instances[player][card].state & STATE_TAPPED) != 0) &&
      ((card_type & 0x47) != TYPE_ENCHANTMENT))
  {
    result |= 2;
  }
  if ((global_displayed_card_instances[player][card].state & STATE_ATTACKING) != 0)
  {
    result |= 4;
  }
  if (global_displayed_card_instances[player][card].blocking != -1)
  {
    result |= 8;
  }
  if ((global_displayed_card_instances[player][card].damage_target_player != -1) &&
      (global_displayed_card_instances[player][card].damage_target_card != -1))
  {
    result |= 0x10;
  }
  if ((global_displayed_card_instances[player][card].state & STATE_OUBLIETTED) != 0)
  {
    result |= 0x20;
  }
  LeaveCriticalSection(&g_duel_render_lock);
  return result;
}

// FUNCTION: MAGIC 0x00448bc6
// FUNCTION: SHANDALAR 0x00452743
void get_displayed_card_attachment(int *player_and_card, int player, int card)
{
  if (player_and_card == (int *)0)
  {
    return;
  }

  if (displayed_card_indices_invalid(player, card) != 0)
  {
    player_and_card[0] = -1;
    player_and_card[1] = -1;
  }
  else
  {
    EnterCriticalSection(&g_duel_render_lock);
    player_and_card[0] = global_displayed_card_instances[player][card].damage_target_player;
    player_and_card[1] = global_displayed_card_instances[player][card].damage_target_card;
    LeaveCriticalSection(&g_duel_render_lock);
    return;
  }
}

// FUNCTION: MAGIC 0x0044a342
// FUNCTION: SHANDALAR 0x00453ec5
int is_attack_phase_window_enabled(void)
{
  int result;

  EnterCriticalSection(&g_duel_render_lock);
  result = g_duel_selection_pending;
  LeaveCriticalSection(&g_duel_render_lock);
  return result;
}

// FUNCTION: MAGIC 0x004e994f
// FUNCTION: SHANDALAR 0x0050775c
int find_battlefield_card_window(HWND hwnd, int *player_and_card, int *unused, HWND *child_hwnd)
{
  HWND *card_windows;
  int card_count;
  int found;
  int index;
  card_id_t found_card_id;
  HWND found_hwnd;

  if (hwnd == (HWND)0 || player_and_card == (int *)0)
  {
    found = 0;
  }
  else
  {
    card_windows = (HWND *)GetWindowLongA(hwnd, g_duel_window_userdata_player_offset);
    card_count = GetWindowLongA(hwnd, g_duel_window_userdata_card_offset);
    found = 0;
    index = 0;
    while (index < card_count && found == 0)
    {
      if (card_window_matches_player_and_card(card_windows[index], player_and_card) != 0)
      {
        found = 1;
        found_card_id = get_card_window_displayed_card_id(card_windows[index]);
        found_hwnd = card_windows[index];
      }
      index++;
    }
  }

  if (unused != (int *)0)
  {
    if (found == 0)
      *unused = -1;
    else
      *unused = found_card_id;
  }
  if (child_hwnd != (HWND *)0)
  {
    if (found == 0)
      *child_hwnd = (HWND)0;
    else
      *child_hwnd = found_hwnd;
  }
  return found;
}

// FUNCTION: MAGIC 0x004d25fe
// FUNCTION: SHANDALAR 0x00578bda
LONG get_card_window_hidden_flag(HWND hwnd)
{
  return GetWindowLongA(hwnd, 8);
}

// FUNCTION: MAGIC 0x004d153b
// FUNCTION: SHANDALAR 0x00577b17
int get_battlefield_card_stagger_offset(HWND hwnd)
{
  int card;
  card_id_t card_id;
  int player;
  unsigned int sign;

  player = GetWindowLongA(hwnd, 0);
  card = GetWindowLongA(hwnd, 4);
  card_id = get_displayed_card_id(player, card);
  sign = (int)card_id >> 0x1f;
  return (((card_id ^ sign) - sign & 3 ^ sign) - sign) * 2 + (card % 3) * 3;
}

// FUNCTION: MAGIC 0x004e915a
// FUNCTION: SHANDALAR 0x00506f72
void reset_battlefield_layout_positions(HWND hwnd)
{
  RECT rect;
  int player;

  if (hwnd == g_duel_player_battlefield_window_hwnd)
    player = 0;
  else
    player = 1;

  GetClientRect(hwnd, &rect);
  g_battlefield_land_x[player] = (rect.right - 10) - g_showlist_smallcard_width;
  g_battlefield_land_y[player] = 10;
  g_battlefield_creature_x[player] = 10;
  g_battlefield_creature_y[player] = (rect.bottom - 10) - g_showlist_smallcard_height;
  g_battlefield_creature_x_step = 5;
  g_battlefield_creature_x_spacing = 10;
  g_battlefield_creature_y_margin = 5;
  g_battlefield_noncreature_x[player] =
      (g_battlefield_land_x[player] - 10) - g_showlist_smallcard_width;
  g_battlefield_noncreature_y[player] = 10;
  g_battlefield_draw_placeholder_x[player] = 5;
  g_battlefield_draw_placeholder_y[player] = g_showlist_smallcard_height / 2;
}

// FUNCTION: MAGIC 0x004e9256
// FUNCTION: SHANDALAR 0x0050706e
void get_next_battlefield_card_position(HWND parent, int *rect, int value, int *x, int *y, int flag)
{
  int player;
  int hidden_descendants;
  int result_x;
  int result_y;
  RECT client_rect;

  if (parent == (HWND)0 || rect == (int *)0 ||
      get_displayed_card_id(rect[0], rect[1]) == -1 ||
      x == (int *)0 || y == (int *)0)
  {
    return;
  }

  if (parent == g_duel_player_battlefield_window_hwnd)
    player = 0;
  else
    player = 1;

  result_y = 0;
  result_x = result_y;
  GetClientRect(parent, &client_rect);
  if ((get_displayed_card_type(rect[0], rect[1]) & TYPE_CREATURE) != 0 &&
      (get_displayed_card_type(rect[0], rect[1]) & TYPE_LAND) == 0)
  {
    result_x = g_battlefield_creature_x[player];
    result_y = g_battlefield_creature_y[player] - value / 2;
    if (flag != 0)
    {
      g_battlefield_creature_x[player] += g_battlefield_creature_x_step + g_showlist_smallcard_width;
      if (client_rect.right - g_showlist_smallcard_width * 2 < g_battlefield_creature_x[player])
      {
        DAT_0057ab6c = (DAT_0057ab6c + 1) % 3;
        if (DAT_0057ab6c == 0)
          g_battlefield_creature_x[player] = 5;
        else if (DAT_0057ab6c == 1)
          g_battlefield_creature_x[player] = g_showlist_smallcard_width / 3;
        else if (DAT_0057ab6c == 2)
          g_battlefield_creature_x[player] = g_showlist_smallcard_width / 6;
        else
          g_battlefield_creature_x[player] = g_showlist_smallcard_width / 2;
        g_battlefield_creature_y[player] -= g_battlefield_creature_x_spacing + g_showlist_smallcard_height;
      }
      if (g_battlefield_creature_y[player] < 0)
      {
        g_battlefield_creature_y_margin += (g_showlist_smallcard_height * 40) / 100;
        if ((client_rect.bottom - 10) - g_showlist_smallcard_height < g_battlefield_creature_y_margin)
          g_battlefield_creature_y_margin = 10;
        g_battlefield_creature_y[player] =
            (client_rect.bottom - g_battlefield_creature_y_margin) - g_showlist_smallcard_height;
        if (DAT_0057ab6c == 0)
          g_battlefield_creature_x[player] = 5;
        else if (DAT_0057ab6c == 1)
          g_battlefield_creature_x[player] = g_showlist_smallcard_width / 3;
        else
          g_battlefield_creature_x[player] = g_showlist_smallcard_width / 6;
        g_battlefield_creature_x_step += 10;
      }
    }
  }
  else if ((get_displayed_card_type(rect[0], rect[1]) & TYPE_LAND) != 0)
  {
    result_x = g_battlefield_land_x[player] + value;
    result_y = g_battlefield_land_y[player];
    hidden_descendants = count_hidden_battlefield_descendants_for_card(parent, rect);
    if (0 < hidden_descendants)
      result_y += hidden_descendants * DAT_00939508 + 5;
    if (flag != 0)
    {
      if (hidden_descendants != 0)
        g_battlefield_land_y[player] += hidden_descendants * DAT_00939508 + 5;
      g_battlefield_land_y[player] += DAT_00939508;
      if ((client_rect.bottom - 5) - g_showlist_smallcard_height < g_battlefield_land_y[player])
      {
        g_battlefield_land_y[player] = g_showlist_smallcard_height / 2;
        g_battlefield_land_x[player] -= g_showlist_smallcard_width / 2;
      }
    }
  }
  else
  {
    if (get_displayed_card_id(rect[0], rect[1]) == unk_007a7d64)
    {
      result_x = g_battlefield_draw_placeholder_x[player];
      result_y = g_battlefield_draw_placeholder_y[player];
      if (flag != 0)
        g_battlefield_draw_placeholder_y[player] += DAT_00939508;
    }
    else
    {
      result_x = g_battlefield_noncreature_x[player] + value;
      result_y = g_battlefield_noncreature_y[player];
      hidden_descendants = count_hidden_battlefield_descendants_for_card(parent, rect);
      if (0 < hidden_descendants)
        result_y += hidden_descendants * DAT_00939508;
      if (flag != 0)
      {
        if (hidden_descendants != 0)
          g_battlefield_noncreature_y[player] += hidden_descendants * DAT_00939508;
        g_battlefield_noncreature_y[player] += DAT_00939508;
        g_battlefield_noncreature_y[player] += DAT_00939508 / 2;
        if ((client_rect.bottom - 10) - g_showlist_smallcard_height < g_battlefield_noncreature_y[player])
        {
          g_battlefield_noncreature_y[player] = g_showlist_smallcard_height / 2;
          g_battlefield_noncreature_x[player] -= g_showlist_smallcard_width / 2;
        }
      }
    }
  }
  *x = result_x;
  *y = result_y;
}

// FUNCTION: MAGIC 0x004e8e42
// FUNCTION: SHANDALAR 0x00506c5a
void resize_battlefield_child_card_windows(HWND hwnd)
{
  struct
  {
    int card_count;
    int index;
    HWND *card_windows;
    RECT rect;
    HWND snapshot_window;
  } s;

  GetClientRect(hwnd, &s.rect);
  s.card_count = GetWindowLongA(hwnd, g_duel_window_userdata_card_offset);
  s.card_windows = (HWND *)GetWindowLongA(hwnd, g_duel_window_userdata_player_offset);
  s.snapshot_window = (HWND)GetWindowLongA(hwnd, g_duel_window_userdata_snapshot_offset);

  for (s.index = 0; s.index < s.card_count; s.index++)
  {
    SetWindowPos(s.card_windows[s.index], (HWND)0, 0, 0,
                 g_showlist_smallcard_width, g_showlist_smallcard_height,
                 SWP_NOMOVE | SWP_NOZORDER);
  }
  for (s.index = 0; s.index < s.card_count; s.index++)
  {
    SendMessageA(hwnd, 0x410, (WPARAM)s.card_windows[s.index], 0);
  }
  SetWindowPos(s.snapshot_window, (HWND)0, 0, 0,
               g_showlist_smallcard_width, g_showlist_smallcard_height,
               SWP_NOMOVE | SWP_NOZORDER);
  SendMessageA(hwnd, 0x410, (WPARAM)s.snapshot_window, 0);
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
    int left_phasebar_x;
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
    int left_phasebar_x_copy; // ebp - 0xa8
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
    int player_row_y_copy;   // ebp - 0x7c
    int lower_phasebar_y;
    int right_chat_x;
    int player_chat_y;
    int opponent_life_y;
    int player_row_y;
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
    s.left_phasebar_x = s.right_phasebar_x - s.lower_phasebar_y;
    s.left_phasebar_x_copy = s.left_phasebar_x;
    s.opponent_phasebar_x = s.preview_top - s.half_phasebar_height;
    s.lower_middle_y = s.opponent_phasebar_x;
    s.player_row_y = s.preview_height + s.preview_top;
    s.player_row_y_copy = s.player_row_y;
    s.small_chat_height = s.preview_width - s.left_phasebar_width;
    s.player_life_x = s.rect.left;
    s.opponent_life_x = s.player_life_x;
    s.opponent_life_y = s.rect.top;
    s.lower_life_y = s.half_phasebar_height + s.player_row_y;
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
    MoveWindow(DAT_0092680c, s.left_phasebar_x_copy, s.lower_middle_y, s.lower_phasebar_y, s.phasebar_height, 1);
    MoveWindow(DAT_0091ce30, s.left_phasebar_x, s.player_row_y_copy, s.lower_phasebar_y, s.phasebar_height, 1);
    MoveWindow(g_phasebar_your_untap_window_hwnd, s.right_phasebar_x, s.opponent_phasebar_x, s.phasebar_width, s.half_phasebar_height, 1);
    MoveWindow(g_duel_player_graveyard_window_hwnd, s.face_x, s.player_row_y, s.phasebar_width, s.half_phasebar_height, 1);
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
    MoveWindow(g_duel_player_face_window_hwnd, s.player_life_x, s.player_row_y_copy, s.left_phasebar_width + s.small_chat_height, s.lower_middle_height, 1);
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
    s.left_phasebar_x = s.opponent_life_x;
    s.left_phasebar_x_copy = s.left_phasebar_x;
    s.opponent_phasebar_x = s.opponent_life_y - s.phasebar_height;
    s.lower_middle_y = s.opponent_phasebar_x;
    s.player_row_y = s.player_life_height + s.lower_life_y;
    s.player_row_y_copy = s.player_row_y;
    s.left_phasebar_width = s.small_chat_height;
    s.battlefield_width = s.opponent_life_x;
    s.preview_x = s.battlefield_width;
    s.preview_y = s.rect.top;
    s.player_face_x = s.player_row_y_copy + s.phasebar_height;
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
    MoveWindow(DAT_0092680c, s.left_phasebar_x_copy, s.lower_middle_y, s.lower_phasebar_y, s.phasebar_height, 1);
    MoveWindow(DAT_0091ce30, s.left_phasebar_x, s.player_row_y_copy, s.lower_phasebar_y, s.phasebar_height, 1);
    MoveWindow(g_phasebar_your_untap_window_hwnd, s.right_phasebar_x, s.opponent_phasebar_x, s.phasebar_width, s.half_phasebar_height, 1);
    MoveWindow(g_duel_player_graveyard_window_hwnd, s.face_x, s.player_row_y, s.phasebar_width, s.half_phasebar_height, 1);
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
  if (g_your_attack_window_hwnd != (HWND)0)
    return;

  DialogBoxParamA(g_app_instance, (LPCSTR)0xf9, g_duel_window_hwnd, still_thinking_dialog_proc, 0);
  g_your_attack_window_hwnd = (HWND)0;
}

typedef struct post_duel_draws_dialog_params_struct
{
  char message[300];
  int show_continue_button;
  int show_keep_button;
  int player_top_card;
  int opponent_top_card;
} post_duel_draws_dialog_params_t;

// GLOBAL: MAGIC 0x00638c78
// GLOBAL: SHANDALAR 0x006503c0
static HBITMAP g_post_duel_draws_background_bitmap;
// GLOBAL: MAGIC 0x00638bb0
// GLOBAL: SHANDALAR 0x006502f8
static COLORREF g_post_duel_draws_text_color;
// GLOBAL: MAGIC 0x00638b44
// GLOBAL: SHANDALAR 0x0065028c
static COLORREF g_post_duel_draws_button_text_color;
// GLOBAL: MAGIC 0x00638bdc
// GLOBAL: SHANDALAR 0x00650324
static HBRUSH g_post_duel_draws_button_brush;
// GLOBAL: MAGIC 0x00638b64
// GLOBAL: SHANDALAR 0x006502ac
static HPEN g_post_duel_draws_button_pen1;
// GLOBAL: MAGIC 0x00638c24
// GLOBAL: SHANDALAR 0x0065036c
static HPEN g_post_duel_draws_button_pen2;
// GLOBAL: MAGIC 0x00638b3c
// GLOBAL: SHANDALAR 0x00650284
static COLORREF g_post_duel_draws_button_unfocus_color;
// GLOBAL: MAGIC 0x00638bb4
// GLOBAL: SHANDALAR 0x006502fc
static COLORREF g_post_duel_draws_button_focus_color;
// GLOBAL: MAGIC 0x0069e24c
// GLOBAL: SHANDALAR 0x006503f4
static COLORREF g_still_thinking_shadow_color;
// GLOBAL: MAGIC 0x0069e248
// GLOBAL: SHANDALAR 0x006503f0
static COLORREF g_still_thinking_text_color;

void setup_post_duel_draws_dialog_resources(HBITMAP *background,
                                            COLORREF *text_color,
                                            COLORREF *button_text_color,
                                            HBRUSH *button_brush,
                                            HPEN *pen1,
                                            HPEN *pen2,
                                            COLORREF *button_unfocus_color,
                                            COLORREF *button_focus_color);
void cleanup_post_duel_draws_dialog_resources(HBITMAP background, HBRUSH button_brush, HPEN pen1, HPEN pen2);

// FUNCTION: MAGIC 0x0049d77b
// FUNCTION: SHANDALAR 0x00539128
void show_post_duel_draws(int duel_result)
{
  struct
  {
    post_duel_draws_dialog_params_t dialog_params;
    int cached_library_cards[500];
    int opponent_top_card;
    char opponent_name[100];
    int player_top_card;
    int unused;
  } s;

  KillTimer(g_duel_window_hwnd, g_duel_timer_id);
  if (copy_cached_library_cards_and_get_count(s.cached_library_cards, 0) != 0)
  {
    s.player_top_card = s.cached_library_cards[0];
  }
  else
  {
    s.player_top_card = -1;
  }
  if (copy_cached_library_cards_and_get_count(s.cached_library_cards, 1) != 0)
  {
    s.opponent_top_card = s.cached_library_cards[0];
  }
  else
  {
    s.opponent_top_card = -1;
  }
  load_text(global_ui_strings_filename, "DIALOG_SHANDALARENDDUEL");
  if (duel_result == 0)
  {
    copy_opponent_name_prefix(s.opponent_name);
    sprintf(s.dialog_params.message, text_lines[0], s.opponent_name);
  }
  else if (duel_result == 1)
  {
    strcpy(s.dialog_params.message, text_lines[1]);
  }
  else
  {
    strcpy(s.dialog_params.message, text_lines[2]);
  }
  s.dialog_params.show_continue_button = 0;
  s.dialog_params.show_keep_button = 0;
  s.dialog_params.player_top_card = s.player_top_card;
  s.dialog_params.opponent_top_card = s.opponent_top_card;
  s.unused = DialogBoxParamA(g_app_instance, (LPCSTR)0xf6, g_duel_window_hwnd, post_duel_draws_dialog_proc,
                             (LPARAM)&s.dialog_params);
}

// FUNCTION: MAGIC 0x0049d9b4
// FUNCTION: SHANDALAR 0x00539361
BOOL CALLBACK post_duel_draws_dialog_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  struct
  {
    HDC paint_dc;
    PAINTSTRUCT paint;
    int control_id;
    RECT card_rect;
    int selected_player;
    int card_id;
    int preview_player_card;
    POINT mouse_point;
    RECT opponent_card_rect;
    RECT player_card_rect;
    int preview_opponent_card;
    RECT drag_after_rect;
    RECT drag_before_rect;
    HDC erase_dc;
    RECT erase_rect;
    COLORREF draw_color;
    DRAWITEMSTRUCT *draw_item;
    HWND ctl_hwnd;
    int ctl_id;
    HDC ctl_hdc;
    HBRUSH brush;
    HWND previous_control;
    HWND current_control;
    int selected_control;
    char text[200];
    char opponent_name[200];
    post_duel_draws_dialog_params_t *dialog_params;
  } s;

  switch (msg)
  {
  case WM_INITDIALOG:
    s.dialog_params = (post_duel_draws_dialog_params_t *)lparam;
    SetWindowLongA(hwnd, DWL_USER, (LONG)s.dialog_params);
    setup_post_duel_draws_dialog_resources(&g_post_duel_draws_background_bitmap,
                                           &g_post_duel_draws_text_color,
                                           &g_post_duel_draws_button_text_color,
                                           &g_post_duel_draws_button_brush,
                                           &g_post_duel_draws_button_pen1,
                                           &g_post_duel_draws_button_pen2,
                                           &g_post_duel_draws_button_unfocus_color,
                                           &g_post_duel_draws_button_focus_color);
    ShowWindow(GetDlgItem(hwnd, 0x4e5), SW_HIDE);
    ShowWindow(GetDlgItem(hwnd, 0x4e6), SW_HIDE);
    SetDlgItemTextA(hwnd, IDOK, gs_ok_00924800);
    load_text(global_ui_strings_filename, "DIALOG_ENDDUEL");
    copy_opponent_name_prefix(s.opponent_name);
    sprintf(s.text, text_lines[0], s.opponent_name);
    SetDlgItemTextA(hwnd, 0x4e3, s.text);
    SetDlgItemTextA(hwnd, 0x4e2, text_lines[1]);
    SetDlgItemTextA(hwnd, 0x4e4, s.dialog_params->message);
    load_text(global_ui_strings_filename, "DIALOG_GAUNTLETENDDUEL");
    SetDlgItemTextA(hwnd, 0x4e7, text_lines[2]);
    SetDlgItemTextA(hwnd, 0x4e8, text_lines[3]);
    if (s.dialog_params->show_continue_button == 0)
    {
      ShowWindow(GetDlgItem(hwnd, 0x4e7), SW_HIDE);
      ShowWindow(GetDlgItem(hwnd, 0x4e8), SW_HIDE);
    }
    else
    {
      if (s.dialog_params->show_keep_button != 0)
      {
        s.selected_control = 0x4e7;
      }
      else
      {
        ShowWindow(GetDlgItem(hwnd, 0x4e7), SW_HIDE);
        SetDlgItemTextA(hwnd, 0x4e8, gs_ok_00924800);
        s.selected_control = 0x4e8;
      }
      SetFocus(GetDlgItem(hwnd, s.selected_control));
      SendMessageA(hwnd, 0x401, s.selected_control, 0);
      change_buttonclass_wndproc(hwnd);
    }
    return FALSE;

  case WM_COMMAND:
    if ((wparam & 0xffff) == 0x4e7)
    {
      cleanup_post_duel_draws_dialog_resources(g_post_duel_draws_background_bitmap,
                                               g_post_duel_draws_button_brush,
                                               g_post_duel_draws_button_pen1,
                                               g_post_duel_draws_button_pen2);
      EndDialog(hwnd, 4);
    }
    else if (((wparam & 0xffff) == 0x4e8) || ((wparam & 0xffff) == IDCANCEL))
    {
      cleanup_post_duel_draws_dialog_resources(g_post_duel_draws_background_bitmap,
                                               g_post_duel_draws_button_brush,
                                               g_post_duel_draws_button_pen1,
                                               g_post_duel_draws_button_pen2);
      EndDialog(hwnd, 0);
    }
    return TRUE;

  case 0x4c8:
    s.current_control = (HWND)wparam;
    s.previous_control = (HWND)lparam;
    if (s.current_control != (HWND)0)
    {
      SendMessageA(hwnd, 0x401, (WPARAM)s.current_control, 0);
    }
    if (s.current_control != (HWND)0)
    {
      InvalidateRect(s.current_control, (RECT *)0, TRUE);
    }
    if (s.previous_control != (HWND)0)
    {
      InvalidateRect(s.previous_control, (RECT *)0, TRUE);
    }
    return FALSE;

  case WM_CTLCOLORBTN:
  case WM_CTLCOLORSTATIC:
    s.ctl_hdc = (HDC)wparam;
    ApplyCardArtPaletteToDc(s.ctl_hdc);
    s.ctl_hwnd = (HWND)lparam;
    s.ctl_id = GetDlgCtrlID(s.ctl_hwnd);
    if ((s.ctl_id == 0x4e7) || (s.ctl_id == 0x4e8))
    {
      if (GetFocus() == s.ctl_hwnd)
      {
        SetTextColor(s.ctl_hdc, g_post_duel_draws_button_focus_color);
      }
      else
      {
        SetTextColor(s.ctl_hdc, g_post_duel_draws_button_text_color);
      }
      SetBkMode(s.ctl_hdc, TRANSPARENT);
      s.brush = GetStockObject(NULL_BRUSH);
    }
    else
    {
      SetTextColor(s.ctl_hdc, g_post_duel_draws_text_color);
      SetBkMode(s.ctl_hdc, TRANSPARENT);
      s.brush = GetStockObject(NULL_BRUSH);
    }
    return (BOOL)s.brush;

  case WM_DRAWITEM:
    s.draw_item = (DRAWITEMSTRUCT *)lparam;
    if (GetFocus() == s.draw_item->hwndItem)
    {
      s.draw_color = g_post_duel_draws_button_focus_color;
    }
    else
    {
      s.draw_color = g_post_duel_draws_button_unfocus_color;
    }
    if (*(int *)&gs_window_title_your_hand_00777bf0[20] == 0)
    {
      s.draw_color = g_post_duel_draws_button_unfocus_color;
    }
    draw_owner_draw_button_centered(s.draw_item,
                 g_post_duel_draws_button_brush,
                 g_post_duel_draws_button_pen1,
                 g_post_duel_draws_button_pen2,
                 s.draw_color,
                 0);
    return TRUE;

  case WM_QUERYNEWPALETTE:
  case WM_PALETTECHANGED:
  case WM_PALETTEISCHANGING:
    return handle_button_palette_message((int)hwnd, msg, (int)wparam, lparam);

  case WM_ERASEBKGND:
    s.erase_dc = (HDC)wparam;
    ApplyCardArtPaletteToDc(s.erase_dc);
    GetClientRect(hwnd, &s.erase_rect);
    if (g_post_duel_draws_background_bitmap != (HBITMAP)0)
    {
      DrawBitmapToRect(s.erase_dc, &s.erase_rect, g_post_duel_draws_background_bitmap);
    }
    else
    {
      FillRect(s.erase_dc, &s.erase_rect, GetStockObject(GRAY_BRUSH));
    }
    return TRUE;

  case WM_CHAR:
    s.dialog_params = (post_duel_draws_dialog_params_t *)GetWindowLongA(hwnd, DWL_USER);
    if (s.dialog_params->show_continue_button == 0)
    {
      if ((wparam == 0xd) || (wparam == 0x20) || (wparam == 0x1b))
      {
        cleanup_post_duel_draws_dialog_resources(g_post_duel_draws_background_bitmap,
                                                 g_post_duel_draws_button_brush,
                                                 g_post_duel_draws_button_pen1,
                                                 g_post_duel_draws_button_pen2);
        EndDialog(hwnd, 1);
      }
    }
    return TRUE;

  case WM_LBUTTONDOWN:
    GetWindowRect(hwnd, &s.drag_before_rect);
    SendMessageA(hwnd, WM_SYSCOMMAND, 0xf012, 0);
    GetWindowRect(hwnd, &s.drag_after_rect);
    if (abs(s.drag_after_rect.left - s.drag_before_rect.left) + abs(s.drag_after_rect.top - s.drag_before_rect.top) > 5)
    {
    }
    else
    {
      s.dialog_params = (post_duel_draws_dialog_params_t *)GetWindowLongA(hwnd, DWL_USER);
      if (s.dialog_params->show_continue_button == 0)
      {
        cleanup_post_duel_draws_dialog_resources(g_post_duel_draws_background_bitmap,
                                                 g_post_duel_draws_button_brush,
                                                 g_post_duel_draws_button_pen1,
                                                 g_post_duel_draws_button_pen2);
        EndDialog(hwnd, 1);
      }
    }
    return TRUE;

  case WM_MOUSEMOVE:
  case WM_RBUTTONDOWN:
    s.mouse_point.x = lparam & 0xffff;
    s.mouse_point.y = (unsigned short)(((unsigned int)lparam >> 16) & 0xffff);
    s.dialog_params = (post_duel_draws_dialog_params_t *)GetWindowLongA(hwnd, DWL_USER);
    if (((msg == WM_MOUSEMOVE) && (g_duel_interface_options.layout != 2)) ||
        ((msg == WM_RBUTTONDOWN) && (g_duel_interface_options.layout == 2)))
    {
      s.preview_opponent_card = s.dialog_params->player_top_card;
      GetWindowRect(GetDlgItem(hwnd, 0x4e6), &s.opponent_card_rect);
      MapWindowPoints((HWND)0, hwnd, (LPPOINT)&s.opponent_card_rect, 2);
      s.preview_player_card = s.dialog_params->opponent_top_card;
      GetWindowRect(GetDlgItem(hwnd, 0x4e5), &s.player_card_rect);
      MapWindowPoints((HWND)0, hwnd, (LPPOINT)&s.player_card_rect, 2);
      if ((s.preview_opponent_card != -1) && (PtInRect(&s.opponent_card_rect, s.mouse_point) != 0))
      {
        SendMessageA(g_duel_card_preview_window_hwnd, 0x401, s.preview_opponent_card, 0);
      }
      else if ((s.preview_player_card != -1) && (PtInRect(&s.player_card_rect, s.mouse_point) != 0))
      {
        SendMessageA(g_duel_card_preview_window_hwnd, 0x401, s.preview_player_card, 0);
      }
    }
    return FALSE;

  case WM_PAINT:
    s.dialog_params = (post_duel_draws_dialog_params_t *)GetWindowLongA(hwnd, DWL_USER);
    UpdateWindow(GetDlgItem(hwnd, 0x4e4));
    get_current_duel_selection(&s.selected_player, (int *)0);
    if (s.selected_player == 0)
    {
      UpdateWindow(GetDlgItem(hwnd, 0x4e2));
    }
    else
    {
      UpdateWindow(GetDlgItem(hwnd, 0x4e3));
    }
    s.paint_dc = BeginPaint(hwnd, &s.paint);
    if (s.paint_dc != (HDC)0)
    {
      ApplyCardArtPaletteToDc(s.paint_dc);
      if (s.selected_player == 0)
      {
        s.card_id = s.dialog_params->player_top_card;
      }
      else
      {
        s.card_id = s.dialog_params->opponent_top_card;
      }
      if (s.selected_player == 0)
      {
        s.control_id = 0x4e6;
      }
      else
      {
        s.control_id = 0x4e5;
      }
      if (s.card_id != -1)
      {
        GetWindowRect(GetDlgItem(hwnd, s.control_id), &s.card_rect);
        MapWindowPoints((HWND)0, hwnd, (LPPOINT)&s.card_rect, 2);
        DrawFullCard(s.paint_dc, &s.card_rect, global_raw_cards_storage + s.card_id, 0, 0x12, 0, gs_illus_00789130);
      }
      if (s.selected_player == 0)
      {
        s.card_id = s.dialog_params->opponent_top_card;
      }
      else
      {
        s.card_id = s.dialog_params->player_top_card;
      }
      if (s.selected_player == 0)
      {
        s.control_id = 0x4e5;
      }
      else
      {
        s.control_id = 0x4e6;
      }
      if (s.card_id != -1)
      {
        GetWindowRect(GetDlgItem(hwnd, s.control_id), &s.card_rect);
        MapWindowPoints((HWND)0, hwnd, (LPPOINT)&s.card_rect, 2);
        DrawFullCard(s.paint_dc, &s.card_rect, global_raw_cards_storage + s.card_id, 0, 0x12, 0, gs_illus_00789130);
      }
      EndPaint(hwnd, &s.paint);
    }
    return TRUE;

  default:
    return FALSE;
  }
}

// FUNCTION: MAGIC 0x0049e567
// FUNCTION: SHANDALAR 0x00539f11
void setup_post_duel_draws_dialog_resources(HBITMAP *background,
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
  {
    *button_brush = GetStockObject(GRAY_BRUSH);
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

// FUNCTION: MAGIC 0x0049e64d
// FUNCTION: SHANDALAR 0x00539ff7
void cleanup_post_duel_draws_dialog_resources(HBITMAP background, HBRUSH button_brush, HPEN pen1, HPEN pen2)
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

// FUNCTION: MAGIC 0x004ddc39
// FUNCTION: SHANDALAR 0x005448ea
BOOL CALLBACK still_thinking_dialog_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  struct
  {
    char text[100];
    HDC dc;
    HGDIOBJ font;
    RECT rect;
    int player;
    int card;
    HWND card_window;
    RECT client_rect;
  } s;

  switch (msg)
  {
  case WM_INITDIALOG:
    g_your_attack_window_hwnd = hwnd;
    g_still_thinking_shadow_color = 0x100009a;
    g_still_thinking_text_color = 0x10000c9;
    SetDlgItemTextA(hwnd, 0x402, gs_still_thinking_0091bd10);
    s.player = 1;
    s.card = -1;
    GetClientRect(hwnd, &s.client_rect);
    s.card_window = CreateWindowExA(0, "MAGICGAME_CardClass", "StillThinking small card", WS_CHILD | WS_VISIBLE,
                                    (s.client_rect.right - g_showlist_smallcard_width) / 2,
                                    (s.client_rect.bottom - g_showlist_smallcard_height) - 10,
                                    g_showlist_smallcard_width, g_showlist_smallcard_height, hwnd, (HMENU)1, g_app_instance, &s.player);
    SetTimer(hwnd, 1, 3000, (TIMERPROC)0);
    return TRUE;

  case WM_QUERYNEWPALETTE:
  case WM_PALETTECHANGED:
  case WM_PALETTEISCHANGING:
    return handle_button_palette_message((int)hwnd, msg, (int)wparam, lparam);

  case WM_ERASEBKGND:
    s.dc = (HDC)wparam;
    ApplyCardArtPaletteToDc(s.dc);
    GetClientRect(hwnd, &s.rect);
    FillRect(s.dc, &s.rect, GetStockObject(BLACK_BRUSH));
    s.font = (HGDIOBJ)SendDlgItemMessageA(hwnd, 0x402, WM_GETFONT, 0, 0);
    SelectObject(s.dc, s.font);
    SetBkMode(s.dc, TRANSPARENT);
    SetTextColor(s.dc, g_still_thinking_shadow_color);
    GetWindowRect(GetDlgItem(hwnd, 0x402), &s.rect);
    MapWindowPoints((HWND)0, hwnd, (LPPOINT)&s.rect, 2);
    GetDlgItemTextA(hwnd, 0x402, s.text, 100);
    SetTextColor(s.dc, g_still_thinking_text_color);
    DrawTextA(s.dc, s.text, -1, &s.rect, DT_CENTER);
    OffsetRect(&s.rect, -2, -2);
    SetTextColor(s.dc, g_still_thinking_shadow_color);
    DrawTextA(s.dc, s.text, -1, &s.rect, DT_CENTER);
    return TRUE;

  case WM_COMMAND:
  case WM_KEYDOWN:
    EndDialog(hwnd, 0);
    return TRUE;

  case WM_LBUTTONDOWN:
  case WM_RBUTTONDOWN:
    EndDialog(hwnd, 0);
    return TRUE;

  case WM_TIMER:
    EndDialog(hwnd, 0);
    return TRUE;

  default:
    return FALSE;
  }
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

static __inline int pick_card_color_matches(card_ptr_t *card, unsigned int color_filter)
{
  if (color_filter == PICK_CARD_COLOR_GOLD)
  {
    return card->color == CP_COLOR_MULTI;
  }

  if (card->color == CP_COLOR_ARTIFACT || card->color == CP_COLOR_LAND)
  {
    return 1;
  }

  if ((color_filter & PICK_CARD_COLOR_BLACK) != 0 && (card->color == CP_COLOR_BLACK || card->req.req_black != 0))
  {
    return 1;
  }

  if ((color_filter & PICK_CARD_COLOR_BLUE) != 0 && (card->color == CP_COLOR_BLUE || card->req.req_blue != 0))
  {
    return 1;
  }

  if ((color_filter & PICK_CARD_COLOR_GREEN) != 0 && (card->color == CP_COLOR_GREEN || card->req.req_green != 0))
  {
    return 1;
  }

  if ((color_filter & PICK_CARD_COLOR_RED) != 0 && (card->color == CP_COLOR_RED || card->req.req_red != 0))
  {
    return 1;
  }

  if ((color_filter & PICK_CARD_COLOR_WHITE) != 0 && (card->color == CP_COLOR_WHITE || card->req.req_white != 0))
  {
    return 1;
  }

  return 0;
}

static __inline int pick_card_type_matches(card_ptr_t *card, unsigned int type_filter)
{
  if ((type_filter & PICK_CARD_TYPE_LAND) != 0 && card->card_type == CP_TYPE_LAND)
  {
    return 1;
  }

  if ((type_filter & PICK_CARD_TYPE_CREATURE) != 0 && card->card_type == CP_TYPE_CREATURE)
  {
    return 1;
  }

  if ((type_filter & PICK_CARD_TYPE_ENCHANTMENT) != 0 && card->card_type == CP_TYPE_ENCHANTMENT)
  {
    return 1;
  }

  if ((type_filter & PICK_CARD_TYPE_SORCERY) != 0 && card->card_type == CP_TYPE_SORCERY)
  {
    return 1;
  }

  if ((type_filter & PICK_CARD_TYPE_INSTANT) != 0 && card->card_type == CP_TYPE_INSTANT)
  {
    return 1;
  }

  if ((type_filter & PICK_CARD_TYPE_INTERRUPT) != 0 && card->card_type == CP_TYPE_INTERRUPT)
  {
    return 1;
  }

  if ((type_filter & PICK_CARD_TYPE_ARTIFACT) != 0 && card->card_type == CP_TYPE_ARTIFACT)
  {
    return 1;
  }

  return 0;
}

static __inline int pick_card_is_available(csvid_t csvid)
{
  return IsCardAvailable(csvid, 0) != 0 || IsCardAvailable(csvid, 1) != 0 || IsCardAvailable(csvid, 2) != 0;
}

// FUNCTION: MAGIC 0x00506b4c
static void populate_pick_card_list(HWND listbox, unsigned int color_filter, unsigned int type_filter)
{
  struct
  {
    int card_index;
    int list_index;
  } s;

  SendMessageA(listbox, LB_RESETCONTENT, 0, 0);
  for (s.card_index = 0; s.card_index < global_available_slots; ++s.card_index)
  {
    if (!pick_card_color_matches(&global_raw_cards_storage[s.card_index], color_filter))
    {
      continue;
    }

    if (!pick_card_type_matches(&global_raw_cards_storage[s.card_index], type_filter))
    {
      continue;
    }

    if (!pick_card_is_available(global_raw_cards_storage[s.card_index].id))
    {
      continue;
    }

    s.list_index = SendMessageA(listbox, LB_ADDSTRING, 0, (LPARAM)global_raw_cards_storage[s.card_index].full_name);
    if (s.list_index != LB_ERR)
    {
      SendMessageA(listbox, LB_SETITEMDATA, s.list_index, s.card_index);
    }
  }
}

static __inline void check_pick_card_filter_buttons(HWND hwnd, PickCardDialogContext *context)
{
  if ((context->color_filter & PICK_CARD_COLOR_BLACK) != 0)
  {
    CheckRadioButton(hwnd, PICK_CARD_COLOR_BLACK_ID, PICK_CARD_COLOR_GOLD_ID, PICK_CARD_COLOR_BLACK_ID);
  }
  else if ((context->color_filter & PICK_CARD_COLOR_BLUE) != 0)
  {
    CheckRadioButton(hwnd, PICK_CARD_COLOR_BLACK_ID, PICK_CARD_COLOR_GOLD_ID, PICK_CARD_COLOR_BLUE_ID);
  }
  else if ((context->color_filter & PICK_CARD_COLOR_GREEN) != 0)
  {
    CheckRadioButton(hwnd, PICK_CARD_COLOR_BLACK_ID, PICK_CARD_COLOR_GOLD_ID, PICK_CARD_COLOR_GREEN_ID);
  }
  else if ((context->color_filter & PICK_CARD_COLOR_RED) != 0)
  {
    CheckRadioButton(hwnd, PICK_CARD_COLOR_BLACK_ID, PICK_CARD_COLOR_GOLD_ID, PICK_CARD_COLOR_RED_ID);
  }
  else if ((context->color_filter & PICK_CARD_COLOR_WHITE) != 0)
  {
    CheckRadioButton(hwnd, PICK_CARD_COLOR_BLACK_ID, PICK_CARD_COLOR_GOLD_ID, PICK_CARD_COLOR_WHITE_ID);
  }
  else
  {
    CheckRadioButton(hwnd, PICK_CARD_COLOR_BLACK_ID, PICK_CARD_COLOR_GOLD_ID, PICK_CARD_COLOR_GOLD_ID);
  }

  if ((context->type_filter & PICK_CARD_TYPE_LAND) != 0)
  {
    CheckRadioButton(hwnd, PICK_CARD_TYPE_CREATURE_ID, PICK_CARD_TYPE_LAND_ID, PICK_CARD_TYPE_LAND_ID);
  }
  else if ((context->type_filter & PICK_CARD_TYPE_CREATURE) != 0)
  {
    CheckRadioButton(hwnd, PICK_CARD_TYPE_CREATURE_ID, PICK_CARD_TYPE_LAND_ID, PICK_CARD_TYPE_CREATURE_ID);
  }
  else if ((context->type_filter & PICK_CARD_TYPE_ENCHANTMENT) != 0)
  {
    CheckRadioButton(hwnd, PICK_CARD_TYPE_CREATURE_ID, PICK_CARD_TYPE_LAND_ID, PICK_CARD_TYPE_ENCHANTMENT_ID);
  }
  else if ((context->type_filter & PICK_CARD_TYPE_SORCERY) != 0)
  {
    CheckRadioButton(hwnd, PICK_CARD_TYPE_CREATURE_ID, PICK_CARD_TYPE_LAND_ID, PICK_CARD_TYPE_SORCERY_ID);
  }
  else if ((context->type_filter & (PICK_CARD_TYPE_INSTANT | PICK_CARD_TYPE_INTERRUPT)) != 0)
  {
    CheckRadioButton(hwnd, PICK_CARD_TYPE_CREATURE_ID, PICK_CARD_TYPE_LAND_ID, PICK_CARD_TYPE_INSTANT_ID);
  }
  else
  {
    CheckRadioButton(hwnd, PICK_CARD_TYPE_CREATURE_ID, PICK_CARD_TYPE_LAND_ID, PICK_CARD_TYPE_ARTIFACT_ID);
  }
}

static __inline void set_pick_card_filter_from_command(PickCardDialogContext *context, unsigned int command)
{
  switch (command)
  {
  case PICK_CARD_COLOR_BLACK_ID:
    context->color_filter = PICK_CARD_COLOR_BLACK;
    break;
  case PICK_CARD_COLOR_BLUE_ID:
    context->color_filter = PICK_CARD_COLOR_BLUE;
    break;
  case PICK_CARD_COLOR_GREEN_ID:
    context->color_filter = PICK_CARD_COLOR_GREEN;
    break;
  case PICK_CARD_COLOR_RED_ID:
    context->color_filter = PICK_CARD_COLOR_RED;
    break;
  case PICK_CARD_COLOR_WHITE_ID:
    context->color_filter = PICK_CARD_COLOR_WHITE;
    break;
  case PICK_CARD_COLOR_GOLD_ID:
    context->color_filter = PICK_CARD_COLOR_GOLD;
    break;
  case PICK_CARD_TYPE_CREATURE_ID:
    context->type_filter = PICK_CARD_TYPE_CREATURE;
    break;
  case PICK_CARD_TYPE_ARTIFACT_ID:
    context->type_filter = PICK_CARD_TYPE_ARTIFACT;
    break;
  case PICK_CARD_TYPE_ENCHANTMENT_ID:
    context->type_filter = PICK_CARD_TYPE_ENCHANTMENT;
    break;
  case PICK_CARD_TYPE_SORCERY_ID:
    context->type_filter = PICK_CARD_TYPE_SORCERY;
    break;
  case PICK_CARD_TYPE_INSTANT_ID:
    context->type_filter = PICK_CARD_TYPE_INSTANT | PICK_CARD_TYPE_INTERRUPT;
    break;
  case PICK_CARD_TYPE_LAND_ID:
    context->type_filter = PICK_CARD_TYPE_LAND;
    break;
  }
}

static __inline int get_selected_pick_card_csvid(HWND hwnd)
{
  struct
  {
    HWND listbox;
    int selected_index;
  } s;

  s.listbox = GetDlgItem(hwnd, PICK_CARD_LISTBOX_ID);
  s.selected_index = SendMessageA(s.listbox, LB_GETCURSEL, 0, 0);
  if (s.selected_index == LB_ERR)
  {
    return -1;
  }

  return SendMessageA(s.listbox, LB_GETITEMDATA, s.selected_index, 0);
}

static __inline void select_first_pick_card(HWND hwnd)
{
  struct
  {
    HWND listbox;
    int selected_csvid;
  } s;

  s.listbox = GetDlgItem(hwnd, PICK_CARD_LISTBOX_ID);
  if (SendMessageA(s.listbox, LB_GETCOUNT, 0, 0) > 0)
  {
    SendMessageA(s.listbox, LB_SETCURSEL, 0, 0);
    s.selected_csvid = get_selected_pick_card_csvid(hwnd);
    SendMessageA(GetDlgItem(hwnd, PICK_CARD_FULLCARD_ID), 0x401, s.selected_csvid, 0);
  }
  else
  {
    SendMessageA(GetDlgItem(hwnd, PICK_CARD_FULLCARD_ID), 0x401, (WPARAM)-1, 0);
  }
}

// FUNCTION: MAGIC 0x0050630c
static BOOL CALLBACK dlgproc_pick_card_from_list(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  struct
  {
    int selected_index;
    int selected_card;
    int show_color_filters;
    int list_selected_index;
    int list_selected_card;
    int color_count;
    int color_index;
  } s;

  switch (msg)
  {
  case WM_INITDIALOG:
    g_pick_card_dialog_context_ptr = (PickCardDialogContext *)lparam;
    if (g_pick_card_dialog_context_ptr->prompt != NULL)
    {
      SetWindowTextA(hwnd, g_pick_card_dialog_context_ptr->prompt);
    }
    SendDlgItemMessageA(hwnd, PICK_CARD_FULLCARD_ID, 0x401, (WPARAM)-1, 0);
    SetWindowLongA(GetDlgItem(hwnd, PICK_CARD_FULLCARD_ID), 0xc, 1);

    s.color_count = 0;
    for (s.color_index = 0; s.color_index < 5; ++s.color_index)
    {
      if ((g_pick_card_dialog_context_ptr->color_filter & (1 << (byte)s.color_index)) != 0)
      {
        ++s.color_count;
      }
    }

    if (s.color_count > 1)
    {
      CheckDlgButton(hwnd, PICK_CARD_COLOR_GOLD_ID, 1);
    }
    else if ((g_pick_card_dialog_context_ptr->color_filter & PICK_CARD_COLOR_BLACK) != 0)
    {
      CheckDlgButton(hwnd, PICK_CARD_COLOR_BLACK_ID, 1);
    }
    else if ((g_pick_card_dialog_context_ptr->color_filter & PICK_CARD_COLOR_BLUE) != 0)
    {
      CheckDlgButton(hwnd, PICK_CARD_COLOR_BLUE_ID, 1);
    }
    else if ((g_pick_card_dialog_context_ptr->color_filter & PICK_CARD_COLOR_GREEN) != 0)
    {
      CheckDlgButton(hwnd, PICK_CARD_COLOR_GREEN_ID, 1);
    }
    else if ((g_pick_card_dialog_context_ptr->color_filter & PICK_CARD_COLOR_RED) != 0)
    {
      CheckDlgButton(hwnd, PICK_CARD_COLOR_RED_ID, 1);
    }
    else if ((g_pick_card_dialog_context_ptr->color_filter & PICK_CARD_COLOR_WHITE) != 0)
    {
      CheckDlgButton(hwnd, PICK_CARD_COLOR_WHITE_ID, 1);
    }
    else
    {
      CheckDlgButton(hwnd, PICK_CARD_COLOR_WHITE_ID, 1);
    }

    if ((g_pick_card_dialog_context_ptr->type_filter & PICK_CARD_TYPE_LAND) != 0)
    {
      CheckDlgButton(hwnd, PICK_CARD_TYPE_LAND_ID, 1);
    }
    else if ((g_pick_card_dialog_context_ptr->type_filter & PICK_CARD_TYPE_CREATURE) != 0)
    {
      CheckDlgButton(hwnd, PICK_CARD_TYPE_CREATURE_ID, 1);
    }
    else if ((g_pick_card_dialog_context_ptr->type_filter & PICK_CARD_TYPE_ENCHANTMENT) != 0)
    {
      CheckDlgButton(hwnd, PICK_CARD_TYPE_ENCHANTMENT_ID, 1);
    }
    else if ((g_pick_card_dialog_context_ptr->type_filter & PICK_CARD_TYPE_SORCERY) != 0)
    {
      CheckDlgButton(hwnd, PICK_CARD_TYPE_SORCERY_ID, 1);
    }
    else if ((g_pick_card_dialog_context_ptr->type_filter & PICK_CARD_TYPE_INSTANT) != 0)
    {
      CheckDlgButton(hwnd, PICK_CARD_TYPE_INSTANT_ID, 1);
    }
    else if ((g_pick_card_dialog_context_ptr->type_filter & PICK_CARD_TYPE_INTERRUPT) != 0)
    {
      CheckDlgButton(hwnd, PICK_CARD_TYPE_INSTANT_ID, 1);
    }
    else if ((g_pick_card_dialog_context_ptr->type_filter & PICK_CARD_TYPE_ARTIFACT) != 0)
    {
      CheckDlgButton(hwnd, PICK_CARD_TYPE_ARTIFACT_ID, 1);
    }
    else
    {
      CheckDlgButton(hwnd, PICK_CARD_TYPE_ARTIFACT_ID, 1);
    }

    populate_pick_card_list(GetDlgItem(hwnd, PICK_CARD_LISTBOX_ID),
                            g_pick_card_dialog_context_ptr->color_filter,
                            g_pick_card_dialog_context_ptr->type_filter);
    SetFocus(GetDlgItem(hwnd, PICK_CARD_LISTBOX_ID));
    return 0;

  case WM_COMMAND:
    switch ((int)(wparam & 0xffff))
    {
    case PICK_CARD_LISTBOX_ID:
      if (HIWORD(wparam) == LBN_SELCHANGE)
      {
        s.list_selected_index = SendDlgItemMessageA(hwnd, PICK_CARD_LISTBOX_ID, LB_GETCURSEL, 0, 0);
        s.list_selected_card = SendDlgItemMessageA(hwnd, PICK_CARD_LISTBOX_ID, LB_GETITEMDATA, s.list_selected_index, 0);
        SendDlgItemMessageA(hwnd, PICK_CARD_FULLCARD_ID, 0x401, s.list_selected_card, 0);
      }
      else if (HIWORD(wparam) == LBN_DBLCLK)
      {
        SendMessageA(hwnd, WM_COMMAND, IDOK, (LPARAM)GetDlgItem(hwnd, IDOK));
      }
      break;

    case PICK_CARD_COLOR_BLACK_ID:
    case PICK_CARD_COLOR_BLUE_ID:
    case PICK_CARD_COLOR_GREEN_ID:
    case PICK_CARD_COLOR_RED_ID:
    case PICK_CARD_COLOR_WHITE_ID:
    case PICK_CARD_COLOR_GOLD_ID:
      if (HIWORD(wparam) == 0)
      {
        g_pick_card_dialog_context_ptr->color_filter = 0;
        if (IsDlgButtonChecked(hwnd, PICK_CARD_COLOR_BLACK_ID) != 0)
        {
          g_pick_card_dialog_context_ptr->color_filter = PICK_CARD_COLOR_BLACK;
        }
        else if (IsDlgButtonChecked(hwnd, PICK_CARD_COLOR_WHITE_ID) != 0)
        {
          g_pick_card_dialog_context_ptr->color_filter = PICK_CARD_COLOR_WHITE;
        }
        else if (IsDlgButtonChecked(hwnd, PICK_CARD_COLOR_BLUE_ID) != 0)
        {
          g_pick_card_dialog_context_ptr->color_filter = PICK_CARD_COLOR_BLUE;
        }
        else if (IsDlgButtonChecked(hwnd, PICK_CARD_COLOR_RED_ID) != 0)
        {
          g_pick_card_dialog_context_ptr->color_filter = PICK_CARD_COLOR_RED;
        }
        else if (IsDlgButtonChecked(hwnd, PICK_CARD_COLOR_GREEN_ID) != 0)
        {
          g_pick_card_dialog_context_ptr->color_filter = PICK_CARD_COLOR_GREEN;
        }
        else if (IsDlgButtonChecked(hwnd, PICK_CARD_COLOR_GOLD_ID) != 0)
        {
          g_pick_card_dialog_context_ptr->color_filter = PICK_CARD_COLOR_GOLD;
        }

        populate_pick_card_list(GetDlgItem(hwnd, PICK_CARD_LISTBOX_ID),
                                g_pick_card_dialog_context_ptr->color_filter,
                                g_pick_card_dialog_context_ptr->type_filter);
        SetFocus(GetDlgItem(hwnd, PICK_CARD_LISTBOX_ID));
      }
      break;

    case PICK_CARD_TYPE_CREATURE_ID:
    case PICK_CARD_TYPE_ARTIFACT_ID:
    case PICK_CARD_TYPE_ENCHANTMENT_ID:
    case PICK_CARD_TYPE_SORCERY_ID:
    case PICK_CARD_TYPE_INSTANT_ID:
    case PICK_CARD_TYPE_LAND_ID:
      if (HIWORD(wparam) == 0)
      {
        g_pick_card_dialog_context_ptr->type_filter = 0;
        if (IsDlgButtonChecked(hwnd, PICK_CARD_TYPE_LAND_ID) != 0)
        {
          g_pick_card_dialog_context_ptr->type_filter |= PICK_CARD_TYPE_LAND;
        }
        if (IsDlgButtonChecked(hwnd, PICK_CARD_TYPE_CREATURE_ID) != 0)
        {
          g_pick_card_dialog_context_ptr->type_filter |= PICK_CARD_TYPE_CREATURE;
        }
        if (IsDlgButtonChecked(hwnd, PICK_CARD_TYPE_ENCHANTMENT_ID) != 0)
        {
          g_pick_card_dialog_context_ptr->type_filter |= PICK_CARD_TYPE_ENCHANTMENT;
        }
        if (IsDlgButtonChecked(hwnd, PICK_CARD_TYPE_SORCERY_ID) != 0)
        {
          g_pick_card_dialog_context_ptr->type_filter |= PICK_CARD_TYPE_SORCERY;
        }
        if (IsDlgButtonChecked(hwnd, PICK_CARD_TYPE_INSTANT_ID) != 0)
        {
          g_pick_card_dialog_context_ptr->type_filter |= PICK_CARD_TYPE_INSTANT | PICK_CARD_TYPE_INTERRUPT;
        }
        if (IsDlgButtonChecked(hwnd, PICK_CARD_TYPE_ARTIFACT_ID) != 0)
        {
          g_pick_card_dialog_context_ptr->type_filter |= PICK_CARD_TYPE_ARTIFACT;
        }

        populate_pick_card_list(GetDlgItem(hwnd, PICK_CARD_LISTBOX_ID),
                                g_pick_card_dialog_context_ptr->color_filter,
                                g_pick_card_dialog_context_ptr->type_filter);
        SetFocus(GetDlgItem(hwnd, PICK_CARD_LISTBOX_ID));

        if ((wparam & 0xffff) == PICK_CARD_TYPE_LAND_ID || (wparam & 0xffff) == PICK_CARD_TYPE_ARTIFACT_ID)
        {
          s.show_color_filters = 0;
        }
        else
        {
          s.show_color_filters = 1;
        }
        ShowWindow(GetDlgItem(hwnd, PICK_CARD_COLOR_BLACK_ID), s.show_color_filters);
        ShowWindow(GetDlgItem(hwnd, PICK_CARD_COLOR_BLUE_ID), s.show_color_filters);
        ShowWindow(GetDlgItem(hwnd, PICK_CARD_COLOR_GREEN_ID), s.show_color_filters);
        ShowWindow(GetDlgItem(hwnd, PICK_CARD_COLOR_RED_ID), s.show_color_filters);
        ShowWindow(GetDlgItem(hwnd, PICK_CARD_COLOR_WHITE_ID), s.show_color_filters);
        ShowWindow(GetDlgItem(hwnd, PICK_CARD_COLOR_GOLD_ID), s.show_color_filters);
      }
      break;

    case IDOK:
      s.selected_index = SendDlgItemMessageA(hwnd, PICK_CARD_LISTBOX_ID, LB_GETCURSEL, 0, 0);
      s.selected_card = SendDlgItemMessageA(hwnd, PICK_CARD_LISTBOX_ID, LB_GETITEMDATA, s.selected_index, 0);
      EndDialog(hwnd, s.selected_card);
      break;

    case IDCANCEL:
      EndDialog(hwnd, -1);
      break;

    default:
      break;
    }
    return 1;

  case 0x30f:
  case 0x310:
  case 0x311:
    return handle_button_palette_message((int)hwnd, msg, (int)wparam, (int)lparam);

  default:
    return 0;
  }
}

// FUNCTION: MAGIC 0x00506240
// FUNCTION: SHANDALAR 0x00411e30
int pick_internal_card_from_list_dialog(char *prompt, int initial_card_id, int filter)
{
  struct
  {
    int dialog_result;
    int card_index;
  } s;

  g_pick_card_dialog_context.prompt = prompt;
  if (initial_card_id != -1)
  {
    g_pick_card_dialog_context.color_filter = initial_card_id;
  }
  if (filter != -1)
  {
    g_pick_card_dialog_context.type_filter = filter;
  }

  s.dialog_result = DialogBoxParamA(g_app_instance,
                                    (LPCSTR)0xe0,
                                    g_duel_window_hwnd,
                                    dlgproc_pick_card_from_list,
                                    (LPARAM)&g_pick_card_dialog_context);
  if (s.dialog_result == -1)
  {
    return -1;
  }

  for (s.card_index = 0; s.card_index < g_card_count; ++s.card_index)
  {
    if (global_cards_data[s.card_index].id == s.dialog_result)
    {
      return s.card_index;
    }
  }

  return -1;
}

// FUNCTION: MAGIC 0x004649ef
void show_opponent_library_window(int unused_color)
{
  (void)unused_color;
#ifdef SHANDALAR
  SelectAdventureListCardIndex(active_player, global_library[1], 500, gs_showlibrary_text_0074bcc0.accept_keys, 0, &g_showlibrary_menu_selection);
#else
  show_deck(active_player, global_library[1], 500, text_lines, 0, "");
#endif
}

// FUNCTION: MAGIC 0x00464a28
// FUNCTION: SHANDALAR 0x005224d9
void show_player_library_window(int unused_color)
{
  (void)unused_color;
#ifdef SHANDALAR
  SelectAdventureListCardIndex(active_player, global_library[0], 500, gs_showlibrary_text_0074bcc0.title, 0, &g_showlibrary_menu_selection);
#else
  show_deck(active_player, global_library[0], 500, text_lines, 0, "");
#endif
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
int GetCardRarity(int internal_card_id)
{
  struct
  {
    char rarity_str[32];
    int rarity;
  } s;

  if (((global_cards_data[internal_card_id].extra_ability & 0x180U) != 0) || (global_cards_data[internal_card_id].expansion == '@'))
  {
    global_cards_data[internal_card_id].rarity = 4;
  }

  if ((signed char)global_cards_data[internal_card_id].rarity != -1)
  {
    s.rarity = (int)(signed char)global_cards_data[internal_card_id].rarity;

    return s.rarity;
  }

  ReadCsvFieldByCsvid(s.rarity_str, global_cards_data[internal_card_id].id, 9, "info.csv");

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

  global_cards_data[internal_card_id].rarity = (unsigned char)s.rarity;

  return s.rarity;
}

// FUNCTION: MAGIC 0x004dd413
// FUNCTION: SHANDALAR 0x005440c4
int get_primary_color_from_duel_deck(int player)
{
  struct
  {
    int white_count;      // ebp - 0x2c
    int card_id;          // ebp - 0x28
    int red_count;        // ebp - 0x24
    int card_count;       // ebp - 0x20
    color_test_t color;   // ebp - 0x1c
    int card_index;       // ebp - 0x18
    int internal_card_id; // ebp - 0x14
    int result;           // ebp - 0x10
    int green_count;      // ebp - 0xc
    int blue_count;       // ebp - 0x8
    int black_count;      // ebp - 0x4
  } s;

  s.black_count = s.white_count = s.green_count = s.red_count = s.blue_count = 0;

  if (player == -1)
  {
    for (s.card_index = 0; s.card_index < 500; s.card_index++)
    {
      if ((deck[s.card_index] != -1) && ((deck[s.card_index] & 0xc000) == 0))
      {
        s.internal_card_id = deck[s.card_index] & 0xfff;
        s.color = global_cards_data[s.internal_card_id].color;
        s.card_count = 1;
        if ((s.color & COLOR_TEST_BLACK) != 0)
        {
          s.black_count += s.card_count;
        }
        if ((s.color & COLOR_TEST_WHITE) != 0)
        {
          s.white_count += s.card_count;
        }
        if ((s.color & COLOR_TEST_GREEN) != 0)
        {
          s.green_count += s.card_count;
        }
        if ((s.color & COLOR_TEST_RED) != 0)
        {
          s.red_count += s.card_count;
        }
        if ((s.color & COLOR_TEST_BLUE) != 0)
        {
          s.blue_count += s.card_count;
        }
      }
    }
  }
  else
  {
    for (s.card_index = 0; s.card_index < 200; s.card_index++)
    {
      s.card_id = initial_library[player][s.card_index].csvid;
      s.card_count = initial_library[player][s.card_index].numcards;
      if ((s.card_id != -1) && (s.card_count != 0))
      {
        s.internal_card_id = CardTypeFromID(s.card_id);
        s.color = global_cards_data[s.internal_card_id].color;
        if ((s.color & COLOR_TEST_BLACK) != 0)
        {
          s.black_count += s.card_count;
        }
        if ((s.color & COLOR_TEST_WHITE) != 0)
        {
          s.white_count += s.card_count;
        }
        if ((s.color & COLOR_TEST_GREEN) != 0)
        {
          s.green_count += s.card_count;
        }
        if ((s.color & COLOR_TEST_RED) != 0)
        {
          s.red_count += s.card_count;
        }
        if ((s.color & COLOR_TEST_BLUE) != 0)
        {
          s.blue_count += s.card_count;
        }
      }
    }
  }

  s.result = COLOR_COLORLESS;
  if ((((s.black_count >= s.white_count) && (s.black_count >= s.green_count)) &&
       (s.black_count >= s.red_count)) &&
      (s.black_count >= s.blue_count))
  {
    s.result = COLOR_BLACK;
  }
  else if (((s.white_count >= s.black_count) && (s.white_count >= s.green_count)) &&
           ((s.white_count >= s.red_count) && (s.white_count >= s.blue_count)))
  {
    s.result = COLOR_WHITE;
  }
  else if (((s.green_count >= s.black_count) && (s.green_count >= s.white_count)) &&
           ((s.green_count >= s.red_count) && (s.green_count >= s.blue_count)))
  {
    s.result = COLOR_GREEN;
  }
  else if ((((s.red_count >= s.black_count) && (s.red_count >= s.green_count)) &&
            (s.red_count >= s.white_count)) &&
           (s.red_count >= s.blue_count))
  {
    s.result = COLOR_RED;
  }
  else if (((s.blue_count >= s.black_count) && (s.blue_count >= s.green_count)) &&
           ((s.blue_count >= s.red_count) && (s.blue_count >= s.white_count)))
  {
    s.result = COLOR_BLUE;
  }
  return s.result;
}

// FUNCTION: MAGIC 0x004dd719
// FUNCTION: SHANDALAR 0x005443ca
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
    target_selection_result_t *posted_action_result;
    int selected_card;
    int selected_player;
    POINT cursor_pos;
    unsigned int hit_test;
    HWND cursor_window;
    target_selection_request_t *action_request;
    MSG modal_msg;
    DWORD thread_exit_code;
    target_selection_result_t *action_result;
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
    s.action_request = (target_selection_request_t *)wparam;
    s.action_result = (target_selection_result_t *)lparam;
    g_duel_modal_action_active = 1;
    memcpy(&g_duel_action_request_copy, s.action_request, sizeof(*s.action_request));
    GetCursorPos(&s.cursor_pos);
    s.cursor_window = WindowFromPoint(s.cursor_pos);
    s.hit_test = SendMessageA(s.cursor_window, WM_NCHITTEST, 0, MAKELONG(s.cursor_pos.x, s.cursor_pos.y));
    SendMessageA(s.cursor_window, WM_SETCURSOR, (WPARAM)s.cursor_window, (s.hit_test & 0xffff) | 0x2000000);
    set_player_directive_value(1, s.action_request->allow_ai_player);
    set_player_directive_value(0, s.action_request->allow_human_player);
    set_duel_prompt_context(g_duel_prompt_context_hwnd, s.action_request->prompt, s.action_request->allow_cancel);
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
        s.action_result->selection_code = -5;
        s.action_result->target_player = -1;
        s.action_result->target_card = -1;
        if (s.action_result->selection_code == 0)
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
          s.posted_action_result = (target_selection_result_t *)s.modal_msg.lParam;
          s.done = 1;
          s.action_result_ok = (unsigned int)(s.posted_action_result->selection_code == 0);
          memcpy(s.action_result, s.posted_action_result, sizeof(*s.action_result));
        }
        else if (s.modal_msg.message == WM_QUIT)
        {
          PostQuitMessage(s.modal_msg.wParam);
          s.done = 1;
          s.action_result_ok = 0;
          s.action_result->selection_code = -2;
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
        strcpy(s.replay_prompt, "Congratulations!");
      }
      else if (s.duel_result == (HWND)0)
      {
        strcpy(s.replay_prompt, "Too bad");
      }
      else if (s.duel_result == (HWND)-1)
      {
        strcpy(s.replay_prompt, "Oh well...");
      }
      else
      {
        s.has_replay_prompt = 0;
      }
      if (s.has_replay_prompt != 0)
      {
        strcat(s.replay_prompt, "\n\nWant to play again?");
        if (MessageBoxA(hwnd, s.replay_prompt, "End of duel", MB_YESNO) == IDYES)
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
        g_last_duel_enemy_primary_color = get_primary_color_from_duel_deck(opponent_initial_library_index);
        g_last_duel_player_primary_color = get_primary_color_from_duel_deck(g_selected_wizard_color);
      }
      else if ((g_duel_mode_flags & 1) != 0)
      {
        g_last_duel_enemy_primary_color = get_primary_color_from_duel_deck(opponent_initial_library_index);
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
      if (g_duel_interface_options.player_territory_color == -1)
      {
        s.backdrop_player_color = g_last_duel_player_primary_color;
      }
      else
      {
        s.backdrop_player_color = g_duel_interface_options.player_territory_color;
      }
      s.backdrop_player_variant = g_duel_interface_options.player_territory_type;
    }
    else
    {
      s.backdrop_enemy_color = g_last_duel_enemy_primary_color;
      s.player_backdrop_result = rand() % 3;
      if (g_duel_interface_options.player_territory_color == -1)
      {
        s.backdrop_player_color = g_last_duel_player_primary_color;
      }
      else
      {
        s.backdrop_player_color = g_duel_interface_options.player_territory_color;
      }
      s.backdrop_player_variant = g_duel_interface_options.player_territory_type;
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
      if (g_duel_cheats_state != 0)
      {
        g_duel_cheats_state = 0;
      }
      else
      {
        g_duel_cheats_state = 1;
      }
#ifndef _DEBUG
      // Immediately disables debug menu normally
      if (DAT_008ce504 == 0)
      {
        g_duel_cheats_state = 0;
      }
#endif
      break;

    case 0x25c:
      if (g_duel_cheats_state != 0)
      {
        g_duel_surface_reset_state = (g_duel_surface_reset_state == 0);
        resize_duel_hand_window(g_duel_life_window_hwnd);
      }
      break;

    case 0x25d:
      if (g_duel_cheats_state != 0)
      {
        show_opponent_library_window(opponent_initial_library_index);
      }
      break;

    case 0x25e:
      if (g_duel_cheats_state != 0)
      {
        show_player_library_window(opponent_initial_library_index);
      }
      break;

    case 0x26b:
    case 0x26c:
      if (g_duel_cheats_state != 0)
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
        s.command_selected_card = (int)(char)g_duel_phase_stop_settings[current_player].phase_flags[current_phase];
        g_duel_phase_stop_settings[current_player].phase_flags[current_phase] =
            (unsigned char)((int)(char)g_duel_phase_stop_settings[current_player].phase_flags[current_phase] & ~PHASE_STOP_ENABLED);
        s.command_saved_phase_flags = add_card_to_hand(s.player_backdrop_color, s.enemy_backdrop_result);
        if (s.command_saved_phase_flags != -1)
        {
          process_card_enters_play(s.player_backdrop_color, s.command_saved_phase_flags);
        }
        g_duel_phase_stop_settings[current_player].phase_flags[current_phase] = (unsigned char)s.command_selected_card;
        notify_duel_action(0, 0xff);
      }
      break;

    case 0x26d:
    case 0x26e:
      if (g_duel_cheats_state != 0)
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
        duel_summary.hand_counts[s.player_backdrop_color] += 1;
        if (((g_duel_network_flags & 2) != 0) && (active_player == s.player_backdrop_color))
        {
          g_network_result_packet_type = 0x12;
          g_network_result_value = s.enemy_backdrop_result;
          TENTATIVE_send_network_result(active_player, 0x12);
        }
        notify_duel_action(0, 0xff);
      }
      break;

    case 0x269:
    case 0x26a:
      if (g_duel_cheats_state != 0)
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
      if (g_duel_cheats_state != 0)
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
      if (g_duel_cheats_state != 0)
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
      if (g_duel_cheats_state != 0)
      {
        life[0] = 0;
        life[1] = 0;
        SendMessageA(g_duel_life_status_window_1_hwnd, 0x432, 0, 0);
        SendMessageA(g_duel_life_status_window_2_hwnd, 0x432, 0, 0);
      }
      break;

    case 0x271:
      if (g_duel_cheats_state != 0)
      {
        ShowWindow(g_duel_palette_window_hwnd, SW_SHOW);
      }
      break;

    case 0x275:
      break;

    case 0x276:
      if (g_duel_cheats_state != 0)
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
      if (g_duel_cheats_state != 0)
      {
        ShowWindow(g_duel_toggleable_status_window_hwnd,
                   -((IsWindowVisible(g_duel_toggleable_status_window_hwnd) == 0)) & SW_SHOW);
      }
      break;

    case 0x273:
      if (g_duel_cheats_state != 0)
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
      if (g_duel_cheats_state != 0)
      {
        g_duel_network_state = 0;
      }
      break;

    case 0x272:
      if (g_duel_cheats_state != 0)
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
    return handle_button_palette_message((int)hwnd, msg, (int)wparam, lparam);

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
