#include <string.h>
#include "../game_support.h"
#include "../global_strings.h"

// FUNCTION: MAGIC 0x0053aa50
// FUNCTION: SHANDALAR 0x00488ca0
int card_dummy(int player, int card, event_t event)
{
  return 0;
}

// FUNCTION: MAGIC 0x0053aa62
// FUNCTION: SHANDALAR 0x00488cb2
int card_data_card(int player, int card, event_t event)
{
  return 0;
}

// FUNCTION: MAGIC 0x0053aa74
unsigned int get_protections_from(int player, int card)
{
  struct
  {
    unsigned int type;
    unsigned int illegal_abilities;
  } s;

  s.illegal_abilities = 0;

  if (PLAYER_CARD_INSTANCE(player, card).internal_card_id == -1 ||
      PLAYER_CARD_INSTANCE(player, card).internal_card_id == unk_0091a80c)
  {
    s.type = global_cards_data[PLAYER_CARD_INSTANCE(player, card).original_internal_card_id].type;
  }
  else
  {
    s.type = global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].type;
  }

  if ((s.type & TYPE_SORCERY) != 0)
  {
    s.illegal_abilities |= KEYWORD_PROT_SORCERIES;
  }
  if ((s.type & TYPE_INSTANT) != 0)
  {
    s.illegal_abilities |= KEYWORD_PROT_INSTANTS;
  }
  if ((s.type & TYPE_INTERRUPT) != 0)
  {
    s.illegal_abilities |= KEYWORD_PROT_INTERRUPTS;
  }
  if ((s.type & TYPE_ENCHANTMENT) != 0)
  {
    s.illegal_abilities |= KEYWORD_PROT_ENCHANTMENTS;
  }
  if ((s.type & TYPE_ARTIFACT) != 0)
  {
    s.illegal_abilities |= KEYWORD_PROT_ARTIFACTS;
  }

  return (0x800 << (unsigned char)(single_color_test_bit_to_color_t(
                                       PLAYER_CARD_INSTANCE(player, card).color) -
                                   1)) |
         s.illegal_abilities;
}

// FUNCTION: MAGIC 0x0053ac18
// FUNCTION: SHANDALAR 0x00488e68
int card_prismatic_dragon(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x0053b155
// FUNCTION: SHANDALAR 0x004893a9
int card_rainbow_knights(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x0053c256
// FUNCTION: SHANDALAR 0x0048a4aa
int card_sindbad(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x0053c406
// FUNCTION: SHANDALAR 0x0048a65a
int card_tempest_efreet(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x0053cbde
// FUNCTION: SHANDALAR 0x0048ae32
int card_xenic_poltergeist(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x0053d06d
// FUNCTION: SHANDALAR 0x0048b2c1
int card_uncle_istvan(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x0053d355
// FUNCTION: SHANDALAR 0x0048b5a9
int card_ironclaw_orcs(int player, int card, event_t event)
{
  if (event == EVENT_BLOCK_LEGALITY && affected_card == card && affected_card_controller == player)
  {
    if (C_get_abilities(attacking_card_controller, attacking_card, EVENT_POWER, card) > 1)
    {
      event_result = 1;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0053d3bc
// FUNCTION: SHANDALAR 0x0048b610
int card_amrou_kithkin(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x0053d41f
// FUNCTION: SHANDALAR 0x0048b673
int card_elven_riders(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x0053d4c9
// FUNCTION: SHANDALAR 0x0048b71c
int card_hypnotic_specter(int player, int card, event_t event)
{
  if ((event == EVENT_DEAL_DAMAGE) && (PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).internal_card_id == unk_009266a4))
  {
    if ((PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).damage_target_card == -1) && (PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).damage_source_player == player) && (PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).damage_source_card == card) && (PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).info_slot != 0))
    {
      PLAYER_CARD_INSTANCE(player, card).damage_source_player = (char)affected_card_controller;
      PLAYER_CARD_INSTANCE(player, card).damage_source_card = affected_card;
    }

    return 0;
  }

  if ((((trigger_condition == TRIGGER_DEAL_DAMAGE) && (affected_card == card)) && (affected_card_controller == player) && (PLAYER_CARD_INSTANCE(player, card).damage_source_player != -1)) && (player == current_turn))
  {
    if (event == EVENT_TRIGGER)
    {
      event_result |= 2;
    }
    if (event == EVENT_RESOLVE_TRIGGER)
    {
      discard((int)PLAYER_CARD_INSTANCE(player, card).damage_source_player, 1, 0);
      PLAYER_CARD_INSTANCE(player, card).damage_source_player = -1;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0053d778
// FUNCTION: SHANDALAR 0x0048b9cb
int card_marsh_viper(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x0053da26
// FUNCTION: SHANDALAR 0x0048bc78
int card_pit_scorpion(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x0053dcd4
// FUNCTION: SHANDALAR 0x0048bf26
int card_nafs_asp(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x0053e216
// FUNCTION: SHANDALAR 0x0048c465
int card_giant_tortoise(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x0053e28a
// FUNCTION: SHANDALAR 0x0048c4db
int card_personal_incarnation(int player, int card, event_t event)
{
  card_instance_t *instance;
  card_instance_t *selected;
  target_t target;
  int damage_source_player;
  int max_damage;
  int damage_to_deal;
  int damage_card;
  int replacement_card;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == 0x21)
  {
    if (PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).damage_target_player == player && PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).damage_target_card == card)
    {
      instance->info_slot |= 1;
    }
    return 0;
  }

  if (event == 0x73)
  {
    if ((instance->info_slot & 1) != 0 && (unk_008b4278 & 4) != 0)
    {
      return 99;
    }
    return 0;
  }

  if (event == 0x90)
  {
    FUN_004e4ff3(0);
    return 0;
  }

  if (event == EVENT_ACTIVATE && (instance->info_slot & 1) != 0 && (unk_008b4278 & 4) != 0)
  {
    damage_source_player = (instance->state & 0x1000) != 0;
    do
    {
      if (unk_008a9000 != 1)
      {
        load_text("prompts.txt", "PERSONAL_INCARNATION");
      }

      if (!C_real_select_target(player, 2, 2, TARGET_ZONE_IN_PLAY, TYPE_NONE, TYPE_NONE, 0, 0,
                                COLOR_TEST_0, COLOR_TEST_0, unk_009266a4, ~SUB_WALL,
                                -1, -1, 0, 0, 0, text_lines[0], 1, &target))
      {
        spell_fizzled = 1;
      }
      else
      {
        selected = &PLAYER_CARD_INSTANCE(target.player, target.card);
        if (selected->damage_target_player == player && selected->damage_target_card == card)
        {
          if (unk_008a9000 != 1)
          {
            load_text("prompts.txt", "PERSONAL_INCARNATION");
          }
          if (selected->info_slot + instance->toughness < 6)
          {
            max_damage = 0;
          }
          else
          {
            max_damage = selected->info_slot - (5 - instance->toughness);
          }
          damage_to_deal = choose_a_number(player, text_lines[1], max_damage);
          if (damage_to_deal > selected->info_slot)
          {
            damage_to_deal = selected->info_slot;
          }
          damage_card = damage_creature(damage_source_player, -1, damage_to_deal,
                                        selected->damage_source_player, selected->damage_source_card);
          if (damage_card != -1)
          {
            *(int *)((char *)&PLAYER_CARD_INSTANCE(player, damage_card) + 0x60) =
                *(int *)((char *)selected + 0x60);
            instance->info_slot &= ~1;
            selected->info_slot -= damage_to_deal;
          }
        }
      }
    } while (!spell_fizzled && (PLAYER_CARD_INSTANCE(target.player, target.card).damage_target_player != player || PLAYER_CARD_INSTANCE(target.player, target.card).damage_target_card != card));
  }

  if (event == EVENT_CLEANUP)
  {
    instance->info_slot &= ~1;
    return 0;
  }

  if (event == 0x77 && affected_card == card && affected_card_controller == player && instance->kill_code != 4 && is_in_play(player, card))
  {
    replacement_card = add_card_to_hand(player, unk_00939348);
    if (replacement_card != -1)
    {
      *(int *)((char *)&PLAYER_CARD_INSTANCE(player, replacement_card) + 0x38) = instance->internal_card_id;
      PLAYER_CARD_INSTANCE(player, replacement_card).state |= ((instance->state >> 8) & 0x10) | 2;
      *(int *)((char *)&PLAYER_CARD_INSTANCE(player, replacement_card) + 0x60) = 0xb5;
      TENTATIVE_set_timestamps(player, replacement_card);
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0053ea3b
// FUNCTION: SHANDALAR 0x0048cc8c
int card_ali_from_cairo(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x0053f22d
// FUNCTION: SHANDALAR 0x0048d477
int card_fungusaur(int player, int card, event_t event)
{
  if (event == EVENT_UNKNOWN80)
  {
    if (((((unk_008b4278 & 0x200) != 0) && (PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).damage_target_player == player)) && (PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).damage_target_card == card)) && (0 < PLAYER_CARD_INSTANCE(player, card).damage_on_card))
    {
      PLAYER_CARD_INSTANCE(player, card).info_slot = 1;
    }

    return 0;
  }

  if ((((trigger_condition == TRIGGER_DEAL_DAMAGE) && (card == affected_card)) && (player == affected_card_controller)) && (0 < PLAYER_CARD_INSTANCE(player, card).damage_on_card))
  {
    PLAYER_CARD_INSTANCE(player, card).info_slot = 1;
  }
  if ((((trigger_condition == TRIGGER_EOT) || (event == EVENT_SHOULD_AI_PLAY)) && ((card == affected_card) && ((player == affected_card_controller) && (PLAYER_CARD_INSTANCE(player, card).info_slot != 0)))) && (player == current_turn))
  {
    if (event == EVENT_TRIGGER)
    {
      event_result |= 2;
    }
    if ((event == EVENT_RESOLVE_TRIGGER) || (event == EVENT_SHOULD_AI_PLAY))
    {
      PLAYER_CARD_INSTANCE(player, card).counter_power++;

      PLAYER_CARD_INSTANCE(player, card).counter_toughness++;

      FUN_005513d7(player, card, 1);
      PLAYER_CARD_INSTANCE(player, card).info_slot = 0;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0053f521
// FUNCTION: SHANDALAR 0x0048d76b
int card_carrion_ants(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x0053fe1c
// FUNCTION: SHANDALAR 0x0048e066
int card_shivan_dragon(int player, int card, event_t event)
{
  int legacy;

  if (event == EVENT_UNTAP_PHASE)
  {
    ++unk_00939520[player][COLOR_RED];
    return 0;
  }
  else if (((event == EVENT_CAST_SPELL) && (affected_card == card)) && (affected_card_controller == player))
  {
    PLAYER_CARD_INSTANCE(player, card).eot_toughness = 0;
    PLAYER_CARD_INSTANCE(player, card).info_slot = PLAYER_CARD_INSTANCE(player, card).eot_toughness;
    return 0;
  }
  else if (event == EVENT_CAN_ACTIVATE)
  {
    return has_mana(player, COLOR_RED, 1);
  }
  else if (event == EVENT_GET_SELECTED_CARD)
  {
    FUN_004e503e(0);
    return 0;
  }
  else if (event == EVENT_ACTIVATE)
  {
    if (has_mana(player, COLOR_RED, 1) != 0)
    {
      if (human_player == player)
      {
        charge_mana(player, 4, -1);
        if (x_value < 1)
        {
          spell_fizzled = 1;
        }
        else
        {
          PLAYER_CARD_INSTANCE(player, card).eot_toughness = x_value;
        }
      }
      else
      {
        charge_mana(player, 4, 1);
        PLAYER_CARD_INSTANCE(player, card).eot_toughness = 1;
      }

      if (spell_fizzled == 1)
      {
        PLAYER_CARD_INSTANCE(player, card).eot_toughness = 0;
      }
      else
      {
        PLAYER_CARD_INSTANCE(player, card).targets[0].player = player;
        PLAYER_CARD_INSTANCE(player, card).targets[0].card = card;
        PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
        if (PLAYER_CARD_INSTANCE(player, card).info_slot == 0)
        {
          *(unsigned int *)&PLAYER_CARD_INSTANCE(player, card).info_slot |= 0x80000;
        }
      }
    }

    return 0;
  }
  else if (event == EVENT_RESOLVE_ACTIVATION)
  {
    if (PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                             PLAYER_CARD_INSTANCE(player, card).parent_card)
            .internal_card_id == -1)
    {
      spell_fizzled = 1;
    }
    else
    {
      *(int *)&PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                                    PLAYER_CARD_INSTANCE(player, card).parent_card)
           .info_slot += PLAYER_CARD_INSTANCE(player, card).eot_toughness & 0xff;
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                           PLAYER_CARD_INSTANCE(player, card).parent_card)
          .number_of_targets = 0;
      if ((PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                                PLAYER_CARD_INSTANCE(player, card).parent_card)
               .info_slot &
           0x80000) != 0)
      {
        *(unsigned int *)&PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                                               PLAYER_CARD_INSTANCE(player, card).parent_card)
             .info_slot &= 0xfff7ffff;
        legacy = create_legacy_effect(card_on_stack_controller,
                                      card_on_stack,
                                      LEGACY_EFFECT_PUMP,
                                      card_on_stack_controller,
                                      card_on_stack);
        if (legacy != -1)
        {
          *(unsigned int *)&PLAYER_CARD_INSTANCE(player, legacy).info_slot |= 0x80000;
        }
      }
    }

    return 0;
  }
  else if (event == EVENT_POW_BOOST)
  {
    return has_mana(player, COLOR_RED, 1);
  }
  else if ((event == EVENT_CAN_WASTE_MANA) && raw_mana_available[player][COLOR_RED] != 0)
  {
    event_result |= 1;
    return 0;
  }
  else
  {
    if (event == EVENT_SHOULD_AI_PLAY)
    {
      if (active_player == player)
      {
        ai_modifier += (basiclandtypes_controlled[player][COLOR_RED] * 3 + 3) * 4;
      }
      else
      {
        ai_modifier += (basiclandtypes_controlled[player][COLOR_RED] * 3 + 3) * -4;
      }
    }

    if ((event == EVENT_CLEANUP) || (event == EVENT_SHOULD_AI_PLAY))
    {
      PLAYER_CARD_INSTANCE(player, card).eot_toughness = 0;
      PLAYER_CARD_INSTANCE(player, card).info_slot = PLAYER_CARD_INSTANCE(player, card).eot_toughness;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0054060c
// FUNCTION: SHANDALAR 0x0048e853
int card_dragon_whelp(int player, int card, event_t event)
{
  int *info_slot_ptr;
  int result;
  unsigned int *state_ptr;
  unsigned int saved_max_x_value;

  if (event == EVENT_UNTAP_PHASE)
  {
    ++unk_00939530[player][0];
    result = 0;
  }
  else if (((event == EVENT_CAST_SPELL) && (affected_card == card)) && (affected_card_controller == player))
  {
    PLAYER_CARD_INSTANCE(player, card).eot_toughness = 0;
    PLAYER_CARD_INSTANCE(player, card).info_slot = PLAYER_CARD_INSTANCE(player, card).eot_toughness;
    result = 0;
  }
  else if (event == EVENT_CAN_ACTIVATE)
  {
    result = has_mana(player, COLOR_RED, 1);
  }
  else if (event == EVENT_GET_SELECTED_CARD)
  {
    FUN_004e503e(0);
    result = 0;
  }
  else if (event == EVENT_ACTIVATE)
  {
    result = has_mana(player, COLOR_RED, 1);
    saved_max_x_value = max_x_value;
    if (result != 0)
    {
      x_value = 0;
      if (player == human_player)
      {
        if ((((player == active_player) && ((unk_00926804 & 2) == 0)) || ((PLAYER_CARD_INSTANCE(player, card).eot_toughness & 0xff0000) == 0x30000)) || unk_00715fb0 != 1)
        {
          max_x_value = -1;
        }
        else
        {
          max_x_value = 3 - ((PLAYER_CARD_INSTANCE(player, card).eot_toughness & 0xff0000) >> 16);
        }
        charge_mana(player, COLOR_RED, -1);
        max_x_value = saved_max_x_value;
        if (x_value < 1)
        {
          spell_fizzled = 1;
        }
      }
      else
      {
        charge_mana(player, COLOR_RED, 1);
        x_value = 1;
      }

      state_ptr = (unsigned int *)&PLAYER_CARD_INSTANCE(player, card).eot_toughness;
      *state_ptr &= 0xff0000;
      if (spell_fizzled == 1)
      {
        PLAYER_CARD_INSTANCE(player, card).eot_toughness = 0;
      }
      else
      {
        PLAYER_CARD_INSTANCE(player, card).targets[0].player = player;
        PLAYER_CARD_INSTANCE(player, card).targets[0].card = card;
        PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
        PLAYER_CARD_INSTANCE(player, card).eot_toughness += x_value * 0x10001;
        if (PLAYER_CARD_INSTANCE(player, card).info_slot == 0)
        {
          PLAYER_CARD_INSTANCE(player, card).info_slot |= 0x80000;
        }
      }
    }
    result = 0;
  }
  else if (event == EVENT_RESOLVE_ACTIVATION)
  {
    if (PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                             PLAYER_CARD_INSTANCE(player, card).parent_card)
            .internal_card_id == -1)
    {
      spell_fizzled = 1;
    }
    else
    {
      info_slot_ptr =
          &PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                                PLAYER_CARD_INSTANCE(player, card).parent_card)
               .info_slot;
      *info_slot_ptr += PLAYER_CARD_INSTANCE(player, card).eot_toughness & 0xff;
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                           PLAYER_CARD_INSTANCE(player, card).parent_card)
          .number_of_targets = 0;
      if ((PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                                PLAYER_CARD_INSTANCE(player, card).parent_card)
               .info_slot &
           0x80000) != 0)
      {
        PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                             PLAYER_CARD_INSTANCE(player, card).parent_card)
            .info_slot &= 0xfff7ffff;
        result = create_legacy_effect(card_on_stack_controller,
                                      card_on_stack,
                                      LEGACY_EFFECT_PUMP,
                                      card_on_stack_controller,
                                      card_on_stack);
        if (result != -1)
        {
          state_ptr = (unsigned int *)&PLAYER_CARD_INSTANCE(player, result).info_slot;
          *state_ptr |= 0x80000;
        }
      }
    }
    result = 0;
  }
  else if (event == EVENT_POW_BOOST)
  {
    result = has_mana(player, COLOR_RED, 1);
    result = FUN_004c0a36(result, 0, 3);
    result -= PLAYER_CARD_INSTANCE(player, card).info_slot;
  }
  else
  {
    if ((((trigger_condition == TRIGGER_EOT) || (event == EVENT_SHOULD_AI_PLAY)) && (affected_card == card) && (affected_card_controller == player) && ((char)PLAYER_CARD_INSTANCE(player, card).info_slot != 0)) && player == current_turn)
    {
      if (PLAYER_CARD_INSTANCE(player, card).info_slot < 4)
      {
        PLAYER_CARD_INSTANCE(player, card).eot_toughness = 0;
        PLAYER_CARD_INSTANCE(player, card).info_slot = PLAYER_CARD_INSTANCE(player, card).eot_toughness;
      }
      else
      {
        if (event == EVENT_TRIGGER)
        {
          event_result |= 2;
        }
        if (event == EVENT_RESOLVE_TRIGGER || event == EVENT_SHOULD_AI_PLAY)
        {
          kill_card(player, card, KILL_DESTROY);
        }
      }
    }
    if (event == EVENT_SHOULD_AI_PLAY)
    {
      if (player == active_player)
      {
        ai_modifier += basiclandtypes_controlled[player][COLOR_RED] * 0xc;
      }
      else
      {
        ai_modifier += basiclandtypes_controlled[player][COLOR_RED] * -0xc;
      }
    }
    result = 0;
  }

  return result;
}

// FUNCTION: MAGIC 0x00540fc8
// FUNCTION: SHANDALAR 0x0048f20f
int card_goblin_balloon_brigade(int player, int card, event_t event)
{
  int legacy_card;

  if (((event == EVENT_CAST_SPELL) && (affected_card == card)) && (affected_card_controller == player))
  {
    PLAYER_CARD_INSTANCE(player, card).info_slot = 0x20;
    return 0;
  }

  if (event == EVENT_UNTAP_PHASE)
  {
    ++unk_00939530[player][0];
    return 0;
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    if ((PLAYER_CARD_INSTANCE(player, card).info_slot == 0) || (has_mana(player, COLOR_RED, 1) == 0))
    {
      return 0;
    }
    return 1;
  }

  if (event == EVENT_ACTIVATE)
  {
    if (has_mana(player, COLOR_RED, 1) != 0)
    {
      charge_mana(player, COLOR_RED, 1);
      if (spell_fizzled != 1)
      {
        PLAYER_CARD_INSTANCE(player, card).targets[0].player = player;
        PLAYER_CARD_INSTANCE(player, card).targets[0].card = card;
        PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
        if ((active_player == player) && ((unk_00926804 & 2) == 0))
        {
          PLAYER_CARD_INSTANCE(player, card).info_slot = 0;
        }
      }
    }
    return 0;
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    if (PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                             PLAYER_CARD_INSTANCE(player, card).parent_card)
            .internal_card_id == -1)
    {
      spell_fizzled = 1;
    }
    else
    {
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                           PLAYER_CARD_INSTANCE(player, card).parent_card)
          .number_of_targets = 0;
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                           PLAYER_CARD_INSTANCE(player, card).parent_card)
          .info_slot = 0x20;
      legacy_card = create_legacy_effect(card_on_stack_controller,
                                         card_on_stack,
                                         unk_00896534,
                                         card_on_stack_controller,
                                         card_on_stack);
      if (legacy_card != -1)
      {
        PLAYER_CARD_INSTANCE(player, legacy_card).info_slot = 0x20;
      }
      else
      {
        spell_fizzled = 1;
      }
    }
    return 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x00541405
// FUNCTION: SHANDALAR 0x0048f64a
int card_whirling_dervish(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x00541740
// FUNCTION: SHANDALAR 0x0048f983
int card_nightmare(int player, int card, event_t event)
{
  unsigned int *state_ptr;
  int legacy_effect_card;

  if (event == EVENT_RESOLVE_SPELL)
  {
    legacy_effect_card = create_legacy_effect(player, card, unk_0093d848, player, card);
    if (legacy_effect_card != -1)
    {
      PLAYER_CARD_INSTANCE(player, legacy_effect_card).info_slot = 1;
      PLAYER_CARD_INSTANCE(player, legacy_effect_card).eot_toughness = 0x10d;
      PLAYER_CARD_INSTANCE(player, legacy_effect_card).token_status = 0x10020;
      state_ptr = (unsigned int *)&PLAYER_CARD_INSTANCE(player, legacy_effect_card).token_status;
      *state_ptr |= 0x2000000;
      PLAYER_CARD_INSTANCE(player, card).damage_source_player = (char)player;
      PLAYER_CARD_INSTANCE(player, card).damage_source_card = legacy_effect_card;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x005418b4
// FUNCTION: SHANDALAR 0x0048faf7
int card_angry_mob(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x00541b88
// FUNCTION: SHANDALAR 0x0048fdcb
int card_gaea_s_liege(int player, int card, event_t event)
{
  card_instance_t *instance;
  int effect_card;
  target_t selected_target;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_RESOLVE_SPELL)
  {
    effect_card = create_legacy_effect(player, card, unk_0093d848, player, card);
    if (effect_card != -1)
    {
      PLAYER_CARD_INSTANCE(player, effect_card).info_slot = 3;
      PLAYER_CARD_INSTANCE(player, effect_card).eot_toughness = 0x10d;
      PLAYER_CARD_INSTANCE(player, effect_card).token_status = 0x10020;
      PLAYER_CARD_INSTANCE(player, effect_card).token_status |= 0x2000000;
      instance->damage_source_player = (char)player;
      instance->damage_source_card = effect_card;
    }
  }
  else if (((event == EVENT_POWER) || (event == EVENT_TOUGHNESS)) && card == affected_card && player == affected_card_controller)
  {
    if ((instance->state & STATE_ATTACKING) == 0)
    {
      PLAYER_CARD_INSTANCE((int)instance->damage_source_player, instance->damage_source_card)
          .eot_toughness |= 1;
      PLAYER_CARD_INSTANCE((int)instance->damage_source_player, instance->damage_source_card)
          .eot_toughness &= 0xfffffffd;
    }
    else
    {
      PLAYER_CARD_INSTANCE((int)instance->damage_source_player, instance->damage_source_card)
          .eot_toughness |= 2;
      PLAYER_CARD_INSTANCE((int)instance->damage_source_player, instance->damage_source_card)
          .eot_toughness &= 0xfffffffe;
    }
  }
  else if (event == EVENT_CAN_ACTIVATE)
  {
    if ((instance->state & 0x20010) == 0 && real_target_available((int *)0,
                                                                  TARGET_SCAN_DIRECT,
                                                                  player,
                                                                  2,
                                                                  2,
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
                                                                  -1,
                                                                  0,
                                                                  0,
                                                                  0) != 0)
    {
      return 1;
    }
  }
  else if (event == EVENT_GET_SELECTED_CARD)
  {
    FUN_004e4ff3(0);
  }
  else if (event == EVENT_ACTIVATE)
  {
    if (unk_008a9000 != 1)
    {
      load_text("prompts.txt", "GAEAS_LIEGE");
    }

    if (!C_real_select_target(player,
                              2,
                              1 - player,
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
                              &selected_target))
    {
      spell_fizzled = 1;
    }
    else
    {
      instance->targets[0].player = selected_target.player;
      instance->targets[0].card = selected_target.card;
      instance->number_of_targets = 1;
      instance->state |= 0x10;
    }
  }
  else if (event == EVENT_RESOLVE_ACTIVATION)
  {
    selected_target.player = instance->targets[0].player;
    selected_target.card = instance->targets[0].card;

    if (!C_real_validate_target(selected_target.player,
                                selected_target.card,
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
      spell_fizzled = 1;
    }
    else
    {
      PLAYER_CARD_INSTANCE(instance->parent_controller, instance->parent_card).info_slot = 3;
      effect_card = create_legacy_effect(card_on_stack_controller,
                                         card_on_stack,
                                         unk_008b49c4,
                                         selected_target.player,
                                         selected_target.card);
      if (effect_card != -1)
      {
        PLAYER_CARD_INSTANCE(card_on_stack_controller, effect_card).token_status |= 0x11020;
      }
    }

    PLAYER_CARD_INSTANCE(instance->parent_controller, instance->parent_card).number_of_targets = 0;
  }
  else if ((event == EVENT_CLEANUP || event == EVENT_SHOULD_AI_PLAY) && card == affected_card && player == affected_card_controller && (instance->info_slot & 0x4000) != 0)
  {
    instance->info_slot &= 0xffffbfff;
    C_get_abilities(player, card, EVENT_POWER, -1);
    C_get_abilities(player, card, EVENT_TOUGHNESS, -1);
  }

  return 0;
}

// FUNCTION: MAGIC 0x005424a4
// FUNCTION: SHANDALAR 0x004906e7
int card_plague_rats(int player, int card, event_t event)
{
  int legacy_card;

  if (event == EVENT_RESOLVE_SPELL)
  {
    legacy_card = create_legacy_effect(player, card, unk_0093d848, player, card);
    if (legacy_card != -1)
    {
      PLAYER_CARD_INSTANCE(player, legacy_card).info_slot = PLAYER_CARD_INSTANCE(player, card).internal_card_id;
      PLAYER_CARD_INSTANCE(player, legacy_card).eot_toughness = 0x20f;
      PLAYER_CARD_INSTANCE(player, legacy_card).token_status = STATUS_INVISIBLE_FX | STATUS_PERMANENT;
      PLAYER_CARD_INSTANCE(player, card).damage_source_player = (unsigned char)player;
      PLAYER_CARD_INSTANCE(player, card).damage_source_card = legacy_card;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x00542610
// FUNCTION: SHANDALAR 0x00490853
int card_keldon_warlord(int player, int card, event_t event)
{
  int legacy_card;

  if (event == EVENT_RESOLVE_SPELL)
  {
    legacy_card = create_legacy_effect(player, card, unk_0093d848, player, card);
    if (legacy_card != -1)
    {
      PLAYER_CARD_INSTANCE(player, legacy_card).eot_toughness = 0x80d;
      PLAYER_CARD_INSTANCE(player, legacy_card).token_status = 0x10020;
      PLAYER_CARD_INSTANCE(player, card).damage_source_player = (char)player;
      PLAYER_CARD_INSTANCE(player, card).damage_source_card = legacy_card;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x00542720
// FUNCTION: SHANDALAR 0x00490963
int card_ghost_ship(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x0054276d
int FUN_0054276d(int player, int card, event_t event, unsigned int color, int amount)
{
  int can_activate;

  if (event == EVENT_CAN_ACTIVATE && (unk_008b4278 & 0x200) != 0 && PLAYER_CARD_INSTANCE(player, card).info_slot == 0)
  {
    can_activate = 1;
    if (PLAYER_CARD_INSTANCE(player, card).kill_code != 2)
    {
      can_activate = 0;
    }
    if (can_activate != 0 && (PLAYER_CARD_INSTANCE(player, card).state & 0x800002) != 2)
    {
      can_activate = 0;
    }
    if (can_activate != 0 && (PLAYER_CARD_INSTANCE(player, card).regen_status & 0x200) == 0)
    {
      can_activate = 0;
    }
    if (can_activate != 0 && has_mana(player, color, amount) == 0)
    {
      can_activate = 0;
    }
    if (can_activate != 0)
    {
      return 99;
    }
    return 0;
  }
  else if (event == 0x90)
  {
    FUN_004e4ff3(0);
    return 0;
  }
  else if (event == EVENT_ACTIVATE && (unk_008b4278 & 0x200) != 0)
  {
    charge_mana(player, color, amount);
    if (spell_fizzled != 1)
    {
      unk_007a7c1c = 1;
      PLAYER_CARD_INSTANCE(player, card).info_slot += 1;
    }

    return 0;
  }
  else if (event == EVENT_RESOLVE_ACTIVATION && (unk_008b4278 & 0x200) != 0)
  {
    PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller, PLAYER_CARD_INSTANCE(player, card).parent_card).info_slot = 0;
    FUN_00542a2a(card_on_stack_controller, card_on_stack);
    return 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x00542a2a
void FUN_00542a2a(int player, int card)
{
  struct
  {
    int found_linked_card; /* [ebp-0xc] */
    int test_player;       /* [ebp-0x8] */
    int test_card;         /* [ebp-0x4] */
  } s;

  s.test_player = 0;
  s.found_linked_card = 0;

  while (s.test_player < 2 && s.found_linked_card == 0)
  {
    s.test_card = 0;
    while (s.test_card < active_cards_count[s.test_player] && s.found_linked_card == 0)
    {
      if (is_in_play(s.test_player, s.test_card) != 0 &&
          PLAYER_CARD_INSTANCE(s.test_player, s.test_card).damage_target_player == player &&
          PLAYER_CARD_INSTANCE(s.test_player, s.test_card).damage_target_card == card &&
          ((PLAYER_CARD_INSTANCE(s.test_player, s.test_card).internal_card_id == unk_008b49c4 &&
            (PLAYER_CARD_INSTANCE(s.test_player, s.test_card).token_status & 0x00800000) != 0) ||
           PLAYER_CARD_INSTANCE(s.test_player, s.test_card).internal_card_id == unk_008b3d10))
      {
        s.found_linked_card = 1;
      }

      ++s.test_card;
    }

    ++s.test_player;
  }

  if (s.found_linked_card == 0)
  {
    PLAYER_CARD_INSTANCE(player, card).kill_code = 0;
    PLAYER_CARD_INSTANCE(player, card).unknown0x14 = 0;
    PLAYER_CARD_INSTANCE(player, card).damage_on_card = 0;
    FUN_004f7783(player, card);
    if (unk_008a9000 != 1)
    {
      play_sound_effect(WAV_REGEN);
    }
    PLAYER_CARD_INSTANCE(player, card).token_status &= ~0x80u;
    PLAYER_CARD_INSTANCE(player, card).blocking = 0xff;
    PLAYER_CARD_INSTANCE(player, card).state &= ~0x0c;
  }
}

// FUNCTION: MAGIC 0x00542d29
// FUNCTION: SHANDALAR 0x00490f6c
int card_uthden_troll(int player, int card, event_t event)
{
  unsigned int color;

  color = single_color_test_bit_to_color_t(
      *(char *)&global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].color);
  if (event == EVENT_UNTAP_PHASE)
  {
    ((int *)((char *)unk_00939520 + player * 0x20))[color] += 2;
    return 0;
  }

  if (event == EVENT_CAN_ACTIVATE || event == EVENT_ACTIVATE || event == EVENT_RESOLVE_ACTIVATION)
  {
    return FUN_0054276d(player, card, event, color, 1);
  }

  return 0;
}

// FUNCTION: MAGIC 0x00542de2
// FUNCTION: SHANDALAR 0x00491025
int card_vampire_bats(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x00543640
// FUNCTION: SHANDALAR 0x00491883
int card_frozen_shade(int player, int card, event_t event)
{
  union
  {
    int legacy_effect_card;
    card_instance_t *instance;
  } u;

  if (((event == EVENT_CAST_SPELL) && (affected_card == card)) && (affected_card_controller == player))
  {
    u.instance = &PLAYER_CARD_INSTANCE(player, card);
    u.instance->eot_toughness = 0;
    u.instance->info_slot = u.instance->eot_toughness;
    return 0;
  }

  if (event == EVENT_UNTAP_PHASE)
  {
    ++unk_00939520[player][COLOR_BLACK];
    return 0;
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    return has_mana(player, COLOR_BLACK, 1);
  }

  if (event == EVENT_GET_SELECTED_CARD)
  {
    FUN_004e503e(0);
    return 0;
  }

  if (event == EVENT_ACTIVATE)
  {
    if (has_mana(player, COLOR_BLACK, 1) != 0)
    {
      if (player == human_player)
      {
        charge_mana(player, COLOR_BLACK, -1);
        if (x_value < 1)
        {
          spell_fizzled = 1;
        }
        else
        {
          PLAYER_CARD_INSTANCE(player, card).eot_toughness = x_value;
        }
      }
      else
      {
        charge_mana(player, COLOR_BLACK, 1);
        PLAYER_CARD_INSTANCE(player, card).eot_toughness = 1;
      }

      if (spell_fizzled == 1)
      {
        PLAYER_CARD_INSTANCE(player, card).eot_toughness = 0;
      }
      else
      {
        PLAYER_CARD_INSTANCE(player, card).targets[0].player = player;
        PLAYER_CARD_INSTANCE(player, card).targets[0].card = card;
        PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
        if (PLAYER_CARD_INSTANCE(player, card).info_slot == 0)
        {
          *(unsigned int *)&PLAYER_CARD_INSTANCE(player, card).info_slot |= 0x80000;
        }
      }
    }
    return 0;
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    if (PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                             PLAYER_CARD_INSTANCE(player, card).parent_card)
            .internal_card_id == -1)
    {
      spell_fizzled = 1;
    }
    else
    {
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                           PLAYER_CARD_INSTANCE(player, card).parent_card)
          .info_slot += PLAYER_CARD_INSTANCE(player, card).eot_toughness & 0xff;
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                           PLAYER_CARD_INSTANCE(player, card).parent_card)
          .info_slot += (PLAYER_CARD_INSTANCE(player, card).eot_toughness & 0xff) * 0x100;
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                           PLAYER_CARD_INSTANCE(player, card).parent_card)
          .number_of_targets = 0;
      if ((PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                                PLAYER_CARD_INSTANCE(player, card).parent_card)
               .info_slot &
           0x80000) != 0)
      {
        *(unsigned int *)&PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                                               PLAYER_CARD_INSTANCE(player, card).parent_card)
             .info_slot &= 0xfff7ffff;
        u.legacy_effect_card = create_legacy_effect(card_on_stack_controller,
                                                    card_on_stack,
                                                    LEGACY_EFFECT_PUMP,
                                                    card_on_stack_controller,
                                                    card_on_stack);
        if (u.legacy_effect_card != -1)
        {
          PLAYER_CARD_INSTANCE(player, u.legacy_effect_card).counter_power = 1;
          PLAYER_CARD_INSTANCE(player, u.legacy_effect_card).counter_toughness = 1;
          *(unsigned int *)&PLAYER_CARD_INSTANCE(player, u.legacy_effect_card).info_slot |= 0x80000;
        }
      }
    }
    return 0;
  }

  if (event == EVENT_POW_BOOST)
  {
    return has_mana(player, COLOR_BLACK, 1);
  }

  if (event == EVENT_TOU_BOOST)
  {
    return has_mana(player, COLOR_BLACK, 1);
  }

  if ((event == EVENT_CAN_WASTE_MANA) && (raw_mana_available[player][COLOR_BLACK] != 0))
  {
    event_result |= 1;
    return 0;
  }

  if ((event == EVENT_CLEANUP) || (event == EVENT_SHOULD_AI_PLAY))
  {
    u.instance = &PLAYER_CARD_INSTANCE(player, card);
    u.instance->eot_toughness = 0;
    u.instance->info_slot = u.instance->eot_toughness;
  }

  return 0;
}

// FUNCTION: MAGIC 0x00543f11
// FUNCTION: SHANDALAR 0x00492155
int card_killer_bees(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x005447da
// FUNCTION: SHANDALAR 0x00492a1e
int card_wall_of_water(int player, int card, event_t event)
{
  int legacy_card;

  if (event == EVENT_UNTAP_PHASE)
  {
    ++unk_00939520[player][COLOR_BLUE];
    return 0;
  }
  else if (event == EVENT_CAST_SPELL)
  {
    if (affected_card == card)
    {
      if (affected_card_controller == player)
      {
        PLAYER_CARD_INSTANCE(player, card).eot_toughness = 0;
        PLAYER_CARD_INSTANCE(player, card).info_slot = PLAYER_CARD_INSTANCE(player, card).eot_toughness;
        return 0;
      }
    }
  }
  else if (event == EVENT_CAN_ACTIVATE)
  {
    return has_mana(player, COLOR_BLUE, 1);
  }
  else if (event == EVENT_GET_SELECTED_CARD)
  {
    FUN_004e503e(0);
    return 0;
  }
  else if (event == EVENT_ACTIVATE)
  {
    if (has_mana(player, COLOR_BLUE, 1) != 0)
    {
      if (player == human_player)
      {
        charge_mana(player, COLOR_BLUE, -1);
        if (x_value < 1)
        {
          spell_fizzled = 1;
        }
        else
        {
          PLAYER_CARD_INSTANCE(player, card).eot_toughness = x_value;
        }
      }
      else
      {
        charge_mana(player, COLOR_BLUE, 1);
        PLAYER_CARD_INSTANCE(player, card).eot_toughness = 1;
      }

      if (spell_fizzled == 1)
      {
        PLAYER_CARD_INSTANCE(player, card).eot_toughness = 0;
      }
      else
      {
        PLAYER_CARD_INSTANCE(player, card).targets[0].player = player;
        PLAYER_CARD_INSTANCE(player, card).targets[0].card = card;
        PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
        if (PLAYER_CARD_INSTANCE(player, card).info_slot == 0)
        {
          *(unsigned int *)&PLAYER_CARD_INSTANCE(player, card).info_slot |= 0x80000;
        }
      }
    }
    return 0;
  }
  else if (event == EVENT_RESOLVE_ACTIVATION)
  {
    if (PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                             PLAYER_CARD_INSTANCE(player, card).parent_card)
            .internal_card_id == -1)
    {
      spell_fizzled = 1;
    }
    else
    {
      *(int *)&PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                                    PLAYER_CARD_INSTANCE(player, card).parent_card)
           .info_slot += PLAYER_CARD_INSTANCE(player, card).eot_toughness & 0xff;
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                           PLAYER_CARD_INSTANCE(player, card).parent_card)
          .number_of_targets = 0;
      if ((PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                                 PLAYER_CARD_INSTANCE(player, card).parent_card)
                .info_slot &
            0x80000) != 0)
      {
        *(unsigned int *)&PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                                               PLAYER_CARD_INSTANCE(player, card).parent_card)
             .info_slot &= 0xfff7ffff;
        legacy_card = create_legacy_effect(card_on_stack_controller,
                                           card_on_stack,
                                           LEGACY_EFFECT_PUMP,
                                           card_on_stack_controller,
                                           card_on_stack);
        if (legacy_card != -1)
        {
          PLAYER_CARD_INSTANCE(player, legacy_card).counter_power = 1;
          *(unsigned int *)&PLAYER_CARD_INSTANCE(player, legacy_card).info_slot |= 0x80000;
        }
      }
    }
    return 0;
  }
  else if (event == EVENT_POW_BOOST)
  {
    return has_mana(player, COLOR_BLUE, 1);
  }
  else if ((event == EVENT_CAN_WASTE_MANA) && raw_mana_available[player][COLOR_BLUE] != 0)
  {
    event_result |= 1;
    return 0;
  }
  else
  {
    if (event == EVENT_SHOULD_AI_PLAY)
    {
      if (player == active_player)
      {
        ai_modifier += basiclandtypes_controlled[player][COLOR_BLUE] * 0xc;
      }
      else
      {
        ai_modifier -= basiclandtypes_controlled[player][COLOR_BLUE] * 0xc;
      }
    }

    if ((event == EVENT_CLEANUP) || (event == EVENT_SHOULD_AI_PLAY))
    {
      PLAYER_CARD_INSTANCE(player, card).eot_toughness = 0;
      PLAYER_CARD_INSTANCE(player, card).info_slot = PLAYER_CARD_INSTANCE(player, card).eot_toughness;
    }
    return 0;
  }
}

// FUNCTION: MAGIC 0x00544ff5
// FUNCTION: SHANDALAR 0x00493239
int card_rag_man(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x005455e2
// FUNCTION: SHANDALAR 0x00493829
int card_time_elemental(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x00545c2e
// FUNCTION: SHANDALAR 0x00493e75
int card_northern_paladin(int player, int card, event_t event)
{
  target_t target;
  int result;

  if (event == EVENT_UNTAP_PHASE)
  {
    ++unk_00939520[player][COLOR_WHITE];
    result = 0;
  }
  else if (event == EVENT_CAN_ACTIVATE)
  {
    result = 1;
    if ((PLAYER_CARD_INSTANCE(player, card).state & (STATE_TAPPED | STATE_SUMMONSICK_NOTAP)) != 0)
    {
      result = 0;
    }
    if (result != 0 && has_mana(player, COLOR_WHITE, 2) == 0)
    {
      result = 0;
    }
    if (result != 0)
    {
      result = real_target_available((int *)0,
                                     TARGET_SCAN_DIRECT,
                                     player,
                                     2,
                                     2,
                                     TARGET_ZONE_IN_PLAY,
                                     TARGET_TYPE_TOKEN | TYPE_ARTIFACT | TYPE_ENCHANTMENT | TYPE_CREATURE | TYPE_LAND,
                                     TYPE_NONE,
                                     0,
                                     get_protections_from(player, card),
                                     1 << (unsigned char)get_sleighted_color(player, card, COLOR_BLACK),
                                     0,
                                     -1,
                                     -1,
                                     0xffffffff,
                                     0xffffffff,
                                     0,
                                     0,
                                     0);
    }
  }
  else if (event == EVENT_GET_SELECTED_CARD)
  {
    FUN_004e4ff3(0);
    result = 0;
  }
  else if (event == EVENT_ACTIVATE)
  {
    result = has_mana(player, COLOR_WHITE, 2);
    if (result != 0 && (PLAYER_CARD_INSTANCE(player, card).state & (STATE_TAPPED | STATE_SUMMONSICK_NOTAP)) == 0)
    {
      charge_mana(player, COLOR_WHITE, 2);
      if (spell_fizzled != 1)
      {
        if (unk_008a9000 != 1)
        {
          load_text("prompts.txt", "NORTHERN_PALADIN");
        }

        if (C_real_select_target(player,
                                 2,
                                 2,
                                 TARGET_ZONE_IN_PLAY,
                                 TARGET_TYPE_TOKEN | TYPE_ARTIFACT | TYPE_ENCHANTMENT | TYPE_CREATURE | TYPE_LAND,
                                 TYPE_NONE,
                                 0,
                                 get_protections_from(player, card),
                                 1 << (unsigned char)get_sleighted_color(player, card, COLOR_BLACK),
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
          PLAYER_CARD_INSTANCE(player, card).targets[0].player = target.player;
          PLAYER_CARD_INSTANCE(player, card).targets[0].card = target.card;
          PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
          PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
        }
      }
    }
    result = 0;
  }
  else if (event == EVENT_RESOLVE_ACTIVATION)
  {
    target.player = PLAYER_CARD_INSTANCE(player, card).targets[0].player;
    target.card = PLAYER_CARD_INSTANCE(player, card).targets[0].card;
    if (C_real_validate_target(target.player,
                               target.card,
                               (char *)0,
                               player,
                               2,
                               2,
                               TARGET_ZONE_IN_PLAY,
                               TARGET_TYPE_TOKEN | TYPE_ARTIFACT | TYPE_ENCHANTMENT | TYPE_CREATURE | TYPE_LAND,
                               TYPE_NONE,
                               0,
                               get_protections_from(player, card),
                               1 << (unsigned char)get_sleighted_color(player, card, COLOR_BLACK),
                               COLOR_TEST_0,
                               -1,
                               ~SUB_WALL,
                               -1,
                               -1,
                               0,
                               0,
                               0) == 0)
    {
      spell_fizzled = 1;
    }
    else
    {
      kill_card(target.player, target.card, KILL_DESTROY);
    }

    PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                         PLAYER_CARD_INSTANCE(player, card).parent_card)
        .number_of_targets = 0;
    result = 0;
  }
  else
  {
    result = 0;
  }

  return result;
}

// FUNCTION: MAGIC 0x005460cb
// FUNCTION: SHANDALAR 0x00494312
int card_royal_assassin(int player, int card, event_t event)
{
  int result;
  target_t selected_target;

  if (event == EVENT_CAN_ACTIVATE)
  {
    result = 0;
    if ((PLAYER_CARD_INSTANCE(player, card).state & 0x20010) == 0)
    {
      result = real_target_available((int *)0,
                                     TARGET_SCAN_DIRECT,
                                     player,
                                     2,
                                     2,
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
                                     1,
                                     0);
    }
    return result;
  }
  else if (event == EVENT_ACTIVATE)
  {
    if (unk_008a9000 != 1)
    {
      load_text("prompts.txt", "ROYAL_ASSASSIN");
    }

    if (C_real_select_target(player,
                             2,
                             1 - player,
                             TARGET_ZONE_IN_PLAY,
                             TYPE_CREATURE,
                             TYPE_NONE,
                             0,
                             get_protections_from(player, card),
                             0,
                             0,
                             -1,
                             ~SUB_WALL,
                             -1,
                             -1,
                             0,
                             TARGET_STATE_TAPPED,
                             0,
                             text_lines[0],
                             1,
                             &selected_target) == 0)
    {
      spell_fizzled = 1;
    }
    else
    {
      PLAYER_CARD_INSTANCE(player, card).targets[0].player = selected_target.player;
      PLAYER_CARD_INSTANCE(player, card).targets[0].card = selected_target.card;
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
      PLAYER_CARD_INSTANCE(player, card).state |= 0x10;
    }
    return 0;
  }
  else if (event == EVENT_GET_SELECTED_CARD)
  {
    FUN_004e4ff3(0);
    return 0;
  }
  else if (event == EVENT_RESOLVE_ACTIVATION)
  {
    selected_target.player = PLAYER_CARD_INSTANCE(player, card).targets[0].player;
    selected_target.card = PLAYER_CARD_INSTANCE(player, card).targets[0].card;
    if (C_real_validate_target(selected_target.player,
                               selected_target.card,
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
                               TARGET_STATE_TAPPED,
                               0) == 0)
    {
      spell_fizzled = 1;
    }
    else
    {
      kill_card(selected_target.player, selected_target.card, KILL_DESTROY);
    }
    PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                         PLAYER_CARD_INSTANCE(player, card).parent_card)
        .number_of_targets = 0;
    return 0;
  }
  else if (((event == EVENT_ATTACK_RATING) && (affected_card == card)) && (affected_card_controller == player))
  {
    unk_00925d3c += 0x30;
    return 0;
  }
  else if (((event == EVENT_BLOCK_RATING) && (affected_card == card)) && (affected_card_controller == player))
  {
    unk_00925d3c += -0x30;
    return 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x005466d0
// FUNCTION: SHANDALAR 0x00494917
int card_osai_vultures(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x00546d3b
// FUNCTION: SHANDALAR 0x00494f82
int card_murk_dwellers(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x00546eca
// FUNCTION: SHANDALAR 0x00495111
int card_sorceress_queen(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x0054760a
// FUNCTION: SHANDALAR 0x00495851
int card_stone_giant(int player, int card, event_t event)
{
  unsigned int target_result;
  unsigned int illegal_abilities;
  int legacy_card;
  unsigned char player_byte;
  target_t selected_target;

  player_byte = (unsigned char)player;
  if (event == EVENT_CAN_ACTIVATE)
  {
    legacy_card = 0;
    if ((PLAYER_CARD_INSTANCE(player, card).state & 0x20010) == 0)
    {
      illegal_abilities = get_protections_from(player, card);
      legacy_card = real_target_available((int *)0,
                                          TARGET_SCAN_DIRECT,
                                          player,
                                          player_byte,
                                          player_byte,
                                          TARGET_ZONE_IN_PLAY,
                                          TYPE_CREATURE,
                                          0,
                                          0,
                                          illegal_abilities,
                                          0,
                                          0,
                                          -1,
                                          -1,
                                          (int)*(short *)&PLAYER_CARD_INSTANCE(player, card).upkeep_colorless - 1U | 0x2000,
                                          0xffffffff,
                                          0,
                                          0,
                                          0);
    }
    return legacy_card;
  }
  else if (event == EVENT_GET_SELECTED_CARD)
  {
    FUN_004e4ff3(0);
    return 0;
  }
  else if ((event == EVENT_ACTIVATE) && ((PLAYER_CARD_INSTANCE(player, card).state & 0x20010) == 0))
  {
    if (unk_008a9000 != 1)
    {
      load_text("prompts.txt", "STONE_GIANT");
    }
    legacy_card = C_real_select_target(player,
                                       player,
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
                                       (int)*(short *)&PLAYER_CARD_INSTANCE(player, card).upkeep_colorless - 1U | 0x2000,
                                       0,
                                       0,
                                       0,
                                       text_lines[0],
                                       1,
                                       &selected_target);
    if (legacy_card == 0)
    {
      spell_fizzled = 1;
    }
    else
    {
      PLAYER_CARD_INSTANCE(player, card).targets[0].player = selected_target.player;
      PLAYER_CARD_INSTANCE(player, card).targets[0].card = selected_target.card;
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
      if ((active_player == player) && ((PLAYER_CARD_INSTANCE(selected_target.player, selected_target.card).token_status & 0x20) != 0))
      {
        ai_modifier += -0x60;
      }
      PLAYER_CARD_INSTANCE(player, card).state |= 0x10;
    }
    return 0;
  }
  else if (event == EVENT_RESOLVE_ACTIVATION)
  {
    selected_target.player = PLAYER_CARD_INSTANCE(player, card).targets[0].player;
    selected_target.card = PLAYER_CARD_INSTANCE(player, card).targets[0].card;
    target_result = C_real_validate_target(selected_target.player,
                                           selected_target.card,
                                           (char *)0,
                                           player,
                                           player_byte,
                                           player_byte,
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
                                           (int)*(short *)&PLAYER_CARD_INSTANCE(player, card).upkeep_colorless - 1U | TARGET_PT_LESSER_OR_EQUAL,
                                           0,
                                           0,
                                           0);
    if (target_result == 0)
    {
      spell_fizzled = 1;
    }
    else
    {
      legacy_card = create_legacy_effect(card_on_stack_controller,
                                         card_on_stack,
                                         unk_00896534,
                                         selected_target.player,
                                         selected_target.card);
      if (legacy_card != -1)
      {
        PLAYER_CARD_INSTANCE(player, legacy_card).mana_color = 5;
        PLAYER_CARD_INSTANCE(player, legacy_card).info_slot = 0x20;
        PLAYER_CARD_INSTANCE(selected_target.player, selected_target.card).token_status = 0x8000000;
      }
    }
    PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                         PLAYER_CARD_INSTANCE(player, card).parent_card)
        .number_of_targets = 0;
    return 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x00547bb3
// FUNCTION: SHANDALAR 0x00495df9
int card_dwarven_warriors(int player, int card, event_t event)
{
  card_instance_t *instance;
  card_instance_t *parent;
  target_t target;

  instance = &PLAYER_CARD_INSTANCE(player, card);
  if (event == EVENT_CAN_ACTIVATE)
  {
    if ((instance->state & 0x20010) == 0 && real_target_available((int *)0,
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
                                                                  0x2002,
                                                                  0xffffffff,
                                                                  0,
                                                                  0,
                                                                  0))
    {
      return 1;
    }
  }
  else if (event == 0x90)
  {
    FUN_004e4ff3(0);
  }
  else if ((event == EVENT_ACTIVATE) && ((instance->state & 0x20010) == 0))
  {
    if (unk_008a9000 != 1)
    {
      load_text("prompts.txt", "DWARVEN_WARRIORS");
    }
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
                              0x2002,
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
      instance->state |= STATE_TAPPED;
    }
  }
  else if (event == EVENT_RESOLVE_ACTIVATION)
  {
    target = instance->targets[0];
    if (!C_real_validate_target(target.player,
                                target.card,
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
                                0x2002,
                                -1,
                                0,
                                0,
                                0))
    {
      spell_fizzled = 1;
    }
    else
    {
      create_legacy_effect(card_on_stack_controller, card_on_stack, unk_007abc7c, target.player, target.card);
    }
    parent = &PLAYER_CARD_INSTANCE(instance->parent_controller, instance->parent_card);
    parent->number_of_targets = 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x00547f99
// FUNCTION: SHANDALAR 0x004961df
int card_cave_people(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x005485e9
// FUNCTION: SHANDALAR 0x0049682f
int card_pradesh_gypsies(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x00548bd9
// FUNCTION: SHANDALAR 0x00496e1d
int card_samite_healer(int player, int card, event_t event)
{
  target_t target;

  if (event == EVENT_CAN_ACTIVATE)
  {
    int result = 1;
    if ((unk_008b4278 & 4) == 0)
    {
      result = 0;
    }
    if (PLAYER_CARD_INSTANCE(player, card).state & 0x20010)
    {
      result = 0;
    }
    if (result != 0)
    {
      if (real_target_available((int *)0,
                                TARGET_SCAN_DIRECT,
                                player,
                                2,
                                2,
                                0x200,
                                0,
                                0,
                                0,
                                0,
                                0,
                                0,
                                unk_009266a4,
                                -1,
                                -1,
                                -1,
                                0,
                                0,
                                0))
      {
        result = 99;
      }
      else
      {
        result = 0;
      }
    }
    return result;
  }

  if (event == EVENT_GET_SELECTED_CARD)
  {
    FUN_004e4ff3(0);
    return 0;
  }

  if (event == EVENT_ACTIVATE)
  {
    if (unk_008a9000 != 1)
    {
      load_text("prompts.txt", "SAMITE_HEALER");
    }
    if (C_real_select_target(player,
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
                             -1,
                             -1,
                             -1,
                             0,
                             0,
                             0,
                             text_lines,
                             1,
                             &target))
    {
      PLAYER_CARD_INSTANCE(player, card).targets[0].player = target.player;
      PLAYER_CARD_INSTANCE(player, card).targets[0].card = target.card;
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
      PLAYER_CARD_INSTANCE(player, card).state |= 0x10;
    }
    else
    {
      spell_fizzled = 1;
    }
    return 0;
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    target.player = PLAYER_CARD_INSTANCE(player, card).targets[0].player;
    target.card = PLAYER_CARD_INSTANCE(player, card).targets[0].card;
    if (C_real_validate_target(target.player,
                               target.card,
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
                               -1,
                               -1,
                               -1,
                               0,
                               0,
                               0))
    {
      if (PLAYER_CARD_INSTANCE(target.player, target.card).info_slot != 0)
      {
        --PLAYER_CARD_INSTANCE(target.player, target.card).info_slot;
      }
    }
    else
    {
      spell_fizzled = 1;
    }
    PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                         PLAYER_CARD_INSTANCE(player, card).parent_card)
        .number_of_targets = 0;
    return 0;
  }

  if (event == 0x3b && (PLAYER_CARD_INSTANCE(player, card).state & 0x20010) == 0)
  {
    ++unk_007a7d88[player];
  }

  return 0;
}

// FUNCTION: MAGIC 0x00549014
// FUNCTION: SHANDALAR 0x00497258
int card_verduran_enchantress(int player, int card, event_t event)
{
  int library_position;

  if ((((((trigger_condition == TRIGGER_SPELL_CAST) && (affected_card == card)) && (affected_card_controller == player)) && ((current_turn == player) && (human_player == player))) && (((PLAYER_CARD_INSTANCE(player, card).state & 0x20) == 0) && ((affected_card_controller == player) && (PLAYER_CARD_INSTANCE(trigger_cause_controller, trigger_cause).internal_card_id != -1)))) && ((global_cards_data[PLAYER_CARD_INSTANCE(trigger_cause_controller, trigger_cause).internal_card_id]
                                                                                                                                                                                                                                                                                                                                                                                                .type &
                                                                                                                                                                                                                                                                                                                                                                                            TYPE_ENCHANTMENT) != 0))
  {
    if (event == EVENT_TRIGGER)
    {
      if ((unk_008b35ec == player) || ((unk_00926804 & 2) != 0))
      {
        event_result |= RESOLVE_TRIGGER_OPTIONAL;
      }
      else
      {
        library_position = 0;
        while ((library_position < 500) && (global_library[player][library_position] != -1))
        {
          ++library_position;
        }

        if ((hand_count[player] < 8) && (5 < library_position))
        {
          event_result |= RESOLVE_TRIGGER_MANDATORY;
        }
        else
        {
          event_result |= RESOLVE_TRIGGER_OPTIONAL;
        }
      }
    }

    if (event == EVENT_RESOLVE_TRIGGER)
    {
      dispatch_event(player, card, EVENT_PLAY_ABILITY);
      FUN_0043e18b(player);
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0054920c
// FUNCTION: SHANDALAR 0x0049744a
int card_serra_angel(int player, int card, event_t event)
{
  if (affected_card == card && affected_card_controller == player)
  {
    PLAYER_CARD_INSTANCE(player, card).state |= STATE_VIGILANCE;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0054926e
// FUNCTION: SHANDALAR 0x004974ab
int card_giant_spider(int player, int card, event_t event)
{
  if (event == EVENT_ABILITIES)
  {
    if (affected_card == card)
    {
      if (affected_card_controller == player)
      {
        if (human_player == player)
        {
          event_result &= ~KEYWORD_FLYING;
        }
      }
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x005492be
// FUNCTION: SHANDALAR 0x004974fb
int card_zombie_master(int player, int card, event_t event)
{
  int color;

  if (event == EVENT_ABILITIES)
  {
    if (is_in_play(player, card))
    {
      if (global_cards_data[PLAYER_CARD_INSTANCE(affected_card_controller, affected_card)
                                .internal_card_id]
              .subtype == SUB_ZOMBIE)
      {
        color = get_hacked_color(player, card, COLOR_BLACK);
        event_result |= (1 << (((char)color - 1U) & 0x1f)) + KEYWORD_REGENERATION;
      }
    }
  }

  if (((event == EVENT_GRAVEYARD_FROM_PLAY) && (affected_card == card)) && (player == affected_card_controller))
  {
    FUN_0055117d(FUN_005493a6, -1);
    TENTATIVE_reassess_all_cards();
  }

  return 0;
}

// FUNCTION: MAGIC 0x005493a6
int FUN_005493a6(int player, int card, int internal_card_id)
{
  if (global_cards_data[internal_card_id].subtype == SUB_ZOMBIE)
  {
    PLAYER_CARD_INSTANCE(player, card).regen_status |= 0x8000000;
  }

  return 1;
}

// FUNCTION: MAGIC 0x00549404
// FUNCTION: SHANDALAR 0x00497640
int card_scathe_zombies(int player, int card, event_t event)
{
  if (((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE)) || (event == EVENT_RESOLVE_ACTIVATION))
  {
    return FUN_0054276d(player, card, event, 1, 1);
  }

  return 0;
}

// FUNCTION: MAGIC 0x00549451
// FUNCTION: SHANDALAR 0x0049768d
int card_goblin_king(int player, int card, event_t event)
{
  if (is_in_play(player, card) && is_in_play(affected_card_controller, affected_card) && (char)global_cards_data[PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).internal_card_id].subtype == 3)
  {
    if (event == EVENT_ABILITIES)
    {
      event_result |= 1 << (get_hacked_color(player, card, COLOR_RED) - 1);
    }
    if (event == EVENT_POWER || event == EVENT_TOUGHNESS)
    {
      ++event_result;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x00549524
// FUNCTION: SHANDALAR 0x00497760
int card_erg_raiders(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x00549726
// FUNCTION: SHANDALAR 0x00497962
int card_el_hajjaj(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x00549c89
// FUNCTION: SHANDALAR 0x00497ec5
int card_leviathan(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x0054a397
// FUNCTION: SHANDALAR 0x004985d4
int card_brothers_of_fire(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x0054a549
// FUNCTION: SHANDALAR 0x00498786
int card_crimson_manticore(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x0054a96d
// FUNCTION: SHANDALAR 0x00498baa
int card_prodigal_sorcerer(int player, int card, event_t event)
{
  unsigned int *state_ptr;
  unsigned int result;

  if (event == EVENT_CAN_ACTIVATE)
  {
    result = (unsigned int)((PLAYER_CARD_INSTANCE(player, card).state & 0x20010) == 0);
  }
  else if (event == EVENT_GET_SELECTED_CARD)
  {
    FUN_004e4ff3(1);
    result = 0;
  }
  else if (event == EVENT_ACTIVATE)
  {
    if (unk_008a9000 != 1)
    {
      load_text("prompts.txt", "PRODIGAL_SORCERER");
    }
    FUN_0054ac4d(player, card, 1);
    if (spell_fizzled != 1)
    {
      state_ptr = (unsigned int *)&PLAYER_CARD_INSTANCE(player, card).state;
      *state_ptr |= 0x10;
    }
    result = 0;
  }
  else if (event == EVENT_RESOLVE_ACTIVATION)
  {
    FUN_0054af10(player, card, event, 1);
    PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                         PLAYER_CARD_INSTANCE(player, card).parent_card)
        .number_of_targets = 0;
    result = 0;
  }
  else if ((event == EVENT_CHECK_PUMP) && ((PLAYER_CARD_INSTANCE(player, card).state & 0x20014) == 0))
  {
    --unk_007a7d88[1 - player];
    result = 0;
  }
  else
  {
    if ((event == EVENT_SHOULD_AI_PLAY) && (player == human_player) && (active_player == player) && ((PLAYER_CARD_INSTANCE(player, card).state & 0x20010) == 0))
    {
      ai_modifier += 0x18;
    }

    if (((event == EVENT_ATTACK_RATING) && (affected_card == card)) && (affected_card_controller == player))
    {
      unk_00925d3c += 0x30;
      result = 0;
    }
    else if (((event == EVENT_BLOCK_RATING) && (affected_card == card)) && (affected_card_controller == player))
    {
      unk_00925d3c += -0x30;
      result = 0;
    }
    else
    {
      result = 0;
    }
  }

  return result;
}

// FUNCTION: MAGIC 0x0054ac4d
int FUN_0054ac4d(int player, int card, int damage_unused)
{
  struct
  {
    char (*prompt)[300];
    int target_card;
    target_t target;
    int unused;
  } s;

  s.unused = 0;
  if (unk_008b35ec == player || (unk_00926804 & 2) != 0)
  {
    if (unk_008a9000 != 1)
    {
      if (!C_real_select_target(player,
                                2,
                                1 - player,
                                TARGET_ZONE_PLAYERS | TARGET_ZONE_IN_PLAY,
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
                                0,
                                text_lines[0],
                                1,
                                &s.target) == 0)
      {
        s.target_card = s.target.card;
        unk_00742fcc = s.target.player;
      }
      else
      {
        spell_fizzled = 1;
        s.target_card = -1;
        unk_00742fcc = -1;
      }
    }
    else
    {
      s.target_card = -1;
      unk_00742fcc = 1 - player;
    }
  }
  else
  {
    if (unk_008a9000 == 1)
    {
      if (internal_rand(3) != 0)
        unk_00939340 = 0;
      else
        unk_00939340 = 1;

      FUN_004e4f11();
    }
    else
    {
      FUN_004e5089();
    }

    if (unk_00939340 == 0)
    {
      if (unk_008a9000 == 1)
        s.prompt = "";
      else
        s.prompt = text_lines;

      C_real_select_target(player,
                           2,
                           1 - player,
                           TARGET_ZONE_PLAYERS | TARGET_ZONE_IN_PLAY,
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
                           0,
                           *s.prompt,
                           1,
                           &s.target);
      s.target_card = s.target.card;
      unk_00742fcc = s.target.player;
    }
    else
    {
      s.target_card = -1;
      unk_00742fcc = 1 - player;
      if (unk_008a9000 == 1)
      {
        unk_00939340 = 0;
        unk_00925bb8 = ((unk_00742fcc == 0) ? 0 : 0x100) | 0xff;
        FUN_004e4f11();
      }
      else
      {
        FUN_004e5089();
      }
    }
  }

  if (spell_fizzled != 1)
  {
    PLAYER_CARD_INSTANCE(player, card).targets[0].card = s.target_card;
    PLAYER_CARD_INSTANCE(player, card).targets[0].player = unk_00742fcc;
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
    return 1;
  }
  else
    return 0;
}

// FUNCTION: MAGIC 0x0054af10
int FUN_0054af10(int player, int card, event_t event, int amount)
{
  int source_player;
  int source_card;

  if (PLAYER_CARD_INSTANCE(player, card).number_of_targets == 0)
    return 0;

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    source_player = card_on_stack_controller;
    source_card = card_on_stack;
  }
  else
  {
    source_player = player;
    source_card = card;
  }

  if (PLAYER_CARD_INSTANCE(player, card).targets[0].player == -1 && PLAYER_CARD_INSTANCE(player, card).targets[0].card == -1)
    return 0;

  if (!C_real_validate_target(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                              PLAYER_CARD_INSTANCE(player, card).targets[0].card,
                              NULL,
                              player,
                              2,
                              2,
                              TARGET_ZONE_PLAYERS | TARGET_ZONE_IN_PLAY,
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
    return 0;
  }

  if (PLAYER_CARD_INSTANCE(player, card).targets[0].card == -1)
  {
    damage_player(PLAYER_CARD_INSTANCE(player, card).targets[0].player, amount, source_player, source_card);
  }
  else
  {
    damage_creature(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                    PLAYER_CARD_INSTANCE(player, card).targets[0].card,
                    amount,
                    source_player,
                    source_card);
  }

  return 1;
}

// FUNCTION: MAGIC 0x0054b1ad
// FUNCTION: SHANDALAR 0x004993ea
int card_pirate_ship(int player, int card, event_t event)
{
  if (event == EVENT_CAN_ACTIVATE)
  {
    return (PLAYER_CARD_INSTANCE(player, card).state & (STATE_SUMMONSICK_NOTAP | STATE_TAPPED)) == 0;
  }

  if (event == EVENT_GET_SELECTED_CARD)
  {
    FUN_004e4ff3(1);
    return 0;
  }

  if (event == EVENT_ACTIVATE)
  {
    if (unk_008a9000 != 1)
    {
      load_text("prompts.txt", "PIRATE_SHIP");
    }
    FUN_0054ac4d(player, card, 1);
    if (spell_fizzled != 1)
    {
      PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
    }
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    FUN_0054af10(player, card, event, 1);
    PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                         PLAYER_CARD_INSTANCE(player, card).parent_card)
        .number_of_targets = 0;
  }

  card_sea_serpent(player, card, event);

  return 0;
}

// FUNCTION: MAGIC 0x0054b360
// FUNCTION: SHANDALAR 0x0049959d
int card_sea_serpent(int player, int card, event_t event)
{
  if (is_in_play(player, card))
  {
    if (basiclandtypes_controlled[player][get_hacked_color(player, card, COLOR_BLUE)] == 0)
    {
      kill_card(player, card, KILL_DESTROY);
    }
  }

  if (event == EVENT_ATTACK_LEGALITY)
  {
    if (basiclandtypes_controlled[1 - player][get_hacked_color(player, card, COLOR_BLUE)] == 0)
    {
      event_result = 1;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0054b401
// FUNCTION: SHANDALAR 0x0049963e
int card_elder_land_wurm(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x0054b4ed
// FUNCTION: SHANDALAR 0x00499729
int card_island_fish_jasconius(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x0054b8ad
// FUNCTION: SHANDALAR 0x00499ae9
int card_lord_of_atlantis(int player, int card, event_t event)
{
  if (is_in_play(player, card) && is_in_play(affected_card_controller, affected_card) && (char)global_cards_data[PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).internal_card_id].subtype == 1)
  {
    if (event == EVENT_ABILITIES)
    {
      event_result |= 1 << (get_hacked_color(player, card, COLOR_BLUE) - 1);
    }
    else if (event == EVENT_POWER || event == EVENT_TOUGHNESS)
    {
      ++event_result;
    }
    else if (event == EVENT_GRAVEYARD_FROM_PLAY && (PLAYER_CARD_INSTANCE(player, card).token_status & 0x80) != 0)
    {
      PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).regen_status |= 0xe000000;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0054ba0b
// FUNCTION: SHANDALAR 0x00499c47
int card_goblin_rock_sled(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x0054baf9
// FUNCTION: SHANDALAR 0x00499d35
int card_orcish_artillery(int player, int card, event_t event)
{
  if (event == EVENT_CAN_ACTIVATE)
  {
    return (PLAYER_CARD_INSTANCE(player, card).state & (STATE_SUMMONSICK_NOTAP | STATE_TAPPED)) == 0 ? 1 : 0;
  }

  if (event == EVENT_GET_SELECTED_CARD)
  {
    FUN_004e4ff3(1);
    return 0;
  }

  if (event == EVENT_ACTIVATE)
  {
    if (unk_008a9000 != 1)
    {
      load_text("prompts.txt", "ORCISH_ARTILLERY");
    }
    FUN_0054ac4d(player, card, 2);
    if (spell_fizzled != 1)
    {
      PLAYER_CARD_INSTANCE(player, card).state |= 0x10;
    }
    return 0;
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    if (FUN_0054af10(player, card, event, 2) != 0)
    {
      damage_player(player, 3, card_on_stack_controller, card_on_stack);
    }
    PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                         PLAYER_CARD_INSTANCE(player, card).parent_card)
        .number_of_targets = 0;

    return 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0054bcc8
// FUNCTION: SHANDALAR 0x00499f04
int card_psionic_entity(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x0054bfaf
// FUNCTION: SHANDALAR 0x0049a1eb
int card_scavenging_ghoul(int player, int card, event_t event)
{
  int result;

  if (event == EVENT_CAN_ACTIVATE && (unk_008b4278 & 0x200) != 0)
  {
    result = FUN_0054276d(player, card, EVENT_CAN_ACTIVATE, 0, 0);
    if (C_get_special_counters(player, card) == 0)
    {
      result = 0;
    }
    return result;
  }

  if (event == EVENT_ACTIVATE && (unk_008b4278 & 0x200) != 0)
  {
    result = FUN_0054276d(player, card, EVENT_ACTIVATE, 0, 0);
    FUN_005514cd(player, card, 1);
    return result;
  }

  if (event == EVENT_RESOLVE_ACTIVATION && (unk_008b4278 & 0x200) != 0)
  {
    return FUN_0054276d(player, card, EVENT_RESOLVE_ACTIVATION, 0, 0);
  }

  if ((trigger_condition == 0xcd || event == 199) && affected_card == card && affected_card_controller == player && unk_008cfdac != 0 && player == current_turn)
  {
    if (event == 0x7d)
    {
      event_result |= 2;
    }
    if (event == 0x7e || event == 199)
    {
      FUN_005513d7(player, card, unk_008cfdac);
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0054c131
// FUNCTION: SHANDALAR 0x0049a36e
int card_sengir_vampire(int player, int card, event_t event)
{
  struct
  {
    int dead_internal_id;   /* [ebp-0x10] */
    int count_triggered;   /* [ebp-0x0c] */
    int shift_index;       /* [ebp-0x08] */
    int tracked;           /* [ebp-0x04] */
  } s;

  if (event == 0x6e && PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).internal_card_id == unk_009266a4 && PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).info_slot != 0)
  {
    if (PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).damage_source_card == card && PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).damage_source_player == player && PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).damage_target_card != -1)
    {
      s.shift_index = PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).damage_target_player;
      if (PLAYER_CARD_INSTANCE(
              s.shift_index, PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).damage_target_card)
              .internal_card_id == -1)
      {
        s.dead_internal_id =
            PLAYER_CARD_INSTANCE(
                s.shift_index, PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).damage_target_card)
                .original_internal_card_id;
      }
      else
      {
        s.dead_internal_id =
            PLAYER_CARD_INSTANCE(
                s.shift_index, PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).damage_target_card)
                .internal_card_id;
      }

      if ((global_cards_data[s.dead_internal_id].type & TYPE_CREATURE) != 0 && PLAYER_CARD_INSTANCE(player, card).number_of_targets < 0x13)
      {
        PLAYER_CARD_INSTANCE(player, card).targets[PLAYER_CARD_INSTANCE(player, card).number_of_targets].card =
            PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).damage_target_card;
        PLAYER_CARD_INSTANCE(player, card).targets[PLAYER_CARD_INSTANCE(player, card).number_of_targets].player =
            s.shift_index;
        ++PLAYER_CARD_INSTANCE(player, card).number_of_targets;
      }
    }
  }
  else if (event == 0x77)
  {
    s.count_triggered = 0;
    for (s.tracked = 0; s.tracked < PLAYER_CARD_INSTANCE(player, card).number_of_targets; ++s.tracked)
    {
      if (PLAYER_CARD_INSTANCE(player, card).targets[s.tracked].card == affected_card && PLAYER_CARD_INSTANCE(player, card).targets[s.tracked].player == affected_card_controller && PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).kill_code != 4 && (PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).token_status & 0x4000000) == 0)
      {
        s.shift_index = s.tracked;
        if (!s.count_triggered)
        {
          ++PLAYER_CARD_INSTANCE(player, card).info_slot;
          s.count_triggered = 1;
        }

        while (++s.shift_index < PLAYER_CARD_INSTANCE(player, card).number_of_targets)
        {
          PLAYER_CARD_INSTANCE(player, card).targets[s.shift_index - 1].player =
              PLAYER_CARD_INSTANCE(player, card).targets[s.shift_index].player;
          PLAYER_CARD_INSTANCE(player, card).targets[s.shift_index - 1].card =
              PLAYER_CARD_INSTANCE(player, card).targets[s.shift_index].card;
        }
        --PLAYER_CARD_INSTANCE(player, card).number_of_targets;
      }
    }
  }
  else if (trigger_condition == 0xd5 && PLAYER_CARD_INSTANCE(player, card).info_slot != 0 && current_turn == player && affected_card == card && affected_card_controller == player)
  {
    if (event == 0x7d)
    {
      event_result |= 2;
    }
    if (event == 0x7e)
    {
      FUN_005513d7(player, card, PLAYER_CARD_INSTANCE(player, card).info_slot);
      PLAYER_CARD_INSTANCE(player, card).counter_power += (short)PLAYER_CARD_INSTANCE(player, card).info_slot;
      PLAYER_CARD_INSTANCE(player, card).counter_toughness +=
          (short)PLAYER_CARD_INSTANCE(player, card).info_slot;
      PLAYER_CARD_INSTANCE(player, card).info_slot = 0;
    }
  }
  else if (event == 0x22 || event == 199)
  {
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0054cb2b
// FUNCTION: SHANDALAR 0x0049ad68
int card_junun_efreet(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x0054cc76
// FUNCTION: SHANDALAR 0x0049aeb3
int card_phantasmal_forces(int player, int card, event_t event)
{
  if (((event == EVENT_SETUP_UPKEEP_COSTS) && (affected_card == card)) && (affected_card_controller == player))
  {
    if ((human_player == player) && (player == unk_00742f60))
    {
      PLAYER_CARD_INSTANCE(player, card).upkeep_flags |= 1;
      PLAYER_CARD_INSTANCE(player, card).upkeep_blue += 1;
    }
    return 0;
  }

  if (event == EVENT_UPKEEP_COSTS_UNPAID)
  {
    kill_card(card_on_stack_controller, card_on_stack, KILL_BURY);
    return 0;
  }

  if ((event == EVENT_SHOULD_AI_PLAY) && (basiclandtypes_controlled[player][COLOR_BLUE] < 1))
  {
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x0054cdc0
// FUNCTION: SHANDALAR 0x0049affe
int card_force_of_nature(int player, int card, event_t event)
{
  if (event == EVENT_SETUP_UPKEEP_COSTS)
  {
    if (affected_card == card)
    {
      if (affected_card_controller == player)
      {
        if (human_player == player)
        {
          if (unk_00742f60 == player)
          {
            PLAYER_CARD_INSTANCE(player, card).upkeep_flags |= 1;
            PLAYER_CARD_INSTANCE(player, card).upkeep_green =
                PLAYER_CARD_INSTANCE(player, card).upkeep_green + '\x04';
          }
        }
      }
    }
  }
  else if (event == EVENT_UPKEEP_COSTS_UNPAID)
  {
    load_text("prompts.txt", "FORCE_OF_NATURE");
    do_dialog(player, player, card, -1, -1, text_lines[0], 0);
    damage_player(player, 8, card_on_stack_controller, card_on_stack);
  }
  else if ((event == EVENT_SHOULD_AI_PLAY) && (basiclandtypes_controlled[player][COLOR_GREEN] < 4))
  {
    damage_player(player, 8, player, card);
  }

  return 0;
}

// FUNCTION: MAGIC 0x0054cf44
// FUNCTION: SHANDALAR 0x0049b183
int card_llanowar_elves(int player, int card, event_t event)
{
  if (event == EVENT_CAN_ACTIVATE)
  {
    return (unsigned int)((PLAYER_CARD_INSTANCE(player, card).state & 0x20010) == 0);
  }

  if ((event == EVENT_ACTIVATE) && ((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0))
  {
    undeclare_mana_available_and_produce_it(player, COLOR_GREEN, 1);
    PLAYER_CARD_INSTANCE(player, card).state |= 0x10;
    produced_mana_color = COLOR_GREEN;
    return 0;
  }

  if ((event == EVENT_COUNT_MANA) && (affected_card == card) && (affected_card_controller == player))
  {
    if ((PLAYER_CARD_INSTANCE(player, card).state & 0x20010) == 0)
    {
      declare_mana_available(player, COLOR_GREEN, 1);
    }
    return 0;
  }

  if ((event == EVENT_ATTACK_RATING) && (affected_card == card) && (affected_card_controller == player))
  {
    unk_00925d3c += 0x18 / (basiclandtypes_controlled[player][COLOR_GREEN] + 2);
    return 0;
  }

  if ((event == EVENT_BLOCK_RATING) && (affected_card == card) && (affected_card_controller == player))
  {
    unk_00925d3c -= 0x60 / (basiclandtypes_controlled[player][COLOR_GREEN] + 2);
    return 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0054d153
// FUNCTION: SHANDALAR 0x0049b392
int card_birds_of_paradise(int player, int card, event_t event)
{
  unsigned int available_colors;
  color_t choice_hint;
  color_t color;
  int current_color;
  card_instance_t *instance;
  char message[300];

  instance = &PLAYER_CARD_INSTANCE(player, card);
  if (event == EVENT_CAN_ACTIVATE)
  {
    return (instance->state & 0x20010) == 0;
  }
  else if ((event == EVENT_ACTIVATE) && ((instance->state & STATE_TAPPED) == 0))
  {
    available_colors = (unsigned int)(unsigned char)instance->damage_source_card;
    if ((((player == active_player) && ((unk_00926804 & 2) == 0)) || (unk_008a9000 == 1)) || (unk_009252e0 != 0))
    {
      choice_hint = -1;
      current_color = 1;
      while ((current_color < 6) && (choice_hint == -1))
      {
        if (((unk_00938e2c & (1 << (((unsigned char)current_color) & 0x1f))) != 0) && ((available_colors & (1 << (((unsigned char)current_color) & 0x1f))) != 0))
        {
          choice_hint = current_color;
        }
        ++current_color;
      }
      if ((choice_hint == -1) && ((unk_00938e2c & 1) != 0))
      {
        choice_hint = COLOR_BLACK;
      }
      if ((choice_hint == -1) && ((unk_00938e2c & 0x40) != 0))
      {
        choice_hint = COLOR_BLACK;
      }
      if (choice_hint == -1)
      {
        spell_fizzled = 1;
      }
    }
    else
    {
      choice_hint = -1;
    }
    if (spell_fizzled != 1)
    {
      if (unk_008a9000 != 1)
      {
        load_text("prompts.txt", "BIRDS_OF_PARADISE");
      }
      color = choose_a_color(player, text_lines[0], 1, choice_hint, (unsigned char)instance->damage_source_card);
      if (color == ~COLOR_COLORLESS)
      {
        spell_fizzled = 1;
      }
      else
      {
        produce_mana(player, color, 1);
        undeclare_mana_available_hex(player, (unsigned char)instance->damage_source_card, 1);
        instance->state |= STATE_TAPPED;
        produced_mana_color = color;
        if (((player == active_player) && ((unk_00926804 & 2) == 0)) && (unk_008a9000 != 1))
        {
          load_text("prompts.txt", "BIRDS_OF_PARADISE2");
          if (color == COLOR_BLACK)
          {
            strcpy(message, text_lines[1]);
          }
          else if (color == COLOR_BLUE)
          {
            strcpy(message, text_lines[2]);
          }
          else if (color == COLOR_GREEN)
          {
            strcpy(message, text_lines[3]);
          }
          else if (color == COLOR_RED)
          {
            strcpy(message, text_lines[4]);
          }
          else
          {
            strcpy(message, text_lines[5]);
          }
          do_dialog(player, player, card, -1, -1, message, 0);
        }
      }
    }
  }
  else if ((event == EVENT_COUNT_MANA) && (card == affected_card) && (player == affected_card_controller))
  {
    if ((instance->state & 0x20010) == 0)
    {
      declare_mana_available_hex(player, (unsigned char)instance->damage_source_card, 1);
    }
  }
  else if ((event == 0x8a) && (card == affected_card) && (player == affected_card_controller))
  {
    unk_00925d3c += 0x60 / (basiclandtypes_controlled[player][7] + 2);
  }
  else if ((event == 0x8b) && (card == affected_card) && (player == affected_card_controller))
  {
    unk_00925d3c -= 0x60 / (basiclandtypes_controlled[player][7] + 2);
  }

  return 0;
}

// FUNCTION: MAGIC 0x0054d69d
// FUNCTION: SHANDALAR 0x0049b8dc
int card_cosmic_horror(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x0054d8ca
// FUNCTION: SHANDALAR 0x0049bb0a
int card_lord_of_the_pit(int player, int card, event_t event)
{
  int creature_to_sacrifice;

  if (event == EVENT_CAST_SPELL && affected_card == card && affected_card_controller == player)
  {
    if (creature_cards_in_play[player] < 2)
    {
      ai_modifier -= 0xa8;
    }
    return 0;
  }

  if (event == EVENT_CHECK_UPK_PAYMENT)
  {
    if (FUN_0054dccd(player, card) == 0)
    {
      event_result |= 1;
    }
    return 0;
  }

  if (event == EVENT_SETUP_UPKEEP_COSTS && affected_card == card && affected_card_controller == player)
  {
    return 0;
  }

  if (event == EVENT_SHOULD_AI_PLAY)
  {
    if (FUN_0054dccd(player, card) == 0)
    {
      ++ai_modifier;
    }
    return 0;
  }

  if (event == EVENT_UPKEEP_PHASE && affected_card == card && affected_card_controller == player)
  {
    ++PLAYER_CARD_INSTANCE(player, card).info_slot;
    if (FUN_0054dccd(player, card) == 0)
    {
      event_result |= 1;
    }
    else
    {
      PLAYER_CARD_INSTANCE(player, card).state |= 0x100000;
      TENTATIVE_reassess_all_cards(0, 0x20);
      load_text("prompts.txt", "LORD_OF_THE_PIT");

      creature_to_sacrifice = FUN_00551921(player);

      PLAYER_CARD_INSTANCE(player, card).state &= 0xffefffff;
      if (creature_to_sacrifice != -1)
      {
        kill_card(player, creature_to_sacrifice, KILL_SACRIFICE);
      }
      else
      {
        event_result |= 1;
      }
    }
    return 0;
  }

  if (event == EVENT_UPKEEP_COSTS_UNPAID)
  {
    damage_player(player, 7, card_on_stack_controller, card_on_stack);
    return 0;
  }

  if (event == EVENT_CLEANUP && affected_card == card && affected_card_controller == player)
  {
    PLAYER_CARD_INSTANCE(player, card).info_slot = 0;
  }

  if (event == EVENT_ATTACK_RATING && affected_card == card && affected_card_controller == player)
  {
    unk_00925d3c -= 0x30;
  }
  else if (event == EVENT_BLOCK_RATING && affected_card == card && affected_card_controller == player)
  {
    unk_00925d3c += 0x30;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0054dccd
int FUN_0054dccd(int player, int card)
{
  int current_card;

  for (current_card = 0; current_card < active_cards_count[player]; ++current_card)
  {
    if (current_card != card && is_in_play(player, current_card) && (global_cards_data[PLAYER_CARD_INSTANCE(player, current_card).internal_card_id].type & TYPE_CREATURE) != 0)
    {
      return 1;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0054dd89
// FUNCTION: SHANDALAR 0x0049bfca
int card_ball_lightning(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x0054dee6
// FUNCTION: SHANDALAR 0x0049c127
int card_cyclopean_mummy(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x0054df54
// FUNCTION: SHANDALAR 0x0049c195
int card_nether_shadow(int player, int card, event_t event)
{
  int hand_card;
  int hand_player;

  if (card == affected_card)
  {
    if (player == affected_card_controller)
    {
      *(unsigned int *)&PLAYER_CARD_INSTANCE(player, card).state &= 0xfffcffff;
    }
  }

  if (event == 0x8d ||
      (event == 0x77 &&
       card == affected_card &&
       player == affected_card_controller &&
       (PLAYER_CARD_INSTANCE(player, card).state & 0x20) == 0 &&
       PLAYER_CARD_INSTANCE(player, card).kill_code != 4))
  {
    if ((PLAYER_CARD_INSTANCE(player, card).state & 0x1000) != 0)
    {
      hand_player = active_player;
    }
    else
    {
      hand_player = unk_008b35ec;
    }

    hand_card = add_card_to_hand(hand_player, unk_008a9194);
    if (hand_card != -1)
    {
      *(unsigned int *)&PLAYER_CARD_INSTANCE(hand_player, hand_card).state |= 2;
      *(int *)((char *)&PLAYER_CARD_INSTANCE(hand_player, hand_card) + 0x64) =
          global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].id;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0054e160
// FUNCTION: SHANDALAR 0x0049c3a1
int card_nether_link(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x0054e36e
// FUNCTION: SHANDALAR 0x0049c5b0
int card_white_knight(int player, int card, event_t event)
{
  int saved_event_result;

  if (event == EVENT_ABILITIES && affected_card == card && affected_card_controller == player)
  {
    event_result |= KEYWORD_PROT_BLACK << ((get_sleighted_color(player, card, COLOR_BLACK) - 1U));
    saved_event_result = event_result;
    FUN_0054e470(player, card, COLOR_BLACK);
    event_result = saved_event_result;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0054e3ef
// FUNCTION: SHANDALAR 0x0049c630
int card_black_knight(int player, int card, event_t event)
{
  int saved_event_result;

  if ((event == 0x34) && (card == affected_card) && (player == affected_card_controller))
  {
    event_result |= 0x800 << (get_sleighted_color(player, card, 5) - 1U);
    saved_event_result = event_result;
    FUN_0054e470(player, card, 5);
    event_result = saved_event_result;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0054e470
void FUN_0054e470(int player, int card, int color)
{
  int test_player;
  int test_card;

  for (test_player = 0; test_player < 2; ++test_player)
  {
    for (test_card = 0; test_card < active_cards_count[test_player]; ++test_card)
    {
      if (is_in_play(test_player, test_card))
      {
        if ((int)(char)PLAYER_CARD_INSTANCE(test_player, test_card).damage_target_player == player &&
            PLAYER_CARD_INSTANCE(test_player, test_card).damage_target_card == card)
        {
          if (((1 << (unsigned char)get_sleighted_color(player, card, color)) &
               (char)PLAYER_CARD_INSTANCE(test_player, test_card).color) != 0 &&
              (((unsigned char)global_cards_data[PLAYER_CARD_INSTANCE(test_player, test_card).internal_card_id].type & 4) != 0))
          {
            kill_card(test_player, test_card, 1);
          }
        }
      }
    }
  }
}

// FUNCTION: MAGIC 0x0054e5f3
// FUNCTION: SHANDALAR 0x0049c834
int card_ali_baba(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x0054e986
// FUNCTION: SHANDALAR 0x0049cbc7
int card_ley_druid(int player, int card, event_t event)
{
  card_instance_t *instance;
  target_t target;

  instance = &PLAYER_CARD_INSTANCE(player, card);
  if (event == EVENT_CAN_ACTIVATE)
  {
    if ((instance->state & 0x20010) == 0 && real_target_available((int *)0,
                                                                  TARGET_SCAN_DIRECT,
                                                                  player,
                                                                  2,
                                                                  2,
                                                                  0x200,
                                                                  TYPE_LAND,
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
                                                                  0) != 0)
    {
      return 1;
    }
    return 0;
  }

  if (event == 0x90)
  {
    FUN_004e4ff3(0);
    return 0;
  }

  if (event == EVENT_ACTIVATE)
  {
    if (unk_008a9000 != 1)
    {
      load_text(0, "LEY DRUID");
    }

    if (C_real_select_target(player,
                             2,
                             player,
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
                             &target) == 0)
    {
      spell_fizzled = 1;
    }
    else
    {
      instance->state |= STATE_TAPPED;
      instance->targets[0] = target;
      instance->number_of_targets = 1;
    }
    return 0;
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    target = instance->targets[0];
    if (C_real_validate_target(target.player,
                               target.card,
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
      spell_fizzled = 1;
    }
    else
    {
      PLAYER_CARD_INSTANCE(target.player, target.card).state &= ~STATE_TAPPED;
      dispatch_event_to_single_card(target.player, target.card, EVENT_UNTAP_PHASE, -1, -1);
    }
    PLAYER_CARD_INSTANCE(instance->parent_controller, instance->parent_card).number_of_targets = 0;
    return 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0054ed44
// FUNCTION: SHANDALAR 0x0049cf85
int card_venom(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x0054f7fe
// FUNCTION: SHANDALAR 0x0049da3f
int card_thicket_basilisk(int player, int card, event_t event)
{
  int opponent;
  int current_card;
  int blocking_card;
  int blocking_iid;
  char grouped_blocker;

  if (event == 0x3c)
  {
    PLAYER_CARD_INSTANCE(player, card).destroys_if_blocked = PLAYER_CARD_INSTANCE(player, card).destroys_if_blocked | 0x3f;
  }
  else if (event == 0x1a)
  {
    opponent = 1 - player;
    if ((player == human_player) && ((PLAYER_CARD_INSTANCE(player, card).state & 0x44) != 0))
    {
      if (PLAYER_CARD_INSTANCE(player, card).blocking == -1)
      {
        blocking_card = card;
      }
      else
      {
        blocking_card = (int)(char)PLAYER_CARD_INSTANCE(player, card).blocking;
      }

      for (current_card = 0; current_card < active_cards_count[opponent]; current_card = current_card + 1)
      {
        blocking_iid = PLAYER_CARD_INSTANCE(opponent, current_card).internal_card_id;
        if (((PLAYER_CARD_INSTANCE(opponent, current_card).blocking == blocking_card) && (blocking_iid != -1)) && ((global_cards_data[blocking_iid].subtype != 0) && ((global_cards_data[blocking_iid].type & TYPE_CREATURE) != 0)))
        {
          create_legacy_effect(player, card, unk_008b3104, opponent, current_card);
        }
      }
    }

    if ((player != human_player) && (PLAYER_CARD_INSTANCE(player, card).blocking != -1))
    {
      grouped_blocker = (char)PLAYER_CARD_INSTANCE(opponent, (int)(char)PLAYER_CARD_INSTANCE(player, card).blocking).blocking;
      if (grouped_blocker == -1)
      {
        blocking_iid = PLAYER_CARD_INSTANCE(opponent, (int)(char)PLAYER_CARD_INSTANCE(player, card).blocking).internal_card_id;
        if ((blocking_iid != -1) && ((global_cards_data[blocking_iid].subtype != 0) && ((global_cards_data[blocking_iid].type & TYPE_CREATURE) != 0)))
        {
          create_legacy_effect(player, card, unk_008b3104, opponent, (int)(char)PLAYER_CARD_INSTANCE(player, card).blocking);
        }
      }
      else
      {
        for (current_card = 0; current_card < active_cards_count[opponent]; current_card = current_card + 1)
        {
          blocking_iid = PLAYER_CARD_INSTANCE(opponent, current_card).internal_card_id;
          if ((is_in_play(opponent, current_card) != 0) && (PLAYER_CARD_INSTANCE(opponent, current_card).blocking == grouped_blocker) && ((global_cards_data[blocking_iid].subtype != 0) && ((global_cards_data[blocking_iid].type & TYPE_CREATURE) != 0)))
          {
            create_legacy_effect(player, card, unk_008b3104, opponent, current_card);
          }
        }
      }
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0054fcbc
// FUNCTION: SHANDALAR 0x0049deff
int card_wall_of_dust(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x0054ffb0
// FUNCTION: SHANDALAR 0x0049e1f3
int card_sisters_of_the_flame(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x0055011b
// FUNCTION: SHANDALAR 0x0049e35e
int card_apprentice_wizard(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x0055025c
// FUNCTION: SHANDALAR 0x0049e49f
int card_abomination(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x00550761
// FUNCTION: SHANDALAR 0x0049e9a4
int card_radjan_spirit(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x00550c4b
// FUNCTION: SHANDALAR 0x0049ee8e
int card_hurr_jackal(int player, int card, event_t event)
{
}
