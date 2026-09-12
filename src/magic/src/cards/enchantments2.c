#include <stdio.h>
#include <string.h>
#include "../game_support.h"
#include "../global_strings.h"

int card_dummy(int player, int card, event_t event);
int card_copy_artifact(int player, int card, event_t event);
int find_highest_value_library_card_by_type(int player, int library_player, unsigned int type);
int titanias_song_count_animations(int player, int card, int internal_card_id);
int titanias_song_uncount_animations(int player, int card, int internal_card_id);
int power_struggle_exchange_permanents(int first_player, int first_card, int second_player, int second_card);
int dispatch_trigger_twice_once_with_each_player_as_reason(int reason_for_trig, trigger_t trig, const char *prompt, int a4);
int gain_control(int player, int card);
int spirit_shackle_add_counter(int player, int card);
void get_landwalk_evasion_masks(unsigned int *out_landwalk_mask, unsigned int *out_basic_land_mask);
int can_block_attacker_with_abilities(int blocker_player, int blocker_card, int attacker_player, int attacker_card,
                                      unsigned int attacker_abilities, unsigned int land_bits);
int helper_ward(int player, int card, event_t event, int color);
int helper_circle_of_protection(int player, int card, event_t event, int color);

// FUNCTION: MAGIC 0x00517370
// FUNCTION: SHANDALAR 0x004ceb50
int card_x_cyclopean(int player, int card, event_t event)
{
  return 0;
}

// FUNCTION: MAGIC 0x00517382
// FUNCTION: SHANDALAR 0x004ceb62
int card_island_sanctuary(int player, int card, event_t event)
{
  int current_card;
  int internal_card_id;
  int found_attacker;
  int index;

  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if (event == EVENT_CAST_SPELL && g_affected_card == card && g_affected_card_controller == player)
  {
    if (count_permanents_by_internal_card_id(player, PLAYER_CARD_INSTANCE(player, card).internal_card_id, player) == 0)
    {
      g_ai_modifier += 0x30;
    }
    return 0;
  }

  if (g_trigger_condition == 0xcf && g_current_phase == 10 && g_current_turn == g_current_player && g_affected_card == card && g_affected_card_controller == player && PLAYER_CARD_INSTANCE(player, card).eot_toughness == 0)
  {
    if (event == EVENT_TRIGGER)
    {
      if (player == g_other_player && (g_duel_network_flags & 2) == 0)
      {
        if ((PLAYER_CARD_INSTANCE(player, card).info_slot & 2) == 0)
        {
          PLAYER_CARD_INSTANCE(player, card).info_slot |= 2;
          for (current_card = 0, found_attacker = 0;
               current_card < g_active_cards_count[g_active_player] && !found_attacker;
               ++current_card)
          {
            internal_card_id = PLAYER_CARD_INSTANCE(g_active_player, current_card).internal_card_id;
            if (is_in_play(g_active_player, current_card) &&
                (global_cards_data[internal_card_id].type & TYPE_CREATURE) != 0 &&
                (PLAYER_CARD_INSTANCE(g_active_player, current_card).regen_status & 0x20) == 0 &&
                (PLAYER_CARD_INSTANCE(g_active_player, current_card).regen_status &
                 (1 << (get_hacked_color(player, card, 2) - 1))) == 0 &&
                (global_cards_data[internal_card_id].subtype != 0 ||
                 (PLAYER_CARD_INSTANCE(g_active_player, current_card).token_status & STATUS_WALL_CAN_ATTACK) != 0))
            {
              found_attacker = 1;
            }
          }
          if (found_attacker && internal_rand(8 - g_duel_summary.hand_counts[player]) == 0)
          {
            PLAYER_CARD_INSTANCE(player, card).info_slot |= 1;
          }
        }
        if ((PLAYER_CARD_INSTANCE(player, card).info_slot & 1) != 0)
        {
          g_event_result |= 2;
        }
      }
      else
      {
        g_event_result |= 1;
      }
    }

    if (event == EVENT_RESOLVE_TRIGGER)
    {
      PLAYER_CARD_INSTANCE(player, card).info_slot &= ~1;
      g_player_special_effect_flags[player] |= 1;
      current_card = create_legacy_effect(player, card, g_duel_generated_internal_card_id_0f, -1, -1);
      if (current_card != -1)
      {
        PLAYER_CARD_INSTANCE(player, current_card).token_status |= 0x400020;
        PLAYER_CARD_INSTANCE(player, current_card).info_slot = (1 << (get_hacked_color(player, card, 2) - 1)) | 0x20;
        if ((PLAYER_CARD_INSTANCE(player, card).token_status & STATUS_HACKED) != 0)
        {
          PLAYER_CARD_INSTANCE(player, current_card).token_status |= 2;
          for (index = 0; index < 6; ++index)
          {
            PLAYER_CARD_INSTANCE(player, current_card).hack_mode[index] = PLAYER_CARD_INSTANCE(player, card).hack_mode[index];
          }
        }
      }
      g_duel_state_0091c4fc = 1;
      ++PLAYER_CARD_INSTANCE(player, card).eot_toughness;
    }
  }

  if ((event == EVENT_CLEANUP || event == EVENT_SHOULD_AI_PLAY) && g_affected_card == card && g_affected_card_controller == player)
  {
    PLAYER_CARD_INSTANCE(player, card).eot_toughness = 0;
    PLAYER_CARD_INSTANCE(player, card).info_slot = PLAYER_CARD_INSTANCE(player, card).eot_toughness;
  }

  return 0;
}

// FUNCTION: MAGIC 0x00517a2c
// FUNCTION: SHANDALAR 0x004cf20b
int card_kormus_bell(int player, int card, event_t event)
{
  int effect_card;

  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  PLAYER_CARD_INSTANCE(player, card).info_slot = get_hacked_color(player, card, 1);

  if (event == EVENT_CHANGE_TYPE && is_in_play(player, card) && ((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0 || (global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].type & TYPE_CREATURE) != 0))
  {
    if ((g_land_can_be_played & LCBP_DURING_EVENT_CHANGE_TYPE_SECOND_PASS) != 0)
    {
      if (is_basic_land_internal_card_id_of_color(g_event_result, PLAYER_CARD_INSTANCE(player, card).info_slot) != 0 && dispatch_function_to_all_cards_in_play(player, card, find_land_animation_legacy, player) == -1)
      {
        effect_card = create_legacy_effect(player, card, g_duel_generated_internal_card_id_28, g_affected_card_controller, g_affected_card);
        if (effect_card != -1)
        {
          int animated_internal_card_id;
          int found;

          PLAYER_CARD_INSTANCE(player, effect_card).info_slot = PLAYER_CARD_INSTANCE(player, card).info_slot;
          PLAYER_CARD_INSTANCE(player, effect_card).token_status |= 0x10000;

          animated_internal_card_id = g_card_count;
          found = 0;
          do
          {
            if (global_cards_data[animated_internal_card_id].id == global_cards_data[g_event_result].id)
            {
              found = 1;
            }
            else
            {
              ++animated_internal_card_id;
            }
          } while (animated_internal_card_id < g_card_count + 16 && !found);
          if (!found)
          {
            animated_internal_card_id = create_a_card_type(g_event_result);
            if (animated_internal_card_id != -1)
            {
              global_cards_data[animated_internal_card_id].type |= TYPE_CREATURE;
              global_cards_data[animated_internal_card_id].extra_ability |= 0x8000;
              global_cards_data[animated_internal_card_id].power = 1;
              global_cards_data[animated_internal_card_id].toughness = 1;
              global_cards_data[animated_internal_card_id].cc[0] = 1;
            }
          }
          if (animated_internal_card_id != -1)
          {
            PLAYER_CARD_INSTANCE(player, effect_card).dummy3 = animated_internal_card_id;
          }
        }
      }
    }
    else
    {
      g_land_can_be_played |= LCBP_NEED_EVENT_CHANGE_TYPE_SECOND_PASS;
    }
  }

  if (event == EVENT_GRAVEYARD_FROM_PLAY && card == g_affected_card && player == g_affected_card_controller)
  {
    dispatch_function_to_all_cards_in_play(player, card, destroy_land_animation_legacies, player);
  }

  return 0;
}

// FUNCTION: MAGIC 0x00517dc9
// FUNCTION: SHANDALAR 0x004cf5a7
int card_living_lands(int player, int card, event_t event)
{
  int effect_card;

  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  PLAYER_CARD_INSTANCE(player, card).info_slot = get_hacked_color(player, card, 3);

  if (event == EVENT_CHANGE_TYPE && is_in_play(player, card))
  {
    if ((g_land_can_be_played & LCBP_DURING_EVENT_CHANGE_TYPE_SECOND_PASS) != 0)
    {
      if (is_basic_land_internal_card_id_of_color(g_event_result, PLAYER_CARD_INSTANCE(player, card).info_slot) != 0 && dispatch_function_to_all_cards_in_play(player, card, find_land_animation_legacy, player) == -1)
      {
        effect_card = create_legacy_effect(player, card, g_duel_generated_internal_card_id_28, g_affected_card_controller, g_affected_card);
        if (effect_card != -1)
        {
          int animated_internal_card_id;
          int found;

          PLAYER_CARD_INSTANCE(player, effect_card).info_slot = PLAYER_CARD_INSTANCE(player, card).info_slot;
          PLAYER_CARD_INSTANCE(player, effect_card).token_status |= 0x10000;

          animated_internal_card_id = g_card_count;
          found = 0;
          do
          {
            if (*(int *)&global_cards_data[animated_internal_card_id].id == *(int *)&global_cards_data[g_event_result].id)
            {
              found = 1;
            }
            else
            {
              ++animated_internal_card_id;
            }
          } while (animated_internal_card_id < g_card_count + 16 && !found);
          if (!found)
          {
            animated_internal_card_id = create_a_card_type(g_event_result);
            if (animated_internal_card_id != -1)
            {
              global_cards_data[animated_internal_card_id].type |= TYPE_CREATURE;
              global_cards_data[animated_internal_card_id].extra_ability |= 0x8000;
              global_cards_data[animated_internal_card_id].power = 1;
              global_cards_data[animated_internal_card_id].toughness = 1;
              global_cards_data[animated_internal_card_id].cc[0] = 1;
            }
          }
          if (animated_internal_card_id != -1)
          {
            PLAYER_CARD_INSTANCE(player, effect_card).dummy3 = animated_internal_card_id;
          }
        }
      }
    }
    else
    {
      g_land_can_be_played |= LCBP_NEED_EVENT_CHANGE_TYPE_SECOND_PASS;
    }
  }

  if (event == EVENT_GRAVEYARD_FROM_PLAY && card == g_affected_card && player == g_affected_card_controller)
  {
    dispatch_function_to_all_cards_in_play(player, card, destroy_land_animation_legacies, player);
  }

  return 0;
}

// FUNCTION: MAGIC 0x005180ed
// FUNCTION: SHANDALAR 0x004cf8cc
int find_land_animation_legacy(int a1, int a2, int player, int card, int internal_card_id)
{
  int result; // ebp-0x4

  result = 0;
  if (internal_card_id == g_duel_generated_internal_card_id_28)
  {
    if ((int)(char)PLAYER_CARD_INSTANCE(player, card).damage_target_player == g_affected_card_controller)
    {
      if (PLAYER_CARD_INSTANCE(player, card).damage_target_card == g_affected_card)
      {
        result = 1;
        g_event_result = result;
      }
    }
  }

  return result;
}

// FUNCTION: MAGIC 0x0051819c
// FUNCTION: SHANDALAR 0x004cf97c
int destroy_land_animation_legacies(int parent_player, int parent_card, int player, int card, int internal_card_id)
{
  if (internal_card_id == g_duel_generated_internal_card_id_28 && PLAYER_CARD_INSTANCE(player, card).damage_source_player == parent_player && PLAYER_CARD_INSTANCE(player, card).damage_source_card == parent_card)
  {
    kill_card(player, card, KILL_DESTROY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x0051823d
// FUNCTION: SHANDALAR 0x004cfa1d
int card_sylvan_library(int player, int card, event_t event)
{
  struct
  {
    union
    {
      int count;
      int source_color;
    } u;
    char prompt[600];
    int selected_index;
    int choice_put_back;
    int current_card;
    int ai_choice;
    int loop_index;
    int card_ids[30];
    int card_indices[30];
  } s;

  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if (event == EVENT_CAST_SPELL && card == g_affected_card && player == g_affected_card_controller)
  {
    g_ai_modifier += 0x30;
    return 0;
  }

  if (event == EVENT_CAN_ACTIVATE &&
      player == g_current_player &&
      PLAYER_CARD_INSTANCE(player, card).info_slot == 0 &&
      g_current_phase == PHASE_DRAW)
  {
    if (g_mana_cost_modifiers_by_color[single_color_test_bit_to_color_t((int)PLAYER_CARD_INSTANCE(player, card).color)] != 0 &&
        has_mana_w_global_cost_mod(player, card, COLOR_ANY, 0) == 0)
    {
      return 0;
    }
    if (player == g_other_player && (g_duel_network_flags & 2) == 0)
    {
      g_activation_event_flags |= 3;
    }
    return 1;
  }

  if (event == EVENT_ACTIVATE && PLAYER_CARD_INSTANCE(player, card).info_slot == 0)
  {
    if (g_mana_cost_modifiers_by_color[single_color_test_bit_to_color_t((int)PLAYER_CARD_INSTANCE(player, card).color)] != 0)
    {
      charge_mana_w_global_cost_mod(player, card, COLOR_COLORLESS, 0);
    }
    if (g_spell_fizzled != 1)
    {
      PLAYER_CARD_INSTANCE(player, card).info_slot = 1;
    }
    return 0;
  }

  if (event == EVENT_RESOLVE_ACTIVATION && PLAYER_CARD_INSTANCE(player, card).info_slot != 0)
  {
    for (s.loop_index = 0; s.loop_index < 2; ++s.loop_index)
    {
      draw_card_for_player(g_current_player);
    }

    for (s.loop_index = 0; s.loop_index < 2; ++s.loop_index)
    {
      s.u.count = 0;
      for (s.current_card = 0; s.current_card < g_active_cards_count[player]; ++s.current_card)
      {
        if (PLAYER_CARD_INSTANCE(player, s.current_card).internal_card_id != -1 &&
            (PLAYER_CARD_INSTANCE(player, s.current_card).state & STATE_JUST_DRAWED) != 0 &&
            (PLAYER_CARD_INSTANCE(player, s.current_card).state & STATE_IN_PLAY) == 0)
        {
          s.card_ids[s.u.count] = PLAYER_CARD_INSTANCE(player, s.current_card).internal_card_id;
          s.card_indices[s.u.count] = s.current_card;
          ++s.u.count;
        }
      }

      s.ai_choice = 0;
      if (g_life[player] > 18)
      {
        s.ai_choice += g_life[player] + 20;
      }
      if (g_duel_summary.hand_counts[player] < 7)
      {
        s.ai_choice += (7 - g_duel_summary.hand_counts[player]) * 4 + 10;
      }
      if (s.loop_index > 0 && s.choice_put_back == 0)
      {
        s.ai_choice /= 2;
      }
      if (g_life[player] < 7)
      {
        s.ai_choice = 0;
      }

      s.choice_put_back = (s.ai_choice <= internal_rand(100));
      if (g_duel_ai_mode_state != 1)
      {
        load_text("prompts.txt", "SYLVAN_LIBRARY");
        sprintf(s.prompt, " %s\n %s", g_text_lines[0], g_text_lines[1]);
      }

      if (do_dialog(player, g_card_on_stack_controller, g_card_on_stack, -1, -1, s.prompt, s.choice_put_back) != 0)
      {
        if ((player == g_active_player || (g_duel_network_flags & 2) != 0) &&
            g_duel_ai_mode_state != 1 &&
            g_duel_network_state == 0)
        {
          if (player == g_active_player)
          {
            load_text("prompts.txt", "SYLVAN_LIBRARY");
          }
          s.selected_index = show_deck(player, s.card_ids, s.u.count, g_text_lines[2], 1, "");
          real_put_on_top_of_deck(player, s.card_ids[s.selected_index]);
          PLAYER_CARD_INSTANCE(player, s.card_indices[s.selected_index]).internal_card_id = -1;
          --g_duel_summary.hand_counts[player];
        }
        else if (s.u.count > 0)
        {
          s.selected_index = internal_rand(s.u.count);
          real_put_on_top_of_deck(player, s.card_ids[s.selected_index]);
          PLAYER_CARD_INSTANCE(player, s.card_indices[s.selected_index]).internal_card_id = -1;
          --g_duel_summary.hand_counts[player];
        }
      }
      else
      {
        g_life[player] -= 4;
      }
    }
    return 0;
  }

  if (event == EVENT_CLEANUP && card == g_affected_card && player == g_affected_card_controller)
  {
    PLAYER_CARD_INSTANCE(player, card).info_slot = 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x00518959
// FUNCTION: SHANDALAR 0x004d0138
int card_land_tax(int player, int card, event_t event)
{
  struct
  {
    int first_selected;
    int available[500];
    char *prompt_lines[3];
    int search_count_or_found_pending_land;
    int selected_count;
    int current_card;
    int selected[6];
    int library_index;
  } s;

  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if (event == EVENT_CAST_SPELL && g_affected_card == card && player == g_affected_card_controller)
  {
    if (count_permanents_by_internal_card_id(player, PLAYER_CARD_INSTANCE(player, card).internal_card_id, player) == 0)
    {
      g_ai_modifier += (g_basiclandtypes_controlled[1 - player][7] -
                        g_basiclandtypes_controlled[player][7] + 2) *
                       12;
    }
    return 0;
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    if (g_current_phase == PHASE_UPKEEP &&
        player == g_current_player &&
        player == g_event_player &&
        PLAYER_CARD_INSTANCE(player, card).info_slot == 0 &&
        g_basiclandtypes_controlled[player][7] < g_basiclandtypes_controlled[1 - player][7])
    {
      if (g_mana_cost_modifiers_by_color[single_color_test_bit_to_color_t((int)PLAYER_CARD_INSTANCE(player, card).color)] == 0 || has_mana_w_global_cost_mod(player, card, COLOR_ANY, 0) != 0)
      {
        if (player == g_other_player && (g_duel_network_flags & 2) == 0 && global_library[player][4] != -1)
        {
          g_activation_event_flags |= 3;
        }
        return 1;
      }
    }
    return 0;
  }

  if (event == EVENT_ACTIVATE && card == g_affected_card && player == g_affected_card_controller)
  {
    if (g_mana_cost_modifiers_by_color[single_color_test_bit_to_color_t((int)PLAYER_CARD_INSTANCE(player, card).color)] != 0)
    {
      charge_mana_w_global_cost_mod(player, card, COLOR_COLORLESS, 0);
    }
    if (g_spell_fizzled != 1)
    {
      ++PLAYER_CARD_INSTANCE(player, card).info_slot;
    }
    return 0;
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    if ((player == g_other_player && (g_duel_network_flags & 2) == 0) || g_duel_ai_mode_state == 1)
    {
      s.selected_count = 0;
      for (s.current_card = 0, s.search_count_or_found_pending_land = 0;
           s.current_card < g_active_cards_count[player] && s.search_count_or_found_pending_land == 0;
           ++s.current_card)
      {
        if (is_card_pending_resolution(player, s.current_card) != 0 &&
            (global_cards_data[PLAYER_CARD_INSTANCE(player, s.current_card).internal_card_id].type & TYPE_LAND) != 0)
        {
          ++s.search_count_or_found_pending_land;
        }
      }
      if (s.search_count_or_found_pending_land == 0)
      {
        g_ai_modifier += 0x30;
      }

      s.search_count_or_found_pending_land = ClampIntToRange(8 - g_duel_summary.hand_counts[player], 1, 3);
      for (s.current_card = 0; s.current_card < s.search_count_or_found_pending_land; ++s.current_card)
      {
        s.library_index = find_highest_value_library_card_by_type(player, player, TYPE_LAND);
        if (global_library[player][s.library_index] > 4)
        {
          s.library_index = -1;
          for (s.current_card = 0;
               s.current_card < 500 && s.library_index == -1 && global_library[player][s.current_card] != -1;
               ++s.current_card)
          {
            if (global_library[player][s.current_card] < 5)
            {
              s.library_index = s.current_card;
            }
          }
        }
        if (s.library_index != -1 && global_library[player][s.library_index] != -1)
        {
          s.selected[s.selected_count] = global_library[player][s.library_index];
          ++s.selected_count;
          remove_card_from_deck(player, s.library_index);
        }
      }

      if (player == g_other_player)
      {
        if (g_duel_ai_mode_state != 1)
        {
          load_text("prompts.txt", "LANDTAX");
        }
        show_deck(0, s.selected, s.selected_count, g_text_lines[3], 0, gs_done_008b40e0);
      }

      for (s.current_card = 0; s.current_card < s.selected_count; ++s.current_card)
      {
        add_card_to_hand(player, s.selected[s.current_card]);
        ++g_duel_summary.hand_counts[player];
      }
    }
    else
    {
      load_text("prompts.txt", "LANDTAX");
      s.prompt_lines[0] = g_text_lines[0];
      s.prompt_lines[1] = g_text_lines[1];
      s.prompt_lines[2] = g_text_lines[2];

      for (s.current_card = 0; s.current_card < 500; ++s.current_card)
      {
        s.available[s.current_card] = 0;
      }
      for (s.current_card = 0; global_library[player][s.current_card] != -1; ++s.current_card)
      {
        if (global_library[player][s.current_card] < 5)
        {
          s.available[s.current_card] = 1;
        }
      }

      s.selected_count = select_from_graveyard_with_dialog(player,
                                                           global_library[player],
                                                           s.available,
                                                           500,
                                                           (int)s.prompt_lines,
                                                           3,
                                                           (int)s.selected,
                                                           0,
                                                           3);
      for (s.current_card = 0; s.current_card < s.selected_count; ++s.current_card)
      {
        add_card_to_hand(player, global_library[player][s.selected[s.current_card]]);
        s.selected[s.current_card + 3] = global_library[player][s.selected[s.current_card]];
        ++g_duel_summary.hand_counts[player];
      }

      if ((g_duel_network_flags & 2) != 0)
      {
        if (g_duel_ai_mode_state != 1)
        {
          load_text("prompts.txt", "LANDTAX");
        }
        show_deck(1 - player, s.selected + 3, s.selected_count, g_text_lines[3], 0, gs_done_008b40e0);
      }

      if (s.selected_count == 1)
      {
        remove_card_from_deck(player, s.selected[0]);
      }
      if (s.selected_count == 2)
      {
        remove_card_from_deck(player, MAX(s.selected[1], s.selected[0]));
        remove_card_from_deck(player, MIN(s.selected[1], s.selected[0]));
      }
      if (s.selected_count == 3)
      {
        s.first_selected = s.selected[0];
        remove_card_from_deck(player, s.selected[0]);
        if (s.first_selected < s.selected[1])
        {
          --s.selected[1];
        }
        if (s.first_selected < s.selected[2])
        {
          --s.selected[2];
        }
        remove_card_from_deck(player, MAX(s.selected[1], s.selected[2]));
        remove_card_from_deck(player, MIN(s.selected[1], s.selected[2]));
      }
      TENTATIVE_reassess_all_cards(0, 0xff);
    }

    shuffle_duel_library(player, player);
    return 0;
  }

  if (event == EVENT_CLEANUP && card == g_affected_card && player == g_affected_card_controller)
  {
    PLAYER_CARD_INSTANCE(player, card).info_slot = 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x00519218
// FUNCTION: SHANDALAR 0x004d09fa
int card_kismet(int player, int card, event_t event)
{
  target_t target;

  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if (event == EVENT_CAST_SPELL && g_affected_card == card && g_affected_card_controller == player)
  {
    if (g_duel_ai_mode_state != 1)
    {
      load_text("prompts.txt", "KISMET");
    }
    if (C_real_select_target(player,
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
      PLAYER_CARD_INSTANCE(player, card).info_slot = target.player;
      SET_TARGET(PLAYER_CARD_INSTANCE(player, card).targets[0], target);
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
      if (count_permanents_by_internal_card_id(player, PLAYER_CARD_INSTANCE(player, card).internal_card_id, -1) == 0)
      {
        g_ai_modifier += 0x30;
      }
    }
    else
    {
      g_spell_fizzled = 1;
    }
  }

  if (event == EVENT_CAST_SPELL &&
      (g_affected_card != card || g_affected_card_controller != player) &&
      is_in_play(player, card) &&
      PLAYER_CARD_INSTANCE(player, card).info_slot == g_affected_card_controller &&
      (global_cards_data[PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).internal_card_id].type &
       (TYPE_LAND | TYPE_CREATURE | TYPE_ARTIFACT)) != 0)
  {
    PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).state |= STATE_TAPPED;
  }

  return 0;
}

// FUNCTION: MAGIC 0x005194e4
// FUNCTION: SHANDALAR 0x004d0cc6
int card_gloom(int player, int card, event_t event)
{
  int chosen_color;

  if (event == EVENT_COUNT_MANA && g_affected_card == card && g_affected_card_controller == player)
  {
    chosen_color = get_sleighted_color(player, card, 5);
    g_mana_cost_modifiers_by_color[chosen_color] += 3;
    return 0;
  }

  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if ((event == EVENT_CAST_SPELL || event == EVENT_SHOULD_AI_PLAY) && g_affected_card == card && g_affected_card_controller == player)
  {
    g_ai_modifier += (g_basiclandtypes_controlled[1 - player][get_sleighted_color(player, card, 5)] - g_basiclandtypes_controlled[player][get_sleighted_color(player, card, 5)] * 2 + 1) * 12;
  }

  return 0;
}
// FUNCTION: MAGIC 0x005195e5
// FUNCTION: SHANDALAR 0x004d0dc7
int card_animate_dead(int player, int card, event_t event)
{
  int allowed[500];
  char prompt[900];
  char graveyard_name[100];
  int chosen_graveyard;
  int graveyard_data[4];
  int current_player;
  int current_slot;

  if (event == EVENT_CAN_CAST)
  {
    if (player == g_active_player || (g_duel_network_flags & 2) != 0)
    {
      return (g_graveyard_card_types[0] | g_graveyard_card_types[1]) & TYPE_CREATURE;
    }

    if (g_duel_ai_mode_state == 1)
    {
      g_ai_recorded_choice = internal_rand(2);
      record_ai_action_selection();
    }
    else
    {
      replay_ai_action_selection();
    }
    PLAYER_CARD_INSTANCE(player, card).info_slot = g_ai_recorded_choice;
    return g_graveyard_card_types[g_ai_recorded_choice] & TYPE_CREATURE;
  }
  else if (((event == EVENT_CAST_SPELL) && (card == g_affected_card)) && (player == g_affected_card_controller))
  {
    if (((player == g_other_player) && (g_duel_network_flags & 2) == 0) || g_duel_ai_mode_state == 1)
    {
      chosen_graveyard = PLAYER_CARD_INSTANCE(player, card).info_slot;
      graveyard_data[3] = find_highest_value_graveyard_card_by_type(chosen_graveyard, TYPE_CREATURE);
    }
    else
    {
      graveyard_data[1] = 0;
      graveyard_data[0] = 0;
      for (current_player = 0; current_player < 2; current_player = current_player + 1)
      {
        current_slot = 0;
        while (1)
        {
          if (499 < current_slot || global_graveyard_slots[current_player][current_slot] == -1)
          {
            break;
          }
          if ((global_cards_data[global_graveyard_slots[current_player][current_slot]].type & TYPE_CREATURE) != 0)
          {
            graveyard_data[current_player] = graveyard_data[current_player] + 1;
            break;
          }
          current_slot = current_slot + 1;
        }
      }

      if (graveyard_data[0] == 0 || graveyard_data[1] == 0)
      {
        if (graveyard_data[0] == 0)
        {
          chosen_graveyard = 1;
        }
        else
        {
          chosen_graveyard = 0;
        }
      }
      else
      {
        load_text("prompts.txt", "ANIMATE_DEAD");
        if ((g_duel_network_flags & 2) == 0 || player != g_other_player)
        {
          sprintf(prompt, " %s\n %s\n %s", g_text_lines[0], g_text_lines[1], g_text_lines[2]);
        }
        else
        {
          sprintf(prompt, " %s\n %s\n %s", g_text_lines[1], g_text_lines[0], g_text_lines[2]);
        }
        chosen_graveyard = do_dialog(player, player, card, -1, -1, prompt, 0);
        if (chosen_graveyard == 2)
        {
          g_spell_fizzled = 1;
        }
        else if ((g_duel_network_flags & 2) != 0 && player == g_other_player)
        {
          chosen_graveyard = 1 - chosen_graveyard;
        }
      }

      if (g_spell_fizzled != 1)
      {
        if (chosen_graveyard == 0)
        {
          load_text("prompts.txt", "ANIMATE_DEAD");
          strcpy(prompt, g_text_lines[4]);
        }
        else
        {
          load_text("prompts.txt", "ANIMATE_DEAD");
          copy_opponent_name_prefix(graveyard_name);
          sprintf(prompt, g_text_lines[3], graveyard_name);
        }

        memset(allowed, 0, 2000);
        for (current_slot = 0; current_slot < 500 && global_graveyard_slots[chosen_graveyard][current_slot] != -1;
             ++current_slot)
        {
          if ((global_cards_data[global_graveyard_slots[chosen_graveyard][current_slot]].type & TYPE_CREATURE) != 0)
          {
            allowed[current_slot] = 1;
          }
        }

        graveyard_data[3] =
            select_card_from_graveyard(player, global_graveyard_slots[chosen_graveyard], allowed, 500, prompt, 0, gs_cancel_008a8c20);
        if (graveyard_data[3] == -1)
        {
          g_spell_fizzled = 1;
        }
      }
    }

    if ((((g_spell_fizzled == 1) || (graveyard_data[3] == -1)) || (global_graveyard_slots[chosen_graveyard][graveyard_data[3]] == -1)) || ((global_cards_data[global_graveyard_slots[chosen_graveyard][graveyard_data[3]]].type & TYPE_CREATURE) == 0))
    {
      g_spell_fizzled = 1;
    }
    else
    {
      current_player = add_card_to_hand(player, global_graveyard_slots[chosen_graveyard][graveyard_data[3]]);
      if (current_player != -1)
      {
        PLAYER_CARD_INSTANCE(player, card).damage_target_card = current_player;
        PLAYER_CARD_INSTANCE(player, card).damage_target_player = player;
        *(unsigned int *)&DAMAGE_TARGET_CARD_INSTANCE(player, card).state &= ~STATE_OWNED_BY_OPPONENT;
        if (chosen_graveyard != 0)
        {
          *(unsigned int *)&DAMAGE_TARGET_CARD_INSTANCE(player, card).state |= 0x1000;
        }
        *(unsigned int *)&DAMAGE_TARGET_CARD_INSTANCE(player, card).state |= 0x20;
        PLAYER_CARD_INSTANCE(player, card).eot_toughness = 1;
        PLAYER_CARD_INSTANCE(player, card).targets[1].player = chosen_graveyard;
        PLAYER_CARD_INSTANCE(player, card).targets[1].card = graveyard_data[3];
        PLAYER_CARD_INSTANCE(player, card).targets[0].player = (int)(char)PLAYER_CARD_INSTANCE(player, card).damage_target_player;
        PLAYER_CARD_INSTANCE(player, card).targets[0].card = PLAYER_CARD_INSTANCE(player, card).damage_target_card;
        PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
      }
    }
    return 0;
  }
  else if (event == EVENT_RESOLVE_SPELL)
  {
    graveyard_data[3] = PLAYER_CARD_INSTANCE(player, card).targets[1].card;
    chosen_graveyard = PLAYER_CARD_INSTANCE(player, card).targets[1].player;
    if (global_graveyard_slots[chosen_graveyard][graveyard_data[3]] == -1)
    {
      kill_card(player, card, KILL_DESTROY);
      TARGET_CARD_INSTANCE(player, card, 0).internal_card_id = -1;
      g_spell_fizzled = 1;
    }
    else
    {
      remove_card_from_graveyard(chosen_graveyard, graveyard_data[3]);
      PLAYER_CARD_INSTANCE(player, card).eot_toughness = 0;
      process_card_enters_play((int)(char)PLAYER_CARD_INSTANCE(player, card).damage_target_player, PLAYER_CARD_INSTANCE(player, card).damage_target_card);
      DAMAGE_TARGET_CARD_INSTANCE(player, card).counter_power = -1;
    }
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    return 0;
  }
  else
  {
    if (((((g_trigger_condition == 0xd4) && (card == g_affected_card)) && ((player == g_affected_card_controller) && ((PLAYER_CARD_INSTANCE(player, card).damage_target_player != -1) && (DAMAGE_TARGET_CARD_INSTANCE(player, card).internal_card_id != -1)))) &&
         (player == g_trigger_cause_controller)) &&
        ((card == g_trigger_cause) && (g_current_turn == player)))
    {
      if (event == EVENT_TRIGGER)
      {
        g_event_result |= 2;
      }
      if (event == EVENT_RESOLVE_TRIGGER)
      {
        if (PLAYER_CARD_INSTANCE(player, card).eot_toughness != 0)
        {
          DAMAGE_TARGET_CARD_INSTANCE(player, card).token_status |= STATUS_OBLITERATED;
        }
        current_player = (char)PLAYER_CARD_INSTANCE(player, card).damage_target_player;
        PLAYER_CARD_INSTANCE(player, card).damage_target_player = -1;
        kill_card(current_player, PLAYER_CARD_INSTANCE(player, card).damage_target_card, KILL_BURY);
      }
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0051a41c
// FUNCTION: SHANDALAR 0x004d1c03
int process_card_enters_play(int player, int card)
{
  int internal_card_id;
  int card_type;

  if (player == -1 || card == -1)
  {
    return 0;
  }

  internal_card_id = PLAYER_CARD_INSTANCE(player, card).internal_card_id;
  card_type = (unsigned char)global_cards_data[internal_card_id].type;

  /* Special-case a couple of card ids: if they can't be cast, immediately bury them. */
  if ((global_cards_data[internal_card_id].id == 0x107 || global_cards_data[internal_card_id].id == 0x27) &&
      dispatch_event_to_single_card(player, card, 0x74, 1 - player, -1) == 0)
  {
    kill_card(player, card, 2);
    return 0;
  }

  if ((card_type & 2) != 0)
  {
    g_duel_summary.creature_counts[player] += 1;
  }
  if ((card_type & 0x40) != 0)
  {
    g_duel_summary.artifact_counts[player] += 1;
  }
  if ((card_type & 4) != 0)
  {
    g_duel_summary.enchantment_counts[player] += 1;
  }

  g_card_types_in_play[player] |= (unsigned int)card_type;

  PLAYER_CARD_INSTANCE(player, card).state |= 0x30022;
  dispatch_event(player, card, 0x6c);

  PLAYER_CARD_INSTANCE(player, card).state |= ((unsigned int)player >= 1u ? 0x400000 : 0) | 0x80;
  dispatch_event_to_single_card(player, card, 0x71, 1 - player, -1);

  PLAYER_CARD_INSTANCE(player, card).state &= ~0x20;

  g_trigger_cause_controller = player;
  g_trigger_cause = card;
  dispatch_trigger_twice_once_with_each_player_as_reason(g_current_player, TRIGGER_COMES_INTO_PLAY, gs_card_into_play_0091c840, 0);

  if ((card_type & 1) != 0)
  {
    g_duel_summary.land_entries += 1;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0051a64c
// FUNCTION: SHANDALAR 0x004d1e33
int card_animate_artifact(int player, int card, event_t event)
{
  int animated_internal_card_id;
  target_t selected_target;

  if (event == EVENT_CAN_CAST)
  {
    return real_target_available((int *)0,
                                 TARGET_SCAN_DIRECT,
                                 player,
                                 2,
                                 2,
                                 0x200,
                                 0x40,
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

  if (((event == EVENT_CAST_SPELL) && (card == g_affected_card)) && (player == g_affected_card_controller))
  {
    if (g_duel_ai_mode_state != 1)
    {
      load_text("prompts.txt", "ANIMATE_ARTIFACT");
    }
    if (C_real_select_target(player,
                             2,
                             2,
                             TARGET_ZONE_IN_PLAY,
                             TYPE_ARTIFACT,
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
                             0,
                             0,
                             g_text_lines[0],
                             1,
                             &selected_target))
    {
      SET_TARGET(PLAYER_CARD_INSTANCE(player, card).targets[0], selected_target);
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
      g_ai_modifier += -0x18;
    }
    else
    {
      g_spell_fizzled = 1;
    }
    return 0;
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
                               0))
    {
      PLAYER_CARD_INSTANCE(player, card).damage_target_player =
          (char)PLAYER_CARD_INSTANCE(player, card).targets[0].player;
      PLAYER_CARD_INSTANCE(player, card).damage_target_card =
          PLAYER_CARD_INSTANCE(player, card).targets[0].card;
      animated_internal_card_id = create_a_card_type(
          DAMAGE_TARGET_CARD_INSTANCE(player, card).internal_card_id);
      if (animated_internal_card_id != -1)
      {
        if ((global_cards_data[DAMAGE_TARGET_CARD_INSTANCE(player, card).internal_card_id]
                 .type &
             0x42) == 0x40)
        {
          global_cards_data[animated_internal_card_id].toughness =
              (short)(char)global_cards_data
                  [DAMAGE_TARGET_CARD_INSTANCE(player, card).internal_card_id]
                      .cc[1];
          global_cards_data[animated_internal_card_id].power =
              global_cards_data[animated_internal_card_id].toughness;
        }
        global_cards_data[animated_internal_card_id].type = 0x42;
        PLAYER_CARD_INSTANCE(player, card).dummy3 = animated_internal_card_id;
        DAMAGE_TARGET_CARD_INSTANCE(player, card).internal_card_id = animated_internal_card_id;
        DAMAGE_TARGET_CARD_INSTANCE(player, card).regen_status |= 0x1000000;
      }
    }
    else
    {
      kill_card(player, card, KILL_BURY);
      g_spell_fizzled = 1;
    }
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    return 0;
  }

  if ((((event == EVENT_CHANGE_TYPE) && ((g_land_can_be_played & LCBP_DURING_EVENT_CHANGE_TYPE_SECOND_PASS) == 0)) && ((PLAYER_CARD_INSTANCE(player, card).damage_target_card == g_affected_card) && (((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player == g_affected_card_controller) && (g_affected_card != -1)))) && is_in_play(player, card))
  {
    g_event_result = PLAYER_CARD_INSTANCE(player, card).dummy3;
    DAMAGE_TARGET_CARD_INSTANCE(player, card).token_status |= 0x40;
  }
  return 0;
}

// FUNCTION: MAGIC 0x0051ae64
// FUNCTION: SHANDALAR 0x004d264b
int card_titania_s_song(int player, int card, event_t event)
{
  struct
  {
    int found;
    int current_card;
    int animated_internal_card_id;
    int existing_count;
  } s;

  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    dispatch_three_arg_callback_to_cards_in_play(titanias_song_count_animations, -1);
    return 0;
  }

  if (event == EVENT_CHANGE_TYPE && (g_land_can_be_played & LCBP_DURING_EVENT_CHANGE_TYPE_SECOND_PASS) == 0 &&
      is_in_play(player, card))
  {
    if ((g_affected_card != -1 && (global_cards_data[g_event_result].type & (TYPE_ARTIFACT | TYPE_CREATURE)) == TYPE_ARTIFACT) ||
        (global_cards_data[PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).internal_card_id].code_pointer == card_copy_artifact &&
         (global_cards_data[PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).dummy3].type & (TYPE_ARTIFACT | TYPE_CREATURE)) == TYPE_ARTIFACT))
    {
      s.current_card = 0;
      s.found = 0;
      for (; s.current_card < MAX(g_active_cards_count[g_other_player], g_active_cards_count[g_active_player]) && !s.found;
           ++s.current_card)
      {
        if (PLAYER_CARD_INSTANCE(g_active_player, s.current_card).internal_card_id == g_duel_generated_internal_card_id_16 &&
            is_in_play(g_active_player, s.current_card) &&
            PLAYER_CARD_INSTANCE(g_active_player, s.current_card).damage_target_player == g_affected_card_controller &&
            PLAYER_CARD_INSTANCE(g_active_player, s.current_card).damage_target_card == g_affected_card)
        {
          s.found = 1;
        }

        if (PLAYER_CARD_INSTANCE(g_other_player, s.current_card).internal_card_id == g_duel_generated_internal_card_id_16 &&
            is_in_play(g_other_player, s.current_card) &&
            PLAYER_CARD_INSTANCE(g_other_player, s.current_card).damage_target_player == g_affected_card_controller &&
            PLAYER_CARD_INSTANCE(g_other_player, s.current_card).damage_target_card == g_affected_card)
        {
          s.found = 1;
        }
      }

      if (!s.found)
      {
        if (global_cards_data[PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).internal_card_id].code_pointer == card_copy_artifact)
        {
          s.animated_internal_card_id = create_a_card_type(PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).dummy3);
        }
        else
        {
          s.animated_internal_card_id = create_a_card_type(PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).internal_card_id);
        }

        if (s.animated_internal_card_id != -1)
        {
          s.current_card = create_legacy_effect(player, card, g_duel_generated_internal_card_id_16, g_affected_card_controller, g_affected_card);
          if (s.current_card != -1)
          {
            PLAYER_CARD_INSTANCE(player, s.current_card).dummy3 = s.animated_internal_card_id;
            PLAYER_CARD_INSTANCE(player, s.current_card).token_status |= STATUS_PERMANENT | 0x10000;
            real_target_available(&s.existing_count, TARGET_SCAN_DIRECT, player, ANYBODY, ANYBODY,
                                  TARGET_ZONE_IN_PLAY, TYPE_NONE, TYPE_NONE, 0, 0, COLOR_TEST_0,
                                  COLOR_TEST_0, PLAYER_CARD_INSTANCE(player, card).internal_card_id,
                                  -1, -1, -1, 0, 0, 0);
            PLAYER_CARD_INSTANCE(player, s.current_card).eot_toughness = s.existing_count;
          }

          global_cards_data[s.animated_internal_card_id].type = TYPE_ARTIFACT | TYPE_CREATURE;
          global_cards_data[s.animated_internal_card_id].toughness =
              global_cards_data[PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).internal_card_id].cc[0] +
              global_cards_data[PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).internal_card_id].cc[1];
          global_cards_data[s.animated_internal_card_id].power = global_cards_data[s.animated_internal_card_id].toughness;
          global_cards_data[s.animated_internal_card_id].code_pointer = card_dummy;
          global_cards_data[s.animated_internal_card_id].extra_ability = EA_BECAME_CREATURE;
          global_cards_data[s.animated_internal_card_id].color = 1;
        }
      }
    }
    return 0;
  }

  if (event == EVENT_GRAVEYARD_FROM_PLAY && card == g_affected_card && player == g_affected_card_controller)
  {
    dispatch_three_arg_callback_to_cards_in_play(titanias_song_uncount_animations, -1);
  }

  return 0;
}

// FUNCTION: SHANDALAR 0x004d2ca9
int titanias_song_count_animations(int player, int card, int internal_card_id)
{
  if (internal_card_id == g_duel_generated_internal_card_id_16)
  {
    ++PLAYER_CARD_INSTANCE(player, card).eot_toughness;
  }

  return 0;
}

// FUNCTION: SHANDALAR 0x004d2cf7
int titanias_song_uncount_animations(int player, int card, int internal_card_id)
{
  if (internal_card_id == g_duel_generated_internal_card_id_16)
  {
    --PLAYER_CARD_INSTANCE(player, card).eot_toughness;
  }

  return 0;
}
// FUNCTION: MAGIC 0x0051b55f
// FUNCTION: SHANDALAR 0x004d2d45
int card_animate_wall(int player, int card, event_t event)
{
  target_t target;

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
                                 0,
                                 0,
                                 -1,
                                 -1,
                                 -1,
                                 ~SUB_WALL,
                                 TARGET_SPECIAL_WALL,
                                 0,
                                 0);
  }

  if (event == EVENT_CAST_SPELL && g_affected_card == card && g_affected_card_controller == player)
  {
    if (g_duel_ai_mode_state != 1)
    {
      load_text("prompts.txt", "ANIMATE_WALL");
    }
    if (C_real_select_target(player,
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
                             &target))
    {
      SET_TARGET(PLAYER_CARD_INSTANCE(player, card).targets[0], target);
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
    }
    else
    {
      g_spell_fizzled = 1;
    }
    return 0;
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    if (C_real_validate_target(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                               PLAYER_CARD_INSTANCE(player, card).targets[0].card,
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
      PLAYER_CARD_INSTANCE(player, card).damage_target_player = (char)PLAYER_CARD_INSTANCE(player, card).targets[0].player;
      PLAYER_CARD_INSTANCE(player, card).damage_target_card = PLAYER_CARD_INSTANCE(player, card).targets[0].card;
    }
    else
    {
      kill_card(player, card, KILL_BURY);
      g_spell_fizzled = 1;
    }
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    return 0;
  }

  if (event == EVENT_ABILITIES && PLAYER_CARD_INSTANCE(player, card).damage_target_card == g_affected_card && PLAYER_CARD_INSTANCE(player, card).damage_target_player == g_affected_card_controller && g_affected_card != -1 && (PLAYER_CARD_INSTANCE(player, card).token_status & STATUS_DYING) == 0)
  {
    DAMAGE_TARGET_CARD_INSTANCE(player, card).token_status |= 0x800;
  }

  if (event == EVENT_GRAVEYARD_FROM_PLAY && g_affected_card == card && g_affected_card_controller == player && PLAYER_CARD_INSTANCE(player, card).damage_target_card != -1)
  {
    DAMAGE_TARGET_CARD_INSTANCE(player, card).token_status &= ~0x800;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0051baf5
// FUNCTION: SHANDALAR 0x004d32db
int card_control_magic(int player, int card, event_t event)
{
  if (event == EVENT_CAST_SPELL && g_affected_card == card && g_affected_card_controller == player && g_duel_ai_mode_state != 1)
  {
    load_text("prompts.txt", "CONTROL_MAGIC");
  }

  return generic_control_aura(player, card, event, TYPE_CREATURE);
}

// FUNCTION: MAGIC 0x0051bb62
// FUNCTION: SHANDALAR 0x004d3348
int card_steal_artifact(int player, int card, event_t event)
{
  if (event != EVENT_CHANGE_TYPE && (PLAYER_CARD_INSTANCE(player, card).token_status & STATUS_DYING) == 0 && PLAYER_CARD_INSTANCE(player, card).damage_target_player != -1 && (global_cards_data[DAMAGE_TARGET_CARD_INSTANCE(player, card).internal_card_id].type & TYPE_ARTIFACT) == 0)
  {
    kill_card(player, card, KILL_DESTROY);
  }

  if (event == EVENT_CAST_SPELL && g_affected_card == card && g_affected_card_controller == player && g_duel_ai_mode_state != 1)
  {
    load_text("prompts.txt", "STEAL_ARTIFACT");
  }

  return generic_control_aura(player, card, event, TYPE_ARTIFACT);
}

// FUNCTION: MAGIC 0x0051bcf0
// FUNCTION: SHANDALAR 0x004d34d6
int generic_control_aura(int player, int card, event_t event, unsigned int required_type)
{
  struct
  {
    target_t target;
    int new_card;
  } s;

  if (event == EVENT_CAN_CAST)
  {
    return real_target_available((int *)0, TARGET_SCAN_DIRECT, player, 2, 2, 0x200, required_type, 0, 0,
                                 get_protections_from(player, card), 0, 0, -1, -1, -1, -1, 0, 0, 0);
  }

  if (event == EVENT_CAST_SPELL && g_affected_card == card && g_affected_card_controller == player)
  {
    if (C_real_select_target(player, 2, 1 - player, TARGET_ZONE_IN_PLAY, required_type, TYPE_NONE, 0,
                             get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0, -1,
                             ~SUB_WALL, -1, -1, 0, 0, 0, g_text_lines[0], 1, &s.target))
    {
      SET_TARGET(PLAYER_CARD_INSTANCE(player, card).targets[0], s.target);
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
    }
    else
    {
      g_spell_fizzled = 1;
    }

    return 0;
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    if (C_real_validate_target(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                               PLAYER_CARD_INSTANCE(player, card).targets[0].card,
                               (char *)0,
                               player, 2, 2, TARGET_ZONE_IN_PLAY, required_type, TYPE_NONE, 0,
                               get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0, -1,
                               ~SUB_WALL, -1, -1, 0, 0, 0))
    {
      PLAYER_CARD_INSTANCE(player, card).damage_target_player = PLAYER_CARD_INSTANCE(player, card).targets[0].player;
      PLAYER_CARD_INSTANCE(player, card).damage_target_card = PLAYER_CARD_INSTANCE(player, card).targets[0].card;
      if (resolve_control_aura_conflict(player, card))
      {
        PLAYER_CARD_INSTANCE(player, card).token_status |= 0x1000000;
        if (PLAYER_CARD_INSTANCE(player, card).targets[0].player != player)
        {
          if (g_duel_ai_mode_state != 1)
          {
            play_sound_effect(WAV_CONTROL);
          }
          s.new_card = gain_control((int)(char)PLAYER_CARD_INSTANCE(player, card).damage_target_player, PLAYER_CARD_INSTANCE(player, card).damage_target_card);
          PLAYER_CARD_INSTANCE(player, card).damage_target_player = player;
          PLAYER_CARD_INSTANCE(player, card).damage_target_card = s.new_card;
        }
      }
    }
    else
    {
      kill_card(player, card, KILL_BURY);
      g_spell_fizzled = 1;
    }

    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    return 0;
  }

  if (g_trigger_condition == (trigger_t)0xd4 && g_affected_card == card && g_affected_card_controller == player &&
      PLAYER_CARD_INSTANCE(player, card).damage_target_player != -1 &&
      DAMAGE_TARGET_CARD_INSTANCE(player, card).internal_card_id != -1 &&
      g_trigger_cause_controller == player && g_trigger_cause == card && g_current_turn == player)
  {
    if (event == (event_t)0x7d)
    {
      g_event_result |= 2;
    }

    if (event == (event_t)0x7e)
    {
      if ((PLAYER_CARD_INSTANCE(player, card).token_status & STATUS_CONTROLLED) != 0)
      {
        if (PLAYER_CARD_INSTANCE(player, card).damage_source_player == -1)
        {
          if (DAMAGE_TARGET_CARD_INSTANCE(player, card).internal_card_id != -1 &&
              ((((DAMAGE_TARGET_CARD_INSTANCE(player, card).state &
                  0x400000) != 0) &&
                PLAYER_CARD_INSTANCE(player, card).damage_target_player == g_active_player) ||
               (((DAMAGE_TARGET_CARD_INSTANCE(player, card).state &
                  0x400000) == 0) &&
                PLAYER_CARD_INSTANCE(player, card).damage_target_player == g_other_player)))
          {
            gain_control((int)(char)PLAYER_CARD_INSTANCE(player, card).damage_target_player, PLAYER_CARD_INSTANCE(player, card).damage_target_card);
          }
        }
        else
        {
          DAMAGE_SOURCE_CARD_INSTANCE(player, card).token_status |= 0x1000000;
          if (PLAYER_CARD_INSTANCE(player, card).damage_source_player != PLAYER_CARD_INSTANCE(player, card).damage_target_player)
          {
            gain_control((int)(char)PLAYER_CARD_INSTANCE(player, card).damage_target_player, PLAYER_CARD_INSTANCE(player, card).damage_target_card);
          }
        }
      }
      else
      {
        dispatch_three_arg_callback_to_cards_in_play(rewire_control_aura_source, -1);
      }
    }
  }

  return 0;
}
// FUNCTION: MAGIC 0x0051c73d
// FUNCTION: SHANDALAR 0x004d3f22
int rewire_control_aura_source(int player, int card, int internal_card_id)
{
  if (((global_cards_data[internal_card_id].id == CARD_ID_CONTROL_MAGIC) ||
       (global_cards_data[internal_card_id].id == CARD_ID_STEAL_ARTIFACT) ||
       (internal_card_id == g_control_aura_special_internal_card_id)) &&
      ((int)(char)PLAYER_CARD_INSTANCE(player, card).damage_source_player == g_affected_card_controller) &&
      (PLAYER_CARD_INSTANCE(player, card).damage_source_card == g_affected_card))
  {
    PLAYER_CARD_INSTANCE(player, card).damage_source_player =
        PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).damage_source_player;
    PLAYER_CARD_INSTANCE(player, card).damage_source_card =
        PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).damage_source_card;
  }
  return 0;
}

// FUNCTION: MAGIC 0x0051c8c0
// FUNCTION: SHANDALAR 0x004d40a4
int gain_control(int player, int card)
{
  struct
  {
    int target_index;   /* [ebp-0x18] */
    int test_player;    /* [ebp-0x14] */
    int test_card;      /* [ebp-0x10] */
    int timestamp_slot; /* [ebp-0x0c] */
    int new_card;       /* [ebp-0x08] */
    int opponent;       /* [ebp-0x04] */
  } s;

  s.opponent = 1 - player;
  s.timestamp_slot = PLAYER_CARD_INSTANCE(player, card).timestamp;
  s.new_card = add_card_to_hand(s.opponent, PLAYER_CARD_INSTANCE(player, card).internal_card_id);
  if (s.new_card != -1)
  {
    memcpy(&PLAYER_CARD_INSTANCE(s.opponent, s.new_card), &PLAYER_CARD_INSTANCE(player, card), 0x12c);
    PLAYER_CARD_INSTANCE(s.opponent, s.new_card).state |= 0x30000;
    PLAYER_CARD_INSTANCE(s.opponent, s.new_card).state &= ~(STATE_ATTACKING | STATE_BLOCKING);
    TENTATIVE_timestamp_player[s.timestamp_slot] = s.opponent;
    TENTATIVE_timestamp_card[s.timestamp_slot] = s.new_card;

    for (s.test_player = 0; s.test_player < 2; ++s.test_player)
    {
      for (s.test_card = 0; s.test_card < g_active_cards_count[s.test_player]; ++s.test_card)
      {
        if ((int)(char)PLAYER_CARD_INSTANCE(s.test_player, s.test_card).damage_target_player == player &&
            PLAYER_CARD_INSTANCE(s.test_player, s.test_card).damage_target_card == card)
        {
          PLAYER_CARD_INSTANCE(s.test_player, s.test_card).damage_target_player = (char)s.opponent;
          PLAYER_CARD_INSTANCE(s.test_player, s.test_card).damage_target_card = s.new_card;
        }
        if ((int)(char)PLAYER_CARD_INSTANCE(s.test_player, s.test_card).damage_source_player == player &&
            PLAYER_CARD_INSTANCE(s.test_player, s.test_card).damage_source_card == card)
        {
          PLAYER_CARD_INSTANCE(s.test_player, s.test_card).damage_source_player = (char)s.opponent;
          PLAYER_CARD_INSTANCE(s.test_player, s.test_card).damage_source_card = s.new_card;
        }
        if (PLAYER_CARD_INSTANCE(s.test_player, s.test_card).number_of_targets != 0)
        {
          for (s.target_index = 0; s.target_index < PLAYER_CARD_INSTANCE(s.test_player, s.test_card).number_of_targets; ++s.target_index)
          {
            if (PLAYER_CARD_INSTANCE(s.test_player, s.test_card).targets[s.target_index].player == player &&
                PLAYER_CARD_INSTANCE(s.test_player, s.test_card).targets[s.target_index].card == card)
            {
              PLAYER_CARD_INSTANCE(s.test_player, s.test_card).targets[s.target_index].player = s.opponent;
              PLAYER_CARD_INSTANCE(s.test_player, s.test_card).targets[s.target_index].card = s.new_card;
            }
          }
        }
      }
    }
  }

  PLAYER_CARD_INSTANCE(player, card).token_status |= 8;
  kill_card(player, card, KILL_REMOVE);
  if ((g_battlefield_extra_ability_flags & 0x10000000) != 0)
  {
    dispatch_event(s.opponent, s.new_card, EVENT_CARDCONTROLLED);
  }
  return s.new_card;
}

// FUNCTION: MAGIC 0x0051d83c
// FUNCTION: SHANDALAR 0x004d5020
int card_feedback(int player, int card, event_t event)
{
  target_t selected_target;

  if ((event == EVENT_SHOULD_AI_PLAY) && is_in_play(player, card) && (PLAYER_CARD_INSTANCE(player, card).damage_target_player != -1))
  {
    if (PLAYER_CARD_INSTANCE(player, card).damage_target_player == g_active_player)
    {
      g_ai_modifier += MAX(g_life[PLAYER_CARD_INSTANCE(player, card).damage_target_player], 1) * 0x18;
    }
    else
    {
      g_ai_modifier -= MAX(g_life[PLAYER_CARD_INSTANCE(player, card).damage_target_player], 1) * 0x18;
    }
  }

  if (event == EVENT_CAN_CAST)
  {
    return real_target_available((int *)0,
                                 TARGET_SCAN_DIRECT,
                                 player,
                                 2,
                                 2,
                                 0x200,
                                 4,
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
  if (((event == EVENT_CAST_SPELL) && (card == g_affected_card)) && (player == g_affected_card_controller))
  {
    if (g_duel_ai_mode_state != 1)
    {
      load_text("prompts.txt", "FEEDBACK");
    }
    if (C_real_select_target(player,
                             2,
                             1 - player,
                             TARGET_ZONE_IN_PLAY,
                             TYPE_ENCHANTMENT,
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
                             0,
                             0,
                             g_text_lines[0],
                             1,
                             &selected_target))
    {
      if (PLAYER_CARD_INSTANCE(player, card).targets[0].player == g_active_player)
      {
        g_ai_modifier += 0x30;
      }
      if (PLAYER_CARD_INSTANCE(player, card).targets[0].player == g_other_player)
      {
        g_ai_modifier += -0x60;
      }
      SET_TARGET(PLAYER_CARD_INSTANCE(player, card).targets[0], selected_target);
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
    }
    else
    {
      g_spell_fizzled = 1;
    }
    return 0;
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
                               TYPE_ENCHANTMENT,
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
      PLAYER_CARD_INSTANCE(player, card).damage_target_player =
          (char)PLAYER_CARD_INSTANCE(player, card).targets[0].player;
      PLAYER_CARD_INSTANCE(player, card).damage_target_card =
          PLAYER_CARD_INSTANCE(player, card).targets[0].card;
    }
    else
    {
      kill_card(player, card, KILL_BURY);
      g_spell_fizzled = 1;
    }
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    return 0;
  }
  if (event == EVENT_CAN_ACTIVATE)
  {
    if (((g_current_phase == EVENT_UPKEEP_PHASE) && (g_event_player == g_current_player)) && (PLAYER_CARD_INSTANCE(player, card).damage_target_player == g_current_player) && ((PLAYER_CARD_INSTANCE(player, card).info_slot & 1) == 0))
    {
      PLAYER_CARD_INSTANCE(player, card).upkeep_flags |= 0x101;
      g_activation_event_flags |= 3;
      return 1;
    }
    return 0;
  }
  if (((event == EVENT_UPKEEP_PHASE) && (g_affected_card == card)) && (g_affected_card_controller == player))
  {
    PLAYER_CARD_INSTANCE(player, card).info_slot |= 1;
    g_upkeep_payment_completed = 1;
    g_event_result |= 1;
    return 0;
  }
  if (event == EVENT_UPKEEP_COSTS_UNPAID)
  {
    damage_player((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                  1,
                  g_card_on_stack_controller,
                  g_card_on_stack);
    return 0;
  }
  if (event == EVENT_CLEANUP)
  {
    PLAYER_CARD_INSTANCE(player, card).info_slot &= ~STATE_JUST_DRAWED;
  }
  return 0;
}

// FUNCTION: MAGIC 0x0051df5d
// FUNCTION: SHANDALAR 0x004d5741
int card_brainwash(int player, int card, event_t event)
{
  target_t target;

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
                                 -1,
                                 -1,
                                 0,
                                 0,
                                 0);
  }

  if (event == EVENT_CAST_SPELL && card == g_affected_card && player == g_affected_card_controller)
  {
    if (g_duel_ai_mode_state != 1)
    {
      load_text("prompts.txt", "BRAINWASH");
    }
    if (C_real_select_target(player,
                             2,
                             1 - player,
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
                             0,
                             g_text_lines[0],
                             1,
                             &target))
    {
      SET_TARGET(PLAYER_CARD_INSTANCE(player, card).targets[0], target);
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
      if (target.player != player)
      {
        g_ai_modifier += (C_get_abilities(target.player, target.card, EVENT_POWER, -1) * 0xc) / 2;
      }
      else
      {
        g_ai_modifier -= (C_get_abilities(target.player, target.card, EVENT_POWER, -1) * 0xc) / 2;
      }
    }
    else
    {
      g_spell_fizzled = 1;
    }
    return 0;
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    if (C_real_validate_target(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                               PLAYER_CARD_INSTANCE(player, card).targets[0].card,
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
      PLAYER_CARD_INSTANCE(player, card).damage_target_player = PLAYER_CARD_INSTANCE(player, card).targets[0].player;
      PLAYER_CARD_INSTANCE(player, card).damage_target_card = PLAYER_CARD_INSTANCE(player, card).targets[0].card;
    }
    else
    {
      kill_card(player, card, KILL_BURY);
      g_spell_fizzled = 1;
    }
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    return 0;
  }

  if (g_trigger_condition == 0xdc &&
      g_current_phase == EVENT_DECLARE_ATTACKERS &&
      card == g_affected_card &&
      player == g_affected_card_controller &&
      g_current_turn == g_current_player &&
      PLAYER_CARD_INSTANCE(player, card).info_slot == 0 &&
      (int)(char)PLAYER_CARD_INSTANCE(player, card).damage_target_player == g_trigger_cause_controller &&
      PLAYER_CARD_INSTANCE(player, card).damage_target_card == g_trigger_cause)
  {
    if (has_mana(g_current_player, COLOR_ANY, 3))
    {
      if (event == EVENT_TRIGGER)
      {
        g_event_result |= 2;
      }
      if (event == EVENT_RESOLVE_TRIGGER)
      {
        push_card_onto_stack(player, card, EVENT_RESOLVE_TRIGGER, player, 0);
        charge_mana(g_current_player, COLOR_COLORLESS, 3);
        obliterate_top_card_of_stack();
        if (g_spell_fizzled == 1)
        {
          g_combat_assignment_cancelled = 1;
          g_spell_fizzled = 0;
        }
        else
        {
          PLAYER_CARD_INSTANCE(player, card).info_slot = 1;
        }
      }
    }
    else
    {
      g_combat_assignment_cancelled = 1;
    }
    if (g_combat_assignment_cancelled != 0)
    {
      DAMAGE_TARGET_CARD_INSTANCE(player, card).state &= ~0x8000;
    }
  }

  if (event == EVENT_ATTACK_LEGALITY && PLAYER_CARD_INSTANCE(player, card).info_slot == 0)
  {
    if (!has_mana(g_current_player, COLOR_ANY, 3))
    {
      g_event_result = 1;
    }
    return 0;
  }

  if (event == EVENT_CLEANUP || event == EVENT_SHOULD_AI_PLAY)
  {
    PLAYER_CARD_INSTANCE(player, card).info_slot = 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0051e631
int check_attached_aura_can_pay_cost(int player, int card, int internal_card_id)
{
  if ((PLAYER_CARD_INSTANCE(player, card).damage_target_card == g_affected_card) &&
      (PLAYER_CARD_INSTANCE(player, card).damage_target_player == g_affected_card_controller) && (g_affected_card != -1))
  {
    global_cards_data[internal_card_id].code_pointer(player, card, 0x79);
    if (g_spell_fizzled == 1)
    {
      g_event_result += 1;
      g_spell_fizzled = 0;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0051e6fc
// FUNCTION: SHANDALAR 0x004d5ee2
int card_spirit_shackle(int player, int card, event_t event)
{
  target_t target;

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
                                 -1,
                                 -1,
                                 0,
                                 0,
                                 0);
  }
  else if (event == EVENT_CAST_SPELL && card == g_affected_card && player == g_affected_card_controller)
  {
    if (g_duel_ai_mode_state != 1)
    {
      load_text("prompts.txt", "SPIRIT_SHACKLE");
    }
    if (C_real_select_target(player,
                             2,
                             1 - player,
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
                             0,
                             g_text_lines[0],
                             1,
                             &target))
    {
      SET_TARGET(PLAYER_CARD_INSTANCE(player, card).targets[0], target);
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
      if (player == g_other_player && (g_duel_network_flags & 2) == 0 &&
          TARGET_CARD_INSTANCE(player, card, 0).toughness < 5 &&
          is_selected_target_already_attached(player, card, PLAYER_CARD_INSTANCE(player, card).internal_card_id) != 0)
      {
        g_ai_modifier -= 0x30;
      }
      if (target.player == player)
      {
        g_ai_modifier -= PLAYER_CARD_INSTANCE(target.player, target.card).attack_rating / 2;
      }
      else
      {
        g_ai_modifier += PLAYER_CARD_INSTANCE(target.player, target.card).attack_rating / 2;
      }
    }
    else
    {
      g_spell_fizzled = 1;
    }
    return 0;
  }
  else if (event == EVENT_RESOLVE_SPELL)
  {
    if (C_real_validate_target(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                               PLAYER_CARD_INSTANCE(player, card).targets[0].card,
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
      PLAYER_CARD_INSTANCE(player, card).damage_target_player = PLAYER_CARD_INSTANCE(player, card).targets[0].player;
      PLAYER_CARD_INSTANCE(player, card).damage_target_card = PLAYER_CARD_INSTANCE(player, card).targets[0].card;
    }
    else
    {
      kill_card(player, card, KILL_BURY);
      g_spell_fizzled = 1;
    }
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    return 0;
  }
  else
  {
    if (event == EVENT_TAP_CARD &&
        PLAYER_CARD_INSTANCE(player, card).damage_target_card == g_affected_card &&
        PLAYER_CARD_INSTANCE(player, card).damage_target_player == g_affected_card_controller &&
        g_affected_card != -1)
    {
      spirit_shackle_add_counter(player, card);
    }
    return 0;
  }
}

// FUNCTION: MAGIC 0x0051ec86
// FUNCTION: SHANDALAR 0x004d646a
int spirit_shackle_add_counter(int player, int card)
{
  DAMAGE_TARGET_CARD_INSTANCE(player, card).special_counters += 0x100;
  if (g_duel_ai_mode_state != 1)
  {
    play_sound_effect(0x25);
  }
  DAMAGE_TARGET_CARD_INSTANCE(player, card).counter_toughness -= 2;

  return 0;
}

// FUNCTION: MAGIC 0x0051ee47
// FUNCTION: SHANDALAR 0x004d662b
int card_relic_bind(int player, int card, event_t event)
{
  struct
  {
    int preferred_player;
    target_t target;
    char message[600];
    int dialog_result;
  } s;

  if (PLAYER_CARD_INSTANCE(player, card).damage_target_player == player &&
      g_affected_card == card &&
      g_affected_card_controller == player &&
      PLAYER_CARD_INSTANCE(player, card).info_slot == 0)
  {
    ++PLAYER_CARD_INSTANCE(player, card).info_slot;
    kill_card(player, card, KILL_SACRIFICE);
  }

  if (event == EVENT_CAN_CAST)
  {
    return real_target_available((int *)0,
                                 TARGET_SCAN_DIRECT,
                                 player,
                                 1 - player,
                                 1 - player,
                                 TARGET_ZONE_IN_PLAY,
                                 TYPE_ARTIFACT,
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
  }

  if (event == EVENT_CAST_SPELL && g_affected_card == card && g_affected_card_controller == player)
  {
    if (g_duel_ai_mode_state != 1)
    {
      load_text("prompts.txt", "RELIC_BIND");
    }
    if (C_real_select_target(player,
                             1 - player,
                             1 - player,
                             TARGET_ZONE_IN_PLAY,
                             TYPE_ARTIFACT,
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
                             &s.target))
    {
      SET_TARGET(PLAYER_CARD_INSTANCE(player, card).targets[0], s.target);
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
      if ((global_cards_data[PLAYER_CARD_INSTANCE(s.target.player, s.target.card).internal_card_id].extra_ability & 1) != 0)
      {
        g_ai_modifier += ((global_cards_data[PLAYER_CARD_INSTANCE(s.target.player, s.target.card).internal_card_id].cc[1] + 2) * 24) / 2;
      }
    }
    else
    {
      g_spell_fizzled = 1;
    }
    return 0;
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    if (C_real_validate_target(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                               PLAYER_CARD_INSTANCE(player, card).targets[0].card,
                               (char *)0,
                               player,
                               1 - player,
                               1 - player,
                               TARGET_ZONE_IN_PLAY,
                               TYPE_ARTIFACT,
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
      PLAYER_CARD_INSTANCE(player, card).damage_target_player = PLAYER_CARD_INSTANCE(player, card).targets[0].player;
      PLAYER_CARD_INSTANCE(player, card).damage_target_card = PLAYER_CARD_INSTANCE(player, card).targets[0].card;
    }
    else
    {
      kill_card(player, card, KILL_BURY);
      g_spell_fizzled = 1;
    }
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    return 0;
  }

  if (event == EVENT_TAP_CARD &&
      PLAYER_CARD_INSTANCE(player, card).damage_target_card == g_affected_card &&
      PLAYER_CARD_INSTANCE(player, card).damage_target_player == g_affected_card_controller &&
      g_affected_card != -1)
  {
    if (g_duel_ai_mode_state != 1)
    {
      load_text("prompts.txt", "RELIC_BIND");
      sprintf(s.message, " %s\n %s", g_text_lines[2], g_text_lines[3]);
    }
    s.dialog_result = do_dialog(player, player, card, -1, -1, s.message, g_life[1 - player] <= g_life[player]);
    if (g_duel_ai_mode_state != 1)
    {
      load_text("prompts.txt", "RELIC_BIND");
    }
    if (g_life[player] >= g_life[1 - player])
    {
      s.preferred_player = 1 - player;
    }
    else
    {
      s.preferred_player = player;
    }
    C_real_select_target(player,
                         2,
                         s.preferred_player,
                         TARGET_ZONE_PLAYERS,
                         TYPE_NONE,
                         TYPE_NONE,
                         0,
                         0,
                         COLOR_TEST_0,
                         COLOR_TEST_0,
                         -1,
                         -1,
                         -1,
                         -1,
                         -1,
                         0,
                         0,
                         g_text_lines[1],
                         0,
                         &s.target);
    if (s.dialog_result != 0)
    {
      damage_player(s.target.player, 1, player, card);
    }
    else
    {
      gain_life(s.target.player, 1, player, card);
    }
  }
  return 0;
}

// FUNCTION: MAGIC 0x0051f540
// FUNCTION: SHANDALAR 0x004d6d23
int card_power_leak(int player, int card, event_t event)
{
  struct
  {
    target_t target;
    char message[900];
    int source_player;
    int damage;
    int unused;
    int dialog_result;
    int source_card;
  } s;

  if (event == EVENT_CAN_CAST)
  {
    return real_target_available((int *)0,
                                 TARGET_SCAN_DIRECT,
                                 player,
                                 2,
                                 2,
                                 TARGET_ZONE_IN_PLAY,
                                 TYPE_ENCHANTMENT,
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

  if (event == EVENT_CAST_SPELL && g_affected_card == card && g_affected_card_controller == player)
  {
    if (g_duel_ai_mode_state != 1)
    {
      load_text("prompts.txt", "POWERLEAK");
    }
    if (C_real_select_target(player,
                             2,
                             1 - player,
                             TARGET_ZONE_IN_PLAY,
                             TYPE_ENCHANTMENT,
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
                             &s.target))
    {
      SET_TARGET(PLAYER_CARD_INSTANCE(player, card).targets[0], s.target);
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
      if ((player == g_other_player) && (PLAYER_CARD_INSTANCE(player, card).targets[0].player == g_other_player))
      {
        g_ai_modifier -= 0x60;
      }
      g_ai_modifier += 0x30;
    }
    else
    {
      g_spell_fizzled = 1;
    }
    return 0;
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    if (C_real_validate_target(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                               PLAYER_CARD_INSTANCE(player, card).targets[0].card,
                               (char *)0,
                               player,
                               2,
                               2,
                               TARGET_ZONE_IN_PLAY,
                               TYPE_ENCHANTMENT,
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
      PLAYER_CARD_INSTANCE(player, card).damage_target_player =
          PLAYER_CARD_INSTANCE(player, card).targets[0].player;
      PLAYER_CARD_INSTANCE(player, card).damage_target_card =
          PLAYER_CARD_INSTANCE(player, card).targets[0].card;
    }
    else
    {
      kill_card(player, card, KILL_BURY);
      g_spell_fizzled = 1;
    }
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    return 0;
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    if (((g_current_phase == 4) && (g_event_player == g_current_player)) && (((int)(char)PLAYER_CARD_INSTANCE(player, card).damage_target_player == g_current_player) && ((PLAYER_CARD_INSTANCE(player, card).info_slot & 1) == 0)))
    {
      PLAYER_CARD_INSTANCE(player, card).upkeep_flags |= 0x101;
      g_activation_event_flags |= 3;
      return 1;
    }
    else
    {
      return 0;
    }
  }

  if (((event == EVENT_UPKEEP_PHASE) && (g_affected_card == card)) && (g_affected_card_controller == player))
  {
    PLAYER_CARD_INSTANCE(player, card).info_slot |= 1;
    g_upkeep_payment_completed = 1;
    g_event_result |= 1;
    return 0;
  }

  if (event == EVENT_UPKEEP_COSTS_UNPAID)
  {
    s.damage = has_mana((int)(char)PLAYER_CARD_INSTANCE(player, card).damage_target_player, COLOR_ANY, 1);
    if ((s.damage == 1) && (10 < g_life[(int)(char)PLAYER_CARD_INSTANCE(player, card).damage_target_player]))
    {
      s.damage = 0;
    }
    if (s.damage >= 2)
    {
      if (((s.damage > 7) || ((g_duel_summary.hand_counts[(int)(char)PLAYER_CARD_INSTANCE(player, card).damage_target_player]) <= 3) || (g_life[(int)(char)PLAYER_CARD_INSTANCE(player, card).damage_target_player] <= 7)))
      {
        s.damage = 2;
      }
      else
      {
        s.damage = 0;
      }
    }

    load_text("prompts.txt", "POWERLEAK");
    sprintf(s.message, " %s\n %s\n %s", g_text_lines[1], g_text_lines[2], g_text_lines[3]);
    s.dialog_result = do_dialog((int)(char)PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                                player,
                                card,
                                (int)(char)PLAYER_CARD_INSTANCE(player, card).damage_target_player, PLAYER_CARD_INSTANCE(player, card).damage_target_card,
                                s.message,
                                s.damage);
    s.source_player = g_card_on_stack_controller;
    s.source_card = g_card_on_stack;
    if (s.dialog_result == 0)
    {
      s.damage = 2;
    }
    else if (s.dialog_result == 1)
    {
      push_card_onto_stack(player, card, 0x7e, 0, 0);
      charge_mana((int)(char)PLAYER_CARD_INSTANCE(player, card).damage_target_player, 0, 1);
      obliterate_top_card_of_stack();
      if (g_spell_fizzled != 1)
      {
        s.damage = 1;
      }
      else
      {
        s.damage = 2;
      }
    }
    else
    {
      push_card_onto_stack(player, card, 0x7e, 0, 0);
      charge_mana((int)(char)PLAYER_CARD_INSTANCE(player, card).damage_target_player, 0, 2);
      obliterate_top_card_of_stack();
      if (g_spell_fizzled != 1)
      {
        s.damage = 0;
      }
      else
      {
        s.damage = 2;
      }
    }
    damage_player((int)(char)PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                  s.damage, s.source_player, s.source_card);
    g_spell_fizzled = -1;
    return 0;
  }

  if (event == EVENT_CLEANUP)
  {
    PLAYER_CARD_INSTANCE(player, card).info_slot &= ~1;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0051fe40
// FUNCTION: SHANDALAR 0x004d7623
int card_energy_flux(int player, int card, event_t event)
{
  int unused = 1 - player;

  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if (event == EVENT_CAST_SPELL && card == g_affected_card && player == g_affected_card_controller)
  {
    if (player == g_other_player)
    {
      if (g_duel_summary.artifact_counts[0] == 0)
      {
        g_ai_modifier -= 0xf0;
      }
      else
      {
        g_ai_modifier += (has_mana(g_other_player, COLOR_ANY, 1) / 2 +
                          (g_duel_summary.artifact_counts[0] - g_duel_summary.artifact_counts[1])) *
                         0x18;
      }
    }
    return 0;
  }

  if (event == EVENT_SETUP_UPKEEP_COSTS &&
      (global_cards_data[PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).internal_card_id].type & TYPE_ARTIFACT) != 0 &&
      g_affected_card_controller == g_event_player &&
      g_event_player == g_current_player)
  {
    PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).upkeep_flags |= 3;
    PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).upkeep_colorless += 2;
  }

  return 0;
}

// FUNCTION: MAGIC 0x00520013
// FUNCTION: SHANDALAR 0x004d77f8
int card_erosion(int player, int card, event_t event)
{
  struct
  {
    char message[900];
    int dialog_result;
    int can_pay_color;
    int default_choice;
    int done;
    int can_pay_any;
  } s;

  s.done = 0;

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
                                 -1,
                                 -1,
                                 0,
                                 0,
                                 0);
  }

  if (event == EVENT_CAST_SPELL && g_affected_card == card && g_affected_card_controller == player)
  {
    if (g_duel_ai_mode_state != 1)
    {
      load_text("prompts.txt", "EROSION");
    }
    g_spell_fizzled = (select_target_land_and_store(player, 1 - player, card) == 0);
    if (g_spell_fizzled != 1)
    {
      if (PLAYER_CARD_INSTANCE(player, card).targets[0].player == g_active_player)
      {
        g_ai_modifier += 0x20 -
                         g_basiclandtypes_controlled
                                 [g_active_player]
                                 [single_color_test_bit_to_color_t(
                                     (int)global_cards_data
                                         [TARGET_CARD_INSTANCE(player, card, 0).internal_card_id]
                                             .color)] *
                             4;
      }
      if (PLAYER_CARD_INSTANCE(player, card).targets[0].player == g_other_player)
      {
        g_ai_modifier -= 0x60;
      }
    }
    return 0;
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    if (C_real_validate_target(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                               PLAYER_CARD_INSTANCE(player, card).targets[0].card,
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
      PLAYER_CARD_INSTANCE(player, card).damage_target_player =
          PLAYER_CARD_INSTANCE(player, card).targets[0].player;
      PLAYER_CARD_INSTANCE(player, card).damage_target_card =
          PLAYER_CARD_INSTANCE(player, card).targets[0].card;
    }
    else
    {
      kill_card(player, card, KILL_BURY);
      g_spell_fizzled = 1;
    }
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    return 0;
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    if (g_current_phase == 4 &&
        g_event_player == g_current_player &&
        PLAYER_CARD_INSTANCE(player, card).damage_target_player == g_current_player &&
        (PLAYER_CARD_INSTANCE(player, card).info_slot & 1) == 0)
    {
      PLAYER_CARD_INSTANCE(player, card).upkeep_flags |= 0x101;
      g_activation_event_flags |= 3;
      return 1;
    }
    else
    {
      return 0;
    }
  }

  if (event == EVENT_UPKEEP_PHASE && g_affected_card == card && g_affected_card_controller == player)
  {
    PLAYER_CARD_INSTANCE(player, card).info_slot |= 1;
    g_upkeep_payment_completed = 1;
    g_event_result |= 1;
    return 0;
  }

  if (event == EVENT_UPKEEP_COSTS_UNPAID)
  {
    s.can_pay_color = has_mana((int)(char)PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                               single_color_test_bit_to_color_t(
                                   (int)(char)DAMAGE_TARGET_CARD_INSTANCE(player, card).mana_color),
                               1);
    s.can_pay_any = has_mana((int)(char)PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                             COLOR_ANY,
                             1);
    if (s.can_pay_color == 1)
    {
      if (s.can_pay_any < 4 && 10 < g_life[(int)(char)PLAYER_CARD_INSTANCE(player, card).damage_target_player])
      {
        s.default_choice = 2;
      }
      else
      {
        s.default_choice = 1;
      }
    }
    else if (s.can_pay_any < 3 && 15 < g_life[(int)(char)PLAYER_CARD_INSTANCE(player, card).damage_target_player])
    {
      s.default_choice = 2;
    }
    else
    {
      s.default_choice = 0;
    }

    while (!s.done)
    {
      load_text("prompts.txt", "EROSION");
      sprintf(s.message, " %s\n %s\n %s", g_text_lines[1], g_text_lines[2], g_text_lines[3]);
      s.dialog_result = do_dialog((int)(char)PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                                  player,
                                  card,
                                  (int)(char)PLAYER_CARD_INSTANCE(player, card).damage_target_player, PLAYER_CARD_INSTANCE(player, card).damage_target_card,
                                  s.message,
                                  s.default_choice);
      switch (s.dialog_result)
      {
      case 0:
        kill_card((int)(char)PLAYER_CARD_INSTANCE(player, card).damage_target_player, PLAYER_CARD_INSTANCE(player, card).damage_target_card,
                  KILL_DESTROY);
        ++s.done;
        break;

      case 1:
        if (has_mana((int)(char)PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                     COLOR_ANY,
                     1))
        {
          DAMAGE_TARGET_CARD_INSTANCE(player, card).state |=
              STATE_NO_AUTO_TAPPING;
          push_card_onto_stack(player, card, 0x7e, 0, 0);
          charge_mana((int)(char)PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                      COLOR_COLORLESS,
                      1);
          obliterate_top_card_of_stack();
          if (g_spell_fizzled != 1)
          {
            ++s.done;
          }
          else
          {
            g_spell_fizzled = 0;
          }
        }
        break;

      case 2:
        --g_life[(int)(char)PLAYER_CARD_INSTANCE(player, card).damage_target_player];
        ++s.done;
        break;
      }
    }
    return 0;
  }

  if (event == EVENT_CLEANUP)
  {
    PLAYER_CARD_INSTANCE(player, card).info_slot &= ~1;
  }
  return 0;
}

// FUNCTION: MAGIC 0x00520ab6
// FUNCTION: SHANDALAR 0x004d829b
int card_cursed_land(int player, int card, event_t event)
{
  if (event == EVENT_CAN_CAST)
  {
    return real_target_available((int *)0,
                                 TARGET_SCAN_DIRECT,
                                 player,
                                 2,
                                 2,
                                 0x200,
                                 1,
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

  if (((event == EVENT_CAST_SPELL) && (g_affected_card == card)) && (g_affected_card_controller == player))
  {
    if (g_duel_ai_mode_state != 1)
    {
      load_text("prompts.txt", "CURSED_LAND");
    }
    g_spell_fizzled = (unsigned int)(select_target_land_and_store(player, 1 - player, card) == 0);
    if (g_spell_fizzled != 1)
    {
      if (PLAYER_CARD_INSTANCE(player, card).targets[0].player == g_active_player)
      {
        g_ai_modifier += 0x30;
      }
      if (PLAYER_CARD_INSTANCE(player, card).targets[0].player == g_other_player)
      {
        g_ai_modifier += -0x60;
      }
    }
    return 0;
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
      PLAYER_CARD_INSTANCE(player, card).damage_target_player =
          (char)PLAYER_CARD_INSTANCE(player, card).targets[0].player;
      PLAYER_CARD_INSTANCE(player, card).damage_target_card =
          PLAYER_CARD_INSTANCE(player, card).targets[0].card;
    }
    else
    {
      kill_card(player, card, KILL_BURY);
      g_spell_fizzled = 1;
    }
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    return 0;
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    if (((g_current_phase == EVENT_UPKEEP_PHASE) && (g_event_player == g_current_player)) && (PLAYER_CARD_INSTANCE(player, card).damage_target_player == g_current_player) && ((PLAYER_CARD_INSTANCE(player, card).info_slot & 1) == 0))
    {
      PLAYER_CARD_INSTANCE(player, card).upkeep_flags |= 0x101;
      g_activation_event_flags |= 3;
      return 1;
    }
    return 0;
  }

  if (((event == EVENT_UPKEEP_PHASE) && (g_affected_card == card)) && (g_affected_card_controller == player))
  {
    PLAYER_CARD_INSTANCE(player, card).info_slot |= 1;
    g_upkeep_payment_completed = 1;
    g_event_result |= 1;
    return 0;
  }

  if (event == EVENT_UPKEEP_COSTS_UNPAID)
  {
    damage_player((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                  1,
                  g_card_on_stack_controller,
                  g_card_on_stack);
    return 0;
  }

  if (event == EVENT_CLEANUP)
  {
    PLAYER_CARD_INSTANCE(player, card).info_slot &= ~1;
    return 0;
  }

  if ((event == EVENT_SHOULD_AI_PLAY) && is_in_play(player, card) && (PLAYER_CARD_INSTANCE(player, card).damage_target_player != -1))
  {
    if (PLAYER_CARD_INSTANCE(player, card).damage_target_player == g_active_player)
    {
      g_ai_modifier += MAX(0x18 - g_life[PLAYER_CARD_INSTANCE(player, card).damage_target_player], 1) * 0x18;
    }
    else
    {
      g_ai_modifier -= MAX(0x18 - g_life[PLAYER_CARD_INSTANCE(player, card).damage_target_player], 1) * 0x18;
    }
  }
  return 0;
}

// FUNCTION: MAGIC 0x0052112c
// FUNCTION: SHANDALAR 0x004d8911
int card_karma(int player, int card, event_t event)
{
  int color;
  int damage;

  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }
  else if (event == EVENT_CAN_ACTIVATE)
  {
    if (((g_current_phase == EVENT_UPKEEP_PHASE) && ((PLAYER_CARD_INSTANCE(player, card).info_slot & 1) == 0)) && ((g_event_player == g_current_player) && ((color = get_hacked_color(player, card, 1),
                                                                                                                                                             g_basiclandtypes_controlled[g_event_player][color] != 0))))
    {
      PLAYER_CARD_INSTANCE(player, card).upkeep_flags |= 0x101;
      g_activation_event_flags |= 3;
      return 1;
    }
    return 0;
  }
  else if (((event == EVENT_UPKEEP_PHASE) && (g_affected_card == card)) && (g_affected_card_controller == player))
  {
    PLAYER_CARD_INSTANCE(player, card).info_slot |= 1;
    g_upkeep_payment_completed = 1;
    g_event_result |= 1;
    return 0;
  }
  else if (event == EVENT_UPKEEP_COSTS_UNPAID)
  {
    color = get_hacked_color(player, card, 1);
    damage_player(g_current_player,
                  g_basiclandtypes_controlled[g_current_player][color],
                  g_card_on_stack_controller,
                  g_card_on_stack);
    return 0;
  }
  else
  {
    if (event == EVENT_CLEANUP)
    {
      PLAYER_CARD_INSTANCE(player, card).info_slot &= ~STATE_JUST_DRAWED;
    }
    if (event == EVENT_SHOULD_AI_PLAY)
    {
      damage_player(1 - g_current_player,
                    g_basiclandtypes_controlled[1 - g_current_player][get_hacked_color(player, card, 1)],
                    player,
                    card);
      if (is_in_play(player, card))
      {
        color = get_hacked_color(player, card, 1);
        if (g_basiclandtypes_controlled[g_active_player][color] != 0)
        {
          damage = 0x18 - g_life[g_active_player] / g_basiclandtypes_controlled[g_active_player][color];
          if (damage < 2)
          {
            damage = 1;
          }
          g_ai_modifier += damage * 0x18;
        }
        color = get_hacked_color(player, card, 1);
        if (g_basiclandtypes_controlled[g_other_player][color] != 0)
        {
          damage = 0x18 - g_life[g_other_player] / g_basiclandtypes_controlled[g_other_player][color];
          if (damage < 2)
          {
            damage = 1;
          }
          g_ai_modifier -= damage * 0x18;
        }
      }
    }
    return 0;
  }
}

// FUNCTION: MAGIC 0x00521465
// FUNCTION: SHANDALAR 0x004d8c4a
int card_evil_presence(int player, int card, event_t event)
{
  if (event == EVENT_CAN_CAST)
  {
    return real_target_available((int *)0,
                                 TARGET_SCAN_DIRECT,
                                 player,
                                 2,
                                 2,
                                 0x200,
                                 1,
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

  if (((event == EVENT_CAST_SPELL) && (g_affected_card == card)) && (g_affected_card_controller == player))
  {
    if (g_duel_ai_mode_state != 1)
    {
      load_text("prompts.txt", "EVIL_PRESENCE");
    }
    g_spell_fizzled = (unsigned int)(select_target_land_and_store(player, 1 - player, card) == 0);
    if (g_spell_fizzled != 1)
    {
      if (PLAYER_CARD_INSTANCE(player, card).targets[0].player == g_active_player)
      {
        g_ai_modifier += 0x40 / (g_basiclandtypes_controlled[g_active_player][7] + 1);
      }
      if (PLAYER_CARD_INSTANCE(player, card).targets[0].player == g_other_player)
      {
        g_ai_modifier += -0x60;
      }
      if (TARGET_CARD_INSTANCE(player, card, 0).internal_card_id == 0)
      {
        g_ai_modifier += -0x90;
      }
    }
    return 0;
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
      PLAYER_CARD_INSTANCE(player, card).damage_target_player =
          (char)PLAYER_CARD_INSTANCE(player, card).targets[0].player;
      PLAYER_CARD_INSTANCE(player, card).damage_target_card =
          PLAYER_CARD_INSTANCE(player, card).targets[0].card;
      PLAYER_CARD_INSTANCE(player, card).info_slot = 1;

      DAMAGE_TARGET_CARD_INSTANCE(player, card).internal_card_id = get_hacked_color(player, card, PLAYER_CARD_INSTANCE(player, card).info_slot) - 1;
      DAMAGE_TARGET_CARD_INSTANCE(player, card).regen_status |= 0x1000000;
    }
    else
    {
      kill_card(player, card, KILL_BURY);
      g_spell_fizzled = 1;
    }
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    return 0;
  }

  if ((((event == EVENT_CHANGE_TYPE) && ((g_land_can_be_played & LCBP_DURING_EVENT_CHANGE_TYPE_SECOND_PASS) == 0)) && ((PLAYER_CARD_INSTANCE(player, card).damage_target_card == g_affected_card) && (((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player == g_affected_card_controller) && (g_affected_card != -1)))) && is_in_play(player, card))
  {
    g_event_result = get_hacked_color(player, card, PLAYER_CARD_INSTANCE(player, card).info_slot) - 1;
  }
  return 0;
}

// FUNCTION: MAGIC 0x00521abd
// FUNCTION: SHANDALAR 0x004d92a4
int card_living_artifact(int player, int card, event_t event)
{
  int ai_scale;

  if (event == EVENT_DEAL_DAMAGE && PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).internal_card_id == g_damage_card_internal_card_id)
  {
    if (PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).damage_target_player == player && PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).damage_target_card == -1 && PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).info_slot != 0)
    {
      PLAYER_CARD_INSTANCE(player, card).eot_toughness +=
          PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).info_slot;
    }
    return 0;
  }

  if (g_trigger_condition == TRIGGER_DEAL_DAMAGE && g_affected_card == card && g_affected_card_controller == player && PLAYER_CARD_INSTANCE(player, card).eot_toughness != 0 && g_current_turn == player)
  {
    if (event == EVENT_TRIGGER)
    {
      g_event_result |= 2;
    }
    if (event == EVENT_RESOLVE_TRIGGER)
    {
      add_special_counters(player, card, PLAYER_CARD_INSTANCE(player, card).eot_toughness);
      PLAYER_CARD_INSTANCE(player, card).eot_toughness = 0;
    }
  }

  if (event == EVENT_CAN_CAST)
  {
    return real_target_available((int *)0,
                                 TARGET_SCAN_DIRECT,
                                 player,
                                 2,
                                 2,
                                 TARGET_ZONE_IN_PLAY,
                                 TYPE_ARTIFACT,
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

  if (event == EVENT_CAST_SPELL && card == g_affected_card && player == g_affected_card_controller)
  {
    if (g_duel_ai_mode_state != 1)
    {
      load_text("prompts.txt", "LIVING_ARTIFACT");
    }

    g_spell_fizzled = select_target_artifact_and_store(player, 2, card) == 0;

    if (g_spell_fizzled != 1)
    {
      if (g_life[player] >= g_life[1 - player])
      {
        ai_scale = 1;
      }
      else
      {
        ai_scale = 3;
      }

      if (PLAYER_CARD_INSTANCE(player, card).targets[0].player == g_active_player)
      {
        g_ai_modifier += global_cards_data[TARGET_CARD_INSTANCE(player, card, 0).internal_card_id]
                             .cc[1] *
                         ai_scale * 0x18;
      }
      if (PLAYER_CARD_INSTANCE(player, card).targets[0].player == g_other_player)
      {
        g_ai_modifier += (ai_scale * 0x18) / 2;
      }
    }
    return 0;
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    if (C_real_validate_target(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                               PLAYER_CARD_INSTANCE(player, card).targets[0].card,
                               (char *)0,
                               player,
                               2,
                               2,
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
                               0) != 0)
    {
      PLAYER_CARD_INSTANCE(player, card).damage_target_player =
          PLAYER_CARD_INSTANCE(player, card).targets[0].player;
      PLAYER_CARD_INSTANCE(player, card).damage_target_card =
          PLAYER_CARD_INSTANCE(player, card).targets[0].card;
    }
    else
    {
      kill_card(player, card, KILL_BURY);
      g_spell_fizzled = 1;
    }
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    return 0;
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    if (g_current_phase == 4 && player == g_current_player && player == g_event_player && PLAYER_CARD_INSTANCE(player, card).info_slot == 0 && C_get_special_counters(player, card) != 0)
    {
      if (g_mana_cost_modifiers_by_color[single_color_test_bit_to_color_t(PLAYER_CARD_INSTANCE(player, card).color)] != 0 && has_mana_w_global_cost_mod(player, card, COLOR_ANY, 0) == 0)
      {
        return 0;
      }

      if (player == g_other_player && (g_duel_network_flags & 2) == 0)
      {
        g_activation_event_flags |= 3;
      }
      return 1;
    }
    else
    {
      return 0;
    }
  }

  if (event == EVENT_ACTIVATE && card == g_affected_card && player == g_affected_card_controller)
  {
    if (g_mana_cost_modifiers_by_color[single_color_test_bit_to_color_t(PLAYER_CARD_INSTANCE(player, card).color)] != 0)
    {
      charge_mana_w_global_cost_mod(player, card, 0, 0);
    }
    if (g_spell_fizzled != 1)
    {
      ++PLAYER_CARD_INSTANCE(player, card).info_slot;
      remove_special_counter(player, card);
    }
    return 0;
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    gain_life(player, 1, g_card_on_stack_controller, g_card_on_stack);
    return 0;
  }

  if (event == EVENT_CLEANUP || event == EVENT_SHOULD_AI_PLAY)
  {
    PLAYER_CARD_INSTANCE(player, card).info_slot = 0;
    PLAYER_CARD_INSTANCE(player, card).eot_toughness = 0;
  }
  return 0;
}

// FUNCTION: MAGIC 0x005223bf
// FUNCTION: SHANDALAR 0x004d9bac
int card_blight(int player, int card, event_t event)
{
  target_t target;
  int result;

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
                                 -1,
                                 -1,
                                 0,
                                 0,
                                 0);
  }

  if (event == EVENT_CAST_SPELL && g_affected_card == card && g_affected_card_controller == player)
  {
    if (g_duel_ai_mode_state != 1)
    {
      load_text("prompts.txt", "BLIGHT");
    }
    if (C_real_select_target(player,
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
      SET_TARGET(PLAYER_CARD_INSTANCE(player, card).targets[0], target);
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
      if (target.player == g_active_player)
      {
        g_ai_modifier += 0x60 / (g_basiclandtypes_controlled[g_active_player][single_color_test_bit_to_color_t((int)global_cards_data[PLAYER_CARD_INSTANCE(target.player, target.card).internal_card_id].color)] + 1);
      }
      if (target.player == g_other_player)
      {
        g_ai_modifier -= 0x18;
      }
    }
    else
    {
      g_spell_fizzled = 1;
    }
    return 0;
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    if (C_real_validate_target(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                               PLAYER_CARD_INSTANCE(player, card).targets[0].card,
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
      PLAYER_CARD_INSTANCE(player, card).damage_target_player = PLAYER_CARD_INSTANCE(player, card).targets[0].player;
      PLAYER_CARD_INSTANCE(player, card).damage_target_card = PLAYER_CARD_INSTANCE(player, card).targets[0].card;
      if (PLAYER_CARD_INSTANCE(player, card).targets[0].player == g_other_player && (g_duel_network_flags & 2) == 0)
      {
        TARGET_CARD_INSTANCE(player, card, 0).state |= STATE_NO_AUTO_TAPPING;
      }
    }
    else
    {
      kill_card(player, card, KILL_BURY);
      g_spell_fizzled = 1;
    }
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    return 0;
  }

  if (event == EVENT_TAP_CARD &&
      PLAYER_CARD_INSTANCE(player, card).damage_target_card == g_affected_card &&
      PLAYER_CARD_INSTANCE(player, card).damage_target_player == g_affected_card_controller &&
      g_affected_card != -1 &&
      (PLAYER_CARD_INSTANCE(player, card).state & STATE_INVISIBLE) == 0 &&
      (PLAYER_CARD_INSTANCE(player, card).info_slot & 2) == 0)
  {
    result = create_legacy_effect(player,
                                  card,
                                  g_duel_generated_internal_card_id_0f,
                                  (int)(char)PLAYER_CARD_INSTANCE(player, card).damage_target_player, PLAYER_CARD_INSTANCE(player, card).damage_target_card);
    if (result != -1)
    {
      PLAYER_CARD_INSTANCE(player, result).kill_code = 5;
    }
    PLAYER_CARD_INSTANCE(player, card).info_slot |= 2;
  }
  return 0;
}

// FUNCTION: MAGIC 0x00522a4f
// FUNCTION: SHANDALAR 0x004da23c
int card_psychic_venom(int player, int card, event_t event)
{
  target_t selected_target;

  if (event == EVENT_CAN_CAST)
  {
    return real_target_available((int *)0,
                                 TARGET_SCAN_DIRECT,
                                 player,
                                 2,
                                 2,
                                 0x200,
                                 1,
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

  if (((event == EVENT_CAST_SPELL) && (card == g_affected_card)) && (player == g_affected_card_controller))
  {
    if (g_duel_ai_mode_state != 1)
    {
      load_text("prompts.txt", "PSYCHIC_VENOM");
    }
    if (C_real_select_target(player,
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
                             ~SUB_WALL,
                             -1,
                             -1,
                             0,
                             0,
                             0,
                             g_text_lines[0],
                             1,
                             &selected_target) != 0)
    {
      SET_TARGET(PLAYER_CARD_INSTANCE(player, card).targets[0], selected_target);
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
      if (selected_target.player == g_active_player)
      {
        g_ai_modifier += 0x60 / (g_basiclandtypes_controlled[g_active_player][single_color_test_bit_to_color_t(
                                     global_cards_data[PLAYER_CARD_INSTANCE(selected_target.player, selected_target.card)
                                                           .internal_card_id]
                                         .color)] +
                                 1);
      }
      if (selected_target.player == g_other_player)
      {
        g_ai_modifier += -0x60;
      }
    }
    else
    {
      g_spell_fizzled = 1;
    }
    return 0;
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
      PLAYER_CARD_INSTANCE(player, card).damage_target_player =
          (char)PLAYER_CARD_INSTANCE(player, card).targets[0].player;
      PLAYER_CARD_INSTANCE(player, card).damage_target_card =
          PLAYER_CARD_INSTANCE(player, card).targets[0].card;
      if ((PLAYER_CARD_INSTANCE(player, card).targets[0].player == g_other_player) && ((g_duel_network_flags & 2) == 0))
      {
        TARGET_CARD_INSTANCE(player, card, 0).state |= 0x40000;
      }
    }
    else
    {
      kill_card(player, card, KILL_BURY);
      g_spell_fizzled = 1;
    }
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    return 0;
  }

  if (((event == EVENT_TAP_CARD) && (PLAYER_CARD_INSTANCE(player, card).damage_target_card == g_affected_card)) && (((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player == g_affected_card_controller) && (g_affected_card != -1)))
  {
    damage_player((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player, 2, player, card);
  }
  return 0;
}

// FUNCTION: MAGIC 0x00522fdc
// FUNCTION: SHANDALAR 0x004da7c9
int card_manabarbs(int player, int card, event_t event)
{
  if ((event == EVENT_TAP_CARD) && is_in_play(player, card))
  {
    if ((global_cards_data[PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).internal_card_id].type & TYPE_LAND) && g_produced_mana_color != -1)
    {
      damage_player(g_affected_card_controller, 1, player, card);
    }
    return 0;
  }

  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if (event == EVENT_CAST_SPELL && card == g_affected_card)
  {
    if (g_affected_card_controller == player)
    {
      g_ai_modifier += (g_basiclandtypes_controlled[g_other_player][7] - g_basiclandtypes_controlled[g_active_player][7]) * 0x18;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x005230ea
// FUNCTION: SHANDALAR 0x004da8d7
int card_mana_flare(int player, int card, event_t event)
{
  struct
  {
    int current_internal_id;
    int land_color;
    int color_index;
    int color_count;
  } s;

  if ((event == EVENT_COUNT_MANA) && (global_cards_data[PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).internal_card_id].type & TYPE_LAND))
  {
    if ((PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).state & STATE_TAPPED) == 0)
    {
      s.land_color = PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).mana_color;
      s.color_index = 0;
      s.color_count = 0;
      for (; s.color_index < 7; ++s.color_index)
      {
        if ((s.land_color & (1 << (unsigned char)s.color_index)) != 0)
        {
          ++s.color_count;
        }
      }

      if (s.color_count > 0)
      {
        declare_mana_available_hex(g_affected_card_controller, s.land_color, 1);
      }
      else
      {
        declare_mana_available(g_affected_card_controller, single_color_test_bit_to_color_t(s.land_color), 1);
      }
    }
    return 0;
  }

  if ((event == EVENT_TAP_CARD) && is_in_play(player, card))
  {
    if (PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).internal_card_id != -1)
    {
      s.current_internal_id = PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).internal_card_id;
    }
    else
    {
      s.current_internal_id = PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).original_internal_card_id;
    }

    if ((global_cards_data[s.current_internal_id].type & TYPE_LAND) != 0)
    {
      if (g_produced_mana_color != -1)
        produce_mana(g_affected_card_controller, g_produced_mana_color, 1);

      return 0;
    }
  }

  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if (event == EVENT_CAST_SPELL)
  {
    if (g_affected_card == card)
    {
      if (g_affected_card_controller == player)
      {
        g_ai_modifier += 0x30;
      }
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x00523399
// FUNCTION: SHANDALAR 0x004dab86
int card_lifetap(int player, int card, event_t event)
{
  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if ((event == EVENT_TAP_CARD) && (g_affected_card_controller != player))
  {
    if (card_has_basic_land_type(g_affected_card_controller, g_affected_card, get_hacked_color(player, card, COLOR_GREEN)) != 0)
    {
      gain_life(player, 1, player, card);
      return 0;
    }
  }
  if ((((event == EVENT_CAST_SPELL) || (event == EVENT_SHOULD_AI_PLAY)) && (g_affected_card == card)) && (g_affected_card_controller == player))
  {
    g_ai_modifier += ((g_basiclandtypes_controlled[g_active_player][get_hacked_color(player, card, COLOR_GREEN)] + 1) * 3) * 8;
  }
  return 0;
}

// FUNCTION: MAGIC 0x0052348b
// FUNCTION: SHANDALAR 0x004dac78
int card_fortified_area(int player, int card, event_t event)
{
  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if (event == EVENT_POWER &&
      player == g_affected_card_controller &&
      global_cards_data[PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).internal_card_id].subtype == 0 &&
      is_in_play(player, card))
  {
    if (is_in_play(g_affected_card_controller, g_affected_card))
    {
      ++g_event_result;
    }

    return 0;
  }

  if (event == EVENT_ABILITIES &&
      player == g_affected_card_controller &&
      global_cards_data[PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).internal_card_id].subtype == 0 &&
      is_in_play(g_affected_card_controller, g_affected_card))
  {
    g_event_result |= 0x40;
  }

  return 0;
}

// FUNCTION: MAGIC 0x005235d1
// FUNCTION: SHANDALAR 0x004dadbe
int card_sunken_city(int player, int card, event_t event)
{
  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if ((event == EVENT_POWER || event == EVENT_TOUGHNESS) && is_in_play(player, card) &&
      is_in_play(g_affected_card_controller, g_affected_card))
  {
    if (((1 << get_sleighted_color(player, card, COLOR_BLUE)) &
         (int)(char)PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).color) != 0)
    {
      ++g_event_result;
    }
    return 0;
  }

  if (event == EVENT_SETUP_UPKEEP_COSTS &&
      g_affected_card == card &&
      g_affected_card_controller == player &&
      player == g_current_player &&
      player == g_event_player)
  {
    PLAYER_CARD_INSTANCE(player, card).upkeep_flags |= 1;
    PLAYER_CARD_INSTANCE(player, card).upkeep_blue += 2;
    return 0;
  }

  if (event == EVENT_UPKEEP_COSTS_UNPAID)
  {
    kill_card(g_card_on_stack_controller, g_card_on_stack, KILL_BURY);
    return 0;
  }

  if (event == EVENT_SHOULD_AI_PLAY && g_basiclandtypes_controlled[player][COLOR_BLUE] < 2)
  {
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x005237dc
// FUNCTION: SHANDALAR 0x004dafcb
int card_bad_moon(int player, int card, event_t event)
{
  if (((event == EVENT_POWER) || (event == EVENT_TOUGHNESS)) && is_in_play(player, card) && is_in_play(g_affected_card_controller, g_affected_card) && (((1 << (char)get_sleighted_color(player, card, COLOR_BLACK)) & PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).color) != 0))
  {
    ++g_event_result;
  }

  if (event == EVENT_CAN_CAST)
    return 1;

  return 0;
}

// FUNCTION: MAGIC 0x005238a7
// FUNCTION: SHANDALAR 0x004db096
int card_orcish_oriflamme(int player, int card, event_t event)
{
  if (event == EVENT_POWER)
  {
    if (((PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).state & STATE_ATTACKING) != 0) && (player == g_current_player) && (player == g_affected_card_controller) && is_in_play(player, card))
    {
      ++g_event_result;
    }
    return 0;
  }

  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if (((event == EVENT_CAST_SPELL) && (card == g_affected_card)) && (player == g_affected_card_controller))
  {
    g_ai_modifier += g_duel_summary.creature_counts[player] * 0xc;
  }
  return 0;
}

// FUNCTION: MAGIC 0x0052398e
// FUNCTION: SHANDALAR 0x004db180
int card_crusade(int player, int card, event_t event)
{
  if ((event == EVENT_POWER || event == EVENT_TOUGHNESS) && is_in_play(player, card))
  {
    if (is_in_play(g_affected_card_controller, g_affected_card))
    {
      if ((1 << (((unsigned char)get_sleighted_color(player, card, 5))) & (int)PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).color) != 0)
      {
        ++g_event_result;
      }
    }
    return 0;
  }

  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  return 0;
}

// FUNCTION: MAGIC 0x00523a60
// FUNCTION: SHANDALAR 0x004db252
int card_aspect_of_wolf(int player, int card, event_t event)
{
  if (event == EVENT_CAN_CAST)
  {
    return real_target_available((int *)0,
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
                                 0,
                                 0);
  }

  if (((event == EVENT_CAST_SPELL) && (g_affected_card == card)) && (g_affected_card_controller == player))
  {
    if (g_duel_ai_mode_state != 1)
    {
      load_text("prompts.txt", "ASPECTOFWOLF");
    }
    if (select_target_creature_and_store(player, player, card) == 0)
    {
      g_spell_fizzled = 1;
    }
    else
    {
      g_spell_fizzled = 0;
    }

    if ((g_spell_fizzled != 1) && (PLAYER_CARD_INSTANCE(player, card).targets[0].player == g_active_player))
    {
      g_ai_modifier += -0x18;
    }
    return 0;
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
                               0))
    {
      PLAYER_CARD_INSTANCE(player, card).damage_target_player =
          (char)PLAYER_CARD_INSTANCE(player, card).targets[0].player;
      PLAYER_CARD_INSTANCE(player, card).damage_target_card =
          PLAYER_CARD_INSTANCE(player, card).targets[0].card;
    }
    else
    {
      kill_card(player, card, KILL_BURY);
      g_spell_fizzled = 1;
    }
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    return 0;
  }

  if (((event == EVENT_POWER) && (PLAYER_CARD_INSTANCE(player, card).damage_target_card == g_affected_card)) && (((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player == g_affected_card_controller) && ((g_affected_card != -1) && is_in_play(player, card))))
  {
    g_event_result += g_basiclandtypes_controlled[player][get_hacked_color(player, card, 3)] / 2;
  }
  if (((event == EVENT_TOUGHNESS) && (PLAYER_CARD_INSTANCE(player, card).damage_target_card == g_affected_card)) && (((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player == g_affected_card_controller) && ((g_affected_card != -1) && is_in_play(player, card))))
  {
    g_event_result += (g_basiclandtypes_controlled[player][get_hacked_color(player, card, 3)] + 1) / 2;
  }
  return 0;
}

// FUNCTION: MAGIC 0x00523ef2
// FUNCTION: SHANDALAR 0x004db6e2
int card_lure(int player, int card, event_t event)
{
  int blocking_card;

  if (event == EVENT_CAN_CAST)
  {
    return real_target_available((int *)0,
                                 TARGET_SCAN_DIRECT,
                                 player,
                                 2,
                                 2,
                                 0x200,
                                 TYPE_CREATURE,
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

  if (event == EVENT_CAST_SPELL && g_affected_card == card && g_affected_card_controller == player)
  {
    if (g_duel_ai_mode_state != 1)
    {
      load_text("prompts.txt", "LURE");
    }
    if (select_target_creature_and_store(player, player, card) == 0)
    {
      g_spell_fizzled = 1;
    }
    else
    {
      g_spell_fizzled = 0;
    }
    return 0;
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    if (C_real_validate_target(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                               PLAYER_CARD_INSTANCE(player, card).targets[0].card,
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
                               0) != 0)
    {
      PLAYER_CARD_INSTANCE(player, card).damage_target_player =
          (char)PLAYER_CARD_INSTANCE(player, card).targets[0].player;
      PLAYER_CARD_INSTANCE(player, card).damage_target_card =
          PLAYER_CARD_INSTANCE(player, card).targets[0].card;
    }
    else
    {
      kill_card(player, card, KILL_BURY);
      g_spell_fizzled = 1;
    }
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    PLAYER_CARD_INSTANCE(player, card).info_slot = 0;
    return 0;
  }

  if (g_trigger_condition == 0xda && PLAYER_CARD_INSTANCE(player, card).info_slot == 0)
  {
    PLAYER_CARD_INSTANCE(player, card).info_slot = 1;
    g_trigger_condition = -1;
    if ((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player == g_current_player && (DAMAGE_TARGET_CARD_INSTANCE(player, card).state & STATE_ATTACKING) != 0 && is_in_play(g_affected_card_controller, g_affected_card) && PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).blocking == -1 && g_affected_card_controller != g_current_player && can_block_with_landwalk_masks(g_affected_card_controller, g_affected_card, (int)(char)PLAYER_CARD_INSTANCE(player, card).damage_target_player, PLAYER_CARD_INSTANCE(player, card).damage_target_card) != 0)
    {
      if (DAMAGE_TARGET_CARD_INSTANCE(player, card).blocking == -1)
      {
        blocking_card = PLAYER_CARD_INSTANCE(player, card).damage_target_card;
      }
      else
      {
        blocking_card = DAMAGE_TARGET_CARD_INSTANCE(player, card).blocking;
      }

      if (event == EVENT_TRIGGER)
      {
        g_event_result |= 2;
      }
      if (event == EVENT_RESOLVE_TRIGGER)
      {
        PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).blocking = (unsigned char)blocking_card;
        PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).state |= 0x8008;
      }
    }
    g_trigger_condition = 0xda;
    PLAYER_CARD_INSTANCE(player, card).info_slot = 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0052460c
// FUNCTION: SHANDALAR 0x004dbdfc
int can_block_with_landwalk_masks(int blocker_player, int blocker_card, int attacker_player, int attacker_card)
{
  struct
  {
    int result;             /* [ebp-0x14] */
    int attacker_abilities; /* [ebp-0x10] */
    unsigned int selected;  /* [ebp-0x0c] */
    unsigned int tmp2;      /* [ebp-0x08] */
    unsigned int tmp1;      /* [ebp-0x04] */
  } s;

  s.attacker_abilities = C_get_abilities(attacker_player, attacker_card, 0x34, -1);
  get_landwalk_evasion_masks(&s.tmp1, &s.tmp2);

  if (blocker_player == 1)
  {
    s.selected = s.tmp1;
  }
  else
  {
    s.selected = s.tmp2;
  }

  s.result = can_block_attacker_with_abilities(blocker_player,
                                               blocker_card,
                                               attacker_player,
                                               attacker_card,
                                               (unsigned int)s.attacker_abilities,
                                               s.selected);
  return s.result;
}

// FUNCTION: MAGIC 0x00524687
// FUNCTION: SHANDALAR 0x004dbe77
int card_spirit_link(int player, int card, event_t event)
{
  int current_damage;

  if (event == EVENT_CAN_CAST)
  {
    return real_target_available((int *)0, TARGET_SCAN_DIRECT, player, 2, 2,
                                 TARGET_ZONE_IN_PLAY, TYPE_CREATURE, TYPE_NONE, 0,
                                 get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0,
                                 -1, -1, -1, -1, 0, 0, 0);
  }

  if (event == EVENT_CAST_SPELL && g_affected_card == card && g_affected_card_controller == player)
  {
    if (g_duel_ai_mode_state != 1)
    {
      load_text("prompts.txt", "SPIRITLINK");
    }
    g_spell_fizzled = (select_target_creature_and_store(player, 2, card) == 0);
    if (g_spell_fizzled != 1)
    {
      g_ai_modifier += C_get_abilities(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                                       PLAYER_CARD_INSTANCE(player, card).targets[0].card,
                                       EVENT_POWER, -1) *
                       0x18;
    }
    return 0;
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    if (C_real_validate_target(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                               PLAYER_CARD_INSTANCE(player, card).targets[0].card,
                               (char *)0, player, 2, 2, TARGET_ZONE_IN_PLAY,
                               TYPE_CREATURE, TYPE_NONE, 0, get_protections_from(player, card),
                               COLOR_TEST_0, COLOR_TEST_0, -1, -1, -1, -1, 0, 0, 0))
    {
      PLAYER_CARD_INSTANCE(player, card).damage_target_player = (char)PLAYER_CARD_INSTANCE(player, card).targets[0].player;
      PLAYER_CARD_INSTANCE(player, card).damage_target_card = PLAYER_CARD_INSTANCE(player, card).targets[0].card;
    }
    else
    {
      kill_card(player, card, KILL_BURY);
      g_spell_fizzled = 1;
    }
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    return 0;
  }

  if (event == EVENT_DEAL_DAMAGE &&
      PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).internal_card_id == g_damage_card_internal_card_id)
  {
    if (PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).damage_source_player == PLAYER_CARD_INSTANCE(player, card).damage_target_player &&
        PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).damage_source_card == PLAYER_CARD_INSTANCE(player, card).damage_target_card &&
        PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).info_slot != 0)
    {
      PLAYER_CARD_INSTANCE(player, card).targets[PLAYER_CARD_INSTANCE(player, card).info_slot].player = g_affected_card_controller;
      PLAYER_CARD_INSTANCE(player, card).targets[PLAYER_CARD_INSTANCE(player, card).info_slot].card = g_affected_card;
      ++PLAYER_CARD_INSTANCE(player, card).info_slot;
    }
    return 0;
  }

  if (g_trigger_condition == TRIGGER_DEAL_DAMAGE && g_affected_card == card &&
      g_affected_card_controller == player && PLAYER_CARD_INSTANCE(player, card).info_slot != 0 && g_current_turn == player)
  {
    if (event == EVENT_TRIGGER)
    {
      g_event_result |= RESOLVE_TRIGGER_MANDATORY;
    }
    if (event == EVENT_RESOLVE_TRIGGER)
    {
      for (current_damage = 0; current_damage < PLAYER_CARD_INSTANCE(player, card).info_slot; ++current_damage)
      {
        gain_life(player,
                  TARGET_CARD_INSTANCE(player, card, current_damage).info_slot,
                  player,
                  card);
      }
      PLAYER_CARD_INSTANCE(player, card).info_slot = 0;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x00524db3
// FUNCTION: SHANDALAR 0x004dc5a1
int card_creature_bond(int player, int card, event_t event)
{
  int effect_card;

  if (event == EVENT_CAN_CAST)
  {
    return real_target_available((int *)0,
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
                                 0,
                                 0);
  }

  if (((event == EVENT_CAST_SPELL) && (card == g_affected_card)) && (player == g_affected_card_controller))
  {
    if (g_duel_ai_mode_state != 1)
    {
      load_text("prompts.txt", "CREATUREBOND");
    }
    g_spell_fizzled = (unsigned int)(select_target_creature_and_store(player, 1 - player, card) == 0);
    if (g_spell_fizzled != 1)
    {
      if (PLAYER_CARD_INSTANCE(player, card).targets[0].player == g_active_player)
      {
        g_ai_modifier += 0x18;
      }
      if (PLAYER_CARD_INSTANCE(player, card).targets[0].player == g_other_player)
      {
        g_ai_modifier += -0x60;
      }
    }
    return 0;
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
                               0))
    {
      PLAYER_CARD_INSTANCE(player, card).damage_target_player =
          (char)PLAYER_CARD_INSTANCE(player, card).targets[0].player;
      PLAYER_CARD_INSTANCE(player, card).damage_target_card =
          PLAYER_CARD_INSTANCE(player, card).targets[0].card;
    }
    else
    {
      kill_card(player, card, KILL_BURY);
      g_spell_fizzled = 1;
    }
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    return 0;
  }

  if (((event == EVENT_GRAVEYARD_FROM_PLAY) && (PLAYER_CARD_INSTANCE(player, card).damage_target_card == g_affected_card)) && (((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player == g_affected_card_controller) && ((g_affected_card != -1) && ((effect_card = add_card_to_hand(player, g_duel_generated_internal_card_id_12)) != -1))))
  {
    PLAYER_CARD_INSTANCE(player, effect_card).original_internal_card_id =
        PLAYER_CARD_INSTANCE(player, card).internal_card_id;
    PLAYER_CARD_INSTANCE(player, effect_card).state |= STATE_IN_PLAY;
    *(int *)&PLAYER_CARD_INSTANCE(player, effect_card).display_pic_csv_id = 0x32;
    PLAYER_CARD_INSTANCE(player, effect_card).info_slot =
        C_get_abilities(g_affected_card_controller, g_affected_card, EVENT_TOUGHNESS, -1);
    PLAYER_CARD_INSTANCE(player, effect_card).damage_target_player = (char)g_affected_card_controller;
    TENTATIVE_set_timestamps(player, effect_card);
  }
  return 0;
}

// FUNCTION: MAGIC 0x005252e0
// FUNCTION: SHANDALAR 0x004dcace
int card_gaseous_form(int player, int card, event_t event)
{
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
                                 -1,
                                 -1,
                                 0,
                                 0,
                                 0);
  }

  if (event == EVENT_CAST_SPELL && g_affected_card == card && g_affected_card_controller == player)
  {
    if (g_duel_ai_mode_state != 1)
    {
      load_text("prompts.txt", "GASEOUSFORM");
    }
    g_spell_fizzled = (select_target_creature_and_store(player, 2, card) == 0);
    return 0;
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    if (C_real_validate_target(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                               PLAYER_CARD_INSTANCE(player, card).targets[0].card,
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
      PLAYER_CARD_INSTANCE(player, card).damage_target_player = PLAYER_CARD_INSTANCE(player, card).targets[0].player;
      PLAYER_CARD_INSTANCE(player, card).damage_target_card = PLAYER_CARD_INSTANCE(player, card).targets[0].card;
    }
    else
    {
      kill_card(player, card, KILL_BURY);
      g_spell_fizzled = 1;
    }
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    return 0;
  }

  if (event == EVENT_PREVENT_DAMAGE &&
      (g_current_phase == 0x1a || g_current_phase == 0x19) &&
      PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).internal_card_id == g_damage_card_internal_card_id)
  {
    if (PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).damage_target_player == PLAYER_CARD_INSTANCE(player, card).damage_target_player &&
        PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).damage_target_card == PLAYER_CARD_INSTANCE(player, card).damage_target_card)
    {
      PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).unknown0x37 = (char)PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).info_slot;
      PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).info_slot = 0;
    }
    if (PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).damage_source_player == PLAYER_CARD_INSTANCE(player, card).damage_target_player &&
        PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).damage_source_card == PLAYER_CARD_INSTANCE(player, card).damage_target_card)
    {
      PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).unknown0x37 = (char)PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).info_slot;
      PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).info_slot = 0;
    }
  }
  return 0;
}

// FUNCTION: MAGIC 0x005258c6
// FUNCTION: SHANDALAR 0x004dd0b4
int card_backfire(int player, int card, event_t event)
{
  int current_damage;
  int damage_card_id;
  int damage_player_id;

  if (event == EVENT_CAN_CAST)
  {
    return real_target_available((int *)0, TARGET_SCAN_DIRECT, player, 2, 2,
                                 TARGET_ZONE_IN_PLAY, TYPE_CREATURE, TYPE_NONE, 0,
                                 get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0,
                                 -1, -1, -1, -1, 0, 0, 0);
  }

  if (event == EVENT_CAST_SPELL && g_affected_card == card && g_affected_card_controller == player)
  {
    if (g_duel_ai_mode_state != 1)
    {
      load_text("prompts.txt", "BACKFIRE");
    }
    g_spell_fizzled = (select_target_creature_and_store(player, 1 - player, card) == 0);
    if (g_spell_fizzled != 1)
    {
      if (PLAYER_CARD_INSTANCE(player, card).targets[0].player == g_active_player)
      {
        g_ai_modifier += C_get_abilities(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                                         PLAYER_CARD_INSTANCE(player, card).targets[0].card,
                                         EVENT_POWER, -1) *
                         0xc;
      }
      else
      {
        g_ai_modifier -= 0x18;
      }
      PLAYER_CARD_INSTANCE(player, card).damage_source_player = (char)-1;
    }
    return 0;
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    if (C_real_validate_target(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                               PLAYER_CARD_INSTANCE(player, card).targets[0].card,
                               (char *)0, player, 2, 2, TARGET_ZONE_IN_PLAY,
                               TYPE_CREATURE, TYPE_NONE, 0, get_protections_from(player, card),
                               COLOR_TEST_0, COLOR_TEST_0, -1, -1, -1, -1, 0, 0, 0))
    {
      PLAYER_CARD_INSTANCE(player, card).damage_target_player = (char)PLAYER_CARD_INSTANCE(player, card).targets[0].player;
      PLAYER_CARD_INSTANCE(player, card).damage_target_card = PLAYER_CARD_INSTANCE(player, card).targets[0].card;
    }
    else
    {
      kill_card(player, card, KILL_BURY);
      g_spell_fizzled = 1;
    }
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    return 0;
  }

  if (event == EVENT_DEAL_DAMAGE &&
      PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).internal_card_id == g_damage_card_internal_card_id)
  {
    if (PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).damage_target_card == -1 &&
        PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).damage_target_player == player &&
        PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).damage_source_card == PLAYER_CARD_INSTANCE(player, card).damage_target_card &&
        PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).damage_source_player == PLAYER_CARD_INSTANCE(player, card).damage_target_player)
    {
      PLAYER_CARD_INSTANCE(player, card).targets[PLAYER_CARD_INSTANCE(player, card).info_slot].player = g_affected_card_controller;
      PLAYER_CARD_INSTANCE(player, card).targets[PLAYER_CARD_INSTANCE(player, card).info_slot].card = g_affected_card;
      ++PLAYER_CARD_INSTANCE(player, card).info_slot;
    }
    return 0;
  }

  if (g_trigger_condition == TRIGGER_DEAL_DAMAGE && g_affected_card == card &&
      g_affected_card_controller == player && PLAYER_CARD_INSTANCE(player, card).info_slot != 0 && player == g_current_turn)
  {
    if (event == EVENT_TRIGGER)
    {
      g_event_result |= RESOLVE_TRIGGER_MANDATORY;
    }
    if (event == EVENT_RESOLVE_TRIGGER)
    {
      for (current_damage = 0; current_damage < PLAYER_CARD_INSTANCE(player, card).info_slot; ++current_damage)
      {
        damage_player_id = PLAYER_CARD_INSTANCE(player, card).targets[current_damage].player;
        damage_card_id = PLAYER_CARD_INSTANCE(player, card).targets[current_damage].card;
        damage_player((int)PLAYER_CARD_INSTANCE(damage_player_id, damage_card_id).damage_source_player,
                      PLAYER_CARD_INSTANCE(damage_player_id, damage_card_id).info_slot,
                      player,
                      card);
      }
      PLAYER_CARD_INSTANCE(player, card).info_slot = 0;
    }
  }

  if (event == EVENT_ATTACK_RATING &&
      PLAYER_CARD_INSTANCE(player, card).damage_target_card == g_affected_card &&
      PLAYER_CARD_INSTANCE(player, card).damage_target_player == g_affected_card_controller &&
      g_affected_card != -1)
  {
    g_ai_score += DAMAGE_TARGET_CARD_INSTANCE(player, card).power *
                  0x18;
  }

  return 0;
}

// FUNCTION: MAGIC 0x00526202
// FUNCTION: SHANDALAR 0x004dd9ef
int card_holy_armor(int player, int card, event_t event)
{
  int local;

  if (event == EVENT_UNTAP_PHASE)
  {
    ++g_ai_mana_demand_by_color[player][COLOR_WHITE];
  }

  if (event == EVENT_CAN_CAST)
  {
    return real_target_available((int *)0,
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
                                 0,
                                 0);
  }

  if (((event == EVENT_CAST_SPELL) && (card == g_affected_card)) && (player == g_affected_card_controller))
  {
    g_ai_modifier += 0x18;
    PLAYER_CARD_INSTANCE(player, card).eot_toughness = 0;
    PLAYER_CARD_INSTANCE(player, card).info_slot = PLAYER_CARD_INSTANCE(player, card).eot_toughness;
    if (g_duel_ai_mode_state != 1)
    {
      load_text("prompts.txt", "HOLY_ARMOR");
    }
    if (select_target_creature_and_store(player, player, card) != 0)
    {
      if ((player == g_other_player) && ((g_duel_network_flags & 2) == 0))
      {
        if (is_selected_target_already_attached(player,
                                                card,
                                                PLAYER_CARD_INSTANCE(player, card).internal_card_id) != 0)
        {
          g_ai_modifier += -0x30;
        }
        if (PLAYER_CARD_INSTANCE(player, card).targets[0].player == player)
        {
          g_ai_modifier += 0xc;
        }
        else
        {
          g_ai_modifier += -0xc;
        }
      }
    }
    else
    {
      g_spell_fizzled = 1;
    }
    return 0;
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
                               0) != 0)
    {
      PLAYER_CARD_INSTANCE(player, card).damage_target_player =
          (char)PLAYER_CARD_INSTANCE(player, card).targets[0].player;
      PLAYER_CARD_INSTANCE(player, card).damage_target_card =
          PLAYER_CARD_INSTANCE(player, card).targets[0].card;
    }
    else
    {
      kill_card(player, card, KILL_BURY);
      g_spell_fizzled = 1;
    }
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    return 0;
  }

  if (event == EVENT_TOUGHNESS && is_in_play(player, card))
  {
    if (PLAYER_CARD_INSTANCE(player, card).damage_target_card == g_affected_card &&
        PLAYER_CARD_INSTANCE(player, card).damage_target_player == g_affected_card_controller &&
        g_affected_card != -1)
    {
      g_event_result += 2;
    }
    return 0;
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    return has_mana_w_global_cost_mod(player, card, COLOR_WHITE, 1);
  }

  if (event == EVENT_GET_SELECTED_CARD)
  {
    if (player == g_current_player)
    {
      if (g_mana_cost_modifiers_by_color[single_color_test_bit_to_color_t((int)PLAYER_CARD_INSTANCE(player, card).color)] != 0)
      {
        g_loaded_recorded_action_code = 1;
      }
      else
      {
        load_recorded_action_code(0);
      }
    }
    else
    {
      g_loaded_recorded_action_code = 1;
    }

    g_ai_recorded_action = ((int)(char)PLAYER_CARD_INSTANCE(player, card).damage_target_player << 8) |
                           PLAYER_CARD_INSTANCE(player, card).damage_target_card;
    return 0;
  }

  if (event == EVENT_ACTIVATE)
  {
    if (has_mana_w_global_cost_mod(player, card, COLOR_WHITE, 1) != 0)
    {
      if (player == g_current_player)
      {
        if (g_mana_cost_modifiers_by_color[single_color_test_bit_to_color_t((int)PLAYER_CARD_INSTANCE(player, card).color)] != 0)
        {
          g_x_value = charge_mana_w_global_cost_mod(player, card, COLOR_WHITE, 1);
        }
        else
        {
          charge_mana(player, COLOR_WHITE, -1);
        }
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
        if (g_mana_cost_modifiers_by_color[single_color_test_bit_to_color_t((int)PLAYER_CARD_INSTANCE(player, card).color)] != 0)
        {
          charge_mana_w_global_cost_mod(player, card, COLOR_WHITE, 1);
        }
        else
        {
          charge_mana(player, COLOR_WHITE, 1);
        }
        PLAYER_CARD_INSTANCE(player, card).eot_toughness = 1;
      }
      if (g_spell_fizzled != 1)
      {
        PLAYER_CARD_INSTANCE(player, card).targets[0].player =
            (int)(char)PLAYER_CARD_INSTANCE(player, card).damage_target_player;
        PLAYER_CARD_INSTANCE(player, card).targets[0].card =
            PLAYER_CARD_INSTANCE(player, card).damage_target_card;
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
    if (DAMAGE_TARGET_CARD_INSTANCE(player, card).internal_card_id != -1)
    {
      PARENT_CARD_INSTANCE(player, card).number_of_targets = 0;
      PARENT_CARD_INSTANCE(player, card).info_slot += (PLAYER_CARD_INSTANCE(player, card).eot_toughness & 0xff) * 0x100;
      if ((PARENT_CARD_INSTANCE(player, card).info_slot &
           0x80000) != 0)
      {
        PARENT_CARD_INSTANCE(player, card).info_slot &= ~STATE_ETB_THIS_TURN;
        local = create_legacy_effect(g_card_on_stack_controller,
                                     g_card_on_stack,
                                     LEGACY_EFFECT_PUMP,
                                     (int)(char)PLAYER_CARD_INSTANCE(player, card).damage_target_player, PLAYER_CARD_INSTANCE(player, card).damage_target_card);
        if (local != -1)
        {
          PLAYER_CARD_INSTANCE(player, local).counter_toughness =
              (short)PLAYER_CARD_INSTANCE(player, card).eot_toughness;
          PLAYER_CARD_INSTANCE(player, local).info_slot |= 0x80000;
        }
      }
    }
    else
    {
      g_spell_fizzled = 1;
    }
    return 0;
  }

  if (event == EVENT_SHOULD_AI_PLAY)
  {
    if (player == g_other_player)
    {
      g_ai_modifier += g_basiclandtypes_controlled[player][COLOR_WHITE] * 0xc;
    }
    else
    {
      g_ai_modifier -= g_basiclandtypes_controlled[player][COLOR_WHITE] * 0xc;
    }
  }

  if ((event == EVENT_CLEANUP) || (event == EVENT_SHOULD_AI_PLAY))
  {
    PLAYER_CARD_INSTANCE(player, card).eot_toughness = 0;
    PLAYER_CARD_INSTANCE(player, card).info_slot = PLAYER_CARD_INSTANCE(player, card).eot_toughness;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0052700b
// FUNCTION: SHANDALAR 0x004de7f8
int card_blessing(int player, int card, event_t event)
{
  int local;

  if (event == EVENT_UNTAP_PHASE)
  {
    ++g_ai_mana_demand_by_color[player][COLOR_WHITE];
  }

  if (event == EVENT_CAN_CAST)
  {
    return real_target_available((int *)0,
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
                                 0,
                                 0);
  }

  if (event == EVENT_CAST_SPELL && g_affected_card == card && g_affected_card_controller == player)
  {
    // g_ai_modifier += 0x18;
    PLAYER_CARD_INSTANCE(player, card).eot_toughness = 0;
    PLAYER_CARD_INSTANCE(player, card).info_slot = PLAYER_CARD_INSTANCE(player, card).eot_toughness;
    if (g_duel_ai_mode_state != 1)
    {
      load_text("prompts.txt", "BLESSING");
    }
    if (select_target_creature_and_store(player, player, card) != 0)
    {
      if ((player == g_other_player) && ((g_duel_network_flags & 2) == 0))
      {
        if (is_selected_target_already_attached(player, card, PLAYER_CARD_INSTANCE(player, card).internal_card_id) != 0)
        {
          g_ai_modifier += -0x60;
        }
        if (PLAYER_CARD_INSTANCE(player, card).targets[0].player == player)
        {
          g_ai_modifier += 0xc;
        }
        else
        {
          g_ai_modifier += -0xc;
        }
      }
    }
    else
    {
      g_spell_fizzled = 1;
    }
    return 0;
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
                               0))
    {
      PLAYER_CARD_INSTANCE(player, card).damage_target_player =
          (char)PLAYER_CARD_INSTANCE(player, card).targets[0].player;
      PLAYER_CARD_INSTANCE(player, card).damage_target_card =
          PLAYER_CARD_INSTANCE(player, card).targets[0].card;
    }
    else
    {
      kill_card(player, card, KILL_BURY);
      g_spell_fizzled = 1;
    }
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    return 0;
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    return has_mana_w_global_cost_mod(player, card, COLOR_WHITE, 1);
  }

  if (event == EVENT_GET_SELECTED_CARD)
  {
    if (player == g_current_player)
    {
      if (g_mana_cost_modifiers_by_color[single_color_test_bit_to_color_t((int)PLAYER_CARD_INSTANCE(player, card).color)] != 0)
      {
        g_loaded_recorded_action_code = 1;
      }
      else
      {
        load_recorded_action_code(0);
      }
    }
    else
    {
      g_loaded_recorded_action_code = 1;
    }

    g_ai_recorded_action = ((int)(char)PLAYER_CARD_INSTANCE(player, card).damage_target_player << 8) |
                           PLAYER_CARD_INSTANCE(player, card).damage_target_card;
    return 0;
  }

  if (event == EVENT_ACTIVATE)
  {
    if (has_mana_w_global_cost_mod(player, card, COLOR_WHITE, 1) != 0)
    {
      if (player == g_current_player)
      {
        if (g_mana_cost_modifiers_by_color[single_color_test_bit_to_color_t((int)PLAYER_CARD_INSTANCE(player, card).color)] != 0)
        {
          g_x_value = charge_mana_w_global_cost_mod(player, card, COLOR_WHITE, 1);
        }
        else
        {
          charge_mana(player, COLOR_WHITE, -1);
        }

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
        if (g_mana_cost_modifiers_by_color[single_color_test_bit_to_color_t((int)PLAYER_CARD_INSTANCE(player, card).color)] != 0)
        {
          charge_mana_w_global_cost_mod(player, card, COLOR_WHITE, 1);
        }
        else
        {
          charge_mana(player, COLOR_WHITE, 1);
        }
        PLAYER_CARD_INSTANCE(player, card).eot_toughness = 1;
      }

      if (g_spell_fizzled != 1)
      {
        PLAYER_CARD_INSTANCE(player, card).targets[0].player =
            (int)(char)PLAYER_CARD_INSTANCE(player, card).damage_target_player;
        PLAYER_CARD_INSTANCE(player, card).targets[0].card =
            PLAYER_CARD_INSTANCE(player, card).damage_target_card;
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
    if (DAMAGE_TARGET_CARD_INSTANCE(player, card).internal_card_id != -1)
    {
      PARENT_CARD_INSTANCE(player, card).info_slot += PLAYER_CARD_INSTANCE(player, card).eot_toughness & 0xff;
      PARENT_CARD_INSTANCE(player, card).info_slot += (PLAYER_CARD_INSTANCE(player, card).eot_toughness & 0xff) * 0x100;
      PARENT_CARD_INSTANCE(player, card).number_of_targets = 0;
      if ((PARENT_CARD_INSTANCE(player, card).info_slot &
           0x80000) != 0)
      {
        PARENT_CARD_INSTANCE(player, card).info_slot &= ~STATE_ETB_THIS_TURN;
        local = create_legacy_effect(g_card_on_stack_controller,
                                     g_card_on_stack,
                                     LEGACY_EFFECT_PUMP,
                                     (int)(char)PLAYER_CARD_INSTANCE(player, card).damage_target_player, PLAYER_CARD_INSTANCE(player, card).damage_target_card);
        if (local != -1)
        {
          PLAYER_CARD_INSTANCE(player, local).counter_power =
              (short)PLAYER_CARD_INSTANCE(player, card).eot_toughness;
          PLAYER_CARD_INSTANCE(player, local).counter_toughness =
              (short)PLAYER_CARD_INSTANCE(player, card).eot_toughness;
          PLAYER_CARD_INSTANCE(player, local).info_slot |= 0x80000;
        }
      }
    }
    else
    {
      g_spell_fizzled = 1;
    }
    return 0;
  }

  if (event == EVENT_SHOULD_AI_PLAY)
  {
    if (player == g_other_player)
    {
      g_ai_modifier += (g_basiclandtypes_controlled[player][COLOR_WHITE] + 2) * 12;
    }
    else
    {
      g_ai_modifier -= (g_basiclandtypes_controlled[player][COLOR_WHITE] + 2) * 12;
    }
  }

  if ((event == EVENT_CLEANUP) || (event == EVENT_SHOULD_AI_PLAY))
  {
    PLAYER_CARD_INSTANCE(player, card).eot_toughness = 0;
    PLAYER_CARD_INSTANCE(player, card).info_slot = PLAYER_CARD_INSTANCE(player, card).eot_toughness;
  }
  return 0;
}

// FUNCTION: MAGIC 0x00527e71
// FUNCTION: SHANDALAR 0x004df65e
int card_firebreathing(int player, int card, event_t event)
{
  int local;

  if (event == EVENT_UNTAP_PHASE)
  {
    ++g_ai_mana_demand_by_color[player][COLOR_RED];
  }

  if (event == EVENT_CAN_CAST)
  {
    return real_target_available((int *)0,
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
                                 0,
                                 0);
  }

  if (event == EVENT_CAST_SPELL && g_affected_card == card && g_affected_card_controller == player)
  {
    PLAYER_CARD_INSTANCE(player, card).eot_toughness = 0;
    PLAYER_CARD_INSTANCE(player, card).info_slot = PLAYER_CARD_INSTANCE(player, card).eot_toughness;
    if (g_duel_ai_mode_state != 1)
    {
      load_text("prompts.txt", "FIREBREATHING");
    }
    if (select_target_creature_and_store(player, player, card) != 0)
    {
      if ((player == g_other_player) && ((g_duel_network_flags & 2) == 0))
      {
        if (is_selected_target_already_attached(player, card, PLAYER_CARD_INSTANCE(player, card).internal_card_id) != 0)
        {
          g_ai_modifier += -0x60;
        }
        if (PLAYER_CARD_INSTANCE(player, card).targets[0].player == player)
        {
          g_ai_modifier += 0xc;
        }
        else
        {
          g_ai_modifier += -0xc;
        }
      }
    }
    else
    {
      g_spell_fizzled = 1;
    }
    return 0;
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    if (C_real_validate_target(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                               PLAYER_CARD_INSTANCE(player, card).targets[0].card,
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
                               0) != 0)
    {
      PLAYER_CARD_INSTANCE(player, card).damage_target_player =
          (char)PLAYER_CARD_INSTANCE(player, card).targets[0].player;
      PLAYER_CARD_INSTANCE(player, card).damage_target_card =
          PLAYER_CARD_INSTANCE(player, card).targets[0].card;
    }
    else
    {
      kill_card(player, card, KILL_BURY);
      g_spell_fizzled = 1;
    }
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    return 0;
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    return has_mana_w_global_cost_mod(player, card, 4, 1);
  }

  if (event == EVENT_GET_SELECTED_CARD)
  {
    if (player == g_current_player)
    {
      if (g_mana_cost_modifiers_by_color[single_color_test_bit_to_color_t((int)PLAYER_CARD_INSTANCE(player, card).color)] != 0)
      {
        g_loaded_recorded_action_code = 1;
      }
      else
      {
        load_recorded_action_code(0);
      }
    }
    else
    {
      g_loaded_recorded_action_code = 1;
    }

    g_ai_recorded_action = ((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player << 8) | PLAYER_CARD_INSTANCE(player, card).damage_target_card;
    return 0;
  }

  if (event == EVENT_ACTIVATE)
  {
    if (has_mana_w_global_cost_mod(player, card, 4, 1) != 0)
    {
      if (player == g_current_player)
      {
        if (g_mana_cost_modifiers_by_color[single_color_test_bit_to_color_t((int)PLAYER_CARD_INSTANCE(player, card).color)] != 0)
        {
          g_x_value = charge_mana_w_global_cost_mod(player, card, 4, 1);
        }
        else
        {
          charge_mana(player, 4, -1);
        }

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
        if (g_mana_cost_modifiers_by_color[single_color_test_bit_to_color_t((int)PLAYER_CARD_INSTANCE(player, card).color)] != 0)
        {
          charge_mana_w_global_cost_mod(player, card, 4, 1);
        }
        else
        {
          charge_mana(player, 4, 1);
        }
        PLAYER_CARD_INSTANCE(player, card).eot_toughness = 1;
      }

      if (g_spell_fizzled != 1)
      {
        PLAYER_CARD_INSTANCE(player, card).targets[0].player =
            (int)PLAYER_CARD_INSTANCE(player, card).damage_target_player;
        PLAYER_CARD_INSTANCE(player, card).targets[0].card =
            PLAYER_CARD_INSTANCE(player, card).damage_target_card;
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
    if (DAMAGE_TARGET_CARD_INSTANCE(player, card).internal_card_id != -1)
    {
      PARENT_CARD_INSTANCE(player, card).info_slot += PLAYER_CARD_INSTANCE(player, card).eot_toughness & 0xff;

      PARENT_CARD_INSTANCE(player, card).number_of_targets = 0;

      if ((PARENT_CARD_INSTANCE(player, card).info_slot &
           0x80000) != 0)
      {
        PARENT_CARD_INSTANCE(player, card).info_slot &= ~0x80000;

        local = create_legacy_effect(g_card_on_stack_controller,
                                     g_card_on_stack,
                                     LEGACY_EFFECT_PUMP,
                                     (int)(char)PLAYER_CARD_INSTANCE(player, card).damage_target_player, PLAYER_CARD_INSTANCE(player, card).damage_target_card);
        if (local != -1)
        {
          PLAYER_CARD_INSTANCE(player, local).counter_power =
              (short)PLAYER_CARD_INSTANCE(player, card).eot_toughness;
          PLAYER_CARD_INSTANCE(player, local).info_slot |= 0x80000;
        }
      }
    }
    else
    {
      g_spell_fizzled = 1;
    }
    return 0;
  }

  if (event == EVENT_SHOULD_AI_PLAY)
  {
    if (player == g_other_player)
    {
      g_ai_modifier += ((g_basiclandtypes_controlled[player][COLOR_RED] + 1) * 3) << 2;
    }
    else
    {
      g_ai_modifier -= ((g_basiclandtypes_controlled[player][COLOR_RED] + 1) * 3) << 2;
    }
  }

  if ((event == EVENT_CLEANUP) || (event == EVENT_SHOULD_AI_PLAY))
  {
    PLAYER_CARD_INSTANCE(player, card).eot_toughness = 0;
    PLAYER_CARD_INSTANCE(player, card).info_slot = PLAYER_CARD_INSTANCE(player, card).eot_toughness;
  }
  return 0;
}

// FUNCTION: MAGIC 0x00528bc0
// FUNCTION: SHANDALAR 0x004e03af
int card_fear(int player, int card, event_t event)
{
  if (event == EVENT_BLOCK_LEGALITY && PLAYER_CARD_INSTANCE(player, card).damage_target_card == g_attacking_card && PLAYER_CARD_INSTANCE(player, card).damage_target_player == g_attacking_card_controller && (PLAYER_CARD_INSTANCE(player, card).state & STATE_INVISIBLE) == 0)
  {
    if ((global_cards_data[PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).internal_card_id].type & TYPE_ARTIFACT) == 0 && (((1 << ((unsigned char)get_sleighted_color(player, card, 1))) & PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).color) == 0))
    {
      ++g_event_result;
    }
    return 0;
  }

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

  if ((event == EVENT_CAST_SPELL) && (card == g_affected_card) && (player == g_affected_card_controller))
  {
    if (g_duel_ai_mode_state != 1)
    {
      load_text("prompts.txt", "FEAR");
    }
    g_spell_fizzled = !select_target_creature_and_store(player, player, card);

    return 0;
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    if (C_real_validate_target(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                               PLAYER_CARD_INSTANCE(player, card).targets[0].card,
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
      PLAYER_CARD_INSTANCE(player, card).damage_target_player = PLAYER_CARD_INSTANCE(player, card).targets[0].player;
      PLAYER_CARD_INSTANCE(player, card).damage_target_card = PLAYER_CARD_INSTANCE(player, card).targets[0].card;
    }
    else
    {
      kill_card(player, card, KILL_BURY);
      g_spell_fizzled = 1;
    }
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x00528fbc
// FUNCTION: SHANDALAR 0x004e07ad
int card_seeker(int player, int card, event_t event)
{
  if (event == EVENT_BLOCK_LEGALITY &&
      PLAYER_CARD_INSTANCE(player, card).damage_target_card == g_attacking_card &&
      PLAYER_CARD_INSTANCE(player, card).damage_target_player == g_attacking_card_controller &&
      (PLAYER_CARD_INSTANCE(player, card).state & STATE_INVISIBLE) == 0)
  {
    if ((global_cards_data[PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).internal_card_id].type & TYPE_ARTIFACT) == 0 &&
        (((1 << (unsigned char)get_sleighted_color(player, card, COLOR_WHITE)) &
          (int)(char)PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).color) == 0))
    {
      ++g_event_result;
    }
    return 0;
  }

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
                                 -1,
                                 -1,
                                 0,
                                 0,
                                 0);
  }

  if (event == EVENT_CAST_SPELL && g_affected_card == card && g_affected_card_controller == player)
  {
    if (g_duel_ai_mode_state != 1)
    {
      load_text("prompts.txt", "SEEKER");
    }
    g_spell_fizzled = (select_target_creature_and_store(player, player, card) == 0);
    return 0;
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    if (C_real_validate_target(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                               PLAYER_CARD_INSTANCE(player, card).targets[0].card,
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
      PLAYER_CARD_INSTANCE(player, card).damage_target_player = PLAYER_CARD_INSTANCE(player, card).targets[0].player;
      PLAYER_CARD_INSTANCE(player, card).damage_target_card = PLAYER_CARD_INSTANCE(player, card).targets[0].card;
    }
    else
    {
      kill_card(player, card, KILL_BURY);
      g_spell_fizzled = 1;
    }
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x005293ba
// FUNCTION: SHANDALAR 0x004e0bab
int card_web(int player, int card, event_t event)
{
  int test_card;
  int greatest_power;

  if (event == EVENT_CAN_CAST)
  {
    return real_target_available((int *)0,
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
                                 0,
                                 0);
  }

  if (((event == EVENT_CAST_SPELL) && (g_affected_card == card)) && (g_affected_card_controller == player))
  {
    if (g_duel_ai_mode_state != 1)
    {
      load_text("prompts.txt", "WEB");
    }
    if (select_target_creature_and_store(player, player, card) == 0)
    {
      g_spell_fizzled = 1;
    }
    else
    {
      g_spell_fizzled = 0;
    }

    if ((g_spell_fizzled != 1) && (g_other_player == player) && ((g_duel_network_flags & 2) == 0))
    {
      test_card = 0;
      greatest_power = -1;
      for (; test_card < g_active_cards_count[1 - player]; ++test_card)
      {
        if (is_in_play(1 - player, test_card) && (greatest_power < PLAYER_CARD_INSTANCE(1 - player, test_card).power))
        {
          greatest_power = PLAYER_CARD_INSTANCE(1 - player, test_card).power;
        }
      }

      if ((TARGET_CARD_INSTANCE(player, card, 0).toughness <= greatest_power) && (greatest_power < TARGET_CARD_INSTANCE(player, card, 0).toughness + 2))
      {
        g_ai_modifier += greatest_power * 3 + 0x18;
      }

      if ((TARGET_CARD_INSTANCE(player, card, 0).regen_status &
           (KEYWORD_FLYING | KEYWORD_REACH)) != 0)
      {
        g_ai_modifier += -0x18;
      }
      else
      {
        g_ai_modifier += TARGET_CARD_INSTANCE(player, card, 0).toughness;
      }
    }
    return 0;
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
                               0) != 0)
    {
      PLAYER_CARD_INSTANCE(player, card).damage_target_player =
          (char)PLAYER_CARD_INSTANCE(player, card).targets[0].player;
      PLAYER_CARD_INSTANCE(player, card).damage_target_card =
          PLAYER_CARD_INSTANCE(player, card).targets[0].card;
    }
    else
    {
      kill_card(player, card, KILL_BURY);
      g_spell_fizzled = 1;
    }
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;

    return 0;
  }

  if (((event == EVENT_TOUGHNESS) && (PLAYER_CARD_INSTANCE(player, card).damage_target_card == g_affected_card)) && (((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player == g_affected_card_controller) && ((g_affected_card != -1) && is_in_play(player, card))))
  {
    g_event_result += 2;
  }
  if (((event == EVENT_ABILITIES) && (PLAYER_CARD_INSTANCE(player, card).damage_target_card == g_affected_card)) && (((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player == g_affected_card_controller) && ((g_affected_card != -1) && is_in_play(player, card))))
  {
    g_event_result |= KEYWORD_REACH;
  }

  return 0;
}

// FUNCTION: MAGIC 0x00529afb
// FUNCTION: SHANDALAR 0x004e12ec
int card_stasis(int player, int card, event_t event)
{
  int current_card;

  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if (event == EVENT_CAST_SPELL && card == g_affected_card && player == g_affected_card_controller)
  {
    if (count_permanents_by_internal_card_id(player, PLAYER_CARD_INSTANCE(player, card).internal_card_id, -1) == 0)
    {
      g_ai_modifier += g_creature_power_by_color[1 - player][7] - g_creature_power_by_color[player][7];
    }
    PLAYER_CARD_INSTANCE(player, card).damage_target_player = (char)player;
    return 0;
  }

  if (event == EVENT_SETUP_UPKEEP_COSTS && card == g_affected_card && player == g_affected_card_controller && player == g_current_player && g_event_player == g_current_player)
  {
    PLAYER_CARD_INSTANCE(player, card).upkeep_flags |= UPKEEP_UPKEEP_TRIGGER;
    PLAYER_CARD_INSTANCE(player, card).upkeep_blue =
        PLAYER_CARD_INSTANCE(player, card).upkeep_blue + 1;
    return 0;
  }

  if (event == EVENT_UPKEEP_COSTS_UNPAID)
  {
    kill_card(g_card_on_stack_controller, g_card_on_stack, KILL_BURY);
    return 0;
  }

  if (event == EVENT_SHOULD_AI_PLAY)
  {
    int max_cards;
    int ai_tweak;
    if (g_basiclandtypes_controlled[player][2] < 1)
    {
      kill_card(player, card, KILL_BURY);
    }
    else
    {
      max_cards = MAX(g_active_cards_count[g_other_player], g_active_cards_count[g_active_player]);

      current_card = 0;
      ai_tweak = 0;
      for (; current_card < max_cards; ++current_card)
      {
        if (is_in_play(g_active_player, current_card) && (global_cards_data[PLAYER_CARD_INSTANCE(g_active_player, current_card).internal_card_id].type & TYPE_CREATURE) != 0)
        {
          if ((PLAYER_CARD_INSTANCE(g_active_player, current_card).state & STATE_TAPPED) != 0)
          {
            ai_tweak += PLAYER_CARD_INSTANCE(g_active_player, current_card).power * 2;
          }
          else if (has_vigilance(g_active_player, current_card) == 0)
          {
            ai_tweak += PLAYER_CARD_INSTANCE(g_active_player, current_card).power;
          }
        }

        if (is_in_play(g_other_player, current_card) && (global_cards_data[PLAYER_CARD_INSTANCE(g_other_player, current_card).internal_card_id].type & TYPE_CREATURE) != 0)
        {
          if ((PLAYER_CARD_INSTANCE(g_other_player, current_card).state & STATE_TAPPED) != 0)
          {
            ai_tweak -= PLAYER_CARD_INSTANCE(g_other_player, current_card).power * 2;
          }
          else if (has_vigilance(g_other_player, current_card) == 0)
          {
            ai_tweak -= PLAYER_CARD_INSTANCE(g_other_player, current_card).power;
          }
        }
      }

      g_ai_modifier += ai_tweak * 0xc;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x00529fd5
// FUNCTION: SHANDALAR 0x004e17ca
int card_magnetic_mountain(int player, int card, event_t event)
{
  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if (event == EVENT_CAST_SPELL && g_affected_card == card && g_affected_card_controller == player)
  {
    g_ai_modifier += g_creature_power_by_color[1 - player][COLOR_BLUE] -
                     g_creature_power_by_color[player][COLOR_BLUE];
    return 0;
  }

  if (event == EVENT_UNTAP &&
      ((1 << get_sleighted_color(player, card, COLOR_BLUE)) & (int)(char)PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).color) != 0 &&
      (global_cards_data[PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).internal_card_id].type & TYPE_CREATURE) != 0)
  {
    PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).untap_status &= ~3;
    return 0;
  }

  if (event == EVENT_SET_UNTAP_COST &&
      (PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).state & STATE_TAPPED) != 0 &&
      g_affected_card_controller == g_current_player &&
      g_event_player == g_current_player)
  {
    if ((global_cards_data[PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).internal_card_id].type & TYPE_CREATURE) != 0 &&
        ((1 << get_sleighted_color(player, card, COLOR_BLUE)) & (int)(char)PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).color) != 0)
    {
      PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).upkeep_flags |= 0x10;
      PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).mana_to_untap[COLOR_COLORLESS] += 4;
    }
    return 0;
  }

  if (event == EVENT_CAST_SPELL)
  {
    if (((1 << get_sleighted_color(player, card, COLOR_BLUE)) & (int)(char)PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).color) != 0 &&
        (global_cards_data[PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).internal_card_id].type & TYPE_CREATURE) != 0)
    {
      PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).mana_to_untap[COLOR_COLORLESS] += 4;
    }

    return 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0052a3fd
// FUNCTION: SHANDALAR 0x004e1bf2
int card_paralyze(int player, int card, event_t event)
{
  struct
  {
    target_t tgt;                /* ebp-0x94 */
    int unused_08;               /* ebp-0x8c */
    target_t *tgt_ptr;           /* ebp-0x88 */
    target_definition_t td;      /* ebp-0x84 */
    int target_player;           /* ebp-0x30 */
    int target_card;             /* ebp-0x2c */
    int internal_card_id;        /* ebp-0x28 */
    char unused_70[0x24];        /* ebp-0x24 */
    target_definition_t *td_ptr; /* ebp-0x04 */
  } s;

  s.td_ptr = &s.td;
  s.tgt_ptr = &s.tgt;

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
                                 ~SUB_WALL,
                                 -1,
                                 -1,
                                 0,
                                 0,
                                 0);
  }

  if (((event == EVENT_CAST_SPELL) && (g_affected_card == card)) && (g_affected_card_controller == player))
  {
    if (g_duel_ai_mode_state != 1)
    {
      load_text("prompts.txt", "PARALYZE");
    }

    if (select_target_creature_and_store(player, 2, card) != 0)
    {
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                           PLAYER_CARD_INSTANCE(player, card).targets[0].card)
          .mana_to_untap[0] += 4;

      if (g_other_player == player && (g_duel_network_flags & 2) == 0)
      {
        if (PLAYER_CARD_INSTANCE(player, card).targets[0].player == player)
        {
          g_ai_modifier += -0x18;
        }
        else
        {
          ;

          if ((char)global_cards_data[PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                                                           PLAYER_CARD_INSTANCE(player, card).targets[0].card)
                                          .internal_card_id]
                  .subtype == 0)
          {
            if ((PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                                      PLAYER_CARD_INSTANCE(player, card).targets[0].card)
                     .token_status &
                 STATUS_WALL_CAN_ATTACK) == 0)
            {
              g_ai_modifier += -0x18;
            }

            if (is_selected_target_already_attached(
                    player,
                    card,
                    PLAYER_CARD_INSTANCE(player, card).internal_card_id) != 0)
            {
              g_ai_modifier += -0x60;
            }
          }
        }
      }
    }
    else
    {
      g_spell_fizzled = 1;
    }

    return 0;
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    if (C_real_validate_target(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                               PLAYER_CARD_INSTANCE(player, card).targets[0].card,
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
                               0) != 0)
    {
      PLAYER_CARD_INSTANCE(player, card).damage_target_player = (char)PLAYER_CARD_INSTANCE(player, card).targets[0].player;
      PLAYER_CARD_INSTANCE(player, card).damage_target_card = PLAYER_CARD_INSTANCE(player, card).targets[0].card;
      tap_card_and_dispatch_event((int)(char)PLAYER_CARD_INSTANCE(player, card).damage_target_player, PLAYER_CARD_INSTANCE(player, card).damage_target_card);
    }
    else
    {
      kill_card(player, card, KILL_BURY);
      g_spell_fizzled = 1;
    }

    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    return 0;
  }

  if (event == EVENT_SHOULD_AI_PLAY &&
      PLAYER_CARD_INSTANCE(player, card).damage_target_player == g_active_player)
  {
    g_ai_modifier +=
        PLAYER_CARD_INSTANCE((int)(char)PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                             PLAYER_CARD_INSTANCE(player, card).damage_target_card)
            .attack_rating /
        2;
  }

  if (event == EVENT_UNTAP && PLAYER_CARD_INSTANCE(player, card).damage_target_card == g_affected_card &&
      PLAYER_CARD_INSTANCE(player, card).damage_target_player == g_affected_card_controller && g_affected_card != -1)
  {
    DAMAGE_TARGET_CARD_INSTANCE(player, card).untap_status &= ~3;
    return 0;
  }

  if (event == EVENT_SET_UNTAP_COST && PLAYER_CARD_INSTANCE(player, card).damage_target_card == g_affected_card &&
      PLAYER_CARD_INSTANCE(player, card).damage_target_player == g_affected_card_controller && g_affected_card != -1 &&
      (DAMAGE_TARGET_CARD_INSTANCE(player, card).state &
       STATE_TAPPED) != 0 &&
      PLAYER_CARD_INSTANCE(player, card).damage_target_player == g_current_player && g_event_player == g_current_player)
  {
    PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).upkeep_flags |= UPKEEP_UNTAP_TRIGGER;
    PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).mana_to_untap[0] += 4;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0052adf2
// FUNCTION: SHANDALAR 0x004e25e6
int is_selected_target_already_attached(int player, int card, int internal_card_id)
{
  struct
  {
    card_instance_t *test_instance; /* ebp-0x14 */
    int test_player;                /* ebp-0x10 */
    int current_card;               /* ebp-0x0c */
    int max_cards;                  /* ebp-0x08 */
    int found;                      /* ebp-0x04 */
  } s;

  (void)internal_card_id;

  s.max_cards = MAX(g_active_cards_count[1], g_active_cards_count[0]);
  s.found = 0;
  for (s.current_card = 0; s.current_card < s.max_cards; ++s.current_card)
  {
    for (s.test_player = 0; s.test_player < 2; ++s.test_player)
    {
      s.test_instance = &global_card_instances[s.test_player][s.current_card];
      if (is_in_play(s.test_player, s.current_card) != 0 &&
          (global_card_instances[player])[card].targets[0].player == (int)(char)s.test_instance->damage_target_player &&
          (global_card_instances[player])[card].targets[0].card == s.test_instance->damage_target_card)
      {
        s.found = 1;
        break;
      }
    }
  }

  return s.found;
}

// FUNCTION: MAGIC 0x0052af2f
// FUNCTION: SHANDALAR 0x004e2723
int card_smoke(int player, int card, event_t event)
{
  target_t selected_target;
  int current_card;

  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if (event == EVENT_CAST_SPELL && g_affected_card == card && g_affected_card_controller == player)
  {
    if (count_permanents_by_internal_card_id(player, PLAYER_CARD_INSTANCE(player, card).internal_card_id, -1) == 0)
    {
      g_ai_modifier +=
          (g_creature_power_by_color[g_active_player][7] - g_creature_power_by_color[g_other_player][7]) * 0xc;
    }
    return 0;
  }

  if (event == EVENT_UNTAP && (global_cards_data[PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).internal_card_id].type & TYPE_CREATURE) != 0)
  {
    PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).untap_status &= ~2;
    g_ai_score_baseline |= 2;
    return 0;
  }

  if (g_current_phase == PHASE_UNTAP && g_affected_card == card && g_affected_card_controller == player)
  {
    if (event == EVENT_TRIGGER)
    {
      if (real_target_available((int *)0, TARGET_SCAN_DIRECT, g_current_player, g_current_player, g_current_player, 0x200, TYPE_CREATURE, 0, 0, 0, 0, 0, -1, -1, -1, -1, 0, 0x800, 0) != 0)
      {
      }
      else if (real_target_available((int *)0, TARGET_SCAN_DIRECT, g_current_player, g_current_player, g_current_player, 0x200, TYPE_CREATURE, 0, 0, 0, 0, 0, -1, -1, -1, -1, 0, 0x400, 0) != 0)
      {
        g_event_result |= 2;
      }
      else
      {
      }
    }

    if (event == EVENT_RESOLVE_TRIGGER)
    {
      if (g_other_player == g_current_player && (g_duel_network_flags & 2) == 0)
      {
        selected_target.player = g_current_player;
        selected_target.card = choose_best_tapped_permanent_for_ai(g_current_player, 2);
        load_text("prompts.txt", "SMOKE");
        do_dialog(player,
                  player,
                  card,
                  selected_target.player,
                  selected_target.card,
                  g_text_lines[1],
                  0);
      }
      else
      {
        load_text("prompts.txt", "SMOKE");
        C_real_select_target(g_current_player,
                             g_current_player,
                             g_current_player,
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
                             TARGET_STATE_COULD_UNTAP | TARGET_STATE_TAPPED,
                             0,
                             g_text_lines[0],
                             0,
                             &selected_target);
      }

      PLAYER_CARD_INSTANCE(selected_target.player, selected_target.card).untap_status |= 2;
      for (current_card = 0; current_card < g_active_cards_count[g_current_player]; ++current_card)
      {
        if (is_in_play(g_current_player, current_card) && (PLAYER_CARD_INSTANCE(g_current_player, current_card).state & STATE_TAPPED) != 0 && (global_cards_data[PLAYER_CARD_INSTANCE(g_current_player, current_card).internal_card_id].type & TYPE_CREATURE) != 0 && (PLAYER_CARD_INSTANCE(g_current_player, current_card).untap_status & 2) == 0)
        {
          PLAYER_CARD_INSTANCE(g_current_player, current_card).untap_status &= ~1;
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

// FUNCTION: MAGIC 0x0052b3d4
// FUNCTION: SHANDALAR 0x004e2bc8
int card_power_surge(int player, int card, event_t event)
{
  struct
  {
    int current_card;
    int damage;
  } s;

  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if (event == EVENT_CAST_SPELL && card == g_affected_card && player == g_affected_card_controller)
  {
    ;
  }

  if (event == EVENT_CAN_SKIP_TURN)
  {
    PLAYER_CARD_INSTANCE(player, card).eot_toughness = 0;
    for (s.current_card = 0; s.current_card < g_active_cards_count[g_current_player]; ++s.current_card)
    {
      if (is_in_play(g_current_player, s.current_card) && (PLAYER_CARD_INSTANCE(g_current_player, s.current_card).state & STATE_TAPPED) == 0 && (global_cards_data[PLAYER_CARD_INSTANCE(g_current_player, s.current_card).internal_card_id].type & TYPE_LAND) != 0)
      {
        ++PLAYER_CARD_INSTANCE(player, card).eot_toughness;
      }
    }
    return 0;
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    if (g_current_phase == 4 && (PLAYER_CARD_INSTANCE(player, card).info_slot & 1) == 0 && g_event_player == g_current_player)
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
    return 0;
  }

  if (event == EVENT_UPKEEP_COSTS_UNPAID)
  {
    damage_player(g_current_player, PLAYER_CARD_INSTANCE(player, card).eot_toughness, g_card_on_stack_controller, g_card_on_stack);
    PLAYER_CARD_INSTANCE(player, card).eot_toughness = 0;
    return 0;
  }

  if (event == EVENT_CLEANUP)
  {
    PLAYER_CARD_INSTANCE(player, card).info_slot &= ~1;
  }

  if (event == EVENT_SHOULD_AI_PLAY)
  {
    s.current_card = 0;
    s.damage = 0;
    for (; s.current_card < g_active_cards_count[1 - g_current_player]; ++s.current_card)
    {
      if (is_in_play(1 - g_current_player, s.current_card) && (PLAYER_CARD_INSTANCE(1 - g_current_player, s.current_card).state & STATE_TAPPED) == 0 && (global_cards_data[PLAYER_CARD_INSTANCE(1 - g_current_player, s.current_card).internal_card_id].type & TYPE_LAND) != 0)
      {
        ++s.damage;
      }
    }
    damage_player(1 - g_current_player, s.damage, player, card);
  }

  return 0;
}

// FUNCTION: MAGIC 0x0052b842
// FUNCTION: SHANDALAR 0x004e303a
int card_burrowing(int player, int card, event_t event)
{
  int result;

  if ((((event == EVENT_CAST_SPELL) && (g_affected_card == card)) && (g_affected_card_controller == player)) && (g_duel_ai_mode_state != 1))
  {
    load_text("prompts.txt", "BURROWING");
  }

  result = generic_creature_ability_aura(player, card, event, 1 << (get_hacked_color(player, card, 4) - 1U));
  if ((((event == EVENT_CAST_SPELL) && (g_affected_card == card)) && (g_affected_card_controller == player)) && (player == g_other_player))
  {
    if (((TARGET_CARD_INSTANCE(player, card, 0).regen_status & (1 << (get_hacked_color(player, card, 4) - 1U))) != 0) || (PLAYER_CARD_INSTANCE(player, card).targets[0].player == g_active_player))
    {
      g_ai_modifier += -0x60;
    }
  }

  return result;
}

// FUNCTION: MAGIC 0x0052b9f0
// FUNCTION: SHANDALAR 0x004e31e9
int card_wanderlust(int player, int card, event_t event)
{
  if (event == EVENT_CAN_CAST)
  {
    return real_target_available((int *)0,
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
                                 0,
                                 0);
  }

  if (((event == EVENT_CAST_SPELL) && (g_affected_card == card)) && (g_affected_card_controller == player))
  {
    if (g_duel_ai_mode_state != 1)
    {
      load_text("prompts.txt", "WANDERLUST");
    }
    if (select_target_creature_and_store(player, 1 - player, card) == 0)
    {
      g_spell_fizzled = 1;
    }
    else
    {
      g_spell_fizzled = 0;
    }
    if (PLAYER_CARD_INSTANCE(player, card).targets[0].player == g_active_player)
    {
      g_ai_modifier += 0x30;
    }
    if (PLAYER_CARD_INSTANCE(player, card).targets[0].player == g_other_player)
    {
      g_ai_modifier += -0x60;
    }
    return 0;
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
                               0))
    {
      PLAYER_CARD_INSTANCE(player, card).damage_target_player =
          (char)PLAYER_CARD_INSTANCE(player, card).targets[0].player;
      PLAYER_CARD_INSTANCE(player, card).damage_target_card =
          PLAYER_CARD_INSTANCE(player, card).targets[0].card;
    }
    else
    {
      kill_card(player, card, KILL_BURY);
      g_spell_fizzled = 1;
    }
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    return 0;
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    if ((((g_current_phase == EVENT_UPKEEP_PHASE) && (g_event_player == g_current_player)) && ((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player == g_current_player)) && ((PLAYER_CARD_INSTANCE(player, card).info_slot & 1) == 0))
    {
      PLAYER_CARD_INSTANCE(player, card).upkeep_flags |= 0x101;
      g_activation_event_flags |= 3;
      return 1;
    }
    return 0;
  }

  if (((event == EVENT_UPKEEP_PHASE) && (g_affected_card == card)) && (g_affected_card_controller == player))
  {
    PLAYER_CARD_INSTANCE(player, card).info_slot |= 1;
    g_upkeep_payment_completed = 1;
    g_event_result |= 1;
    return 0;
  }

  if (event == EVENT_UPKEEP_COSTS_UNPAID)
  {
    damage_player((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                  1,
                  g_card_on_stack_controller,
                  g_card_on_stack);
    return 0;
  }

  if (event == EVENT_CLEANUP)
  {
    PLAYER_CARD_INSTANCE(player, card).info_slot &= ~STATE_JUST_DRAWED;
  }
  if ((event == EVENT_SHOULD_AI_PLAY) && is_in_play(player, card) && ((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player != -1))
  {
    if ((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player == g_active_player)
    {
      g_ai_modifier += MAX(0x18 - g_life[(int)PLAYER_CARD_INSTANCE(player, card).damage_target_player], 1) * 0x18;
    }
    else
    {
      g_ai_modifier -= -(-MAX(0x18 - g_life[(int)PLAYER_CARD_INSTANCE(player, card).damage_target_player], 1) * 0x18);
    }
  }
  return 0;
}

// FUNCTION: MAGIC 0x0052c052
// FUNCTION: SHANDALAR 0x004e384b
int card_instill_energy(int player, int card, event_t event)
{
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

  if (event == EVENT_CAST_SPELL && g_affected_card == card && g_affected_card_controller == player)
  {
    if (g_duel_ai_mode_state != 1)
    {
      load_text("prompts.txt", "INSTILL_ENERGY");
    }

    if (!select_target_creature_and_store(player, player, card))
    {
      g_spell_fizzled = 1;
    }
    else
    {
      g_spell_fizzled = 0;
    }

    if (g_spell_fizzled != 1 && player == g_other_player)
    {
      if (global_cards_data[TARGET_CARD_INSTANCE(player, card, 0).internal_card_id]
                  .subtype == 0 &&
          (TARGET_CARD_INSTANCE(player, card, 0).token_status &
           STATUS_WALL_CAN_ATTACK) == 0)
      {
        g_ai_modifier -= 0x30;
      }
      if ((global_cards_data[TARGET_CARD_INSTANCE(player, card, 0).internal_card_id]
               .extra_ability &
           1) != 0 &&
          PLAYER_CARD_INSTANCE(player, card).targets[0].player == player)
      {
        g_ai_modifier += 0x30;
      }
    }
    return 0;
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    if (C_real_validate_target(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                               PLAYER_CARD_INSTANCE(player, card).targets[0].card,
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
      PLAYER_CARD_INSTANCE(player, card).damage_target_player = (char)PLAYER_CARD_INSTANCE(player, card).targets[0].player;
      PLAYER_CARD_INSTANCE(player, card).damage_target_card = PLAYER_CARD_INSTANCE(player, card).targets[0].card;
      if ((DAMAGE_TARGET_CARD_INSTANCE(player, card).state &
           STATE_IN_PLAY) != 0)
      {
        PLAYER_CARD_INSTANCE(player, card).eot_toughness = 1;
        DAMAGE_TARGET_CARD_INSTANCE(player, card).state &= ~STATE_SUMMONSICK_BOTH;
      }
    }
    else
    {
      kill_card(player, card, KILL_BURY);
      g_spell_fizzled = 1;
    }
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    return 0;
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    if (g_mana_cost_modifiers_by_color[single_color_test_bit_to_color_t((color_test_t)PLAYER_CARD_INSTANCE(player, card).color)] != 0 && has_mana_w_global_cost_mod(player, card, COLOR_ANY, 0) == 0)
    {
      return 0;
    }

    return (PLAYER_CARD_INSTANCE(player, card).info_slot == 0 && g_affected_card_controller == g_current_player &&
            (PLAYER_CARD_INSTANCE(player, card).state & STATE_INVISIBLE) == 0);
  }

  if (event == EVENT_ACTIVATE && PLAYER_CARD_INSTANCE(player, card).info_slot == 0)
  {
    if (g_mana_cost_modifiers_by_color[single_color_test_bit_to_color_t((color_test_t)PLAYER_CARD_INSTANCE(player, card).color)] != 0)
    {
      charge_mana_w_global_cost_mod(player, card, 0, 0);
    }

    if (g_spell_fizzled != 1)
    {
      ++PLAYER_CARD_INSTANCE(player, card).info_slot;
    }
    return 0;
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    if (DAMAGE_TARGET_CARD_INSTANCE(player, card).internal_card_id != -1)
    {
      DAMAGE_TARGET_CARD_INSTANCE(player, card).state &= ~STATE_TAPPED;
    }
    else
    {
      g_spell_fizzled = 1;
    }
    return 0;
  }

  if (g_trigger_condition == TRIGGER_LEAVE_PLAY && g_affected_card == card && g_affected_card_controller == player &&
      PLAYER_CARD_INSTANCE(player, card).eot_toughness != 0 &&
      PLAYER_CARD_INSTANCE(player, card).damage_target_player != -1 &&
      DAMAGE_TARGET_CARD_INSTANCE(player, card).internal_card_id != -1 &&
      g_trigger_cause_controller == player && g_trigger_cause == card && g_current_turn == player)
  {
    if (event == EVENT_TRIGGER)
    {
      g_event_result |= 2;
    }
    if (event == EVENT_RESOLVE_TRIGGER)
    {
      DAMAGE_TARGET_CARD_INSTANCE(player, card).state |= 0x30000;
    }
  }

  if (event == EVENT_CLEANUP && g_affected_card == card && g_affected_card_controller == player)
  {
    PLAYER_CARD_INSTANCE(player, card).eot_toughness = 0;
    PLAYER_CARD_INSTANCE(player, card).info_slot = PLAYER_CARD_INSTANCE(player, card).eot_toughness;
  }

  if (event == EVENT_CARDCONTROLLED && PLAYER_CARD_INSTANCE(player, card).damage_target_card == g_affected_card &&
      PLAYER_CARD_INSTANCE(player, card).damage_target_player == g_affected_card_controller && g_affected_card != -1)
  {
    if ((DAMAGE_TARGET_CARD_INSTANCE(player, card).state &
         STATE_IN_PLAY) != 0)
    {
      PLAYER_CARD_INSTANCE(player, card).eot_toughness = 1;
      DAMAGE_TARGET_CARD_INSTANCE(player, card).state &= ~STATE_SUMMONSICK_BOTH;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0052cde5
// FUNCTION: SHANDALAR 0x004e45de
int card_flood(int player, int card, event_t event)
{
  target_t target;

  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if (event == EVENT_CAST_SPELL && g_affected_card == card && g_affected_card_controller == player)
  {
    g_ai_modifier += 0x30;
    return 0;
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    return has_mana_w_global_cost_mod(player, card, COLOR_BLUE, 2) &&
           real_target_available((int *)0,
                                 TARGET_SCAN_DIRECT,
                                 player,
                                 2,
                                 2,
                                 TARGET_ZONE_IN_PLAY,
                                 TYPE_CREATURE,
                                 TYPE_NONE,
                                 0,
                                 get_protections_from(player, card) | 0x20,
                                 COLOR_TEST_0,
                                 COLOR_TEST_0,
                                 -1,
                                 -1,
                                 -1,
                                 -1,
                                 0,
                                 0,
                                 0);
  }

  if (event == EVENT_ACTIVATE)
  {
    charge_mana_w_global_cost_mod(player, card, COLOR_BLUE, 2);
    if (g_spell_fizzled != 1)
    {
      if (g_duel_ai_mode_state != 1)
      {
        load_text("prompts.txt", "FLOOD");
      }
      if (C_real_select_target(player,
                               2,
                               1 - player,
                               TARGET_ZONE_IN_PLAY,
                               TYPE_CREATURE,
                               TYPE_NONE,
                               0,
                               get_protections_from(player, card) | 0x20,
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
      }
      else
      {
        g_spell_fizzled = 1;
      }
    }
    return 0;
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    if (C_real_validate_target(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                               PLAYER_CARD_INSTANCE(player, card).targets[0].card,
                               (char *)0,
                               player,
                               2,
                               2,
                               TARGET_ZONE_IN_PLAY,
                               TYPE_CREATURE,
                               TYPE_NONE,
                               0,
                               get_protections_from(player, card) | 0x20,
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
      tap_card_and_dispatch_event(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
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

// FUNCTION: MAGIC 0x0052d1c1
// FUNCTION: SHANDALAR 0x004e49ba
int card_greed(int player, int card, event_t event)
{
  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    if (player == g_other_player && (g_duel_network_flags & 2) == 0 && g_life[player] == 2)
    {
      return 0;
    }
    return has_mana_w_global_cost_mod(player, card, COLOR_BLACK, 1) && g_life[player] >= 2;
  }

  if (event == EVENT_ACTIVATE)
  {
    charge_mana_w_global_cost_mod(player, card, COLOR_BLACK, 1);
    g_ai_modifier -= (g_duel_summary.hand_counts[player] * 2 - 4 - g_life[player] + 10) * 12;
    return 0;
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    draw_card_for_player(player);
    g_life[player] -= 2;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0052d2d4
// FUNCTION: SHANDALAR 0x004e4ace
int card_pestilence(int player, int card, event_t event)
{
  struct
  {
    card_instance_t *instance;
    int min_toughness;
    int creature_toughness_counts[16];
    int opponent_life_div_swamps;
    int toughness_index;
    int best_score;
    int loop_player;
    int loop_card;
    int max_active;
    int running_score;
  } s;

  if (event == EVENT_SHOULD_AI_PLAY)
  {
    if (is_in_play(player, card) != 0)
    {
      s.min_toughness = MIN(g_basiclandtypes_controlled[player][COLOR_BLACK], g_life[player]);

      for (s.toughness_index = 0; s.toughness_index < 16; ++s.toughness_index)
      {
        s.creature_toughness_counts[s.toughness_index] = 0;
      }

      s.max_active = MAX(g_active_cards_count[1], g_active_cards_count[0]);

      for (s.loop_card = 0; s.max_active > s.loop_card; ++s.loop_card)
      {
        for (s.loop_player = 0; s.loop_player < 2; ++s.loop_player)
        {
          s.instance = &PLAYER_CARD_INSTANCE(s.loop_player, s.loop_card);
          if (is_in_play(s.loop_player, s.loop_card) != 0)
          {
            if (s.instance->toughness <= s.min_toughness && s.instance->toughness < 16)
            {
              if (s.loop_player == player)
              {
                --s.creature_toughness_counts[s.instance->toughness];
              }
              else
              {
                ++s.creature_toughness_counts[s.instance->toughness];
              }
            }
          }
        }
      }

      s.toughness_index = 0;
      s.running_score = 0;
      s.best_score = 0;
      for (; s.toughness_index < 16; ++s.toughness_index)
      {
        s.running_score += s.creature_toughness_counts[s.toughness_index] * (s.toughness_index + 1) * 0x18;
        if (s.running_score > s.best_score)
        {
          s.best_score = s.running_score;
        }
      }

      if (player == g_active_player)
      {
        g_ai_modifier -= s.best_score;
      }
      else
      {
        g_ai_modifier += s.best_score;
      }

      if (g_life[1 - player] < g_life[player] && g_basiclandtypes_controlled[player][COLOR_BLACK] != 0)
      {
        s.opponent_life_div_swamps = g_life[1 - player] / g_basiclandtypes_controlled[player][COLOR_BLACK];
        if (player == g_active_player)
        {
          g_ai_modifier -= MAX(0x18 - s.opponent_life_div_swamps, 1) * 0x18;
        }
        else
        {
          g_ai_modifier += MAX(0x18 - s.opponent_life_div_swamps, 1) * 0x18;
        }
      }
    }
  }

  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if (event == EVENT_CAST_SPELL && g_affected_card == card && g_affected_card_controller == player)
  {
    if (count_permanents_by_internal_card_id(player, PLAYER_CARD_INSTANCE(player, card).internal_card_id, player) != 0)
    {
      g_ai_modifier -= 0x60;
    }
    return 0;
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    return has_mana_w_global_cost_mod(player, card, 1, 1);
  }

  if (event == EVENT_ACTIVATE)
  {
    if (has_mana_w_global_cost_mod(player, card, 1, 1) != 0)
    {
      charge_mana_w_global_cost_mod(player, card, 1, 1);
      return 0;
    }
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    damage_player(1 - player, 1, g_card_on_stack_controller, g_card_on_stack);
    damage_player(player, 1, g_card_on_stack_controller, g_card_on_stack);
    dispatch_function_to_all_cards_in_play(g_card_on_stack_controller, g_card_on_stack, damage_creature_callback, -1);
    return 0;
  }

  if (g_trigger_condition == TRIGGER_EOT && g_affected_card == card && g_affected_card_controller == player && player == g_current_turn)
  {
    if (event == EVENT_TRIGGER)
    {
      g_event_result |= 2;
    }
    if (event == EVENT_RESOLVE_TRIGGER && has_permanent_of_type(player, TYPE_CREATURE) == 0 && has_permanent_of_type(1 - player, TYPE_CREATURE) == 0)
    {
      kill_card(player, card, KILL_DESTROY);
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0052d761
// FUNCTION: SHANDALAR 0x004e4f5c
int damage_creature_callback(int source_player, int source_card, int test_player, int test_card, int internal_card_id)
{
  if ((global_cards_data[internal_card_id].type & TYPE_CREATURE) != 0)
  {
    damage_creature(test_player, test_card, 1, source_player, source_card);
  }

  return 0;
}

// FUNCTION: MAGIC 0x0052d7a5
// FUNCTION: SHANDALAR 0x004e4fa0
int generic_creature_ability_aura(int player, int card, int event, unsigned int trigger_flag)
{
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

  if ((event == EVENT_CAST_SPELL) && (card == g_affected_card) && (player == g_affected_card_controller))
  {
    if (select_target_creature_and_store(player, player, card) == 0)
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
    if (C_real_validate_target(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                               PLAYER_CARD_INSTANCE(player, card).targets[0].card,
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
      PLAYER_CARD_INSTANCE(player, card).damage_target_player = PLAYER_CARD_INSTANCE(player, card).targets[0].player;
      PLAYER_CARD_INSTANCE(player, card).damage_target_card = PLAYER_CARD_INSTANCE(player, card).targets[0].card;
    }
    else
    {
      kill_card(player, card, KILL_BURY);
      g_spell_fizzled = 1;
    }
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
  }
  if (PLAYER_CARD_INSTANCE(player, card).damage_target_card == g_affected_card &&
      PLAYER_CARD_INSTANCE(player, card).damage_target_player == g_affected_card_controller &&
      g_affected_card != -1 &&
      (PLAYER_CARD_INSTANCE(player, card).state & STATE_INVISIBLE) == 0 &&
      event == EVENT_ABILITIES)
  {
    g_event_result |= trigger_flag;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0052dae0
// FUNCTION: SHANDALAR 0x004e52db
int card_holy_strength(int player, int card, event_t event)
{
  if ((((event == EVENT_CAST_SPELL) && (g_affected_card == card)) && (g_affected_card_controller == player)) && (g_duel_ai_mode_state != 1))
  {
    load_text("prompts.txt", "HOLY_STRENGTH");
  }

  return generic_creature_stat_aura(player, card, event, 1, 2);
}

// FUNCTION: MAGIC 0x0052db4f
// FUNCTION: SHANDALAR 0x004e534a
int card_giant_strength(int player, int card, event_t event)
{
  if ((((event == EVENT_CAST_SPELL) && (g_affected_card == card)) && (g_affected_card_controller == player)) && (g_duel_ai_mode_state != 1))
  {
    load_text("prompts.txt", "GIANT_STRENGTH");
  }
  return generic_creature_stat_aura(player, card, event, 2, 2);
}

// FUNCTION: MAGIC 0x0052dbbc
// FUNCTION: SHANDALAR 0x004e53b9
int card_immolation(int player, int card, event_t event)
{
  if ((((event == EVENT_CAST_SPELL) && (g_affected_card == card)) && (g_affected_card_controller == player)) && (g_duel_ai_mode_state != 1))
  {
    load_text("prompts.txt", "IMMOLATION");
  }
  return generic_creature_stat_aura(player, card, event, 2, -2);
}

// FUNCTION: MAGIC 0x0052dc2b
// FUNCTION: SHANDALAR 0x004e5426
int card_divine_transformation(int player, int card, event_t event)
{
  if ((((event == EVENT_CAST_SPELL) && (g_affected_card == card)) && (g_affected_card_controller == player)) && (g_duel_ai_mode_state != 1))
  {
    load_text("prompts.txt", "DIVINE_TRANSFORMATION");
  }
  return generic_creature_stat_aura(player, card, event, 3, 3);
}

// FUNCTION: MAGIC 0x0052dc98
// FUNCTION: SHANDALAR 0x004e5495
int card_unholy_strength(int player, int card, event_t event)
{
  if ((((event == EVENT_CAST_SPELL) && (g_affected_card == card)) && (g_affected_card_controller == player)) && (g_duel_ai_mode_state != 1))
  {
    load_text("prompts.txt", "UNHOLY_STRENGTH");
  }
  return generic_creature_stat_aura(player, card, event, 2, 1);
}

// FUNCTION: MAGIC 0x0052dd05
// FUNCTION: SHANDALAR 0x004e5502
int card_weakness(int player, int card, event_t event)
{
  if ((((event == EVENT_CAST_SPELL) && (g_affected_card == card)) && (g_affected_card_controller == player)) && (g_duel_ai_mode_state != 1))
  {
    load_text("prompts.txt", "WEAKNESS");
  }
  return generic_creature_stat_aura(player, card, event, -2, -1);
}

// FUNCTION: MAGIC 0x0052dd74
// FUNCTION: SHANDALAR 0x004e5571
int generic_creature_stat_aura(int player, int card, event_t event, int power_modifier, int toughness_modifier)
{
  int preferred_controller;

  if (event == EVENT_CAN_CAST)
  {
    if ((player == g_active_player) || ((g_duel_network_flags & 2) != 0))
    {
      return real_target_available((int *)0,
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
                                   0,
                                   0);
    }
    else if (toughness_modifier + power_modifier >= 0)
    {
      return real_target_available((int *)0,
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
                                   0,
                                   0);
    }
    else
    {
      return real_target_available((int *)0,
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
                                   0,
                                   0);
    }
  }

  if (((event == EVENT_CAST_SPELL) && (card == g_affected_card)) && (player == g_affected_card_controller))
  {
    if (toughness_modifier + power_modifier >= 0)
    {
      preferred_controller = player;
    }
    else
    {
      preferred_controller = 1 - player;
    }

    if (select_target_creature_and_store(player, preferred_controller, card) == 0)
    {
      g_spell_fizzled = 1;
    }
    else
    {
      g_ai_modifier += -0x18;
    }
    return 0;
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    if (C_real_validate_target(global_card_instances[player][card].targets[0].player,
                               global_card_instances[player][card].targets[0].card,
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
      global_card_instances[player][card].damage_target_player =
          (char)global_card_instances[player][card].targets[0].player;
      global_card_instances[player][card].damage_target_card =
          global_card_instances[player][card].targets[0].card;
    }
    else
    {
      kill_card(player, card, KILL_BURY);
      g_spell_fizzled = 1;
    }
    global_card_instances[player][card].number_of_targets = 0;
    return 0;
  }

  if (((event == EVENT_POWER) && (global_card_instances[player][card].damage_target_card == g_affected_card)) && (((int)global_card_instances[player][card].damage_target_player == g_affected_card_controller) && (g_affected_card != -1)))
  {
    if (is_in_play(player, card))
    {
      g_event_result += power_modifier;
    }
  }
  if (((event == EVENT_TOUGHNESS) && (global_card_instances[player][card].damage_target_card == g_affected_card)) && (((int)global_card_instances[player][card].damage_target_player == g_affected_card_controller) && (g_affected_card != -1)))
  {
    if (is_in_play(player, card))
    {
      g_event_result += toughness_modifier;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0052e233
// FUNCTION: SHANDALAR 0x004e5a30
int card_castle(int player, int card, event_t event)
{
  if ((event == EVENT_TOUGHNESS) && is_in_play(player, card))
  {
    if ((player == g_affected_card_controller) && ((PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).state & (STATE_TAPPED | STATE_ATTACKING)) == 0) && is_in_play(g_affected_card_controller, g_affected_card))
    {
      g_event_result += 2;
    }
    return 0;
  }

  if (event == EVENT_CAN_CAST)
    return 1;

  return 0;
}

// FUNCTION: MAGIC 0x0052e2ed
// FUNCTION: SHANDALAR 0x004e5aea
int card_black_ward(int player, int card, event_t event)
{
  return helper_ward(player, card, event, get_sleighted_color(player, card, COLOR_BLACK));
}

// FUNCTION: MAGIC 0x0052e324
// FUNCTION: SHANDALAR 0x004e5b21
int card_green_ward(int player, int card, event_t event)
{
  return helper_ward(player, card, event, get_sleighted_color(player, card, COLOR_GREEN));
}

// FUNCTION: MAGIC 0x0052e35b
// FUNCTION: SHANDALAR 0x004e5b58
int card_blue_ward(int player, int card, event_t event)
{
  return helper_ward(player, card, event, get_sleighted_color(player, card, COLOR_BLUE));
}

// FUNCTION: MAGIC 0x0052e392
// FUNCTION: SHANDALAR 0x004e5b8f
int card_red_ward(int player, int card, event_t event)
{
  return helper_ward(player, card, event, get_sleighted_color(player, card, COLOR_RED));
}

// FUNCTION: MAGIC 0x0052e3c9
// FUNCTION: SHANDALAR 0x004e5bc6
int card_white_ward(int player, int card, event_t event)
{
  return helper_ward(player, card, event, get_sleighted_color(player, card, COLOR_WHITE));
}

// FUNCTION: MOK 0x004bc490
// FUNCTION: MAGIC 0x0052e400
// FUNCTION: SHANDALAR 0x004e5bfd
int helper_ward(int player, int card, event_t event, int color)
{
  unsigned int ward_color;
  int aura_count;
  int current_card;
  int test_player;
  int old_event_result;
  int protection_color;

  ward_color = 1 << ((unsigned char)color);

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
    if (event == EVENT_CAST_SPELL && card == g_affected_card && player == g_affected_card_controller)
    {
      if (g_duel_ai_mode_state != 1)
      {
        load_text("prompts.txt", "ANY_WARD");
      }

      if (!select_target_creature_and_store(player, player, card))
      {
        g_spell_fizzled = 1;
      }
      else
      {
        if (player == g_other_player && (g_duel_network_flags & 2) == 0 && is_selected_target_already_attached(player, card, PLAYER_CARD_INSTANCE(player, card).internal_card_id) != 0)
        {
          g_ai_modifier -= 0x60;
        }
        if (PLAYER_CARD_INSTANCE(player, card).damage_target_player == g_other_player)
        {
          aura_count = g_creature_power_by_color[g_active_player][color];
          g_ai_modifier += (aura_count + 1) * C_get_abilities(PLAYER_CARD_INSTANCE(player, card).damage_target_player, PLAYER_CARD_INSTANCE(player, card).damage_target_card, EVENT_POWER, -1) * 3;
        }
        if (PLAYER_CARD_INSTANCE(player, card).damage_target_player == g_active_player)
        {
          g_ai_modifier -= 0x60;
        }
      }
    }

    if (event == EVENT_RESOLVE_SPELL)
    {
      if (C_real_validate_target(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                                 PLAYER_CARD_INSTANCE(player, card).targets[0].card,
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
        PLAYER_CARD_INSTANCE(player, card).damage_target_player = PLAYER_CARD_INSTANCE(player, card).targets[0].player;
        PLAYER_CARD_INSTANCE(player, card).damage_target_card = PLAYER_CARD_INSTANCE(player, card).targets[0].card;
      }
      else
      {
        kill_card(player, card, KILL_BURY);
        g_spell_fizzled = 1;
      }
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    }

    if (PLAYER_CARD_INSTANCE(player, card).damage_target_card != -1)
    {
      for (test_player = 0; test_player < 2; ++test_player)
      {
        for (current_card = 0; current_card < g_active_cards_count[test_player]; ++current_card)
        {
          old_event_result = g_event_result;
          if (is_in_play(test_player, current_card) && PLAYER_CARD_INSTANCE(test_player, current_card).damage_target_card == PLAYER_CARD_INSTANCE(player, card).damage_target_card && PLAYER_CARD_INSTANCE(test_player, current_card).damage_target_player == PLAYER_CARD_INSTANCE(player, card).damage_target_player && (unsigned char)PLAYER_CARD_INSTANCE(test_player, current_card).color == ward_color && (player != test_player || card != current_card) && (global_cards_data[PLAYER_CARD_INSTANCE(test_player, current_card).internal_card_id].type & TYPE_ENCHANTMENT) != 0)
          {
            kill_card(test_player, current_card, KILL_BURY);
          }
          g_event_result = old_event_result;
        }
      }
    }

    if (PLAYER_CARD_INSTANCE(player, card).damage_target_card == g_affected_card && PLAYER_CARD_INSTANCE(player, card).damage_target_player == g_affected_card_controller && g_affected_card != -1 && (PLAYER_CARD_INSTANCE(player, card).state & STATE_INVISIBLE) == 0 && event == EVENT_ABILITIES)
    {
      protection_color = get_sleighted_color(player, card, color);
      g_event_result |= 0x800 << (((unsigned char)protection_color - 1));
    }

    if (event == EVENT_CAST_SPELL && PLAYER_CARD_INSTANCE(player, card).damage_target_player == PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).damage_target_player && PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).damage_target_card == PLAYER_CARD_INSTANCE(player, card).damage_target_card && (ward_color & (unsigned char)PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).color) != 0 && (PLAYER_CARD_INSTANCE(player, card).state & STATE_INVISIBLE) == 0)
    {
      g_event_result = 1;
    }

    if (event == EVENT_SHOULD_AI_PLAY && g_current_phase == EVENT_CLEANUP && card == g_affected_card && player == g_affected_card_controller)
    {
      test_player = 1 - player;
      aura_count = 0;
      for (current_card = 0; current_card < g_active_cards_count[test_player]; ++current_card)
      {
        if ((ward_color & (unsigned char)PLAYER_CARD_INSTANCE(test_player, current_card).color) != 0 && is_in_play(test_player, current_card))
        {
          aura_count += 0x18;
        }
      }

      if (aura_count != 0)
      {
        g_ai_modifier += aura_count + 0x18;
      }
    }

    return 0;
  }
}

// FUNCTION: MAGIC 0x0052ed45
// FUNCTION: SHANDALAR 0x004e6541
int card_unstable_mutation(int player, int card, event_t event)
{
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
                                 -1,
                                 -1,
                                 0,
                                 0,
                                 0);
  }

  if (event == EVENT_CAST_SPELL && g_affected_card == card && g_affected_card_controller == player)
  {
    if (g_duel_ai_mode_state != 1)
    {
      load_text("prompts.txt", "UNSTABLE_MUTATION");
    }
    g_spell_fizzled = (select_target_creature_and_store(player, player, card) == 0);
    if (g_spell_fizzled != 1 &&
        g_other_player == player &&
        (TARGET_CARD_INSTANCE(player, card, 0).state &
         STATE_SUMMONSICK_BOTH) != 0)
    {
      g_ai_modifier -= 99;
    }
    return 0;
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    if (C_real_validate_target(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                               PLAYER_CARD_INSTANCE(player, card).targets[0].card,
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
      PLAYER_CARD_INSTANCE(player, card).damage_target_player = PLAYER_CARD_INSTANCE(player, card).targets[0].player;
      PLAYER_CARD_INSTANCE(player, card).damage_target_card = PLAYER_CARD_INSTANCE(player, card).targets[0].card;
    }
    else
    {
      kill_card(player, card, KILL_BURY);
      g_spell_fizzled = 1;
    }
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    return 0;
  }

  if ((event == EVENT_POWER || event == EVENT_TOUGHNESS) &&
      is_in_play(player, card) &&
      PLAYER_CARD_INSTANCE(player, card).damage_target_card == g_affected_card &&
      PLAYER_CARD_INSTANCE(player, card).damage_target_player == g_affected_card_controller &&
      g_affected_card != -1)
  {
    g_event_result += 3;
    return 0;
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    if (g_current_phase == EVENT_UPKEEP_PHASE &&
        g_event_player == g_current_player &&
        PLAYER_CARD_INSTANCE(player, card).damage_target_player == g_current_player &&
        (PLAYER_CARD_INSTANCE(player, card).info_slot & 1) == 0)
    {
      PLAYER_CARD_INSTANCE(player, card).upkeep_flags |= 0x101;
      g_activation_event_flags |= 3;
      return 1;
    }
    else
    {
      return 0;
    }
  }

  if (event == EVENT_UPKEEP_PHASE && g_affected_card == card && g_affected_card_controller == player)
  {
    PLAYER_CARD_INSTANCE(player, card).info_slot |= 1;
    g_upkeep_payment_completed = 1;
    g_event_result |= 1;
    return 0;
  }

  if (event == EVENT_UPKEEP_COSTS_UNPAID)
  {
    --DAMAGE_TARGET_CARD_INSTANCE(player, card).counter_power;
    --DAMAGE_TARGET_CARD_INSTANCE(player, card).counter_toughness;
    DAMAGE_TARGET_CARD_INSTANCE(player, card).special_counters += 0x10000;
    if (g_duel_ai_mode_state != 1)
    {
      play_sound_effect(0x25);
    }
    return 0;
  }

  if (event == EVENT_CLEANUP)
  {
    PLAYER_CARD_INSTANCE(player, card).info_slot &= ~1;
  }

  if (event == EVENT_SHOULD_AI_PLAY)
  {
    DAMAGE_TARGET_CARD_INSTANCE(player, card).counter_power -= 2;
    DAMAGE_TARGET_CARD_INSTANCE(player, card).counter_toughness -= 2;
    DAMAGE_TARGET_CARD_INSTANCE(player, card).regen_status |= 0x6000000;
    if (g_other_player == player &&
        g_other_player == g_current_player &&
        (DAMAGE_TARGET_CARD_INSTANCE(player, card).state &
         0x40) == 0)
    {
      g_ai_modifier -= 0x3c;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0052f80a
// FUNCTION: SHANDALAR 0x004e7006
int card_copy_artifact(int player, int card, event_t event)
{
  target_t selected_target;
  int copied_internal_id;
  int special_copy;

  if (event == EVENT_CAN_CAST)
  {
    return real_target_available((int *)0,
                                 TARGET_SCAN_DIRECT,
                                 player,
                                 2,
                                 2,
                                 TARGET_ZONE_IN_PLAY,
                                 TYPE_ARTIFACT,
                                 TYPE_NONE,
                                 0,
                                 get_protections_from(player, card),
                                 0,
                                 0,
                                 -1,
                                 -1,
                                 0xffffffff,
                                 0xffffffff,
                                 TARGET_SPECIAL_USE_ORIGINAL_TYPE,
                                 0,
                                 0);
  }

  if (event == EVENT_CAST_SPELL && card == g_affected_card && player == g_affected_card_controller)
  {
    if (g_duel_ai_mode_state != 1)
    {
      load_text("prompts.txt", "COPY_ARTIFACT");
    }

    if (C_real_select_target(player,
                             2,
                             2,
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
                             TARGET_SPECIAL_USE_ORIGINAL_TYPE,
                             0,
                             0,
                             g_text_lines[0],
                             1,
                             &selected_target))
    {
      PLAYER_CARD_INSTANCE(player, card)
          .targets[PLAYER_CARD_INSTANCE(player, card).number_of_targets]
          .player = selected_target.player;
      PLAYER_CARD_INSTANCE(player, card)
          .targets[PLAYER_CARD_INSTANCE(player, card).number_of_targets]
          .card = selected_target.card;
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
    }
    else
    {
      g_spell_fizzled = 1;
    }

    return 0;
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    if (C_real_validate_target(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                               PLAYER_CARD_INSTANCE(player, card).targets[0].card,
                               (char *)0,
                               player,
                               2,
                               2,
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
                               TARGET_SPECIAL_USE_ORIGINAL_TYPE,
                               0,
                               0))
    {
      special_copy = 0;
      if (
          global_cards_data[TARGET_CARD_INSTANCE(player, card, 0).original_internal_card_id]
                  .id ==
              CARD_ID_PRIMAL_CLAY ||
          global_cards_data[TARGET_CARD_INSTANCE(player, card, 0).original_internal_card_id]
                  .id ==
              CARD_ID_SHAPESHIFTER)
      {
        special_copy = 1;
      }

      if (special_copy)
      {
        copied_internal_id =
            create_a_card_type(TARGET_CARD_INSTANCE(player, card, 0).internal_card_id);
      }
      else
      {
        copied_internal_id =
            create_a_card_type(TARGET_CARD_INSTANCE(player, card, 0).original_internal_card_id);
      }

      if (copied_internal_id != -1)
      {
        PLAYER_CARD_INSTANCE(player, card).internal_card_id = PLAYER_CARD_INSTANCE(player, card).dummy3 = copied_internal_id;
        PLAYER_CARD_INSTANCE(player, card).regen_status = KEYWORD_RECALC_CHANGE_TYPE;
        global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].type |= TYPE_ENCHANTMENT;
      }

      PLAYER_CARD_INSTANCE(player, card).color =
          TARGET_CARD_INSTANCE(player, card, 0).color;
      if (special_copy)
      {
        if ((global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].type & TYPE_CREATURE) !=
            0)
        {
          ++g_duel_summary.creature_counts[player];
        }
        if ((global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].type & TYPE_ARTIFACT) !=
            0)
        {
          ++g_duel_summary.artifact_counts[player];
        }
        if ((global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].type &
             TYPE_ENCHANTMENT) != 0)
        {
          ++g_duel_summary.enchantment_counts[player];
        }
        g_card_types_in_play[player] |=
            (unsigned char)global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].type;
        PLAYER_CARD_INSTANCE(player, card).state |=
            ((player == 0) ? 0 : STATE_POWER_STRUGGLE) | STATE_SUMMONSICK |
            STATE_SUMMONSICK_NOATTACK | STATE_SUMMONSICK_NOTAP | STATE_IN_PLAY;
        g_trigger_cause_controller = player;
        g_trigger_cause = card;
        dispatch_trigger_twice_once_with_each_player_as_reason(g_current_player,
                                                               TRIGGER_COMES_INTO_PLAY,
                                                               gs_card_into_play_0091c840,
                                                               0);
      }
      else
      {
        process_card_enters_play(player, card);
      }
    }
    else
    {
      kill_card(player, card, KILL_BURY);
      g_spell_fizzled = 1;
    }

    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    return 0;
  }

  if (event == EVENT_CHANGE_TYPE && (g_land_can_be_played & LCBP_DURING_EVENT_CHANGE_TYPE_SECOND_PASS) == 0 &&
      card == g_affected_card && player == g_affected_card_controller && is_in_play(player, card))
  {
    g_event_result = PLAYER_CARD_INSTANCE(player, card).dummy3;
  }

  return 0;
}

// FUNCTION: MAGIC 0x005301b4
// FUNCTION: SHANDALAR 0x004e79b0
int card_warp_artifact(int player, int card, event_t event)
{
  target_t selected_target;

  if (event == EVENT_CAN_CAST)
  {
    return real_target_available((int *)0,
                                 TARGET_SCAN_DIRECT,
                                 player,
                                 2,
                                 2,
                                 0x200,
                                 0x40,
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

  if (((event == EVENT_CAST_SPELL) && (card == g_affected_card)) && (player == g_affected_card_controller))
  {
    if (g_duel_ai_mode_state != 1)
    {
      load_text("prompts.txt", "WARP_ARTIFACT");
    }
    if (C_real_select_target(player,
                             2,
                             1 - player,
                             TARGET_ZONE_IN_PLAY,
                             TYPE_ARTIFACT,
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
                             0,
                             0,
                             g_text_lines[0],
                             1,
                             &selected_target))
    {
      SET_TARGET(PLAYER_CARD_INSTANCE(player, card).targets[0], selected_target);
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
      if (selected_target.player == g_active_player)
      {
        g_ai_modifier -=
            ((char)global_cards_data[PLAYER_CARD_INSTANCE(selected_target.player, selected_target.card)
                                         .internal_card_id]
                     .cc[1] *
                 3 +
             3) *
            4;
      }
      else
      {
        g_ai_modifier += -0x60;
      }
    }
    else
    {
      g_spell_fizzled = 1;
    }
    return 0;
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
                               0))
    {
      PLAYER_CARD_INSTANCE(player, card).damage_target_player =
          (char)PLAYER_CARD_INSTANCE(player, card).targets[0].player;
      PLAYER_CARD_INSTANCE(player, card).damage_target_card =
          PLAYER_CARD_INSTANCE(player, card).targets[0].card;
    }
    else
    {
      kill_card(player, card, KILL_BURY);
      g_spell_fizzled = 1;
    }
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    return 0;
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    if (((g_current_phase == EVENT_UPKEEP_PHASE) && (g_event_player == g_current_player)) && (PLAYER_CARD_INSTANCE(player, card).damage_target_player == g_current_player) && ((PLAYER_CARD_INSTANCE(player, card).info_slot & 1) == 0))
    {
      PLAYER_CARD_INSTANCE(player, card).upkeep_flags |= 0x101;
      g_activation_event_flags |= 3;
      return 1;
    }
    else
    {
      return 0;
    }
  }

  if (((event == EVENT_UPKEEP_PHASE) && (g_affected_card == card)) && (g_affected_card_controller == player))
  {
    PLAYER_CARD_INSTANCE(player, card).info_slot |= 1;
    g_upkeep_payment_completed = 1;
    g_event_result |= 1;
    return 0;
  }

  if (event == EVENT_UPKEEP_COSTS_UNPAID)
  {
    damage_player((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                  1,
                  g_card_on_stack_controller,
                  g_card_on_stack);
    return 0;
  }

  if (event == EVENT_CLEANUP)
  {
    PLAYER_CARD_INSTANCE(player, card).info_slot &= ~STATE_JUST_DRAWED;
  }

  if ((event == EVENT_SHOULD_AI_PLAY) && is_in_play(player, card) && (PLAYER_CARD_INSTANCE(player, card).damage_target_player != -1))
  {
    if (PLAYER_CARD_INSTANCE(player, card).damage_target_player == g_active_player)
    {
      g_ai_modifier += MAX(0x18 - g_life[PLAYER_CARD_INSTANCE(player, card).damage_target_player], 1) * 0x18;
    }
    else
    {
      g_ai_modifier -= -(-MAX(0x18 - g_life[PLAYER_CARD_INSTANCE(player, card).damage_target_player], 1) * 0x18);
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x005308b5
// FUNCTION: SHANDALAR 0x004e80b1
int card_power_struggle(int player, int card, event_t event)
{
  struct
  {
    int chosen_index;
    char prompt[500];
    int chosen_cards[2];
    int attempts;
    int selection_index;
    int loop_player;
    int found_pair;
    int counts[2];
    int candidates[2][150];
    int current_card;
  } s;

  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if (event == EVENT_CAST_SPELL && card == g_affected_card && player == g_affected_card_controller)
  {
    g_ai_modifier += 0x18;
    return 0;
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    if (g_current_phase == EVENT_UPKEEP_PHASE &&
        (PLAYER_CARD_INSTANCE(player, card).info_slot & 1) == 0 &&
        g_event_player == g_current_player)
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
    return 0;
  }

  if (event == EVENT_UPKEEP_COSTS_UNPAID)
  {
    s.found_pair = 0;
    for (s.loop_player = 0; s.loop_player < 2; ++s.loop_player)
    {
      s.selection_index = 0;
      for (s.current_card = 0; s.current_card < g_active_cards_count[s.loop_player]; ++s.current_card)
      {
        if (is_in_play(s.loop_player, s.current_card) != 0 &&
            (global_cards_data[PLAYER_CARD_INSTANCE(s.loop_player, s.current_card).internal_card_id].type &
             (TYPE_LAND | TYPE_CREATURE | TYPE_ARTIFACT)) != 0 &&
            (PLAYER_CARD_INSTANCE(s.loop_player, s.current_card).regen_status &
             get_protections_from(g_card_on_stack_controller, g_card_on_stack)) == 0)
        {
          s.candidates[s.loop_player][s.selection_index] = s.current_card;
          ++s.selection_index;
        }
      }
      s.counts[s.loop_player] = s.selection_index;
    }

    if (s.counts[g_current_player] < 1)
    {
      g_spell_fizzled = 1;
    }
    else
    {
      if ((g_duel_network_flags & 2) == 0)
      {
        s.chosen_index = internal_rand(s.counts[g_current_player]);
      }
      else
      {
        s.chosen_index = network_random(g_card_on_stack_controller, s.counts[g_current_player]);
      }
      s.chosen_cards[g_current_player] = s.candidates[g_current_player][s.chosen_index];

      s.found_pair = 0;
      s.attempts = 0;
      if ((g_duel_network_flags & 2) == 0)
      {
        s.selection_index = internal_rand(s.counts[1 - g_current_player]);
      }
      else
      {
        s.selection_index = network_random(g_card_on_stack_controller, s.counts[1 - g_current_player]);
      }
      while (s.found_pair == 0 && s.attempts < s.counts[1 - g_current_player])
      {
        s.chosen_cards[1 - g_current_player] = s.candidates[1 - g_current_player][s.selection_index];
        if ((global_cards_data[PLAYER_CARD_INSTANCE(g_current_player, s.chosen_cards[g_current_player]).internal_card_id].type &
             global_cards_data[PLAYER_CARD_INSTANCE(1 - g_current_player, s.chosen_cards[1 - g_current_player]).internal_card_id].type) == 0)
        {
          s.selection_index = (s.selection_index + 1) % s.counts[1 - g_current_player];
          ++s.attempts;
        }
        else
        {
          s.found_pair = 1;
        }
      }
      if (s.found_pair != 1)
      {
        g_spell_fizzled = 1;
      }
    }

    if (s.found_pair == 1 && g_spell_fizzled != 1)
    {
      load_text("prompts.txt", "POWER_STRUGGLE");
      sprintf(s.prompt,
              g_text_lines[0],
              get_displayed_card_name(g_current_player, s.chosen_cards[g_current_player]),
              get_displayed_card_name(1 - g_current_player, s.chosen_cards[1 - g_current_player]));
      do_dialog(player, player, card, -1, -1, s.prompt, 0);
      power_struggle_exchange_permanents(g_current_player,
                                         s.chosen_cards[g_current_player],
                                         1 - g_current_player,
                                         s.chosen_cards[1 - g_current_player]);
    }
    if (g_spell_fizzled != 1)
    {
      play_sound_effect(0x36);
    }
    return 0;
  }

  if (event == EVENT_CLEANUP)
  {
    PLAYER_CARD_INSTANCE(player, card).info_slot &= ~1;
  }

  return 0;
}

// FUNCTION: SHANDALAR 0x004d45d9
int power_struggle_exchange_permanents(int first_player, int first_card, int second_player, int second_card)
{
  struct
  {
    int target_index;     /* [ebp-0x1c] */
    int test_card;        /* [ebp-0x18] */
    int second_timestamp; /* [ebp-0x14] */
    int first_timestamp;  /* [ebp-0x10] */
    int test_player;      /* [ebp-0x0c] */
    int new_second_card;  /* [ebp-0x08] */
    int new_first_card;   /* [ebp-0x04] */
  } s;

  s.first_timestamp = PLAYER_CARD_INSTANCE(first_player, first_card).timestamp;
  s.second_timestamp = PLAYER_CARD_INSTANCE(second_player, second_card).timestamp;
  s.new_first_card = add_card_to_hand(first_player, PLAYER_CARD_INSTANCE(second_player, second_card).internal_card_id);
  if (s.new_first_card == -1)
  {
    return 0;
  }

  s.new_second_card = add_card_to_hand(second_player, PLAYER_CARD_INSTANCE(first_player, first_card).internal_card_id);
  if (s.new_second_card == -1)
  {
    PLAYER_CARD_INSTANCE(first_player, s.new_first_card).internal_card_id = -1;
    return 0;
  }

  memcpy(&PLAYER_CARD_INSTANCE(first_player, s.new_first_card),
         &PLAYER_CARD_INSTANCE(second_player, second_card),
         sizeof(card_instance_t));
  memcpy(&PLAYER_CARD_INSTANCE(second_player, s.new_second_card),
         &PLAYER_CARD_INSTANCE(first_player, first_card),
         sizeof(card_instance_t));

  PLAYER_CARD_INSTANCE(first_player, s.new_first_card).state |= STATE_SUMMONSICK_BOTH;
  PLAYER_CARD_INSTANCE(second_player, s.new_second_card).state |= STATE_SUMMONSICK_BOTH;
  PLAYER_CARD_INSTANCE(first_player, s.new_first_card).state &= ~(STATE_ATTACKING | STATE_BLOCKING);
  PLAYER_CARD_INSTANCE(second_player, s.new_second_card).state &= ~(STATE_ATTACKING | STATE_BLOCKING);

  TENTATIVE_timestamp_player[s.first_timestamp] = second_player;
  TENTATIVE_timestamp_card[s.first_timestamp] = s.new_second_card;
  TENTATIVE_timestamp_player[s.second_timestamp] = first_player;
  TENTATIVE_timestamp_card[s.second_timestamp] = s.new_first_card;

  PLAYER_CARD_INSTANCE(second_player, s.new_second_card).state |= STATE_POWER_STRUGGLE;

  for (s.test_player = 0; s.test_player < 2; ++s.test_player)
  {
    for (s.test_card = 0; s.test_card < g_active_cards_count[s.test_player]; ++s.test_card)
    {
      if ((int)(char)PLAYER_CARD_INSTANCE(s.test_player, s.test_card).damage_target_player == first_player &&
          PLAYER_CARD_INSTANCE(s.test_player, s.test_card).damage_target_card == first_card)
      {
        PLAYER_CARD_INSTANCE(s.test_player, s.test_card).damage_target_player = (char)second_player;
        PLAYER_CARD_INSTANCE(s.test_player, s.test_card).damage_target_card = s.new_second_card;
      }
      if ((int)(char)PLAYER_CARD_INSTANCE(s.test_player, s.test_card).damage_source_player == first_player &&
          PLAYER_CARD_INSTANCE(s.test_player, s.test_card).damage_source_card == first_card)
      {
        PLAYER_CARD_INSTANCE(s.test_player, s.test_card).damage_source_player = (char)second_player;
        PLAYER_CARD_INSTANCE(s.test_player, s.test_card).damage_source_card = s.new_second_card;
      }
      if (PLAYER_CARD_INSTANCE(s.test_player, s.test_card).number_of_targets != 0)
      {
        for (s.target_index = 0;
             s.target_index < PLAYER_CARD_INSTANCE(s.test_player, s.test_card).number_of_targets;
             ++s.target_index)
        {
          if (PLAYER_CARD_INSTANCE(s.test_player, s.test_card).targets[s.target_index].player == first_player &&
              PLAYER_CARD_INSTANCE(s.test_player, s.test_card).targets[s.target_index].card == first_card)
          {
            PLAYER_CARD_INSTANCE(s.test_player, s.test_card).targets[s.target_index].player = second_player;
            PLAYER_CARD_INSTANCE(s.test_player, s.test_card).targets[s.target_index].card = s.new_second_card;
          }
        }
      }

      if ((int)(char)PLAYER_CARD_INSTANCE(s.test_player, s.test_card).damage_target_player == second_player &&
          PLAYER_CARD_INSTANCE(s.test_player, s.test_card).damage_target_card == second_card)
      {
        PLAYER_CARD_INSTANCE(s.test_player, s.test_card).damage_target_player = (char)first_player;
        PLAYER_CARD_INSTANCE(s.test_player, s.test_card).damage_target_card = s.new_first_card;
      }
      if ((int)(char)PLAYER_CARD_INSTANCE(s.test_player, s.test_card).damage_source_player == second_player &&
          PLAYER_CARD_INSTANCE(s.test_player, s.test_card).damage_source_card == second_card)
      {
        PLAYER_CARD_INSTANCE(s.test_player, s.test_card).damage_source_player = (char)first_player;
        PLAYER_CARD_INSTANCE(s.test_player, s.test_card).damage_source_card = s.new_first_card;
      }
      if (PLAYER_CARD_INSTANCE(s.test_player, s.test_card).number_of_targets != 0)
      {
        for (s.target_index = 0;
             s.target_index < PLAYER_CARD_INSTANCE(s.test_player, s.test_card).number_of_targets;
             ++s.target_index)
        {
          if (PLAYER_CARD_INSTANCE(s.test_player, s.test_card).targets[s.target_index].player == second_player &&
              PLAYER_CARD_INSTANCE(s.test_player, s.test_card).targets[s.target_index].card == second_card)
          {
            PLAYER_CARD_INSTANCE(s.test_player, s.test_card).targets[s.target_index].player = first_player;
            PLAYER_CARD_INSTANCE(s.test_player, s.test_card).targets[s.target_index].card = s.new_first_card;
          }
        }
      }
    }
  }

  if ((g_battlefield_extra_ability_flags & EA_CONTROLLED) != 0)
  {
    dispatch_event(first_player, s.new_first_card, EVENT_CARDCONTROLLED);
    dispatch_event(second_player, s.new_second_card, EVENT_CARDCONTROLLED);
  }

  PLAYER_CARD_INSTANCE(first_player, first_card).internal_card_id = -1;
  PLAYER_CARD_INSTANCE(second_player, second_card).internal_card_id = -1;
  reassess_all_cards_and_mana();
  return 1;
}

// FUNCTION: MAGIC 0x00530f1a
// FUNCTION: SHANDALAR 0x004e8714
int card_necropolis_of_azar(int player, int card, event_t event)
{
  int spawn_card;

  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    if (C_get_special_counters(player, card) != 0 &&
        has_mana_w_global_cost_mod(player, card, COLOR_ANY, 5) != 0)
    {
      if (player == g_other_player && (g_duel_network_flags & 2) == 0 && g_ai_speculation_depth > 0)
      {
        g_activation_event_flags |= 3;
      }
      return 1;
    }
    else
    {
      return 0;
    }
  }

  if (event == EVENT_ACTIVATE && has_mana_w_global_cost_mod(player, card, COLOR_ANY, 5) != 0)
  {
    charge_mana_w_global_cost_mod(player, card, COLOR_COLORLESS, 5);
    if (g_spell_fizzled != 1)
    {
      remove_special_counter(player, card);
      if (g_ai_speculation_depth > 0)
      {
        --g_ai_speculation_depth;
      }
    }
    return 0;
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    if (g_duel_ai_mode_state != 1)
    {
      play_sound_effect(0x32);
    }
    spawn_card = add_card_to_hand(player, find_internal_card_id_by_csv_id(CARD_ID_SPAWN_OF_AZAR));
    if (spawn_card != -1)
    {
      process_card_enters_play(player, spawn_card);
      PLAYER_CARD_INSTANCE(player, spawn_card).color =
          (char)(2 << ((unsigned char)(get_sleighted_color(player, card, COLOR_BLACK) - 1)));
      PLAYER_CARD_INSTANCE(player, spawn_card).token_status |= STATUS_TOKEN;
      if ((PLAYER_CARD_INSTANCE(player, card).token_status & STATUS_HACKED) != 0)
      {
        PLAYER_CARD_INSTANCE(player, spawn_card).token_status |= STATUS_HACKED;
        PLAYER_CARD_INSTANCE(player, spawn_card).hack_mode[COLOR_BLACK] = PLAYER_CARD_INSTANCE(player, card).hack_mode[COLOR_BLACK];
      }
      PLAYER_CARD_INSTANCE(player, spawn_card).display_pic_info =
          global_cards_data[PLAYER_CARD_INSTANCE(player, card).original_internal_card_id].id;
      if ((g_duel_network_flags & 2) != 0)
      {
        PLAYER_CARD_INSTANCE(player, spawn_card).counter_power += network_random(g_card_on_stack_controller, 3);
        PLAYER_CARD_INSTANCE(player, spawn_card).counter_toughness += network_random(g_card_on_stack_controller, 3);
      }
      else
      {
        PLAYER_CARD_INSTANCE(player, spawn_card).counter_power += internal_rand(3);
        PLAYER_CARD_INSTANCE(player, spawn_card).counter_toughness += internal_rand(3);
      }
    }
    return 0;
  }

  if (event == EVENT_GRAVEYARD_FROM_PLAY)
  {
    if ((global_cards_data[PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).internal_card_id].type & TYPE_CREATURE) != 0 &&
        (PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).state & STATE_INVISIBLE) == 0 &&
        PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).kill_code != KILL_REMOVE &&
        ((int)(char)PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).color &
         (2 << ((unsigned char)(get_sleighted_color(player, card, COLOR_BLACK) - 1)))) == 0)
    {
      add_special_counter(player, card);
    }
  }

  return 0;
}
// FUNCTION: MAGIC 0x0053152d
// FUNCTION: SHANDALAR 0x004e8d27
int card_regeneration(int player, int card, event_t event)
{
  int can_activate;

  if (event == EVENT_UNTAP_PHASE)
  {
    g_ai_mana_demand_by_color[player][COLOR_GREEN] += 2;
  }

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

  if (event == EVENT_CAST_SPELL && card == g_affected_card && player == g_affected_card_controller)
  {
    if (g_duel_ai_mode_state != 1)
    {
      load_text("prompts.txt", "REGENERATION");
    }

    if (!select_target_creature_and_store(player, player, card))
    {
      g_spell_fizzled = 1;
    }
    else
    {
      g_spell_fizzled = 0;
    }

    if (g_spell_fizzled != 1 && player == g_other_player && (g_duel_network_flags & 2) == 0)
    {
      if ((TARGET_CARD_INSTANCE(player, card, 0).regen_status &
           0x200) != 0 ||
          PLAYER_CARD_INSTANCE(player, card).targets[0].player == g_active_player)
      {
        g_ai_modifier -= 0x30;
      }
      if (is_selected_target_already_attached(player, card, PLAYER_CARD_INSTANCE(player, card).internal_card_id) != 0)
      {
        g_ai_modifier -= 0x60;
      }
    }
    return 0;
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    if (C_real_validate_target(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                               PLAYER_CARD_INSTANCE(player, card).targets[0].card,
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
      PLAYER_CARD_INSTANCE(player, card).damage_target_player = PLAYER_CARD_INSTANCE(player, card).targets[0].player;
      PLAYER_CARD_INSTANCE(player, card).damage_target_card = PLAYER_CARD_INSTANCE(player, card).targets[0].card;
    }
    else
    {
      kill_card(player, card, KILL_BURY);
      g_spell_fizzled = 1;
    }
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    return 0;
  }

  if (event == EVENT_CAN_ACTIVATE && (g_land_can_be_played & LCBP_REGENERATION) != 0 && PLAYER_CARD_INSTANCE(player, card).info_slot == 0)
  {
    can_activate = 1;
    if ((DAMAGE_TARGET_CARD_INSTANCE(player, card).state &
         0x800002) != STATE_IN_PLAY)
    {
      can_activate = 0;
    }
    if (DAMAGE_TARGET_CARD_INSTANCE(player, card).kill_code != KILL_DESTROY)
    {
      can_activate = 0;
    }
    if (has_mana_w_global_cost_mod(player, card, 3, 1) == 0)
    {
      can_activate = 0;
    }
    return can_activate ? 99 : 0;
  }

  if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_target(0);
    return 0;
  }

  if (event == EVENT_ACTIVATE && (g_land_can_be_played & LCBP_REGENERATION) != 0)
  {
    charge_mana_w_global_cost_mod(player, card, 3, 1);
    if (g_spell_fizzled != 1)
    {
      g_upkeep_payment_completed = 1;
      ++PLAYER_CARD_INSTANCE(player, card).info_slot;
    }
    return 0;
  }

  if (event == EVENT_RESOLVE_ACTIVATION && (g_land_can_be_played & LCBP_REGENERATION) != 0)
  {
    PARENT_CARD_INSTANCE(player, card).info_slot = 0;
    regenerate_card(PARENT_CARD_INSTANCE(player, card).damage_target_player,
                    PARENT_CARD_INSTANCE(player, card).damage_target_card);
  }

  return 0;
}

// FUNCTION: MAGIC 0x00531d34
// FUNCTION: SHANDALAR 0x004e9531
int card_eternal_warrior(int player, int card, event_t event)
{

  if (PLAYER_CARD_INSTANCE(player, card).damage_target_card == g_affected_card &&
      PLAYER_CARD_INSTANCE(player, card).damage_target_player == g_affected_card_controller &&
      g_affected_card != -1 &&
      (PLAYER_CARD_INSTANCE(player, card).token_status & STATUS_DYING) == 0 &&
      (PLAYER_CARD_INSTANCE(player, card).state & STATE_INVISIBLE) == 0)
  {
    DAMAGE_TARGET_CARD_INSTANCE(player, card).state |= STATE_VIGILANCE;
  }

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
                                 -1,
                                 -1,
                                 0,
                                 0,
                                 0);
  }

  if (event == EVENT_CAST_SPELL && g_affected_card == card && g_affected_card_controller == player)
  {
    if (g_duel_ai_mode_state != 1)
    {
      load_text("prompts.txt", "ETERNAL_WARRIOR");
    }
    g_spell_fizzled = (select_target_creature_and_store(player, player, card) == 0);
    if (g_spell_fizzled != 1 &&
        g_other_player == player &&
        (g_duel_network_flags & 2) == 0 &&
        (has_vigilance(PLAYER_CARD_INSTANCE(player, card).targets[0].player, PLAYER_CARD_INSTANCE(player, card).targets[0].card) != 0 ||
         PLAYER_CARD_INSTANCE(player, card).targets[0].player == g_active_player))
    {
      g_ai_modifier -= 0x30;
    }
    return 0;
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    if (C_real_validate_target(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                               PLAYER_CARD_INSTANCE(player, card).targets[0].card,
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
      PLAYER_CARD_INSTANCE(player, card).damage_target_player = PLAYER_CARD_INSTANCE(player, card).targets[0].player;
      PLAYER_CARD_INSTANCE(player, card).damage_target_card = PLAYER_CARD_INSTANCE(player, card).targets[0].card;
    }
    else
    {
      kill_card(player, card, KILL_BURY);
      g_spell_fizzled = 1;
    }
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    return 0;
  }

  if (event == EVENT_GRAVEYARD_FROM_PLAY && g_affected_card == card && g_affected_card_controller == player)
  {
    DAMAGE_TARGET_CARD_INSTANCE(player, card).state &= ~STATE_VIGILANCE;
  }

  return 0;
}

// FUNCTION: MAGIC 0x005322d7
// FUNCTION: SHANDALAR 0x004e9ad4
int card_the_brute(int player, int card, event_t event)
{
  int can_activate;

  if (event == EVENT_POWER &&
      PLAYER_CARD_INSTANCE(player, card).damage_target_card == g_affected_card &&
      PLAYER_CARD_INSTANCE(player, card).damage_target_player == g_affected_card_controller &&
      g_affected_card != -1 &&
      is_in_play(player, card))
  {
    ++g_event_result;
  }

  if (event == EVENT_UNTAP_PHASE)
  {
    g_ai_mana_demand_by_color[player][COLOR_RED] += 2;
    return 0;
  }

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
                                 -1,
                                 -1,
                                 -1,
                                 0,
                                 0,
                                 0);
  }

  if (event == EVENT_CAST_SPELL && g_affected_card == card && g_affected_card_controller == player)
  {
    if (g_duel_ai_mode_state != 1)
    {
      load_text("prompts.txt", "THE_BRUTE");
    }
    g_spell_fizzled = (select_target_creature_and_store(player, player, card) == 0);
    if (g_spell_fizzled != 1 &&
        g_other_player == player &&
        (g_duel_network_flags & 2) == 0 &&
        is_selected_target_already_attached(player, card, PLAYER_CARD_INSTANCE(player, card).internal_card_id) != 0)
    {
      g_ai_modifier -= 0xc;
    }
    return 0;
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    if (C_real_validate_target(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                               PLAYER_CARD_INSTANCE(player, card).targets[0].card,
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
      PLAYER_CARD_INSTANCE(player, card).damage_target_player = PLAYER_CARD_INSTANCE(player, card).targets[0].player;
      PLAYER_CARD_INSTANCE(player, card).damage_target_card = PLAYER_CARD_INSTANCE(player, card).targets[0].card;
    }
    else
    {
      kill_card(player, card, KILL_BURY);
      g_spell_fizzled = 1;
    }
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    return 0;
  }

  if (event == EVENT_CAN_ACTIVATE && (g_land_can_be_played & LCBP_REGENERATION) != 0 && PLAYER_CARD_INSTANCE(player, card).info_slot == 0)
  {
    can_activate = 1;
    if ((DAMAGE_TARGET_CARD_INSTANCE(player, card).state &
         0x800002) != STATE_IN_PLAY)
    {
      can_activate = 0;
    }
    if ((char)DAMAGE_TARGET_CARD_INSTANCE(player, card).kill_code != KILL_DESTROY)
    {
      can_activate = 0;
    }
    if (!has_mana_w_global_cost_mod(player, card, COLOR_RED, 3))
    {
      can_activate = 0;
    }
    return (can_activate != 0) ? 99 : 0;
  }

  if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_target(0);
    return 0;
  }

  if (event == EVENT_ACTIVATE && (g_land_can_be_played & LCBP_REGENERATION) != 0)
  {
    charge_mana_w_global_cost_mod(player, card, COLOR_RED, 3);
    if (g_spell_fizzled != 1)
    {
      g_upkeep_payment_completed = 1;
      ++PLAYER_CARD_INSTANCE(player, card).info_slot;
    }
    return 0;
  }

  if (event == EVENT_RESOLVE_ACTIVATION && (g_land_can_be_played & LCBP_REGENERATION) != 0)
  {
    PARENT_CARD_INSTANCE(player, card).info_slot = 0;
    regenerate_card((int)(char)PARENT_CARD_INSTANCE(player, card).damage_target_player,
                    PARENT_CARD_INSTANCE(player, card).damage_target_card);
    return 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x00532ad7
// FUNCTION: SHANDALAR 0x004ea2d4
int card_co_p_black(int player, int card, event_t event)
{
  return helper_circle_of_protection(player, card, event, get_sleighted_color(player, card, COLOR_BLACK));
}

// FUNCTION: MAGIC 0x00532b0e
// FUNCTION: SHANDALAR 0x004ea30b
int card_co_p_white(int player, int card, event_t event)
{
  return helper_circle_of_protection(player, card, event, get_sleighted_color(player, card, COLOR_WHITE));
}

// FUNCTION: MAGIC 0x00532b45
// FUNCTION: SHANDALAR 0x004ea342
int card_co_p_red(int player, int card, event_t event)
{
  return helper_circle_of_protection(player, card, event, get_sleighted_color(player, card, COLOR_RED));
}

// FUNCTION: MAGIC 0x00532b7c
// FUNCTION: SHANDALAR 0x004ea379
int card_co_p_blue(int player, int card, event_t event)
{
  return helper_circle_of_protection(player, card, event, get_sleighted_color(player, card, COLOR_BLUE));
}

// FUNCTION: MAGIC 0x00532bb3
// FUNCTION: SHANDALAR 0x004ea3b0
int card_co_p_green(int player, int card, event_t event)
{
  return helper_circle_of_protection(player, card, event, get_sleighted_color(player, card, COLOR_GREEN));
}

// FUNCTION: MOK 0x004bdc80
// FUNCTION: MAGIC 0x00532bea
// FUNCTION: SHANDALAR 0x004ea3e7
int helper_circle_of_protection(int player, int card, event_t event, int color)
{
  target_t target;
  unsigned int target_color;
  int target_player;
  int target_card;

  target_color = 1 << ((unsigned char)color);

  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }
  else
  {
    if (event == EVENT_CAST_SPELL && g_affected_card == card && g_affected_card_controller == player && count_permanents_by_internal_card_id(player, PLAYER_CARD_INSTANCE(player, card).internal_card_id, player) == 0)
    {
      g_ai_modifier +=
          (g_basiclandtypes_controlled[g_active_player][color] +
           g_creature_power_by_color[g_active_player][color] / 2) *
          0x18;
    }

    if (event == EVENT_CAN_ACTIVATE)
    {
      if ((g_land_can_be_played & LCBP_DAMAGE_PREVENTION) == 0 || has_mana_w_global_cost_mod(player, card, 7, 1) == 0 || real_target_available((int *)0, TARGET_SCAN_DIRECT, player, 2, 2, TARGET_ZONE_IN_PLAY, TYPE_NONE, TYPE_NONE, 0, 0, target_color, 0, g_damage_card_internal_card_id, -1, 0xffffffff, 0xffffffff, TARGET_SPECIAL_DAMAGE_PLAYER, 0, 0) == 0)
      {
        return 0;
      }
      return 99;
    }
    else
    {
      if (event == EVENT_ACTIVATE && (PLAYER_CARD_INSTANCE(player, card).state & STATE_INVISIBLE) == 0)
      {
        charge_mana_w_global_cost_mod(player, card, 0, 1);
        if (g_spell_fizzled != 1)
        {
          if (g_duel_ai_mode_state != 1)
          {
            load_text("prompts.txt", "CIRCLE_OF_PROTECTION");
          }

          if (C_real_select_target(player,
                                   2,
                                   2,
                                   TARGET_ZONE_IN_PLAY,
                                   TYPE_NONE,
                                   TYPE_NONE,
                                   0,
                                   0,
                                   target_color,
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
                                   &target))
          {
            SET_TARGET(PLAYER_CARD_INSTANCE(player, card).targets[0], target);
            PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
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
                                   2,
                                   2,
                                   TARGET_ZONE_IN_PLAY,
                                   TYPE_NONE,
                                   TYPE_NONE,
                                   0,
                                   0,
                                   target_color,
                                   COLOR_TEST_0,
                                   g_damage_card_internal_card_id,
                                   ~SUB_WALL,
                                   -1,
                                   -1,
                                   TARGET_SPECIAL_DAMAGE_PLAYER,
                                   0,
                                   0))
        {
          target_player = PLAYER_CARD_INSTANCE(player, card).targets[0].player;
          target_card = PLAYER_CARD_INSTANCE(player, card).targets[0].card;
          if (PLAYER_CARD_INSTANCE(target_player, target_card).info_slot != 0)
          {
            PLAYER_CARD_INSTANCE(target_player, target_card).info_slot = 0;
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
  }
}

// FUNCTION: MAGIC 0x005330c6
// FUNCTION: SHANDALAR 0x004ea8c1
int card_co_p_artifacts(int player, int card, event_t event)
{
  struct
  {
    target_t target;
    int color;
  } s;

  s.color = COLOR_ARTIFACT;

  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if (event == EVENT_CAST_SPELL && g_affected_card == card && g_affected_card_controller == player)
  {
    if (count_permanents_by_internal_card_id(player, PLAYER_CARD_INSTANCE(player, card).internal_card_id, player) == 0)
    {
      g_ai_modifier += (g_basiclandtypes_controlled[g_active_player][COLOR_ARTIFACT] +
                        g_basiclandtypes_controlled[g_active_player][COLOR_ANY] +
                        g_creature_power_by_color[g_active_player][COLOR_COLORLESS] / 2) *
                       0x18;
    }
    return 0;
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    return ((g_land_can_be_played & LCBP_DAMAGE_PREVENTION) != 0 &&
            has_mana_w_global_cost_mod(player, card, COLOR_ANY, 2) != 0 &&
            real_target_available((int *)0, TARGET_SCAN_DIRECT, player, 2, 2,
                                  TARGET_ZONE_IN_PLAY, TYPE_NONE, TYPE_NONE, 0, 0,
                                  1 << (unsigned char)s.color, COLOR_TEST_0, g_damage_card_internal_card_id,
                                  -1, -1, -1, TARGET_SPECIAL_DAMAGE_PLAYER, 0, 0) != 0)
               ? 99
               : 0;
  }

  if (event == EVENT_ACTIVATE && (PLAYER_CARD_INSTANCE(player, card).state & STATE_INVISIBLE) == 0)
  {
    charge_mana_w_global_cost_mod(player, card, COLOR_COLORLESS, 2);
    if (g_spell_fizzled != 1)
    {
      if (g_duel_ai_mode_state != 1)
      {
        load_text("prompts.txt", "CIRCLE_OF_PROTECTION");
      }
      if (C_real_select_target(player, 2, 2, TARGET_ZONE_IN_PLAY,
                               TYPE_NONE, TYPE_NONE, 0, 0,
                               1 << (unsigned char)s.color, COLOR_TEST_0,
                               g_damage_card_internal_card_id, -1, -1, -1,
                               TARGET_SPECIAL_DAMAGE_PLAYER, 0, 0,
                               g_text_lines[0], 1, &s.target))
      {
        SET_TARGET(PLAYER_CARD_INSTANCE(player, card).targets[0], s.target);
        PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
      }
      else
      {
        g_spell_fizzled = 1;
      }
    }

    return 0;
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    if (C_real_validate_target(PLAYER_CARD_INSTANCE(player, card).targets[0].player, PLAYER_CARD_INSTANCE(player, card).targets[0].card,
                               (char *)0, player, 2, 2, TARGET_ZONE_IN_PLAY,
                               TYPE_NONE, TYPE_NONE, 0, 0,
                               1 << (unsigned char)s.color, COLOR_TEST_0,
                               g_damage_card_internal_card_id, -1, -1, -1,
                               TARGET_SPECIAL_DAMAGE_PLAYER, 0, 0))
    {
      SET_TARGET(s.target, PLAYER_CARD_INSTANCE(player, card).targets[0]);
      if (PLAYER_CARD_INSTANCE(s.target.player, s.target.card).info_slot != 0)
      {
        PLAYER_CARD_INSTANCE(s.target.player, s.target.card).info_slot = 0;
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

// FUNCTION: MAGIC 0x005335b5
// FUNCTION: SHANDALAR 0x004eadb0
int card_phantasmal_terrain(int player, int card, event_t event)
{
  int land_type;

  if (event == EVENT_CAN_CAST)
  {
    return real_target_available((int *)0,
                                 TARGET_SCAN_DIRECT,
                                 player,
                                 2,
                                 2,
                                 0x200,
                                 1,
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

  if (((event == EVENT_CAST_SPELL) && (card == g_affected_card)) && (player == g_affected_card_controller))
  {
    if (g_duel_ai_mode_state != 1)
    {
      load_text("prompts.txt", "PHANTASMAL_TERRAIN");
    }
    if (!select_target_land_and_store(player, 1 - player, card) == 0)
    {
      if ((player == g_active_player) || ((g_duel_network_flags & 2) != 0))
      {
        if (g_duel_ai_mode_state != 1)
        {
          load_text("prompts.txt", "PHANTASMAL_TERRAIN");
        }
        land_type = choose_a_color(player,
                                   g_text_lines[1],
                                   0,
                                   -1,
                                   COLOR_TEST_WHITE | COLOR_TEST_RED | COLOR_TEST_GREEN | COLOR_TEST_BLUE | COLOR_TEST_BLACK);
        if (land_type == -1)
        {
          g_spell_fizzled = 1;
        }
      }
      else if (g_duel_ai_mode_state == 1)
      {
        g_ai_recorded_choice = internal_rand(5) + 1;
        land_type = g_ai_recorded_choice;
        record_ai_action_selection();
      }
      else
      {
        replay_ai_action_selection();
        land_type = g_ai_recorded_choice;
      }

      if (PLAYER_CARD_INSTANCE(player, card).targets[0].player == player)
      {
        g_ai_modifier += -0x30;
      }
      PLAYER_CARD_INSTANCE(player, card).dummy3 = land_type;
    }
    else
    {
      g_spell_fizzled = 1;
    }
    return 0;
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
      PLAYER_CARD_INSTANCE(player, card).damage_target_player =
          (char)PLAYER_CARD_INSTANCE(player, card).targets[0].player;
      PLAYER_CARD_INSTANCE(player, card).damage_target_card =
          PLAYER_CARD_INSTANCE(player, card).targets[0].card;
      --PLAYER_CARD_INSTANCE(player, card).dummy3;
      DAMAGE_TARGET_CARD_INSTANCE(player, card).internal_card_id = PLAYER_CARD_INSTANCE(player, card).dummy3;
      DAMAGE_TARGET_CARD_INSTANCE(player, card).regen_status |= 0x1000000;
    }
    else
    {
      kill_card(player, card, KILL_BURY);
      g_spell_fizzled = 1;
    }
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    return 0;
  }

  if ((((event == EVENT_CHANGE_TYPE) && ((g_land_can_be_played & LCBP_DURING_EVENT_CHANGE_TYPE_SECOND_PASS) == 0)) && ((PLAYER_CARD_INSTANCE(player, card).damage_target_card == g_affected_card) && (((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player == g_affected_card_controller) && (g_affected_card != -1)))) && is_in_play(player, card))
  {
    g_event_result = PLAYER_CARD_INSTANCE(player, card).dummy3;
  }
  return 0;
}

// FUNCTION: MAGIC 0x00533bc2
// FUNCTION: SHANDALAR 0x004eb3bf
int card_conversion(int player, int card, event_t event)
{
  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if ((((event == EVENT_CAST_SPELL) || (event == EVENT_SHOULD_AI_PLAY)) && (g_affected_card == card)) && (g_affected_card_controller == player) && count_permanents_by_internal_card_id(player, PLAYER_CARD_INSTANCE(player, card).internal_card_id, -1) == 0)
  {
    g_ai_modifier +=
        (g_creature_power_by_color[g_other_player][COLOR_WHITE] -
         g_creature_power_by_color[g_active_player][COLOR_WHITE]) *
        0xc;
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    PLAYER_CARD_INSTANCE(player, card).info_slot = 5;
    return 0;
  }

  if (((event == EVENT_SETUP_UPKEEP_COSTS) && (g_affected_card == card)) && (g_affected_card_controller == player) && player == g_current_player && g_event_player == g_current_player)
  {
    PLAYER_CARD_INSTANCE(player, card).upkeep_flags |= 1;
    PLAYER_CARD_INSTANCE(player, card).upkeep_white += 2;
    return 0;
  }

  if (event == EVENT_UPKEEP_COSTS_UNPAID)
  {
    kill_card(g_card_on_stack_controller, g_card_on_stack, KILL_BURY);
    return 0;
  }

  if ((event == EVENT_CHANGE_TYPE) && ((g_land_can_be_played & LCBP_DURING_EVENT_CHANGE_TYPE_SECOND_PASS) == 0) && is_in_play(player, card) && is_in_play(g_affected_card_controller, g_affected_card) && is_basic_land_internal_card_id_of_color(g_event_result, get_hacked_color(player, card, 4)) != 0)
  {
    g_event_result = get_hacked_color(player, card, PLAYER_CARD_INSTANCE(player, card).info_slot) - 1;
  }

  return 0;
}

// FUNCTION: MAGIC 0x00533e91
// FUNCTION: SHANDALAR 0x004eb68f
int card_wild_growth(int player, int card, event_t event)
{
  if (event == EVENT_CAN_CAST)
  {
    return real_target_available((int *)0,
                                 TARGET_SCAN_DIRECT,
                                 player,
                                 2,
                                 2,
                                 0x200,
                                 1,
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

  if (((event == EVENT_CAST_SPELL) && (g_affected_card == card)) && (player == g_affected_card_controller))
  {
    if (g_duel_ai_mode_state != 1)
    {
      load_text("prompts.txt", "WILD_GROWTH");
    }
    if (select_target_land_and_store(player, player, card) != 0)
    {
      if (PLAYER_CARD_INSTANCE(player, card).targets[0].player == g_other_player)
      {
        g_ai_modifier += g_creature_power_by_color[g_other_player][COLOR_GREEN] * 0xc;
      }
    }
    else
    {
      g_spell_fizzled = 1;
    }
    return 0;
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
                               0) != 0)
    {
      PLAYER_CARD_INSTANCE(player, card).damage_target_player =
          (char)PLAYER_CARD_INSTANCE(player, card).targets[0].player;
      PLAYER_CARD_INSTANCE(player, card).damage_target_card =
          PLAYER_CARD_INSTANCE(player, card).targets[0].card;
    }
    else
    {
      kill_card(player, card, KILL_BURY);
      g_spell_fizzled = 1;
    }
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    return 0;
  }

  if (((event == EVENT_TAP_CARD) && (PLAYER_CARD_INSTANCE(player, card).damage_target_card == g_affected_card)) && (((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player == g_affected_card_controller) && ((g_affected_card != -1) && (g_produced_mana_color != -1))))
  {
    produce_mana((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                 COLOR_GREEN,
                 1);
    return 0;
  }

  if ((event == EVENT_COUNT_MANA) && (((PLAYER_CARD_INSTANCE(player, card).damage_target_card == g_affected_card) && ((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player == g_affected_card_controller)) && ((g_affected_card != -1) && ((PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).state & STATE_TAPPED) == 0))))
  {
    declare_mana_available(g_affected_card_controller, COLOR_GREEN, 1);
  }
  return 0;
}

// FUNCTION: MAGIC 0x00534336
// FUNCTION: SHANDALAR 0x004ebb33
int card_flight(int player, int card, event_t event)
{
  if (event == EVENT_CAN_CAST)
  {
    load_recorded_action_target(0);
    return real_target_available((int *)0,
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
                                 0,
                                 0);
  }

  if (((event == EVENT_CAST_SPELL) && (card == g_affected_card)) && (player == g_affected_card_controller))
  {
    if (g_duel_ai_mode_state != 1)
    {
      load_text("prompts.txt", "FLIGHT");
    }
    if (select_target_creature_and_store(player, player, card))
    {
      if (PLAYER_CARD_INSTANCE(player, card).targets[0].player == g_active_player)
      {
        g_ai_modifier += -0x18;
      }
      if ((TARGET_CARD_INSTANCE(player, card, 0).regen_status &
           KEYWORD_FLYING) != 0)
      {
        g_ai_modifier += -99;
      }
    }
    else
    {
      g_spell_fizzled = 1;
    }
    return 0;
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
                               0))
    {
      PLAYER_CARD_INSTANCE(player, card).damage_target_player =
          (char)PLAYER_CARD_INSTANCE(player, card).targets[0].player;
      PLAYER_CARD_INSTANCE(player, card).damage_target_card =
          PLAYER_CARD_INSTANCE(player, card).targets[0].card;
      PLAYER_CARD_INSTANCE(player, card).info_slot = 1;
    }
    else
    {
      kill_card(player, card, KILL_BURY);
      g_spell_fizzled = 1;
    }
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    return 0;
  }

  if (((PLAYER_CARD_INSTANCE(player, card).info_slot != 0) && (PLAYER_CARD_INSTANCE(player, card).targets[0].card == g_affected_card)) && (((int)PLAYER_CARD_INSTANCE(player, card).targets[0].player == g_affected_card_controller) && ((g_affected_card != -1) && (event == EVENT_ABILITIES))))
  {
    g_event_result |= KEYWORD_FLYING;
  }
  return 0;
}

// FUNCTION: MAGIC 0x005347a7
// FUNCTION: SHANDALAR 0x004ebfa3
int card_lifeforce(int player, int card, event_t event)
{
  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }
  else if (((event == EVENT_CAST_SPELL) && (card == g_affected_card)) && (player == g_affected_card_controller))
  {
    g_ai_modifier += g_basiclandtypes_controlled[1 - player][COLOR_BLACK] * 5 + 0x18;
    return 0;
  }
  else if (event == EVENT_CAN_ACTIVATE)
  {
    if (g_current_spell_player == -1)
    {
      return 0;
    }
    if (((g_land_can_be_played & LCBP_SPELL_BEING_PLAYED) != 0) && (has_mana_w_global_cost_mod(player, card, COLOR_GREEN, 2) != 0))
    {
      if (C_real_validate_target(g_current_spell_player,
                                 g_current_spell_card,
                                 (char *)0,
                                 player,
                                 ANYBODY,
                                 ANYBODY,
                                 0,
                                 TYPE_NONE,
                                 TYPE_NONE,
                                 0,
                                 0,
                                 1 << (((unsigned char)get_sleighted_color(player, card, COLOR_BLACK))),
                                 COLOR_TEST_0,
                                 -1,
                                 ~SUB_WALL,
                                 -1,
                                 -1,
                                 TARGET_SPECIAL_SPELL_ON_STACK,
                                 0,
                                 0) != 0)
      {
        return 99;
      }
    }
    return 0;
  }
  else if ((event == EVENT_ACTIVATE) && (g_current_spell_player != -1))
  {
    if (has_mana_w_global_cost_mod(player, card, COLOR_GREEN, 2) != 0)
    {
      charge_mana_w_global_cost_mod(player, card, COLOR_GREEN, 2);
      if (g_spell_fizzled != 1)
      {
        PLAYER_CARD_INSTANCE(player, card).targets[0].player = g_current_spell_player;
        PLAYER_CARD_INSTANCE(player, card).targets[0].card = g_current_spell_card;
      }
    }
    return 0;
  }
  else
  {
    if (event == EVENT_RESOLVE_ACTIVATION)
    {
      if (C_real_validate_target(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                                 PLAYER_CARD_INSTANCE(player, card).targets[0].card,
                                 (char *)0,
                                 player,
                                 ANYBODY,
                                 ANYBODY,
                                 0,
                                 TYPE_NONE,
                                 TYPE_NONE,
                                 0,
                                 0,
                                 1 << (((unsigned char)get_sleighted_color(player, card, COLOR_BLACK))),
                                 COLOR_TEST_0,
                                 -1,
                                 ~SUB_WALL,
                                 -1,
                                 -1,
                                 TARGET_SPECIAL_SPELL_ON_STACK,
                                 0,
                                 0))
      {
        kill_card(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                  PLAYER_CARD_INSTANCE(player, card).targets[0].card,
                  KILL_BURY);
      }
      else
      {
        g_spell_fizzled = 1;
      }
    }
    return 0;
  }
}

// FUNCTION: MAGIC 0x00534ac2
// FUNCTION: SHANDALAR 0x004ec2be
int card_deathgrip(int player, int card, event_t event)
{
  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if (event == EVENT_CAST_SPELL && card == g_affected_card && player == g_affected_card_controller)
  {
    g_ai_modifier += g_basiclandtypes_controlled[1 - player][COLOR_GREEN] * 5 + 0x18;
    return 0;
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    if (g_current_spell_player != -1)
    {
      return ((g_land_can_be_played & LCBP_SPELL_BEING_PLAYED) != 0 &&
              has_mana_w_global_cost_mod(player, card, COLOR_BLACK, 2) != 0 &&
              C_real_validate_target(g_current_spell_player, g_current_spell_card, (char *)0, player, 2, 2, 0, TYPE_NONE, TYPE_NONE, 0, 0, 1 << get_sleighted_color(player, card, COLOR_GREEN), COLOR_TEST_0, -1, ~SUB_WALL, -1, -1, TARGET_SPECIAL_SPELL_ON_STACK, 0, 0))
                 ? 99
                 : 0;
    }
    else
    {
      return 0;
    }
  }

  if (event == EVENT_ACTIVATE && has_mana_w_global_cost_mod(player, card, COLOR_BLACK, 2) != 0 && g_current_spell_player != -1)
  {
    charge_mana_w_global_cost_mod(player, card, COLOR_BLACK, 2);
    if (g_spell_fizzled != 1)
    {
      PLAYER_CARD_INSTANCE(player, card).targets[0].player = g_current_spell_player;
      PLAYER_CARD_INSTANCE(player, card).targets[0].card = g_current_spell_card;
    }
    return 0;
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    if (C_real_validate_target(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
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
                               1 << get_sleighted_color(player, card, COLOR_GREEN),
                               COLOR_TEST_0,
                               -1,
                               ~SUB_WALL,
                               -1,
                               -1,
                               TARGET_SPECIAL_SPELL_ON_STACK,
                               0,
                               0))
    {
      kill_card(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                PLAYER_CARD_INSTANCE(player, card).targets[0].card,
                KILL_BURY);
    }
    else
    {
      g_spell_fizzled = 1;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x00534ddb
// FUNCTION: SHANDALAR 0x004ec5d7
int choose_best_tapped_permanent_for_ai(int player, int mode)
{
  struct
  {
    int card_0x40;                  /* ebp-0x5a4 */
    int score_0x40;                 /* ebp-0x5a0 */
    int abilities_0x40;             /* ebp-0x59c */
    int internal_id_0x40;           /* ebp-0x598 */
    int candidate_toughnesses2[50]; /* ebp-0x594 */
    int count2;                     /* ebp-0x4cc */
    int candidate_powers2[50];      /* ebp-0x4c8 */
    int candidate_scores2[50];      /* ebp-0x400 */
    int candidate_abilities2[50];   /* ebp-0x338 */
    int best_toughness2;            /* ebp-0x270 */
    int best_power2;                /* ebp-0x26c */
    int candidate_cards2[50];       /* ebp-0x268 */
    int count1;                     /* ebp-0x1a0 */
    int candidate_scores1[50];      /* ebp-0x19c */
    int candidate_cards1[50];       /* ebp-0xd4 */
    int current_card;               /* ebp-0x0c */
    int best_score;                 /* ebp-0x08 */
    int best_card;                  /* ebp-0x04 */
  } s;

  if (player == -1)
  {
    return -1;
  }

  if (mode == 1)
  {
    s.best_card = -1;
    s.best_score = -10;
    s.count1 = 0;
    for (s.current_card = 0; s.current_card < g_active_cards_count[player]; ++s.current_card)
    {
      if (is_in_play(player, s.current_card) &&
          (global_cards_data[PLAYER_CARD_INSTANCE(player, s.current_card).internal_card_id].type & TYPE_LAND) &&
          (PLAYER_CARD_INSTANCE(player, s.current_card).state & STATE_TAPPED))
      {
        s.candidate_cards1[s.count1] = s.current_card;
        s.candidate_scores1[s.count1] = 0;
        ++s.count1;
      }
    }

    for (s.current_card = 0; s.current_card < s.count1; ++s.current_card)
    {
      if ((global_cards_data[PLAYER_CARD_INSTANCE(player, s.candidate_cards1[s.current_card]).internal_card_id].extra_ability & 1) != 0)
      {
        ++s.candidate_scores1[s.current_card];
      }
      if ((PLAYER_CARD_INSTANCE(player, s.current_card).state & STATE_NO_AUTO_TAPPING) != 0)
      {
        s.candidate_scores1[s.current_card] = -1;
      }
    }

    for (s.current_card = 0; s.current_card < s.count1; ++s.current_card)
    {
      if (s.best_score < s.candidate_scores1[s.current_card])
      {
        s.best_score = s.candidate_scores1[s.current_card];
        s.best_card = s.candidate_cards1[s.current_card];
      }
    }
  }

  else if (mode == 2)
  {
    s.best_card = -1;
    s.best_score = -1;
    s.best_power2 = 0;
    s.best_toughness2 = 0;
    s.count2 = 0;

    for (s.current_card = 0; s.current_card < g_active_cards_count[player]; ++s.current_card)
    {
      if (is_in_play(player, s.current_card) &&
          (global_cards_data[PLAYER_CARD_INSTANCE(player, s.current_card).internal_card_id].type & TYPE_CREATURE) &&
          (PLAYER_CARD_INSTANCE(player, s.current_card).state & STATE_TAPPED))
      {
        s.candidate_cards2[s.count2] = s.current_card;
        s.candidate_powers2[s.count2] = C_get_abilities(player, s.current_card, EVENT_POWER, -1);
        if (s.best_power2 < s.candidate_powers2[s.count2])
        {
          s.best_power2 = s.candidate_powers2[s.count2];
        }
        s.candidate_toughnesses2[s.count2] = C_get_abilities(player, s.current_card, EVENT_TOUGHNESS, -1);
        if (s.best_toughness2 < s.candidate_toughnesses2[s.count2])
        {
          s.best_toughness2 = s.candidate_toughnesses2[s.count2];
        }
        s.candidate_abilities2[s.count2] = C_get_abilities(player, s.current_card, EVENT_ABILITIES, -1);
        s.candidate_scores2[s.count2] = 0;
        ++s.count2;
      }
    }

    for (s.current_card = 0; s.current_card < s.count2; ++s.current_card)
    {
      if (s.candidate_powers2[s.current_card] == s.best_power2)
      {
        s.candidate_scores2[s.current_card] += 3;
      }
      if (s.candidate_toughnesses2[s.current_card] == s.best_toughness2)
      {
        s.candidate_scores2[s.current_card] += 2;
      }
      if ((s.candidate_abilities2[s.current_card] & 0x20) != 0)
      {
        ++s.candidate_scores2[s.current_card];
      }
      if ((s.candidate_abilities2[s.current_card] & 0x100) != 0)
      {
        ++s.candidate_scores2[s.current_card];
      }
      while (s.candidate_abilities2[s.current_card] != 0)
      {
        if ((s.candidate_abilities2[s.current_card] & 1) != 0)
        {
          ++s.candidate_scores2[s.current_card];
        }
        s.candidate_abilities2[s.current_card] >>= 1;
      }
      if ((global_cards_data[PLAYER_CARD_INSTANCE(player, s.candidate_cards2[s.current_card]).internal_card_id].extra_ability & 0x1000) != 0)
      {
        ++s.candidate_scores2[s.current_card];
      }
      if ((global_cards_data[PLAYER_CARD_INSTANCE(player, s.candidate_cards2[s.current_card]).internal_card_id].extra_ability & 1) != 0)
      {
        ++s.candidate_scores2[s.current_card];
      }
    }

    for (s.current_card = 0; s.current_card < s.count2; ++s.current_card)
    {
      if (s.best_score < s.candidate_scores2[s.current_card])
      {
        s.best_score = s.candidate_scores2[s.current_card];
        s.best_card = s.candidate_cards2[s.current_card];
      }
    }
  }

  else if (mode == 0x40)
  {
    s.best_score = -1;
    s.best_card = s.best_score;
    for (s.card_0x40 = 0; s.card_0x40 < g_active_cards_count[player]; ++s.card_0x40)
    {
      if (is_in_play(player, s.card_0x40) &&
          (PLAYER_CARD_INSTANCE(player, s.card_0x40).state & STATE_TAPPED) &&
          (global_cards_data[PLAYER_CARD_INSTANCE(player, s.card_0x40).internal_card_id].type & 0x40) &&
          (PLAYER_CARD_INSTANCE(player, s.card_0x40).untap_status & 1) != 0)
      {
        s.internal_id_0x40 = PLAYER_CARD_INSTANCE(player, s.card_0x40).internal_card_id;
        s.score_0x40 = 0;

        s.score_0x40 += C_get_abilities(player, s.card_0x40, EVENT_POWER, -1) +
                        C_get_abilities(player, s.card_0x40, EVENT_TOUGHNESS, -1);

        s.abilities_0x40 = C_get_abilities(player, s.card_0x40, EVENT_ABILITIES, -1);
        if ((s.abilities_0x40 & 0x20) != 0)
        {
          ++s.score_0x40;
        }
        if ((s.abilities_0x40 & 0x100) != 0)
        {
          ++s.score_0x40;
        }
        while (s.abilities_0x40 != 0)
        {
          if ((s.abilities_0x40 & 1) != 0)
          {
            ++s.score_0x40;
          }
          s.abilities_0x40 >>= 1;
        }
        if ((global_cards_data[s.internal_id_0x40].extra_ability & 0x1000) != 0)
        {
          ++s.score_0x40;
        }
        if ((global_cards_data[s.internal_id_0x40].extra_ability & 1) != 0)
        {
          ++s.score_0x40;
        }
        if ((char)global_cards_data[s.internal_id_0x40].cc[1] != -1)
        {
          s.score_0x40 += (char)global_cards_data[s.internal_id_0x40].cc[1];
        }
        if (s.best_score < s.score_0x40)
        {
          s.best_score = s.score_0x40;
          s.best_card = s.card_0x40;
        }
      }
    }

    /* fallthrough to return */
  }
  else
  {
    s.best_card = -1;
  }

  return s.best_card;
}
