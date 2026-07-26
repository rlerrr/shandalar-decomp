#include <windows.h>
#include <stdio.h>
#include <string.h>

#include "defs.h"
#include "magic/src/cards.h"
#include "magic/src/duel_engine.h"
#include "magic/src/game_support.h"
#include "magic/src/global_duel_ui_ids.h"
#include "magic/src/global_other.h"
#include "magic/src/global_state.h"
#include "magic/src/global_strings.h"

extern int ai_blocker_count;
extern int ai_blocker_cards[16];

void update_phase_display(int player, phase_t phase);
int prompt_stop_phase_anyway(phase_t phase);
char *get_displayed_card_name(int player, int card);
int process_killed_card(int player, int card);
int FUN_00515ee6(int damage_step, int abilities);
int adjust_two_headed_giant_damage(int player, int card, int amount);
void clear_combat_damage_assignment_matrix(void);
void assign_attacker_combat_damage(int player, int index, int has_trample, int damage_ids, int damage_step, int mode, int *best_low, int *best_high);
void assign_blocker_combat_damage(int player, int index, int multiple_attackers, int damage_ids, int damage_step, int mode, int *best_low, int *best_high);
void highlight_combat_damage_attacker(int player, int card, int highlight);
void highlight_combat_damage_blocker(int player, int card, int highlight);
int has_damage_card_targeting(int player, int card);

#define COMBAT_DAMAGE_FLAG_COMBAT 0x40000
#define COMBAT_DAMAGE_FLAG_TRAMPLE 0x80000
#define COMBAT_DAMAGE_FLAG_FIRST_STRIKE 0x100000
#define COMBAT_DAMAGE_FLAG_CANCELABLE 0x8000000

// GLOBAL: MAGIC 0x0057dd64
char s_PROMPT_RESOLVECOMBAT_0057dd64[0x18] = "PROMPT_RESOLVECOMBAT";

// GLOBAL: MAGIC 0x0057dd7c
char DAT_0057dd7c[4] = "";

// GLOBAL: MAGIC 0x0057dd80
char DAT_0057dd80[4] = "";

// GLOBAL: MAGIC 0x0057dd84
char DAT_0057dd84[4] = "";

// GLOBAL: MAGIC 0x0057dd88
char s_PROMPT_RESOLVECOMBAT_0057dd88[0x18] = "PROMPT_RESOLVECOMBAT";

// GLOBAL: MAGIC 0x0057dda0
char DAT_0057dda0[4] = "";

// GLOBAL: MAGIC 0x0057dda4
char DAT_0057dda4[4] = "";

// GLOBAL: MAGIC 0x0057dda8
char DAT_0057dda8[4] = "";

// GLOBAL: MAGIC 0x00707c18
int combat_damage_attacker_abilities[16];

// GLOBAL: MAGIC 0x00707c58
int combat_damage_blocker_abilities[16];

// GLOBAL: MAGIC 0x00707cd8
int combat_damage_attacker_cards[16];

// GLOBAL: MAGIC 0x00707e28
int combat_damage_attacker_toughness[16];

// GLOBAL: MAGIC 0x00707e68
int combat_damage_blocker_toughness[16];

// GLOBAL: MAGIC 0x00707f78
int combat_damage_best_assignment_codes[16];

// GLOBAL: MAGIC 0x00708040
int combat_damage_current_assignment_codes[16];

// GLOBAL: MAGIC 0x00708124
int combat_damage_attacker_count;

// GLOBAL: MAGIC 0x00708210
int combat_damage_assignment_matrix[16][16];

// GLOBAL: MAGIC 0x00708618
int combat_damage_attacker_damage[16];

// GLOBAL: MAGIC 0x007086a0
int combat_damage_blocker_damage[16];

// FUNCTION: MAGIC 0x00515bba
int adjust_two_headed_giant_damage(int player, int card, int amount)
{
  struct
  {
    card_instance_t *instance;
    int index;
    card_function_pointer code_pointer;
    int found_trampler;
    int damage_source_card;
    int damage_source_player;
    int damage_player;
  } s;

  s.damage_player = player;
  s.found_trampler = 0;
  s.code_pointer = global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].code_pointer;
  s.damage_source_player = (int)(char)PLAYER_CARD_INSTANCE(player, card).damage_source_player;
  s.damage_source_card = PLAYER_CARD_INSTANCE(player, card).damage_source_card;

  if (s.code_pointer == card_two_headed_giant_of_foriys || s.code_pointer == FUN_00481e25)
  {
    s.found_trampler = 1;
  }

  for (s.index = 0; s.index < active_cards_count[s.damage_player] && s.found_trampler == 0; s.index = s.index + 1)
  {
    if (PLAYER_CARD_INSTANCE(s.damage_player, s.index).internal_card_id == unk_008a8df0 &&
        PLAYER_CARD_INSTANCE(s.damage_player, s.index).damage_target_player == player &&
        PLAYER_CARD_INSTANCE(s.damage_player, s.index).damage_target_card == card)
    {
      s.found_trampler = 1;
    }
  }

  if (s.found_trampler != 0)
  {
    if (s.code_pointer != FUN_00481e25)
    {
      for (s.index = 0; s.index < active_cards_count[s.damage_player]; s.index = s.index + 1)
      {
        s.instance = &PLAYER_CARD_INSTANCE(s.damage_player, s.index);
        if (global_cards_data[s.instance->internal_card_id].code_pointer == FUN_00481e25 &&
            s.instance->damage_source_player == player &&
            s.instance->damage_source_card == card)
        {
          s.instance->power = amount;
          s.instance->info_slot -= 1;
        }
      }
    }
    else
    {
      for (s.index = 0; s.index < active_cards_count[s.damage_player]; s.index = s.index + 1)
      {
        s.instance = &PLAYER_CARD_INSTANCE(s.damage_player, s.index);
        if (s.index != card &&
            global_cards_data[s.instance->internal_card_id].code_pointer == FUN_00481e25 &&
            s.instance->damage_source_player == s.damage_source_player &&
            s.instance->damage_source_card == s.damage_source_card)
        {
          s.instance->power = amount;
          s.instance->info_slot -= 1;
        }
      }
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x00515ee6
int FUN_00515ee6(int damage_step, int abilities)
{
  if (damage_step == 0 && (abilities & 0x100) != 0)
  {
    return 1;
  }

  if (damage_step != 0 && (abilities & 0x100) == 0)
  {
    return 1;
  }

  return 0;
}

// FUNCTION: MAGIC 0x00516063
void clear_combat_damage_assignment_matrix(void)
{
  int blocker_index;
  int attacker_index;

  for (blocker_index = 0; blocker_index < 16; blocker_index = blocker_index + 1)
  {
    for (attacker_index = 0; attacker_index < 16; attacker_index = attacker_index + 1)
    {
      combat_damage_assignment_matrix[attacker_index][blocker_index] = 0;
    }
  }
}

// FUNCTION: MAGIC 0x005160c1
void assign_attacker_combat_damage(int player, int attacker_index, int has_trample, int damage_ids, int damage_step, int mode, int *best_low, int *best_high)
{
  struct
  {
    union
    {
      int terminal_total_assigned;
      int assignment_base;
    } shared_value;         // ebp - 0x3c
    int combination_count;  // ebp - 0x38
    int damage_remaining;   // ebp - 0x34
    int defending_player;   // ebp - 0x30
    int assignment_index;   // ebp - 0x2c
    int assignment_code;    // ebp - 0x28
    int blocker_index;      // ebp - 0x24
    int damage_to_assign;   // ebp - 0x20
    int use_shortcut;       // ebp - 0x1c
    int trample_assigned;   // ebp - 0x18
    int attacker_card;      // ebp - 0x14
    int damage_card;        // ebp - 0x10
    int life_after_trample; // ebp - 0xc
    int score;              // ebp - 0x8
    int trample_damage;     // ebp - 0x4
  } s;

  s.defending_player = 1 - player;
  if (combat_damage_attacker_count <= attacker_index)
  {
    s.score = 0;
    s.life_after_trample = life[s.defending_player];
    for (s.blocker_index = 0; s.blocker_index < ai_blocker_count; s.blocker_index = s.blocker_index + 1)
    {
      s.shared_value.terminal_total_assigned = s.trample_assigned = 0;
      for (s.assignment_index = 0; s.assignment_index < combat_damage_attacker_count; s.assignment_index = s.assignment_index + 1)
      {
        s.shared_value.terminal_total_assigned += combat_damage_assignment_matrix[s.assignment_index][s.blocker_index];
        if ((combat_damage_attacker_abilities[s.assignment_index] & 0x80) != 0)
        {
          s.trample_assigned += combat_damage_assignment_matrix[s.assignment_index][s.blocker_index];
          s.attacker_card = combat_damage_attacker_cards[s.assignment_index];
        }
      }
      if (combat_damage_blocker_toughness[s.blocker_index] <= s.shared_value.terminal_total_assigned)
      {
        s.score += PLAYER_CARD_INSTANCE(s.defending_player, ai_blocker_cards[s.blocker_index]).attack_rating;
        s.score += s.shared_value.terminal_total_assigned - combat_damage_blocker_toughness[s.blocker_index];
        s.trample_damage = ClampIntToRange(s.shared_value.terminal_total_assigned - combat_damage_blocker_toughness[s.blocker_index], 0, s.trample_assigned);
        s.life_after_trample -= s.trample_damage;
      }
      else
      {
        s.score += s.shared_value.terminal_total_assigned * 2;
      }
    }

    if (s.life_after_trample <= 0)
    {
      s.score += 999 - s.life_after_trample * 0x18;
    }
    else
    {
      s.score += ((life[s.defending_player] - s.life_after_trample) * 0x30) / s.life_after_trample;
    }

    if (has_trample == 0)
    {
      if (*best_high < s.score)
      {
        *best_high = s.score;
        for (s.blocker_index = 0; s.blocker_index < 16; s.blocker_index = s.blocker_index + 1)
        {
          combat_damage_best_assignment_codes[s.blocker_index] = combat_damage_current_assignment_codes[s.blocker_index];
        }
      }
    }
    else if (s.score < *best_low)
    {
      *best_low = s.score;
      for (s.blocker_index = 0; s.blocker_index < 16; s.blocker_index = s.blocker_index + 1)
      {
        combat_damage_best_assignment_codes[s.blocker_index] = combat_damage_current_assignment_codes[s.blocker_index];
      }
    }
  }
  else
  {
    s.shared_value.assignment_base = combat_damage_attacker_damage[attacker_index] + 1;
    s.combination_count = 1;
    for (s.blocker_index = 0; s.blocker_index < ai_blocker_count; s.blocker_index = s.blocker_index + 1)
    {
      s.combination_count *= s.shared_value.assignment_base;
    }
    s.use_shortcut = (g_shandalar_difficulty + 1) * 0x100 < s.combination_count;

    if (mode != 0)
    {
      s.damage_remaining = s.shared_value.assignment_base - 1;
      s.assignment_code = combat_damage_best_assignment_codes[attacker_index];
      for (s.assignment_index = 0; s.assignment_index < ai_blocker_count; s.assignment_index = s.assignment_index + 1)
      {
        s.damage_to_assign = s.assignment_code % s.shared_value.assignment_base;
        if (has_damage_card_targeting(s.defending_player, ai_blocker_cards[s.assignment_index]) != 0)
        {
          s.damage_to_assign = 0;
        }
        if (s.damage_to_assign != 0)
        {
          s.damage_card = damage_creature(s.defending_player, ai_blocker_cards[s.assignment_index], s.damage_to_assign,
                                          player, combat_damage_attacker_cards[attacker_index]);
          *(int *)(damage_ids + s.assignment_index * 4) = s.damage_card;
          if (s.damage_card != -1)
          {
            PLAYER_CARD_INSTANCE(player, s.damage_card).token_status |= COMBAT_DAMAGE_FLAG_COMBAT;
            if ((combat_damage_attacker_abilities[attacker_index] & 0x80) != 0)
            {
              PLAYER_CARD_INSTANCE(player, s.damage_card).token_status |= COMBAT_DAMAGE_FLAG_TRAMPLE;
            }
            if (damage_step == 0)
            {
              PLAYER_CARD_INSTANCE(player, s.damage_card).token_status |= COMBAT_DAMAGE_FLAG_FIRST_STRIKE;
            }
          }
        }
        combat_damage_assignment_matrix[attacker_index][s.assignment_index] = s.damage_to_assign;
        s.assignment_code /= s.shared_value.assignment_base;
        s.damage_remaining -= s.damage_to_assign;
      }
      assign_attacker_combat_damage(player, attacker_index + 1, has_trample, damage_ids, damage_step, mode, best_low, best_high);
    }
    else if (s.use_shortcut != 0)
    {
      s.damage_remaining = s.shared_value.assignment_base - 1;
      s.assignment_code = 0;
      for (s.blocker_index = ai_blocker_count - 1; 0 < s.blocker_index; s.blocker_index = s.blocker_index - 1)
      {
        if (combat_damage_blocker_toughness[s.blocker_index] < s.damage_remaining &&
            (combat_damage_blocker_abilities[s.blocker_index] & 0x200) == 0)
        {
          s.assignment_code += combat_damage_blocker_toughness[s.blocker_index];
          combat_damage_assignment_matrix[attacker_index][s.blocker_index] =
              combat_damage_blocker_toughness[s.blocker_index];
          s.damage_remaining -= combat_damage_blocker_toughness[s.blocker_index];
        }
        s.assignment_code *= s.shared_value.assignment_base;
      }
      if (s.damage_remaining != 0)
      {
        s.assignment_code += s.damage_remaining;
      }
      combat_damage_current_assignment_codes[attacker_index] = s.assignment_code;
      assign_attacker_combat_damage(player, attacker_index + 1, has_trample, damage_ids, damage_step, mode, best_low, best_high);
    }
    else
    {
      s.combination_count = MIN(s.combination_count, 5000000);
      for (s.blocker_index = 0; s.blocker_index < s.combination_count; s.blocker_index = s.blocker_index + 1)
      {
        s.damage_remaining = s.shared_value.assignment_base - 1;
        s.assignment_code = s.blocker_index;
        memset(&combat_damage_assignment_matrix[attacker_index][0], 0, 0x40);

        for (s.assignment_index = 0; s.assignment_index < ai_blocker_count && 0 < s.damage_remaining; s.assignment_index = s.assignment_index + 1)
        {
          s.damage_to_assign = s.assignment_code % s.shared_value.assignment_base;
          if (s.use_shortcut != 0 && s.damage_to_assign > 1 && s.damage_to_assign < s.damage_remaining)
          {
            break;
          }
          combat_damage_assignment_matrix[attacker_index][s.assignment_index] = s.damage_to_assign;
          s.assignment_code /= s.shared_value.assignment_base;
          s.damage_remaining -= s.damage_to_assign;
        }
        if (s.damage_remaining != 0)
        {
          continue;
        }

        combat_damage_current_assignment_codes[attacker_index] = s.blocker_index;
        assign_attacker_combat_damage(player, attacker_index + 1, has_trample, damage_ids, damage_step, mode, best_low, best_high);
      }
    }
  }
}

// FUNCTION: MAGIC 0x00516736
void assign_blocker_combat_damage(int player, int blocker_index, int multiple_attackers, int damage_ids, int damage_step, int mode, int *best_low, int *best_high)
{
  struct
  {
    union
    {
      int terminal_total_assigned;
      int assignment_base;
    } shared_value;
    int combination_count;
    int damage_remaining;
    int defending_player;
    int attacker_index;
    int assignment_code;
    int assignment_index;
    int damage_to_assign;
    int restricted_assignments;
    int damage_card;
    int score;
  } s;

  s.defending_player = 1 - player;
  if (!(ai_blocker_count > blocker_index))
  {
    s.score = 0;
    for (s.assignment_index = 0; s.assignment_index < combat_damage_attacker_count; s.assignment_index = s.assignment_index + 1)
    {
      s.shared_value.terminal_total_assigned = 0;
      for (s.attacker_index = 0; s.attacker_index < ai_blocker_count; s.attacker_index = s.attacker_index + 1)
      {
        s.shared_value.terminal_total_assigned += combat_damage_assignment_matrix[s.attacker_index][s.assignment_index];
      }
      if (combat_damage_attacker_toughness[s.assignment_index] <= s.shared_value.terminal_total_assigned)
      {
        s.score += PLAYER_CARD_INSTANCE(player, combat_damage_attacker_cards[s.assignment_index]).attack_rating;
        s.score += s.shared_value.terminal_total_assigned - combat_damage_attacker_toughness[s.assignment_index];
      }
      else
      {
        s.score += s.shared_value.terminal_total_assigned * 2;
      }
    }

    if (multiple_attackers == 0)
    {
      if (*best_high < s.score)
      {
        *best_high = s.score;
        for (s.assignment_index = 0; s.assignment_index < 16; s.assignment_index = s.assignment_index + 1)
        {
          combat_damage_best_assignment_codes[s.assignment_index] = combat_damage_current_assignment_codes[s.assignment_index];
        }
      }
    }
    else if (s.score < *best_low)
    {
      *best_low = s.score;
      for (s.assignment_index = 0; s.assignment_index < 16; s.assignment_index = s.assignment_index + 1)
      {
        combat_damage_best_assignment_codes[s.assignment_index] = combat_damage_current_assignment_codes[s.assignment_index];
      }
    }
  }
  else
  {
    if ((PLAYER_CARD_INSTANCE(s.defending_player, ai_blocker_cards[blocker_index]).state & 0x10) != 0)
    {
      assign_blocker_combat_damage(player, blocker_index + 1, multiple_attackers, damage_ids, damage_step, mode, best_low, best_high);
    }
    else
    {
      s.shared_value.assignment_base = combat_damage_blocker_damage[blocker_index] + 1;
      s.combination_count = 1;
      for (s.assignment_index = 0; s.assignment_index < combat_damage_attacker_count; s.assignment_index = s.assignment_index + 1)
      {
        s.combination_count *= s.shared_value.assignment_base;
      }
      s.restricted_assignments = s.combination_count > 0x100;

      if (mode != 0)
      {
        s.damage_remaining = s.shared_value.assignment_base - 1;
        s.assignment_code = combat_damage_best_assignment_codes[blocker_index];
        for (s.attacker_index = 0; s.attacker_index < combat_damage_attacker_count; s.attacker_index = s.attacker_index + 1)
        {
          s.damage_to_assign = s.assignment_code % s.shared_value.assignment_base;
          if (has_damage_card_targeting(s.defending_player, ai_blocker_cards[s.attacker_index]) != 0)
          {
            s.damage_to_assign = 0;
          }
          if (s.damage_to_assign != 0)
          {
            s.damage_card = damage_creature(player, combat_damage_attacker_cards[s.attacker_index], s.damage_to_assign,
                                            s.defending_player, ai_blocker_cards[blocker_index]);
            *(int *)(damage_ids + s.attacker_index * 4) = s.damage_card;
            if (s.damage_card != -1)
            {
              PLAYER_CARD_INSTANCE(s.defending_player, s.damage_card).token_status |= COMBAT_DAMAGE_FLAG_COMBAT;
              if (damage_step == 0)
              {
                PLAYER_CARD_INSTANCE(s.defending_player, s.damage_card).token_status |= COMBAT_DAMAGE_FLAG_FIRST_STRIKE;
              }
            }
          }
          s.assignment_code /= s.shared_value.assignment_base;
          s.damage_remaining -= s.damage_to_assign;
        }
        assign_blocker_combat_damage(player, blocker_index + 1, multiple_attackers, damage_ids, damage_step, mode, best_low, best_high);
      }
      else
      {
        for (s.assignment_index = 0; s.assignment_index < s.combination_count; s.assignment_index = s.assignment_index + 1)
        {
          s.damage_remaining = s.shared_value.assignment_base - 1;
          s.assignment_code = s.assignment_index;
          for (s.attacker_index = 0; s.attacker_index < combat_damage_attacker_count; s.attacker_index = s.attacker_index + 1)
          {
            s.damage_to_assign = s.assignment_code % s.shared_value.assignment_base;
            if (s.restricted_assignments != 0 && s.damage_to_assign > 1 && s.damage_to_assign < s.damage_remaining)
            {
              continue;
            }

            combat_damage_assignment_matrix[blocker_index][s.attacker_index] = s.damage_to_assign;
            s.assignment_code /= s.shared_value.assignment_base;
            s.damage_remaining -= s.damage_to_assign;
          }
          if (s.damage_remaining != 0)
          {
            continue;
          }

          combat_damage_current_assignment_codes[blocker_index] = s.assignment_index;
          assign_blocker_combat_damage(player, blocker_index + 1, multiple_attackers, damage_ids, damage_step, mode, best_low, best_high);
        }
      }
    }
  }
}

// FUNCTION: MAGIC 0x00516db1
void highlight_combat_damage_attacker(int player, int card, int highlight)
{
  if (highlight != 0)
  {
    PLAYER_CARD_INSTANCE(player, card).token_status |= 0x20000;
  }
  else
  {
    PLAYER_CARD_INSTANCE(player, card).token_status &= ~0x20000;
  }
}

// FUNCTION: MAGIC 0x00516e2f
void highlight_combat_damage_blocker(int player, int card, int highlight)
{
  if (highlight != 0)
  {
    PLAYER_CARD_INSTANCE(player, card).token_status |= 0x20000;
  }
  else
  {
    PLAYER_CARD_INSTANCE(player, card).token_status &= ~0x20000;
  }
}

// FUNCTION: MAGIC 0x0051724e
int has_damage_card_targeting(int player, int card)
{
  struct
  {
    int loop_player;
    int loop_card;
  } s;

  for (s.loop_player = 0; s.loop_player < 2; s.loop_player = s.loop_player + 1)
  {
    for (s.loop_card = 0; s.loop_card < active_cards_count[s.loop_player]; s.loop_card = s.loop_card + 1)
    {
      if (PLAYER_CARD_INSTANCE(s.loop_player, s.loop_card).damage_target_player == player &&
          PLAYER_CARD_INSTANCE(s.loop_player, s.loop_card).damage_target_card == card &&
          global_cards_data[PLAYER_CARD_INSTANCE(s.loop_player, s.loop_card).internal_card_id].id == 0x285)
      {
        return 1;
      }
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x005136ed
void resolve_combat_damage(int player)
{
  struct
  {
    int stop_prompt_result;
    int blocker_score[4];
    int trample_damage;
    int attacker_score_bounds[2]; // ebp - 0xdc
    int blocker_damage_remaining;
    int damage_to_assign;
    int multiple_attackers;
    int scan_card;
    target_t selected_target;
    int played_damage_sound;
    int damage_index;
    int attacker_damage_remaining;
    int blocker_has_trample;
    int blocker_damage_ids[16];
    int defending_player;
    int loop_index; // ebp - 0x68
    int damage_step;
    int assigning_player;
    int card;
    int total_blocker_damage;
    int firststrike_attacker_damage;
    int attacker_damage_ids[16];
    int total_attacker_damage;
    int canceled_assignment;
    int damage_effect_card;
    int target_is_valid;
  } s;

  s.defending_player = 1 - player;
  s.played_damage_sound = 0;

  for (s.damage_step = 0; s.damage_step < 2; s.damage_step++)
  {
    if (s.damage_step != 0)
    {
      current_phase = PHASE_NORMAL_COMBAT_DAMAGE;
    }
    else
    {
      current_phase = PHASE_FIRST_STRIKE_DAMAGE;
    }
    update_phase_display(player, current_phase);

    for (s.card = 0; s.card < active_cards_count[player]; s.card = s.card + 1)
    {
      if (PLAYER_CARD_INSTANCE(player, s.card).blocking != -1 &&
          PLAYER_CARD_INSTANCE(player, s.card).blocking != s.card)
      {
        continue;
      }

      if (s.played_damage_sound == 0 && g_duel_ai_mode_state != 1)
      {
        play_sound_effect(0x14);
        s.played_damage_sound = 1;
      }

      combat_damage_attacker_count = s.total_attacker_damage = s.firststrike_attacker_damage = 0;
      s.multiple_attackers = 0;

      for (s.scan_card = 0; s.scan_card < active_cards_count[player]; s.scan_card = s.scan_card + 1)
      {
        if (((s.scan_card == s.card ||
              PLAYER_CARD_INSTANCE(player, s.scan_card).blocking == s.card) &&
             PLAYER_CARD_INSTANCE(player, s.scan_card).internal_card_id != -1) &&
            ((PLAYER_CARD_INSTANCE(player, s.scan_card).state & 0x800006) == 6))
        {
          combat_damage_attacker_cards[combat_damage_attacker_count] = s.scan_card;
          combat_damage_attacker_toughness[combat_damage_attacker_count] =
              C_get_abilities(player, s.scan_card, 0x33, -1);
          combat_damage_attacker_abilities[combat_damage_attacker_count] =
              C_get_abilities(player, s.scan_card, 0x34, -1);
          combat_damage_attacker_damage[combat_damage_attacker_count] = s.attacker_damage_remaining = 0;

          if (FUN_00515ee6(s.damage_step, combat_damage_attacker_abilities[combat_damage_attacker_count]) != 0)
          {
            s.attacker_damage_remaining = C_get_abilities(player, s.scan_card, 0x32, -1);
            if (s.attacker_damage_remaining < 0)
            {
              s.attacker_damage_remaining = 0;
            }
            combat_damage_attacker_damage[combat_damage_attacker_count] = s.attacker_damage_remaining;
            s.total_attacker_damage += s.attacker_damage_remaining;
            if ((combat_damage_attacker_abilities[combat_damage_attacker_count] & 0x80) != 0)
            {
              s.firststrike_attacker_damage += s.attacker_damage_remaining;
            }
          }

          combat_damage_attacker_count = combat_damage_attacker_count + 1;
          if (combat_damage_attacker_count == 16)
          {
            break;
          }
        }
      }

      if (1 < combat_damage_attacker_count)
      {
        s.multiple_attackers = 1;
      }

      ai_blocker_count = s.total_blocker_damage = 0;
      s.blocker_has_trample = 0;

      for (s.scan_card = 0; s.scan_card < active_cards_count[s.defending_player]; s.scan_card = s.scan_card + 1)
      {
        if (is_in_play(s.defending_player, s.scan_card) != 0 &&
            PLAYER_CARD_INSTANCE(s.defending_player, s.scan_card).blocking == s.card)
        {
          ai_blocker_cards[ai_blocker_count] = s.scan_card;
          combat_damage_blocker_toughness[ai_blocker_count] =
              C_get_abilities(s.defending_player, s.scan_card, 0x33, s.card) -
              PLAYER_CARD_INSTANCE(s.defending_player, s.scan_card).damage_on_card;
          combat_damage_blocker_abilities[ai_blocker_count] =
              C_get_abilities(s.defending_player, s.scan_card, 0x34, -1);

          combat_damage_blocker_damage[ai_blocker_count] = s.blocker_damage_remaining = 0;

          if ((PLAYER_CARD_INSTANCE(s.defending_player, s.scan_card).state & 0x10) == 0 &&
              FUN_00515ee6(s.damage_step, combat_damage_blocker_abilities[ai_blocker_count]) != 0)
          {
            s.blocker_damage_remaining = C_get_abilities(s.defending_player, s.scan_card, 0x32, s.card);
            if (s.blocker_damage_remaining < 0)
            {
              s.blocker_damage_remaining = 0;
            }
            combat_damage_blocker_damage[ai_blocker_count] = s.blocker_damage_remaining;
            s.total_blocker_damage += s.blocker_damage_remaining;
          }

          if ((combat_damage_blocker_abilities[ai_blocker_count] & 0x40) != 0)
          {
            s.blocker_has_trample = 1;
          }

          ai_blocker_count = ai_blocker_count + 1;
          if (ai_blocker_count == 16)
          {
            break;
          }
        }
      }

      if (ai_blocker_count != 0)
      {
        for (s.damage_index = 0; s.damage_index < combat_damage_attacker_count; s.damage_index = s.damage_index + 1)
        {
          PLAYER_CARD_INSTANCE(player, combat_damage_attacker_cards[s.damage_index]).state |= 0x200;
        }
      }

      if (combat_damage_attacker_count == 0 && ai_blocker_count == 0)
      {
        continue;
      }

      if (ai_blocker_count > 1)
      {
        if ((s.blocker_has_trample == 0 && player == other_player) ||
            (s.blocker_has_trample != 0 && player == active_player))
        {
          s.assigning_player = other_player;
        }
        else
        {
          s.assigning_player = active_player;
        }

        if ((g_duel_network_flags & 2) == 0 &&
            (g_duel_ai_mode_state == 1 || s.assigning_player == other_player))
        {
          s.attacker_score_bounds[0] = 0x7fffffff;
          s.attacker_score_bounds[1] = 0xffff8001;
          clear_combat_damage_assignment_matrix();
          for (s.loop_index = 0; s.loop_index < 16; s.loop_index = s.loop_index + 1)
          {
            s.attacker_damage_ids[s.loop_index] = -1;
          }
          assign_attacker_combat_damage(player, 0, s.blocker_has_trample, (int)s.attacker_damage_ids, s.damage_step, 0,
                                        s.attacker_score_bounds, s.attacker_score_bounds + 1);
          assign_attacker_combat_damage(player, 0, s.blocker_has_trample, (int)s.attacker_damage_ids, s.damage_step, 1,
                                        s.attacker_score_bounds, s.attacker_score_bounds + 1);
        }
        else
        {
          for (s.damage_index = 0; s.damage_index < combat_damage_attacker_count; s.damage_index = s.damage_index + 1)
          {
            for (s.loop_index = 0; s.loop_index < 16; s.loop_index = s.loop_index + 1)
            {
              s.attacker_damage_ids[s.loop_index] = -1;
            }
            s.attacker_damage_remaining = combat_damage_attacker_damage[s.damage_index];

            while (s.attacker_damage_remaining != 0)
            {
              load_text(global_ui_strings_filename, s_PROMPT_RESOLVECOMBAT_0057dd64);
              if (g_duel_interface_options.layout != 2)
              {
                if (combat_damage_attacker_abilities[s.damage_index] & 0x80)
                {
                  sprintf(unk_00748770, text_lines[1],
                          get_displayed_card_name(player, combat_damage_attacker_cards[s.damage_index]),
                          s.attacker_damage_remaining);
                }
                else
                {
                  sprintf(unk_00748770, text_lines[0],
                          get_displayed_card_name(player, combat_damage_attacker_cards[s.damage_index]),
                          s.attacker_damage_remaining);
                }
              }
              else if (combat_damage_attacker_abilities[s.damage_index] & 0x80)
              {
                sprintf(unk_00748770, text_lines[3], s.attacker_damage_remaining);
              }
              else
              {
                sprintf(unk_00748770, text_lines[2], s.attacker_damage_remaining);
              }

              highlight_combat_damage_attacker(player, combat_damage_attacker_cards[s.damage_index], 1);
              s.target_is_valid = 0;
              while (s.target_is_valid == 0)
              {
                C_real_select_target(s.assigning_player, s.defending_player, s.defending_player, 0x200, 2, 0, 0, 0, 0, 0,
                                     -1, -1, -1, -1, 0, 0x10, 0, unk_00748770, 0, &s.selected_target);
                for (s.loop_index = 0; s.loop_index < ai_blocker_count; s.loop_index = s.loop_index + 1)
                {
                  if (ai_blocker_cards[s.loop_index] == s.selected_target.card)
                  {
                    s.target_is_valid = 1;
                  }
                }
                if (s.target_is_valid == 0 && g_duel_ai_mode_state != 1)
                {
                  set_duel_prompt_text(text_lines[4]);
                  Sleep(1500);
                  set_duel_prompt_text(DAT_0057dd7c);
                }
                if (s.target_is_valid == 1 && has_damage_card_targeting(s.selected_target.player, s.selected_target.card) != 0)
                {
                  s.target_is_valid = 0;
                  if (g_duel_ai_mode_state != 1)
                  {
                    set_duel_prompt_text(text_lines[5]);
                    Sleep(1500);
                    set_duel_prompt_text(DAT_0057dd80);
                  }
                }
              }

              strcpy(unk_00748770, DAT_0057dd84);
              highlight_combat_damage_attacker(player, combat_damage_attacker_cards[s.damage_index], 0);
              if (s.selected_target.player != -1 && s.selected_target.card != -1 && s.selected_target.card != -2)
              {
                for (s.loop_index = 0; s.loop_index < ai_blocker_count; s.loop_index = s.loop_index + 1)
                {
                  if (ai_blocker_cards[s.loop_index] == s.selected_target.card)
                  {
                    if (unk_00715fb0 != 0)
                    {
                      s.damage_to_assign = s.attacker_damage_remaining;
                    }
                    else
                    {
                      s.damage_to_assign = 1;
                    }

                    if (s.attacker_damage_ids[s.loop_index] == -1)
                    {
                      s.damage_effect_card = damage_creature(s.selected_target.player, s.selected_target.card,
                                                             s.damage_to_assign,
                                                             player, combat_damage_attacker_cards[s.damage_index]);
                      s.attacker_damage_ids[s.loop_index] = s.damage_effect_card;
                      if (s.damage_effect_card != -1)
                      {
                        PLAYER_CARD_INSTANCE(player, s.damage_effect_card).token_status |= COMBAT_DAMAGE_FLAG_COMBAT;
                        if ((combat_damage_attacker_abilities[s.damage_index] & 0x80) != 0)
                        {
                          PLAYER_CARD_INSTANCE(player, s.damage_effect_card).token_status |= COMBAT_DAMAGE_FLAG_TRAMPLE;
                        }
                        if (s.damage_step == 0)
                        {
                          PLAYER_CARD_INSTANCE(player, s.damage_effect_card).token_status |= COMBAT_DAMAGE_FLAG_FIRST_STRIKE;
                        }
                      }
                    }
                    else
                    {
                      PLAYER_CARD_INSTANCE(player, s.attacker_damage_ids[s.loop_index]).info_slot += s.damage_to_assign;
                    }
                    s.attacker_damage_remaining -= s.damage_to_assign;
                  }
                }
              }
            }
          }
        }
      }
      else if (ai_blocker_count == 1)
      {
        for (s.damage_index = 0; s.damage_index < combat_damage_attacker_count; s.damage_index = s.damage_index + 1)
        {
          if (FUN_00515ee6(s.damage_step, combat_damage_attacker_abilities[s.damage_index]) != 0)
          {
            s.damage_effect_card = damage_creature(s.defending_player, ai_blocker_cards[0],
                                                   combat_damage_attacker_damage[s.damage_index],
                                                   player, combat_damage_attacker_cards[s.damage_index]);
            s.attacker_damage_ids[0] = s.damage_effect_card;
            if (s.damage_effect_card != -1)
            {
              PLAYER_CARD_INSTANCE(player, s.damage_effect_card).token_status |= COMBAT_DAMAGE_FLAG_COMBAT;
              if ((combat_damage_attacker_abilities[s.damage_index] & 0x80) != 0)
              {
                PLAYER_CARD_INSTANCE(player, s.damage_effect_card).token_status |= COMBAT_DAMAGE_FLAG_TRAMPLE;
              }
              if (s.damage_step == 0)
              {
                PLAYER_CARD_INSTANCE(player, s.damage_effect_card).token_status |= COMBAT_DAMAGE_FLAG_FIRST_STRIKE;
              }
            }
          }
        }
      }
      else
      {
        for (s.damage_index = 0; s.damage_index < combat_damage_attacker_count; s.damage_index = s.damage_index + 1)
        {
          if (FUN_00515ee6(s.damage_step, combat_damage_attacker_abilities[s.damage_index]) != 0 &&
              ((PLAYER_CARD_INSTANCE(player, combat_damage_attacker_cards[s.damage_index]).state & STATE_ISBLOCKED) == 0 ||
               (combat_damage_attacker_abilities[s.damage_index] & 0x80) != 0))
          {
            damage_player(s.defending_player, combat_damage_attacker_damage[s.damage_index],
                          player, combat_damage_attacker_cards[s.damage_index]);
          }
        }
      }

      if ((battlefield_extra_ability_flags & 4) != 0 &&
          s.defending_player == other_player &&
          (g_duel_network_flags & 2) == 0)
      {
        s.trample_damage = 0;
        s.blocker_score[2] = 0;
        if (combat_damage_attacker_count > 1)
        {
          for (s.damage_index = 0; s.damage_index < ai_blocker_count; s.damage_index = s.damage_index + 1)
          {
            if ((PLAYER_CARD_INSTANCE(s.defending_player, ai_blocker_cards[s.damage_index]).token_status & COMBAT_DAMAGE_FLAG_CANCELABLE) != 0 &&
                (global_cards_data[PLAYER_CARD_INSTANCE(s.defending_player, ai_blocker_cards[s.damage_index]).internal_card_id].code_pointer != FUN_00481e25 ||
                 PLAYER_CARD_INSTANCE(s.defending_player, ai_blocker_cards[s.damage_index]).info_slot >= 2))
            {
              adjust_two_headed_giant_damage(s.defending_player, ai_blocker_cards[s.damage_index], combat_damage_blocker_damage[s.damage_index]);
              combat_damage_blocker_damage[s.damage_index] = 0;
            }
          }
        }
        else
        {
          for (s.damage_index = 0; s.damage_index < ai_blocker_count; s.damage_index = s.damage_index + 1)
          {
            if (PLAYER_CARD_INSTANCE(s.defending_player, ai_blocker_cards[s.damage_index]).token_status & COMBAT_DAMAGE_FLAG_CANCELABLE)
            {
              s.trample_damage += combat_damage_blocker_damage[s.damage_index];
            }
            else
            {
              s.blocker_score[2] += combat_damage_blocker_damage[s.damage_index];
            }
          }

          for (s.damage_index = 0; s.damage_index < ai_blocker_count; s.damage_index = s.damage_index + 1)
          {
            if ((PLAYER_CARD_INSTANCE(s.defending_player, ai_blocker_cards[s.damage_index]).token_status & COMBAT_DAMAGE_FLAG_CANCELABLE) != 0)
            {
              s.blocker_score[3] = combat_damage_blocker_damage[s.damage_index];
              if (PLAYER_CARD_INSTANCE(player, combat_damage_attacker_cards[0]).toughness <= s.blocker_score[2] + s.trample_damage)
              {
                while (s.blocker_score[3] != 0 &&
                       s.blocker_score[2] < PLAYER_CARD_INSTANCE(player, combat_damage_attacker_cards[0]).toughness)
                {
                  s.blocker_score[2] += 1;
                  s.trample_damage -= 1;
                  s.blocker_score[3] -= 1;
                }
              }
              if (s.blocker_score[3] != 0 &&
                  (global_cards_data[PLAYER_CARD_INSTANCE(s.defending_player, ai_blocker_cards[s.damage_index]).internal_card_id].code_pointer != FUN_00481e25 ||
                   PLAYER_CARD_INSTANCE(s.defending_player, ai_blocker_cards[s.damage_index]).info_slot >= 2))
              {
                adjust_two_headed_giant_damage(s.defending_player, ai_blocker_cards[s.damage_index], s.blocker_score[3]);
                combat_damage_blocker_damage[s.damage_index] -= s.blocker_score[3];
              }
            }
          }
        }
      }

      if (combat_damage_attacker_count > 1 ||
          ((battlefield_extra_ability_flags & 4) &&
           (s.defending_player == active_player || (g_duel_network_flags & 2))))
      {
        if ((s.multiple_attackers == 0 && player == active_player) ||
            (s.multiple_attackers != 0 && player == other_player))
        {
          s.assigning_player = other_player;
        }
        else
        {
          s.assigning_player = active_player;
        }

        if ((g_duel_network_flags & 2) == 0 &&
            (g_duel_ai_mode_state == 1 || s.assigning_player == other_player))
        {
          s.blocker_score[0] = 0x7fffffff;
          s.blocker_score[1] = -1;
          clear_combat_damage_assignment_matrix();
          for (s.loop_index = 0; s.loop_index < 16; s.loop_index = s.loop_index + 1)
          {
            s.blocker_damage_ids[s.loop_index] = -1;
          }
          assign_blocker_combat_damage(player, 0, s.multiple_attackers, (int)s.blocker_damage_ids, s.damage_step, 0, s.blocker_score, s.blocker_score + 1);
          assign_blocker_combat_damage(player, 0, s.multiple_attackers, (int)s.blocker_damage_ids, s.damage_step, 1, s.blocker_score, s.blocker_score + 1);
        }
        else
        {
          for (s.damage_index = 0; s.damage_index < ai_blocker_count; s.damage_index = s.damage_index + 1)
          {
            for (s.loop_index = 0; s.loop_index < 16; s.loop_index = s.loop_index + 1)
            {
              s.blocker_damage_ids[s.loop_index] = -1;
            }
            s.blocker_damage_remaining = combat_damage_blocker_damage[s.damage_index];
            s.canceled_assignment = 0;

            while (s.blocker_damage_remaining != 0 && s.canceled_assignment == 0)
            {
              load_text(global_ui_strings_filename, s_PROMPT_RESOLVECOMBAT_0057dd88);
              if (g_duel_interface_options.layout != 2)
              {
                sprintf(unk_00748770, text_lines[6],
                        get_displayed_card_name(s.defending_player, ai_blocker_cards[s.damage_index]), s.blocker_damage_remaining);
              }
              else
              {
                sprintf(unk_00748770, text_lines[7], s.blocker_damage_remaining);
              }

              highlight_combat_damage_blocker(s.defending_player, ai_blocker_cards[s.damage_index], 1);
              s.target_is_valid = 0;
              if (combat_damage_attacker_count == 1 &&
                  ((PLAYER_CARD_INSTANCE(s.defending_player, ai_blocker_cards[s.damage_index]).token_status & COMBAT_DAMAGE_FLAG_CANCELABLE) == 0 ||
                   (global_cards_data[PLAYER_CARD_INSTANCE(s.defending_player, ai_blocker_cards[s.damage_index]).internal_card_id].code_pointer == FUN_00481e25 &&
                    PLAYER_CARD_INSTANCE(s.defending_player, ai_blocker_cards[s.damage_index]).info_slot < 2)))
              {
                s.target_is_valid = 1;
                unk_00715fb0 = 0;
                s.selected_target.player = player;
                s.selected_target.card = combat_damage_attacker_cards[0];
              }

              while (s.target_is_valid == 0)
              {
                C_real_select_target(s.assigning_player, player, player, 0x200, 2, 0, 0, 0, 0, 0,
                                     -1, -1, -1, -1, 0, 2, 0, unk_00748770,
                                     (PLAYER_CARD_INSTANCE(s.defending_player, ai_blocker_cards[s.damage_index]).token_status &
                                      COMBAT_DAMAGE_FLAG_CANCELABLE) >>
                                         26,
                                     &s.selected_target);
                if (s.selected_target.card == -2)
                {
                  s.target_is_valid = 1;
                  s.canceled_assignment = 1;
                  adjust_two_headed_giant_damage(s.defending_player, ai_blocker_cards[s.damage_index],
                                                 s.blocker_damage_remaining);
                }
                else
                {
                  for (s.loop_index = 0; s.loop_index < combat_damage_attacker_count; s.loop_index = s.loop_index + 1)
                  {
                    if (combat_damage_attacker_cards[s.loop_index] == s.selected_target.card)
                    {
                      s.target_is_valid = 1;
                    }
                  }
                  if (s.target_is_valid == 0 && g_duel_ai_mode_state != 1)
                  {
                    set_duel_prompt_text(text_lines[8]);
                    Sleep(1500);
                    set_duel_prompt_text(DAT_0057dda0);
                  }
                  if (s.target_is_valid == 1 && has_damage_card_targeting(s.selected_target.player, s.selected_target.card) != 0)
                  {
                    s.target_is_valid = 0;
                    if (g_duel_ai_mode_state != 1)
                    {
                      set_duel_prompt_text(text_lines[9]);
                      Sleep(1500);
                      set_duel_prompt_text(DAT_0057dda4);
                    }
                  }
                }
              }

              strcpy(unk_00748770, DAT_0057dda8);
              highlight_combat_damage_blocker(s.defending_player, ai_blocker_cards[s.damage_index], 0);
              if (s.selected_target.player != -1 && s.selected_target.card != -1 && s.selected_target.card != -2)
              {
                for (s.loop_index = 0; s.loop_index < combat_damage_attacker_count; s.loop_index = s.loop_index + 1)
                {
                  if (combat_damage_attacker_cards[s.loop_index] == s.selected_target.card)
                  {
                    if (unk_00715fb0 != 0 ||
                        (global_cards_data[PLAYER_CARD_INSTANCE(s.defending_player, ai_blocker_cards[s.damage_index]).internal_card_id].code_pointer == FUN_00481e25 &&
                         PLAYER_CARD_INSTANCE(s.defending_player, ai_blocker_cards[s.damage_index]).info_slot < 2))
                    {
                      s.damage_to_assign = s.blocker_damage_remaining;
                    }
                    else
                    {
                      s.damage_to_assign = 1;
                    }
                    if (s.blocker_damage_ids[s.loop_index] == -1)
                    {
                      s.damage_effect_card = damage_creature(player, s.selected_target.card,
                                                             s.damage_to_assign,
                                                             s.defending_player, ai_blocker_cards[s.damage_index]);
                      s.blocker_damage_ids[s.loop_index] = s.damage_effect_card;
                      if (s.damage_effect_card != -1)
                      {
                        PLAYER_CARD_INSTANCE(s.defending_player, s.damage_effect_card).token_status |= COMBAT_DAMAGE_FLAG_COMBAT;
                        if (s.damage_step == 0)
                        {
                          PLAYER_CARD_INSTANCE(s.defending_player, s.damage_effect_card).token_status |= COMBAT_DAMAGE_FLAG_FIRST_STRIKE;
                        }
                      }
                    }
                    else
                    {
                      PLAYER_CARD_INSTANCE(s.defending_player, s.blocker_damage_ids[s.loop_index]).info_slot += s.damage_to_assign;
                    }
                    s.blocker_damage_remaining -= s.damage_to_assign;
                  }
                }
              }
            }
          }

          if ((PLAYER_CARD_INSTANCE(s.defending_player, ai_blocker_cards[s.damage_index - 1]).token_status & COMBAT_DAMAGE_FLAG_CANCELABLE) != 0 &&
              s.selected_target.card != -2 &&
              (global_cards_data[PLAYER_CARD_INSTANCE(s.defending_player, ai_blocker_cards[s.damage_index - 1]).internal_card_id].code_pointer != FUN_00481e25 ||
               PLAYER_CARD_INSTANCE(s.defending_player, ai_blocker_cards[s.damage_index - 1]).info_slot >= 2) &&
              ((s.damage_step == 0 && (PLAYER_CARD_INSTANCE(s.defending_player, ai_blocker_cards[s.damage_index - 1]).regen_status & 0x100) != 0) ||
               (s.damage_step == 1 && (PLAYER_CARD_INSTANCE(s.defending_player, ai_blocker_cards[s.damage_index - 1]).regen_status & 0x100) == 0)))
          {
            adjust_two_headed_giant_damage(s.defending_player, ai_blocker_cards[s.damage_index - 1],
                                           s.blocker_damage_remaining);
          }
        }
      }
      else if (combat_damage_attacker_count == 1)
      {
        for (s.damage_index = 0; s.damage_index < ai_blocker_count; s.damage_index = s.damage_index + 1)
        {
          if (FUN_00515ee6(s.damage_step, combat_damage_blocker_abilities[s.damage_index]) != 0)
          {
            s.damage_effect_card = damage_creature(player, combat_damage_attacker_cards[0],
                                                   combat_damage_blocker_damage[s.damage_index],
                                                   s.defending_player, ai_blocker_cards[s.damage_index]);
            s.blocker_damage_ids[0] = s.damage_effect_card;
            if (s.damage_effect_card != -1)
            {
              PLAYER_CARD_INSTANCE(s.defending_player, s.damage_effect_card).token_status |= COMBAT_DAMAGE_FLAG_COMBAT;
              if (s.damage_step == 0)
              {
                PLAYER_CARD_INSTANCE(s.defending_player, s.damage_effect_card).token_status |= COMBAT_DAMAGE_FLAG_FIRST_STRIKE;
              }
            }
          }
        }
      }
    }

    ai_blocker_count = 0;
    for (s.scan_card = 0; s.scan_card < active_cards_count[s.defending_player]; s.scan_card = s.scan_card + 1)
    {
      if (PLAYER_CARD_INSTANCE(s.defending_player, s.scan_card).internal_card_id != -1 &&
          PLAYER_CARD_INSTANCE(s.defending_player, s.scan_card).blocking != -1)
      {
        ai_blocker_cards[ai_blocker_count] = s.scan_card;
        combat_damage_blocker_toughness[ai_blocker_count] =
            C_get_abilities(s.defending_player, s.scan_card, 0x33, s.card) -
            PLAYER_CARD_INSTANCE(s.defending_player, s.scan_card).damage_on_card;
        if (has_damage_card_targeting(s.defending_player, s.scan_card) != 0)
        {
          combat_damage_blocker_toughness[ai_blocker_count] = 0;
        }
        ai_blocker_count = ai_blocker_count + 1;
        if (ai_blocker_count == 16)
        {
          break;
        }
      }
    }

    if (g_duel_ai_mode_state != 1)
    {
      phase_was_skipped = 0;
    }
    phase_response_window_open = 0;
    if (g_duel_ai_mode_state != 1)
    {
      phase_stop_suppressed = 0;
    }

    process_damage_prevention(player);
    if (phase_was_skipped == 0 && (s.stop_prompt_result = prompt_stop_phase_anyway(current_phase)) != 0 && g_duel_ai_mode_state != 1)
    {
      phase_was_skipped = 1;
    }

    for (s.loop_index = 0; s.loop_index < ai_blocker_count; s.loop_index = s.loop_index + 1)
    {
      s.scan_card = ai_blocker_cards[s.loop_index];
      s.blocker_damage_remaining = combat_damage_blocker_toughness[s.loop_index];

      for (s.card = 0; s.card < active_cards_count[player]; s.card = s.card + 1)
      {
        if (PLAYER_CARD_INSTANCE(player, s.card).original_internal_card_id == unk_009266a4 &&
            PLAYER_CARD_INSTANCE(player, s.card).damage_target_player == s.defending_player &&
            PLAYER_CARD_INSTANCE(player, s.card).damage_target_card == s.scan_card &&
            ((s.damage_step == 0 && (PLAYER_CARD_INSTANCE(player, s.card).token_status & COMBAT_DAMAGE_FLAG_FIRST_STRIKE) != 0) ||
             (s.damage_step == 1 && (PLAYER_CARD_INSTANCE(player, s.card).token_status & COMBAT_DAMAGE_FLAG_FIRST_STRIKE) == 0)) &&
            (PLAYER_CARD_INSTANCE(player, s.card).token_status & COMBAT_DAMAGE_FLAG_COMBAT) != 0 &&
            (PLAYER_CARD_INSTANCE(player, s.card).token_status & COMBAT_DAMAGE_FLAG_TRAMPLE) == 0)
        {
          s.blocker_damage_remaining -= PLAYER_CARD_INSTANCE(player, s.card).info_slot;
        }
      }

      for (s.card = 0; s.card < active_cards_count[player]; s.card = s.card + 1)
      {
        if (PLAYER_CARD_INSTANCE(player, s.card).original_internal_card_id == unk_009266a4 &&
            PLAYER_CARD_INSTANCE(player, s.card).damage_target_player == s.defending_player &&
            PLAYER_CARD_INSTANCE(player, s.card).damage_target_card == s.scan_card &&
            ((s.damage_step == 0 && (PLAYER_CARD_INSTANCE(player, s.card).token_status & COMBAT_DAMAGE_FLAG_FIRST_STRIKE) != 0) ||
             (s.damage_step == 1 && (PLAYER_CARD_INSTANCE(player, s.card).token_status & COMBAT_DAMAGE_FLAG_FIRST_STRIKE) == 0)) &&
            (PLAYER_CARD_INSTANCE(player, s.card).token_status & COMBAT_DAMAGE_FLAG_TRAMPLE) != 0 &&
            s.blocker_damage_remaining - (PLAYER_CARD_INSTANCE(player, s.card).unknown0x37 +
                                          PLAYER_CARD_INSTANCE(player, s.card).info_slot) <
                0)
        {
          damage_player(s.defending_player,
                        (-(s.blocker_damage_remaining - (PLAYER_CARD_INSTANCE(player, s.card).unknown0x37 +
                                                         PLAYER_CARD_INSTANCE(player, s.card).info_slot)) <
                         (PLAYER_CARD_INSTANCE(player, s.card).unknown0x37 + PLAYER_CARD_INSTANCE(player, s.card).info_slot))
                            ? -(s.blocker_damage_remaining - (PLAYER_CARD_INSTANCE(player, s.card).unknown0x37 +
                                                              PLAYER_CARD_INSTANCE(player, s.card).info_slot))
                            : (PLAYER_CARD_INSTANCE(player, s.card).unknown0x37 + PLAYER_CARD_INSTANCE(player, s.card).info_slot),
                        (int)(char)PLAYER_CARD_INSTANCE(player, s.card).damage_source_player,
                        PLAYER_CARD_INSTANCE(player, s.card).damage_source_card);
          s.blocker_damage_remaining -= PLAYER_CARD_INSTANCE(player, s.card).unknown0x37 +
                                        PLAYER_CARD_INSTANCE(player, s.card).info_slot;
        }
      }
    }

    regenerate_or_graveyard_triggers();
    process_damage_prevention(player);
    if ((battlefield_extra_ability_flags & 1) != 0)
    {
      battlefield_extra_ability_flags &= ~1;
      pending_killed_card_handler = process_killed_card;
      regenerate_or_graveyard_triggers();
      process_damage_prevention(player);
    }

    phase_response_window_open = 0;
    if (g_duel_ai_mode_state != 1)
    {
      phase_stop_suppressed = 0;
    }
    if (phase_was_skipped == 0)
    {
      prompt_stop_phase_anyway(current_phase);
    }
  }
}
