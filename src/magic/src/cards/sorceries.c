#include <stdio.h>
#include <string.h>
#include "../game_support.h"
#include "../global_strings.h"

int GetCardRarity(int internal_card_id);
int find_highest_value_library_card_by_type(int player, int library_player, unsigned int type);
int coin_flip(int player, char *dialog_title, int show_dialog_if_animation_is_off);

// FUNCTION: MAGIC 0x00401000
// FUNCTION: SHANDALAR 0x00446d20
int card_call_from_the_grave(int player, int card, event_t event)
{
  int attempt_count;
  int found_player;
  int graveyard_index;
  int hand_card;
  int selected_internal_card_id;
  int slot_limit;
  int test_index;
  int test_player;
  int total_damage;

  slot_limit = 500;

  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    graveyard_index = 0;
    if (g_other_player == player && (g_duel_network_flags & 2) != 0)
    {
      TENTATIVE_wait_for_network_result(found_player, 0x1c);
      found_player = g_target_pair_network_packet.target.player;
      graveyard_index = g_target_pair_network_packet.target.card;
      selected_internal_card_id = global_graveyard_slots[found_player][graveyard_index];
    }
    else
    {
      for (test_index = 0; test_index < slot_limit; ++test_index)
      {
        if (global_graveyard_slots[g_active_player][test_index] == -1 &&
            global_graveyard_slots[g_other_player][test_index] == -1)
        {
          slot_limit = test_index;
        }
      }

      attempt_count = 0;
      do
      {
        ++attempt_count;
        if (attempt_count > 999)
        {
          graveyard_index = -1;
          break;
        }
        found_player = internal_rand(2);
        graveyard_index = internal_rand(slot_limit);
        selected_internal_card_id = global_graveyard_slots[found_player][graveyard_index];
      } while (selected_internal_card_id == -1 ||
               (global_cards_data[selected_internal_card_id].type & TYPE_CREATURE) == 0);

      if ((g_duel_network_flags & 2) != 0 && g_active_player == player)
      {
        g_target_pair_network_packet.packet_type = 0x1c;
        g_target_pair_network_packet.target.player = 1 - found_player;
        g_target_pair_network_packet.target.card = graveyard_index;
        TENTATIVE_send_network_result(player, 0x1c);
      }
    }

    if (graveyard_index == -1)
    {
      test_player = 0;
      while (test_player < 2 && graveyard_index == -1)
      {
        test_index = 0;
        while (test_index < 500 &&
               global_graveyard_slots[test_player][test_index] != -1 &&
               graveyard_index == -1)
        {
          selected_internal_card_id = global_graveyard_slots[test_player][test_index];
          if ((global_cards_data[selected_internal_card_id].type & TYPE_CREATURE) != 0)
          {
            graveyard_index = test_index;
            found_player = test_player;
          }
          ++test_index;
        }
        ++test_player;
      }
    }

    if (graveyard_index != -1 && global_graveyard_slots[found_player][graveyard_index] != -1)
    {
      if (g_duel_ai_mode_state != 1)
      {
        play_sound_effect(0x2f);
      }

      hand_card = add_card_to_hand(player, global_graveyard_slots[found_player][graveyard_index]);
      if (found_player == 0)
      {
        PLAYER_CARD_INSTANCE(player, hand_card).state = 0;
      }
      else
      {
        PLAYER_CARD_INSTANCE(player, hand_card).state = STATE_OWNED_BY_OPPONENT;
      }

      if (hand_card != -1)
      {
        remove_card_from_graveyard(found_player, graveyard_index);
        process_card_enters_play(player, hand_card);
        total_damage = (int)global_cards_data[selected_internal_card_id].cc[1];
        if (total_damage == -1)
        {
          total_damage = 0;
        }
        damage_player(player, global_cards_data[selected_internal_card_id].cc[0] + total_damage, player, card);
      }
    }

    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004013ed
// FUNCTION: SHANDALAR 0x0044710d
int card_time_walk(int player, int card, event_t event)
{
  int current_card;
  int current_player;
  int effect_card;
  int effect_found;

  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if (event == EVENT_CAST_SPELL && g_affected_card == card && g_affected_card_controller == player)
  {
    g_ai_modifier += 0x60;
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    if (g_duel_extra_turn_player == -1)
    {
      effect_found = 0;
      for (effect_card = 0; effect_card < 2 && !effect_found; ++effect_card)
      {
        for (current_card = 0; current_card < g_active_cards_count[effect_card]; ++current_card)
        {
          if (PLAYER_CARD_INSTANCE(effect_card, current_card).internal_card_id == g_duel_generated_internal_card_id_0f && (PLAYER_CARD_INSTANCE(effect_card, current_card).token_status & 0x100) != 0)
          {
            effect_found = 1;
          }
        }
      }
      if (!effect_found)
      {
        g_duel_extra_turn_player = player;
      }
    }

    current_player = create_legacy_effect(player, card, g_duel_generated_internal_card_id_0f, -1, -1);
    PLAYER_CARD_INSTANCE(player, current_player).token_status |= 0x120;
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x0040159b
// FUNCTION: SHANDALAR 0x004472bb
int card_balance(int player, int card, event_t event)
{
  struct
  {
    int player_1_count;
    int pad;
    int player_0_count;
    int current_card;
    char land_prompt[300];
    char creature_prompt[300];
  } s;

  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    if (g_duel_ai_mode_state != 1)
    {
      load_text("prompts.txt", "BALANCE");
      strcpy(s.land_prompt, g_text_lines[0]);
      strcpy(s.creature_prompt, g_text_lines[1]);
    }

    do
    {
      s.player_1_count = 0;
      s.player_0_count = s.player_1_count;
      s.current_card = 0;
      for (; (g_active_cards_count[1] <= g_active_cards_count[0] ? g_active_cards_count[0] : g_active_cards_count[1]) > s.current_card; ++s.current_card)
      {
        if (is_in_play(0, s.current_card) && (global_cards_data[PLAYER_CARD_INSTANCE(0, s.current_card).internal_card_id].type & TYPE_LAND) != 0)
        {
          ++s.player_0_count;
        }
        if (is_in_play(1, s.current_card) && (global_cards_data[PLAYER_CARD_INSTANCE(1, s.current_card).internal_card_id].type & TYPE_LAND) != 0)
        {
          ++s.player_1_count;
        }
      }

      if (g_duel_ai_mode_state != 1)
      {
        strcpy(g_text_lines[0], s.land_prompt);
      }

      if (s.player_1_count < s.player_0_count)
      {
        sacrifice_a_land(0);
      }
      else if (s.player_0_count < s.player_1_count)
      {
        sacrifice_a_land(1);
      }

      TENTATIVE_reassess_all_cards(0, 0xff);
    } while (s.player_0_count != s.player_1_count);

    do
    {
      if (g_duel_summary.hand_counts[1] < g_duel_summary.hand_counts[0])
      {
        discard(0, 0, 0);
      }
      if (g_duel_summary.hand_counts[0] < g_duel_summary.hand_counts[1])
      {
        discard(1, 0, 0);
      }
    } while (g_duel_summary.hand_counts[0] != g_duel_summary.hand_counts[1]);

    do
    {
      s.player_1_count = 0;
      s.player_0_count = s.player_1_count;
      s.current_card = 0;
      for (; (g_active_cards_count[1] <= g_active_cards_count[0] ? g_active_cards_count[0] : g_active_cards_count[1]) > s.current_card; ++s.current_card)
      {
        if (is_in_play(0, s.current_card) && (global_cards_data[PLAYER_CARD_INSTANCE(0, s.current_card).internal_card_id].type & TYPE_CREATURE) != 0 && PLAYER_CARD_INSTANCE(0, s.current_card).kill_code != 3)
        {
          ++s.player_0_count;
        }
        if (is_in_play(1, s.current_card) && (global_cards_data[PLAYER_CARD_INSTANCE(1, s.current_card).internal_card_id].type & TYPE_CREATURE) != 0 && PLAYER_CARD_INSTANCE(0, s.current_card).kill_code != 3)
        {
          ++s.player_1_count;
        }
      }

      if (g_duel_ai_mode_state != 1)
      {
        strcpy(g_text_lines[0], s.creature_prompt);
      }

      if (s.player_1_count < s.player_0_count)
      {
        s.current_card = choose_creature_to_sacrifice(0);
        kill_card(0, s.current_card, KILL_SACRIFICE);
      }
      if (s.player_0_count < s.player_1_count)
      {
        s.current_card = choose_creature_to_sacrifice(1);
        kill_card(1, s.current_card, KILL_SACRIFICE);
      }

      TENTATIVE_reassess_all_cards(0, 0xff);
    } while (s.player_0_count != s.player_1_count);

    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004019b4
// FUNCTION: SHANDALAR 0x004476d4
int card_braingeyser(int player, int card, event_t event)
{
  target_t target;
  int cards_to_draw;
  int target_player;

  if (event == EVENT_CAN_CAST)
  {
    if (((player == g_other_player) && ((g_duel_network_flags & 2) == 0)) && has_mana(player, 7, 3) == 0)
    {
      return 0;
    }
    return 1;
  }

  if (((event == EVENT_CAST_SPELL) && (card == g_affected_card)) && (player == g_affected_card_controller))
  {
    if (g_duel_ai_mode_state != 1)
    {
      load_text("prompts.txt", "BRAINGEYSER");
    }
    if (C_real_select_target(player,
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
                             &target) != 0)
    {
      PLAYER_CARD_INSTANCE(player, card).info_slot = g_x_value;
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
    for (cards_to_draw = 0; cards_to_draw < PLAYER_CARD_INSTANCE(player, card).info_slot; ++cards_to_draw)
    {
      draw_card_for_player(target_player);
    }
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x00401c46
// FUNCTION: SHANDALAR 0x00447966
int card_wheel_of_fortune(int player, int card, event_t event)
{
  int current_player;
  int player_index;
  int cards_in_hand;
  int current_card;
  int player_zero_library;
  int player_one_library;
  int dummy;

  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if (event == EVENT_CAST_SPELL && card == g_affected_card && player == g_affected_card_controller)
  {
    g_ai_modifier += g_duel_summary.hand_counts[player] * -0x18 + 0x30;
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    current_player = g_current_player;
    for (player_index = 0; player_index < 2; ++player_index)
    {
      cards_in_hand = 0;
      for (current_card = 0; current_card < g_active_cards_count[current_player]; ++current_card)
      {
        if (is_card_pending_resolution(current_player, current_card))
        {
          ++cards_in_hand;
        }
      }
      for (current_card = 0; current_card < cards_in_hand; ++current_card)
      {
        discard(current_player, 1, 0);
      }
      current_player = current_player == 0 ? 1 : 0;
    }

    TENTATIVE_reassess_all_cards(0, 0x30);
    player_zero_library = 0;
    player_one_library = 0;
    for (current_card = 0; current_card < 500; ++current_card)
    {
      if (global_library[0][current_card] != -1)
      {
        ++player_zero_library;
      }
      if (global_library[1][current_card] != -1)
      {
        ++player_one_library;
      }
    }

    if (player_zero_library < 7 && player_one_library < 7)
    {
      if (g_duel_ai_mode_state == 1)
      {
        g_life[0] = -99;
        g_life[1] = -99;
      }
      else
      {
        load_text("prompts.txt", "WHEELOFFORTUNE");
        set_duel_prompt_text(g_text_lines[0]);
        Sleep(2500);
        set_duel_prompt_text("");
        exit_duel_thread(2);
      }
    }
    else
    {
      draw_cards_and_set_hand_count(0, 7);
      draw_cards_and_set_hand_count(1, 7);
    }

    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x00401e7f
// FUNCTION: SHANDALAR 0x00447b9f
int card_rebirth(int player, int card, event_t event)
{
  char prompt[600];
  int ante_slot;
  int chosen[4];
  int dialog_result;
  int player_index;
  int top_card;

  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    PLAYER_CARD_INSTANCE(player, card).info_slot = 0;

    for (player_index = 0; player_index < 2; ++player_index)
    {
      if (global_library[player_index][0] == -1)
      {
        chosen[player_index] = 0;
      }
      else
      {
        if (g_duel_ai_mode_state != 1)
        {
          load_text("prompts.txt", "REBIRTH");
          sprintf(prompt, " %s\n %s", g_text_lines[0], g_text_lines[1]);
        }

        dialog_result = do_dialog(player_index,
                                  player,
                                  card,
                                  -1,
                                  -1,
                                  prompt,
                                  (unsigned int)(g_life[player_index] > 15));
        if (dialog_result == 0)
        {
          chosen[player_index] = 1;
        }
        else
        {
          chosen[player_index] = 0;
        }
      }
    }

    for (player_index = 0; player_index < 2; ++player_index)
    {
      if (chosen[player_index] != 0)
      {
        g_life[player_index] = 20;

        for (ante_slot = 0;
             global_ante_cards[player_index][ante_slot] != -1 && ante_slot < 16;
             ++ante_slot)
        {
        }

        if (ante_slot < 16)
        {
          top_card = global_library[player_index][0];
          remove_card_from_deck(player_index, 0);
          global_ante_cards[player_index][ante_slot] = top_card;
        }
      }
    }

    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004020c4
// FUNCTION: SHANDALAR 0x00447de6
int card_winds_of_change(int player, int card, event_t event)
{
  struct
  {
    int cards_moved;
    int player_index;
    int current_card;
    int current_player;
  } s;

  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    for (s.player_index = 0, s.current_player = g_current_player; s.player_index < 2;
         ++s.player_index, (g_current_player != 0 ? --s.current_player : ++s.current_player))
    {
      s.cards_moved = 0;

      for (s.current_card = 0; s.current_card < g_active_cards_count[s.current_player]; ++s.current_card)
      {
        if (is_card_pending_resolution(s.current_player, s.current_card) != 0)
        {
          put_card_on_bottom_of_library(s.current_player, PLAYER_CARD_INSTANCE(s.current_player, s.current_card).internal_card_id);
          PLAYER_CARD_INSTANCE(s.current_player, s.current_card).internal_card_id = -1;
          ++s.cards_moved;
        }
      }

      TENTATIVE_reassess_all_cards(0, 0x30);
      shuffle_duel_library(player, s.current_player);
      draw_cards_and_set_hand_count(s.current_player, s.cards_moved);
    }

    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x00402229
// FUNCTION: SHANDALAR 0x00447f4b
int card_timetwister(int player, int card, event_t event)
{
  int current_player;
  int player_index;
  int current_card;

  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if (event == EVENT_CAST_SPELL && g_affected_card == card && g_affected_card_controller == player)
  {
    g_ai_modifier += 0x30 - g_duel_summary.hand_counts[player] * 0x18;
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    player_index = 0;
    current_player = current_player;
    while (player_index < 2)
    {
      current_card = 0;
      while (current_card < g_active_cards_count[current_player])
      {
        if (is_card_pending_resolution(current_player, current_card) != 0)
        {
          put_card_on_bottom_of_library(current_player, PLAYER_CARD_INSTANCE(current_player, current_card).internal_card_id);
          PLAYER_CARD_INSTANCE(current_player, current_card).internal_card_id = -1;
        }
        ++current_card;
      }

      current_card = 0;
      while (current_card < 500)
      {
        if (global_graveyard_slots[current_player][current_card] == -1)
        {
          break;
        }

        put_card_on_bottom_of_library(current_player, global_graveyard_slots[current_player][current_card]);
        ++current_card;
      }

      current_card = 0;
      while (current_card < 500)
      {
        global_graveyard_slots[current_player][current_card] = -1;
        ++current_card;
      }

      TENTATIVE_reassess_all_cards(0, 0x30);
      shuffle_duel_library(player, current_player);
      draw_cards_and_set_hand_count(current_player, 7);

      ++player_index;
      if (current_player != 0)
      {
        --current_player;
      }
      else
      {
        ++current_player;
      }
    }

    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x0040246a
// FUNCTION: SHANDALAR 0x0044818c
void draw_cards_and_set_hand_count(int player, int amount)
{
  int cards_drawn;

  for (cards_drawn = 0; cards_drawn < amount; ++cards_drawn)
  {
    draw_card_for_player(player);
    if (player != 0)
    {
      g_hand_count_adjustment = 0;
    }
  }
  g_duel_summary.hand_counts[player] = amount;
}

// FUNCTION: MAGIC 0x004024c5
// FUNCTION: SHANDALAR 0x004481e7
int card_channel(int player, int card, event_t event)
{
  PLAYER_CARD_INSTANCE(player, card).mana_color = 1;
  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    create_legacy_effect(player, card, g_duel_generated_internal_card_id_0e, -1, -1);
    declare_mana_available(player, COLOR_COLORLESS, 1);
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x00402560
// FUNCTION: SHANDALAR 0x00448282
int card_energy_tap(int player, int card, event_t event)
{
  target_t target;

  if (event == EVENT_CAN_CAST)
  {
    return real_target_available((int *)0,
                                 TARGET_SCAN_DIRECT,
                                 player,
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
                                 -1,
                                 0,
                                 0,
                                 TARGET_STATE_TAPPED);
  }

  if (event == EVENT_CAST_SPELL && g_affected_card == card && g_affected_card_controller == player)
  {
    if (g_duel_ai_mode_state != 1)
    {
      load_text("prompts.txt", "ENERGYTAP");
    }
    if (C_real_select_target(player,
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
                             -1,
                             0,
                             0,
                             TARGET_STATE_TAPPED,
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

  if (event == EVENT_RESOLVE_SPELL)
  {
    SET_TARGET(target, PLAYER_CARD_INSTANCE(player, card).targets[0]);
    if (C_real_validate_target(target.player,
                               target.card,
                               (char *)0,
                               player,
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
                               -1,
                               0,
                               0,
                               TARGET_STATE_TAPPED))
    {
      tap_card_and_dispatch_event(target.player, target.card);
      g_raw_mana_available[player][COLOR_COLORLESS] +=
          (int)(char)global_cards_data[PLAYER_CARD_INSTANCE(target.player, target.card).internal_card_id].cc[0] +
          ClampIntToRange((int)(char)global_cards_data[PLAYER_CARD_INSTANCE(target.player, target.card).internal_card_id].cc[1], 0, 99);
      g_raw_mana_available[player][COLOR_ANY] +=
          (int)(char)global_cards_data[PLAYER_CARD_INSTANCE(target.player, target.card).internal_card_id].cc[0] +
          ClampIntToRange((int)(char)global_cards_data[PLAYER_CARD_INSTANCE(target.player, target.card).internal_card_id].cc[1], 0, 99);
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

// FUNCTION: MAGIC 0x0040293f
// FUNCTION: SHANDALAR 0x00448662
int card_stream_of_life(int player, int card, event_t event)
{
  target_t target;

  if (event == EVENT_CAN_CAST)
  {
    if (((g_other_player == player) && ((g_duel_network_flags & 2) == 0)) && has_mana(player, 7, 2) == 0)
    {
      return 0;
    }
    return 1;
  }

  if (((event == EVENT_CAST_SPELL) && (g_affected_card == card)) && (g_affected_card_controller == player))
  {
    g_ai_modifier -= (g_life[player] * 0x18) / count_active_card_instances_plus_one(player, PLAYER_CARD_INSTANCE(player, card).internal_card_id);
    if (g_duel_ai_mode_state != 1)
    {
      load_text("prompts.txt", "STREAMOFLIFE");
    }
    if (C_real_select_target(player,
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
                             &target) != 0)
    {
      PLAYER_CARD_INSTANCE(player, card).info_slot = g_x_value;
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
    gain_life(PLAYER_CARD_INSTANCE(player, card).targets[0].player, PLAYER_CARD_INSTANCE(player, card).info_slot, player, card);
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x00402c15
// FUNCTION: SHANDALAR 0x00448939
int card_tranquility(int player, int card, event_t event)
{
  int current_card;
  int current_player;

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
        if (is_in_play(current_player, current_card) && (global_cards_data[PLAYER_CARD_INSTANCE(current_player, current_card).internal_card_id].type & TYPE_ENCHANTMENT) != 0)
        {
          kill_card(current_player, current_card, KILL_DESTROY);
        }
      }
    }
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x00402d0c
// FUNCTION: SHANDALAR 0x00448a30
int card_volcanic_eruption(int player, int card, event_t event)
{
  target_t selected_target;
  int required_subtype;
  int current_target;
  int selecting_done;
  int invalid_targets;
  int damage_effect_card;
  char prompt[52];

  if (event == EVENT_CAN_CAST)
  {
    real_target_available(g_activation_scan_flags == 0 ? &g_max_x_value : (int *)0,
                          TARGET_SCAN_DIRECT,
                          player,
                          2,
                          2,
                          TARGET_ZONE_IN_PLAY,
                          TYPE_NONE,
                          TYPE_NONE,
                          0,
                          get_protections_from(player, card),
                          0,
                          0,
                          3,
                          -1,
                          -1,
                          -1,
                          0,
                          0,
                          0);
    if (player == g_current_player && (g_duel_network_flags & 2) == 0 && (g_max_x_value == 0 || has_mana(player, 7, 4) == 0))
    {
      return 0;
    }
    return 1;
  }

  if (event == EVENT_CAST_SPELL && card == g_affected_card && player == g_affected_card_controller)
  {
    g_ai_modifier -= 0x24 / count_active_card_instances_plus_one(player, PLAYER_CARD_INSTANCE(player, card).internal_card_id);
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    required_subtype = get_hacked_color(player, card, 4) - 1;
    current_target = 0;
    selecting_done = 0;

    while (current_target < g_x_value && selecting_done == 0 && g_spell_fizzled != 1)
    {
      if (g_duel_ai_mode_state != 1)
      {
        load_text("prompts.txt", "VOLCANIC_ERUPTION");
        sprintf(prompt, g_text_lines[0], current_target + 1, g_x_value);
      }

      if (C_real_select_target(player,
                               2,
                               1 - player,
                               TARGET_ZONE_IN_PLAY,
                               TYPE_NONE,
                               TYPE_NONE,
                               0,
                               get_protections_from(player, card),
                               COLOR_TEST_0,
                               COLOR_TEST_0,
                               required_subtype,
                               ~SUB_WALL,
                               -1,
                               -1,
                               0,
                               0,
                               0,
                               prompt,
                               1,
                               &selected_target))
      {
        PLAYER_CARD_INSTANCE(selected_target.player, selected_target.card).state |= (STATE_CANNOT_TARGET | STATE_TARGETTED);
        TENTATIVE_reassess_all_cards(0, 0x20);
        SET_TARGET(PLAYER_CARD_INSTANCE(player, card).targets[PLAYER_CARD_INSTANCE(player, card).number_of_targets], selected_target);
        ++PLAYER_CARD_INSTANCE(player, card).number_of_targets;
      }
      else
      {
        if (selected_target.card == -1)
        {
          g_spell_fizzled = 1;
        }
        else
        {
          selecting_done = 1;
        }
      }

      ++current_target;
    }

    for (current_target = 0; current_target < PLAYER_CARD_INSTANCE(player, card).number_of_targets; ++current_target)
    {
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).targets[current_target].player,
                           PLAYER_CARD_INSTANCE(player, card).targets[current_target].card)
          .state &= ~(STATE_CANNOT_TARGET | STATE_TARGETTED);
    }

    if (g_spell_fizzled == 1)
    {
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    required_subtype = get_hacked_color(player, card, 4) - 1;
    invalid_targets = 0;

    for (current_target = 0; current_target < PLAYER_CARD_INSTANCE(player, card).number_of_targets; ++current_target)
    {
      if (C_real_validate_target(PLAYER_CARD_INSTANCE(player, card).targets[current_target].player,
                                 PLAYER_CARD_INSTANCE(player, card).targets[current_target].card,
                                 (char *)0,
                                 player,
                                 2,
                                 2,
                                 TARGET_ZONE_IN_PLAY,
                                 TYPE_NONE,
                                 TYPE_NONE,
                                 0,
                                 get_protections_from(player, card),
                                 COLOR_TEST_0,
                                 COLOR_TEST_0,
                                 required_subtype,
                                 ~SUB_WALL,
                                 -1,
                                 -1,
                                 0,
                                 0,
                                 0))
      {
        kill_card(PLAYER_CARD_INSTANCE(player, card).targets[current_target].player,
                  PLAYER_CARD_INSTANCE(player, card).targets[current_target].card,
                  KILL_DESTROY);
      }
      else
      {
        ++invalid_targets;
      }
    }

    if (PLAYER_CARD_INSTANCE(player, card).number_of_targets == invalid_targets)
    {
      g_spell_fizzled = 1;
    }

    if (g_spell_fizzled != 1 && (damage_effect_card = add_card_to_hand(player, g_duel_generated_internal_card_id_12)) != -1)
    {
      PLAYER_CARD_INSTANCE(player, damage_effect_card).original_internal_card_id = PLAYER_CARD_INSTANCE(player, card).internal_card_id;
      PLAYER_CARD_INSTANCE(player, damage_effect_card).state |= 2;
      PLAYER_CARD_INSTANCE(player, damage_effect_card).display_pic_info = 0x109;
      PLAYER_CARD_INSTANCE(player, damage_effect_card).info_slot = PLAYER_CARD_INSTANCE(player, card).number_of_targets - invalid_targets;
      TENTATIVE_set_timestamps(player, damage_effect_card);
    }

    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x00403557
// FUNCTION: SHANDALAR 0x0044927d
int card_earthquake(int player, int card, event_t event)
{
  int current_card;
  int current_player;

  if (event == EVENT_CAN_CAST)
  {
    if (((player == g_other_player) && ((g_duel_network_flags & 2) == 0)) && has_mana(player, COLOR_ANY, 2) == 0)
    {
      return 0;
    }
    return 1;
  }

  if (((event == EVENT_CAST_SPELL) && (g_affected_card == card)) && (g_affected_card_controller == player))
  {
    g_ai_modifier -= 0x24 / count_active_card_instances_plus_one(player, PLAYER_CARD_INSTANCE(player, card).internal_card_id);
    PLAYER_CARD_INSTANCE(player, card).info_slot = g_x_value;
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    current_player = player;
    while (current_player >= 0 && current_player < 2)
    {
      damage_player(current_player, PLAYER_CARD_INSTANCE(player, card).info_slot, player, card);
      for (current_card = 0; current_card < g_active_cards_count[current_player]; ++current_card)
      {
        if (is_in_play(current_player, current_card) && (global_cards_data[PLAYER_CARD_INSTANCE(current_player, current_card).internal_card_id].type & TYPE_CREATURE) != 0 && (C_get_abilities(current_player, current_card, EVENT_ABILITIES, -1) & KEYWORD_FLYING) == 0)
        {
          damage_creature(current_player, current_card, PLAYER_CARD_INSTANCE(player, card).info_slot, player, card);
        }
      }
      if (player == 0)
      {
        ++current_player;
      }
      else
      {
        --current_player;
      }
    }
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004037e1
// FUNCTION: SHANDALAR 0x00449507
int card_hurricane(int player, int card, event_t event)
{
  int current_card;
  int current_player;

  if (event == EVENT_CAN_CAST)
  {
    if (((player == g_other_player) && ((g_duel_network_flags & 2) == 0)) && has_mana(player, COLOR_ANY, 2) == 0)
    {
      return 0;
    }
    return 1;
  }

  if (((event == EVENT_CAST_SPELL) && (g_affected_card == card)) && (g_affected_card_controller == player))
  {
    g_ai_modifier -= 0x24 / count_active_card_instances_plus_one(player, PLAYER_CARD_INSTANCE(player, card).internal_card_id);
    PLAYER_CARD_INSTANCE(player, card).info_slot = g_x_value;
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    current_player = player;
    while (current_player >= 0 && current_player < 2)
    {
      damage_player(current_player, PLAYER_CARD_INSTANCE(player, card).info_slot, player, card);
      for (current_card = 0; current_card < g_active_cards_count[current_player]; ++current_card)
      {
        if (is_in_play(current_player, current_card) && (global_cards_data[PLAYER_CARD_INSTANCE(current_player, current_card).internal_card_id].type & TYPE_CREATURE) != 0 && (C_get_abilities(current_player, current_card, EVENT_ABILITIES, -1) & KEYWORD_FLYING) != 0)
        {
          damage_creature(current_player, current_card, PLAYER_CARD_INSTANCE(player, card).info_slot, player, card);
        }
      }
      if (player == 0)
      {
        ++current_player;
      }
      else
      {
        --current_player;
      }
    }
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x00403a6b
// FUNCTION: SHANDALAR 0x00449791
int card_armageddon(int player, int card, event_t event)
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
        if (is_in_play(current_player, current_card) && (global_cards_data[PLAYER_CARD_INSTANCE(current_player, current_card).internal_card_id].type & TYPE_LAND) != 0)
        {
          kill_card(current_player, current_card, KILL_DESTROY);
        }
      }
    }
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x00403b62
// FUNCTION: SHANDALAR 0x00449888
int card_tsunami(int player, int card, event_t event)
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
        if (is_in_play(current_player, current_card) && (global_cards_data[PLAYER_CARD_INSTANCE(current_player, current_card).internal_card_id].type & TYPE_LAND) != 0)
        {
          if (card_has_basic_land_type(current_player, current_card, get_hacked_color(player, card, 2)) != 0)
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

// FUNCTION: MAGIC 0x00403c84
// FUNCTION: SHANDALAR 0x004499aa
int card_ashes_to_ashes(int player, int card, event_t event)
{
  int invalid_targets;
  int target_count;
  int target_index;

  if (event == EVENT_CAN_CAST)
  {
    real_target_available(&target_count,
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
                          COLOR_TEST_0,
                          -1,
                          -1,
                          -1,
                          -1,
                          0,
                          0,
                          0);
    return target_count >= 2;
  }

  if (event == EVENT_CAST_SPELL && card == g_affected_card && player == g_affected_card_controller)
  {
    target_index = 0;
    while (target_index < 2 && g_spell_fizzled != 1)
    {
      if (g_duel_ai_mode_state != 1)
      {
        load_text("prompts.txt", "ASHESTOASHES");
      }

      if (C_real_select_target(player,
                               2,
                               1 - player,
                               TARGET_ZONE_IN_PLAY,
                               TYPE_CREATURE,
                               TYPE_ARTIFACT,
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
                               g_text_lines[target_index],
                               1,
                               &PLAYER_CARD_INSTANCE(player, card).targets[target_index]))
      {
        PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).targets[target_index].player,
                             PLAYER_CARD_INSTANCE(player, card).targets[target_index].card)
            .state |= STATE_CANNOT_TARGET | STATE_TARGETTED;
        TENTATIVE_reassess_all_cards(0, 0x20);
      }
      else
      {
        g_spell_fizzled = 1;
      }
      ++target_index;
    }

    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 2;
    for (target_index = 0;
         target_index < PLAYER_CARD_INSTANCE(player, card).number_of_targets;
         ++target_index)
    {
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).targets[target_index].player,
                           PLAYER_CARD_INSTANCE(player, card).targets[target_index].card)
          .state &= ~(STATE_CANNOT_TARGET | STATE_TARGETTED);
    }

    if (g_spell_fizzled == 1)
    {
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    invalid_targets = 0;
    for (target_index = 0;
         target_index < PLAYER_CARD_INSTANCE(player, card).number_of_targets;
         ++target_index)
    {
      if (C_real_validate_target(PLAYER_CARD_INSTANCE(player, card).targets[target_index].player,
                                 PLAYER_CARD_INSTANCE(player, card).targets[target_index].card,
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
                                 COLOR_TEST_0,
                                 -1,
                                 -1,
                                 -1,
                                 -1,
                                 0,
                                 0,
                                 0))
      {
        kill_card(PLAYER_CARD_INSTANCE(player, card).targets[target_index].player,
                  PLAYER_CARD_INSTANCE(player, card).targets[target_index].card,
                  KILL_REMOVE);
      }
      else
      {
        ++invalid_targets;
      }
    }

    if (invalid_targets == 2)
    {
      g_spell_fizzled = 1;
    }
    else
    {
      damage_player(player, 5, player, card);
    }

    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x0040420e
// FUNCTION: SHANDALAR 0x00449f2c
int card_desert_twister(int player, int card, event_t event)
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
                                 TARGET_TYPE_PERMANENT,
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
    g_ai_modifier -= 0x30 / count_active_card_instances_plus_one(player, PLAYER_CARD_INSTANCE(player, card).internal_card_id);
    if (g_duel_ai_mode_state != 1)
    {
      load_text("prompts.txt", "DESERT_TWISTER");
    }

    if (C_real_select_target(player,
                             2,
                             1 - player,
                             TARGET_ZONE_IN_PLAY,
                             TARGET_TYPE_PERMANENT,
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

    if (C_real_validate_target(target.player,
                               target.card,
                               (char *)0,
                               player,
                               2,
                               2,
                               TARGET_ZONE_IN_PLAY,
                               TARGET_TYPE_PERMANENT,
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

// FUNCTION: MAGIC 0x0040452a
// FUNCTION: SHANDALAR 0x0044a249
int card_winter_blast(int player, int card, event_t event)
{
  int stop_selecting;
  int target_index;
  target_t selected_target;

  if (event == EVENT_CAN_CAST)
  {
    real_target_available(g_activation_scan_flags == 0 ? &g_max_x_value : (int *)0,
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
    if (player == g_other_player &&
        (g_duel_network_flags & 2) == 0 &&
        (g_max_x_value == 0 || !has_mana(player, COLOR_ANY, 2)))
    {
      return 0;
    }
    return 1;
  }

  if (event == EVENT_CAST_SPELL && g_affected_card == card && g_affected_card_controller == player)
  {
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    target_index = 0;
    stop_selecting = 0;

    for (; target_index < g_x_value && stop_selecting == 0 && g_spell_fizzled != 1; ++target_index)
    {
      if (g_duel_ai_mode_state != 1)
      {
        load_text("prompts.txt", "WINTER_BLAST");
        sprintf(g_text_lines[0], g_text_lines[0], target_index + 1, g_x_value);
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
                               &selected_target))
      {
        PLAYER_CARD_INSTANCE(selected_target.player, selected_target.card).state |=
            STATE_CANNOT_TARGET | STATE_TARGETTED;
        TENTATIVE_reassess_all_cards(0, 0x20);
        SET_TARGET(PLAYER_CARD_INSTANCE(player, card).targets[PLAYER_CARD_INSTANCE(player, card).number_of_targets], selected_target);
        ++PLAYER_CARD_INSTANCE(player, card).number_of_targets;
      }
      else
      {
        if (selected_target.card == -1)
        {
          g_spell_fizzled = 1;
        }
        else
        {
          stop_selecting = 1;
        }
      }
    }

    for (target_index = 0; target_index < PLAYER_CARD_INSTANCE(player, card).number_of_targets; ++target_index)
    {
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).targets[target_index].player,
                           PLAYER_CARD_INSTANCE(player, card).targets[target_index].card)
          .state &= ~(STATE_CANNOT_TARGET | STATE_TARGETTED);
    }

    if (g_spell_fizzled == 1)
    {
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    for (target_index = 0; target_index < PLAYER_CARD_INSTANCE(player, card).number_of_targets; ++target_index)
    {
      if (C_real_validate_target(PLAYER_CARD_INSTANCE(player, card).targets[target_index].player,
                                 PLAYER_CARD_INSTANCE(player, card).targets[target_index].card,
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
        tap_card_and_dispatch_event(PLAYER_CARD_INSTANCE(player, card).targets[target_index].player,
                                    PLAYER_CARD_INSTANCE(player, card).targets[target_index].card);
        if ((C_get_abilities(PLAYER_CARD_INSTANCE(player, card).targets[target_index].player,
                             PLAYER_CARD_INSTANCE(player, card).targets[target_index].card,
                             EVENT_ABILITIES,
                             -1) &
             KEYWORD_FLYING) != 0)
        {
          damage_creature(PLAYER_CARD_INSTANCE(player, card).targets[target_index].player,
                          PLAYER_CARD_INSTANCE(player, card).targets[target_index].card,
                          2,
                          player,
                          card);
        }
      }
      else
      {
        g_spell_fizzled = 1;
      }
    }

    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x00404c4c
// FUNCTION: SHANDALAR 0x0044a96c
int count_active_card_instances_plus_one(int player, int internal_card_id)
{
  int card;
  int result;

  result = 1;
  for (card = 0; card < g_active_cards_count[player]; ++card)
  {
    if (PLAYER_CARD_INSTANCE(player, card).internal_card_id == internal_card_id && (PLAYER_CARD_INSTANCE(player, card).state & 2) == 0)
    {
      ++result;
    }
  }

  return result;
}

// FUNCTION: MAGIC 0x00404cff
// FUNCTION: SHANDALAR 0x0044aa1f
int count_permanents_by_internal_card_id(int player, int internal_card_id, int who_to_check)
{
  int card;
  int result;

  result = 0;
  for (player = 0; player < 2; ++player)
  {
    if (who_to_check == -1 || who_to_check == player)
    {
      for (card = 0; card < g_active_cards_count[player]; ++card)
      {
        if (is_in_play(player, card) && PLAYER_CARD_INSTANCE(player, card).internal_card_id == internal_card_id)
        {
          ++result;
        }
      }
    }
  }

  return result;
}

// FUNCTION: MAGIC 0x00404dce
// FUNCTION: SHANDALAR 0x0044aaee
int card_fireball(int player, int card, event_t event)
{
  struct
  {
    int player_target_selected[2];
    int possible_targets;
    int damage_per_target;
    int target_index;
    int select_ok;
    target_t chosen_target;
    int num_targets;
  } s;
  int result;
  int mana_available_minus_one;
  int mana_to_pay;
  int num_selected_targets;
  int invalid_targets;
  unsigned int illegal_abilities;

  if (event == EVENT_CAN_CAST)
  {
    if (player == g_active_player || (g_duel_network_flags & 2) != 0)
    {
      g_adventure_world_state = 1;
    }
    else if (has_mana(player, 7, 2) == 0)
    {
      return 0;
    }

    return 1;
  }
  else
  {
    if (event == EVENT_CAST_SPELL && card == g_affected_card && player == g_affected_card_controller)
    {
      result = count_active_card_instances_plus_one(player, PLAYER_CARD_INSTANCE(player, card).internal_card_id);
      g_ai_modifier -= 0x48 / result;

      if (((player == g_other_player) && ((g_duel_network_flags & 2) == 0)) || g_duel_ai_mode_state == 1)
      {
        PLAYER_CARD_INSTANCE(player, card).info_slot = g_x_value;

        if (g_duel_ai_mode_state == 1)
        {
          if ((g_land_can_be_played & LCBP_CARD_BEING_COPIED) == 0)
          {
            illegal_abilities = get_protections_from(player, card);
            real_target_available(&s.possible_targets,
                                  TARGET_SCAN_DIRECT,
                                  player,
                                  2,
                                  1 - player,
                                  TARGET_ZONE_IN_PLAY,
                                  TYPE_CREATURE,
                                  TYPE_NONE,
                                  0,
                                  illegal_abilities,
                                  0,
                                  0,
                                  -1,
                                  -1,
                                  0xffffffff,
                                  0xffffffff,
                                  0,
                                  0,
                                  0);
            ++s.possible_targets;
            if (s.possible_targets > 5)
            {
              s.possible_targets = 5;
            }
            g_ai_recorded_choice =
                ClampIntToRange(internal_rand((g_x_value + 1) / 2) + 1, 1, s.possible_targets);
          }
          else
          {
            g_ai_recorded_choice = PLAYER_CARD_INSTANCE(g_current_spell_player, g_current_spell_card).number_of_targets;
          }

          record_ai_action_selection();
        }
        else
        {
          replay_ai_action_selection();
        }

        s.num_targets = g_ai_recorded_choice;
        if (s.num_targets == 99)
        {
          s.num_targets = 1;
        }

        result = g_x_value - s.num_targets;
        s.player_target_selected[0] = 0;

        for (s.target_index = 0; s.target_index < s.num_targets; ++s.target_index)
        {
          do
          {
            select_damage_target(player, card, s.damage_per_target);
            if (s.player_target_selected[0] == 0)
            {
              break;
            }
          } while (PLAYER_CARD_INSTANCE(player, card).targets[0].card == -1);

          PLAYER_CARD_INSTANCE(player, card).targets[(s.num_targets - 1) - s.target_index].player =
              PLAYER_CARD_INSTANCE(player, card).targets[0].player;
          PLAYER_CARD_INSTANCE(player, card).targets[(s.num_targets - 1) - s.target_index].card =
              PLAYER_CARD_INSTANCE(player, card).targets[0].card;
          if (PLAYER_CARD_INSTANCE(player, card).targets[0].card == -1)
          {
            s.player_target_selected[0] = 1;
          }
          else
          {
            PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                                 PLAYER_CARD_INSTANCE(player, card).targets[0].card)
                .state |= STATE_CANNOT_TARGET | STATE_TARGETTED;
          }
        }

        PLAYER_CARD_INSTANCE(player, card).number_of_targets = s.num_targets;
        PLAYER_CARD_INSTANCE(player, card).info_slot = (result + 1) / s.num_targets;

        for (s.target_index = 0; s.target_index < s.num_targets; ++s.target_index)
        {
          if (PLAYER_CARD_INSTANCE(player, card).targets[s.target_index].card != -1)
          {
            PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).targets[s.target_index].player,
                                 PLAYER_CARD_INSTANCE(player, card).targets[s.target_index].card)
                .state &= ~0x300000;
          }
        }
      }
      else if ((g_land_can_be_played & LCBP_CARD_BEING_COPIED) == 0)
      {
        mana_available_minus_one = has_mana(player, COLOR_ANY, 1) - 1;
        illegal_abilities = get_protections_from(player, card);
        real_target_available(&s.possible_targets,
                              TARGET_SCAN_DIRECT,
                              player,
                              2,
                              2,
                              TARGET_ZONE_IN_PLAY,
                              TYPE_CREATURE,
                              TYPE_NONE,
                              0,
                              illegal_abilities,
                              0,
                              0,
                              -1,
                              -1,
                              0xffffffff,
                              0xffffffff,
                              0,
                              0,
                              0);
        s.possible_targets += 2;
        mana_to_pay = mana_available_minus_one;
        num_selected_targets = 1;
        if (choose_fireball_options(player,
                                    PLAYER_CARD_INSTANCE(player, card).internal_card_id,
                                    mana_available_minus_one,
                                    s.possible_targets,
                                    &mana_to_pay,
                                    &num_selected_targets,
                                    &s.damage_per_target) == 0)
        {
          g_spell_fizzled = 1;
        }
        else
        {
          PLAYER_CARD_INSTANCE(player, card).info_slot = s.damage_per_target;
          g_x_value = 0;
          g_mana_charge[4] = 1;
          charge_mana(player, COLOR_COLORLESS, mana_to_pay);
          if (g_spell_fizzled != 1)
          {
            g_x_value = mana_to_pay - (num_selected_targets - 1);
            PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
            s.player_target_selected[1] = 0;
            s.player_target_selected[0] = 0;
            s.target_index = 0;
            while (s.target_index < num_selected_targets && g_spell_fizzled != 1)
            {
              do
              {
                if (g_duel_ai_mode_state != 1)
                {
                  load_text("prompts.txt", "FIREBALL");
                  sprintf(g_text_lines[0], g_text_lines[0], s.target_index + 1, num_selected_targets);
                }

                s.select_ok = C_real_select_target(player,
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
                                                   g_text_lines[0],
                                                   1,
                                                   &s.chosen_target);

                if (g_duel_ai_mode_state != 1 && s.chosen_target.card == -1 &&
                    s.player_target_selected[s.chosen_target.player] != 0)
                {
                  set_duel_prompt_text(g_text_lines[1]);
                  Sleep(0x9c4);
                  set_duel_prompt_text("");
                }
              } while (s.select_ok != 0 && s.chosen_target.card == -1 &&
                       s.player_target_selected[s.chosen_target.player] != 0);

              if (s.select_ok == 0)
              {
                g_spell_fizzled = 1;
              }
              else
              {
                if (s.chosen_target.card == -1)
                {
                  s.player_target_selected[s.chosen_target.player] = 1;
                }
                else
                {
                  PLAYER_CARD_INSTANCE(s.chosen_target.player, s.chosen_target.card).state |=
                      STATE_CANNOT_TARGET | STATE_TARGETTED;
                  TENTATIVE_reassess_all_cards(0, 0x20);
                }

                PLAYER_CARD_INSTANCE(player, card)
                    .targets[PLAYER_CARD_INSTANCE(player, card).number_of_targets]
                    .player = s.chosen_target.player;
                PLAYER_CARD_INSTANCE(player, card)
                    .targets[PLAYER_CARD_INSTANCE(player, card).number_of_targets]
                    .card = s.chosen_target.card;
                ++PLAYER_CARD_INSTANCE(player, card).number_of_targets;
              }

              ++s.target_index;
            }

            for (s.target_index = 0;
                 s.target_index < PLAYER_CARD_INSTANCE(player, card).number_of_targets;
                 ++s.target_index)
            {
              if (PLAYER_CARD_INSTANCE(player, card).targets[s.target_index].card != -1)
              {
                PLAYER_CARD_INSTANCE(
                    PLAYER_CARD_INSTANCE(player, card).targets[s.target_index].player,
                    PLAYER_CARD_INSTANCE(player, card).targets[s.target_index].card)
                    .state &= ~0x300000;
              }
            }

            if (g_spell_fizzled == 1)
            {
              PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
            }
          }
        }
      }
      else
      {
        PLAYER_CARD_INSTANCE(player, card).info_slot =
            PLAYER_CARD_INSTANCE(g_current_spell_player, g_current_spell_card).info_slot;
        num_selected_targets = PLAYER_CARD_INSTANCE(g_current_spell_player, g_current_spell_card).number_of_targets;
        PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
        s.player_target_selected[1] = 0;
        s.player_target_selected[0] = 0;
        s.target_index = 0;
        while (s.target_index < num_selected_targets && g_spell_fizzled != 1)
        {
          do
          {
            if (g_duel_ai_mode_state != 1)
            {
              load_text("prompts.txt", "FIREBALL");
              sprintf(g_text_lines[0], g_text_lines[0], s.target_index + 1, num_selected_targets);
            }

            s.select_ok = C_real_select_target(player,
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
                                               g_text_lines[0],
                                               1,
                                               &s.chosen_target);

            if (g_duel_ai_mode_state != 1 && s.chosen_target.card == -1 &&
                s.player_target_selected[s.chosen_target.player] != 0)
            {
              set_duel_prompt_text(g_text_lines[1]);
              Sleep(0x9c4);
              set_duel_prompt_text("");
            }
          } while (s.select_ok != 0 && s.chosen_target.card == -1 &&
                   s.player_target_selected[s.chosen_target.player] != 0);

          if (s.select_ok == 0)
          {
            g_spell_fizzled = 1;
          }
          else
          {
            if (s.chosen_target.card == -1)
            {
              s.player_target_selected[s.chosen_target.player] = 1;
            }
            else
            {
              PLAYER_CARD_INSTANCE(s.chosen_target.player, s.chosen_target.card).state |=
                  STATE_CANNOT_TARGET | STATE_TARGETTED;
              TENTATIVE_reassess_all_cards(0, 0x20);
            }

            PLAYER_CARD_INSTANCE(player, card)
                .targets[PLAYER_CARD_INSTANCE(player, card).number_of_targets]
                .player = s.chosen_target.player;
            PLAYER_CARD_INSTANCE(player, card)
                .targets[PLAYER_CARD_INSTANCE(player, card).number_of_targets]
                .card = s.chosen_target.card;
            ++PLAYER_CARD_INSTANCE(player, card).number_of_targets;
          }

          ++s.target_index;
        }

        for (s.target_index = 0;
             s.target_index < PLAYER_CARD_INSTANCE(player, card).number_of_targets;
             ++s.target_index)
        {
          if (PLAYER_CARD_INSTANCE(player, card).targets[s.target_index].card != -1)
          {
            PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).targets[s.target_index].player,
                                 PLAYER_CARD_INSTANCE(player, card).targets[s.target_index].card)
                .state &= ~0x300000;
          }
        }

        if (g_spell_fizzled == 1)
        {
          PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
        }
      }
    }

    if (event == EVENT_RESOLVE_SPELL)
    {
      invalid_targets = 0;
      if (player == g_other_player && (g_duel_network_flags & 2) == 0)
      {
        result = PLAYER_CARD_INSTANCE(player, card).info_slot;
        for (s.target_index = 0;
             s.target_index < PLAYER_CARD_INSTANCE(player, card).number_of_targets;
             ++s.target_index)
        {
          PLAYER_CARD_INSTANCE(player, card).targets[0].player =
              PLAYER_CARD_INSTANCE(player, card).targets[s.target_index].player;
          PLAYER_CARD_INSTANCE(player, card).targets[0].card =
              PLAYER_CARD_INSTANCE(player, card).targets[s.target_index].card;
          deal_damage_to_selected_target(player, card, EVENT_RESOLVE_SPELL, result);
        }
      }
      else
      {
        illegal_abilities = get_protections_from(player, card);
        for (s.target_index = 0;
             s.target_index < PLAYER_CARD_INSTANCE(player, card).number_of_targets;
             ++s.target_index)
        {
          if (C_real_validate_target(
                  PLAYER_CARD_INSTANCE(player, card).targets[s.target_index].player,
                  PLAYER_CARD_INSTANCE(player, card).targets[s.target_index].card,
                  (char *)0,
                  player,
                  2,
                  2,
                  TARGET_ZONE_PLAYERS | TARGET_ZONE_IN_PLAY,
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
                  0))
          {
            damage_creature(PLAYER_CARD_INSTANCE(player, card).targets[s.target_index].player,
                            PLAYER_CARD_INSTANCE(player, card).targets[s.target_index].card,
                            PLAYER_CARD_INSTANCE(player, card).info_slot,
                            player,
                            card);
          }
          else
          {
            ++invalid_targets;
          }
        }
      }

      if (PLAYER_CARD_INSTANCE(player, card).number_of_targets == invalid_targets)
      {
        g_spell_fizzled = 1;
      }

      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
      kill_card(player, card, KILL_BURY);
    }

    return 0;
  }
}

// FUNCTION: MAGIC 0x004060cd
// FUNCTION: SHANDALAR 0x0044bde2
int card_detonate(int player, int card, event_t event)
{
  int converted_cost;
  int internal_card_id;
  target_t target;

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
                                 COLOR_TEST_0,
                                 COLOR_TEST_0,
                                 -1,
                                 -1,
                                 -1,
                                 -1,
                                 0,
                                 0,
                                 0) != 0;
  }

  if (event == EVENT_CAST_SPELL && g_affected_card == card && g_affected_card_controller == player)
  {
    if (g_duel_ai_mode_state != 1)
    {
      load_text("prompts.txt", "DETONATE");
    }

    if (C_real_select_target(player,
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
      PLAYER_CARD_INSTANCE(player, card).info_slot = g_x_value;
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
    if (C_real_validate_target(target.player,
                               target.card,
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
                               -1,
                               -1,
                               -1,
                               0,
                               0,
                               0))
    {
      internal_card_id = PLAYER_CARD_INSTANCE(target.player, target.card).internal_card_id;
      converted_cost = global_cards_data[internal_card_id].cc[0] + global_cards_data[internal_card_id].cc[1];
      if (converted_cost == PLAYER_CARD_INSTANCE(player, card).info_slot)
      {
        damage_player(target.player, PLAYER_CARD_INSTANCE(player, card).info_slot, player, card);
        kill_card(target.player, target.card, KILL_BURY);
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

    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004064d6
// FUNCTION: SHANDALAR 0x0044c1eb
int card_mana_clash(int player, int card, event_t event)
{
  int p1_flip;
  int p0_flip;
  char prompt1[300] = " ";
  char prompt2[300] = " ";

  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if (event == EVENT_CAST_SPELL && g_affected_card == card && g_affected_card_controller == player)
  {
    g_ai_modifier += ((g_life[player] - g_life[1 - player]) * 3) << 3;
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    if (g_duel_ai_mode_state != 1)
    {
      load_text("prompts.txt", "MANACLASH");
      strcpy(prompt1, g_text_lines[0]);
      strcpy(prompt2, g_text_lines[0] + 0x12c);

      do
      {
        p0_flip = coin_flip(player, player == g_active_player ? prompt1 : prompt2, 1);
        p1_flip = coin_flip(player, player == g_active_player ? prompt2 : prompt1, 1);

        if (p0_flip == 1)
        {
          damage_player(0, 1, player, card);
        }
        if (p1_flip == 1)
        {
          damage_player(1, 1, player, card);
        }
        if (p0_flip != 0 || p1_flip != 0)
        {
          do_dialog(player, player, card, -1, -1, g_text_lines[0] + 0x258, 0);
        }
      } while (p0_flip != 0 || p1_flip != 0);

      kill_card(player, card, KILL_BURY);
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x004066fb
// FUNCTION: SHANDALAR 0x0044c410
int card_word_of_binding(int player, int card, event_t event)
{
  int stop_selecting;
  int target_index;
  target_t selected_target;

  if (event == EVENT_CAN_CAST)
  {
    real_target_available(g_activation_scan_flags == 0 ? &g_max_x_value : (int *)0,
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
    if (player == g_other_player &&
        (g_duel_network_flags & 2) == 0 &&
        (g_max_x_value == 0 || !has_mana(player, COLOR_ANY, 3)))
    {
      return 0;
    }
    return 1;
  }

  if (event == EVENT_CAST_SPELL && g_affected_card == card && player == g_affected_card_controller)
  {
    PLAYER_CARD_INSTANCE(player, card).info_slot = g_x_value;
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    target_index = 0;
    stop_selecting = 0;

    while (target_index < g_x_value && stop_selecting == 0 && g_spell_fizzled != 1)
    {
      if (g_duel_ai_mode_state != 1)
      {
        load_text("prompts.txt", "WORDOFBINDING");
        sprintf(g_text_lines[0], g_text_lines[0], target_index + 1, g_x_value);
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
                               &selected_target))
      {
        PLAYER_CARD_INSTANCE(selected_target.player, selected_target.card).state |=
            STATE_CANNOT_TARGET | STATE_TARGETTED;
        TENTATIVE_reassess_all_cards(0, 0x20);
        SET_TARGET(PLAYER_CARD_INSTANCE(player, card).targets[PLAYER_CARD_INSTANCE(player, card).number_of_targets], selected_target);
        ++PLAYER_CARD_INSTANCE(player, card).number_of_targets;
      }
      else
      {
        if (selected_target.card == -1)
        {
          g_spell_fizzled = 1;
        }
        else
        {
          stop_selecting = 1;
        }
      }
      ++target_index;
    }

    for (target_index = 0; target_index < PLAYER_CARD_INSTANCE(player, card).number_of_targets; ++target_index)
    {
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).targets[target_index].player,
                           PLAYER_CARD_INSTANCE(player, card).targets[target_index].card)
          .state &= ~(STATE_CANNOT_TARGET | STATE_TARGETTED);
    }

    if (g_spell_fizzled == 1)
    {
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    for (target_index = 0; target_index < PLAYER_CARD_INSTANCE(player, card).number_of_targets; ++target_index)
    {
      if (C_real_validate_target(PLAYER_CARD_INSTANCE(player, card).targets[target_index].player,
                                 PLAYER_CARD_INSTANCE(player, card).targets[target_index].card,
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
        tap_card_and_dispatch_event(PLAYER_CARD_INSTANCE(player, card).targets[target_index].player,
                                    PLAYER_CARD_INSTANCE(player, card).targets[target_index].card);
      }
      else
      {
        g_spell_fizzled = 1;
      }
    }

    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x00406d4f
// FUNCTION: SHANDALAR 0x0044ca6b
int card_raise_dead(int player, int card, event_t event)
{
  struct
  {
    int hand_card;
    int graveyard_index;
    int result;
  } s;

  if (event == EVENT_CAN_CAST)
  {
    s.result = 0;
    for (s.hand_card = 0;
         s.hand_card < 500 && s.result == 0 && global_graveyard_slots[player][s.hand_card] != -1;
         ++s.hand_card)
    {
      if ((global_cards_data[global_graveyard_slots[player][s.hand_card]].type & TYPE_CREATURE) != 0)
      {
        s.result = 1;
      }
    }
    return s.result;
  }

  if (((event == EVENT_CAST_SPELL) && (card == g_affected_card)) && (player == g_affected_card_controller))
  {
    if (((player == g_other_player) && (g_duel_network_flags & 2) == 0) || g_duel_ai_mode_state == 1)
    {
      s.graveyard_index = find_highest_value_graveyard_card_by_type(player, 2);
    }
    else
    {
      if ((player == g_active_player) && (g_duel_ai_mode_state != 1))
      {
        load_text("prompts.txt", "RAISEDEAD");
      }
      do
      {
        s.graveyard_index =
            show_deck(player, global_graveyard_slots[player], 500, g_text_lines[0], 0, gs_cancel_008a8c20);
      } while (s.graveyard_index != -1 && (global_cards_data[global_graveyard_slots[player][s.graveyard_index]].type & TYPE_CREATURE) == 0);
    }

    if (s.graveyard_index != -1 && global_graveyard_slots[player][s.graveyard_index] != -1 && (global_cards_data[global_graveyard_slots[player][s.graveyard_index]].type & TYPE_CREATURE) != 0)
    {
      s.hand_card = add_card_to_hand(player, global_graveyard_slots[player][s.graveyard_index]);
      PLAYER_CARD_INSTANCE(player, s.hand_card).state |= 0x20;
      PLAYER_CARD_INSTANCE(player, card).targets[0].player = player;
      PLAYER_CARD_INSTANCE(player, card).targets[0].card = s.hand_card;
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
      PLAYER_CARD_INSTANCE(player, card).info_slot = s.graveyard_index;
    }
    else
    {
      g_spell_fizzled = 1;
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    s.graveyard_index = PLAYER_CARD_INSTANCE(player, card).info_slot;
    if (s.graveyard_index != -1 && global_graveyard_slots[player][s.graveyard_index] != -1 && (global_cards_data[global_graveyard_slots[player][s.graveyard_index]].type & TYPE_CREATURE) != 0)
    {
      remove_card_from_graveyard(player, s.graveyard_index);
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                           PLAYER_CARD_INSTANCE(player, card).targets[0].card)
          .state &= ~STATE_INVISIBLE;
      ++g_duel_summary.hand_counts[player];
    }
    else
    {
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                           PLAYER_CARD_INSTANCE(player, card).targets[0].card)
          .internal_card_id = -1;
    }
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x0040727c
// FUNCTION: SHANDALAR 0x0044cf98
int card_regrowth(int player, int card, event_t event)
{
  struct
  {
    int hand_card;
    int graveyard_index;
    int result;
  } s;

  if (event == EVENT_CAN_CAST)
  {
    s.result = 0;
    for (s.hand_card = 0;
         s.hand_card < 500 && s.result == 0 && global_graveyard_slots[player][s.hand_card] != -1;
         ++s.hand_card)
    {
      s.result = 1;
    }
    return s.result;
  }

  if (((event == EVENT_CAST_SPELL) && (g_affected_card == card)) && (g_affected_card_controller == player))
  {
    if (((player == g_other_player) && (g_duel_network_flags & 2) == 0) || g_duel_ai_mode_state == 1)
    {
      s.graveyard_index = find_highest_value_graveyard_card_by_type(player, -1);
    }
    else
    {
      if (player == g_active_player)
      {
        load_text("prompts.txt", "REGROWTH");
      }
      s.graveyard_index =
          show_deck(player, global_graveyard_slots[player], 500, g_text_lines[0], 0, gs_cancel_008a8c20);
    }

    if (s.graveyard_index != -1 && global_graveyard_slots[player][s.graveyard_index] != -1)
    {
      s.hand_card = add_card_to_hand(player, global_graveyard_slots[player][s.graveyard_index]);
      PLAYER_CARD_INSTANCE(player, s.hand_card).state |= 0x20;
      PLAYER_CARD_INSTANCE(player, card).targets[0].player = player;
      PLAYER_CARD_INSTANCE(player, card).targets[0].card = s.hand_card;
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
      PLAYER_CARD_INSTANCE(player, card).info_slot = s.graveyard_index;
    }
    else
    {
      g_spell_fizzled = 1;
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    s.graveyard_index = PLAYER_CARD_INSTANCE(player, card).info_slot;
    if (s.graveyard_index != -1 && global_graveyard_slots[player][s.graveyard_index] != -1)
    {
      remove_card_from_graveyard(player, s.graveyard_index);
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                           PLAYER_CARD_INSTANCE(player, card).targets[0].card)
          .state &= ~STATE_INVISIBLE;
      ++g_duel_summary.hand_counts[player];
    }
    else
    {
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                           PLAYER_CARD_INSTANCE(player, card).targets[0].card)
          .internal_card_id = -1;
    }
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004076dc
// FUNCTION: SHANDALAR 0x0044d3fa
int card_demonic_tutor(int player, int card, event_t event)
{
  int found_card;
  unsigned int type_mask;

  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if (event == 0x6c && g_affected_card == card && g_affected_card_controller == player && player == g_other_player && (g_duel_network_flags & 2) == 0 && global_library[player][0] == -1)
  {
    g_spell_fizzled = 1;
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    found_card = -1;

    if (((player == g_other_player) && (g_duel_network_flags & 2) == 0) || g_duel_ai_mode_state == 1 || g_duel_network_state != 0)
    {
      if (player == g_other_player)
      {
        if (g_duel_ai_mode_state == 1)
        {
          g_ai_recorded_choice = internal_rand(4);
          record_ai_action_selection();
        }
        else
        {
          replay_ai_action_selection();
        }

        switch (g_ai_recorded_choice)
        {
        default:
        case 0:
          type_mask = TYPE_CREATURE;
          break;
        case 1:
          type_mask = TYPE_ARTIFACT;
          break;
        case 2:
          type_mask = TYPE_SORCERY;
          break;
        case 3:
          type_mask = TYPE_INSTANT;
          break;
        }
      }
      else
      {
        type_mask = TYPE_CREATURE | TYPE_ARTIFACT;
      }

      found_card = choose_best_card_from_library(player, type_mask);
      if (found_card == -1)
      {
        found_card = choose_best_card_from_library(player, 0xffffffff);
      }
      if (found_card != -1 && global_library[player][found_card] != -1)
      {
        add_card_to_hand(player, global_library[player][found_card]);
        remove_card_from_deck(player, found_card);
      }
    }
    else
    {
      if (player == g_active_player && g_duel_ai_mode_state != 1)
      {
        load_text("prompts.txt", "DEMONIC_TUTOR");
      }
      found_card = show_deck(player, global_library[player], 500, g_text_lines[0], 1, gs_cancel_008a8c20);
      if (found_card != -1 && global_library[player][found_card] != -1)
      {
        add_card_to_hand(player, global_library[player][found_card]);
        remove_card_from_deck(player, found_card);
      }
    }

    if (found_card != -1)
    {
      ++g_duel_summary.hand_counts[player];
      TENTATIVE_reassess_all_cards(0, 0x30);
      shuffle_duel_library(player, player);
    }

    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004079f3
// FUNCTION: SHANDALAR 0x0044d711
int card_untamed_wilds(int player, int card, event_t event)
{
  struct
  {
    int test_index;
    int hand_card;
    int library_index;
    int found_basic_land;
  } s;

  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if (event == EVENT_CAST_SPELL && g_affected_card == card && player == g_affected_card_controller)
  {
    g_ai_modifier += 0x18;
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    if ((player == g_other_player && (g_duel_network_flags & 2) == 0) ||
        g_duel_ai_mode_state == 1 ||
        g_duel_network_state != 0)
    {
      s.library_index = find_highest_value_library_card_by_type(player, player, TYPE_LAND);
      if (s.library_index != -1 && global_library[player][s.library_index] > 4)
      {
        s.library_index = -1;
        for (s.test_index = 0;
             s.test_index < 500 && s.library_index == -1 && global_library[player][s.test_index] != -1;
             ++s.test_index)
        {
          if (global_library[player][s.test_index] <= 4)
          {
            s.library_index = s.test_index;
          }
        }
      }
    }
    else
    {
      if (player == g_active_player && g_duel_ai_mode_state != 1)
      {
        load_text("prompts.txt", "UNTAMED_WILDS");
      }

      s.found_basic_land = 0;
      for (s.test_index = 0;
           s.test_index < 500 && s.found_basic_land == 0 && global_library[player][s.test_index] != -1;
           ++s.test_index)
      {
        if (global_library[player][s.test_index] <= 4)
        {
          s.found_basic_land = 1;
        }
      }

      if (s.found_basic_land)
      {
        do
        {
          s.library_index = show_deck(player, global_library[player], 500, g_text_lines[0], 1, gs_cancel_008a8c20);
        } while (s.library_index != -1 && global_library[player][s.library_index] > 4);
      }
      else
      {
        s.library_index = -1;
        show_deck(player, global_library[player], 500, g_text_lines[0], 0, gs_done_008b40e0);
      }
    }

    if (s.library_index != -1 &&
        (global_library[player][s.library_index] == -1 || global_library[player][s.library_index] > 4))
    {
      s.library_index = -1;
    }

    if (s.library_index != -1 && global_library[player][s.library_index] != -1)
    {
      s.hand_card = add_card_to_hand(player, global_library[player][s.library_index]);
      if (s.hand_card != -1)
      {
        remove_card_from_deck(player, s.library_index);
        process_card_enters_play(player, s.hand_card);
        TENTATIVE_reassess_all_cards(0, 0x30);
      }
    }

    shuffle_duel_library(player, player);
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x00407d8d
// FUNCTION: SHANDALAR 0x0044daac
int card_visions(int player, int card, event_t event)
{
  char dialog_text[600];
  target_t target;

  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if (event == EVENT_CAST_SPELL && g_affected_card == card && g_affected_card_controller == player)
  {
    if (g_duel_ai_mode_state != 1)
    {
      load_text("prompts.txt", "VISIONS");
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
    if (g_duel_ai_mode_state != 1)
    {
      load_text("prompts.txt", "VISIONS");
    }

    if ((g_active_player == player || (g_duel_network_flags & 2) != 0) &&
        g_duel_ai_mode_state != 1 &&
        g_duel_network_state == 0)
    {
      show_deck(player,
                global_library[PLAYER_CARD_INSTANCE(player, card).targets[0].player],
                5,
                g_text_lines[1],
                0,
                gs_done_008b40e0);
    }

    if (g_duel_ai_mode_state != 1)
    {
      sprintf(dialog_text, " %s\n %s", g_text_lines[2], g_text_lines[3]);
    }

    if (do_dialog(player, player, card, -1, -1, dialog_text, 1) == 0)
    {
      shuffle_duel_library(player, PLAYER_CARD_INSTANCE(player, card).targets[0].player);
    }

    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x0040808e
// FUNCTION: SHANDALAR 0x0044ddad
int card_mind_twist(int player, int card, event_t event)
{
  target_t target;
  int amount;

  if (event == EVENT_CAN_CAST)
  {
    if (player == g_other_player && (g_duel_network_flags & 2) == 0 && has_mana(player, COLOR_ANY, 2) == 0)
    {
      return 0;
    }
    return 1;
  }

  if (event == 0x6c && g_affected_card == card && g_affected_card_controller == player)
  {
    if (g_duel_ai_mode_state != 1)
    {
      load_text("prompts.txt", "MINDTWIST");
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
      PLAYER_CARD_INSTANCE(player, card).info_slot = g_x_value;
      SET_TARGET(PLAYER_CARD_INSTANCE(player, card).targets[0], target);
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
      if (player == g_other_player)
      {
        amount = (g_duel_summary.hand_counts[1 - player] - g_x_value) * 0xc;
        if (amount < 1)
        {
          amount = 0;
        }
        g_ai_modifier -= amount;
        if (g_x_value <= g_duel_summary.hand_counts[1 - player])
        {
          amount = g_x_value;
        }
        else
        {
          amount = g_duel_summary.hand_counts[1 - player];
        }
        g_ai_modifier += amount * 0x18;
      }
    }
    else
    {
      g_spell_fizzled = 1;
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    for (amount = 0; amount < PLAYER_CARD_INSTANCE(player, card).info_slot; ++amount)
    {
      discard(PLAYER_CARD_INSTANCE(player, card).targets[0].player, 1, 0);
    }
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x0040838f
// FUNCTION: SHANDALAR 0x0044e0b0
int card_mind_bomb(int player, int card, event_t event)
{
  char dialog_text[1500];
  char option0[300];
  char option1[300];
  char option2[300];
  char option3[300];
  char prompt[300];
  int choice;
  int current_player;
  unsigned int ai_choice;

  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    if (g_duel_ai_mode_state != 1)
    {
      load_text("prompts.txt", "MINDBOMB");
      strcpy(prompt, g_text_lines[0]);
      strcpy(option1, g_text_lines[1]);
      strcpy(option2, g_text_lines[2]);
      strcpy(option0, g_text_lines[3]);
      strcpy(option3, g_text_lines[4]);
    }

    current_player = player;
    while (current_player >= 0 && current_player < 2)
    {
      if (g_duel_summary.hand_counts[current_player] == 0)
      {
        if (g_duel_ai_mode_state != 1)
        {
          sprintf(dialog_text, "\n%s\n %s\n _%s\n _%s\n _%s",
                  prompt, option1, option2, option0, option3);
        }
        ai_choice = 0;
      }
      else if (g_duel_summary.hand_counts[current_player] == 1)
      {
        if (g_duel_ai_mode_state != 1)
        {
          sprintf(dialog_text, "\n%s\n %s\n %s\n _%s\n _%s",
                  prompt, option1, option2, option0, option3);
        }
        ai_choice = (unsigned int)(g_life[current_player] < 5);
      }
      else if (g_duel_summary.hand_counts[current_player] == 2)
      {
        if (g_duel_ai_mode_state != 1)
        {
          sprintf(dialog_text, "\n%s\n %s\n %s\n %s\n _%s",
                  prompt, option1, option2, option0, option3);
        }
        if (g_life[current_player] < 5)
        {
          ai_choice = 2;
        }
        else
        {
          ai_choice = 0;
        }
      }
      else
      {
        if (g_duel_ai_mode_state != 1)
        {
          sprintf(dialog_text, "\n%s\n %s\n %s\n %s\n %s",
                  prompt, option1, option2, option0, option3);
        }
        if (g_life[current_player] < 5)
        {
          ai_choice = 3;
        }
        else
        {
          ai_choice = 0;
        }
      }

      choice = do_dialog(current_player, player, card, -1, -1, dialog_text, ai_choice);
      if (choice == 1)
      {
        damage_player(current_player, 2, player, card);
        discard(current_player, 0, 1);
      }
      else if (choice == 2)
      {
        damage_player(current_player, 1, player, card);
        discard(current_player, 0, 1);
        discard(current_player, 0, 1);
      }
      else if (choice == 3)
      {
        discard(current_player, 0, 1);
        discard(current_player, 0, 1);
        discard(current_player, 0, 1);
      }
      else
      {
        damage_player(current_player, 3, player, card);
      }

      if (player == 0)
      {
        ++current_player;
      }
      else
      {
        --current_player;
      }
    }

    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004087cc
// FUNCTION: SHANDALAR 0x0044e4ed
int find_highest_value_graveyard_card_by_type(int player, unsigned int type)
{
  int score;
  int best_score;
  int graveyard_index;
  int best_index;
  int internal_card_id;

  best_score = 0;
  best_index = -1;
  if (player == -1)
  {
    return -1;
  }

  for (graveyard_index = 0; graveyard_index < 500 && global_graveyard_slots[player][graveyard_index] != -1;
       ++graveyard_index)
  {
    internal_card_id = global_graveyard_slots[player][graveyard_index];
    if (type == -1 || (type & global_cards_data[internal_card_id].type) != 0)
    {
      score = (char)global_cards_data[internal_card_id].cc[0] * 3 + abs((int)(char)global_cards_data[internal_card_id].cc[1]) * 2;
      if (best_score < score)
      {
        best_index = graveyard_index;
        best_score = score;
      }
    }
  }

  return best_index;
}

// FUNCTION: SHANDALAR 0x0044e5f0
int find_highest_value_library_card_by_type(int player, int library_player, unsigned int type)
{
  int best_index;
  int best_score;
  int card_score;
  int internal_card_id;
  int library_index;

  best_score = -99;
  best_index = -1;
  if (library_player == -1)
  {
    return -1;
  }

  for (library_index = 0;
       library_index < 500 && global_library[library_player][library_index] != -1;
       ++library_index)
  {
    internal_card_id = global_library[library_player][library_index];
    if (type == (unsigned int)-1 || (type & global_cards_data[internal_card_id].type) != 0)
    {
      card_score = abs((int)global_cards_data[internal_card_id].cc[1]);
      card_score += global_cards_data[internal_card_id].cc[0] * 2;
      if (g_basiclandtypes_controlled[player][COLOR_ANY] < card_score)
      {
        card_score = -card_score;
      }
      if (card_score == 0)
      {
        card_score = 99;
      }
      card_score += GetCardRarity(internal_card_id) * 2;
      if (best_score < card_score)
      {
        best_index = library_index;
        best_score = card_score;
      }
    }
  }

  return best_index;
}

// FUNCTION: MAGIC 0x00408b2d
// FUNCTION: SHANDALAR 0x0044e84e
int card_wrath_of_god(int player, int card, event_t event)
{
  int current_card;
  int current_player;

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
        if (is_in_play(current_player, current_card) && (global_cards_data[PLAYER_CARD_INSTANCE(current_player, current_card).internal_card_id].type & TYPE_CREATURE) != 0)
        {
          kill_card(current_player, current_card, KILL_BURY);
        }
      }
    }
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x00408c24
// FUNCTION: SHANDALAR 0x0044e945
int card_flashfires(int player, int card, event_t event)
{
  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    PLAYER_CARD_INSTANCE(player, card).info_slot = get_hacked_color(player, card, 5);
    dispatch_function_to_all_cards_in_play(player, card, helper_destroy_basiclandtype, -1);
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x00408cbd
// FUNCTION: SHANDALAR 0x0044e9de
int card_pyrotechnics(int player, int card, event_t event)
{
  int invalid_targets;
  int target_index;
  target_t selected_target;

  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if (event == EVENT_CAST_SPELL && g_affected_card == card && g_affected_card_controller == player)
  {
    if ((player == g_other_player && (g_duel_network_flags & 2) == 0) ||
        g_duel_ai_mode_state == 1)
    {
      for (target_index = 0; target_index < 4; ++target_index)
      {
        select_damage_target(player, card, 1);
        SET_TARGET(PLAYER_CARD_INSTANCE(player, card).targets[3 - target_index], PLAYER_CARD_INSTANCE(player, card).targets[0]);
        PLAYER_CARD_INSTANCE(player, card).number_of_targets = 4;
      }
    }
    else
    {
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
      if (g_duel_ai_mode_state != 1)
      {
        load_text("prompts.txt", "PYROTECHNICS");
      }

      target_index = 0;
      while (target_index < 4 && g_spell_fizzled != 1)
      {
        if (C_real_select_target(player,
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
                                 -1,
                                 -1,
                                 -1,
                                 0,
                                 0,
                                 0,
                                 g_text_lines[target_index],
                                 1,
                                 &selected_target))
        {
          PLAYER_CARD_INSTANCE(selected_target.player, selected_target.card).state |= STATE_TARGETTED;
          TENTATIVE_reassess_all_cards(0, 0x20);
          SET_TARGET(PLAYER_CARD_INSTANCE(player, card).targets[target_index], selected_target);
          ++PLAYER_CARD_INSTANCE(player, card).number_of_targets;
        }
        else
        {
          g_spell_fizzled = 1;
        }
        ++target_index;
      }

      for (target_index = 0; target_index < PLAYER_CARD_INSTANCE(player, card).number_of_targets; ++target_index)
      {
        PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).targets[target_index].player,
                             PLAYER_CARD_INSTANCE(player, card).targets[target_index].card)
            .state &= ~(STATE_CANNOT_TARGET | STATE_TARGETTED);
      }

      if (g_spell_fizzled == 1)
      {
        PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
      }
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    if ((player == g_other_player && (g_duel_network_flags & 2) == 0) ||
        g_duel_ai_mode_state == 1)
    {
      for (target_index = 0; target_index < 4; ++target_index)
      {
        SET_TARGET(PLAYER_CARD_INSTANCE(player, card).targets[0], PLAYER_CARD_INSTANCE(player, card).targets[target_index]);
        deal_damage_to_selected_target(player, card, EVENT_RESOLVE_SPELL, 1);
      }
    }
    else
    {
      invalid_targets = 0;
      for (target_index = 0; target_index < PLAYER_CARD_INSTANCE(player, card).number_of_targets; ++target_index)
      {
        if (C_real_validate_target(PLAYER_CARD_INSTANCE(player, card).targets[target_index].player,
                                   PLAYER_CARD_INSTANCE(player, card).targets[target_index].card,
                                   (char *)0,
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
                                   -1,
                                   -1,
                                   -1,
                                   0,
                                   0,
                                   0))
        {
          damage_creature(PLAYER_CARD_INSTANCE(player, card).targets[target_index].player,
                          PLAYER_CARD_INSTANCE(player, card).targets[target_index].card,
                          1,
                          player,
                          card);
        }
        else
        {
          ++invalid_targets;
        }
      }

      if (PLAYER_CARD_INSTANCE(player, card).number_of_targets == invalid_targets)
      {
        g_spell_fizzled = 1;
      }
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    }

    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x0040948b
// FUNCTION: SHANDALAR 0x0044f1ac
int card_disintegrate(int player, int card, event_t event)
{
  int target_player;
  int target_card;
  int damage_dealt;
  int legacy_card;

  if (event == EVENT_CAN_CAST)
  {
    if (((player == g_other_player) && ((g_duel_network_flags & 2) == 0)) && has_mana(player, COLOR_ANY, 2) == 0)
    {
      return 0;
    }
    return 1;
  }

  if (((event == EVENT_CAST_SPELL) && (g_affected_card == card)) && (g_affected_card_controller == player))
  {
    PLAYER_CARD_INSTANCE(player, card).info_slot = g_x_value;
    if (g_duel_ai_mode_state != 1)
    {
      load_text("prompts.txt", "DISINTEGRATE");
    }
    if (select_damage_target(player, card, PLAYER_CARD_INSTANCE(player, card).info_slot) != 0)
    {
      g_ai_modifier -= 0x30 / count_active_card_instances_plus_one(player, PLAYER_CARD_INSTANCE(player, card).internal_card_id);
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    target_player = PLAYER_CARD_INSTANCE(player, card).targets[0].player;
    target_card = PLAYER_CARD_INSTANCE(player, card).targets[0].card;
    damage_dealt = deal_damage_to_selected_target(player, card, EVENT_RESOLVE_SPELL, PLAYER_CARD_INSTANCE(player, card).info_slot);
    if (damage_dealt != 0 && target_card != -1)
    {
      legacy_card = create_legacy_effect(player, card, g_duel_generated_internal_card_id_17, target_player, target_card);
      if (legacy_card != -1)
      {
        PLAYER_CARD_INSTANCE(player, legacy_card).info_slot = 0x200;
      }
      PLAYER_CARD_INSTANCE(target_player, target_card).regen_status = 0x8000000;
    }
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004097ba
// FUNCTION: SHANDALAR 0x0044f4db
int card_drain_life(int player, int card, event_t event)
{
  int damage_legacy_card;
  int damage_dealt;

  if (event == EVENT_CAN_CAST)
  {
    if (((player == g_other_player) && ((g_duel_network_flags & 2) == 0)) && has_mana(player, COLOR_ANY, 3) == 0)
    {
      return 0;
    }
    return 1;
  }

  if (((event == EVENT_CAST_SPELL) && (g_affected_card == card)) && (g_affected_card_controller == player))
  {
    if ((g_land_can_be_played & LCBP_CARD_BEING_COPIED) == 0)
    {
      g_x_value = 0;
      charge_mana(player, COLOR_BLACK, -1);
      if (g_spell_fizzled == 1)
      {
        return 0;
      }
      PLAYER_CARD_INSTANCE(player, card).info_slot = g_x_value;
    }
    else
    {
      PLAYER_CARD_INSTANCE(player, card).info_slot = PLAYER_CARD_INSTANCE(g_current_spell_player, g_current_spell_card).info_slot;
    }
    if (g_duel_ai_mode_state != 1)
    {
      load_text("prompts.txt", "DRAIN_LIFE");
    }
    select_damage_target(player, card, PLAYER_CARD_INSTANCE(player, card).info_slot);
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    damage_dealt = deal_damage_to_selected_target(player, card, EVENT_RESOLVE_SPELL, PLAYER_CARD_INSTANCE(player, card).info_slot);
    if (damage_dealt == 0 || PLAYER_CARD_INSTANCE(player, card).info_slot < 1)
    {
      PLAYER_CARD_INSTANCE(player, card).info_slot = 0;
    }
    else
    {
      damage_legacy_card = add_card_to_hand(player, g_duel_generated_internal_card_id_19);
      if (damage_legacy_card != -1)
      {
        PLAYER_CARD_INSTANCE(player, damage_legacy_card).original_internal_card_id = PLAYER_CARD_INSTANCE(player, card).internal_card_id;
        PLAYER_CARD_INSTANCE(player, damage_legacy_card).state |= STATE_IN_PLAY;
        PLAYER_CARD_INSTANCE(player, damage_legacy_card).display_pic_info = CARD_ID_DRAIN_LIFE;
        PLAYER_CARD_INSTANCE(player, damage_legacy_card).unknown0x14 = TRIGGER_DEAL_DAMAGE;
        TENTATIVE_set_timestamps(player, damage_legacy_card);
        PLAYER_CARD_INSTANCE(player, damage_legacy_card).damage_source_player = (char)player;
        PLAYER_CARD_INSTANCE(player, damage_legacy_card).damage_source_card = card;
      }
    }
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    kill_card(player, card, KILL_BURY);
  }

  if (((event == EVENT_DEAL_DAMAGE) && PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).internal_card_id == g_damage_card_internal_card_id) && PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).damage_target_card == -1 && PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).damage_source_player == player && PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).damage_source_card == card && PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).info_slot != 0)
  {
    PLAYER_CARD_INSTANCE(player, card).damage_source_player = (char)g_affected_card_controller;
    PLAYER_CARD_INSTANCE(player, card).damage_source_card = g_affected_card;
  }

  return 0;
}

// FUNCTION: MAGIC 0x00409df5
// FUNCTION: SHANDALAR 0x0044fb16
int card_stone_rain(int player, int card, event_t event)
{
  int target_player;
  int target_card;

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

  if (event == EVENT_CAST_SPELL)
  {
    if (card == g_affected_card)
    {
      if (player == g_affected_card_controller)
      {
        if (g_duel_ai_mode_state != 1)
        {
          load_text("prompts.txt", "STONE_RAIN");
        }
        if (select_target_land_and_store(player, 2, card) != 0)
        {
          g_ai_modifier +=
              ((-(unsigned int)(PLAYER_CARD_INSTANCE(player, card).targets[0].player == player) & 0xfffffffbU) * 3 + 9) * 4;
        }
        else
        {
          g_spell_fizzled = 1;
        }
        if (g_spell_fizzled == 1)
        {
          PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
        }
      }
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    target_player = PLAYER_CARD_INSTANCE(player, card).targets[0].player;
    target_card = PLAYER_CARD_INSTANCE(player, card).targets[0].card;
    if (C_real_validate_target(target_player,
                               target_card,
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
      kill_card(target_player, target_card, KILL_DESTROY);
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

// FUNCTION: MAGIC 0x0040a06a
// FUNCTION: SHANDALAR 0x0044fd8c
int card_drain_power(int player, int card, event_t event)
{
  target_t target;
  int target_player;
  int color_index;

  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if (event == 0x6c && g_affected_card == card && g_affected_card_controller == player)
  {
    if (g_duel_ai_mode_state != 1)
    {
      load_text("prompts.txt", "DRAIN_POWER");
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
    target_player = PLAYER_CARD_INSTANCE(player, card).targets[0].player;
    dispatch_three_arg_callback_to_cards_in_play(drain_power_draw_mana_from_land, target_player);
    if (target_player != player)
    {
      for (color_index = 0; color_index < 8; ++color_index)
      {
        g_raw_mana_available[player][color_index] += g_raw_mana_available[target_player][color_index];
        g_raw_mana_available[target_player][color_index] = 0;
      }
    }
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x0040a2ae
int drain_power_draw_mana_from_land(int player, int card, int internal_card_id)
{
  g_required_mana_color_mask = 0x3e;
  g_produced_mana_color = -1;

  if ((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0 && (global_cards_data[internal_card_id].type & TYPE_LAND) != 0 && (global_cards_data[internal_card_id].extra_ability & 0x1000) != 0)
  {
    dispatch_event_to_single_card(player, card, EVENT_ACTIVATE, 1 - player, -1);
    if (g_spell_fizzled == 1)
    {
      g_spell_fizzled = -1;
      g_required_mana_color_mask = 1;
      dispatch_event_to_single_card(player, card, EVENT_ACTIVATE, 1 - player, -1);
      g_required_mana_color_mask = 0x3e;
    }
    if ((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) != 0)
    {
      dispatch_event(player, card, EVENT_TAP_CARD);
    }
  }

  g_required_mana_color_mask = 0;
  return 0;
}
