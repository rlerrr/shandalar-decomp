#include <stdio.h>
#include <string.h>
#include "../game_support.h"
#include "../global_strings.h"

// FUNCTION: MAGIC 0x004f5d83
// FUNCTION: SHANDALAR 0x004afe3b
int card_fork(int player, int card, event_t event)
{
  int new_card;

  if (event == EVENT_CAN_CAST)
  {
    load_recorded_action_target(0);
    if (g_current_spell_player == -1)
    {
      return 0;
    }
    PLAYER_CARD_INSTANCE(player, card).info_slot = g_x_value;
    if ((global_cards_data[PLAYER_CARD_INSTANCE(g_current_spell_player, g_current_spell_card).internal_card_id].type & (TYPE_INSTANT | TYPE_INTERRUPT)) == 0 || !C_real_validate_target(g_current_spell_player, g_current_spell_card, (char *)0, player, 2, 2, 0,
                                                                                                                                                                      TYPE_NONE, TYPE_NONE, 0, 0, COLOR_TEST_0, COLOR_TEST_0,
                                                                                                                                                                      -1, ~SUB_WALL, -1, -1, TARGET_SPECIAL_SPELL_ON_STACK, 0,
                                                                                                                                                                      0))
    {
      return 0;
    }
    return 99;
  }

  if (event == EVENT_CAST_SPELL && g_affected_card == card && g_affected_card_controller == player)
  {
    if (g_current_spell_player == -1)
    {
      g_spell_fizzled = 1;
    }
    else
    {
      PLAYER_CARD_INSTANCE(player, card).targets[0].player = g_current_spell_player;
      PLAYER_CARD_INSTANCE(player, card).targets[0].card = g_current_spell_card;
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    new_card = add_card_to_hand(player,
                                PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                                                     PLAYER_CARD_INSTANCE(player, card).targets[0].card)
                                    .internal_card_id);
    if (new_card != -1)
    {
      *(char *)((char *)&PLAYER_CARD_INSTANCE(player, new_card) + 0x1a) = 0x10;
      PLAYER_CARD_INSTANCE(player, new_card).token_status |= 8;
      g_x_value = PLAYER_CARD_INSTANCE(player, card).info_slot;
      g_land_can_be_played |= 0x400;
      process_card_enters_play(player, new_card);
      g_land_can_be_played &= ~0x400;
    }
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

int tap_mana_producing_land_callback(int player, int card, int internal_card_id);

// FUNCTION: MAGIC 0x004f6108
// FUNCTION: SHANDALAR 0x004b01c2
int card_mana_short(int player, int card, event_t event)
{
  int target_player;
  int color;

  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if (((event == EVENT_CAST_SPELL) && (card == g_affected_card)) && (player == g_affected_card_controller))
  {
    target_t selected_target;

    load_text("prompts.txt", "MANASHORT");
    if (!C_real_select_target(player,
                              2,
                              1 - player,
                              TARGET_ZONE_PLAYERS,
                              TYPE_NONE,
                              TYPE_NONE,
                              0,
                              0,
                              COLOR_TEST_0,
                              COLOR_TEST_0,
                              -1,
                              ~SUB_WALL,
                              -1,
                              -1,
                              0,
                              0,
                              0,
                              g_text_lines[0],
                              1,
                              &selected_target))
    {
      g_spell_fizzled = 1;
    }
    else
    {
      PLAYER_CARD_INSTANCE(player, card).targets[0] = selected_target;
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    target_player = PLAYER_CARD_INSTANCE(player, card).targets[0].player;
    dispatch_three_arg_callback_to_cards_in_play(tap_mana_producing_land_callback, target_player);
    for (color = 0; color < 8; ++color)
    {
      g_raw_mana_available[target_player][color] = 0;
    }
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004f6311
int tap_mana_producing_land_callback(int player, int card, int internal_card_id)
{
  unk_00938e2c = 1;
  g_produced_mana_color = -1;
  if ((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0 && (global_cards_data[internal_card_id].type & TYPE_LAND) != 0 && (global_cards_data[internal_card_id].extra_ability & 0x1000) != 0 && (PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0 && (global_cards_data[internal_card_id].type & TYPE_LAND) != 0)
  {
    dispatch_event_to_single_card(player, card, EVENT_ACTIVATE, 1 - player, -1);
    if ((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) != 0)
    {
      dispatch_event(player, card, EVENT_TAP_CARD);
    }
  }
  unk_00938e2c = 0;
  return 0;
}

// FUNCTION: MAGIC 0x004f6456
// FUNCTION: SHANDALAR 0x004b0512
int card_siren_s_call(int player, int card, event_t event)
{
  if (event == EVENT_CAN_CAST)
  {
    return (g_current_player != player && g_current_phase < PHASE_DECLARE_ATTACKERS) ? 1 : 0;
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    create_legacy_effect(player, card, unk_00789314, -1, -1);
    kill_card(player, card, KILL_DESTROY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004f64d5
// FUNCTION: SHANDALAR 0x004b0590
int card_ancestral_recall(int player, int card, event_t event)
{
  card_instance_t *instance;
  target_t selected_target;
  int target_player;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if (((event == EVENT_CAST_SPELL) && (g_affected_card == card)) && (g_affected_card_controller == player))
  {
    load_text("prompts.txt", "ANCESTRAL_RECALL");
    if (!C_real_select_target(player,
                              2,
                              player,
                              TARGET_ZONE_PLAYERS,
                              TYPE_NONE,
                              TYPE_NONE,
                              0,
                              0,
                              COLOR_TEST_0,
                              COLOR_TEST_0,
                              -1,
                              ~SUB_WALL,
                              -1,
                              -1,
                              0,
                              0,
                              0,
                              g_text_lines[0],
                              1,
                              &selected_target))
    {
      g_spell_fizzled = 1;
    }
    else
    {
      instance->targets[0] = selected_target;
      instance->number_of_targets = 1;
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    target_player = instance->targets[0].player;
    draw_card_for_player(target_player);
    draw_card_for_player(target_player);
    draw_card_for_player(target_player);
    instance->number_of_targets = 0;
    kill_card(player, card, KILL_DESTROY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004f66bc
// FUNCTION: SHANDALAR 0x004b0777
int card_simulacrum(int player, int card, event_t event)
{
  card_instance_t *damage;
  card_instance_t *instance;
  int current_card;
  int current_player;
  int target_card;
  int target_player;

  instance = &PLAYER_CARD_INSTANCE(player, card);
  if (event == EVENT_CAN_CAST)
  {
    load_recorded_action_target(0);
    if (!real_target_available((int *)0, TARGET_SCAN_DIRECT, player, player, player, 0x200, 2, 0, 0,
                               get_protections_from(player, card), 0, 0, -1, -1, 0xffffffff,
                               0xffffffff, 0, 0, 0))
    {
      return 0;
    }
    if ((g_land_can_be_played & 4) == 0)
    {
      return 1;
    }
    return real_target_available((int *)0, TARGET_SCAN_DIRECT, player, 2, 2, 0x200, 0, 0, 0, 0, 0, 0,
                                 g_damage_card_internal_card_id, -1, 0xffffffff, 0xffffffff, 0x20, 0, 0)
               ? 99
               : 0;
  }

  if ((event == EVENT_CAST_SPELL) && (card == g_card_on_stack) && (player == g_card_on_stack_controller))
  {
    load_text("prompts.txt", "SIMULACRUM");
    if (!select_target_creature_and_store(player, player, card))
    {
      g_spell_fizzled = 1;
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    target_player = instance->targets[0].player;
    target_card = instance->targets[0].card;

    if (!C_real_validate_target(target_player, target_card, (char *)0, player, player, player,
                                TARGET_ZONE_IN_PLAY, TYPE_CREATURE, TYPE_NONE, 0,
                                get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0, -1,
                                ~SUB_WALL, -1, -1, 0, 0, 0))
    {
      g_spell_fizzled = 1;
    }
    else
    {
      if ((g_land_can_be_played & 4) != 0)
      {
        instance->eot_toughness = 0;
        for (current_player = 0; current_player < 2; ++current_player)
        {
          for (current_card = 0; current_card < g_active_cards_count[current_player]; ++current_card)
          {
            damage = &PLAYER_CARD_INSTANCE(current_player, current_card);
            if (damage->internal_card_id == g_damage_card_internal_card_id && damage->damage_target_player == player && damage->damage_target_card == -1)
            {
              damage_creature(target_player, target_card, damage->info_slot,
                              (int)damage->damage_source_player, damage->damage_source_card);
              instance->eot_toughness += damage->info_slot;
              damage->info_slot = 0;
            }
          }
        }
      }

      instance->info_slot = 0;
      for (current_player = 0; current_player < 2; ++current_player)
      {
        for (current_card = 0; current_card < 151; ++current_card)
        {
          if (unk_0093b280[current_player][current_card][player][0] > 0)
          {
            damage_creature(target_player, target_card,
                            unk_0093b280[current_player][current_card][player][0],
                            current_player, current_card);
            instance->info_slot += unk_0093b280[current_player][current_card][player][0];
            unk_0093b280[current_player][current_card][player][0] = 0;
          }
        }
      }

      g_life[player] += instance->info_slot;
      instance->info_slot = 0;
    }

    instance->number_of_targets = 0;
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004f6d49
// FUNCTION: SHANDALAR 0x004b0e02
int card_shatter(int player, int card, event_t event)
{
  card_instance_t *instance;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_CAN_CAST)
  {
    load_recorded_action_target(0);
    return real_target_available((int *)0, TARGET_SCAN_DIRECT, player, 2, 2, TARGET_ZONE_IN_PLAY, TYPE_ARTIFACT, TYPE_NONE, 0, 0,
                                 COLOR_TEST_0, COLOR_TEST_0, -1, ~SUB_WALL, -1, -1, 0, 0, 0);
  }

  if (((event == EVENT_CAST_SPELL) && (g_affected_card == card)) && (g_affected_card_controller == player))
  {
    load_text("prompts.txt", "SHATTER");
    if (!C_real_select_target(player,
                              2,
                              1 - player,
                              TARGET_ZONE_IN_PLAY,
                              TYPE_ARTIFACT,
                              TYPE_NONE,
                              0,
                              get_protections_from(player, card),
                              COLOR_TEST_0,
                              COLOR_TEST_0,
                              -1,
                              ~SUB_WALL,
                              -1,
                              -1,
                              0,
                              0,
                              0,
                              g_text_lines[0],
                              1,
                              &instance->targets[0]))
    {
      g_spell_fizzled = 1;
    }
    else
    {
      g_ai_modifier -= 0x10;
      instance->number_of_targets = 1;
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    if (!C_real_validate_target(instance->targets[0].player,
                                instance->targets[0].card,
                                (char *)0,
                                player,
                                2,
                                2,
                                TARGET_ZONE_IN_PLAY,
                                TYPE_ARTIFACT,
                                TYPE_NONE,
                                0,
                                0,
                                COLOR_TEST_0,
                                COLOR_TEST_0,
                                -1,
                                ~SUB_WALL,
                                -1,
                                -1,
                                0,
                                0,
                                0))
    {
      g_spell_fizzled = 1;
    }
    else
    {
      kill_card(instance->targets[0].player, instance->targets[0].card, KILL_DESTROY);
    }
    instance->number_of_targets = 0;
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004f6f2a
// FUNCTION: SHANDALAR 0x004b0fe3
int card_disenchant(int player, int card, event_t event)
{
  card_instance_t *instance;
  target_t selected_target;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_CAN_CAST)
  {
    load_recorded_action_target(0);
    return real_target_available((int *)0,
                                 TARGET_SCAN_DIRECT,
                                 player,
                                 2,
                                 2,
                                 TARGET_ZONE_IN_PLAY,
                                 TYPE_ARTIFACT | TYPE_ENCHANTMENT,
                                 TYPE_NONE,
                                 0,
                                 get_protections_from(player, card),
                                 COLOR_TEST_0,
                                 COLOR_TEST_0,
                                 -1,
                                 ~SUB_WALL,
                                 -1,
                                 -1,
                                 0,
                                 0,
                                 0);
  }

  if (((event == EVENT_CAST_SPELL) && (g_affected_card == card)) && (g_affected_card_controller == player))
  {
    g_ai_modifier -= 0x10;
    load_text("prompts.txt", "DISENCHANT");
    if (!C_real_select_target(player,
                              2,
                              2,
                              TARGET_ZONE_IN_PLAY,
                              TYPE_ARTIFACT | TYPE_ENCHANTMENT,
                              TYPE_NONE,
                              0,
                              get_protections_from(player, card),
                              COLOR_TEST_0,
                              COLOR_TEST_0,
                              -1,
                              ~SUB_WALL,
                              -1,
                              -1,
                              0,
                              0,
                              0,
                              g_text_lines[0],
                              1,
                              &selected_target))
    {
      g_spell_fizzled = 1;
    }
    else
    {
      instance->targets[0] = selected_target;
      instance->number_of_targets = 1;
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    selected_target = instance->targets[0];
    if (!C_real_validate_target(selected_target.player,
                                selected_target.card,
                                (char *)0,
                                player,
                                2,
                                2,
                                TARGET_ZONE_IN_PLAY,
                                TYPE_ARTIFACT | TYPE_ENCHANTMENT,
                                TYPE_NONE,
                                0,
                                get_protections_from(player, card),
                                COLOR_TEST_0,
                                COLOR_TEST_0,
                                -1,
                                ~SUB_WALL,
                                -1,
                                -1,
                                0,
                                0,
                                0))
    {
      g_spell_fizzled = 1;
    }
    else
    {
      kill_card(selected_target.player, selected_target.card, KILL_DESTROY);
    }
    instance->number_of_targets = 0;
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004f71db
// FUNCTION: SHANDALAR 0x004b1292
int card_twiddle(int player, int card, event_t event)
{
  card_instance_t *instance;
  target_t target;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_CAN_CAST)
  {
    load_recorded_action_target(0);
    return real_target_available((int *)0,
                                 TARGET_SCAN_DIRECT,
                                 player,
                                 2,
                                 2,
                                 TARGET_ZONE_IN_PLAY,
                                 TYPE_ARTIFACT | TYPE_CREATURE | TYPE_LAND,
                                 TYPE_NONE,
                                 0,
                                 get_protections_from(player, card),
                                 COLOR_TEST_0,
                                 COLOR_TEST_0,
                                 -1,
                                 ~SUB_WALL,
                                 -1,
                                 -1,
                                 0,
                                 0,
                                 0);
  }

  if (event == 0x6c && g_affected_card == card && g_affected_card_controller == player)
  {
    g_ai_modifier -= 0x30;
    load_text("prompts.txt", "TWIDDLE");
    if (!C_real_select_target(player,
                              2,
                              2,
                              TARGET_ZONE_IN_PLAY,
                              TYPE_ARTIFACT | TYPE_CREATURE | TYPE_LAND,
                              TYPE_NONE,
                              0,
                              get_protections_from(player, card),
                              COLOR_TEST_0,
                              COLOR_TEST_0,
                              -1,
                              ~SUB_WALL,
                              -1,
                              -1,
                              0,
                              0,
                              0,
                              g_text_lines[0],
                              1,
                              &target))
    {
      g_spell_fizzled = 1;
    }
    else
    {
      instance->targets[0] = target;
      *((char *)instance + 0x32) = 1;
      if ((g_land_can_be_played & 0x400) == 0)
      {
        instance->info_slot = (PLAYER_CARD_INSTANCE(target.player, target.card).state & STATE_TAPPED) != 0;
      }
      else
      {
        instance->info_slot = PLAYER_CARD_INSTANCE(g_current_spell_player, g_current_spell_card).info_slot;
      }
      if (player == g_other_player)
      {
        if ((global_cards_data[PLAYER_CARD_INSTANCE(target.player, target.card).internal_card_id].type & TYPE_LAND) != 0)
        {
          g_ai_modifier -= 0x18;
        }
        if ((instance->info_slot == 0 && target.player == g_other_player) || (instance->info_slot == 1 && target.player == g_active_player))
        {
          g_ai_modifier -= 0x60;
        }
      }
    }
    return 0;
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    if (!C_real_validate_target(instance->targets[0].player,
                                instance->targets[0].card,
                                (char *)0,
                                player,
                                2,
                                2,
                                TARGET_ZONE_IN_PLAY,
                                TYPE_ARTIFACT | TYPE_CREATURE | TYPE_LAND,
                                TYPE_NONE,
                                0,
                                get_protections_from(player, card),
                                COLOR_TEST_0,
                                COLOR_TEST_0,
                                -1,
                                ~SUB_WALL,
                                -1,
                                -1,
                                0,
                                0,
                                0))
    {
      g_spell_fizzled = 1;
    }
    else if (instance->info_slot == 0)
    {
      tap_card_and_dispatch_event(instance->targets[0].player, instance->targets[0].card);
    }
    else
    {
      PLAYER_CARD_INSTANCE(instance->targets[0].player, instance->targets[0].card).state &= ~STATE_TAPPED;
    }
    *((char *)instance + 0x32) = 0;
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004f7783
// FUNCTION: SHANDALAR 0x004b183c
int tap_card_and_dispatch_event(int player, int card)
{
  if ((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0)
  {
    PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
    if ((global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].extra_ability & 0x1000) != 0)
    {
      g_produced_mana_color = -1;
    }
    dispatch_event(player, card, EVENT_TAP_CARD);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004f7857
// FUNCTION: SHANDALAR 0x004b1910
int card_tunnel(int player, int card, event_t event)
{
  card_instance_t *instance;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_CAN_CAST)
  {
    load_recorded_action_target(0);
    return real_target_available((int *)0,
                                 TARGET_SCAN_DIRECT,
                                 player,
                                 2,
                                 2,
                                 TARGET_ZONE_IN_PLAY,
                                 TYPE_CREATURE,
                                 TYPE_NONE,
                                 0,
                                 get_protections_from(player, card),
                                 COLOR_TEST_0,
                                 COLOR_TEST_0,
                                 -1,
                                 ~SUB_WALL,
                                 -1,
                                 -1,
                                 TARGET_SPECIAL_WALL,
                                 0,
                                 0);
  }

  if (((event == EVENT_CAST_SPELL) && (card == g_affected_card)) && (player == g_affected_card_controller))
  {
    load_text("prompts.txt", "TUNNEL");
    if (!C_real_select_target(player,
                              2,
                              player,
                              TARGET_ZONE_IN_PLAY,
                              TYPE_CREATURE,
                              TYPE_NONE,
                              0,
                              get_protections_from(player, card),
                              COLOR_TEST_0,
                              COLOR_TEST_0,
                              -1,
                              ~SUB_WALL,
                              -1,
                              -1,
                              TARGET_SPECIAL_WALL,
                              0,
                              0,
                              g_text_lines[0],
                              1,
                              &instance->targets[0]))
    {
      g_spell_fizzled = 1;
    }
    else
    {
      instance->number_of_targets = 1;
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    if (!C_real_validate_target(instance->targets[0].player,
                                instance->targets[0].card,
                                (char *)0,
                                player,
                                2,
                                2,
                                TARGET_ZONE_IN_PLAY,
                                TYPE_CREATURE,
                                TYPE_NONE,
                                0,
                                get_protections_from(player, card),
                                COLOR_TEST_0,
                                COLOR_TEST_0,
                                -1,
                                ~SUB_WALL,
                                -1,
                                -1,
                                TARGET_SPECIAL_WALL,
                                0,
                                0))
    {
      g_spell_fizzled = 1;
    }
    else
    {
      kill_card(instance->targets[0].player, instance->targets[0].card, KILL_BURY);
    }
    instance->number_of_targets = 0;
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004f7b10
// FUNCTION: SHANDALAR 0x004b1bcb
int card_marsh_gas(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x004f7c84
// FUNCTION: SHANDALAR 0x004b1d3f
int card_howl_from_beyond(int player, int card, event_t event)
{
  card_instance_t *instance;
  int effect_card;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_CAN_CAST)
  {
    if (((player == g_other_player) && ((g_duel_network_flags & 2) == 0)) && has_mana(player, 7, 2) == 0)
    {
      return 0;
    }
    load_recorded_action_target(0);
    return real_target_available((int *)0,
                                 TARGET_SCAN_DIRECT,
                                 player,
                                 2,
                                 (unsigned char)player,
                                 TARGET_ZONE_IN_PLAY,
                                 TYPE_CREATURE,
                                 TYPE_NONE,
                                 0,
                                 get_protections_from(player, card),
                                 COLOR_TEST_0,
                                 COLOR_TEST_0,
                                 -1,
                                 ~SUB_WALL,
                                 -1,
                                 -1,
                                 0,
                                 0,
                                 0);
  }

  if (((event == EVENT_CAST_SPELL) && (card == g_affected_card)) && (player == g_affected_card_controller))
  {
    load_text("prompts.txt", "HOWL_FROM_BEYOND");
    if (!select_target_creature_and_store(player, player, card))
    {
      g_spell_fizzled = 1;
    }
    else
    {
      g_ai_modifier += ((((g_current_phase < 0x15) - 1) & -2) * 3 + 9) * -4;
      instance->info_slot = g_x_value;
      if ((player == g_other_player) && (PLAYER_CARD_INSTANCE(instance->targets[0].player, instance->targets[0].card).state & 0x30000) != 0)
      {
        g_ai_modifier += -99;
      }
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    if (!C_real_validate_target(instance->targets[0].player,
                                instance->targets[0].card,
                                (char *)0,
                                player,
                                2,
                                2,
                                TARGET_ZONE_IN_PLAY,
                                TYPE_CREATURE,
                                TYPE_NONE,
                                0,
                                get_protections_from(player, card),
                                COLOR_TEST_0,
                                COLOR_TEST_0,
                                -1,
                                ~SUB_WALL,
                                -1,
                                -1,
                                0,
                                0,
                                0))
    {
      g_spell_fizzled = 1;
    }
    else
    {
      effect_card =
          create_legacy_effect(player, card, LEGACY_EFFECT_PUMP, instance->targets[0].player, instance->targets[0].card);
      if (effect_card != -1)
      {
        PLAYER_CARD_INSTANCE(player, effect_card).counter_power = (short)instance->info_slot;
      }
    }
    instance->number_of_targets = 0;
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004f801d
// FUNCTION: SHANDALAR 0x004b20dc
int card_berserk(int player, int card, event_t event)
{
  card_instance_t *instance;
  int effect_card;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_CAN_CAST)
  {
    load_recorded_action_target(0);
    if (g_current_phase < 0x1e && real_target_available((int *)0,
                                                      TARGET_SCAN_DIRECT,
                                                      player,
                                                      2,
                                                      2,
                                                      TARGET_ZONE_IN_PLAY,
                                                      TYPE_CREATURE,
                                                      TYPE_NONE,
                                                      0,
                                                      get_protections_from(player, card),
                                                      COLOR_TEST_0,
                                                      COLOR_TEST_0,
                                                      -1,
                                                      ~SUB_WALL,
                                                      -1,
                                                      -1,
                                                      0,
                                                      0,
                                                      0) != 0)
    {
      return 1;
    }
    return 0;
  }

  if (((event == EVENT_CAST_SPELL) && (card == g_affected_card)) && (player == g_affected_card_controller))
  {
    load_text("prompts.txt", "BERSERK");
    if (!select_target_creature_and_store(player, player, card))
    {
      g_spell_fizzled = 1;
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    if (!C_real_validate_target(instance->targets[0].player,
                                instance->targets[0].card,
                                (char *)0,
                                player,
                                2,
                                2,
                                TARGET_ZONE_IN_PLAY,
                                TYPE_CREATURE,
                                TYPE_NONE,
                                0,
                                get_protections_from(player, card),
                                COLOR_TEST_0,
                                COLOR_TEST_0,
                                -1,
                                ~SUB_WALL,
                                -1,
                                -1,
                                0,
                                0,
                                0))
    {
      g_spell_fizzled = 1;
    }
    else
    {
      effect_card =
          create_legacy_effect(player, card, unk_008b49c4, instance->targets[0].player, instance->targets[0].card);
      if (effect_card != -1)
      {
        PLAYER_CARD_INSTANCE(player, effect_card).info_slot = 0x80;
        PLAYER_CARD_INSTANCE(player, effect_card).counter_power =
            PLAYER_CARD_INSTANCE(instance->targets[0].player, instance->targets[0].card).power;
        PLAYER_CARD_INSTANCE(player, effect_card).state |= 0x4000;
      }
    }
    instance->number_of_targets = 0;
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004f8305
// FUNCTION: SHANDALAR 0x004b23c6
int card_righteousness(int player, int card, event_t event)
{
  card_instance_t *instance;
  target_t selected_target;
  int effect_card;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_CAN_CAST)
  {
    load_recorded_action_target(0);
    return real_target_available((int *)0,
                                 TARGET_SCAN_DIRECT,
                                 player,
                                 2,
                                 2,
                                 TARGET_ZONE_IN_PLAY,
                                 TYPE_CREATURE,
                                 TYPE_NONE,
                                 0,
                                 get_protections_from(player, card),
                                 COLOR_TEST_0,
                                 COLOR_TEST_0,
                                 -1,
                                 ~SUB_WALL,
                                 -1,
                                 -1,
                                 0,
                                 TARGET_STATE_BLOCKING,
                                 0);
  }

  if (((event == EVENT_CAST_SPELL) && (card == g_affected_card)) && (player == g_affected_card_controller))
  {
    load_text("prompts.txt", "RIGHTEOUSNESS");
    if (!C_real_select_target(player,
                              2,
                              player,
                              TARGET_ZONE_IN_PLAY,
                              TYPE_CREATURE,
                              TYPE_NONE,
                              0,
                              get_protections_from(player, card),
                              COLOR_TEST_0,
                              COLOR_TEST_0,
                              -1,
                              ~SUB_WALL,
                              -1,
                              -1,
                              0,
                              TARGET_STATE_BLOCKING,
                              0,
                              g_text_lines[0],
                              1,
                              &selected_target))
    {
      g_spell_fizzled = 1;
    }
    else
    {
      instance->targets[0] = selected_target;
      instance->number_of_targets = 1;
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    selected_target = instance->targets[0];
    if (!C_real_validate_target(selected_target.player,
                                selected_target.card,
                                (char *)0,
                                player,
                                2,
                                2,
                                TARGET_ZONE_IN_PLAY,
                                TYPE_CREATURE,
                                TYPE_NONE,
                                0,
                                get_protections_from(player, card),
                                COLOR_TEST_0,
                                COLOR_TEST_0,
                                -1,
                                ~SUB_WALL,
                                -1,
                                -1,
                                0,
                                TARGET_STATE_BLOCKING,
                                0))
    {
      g_spell_fizzled = 1;
    }
    else
    {
      effect_card = create_legacy_effect(player, card, LEGACY_EFFECT_PUMP, selected_target.player, selected_target.card);
      if (effect_card != -1)
      {
        PLAYER_CARD_INSTANCE(player, effect_card).counter_power = 7;
        PLAYER_CARD_INSTANCE(player, effect_card).power = 7;
      }
    }
    instance->number_of_targets = 0;
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004f8639
// FUNCTION: SHANDALAR 0x004b26fc
int card_blood_lust(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x004f8901
// FUNCTION: SHANDALAR 0x004b29c6
int card_fog(int player, int card, event_t event)
{
  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    create_legacy_effect(player, card, unk_007894a0, -1, -1);
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004f895d
// FUNCTION: SHANDALAR 0x004b2a22
int card_swords_to_plowshares(int player, int card, event_t event)
{
  card_instance_t *instance;
  int target_player;
  int target_card;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_CAN_CAST)
  {
    load_recorded_action_target(0);
    return real_target_available((int *)0,
                                 TARGET_SCAN_DIRECT,
                                 player,
                                 2,
                                 2,
                                 TARGET_ZONE_IN_PLAY,
                                 TYPE_CREATURE,
                                 TYPE_NONE,
                                 0,
                                 get_protections_from(player, card),
                                 COLOR_TEST_0,
                                 COLOR_TEST_0,
                                 -1,
                                 ~SUB_WALL,
                                 -1,
                                 -1,
                                 0,
                                 0,
                                 0);
  }

  if (((event == EVENT_CAST_SPELL) && (card == g_affected_card)) && (player == g_affected_card_controller))
  {
    load_text("prompts.txt", "SWORD_TO_PLOWSHARES");
    if (!select_target_creature_and_store(player, 1 - player, card))
    {
      g_spell_fizzled = 1;
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    target_player = instance->targets[0].player;
    target_card = instance->targets[0].card;
    if (!C_real_validate_target(target_player,
                                target_card,
                                (char *)0,
                                player,
                                2,
                                2,
                                TARGET_ZONE_IN_PLAY,
                                TYPE_CREATURE,
                                TYPE_NONE,
                                0,
                                get_protections_from(player, card),
                                COLOR_TEST_0,
                                COLOR_TEST_0,
                                -1,
                                ~SUB_WALL,
                                -1,
                                -1,
                                0,
                                0,
                                0))
    {
      g_spell_fizzled = 1;
    }
    else
    {
      gain_life(target_player, C_get_abilities(target_player, target_card, EVENT_POWER, -1), player, card);
      kill_card(target_player, target_card, KILL_REMOVE);
    }
    instance->number_of_targets = 0;
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004f8b77
// FUNCTION: SHANDALAR 0x004b2c3e
int card_death_ward(int player, int card, event_t event)
{
  struct
  {
    int found_dead_creature;
    int current_player;
    int current_card;
    int internal_card_id;
    int target_player;
  } s;

  if (event == EVENT_CAN_CAST && (g_land_can_be_played & 0x200) != 0)
  {
    s.found_dead_creature = 0;

    load_recorded_action_target(0);
    for (s.current_player = 0; s.current_player < 2; ++s.current_player)
    {
      for (s.current_card = 0; s.current_card < g_active_cards_count[s.current_player]; ++s.current_card)
      {
        if (s.found_dead_creature == 0)
        {
          s.internal_card_id = PLAYER_CARD_INSTANCE(s.current_player, s.current_card).internal_card_id;
          if (is_in_play(s.current_player, s.current_card) &&
              (global_cards_data[s.internal_card_id].type & TYPE_CREATURE) != 0 &&
              (int)(char)PLAYER_CARD_INSTANCE(s.current_player, s.current_card).kill_code == 2)
          {
            s.found_dead_creature = 1;
          }
        }
      }
    }

    if (s.found_dead_creature != 0)
    {
      return 99;
    }
    return 0;
  }

  if (event == EVENT_CAST_SPELL && card == g_affected_card && player == g_affected_card_controller && (g_land_can_be_played & 0x200) != 0)
  {
    s.found_dead_creature = 0;
    do
    {
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
      load_text("prompts.txt", "DEATH_WARD");
      if (!select_target_creature_and_store(player, -1, card))
      {
        g_spell_fizzled = 1;
      }

      s.target_player = PLAYER_CARD_INSTANCE(player, card).targets[0].player;
      s.current_card = PLAYER_CARD_INSTANCE(player, card).targets[0].card;
      if ((int)(char)PLAYER_CARD_INSTANCE(s.target_player, s.current_card).kill_code == 2)
      {
        if (((PLAYER_CARD_INSTANCE(s.target_player, s.current_card).token_status & 2) != 0) || s.target_player == g_active_player)
        {
          g_ai_modifier -= 0x30;
        }
        s.found_dead_creature = 1;
      }
      else if (g_duel_ai_mode_state == 1)
      {
        rewind_recorded_action();
      }
      else
      {
        load_text("prompts.txt", "DEATH_WARD2");
        set_duel_prompt_text(g_text_lines[1]);
        Sleep(0x9c4);
        set_duel_prompt_text("");
      }
    } while ((g_spell_fizzled != 1) && s.found_dead_creature == 0);
  }

  if (event == EVENT_RESOLVE_SPELL && (g_land_can_be_played & 0x200) != 0)
  {
    s.target_player = PLAYER_CARD_INSTANCE(player, card).targets[0].player;
    s.current_card = PLAYER_CARD_INSTANCE(player, card).targets[0].card;
    if (!C_real_validate_target(s.target_player,
                                s.current_card,
                                (char *)0,
                                player,
                                2,
                                2,
                                TARGET_ZONE_IN_PLAY,
                                TYPE_CREATURE,
                                TYPE_NONE,
                                0,
                                0,
                                COLOR_TEST_0,
                                COLOR_TEST_0,
                                -1,
                                ~SUB_WALL,
                                -1,
                                -1,
                                0,
                                0,
                                0))
    {
      g_spell_fizzled = 1;
    }
    else
    {
      regenerate_card(s.target_player, s.current_card);
    }
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004f9092
// FUNCTION: SHANDALAR 0x004b315b
int card_hurkyl_s_recall(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x004f94f3
// FUNCTION: SHANDALAR 0x004b35bf
int card_jump(int player, int card, event_t event)
{
  card_instance_t *instance;
  int target_player;
  int target_card;
  int effect_card;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_CAN_CAST)
  {
    load_recorded_action_target(0);
    return real_target_available((int *)0,
                                 TARGET_SCAN_DIRECT,
                                 player,
                                 2,
                                 2,
                                 TARGET_ZONE_IN_PLAY,
                                 TYPE_CREATURE,
                                 TYPE_NONE,
                                 0,
                                 get_protections_from(player, card),
                                 COLOR_TEST_0,
                                 COLOR_TEST_0,
                                 -1,
                                 ~SUB_WALL,
                                 -1,
                                 -1,
                                 0,
                                 0,
                                 0);
  }

  if (((event == EVENT_CAST_SPELL) && (card == g_affected_card)) && (player == g_affected_card_controller))
  {
    load_text("prompts.txt", "JUMP");
    if (!select_target_creature_and_store(player, player, card))
    {
      g_spell_fizzled = 1;
    }
    else
    {
      if (instance->targets[0].player == g_active_player)
      {
        g_ai_modifier -= 0x18;
      }
      if ((PLAYER_CARD_INSTANCE(instance->targets[0].player, instance->targets[0].card).regen_status & 0x20) != 0)
      {
        g_ai_modifier += -99;
      }
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    target_player = instance->targets[0].player;
    target_card = instance->targets[0].card;
    if (!C_real_validate_target(target_player,
                                target_card,
                                (char *)0,
                                player,
                                2,
                                2,
                                TARGET_ZONE_IN_PLAY,
                                TYPE_CREATURE,
                                TYPE_NONE,
                                0,
                                get_protections_from(player, card),
                                COLOR_TEST_0,
                                COLOR_TEST_0,
                                -1,
                                ~SUB_WALL,
                                -1,
                                -1,
                                0,
                                0,
                                0))
    {
      g_spell_fizzled = 1;
    }
    else
    {
      effect_card = create_legacy_effect(player, card, unk_00896534, target_player, target_card);
      if (effect_card != -1)
      {
        PLAYER_CARD_INSTANCE(player, effect_card).token_status = 0;
        PLAYER_CARD_INSTANCE(player, effect_card).regen_status = 0x20;
      }
    }
    instance->number_of_targets = 0;
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004f9834
// FUNCTION: SHANDALAR 0x004b3902
int card_morale(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x004f99c8
// FUNCTION: SHANDALAR 0x004b3a96
int card_piety(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x004f9b91
// FUNCTION: SHANDALAR 0x004b3c5f
int card_terror(int player, int card, event_t event)
{
  card_instance_t *instance;
  int illegal_color;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_CAN_CAST)
  {
    load_recorded_action_target(0);
    illegal_color = 1 << ((unsigned char)get_sleighted_color(player, card, 1) & 0x1f);
    return real_target_available((int *)0,
                                 TARGET_SCAN_DIRECT,
                                 player,
                                 2,
                                 2,
                                 TARGET_ZONE_IN_PLAY,
                                 TYPE_CREATURE,
                                 TYPE_ARTIFACT,
                                 0,
                                 get_protections_from(player, card),
                                 COLOR_TEST_0,
                                 illegal_color,
                                 -1,
                                 ~SUB_WALL,
                                 -1,
                                 -1,
                                 0,
                                 0,
                                 0);
  }

  if (((event == EVENT_CAST_SPELL) && (g_affected_card == card)) && (player == g_affected_card_controller))
  {
    load_text("prompts.txt", "TERROR");
    illegal_color = 1 << ((unsigned char)get_sleighted_color(player, card, 1) & 0x1f);
    if (!C_real_select_target(player,
                              2,
                              1 - player,
                              TARGET_ZONE_IN_PLAY,
                              TYPE_CREATURE,
                              TYPE_ARTIFACT,
                              0,
                              get_protections_from(player, card),
                              COLOR_TEST_0,
                              illegal_color,
                              -1,
                              ~SUB_WALL,
                              -1,
                              -1,
                              0,
                              0,
                              0,
                              g_text_lines[0],
                              1,
                              &instance->targets[0]))
    {
      g_spell_fizzled = 1;
    }
    else
    {
      instance->number_of_targets = 1;
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    illegal_color = 1 << ((unsigned char)get_sleighted_color(player, card, 1) & 0x1f);
    if (!C_real_validate_target(instance->targets[0].player,
                                instance->targets[0].card,
                                (char *)0,
                                player,
                                2,
                                2,
                                TARGET_ZONE_IN_PLAY,
                                TYPE_CREATURE,
                                TYPE_ARTIFACT,
                                0,
                                get_protections_from(player, card),
                                COLOR_TEST_0,
                                illegal_color,
                                -1,
                                ~SUB_WALL,
                                -1,
                                -1,
                                0,
                                0,
                                0))
    {
      g_spell_fizzled = 1;
    }
    else
    {
      kill_card(instance->targets[0].player, instance->targets[0].card, KILL_BURY);
    }
    instance->number_of_targets = 0;
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004f9e9e
// FUNCTION: SHANDALAR 0x004b3f6b
int card_lightning_bolt(int player, int card, event_t event)
{
  if (event == EVENT_CAN_CAST)
  {
    load_recorded_action_target(1);
    return 1;
  }

  if (((event == EVENT_CAST_SPELL) && (card == g_affected_card)) && (player == g_affected_card_controller))
  {
    load_text("prompts.txt", "LIGHTNING_BOLT");
    select_damage_target(player, card, 3);
    if (g_spell_fizzled != 1)
    {
      g_ai_modifier -= 0x24;
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    deal_damage_to_selected_target(player, card, EVENT_RESOLVE_SPELL, 3);
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004f9f59
// FUNCTION: SHANDALAR 0x004b4028
int card_crumble(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x004fa213
// FUNCTION: SHANDALAR 0x004b42e2
int card_giant_growth(int player, int card, event_t event)
{
  card_instance_t *instance;
  int effect_card;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_CAN_CAST)
  {
    load_recorded_action_target(0);
    return real_target_available((int *)0,
                                 TARGET_SCAN_DIRECT,
                                 player,
                                 2,
                                 2,
                                 TARGET_ZONE_IN_PLAY,
                                 TYPE_CREATURE,
                                 TYPE_NONE,
                                 0,
                                 get_protections_from(player, card),
                                 COLOR_TEST_0,
                                 COLOR_TEST_0,
                                 -1,
                                 ~SUB_WALL,
                                 -1,
                                 -1,
                                 0,
                                 0,
                                 0);
  }

  if (((event == EVENT_CAST_SPELL) && (g_affected_card == card)) && (g_affected_card_controller == player))
  {
    g_ai_modifier += ((((g_current_phase < 0x15) - 1) & -2) * 3 + 0xc) * -4;
    if (g_current_phase < 0x15)
    {
      g_ai_modifier -= 0xc;
    }
    load_text("prompts.txt", "GIANT_GROWTH");
    if (!select_target_creature_and_store(player, player, card))
    {
      g_spell_fizzled = 1;
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    if (!C_real_validate_target(instance->targets[0].player,
                                instance->targets[0].card,
                                (char *)0,
                                player,
                                2,
                                2,
                                TARGET_ZONE_IN_PLAY,
                                TYPE_CREATURE,
                                TYPE_NONE,
                                0,
                                get_protections_from(player, card),
                                COLOR_TEST_0,
                                COLOR_TEST_0,
                                -1,
                                ~SUB_WALL,
                                -1,
                                -1,
                                0,
                                0,
                                0))
    {
      g_spell_fizzled = 1;
    }
    else
    {
      effect_card =
          create_legacy_effect(player, card, LEGACY_EFFECT_PUMP, instance->targets[0].player, instance->targets[0].card);
      if (effect_card != -1)
      {
        PLAYER_CARD_INSTANCE(player, effect_card).counter_power = 3;
        PLAYER_CARD_INSTANCE(player, effect_card).power = 3;
      }
    }
    instance->number_of_targets = 0;
    kill_card(player, card, KILL_BURY);
  }

  if ((event == 0x3b) && has_mana(player, 3, 1) != 0)
  {
    unk_007a7d80[player] += 3;
    unk_007a7d88[player] += 3;
  }

  return 0;
}

// FUNCTION: MAGIC 0x004fa4eb
// FUNCTION: SHANDALAR 0x004b45ba
int card_unsummon(int player, int card, event_t event)
{
  card_instance_t *instance;
  int target_player;
  int target_card;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_CAN_CAST)
  {
    load_recorded_action_target(0);
    return real_target_available((int *)0,
                                 TARGET_SCAN_DIRECT,
                                 player,
                                 2,
                                 2,
                                 TARGET_ZONE_IN_PLAY,
                                 TYPE_CREATURE,
                                 TYPE_NONE,
                                 0,
                                 get_protections_from(player, card),
                                 COLOR_TEST_0,
                                 COLOR_TEST_0,
                                 -1,
                                 ~SUB_WALL,
                                 -1,
                                 -1,
                                 0,
                                 0,
                                 0);
  }

  if (((event == EVENT_CAST_SPELL) && (g_affected_card == card)) && (g_affected_card_controller == player))
  {
    g_ai_modifier -= 0x30;
    load_text("prompts.txt", "UNSUMMON");
    if (!select_target_creature_and_store(player, 1 - player, card))
    {
      g_spell_fizzled = 1;
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    target_player = instance->targets[0].player;
    target_card = instance->targets[0].card;
    if (!C_real_validate_target(target_player,
                                target_card,
                                (char *)0,
                                player,
                                2,
                                2,
                                TARGET_ZONE_IN_PLAY,
                                TYPE_CREATURE,
                                TYPE_NONE,
                                0,
                                get_protections_from(player, card),
                                COLOR_TEST_0,
                                COLOR_TEST_0,
                                -1,
                                ~SUB_WALL,
                                -1,
                                -1,
                                0,
                                0,
                                0))
    {
      g_spell_fizzled = 1;
    }
    else
    {
      kill_card(target_player, target_card, 5);
    }
    instance->number_of_targets = 0;
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004fa77b
static int __cdecl sandstorm_damage_attacking_creature(int source_player,
                                                       int source_card,
                                                       int player,
                                                       int card,
                                                       int internal_card_id)
{
  (void)internal_card_id;

  if ((PLAYER_CARD_INSTANCE(player, card).state & STATE_ATTACKING) != 0)
  {
    damage_creature(player, card, 1, source_player, source_card);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004fa6e3
// FUNCTION: SHANDALAR 0x004b47b1
int card_sandstorm(int player, int card, event_t event)
{
  if (event == EVENT_CAN_CAST)
  {
    if (*(&player) == g_active_player)
    {
      return 1;
    }

    if ((g_duel_network_flags & 2) != 0)
    {
      return 1;
    }

    if (*(&player) == g_current_player)
    {
      return 0;
    }

    return 1;
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    dispatch_function_to_all_cards_in_play(player, card, sandstorm_damage_attacking_creature, -1);
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004fa7dc
// FUNCTION: SHANDALAR 0x004b48ac
int card_purelace(int player, int card, event_t event)
{
  card_instance_t *instance;
  card_instance_t *target_instance;
  target_t target;
  int color;
  int valid;

  instance = &PLAYER_CARD_INSTANCE(player, card);
  if (event == EVENT_CAN_CAST)
  {
    if (g_current_spell_player == -1)
    {
      load_recorded_action_target(0);
      return real_target_available((int *)0, TARGET_SCAN_DIRECT, player, 2, 2, 0x200, 0xff, 0, 0,
                                   get_protections_from(player, card), 0, 0, -1, -1, 0xffffffff,
                                   0xffffffff, 0, 0, 0);
    }

    if ((((player == g_active_player) || ((g_duel_network_flags & 2) != 0)) || ((g_other_player == player) && (g_active_player == g_current_player))) && C_real_validate_target(g_current_spell_player, g_current_spell_card, (char *)0, player, 2, 2, 0,
                                                                                                                                                             TYPE_EFFECT | TYPE_ARTIFACT | TYPE_INTERRUPT | TYPE_INSTANT | TYPE_SORCERY |
                                                                                                                                                                 TYPE_ENCHANTMENT | TYPE_CREATURE | TYPE_LAND,
                                                                                                                                                             TYPE_NONE, 0, 0, COLOR_TEST_0, COLOR_TEST_0, -1,
                                                                                                                                                             ~SUB_WALL, -1, -1, TARGET_SPECIAL_SPELL_ON_STACK, 0, 0))
    {
      return 99;
    }
    return 0;
  }

  if ((event == EVENT_CAST_SPELL) && (card == g_card_on_stack) && (player == g_card_on_stack_controller))
  {
    g_ai_modifier -= 0x18;
    if (g_current_spell_player == -1)
    {
      load_text("prompts.txt", "ANY_LACE");
      if (!C_real_select_target(player, 2, 2, TARGET_ZONE_IN_PLAY,
                                TYPE_EFFECT | TYPE_ARTIFACT | TYPE_INTERRUPT | TYPE_INSTANT | TYPE_SORCERY |
                                    TYPE_ENCHANTMENT | TYPE_CREATURE | TYPE_LAND,
                                TYPE_NONE, 0, get_protections_from(player, card), COLOR_TEST_0,
                                COLOR_TEST_0, -1, ~SUB_WALL, -1, -1, 0, 0, 0,
                                g_text_lines[0], 1, &target))
      {
        g_spell_fizzled = 1;
      }
      else
      {
        instance->targets[0].player = target.player;
        instance->targets[0].card = target.card;
        instance->number_of_targets = 1;
        if ((g_other_player == player) && (((global_cards_data[instance->internal_card_id].color & PLAYER_CARD_INSTANCE(target.player, target.card).color) != 0) || (target.player == g_active_player)))
        {
          g_ai_modifier -= 0x30;
        }
      }
    }
    else
    {
      instance->targets[0].player = g_current_spell_player;
      instance->targets[0].card = g_current_spell_card;
      instance->number_of_targets = 1;
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    if (g_current_spell_player == -1)
    {
      valid = C_real_validate_target(instance->targets[0].player, instance->targets[0].card,
                                     (char *)0, player, 2, 2, TARGET_ZONE_IN_PLAY,
                                     TYPE_EFFECT | TYPE_ARTIFACT | TYPE_INTERRUPT | TYPE_INSTANT |
                                         TYPE_SORCERY | TYPE_ENCHANTMENT | TYPE_CREATURE | TYPE_LAND,
                                     TYPE_NONE, 0, get_protections_from(player, card), COLOR_TEST_0,
                                     COLOR_TEST_0, -1, ~SUB_WALL, -1, -1, 0, 0, 0);
    }
    else
    {
      valid = C_real_validate_target(instance->targets[0].player, instance->targets[0].card,
                                     (char *)0, player, 2, 2, 0,
                                     TYPE_EFFECT | TYPE_ARTIFACT | TYPE_INTERRUPT | TYPE_INSTANT |
                                         TYPE_SORCERY | TYPE_ENCHANTMENT | TYPE_CREATURE | TYPE_LAND,
                                     TYPE_NONE, 0, 0, COLOR_TEST_0, COLOR_TEST_0, -1,
                                     ~SUB_WALL, -1, -1, TARGET_SPECIAL_SPELL_ON_STACK, 0, 0);
    }

    if (!valid)
    {
      g_spell_fizzled = 1;
    }
    else
    {
      color = single_color_test_bit_to_color_t(global_cards_data[instance->internal_card_id].color);
      color = get_sleighted_color(player, card, color);
      target_instance = &PLAYER_CARD_INSTANCE(instance->targets[0].player, instance->targets[0].card);
      target_instance->color = (char)(1 << ((unsigned char)color & 0x1f));
      target_instance->state |= 0x2000;
      if (g_duel_ai_mode_state != 1)
      {
        play_sound_effect(WAV_CHANGEC);
      }
    }

    instance->number_of_targets = 0;
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004faee2
// FUNCTION: SHANDALAR 0x004b4fb3
void replace_card_hack_color(int player, int card, int color_from, unsigned char color_to)
{
  int current_color;

  for (current_color = 1; current_color < 6; ++current_color)
  {
    if (PLAYER_CARD_INSTANCE(player, card).hack_mode[current_color] == color_from)
    {
      PLAYER_CARD_INSTANCE(player, card).hack_mode[current_color] = (char)color_to;
    }
  }
  if (PLAYER_CARD_INSTANCE(player, card).hack_mode[color_from] == 0)
  {
    PLAYER_CARD_INSTANCE(player, card).hack_mode[color_from] = (char)color_to;
  }
}

// FUNCTION: MAGIC 0x004fafeb
// FUNCTION: SHANDALAR 0x004b50c0
int card_magical_hack(int player, int card, event_t event)
{
  extern card_ptr_t global_raw_cards_storage[2000];
  struct
  {
    unsigned int available_colors;
    int old_color;
    int new_color;
    target_t target;
    char prompt[200];
  } s;
  card_ptr_t *raw_card;
  int target_iid;

  if (event == EVENT_CAN_CAST)
  {
    if ((g_active_player == player) || (((unsigned char)g_duel_network_flags & 2) != 0))
    {
      if (g_current_spell_player == -1)
      {
        load_recorded_action_target(0);
        return 1;
      }
      return 99;
    }
    if ((g_current_spell_player == -1) || (g_active_player != g_current_player))
    {
      load_recorded_action_target(0);
      return 1;
    }
    return 99;
  }

  if (event == EVENT_CAST_SPELL && g_affected_card == card && g_affected_card_controller == player)
  {
    if (g_current_spell_player == -1)
    {
      load_text("prompts.txt", "MAGICAL_HACK");
      if (C_real_select_target(player,
                               2,
                               2,
                               TARGET_ZONE_IN_PLAY,
                               TYPE_ARTIFACT | TYPE_INTERRUPT | TYPE_INSTANT | TYPE_SORCERY | TYPE_ENCHANTMENT | TYPE_CREATURE | TYPE_LAND,
                               TYPE_NONE,
                               0,
                               get_protections_from(player, card),
                               COLOR_TEST_0,
                               COLOR_TEST_0,
                               -1,
                               ~SUB_WALL,
                               -1,
                               -1,
                               0,
                               0,
                               0,
                               g_text_lines[0],
                               1,
                               &s.target) == 0)
      {
        g_spell_fizzled = 1;
      }
      else
      {
        PLAYER_CARD_INSTANCE(player, card).targets[0].player = s.target.player;
        PLAYER_CARD_INSTANCE(player, card).targets[0].card = s.target.card;
        PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
      }
    }
    else
    {
      PLAYER_CARD_INSTANCE(player, card).targets[0].player = g_current_spell_player;
      PLAYER_CARD_INSTANCE(player, card).targets[0].card = g_current_spell_card;
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
    }

    if (g_spell_fizzled != 1)
    {
      if ((player == g_other_player) && (((unsigned char)g_duel_network_flags & 2) == 0))
      {
        target_iid =
            PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).targets[0].player, PLAYER_CARD_INSTANCE(player, card).targets[0].card)
                .internal_card_id;
        raw_card = &global_raw_cards_storage[global_cards_data[target_iid].id];
        s.available_colors = *(unsigned int *)&raw_card->hack_colors;
        if (s.available_colors == 0)
        {
          g_spell_fizzled = 1;
        }
        else
        {
          if ((PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).targets[0].player, PLAYER_CARD_INSTANCE(player, card).targets[0].card).token_status & STATUS_HACKED) != 0)
          {
            s.old_color = single_color_test_bit_to_color_t((unsigned char)s.available_colors);
            s.old_color = get_hacked_color(PLAYER_CARD_INSTANCE(player, card).targets[0].player, PLAYER_CARD_INSTANCE(player, card).targets[0].card, s.old_color);
            s.available_colors = 1 << ((unsigned char)s.old_color & 0x1f);
          }

          do
          {
            s.old_color = internal_rand(5) + 1;
          } while ((s.available_colors & (1 << ((unsigned char)s.old_color & 0x1f))) == 0);

          do
          {
            s.new_color = internal_rand(5) + 1;
          } while (s.new_color == s.old_color);

          if (g_duel_ai_mode_state == 1)
          {
            g_ai_recorded_choice = s.old_color;
            record_ai_action_selection();
            g_ai_recorded_choice = s.new_color;
            record_ai_action_selection();
          }
          else
          {
            replay_ai_action_selection();
            s.old_color = g_ai_recorded_choice;
            replay_ai_action_selection();
            s.new_color = g_ai_recorded_choice;
          }

          PLAYER_CARD_INSTANCE(player, card).info_slot = s.new_color * 0x100 + s.old_color;
          if (g_duel_ai_mode_state != 1)
          {
            load_text("prompts.txt", "MAGICAL_HACK");
            strcpy(s.prompt, "\n");
            sprintf(s.prompt + strlen(s.prompt), g_text_lines[1], "", "");
            do_dialog(player,
                      player,
                      card,
                      PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                      PLAYER_CARD_INSTANCE(player, card).targets[0].card,
                      s.prompt,
                      0);
          }
        }
      }
    }

    if (g_spell_fizzled == 1)
    {
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    s.target.player = PLAYER_CARD_INSTANCE(player, card).targets[0].player;
    s.target.card = PLAYER_CARD_INSTANCE(player, card).targets[0].card;
    if (g_duel_ai_mode_state != 1)
    {
      play_sound_effect(WAV_CHANGET);
    }
    PLAYER_CARD_INSTANCE(s.target.player, s.target.card).token_status |= STATUS_HACKED;
    replace_card_hack_color(s.target.player,
                 s.target.card,
                 (unsigned char)PLAYER_CARD_INSTANCE(player, card).info_slot,
                 *((unsigned char *)&PLAYER_CARD_INSTANCE(player, card).info_slot + 1));
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004fb8fd
// FUNCTION: SHANDALAR 0x004b59fb
void replace_card_sleight_color(int player, int card, int color_from, unsigned char color_to)
{
  int current_color;
  for (current_color = 1; current_color < 6; ++current_color)
  {
    if (PLAYER_CARD_INSTANCE(player, card).color_id[current_color] == color_from)
    {
      PLAYER_CARD_INSTANCE(player, card).color_id[current_color] = color_to;
    }
  }
  if (PLAYER_CARD_INSTANCE(player, card).color_id[color_from] == 0)
  {
    PLAYER_CARD_INSTANCE(player, card).color_id[color_from] = color_to;
  }
}

// FUNCTION: MAGIC 0x004fba06
// FUNCTION: SHANDALAR 0x004b5b0c
int card_sleight_of_mind(int player, int card, event_t event)
{
  extern card_ptr_t global_raw_cards_storage[2000];
  struct
  {
    card_instance_t *instance;
    target_t target;
    unsigned int available_colors;
    int target_iid;
    int old_color;
    int new_color;
    char pad[0xe4 - 0x1c];
  } s;

  if (event == EVENT_CAN_CAST)
  {
    if ((g_active_player == player) || ((g_duel_network_flags & 2) != 0))
    {
      if (g_current_spell_player != -1)
      {
        return 99;
      }
      load_recorded_action_target(0);
      return 1;
    }
    if (g_current_spell_player != -1 && g_active_player == g_current_player)
    {
      return 99;
    }
    load_recorded_action_target(0);
    return 1;
  }

  s.instance = &PLAYER_CARD_INSTANCE(player, card);
  if (event == EVENT_CAST_SPELL && g_affected_card == card && g_affected_card_controller == player)
  {
    if (g_current_spell_player == -1)
    {
      load_text(0, "SLEIGHT OF MIND");
      if (C_real_select_target(player,
                               2,
                               2,
                               TARGET_ZONE_IN_PLAY,
                               TYPE_ARTIFACT | TYPE_INTERRUPT | TYPE_INSTANT | TYPE_SORCERY | TYPE_ENCHANTMENT | TYPE_CREATURE | TYPE_LAND,
                               TYPE_NONE,
                               0,
                               get_protections_from(player, card),
                               COLOR_TEST_0,
                               COLOR_TEST_0,
                               -1,
                               ~SUB_WALL,
                               -1,
                               -1,
                                0,
                                0,
                                0,
                                g_text_lines[0],
                                1,
                                &s.target) == 0)
      {
        g_spell_fizzled = 1;
      }
      else
      {
        s.instance->targets[0] = s.target;
        s.instance->number_of_targets = 1;
      }
    }
    else
    {
      s.instance->targets[0].player = g_current_spell_player;
      s.instance->targets[0].card = g_current_spell_card;
      s.instance->number_of_targets = 1;
    }

    if (g_spell_fizzled != 1)
    {
      s.target_iid =
          PLAYER_CARD_INSTANCE(s.instance->targets[0].player, s.instance->targets[0].card).internal_card_id;
      s.available_colors = global_raw_cards_storage[global_cards_data[s.target_iid].id].sleight_color;
      if (s.available_colors == 0)
      {
        g_spell_fizzled = 1;
      }
      else
      {
        if ((PLAYER_CARD_INSTANCE(s.instance->targets[0].player, s.instance->targets[0].card).token_status & STATUS_SLEIGHTED) != 0)
        {
          s.old_color = single_color_test_bit_to_color_t((unsigned char)s.available_colors);
          s.old_color = get_sleighted_color(s.instance->targets[0].player, s.instance->targets[0].card, s.old_color);
          s.available_colors = 1 << ((unsigned char)s.old_color & 0x1f);
        }

        do
        {
          s.old_color = internal_rand(5) + 1;
        } while ((s.available_colors & (1 << ((unsigned char)s.old_color & 0x1f))) == 0);

        do
        {
          s.new_color = internal_rand(5) + 1;
        } while (s.new_color == s.old_color);

        if (g_duel_ai_mode_state == 1)
        {
          g_ai_recorded_choice = s.old_color;
          record_ai_action_selection();
          g_ai_recorded_choice = s.new_color;
          record_ai_action_selection();
        }
        else
        {
          replay_ai_action_selection();
          s.old_color = g_ai_recorded_choice;
          replay_ai_action_selection();
          s.new_color = g_ai_recorded_choice;
        }

        s.instance->info_slot = s.new_color * 0x100 + s.old_color;
      }
    }

    if (g_spell_fizzled == 1)
    {
      s.instance->number_of_targets = 0;
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    s.target = s.instance->targets[0];
    if (g_duel_ai_mode_state != 1)
    {
      play_sound_effect(WAV_CHANGET);
    }
    PLAYER_CARD_INSTANCE(s.target.player, s.target.card).token_status |= STATUS_SLEIGHTED;
    replace_card_sleight_color(s.target.player,
                 s.target.card,
                 (unsigned char)s.instance->info_slot,
                 (unsigned char)((unsigned int)s.instance->info_slot >> 8));
    s.instance->number_of_targets = 0;
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004fc353
// FUNCTION: SHANDALAR 0x004b647a
int card_blue_elemental_blast(int player, int card, event_t event)
{
  card_instance_t *instance;
  target_t target;
  int color;
  int valid;
  int target_player;
  int target_card;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_CAN_CAST)
  {
    if (g_current_spell_player == -1)
    {
      load_recorded_action_target(0);
      color = get_sleighted_color(player, card, 4);
      return real_target_available((int *)0, TARGET_SCAN_DIRECT, player, 2, 2, 0x200, 0x1047, 0, 0,
                                   get_protections_from(player, card), 1 << ((unsigned char)color & 0x1f), 0, -1, -1,
                                   0xffffffff, 0xffffffff, 0, 0, 0);
    }
    color = get_sleighted_color(player, card, 4);
    return C_real_validate_target(g_current_spell_player, g_current_spell_card, (char *)0, player, 2, 2, 0,
                                  TYPE_NONE, TYPE_NONE, 0, 0, 1 << ((unsigned char)color & 0x1f),
                                  COLOR_TEST_0, -1, ~SUB_WALL, -1, -1,
                                  TARGET_SPECIAL_SPELL_ON_STACK, 0, 0)
               ? 99
               : 0;
  }

  if ((event == EVENT_CAST_SPELL) && (card == g_card_on_stack) && (player == g_card_on_stack_controller))
  {
    if (g_current_spell_player == -1)
    {
      load_text("prompts.txt", "BLUE_ELEMENTAL_BLAST");
      color = get_sleighted_color(player, card, 4);
      if (!C_real_select_target(player, 2, 1 - player, TARGET_ZONE_IN_PLAY,
                                TARGET_TYPE_TOKEN | TYPE_ARTIFACT | TYPE_ENCHANTMENT | TYPE_CREATURE | TYPE_LAND,
                                TYPE_NONE, 0, get_protections_from(player, card),
                                1 << ((unsigned char)color & 0x1f), COLOR_TEST_0, -1, ~SUB_WALL, -1, -1,
                                0, 0, 0, g_text_lines[0], 1, &target))
      {
        g_spell_fizzled = 1;
      }
      else
      {
        instance->targets[0].player = target.player;
        instance->targets[0].card = target.card;
        instance->number_of_targets = 1;
      }
    }
    else
    {
      instance->targets[0].player = g_current_spell_player;
      instance->targets[0].card = g_current_spell_card;
      instance->number_of_targets = 1;
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    valid = 0;
    if (g_current_spell_player == -1)
    {
      color = get_sleighted_color(player, card, 4);
      valid = C_real_validate_target(instance->targets[0].player, instance->targets[0].card,
                                     (char *)0, player, 2, 2, TARGET_ZONE_IN_PLAY, TYPE_NONE,
                                     TYPE_NONE, 0, get_protections_from(player, card),
                                     1 << ((unsigned char)color & 0x1f), COLOR_TEST_0, -1, ~SUB_WALL,
                                     -1, -1, 0, 0, 0);
    }
    else
    {
      color = get_sleighted_color(player, card, 4);
      valid = C_real_validate_target(instance->targets[0].player, instance->targets[0].card,
                                     (char *)0, player, 2, 2, 0, TYPE_NONE, TYPE_NONE, 0, 0,
                                     1 << ((unsigned char)color & 0x1f), COLOR_TEST_0, -1, ~SUB_WALL,
                                     -1, -1, TARGET_SPECIAL_SPELL_ON_STACK, 0, 0);
    }

    if (valid)
    {
      target_player = instance->targets[0].player;
      target_card = instance->targets[0].card;
      if ((PLAYER_CARD_INSTANCE(target_player, target_card).mana_color & (1 << ((unsigned char)color & 0x1f))) != 0)
      {
        kill_card(target_player, target_card, KILL_DESTROY);
      }
    }
    else
    {
      g_spell_fizzled = 1;
    }

    instance->number_of_targets = 0;
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004fc90c
// FUNCTION: SHANDALAR 0x004b6a32
int card_counterspell(int player, int card, event_t event)
{
  card_instance_t *instance;
  int valid;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_CAN_CAST)
  {
    load_recorded_action_target(0);
    if (g_current_spell_player == -1)
    {
      return 0;
    }
    return C_real_validate_target(g_current_spell_player, g_current_spell_card, (char *)0, player, 2, 2, 0,
                                  TYPE_NONE, TYPE_NONE, 0, 0, COLOR_TEST_0, COLOR_TEST_0,
                                  -1, ~SUB_WALL, -1, -1, TARGET_SPECIAL_SPELL_ON_STACK, 0, 0)
               ? 99
               : 0;
  }

  if ((event == EVENT_CAST_SPELL) && (card == g_card_on_stack) && (player == g_card_on_stack_controller))
  {
    if (g_current_spell_player == -1)
    {
      g_spell_fizzled = 1;
    }
    else
    {
      instance->targets[0].player = g_current_spell_player;
      instance->targets[0].card = g_current_spell_card;
      instance->number_of_targets = 1;
    }
    g_ai_modifier -= 0x24;
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    valid = C_real_validate_target(instance->targets[0].player, instance->targets[0].card,
                                   (char *)0, player, 2, 2, 0, TYPE_NONE, TYPE_NONE, 0, 0,
                                   COLOR_TEST_0, COLOR_TEST_0, -1, ~SUB_WALL, -1, -1,
                                   TARGET_SPECIAL_SPELL_ON_STACK, 0, 0);
    if (valid)
    {
      if ((PLAYER_CARD_INSTANCE(instance->targets[0].player, instance->targets[0].card).state & 0x20) != 0)
      {
        kill_card(instance->targets[0].player, instance->targets[0].card, KILL_BURY);
      }
    }
    else
    {
      g_spell_fizzled = 1;
    }

    instance->number_of_targets = 0;
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004fcc3f
// FUNCTION: SHANDALAR 0x004b6d67
int card_power_sink(int player, int card, event_t event)
{
  struct
  {
    int target_player;
    int target_card;
    int mana_paid;
    int color;
    int current_card;
  } s;

  if (event == EVENT_CAN_CAST)
  {
    load_recorded_action_target(0);
    if (g_current_spell_player == -1)
    {
      return 0;
    }
    if (player == g_other_player && (g_duel_network_flags & 2) == 0 && has_mana(player, COLOR_ANY, 2) == 0)
    {
      return 0;
    }
    if (!C_real_validate_target(g_current_spell_player,
                                g_current_spell_card,
                                (char *)0,
                                player,
                                2,
                                2,
                                0,
                                TYPE_NONE,
                                TYPE_NONE,
                                0,
                                0,
                                COLOR_TEST_0,
                                COLOR_TEST_0,
                                -1,
                                ~SUB_WALL,
                                -1,
                                -1,
                                TARGET_SPECIAL_SPELL_ON_STACK,
                                0,
                                0))
    {
      return 0;
    }
    return 99;
  }

  if (event == EVENT_CAST_SPELL && card == g_affected_card && player == g_affected_card_controller)
  {
    if (g_current_spell_player == -1)
    {
      g_spell_fizzled = 1;
    }
    else
    {
      PLAYER_CARD_INSTANCE(player, card).targets[0].player = g_current_spell_player;
      PLAYER_CARD_INSTANCE(player, card).targets[0].card = g_current_spell_card;
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
      PLAYER_CARD_INSTANCE(player, card).info_slot = g_x_value;
    }
    return 0;
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    if (!C_real_validate_target(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                                PLAYER_CARD_INSTANCE(player, card).targets[0].card,
                                (char *)0,
                                player,
                                2,
                                2,
                                0,
                                TYPE_NONE,
                                TYPE_NONE,
                                0,
                                0,
                                COLOR_TEST_0,
                                COLOR_TEST_0,
                                -1,
                                ~SUB_WALL,
                                -1,
                                -1,
                                TARGET_SPECIAL_SPELL_ON_STACK,
                                0,
                                0))
    {
      g_spell_fizzled = 1;
    }
    else
    {
      s.target_player = PLAYER_CARD_INSTANCE(player, card).targets[0].player;
      s.target_card = PLAYER_CARD_INSTANCE(player, card).targets[0].card;

      if (s.target_player == g_current_player || (g_duel_network_flags & 2) != 0)
      {
        push_card_onto_stack(player, card, 0x7e, 0, 0);
        s.mana_paid = charge_mana(s.target_player, COLOR_COLORLESS, PLAYER_CARD_INSTANCE(player, card).info_slot);
        obliterate_top_card_of_stack();
        g_spell_fizzled = 0;
      }
      else if (has_mana(s.target_player, COLOR_ANY, 1) == 0)
      {
        s.mana_paid = 0;
      }
      else
      {
        s.mana_paid = charge_mana(s.target_player, COLOR_COLORLESS, PLAYER_CARD_INSTANCE(player, card).info_slot);
      }

      if (s.mana_paid < PLAYER_CARD_INSTANCE(player, card).info_slot)
      {
        for (s.color = 0; s.color < 7 && s.mana_paid < PLAYER_CARD_INSTANCE(player, card).info_slot; ++s.color)
        {
          while (g_raw_mana_available[s.target_player][s.color] > 0 &&
                 s.mana_paid < PLAYER_CARD_INSTANCE(player, card).info_slot)
          {
            --g_raw_mana_available[s.target_player][s.color];
            --g_raw_mana_available[s.target_player][7];
            if (s.color != 6)
            {
              ++s.mana_paid;
            }
          }
        }

        g_mana_charge[6] = PLAYER_CARD_INSTANCE(player, card).info_slot;
        for (s.current_card = 0;
             s.current_card < g_active_cards_count[s.target_player] &&
             s.mana_paid < PLAYER_CARD_INSTANCE(player, card).info_slot;
             ++s.current_card)
        {
          if (is_in_play(s.target_player, s.current_card))
          {
            if ((global_cards_data[PLAYER_CARD_INSTANCE(s.target_player, s.current_card).internal_card_id].type & TYPE_LAND) != 0 &&
                (global_cards_data[PLAYER_CARD_INSTANCE(s.target_player, s.current_card).internal_card_id].extra_ability & 0x1000) != 0 &&
                (PLAYER_CARD_INSTANCE(s.target_player, s.current_card).state & STATE_TAPPED) == 0 &&
                (((PLAYER_CARD_INSTANCE(s.target_player, s.current_card).state & 3) == 0) ||
                 (global_cards_data[PLAYER_CARD_INSTANCE(s.target_player, s.current_card).internal_card_id].type & TYPE_CREATURE) == 0))
            {
              drain_power_draw_mana_from_land(s.target_player,
                                              s.current_card,
                                              PLAYER_CARD_INSTANCE(s.target_player, s.current_card).internal_card_id);

              for (s.color = 0; s.color < 7 && s.mana_paid < PLAYER_CARD_INSTANCE(player, card).info_slot; ++s.color)
              {
                while (g_raw_mana_available[s.target_player][s.color] > 0 &&
                       s.mana_paid < PLAYER_CARD_INSTANCE(player, card).info_slot)
                {
                  --g_raw_mana_available[s.target_player][s.color];
                  --g_raw_mana_available[s.target_player][7];
                  if (s.color != 6)
                  {
                    ++s.mana_paid;
                  }
                }
              }
            }
          }
        }
        g_mana_charge[6] = 0;
      }

      if (s.mana_paid < PLAYER_CARD_INSTANCE(player, card).info_slot)
      {
        kill_card(s.target_player, s.target_card, KILL_BURY);
      }
    }

    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004fd465
// FUNCTION: SHANDALAR 0x004b7590
int card_spell_blast(int player, int card, event_t event)
{
  card_instance_t *instance;
  int cost;

  instance = &PLAYER_CARD_INSTANCE(player, card);
  if (event == EVENT_CAN_CAST)
  {
    load_recorded_action_target(0);
    if (g_current_spell_player == -1)
    {
      return 0;
    }

    if ((g_other_player == player) && ((g_duel_network_flags & 2) == 0) && !has_mana_w_global_cost_mod(player, card, 7, 2))
    {
      return 0;
    }

    cost = (int)(char)global_cards_data[PLAYER_CARD_INSTANCE(g_current_spell_player, g_current_spell_card).internal_card_id].cc[1];
    if (cost == -1)
    {
      cost = g_x_value;
    }
    cost += (int)(char)global_cards_data[PLAYER_CARD_INSTANCE(g_current_spell_player, g_current_spell_card).internal_card_id].cc[0];
    instance->info_slot = cost;

    if (!has_mana(player, 2, 1) || !has_mana_w_global_cost_mod(player, card, 7, cost + 1) || !C_real_validate_target(g_current_spell_player, g_current_spell_card, (char *)0, player, 2, 2, 0, TYPE_NONE, TYPE_NONE, 0, 0, COLOR_TEST_0, COLOR_TEST_0, -1, ~SUB_WALL, -1, -1, TARGET_SPECIAL_SPELL_ON_STACK, 0, 0))
    {
      return 0;
    }

    unk_0091bfb4 = 1;
    return 99;
  }

  if ((event == EVENT_CAST_SPELL) && (card == g_card_on_stack) && (player == g_card_on_stack_controller) && (g_current_spell_player != -1))
  {
    g_mana_charge[COLOR_BLUE] = 1;
    charge_mana_w_global_cost_mod(player, card, 0, instance->info_slot);
    if (g_spell_fizzled != 1)
    {
      instance->targets[0].player = g_current_spell_player;
      instance->targets[0].card = g_current_spell_card;
      instance->number_of_targets = 1;
      g_x_value = instance->info_slot;
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    if (!C_real_validate_target(instance->targets[0].player, instance->targets[0].card, (char *)0,
                                player, 2, 2, 0, TYPE_NONE, TYPE_NONE, 0, 0, COLOR_TEST_0,
                                COLOR_TEST_0, -1, ~SUB_WALL, -1, -1,
                                TARGET_SPECIAL_SPELL_ON_STACK, 0, 0))
    {
      g_spell_fizzled = 1;
    }
    else if ((PLAYER_CARD_INSTANCE(instance->targets[0].player, instance->targets[0].card).state & 0x20) != 0)
    {
      kill_card(instance->targets[0].player, instance->targets[0].card, KILL_DESTROY);
    }

    instance->number_of_targets = 0;
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004fd933
// FUNCTION: SHANDALAR 0x004b7a5b
int card_red_elemental_blast(int player, int card, event_t event)
{
  card_instance_t *instance;
  target_t target;
  int color;
  int valid;
  int target_player;
  int target_card;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_CAN_CAST)
  {
    if (g_current_spell_player == -1)
    {
      load_recorded_action_target(0);
      color = get_sleighted_color(player, card, 2);
      return real_target_available((int *)0, TARGET_SCAN_DIRECT, player, 2, 2, 0x200, 0x1047, 0, 0,
                                   get_protections_from(player, card), 1 << ((unsigned char)color & 0x1f), 0, -1, -1,
                                   0xffffffff, 0xffffffff, 0, 0, 0);
    }
    color = get_sleighted_color(player, card, 2);
    return C_real_validate_target(g_current_spell_player, g_current_spell_card, (char *)0, player, 2, 2, 0,
                                  TYPE_NONE, TYPE_NONE, 0, 0, 1 << ((unsigned char)color & 0x1f),
                                  COLOR_TEST_0, -1, ~SUB_WALL, -1, -1,
                                  TARGET_SPECIAL_SPELL_ON_STACK, 0, 0)
               ? 99
               : 0;
  }

  if ((event == EVENT_CAST_SPELL) && (card == g_card_on_stack) && (player == g_card_on_stack_controller))
  {
    if (g_current_spell_player == -1)
    {
      load_text("prompts.txt", "RED_ELEMENTAL_BLAST");
      color = get_sleighted_color(player, card, 2);
      if (!C_real_select_target(player, 2, 1 - player, TARGET_ZONE_IN_PLAY,
                                TARGET_TYPE_TOKEN | TYPE_ARTIFACT | TYPE_ENCHANTMENT | TYPE_CREATURE | TYPE_LAND,
                                TYPE_NONE, 0, get_protections_from(player, card),
                                1 << ((unsigned char)color & 0x1f), COLOR_TEST_0, -1, ~SUB_WALL, -1, -1,
                                0, 0, 0, g_text_lines[0], 1, &target))
      {
        g_spell_fizzled = 1;
      }
      else
      {
        instance->targets[0].player = target.player;
        instance->targets[0].card = target.card;
        instance->number_of_targets = 1;
      }
    }
    else
    {
      instance->targets[0].player = g_current_spell_player;
      instance->targets[0].card = g_current_spell_card;
      instance->number_of_targets = 1;
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    valid = 0;
    if (g_current_spell_player == -1)
    {
      color = get_sleighted_color(player, card, 2);
      valid = C_real_validate_target(instance->targets[0].player, instance->targets[0].card,
                                     (char *)0, player, 2, 2, TARGET_ZONE_IN_PLAY, TYPE_NONE,
                                     TYPE_NONE, 0, get_protections_from(player, card),
                                     1 << ((unsigned char)color & 0x1f), COLOR_TEST_0, -1, ~SUB_WALL,
                                     -1, -1, 0, 0, 0);
    }
    else
    {
      color = get_sleighted_color(player, card, 2);
      valid = C_real_validate_target(instance->targets[0].player, instance->targets[0].card,
                                     (char *)0, player, 2, 2, 0, TYPE_NONE, TYPE_NONE, 0, 0,
                                     1 << ((unsigned char)color & 0x1f), COLOR_TEST_0, -1, ~SUB_WALL,
                                     -1, -1, TARGET_SPECIAL_SPELL_ON_STACK, 0, 0);
    }

    if (valid)
    {
      target_player = instance->targets[0].player;
      target_card = instance->targets[0].card;
      if ((PLAYER_CARD_INSTANCE(target_player, target_card).mana_color & (1 << ((unsigned char)color & 0x1f))) != 0)
      {
        kill_card(target_player, target_card, KILL_DESTROY);
      }
    }
    else
    {
      g_spell_fizzled = 1;
    }

    instance->number_of_targets = 0;
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004fdeec
// FUNCTION: SHANDALAR 0x004b8014
int card_dark_ritual(int player, int card, event_t event)
{
  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if ((event == EVENT_CAST_SPELL) && (card == g_affected_card) && (player == g_affected_card_controller))
  {
    if (g_duel_summary.hand_counts[player] < 8)
    {
      g_ai_modifier -= 0x3c;
    }
    else
    {
      g_ai_modifier -= 0x18;
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    produce_mana(player, COLOR_BLACK, 3);
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004fdf88
// FUNCTION: SHANDALAR 0x004b80b2
int card_alabaster_potion(int player, int card, event_t event)
{
  return gain_life_or_prevent_damage(player, card, event, g_x_value);
}

// FUNCTION: MAGIC 0x004fdfb2
// FUNCTION: SHANDALAR 0x004b80dc
int card_healing_salve(int player, int card, event_t event)
{
  return gain_life_or_prevent_damage(player, card, event, 3);
}

// FUNCTION: MAGIC 0x004fdfd8
// FUNCTION: SHANDALAR 0x004b8102
int gain_life_or_prevent_damage(int player, int card, event_t event, int amount)
{
  card_instance_t *instance;
  target_t selected_target;
  int done;
  int cancelled;
  int target_index;
  int source_player;
  int source_card;
  char prompt[300];

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_CAN_CAST)
  {
    if (((player == g_other_player) && ((g_duel_network_flags & 2) == 0)) && (amount == 0))
    {
      return 0;
    }
    if ((g_land_can_be_played & 4) == 0)
    {
      return 1;
    }
    return 99;
  }

  if (((event == EVENT_CAST_SPELL) && (g_affected_card == card)) && (g_affected_card_controller == player))
  {
    g_ai_modifier -= 0x60;
    if ((g_land_can_be_played & 4) == 0)
    {
      load_text("prompts.txt", "HEALING_SALVE");
      if (!C_real_select_target(player,
                                2,
                                player,
                                TARGET_ZONE_PLAYERS,
                                TYPE_NONE,
                                TYPE_NONE,
                                0,
                                0,
                                COLOR_TEST_0,
                                COLOR_TEST_0,
                                -1,
                                ~SUB_WALL,
                                -1,
                                -1,
                                0,
                                0,
                                0,
                                g_text_lines[0],
                                1,
                                &selected_target))
      {
        g_spell_fizzled = 1;
      }
      else
      {
        instance->targets[0] = selected_target;
        instance->number_of_targets = 1;
        instance->info_slot = amount;
      }
    }
    else
    {
      instance->number_of_targets = 0;
      source_player = -1;
      source_card = 0;
      cancelled = 0;
      done = 0;
      g_spell_fizzled = -1;
      while ((instance->number_of_targets < amount) && !cancelled && (g_spell_fizzled != 1) && !done)
      {
        load_text("prompts.txt", "HEALING_SALVE2");
        strcpy(prompt, g_text_lines[1]);
        if (!C_real_select_target(player,
                                  2,
                                  player,
                                  TARGET_ZONE_IN_PLAY,
                                  TYPE_NONE,
                                  TYPE_NONE,
                                  0,
                                  0,
                                  COLOR_TEST_0,
                                  COLOR_TEST_0,
                                  g_damage_card_internal_card_id,
                                  ~SUB_WALL,
                                  -1,
                                  -1,
                                  0,
                                  0,
                                  0,
                                  prompt,
                                  3,
                                  &selected_target))
        {
          if (selected_target.card == -1)
          {
            g_spell_fizzled = 1;
          }
          else
          {
            cancelled = 1;
          }
        }
        else if (((PLAYER_CARD_INSTANCE(selected_target.player, selected_target.card).damage_source_player == source_player) && (PLAYER_CARD_INSTANCE(selected_target.player, selected_target.card).damage_source_card == source_card)) || source_player == -1)
        {
          source_player = PLAYER_CARD_INSTANCE(selected_target.player, selected_target.card).damage_source_player;
          source_card = PLAYER_CARD_INSTANCE(selected_target.player, selected_target.card).damage_source_card;
          PLAYER_CARD_INSTANCE(selected_target.player, selected_target.card).state |= 0x200000;
          TENTATIVE_reassess_all_cards(0, 0xff);
          instance->targets[instance->number_of_targets] = selected_target;
          ++instance->number_of_targets;
          if (instance->number_of_targets == 19)
          {
            done = 1;
          }
          if (unk_00715fb0 == 1)
          {
            while ((instance->number_of_targets < amount) && !done)
            {
              instance->targets[instance->number_of_targets] = selected_target;
              ++instance->number_of_targets;
              if (instance->number_of_targets == 19)
              {
                done = 1;
              }
            }
          }
        }
        else if (g_duel_ai_mode_state == 1)
        {
          rewind_recorded_action();
        }
        else
        {
          load_text("prompts.txt", "HEALING_SALVE3");
          set_duel_prompt_text(g_text_lines[2]);
          Sleep(0x9c4);
          set_duel_prompt_text("");
        }
      }
      for (target_index = 0; target_index < instance->number_of_targets; ++target_index)
      {
        PLAYER_CARD_INSTANCE(instance->targets[target_index].player, instance->targets[target_index].card).state &= ~0x300000;
      }
      if (g_spell_fizzled == 1)
      {
        instance->number_of_targets = 0;
      }
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    while (instance->number_of_targets > 0)
    {
      --instance->number_of_targets;
      selected_target = instance->targets[instance->number_of_targets];
      if ((g_land_can_be_played & 4) == 0)
      {
        gain_life(selected_target.player, instance->info_slot, player, card);
      }
      else if (!C_real_validate_target(selected_target.player,
                                       selected_target.card,
                                       (char *)0,
                                       player,
                                       2,
                                       2,
                                       TARGET_ZONE_IN_PLAY,
                                       TYPE_NONE,
                                       TYPE_NONE,
                                       0,
                                       0,
                                       COLOR_TEST_0,
                                       COLOR_TEST_0,
                                       g_damage_card_internal_card_id,
                                       ~SUB_WALL,
                                       -1,
                                       -1,
                                       0,
                                       0,
                                       0))
      {
        g_spell_fizzled = 1;
      }
      else if (PLAYER_CARD_INSTANCE(selected_target.player, selected_target.card).info_slot != 0)
      {
        --PLAYER_CARD_INSTANCE(selected_target.player, selected_target.card).info_slot;
      }
    }
    kill_card(player, card, KILL_DESTROY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004feb03
// FUNCTION: SHANDALAR 0x004b8c28
int card_reverse_damage(int player, int card, event_t event)
{
  struct
  {
    target_t target;
    int damage_sources[300];
    int damage_amounts[150];
    int internal_card_ids[150];
    int selected;
    int current_card;
    int controller;
    int count;
    int best_damage;
  } s;
  int result;

  if (event == EVENT_CAN_CAST)
  {
    load_recorded_action_target(0);
    if ((g_land_can_be_played & 4) == 0)
    {
      result = 1;
    }
    else
    {
      result = real_target_available((int *)0,
                                     TARGET_SCAN_DIRECT,
                                     player,
                                     2,
                                     2,
                                     TARGET_ZONE_IN_PLAY,
                                     TYPE_NONE,
                                     TYPE_NONE,
                                     0,
                                     0,
                                     0,
                                     0,
                                     g_damage_card_internal_card_id,
                                     -1,
                                     0xffffffff,
                                     0xffffffff,
                                     0x20,
                                     0,
                                     0);
      if (result != 0)
      {
        result = 99;
      }
    }
  }
  else
  {
    if ((((event == EVENT_CAST_SPELL) && (card == g_affected_card)) && (player == g_affected_card_controller)) && ((g_land_can_be_played & 4) != 0))
    {
      load_text("prompts.txt", "REVERSE_DAMAGE");
      result = C_real_select_target(player,
                                    2,
                                    2,
                                    TARGET_ZONE_IN_PLAY,
                                    TYPE_NONE,
                                    TYPE_NONE,
                                    0,
                                    0,
                                    COLOR_TEST_0,
                                    COLOR_TEST_0,
                                    g_damage_card_internal_card_id,
                                    ~SUB_WALL,
                                    -1,
                                    -1,
                                    TARGET_SPECIAL_DAMAGE_PLAYER,
                                    0,
                                    0,
                                    g_text_lines[0],
                                    1,
                                    &s.target);
      if (result == 0)
      {
        g_spell_fizzled = 1;
      }
      else
      {
        PLAYER_CARD_INSTANCE(player, card).targets[0].player = s.target.player;
        PLAYER_CARD_INSTANCE(player, card).targets[0].card = s.target.card;
        PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
        *(unsigned int *)&PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
      }
    }

    if (event == EVENT_RESOLVE_SPELL)
    {
      if ((g_land_can_be_played & 4) == 0)
      {
        s.count = 0;
        s.best_damage = 0;
        for (s.controller = 0; s.controller < 2; s.controller = s.controller + 1)
        {
          for (s.current_card = 0; s.current_card < 150; s.current_card = s.current_card + 1)
          {
            if (0 < unk_0093b280[s.controller][s.current_card][player][0])
            {
              s.damage_sources[s.count * 2] = s.controller;
              s.damage_sources[s.count * 2 + 1] = s.current_card;
              s.damage_amounts[s.count] = unk_0093b280[s.controller][s.current_card][player][0];
              if (PLAYER_CARD_INSTANCE(s.controller, s.current_card).internal_card_id == -1)
              {
                s.internal_card_ids[s.count] = *(int *)((char *)&PLAYER_CARD_INSTANCE(s.controller, s.current_card) + 0x38);
                s.count = s.count + 1;
              }
              else
              {
                s.internal_card_ids[s.count] = PLAYER_CARD_INSTANCE(s.controller, s.current_card).internal_card_id;
                s.count = s.count + 1;
              }
            }
          }
        }
        if (0 < s.count)
        {
          if (((player == g_other_player) && ((g_duel_network_flags & 2) == 0)) || (g_duel_ai_mode_state == 1))
          {
            s.best_damage = 0;
            for (s.current_card = 0; s.current_card < s.count; s.current_card = s.current_card + 1)
            {
              if (s.best_damage < s.damage_amounts[s.current_card])
              {
                s.best_damage = s.current_card;
              }
            }
            s.selected = s.best_damage;
          }
          else
          {
            load_text("prompts.txt", "REVERSE_DAMAGE");
            s.selected = select_damage_card_from_list(player, s.internal_card_ids, s.damage_amounts, s.count, 0x89684c, 1, g_text_lines[1]);
          }
          gain_life(player,
                    unk_0093b280[s.damage_sources[s.selected * 2]][s.damage_sources[s.selected * 2 + 1]][player][0] *
                        2,
                    player, card);
          unk_0093b280[s.damage_sources[s.selected * 2]][s.damage_sources[s.selected * 2 + 1]][player][0] = 0;
        }
        if (g_life[1 - player] < 1)
        {
          g_ai_modifier += 1000;
        }
        else
        {
          g_ai_modifier += (s.best_damage * 100) / g_life[1 - player] - 100;
        }
      }
      else
      {
        s.target.player = PLAYER_CARD_INSTANCE(player, card).targets[0].player;
        s.target.card = PLAYER_CARD_INSTANCE(player, card).targets[0].card;
        result = C_real_validate_target(s.target.player,
                                        s.target.card,
                                        (char *)0,
                                        player,
                                        2,
                                        2,
                                        TARGET_ZONE_IN_PLAY,
                                        TYPE_NONE,
                                        TYPE_NONE,
                                        0,
                                        0,
                                        COLOR_TEST_0,
                                        COLOR_TEST_0,
                                        g_damage_card_internal_card_id,
                                        ~SUB_WALL,
                                        -1,
                                        -1,
                                        TARGET_SPECIAL_DAMAGE_PLAYER,
                                        0,
                                        0);
        if (result == 0)
        {
          g_spell_fizzled = 1;
        }
        else if (PLAYER_CARD_INSTANCE(s.target.player, s.target.card).info_slot != 0)
        {
          gain_life(player,
                    unk_0093b280[(char)PLAYER_CARD_INSTANCE(s.target.player, s.target.card).damage_source_player]
                                [PLAYER_CARD_INSTANCE(s.target.player, s.target.card).damage_source_card][player][0] *
                            2 +
                        PLAYER_CARD_INSTANCE(s.target.player, s.target.card).info_slot,
                    player, card);
          PLAYER_CARD_INSTANCE(s.target.player, s.target.card).info_slot = 0;
        }
        PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                             PLAYER_CARD_INSTANCE(player, card).parent_card)
            .number_of_targets = 0;
      }
      kill_card(player, card, KILL_BURY);
    }
    result = 0;
  }

  return result;
}

// FUNCTION: MAGIC 0x004ff33d
// FUNCTION: SHANDALAR 0x004b9465
int card_eye_for_an_eye(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x004ff68a
// FUNCTION: SHANDALAR 0x004b97b4
int card_inferno(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x004ff86e
// FUNCTION: SHANDALAR 0x004b9998
int card_fissure(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x004ffb2e
// FUNCTION: SHANDALAR 0x004b9c58
int card_orcish_catapult(int player, int card, event_t event)
{
}
