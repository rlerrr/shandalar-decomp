#include <string.h>
#include "../game_support.h"
#include "../global_strings.h"

// FUNCTION: MAGIC 0x004b6120
// FUNCTION: SHANDALAR 0x00429ee0
int card_swamp(int player, int card, event_t event)
{
  return mana_producer_sound_on_resolve(player, card, event, COLOR_BLACK);
}

// FUNCTION: MAGIC 0x004b6146
// FUNCTION: SHANDALAR 0x00429f06
int card_island(int player, int card, event_t event)
{
  return mana_producer_sound_on_resolve(player, card, event, COLOR_BLUE);
}

// FUNCTION: MAGIC 0x004b616c
// FUNCTION: SHANDALAR 0x00429f2c
int card_forest(int player, int card, event_t event)
{
  return mana_producer_sound_on_resolve(player, card, event, COLOR_GREEN);
}

// FUNCTION: MAGIC 0x004b6192
// FUNCTION: SHANDALAR 0x00429f52
int card_mountain(int player, int card, event_t event)
{
  return mana_producer_sound_on_resolve(player, card, event, COLOR_RED);
}

// FUNCTION: MAGIC 0x004b61b8
// FUNCTION: SHANDALAR 0x00429f78
int card_plains(int player, int card, event_t event)
{
  return mana_producer_sound_on_resolve(player, card, event, COLOR_WHITE);
}

// FUNCTION: MAGIC 0x004b61de
// FUNCTION: SHANDALAR 0x00429f9e
int card_gem_bazaar(int player, int card, event_t event)
{
  if (event == EVENT_COUNT_MANA)
  {
    return mana_producer_sound_on_resolve(player,
                                         card,
                                         event,
                                         single_color_test_bit_to_color_t(
                                             (color_test_t)(global_card_instances[player])[card].mana_color));
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    return mana_producer_sound_on_resolve(player,
                                         card,
                                         event,
                                         single_color_test_bit_to_color_t(
                                             (color_test_t)(global_card_instances[player])[card].mana_color));
  }

  if (event == EVENT_ACTIVATE)
  {
    return mana_producer_sound_on_resolve(player,
                                         card,
                                         event,
                                         single_color_test_bit_to_color_t(
                                             (color_test_t)(global_card_instances[player])[card].mana_color));
  }

  if (event == EVENT_RESOLVE_SPELL ||
      ((event == EVENT_TAP_CARD || event == EVENT_PLAY_ABILITY) && affected_card == card &&
       affected_card_controller == player))
  {
    if (g_duel_ai_mode_state != 1)
    {
      play_sound_effect(0x31);
    }

    if ((g_duel_network_flags & 2) != 0)
    {
      (global_card_instances[player])[card].mana_color = (char)(1 << (unsigned char)(network_random(player, 5) + 1));
    }
    else if (player == active_player)
    {
      (global_card_instances[player])[card].mana_color = (char)(1 << (unsigned char)(internal_rand(5) + 1));
    }
    else
    {
      (global_card_instances[player])[card].mana_color = (char)DAT_00775d3c;
    }

    (global_card_instances[player])[card].info_slot = (global_card_instances[player])[card].mana_color;
    return 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x004b65b6
// FUNCTION: SHANDALAR 0x0042a375
int card_oasis(int player, int card, event_t event)
{
  struct
  {
    int dummy0;
    int dummy1;
    int dummy2;
    int dummy3;
  } s;
}
// FUNCTION: MAGIC 0x004b6bd6
// FUNCTION: SHANDALAR 0x0042a995
int card_strip_mine(int player, int card, event_t event)
{
  struct
  {
    char unused_394[0x390];
    int unused_4;
  } s;

  return 0;
}

// FUNCTION: MAGIC 0x004b7099
// FUNCTION: SHANDALAR 0x0042ae58
int card_library_of_alexandria(int player, int card, event_t event)
{
  struct
  {
    char dialog[0x384];
    int action;
    int default_action;
    int unused;
  } s;

  if (event == EVENT_COUNT_MANA || event == EVENT_CAN_ACTIVATE)
  {
    return mana_producer_sound_on_resolve(player, card, event, COLOR_COLORLESS);
  }

  if (event == EVENT_ACTIVATE)
  {
    if (g_duel_ai_mode_state != 1)
    {
      load_text("prompts.txt", "LIBRARY_OF_ALEXANDRIA");
    }

    if (hand_count[player] == 7)
    {
      if (g_duel_ai_mode_state != 1)
      {
        sprintf(s.dialog, " %s\n %s\n %s", text_lines[0], text_lines[1], text_lines[2]);
      }
      s.default_action = 1;
    }
    else
    {
      if (g_duel_ai_mode_state != 1)
      {
        sprintf(s.dialog, " %s\n %s\n %s", text_lines[0], text_lines[1], text_lines[2]);
      }
      s.default_action = 0;
    }

    if (((other_player == player) && ((g_duel_network_flags & 2) == 0)) && (g_duel_ai_mode_state != 1))
    {
      s.action = do_dialog(player, player, card, -1, -1, s.dialog, s.default_action);
    }
    else
    {
      s.action = s.default_action;
    }

    PLAYER_CARD_INSTANCE(player, card).info_slot = 0;
    if (s.action == 0)
    {
      mana_producer_sound_on_resolve(player, card, event, COLOR_COLORLESS);
    }
    else if (s.action == 1)
    {
      PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
      undeclare_mana_available(player, COLOR_COLORLESS, 1);
      PLAYER_CARD_INSTANCE(player, card).info_slot = 1;
      produced_mana_color = -1;
    }
    else
    {
      spell_fizzled = 1;
    }

    return 0;
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    card_instance_t *instance = &PLAYER_CARD_INSTANCE(player, card);
    if (PLAYER_CARD_INSTANCE(instance->parent_controller, instance->parent_card).info_slot == 1)
    {
      PLAYER_CARD_INSTANCE(instance->parent_controller, instance->parent_card).info_slot = 0;
      draw_card_for_player(player);
    }
    return 0;
  }

  if (event == EVENT_UNTAP_PHASE || event == EVENT_RESOLVE_SPELL)
  {
    return mana_producer_sound_on_resolve(player, card, event, COLOR_COLORLESS);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004b74b2
// FUNCTION: SHANDALAR 0x0042b270
int card_mishra_s_factory(int player, int card, event_t event)
{
  struct
  {
    char unused_4e0[0x4dc];
    int unused_4;
  } s;

  return 0;
}

// FUNCTION: MAGIC 0x004b85f5
// FUNCTION: SHANDALAR 0x0042c3ae
int card_assembly_worker(int player, int card, event_t event)
{
  struct
  {
    char unused_4e0[0x4dc];
    int unused_4;
  } s;

  return 0;
}

// FUNCTION: MAGIC 0x004b978b
// FUNCTION: SHANDALAR 0x0042d548
int card_mishra_s_workshop(int player, int card, event_t event)
{
  if (event == EVENT_COUNT_MANA && affected_card == card && affected_card_controller == player)
  {
    if (!is_animated_and_sick(player, card))
    {
      if ((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0)
      {
        declare_mana_available(player, COLOR_ARTIFACT, 3);
      }
    }
    return 0;
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    if ((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0 && !is_animated_and_sick(player, card))
    {
      return 1;
    }
    return 0;
  }

  if (event == EVENT_ACTIVATE)
  {
    undeclare_mana_available_and_produce_it(player, COLOR_ARTIFACT, 3);
    PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
    produced_mana_color = COLOR_ARTIFACT;
    return 0;
  }

  if (event == EVENT_RESOLVE_SPELL && g_duel_ai_mode_state != 1)
  {
    play_sound_effect(WAV_GREY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004b99d0
// FUNCTION: SHANDALAR 0x0042d790
int mana_producer_sound_on_resolve(int player, int card, event_t event, color_t color)
{
  if (event == EVENT_COUNT_MANA && affected_card == card && affected_card_controller == player)
  {
    if (((PLAYER_CARD_INSTANCE(player, card).state & (STATE_SUMMONSICK_NOATTACK | STATE_SUMMONSICK_NOTAP)) == 0 || (global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].type & TYPE_CREATURE) == 0) && (PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0)
    {
      declare_mana_available(player, color, 1);
    }
    return 0;
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    if ((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0 && ((PLAYER_CARD_INSTANCE(player, card).state & (STATE_SUMMONSICK_NOATTACK | STATE_SUMMONSICK_NOTAP)) == 0 || (global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].type & TYPE_CREATURE) == 0))
      return 1;
    else
      return 0;
  }

  if (event == EVENT_ACTIVATE)
  {
    undeclare_mana_available_and_produce_it(player, color, 1);
    PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
    produced_mana_color = color;
    return 0;
  }

  if (event == EVENT_RESOLVE_SPELL && g_duel_ai_mode_state != 1)
  {
    play_sound_effect(color + WAV_GREY);
  }

  return 0;
}
