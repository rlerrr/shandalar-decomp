#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "cardartlib/src/assert.h"
#include "defs.h"
#include "magic/src/duel_engine.h"
#include "magic/src/game_support.h"
#include "magic/src/global_duel_ui_ids.h"
#include "magic/src/global_other.h"
#include "magic/src/global_state.h"
#include "magic/src/global_strings.h"
#include "magic/src/network.h"
#include "magic/src/shared_startup.h"
#ifdef SHANDALAR
#include "deckdll/src/magsnd.h"
#endif

extern int g_life[2];
int DrawRandomCardFromInitialLibrary(int library_index);
int add_card_to_hand(int player, int internal_card_id);
int AddCardToDeckSorted(int card_id);
int GetCardRarity(int card_id);
void AddJournalEntry(int entry_type, int entry_arg);
void TENTATIVE_reassess_all_cards(int view_player, int present_after_draw);
void append_to_trace_txt(char *text);
void AddCardToCLPacket(int card_in_packet);
int GetCardFromCLPacket(int packet_index);
void TENTATIVE_savegame(int autosave_slot);
void reset_recorded_actions(void);
void update_phase_display(int player, phase_t phase);
void layout_attack_phase_window(HWND hwnd);
int update_duel_selection_display_if_human(int player, int phase);
int player_can_stop_at_phase(int player, phase_t phase);
void reset_trigger_dispatch_state(void);
int contains_player_card_pair(int *card_pairs, int card_pair_count, int player, int card);
int can_activate_mana_source_for_stop_prompt(int player, int card);
int activate_mana_source_card(int player, int card);
int prompt_stop_phase_anyway(phase_t phase);
void resolve_mana_burn(void);
int show_ai_action_log_dialog(int use_saved_actions, int score);
int get_ai_search_elapsed_time(void);
void clear_all_upkeep_flags(void);
int allow_response(int response_player, int phase, char *prompt, int event_code);
int dispatch_trigger(int player, trigger_t trig, const char *prompt, int TENTATIVE_allow_response);
int dispatch_trigger_twice_once_with_each_player_as_reason(int reason_for_trig, trigger_t trig, const char *prompt, int a4);
void start_ai_decision_search(int decision_code, int time_scale);
void __stdcall reset_ai_search_trial_state(void);
int reset_empty_card_original_ids(void);
void compact_timestamp_slots(void);
int reset_stack_tracking_state(void);
void rebuild_battlefield_summary(void);
extern int g_saved_stop_phase_player;
extern int g_response_prompt_enabled;
extern int g_face_ui_refresh_flags;
extern HWND g_duel_opponent_battlefield_window;
extern HWND g_duel_player_battlefield_window;
extern int g_duel_selected_player_card;
extern int g_duel_selected_opponent_card;
extern int g_duel_selection_pending;
extern int g_duel_current_selection_forced;

// GLOBAL: SHANDALAR 0x007be748
// GLOBAL: MAGIC 0x007a79b8
int g_manalink_is_host;

int init_turn(int player);
int TENTATIVE_start_turn(int player);
int untap_phase_exe(unsigned int player);
int draw_phase(unsigned int player);
int main_phase(unsigned int player, int phase_mode, int *phase_value);
int discard_phase(unsigned int player, int phase_mode);
void cleanup_phase(unsigned int player);
int restore_duel_turn_resume_state(void);
int upkeep_phase(unsigned int player);
int end_turn_phase(unsigned int player);
int ai_decision_phase(unsigned int player, int *next_state, int *phase_mode, int *phase_value);
#ifndef SHANDALAR
extern HANDLE global_mutex_GameInit;
#endif
#ifdef SHANDALAR
extern int g_world_location_music_active;
extern int g_random_seed_initialized;
extern int g_random_seed_tick_value;
int single_color_test_bit_to_color_t(int color_mask);
void ShowMouseCursorNested(void);
void HideMouseCursorNested(void);
#endif
void refresh_ai_random_table(void);

// GLOBAL: MAGIC 0x009251d4
// GLOBAL: SHANDALAR 0x00939304
int g_duel_result;

// GLOBAL: MAGIC 0x00896690
// GLOBAL: SHANDALAR 0x008aa890
char g_duel_winner;

// GLOBAL: MAGIC 0x00712544
// GLOBAL: SHANDALAR 0x007894f4
int g_reveal_all_world_info;

// GLOBAL: MAGIC 0x00743098
// GLOBAL: SHANDALAR 0x00748430
int g_ai_response_state;

// GLOBAL: MAGIC 0x0091c0ec
// GLOBAL: SHANDALAR 0x0093022c
int g_duel_action_pending;

// GLOBAL: MAGIC 0x0069c490
// GLOBAL: SHANDALAR 0x005b7f58
char g_duel_save_extension[0x10];

// GLOBAL: MAGIC 0x007abc40
// GLOBAL: SHANDALAR 0x007bfe40
char g_duel_run_save_extensions[4][10];

// FUNCTION: MAGIC 0x004d2a09
// FUNCTION: SHANDALAR 0x004f87d9
void copy_autosave_to_save_file(LPCSTR save_path)
{
  char autosave_path[0x108];

  if (g_duel_run_mode == 0)
  {
    strcpy(autosave_path, global_savegame_path);
    strcat(autosave_path, "/AUTOSAVE");
    strcat(autosave_path, g_duel_save_extension + 1);
    CopyFileA(autosave_path, save_path, 0);
  }
  else
  {
    sprintf(autosave_path, "%s\\AUTOSAVE.%s", global_savegame_path,
            g_duel_run_save_extensions[g_duel_run_mode]);
    CopyFileA(autosave_path, save_path, 0);
  }
}

// FUNCTION: MAGIC 0x00464b6f
// FUNCTION: SHANDALAR 0x00562893
void StopWorldLocationMusic(void)
{
#ifdef SHANDALAR
  if (g_world_location_music_active != 0)
  {
    sound_stop(0x10);
  }
  g_world_location_music_active = 0;
#endif
}

// FUNCTION: MAGIC 0x00464bb0
// FUNCTION: SHANDALAR 0x00522660
void FUN_00464bb0(void)
{
  g_face_ui_refresh_flags = 0;
}

#ifdef SHANDALAR
// GLOBAL: SHANDALAR 0x00604428
int g_ai_random_table[100];

// FUNCTION: SHANDALAR 0x0052261f
void refresh_ai_random_table(void)
{
  int i;

  for (i = 0; i < 100; i++)
  {
    g_ai_random_table[i] = rand();
  }
  FUN_00464bb0();
}
#endif

// FUNCTION: MAGIC 0x0043de00
// FUNCTION: SHANDALAR 0x00409680
void reset_duel_globals(void)
{
  int card;
  int player;

  g_active_player = 0;
  g_other_player = 1;
  for (card = g_card_count; card < g_card_count + 16; ++card)
  {
    global_cards_data[card].id = -1;
  }

  for (player = 0; player < 2; ++player)
  {
    g_poison_counters[player] = 0;
    for (card = 0; card <= 7; ++card)
    {
      g_raw_mana_available[player][card] = 0;
      g_basiclandtypes_controlled[player][card] = g_raw_mana_available[player][card];
      g_special_mana_pool[player][card] = g_basiclandtypes_controlled[player][card];
      g_raw_mana_available_hex[player][0] = -1;
      g_mana_color_conversions[player][0] = -1;
      g_mana_pool_colorless_components[player] = 0;
    }
  }

  global_stack_cards[0].player = -1;
  for (card = 0; card < 16; ++card)
  {
    if ((char)global_cards_data[g_card_count + card].secret == -1)
    {
      global_cards_data[g_card_count + card].id = -1;
    }
  }

  g_land_can_be_played = LCBP_NONE;
  g_max_x_value = -1;
  g_duel_state_0077784c = g_max_x_value;
  g_stop_phase = g_duel_state_0077784c;
  g_previous_stop_phase_player = -1;
  g_stop_phase_player = g_previous_stop_phase_player;
  g_previous_stop_phase = g_stop_phase_player;
  g_activation_scan_flags = 0;
  g_duel_state_00791418 = 0;
  g_saved_stop_phase_player = -1;
  g_saved_stop_phase = g_saved_stop_phase_player;
  g_duel_state_007a7d78 = 0;
  g_battlefield_ui_flags = 0;
  g_response_prompt_enabled = 0;
  g_current_action_event_code = -1;
  if (g_shandalar_difficulty == 0)
  {
    g_duel_state_00925ac4 |= 2;
  }
  else
  {
    g_duel_state_00925ac4 &= -3;
  }

  g_ai_search_flags = 0;
  g_response_card_type_mask = 0x30;
  g_response_action_mask = -1;
  g_duel_state_0091b2c4 = -1;
  g_produced_mana_card = -1;
  g_produced_mana_color_valid = -1;
  g_produced_mana_color = -1;
  g_duel_network_state = 0;
  g_duel_initialized = 1;
  g_duel_action_pending = 0;
  g_phase_stop_suppressed = 0;
  g_phase_was_skipped = 0;
  g_attacking_creature_count = 0;
  g_duel_extra_turn_player = -1;
  g_duel_state_0093a848 = 0;
  g_response_window_depth = 1;
  g_trigger_dispatch_depth = 0;
  g_max_trigger_dispatch_depth = 0;
  g_nested_trigger_depth = 0;
  for (card = 0; card <= 7; ++card)
  {
    g_mana_charge[card] = 0;
  }

  g_max_x_value = -1;
  g_current_spell_player = -1;
  g_current_spell_card = -1;
  g_duel_result = 0;
  g_player_special_effect_flags[g_other_player] = 0;
  g_player_special_effect_flags[g_active_player] = g_player_special_effect_flags[g_other_player];
  g_regenerate_or_graveyard_triggers_in_progress = 0;
}

// FUNCTION: MAGIC 0x004458d4
// FUNCTION: SHANDALAR 0x0041116d
int reset_timestamp_players(void)
{
  int i;
  for (i = 0; i < 500; i++)
  {
    TENTATIVE_timestamp_player[i] = -1;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0044b56a
// FUNCTION: SHANDALAR 0x00403eeb
int check_duel_finished(void)
{
  int player_died;
  int result;

  if ((g_life[0] <= 0) || (g_life[1] <= 0))
  {
    player_died = 1;
  }
  else if ((g_poison_counters[0] >= 10) || (g_poison_counters[1] >= 10))
  {
    player_died = 1;
  }
  else
  {
    player_died = 0;
  }
  if (player_died)
  {
    if (g_duel_ai_mode_state == 1)
    {
      if (g_poison_counters[0] >= 10)
      {
        g_life[0] = -99;
      }
      if (g_poison_counters[1] >= 10)
      {
        g_life[1] = -99;
      }
      result = 0;
    }
    else
    {
      TENTATIVE_reassess_all_cards(0, 0xff);
      result = 1;
    }
  }
  else
  {
    result = 0;
  }
  return result;
}

// FUNCTION: MAGIC 0x0044a7e0
// FUNCTION: SHANDALAR 0x00403160
void run_duel_turn(unsigned int player)
{
  struct
  {
    char trace[100];
    int next_state;
    int phase_value;
    int phase_mode;
  } s;

  s.phase_mode = 0;
  s.next_state = 0;
  if (TRACE_ENABLED)
  {
    append_to_trace_txt("___________________________________________________________________________\n");
    if (player != 0)
    {
      sprintf(s.trace, "Turn #%d. We are the non-phasing player(%d).\n\n", g_duel_result, player);
    }
    else
    {
      sprintf(s.trace, "Turn #%d. We are the phasing player(%d).\n\n", g_duel_result, player);
    }
    append_to_trace_txt(s.trace);
  }

  while (s.next_state != -1)
  {
    switch (s.next_state)
    {
    case 0:
      init_turn(player);
      s.next_state = restore_duel_turn_resume_state();
      if (s.next_state != 10)
      {
        break;
      }
    case 2:
      if (TENTATIVE_start_turn(player) != 0)
      {
        s.next_state = -1;
        break;
      }
    case 3:
      if (untap_phase_exe(player) != 0)
      {
        s.next_state = -1;
        break;
      }
    case 4:
      if (upkeep_phase(player) != 0)
      {
        s.next_state = -1;
        break;
      }
    case 5:
      if (draw_phase(player) != 0)
      {
        s.next_state = -1;
        break;
      }
    case 1:
    case 6:
      if (main_phase(player, s.phase_mode, &s.phase_value) != 0)
      {
        s.next_state = -1;
        break;
      }
      s.phase_mode = 0;
    case 7:
      if (discard_phase(player, s.phase_mode) != 0)
      {
        s.next_state = -1;
        break;
      }
      s.phase_mode = 0;
    case 8:
      cleanup_phase(player);
      end_turn_phase(player);
      if (ai_decision_phase(player, &s.next_state, &s.phase_mode, &s.phase_value) != 0)
      {
        s.next_state = -1;
        break;
      }
    }
  }
}

// FUNCTION: MAGIC 0x0044bcf7
// FUNCTION: SHANDALAR 0x00404677
int restore_duel_turn_resume_state(void)
{
  int next_state = 10;
  if ((g_duel_ai_mode_state < 0) || (g_duel_use_previous_backdrop_colors != 0))
  {
    next_state = g_duel_ai_mode_state;
    g_duel_ai_mode_state = 0;
    g_duel_use_previous_backdrop_colors = 0;
    if ((next_state == -1) || (next_state == -2))
    {
      next_state = 1;
    }
    else if (next_state == -10)
    {
      TENTATIVE_reassess_all_cards(0, 0xff);
      g_stop_phase_player = g_current_player;
      g_stop_phase = g_current_phase;
      if (g_current_phase == PHASE_START)
      {
        g_stop_phase = -1;
      }
      if (g_current_phase == PHASE_CLEANUP)
      {
        g_stop_phase = PHASE_CLEANUP2;
      }
      if (g_current_phase == PHASE_START)
      {
        next_state = 2;
      }
      else if (g_current_phase == PHASE_UNTAP)
      {
        next_state = 3;
      }
      else if (g_current_phase == PHASE_UPKEEP)
      {
        next_state = 4;
      }
      else if (g_current_phase == PHASE_DRAW)
      {
        next_state = 5;
      }
      else if (g_current_phase == PHASE_MAIN1)
      {
        next_state = 6;
      }
      else if (g_current_phase == PHASE_DISCARD)
      {
        next_state = 7;
      }
      else if (g_current_phase == PHASE_CLEANUP)
      {
        next_state = 8;
      }
    }
  }
  return next_state;
}

// FUNCTION: MAGIC 0x0044b89b
// FUNCTION: SHANDALAR 0x0040421c
int init_turn(int player)
{
  struct
  {
    int trace_counter;
    char trace[100];
    int other_player;
    int card;
    int loop_player;
  } s;

  if (TRACE_ENABLED)
  {
    s.trace_counter = g_duel_trace_counter;
    g_duel_trace_counter++;
    sprintf(s.trace, "%d: Entering Init Turn.\n", s.trace_counter);
    append_to_trace_txt(s.trace);
  }
  s.other_player = 1 - player;
  g_current_player = player;
  g_ai_decision_code = -1;
  if (g_active_player == player)
  {
    g_duel_state_0077784c++;
  }
  g_duel_result++;
  for (s.card = 0; s.card < 0x97; s.card = s.card + 1)
  {
    g_damage_accumulators[s.other_player][s.card][1].amount = 0;
    g_damage_accumulators[s.other_player][s.card][0].amount = g_damage_accumulators[s.other_player][s.card][1].amount;
    g_damage_accumulators[player][s.card][1].amount = g_damage_accumulators[s.other_player][s.card][0].amount;
    g_damage_accumulators[player][s.card][0].amount = g_damage_accumulators[player][s.card][1].amount;
    g_damage_accumulators[s.other_player][s.card][1].unknown_c = -1;
    g_damage_accumulators[s.other_player][s.card][0].unknown_c = g_damage_accumulators[s.other_player][s.card][1].unknown_c;
    g_damage_accumulators[s.other_player][s.card][1].unknown_8 = (char)g_damage_accumulators[s.other_player][s.card][0].unknown_c;
    g_damage_accumulators[s.other_player][s.card][0].unknown_8 = g_damage_accumulators[s.other_player][s.card][1].unknown_8;
    g_damage_accumulators[player][s.card][1].unknown_c = g_damage_accumulators[s.other_player][s.card][0].unknown_8;
    g_damage_accumulators[player][s.card][0].unknown_c = g_damage_accumulators[player][s.card][1].unknown_c;
    g_damage_accumulators[player][s.card][1].unknown_8 = (char)g_damage_accumulators[player][s.card][0].unknown_c;
    g_damage_accumulators[player][s.card][0].unknown_8 = g_damage_accumulators[player][s.card][1].unknown_8;
  }
  reset_empty_card_original_ids();
  compact_timestamp_slots();
  for (s.card = 0; s.card < 0x26; s.card = s.card + 1)
  {
    g_duel_phase_stop_settings[0].phase_flags[s.card] &= (PHASE_STOP_ENABLED | PHASE_STOP_OPPONENT);
    g_duel_phase_stop_settings[1].phase_flags[s.card] &= (PHASE_STOP_ENABLED | PHASE_STOP_OPPONENT);
  }
#ifndef SHANDALAR
  StopWorldLocationMusic();
#else
  refresh_ai_random_table();
#endif
  reassess_all_cards_and_mana();
  reset_stack_tracking_state();
  rebuild_battlefield_summary();
  for (s.loop_player = 0; s.loop_player < 2; s.loop_player = s.loop_player + 1)
  {
    for (s.card = 0; s.card < 0x96; s.card = s.card + 1)
    {
      if (global_card_instances[s.loop_player][s.card].internal_card_id != -1)
      {
        g_active_cards_count[s.loop_player] = s.card + 1;
      }
    }
  }
  return 0;
}

// FUNCTION: MAGIC 0x0044be6f
// FUNCTION: SHANDALAR 0x004047ef
int TENTATIVE_start_turn(int player)
{
  struct
  {
    int trace_counter;
    char trace[100];
    int card;
  } s;

  if (TRACE_ENABLED)
  {
    s.trace_counter = g_duel_trace_counter;
    g_duel_trace_counter++;
    sprintf(s.trace, "%d: Entering Start Turn.\n", s.trace_counter);
    append_to_trace_txt(s.trace);
  }
  if ((g_duel_ai_mode_state != 1) && ((g_duel_network_flags & 2) == 0))
  {
    TENTATIVE_savegame(0);
  }
  if ((g_land_can_be_played & LCBP_SKIP_TURN) != 0)
  {
    g_land_can_be_played &= ~LCBP_SKIP_TURN;
    C_dispatch_event_raw(0x22);
    return 1;
  }
  reset_recorded_actions();
  g_current_phase = PHASE_START;
  update_phase_display(player, g_current_phase);
  C_dispatch_event_raw(0x6a);
  if ((g_land_can_be_played & LCBP_SKIP_TURN) != 0)
  {
    g_land_can_be_played &= ~LCBP_SKIP_TURN;
    C_dispatch_event_raw(0x22);
    return 1;
  }
  g_duel_winner = 0;
  g_attacking_creature_count = 0;
  g_land_can_be_played &= ~LCBP_FLAGS_CLEARED_EACH_TURN;
  for (s.card = 0; s.card < g_active_cards_count[player]; s.card = s.card + 1)
  {
    global_card_instances[player][s.card].state &= ~(STATE_ATTACKING | STATE_BLOCKING | STATE_PLAYED_FROM_HAND | STATE_UNKNOWN8000 | STATE_SUMMONSICK_NOATTACK | STATE_SUMMONSICK_NOTAP);
  }
  TENTATIVE_reassess_all_cards(0, 0xff);
  return 0;
}

// FUNCTION: MAGIC 0x004b51b9
// FUNCTION: SHANDALAR 0x0056b9c2
int reset_empty_card_original_ids(void)
{
  int player;
  int card;

  for (player = 0; player < 2; player = player + 1)
  {
    for (card = 0; card < 0x96; card = card + 1)
    {
      if (global_card_instances[player][card].internal_card_id == -1)
      {
        global_card_instances[player][card].original_internal_card_id = -1;
      }
    }
  }
  return 0;
}

// FUNCTION: MAGIC 0x004459b5
// FUNCTION: SHANDALAR 0x0041124e
void compact_timestamp_slots(void)
{
  int next_slot;
  int slot;

  for (slot = 0; slot < 500; slot = slot + 1)
  {
    while ((TENTATIVE_timestamp_player[slot] != -1) &&
           ((global_card_instances[TENTATIVE_timestamp_player[slot]][TENTATIVE_timestamp_card[slot]].internal_card_id == -1) ||
            ((int)global_card_instances[TENTATIVE_timestamp_player[slot]][TENTATIVE_timestamp_card[slot]].timestamp != slot)))
    {
      next_slot = slot;
      while (next_slot + 1 < 500)
      {
        next_slot++;
        TENTATIVE_timestamp_player[next_slot - 1] = TENTATIVE_timestamp_player[next_slot];
        TENTATIVE_timestamp_card[next_slot - 1] = TENTATIVE_timestamp_card[next_slot];
        if ((int)global_card_instances[TENTATIVE_timestamp_player[next_slot]][TENTATIVE_timestamp_card[next_slot]].timestamp == next_slot)
        {
          global_card_instances[TENTATIVE_timestamp_player[next_slot]][TENTATIVE_timestamp_card[next_slot]].timestamp--;
        }
      }
      TENTATIVE_timestamp_player[499] = -1;
    }
  }
}

// FUNCTION: MAGIC 0x0044386c
// FUNCTION: SHANDALAR 0x0040f106
int reset_stack_tracking_state(void)
{
  g_stack_size = 0;
  global_stack_cards[g_stack_size].player = -1;
  return 0;
}

// FUNCTION: MAGIC 0x00442f0c
// FUNCTION: SHANDALAR 0x0040e7a6
void rebuild_battlefield_summary(void)
{
  struct
  {
    int toughness_color;
    int power_color;
    int toughness;
    int card_color;
    int power;
    int card;
    int player;
    int internal_card_id;
  } s;

  for (s.card = 0; s.card <= 7; s.card = s.card + 1)
  {
    g_ai_mana_demand_by_color[1][s.card] = 0;
    g_ai_mana_demand_by_color[0][s.card] = g_ai_mana_demand_by_color[1][s.card];
    g_creature_toughness_by_color[1][s.card] = g_ai_mana_demand_by_color[0][s.card];
    g_creature_toughness_by_color[0][s.card] = g_creature_toughness_by_color[1][s.card];
    g_creature_power_by_color[1][s.card] = g_creature_toughness_by_color[0][s.card];
    g_creature_power_by_color[0][s.card] = g_creature_power_by_color[1][s.card];
  }
  g_card_types_in_play[1] = 0;
  g_card_types_in_play[0] = g_card_types_in_play[1];
  g_graveyard_card_types[1] = 0;
  g_graveyard_card_types[0] = g_graveyard_card_types[1];
  g_creature_count_summary[1] = 0;
  g_creature_count_summary[0] = g_creature_count_summary[1];
  memset(&g_duel_summary, 0, sizeof(g_duel_summary));
  g_duel_summary.life_totals[0] = g_life[0];
  g_duel_summary.life_totals[1] = g_life[1];
  for (s.player = 0; s.player < 2; s.player = s.player + 1)
  {
    g_duel_summary.hand_counts[s.player] = 0;
    for (s.card = 0; s.card < g_active_cards_count[s.player]; s.card = s.card + 1)
    {
      if (is_in_play(s.player, s.card))

      {
        s.internal_card_id = global_card_instances[s.player][s.card].internal_card_id;
        s.card_color = global_cards_data[s.internal_card_id].color;
        if ((global_cards_data[s.internal_card_id].type & TYPE_CREATURE) != 0)
        {
          s.power = C_get_abilities(s.player, s.card, EVENT_POWER, -1);
          s.toughness = C_get_abilities(s.player, s.card, EVENT_TOUGHNESS, -1);
          s.power_color = single_color_test_bit_to_color_t(s.card_color);
          g_creature_power_by_color[s.player][s.power_color] += s.power;
          g_creature_power_by_color[s.player][7] += s.power;
          s.toughness_color = single_color_test_bit_to_color_t(s.card_color);
          g_creature_toughness_by_color[s.player][s.toughness_color] += s.toughness;
          g_creature_toughness_by_color[s.player][7] += s.toughness;
          g_creature_count_summary[s.player]++;
        }
        g_card_types_in_play[s.player] |= global_cards_data[s.internal_card_id].type;
        if ((global_cards_data[s.internal_card_id].type & TYPE_CREATURE) != 0)
        {
          g_duel_summary.creature_counts[s.player]++;
        }
        if ((global_cards_data[s.internal_card_id].type & TYPE_ARTIFACT) != 0)
        {
          g_duel_summary.artifact_counts[s.player]++;
        }
        if ((global_cards_data[s.internal_card_id].type & TYPE_ENCHANTMENT) != 0)
        {
          g_duel_summary.enchantment_counts[s.player]++;
        }
      }
      else if (((global_card_instances[s.player][s.card].state & STATE_OUBLIETTED) == 0) &&
               (global_card_instances[s.player][s.card].internal_card_id != -1))
      {
        g_duel_summary.hand_counts[s.player]++;
      }
    }
    for (s.card = 0; s.card < 500; s.card = s.card + 1)
    {
      if (global_graveyard_slots[s.player][s.card] != -1)
      {
        g_graveyard_card_types[s.player] |= global_cards_data[global_graveyard_slots[s.player][s.card]].type;
      }
    }
  }
}

// FUNCTION: MAGIC 0x004d2843
// FUNCTION: SHANDALAR 0x004f8613
void TENTATIVE_savegame(int autosave_slot)
{
  struct
  {
    int saved_phase;
    char path[0x108];
  } s;

  if (g_duel_run_mode == 0)
  {
    if ((g_duel_mode_flags & 1) == 0)
    {
      s.saved_phase = g_current_phase;
      g_current_phase = autosave_slot;
      strcpy(s.path, global_savegame_path);
      strcat(s.path, "/AUTOSAVE");
      strcat(s.path, g_duel_save_extension + 1);
      save_gametype0(s.path);
      g_current_phase = s.saved_phase;
    }

    if ((g_duel_mode_flags & 1) != 0 && g_duel_cheats_state != 0)
    {
      s.saved_phase = g_current_phase;
      g_current_phase = autosave_slot;
      strcpy(s.path, global_savegame_path);
      strcat(s.path, "/SHANDSAVE");
      strcat(s.path, g_duel_save_extension + 1);
      save_gametype0(s.path);
      g_current_phase = s.saved_phase;
    }
  }
  else
  {
    s.saved_phase = g_current_phase;
    g_current_phase = autosave_slot;
    sprintf(s.path, "%s\\AUTOSAVE.%s", global_savegame_path,
            g_duel_run_save_extensions[g_duel_run_mode]);
    if (g_duel_run_mode == 1)
    {
      save_soloduel(s.path);
    }
    else if (g_duel_run_mode == 2)
    {
      save_gauntlet(s.path);
    }
    else if (g_duel_run_mode == 3)
    {
      save_sealeddeck(s.path);
    }
    g_current_phase = s.saved_phase;
  }
}

// FUNCTION: MAGIC 0x004e4e75
// FUNCTION: SHANDALAR 0x004c40e5
void reset_recorded_actions(void)
{
  g_recorded_action_count = 0;
  g_recorded_action_codes[g_recorded_action_count] = 99;
}

// FUNCTION: MAGIC 0x004432ed
// FUNCTION: SHANDALAR 0x0040eb87
void update_phase_display(int player, phase_t phase)
{
  if (g_duel_ai_mode_state != 1)
  {
    update_duel_selection_display_if_human(player, phase);
  }
  g_spell_fizzled = 0;
}

// FUNCTION: MAGIC 0x00446036
// FUNCTION: SHANDALAR 0x004118ce
void clear_all_upkeep_flags(void)
{
  int card;
  int player;

  for (player = 0; player < 2; player = player + 1)
  {
    for (card = 0; card < g_active_cards_count[player]; card = card + 1)
    {
      if (is_in_play(player, card) != 0)
      {
        global_card_instances[player][card].upkeep_flags = 0;
      }
    }
  }
}

// FUNCTION: MAGIC 0x00446590
// FUNCTION: SHANDALAR 0x00450110
int update_duel_selection_display(int player, int phase)
{
  int changed;

  changed = 0;
  KillTimer(g_duel_window_hwnd, g_duel_timer_id);
  EnterCriticalSection(&g_duel_render_lock);
  if (g_duel_selected_player_card != player ||
      g_duel_selected_opponent_card != phase)
  {
    changed |= 1;
  }
  g_duel_selected_player_card = player;
  g_duel_selected_opponent_card = phase;
  g_duel_selection_pending = g_attacking_creature_count;
  LeaveCriticalSection(&g_duel_render_lock);
  if (changed != 0)
  {
    SendMessageA(g_duel_opponent_battlefield_window, 0x432, 0, 0);
    UpdateWindow(g_duel_opponent_battlefield_window);
    SendMessageA(g_duel_player_battlefield_window, 0x432, 0, 0);
    UpdateWindow(g_duel_player_battlefield_window);
  }
  if (phase == PHASE_DECLARE_ATTACKERS && player == 1)
  {
    g_duel_current_selection_forced = 0;
  }
  if (phase == PHASE_DECLARE_ATTACKERS && g_attacking_creature_count != 0)
  {
    layout_attack_phase_window(g_duel_attack_phase_window_hwnd);
  }
  if (phase == PHASE_MAIN2)
  {
    layout_attack_phase_window(g_duel_attack_phase_window_hwnd);
  }
  return 0;
}

// FUNCTION: MAGIC 0x0044bfe9
// FUNCTION: SHANDALAR 0x00404969
int untap_phase_exe(unsigned int player)
{
  int trace_counter;
  struct
  {
    char trace[100];
    int selected_card;
    int winter_orb_card_type;
    int optional_untap_count;
    int event_result_copy;
    int must_untap_cards[100];
    int optional_untap_cards[100];
    int card;
    int must_untap_count;
    int did_skip_untap;
    int loop_player;
    int done;
  } s;

  if (TRACE_ENABLED)
  {
    trace_counter = g_duel_trace_counter;
    g_duel_trace_counter++;
    sprintf(s.trace, "%d: Entering Untap Phase.\n", trace_counter);
    append_to_trace_txt(s.trace);
  }

  s.winter_orb_card_type = CardTypeFromID(0xe9);
  if ((s.winter_orb_card_type == -1) ||
      (real_target_available(NULL, 0, player, 2, 2, 0x200, 0, 0, 0, 0, 0, 0, s.winter_orb_card_type, -1, -1, -1, 0, 0, 0) == 0))
  {
    g_current_phase = PHASE_UNTAP;
    update_phase_display(player, g_current_phase);
    g_phase_was_skipped = 0;
    g_phase_stop_suppressed = 0;
    g_phase_response_window_open = 1;
    g_ai_score_baseline = 0;

    for (s.card = 0; s.card < g_active_cards_count[player]; s.card = s.card + 1)
    {
      if ((is_in_play(player, s.card) != 0) &&
          ((global_card_instances[player][s.card].state & STATE_TAPPED) != 0))
      {
        global_card_instances[player][s.card].untap_status = 3;
        dispatch_event(player, s.card, 0x82);
      }
    }

    s.done = 0;
    while (s.done == 0)
    {
      s.must_untap_count = s.optional_untap_count = 0;
      for (s.loop_player = 0; s.loop_player < 2; s.loop_player = s.loop_player + 1)
      {
        for (s.card = 0; s.card < g_active_cards_count[s.loop_player]; s.card = s.card + 1)
        {
          if (is_in_play(s.loop_player, s.card) != 0)
          {
            g_affected_card_controller = s.loop_player;
            g_affected_card = s.card;
            g_event_result = 0;
            C_dispatch_event_raw(0x7d);
            s.event_result_copy = g_event_result;
            if (s.event_result_copy == 1)
            {
              s.optional_untap_cards[s.optional_untap_count * 2] = s.loop_player;
              s.optional_untap_cards[s.optional_untap_count * 2 + 1] = s.card;
              s.optional_untap_count++;
            }
            else if (s.event_result_copy == 2)
            {
              s.must_untap_cards[s.must_untap_count * 2] = s.loop_player;
              s.must_untap_cards[s.must_untap_count * 2 + 1] = s.card;
              s.must_untap_count++;
            }
          }
        }
      }

      if (((g_other_player == player) && ((g_duel_network_flags & 2) == 0)) ||
          (g_duel_ai_mode_state == 1) || (g_duel_network_state != 0))
      {
        s.done = 1;
        if ((g_other_player == player) &&
            ((g_duel_network_flags & 2) == 0) &&
            (g_duel_ai_mode_state != 1) &&
            (player_can_stop_at_phase(player, PHASE_UNTAP) != 0))
        {
          s.done = 0;
        }
      }
      else if (((s.must_untap_count == 0) || (s.must_untap_count == 1)) &&
               (s.optional_untap_count == 0) &&
               (player_can_stop_at_phase(player, PHASE_UNTAP) == 0))
      {
        s.done = 1;
      }

      if (s.done == 0)
      {
        reset_trigger_dispatch_state();
        if (((g_active_player == player) || ((g_duel_network_flags & 2) != 0)) &&
            (g_duel_ai_mode_state != 1) &&
            (g_duel_network_state == 0) &&
            ((s.must_untap_count > 0) || (s.optional_untap_count > 0)))
        {
          load_text(global_ui_strings_filename, "PROMPT_UNTAP");
          s.selected_card = select_card_for_action(player, -1, player, 0xff, 0, g_text_lines[0], 2);
        }
        else
        {
          if (g_active_player == player)
          {
            g_stop_phase = g_stop_phase_player = -1;
          }
          else
          {
            g_previous_stop_phase = g_previous_stop_phase_player = -1;
          }
          load_text(global_ui_strings_filename, "PROMPT_UNTAP");
          s.selected_card = select_card_for_action(player, player, player, 0xff, 0, g_text_lines[1], 2);
          g_phase_was_skipped = 1;
        }

        switch (g_target_selection_status_code)
        {
          case 0:
            if (s.selected_card != -1)
            {
              if ((contains_player_card_pair(s.must_untap_cards, s.must_untap_count, g_target_player_choice, s.selected_card) != 0) ||
                  (contains_player_card_pair(s.optional_untap_cards, s.optional_untap_count, g_target_player_choice, s.selected_card) != 0))
              {
                g_affected_card_controller = g_target_player_choice;
                g_affected_card = s.selected_card;
                C_dispatch_event_raw(0x7e);
              }
              else if ((can_activate_mana_source_for_stop_prompt(g_target_player_choice, s.selected_card) != 0) &&
                       (g_target_player_choice == player))
              {
                activate_mana_source_card(g_target_player_choice, s.selected_card);
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

    if (s.must_untap_count != 0)
    {
      s.done = 0;
      while (s.done == 0)
      {
        s.must_untap_count = 0;
        for (s.loop_player = 0; (s.loop_player < 2) && (s.must_untap_count == 0); s.loop_player++)
        {
          for (s.card = 0; (s.card < g_active_cards_count[s.loop_player]) && (s.must_untap_count == 0); s.card++)
          {
            if (is_in_play(s.loop_player, s.card) != 0)
            {
              g_affected_card_controller = s.loop_player;
              g_affected_card = s.card;
              g_event_result = 0;
              C_dispatch_event_raw(0x7d);
              s.event_result_copy = g_event_result;
              if (s.event_result_copy == 2)
              {
                s.must_untap_cards[s.must_untap_count * 2] = s.loop_player;
                s.must_untap_cards[s.must_untap_count * 2 + 1] = s.card;
                s.must_untap_count++;
              }
            }
          }
        }

        if (s.must_untap_count == 0)
        {
          s.done = 1;
        }
        else
        {
          g_affected_card_controller = s.must_untap_cards[0];
          g_affected_card = s.must_untap_cards[1];
          C_dispatch_event_raw(0x7e);
        }
      }
    }

    for (s.card = 0; s.card < g_active_cards_count[player]; s.card = s.card + 1)
    {
      if (((global_card_instances[player][s.card].untap_status & 1) != 0) &&
          ((global_card_instances[player][s.card].untap_status & 2) != 0))
      {
        global_card_instances[player][s.card].state &= -17;
        dispatch_event_to_single_card(player, s.card, 0x83, 1 - player, -1);
      }
    }

    for (s.card = 0; s.card < g_active_cards_count[player]; s.card = s.card + 1)
    {
      if (is_in_play(player, s.card) != 0)
      {
        global_card_instances[player][s.card].untap_status = 0;
      }
    }

    reassess_all_cards_and_mana();
    TENTATIVE_reassess_all_cards(0, 0xff);
    s.did_skip_untap = g_phase_was_skipped;
    if (s.did_skip_untap == 0)
    {
      prompt_stop_phase_anyway(1);
    }
    resolve_mana_burn();
    if (check_duel_finished() != 0)
    {
      return 1;
    }
  }
  return 0;
}

// FUNCTION: MAGIC 0x0044c88f
// FUNCTION: SHANDALAR 0x0040520a
int upkeep_phase(unsigned int player)
{
  struct
  {
    int trace_counter;
    char trace[100];
  } s;

  if (TRACE_ENABLED)
  {
    s.trace_counter = g_duel_trace_counter;
    g_duel_trace_counter++;
    sprintf(s.trace, "%d: Entering UpKeep Phase.\n", s.trace_counter);
    append_to_trace_txt(s.trace);
  }

  g_ai_speculation_depth = 0;
  g_phase_was_skipped = 0;
  g_phase_response_window_open = 0;
  if ((g_duel_network_flags & 2) != 0)
  {
    if (g_stop_phase == -1)
    {
      g_phase_stop_suppressed = 0;
    }
    else if (((g_stop_phase == PHASE_UPKEEP) && (player == g_stop_phase_player)) ||
             ((g_previous_stop_phase == PHASE_UPKEEP) && (player == g_previous_stop_phase_player)))
    {
      g_phase_stop_suppressed = 0;
    }
    else
    {
      g_phase_stop_suppressed = 1;
    }
  }
  else
  {
    if (g_duel_ai_mode_state != 1)
    {
      if ((g_stop_phase != -1) &&
          ((g_stop_phase != PHASE_UPKEEP) || (player != g_stop_phase_player)))
      {
        g_phase_stop_suppressed = 1;
      }
      else
      {
        g_phase_stop_suppressed = 0;
      }
    }
  }
  g_current_phase = PHASE_BEGIN_UPKEEP;
  update_phase_display(player, g_current_phase);
  dispatch_trigger(player, 0xc9, gs_begin_upkeep_008cf080, 0);
  g_phase_response_window_open = 1;
  g_current_phase = PHASE_UPKEEP;
  clear_all_upkeep_flags();
  allow_response(-1, g_current_phase, gs_upkeep_phase_00777d30, PHASE_UPKEEP);
  g_phase_response_window_open = 0;
  dispatch_trigger_twice_once_with_each_player_as_reason(player, 0xcb, gs_end_upkeep_00925c00, 0);
  g_phase_stop_suppressed = 0;
  g_response_processing_flags = 1;
  process_damage_prevention(player);
  g_response_processing_flags = 0;
  g_ai_speculation_depth = 0;
  resolve_mana_burn();
  if (check_duel_finished() != 0)
  {
    return 1;
  }
  return 0;
}

// FUNCTION: MAGIC 0x0044caa3
// FUNCTION: SHANDALAR 0x0040541e
int draw_phase(unsigned int player)
{
  int trace_counter;
  struct
  {
    char trace[100];
    int card;
    int draw_index;
    int draw_count;
  } s;

  if (TRACE_ENABLED)
  {
    trace_counter = g_duel_trace_counter;
    g_duel_trace_counter++;
    sprintf(s.trace, "%d: Entering Draw Phase.\n", trace_counter);
    append_to_trace_txt(s.trace);
  }

  if (g_first_turn_draw_pending != 0)
  {
    g_first_turn_draw_pending = 0;
    if (g_starting_player_was_random != 0)
    {
      return 0;
    }
  }

  {
    g_current_phase = PHASE_DRAW;
    update_phase_display(player, g_current_phase);
    g_phase_was_skipped = 0;
    g_phase_response_window_open = 0;
    if ((g_duel_network_flags & 2) != 0)
    {
      if (g_stop_phase == -1)
      {
        g_phase_stop_suppressed = 0;
      }
      else if (((g_stop_phase == PHASE_DRAW) && (player == g_stop_phase_player)) ||
               ((g_previous_stop_phase == PHASE_DRAW) && (player == g_previous_stop_phase_player)))
      {
        g_phase_stop_suppressed = 0;
      }
      else
      {
        g_phase_stop_suppressed = 1;
      }
    }
    else
    {
      if (g_duel_ai_mode_state != 1)
      {
        if ((g_stop_phase != -1) &&
            ((g_stop_phase != PHASE_DRAW) || (player != g_stop_phase_player)))
        {
          g_phase_stop_suppressed = 1;
        }
        else
        {
          g_phase_stop_suppressed = 0;
        }
      }
    }
    dispatch_trigger(player, 0xce, gs_draw_phase_008cf560, 1);
    g_event_result = 1;
    C_dispatch_event_raw(10);
    s.draw_count = g_event_result;
    if (s.draw_count > 0)
    {
      if ((player == g_other_player) && ((g_duel_network_flags & 2) == 0))
      {
        for (s.draw_index = 0; s.draw_count > s.draw_index; s.draw_index = s.draw_index + 1)
        {
          draw_card_for_player(player);
        }
      }
      else
      {
        for (s.draw_index = 0; s.draw_count > s.draw_index; s.draw_index = s.draw_index + 1)
        {
          s.card = add_card_to_hand(player, g_draw_card_placeholder_internal_card_id);
          if (s.card != -1)
          {
            global_card_instances[player][s.card].state |= 2;
          }
        }
        TENTATIVE_reassess_all_cards(0, 0x30);
      }
    }

    g_phase_response_window_open = 1;
    g_phase_was_skipped = 0;
    allow_response(-1, g_current_phase, gs_draw_phase_0091c100, g_current_phase);
    g_phase_response_window_open = 0;
    if ((g_battlefield_extra_ability_flags & 2) != 0)
    {
      dispatch_trigger_twice_once_with_each_player_as_reason(player, 0xe2, gs_end_draw_0091c510, 1);
    }
    g_phase_stop_suppressed = 0;
    resolve_mana_burn();
    if (check_duel_finished() != 0)
    {
      return 1;
    }
  }
  return 0;
}

// FUNCTION: MAGIC 0x0044f682
// FUNCTION: SHANDALAR 0x00407feb
int discard_phase(unsigned int player, int phase_mode)
{
  struct
  {
    int trace_counter;
    char trace[100];
    int hand_count_for_discard;
    int card;
    int did_discard;
    int allow_response_result;
  } s;

  s.trace_counter = g_duel_trace_counter;
  s.did_discard = 0;
  if (TRACE_ENABLED)
  {
    g_duel_trace_counter++;
    sprintf(s.trace, "%d: Entering Discard Phase.\n", s.trace_counter);
    append_to_trace_txt(s.trace);
  }

  for (s.card = 0; s.card < g_active_cards_count[player]; s.card = s.card + 1)
  {
    if ((global_card_instances[player][s.card].internal_card_id != -1) &&
        ((global_card_instances[player][s.card].state & STATE_IN_PLAY) != 0) &&
        (global_cards_data[global_card_instances[player][s.card].internal_card_id].id == 0x8c))
    {
      return 0;
    }
  }

  if (phase_mode == 0)
  {
    g_phase_was_skipped = 0;
    g_phase_response_window_open = 0;
    if ((g_duel_network_flags & 2) != 0)
    {
      if (g_stop_phase == -1)
      {
        g_phase_stop_suppressed = 0;
      }
      else if (((g_stop_phase == PHASE_DISCARD) && (player == g_stop_phase_player)) ||
               ((g_previous_stop_phase == PHASE_DISCARD) && (player == g_previous_stop_phase_player)))
      {
        g_phase_stop_suppressed = 0;
      }
      else
      {
        g_phase_stop_suppressed = 1;
      }
    }
    else
    {
      if (g_duel_ai_mode_state != 1)
      {
        if ((g_stop_phase == -1) ||
            ((g_stop_phase == PHASE_DISCARD) && (player == g_stop_phase_player)))
        {
          g_phase_stop_suppressed = 0;
        }
        else
        {
          g_phase_stop_suppressed = 1;
        }
      }
    }
  }

  do
  {
    if ((phase_mode == 0) &&
        (g_duel_ai_mode_state != 1) &&
        (g_active_player == player) &&
        ((g_duel_network_flags & 2) == 0))
    {
      TENTATIVE_reassess_all_cards(0, 0xff);
      start_ai_decision_search(3, 0x1e);
    }
    if (g_ai_decision_code == 3)
    {
      reset_ai_search_trial_state();
      g_ai_side_score_player_1 = 0;
      g_ai_side_score_player_0 = g_ai_side_score_player_1;
      g_duel_state_008a8de4 = g_ai_side_score_player_0;
      g_ai_modifier = g_duel_state_008a8de4;
    }
    g_current_phase = PHASE_DISCARD;
    update_phase_display(player, g_current_phase);
    g_phase_response_window_open = 1;
    s.allow_response_result = allow_response(-1, g_current_phase, gs_discard_phase_0091d080, PHASE_DISCARD);
    g_phase_response_window_open = 0;
  } while (s.allow_response_result != 0);

  if ((g_duel_ai_mode_state != 1) ||
      ((g_other_player == player) && ((g_duel_network_flags & 2) == 0)))
  {
    if (g_duel_ai_mode_state != 1)
    {
      g_phase_stop_suppressed = 0;
    }
    if ((g_player_special_effect_flags[player] & 2) == 0)
    {
      if ((g_duel_network_flags & 2) == 0)
      {
        if (g_active_player == player)
        {
          s.hand_count_for_discard = 0;
        }
        else
        {
          s.hand_count_for_discard = g_hand_count_adjustment;
        }
      }
      else
      {
        s.hand_count_for_discard = 0;
      }
      for (s.card = 0; s.card < g_active_cards_count[player]; s.card = s.card + 1)
      {
        if ((global_card_instances[player][s.card].internal_card_id != -1) &&
            ((global_card_instances[player][s.card].state & STATE_IN_PLAY) == 0))
        {
          s.hand_count_for_discard++;
        }
      }
      g_event_result = 0;
      C_dispatch_event_raw(0x1f);
      s.did_discard = 0;
      while ((g_event_result == 0) && (s.hand_count_for_discard > 7))
      {
        discard(player, 0, 1);
        s.hand_count_for_discard--;
        if ((player == g_other_player) &&
            ((g_duel_network_flags & 2) == 0) &&
            (g_duel_ai_mode_state == 1))
        {
          g_ai_modifier -= 0x18;
        }
        if (((g_active_player == player) || ((g_duel_network_flags & 2) != 0)) &&
            (g_duel_ai_mode_state == 0) &&
            (g_duel_network_state == 0))
        {
          s.did_discard = 1;
        }
      }
    }
  }

  if ((g_battlefield_extra_ability_flags & 2) != 0)
  {
    dispatch_trigger_twice_once_with_each_player_as_reason(player, 0xe4, gs_end_discard_007ab020, 1);
  }
  resolve_mana_burn();
  if (check_duel_finished() != 0)
  {
    return 1;
  }
  return 0;
}

// FUNCTION: MAGIC 0x0044fb96
// FUNCTION: SHANDALAR 0x004084f7
void cleanup_phase(unsigned int player)
{
  struct
  {
    int trace_counter;
    char trace[100];
    int other_player;
    int card;
  } s;

  s.other_player = 1 - player;
  if (TRACE_ENABLED)
  {
    s.trace_counter = g_duel_trace_counter;
    g_duel_trace_counter++;
    sprintf(s.trace, "%d: Entering Cleanup Phase.\n", s.trace_counter);
    append_to_trace_txt(s.trace);
  }

  if (g_duel_ai_mode_state != 1)
  {
    g_current_phase = PHASE_CLEANUP;
    update_phase_display(player, g_current_phase);
    g_phase_was_skipped = 0;
    g_phase_response_window_open = 0;
    if ((g_duel_network_flags & 2) != 0)
    {
      if (g_stop_phase == -1)
      {
        g_phase_stop_suppressed = 0;
      }
      else if (((g_stop_phase == PHASE_CLEANUP2) && (player == g_stop_phase_player)) ||
               ((g_previous_stop_phase == PHASE_CLEANUP2) && (player == g_previous_stop_phase_player)))
      {
        g_phase_stop_suppressed = 0;
      }
      else
      {
        g_phase_stop_suppressed = 1;
      }
    }
    else
    {
      if (g_duel_ai_mode_state != 1)
      {
        if ((g_stop_phase == -1) ||
            ((g_stop_phase == PHASE_CLEANUP2) && (player == g_stop_phase_player)))
        {
          g_phase_stop_suppressed = 0;
        }
        else
        {
          g_phase_stop_suppressed = 1;
        }
      }
    }
  }

  for (s.card = 0; s.card < MAX(g_active_cards_count[0], g_active_cards_count[1]); s.card++)
  {
    if (is_in_play(player, s.card) != 0)
    {
      global_card_instances[player][s.card].damage_on_card = 0;
      dispatch_event_to_single_card(player, s.card, 0x22, s.other_player, -1);
    }
    if (is_in_play(s.other_player, s.card) != 0)
    {
      global_card_instances[s.other_player][s.card].damage_on_card = 0;
      dispatch_event_to_single_card(s.other_player, s.card, 0x22, player, -1);
    }
  }

  if (g_opponent_starting_card_id_2 != -1)
  {
    (*global_cards_data[g_opponent_starting_card_id_2].code_pointer)(0, 0x94, 0x22);
  }
}

// FUNCTION: MAGIC 0x0044fe1a
// FUNCTION: SHANDALAR 0x0040877b
int end_turn_phase(unsigned int player)
{
  struct
  {
    int trace_counter;
    char trace[100];
    int found;
    short dynamic_card_count;
    int other_player;
    int dynamic_card_index;
    int card;
    short dynamic_card_ids[300];
  } s;

  s.other_player = 1 - player;
  if (TRACE_ENABLED)
  {
    s.trace_counter = g_duel_trace_counter;
    g_duel_trace_counter++;
    sprintf(s.trace, "%d: Entering End Turn Phase.\n", s.trace_counter);
    append_to_trace_txt(s.trace);
  }

  dispatch_trigger_twice_once_with_each_player_as_reason(player, 0xcd, gs_end_of_turn_008cd980, 0);
  regenerate_or_graveyard_triggers();
  if (g_duel_ai_mode_state != 1)
  {
    g_phase_stop_suppressed = 0;
  }
  process_damage_prevention(player);
  TENTATIVE_reassess_all_cards(0, 0xff);

  for (s.card = 0, s.dynamic_card_count = 0; s.card < 0x96; s.card = s.card + 1)
  {
    if (global_card_instances[player][s.card].internal_card_id != -1)
    {
      global_card_instances[player][s.card].state &= ~(STATE_JUST_DRAWED | STATE_ATTACKING | STATE_BLOCKING | STATE_ATTACKED | STATE_ISBLOCKED | STATE_UNKNOWN8000);
      global_card_instances[player][s.card].blocking = -1;
      global_card_instances[player][s.card].damage_on_card = 0;
    }
    if (global_card_instances[s.other_player][s.card].internal_card_id != -1)
    {
      global_card_instances[s.other_player][s.card].state &= ~(STATE_JUST_DRAWED | STATE_ATTACKING | STATE_BLOCKING | STATE_ATTACKED | STATE_ISBLOCKED | STATE_UNKNOWN8000);
      global_card_instances[s.other_player][s.card].blocking = -1;
      global_card_instances[s.other_player][s.card].damage_on_card = 0;
    }
    if (global_card_instances[player][s.card].internal_card_id >= g_card_count)
    {
      s.dynamic_card_ids[s.dynamic_card_count] = (short)global_card_instances[player][s.card].internal_card_id;
      s.dynamic_card_count++;
    }
    if (global_card_instances[s.other_player][s.card].internal_card_id >= g_card_count)
    {
      s.dynamic_card_ids[s.dynamic_card_count] = (short)global_card_instances[s.other_player][s.card].internal_card_id;
      s.dynamic_card_count++;
    }
  }

  for (s.card = g_card_count; s.card < g_card_count + 0x10; s.card = s.card + 1)
  {
    if (global_cards_data[s.card].id != -1)
    {
      s.dynamic_card_index = s.dynamic_card_count - 1;
      s.found = 0;
      for (; (s.dynamic_card_index >= 0) && (s.found == 0); s.dynamic_card_index--)
      {
        if (s.dynamic_card_ids[s.dynamic_card_index] == s.card)
        {
          s.found = 1;
        }
      }

      for (s.dynamic_card_index = 0;
           s.dynamic_card_index < MAX(g_active_cards_count[g_other_player], g_active_cards_count[g_active_player]);
           s.dynamic_card_index++)
      {
        if ((global_card_instances[g_active_player][s.dynamic_card_index].internal_card_id != -1) &&
            ((global_card_instances[g_active_player][s.dynamic_card_index].state & STATE_IN_PLAY) != 0) &&
            (global_card_instances[g_active_player][s.dynamic_card_index].dummy3 == s.card))
        {
          s.found = 1;
        }
        if ((global_card_instances[g_other_player][s.dynamic_card_index].internal_card_id != -1) &&
            ((global_card_instances[g_other_player][s.dynamic_card_index].state & STATE_IN_PLAY) != 0) &&
            (global_card_instances[g_other_player][s.dynamic_card_index].dummy3 == s.card))
        {
          s.found = 1;
        }
      }

      if (s.found == 0)
      {
        global_cards_data[s.card].id = -1;
      }
    }
  }

  g_battlefield_extra_ability_flags = 0;
  return 0;
}

// FUNCTION: MAGIC 0x00450468
// FUNCTION: SHANDALAR 0x00408dcc
int ai_decision_phase(unsigned int player, int *next_state, int *phase_mode, int *phase_value)
{
  struct
  {
    char debug_text[100];
    char trace[100];
    int saved_ai_try_count;
    int opponent;
    int index;
    int loop_player;
    int skip_cleanup_stop_prompt;
    int saved_special_mana[2][8];
    int saved_ai_flags;
    int ai_score;
  } s;
  int trace_counter;

  if (TRACE_ENABLED)
  {
    trace_counter = g_duel_trace_counter;
    g_duel_trace_counter++;
    sprintf(s.trace, "%d: Entering AI Decision Phase.\n", trace_counter);
    append_to_trace_txt(s.trace);
  }

  *next_state = -1;
  if (g_duel_ai_mode_state == 1)
  {
    s.opponent = 1 - player;
    for (s.index = 0; s.index <= 7; s.index = s.index + 1)
    {
      s.saved_special_mana[s.opponent][s.index] = g_special_mana_pool[s.opponent][s.index];
      g_special_mana_pool[s.opponent][s.index] = g_basiclandtypes_controlled[s.opponent][s.index];
    }

    C_dispatch_event_raw(199);
    for (s.index = 0; s.index < g_active_cards_count[g_other_player]; s.index = s.index + 1)
    {
      if (global_card_instances[g_other_player][s.index].internal_card_id != -1)
      {
        (*global_cards_data[global_card_instances[g_other_player][s.index].internal_card_id].code_pointer)(g_other_player, s.index, 0x38);
      }
    }

    process_damage_prevention(player);
    regenerate_or_graveyard_triggers();
    for (s.loop_player = 0; s.loop_player < 2; s.loop_player = s.loop_player + 1)
    {
      for (s.index = 0; s.index <= 7; s.index = s.index + 1)
      {
        g_special_mana_pool[s.loop_player][s.index] = s.saved_special_mana[s.loop_player][s.index];
      }
    }

    reassess_all_cards_and_mana();
    s.ai_score = g_ai_modifier + ai_opinion_of_gamestate(g_other_player);
    if (g_life[1] > 0)
    {
      g_ai_search_flags |= 4;
    }
    if (g_reveal_all_world_info != 0)
    {
      show_ai_action_log_dialog(0, s.ai_score);
    }
    if ((g_ai_search_best_score < s.ai_score) && (g_recorded_action_cursor == 0))
    {
      g_ai_search_best_score = s.ai_score;
      save_recorded_ai_actions();
      s.saved_ai_flags = g_ai_search_flags;
      s.saved_ai_try_count = g_ai_search_try_count;
    }
    if (g_ai_search_target_depth == 999)
    {
      g_ai_search_target_depth = -1;
    }
    g_ai_search_force_pass = 0;
    *phase_value = g_ai_search_force_pass;
    g_recorded_action_cursor = *phase_value;
    if ((g_ai_search_time_limit / 2 < get_ai_search_elapsed_time()) &&
        ((((g_shandalar_difficulty + 1) * 5) * 5 <= g_ai_search_try_count) ||
         (((g_ai_search_flags & 4) == 0 ? 0xc8 : 0x32) < get_ai_search_elapsed_time())) &&
        ((g_reveal_all_world_info == 0) || (g_ai_search_try_count > 0x32)))
    {
      sprintf(s.debug_text, "phase: %3d num tries: %4d mtime %4d calls %d\n", g_ai_response_state, g_ai_search_try_count, g_ai_search_time_limit / 2, g_affected_card_context_index);
      OutputDebugStringA(s.debug_text);
      if (g_reveal_all_world_info != 0)
      {
        show_ai_action_log_dialog(1, g_ai_search_best_score);
      }
      g_adventure_duel_internal_card_id = -1;
      g_duel_ai_mode_state = 0;
      g_ai_search_target_depth = -1;
      g_ai_search_flags = s.saved_ai_flags;
    }
    else
    {
    }

    g_ai_search_try_count++;
    g_duel_state_007161cc = 0;
    if (g_ai_decision_code == 1)
    {
      if ((g_land_can_be_played & TENTATIVE_LCBP_DURING_SECOND_MAIN_PHASE) != 0)
      {
        g_current_phase = PHASE_MAIN2;
      }
      else
      {
        g_current_phase = PHASE_MAIN1;
      }
      *phase_mode = 1;
      *next_state = 6;
    }
    if (g_ai_decision_code == 2)
    {
      g_current_phase = PHASE_NORMAL_COMBAT_DAMAGE;
      *phase_mode = 2;
      *next_state = 6;
    }
    if (g_ai_decision_code == 3)
    {
      *phase_mode = 1;
      *next_state = 7;
    }
    if (g_ai_decision_code == 4)
    {
      if ((g_land_can_be_played & TENTATIVE_LCBP_DURING_COMBAT) != 0)
      {
        g_current_phase = PHASE_MAIN2;
      }
      else
      {
        g_current_phase = PHASE_MAIN1;
      }
      *phase_value = 0;
      *phase_mode = 4;
      *next_state = 6;
    }
    if (g_ai_decision_code == 5)
    {
      g_current_phase = PHASE_NORMAL_COMBAT_DAMAGE;
      *phase_mode = 5;
      *next_state = 6;
    }
    if (g_ai_decision_code == 6)
    {
      g_current_phase = PHASE_NORMAL_COMBAT_DAMAGE;
      *phase_mode = 6;
      *next_state = 6;
    }
    if (g_ai_decision_code == 7)
    {
      if ((g_land_can_be_played & TENTATIVE_LCBP_DURING_COMBAT) != 0)
      {
        g_current_phase = PHASE_MAIN2;
      }
      else
      {
        g_current_phase = PHASE_MAIN1;
      }
      *phase_value = 0;
      *phase_mode = 7;
      *next_state = 6;
    }
    if (g_ai_decision_code == 8)
    {
      g_current_phase = PHASE_NORMAL_COMBAT_DAMAGE;
      *phase_mode = 8;
      *next_state = 6;
    }
  }

  if (*next_state == -1)
  {
    g_current_phase = PHASE_CLEANUP2;
    s.skip_cleanup_stop_prompt = 0;
    if (s.skip_cleanup_stop_prompt == 0)
    {
      prompt_stop_phase_anyway(0x20);
    }
    resolve_mana_burn();
    if (check_duel_finished() != 0)
    {
      return 1;
    }
    if ((g_current_player == g_stop_phase_player) && (g_stop_phase == PHASE_CLEANUP2))
    {
      g_stop_phase_player = -1;
      g_stop_phase = g_stop_phase_player;
    }
    should_skip_phase(player);
    play_sound_effect(5);
  }
  return 0;
}

// FUNCTION: MAGIC 0x004b559a
// FUNCTION: SHANDALAR 0x0056bd9d
int AddCardToDeckSorted(int card_id)
{

  struct
  {
    int selected_state_sprite;
    int location_block_start_index;
    unsigned int inner_index;
    int entry_index;
    int candidate_sort_key;
  } s;

  if (GetCardRarity(card_id) >= 3)
  {
    AddJournalEntry((int)card_id / 0x100 + JOURNAL_ENTRY_CARD_FOUND_SET0, card_id & 0xff);
  }
  s.entry_index = single_color_test_bit_to_color_t(global_cards_data[card_id].color) * 0x20 + (unsigned int)global_cards_data[card_id].type * 0x100 +
                  (int)global_cards_data[card_id].name[0];
  s.selected_state_sprite = 0;
  for (s.location_block_start_index = 0; s.location_block_start_index < 500; s.location_block_start_index = s.location_block_start_index + 1)
  {
    if (deck[s.location_block_start_index] == -1)
    {
      s.selected_state_sprite = 1;
    }
  }
  if (s.selected_state_sprite == 0)
  {
    return -1;
  }
  for (s.location_block_start_index = 0x1f2; s.location_block_start_index >= 0; s.location_block_start_index = s.location_block_start_index - 1)
  {
    if (deck[s.location_block_start_index] == -1)
    {
      continue;
    }

    s.inner_index = (unsigned int)deck[s.location_block_start_index] & 0xfff;
    s.candidate_sort_key = single_color_test_bit_to_color_t(global_cards_data[s.inner_index].color) * 0x20 + (int)global_cards_data[s.inner_index].name[0] +
                           (unsigned int)global_cards_data[s.inner_index].type * 0x100;
    if (s.candidate_sort_key >= s.entry_index)
    {
      deck[s.location_block_start_index + 1] = card_id;
      card_id = (unsigned int)-1;
      return s.location_block_start_index + 1;
    }
    deck[s.location_block_start_index + 1] = (unsigned int)deck[s.location_block_start_index];
  }
  deck[0] = card_id;
  return 0;
}

// FUNCTION: MAGIC 0x004b6091
// FUNCTION: SHANDALAR 0x0056c892
int CountDuelPoolEligibleTowns(void)
{
  int duel_pool_count;
  int town_index;

  duel_pool_count = 0;
  for (town_index = 0; town_index < 0x80; town_index = town_index + 1)
  {
    if ((((g_town_slots[town_index].status_and_ruling_wizard & 0xff01U) == 1) &&
         (1 < g_town_slots[town_index].location_type)) &&
        (g_town_slots[town_index].location_type < 4))
    {
      duel_pool_count = duel_pool_count + 1;
    }
  }
  return duel_pool_count;
}

// FUNCTION: MAGIC 0x004e1c4f
// FUNCTION: SHANDALAR 0x00561441
char *GetCreatureName(int creature_type)
{
#ifdef SHANDALAR
  return g_shandalar_monster_definitions[creature_type].name;
#else
  (void)creature_type;
  return "";
#endif
}

// FUNCTION: MAGIC 0x004c0a0a
// FUNCTION: SHANDALAR 0x004ece9a
int SeedRandomFromTickCount(void)
{
  // GLOBAL: SHANDALAR 0x007483f4
  // GLOBAL: MAGIC 0x0074b634
  static int g_random_seed_tick_value;

  g_random_seed_tick_value = GetTickCount() & 0x7fff;
  g_random_seed_initialized = 1;
  return 0;
}

// FUNCTION: MAGIC 0x004b5416
// FUNCTION: SHANDALAR 0x0056bc19
int draw_random_unmarked_deck_card(void)
{
  int candidate_count;
  int candidate_indexes[500];
  int deck_index;
  int random_index;

  candidate_count = 0;
  for (deck_index = 0; deck_index < 500; deck_index++)
  {
    if ((deck[deck_index] != -1) && ((deck[deck_index] & 0xc000) == 0))
    {
      candidate_indexes[candidate_count] = deck_index;
      candidate_count++;
    }
  }
  if (candidate_count == 0)
  {
    return -1;
  }
  random_index = internal_rand(candidate_count);
  deck[candidate_indexes[random_index]] |= 0x8000;
  return deck[candidate_indexes[random_index]] & 0xfff;
}

// FUNCTION: MAGIC 0x00464700
// FUNCTION: SHANDALAR 0x005221b0
void copy_initial_library_to_player_zero(int wizard_color)
{
  int card_index;

  for (card_index = 0; card_index < 200; card_index++)
  {
    g_initial_library[0][card_index].csvid = g_initial_library[wizard_color][card_index].csvid;
    g_initial_library[0][card_index].numcards = g_initial_library[wizard_color][card_index].numcards;
  }
}

// FUNCTION: MAGIC 0x0046476f
// FUNCTION: SHANDALAR 0x0052221f
void remove_iid_from_initial_library(int player, int internal_card_id)
{
  int card_index;

  for (card_index = 0; card_index < 200; card_index++)
  {
    if ((g_initial_library[player][card_index].numcards > 0) &&
        (CardTypeFromID(g_initial_library[player][card_index].csvid) == internal_card_id))
    {
      g_initial_library[player][card_index].numcards--;
      return;
    }
  }
}

// FUNCTION: MAGIC 0x004b3dee
// FUNCTION: SHANDALAR 0x0056a05a
void perform_player_mulligan(int player, int wizard_color)
{
  struct
  {
    char trace_text[500];
    int library_index;
    int card_index;
    int packet_card;
    int found_card;
  } s;

  if ((g_duel_network_flags & 2) != 0)
  {
    append_to_trace_txt("Doing the mulligan.\n");
  }
  if ((g_duel_network_flags & 2) != 0)
  {
    append_to_trace_txt("Discarding my the hand.\n");
  }
  for (s.card_index = 0; s.card_index < 0x96; s.card_index++)
  {
    if (global_card_instances[player][s.card_index].internal_card_id != -1)
    {
      s.found_card = 0;

      for (s.library_index = 0; (s.library_index < 200) && (s.found_card == 0); s.library_index++)
      {
        if (CardTypeFromID(g_initial_library[wizard_color][s.library_index].csvid) ==
            global_card_instances[player][s.card_index].internal_card_id)
        {
          g_initial_library[wizard_color][s.library_index].numcards++;
          s.found_card = 1;
        }
      }
      global_card_instances[player][s.card_index].internal_card_id = -1;
    }
  }
  if ((g_duel_network_flags & 2) != 0)
  {
    if (g_active_player == player)
    {
      append_to_trace_txt("Re-dealing my hand.\n");
      for (s.card_index = 0; s.card_index < 7; s.card_index++)
      {
        s.packet_card = DrawRandomCardFromInitialLibrary(wizard_color);
        add_card_to_hand(player, s.packet_card);
        AddCardToCLPacket(s.packet_card);
        sprintf(s.trace_text, "%s(%d)\n", global_cards_data[s.packet_card].name, s.packet_card);
        append_to_trace_txt(s.trace_text);
      }
      append_to_trace_txt("NONE\n");
      AddCardToCLPacket(-1);
      TENTATIVE_send_network_result(0, 3);
    }
    else
    {
      append_to_trace_txt("Getting opponent's hand.\n");
      TENTATIVE_wait_for_network_result(1, 3);
      s.card_index = s.packet_card = 0;
      while (s.packet_card != -1)
      {
        s.packet_card = GetCardFromCLPacket(s.card_index++);
        sprintf(s.trace_text, "%s(%d)\n", global_cards_data[s.packet_card].name, s.packet_card);
        append_to_trace_txt(s.trace_text);
        if (s.packet_card != -1)
        {
          add_card_to_hand(player, s.packet_card);
        }
      }
    }
  }
  else
  {
    for (s.card_index = 0; s.card_index < 7; s.card_index++)
    {
      add_card_to_hand(player, DrawRandomCardFromInitialLibrary(wizard_color));
    }
  }
}

// FUNCTION: MAGIC 0x004b3cc2
// FUNCTION: SHANDALAR 0x00569f2e
void evaluate_opening_hand_land_counts(int *redraw_result, int *ante_result, int *ante_info)
{
  int card_index;
  int opponent_land_count;
  int player_land_count;

  player_land_count = 0;
  opponent_land_count = 0;
  for (card_index = 0; card_index < 7; card_index++)
  {
    if ((global_cards_data[global_card_instances[1][card_index].internal_card_id].type & TYPE_LAND) != 0)
    {
      opponent_land_count++;
    }
    if ((global_cards_data[global_card_instances[0][card_index].internal_card_id].type & TYPE_LAND) != 0)
    {
      player_land_count++;
    }
  }
  if (player_land_count == 0)
  {
    *redraw_result = 1;
  }
  else if (player_land_count == 7)
  {
    *redraw_result = 2;
  }
  else
  {
    *redraw_result = 0;
  }
  if (opponent_land_count == 0)
  {
    *ante_result = 1;
  }
  else if (opponent_land_count == 7)
  {
    *ante_result = 2;
  }
  else
  {
    *ante_result = 0;
  }
  if ((opponent_land_count < 2) || (opponent_land_count > 5))
  {
    *ante_info = 1;
  }
  else
  {
    *ante_info = 0;
  }
}

// FUNCTION: MAGIC 0x004b23a9
// FUNCTION: SHANDALAR 0x0056862e
int play_duel(int player, int creature_type)
{
  struct
  {
    int unused_68;                  // ebp - 0x68
    int wizard_color;               // ebp - 0x64
    int library_count;              // ebp - 0x60
    int quantity;                   // ebp - 0x5c
    int loop_5c;                    // ebp - 0x58
    int packet_card;                // ebp - 0x54
    int loop_player;                // ebp - 0x50
    int shandalar_deck_minimums[6]; // ebp - 0x4c
    unsigned int mulligan_accepted; // ebp - 0x34
    int ante_result;                // ebp - 0x30
    int victory_count;              // ebp - 0x2c
    int opening_hand_count;         // ebp - 0x28
    int random_starting_player;     // ebp - 0x24
    int town_index;                 // ebp - 0x20
    int redraw_result;              // ebp - 0x1c
    int card_index;                 // ebp - 0x18
    int ante_info;                  // ebp - 0x14
    unsigned int next_player;       // ebp - 0x10
    unsigned int starting_player;   // ebp - 0xc
    int town_count;                 // ebp - 0x8
    int player_index;               // ebp - 0x4
  } s;

  (void)player;
  if ((g_duel_mode_flags & 1) == 0)
  {
    creature_type = -1;
  }

  StopWorldLocationMusic();
#ifdef SHANDALAR
  refresh_ai_random_table();
#endif
  g_duel_active = 1;
  for (s.player_index = 0; s.player_index < 2; s.player_index = s.player_index + 1)
  {
    for (s.card_index = 0; s.card_index < 0x96; s.card_index = s.card_index + 1)
    {
      global_card_instances[s.player_index][s.card_index].original_internal_card_id = -1;
      global_card_instances[s.player_index][s.card_index].internal_card_id =
          global_card_instances[s.player_index][s.card_index].original_internal_card_id;
    }
    for (s.card_index = 0; s.card_index < 500; s.card_index = s.card_index + 1)
    {
      global_exile[s.player_index][s.card_index] = -1;
      global_graveyard_slots[s.player_index][s.card_index] = global_exile[s.player_index][s.card_index];
    }
    g_player_duel_state_00938e38[s.player_index] = 0;
    g_active_cards_count[s.player_index] = 0;
  }

  g_duel_special_land_card_ids[1] = 0xef;
  g_duel_special_land_card_ids[2] = 0x7e;
  g_duel_special_land_card_ids[3] = 0x5b;
  g_duel_special_land_card_ids[4] = 0xa4;
  g_duel_special_land_card_ids[5] = 0xbc;
  g_battlefield_extra_ability_flags = 0;
  g_lich_active[0] = 0;
  g_lich_active[1] = 0;
  reset_duel_globals();
  reset_timestamp_players();

  if (creature_type != -1)
  {
    s.shandalar_deck_minimums[4] = 0;
    s.shandalar_deck_minimums[0] = 0x1e;
    s.shandalar_deck_minimums[1] = 0x23;
    s.shandalar_deck_minimums[2] = 0x28;
    s.shandalar_deck_minimums[3] = 0x28;
    g_life[0] = 10;
    if ((g_world_magic_bitmap & 2) != 0)
    {
      g_life[0] += 2;
    }
    if ((g_world_magic_bitmap & 0x800) != 0)
    {
      g_life[0] += 3;
    }
    if ((g_world_magic_bitmap & 0x80) != 0)
    {
      g_life[0] += 5;
    }
    g_life[0] = CountDuelPoolEligibleTowns();
    g_life[0] += g_next_duel_life_delta;
    g_life[0] += g_dungeon_life_reward_delta;
    if (g_next_duel_card_id > 0 && g_next_duel_card_id <= 5)
    {
      g_life[0] += g_next_duel_card_id;
    }
    g_player_starting_life = g_life[0];
    g_next_duel_life_delta = 0;
    g_life[1] = g_shandalar_monster_definitions[creature_type].tier;
    if (creature_type <= 0x24 && creature_type % 7 != 0)
    {
      g_life[1] += g_shandalar_difficulty * 2;
    }
    else if (creature_type <= 0x24 && creature_type % 7 == 0)
    {
      g_life[1] += g_shandalar_difficulty * 5;
    }
    else if (creature_type < 0x37)
    {
      g_life[1] += g_shandalar_difficulty * 2;
    }
    else if (creature_type >= 0x37)
    {
      g_life[1] += g_shandalar_difficulty * 50;
    }
    if (g_shandalar_monster_definitions[creature_type].encounter_type == 0xb)
    {
      for (s.card_index = 0; s.card_index < 10; s.card_index = s.card_index + 1)
      {
        if ((g_world_magic_bitmap & (1 << (byte)s.card_index)) != 0)
        {
          g_life[1]++;
        }
      }
    }
    if (g_shandalar_monster_definitions[creature_type].encounter_type == 0xc)
    {
      g_life[1] += 10;
      s.victory_count = 0;
      s.town_count = 0;
      for (s.card_index = 0; s.card_index < 1000; s.card_index = s.card_index + 1)
      {
        if (g_duel_victory_log[s.card_index] == 0)
        {
          break;
        }
        if ((g_duel_victory_log[s.card_index] >> 4) == g_current_encounter_color)
        {
          s.victory_count++;
        }
      }
      g_life[1] -= s.victory_count;
      for (s.town_index = 0; s.town_index < 128; s.town_index = s.town_index + 1)
      {
        if ((g_town_slots[s.town_index].status_and_ruling_wizard & 0xff00) != 0)
        {
          if (((g_town_slots[s.town_index].status_and_ruling_wizard >> 8) - 1) == s.card_index)
          {
            s.town_count++;
          }
        }
      }
      g_life[1] += s.town_count * g_shandalar_difficulty;
      g_life[1] = MAX(g_life[1], g_shandalar_difficulty * 5 + 20);
    }
    if (g_shandalar_monster_definitions[creature_type].encounter_type == 0xd)
    {
      g_life[1] = g_shandalar_difficulty * 100 + 100;
    }
    strcpy(g_duel_message_text, GetCreatureName(creature_type));
    s.opening_hand_count = ClampIntToRange(g_current_encounter_strength + g_shandalar_difficulty + 4, 0, 99);
    if (g_shandalar_difficulty == 3)
    {
      s.opening_hand_count = 7;
    }
    s.opening_hand_count += g_encounter_opening_hand_size_modifier;
    s.opening_hand_count = 7;
    if (s.opening_hand_count < 3)
    {
      s.opening_hand_count = 3;
    }
    g_hand_count_adjustment = 7 - s.opening_hand_count;
    SeedRandomFromTickCount();
    if (g_shandalar_difficulty != 0 && internal_rand(2) != 0 && g_duel_ai_mode_state == 0)
    {
      s.starting_player = 1;
    }
    else
    {
      s.starting_player = 0;
    }
    s.random_starting_player = 1;
    g_starting_player_was_random = 1;
    if (g_force_opponent_starts_duel != 0 || g_next_duel_card_id == 0)
    {
      if (g_force_opponent_starts_duel != 0)
      {
        s.starting_player = 1;
      }
      else
      {
        s.starting_player = 0;
      }
      s.random_starting_player = 0;
      g_starting_player_was_random = 0;
      g_force_opponent_starts_duel = 0;
    }

    if (g_duel_ai_mode_state != 0)
    {
      goto idk;
    }
    g_restore_deck_after_duel = 0;
    for (s.card_index = 0; s.card_index < 500; s.card_index = s.card_index + 1)
    {
      if (deck[s.card_index] != -1 && (deck[s.card_index] & 0x4000) == 0)
      {
        s.shandalar_deck_minimums[4]++;
      }
    }
    if (s.shandalar_deck_minimums[4] < s.shandalar_deck_minimums[g_shandalar_difficulty])
    {
      g_restore_deck_after_duel = 1;
      memcpy(g_deck_restore_buffer, deck, sizeof(g_deck_restore_buffer));
      for (s.card_index = 0;
           s.card_index < s.shandalar_deck_minimums[g_shandalar_difficulty] - s.shandalar_deck_minimums[4];
           s.card_index = s.card_index + 1)
      {
        AddCardToDeckSorted(internal_rand(5));
      }
    }
    for (s.card_index = 0; s.card_index < 0x3c; s.card_index = s.card_index + 1)
    {
      global_library[1][s.card_index] = 0;
      global_library[0][s.card_index] = global_library[1][s.card_index];
    }
    for (s.card_index = 0x3c; s.card_index < 500; s.card_index = s.card_index + 1)
    {
      global_library[1][s.card_index] = -1;
      global_library[0][s.card_index] = global_library[1][s.card_index];
    }
    TENTATIVE_reassess_all_cards(0, 0x30);
    for (s.town_index = 0; s.town_index < 4; s.town_index = s.town_index + 1)
    {
      for (s.card_index = 0; s.card_index < 500; s.card_index = s.card_index + 1)
      {
        if ((deck[s.card_index] & 0x4000) == 0 &&
            (deck[s.card_index] & 0xfff) == global_ante_cards[0][s.town_index])
        {
          deck[s.card_index] |= 0x8000;
          break;
        }
      }
    }
    for (s.card_index = 0; s.card_index < 7; s.card_index = s.card_index + 1)
    {
      if (g_selected_wizard_color == -1)
      {
        add_card_to_hand(0, draw_random_unmarked_deck_card());
      }
      else
      {
        add_card_to_hand(0, DrawRandomCardFromInitialLibrary(g_selected_wizard_color));
      }
    }
    if (g_opponent_initial_library_index != -1)
    {
      if (g_selected_wizard_color == -1)
      {
        copy_initial_library_to_player_zero(g_opponent_initial_library_index);
        if (global_ante_cards[1][0] != -1)
        {
          remove_iid_from_initial_library(0, global_ante_cards[1][0]);
        }
      }
      for (s.card_index = 0; s.opening_hand_count > s.card_index; s.card_index = s.card_index + 1)
      {
        add_card_to_hand(1, DrawRandomCardFromInitialLibrary(g_selected_wizard_color != -1));
      }
    }
    evaluate_opening_hand_land_counts(&s.ante_info, &s.ante_result, &s.redraw_result);
    run_duel_coin_flip_dialogs(&s.starting_player,
                               &s.mulligan_accepted,
                               s.starting_player,
                               s.random_starting_player,
                               global_ante_cards[1][0],
                               global_ante_cards[0][0],
                               s.ante_info,
                               s.ante_result,
                               s.redraw_result,
                               &s.shandalar_deck_minimums[5]);
    if (s.mulligan_accepted != 0)
    {
      for (s.card_index = 0; s.card_index < 0x96; s.card_index = s.card_index + 1)
      {
        global_card_instances[0][s.card_index].internal_card_id = -1;
        global_card_instances[0][s.card_index].original_internal_card_id = -1;
      }
      for (s.card_index = 0; s.card_index < 500; s.card_index = s.card_index + 1)
      {
        if (deck[s.card_index] != -1)
        {
          deck[s.card_index] &= -32769;
        }
      }
      for (s.card_index = 0; s.card_index < 500; s.card_index = s.card_index + 1)
      {
        if ((deck[s.card_index] & 0xfff) == global_ante_cards[0][0])
        {
          deck[s.card_index] |= 0x8000;
          break;
        }
      }
      for (s.card_index = 0; s.card_index < 7; s.card_index = s.card_index + 1)
      {
        if (g_selected_wizard_color == -1)
        {
          add_card_to_hand(0, draw_random_unmarked_deck_card());
        }
        else
        {
          add_card_to_hand(0, DrawRandomCardFromInitialLibrary(g_selected_wizard_color));
        }
      }
      TENTATIVE_reassess_all_cards(0, 0x30);
    }
    if (s.ante_result != 0 || (s.mulligan_accepted != 0 && s.redraw_result != 0))
    {
      for (s.card_index = 0; s.card_index < 0x96; s.card_index = s.card_index + 1)
      {
        global_card_instances[1][s.card_index].internal_card_id = -1;
        global_card_instances[1][s.card_index].original_internal_card_id = -1;
      }
      if (g_selected_wizard_color == -1)
      {
        copy_initial_library_to_player_zero(g_opponent_initial_library_index);
        if (global_ante_cards[1][0] != -1)
        {
          remove_iid_from_initial_library(0, global_ante_cards[1][0]);
        }
      }
      for (s.card_index = 0; s.opening_hand_count > s.card_index; s.card_index = s.card_index + 1)
      {
        add_card_to_hand(1, DrawRandomCardFromInitialLibrary(g_selected_wizard_color != -1));
      }
      TENTATIVE_reassess_all_cards(0, 0x30);
    }
    if (g_selected_wizard_color == -1)
    {
      g_opponent_initial_library_index = 0;
    }
    for (s.card_index = 0; s.card_index < 500; s.card_index = s.card_index + 1)
    {
      if (g_selected_wizard_color == -1)
      {
        global_library[0][s.card_index] = draw_random_unmarked_deck_card();
      }
      else
      {
        global_library[0][s.card_index] = DrawRandomCardFromInitialLibrary(g_selected_wizard_color);
      }
      global_library[1][s.card_index] = DrawRandomCardFromInitialLibrary(g_opponent_initial_library_index);
    }
    g_selected_wizard_color = -1;
    if ((g_shandalar_monster_definitions[creature_type].preduel_flags & 2) != 0 && g_monster_timer % 3 == 0)
    {
      memcpy(global_library[1], global_library[0], 1000);
      s.card_index = g_duel_ai_mode_state;
      g_duel_ai_mode_state = 1;
      shuffle_duel_library(0, 1);
      g_duel_ai_mode_state = s.card_index;
      memcpy(global_card_instances[1], global_card_instances[0], sizeof(global_card_instances[1]));
      for (s.card_index = 0; s.card_index < 0x96; s.card_index = s.card_index + 1)
      {
        if (global_card_instances[1][s.card_index].internal_card_id != -1)
        {
          global_card_instances[1][s.card_index].state |= 0x1000;
        }
      }
      g_hand_count_adjustment = 0;
    }
    g_duel_ai_mode_state = 0;
    reset_duel_globals();
    if (g_opponent_starting_card_id_1 != -1)
    {
      s.card_index = add_card_to_hand(1, g_opponent_starting_card_id_1);
      process_card_enters_play(1, s.card_index);
      g_opponent_starting_card_id_1 = -1;
      if (g_next_duel_card_id == -1)
      {
        g_next_duel_card_id = 0;
      }
    }
    if (g_opponent_starting_card_id_2 != -1)
    {
      s.card_index = add_card_to_hand(1, g_opponent_starting_card_id_2);
      process_card_enters_play(1, s.card_index);
      g_opponent_starting_card_id_2 = -1;
      if (g_next_duel_card_id == -1)
      {
        g_next_duel_card_id = 0;
      }
    }
    if (g_next_duel_card_id > 5)
    {
      s.card_index = add_card_to_hand(0, g_next_duel_card_id);
      process_card_enters_play(0, s.card_index);
    }

    goto idk;
  }
  else
  {
    if (TRACE_ENABLED)
    {
      append_to_trace_txt("Starting the duel.\n");
    }
    g_life[1] = 0x14;
    g_life[0] = g_life[1];
    copy_opponent_name_prefix(g_duel_message_text);
    s.opening_hand_count = 7;
    g_hand_count_adjustment = 0;
    g_encounter_color_match_mode = 1;
    g_current_encounter_color = -1;
    SeedRandomFromTickCount();
    if ((g_duel_network_flags & 2) != 0)
    {
      send_battlefield_status_packet(0);
      receive_battlefield_status_packet(1);
    }
    if (g_duel_state_00896718 == -1)
    {
      if ((g_duel_network_flags & 2) != 0)
      {
        s.starting_player = internal_rand(2);
      }
      else
      {
        if (g_shandalar_difficulty != 0 && internal_rand(2) != 0 && g_duel_ai_mode_state == 0)
        {
          s.starting_player = 1;
        }
        else
        {
          s.starting_player = 0;
        }
      }
      s.random_starting_player = 1;
    }
    else
    {
      s.starting_player = g_duel_state_00896718;
      s.random_starting_player = 0;
    }
    if ((g_duel_network_flags & 2) != 0)
    {
      if (g_manalink_is_host != 0)
      {
        g_network_result_packet.packet_type = 1;
        g_network_result_packet.result = 1 - s.starting_player;
        TENTATIVE_send_network_result(0, 1);
      }
      else
      {
        TENTATIVE_wait_for_network_result(1, 1);
        s.starting_player = g_network_result_packet.result;
      }
    }
    g_starting_player_was_random = 1;
    if (g_duel_use_previous_backdrop_colors == 0)
    {
      for (s.loop_5c = 0; s.loop_5c < 0x3c; s.loop_5c = s.loop_5c + 1)
      {
        global_library[1][s.loop_5c] = 0;
        global_library[0][s.loop_5c] = global_library[1][s.loop_5c];
      }
      for (s.loop_5c = 0x3c; s.loop_5c < 500; s.loop_5c = s.loop_5c + 1)
      {
        global_library[1][s.loop_5c] = -1;
        global_library[0][s.loop_5c] = global_library[1][s.loop_5c];
      }
      TENTATIVE_reassess_all_cards(0, 0x30);
      for (s.loop_5c = 0; s.loop_5c < 0x10; s.loop_5c = s.loop_5c + 1)
      {
        global_ante_cards[1][s.loop_5c] = -1;
        global_ante_cards[0][s.loop_5c] = global_ante_cards[1][s.loop_5c];
      }
      if (g_duel_state_00789104 != 0)
      {
        if ((g_duel_network_flags & 2) != 0)
        {
          global_ante_cards[0][0] = DrawRandomCardFromInitialLibrary(g_selected_wizard_color);
          g_network_result_packet.packet_type = 2;
          g_network_result_packet.result = global_ante_cards[0][0];
          TENTATIVE_send_network_result(0, 2);
          TENTATIVE_wait_for_network_result(1, 2);
          global_ante_cards[1][0] = g_network_result_packet.result;
        }
        else
        {
          global_ante_cards[0][0] = DrawRandomCardFromInitialLibrary(g_selected_wizard_color);
          global_ante_cards[1][0] = DrawRandomCardFromInitialLibrary(g_opponent_initial_library_index);
        }
      }
      if ((g_duel_network_flags & 2) != 0)
      {
        for (s.loop_5c = 0; s.loop_5c < 7; s.loop_5c = s.loop_5c + 1)
        {
          s.packet_card = DrawRandomCardFromInitialLibrary(g_selected_wizard_color);
          add_card_to_hand(0, s.packet_card);
          AddCardToCLPacket(s.packet_card);
        }
        AddCardToCLPacket(-1);
        TENTATIVE_send_network_result(s.player_index, 3);
        TENTATIVE_wait_for_network_result(1, 3);
        s.packet_card = 0;
        s.loop_5c = s.packet_card;
        while (s.packet_card != -1)
        {
          s.unused_68 = s.loop_5c;
          s.loop_5c++;
          s.packet_card = GetCardFromCLPacket(s.unused_68);
          if (s.packet_card != -1)
          {
            add_card_to_hand(1, s.packet_card);
          }
        }
      }
      else
      {
        for (s.loop_5c = 0; s.loop_5c < 7; s.loop_5c = s.loop_5c + 1)
        {
          add_card_to_hand(0, DrawRandomCardFromInitialLibrary(g_selected_wizard_color));
          add_card_to_hand(1, DrawRandomCardFromInitialLibrary(g_opponent_initial_library_index));
        }
      }
      evaluate_opening_hand_land_counts(&s.ante_info, &s.ante_result, &s.redraw_result);
      for (s.loop_player = 0; s.loop_player <= 1; s.loop_player++)
      {
        if (s.loop_player == 0)
        {
          s.wizard_color = g_selected_wizard_color;
        }
        else
        {
          s.wizard_color = g_opponent_initial_library_index;
        }
        s.library_count = 0;
        for (s.loop_5c = 0; s.loop_5c < 200; s.loop_5c = s.loop_5c + 1)
        {
          if (g_initial_library[s.wizard_color][s.loop_5c].csvid != -1)
          {
            for (s.quantity = 0; s.quantity < g_initial_library[s.wizard_color][s.loop_5c].numcards;
                 s.quantity = s.quantity + 1)
            {
              global_library[s.loop_player][s.library_count] = 0;
              s.library_count++;
            }
          }
        }
        for (s.loop_5c = s.library_count; s.loop_5c < 500; s.loop_5c = s.loop_5c + 1)
        {
          global_library[s.loop_player][s.loop_5c] = -1;
        }
      }
      run_duel_coin_flip_dialogs(&s.starting_player,
                                 &s.mulligan_accepted,
                                 s.starting_player,
                                 s.random_starting_player,
                                 global_ante_cards[1][0],
                                 global_ante_cards[0][0],
                                 s.ante_info,
                                 s.ante_result,
                                 s.redraw_result,
                                 &s.shandalar_deck_minimums[5]);
      if (s.mulligan_accepted != 0)
      {
        perform_player_mulligan(0, g_selected_wizard_color);
        TENTATIVE_reassess_all_cards(0, 0x30);
      }
      if (((g_duel_network_flags & 2) == 0 && (s.ante_result != 0 || (s.mulligan_accepted != 0 && s.redraw_result != 0))) ||
          ((g_duel_network_flags & 2) != 0 && s.shandalar_deck_minimums[5] != 0))
      {
        perform_player_mulligan(1, g_opponent_initial_library_index);
        TENTATIVE_reassess_all_cards(0, 0x30);
      }
      for (s.loop_player = 0; s.loop_player <= 1; s.loop_player++)
      {
        if ((g_duel_network_flags & 2) && s.loop_player == 1)
        {
          for (s.loop_5c = 0; s.loop_5c < 500; s.loop_5c = s.loop_5c + 1)
          {
            global_library[s.loop_player][s.loop_5c] = -1;
          }

          continue;
        }

        if (s.loop_player == 0)
        {
          s.wizard_color = g_selected_wizard_color;
        }
        else
        {
          s.wizard_color = g_opponent_initial_library_index;
        }
        s.library_count = 0;
        for (s.loop_5c = 0; s.loop_5c < 200; s.loop_5c = s.loop_5c + 1)
        {
          if (g_initial_library[s.wizard_color][s.loop_5c].csvid != -1)
          {
            for (s.quantity = 0; s.quantity < g_initial_library[s.wizard_color][s.loop_5c].numcards;
                 s.quantity = s.quantity + 1)
            {
              global_library[s.loop_player][s.library_count] =
                  CardTypeFromID(g_initial_library[s.wizard_color][s.loop_5c].csvid);
              s.library_count++;
            }
            g_initial_library[s.wizard_color][s.loop_5c].numcards = 0;
          }
        }
        for (s.loop_5c = s.library_count; s.loop_5c < 500; s.loop_5c = s.loop_5c + 1)
        {
          global_library[s.loop_player][s.loop_5c] = -1;
        }
      }
      g_selected_wizard_color = -1;
      s.loop_5c = g_duel_ai_mode_state;
      g_duel_ai_mode_state = 1;
      shuffle_duel_library(0, 0);
      shuffle_duel_library(1, 1);
      g_duel_ai_mode_state = s.loop_5c;
    }
  }

idk:
  reset_duel_globals();

  if (g_duel_ai_mode_state == -1)
  {
    load_selected_duel_save_slot(0);
  }
  if (g_duel_ai_mode_state == -2)
  {
    load_selected_duel_save_slot(1);
  }
#ifdef SHANDALAR
  ShowMouseCursorNested();
#endif
  if (g_duel_ai_mode_state == -10 && 0 < g_duel_run_mode)
  {
    if (g_duel_run_mode == 1)
    {
      load_duel_run_mode_1_save(g_duel_prompt_text_pointer);
    }
    else if (g_duel_run_mode == 2)
    {
      load_duel_run_mode_2_save(g_duel_prompt_text_pointer);
    }
    else if (g_duel_run_mode == 3)
    {
      load_duel_run_mode_3_save(g_duel_prompt_text_pointer);
    }
    TENTATIVE_reassess_all_cards(0, 0xff);
    s.next_player = g_current_player;
  }
  else if (g_duel_ai_mode_state == -10)
  {
    load_gametype0(g_duel_prompt_text_pointer);
    TENTATIVE_reassess_all_cards(0, 0xff);
    s.next_player = g_current_player;
  }
  else if (g_duel_ai_mode_state == -1)
  {
    g_first_turn_draw_pending = 0;
    TENTATIVE_reassess_all_cards(0, 0xff);
    s.next_player = 1;
  }
  else if (g_duel_ai_mode_state == -2)
  {
    g_first_turn_draw_pending = 0;
    TENTATIVE_reassess_all_cards(0, 0xff);
    s.next_player = 0;
  }
  else
  {
    g_first_turn_draw_pending = 1;
    s.next_player = s.starting_player;
  }

#ifndef SHANDALAR
  do
  {
  } while (WaitForSingleObject(global_mutex_GameInit, 0xffffffff) != 0);
#endif
  while (g_adventure_world_exit_requested == 0 && check_duel_finished() == 0)
  {
    if (g_duel_state_0093a848 != 0)
    {
      if ((g_duel_state_0093a848 & 1U) != 0)
      {
        g_duel_state_0093a848 = 0;
        if (s.next_player == 1)
        {
          g_duel_extra_turn_player = -1;
        }
        run_duel_turn(0);
        g_duel_extra_turn_player = -1;
      }
      else
      {
        g_duel_state_0093a848 = 0;
        if (s.next_player == 0)
        {
          g_duel_extra_turn_player = -1;
        }
        run_duel_turn(1);
        g_duel_extra_turn_player = -1;
      }
    }
    else
    {
      run_duel_turn(s.next_player);
      s.next_player = 1 - s.next_player;
    }
  }
#ifndef SHANDALAR
  ReleaseMutex(global_mutex_GameInit);
#else
  HideMouseCursorNested();
#endif

  if (g_one_deck_mode == 0)
  {
    for (s.card_index = 0; s.card_index < 500; s.card_index = s.card_index + 1)
    {
      if (deck[s.card_index] != -1)
      {
        deck[s.card_index] &= -32769;
      }
    }
  }
  else
  {
    OutputDebugStringA("OneDeck ONEDECK ONE DECK\n");
  }

  g_next_duel_card_id = -1;
  g_opponent_starting_card_id_1 = g_next_duel_card_id;
  g_opponent_starting_card_id_2 = g_opponent_starting_card_id_1;
  g_duel_ai_mode_state = 0;
  for (s.card_index = 0; s.card_index < 4; s.card_index = s.card_index + 1)
  {
    g_ai_combat_value_weights[s.card_index] = 8;
  }
  g_duel_active = 0;
  g_duel_state_00742fc0 = 1;
  if (g_life[0] > 0 && g_poison_counters[0] < 10 && (g_life[1] <= 0 || g_poison_counters[1] >= 10))
  {
    return 1;
  }
  if (g_life[1] > 0 && g_poison_counters[1] < 10 && (g_life[0] <= 0 || g_poison_counters[0] >= 10))
  {
    return 0;
  }
  return -1;
}

// FUNCTION: MAGIC 0x004dbe21
// FUNCTION: SHANDALAR 0x00542adc
DWORD WINAPI start_duel_thread(LPVOID unused)
{
  DWORD result;
  int counter;

  (void)unused;
  srand(GetTickCount());
  for (counter = 0; counter < rand() % 50000; counter = counter + 1)
  {
    rand();
  }

  InitializeDuelUiGlobalIds();
  result = play_duel(0, g_duel_creature_type);
  PostMessageA(g_duel_window_hwnd, 0x401, result, 0);
  return result;
}
