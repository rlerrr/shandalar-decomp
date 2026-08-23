#include <string.h>
#include "../game_support.h"
#include "../global_strings.h"

int find_highest_value_library_card_by_type(int player, int library_player, unsigned int type);

// FUNCTION: MAGIC 0x0040a8d0
// FUNCTION: SHANDALAR 0x00401000
int card_contract_from_below(int player, int card, event_t event)
{
  int cards_in_hand;
  int current_card;

  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if (event == EVENT_CAST_SPELL && g_affected_card == card && g_affected_card_controller == player)
  {
    g_ai_modifier += g_duel_summary.hand_counts[player] * -0x18 + 0x48;
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    cards_in_hand = 0;
    for (current_card = 0; current_card < g_active_cards_count[player]; ++current_card)
    {
      if (is_card_pending_resolution(player, current_card))
      {
        ++cards_in_hand;
      }
    }
    for (current_card = 0; current_card < cards_in_hand; ++current_card)
    {
      discard(player, 0, 0);
    }
    ante_drawn_card(player);
    draw_cards_and_set_hand_count(player, 7);
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x0040a9ed
// FUNCTION: SHANDALAR 0x0040111d
int card_darkpact(int player, int card, event_t event)
{
  struct
  {
    target_t target;
    int target_player;
    int index;
  } s;

  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if (event == EVENT_CAST_SPELL && g_affected_card == card && g_affected_card_controller == player)
  {
    load_text("promptsX1.txt", "DARKPACT");
    if (!C_real_select_target(player, 2, 2, TARGET_ZONE_PLAYERS, TYPE_NONE, TYPE_NONE, 0, 0,
                              COLOR_TEST_0, COLOR_TEST_0, -1, ~SUB_WALL, -1, -1, 0, 0, 0,
                              g_text_lines[0], 1, &s.target))
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

  if (event == EVENT_RESOLVE_SPELL)
  {
    s.target_player = PLAYER_CARD_INSTANCE(player, card).targets[0].player;
    if (global_library[player][0] != -1)
    {
      PLAYER_CARD_INSTANCE(player, card).info_slot = global_library[player][0];
      remove_card_from_deck(player, 0);
      for (s.index = 0; s.index < 16 && global_ante_cards[s.target_player][s.index] != -1; ++s.index)
      {
        real_put_on_top_of_deck(player, global_ante_cards[s.target_player][s.index]);
        global_ante_cards[s.target_player][s.index] = -1;
      }
      global_ante_cards[s.target_player][0] = PLAYER_CARD_INSTANCE(player, card).info_slot;
      PLAYER_CARD_INSTANCE(player, card).info_slot = add_card_to_hand(player, PLAYER_CARD_INSTANCE(player, card).info_slot);
      if (PLAYER_CARD_INSTANCE(player, card).info_slot != -1)
      {
        PLAYER_CARD_INSTANCE(player, PLAYER_CARD_INSTANCE(player, card).info_slot).token_status |= 0x10000;
      }
      load_text("promptsX1.txt", "DARKPACT");
      if (player == g_active_player)
      {
        do_dialog(player, player, card, player, PLAYER_CARD_INSTANCE(player, card).info_slot, g_text_lines[1], 0);
      }
      else
      {
        do_dialog(player, player, card, player, PLAYER_CARD_INSTANCE(player, card).info_slot, g_text_lines[2], 0);
      }
      PLAYER_CARD_INSTANCE(player, PLAYER_CARD_INSTANCE(player, card).info_slot).internal_card_id = -1;
    }
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x0040aee0
// FUNCTION: SHANDALAR 0x00401611
int card_demonic_attorney(int player, int card, event_t event)
{
  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    ante_drawn_card(player);
    ante_drawn_card(1 - player);
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x0040af3f
// FUNCTION: SHANDALAR 0x00401670
int card_drafna_s_restoration(int player, int card, event_t event)
{
  struct
  {
    target_t target;
    int target_player;
    int selectable[500];
    int selected[500];
    int selected_count;
    int graveyard_index;
    int compact_index;
    char (*prompt)[300];
  } s;

  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if (event == EVENT_CAST_SPELL && g_affected_card == card && g_affected_card_controller == player)
  {
    load_text("promptsX1.txt", "DRAFNAS_RESTORATION");
    if (!C_real_select_target(player, 2, 2, TARGET_ZONE_PLAYERS, TYPE_NONE, TYPE_NONE, 0, 0,
                              COLOR_TEST_0, COLOR_TEST_0, -1, ~SUB_WALL, -1, -1, 0, 0, 0,
                              g_text_lines[0], 1, &s.target))
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

  if (event == EVENT_RESOLVE_SPELL)
  {
    s.target_player = PLAYER_CARD_INSTANCE(player, card).targets[0].player;
    s.selected_count = 0;

    if (((player == g_other_player) && ((g_duel_network_flags & 2) == 0)) || g_duel_ai_mode_state == 1)
    {
      s.selected[0] = find_highest_value_graveyard_card_by_type(player, TYPE_ARTIFACT);
      if (s.selected[0] != -1)
      {
        s.selected_count = 1;
      }
    }
    else
    {
      s.selected_count = 0;
      for (s.graveyard_index = 0;
           s.graveyard_index < 500 && global_graveyard_slots[s.target_player][s.graveyard_index] != -1;
           ++s.graveyard_index)
      {
        if ((global_cards_data[global_graveyard_slots[s.target_player][s.graveyard_index]].type & TYPE_ARTIFACT) != 0)
        {
          s.selectable[s.graveyard_index] = 1;
          ++s.selected_count;
        }
        else
        {
          s.selectable[s.graveyard_index] = 0;
        }
      }

      if (s.selected_count != 0)
      {
        load_text("promptsX1.txt", "DRAFNAS_RESTORATION");
        s.prompt = g_text_lines;
        s.selected_count = select_from_graveyard_with_dialog(player,
                                                             global_graveyard_slots[s.target_player],
                                                             s.selectable,
                                                             500,
                                                             (int)&s.prompt,
                                                             1,
                                                             (int)s.selected,
                                                             0,
                                                             s.selected_count);
      }
    }

    for (s.graveyard_index = 0; s.graveyard_index < s.selected_count; ++s.graveyard_index)
    {
      if (s.selected[s.graveyard_index] != -1 &&
          global_graveyard_slots[s.target_player][s.selected[s.graveyard_index]] != -1 &&
          (global_cards_data[global_graveyard_slots[s.target_player][s.selected[s.graveyard_index]]].type & TYPE_ARTIFACT) != 0)
      {
        real_put_on_top_of_deck(s.target_player, global_graveyard_slots[s.target_player][s.selected[s.graveyard_index]]);
        global_graveyard_slots[s.target_player][s.selected[s.graveyard_index]] = -2;
      }
    }

    s.graveyard_index = 0;
    while (s.graveyard_index < 500 && global_graveyard_slots[s.target_player][s.graveyard_index] != -1)
    {
      if (global_graveyard_slots[s.target_player][s.graveyard_index] == -2)
      {
        for (s.compact_index = s.graveyard_index; s.compact_index < 499; ++s.compact_index)
        {
          global_graveyard_slots[s.target_player][s.compact_index] = global_graveyard_slots[s.target_player][s.compact_index + 1];
          if (global_graveyard_slots[s.target_player][s.compact_index] == -1)
          {
            break;
          }
        }
        global_graveyard_slots[s.target_player][499] = -1;
      }
      else
      {
        ++s.graveyard_index;
      }
    }

    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x0040b8bc
// FUNCTION: SHANDALAR 0x00401fee
int card_reconstruction(int player, int card, event_t event)
{
  struct
  {
    int can_select;
    int selectable[500];
    int hand_card;
    int graveyard_index;
    int current_card;
    char (*prompt)[300];
  } s;

  if (event == EVENT_CAN_CAST)
  {
    return graveyard_has_type(player, TYPE_ARTIFACT);
  }

  if (event == EVENT_CAST_SPELL && g_affected_card == card && g_affected_card_controller == player)
  {
    if (((player == g_other_player) && ((g_duel_network_flags & 2) == 0)) || g_duel_ai_mode_state == 1)
    {
      s.can_select = 1;
      s.graveyard_index = find_highest_value_graveyard_card_by_type(player, TYPE_ARTIFACT);
    }
    else
    {
      for (s.current_card = 0;
           s.current_card < 500 && global_graveyard_slots[player][s.current_card] != -1;
           ++s.current_card)
      {
        s.selectable[s.current_card] =
            (global_cards_data[global_graveyard_slots[player][s.current_card]].type & TYPE_ARTIFACT) != 0;
      }
      load_text("promptsX1.txt", "RECONSTRUCTION");
      s.prompt = g_text_lines;
      s.can_select = select_from_graveyard_with_dialog(player,
                                                       global_graveyard_slots[player],
                                                       s.selectable,
                                                       500,
                                                       (int)&s.prompt,
                                                       1,
                                                       (int)&s.graveyard_index,
                                                       0,
                                                       1);
    }

    if (s.can_select != 0 &&
        s.graveyard_index != -1 &&
        global_graveyard_slots[player][s.graveyard_index] != -1 &&
        (global_cards_data[global_graveyard_slots[player][s.graveyard_index]].type & TYPE_ARTIFACT) != 0)
    {
      s.hand_card = add_card_to_hand(player, global_graveyard_slots[player][s.graveyard_index]);
      if (s.hand_card != -1)
      {
        PLAYER_CARD_INSTANCE(player, s.hand_card).state |= STATE_INVISIBLE;
        PLAYER_CARD_INSTANCE(player, card).targets[0].player = player;
        PLAYER_CARD_INSTANCE(player, card).targets[0].card = s.hand_card;
        PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
        PLAYER_CARD_INSTANCE(player, card).info_slot = s.graveyard_index;
      }
    }
    else
    {
      g_spell_fizzled = 1;
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    s.graveyard_index = PLAYER_CARD_INSTANCE(player, card).info_slot;
    if (s.graveyard_index == -1 ||
        global_graveyard_slots[player][s.graveyard_index] == -1 ||
        (global_cards_data[global_graveyard_slots[player][s.graveyard_index]].type & TYPE_ARTIFACT) == 0)
    {
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                           PLAYER_CARD_INSTANCE(player, card).targets[0].card)
          .targets[6]
          .card = -1;
    }
    else
    {
      remove_card_from_graveyard(player, s.graveyard_index);
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                           PLAYER_CARD_INSTANCE(player, card).targets[0].card)
          .state &= ~STATE_INVISIBLE;
      ++g_duel_summary.hand_counts[player];
    }
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x0040bdc6
// FUNCTION: SHANDALAR 0x004024fc
int card_resurrection(int player, int card, event_t event)
{
  struct
  {
    int can_select;
    int selectable[500];
    int hand_card;
    int graveyard_index;
    char (*prompt)[300];
  } s;

  if (event == EVENT_CAN_CAST)
  {
    return graveyard_has_type(player, 2);
  }

  if (((event == EVENT_CAST_SPELL) && (card == g_affected_card)) && (player == g_affected_card_controller))
  {
    if (((player == g_other_player) && ((g_duel_network_flags & 2) == 0)) || (g_duel_ai_mode_state == 1))
    {
      s.can_select = 1;
      s.graveyard_index = find_highest_value_graveyard_card_by_type(player, 2);
    }
    else
    {
      for (s.hand_card = 0;
           (s.hand_card < 500 && global_graveyard_slots[player][s.hand_card] != -1);
           ++s.hand_card)
      {
        if ((global_cards_data[global_graveyard_slots[player][s.hand_card]].type & TYPE_CREATURE) != 0)
        {
          s.selectable[s.hand_card] = 1;
        }
        else
        {
          s.selectable[s.hand_card] = 0;
        }
      }
      load_text("promptsX1.txt", "RESURRECTION");
      s.prompt = g_text_lines;
      s.can_select = select_from_graveyard_with_dialog(player,
                                       global_graveyard_slots[player],
                                       s.selectable,
                                       500,
                                       (int)&s.prompt,
                                       1,
                                       (int)&s.graveyard_index,
                                       0,
                                       1);
    }

    if (s.can_select != 0 && global_graveyard_slots[player][s.graveyard_index] != -1)
    {
      s.hand_card = add_card_to_hand(player, global_graveyard_slots[player][s.graveyard_index]);
      if (s.hand_card != -1)
      {
        PLAYER_CARD_INSTANCE(player, s.hand_card).state |= 0x20;
        PLAYER_CARD_INSTANCE(player, card).info_slot = s.graveyard_index;
        PLAYER_CARD_INSTANCE(player, card).targets[0].player = player;
        PLAYER_CARD_INSTANCE(player, card).targets[0].card = s.hand_card;
        PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
      }
    }
    else
    {
      g_spell_fizzled = 1;
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    if (PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                             PLAYER_CARD_INSTANCE(player, card).targets[0].card)
                .internal_card_id == global_graveyard_slots[player][PLAYER_CARD_INSTANCE(player, card).info_slot] &&
        (global_cards_data[global_graveyard_slots[player][PLAYER_CARD_INSTANCE(player, card).info_slot]].type & TYPE_CREATURE) != 0)
    {
      process_card_enters_play(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                   PLAYER_CARD_INSTANCE(player, card).targets[0].card);
      remove_card_from_graveyard(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                   PLAYER_CARD_INSTANCE(player, card).info_slot);
    }
    else
    {
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                           PLAYER_CARD_INSTANCE(player, card).targets[0].card)
          .token_status |= 4;
      kill_card(player, card, KILL_DESTROY);
      g_spell_fizzled = 1;
    }
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x0040c399
// FUNCTION: SHANDALAR 0x00402ad0
int card_shatterstorm(int player, int card, event_t event)
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
        if (is_in_play(current_player, current_card) != 0 &&
            (global_cards_data[PLAYER_CARD_INSTANCE(current_player, current_card).internal_card_id].type & TYPE_ARTIFACT) != 0)
        {
          kill_card(current_player, current_card, KILL_BURY);
        }
      }
    }

    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x0040c490
// FUNCTION: SHANDALAR 0x00402bc7
int card_transmute_artifact(int player, int card, event_t event)
{
  struct
  {
    target_t target;
    int selectable[500];
    int library_index;
    int current_card;
    int selectable_count;
    int hand_card;
    int selected_value;
    int extra_cost;
    int internal_card_id;
    int paid;
    int selection_result;
    char (*prompt)[300];
  } s;

  if (event == EVENT_CAN_CAST)
  {
    return real_target_available((int *)0, TARGET_SCAN_DIRECT, player, player, player, TARGET_ZONE_IN_PLAY,
                                 TYPE_ARTIFACT, TYPE_NONE, 0, 0, COLOR_TEST_0, COLOR_TEST_0,
                                 -1, -1, -1, -1, 0, 0, 0);
  }

  if (event == EVENT_CAST_SPELL && g_affected_card == card && g_affected_card_controller == player)
  {
    load_text("promptsX1.txt", "TRANSMUTE_ARTIFACT");
    if (!C_real_select_target(player, player, player, TARGET_ZONE_IN_PLAY,
                              TYPE_ARTIFACT, TYPE_NONE, 0, 0, COLOR_TEST_0, COLOR_TEST_0,
                              -1, ~SUB_WALL, -1, -1, 0, 0, 0, g_text_lines[0], 1, &s.target))
    {
      g_spell_fizzled = 1;
    }
    else
    {
      PLAYER_CARD_INSTANCE(player, card).targets[0].player = s.target.player;
      PLAYER_CARD_INSTANCE(player, card).targets[0].card = s.target.card;
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
      s.internal_card_id = PLAYER_CARD_INSTANCE(s.target.player, s.target.card).internal_card_id;
      if ((int)(char)global_cards_data[s.internal_card_id].cc[0] +
              (((int)(char)global_cards_data[s.internal_card_id].cc[1] == -1) ? 1 : 0) ==
          0)
      {
        PLAYER_CARD_INSTANCE(player, card).info_slot = (int)(char)global_cards_data[s.internal_card_id].cc[1];
      }
      else
      {
        PLAYER_CARD_INSTANCE(player, card).info_slot = 1;
      }
      if (g_duel_ai_mode_state != 1)
      {
        play_sound_effect(0xf);
      }
      kill_card(s.target.player, s.target.card, KILL_SACRIFICE);
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    s.library_index = -1;
    s.selection_result = 0;
    if (player == g_active_player || (g_duel_network_flags & 2) != 0)
    {
      s.selectable_count = 0;
      for (s.current_card = 0; s.current_card < 500 && global_library[player][s.current_card] != -1; ++s.current_card)
      {
        if ((global_cards_data[global_library[player][s.current_card]].type & TYPE_ARTIFACT) != 0)
        {
          s.selectable[s.current_card] = 1;
          ++s.selectable_count;
        }
        else
        {
          s.selectable[s.current_card] = 0;
        }
      }

      if (s.selectable_count != 0)
      {
        if (g_duel_ai_mode_state != 1 && g_duel_network_state == 0)
        {
          load_text("promptsX1.txt", "TRANSMUTE_ARTIFACT");
          s.prompt = g_text_lines;
          s.selection_result = select_from_graveyard_with_dialog(player,
                                                                 global_library[player],
                                                                 s.selectable,
                                                                 500,
                                                                 (int)&s.prompt,
                                                                 1,
                                                                 (int)&s.library_index,
                                                                 1,
                                                                 1);
        }
        else
        {
          s.library_index = find_highest_value_library_card_by_type(player, player, TYPE_ARTIFACT);
          if (s.library_index != -1)
          {
            s.selection_result = 1;
          }
        }
      }
    }
    else
    {
      s.library_index = find_highest_value_library_card_by_type(player, player, TYPE_ARTIFACT);
      if (s.library_index != -1)
      {
        s.selection_result = 1;
      }
    }

    if (s.selection_result < 1 ||
        s.library_index == -1 ||
        global_library[player][s.library_index] == -1 ||
        (global_cards_data[global_library[player][s.library_index]].type & TYPE_ARTIFACT) == 0)
    {
      g_spell_fizzled = 1;
    }
    else
    {
      s.internal_card_id = global_library[player][s.library_index];
      if ((int)(char)global_cards_data[s.internal_card_id].cc[0] +
              (((int)(char)global_cards_data[s.internal_card_id].cc[1] == -1) ? 1 : 0) ==
          0)
      {
        s.selected_value = (int)(char)global_cards_data[s.internal_card_id].cc[1];
      }
      else
      {
        s.selected_value = 1;
      }

      s.extra_cost = s.selected_value - PLAYER_CARD_INSTANCE(player, card).info_slot;
      if (s.extra_cost < 1)
      {
        s.extra_cost = 0;
      }

      s.hand_card = add_card_to_hand(player, s.internal_card_id);
      if (s.hand_card != -1)
      {
        PLAYER_CARD_INSTANCE(player, s.hand_card).state |= STATE_INVISIBLE;
        s.paid = 0;
        if (has_mana(player, COLOR_ANY, s.extra_cost) != 0)
        {
          charge_mana(player, COLOR_COLORLESS, s.extra_cost);
          if (g_spell_fizzled != 1)
          {
            s.paid = 1;
            process_card_enters_play(player, s.hand_card);
          }
        }
        if (s.paid == 0)
        {
          kill_card(player, s.hand_card, KILL_BURY);
        }
      }
      remove_card_from_deck(player, s.library_index);
    }

    shuffle_duel_library(player, player);
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}
