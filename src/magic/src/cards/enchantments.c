#include <string.h>
#include "../game_support.h"
#include "../global_strings.h"

// FUNCTION: MAGIC 0x00435ef0
// FUNCTION: SHANDALAR 0x004f8890
int card_artifact_possession(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x0043641d
// FUNCTION: SHANDALAR 0x004f8dbe
int card_artifact_ward(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x00436955
// FUNCTION: SHANDALAR 0x004f92f9
int card_consecrate_land(int player, int card, event_t event)
{
  card_instance_t *instance;
  target_t target;

  instance = &PLAYER_CARD_INSTANCE(player, card);
  if (event == EVENT_CAN_CAST)
  {
    return real_target_available((int *)0,
                                 TARGET_SCAN_DIRECT,
                                 player,
                                 2,
                                 2,
                                 TARGET_ZONE_IN_PLAY,
                                 TYPE_LAND,
                                 TYPE_NONE,
                                 0,
                                 get_protections_from(player, card),
                                 COLOR_TEST_0,
                                 COLOR_TEST_0,
                                 -1,
                                 -1,
                                 0xffffffff,
                                 0xffffffff,
                                 0,
                                 0,
                                 0);
  }

  if ((event == EVENT_CAST_SPELL) && (card == card_on_stack) && (player == card_on_stack_controller))
  {
    load_text("promptsX1.txt", "CONSECRATE_LAND");
    if (!C_real_select_target(player,
                              2,
                              2,
                              TARGET_ZONE_IN_PLAY,
                              TYPE_LAND,
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
      instance->number_of_targets = 1;
      if (player == active_player && (unk_00926804 & 2) == 0)
      {
        if (target.player == unk_008b35ec)
        {
          ai_modifier -= 0x30;
        }
        else
        {
          if ((global_cards_data[PLAYER_CARD_INSTANCE(target.player, target.card).internal_card_id].type & TYPE_CREATURE) != 0)
          {
            ai_modifier += 0x60;
          }
        }
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
                                TYPE_LAND,
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
      kill_card(player, card, KILL_BURY);
      spell_fizzled = 1;
    }
    else
    {
      instance->damage_target_player = instance->targets[0].player;
      instance->damage_target_card = instance->targets[0].card;
      *(int *)((char *)&PLAYER_CARD_INSTANCE(instance->damage_target_player, instance->damage_target_card) + 0x14) |= 0x4000000;
      FUN_0055117d(FUN_00437375, -1);
    }
    instance->number_of_targets = 0;
  }

  if (event == 0x34 && instance->damage_target_card == card_on_stack && instance->damage_target_player == card_on_stack_controller && card_on_stack != -1)
  {
    event_result |= 0x20000;
  }
  if (event == 0x77 && card_on_stack != -1 && *(unsigned char *)((char *)&PLAYER_CARD_INSTANCE(card_on_stack_controller, card_on_stack) + 0x64) == 2 && instance->damage_target_card == card_on_stack && instance->damage_target_player == card_on_stack_controller)
  {
    *(unsigned char *)((char *)&PLAYER_CARD_INSTANCE(card_on_stack_controller, card_on_stack) + 0x64) = 0;
    *(int *)((char *)&PLAYER_CARD_INSTANCE(card_on_stack_controller, card_on_stack) + 0x10) = 0;
    ++event_result;
  }
  if (event == 0x77 && card == card_on_stack && player == card_on_stack_controller && instance->damage_target_player != -1 && instance->damage_target_card != -1 && PLAYER_CARD_INSTANCE(instance->damage_target_player, instance->damage_target_card).internal_card_id != -1)
  {
    *(int *)((char *)&PLAYER_CARD_INSTANCE(instance->damage_target_player, instance->damage_target_card) + 0x14) &= ~0x4000000;
  }
  if (trigger_condition == 0xd4 && card == card_on_stack && player == card_on_stack_controller && instance->damage_target_player != -1 && instance->damage_target_card != -1 && PLAYER_CARD_INSTANCE(instance->damage_target_player, instance->damage_target_card).internal_card_id != -1 && player == trigger_cause_controller && trigger_cause == card && player == current_turn)
  {
    if (event == 0x7d)
    {
      event_result |= 2;
    }
    if (event == 0x7e)
    {
      *(int *)((char *)&PLAYER_CARD_INSTANCE(instance->damage_target_player, instance->damage_target_card) + 0x14) &= ~0x4000000;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x00437375
int FUN_00437375(int player, int card, int internal_card_id)
{
  if ((int)(char)PLAYER_CARD_INSTANCE(player, card).damage_target_player ==
      (int)(char)PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).damage_target_player)
  {
    if (PLAYER_CARD_INSTANCE(player, card).damage_target_card ==
        PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).damage_target_card)
    {
      if (((unsigned char)global_cards_data[internal_card_id].type & TYPE_ENCHANTMENT) != 0)
      {
        if (card != affected_card || player != affected_card_controller)
        {
          kill_card(player, card, 1);
        }
      }
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x004374a1
// FUNCTION: SHANDALAR 0x004f9e49
int card_cyclone(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x00437aed
// FUNCTION: SHANDALAR 0x004fa491
int card_damping_field(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x00437f75
// FUNCTION: SHANDALAR 0x004fa91d
int card_drop_of_honey(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x004385c3
// FUNCTION: SHANDALAR 0x004faf62
int card_earthbind(int player, int card, event_t event)
{
  target_t selected_target;
  unsigned int illegal_abilities;

  if (event == EVENT_CAN_CAST)
  {
    FUN_004e4ff3(0);
    return real_target_available((int *)0,
                                 TARGET_SCAN_DIRECT,
                                 player,
                                 2,
                                 1 - player,
                                 0x200,
                                 2,
                                 0,
                                 0,
                                 get_protections_from(player, card),
                                 0,
                                 0,
                                 -1,
                                 -1,
                                 -1,
                                 -1,
                                 0,
                                 0,
                                 0);
  }
  else
  {
    if (((event == EVENT_CAST_SPELL) && (card == affected_card)) && (player == affected_card_controller))
    {
      load_text("promptsX1.txt", "EARTHBIND");
      illegal_abilities = get_protections_from(player, card);
      if (C_real_select_target(player,
                               2,
                               1 - player,
                               TARGET_ZONE_IN_PLAY,
                               TYPE_CREATURE,
                               TYPE_NONE,
                               0,
                               illegal_abilities,
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
                               &selected_target) == 0)
      {
        spell_fizzled = 1;
      }
      else
      {
        PLAYER_CARD_INSTANCE(player, card).targets[PLAYER_CARD_INSTANCE(player, card).number_of_targets] =
            selected_target;
        ++PLAYER_CARD_INSTANCE(player, card).number_of_targets;
        if ((PLAYER_CARD_INSTANCE(selected_target.player, selected_target.card).regen_status & KEYWORD_FLYING) == 0)
        {
          ai_modifier += -0x60;
        }
        else
        {
          ai_modifier += 0x18;
        }
      }
    }
    if (event == EVENT_RESOLVE_SPELL)
    {
      if (C_real_validate_target(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                                 PLAYER_CARD_INSTANCE(player, card).targets[0].card,
                                 (char *)0,
                                 player,
                                 ANYBODY,
                                 ANYBODY,
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
                                 0) == 0)
      {
        kill_card(player, card, KILL_BURY);
        spell_fizzled = 1;
      }
      else
      {
        PLAYER_CARD_INSTANCE(player, card).damage_target_player =
            (char)PLAYER_CARD_INSTANCE(player, card).targets[0].player;
        PLAYER_CARD_INSTANCE(player, card).damage_target_card =
            PLAYER_CARD_INSTANCE(player, card).targets[0].card;
      }
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    }
    if ((((event == EVENT_ABILITIES) && (PLAYER_CARD_INSTANCE(player, card).damage_target_card == affected_card)) && ((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player == affected_card_controller)) && (affected_card != -1))
    {
      event_result &= 0xffffffdf;
    }
    return 0;
  }
}

// FUNCTION: MAGIC 0x00438b5c
// FUNCTION: SHANDALAR 0x004fb4fd
int card_farmstead(int player, int card, event_t event)
{
  card_instance_t *instance;
  card_instance_t *parent;

  instance = &PLAYER_CARD_INSTANCE(player, card);
  parent = (card_instance_t *)0;

  if (event == EVENT_CAN_CAST)
  {
    return real_target_available((int *)0,
                                 TARGET_SCAN_DIRECT,
                                 player,
                                 2,
                                 player,
                                 TARGET_ZONE_IN_PLAY,
                                 TYPE_LAND,
                                 TYPE_NONE,
                                 0,
                                 get_protections_from(player, card),
                                 0,
                                 0,
                                 -1,
                                 -1,
                                 0xffffffff,
                                 0xffffffff,
                                 0,
                                 0,
                                 0);
  }
  else
  {
    if (event == EVENT_CAST_SPELL && affected_card == card && player == affected_card_controller)
    {
      load_text("promptsX1.txt", "FARMSTEAD");
      if (!FUN_00551b60(player, player, card))
      {
        spell_fizzled = 1;
      }
      if (spell_fizzled != 1)
      {
        if (instance->targets[0].player == unk_008b35ec)
        {
          ai_modifier -= 0x60;
        }
        if (instance->targets[0].player == active_player)
        {
          ai_modifier += 0x30;
        }
      }
    }

    if (event == EVENT_RESOLVE_SPELL)
    {
      if (C_real_validate_target(instance->targets[0].player,
                                 instance->targets[0].card,
                                 (char *)0,
                                 player,
                                 2,
                                 2,
                                 TARGET_ZONE_IN_PLAY,
                                 TYPE_LAND,
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
                                 0) == 0)
      {
        kill_card(player, card, KILL_BURY);
        spell_fizzled = 1;
      }
      else
      {
        instance->damage_target_player = instance->targets[0].player;
        instance->damage_target_card = instance->targets[0].card;
      }
      instance->number_of_targets = 0;
    }

    if (event == EVENT_CAN_ACTIVATE)
    {
      if (current_phase == 4 && instance->damage_target_player == human_player && instance->info_slot == 0 && has_mana_w_global_cost_mod(player, card, 5, 2) != 0)
      {
        if (player == active_player && (unk_00926804 & 2) == 0 && (internal_rand(100) < ((basiclandtypes_controlled[player][COLOR_WHITE] + 1) / 2) * 0x14 || life[active_player] < 5))
        {
          unk_008b3270 |= 3;
        }
        return 1;
      }
      return 0;
    }
    else
    {
      if (event == EVENT_ACTIVATE)
      {
        ++instance->info_slot;
        charge_mana_w_global_cost_mod(player, card, 5, 2);
        if (spell_fizzled == 1)
        {
          instance->info_slot = 0;
        }
        else
        {
          instance->targets[0].player = instance->damage_target_player;
          instance->targets[0].card = -1;
          instance->number_of_targets = 1;
        }
      }

      if (event == EVENT_RESOLVE_ACTIVATION)
      {
        gain_life(instance->targets[0].player, 1, player, card);
        parent = &PLAYER_CARD_INSTANCE(instance->parent_controller, instance->parent_card);
        parent->number_of_targets = 0;
      }

      if (event == EVENT_CLEANUP || event == EVENT_SHOULD_AI_PLAY)
      {
        instance->info_slot = 0;
      }

      return 0;
    }
  }
}

// FUNCTION: MAGIC 0x004391fe
// FUNCTION: SHANDALAR 0x004fbb9f
int card_fastbond(int player, int card, event_t event)
{
  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if (player == human_player && (unk_008b4278 & 1) != 0 && current_phase > 0x13 && current_phase < 0x1f)
  {
    unk_008b4278 &= ~1;
  }

  if ((trigger_condition == 0xdb || trigger_condition == 0xd3) && affected_card == card && affected_card_controller == player && unk_008cfdb0 > 0 && current_turn == player && trigger_cause_controller == player && PLAYER_CARD_INSTANCE(trigger_cause_controller, trigger_cause).internal_card_id != -1 && (global_cards_data[PLAYER_CARD_INSTANCE(trigger_cause_controller, trigger_cause).internal_card_id].type & TYPE_LAND) != 0 && (PLAYER_CARD_INSTANCE(player, card).state & 0x20) == 0)
  {
    if (event == 0x7d)
    {
      event_result |= 2;
    }
    if (event == 0x7e)
    {
      damage_player(player, 1, player, card);
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x004393af
// FUNCTION: SHANDALAR 0x004fbd52
int card_fishliver_oil(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x00439551
// FUNCTION: SHANDALAR 0x004fbeef
int card_gate_to_phyrexia(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x00439e81
// FUNCTION: SHANDALAR 0x004fc825
int card_haunting_wind(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x00439f56
// FUNCTION: SHANDALAR 0x004fc8fa
int card_invisibility(int player, int card, event_t event)
{
  card_instance_t *instance;

  instance = &PLAYER_CARD_INSTANCE(player, card);
  if (event == EVENT_CAN_CAST)
  {
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
                                 -1,
                                 0xffffffff,
                                 0xffffffff,
                                 0,
                                 0,
                                 0);
  }

  if ((event == EVENT_CAST_SPELL) && (card == card_on_stack) && (player == card_on_stack_controller))
  {
    load_text("promptsX1.txt", "INVISIBILITY");
    spell_fizzled = !FUN_00551638(player, player, card);
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
      kill_card(player, card, KILL_BURY);
      spell_fizzled = 1;
    }
    else
    {
      instance->damage_target_player = instance->targets[0].player;
      instance->damage_target_card = instance->targets[0].card;
    }
    instance->number_of_targets = 0;
  }

  if (event == 0x78 && instance->damage_target_card == attacking_card && instance->damage_target_player == attacking_card_controller && (instance->state & 0x20) == 0 && global_cards_data[PLAYER_CARD_INSTANCE(card_on_stack_controller, card_on_stack).internal_card_id].subtype != 0)
  {
    ++event_result;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0043a2e0
// FUNCTION: SHANDALAR 0x004fcc82
int card_jihad(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x0043a7ae
// FUNCTION: SHANDALAR 0x004fd152
int card_kudzu(int player, int card, event_t event)
{
  card_instance_t *instance;
  target_t new_target;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_CAN_CAST)
  {
    return real_target_available((int *)0,
                                 TARGET_SCAN_DIRECT,
                                 player,
                                 2,
                                 1 - player,
                                 TARGET_ZONE_IN_PLAY,
                                 TYPE_LAND,
                                 TYPE_NONE,
                                 0,
                                 get_protections_from(player, card),
                                 0,
                                 0,
                                 -1,
                                 -1,
                                 -1,
                                 ~SUB_WALL,
                                 0,
                                 0,
                                 0);
  }

  if (event == EVENT_CAST_SPELL && card == affected_card && player == affected_card_controller)
  {
    load_text("promptsX1.txt", "KUDZU");
    if (!FUN_00551b60(player, 1 - player, card))
    {
      spell_fizzled = 1;
    }
    else if (instance->targets[0].player == player)
    {
      ai_modifier -= 0x18;
    }
    else
    {
      ai_modifier += (basiclandtypes_controlled[1 - player][7] - basiclandtypes_controlled[player][7]) * 0xc;
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
                                TYPE_LAND,
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
      kill_card(player, card, KILL_BURY);
      spell_fizzled = 1;
    }
    else
    {
      instance->damage_target_player = (char)instance->targets[0].player;
      instance->damage_target_card = instance->targets[0].card;
    }
    instance->number_of_targets = 0;
    return 0;
  }

  if (event == 0x81 && instance->damage_target_card == affected_card && instance->damage_target_player == affected_card_controller && affected_card != -1 && (instance->state & 0x20) == 0)
  {
    load_text("promptsX1.txt", "KUDZU_2");

    new_target.player = -1;
    new_target.card = -1;

    if (instance->damage_target_player == human_player || (unk_00926804 & 2) != 0)
    {
      if (!C_real_select_target(instance->damage_target_player,
                                2,
                                2,
                                TARGET_ZONE_IN_PLAY,
                                TYPE_LAND,
                                TYPE_NONE,
                                0,
                                get_protections_from(0, 0),
                                COLOR_TEST_0,
                                COLOR_TEST_0,
                                -1,
                                ~SUB_WALL,
                                -1,
                                -1,
                                0,
                                0,
                                0,
                                text_lines[1],
                                0,
                                &new_target))
      {
        new_target.player = -1;
        new_target.card = -1;
      }
    }
    else if (FUN_0048194e(instance->damage_target_player, 1 - instance->damage_target_player, (int *)&new_target))
    {
      do_dialog(0, 0, 0, new_target.player, new_target.card, text_lines[2], 0);
    }

    kill_card(affected_card_controller, affected_card, KILL_DESTROY);

    if (new_target.player != -1)
    {
      instance->damage_target_player = (char)new_target.player;
      instance->damage_target_card = new_target.card;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0043aee2
// FUNCTION: SHANDALAR 0x004fd888
int card_lance(int player, int card, event_t event)
{
  if ((event == EVENT_CAST_SPELL) && (card == affected_card) && (player == affected_card_controller))
  {
    load_text("promptsX1.txt", "LANCE");
  }

  return FUN_0052d7a5(player, card, event, 0x100);
}

// FUNCTION: MAGIC 0x0043af43
// FUNCTION: SHANDALAR 0x004fd8eb
int card_lich(int player, int card, event_t event)
{
  card_instance_t *instance;
  card_instance_t *damage_card;
  int current_card;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (is_in_play(player, card) && life[player] != -99)
  {
    life[player] = 0;
    for (current_card = 0; current_card < active_cards_count[player]; ++current_card)
    {
      if (is_in_play(player, current_card) && (global_cards_data[PLAYER_CARD_INSTANCE(player, current_card).internal_card_id].type & (TYPE_ARTIFACT | TYPE_ENCHANTMENT | TYPE_CREATURE | TYPE_LAND)) != 0)
      {
        ++life[player];
      }
    }
  }

  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }
  else
  {
    if (event == EVENT_CAST_SPELL && card == affected_card && player == affected_card_controller && active_player == player)
    {
      ai_modifier -= life[player] - 5;
    }

    if (event == EVENT_RESOLVE_SPELL)
    {
      unk_008b44d0[player] = 1;
    }

    if (event == EVENT_DEAL_DAMAGE && PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).internal_card_id == unk_009266a4 && PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).damage_target_player == player && PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).damage_target_card == -1 && PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).info_slot != 0)
    {
      instance->targets[instance->info_slot].player = affected_card_controller;
      instance->targets[instance->info_slot].card = affected_card;
      ++instance->info_slot;
    }

    if (trigger_condition == TRIGGER_DEAL_DAMAGE && card == affected_card && player == affected_card_controller && instance->info_slot != 0 && player == current_turn)
    {
      if (event == EVENT_TRIGGER)
      {
        event_result |= RESOLVE_TRIGGER_MANDATORY;
      }

      if (event == EVENT_RESOLVE_TRIGGER)
      {
        do
        {
          damage_card = &PLAYER_CARD_INSTANCE(instance->targets[instance->info_slot - 1].player,
                                              instance->targets[instance->info_slot - 1].card);
          FUN_0043b4f3(player, damage_card->info_slot);
          --instance->info_slot;
          if (instance->info_slot == 0)
          {
            break;
          }
        } while (life[player] != -99);
      }
    }

    if (event == EVENT_GRAVEYARD_FROM_PLAY && card == affected_card && player == affected_card_controller)
    {
      if (is_in_play(player, card))
      {
        if (g_duel_ai_mode_state == 1)
        {
          life[player] = -99;
        }
        else
        {
          FUN_004b4110(player);
        }
      }
    }

    return 0;
  }
}

// FUNCTION: MAGIC 0x0043b4f3
int FUN_0043b4f3(int player, int amount)
{
  card_instance_t *instance;
  int max_targets;
  int sacrifices_to_make;
  int sacrifices_made;
  char prompt[300];
  target_t target;

  real_target_available(&max_targets,
                        TARGET_SCAN_DIRECT,
                        player,
                        (unsigned char)player,
                        (unsigned char)player,
                        TARGET_ZONE_IN_PLAY,
                        TYPE_ARTIFACT | TYPE_INTERRUPT | TYPE_INSTANT | TYPE_SORCERY | TYPE_ENCHANTMENT | TYPE_CREATURE | TYPE_LAND,
                        TARGET_TYPE_TOKEN,
                        0,
                        0,
                        0,
                        0,
                        -1,
                        -1,
                        0xffffffff,
                        0xffffffff,
                        0,
                        0,
                        0);

  sacrifices_to_make = max_targets;
  if (amount <= max_targets)
  {
    sacrifices_to_make = amount;
  }

  sacrifices_made = 0;
  while (sacrifices_made < sacrifices_to_make && life[player] != -99)
  {
    if (g_duel_ai_mode_state == 1)
    {
      target.player = player;
      do
      {
        do
        {
          unk_00939340 = internal_rand(active_cards_count[player]);
          target.card = unk_00939340;
        } while (!is_in_play(target.player, target.card));
        instance = &PLAYER_CARD_INSTANCE(target.player, target.card);
      } while ((global_cards_data[instance->internal_card_id].type & 0x7f) == 0 || (instance->token_status & 0x10) != 0);
      FUN_004e4f11();
    }
    else if (player == active_player && (unk_00926804 & 2) == 0)
    {
      FUN_004e5089();
      target.player = player;
      target.card = unk_00939340;
    }
    else
    {
      load_text("promptsX1.txt", "LICH");
      sprintf(prompt, text_lines[0], sacrifices_made + 1, sacrifices_to_make);
      C_real_select_target(player,
                           player,
                           player,
                           TARGET_ZONE_IN_PLAY,
                           TYPE_ARTIFACT | TYPE_INTERRUPT | TYPE_INSTANT | TYPE_SORCERY | TYPE_ENCHANTMENT | TYPE_CREATURE | TYPE_LAND,
                           TARGET_TYPE_TOKEN,
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
                           prompt,
                           0,
                           &target);
      if (g_duel_ai_mode_state != 1)
      {
        play_sound_effect(WAV_SACRFICE);
      }
    }

    kill_card(target.player, target.card, KILL_SACRIFICE);
    ++sacrifices_made;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0043b725
// FUNCTION: SHANDALAR 0x004fe0c6
int card_oubliette(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x0043c023
// FUNCTION: SHANDALAR 0x004fe9cd
int card_powerleech(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x0043c1a5
// FUNCTION: SHANDALAR 0x004feb50
int card_raging_river(int player, int card, event_t event)
{
  int current_card;
  int defender;
  int done;
  int legacy_card;
  int selected_cards[151];
  card_instance_t *instance;
  char message[600];
  target_t target;

  instance = &PLAYER_CARD_INSTANCE(player, card);
  if (event == EVENT_CAN_CAST)
  {
    return player == unk_008b35ec || (unk_00926804 & 2) != 0;
  }

  if ((event == EVENT_RESOLVE_SPELL) && dispatch_function_to_all_cards_in_play(player, card, FUN_00483190, player) == -1)
  {
    *(int *)((char *)instance + 0x14) |= 0x1000000;
  }
  if ((event == 0x92) && (player == human_player) && ((*(unsigned char *)((char *)instance + 0x17) & 1) != 0))
  {
    defender = 1 - human_player;
    if ((unk_008b35ec == defender) || ((unk_00926804 & 2) != 0))
    {
      for (current_card = 0; current_card < active_cards_count[defender]; ++current_card)
      {
        selected_cards[current_card] = 0;
      }

      done = 0;
      while (done == 0)
      {
        load_text("promptsX1.txt", "RAGING_RIVER");
        if (!C_real_select_target(defender,
                                  defender,
                                  defender,
                                  TARGET_ZONE_IN_PLAY,
                                  TYPE_CREATURE,
                                  TYPE_NONE,
                                  0,
                                  KEYWORD_FLYING,
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
                                  2,
                                  &target))
        {
          done = 1;
        }
        else
        {
          legacy_card = create_legacy_effect(player, card, unk_008b3bd4, target.player, target.card);
          if (legacy_card != -1)
          {
            PLAYER_CARD_INSTANCE(player, legacy_card).info_slot = 2;
          }
          selected_cards[target.card] = 1;
        }
      }

      for (current_card = 0; current_card < active_cards_count[defender]; ++current_card)
      {
        if (is_in_play(defender, current_card) && (global_cards_data[PLAYER_CARD_INSTANCE(defender, current_card).internal_card_id].type & TYPE_CREATURE) != 0 && selected_cards[current_card] == 0 && ((*(unsigned char *)((char *)&PLAYER_CARD_INSTANCE(defender, current_card) + 0x24) & 0x20) == 0))
        {
          legacy_card = create_legacy_effect(player, card, unk_008b3bd4, defender, current_card);
          if (legacy_card != -1)
          {
            PLAYER_CARD_INSTANCE(player, legacy_card).info_slot = 1;
          }
        }
      }
    }
    else
    {
      FUN_0043c7ab(defender, player, card);
    }
  }

  if (trigger_condition == 0xde && card == card_on_stack && player == card_on_stack_controller && ((*(unsigned char *)((char *)instance + 0x17) & 1) != 0) && ((*(unsigned char *)((char *)&PLAYER_CARD_INSTANCE(trigger_cause_controller, trigger_cause) + 0x24) & 0x20) == 0) && current_turn == human_player && player == human_player)
  {
    if (event == 0x7d)
    {
      event_result |= 2;
    }
    if (event == 0x7e)
    {
      legacy_card = create_legacy_effect(player, card, unk_008b3bd4, trigger_cause_controller, trigger_cause);
      load_text("promptsX1.txt", "RAGING_RIVER2");
      sprintf(message, " %s\n %s", "Attack from left bank", "Attack from right bank");
      if (do_dialog(player, player, card, -1, -1, message, 0) == 0)
      {
        PLAYER_CARD_INSTANCE(player, legacy_card).info_slot = 1;
      }
      else
      {
        PLAYER_CARD_INSTANCE(player, legacy_card).info_slot = 2;
      }
    }
  }

  if (event == 0x77 && card == card_on_stack && player == card_on_stack_controller && ((*(unsigned char *)((char *)instance + 0x17) & 1) != 0))
  {
    legacy_card = dispatch_function_to_all_cards_in_play(player, card, FUN_00483242, player);
    if (legacy_card != -1)
    {
      *(int *)((char *)&PLAYER_CARD_INSTANCE(player, legacy_card) + 0x14) |= 0x1000000;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0043c7ab
int FUN_0043c7ab(int who_is_being_divided, int player, int card)
{
  int bank;
  int current_card;
  int legacy_card;
  int power_total[3];

  power_total[0] = 0;
  power_total[1] = 0;
  bank = internal_rand(2);
  for (current_card = 0; current_card < active_cards_count[who_is_being_divided]; ++current_card)
  {
    if (is_in_play(who_is_being_divided, current_card) && (global_cards_data[PLAYER_CARD_INSTANCE(who_is_being_divided, current_card).internal_card_id].type & TYPE_CREATURE) != 0 && ((*(unsigned char *)((char *)&PLAYER_CARD_INSTANCE(who_is_being_divided, current_card) + 0x24) & 0x20) == 0))
    {
      legacy_card = create_legacy_effect(player, card, unk_008b3bd4, who_is_being_divided, current_card);
      if (legacy_card != -1)
      {
        if (power_total[1 - bank] < power_total[bank])
        {
          bank ^= 1;
        }
        power_total[bank] += *(short *)((char *)&PLAYER_CARD_INSTANCE(who_is_being_divided, current_card) + 0xa);
        PLAYER_CARD_INSTANCE(player, legacy_card).info_slot = bank == 0 ? 1 : 2;
      }
    }
  }

  return 0;
}
