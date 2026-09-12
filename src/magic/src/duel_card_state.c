#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <windows.h>

#include "cardartlib/src/assert.h"
#include "cardartlib/src/palette.h"
#include "deckdll/src/card_db.h"
#include "deckdll/src/magsnd.h"
#include "deckdll/src/shared_resources.h"
#include "drawcardlib/Drawcardlib.h"
#include "duel_engine.h"
#include "game_support.h"
#include "global_strings.h"
#include "shared_startup.h"

// FUNCTION: MAGIC 0x0043e668
// FUNCTION: SHANDALAR 0x00409ee8
void discard(int player, int flags, int player_who_controls_effect)
{
  struct
  {
    int tries;         // ebp - 0x4cc
    int selected_card; // ebp - 0x4c8
    target_t target;
    char prompt[600];
    char prompt_line_2[300];
    int dialog_result;
    char prompt_line_1[300];
    int current_card;
    unsigned int found;
  } s;

  s.tries = 0;

  if (g_duel_summary.hand_counts[player] <= 0)
    return;

  if ((g_other_player == player && (g_duel_network_flags & 2) == 0 && (g_duel_summary.hand_counts[player] + g_hand_count_adjustment) <= 0))
    return;

  if (((g_active_player == player || (g_duel_network_flags & 2) != 0) && g_duel_ai_mode_state != 1) && flags == 0)
  {
    load_text(global_ui_strings_filename, "PROMPT_DISCARDACARD");
    C_real_select_target(player,
                         player,
                         player,
                         TARGET_ZONE_HAND,
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
                         0,
                         &s.target);
    s.selected_card = s.target.card;
  }
  else
  {
    s.found = 0;
    if ((g_duel_network_flags & 2) != 0 && g_other_player == player)
    {
      TENTATIVE_wait_for_network_result(player, 0x14);
      s.selected_card = g_network_result_packet.result;
      if (s.selected_card == -1)
        s.found = 0;
      else
        s.found = 1;
    }
    else
    {
      s.tries = 0;
      do
      {
        s.selected_card = internal_rand(g_active_cards_count[player]);
        if (PLAYER_CARD_INSTANCE(player, s.selected_card).internal_card_id != -1 && (PLAYER_CARD_INSTANCE(player, s.selected_card).state & 2) == 0 && (PLAYER_CARD_INSTANCE(player, s.selected_card).state & 0x20) == 0)
        {
          s.found = 1;
        }
      } while (s.found == 0 && ++s.tries < 999);

      if (s.found == 0)
      {
        for (s.current_card = 0; s.current_card < g_active_cards_count[player] && s.found == 0; s.current_card++)
        {
          if (PLAYER_CARD_INSTANCE(player, s.current_card).internal_card_id != -1 && (PLAYER_CARD_INSTANCE(player, s.current_card).state & 2) == 0 && (PLAYER_CARD_INSTANCE(player, s.current_card).state & 0x20) == 0)
          {
            s.found = 1;
            s.selected_card = s.current_card;
          }
        }
      }

      if ((g_duel_network_flags & 2) != 0 && g_active_player == player)
      {
        if (s.found == 0)
        {
          s.selected_card = -1;
        }
        g_network_result_packet.result = s.selected_card;
        g_network_result_packet.packet_type = 0x14;
        TENTATIVE_send_network_result(player, 0x14);
      }
    }
  }

  if ((g_player_special_effect_flags[player] & 2) == 0 || player_who_controls_effect != 0)
  {
    if (g_other_player == player && (g_duel_network_flags & 2) == 0 && g_duel_ai_mode_state != 1)
    {
      if (flags)
      {
        load_text(global_ui_strings_filename, "PROMPT_DISCARDACARD");
        do_dialog(player, player, s.selected_card, -1, -1, g_text_lines[1], 0);
      }
      else
      {
        load_text(global_ui_strings_filename, "PROMPT_DISCARDACARD");
        do_dialog(player, player, s.selected_card, -1, -1, g_text_lines[2], 0);
      }
    }
    discard_card_from_hand(player, s.selected_card);
  }
  else
  {
    load_text(global_ui_strings_filename, "PROMPT_DISCARDACARD");
    strcpy(s.prompt_line_1, g_text_lines[3]);
    strcpy(s.prompt_line_2, g_text_lines[4]);
    sprintf(s.prompt, " %s\n %s", s.prompt_line_1, s.prompt_line_2);
    s.dialog_result = do_dialog(player, player, s.selected_card, -1, -1, s.prompt, 0);
    if (s.dialog_result == 0)
    {
      real_put_on_top_of_deck(player, PLAYER_CARD_INSTANCE(player, s.selected_card).internal_card_id);
      PLAYER_CARD_INSTANCE(player, s.selected_card).internal_card_id = -1;
      request_duel_display_refresh_if_human(player, s.selected_card, 10, 1);
      if (g_duel_ai_mode_state != 1)
      {
        play_sound_effect(WAV_DISCARD);
      }
      --g_duel_summary.hand_counts[player];
    }
    else
    {
      discard_card_from_hand(player, s.selected_card);
    }
  }
}

// FUNCTION: MOK 0x004358f0
// FUNCTION: MAGIC 0x00442763
// FUNCTION: SHANDALAR 0x0040dffd
color_t single_color_test_bit_to_color_t(color_test_t color_test)
{
  if ((color_test & COLOR_TEST_BLACK) != 0)
  {
    return COLOR_BLACK;
  }
  if ((color_test & COLOR_TEST_BLUE) != 0)
  {
    return COLOR_BLUE;
  }
  if ((color_test & COLOR_TEST_GREEN) != 0)
  {
    return COLOR_GREEN;
  }
  if ((color_test & COLOR_TEST_RED) != 0)
  {
    return COLOR_RED;
  }
  if ((color_test & COLOR_TEST_WHITE) != 0)
  {
    return COLOR_WHITE;
  }

  return COLOR_COLORLESS;
}

// FUNCTION: MAGIC 0x004464f0
// FUNCTION: SHANDALAR 0x00411d90
int is_in_play(int player, int card)
{
  if (global_card_instances[player][card].internal_card_id == -1)
    return 0;

  return (global_card_instances[player][card].state & (STATE_OUBLIETTED | STATE_INVISIBLE | STATE_IN_PLAY)) == STATE_IN_PLAY;
}

// FUNCTION: MAGIC 0x004817fd
// FUNCTION: SHANDALAR 0x00485308
int ante_drawn_card(int player)
{
  int ante_slot = 0;
  int card_in_hand;

  while (global_ante_cards[player][ante_slot] != -1 && ante_slot < 0x10)
  {
    ++ante_slot;
  }

  if (ante_slot < 0x10)
  {
    card_in_hand = draw_card_for_player(player);
    global_ante_cards[player][ante_slot] = PLAYER_CARD_INSTANCE(player, card_in_hand).internal_card_id;
    --g_duel_summary.hand_counts[player];
    load_text("promptsX1.txt", "ANTE_A_CARD");
    if (g_active_player == player)
    {
      do_dialog(player, player, card_in_hand, -1, -1, g_text_lines[0], 0);
    }
    else
    {
      do_dialog(player, player, card_in_hand, -1, -1, g_text_lines[1], 0);
    }

    PLAYER_CARD_INSTANCE(player, card_in_hand).internal_card_id = -1;

    return 1;
  }
  else
  {
    return 0;
  }
}

// FUNCTION: MAGIC 0x00481e25
// FUNCTION: SHANDALAR 0x0048592f
int card_two_headed_giant_of_foriys_legacy(int player, int card, int event)
{
  if (event == 0x78 && card == g_affected_card && player == g_affected_card_controller)
  {
    if ((int)(char)DAMAGE_SOURCE_CARD_INSTANCE(player, card).blocking == g_attacking_card ||
        (int)(char)DAMAGE_SOURCE_CARD_INSTANCE(player, card).blocking ==
            (int)(char)PLAYER_CARD_INSTANCE(g_attacking_card_controller, g_attacking_card).blocking)
    {
      g_event_result = 1;
    }
  }

  dispatch_function_to_all_cards_in_play(player, card, reattach_if_attached_to_source, -1);

  if (g_trigger_condition == 0xdf && card == g_affected_card && player == g_affected_card_controller && g_current_turn == player && player != g_current_player && player == g_trigger_cause_controller && card == g_trigger_cause)
  {
    if (event == 0x7d)
    {
      g_event_result |= 2;
    }
    if (event == 0x7e)
    {
      g_battlefield_extra_ability_flags |= 4;
      DAMAGE_SOURCE_CARD_INSTANCE(player, card).token_status |= 0x8000000;
    }
  }

  if ((g_trigger_condition == 0xcc || event == 199) && card == g_affected_card && player == g_affected_card_controller && g_current_turn == player)
  {
    if (event == 0x7d)
    {
      g_event_result |= 2;
    }
    if (event == 0x7e || event == 199)
    {
      kill_card(player, card, KILL_BURY);
      g_battlefield_extra_ability_flags &= 0xfffffffb;

      DAMAGE_SOURCE_CARD_INSTANCE(player, card).token_status &= ~STATUS_SPECIAL_BLOCKER;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x004821f5
// FUNCTION: SHANDALAR 0x00485d06
int reattach_if_attached_to_source(int source_player, int source_card, int test_player, int test_card, int internal_card_id)
{
  (void)internal_card_id;

  if ((int)(char)PLAYER_CARD_INSTANCE(test_player, test_card).damage_target_player == source_player &&
      PLAYER_CARD_INSTANCE(test_player, test_card).damage_target_card == source_card)
  {
    PLAYER_CARD_INSTANCE(test_player, test_card).damage_target_player =
        PLAYER_CARD_INSTANCE(source_player, source_card).damage_source_player;
    PLAYER_CARD_INSTANCE(test_player, test_card).damage_target_card =
        PLAYER_CARD_INSTANCE(source_player, source_card).damage_source_card;
  }

  return 0;
}

// FUNCTION: MAGIC 0x004823a5
// FUNCTION: SHANDALAR 0x00485eb8
int resolve_control_aura_conflict(int player, int card)
{
  struct
  {
    card_instance_t *instance; /* [ebp-0x20] */
    int target_player;         /* [ebp-0x1c] */
    int target_card;           /* [ebp-0x18] */
    int loop_player;           /* [ebp-0x14] */
    int loop_card;             /* [ebp-0x10] */
    int found_legacy;          /* [ebp-0x0c] */
    int saved_player;          /* [ebp-0x08] */
    int saved_card;            /* [ebp-0x04] */
  } s;

  s.found_legacy = 0;

  s.target_player = (int)(&PLAYER_CARD_INSTANCE(player, card))->damage_target_player;
  s.target_card = (&PLAYER_CARD_INSTANCE(player, card))->damage_target_card;

  s.loop_player = s.target_player;
  for (s.loop_card = 0; s.loop_card < g_active_cards_count[s.loop_player]; ++s.loop_card)
  {
    s.instance = &PLAYER_CARD_INSTANCE(s.loop_player, s.loop_card);
    if (is_in_play(s.loop_player, s.loop_card) &&
        s.instance->internal_card_id == g_control_aura_legacy_internal_card_id &&
        s.instance->damage_target_player == s.target_player &&
        s.instance->damage_target_card == s.target_card &&
        (PLAYER_CARD_INSTANCE((int)s.instance->damage_source_player, s.instance->damage_source_card).state & STATE_TAPPED) == 0)
    {
      s.found_legacy = 1;
      s.saved_player = s.instance->targets[0].player;
      s.saved_card = s.instance->targets[0].card;
      s.instance->targets[0].player = player;
      s.instance->targets[0].card = card;
      s.instance->info_slot = 1;
    }
  }

  for (s.loop_player = 0; s.loop_player < 2; ++s.loop_player)
  {
    for (s.loop_card = 0; s.loop_card < g_active_cards_count[s.loop_player]; ++s.loop_card)
    {
      s.instance = &PLAYER_CARD_INSTANCE(s.loop_player, s.loop_card);
      if (is_in_play(s.loop_player, s.loop_card) &&
          (global_cards_data[s.instance->internal_card_id].id == 0x2c ||
           global_cards_data[s.instance->internal_card_id].id == 0xea ||
           s.instance->internal_card_id == g_control_aura_special_internal_card_id) &&
          s.instance->damage_target_player == s.target_player &&
          s.instance->damage_target_card == s.target_card &&
          (s.instance->token_status & 0x1000000) != 0)
      {
        if (s.found_legacy == 0)
        {
          s.instance->token_status &= ~0x1000000;
        }

        if (s.saved_player == -1)
        {
          (&PLAYER_CARD_INSTANCE(player, card))->damage_source_player = (int8_t)s.loop_player;
          (&PLAYER_CARD_INSTANCE(player, card))->damage_source_card = s.loop_card;
        }
        else
        {
          (&PLAYER_CARD_INSTANCE(player, card))->damage_source_player = (int8_t)s.saved_player;
          (&PLAYER_CARD_INSTANCE(player, card))->damage_source_card = s.saved_card;
        }
      }
    }
  }

  return !s.found_legacy;
}

// FUNCTION: MAGIC 0x004827ad
// FUNCTION: SHANDALAR 0x004862c0
unsigned int get_card_color_after_hacks(int player, int card)
{
  unsigned int result;
  int base_color;

  result = 0;
  base_color = global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].color;
  if ((base_color & 2) != 0)
  {
    result |= 1 << (unsigned char)get_hacked_color(player, card, 1);
  }
  if ((base_color & 4) != 0)
  {
    result |= 1 << (unsigned char)get_hacked_color(player, card, 2);
  }
  if ((base_color & 8) != 0)
  {
    result |= 1 << (unsigned char)get_hacked_color(player, card, 3);
  }
  if ((base_color & 0x10) != 0)
  {
    result |= 1 << (unsigned char)get_hacked_color(player, card, 4);
  }
  if ((base_color & 0x20) != 0)
  {
    result |= 1 << (unsigned char)get_hacked_color(player, card, 5);
  }
  return result;
}

// FUNCTION: MAGIC 0x00482a97
// FUNCTION: SHANDALAR 0x004865a8
int has_effect_source_type(int player, int card, unsigned int flags)
{
  struct
  {
    int source_internal_card_id;
    card_instance_t *instance;
    int found;
    int test_card;
    int source_card;
    int source_player;
    int test_player;
  } s;

  if (player == -1 || flags == 0)
  {
    return 0;
  }

  s.test_player = 0;
  s.found = 0;
  for (; s.test_player < 2 && s.found == 0; ++s.test_player)
  {
    for (s.test_card = 0; s.test_card < g_active_cards_count[s.test_player] && s.found == 0; ++s.test_card)
    {
      s.instance = &PLAYER_CARD_INSTANCE(s.test_player, s.test_card);
      if (s.instance->internal_card_id == g_damage_card_internal_card_id &&
          is_in_play(s.test_player, s.test_card) != 0 &&
          (int)(char)s.instance->damage_target_player == player &&
          s.instance->damage_target_card == card &&
          s.instance->info_slot != 0)
      {
        s.source_player = (int)(char)s.instance->damage_source_player;
        s.source_card = s.instance->damage_source_card;

        if (PLAYER_CARD_INSTANCE(s.source_player, s.source_card).internal_card_id != -1 &&
            global_cards_data[PLAYER_CARD_INSTANCE(s.source_player, s.source_card).internal_card_id].id == g_legacy_display_internal_card_id)
        {
          s.source_player = (int)(char)PLAYER_CARD_INSTANCE(s.source_player, s.source_card).damage_source_player;
          s.source_player = PLAYER_CARD_INSTANCE(s.source_player, s.source_card).damage_source_card;
        }

        if (PLAYER_CARD_INSTANCE(s.source_player, s.source_card).internal_card_id != -1)
        {
          s.source_internal_card_id = PLAYER_CARD_INSTANCE(s.source_player, s.source_card).internal_card_id;
        }
        else
        {
          s.source_internal_card_id = PLAYER_CARD_INSTANCE(s.source_player, s.source_card).original_internal_card_id;
        }

        if ((flags & (unsigned char)global_cards_data[s.source_internal_card_id].type) != 0)
        {
          s.found = 1;
        }
      }
    }
  }

  return s.found;
}

// FUNCTION: MAGIC 0x00483190
// FUNCTION: SHANDALAR 0x00486ca1
int find_matching_active_control_effect(int player, int card, int source_player, int source_card, int internal_card_id)
{
  int result = 0;

  if (global_cards_data[internal_card_id].id ==
          global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].id &&
      ((PLAYER_CARD_INSTANCE(source_player, source_card).token_status & 0x01000000) != 0))
  {
    g_event_result = (source_player << 16) | source_card;
    result = 1;
  }

  return result;
}

// FUNCTION: MAGIC 0x00483242
// FUNCTION: SHANDALAR 0x00486d53
int find_matching_inactive_control_effect(int player, int card, int source_player, int source_card, int internal_card_id)
{
  int result = 0;

  if (global_cards_data[internal_card_id].id ==
          global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].id &&
      ((PLAYER_CARD_INSTANCE(source_player, source_card).token_status & 0x01000000) == 0))
  {
    g_event_result = (source_player << 16) | source_card;
    result = 1;
  }

  return result;
}

// FUNCTION: MAGIC 0x004832f4
// FUNCTION: SHANDALAR 0x00486e05
int graveyard_has_type(int player, int type)
{
  struct
  {
    int result;
    int graveyard_index;
    int done;
  } s;

  s.result = 0;
  if (player == -1 || player == g_active_player)
  {
    for (s.graveyard_index = 0, s.done = 0; s.graveyard_index < 500 && s.done == 0; ++s.graveyard_index)
    {
      if (global_graveyard_slots[g_active_player][s.graveyard_index] != -1)
      {
        if ((type & global_cards_data[global_graveyard_slots[g_active_player][s.graveyard_index]].type) != 0)
        {
          s.result = 1;
          s.done = s.result;
        }
      }
      else
      {
        s.done = 1;
      }
    }
  }

  if ((player == -1 || player == g_other_player) && s.result == 0)
  {
    for (s.graveyard_index = 0, s.done = 0; s.graveyard_index < 500 && s.done == 0; ++s.graveyard_index)
    {
      if (global_graveyard_slots[g_other_player][s.graveyard_index] != -1)
      {
        if ((type & global_cards_data[global_graveyard_slots[g_other_player][s.graveyard_index]].type) != 0)
        {
          s.result = 1;
          s.done = s.result;
        }
      }
      else
      {
        s.done = 1;
      }
    }
  }

  return s.result;
}

// FUNCTION: MAGIC 0x00483489
// FUNCTION: SHANDALAR 0x00486f9a
int card_matches_excluded_land_type(int player, int card, int excluded_internal_card_id)
{
  int internal_card_id;

  internal_card_id = PLAYER_CARD_INSTANCE(player, card).internal_card_id;

  if (excluded_internal_card_id == -1)
  {
    return 0;
  }

  if (global_cards_data[excluded_internal_card_id].code_pointer == (int)global_cards_data[internal_card_id].code_pointer)
  {
    return 1;
  }

  if ((int)*(char *)&global_cards_data[internal_card_id].subtype == 0xc)
  {
    if ((int)*(char *)&global_cards_data[excluded_internal_card_id].subtype != 0xc)
    {
      if (((int)*(char *)&global_cards_data[excluded_internal_card_id].color & (int)*(char *)&global_cards_data[internal_card_id].color) != 0)
      {
        return 1;
      }
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x00484581
// FUNCTION: SHANDALAR 0x0048808b
int is_basic_land_internal_card_id_of_color(int internal_card_id, color_t color)
{
  int result;

  result = 0;
  if ((char)global_cards_data[internal_card_id].subtype == 13 && global_cards_data[internal_card_id].id == g_duel_special_land_card_ids[color])
  {
    result = 1;
  }
  if ((char)global_cards_data[internal_card_id].subtype == 12 &&
      ((1 << color) & (int)PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).mana_color) != 0)
  {
    result = 1;
  }

  return result;
}

// FUNCTION: MAGIC 0x0048463d
// FUNCTION: SHANDALAR 0x00488147
int card_has_basic_land_type(int player, int card, int land_color)
{
  struct
  {
    int result;
    int internal_card_id;
  } s;

  s.result = 0;

  if (global_card_instances[player][card].internal_card_id != -1)
  {
    s.internal_card_id = global_card_instances[player][card].internal_card_id;
  }
  else
  {
    s.internal_card_id = global_card_instances[player][card].original_internal_card_id;
  }

  if (player == -1 || card == -1)
  {
    return 0;
  }

  if ((char)global_cards_data[s.internal_card_id].subtype == 13 && global_cards_data[s.internal_card_id].id == g_duel_special_land_card_ids[land_color])
  {
    s.result = 1;
  }
  if ((char)global_cards_data[s.internal_card_id].subtype == 12 && ((1 << (unsigned char)land_color) & (int)(char)global_card_instances[player][card].mana_color) != 0)
  {
    s.result = 1;
  }

  return s.result;
}

// FUNCTION: MAGIC 0x004848a0
// FUNCTION: SHANDALAR 0x004883aa
int gain_life(int player, int amount, ...)
{
  int saved_trigger_cause_controller;
  int saved_trigger_cause;
  int num_cards_drawn;

  g_life[player] += amount;

  if ((g_battlefield_extra_ability_flags & 0x00200000) != 0)
  {
    saved_trigger_cause_controller = g_trigger_cause_controller;
    saved_trigger_cause = g_trigger_cause;
    push_affected_card_stack();

    g_trigger_cause_controller = player;
    g_trigger_cause = -1;
    g_life_gained = amount;
    dispatch_trigger_twice_once_with_each_player_as_reason(g_current_player, TRIGGER_GAIN_LIFE, &gs_gain_life_007895e0[0], 0);

    g_trigger_cause_controller = saved_trigger_cause_controller;
    g_trigger_cause = saved_trigger_cause;
    pop_affected_card_stack();
  }

  if (g_lich_active[player] != 0)
  {
    for (num_cards_drawn = 0; num_cards_drawn < amount; ++num_cards_drawn)
    {
      draw_card_for_player(player);
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x00495311
// FUNCTION: SHANDALAR 0x00465dac
char *get_hunting_subtype_name(int value)
{
  if (value < 0 || value >= 0xd4)
  {
    return "";
  }
  else
    return gs_hunting_subtype_name_00926930[value];
}

// FUNCTION: MAGIC 0x004b08e6
// FUNCTION: SHANDALAR 0x0046d8c1
void kill_card(int player, int card, kill_t kill_mode)
{
  int internal_card_id;

  if (player == -1 || card == -1)
  {
    return;
  }

  if ((PLAYER_CARD_INSTANCE(player, card).token_status & 0x80) != 0)
  {
    return;
  }

  PLAYER_CARD_INSTANCE(player, card).token_status |= 0x80;
  internal_card_id = PLAYER_CARD_INSTANCE(player, card).internal_card_id;
  if (internal_card_id == -1)
  {
    return;
  }

  if ((PLAYER_CARD_INSTANCE(player, card).state & 2) == 0)
  {
    kill_mode = 3;
  }

  if ((PLAYER_CARD_INSTANCE(player, card).token_status & 8) != 0 || kill_mode == 3 || kill_mode == 4 ||
      (global_cards_data[internal_card_id].type & 0x43) == 0 || global_cards_data[internal_card_id].type == 0x80)
  {
    PLAYER_CARD_INSTANCE(player, card).kill_code = kill_mode;
    process_killed_card(player, card);
  }
  else
  {
    PLAYER_CARD_INSTANCE(player, card).kill_code = kill_mode;
    PLAYER_CARD_INSTANCE(player, card).state |= 2;
    PLAYER_CARD_INSTANCE(player, card).unknown0x14 = 0xd6;
    pending_killed_card_handler = process_killed_card;
  }
}

// FUNCTION: MAGIC 0x004b0b53
// FUNCTION: SHANDALAR 0x0046db2e
int regenerate_or_graveyard_triggers(void)
{
  if (pending_killed_card_handler == 0 || g_regenerate_or_graveyard_triggers_in_progress != 0)
  {
    return 0;
  }

  g_regenerate_or_graveyard_triggers_in_progress = 1;

  g_land_can_be_played |= LCBP_REGENERATION;
  allow_response(-2, g_current_phase, gs_use_regeneration_effects_0091c680, 0x70);
  g_land_can_be_played &= ~LCBP_REGENERATION;

  dispatch_trigger_twice_once_with_each_player_as_reason(g_current_player, TRIGGER_GRAVEYARD_ORDER, gs_graveyard_order_0091cbd0, 0);
  dispatch_trigger_twice_once_with_each_player_as_reason(g_current_player, TRIGGER_GRAVEYARD_FROM_PLAY, gs_cards_to_graveyard_008a8ed0, 0);

  pending_killed_card_handler = 0;
  g_regenerate_or_graveyard_triggers_in_progress = 0;

  TENTATIVE_reassess_all_cards(0, 0xff);
  return 0;
}

// FUNCTION: MAGIC 0x004b0c12
// FUNCTION: SHANDALAR 0x0046dbed
int process_killed_card(int player, int card)
{
  struct
  {
    int saved_trigger_cause_controller;
    int saved_trigger_cause;
    int kill_mode;
    int internal_card_id;
  } s;

  s.internal_card_id = PLAYER_CARD_INSTANCE(player, card).internal_card_id;
  s.kill_mode = (int)(char)PLAYER_CARD_INSTANCE(player, card).kill_code;
  if (s.kill_mode == 0)
  {
    return 0;
  }

  if ((PLAYER_CARD_INSTANCE(player, card).token_status & 8) != 0)
  {
  }
  else
  {
    if (s.kill_mode != 4 &&
        (((unsigned char)global_cards_data[s.internal_card_id].type & TYPE_CREATURE) != 0) &&
        (PLAYER_CARD_INSTANCE(player, card).state & 0x20) == 0)
    {
      ++g_duel_summary.creatures_died;
    }

    if (((unsigned char)global_cards_data[s.internal_card_id].type & 0xc7) != 0)
    {
      push_affected_card_stack();
      g_event_result = 0;
      g_affected_card_controller = player;
      g_affected_card = card;
      g_attacking_card_controller = 1 - player;
      g_attacking_card = -1;
      C_dispatch_event_raw(0x77);
      if (g_event_result > 0)
      {
        PLAYER_CARD_INSTANCE(player, card).token_status &= ~STATUS_DYING;
        pop_affected_card_stack();
        return 0;
      }

      s.kill_mode = (int)(char)PLAYER_CARD_INSTANCE(player, card).kill_code;
      pop_affected_card_stack();
    }

    if ((unsigned char)global_cards_data[s.internal_card_id].type & 0x80)
    {
    }
    else if ((PLAYER_CARD_INSTANCE(player, card).token_status & STATUS_TOKEN) == 0)
    {
      if (s.kill_mode != 4)
      {
        if (s.kill_mode != 3 && g_duel_ai_mode_state != 1)
        {
          if (((unsigned char)global_cards_data[PLAYER_CARD_INSTANCE(player, card).original_internal_card_id].type & TYPE_CREATURE) != 0)
          {
            play_sound_effect(WAV_KILL);
          }
          else if (((unsigned char)global_cards_data[PLAYER_CARD_INSTANCE(player, card).original_internal_card_id].type & TYPE_SPELL) == 0)
          {
            play_sound_effect(WAV_BURIED);
          }
        }

        move_card_to_graveyard(player, card);
        if (s.kill_mode == 3)
        {
          dispatch_trigger_twice_once_with_each_player_as_reason(g_current_player, TRIGGER_GRAVEYARD_FROM_PLAY, gs_cards_to_graveyard_008a8ed0, 0);
        }
      }
      else
      {
        move_card_to_exile((PLAYER_CARD_INSTANCE(player, card).state & STATE_OWNED_BY_OPPONENT) >> 12,
                           PLAYER_CARD_INSTANCE(player, card).original_internal_card_id);
      }
    }
  }

  push_affected_card_stack();
  s.saved_trigger_cause_controller = g_trigger_cause_controller;
  s.saved_trigger_cause = g_trigger_cause;
  g_trigger_cause_controller = player;
  g_trigger_cause = card;
  if (((unsigned int)(unsigned char)global_cards_data[s.internal_card_id].type & 0x1047) != 0)
  {
    dispatch_trigger_twice_once_with_each_player_as_reason(g_current_player, TRIGGER_LEAVE_PLAY, gs_card_leaving_play_007aaef0, 0);
  }
  g_trigger_cause_controller = s.saved_trigger_cause_controller;
  g_trigger_cause = s.saved_trigger_cause;
  pop_affected_card_stack();

  if (((unsigned char)global_cards_data[s.internal_card_id].type & TYPE_CREATURE) != 0)
  {
    --g_duel_summary.creature_counts[player];
  }
  if (((unsigned char)global_cards_data[s.internal_card_id].type & TYPE_ARTIFACT) != 0)
  {
    --g_duel_summary.artifact_counts[player];
  }
  if (((unsigned char)global_cards_data[s.internal_card_id].type & TYPE_ENCHANTMENT) != 0)
  {
    --g_duel_summary.enchantment_counts[player];
  }

  PLAYER_CARD_INSTANCE(player, card).internal_card_id = -1;
  PLAYER_CARD_INSTANCE(player, card).kill_code = 0;
  PLAYER_CARD_INSTANCE(player, card).unknown0x14 = 0;
  if (g_duel_ai_mode_state != 1)
  {
    request_duel_display_refresh_if_human(player, card, 7, 2);
  }

  clear_card_attachments(player, card);
  if (((unsigned int)(unsigned char)global_cards_data[s.internal_card_id].type & 0x1047) != 0)
  {
    reassess_all_cards_and_mana();
  }

  return 0;
}

// FUNCTION: MAGIC 0x004b117e
// FUNCTION: SHANDALAR 0x0046e159
void clear_card_attachments(int player, int card)
{
  struct
  {
    int current_card;   /* ebp - 0x970 */
    int linked_count;   /* ebp - 0x96c */
    int linked_type;    /* ebp - 0x968 */
    int current_player; /* ebp - 0x964 */
    int linked_cards[600];
  } s;

  s.linked_count = 0;
  for (s.current_player = 0; s.current_player < 2; ++s.current_player)
  {
    for (s.current_card = 0; s.current_card < g_active_cards_count[s.current_player]; ++s.current_card)
    {
      if (is_in_play(s.current_player, s.current_card) &&
          PLAYER_CARD_INSTANCE(s.current_player, s.current_card).damage_target_player == player &&
          PLAYER_CARD_INSTANCE(s.current_player, s.current_card).damage_target_card == card &&
          (s.current_player != player || s.current_card != card))
      {
        s.linked_type = global_cards_data[PLAYER_CARD_INSTANCE(s.current_player, s.current_card).internal_card_id].type;
        if ((s.linked_type & 0x43) != 0)
        {
          PLAYER_CARD_INSTANCE(s.current_player, s.current_card).damage_target_player = -1;
          PLAYER_CARD_INSTANCE(s.current_player, s.current_card).damage_target_card = -1;
        }
        else
        {
          s.linked_cards[s.linked_count * 2] = s.current_player;
          s.linked_cards[s.linked_count * 2 + 1] = s.current_card;
          ++s.linked_count;
        }
      }
    }
  }

  while (s.linked_count != 0)
  {
    --s.linked_count;
    kill_card(s.linked_cards[s.linked_count * 2], s.linked_cards[s.linked_count * 2 + 1], 2);
  }

  PLAYER_CARD_INSTANCE(player, card).damage_on_card = 0;
  PLAYER_CARD_INSTANCE(player, card).counter_toughness = PLAYER_CARD_INSTANCE(player, card).damage_on_card;
  PLAYER_CARD_INSTANCE(player, card).counter_power = PLAYER_CARD_INSTANCE(player, card).counter_toughness;
  PLAYER_CARD_INSTANCE(player, card).blocking = 0xff;
}

// FUNCTION: MAGIC 0x004b14f5
// FUNCTION: SHANDALAR 0x0046e4d0
void move_card_to_graveyard(int player, int card)
{
  int graveyard_index;
  int original_internal_card_id;
  unsigned int graveyard_player;

  original_internal_card_id = PLAYER_CARD_INSTANCE(player, card).original_internal_card_id;
  graveyard_player = (PLAYER_CARD_INSTANCE(player, card).state & 0x1000) != 0;
  g_graveyard_card_types[graveyard_player] |= global_cards_data[original_internal_card_id].type;

  graveyard_index = 0;
  while (graveyard_index < 500)
  {
    if (global_graveyard_slots[graveyard_player][graveyard_index] == -1)
    {
      break;
    }
    ++graveyard_index;
  }

  if (graveyard_index < 500)
  {
    global_graveyard_slots[graveyard_player][graveyard_index] = original_internal_card_id;
  }
}

// FUNCTION: MAGIC 0x004b15f7
// FUNCTION: SHANDALAR 0x0046e5d2
void remove_card_from_graveyard(int player, int graveyard_index)
{
  int i = graveyard_index;
  for (; i < 499; ++i)
  {
    global_graveyard_slots[player][i] = global_graveyard_slots[player][i + 1];
  }
  global_graveyard_slots[player][499] = -1;
}

// FUNCTION: MAGIC 0x004b1670
// FUNCTION: SHANDALAR 0x0046e64b
void move_card_to_exile(int player, int internal_card_id)
{
  int exile_index;

  if (g_duel_ai_mode_state != 1 && (global_cards_data[internal_card_id].type & TYPE_CREATURE) != 0)
  {
    play_sound_effect(WAV_DESTROY);
  }

  for (exile_index = 0; exile_index < 500; exile_index++)
  {
    if (global_exile[player][exile_index] == -1)
    {
      global_exile[player][exile_index] = internal_card_id;
      return;
    }
  }
}

// FUNCTION: MAGIC 0x004b4110
// FUNCTION: SHANDALAR 0x0056a37b
void exit_duel_thread(int player)
{
  PostMessageA((void *)g_duel_window_hwnd, 0x401, (unsigned int)player, 0);
  ExitThread((unsigned int)player);
}

// FUNCTION: MAGIC 0x004b4654
// FUNCTION: SHANDALAR 0x0056ae5d
int add_card_to_hand(int player, int internal_card_id)
{
  int card;

  if (internal_card_id == -1)
  {
    return -1;
  }

  for (card = 0; card < 0x96; ++card)
  {
    if (PLAYER_CARD_INSTANCE(player, card).original_internal_card_id == -1)
    {
      initialize_card_instance(player, internal_card_id, card);
      if (g_active_cards_count[player] <= card)
      {
        g_active_cards_count[player] = card + 1;
      }
      return card;
    }
  }

  show_duel_system_error("AddCard error: No more room in cd to add a card");
  return -1;
}

int calculate_attack_rating(int player, int card);

// FUNCTION: MAGIC 0x004b4720
// FUNCTION: SHANDALAR 0x0056af29
void initialize_card_instance(int player, int internal_card_id, int card)
{
  int i;

  PLAYER_CARD_INSTANCE(player, card).original_internal_card_id = internal_card_id;
  PLAYER_CARD_INSTANCE(player, card).internal_card_id = PLAYER_CARD_INSTANCE(player, card).original_internal_card_id;
  PLAYER_CARD_INSTANCE(player, card).dummy3 = 0;

  if (player == 0)
  {
    PLAYER_CARD_INSTANCE(player, card).state = 0;
  }
  else
  {
    PLAYER_CARD_INSTANCE(player, card).state = 0x1000;
  }

  PLAYER_CARD_INSTANCE(player, card).damage_on_card = 0;
  PLAYER_CARD_INSTANCE(player, card).damage_target_player = (char)-1;
  PLAYER_CARD_INSTANCE(player, card).damage_target_card = -1;
  PLAYER_CARD_INSTANCE(player, card).damage_source_player = (char)-1;
  PLAYER_CARD_INSTANCE(player, card).damage_source_card = -1;

  PLAYER_CARD_INSTANCE(player, card).power = global_cards_data[internal_card_id].power;
  PLAYER_CARD_INSTANCE(player, card).toughness = global_cards_data[internal_card_id].toughness;
  PLAYER_CARD_INSTANCE(player, card).counter_power = 0;
  PLAYER_CARD_INSTANCE(player, card).counter_toughness = 0;

  PLAYER_CARD_INSTANCE(player, card).color = (char)global_cards_data[internal_card_id].color;
  PLAYER_CARD_INSTANCE(player, card).mana_color = PLAYER_CARD_INSTANCE(player, card).color;

  PLAYER_CARD_INSTANCE(player, card).blocking = 0xff;
  PLAYER_CARD_INSTANCE(player, card).unknown0x37 = 0;
  PLAYER_CARD_INSTANCE(player, card).kill_code = 0;

  PLAYER_CARD_INSTANCE(player, card).eot_toughness = 0;
  PLAYER_CARD_INSTANCE(player, card).info_slot = PLAYER_CARD_INSTANCE(player, card).eot_toughness;
  PLAYER_CARD_INSTANCE(player, card).timestamp = -1;

  PLAYER_CARD_INSTANCE(player, card).token_status = 0;
  PLAYER_CARD_INSTANCE(player, card).regen_status = 0x08000000;

  PLAYER_CARD_INSTANCE(player, card).attack_rating = calculate_attack_rating(player, card);

  PLAYER_CARD_INSTANCE(player, card).special_counters = 0;
  PLAYER_CARD_INSTANCE(player, card).unknown0x14 = 0;
  PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
  PLAYER_CARD_INSTANCE(player, card).untap_status = 0;
  PLAYER_CARD_INSTANCE(player, card).upkeep_flags = PLAYER_CARD_INSTANCE(player, card).untap_status;

  for (i = 0; i < 7; ++i)
  {
    PLAYER_CARD_INSTANCE(player, card).mana_to_untap[i] = 0;
  }

  PLAYER_CARD_INSTANCE(player, card).upkeep_colorless = 0;
  PLAYER_CARD_INSTANCE(player, card).upkeep_black = 0;
  PLAYER_CARD_INSTANCE(player, card).upkeep_blue = 0;
  PLAYER_CARD_INSTANCE(player, card).upkeep_green = 0;
  PLAYER_CARD_INSTANCE(player, card).upkeep_red = 0;
  PLAYER_CARD_INSTANCE(player, card).upkeep_white = 0;
  PLAYER_CARD_INSTANCE(player, card).upkeep_artmana = 0;

  for (i = 0; i < 6; ++i)
  {
    PLAYER_CARD_INSTANCE(player, card).color_id[i] = 0;
    PLAYER_CARD_INSTANCE(player, card).hack_mode[i] = 0;
  }

  for (i = 0; i < 19; ++i)
  {
    PLAYER_CARD_INSTANCE(player, card).targets[i].player = (char)-1;
    PLAYER_CARD_INSTANCE(player, card).targets[i].card = -1;
  }

  PLAYER_CARD_INSTANCE(player, card).parent_controller = -1;
  PLAYER_CARD_INSTANCE(player, card).parent_card = -1;

  PLAYER_CARD_INSTANCE(player, card).counters = 0;

  if ((global_cards_data[internal_card_id].extra_ability & EA_MANA_SOURCE))
  {
    if (global_cards_data[internal_card_id].type == 1 || global_cards_data[internal_card_id].type == 0x40)
    {
      PLAYER_CARD_INSTANCE(player, card).color = 1;
    }

    switch (global_cards_data[internal_card_id].id)
    {
    case 0x134:
    case 0x27b:
      PLAYER_CARD_INSTANCE(player, card).mana_color = 0x10;
      break;

    case 0x139:
      PLAYER_CARD_INSTANCE(player, card).mana_color = 2;
      break;

    case 300:
    case 0x272:
      PLAYER_CARD_INSTANCE(player, card).mana_color = 1;
      break;

    case 0xf:
    case 0x193:
      PLAYER_CARD_INSTANCE(player, card).mana_color = 0x3e;
      break;

    default:
      break;
    }
  }

  TENTATIVE_set_timestamps(player, card);
}

// FUNCTION: MAGIC 0x004b58e2
// FUNCTION: SHANDALAR 0x0056c0e5
int is_card_color_compatible_with_mask(int card_color, int color_mask, int compatibility_level)
{
  if (card_color == 1 || color_mask == 1)
  {
    return 1;
  }

  card_color = (int)single_color_test_bit_to_color_t(card_color);
  color_mask = (int)single_color_test_bit_to_color_t(color_mask);

  if ((char)wizard_card_color_preference_table[color_mask * 3] == card_color)
  {
    return 1;
  }

  if (compatibility_level > 1 && (char)wizard_card_color_preference_table[color_mask * 3 + 1] == card_color)
  {
    return 1;
  }

  if (compatibility_level > 2 && (char)wizard_card_color_preference_table[color_mask * 3 + 2] == card_color)
  {
    return 1;
  }

  if (compatibility_level > 3)
  {
    return 1;
  }

  return 0;
}

// FUNCTION: MAGIC 0x004b59b2
// FUNCTION: SHANDALAR 0x0056c1b5
void shuffle_duel_library(int player, int deck_owner)
{
  struct
  {
    int swap_slot;    /* [ebp-0x14] */
    int temp;         /* [ebp-0x10] */
    int unused;       /* [ebp-0x0c] */
    int current_slot; /* [ebp-0x08] */
    int deck_size;    /* [ebp-0x04] */
  } s;

  if (g_duel_ai_mode_state != 1)
  {
    play_sound_effect(WAV_SHUFFLE);
    notify_library_shuffled(deck_owner);
  }

  if ((g_duel_network_flags & 2) == 0 || g_active_player == player)
  {
    s.deck_size = 500;
    for (s.current_slot = 0; s.current_slot < 500; ++s.current_slot)
    {
      if (global_library[deck_owner][s.current_slot] == -1)
      {
        s.deck_size = s.current_slot;
        break;
      }
    }

    for (s.current_slot = 0; s.current_slot < s.deck_size; ++s.current_slot)
    {
      s.swap_slot = s.current_slot + internal_rand(s.deck_size - s.current_slot);
      if (global_library[deck_owner][s.swap_slot] != -1)
      {
        s.temp = global_library[deck_owner][s.swap_slot];
        global_library[deck_owner][s.swap_slot] = global_library[deck_owner][s.current_slot];
        global_library[deck_owner][s.current_slot] = s.temp;
      }
    }

    for (s.current_slot = s.deck_size; s.current_slot < 500; ++s.current_slot)
    {
      global_library[deck_owner][s.current_slot] = -1;
    }

    if ((g_duel_network_flags & 2) != 0)
    {
      send_library_contents(deck_owner);
    }
  }
  else
  {
    receive_library_contents(deck_owner);
  }
}

// FUNCTION: MAGIC 0x004b5b7a
// FUNCTION: SHANDALAR 0x0056c37c
void send_library_contents(int deck_owner)
{
  int done;
  int index;

  done = 0;
  index = 0;
  while (done == 0)
  {
    if (global_library[deck_owner][index] != -1 && index < 500)
      AddCardToCLPacket(global_library[deck_owner][index]);
    else
      done = 1;
    ++index;
  }

  AddCardToCLPacket(-1);
  TENTATIVE_send_network_result(0, 4);
}

// FUNCTION: MAGIC 0x004b5c19
// FUNCTION: SHANDALAR 0x0056c41b
void receive_library_contents(int deck_owner)
{
  struct
  {
    int index;
    int done;
    int packet_card;
  } s;

  s.done = 0;
  s.index = 0;
  TENTATIVE_wait_for_network_result(1, 4);
  while (s.done == 0)
  {
    s.packet_card = GetCardFromCLPacket(s.index);
    if (s.packet_card == -1)
    {
      s.done = 1;
    }
    else
    {
      global_library[deck_owner][s.index] = s.packet_card;
      ++s.index;
    }
  }
}

// FUNCTION: MAGIC 0x004b5c94
// FUNCTION: SHANDALAR 0x0056c496
void remove_card_from_deck(int player, int position)
{
  int current_slot;

  for (current_slot = position; current_slot < 499; ++current_slot)
  {
    global_library[player][current_slot] = global_library[player][current_slot + 1];
  }

  global_library[player][499] = -1;
}

// FUNCTION: MAGIC 0x004b5cf5
// FUNCTION: SHANDALAR 0x0056c4f7
void put_card_on_bottom_of_library(int player, int internal_card_id)
{
  int deck_position;

  deck_position = 0;
  while (deck_position < 500 && global_library[player][deck_position] != -1)
  {
    ++deck_position;
  }
  if (deck_position < 500)
  {
    global_library[player][deck_position] = internal_card_id;
  }
}

// FUNCTION: MAGIC 0x004b5d72
// FUNCTION: SHANDALAR 0x0056c574
void real_put_on_top_of_deck(int player, int internal_card_id)
{
  int index;

  for (index = 499; index > 0; --index)
  {
    global_library[player][index] = global_library[player][index - 1];
  }
  global_library[player][0] = internal_card_id;
}

// FUNCTION: MAGIC 0x004b5f03
// FUNCTION: SHANDALAR 0x0056c705
int find_internal_card_id_by_csv_id(card_id_t card_id)
{
  struct
  {
    int result;
    int i;
  } s;

  if (card_id == -1)
  {
    return -1;
  }

  s.result = -1;
  for (s.i = 0; s.i < g_card_count + 0x10; ++s.i)
  {
    if (global_cards_data[s.i].id == card_id)
    {
      return s.i;
    }
  }

  return s.result;
}

// FUNCTION: MAGIC 0x004bff5a
// FUNCTION: SHANDALAR 0x004c0dd4
int has_pending_damage(int player, int card)
{
  struct
  {
    int result;
    int test_card;
    int test_player;
  } s;

  s.result = 0;
  for (s.test_player = 0; s.test_player < 2 && s.result == 0; ++s.test_player)
  {
    for (s.test_card = 0; s.test_card < g_active_cards_count[s.test_player] && s.result == 0; ++s.test_card)
    {
      if (PLAYER_CARD_INSTANCE(s.test_player, s.test_card).internal_card_id == g_damage_card_internal_card_id &&
          PLAYER_CARD_INSTANCE(s.test_player, s.test_card).damage_target_player == player &&
          PLAYER_CARD_INSTANCE(s.test_player, s.test_card).damage_target_card == card)
      {
        s.result = 1;
      }
    }
  }
  return s.result;
}

// FUNCTION: MAGIC 0x004c081a
// FUNCTION: SHANDALAR 0x004c1695
int is_basic_land(int player, int card)
{
  int result;

  result = 0;
  if ((int)(char)global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].subtype == 0xd)
  {
    result = 1;
  }

  return result;
}

// FUNCTION: MAGIC 0x004c0880
// FUNCTION: SHANDALAR 0x004c16fb
int basic_land_matches_type_index(int internal_card_id, int extra)
{
  int result;

  result = 0;
  if (extra == 0 && (extra == internal_card_id || global_cards_data[internal_card_id].id == g_duel_special_land_card_ids[1]))
  {
    result = 1;
  }
  if (extra == 1 && (extra == internal_card_id || global_cards_data[internal_card_id].id == g_duel_special_land_card_ids[2]))
  {
    result = 1;
  }
  if (extra == 2 && (extra == internal_card_id || global_cards_data[internal_card_id].id == g_duel_special_land_card_ids[3]))
  {
    result = 1;
  }
  if (extra == 3 && (extra == internal_card_id || global_cards_data[internal_card_id].id == g_duel_special_land_card_ids[4]))
  {
    result = 1;
  }
  if (extra == 4 && (extra == internal_card_id || global_cards_data[internal_card_id].id == g_duel_special_land_card_ids[5]))
  {
    result = 1;
  }
  return result;
}

// FUNCTION: MAGIC 0x004e698f
// FUNCTION: SHANDALAR 0x004c5bf6
void get_landwalk_evasion_masks(unsigned int *out_landwalk_mask, unsigned int *out_basic_land_mask)
{
  int color;
  unsigned int bits2;
  unsigned int bits1;

  bits2 = 0;
  bits1 = bits2;

  for (color = 1; color <= 5; ++color)
  {
    if (g_basiclandtypes_controlled[1][color] > 0)
    {
      bits1 |= 1u << (color - 1);
    }
    if (g_basiclandtypes_controlled[0][color] > 0)
    {
      bits2 |= 1u << (color - 1);
    }
  }

  if (out_landwalk_mask != NULL)
  {
    *out_landwalk_mask = bits1;
  }
  if (out_basic_land_mask != NULL)
  {
    *out_basic_land_mask = bits2;
  }
}

// FUNCTION: MOK 0x004a09a0
// FUNCTION: MAGIC 0x004ef850
// FUNCTION: SHANDALAR 0x004a9910
int create_legacy_effect(int player, int card, int legacy_iid, int target_player, int target_card)
{
  struct
  {
    int i;
    int legacy_card;
    int source_internal_card_id;
  } s;

  s.legacy_card = add_card_to_hand(player, legacy_iid);
  if (s.legacy_card != -1 && card != -1)
  {
    PLAYER_CARD_INSTANCE(player, s.legacy_card).state = ((player != 0) ? STATE_OWNED_BY_OPPONENT : 0) | STATE_IN_PLAY;
    PLAYER_CARD_INSTANCE(player, s.legacy_card).mana_color = PLAYER_CARD_INSTANCE(player, card).mana_color;
    PLAYER_CARD_INSTANCE(player, s.legacy_card).color = PLAYER_CARD_INSTANCE(player, card).color;
    PLAYER_CARD_INSTANCE(player, s.legacy_card).damage_source_player = (char)player;
    PLAYER_CARD_INSTANCE(player, s.legacy_card).damage_source_card = card;

    if (PLAYER_CARD_INSTANCE(player, card).internal_card_id == -1 || PLAYER_CARD_INSTANCE(player, card).internal_card_id == g_stack_proxy_internal_card_id)
    {
      s.source_internal_card_id = PLAYER_CARD_INSTANCE(player, card).original_internal_card_id;
    }
    else
    {
      s.source_internal_card_id = PLAYER_CARD_INSTANCE(player, card).internal_card_id;
    }

    *(unsigned int *)&PLAYER_CARD_INSTANCE(player, s.legacy_card).display_pic_csv_id =
        ((unsigned int)get_card_display_pic_num(global_cards_data[s.source_internal_card_id].id, player, card) << 16) |
        (unsigned int)global_cards_data[s.source_internal_card_id].id;
    PLAYER_CARD_INSTANCE(player, s.legacy_card).damage_target_player = (char)target_player;
    PLAYER_CARD_INSTANCE(player, s.legacy_card).damage_target_card = target_card;

    if (target_player != -1 && target_card != -1)
    {
      PLAYER_CARD_INSTANCE(target_player, target_card).regen_status |= 0x0f000000;
    }

    PLAYER_CARD_INSTANCE(player, s.legacy_card).token_status |= PLAYER_CARD_INSTANCE(player, card).token_status & 6;
    for (s.i = 0; s.i < 6; ++s.i)
    {
      PLAYER_CARD_INSTANCE(player, s.legacy_card).color_id[s.i] = PLAYER_CARD_INSTANCE(player, card).color_id[s.i];
      PLAYER_CARD_INSTANCE(player, s.legacy_card).hack_mode[s.i] = PLAYER_CARD_INSTANCE(player, card).hack_mode[s.i];
    }
  }

  return s.legacy_card;
}

// FUNCTION: MAGIC 0x00500135
// FUNCTION: SHANDALAR 0x004ba25e
int create_a_card_type(int internal_card_id)
{
  int new_internal_card_id;

  for (new_internal_card_id = g_card_count; new_internal_card_id < g_card_count + 0x10; ++new_internal_card_id)
  {
    if (*(int *)&global_cards_data[new_internal_card_id].id == -1)
    {
      memcpy(&global_cards_data[new_internal_card_id], &global_cards_data[internal_card_id], 0x48);
      return new_internal_card_id;
    }
  }

  show_duel_system_error("AddType error: No room in ci to add another type");
  return -1;
}

// FUNCTION: MAGIC 0x005001c4
// FUNCTION: SHANDALAR 0x004ba2ed
void invalidate_dynamic_card_type(int internal_card_id)
{
  global_cards_data[internal_card_id].id = -1;
}

// FUNCTION: MAGIC 0x005001e0
// FUNCTION: SHANDALAR 0x004ba309
int get_hacked_color(int player, int card, int orig_color)
{
  if ((char)global_card_instances[player][card].hack_mode[orig_color] == 0)
  {
    return orig_color;
  }
  else
  {
    return (char)global_card_instances[player][card].hack_mode[orig_color];
  }
}

// FUNCTION: MAGIC 0x0050026d
// FUNCTION: SHANDALAR 0x004ba392
int get_sleighted_color(int player, int card, int orig_color)
{
  if ((char)global_card_instances[player][card].color_id[orig_color] == 0)
  {
    return orig_color;
  }
  else
  {
    return (char)global_card_instances[player][card].color_id[orig_color];
  }
}

// FUNCTION: MAGIC 0x0050047c
// FUNCTION: SHANDALAR 0x004ba5a1
int damage_creature(int target_player, int target_card, int amount, int source_player, int source_card)
{
  struct
  {
    int source_internal_card_id;
    int result;
    int damage_player;
  } s;

  if (target_player == -1 || source_player == -1 || amount <= 0)
  {
    return -1;
  }

  if (target_card == -1)
  {
    s.damage_player = target_player;
  }
  else
  {
    s.damage_player = source_player;
  }

  s.result = add_card_to_hand(s.damage_player, g_damage_card_internal_card_id);
  if (s.result != -1)
  {
    PLAYER_CARD_INSTANCE(s.damage_player, s.result).state |=
        ((-(s.damage_player != 0)) & 0x1000) | 2;
    PLAYER_CARD_INSTANCE(s.damage_player, s.result).damage_target_player = (char)target_player;
    PLAYER_CARD_INSTANCE(s.damage_player, s.result).damage_target_card = target_card;
    PLAYER_CARD_INSTANCE(s.damage_player, s.result).info_slot = amount;
    PLAYER_CARD_INSTANCE(s.damage_player, s.result).damage_source_player = (char)source_player;
    PLAYER_CARD_INSTANCE(s.damage_player, s.result).damage_source_card = source_card;
    if (source_card != -1)
    {
      if (PLAYER_CARD_INSTANCE(source_player, source_card).internal_card_id == -1 ||
          PLAYER_CARD_INSTANCE(source_player, source_card).internal_card_id == g_stack_proxy_internal_card_id)
      {
        s.source_internal_card_id = PLAYER_CARD_INSTANCE(source_player, source_card).original_internal_card_id;
      }
      else
      {
        s.source_internal_card_id = PLAYER_CARD_INSTANCE(source_player, source_card).internal_card_id;
      }

      PLAYER_CARD_INSTANCE(s.damage_player, s.result).color = PLAYER_CARD_INSTANCE(source_player, source_card).color;
      if ((global_cards_data[s.source_internal_card_id].type & TYPE_ARTIFACT) != 0)
      {
        PLAYER_CARD_INSTANCE(s.damage_player, s.result).color |= 0x40;
      }

      PLAYER_CARD_INSTANCE(s.damage_player, s.result).eot_toughness =
          (unsigned int)(unsigned char)global_cards_data[s.source_internal_card_id].type;

      if ((PLAYER_CARD_INSTANCE(source_player, source_card).state & 4) != 0)
      {
        if (g_current_phase == 0x19)
        {
          PLAYER_CARD_INSTANCE(s.damage_player, s.result).token_status |= 0x100000;
        }
        if (g_current_phase == 0x1a)
        {
          PLAYER_CARD_INSTANCE(s.damage_player, s.result).token_status |= 0x40000;
        }
      }
      if (global_cards_data[s.source_internal_card_id].id == g_effect_display_internal_card_id || global_cards_data[s.source_internal_card_id].id == g_legacy_display_internal_card_id || global_cards_data[s.source_internal_card_id].id == g_multiblock_display_internal_card_id)
      {
        *(unsigned int *)&PLAYER_CARD_INSTANCE(s.damage_player, s.result).display_pic_csv_id =
            *(unsigned int *)&PLAYER_CARD_INSTANCE(source_player, source_card).display_pic_csv_id;
      }
      else
      {
        *(unsigned int *)&PLAYER_CARD_INSTANCE(s.damage_player, s.result).display_pic_csv_id =
            (get_card_display_pic_num(global_cards_data[s.source_internal_card_id].id, source_player, source_card) << 16) |
            global_cards_data[s.source_internal_card_id].id;
      }
    }
    else
    {
      *(unsigned int *)&PLAYER_CARD_INSTANCE(s.damage_player, s.result).display_pic_csv_id = 0xef;
    }
    g_land_can_be_played |= LCBP_PENDING_DAMAGE_CARDS;
  }

  return s.result;
}

// FUNCTION: MAGIC 0x00500a13
// FUNCTION: SHANDALAR 0x004bab38
void damage_player(int target_player, int amount, int source_player, int source_card)
{
  damage_creature(target_player, -1, amount, source_player, source_card);
}

// FUNCTION: MAGIC 0x005510dc
// FUNCTION: SHANDALAR 0x0049f31f
int has_permanent_of_type(int player, int type_mask)
{
  int current_card;

  for (current_card = 0; current_card < g_active_cards_count[player]; ++current_card)
  {
    if (is_in_play(player, current_card) && (type_mask & global_cards_data[PLAYER_CARD_INSTANCE(player, current_card).internal_card_id].type) != 0)
    {
      return 1;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x00551240
// FUNCTION: SHANDALAR 0x0049f483
void add_special_counter(int player, int card)
{
  if ((PLAYER_CARD_INSTANCE(player, card).special_counters & 0xffu) == 0xffu)
  {
    return;
  }

  /* The exe increments only the low byte and preserves the upper 3 bytes. */
  PLAYER_CARD_INSTANCE(player, card).special_counters =
      ((PLAYER_CARD_INSTANCE(player, card).special_counters + 1) & 0xff) |
      (PLAYER_CARD_INSTANCE(player, card).special_counters & 0xffffff00);

  if (g_duel_ai_mode_state != 1)
  {
    play_sound_effect(ICON_HASTE);
  }
}

// FUNCTION: MAGIC 0x00551334
// FUNCTION: SHANDALAR 0x0049f577
void remove_special_counter(int player, int card)
{
  /* The exe only decrements the low byte of special_counters and preserves the upper 3 bytes. */
  global_card_instances[player][card].special_counters =
      (global_card_instances[player][card].special_counters & 0xffffff00) |
      ((global_card_instances[player][card].special_counters - 1) & 0xff);
}

// FUNCTION: MAGIC 0x005514cd
// FUNCTION: SHANDALAR 0x0049f710
void remove_special_counters(int player, int card, int amount)
{
  global_card_instances[player][card].special_counters =
      ((global_card_instances[player][card].special_counters - amount) & 0xff) |
      (global_card_instances[player][card].special_counters & 0xffffff00);
}

// FUNCTION: MAGIC 0x00551572
// FUNCTION: SHANDALAR 0x0049f7b5
void set_special_counters(int player, int card, int amount)
{
  if (amount > 0xff)
  {
    amount = 0xff;
  }

  global_card_instances[player][card].special_counters =
      (global_card_instances[player][card].special_counters & 0xffffff00) | (unsigned char)amount;
}

// FUNCTION: MAGIC 0x005515f5
// FUNCTION: SHANDALAR 0x0049f838
unsigned int C_get_special_counters(int player, int card)
{
  return global_card_instances[player][card].special_counters & 0xff;
}

// FUNCTION: MAGIC 0x0055d802
// FUNCTION: SHANDALAR 0x00573b1c
void extract_numbered_text_choice(char *out, char *in, int choice)
{
  struct
  {
    char *write_ptr;
    int done;
    int matched;
    char *scan;
  } s;

  if (out == NULL || in == NULL)
  {
    return;
  }

  s.done = 0;
  s.matched = 0;
  s.scan = in;
  while (s.done == 0)
  {
    while (*s.scan != '\0' && strncmp(s.scan, "|#", 2) != 0)
    {
      ++s.scan;
    }

    if (*s.scan == '\0')
    {
      s.done = 1;
    }
    else
    {
      ++s.scan;
      ++s.scan;
      if (atoi(s.scan) == choice)
      {
        s.matched = 1;
        ++s.scan;
        s.write_ptr = out;
        while (*s.scan != '\0' && strncmp(s.scan, "|#", 2) != 0)
        {
          *s.write_ptr = *s.scan;
          ++s.scan;
          ++s.write_ptr;
        }
        *s.write_ptr = '\0';
        s.done = 1;
      }
    }
  }
}
