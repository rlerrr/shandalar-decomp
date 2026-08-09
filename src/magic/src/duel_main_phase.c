#include <windows.h>
#include <stdio.h>
#include <string.h>

#include "defs.h"
#include "magic/src/game_support.h"
#include "magic/src/global_duel_ui_ids.h"
#include "magic/src/global_other.h"
#include "magic/src/global_state.h"
#include "magic/src/global_strings.h"
#include "magic/src/cards.h"

extern int life[2];
extern char DAT_00896690;
extern int _DAT_0091c0ec;
#ifdef SHANDALAR
char DAT_0091ce40[300];
#else
extern char DAT_0091ce40[300];
#endif
void TENTATIVE_reassess_all_cards(int view_player, int present_after_draw);
void append_to_trace_txt(char *text);
void update_phase_display(int player, phase_t phase);
void reset_trigger_dispatch_state(void);
int player_can_stop_at_phase(int player, phase_t phase);
int allow_response(int response_player, int phase, char *prompt, int event_code);
int dispatch_trigger(int player, trigger_t trig, const char *prompt, int TENTATIVE_allow_response);
int dispatch_trigger_twice_once_with_each_player_as_reason(int reason_for_trig, trigger_t trig, const char *prompt, int a4);
void start_ai_decision_search(int decision_code, int time_scale);
void __stdcall reset_ai_search_trial_state(void);
void resolve_mana_burn(void);
int check_duel_finished(void);
void show_opponent_taunt(char *text);
int player_has_legal_attacker(int player);
int human_has_phase_stop(phase_t phase);
int should_skip_phase(int player);
int select_card_for_action(int player, int a2, int a3, unsigned int a4, unsigned int a5, char *prompt, int a7);
int update_attacker_count_and_check_combat_done(int player);
int process_response_actions(int player, const char *prompt);
int choose_ai_main_phase_action(int player);
int put_card_on_stack(int player, int card, int pay_costs);
int resolve_card_on_stack(int player, int card);
int dispatch_event_to_single_card(int player, int card, int event, int a4, int a5);
int activate(int player, int a2, int card);
void resolve_activated_ability(int player, int card);
void request_duel_display_refresh_if_human(int player, int card, int a3, int a4);
int can_attack(int player, int card);
int get_recorded_action_count(void);
void push_affected_card_stack(void);
void pop_affected_card_stack(void);
int C_get_abilities(int player, int card, int event, int a4);
void set_duel_prompt_text(char *text);
void process_damage_prevention(int player);
unsigned int choose_attackers_ai(int player);
void choose_blockers_human(int player);
void setup_ai_combat_abilities(int player);
void choose_blockers_ai(int player);
void mark_blocked_attackers(int player);
void resolve_combat_damage(int player);
int cleanup_combat_state(int player);
void reassess_all_cards_and_mana(void);
int is_in_play(int player, int card);
int has_mana(int player, color_t color, int amount);
int player_has_available_blocker(int player);
int assign_blocker_to_attacker(int blocker_player, int blocker_card, int attacker_player, int attacker_card);
void get_landwalk_evasion_masks(unsigned int *out_landwalk_mask, unsigned int *out_basic_land_mask);
int can_block_attacker_with_abilities(int blocker_player, int blocker_card, int attacker_player, int attacker_card, unsigned int attacker_abilities,
                                      unsigned int evasion_mask);
int play_sound_effect(int sound_id);
void save_combat_simulation_state(void);
void restore_combat_simulation_state(void);
void setup_combat_damage_simulation(int player);
int score_gamestate_after_simulated_card_death(int player, int card);
void dispatch_ai_combat_pump_checks(int player);
int score_ai_blocker_candidate(int player, int card);
unsigned int get_destroy_if_blocked_combat_flags(int attacker_player, int attacker_card, int blocker_player, int blocker_card);

extern int combat_damage_attacker_abilities[16];
extern int combat_damage_attacker_cards[16];
extern int combat_damage_attacker_count;
extern int combat_damage_attacker_damage[16];
extern int combat_damage_attacker_toughness[16];
extern int combat_damage_blocker_abilities[16];
extern int combat_damage_blocker_damage[16];
extern int combat_damage_blocker_toughness[16];

// GLOBAL: MAGIC 0x00637804
// GLOBAL: SHANDALAR 0x005a5ed4
int main_phase_selected_internal_card_id;

// GLOBAL: MAGIC 0x00637808
// GLOBAL: SHANDALAR 0x005a5ed8
char DAT_00637808[0x100];

// GLOBAL: MAGIC 0x00637908
// GLOBAL: SHANDALAR 0x005a5fd8
int main_phase_resume_mode;

// GLOBAL: MAGIC 0x0063790c
// GLOBAL: SHANDALAR 0x005a5fdc
int main_phase_selected_bandmate_card;

// GLOBAL: MAGIC 0x00637910
// GLOBAL: SHANDALAR 0x005a5fe0
int main_phase_selected_card;

// GLOBAL: MAGIC 0x0093f4ac
// GLOBAL: SHANDALAR 0x0097f1a8
int legal_attacker_count;

// FUNCTION: MAGIC 0x0044118f
// FUNCTION: SHANDALAR 0x0040ca0f
int player_has_legal_attacker(int player)
{
  int card;
  for (card = 0; active_cards_count[player] > card;
       card++)
  {
    if ((global_card_instances[player][card].internal_card_id == -1) ||
        ((global_card_instances[player][card].state & (STATE_IN_PLAY | STATE_ATTACKING)) == 0))
    {
      continue;
    }

    if (can_attack(player, card))
    {
      return 1;
    }
  }
  return 0;
}

// FUNCTION: MAGIC 0x00441565
// FUNCTION: SHANDALAR 0x0040cde5
int update_attacker_count_and_check_combat_done(int player)
{
  int card;

  attacking_creature_count = 0;
  legal_attacker_count = 0;
  for (card = 0; card < active_cards_count[player]; card++)
  {
    if (is_in_play(player, card) != 0)
    {
      if ((global_card_instances[player][card].state & STATE_ATTACKING))
      {
        attacking_creature_count = 1;
      }
      else if (global_card_instances[player][card].state & STATE_UNKNOWN8000)
      {
        if (can_attack(player, card) != 0)
        {
          legal_attacker_count++;
        }
      }
    }
  }

  if (attacking_creature_count != 0 || legal_attacker_count != 0)
    return 0;

  return 1;
}

// FUNCTION: MAGIC 0x00441688
// FUNCTION: SHANDALAR 0x0040cf08
int player_has_available_blocker(int player)
{
  struct
  {
    int blocker;
    int attacking_player;
    unsigned int attacker_abilities;
    unsigned int evasion_mask;
    unsigned int player_0_evasion_mask;
    int attacker;
    unsigned int player_1_evasion_mask;
  } s;

  s.attacking_player = 1 - player;
  get_landwalk_evasion_masks(&s.player_1_evasion_mask, &s.player_0_evasion_mask);
  if (player == 1)
  {
    s.evasion_mask = s.player_1_evasion_mask;
  }
  else
  {
    s.evasion_mask = s.player_0_evasion_mask;
  }
  s.attacker = 0;
  do
  {
    if (active_cards_count[s.attacking_player] <= s.attacker)
    {
      return 0;
    }
    if ((global_card_instances[s.attacking_player][s.attacker].internal_card_id != -1) &&
        ((global_card_instances[s.attacking_player][s.attacker].state & STATE_ATTACKING) != 0))
    {
      s.attacker_abilities = C_get_abilities(s.attacking_player, s.attacker, EVENT_ABILITIES, -1);
      for (s.blocker = 0; s.blocker < active_cards_count[player]; s.blocker++)
      {
        if (((global_card_instances[s.attacking_player][s.attacker].internal_card_id != -1) &&
             ((global_card_instances[player][s.blocker].state & (STATE_IN_PLAY | STATE_BLOCKING | STATE_TAPPED)) == STATE_IN_PLAY)) &&
            (can_block_attacker_with_abilities(player, s.blocker, s.attacking_player, s.attacker, s.attacker_abilities, s.evasion_mask) != 0))
        {
          return 1;
        }
      }
    }
    s.attacker++;
  } while (1);
}

// FUNCTION: MAGIC 0x00440d82
// FUNCTION: SHANDALAR 0x0040c602
void choose_blockers_human(int player)
{
  struct
  {
    int attacking_cards[16];
    int attacking_card_abilities[16];
    int attacking_player_copy;
    int best_attacker_abilities;
    int attacker_abilities;
    int attacker;
    char attacker_blocking_group;
    int internal_card_id;
    int attacker_count;
    int defending_player;
    target_t selected_blocker;
    int done;
    target_t blocked_attacker;
  } s;

  if (g_duel_ai_mode_state != 1)
  {
    TENTATIVE_reassess_all_cards(0, 0xff);
    s.attacking_player_copy = player;
    s.defending_player = 1 - player;
    s.best_attacker_abilities = 0;
    s.attacker_count = 0;
    for (s.attacker = 0; s.attacker < active_cards_count[player]; s.attacker++)
    {
      s.internal_card_id = global_card_instances[player][s.attacker].internal_card_id;
      if ((s.internal_card_id != -1) && ((global_card_instances[player][s.attacker].state & STATE_ATTACKING) != 0))
      {
        s.attacker_abilities = C_get_abilities(player, s.attacker, 0x32, -1);
        s.attacking_cards[s.attacker_count] = s.attacker;
        s.attacking_card_abilities[s.attacker_count] = s.attacker_abilities;
        s.attacker_count++;
        if (s.best_attacker_abilities < s.attacker_abilities)
        {
          s.best_attacker_abilities = s.attacker_abilities;
        }
      }
    }
    s.done = 0;
    while ((s.done == 0) && (player_has_available_blocker(s.defending_player) != 0))
    {
      load_text(global_ui_strings_filename, "PROMPT_CHOOSEBLOCKERS");
      if (C_real_select_target(s.defending_player, s.defending_player, s.defending_player, 0x2200, 0x2000002, 0, 0, 0, 0, 0,
                               -1, -1, -1, -1, 0x400, 0, 0x11, text_lines[0], 2, &s.selected_blocker) == 0)
      {
        s.done = 1;
      }
      else
      {
        combat_assignment_cancelled = 0;
        if ((battlefield_extra_ability_flags & 0x800000) != 0)
        {
          push_affected_card_stack();
          trigger_cause_controller = s.selected_blocker.player;
          trigger_cause = s.selected_blocker.card;
          dispatch_trigger(s.defending_player, TRIGGER_PAY_TO_BLOCK, gs_pay_for_blocker_009263f0, 1);
          pop_affected_card_stack();
        }
        if ((combat_assignment_cancelled == 0) &&
            (C_real_select_target(s.defending_player, s.attacking_player_copy, s.attacking_player_copy, 0x200, 2, 0, 0, 0,
                                  0, 0, -1, -1, -1, -1, 0, 2, 0, text_lines[1], 1, &s.blocked_attacker) != 0))
        {
          if (assign_blocker_to_attacker(s.selected_blocker.player, s.selected_blocker.card, s.blocked_attacker.player, s.blocked_attacker.card) != 0)
          {
            s.attacker_blocking_group = global_card_instances[s.blocked_attacker.player][s.blocked_attacker.card].blocking;
            if ((int)s.attacker_blocking_group == -1)
            {
              global_card_instances[s.selected_blocker.player][s.selected_blocker.card].blocking = (unsigned char)s.blocked_attacker.card;
            }
            else
            {
              global_card_instances[s.selected_blocker.player][s.selected_blocker.card].blocking = (unsigned char)s.attacker_blocking_group;
            }
            global_card_instances[s.selected_blocker.player][s.selected_blocker.card].state |= STATE_BLOCKING;
            if ((g_duel_ai_mode_state != 1) && ((int)s.attacker_blocking_group == -1))
            {
              play_sound_effect(0x15);
            }
            TENTATIVE_reassess_all_cards(0, 0xff);
            if ((battlefield_extra_ability_flags & 0x100000) != 0)
            {
              trigger_cause_controller = s.selected_blocker.player;
              trigger_cause = s.selected_blocker.card;
              dispatch_trigger_twice_once_with_each_player_as_reason(current_player, TRIGGER_BLOCKER_CHOSEN, gs_blocker_selected_00926210, 0);
            }
          }
          else if (g_duel_ai_mode_state != 1)
          {
            load_text(global_ui_strings_filename, "PROMPT_CHOOSEBLOCKERS");
            set_duel_prompt_text(text_lines[2]);
            Sleep(2000);
            set_duel_prompt_text("");
          }
        }
      }
    }
  }
}

// FUNCTION: MAGIC 0x00445d1a
// FUNCTION: SHANDALAR 0x004115b3
void mark_blocked_attackers(int player)
{
  struct
  {
    int is_blocked;
    int card;
    int opponent;
    int attacker;
    int internal_card_id;
    int blocker_internal_card_id;
  } s;

  s.opponent = 1 - player;
  for (s.attacker = 0; s.attacker < active_cards_count[player]; s.attacker++)
  {
    s.internal_card_id = global_card_instances[player][s.attacker].internal_card_id;
    if ((s.internal_card_id != -1) &&
        ((global_card_instances[player][s.attacker].state & (STATE_OUBLIETTED | STATE_IN_PLAY | STATE_ATTACKING)) == (STATE_IN_PLAY | STATE_ATTACKING)))
    {
      s.is_blocked = 0;
      for (s.card = 0; s.card < active_cards_count[s.opponent]; s.card++)
      {
        s.blocker_internal_card_id = global_card_instances[s.opponent][s.card].internal_card_id;
        if ((s.blocker_internal_card_id != -1) &&
            ((char)global_card_instances[s.opponent][s.card].blocking == s.attacker))
        {
          s.is_blocked = 1;
        }
      }
      if (s.is_blocked == 0)
      {
        continue;
      }
      for (s.card = 0; s.card < active_cards_count[player]; s.card++)
      {
        if ((s.attacker == s.card) ||
            (((char)global_card_instances[player][s.card].blocking == s.attacker) &&
             ((global_card_instances[player][s.card].state & (STATE_OUBLIETTED | STATE_IN_PLAY | STATE_ATTACKING)) ==
              (STATE_IN_PLAY | STATE_ATTACKING))))
        {
          global_card_instances[player][s.card].state |= STATE_ISBLOCKED;
        }
      }
    }
  }
}

// FUNCTION: MAGIC 0x0044ae9e
// FUNCTION: SHANDALAR 0x00403818
int human_has_phase_stop(phase_t phase)
{
  if ((char)g_duel_phase_stop_settings[current_player].phase_flags[phase] != 0)
  {
    return 1;
  }
  if ((stop_phase == phase) && (stop_phase_player == current_player))
  {
    return 1;
  }
  if ((previous_stop_phase == phase) && (previous_stop_phase_player == current_player))
  {
    return 1;
  }
  return 0;
}

// FUNCTION: MAGIC 0x0044cda7
// FUNCTION: SHANDALAR 0x0040571e
int main_phase(unsigned int player, int phase_mode, int *phase_value)
{
  struct
  {
    char trace_combat[100];
    char trace_blockers_fast_effects[100];
    char trace_choose_blockers[100];
    char trace_attackers_fast_effects[100];
    char trace_choose_attackers[100];
    int bandmate_count;
    int bandmate_index;
    int bandmates[150];
    char trace_assign_attacker[100];
    int activation_result;
    char trace_nonphasing_postcombat_fast_effects[100];
    char trace_no_attackers_nonphasing_fast_effects[100];
    char trace_nonphasing_precombat_fast_effects[100];
    char trace_pick_card[100];
    char trace_skip_pick_card[100];
    char trace_enter_phase[100];
    int player_can_stop_here;
    target_t selected_bandmate;
    int human_stop_enabled;
    int other_player;
    int allow_response_result;
    int retry_phase_prompt;
    int skip_phase;
    int saved_response_state;
  } s;

  s.other_player = 1 - player;
  if (TRACE_ENABLED)
  {
    sprintf(s.trace_enter_phase, "%d: Entering Main Phase.\n", duel_trace_counter++);
    append_to_trace_txt(s.trace_enter_phase);
  }
  switch (phase_mode)
  {
  case 1:
    goto resume_main_phase_response_window;
  case 2:
    goto resume_after_blockers_response;
  case 4:
    goto resume_cast_selected_card;
  case 6:
    goto resume_choose_attackers;
  case 7:
    goto resume_activate_selected_card;
  case 8:
    goto resume_ai_attackers_response;
  case 3:
  case 5:
  default:
    break;
  }

  attacking_creature_count = 0;
  land_can_be_played &= -512;
  current_phase = PHASE_MAIN1;
  update_phase_display(player, current_phase);
  phase_was_skipped = 0;
  phase_response_window_open = 0;
  if (g_duel_ai_mode_state != 1)
  {
    phase_stop_suppressed = 0;
  }
restart_active_main_phase_ai_prompt:
  if ((player == other_player) && ((g_duel_network_flags & 2) == 0))
  {
    TENTATIVE_reassess_all_cards(0, 0xff);
    start_ai_decision_search(1, ((current_phase < PHASE_MAIN2) - 1 & 0xffffffd3) + 0x5a);
    ai_search_target_depth = 0;
  }
resume_main_phase_response_window:

  phase_response_window_open = 1;
  _DAT_0091c0ec = 0;
  if ((player == other_player) && ((g_duel_network_flags & 2) == 0))
  {
    reset_ai_search_trial_state();
    DAT_008cdab4 = 0;
    DAT_008cdab0 = DAT_008cdab4;
    DAT_008a8de4 = DAT_008cdab0;
    ai_modifier = DAT_008a8de4;
    if ((g_duel_ai_mode_state != 1) && (((land_can_be_played & 0x40U) == 0 && (DAT_00896690 != '\0'))))
    {
      show_opponent_taunt(&DAT_00896690);
      land_can_be_played |= 0x40;
      DAT_00896690 = '\0';
    }
  }
restart_main_phase_action_loop:
  *phase_value = 0;
  main_phase_resume_mode = 6;
  do
  {
    DAT_0093d850 = 0;
    if ((((player == other_player) && ((g_duel_network_flags & 2) == 0)) && (g_duel_ai_mode_state != 1)) &&
        ((DAT_00925d30 & 2U) != 0))
    {
      DAT_00925d30 = 0;
      goto restart_active_main_phase_ai_prompt;
    }
    unk_00712938 = 0;
    if ((g_duel_ai_mode_state != 1) && (DAT_00896690 != '\0'))
    {
      show_opponent_taunt(&DAT_00896690);
    }
    if ((player == active_player) || ((g_duel_network_flags & 2) != 0))
    {
      if (spell_fizzled == 1)
      {
        spell_fizzled = 0;
      }
      do
      {
        s.retry_phase_prompt = 1;
        land_can_be_played |= 0x80;
        strcpy(g_ui_message_buffer, "");
        if (current_phase <= PHASE_MAIN1)
        {
          strcpy(g_ui_message_buffer, gs_prompt_main_phase_precombat_cast_spells_008cc710);
          if ((land_can_be_played & 1U) == 0)
          {
            strcpy(g_ui_message_buffer, gs_prompt_main_phase_precombat_cast_spells_play_land_00777970);
          }
        }
        else if (current_phase >= PHASE_MAIN2)
        {
          strcpy(g_ui_message_buffer, gs_prompt_main_phase_postcombat_cast_spells_007ab2d0);
          if ((land_can_be_played & 1U) == 0)
          {
            strcpy(g_ui_message_buffer, gs_prompt_main_phase_postcombat_cast_spells_play_land_0091cf50);
          }
        }
        else
        {
          if (player == current_player)
          {
            strcpy(g_ui_message_buffer, gs_prompt_combat_choose_attackers_0093d990);
          }
          else
          {
            strcpy(g_ui_message_buffer, gs_prompt_combat_choose_blockers_008b4140);
          }
          if ((player_has_legal_attacker(player) == 0) && (human_has_phase_stop(PHASE_DECLARE_ATTACKERS) == 0))
          {
            current_phase = PHASE_MAIN2;
            s.retry_phase_prompt = 0;
          }
        }
      } while (s.retry_phase_prompt == 0);
      do
      {
        s.retry_phase_prompt = 1;
        reset_trigger_dispatch_state();
        phase_response_window_open = 1;
        phase_stop_suppressed = 0;
        if ((g_duel_network_flags & 2) != 0)
        {
          s.skip_phase = should_skip_phase(player);
          s.player_can_stop_here = player_can_stop_at_phase(player, current_phase);
        }
        if ((should_skip_phase(player) != 0 && (current_phase != PHASE_DECLARE_ATTACKERS || legal_attacker_count == 0)) ||
            (should_skip_phase(player) == 0 && current_phase == PHASE_MAIN2 && player_can_stop_at_phase(player, current_phase) == 0))
        {

          g_target_selection_status_code = -2;
          main_phase_selected_card = 0xffffffff;
          if (TRACE_ENABLED)
          {
            sprintf(s.trace_skip_pick_card, "%d: Main Phase skipping PickACard. SkipPhase:%d ThePhase:%d mustattackers:%d UserWantsToStop:%d\n", duel_trace_counter++, s.skip_phase,
                    current_phase, legal_attacker_count, s.player_can_stop_here);
            append_to_trace_txt(s.trace_skip_pick_card);
          }
        }
        else
        {
          main_phase_selected_card = select_card_for_action(player, player, player, 0, 0, g_ui_message_buffer, 2);
          if (TRACE_ENABLED)
          {
            sprintf(s.trace_pick_card, "%d: Main Phase PickACard: ThePhase:%d Player:%d card:%d\n", duel_trace_counter++, current_phase,
                    player, main_phase_selected_card);
            append_to_trace_txt(s.trace_pick_card);
          }
        }

        if (main_phase_selected_card == 0xfffffffe)
        {
          if (player == active_player)
          {
            stop_phase_player = -1;
            stop_phase = stop_phase_player;
          }
          else
          {
            previous_stop_phase_player = -1;
            previous_stop_phase = previous_stop_phase_player;
          }
          main_phase_selected_card = 0xffffffff;
        }

        if ((current_phase == PHASE_DECLARE_ATTACKERS) && ((int)main_phase_selected_card < 0))
        {
          update_attacker_count_and_check_combat_done(player);
          if (legal_attacker_count != 0)
          {
            s.retry_phase_prompt = 0;
          }
        }
      } while (s.retry_phase_prompt == 0);

      if ((g_target_selection_status_code == -2) && (g_duel_ai_mode_state != 1))
      {
        if (current_phase < PHASE_AFTER_BLOCKING)
        {
          if ((stop_phase > PHASE_AFTER_BLOCKING) || (previous_stop_phase > PHASE_AFTER_BLOCKING))
          {
            phase_stop_suppressed = 1;
          }
          else
          {
            phase_stop_suppressed = 0;
          }
        }
        if (current_phase < PHASE_BEFORE_BLOCKING)
        {
          if (stop_phase > PHASE_BEFORE_BLOCKING)
          {
            phase_stop_suppressed = 1;
          }
          else
          {
            phase_stop_suppressed = 0;
          }
        }
      }
      land_can_be_played &= -129;
      if ((g_target_selection_status_code == -2) &&
          ((((stop_phase_player == -1 || (stop_phase != -1)) || (previous_stop_phase_player == -1)) ||
            (previous_stop_phase != -1))))
      {
        if (current_phase <= PHASE_MAIN1)
        {
          if (battlefield_extra_ability_flags & 0x1000000)
          {
            C_dispatch_event_raw(0x89);
          }
          resolve_mana_burn();
          if (check_duel_finished() != 0)
          {
            return 1;
          }
          update_phase_display(player, current_phase);
          if (!((attacking_creature_count == 0) &&
                ((player_has_legal_attacker(player) == 0) || (should_skip_phase(player) != 0)) &&
                ((active_player != stop_phase_player) || (stop_phase != PHASE_DECLARE_ATTACKERS)) &&
                ((other_player != previous_stop_phase_player) || (previous_stop_phase != PHASE_DECLARE_ATTACKERS)) &&
                (((char)g_duel_phase_stop_settings[player].phase_flags[PHASE_DECLARE_ATTACKERS] & (PHASE_STOP_ENABLED | PHASE_STOP_OPPONENT)) == 0)))
          {
            if ((battlefield_extra_ability_flags & 0x2000000) != 0)
            {
              C_dispatch_event_raw(0x92);
            }
            if ((g_duel_network_flags & 2) != 0)
            {
              DAT_00789714 = 1;
              _DAT_00743020 = 0xffffffff;
              spell_fizzled = 0;
              unk_008b3270 = spell_fizzled;
              s.saved_response_state = DAT_00777aa8;
              reset_trigger_dispatch_state();
              phase_response_window_open = 1;
              phase_stop_suppressed = 0;
              s.human_stop_enabled = 0;
              if (((active_player == current_player) &&
                   (((char)g_duel_phase_stop_settings[current_player].phase_flags[current_phase] & PHASE_STOP_OPPONENT) != 0)) ||
                  ((other_player == current_player &&
                    (((char)g_duel_phase_stop_settings[current_player].phase_flags[current_phase] & PHASE_STOP_ENABLED) != 0))))
              {
                s.human_stop_enabled = 1;
              }

              if ((should_skip_phase(player) == 0) &&
                  (((s.human_stop_enabled != 0 ||
                     ((current_player == previous_stop_phase_player && (previous_stop_phase == current_phase)))) ||
                    ((current_player == stop_phase_player && (stop_phase == current_phase))))))
              {
                DAT_00777aa8 = 1;
              }
              else
              {
                DAT_00777aa8 = 0;
              }
              if (DAT_00777aa8 != 0)
              {
                sprintf(s.trace_nonphasing_precombat_fast_effects, "%d: Main Phase Non-Phasing player's Fast Effects: ThePhase:%d\n", duel_trace_counter++, current_phase);
                append_to_trace_txt(s.trace_nonphasing_precombat_fast_effects);
                if (process_response_actions(1 - current_player,
                                             gs_prompt_main_phase_precombat_cast_spells_008cc710) != 0)
                {
                  DAT_00777aa8 = s.saved_response_state;
                  phase_response_window_open = 0;
                  phase_stop_suppressed = 0;
                  goto resume_main_phase_response_window;
                }
              }
              DAT_00777aa8 = s.saved_response_state;
              phase_response_window_open = 0;
              phase_stop_suppressed = 0;
            }
            current_phase = PHASE_DECLARE_ATTACKERS;
            if (((active_player == current_player) && (active_player == stop_phase_player)) &&
                (stop_phase == PHASE_DECLARE_ATTACKERS))
            {
              g_duel_phase_stop_settings[active_player].phase_flags[PHASE_DECLARE_ATTACKERS] = (char)g_duel_phase_stop_settings[active_player].phase_flags[PHASE_DECLARE_ATTACKERS] | PHASE_STOP_SUPPRESSED;
              g_duel_phase_stop_settings[active_player].phase_flags[PHASE_BEFORE_BLOCKING] = (char)g_duel_phase_stop_settings[active_player].phase_flags[PHASE_BEFORE_BLOCKING] | PHASE_STOP_SUPPRESSED;
              g_duel_phase_stop_settings[active_player].phase_flags[PHASE_AFTER_BLOCKING] = (char)g_duel_phase_stop_settings[active_player].phase_flags[PHASE_AFTER_BLOCKING] | PHASE_STOP_SUPPRESSED;
            }
            if ((((g_duel_network_flags & 2) != 0) && (other_player == current_player)) &&
                ((other_player == previous_stop_phase_player && (previous_stop_phase == PHASE_DECLARE_ATTACKERS))))
            {
              g_duel_phase_stop_settings[other_player].phase_flags[PHASE_DECLARE_ATTACKERS] = (char)g_duel_phase_stop_settings[other_player].phase_flags[PHASE_DECLARE_ATTACKERS] | PHASE_STOP_SUPPRESSED;
              g_duel_phase_stop_settings[other_player].phase_flags[PHASE_BEFORE_BLOCKING] = (char)g_duel_phase_stop_settings[other_player].phase_flags[PHASE_BEFORE_BLOCKING] | PHASE_STOP_SUPPRESSED;
              g_duel_phase_stop_settings[other_player].phase_flags[PHASE_AFTER_BLOCKING] = (char)g_duel_phase_stop_settings[other_player].phase_flags[PHASE_AFTER_BLOCKING] | PHASE_STOP_SUPPRESSED;
            }
            if ((should_skip_phase(player) != 0) &&
                (attacking_creature_count == 0) &&
                (update_attacker_count_and_check_combat_done(player) != 0))
            {
              break;
            }
            update_phase_display(player, current_phase);
            if (((player == active_player) || ((g_duel_network_flags & 2) != 0)) && (g_duel_ai_mode_state != 1))
            {
              strcpy(g_ui_message_buffer, gs_prompt_combat_choose_attackers_0093d990);
            }

            continue;
          }
          else
          {
            if ((g_duel_network_flags & 2) != 0)
            {
              DAT_00789714 = 1;
              _DAT_00743020 = 0xffffffff;
              spell_fizzled = 0;
              unk_008b3270 = spell_fizzled;
              s.saved_response_state = DAT_00777aa8;
              reset_trigger_dispatch_state();
              phase_response_window_open = 1;
              phase_stop_suppressed = 0;
              s.human_stop_enabled = 0;
              if (((active_player == current_player) &&
                   (((char)g_duel_phase_stop_settings[current_player].phase_flags[current_phase] & PHASE_STOP_OPPONENT) != 0)) ||
                  ((other_player == current_player &&
                    (((char)g_duel_phase_stop_settings[current_player].phase_flags[current_phase] & PHASE_STOP_ENABLED) != 0))))
              {
                s.human_stop_enabled = 1;
              }
              if ((should_skip_phase(player) == 0) &&
                  (((s.human_stop_enabled != 0 ||
                     ((current_player == previous_stop_phase_player && (previous_stop_phase == current_phase)))) ||
                    ((current_player == stop_phase_player && (stop_phase == current_phase))))))
              {
                DAT_00777aa8 = 1;
              }
              else
              {
                DAT_00777aa8 = 0;
              }
              if (DAT_00777aa8 != 0)
              {
                sprintf(s.trace_no_attackers_nonphasing_fast_effects, "%d: Main Phase (No attackers) Non-Phasing player's Fast Effects: ThePhase:%d\n", duel_trace_counter++, current_phase);
                append_to_trace_txt(s.trace_no_attackers_nonphasing_fast_effects);
                if (process_response_actions(1 - current_player,
                                             gs_prompt_main_phase_precombat_cast_spells_008cc710) != 0)
                {
                  DAT_00777aa8 = s.saved_response_state;
                  phase_response_window_open = 0;
                  phase_stop_suppressed = 0;
                  goto resume_main_phase_response_window;
                }
              }
              DAT_00777aa8 = s.saved_response_state;
              phase_response_window_open = 0;
              phase_stop_suppressed = 0;
            }
          }
        }
        else
        {
          if (current_phase < PHASE_MAIN2)
          {
            strcpy(g_ui_message_buffer, "");
            if ((0 < attacking_creature_count) || (update_attacker_count_and_check_combat_done(player) == 0))
            {
              *phase_value = 1;
            }
          }
          else
          {
            if ((g_duel_network_flags & 2) != 0)
            {
              DAT_00789714 = 1;
              _DAT_00743020 = 0xffffffff;
              spell_fizzled = 0;
              unk_008b3270 = spell_fizzled;
              s.saved_response_state = DAT_00777aa8;
              reset_trigger_dispatch_state();
              phase_response_window_open = 1;
              phase_stop_suppressed = 0;
              s.human_stop_enabled = 0;
              if (((active_player == current_player) &&
                   (((char)g_duel_phase_stop_settings[current_player].phase_flags[current_phase] & PHASE_STOP_OPPONENT) != 0)) ||
                  ((other_player == current_player &&
                    (((char)g_duel_phase_stop_settings[current_player].phase_flags[current_phase] & PHASE_STOP_ENABLED) != 0))))
              {
                s.human_stop_enabled = 1;
              }
              if ((should_skip_phase(player) == 0) &&
                  (((s.human_stop_enabled != 0 ||
                     ((current_player == previous_stop_phase_player && (previous_stop_phase == current_phase)))) ||
                    ((current_player == stop_phase_player && (stop_phase == current_phase))))))
              {
                DAT_00777aa8 = 1;
              }
              else
              {
                DAT_00777aa8 = 0;
              }
              if (TRACE_ENABLED)
              {
                sprintf(s.trace_nonphasing_postcombat_fast_effects, "%d: Main Phase Non-Phasing player's Fast Effects: ThePhase:%d\n", duel_trace_counter++, current_phase);
                append_to_trace_txt(s.trace_nonphasing_postcombat_fast_effects);
              }
              if (process_response_actions(1 - current_player,
                                           gs_prompt_main_phase_postcombat_cast_spells_007ab2d0) != 0)
              {
                DAT_00777aa8 = s.saved_response_state;
                phase_response_window_open = 0;
                phase_stop_suppressed = 0;
                goto resume_main_phase_response_window;
              }
              DAT_00777aa8 = s.saved_response_state;
              phase_response_window_open = 0;
              phase_stop_suppressed = 0;
            }
            goto finish_main_phase;
          }
        }
        break;
      }
    }
    else
    {
      reset_trigger_dispatch_state();
      unk_00712938 = 2;
      main_phase_selected_card = choose_ai_main_phase_action(player);
      if (main_phase_selected_card == 0xffffffff)
      {
        if (g_duel_ai_mode_state != 1)
        {
          allow_response(0, current_phase, gs_main_phase_008cee80, current_phase);
        }
        if (current_phase == PHASE_MAIN1)
        {
          current_phase = PHASE_DECLARE_ATTACKERS;
        }
        else
        {
          current_phase = PHASE_MAIN2;
        }
        update_phase_display(player, current_phase);
        resolve_mana_burn();
        if (check_duel_finished() != 0)
        {
          return 1;
        }
        *phase_value = 1;
      }
    }
    unk_00712938 = 3;
    main_phase_resume_mode = 0;
    if (main_phase_selected_card != 0xffffffff)
    {
      main_phase_selected_internal_card_id = global_card_instances[player][main_phase_selected_card].internal_card_id;
      if ((global_card_instances[player][main_phase_selected_card].state & 0x12) == 0)
      {
        if (((global_cards_data[main_phase_selected_internal_card_id].type & 1) != 0) && ((land_can_be_played & 1U) != 0))
        {
          if (g_duel_ai_mode_state == 1)
          {
            *phase_value = 1;
          }
          goto check_main_phase_abort;
        }

        if (current_phase == PHASE_DECLARE_ATTACKERS)
        {
          goto check_main_phase_abort;
        }

        x_value = 0;
        current_spell_player = -1;
        if (put_card_on_stack(player, main_phase_selected_card, 0) != 0)
        {
          if (((player == active_player) && ((g_duel_network_flags & 2) == 0)) &&
              ((current_phase != PHASE_DECLARE_ATTACKERS && ((global_cards_data[main_phase_selected_internal_card_id].type & 1) == 0))))
          {
            start_ai_decision_search(4, 0x1e);
          }
        resume_cast_selected_card:
          if (ai_decision_code == 4)
          {
            reset_ai_search_trial_state();
            DAT_008cdab4 = 0;
            DAT_008cdab0 = DAT_008cdab4;
            DAT_008a8de4 = DAT_008cdab0;
            ai_modifier = DAT_008a8de4;
          }
          if (put_card_on_stack(player, main_phase_selected_card, 1) != 0)
          {
            main_phase_selected_internal_card_id = global_card_instances[player][main_phase_selected_card].internal_card_id;
            if (resolve_card_on_stack(player, main_phase_selected_card) != 0)
            {
              main_phase_resume_mode = 6;
              if ((global_cards_data[main_phase_selected_internal_card_id].type & 1) != 0)
              {
                if (player == active_player)
                {
                  ai_search_time_limit = 0;
                }
                land_can_be_played |= 1;
              }
              if ((global_cards_data[main_phase_selected_internal_card_id].type & 2) != 0)
              {
                global_card_instances[player][main_phase_selected_card].state |= 0x400;
              }
            }
          }
          if ((((player == other_player) && ((g_duel_network_flags & 2) == 0)) &&
               (g_duel_ai_mode_state != 1)) &&
              (ai_action_replay_available == 0))
            goto restart_active_main_phase_ai_prompt;
          if ((g_duel_ai_mode_state == 1) && (ai_decision_code == 4))
          {
            *phase_value = 1;
          }
        }
      }
      else if (((current_phase <= PHASE_MAIN1) || (current_phase >= PHASE_MAIN2)) ||
               ((other_player == player && ((g_duel_network_flags & 2) == 0))))
      {
        max_x_value = -1;
        ai_recorded_action = -1;
        if (((((global_cards_data[main_phase_selected_internal_card_id].extra_ability & 3) != 0) &&
              ((global_card_instances[player][main_phase_selected_card].state & 0x24) == 0)) ||
             ((global_cards_data[main_phase_selected_internal_card_id].extra_ability & 0x1000) != 0)) &&
            (dispatch_event_to_single_card(player, main_phase_selected_card, 0x73, s.other_player, -1) != 0))
        {
          if (((player == other_player) && ((g_duel_network_flags & 2) == 0)) &&
              (g_duel_ai_mode_state != 1))
          {
            stop_phase_player = current_player;
            stop_phase = current_phase;
          }
          if (g_duel_ai_mode_state != 1)
          {
            spell_fizzled = -1;
          }
          s.activation_result = activate(player, player, main_phase_selected_card);
          max_x_value = -1;
          if (s.activation_result != 0)
          {
            if ((((g_duel_network_flags & 2) == 0) && (g_duel_ai_mode_state != 1)) &&
                ((player == active_player &&
                  ((current_phase != PHASE_DECLARE_ATTACKERS &&
                    ((global_cards_data[global_card_instances[player][main_phase_selected_card].internal_card_id].extra_ability & 0x1000) == 0))))))
            {
              start_ai_decision_search(7, 0xf);
            }
          resume_activate_selected_card:
            main_phase_selected_internal_card_id = global_card_instances[player][main_phase_selected_card].internal_card_id;
            if (ai_decision_code == 7)
            {
              reset_ai_search_trial_state();
              DAT_008cdab4 = 0;
              DAT_008cdab0 = DAT_008cdab4;
              DAT_008a8de4 = DAT_008cdab0;
              ai_modifier = DAT_008a8de4;
            }
            if (spell_fizzled != 1)
            {
              resolve_activated_ability(player, main_phase_selected_card);
            }
            if ((g_duel_ai_mode_state == 1) && (ai_decision_code == 7))
            {
              *phase_value = 1;
            }
          }
          if (spell_fizzled == 1)
          {
            if ((player == other_player) && ((g_duel_network_flags & 2) == 0))
            {
              DAT_0093d850 = 1;
              *phase_value = 1;
            }
            spell_fizzled = 0;
            goto check_main_phase_abort;
          }
          spell_fizzled = 0;
          if ((((player == other_player) && ((g_duel_network_flags & 2) == 0)) &&
               (g_duel_ai_mode_state != 1)) &&
              (ai_action_replay_available == 0))
            goto restart_active_main_phase_ai_prompt;
          if (g_duel_ai_mode_state != 1)
          {
            request_duel_display_refresh_if_human(player, main_phase_selected_card, 3, 2);
          }
        }
        x_value = 0;
      }
      else if ((((player == active_player) || ((g_duel_network_flags & 2) != 0)) &&
                (((global_cards_data[main_phase_selected_internal_card_id].type & 2) != 0 ||
                  ((global_card_instances[player][main_phase_selected_card].state & 0x1000000) != 0)))) &&
               (((global_card_instances[player][main_phase_selected_card].state & 0x10014) == 0 &&
                 (attacking_creature_count >= 0))) &&
               (can_attack(player, main_phase_selected_card) != 0))
      {
        if (TRACE_ENABLED)
        {
          sprintf(s.trace_assign_attacker, "%d: Main Phase assigning attacker: ThePhase:%d Player:%d card:%d\n", duel_trace_counter++, current_phase,
                  player, main_phase_selected_card);
          append_to_trace_txt(s.trace_assign_attacker);
        }
        combat_assignment_cancelled = 0;
        if ((battlefield_extra_ability_flags & 0x400000) != 0)
        {
          push_affected_card_stack();
          strcpy(DAT_00637808, g_ui_message_buffer);
          trigger_cause_controller = player;
          trigger_cause = main_phase_selected_card;
          dispatch_trigger(player, 0xdc, gs_pay_for_attacker_007a7880, 1);
          strcpy(g_ui_message_buffer, DAT_00637808);
          pop_affected_card_stack();
        }
        if (combat_assignment_cancelled == 0)
        {
          global_card_instances[player][main_phase_selected_card].blocking = 0xff;
          if (attacking_creature_count++ == 0)
          {
            resolve_mana_burn();
            if (check_duel_finished() != 0)
            {
              return 1;
            }
            update_phase_display(player, current_phase);
          }
          if ((((C_get_abilities(player, main_phase_selected_card, 0x34, -1) & 0x200040) != 0) && (1 < attacking_creature_count)) && (unk_00715fb0 == 0))
          {
            strcpy(g_ui_message_buffer, gs_prompt_band_with_other_attacker_007a7af0);
            if (C_real_select_target(player, player, player, 0x200, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, 0, 2, 0,
                                     g_ui_message_buffer, 2, &s.selected_bandmate) != 0)
            {
              main_phase_selected_bandmate_card = s.selected_bandmate.card;
              if ((char)global_card_instances[player][main_phase_selected_bandmate_card].blocking == -1)
              {
                global_card_instances[player][main_phase_selected_bandmate_card].blocking = (unsigned char)main_phase_selected_bandmate_card;
                global_card_instances[player][main_phase_selected_card].blocking = (unsigned char)main_phase_selected_bandmate_card;
              }
              else
              {
                global_card_instances[player][main_phase_selected_card].blocking =
                    global_card_instances[player][main_phase_selected_bandmate_card].blocking;
              }
            }
          }
          global_card_instances[player][main_phase_selected_card].state |= 4;
          if ((battlefield_extra_ability_flags & 0x80000) != 0)
          {
            trigger_cause_controller = player;
            trigger_cause = main_phase_selected_card;
            dispatch_trigger_twice_once_with_each_player_as_reason(current_player, 0xde, gs_attacker_selected_008b32d0, 0);
          }
          main_phase_resume_mode = 2;
        }
        else if (g_duel_ai_mode_state != 1)
        {
          set_duel_prompt_text(gs_prompt_illegal_attacker_008b3d20);
          Sleep(2000);
          set_duel_prompt_text("");
        }
      }
    }
  check_main_phase_abort:
    if (g_adventure_world_exit_requested != 0)
    {
      return 1;
    }

  } while (*phase_value == 0);

  unk_00712938 = 4;
  DAT_00789714 = 1;
  process_damage_prevention(player);
  if ((((player == other_player) && ((g_duel_network_flags & 2) == 0)) &&
       ((g_duel_ai_mode_state != 1 || (ai_decision_code != 1)))) &&
      (current_phase == PHASE_MAIN2))
  {
    goto finish_main_phase;
  }

  if (((player == other_player) && ((g_duel_network_flags & 2) == 0)) && (current_phase < PHASE_MAIN2))
  {
    if ((battlefield_extra_ability_flags & 0x1000000) != 0)
    {
      C_dispatch_event_raw(0x89);
    }
    if (choose_attackers_ai(player) != 0)
    {
      if (other_player == current_player)
      {
        stop_phase = -1;
        stop_phase_player = stop_phase;
      }
      trigger_cause = 0;
      trigger_cause_controller = player;
      for (; trigger_cause < active_cards_count[player];
           trigger_cause = trigger_cause + 1)
      {
        if ((global_card_instances[trigger_cause_controller][trigger_cause].internal_card_id != -1) &&
            ((global_card_instances[trigger_cause_controller][trigger_cause].state & 0x800006) == 6))
        {
          combat_assignment_cancelled = 0;
          if ((battlefield_extra_ability_flags & 0x400000) != 0)
          {
            push_affected_card_stack();
            strcpy(DAT_00637808, g_ui_message_buffer);
            dispatch_trigger(player, 0xdc, gs_pay_for_attacker_007a7880, 1);
            strcpy(g_ui_message_buffer, DAT_00637808);
            pop_affected_card_stack();
          }
          if (combat_assignment_cancelled != 0)
          {
            *(unsigned int *)&global_card_instances[trigger_cause_controller][trigger_cause].state &= 0xfffffffb;
            if ((char)global_card_instances[trigger_cause_controller][trigger_cause].blocking != -1)
            {
              s.bandmate_index = 0;
              s.bandmate_count = 0;
              for (; s.bandmate_index < active_cards_count[player]; s.bandmate_index = s.bandmate_index + 1)
              {
                if ((s.bandmate_index != trigger_cause) &&
                    ((char)global_card_instances[player][s.bandmate_index].blocking ==
                     (char)global_card_instances[trigger_cause_controller][trigger_cause].blocking))
                {
                  s.bandmates[s.bandmate_count] = s.bandmate_index;
                  s.bandmate_count = s.bandmate_count + 1;
                }
              }
              if (s.bandmate_count == 1)
              {
                global_card_instances[player][s.bandmates[0]].blocking = 0xff;
              }
              else
              {
                for (s.bandmate_index = 0; s.bandmate_index < s.bandmate_count; s.bandmate_index = s.bandmate_index + 1)
                {
                  global_card_instances[player][s.bandmates[s.bandmate_index]].blocking =
                      (unsigned char)s.bandmates[0];
                }
              }
            }
          }
        }
      }
      resolve_mana_burn();
      if (check_duel_finished() != 0)
      {
        return 1;
      }
      update_phase_display(player, current_phase);
      C_dispatch_event_raw(0x15);
      update_phase_display(player, current_phase);
      current_phase = PHASE_BEFORE_BLOCKING;
      update_phase_display(player, current_phase);
      do
      {
        if (g_duel_ai_mode_state != 1)
        {
          TENTATIVE_reassess_all_cards(0, 0xff);
          start_ai_decision_search(8, 0x1e);
        }
      resume_ai_attackers_response:
        if (ai_decision_code == 8)
        {
          reset_ai_search_trial_state();
          DAT_008cdab4 = 0;
          DAT_008cdab0 = DAT_008cdab4;
          DAT_008a8de4 = DAT_008cdab0;
          ai_modifier = DAT_008a8de4;
        }
        phase_response_window_open = 1;
        s.allow_response_result = allow_response(-2, current_phase, gs_assign_attackers_008b45f0, PHASE_BEFORE_BLOCKING);
        phase_response_window_open = 0;
      } while (s.allow_response_result != 0);
      current_phase = PHASE_DECLARE_BLOCKERS;
      update_phase_display(player, current_phase);
      dispatch_trigger(1 - player, 0xda, gs_choose_defenders_00789a50, 0);
      choose_blockers_human(player);
      attacking_creature_count = 1;
    }
  }
  if (((player == active_player) || ((g_duel_network_flags & 2) != 0)) && (current_phase < PHASE_MAIN2))
  {
    if ((should_skip_phase(player) != 0) &&
        (attacking_creature_count == 0) &&
        (human_has_phase_stop(PHASE_DECLARE_ATTACKERS) == 0) &&
        ((g_duel_network_flags & 2) == 0))
      goto advance_to_postcombat_main;
    C_dispatch_event_raw(0x15);
    if ((g_duel_ai_mode_state == 1) && (current_phase < PHASE_DECLARE_ATTACKERS))
    {
      choose_attackers_ai(player);
    }
    if ((update_attacker_count_and_check_combat_done(player) != 0) &&
        (((player == other_player) && ((g_duel_network_flags & 2) == 0)) ||
         (human_has_phase_stop(PHASE_DECLARE_ATTACKERS) == 0)))
      goto advance_to_postcombat_main;
    if (attacking_creature_count == 0)
    {
      attacking_creature_count++;
      resolve_mana_burn();
      if (check_duel_finished() != 0)
      {
        return 1;
      }
      update_phase_display(player, current_phase);
    }
    if (TRACE_ENABLED)
    {
      sprintf(s.trace_choose_attackers, "%d: Main Phase PX Choose Attackers: ThePhase:%d\n", duel_trace_counter++, current_phase);
      append_to_trace_txt(s.trace_choose_attackers);
    }
    dispatch_trigger(player, 0xd9, gs_choose_attackers_00939570, 0);
    do
    {
      if ((g_duel_ai_mode_state != 1) && ((g_duel_network_flags & 2) == 0))
      {
        TENTATIVE_reassess_all_cards(0, 0xff);
        start_ai_decision_search(6, 0x1e);
      }
    resume_choose_attackers:
      if (ai_decision_code == 6)
      {
        reset_ai_search_trial_state();
        DAT_008cdab4 = 0;
        DAT_008cdab0 = DAT_008cdab4;
        DAT_008a8de4 = DAT_008cdab0;
        ai_modifier = DAT_008a8de4;
      }
      current_phase = PHASE_DECLARE_ATTACKERS;
      update_phase_display(player, current_phase);
      current_phase = PHASE_BEFORE_BLOCKING;
      update_phase_display(player, current_phase);
      TENTATIVE_reassess_all_cards(0, 0xff);
      phase_response_window_open = 1;
      if (TRACE_ENABLED)
      {
        sprintf(s.trace_attackers_fast_effects, "%d: Main Phase Attackers Fast Effects: ThePhase:%d\n", duel_trace_counter++, current_phase);
        append_to_trace_txt(s.trace_attackers_fast_effects);
      }
      s.allow_response_result = allow_response(-2, current_phase, gs_assign_attackers_008b45f0, PHASE_BEFORE_BLOCKING);
      phase_response_window_open = 0;
    } while (s.allow_response_result != 0);
    current_phase = PHASE_DECLARE_BLOCKERS;
    update_phase_display(player, current_phase);
    if (TRACE_ENABLED)
    {
      sprintf(s.trace_choose_blockers, "%d: Main Phase PX Choose Blockers: ThePhase:%d\n", duel_trace_counter++, current_phase);
      append_to_trace_txt(s.trace_choose_blockers);
    }
    dispatch_trigger_twice_once_with_each_player_as_reason(player, 0xda, gs_choose_defenders_00789a50, 0);
    if ((g_duel_network_flags & 2) == 0)
    {
      setup_ai_combat_abilities(player);
      choose_blockers_ai(player);
    }
    else
    {
      choose_blockers_human(player);
    }
    update_phase_display(player, current_phase);
  }

resolve_combat_if_needed:
  C_dispatch_event_raw(0x1a);
  mark_blocked_attackers(player);
  if (((((0 < attacking_creature_count) ||
         (((player == active_player) || ((g_duel_network_flags & 2) != 0)) &&
          (human_has_phase_stop(PHASE_AFTER_BLOCKING) != 0))) &&
        ((land_can_be_played & 8U) == 0))) ||
      (((player == active_player) || ((g_duel_network_flags & 2) != 0)) && (g_duel_ai_mode_state == 1)))
  {
    do
    {
      if ((g_duel_ai_mode_state != 1) && ((g_duel_network_flags & 2) == 0))
      {
        TENTATIVE_reassess_all_cards(0, 0xff);
        start_ai_decision_search(2, 0x1e);
      }
    resume_after_blockers_response:
      if (ai_decision_code == 2)
      {
        reset_ai_search_trial_state();
        DAT_008cdab4 = 0;
        DAT_008cdab0 = DAT_008cdab4;
        DAT_008a8de4 = DAT_008cdab0;
        ai_modifier = DAT_008a8de4;
      }
      current_phase = PHASE_AFTER_BLOCKING;
      update_phase_display(player, current_phase);
      if (TRACE_ENABLED)
      {
        sprintf(s.trace_blockers_fast_effects, "%d: Main Phase Blockers Fast Effects: ThePhase:%d\n", duel_trace_counter++, current_phase);
        append_to_trace_txt(s.trace_blockers_fast_effects);
      }
      phase_response_window_open = 1;
      s.allow_response_result = allow_response(-2, current_phase, gs_assign_blockers_008a8c40, PHASE_AFTER_BLOCKING);
      phase_response_window_open = 0;
    } while (s.allow_response_result != 0);
    if (TRACE_ENABLED)
    {
      sprintf(s.trace_combat, "%d: Main Phase Combat: ThePhase:%d\n", duel_trace_counter++, current_phase);
      append_to_trace_txt(s.trace_combat);
    }
    resolve_combat_damage(player);
    land_can_be_played |= 8;
    DAT_00789714 = 1;
    dispatch_trigger_twice_once_with_each_player_as_reason(player, 0xcc, gs_end_of_combat_008b43a0, 0);
    cleanup_combat_state(player);
    reassess_all_cards_and_mana();
    TENTATIVE_reassess_all_cards(0, 0xff);
    resolve_mana_burn();
    if (check_duel_finished() != 0)
    {
      return 1;
    }
  }

advance_to_postcombat_main:
  if (((player == active_player) || ((g_duel_network_flags & 2) != 0)) && (g_duel_ai_mode_state != 1))
  {
    if (current_phase < PHASE_MAIN2)
    {
      if (life[1] <= duel_summary.life_totals[1] / 2)
      {
        show_opponent_taunt("'Ouch, that hurt.'");
      }
      attacking_creature_count = 0;
      current_phase = PHASE_MAIN2;
      phase_stop_suppressed = 0;
      update_phase_display(player, current_phase);
      if (should_skip_phase(player) == 0)
        goto resume_main_phase_response_window;
    }
    else if (life[0] <= life[1] / 2)
    {
      show_opponent_taunt("'Give up, you're doomed.'");
    }
  }
  if (((player == other_player) && ((g_duel_network_flags & 2) == 0)) && (current_phase != PHASE_MAIN2))
  {
    current_phase = PHASE_MAIN2;
    update_phase_display(player, current_phase);
    if (g_duel_ai_mode_state == 1)
    {
      if ((ai_decision_code == 1) || (ai_decision_code == 2))
      {
        goto restart_main_phase_action_loop;
      }
      goto finish_main_phase;
    }
    land_can_be_played |= 0x100;
    goto restart_active_main_phase_ai_prompt;
  }
finish_main_phase:
  unk_00712938 = 5;
  if (battlefield_extra_ability_flags & 2)
  {
    dispatch_trigger_twice_once_with_each_player_as_reason(player, 0xe3, gs_end_main_008b26c0, 1);
  }
  resolve_mana_burn();
  if (check_duel_finished() != 0)
    return 1;
  return 0;
}
