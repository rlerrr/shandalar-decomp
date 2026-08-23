#include <stdio.h>
#include <string.h>
#include "../game_support.h"
#include "../global_strings.h"

int cyclone_should_pay_upkeep(int damage_amount);
int choose_lowest_value_creature_with_power(int target_player, int required_power, target_t *target);
int tawnos_coffin_phase_in(int target_player, int target_card);
int tawnos_coffin_phase_out(int target_player, int target_card);

// FUNCTION: MAGIC 0x00435ef0
// FUNCTION: SHANDALAR 0x004f8890
int card_artifact_possession(int player, int card, event_t event)
{
  card_instance_t *instance;
  int target_internal_card_id;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_CAN_CAST)
  {
    return real_target_available((int *)0, TARGET_SCAN_DIRECT, player, 2, 2,
                                 TARGET_ZONE_IN_PLAY, TYPE_ARTIFACT, TYPE_NONE, 0,
                                 get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0,
                                 -1, -1, -1, -1, 0, 0, 0);
  }

  if (event == EVENT_CAST_SPELL && g_affected_card == card && g_affected_card_controller == player)
  {
    load_text("promptsX1.txt", "ARTIFACT_POSSESSION");
    if (!select_target_artifact_and_store(player, 2, card))
    {
      g_spell_fizzled = 1;
    }
    else if (player == g_other_player && g_spell_fizzled != 1)
    {
      if (instance->targets[0].player == g_other_player)
      {
        g_ai_modifier -= 0x90;
      }
      else
      {
        target_internal_card_id = PLAYER_CARD_INSTANCE(instance->targets[0].player, instance->targets[0].card).internal_card_id;
        if ((global_cards_data[target_internal_card_id].type & TYPE_CREATURE) != 0 ||
            (global_cards_data[target_internal_card_id].extra_ability & 0x1001) != 0)
        {
          g_ai_modifier += 0x30;
        }
      }
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    if (!C_real_validate_target(instance->targets[0].player, instance->targets[0].card,
                                (char *)0, player, 2, 2, TARGET_ZONE_IN_PLAY,
                                TYPE_ARTIFACT, TYPE_NONE, 0, get_protections_from(player, card),
                                COLOR_TEST_0, COLOR_TEST_0, -1, -1, -1, -1, 0, 0, 0))
    {
      kill_card(player, card, KILL_BURY);
      g_spell_fizzled = 1;
    }
    else
    {
      instance->damage_target_player = (char)instance->targets[0].player;
      instance->damage_target_card = instance->targets[0].card;
    }
    instance->number_of_targets = 0;
  }

  if ((event == EVENT_TAP_CARD || event == EVENT_UNTAP_CARD) &&
      instance->damage_target_card == g_affected_card &&
      instance->damage_target_player == g_affected_card_controller &&
      g_affected_card != -1 &&
      (instance->state & STATE_INVISIBLE) == 0)
  {
    damage_player((int)instance->damage_target_player, 2, player, card);
  }

  return 0;
}

// FUNCTION: MAGIC 0x0043641d
// FUNCTION: SHANDALAR 0x004f8dbe
int card_artifact_ward(int player, int card, event_t event)
{
  card_instance_t *instance;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_CAN_CAST)
  {
    return real_target_available((int *)0, TARGET_SCAN_DIRECT, player, 2, 2,
                                 TARGET_ZONE_IN_PLAY, TYPE_CREATURE, TYPE_NONE, 0,
                                 get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0,
                                 -1, -1, -1, -1, 0, 0, 0);
  }

  if (event == EVENT_CAST_SPELL && g_affected_card == card && g_affected_card_controller == player)
  {
    load_text("promptsX1.txt", "ARTIFACT_WARD");
    if (!select_target_creature_and_store(player, player, card))
    {
      g_spell_fizzled = 1;
    }
    else
    {
      if (player == g_other_player && (g_duel_network_flags & 2) == 0 &&
          (is_selected_target_already_attached(player, card, PLAYER_CARD_INSTANCE(player, card).internal_card_id) != 0 ||
           (PLAYER_CARD_INSTANCE(instance->targets[0].player, instance->targets[0].card).card_color & COLOR_TEST_ARTIFACT) != 0))
      {
        g_ai_modifier -= 0x60;
      }
      if (instance->targets[0].player == g_other_player)
      {
        g_ai_modifier += C_get_abilities(instance->targets[0].player, instance->targets[0].card, EVENT_POWER, -1) *
                         (g_duel_summary.artifact_counts[0] + 1) * 3;
      }
      if (instance->targets[0].player == g_active_player)
      {
        g_ai_modifier -= 0x60;
      }
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    if (!C_real_validate_target(instance->targets[0].player, instance->targets[0].card,
                                (char *)0, player, 2, 2, TARGET_ZONE_IN_PLAY,
                                TYPE_CREATURE, TYPE_NONE, 0, get_protections_from(player, card),
                                COLOR_TEST_0, COLOR_TEST_0, -1, -1, -1, -1, 0, 0, 0))
    {
      kill_card(player, card, KILL_BURY);
      g_spell_fizzled = 1;
    }
    else
    {
      instance->damage_target_player = (char)instance->targets[0].player;
      instance->damage_target_card = instance->targets[0].card;
    }
    instance->number_of_targets = 0;
  }

  if (event == EVENT_ABILITIES &&
      instance->damage_target_card == g_affected_card &&
      instance->damage_target_player == g_affected_card_controller &&
      g_affected_card != -1 &&
      (instance->state & STATE_INVISIBLE) == 0)
  {
    g_event_result |= 0x10000;
  }

  return 0;
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

  if ((event == EVENT_CAST_SPELL) && (card == g_affected_card) && (player == g_affected_card_controller))
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
                              -1,
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
      instance->number_of_targets = 1;
      if (player == g_other_player && (g_duel_network_flags & 2) == 0)
      {
        if (target.player == g_active_player)
        {
          g_ai_modifier -= 0x30;
        }
        else
        {
          if (is_selected_target_already_attached(player, card, instance->internal_card_id) == 0)
          {
            if ((global_cards_data[PLAYER_CARD_INSTANCE(target.player, target.card).internal_card_id].type & TYPE_CREATURE) != 0)
            {
              g_ai_modifier += 0x60;
            }
          }
          else
          {
            g_ai_modifier -= 0x60;
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
                                -1,
                                -1,
                                -1,
                                0,
                                0,
                                0))
    {
      kill_card(player, card, KILL_BURY);
      g_spell_fizzled = 1;
    }
    else
    {
      instance->damage_target_player = instance->targets[0].player;
      instance->damage_target_card = instance->targets[0].card;
      PLAYER_CARD_INSTANCE(instance->damage_target_player, instance->damage_target_card).token_status |= 0x4000000;
      dispatch_three_arg_callback_to_cards_in_play(destroy_other_auras_on_same_permanent, -1);
    }
    instance->number_of_targets = 0;
  }

  if (event == 0x34 && instance->damage_target_card == g_affected_card && instance->damage_target_player == g_affected_card_controller && g_affected_card != -1)
  {
    g_event_result |= 0x20000;
  }
  if (event == 0x77 && g_affected_card != -1 && PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).kill_code == KILL_DESTROY && instance->damage_target_card == g_affected_card && instance->damage_target_player == g_affected_card_controller)
  {
    PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).kill_code = 0;
    PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).unknown0x14 = 0;
    ++g_event_result;
  }
  if (event == 0x77 && card == g_affected_card && player == g_affected_card_controller && instance->damage_target_player != -1 && instance->damage_target_card != -1 && PLAYER_CARD_INSTANCE(instance->damage_target_player, instance->damage_target_card).internal_card_id != -1)
  {
    PLAYER_CARD_INSTANCE(instance->damage_target_player, instance->damage_target_card).token_status &= ~0x4000000;
  }
  if (g_trigger_condition == 0xd4 && card == g_affected_card && player == g_affected_card_controller && instance->damage_target_player != -1 && instance->damage_target_card != -1 && PLAYER_CARD_INSTANCE(instance->damage_target_player, instance->damage_target_card).internal_card_id != -1 && player == g_trigger_cause_controller && g_trigger_cause == card && player == g_current_turn)
  {
    if (event == 0x7d)
    {
      g_event_result |= 2;
    }
    if (event == 0x7e)
    {
      PLAYER_CARD_INSTANCE(instance->damage_target_player, instance->damage_target_card).token_status &= ~0x4000000;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x00437375
int destroy_other_auras_on_same_permanent(int player, int card, int internal_card_id)
{
  if ((int)(char)PLAYER_CARD_INSTANCE(player, card).damage_target_player ==
      (int)(char)PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).damage_target_player)
  {
    if (PLAYER_CARD_INSTANCE(player, card).damage_target_card ==
        PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).damage_target_card)
    {
      if (((unsigned char)global_cards_data[internal_card_id].type & TYPE_ENCHANTMENT) != 0)
      {
        if (card != g_affected_card || player != g_affected_card_controller)
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
  struct
  {
    char prompt[900];
    int paid;
    int saved_player;
    int ai_choice;
    int current_player;
    int current_card;
    unsigned int counters;
    int saved_card;
  } s;

  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if (event == EVENT_CAST_SPELL)
  {
    g_ai_modifier += ((g_life[player] - g_life[1 - player]) * 3 + 6) * 8;
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    if (g_current_phase == PHASE_UPKEEP &&
        (PLAYER_CARD_INSTANCE(player, card).info_slot & 1) == 0 &&
        player == unk_00742f60 &&
        player == g_current_player)
    {
      PLAYER_CARD_INSTANCE(player, card).upkeep_flags |= 0x101;
      unk_008b3270 |= 3;
      return 1;
    }
    return 0;
  }

  if (event == EVENT_UPKEEP_PHASE && card == g_affected_card && player == g_affected_card_controller)
  {
    PLAYER_CARD_INSTANCE(player, card).info_slot |= 1;
    unk_007a7c1c = 1;
    g_event_result |= 1;
    add_special_counter(player, card);
  }

  if (event == EVENT_UPKEEP_COSTS_UNPAID)
  {
    s.counters = C_get_special_counters(player, card);
    if ((g_duel_network_flags & 2) == 0)
    {
      s.ai_choice = cyclone_should_pay_upkeep(s.counters);
    }

    s.paid = 0;
    load_text("promptsX1.txt", "CYCLONE");
    sprintf(s.prompt, "%s\n %s\n %s\n", g_text_lines[0], g_text_lines[1], g_text_lines[2]);
    if (has_mana(player, COLOR_GREEN, s.counters) != 0 &&
        do_dialog(player, player, card, -1, -1, s.prompt, s.ai_choice) != 0)
    {
      s.saved_player = g_card_on_stack_controller;
      s.saved_card = g_card_on_stack;
      push_card_onto_stack(g_card_on_stack_controller, g_card_on_stack, EVENT_RESOLVE_TRIGGER, 0, 0);
      charge_mana(player, COLOR_GREEN, s.counters);
      obliterate_top_card_of_stack();
      g_card_on_stack_controller = s.saved_player;
      g_card_on_stack = s.saved_card;
      if (g_spell_fizzled != 1)
      {
        s.paid = 1;
        s.current_player = player;
        while (s.current_player >= 0 && s.current_player < 2)
        {
          damage_player(s.current_player, s.counters, g_card_on_stack_controller, g_card_on_stack);
          for (s.current_card = 0; s.current_card < g_active_cards_count[s.current_player]; ++s.current_card)
          {
            if (is_in_play(s.current_player, s.current_card) != 0 &&
                (global_cards_data[PLAYER_CARD_INSTANCE(s.current_player, s.current_card).internal_card_id].type & TYPE_CREATURE) != 0)
            {
              damage_creature(s.current_player, s.current_card, s.counters, g_card_on_stack_controller, g_card_on_stack);
            }
          }

          if (player == 0)
          {
            ++s.current_player;
          }
          else
          {
            --s.current_player;
          }
        }
      }
    }

    if (s.paid == 0)
    {
      kill_card(g_card_on_stack_controller, g_card_on_stack, KILL_BURY);
    }
  }

  if (event == EVENT_CLEANUP)
  {
    PLAYER_CARD_INSTANCE(player, card).info_slot &= ~1;
  }

  if (event == EVENT_SHOULD_AI_PLAY &&
      has_mana(player, COLOR_GREEN, 1) < (int)C_get_special_counters(player, card))
  {
    kill_card(player, card, KILL_DESTROY);
  }

  return 0;
}

// FUNCTION: SHANDALAR 0x004fa29a
int cyclone_should_pay_upkeep(int damage_amount)
{
  int player_totals[4];
  int current_player;
  int current_card;
  int power;
  int result;

  result = 0;
  for (current_player = 0; current_player < 2; ++current_player)
  {
    player_totals[current_player] = 0;
    player_totals[current_player + 2] = 0;
    for (current_card = 0; current_card < g_active_cards_count[current_player]; ++current_card)
    {
      if (is_in_play(current_player, current_card) != 0 &&
          (global_cards_data[PLAYER_CARD_INSTANCE(current_player, current_card).internal_card_id].type & TYPE_CREATURE) != 0)
      {
        power = PLAYER_CARD_INSTANCE(current_player, current_card).power;
        player_totals[current_player + 2] += power;
        if (damage_amount < power ||
            (PLAYER_CARD_INSTANCE(current_player, current_card).card_color & COLOR_TEST_RED) != 0)
        {
          player_totals[current_player] += power;
        }
      }
    }
  }

  if (damage_amount < g_life[g_active_player])
  {
    if (damage_amount < g_life[g_other_player] &&
        (player_totals[g_active_player + 2] - player_totals[g_active_player]) -
            (player_totals[g_other_player + 2] - player_totals[g_other_player]) >= 0)
    {
      result = 1;
    }
  }
  else if (g_life[g_other_player] <= damage_amount ||
           player_totals[g_other_player + 2] < player_totals[g_active_player + 2])
  {
    result = 1;
  }

  return result;
}

// FUNCTION: MAGIC 0x00437aed
// FUNCTION: SHANDALAR 0x004fa491
int card_damping_field(int player, int card, event_t event)
{
  target_t target;
  int current_card;

  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if (event == EVENT_CAST_SPELL && g_affected_card == card && g_affected_card_controller == player &&
      count_permanents_by_internal_card_id(player, PLAYER_CARD_INSTANCE(player, card).internal_card_id, -1) == 0)
  {
    g_ai_modifier += (g_duel_summary.artifact_counts[1 - player] - g_duel_summary.artifact_counts[player]) * 0xc;
  }

  if (event == EVENT_UNTAP &&
      (global_cards_data[PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).internal_card_id].type & TYPE_ARTIFACT) != 0)
  {
    PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).untap_status &= ~UNTAP_STATUS_WILL_UNTAP;
  }

  if (g_current_phase == EVENT_UNTAP_PHASE && g_affected_card == card && g_affected_card_controller == player)
  {
    if (event == EVENT_TRIGGER &&
        real_target_available((int *)0, TARGET_SCAN_DIRECT, g_current_player, g_current_player, g_current_player,
                              TARGET_ZONE_IN_PLAY, TYPE_ARTIFACT, TYPE_NONE, 0, 0,
                              COLOR_TEST_0, COLOR_TEST_0, -1, -1, -1, -1, 0,
                              TARGET_STATE_WILL_UNTAP, 0) == 0 &&
        real_target_available((int *)0, TARGET_SCAN_DIRECT, g_current_player, g_current_player, g_current_player,
                              TARGET_ZONE_IN_PLAY, TYPE_ARTIFACT, TYPE_NONE, 0, 0,
                              COLOR_TEST_0, COLOR_TEST_0, -1, -1, -1, -1, 0,
                              TARGET_STATE_COULD_UNTAP, 0) != 0)
    {
      g_event_result |= RESOLVE_TRIGGER_MANDATORY;
    }

    if (event == EVENT_RESOLVE_TRIGGER)
    {
      if (g_other_player == g_current_player && (g_duel_network_flags & 2) == 0)
      {
        target.player = g_current_player;
        target.card = choose_best_tapped_permanent_for_ai(g_current_player, TYPE_ARTIFACT);
        load_text("promptsX1.txt", "DAMPING_FIELD");
        do_dialog(player, player, card, target.player, target.card, g_text_lines[1], 0);
      }
      else
      {
        load_text("promptsX1.txt", "DAMPING_FIELD");
        C_real_select_target(g_current_player, g_current_player, g_current_player,
                             TARGET_ZONE_IN_PLAY, TYPE_ARTIFACT, TYPE_NONE, 0, 0,
                             COLOR_TEST_0, COLOR_TEST_0, -1, -1, -1, -1, 0,
                             TARGET_STATE_COULD_UNTAP | TARGET_STATE_TAPPED, 0,
                             g_text_lines[0], 0, &target);
      }

      PLAYER_CARD_INSTANCE(target.player, target.card).untap_status |= UNTAP_STATUS_WILL_UNTAP;
      for (current_card = 0; current_card < g_active_cards_count[g_current_player]; ++current_card)
      {
        if (is_in_play(g_current_player, current_card) != 0 &&
            (PLAYER_CARD_INSTANCE(g_current_player, current_card).state & STATE_TAPPED) != 0 &&
            (global_cards_data[PLAYER_CARD_INSTANCE(g_current_player, current_card).internal_card_id].type & TYPE_ARTIFACT) != 0 &&
            (PLAYER_CARD_INSTANCE(g_current_player, current_card).untap_status & UNTAP_STATUS_WILL_UNTAP) == 0)
        {
          PLAYER_CARD_INSTANCE(g_current_player, current_card).untap_status &= ~UNTAP_STATUS_COULD_UNTAP;
        }
      }
    }
  }

  if (event == EVENT_CLEANUP)
  {
    PLAYER_CARD_INSTANCE(player, card).info_slot = 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x00437f75
// FUNCTION: SHANDALAR 0x004fa91d
int card_drop_of_honey(int player, int card, event_t event)
{
  struct
  {
    target_t target;
    char prompt[900];
    int min_power;
    int min_count;
    int current_player;
    int current_card;
    int max_cards;
    int found_creature;
    int player_creatures;
    int opponent_creatures;
    int player_creature_count;
    int opponent_creature_count;
    int power;
  } s;

  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if (event == EVENT_CAST_SPELL && card == g_affected_card && player == g_affected_card_controller)
  {
    s.player_creatures = g_creature_power_by_color[player][7];
    s.player_creature_count = ClampIntToRange(g_duel_summary.creature_counts[player], 1, 99);
    s.opponent_creatures = g_creature_power_by_color[1 - player][7];
    s.opponent_creature_count = ClampIntToRange(g_duel_summary.creature_counts[1 - player], 1, 99);
    g_ai_modifier += (s.player_creatures * 0xc) / s.player_creature_count -
                     (s.opponent_creatures * 0xc) / s.opponent_creature_count;
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    if (g_current_phase == EVENT_UPKEEP_PHASE &&
        g_current_player == unk_00742f60 &&
        player == g_current_player &&
        (PLAYER_CARD_INSTANCE(player, card).info_slot & 1) == 0 &&
        real_target_available((int *)0, TARGET_SCAN_DIRECT, player, 2, 2,
                              TARGET_ZONE_IN_PLAY, TYPE_CREATURE, TYPE_NONE, 0,
                              get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0,
                              -1, -1, -1, -1, 0, 0, 0) != 0)
    {
      PLAYER_CARD_INSTANCE(player, card).upkeep_flags |= 0x101;
      unk_008b3270 |= 3;
      return 1;
    }
    return 0;
  }

  if (event == EVENT_UPKEEP_PHASE && card == g_affected_card && player == g_affected_card_controller)
  {
    PLAYER_CARD_INSTANCE(player, card).info_slot |= 1;
    unk_007a7c1c = 1;
    g_event_result |= 1;
  }

  if (event == EVENT_UPKEEP_COSTS_UNPAID)
  {
    s.min_power = CARD_ID_CONCENTRATE;
    s.min_count = 0;
    s.target.player = -1;
    s.target.card = -1;

    for (s.current_player = 0; s.current_player < 2; ++s.current_player)
    {
      for (s.current_card = 0; s.current_card < g_active_cards_count[s.current_player]; ++s.current_card)
      {
        if (is_in_play(s.current_player, s.current_card) != 0 &&
            (global_cards_data[PLAYER_CARD_INSTANCE(s.current_player, s.current_card).internal_card_id].type & TYPE_CREATURE) != 0 &&
            (PLAYER_CARD_INSTANCE(s.current_player, s.current_card).regen_status & get_protections_from(player, card)) == 0)
        {
          s.power = C_get_abilities(s.current_player, s.current_card, EVENT_POWER, -1);
          if (s.power < s.min_power)
          {
            s.target.player = s.current_player;
            s.target.card = s.current_card;
            s.min_count = 0;
            s.min_power = s.power;
          }
          if (s.power == s.min_power)
          {
            ++s.min_count;
          }
        }
      }
    }

    if (s.min_count == 1)
    {
      kill_card(s.target.player, s.target.card, KILL_BURY);
    }
    if (s.min_count > 1)
    {
      if (player == g_active_player || (g_duel_network_flags & 2) != 0)
      {
        load_text("promptsX1.txt", "DROP_OF_HONEY");
        sprintf(s.prompt, "%s %d.", g_text_lines[0], s.min_power);
        C_real_select_target(player, 2, 2, TARGET_ZONE_IN_PLAY, TYPE_CREATURE, TYPE_NONE,
                             0, 0, COLOR_TEST_0, COLOR_TEST_0, -1, -1, s.min_power, -1,
                             0, 0, 0, s.prompt, 0, &s.target);
      }
      else
      {
        choose_lowest_value_creature_with_power(-1, s.min_power, &s.target);
      }
      kill_card(s.target.player, s.target.card, KILL_BURY);
    }
  }

  if (event == EVENT_CLEANUP)
  {
    PLAYER_CARD_INSTANCE(player, card).info_slot &= ~1;
  }

  if (event != EVENT_CHANGE_TYPE && card == g_affected_card && player == g_affected_card_controller)
  {
    if (g_active_cards_count[g_other_player] <= g_active_cards_count[g_active_player])
    {
      s.max_cards = g_active_cards_count[g_active_player];
    }
    else
    {
      s.max_cards = g_active_cards_count[g_other_player];
    }

    s.current_card = 0;
    s.found_creature = 0;
    while (s.current_card < s.max_cards && s.found_creature == 0)
    {
      if ((is_in_play(g_active_player, s.current_card) != 0 &&
           (global_cards_data[PLAYER_CARD_INSTANCE(g_active_player, s.current_card).internal_card_id].type & TYPE_CREATURE) != 0) ||
          (is_in_play(g_other_player, s.current_card) != 0 &&
           (global_cards_data[PLAYER_CARD_INSTANCE(g_other_player, s.current_card).internal_card_id].type & TYPE_CREATURE) != 0))
      {
        s.found_creature = 1;
      }
      ++s.current_card;
    }
    if (s.found_creature == 0)
    {
      kill_card(player, card, KILL_BURY);
    }
  }

  return 0;
}

// FUNCTION: SHANDALAR 0x00487b4e
int choose_lowest_value_creature_with_power(int target_player, int required_power, target_t *target)
{
  int found;
  int best_score;
  int current_player;
  int current_card;
  int power;
  int toughness;
  int score;

  found = 0;
  target->player = -1;
  target->card = -1;
  best_score = 0x7fff;

  for (current_player = 0; current_player < 2; ++current_player)
  {
    if (target_player == -1 || current_player == target_player)
    {
      for (current_card = 0; current_card < g_active_cards_count[current_player]; ++current_card)
      {
        if (is_in_play(current_player, current_card) != 0 &&
            (global_cards_data[PLAYER_CARD_INSTANCE(current_player, current_card).internal_card_id].type & TYPE_CREATURE) != 0 &&
            (PLAYER_CARD_INSTANCE(current_player, current_card).state & STATE_CANNOT_TARGET) == 0)
        {
          power = C_get_abilities(current_player, current_card, EVENT_POWER, -1);
          if (power == required_power)
          {
            toughness = C_get_abilities(current_player, current_card, EVENT_TOUGHNESS, -1);
            score = (toughness + 2) * (power + CARD_ID_ANIMATE_ARTIFACT);
            if (score < best_score)
            {
              target->player = current_player;
              target->card = current_card;
              found = 1;
              best_score = score;
            }
          }
        }
      }
    }
  }

  return found;
}

// FUNCTION: MAGIC 0x004385c3
// FUNCTION: SHANDALAR 0x004faf62
int card_earthbind(int player, int card, event_t event)
{
  target_t selected_target;
  unsigned int illegal_abilities;

  if (event == EVENT_CAN_CAST)
  {
    load_recorded_action_target(0);
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
    if (((event == EVENT_CAST_SPELL) && (card == g_affected_card)) && (player == g_affected_card_controller))
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
                               -1,
                               -1,
                               -1,
                               0,
                               0,
                               0,
                               g_text_lines[0],
                               1,
                               &selected_target) == 0)
      {
        g_spell_fizzled = 1;
      }
      else
      {
        PLAYER_CARD_INSTANCE(player, card).targets[PLAYER_CARD_INSTANCE(player, card).number_of_targets] =
            selected_target;
        ++PLAYER_CARD_INSTANCE(player, card).number_of_targets;
        if ((PLAYER_CARD_INSTANCE(selected_target.player, selected_target.card).regen_status & KEYWORD_FLYING) == 0)
        {
          g_ai_modifier += -0x60;
        }
        else
        {
          g_ai_modifier += 0x18;
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
                                 -1,
                                 -1,
                                 -1,
                                 0,
                                 0,
                                 0) == 0)
      {
        kill_card(player, card, KILL_BURY);
        g_spell_fizzled = 1;
      }
      else
      {
        PLAYER_CARD_INSTANCE(player, card).damage_target_player =
            (char)PLAYER_CARD_INSTANCE(player, card).targets[0].player;
        PLAYER_CARD_INSTANCE(player, card).damage_target_card =
            PLAYER_CARD_INSTANCE(player, card).targets[0].card;
        if ((PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                                  PLAYER_CARD_INSTANCE(player, card).targets[0].card)
                 .regen_status &
             KEYWORD_FLYING) != 0)
        {
          damage_creature(PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                          PLAYER_CARD_INSTANCE(player, card).damage_target_card,
                          2,
                          player,
                          card);
        }
      }
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    }
    if ((((event == EVENT_ABILITIES) && (PLAYER_CARD_INSTANCE(player, card).damage_target_card == g_affected_card)) && ((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player == g_affected_card_controller)) && (g_affected_card != -1))
    {
      g_event_result &= 0xffffffdf;
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
    if (event == EVENT_CAST_SPELL && g_affected_card == card && player == g_affected_card_controller)
    {
      load_text("promptsX1.txt", "FARMSTEAD");
      if (!select_target_land_and_store(player, player, card))
      {
        g_spell_fizzled = 1;
      }
      if (g_spell_fizzled != 1)
      {
        if (instance->targets[0].player == g_active_player)
        {
          g_ai_modifier -= 0x60;
        }
        if (instance->targets[0].player == g_other_player)
        {
          g_ai_modifier += 0x30;
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
        g_spell_fizzled = 1;
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
      if (g_current_phase == 4 && instance->damage_target_player == g_current_player && instance->info_slot == 0 && has_mana_w_global_cost_mod(player, card, 5, 2) != 0)
      {
        if (player == g_other_player && (g_duel_network_flags & 2) == 0 && (internal_rand(100) < ((g_basiclandtypes_controlled[player][COLOR_WHITE] + 1) / 2) * 0x14 || g_life[g_other_player] < 5))
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
        if (g_spell_fizzled == 1)
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

  if (player == g_current_player && (g_land_can_be_played & 1) != 0 && g_current_phase > 0x13 && g_current_phase < 0x1f)
  {
    g_land_can_be_played &= ~1;
  }

  if ((g_trigger_condition == 0xdb || g_trigger_condition == 0xd3) && g_affected_card == card && g_affected_card_controller == player && g_duel_summary.land_entries > 0 && g_current_turn == player && g_trigger_cause_controller == player && PLAYER_CARD_INSTANCE(g_trigger_cause_controller, g_trigger_cause).internal_card_id != -1 && (global_cards_data[PLAYER_CARD_INSTANCE(g_trigger_cause_controller, g_trigger_cause).internal_card_id].type & TYPE_LAND) != 0 && (PLAYER_CARD_INSTANCE(player, card).state & 0x20) == 0)
  {
    if (event == 0x7d)
    {
      g_event_result |= 2;
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
  int hacked_color;
  int result;

  if (event == EVENT_CAST_SPELL && card == g_affected_card && player == g_affected_card_controller)
  {
    load_text("promptsX1.txt", "FISHLIVEROIL");
  }

  hacked_color = get_hacked_color(player, card, COLOR_BLUE);
  result = generic_creature_ability_aura(player, card, event, 1 << ((unsigned char)(hacked_color - 1) & 0x1f));

  if (event == EVENT_CAST_SPELL && card == g_affected_card && player == g_affected_card_controller &&
      player == g_other_player)
  {
    hacked_color = get_hacked_color(player, card, COLOR_BLUE);
    if ((PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                              PLAYER_CARD_INSTANCE(player, card).targets[0].card)
             .regen_status &
         (1 << ((unsigned char)(hacked_color - 1) & 0x1f))) != 0 ||
        PLAYER_CARD_INSTANCE(player, card).targets[0].player == g_active_player)
    {
      g_ai_modifier -= 0x60;
    }
  }

  return result;
}

// FUNCTION: MAGIC 0x00439551
// FUNCTION: SHANDALAR 0x004fbeef
int card_gate_to_phyrexia(int player, int card, event_t event)
{
  card_instance_t *instance;
  card_instance_t *parent;
  target_t creature;
  target_t artifact;
  int current_card;
  int current_internal_card_id;
  int current_score;
  int best_creature_score;
  int best_artifact_score;
  int artifact_player;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if (event == EVENT_CAST_SPELL && g_affected_card == card && g_affected_card_controller == player &&
      player == g_other_player)
  {
    g_ai_modifier += 0x18;
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    if (g_current_phase == EVENT_UPKEEP_PHASE && g_current_player == player &&
        real_target_available((int *)0, TARGET_SCAN_DIRECT, player, player, player,
                              TARGET_ZONE_IN_PLAY, TYPE_CREATURE, TYPE_NONE, 0, 0,
                              COLOR_TEST_0, COLOR_TEST_0, -1, -1, -1, -1, 0, 0, 0) != 0 &&
        real_target_available((int *)0, TARGET_SCAN_DIRECT, player, 2, 2,
                              TARGET_ZONE_IN_PLAY, TYPE_ARTIFACT, TYPE_NONE, 0,
                              get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0,
                              -1, -1, -1, -1, 0, 0, 0) != 0)
    {
      if (player == g_other_player && (g_duel_network_flags & 2) == 0)
      {
        artifact_player = 1 - player;
        best_creature_score = 999;
        best_artifact_score = -1;
        creature.player = player;
        creature.card = -1;
        artifact.player = artifact_player;
        artifact.card = -1;

        for (current_card = 0; current_card < g_active_cards_count[player]; ++current_card)
        {
          current_internal_card_id = PLAYER_CARD_INSTANCE(player, current_card).internal_card_id;
          if (is_in_play(player, current_card) != 0 &&
              (global_cards_data[current_internal_card_id].type & TYPE_CREATURE) != 0)
          {
            current_score = PLAYER_CARD_INSTANCE(player, current_card).power +
                            global_cards_data[current_internal_card_id].cc[0] +
                            global_cards_data[current_internal_card_id].cc[1];
            if (current_score < best_creature_score)
            {
              creature.card = current_card;
              best_creature_score = current_score;
            }
          }
        }

        for (current_card = 0; current_card < g_active_cards_count[artifact_player]; ++current_card)
        {
          current_internal_card_id = PLAYER_CARD_INSTANCE(artifact_player, current_card).internal_card_id;
          if (is_in_play(artifact_player, current_card) != 0 &&
              (global_cards_data[current_internal_card_id].type & TYPE_ARTIFACT) != 0)
          {
            current_score = PLAYER_CARD_INSTANCE(artifact_player, current_card).power +
                            global_cards_data[current_internal_card_id].cc[0] +
                            global_cards_data[current_internal_card_id].cc[1] +
                            C_get_special_counters(artifact_player, current_card);
            if ((global_cards_data[current_internal_card_id].extra_ability & 0x1000) != 0)
            {
              current_score += 4 - g_basiclandtypes_controlled[artifact_player][7];
            }
            if (best_artifact_score < current_score)
            {
              artifact.card = current_card;
              best_artifact_score = current_score;
            }
          }
        }

        if (creature.card != -1 && artifact.card != -1 && best_creature_score < best_artifact_score)
        {
          unk_008b3270 |= 3;
          instance->targets[0] = artifact;
          instance->targets[1] = creature;
        }
      }
      return 1;
    }
    return 0;
  }

  if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_target(0);
    return 0;
  }

  if (event == EVENT_ACTIVATE)
  {
    if (g_active_player == player || (g_duel_network_flags & 2) != 0)
    {
      load_text("promptsX1.txt", "GATE_TO_PHYREXIA");
      if (!C_real_select_target(player, player, player, TARGET_ZONE_IN_PLAY,
                                TYPE_CREATURE, TYPE_NONE, 0, 0,
                                COLOR_TEST_0, COLOR_TEST_0, -1, -1, -1, -1,
                                0, 0, 0, g_text_lines[0], 1, &creature))
      {
        g_spell_fizzled = 1;
      }
      else
      {
        load_text("promptsX1.txt", "GATE_TO_PHYREXIA");
        if (!C_real_select_target(player, 2, 2, TARGET_ZONE_IN_PLAY,
                                  TYPE_ARTIFACT, TYPE_NONE, 0,
                                  get_protections_from(player, card),
                                  COLOR_TEST_0, COLOR_TEST_0, -1, -1, -1, -1,
                                  0, 0, 0, g_text_lines[1], 1, &artifact))
        {
          g_spell_fizzled = 1;
        }
        else
        {
          if (g_duel_ai_mode_state != 1)
          {
            play_sound_effect(0xf);
          }
          kill_card(creature.player, creature.card, KILL_SACRIFICE);
          instance->targets[0] = artifact;
          instance->number_of_targets = 1;
        }
      }
    }
    else
    {
      if (PLAYER_CARD_INSTANCE(instance->targets[1].player, instance->targets[1].card).internal_card_id == -1)
      {
        g_spell_fizzled = 1;
      }
      else
      {
        kill_card(instance->targets[1].player, instance->targets[1].card, KILL_SACRIFICE);
        instance->number_of_targets = 1;
      }
    }
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    if (!C_real_validate_target(instance->targets[0].player, instance->targets[0].card,
                                (char *)0, player, 2, 2, TARGET_ZONE_IN_PLAY,
                                TYPE_ARTIFACT, TYPE_NONE, 0, get_protections_from(player, card),
                                COLOR_TEST_0, COLOR_TEST_0, -1, -1, -1, -1, 0, 0, 0))
    {
      g_spell_fizzled = 1;
    }
    else
    {
      kill_card(instance->targets[0].player, instance->targets[0].card, KILL_DESTROY);
    }
    parent = &PLAYER_CARD_INSTANCE(instance->parent_controller, instance->parent_card);
    parent->number_of_targets = 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x00439e81
// FUNCTION: SHANDALAR 0x004fc825
int card_haunting_wind(int player, int card, event_t event)
{
  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if ((event == EVENT_TAP_CARD || event == EVENT_PLAY_ABILITY) &&
      (global_cards_data[PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).internal_card_id].type &
       TYPE_ARTIFACT) != 0 &&
      (PLAYER_CARD_INSTANCE(player, card).state & STATE_INVISIBLE) == 0)
  {
    damage_player(g_affected_card_controller, 1, player, card);
  }

  return 0;
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

  if (event == EVENT_CAST_SPELL && card == g_affected_card && player == g_affected_card_controller)
  {
    load_text("promptsX1.txt", "INVISIBILITY");
    g_spell_fizzled = !select_target_creature_and_store(player, player, card);
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
                                -1,
                                -1,
                                -1,
                                0,
                                0,
                                0))
    {
      kill_card(player, card, KILL_BURY);
      g_spell_fizzled = 1;
    }
    else
    {
      instance->damage_target_player = instance->targets[0].player;
      instance->damage_target_card = instance->targets[0].card;
    }
    instance->number_of_targets = 0;
  }

  if (event == EVENT_BLOCK_LEGALITY &&
      instance->damage_target_card == g_attacking_card &&
      instance->damage_target_player == g_attacking_card_controller &&
      (instance->state & 0x20) == 0 &&
      global_cards_data[PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).internal_card_id].subtype != 0)
  {
    ++g_event_result;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0043a2e0
// FUNCTION: SHANDALAR 0x004fcc82
int card_jihad(int player, int card, event_t event)
{
  card_instance_t *instance;
  int chosen_color;
  int ai_choice;
  int best_score;
  int current_color;
  int opponent;
  int current_card;
  int found;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if (event == EVENT_CAST_SPELL && g_affected_card == card && g_affected_card_controller == player)
  {
    if (player == g_other_player && (g_duel_network_flags & 2) == 0)
    {
      ai_choice = COLOR_BLACK;
      best_score = -1;
      for (current_color = COLOR_BLACK; current_color <= COLOR_WHITE; ++current_color)
      {
        if (best_score < g_basiclandtypes_controlled[1 - player][current_color] +
                         g_creature_power_by_color[1 - player][current_color])
        {
          best_score = g_basiclandtypes_controlled[1 - player][current_color] +
                       g_basiclandtypes_controlled[1 - player][current_color];
          ai_choice = current_color;
        }
      }
    }
    else
    {
      ai_choice = internal_rand(5) + 1;
    }

    load_text("promptsX1.txt", "JIHAD");
    if (player == g_other_player && (g_duel_network_flags & 2) == 0)
    {
      chosen_color = choose_a_color(player, g_text_lines[0], 1, ai_choice, 0x3e);
    }
    else
    {
      chosen_color = choose_a_color(player, g_text_lines[0], 1, -1, 0x3e);
    }
    instance->info_slot = chosen_color;
    if (instance->info_slot == -1)
    {
      g_spell_fizzled = 1;
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    load_text("promptsX1.txt", "JIHAD");
    do_dialog(player, player, card, -1, -1, g_text_lines[instance->info_slot], 0);
  }

  if ((event == EVENT_POWER || event == EVENT_TOUGHNESS) &&
      is_in_play(player, card) != 0 &&
      is_in_play(g_affected_card_controller, g_affected_card) != 0 &&
      ((1 << ((unsigned char)get_sleighted_color(player, card, COLOR_WHITE) & 0x1f)) &
       (int)(char)PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).color) != 0)
  {
    if (event == EVENT_POWER)
    {
      g_event_result += 2;
    }
    else
    {
      g_event_result += 1;
    }
  }

  if (event != EVENT_CHANGE_TYPE && instance->info_slot != 0 &&
      g_affected_card == card && g_affected_card_controller == player && g_spell_fizzled != 1)
  {
    opponent = 1 - player;
    chosen_color = instance->info_slot;
    found = 0;
    current_card = 0;
    while (current_card < g_active_cards_count[opponent] && found == 0)
    {
      if (is_in_play(opponent, current_card) != 0 &&
          (global_cards_data[PLAYER_CARD_INSTANCE(opponent, current_card).internal_card_id].type &
           (TYPE_LAND | TYPE_CREATURE | TYPE_ENCHANTMENT | TYPE_ARTIFACT)) != 0 &&
          ((1 << ((unsigned char)chosen_color & 0x1f)) &
           (int)(char)PLAYER_CARD_INSTANCE(opponent, current_card).color) != 0)
      {
        found = 1;
      }
      ++current_card;
    }
    if (found == 0)
    {
      instance->info_slot = 0;
      kill_card(player, card, KILL_BURY);
    }
  }

  return 0;
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
                                 -1,
                                 0,
                                 0,
                                 0);
  }

  if (event == EVENT_CAST_SPELL && card == g_affected_card && player == g_affected_card_controller)
  {
    load_text("promptsX1.txt", "KUDZU");
    if (!select_target_land_and_store(player, 1 - player, card))
    {
      g_spell_fizzled = 1;
    }
    else if (instance->targets[0].player == player)
    {
      g_ai_modifier -= 0x18;
    }
    else
    {
      g_ai_modifier += (g_basiclandtypes_controlled[1 - player][7] - g_basiclandtypes_controlled[player][7]) * 0xc;
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
                                -1,
                                -1,
                                -1,
                                0,
                                0,
                                0))
    {
      kill_card(player, card, KILL_BURY);
      g_spell_fizzled = 1;
    }
    else
    {
      instance->damage_target_player = (char)instance->targets[0].player;
      instance->damage_target_card = instance->targets[0].card;
    }
    instance->number_of_targets = 0;
    return 0;
  }

  if (event == 0x81 && instance->damage_target_card == g_affected_card && instance->damage_target_player == g_affected_card_controller && g_affected_card != -1 && (instance->state & 0x20) == 0)
  {
    card_instance_t *original_kudzu;
    int valid_targets;

    real_target_available(&valid_targets,
                          TARGET_SCAN_DIRECT,
                          0,
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
                          -1,
                          -1,
                          0,
                          0,
                          0);
    if (valid_targets < 2)
    {
      kill_card(0, 0, KILL_DESTROY);
    }
    else
    {
      load_text("promptsX1.txt", "KUDZU");
      original_kudzu = &PLAYER_CARD_INSTANCE(0, 0);
      PLAYER_CARD_INSTANCE(original_kudzu->damage_target_player, original_kudzu->damage_target_card).state |= 0x300000;

      if (original_kudzu->damage_target_player == g_active_player || (g_duel_network_flags & 2) != 0)
      {
        if (C_real_select_target(original_kudzu->damage_target_player,
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
                                 -1,
                                 -1,
                                 -1,
                                 0,
                                 0,
                                 0,
                                 g_text_lines[1],
                                 0,
                                 &new_target))
        {
          original_kudzu->damage_target_player = (char)new_target.player;
          original_kudzu->damage_target_card = new_target.card;
        }
      }
      else if (select_best_land_target_by_score(original_kudzu->damage_target_player, 1 - original_kudzu->damage_target_player, (int *)&new_target))
      {
        do_dialog(0, 0, 0, new_target.player, new_target.card, g_text_lines[2], 0);
        g_ai_modifier +=
            (g_basiclandtypes_controlled[g_other_player][7] - g_basiclandtypes_controlled[g_active_player][7]) * 0x18;
        original_kudzu->damage_target_player = (char)new_target.player;
        original_kudzu->damage_target_card = new_target.card;
      }
    }
    kill_card(g_affected_card_controller, g_affected_card, KILL_DESTROY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x0043aee2
// FUNCTION: SHANDALAR 0x004fd888
int card_lance(int player, int card, event_t event)
{
  if ((event == EVENT_CAST_SPELL) && (card == g_affected_card) && (player == g_affected_card_controller))
  {
    load_text("promptsX1.txt", "LANCE");
  }

  return generic_creature_ability_aura(player, card, event, 0x100);
}

// FUNCTION: MAGIC 0x0043af43
// FUNCTION: SHANDALAR 0x004fd8eb
int card_lich(int player, int card, event_t event)
{
  card_instance_t *instance;
  card_instance_t *damage_card;
  int current_card;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (is_in_play(player, card) && g_life[player] != -99)
  {
    g_life[player] = 0;
    for (current_card = 0; current_card < g_active_cards_count[player]; ++current_card)
    {
      if (is_in_play(player, current_card) && (global_cards_data[PLAYER_CARD_INSTANCE(player, current_card).internal_card_id].type & (TYPE_ARTIFACT | TYPE_ENCHANTMENT | TYPE_CREATURE | TYPE_LAND)) != 0)
      {
        ++g_life[player];
      }
    }
  }

  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }
  else
  {
    if (event == EVENT_CAST_SPELL && card == g_affected_card && player == g_affected_card_controller && g_other_player == player)
    {
      g_ai_modifier -= g_life[player] - 5;
    }

    if (event == EVENT_RESOLVE_SPELL)
    {
      unk_008b44d0[player] = 1;
    }

    if (event == EVENT_DEAL_DAMAGE && PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).internal_card_id == g_damage_card_internal_card_id && PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).damage_target_player == player && PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).damage_target_card == -1 && PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).info_slot != 0)
    {
      instance->targets[instance->info_slot].player = g_affected_card_controller;
      instance->targets[instance->info_slot].card = g_affected_card;
      ++instance->info_slot;
    }

    if (g_trigger_condition == TRIGGER_DEAL_DAMAGE && card == g_affected_card && player == g_affected_card_controller && instance->info_slot != 0 && player == g_current_turn)
    {
      if (event == EVENT_TRIGGER)
      {
        g_event_result |= RESOLVE_TRIGGER_MANDATORY;
      }

      if (event == EVENT_RESOLVE_TRIGGER)
      {
        do
        {
          damage_card = &PLAYER_CARD_INSTANCE(instance->targets[instance->info_slot - 1].player,
                                              instance->targets[instance->info_slot - 1].card);
          sacrifice_permanents_for_lich_damage(player, damage_card->info_slot);
          --instance->info_slot;
          if (instance->info_slot == 0)
          {
            break;
          }
        } while (g_life[player] != -99);
      }
    }

    if (event == EVENT_GRAVEYARD_FROM_PLAY && card == g_affected_card && player == g_affected_card_controller)
    {
      if (is_in_play(player, card))
      {
        if (g_duel_ai_mode_state == 1)
        {
          g_life[player] = -99;
        }
        else
        {
          exit_duel_thread(player);
        }
      }
    }

    return 0;
  }
}

// FUNCTION: MAGIC 0x0043b4f3
// FUNCTION: SHANDALAR 0x004fde93
int sacrifice_permanents_for_lich_damage(int player, int amount)
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
  while (sacrifices_made < sacrifices_to_make && g_life[player] != -99)
  {
    if (g_duel_ai_mode_state == 1)
    {
      target.player = player;
      do
      {
        do
        {
          g_ai_recorded_choice = internal_rand(g_active_cards_count[player]);
          target.card = g_ai_recorded_choice;
        } while (!is_in_play(target.player, target.card));
        instance = &PLAYER_CARD_INSTANCE(target.player, target.card);
      } while ((global_cards_data[instance->internal_card_id].type & 0x7f) == 0 || (instance->token_status & 0x10) != 0);
      record_ai_action_selection();
    }
    else if (player == g_other_player && (g_duel_network_flags & 2) == 0)
    {
      replay_ai_action_selection();
      target.player = player;
      target.card = g_ai_recorded_choice;
    }
    else
    {
      load_text("promptsX1.txt", "LICH");
      sprintf(prompt, g_text_lines[0], sacrifices_made + 1, sacrifices_to_make);
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
  card_instance_t *instance;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_CAN_CAST)
  {
    return real_target_available((int *)0, TARGET_SCAN_DIRECT, player, 2, 2,
                                 TARGET_ZONE_IN_PLAY, TYPE_CREATURE, TYPE_NONE, 0,
                                 get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0,
                                 -1, -1, -1, -1, 0, 0, 0);
  }

  if (event == EVENT_CAST_SPELL && g_affected_card == card && g_affected_card_controller == player)
  {
    load_text("promptsX1.txt", "OUBLIETTE");
    if (!select_target_creature_and_store(player, 2, card))
    {
      g_spell_fizzled = 1;
    }
    else
    {
      g_spell_fizzled = 0;
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    if (!C_real_validate_target(instance->targets[0].player, instance->targets[0].card,
                                (char *)0, player, 2, 2, TARGET_ZONE_IN_PLAY,
                                TYPE_CREATURE, TYPE_NONE, 0, get_protections_from(player, card),
                                COLOR_TEST_0, COLOR_TEST_0, -1, -1, -1, -1, 0, 0, 0))
    {
      kill_card(player, card, KILL_BURY);
      g_spell_fizzled = 1;
    }
    else
    {
      instance->damage_target_player = (char)instance->targets[0].player;
      instance->damage_target_card = instance->targets[0].card;
      tawnos_coffin_phase_out(instance->targets[0].player, instance->targets[0].card);
    }
    instance->number_of_targets = 0;
  }

  if (g_trigger_condition == TRIGGER_LEAVE_PLAY && g_affected_card == card &&
      g_affected_card_controller == player && player == g_current_turn &&
      g_trigger_cause_controller == player && g_trigger_cause == card &&
      instance->damage_target_player != -1 && instance->damage_target_card != -1)
  {
    if (event == EVENT_TRIGGER)
    {
      g_event_result |= RESOLVE_TRIGGER_MANDATORY;
    }
    if (event == EVENT_RESOLVE_TRIGGER)
    {
      tawnos_coffin_phase_in((int)instance->damage_target_player, instance->damage_target_card);
    }
  }

  if (event == EVENT_GRAVEYARD_FROM_PLAY && g_affected_card == card && g_affected_card_controller == player)
  {
    tawnos_coffin_phase_in((int)instance->damage_target_player, instance->damage_target_card);
    instance->damage_target_card = -1;
    instance->damage_target_player = (char)instance->damage_target_card;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0043c023
// FUNCTION: SHANDALAR 0x004fe9cd
int card_powerleech(int player, int card, event_t event)
{
  int opponent;
  int current_card;

  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    opponent = 1 - player;
    for (current_card = 0; current_card < g_active_cards_count[opponent]; ++current_card)
    {
      if (is_in_play(opponent, current_card) != 0 &&
          (global_cards_data[PLAYER_CARD_INSTANCE(opponent, current_card).internal_card_id].type & TYPE_ARTIFACT) != 0)
      {
        g_ai_modifier += 0xc;
      }
    }
  }

  if ((event == EVENT_TAP_CARD || event == EVENT_PLAY_ABILITY) &&
      (global_cards_data[PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).internal_card_id].type &
       TYPE_ARTIFACT) != 0 &&
      g_affected_card_controller != player &&
      (PLAYER_CARD_INSTANCE(player, card).state & STATE_INVISIBLE) == 0)
  {
    gain_life(player, 1, player, card);
  }

  return 0;
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
    return player == g_active_player || (g_duel_network_flags & 2) != 0;
  }

  if ((event == EVENT_RESOLVE_SPELL) && dispatch_function_to_all_cards_in_play(player, card, find_matching_active_control_effect, player) == -1)
  {
    *(int *)((char *)instance + 0x14) |= 0x1000000;
  }
  if ((event == 0x92) && (player == g_current_player) && ((*(unsigned char *)((char *)instance + 0x17) & 1) != 0))
  {
    defender = 1 - g_current_player;
    if ((g_active_player == defender) || ((g_duel_network_flags & 2) != 0))
    {
      for (current_card = 0; current_card < g_active_cards_count[defender]; ++current_card)
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
                                  g_text_lines[0],
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

      for (current_card = 0; current_card < g_active_cards_count[defender]; ++current_card)
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
      divide_creatures_into_two_piles(defender, player, card);
    }
  }

  if (g_trigger_condition == 0xde && card == g_card_on_stack && player == g_card_on_stack_controller && ((*(unsigned char *)((char *)instance + 0x17) & 1) != 0) && ((*(unsigned char *)((char *)&PLAYER_CARD_INSTANCE(g_trigger_cause_controller, g_trigger_cause) + 0x24) & 0x20) == 0) && g_current_turn == g_current_player && player == g_current_player)
  {
    if (event == 0x7d)
    {
      g_event_result |= 2;
    }
    if (event == 0x7e)
    {
      legacy_card = create_legacy_effect(player, card, unk_008b3bd4, g_trigger_cause_controller, g_trigger_cause);
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

  if (event == 0x77 && card == g_card_on_stack && player == g_card_on_stack_controller && ((*(unsigned char *)((char *)instance + 0x17) & 1) != 0))
  {
    legacy_card = dispatch_function_to_all_cards_in_play(player, card, find_matching_inactive_control_effect, player);
    if (legacy_card != -1)
    {
      *(int *)((char *)&PLAYER_CARD_INSTANCE(player, legacy_card) + 0x14) |= 0x1000000;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0043c7ab
// FUNCTION: SHANDALAR 0x004ff15c
int divide_creatures_into_two_piles(int who_is_being_divided, int player, int card)
{
  int bank;
  int current_card;
  int power_total[3];

  power_total[0] = 0;
  power_total[1] = 0;
  bank = internal_rand(2);
  for (current_card = 0; current_card < g_active_cards_count[who_is_being_divided]; ++current_card)
  {
    if (is_in_play(who_is_being_divided, current_card) && (global_cards_data[PLAYER_CARD_INSTANCE(who_is_being_divided, current_card).internal_card_id].type & TYPE_CREATURE) != 0 && (PLAYER_CARD_INSTANCE(who_is_being_divided, current_card).regen_status & 0x20) == 0)
    {
      power_total[2] = create_legacy_effect(player, card, unk_008b3bd4, who_is_being_divided, current_card);
      if (power_total[2] != -1)
      {
        if (power_total[1 - bank] < power_total[bank])
        {
          bank ^= 1;
        }
        power_total[bank] += PLAYER_CARD_INSTANCE(who_is_being_divided, current_card).toughness;
        if (bank != 0)
        {
          PLAYER_CARD_INSTANCE(player, power_total[2]).eot_toughness = 2;
        }
        else
        {
          PLAYER_CARD_INSTANCE(player, power_total[2]).eot_toughness = 1;
        }
      }
    }
  }

  return 0;
}
