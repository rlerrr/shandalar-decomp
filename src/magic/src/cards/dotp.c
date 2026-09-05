#include <stdio.h>
#include <string.h>
#include "../game_support.h"
#include "../global_duel_ui_ids.h"
#include "../global_state.h"
#include "../global_strings.h"

extern card_ptr_t global_raw_cards_storage[2000];
extern int combat_eval_abilities;

int check_duel_finished(void);
#ifdef SHANDALAR
int SelectAdventureListCardIndex(int player, int *card_ids, int card_count, char *title,
                                 int require_card_click, int *out_selection);
#endif

// FUNCTION: SHANDALAR 0x004323cc
static int bone_flute_effect(int player, int card, int target_player, int target_card, int internal_card_id)
{
  int legacy_card;

  if ((global_cards_data[internal_card_id].type & TYPE_CREATURE) != 0)
  {
    legacy_card = create_legacy_effect(player, card, LEGACY_EFFECT_PUMP, target_player, target_card);
    if (legacy_card != -1)
    {
      PLAYER_CARD_INSTANCE(player, legacy_card).counter_power = -1;
    }
  }
  return 0;
}

static __inline int sacrifice_basic_land_type_for_mold_demon(int player, int land_color)
{
  int current_card;

  for (current_card = 0; current_card < g_active_cards_count[player]; ++current_card)
  {
    if (is_in_play(player, current_card) &&
        PLAYER_CARD_INSTANCE(player, current_card).kill_code == 0 &&
        card_has_basic_land_type(player, current_card, land_color) != 0)
    {
      if (g_duel_ai_mode_state != 1)
      {
        play_sound_effect(WAV_SACRFICE);
      }
      kill_card(player, current_card, KILL_SACRIFICE);
      return 1;
    }
  }

  return 0;
}

static __inline int pay_mold_demon_upkeep(int player, int card)
{
  int color;

  color = get_hacked_color(player, card, COLOR_BLACK);
  if (g_basiclandtypes_controlled[player][color] >= 2 &&
      sacrifice_basic_land_type_for_mold_demon(player, color) != 0 &&
      sacrifice_basic_land_type_for_mold_demon(player, color) != 0)
  {
    return 1;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0043203f
// FUNCTION: SHANDALAR 0x00440d44
int TENTATIVE_sacrifice_basic_land_type(int player, int card, int count, int land_color, int allow_cancel)
{
  target_t targets[20];
  char prompt[300];
  int selected_count;
  int cleanup_index;
  int land_type_index;

  if (count > 20)
  {
    count = 20;
  }

  land_type_index = land_color - 1;
  if (g_duel_ai_mode_state != 1)
  {
    load_text("promptsX2.txt", "SACRIFICE_X_BASICLAND");
    strcpy(prompt, g_text_lines[land_type_index]);
  }

  selected_count = 0;
  while (selected_count < count && g_spell_fizzled != 1)
  {
    if (C_real_select_target(player,
                             player,
                             player,
                             TARGET_ZONE_IN_PLAY,
                             TYPE_NONE,
                             TYPE_NONE,
                             0,
                             0,
                             COLOR_TEST_0,
                             COLOR_TEST_0,
                             land_type_index,
                             -1,
                             -1,
                             -1,
                             0,
                             0,
                             0,
                             prompt,
                             allow_cancel,
                             &targets[selected_count]) == 0)
    {
      if (targets[selected_count].card != -2)
      {
        for (cleanup_index = 0; cleanup_index < selected_count; ++cleanup_index)
        {
          PLAYER_CARD_INSTANCE(targets[cleanup_index].player, targets[cleanup_index].card).state &=
              ~(STATE_CANNOT_TARGET | STATE_TARGETTED);
        }
      }
      g_spell_fizzled = 1;
    }
    else
    {
      PLAYER_CARD_INSTANCE(targets[selected_count].player, targets[selected_count].card).state |=
          STATE_CANNOT_TARGET | STATE_TARGETTED;
    }
    TENTATIVE_reassess_all_cards(0, 0x20);
    ++selected_count;
  }

  if (g_spell_fizzled == 1)
  {
    g_spell_fizzled = -1;
    return 0;
  }

  for (selected_count = 0; selected_count < count; ++selected_count)
  {
    kill_card(targets[selected_count].player, targets[selected_count].card, KILL_SACRIFICE);
    if (g_duel_ai_mode_state != 1)
    {
      play_sound_effect(WAV_SACRFICE);
    }
  }

  return 1;
}

static __inline int is_aura_attached_to_type(int aura_player, int aura_card, int attached_controller, unsigned int attached_type)
{
  int attached_player;
  int attached_card;

  if (!is_in_play(aura_player, aura_card) ||
      (global_cards_data[PLAYER_CARD_INSTANCE(aura_player, aura_card).internal_card_id].type & TYPE_ENCHANTMENT) == 0)
  {
    return 0;
  }

  attached_player = PLAYER_CARD_INSTANCE(aura_player, aura_card).damage_target_player;
  attached_card = PLAYER_CARD_INSTANCE(aura_player, aura_card).damage_target_card;
  if (attached_player == -1 || attached_card == -1 || !is_in_play(attached_player, attached_card))
  {
    return 0;
  }

  if (attached_controller != 2 && attached_player != attached_controller)
  {
    return 0;
  }

  return (global_cards_data[PLAYER_CARD_INSTANCE(attached_player, attached_card).internal_card_id].type & attached_type) != 0;
}

static __inline int find_aura_attached_to_type(target_t *target, int attached_controller, unsigned int attached_type)
{
  int current_player;
  int current_card;

  for (current_player = 0; current_player < 2; ++current_player)
  {
    for (current_card = 0; current_card < g_active_cards_count[current_player]; ++current_card)
    {
      if (is_aura_attached_to_type(current_player, current_card, attached_controller, attached_type))
      {
        target->player = current_player;
        target->card = current_card;
        return 1;
      }
    }
  }

  return 0;
}

// FUNCTION: SHANDALAR 0x0043919e
int savaen_elves_has_target(void)
{
  int current_player;
  int current_card;
  int result;
  int internal_card_id;

  current_player = 0;
  result = 0;
  while (current_player < 2 && result == 0)
  {
    current_card = 0;
    while (current_card < g_active_cards_count[current_player] && result == 0)
    {
      internal_card_id = PLAYER_CARD_INSTANCE(current_player, current_card).internal_card_id;
      if (is_in_play(current_player, current_card) &&
          global_raw_cards_storage[global_cards_data[internal_card_id].id].card_type == CP_TYPE_ENCHANTMENT &&
          global_raw_cards_storage[global_cards_data[internal_card_id].id].subtype == 0x6c)
      {
        result = 1;
      }
      ++current_card;
    }
    ++current_player;
  }

  return result;
}

// FUNCTION: SHANDALAR 0x00440b8a
int miracle_worker_has_target(int player)
{
  int current_player;
  int current_card;
  int result;
  int attached_player;
  int attached_card;
  int attached_internal_card_id;

  current_player = 0;
  result = 0;
  while (current_player < 2 && result == 0)
  {
    current_card = 0;
    while (current_card < g_active_cards_count[current_player] && result == 0)
    {
      attached_player = PLAYER_CARD_INSTANCE(current_player, current_card).damage_target_player;
      attached_card = PLAYER_CARD_INSTANCE(current_player, current_card).damage_target_card;
      if (is_in_play(current_player, current_card) &&
          (global_cards_data[PLAYER_CARD_INSTANCE(current_player, current_card).internal_card_id].type & TYPE_ENCHANTMENT) != 0 &&
          attached_player == player &&
          (attached_internal_card_id = PLAYER_CARD_INSTANCE(attached_player, attached_card).internal_card_id) != -1 &&
          (global_cards_data[attached_internal_card_id].type & TYPE_CREATURE) != 0)
      {
        result = 1;
      }
      ++current_card;
    }
    ++current_player;
  }

  return result;
}

// FUNCTION: MAGIC 0x00422c40
// FUNCTION: SHANDALAR 0x00431920
int card_alchor_s_tomb(int player, int card, event_t event)
{
  target_t target;
  int color;
  int choice_hint;

  if (event == EVENT_CAN_ACTIVATE)
  {
    return has_mana(player, COLOR_ANY, 2) &&
           (((PLAYER_CARD_INSTANCE(player, card).state & STATE_SUMMONSICK_BOTH) == 0) ||
            ((global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].type & TYPE_CREATURE) == 0)) &&
           (PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0;
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
      if (g_duel_ai_mode_state != 1)
      {
        load_text("promptsX2.txt", "ALCHORS_TOMB");
      }
      if (C_real_select_target(player, player, player, TARGET_ZONE_IN_PLAY,
                               TARGET_TYPE_PERMANENT, TYPE_NONE, 0,
                               get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0,
                               -1, -1, -1, -1, 0, 0, 0, g_text_lines[0], 1, &target) == 0)
      {
        g_spell_fizzled = 1;
      }
      else
      {
        if (g_other_player == player && (g_duel_network_flags & 2) == 0 && g_duel_ai_mode_state == 1)
        {
          g_ai_recorded_choice = internal_rand(5) + 1;
          record_ai_action_selection();
        }
        else if (g_other_player == player && (g_duel_network_flags & 2) == 0)
        {
          replay_ai_action_selection();
        }
        choice_hint = -1;
        if (g_other_player == player && (g_duel_network_flags & 2) == 0)
        {
          choice_hint = g_ai_recorded_choice;
        }
        color = choose_a_color(player, g_text_lines[1], 1, choice_hint, 0x3e);
        PLAYER_CARD_INSTANCE(player, card).info_slot = color;
        if (PLAYER_CARD_INSTANCE(player, card).info_slot == -1)
        {
          g_spell_fizzled = 1;
        }
        else
        {
          SET_TARGET(PLAYER_CARD_INSTANCE(player, card).targets[0], target);
          PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
          PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
        }
      }
    }
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    SET_TARGET(target, PLAYER_CARD_INSTANCE(player, card).targets[0]);
    if (C_real_validate_target(target.player, target.card, (char *)0, player, player, player,
                               TARGET_ZONE_IN_PLAY, TARGET_TYPE_PERMANENT, TYPE_NONE, 0,
                               get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0,
                               -1, -1, -1, -1, 0, 0, 0) == 0)
    {
      g_spell_fizzled = 1;
    }
    else
    {
      PLAYER_CARD_INSTANCE(target.player, target.card).color =
          (char)(1 << (unsigned char)PLAYER_CARD_INSTANCE(player, card).info_slot);
      if (g_duel_ai_mode_state != 1)
      {
        play_sound_effect(0x27);
      }
    }
    PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                         PLAYER_CARD_INSTANCE(player, card).parent_card)
        .number_of_targets = 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x00423198
// FUNCTION: SHANDALAR 0x00431e7a
int card_barl_s_cage(int player, int card, event_t event)
{
  target_t target;

  if (event == EVENT_CAN_ACTIVATE)
  {
    return has_mana(player, COLOR_ANY, 3) &&
           (((PLAYER_CARD_INSTANCE(player, card).state & STATE_SUMMONSICK_BOTH) == 0) ||
            ((global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].type & TYPE_CREATURE) == 0)) &&
           real_target_available((int *)0, TARGET_SCAN_DIRECT, player, 2, 2, TARGET_ZONE_IN_PLAY,
                                 TYPE_CREATURE, TYPE_NONE, 0, get_protections_from(player, card),
                                 COLOR_TEST_0, COLOR_TEST_0, -1, -1, -1, -1, 0, 0, 0) != 0;
  }
  else if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_target(0);
  }
  else
  {
    if (event == EVENT_ACTIVATE)
    {
      charge_mana(player, COLOR_COLORLESS, 3);
      if (g_spell_fizzled != 1)
      {
        load_text("promptsX2.txt", "BARLS_CAGE");
        if (C_real_select_target(player, 2, 2, TARGET_ZONE_IN_PLAY, TYPE_CREATURE, TYPE_NONE, 0,
                                 0, COLOR_TEST_0, COLOR_TEST_0, -1, -1, -1, -1, 0, 0, 0,
                                 g_text_lines[0], 1, &target) != 0)
        {
          SET_TARGET(PLAYER_CARD_INSTANCE(player, card).targets[0], target);
          PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
          if (g_other_player == player && (g_duel_network_flags & 2) == 0 && target.player == g_other_player)
          {
            g_ai_modifier -= 0x30;
          }
        }
        else
        {
          g_spell_fizzled = 1;
        }
      }
    }

    if (event == EVENT_GET_SELECTED_CARD)
    {
      load_recorded_action_target(0);
    }

    if (event == EVENT_RESOLVE_ACTIVATION)
    {
      SET_TARGET(target, PLAYER_CARD_INSTANCE(player, card).targets[0]);
      if (C_real_validate_target(target.player, target.card, (char *)0, player, 2, 2, TARGET_ZONE_IN_PLAY,
                                 TYPE_CREATURE, TYPE_NONE, 0, get_protections_from(player, card),
                                 COLOR_TEST_0, COLOR_TEST_0, -1, -1, -1, -1, 0, 0, 0) != 0)
      {
        create_legacy_effect(g_card_on_stack_controller, g_card_on_stack, g_duel_generated_internal_card_id_0b, target.player, target.card);
      }
      else
      {
        g_spell_fizzled = 1;
      }
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                           PLAYER_CARD_INSTANCE(player, card).parent_card)
          .number_of_targets = 0;
    }
  }
  return 0;
}

// FUNCTION: MAGIC 0x0042357d
// FUNCTION: SHANDALAR 0x00432260
int card_bone_flute(int player, int card, event_t event)
{
  if (event == EVENT_CAN_ACTIVATE)
  {
    return ((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0 && has_mana(player, COLOR_ANY, 2)) ? 1 : 0;
  }

  if (event == EVENT_ACTIVATE)
  {
    charge_mana(player, COLOR_COLORLESS, 2);
    if (g_spell_fizzled != 1)
    {
      PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
    }
    return 0;
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    dispatch_function_to_all_cards_in_play(g_card_on_stack_controller, g_card_on_stack, bone_flute_effect, -1);
    return 0;
  }

  if (event == EVENT_CHECK_PUMP && (PLAYER_CARD_INSTANCE(player, card).state & (STATE_TAPPED | STATE_SUMMONSICK_NOTAP)) == 0 &&
      has_mana(player, COLOR_ANY, 2))
  {
    --g_global_power_bonus[1 - player];
  }
  return 0;
}

// FUNCTION: MAGIC 0x00423772
// FUNCTION: SHANDALAR 0x00432455
int card_book_of_rass(int player, int card, event_t event)
{
  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    if (g_other_player == player && (g_duel_network_flags & 2) == 0 && g_life[player] == 2)
    {
      return 0;
    }
    return has_mana_w_global_cost_mod(player, card, COLOR_ANY, 2) != 0 && g_life[player] >= 2;
  }

  if (event == EVENT_ACTIVATE)
  {
    charge_mana_w_global_cost_mod(player, card, COLOR_COLORLESS, 2);
    if (g_spell_fizzled != 1)
    {
      g_life[player] -= 2;
      g_ai_modifier -= (g_duel_summary.hand_counts[player] * 2 - 4 - g_life[player] + 10) * 12;
    }
    return 0;
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    draw_card_for_player(player);
  }
  return 0;
}

// FUNCTION: MAGIC 0x00423893
// FUNCTION: SHANDALAR 0x00432575
int card_fountain_of_youth(int player, int card, event_t event)
{
  int count;

  if (event == EVENT_CAST_SPELL && g_affected_card == card && g_affected_card_controller == player &&
      g_other_player == player && (g_duel_network_flags & 2) == 0)
  {
    count = count_permanents_by_internal_card_id(player, PLAYER_CARD_INSTANCE(player, card).internal_card_id, player);
    if (count != 0 && 3 < g_basiclandtypes_controlled[g_other_player][COLOR_ANY] / count)
    {
      g_ai_modifier += 0x30;
    }
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    if (has_mana(player, COLOR_ANY, 2) == 0 ||
        (((PLAYER_CARD_INSTANCE(player, card).state & STATE_SUMMONSICK_BOTH) != 0 &&
          (global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].type & TYPE_CREATURE) != 0) ||
         (PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) != 0))
    {
      return 0;
    }
    return 1;
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
    gain_life(g_card_on_stack_controller, 1, g_card_on_stack_controller, g_card_on_stack);
  }
  return 0;
}

// FUNCTION: MAGIC 0x00423ab3
// FUNCTION: SHANDALAR 0x00432798
int card_life_chisel(int player, int card, event_t event)
{
  target_t target;

  if (event == EVENT_CAN_ACTIVATE)
  {
    if (g_current_phase == PHASE_UPKEEP && g_current_player == player)

    {
      return ((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0 &&
              PLAYER_CARD_INSTANCE(player, card).info_slot == 0 &&
              real_target_available((int *)0, TARGET_SCAN_DIRECT, player, player, player, TARGET_ZONE_IN_PLAY, TYPE_CREATURE, TYPE_NONE, 0, get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0, -1, -1, -1, -1, 0, 0, 0) != 0);
    }
    else
    {
      return 0;
    }
  }

  if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_target(0);
    return 0;
  }

  if (event == EVENT_ACTIVATE)
  {
    load_text("promptsX2.txt", "LIFE_CHISEL");
    if (!C_real_select_target(player, player, player, TARGET_ZONE_IN_PLAY, TYPE_CREATURE, TYPE_NONE, 0,
                              0, COLOR_TEST_0, COLOR_TEST_0, -1, -1, -1, -1, 0, 0, 0,
                              g_text_lines[0], 1, &target) == 0)
    {
      PLAYER_CARD_INSTANCE(player, card).eot_toughness =
          (int)PLAYER_CARD_INSTANCE(target.player, target.card).toughness;
      if (g_duel_ai_mode_state != 1)
      {
        play_sound_effect(0xf);
      }
      kill_card(target.player, target.card, KILL_SACRIFICE);
      PLAYER_CARD_INSTANCE(player, card).info_slot = 1;
    }
    else
    {
      g_spell_fizzled = 1;
      PLAYER_CARD_INSTANCE(player, card).info_slot = 0;
    }
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    gain_life(player, PLAYER_CARD_INSTANCE(player, card).eot_toughness,
              g_card_on_stack_controller, g_card_on_stack);
    PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                         PLAYER_CARD_INSTANCE(player, card).parent_card)
        .number_of_targets = 0;
  }

  if (event == EVENT_CLEANUP && g_affected_card == card && g_affected_card_controller == player)
  {
    PLAYER_CARD_INSTANCE(player, card).info_slot = 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x00423e69
// FUNCTION: SHANDALAR 0x00432b51
int card_poison_snake(int player, int card, event_t event)
{
  if (event == EVENT_DEAL_DAMAGE &&
      PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).internal_card_id == g_damage_card_internal_card_id)
  {
    if (PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).damage_target_card == -1 &&
        PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).damage_source_player == player &&
        PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).damage_source_card == card &&
        PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).info_slot != 0)
    {
      PLAYER_CARD_INSTANCE(player, card).damage_source_player = (char)g_affected_card_controller;
      PLAYER_CARD_INSTANCE(player, card).damage_source_card = g_affected_card;
    }
  }
  else if (g_trigger_condition == TRIGGER_DEAL_DAMAGE && g_affected_card == card &&
           g_affected_card_controller == player &&
           PLAYER_CARD_INSTANCE(player, card).damage_source_player != -1 &&
           player == g_current_turn)
  {
    if (event == EVENT_TRIGGER)
    {
      g_event_result |= 2;
    }
    if (event == EVENT_RESOLVE_TRIGGER)
    {
      ++g_poison_counters[PLAYER_CARD_INSTANCE(player, card).damage_source_player];
      PLAYER_CARD_INSTANCE(player, card).damage_source_player = -1;
      check_duel_finished();
    }
  }
  return 0;
}
// FUNCTION: MAGIC 0x00424116
// FUNCTION: SHANDALAR 0x00432dff
int card_relic_barrier(int player, int card, event_t event)
{
  target_t target;

  if (event == EVENT_CAN_ACTIVATE)
  {
    return (PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0 &&
           (((PLAYER_CARD_INSTANCE(player, card).state & STATE_SUMMONSICK_BOTH) == 0) ||
            ((global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].type & TYPE_CREATURE) == 0)) &&
           real_target_available((int *)0, TARGET_SCAN_DIRECT, player, 2, player, TARGET_ZONE_IN_PLAY,
                                 TYPE_ARTIFACT, TYPE_NONE, 0, get_protections_from(player, card),
                                 COLOR_TEST_0, COLOR_TEST_0, -1, -1, -1, -1, 0, 0, 0) != 0;
  }
  else
  {
    if (event == EVENT_ACTIVATE)
    {
      if (g_duel_ai_mode_state != 1)
      {
        load_text("promptsX2.txt", "RELIC_BARRIER");
      }
      if (C_real_select_target(player, 2, player, TARGET_ZONE_IN_PLAY, TYPE_ARTIFACT, TYPE_NONE, 0,
                               get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0, -1, -1, -1, -1,
                               0, 0, 0, g_text_lines[0], 1, &target) == 0)
      {
        g_spell_fizzled = 1;
      }
      else
      {
        SET_TARGET(PLAYER_CARD_INSTANCE(player, card).targets[0], target);
        PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
        PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
        if (player == g_other_player && target.player == g_other_player)
        {
          g_ai_modifier -= 0x30;
        }
      }
    }

    if (event == EVENT_RESOLVE_ACTIVATION)
    {
      SET_TARGET(target, PLAYER_CARD_INSTANCE(player, card).targets[0]);
      if (C_real_validate_target(target.player, target.card, (char *)0, player, 2, 2, TARGET_ZONE_IN_PLAY,
                                 TYPE_ARTIFACT, TYPE_NONE, 0, get_protections_from(player, card),
                                 COLOR_TEST_0, COLOR_TEST_0, -1, -1, -1, -1, 0, 0, 0) == 0)
      {
        g_spell_fizzled = 1;
      }
      else
      {
        PLAYER_CARD_INSTANCE(target.player, target.card).state |= STATE_TAPPED;
      }
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                           PLAYER_CARD_INSTANCE(player, card).parent_card)
          .number_of_targets = 0;
    }
  }
  return 0;
}

// FUNCTION: MAGIC 0x0042454b
// FUNCTION: SHANDALAR 0x00433233
int card_serpent_generator(int player, int card, event_t event)
{
  int snake_card;

  if (event == EVENT_CAN_ACTIVATE)
  {
    if (PLAYER_CARD_INSTANCE(player, card).info_slot == 0 && has_mana(player, COLOR_ANY, 4) != 0 &&
        (((PLAYER_CARD_INSTANCE(player, card).state & STATE_SUMMONSICK_BOTH) == 0) ||
         ((global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].type & TYPE_CREATURE) == 0)) &&
        (PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0)
    {
      if (player == g_other_player && (g_duel_network_flags & 2) == 0 && g_ai_speculation_depth > 0)
      {
        g_activation_event_flags |= 3;
      }
      return 1;
    }
    return 0;
  }

  if (event == EVENT_ACTIVATE)
  {
    charge_mana(player, COLOR_COLORLESS, 4);
    if (g_spell_fizzled != 1)
    {
      PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
      PLAYER_CARD_INSTANCE(player, card).info_slot |= 1;
      if (g_ai_speculation_depth > 0)
      {
        --g_ai_speculation_depth;
      }
    }
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    snake_card = add_card_to_hand(player, find_internal_card_id_by_csv_id(CARD_ID_POISON_SNAKE));
    if (snake_card != -1)
    {
      process_card_enters_play(player, snake_card);
      PLAYER_CARD_INSTANCE(player, snake_card).token_status |= STATUS_TOKEN;
    }
    PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                         PLAYER_CARD_INSTANCE(player, card).parent_card)
        .info_slot = 0;
  }
  return 0;
}

// FUNCTION: SHANDALAR 0x00433b35
static int war_barge_kill_attached_creature(int player, int card, int test_player, int test_card, int internal_card_id)
{
  if (internal_card_id == g_duel_generated_internal_card_id_03 &&
      PLAYER_CARD_INSTANCE(test_player, test_card).damage_source_player == player &&
      PLAYER_CARD_INSTANCE(test_player, test_card).damage_source_card == card &&
      PLAYER_CARD_INSTANCE(test_player, test_card).damage_target_player != -1 &&
      PLAYER_CARD_INSTANCE(test_player, test_card).damage_target_card != -1)
  {
    kill_card((int)PLAYER_CARD_INSTANCE(test_player, test_card).damage_target_player,
              PLAYER_CARD_INSTANCE(test_player, test_card).damage_target_card, KILL_BURY);
    g_battlefield_extra_ability_flags |= 1;
  }
  return 0;
}

// FUNCTION: MAGIC 0x00424852
// FUNCTION: SHANDALAR 0x00433539
int card_war_barge(int player, int card, event_t event)
{
  target_t target;
  int legacy_card;
  int hacked_color;
  int landwalk_mask;

  hacked_color = get_hacked_color(player, card, COLOR_BLUE);
  landwalk_mask = 1 << (unsigned char)(hacked_color - 1);

  if (event == EVENT_CAN_ACTIVATE)
  {
    if ((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0 &&
        has_mana(player, COLOR_ANY, 3) &&
        real_target_available((int *)0, TARGET_SCAN_DIRECT, player, 2, 2, TARGET_ZONE_IN_PLAY,
                              TYPE_CREATURE, TYPE_NONE, 0, get_protections_from(player, card),
                              COLOR_TEST_0, COLOR_TEST_0, -1, -1, -1, -1, 0, 0, 0) != 0)
    {
      return 1;
    }
  }
  else if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_target(0);
  }
  else
  {
    if (event == EVENT_ACTIVATE)
    {
      charge_mana(player, COLOR_COLORLESS, 3);
      if (g_spell_fizzled != 1)
      {
        if (g_duel_ai_mode_state != 1)
        {
          load_text("promptsX2.txt", "WAR_BARGE");
        }
        if (C_real_select_target(player, 2, 2, TARGET_ZONE_IN_PLAY, TYPE_CREATURE, TYPE_NONE, 0,
                                 get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0,
                                 -1, -1, -1, -1, 0, 0, 0, g_text_lines[0], 1, &target) == 0)
        {
          g_spell_fizzled = 1;
        }
        else
        {
          SET_TARGET(PLAYER_CARD_INSTANCE(player, card).targets[0], target);
          PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
          if (player == g_other_player &&
              (PLAYER_CARD_INSTANCE(target.player, target.card).regen_status & landwalk_mask) != 0)
          {
            g_ai_modifier -= 0x60;
          }
        }
      }
    }

    if (event == EVENT_RESOLVE_ACTIVATION)
    {
      SET_TARGET(target, PLAYER_CARD_INSTANCE(player, card).targets[0]);
      if (C_real_validate_target(target.player, target.card, (char *)0, player, 2, 2,
                                 TARGET_ZONE_IN_PLAY, TYPE_CREATURE, TYPE_NONE, 0,
                                 get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0,
                                 -1, -1, -1, -1, 0, 0, 0) == 0)
      {
        g_spell_fizzled = 1;
      }
      else
      {
        PLAYER_CARD_INSTANCE(target.player, target.card).regen_status = KEYWORD_RECALC_ABILITIES;
        legacy_card = create_legacy_effect(g_card_on_stack_controller, g_card_on_stack, g_duel_generated_internal_card_id_03,
                                           target.player, target.card);
        if (legacy_card != -1)
        {
          PLAYER_CARD_INSTANCE(player, legacy_card).info_slot = landwalk_mask;
        }
      }
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                           PLAYER_CARD_INSTANCE(player, card).parent_card)
          .number_of_targets = 0;
    }

    if (g_trigger_condition == TRIGGER_LEAVE_PLAY &&
        g_affected_card == card &&
        g_affected_card_controller == player &&
        player == g_trigger_cause_controller &&
        g_trigger_cause == card &&
        player == g_current_turn)
    {
      if (event == EVENT_TRIGGER)
      {
        g_event_result |= 2;
      }
      if (event == EVENT_RESOLVE_TRIGGER)
      {
        dispatch_function_to_all_cards_in_play(player, card, war_barge_kill_attached_creature, -1);
      }
    }
  }
  return 0;
}

// FUNCTION: MAGIC 0x00424fbe
// FUNCTION: SHANDALAR 0x00433ca1
int card_akron_legionnaire(int player, int card, event_t event)
{
  if (event == EVENT_ATTACK_LEGALITY &&
      player == g_current_player &&
      (global_cards_data[PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).internal_card_id].type &
       (TYPE_CREATURE | TYPE_ARTIFACT)) != (TYPE_CREATURE | TYPE_ARTIFACT) &&
      global_raw_cards_storage[global_cards_data[PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).internal_card_id].id].subtype != 0x70)
  {
    ++g_event_result;
  }
  return 0;
}

// FUNCTION: MAGIC 0x0042508d
// FUNCTION: SHANDALAR 0x00433d6f
int card_banshee(int player, int card, event_t event)
{
  if (event == EVENT_CAN_ACTIVATE)
  {
    if (g_other_player == player && (g_duel_network_flags & 2) == 0 && has_mana(player, COLOR_ANY, 1) == 0)
    {
      return 0;
    }
    return (PLAYER_CARD_INSTANCE(player, card).state & (STATE_TAPPED | STATE_SUMMONSICK_NOTAP)) == 0;
  }

  if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_target(1);
    return 0;
  }

  if (event == EVENT_ACTIVATE)
  {
    charge_mana(player, COLOR_COLORLESS, -1);
    if (g_spell_fizzled != 1)
    {
      PLAYER_CARD_INSTANCE(player, card).info_slot = g_x_value;
      if (g_duel_ai_mode_state != 1)
      {
        load_text("promptsX2.txt", "BANSHEE");
      }
      select_damage_target(player, card, PLAYER_CARD_INSTANCE(player, card).info_slot / 2);
      if (g_spell_fizzled != 1)
      {
        PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
      }
    }
    return 0;
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    deal_damage_to_selected_target(player, card, event, PLAYER_CARD_INSTANCE(player, card).info_slot / 2);
    damage_player(player, (PLAYER_CARD_INSTANCE(player, card).info_slot + 1) / 2,
                  g_card_on_stack_controller, g_card_on_stack);
    PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                         PLAYER_CARD_INSTANCE(player, card).parent_card)
        .number_of_targets = 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x00425371
// FUNCTION: SHANDALAR 0x00434051
int card_beasts_of_bogardan(int player, int card, event_t event)
{
  int opponent;
  int current_card;

  if (event == EVENT_ABILITIES && g_affected_card == card && g_affected_card_controller == player)
  {
    g_event_result |= KEYWORD_PROT_BLACK << (((unsigned char)get_sleighted_color(player, card, COLOR_RED) - 1) & 0x1f);
  }

  if ((event == EVENT_POWER || event == EVENT_TOUGHNESS) && g_affected_card == card &&
      g_affected_card_controller == player)
  {
    opponent = 1 - player;
    for (current_card = 0; current_card < g_active_cards_count[opponent]; ++current_card)
    {
      if (is_in_play(opponent, current_card))
      {
        if (((1 << ((unsigned char)get_sleighted_color(player, card, COLOR_WHITE) & 0x1f)) & (unsigned char)PLAYER_CARD_INSTANCE(opponent, current_card).color) != 0)
        {
          ++g_event_result;
          return 0;
        }
      }
    }
  }
  return 0;
}

// FUNCTION: MAGIC 0x004254a9
// FUNCTION: SHANDALAR 0x00434189
int card_bog_rats(int player, int card, event_t event)
{
  if (event == EVENT_BLOCK_LEGALITY && g_attacking_card == card &&
      g_attacking_card_controller == player &&
      global_cards_data[PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).internal_card_id].subtype == 0)
  {
    g_event_result = 1;
  }
  return 0;
}

// FUNCTION: MAGIC 0x00425533
// FUNCTION: SHANDALAR 0x00434213
int card_coal_golem(int player, int card, event_t event)
{
  if (event == EVENT_CAN_ACTIVATE)
  {
    return (PLAYER_CARD_INSTANCE(player, card).state & STATE_SUMMONSICK_BOTH) == 0 &&
           has_mana(player, COLOR_ANY, 3);
  }

  if (event == EVENT_ACTIVATE)
  {
    charge_mana(player, COLOR_COLORLESS, 3);
    if (g_spell_fizzled != 1)
    {
      undeclare_mana_available_and_produce_it(player, COLOR_RED, 3);
      g_produced_mana_color = COLOR_RED;
    }
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    if (g_duel_ai_mode_state != 1)
    {
      play_sound_effect(0xf);
    }
    kill_card(g_card_on_stack_controller, g_card_on_stack, KILL_SACRIFICE);
  }
  return 0;
}

// FUNCTION: MAGIC 0x00425625
// FUNCTION: SHANDALAR 0x00434305
int card_d_avenant_archer(int player, int card, event_t event)
{
  target_t target;

  if (event == EVENT_CAN_ACTIVATE)
  {
    return (PLAYER_CARD_INSTANCE(player, card).state & (STATE_TAPPED | STATE_SUMMONSICK_NOTAP)) == 0 &&
           real_target_available((int *)0, TARGET_SCAN_DIRECT, player, 2, 2, TARGET_ZONE_IN_PLAY,
                                 TYPE_CREATURE, TYPE_NONE, 0, get_protections_from(player, card),
                                 COLOR_TEST_0, COLOR_TEST_0, -1, -1, -1, -1, 0,
                                 TARGET_STATE_IN_COMBAT, 0) != 0;
  }
  else if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_target(0);
  }
  else if (event == EVENT_ACTIVATE)
  {
    if (g_duel_ai_mode_state != 1)
    {
      load_text("promptsX2.txt", "DAVENANT_ARCHER");
    }
    if (C_real_select_target(player, 2, 1 - player, TARGET_ZONE_IN_PLAY, TYPE_CREATURE, TYPE_NONE, 0,
                             get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0, -1, -1, -1, -1,
                             0, TARGET_STATE_IN_COMBAT, 0, g_text_lines[0], 1, &target) == 0)
    {
      g_spell_fizzled = 1;
    }
    else
    {
      SET_TARGET(PLAYER_CARD_INSTANCE(player, card).targets[0], target);
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
      PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
    }
  }
  else if (event == EVENT_RESOLVE_ACTIVATION)
  {
    SET_TARGET(target, PLAYER_CARD_INSTANCE(player, card).targets[0]);
    if (C_real_validate_target(target.player, target.card, (char *)0, player, 2, 2, TARGET_ZONE_IN_PLAY,
                               TYPE_CREATURE, TYPE_NONE, 0, get_protections_from(player, card),
                               COLOR_TEST_0, COLOR_TEST_0, -1, -1, -1, -1, 0, TARGET_STATE_IN_COMBAT, 0) == 0)
    {
      g_spell_fizzled = 1;
    }
    else
    {
      damage_creature(target.player, target.card, 1, g_card_on_stack_controller, g_card_on_stack);
    }
    PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                         PLAYER_CARD_INSTANCE(player, card).parent_card)
        .number_of_targets = 0;
  }
  return 0;
}

// FUNCTION: MAGIC 0x004259ba
// FUNCTION: SHANDALAR 0x0043469a
int card_elder_spawn(int player, int card, event_t event)
{
  int color;

  if (event == EVENT_BLOCK_LEGALITY && card == g_attacking_card && player == g_attacking_card_controller)
  {
    if ((PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).color &
         (1 << (unsigned char)get_sleighted_color(player, card, COLOR_RED))) != 0)
    {
      g_event_result = 1;
    }
  }
  else if (event == EVENT_CHECK_UPK_PAYMENT)
  {
    color = get_hacked_color(player, card, COLOR_BLUE);
    if (g_basiclandtypes_controlled[player][color] > 0)
    {
      g_event_result |= 1;
    }
  }
  else
  {
    if (event == EVENT_SETUP_UPKEEP_COSTS &&
        g_affected_card == card &&
        g_affected_card_controller == player &&
        PLAYER_CARD_INSTANCE(player, card).info_slot == 0 &&
        player == g_current_player &&
        player == g_event_player)
    {
      PLAYER_CARD_INSTANCE(player, card).upkeep_flags |= 0x101;
    }

    if (event == EVENT_UPKEEP_PHASE &&
        g_affected_card == card &&
        g_affected_card_controller == player)
    {
      ++PLAYER_CARD_INSTANCE(player, card).info_slot;
      color = get_hacked_color(player, card, COLOR_BLUE);
      if (g_basiclandtypes_controlled[player][color] < 1)
      {
        g_event_result |= 1;
      }
      else
      {
        TENTATIVE_sacrifice_basic_land_type(player, card, 1, color, 0);
      }
    }
    else if (event == EVENT_UPKEEP_COSTS_UNPAID)
    {
      load_text("promptsX2.txt", "ELDER_SPAWN");
      do_dialog(player, player, card, -1, -1, g_text_lines[1], 0);
      damage_player(player, 6, g_card_on_stack_controller, g_card_on_stack);
      kill_card(g_card_on_stack_controller, g_card_on_stack, KILL_BURY);
    }
    else
    {
      if (event == EVENT_SHOULD_AI_PLAY)
      {
        color = get_hacked_color(player, card, COLOR_BLUE);
        if (g_basiclandtypes_controlled[player][color] == 0)
        {
          damage_player(player, 6, player, card);
          kill_card(player, card, KILL_BURY);
        }
      }
      if ((event == EVENT_CLEANUP || event == EVENT_SHOULD_AI_PLAY) &&
          g_affected_card == card &&
          g_affected_card_controller == player)
      {
        PLAYER_CARD_INSTANCE(player, card).info_slot = 0;
      }
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x00425d36
// FUNCTION: SHANDALAR 0x00434a1e
int card_elves_of_deep_shadow(int player, int card, event_t event)
{
  if (event == EVENT_COUNT_MANA && card == g_affected_card && player == g_affected_card_controller)
  {
    if ((PLAYER_CARD_INSTANCE(player, card).state & (STATE_TAPPED | STATE_SUMMONSICK_NOTAP)) == 0)
    {
      declare_mana_available(player, COLOR_BLACK, 1);
    }
    return 0;
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    return (PLAYER_CARD_INSTANCE(player, card).state & (STATE_TAPPED | STATE_SUMMONSICK_NOTAP)) == 0;
  }

  if (event == EVENT_ACTIVATE)
  {
    undeclare_mana_available_and_produce_it(player, COLOR_BLACK, 1);
    PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
    g_produced_mana_color = COLOR_BLACK;
    return 0;
  }

  if (event == EVENT_TAP_CARD && card == g_affected_card && player == g_affected_card_controller &&
      g_produced_mana_color != -1)
  {
    damage_player(g_affected_card_controller, 1, player, card);
  }

  if (event == EVENT_ATTACK_RATING && card == g_affected_card && player == g_affected_card_controller)
  {
    g_ai_score += 0x18 / (g_basiclandtypes_controlled[player][COLOR_BLACK] + 2);
    return 0;
  }

  if (event == EVENT_BLOCK_RATING && card == g_affected_card && player == g_affected_card_controller)
  {
    g_ai_score -= 0x60 / (g_basiclandtypes_controlled[player][COLOR_BLACK] + 2);
    return 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x00425f60
// FUNCTION: SHANDALAR 0x00434c40
int card_emerald_dragonfly(int player, int card, event_t event)
{
  int legacy_card;

  if (event == EVENT_CAN_ACTIVATE)
  {
    return (has_mana(player, COLOR_GREEN, 2) != 0 &&
            (PLAYER_CARD_INSTANCE(player, card).info_slot & KEYWORD_FIRST_STRIKE) == 0);
  }

  if (event == EVENT_ACTIVATE)
  {
    charge_mana(player, COLOR_GREEN, 2);
    if (g_spell_fizzled != 1)
    {
      PLAYER_CARD_INSTANCE(player, card).targets[0].player = player;
      PLAYER_CARD_INSTANCE(player, card).targets[0].card = card;
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
    }
    return 0;
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    if (PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                             PLAYER_CARD_INSTANCE(player, card).parent_card)
                .internal_card_id != -1 &&
        (PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                              PLAYER_CARD_INSTANCE(player, card).parent_card)
             .info_slot &
         KEYWORD_FIRST_STRIKE) == 0)
    {
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                           PLAYER_CARD_INSTANCE(player, card).parent_card)
          .info_slot |= KEYWORD_FIRST_STRIKE;
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                           PLAYER_CARD_INSTANCE(player, card).parent_card)
          .number_of_targets = 0;
      legacy_card = create_legacy_effect(g_card_on_stack_controller, g_card_on_stack, g_duel_generated_internal_card_id_03,
                                         g_card_on_stack_controller, g_card_on_stack);
      if (legacy_card != -1)
      {
        PLAYER_CARD_INSTANCE(player, legacy_card).info_slot = KEYWORD_FIRST_STRIKE;
        PLAYER_CARD_INSTANCE(player, legacy_card).regen_status = 0;
      }
    }
    return 0;
  }

  if (event == EVENT_CHECK_ABILITIES && g_affected_card == card && g_affected_card_controller == player &&
      has_mana(player, COLOR_GREEN, 2) != 0)
  {
    combat_eval_abilities |= KEYWORD_FIRST_STRIKE;
    return 0;
  }

  if (event == EVENT_CLEANUP || event == EVENT_SHOULD_AI_PLAY)
  {
    PLAYER_CARD_INSTANCE(player, card).info_slot = 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x00426402
// FUNCTION: SHANDALAR 0x004350e4
int card_exorcist(int player, int card, event_t event)
{
  target_t target;

  if (event == EVENT_UNTAP_PHASE)
  {
    ++g_ai_mana_demand_by_color[player][COLOR_WHITE];
  }
  else if (event == EVENT_CAN_ACTIVATE)
  {
    if ((PLAYER_CARD_INSTANCE(player, card).state & (STATE_TAPPED | STATE_SUMMONSICK_NOTAP)) == 0 &&
        has_mana(player, COLOR_WHITE, 1) != 0 &&
        has_mana(player, COLOR_ANY, 2) != 0 &&
        real_target_available((int *)0, TARGET_SCAN_DIRECT, player, 2, 2, TARGET_ZONE_IN_PLAY,
                              TYPE_CREATURE, TYPE_NONE, 0, get_protections_from(player, card),
                              1 << (unsigned char)get_sleighted_color(player, card, COLOR_BLACK),
                              COLOR_TEST_0, -1, -1, -1, -1, 0, 0, 0) != 0)
    {
      return 1;
    }
  }
  else if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_target(0);
  }
  else if (event == EVENT_ACTIVATE)
  {
    g_mana_charge[COLOR_COLORLESS] = 1;
    charge_mana(player, COLOR_WHITE, 1);
    if (g_spell_fizzled != 1)
    {
      if (g_duel_ai_mode_state != 1)
      {
        load_text("promptsX2.txt", "EXORCIST");
      }
      if (C_real_select_target(player, 2, 2, TARGET_ZONE_IN_PLAY, TYPE_CREATURE, TYPE_NONE, 0,
                               get_protections_from(player, card),
                               1 << (unsigned char)get_sleighted_color(player, card, COLOR_BLACK),
                               COLOR_TEST_0,
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
  else
  {
    if (event == EVENT_RESOLVE_ACTIVATION)
    {
      SET_TARGET(target, PLAYER_CARD_INSTANCE(player, card).targets[0]);
      if (C_real_validate_target(target.player, target.card, (char *)0, player, 2, 2, TARGET_ZONE_IN_PLAY,
                                 TYPE_CREATURE, TYPE_NONE, 0, get_protections_from(player, card),
                                 1 << (unsigned char)get_sleighted_color(player, card, COLOR_BLACK),
                                 COLOR_TEST_0, -1, -1, -1, -1, 0, 0, 0) != 0)
      {
        kill_card(target.player, target.card, KILL_DESTROY);
      }
      else
      {
        g_spell_fizzled = 1;
      }
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                           PLAYER_CARD_INSTANCE(player, card).parent_card)
          .number_of_targets = 0;
    }

    if (event == EVENT_SHOULD_AI_PLAY && g_current_phase == PHASE_CLEANUP &&
        g_affected_card == card && g_affected_card_controller == player)
    {
      g_ai_modifier += g_basiclandtypes_controlled[1 - player][get_sleighted_color(player, card, COLOR_BLACK)] * 0xc;
    }
  }
  return 0;
}

// FUNCTION: MAGIC 0x004268ac
// FUNCTION: SHANDALAR 0x00435590
int card_fallen_angel(int player, int card, event_t event)
{
  int legacy_card;
  target_t target;
  card_instance_t *parent;

  if (event == EVENT_CAST_SPELL && g_affected_card_controller == player &&
      (global_cards_data[PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).internal_card_id].type &
       TYPE_CREATURE) != 0)
  {
    g_ai_modifier += 0x30;
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    return real_target_available((int *)0, TARGET_SCAN_DIRECT, player, player, player, TARGET_ZONE_IN_PLAY,
                                 TYPE_CREATURE, TYPE_NONE, 0, 0, COLOR_TEST_0, COLOR_TEST_0, -1, -1,
                                 -1, -1, 0, 0, 0);
  }
  else if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_target(0);
    return 0;
  }
  else
  {
    if (event == EVENT_ACTIVATE)
    {
      load_text("promptsX2.txt", "FALLEN_ANGEL");
      if (C_real_select_target(player, player, player, TARGET_ZONE_IN_PLAY, TYPE_CREATURE, TYPE_NONE, 0, 0,
                               COLOR_TEST_0, COLOR_TEST_0, -1, -1, -1, -1, 0, 0, 0, g_text_lines[0], 1,
                               &target) == 0)
      {
        g_spell_fizzled = 1;
      }
      else
      {
        if (g_duel_ai_mode_state != 1)
        {
          play_sound_effect(0xf);
        }
        kill_card(target.player, target.card, KILL_SACRIFICE);
        PLAYER_CARD_INSTANCE(player, card).targets[0].player = player;
        PLAYER_CARD_INSTANCE(player, card).targets[0].card = card;
        PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
        if (PLAYER_CARD_INSTANCE(player, card).info_slot == 0)
        {
          PLAYER_CARD_INSTANCE(player, card).info_slot |= 0x80000;
        }
      }
    }

    if (event == EVENT_RESOLVE_ACTIVATION)
    {
      parent = &PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                                     PLAYER_CARD_INSTANCE(player, card).parent_card);
      if (parent->internal_card_id == -1)
      {
        g_spell_fizzled = 1;
      }
      else
      {
        parent->info_slot += 2;
        parent->info_slot += 0x100;
        parent->number_of_targets = 0;
        if ((parent->info_slot & 0x80000) != 0)
        {
          parent->info_slot &= 0xfff7ffff;
          legacy_card = create_legacy_effect(g_card_on_stack_controller, g_card_on_stack, LEGACY_EFFECT_PUMP,
                                             g_card_on_stack_controller, g_card_on_stack);
          if (legacy_card != -1)
          {
            PLAYER_CARD_INSTANCE(player, legacy_card).info_slot |= 0x80000;
          }
        }
      }
    }

    if (event == EVENT_CLEANUP || event == EVENT_SHOULD_AI_PLAY)
    {
      PLAYER_CARD_INSTANCE(player, card).info_slot = 0;
    }

    if (event == EVENT_SHOULD_AI_PLAY && g_current_phase == PHASE_DISCARD && g_other_player == player)
    {
      g_ai_modifier += g_duel_summary.creature_counts[player] * 0x18;
    }
  }
  return 0;
}

// FUNCTION: MAGIC 0x00426f4e
// FUNCTION: SHANDALAR 0x00435c34
int card_fire_drake(int player, int card, event_t event)
{
  int legacy_card;

  if (event == EVENT_UNTAP_PHASE)
  {
    ++g_ai_mana_demand_by_color[player][COLOR_RED];
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    return (PLAYER_CARD_INSTANCE(player, card).info_slot == 0 && has_mana(player, COLOR_RED, 1) != 0) ? 1 : 0;
  }

  if (event == EVENT_ACTIVATE)
  {
    charge_mana(player, COLOR_RED, 1);
    if (g_spell_fizzled != 1)
    {
      PLAYER_CARD_INSTANCE(player, card).targets[0].player = player;
      PLAYER_CARD_INSTANCE(player, card).targets[0].card = card;
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
      PLAYER_CARD_INSTANCE(player, card).info_slot = 1;
    }
    else
    {
      PLAYER_CARD_INSTANCE(player, card).info_slot = 0;
    }
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    legacy_card = create_legacy_effect(g_card_on_stack_controller, g_card_on_stack, LEGACY_EFFECT_PUMP,
                                       g_card_on_stack_controller, g_card_on_stack);
    if (legacy_card != -1)
    {
      PLAYER_CARD_INSTANCE(player, legacy_card).counter_power = 1;
    }
    PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                         PLAYER_CARD_INSTANCE(player, card).parent_card)
        .number_of_targets = 0;
  }

  if (event == EVENT_POW_BOOST)
  {
    return ClampIntToRange(has_mana(player, COLOR_RED, 1), 0, 1);
  }

  if (event == EVENT_CLEANUP && card == g_affected_card && player == g_affected_card_controller)
  {
    PLAYER_CARD_INSTANCE(player, card).info_slot = 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0042727b
// FUNCTION: SHANDALAR 0x00435f5f
int card_fire_sprites(int player, int card, event_t event)
{
  if (event == EVENT_CAN_ACTIVATE)
  {
    return (PLAYER_CARD_INSTANCE(player, card).state & (STATE_TAPPED | STATE_SUMMONSICK_NOTAP)) == 0 &&
           has_mana(player, COLOR_GREEN, 1);
  }

  if (event == EVENT_ACTIVATE)
  {
    charge_mana(player, COLOR_GREEN, 1);
    if (g_spell_fizzled != 1)
    {
      undeclare_mana_available_and_produce_it(player, COLOR_RED, 1);
      PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
      g_produced_mana_color = COLOR_RED;
    }

    return 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0042736f
// FUNCTION: SHANDALAR 0x00436053
int card_ghosts_of_the_damned(int player, int card, event_t event)
{
  target_t target;
  int legacy_card;

  if (event == EVENT_CAN_ACTIVATE)
  {
    return (PLAYER_CARD_INSTANCE(player, card).state & (STATE_TAPPED | STATE_SUMMONSICK_NOTAP)) == 0 &&
           real_target_available((int *)0, TARGET_SCAN_DIRECT, player, 2, 2, TARGET_ZONE_IN_PLAY,
                                 TYPE_CREATURE, TYPE_NONE, 0, get_protections_from(player, card),
                                 COLOR_TEST_0, COLOR_TEST_0, -1, -1, -1, -1, 0, 0, 0) != 0;
  }
  else if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_target(0);
  }
  else if (event == EVENT_ACTIVATE)
  {
    if (g_duel_ai_mode_state != 1)
    {
      load_text("promptsX2.txt", "GHOSTS_OF_THE_DAMNED");
    }
    if (C_real_select_target(player, 2, 2, TARGET_ZONE_IN_PLAY, TYPE_CREATURE, TYPE_NONE, 0,
                             get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0,
                             -1, -1, -1, -1, 0, 0, 0, g_text_lines[0], 1, &target) == 0)
    {
      g_spell_fizzled = 1;
    }
    else
    {
      SET_TARGET(PLAYER_CARD_INSTANCE(player, card).targets[0], target);
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
      PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
    }
  }
  else if (event == EVENT_RESOLVE_ACTIVATION)
  {
    SET_TARGET(target, PLAYER_CARD_INSTANCE(player, card).targets[0]);
    if (C_real_validate_target(target.player, target.card, (char *)0, player, 2, 2, TARGET_ZONE_IN_PLAY,
                               TYPE_CREATURE, TYPE_NONE, 0, get_protections_from(player, card),
                               COLOR_TEST_0, COLOR_TEST_0, -1, -1, -1, -1, 0, 0, 0) == 0)
    {
      g_spell_fizzled = 1;
    }
    else
    {
      legacy_card = create_legacy_effect(g_card_on_stack_controller, g_card_on_stack, LEGACY_EFFECT_PUMP,
                                         target.player, target.card);
      if (legacy_card != -1)
      {
        PLAYER_CARD_INSTANCE(player, legacy_card).counter_power = -1;
        PLAYER_CARD_INSTANCE(player, legacy_card).counter_toughness = 0;
      }
    }
    PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                         PLAYER_CARD_INSTANCE(player, card).parent_card)
        .number_of_targets = 0;
  }
  else if (event == EVENT_CHECK_PUMP &&
           (PLAYER_CARD_INSTANCE(player, card).state & (STATE_TAPPED | STATE_SUMMONSICK_NOTAP)) == 0)
  {
    --g_global_power_bonus[1 - player];
  }
  else if (event == EVENT_ATTACK_RATING && g_affected_card == card && g_affected_card_controller == player)
  {
    g_ai_score += 0xc;
  }
  else if (event == EVENT_BLOCK_RATING && g_affected_card == card && g_affected_card_controller == player)
  {
    g_ai_score -= 0xc;
  }

  return 0;
}

// FUNCTION: MAGIC 0x00427841
// FUNCTION: SHANDALAR 0x00436525
int card_giant_turtle(int player, int card, event_t event)
{
  if (event == EVENT_ATTACK_LEGALITY && PLAYER_CARD_INSTANCE(player, card).info_slot != 0)
  {
    g_event_result = 1;
  }

  if (event == EVENT_CLEANUP && g_affected_card == card && g_affected_card_controller == player &&
      g_current_player == player)
  {
    if (PLAYER_CARD_INSTANCE(player, card).state & STATE_ATTACKED)
    {
      PLAYER_CARD_INSTANCE(player, card).info_slot = 1;
    }
    else
    {
      PLAYER_CARD_INSTANCE(player, card).info_slot = 0;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0042796e
// FUNCTION: SHANDALAR 0x00436655
int card_goblin_digging_team(int player, int card, event_t event)
{
  target_t target;

  if (event == EVENT_CAN_ACTIVATE)
  {
    return (PLAYER_CARD_INSTANCE(player, card).state & (STATE_TAPPED | STATE_SUMMONSICK_NOTAP)) == 0 &&
           real_target_available((int *)0, TARGET_SCAN_DIRECT, player, 2, 2, TARGET_ZONE_IN_PLAY,
                                 TYPE_CREATURE, TYPE_NONE, 0, get_protections_from(player, card),
                                 COLOR_TEST_0, COLOR_TEST_0, -1, -1, -1, -1, TARGET_SPECIAL_WALL, 0, 0) != 0;
  }
  else if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_target(0);
  }
  else
  {
    if (event == EVENT_ACTIVATE)
    {
      load_text("promptsX2.txt", "GOBLIN_DIGGING_TEAM");
      if (C_real_select_target(player, 2, 2, TARGET_ZONE_IN_PLAY, TYPE_CREATURE, TYPE_NONE, 0,
                               get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0, -1, -1, -1, -1,
                               TARGET_SPECIAL_WALL, 0, 0, g_text_lines[0], 1, &target) == 0)
      {
        g_spell_fizzled = 1;
      }
      else
      {
        SET_TARGET(PLAYER_CARD_INSTANCE(player, card).targets[0], target);
        PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
        PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
        kill_card(player, card, KILL_SACRIFICE);
      }
    }

    if (event == EVENT_RESOLVE_ACTIVATION)
    {
      SET_TARGET(target, PLAYER_CARD_INSTANCE(player, card).targets[0]);
      if (C_real_validate_target(target.player, target.card, (char *)0, player, 2, 2, TARGET_ZONE_IN_PLAY,
                                 TYPE_CREATURE, TYPE_NONE, 0, get_protections_from(player, card),
                                 COLOR_TEST_0, COLOR_TEST_0, -1, -1, -1, -1, TARGET_SPECIAL_WALL, 0, 0) == 0)
      {
        g_spell_fizzled = 1;
      }
      else
      {
        kill_card(target.player, target.card, KILL_DESTROY);
      }
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                           PLAYER_CARD_INSTANCE(player, card).parent_card)
          .number_of_targets = 0;
    }
  }
  return 0;
}

// FUNCTION: MAGIC 0x00427cee
// FUNCTION: SHANDALAR 0x004369d5
int card_goblins_of_the_flarg(int player, int card, event_t event)
{
  if (event == EVENT_CHANGE_TYPE && (g_land_can_be_played & 0x20000) == 0)
  {
    if (is_in_play(player, card) && PLAYER_CARD_INSTANCE(player, card).info_slot == 0 &&
        is_in_play(g_affected_card_controller, g_affected_card) &&
        player == g_affected_card_controller && global_cards_data[g_event_result].subtype == SUB_DWARF)
    {
      PLAYER_CARD_INSTANCE(player, card).info_slot = 1;
      kill_card(player, card, KILL_BURY);
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x00427dfe
// FUNCTION: SHANDALAR 0x00436ae4
int card_hyperion_blacksmith(int player, int card, event_t event)
{
  struct
  {
    target_t target;
    char dialog[600];
  } s;

  if (event == EVENT_CAN_ACTIVATE)
  {
    return (PLAYER_CARD_INSTANCE(player, card).state & (STATE_TAPPED | STATE_SUMMONSICK_NOTAP)) == 0 &&
           real_target_available((int *)0, TARGET_SCAN_DIRECT, player, 1 - player, 1 - player,
                                 TARGET_ZONE_IN_PLAY, TYPE_ARTIFACT, TYPE_NONE, 0,
                                 get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0, -1, -1,
                                 -1, -1, 0, 0, 0) != 0;
  }
  else if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_target(0);
  }
  else
  {
    if (event == EVENT_ACTIVATE)
    {
      g_ai_modifier -= 0x30;
      load_text("promptsX2.txt", "HYPERION_BLACKSMITH");
      if (C_real_select_target(player, 1 - player, 1 - player, TARGET_ZONE_IN_PLAY,
                               TYPE_ARTIFACT, TYPE_NONE, 0, get_protections_from(player, card),
                               COLOR_TEST_0, COLOR_TEST_0, -1, -1, -1, -1, 0, 0, 0,
                               g_text_lines[0], 1, &s.target) != 0)
      {
        PLAYER_CARD_INSTANCE(player, card).targets[0] = s.target;
        PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
        PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
        if ((g_land_can_be_played & 0x400) == 0)
        {
          load_text("promptsX2.txt", "HYPERION_BLACKSMITH");
          sprintf(s.dialog, " %s\n %s", g_text_lines[0], g_text_lines[1]);
          PLAYER_CARD_INSTANCE(player, card).info_slot =
              do_dialog(player, player, card, s.target.player, s.target.card, s.dialog,
                        (PLAYER_CARD_INSTANCE(s.target.player, s.target.card).state & STATE_TAPPED) >> 4);
        }
        else
        {
          PLAYER_CARD_INSTANCE(player, card).info_slot =
              PLAYER_CARD_INSTANCE(g_current_spell_player, g_current_spell_card).info_slot;
        }
        if (player == g_other_player)
        {
          if ((global_cards_data[PLAYER_CARD_INSTANCE(s.target.player, s.target.card).internal_card_id].type &
               TYPE_LAND) != 0)
          {
            g_ai_modifier -= 0x18;
          }
          if (PLAYER_CARD_INSTANCE(player, card).info_slot == 1)
          {
            g_ai_modifier -= 0x60;
          }
        }
      }
      else
      {
        g_spell_fizzled = 1;
      }
    }

    if (event == EVENT_RESOLVE_ACTIVATION)
    {
      SET_TARGET(s.target, PLAYER_CARD_INSTANCE(player, card).targets[0]);
      if (C_real_validate_target(s.target.player, s.target.card, (char *)0, player, 1 - player, 1 - player,
                                 TARGET_ZONE_IN_PLAY, TYPE_ARTIFACT, TYPE_NONE, 0,
                                 get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0, -1, -1,
                                 -1, -1, 0, 0, 0) != 0)
      {
        if (PLAYER_CARD_INSTANCE(player, card).info_slot == 0)
        {
          tap_card_and_dispatch_event(s.target.player, s.target.card);
        }
        else
        {
          PLAYER_CARD_INSTANCE(s.target.player, s.target.card).state &= ~STATE_TAPPED;
        }
      }
      else
      {
        g_spell_fizzled = 1;
      }
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                           PLAYER_CARD_INSTANCE(player, card).parent_card)
          .number_of_targets = 0;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x00428404
// FUNCTION: SHANDALAR 0x004370eb
int card_ivory_guardians(int player, int card, event_t event)
{
  int opponent;
  int current_card;
  int color;
  int color_mask;

  if (event == EVENT_ABILITIES && g_affected_card == card && g_affected_card_controller == player)
  {
    g_event_result |= KEYWORD_PROT_BLACK << (get_sleighted_color(player, card, COLOR_RED) - 1);
  }

  if ((event == EVENT_POWER || event == EVENT_TOUGHNESS) &&
      g_affected_card == card && g_affected_card_controller == player &&
      is_in_play(player, card))
  {
    color = get_sleighted_color(player, card, COLOR_RED);
    color_mask = 1 << (unsigned char)color;
    opponent = 1 - player;
    for (current_card = 0; current_card < g_active_cards_count[opponent]; ++current_card)
    {
      if (is_in_play(opponent, current_card) &&
          (global_cards_data[PLAYER_CARD_INSTANCE(opponent, current_card).internal_card_id].type & TARGET_TYPE_PERMANENT) != 0 &&
          (color_mask & PLAYER_CARD_INSTANCE(opponent, current_card).color) != 0)
      {
        ++g_event_result;
        break;
      }
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x004285fb
// FUNCTION: SHANDALAR 0x004372e0
int card_crimson_kobolds(int player, int card, event_t event)
{
  int color;

  color = get_sleighted_color(player, card, COLOR_RED);
  if ((PLAYER_CARD_INSTANCE(player, card).token_status & STATUS_COLOR_FORCED) == 0 &&
      ((1 << (unsigned char)color) & PLAYER_CARD_INSTANCE(player, card).color) == 0)
  {
    color = get_sleighted_color(player, card, COLOR_RED);
    PLAYER_CARD_INSTANCE(player, card).color = (char)(1 << (unsigned char)color);
    if (g_duel_ai_mode_state != 1)
    {
      play_sound_effect(0x27);
    }
  }
  return 0;
}

// FUNCTION: MAGIC 0x004286fb
// FUNCTION: SHANDALAR 0x004373e0
int card_kobold_drill_sergeant(int player, int card, event_t event)
{
  if (g_affected_card_controller == player &&
      global_raw_cards_storage[global_cards_data[PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).internal_card_id].id].subtype ==
          HARDCODED_SUBTYPE_KOBOLD)
  {
    if (is_in_play(g_affected_card_controller, g_affected_card))
    {
      if (event == EVENT_ABILITIES)
      {
        if (is_in_play(player, card))
        {
          g_event_result |= KEYWORD_TRAMPLE;
        }
      }
      if (event == EVENT_TOUGHNESS)
      {
        if (is_in_play(player, card))
        {
          ++g_event_result;
        }
      }
    }
  }
  return 0;
}

// FUNCTION: MAGIC 0x004287de
// FUNCTION: SHANDALAR 0x004374c4
int card_kobold_overlord(int player, int card, event_t event)
{
  if (player == g_affected_card_controller &&
      global_raw_cards_storage[global_cards_data[PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).internal_card_id].id].subtype ==
          HARDCODED_SUBTYPE_KOBOLD)
  {
    if (is_in_play(g_affected_card_controller, g_affected_card) && event == EVENT_ABILITIES)
    {
      if (is_in_play(player, card))
      {
        g_event_result |= KEYWORD_FIRST_STRIKE;
      }
    }
  }
  return 0;
}

// FUNCTION: MAGIC 0x0042889a
// FUNCTION: SHANDALAR 0x0043757f
int card_kobold_taskmaster(int player, int card, event_t event)
{
  if (g_affected_card_controller == player &&
      global_raw_cards_storage[global_cards_data[PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).internal_card_id].id].subtype ==
          HARDCODED_SUBTYPE_KOBOLD)
  {
    if (is_in_play(g_affected_card_controller, g_affected_card) && event == EVENT_POWER)
    {
      if (is_in_play(player, card))
      {
        ++g_event_result;
      }
    }
  }
  return 0;
}

// FUNCTION: MAGIC 0x00428952
// FUNCTION: SHANDALAR 0x00437637
int card_merfolk_assassin(int player, int card, event_t event)
{
  target_t target;
  int hacked_color;
  int required_abilities;

  hacked_color = get_hacked_color(player, card, COLOR_BLUE);
  required_abilities = 1 << (unsigned char)(hacked_color - 1);

  if (event == EVENT_CAN_ACTIVATE)
  {
    return (PLAYER_CARD_INSTANCE(player, card).state & (STATE_TAPPED | STATE_SUMMONSICK_NOTAP)) == 0 &&
           real_target_available((int *)0, TARGET_SCAN_DIRECT, player, 2, 2, TARGET_ZONE_IN_PLAY,
                                 TYPE_CREATURE, TYPE_NONE, required_abilities, get_protections_from(player, card),
                                 COLOR_TEST_0, COLOR_TEST_0, -1, -1, -1, -1, 0, 0, 0) != 0;
  }
  else if (event == EVENT_ACTIVATE)
  {
    if (g_duel_ai_mode_state != 1)
    {
      load_text("promptsX2.txt", "MERFOLK_ASSASSIN");
    }
    if (C_real_select_target(player, 2, 2, TARGET_ZONE_IN_PLAY, TYPE_CREATURE, TYPE_NONE, required_abilities,
                             get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0, -1, -1, -1, -1,
                             0, 0, 0, g_text_lines[0], 1, &target) != 0)
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
  else if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_target(0);
  }
  else if (event == EVENT_RESOLVE_ACTIVATION)
  {
    SET_TARGET(target, PLAYER_CARD_INSTANCE(player, card).targets[0]);
    if (C_real_validate_target(target.player, target.card, (char *)0, player, 2, 2, TARGET_ZONE_IN_PLAY,
                               TYPE_CREATURE, TYPE_NONE, required_abilities, get_protections_from(player, card),
                               COLOR_TEST_0, COLOR_TEST_0, -1, -1, -1, -1, 0, 0, 0) != 0)
    {
      kill_card(target.player, target.card, KILL_DESTROY);
    }
    else
    {
      g_spell_fizzled = 1;
    }
    PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                         PLAYER_CARD_INSTANCE(player, card).parent_card)
        .number_of_targets = 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x00428d2c
// FUNCTION: SHANDALAR 0x00437a11
int card_miracle_worker(int player, int card, event_t event)
{
  struct
  {
    target_t target;
    int attached_card;
    int done;
    int attached_internal_card_id;
  } s;
  int attached_player;

  if (event == EVENT_CAN_ACTIVATE)
  {
    return (PLAYER_CARD_INSTANCE(player, card).state & (STATE_TAPPED | STATE_SUMMONSICK_NOTAP)) == 0 &&
           miracle_worker_has_target(player);
  }
  else if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_target(0);
  }
  else
  {
    if (event == EVENT_ACTIVATE)
    {
      s.done = 0;
      do
      {
        load_text("promptsX2.txt", "MIRACLE_WORKER");
        if (C_real_select_target(player, 2, 2, TARGET_ZONE_IN_PLAY, TYPE_ENCHANTMENT, TYPE_NONE, 0,
                                 get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0, -1, -1,
                                 -1, -1, 0, 0, 0, g_text_lines[0], 1, &s.target) == 0)
        {
          g_spell_fizzled = 1;
        }
        else
        {
          attached_player = PLAYER_CARD_INSTANCE(s.target.player, s.target.card).damage_target_player;
          s.attached_card = PLAYER_CARD_INSTANCE(s.target.player, s.target.card).damage_target_card;
          if (attached_player == -1 ||
              s.attached_card == -1 ||
              (global_cards_data[PLAYER_CARD_INSTANCE(attached_player, s.attached_card).internal_card_id].type &
               TYPE_CREATURE) == 0)
          {
            if (g_duel_ai_mode_state == 1)
            {
              rewind_recorded_action();
            }
            else
            {
              set_duel_prompt_text(g_text_lines[1]);
              Sleep(0x9c4);
              set_duel_prompt_text("");
            }
          }
          else if (attached_player == player)
          {
            PLAYER_CARD_INSTANCE(player, card).targets[0] = s.target;
            PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
            PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
            s.done = 1;
            if (player == g_other_player)
            {
              if (s.target.player == g_active_player)
              {
                g_ai_modifier += 0x30;
              }
              else
              {
                g_ai_modifier -= 0x30;
              }
            }
          }
          else if (g_duel_ai_mode_state == 1)
          {
            rewind_recorded_action();
          }
          else
          {
            set_duel_prompt_text(g_text_lines[2]);
            Sleep(0x9c4);
            set_duel_prompt_text("");
          }
        }
      } while (g_spell_fizzled != 1 && s.done == 0);
    }

    if (event == EVENT_RESOLVE_ACTIVATION)
    {
      SET_TARGET(s.target, PLAYER_CARD_INSTANCE(player, card).targets[0]);
      if (C_real_validate_target(s.target.player, s.target.card, (char *)0, player, 2, 2,
                                 TARGET_ZONE_IN_PLAY, TYPE_ENCHANTMENT, TYPE_NONE, 0,
                                 get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0,
                                 -1, -1, -1, -1, 0, 0, 0) == 0)
      {
        g_spell_fizzled = 1;
      }
      else
      {
        attached_player = PLAYER_CARD_INSTANCE(s.target.player, s.target.card).damage_target_player;
        s.attached_card = PLAYER_CARD_INSTANCE(s.target.player, s.target.card).damage_target_card;
        if (attached_player == -1 || s.attached_card == -1)
        {
          g_spell_fizzled = 1;
        }
        else
        {
          s.attached_internal_card_id =
              PLAYER_CARD_INSTANCE(attached_player, s.attached_card).internal_card_id;
          if (s.attached_internal_card_id == -1 ||
              (global_cards_data[s.attached_internal_card_id].type & TYPE_CREATURE) == 0)
          {
            g_spell_fizzled = 1;
          }
          else
          {
            kill_card(s.target.player, s.target.card, KILL_DESTROY);
          }
        }
      }
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                           PLAYER_CARD_INSTANCE(player, card).parent_card)
          .number_of_targets = 0;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x00429361
// FUNCTION: SHANDALAR 0x00438047
int card_mold_demon(int player, int card, event_t event)
{
  char dialog[600];
  int color;
  int choice;

  if (g_trigger_condition == 0xdb &&
      g_affected_card == card &&
      g_affected_card_controller == player &&
      g_current_player == g_current_turn &&
      g_trigger_cause_controller == player &&
      g_trigger_cause == card)
  {
    if (event == EVENT_TRIGGER)
    {
      g_event_result |= RESOLVE_TRIGGER_MANDATORY;
    }
    if (event == EVENT_RESOLVE_TRIGGER)
    {
      if (g_duel_ai_mode_state != 1)
      {
        load_text("promptsX2.txt", "MOLD_DEMON");
      }
      color = get_hacked_color(player, card, COLOR_BLACK);
      sprintf(dialog, " %s\n %s", g_text_lines[0], g_text_lines[color]);
      color = get_hacked_color(player, card, COLOR_BLACK);
      if (g_basiclandtypes_controlled[player][color] > 1)
      {
        color = get_hacked_color(player, card, COLOR_BLACK);
        choice = do_dialog(player,
                           player,
                           card,
                           -1,
                           -1,
                           dialog,
                           g_basiclandtypes_controlled[player][color] > 1);
        if (choice != 0)
        {
          color = get_hacked_color(player, card, COLOR_BLACK);
          TENTATIVE_sacrifice_basic_land_type(player, card, 2, color, 0);
          return 0;
        }
      }
      kill_card(player, card, KILL_BURY);
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    if (g_duel_ai_mode_state != 1)
    {
      load_text("promptsX2.txt", "MOLD_DEMON");
    }
    color = get_hacked_color(player, card, COLOR_BLACK);
    sprintf(dialog, " %s\n %s", g_text_lines[0], g_text_lines[color]);
    color = get_hacked_color(player, card, COLOR_BLACK);
    if (g_basiclandtypes_controlled[player][color] > 1)
    {
      color = get_hacked_color(player, card, COLOR_BLACK);
      choice = do_dialog(player,
                         player,
                         card,
                         -1,
                         -1,
                         dialog,
                         g_basiclandtypes_controlled[player][color] > 1);
      if (choice != 0)
      {
        color = get_hacked_color(player, card, COLOR_BLACK);
        TENTATIVE_sacrifice_basic_land_type(player, card, 2, color, 0);
        return 0;
      }
    }
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x00429617
// FUNCTION: SHANDALAR 0x004382fd
int card_people_of_the_woods(int player, int card, event_t event)
{
  int legacy_card;

  if (event == EVENT_RESOLVE_SPELL)
  {
    legacy_card = create_legacy_effect(player, card, g_duel_generated_internal_card_id_1a, player, card);
    if (legacy_card != -1)
    {
      PLAYER_CARD_INSTANCE(player, legacy_card).info_slot = 3;
      PLAYER_CARD_INSTANCE(player, legacy_card).eot_toughness = 0x109;
      PLAYER_CARD_INSTANCE(player, legacy_card).token_status = STATUS_INVISIBLE_FX | STATUS_PERMANENT;
      PLAYER_CARD_INSTANCE(player, legacy_card).token_status |= 0x2000000;
      PLAYER_CARD_INSTANCE(player, card).damage_source_player = (char)player;
      PLAYER_CARD_INSTANCE(player, card).damage_source_card = legacy_card;
    }
  }
  return 0;
}

// FUNCTION: MAGIC 0x0042978b
// FUNCTION: SHANDALAR 0x00438471
int card_pixie_queen(int player, int card, event_t event)
{
  target_t target;
  int legacy_card;

  if (event == EVENT_CAST_SPELL && g_affected_card == card && g_affected_card_controller == player)
  {
    g_ai_modifier += 12;
  }
  else if (event == EVENT_CAN_ACTIVATE)
  {
    return (PLAYER_CARD_INSTANCE(player, card).state & (STATE_TAPPED | STATE_SUMMONSICK_NOTAP)) == 0 &&
           has_mana(player, COLOR_GREEN, 3) &&
           real_target_available((int *)0, TARGET_SCAN_DIRECT, player, 2, player, TARGET_ZONE_IN_PLAY,
                                 TYPE_CREATURE, TYPE_NONE, 0, get_protections_from(player, card),
                                 COLOR_TEST_0, COLOR_TEST_0, -1, -1, -1, -1, 0, 0, 0) != 0;
  }
  else if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_target(0);
  }
  else
  {
    if (event == EVENT_ACTIVATE)
    {
      charge_mana(player, COLOR_GREEN, 3);
      if (g_spell_fizzled != 1)
      {
        if (g_duel_ai_mode_state != 1)
        {
          load_text("promptsX2.txt", "PIXIE_QUEEN");
        }
        if (C_real_select_target(player, 2, player, TARGET_ZONE_IN_PLAY, TYPE_CREATURE, TYPE_NONE, 0,
                                 get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0, -1, -1, -1,
                                 -1, 0, 0, 0, g_text_lines[0], 1, &target) != 0)
        {
          SET_TARGET(PLAYER_CARD_INSTANCE(player, card).targets[0], target);
          PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
          if (g_other_player == player &&
              (PLAYER_CARD_INSTANCE(target.player, target.card).regen_status & KEYWORD_FLYING) != 0)
          {
            g_ai_modifier -= 96;
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
      if (C_real_validate_target(target.player, target.card, (char *)0, player, 2, 2,
                                 TARGET_ZONE_IN_PLAY, TYPE_CREATURE, TYPE_NONE, 0,
                                 get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0, -1, -1, -1,
                                 -1, 0, 0, 0) == 0)
      {
        g_spell_fizzled = 1;
      }
      else
      {
        PLAYER_CARD_INSTANCE(target.player, target.card).regen_status = KEYWORD_RECALC_ABILITIES;
        legacy_card = create_legacy_effect(g_card_on_stack_controller, g_card_on_stack, g_duel_generated_internal_card_id_03,
                                           target.player, target.card);
        if (legacy_card != -1)
        {
          PLAYER_CARD_INSTANCE(player, legacy_card).info_slot = KEYWORD_FLYING;
        }
      }
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                           PLAYER_CARD_INSTANCE(player, card).parent_card)
          .number_of_targets = 0;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x00429d2d
// FUNCTION: SHANDALAR 0x00438a16
int card_rabid_wombat(int player, int card, event_t event)
{
  int current_player;
  int current_card;

  if (g_affected_card == card && g_affected_card_controller == player)
  {
    PLAYER_CARD_INSTANCE(player, card).state |= STATE_VIGILANCE;
  }

  if ((event == EVENT_POWER || event == EVENT_TOUGHNESS) &&
      g_affected_card == card && g_affected_card_controller == player)
  {
    for (current_player = 0; current_player < 2; ++current_player)
    {
      for (current_card = 0; current_card < g_active_cards_count[current_player]; ++current_card)
      {
        if (is_in_play(current_player, current_card) &&
            global_raw_cards_storage[global_cards_data[PLAYER_CARD_INSTANCE(current_player, current_card).internal_card_id].id].card_type ==
                CP_TYPE_ENCHANTMENT &&
            PLAYER_CARD_INSTANCE(current_player, current_card).damage_target_player == player &&
            PLAYER_CARD_INSTANCE(current_player, current_card).damage_target_card == card)
        {
          g_event_result += 2;
        }
      }
    }
  }
  return 0;
}

// FUNCTION: MAGIC 0x00429ee1
// FUNCTION: SHANDALAR 0x00438bce
int card_savaen_elves(int player, int card, event_t event)
{
  struct
  {
    target_t target;
    int attached_card;
    int done;
    int attached_internal_card_id;
  } s;
  int attached_player;

  if (event == EVENT_CAN_ACTIVATE)
  {
    load_recorded_action_target(0);
    return (PLAYER_CARD_INSTANCE(player, card).state & (STATE_TAPPED | STATE_SUMMONSICK_NOTAP)) == 0 &&
           has_mana(player, COLOR_GREEN, 2) &&
           savaen_elves_has_target();
  }
  else
  {
    if (event == EVENT_ACTIVATE)
    {
      charge_mana(player, COLOR_GREEN, 2);
      if (g_spell_fizzled != 1)
      {
        s.done = 0;
        do
        {
          load_text("promptsX2.txt", "SAVAEN_ELVES");
          if (C_real_select_target(player, 2, 2, TARGET_ZONE_IN_PLAY, TYPE_ENCHANTMENT, TYPE_NONE, 0,
                                   get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0, -1, -1,
                                   -1, -1, 0, 0, 0, g_text_lines[0], 1, &s.target) == 0)
          {
            g_spell_fizzled = 1;
          }
          else
          {
            attached_player = PLAYER_CARD_INSTANCE(s.target.player, s.target.card).damage_target_player;
            s.attached_card = PLAYER_CARD_INSTANCE(s.target.player, s.target.card).damage_target_card;
            if (attached_player == -1 ||
                s.attached_card == -1 ||
                (global_cards_data[PLAYER_CARD_INSTANCE(attached_player, s.attached_card).internal_card_id].type &
                 TYPE_LAND) == 0)
            {
              if (g_duel_ai_mode_state == 1)
              {
                rewind_recorded_action();
              }
              else
              {
                set_duel_prompt_text(g_text_lines[1]);
                Sleep(0x9c4);
                set_duel_prompt_text("");
              }
            }
            else
            {
              PLAYER_CARD_INSTANCE(player, card).targets[0] = s.target;
              PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
              PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
              s.done = 1;
              if (player == g_other_player)
              {
                if (s.target.player == g_active_player)
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
        } while (g_spell_fizzled != 1 && s.done == 0);
      }
    }

    if (event == EVENT_RESOLVE_ACTIVATION)
    {
      SET_TARGET(s.target, PLAYER_CARD_INSTANCE(player, card).targets[0]);
      if (C_real_validate_target(s.target.player, s.target.card, (char *)0, player, 2, 2,
                                 TARGET_ZONE_IN_PLAY, TYPE_ENCHANTMENT, TYPE_NONE, 0,
                                 get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0,
                                 -1, -1, -1, -1, 0, 0, 0) == 0)
      {
        g_spell_fizzled = 1;
      }
      else
      {
        attached_player = PLAYER_CARD_INSTANCE(s.target.player, s.target.card).damage_target_player;
        s.attached_card = PLAYER_CARD_INSTANCE(s.target.player, s.target.card).damage_target_card;
        if (attached_player == -1 || s.attached_card == -1)
        {
          g_spell_fizzled = 1;
        }
        else
        {
          s.attached_internal_card_id =
              PLAYER_CARD_INSTANCE(attached_player, s.attached_card).internal_card_id;
          if (s.attached_internal_card_id == -1 ||
              (global_cards_data[s.attached_internal_card_id].type & TYPE_LAND) == 0)
          {
            g_spell_fizzled = 1;
          }
          else
          {
            kill_card(s.target.player, s.target.card, KILL_DESTROY);
          }
        }
      }
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                           PLAYER_CARD_INSTANCE(player, card).parent_card)
          .number_of_targets = 0;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0042a5b3
// FUNCTION: SHANDALAR 0x004392a0
int card_scavenger_folk(int player, int card, event_t event)
{
  target_t target;

  if (event == EVENT_CAN_ACTIVATE)
  {
    return (PLAYER_CARD_INSTANCE(player, card).state & (STATE_TAPPED | STATE_SUMMONSICK_NOTAP)) == 0 &&
           has_mana(player, COLOR_GREEN, 1) &&
           real_target_available((int *)0, TARGET_SCAN_DIRECT, player, 2, 2, TARGET_ZONE_IN_PLAY,
                                 TYPE_ARTIFACT, TYPE_NONE, 0, get_protections_from(player, card),
                                 COLOR_TEST_0, COLOR_TEST_0, -1, -1, -1, -1, 0, 0, 0) != 0;
  }
  else if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_target(0);
  }
  else
  {
    if (event == EVENT_ACTIVATE)
    {
      charge_mana(player, COLOR_GREEN, 1);
      if (g_spell_fizzled != 1)
      {
        load_text("promptsX2.txt", "SCAVENGER_FOLK");
        if (C_real_select_target(player, 2, 2, TARGET_ZONE_IN_PLAY, TYPE_ARTIFACT, TYPE_NONE, 0,
                                 get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0, -1, -1, -1, -1,
                                 0, 0, 0, g_text_lines[0], 1, &target) != 0)
        {
          SET_TARGET(PLAYER_CARD_INSTANCE(player, card).targets[0], target);
          PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
          PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
          kill_card(player, card, KILL_SACRIFICE);
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
                                 TYPE_ARTIFACT, TYPE_NONE, 0, get_protections_from(player, card),
                                 COLOR_TEST_0, COLOR_TEST_0, -1, -1, -1, -1, 0, 0, 0) != 0)
      {
        kill_card(target.player, target.card, KILL_DESTROY);
      }
      else
      {
        g_spell_fizzled = 1;
      }
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                           PLAYER_CARD_INSTANCE(player, card).parent_card)
          .number_of_targets = 0;
    }
  }
  return 0;
}

// FUNCTION: MAGIC 0x0042a968
// FUNCTION: SHANDALAR 0x00439655
int card_spinal_villain(int player, int card, event_t event)
{
  target_t target;
  int sleighted_color;
  color_test_t required_color;

  sleighted_color = get_sleighted_color(player, card, COLOR_BLUE);
  required_color = 1 << (unsigned char)sleighted_color;

  if (event == EVENT_CAN_ACTIVATE)
  {
    return (PLAYER_CARD_INSTANCE(player, card).state & (STATE_TAPPED | STATE_SUMMONSICK_NOTAP)) == 0 &&
           real_target_available((int *)0, TARGET_SCAN_DIRECT, player, 2, 2, TARGET_ZONE_IN_PLAY,
                                 TYPE_CREATURE, TYPE_NONE, 0, get_protections_from(player, card),
                                 required_color, COLOR_TEST_0, -1, -1, -1, -1, 0, 0, 0) != 0;
  }
  else if (event == EVENT_ACTIVATE)
  {
    if (g_duel_ai_mode_state != 1)
    {
      load_text("promptsX2.txt", "SPINAL_VILLIAN");
    }
    if (C_real_select_target(player, 2, 2, TARGET_ZONE_IN_PLAY, TYPE_CREATURE, TYPE_NONE, 0,
                             get_protections_from(player, card), required_color, COLOR_TEST_0, -1, -1, -1, -1,
                             0, 0, 0, g_text_lines[0], 1, &target) != 0)
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
  else if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_target(0);
  }
  else if (event == EVENT_RESOLVE_ACTIVATION)
  {
    SET_TARGET(target, PLAYER_CARD_INSTANCE(player, card).targets[0]);
    if (C_real_validate_target(target.player, target.card, (char *)0, player, 2, 2, TARGET_ZONE_IN_PLAY,
                               TYPE_CREATURE, TYPE_NONE, 0, get_protections_from(player, card),
                               required_color, COLOR_TEST_0, -1, -1, -1, -1, 0, 0, 0) == 0)
    {
      g_spell_fizzled = 1;
    }
    else
    {
      kill_card(target.player, target.card, KILL_DESTROY);
    }
    PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                         PLAYER_CARD_INSTANCE(player, card).parent_card)
        .number_of_targets = 0;
  }
  else if (event == EVENT_ATTACK_RATING && g_affected_card == card && g_affected_card_controller == player &&
           g_basiclandtypes_controlled[1 - player][sleighted_color] != 0)
  {
    g_ai_score += 0x18;
  }
  else if (event == EVENT_BLOCK_RATING && g_affected_card == card && g_affected_card_controller == player &&
           g_basiclandtypes_controlled[1 - player][sleighted_color] != 0)
  {
    g_ai_score -= 0x18;
  }
  else if (event == EVENT_SHOULD_AI_PLAY && g_current_phase == PHASE_CLEANUP &&
           g_affected_card == card && g_affected_card_controller == player)
  {
    g_ai_modifier += g_basiclandtypes_controlled[1 - player][sleighted_color] * 0xc;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0042ae69
// FUNCTION: SHANDALAR 0x00439b5c
int card_drowned(int player, int card, event_t event)
{
  int hacked_color;

  if (event == EVENT_UNTAP_PHASE)
  {
    hacked_color = get_hacked_color(player, card, 1);
    ++g_ai_mana_demand_by_color[player][hacked_color];
  }

  if (event == EVENT_CAN_ACTIVATE || event == EVENT_ACTIVATE || event == EVENT_RESOLVE_ACTIVATION)
  {
    return generic_regeneration_ability(player, card, event, 1, 1);
  }

  return 0;
}

// FUNCTION: MAGIC 0x0042aee8
// FUNCTION: SHANDALAR 0x00439bdb
int card_tracker(int player, int card, event_t event)
{
  target_t target;

  if (event == EVENT_CAN_ACTIVATE)
  {
    return ((PLAYER_CARD_INSTANCE(player, card).state & (STATE_TAPPED | STATE_SUMMONSICK_NOTAP)) == 0 &&
            has_mana(player, COLOR_GREEN, 2) &&
            real_target_available((int *)0, TARGET_SCAN_DIRECT, player, 2, 1 - player, TARGET_ZONE_IN_PLAY,
                                  TYPE_CREATURE, TYPE_NONE, 0, get_protections_from(player, card),
                                  COLOR_TEST_0, COLOR_TEST_0, -1, -1, -1, -1, 0, 0, 0) != 0);
  }

  if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_target(0);
  }
  else
  {
    if (event == EVENT_ACTIVATE)
    {
      charge_mana(player, COLOR_GREEN, 2);
      if (g_spell_fizzled != 1)
      {
        if (g_duel_ai_mode_state != 1)
        {
          load_text("promptsX2.txt", "TRACKER");
        }
        if (C_real_select_target(player, 2, 1 - player, TARGET_ZONE_IN_PLAY, TYPE_CREATURE, TYPE_NONE, 0,
                                 get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0, -1, -1, -1,
                                 -1, 0, 0, 0, g_text_lines[0], 1, &target) != 0)
        {
          SET_TARGET(PLAYER_CARD_INSTANCE(player, card).targets[0], target);
          PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
          PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
          PLAYER_CARD_INSTANCE(target.player, target.card).regen_status |= KEYWORD_RECALC_POWER;
          PLAYER_CARD_INSTANCE(player, card).regen_status |= KEYWORD_RECALC_POWER;
          PLAYER_CARD_INSTANCE(player, card).info_slot =
              C_get_abilities(target.player, target.card, EVENT_POWER, -1);
          PLAYER_CARD_INSTANCE(player, card).eot_toughness =
              C_get_abilities(player, card, EVENT_POWER, -1);
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
      if (C_real_validate_target(target.player, target.card, (char *)0, player, 2, 2,
                                 TARGET_ZONE_IN_PLAY, TYPE_CREATURE, TYPE_NONE, 0,
                                 get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0, -1, -1, -1,
                                 -1, 0, 0, 0) != 0)
      {
        damage_creature(target.player, target.card, PLAYER_CARD_INSTANCE(player, card).eot_toughness,
                        g_card_on_stack_controller, g_card_on_stack);
        if (PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                                 PLAYER_CARD_INSTANCE(player, card).parent_card)
                .internal_card_id != -1)
        {
          damage_creature(g_card_on_stack_controller, g_card_on_stack,
                          PLAYER_CARD_INSTANCE(player, card).info_slot, target.player, target.card);
        }
      }
      else
      {
        g_spell_fizzled = 1;
      }
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                           PLAYER_CARD_INSTANCE(player, card).parent_card)
          .number_of_targets = 0;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0042b4ab
// FUNCTION: SHANDALAR 0x0043a19e
int card_wall_of_opposition(int player, int card, event_t event)
{
  int legacy_card;

  if (event == EVENT_CAST_SPELL &&
      card == g_affected_card &&
      player == g_affected_card_controller)
  {
    PLAYER_CARD_INSTANCE(player, card).eot_toughness = 0;
    PLAYER_CARD_INSTANCE(player, card).info_slot = PLAYER_CARD_INSTANCE(player, card).eot_toughness;
    return 0;
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    return has_mana(player, COLOR_ANY, 1);
  }

  if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_code(0);
    return 0;
  }

  if (event == EVENT_ACTIVATE)
  {
    if (has_mana(player, COLOR_ANY, 1) != 0)
    {
      if (g_current_player == player)
      {
        charge_mana(player, COLOR_COLORLESS, -1);
        if (g_x_value > 0)
        {
          PLAYER_CARD_INSTANCE(player, card).eot_toughness = g_x_value;
        }
        else
        {
          g_spell_fizzled = 1;
        }
      }
      else
      {
        charge_mana(player, COLOR_COLORLESS, 1);
        PLAYER_CARD_INSTANCE(player, card).eot_toughness = 1;
      }

      if (g_spell_fizzled != 1)
      {
        PLAYER_CARD_INSTANCE(player, card).targets[0].player = player;
        PLAYER_CARD_INSTANCE(player, card).targets[0].card = card;
        PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
        if (PLAYER_CARD_INSTANCE(player, card).info_slot == 0)
        {
          PLAYER_CARD_INSTANCE(player, card).info_slot |= 0x80000;
        }
      }
      else
      {
        PLAYER_CARD_INSTANCE(player, card).eot_toughness = 0;
      }
    }
    return 0;
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    if (PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                             PLAYER_CARD_INSTANCE(player, card).parent_card)
            .internal_card_id != -1)
    {
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                           PLAYER_CARD_INSTANCE(player, card).parent_card)
          .info_slot += (PLAYER_CARD_INSTANCE(player, card).eot_toughness & 0xff);
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
        legacy_card = create_legacy_effect(g_card_on_stack_controller, g_card_on_stack, LEGACY_EFFECT_PUMP,
                                           g_card_on_stack_controller, g_card_on_stack);
        if (legacy_card != -1)
        {
          PLAYER_CARD_INSTANCE(player, legacy_card).counter_power = 1;
          PLAYER_CARD_INSTANCE(player, legacy_card).counter_toughness = 1;
          PLAYER_CARD_INSTANCE(player, legacy_card).info_slot |= 0x80000;
        }
        else
        {
          g_spell_fizzled = 1;
        }
      }
    }
    else
    {
      g_spell_fizzled = 1;
    }
    return 0;
  }

  if (event == EVENT_POW_BOOST)
  {
    return has_mana(player, COLOR_ANY, 1);
  }

  if (event == EVENT_CAN_WASTE_MANA)
  {
    g_event_result |= 1;
    return 0;
  }

  if (event == EVENT_SHOULD_AI_PLAY && g_current_phase == PHASE_DISCARD)
  {
    if (player == g_other_player)
    {
      g_ai_modifier += (g_basiclandtypes_controlled[player][COLOR_ANY] * 3 + 6) * 4;
    }
    else
    {
      g_ai_modifier -= (g_basiclandtypes_controlled[player][COLOR_ANY] * 3 + 6) * 4;
    }
  }

  if (event == EVENT_CLEANUP || event == EVENT_SHOULD_AI_PLAY)
  {
    PLAYER_CARD_INSTANCE(player, card).eot_toughness = 0;
    PLAYER_CARD_INSTANCE(player, card).info_slot = PLAYER_CARD_INSTANCE(player, card).eot_toughness;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0042bcd9
// FUNCTION: SHANDALAR 0x0043a9c9
int card_wall_of_tombstones(int player, int card, event_t event)
{
  int graveyard_index;

  if (g_trigger_condition == 0xcb && g_affected_card == card &&
      g_affected_card_controller == player &&
      PLAYER_CARD_INSTANCE(player, card).eot_toughness == 0)
  {
    ++PLAYER_CARD_INSTANCE(player, card).eot_toughness;
    if (player == g_current_player && player == g_current_turn)
    {
      PLAYER_CARD_INSTANCE(player, card).info_slot = 0;
      for (graveyard_index = 0; global_graveyard_slots[player][graveyard_index] != -1; ++graveyard_index)
      {
        if (global_cards_data[global_graveyard_slots[player][graveyard_index]].type & TYPE_CREATURE)
        {
          ++PLAYER_CARD_INSTANCE(player, card).info_slot;
        }
      }
      PLAYER_CARD_INSTANCE(player, card).regen_status |= KEYWORD_RECALC_TOUGHNESS;
      C_get_abilities(player, card, EVENT_TOUGHNESS, -1);
    }
    PLAYER_CARD_INSTANCE(player, card).eot_toughness = 0;
  }

  if (event == EVENT_TOUGHNESS && g_affected_card == card && g_affected_card_controller == player)
  {
    g_event_result += PLAYER_CARD_INSTANCE(player, card).info_slot;
  }
  return 0;
}

// FUNCTION: MAGIC 0x0042bf2d
// FUNCTION: SHANDALAR 0x0043ac21
int card_wall_of_wonder(int player, int card, event_t event)
{
  int legacy_effect_card;

  if (event == EVENT_UNTAP_PHASE)
  {
    g_ai_mana_demand_by_color[player][COLOR_BLUE] += 2;
    return 0;
  }

  if (event == EVENT_CAST_SPELL && g_affected_card == card && g_affected_card_controller == player)
  {
    PLAYER_CARD_INSTANCE(player, card).eot_toughness = 0;
    PLAYER_CARD_INSTANCE(player, card).info_slot = PLAYER_CARD_INSTANCE(player, card).eot_toughness;
    return 0;
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    return (has_mana(player, COLOR_BLUE, 2) && has_mana(player, COLOR_ANY, 4));
  }

  if (event == EVENT_ACTIVATE)
  {
    g_mana_charge[COLOR_COLORLESS] = 2;
    charge_mana(player, COLOR_BLUE, 2);
    if (g_spell_fizzled != 1)
    {
      PLAYER_CARD_INSTANCE(player, card).targets[0].player = player;
      PLAYER_CARD_INSTANCE(player, card).targets[0].card = card;
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
      if (PLAYER_CARD_INSTANCE(player, card).info_slot == 0)
      {
        PLAYER_CARD_INSTANCE(player, card).info_slot |= 0x80000;
      }
    }

    return 0;
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    if (PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                             PLAYER_CARD_INSTANCE(player, card).parent_card)
            .internal_card_id != -1)
    {
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                           PLAYER_CARD_INSTANCE(player, card).parent_card)
          .info_slot += 4;
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                           PLAYER_CARD_INSTANCE(player, card).parent_card)
          .info_slot += 0xfc00;
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
            .info_slot &= ~0x80000;

        legacy_effect_card = create_legacy_effect(g_card_on_stack_controller,
                                                  g_card_on_stack,
                                                  LEGACY_EFFECT_PUMP,
                                                  g_card_on_stack_controller,
                                                  g_card_on_stack);
        if (legacy_effect_card != -1)
        {
          PLAYER_CARD_INSTANCE(player, legacy_effect_card).info_slot |= 0x80000;
        }
      }
      ++PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                             PLAYER_CARD_INSTANCE(player, card).parent_card)
            .eot_toughness;
    }
    else
    {
      g_spell_fizzled = 1;
    }

    return 0;
  }

  if (event == EVENT_ABILITIES && g_affected_card == card && g_affected_card_controller == player &&
      PLAYER_CARD_INSTANCE(player, card).eot_toughness != 0)
  {
    PLAYER_CARD_INSTANCE(player, card).token_status |= STATUS_WALL_CAN_ATTACK;
  }

  if (event == EVENT_POW_BOOST)
  {
    return (has_mana(player, COLOR_BLUE, 2) && has_mana(player, COLOR_ANY, 4));
  }

  if (event == EVENT_CLEANUP || event == EVENT_SHOULD_AI_PLAY)
  {
    PLAYER_CARD_INSTANCE(player, card).token_status &= ~STATUS_WALL_CAN_ATTACK;
    PLAYER_CARD_INSTANCE(player, card).eot_toughness = 0;
    PLAYER_CARD_INSTANCE(player, card).info_slot = PLAYER_CARD_INSTANCE(player, card).eot_toughness;
  }

  return 0;
}
// FUNCTION: MAGIC 0x0042c77e
// FUNCTION: SHANDALAR 0x0043b476
int card_water_wurm(int player, int card, event_t event)
{
  if (card == g_affected_card && player == g_affected_card_controller)
  {
    if (0 < g_basiclandtypes_controlled[1 - player][get_hacked_color(player, card, COLOR_BLUE)] &&
        event == EVENT_TOUGHNESS)
    {
      ++g_event_result;
    }
  }
  return 0;
}

// FUNCTION: MAGIC 0x0042c7e7
// FUNCTION: SHANDALAR 0x0043b4e1
int card_witch_hunter(int player, int card, event_t event)
{
  struct
  {
    target_t target;
    char dialog[596];
  } s;
  int bounce_available;

  if (event == EVENT_CAN_ACTIVATE)
  {
    return (PLAYER_CARD_INSTANCE(player, card).state & (STATE_TAPPED | STATE_SUMMONSICK_NOTAP)) == 0;
  }
  else if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_target(0);
  }
  else
  {
    if (event == EVENT_ACTIVATE)
    {
      g_ai_recorded_choice = 0;
      PLAYER_CARD_INSTANCE(player, card).info_slot = 0;
      if (g_duel_ai_mode_state != 1)
      {
        load_text("promptsX2.txt", "WITCH_HUNTER");
      }

      bounce_available =
          has_mana(player, COLOR_WHITE, 2) &&
          has_mana(player, COLOR_ANY, 3) &&
          real_target_available((int *)0, TARGET_SCAN_DIRECT, player, 1 - player, 1 - player,
                                TARGET_ZONE_IN_PLAY, TYPE_CREATURE, TYPE_NONE, 0,
                                get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0,
                                -1, -1, -1, -1, 0, 0, 0) != 0;
      if (bounce_available)
      {
        if (player == g_other_player && (g_duel_network_flags & 2) == 0)
        {
          if (g_duel_ai_mode_state == 1)
          {
            g_ai_recorded_choice = internal_rand(2);
            record_ai_action_selection();
          }
          else
          {
            replay_ai_action_selection();
          }
        }
        sprintf(s.dialog, " %s\n %s", g_text_lines[0], g_text_lines[1]);
        PLAYER_CARD_INSTANCE(player, card).info_slot =
            do_dialog(player, player, card, -1, -1, s.dialog, g_ai_recorded_choice);
      }

      if (PLAYER_CARD_INSTANCE(player, card).info_slot == 0)
      {
        g_spell_fizzled =
            C_real_select_target(player, 2, 1 - player, TARGET_ZONE_PLAYERS, TYPE_NONE, TYPE_NONE,
                                 0, 0, COLOR_TEST_0, COLOR_TEST_0, -1, -1, -1, -1, 0, 0, 0,
                                 g_text_lines[2], 1, &s.target) == 0;
      }
      else
      {
        g_mana_charge[COLOR_COLORLESS] = 1;
        charge_mana(player, COLOR_WHITE, 2);
        if (g_spell_fizzled != 1)
        {
          if (C_real_select_target(player, 1 - player, 1 - player, TARGET_ZONE_IN_PLAY,
                                   TYPE_CREATURE, TYPE_NONE, 0, get_protections_from(player, card),
                                   COLOR_TEST_0, COLOR_TEST_0, -1, -1, -1, -1, 0, 0, 0,
                                   g_text_lines[3], 1, &s.target) == 0)
          {
            g_spell_fizzled = 1;
          }
          else
          {
            g_spell_fizzled = 0;
          }
        }
      }

      if (g_spell_fizzled != 1)
      {
        PLAYER_CARD_INSTANCE(player, card).targets[0] = s.target;
        PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
        PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
      }
    }

    if (event == EVENT_RESOLVE_ACTIVATION)
    {
      SET_TARGET(s.target, PLAYER_CARD_INSTANCE(player, card).targets[0]);
      if (PLAYER_CARD_INSTANCE(player, card).info_slot == 0)
      {
        damage_player(s.target.player, 1, g_card_on_stack_controller, g_card_on_stack);
      }
      else if (C_real_validate_target(s.target.player, s.target.card, (char *)0, player,
                                      1 - player, 1 - player, TARGET_ZONE_IN_PLAY,
                                      TYPE_CREATURE, TYPE_NONE, 0,
                                      get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0,
                                      -1, -1, -1, -1, 0, 0, 0) != 0)
      {
        hurkyls_recall_bounce_artifact(s.target.player, s.target.card);
      }
      else
      {
        g_spell_fizzled = 1;
      }
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                           PLAYER_CARD_INSTANCE(player, card).parent_card)
          .number_of_targets = 0;
    }
  }

  if (event == EVENT_CHECK_PUMP &&
      (PLAYER_CARD_INSTANCE(player, card).state & 0x20014) == 0)
  {
    --g_global_toughness_bonus[1 - player];
  }

  if (event == 199 &&
      player == g_current_player &&
      player == g_other_player &&
      (PLAYER_CARD_INSTANCE(player, card).state & (STATE_TAPPED | STATE_SUMMONSICK_NOTAP)) == 0)
  {
    g_ai_modifier += 0x18;
  }

  if (event == EVENT_ATTACK_RATING &&
      card == g_affected_card &&
      player == g_affected_card_controller)
  {
    g_ai_score += 0x30;
  }
  else if (event == EVENT_BLOCK_RATING &&
           card == g_affected_card &&
           player == g_affected_card_controller)
  {
    g_ai_score -= 0x30;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0042d144
// FUNCTION: SHANDALAR 0x0043be37
int card_wormwood_treefolk(int player, int card, event_t event)
{
  struct
  {
    int green_mana;
    char dialog[600];
    int default_choice;
    int legacy_card;
    int black_mana;
  } s;

  if (event == EVENT_CAN_ACTIVATE)
  {
    return ((PLAYER_CARD_INSTANCE(player, card).state & (STATE_TAPPED | STATE_SUMMONSICK_NOTAP)) == 0 &&
            (has_mana(player, COLOR_GREEN, 2) != 0 || has_mana(player, COLOR_BLACK, 2) != 0));
  }

  if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_target(0);
    return 0;
  }

  if (event == EVENT_ACTIVATE)
  {
    s.green_mana = has_mana(player, COLOR_GREEN, 1);
    s.black_mana = has_mana(player, COLOR_BLACK, 1);
    PLAYER_CARD_INSTANCE(player, card).info_slot = 0;

    if (s.green_mana > 1 && s.black_mana > 1)
    {
      if (g_other_player == player && (g_duel_network_flags & 2) == 0)
      {
        if (s.green_mana < s.black_mana)
        {
          s.default_choice = 1;
        }
        else
        {
          s.default_choice = 0;
        }
      }

      if (g_duel_ai_mode_state != 1)
      {
        load_text("promptsX2.txt", "WORMWOOD_TREEFOLK");
        sprintf(s.dialog, " %s\n %s", g_text_lines[0], g_text_lines[1]);
      }

      PLAYER_CARD_INSTANCE(player, card).info_slot =
          do_dialog(player, player, card, -1, -1, s.dialog, s.default_choice);
    }
    else if (s.black_mana > 1)
    {
      PLAYER_CARD_INSTANCE(player, card).info_slot = 1;
    }

    if (PLAYER_CARD_INSTANCE(player, card).info_slot != 0)
    {
      charge_mana(player, COLOR_BLACK, 2);
    }
    else
    {
      charge_mana(player, COLOR_GREEN, 2);
    }

    if (g_spell_fizzled != 1)
    {
      PLAYER_CARD_INSTANCE(player, card).targets[0].player = player;
      PLAYER_CARD_INSTANCE(player, card).targets[0].card = card;
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;

      if (g_other_player == player &&
          (((PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                                  PLAYER_CARD_INSTANCE(player, card).targets[0].card)
                 .regen_status &
             (1 << (get_hacked_color(player,
                                     card,
                                     PLAYER_CARD_INSTANCE(player, card).info_slot == 0 ? COLOR_GREEN : COLOR_BLACK) -
                    1U))) != 0) ||
           PLAYER_CARD_INSTANCE(player, card).targets[0].player == g_active_player))
      {
        g_ai_modifier -= 0x60;
      }
    }
    return 0;
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    if (PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                             PLAYER_CARD_INSTANCE(player, card).parent_card)
            .internal_card_id != -1)
    {
      damage_player(g_card_on_stack_controller, 2, g_card_on_stack_controller, g_card_on_stack);
      s.legacy_card = create_legacy_effect(g_card_on_stack_controller,
                                           g_card_on_stack,
                                           g_duel_generated_internal_card_id_03,
                                           g_card_on_stack_controller,
                                           g_card_on_stack);
      if (s.legacy_card != -1)
      {
        PLAYER_CARD_INSTANCE(player, s.legacy_card).info_slot =
            1 << (unsigned char)(get_hacked_color(
                                     player,
                                     card,
                                     PLAYER_CARD_INSTANCE(player, card).info_slot == 0 ? COLOR_GREEN : COLOR_BLACK) -
                                 1);
        if (PLAYER_CARD_INSTANCE(player, card).info_slot != 0)
        {
          PLAYER_CARD_INSTANCE(player, s.legacy_card).eot_toughness = 2;
        }
        else
        {
          PLAYER_CARD_INSTANCE(player, s.legacy_card).eot_toughness = 1;
        }
        PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                             PLAYER_CARD_INSTANCE(player, card).parent_card)
            .regen_status = KEYWORD_RECALC_ABILITIES;
      }
    }
    PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                         PLAYER_CARD_INSTANCE(player, card).parent_card)
        .number_of_targets = 0;
  }

  return 0;
}
// FUNCTION: MAGIC 0x0042d8c1
// FUNCTION: SHANDALAR 0x0043c5b6
int card_wall_of_light(int player, int card, event_t event)
{
  struct
  {
    int opponent;
    int current_card;
    int old_event_result;
    int ai_bonus;
  } s;

  if (event == EVENT_SHOULD_AI_PLAY && g_current_phase == PHASE_CLEANUP &&
      card == g_affected_card && player == g_affected_card_controller)
  {
    s.current_card = 0;
    s.opponent = 1 - player;
    s.ai_bonus = 0;
    for (; s.current_card < g_active_cards_count[s.opponent]; ++s.current_card)
    {
      if ((PLAYER_CARD_INSTANCE(s.opponent, s.current_card).color & (1 << (unsigned char)get_sleighted_color(player, card, COLOR_BLACK))) != 0 &&
          is_in_play(s.opponent, s.current_card))
      {
        s.ai_bonus += 0x18;
      }
    }
    if (s.ai_bonus != 0)
    {
      g_ai_modifier += s.ai_bonus + 0x18;
    }
  }

  if (event == EVENT_ABILITIES && card == g_affected_card && player == g_affected_card_controller)
  {
    g_event_result |= KEYWORD_PROT_BLACK << (get_sleighted_color(player, card, COLOR_BLACK) - 1);
    s.old_event_result = g_event_result;
    destroy_attached_auras_of_color(player, card, COLOR_BLACK);
    g_event_result = s.old_event_result;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0042da37
// FUNCTION: SHANDALAR 0x0043c72a
int card_mountain_yeti(int player, int card, event_t event)
{
  struct
  {
    int opponent;
    int current_card;
    int old_event_result;
    int ai_bonus;
  } s;

  if (event == EVENT_SHOULD_AI_PLAY && g_current_phase == PHASE_CLEANUP &&
      card == g_affected_card && player == g_affected_card_controller)
  {
    s.current_card = 0;
    s.opponent = 1 - player;
    s.ai_bonus = 0;
    for (; s.current_card < g_active_cards_count[s.opponent]; ++s.current_card)
    {
      if ((PLAYER_CARD_INSTANCE(s.opponent, s.current_card).color & (1 << (unsigned char)get_sleighted_color(player, card, COLOR_WHITE))) != 0 &&
          is_in_play(s.opponent, s.current_card))
      {
        s.ai_bonus += 0x18;
      }
    }
    if (s.ai_bonus != 0)
    {
      g_ai_modifier += s.ai_bonus + 0x18;
    }
  }

  if (event == EVENT_ABILITIES && card == g_affected_card && player == g_affected_card_controller)
  {
    g_event_result |= KEYWORD_PROT_BLACK << (get_sleighted_color(player, card, COLOR_WHITE) - 1);
    s.old_event_result = g_event_result;
    destroy_attached_auras_of_color(player, card, COLOR_WHITE);
    g_event_result = s.old_event_result;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0042dbad
// FUNCTION: SHANDALAR 0x0043c89c
int card_angelic_voices(int player, int card, event_t event)
{
  int found_other_color_creature;
  int color_mask;
  int current_card;

  found_other_color_creature = 0;
  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if ((event == EVENT_POWER || event == EVENT_TOUGHNESS) && is_in_play(player, card) &&
      player == g_affected_card_controller)
  {
    color_mask = ~((1 << (unsigned char)get_sleighted_color(player, card, COLOR_WHITE)) | 1);
    for (current_card = 0; current_card < g_active_cards_count[player]; ++current_card)
    {
      if (is_in_play(player, current_card) &&
          (global_cards_data[PLAYER_CARD_INSTANCE(player, current_card).internal_card_id].type & (TYPE_CREATURE | TYPE_ARTIFACT)) == TYPE_CREATURE &&
          ((color_mask & PLAYER_CARD_INSTANCE(player, current_card).color) != 0))
      {
        found_other_color_creature = 1;
        break;
      }
    }
    if (!found_other_color_creature)
    {
      ++g_event_result;
    }
  }
  return 0;
}

// FUNCTION: MAGIC 0x0042dd13
// FUNCTION: SHANDALAR 0x0043ca02
int card_blood_moon(int player, int card, event_t event)
{
  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if (event == EVENT_CAST_SPELL &&
      g_affected_card == card &&
      g_affected_card_controller == player &&
      count_permanents_by_internal_card_id(player,
                                           PLAYER_CARD_INSTANCE(player, card).internal_card_id,
                                           -1) == 0)
  {
    g_ai_modifier +=
        (g_creature_power_by_color[g_other_player][COLOR_RED] -
         g_creature_power_by_color[g_active_player][COLOR_RED]) *
        0xc;
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    PLAYER_CARD_INSTANCE(player, card).info_slot = COLOR_RED;
  }
  else if (event == EVENT_CHANGE_TYPE && (g_land_can_be_played & 0x20000) == 0 &&
           is_in_play(player, card) &&
           is_in_play(g_affected_card_controller, g_affected_card))
  {
    if ((global_cards_data[g_event_result].type & TYPE_LAND) != 0 &&
        global_cards_data[g_event_result].subtype != 13)
    {
      g_event_result =
          get_hacked_color(player, card, PLAYER_CARD_INSTANCE(player, card).info_slot) - 1;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0042e107
// FUNCTION: SHANDALAR 0x0043cdfb
int card_greater_realm_of_preservation(int player, int card, event_t event)
{
  card_instance_t *instance;
  target_t target;
  unsigned int target_color;
  int target_player;
  int target_card;

  instance = &PLAYER_CARD_INSTANCE(player, card);
  target_color = (1 << (unsigned char)get_sleighted_color(player, card, COLOR_RED)) |
                 (1 << (unsigned char)get_sleighted_color(player, card, COLOR_BLACK));

  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if (event == EVENT_CAST_SPELL && g_affected_card == card && g_affected_card_controller == player &&
      count_permanents_by_internal_card_id(player, instance->internal_card_id, player) == 0)
  {
    g_ai_modifier += (g_basiclandtypes_controlled[g_active_player][target_color] +
                      g_creature_power_by_color[g_active_player][target_color] / 2) *
                     0x18;
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    if ((g_land_can_be_played & 4) == 0 ||
        has_mana_w_global_cost_mod(player, card, COLOR_WHITE, 1) == 0 ||
        has_mana_w_global_cost_mod(player, card, COLOR_ANY, 2) == 0 ||
        real_target_available((int *)0, TARGET_SCAN_DIRECT, player, 2, 2, TARGET_ZONE_IN_PLAY,
                              TYPE_NONE, TYPE_NONE, 0, 0, target_color, COLOR_TEST_0,
                              g_damage_card_internal_card_id, -1, 0xffffffff, 0xffffffff,
                              TARGET_SPECIAL_DAMAGE_PLAYER, 0, 0) == 0)
    {
      return 0;
    }
    return 99;
  }

  if (event == EVENT_ACTIVATE && (instance->state & STATE_INVISIBLE) == 0)
  {
    g_mana_charge[COLOR_COLORLESS] = 1;
    charge_mana_w_global_cost_mod(player, card, COLOR_WHITE, 1);
    if (g_spell_fizzled != 1)
    {
      if (g_duel_ai_mode_state != 1)
      {
        load_text("promptsX2.txt", "GREATER_REALM_OF_PRESERVATION");
      }

      if (C_real_select_target(player, 2, 2, TARGET_ZONE_IN_PLAY, TYPE_NONE, TYPE_NONE,
                               0, 0, target_color, COLOR_TEST_0,
                               g_damage_card_internal_card_id, -1, -1, -1,
                               TARGET_SPECIAL_DAMAGE_PLAYER, 0, 0,
                               g_text_lines[0], 1, &target) == 0)
      {
        g_spell_fizzled = 1;
      }
      else
      {
        SET_TARGET(instance->targets[0], target);
        instance->number_of_targets = 1;
      }
    }
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    if (C_real_validate_target(instance->targets[0].player, instance->targets[0].card,
                               (char *)0, player, 2, 2, TARGET_ZONE_IN_PLAY,
                               TYPE_NONE, TYPE_NONE, 0, 0, target_color, COLOR_TEST_0,
                               g_damage_card_internal_card_id, -1, -1, -1,
                               TARGET_SPECIAL_DAMAGE_PLAYER, 0, 0) == 0)
    {
      g_spell_fizzled = 1;
    }
    else
    {
      target_player = instance->targets[0].player;
      target_card = instance->targets[0].card;
      if (PLAYER_CARD_INSTANCE(target_player, target_card).info_slot != 0)
      {
        PLAYER_CARD_INSTANCE(target_player, target_card).info_slot = 0;
      }
    }
    PLAYER_CARD_INSTANCE(instance->parent_controller, instance->parent_card).number_of_targets = 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0042e5d6
// FUNCTION: SHANDALAR 0x0043d2cc
int card_lifeblood(int player, int card, event_t event)
{
  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if (event == EVENT_TAP_CARD && g_affected_card_controller != player)
  {
    if (card_has_basic_land_type(g_affected_card_controller, g_affected_card,
                                 get_hacked_color(player, card, COLOR_RED)) != 0)
    {
      gain_life(player, 1, player, card);
      return 0;
    }
  }

  if ((event == EVENT_CAST_SPELL || event == EVENT_SHOULD_AI_PLAY) &&
      g_affected_card == card && g_affected_card_controller == player)
  {
    g_ai_modifier += (g_basiclandtypes_controlled[g_active_player][get_hacked_color(player, card, COLOR_RED)] * 3 + 3) * 8;
  }
  return 0;
}

// FUNCTION: MAGIC 0x0042e6c5
// FUNCTION: SHANDALAR 0x0043d3be
int card_hidden_path(int player, int card, event_t event)
{
  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if (event == EVENT_ABILITIES && is_in_play(player, card) &&
      is_in_play(g_affected_card_controller, g_affected_card) &&
      (global_cards_data[PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).internal_card_id].type & TYPE_CREATURE))
  {
    if (((1 << (unsigned char)get_sleighted_color(player, card, COLOR_GREEN)) &
         PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).color) != 0)
    {
      g_event_result |= 1 << (get_hacked_color(player, card, COLOR_GREEN) - 1);
    }
  }
  return 0;
}

// FUNCTION: MAGIC 0x0042e7ea
// FUNCTION: SHANDALAR 0x0043d4e3
int card_moat(int player, int card, event_t event)
{
  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if (event == EVENT_ATTACK_LEGALITY &&
      (PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).regen_status & KEYWORD_FLYING) == 0)
  {
    ++g_event_result;
  }
  return 0;
}

// FUNCTION: MAGIC 0x0042e85a
// FUNCTION: SHANDALAR 0x0043d553
int card_spiritual_sanctuary(int player, int card, event_t event)
{
  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    if (g_current_phase == PHASE_UPKEEP &&
        g_current_player == g_event_player &&
        (PLAYER_CARD_INSTANCE(player, card).info_slot & 1) == 0 &&
        g_basiclandtypes_controlled[g_current_player][get_hacked_color(player, card, COLOR_WHITE)] != 0)
    {
      PLAYER_CARD_INSTANCE(player, card).upkeep_flags |=
          UPKEEP_UPKEEP_TRIGGER | UPKEEP_UPKEEP_NODIALOG;
      g_activation_event_flags |= 3;
      return 1;
    }
    return 0;
  }

  if (event == EVENT_UPKEEP_PHASE &&
      g_affected_card == card &&
      g_affected_card_controller == player)
  {
    PLAYER_CARD_INSTANCE(player, card).info_slot |= 1;
    g_upkeep_payment_completed = 1;
    g_event_result |= 1;
  }

  if (event == EVENT_UPKEEP_COSTS_UNPAID)
  {
    gain_life(g_current_player, 1);
  }

  if (event == EVENT_CLEANUP)
  {
    PLAYER_CARD_INSTANCE(player, card).info_slot &= ~1;
  }

  if (event == EVENT_SHOULD_AI_PLAY &&
      g_current_phase == PHASE_CLEANUP &&
      is_in_play(player, card) &&
      player == g_other_player &&
      g_basiclandtypes_controlled[player][get_hacked_color(player, card, COLOR_WHITE)] != 0)
  {
    g_ai_modifier += 0x18;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0042eaa2
// FUNCTION: SHANDALAR 0x0043d798
int card_boomerang(int player, int card, event_t event)
{
  target_t target;

  if (event == EVENT_CAN_CAST)
  {
    load_recorded_action_target(0);
    return real_target_available((int *)0, TARGET_SCAN_DIRECT, player, 2, 2, TARGET_ZONE_IN_PLAY,
                                 TARGET_TYPE_TOKEN | TYPE_ARTIFACT | TYPE_ENCHANTMENT | TYPE_CREATURE | TYPE_LAND,
                                 TYPE_NONE, 0, get_protections_from(player, card),
                                 COLOR_TEST_0, COLOR_TEST_0, -1, -1, -1, -1, 0, 0, 0);
  }

  if (event == EVENT_CAST_SPELL && card == g_affected_card && player == g_affected_card_controller)
  {
    g_ai_modifier -= 0x30;
    load_text("promptsX2.txt", "BOOMERANG");
    if (C_real_select_target(player, 2, 1 - player, TARGET_ZONE_IN_PLAY,
                             TARGET_TYPE_TOKEN | TYPE_ARTIFACT | TYPE_ENCHANTMENT | TYPE_CREATURE | TYPE_LAND,
                             TYPE_NONE, 0,
                             get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0, -1, -1, -1, -1,
                             0, 0, 0, g_text_lines[0], 1, &target) != 0)
    {
      PLAYER_CARD_INSTANCE(player, card).targets[0].card = target.card;
      PLAYER_CARD_INSTANCE(player, card).targets[0].player = target.player;
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
    }
    else
    {
      g_spell_fizzled = 1;
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    SET_TARGET(target, PLAYER_CARD_INSTANCE(player, card).targets[0]);
    if (C_real_validate_target(target.player, target.card, (char *)0, player, 2, 2, TARGET_ZONE_IN_PLAY,
                               TARGET_TYPE_TOKEN | TYPE_ARTIFACT | TYPE_ENCHANTMENT | TYPE_CREATURE | TYPE_LAND,
                               TYPE_NONE, 0, get_protections_from(player, card),
                               COLOR_TEST_0, COLOR_TEST_0, -1, -1, -1, -1, 0, 0, 0) != 0)
    {
      hurkyls_recall_bounce_artifact(target.player, target.card);
    }
    else
    {
      g_spell_fizzled = 1;
    }
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x0042ed6e
// FUNCTION: SHANDALAR 0x0043da66
int card_divine_offering(int player, int card, event_t event)
{
  target_t target;

  if (event == EVENT_CAN_CAST)
  {
    load_recorded_action_target(0);
    return real_target_available((int *)0, TARGET_SCAN_DIRECT, player, 2, 2, TARGET_ZONE_IN_PLAY,
                                 TYPE_ARTIFACT, TYPE_NONE, 0, get_protections_from(player, card),
                                 COLOR_TEST_0, COLOR_TEST_0, -1, -1, -1, -1, 0, 0, 0);
  }

  if (event == EVENT_CAST_SPELL && g_affected_card == card && g_affected_card_controller == player)
  {
    load_text("promptsX2.txt", "DIVINE_OFFERING");
    if (C_real_select_target(player, 2, 2, TARGET_ZONE_IN_PLAY, TYPE_ARTIFACT, TYPE_NONE, 0,
                             get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0, -1, -1, -1, -1,
                              0, 0, 0, g_text_lines[0], 1, &target) != 0)
    {
      SET_TARGET(PLAYER_CARD_INSTANCE(player, card).targets[0], target);
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
      if (g_other_player == player && global_cards_data[PLAYER_CARD_INSTANCE(target.player, target.card).internal_card_id].cc[1] == 0)
      {
        g_ai_modifier -= 0x30;
      }
    }
    else
    {
      g_spell_fizzled = 1;
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    SET_TARGET(target, PLAYER_CARD_INSTANCE(player, card).targets[0]);
    if (C_real_validate_target(target.player, target.card, (char *)0, player, 2, 2, TARGET_ZONE_IN_PLAY,
                               TYPE_ARTIFACT, TYPE_NONE, 0, get_protections_from(player, card),
                               COLOR_TEST_0, COLOR_TEST_0, -1, -1, -1, -1, 0, 0, 0) != 0)
    {
      gain_life(player,
                (int)global_cards_data[PLAYER_CARD_INSTANCE(target.player, target.card).internal_card_id].cc[0] +
                    (int)global_cards_data[PLAYER_CARD_INSTANCE(target.player, target.card).internal_card_id].cc[1]);
      kill_card(target.player, target.card, KILL_DESTROY);
    }
    else
    {
      g_spell_fizzled = 1;
    }
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x0042f104
// FUNCTION: SHANDALAR 0x0043ddff
int card_great_defender(int player, int card, event_t event)
{
  target_t target;
  int mana_value;
  int legacy_card;

  if (event == EVENT_CAN_CAST)
  {
    load_recorded_action_target(0);
    return real_target_available((int *)0, TARGET_SCAN_DIRECT, player, 2, 2, TARGET_ZONE_IN_PLAY,
                                 TYPE_CREATURE, TYPE_NONE, 0, get_protections_from(player, card),
                                 COLOR_TEST_0, COLOR_TEST_0, -1, -1, -1, -1, 0, 0, 0);
  }

  if (event == EVENT_CAST_SPELL && g_affected_card == card && g_affected_card_controller == player)
  {
    load_text("promptsX2.txt", "GREAT_DEFENDER");
    if (!C_real_select_target(player, 2, 2, TARGET_ZONE_IN_PLAY, TYPE_CREATURE, TYPE_NONE, 0,
                              get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0, -1, -1,
                              -1, -1, 0, 0, 0, g_text_lines[0], 1, &target) == 0)
    {
      SET_TARGET(PLAYER_CARD_INSTANCE(player, card).targets[0], target);
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
    }
    else
    {
      g_spell_fizzled = 1;
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    SET_TARGET(target, PLAYER_CARD_INSTANCE(player, card).targets[0]);
    if (C_real_validate_target(target.player, target.card, (char *)0, player, 2, 2,
                                TARGET_ZONE_IN_PLAY, TYPE_CREATURE, TYPE_NONE, 0,
                                get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0, -1, -1,
                                -1, -1, 0, 0, 0))
    {
      legacy_card = create_legacy_effect(player, card, LEGACY_EFFECT_PUMP, target.player, target.card);
      if (legacy_card != -1)
      {
        if (global_cards_data[PLAYER_CARD_INSTANCE(target.player, target.card).internal_card_id].cc[1] == -1)
        {
          mana_value = 0;
        }
        else
        {
          mana_value = (int)global_cards_data[PLAYER_CARD_INSTANCE(target.player, target.card).internal_card_id].cc[1];
        }
        PLAYER_CARD_INSTANCE(player, legacy_card).counter_toughness =
            (short)global_cards_data[PLAYER_CARD_INSTANCE(target.player, target.card).internal_card_id].cc[0] +
            mana_value;
      }
    }
    else
    {
      g_spell_fizzled = 1;
    }
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x0042f4c9
// FUNCTION: SHANDALAR 0x0043e1c6
int card_hell_swarm(int player, int card, event_t event)
{
  int current_player;
  int current_card;
  int legacy_card;

  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    for (current_player = 0; current_player < 2; ++current_player)
    {
      for (current_card = 0; current_card < g_active_cards_count[current_player]; ++current_card)
      {
        if (is_in_play(current_player, current_card) &&
            (global_cards_data[PLAYER_CARD_INSTANCE(current_player, current_card).internal_card_id].type & TYPE_CREATURE))
        {
          legacy_card = create_legacy_effect(player, card, g_duel_generated_internal_card_id_0c, current_player, current_card);
          if (legacy_card != -1)
          {
            PLAYER_CARD_INSTANCE(player, legacy_card).info_slot = -1;
            PLAYER_CARD_INSTANCE(player, legacy_card).eot_toughness = 0;
          }
        }
      }
    }
    kill_card(player, card, KILL_BURY);
  }
  return 0;
}

// FUNCTION: MAGIC 0x0042f63d
// FUNCTION: SHANDALAR 0x0043e33a
int card_holy_light(int player, int card, event_t event)
{
  int current_player;
  int current_card;
  int legacy_card;

  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    for (current_player = 0; current_player < 2; ++current_player)
    {
      for (current_card = 0; current_card < g_active_cards_count[current_player]; ++current_card)
      {
        if (is_in_play(current_player, current_card) &&
            (global_cards_data[PLAYER_CARD_INSTANCE(current_player, current_card).internal_card_id].type & TYPE_CREATURE))
        {
          if (((1 << (unsigned char)get_sleighted_color(player, card, COLOR_WHITE)) &
               PLAYER_CARD_INSTANCE(current_player, current_card).color) == 0)
          {
            legacy_card = create_legacy_effect(player, card, g_duel_generated_internal_card_id_0c, current_player, current_card);
            if (legacy_card != -1)
            {
              PLAYER_CARD_INSTANCE(player, legacy_card).info_slot = -1;
              PLAYER_CARD_INSTANCE(player, legacy_card).eot_toughness = 0xffffffff;
            }
          }
        }
      }
    }
    kill_card(player, card, KILL_BURY);
  }
  return 0;
}

// FUNCTION: MAGIC 0x0042f803
// FUNCTION: SHANDALAR 0x0043e500
int card_riptide(int player, int card, event_t event)
{
  int current_player;
  int color_mask;
  int current_card;

  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    color_mask = 1 << (unsigned char)get_sleighted_color(player, card, COLOR_BLUE);
    for (current_player = 0; current_player < 2; ++current_player)
    {
      for (current_card = 0; current_card < g_active_cards_count[current_player]; ++current_card)
      {
        if (is_in_play(current_player, current_card) &&
            (global_cards_data[PLAYER_CARD_INSTANCE(current_player, current_card).internal_card_id].type & TYPE_CREATURE) &&
            (color_mask & PLAYER_CARD_INSTANCE(current_player, current_card).color))
        {
          tap_card_and_dispatch_event(current_player, current_card);
        }
      }
    }
    kill_card(player, card, KILL_BURY);
  }
  return 0;
}

// FUNCTION: MAGIC 0x0042f94e
// FUNCTION: SHANDALAR 0x0043e64b
int card_shield_wall(int player, int card, event_t event)
{
  int current_card;
  int legacy_card;

  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    for (current_card = 0; current_card < g_active_cards_count[player]; ++current_card)
    {
      if (is_in_play(player, current_card) &&
          (global_cards_data[PLAYER_CARD_INSTANCE(player, current_card).internal_card_id].type & TYPE_CREATURE))
      {
        legacy_card = create_legacy_effect(player, card, LEGACY_EFFECT_PUMP, player, current_card);
        if (legacy_card != -1)
        {
          PLAYER_CARD_INSTANCE(player, legacy_card).counter_toughness = 2;
        }
      }
    }
    kill_card(player, card, KILL_BURY);
  }
  return 0;
}

// FUNCTION: MAGIC 0x0042fa71
// FUNCTION: SHANDALAR 0x0043e76e
int card_storm_seeker(int player, int card, event_t event)
{
  target_t target;
  int target_player;
  int current_card;
  int damage;

  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if (event == EVENT_CAST_SPELL && g_affected_card == card && g_affected_card_controller == player)
  {
    if (g_duel_ai_mode_state != 1)
    {
      load_text("promptsX2.txt", "STORM_SEEKER");
    }
    if (C_real_select_target(player, 2, 1 - player, TARGET_ZONE_PLAYERS, TYPE_NONE, TYPE_NONE, 0, 0,
                             COLOR_TEST_0, COLOR_TEST_0, -1, -1, -1, -1, 0, 0, 0, g_text_lines[0],
                             1, &target) == 0)
    {
      g_spell_fizzled = 1;
    }
    else
    {
      SET_TARGET(PLAYER_CARD_INSTANCE(player, card).targets[0], target);
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    target_player = PLAYER_CARD_INSTANCE(player, card).targets[0].player;
    damage = 0;
    for (current_card = 0; current_card < g_active_cards_count[target_player]; ++current_card)
    {
      if (is_card_pending_resolution(target_player, current_card) != 0)
      {
        ++damage;
      }
    }
    damage_player(target_player, damage, player, card);
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x0042ff6c
// FUNCTION: SHANDALAR 0x0043ec68
int card_force_spike(int player, int card, event_t event)
{
  int target_player;
  int target_card;
  int mana_paid;

  if (event == EVENT_CAN_CAST)
  {
    load_recorded_action_target(0);
    if (g_current_spell_player == -1)
    {
      return 0;
    }
    if (C_real_validate_target(g_current_spell_player, g_current_spell_card, (char *)0, player, 2, 2,
                               0, TYPE_NONE, TYPE_NONE, 0, 0, COLOR_TEST_0, COLOR_TEST_0, -1, -1, -1, -1,
                               TARGET_SPECIAL_SPELL_ON_STACK, 0, 0) == 0)
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
    if (g_other_player == g_current_spell_player)
    {
      g_ai_modifier -= 0x24;
    }
  }

  if (event == EVENT_CAN_COUNTER && g_special_mana_pool[player][COLOR_BLUE] > 0)
  {
    g_ai_modifier += 0x18;
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    target_player = PLAYER_CARD_INSTANCE(player, card).targets[0].player;
    target_card = PLAYER_CARD_INSTANCE(player, card).targets[0].card;
    if (C_real_validate_target(target_player, target_card, (char *)0, player, 2, 2,
                               0, TYPE_NONE, TYPE_NONE, 0, 0, COLOR_TEST_0, COLOR_TEST_0, -1, -1, -1, -1,
                               TARGET_SPECIAL_SPELL_ON_STACK, 0, 0) == 0)
    {
      g_spell_fizzled = 1;
    }
    else
    {
      if (has_mana(target_player, COLOR_ANY, 1) == 0)
      {
        mana_paid = 0;
      }
      else if ((target_player == g_active_player || (g_duel_network_flags & 2) != 0) && g_duel_ai_mode_state != 1)
      {
        push_card_onto_stack(player, card, EVENT_RESOLVE_TRIGGER, 0, 0);
        mana_paid = charge_mana(target_player, COLOR_COLORLESS, 1);
        obliterate_top_card_of_stack();
        g_spell_fizzled = 0;
      }
      else
      {
        mana_paid = charge_mana(target_player, COLOR_COLORLESS, 1);
      }

      if (mana_paid == 0)
      {
        kill_card(target_player, target_card, KILL_BURY);
      }
    }
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004302cd
// FUNCTION: SHANDALAR 0x0043efc8
int card_remove_soul(int player, int card, event_t event)
{
  int target_player;
  int target_card;

  if (event == EVENT_CAN_CAST)
  {
    load_recorded_action_target(0);
    if (g_current_spell_player == -1)
    {
      return 0;
    }
    if ((global_cards_data[PLAYER_CARD_INSTANCE(g_current_spell_player, g_current_spell_card).internal_card_id].type &
         (TYPE_CREATURE | TYPE_ARTIFACT)) == TYPE_CREATURE &&
        C_real_validate_target(g_current_spell_player, g_current_spell_card, (char *)0, player, 2, 2,
                               0, TYPE_CREATURE, TYPE_NONE, 0, 0, COLOR_TEST_0, COLOR_TEST_0, -1, -1,
                               -1, -1, TARGET_SPECIAL_SPELL_ON_STACK, 0, 0) != 0)
    {
      return 99;
    }
    return 0;
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
    g_ai_modifier -= 0x24;
  }

  if (event == EVENT_CAN_COUNTER && g_special_mana_pool[player][COLOR_BLUE] >= 2)
  {
    g_ai_modifier += 0x18;
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    if (C_real_validate_target(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                               PLAYER_CARD_INSTANCE(player, card).targets[0].card, (char *)0, player, 2, 2,
                               0, TYPE_CREATURE, TYPE_NONE, 0, 0, COLOR_TEST_0, COLOR_TEST_0, -1, -1,
                               -1, -1, TARGET_SPECIAL_SPELL_ON_STACK, 0, 0) == 0)
    {
      g_spell_fizzled = 1;
    }
    else
    {
      target_player = PLAYER_CARD_INSTANCE(player, card).targets[0].player;
      target_card = PLAYER_CARD_INSTANCE(player, card).targets[0].card;
      if ((PLAYER_CARD_INSTANCE(target_player, target_card).state & STATE_INVISIBLE) != 0)
      {
        kill_card(target_player, target_card, KILL_BURY);
      }
    }
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x0043064b
// FUNCTION: SHANDALAR 0x0043f348
int card_reset(int player, int card, event_t event)
{
  int current_card;

  if (event == EVENT_CAN_CAST)
  {
    if (g_current_player != player && g_current_phase >= 10 && g_current_phase <= 0x22)
    {
      return 1;
    }
    return 0;
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    for (current_card = 0; current_card < g_active_cards_count[player]; ++current_card)
    {
      if (is_in_play(player, current_card) &&
          (global_cards_data[PLAYER_CARD_INSTANCE(player, current_card).internal_card_id].type & TYPE_LAND))
      {
        PLAYER_CARD_INSTANCE(player, current_card).state &= ~STATE_TAPPED;
      }
    }
    kill_card(player, card, KILL_BURY);
  }
  return 0;
}

// FUNCTION: MAGIC 0x00430770
// FUNCTION: SHANDALAR 0x0043f46e
int card_acid_rain(int player, int card, event_t event)
{
  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    PLAYER_CARD_INSTANCE(player, card).info_slot = get_hacked_color(player, card, COLOR_GREEN);
    dispatch_function_to_all_cards_in_play(player, card, helper_destroy_basiclandtype, -1);
    kill_card(player, card, KILL_BURY);
  }
  return 0;
}

// FUNCTION: MAGIC 0x00430809
// FUNCTION: SHANDALAR 0x0043f507
int card_amnesia(int player, int card, event_t event)
{
  target_t target;
  int target_player;
  int current_card;
  int internal_card_id;
  int hand_card_count;
  int discard_count;
  int hand_cards[30];
  int discard_cards[30];

  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if (event == EVENT_CAST_SPELL && g_affected_card == card && g_affected_card_controller == player)
  {
    if (g_duel_ai_mode_state != 1)
    {
      load_text("promptsX2.txt", "AMNESIA");
    }
    if (C_real_select_target(player, 2, 1 - player, TARGET_ZONE_PLAYERS, TYPE_NONE, TYPE_NONE, 0, 0,
                             COLOR_TEST_0, COLOR_TEST_0, -1, -1, -1, -1, 0, 0, 0, g_text_lines[0],
                             1, &target) == 0)
    {
      g_spell_fizzled = 1;
    }
    else
    {
      SET_TARGET(PLAYER_CARD_INSTANCE(player, card).targets[0], target);
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    target_player = PLAYER_CARD_INSTANCE(player, card).targets[0].player;
    hand_card_count = 0;
    discard_count = 0;
    for (current_card = 0; current_card < g_active_cards_count[target_player]; ++current_card)
    {
      internal_card_id = PLAYER_CARD_INSTANCE(target_player, current_card).internal_card_id;
      if (internal_card_id != -1 &&
          (PLAYER_CARD_INSTANCE(target_player, current_card).state & STATE_IN_PLAY) == 0)
      {
        hand_cards[hand_card_count] = internal_card_id;
        ++hand_card_count;
        if ((global_cards_data[internal_card_id].type & TYPE_LAND) == 0)
        {
          discard_cards[discard_count] = current_card;
          ++discard_count;
        }
      }
    }
    if ((g_active_player == player || (g_duel_network_flags & 2) != 0) &&
        g_duel_ai_mode_state != 1)
    {
      if (g_active_player == player)
      {
        load_text("promptsX2.txt", "AMNESIA");
      }
#ifdef SHANDALAR
      SelectAdventureListCardIndex(player,
                                   hand_cards,
                                   hand_card_count,
                                   g_text_lines[1],
                                   0,
                                   (int *)gs_done_008b40e0);
#else
      show_deck(player, hand_cards, hand_card_count, g_text_lines[1], 0, gs_done_008b40e0);
#endif
    }
    for (current_card = 0; current_card < discard_count; ++current_card)
    {
      discard_card_from_hand(target_player, discard_cards[current_card]);
    }
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x00430b8e
// FUNCTION: SHANDALAR 0x0043f890
int card_cleanse(int player, int card, event_t event)
{
  int current_player;
  int current_card;

  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    for (current_player = 0; current_player < 2; ++current_player)
    {
      for (current_card = 0; current_card < g_active_cards_count[current_player]; ++current_card)
      {
        if (is_in_play(current_player, current_card) &&
            (global_cards_data[PLAYER_CARD_INSTANCE(current_player, current_card).internal_card_id].type & TYPE_CREATURE))
        {
          if (((1 << (unsigned char)get_sleighted_color(player, card, COLOR_BLACK)) &
               PLAYER_CARD_INSTANCE(current_player, current_card).color) != 0)
          {
            kill_card(current_player, current_card, KILL_DESTROY);
          }
        }
      }
    }
    kill_card(player, card, KILL_BURY);
  }
  return 0;
}

// FUNCTION: MAGIC 0x00430cd7
// FUNCTION: SHANDALAR 0x0043f9d9
int card_eternal_flame(int player, int card, event_t event)
{
  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }
  else if (event == EVENT_CAST_SPELL && g_affected_card == card && g_affected_card_controller == player)
  {
    PLAYER_CARD_INSTANCE(player, card).targets[0].player = 1 - player;
    PLAYER_CARD_INSTANCE(player, card).targets[0].card = -1;
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
    return 0;
  }
  else
  {
    if (event == EVENT_RESOLVE_SPELL)
    {
      damage_player(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                    g_basiclandtypes_controlled[player][get_hacked_color(player, card, COLOR_RED)], player, card);
      damage_player(player, (g_basiclandtypes_controlled[player][get_hacked_color(player, card, COLOR_RED)] + 1) / 2,
                    player, card);
      kill_card(player, card, KILL_BURY);
    }
  }
  return 0;
}

// FUNCTION: MAGIC 0x00430e79
// FUNCTION: SHANDALAR 0x0043fb7a
int card_inquisition(int player, int card, event_t event)
{
  target_t target;
  int target_player;
  int damage;
  int current_card;
  int internal_card_id;
  int card_count;
  int hand_cards[30];
  int color_mask;

  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if (event == EVENT_CAST_SPELL && g_affected_card == card && g_affected_card_controller == player)
  {
    if (g_duel_ai_mode_state != 1)
    {
      load_text("promptsX2.txt", "INQUISITION");
    }
    if (C_real_select_target(player, 2, 1 - player, TARGET_ZONE_PLAYERS, TYPE_NONE, TYPE_NONE, 0, 0,
                             COLOR_TEST_0, COLOR_TEST_0, -1, -1, -1, -1, 0, 0, 0, g_text_lines[0],
                             1, &target) == 0)
    {
      g_spell_fizzled = 1;
    }
    else
    {
      SET_TARGET(PLAYER_CARD_INSTANCE(player, card).targets[0], target);
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    target_player = PLAYER_CARD_INSTANCE(player, card).targets[0].player;
    color_mask = 1 << (unsigned char)get_sleighted_color(player, card, COLOR_WHITE);
    card_count = 0;
    damage = 0;
    for (current_card = 0; current_card < g_active_cards_count[target_player]; ++current_card)
    {
      internal_card_id = PLAYER_CARD_INSTANCE(target_player, current_card).internal_card_id;
      if (is_card_pending_resolution(target_player, current_card) != 0)
      {
        hand_cards[card_count] = internal_card_id;
        ++card_count;
        if ((PLAYER_CARD_INSTANCE(target_player, current_card).color & color_mask) != 0)
        {
          ++damage;
        }
      }
    }
    if ((player == g_active_player || (g_duel_network_flags & 2) != 0) &&
        g_duel_ai_mode_state != 1)
    {
      if (player == g_active_player)
      {
        load_text("promptsX2.txt", "INQUISITION");
      }
#ifdef SHANDALAR
      SelectAdventureListCardIndex(player,
                                   hand_cards,
                                   card_count,
                                   g_text_lines[1],
                                   0,
                                   (int *)gs_done_008b40e0);
#else
      show_deck(player, hand_cards, card_count, g_text_lines[1], 0, gs_done_008b40e0);
#endif
    }
    damage_player(target_player, damage, player, card);
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004311fb
// FUNCTION: SHANDALAR 0x0043fefe
int card_jovial_evil(int player, int card, event_t event)
{
  target_t target;
  int target_player;
  int current_card;
  int damage;
  int color_mask;

  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if (event == EVENT_CAST_SPELL && g_affected_card == card && g_affected_card_controller == player)
  {
    if (g_duel_ai_mode_state != 1)
    {
      load_text("promptsX2.txt", "JOVIAL_EVIL");
    }
    if (C_real_select_target(player, 2, 1 - player, TARGET_ZONE_PLAYERS, TYPE_NONE, TYPE_NONE,
                             0, 0, COLOR_TEST_0, COLOR_TEST_0, -1, -1, -1, -1, 0, 0, 0,
                             g_text_lines[0], 1, &target))
    {
      SET_TARGET(PLAYER_CARD_INSTANCE(player, card).targets[0], target);
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
    }
    else
    {
      g_spell_fizzled = 1;
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    target_player = PLAYER_CARD_INSTANCE(player, card).targets[0].player;
    color_mask = 1 << (unsigned char)get_sleighted_color(player, card, COLOR_WHITE);
    damage = 0;
    for (current_card = 0; current_card < g_active_cards_count[target_player]; ++current_card)
    {
      if (is_in_play(target_player, current_card) &&
          (global_cards_data[PLAYER_CARD_INSTANCE(target_player, current_card).internal_card_id].type & TYPE_CREATURE) &&
          (PLAYER_CARD_INSTANCE(target_player, current_card).color & color_mask) != 0)
      {
        ++damage;
      }
    }
    if (damage != 0)
    {
      damage_player(target_player, damage * 2, player, card);
    }
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004314d4
// FUNCTION: SHANDALAR 0x004401d9
int card_martyr_s_cry(int player, int card, event_t event)
{
  struct
  {
    int killed_count[2];
    int current_card;
    int current_player;
    int color_mask;
  } s;

  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    s.color_mask = 1 << (unsigned char)get_sleighted_color(player, card, COLOR_WHITE);
    s.killed_count[0] = s.killed_count[1] = 0;
    for (s.current_player = 0; s.current_player < 2; ++s.current_player)
    {
      for (s.current_card = 0; s.current_card < g_active_cards_count[s.current_player]; ++s.current_card)
      {
        if (is_in_play(s.current_player, s.current_card) &&
            (global_cards_data[PLAYER_CARD_INSTANCE(s.current_player, s.current_card).internal_card_id].type & TYPE_CREATURE) &&
            (s.color_mask & PLAYER_CARD_INSTANCE(s.current_player, s.current_card).color))
        {
          kill_card(s.current_player, s.current_card, KILL_REMOVE);
          ++s.killed_count[s.current_player];
        }
      }
    }

    for (s.current_player = 0; s.current_player < 2; ++s.current_player)
    {
      for (s.current_card = 0; s.current_card < s.killed_count[s.current_player]; ++s.current_card)
      {
        draw_card_for_player(s.current_player);
      }
    }
    kill_card(player, card, KILL_BURY);
  }
  return 0;
}

// FUNCTION: MAGIC 0x00431683
// FUNCTION: SHANDALAR 0x00440388
int card_syphon_soul(int player, int card, event_t event)
{
  int effect_card;

  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    damage_player(1 - player, 2, player, card);
    effect_card = add_card_to_hand(player, g_duel_generated_internal_card_id_19);
    if (effect_card != -1)
    {
      PLAYER_CARD_INSTANCE(player, effect_card).original_internal_card_id =
          PLAYER_CARD_INSTANCE(player, card).internal_card_id;
      PLAYER_CARD_INSTANCE(player, effect_card).state |= 2;
      PLAYER_CARD_INSTANCE(player, effect_card).display_pic_info = CARD_ID_SYPHON_SOUL;
      PLAYER_CARD_INSTANCE(player, effect_card).unknown0x14 = 0xD7;
      TENTATIVE_set_timestamps(player, effect_card);
      PLAYER_CARD_INSTANCE(player, effect_card).damage_source_player = (char)player;
      PLAYER_CARD_INSTANCE(player, effect_card).damage_source_card = card;
    }
    kill_card(player, card, KILL_BURY);
  }

  if (event == EVENT_DEAL_DAMAGE &&
      PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).internal_card_id == g_damage_card_internal_card_id &&
      PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).damage_target_card == -1 &&
      PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).damage_source_player == player &&
      PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).damage_source_card == card &&
      PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).info_slot != 0)
  {
    PLAYER_CARD_INSTANCE(player, card).damage_source_player = (char)g_affected_card_controller;
    PLAYER_CARD_INSTANCE(player, card).damage_source_card = g_affected_card;
  }

  return 0;
}

// FUNCTION: MAGIC 0x004319ff
// FUNCTION: SHANDALAR 0x00440704
int card_typhoon(int player, int card, event_t event)
{
  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    damage_player(1 - player, g_basiclandtypes_controlled[1 - player][get_hacked_color(player, card, COLOR_BLUE)], player, card);
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    kill_card(player, card, KILL_BURY);
  }
  return 0;
}

// FUNCTION: MAGIC 0x00431aae
// FUNCTION: SHANDALAR 0x004407b3
int card_transmute_FX(int player, int card, event_t event)
{
  int old_toughness;

  if (event == EVENT_ABILITIES &&
      PLAYER_CARD_INSTANCE(player, card).damage_target_card == g_affected_card &&
      PLAYER_CARD_INSTANCE(player, card).damage_target_player == g_affected_card_controller &&
      g_affected_card != -1)
  {
    ++PLAYER_CARD_INSTANCE(player, card).info_slot;
  }

  if (PLAYER_CARD_INSTANCE(player, card).info_slot != 0)
  {
    old_toughness = PLAYER_CARD_INSTANCE((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                                         PLAYER_CARD_INSTANCE(player, card).damage_target_card)
                        .toughness;
    PLAYER_CARD_INSTANCE((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                         PLAYER_CARD_INSTANCE(player, card).damage_target_card)
        .toughness =
        PLAYER_CARD_INSTANCE((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                             PLAYER_CARD_INSTANCE(player, card).damage_target_card)
            .power;
    PLAYER_CARD_INSTANCE((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                         PLAYER_CARD_INSTANCE(player, card).damage_target_card)
        .power = old_toughness;
    PLAYER_CARD_INSTANCE(player, card).info_slot = 0;
  }
  return 0;
}
// FUNCTION: MAGIC 0x00431e02
// FUNCTION: SHANDALAR 0x00440b07
int helper_destroy_basiclandtype(int source_player,
                                 int source_card,
                                 int test_player,
                                 int test_card,
                                 int internal_card_id)
{
  if (global_cards_data[internal_card_id].type & 1)
  {
    if (card_has_basic_land_type(test_player, test_card, PLAYER_CARD_INSTANCE(source_player, source_card).info_slot) != 0)
    {
      kill_card(test_player, test_card, KILL_DESTROY);
    }
  }
  return 0;
}
