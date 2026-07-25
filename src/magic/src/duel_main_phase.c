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
int allow_response(int param_1, int param_2, char *param_3, int param_4);
int dispatch_trigger(int player, trigger_t trig, const char *prompt, int TENTATIVE_allow_response);
int dispatch_trigger_twice_once_with_each_player_as_reason(int reason_for_trig, trigger_t trig, const char *prompt, int a4);
void start_ai_decision_search(int decision_code, int time_scale);
void __stdcall FUN_004e4e9a(void);
void resolve_mana_burn(void);
int check_duel_finished(void);
void FUN_004b5fc9(char *text);
int player_has_legal_attacker(int player);
int human_has_phase_stop(phase_t phase);
int should_skip_phase(int player);
int select_card_for_action(int player, int a2, int a3, int a4, int a5, char *prompt, int a7);
int update_attacker_count_and_check_combat_done(int player);
int process_response_actions(int player, const char *prompt);
int choose_ai_main_phase_action(int player);
int put_card_on_stack(int player, int card, int pay_costs);
int resolve_card_on_stack(int player, int card);
int dispatch_event_to_single_card(int player, int card, int event, int a4, int a5);
int activate(int player, int a2, int card);
void resolve_activated_ability(int player, int card);
void FUN_004a61a1(int player, int card, int a3, int a4);
int can_attack(int player, int card);
int get_recorded_action_count(void);
void push_affected_card_stack(void);
void pop_affected_card_stack(void);
unsigned int C_get_abilities(int player, int card, int event, int a4);
int C_real_select_target();
void FUN_004a61d6(char *text);
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
void get_landwalk_evasion_masks(unsigned int *param_1, unsigned int *param_2);
int can_block_attacker_with_abilities(int blocker_player, int blocker_card, int attacker_player, int attacker_card, unsigned int attacker_abilities,
                                      unsigned int evasion_mask);
void play_sound_effect(int sound_id);

// GLOBAL: MAGIC 0x00637804
int main_phase_selected_internal_card_id;

// GLOBAL: MAGIC 0x00637808
char DAT_00637808[0x100];

// GLOBAL: MAGIC 0x00637908
int main_phase_resume_mode;

// GLOBAL: MAGIC 0x0063790c
int main_phase_selected_bandmate_card;

// GLOBAL: MAGIC 0x00637910
int main_phase_selected_card;

// GLOBAL: MAGIC 0x0093f4ac
int legal_attacker_count;

// GLOBAL: MAGIC 0x007081ac
int ai_blocker_count;

// GLOBAL: MAGIC 0x007086e0
int ai_blocker_cards[16];

// GLOBAL: MAGIC 0x00707de0
int ai_block_assignments[16];

// GLOBAL: MAGIC 0x00708080
int ai_blocker_player;

// GLOBAL: MAGIC 0x00708698
int DAT_00708698;

// GLOBAL: MAGIC 0x0070a830
int untapped_royal_assassin_count;

// GLOBAL: MAGIC 0x0070a834
int defending_cop_color_mask;

// GLOBAL: MAGIC 0x0070a838
int defending_cop_available_mana;

// GLOBAL: MAGIC 0x00709570
char ai_combat_eval_table[0x12c0];

// GLOBAL: MAGIC 0x008cefac
int combat_eval_power;

// GLOBAL: MAGIC 0x0093b278
int combat_eval_toughness;

// GLOBAL: MAGIC 0x0092608c
int combat_eval_abilities;

// GLOBAL: MAGIC 0x0069fbc0
int _DAT_0069fbc0;

// GLOBAL: MAGIC 0x006a72e8
int _DAT_006a72e8;

// GLOBAL: MAGIC 0x0057aaec
int DAT_0057aaec;

// GLOBAL: MAGIC 0x0056e60c
char s__d__Entering_Main_Phase__0056e60c[0x1a] = "%d: Entering Main Phase.\n";

// GLOBAL: MAGIC 0x0056e628
char DAT_0056e628[4] = "";

// GLOBAL: MAGIC 0x0056e62c
char s__d__Main_Phase_skipping_PickACar_0056e62c[0x61] = "%d: Main Phase skipping PickACard. SkipPhase:%d ThePhase:%d mustattackers:%d UserWantsToStop:%d\n";

// GLOBAL: MAGIC 0x0056e690
char s__d__Main_Phase_PickACard__ThePha_0056e690[0x39] = "%d: Main Phase PickACard: ThePhase:%d Player:%d card:%d\n";

// GLOBAL: MAGIC 0x0056e6cc
char s__d__Main_Phase_Non_Phasing_playe_0056e6cc[0x3f] = "%d: Main Phase Non-Phasing player's Fast Effects: ThePhase:%d\n";

// GLOBAL: MAGIC 0x0056e70c
char s__d__Main_Phase__No_attackers__No_0056e70c[0x4e] = "%d: Main Phase (No attackers) Non-Phasing player's Fast Effects: ThePhase:%d\n";

// GLOBAL: MAGIC 0x0056e75c
char DAT_0056e75c[4] = "";

// GLOBAL: MAGIC 0x0056e760
char s__d__Main_Phase_Non_Phasing_playe_0056e760[0x3f] = "%d: Main Phase Non-Phasing player's Fast Effects: ThePhase:%d\n";

// GLOBAL: MAGIC 0x0056e7a0
char s__d__Main_Phase_assigning_attacke_0056e7a0[0x42] = "%d: Main Phase assigning attacker: ThePhase:%d Player:%d card:%d\n";

// GLOBAL: MAGIC 0x0056e7e4
char DAT_0056e7e4[4] = "";

// GLOBAL: MAGIC 0x0056e7e8
char s__d__Main_Phase_PX_Choose_Attacke_0056e7e8[0x31] = "%d: Main Phase PX Choose Attackers: ThePhase:%d\n";

// GLOBAL: MAGIC 0x0056e81c
char s__d__Main_Phase_Attackers_Fast_Ef_0056e81c[0x34] = "%d: Main Phase Attackers Fast Effects: ThePhase:%d\n";

// GLOBAL: MAGIC 0x0056e850
char s__d__Main_Phase_PX_Choose_Blocker_0056e850[0x30] = "%d: Main Phase PX Choose Blockers: ThePhase:%d\n";

// GLOBAL: MAGIC 0x0056e880
char s__d__Main_Phase_Blockers_Fast_Eff_0056e880[0x33] = "%d: Main Phase Blockers Fast Effects: ThePhase:%d\n";

// GLOBAL: MAGIC 0x0056e8b4
char s__d__Main_Phase_Combat__ThePhase__0056e8b4[0x24] = "%d: Main Phase Combat: ThePhase:%d\n";

// GLOBAL: MAGIC 0x0056e8d8
char s__Ouch__that_hurt___0056e8d8[0x14] = "'Ouch, that hurt.'";

// GLOBAL: MAGIC 0x0056e8ec
char s__Give_up__you_re_doomed___0056e8ec[0x1a] = "'Give up, you're doomed.'";

// GLOBAL: MAGIC 0x00561904
char s_PROMPT_CHOOSEBLOCKERS_00561904[0x18] = "PROMPT_CHOOSEBLOCKERS";

// GLOBAL: MAGIC 0x0056191c
char s_PROMPT_CHOOSEBLOCKERS_0056191c[0x18] = "PROMPT_CHOOSEBLOCKERS";

// GLOBAL: MAGIC 0x00561934
char DAT_00561934[4] = "\\";

// FUNCTION: MAGIC 0x0044118f
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
int player_has_available_blocker(int player)
{
  int attacking_player;
  unsigned int attacker_abilities;
  int blocker;
  unsigned int evasion_mask;
  unsigned int player_0_evasion_mask;
  int attacker;
  unsigned int player_1_evasion_mask;

  attacking_player = 1 - player;
  get_landwalk_evasion_masks(&player_1_evasion_mask, &player_0_evasion_mask);
  if (player == 1)
  {
    evasion_mask = player_1_evasion_mask;
  }
  else
  {
    evasion_mask = player_0_evasion_mask;
  }
  attacker = 0;
  do
  {
    if (active_cards_count[attacking_player] <= attacker)
    {
      return 0;
    }
    if ((global_card_instances[attacking_player][attacker].internal_card_id != -1) &&
        ((global_card_instances[attacking_player][attacker].state & STATE_ATTACKING) != 0))
    {
      attacker_abilities = C_get_abilities(attacking_player, attacker, EVENT_ABILITIES, -1);
      for (blocker = 0; blocker < active_cards_count[player]; blocker++)
      {
        if (((global_card_instances[attacking_player][attacker].internal_card_id != -1) &&
             ((global_card_instances[player][blocker].state & (STATE_IN_PLAY | STATE_BLOCKING | STATE_TAPPED)) == STATE_IN_PLAY)) &&
            (can_block_attacker_with_abilities(player, blocker, attacking_player, attacker, attacker_abilities, evasion_mask) != 0))
        {
          return 1;
        }
      }
    }
    attacker++;
  } while (1);
}

// FUNCTION: MAGIC 0x00440d82
void choose_blockers_human(int player)
{
  unsigned int *state_ptr;
  char attacker_blocking_group;
  int select_result;
  int attacking_cards[16];
  int attacking_card_abilities[16];
  unsigned int attacking_player_copy;
  int best_attacker_abilities;
  int attacker_abilities;
  int attacker;
  int attacker_blocking_group_int;
  int internal_card_id;
  int attacker_count;
  int defending_player;
  target_t selected_blocker;
  int done;
  target_t blocked_attacker;

  if (g_duel_ai_mode_state != 1)
  {
    TENTATIVE_reassess_all_cards(0, 0xff);
    attacking_player_copy = player;
    defending_player = 1 - player;
    best_attacker_abilities = 0;
    attacker_count = 0;
    for (attacker = 0; attacker < active_cards_count[player]; attacker++)
    {
      internal_card_id = global_card_instances[player][attacker].internal_card_id;
      if ((internal_card_id != -1) && ((global_card_instances[player][attacker].state & STATE_ATTACKING) != 0))
      {
        attacker_abilities = C_get_abilities(player, attacker, 0x32, -1);
        attacking_cards[attacker_count] = attacker;
        attacking_card_abilities[attacker_count] = attacker_abilities;
        attacker_count++;
        if (best_attacker_abilities < attacker_abilities)
        {
          best_attacker_abilities = attacker_abilities;
        }
      }
    }
    done = 0;
    while ((done == 0) && (select_result = player_has_available_blocker(defending_player), select_result != 0))
    {
      load_text(global_ui_strings_filename, s_PROMPT_CHOOSEBLOCKERS_00561904);
      select_result = C_real_select_target(defending_player, defending_player, defending_player, 0x2200, 0x2000002, 0, 0, 0, 0, 0,
                                           -1, -1, -1, -1, 0x400, 0, 0x11, text_lines[0], 2, &selected_blocker);
      if (select_result == 0)
      {
        done = 1;
      }
      else
      {
        combat_assignment_cancelled = 0;
        if ((battlefield_extra_ability_flags & 0x800000) != 0)
        {
          push_affected_card_stack();
          trigger_cause_controller = selected_blocker.player;
          trigger_cause = selected_blocker.card;
          dispatch_trigger(defending_player, TRIGGER_PAY_TO_BLOCK, gs_pay_for_blocker_009263f0, 1);
          pop_affected_card_stack();
        }
        if ((combat_assignment_cancelled == 0) &&
            (select_result = C_real_select_target(defending_player, attacking_player_copy, attacking_player_copy, 0x200, 2, 0, 0, 0,
                                                  0, 0, -1, -1, -1, -1, 0, 2, 0, text_lines[1], 1, &blocked_attacker),
             select_result != 0))
        {
          select_result = assign_blocker_to_attacker(selected_blocker.player, selected_blocker.card, blocked_attacker.player, blocked_attacker.card);
          if (select_result == 0)
          {
            if (g_duel_ai_mode_state != 1)
            {
              load_text(global_ui_strings_filename, s_PROMPT_CHOOSEBLOCKERS_0056191c);
              FUN_004a61d6(text_lines[2]);
              Sleep(2000);
              FUN_004a61d6(DAT_00561934);
            }
          }
          else
          {
            attacker_blocking_group = global_card_instances[blocked_attacker.player][blocked_attacker.card].blocking;
            attacker_blocking_group_int = (int)attacker_blocking_group;
            if (attacker_blocking_group_int == -1)
            {
              global_card_instances[selected_blocker.player][selected_blocker.card].blocking = (unsigned char)blocked_attacker.card;
            }
            else
            {
              global_card_instances[selected_blocker.player][selected_blocker.card].blocking = (unsigned char)attacker_blocking_group;
            }
            state_ptr = (unsigned int *)&global_card_instances[selected_blocker.player][selected_blocker.card].state;
            *state_ptr = *state_ptr | STATE_BLOCKING;
            if ((g_duel_ai_mode_state != 1) && (attacker_blocking_group_int == -1))
            {
              play_sound_effect(0x15);
            }
            TENTATIVE_reassess_all_cards(0, 0xff);
            if ((battlefield_extra_ability_flags & 0x100000) != 0)
            {
              trigger_cause_controller = selected_blocker.player;
              trigger_cause = selected_blocker.card;
              dispatch_trigger_twice_once_with_each_player_as_reason(current_player, TRIGGER_BLOCKER_CHOSEN, gs_blocker_selected_00926210, 0);
            }
          }
        }
      }
    }
  }
}

// FUNCTION: MAGIC 0x00445d1a
void mark_blocked_attackers(int player)
{
  int opponent;
  int attacker;
  int card;
  int is_blocked;

  opponent = 1 - player;
  for (attacker = 0; attacker < active_cards_count[player]; attacker++)
  {
    if ((global_card_instances[player][attacker].internal_card_id != -1) &&
        ((global_card_instances[player][attacker].state & (STATE_OUBLIETTED | STATE_IN_PLAY | STATE_ATTACKING)) == (STATE_IN_PLAY | STATE_ATTACKING)))
    {
      is_blocked = 0;
      for (card = 0; card < active_cards_count[opponent]; card++)
      {
        if ((global_card_instances[opponent][card].internal_card_id != -1) &&
            ((char)global_card_instances[opponent][card].blocking == attacker))
        {
          is_blocked = 1;
        }
      }
      if (is_blocked)
      {
        for (card = 0; card < active_cards_count[player]; card++)
        {
          if ((attacker == card) ||
              (((char)global_card_instances[player][card].blocking == attacker) &&
               ((global_card_instances[player][card].state & (STATE_OUBLIETTED | STATE_IN_PLAY | STATE_ATTACKING)) == (STATE_IN_PLAY | STATE_ATTACKING))))
          {
            global_card_instances[player][card].state |= STATE_ISBLOCKED;
          }
        }
      }
    }
  }
}

// FUNCTION: MAGIC 0x0044ae9e
int human_has_phase_stop(phase_t phase)
{
  if (g_duel_phase_stop_settings[current_player].phase_flags[phase] != 0)
  {
    return 1;
  }
  if ((phase == stop_phase) && (current_player == stop_phase_player))
  {
    return 1;
  }
  if ((previous_stop_phase == phase) && (current_player == previous_stop_phase_player))
  {
    return 1;
  }
  return 0;
}

// FUNCTION: MAGIC 0x004e3380
int choose_ai_main_phase_action(int player)
{
  unsigned int result;
  int temp;
  unsigned int land_colors_needed;
  unsigned int candidates[20];
  int color;
  unsigned int selected_color;
  unsigned int playable_types;
  int fewest_controlled;
  int nonbasic_land_count;
  unsigned int possible_land_colors;
  int opponent;
  unsigned int card;
  unsigned int exact_land_colors_needed;
  unsigned int preferred_land_color;
  int internal_card_id;
  int candidate_count;
  unsigned int color_mask;
  int ai_depth;
  int colorless_land;
  int playable_card_count;
  unsigned int missing_land_colors;

  if ((g_duel_ai_mode_state == 1) || (DAT_0072c8e4 != 0))
  {
    opponent = 1 - player;
    playable_card_count = 0;
    nonbasic_land_count = 0;
    missing_land_colors = 0;
    possible_land_colors = 0;
    land_colors_needed = 0;
    playable_types = 2;
    if (((land_can_be_played & 1U) == 0) && (0 < *(int *)&DAT_008cfd70[player * 4 + 8] + unk_007161d8))
    {
      for (card = 0; (int)card < active_cards_count[player]; card++)
      {
        if (global_card_instances[player][card].internal_card_id != -1)
        {
          if (((global_card_instances[player][card].state & (STATE_OUBLIETTED | STATE_IN_PLAY)) == STATE_IN_PLAY) &&
              ((global_cards_data[global_card_instances[player][card].internal_card_id].type & 2) != 0))
          {
            playable_types |= 0x7c;
          }
          if ((global_card_instances[player][card].state & (STATE_IN_PLAY | STATE_TAPPED)) == 0)
          {
            if (((global_cards_data[global_card_instances[player][card].internal_card_id].type & 1) != 0) &&
                (missing_land_colors = 1, global_card_instances[player][card].mana_color != 0))
            {
              nonbasic_land_count++;
            }
            if (global_cards_data[global_card_instances[player][card].internal_card_id].cc[1] == -1)
            {
              nonbasic_land_count += 99;
            }
            playable_card_count++;
          }
        }
      }
      if (missing_land_colors != 0)
      {
        colorless_land = 0;
        missing_land_colors = 0;
        for (card = 0; (int)card < active_cards_count[player]; card++)
        {
          internal_card_id = global_card_instances[player][card].internal_card_id;
          if ((internal_card_id != -1) && ((global_card_instances[player][card].state & (STATE_IN_PLAY | STATE_TAPPED)) == 0))
          {
            color_mask = (unsigned int)global_cards_data[internal_card_id].color;
            if (((global_cards_data[internal_card_id].type & 1) != 0) && (land_colors_needed |= color_mask, color_mask == 0))
            {
              colorless_land = 1;
            }
            if ((playable_types & global_cards_data[internal_card_id].type) != 0)
            {
              for (color = 1; color < 6; color++)
              {
                if ((color_mask & (1 << (unsigned char)color)) != 0)
                {
                  if (basiclandtypes_controlled[player][color] + 1 == (int)global_cards_data[internal_card_id].cc[0])
                  {
                    possible_land_colors |= 1 << (unsigned char)color;
                  }
                  if (basiclandtypes_controlled[player][color] + 1 < (int)global_cards_data[internal_card_id].cc[0])
                  {
                    missing_land_colors |= 1 << (unsigned char)color;
                  }
                  temp = abs((int)global_cards_data[internal_card_id].cc[1]);
                  if (basiclandtypes_controlled[player][7] < temp + global_cards_data[internal_card_id].cc[0])
                  {
                    missing_land_colors |= 0xff;
                  }
                }
              }
            }
          }
        }
        fewest_controlled = 999;
        preferred_land_color = land_colors_needed;
        for (card = 0; (int)card < 6; card++)
        {
          if (0 < unk_00939520[player][card])
          {
            missing_land_colors |= 1 << (unsigned char)card;
          }
          if (((land_colors_needed & (1 << (unsigned char)card)) != 0) &&
              (basiclandtypes_controlled[player][card] - unk_00939520[player][card] < fewest_controlled))
          {
            fewest_controlled = basiclandtypes_controlled[player][card] - unk_00939520[player][card];
            preferred_land_color = 1 << (unsigned char)card;
          }
        }
        if (land_colors_needed == 0)
        {
          if ((g_duel_ai_mode_state != 1) && (colorless_land != 0))
          {
            unk_0057aae8 = 1;
            FUN_004e5089();
          }
          selected_color = 99;
        }
        else
        {
          exact_land_colors_needed = preferred_land_color;
          if ((missing_land_colors & land_colors_needed) != 0)
          {
            exact_land_colors_needed = missing_land_colors & land_colors_needed;
          }
          playable_types = possible_land_colors & land_colors_needed;
          result = playable_types;
          if ((playable_types == 0) && (result = exact_land_colors_needed, colorless_land != 0))
          {
            selected_color = 0xffffffff;
          }
          else
          {
            do
            {
              exact_land_colors_needed = result;
              selected_color = internal_rand(7);
              result = exact_land_colors_needed;
            } while ((exact_land_colors_needed & (1 << (unsigned char)selected_color)) == 0);
          }
          if (g_duel_ai_mode_state == 1)
          {
            unk_00939340 = selected_color;
            unk_00925bb8 = -1;
            temp = internal_rand(4);
            if ((temp == 0) && (DAT_008a8d74 == 0))
            {
              unk_00939340 = -2;
            }
            if ((((possible_land_colors == 0) && (missing_land_colors == 0)) && (colorless_land == 0)) &&
                ((playable_card_count < 7) && (3 < basiclandtypes_controlled[player][7] - nonbasic_land_count)))
            {
              unk_00939340 = -2;
            }
          }
          else
          {
            unk_0057aae8 = 1;
            FUN_004e5089();
            selected_color = unk_00939340;
          }
        }
        result = 0xffffffff;
        for (card = 0; (int)card < active_cards_count[player]; card++)
        {
          internal_card_id = global_card_instances[player][card].internal_card_id;
          if (((((internal_card_id != -1) && ((global_card_instances[player][card].state & (STATE_IN_PLAY | STATE_TAPPED)) == 0)) &&
                ((global_cards_data[internal_card_id].type & 1) != 0)) &&
               ((((selected_color == 0xffffffff && (global_card_instances[player][card].mana_color == 0)) ||
                  (((1 << (unsigned char)selected_color) & (int)(char)global_card_instances[player][card].mana_color) != 0)) ||
                 (land_colors_needed == 0)))) &&
              ((result == 0xffffffff) || (4 < internal_card_id)))
          {
            result = card;
          }
        }
        if ((g_duel_ai_mode_state == 1) && ((land_colors_needed != 0) || (colorless_land != 0)))
        {
          if (unk_00939340 == -2)
          {
            unk_00925bb8 = -1;
          }
          else
          {
            unk_00925bb8 = player << 8 | result | 0x1000;
          }
          unk_0057aae8 = 1;
          FUN_004e4f11();
          if (unk_00939340 != -2)
          {
            if (0xf < DAT_008a8de4)
            {
              DAT_008a8de4--;
            }
            DAT_0091bf70[DAT_008a8de4] = global_card_instances[player][result].internal_card_id;
            DAT_007abc00[DAT_008a8de4] = result;
            DAT_008a8de4++;
          }
        }
        if (unk_00939340 != -2)
        {
          return result;
        }
        if ((land_colors_needed == 0) && (colorless_land == 0))
        {
          return result;
        }
        land_can_be_played |= 1;
      }
    }
    candidate_count = 0;
    _DAT_0069fbc0 = 2;
    _DAT_006a72e8 = 0x20;
    if (PHASE_BEFORE_BLOCKING < current_phase)
    {
      _DAT_0069fbc0 = 4;
      _DAT_006a72e8 = 0x40;
    }
    if (current_phase < PHASE_DECLARE_ATTACKERS)
    {
      _DAT_0069fbc0 = 1;
      _DAT_006a72e8 = 0x10;
    }
    if (PHASE_NORMAL_COMBAT_DAMAG2 < current_phase)
    {
      _DAT_0069fbc0 = 8;
      _DAT_006a72e8 = 0xffffff80;
    }
    if (current_phase == PHASE_DISCARD)
    {
      _DAT_0069fbc0 = 0xf;
      _DAT_006a72e8 = 0xfffffff0;
    }
    for (card = 0; (int)card < active_cards_count[player]; card++)
    {
      internal_card_id = global_card_instances[player][card].internal_card_id;
      if (internal_card_id != -1)
      {
        if ((global_card_instances[player][card].state & STATE_IN_PLAY) == 0)
        {
          if (((_DAT_0069fbc0 & (int)(char)global_cards_data[internal_card_id].act_phases) != 0) &&
              ((global_cards_data[internal_card_id].type & 0x7e) != 0))
          {
            selected_color = single_color_test_bit_to_color_t((int)global_cards_data[internal_card_id].color);
            temp = FUN_0043fdb3(player, player, card);
            if ((temp != 0) &&
                (((global_cards_data[internal_card_id].type & 0x3c) == 0) ||
                 (temp = (*global_cards_data[internal_card_id].code_pointer)(player, card, 0x74), temp != 0)))
            {
              candidates[candidate_count] = card;
              candidate_count++;
            }
          }
        }
        else if (((((global_card_instances[player][card].state & (STATE_ATTACKING | STATE_TAPPED | STATE_INVISIBLE)) == 0) &&
                   ((global_cards_data[internal_card_id].extra_ability & 0x1003U) != 0x1000)) &&
                  ((_DAT_006a72e8 & (int)(char)global_cards_data[internal_card_id].act_phases) != 0)) &&
                 (temp = dispatch_event_to_single_card(player, card, 0x73, 1 - player, -1), temp != 0))
        {
          candidates[candidate_count] = card;
          candidate_count++;
        }
      }
    }
    if (candidate_count == 0)
    {
      DAT_008a8d7c = -1;
      DAT_0057aaec = 0;
      if (DAT_008a8d74 == 1)
      {
        DAT_0091c500 = -1;
      }
      result = 0xffffffff;
    }
    else
    {
      if (g_duel_ai_mode_state == 1)
      {
        if (DAT_0089652c == -9999)
        {
          ai_search_flags = 0;
        }
        if ((unk_008cc840 == 0x19) && (candidate_count < 3))
        {
          ai_search_flags |= 1;
        }
        unk_00939340 = ai_search_flags;
        FUN_004e4f11();
      }
      else
      {
        FUN_004e5089();
        ai_search_flags = unk_00939340;
      }
      candidates[candidate_count] = 0xffffffff;
      candidate_count++;
      if (g_duel_ai_mode_state == 1)
      {
        if ((DAT_008a8d7c == -1) || (DAT_008a8d74 != 0))
        {
          unk_00939340 = internal_rand(candidate_count);
          if (DAT_008a8d74 != 0)
          {
            unk_00939340 = candidate_count - 1;
            if (DAT_008a8d74 == 1)
            {
              temp = ClampIntToRange(candidate_count * candidate_count, 10, 0x14);
              DAT_0091c500 = temp * (g_shandalar_difficulty + 1) * 5;
            }
            DAT_008a8d74 = -1;
          }
          unk_00925bb8 = (-((global_card_instances[player][candidates[unk_00939340]].state & 2) == 0) & 0xfffff000) + 0x2000 |
                         candidates[unk_00939340] | ((player == 0) - 1) & 0x100;
          unk_0057aae8 = 2;
          FUN_004e4f11();
        }
        else
        {
          ai_depth = get_recorded_action_count();
          temp = DAT_0057aaec;
          if ((DAT_0057aaec == 0) && (DAT_008a8d7c < ai_depth))
          {
            DAT_008a8d7c = ai_depth;
          }
          if (ai_depth == DAT_008a8d7c)
          {
            unk_00939340 = DAT_0057aaec;
            DAT_0057aaec++;
            if (candidate_count - 1 <= temp)
            {
              DAT_008a8d7c++;
              DAT_0057aaec = 0;
            }
          }
          if (DAT_008a8d7c < ai_depth)
          {
            unk_00939340 = candidate_count - 1;
          }
          if (ai_depth < DAT_008a8d7c)
          {
            unk_0057aae8 = 2;
            FUN_004e5089();
            FUN_004e51bb();
            if (candidate_count <= unk_00939340)
            {
              unk_00939340 = candidate_count - 1;
            }
          }
          unk_00925bb8 = (-((global_card_instances[player][candidates[unk_00939340]].state & 2) == 0) & 0xfffff000) + 0x2000 |
                         candidates[unk_00939340] | ((player == 0) - 1) & 0x100;
          unk_0057aae8 = 2;
          FUN_004e4f11();
          if ((candidate_count - 1 == unk_00939340) && (ai_depth < DAT_008a8d7c))
          {
            DAT_008a8d7c = -1;
            DAT_0057aaec = 0;
          }
        }
      }
      else
      {
        unk_0057aae8 = 2;
        FUN_004e5089();
        if (candidate_count <= unk_00939340)
        {
          unk_00939340 = candidate_count - 1;
        }
      }
      if (candidates[unk_00939340] != 0xffffffff)
      {
        if (0xf < DAT_008a8de4)
        {
          DAT_008a8de4--;
        }
        DAT_0091bf70[DAT_008a8de4] = global_card_instances[player][candidates[unk_00939340]].internal_card_id;
        DAT_007abc00[DAT_008a8de4] = candidates[unk_00939340];
        DAT_008a8de4++;
      }
      result = candidates[unk_00939340];
    }
  }
  else
  {
    result = 0xffffffff;
  }
  return result;
}

// FUNCTION: MAGIC 0x0050e79b
void setup_ai_combat_abilities(int player)
{
  unsigned int *state_ptr;
  int attached_card;
  unsigned int saved_state;
  int saved_ai_mode;
  int saved_combat_eval_flag;
  int defending_player;
  int internal_card_id;
  int attached_player;
  int mana_available;
  int card;
  color_t card_color;

  saved_combat_eval_flag = DAT_0093f4b0;
  saved_ai_mode = g_duel_ai_mode_state;
  defending_player = 1 - player;
  g_duel_ai_mode_state = 1;
  DAT_0093f4b0 = 1;
  untapped_royal_assassin_count = 0;
  memset(&ai_combat_eval_table, 0, 0x12c0);
  for (card = 0; card < active_cards_count[player]; card++)
  {
    if (is_in_play(player, card) != 0)
    {
      internal_card_id = global_card_instances[player][card].internal_card_id;
      attached_player = (int)(char)global_card_instances[player][card].damage_target_player;
      attached_card = global_card_instances[player][card].damage_target_card;
      if ((global_cards_data[internal_card_id].code_pointer == card_regeneration) &&
          (mana_available = has_mana(attached_player, COLOR_GREEN, 1), mana_available != 0))
      {
        *(unsigned int *)&ai_combat_eval_table[attached_player * 0x960 + attached_card * 0x10 + 8] |= 0x200;
        state_ptr = (unsigned int *)&global_card_instances[attached_player][attached_card].regen_status;
        *state_ptr |= 0x200;
      }
      else if ((global_cards_data[internal_card_id].code_pointer == card_the_brute) &&
               (mana_available = has_mana(attached_player, COLOR_RED, 3), mana_available != 0))
      {
        *(unsigned int *)&ai_combat_eval_table[attached_player * 0x960 + attached_card * 0x10 + 8] |= 0x200;
        state_ptr = (unsigned int *)&global_card_instances[attached_player][attached_card].regen_status;
        *state_ptr |= 0x200;
      }
      if (global_cards_data[internal_card_id].code_pointer == card_holy_armor)
      {
        mana_available = has_mana(attached_player, COLOR_WHITE, 1);
        *(int *)&ai_combat_eval_table[attached_player * 0x960 + attached_card * 0x10 + 4] += mana_available;
      }
      else if (global_cards_data[internal_card_id].code_pointer == card_firebreathing)
      {
        mana_available = has_mana(attached_player, COLOR_RED, 1);
        *(int *)&ai_combat_eval_table[attached_player * 0x960 + attached_card * 0x10] += mana_available;
      }
      else if (global_cards_data[internal_card_id].code_pointer == card_blessing)
      {
        mana_available = has_mana(attached_player, COLOR_WHITE, 1);
        *(int *)&ai_combat_eval_table[attached_player * 0x960 + attached_card * 0x10] += mana_available;
        mana_available = has_mana(attached_player, COLOR_WHITE, 1);
        *(int *)&ai_combat_eval_table[attached_player * 0x960 + attached_card * 0x10 + 4] += mana_available;
      }
      else if (global_cards_data[internal_card_id].code_pointer == card_uncle_istvan)
      {
        *(unsigned int *)&ai_combat_eval_table[player * 0x960 + card * 0x10 + 8] |= 0x1ff800;
      }
      else if (global_cards_data[internal_card_id].code_pointer == card_murk_dwellers)
      {
        *(unsigned int *)&ai_combat_eval_table[player * 0x960 + card * 0x10 + 8] |= 0x80000000;
      }
      else if (global_cards_data[internal_card_id].code_pointer == card_gaseous_form)
      {
        *(unsigned int *)&ai_combat_eval_table[attached_player * 0x960 + attached_card * 0x10 + 8] |= 0x1ff800;
        *(int *)&ai_combat_eval_table[attached_player * 0x960 + attached_card * 0x10] = 0xffffff9d;
      }
      state_ptr = (unsigned int *)&global_card_instances[player][card].regen_status;
      *state_ptr |= 0xe000000;
      saved_state = global_card_instances[player][card].state;
      if (other_player == player)
      {
        if (((unsigned char *)&global_card_instances[player][card].state)[1] & 0x20)
        {
          global_card_instances[player][card].state |= STATE_ATTACKING;
        }
        else
        {
          global_card_instances[player][card].state |= STATE_TAPPED | STATE_ATTACKING;
        }
      }
      combat_eval_power = C_get_abilities(player, card, 0x32, -1);
      combat_eval_toughness = C_get_abilities(player, card, 0x33, -1);
      combat_eval_abilities = C_get_abilities(player, card, EVENT_ABILITIES, -1);
      card_color = single_color_test_bit_to_color_t((int)global_cards_data[internal_card_id].color);
      if (((combat_eval_abilities & 0x200) != 0) && (mana_available = has_mana(player, card_color, 1), mana_available == 0))
      {
        combat_eval_abilities &= 0xfffffdff;
      }
      if (active_player == player)
      {
        dispatch_event(player, card, EVENT_CHECK_ABILITIES);
      }
      *(int *)&ai_combat_eval_table[player * 0x960 + card * 0x10] += combat_eval_power;
      *(int *)&ai_combat_eval_table[player * 0x960 + card * 0x10 + 4] += combat_eval_toughness;
      *(unsigned int *)&ai_combat_eval_table[player * 0x960 + card * 0x10 + 8] |= combat_eval_abilities;
      global_card_instances[player][card].state = saved_state;
    }
  }
  defending_cop_color_mask = 0;
  for (card = 0; card < active_cards_count[defending_player]; card++)
  {
    if (is_in_play(defending_player, card) != 0)
    {
      internal_card_id = global_card_instances[defending_player][card].internal_card_id;
      attached_player = (int)(char)global_card_instances[defending_player][card].damage_target_player;
      attached_card = global_card_instances[defending_player][card].damage_target_card;
      if ((global_cards_data[internal_card_id].code_pointer == card_regeneration) &&
          (mana_available = has_mana(attached_player, COLOR_GREEN, 1), mana_available != 0))
      {
        *(unsigned int *)&ai_combat_eval_table[attached_player * 0x960 + attached_card * 0x10 + 8] |= 0x200;
        state_ptr = (unsigned int *)&global_card_instances[attached_player][attached_card].regen_status;
        *state_ptr |= 0x200;
      }
      else if ((global_cards_data[internal_card_id].code_pointer == card_the_brute) &&
               (mana_available = has_mana(attached_player, COLOR_RED, 3), mana_available != 0))
      {
        *(unsigned int *)&ai_combat_eval_table[attached_player * 0x960 + attached_card * 0x10 + 8] |= 0x200;
        state_ptr = (unsigned int *)&global_card_instances[attached_player][attached_card].regen_status;
        *state_ptr |= 0x200;
      }
      if (global_cards_data[internal_card_id].code_pointer == card_holy_armor)
      {
        mana_available = has_mana(attached_player, COLOR_WHITE, 1);
        *(int *)&ai_combat_eval_table[attached_player * 0x960 + attached_card * 0x10 + 4] += mana_available;
      }
      else if (global_cards_data[internal_card_id].code_pointer == card_firebreathing)
      {
        mana_available = has_mana(attached_player, COLOR_RED, 1);
        *(int *)&ai_combat_eval_table[attached_player * 0x960 + attached_card * 0x10] += mana_available;
      }
      else if (global_cards_data[internal_card_id].code_pointer == card_blessing)
      {
        mana_available = has_mana(attached_player, COLOR_WHITE, 1);
        *(int *)&ai_combat_eval_table[attached_player * 0x960 + attached_card * 0x10] += mana_available;
        mana_available = has_mana(attached_player, COLOR_WHITE, 1);
        *(int *)&ai_combat_eval_table[attached_player * 0x960 + attached_card * 0x10 + 4] += mana_available;
      }
      else if ((global_cards_data[internal_card_id].code_pointer == card_royal_assassin) &&
               ((global_card_instances[defending_player][card].state & (STATE_TAPPED | STATE_INVISIBLE)) == 0))
      {
        untapped_royal_assassin_count++;
      }
      else if (global_cards_data[internal_card_id].code_pointer == card_uncle_istvan)
      {
        *(unsigned int *)&ai_combat_eval_table[defending_player * 0x960 + card * 0x10 + 8] |= 0x1ff800;
      }
      else if (global_cards_data[internal_card_id].code_pointer == card_murk_dwellers)
      {
        *(unsigned int *)&ai_combat_eval_table[defending_player * 0x960 + card * 0x10 + 8] |= 0x80000000;
      }
      else if (global_cards_data[internal_card_id].code_pointer == card_gaseous_form)
      {
        *(unsigned int *)&ai_combat_eval_table[attached_player * 0x960 + attached_card * 0x10 + 8] |= 0x1ff800;
        *(int *)&ai_combat_eval_table[attached_player * 0x960 + attached_card * 0x10] = 0xffffff9d;
      }
      state_ptr = (unsigned int *)&global_card_instances[defending_player][card].regen_status;
      *state_ptr |= 0xe000000;
      saved_state = global_card_instances[defending_player][card].state;
      global_card_instances[defending_player][card].state |= STATE_BLOCKING;
      combat_eval_power = C_get_abilities(defending_player, card, 0x32, -1);
      combat_eval_toughness = C_get_abilities(defending_player, card, 0x33, -1);
      combat_eval_abilities = C_get_abilities(defending_player, card, EVENT_ABILITIES, -1);
      card_color = single_color_test_bit_to_color_t((int)global_cards_data[internal_card_id].color);
      if (((combat_eval_abilities & 0x200) != 0) && (mana_available = has_mana(defending_player, card_color, 1), mana_available == 0))
      {
        combat_eval_abilities &= 0xfffffdff;
      }
      if (active_player == defending_player)
      {
        dispatch_event(defending_player, card, EVENT_CHECK_ABILITIES);
      }
      *(int *)&ai_combat_eval_table[defending_player * 0x960 + card * 0x10] += combat_eval_power;
      *(int *)&ai_combat_eval_table[defending_player * 0x960 + card * 0x10 + 4] += combat_eval_toughness;
      *(unsigned int *)&ai_combat_eval_table[defending_player * 0x960 + card * 0x10 + 8] |= combat_eval_abilities;
      global_card_instances[defending_player][card].state = saved_state;
      internal_card_id = global_card_instances[defending_player][card].internal_card_id;
      if (global_cards_data[internal_card_id].code_pointer == card_co_p_black)
      {
        defending_cop_color_mask |= 2;
      }
      if (global_cards_data[internal_card_id].code_pointer == card_co_p_blue)
      {
        defending_cop_color_mask |= 4;
      }
      if (global_cards_data[internal_card_id].code_pointer == card_co_p_green)
      {
        defending_cop_color_mask |= 8;
      }
      if (global_cards_data[internal_card_id].code_pointer == card_co_p_red)
      {
        defending_cop_color_mask |= 0x10;
      }
      if (global_cards_data[internal_card_id].code_pointer == card_co_p_white)
      {
        defending_cop_color_mask |= 0x20;
      }
    }
  }
  if (defending_cop_color_mask == 0)
  {
    defending_cop_available_mana = 0;
  }
  else
  {
    defending_cop_available_mana = has_mana(defending_player, COLOR_ANY, 0);
  }
  reassess_all_cards_and_mana();
  g_duel_ai_mode_state = saved_ai_mode;
  DAT_0093f4b0 = saved_combat_eval_flag;
}

// FUNCTION: MAGIC 0x00510a3f
unsigned int choose_attackers_ai(int player)
{
  (void)player;
  return 0;
}

// FUNCTION: MAGIC 0x005129d1
void choose_blockers_ai(int player)
{
  int saved_event_state;
  int blocker_index;

  saved_event_state = unk_00712938;
  unk_00712938 = 2;
  DAT_00708698 = 0xffffd8f1;
  for (blocker_index = 0; blocker_index < 0x10; blocker_index++)
  {
    ai_block_assignments[blocker_index] = 0;
  }
  if (g_duel_ai_mode_state == 1 || active_player == current_player)
  {
    for (blocker_index = 0; blocker_index < ai_blocker_count; blocker_index++)
    {
      if ((battlefield_extra_ability_flags & 0x800000) != 0)
      {
        push_affected_card_stack();
        trigger_cause_controller = ai_blocker_player;
        trigger_cause = ai_blocker_cards[blocker_index];
        dispatch_trigger(1 - current_player, TRIGGER_PAY_TO_BLOCK, gs_pay_for_blocker_009263f0, 1);
        pop_affected_card_stack();
      }
      if ((ai_block_assignments[blocker_index] != -1) && ((battlefield_extra_ability_flags & 0x100000) != 0))
      {
        trigger_cause_controller = ai_blocker_player;
        trigger_cause = ai_blocker_cards[blocker_index];
        dispatch_trigger_twice_once_with_each_player_as_reason(current_player, TRIGGER_BLOCKER_CHOSEN, gs_blocker_selected_00926210, 0);
      }
      global_card_instances[ai_blocker_player][ai_blocker_cards[blocker_index]].blocking = (unsigned char)ai_block_assignments[blocker_index];
      if (g_duel_ai_mode_state != 1)
      {
        FUN_004a61a1(ai_blocker_player, ai_blocker_cards[blocker_index], 5, 2);
        if (ai_block_assignments[blocker_index] != -1)
        {
          play_sound_effect(0x15);
        }
      }
    }
  }
  unk_00712938 = saved_event_state;
}

// FUNCTION: MAGIC 0x005136ed
void resolve_combat_damage(int player)
{
  (void)player;
}

// FUNCTION: MAGIC 0x00515f34
int cleanup_combat_state(int player)
{
  int card;

  for (card = 0; card < ai_blocker_count; card++)
  {
    global_card_instances[1 - player][ai_blocker_cards[card]].state &= ~STATE_BLOCKING;
  }
  for (card = 0; card < active_cards_count[player]; card++)
  {
    if ((global_card_instances[player][card].state & STATE_ATTACKING) != 0)
    {
      global_card_instances[player][card].state &= ~STATE_ATTACKING;
      global_card_instances[player][card].state |= STATE_ATTACKED;
    }
  }
  return 1;
}

// FUNCTION: MAGIC 0x0044cda7
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
    sprintf(s.trace_enter_phase, s__d__Entering_Main_Phase__0056e60c, duel_trace_counter++);
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
    DAT_008a8d7c = 0;
  }
resume_main_phase_response_window:

  phase_response_window_open = 1;
  _DAT_0091c0ec = 0;
  if ((player == other_player) && ((g_duel_network_flags & 2) == 0))
  {
    FUN_004e4e9a();
    DAT_008cdab4 = 0;
    DAT_008cdab0 = DAT_008cdab4;
    DAT_008a8de4 = DAT_008cdab0;
    ai_modifier = DAT_008a8de4;
    if ((g_duel_ai_mode_state != 1) && (((land_can_be_played & 0x40U) == 0 && (DAT_00896690 != '\0'))))
    {
      FUN_004b5fc9(&DAT_00896690);
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
      FUN_004b5fc9(&DAT_00896690);
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
        strcpy(unk_00748770, DAT_0056e628);
        if (current_phase <= PHASE_MAIN1)
        {
          strcpy(unk_00748770, gs_prompt_main_phase_precombat_cast_spells_008cc710);
          if ((land_can_be_played & 1U) == 0)
          {
            strcpy(unk_00748770, gs_prompt_main_phase_precombat_cast_spells_play_land_00777970);
          }
        }
        else if (current_phase >= PHASE_MAIN2)
        {
          strcpy(unk_00748770, gs_prompt_main_phase_postcombat_cast_spells_007ab2d0);
          if ((land_can_be_played & 1U) == 0)
          {
            strcpy(unk_00748770, gs_prompt_main_phase_postcombat_cast_spells_play_land_0091cf50);
          }
        }
        else
        {
          if (player == current_player)
          {
            strcpy(unk_00748770, gs_prompt_combat_choose_attackers_0093d990);
          }
          else
          {
            strcpy(unk_00748770, gs_prompt_combat_choose_blockers_008b4140);
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
            sprintf(s.trace_skip_pick_card, s__d__Main_Phase_skipping_PickACar_0056e62c, duel_trace_counter++, s.skip_phase,
                    current_phase, legal_attacker_count, s.player_can_stop_here);
            append_to_trace_txt(s.trace_skip_pick_card);
          }
        }
        else
        {
          main_phase_selected_card = select_card_for_action(player, player, player, 0, 0, unk_00748770, 2);
          if (TRACE_ENABLED)
          {
            sprintf(s.trace_pick_card, s__d__Main_Phase_PickACard__ThePha_0056e690, duel_trace_counter++, current_phase,
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
                sprintf(s.trace_nonphasing_precombat_fast_effects, s__d__Main_Phase_Non_Phasing_playe_0056e6cc, duel_trace_counter++, current_phase);
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
              strcpy(unk_00748770, gs_prompt_combat_choose_attackers_0093d990);
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
                sprintf(s.trace_no_attackers_nonphasing_fast_effects, s__d__Main_Phase__No_attackers__No_0056e70c, duel_trace_counter++, current_phase);
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
            strcpy(unk_00748770, DAT_0056e75c);
            if ((0 < attacking_creature_count) || (update_attacker_count_and_check_combat_done(player) == 0))
            {
              *phase_value = 1;
            }
          }
          else if ((g_duel_network_flags & 2) != 0)
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
              sprintf(s.trace_nonphasing_postcombat_fast_effects, s__d__Main_Phase_Non_Phasing_playe_0056e760, duel_trace_counter++, current_phase);
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
          break;
        }

        if (current_phase == PHASE_DECLARE_ATTACKERS)
        {
          break;
        }

        x_value = 0;
        unk_008ce508 = -1;
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
            FUN_004e4e9a();
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
                  DAT_0091c500 = 0;
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
              (DAT_0072c8e4 == 0))
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
        unk_00925bb8 = -1;
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
              FUN_004e4e9a();
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
              (DAT_0072c8e4 == 0))
            goto restart_active_main_phase_ai_prompt;
          if (g_duel_ai_mode_state != 1)
          {
            FUN_004a61a1(player, main_phase_selected_card, 3, 2);
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
          sprintf(s.trace_assign_attacker, s__d__Main_Phase_assigning_attacke_0056e7a0, duel_trace_counter++, current_phase,
                  player, main_phase_selected_card);
          append_to_trace_txt(s.trace_assign_attacker);
        }
        combat_assignment_cancelled = 0;
        if ((battlefield_extra_ability_flags & 0x400000) != 0)
        {
          push_affected_card_stack();
          strcpy(&DAT_00637808, unk_00748770);
          trigger_cause_controller = player;
          trigger_cause = main_phase_selected_card;
          dispatch_trigger(player, 0xdc, gs_pay_for_attacker_007a7880, 1);
          strcpy(unk_00748770, &DAT_00637808);
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
            strcpy(unk_00748770, gs_prompt_band_with_other_attacker_007a7af0);
            if (C_real_select_target(player, player, player, 0x200, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, 0, 2, 0,
                                     unk_00748770, 2, &s.selected_bandmate) != 0)
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
          FUN_004a61d6(gs_prompt_illegal_attacker_008b3d20);
          Sleep(2000);
          FUN_004a61d6(DAT_0056e7e4);
        }
      }
    }
  check_main_phase_abort:
    if (DAT_0091bfb0 != 0)
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
            strcpy(&DAT_00637808, unk_00748770);
            dispatch_trigger(player, 0xdc, gs_pay_for_attacker_007a7880, 1);
            strcpy(unk_00748770, &DAT_00637808);
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
          FUN_004e4e9a();
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
      sprintf(s.trace_choose_attackers, s__d__Main_Phase_PX_Choose_Attacke_0056e7e8, duel_trace_counter++, current_phase);
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
        FUN_004e4e9a();
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
        sprintf(s.trace_attackers_fast_effects, s__d__Main_Phase_Attackers_Fast_Ef_0056e81c, duel_trace_counter++, current_phase);
        append_to_trace_txt(s.trace_attackers_fast_effects);
      }
      s.allow_response_result = allow_response(-2, current_phase, gs_assign_attackers_008b45f0, PHASE_BEFORE_BLOCKING);
      phase_response_window_open = 0;
    } while (s.allow_response_result != 0);
    current_phase = PHASE_DECLARE_BLOCKERS;
    update_phase_display(player, current_phase);
    if (TRACE_ENABLED)
    {
      sprintf(s.trace_choose_blockers, s__d__Main_Phase_PX_Choose_Blocker_0056e850, duel_trace_counter++, current_phase);
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
        FUN_004e4e9a();
        DAT_008cdab4 = 0;
        DAT_008cdab0 = DAT_008cdab4;
        DAT_008a8de4 = DAT_008cdab0;
        ai_modifier = DAT_008a8de4;
      }
      current_phase = PHASE_AFTER_BLOCKING;
      update_phase_display(player, current_phase);
      if (TRACE_ENABLED)
      {
        sprintf(s.trace_blockers_fast_effects, s__d__Main_Phase_Blockers_Fast_Eff_0056e880, duel_trace_counter++, current_phase);
        append_to_trace_txt(s.trace_blockers_fast_effects);
      }
      phase_response_window_open = 1;
      s.allow_response_result = allow_response(-2, current_phase, gs_assign_blockers_008a8c40, PHASE_AFTER_BLOCKING);
      phase_response_window_open = 0;
    } while (s.allow_response_result != 0);
    if (TRACE_ENABLED)
    {
      sprintf(s.trace_combat, s__d__Main_Phase_Combat__ThePhase__0056e8b4, duel_trace_counter++, current_phase);
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
      if (life[1] <= (int)*(int *)&DAT_008cfd70[4] / 2)
      {
        FUN_004b5fc9(s__Ouch__that_hurt___0056e8d8);
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
      FUN_004b5fc9(s__Give_up__you_re_doomed___0056e8ec);
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
