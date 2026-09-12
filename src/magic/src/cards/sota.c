#include <stdio.h>
#include <string.h>
#include "../game_support.h"
#include "../global_duel_ui_ids.h"
#include "../global_strings.h"

extern card_ptr_t global_raw_cards_storage[2000];

int coin_flip(int player, char *dialog_title, int show_dialog_if_animation_is_off);
int move_ante_to_graveyard(int player, int ante_index);
int put_card_into_ante(int ante_player, int card_player, int card);
void hurkyls_recall_bounce_artifact(int target_player, int target_card);
int pick_internal_card_from_list_dialog(char *prompt, int initial_card_id, int filter);
int card_city_of_brass(int player, int card, event_t event);
int dispatch_trigger_twice_once_with_each_player_as_reason(int reason_for_trig, trigger_t trig, const char *prompt, int a4);
int tawnos_coffin_phase_in(int target_player, int target_card);
int tawnos_coffin_phase_out(int target_player, int target_card);
int tawnos_coffin_phase_attachment_callback(int source_player, int source_card, int target_player, int target_card, int target_internal_card_id);

// FUNCTION: SHANDALAR 0x00487123
int move_ante_to_graveyard(int player, int ante_index)
{
  int ante_slot;
  int done;
  int graveyard_slot;
  int internal_card_id;
  int result;

  graveyard_slot = 0;
  result = 0;
  if (ante_index < 16 && ante_index > -2)
  {
    while (graveyard_slot < 500 && global_graveyard_slots[player][graveyard_slot] != -1)
    {
      ++graveyard_slot;
    }

    if (graveyard_slot < 500)
    {
      if (ante_index == -1)
      {
        ante_slot = 0;
        done = 0;
        while (ante_slot < 16 && done == 0)
        {
          internal_card_id = global_ante_cards[player][ante_slot];
          if (internal_card_id == -1)
          {
            done = 1;
          }
          else
          {
            global_graveyard_slots[player][graveyard_slot] = internal_card_id;
            ++graveyard_slot;
            g_graveyard_card_types[player] |= global_cards_data[internal_card_id].type;
          }
          ++ante_slot;
        }
      }
      else
      {
        internal_card_id = global_ante_cards[player][ante_index];
        if (internal_card_id == -1)
        {
          result = 3;
        }
        else
        {
          global_graveyard_slots[player][graveyard_slot] = internal_card_id;
          g_graveyard_card_types[player] |= global_cards_data[internal_card_id].type;
        }
      }
    }
    else
    {
      result = 2;
    }

    if (result == 0)
    {
      if (ante_index == -1)
      {
        ante_slot = 0;
      }
      else
      {
        ante_slot = ante_index;
      }

      done = 0;
      while (ante_slot < 16 && done == 0)
      {
        if (ante_index == -1)
        {
          global_ante_cards[player][ante_slot] = -1;
        }
        else
        {
          global_ante_cards[player][ante_slot] = global_ante_cards[player][ante_slot + 1];
        }
        if (global_ante_cards[player][ante_slot + 1] == -1)
        {
          done = 1;
        }
        ++ante_slot;
      }
    }
  }
  else
  {
    result = 1;
  }

  return result;
}

// FUNCTION: SHANDALAR 0x00485185
int put_card_into_ante(int ante_player, int card_player, int card)
{
  int ante_slot = 0;

  while (global_ante_cards[ante_player][ante_slot] != -1 && ante_slot < 16)
  {
    ++ante_slot;
  }

  if (ante_slot < 16 && PLAYER_CARD_INSTANCE(card_player, card).internal_card_id != -1)
  {
    global_ante_cards[ante_player][ante_slot] = PLAYER_CARD_INSTANCE(card_player, card).internal_card_id;
    PLAYER_CARD_INSTANCE(card_player, card).token_status |= 8;
    kill_card(card_player, card, KILL_DESTROY);
    return 1;
  }
  else
  {
    return 0;
  }
}

// FUNCTION: MAGIC 0x004650b0
// FUNCTION: SHANDALAR 0x004547d0
int card_ashnod_s_altar(int player, int card, event_t event)
{
  int selected_card;

  if (event == EVENT_CAN_ACTIVATE)
  {
    return ((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0 &&
            real_target_available((int *)0, TARGET_SCAN_DIRECT, player, player, player, TARGET_ZONE_IN_PLAY,
                                  TYPE_CREATURE, TYPE_NONE, 0, 0, COLOR_TEST_0, COLOR_TEST_0,
                                  -1, -1, -1, -1, 0, 0, 0) != 0);
  }

  if (event == EVENT_ACTIVATE)
  {
    load_text("promptsX1.txt", "ASHNODS_ALTAR");
    selected_card = choose_creature_to_sacrifice(player);
    if (selected_card != -1)
    {
      kill_card(player, selected_card, KILL_SACRIFICE);
      undeclare_mana_available_and_produce_it(player, COLOR_COLORLESS, 2);
      g_produced_mana_color = 0;
    }
    else
    {
      g_spell_fizzled = 1;
    }
  }

  if (event == EVENT_COUNT_MANA &&
      g_affected_card == card &&
      g_affected_card_controller == player &&
      (PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0 &&
      (((PLAYER_CARD_INSTANCE(player, card).state & STATE_SUMMONSICK_BOTH) == 0) ||
       ((global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].type & TYPE_CREATURE) == 0)) &&
      real_target_available((int *)0, TARGET_SCAN_DIRECT, player, player, player, TARGET_ZONE_IN_PLAY,
                            TYPE_CREATURE, TYPE_NONE, 0, 0, COLOR_TEST_0, COLOR_TEST_0,
                            -1, -1, -1, -1, 0, 0, 0) != 0)
  {
    declare_mana_available(player, COLOR_COLORLESS, 2);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004652e8
// FUNCTION: SHANDALAR 0x00454a08
int card_ashnod_s_transmogrant(int player, int card, event_t event)
{
  int dynamic_internal_card_id;
  int legacy_card;
  target_t target;

  if (event == EVENT_CAN_ACTIVATE)
  {
    if ((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0 &&
        real_target_available((int *)0, TARGET_SCAN_DIRECT, player, 2, 2, TARGET_ZONE_IN_PLAY,
                              TYPE_CREATURE, TYPE_ARTIFACT, 0, get_protections_from(player, card),
                              COLOR_TEST_0, COLOR_TEST_0, -1, -1, -1, -1, 0, 0, 0) != 0)
    {
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
    load_text("promptsX1.txt", "ASHNODS_TRANSMORGRANT");
    if (C_real_select_target(player, 2, 2, TARGET_ZONE_IN_PLAY, TYPE_CREATURE, TYPE_ARTIFACT,
                             0, get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0,
                             -1, -1, -1, -1, 0, 0, 0, g_text_lines[0], 1, &target))
    {
      SET_TARGET(PLAYER_CARD_INSTANCE(player, card).targets[0], target);
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
      PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
      kill_card(player, card, KILL_SACRIFICE);
      if (target.player == g_active_player)
      {
        g_ai_modifier -= 0x18;
      }
      if (global_cards_data[TARGET_CARD_INSTANCE(player, card, 0).internal_card_id].subtype == 0 &&
          (TARGET_CARD_INSTANCE(player, card, 0).token_status & 0x800) == 0)
      {
        g_ai_modifier -= 0x18;
      }
    }
    else
    {
      g_spell_fizzled = 1;
    }
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    SET_TARGET(target, PLAYER_CARD_INSTANCE(player, card).targets[0]);
    if (C_real_validate_target(target.player, target.card, (char *)0, player, 2, 2, TARGET_ZONE_IN_PLAY,
                               TYPE_CREATURE, TYPE_ARTIFACT, 0, get_protections_from(player, card),
                               COLOR_TEST_0, COLOR_TEST_0, -1, -1, -1, -1, 0, 0, 0))
    {
      legacy_card = create_legacy_effect(g_card_on_stack_controller, g_card_on_stack, g_duel_generated_internal_card_id_22,
                                         target.player, target.card);
      if (legacy_card != -1)
      {
        dynamic_internal_card_id = create_a_card_type(PLAYER_CARD_INSTANCE(target.player, target.card).internal_card_id);
        if (dynamic_internal_card_id != -1)
        {
          global_cards_data[dynamic_internal_card_id].type |= TYPE_CREATURE | TYPE_ARTIFACT;
          PLAYER_CARD_INSTANCE(player, legacy_card).dummy3 = dynamic_internal_card_id;
          PLAYER_CARD_INSTANCE(player, legacy_card).token_status |= 0x20;
        }
      }

      if ((TARGET_CARD_INSTANCE(player, card, 0).counters & 0xff00) < 0xff01)
      {
        TARGET_CARD_INSTANCE(player, card, 0).counters = ((TARGET_CARD_INSTANCE(player, card, 0).counters + 0x100) & 0xff00) |
                                                         (TARGET_CARD_INSTANCE(player, card, 0).counters & 0xffff00ff);
      }
      if (g_duel_ai_mode_state != 1)
      {
        play_sound_effect(WAV_COUNTER);
      }
    }
    else
    {
      g_spell_fizzled = 1;
    }
    PARENT_CARD_INSTANCE(player, card).number_of_targets = 0;
  }

  if (event == EVENT_CHECK_PUMP && (PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0)
  {
    ++g_global_power_bonus[player];
    ++g_global_toughness_bonus[player];
  }

  return 0;
}

// FUNCTION: MAGIC 0x00465b27
// FUNCTION: SHANDALAR 0x00455247
int card_basalt_monolith(int player, int card, event_t event)
{
  int result;

  if (((event == EVENT_UNTAP) && (card == g_affected_card)) && (player == g_affected_card_controller))
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
    if (((player == g_other_player) && ((g_duel_network_flags & 2) == 0)) && PLAYER_CARD_INSTANCE(player, card).info_slot != 0)
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
        if (g_spell_fizzled != 1)
        {
          PLAYER_CARD_INSTANCE(player, card).eot_toughness = 1;
        }
      }
    }
    else
    {
      g_ai_modifier -= 0x24;
      undeclare_mana_available_and_produce_it(player, COLOR_COLORLESS, 3);
      g_produced_mana_color = 0;
      PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
    }
  }

  if ((event == EVENT_RESOLVE_ACTIVATION) && (PARENT_CARD_INSTANCE(player, card).state & STATE_TAPPED) && PLAYER_CARD_INSTANCE(player, card).eot_toughness != 0)
  {
    g_battlefield_extra_ability_flags |= 2;
    PARENT_CARD_INSTANCE(player, card).info_slot = 1;
    PARENT_CARD_INSTANCE(player, card).eot_toughness = 0;
  }

  if ((((((g_trigger_condition == 0xcb) || (g_trigger_condition == 0xe2)) || (g_trigger_condition == 0xe3)) || (g_trigger_condition == 0xe4)) && ((card == g_affected_card) && (player == g_affected_card_controller))) && ((g_current_turn == player) && ((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) != 0) && PLAYER_CARD_INSTANCE(player, card).info_slot != 0))
  {
    if (event == EVENT_TRIGGER)
    {
      g_event_result |= 2;
    }
    if (event == EVENT_RESOLVE_TRIGGER)
    {
      PLAYER_CARD_INSTANCE(player, card).info_slot = 0;
      PLAYER_CARD_INSTANCE(player, card).state &= ~STATE_TAPPED;
    }
  }

  if (((((event == EVENT_COUNT_MANA) && (card == g_affected_card)) && (player == g_affected_card_controller)) && ((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0)) && (((PLAYER_CARD_INSTANCE(player, card).state & STATE_SUMMONSICK_BOTH) == 0) || (global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].type & TYPE_CREATURE) == 0))
  {
    declare_mana_available(player, COLOR_COLORLESS, 3);
  }

  return 0;
}

// FUNCTION: MAGIC 0x00466435
// FUNCTION: SHANDALAR 0x00455b59
int card_copper_tablet(int player, int card, event_t event)
{
  if (event == EVENT_CAST_SPELL && card == g_affected_card && player == g_affected_card_controller)
  {
    g_ai_modifier += ((g_life[player] - g_life[1 - player]) * 3) << 3;
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    if ((g_current_phase == PHASE_UPKEEP) && ((PLAYER_CARD_INSTANCE(player, card).info_slot & 1) == 0) &&
        (g_event_player == g_current_player) &&
        (((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0) ||
         ((global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].type & TYPE_CREATURE) != 0)))
    {
      PLAYER_CARD_INSTANCE(player, card).upkeep_flags |= 0x101;
      g_activation_event_flags |= 3;
      return 1;
    }
    return 0;
  }

  if (event == EVENT_UPKEEP_PHASE && card == g_affected_card && player == g_affected_card_controller)
  {
    PLAYER_CARD_INSTANCE(player, card).info_slot |= 1;
    g_upkeep_payment_completed = 1;
    g_event_result |= 1;
  }
  if (event == EVENT_UPKEEP_COSTS_UNPAID)
  {
    damage_player(g_current_player, 1, g_card_on_stack_controller, g_card_on_stack);
  }
  if (event == EVENT_CLEANUP)
  {
    PLAYER_CARD_INSTANCE(player, card).info_slot &= ~1;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0046667e
// FUNCTION: SHANDALAR 0x00455da6
int card_cyclopean_tomb(int player, int card, event_t event)
{
  int chosen_land_type;
  int legacy_card;
  target_t target;

  chosen_land_type = get_hacked_color(player, card, 1) - 1;

  if (event == EVENT_CAST_SPELL && g_affected_card == card && g_affected_card_controller == player)
  {
    PLAYER_CARD_INSTANCE(player, card).info_slot = g_cyclopean_tomb_cast_sequence;
    ++g_cyclopean_tomb_cast_sequence;
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    if (g_current_phase == 4 && player == g_current_player && player == g_event_player && (PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0 && (((PLAYER_CARD_INSTANCE(player, card).state & STATE_SUMMONSICK_BOTH) == 0) || (global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].type & TYPE_CREATURE) == 0) && has_mana(player, 7, 2) != 0)
    {
      if (real_target_available((int *)0, TARGET_SCAN_DIRECT, player, 2, 1 - player, 0x200, TYPE_LAND, 0, 0,
                                get_protections_from(player, card), 0, 0, chosen_land_type,
                                -1, -1, -1, 0x100, 0, 0) != 0)
      {
        if (player == g_active_player || (g_duel_network_flags & 2) != 0)
        {
          return 1;
        }
        if (internal_rand(100) < ((has_mana(player, 7, 1) + 1) / 2) * 0x14)
        {
          g_activation_event_flags |= 3;
          return 1;
        }
      }
    }
    return 0;
  }

  if (event == EVENT_ACTIVATE && g_affected_card == card && g_affected_card_controller == player)
  {
    charge_mana(player, COLOR_COLORLESS, 2);
    if (g_spell_fizzled != 1)
    {
      if (player == g_other_player && (g_duel_network_flags & 2) == 0)
      {
        if (select_land_for_cyclopean_tomb_ai(player, card, 1 - player))
        {
          PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
        }
      }
      else
      {
        load_text("promptsX1.txt", "CYCLOPEAN_TOMB");
        if (C_real_select_target(player, 2, player, TARGET_ZONE_IN_PLAY, TYPE_LAND, TYPE_NONE, 0,
                                 get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0,
                                 chosen_land_type, ~SUB_WALL, -1, -1,
                                 TARGET_SPECIAL_NOT_LAND_SUBTYPE, 0, 0, g_text_lines[0], 1,
                                 &target))
        {
          SET_TARGET(PLAYER_CARD_INSTANCE(player, card).targets[0], target);
          PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
          PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
        }
        else
        {
          g_spell_fizzled = 1;
        }
      }
    }
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    SET_TARGET(target, PLAYER_CARD_INSTANCE(player, card).targets[0]);
    if (C_real_validate_target(target.player, target.card, (char *)0,
                               player, 2, 2, TARGET_ZONE_IN_PLAY, TYPE_LAND, TYPE_NONE, 0,
                               get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0,
                               chosen_land_type, ~SUB_WALL, -1, -1,
                               TARGET_SPECIAL_NOT_LAND_SUBTYPE, 0, 0))
    {
      legacy_card = create_legacy_effect(g_card_on_stack_controller, g_card_on_stack, g_duel_generated_internal_card_id_23,
                                         target.player, target.card);
      if (legacy_card != -1)
      {
        PLAYER_CARD_INSTANCE(player, legacy_card).dummy3 = get_hacked_color(player, card, 1) - 1;
        PLAYER_CARD_INSTANCE(player, legacy_card).token_status = 0x10000;
        PLAYER_CARD_INSTANCE(player, legacy_card).info_slot = PARENT_CARD_INSTANCE(player, card).info_slot;
        PLAYER_CARD_INSTANCE(player, legacy_card).eot_toughness = 1;
      }
    }
    else
    {
      g_spell_fizzled = 1;
    }
    PARENT_CARD_INSTANCE(player, card).number_of_targets = 0;
  }

  if (event == 0x77 && g_affected_card == card && g_affected_card_controller == player)
  {
    legacy_card = create_legacy_effect(player, card, g_duel_generated_internal_card_id_23, -1, -1);
    if (legacy_card != -1)
    {
      PLAYER_CARD_INSTANCE(player, legacy_card).info_slot = PLAYER_CARD_INSTANCE(player, card).info_slot;
      PLAYER_CARD_INSTANCE(player, legacy_card).eot_toughness = 2;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x00466e6d
// FUNCTION: SHANDALAR 0x0045659d
int select_land_for_cyclopean_tomb_ai(int player, int card, int target_player)
{
  int land_type;
  int preferred_basic;
  int smallest_count;
  int current_type;
  int current_card;
  int internal_card_id;
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
    if (current_type != land_type && g_basiclandtypes_controlled[target_player][current_type] != 0 && g_basiclandtypes_controlled[target_player][current_type] < smallest_count)
    {
      smallest_count = g_basiclandtypes_controlled[target_player][current_type];
      preferred_basic = current_type;
    }
  }
  if (preferred_basic != -1)
  {
    --preferred_basic;
  }

  for (current_card = 0; current_card < g_active_cards_count[target_player] && !found; ++current_card)
  {
    internal_card_id = PLAYER_CARD_INSTANCE(target_player, current_card).internal_card_id;
    if (is_in_play(target_player, current_card) && (global_cards_data[internal_card_id].type & TYPE_LAND) != 0)
    {
      if (global_cards_data[internal_card_id].subtype == SUB_BASIC_LAND)
      {
        if (preferred_basic != -1 && internal_card_id == preferred_basic)
        {
          preferred_basic = -1;
          PLAYER_CARD_INSTANCE(player, card).targets[0].player = target_player;
          PLAYER_CARD_INSTANCE(player, card).targets[0].card = current_card;
          PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
        }
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
  if ((event == EVENT_TOUGHNESS || event == EVENT_POWER) && is_in_play(player, card) && (((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0) || (global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].type & TYPE_CREATURE)))
  {
    if (((1 << (unsigned char)get_sleighted_color(player, card, 4)) &
         (char)PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).color) != 0)
    {
      ++g_event_result;
    }
  }

  if (event == EVENT_TAP_CARD)
  {
    if (card_has_basic_land_type(g_affected_card_controller, g_affected_card,
                                 get_hacked_color(player, card, 4)) != 0 &&
        is_in_play(player, card) &&
        ((((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0) ||
          (global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].type & TYPE_CREATURE)) &&
         g_produced_mana_color != -1))
    {
      produce_mana(g_affected_card_controller, COLOR_RED, 1);
    }
  }

  if (event == EVENT_COUNT_MANA)
  {
    if (card_has_basic_land_type(g_affected_card_controller, g_affected_card,
                                 get_hacked_color(player, card, 4)) != 0 &&
        is_in_play(player, card) &&
        (((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0) ||
         (global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].type & TYPE_CREATURE)))
    {
      declare_mana_available(g_affected_card_controller, COLOR_RED, 1);
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x004675a1
// FUNCTION: SHANDALAR 0x00456cd1
int card_icy_manipulator(int player, int card, event_t event)
{
  target_t target;

  if ((event == EVENT_CAST_SPELL) && (card == g_affected_card) && (player == g_affected_card_controller))
  {
    g_ai_modifier += 0xc;
  }
  if (event == EVENT_CAN_ACTIVATE)
  {
    return (has_mana(player, COLOR_ANY, 1) && CAN_TAP(player, card) && real_target_available((int *)0, TARGET_SCAN_DIRECT, player, 2, 2, TARGET_ZONE_IN_PLAY, TYPE_ARTIFACT | TYPE_CREATURE | TYPE_LAND, TYPE_NONE, 0, get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0, -1, -1, 0xffffffff, 0xffffffff, 0, 0, 0));
  }

  if (event == 0x90)
  {
    load_recorded_action_target(0);
    return 0;
  }

  if ((event == EVENT_ACTIVATE) && has_mana(player, 7, 1))
  {
    charge_mana(player, 0, 1);
    if (g_spell_fizzled != 1)
    {
      load_text("promptsX1.txt", "ICY_MANIPULATOR");

      if (C_real_select_target(player,
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
        SET_TARGET(PLAYER_CARD_INSTANCE(player, card).targets[0], target);
        PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
        if (g_other_player == player)
        {
          if ((global_cards_data[PLAYER_CARD_INSTANCE(target.player, target.card).internal_card_id].type & TYPE_LAND) != 0)
          {
            g_ai_modifier -= 0x18;
          }
          if (target.player == g_other_player)
          {
            g_ai_modifier -= 0x60;
          }
        }
        PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
      }
      else
      {
        g_spell_fizzled = 1;
      }
    }
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    SET_TARGET(target, PLAYER_CARD_INSTANCE(player, card).targets[0]);

    if (C_real_validate_target(target.player,
                               target.card,
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
                               -1,
                               -1,
                               -1,
                               0,
                               0,
                               0))
    {
      tap_card_and_dispatch_event(target.player, target.card);
    }
    else
    {
      g_spell_fizzled = 1;
    }
    PARENT_CARD_INSTANCE(player, card).number_of_targets = 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x00467a6c
// FUNCTION: SHANDALAR 0x00457199
int card_jade_statue(int player, int card, event_t event)
{
  int animated_internal_card_id;

  if ((event == EVENT_CAST_SPELL) && (card == g_affected_card) && (player == g_affected_card_controller))
  {
    PLAYER_CARD_INSTANCE(player, card).dummy3 = PLAYER_CARD_INSTANCE(player, card).internal_card_id;
  }
  if (event == EVENT_RESOLVE_SPELL)
  {
    PLAYER_CARD_INSTANCE(player, card).state |= 0x3000000;
    PLAYER_CARD_INSTANCE(player, card).info_slot = -1;
  }
  if (((event == 0x79) || ((event == 0x78) && (card == g_affected_card) && (player == g_affected_card_controller))) && (PLAYER_CARD_INSTANCE(player, card).state & 4) == 0 && !has_mana(player, 7, 2))
  {
    g_event_result = 1;
  }
  if ((((g_trigger_condition == 0xdc) && (g_current_phase == 0x15) && (g_current_turn == g_current_player)) || ((g_trigger_condition == 0xdd) && (g_current_phase == 0x17) && (g_current_turn != g_current_player))) && card == g_affected_card && player == g_affected_card_controller && PLAYER_CARD_INSTANCE(player, card).eot_toughness == 0 && (PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0 && g_trigger_cause_controller == player && g_trigger_cause == card)
  {
    if (has_mana(player, 7, 2))
    {
      if (event == 0x7d)
      {
        g_event_result |= 2;
      }
      if (event == 0x7e)
      {
        push_card_onto_stack(player, card, EVENT_RESOLVE_ACTIVATION, 0, 0);
        charge_mana(player, 0, 2);
        obliterate_top_card_of_stack();
        if (g_spell_fizzled == 1)
        {
          g_combat_assignment_cancelled = 1;
          g_spell_fizzled = 0;
        }
        else
        {
          animated_internal_card_id = create_a_card_type(PLAYER_CARD_INSTANCE(player, card).internal_card_id);
          if (animated_internal_card_id != -1)
          {
            global_cards_data[animated_internal_card_id].type |= TYPE_CREATURE;
            global_cards_data[animated_internal_card_id].power = 3;
            global_cards_data[animated_internal_card_id].toughness = 6;
            PLAYER_CARD_INSTANCE(player, card).dummy3 = animated_internal_card_id;
            PLAYER_CARD_INSTANCE(player, card).regen_status |= 0x1000000;
            PLAYER_CARD_INSTANCE(player, card).info_slot = PLAYER_CARD_INSTANCE(player, card).internal_card_id;
          }
          PLAYER_CARD_INSTANCE(player, card).eot_toughness = 1;
        }
      }
    }
    else
    {
      g_combat_assignment_cancelled = 1;
    }
  }
  if ((event == 0x3c) && (g_land_can_be_played & LCBP_DURING_EVENT_CHANGE_TYPE_SECOND_PASS) == 0 && card == g_affected_card && player == g_affected_card_controller && is_in_play(player, card))
  {
    g_event_result = PLAYER_CARD_INSTANCE(player, card).dummy3;
  }
  if (g_trigger_condition == 0xcc && card == g_affected_card && player == g_affected_card_controller && g_current_turn == player && PLAYER_CARD_INSTANCE(player, card).info_slot != -1)
  {
    if (event == 0x7d)
    {
      g_event_result |= 2;
    }
    if (event == 0x7e)
    {
      invalidate_dynamic_card_type(PLAYER_CARD_INSTANCE(player, card).dummy3);
      PLAYER_CARD_INSTANCE(player, card).dummy3 = PLAYER_CARD_INSTANCE(player, card).info_slot;
      PLAYER_CARD_INSTANCE(player, card).info_slot = -1;
      PLAYER_CARD_INSTANCE(player, card).eot_toughness = 0;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x00468109
// FUNCTION: SHANDALAR 0x0045782f
int card_jalum_tome(int player, int card, event_t event)
{
  int current_card;
  int internal_card_id;
  int land_count;

  if (event == EVENT_CAN_ACTIVATE)
  {
    if ((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0 &&
        (((PLAYER_CARD_INSTANCE(player, card).state & STATE_SUMMONSICK_BOTH) == 0) ||
         ((global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].type & TYPE_CREATURE) == 0)) &&
        has_mana(player, COLOR_ANY, 2) != 0)
    {
      if (g_other_player == player && (g_duel_network_flags & 2) == 0)
      {
        land_count = 0;
        for (current_card = 0; current_card < g_active_cards_count[player]; ++current_card)
        {
          internal_card_id = PLAYER_CARD_INSTANCE(player, current_card).internal_card_id;
          if (internal_card_id != -1 && (global_cards_data[internal_card_id].type & TYPE_LAND) != 0)
          {
            ++land_count;
          }
        }
        if (land_count < 3 || land_count > 8)
        {
          g_ai_modifier += 0x30;
        }
      }
      return 1;
    }
  }
  else
  {
    if (event == EVENT_ACTIVATE)
    {
      charge_mana(player, COLOR_COLORLESS, 2);
      if (g_spell_fizzled != 1)
      {
        PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
      }
    }
    if (event == EVENT_RESOLVE_ACTIVATION)
    {
      draw_card_for_player(player);
      discard(player, 0, 0);
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x00468339
// FUNCTION: SHANDALAR 0x00457a60
int card_jandor_s_ring(int player, int card, event_t event)
{
  int current_card;
  struct
  {
    int count;
    int selected_index;
    int card_ids[30];
    int card_indices[30];
  } s;

  if (event == EVENT_CAN_ACTIVATE)
  {
    return (PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0 &&
           (((PLAYER_CARD_INSTANCE(player, card).state & STATE_SUMMONSICK_BOTH) == 0) ||
            ((global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].type & TYPE_CREATURE) == 0)) &&
           has_mana(player, COLOR_ANY, 2) != 0;
  }

  if (event == EVENT_ACTIVATE)
  {
    charge_mana(player, COLOR_COLORLESS, 2);
    if (g_spell_fizzled != 1)
    {
      PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
    }
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    s.count = 0;
    for (current_card = 0; current_card < g_active_cards_count[player]; ++current_card)
    {
      if (PLAYER_CARD_INSTANCE(player, current_card).internal_card_id != -1 &&
          (PLAYER_CARD_INSTANCE(player, current_card).state & STATE_JUST_DRAWED) != 0 &&
          (PLAYER_CARD_INSTANCE(player, current_card).state & STATE_IN_PLAY) == 0)
      {
        s.card_ids[s.count] = PLAYER_CARD_INSTANCE(player, current_card).internal_card_id;
        s.card_indices[s.count] = current_card;
        ++s.count;
      }
    }

    if (s.count > 0)
    {
      if (player == g_active_player || (g_duel_network_flags & 2) != 0)
      {
        if (player == g_active_player)
        {
          load_text("promptsX1.txt", "JANDORS_RING");
        }
        s.selected_index = show_deck(player, s.card_ids, s.count, g_text_lines[0], 1, "");
      }
      else
      {
        s.selected_index = internal_rand(s.count);
      }
      kill_card(player, s.card_indices[s.selected_index], KILL_DESTROY);
      draw_card_for_player(player);
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0046867c
// FUNCTION: SHANDALAR 0x00457da1
int card_jeweled_bird(int player, int card, event_t event)
{
  if (event == EVENT_CAN_ACTIVATE)
  {
    return CAN_TAP(player, card);
  }

  if (event == EVENT_ACTIVATE)
  {
    if (g_other_player == player &&
        (global_ante_cards[player][1] != -1 ||
         (global_ante_cards[player][0] != -1 &&
          global_raw_cards_storage[global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].id].expansion_rarity <=
              global_raw_cards_storage[global_cards_data[global_ante_cards[player][0]].id].expansion_rarity)))
    {
      g_ai_modifier += 0x30;
    }
    PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    draw_card_for_player(player);
    move_ante_to_graveyard(player, -1);
    put_card_into_ante(player, g_card_on_stack_controller, g_card_on_stack);
  }

  return 0;
}

// FUNCTION: MAGIC 0x00468878
// FUNCTION: SHANDALAR 0x00457f9e
int card_living_wall(int player, int card, event_t event)
{
  if (event == EVENT_CAN_ACTIVATE || event == EVENT_ACTIVATE || event == EVENT_RESOLVE_ACTIVATION)
  {
    return generic_regeneration_ability(player, card, event, 0, 1);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004688c5
// FUNCTION: SHANDALAR 0x00457feb
int card_mana_crypt(int player, int card, event_t event)
{
  struct
  {
    int result;
    char dialog_text[900];
    int choice;
  } s;

  if (event == EVENT_CAST_SPELL && g_affected_card == card && g_affected_card_controller == player)
  {
    g_ai_modifier += (4 - g_basiclandtypes_controlled[g_other_player][7]) * 0x18;
  }

  if (event == EVENT_ACTIVATE)
  {
    undeclare_mana_available_and_produce_it(player, COLOR_COLORLESS, 2);
    PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
    g_produced_mana_color = 0;
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    s.result = 0;
    if ((((PLAYER_CARD_INSTANCE(player, card).state & STATE_SUMMONSICK_BOTH) == 0) ||
         ((global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].type & TYPE_CREATURE) == 0)) &&
        (PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0)
    {
      if (g_current_phase != PHASE_UPKEEP || (PLAYER_CARD_INSTANCE(player, card).info_slot & 1) != 0)
      {
        s.result = 1;
      }
      else
      {
        if (g_current_player == g_event_player && g_current_player == player)
        {
          PLAYER_CARD_INSTANCE(player, card).upkeep_flags |= 0x101;
          g_activation_event_flags |= 3;
          s.result = 1;
        }
      }
    }
    return s.result;
  }

  if (event == EVENT_UPKEEP_PHASE && g_affected_card == card && g_affected_card_controller == player)
  {
    PLAYER_CARD_INSTANCE(player, card).info_slot |= 1;
    g_upkeep_payment_completed = 1;
    g_event_result |= 1;
  }

  if (event == EVENT_UPKEEP_COSTS_UNPAID)
  {
    load_text("promptsX1.txt", "MANA_CRYPT");
    if (g_active_player == player)
    {
      sprintf(s.dialog_text, "%s\n %s\n %s", g_text_lines[0], g_text_lines[1], g_text_lines[2]);
    }
    else
    {
      sprintf(s.dialog_text, " %s\n %s", g_text_lines[1], g_text_lines[2]);
    }
    s.choice = do_dialog(player, player, card, -1, -1, s.dialog_text, internal_rand(2));
    if (coin_flip(player, get_displayed_card_name(g_card_on_stack_controller, g_card_on_stack), 1) != s.choice)
    {
      damage_player(player, 3, g_card_on_stack_controller, g_card_on_stack);
    }
  }

  if (event == EVENT_CLEANUP)
  {
    PLAYER_CARD_INSTANCE(player, card).info_slot &= ~1;
  }

  if (event == EVENT_COUNT_MANA &&
      g_affected_card == card &&
      g_affected_card_controller == player &&
      (PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0 &&
      (((PLAYER_CARD_INSTANCE(player, card).state & STATE_SUMMONSICK_BOTH) == 0) ||
       ((global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].type & TYPE_CREATURE) == 0)))
  {
    declare_mana_available(player, COLOR_COLORLESS, 2);
  }

  if (event == EVENT_SHOULD_AI_PLAY && g_current_phase == PHASE_DISCARD && g_current_player != player)
  {
    damage_player(player, 2, player, card);
  }

  return 0;
}

// FUNCTION: MAGIC 0x00468db5
// FUNCTION: SHANDALAR 0x004584e4
int card_mightstone(int player, int card, event_t event)
{
  if (event == EVENT_CAST_SPELL && card == g_affected_card && player == g_affected_card_controller)
  {
    g_ai_modifier += (g_duel_summary.creature_counts[player] - ((int *)&g_duel_summary)[5 - player]) * 0xc;
  }

  if (event == EVENT_POWER &&
      (((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0) ||
       ((global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].type & TYPE_CREATURE) != 0)) &&
      (PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).state & STATE_ATTACKING) != 0 &&
      g_affected_card_controller == g_current_player &&
      is_in_play(player, card) != 0)
  {
    ++g_event_result;
  }

  return 0;
}

// FUNCTION: MAGIC 0x00468eff
// FUNCTION: SHANDALAR 0x0045862c
int card_obelisk_of_undoing(int player, int card, event_t event)
{
  target_t target;

  if (event == EVENT_CAN_ACTIVATE)
  {
    return ((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0 &&
            (((PLAYER_CARD_INSTANCE(player, card).state & STATE_SUMMONSICK_BOTH) == 0) ||
             ((global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].type & TYPE_CREATURE) == 0)) &&
            has_mana(player, COLOR_ANY, 6) != 0 &&
            real_target_available((int *)0, TARGET_SCAN_DIRECT, player, player | 0xc, player | 0xc, TARGET_ZONE_IN_PLAY,
                                  0x1047, TYPE_NONE, 0, get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0,
                                  -1, -1, -1, -1, 0, 0, 0) != 0);
  }

  if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_target(0);
    return 0;
  }

  if (event == EVENT_ACTIVATE)
  {
    charge_mana(player, COLOR_COLORLESS, 6);
    if (g_spell_fizzled != 1)
    {
      load_text("promptsX1.txt", "OBELISK_OF_UNDOING");
      if (C_real_select_target(player, player | 0xc, player | 0xc, TARGET_ZONE_IN_PLAY,
                               0x1047, TYPE_NONE, 0, get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0,
                               -1, -1, -1, -1, 0, 0, 0, g_text_lines[0], 1, &target) != 0)
      {
        SET_TARGET(PLAYER_CARD_INSTANCE(player, card).targets[0], target);
        PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
        PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
      }
      else
      {
        g_spell_fizzled = 1;
      }
    }
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    if (C_real_validate_target(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                               PLAYER_CARD_INSTANCE(player, card).targets[0].card,
                               (char *)0,
                               player,
                               player | 0xc,
                               player | 0xc,
                               TARGET_ZONE_IN_PLAY,
                               0x1047,
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
                               0) != 0)
    {
      hurkyls_recall_bounce_artifact(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                                     PLAYER_CARD_INSTANCE(player, card).targets[0].card);
    }
    else
    {
      g_spell_fizzled = 1;
    }
    PARENT_CARD_INSTANCE(player, card).number_of_targets = 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x00482e9f
// FUNCTION: SHANDALAR 0x004869b0
int pyramids_can_destroy_enchantment_on_land(void)
{
  int can_activate;
  int current_player;

  can_activate = 0;

  for (current_player = 0; current_player < 2 && can_activate == 0; current_player++)
  {
    int current_card;
    for (current_card = 0; current_card < g_active_cards_count[current_player] && can_activate == 0; current_card++)
    {
      if (is_in_play(current_player, current_card) != 0 &&
          (global_cards_data[PLAYER_CARD_INSTANCE(current_player, current_card).internal_card_id].type & TYPE_ENCHANTMENT) != 0 &&
          (int)PLAYER_CARD_INSTANCE(current_player, current_card).damage_target_player != -1)
      {
        int attached_internal_card_id = DAMAGE_TARGET_CARD_INSTANCE(current_player, current_card).internal_card_id;
        if (attached_internal_card_id != -1 &&
            (global_cards_data[attached_internal_card_id].type & TYPE_LAND) != 0)
        {
          can_activate = 1;
        }
      }
    }
  }

  return can_activate;
}

// FUNCTION: SHANDALAR 0x0049ff1f
int select_land_target_into_next_slot(int player, unsigned int preferred_controller, int card)
{
  target_t target;
  int selected;

  if (preferred_controller == (unsigned int)-1)
  {
    preferred_controller = 2;
  }

  selected = C_real_select_target(player, 2, preferred_controller, TARGET_ZONE_IN_PLAY,
                                  TYPE_LAND, TYPE_NONE, 0, 0, COLOR_TEST_0, COLOR_TEST_0,
                                  -1, -1, -1, -1, 0, 0, 0, g_text_lines[0], 1, &target);
  if (selected != 0)
  {
    SET_TARGET(PLAYER_CARD_INSTANCE(player, card).targets[PLAYER_CARD_INSTANCE(player, card).number_of_targets], target);
    ++PLAYER_CARD_INSTANCE(player, card).number_of_targets;
  }

  return selected != 0;
}

// FUNCTION: MAGIC 0x00469386
// FUNCTION: SHANDALAR 0x00458ab3
int card_pyramids(int player, int card, event_t event)
{
  int target_internal_card_id;
  int attached_internal_card_id;
  int can_activate;
  int done;
  int current_card;
  int current_player;
  target_t target;

  if (event == EVENT_CAN_ACTIVATE)
  {
    can_activate = 0;
    if (has_mana(player, COLOR_ANY, 2) != 0 &&
        (PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0)
    {
      if ((g_land_can_be_played & LCBP_REGENERATION) != 0)
      {
        current_player = 0;
        while (current_player < 2)
        {
          current_card = 0;
          while (current_card < g_active_cards_count[current_player] && can_activate == 0)
          {
            target_internal_card_id = PLAYER_CARD_INSTANCE(current_player, current_card).internal_card_id;
            if (is_in_play(current_player, current_card) != 0 &&
                (global_cards_data[target_internal_card_id].type & TYPE_LAND) != 0 &&
                (int)(char)PLAYER_CARD_INSTANCE(current_player, current_card).kill_code == KILL_DESTROY)
            {
              can_activate = 99;
            }
            ++current_card;
          }
          ++current_player;
        }
      }
      else
      {
        can_activate = pyramids_can_destroy_enchantment_on_land();
      }
    }
    return can_activate;
  }

  if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_target(0);
    return 0;
  }

  if (event == EVENT_ACTIVATE)
  {
    charge_mana(player, COLOR_COLORLESS, 2);
    if (g_spell_fizzled != 1)
    {
      done = 0;
      if ((g_land_can_be_played & LCBP_REGENERATION))
      {
        do
        {
          PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
          load_text("promptsX1.txt", "PYRAMIDS");
          if (select_land_target_into_next_slot(player, (unsigned int)-1, card) == 0)
          {
            g_spell_fizzled = 1;
          }
          else
          {
            SET_TARGET(target, PLAYER_CARD_INSTANCE(player, card).targets[0]);
            if ((int)(char)PLAYER_CARD_INSTANCE(target.player, target.card).kill_code == KILL_DESTROY)
            {
              done = 1;
              if ((PLAYER_CARD_INSTANCE(target.player, target.card).token_status & 0x200) != 0 ||
                  target.player == g_active_player)
              {
                g_ai_modifier -= 0x30;
              }
            }
            else if (g_duel_ai_mode_state == 1)
            {
              rewind_recorded_action();
            }
            else
            {
              load_text("promptsX1.txt", "PYRAMIDS");
              set_duel_prompt_text(g_text_lines[1]);
              Sleep(0x9c4);
              set_duel_prompt_text("");
            }
          }
        } while (g_spell_fizzled != 1 && done == 0);
      }
      else
      {
        do
        {
          load_text("promptsX1.txt", "PYRAMIDS");
          if (C_real_select_target(player, 2, 2, TARGET_ZONE_IN_PLAY, TYPE_ENCHANTMENT, TYPE_NONE,
                                   0, get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0,
                                   -1, -1, -1, -1, 0, 0, 0, g_text_lines[2], 1, &target))
          {
            if ((int)PLAYER_CARD_INSTANCE(target.player, target.card).damage_target_player == -1 ||
                PLAYER_CARD_INSTANCE(target.player, target.card).damage_target_card == -1 ||
                (global_cards_data[DAMAGE_TARGET_CARD_INSTANCE(target.player, target.card).internal_card_id]
                     .type &
                 TYPE_LAND) == 0)
            {
              if (g_duel_ai_mode_state == 1)
              {
                rewind_recorded_action();
              }
              else
              {
                load_text("promptsX1.txt", "PYRAMIDS");
                set_duel_prompt_text(g_text_lines[3]);
                Sleep(0x9c4);
                set_duel_prompt_text("");
              }
            }
            else
            {
              SET_TARGET(PLAYER_CARD_INSTANCE(player, card).targets[0], target);
              PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
              done = 1;
              if (g_other_player == player)
              {
                if (target.player == g_active_player)
                {
                  g_ai_modifier += 0x30;
                }
                else
                {
                  g_ai_modifier -= 0x30;
                }
              }
            }
          }
          else
          {
            g_spell_fizzled = 1;
          }
        } while (g_spell_fizzled != 1 && done == 0);
      }
    }
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    SET_TARGET(target, PLAYER_CARD_INSTANCE(player, card).targets[0]);
    if ((g_land_can_be_played & LCBP_REGENERATION) != 0)
    {
      if (C_real_validate_target(target.player, target.card, (char *)0, player, 2, 2, TARGET_ZONE_IN_PLAY,
                                 TYPE_LAND, TYPE_NONE, 0, 0, COLOR_TEST_0, COLOR_TEST_0,
                                 -1, -1, -1, -1, 0, 0, 0))
      {
        regenerate_card(target.player, target.card);
      }
      else
      {
        g_spell_fizzled = 1;
      }
    }
    else
    {
      if (C_real_validate_target(target.player, target.card, (char *)0, player, 2, 2, TARGET_ZONE_IN_PLAY,
                                 TYPE_ENCHANTMENT, TYPE_NONE, 0, get_protections_from(player, card),
                                 COLOR_TEST_0, COLOR_TEST_0, -1, -1, -1, -1, 0, 0, 0))
      {
        attached_internal_card_id = -1;
        if ((int)PLAYER_CARD_INSTANCE(target.player, target.card).damage_target_player != -1 &&
            PLAYER_CARD_INSTANCE(target.player, target.card).damage_target_card != -1)
        {
          attached_internal_card_id = DAMAGE_TARGET_CARD_INSTANCE(target.player, target.card).internal_card_id;
        }
        if (attached_internal_card_id != -1 &&
            (global_cards_data[attached_internal_card_id].type & TYPE_LAND))
        {
          kill_card(target.player, target.card, KILL_DESTROY);
        }
        else
        {
          g_spell_fizzled = 1;
        }
      }
      else
      {
        g_spell_fizzled = 1;
      }
    }

    PARENT_CARD_INSTANCE(player, card).number_of_targets = 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x00469d09
// FUNCTION: SHANDALAR 0x00459437
int card_rakalite(int player, int card, event_t event)
{
  target_t target;

  if (event == EVENT_CAN_ACTIVATE)
  {
    return ((g_land_can_be_played & LCBP_DAMAGE_PREVENTION) != 0 &&
            (PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0 &&
            has_mana(player, COLOR_ANY, 2) != 0 &&
            real_target_available((int *)0, TARGET_SCAN_DIRECT, player, 2, 2, TARGET_ZONE_IN_PLAY,
                                  TYPE_NONE, TYPE_NONE, 0, 0, COLOR_TEST_0, COLOR_TEST_0,
                                  g_damage_card_internal_card_id, -1, -1, -1, 0, 0, 0) != 0)
               ? 99
               : 0;
  }

  if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_target(0);
    return 0;
  }

  if (event == EVENT_ACTIVATE)
  {
    charge_mana(player, COLOR_COLORLESS, 2);
    if (g_spell_fizzled != 1)
    {
      load_text("promptsX1.txt", "RAKALITE");
      if (C_real_select_target(player, 2, 2, TARGET_ZONE_IN_PLAY, TYPE_NONE, TYPE_NONE,
                               0, 0, COLOR_TEST_0, COLOR_TEST_0, g_damage_card_internal_card_id,
                               -1, -1, -1, 0, 0, 0, g_text_lines[0], 1, &target) != 0)
      {
        SET_TARGET(PLAYER_CARD_INSTANCE(player, card).targets[0], target);
        PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
        PLAYER_CARD_INSTANCE(player, card).info_slot = 1;
      }
      else
      {
        g_spell_fizzled = 1;
      }
    }
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    SET_TARGET(target, PLAYER_CARD_INSTANCE(player, card).targets[0]);
    if (C_real_validate_target(target.player, target.card, (char *)0, player, 2, 2, TARGET_ZONE_IN_PLAY,
                               TYPE_NONE, TYPE_NONE, 0, 0, COLOR_TEST_0, COLOR_TEST_0,
                               g_damage_card_internal_card_id, -1, -1, -1, 0, 0, 0) != 0)
    {
      if (PLAYER_CARD_INSTANCE(target.player, target.card).info_slot != 0)
      {
        --PLAYER_CARD_INSTANCE(target.player, target.card).info_slot;
      }
    }
    else
    {
      g_spell_fizzled = 1;
    }
    PARENT_CARD_INSTANCE(player, card).number_of_targets = 0;
  }

  if (event == EVENT_CLEANUP && PLAYER_CARD_INSTANCE(player, card).info_slot != 0)
  {
    hurkyls_recall_bounce_artifact(player, card);
  }

  return 0;
}

// FUNCTION: MAGIC 0x0046a138
// FUNCTION: SHANDALAR 0x00459866
int card_ring_of_ma_r_f(int player, int card, event_t event)
{
  int internal_card_id;
  int scratch;

  if (g_trigger_condition == 0xcf &&
      has_mana(player, COLOR_ANY, 5) != 0 &&
      (PLAYER_CARD_INSTANCE(player, card).state & STATE_INVISIBLE) == 0 &&
      (PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0 &&
      (((PLAYER_CARD_INSTANCE(player, card).state & STATE_SUMMONSICK_BOTH) == 0) ||
       ((global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].type & TYPE_CREATURE) == 0)) &&
      card == g_affected_card &&
      player == g_affected_card_controller &&
      player == g_current_turn)
  {
    if (event == EVENT_TRIGGER)
    {
      if (player == g_other_player && (g_duel_network_flags & 2) == 0)
      {
        g_event_result |= RESOLVE_TRIGGER_MANDATORY;
      }
      else
      {
        g_event_result |= RESOLVE_TRIGGER_OPTIONAL;
      }
    }

    if (event == EVENT_RESOLVE_TRIGGER)
    {
      push_card_onto_stack(player, card, EVENT_RESOLVE_ACTIVATION, 0, 0);
      charge_mana(player, COLOR_COLORLESS, 5);
      obliterate_top_card_of_stack();
      if (g_spell_fizzled != 1)
      {
        PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
        internal_card_id = -1;
        if (player == g_active_player || (g_duel_network_flags & 2) != 0)
        {
          if (g_duel_ai_mode_state != 1)
          {
            if (player == g_active_player)
            {
              load_text("promptsX1.txt", "RING_OF_MARUF");
              internal_card_id = pick_internal_card_from_list_dialog(g_text_lines[0], -1, -1);
              if ((g_duel_network_flags & 2) != 0)
              {
                g_network_result_packet.packet_type = 0x1a;
                g_network_result_packet.result = internal_card_id;
                TENTATIVE_send_network_result(0, 0x1a);
              }
            }
            else
            {
              TENTATIVE_wait_for_network_result(1, 0x1a);
              internal_card_id = g_network_result_packet.result;
            }
          }
        }
        else
        {
          scratch = choose_best_card_from_library(player, 0x5b);
          if (scratch == -1)
          {
            internal_card_id = 0;
            scratch = 0;
            while (internal_card_id < g_card_count && scratch == 0)
            {
              if (global_cards_data[internal_card_id].code_pointer == card_city_of_brass)
              {
                scratch = 1;
              }
              ++internal_card_id;
            }
          }
          else
          {
            internal_card_id = global_library[player][scratch];
          }
        }

        scratch = add_card_to_hand(player, internal_card_id);
        if (scratch != -1)
        {
          hurkyls_recall_bounce_artifact(player, scratch);
        }
        kill_card(player, card, KILL_REMOVE);
        g_duel_state_0091c4fc = 1;
      }
      else
      {
        g_spell_fizzled = -1;
      }
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0046a4be
// FUNCTION: SHANDALAR 0x00459be7
int card_rocket_launcher(int player, int card, event_t event)
{
  if (event == EVENT_CAST_SPELL && g_affected_card == card && g_affected_card_controller == player)
  {
    g_ai_modifier += (g_special_mana_pool[g_other_player][COLOR_ANY] - 4) * 12;
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    return (PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0 &&
           has_mana(player, COLOR_ANY, 2) != 0 &&
           (PLAYER_CARD_INSTANCE(player, card).state & STATE_SUMMONSICK_BOTH) == 0;
  }

  if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_target(1);
    return 0;
  }

  if (event == EVENT_ACTIVATE && has_mana(player, COLOR_ANY, 2) != 0)
  {
    charge_mana(player, COLOR_COLORLESS, 2);
    if (g_spell_fizzled != 1)
    {
      load_text("promptsX1.txt", "ROCKET_LAUNCHER");
      select_damage_target(player, card, 1);
    }
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    deal_damage_to_selected_target(player, card, event, 1);

    if (PARENT_CARD_INSTANCE(player, card).internal_card_id != -1)
    {
      PARENT_CARD_INSTANCE(player, card).number_of_targets = 0;
      PARENT_CARD_INSTANCE(player, card).info_slot = 1;
    }
  }

  if ((event == EVENT_CLEANUP || event == EVENT_SHOULD_AI_PLAY) &&
      PLAYER_CARD_INSTANCE(player, card).info_slot != 0)
  {
    kill_card(player, card, KILL_DESTROY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x0046a845
// FUNCTION: SHANDALAR 0x00459f70
int card_sandals_of_abdallah(int player, int card, event_t event)
{
  int legacy_card;
  target_t target;

  if (event == EVENT_CAST_SPELL && card == g_affected_card && player == g_affected_card_controller)
  {
    g_ai_modifier += 0xc;
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    return (PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0 &&
           (((PLAYER_CARD_INSTANCE(player, card).state & STATE_SUMMONSICK_BOTH) == 0) ||
            ((global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].type & TYPE_CREATURE) == 0)) &&
           has_mana(player, COLOR_ANY, 2) != 0 &&
           real_target_available((int *)0, TARGET_SCAN_DIRECT, player, 2, player, TARGET_ZONE_IN_PLAY,
                                 TYPE_CREATURE, TYPE_NONE, 0, get_protections_from(player, card),
                                 COLOR_TEST_0, COLOR_TEST_0, -1, -1, -1, -1, 0, 0, 0) != 0;
  }

  if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_target(0);
    return 0;
  }

  if (event == EVENT_ACTIVATE)
  {
    charge_mana(player, COLOR_COLORLESS, 2);
    if (g_spell_fizzled != 1)
    {
      load_text("promptsX1.txt", "SANDALS_OF_ABDALLAH");
      if (C_real_select_target(player, 2, player, TARGET_ZONE_IN_PLAY, TYPE_CREATURE, TYPE_NONE,
                               0, get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0,
                               -1, -1, -1, -1, 0, 0, 0, g_text_lines[0], 1, &target) != 0)
      {
        SET_TARGET(PLAYER_CARD_INSTANCE(player, card).targets[0], target);
        PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
        PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
      }
      else
      {
        g_spell_fizzled = 1;
      }
    }
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    SET_TARGET(target, PLAYER_CARD_INSTANCE(player, card).targets[0]);
    if (C_real_validate_target(target.player, target.card, (char *)0, player, 2, 2, TARGET_ZONE_IN_PLAY,
                               TYPE_CREATURE, TYPE_NONE, 0, 0, COLOR_TEST_0, COLOR_TEST_0,
                               -1, -1, -1, -1, 0, 0, 0) != 0)
    {
      legacy_card = create_legacy_effect(g_card_on_stack_controller, g_card_on_stack, g_duel_generated_internal_card_id_03,
                                         target.player, target.card);
      if (legacy_card != -1)
      {
        PLAYER_CARD_INSTANCE(player, legacy_card).info_slot = 1 << (get_hacked_color(player, card, COLOR_BLUE) - 1);
      }
      PLAYER_CARD_INSTANCE(target.player, target.card).regen_status = 0x8000000;
    }
    else
    {
      g_spell_fizzled = 1;
    }
    PARENT_CARD_INSTANCE(player, card).number_of_targets = 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0046ad1a
// FUNCTION: SHANDALAR 0x0045a443
int card_staff_of_zegon(int player, int card, event_t event)
{
  int legacy_card;
  target_t target;

  if (event == EVENT_CAN_ACTIVATE)
  {
    return (PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0 &&
           (((PLAYER_CARD_INSTANCE(player, card).state & STATE_SUMMONSICK_BOTH) == 0) ||
            ((global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].type & TYPE_CREATURE) == 0)) &&
           has_mana(player, COLOR_ANY, 3) != 0 &&
           real_target_available((int *)0, TARGET_SCAN_DIRECT, player, 2, player, TARGET_ZONE_IN_PLAY,
                                 TYPE_CREATURE, TYPE_NONE, 0, get_protections_from(player, card),
                                 COLOR_TEST_0, COLOR_TEST_0, -1, -1, -1, -1, 0, 0, 0) != 0;
  }

  if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_target(0);
    return 0;
  }

  if (event == EVENT_ACTIVATE)
  {
    charge_mana(player, COLOR_COLORLESS, 3);
    if (g_spell_fizzled != 1)
    {
      load_text("promptsX1.txt", "STAFF_OF_ZEGON");
      if (C_real_select_target(player, 2, 1 - player, TARGET_ZONE_IN_PLAY, TYPE_CREATURE, TYPE_NONE,
                               0, get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0,
                               -1, -1, -1, -1, 0, 0, 0, g_text_lines[0], 1, &target))
      {
        SET_TARGET(PLAYER_CARD_INSTANCE(player, card).targets[0], target);
        PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
        PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
      }
      else
      {
        g_spell_fizzled = 1;
      }
    }
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    SET_TARGET(target, PLAYER_CARD_INSTANCE(player, card).targets[0]);
    if (C_real_validate_target(target.player, target.card, (char *)0, player, 2, 2, TARGET_ZONE_IN_PLAY,
                               TYPE_CREATURE, TYPE_NONE, 0, 0, COLOR_TEST_0, COLOR_TEST_0,
                               -1, -1, -1, -1, 0, 0, 0))
    {
      legacy_card = create_legacy_effect(g_card_on_stack_controller, g_card_on_stack, LEGACY_EFFECT_PUMP,
                                         target.player, target.card);
      if (legacy_card != -1)
      {
        PLAYER_CARD_INSTANCE(player, legacy_card).counter_power = -2;
        PLAYER_CARD_INSTANCE(player, legacy_card).counter_toughness = 0;
      }
    }
    else
    {
      g_spell_fizzled = 1;
    }
    PARENT_CARD_INSTANCE(player, card).number_of_targets = 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0046b1ab
// FUNCTION: SHANDALAR 0x0045a8d4
int card_su_chi(int player, int card, event_t event)
{
  if (event == EVENT_GRAVEYARD_FROM_PLAY &&
      g_affected_card == card &&
      g_affected_card_controller == player &&
      (PLAYER_CARD_INSTANCE(player, card).state & STATE_INVISIBLE) == 0 &&
      PLAYER_CARD_INSTANCE(player, card).kill_code != KILL_REMOVE)
  {
    produce_mana(player, COLOR_COLORLESS, 4);
  }

  return 0;
}

// FUNCTION: MAGIC 0x0046b262
// FUNCTION: SHANDALAR 0x0045a98b
int card_tablet_of_epityr(int player, int card, event_t event)
{
  int affected_internal_card_id;

  if (event == EVENT_GRAVEYARD_FROM_PLAY &&
      (PLAYER_CARD_INSTANCE(player, card).state & STATE_INVISIBLE) == 0 &&
      (PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).state & STATE_INVISIBLE) == 0 &&
      g_affected_card_controller == player)
  {
    if (PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).internal_card_id != -1)
    {
      affected_internal_card_id =
          PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).internal_card_id;
    }
    else
    {
      affected_internal_card_id =
          PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).original_internal_card_id;
    }

    if ((global_cards_data[affected_internal_card_id].type & TYPE_ARTIFACT) != 0 &&
        PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).kill_code != 0 &&
        PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).kill_code != KILL_REMOVE &&
        (((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0) ||
         ((global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].type & TYPE_CREATURE) != 0)))
    {
      if ((PLAYER_CARD_INSTANCE(player, card).info_slot & 0x100) != 0)
      {
        PLAYER_CARD_INSTANCE(player, card).info_slot = 1;
      }
      else
      {
        ++PLAYER_CARD_INSTANCE(player, card).info_slot;
      }
    }
  }

  if (g_trigger_condition == TRIGGER_GRAVEYARD_FROM_PLAY &&
      g_affected_card == card &&
      g_affected_card_controller == player &&
      (PLAYER_CARD_INSTANCE(player, card).info_slot & 0xff) != 0 &&
      player == g_current_turn &&
      (((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0) ||
       ((global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].type & TYPE_CREATURE) != 0)))
  {
    PLAYER_CARD_INSTANCE(player, card).info_slot |= 0x100;
    if (event == EVENT_TRIGGER)
    {
      if (has_mana(player, COLOR_ANY, 1) != 0)
      {
        if (g_other_player == player && (g_duel_network_flags & 2) == 0)
        {
          g_event_result |= RESOLVE_TRIGGER_MANDATORY;
        }
        else
        {
          g_event_result |= RESOLVE_TRIGGER_OPTIONAL;
        }
      }
    }
    if (event == EVENT_RESOLVE_TRIGGER)
    {
      push_card_onto_stack(player, card, EVENT_RESOLVE_ACTIVATION, 0, 0);
      charge_mana(player, COLOR_COLORLESS, 1);
      obliterate_top_card_of_stack();
      if (g_spell_fizzled != 1)
      {
        dispatch_event(player, card, EVENT_PLAY_ABILITY);
        gain_life(player, 1, player, card);
        --PLAYER_CARD_INSTANCE(player, card).info_slot;
      }
      else
      {
        g_spell_fizzled = -1;
      }
    }
    if ((PLAYER_CARD_INSTANCE(player, card).info_slot & 0xff) != 0)
    {
      PLAYER_CARD_INSTANCE(player, card).state &= ~STATE_PROCESSING;
    }
  }

  return 0;
}

// FUNCTION: SHANDALAR 0x00488638
int tawnos_coffin_phase_out_one(int target_player, int target_card)
{
  int saved_trigger_cause;
  int saved_trigger_cause_controller;

  saved_trigger_cause_controller = g_trigger_cause_controller;
  saved_trigger_cause = g_trigger_cause;
  if (target_player == -1 || target_card == -1)
  {
    return 0;
  }

  push_affected_card_stack();
  g_trigger_cause_controller = target_player;
  g_trigger_cause = target_card;
  dispatch_trigger_twice_once_with_each_player_as_reason(g_current_player, TRIGGER_LEAVE_PLAY,
                                                         gs_card_leaving_play_007aaef0, 0);
  g_trigger_cause_controller = saved_trigger_cause_controller;
  g_trigger_cause = saved_trigger_cause;
  pop_affected_card_stack();
  PLAYER_CARD_INSTANCE(target_player, target_card).damage_on_card = 0;
  PLAYER_CARD_INSTANCE(target_player, target_card).blocking = (char)-1;
  PLAYER_CARD_INSTANCE(target_player, target_card).state &= ~STATE_ATTACKING;
  PLAYER_CARD_INSTANCE(target_player, target_card).state |= STATE_OUBLIETTED;
  TENTATIVE_reassess_all_cards(0, 0x20);

  return 0;
}

// FUNCTION: SHANDALAR 0x004884c1
int tawnos_coffin_phase_in_one(int target_player, int target_card)
{
  int saved_trigger_cause;
  int saved_trigger_cause_controller;

  saved_trigger_cause = g_trigger_cause;
  saved_trigger_cause_controller = g_trigger_cause_controller;
  if (target_player != -1 && target_card != -1)
  {
    PLAYER_CARD_INSTANCE(target_player, target_card).state &= ~STATE_OUBLIETTED;
    push_affected_card_stack();
    g_trigger_cause_controller = target_player;
    g_trigger_cause = target_card;
    dispatch_trigger_twice_once_with_each_player_as_reason(g_current_player, TRIGGER_COMES_INTO_PLAY,
                                                           gs_card_into_play_0091c840, 0);
    g_trigger_cause_controller = saved_trigger_cause_controller;
    g_trigger_cause = saved_trigger_cause;
    pop_affected_card_stack();
    PLAYER_CARD_INSTANCE(target_player, target_card).state |= STATE_TAPPED;
    PLAYER_CARD_INSTANCE(target_player, target_card).regen_status |= 0x6000000;
    TENTATIVE_reassess_all_cards(0, 0x20);
  }

  return 0;
}

// FUNCTION: SHANDALAR 0x0048889d
int tawnos_coffin_phase_attachment_callback(int source_player,
                                            int source_card,
                                            int target_player,
                                            int target_card,
                                            int target_internal_card_id)
{
  int current_card;
  int current_player;
  int found;
  int next_card;
  int next_player;
  int target_card_id;

  current_player = target_player;
  current_card = target_card;
  found = 0;
  if (global_cards_data[target_internal_card_id].id != 0x1bf)
  {
    while ((int)PLAYER_CARD_INSTANCE(current_player, current_card).damage_target_player != -1 &&
           found == 0)
    {
      if ((int)PLAYER_CARD_INSTANCE(current_player, current_card).damage_target_player == source_player &&
          PLAYER_CARD_INSTANCE(current_player, current_card).damage_target_card == source_card)
      {
        found = 1;
      }
      next_player = (int)PLAYER_CARD_INSTANCE(current_player, current_card).damage_target_player;
      next_card = PLAYER_CARD_INSTANCE(current_player, current_card).damage_target_card;
      if (next_player == -1)
      {
        break;
      }
      current_player = next_player;
      current_card = next_card;
    }
  }

  if (found != 0)
  {
    target_card_id = global_cards_data[target_internal_card_id].id;
    if (global_raw_cards_storage[target_card_id].card_type == CP_TYPE_ENCHANTMENT &&
        global_raw_cards_storage[target_card_id].subtype != 0xcd &&
        global_raw_cards_storage[target_card_id].subtype != 0xd3)
    {
      if ((PLAYER_CARD_INSTANCE(target_player, target_card).state & STATE_OUBLIETTED) == 0)
      {
        tawnos_coffin_phase_out_one(target_player, target_card);
      }
      else
      {
        tawnos_coffin_phase_in_one(target_player, target_card);
      }
    }
    else if ((global_cards_data[target_internal_card_id].type & TYPE_EFFECT) != 0)
    {
      if ((PLAYER_CARD_INSTANCE(target_player, target_card).token_status & STATUS_PERMANENT) == 0)
      {
        kill_card(target_player, target_card, KILL_DESTROY);
      }
      else
      {
        PLAYER_CARD_INSTANCE(target_player, target_card).state ^= STATE_OUBLIETTED;
      }
    }
  }

  return 0;
}

// FUNCTION: SHANDALAR 0x00488789
int tawnos_coffin_phase_in_attached(int target_player, int target_card)
{
  int current_card;
  int current_player;

  for (current_player = 0; current_player < 2; ++current_player)
  {
    for (current_card = 0; current_card < g_active_cards_count[current_player]; ++current_card)
    {
      if (PLAYER_CARD_INSTANCE(current_player, current_card).internal_card_id != -1 &&
          (PLAYER_CARD_INSTANCE(current_player, current_card).state &
           (STATE_OUBLIETTED | STATE_INVISIBLE | STATE_IN_PLAY)) == (STATE_OUBLIETTED | STATE_IN_PLAY))
      {
        tawnos_coffin_phase_attachment_callback(target_player,
                                                target_card,
                                                current_player,
                                                current_card,
                                                PLAYER_CARD_INSTANCE(current_player, current_card).internal_card_id);
      }
    }
  }

  return 0;
}

// FUNCTION: SHANDALAR 0x004885e4
int tawnos_coffin_phase_out(int target_player, int target_card)
{
  if (target_player != -1 && target_card != -1)
  {
    dispatch_function_to_all_cards_in_play(target_player, target_card,
                                           tawnos_coffin_phase_attachment_callback, -1);
    tawnos_coffin_phase_out_one(target_player, target_card);
  }

  return 0;
}

// FUNCTION: SHANDALAR 0x00488474
int tawnos_coffin_phase_in(int target_player, int target_card)
{
  if (target_player != -1 && target_card != -1)
  {
    tawnos_coffin_phase_in_attached(target_player, target_card);
    tawnos_coffin_phase_in_one(target_player, target_card);
  }

  return 0;
}

// FUNCTION: MAGIC 0x0046b7c7
// FUNCTION: SHANDALAR 0x0045aef3
int card_tawnos_s_coffin(int player, int card, event_t event)
{
  struct
  {
    int current_player;
    int exiled_power;
    int current_card;
  } s;

  if (event == EVENT_UNTAP && g_affected_card == card && g_affected_card_controller == player)
  {
    PLAYER_CARD_INSTANCE(player, card).untap_status &= ~2;
  }

  if (g_current_phase == PHASE_UNTAP && g_affected_card == card && g_affected_card_controller == player)
  {
    if (event == EVENT_TRIGGER &&
        (PLAYER_CARD_INSTANCE(player, card).untap_status & 1) != 0 &&
        (PLAYER_CARD_INSTANCE(player, card).untap_status & 2) == 0 &&
        (g_ai_score_baseline & global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].type) == 0)
    {
      if ((g_other_player == player && (g_duel_network_flags & 2) == 0) ||
          g_duel_ai_mode_state == 1 ||
          g_duel_network_state != 0)
      {
        if ((int)PLAYER_CARD_INSTANCE(player, card).damage_source_player == -1 ||
            (int)PLAYER_CARD_INSTANCE(player, card).damage_source_player == g_other_player)
        {
          g_event_result |= RESOLVE_TRIGGER_MANDATORY;
        }
        else
        {

          s.exiled_power =
              PLAYER_CARD_INSTANCE(
                  (int)DAMAGE_SOURCE_CARD_INSTANCE(player, card).damage_target_player,
                  DAMAGE_SOURCE_CARD_INSTANCE(player, card).damage_target_card)
                  .power;

          s.current_player = 1 - player;
          for (s.current_card = 0;
               s.current_card < g_active_cards_count[s.current_player] && g_event_result == 0;
               ++s.current_card)
          {
            if (is_in_play(s.current_player, s.current_card) != 0 &&
                (global_cards_data[PLAYER_CARD_INSTANCE(s.current_player, s.current_card).internal_card_id].type &
                 TYPE_CREATURE) != 0 &&
                s.exiled_power < PLAYER_CARD_INSTANCE(s.current_player, s.current_card).power)
            {
              g_event_result |= RESOLVE_TRIGGER_MANDATORY;
            }
          }
        }
      }
      else
      {
        g_event_result |= RESOLVE_TRIGGER_OPTIONAL;
      }
    }

    if (event == EVENT_RESOLVE_TRIGGER)
    {
      PLAYER_CARD_INSTANCE(player, card).untap_status |= 2;
    }
  }

  if (event == EVENT_CAST_SPELL && g_affected_card == card && g_affected_card_controller == player)
  {
    PLAYER_CARD_INSTANCE(player, card).info_slot = -1;
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    return ((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0 &&
            (((PLAYER_CARD_INSTANCE(player, card).state & STATE_SUMMONSICK_BOTH) == 0) ||
             ((global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].type & TYPE_CREATURE) == 0)) &&
            has_mana(player, COLOR_ANY, 3) != 0 &&
            real_target_available((int *)0, TARGET_SCAN_DIRECT, player, 2, 2, TARGET_ZONE_IN_PLAY,
                                  TYPE_CREATURE, TYPE_NONE, 0, get_protections_from(player, card),
                                  COLOR_TEST_0, COLOR_TEST_0, -1, -1, -1, -1, 0, 0, 0) != 0);
  }

  if (event == EVENT_ACTIVATE)
  {
    charge_mana(player, COLOR_COLORLESS, 3);
    if (g_spell_fizzled != 1)
    {
      load_text("promptsX1.txt", "TAWNOS_COFFIN");
      if (select_target_creature_and_store(player, 2, card) != 0)
      {
        PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
      }
      else
      {
        g_spell_fizzled = 1;
      }
    }
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    if (PARENT_CARD_INSTANCE(player, card).internal_card_id != -1)
    {
      if (C_real_validate_target(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                                 PLAYER_CARD_INSTANCE(player, card).targets[0].card,
                                 (char *)0, player, 2, 2, TARGET_ZONE_IN_PLAY,
                                 TYPE_CREATURE, TYPE_NONE, 0, get_protections_from(player, card),
                                 COLOR_TEST_0, COLOR_TEST_0, -1, -1, -1, -1, 0, 0, 0) != 0)
      {
        PARENT_CARD_INSTANCE(player, card).damage_source_player = (char)PLAYER_CARD_INSTANCE(player, card).targets[0].player;
        PARENT_CARD_INSTANCE(player, card).damage_source_card = PLAYER_CARD_INSTANCE(player, card).targets[0].card;
        PARENT_CARD_INSTANCE(player, card).info_slot = PLAYER_CARD_INSTANCE(player, card).targets[0].card;
        tawnos_coffin_phase_out(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                                PLAYER_CARD_INSTANCE(player, card).targets[0].card);
      }
      else
      {
        g_spell_fizzled = 1;
      }
      PARENT_CARD_INSTANCE(player, card).number_of_targets = 0;
    }
  }

  if (g_trigger_condition == TRIGGER_LEAVE_PLAY &&
      g_affected_card == card &&
      g_affected_card_controller == player &&
      player == g_current_turn &&
      PLAYER_CARD_INSTANCE(player, card).info_slot != -1 &&
      (int)PLAYER_CARD_INSTANCE(player, card).damage_source_player != -1 &&
      PLAYER_CARD_INSTANCE(player, card).damage_source_card != -1 &&
      g_trigger_cause_controller == player &&
      g_trigger_cause == card)
  {
    if (event == EVENT_TRIGGER)
    {
      g_event_result |= RESOLVE_TRIGGER_MANDATORY;
    }
    if (event == EVENT_RESOLVE_TRIGGER)
    {
      tawnos_coffin_phase_in((int)(char)PLAYER_CARD_INSTANCE(player, card).damage_source_player, PLAYER_CARD_INSTANCE(player, card).damage_source_card);
    }
  }

  if (event == EVENT_GRAVEYARD_FROM_PLAY)
  {
    if (g_affected_card == card &&
        g_affected_card_controller == player &&
        PLAYER_CARD_INSTANCE(player, card).info_slot != -1)
    {
      tawnos_coffin_phase_in((int)(char)PLAYER_CARD_INSTANCE(player, card).damage_source_player, PLAYER_CARD_INSTANCE(player, card).damage_source_card);
      PLAYER_CARD_INSTANCE(player, card).info_slot = -1;
    }

    if ((int)PLAYER_CARD_INSTANCE(player, card).damage_source_player == g_affected_card_controller &&
        PLAYER_CARD_INSTANCE(player, card).damage_source_card == g_affected_card)
    {
      PLAYER_CARD_INSTANCE(player, card).info_slot = -1;
      PLAYER_CARD_INSTANCE(player, card).damage_source_card =
          PLAYER_CARD_INSTANCE(player, card).info_slot;
      PLAYER_CARD_INSTANCE(player, card).damage_source_player =
          (char)PLAYER_CARD_INSTANCE(player, card).damage_source_card;
    }
  }

  if (PLAYER_CARD_INSTANCE(player, card).info_slot != -1 &&
      (PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0)
  {
    PLAYER_CARD_INSTANCE(player, card).info_slot = -1;
    tawnos_coffin_phase_in((int)(char)PLAYER_CARD_INSTANCE(player, card).damage_source_player, PLAYER_CARD_INSTANCE(player, card).damage_source_card);
    PLAYER_CARD_INSTANCE(player, card).damage_source_card = -1;
    PLAYER_CARD_INSTANCE(player, card).damage_source_player =
        (char)PLAYER_CARD_INSTANCE(player, card).damage_source_card;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0046c7dd
// FUNCTION: SHANDALAR 0x0045bf15
int card_urza_s_chalice(int player, int card, event_t event)
{
  if (event == EVENT_CAST_SPELL && g_affected_card == card && g_affected_card_controller == player)
  {
    g_ai_modifier += g_basiclandtypes_controlled[g_other_player][COLOR_ANY] * 0xc;
  }

  if (g_trigger_condition == TRIGGER_SPELL_CAST &&
      g_affected_card == card &&
      g_affected_card_controller == player &&
      player == g_current_turn &&
      (((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0) ||
       ((global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].type & TYPE_CREATURE) != 0)) &&
      (PLAYER_CARD_INSTANCE(player, card).state & STATE_INVISIBLE) == 0 &&
      has_mana(player, COLOR_ANY, 1) != 0 &&
      PLAYER_CARD_INSTANCE(g_trigger_cause_controller, g_trigger_cause).internal_card_id != -1 &&
      ((global_cards_data[PLAYER_CARD_INSTANCE(g_trigger_cause_controller, g_trigger_cause).internal_card_id].type &
        TYPE_ARTIFACT) != 0))
  {
    if (event == EVENT_TRIGGER)
    {
      if (g_other_player == player && (g_duel_network_flags & 2) == 0)
      {
        g_event_result |= RESOLVE_TRIGGER_MANDATORY;
      }
      else
      {
        g_event_result |= RESOLVE_TRIGGER_OPTIONAL;
      }
    }
    if (event == EVENT_RESOLVE_TRIGGER)
    {
      push_card_onto_stack(player, card, EVENT_RESOLVE_ACTIVATION, 0, 0);
      charge_mana(player, COLOR_COLORLESS, 1);
      obliterate_top_card_of_stack();
      if (g_spell_fizzled != 1)
      {
        dispatch_event(player, card, EVENT_PLAY_ABILITY);
        gain_life(player, 1, player, card);
        if (g_other_player == player)
        {
          g_ai_modifier -= 0x18;
        }
      }
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0046ca6f
// FUNCTION: SHANDALAR 0x0045c1a8
int card_urza_s_miter(int player, int card, event_t event)
{
  int done;

  if (event == EVENT_GRAVEYARD_FROM_PLAY &&
      player == g_affected_card_controller &&
      (((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0) ||
       ((global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].type & TYPE_CREATURE) != 0)) &&
      (global_cards_data[PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).internal_card_id].type & TYPE_ARTIFACT) != 0 &&
      (PLAYER_CARD_INSTANCE(player, card).state & STATE_INVISIBLE) == 0 &&
      PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).kill_code != 0 &&
      PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).kill_code != KILL_REMOVE &&
      PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).kill_code != KILL_SACRIFICE)
  {
    ++PLAYER_CARD_INSTANCE(player, card).info_slot;
  }

  if (g_trigger_condition == TRIGGER_GRAVEYARD_FROM_PLAY &&
      card == g_affected_card &&
      player == g_affected_card_controller &&
      player == g_current_turn &&
      PLAYER_CARD_INSTANCE(player, card).info_slot != 0 &&
      player == g_trigger_cause_controller &&
      (((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0) ||
       ((global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].type & TYPE_CREATURE) != 0)) &&
      has_mana(player, COLOR_ANY, 3) != 0)
  {
    if (event == EVENT_TRIGGER)
    {
      if (player == g_other_player && (g_duel_network_flags & 2) == 0)
      {
        g_event_result |= RESOLVE_TRIGGER_MANDATORY;
      }
      else
      {
        g_event_result |= RESOLVE_TRIGGER_OPTIONAL;
      }
    }
    if (event == EVENT_RESOLVE_TRIGGER)
    {
      done = 0;
      do
      {
        if (has_mana(player, COLOR_ANY, 3) != 0)
        {
          push_card_onto_stack(player, card, EVENT_RESOLVE_ACTIVATION, 0, 0);
          charge_mana(player, COLOR_COLORLESS, 3);
          obliterate_top_card_of_stack();
          if (g_spell_fizzled != 1)
          {
            dispatch_event(player, card, EVENT_PLAY_ABILITY);
            draw_card_for_player(player);
            --PLAYER_CARD_INSTANCE(player, card).info_slot;
          }
          else
          {
            done = 1;
          }
        }
        else
        {
          done = 1;
        }
      } while (done == 0 && PLAYER_CARD_INSTANCE(player, card).info_slot != 0);
      PLAYER_CARD_INSTANCE(player, card).info_slot = 0;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0046cef9
// FUNCTION: SHANDALAR 0x0045c62c
int card_weakstone(int player, int card, event_t event)
{
  if (event == EVENT_POWER &&
      (((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0) ||
       ((global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].type & TYPE_CREATURE) != 0)) &&
      (PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).state & STATE_ATTACKING) != 0 &&
      g_affected_card_controller == g_current_player &&
      is_in_play(player, card) != 0)
  {
    --g_event_result;
  }

  return 0;
}
