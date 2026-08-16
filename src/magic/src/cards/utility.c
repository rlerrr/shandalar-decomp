#include <string.h>
#include "../game_support.h"
#include "../global_duel_ui_ids.h"
#include "../global_strings.h"

int create_damage_effect_copy_for_target_player(int player, int card);
int titanias_leg_kill_invalid_animation(int source_player, int source_card, int test_player, int test_card, int internal_card_id);

// FUNCTION: MAGIC 0x004efd03
// FUNCTION: SHANDALAR 0x004a9dc3
int card_power_up(int player, int card, event_t event)
{
  if (PLAYER_CARD_INSTANCE(player, card).damage_target_card == g_affected_card)
  {
    if ((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player == g_affected_card_controller)
    {
      if (g_affected_card != -1)
      {
        if (event == EVENT_POWER)
        {
          g_event_result += PLAYER_CARD_INSTANCE(player, card).counter_power;
        }

        if (event == EVENT_TOUGHNESS)
        {
          g_event_result += PLAYER_CARD_INSTANCE(player, card).power;
        }
      }
    }
  }

  if (event == EVENT_CLEANUP)
  {
    if ((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player != -1 &&
        PLAYER_CARD_INSTANCE(player, card).damage_target_card != -1)
    {
      PLAYER_CARD_INSTANCE((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                           PLAYER_CARD_INSTANCE(player, card).damage_target_card)
          .regen_status |= 0x0f000000;
    }
    kill_card(player, card, KILL_REMOVE);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004f0186
// FUNCTION: SHANDALAR 0x004aa246
int card_unblockable(int player, int card, event_t event)
{
  if (event == EVENT_BLOCK_LEGALITY)
  {
    if (g_attacking_card == PLAYER_CARD_INSTANCE(player, card).damage_target_card &&
        g_attacking_card_controller == PLAYER_CARD_INSTANCE(player, card).damage_target_player)
    {
      ++g_event_result;
    }
  }

  if (event == EVENT_CLEANUP)
  {
    return 0;
  }

  if (event == EVENT_SHOULD_AI_PLAY)
  {
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004f0246
// FUNCTION: SHANDALAR 0x004aa306
int card_no_attack(int player, int card, event_t event)
{
  card_instance_t *instance;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_ATTACK_LEGALITY &&
      g_affected_card == instance->damage_target_card &&
      g_affected_card_controller == (int)instance->damage_target_player)
  {
    g_event_result = 1;
  }

  if (event == EVENT_CLEANUP)
  {
    if (instance->damage_target_card == -1 ||
        instance->damage_target_player == -1 ||
        !is_in_play((int)instance->damage_target_player, instance->damage_target_card))
    {
      kill_card(player, card, KILL_REMOVE);
    }
    else if ((int)instance->damage_target_player == g_current_turn)
    {
      if (instance->info_slot != 0)
      {
        kill_card(player, card, KILL_REMOVE);
      }
      else
      {
        instance->info_slot = 1;
      }
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x004f03ad
// FUNCTION: SHANDALAR 0x004aa46d
int card_add_ability(int player, int card, event_t event)
{
  card_instance_t *instance;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_ABILITIES &&
      g_affected_card == instance->damage_target_card &&
      g_affected_card_controller == (int)instance->damage_target_player &&
      g_affected_card != -1)
  {
    g_event_result |= instance->info_slot;
  }

  if (event == EVENT_CLEANUP || event == EVENT_SHOULD_AI_PLAY)
  {
    kill_card(player, card, KILL_REMOVE);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004f0a6b
// FUNCTION: SHANDALAR 0x004aab29
int card_take_ability(int player, int card, event_t event)
{
  card_instance_t *instance;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_ABILITIES &&
      g_affected_card == instance->damage_target_card &&
      g_affected_card_controller == (int)instance->damage_target_player &&
      g_affected_card != -1)
  {
    g_event_result &= ~instance->info_slot;
  }

  if (event == EVENT_CLEANUP || event == EVENT_SHOULD_AI_PLAY)
  {
    kill_card(player, card, KILL_REMOVE);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004f0c29
// FUNCTION: SHANDALAR 0x004aace7
int card_damage(int player, int card, event_t event)
{
  struct
  {
    int source_internal_card_id;
    int redirected_source_internal_card_id;
    int protection_source_internal_card_id;
    unsigned int target_abilities;
    int source_color;
    int redirected_source_card;
    int redirected_source_player;
  } s;

  if (card == g_affected_card &&
      player == g_affected_card_controller &&
      PLAYER_CARD_INSTANCE(player, card).damage_target_card != -1 &&
      (g_land_can_be_played & 4) != 0)
  {
    s.target_abilities =
        C_get_abilities((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                        PLAYER_CARD_INSTANCE(player, card).damage_target_card,
                        EVENT_ABILITIES,
                        -1);
    if ((s.target_abilities & 0x1ff800) != 0)
    {
      s.source_color =
          single_color_test_bit_to_color_t(
              (color_test_t)PLAYER_CARD_INSTANCE((int)PLAYER_CARD_INSTANCE(player, card).damage_source_player,
                                                 PLAYER_CARD_INSTANCE(player, card).damage_source_card)
                  .color);
      if ((s.target_abilities & (0x800 << (s.source_color - 1))) != 0)
      {
        PLAYER_CARD_INSTANCE(player, card).info_slot = 0;
        kill_card(player, card, KILL_BURY);
      }
      else if ((s.target_abilities & 0x10000) != 0)
      {
        if (PLAYER_CARD_INSTANCE((int)PLAYER_CARD_INSTANCE(player, card).damage_source_player,
                                 PLAYER_CARD_INSTANCE(player, card).damage_source_card)
                .internal_card_id == -1)
        {
          s.protection_source_internal_card_id =
              PLAYER_CARD_INSTANCE((int)PLAYER_CARD_INSTANCE(player, card).damage_source_player,
                                   PLAYER_CARD_INSTANCE(player, card).damage_source_card)
                  .original_internal_card_id;
        }
        else
        {
          s.protection_source_internal_card_id =
              PLAYER_CARD_INSTANCE((int)PLAYER_CARD_INSTANCE(player, card).damage_source_player,
                                   PLAYER_CARD_INSTANCE(player, card).damage_source_card)
                  .internal_card_id;
        }
        if ((global_cards_data[s.protection_source_internal_card_id].type & TYPE_ARTIFACT) != 0)
        {
          PLAYER_CARD_INSTANCE(player, card).info_slot = 0;
          kill_card(player, card, KILL_BURY);
        }
      }
    }
  }

  if (event == EVENT_DEAL_DAMAGE &&
      card == g_affected_card &&
      player == g_affected_card_controller &&
      (PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0)
  {
    PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;

    if (PLAYER_CARD_INSTANCE((int)PLAYER_CARD_INSTANCE(player, card).damage_source_player,
                             PLAYER_CARD_INSTANCE(player, card).damage_source_card)
                .original_internal_card_id == g_damage_effect_internal_card_id &&
        PLAYER_CARD_INSTANCE(player, card).info_slot != 0)
    {
      PLAYER_CARD_INSTANCE((int)PLAYER_CARD_INSTANCE(player, card).damage_source_player,
                           PLAYER_CARD_INSTANCE(player, card).damage_source_card)
          .eot_toughness = (unsigned int)(int)PLAYER_CARD_INSTANCE(player, card).damage_source_player;
      create_damage_effect_copy_for_target_player((int)PLAYER_CARD_INSTANCE(player, card).damage_source_player,
                                            PLAYER_CARD_INSTANCE(player, card).damage_source_card);
    }

    if (PLAYER_CARD_INSTANCE(player, card).damage_target_card == -1)
    {
      if (g_duel_ai_mode_state != 1)
      {
        play_sound_effect(0xe);
      }

      if (PLAYER_CARD_INSTANCE((int)PLAYER_CARD_INSTANCE(player, card).damage_source_player,
                               PLAYER_CARD_INSTANCE(player, card).damage_source_card)
                  .internal_card_id == -1 ||
          global_cards_data[PLAYER_CARD_INSTANCE((int)PLAYER_CARD_INSTANCE(player, card).damage_source_player,
                                                 PLAYER_CARD_INSTANCE(player, card).damage_source_card)
                                .internal_card_id]
                  .id != unk_008a8de8)
      {
        unk_0093b280[(int)PLAYER_CARD_INSTANCE(player, card).damage_source_player]
                    [PLAYER_CARD_INSTANCE(player, card).damage_source_card]
                    [(int)PLAYER_CARD_INSTANCE(player, card).damage_target_player][0] +=
            PLAYER_CARD_INSTANCE(player, card).info_slot;

        if (PLAYER_CARD_INSTANCE((int)PLAYER_CARD_INSTANCE(player, card).damage_source_player,
                                 PLAYER_CARD_INSTANCE(player, card).damage_source_card)
                .internal_card_id == -1)
        {
          s.source_internal_card_id =
              PLAYER_CARD_INSTANCE((int)PLAYER_CARD_INSTANCE(player, card).damage_source_player,
                                   PLAYER_CARD_INSTANCE(player, card).damage_source_card)
                  .original_internal_card_id;
        }
        else
        {
          s.source_internal_card_id =
              PLAYER_CARD_INSTANCE((int)PLAYER_CARD_INSTANCE(player, card).damage_source_player,
                                   PLAYER_CARD_INSTANCE(player, card).damage_source_card)
                  .internal_card_id;
        }

        unk_0093b280[(int)PLAYER_CARD_INSTANCE(player, card).damage_source_player]
                    [PLAYER_CARD_INSTANCE(player, card).damage_source_card]
                    [(int)PLAYER_CARD_INSTANCE(player, card).damage_target_player][1] =
            (unsigned int)global_cards_data[s.source_internal_card_id].type;
        if ((unk_0093b280[(int)PLAYER_CARD_INSTANCE(player, card).damage_source_player]
                           [PLAYER_CARD_INSTANCE(player, card).damage_source_card]
                           [(int)PLAYER_CARD_INSTANCE(player, card).damage_target_player][1] &
             TYPE_ARTIFACT) == 0)
        {
          unk_0093b280[0][150][(int)PLAYER_CARD_INSTANCE(player, card).damage_target_player][0] = 1;
        }
        else
        {
          unk_0093b280[1][150][(int)PLAYER_CARD_INSTANCE(player, card).damage_target_player][0] = 1;
        }
      }
      else
      {
        s.redirected_source_player =
            (int)PLAYER_CARD_INSTANCE((int)PLAYER_CARD_INSTANCE(player, card).damage_source_player,
                                      PLAYER_CARD_INSTANCE(player, card).damage_source_card)
                .damage_source_player;
        s.redirected_source_card =
            PLAYER_CARD_INSTANCE((int)PLAYER_CARD_INSTANCE(player, card).damage_source_player,
                                 PLAYER_CARD_INSTANCE(player, card).damage_source_card)
                .damage_source_card;

        unk_0093b280[s.redirected_source_player][s.redirected_source_card]
                    [(int)PLAYER_CARD_INSTANCE(player, card).damage_target_player][0] +=
            PLAYER_CARD_INSTANCE(player, card).info_slot;

        if (PLAYER_CARD_INSTANCE(s.redirected_source_player, s.redirected_source_card).internal_card_id == -1)
        {
          s.redirected_source_internal_card_id =
              PLAYER_CARD_INSTANCE(s.redirected_source_player, s.redirected_source_card).original_internal_card_id;
        }
        else
        {
          s.redirected_source_internal_card_id =
              PLAYER_CARD_INSTANCE(s.redirected_source_player, s.redirected_source_card).internal_card_id;
        }

        unk_0093b280[s.redirected_source_player][s.redirected_source_card]
                    [(int)PLAYER_CARD_INSTANCE(player, card).damage_target_player][1] =
            (unsigned int)global_cards_data[s.redirected_source_internal_card_id].type;
        if ((unk_0093b280[s.redirected_source_player][s.redirected_source_card]
                           [(int)PLAYER_CARD_INSTANCE(player, card).damage_target_player][1] &
             TYPE_ARTIFACT) == 0)
        {
          unk_0093b280[0][150][(int)PLAYER_CARD_INSTANCE(player, card).damage_target_player][0] = 1;
        }
        else
        {
          unk_0093b280[1][150][(int)PLAYER_CARD_INSTANCE(player, card).damage_target_player][0] = 1;
        }
      }

      g_life[(int)PLAYER_CARD_INSTANCE(player, card).damage_target_player] -=
          PLAYER_CARD_INSTANCE(player, card).info_slot;
      g_duel_summary.player_damage_totals[(int)PLAYER_CARD_INSTANCE(player, card).damage_target_player] +=
          PLAYER_CARD_INSTANCE(player, card).info_slot;
    }
    else if (is_in_play((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                        PLAYER_CARD_INSTANCE(player, card).damage_target_card))
    {
      PLAYER_CARD_INSTANCE((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                           PLAYER_CARD_INSTANCE(player, card).damage_target_card)
          .damage_on_card += PLAYER_CARD_INSTANCE(player, card).info_slot;
      if (g_duel_ai_mode_state != 1)
      {
        play_sound_effect(0x16);
        request_duel_display_refresh_if_human((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                      PLAYER_CARD_INSTANCE(player, card).damage_target_card,
                      6,
                      2);
      }
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x004f1e72
// FUNCTION: SHANDALAR 0x004abf34
int card_asp_sting(int player, int card, event_t event)
{
  int paid;
  int target_player;

  target_player = (int)PLAYER_CARD_INSTANCE(player, card).damage_target_player;

  if ((g_trigger_condition == TRIGGER_UPKEEP || event == EVENT_SHOULD_AI_PLAY) &&
      g_affected_card == card &&
      g_affected_card_controller == player &&
      target_player == g_current_player)
  {
    if (event == EVENT_TRIGGER)
    {
      g_event_result |= 2;
    }
    if (event == EVENT_RESOLVE_TRIGGER || event == EVENT_SHOULD_AI_PLAY)
    {
      paid = 0;
      if (has_mana(target_player, COLOR_COLORLESS, 1) != 0)
      {
        charge_mana(target_player, COLOR_COLORLESS, 1);
        if (g_spell_fizzled != 1)
        {
          paid = 1;
        }
      }
      if (paid == 0)
      {
        g_spell_fizzled = 0;
        damage_player(target_player, 1, player, card);
      }
      kill_card(player, card, KILL_REMOVE);
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x004f208d
// FUNCTION: SHANDALAR 0x004ac14c
int card_stoning(int player, int card, event_t event)
{
  if (event == EVENT_CLEANUP || event == EVENT_SHOULD_AI_PLAY)
  {
    if (PLAYER_CARD_INSTANCE(player, card).damage_target_card != -1 &&
        PLAYER_CARD_INSTANCE(player, card).damage_target_player != -1 &&
        is_in_play((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                   PLAYER_CARD_INSTANCE(player, card).damage_target_card) != 0)
    {
      kill_card((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                PLAYER_CARD_INSTANCE(player, card).damage_target_card,
                KILL_DESTROY);
    }
    kill_card(player, card, KILL_REMOVE);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004f21b5
// FUNCTION: SHANDALAR 0x004ac273
int card_t_elemental_FX(int player, int card, event_t event)
{
  card_instance_t *instance;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if ((g_trigger_condition == TRIGGER_END_COMBAT || event == EVENT_SHOULD_AI_PLAY) &&
      g_affected_card == card &&
      g_affected_card_controller == player)
  {
    if (event == EVENT_TRIGGER)
    {
      g_event_result |= 2;
    }

    if (event == EVENT_RESOLVE_TRIGGER || event == EVENT_SHOULD_AI_PLAY)
    {
      if (instance->damage_target_player != -1)
      {
        damage_player(player, 5, player, card);
      }

      if (is_in_play((int)instance->damage_source_player, instance->damage_source_card) != 0)
      {
        kill_card((int)instance->damage_source_player, instance->damage_source_card, KILL_BURY);
      }

      kill_card(player, card, KILL_BURY);
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x004f2360
// FUNCTION: SHANDALAR 0x004ac420
int card_marsh_gas_1(int player, int card, event_t event)
{
  if (event == EVENT_POWER &&
      is_in_play(g_affected_card_controller, g_affected_card) != 0 &&
      (global_cards_data[PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).internal_card_id].type & TYPE_CREATURE) != 0)
  {
    g_event_result -= 2;
  }

  if (event == EVENT_CLEANUP || event == EVENT_SHOULD_AI_PLAY)
  {
    kill_card(player, card, KILL_REMOVE);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004f272d
// FUNCTION: SHANDALAR 0x004ac7ed
int card_fog_effect(int player, int card, event_t event)
{
  card_instance_t *damage;

  if (event == EVENT_PREVENT_DAMAGE &&
      PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).internal_card_id == g_damage_card_internal_card_id)
  {
    damage = &PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card);
    if (damage->info_slot != 0 &&
        (damage->eot_toughness & TYPE_CREATURE) != 0)
    {
      damage->unknown0x37 = (char)damage->info_slot;
      damage->info_slot = 0;
    }
  }

  if (event == EVENT_CLEANUP || event == EVENT_SHOULD_AI_PLAY)
  {
    kill_card(player, card, KILL_REMOVE);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004f29a0
// FUNCTION: SHANDALAR 0x004aca60
int card_channel_1(int player, int card, event_t event)
{
  if (event == EVENT_COUNT_MANA && g_affected_card == card && g_affected_card_controller == player)
  {
    if (g_life[player] > 1)
    {
      declare_mana_available(player, COLOR_COLORLESS, g_life[player] - 1);
    }
    return 0;
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    return g_life[player] > 1;
  }

  if (event == EVENT_ACTIVATE)
  {
    if (g_life[player] > 1)
    {
      --g_life[player];
      produce_mana(player, COLOR_COLORLESS, 1);
      g_produced_mana_color = COLOR_COLORLESS;
    }
    else
    {
      g_spell_fizzled = 1;
    }
    return 0;
  }

  if (event == EVENT_CLEANUP || event == EVENT_SHOULD_AI_PLAY)
  {
    kill_card(player, card, KILL_REMOVE);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004f2b93
// FUNCTION: SHANDALAR 0x004acc56
int card_generic(int player, int card, event_t event)
{
  card_instance_t *instance;
  card_instance_t *source;
  card_instance_t *target;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if ((instance->token_status & STATUS_BERSERK) != 0)
  {
    if (instance->damage_target_card == g_affected_card &&
        (int)instance->damage_target_player == g_affected_card_controller &&
        g_affected_card != -1)
    {
      if (event == EVENT_ABILITIES)
      {
        g_event_result |= instance->info_slot;
      }

      if (event == EVENT_POWER)
      {
        g_event_result += instance->counter_power;
      }
    }

    if (card == g_affected_card &&
        player == g_affected_card_controller &&
        event == EVENT_CLEANUP &&
        instance->damage_target_card != -1 &&
        (PLAYER_CARD_INSTANCE((int)instance->damage_target_player,
                              instance->damage_target_card)
             .state &
         0x40) != 0)
    {
      instance->kill_code = 5;
    }
  }

  if ((instance->token_status & STATUS_LEGACY_TYPECHANGE) != 0)
  {
    if (event == EVENT_CHANGE_TYPE &&
        (g_land_can_be_played & LCBP_DURING_EVENT_CHANGE_TYPE_SECOND_PASS) == 0 &&
        instance->damage_target_card == g_affected_card &&
        (int)instance->damage_target_player == g_affected_card_controller &&
        g_affected_card != -1 &&
        PLAYER_CARD_INSTANCE((int)instance->damage_source_player,
                             instance->damage_source_card)
                .internal_card_id != -1)
    {
      source = &PLAYER_CARD_INSTANCE((int)instance->damage_source_player, instance->damage_source_card);
      g_event_result = get_hacked_color(player, card, source->info_slot) - 1;
    }

    if (g_trigger_condition == TRIGGER_LEAVE_PLAY &&
        card == g_affected_card &&
        player == g_affected_card_controller &&
        player == g_current_turn &&
        (int)instance->damage_source_player == g_trigger_cause_controller &&
        instance->damage_source_card == g_trigger_cause)
    {
      if (event == EVENT_TRIGGER)
      {
        g_event_result |= 2;
      }

      if (event == EVENT_RESOLVE_TRIGGER)
      {
        instance->token_status &= ~STATUS_INVISIBLE_FX;
        kill_card(player, card, KILL_DESTROY);
      }
    }

    if (event == EVENT_GRAVEYARD_FROM_PLAY &&
        (int)instance->damage_source_player == g_affected_card_controller &&
        instance->damage_source_card == g_affected_card)
    {
      target = &PLAYER_CARD_INSTANCE((int)instance->damage_target_player, instance->damage_target_card);
      target->regen_status |= 0x1000000;
      kill_card(player, card, KILL_BURY);
    }
  }

  if ((instance->token_status & STATUS_ISLAND_SANCTUARY) != 0)
  {
    if (event == EVENT_CAN_SKIP_TURN && player == g_current_player)
    {
      kill_card(player, card, KILL_DESTROY);
      unk_007a79b0[player] &= ~1;
    }

    if (event == EVENT_ATTACK_LEGALITY &&
        (instance->info_slot & PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).color) == 0)
    {
      ++g_event_result;
    }
  }

  if ((instance->token_status & STATUS_TIMEWALK) != 0)
  {
    if (event == EVENT_CAN_SKIP_TURN && g_duel_extra_turn_player == -1)
    {
      g_duel_extra_turn_player = player;
    }

    if (event == EVENT_CLEANUP && DAT_0093a848 == 0)
    {
      DAT_0093a848 = 1 << (unsigned char)player;
      instance->token_status &= ~STATUS_PERMANENT;
    }
  }

  if (card == g_affected_card && player == g_affected_card_controller)
  {
    if (instance->kill_code == 5)
    {
      if ((g_trigger_condition == TRIGGER_EOT || event == EVENT_SHOULD_AI_PLAY) &&
          (int)instance->damage_target_player == g_current_turn)
      {
        if (event == EVENT_TRIGGER)
        {
          g_event_result |= 2;
        }

        if (event == EVENT_RESOLVE_TRIGGER || event == EVENT_SHOULD_AI_PLAY)
        {
          kill_card((int)instance->damage_target_player, instance->damage_target_card, KILL_DESTROY);
          if (instance->internal_card_id != -1)
          {
            kill_card(player, card, KILL_DESTROY);
          }
        }
      }
    }
    else if ((instance->token_status & STATUS_PERMANENT) == 0 &&
             (event == EVENT_CLEANUP || event == EVENT_SHOULD_AI_PLAY))
    {
      kill_card(player, card, KILL_BURY);
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x004f3587
// FUNCTION: SHANDALAR 0x004ad642
int card_asterisk_FX(int player, int card, event_t event)
{
  int count;
  int current_card;
  int current_iid;
  int current_player;
  card_instance_t *instance;
  int mode;
  int target_player;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (instance->damage_target_card == g_affected_card &&
      (int)instance->damage_target_player == g_affected_card_controller &&
      g_affected_card != -1 &&
      ((event == EVENT_POWER && (instance->eot_toughness & 4) != 0) ||
       (event == EVENT_TOUGHNESS && (instance->eot_toughness & 8) != 0)))
  {
    count = 0;
    mode = instance->eot_toughness & 0xf00;
    if (mode == 0x200)
    {
      target_player = (int)instance->damage_target_player;
      for (current_player = 0; current_player < 2; ++current_player)
      {
        if ((target_player == current_player && (instance->eot_toughness & 1) != 0) ||
            (target_player != current_player && (instance->eot_toughness & 2) != 0))
        {
          for (current_card = 0; current_card < g_active_cards_count[current_player]; ++current_card)
          {
            current_iid = PLAYER_CARD_INSTANCE(current_player, current_card).internal_card_id;
            if (is_in_play(current_player, current_card) != 0)
            {
              if ((instance->eot_toughness & 0x100000) == 0)
              {
                if (global_cards_data[instance->info_slot].id == global_cards_data[current_iid].id)
                {
                  ++count;
                }
              }
              else if ((instance->info_slot & global_cards_data[current_iid].type) != 0)
              {
                ++count;
              }
            }
          }
        }
      }
    }
    else if (mode == 0x100)
    {
      target_player = (int)instance->damage_target_player;
      if ((instance->eot_toughness & 1) != 0)
      {
        count = g_basiclandtypes_controlled[target_player][get_hacked_color(target_player,
                                                                            instance->damage_target_card,
                                                                            instance->info_slot)];
      }
      if ((instance->eot_toughness & 2) != 0)
      {
        count += g_basiclandtypes_controlled[1 - target_player][get_hacked_color(target_player,
                                                                                 instance->damage_target_card,
                                                                                 instance->info_slot)];
      }
    }
    else if (mode == 0x400)
    {
      if (event == EVENT_POWER)
      {
        count = instance->info_slot & 0xff;
      }
      else
      {
        count = (instance->info_slot >> 8) & 0xff;
      }
    }
    else if (mode == 0x800)
    {
      target_player = (int)instance->damage_target_player;
      for (current_player = 0; current_player < 2; ++current_player)
      {
        if ((target_player == current_player && (instance->eot_toughness & 1) != 0) ||
            (target_player != current_player && (instance->eot_toughness & 2) != 0))
        {
          for (current_card = 0; current_card < g_active_cards_count[current_player]; ++current_card)
          {
            if (is_in_play(current_player, current_card) != 0)
            {
              current_iid = PLAYER_CARD_INSTANCE(current_player, current_card).internal_card_id;
              if ((global_cards_data[current_iid].type & TYPE_CREATURE) != 0 &&
                  global_cards_data[current_iid].subtype != 0)
              {
                ++count;
              }
            }
          }
        }
      }
    }

    g_event_result += count;
    if (event == EVENT_POWER)
    {
      instance->counter_power = (short)count;
    }
    else
    {
      instance->counter_toughness = (short)count;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x004f3e18
// FUNCTION: SHANDALAR 0x004aded3
int card_piggy_FX(int player, int card, event_t event)
{
  card_instance_t *instance;
  card_instance_t *legacy;
  int current_card;
  int found;
  int power_delta;
  int target_player;
  int toughness_delta;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if ((instance->token_status & STATUS_CONTROLLED) != 0 &&
      instance->damage_target_card == g_affected_card &&
      (int)instance->damage_target_player == g_affected_card_controller &&
      g_affected_card != -1 &&
      (event == EVENT_POWER || event == EVENT_TOUGHNESS))
  {
    power_delta = 0;
    toughness_delta = 0;
    target_player = (int)instance->damage_target_player;
    found = 0;

    for (current_card = 0; current_card < g_active_cards_count[target_player] && found == 0; ++current_card)
    {
      legacy = &PLAYER_CARD_INSTANCE(target_player, current_card);
      if (legacy->internal_card_id == unk_0093d848 &&
          is_in_play(target_player, current_card) != 0 &&
          legacy->damage_target_player == instance->damage_target_player &&
          legacy->damage_target_card == instance->damage_target_card)
      {
        found = 1;
        power_delta = -legacy->counter_power;
        toughness_delta = -legacy->counter_toughness;
      }
    }

    if (event == EVENT_POWER)
    {
      g_event_result += instance->counter_power + power_delta;
    }
    else
    {
      g_event_result += instance->counter_toughness + toughness_delta;
    }
  }

  if ((instance->token_status & STATUS_PERMANENT) == 0 &&
      (event == EVENT_CLEANUP || event == EVENT_SHOULD_AI_PLAY))
  {
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004f41d5
// FUNCTION: SHANDALAR 0x004ae290
int card_titanias_leg(int player, int card, event_t event)
{
  if (event == EVENT_CLEANUP && PLAYER_CARD_INSTANCE(player, card).eot_toughness == 0)
  {
    invalidate_dynamic_card_type(PLAYER_CARD_INSTANCE(player, card).dummy3);
    kill_card(player, card, KILL_BURY);
    PLAYER_CARD_INSTANCE((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                         PLAYER_CARD_INSTANCE(player, card).damage_target_card)
        .regen_status |= 0x1000000;
    C_get_abilities((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                    PLAYER_CARD_INSTANCE(player, card).damage_target_card,
                    EVENT_CHANGE_TYPE,
                    -1);
    dispatch_function_to_all_cards_in_play(player, card, titanias_leg_kill_invalid_animation, -1);
    C_count_colors_of_lands_in_play();
  }

  if (event == EVENT_CHANGE_TYPE &&
      (g_land_can_be_played & LCBP_DURING_EVENT_CHANGE_TYPE_SECOND_PASS) == 0 &&
      PLAYER_CARD_INSTANCE(player, card).damage_target_card == g_affected_card &&
      (int)PLAYER_CARD_INSTANCE(player, card).damage_target_player == g_affected_card_controller &&
      g_affected_card != -1)
  {
    g_event_result = PLAYER_CARD_INSTANCE(player, card).dummy3;
  }

  return 0;
}

// FUNCTION: SHANDALAR 0x004ae506
int titanias_leg_kill_invalid_animation(int source_player, int source_card, int test_player, int test_card, int internal_card_id)
{
  if (PLAYER_CARD_INSTANCE(test_player, test_card).damage_target_card ==
          PLAYER_CARD_INSTANCE(source_player, source_card).damage_target_card &&
      PLAYER_CARD_INSTANCE(test_player, test_card).damage_target_player ==
          PLAYER_CARD_INSTANCE(source_player, source_card).damage_target_player &&
      (global_cards_data[internal_card_id].type & TYPE_ENCHANTMENT) != 0)
  {
    if ((global_cards_data[PLAYER_CARD_INSTANCE((int)PLAYER_CARD_INSTANCE(test_player, test_card).damage_target_player,
                                                PLAYER_CARD_INSTANCE(test_player, test_card).damage_target_card)
                             .internal_card_id]
             .type &
         TYPE_CREATURE) == 0)
    {
      kill_card(test_player, test_card, KILL_BURY);
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x004f45e9
// FUNCTION: SHANDALAR 0x004ae6a4
int card_disintegrt_FX(int player, int card, event_t event)
{
  card_instance_t *instance;
  card_instance_t *target;

  instance = &PLAYER_CARD_INSTANCE(player, card);
  target = &PLAYER_CARD_INSTANCE((int)instance->damage_target_player, instance->damage_target_card);

  if ((target->token_status & STATUS_DYING) != 0)
  {
    target->kill_code = KILL_REMOVE;
  }

  if ((event == EVENT_CLEANUP || event == EVENT_SHOULD_AI_PLAY) &&
      instance->damage_target_player != -1)
  {
    target->regen_status = 0x8000000;
  }

  if (event == EVENT_CLEANUP || event == EVENT_SHOULD_AI_PLAY)
  {
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004f47f7
// FUNCTION: SHANDALAR 0x004ae8b2
int card_sirens_call_FX(int player, int card, event_t event)
{
  int current_card;
  int current_iid;
  int target_player;

  target_player = 1 - player;

  if (event == EVENT_CLEANUP || event == EVENT_SHOULD_AI_PLAY)
  {
    for (current_card = 0; current_card < g_active_cards_count[target_player]; ++current_card)
    {
      if (is_in_play(target_player, current_card) != 0)
      {
        current_iid = PLAYER_CARD_INSTANCE(target_player, current_card).internal_card_id;
        if (current_iid != -1 &&
            (global_cards_data[current_iid].type & TYPE_CREATURE) != 0 &&
            global_cards_data[current_iid].subtype != SUB_WALL &&
            (PLAYER_CARD_INSTANCE(target_player, current_card).state & STATE_ATTACKED) == 0 &&
            can_attack(target_player, current_card) != 0)
        {
          kill_card(target_player, current_card, KILL_DESTROY);
        }
      }
    }
    kill_card(player, card, KILL_REMOVE);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004f4a09
// FUNCTION: SHANDALAR 0x004aeac7
int card_ebony_horse_FX(int player, int card, event_t event)
{
  if (event == EVENT_PREVENT_DAMAGE &&
      (g_current_phase == PHASE_NORMAL_COMBAT_DAMAGE || g_current_phase == PHASE_FIRST_STRIKE_DAMAGE))
  {
    if (PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).damage_target_player ==
            PLAYER_CARD_INSTANCE(player, card).damage_target_player &&
        PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).damage_target_card ==
            PLAYER_CARD_INSTANCE(player, card).damage_target_card)
    {
      PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).info_slot = 0;
    }
    if (PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).damage_source_player ==
            PLAYER_CARD_INSTANCE(player, card).damage_target_player &&
        PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).damage_source_card ==
            PLAYER_CARD_INSTANCE(player, card).damage_target_card)
    {
      PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).info_slot = 0;
    }
  }

  if ((g_trigger_condition == TRIGGER_END_COMBAT || event == EVENT_SHOULD_AI_PLAY) &&
      g_affected_card == card &&
      g_affected_card_controller == player)
  {
    if (event == EVENT_TRIGGER)
    {
      g_event_result |= RESOLVE_TRIGGER_MANDATORY;
    }
    if (event == EVENT_RESOLVE_TRIGGER || event == EVENT_SHOULD_AI_PLAY)
    {
      kill_card(player, card, KILL_BURY);
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x004f4cc8
// FUNCTION: SHANDALAR 0x004aed84
int card_poltergeist_FX(int player, int card, event_t event)
{
  if (event == EVENT_CHANGE_TYPE &&
      (g_land_can_be_played & LCBP_DURING_EVENT_CHANGE_TYPE_SECOND_PASS) == 0 &&
      PLAYER_CARD_INSTANCE(player, card).damage_target_card == g_affected_card &&
      (int)PLAYER_CARD_INSTANCE(player, card).damage_target_player == g_affected_card_controller &&
      g_affected_card != -1 &&
      PLAYER_CARD_INSTANCE(player, card).dummy3 != -1)
  {
    g_event_result = PLAYER_CARD_INSTANCE(player, card).dummy3;
    PLAYER_CARD_INSTANCE((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                         PLAYER_CARD_INSTANCE(player, card).damage_target_card)
        .token_status |= 0x40;
  }

  if ((g_trigger_condition == TRIGGER_UPKEEP || event == EVENT_SHOULD_AI_PLAY) &&
      card == g_affected_card &&
      player == g_affected_card_controller &&
      g_current_player == player &&
      player == g_current_turn)
  {
    if (event == EVENT_TRIGGER)
    {
      g_event_result |= RESOLVE_TRIGGER_MANDATORY;
    }
    if (event == EVENT_RESOLVE_TRIGGER || event == EVENT_SHOULD_AI_PLAY)
    {
      invalidate_dynamic_card_type(PLAYER_CARD_INSTANCE(player, card).dummy3);
      PLAYER_CARD_INSTANCE(player, card).dummy3 = -1;
      PLAYER_CARD_INSTANCE((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                           PLAYER_CARD_INSTANCE(player, card).damage_target_card)
          .regen_status |= 0x1000000;
      C_get_abilities((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                      PLAYER_CARD_INSTANCE(player, card).damage_target_card,
                      EVENT_CHANGE_TYPE,
                      -1);
      C_count_colors_of_lands_in_play();
      kill_card(player, card, KILL_BURY);
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x004f5053
// FUNCTION: SHANDALAR 0x004af111
int card_graveyard(int player, int card, event_t event)
{
  card_instance_t *instance;
  int amount;
  int blocked_group;
  int current_card;
  int current_player;
  int internal_card_id;
  int target_player;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (g_trigger_condition == TRIGGER_GRAVEYARD_FROM_PLAY &&
      g_affected_card == card &&
      player == g_affected_card_controller &&
      player == g_current_turn)
  {
    if (event == EVENT_TRIGGER)
    {
      g_event_result |= 2;
    }

    if (event == EVENT_RESOLVE_TRIGGER)
    {
      internal_card_id = find_internal_card_id_by_csv_id(0x200);
      if (instance->original_internal_card_id == internal_card_id)
      {
        if (player == g_active_player && g_duel_ai_mode_state == 1)
        {
          amount = 1;
        }
        else
        {
          amount = 2;
        }
        gain_life(player, amount);
      }

      internal_card_id = find_internal_card_id_by_csv_id(0xb5);
      if (instance->original_internal_card_id == internal_card_id &&
          g_life[(instance->token_status & STATUS_LEGACY_TYPECHANGE) >> 12] > -1)
      {
        g_life[(instance->token_status & STATUS_LEGACY_TYPECHANGE) >> 12] /= 2;
      }

      internal_card_id = find_internal_card_id_by_csv_id(0x32);
      if (instance->original_internal_card_id == internal_card_id)
      {
        damage_player((int)instance->damage_target_player, instance->info_slot, player, card);
      }

      internal_card_id = find_internal_card_id_by_csv_id(0x109);
      if (instance->original_internal_card_id == internal_card_id)
      {
        for (current_player = 0; current_player < 2; ++current_player)
        {
          damage_player(current_player, instance->info_slot, player, card);

          for (current_card = 0; current_card < g_active_cards_count[current_player]; ++current_card)
          {
            if (is_in_play(current_player, current_card) != 0 &&
                (global_cards_data[PLAYER_CARD_INSTANCE(current_player, current_card).internal_card_id].type &
                 TYPE_CREATURE) != 0)
            {
              damage_creature(current_player, current_card, instance->info_slot, player, card);
            }
          }
        }
        g_battlefield_extra_ability_flags |= 1;
      }

      internal_card_id = find_internal_card_id_by_csv_id(0x187);
      if (instance->original_internal_card_id == internal_card_id)
      {
        target_player = 1 - player;
        if (instance->info_slot == -2)
        {
          for (current_card = 0; current_card < g_active_cards_count[target_player]; ++current_card)
          {
            if (is_in_play(target_player, current_card) != 0 &&
                PLAYER_CARD_INSTANCE(target_player, current_card).blocking == instance->damage_source_card)
            {
              kill_card(target_player, current_card, KILL_BURY);
            }
          }
        }
        else
        {
          blocked_group = (int)PLAYER_CARD_INSTANCE(target_player, instance->info_slot).blocking;
          if (blocked_group == -1)
          {
            kill_card(target_player, instance->info_slot, KILL_BURY);
          }
          else
          {
            for (current_card = 0; current_card < g_active_cards_count[target_player]; ++current_card)
            {
              if (is_in_play(target_player, current_card) != 0 &&
                  PLAYER_CARD_INSTANCE(target_player, current_card).blocking == blocked_group)
              {
                kill_card(target_player, current_card, KILL_BURY);
              }
            }
          }
        }
        g_battlefield_extra_ability_flags |= 1;
      }

      kill_card(player, card, KILL_REMOVE);
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x004f56f2
// FUNCTION: SHANDALAR 0x004af7ac
int card_damage_legacy(int player, int card, event_t event)
{
  card_instance_t *damage;
  card_instance_t *instance;
  int amount;
  int current_card;
  int current_player;
  int total;
  int toughness;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (g_trigger_condition == TRIGGER_DEAL_DAMAGE &&
      g_affected_card == card &&
      g_affected_card_controller == player &&
      player == g_current_turn)
  {
    if (event == EVENT_TRIGGER)
    {
      g_event_result |= RESOLVE_TRIGGER_MANDATORY;
    }

    if (event == EVENT_RESOLVE_TRIGGER)
    {
      if (instance->original_internal_card_id == find_internal_card_id_by_csv_id(0x44) ||
          instance->original_internal_card_id == find_internal_card_id_by_csv_id(0x325))
      {
        total = 0;
        for (current_player = 0; current_player < 2; ++current_player)
        {
          for (current_card = 0; current_card < g_active_cards_count[current_player]; ++current_card)
          {
            damage = &PLAYER_CARD_INSTANCE(current_player, current_card);
            if (damage->internal_card_id == g_damage_card_internal_card_id &&
                instance->damage_source_player == damage->damage_source_player &&
                instance->damage_source_card == damage->damage_source_card)
            {
              if (damage->damage_target_card == -1)
              {
                amount = damage->info_slot;
                if (g_life[(int)damage->damage_target_player] < 0)
                {
                  amount += g_life[(int)damage->damage_target_player];
                }
              }
              else
              {
                toughness = C_get_abilities((int)damage->damage_target_player,
                                            damage->damage_target_card,
                                            EVENT_TOUGHNESS,
                                            -1);
                if (toughness < damage->info_slot)
                {
                  amount = C_get_abilities((int)damage->damage_target_player,
                                           damage->damage_target_card,
                                           EVENT_TOUGHNESS,
                                           -1);
                }
                else
                {
                  amount = damage->info_slot;
                }
              }

              if (amount < 0)
              {
                amount = 0;
              }
              total += amount;
            }
          }
        }
        gain_life(player, total, player, card);
      }
      kill_card(player, card, KILL_REMOVE);
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x004f5b91
// FUNCTION: SHANDALAR 0x004afc4a
int card_activation(int player, int card, event_t event)
{
  int result;

  if (event != EVENT_RESOLVE_ACTIVATION && event != EVENT_UPKEEP_COSTS_UNPAID &&
      event != EVENT_RESOLVE_TRIGGER)
  {
    return 0;
  }

  g_card_on_stack_controller = PLAYER_CARD_INSTANCE(player, card).parent_controller;
  g_card_on_stack = PLAYER_CARD_INSTANCE(player, card).parent_card;
  result = (*global_cards_data[PLAYER_CARD_INSTANCE(player, card).original_internal_card_id].code_pointer)(
      player, card, event);
  g_card_on_stack_controller = -1;
  g_card_on_stack = -1;

  return result;
}

// FUNCTION: MAGIC 0x004f5c93
// FUNCTION: SHANDALAR 0x004afd4c
int card_draw_card(int player, int card, event_t event)
{
  if (event == EVENT_CAN_ACTIVATE && g_current_phase == PHASE_DRAW && unk_00742f60 == player &&
      PLAYER_CARD_INSTANCE(player, card).info_slot == 0 && g_trigger_condition == -1)
  {
    unk_008b3270 |= 3;
    return 1;
  }

  if (event == EVENT_ACTIVATE)
  {
    PLAYER_CARD_INSTANCE(player, card).info_slot |= 1;
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    kill_card(g_card_on_stack_controller, g_card_on_stack, KILL_REMOVE);
    draw_card_for_player(player);
  }

  return 0;
}
