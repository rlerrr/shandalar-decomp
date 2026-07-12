#include <string.h>
#include "../game_support.h"
#include "../global_strings.h"

// FUNCTION: MAGIC 0x004650b0
// FUNCTION: SHANDALAR 0x004547d0
int card_ashnod_s_altar(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x004652e8
// FUNCTION: SHANDALAR 0x00454a08
int card_ashnod_s_transmogrant(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x00465b27
// FUNCTION: SHANDALAR 0x00455247
int card_basalt_monolith(int player, int card, event_t event)
{
  int result;

  if (((event == EVENT_UNTAP) && (card == affected_card)) && (player == affected_card_controller))
  {
    PLAYER_CARD_INSTANCE(player, card).untap_status &= ~3;
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    result = 1;
    if (((PLAYER_CARD_INSTANCE(player, card).state & STATE_SUMMONSICK_BOTH) != 0) && (global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].type & TYPE_CREATURE) != 0)
    {
      result = 0;
    }
    if ((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) && has_mana(player, 7, 3) == 0)
    {
      result = 0;
    }
    if (((player == active_player) && ((g_duel_network_flags & 2) == 0)) && PLAYER_CARD_INSTANCE(player, card).info_slot != 0)
    {
      result = 0;
    }
    return result;
  }

  if (event == EVENT_ACTIVATE)
  {
    if ((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) != 0)
    {
      if (has_mana(player, 7, 3))
      {
        charge_mana(player, 0, 3);
        if (spell_fizzled != 1)
        {
          PLAYER_CARD_INSTANCE(player, card).eot_toughness = 1;
        }
      }
    }
    else
    {
      ai_modifier -= 0x24;
      undeclare_mana_available_and_produce_it(player, COLOR_COLORLESS, 3);
      produced_mana_color = 0;
      PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
    }
  }

  if ((event == EVENT_RESOLVE_ACTIVATION) && (PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller, PLAYER_CARD_INSTANCE(player, card).parent_card).state & STATE_TAPPED) && PLAYER_CARD_INSTANCE(player, card).eot_toughness != 0)
  {
    unk_007abc78 |= 2;
    PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                         PLAYER_CARD_INSTANCE(player, card).parent_card)
        .info_slot = 1;
    PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                         PLAYER_CARD_INSTANCE(player, card).parent_card)
        .eot_toughness = 0;
  }

  if ((((((trigger_condition == 0xcb) || (trigger_condition == 0xe2)) || (trigger_condition == 0xe3)) || (trigger_condition == 0xe4)) && ((card == affected_card) && (player == affected_card_controller))) && ((current_turn == player) && ((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) != 0) && PLAYER_CARD_INSTANCE(player, card).info_slot != 0))
  {
    if (event == EVENT_TRIGGER)
    {
      event_result |= 2;
    }
    if (event == EVENT_RESOLVE_TRIGGER)
    {
      PLAYER_CARD_INSTANCE(player, card).info_slot = 0;
      PLAYER_CARD_INSTANCE(player, card).state &= ~STATE_TAPPED;
    }
  }

  if (((((event == EVENT_COUNT_MANA) && (card == affected_card)) && (player == affected_card_controller)) && ((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0)) && (((PLAYER_CARD_INSTANCE(player, card).state & STATE_SUMMONSICK_BOTH) == 0) || (global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].type & TYPE_CREATURE) == 0))
  {
    declare_mana_available(player, COLOR_COLORLESS, 3);
  }

  return 0;
}

// FUNCTION: MAGIC 0x00466435
// FUNCTION: SHANDALAR 0x00455b59
int card_copper_tablet(int player, int card, event_t event)
{
  if (event == EVENT_CAST_SPELL && card == affected_card && player == affected_card_controller)
  {
    ai_modifier += ((life[player] - life[1 - player]) * 3) << 3;
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    if ((current_phase == PHASE_UPKEEP) && ((PLAYER_CARD_INSTANCE(player, card).info_slot & 1) == 0) &&
        (unk_00742f60 == human_player) &&
        (((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0) ||
         ((global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].type & TYPE_CREATURE) != 0)))
    {
      PLAYER_CARD_INSTANCE(player, card).upkeep_flags |= 0x101;
      unk_008b3270 |= 3;
      return 1;
    }
    return 0;
  }

  if (event == EVENT_UPKEEP_PHASE && card == affected_card && player == affected_card_controller)
  {
    PLAYER_CARD_INSTANCE(player, card).regen_status |= 1;
    unk_007a7c1c = 1;
    event_result |= 1;
  }
  if (event == EVENT_UPKEEP_COSTS_UNPAID)
  {
    damage_player(human_player, 1, card_on_stack_controller, card_on_stack);
  }
  if (event == EVENT_CLEANUP)
  {
    PLAYER_CARD_INSTANCE(player, card).regen_status &= ~1;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0046667e
// FUNCTION: SHANDALAR 0x00455da6
int card_cyclopean_tomb(int player, int card, event_t event)
{
  card_instance_t *instance;
  card_instance_t *legacy;
  target_t target;
  int chosen_land_type;
  int legacy_card;
  int can_activate;

  instance = &PLAYER_CARD_INSTANCE(player, card);
  chosen_land_type = get_hacked_color(player, card, 1) - 1;

  if (event == EVENT_CAST_SPELL && affected_card == card && affected_card_controller == player)
  {
    TENTATIVE_set_timestamps(player, card);
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    if (current_phase == 4 && player == current_turn && player == human_player && (instance->state & STATE_TAPPED) == 0 && (((instance->untap_status & 3) == 0) || (global_cards_data[instance->internal_card_id].type & TYPE_CREATURE) == 0) && has_mana(player, 7, 2) != 0)
    {
      can_activate = real_target_available((int *)0, TARGET_SCAN_DIRECT, player, 2, 1 - player, 0x200, TYPE_LAND, 0, 0,
                                           get_protections_from(player, card), 0, 0, chosen_land_type,
                                           -1, -1, -1, 0x100, 0, 0);
      if (can_activate != 0)
      {
        if (player == unk_008b35ec || (g_duel_network_flags & 2) != 0)
        {
          return 1;
        }
        if (internal_rand(100) < ((has_mana(player, 7, 1) + 1) / 2) * 0x14)
        {
          unk_008b3270 |= 3;
          return 1;
        }
      }
    }
    return 0;
  }

  if (event == EVENT_ACTIVATE && affected_card == card && affected_card_controller == player && !spell_fizzled)
  {
    if (player == 1 - human_player && (g_duel_network_flags & 2) == 0)
    {
      if (FUN_00466e6d(player, card, 1 - player))
      {
        instance->state |= STATE_TAPPED;
      }
    }
    else
    {
      load_text("promptsX1.txt", "CYCLOPEAN_TOMB");
      if (!C_real_select_target(player, 2, player, TARGET_ZONE_IN_PLAY, TYPE_LAND, TYPE_NONE, 0,
                                get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0,
                                chosen_land_type, ~SUB_WALL, -1, -1,
                                TARGET_SPECIAL_NOT_LAND_SUBTYPE, 0, 0, text_lines[0], 1,
                                &target))
      {
        spell_fizzled = 1;
      }
      else
      {
        instance->targets[0].player = target.player;
        instance->targets[0].card = target.card;
        instance->number_of_targets = 1;
        instance->state |= STATE_TAPPED;
      }
    }
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    if (!C_real_validate_target(instance->targets[0].player, instance->targets[0].card, (char *)0,
                                player, 2, 2, TARGET_ZONE_IN_PLAY, TYPE_LAND, TYPE_NONE, 0,
                                get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0,
                                chosen_land_type, ~SUB_WALL, -1, -1,
                                TARGET_SPECIAL_NOT_LAND_SUBTYPE, 0, 0))
    {
      spell_fizzled = 1;
    }
    else
    {
      legacy_card = create_legacy_effect(affected_card_controller, affected_card, unk_00789108,
                                         instance->targets[0].player, instance->targets[0].card);
      if (legacy_card != -1)
      {
        legacy = &PLAYER_CARD_INSTANCE(player, legacy_card);
        legacy->info_slot = chosen_land_type;
        legacy->token_status = 0x10000;
        legacy->timestamp = instance->timestamp;
        *(int *)((char *)legacy + 0x48) = 1;
      }
    }
    instance->number_of_targets = 0;
  }

  if (event == 0x77 && affected_card == card && affected_card_controller == player)
  {
    legacy_card = create_legacy_effect(player, card, unk_00789108, -1, -1);
    if (legacy_card != -1)
    {
      legacy = &PLAYER_CARD_INSTANCE(player, legacy_card);
      legacy->timestamp = instance->timestamp;
      *(int *)((char *)legacy + 0x48) = 2;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x00466e6d
int FUN_00466e6d(int player, int card, int target_player)
{
  card_instance_t *instance;
  int land_type;
  int preferred_basic;
  int smallest_count;
  int current_type;
  int current_card;
  int found;

  found = 0;
  land_type = get_hacked_color(player, card, 1);
  if (target_player == -1)
  {
    target_player = 1 - player;
  }

  preferred_basic = -1;
  smallest_count = 999;
  for (current_type = 1; current_type < 6; ++current_type)
  {
    if (current_type != land_type && basiclandtypes_controlled[target_player][current_type] != 0 && basiclandtypes_controlled[target_player][current_type] < smallest_count)
    {
      smallest_count = basiclandtypes_controlled[target_player][current_type];
      preferred_basic = current_type - 1;
    }
  }

  for (current_card = 0; current_card < active_cards_count[target_player] && !found; ++current_card)
  {
    instance = &PLAYER_CARD_INSTANCE(target_player, current_card);
    if (is_in_play(target_player, current_card) && (global_cards_data[instance->internal_card_id].type & TYPE_LAND) != 0)
    {
      if (preferred_basic != -1 && global_cards_data[instance->internal_card_id].subtype == preferred_basic)
      {
        preferred_basic = -1;
        PLAYER_CARD_INSTANCE(player, card).targets[0].player = target_player;
        PLAYER_CARD_INSTANCE(player, card).targets[0].card = current_card;
        PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
      }
      else
      {
        PLAYER_CARD_INSTANCE(player, card).targets[0].player = target_player;
        PLAYER_CARD_INSTANCE(player, card).targets[0].card = current_card;
        PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
        found = 1;
      }
    }
  }

  return PLAYER_CARD_INSTANCE(player, card).number_of_targets;
}

// FUNCTION: MAGIC 0x00467167
// FUNCTION: SHANDALAR 0x00456897
int card_gauntlet_of_might(int player, int card, event_t event)
{
  int chosen_color;
  unsigned int affected_color;

  if ((event == EVENT_POWER || event == EVENT_TOUGHNESS) && is_in_play(player, card) && (((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0) || (global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].type & TYPE_CREATURE)))
  {
    chosen_color = get_sleighted_color(player, card, 4);
    affected_color = (unsigned char)PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).color;
    if (((1 << ((unsigned char)chosen_color & 0x1f)) & affected_color) != 0)
    {
      ++event_result;
    }
  }

  if (event == EVENT_TAP_CARD)
  {
    chosen_color = get_hacked_color(player, card, 4);
    if (FUN_0048463d(affected_card_controller, affected_card, chosen_color) != 0 && is_in_play(player, card) && ((((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0) || (global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].type & TYPE_CREATURE)) && produced_mana_color != -1))
    {
      undeclare_mana_available_and_produce_it(affected_card_controller, COLOR_RED, 1);
    }
  }

  if (event == EVENT_COUNT_MANA)
  {
    chosen_color = get_hacked_color(player, card, 4);
    if (FUN_0048463d(affected_card_controller, affected_card, chosen_color) != 0 && is_in_play(player, card) && (((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0) || (global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].type & TYPE_CREATURE)))
    {
      declare_mana_available(affected_card_controller, COLOR_RED, 1);
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x004675a1
// FUNCTION: SHANDALAR 0x00456cd1
int card_icy_manipulator(int player, int card, event_t event)
{
  card_instance_t *instance;
  card_instance_t *parent;
  card_instance_t *target_instance;
  target_t target;
  keyword_t illegal_abilities;

  instance = &PLAYER_CARD_INSTANCE(player, card);
  if ((event == EVENT_CAST_SPELL) && (card == card_on_stack) && (player == card_on_stack_controller))
  {
    ai_modifier += 0xc;
  }
  if (event == EVENT_CAN_ACTIVATE)
  {
    if (has_mana(player, 7, 1) && (((instance->token_status & 3) == 0) || (global_cards_data[instance->internal_card_id].type & TYPE_CREATURE) == 0) && (instance->state & STATE_TAPPED) == 0)
    {
      illegal_abilities = get_protections_from(player, card);
      if (real_target_available((int *)0,
                                TARGET_SCAN_DIRECT,
                                player,
                                2,
                                2,
                                TARGET_ZONE_IN_PLAY,
                                TYPE_ARTIFACT | TYPE_CREATURE | TYPE_LAND,
                                TYPE_NONE,
                                0,
                                illegal_abilities,
                                COLOR_TEST_0,
                                COLOR_TEST_0,
                                -1,
                                -1,
                                0xffffffff,
                                0xffffffff,
                                0,
                                0,
                                0))
      {
        return 1;
      }
    }
  }
  else if (event == 0x90)
  {
    FUN_004e4ff3(0);
  }
  else
  {
    if ((event == EVENT_ACTIVATE) && has_mana(player, 7, 1))
    {
      charge_mana(player, 0, 1);
      if (spell_fizzled != 1)
      {
        load_text("promptsX1.txt", "ICY_MANIPULATOR");
        illegal_abilities = get_protections_from(player, card);
        if (!C_real_select_target(player,
                                  2,
                                  2,
                                  TARGET_ZONE_IN_PLAY,
                                  TYPE_ARTIFACT | TYPE_CREATURE | TYPE_LAND,
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
                                  &target))
        {
          spell_fizzled = 1;
        }
        else
        {
          instance->targets[0] = target;
          instance->number_of_targets = 1;
          if (active_player == player)
          {
            target_instance = &PLAYER_CARD_INSTANCE(target.player, target.card);
            if ((global_cards_data[target_instance->internal_card_id].type & TYPE_LAND) != 0)
            {
              ai_modifier -= 0x18;
            }
            if (target.player == active_player)
            {
              ai_modifier -= 0x60;
            }
          }
          instance->state |= STATE_TAPPED;
        }
      }
    }
    if (event == EVENT_RESOLVE_ACTIVATION)
    {
      target = instance->targets[0];
      illegal_abilities = get_protections_from(player, card);
      if (!C_real_validate_target(target.player,
                                  target.card,
                                  (char *)0,
                                  player,
                                  2,
                                  2,
                                  TARGET_ZONE_IN_PLAY,
                                  TYPE_ARTIFACT | TYPE_CREATURE | TYPE_LAND,
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
                                  0))
      {
        spell_fizzled = 1;
      }
      else
      {
        FUN_004f7783(target.player, target.card);
      }
      parent = &PLAYER_CARD_INSTANCE(instance->parent_controller, instance->parent_card);
      parent->number_of_targets = 0;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x00467a6c
// FUNCTION: SHANDALAR 0x00457199
int card_jade_statue(int player, int card, event_t event)
{
  int animated_internal_card_id;
  card_instance_t *instance;

  instance = &PLAYER_CARD_INSTANCE(player, card);
  if ((event == EVENT_CAST_SPELL) && (card == card_on_stack) && (player == card_on_stack_controller))
  {
    *(int *)((char *)instance + 0x1c) = instance->internal_card_id;
  }
  if (event == EVENT_RESOLVE_SPELL)
  {
    instance->state |= 0x3000000;
    *(int *)((char *)instance + 0x34) = -1;
  }
  if (((event == 0x79) || ((event == 0x78) && (card == card_on_stack) && (player == card_on_stack_controller))) && (instance->state & 4) == 0 && !has_mana(player, 7, 2))
  {
    event_result = 1;
  }
  if ((((trigger_condition == 0xdc) && (current_phase == 0x15) && (current_turn == human_player)) || ((trigger_condition == 0xdd) && (current_phase == 0x17) && (current_turn != human_player))) && card == card_on_stack && player == card_on_stack_controller && instance->info_slot == 0 && (instance->state & STATE_TAPPED) == 0 && trigger_cause_controller == player && trigger_cause == card)
  {
    if (!has_mana(player, 7, 2))
    {
      unk_0078944c = 1;
    }
    else
    {
      if (event == 0x7d)
      {
        event_result |= 2;
      }
      if (event == 0x7e)
      {
        FUN_00443ee2(player, card, EVENT_RESOLVE_ACTIVATION, 0, 0);
        charge_mana(player, 0, 2);
        obliterate_top_card_of_stack();
        if (spell_fizzled == 1)
        {
          unk_0078944c = 1;
          spell_fizzled = 0;
        }
        else
        {
          animated_internal_card_id = create_a_card_type(instance->internal_card_id);
          if (animated_internal_card_id != -1)
          {
            global_cards_data[animated_internal_card_id].type |= TYPE_CREATURE;
            global_cards_data[animated_internal_card_id].power = 3;
            global_cards_data[animated_internal_card_id].toughness = 6;
            *(int *)((char *)instance + 0x1c) = animated_internal_card_id;
            *(int *)((char *)instance + 0x24) |= 0x1000000;
            *(int *)((char *)instance + 0x34) = instance->internal_card_id;
          }
          instance->info_slot = 1;
        }
      }
    }
  }
  if ((event == 0x3c) && (unk_008b4278 & 0x20000) == 0 && card == card_on_stack && player == card_on_stack_controller && is_in_play(player, card))
  {
    event_result = *(int *)((char *)instance + 0x1c);
  }
  if (trigger_condition == 0xcc && card == card_on_stack && player == card_on_stack_controller && current_turn == player && *(int *)((char *)instance + 0x34) != -1)
  {
    if (event == 0x7d)
    {
      event_result |= 2;
    }
    if (event == 0x7e)
    {
      FUN_005001c4(*(int *)((char *)instance + 0x1c));
      *(int *)((char *)instance + 0x1c) = *(int *)((char *)instance + 0x34);
      *(int *)((char *)instance + 0x34) = -1;
      instance->info_slot = 0;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x00468109
// FUNCTION: SHANDALAR 0x0045782f
int card_jalum_tome(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x00468339
// FUNCTION: SHANDALAR 0x00457a60
int card_jandor_s_ring(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x0046867c
// FUNCTION: SHANDALAR 0x00457da1
int card_jeweled_bird(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x00468878
// FUNCTION: SHANDALAR 0x00457f9e
int card_living_wall(int player, int card, event_t event)
{
  if (event == EVENT_CAN_ACTIVATE || event == EVENT_ACTIVATE || event == EVENT_RESOLVE_ACTIVATION)
  {
    return FUN_0054276d(player, card, event, 0, 1);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004688c5
// FUNCTION: SHANDALAR 0x00457feb
int card_mana_crypt(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x00468db5
// FUNCTION: SHANDALAR 0x004584e4
int card_mightstone(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x00468eff
// FUNCTION: SHANDALAR 0x0045862c
int card_obelisk_of_undoing(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x00469386
// FUNCTION: SHANDALAR 0x00458ab3
int card_pyramids(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x00469d09
// FUNCTION: SHANDALAR 0x00459437
int card_rakalite(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x0046a138
// FUNCTION: SHANDALAR 0x00459866
int card_ring_of_ma_r_f(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x0046a4be
// FUNCTION: SHANDALAR 0x00459be7
int card_rocket_launcher(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x0046a845
// FUNCTION: SHANDALAR 0x00459f70
int card_sandals_of_abdallah(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x0046ad1a
// FUNCTION: SHANDALAR 0x0045a443
int card_staff_of_zegon(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x0046b1ab
// FUNCTION: SHANDALAR 0x0045a8d4
int card_su_chi(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x0046b262
// FUNCTION: SHANDALAR 0x0045a98b
int card_tablet_of_epityr(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x0046b7c7
// FUNCTION: SHANDALAR 0x0045aef3
int card_tawnos_s_coffin(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x0046c7dd
// FUNCTION: SHANDALAR 0x0045bf15
int card_urza_s_chalice(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x0046ca6f
// FUNCTION: SHANDALAR 0x0045c1a8
int card_urza_s_miter(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x0046cef9
// FUNCTION: SHANDALAR 0x0045c62c
int card_weakstone(int player, int card, event_t event)
{
}
