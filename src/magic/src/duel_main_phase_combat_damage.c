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
char *FUN_0044a3bf(int player, int card);
int FUN_004b0c12(int player, int card);
int FUN_00515ee6(int damage_step, int abilities);
int FUN_00515bba(int player, int card, short amount);
void FUN_00516063(void);
void FUN_005160c1(int player, int index, int has_trample, int damage_ids, int damage_step, int mode, int *best_low, int *best_high);
void FUN_00516736(int player, int index, int multiple_attackers, int damage_ids, int damage_step, int mode, int *best_low, int *best_high);
void FUN_00516db1(int player, int card, int highlight);
void FUN_00516e2f(int player, int card, int highlight);
int FUN_0051724e(int player, int card);

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
int FUN_00515bba(int player, int card, short amount)
{
  card_function_pointer code_pointer;
  int damage_source_player;
  int damage_source_card;
  int found_trampler;
  int index;

  found_trampler = 0;
  code_pointer = global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].code_pointer;
  damage_source_player = (int)(char)PLAYER_CARD_INSTANCE(player, card).damage_source_player;
  damage_source_card = PLAYER_CARD_INSTANCE(player, card).damage_source_card;

  if (code_pointer == card_two_headed_giant_of_foriys || code_pointer == FUN_00481e25)
  {
    found_trampler = 1;
  }

  index = 0;
  while (index < active_cards_count[player] && found_trampler == 0)
  {
    if (PLAYER_CARD_INSTANCE(player, index).internal_card_id == unk_008a8df0 &&
        PLAYER_CARD_INSTANCE(player, index).damage_target_player == player &&
        PLAYER_CARD_INSTANCE(player, index).damage_target_card == card)
    {
      found_trampler = 1;
    }
    index = index + 1;
  }

  if (found_trampler != 0)
  {
    if (code_pointer == FUN_00481e25)
    {
      for (index = 0; index < active_cards_count[player]; index = index + 1)
      {
        if (index != card &&
            global_cards_data[PLAYER_CARD_INSTANCE(player, index).internal_card_id].code_pointer == FUN_00481e25 &&
            PLAYER_CARD_INSTANCE(player, index).damage_source_player == damage_source_player &&
            PLAYER_CARD_INSTANCE(player, index).damage_source_card == damage_source_card)
        {
          PLAYER_CARD_INSTANCE(player, index).power = amount;
          PLAYER_CARD_INSTANCE(player, index).info_slot -= 1;
        }
      }
    }
    else
    {
      for (index = 0; index < active_cards_count[player]; index = index + 1)
      {
        if (global_cards_data[PLAYER_CARD_INSTANCE(player, index).internal_card_id].code_pointer == FUN_00481e25 &&
            PLAYER_CARD_INSTANCE(player, index).damage_source_player == player &&
            PLAYER_CARD_INSTANCE(player, index).damage_source_card == card)
        {
          PLAYER_CARD_INSTANCE(player, index).power = amount;
          PLAYER_CARD_INSTANCE(player, index).info_slot -= 1;
        }
      }
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x00515ee6
int FUN_00515ee6(int damage_step, int abilities)
{
  int result;

  if (damage_step == 0 && (abilities & 0x100) != 0)
  {
    result = 1;
  }
  else if (damage_step == 0 || (abilities & 0x100) != 0)
  {
    result = 0;
  }
  else
  {
    result = 1;
  }

  return result;
}

// FUNCTION: MAGIC 0x00516063
void FUN_00516063(void)
{
  int blocker_index;
  int attacker_index;

  for (attacker_index = 0; attacker_index < 16; attacker_index = attacker_index + 1)
  {
    for (blocker_index = 0; blocker_index < 16; blocker_index = blocker_index + 1)
    {
      combat_damage_assignment_matrix[attacker_index][blocker_index] = 0;
    }
  }
}

// FUNCTION: MAGIC 0x005160c1
void FUN_005160c1(int player, int attacker_index, int has_trample, int damage_ids, int damage_step, int mode, int *best_low, int *best_high)
{
  int defending_player;
  int attacker_damage;
  int assignment_base;
  int combination_count;
  int total_assigned;
  int assignment_code;
  int assignment_index;
  int blocker_index;
  int damage_to_assign;
  int trample_assigned;
  int life_after_trample;
  int score;
  int damage_card;

  defending_player = 1 - player;
  if (attacker_index < combat_damage_attacker_count)
  {
    attacker_damage = combat_damage_attacker_damage[attacker_index];
    assignment_base = attacker_damage + 1;
    combination_count = 1;
    for (assignment_index = 0; assignment_index < ai_blocker_count; assignment_index = assignment_index + 1)
    {
      combination_count *= assignment_base;
    }

    if (mode == 0)
    {
      if ((g_shandalar_difficulty + 1) * 0x100 < combination_count)
      {
        assignment_code = 0;
        total_assigned = attacker_damage;
        assignment_index = ai_blocker_count;
        while (assignment_index = assignment_index - 1, 0 < assignment_index)
        {
          if (combat_damage_blocker_toughness[assignment_index] < total_assigned &&
              (((unsigned char *)&combat_damage_blocker_abilities[assignment_index])[1] & 2) == 0)
          {
            assignment_code += combat_damage_blocker_toughness[assignment_index];
            combat_damage_assignment_matrix[attacker_index][assignment_index] =
                combat_damage_blocker_toughness[assignment_index];
            total_assigned -= combat_damage_blocker_toughness[assignment_index];
          }
          assignment_code *= assignment_base;
        }
        if (total_assigned != 0)
        {
          assignment_code += total_assigned;
        }
        combat_damage_current_assignment_codes[attacker_index] = assignment_code;
        FUN_005160c1(player, attacker_index + 1, has_trample, damage_ids, damage_step, 0, best_low, best_high);
      }
      else
      {
        if (4999999 < combination_count)
        {
          combination_count = 5000000;
        }
        for (assignment_index = 0; assignment_index < combination_count; assignment_index = assignment_index + 1)
        {
          assignment_code = assignment_index;
          memset(&combat_damage_assignment_matrix[attacker_index][0], 0, 0x40);
          blocker_index = 0;
          for (total_assigned = attacker_damage; blocker_index < ai_blocker_count && 0 < total_assigned; total_assigned = total_assigned - damage_to_assign)
          {
            damage_to_assign = assignment_code % assignment_base;
            combat_damage_assignment_matrix[attacker_index][blocker_index] = damage_to_assign;
            assignment_code /= assignment_base;
            blocker_index = blocker_index + 1;
          }
          if (total_assigned == 0)
          {
            combat_damage_current_assignment_codes[attacker_index] = assignment_index;
            FUN_005160c1(player, attacker_index + 1, has_trample, damage_ids, damage_step, 0, best_low, best_high);
          }
        }
      }
    }
    else
    {
      assignment_code = combat_damage_best_assignment_codes[attacker_index];
      for (blocker_index = 0; blocker_index < ai_blocker_count; blocker_index = blocker_index + 1)
      {
        damage_to_assign = assignment_code % assignment_base;
        if (FUN_0051724e(defending_player, ai_blocker_cards[blocker_index]) != 0)
        {
          damage_to_assign = 0;
        }
        if (damage_to_assign != 0)
        {
          damage_card = damage_creature(defending_player, ai_blocker_cards[blocker_index], damage_to_assign,
                                        player, combat_damage_attacker_cards[attacker_index]);
          *(int *)(damage_ids + blocker_index * 4) = damage_card;
          if (damage_card != -1)
          {
            PLAYER_CARD_INSTANCE(player, damage_card).token_status |= COMBAT_DAMAGE_FLAG_COMBAT;
            if (((unsigned char)combat_damage_attacker_abilities[attacker_index] & 0x80) != 0)
            {
              PLAYER_CARD_INSTANCE(player, damage_card).token_status |= COMBAT_DAMAGE_FLAG_TRAMPLE;
            }
            if (damage_step == 0)
            {
              PLAYER_CARD_INSTANCE(player, damage_card).token_status |= COMBAT_DAMAGE_FLAG_FIRST_STRIKE;
            }
          }
        }
        combat_damage_assignment_matrix[attacker_index][blocker_index] = damage_to_assign;
        assignment_code /= assignment_base;
      }
      FUN_005160c1(player, attacker_index + 1, has_trample, damage_ids, damage_step, mode, best_low, best_high);
    }
  }
  else
  {
    score = 0;
    life_after_trample = life[defending_player];
    for (blocker_index = 0; blocker_index < ai_blocker_count; blocker_index = blocker_index + 1)
    {
      trample_assigned = 0;
      total_assigned = 0;
      for (assignment_index = 0; assignment_index < combat_damage_attacker_count; assignment_index = assignment_index + 1)
      {
        total_assigned += combat_damage_assignment_matrix[assignment_index][blocker_index];
        if (((unsigned char)combat_damage_attacker_abilities[assignment_index] & 0x80) != 0)
        {
          trample_assigned += combat_damage_assignment_matrix[assignment_index][blocker_index];
        }
      }
      if (total_assigned < combat_damage_blocker_toughness[blocker_index])
      {
        score += total_assigned * 2;
      }
      else
      {
        score += PLAYER_CARD_INSTANCE(defending_player, ai_blocker_cards[blocker_index]).attack_rating +
                 (total_assigned - combat_damage_blocker_toughness[blocker_index]);
        damage_card = ClampIntToRange(total_assigned - combat_damage_blocker_toughness[blocker_index], 0, trample_assigned);
        life_after_trample -= damage_card;
      }
    }

    if (life_after_trample < 1)
    {
      life_after_trample = life_after_trample * -0x18 + 999;
    }
    else
    {
      life_after_trample = ((life[defending_player] - life_after_trample) * 0x30) / life_after_trample;
    }
    score += life_after_trample;

    if (has_trample == 0)
    {
      if (*best_high < score)
      {
        *best_high = score;
        for (assignment_index = 0; assignment_index < 16; assignment_index = assignment_index + 1)
        {
          combat_damage_best_assignment_codes[assignment_index] = combat_damage_current_assignment_codes[assignment_index];
        }
      }
    }
    else if (score < *best_low)
    {
      *best_low = score;
      for (assignment_index = 0; assignment_index < 16; assignment_index = assignment_index + 1)
      {
        combat_damage_best_assignment_codes[assignment_index] = combat_damage_current_assignment_codes[assignment_index];
      }
    }
  }
}

// FUNCTION: MAGIC 0x00516736
void FUN_00516736(int player, int blocker_index, int multiple_attackers, int damage_ids, int damage_step, int mode, int *best_low, int *best_high)
{
  int defending_player;
  int blocker_damage;
  int assignment_base;
  int combination_count;
  int total_assigned;
  int assignment_code;
  int attacker_index;
  int assignment_index;
  int damage_to_assign;
  int score;
  int damage_card;

  defending_player = 1 - player;
  if (blocker_index < ai_blocker_count)
  {
    if ((PLAYER_CARD_INSTANCE(defending_player, ai_blocker_cards[blocker_index]).state & 0x10) == 0)
    {
      blocker_damage = combat_damage_blocker_damage[blocker_index];
      assignment_base = blocker_damage + 1;
      combination_count = 1;
      for (assignment_index = 0; assignment_index < combat_damage_attacker_count; assignment_index = assignment_index + 1)
      {
        combination_count *= assignment_base;
      }

      if (mode == 0)
      {
        for (assignment_index = 0; assignment_index < combination_count; assignment_index = assignment_index + 1)
        {
          assignment_code = assignment_index;
          total_assigned = blocker_damage;
          for (attacker_index = 0; attacker_index < combat_damage_attacker_count; attacker_index = attacker_index + 1)
          {
            damage_to_assign = assignment_code % assignment_base;
            if (combination_count < 0x101 || damage_to_assign < 2 || total_assigned <= damage_to_assign)
            {
              combat_damage_assignment_matrix[blocker_index][attacker_index] = damage_to_assign;
              assignment_code /= assignment_base;
              total_assigned -= damage_to_assign;
            }
          }
          if (total_assigned == 0)
          {
            combat_damage_current_assignment_codes[blocker_index] = assignment_index;
            FUN_00516736(player, blocker_index + 1, multiple_attackers, damage_ids, damage_step, 0, best_low, best_high);
          }
        }
      }
      else
      {
        assignment_code = combat_damage_best_assignment_codes[blocker_index];
        for (attacker_index = 0; attacker_index < combat_damage_attacker_count; attacker_index = attacker_index + 1)
        {
          damage_to_assign = assignment_code % assignment_base;
          if (FUN_0051724e(defending_player, ai_blocker_cards[attacker_index]) != 0)
          {
            damage_to_assign = 0;
          }
          if (damage_to_assign != 0)
          {
            damage_card = damage_creature(player, combat_damage_attacker_cards[attacker_index], damage_to_assign,
                                          defending_player, ai_blocker_cards[blocker_index]);
            *(int *)(damage_ids + attacker_index * 4) = damage_card;
            if (damage_card != -1)
            {
              PLAYER_CARD_INSTANCE(defending_player, damage_card).token_status |= COMBAT_DAMAGE_FLAG_COMBAT;
              if (damage_step == 0)
              {
                PLAYER_CARD_INSTANCE(defending_player, damage_card).token_status |= COMBAT_DAMAGE_FLAG_FIRST_STRIKE;
              }
            }
          }
          assignment_code /= assignment_base;
        }
        FUN_00516736(player, blocker_index + 1, multiple_attackers, damage_ids, damage_step, mode, best_low, best_high);
      }
    }
    else
    {
      FUN_00516736(player, blocker_index + 1, multiple_attackers, damage_ids, damage_step, mode, best_low, best_high);
    }
  }
  else
  {
    score = 0;
    for (assignment_index = 0; assignment_index < combat_damage_attacker_count; assignment_index = assignment_index + 1)
    {
      total_assigned = 0;
      for (attacker_index = 0; attacker_index < ai_blocker_count; attacker_index = attacker_index + 1)
      {
        total_assigned += combat_damage_assignment_matrix[attacker_index][assignment_index];
      }
      if (total_assigned < combat_damage_attacker_toughness[assignment_index])
      {
        score += total_assigned * 2;
      }
      else
      {
        score += PLAYER_CARD_INSTANCE(player, combat_damage_attacker_cards[assignment_index]).attack_rating +
                 (total_assigned - combat_damage_attacker_toughness[assignment_index]);
      }
    }

    if (multiple_attackers == 0)
    {
      if (*best_high < score)
      {
        *best_high = score;
        for (assignment_index = 0; assignment_index < 16; assignment_index = assignment_index + 1)
        {
          combat_damage_best_assignment_codes[assignment_index] = combat_damage_current_assignment_codes[assignment_index];
        }
      }
    }
    else if (score < *best_low)
    {
      *best_low = score;
      for (assignment_index = 0; assignment_index < 16; assignment_index = assignment_index + 1)
      {
        combat_damage_best_assignment_codes[assignment_index] = combat_damage_current_assignment_codes[assignment_index];
      }
    }
  }
}

// FUNCTION: MAGIC 0x00516db1
void FUN_00516db1(int player, int card, int highlight)
{
  if (highlight == 0)
  {
    PLAYER_CARD_INSTANCE(player, card).token_status &= 0xfffdffff;
  }
  else
  {
    PLAYER_CARD_INSTANCE(player, card).token_status |= 0x20000;
  }
}

// FUNCTION: MAGIC 0x00516e2f
void FUN_00516e2f(int player, int card, int highlight)
{
  if (highlight == 0)
  {
    PLAYER_CARD_INSTANCE(player, card).token_status &= 0xfffdffff;
  }
  else
  {
    PLAYER_CARD_INSTANCE(player, card).token_status |= 0x20000;
  }
}

// FUNCTION: MAGIC 0x0051724e
int FUN_0051724e(int player, int card)
{
  int loop_player;
  int loop_card;

  loop_player = 0;
  do
  {
    if (1 < loop_player)
    {
      return 0;
    }

    for (loop_card = 0; loop_card < active_cards_count[loop_player]; loop_card = loop_card + 1)
    {
      if (PLAYER_CARD_INSTANCE(loop_player, loop_card).damage_target_player == player &&
          PLAYER_CARD_INSTANCE(loop_player, loop_card).damage_target_card == card &&
          global_cards_data[PLAYER_CARD_INSTANCE(loop_player, loop_card).internal_card_id].id == 0x285)
      {
        return 1;
      }
    }
    loop_player = loop_player + 1;
  } while (1);
}

// FUNCTION: MAGIC 0x005136ed
void resolve_combat_damage(int player)
{
  struct
  {
    int stack_padding;
    int blocker_score[4];
    int trample_damage;
    int scratch[6];
    target_t selected_target;
    int played_damage_sound;
    int damage_index;
    int attacker_damage_remaining;
    int blocker_has_trample;
    int blocker_damage_ids[16];
    int defending_player;
    int loop_index;
    int damage_step;
    int assigning_player;
    int card;
    int total_blocker_damage;
    int firststrike_attacker_damage;
    int attacker_damage_ids[16];
    int total_attacker_damage;
    int canceled_assignment;
    int damage_card;
    int valid_target;
  } s;

  s.defending_player = 1 - player;
  s.played_damage_sound = 0;
  s.damage_step = 0;

  while (s.damage_step < 2)
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
      if (PLAYER_CARD_INSTANCE(player, s.card).blocking == 0xff ||
          PLAYER_CARD_INSTANCE(player, s.card).blocking == s.card)
      {
        if (s.played_damage_sound == 0 && g_duel_ai_mode_state != 1)
        {
          play_sound_effect(0x14);
          s.played_damage_sound = 1;
        }

        s.firststrike_attacker_damage = 0;
        s.total_attacker_damage = 0;
        combat_damage_attacker_count = 0;
        s.scratch[4] = 0;

        for (s.scratch[5] = 0; s.scratch[5] < active_cards_count[player]; s.scratch[5] = s.scratch[5] + 1)
        {
          if (((s.scratch[5] == s.card ||
                PLAYER_CARD_INSTANCE(player, s.scratch[5]).blocking == s.card) &&
               PLAYER_CARD_INSTANCE(player, s.scratch[5]).internal_card_id != -1) &&
              ((PLAYER_CARD_INSTANCE(player, s.scratch[5]).state & 0x800006) == 6))
          {
            combat_damage_attacker_cards[combat_damage_attacker_count] = s.scratch[5];
            combat_damage_attacker_toughness[combat_damage_attacker_count] =
                C_get_abilities(player, s.scratch[5], 0x33, -1);
            combat_damage_attacker_abilities[combat_damage_attacker_count] =
                C_get_abilities(player, s.scratch[5], 0x34, -1);
            s.attacker_damage_remaining = 0;
            combat_damage_attacker_damage[combat_damage_attacker_count] = 0;

            if (FUN_00515ee6(s.damage_step, combat_damage_attacker_abilities[combat_damage_attacker_count]) != 0)
            {
              s.attacker_damage_remaining = C_get_abilities(player, s.scratch[5], 0x32, -1);
              if (s.attacker_damage_remaining < 0)
              {
                s.attacker_damage_remaining = 0;
              }
              combat_damage_attacker_damage[combat_damage_attacker_count] = s.attacker_damage_remaining;
              s.total_attacker_damage += s.attacker_damage_remaining;
              if (((unsigned char)combat_damage_attacker_abilities[combat_damage_attacker_count] & 0x80) != 0)
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
          s.scratch[4] = 1;
        }

        s.total_blocker_damage = 0;
        ai_blocker_count = 0;
        s.blocker_has_trample = 0;

        for (s.scratch[5] = 0; s.scratch[5] < active_cards_count[s.defending_player]; s.scratch[5] = s.scratch[5] + 1)
        {
          if (is_in_play(s.defending_player, s.scratch[5]) != 0 &&
              PLAYER_CARD_INSTANCE(s.defending_player, s.scratch[5]).blocking == s.card)
          {
            ai_blocker_cards[ai_blocker_count] = s.scratch[5];
            combat_damage_blocker_toughness[ai_blocker_count] =
                C_get_abilities(s.defending_player, s.scratch[5], 0x33, s.card) -
                PLAYER_CARD_INSTANCE(s.defending_player, s.scratch[5]).damage_on_card;
            combat_damage_blocker_abilities[ai_blocker_count] =
                C_get_abilities(s.defending_player, s.scratch[5], 0x34, -1);
            s.scratch[2] = 0;
            combat_damage_blocker_damage[ai_blocker_count] = 0;

            if ((PLAYER_CARD_INSTANCE(s.defending_player, s.scratch[5]).state & 0x10) == 0 &&
                FUN_00515ee6(s.damage_step, combat_damage_blocker_abilities[ai_blocker_count]) != 0)
            {
              s.scratch[2] = C_get_abilities(s.defending_player, s.scratch[5], 0x32, s.card);
              if (s.scratch[2] < 0)
              {
                s.scratch[2] = 0;
              }
              combat_damage_blocker_damage[ai_blocker_count] = s.scratch[2];
              s.total_blocker_damage += s.scratch[2];
            }

            if (((unsigned char)combat_damage_blocker_abilities[ai_blocker_count] & 0x40) != 0)
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

        if (combat_damage_attacker_count != 0 || ai_blocker_count != 0)
        {
          if (ai_blocker_count < 2)
          {
            if (ai_blocker_count == 1)
            {
              for (s.damage_index = 0; s.damage_index < combat_damage_attacker_count; s.damage_index = s.damage_index + 1)
              {
                if (FUN_00515ee6(s.damage_step, combat_damage_attacker_abilities[s.damage_index]) != 0)
                {
                  s.attacker_damage_ids[0] = damage_creature(s.defending_player, ai_blocker_cards[0],
                                                             combat_damage_attacker_damage[s.damage_index],
                                                             player, combat_damage_attacker_cards[s.damage_index]);
                  s.damage_card = s.attacker_damage_ids[0];
                  if (s.attacker_damage_ids[0] != -1)
                  {
                    PLAYER_CARD_INSTANCE(player, s.attacker_damage_ids[0]).token_status |= COMBAT_DAMAGE_FLAG_COMBAT;
                    if (((unsigned char)combat_damage_attacker_abilities[s.damage_index] & 0x80) != 0)
                    {
                      PLAYER_CARD_INSTANCE(player, s.attacker_damage_ids[0]).token_status |= COMBAT_DAMAGE_FLAG_TRAMPLE;
                    }
                    if (s.damage_step == 0)
                    {
                      PLAYER_CARD_INSTANCE(player, s.attacker_damage_ids[0]).token_status |= COMBAT_DAMAGE_FLAG_FIRST_STRIKE;
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
                    ((((unsigned char *)&PLAYER_CARD_INSTANCE(player, combat_damage_attacker_cards[s.damage_index]).state)[1] & 2) == 0 ||
                     ((unsigned char)combat_damage_attacker_abilities[s.damage_index] & 0x80) != 0))
                {
                  damage_player(s.defending_player, combat_damage_attacker_damage[s.damage_index],
                                player, combat_damage_attacker_cards[s.damage_index]);
                }
              }
            }
          }
          else
          {
            if ((s.blocker_has_trample == 0 && player == active_player) ||
                (s.blocker_has_trample != 0 && player == other_player))
            {
              s.assigning_player = active_player;
            }
            else
            {
              s.assigning_player = other_player;
            }

            if (((unsigned char)g_duel_network_flags & 2) == 0 &&
                (g_duel_ai_mode_state == 1 || s.assigning_player == active_player))
            {
              s.scratch[0] = 0x7fffffff;
              s.scratch[1] = 0xffff8001;
              FUN_00516063();
              for (s.loop_index = 0; s.loop_index < 16; s.loop_index = s.loop_index + 1)
              {
                s.attacker_damage_ids[s.loop_index] = -1;
              }
              FUN_005160c1(player, 0, s.blocker_has_trample, (int)s.attacker_damage_ids, s.damage_step, 0, s.scratch, s.scratch + 1);
              FUN_005160c1(player, 0, s.blocker_has_trample, (int)s.attacker_damage_ids, s.damage_step, 1, s.scratch, s.scratch + 1);
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
                  if (g_duel_interface_options.layout == 2)
                  {
                    if (((unsigned char)combat_damage_attacker_abilities[s.damage_index] & 0x80) == 0)
                    {
                      sprintf(unk_00748770, text_lines[2], s.attacker_damage_remaining);
                    }
                    else
                    {
                      sprintf(unk_00748770, text_lines[3], s.attacker_damage_remaining);
                    }
                  }
                  else if (((unsigned char)combat_damage_attacker_abilities[s.damage_index] & 0x80) == 0)
                  {
                    sprintf(unk_00748770, text_lines[0],
                            FUN_0044a3bf(player, combat_damage_attacker_cards[s.damage_index]),
                            s.attacker_damage_remaining);
                  }
                  else
                  {
                    sprintf(unk_00748770, text_lines[1],
                            FUN_0044a3bf(player, combat_damage_attacker_cards[s.damage_index]),
                            s.attacker_damage_remaining);
                  }

                  FUN_00516db1(player, combat_damage_attacker_cards[s.damage_index], 1);
                  s.valid_target = 0;
                  while (s.valid_target == 0)
                  {
                    C_real_select_target(s.assigning_player, s.defending_player, s.defending_player, 0x200, 2, 0, 0, 0, 0, 0,
                                         -1, -1, -1, -1, 0, 0x10, 0, unk_00748770, 0, &s.selected_target);
                    for (s.loop_index = 0; s.loop_index < ai_blocker_count; s.loop_index = s.loop_index + 1)
                    {
                      if (ai_blocker_cards[s.loop_index] == s.selected_target.card)
                      {
                        s.valid_target = 1;
                      }
                    }
                    if (s.valid_target == 0 && g_duel_ai_mode_state != 1)
                    {
                      FUN_004a61d6(text_lines[4]);
                      Sleep(1500);
                      FUN_004a61d6(DAT_0057dd7c);
                    }
                    if (s.valid_target == 1 && FUN_0051724e(s.selected_target.player, s.selected_target.card) != 0)
                    {
                      s.valid_target = 0;
                      if (g_duel_ai_mode_state != 1)
                      {
                        FUN_004a61d6(text_lines[5]);
                        Sleep(1500);
                        FUN_004a61d6(DAT_0057dd80);
                      }
                    }
                  }

                  strcpy(unk_00748770, DAT_0057dd84);
                  FUN_00516db1(player, combat_damage_attacker_cards[s.damage_index], 0);
                  if (s.selected_target.player != -1 && s.selected_target.card != -1 && s.selected_target.card != -2)
                  {
                    for (s.loop_index = 0; s.loop_index < ai_blocker_count; s.loop_index = s.loop_index + 1)
                    {
                      if (ai_blocker_cards[s.loop_index] == s.selected_target.card)
                      {
                        if (unk_00715fb0 == 0)
                        {
                          s.scratch[3] = 1;
                        }
                        else
                        {
                          s.scratch[3] = s.attacker_damage_remaining;
                        }
                        if (s.attacker_damage_ids[s.loop_index] == -1)
                        {
                          s.damage_card = damage_creature(s.selected_target.player, s.selected_target.card, s.scratch[3],
                                                          player, combat_damage_attacker_cards[s.damage_index]);
                          s.attacker_damage_ids[s.loop_index] = s.damage_card;
                          if (s.damage_card != -1)
                          {
                            PLAYER_CARD_INSTANCE(player, s.damage_card).token_status |= COMBAT_DAMAGE_FLAG_COMBAT;
                            if (((unsigned char)combat_damage_attacker_abilities[s.damage_index] & 0x80) != 0)
                            {
                              PLAYER_CARD_INSTANCE(player, s.damage_card).token_status |= COMBAT_DAMAGE_FLAG_TRAMPLE;
                            }
                            if (s.damage_step == 0)
                            {
                              PLAYER_CARD_INSTANCE(player, s.damage_card).token_status |= COMBAT_DAMAGE_FLAG_FIRST_STRIKE;
                            }
                          }
                        }
                        else
                        {
                          PLAYER_CARD_INSTANCE(player, s.attacker_damage_ids[s.loop_index]).info_slot += s.scratch[3];
                        }
                        s.attacker_damage_remaining -= s.scratch[3];
                      }
                    }
                  }
                }
              }
            }
          }

          if ((battlefield_extra_ability_flags & 4) != 0 &&
              s.defending_player == active_player &&
              ((unsigned char)g_duel_network_flags & 2) == 0)
          {
            s.trample_damage = 0;
            s.blocker_score[2] = 0;
            if (combat_damage_attacker_count < 2)
            {
              for (s.damage_index = 0; s.damage_index < ai_blocker_count; s.damage_index = s.damage_index + 1)
              {
                if ((PLAYER_CARD_INSTANCE(s.defending_player, ai_blocker_cards[s.damage_index]).token_status & COMBAT_DAMAGE_FLAG_CANCELABLE) == 0)
                {
                  s.blocker_score[2] += combat_damage_blocker_damage[s.damage_index];
                }
                else
                {
                  s.trample_damage += combat_damage_blocker_damage[s.damage_index];
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
                      s.trample_damage -= 1;
                      s.blocker_score[3] -= 1;
                      s.blocker_score[2] += 1;
                    }
                  }
                  if (s.blocker_score[3] != 0 &&
                      (global_cards_data[PLAYER_CARD_INSTANCE(s.defending_player, ai_blocker_cards[s.damage_index]).internal_card_id].code_pointer != FUN_00481e25 ||
                       1 < PLAYER_CARD_INSTANCE(s.defending_player, ai_blocker_cards[s.damage_index]).info_slot))
                  {
                    FUN_00515bba(s.defending_player, ai_blocker_cards[s.damage_index], (short)s.blocker_score[3]);
                    combat_damage_blocker_damage[s.damage_index] -= s.blocker_score[3];
                  }
                }
              }
            }
            else
            {
              for (s.damage_index = 0; s.damage_index < ai_blocker_count; s.damage_index = s.damage_index + 1)
              {
                if ((PLAYER_CARD_INSTANCE(s.defending_player, ai_blocker_cards[s.damage_index]).token_status & COMBAT_DAMAGE_FLAG_CANCELABLE) != 0 &&
                    (global_cards_data[PLAYER_CARD_INSTANCE(s.defending_player, ai_blocker_cards[s.damage_index]).internal_card_id].code_pointer != FUN_00481e25 ||
                     1 < PLAYER_CARD_INSTANCE(s.defending_player, ai_blocker_cards[s.damage_index]).info_slot))
                {
                  FUN_00515bba(s.defending_player, ai_blocker_cards[s.damage_index], (short)combat_damage_blocker_damage[s.damage_index]);
                  combat_damage_blocker_damage[s.damage_index] = 0;
                }
              }
            }
          }

          if (combat_damage_attacker_count < 2 &&
              ((battlefield_extra_ability_flags & 4) == 0 ||
               (s.defending_player != other_player && ((unsigned char)g_duel_network_flags & 2) == 0)))
          {
            if (combat_damage_attacker_count == 1)
            {
              for (s.damage_index = 0; s.damage_index < ai_blocker_count; s.damage_index = s.damage_index + 1)
              {
                if (FUN_00515ee6(s.damage_step, combat_damage_blocker_abilities[s.damage_index]) != 0)
                {
                  s.blocker_damage_ids[0] = damage_creature(player, combat_damage_attacker_cards[0],
                                                            combat_damage_blocker_damage[s.damage_index],
                                                            s.defending_player, ai_blocker_cards[s.damage_index]);
                  s.damage_card = s.blocker_damage_ids[0];
                  if (s.blocker_damage_ids[0] != -1)
                  {
                    PLAYER_CARD_INSTANCE(s.defending_player, s.blocker_damage_ids[0]).token_status |= COMBAT_DAMAGE_FLAG_COMBAT;
                    if (s.damage_step == 0)
                    {
                      PLAYER_CARD_INSTANCE(s.defending_player, s.blocker_damage_ids[0]).token_status |= COMBAT_DAMAGE_FLAG_FIRST_STRIKE;
                    }
                  }
                }
              }
            }
          }
          else
          {
            if ((s.scratch[4] == 0 && player == other_player) ||
                (s.scratch[4] != 0 && player == active_player))
            {
              s.assigning_player = active_player;
            }
            else
            {
              s.assigning_player = other_player;
            }

            if (((unsigned char)g_duel_network_flags & 2) == 0 &&
                (g_duel_ai_mode_state == 1 || s.assigning_player == active_player))
            {
              s.blocker_score[0] = 0x7fffffff;
              s.blocker_score[1] = -1;
              FUN_00516063();
              for (s.loop_index = 0; s.loop_index < 16; s.loop_index = s.loop_index + 1)
              {
                s.blocker_damage_ids[s.loop_index] = -1;
              }
              FUN_00516736(player, 0, s.scratch[4], (int)s.blocker_damage_ids, s.damage_step, 0, s.blocker_score, s.blocker_score + 1);
              FUN_00516736(player, 0, s.scratch[4], (int)s.blocker_damage_ids, s.damage_step, 1, s.blocker_score, s.blocker_score + 1);
            }
            else
            {
              for (s.damage_index = 0; s.damage_index < ai_blocker_count; s.damage_index = s.damage_index + 1)
              {
                for (s.loop_index = 0; s.loop_index < 16; s.loop_index = s.loop_index + 1)
                {
                  s.blocker_damage_ids[s.loop_index] = -1;
                }
                s.scratch[2] = combat_damage_blocker_damage[s.damage_index];
                s.canceled_assignment = 0;

                while (s.scratch[2] != 0 && s.canceled_assignment == 0)
                {
                  load_text(global_ui_strings_filename, s_PROMPT_RESOLVECOMBAT_0057dd88);
                  if (g_duel_interface_options.layout == 2)
                  {
                    sprintf(unk_00748770, text_lines[7], s.scratch[2]);
                  }
                  else
                  {
                    sprintf(unk_00748770, text_lines[6],
                            FUN_0044a3bf(s.defending_player, ai_blocker_cards[s.damage_index]), s.scratch[2]);
                  }

                  FUN_00516e2f(s.defending_player, ai_blocker_cards[s.damage_index], 1);
                  s.valid_target = 0;
                  if (combat_damage_attacker_count == 1 &&
                      ((PLAYER_CARD_INSTANCE(s.defending_player, ai_blocker_cards[s.damage_index]).token_status & COMBAT_DAMAGE_FLAG_CANCELABLE) == 0 ||
                       (global_cards_data[PLAYER_CARD_INSTANCE(s.defending_player, ai_blocker_cards[s.damage_index]).internal_card_id].code_pointer == FUN_00481e25 &&
                        PLAYER_CARD_INSTANCE(s.defending_player, ai_blocker_cards[s.damage_index]).info_slot < 2)))
                  {
                    s.valid_target = 1;
                    unk_00715fb0 = 0;
                    s.selected_target.player = player;
                    s.selected_target.card = combat_damage_attacker_cards[0];
                  }

                  while (s.valid_target == 0)
                  {
                    C_real_select_target(s.assigning_player, player, player, 0x200, 2, 0, 0, 0, 0, 0,
                                         -1, -1, -1, -1, 0, 2, 0, unk_00748770,
                                         (PLAYER_CARD_INSTANCE(s.defending_player, ai_blocker_cards[s.damage_index]).token_status &
                                          COMBAT_DAMAGE_FLAG_CANCELABLE) >> 26,
                                         &s.selected_target);
                    if (s.selected_target.card == -2)
                    {
                      s.valid_target = 1;
                      s.canceled_assignment = 1;
                      FUN_00515bba(s.defending_player, ai_blocker_cards[s.damage_index], (short)s.scratch[2]);
                    }
                    else
                    {
                      for (s.loop_index = 0; s.loop_index < combat_damage_attacker_count; s.loop_index = s.loop_index + 1)
                      {
                        if (combat_damage_attacker_cards[s.loop_index] == s.selected_target.card)
                        {
                          s.valid_target = 1;
                        }
                      }
                      if (s.valid_target == 0 && g_duel_ai_mode_state != 1)
                      {
                        FUN_004a61d6(text_lines[8]);
                        Sleep(1500);
                        FUN_004a61d6(DAT_0057dda0);
                      }
                      if (s.valid_target == 1 && FUN_0051724e(s.selected_target.player, s.selected_target.card) != 0)
                      {
                        s.valid_target = 0;
                        if (g_duel_ai_mode_state != 1)
                        {
                          FUN_004a61d6(text_lines[9]);
                          Sleep(1500);
                          FUN_004a61d6(DAT_0057dda4);
                        }
                      }
                    }
                  }

                  strcpy(unk_00748770, DAT_0057dda8);
                  FUN_00516e2f(s.defending_player, ai_blocker_cards[s.damage_index], 0);
                  if (s.selected_target.player != -1 && s.selected_target.card != -1 && s.selected_target.card != -2)
                  {
                    for (s.loop_index = 0; s.loop_index < combat_damage_attacker_count; s.loop_index = s.loop_index + 1)
                    {
                      if (combat_damage_attacker_cards[s.loop_index] == s.selected_target.card)
                      {
                        if (unk_00715fb0 == 0 &&
                            (global_cards_data[PLAYER_CARD_INSTANCE(s.defending_player, ai_blocker_cards[s.damage_index]).internal_card_id].code_pointer != FUN_00481e25 ||
                             1 < PLAYER_CARD_INSTANCE(s.defending_player, ai_blocker_cards[s.damage_index]).info_slot))
                        {
                          s.scratch[3] = 1;
                        }
                        else
                        {
                          s.scratch[3] = s.scratch[2];
                        }
                        if (s.blocker_damage_ids[s.loop_index] == -1)
                        {
                          s.damage_card = damage_creature(player, s.selected_target.card, s.scratch[3],
                                                          s.defending_player, ai_blocker_cards[s.damage_index]);
                          s.blocker_damage_ids[s.loop_index] = s.damage_card;
                          if (s.damage_card != -1)
                          {
                            PLAYER_CARD_INSTANCE(s.defending_player, s.damage_card).token_status |= COMBAT_DAMAGE_FLAG_COMBAT;
                            if (s.damage_step == 0)
                            {
                              PLAYER_CARD_INSTANCE(s.defending_player, s.damage_card).token_status |= COMBAT_DAMAGE_FLAG_FIRST_STRIKE;
                            }
                          }
                        }
                        else
                        {
                          PLAYER_CARD_INSTANCE(s.defending_player, s.blocker_damage_ids[s.loop_index]).info_slot += s.scratch[3];
                        }
                        s.scratch[2] -= s.scratch[3];
                      }
                    }
                  }
                }
              }

              if (0 < ai_blocker_count)
              {
                s.loop_index = ai_blocker_cards[s.damage_index - 1];
                if ((PLAYER_CARD_INSTANCE(s.defending_player, s.loop_index).token_status & COMBAT_DAMAGE_FLAG_CANCELABLE) != 0 &&
                    s.selected_target.card != -2 &&
                    (global_cards_data[PLAYER_CARD_INSTANCE(s.defending_player, s.loop_index).internal_card_id].code_pointer != FUN_00481e25 ||
                     1 < PLAYER_CARD_INSTANCE(s.defending_player, s.loop_index).info_slot) &&
                    ((s.damage_step == 0 && (((unsigned char *)&PLAYER_CARD_INSTANCE(s.defending_player, s.loop_index).regen_status)[1] & 1) != 0) ||
                     (s.damage_step == 1 && (((unsigned char *)&PLAYER_CARD_INSTANCE(s.defending_player, s.loop_index).regen_status)[1] & 1) == 0)))
                {
                  FUN_00515bba(s.defending_player, s.loop_index, (short)s.scratch[2]);
                }
              }
            }
          }
        }
      }
    }

    ai_blocker_count = 0;
    for (s.scratch[5] = 0; s.scratch[5] < active_cards_count[s.defending_player]; s.scratch[5] = s.scratch[5] + 1)
    {
      if (PLAYER_CARD_INSTANCE(s.defending_player, s.scratch[5]).internal_card_id != -1 &&
          PLAYER_CARD_INSTANCE(s.defending_player, s.scratch[5]).blocking != 0xff)
      {
        ai_blocker_cards[ai_blocker_count] = s.scratch[5];
        combat_damage_blocker_toughness[ai_blocker_count] =
            C_get_abilities(s.defending_player, s.scratch[5], 0x33, s.card) -
            PLAYER_CARD_INSTANCE(s.defending_player, s.scratch[5]).damage_on_card;
        if (FUN_0051724e(s.defending_player, s.scratch[5]) != 0)
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
    if (phase_was_skipped == 0 && prompt_stop_phase_anyway(current_phase) != 0 && g_duel_ai_mode_state != 1)
    {
      phase_was_skipped = 1;
    }

    for (s.loop_index = 0; s.loop_index < ai_blocker_count; s.loop_index = s.loop_index + 1)
    {
      s.scratch[5] = ai_blocker_cards[s.loop_index];
      s.scratch[2] = combat_damage_blocker_toughness[s.loop_index];

      for (s.card = 0; s.card < active_cards_count[player]; s.card = s.card + 1)
      {
        if (PLAYER_CARD_INSTANCE(player, s.card).original_internal_card_id == unk_009266a4 &&
            PLAYER_CARD_INSTANCE(player, s.card).damage_target_player == s.defending_player &&
            PLAYER_CARD_INSTANCE(player, s.card).damage_target_card == s.scratch[5] &&
            ((s.damage_step == 0 && (PLAYER_CARD_INSTANCE(player, s.card).token_status & COMBAT_DAMAGE_FLAG_FIRST_STRIKE) != 0) ||
             (s.damage_step == 1 && (PLAYER_CARD_INSTANCE(player, s.card).token_status & COMBAT_DAMAGE_FLAG_FIRST_STRIKE) == 0)) &&
            (PLAYER_CARD_INSTANCE(player, s.card).token_status & COMBAT_DAMAGE_FLAG_COMBAT) != 0 &&
            (PLAYER_CARD_INSTANCE(player, s.card).token_status & COMBAT_DAMAGE_FLAG_TRAMPLE) == 0)
        {
          s.scratch[2] -= PLAYER_CARD_INSTANCE(player, s.card).info_slot;
        }
      }

      for (s.card = 0; s.card < active_cards_count[player]; s.card = s.card + 1)
      {
        if (PLAYER_CARD_INSTANCE(player, s.card).original_internal_card_id == unk_009266a4 &&
            PLAYER_CARD_INSTANCE(player, s.card).damage_target_player == s.defending_player &&
            PLAYER_CARD_INSTANCE(player, s.card).damage_target_card == s.scratch[5] &&
            ((s.damage_step == 0 && (PLAYER_CARD_INSTANCE(player, s.card).token_status & COMBAT_DAMAGE_FLAG_FIRST_STRIKE) != 0) ||
             (s.damage_step == 1 && (PLAYER_CARD_INSTANCE(player, s.card).token_status & COMBAT_DAMAGE_FLAG_FIRST_STRIKE) == 0)) &&
            (PLAYER_CARD_INSTANCE(player, s.card).token_status & COMBAT_DAMAGE_FLAG_TRAMPLE) != 0 &&
            s.scratch[2] - ((int)(char)PLAYER_CARD_INSTANCE(player, s.card).unknown0x37 +
                            PLAYER_CARD_INSTANCE(player, s.card).info_slot) < 0)
        {
          s.valid_target = -(s.scratch[2] - ((int)(char)PLAYER_CARD_INSTANCE(player, s.card).unknown0x37 +
                                            PLAYER_CARD_INSTANCE(player, s.card).info_slot));
          s.damage_card = (int)(char)PLAYER_CARD_INSTANCE(player, s.card).unknown0x37 +
                          PLAYER_CARD_INSTANCE(player, s.card).info_slot;
          if (s.damage_card <= s.valid_target)
          {
            s.valid_target = s.damage_card;
          }
          damage_player(s.defending_player, s.valid_target,
                        (int)(char)PLAYER_CARD_INSTANCE(player, s.card).damage_source_player,
                        PLAYER_CARD_INSTANCE(player, s.card).damage_source_card);
          s.scratch[2] -= (int)(char)PLAYER_CARD_INSTANCE(player, s.card).unknown0x37 +
                          PLAYER_CARD_INSTANCE(player, s.card).info_slot;
        }
      }
    }

    regenerate_or_graveyard_triggers();
    process_damage_prevention(player);
    if ((battlefield_extra_ability_flags & 1) != 0)
    {
      battlefield_extra_ability_flags &= 0xfffffffe;
      DAT_007ab2cc = FUN_004b0c12;
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

    s.damage_step = s.damage_step + 1;
  }
}
