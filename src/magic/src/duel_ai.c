
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "defs.h"
#include "magic/src/cards.h"
#include "magic/src/duel_engine.h"
#include "magic/src/game_support.h"
#include "magic/src/global_duel_ui_ids.h"
#include "magic/src/global_other.h"
#include "magic/src/global_state.h"
#include "magic/src/global_strings.h"

extern int life[2];
extern char DAT_00896690;
extern int _DAT_0091c0ec;
extern char DAT_0091ce40[300];

void TENTATIVE_reassess_all_cards(int view_player, int present_after_draw);
void append_to_trace_txt(char *text);
int dispatch_trigger(int player, trigger_t trig, const char *prompt, int TENTATIVE_allow_response);
int dispatch_trigger_twice_once_with_each_player_as_reason(int reason_for_trig, trigger_t trig, const char *prompt, int a4);
void resolve_mana_burn(void);
int check_duel_finished(void);
void show_opponent_taunt(char *text);
void FUN_00464bb0(void);
int get_duel_thread_time_ms(void);
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
void mark_blocked_attackers(int player);
void resolve_combat_damage(int player);
void reassess_all_cards_and_mana(void);
int GetCardRarity(int card_id);
int reset_duel_tick_timer_indirect(void);
void update_duel_thread_time_marker(void);
void StopWorldLocationMusic(void);
void __cdecl _assert(void *expr, void *filename, unsigned line);
void append_displayed_card_name(int player, int card);
int is_in_play(int player, int card);
int has_mana(int player, color_t color, int amount);
int player_has_available_blocker(int player);
int assign_blocker_to_attacker(int blocker_player, int blocker_card, int attacker_player, int attacker_card);
void get_landwalk_evasion_masks(unsigned int *out_landwalk_mask, unsigned int *out_basic_land_mask);
int can_block_attacker_with_abilities(int blocker_player, int blocker_card, int attacker_player, int attacker_card, unsigned int attacker_abilities,
                                      unsigned int evasion_mask);
int play_sound_effect(int sound_id);
void record_ai_action_selection(void);
void replay_ai_action_selection(void);
void save_ai_search_state(void);
#ifdef SHANDALAR
void refresh_ai_random_table(void);
#endif
void restore_ai_search_state(void);
int ai_opinion_of_gamestate_continued(int player, int score);
void setup_ai_combat_abilities(int player);
int score_ai_attacker_candidate(int player, int card);
void setup_combat_damage_simulation(int player);
void save_combat_simulation_state(void);
void restore_combat_simulation_state(void);
int score_gamestate_after_simulated_card_death(int player, int card);
void dispatch_ai_combat_pump_checks(int player);
int score_ai_blocker_candidate(int player, int card);
unsigned int get_destroy_if_blocked_combat_flags(int attacker_player, int attacker_card, int blocker_player, int blocker_card);
void choose_blockers_ai(int player);
void enumerate_ai_block_assignments(int player, int blocker_index);
int score_current_ai_block_assignment(void);

extern int combat_damage_attacker_abilities[16];
extern int combat_damage_attacker_cards[16];
extern int combat_damage_attacker_count;
extern int combat_damage_attacker_damage[16];
extern int combat_damage_attacker_toughness[16];
extern int combat_damage_blocker_abilities[16];
extern int combat_damage_blocker_damage[16];
extern int combat_damage_blocker_toughness[16];
extern int g_reveal_all_world_info;
extern int DAT_00743098;

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
  int duel_summary_copy[24];
  int hand_counts[2];
  int creature_counts[2];
  int artifact_counts[2];
  int enchantment_counts[2];
  int cards_drawn_value;
  int creatures_died_this_turn_value;
  int land_entries_this_turn_value;
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

// GLOBAL: SHANDALAR 0x0074f1f0
static AiSearchBackup g_ai_search_backup;

typedef struct ai_combat_eval_entry_t
{
  int power;
  int toughness;
  unsigned int abilities;
  int cached_score;
} ai_combat_eval_entry_t;

#define AI_CARD(player_, card_) global_card_instances[(player_)][(card_)]
#define AI_CARD_STATE(player_, card_) AI_CARD(player_, card_).state
#define AI_CARD_INTERNAL_ID(player_, card_) AI_CARD(player_, card_).internal_card_id
#define AI_CARD_BLOCKING(player_, card_) AI_CARD(player_, card_).blocking
#define AI_EVAL(player_, card_) (*(ai_combat_eval_table[(player_)] + (card_)))

// GLOBAL: MAGIC 0x007081ac
// GLOBAL: SHANDALAR 0x005a7ba4
int ai_blocker_count;

// GLOBAL: MAGIC 0x007086e0
// GLOBAL: SHANDALAR 0x005a80d8
int ai_blocker_cards[16];

// GLOBAL: MAGIC 0x00707d58
// GLOBAL: SHANDALAR 0x005a7750
int ai_block_damage_orders[16];

// GLOBAL: MAGIC 0x00707de0
// GLOBAL: SHANDALAR 0x005a77d8
int ai_block_assignments[16];

// GLOBAL: MAGIC 0x00708080
// GLOBAL: SHANDALAR 0x005a7a78
int ai_blocker_player;

// GLOBAL: MAGIC 0x00708698
// GLOBAL: SHANDALAR 0x005a8090
int DAT_00708698;

// GLOBAL: MAGIC 0x00708610
// GLOBAL: SHANDALAR 0x005a8008
int DAT_00708610;

// GLOBAL: MAGIC 0x00708658
// GLOBAL: SHANDALAR 0x005a8050
int DAT_00708658[16];
// GLOBAL: MAGIC 0x00707ef0
// GLOBAL: SHANDALAR 0x005a78e8
int DAT_00707ef0[16];
// GLOBAL: MAGIC 0x00708128
// GLOBAL: SHANDALAR 0x005a7b20
int DAT_00708128[16];
// GLOBAL: MAGIC 0x00707f38
// GLOBAL: SHANDALAR 0x005a7930
int DAT_00707f38[16];
// GLOBAL: MAGIC 0x00707d98
// GLOBAL: SHANDALAR 0x005a7790
int DAT_00707d98[16];
// GLOBAL: MAGIC 0x007080c8
// GLOBAL: SHANDALAR 0x005a7ac0
int DAT_007080c8[16];
// GLOBAL: MAGIC 0x00708088
// GLOBAL: SHANDALAR 0x005a7a80
int DAT_00708088[16];
// GLOBAL: MAGIC 0x00707ea8
// GLOBAL: SHANDALAR 0x005a78a0
int DAT_00707ea8[16];
// GLOBAL: MAGIC 0x007081a8
// GLOBAL: SHANDALAR 0x005a7ba0
int DAT_007081a8;
// GLOBAL: MAGIC 0x00708108
// GLOBAL: SHANDALAR 0x005a7b00
unsigned int DAT_00708108[7];
// GLOBAL: MAGIC 0x00708168
// GLOBAL: SHANDALAR 0x005a7b60
int DAT_00708168[16];
// GLOBAL: MAGIC 0x007081b0
// GLOBAL: SHANDALAR 0x005a7ba8
unsigned int DAT_007081b0[7];
// GLOBAL: MAGIC 0x00707ff8
// GLOBAL: SHANDALAR 0x005a79f0
int DAT_00707ff8;
// GLOBAL: MAGIC 0x00707ffc
// GLOBAL: SHANDALAR 0x005a79f4
unsigned int DAT_00707ffc;
// GLOBAL: MAGIC 0x00707e20
// GLOBAL: SHANDALAR 0x005a7818
int _DAT_00707e20;
// GLOBAL: MAGIC 0x00707ee8
// GLOBAL: SHANDALAR 0x005a78e0
int DAT_00707ee8;
// GLOBAL: MAGIC 0x00707fb8
// GLOBAL: SHANDALAR 0x005a79b0
int ai_candidate_block_assignments[16];
// GLOBAL: MAGIC 0x007081d0
// GLOBAL: SHANDALAR 0x005a7bc8
int ai_attacker_blocker_counts[16];
// GLOBAL: MAGIC 0x00708720
// GLOBAL: SHANDALAR 0x005a8118
int DAT_00708720;
// GLOBAL: MAGIC 0x00708724
// GLOBAL: SHANDALAR 0x005a811c
unsigned int DAT_00708724;
// GLOBAL: MAGIC 0x00708728
// GLOBAL: SHANDALAR 0x005a8120
int DAT_00708728[16];
// GLOBAL: MAGIC 0x00707c98
// GLOBAL: SHANDALAR 0x005a7690
int DAT_00707c98[16];

// GLOBAL: MAGIC 0x00707bf8
// GLOBAL: SHANDALAR 0x005a75f0
unsigned int DAT_00707bf8;

// GLOBAL: MAGIC 0x00707dd8
// GLOBAL: SHANDALAR 0x005a77d0
int DAT_00707dd8;

// GLOBAL: MAGIC 0x0070a830
// GLOBAL: SHANDALAR 0x0097db20
int untapped_royal_assassin_count;

// GLOBAL: MAGIC 0x0070a834
// GLOBAL: SHANDALAR 0x0097db24
int defending_cop_color_mask;

// GLOBAL: MAGIC 0x0070a838
// GLOBAL: SHANDALAR 0x0097db2c
int defending_cop_available_mana;

// GLOBAL: MAGIC 0x0072c8f0
// GLOBAL: SHANDALAR 0x00765e30
static card_instance_t combat_sim_saved_card_instances[2][151];
// GLOBAL: MAGIC 0x00742ae0
// GLOBAL: SHANDALAR 0x0077c070
static card_data_t combat_sim_saved_dynamic_card_data[16];
// GLOBAL: MAGIC 0x0069fbc8
// GLOBAL: SHANDALAR 0x005abeb0
static int combat_sim_saved_library[2][500];
// GLOBAL: MAGIC 0x006a4ef0
// GLOBAL: SHANDALAR 0x005b11d8
static int combat_sim_saved_graveyard_slots[2][500];
// GLOBAL: MAGIC 0x006aa3c8
// GLOBAL: SHANDALAR 0x005b66b0
static int combat_sim_saved_exile[2][500];
// GLOBAL: MAGIC 0x006aa380
// GLOBAL: SHANDALAR 0x005b6668
static int combat_sim_saved_special_mana_pool[2][8];
// GLOBAL: MAGIC 0x006a4eb0
// GLOBAL: SHANDALAR 0x005b1198
static int combat_sim_saved_raw_mana_available[2][8];
// GLOBAL: MAGIC 0x006a4e20
// GLOBAL: SHANDALAR 0x005b1108
static int combat_sim_saved_basiclandtypes_controlled[2][8];
// GLOBAL: MAGIC 0x006a0b70
// GLOBAL: SHANDALAR 0x005ace58
static int combat_sim_saved_raw_mana_available_hex[2][51];
// GLOBAL: MAGIC 0x0069e3b0
// GLOBAL: SHANDALAR 0x005aa698
static int combat_sim_saved_life[2];
// GLOBAL: MAGIC 0x006a4e60
// GLOBAL: SHANDALAR 0x005b1148
static int combat_sim_saved_DAT_007abce0[2];
// GLOBAL: MAGIC 0x006a4e88
// GLOBAL: SHANDALAR 0x005b1170
static unsigned int combat_sim_saved_card_types_in_play[2];
// GLOBAL: MAGIC 0x006a4e90
// GLOBAL: SHANDALAR 0x005b1178
static int combat_sim_saved_graveyard_card_types[2];
// GLOBAL: MAGIC 0x006a72f0
// GLOBAL: SHANDALAR 0x005b35d8
static int combat_sim_saved_duel_summary[24];
// GLOBAL: MAGIC 0x006a5e90
// GLOBAL: SHANDALAR 0x005b2178
static int combat_sim_saved_ante_cards[2][16];
// GLOBAL: MAGIC 0x006a9b38
// GLOBAL: SHANDALAR 0x005b5e20
static int combat_sim_saved_timestamp_player[500];
// GLOBAL: MAGIC 0x0069e3b8
// GLOBAL: SHANDALAR 0x005aa6a0
static int combat_sim_saved_timestamp_card[500];
// GLOBAL: MAGIC 0x006a4d18
// GLOBAL: SHANDALAR 0x005b1000
static int combat_sim_saved_land_can_be_played;
// GLOBAL: MAGIC 0x006a8a64
// GLOBAL: SHANDALAR 0x005b4d4c
static phase_t combat_sim_saved_current_phase;
// GLOBAL: MAGIC 0x006a4e9c
// GLOBAL: SHANDALAR 0x005b1184
static int combat_sim_saved_current_casting_internal_card_id;
// GLOBAL: MAGIC 0x006a4ea8
// GLOBAL: SHANDALAR 0x005b1190
static int combat_sim_saved_attacking_creature_count;
// GLOBAL: MAGIC 0x006a7350
// GLOBAL: SHANDALAR 0x005b3638
static int combat_sim_saved_spell_fizzled;
// GLOBAL: MAGIC 0x0069fb40
// GLOBAL: SHANDALAR 0x005abe28
static int combat_sim_saved_stack_data[32];
// GLOBAL: MAGIC 0x006a7bb8
// GLOBAL: SHANDALAR 0x005b3ea0
static target_t combat_sim_saved_stack_cards[32];
// GLOBAL: MAGIC 0x006a4c18
// GLOBAL: SHANDALAR 0x005b0f00
static target_t combat_sim_saved_stack_damage_targets[32];
// GLOBAL: MAGIC 0x006a4ea0
// GLOBAL: SHANDALAR 0x005b1188
static int combat_sim_saved_active_cards_count[2];
// GLOBAL: MAGIC 0x006a7ba8
// GLOBAL: SHANDALAR 0x005b3e90
static int combat_sim_saved_stack_size;
// GLOBAL: MAGIC 0x006a0d50
// GLOBAL: SHANDALAR 0x005ad038
static int(__cdecl *combat_sim_saved_pending_killed_card_handler)(int, int);
// GLOBAL: MAGIC 0x006a0d08
// GLOBAL: SHANDALAR 0x005acff0
static int combat_sim_saved_ai_modifier;
// GLOBAL: MAGIC 0x006aa310
// GLOBAL: SHANDALAR 0x005b65f8
static int combat_sim_saved_mana_charge[8];
// GLOBAL: MAGIC 0x006a7bac
// GLOBAL: SHANDALAR 0x005b3e94
static int combat_sim_saved_x_value;
// GLOBAL: MAGIC 0x006a9b28
// GLOBAL: SHANDALAR 0x005b5e10
static int combat_sim_saved_max_x_value;
// GLOBAL: MAGIC 0x006a7bb4
// GLOBAL: SHANDALAR 0x005b3e9c
static int combat_sim_saved_extra_turn_player;
// GLOBAL: MAGIC 0x006a8658
// GLOBAL: SHANDALAR 0x005b4940
static int combat_sim_saved_DAT_0093a848;
// GLOBAL: MAGIC 0x006a8a60
// GLOBAL: SHANDALAR 0x005b4d48
static phase_t combat_sim_saved_stop_phase;
// GLOBAL: MAGIC 0x006aa330
// GLOBAL: SHANDALAR 0x005b6618
static int combat_sim_saved_stop_phase_player;
// GLOBAL: MAGIC 0x006a25b8
// GLOBAL: SHANDALAR 0x005ae8a0
static int combat_sim_saved_battlefield_extra_ability_flags;
// GLOBAL: MAGIC 0x006aa308
// GLOBAL: SHANDALAR 0x005b65f0
static int combat_sim_saved_unk_008b44d0[2];
// GLOBAL: MAGIC 0x006a4c10
// GLOBAL: SHANDALAR 0x005b0ef8
static int combat_sim_saved_unk_007a79b0[2];

// GLOBAL: MAGIC 0x00709570
// GLOBAL: SHANDALAR 0x0097c860
ai_combat_eval_entry_t ai_combat_eval_table[2][150];

// GLOBAL: MAGIC 0x008cefac
// GLOBAL: SHANDALAR 0x0097db28
int combat_eval_power;

// GLOBAL: MAGIC 0x0093b278
// GLOBAL: SHANDALAR 0x0097db38
int combat_eval_toughness;

// GLOBAL: MAGIC 0x0092608c
// GLOBAL: SHANDALAR 0x0097db34
int combat_eval_abilities;

// GLOBAL: MAGIC 0x0069fbc0
// GLOBAL: SHANDALAR 0x005abea8
int ai_castable_phase_mask;

// GLOBAL: MAGIC 0x006a72e8
// GLOBAL: SHANDALAR 0x005b35d0
int ai_activatable_phase_mask;

// GLOBAL: MAGIC 0x0057aaec
// GLOBAL: SHANDALAR 0x00589a3c
int ai_search_candidate_offset = 0;

// GLOBAL: MAGIC 0x0057dd20
// GLOBAL: SHANDALAR 0x00581808
int DAT_0057dd20[16] = {4, 4, 4, 4, 3, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};

// FUNCTION: MAGIC 0x004e3380
// FUNCTION: SHANDALAR 0x004c25f0
int choose_ai_main_phase_action(int player)
{
  struct
  {
    unsigned int land_colors_needed;
    unsigned int candidates[20];
    int color;
    unsigned int result;
    unsigned int selected_color;
    int fewest_controlled;
    int nonbasic_land_count;
    unsigned int playable_types;
    int opponent;
    int card;
    unsigned int possible_land_colors;
    unsigned int preferred_land_color;
    int internal_card_id;
    int candidate_count;
    unsigned int exact_land_colors_needed;
    int ai_depth;
    int colorless_land;
    int playable_card_count;
    unsigned int missing_land_colors;
  } s;

  if ((g_duel_ai_mode_state != 1) && (ai_action_replay_available == 0))
  {
    return -1;
  }

  s.opponent = 1 - player;
  s.playable_card_count = 0;
  s.nonbasic_land_count = s.playable_card_count;
  s.missing_land_colors = s.nonbasic_land_count;
  s.possible_land_colors = s.missing_land_colors;
  s.land_colors_needed = s.possible_land_colors;
  s.playable_types = 2;
  if ((land_can_be_played & 1U) || (duel_summary.hand_counts[player] + unk_007161d8 <= 0))
  {
  }
  else
  {
    for (s.card = 0; (int)s.card < active_cards_count[player]; s.card++)
    {
      if (global_card_instances[player][s.card].internal_card_id != -1)
      {
        if (((global_card_instances[player][s.card].state & (STATE_OUBLIETTED | STATE_IN_PLAY)) == STATE_IN_PLAY) &&
            ((global_cards_data[global_card_instances[player][s.card].internal_card_id].type & 2) != 0))
        {
          s.playable_types |= 0x7c;
        }
        if ((global_card_instances[player][s.card].state & (STATE_IN_PLAY | STATE_TAPPED)) == 0)
        {
          if ((global_cards_data[global_card_instances[player][s.card].internal_card_id].type & 1) != 0)
          {
            s.missing_land_colors = 1;
            if (global_card_instances[player][s.card].mana_color != 0)
            {
              s.nonbasic_land_count++;
            }
          }
          if (global_cards_data[global_card_instances[player][s.card].internal_card_id].cc[1] == -1)
          {
            s.nonbasic_land_count += 99;
          }
          s.playable_card_count++;
        }
      }
    }
    if (s.missing_land_colors == 0)
    {
    }
    else
    {
      s.missing_land_colors = s.colorless_land = 0;
      for (s.card = 0; (int)s.card < active_cards_count[player]; s.card++)
      {
        s.internal_card_id = global_card_instances[player][s.card].internal_card_id;
        if (s.internal_card_id == -1)
        {
          continue;
        }

        if ((global_card_instances[player][s.card].state & (STATE_IN_PLAY | STATE_TAPPED)) == 0)
        {
          s.selected_color = (unsigned int)global_cards_data[s.internal_card_id].color;
          if ((global_cards_data[s.internal_card_id].type & 1) != 0)
          {
            s.land_colors_needed |= s.selected_color;
            if (s.selected_color == 0)
            {
              s.colorless_land = 1;
            }
          }
          if ((s.playable_types & global_cards_data[s.internal_card_id].type) != 0)
          {
            for (s.color = 1; s.color < 6; s.color++)
            {
              if ((s.selected_color & (1 << (unsigned char)s.color)) == 0)
              {
                continue;
              }

              if (basiclandtypes_controlled[player][s.color] + 1 == (int)global_cards_data[s.internal_card_id].cc[0])
              {
                s.possible_land_colors |= 1 << (unsigned char)s.color;
              }
              if (basiclandtypes_controlled[player][s.color] + 1 < (int)global_cards_data[s.internal_card_id].cc[0])
              {
                s.missing_land_colors |= 1 << (unsigned char)s.color;
              }
              if (basiclandtypes_controlled[player][7] < abs((int)global_cards_data[s.internal_card_id].cc[1]) + global_cards_data[s.internal_card_id].cc[0])
              {
                s.missing_land_colors |= 0xff;
              }
            }
          }
        }
      }
      s.fewest_controlled = 999;
      s.preferred_land_color = s.land_colors_needed;
      for (s.card = 0; (int)s.card < 6; s.card++)
      {
        if (0 < ai_mana_demand_by_color[player][s.card])
        {
          s.missing_land_colors |= 1 << (unsigned char)s.card;
        }
        if (((s.land_colors_needed & (1 << (unsigned char)s.card)) != 0) &&
            (basiclandtypes_controlled[player][s.card] - ai_mana_demand_by_color[player][s.card] < s.fewest_controlled))
        {
          s.fewest_controlled = basiclandtypes_controlled[player][s.card] - ai_mana_demand_by_color[player][s.card];
          s.preferred_land_color = 1 << (unsigned char)s.card;
        }
      }
      if (s.land_colors_needed != 0)
      {
        s.exact_land_colors_needed = s.preferred_land_color;
        s.playable_types = s.missing_land_colors & s.land_colors_needed;
        if (s.playable_types != 0)
        {
          s.exact_land_colors_needed = s.playable_types;
        }
        s.playable_types = s.possible_land_colors & s.land_colors_needed;
        if (s.playable_types != 0)
        {
          s.exact_land_colors_needed = s.playable_types;
        }
        if ((s.playable_types == 0) && (s.colorless_land != 0))
        {
          s.selected_color = 0xffffffff;
        }
        else
        {
          do
          {
            s.selected_color = internal_rand(7);
          } while ((s.exact_land_colors_needed & (1 << (unsigned char)s.selected_color)) == 0);
        }
        if (g_duel_ai_mode_state == 1)
        {
          ai_recorded_choice = s.selected_color;
          ai_recorded_action = -1;
          if ((internal_rand(4) == 0) && (ai_search_force_pass == 0))
          {
            ai_recorded_choice = -2;
          }
          if ((((s.possible_land_colors == 0) && (s.missing_land_colors == 0)) && (s.colorless_land == 0)) &&
              ((s.playable_card_count < 7) && (4 <= basiclandtypes_controlled[player][7] - s.nonbasic_land_count)))
          {
            ai_recorded_choice = -2;
          }
        }
        else
        {
          ai_recorded_action_type = 1;
          replay_ai_action_selection();
          s.selected_color = ai_recorded_choice;
        }
      }
      else
      {
        if ((g_duel_ai_mode_state != 1) && (s.colorless_land != 0))
        {
          ai_recorded_action_type = 1;
          replay_ai_action_selection();
        }
        s.selected_color = 99;
      }
      s.result = 0xffffffff;
      for (s.card = 0; (int)s.card < active_cards_count[player]; s.card++)
      {
        s.internal_card_id = global_card_instances[player][s.card].internal_card_id;
        if (((((s.internal_card_id != -1) && ((global_card_instances[player][s.card].state & (STATE_IN_PLAY | STATE_TAPPED)) == 0)) &&
              ((global_cards_data[s.internal_card_id].type & 1) != 0)) &&
             ((((s.selected_color == 0xffffffff && (global_card_instances[player][s.card].mana_color == 0)) ||
                (((1 << (unsigned char)s.selected_color) & (int)(char)global_card_instances[player][s.card].mana_color) != 0)) ||
               (s.land_colors_needed == 0)))) &&
            ((s.result == 0xffffffff) || (4 < s.internal_card_id)))
        {
          s.result = s.card;
        }
      }
      if ((g_duel_ai_mode_state == 1) && ((s.land_colors_needed != 0) || (s.colorless_land != 0)))
      {
        if (ai_recorded_choice == -2)
        {
          ai_recorded_action = -1;
        }
        else
        {
          ai_recorded_action = ((player << 8) | s.result) | 0x1000;
        }
        ai_recorded_action_type = 1;
        record_ai_action_selection();
        if (ai_recorded_choice != -2)
        {
          if (DAT_008a8de4 >= 0x10)
          {
            DAT_008a8de4--;
          }
          DAT_0091bf70[DAT_008a8de4] = global_card_instances[player][s.result].internal_card_id;
          DAT_007abc00[DAT_008a8de4] = s.result;
          DAT_008a8de4++;
        }
      }
      if ((ai_recorded_choice == -2) && ((s.land_colors_needed != 0) || (s.colorless_land != 0)))
      {
        land_can_be_played |= 1;
      }
      else
      {
        return s.result;
      }
    }
  }
  s.candidate_count = 0;
  ai_castable_phase_mask = 2;
  ai_activatable_phase_mask = 0x20;
  if (PHASE_DECLARE_BLOCKERS <= current_phase)
  {
    ai_castable_phase_mask = 4;
    ai_activatable_phase_mask = 0x40;
  }
  if (current_phase <= PHASE_MAIN1)
  {
    ai_castable_phase_mask = 1;
    ai_activatable_phase_mask = 0x10;
  }
  if (PHASE_MAIN2 <= current_phase)
  {
    ai_castable_phase_mask = 8;
    ai_activatable_phase_mask = 0xffffff80;
  }
  if (current_phase == PHASE_DISCARD)
  {
    ai_castable_phase_mask = 0xf;
    ai_activatable_phase_mask = 0xfffffff0;
  }
  for (s.card = 0; (int)s.card < active_cards_count[player]; s.card++)
  {
    s.internal_card_id = global_card_instances[player][s.card].internal_card_id;
    if (s.internal_card_id == -1)
    {
      continue;
    }

    if ((global_card_instances[player][s.card].state & STATE_IN_PLAY) == 0)
    {
      if (((ai_castable_phase_mask & (int)(char)global_cards_data[s.internal_card_id].act_phases) != 0) &&
          ((global_cards_data[s.internal_card_id].type & 0x7e) != 0))
      {
        s.selected_color = single_color_test_bit_to_color_t((int)global_cards_data[s.internal_card_id].color);
        if ((can_pay_card_mana_cost(player, player, s.card) != 0) &&
            (((global_cards_data[s.internal_card_id].type & 0x3c) == 0) ||
             ((*global_cards_data[s.internal_card_id].code_pointer)(player, s.card, 0x74) != 0)))
        {
          s.candidates[s.candidate_count] = s.card;
          s.candidate_count++;
        }
      }
    }
    else if (((((global_card_instances[player][s.card].state & (STATE_ATTACKING | STATE_TAPPED | STATE_INVISIBLE)) == 0) &&
               ((global_cards_data[s.internal_card_id].extra_ability & 0x1003U) != 0x1000)) &&
              ((ai_activatable_phase_mask & (int)(char)global_cards_data[s.internal_card_id].act_phases) != 0)) &&
             (dispatch_event_to_single_card(player, s.card, 0x73, 1 - player, -1) != 0))
    {
      s.candidates[s.candidate_count] = s.card;
      s.candidate_count++;
    }
  }
  if (s.candidate_count == 0)
  {
    ai_search_target_depth = -1;
    ai_search_candidate_offset = 0;
    if (ai_search_force_pass == 1)
    {
      ai_search_time_limit = -1;
    }
    return -1;
  }

  if (g_duel_ai_mode_state == 1)
  {
    if (ai_search_best_score == -9999)
    {
      ai_search_flags = 0;
    }
    if ((ai_search_try_count == 0x19) && (s.candidate_count < 3))
    {
      ai_search_flags |= 1;
    }
    ai_recorded_choice = ai_search_flags;
    record_ai_action_selection();
  }
  else
  {
    replay_ai_action_selection();
    ai_search_flags = ai_recorded_choice;
  }
  s.candidates[s.candidate_count] = 0xffffffff;
  s.candidate_count++;
  if (g_duel_ai_mode_state == 1)
  {
    if ((ai_search_target_depth == -1) || (ai_search_force_pass != 0))
    {
      ai_recorded_choice = internal_rand(s.candidate_count);
      if (ai_search_force_pass != 0)
      {
        ai_recorded_choice = s.candidate_count - 1;
        if (ai_search_force_pass == 1)
        {
          ai_search_time_limit = ClampIntToRange(s.candidate_count * s.candidate_count, 10, 0x14) * (g_shandalar_difficulty + 1) * 5;
        }
        ai_search_force_pass = -1;
      }
      ai_recorded_action = (((global_card_instances[player][s.candidates[ai_recorded_choice]].state & 2) == 0) ? 0x1000 : 0x2000) |
                           s.candidates[ai_recorded_choice] | (player != 0 ? 0x100 : 0);
      ai_recorded_action_type = 2;
      record_ai_action_selection();
    }
    else
    {
      s.ai_depth = get_recorded_action_count();
      if ((ai_search_candidate_offset == 0) && (ai_search_target_depth < s.ai_depth))
      {
        ai_search_target_depth = s.ai_depth;
      }
      if (s.ai_depth == ai_search_target_depth)
      {
        ai_recorded_choice = ai_search_candidate_offset;
        ai_search_candidate_offset++;
        if (s.candidate_count - 1 <= ai_recorded_choice)
        {
          ai_search_target_depth++;
          ai_search_candidate_offset = 0;
        }
      }
      if (ai_search_target_depth < s.ai_depth)
      {
        ai_recorded_choice = s.candidate_count - 1;
      }
      if (s.ai_depth < ai_search_target_depth)
      {
        ai_recorded_action_type = 2;
        replay_ai_action_selection();
        rewind_recorded_action();
        if (ai_recorded_choice >= s.candidate_count)
        {
          ai_recorded_choice = s.candidate_count - 1;
        }
      }
      ai_recorded_action = (((global_card_instances[player][s.candidates[ai_recorded_choice]].state & 2) == 0) ? 0x1000 : 0x2000) |
                           s.candidates[ai_recorded_choice] | (player != 0 ? 0x100 : 0);
      ai_recorded_action_type = 2;
      record_ai_action_selection();
      if ((s.candidate_count - 1 == ai_recorded_choice) && (s.ai_depth < ai_search_target_depth))
      {
        ai_search_target_depth = -1;
        ai_search_candidate_offset = 0;
      }
    }
  }
  else
  {
    ai_recorded_action_type = 2;
    replay_ai_action_selection();
    if (ai_recorded_choice >= s.candidate_count)
    {
      ai_recorded_choice = s.candidate_count - 1;
    }
  }
  if (s.candidates[ai_recorded_choice] != 0xffffffff)
  {
    if (DAT_008a8de4 >= 0x10)
    {
      DAT_008a8de4--;
    }
    DAT_0091bf70[DAT_008a8de4] = global_card_instances[player][s.candidates[ai_recorded_choice]].internal_card_id;
    DAT_007abc00[DAT_008a8de4] = s.candidates[ai_recorded_choice];
    DAT_008a8de4++;
  }

  return s.candidates[ai_recorded_choice];
  return -1;
}

// FUNCTION: MAGIC 0x004e4f11
void record_ai_action_selection(void)
{
  if (recorded_action_count < 0x100)
  {
    unk_006a21b8[recorded_action_count] = ai_recorded_action;
    unk_006a8258[recorded_action_count] = PLAYER_CARD_INSTANCE((ai_recorded_action & 0x100) >> 8, ai_recorded_action & 0xff).internal_card_id;
    unk_006a8660[recorded_action_count] = ai_recorded_action_type;
    unk_006a6ee8[recorded_action_count] = ai_recorded_choice;
    ++recorded_action_count;
    if (unk_006a6ee8[0] == 99 || recorded_action_codes[0] == 99)
    {
      ai_recorded_action = -1;
    }
  }
  else
  {
    spell_fizzled = 1;
  }

  ai_recorded_action_type = 0;
}

// FUNCTION: MAGIC 0x004e5089
void replay_ai_action_selection(void)
{
  if (unk_006a5f18[recorded_action_count] != ai_recorded_action_type)
  {
    ai_recorded_action_type |= 0x100;
  }

  ai_recorded_action = unk_006a1db8[recorded_action_count];
  ai_recorded_choice = recorded_action_codes[recorded_action_count];
  if (ai_recorded_choice != 99)
  {
    ++recorded_action_count;
  }

  ai_recorded_action_type = 0;
}

// FUNCTION: MAGIC 0x004e4e9a
// FUNCTION: SHANDALAR 0x004c410a
void __stdcall reset_ai_search_trial_state(void)
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
// FUNCTION: SHANDALAR 0x004c4364
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
  ai_action_replay_available = 1;
}

// FUNCTION: MAGIC 0x004e51e8
// FUNCTION: SHANDALAR 0x004c4457
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
  s.score += (ai_combat_value_weights[player] * s.life_score) / 8;

  s.life_score = 0;
  for (s.card = 1; s.card <= life[s.opponent]; s.card = s.card + 1)
  {
    s.life_score += 0x18 / s.card + 0xc;
  }
  s.score -= (ai_combat_value_weights[s.opponent] * s.life_score) / 8;

  if (life[player] <= 0)
  {
    s.score += (life[player] * 4 - 8) * 0x4b;
  }
  if (life[s.opponent] <= 0)
  {
    s.score -= (life[s.opponent] - 2) * 0x100;
  }

  if (g_reveal_all_world_info != 0)
  {
    strcpy(g_ui_message_buffer, "");
  }

  for (s.current_player = 0; s.current_player < 2; s.current_player = s.current_player + 1)
  {
    s.opponent = 1 - s.current_player;
    s.side_score = -(((other_player == s.current_player ? 0x30 : 0) + 0x18) *
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
            current_player == s.current_player)
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
            player == current_player &&
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
            s.side_score += (ai_combat_value_weights[s.opponent] * s.damage_power * 0x18) / 0x10;
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
      if ((g_reveal_all_world_info & 2) != 0 && s.current_player + 2 == g_reveal_all_world_info)
      {
        append_displayed_card_name(s.current_player, s.card);
        strcat(g_ui_message_buffer, " ");
        strcat(g_ui_message_buffer, _itoa(s.card_value, ai_action_dialog_number_buffer, 10));
        strcat(g_ui_message_buffer, "\n");
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

  if (DAT_0093f4b0 == 0 && player == current_player)
  {
    s.score = ai_opinion_of_gamestate_continued(player, s.score);
  }
  unk_00712938 = 0;
  return s.score;
}

// FUNCTION: MAGIC 0x004e5db4
// FUNCTION: SHANDALAR 0x004c501b
int ai_opinion_of_gamestate_continued(int player, int score)
{
  struct
  {
    int damage_to_player_score;
    int blocker_trade_score;
    int attacker_power;
    int attacker_toughness;
    unsigned int block_result;
    int best_blocker;
    int test_card;
    int opponent_player;
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

  for (s.card = 0; s.card <= 7; s.card = s.card + 1)
  {
    s.saved_special_mana[s.card] = (char)special_mana_pool[s.opponent_player][s.card];
    special_mana_pool[s.opponent_player][s.card] = basiclandtypes_controlled[s.opponent_player][s.card];
  }

  for (s.ranked_index = 0; s.ranked_index < 8; s.ranked_index = s.ranked_index + 1)
  {
    s.ranked_attackers[s.ranked_index * 3 + 1] = -1;
  }

  s.creature_count = 0;
  for (s.test_card = 0; s.test_card < active_cards_count[player]; s.test_card = s.test_card + 1)
  {
    if (global_card_instances[player][s.test_card].internal_card_id == -1 ||
        (global_card_instances[player][s.test_card].state & (STATE_IN_PLAY | STATE_PLAYED_FROM_HAND)) == 0 ||
        (global_card_instances[player][s.test_card].state & STATE_OUBLIETTED) != 0 ||
        (global_cards_data[global_card_instances[player][s.test_card].internal_card_id].type & TYPE_CREATURE) == 0)
    {
      continue;
    }

    s.creature_index_by_card[s.test_card] = (char)s.creature_count;
    s.blocker_power[s.creature_count] = C_get_abilities(player, s.test_card, EVENT_POWER, -1);
    s.blocker_toughness[s.creature_count] = C_get_abilities(player, s.test_card, EVENT_TOUGHNESS, -1);
    s.blocker_scores[s.creature_count] = (int)global_card_instances[player][s.test_card].attack_rating;
    s.creature_count++;
  }

  for (s.card = 0; s.card < active_cards_count[s.opponent_player]; s.card = s.card + 1)
  {
    s.internal_card_id = global_card_instances[s.opponent_player][s.card].internal_card_id;
    if (is_in_play(s.opponent_player, s.card) == 0 ||
        (global_cards_data[s.internal_card_id].type & TYPE_CREATURE) == 0)
    {
      continue;
    }

    if ((global_cards_data[s.internal_card_id].subtype == 0) &&
        (global_card_instances[s.opponent_player][s.card].token_status & 0x800) == 0)
    {
      continue;
    }

    s.attacker_power = C_get_abilities(s.opponent_player, s.card, EVENT_POWER, -1);
    s.attacker_toughness = C_get_abilities(s.opponent_player, s.card, EVENT_TOUGHNESS, -1);
    if (active_player == s.opponent_player)
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

    for (s.test_card = 0; s.test_card < 8; s.test_card++)
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
    }
  }

  s.expected_damage = 0;
  for (s.ranked_index = 0; s.ranked_index < 8; s.ranked_index++)
  {
    if (s.ranked_attackers[s.ranked_index * 3 + 1] == -1)
    {
      break;
    }

    s.card = s.ranked_attackers[s.ranked_index * 3];
    s.attacker_abilities = C_get_abilities(s.opponent_player, s.card, EVENT_ABILITIES, -1);
    s.attacker_power = s.ranked_attackers[s.ranked_index * 3 + 1];
    s.attacker_toughness = s.ranked_attackers[s.ranked_index * 3 + 2];
    s.best_blocker = 0;
    s.block_result = s.best_blocker;
    s.creature_index = 0;
    s.smallest_blocker_score = 0x7fff;

    for (s.test_card = 0; s.test_card < active_cards_count[player]; s.test_card = s.test_card + 1)
    {
      if (is_in_play(player, s.test_card) == 0 || (global_cards_data[global_card_instances[player][s.test_card].internal_card_id].type & TYPE_CREATURE) == 0)
      {
        continue;
      }
      s.creature_index = (int)s.creature_index_by_card[s.test_card];
      s.test_power = s.blocker_power[s.creature_index];
      s.test_toughness = s.blocker_toughness[s.creature_index];
      if ((global_card_instances[player][s.test_card].state &
           ((has_vigilance(player, s.test_card) == 0 ? STATE_ATTACKING : 0) + STATE_BLOCKING)) != 0)
      {
        continue;
      }
      if (can_block_attacker_with_abilities(player,
                                            s.test_card,
                                            s.opponent_player,
                                            s.card,
                                            s.attacker_abilities,
                                            s.landwalk_mask) == 0)
      {
        continue;
      }
      s.block_result |= 1;
      if (s.attacker_power < s.test_toughness || s.attacker_toughness <= s.test_power)
      {
        s.block_result |= 2;
        global_card_instances[player][s.test_card].state |= STATE_BLOCKING;
        break;
      }
      if (s.blocker_scores[s.creature_index] < s.smallest_blocker_score)
      {
        s.smallest_blocker_score = s.blocker_scores[s.creature_index];
        s.best_blocker = s.test_card;
      }
    }

    if ((s.block_result & 2) == 0)
    {
      s.damage_to_player_score = ((ai_combat_value_weights[player] * s.attacker_power * 0x18) / 4) / ClampIntToRange(life[player] + 1, 1, 99);
      s.blocker_trade_score = (unk_007a7d18[player] * s.smallest_blocker_score) / 0x10;
      if (s.block_result == 0 || (s.damage_to_player_score < s.blocker_trade_score && life[player] > s.expected_damage + s.attacker_power))
      {
        s.expected_damage += s.attacker_power;
        score -= s.damage_to_player_score;
      }
      else
      {
        score -= s.blocker_trade_score;
        global_card_instances[player][s.best_blocker].state |= STATE_BLOCKING;
      }
    }
  }

  if (life[player] <= s.expected_damage && life[s.opponent_player] > 0)
  {
    score -= 0x100;
  }

  for (s.card = 0; s.card <= 7; s.card = s.card + 1)
  {
    special_mana_pool[s.opponent_player][s.card] = (int)s.saved_special_mana[s.card];
  }

  return score;
}

// FUNCTION: MAGIC 0x004e6754
// FUNCTION: SHANDALAR 0x004c59bb
int show_ai_action_log_dialog(int use_saved_actions, int score)
{
  struct
  {
    int internal_card_id;
    int action_count;
    unsigned int action_flags;
    int action_index;
  } s;

  strcpy(g_ui_message_buffer, "AI:");
  strcat(g_ui_message_buffer, _itoa(score, ai_action_dialog_number_buffer, 10));
  strcat(g_ui_message_buffer, " L:");
  strcat(g_ui_message_buffer, _itoa(life[0], ai_action_dialog_number_buffer, 10));
  strcat(g_ui_message_buffer, "/");
  strcat(g_ui_message_buffer, _itoa(life[1], ai_action_dialog_number_buffer, 10));
  strcat(g_ui_message_buffer, " ...\n");

  for (s.action_index = 0; (s.action_count = use_saved_actions != 0 ? saved_recorded_action_count : recorded_action_count) > s.action_index; s.action_index++)
  {
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
        strcat(g_ui_message_buffer, "Cast ");
      }
      if ((s.action_flags & 0x2000) != 0)
      {
        strcat(g_ui_message_buffer, "Tap ");
      }
      if ((s.action_flags & 0x4000) != 0)
      {
        strcat(g_ui_message_buffer, "...target ");
      }
      if ((s.action_flags & 0x100) == 0)
      {
        strcat(g_ui_message_buffer, "e");
      }
      if ((s.action_flags & 0xff) == 0xff)
      {
        strcat(g_ui_message_buffer, "Player");
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
        strcat(g_ui_message_buffer, global_cards_data[s.internal_card_id].name);
      }
      strcat(g_ui_message_buffer, "\n");
    }
  }

  do_dialog(0, 0, 0, -1, -1, g_ui_message_buffer, 0);
  return 0;
}

// FUNCTION: MAGIC 0x00441cf2
// FUNCTION: SHANDALAR 0x0040d572
void start_ai_decision_search(int decision_code, int time_scale)
{
  ai_search_best_score = -9999;
  g_duel_ai_mode_state = 1;
  ai_decision_code = decision_code;
  ai_search_time_limit = ((g_shandalar_difficulty + 1) * time_scale) / 4;

  DAT_00775d3c = 1 << (unsigned char)(internal_rand(5) + 1);

  save_ai_search_state();
  reset_duel_tick_timer_indirect();
  update_duel_thread_time_marker();

  ai_search_try_count = 0;
  ai_search_force_pass = 1;
  unk_00712938 = 0;
}

// FUNCTION: MAGIC 0x004e430e
// FUNCTION: SHANDALAR 0x004c357e
void save_ai_search_state(void)
{
  saved_recorded_action_count = 0;
  memcpy(g_ai_search_backup.card_instances, global_card_instances, sizeof(g_ai_search_backup.card_instances));
  memcpy(g_ai_search_backup.extra_card_data, global_cards_data + g_card_count, sizeof(g_ai_search_backup.extra_card_data));
  memcpy(g_ai_search_backup.library, global_library, sizeof(g_ai_search_backup.library));
  memcpy(g_ai_search_backup.graveyard, global_graveyard_slots, sizeof(g_ai_search_backup.graveyard));
  memcpy(g_ai_search_backup.exile, global_exile, sizeof(g_ai_search_backup.exile));
  memcpy(g_ai_search_backup.special_mana, special_mana_pool, sizeof(g_ai_search_backup.special_mana));
  memcpy(g_ai_search_backup.raw_mana, raw_mana_available, sizeof(g_ai_search_backup.raw_mana));
  memcpy(g_ai_search_backup.basic_lands, basiclandtypes_controlled, sizeof(g_ai_search_backup.basic_lands));
  memcpy(g_ai_search_backup.raw_mana_hex, raw_mana_available_hex, sizeof(g_ai_search_backup.raw_mana_hex));
  memcpy(g_ai_search_backup.life_totals, life, sizeof(g_ai_search_backup.life_totals));
  memcpy(g_ai_search_backup.DAT_007abce0_pair, &DAT_007abce0, sizeof(g_ai_search_backup.DAT_007abce0_pair));
  memcpy(g_ai_search_backup.card_types, card_types_in_play, sizeof(g_ai_search_backup.card_types));
  memcpy(g_ai_search_backup.graveyard_type_flags, graveyard_card_types, sizeof(g_ai_search_backup.graveyard_type_flags));
  memcpy(g_ai_search_backup.duel_summary_copy, &duel_summary, sizeof(g_ai_search_backup.duel_summary_copy));
  memcpy(g_ai_search_backup.hand_counts, duel_summary.hand_counts, sizeof(g_ai_search_backup.hand_counts));
  memcpy(g_ai_search_backup.creature_counts, duel_summary.creature_counts, sizeof(g_ai_search_backup.creature_counts));
  memcpy(g_ai_search_backup.artifact_counts, duel_summary.artifact_counts, sizeof(g_ai_search_backup.artifact_counts));
  memcpy(g_ai_search_backup.enchantment_counts, duel_summary.enchantment_counts, sizeof(g_ai_search_backup.enchantment_counts));
  g_ai_search_backup.cards_drawn_value = duel_summary.cards_drawn;
  g_ai_search_backup.creatures_died_this_turn_value = duel_summary.creatures_died;
  g_ai_search_backup.land_entries_this_turn_value = duel_summary.land_entries;
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
  g_ai_search_backup.DAT_007ab2cc_value = pending_killed_card_handler;
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
#ifndef SHANDALAR
  StopWorldLocationMusic();
#else
  refresh_ai_random_table();
#endif
}

// FUNCTION: MAGIC 0x004e4632
// FUNCTION: SHANDALAR 0x004c38a2
void restore_ai_search_state(void)
{
  memcpy(global_card_instances, g_ai_search_backup.card_instances, sizeof(g_ai_search_backup.card_instances));
  memcpy(global_cards_data + g_card_count, g_ai_search_backup.extra_card_data, sizeof(g_ai_search_backup.extra_card_data));
  memcpy(global_library, g_ai_search_backup.library, sizeof(g_ai_search_backup.library));
  memcpy(global_graveyard_slots, g_ai_search_backup.graveyard, sizeof(g_ai_search_backup.graveyard));
  memcpy(global_exile, g_ai_search_backup.exile, sizeof(g_ai_search_backup.exile));
  memcpy(special_mana_pool, g_ai_search_backup.special_mana, sizeof(g_ai_search_backup.special_mana));
  memcpy(raw_mana_available, g_ai_search_backup.raw_mana, sizeof(g_ai_search_backup.raw_mana));
  memcpy(basiclandtypes_controlled, g_ai_search_backup.basic_lands, sizeof(g_ai_search_backup.basic_lands));
  memcpy(raw_mana_available_hex, g_ai_search_backup.raw_mana_hex, sizeof(g_ai_search_backup.raw_mana_hex));
  memcpy(life, g_ai_search_backup.life_totals, sizeof(g_ai_search_backup.life_totals));
  memcpy(&DAT_007abce0, g_ai_search_backup.DAT_007abce0_pair, sizeof(g_ai_search_backup.DAT_007abce0_pair));
  memcpy(card_types_in_play, g_ai_search_backup.card_types, sizeof(g_ai_search_backup.card_types));
  memcpy(graveyard_card_types, g_ai_search_backup.graveyard_type_flags, sizeof(g_ai_search_backup.graveyard_type_flags));
  memcpy(&duel_summary, g_ai_search_backup.duel_summary_copy, sizeof(g_ai_search_backup.duel_summary_copy));
  memcpy(duel_summary.hand_counts, g_ai_search_backup.hand_counts, sizeof(g_ai_search_backup.hand_counts));
  memcpy(duel_summary.creature_counts, g_ai_search_backup.creature_counts, sizeof(g_ai_search_backup.creature_counts));
  memcpy(duel_summary.artifact_counts, g_ai_search_backup.artifact_counts, sizeof(g_ai_search_backup.artifact_counts));
  memcpy(duel_summary.enchantment_counts, g_ai_search_backup.enchantment_counts, sizeof(g_ai_search_backup.enchantment_counts));
  duel_summary.cards_drawn = g_ai_search_backup.cards_drawn_value;
  duel_summary.creatures_died = g_ai_search_backup.creatures_died_this_turn_value;
  duel_summary.land_entries = g_ai_search_backup.land_entries_this_turn_value;
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
  pending_killed_card_handler = g_ai_search_backup.DAT_007ab2cc_value;
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
  FUN_00464bb0();
}

// FUNCTION: MAGIC 0x004a7ccf
// FUNCTION: SHANDALAR 0x00559e52
int get_ai_search_elapsed_time(void)
{
  unsigned int current_time;

  current_time = get_duel_thread_time_ms();
  return (int)((current_time - DAT_0064ee98) * 100) / 0x151d;
}

// FUNCTION: MAGIC 0x0050e79b
// FUNCTION: SHANDALAR 0x0041d33b
void setup_ai_combat_abilities(int player)
{
  struct
  {
    int attached_player;
    unsigned int saved_state;
    int saved_ai_mode;
    int defending_player;
    int saved_combat_eval_flag;
    int card;
    int mana_available;
    int attached_card;
    int internal_card_id;
  } s;

  s.defending_player = 1 - player;
  s.saved_ai_mode = g_duel_ai_mode_state;
  s.saved_combat_eval_flag = DAT_0093f4b0;
  g_duel_ai_mode_state = 1;
  DAT_0093f4b0 = 1;
  untapped_royal_assassin_count = 0;
  memset(ai_combat_eval_table, 0, sizeof(ai_combat_eval_table));
  for (s.card = 0; s.card < active_cards_count[player]; s.card++)
  {
    if (is_in_play(player, s.card) == 0)
    {
      continue;
    }
    s.internal_card_id = global_card_instances[player][s.card].internal_card_id;
    s.attached_player = (int)(char)global_card_instances[player][s.card].damage_target_player;
    s.attached_card = global_card_instances[player][s.card].damage_target_card;
    if ((global_cards_data[s.internal_card_id].code_pointer == card_regeneration) &&
        has_mana(s.attached_player, COLOR_GREEN, 1) != 0)
    {
      ai_combat_eval_table[s.attached_player][s.attached_card].abilities |= 0x200;
      global_card_instances[s.attached_player][s.attached_card].regen_status |= 0x200;
    }
    else if ((global_cards_data[s.internal_card_id].code_pointer == card_the_brute) &&
             has_mana(s.attached_player, COLOR_RED, 3) != 0)
    {
      ai_combat_eval_table[s.attached_player][s.attached_card].abilities |= 0x200;
      global_card_instances[s.attached_player][s.attached_card].regen_status |= 0x200;
    }
    if (global_cards_data[s.internal_card_id].code_pointer == card_holy_armor)
    {
      ai_combat_eval_table[s.attached_player][s.attached_card].toughness += has_mana(s.attached_player, COLOR_WHITE, 1);
    }
    else if (global_cards_data[s.internal_card_id].code_pointer == card_firebreathing)
    {
      ai_combat_eval_table[s.attached_player][s.attached_card].power += has_mana(s.attached_player, COLOR_RED, 1);
    }
    else if (global_cards_data[s.internal_card_id].code_pointer == card_blessing)
    {
      ai_combat_eval_table[s.attached_player][s.attached_card].power += has_mana(s.attached_player, COLOR_WHITE, 1);
      ai_combat_eval_table[s.attached_player][s.attached_card].toughness += has_mana(s.attached_player, COLOR_WHITE, 1);
    }
    else if (global_cards_data[s.internal_card_id].code_pointer == card_uncle_istvan)
    {
      ai_combat_eval_table[player][s.card].abilities |= 0x1ff800;
    }
    else if (global_cards_data[s.internal_card_id].code_pointer == card_murk_dwellers)
    {
      ai_combat_eval_table[player][s.card].abilities |= 0x80000000;
    }
    else if (global_cards_data[s.internal_card_id].code_pointer == card_gaseous_form)
    {
      ai_combat_eval_table[s.attached_player][s.attached_card].abilities |= 0x1ff800;
      ai_combat_eval_table[s.attached_player][s.attached_card].power = 0xffffff9d;
    }
    global_card_instances[player][s.card].regen_status |= 0xe000000;
    s.saved_state = global_card_instances[player][s.card].state;
    if (other_player == player)
    {
      if (((unsigned char *)&global_card_instances[player][s.card].state)[1] & 0x20)
      {
        global_card_instances[player][s.card].state |= STATE_ATTACKING;
      }
      else
      {
        global_card_instances[player][s.card].state |= STATE_TAPPED | STATE_ATTACKING;
      }
    }
    combat_eval_power = C_get_abilities(player, s.card, EVENT_POWER, -1);
    combat_eval_toughness = C_get_abilities(player, s.card, EVENT_TOUGHNESS, -1);
    combat_eval_abilities = C_get_abilities(player, s.card, EVENT_ABILITIES, -1);
    if (((combat_eval_abilities & 0x200) != 0) &&
        has_mana(player, single_color_test_bit_to_color_t((int)global_cards_data[s.internal_card_id].color), 1) == 0)
    {
      combat_eval_abilities &= 0xfffffdff;
    }
    if (active_player == player)
    {
      dispatch_event(player, s.card, EVENT_CHECK_ABILITIES);
    }
    ai_combat_eval_table[player][s.card].power += combat_eval_power;
    ai_combat_eval_table[player][s.card].toughness += combat_eval_toughness;
    ai_combat_eval_table[player][s.card].abilities |= combat_eval_abilities;
    global_card_instances[player][s.card].state = s.saved_state;
  }
  defending_cop_color_mask = 0;
  for (s.card = 0; s.card < active_cards_count[s.defending_player]; s.card++)
  {
    if (is_in_play(s.defending_player, s.card) == 0)
    {
      continue;
    }
    s.internal_card_id = global_card_instances[s.defending_player][s.card].internal_card_id;
    s.attached_player = (int)(char)global_card_instances[s.defending_player][s.card].damage_target_player;
    s.attached_card = global_card_instances[s.defending_player][s.card].damage_target_card;
    if ((global_cards_data[s.internal_card_id].code_pointer == card_regeneration) &&
        has_mana(s.attached_player, COLOR_GREEN, 1) != 0)
    {
      ai_combat_eval_table[s.attached_player][s.attached_card].abilities |= 0x200;
      global_card_instances[s.attached_player][s.attached_card].regen_status |= 0x200;
    }
    else if ((global_cards_data[s.internal_card_id].code_pointer == card_the_brute) &&
             has_mana(s.attached_player, COLOR_RED, 3) != 0)
    {
      ai_combat_eval_table[s.attached_player][s.attached_card].abilities |= 0x200;
      global_card_instances[s.attached_player][s.attached_card].regen_status |= 0x200;
    }
    if (global_cards_data[s.internal_card_id].code_pointer == card_holy_armor)
    {
      ai_combat_eval_table[s.attached_player][s.attached_card].toughness += has_mana(s.attached_player, COLOR_WHITE, 1);
    }
    else if (global_cards_data[s.internal_card_id].code_pointer == card_firebreathing)
    {
      ai_combat_eval_table[s.attached_player][s.attached_card].power += has_mana(s.attached_player, COLOR_RED, 1);
    }
    else if (global_cards_data[s.internal_card_id].code_pointer == card_blessing)
    {
      ai_combat_eval_table[s.attached_player][s.attached_card].power += has_mana(s.attached_player, COLOR_WHITE, 1);
      ai_combat_eval_table[s.attached_player][s.attached_card].toughness += has_mana(s.attached_player, COLOR_WHITE, 1);
    }
    else if ((global_cards_data[s.internal_card_id].code_pointer == card_royal_assassin) &&
             ((global_card_instances[s.defending_player][s.card].state & (STATE_TAPPED | STATE_INVISIBLE)) == 0))
    {
      untapped_royal_assassin_count++;
    }
    else if (global_cards_data[s.internal_card_id].code_pointer == card_uncle_istvan)
    {
      ai_combat_eval_table[s.defending_player][s.card].abilities |= 0x1ff800;
    }
    else if (global_cards_data[s.internal_card_id].code_pointer == card_murk_dwellers)
    {
      ai_combat_eval_table[s.defending_player][s.card].abilities |= 0x80000000;
    }
    else if (global_cards_data[s.internal_card_id].code_pointer == card_gaseous_form)
    {
      ai_combat_eval_table[s.attached_player][s.attached_card].abilities |= 0x1ff800;
      ai_combat_eval_table[s.attached_player][s.attached_card].power = 0xffffff9d;
    }
    global_card_instances[s.defending_player][s.card].regen_status |= 0xe000000;
    s.saved_state = global_card_instances[s.defending_player][s.card].state;
    global_card_instances[s.defending_player][s.card].state |= STATE_BLOCKING;
    combat_eval_power = C_get_abilities(s.defending_player, s.card, EVENT_POWER, -1);
    combat_eval_toughness = C_get_abilities(s.defending_player, s.card, EVENT_TOUGHNESS, -1);
    combat_eval_abilities = C_get_abilities(s.defending_player, s.card, EVENT_ABILITIES, -1);
    if (((combat_eval_abilities & 0x200) != 0) &&
        has_mana(s.defending_player, single_color_test_bit_to_color_t((int)global_cards_data[s.internal_card_id].color), 1) == 0)
    {
      combat_eval_abilities &= 0xfffffdff;
    }
    if (active_player == s.defending_player)
    {
      dispatch_event(s.defending_player, s.card, EVENT_CHECK_ABILITIES);
    }
    ai_combat_eval_table[s.defending_player][s.card].power += combat_eval_power;
    ai_combat_eval_table[s.defending_player][s.card].toughness += combat_eval_toughness;
    ai_combat_eval_table[s.defending_player][s.card].abilities |= combat_eval_abilities;
    global_card_instances[s.defending_player][s.card].state = s.saved_state;
    s.internal_card_id = global_card_instances[s.defending_player][s.card].internal_card_id;
    if (global_cards_data[s.internal_card_id].code_pointer == card_co_p_black)
    {
      defending_cop_color_mask |= 2;
    }
    if (global_cards_data[s.internal_card_id].code_pointer == card_co_p_blue)
    {
      defending_cop_color_mask |= 4;
    }
    if (global_cards_data[s.internal_card_id].code_pointer == card_co_p_green)
    {
      defending_cop_color_mask |= 8;
    }
    if (global_cards_data[s.internal_card_id].code_pointer == card_co_p_red)
    {
      defending_cop_color_mask |= 0x10;
    }
    if (global_cards_data[s.internal_card_id].code_pointer == card_co_p_white)
    {
      defending_cop_color_mask |= 0x20;
    }
  }
  if (defending_cop_color_mask != 0)
  {
    defending_cop_available_mana = has_mana(s.defending_player, COLOR_ANY, 0);
  }
  else
  {
    defending_cop_available_mana = 0;
  }
  reassess_all_cards_and_mana();
  g_duel_ai_mode_state = s.saved_ai_mode;
  DAT_0093f4b0 = s.saved_combat_eval_flag;
}

// FUNCTION: MAGIC 0x00516ecf
// FUNCTION: SHANDALAR 0x00425a82
int score_ai_attacker_candidate(int player, int card)
{
  struct
  {
    int adjusted_toughness_plus_3;
    int adjusted_power_plus_5;
    int adjusted_power_plus_2;
    int adjusted_toughness;
    int internal_card_id;
    int adjusted_power;
    int adjusted_toughness_plus_2;
    int score;
  } s;

  s.internal_card_id = global_card_instances[player][card].internal_card_id;
  s.adjusted_power = C_get_abilities(player, card, EVENT_POWER, -1);
  s.adjusted_toughness = C_get_abilities(player, card, EVENT_TOUGHNESS, -1);
  if (s.adjusted_power != 0)
  {
    s.adjusted_power_plus_5 = s.adjusted_power + 5;
  }
  else
  {
    s.adjusted_power_plus_5 = s.adjusted_power;
  }
  s.adjusted_power_plus_2 = s.adjusted_power_plus_5 + 2;
  if (s.adjusted_toughness != 0)
  {
    s.adjusted_toughness_plus_3 = s.adjusted_toughness + 3;
  }
  else
  {
    s.adjusted_toughness_plus_3 = s.adjusted_toughness;
  }
  s.adjusted_toughness_plus_2 = s.adjusted_toughness_plus_3 + 2;
  s.score = (global_cards_data[s.internal_card_id].ai_base_value + s.adjusted_power_plus_2) * s.adjusted_toughness_plus_2 * 5;
  if ((global_cards_data[s.internal_card_id].static_ability & 0x1f) != 0)
  {
    s.score = (s.score * 3) / 2;
  }
  if ((global_cards_data[s.internal_card_id].static_ability & KEYWORD_REGENERATION) != 0)
  {
    s.score = (s.score * 3) / 2;
  }
  if ((global_cards_data[s.internal_card_id].extra_ability & 3) != 0)
  {
    s.score /= 2;
  }
  if (global_cards_data[s.internal_card_id].code_pointer != card_dummy)
  {
    s.score = (s.score * 3) / 2;
  }
  if ((global_cards_data[s.internal_card_id].static_ability & 0x1c0) != 0)
  {
    s.score = (s.score * 3) / 2;
  }
  if ((global_cards_data[s.internal_card_id].extra_ability & 8) != 0)
  {
    s.score *= 3;
  }
  if ((global_cards_data[s.internal_card_id].extra_ability & 0x10) != 0)
  {
    s.score *= 3;
  }
  if ((global_card_instances[player][card].state & STATE_UNKNOWN8000) != 0)
  {
    s.score <<= 1;
  }
  return s.score;
}

// FUNCTION: MAGIC 0x00510a3f
// FUNCTION: SHANDALAR 0x0041f5e3
unsigned int choose_attackers_ai(int player)
{
  struct
  {
    int pad_518;
    unsigned int source_mask;
    unsigned int temp_mask;
    int sort_temp;
    int highest_power;
    int highest_power_index;
    int remaining_power;
    int untapped_blocker_count;
    int candidate_cards[16];
    unsigned int block_result_mask;
    int blocker_power_value;
    int blocker_toughness_value;
    int band_anchor_card;
    int blocker_power[32];
    int blocker_toughness[16];
    char blocker_index_by_card[152];
    unsigned int saved_attacker_abilities[16];
    int best_blocker_card;
    unsigned int band_regeneration_ability;
    unsigned int subset_mask;
    int subset_legal;
    int inner;
    unsigned int landwalk_mask;
    int card_id;
    int expected_damage;
    int card;
    unsigned int selected_mask;
    int saved_event_state;
    int band_candidate_power;
    int saved_attacker_scores[16];
    int best_score;
    int candidate_count;
    int pressure_score;
    int saved_ai_mode;
    int band_candidate_count;
    int band_candidate_cards[16];
    int internal_card_id;
    unsigned int attacker_abilities;
    int own_toughness[14];
    int ranked_blockers[18];
    int own_creature_index;
    int candidate_powers[16];
    int test_toughness;
    char own_index_by_card[152];
    int highest_blocker_toughness;
    int candidate_scores[16];
    int test_power;
    int smallest_blocker_score;
    int assassin_count;
    int ranked_index;
    int cost_to_chump;
    int blocker_creature_index;
    unsigned int forced_attack_mask;
    int saved_attacker_damage[16];
    int saved_attacker_toughness[16];
    unsigned int band_common_abilities;
  } s;

  setup_ai_combat_abilities(player);
  s.saved_event_state = unk_00712938;
  unk_00712938 = 2;
  s.saved_ai_mode = g_duel_ai_mode_state;
  if (g_duel_ai_mode_state == 1)
  {
    ai_search_flags ^= 2;
  }

  for (s.card = 0; s.card < active_cards_count[player]; s.card++)
  {
    AI_CARD_STATE(player, s.card) &= ~STATE_ATTACKING;
  }
  C_dispatch_event_raw(0x15);

  s.band_regeneration_ability = 0;
  s.band_candidate_power = s.band_regeneration_ability;
  s.band_candidate_count = s.band_candidate_power;
  s.forced_attack_mask = s.band_candidate_count;
  s.candidate_count = s.forced_attack_mask;
  s.band_common_abilities = 0xffffffff;
  memset(s.candidate_cards, 0, sizeof(s.candidate_cards));
  for (s.card = 0; s.card < active_cards_count[player]; s.card++)
  {
    s.internal_card_id = AI_CARD_INTERNAL_ID(player, s.card);
    if ((s.internal_card_id != -1) &&
        (((global_cards_data[s.internal_card_id].type & TYPE_CREATURE) != 0 ||
          ((AI_CARD_STATE(player, s.card) & STATE_NONCREATURE_CAN_ATTACK) != 0)) &&
         ((AI_CARD_STATE(player, s.card) & 0x820012) == STATE_IN_PLAY)) &&
        (can_attack(player, s.card) != 0))
    {
      if ((AI_CARD_STATE(player, s.card) & STATE_UNKNOWN8000) != 0)
      {
        s.forced_attack_mask |= 1 << (unsigned char)s.candidate_count;
      }
      if (((ai_search_flags & 2) != 0) &&
          ((AI_CARD_STATE(player, s.card) & STATE_UNKNOWN8000) == 0) &&
          ((AI_EVAL(player, s.card).abilities & KEYWORD_BANDING) != 0))
      {
        s.band_candidate_power += AI_EVAL(player, s.card).power;
        s.band_common_abilities &= AI_EVAL(player, s.card).abilities;
        s.band_regeneration_ability |= AI_EVAL(player, s.card).abilities & KEYWORD_REGENERATION;
        s.band_candidate_cards[s.band_candidate_count] = s.card;
        s.band_candidate_count++;
      }
      else
      {
        s.candidate_scores[s.candidate_count] = score_ai_attacker_candidate(player, s.card);
        s.candidate_cards[s.candidate_count] = s.card;
        s.candidate_count++;
        AI_CARD_STATE(player, s.card) |= STATE_ATTACKING;
      }
    }
  }
  s.band_common_abilities |= s.band_regeneration_ability;

  if (s.candidate_count > 7)
  {
    s.untapped_blocker_count = 0;
    for (s.card = 0; s.card < active_cards_count[ai_blocker_player]; s.card++)
    {
      s.internal_card_id = AI_CARD_INTERNAL_ID(ai_blocker_player, s.card);
      if ((is_in_play(ai_blocker_player, s.card) == 0) ||
          ((global_cards_data[s.internal_card_id].type & TYPE_CREATURE) == 0) ||
          ((AI_CARD_STATE(ai_blocker_player, s.card) & STATE_TAPPED) != 0))
      {
        continue;
      }
      s.untapped_blocker_count++;
    }
    for (s.card = 0; s.card < 16; s.card++)
    {
      s.candidate_powers[s.card] = 0;
    }
    for (s.card = 0; s.card < s.candidate_count; s.card++)
    {
      s.candidate_powers[s.card] = AI_EVAL(player, s.candidate_cards[s.card]).power;
    }
    while (s.untapped_blocker_count-- != 0)
    {
      s.highest_power = 0;
      s.highest_power_index = 0;
      for (s.card = 0; s.card < s.candidate_count; s.card++)
      {
        if (s.highest_power < s.candidate_powers[s.card])
        {
          s.highest_power = s.candidate_powers[s.card];
          s.highest_power_index = s.card;
        }
      }
      s.candidate_powers[s.highest_power_index] = 0;
    }
    s.remaining_power = 0;
    for (s.card = 0; s.card < s.candidate_count; s.card++)
    {
      s.remaining_power += s.candidate_powers[s.card];
    }
    if (life[1 - player] <= s.remaining_power)
    {
      for (s.card = 0; s.card < s.candidate_count; s.card++)
      {
        AI_CARD_STATE(player, s.candidate_cards[s.card]) |= STATE_ATTACKING;
      }
      attacking_creature_count = (1 << (unsigned char)s.candidate_count) - 1;
      return attacking_creature_count;
    }
  }

  if (s.candidate_count > 7)
  {
    for (s.card = 0; s.card < s.candidate_count; s.card++)
    {
      for (s.inner = s.card; s.inner < s.candidate_count; s.inner++)
      {
        if (s.candidate_scores[s.inner] > s.candidate_scores[s.card])
        {
          s.sort_temp = s.candidate_scores[s.card];
          s.candidate_scores[s.card] = s.candidate_scores[s.inner];
          s.candidate_scores[s.inner] = s.sort_temp;
          s.sort_temp = s.candidate_cards[s.card];
          s.candidate_cards[s.card] = s.candidate_cards[s.inner];
          s.candidate_cards[s.inner] = s.sort_temp;
          s.temp_mask = (1 << (unsigned char)s.card) & s.forced_attack_mask;
          s.forced_attack_mask &= ~(1 << (unsigned char)s.card);
          s.source_mask = (1 << (unsigned char)s.inner) & s.forced_attack_mask;
          s.forced_attack_mask &= ~(1 << (unsigned char)s.inner);
          if (s.temp_mask != 0)
          {
            s.forced_attack_mask |= 1 << (unsigned char)s.inner;
          }
          if (s.source_mask != 0)
          {
            s.forced_attack_mask |= 1 << (unsigned char)s.card;
          }
        }
      }
    }
    s.candidate_count = s.candidate_count < 7 ? s.candidate_count : 7;
    for (s.card = 7; s.card < 16; s.card++)
    {
      AI_CARD_STATE(player, s.candidate_cards[s.card]) &= ~STATE_ATTACKING;
    }
    memset(&s.candidate_cards[7], 0, 36);
    s.forced_attack_mask = 0;
    s.candidate_count = s.forced_attack_mask;
    for (s.card = 0; s.card < active_cards_count[player]; s.card++)
    {
      if ((AI_CARD_STATE(player, s.card) & STATE_ATTACKING) != 0)
      {
        if ((AI_CARD_STATE(player, s.card) & STATE_UNKNOWN8000) != 0)
        {
          s.forced_attack_mask |= 1 << (unsigned char)s.candidate_count;
        }
        s.candidate_cards[s.candidate_count] = s.card;
        s.candidate_count++;
      }
    }
  }

  setup_combat_damage_simulation(player);
  memcpy(s.saved_attacker_damage, combat_damage_attacker_damage, 0x40);
  memcpy(s.saved_attacker_toughness, combat_damage_attacker_toughness, 0x40);
  memcpy(s.saved_attacker_abilities, combat_damage_attacker_abilities, 0x40);
  memcpy(s.saved_attacker_scores, DAT_00708658, 0x40);
  memcpy(s.blocker_toughness, DAT_00707ef0, 0x40);
  memcpy(DAT_00707f38, DAT_00708128, 0x40);
  memcpy(DAT_00707d98, DAT_007080c8, 0x40);

  save_combat_simulation_state();
  g_duel_ai_mode_state = 1;
  C_dispatch_event_raw(199);
  g_duel_ai_mode_state = s.saved_ai_mode;

  for (s.card = 0; s.card <= 7; s.card++)
  {
    special_mana_pool[ai_blocker_player][s.card] = basiclandtypes_controlled[ai_blocker_player][s.card];
  }

  s.highest_blocker_toughness = 0;
  s.blocker_creature_index = s.highest_blocker_toughness;
  for (s.card = 0; s.card < active_cards_count[ai_blocker_player]; s.card++)
  {
    s.internal_card_id = AI_CARD_INTERNAL_ID(ai_blocker_player, s.card);
    if ((is_in_play(ai_blocker_player, s.card) == 0) ||
        ((global_cards_data[s.internal_card_id].type & TYPE_CREATURE) == 0))
    {
    }
    else
    {
      s.blocker_power[s.blocker_creature_index] = AI_EVAL(ai_blocker_player, s.card).power;
      s.blocker_power[s.blocker_creature_index + 16] = AI_EVAL(ai_blocker_player, s.card).toughness;
      if ((global_cards_data[s.internal_card_id].extra_ability & 8) != 0)
      {
        s.blocker_power[s.blocker_creature_index] +=
            (*global_cards_data[s.internal_card_id].code_pointer)(ai_blocker_player, s.card, EVENT_POW_BOOST);
      }
      if ((global_cards_data[s.internal_card_id].extra_ability & 0x10) != 0)
      {
        s.blocker_power[s.blocker_creature_index + 16] +=
            (*global_cards_data[s.internal_card_id].code_pointer)(ai_blocker_player, s.card, EVENT_TOU_BOOST);
      }
      if (s.highest_blocker_toughness < s.blocker_power[s.blocker_creature_index + 16])
      {
        s.highest_blocker_toughness = s.blocker_power[s.blocker_creature_index + 16];
      }
      s.blocker_index_by_card[s.card] = (char)s.blocker_creature_index;
      s.blocker_creature_index++;
    }
  }

  s.band_anchor_card = -1;
  for (s.card = 0; s.candidate_count > s.card; s.card++)
  {
    if ((s.band_anchor_card == -1) &&
        (s.saved_attacker_damage[s.card] < s.highest_blocker_toughness) &&
        (s.highest_blocker_toughness <= s.saved_attacker_damage[s.card] + s.band_candidate_power) &&
        ((s.saved_attacker_abilities[s.card] & s.band_common_abilities) == s.saved_attacker_abilities[s.card]))
    {
      s.saved_attacker_damage[s.card] += s.band_candidate_power;
      s.band_anchor_card = s.candidate_cards[s.card];
    }
  }

  s.own_creature_index = 0;
  for (s.inner = 0; s.inner < active_cards_count[player]; s.inner++)
  {
    if ((is_in_play(player, s.inner) == 0) ||
        ((global_cards_data[AI_CARD_INTERNAL_ID(player, s.inner)].type & TYPE_CREATURE) == 0))
    {
    }
    else
    {
      s.candidate_powers[s.own_creature_index] = AI_EVAL(player, s.inner).power;
      s.own_toughness[s.own_creature_index] = AI_EVAL(player, s.inner).toughness;
      s.candidate_scores[s.own_creature_index] =
          (s.candidate_powers[s.own_creature_index] + 1) * (s.own_toughness[s.own_creature_index] + 1);
      for (s.subset_mask = 0; (int)s.subset_mask < combat_damage_attacker_count; s.subset_mask++)
      {
        if (combat_damage_attacker_cards[s.subset_mask] == s.inner)
        {
          s.candidate_scores[s.own_creature_index] = DAT_00708658[s.subset_mask];
        }
      }
      s.own_index_by_card[s.inner] = (char)s.own_creature_index;
      s.own_creature_index++;
    }
  }

  restore_combat_simulation_state();
  s.best_score = 9999;
  get_landwalk_evasion_masks(&s.landwalk_mask, NULL);

  for (s.subset_mask = 0; (1 << (unsigned char)s.candidate_count) > (int)s.subset_mask; s.subset_mask++)
  {
    s.subset_legal = 1;
    combat_damage_attacker_count = 0;
    _DAT_00707e20 = 0;
    for (s.card = 0; s.card < s.candidate_count; s.card++)
    {
      AI_CARD_STATE(player, s.candidate_cards[s.card]) &= ~STATE_ATTACKING;
      s.card_id = global_cards_data[AI_CARD_INTERNAL_ID(player, s.candidate_cards[s.card])].id;
      if ((s.subset_mask & (1 << (unsigned char)s.card)) != 0)
      {
        AI_CARD_STATE(player, s.candidate_cards[s.card]) |= STATE_ATTACKING;
        combat_damage_attacker_cards[combat_damage_attacker_count] = s.candidate_cards[s.card];
        combat_damage_attacker_damage[combat_damage_attacker_count] = s.saved_attacker_damage[s.card];
        combat_damage_attacker_toughness[combat_damage_attacker_count] = s.saved_attacker_toughness[s.card];
        combat_damage_attacker_abilities[combat_damage_attacker_count] = s.saved_attacker_abilities[s.card];
        DAT_00708658[combat_damage_attacker_count] = s.saved_attacker_scores[s.card];
        DAT_00707ef0[combat_damage_attacker_count] = s.blocker_toughness[s.card];
        DAT_00708128[combat_damage_attacker_count] = DAT_00707f38[s.card];
        DAT_007080c8[combat_damage_attacker_count] = DAT_00707d98[s.card];
        if ((s.card_id == 0x28) || (s.card_id == 0x98))
        {
          _DAT_00707e20 |= 1 << (unsigned char)combat_damage_attacker_count;
        }
        combat_damage_attacker_count++;
      }
      else
      {
        if ((s.card_id == 0x19f) || (s.card_id == 0x84))
        {
          s.subset_legal = 0;
        }
        if ((s.forced_attack_mask & (1 << (unsigned char)s.card)) != 0)
        {
          s.subset_legal = 0;
        }
      }
    }

    if (s.subset_legal == 0)
    {
      continue;
    }

    if (untapped_royal_assassin_count != 0)
    {
      s.assassin_count = 0;
      for (s.card = 0; s.card < combat_damage_attacker_count; s.card++, s.assassin_count++)
      {
        if ((AI_CARD_STATE(player, combat_damage_attacker_cards[s.card]) & STATE_VIGILANCE) == 0)
        {
          DAT_00708088[s.card] = s.card;
        }
        else
        {
          DAT_00708088[s.card] = 99;
          s.assassin_count--;
        }
      }
      for (s.card = 0; s.card < combat_damage_attacker_count; s.card++)
      {
        for (s.inner = s.card; s.inner < combat_damage_attacker_count; s.inner++)
        {
          if ((DAT_00708088[s.card] != 99) &&
              (combat_damage_attacker_damage[DAT_00708088[s.card]] <
               combat_damage_attacker_damage[DAT_00708088[s.inner]]))
          {
            s.pad_518 = DAT_00708088[s.inner];
            DAT_00708088[s.inner] = DAT_00708088[s.card];
            DAT_00708088[s.card] = s.pad_518;
          }
        }
      }
      for (s.card = 0; s.card < combat_damage_attacker_count; s.card++)
      {
        if (DAT_00708088[s.card] == 99)
        {
          DAT_00707ea8[s.card] = 99;
        }
        else
        {
          DAT_00707ea8[DAT_00708088[s.card]] = s.card;
        }
      }
    }

    DAT_007081a8 = 1;
    choose_blockers_ai(player);
    save_combat_simulation_state();
    g_duel_ai_mode_state = 1;
    C_dispatch_event_raw(199);
    g_duel_ai_mode_state = s.saved_ai_mode;

    s.expected_damage = 0;
    for (s.ranked_index = 0; s.ranked_index < 8; s.ranked_index++)
    {
      s.ranked_blockers[s.ranked_index * 2 + 3] = -1;
    }

    for (s.card = 0; s.card < active_cards_count[ai_blocker_player]; s.card++)
    {
      s.internal_card_id = AI_CARD_INTERNAL_ID(ai_blocker_player, s.card);
      if ((is_in_play(ai_blocker_player, s.card) == 0) ||
          ((global_cards_data[s.internal_card_id].type & TYPE_CREATURE) == 0) ||
          ((AI_CARD_STATE(ai_blocker_player, s.card) & STATE_PLAYED_FROM_HAND) == 0))
      {
      }
      else
      {
        s.blocker_creature_index = (int)s.blocker_index_by_card[s.card];
        s.blocker_power_value = s.blocker_power[s.blocker_creature_index];
        if ((AI_EVAL(ai_blocker_player, s.card).abilities & 0x80000000) != 0)
        {
          s.blocker_power_value += 2;
        }
        for (s.inner = 0; s.inner < 8; s.inner++)
        {
          if (s.blocker_power_value > s.ranked_blockers[s.inner * 2 + 3])
          {
            for (s.ranked_index = 7; s.inner < s.ranked_index; s.ranked_index--)
            {
              s.ranked_blockers[s.ranked_index * 2 + 2] = s.ranked_blockers[s.ranked_index * 2];
              s.ranked_blockers[s.ranked_index * 2 + 3] = s.ranked_blockers[s.ranked_index * 2 + 1];
            }
            s.ranked_blockers[s.inner * 2 + 2] = s.card;
            s.ranked_blockers[s.inner * 2 + 3] = s.blocker_power_value;
            break;
          }
        }
      }
    }

    for (s.ranked_index = 0; s.ranked_index < 8; s.ranked_index++)
    {
      if (s.ranked_blockers[s.ranked_index * 2 + 3] == -1)
      {
        break;
      }
      s.card = s.ranked_blockers[s.ranked_index * 2 + 2];
      s.attacker_abilities = C_get_abilities(ai_blocker_player, s.card, EVENT_ABILITIES, -1);
      s.blocker_creature_index = (int)s.blocker_index_by_card[s.card];
      s.blocker_power_value = s.blocker_power[s.blocker_creature_index];
      if ((AI_EVAL(ai_blocker_player, s.card).abilities & 0x80000000) != 0)
      {
        s.blocker_power_value += 2;
      }
      s.blocker_toughness_value = s.blocker_power[s.blocker_creature_index + 16];
      s.best_blocker_card = 0;
      s.block_result_mask = s.best_blocker_card;
      s.own_creature_index = 0;
      s.smallest_blocker_score = 0x7fff;

      for (s.inner = 0; s.inner < active_cards_count[player]; s.inner++)
      {
        if ((is_in_play(player, s.inner) == 0) ||
            ((AI_CARD_STATE(player, s.inner) & STATE_PLAYED_FROM_HAND) == 0) ||
            ((global_cards_data[AI_CARD_INTERNAL_ID(player, s.inner)].type & TYPE_CREATURE) == 0))
        {
        }
        else
        {
          s.own_creature_index = (int)s.own_index_by_card[s.inner];
          s.test_power = s.candidate_powers[s.own_creature_index];
          s.test_toughness = s.own_toughness[s.own_creature_index];
          if (((AI_CARD_STATE(player, s.inner) &
                ((has_vigilance(player, s.inner) == 0 ? STATE_ATTACKING : 0) + STATE_BLOCKING)) != 0) ||
              (can_block_attacker_with_abilities(player, s.inner, ai_blocker_player, s.card,
                                                 s.attacker_abilities, s.landwalk_mask) == 0))
          {
          }
          else
          {
            s.block_result_mask |= 1;
            if ((s.blocker_power_value < s.test_toughness) || (s.blocker_toughness_value <= s.test_power))
            {
              s.block_result_mask |= 2;
              AI_CARD_STATE(player, s.inner) |= STATE_BLOCKING;
              break;
            }
            if (s.candidate_scores[s.own_creature_index] < s.smallest_blocker_score)
            {
              s.smallest_blocker_score = s.candidate_scores[s.own_creature_index];
              s.best_blocker_card = s.inner;
            }
          }
        }
      }

      if ((s.block_result_mask & 2) == 0)
      {
        s.pressure_score = (ai_combat_value_weights[player] * s.blocker_power_value * 0x18) / 4;
        s.test_toughness = life[player] - s.blocker_power_value;
        if (s.test_toughness < 2)
        {
          s.test_toughness = 1;
        }
        s.test_toughness = ClampIntToRange(s.test_toughness, 1, 99);
        s.pressure_score /= s.test_toughness;
        if (s.block_result_mask != 0)
        {
          s.cost_to_chump = (ai_combat_value_weights[player + 2] * s.smallest_blocker_score) / 8;
          if (s.cost_to_chump <= s.pressure_score)
          {
            DAT_00708698 += s.cost_to_chump;
            AI_CARD_STATE(player, s.best_blocker_card) |= STATE_BLOCKING;
            continue;
          }
        }
        s.expected_damage += s.blocker_power_value;
        DAT_00708698 += s.pressure_score;
      }
    }

    restore_combat_simulation_state();
    if ((life[player] <= s.expected_damage) && (0 < life[player]) && (0 < life[1 - player]))
    {
      DAT_00708698 += ((s.expected_damage - life[player]) + 2) * 0x80;
      DAT_00708698 += DAT_00707ff8;
    }
    if (s.best_score > DAT_00708698)
    {
      s.best_score = DAT_00708698;
      s.selected_mask = s.subset_mask;
    }
  }

  if ((s.selected_mask != 0) && ((battlefield_extra_ability_flags & 0x2000000) != 0))
  {
    C_dispatch_event_raw(0x92);
  }

  for (s.card = 0; s.candidate_count > s.card; s.card++)
  {
    AI_CARD_STATE(player, s.candidate_cards[s.card]) &= ~STATE_ATTACKING;
    if ((s.selected_mask & (1 << (unsigned char)s.card)) != 0)
    {
      AI_CARD_STATE(player, s.candidate_cards[s.card]) |= STATE_ATTACKING;
      if ((battlefield_extra_ability_flags & 0x80000) != 0)
      {
        trigger_cause_controller = player;
        trigger_cause = s.candidate_cards[s.card];
        dispatch_trigger_twice_once_with_each_player_as_reason(current_player, TRIGGER_ATTACKER_CHOSEN,
                                                               gs_attacker_selected_008b32d0, 0);
      }
      if ((s.band_candidate_count != 0) &&
          (s.band_anchor_card == -1) &&
          ((s.saved_attacker_abilities[s.card] & s.band_common_abilities) == s.saved_attacker_abilities[s.card]))
      {
        s.band_anchor_card = s.candidate_cards[s.card];
      }
    }
  }

  if ((s.band_candidate_count != 0) && (s.band_anchor_card != -1) &&
      ((AI_CARD_STATE(player, s.band_anchor_card) & STATE_ATTACKING) != 0))
  {
    for (s.card = 0; s.band_candidate_count > s.card; s.card++)
    {
      AI_CARD_BLOCKING(player, s.band_candidate_cards[s.card]) = (unsigned char)s.band_anchor_card;
      AI_CARD_STATE(player, s.band_candidate_cards[s.card]) |= STATE_ATTACKING;
      if ((battlefield_extra_ability_flags & 0x80000) != 0)
      {
        trigger_cause_controller = player;
        trigger_cause = s.band_candidate_cards[s.card];
        dispatch_trigger_twice_once_with_each_player_as_reason(current_player, TRIGGER_ATTACKER_CHOSEN,
                                                               gs_attacker_selected_008b32d0, 0);
      }
    }
    AI_CARD_BLOCKING(player, s.band_anchor_card) = (unsigned char)s.band_anchor_card;
  }
  else if ((s.band_candidate_count != 0) && (s.highest_blocker_toughness == 0))
  {
    for (s.card = 0; s.band_candidate_count > s.card; s.card++)
    {
      AI_CARD_STATE(player, s.band_candidate_cards[s.card]) |= STATE_ATTACKING;
      if ((battlefield_extra_ability_flags & 0x80000) != 0)
      {
        trigger_cause_controller = player;
        trigger_cause = s.band_candidate_cards[s.card];
        dispatch_trigger_twice_once_with_each_player_as_reason(current_player, TRIGGER_ATTACKER_CHOSEN,
                                                               gs_attacker_selected_008b32d0, 0);
      }
    }
    s.selected_mask = 1;
  }

  attacking_creature_count = s.selected_mask;
  unk_00712938 = s.saved_event_state;
  return s.selected_mask;
}

// FUNCTION: MAGIC 0x0050e6f0
// FUNCTION: SHANDALAR 0x0041d290
int score_gamestate_after_simulated_card_death(int player, int card)
{
  int saved_ai_modifier;
  int score;

  save_combat_simulation_state();
  saved_ai_modifier = ai_modifier;
  ai_modifier = 0;
  AI_CARD(player, card).token_status |= 8;
  kill_card(player, card, KILL_DESTROY);
  C_dispatch_event_raw(199);
  process_damage_prevention(player);
  score = ai_modifier + ai_opinion_of_gamestate(player);
  restore_combat_simulation_state();
  ai_modifier = saved_ai_modifier;
  return score;
}

// FUNCTION: MAGIC 0x00516bf1
// FUNCTION: SHANDALAR 0x004257a4
void dispatch_ai_combat_pump_checks(int player)
{
  int card;

  for (card = 0; card < 4; card++)
  {
    unk_007a7d80[card] = 0;
  }

  for (card = 0; MAX(active_cards_count[active_player], active_cards_count[other_player]) > card; card++)
  {
    if (is_in_play(active_player, card) != 0)
    {
      (*global_cards_data[AI_CARD_INTERNAL_ID(active_player, card)].code_pointer)(active_player, card, EVENT_CHECK_PUMP);
    }

    if ((is_in_play(other_player, card) != 0) ||
        ((AI_CARD_INTERNAL_ID(other_player, card) != -1) &&
         ((global_cards_data[AI_CARD_INTERNAL_ID(other_player, card)].type & 0x10) != 0)))
    {
      (*global_cards_data[AI_CARD_INTERNAL_ID(other_player, card)].code_pointer)(other_player, card, EVENT_CHECK_PUMP);
    }
  }
}

// FUNCTION: MAGIC 0x005170d1
// FUNCTION: SHANDALAR 0x00425c7e
int score_ai_blocker_candidate(int player, int card)
{
  struct
  {
    int adjusted_toughness;
    int adjusted_power;
    int power_plus_two;
    int toughness;
    int internal_card_id;
    int power;
    int toughness_plus_two;
    int score;
  } s;

  s.internal_card_id = global_card_instances[player][card].internal_card_id;
  s.power = C_get_abilities(player, card, EVENT_POWER, -1);
  s.toughness = C_get_abilities(player, card, EVENT_TOUGHNESS, -1);
  if (s.power != 0)
  {
    s.adjusted_power = s.power + 5;
  }
  else
  {
    s.adjusted_power = s.power;
  }
  s.power_plus_two = s.adjusted_power + 2;
  if (s.toughness != 0)
  {
    s.adjusted_toughness = s.toughness + 5;
  }
  else
  {
    s.adjusted_toughness = s.toughness;
  }
  s.toughness_plus_two = s.adjusted_toughness + 2;
  s.score = ((global_cards_data[s.internal_card_id].ai_base_value + s.power_plus_two) * s.toughness_plus_two) * 5;
  if ((global_cards_data[s.internal_card_id].static_ability & 0x200) != 0)
  {
    s.score = (s.score * 3) / 2;
  }
  if ((global_cards_data[s.internal_card_id].extra_ability & (EA_ACT_ABILITY | EA_ACT_INTERRUPT)) != 0)
  {
    s.score /= 2;
  }
  if ((global_cards_data[s.internal_card_id].static_ability & 0x20) != 0)
  {
    s.score = (s.score * 3) / 2;
  }
  if ((global_cards_data[s.internal_card_id].extra_ability & EA_INF_POWER) != 0)
  {
    s.score *= 3;
  }
  if ((global_cards_data[s.internal_card_id].extra_ability & EA_INF_TOUGHNESS) != 0)
  {
    s.score *= 3;
  }
  return s.score;
}

// FUNCTION: MAGIC 0x00446281
// FUNCTION: SHANDALAR 0x00411b19
unsigned int get_destroy_if_blocked_combat_flags(int attacker_player, int attacker_card, int blocker_player, int blocker_card)
{
  unsigned int flags;

  flags = 0;
  if ((AI_CARD(attacker_player, attacker_card).destroys_if_blocked == 0) &&
      (AI_CARD(blocker_player, blocker_card).destroys_if_blocked == 0))
  {
    return 0;
  }

  if ((AI_CARD(attacker_player, attacker_card).destroys_if_blocked &
       AI_CARD(blocker_player, blocker_card).color & 0x3f) != 0)
  {
    flags |= 1;
    if ((global_cards_data[AI_CARD_INTERNAL_ID(blocker_player, blocker_card)].subtype == 0) &&
        ((AI_CARD(attacker_player, attacker_card).destroys_if_blocked & 0x80) == 0))
    {
      flags &= ~1;
    }
  }

  if ((AI_CARD(attacker_player, attacker_card).color &
       AI_CARD(blocker_player, blocker_card).destroys_if_blocked & 0x3f) != 0)
  {
    flags |= 2;
    if ((global_cards_data[AI_CARD_INTERNAL_ID(attacker_player, attacker_card)].subtype == 0) &&
        ((AI_CARD(blocker_player, blocker_card).destroys_if_blocked & 0x80) == 0))
    {
      flags &= ~2;
    }
  }
  return flags;
}

// FUNCTION: MAGIC 0x0050f53a
// FUNCTION: SHANDALAR 0x0041e0d9
void setup_combat_damage_simulation(int player)
{
  int unused_padding_1;
  int unused_padding_2;
  unsigned int block_interaction_flags;
  struct
  {
    int blocker_scores[16];
    unsigned int regen_ability_bits;
    int baseline_score;
    int blocker_candidate_index;
    int card_color;
    int band_scan_index;
    int power;
    int attacker_index;
    int card;
    int saved_ai_mode;
    int internal_card_id;
    int saved_ai_modifier;
    int blocker_candidate_count;
    int band_anchor_index;
    int blocker_candidates[16];
    unsigned int player_0_landwalk_mask;
    int combat_score;
    int cached_score;
    unsigned int player_1_landwalk_mask;
  } s;

  ai_blocker_player = 1 - player;
  if (DAT_007081a8 == 0)
  {
    get_landwalk_evasion_masks(&s.player_1_landwalk_mask, &s.player_0_landwalk_mask);
    if (ai_blocker_player == 1)
    {
      DAT_00708724 = s.player_1_landwalk_mask;
    }
    else
    {
      DAT_00708724 = s.player_0_landwalk_mask;
    }

    DAT_00707ffc = 0;
    _DAT_00707e20 = DAT_00707ffc;
    DAT_00708720 = 0;
    combat_damage_attacker_count = DAT_00708720;
    s.saved_ai_mode = g_duel_ai_mode_state;
    g_duel_ai_mode_state = 1;
    DAT_0093f4b0 = 1;
    save_combat_simulation_state();
    s.saved_ai_modifier = ai_modifier;
    ai_modifier = 0;
    C_dispatch_event_raw(199);
    process_damage_prevention(player);
    s.baseline_score = ai_opinion_of_gamestate(player);
    s.baseline_score += ai_modifier;
    for (s.card = 0; s.card < active_cards_count[player]; s.card++)
    {
      AI_CARD_STATE(player, s.card) &= ~STATE_ATTACKING;
    }
    dispatch_ai_combat_pump_checks(player);
    restore_combat_simulation_state();
    ai_modifier = s.saved_ai_modifier;

    for (s.card = 0; s.card < active_cards_count[player]; s.card++)
    {
      s.internal_card_id = AI_CARD_INTERNAL_ID(player, s.card);
      if (((s.internal_card_id != -1) &&
           ((AI_CARD_STATE(player, s.card) & STATE_ATTACKING) != 0)) &&
          ((AI_CARD_BLOCKING(player, s.card) == -1) || (AI_CARD_BLOCKING(player, s.card) == s.card)))
      {
        s.card_color = single_color_test_bit_to_color_t((int)global_cards_data[s.internal_card_id].color);
        s.power = AI_EVAL(player, s.card).power;
        combat_damage_attacker_cards[combat_damage_attacker_count] = s.card;
        combat_damage_attacker_damage[combat_damage_attacker_count] = unk_007a7d80[player] + s.power;
        combat_damage_attacker_toughness[combat_damage_attacker_count] = AI_EVAL(player, s.card).toughness + unk_007a7d88[player];
        combat_damage_attacker_abilities[combat_damage_attacker_count] = AI_EVAL(player, s.card).abilities;
        if (has_mana(player, s.card_color, 1) == 0)
        {
          combat_damage_attacker_abilities[combat_damage_attacker_count] &= 0xfffffdff;
        }
        unk_00925d3c = 0;
        dispatch_event(player, s.card, 0x8a);
        DAT_00708128[combat_damage_attacker_count] = unk_00925d3c;
        if ((global_cards_data[s.internal_card_id].extra_ability & EA_INF_POWER) != 0)
        {
          combat_damage_attacker_damage[combat_damage_attacker_count] +=
              (*global_cards_data[s.internal_card_id].code_pointer)(player, s.card, EVENT_POW_BOOST);
        }
        if ((global_cards_data[s.internal_card_id].extra_ability & EA_INF_TOUGHNESS) != 0)
        {
          combat_damage_attacker_toughness[combat_damage_attacker_count] +=
              (*global_cards_data[s.internal_card_id].code_pointer)(player, s.card, EVENT_TOU_BOOST);
        }
        if (AI_EVAL(player, s.card).cached_score == 0)
        {
          AI_EVAL(player, s.card).cached_score = score_gamestate_after_simulated_card_death(player, s.card);
        }
        s.cached_score = AI_EVAL(player, s.card).cached_score;
        s.combat_score = ((abs((int)global_cards_data[s.internal_card_id].cc[1]) +
                           global_cards_data[s.internal_card_id].cc[0]) -
                          s.cached_score) +
                         s.baseline_score;
        if ((combat_damage_attacker_abilities[combat_damage_attacker_count] & 0x200) != 0)
        {
          if (has_mana(player, s.card_color, 1) == 0)
          {
            s.combat_score <<= 1;
          }
          else
          {
            s.combat_score /= 3;
          }
        }
        AI_CARD(player, s.card).attack_rating = s.combat_score;
        DAT_00708658[combat_damage_attacker_count] = s.combat_score;
        combat_damage_attacker_toughness[combat_damage_attacker_count] -= AI_CARD(player, s.card).damage_on_card;
        DAT_00707ef0[combat_damage_attacker_count] = 0;
        if (AI_CARD(player, s.card).destroys_if_blocked != 0)
        {
          _DAT_00707e20 |= 1 << (unsigned char)combat_damage_attacker_count;
        }
        combat_damage_attacker_count++;
        if ((g_duel_ai_mode_state == 1) && (combat_damage_attacker_count > 6))
        {
          break;
        }
      }
    }

    for (s.card = 0; s.card < active_cards_count[player]; s.card++)
    {
      s.internal_card_id = AI_CARD_INTERNAL_ID(player, s.card);
      if ((((s.internal_card_id != -1) &&
            ((AI_CARD_STATE(player, s.card) & STATE_ATTACKING) != 0)) &&
           (AI_CARD_BLOCKING(player, s.card) != -1)) &&
          (AI_CARD_BLOCKING(player, s.card) != s.card))
      {
        s.band_anchor_index = -1;
        for (s.attacker_index = 0; s.attacker_index < combat_damage_attacker_count; s.attacker_index++)
        {
          if (AI_CARD_BLOCKING(player, s.card) == combat_damage_attacker_cards[s.attacker_index])
          {
            s.band_anchor_index = s.attacker_index;
            break;
          }
        }

        if (s.band_anchor_index != -1)
        {
          s.card_color = single_color_test_bit_to_color_t((int)global_cards_data[s.internal_card_id].color);
          DAT_00708168[DAT_00708720] = s.card;
          s.power = AI_EVAL(player, s.card).power;
          combat_damage_attacker_damage[s.band_anchor_index] += s.power;
          combat_damage_attacker_toughness[s.band_anchor_index] += AI_EVAL(player, s.card).toughness;
          s.regen_ability_bits =
              (AI_EVAL(player, s.card).abilities & 0x200) | (combat_damage_attacker_abilities[s.band_anchor_index] & 0x200);
          combat_damage_attacker_abilities[s.band_anchor_index] &= AI_EVAL(player, s.card).abilities;
          combat_damage_attacker_abilities[s.band_anchor_index] |= s.regen_ability_bits;
          if ((global_cards_data[s.internal_card_id].extra_ability & EA_INF_POWER) != 0)
          {
            combat_damage_attacker_damage[s.band_anchor_index] +=
                (*global_cards_data[s.internal_card_id].code_pointer)(player, s.card, EVENT_POW_BOOST);
          }
          if ((global_cards_data[s.internal_card_id].extra_ability & EA_INF_TOUGHNESS) != 0)
          {
            combat_damage_attacker_toughness[s.band_anchor_index] +=
                (*global_cards_data[s.internal_card_id].code_pointer)(player, s.card, EVENT_TOU_BOOST);
          }
          if (AI_EVAL(player, s.card).cached_score == 0)
          {
            AI_EVAL(player, s.card).cached_score = score_gamestate_after_simulated_card_death(player, s.card);
          }
          s.cached_score = AI_EVAL(player, s.card).cached_score;
          s.combat_score = ((abs((int)global_cards_data[s.internal_card_id].cc[1]) +
                             global_cards_data[s.internal_card_id].cc[0]) -
                            s.cached_score) +
                           s.baseline_score;
          if ((combat_damage_attacker_abilities[s.band_anchor_index] & 0x200) != 0)
          {
            if (has_mana(player, s.card_color, 1) == 0)
            {
              s.combat_score <<= 1;
            }
            else
            {
              s.combat_score /= 3;
            }
          }
          AI_CARD(player, s.card).attack_rating = s.combat_score;
          if (s.combat_score < DAT_00708658[s.band_anchor_index])
          {
            DAT_00708658[s.band_anchor_index] = s.combat_score;
          }
          combat_damage_attacker_toughness[s.band_anchor_index] -= AI_CARD(player, s.card).damage_on_card;
          DAT_00707ef0[s.band_anchor_index] = 0;
          if (AI_CARD(player, s.card).destroys_if_blocked != 0)
          {
            _DAT_00707e20 |= 1 << (unsigned char)s.band_anchor_index;
          }
          DAT_00708720++;
        }
      }
    }

    s.blocker_candidate_count = 0;
    for (s.card = 0; s.card < active_cards_count[ai_blocker_player]; s.card++)
    {
      s.internal_card_id = AI_CARD_INTERNAL_ID(ai_blocker_player, s.card);
      if ((((s.internal_card_id != -1) &&
            (((global_cards_data[s.internal_card_id].type & TYPE_CREATURE) != 0) ||
             ((AI_CARD_STATE(ai_blocker_player, s.card) & STATE_NONCREATURE_CAN_ATTACK) != 0))) &&
           ((AI_CARD_STATE(ai_blocker_player, s.card) & (STATE_OUBLIETTED | STATE_IN_PLAY | STATE_TAPPED)) == STATE_IN_PLAY)) &&
          (AI_CARD_BLOCKING(ai_blocker_player, s.card) == -1))
      {
        s.blocker_candidates[s.blocker_candidate_count] = s.card;
        s.blocker_candidate_count++;
      }
      if ((g_duel_ai_mode_state == 1) && (s.blocker_candidate_count > 15))
      {
        break;
      }
    }

    if ((g_duel_ai_mode_state == 1) && (s.blocker_candidate_count > 6))
    {
      for (s.card = 0; s.card < s.blocker_candidate_count; s.card++)
      {
        s.blocker_scores[s.card] = score_ai_blocker_candidate(ai_blocker_player, s.blocker_candidates[s.card]);
      }
      for (s.card = 0; s.card < s.blocker_candidate_count; s.card++)
      {
        for (s.attacker_index = s.card; s.attacker_index < s.blocker_candidate_count; s.attacker_index++)
        {
          if (s.blocker_scores[s.card] < s.blocker_scores[s.attacker_index])
          {
            s.power = s.blocker_scores[s.card];
            s.blocker_scores[s.card] = s.blocker_scores[s.attacker_index];
            s.blocker_scores[s.attacker_index] = s.power;
            s.power = s.blocker_candidates[s.card];
            s.blocker_candidates[s.card] = s.blocker_candidates[s.attacker_index];
            s.blocker_candidates[s.attacker_index] = s.power;
          }
        }
      }
      if (s.blocker_candidate_count > 6)
      {
        s.blocker_candidate_count = 7;
      }
      for (s.card = 0; s.card < s.blocker_candidate_count; s.card++)
      {
        for (s.attacker_index = s.card; s.attacker_index < s.blocker_candidate_count; s.attacker_index++)
        {
          if (s.blocker_candidates[s.attacker_index] < s.blocker_candidates[s.card])
          {
            s.power = s.blocker_candidates[s.card];
            s.blocker_candidates[s.card] = s.blocker_candidates[s.attacker_index];
            s.blocker_candidates[s.attacker_index] = s.power;
          }
        }
      }
    }

    ai_blocker_count = 0;
    for (s.blocker_candidate_index = 0; s.blocker_candidate_index < s.blocker_candidate_count; s.blocker_candidate_index++)
    {
      s.card = s.blocker_candidates[s.blocker_candidate_index];
      s.internal_card_id = AI_CARD_INTERNAL_ID(ai_blocker_player, s.card);
      s.card_color = single_color_test_bit_to_color_t((int)global_cards_data[s.internal_card_id].color);
      AI_CARD_STATE(ai_blocker_player, s.card) |= STATE_BLOCKING;
      s.power = AI_EVAL(ai_blocker_player, s.card).power;
      ai_blocker_cards[ai_blocker_count] = s.card;
      combat_damage_blocker_damage[ai_blocker_count] = unk_007a7d80[ai_blocker_player] + s.power;
      combat_damage_blocker_toughness[ai_blocker_count] = AI_EVAL(ai_blocker_player, s.card).toughness + unk_007a7d88[ai_blocker_player];
      combat_damage_blocker_abilities[ai_blocker_count] = AI_EVAL(ai_blocker_player, s.card).abilities;
      if (has_mana(ai_blocker_player, s.card_color, 1) == 0)
      {
        combat_damage_blocker_abilities[ai_blocker_count] &= 0xfffffdff;
      }
      unk_00925d3c = 0;
      dispatch_event(ai_blocker_player, s.card, 0x8b);
      DAT_007080c8[ai_blocker_count] = unk_00925d3c;
      if (ai_blocker_player == active_player)
      {
        if ((global_cards_data[s.internal_card_id].extra_ability & EA_INF_POWER) != 0)
        {
          combat_damage_blocker_damage[ai_blocker_count] +=
              (*global_cards_data[s.internal_card_id].code_pointer)(ai_blocker_player, s.card, EVENT_POW_BOOST);
        }
        if ((global_cards_data[s.internal_card_id].extra_ability & EA_INF_TOUGHNESS) != 0)
        {
          combat_damage_blocker_toughness[ai_blocker_count] +=
              (*global_cards_data[s.internal_card_id].code_pointer)(ai_blocker_player, s.card, EVENT_TOU_BOOST);
        }
      }
      if (AI_EVAL(ai_blocker_player, s.card).cached_score == 0)
      {
        save_combat_simulation_state();
        s.saved_ai_modifier = ai_modifier;
        ai_modifier = 0;
        AI_CARD(ai_blocker_player, s.card).token_status |= 8;
        kill_card(ai_blocker_player, s.card, KILL_DESTROY);
        C_dispatch_event_raw(199);
        process_damage_prevention(player);
        s.cached_score = ai_opinion_of_gamestate(player);
        s.cached_score += ai_modifier;
        restore_combat_simulation_state();
        AI_EVAL(ai_blocker_player, s.card).cached_score = s.cached_score;
      }
      s.cached_score = AI_EVAL(ai_blocker_player, s.card).cached_score;
      s.combat_score = s.cached_score - s.baseline_score;
      if ((combat_damage_blocker_abilities[ai_blocker_count] & 0x200) != 0)
      {
        if (has_mana(ai_blocker_player, s.card_color, 1) == 0)
        {
          s.combat_score <<= 1;
        }
        else
        {
          s.combat_score /= 5;
        }
      }
      DAT_00708728[ai_blocker_count] = s.combat_score;
      AI_CARD(ai_blocker_player, s.card).attack_rating = s.combat_score;
      combat_damage_blocker_toughness[ai_blocker_count] -= AI_CARD(ai_blocker_player, s.card).damage_on_card;
      AI_CARD_STATE(ai_blocker_player, s.card) &= ~STATE_BLOCKING;
      for (s.attacker_index = 0; s.attacker_index < combat_damage_attacker_count; s.attacker_index++)
      {
        if (can_block_attacker_with_abilities(ai_blocker_player, s.card, player, combat_damage_attacker_cards[s.attacker_index],
                                              combat_damage_attacker_abilities[s.attacker_index], DAT_00708724) == 0)
        {
          if (DAT_00708720 != 0)
          {
            for (s.band_scan_index = 0; s.band_scan_index < DAT_00708720; s.band_scan_index++)
            {
              if ((AI_CARD_BLOCKING(player, DAT_00708168[s.band_scan_index]) == combat_damage_attacker_cards[s.attacker_index]) &&
                  (can_block_attacker_with_abilities(ai_blocker_player, s.card, player, DAT_00708168[s.band_scan_index],
                                                     combat_damage_attacker_abilities[s.attacker_index], DAT_00708724) != 0))
              {
                DAT_00707ef0[s.attacker_index] |= 1 << (unsigned char)ai_blocker_count;
              }
            }
          }
        }
        else
        {
          DAT_00707ef0[s.attacker_index] |= 1 << (unsigned char)ai_blocker_count;
        }
      }
      if (AI_CARD(ai_blocker_player, s.card).destroys_if_blocked != 0)
      {
        DAT_00707ffc |= 1 << (unsigned char)ai_blocker_count;
      }
      AI_CARD_STATE(ai_blocker_player, s.card) &= ~STATE_BLOCKING;
      DAT_00707c98[ai_blocker_count] =
          (ai_combat_value_weights[ai_blocker_player + 2] * DAT_00708728[ai_blocker_count]) /
          ClampIntToRange(combat_damage_blocker_toughness[ai_blocker_count] + 1, 1, 99);
      ai_blocker_count++;
      if ((g_duel_ai_mode_state == 1) && (7 <= ai_blocker_count))
      {
        break;
      }
    }

    memset(DAT_00708108, 0, 0x1c);
    memset(DAT_007081b0, 0, 0x1c);
    for (s.card = 0; s.card < combat_damage_attacker_count; s.card++)
    {
      for (s.attacker_index = 0; s.attacker_index < ai_blocker_count; s.attacker_index++)
      {
        block_interaction_flags = get_destroy_if_blocked_combat_flags(player, combat_damage_attacker_cards[s.card], ai_blocker_player,
                                                                      ai_blocker_cards[s.attacker_index]);
        if ((block_interaction_flags & 1) != 0)
        {
          DAT_00708108[s.card] |= 1 << (unsigned char)s.attacker_index;
        }
        if ((block_interaction_flags & 2) != 0)
        {
          DAT_007081b0[s.attacker_index] |= 1 << (unsigned char)s.card;
        }
      }
    }
    g_duel_ai_mode_state = s.saved_ai_mode;
  }
  DAT_007081a8 = 0;
  DAT_0093f4b0 = 0;
  DAT_00707ee8 = DAT_0057dd20[combat_damage_attacker_count];
}

// FUNCTION: MAGIC 0x004e4913
// FUNCTION: SHANDALAR 0x004c3b83
void save_combat_simulation_state(void)
{
  memcpy(combat_sim_saved_card_instances, global_card_instances, sizeof(combat_sim_saved_card_instances));
  memcpy(combat_sim_saved_dynamic_card_data, global_cards_data + g_card_count, sizeof(combat_sim_saved_dynamic_card_data));
  memcpy(combat_sim_saved_library, global_library, sizeof(combat_sim_saved_library));
  memcpy(combat_sim_saved_graveyard_slots, global_graveyard_slots, sizeof(combat_sim_saved_graveyard_slots));
  memcpy(combat_sim_saved_exile, global_exile, sizeof(combat_sim_saved_exile));
  memcpy(combat_sim_saved_special_mana_pool, special_mana_pool, sizeof(combat_sim_saved_special_mana_pool));
  memcpy(combat_sim_saved_raw_mana_available, raw_mana_available, sizeof(combat_sim_saved_raw_mana_available));
  memcpy(combat_sim_saved_basiclandtypes_controlled, basiclandtypes_controlled, sizeof(combat_sim_saved_basiclandtypes_controlled));
  memcpy(combat_sim_saved_raw_mana_available_hex, raw_mana_available_hex, sizeof(combat_sim_saved_raw_mana_available_hex));
  memcpy(combat_sim_saved_life, life, sizeof(combat_sim_saved_life));
  memcpy(combat_sim_saved_DAT_007abce0, &DAT_007abce0, sizeof(combat_sim_saved_DAT_007abce0));
  memcpy(combat_sim_saved_card_types_in_play, card_types_in_play, sizeof(combat_sim_saved_card_types_in_play));
  memcpy(combat_sim_saved_graveyard_card_types, graveyard_card_types, sizeof(combat_sim_saved_graveyard_card_types));
  memcpy(combat_sim_saved_duel_summary, &duel_summary, sizeof(combat_sim_saved_duel_summary));
  memcpy(combat_sim_saved_ante_cards, global_ante_cards, sizeof(combat_sim_saved_ante_cards));
  memcpy(combat_sim_saved_timestamp_player, TENTATIVE_timestamp_player, sizeof(combat_sim_saved_timestamp_player));
  memcpy(combat_sim_saved_timestamp_card, TENTATIVE_timestamp_card, sizeof(combat_sim_saved_timestamp_card));
  combat_sim_saved_land_can_be_played = land_can_be_played;
  combat_sim_saved_current_phase = current_phase;
  combat_sim_saved_current_casting_internal_card_id = current_casting_internal_card_id;
  combat_sim_saved_attacking_creature_count = attacking_creature_count;
  combat_sim_saved_spell_fizzled = spell_fizzled;
  memcpy(combat_sim_saved_stack_data, stack_data, sizeof(combat_sim_saved_stack_data));
  memcpy(combat_sim_saved_stack_cards, global_stack_cards, sizeof(combat_sim_saved_stack_cards));
  memcpy(combat_sim_saved_stack_damage_targets, global_stack_damage_targets, sizeof(combat_sim_saved_stack_damage_targets));
  memcpy(combat_sim_saved_active_cards_count, active_cards_count, sizeof(combat_sim_saved_active_cards_count));
  combat_sim_saved_stack_size = stack_size;
  combat_sim_saved_pending_killed_card_handler = pending_killed_card_handler;
  combat_sim_saved_ai_modifier = ai_modifier;
  memcpy(combat_sim_saved_mana_charge, mana_charge, sizeof(combat_sim_saved_mana_charge));
  combat_sim_saved_x_value = x_value;
  combat_sim_saved_max_x_value = max_x_value;
  combat_sim_saved_extra_turn_player = g_duel_extra_turn_player;
  combat_sim_saved_DAT_0093a848 = DAT_0093a848;
  combat_sim_saved_stop_phase = stop_phase;
  combat_sim_saved_stop_phase_player = stop_phase_player;
  combat_sim_saved_battlefield_extra_ability_flags = battlefield_extra_ability_flags;
  memcpy(combat_sim_saved_unk_008b44d0, unk_008b44d0, sizeof(combat_sim_saved_unk_008b44d0));
  memcpy(combat_sim_saved_unk_007a79b0, unk_007a79b0, sizeof(combat_sim_saved_unk_007a79b0));
}

// FUNCTION: MAGIC 0x004e4bc4
// FUNCTION: SHANDALAR 0x004c3e34
void restore_combat_simulation_state(void)
{
  memcpy(global_card_instances, combat_sim_saved_card_instances, sizeof(combat_sim_saved_card_instances));
  memcpy(global_cards_data + g_card_count, combat_sim_saved_dynamic_card_data, sizeof(combat_sim_saved_dynamic_card_data));
  memcpy(global_library, combat_sim_saved_library, sizeof(combat_sim_saved_library));
  memcpy(global_graveyard_slots, combat_sim_saved_graveyard_slots, sizeof(combat_sim_saved_graveyard_slots));
  memcpy(global_exile, combat_sim_saved_exile, sizeof(combat_sim_saved_exile));
  memcpy(special_mana_pool, combat_sim_saved_special_mana_pool, sizeof(combat_sim_saved_special_mana_pool));
  memcpy(raw_mana_available, combat_sim_saved_raw_mana_available, sizeof(combat_sim_saved_raw_mana_available));
  memcpy(basiclandtypes_controlled, combat_sim_saved_basiclandtypes_controlled, sizeof(combat_sim_saved_basiclandtypes_controlled));
  memcpy(raw_mana_available_hex, combat_sim_saved_raw_mana_available_hex, sizeof(combat_sim_saved_raw_mana_available_hex));
  memcpy(life, combat_sim_saved_life, sizeof(combat_sim_saved_life));
  memcpy(&DAT_007abce0, combat_sim_saved_DAT_007abce0, sizeof(combat_sim_saved_DAT_007abce0));
  memcpy(card_types_in_play, combat_sim_saved_card_types_in_play, sizeof(combat_sim_saved_card_types_in_play));
  memcpy(graveyard_card_types, combat_sim_saved_graveyard_card_types, sizeof(combat_sim_saved_graveyard_card_types));
  memcpy(&duel_summary, combat_sim_saved_duel_summary, sizeof(combat_sim_saved_duel_summary));
  memcpy(global_ante_cards, combat_sim_saved_ante_cards, sizeof(combat_sim_saved_ante_cards));
  memcpy(TENTATIVE_timestamp_player, combat_sim_saved_timestamp_player, sizeof(combat_sim_saved_timestamp_player));
  memcpy(TENTATIVE_timestamp_card, combat_sim_saved_timestamp_card, sizeof(combat_sim_saved_timestamp_card));
  land_can_be_played = combat_sim_saved_land_can_be_played;
  current_phase = combat_sim_saved_current_phase;
  current_casting_internal_card_id = combat_sim_saved_current_casting_internal_card_id;
  attacking_creature_count = combat_sim_saved_attacking_creature_count;
  spell_fizzled = combat_sim_saved_spell_fizzled;
  memcpy(stack_data, combat_sim_saved_stack_data, sizeof(combat_sim_saved_stack_data));
  memcpy(global_stack_cards, combat_sim_saved_stack_cards, sizeof(combat_sim_saved_stack_cards));
  memcpy(global_stack_damage_targets, combat_sim_saved_stack_damage_targets, sizeof(combat_sim_saved_stack_damage_targets));
  memcpy(active_cards_count, combat_sim_saved_active_cards_count, sizeof(combat_sim_saved_active_cards_count));
  stack_size = combat_sim_saved_stack_size;
  pending_killed_card_handler = combat_sim_saved_pending_killed_card_handler;
  ai_modifier = combat_sim_saved_ai_modifier;
  memcpy(mana_charge, combat_sim_saved_mana_charge, sizeof(combat_sim_saved_mana_charge));
  x_value = combat_sim_saved_x_value;
  max_x_value = combat_sim_saved_max_x_value;
  g_duel_extra_turn_player = combat_sim_saved_extra_turn_player;
  DAT_0093a848 = combat_sim_saved_DAT_0093a848;
  stop_phase = combat_sim_saved_stop_phase;
  stop_phase_player = combat_sim_saved_stop_phase_player;
  battlefield_extra_ability_flags = combat_sim_saved_battlefield_extra_ability_flags;
  memcpy(unk_008b44d0, combat_sim_saved_unk_008b44d0, sizeof(combat_sim_saved_unk_008b44d0));
  memcpy(unk_007a79b0, combat_sim_saved_unk_007a79b0, sizeof(combat_sim_saved_unk_007a79b0));
}

// FUNCTION: MAGIC 0x005129d1
// FUNCTION: SHANDALAR 0x00421578
void choose_blockers_ai(int player)
{
  int saved_event_state;
  int blocker_index;

  saved_event_state = unk_00712938;
  unk_00712938 = 2;
  setup_combat_damage_simulation(player);
  DAT_00708698 = 0xffffd8f1;
  for (blocker_index = 0; blocker_index < 0x10; blocker_index++)
  {
    ai_block_damage_orders[blocker_index] = 0;
  }
  enumerate_ai_block_assignments(player, 0);
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
        request_duel_display_refresh_if_human(ai_blocker_player, ai_blocker_cards[blocker_index], 5, 2);
        if (ai_block_assignments[blocker_index] != -1)
        {
          play_sound_effect(0x15);
        }
      }
    }
  }
  unk_00712938 = saved_event_state;
}

// FUNCTION: MAGIC 0x00512bb2
// FUNCTION: SHANDALAR 0x00421758
void enumerate_ai_block_assignments(int player, int blocker_index)
{
  struct
  {
    card_instance_t *blocker;
    int attacker_index;
    int score;
  } s;

  if (ai_blocker_count == blocker_index)
  {
    s.score = score_current_ai_block_assignment();
    if (DAT_00708698 < s.score)
    {
      DAT_00708698 = s.score;
      memcpy(ai_block_assignments, ai_candidate_block_assignments, 0x1c);
      DAT_00707ff8 = DAT_00708610;
    }
  }
  else
  {
    s.blocker = &global_card_instances[ai_blocker_player][ai_blocker_cards[blocker_index]];
    if (((s.blocker->state & STATE_UNKNOWN8000) == 0) || (s.blocker->blocking == -1))
    {
      ai_candidate_block_assignments[blocker_index] = -1;
      enumerate_ai_block_assignments(player, blocker_index + 1);
    }
    for (s.attacker_index = 0; s.attacker_index < combat_damage_attacker_count; s.attacker_index++)
    {
      if (ai_attacker_blocker_counts[s.attacker_index] >= DAT_00707ee8)
      {
        continue;
      }
      if ((DAT_00707ef0[s.attacker_index] & (1 << (unsigned char)blocker_index)) == 0)
      {
        continue;
      }
      if ((s.blocker->state & STATE_UNKNOWN8000) != 0)
      {
        if (combat_damage_attacker_cards[s.attacker_index] != s.blocker->blocking)
        {
          continue;
        }
      }
      {
        ai_attacker_blocker_counts[s.attacker_index]++;
        ai_candidate_block_assignments[blocker_index] = combat_damage_attacker_cards[s.attacker_index];
        enumerate_ai_block_assignments(player, blocker_index + 1);
        ai_attacker_blocker_counts[s.attacker_index]--;
      }
    }
  }
}

// FUNCTION: MAGIC 0x00512d46
// FUNCTION: SHANDALAR 0x004218ec
int score_current_ai_block_assignment(void)
{
  struct
  {
    int best_prevented_damage;
    int best_prevention_index;
    int blocker_damage;
    int attacker_damage_remaining;
    int blocker_index;
    int blocked_attacker_index;
    int blockers_on_attacker;
    int lethal_blocker_count;
    int best_damage_assignment_score;
    unsigned int damage_assignment_mask;
    int total_blocker_toughness;
    int best_blocker_score;
    int attacker_damage;
    int blocker_scan_index;
    int has_banding;
    int blocker_score_candidate;
    int attacker_index;
    int current_damage_assignment_score;
    int cop_mana;
    int saved_life;
    int prevention_count;
    int remaining_life;
    int prevented_damage[16];
    unsigned int destroyed_by_attacker_mask;
    int vulnerable_blockers[16];
    int assignment_score;
    int best_blocker;
  } s;

  s.cop_mana = 0;
  unk_00925d3c = 0;
  s.assignment_score = unk_00925d3c;
  s.remaining_life = life[ai_blocker_player];
  s.prevention_count = 0;
  DAT_00708610 = 0;
  if (defending_cop_color_mask != 0)
  {
    s.cop_mana = defending_cop_available_mana;
  }

  for (s.attacker_index = 0; s.attacker_index < combat_damage_attacker_count; s.attacker_index++)
  {
    s.attacker_damage = combat_damage_attacker_damage[s.attacker_index];
    s.blocker_damage = 0;
    s.total_blocker_toughness = 0;
    s.blocked_attacker_index = -1;
    s.best_blocker_score = -1;
    s.lethal_blocker_count = 0;
    s.has_banding = 0;
    s.destroyed_by_attacker_mask = 0;
    s.blockers_on_attacker = 0;
    unk_00925d3c += DAT_00708128[s.attacker_index];

    for (s.blocker_index = 0; s.blocker_index < ai_blocker_count; s.blocker_index++)
    {
      if (combat_damage_attacker_cards[s.attacker_index] != ai_candidate_block_assignments[s.blocker_index])
      {
        continue;
      }
      s.blockers_on_attacker++;
      unk_00925d3c += DAT_007080c8[s.blocker_index];
      s.blocker_damage += combat_damage_blocker_damage[s.blocker_index];
      s.total_blocker_toughness += combat_damage_blocker_toughness[s.blocker_index];
      s.blocked_attacker_index = s.blocker_index;
      if ((combat_damage_blocker_abilities[s.blocker_index] & 0x40) != 0)
      {
        s.has_banding = 1;
      }
      if ((DAT_007081b0[s.blocker_index] & (1 << (unsigned char)s.attacker_index)) != 0)
      {
        s.blocker_damage += 99;
      }
      if ((DAT_00708108[s.attacker_index] & (1 << (unsigned char)s.blocker_index)) != 0)
      {
        s.destroyed_by_attacker_mask |= 1 << (unsigned char)s.blocker_index;
      }
      if (((combat_damage_blocker_toughness[s.blocker_index] <= s.attacker_damage) ||
           (s.destroyed_by_attacker_mask != 0)) &&
          ((combat_damage_blocker_abilities[s.blocker_index] & 0x200) == 0))
      {
        s.vulnerable_blockers[s.lethal_blocker_count] = s.blocker_index;
        s.lethal_blocker_count++;
      }
      s.blocker_score_candidate = DAT_00707c98[s.blocker_index];
      if (s.best_blocker_score < s.blocker_score_candidate)
      {
        s.best_blocker_score = s.blocker_score_candidate;
        s.best_blocker = s.blocker_index;
      }
    }

    s.saved_life = s.remaining_life;
    if (s.blocked_attacker_index == -1)
    {
      if ((untapped_royal_assassin_count == 0) || (untapped_royal_assassin_count <= DAT_00707ea8[s.attacker_index]))
      {
        if ((combat_damage_attacker_abilities[s.attacker_index] & 0x80000000) != 0)
        {
          s.remaining_life -= s.attacker_damage + 2;
        }
        else
        {
          s.remaining_life -= s.attacker_damage;
        }
      }
    }
    else if (((combat_damage_attacker_abilities[s.attacker_index] & 0x80) != 0) &&
             (s.total_blocker_toughness < s.attacker_damage))
    {
      s.remaining_life -= s.attacker_damage - s.total_blocker_toughness;
    }

    if (((defending_cop_color_mask != 0) && (s.remaining_life < s.saved_life)) &&
        ((defending_cop_color_mask &
          ((int)(char)global_card_instances[1 - ai_blocker_player][combat_damage_attacker_cards[s.attacker_index]].color)) != 0))
    {
      s.prevented_damage[s.prevention_count] = s.saved_life - s.remaining_life;
      s.prevention_count++;
    }

    if ((s.blocked_attacker_index != -1) &&
        (((combat_damage_attacker_toughness[s.attacker_index] <= s.blocker_damage) &&
          (((combat_damage_attacker_abilities[s.attacker_index] & 0x300) == 0) ||
           (s.lethal_blocker_count == 0))) ||
         ((untapped_royal_assassin_count != 0) &&
          (DAT_00707ea8[s.attacker_index] < untapped_royal_assassin_count))))
    {
      s.assignment_score += (ai_combat_value_weights[3 - ai_blocker_player] * DAT_00708658[s.attacker_index]) / 8;
    }

    s.best_damage_assignment_score = -1;
    for (s.damage_assignment_mask = 0;
         (int)s.damage_assignment_mask < (1 << (unsigned char)s.lethal_blocker_count);
         s.damage_assignment_mask++)
    {
      s.attacker_damage_remaining = s.attacker_damage;
      if (s.destroyed_by_attacker_mask != 0)
      {
        s.attacker_damage_remaining = 99;
      }
      else if (s.has_banding)
      {
        if (s.blocker_damage - s.blockers_on_attacker >= s.attacker_damage)
        {
          s.current_damage_assignment_score = 0;
          goto finish_damage_assignment_score;
        }
        if (s.damage_assignment_mask != 0)
        {
          s.current_damage_assignment_score = 9999;
        }
        else
        {
          s.current_damage_assignment_score = 0;
        }
        for (s.blocker_scan_index = 0; s.lethal_blocker_count > s.blocker_scan_index; s.blocker_scan_index++)
        {
          if ((s.damage_assignment_mask & (1 << (unsigned char)s.blocker_scan_index)) != 0)
          {
            s.blocker_index = s.vulnerable_blockers[s.blocker_scan_index];
            if (DAT_00708728[s.blocker_index] < s.current_damage_assignment_score)
            {
              s.current_damage_assignment_score = DAT_00708728[s.blocker_index];
            }
            if (s.attacker_damage < combat_damage_blocker_toughness[s.blocker_index])
            {
              s.current_damage_assignment_score = 0;
            }
            else if (((combat_damage_blocker_abilities[s.blocker_index] & 0x1ff800) != 0) &&
                     ((((int)(char)global_card_instances[1 - ai_blocker_player][combat_damage_attacker_cards[s.attacker_index]].color << 10) &
                       combat_damage_blocker_abilities[s.blocker_index] & 0x1ff800) != 0))
            {
              s.current_damage_assignment_score = 0;
            }
          }
        }
        goto finish_damage_assignment_score;
      }

      s.current_damage_assignment_score = 0;
      for (s.blocker_scan_index = 0; s.lethal_blocker_count > s.blocker_scan_index; s.blocker_scan_index++)
      {
        if ((s.damage_assignment_mask & (1 << (unsigned char)s.blocker_scan_index)) != 0)
        {
          s.blocker_index = s.vulnerable_blockers[s.blocker_scan_index];
          if ((((combat_damage_blocker_abilities[s.blocker_index] & 0x100) == 0) ||
               (combat_damage_blocker_damage[s.blocker_index] < combat_damage_attacker_toughness[s.attacker_index])) &&
              ((((int)(char)global_card_instances[1 - ai_blocker_player][combat_damage_attacker_cards[s.attacker_index]].color << 10) &
                combat_damage_blocker_abilities[s.blocker_index] & 0x1ff800) == 0))
          {
            s.current_damage_assignment_score += DAT_00708728[s.blocker_index];
          }
          s.attacker_damage_remaining -= combat_damage_blocker_toughness[s.blocker_index];
          if (s.attacker_damage_remaining < 0)
          {
            break;
          }
        }
      }

    finish_damage_assignment_score:
      if ((s.attacker_damage_remaining >= 0) && (s.best_damage_assignment_score < s.current_damage_assignment_score))
      {
        s.best_damage_assignment_score = s.current_damage_assignment_score;
        DAT_00707bf8 = s.damage_assignment_mask;
      }
    }

    if (s.lethal_blocker_count != 0)
    {
      ai_block_damage_orders[s.attacker_index] = DAT_00707bf8;
    }
    else
    {
      ai_block_damage_orders[s.attacker_index] = -s.best_blocker;
    }
    s.assignment_score -= s.best_damage_assignment_score;
  }

  s.assignment_score += unk_00925d3c;
  if (s.prevention_count != 0)
  {
    while (s.cop_mana != 0)
    {
      s.best_prevented_damage = 0;
      for (s.attacker_index = 0; s.prevention_count > s.attacker_index; s.attacker_index++)
      {
        if (s.best_prevented_damage < s.prevented_damage[s.attacker_index])
        {
          s.best_prevented_damage = s.prevented_damage[s.attacker_index];
          s.best_prevention_index = s.attacker_index;
        }
      }
      if (s.best_prevented_damage != 0)
      {
        s.remaining_life += s.prevented_damage[s.best_prevention_index];
        s.prevented_damage[s.best_prevention_index] = 0;
        s.cop_mana--;
      }
      else
      {
        s.cop_mana = 0;
      }
    }
  }
  if (s.remaining_life <= 0)
  {
    s.assignment_score -= 999 - (s.remaining_life * 0x60);
  }
  else
  {
    s.assignment_score -= (((life[ai_blocker_player] - s.remaining_life) * ai_combat_value_weights[ai_blocker_player] * 0x18) / 4) / s.remaining_life;
    if (DAT_00707dd8 != 0)
    {
      DAT_00708610 = ((life[ai_blocker_player] - s.remaining_life) * 700) / life[ai_blocker_player];
      s.assignment_score -= DAT_00708610;
    }
  }
  return s.assignment_score;
}

// FUNCTION: MAGIC 0x00515f34
// FUNCTION: SHANDALAR 0x00424ae4
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
