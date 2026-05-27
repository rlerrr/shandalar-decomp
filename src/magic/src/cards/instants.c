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
    FUN_004e4ff3(0);
    if (unk_008ce508 == -1)
    {
      return 0;
    }
    PLAYER_CARD_INSTANCE(player, card).info_slot = x_value;
    if ((global_cards_data[PLAYER_CARD_INSTANCE(unk_008ce508, unk_008ce4f4).internal_card_id].type & (TYPE_INSTANT | TYPE_INTERRUPT)) == 0 || !C_real_validate_target(unk_008ce508, unk_008ce4f4, (char *)0, player, 2, 2, 0,
                                                                                                                                                                      TYPE_NONE, TYPE_NONE, 0, 0, COLOR_TEST_0, COLOR_TEST_0,
                                                                                                                                                                      -1, ~SUB_WALL, -1, -1, TARGET_SPECIAL_SPELL_ON_STACK, 0,
                                                                                                                                                                      0))
    {
      return 0;
    }
    return 99;
  }

  if (event == EVENT_CAST_SPELL && affected_card == card && affected_card_controller == player)
  {
    if (unk_008ce508 == -1)
    {
      spell_fizzled = 1;
    }
    else
    {
      PLAYER_CARD_INSTANCE(player, card).targets[0].player = unk_008ce508;
      PLAYER_CARD_INSTANCE(player, card).targets[0].card = unk_008ce4f4;
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
      x_value = PLAYER_CARD_INSTANCE(player, card).info_slot;
      unk_008b4278 |= 0x400;
      FUN_0051a41c(player, new_card);
      unk_008b4278 &= ~0x400;
    }
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

int FUN_004f6311(int player, int card, int internal_card_id);

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

  if (((event == EVENT_CAST_SPELL) && (card == affected_card)) && (player == affected_card_controller))
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
                              text_lines[0],
                              1,
                              &selected_target))
    {
      spell_fizzled = 1;
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
    FUN_0055117d(FUN_004f6311, target_player);
    for (color = 0; color < 8; ++color)
    {
      raw_mana_available[target_player][color] = 0;
    }
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004f6311
int FUN_004f6311(int player, int card, int internal_card_id)
{
  unk_00938e2c = 1;
  produced_mana_color = -1;
  if ((PLAYER_CARD_INSTANCE(player, card).state & 0x10) == 0 && (global_cards_data[internal_card_id].type & TYPE_LAND) != 0 && (global_cards_data[internal_card_id].extra_ability & 0x1000) != 0 && (PLAYER_CARD_INSTANCE(player, card).state & 0x10) == 0 && (global_cards_data[internal_card_id].type & TYPE_LAND) != 0)
  {
    dispatch_event_to_single_card(player, card, EVENT_ACTIVATE, 1 - player, -1);
    if ((PLAYER_CARD_INSTANCE(player, card).state & 0x10) != 0)
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
    return (human_player != player && current_phase < PHASE_DECLARE_ATTACKERS) ? 1 : 0;
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

  if (((event == EVENT_CAST_SPELL) && (affected_card == card)) && (affected_card_controller == player))
  {
    load_text((int)"prompts.txt", "ANCESTRAL_RECALL");
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
                              text_lines[0],
                              1,
                              &selected_target))
    {
      spell_fizzled = 1;
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
    FUN_0043e18b(target_player);
    FUN_0043e18b(target_player);
    FUN_0043e18b(target_player);
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
    FUN_004e4ff3(0);
    if (!real_target_available((int *)0, TARGET_SCAN_DIRECT, player, player, player, 0x200, 2, 0, 0,
                               get_protections_from(player, card), 0, 0, -1, -1, 0xffffffff,
                               0xffffffff, 0, 0, 0))
    {
      return 0;
    }
    if ((unk_008b4278 & 4) == 0)
    {
      return 1;
    }
    return real_target_available((int *)0, TARGET_SCAN_DIRECT, player, 2, 2, 0x200, 0, 0, 0, 0, 0, 0,
                                 unk_009266a4, -1, 0xffffffff, 0xffffffff, 0x20, 0, 0)
               ? 99
               : 0;
  }

  if ((event == EVENT_CAST_SPELL) && (card == card_on_stack) && (player == card_on_stack_controller))
  {
    load_text((int)"prompts.txt", "SIMULACRUM");
    if (!FUN_00551638(player, player, card))
    {
      spell_fizzled = 1;
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
      spell_fizzled = 1;
    }
    else
    {
      if ((unk_008b4278 & 4) != 0)
      {
        instance->eot_toughness = 0;
        for (current_player = 0; current_player < 2; ++current_player)
        {
          for (current_card = 0; current_card < active_cards_count[current_player]; ++current_card)
          {
            damage = &PLAYER_CARD_INSTANCE(current_player, current_card);
            if (damage->internal_card_id == unk_009266a4 && damage->damage_target_player == player && damage->damage_target_card == -1)
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

      life[player] += instance->info_slot;
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
    FUN_004e4ff3(0);
    return real_target_available((int *)0, TARGET_SCAN_DIRECT, player, 2, 2, TARGET_ZONE_IN_PLAY, TYPE_ARTIFACT, TYPE_NONE, 0, 0,
                                 COLOR_TEST_0, COLOR_TEST_0, -1, ~SUB_WALL, -1, -1, 0, 0, 0);
  }

  if (((event == EVENT_CAST_SPELL) && (affected_card == card)) && (affected_card_controller == player))
  {
    load_text((int)"prompts.txt", "SHATTER");
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
                              text_lines[0],
                              1,
                              &instance->targets[0]))
    {
      spell_fizzled = 1;
    }
    else
    {
      ai_modifier -= 0x10;
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
      spell_fizzled = 1;
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
    FUN_004e4ff3(0);
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

  if (((event == EVENT_CAST_SPELL) && (affected_card == card)) && (affected_card_controller == player))
  {
    ai_modifier -= 0x10;
    load_text((int)"prompts.txt", "DISENCHANT");
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
                              text_lines[0],
                              1,
                              &selected_target))
    {
      spell_fizzled = 1;
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
      spell_fizzled = 1;
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
    FUN_004e4ff3(0);
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

  if (event == 0x6c && affected_card == card && affected_card_controller == player)
  {
    ai_modifier -= 0x30;
    load_text((int)"prompts.txt", "TWIDDLE");
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
                              text_lines[0],
                              1,
                              &target))
    {
      spell_fizzled = 1;
    }
    else
    {
      instance->targets[0] = target;
      *((char *)instance + 0x32) = 1;
      if ((unk_008b4278 & 0x400) == 0)
      {
        instance->info_slot = (PLAYER_CARD_INSTANCE(target.player, target.card).state & STATE_TAPPED) != 0;
      }
      else
      {
        instance->info_slot = PLAYER_CARD_INSTANCE(unk_008ce508, unk_008ce4f4).info_slot;
      }
      if (player == active_player)
      {
        if ((global_cards_data[PLAYER_CARD_INSTANCE(target.player, target.card).internal_card_id].type & TYPE_LAND) != 0)
        {
          ai_modifier -= 0x18;
        }
        if ((instance->info_slot == 0 && target.player == active_player) || (instance->info_slot == 1 && target.player == unk_008b35ec))
        {
          ai_modifier -= 0x60;
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
      spell_fizzled = 1;
    }
    else if (instance->info_slot == 0)
    {
      FUN_004f7783(instance->targets[0].player, instance->targets[0].card);
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
int FUN_004f7783(int player, int card)
{
  if ((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0)
  {
    PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
    if ((global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].extra_ability & 0x1000) != 0)
    {
      produced_mana_color = -1;
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
    FUN_004e4ff3(0);
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

  if (((event == EVENT_CAST_SPELL) && (card == affected_card)) && (player == affected_card_controller))
  {
    load_text((int)"prompts.txt", "TUNNEL");
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
                              text_lines[0],
                              1,
                              &instance->targets[0]))
    {
      spell_fizzled = 1;
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
      spell_fizzled = 1;
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
    if (((player == active_player) && ((unk_00926804 & 2) == 0)) && has_mana(player, 7, 2) == 0)
    {
      return 0;
    }
    FUN_004e4ff3(0);
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

  if (((event == EVENT_CAST_SPELL) && (card == affected_card)) && (player == affected_card_controller))
  {
    load_text((int)"prompts.txt", "HOWL_FROM_BEYOND");
    if (!FUN_00551638(player, player, card))
    {
      spell_fizzled = 1;
    }
    else
    {
      ai_modifier += ((((current_phase < 0x15) - 1) & -2) * 3 + 9) * -4;
      instance->info_slot = x_value;
      if ((player == active_player) && (PLAYER_CARD_INSTANCE(instance->targets[0].player, instance->targets[0].card).state & 0x30000) != 0)
      {
        ai_modifier += -99;
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
      spell_fizzled = 1;
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
    FUN_004e4ff3(0);
    if (current_phase < 0x1e && real_target_available((int *)0,
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

  if (((event == EVENT_CAST_SPELL) && (card == affected_card)) && (player == affected_card_controller))
  {
    load_text((int)"prompts.txt", "BERSERK");
    if (!FUN_00551638(player, player, card))
    {
      spell_fizzled = 1;
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
      spell_fizzled = 1;
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
    FUN_004e4ff3(0);
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

  if (((event == EVENT_CAST_SPELL) && (card == affected_card)) && (player == affected_card_controller))
  {
    load_text((int)"prompts.txt", "RIGHTEOUSNESS");
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
                              text_lines[0],
                              1,
                              &selected_target))
    {
      spell_fizzled = 1;
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
      spell_fizzled = 1;
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
    FUN_004e4ff3(0);
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

  if (((event == EVENT_CAST_SPELL) && (card == affected_card)) && (player == affected_card_controller))
  {
    load_text((int)"prompts.txt", "SWORD_TO_PLOWSHARES");
    if (!FUN_00551638(player, 1 - player, card))
    {
      spell_fizzled = 1;
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
      spell_fizzled = 1;
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
  card_instance_t *instance;
  int current_player;
  int current_card;
  int internal_card_id;
  int target_player;
  int target_card;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if ((event == EVENT_CAN_CAST) && (unk_008b4278 & 2) != 0)
  {
    FUN_004e4ff3(0);
    for (current_player = 0; current_player < 2; ++current_player)
    {
      for (current_card = 0; current_card < active_cards_count[current_player]; ++current_card)
      {
        internal_card_id = PLAYER_CARD_INSTANCE(current_player, current_card).internal_card_id;
        if (is_in_play(current_player, current_card) && (global_cards_data[internal_card_id].type & TYPE_CREATURE) != 0 && PLAYER_CARD_INSTANCE(current_player, current_card).mana_color == 2)
        {
          return 99;
        }
      }
    }
    return 0;
  }

  if (((event == EVENT_CAST_SPELL) && (card == affected_card)) && ((player == affected_card_controller) && (unk_008b4278 & 2) != 0))
  {
    int found_dead_creature;

    found_dead_creature = 0;
    do
    {
      instance->number_of_targets = 0;
      load_text((int)"prompts.txt", "DEATH_WARD");
      if (!C_real_select_target(player,
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
                                0,
                                text_lines[0],
                                1,
                                &instance->targets[instance->number_of_targets]))
      {
        spell_fizzled = 1;
      }
      else
      {
        ++instance->number_of_targets;
      }
      target_player = instance->targets[0].player;
      target_card = instance->targets[0].card;
      if (PLAYER_CARD_INSTANCE(target_player, target_card).mana_color == 2)
      {
        if (((PLAYER_CARD_INSTANCE(target_player, target_card).token_status & 2) != 0) || target_player == unk_008b35ec)
        {
          ai_modifier -= 0x30;
        }
        found_dead_creature = 1;
      }
      else if (unk_008a9000 == 1)
      {
        FUN_004e51bb();
      }
      else
      {
        load_text((int)"prompts.txt", "DEATH_WARD2");
        FUN_004a61d6(text_lines[1]);
        Sleep(0x9c4);
        FUN_004a61d6("");
      }
    } while ((spell_fizzled != 1) && !found_dead_creature);
  }

  if ((event == EVENT_RESOLVE_SPELL) && (unk_008b4278 & 2) != 0)
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
      spell_fizzled = 1;
    }
    else
    {
      FUN_00542a2a(target_player, target_card);
    }
    instance->number_of_targets = 0;
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
    FUN_004e4ff3(0);
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

  if (((event == EVENT_CAST_SPELL) && (card == affected_card)) && (player == affected_card_controller))
  {
    load_text((int)"prompts.txt", "JUMP");
    if (!FUN_00551638(player, player, card))
    {
      spell_fizzled = 1;
    }
    else
    {
      if (instance->targets[0].player == unk_008b35ec)
      {
        ai_modifier -= 0x18;
      }
      if ((PLAYER_CARD_INSTANCE(instance->targets[0].player, instance->targets[0].card).regen_status & 0x20) != 0)
      {
        ai_modifier += -99;
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
      spell_fizzled = 1;
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
    FUN_004e4ff3(0);
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

  if (((event == EVENT_CAST_SPELL) && (affected_card == card)) && (player == affected_card_controller))
  {
    load_text((int)"prompts.txt", "TERROR");
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
                              text_lines[0],
                              1,
                              &instance->targets[0]))
    {
      spell_fizzled = 1;
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
      spell_fizzled = 1;
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
    FUN_004e4ff3(1);
    return 1;
  }

  if (((event == EVENT_CAST_SPELL) && (card == affected_card)) && (player == affected_card_controller))
  {
    load_text((int)"prompts.txt", "LIGHTNING_BOLT");
    FUN_0054ac4d(player, card, 3);
    if (spell_fizzled != 1)
    {
      ai_modifier -= 0x24;
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    FUN_0054af10(player, card, EVENT_RESOLVE_SPELL, 3);
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
    FUN_004e4ff3(0);
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

  if (((event == EVENT_CAST_SPELL) && (affected_card == card)) && (affected_card_controller == player))
  {
    ai_modifier += ((((current_phase < 0x15) - 1) & -2) * 3 + 0xc) * -4;
    if (current_phase < 0x15)
    {
      ai_modifier -= 0xc;
    }
    load_text((int)"prompts.txt", "GIANT_GROWTH");
    if (!FUN_00551638(player, player, card))
    {
      spell_fizzled = 1;
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
      spell_fizzled = 1;
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
    FUN_004e4ff3(0);
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

  if (((event == EVENT_CAST_SPELL) && (affected_card == card)) && (affected_card_controller == player))
  {
    ai_modifier -= 0x30;
    load_text((int)"prompts.txt", "UNSUMMON");
    if (!FUN_00551638(player, 1 - player, card))
    {
      spell_fizzled = 1;
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
      spell_fizzled = 1;
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

// FUNCTION: MAGIC 0x004fa6e3
// FUNCTION: SHANDALAR 0x004b47b1
int card_sandstorm(int player, int card, event_t event)
{

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
    if (unk_008ce508 == -1)
    {
      FUN_004e4ff3(0);
      return real_target_available((int *)0, TARGET_SCAN_DIRECT, player, 2, 2, 0x200, 0xff, 0, 0,
                                   get_protections_from(player, card), 0, 0, -1, -1, 0xffffffff,
                                   0xffffffff, 0, 0, 0);
    }

    if ((((player == unk_008b35ec) || ((unk_00926804 & 2) != 0)) || ((active_player == player) && (unk_008b35ec == human_player))) && C_real_validate_target(unk_008ce508, unk_008ce4f4, (char *)0, player, 2, 2, 0,
                                                                                                                                                             TYPE_EFFECT | TYPE_ARTIFACT | TYPE_INTERRUPT | TYPE_INSTANT | TYPE_SORCERY |
                                                                                                                                                                 TYPE_ENCHANTMENT | TYPE_CREATURE | TYPE_LAND,
                                                                                                                                                             TYPE_NONE, 0, 0, COLOR_TEST_0, COLOR_TEST_0, -1,
                                                                                                                                                             ~SUB_WALL, -1, -1, TARGET_SPECIAL_SPELL_ON_STACK, 0, 0))
    {
      return 99;
    }
    return 0;
  }

  if ((event == EVENT_CAST_SPELL) && (card == card_on_stack) && (player == card_on_stack_controller))
  {
    ai_modifier -= 0x18;
    if (unk_008ce508 == -1)
    {
      load_text((int)"prompts.txt", "ANY_LACE");
      if (!C_real_select_target(player, 2, 2, TARGET_ZONE_IN_PLAY,
                                TYPE_EFFECT | TYPE_ARTIFACT | TYPE_INTERRUPT | TYPE_INSTANT | TYPE_SORCERY |
                                    TYPE_ENCHANTMENT | TYPE_CREATURE | TYPE_LAND,
                                TYPE_NONE, 0, get_protections_from(player, card), COLOR_TEST_0,
                                COLOR_TEST_0, -1, ~SUB_WALL, -1, -1, 0, 0, 0,
                                text_lines[0], 1, &target))
      {
        spell_fizzled = 1;
      }
      else
      {
        instance->targets[0].player = target.player;
        instance->targets[0].card = target.card;
        instance->number_of_targets = 1;
        if ((active_player == player) && (((global_cards_data[instance->internal_card_id].color & PLAYER_CARD_INSTANCE(target.player, target.card).color) != 0) || (target.player == unk_008b35ec)))
        {
          ai_modifier -= 0x30;
        }
      }
    }
    else
    {
      instance->targets[0].player = unk_008ce508;
      instance->targets[0].card = unk_008ce4f4;
      instance->number_of_targets = 1;
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    if (unk_008ce508 == -1)
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
      spell_fizzled = 1;
    }
    else
    {
      color = single_color_test_bit_to_color_t(global_cards_data[instance->internal_card_id].color);
      color = get_sleighted_color(player, card, color);
      target_instance = &PLAYER_CARD_INSTANCE(instance->targets[0].player, instance->targets[0].card);
      target_instance->color = (char)(1 << ((unsigned char)color & 0x1f));
      target_instance->state |= 0x2000;
      if (unk_008a9000 != 1)
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
void FUN_004faee2(int player, int card, int color_from, unsigned char color_to)
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
    if ((unk_008b35ec == player) || (((unsigned char)unk_00926804 & 2) != 0))
    {
      if (unk_008ce508 == -1)
      {
        FUN_004e4ff3(0);
        return 1;
      }
      return 99;
    }
    if ((unk_008ce508 == -1) || (unk_008b35ec != human_player))
    {
      FUN_004e4ff3(0);
      return 1;
    }
    return 99;
  }

  if (event == EVENT_CAST_SPELL && affected_card == card && affected_card_controller == player)
  {
    if (unk_008ce508 == -1)
    {
      load_text((int)"prompts.txt", "MAGICAL_HACK");
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
                               text_lines[0],
                               1,
                               &s.target) == 0)
      {
        spell_fizzled = 1;
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
      PLAYER_CARD_INSTANCE(player, card).targets[0].player = unk_008ce508;
      PLAYER_CARD_INSTANCE(player, card).targets[0].card = unk_008ce4f4;
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
    }

    if (spell_fizzled != 1)
    {
      if ((player == active_player) && (((unsigned char)unk_00926804 & 2) == 0))
      {
        target_iid =
            PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).targets[0].player, PLAYER_CARD_INSTANCE(player, card).targets[0].card)
                .internal_card_id;
        raw_card = &global_raw_cards_storage[global_cards_data[target_iid].id];
        s.available_colors = *(unsigned int *)&raw_card->hack_colors;
        if (s.available_colors == 0)
        {
          spell_fizzled = 1;
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

          if (unk_008a9000 == 1)
          {
            unk_00939340 = s.old_color;
            FUN_004e4f11();
            unk_00939340 = s.new_color;
            FUN_004e4f11();
          }
          else
          {
            FUN_004e5089();
            s.old_color = unk_00939340;
            FUN_004e5089();
            s.new_color = unk_00939340;
          }

          PLAYER_CARD_INSTANCE(player, card).info_slot = s.new_color * 0x100 + s.old_color;
          if (unk_008a9000 != 1)
          {
            load_text((int)"prompts.txt", "MAGICAL_HACK");
            strcpy(s.prompt, "\n");
            sprintf(s.prompt + strlen(s.prompt), text_lines[1], "", "");
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

    if (spell_fizzled == 1)
    {
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    s.target.player = PLAYER_CARD_INSTANCE(player, card).targets[0].player;
    s.target.card = PLAYER_CARD_INSTANCE(player, card).targets[0].card;
    if (unk_008a9000 != 1)
    {
      play_sound_effect(WAV_CHANGET);
    }
    PLAYER_CARD_INSTANCE(s.target.player, s.target.card).token_status |= STATUS_HACKED;
    FUN_004faee2(s.target.player,
                 s.target.card,
                 (unsigned char)PLAYER_CARD_INSTANCE(player, card).info_slot,
                 *((unsigned char *)&PLAYER_CARD_INSTANCE(player, card).info_slot + 1));
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004fb8fd
void FUN_004fb8fd(int player, int card, int color_from, unsigned char color_to)
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
  card_instance_t *instance;
  target_t target;
  unsigned int available_colors;
  int target_iid;
  int old_color;
  int new_color;

  if (event == EVENT_CAN_CAST)
  {
    if ((unk_008b35ec == player) || ((unk_00926804 & 2) != 0))
    {
      if (unk_008ce508 == -1)
      {
        FUN_004e4ff3(0);
        return 1;
      }
      return 99;
    }
    if (unk_008ce508 == -1 || unk_008b35ec != human_player)
    {
      FUN_004e4ff3(0);
      return 1;
    }
    return 99;
  }

  instance = &PLAYER_CARD_INSTANCE(player, card);
  if (event == EVENT_CAST_SPELL && affected_card == card && affected_card_controller == player)
  {
    if (unk_008ce508 == -1)
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
                               text_lines[0],
                               1,
                               &target) == 0)
      {
        spell_fizzled = 1;
      }
      else
      {
        instance->targets[0] = target;
        instance->number_of_targets = 1;
      }
    }
    else
    {
      instance->targets[0].player = unk_008ce508;
      instance->targets[0].card = unk_008ce4f4;
      instance->number_of_targets = 1;
    }

    if (spell_fizzled != 1)
    {
      target_iid =
          PLAYER_CARD_INSTANCE(instance->targets[0].player, instance->targets[0].card).internal_card_id;
      available_colors = global_raw_cards_storage[global_cards_data[target_iid].id].sleight_color;
      if (available_colors == 0)
      {
        spell_fizzled = 1;
      }
      else
      {
        if ((PLAYER_CARD_INSTANCE(instance->targets[0].player, instance->targets[0].card).token_status & STATUS_SLEIGHTED) != 0)
        {
          old_color = single_color_test_bit_to_color_t((unsigned char)available_colors);
          old_color = get_sleighted_color(instance->targets[0].player, instance->targets[0].card, old_color);
          available_colors = 1 << ((unsigned char)old_color & 0x1f);
        }

        do
        {
          old_color = internal_rand(5) + 1;
        } while ((available_colors & (1 << ((unsigned char)old_color & 0x1f))) == 0);

        do
        {
          new_color = internal_rand(5) + 1;
        } while (new_color == old_color);

        if (unk_008a9000 == 1)
        {
          unk_00939340 = old_color;
          FUN_004e4f11();
          unk_00939340 = new_color;
          FUN_004e4f11();
        }
        else
        {
          FUN_004e5089();
          old_color = unk_00939340;
          FUN_004e5089();
          new_color = unk_00939340;
        }

        instance->info_slot = new_color * 0x100 + old_color;
      }
    }

    if (spell_fizzled == 1)
    {
      instance->number_of_targets = 0;
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    target = instance->targets[0];
    if (unk_008a9000 != 1)
    {
      play_sound_effect(WAV_CHANGET);
    }
    PLAYER_CARD_INSTANCE(target.player, target.card).token_status |= STATUS_SLEIGHTED;
    FUN_004fb8fd(target.player,
                 target.card,
                 (unsigned char)instance->info_slot,
                 (unsigned char)((unsigned int)instance->info_slot >> 8));
    instance->number_of_targets = 0;
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
    if (unk_008ce508 == -1)
    {
      FUN_004e4ff3(0);
      color = get_sleighted_color(player, card, 4);
      return real_target_available((int *)0, TARGET_SCAN_DIRECT, player, 2, 2, 0x200, 0x1047, 0, 0,
                                   get_protections_from(player, card), 1 << ((unsigned char)color & 0x1f), 0, -1, -1,
                                   0xffffffff, 0xffffffff, 0, 0, 0);
    }
    color = get_sleighted_color(player, card, 4);
    return C_real_validate_target(unk_008ce508, unk_008ce4f4, (char *)0, player, 2, 2, 0,
                                  TYPE_NONE, TYPE_NONE, 0, 0, 1 << ((unsigned char)color & 0x1f),
                                  COLOR_TEST_0, -1, ~SUB_WALL, -1, -1,
                                  TARGET_SPECIAL_SPELL_ON_STACK, 0, 0)
               ? 99
               : 0;
  }

  if ((event == EVENT_CAST_SPELL) && (card == card_on_stack) && (player == card_on_stack_controller))
  {
    if (unk_008ce508 == -1)
    {
      load_text((int)"prompts.txt", "BLUE_ELEMENTAL_BLAST");
      color = get_sleighted_color(player, card, 4);
      if (!C_real_select_target(player, 2, 1 - player, TARGET_ZONE_IN_PLAY,
                                TARGET_TYPE_TOKEN | TYPE_ARTIFACT | TYPE_ENCHANTMENT | TYPE_CREATURE | TYPE_LAND,
                                TYPE_NONE, 0, get_protections_from(player, card),
                                1 << ((unsigned char)color & 0x1f), COLOR_TEST_0, -1, ~SUB_WALL, -1, -1,
                                0, 0, 0, text_lines[0], 1, &target))
      {
        spell_fizzled = 1;
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
      instance->targets[0].player = unk_008ce508;
      instance->targets[0].card = unk_008ce4f4;
      instance->number_of_targets = 1;
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    valid = 0;
    if (unk_008ce508 == -1)
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
      spell_fizzled = 1;
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
    FUN_004e4ff3(0);
    if (unk_008ce508 == -1)
    {
      return 0;
    }
    return C_real_validate_target(unk_008ce508, unk_008ce4f4, (char *)0, player, 2, 2, 0,
                                  TYPE_NONE, TYPE_NONE, 0, 0, COLOR_TEST_0, COLOR_TEST_0,
                                  -1, ~SUB_WALL, -1, -1, TARGET_SPECIAL_SPELL_ON_STACK, 0, 0)
               ? 99
               : 0;
  }

  if ((event == EVENT_CAST_SPELL) && (card == card_on_stack) && (player == card_on_stack_controller))
  {
    if (unk_008ce508 == -1)
    {
      spell_fizzled = 1;
    }
    else
    {
      instance->targets[0].player = unk_008ce508;
      instance->targets[0].card = unk_008ce4f4;
      instance->number_of_targets = 1;
    }
    ai_modifier -= 0x24;
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
      spell_fizzled = 1;
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
  card_instance_t *instance;
  int target_player;
  int target_card;
  int mana_paid;
  int color;
  int current_card;
  card_instance_t *target_instance;
  int internal_card_id;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_CAN_CAST)
  {
    FUN_004e4ff3(0);
    if (unk_008ce508 == -1)
    {
      return 0;
    }
    if (player == active_player && (unk_00926804 & 2) == 0 && has_mana(player, COLOR_COLORLESS, 2) == 0)
    {
      return 0;
    }
    if (!C_real_validate_target(unk_008ce508,
                                unk_008ce4f4,
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

  if (event == EVENT_CAST_SPELL && card == affected_card && player == affected_card_controller)
  {
    if (unk_008ce508 == -1)
    {
      spell_fizzled = 1;
    }
    else
    {
      instance->targets[0].player = unk_008ce508;
      instance->targets[0].card = unk_008ce4f4;
      instance->number_of_targets = 1;
      instance->info_slot = x_value;
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
      spell_fizzled = 1;
    }
    else
    {
      target_player = instance->targets[0].player;
      target_card = instance->targets[0].card;

      if (target_player == human_player || (unk_00926804 & 2) != 0)
      {
        FUN_00443ee2(player, card, 0x7e, 0, 0);
        mana_paid = charge_mana(target_player, 0, instance->info_slot);
        obliterate_top_card_of_stack();
        spell_fizzled = 0;
      }
      else if (has_mana(target_player, COLOR_COLORLESS, 1) == 0)
      {
        mana_paid = 0;
      }
      else
      {
        mana_paid = charge_mana(target_player, 0, instance->info_slot);
      }

      if (mana_paid < instance->info_slot)
      {
        for (color = 0; color < 7 && mana_paid < instance->info_slot; ++color)
        {
          while (raw_mana_available[target_player][color] > 0 && mana_paid < instance->info_slot)
          {
            --raw_mana_available[target_player][color];
            --raw_mana_available[target_player][7];
            if (color != 6)
            {
              ++mana_paid;
            }
          }
        }

        unk_008ce510[6] = instance->info_slot;
        for (current_card = 0; current_card < active_cards_count[target_player] && mana_paid < instance->info_slot;
             ++current_card)
        {
          if (is_in_play(target_player, current_card))
          {
            target_instance = &PLAYER_CARD_INSTANCE(target_player, current_card);
            internal_card_id = target_instance->internal_card_id;
            if ((global_cards_data[internal_card_id].type & TYPE_LAND) != 0 && (global_cards_data[internal_card_id].extra_ability & 0x1000) != 0 && (target_instance->state & STATE_TAPPED) == 0 && (((target_instance->state & 3) == 0) || (global_cards_data[internal_card_id].type & TYPE_CREATURE) == 0))
            {
              drain_power_draw_mana_from_land(target_player, current_card, internal_card_id);
              for (color = 0; color < 7 && mana_paid < instance->info_slot; ++color)
              {
                while (raw_mana_available[target_player][color] > 0 && mana_paid < instance->info_slot)
                {
                  --raw_mana_available[target_player][color];
                  --raw_mana_available[target_player][7];
                  if (color != 6)
                  {
                    ++mana_paid;
                  }
                }
              }
            }
          }
        }
        unk_008ce510[6] = 0;
      }

      if (mana_paid < instance->info_slot)
      {
        kill_card(target_player, target_card, KILL_BURY);
      }
    }

    instance->number_of_targets = 0;
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
    FUN_004e4ff3(0);
    if (unk_008ce508 == -1)
    {
      return 0;
    }

    if ((active_player == player) && ((unk_00926804 & 2) == 0) && !has_mana_w_global_cost_mod(player, card, 7, 2))
    {
      return 0;
    }

    cost = (int)(char)global_cards_data[PLAYER_CARD_INSTANCE(unk_008ce508, unk_008ce4f4).internal_card_id].cc[1];
    if (cost == -1)
    {
      cost = x_value;
    }
    cost += (int)(char)global_cards_data[PLAYER_CARD_INSTANCE(unk_008ce508, unk_008ce4f4).internal_card_id].cc[0];
    instance->info_slot = cost;

    if (!has_mana(player, 2, 1) || !has_mana_w_global_cost_mod(player, card, 7, cost + 1) || !C_real_validate_target(unk_008ce508, unk_008ce4f4, (char *)0, player, 2, 2, 0, TYPE_NONE, TYPE_NONE, 0, 0, COLOR_TEST_0, COLOR_TEST_0, -1, ~SUB_WALL, -1, -1, TARGET_SPECIAL_SPELL_ON_STACK, 0, 0))
    {
      return 0;
    }

    unk_0091bfb4 = 1;
    return 99;
  }

  if ((event == EVENT_CAST_SPELL) && (card == card_on_stack) && (player == card_on_stack_controller) && (unk_008ce508 != -1))
  {
    unk_008ce518 = 1;
    charge_mana_w_global_cost_mod(player, card, 0, instance->info_slot);
    if (spell_fizzled != 1)
    {
      instance->targets[0].player = unk_008ce508;
      instance->targets[0].card = unk_008ce4f4;
      instance->number_of_targets = 1;
      x_value = instance->info_slot;
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    if (!C_real_validate_target(instance->targets[0].player, instance->targets[0].card, (char *)0,
                                player, 2, 2, 0, TYPE_NONE, TYPE_NONE, 0, 0, COLOR_TEST_0,
                                COLOR_TEST_0, -1, ~SUB_WALL, -1, -1,
                                TARGET_SPECIAL_SPELL_ON_STACK, 0, 0))
    {
      spell_fizzled = 1;
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
    if (unk_008ce508 == -1)
    {
      FUN_004e4ff3(0);
      color = get_sleighted_color(player, card, 2);
      return real_target_available((int *)0, TARGET_SCAN_DIRECT, player, 2, 2, 0x200, 0x1047, 0, 0,
                                   get_protections_from(player, card), 1 << ((unsigned char)color & 0x1f), 0, -1, -1,
                                   0xffffffff, 0xffffffff, 0, 0, 0);
    }
    color = get_sleighted_color(player, card, 2);
    return C_real_validate_target(unk_008ce508, unk_008ce4f4, (char *)0, player, 2, 2, 0,
                                  TYPE_NONE, TYPE_NONE, 0, 0, 1 << ((unsigned char)color & 0x1f),
                                  COLOR_TEST_0, -1, ~SUB_WALL, -1, -1,
                                  TARGET_SPECIAL_SPELL_ON_STACK, 0, 0)
               ? 99
               : 0;
  }

  if ((event == EVENT_CAST_SPELL) && (card == card_on_stack) && (player == card_on_stack_controller))
  {
    if (unk_008ce508 == -1)
    {
      load_text((int)"prompts.txt", "RED_ELEMENTAL_BLAST");
      color = get_sleighted_color(player, card, 2);
      if (!C_real_select_target(player, 2, 1 - player, TARGET_ZONE_IN_PLAY,
                                TARGET_TYPE_TOKEN | TYPE_ARTIFACT | TYPE_ENCHANTMENT | TYPE_CREATURE | TYPE_LAND,
                                TYPE_NONE, 0, get_protections_from(player, card),
                                1 << ((unsigned char)color & 0x1f), COLOR_TEST_0, -1, ~SUB_WALL, -1, -1,
                                0, 0, 0, text_lines[0], 1, &target))
      {
        spell_fizzled = 1;
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
      instance->targets[0].player = unk_008ce508;
      instance->targets[0].card = unk_008ce4f4;
      instance->number_of_targets = 1;
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    valid = 0;
    if (unk_008ce508 == -1)
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
      spell_fizzled = 1;
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

  if ((event == EVENT_CAST_SPELL) && (card == affected_card) && (player == affected_card_controller))
  {
    if (hand_count[player] < 8)
    {
      ai_modifier -= 0x3c;
    }
    else
    {
      ai_modifier -= 0x18;
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

}

// FUNCTION: MAGIC 0x004fdfb2
// FUNCTION: SHANDALAR 0x004b80dc
int card_healing_salve(int player, int card, event_t event)
{
  return gain_life_or_prevent_damage(player, card, event, 3);
}

// FUNCTION: MAGIC 0x004fdfd8
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
    if (((player == active_player) && ((unk_00926804 & 2) == 0)) && (amount == 0))
    {
      return 0;
    }
    if ((unk_008b4278 & 4) == 0)
    {
      return 1;
    }
    return 99;
  }

  if (((event == EVENT_CAST_SPELL) && (affected_card == card)) && (affected_card_controller == player))
  {
    ai_modifier -= 0x60;
    if ((unk_008b4278 & 4) == 0)
    {
      load_text((int)"prompts.txt", "HEALING_SALVE");
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
                                text_lines[0],
                                1,
                                &selected_target))
      {
        spell_fizzled = 1;
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
      spell_fizzled = -1;
      while ((instance->number_of_targets < amount) && !cancelled && (spell_fizzled != 1) && !done)
      {
        load_text((int)"prompts.txt", "HEALING_SALVE2");
        strcpy(prompt, text_lines[1]);
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
                                  unk_009266a4,
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
            spell_fizzled = 1;
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
          TENTATIVE_reassess_all_cards();
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
        else if (unk_008a9000 == 1)
        {
          FUN_004e51bb();
        }
        else
        {
          load_text((int)"prompts.txt", "HEALING_SALVE3");
          FUN_004a61d6(text_lines[2]);
          Sleep(0x9c4);
          FUN_004a61d6("");
        }
      }
      for (target_index = 0; target_index < instance->number_of_targets; ++target_index)
      {
        PLAYER_CARD_INSTANCE(instance->targets[target_index].player, instance->targets[target_index].card).state &= ~0x300000;
      }
      if (spell_fizzled == 1)
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
      if ((unk_008b4278 & 4) == 0)
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
                                       unk_009266a4,
                                       ~SUB_WALL,
                                       -1,
                                       -1,
                                       0,
                                       0,
                                       0))
      {
        spell_fizzled = 1;
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
    FUN_004e4ff3(0);
    if ((unk_008b4278 & 4) == 0)
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
                                     unk_009266a4,
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
    if ((((event == EVENT_CAST_SPELL) && (card == affected_card)) && (player == affected_card_controller)) && ((unk_008b4278 & 4) != 0))
    {
      load_text((int)"prompts.txt", "REVERSE_DAMAGE");
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
                                    unk_009266a4,
                                    ~SUB_WALL,
                                    -1,
                                    -1,
                                    TARGET_SPECIAL_DAMAGE_PLAYER,
                                    0,
                                    0,
                                    text_lines[0],
                                    1,
                                    &s.target);
      if (result == 0)
      {
        spell_fizzled = 1;
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
      if ((unk_008b4278 & 4) == 0)
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
          if (((player == active_player) && ((unk_00926804 & 2) == 0)) || (unk_008a9000 == 1))
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
            load_text((int)"prompts.txt", "REVERSE_DAMAGE");
            s.selected = FUN_004b413c(player, s.internal_card_ids, s.damage_amounts, s.count, 0x89684c, 1, text_lines[1]);
          }
          gain_life(player,
                    unk_0093b280[s.damage_sources[s.selected * 2]][s.damage_sources[s.selected * 2 + 1]][player][0] *
                        2,
                    player, card);
          unk_0093b280[s.damage_sources[s.selected * 2]][s.damage_sources[s.selected * 2 + 1]][player][0] = 0;
        }
        if (life[1 - player] < 1)
        {
          ai_modifier += 1000;
        }
        else
        {
          ai_modifier += (s.best_damage * 100) / life[1 - player] - 100;
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
                                        unk_009266a4,
                                        ~SUB_WALL,
                                        -1,
                                        -1,
                                        TARGET_SPECIAL_DAMAGE_PLAYER,
                                        0,
                                        0);
        if (result == 0)
        {
          spell_fizzled = 1;
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

