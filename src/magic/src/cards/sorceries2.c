#include <string.h>
#include "../game_support.h"
#include "../global_strings.h"

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

  if (event == EVENT_CAST_SPELL && affected_card == card && affected_card_controller == player)
  {
    ai_modifier += hand_count[player] * -0x18 + 0x48;
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    cards_in_hand = hand_count[player];
    for (current_card = 0; current_card < cards_in_hand; ++current_card)
    {
      discard(player, 0, 0);
    }
    FUN_004817fd(player);
    FUN_0040246a(player, 7);
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

  if (event == EVENT_CAST_SPELL && affected_card == card && affected_card_controller == player)
  {
    load_text("promptsX1.txt", "DARKPACT");
    if (!C_real_select_target(player, 2, 2, TARGET_ZONE_PLAYERS, TYPE_NONE, TYPE_NONE, 0, 0,
                              COLOR_TEST_0, COLOR_TEST_0, -1, ~SUB_WALL, -1, -1, 0, 0, 0,
                              text_lines[0], 1, &s.target))
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
      global_ante_cards[s.target_player][0] = (char)PLAYER_CARD_INSTANCE(player, card).info_slot;
      PLAYER_CARD_INSTANCE(player, card).info_slot = add_card_to_hand(player, PLAYER_CARD_INSTANCE(player, card).info_slot);
      if (PLAYER_CARD_INSTANCE(player, card).info_slot != -1)
      {
        PLAYER_CARD_INSTANCE(player, PLAYER_CARD_INSTANCE(player, card).info_slot).token_status |= 0x10000;
      }
      load_text("promptsX1.txt", "DARKPACT");
      if (player == human_player)
      {
        do_dialog(player, player, card, player, PLAYER_CARD_INSTANCE(player, card).info_slot, text_lines[1], 0);
      }
      else
      {
        do_dialog(player, player, card, player, PLAYER_CARD_INSTANCE(player, card).info_slot, text_lines[2], 0);
      }
      if (PLAYER_CARD_INSTANCE(player, card).info_slot != -1)
      {
        PLAYER_CARD_INSTANCE(player, PLAYER_CARD_INSTANCE(player, card).info_slot).internal_card_id = -1;
      }
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
    FUN_004817fd(player);
    FUN_004817fd(1 - player);
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x0040af3f
// FUNCTION: SHANDALAR 0x00401670
int card_drafna_s_restoration(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x0040b8bc
// FUNCTION: SHANDALAR 0x00401fee
int card_reconstruction(int player, int card, event_t event)
{
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
    return FUN_004832f4(player, 2);
  }

  if (((event == EVENT_CAST_SPELL) && (card == affected_card)) && (player == affected_card_controller))
  {
    if (((player == active_player) && ((unk_00926804 & 2) == 0)) || (unk_008a9000 == 1))
    {
      s.can_select = 1;
      s.graveyard_index = FUN_004087cc(player, 2);
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
      s.prompt = text_lines;
      s.can_select = FUN_004a62d7(player,
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
      spell_fizzled = 1;
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    if (PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                             PLAYER_CARD_INSTANCE(player, card).targets[0].card)
                .internal_card_id == global_graveyard_slots[player][PLAYER_CARD_INSTANCE(player, card).info_slot] &&
        (global_cards_data[global_graveyard_slots[player][PLAYER_CARD_INSTANCE(player, card).info_slot]].type & TYPE_CREATURE) != 0)
    {
      FUN_0051a41c(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                   PLAYER_CARD_INSTANCE(player, card).targets[0].card);
      FUN_004b15f7(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                   PLAYER_CARD_INSTANCE(player, card).info_slot);
    }
    else
    {
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                           PLAYER_CARD_INSTANCE(player, card).targets[0].card)
          .token_status |= 4;
      kill_card(player, card, KILL_DESTROY);
      spell_fizzled = 1;
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
}

// FUNCTION: MAGIC 0x0040c490
// FUNCTION: SHANDALAR 0x00402bc7
int card_transmute_artifact(int player, int card, event_t event)
{
}
