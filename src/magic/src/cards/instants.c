#include <stdio.h>
#include <string.h>
#include "../game_support.h"
#include "../global_strings.h"

int dispatch_trigger_twice_once_with_each_player_as_reason(int reason_for_trig, trigger_t trig, const char *prompt, int a4);
void record_ai_action_selection(void);
void replay_ai_action_selection(void);
int choose_orcish_catapult_targets(int player, int card, target_t *targets);
void hurkyls_recall_bounce_artifact(int target_player, int target_card);
int gain_life_or_prevent_damage(int player, int card, event_t event, int amount);
void show_duel_system_error(const char *err_msg);

// FUNCTION: MAGIC 0x004f5d83
// FUNCTION: SHANDALAR 0x004afe3b
int card_fork(int player, int card, event_t event)
{
  int new_card;

  if (event == EVENT_CAN_CAST)
  {
    load_recorded_action_target(0);
    if (g_current_spell_player != -1)
    {
      PLAYER_CARD_INSTANCE(player, card).info_slot = g_x_value;
      return ((global_cards_data[PLAYER_CARD_INSTANCE(g_current_spell_player, g_current_spell_card).internal_card_id].type & (TYPE_INSTANT | TYPE_SORCERY)) != 0 && C_real_validate_target(g_current_spell_player, g_current_spell_card, (char *)0, player, 2, 2, 0,
                                                                                                                                                                                           TYPE_NONE, TYPE_NONE, 0, 0, COLOR_TEST_0, COLOR_TEST_0,
                                                                                                                                                                                           -1, ~SUB_WALL, -1, -1, TARGET_SPECIAL_SPELL_ON_STACK, 0,
                                                                                                                                                                                           0))
                 ? 99
                 : 0;
    }
    else
    {
      return 0;
    }
  }

  if (event == EVENT_CAST_SPELL && g_affected_card == card && g_affected_card_controller == player)
  {
    if (g_current_spell_player != -1)
    {
      PLAYER_CARD_INSTANCE(player, card).targets[0].player = g_current_spell_player;
      PLAYER_CARD_INSTANCE(player, card).targets[0].card = g_current_spell_card;
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
    }
    else
    {
      g_spell_fizzled = 1;
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    new_card = add_card_to_hand(player,
                                TARGET_CARD_INSTANCE(player, card, 0).internal_card_id);
    if (new_card != -1)
    {
      PLAYER_CARD_INSTANCE(player, new_card).color = COLOR_TEST_RED;
      PLAYER_CARD_INSTANCE(player, new_card).token_status |= 8;
      g_x_value = PLAYER_CARD_INSTANCE(player, card).info_slot;
      g_land_can_be_played |= LCBP_CARD_BEING_COPIED;
      process_card_enters_play(player, new_card);
      g_land_can_be_played &= ~LCBP_CARD_BEING_COPIED;
    }
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

int tap_mana_producing_land_callback(int player, int card, int internal_card_id);

// FUNCTION: MAGIC 0x004f6108
// FUNCTION: SHANDALAR 0x004b01c2
int card_mana_short(int player, int card, event_t event)
{
  int target_player;
  int color;

  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if (((event == EVENT_CAST_SPELL) && (card == g_affected_card)) && (player == g_affected_card_controller))
  {
    target_t selected_target;

    load_text("prompts.txt", "MANASHORT");
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
                             &selected_target))
    {
      SET_TARGET(PLAYER_CARD_INSTANCE(player, card).targets[0], selected_target);
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
    dispatch_three_arg_callback_to_cards_in_play(tap_mana_producing_land_callback, target_player);
    for (color = 0; color < 8; ++color)
    {
      g_raw_mana_available[target_player][color] = 0;
    }
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004f6311
int tap_mana_producing_land_callback(int player, int card, int internal_card_id)
{
  g_required_mana_color_mask = 1;
  g_produced_mana_color = -1;
  if ((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0 && (global_cards_data[internal_card_id].type & TYPE_LAND) != 0 && (global_cards_data[internal_card_id].extra_ability & 0x1000) != 0 && (PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0 && (global_cards_data[internal_card_id].type & TYPE_LAND) != 0)
  {
    dispatch_event_to_single_card(player, card, EVENT_ACTIVATE, 1 - player, -1);
    if ((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) != 0)
    {
      dispatch_event(player, card, EVENT_TAP_CARD);
    }
  }
  g_required_mana_color_mask = 0;
  return 0;
}

// FUNCTION: MAGIC 0x004f6456
// FUNCTION: SHANDALAR 0x004b0512
int card_siren_s_call(int player, int card, event_t event)
{
  if (event == EVENT_CAN_CAST)
  {
    return (g_current_player != player && g_current_phase < PHASE_DECLARE_ATTACKERS) ? 1 : 0;
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    create_legacy_effect(player, card, g_duel_generated_internal_card_id_18, -1, -1);
    kill_card(player, card, KILL_DESTROY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004f64d5
// FUNCTION: SHANDALAR 0x004b0590
int card_ancestral_recall(int player, int card, event_t event)
{
  struct
  {
    target_t selected_target;
    int target_player;
  } s;

  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if (((event == EVENT_CAST_SPELL) && (g_affected_card == card)) && (g_affected_card_controller == player))
  {
    load_text("prompts.txt", "ANCESTRAL_RECALL");
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
                             &s.selected_target))
    {
      SET_TARGET(PLAYER_CARD_INSTANCE(player, card).targets[0], s.selected_target);
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
    }
    else
    {
      g_spell_fizzled = 1;
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    s.target_player = PLAYER_CARD_INSTANCE(player, card).targets[0].player;
    draw_card_for_player(s.target_player);
    draw_card_for_player(s.target_player);
    draw_card_for_player(s.target_player);
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    kill_card(player, card, KILL_DESTROY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004f66bc
// FUNCTION: SHANDALAR 0x004b0777
int card_simulacrum(int player, int card, event_t event)
{
  int current_card;
  int current_player;
  int target_card;
  int target_player;

  if (event == EVENT_CAN_CAST)
  {
    load_recorded_action_target(0);
    if (!real_target_available((int *)0, TARGET_SCAN_DIRECT, player, player, player, 0x200, 2, 0, 0,
                               get_protections_from(player, card), 0, 0, -1, -1, 0xffffffff,
                               0xffffffff, 0, 0, 0))
    {
      return 0;
    }
    if ((g_land_can_be_played & LCBP_DAMAGE_PREVENTION) == 0)
    {
      return 1;
    }
    return real_target_available((int *)0, TARGET_SCAN_DIRECT, player, 2, 2, 0x200, 0, 0, 0, 0, 0, 0,
                                 g_damage_card_internal_card_id, -1, 0xffffffff, 0xffffffff, 0x20, 0, 0)
               ? 99
               : 0;
  }

  if ((event == EVENT_CAST_SPELL) && (card == g_affected_card) && (player == g_affected_card_controller))
  {
    load_text("prompts.txt", "SIMULACRUM");
    if (!select_target_creature_and_store(player, player, card))
    {
      g_spell_fizzled = 1;
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    target_player = PLAYER_CARD_INSTANCE(player, card).targets[0].player;
    target_card = PLAYER_CARD_INSTANCE(player, card).targets[0].card;

    if (C_real_validate_target(target_player, target_card, (char *)0, player, player, player,
                               TARGET_ZONE_IN_PLAY, TYPE_CREATURE, TYPE_NONE, 0,
                               get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0, -1,
                               ~SUB_WALL, -1, -1, 0, 0, 0))
    {
      if ((g_land_can_be_played & LCBP_DAMAGE_PREVENTION) != 0)
      {
        PLAYER_CARD_INSTANCE(player, card).eot_toughness = 0;
        for (current_player = 0; current_player < 2; ++current_player)
        {
          for (current_card = 0; current_card < g_active_cards_count[current_player]; ++current_card)
          {
            if (PLAYER_CARD_INSTANCE(current_player, current_card).internal_card_id == g_damage_card_internal_card_id && PLAYER_CARD_INSTANCE(current_player, current_card).damage_target_player == player && PLAYER_CARD_INSTANCE(current_player, current_card).damage_target_card == -1)
            {
              damage_creature(target_player, target_card, PLAYER_CARD_INSTANCE(current_player, current_card).info_slot,
                              (int)PLAYER_CARD_INSTANCE(current_player, current_card).damage_source_player, PLAYER_CARD_INSTANCE(current_player, current_card).damage_source_card);
              PLAYER_CARD_INSTANCE(player, card).eot_toughness += PLAYER_CARD_INSTANCE(current_player, current_card).info_slot;
              PLAYER_CARD_INSTANCE(current_player, current_card).info_slot = 0;
            }
          }
        }
      }

      PLAYER_CARD_INSTANCE(player, card).info_slot = 0;
      for (current_player = 0; current_player < 2; ++current_player)
      {
        for (current_card = 0; current_card < 150; ++current_card)
        {
          if (g_damage_accumulators[current_player][current_card][player].amount > 0)
          {
            damage_creature(target_player, target_card,
                            g_damage_accumulators[current_player][current_card][player].amount,
                            current_player, current_card);
            PLAYER_CARD_INSTANCE(player, card).info_slot +=
                g_damage_accumulators[current_player][current_card][player].amount;
            g_damage_accumulators[current_player][current_card][player].amount = 0;
          }
        }
      }

      g_life[player] += PLAYER_CARD_INSTANCE(player, card).info_slot;
      PLAYER_CARD_INSTANCE(player, card).info_slot = 0;
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

// FUNCTION: MAGIC 0x004f6d49
// FUNCTION: SHANDALAR 0x004b0e02
int card_shatter(int player, int card, event_t event)
{
  target_t target;

  if (event == EVENT_CAN_CAST)
  {
    load_recorded_action_target(0);
    return real_target_available((int *)0, TARGET_SCAN_DIRECT, player, 2, 2, TARGET_ZONE_IN_PLAY, TYPE_ARTIFACT, TYPE_NONE, 0, 0,
                                 COLOR_TEST_0, COLOR_TEST_0, -1, ~SUB_WALL, -1, -1, 0, 0, 0);
  }

  if (((event == EVENT_CAST_SPELL) && (g_affected_card == card)) && (g_affected_card_controller == player))
  {
    load_text("prompts.txt", "SHATTER");
    if (select_target_artifact_and_store(player, 1 - player, card))
    {
      g_ai_modifier -= 0x10;
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
                               0,
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

// FUNCTION: MAGIC 0x004f6f2a
// FUNCTION: SHANDALAR 0x004b0fe3
int card_disenchant(int player, int card, event_t event)
{
  target_t selected_target;

  if (event == EVENT_CAN_CAST)
  {
    load_recorded_action_target(0);
    return real_target_available((int *)0,
                                 TARGET_SCAN_DIRECT,
                                 player,
                                 2,
                                 2,
                                 TARGET_ZONE_IN_PLAY,
                                 TYPE_ARTIFACT | TYPE_ENCHANTMENT,
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
                                 0);
  }

  if (((event == EVENT_CAST_SPELL) && (g_affected_card == card)) && (g_affected_card_controller == player))
  {
    g_ai_modifier -= 0x10;
    load_text("prompts.txt", "DISENCHANT");
    if (C_real_select_target(player,
                             2,
                             2,
                             TARGET_ZONE_IN_PLAY,
                             TYPE_ARTIFACT | TYPE_ENCHANTMENT,
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
      SET_TARGET(PLAYER_CARD_INSTANCE(player, card).targets[0], selected_target);
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
    }
    else
    {
      g_spell_fizzled = 1;
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    SET_TARGET(selected_target, PLAYER_CARD_INSTANCE(player, card).targets[0]);
    if (C_real_validate_target(selected_target.player,
                               selected_target.card,
                               (char *)0,
                               player,
                               2,
                               2,
                               TARGET_ZONE_IN_PLAY,
                               TYPE_ARTIFACT | TYPE_ENCHANTMENT,
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
      kill_card(selected_target.player, selected_target.card, KILL_DESTROY);
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

// FUNCTION: MAGIC 0x004f71db
// FUNCTION: SHANDALAR 0x004b1292
int card_twiddle(int player, int card, event_t event)
{
  target_t target;
  char dialog[600];
  char tap_text[300];
  char untap_text[300];

  if (event == EVENT_CAN_CAST)
  {
    load_recorded_action_target(0);
    return real_target_available((int *)0,
                                 TARGET_SCAN_DIRECT,
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
                                 ~SUB_WALL,
                                 -1,
                                 -1,
                                 0,
                                 0,
                                 0);
  }

  if (event == 0x6c && g_affected_card == card && g_affected_card_controller == player)
  {
    g_ai_modifier -= 0x30;
    load_text("prompts.txt", "TWIDDLE");
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
      if ((g_land_can_be_played & LCBP_CARD_BEING_COPIED) != 0)
      {
        PLAYER_CARD_INSTANCE(player, card).info_slot =
            PLAYER_CARD_INSTANCE(g_current_spell_player, g_current_spell_card).info_slot;
      }
      else
      {
        load_text("prompts.txt", "TWIDDLE");
        strcpy(tap_text, g_text_lines[1]);
        strcpy(untap_text, g_text_lines[2]);
        sprintf(dialog, " %s\n %s", tap_text, untap_text);
        PLAYER_CARD_INSTANCE(player, card).info_slot =
            do_dialog(player, player, card, target.player, target.card, dialog,
                      (PLAYER_CARD_INSTANCE(target.player, target.card).state & STATE_TAPPED) >> 4);
      }
      if (player == g_other_player)
      {
        if ((global_cards_data[PLAYER_CARD_INSTANCE(target.player, target.card).internal_card_id].type & TYPE_LAND) != 0)
        {
          g_ai_modifier -= 0x18;
        }
        if ((PLAYER_CARD_INSTANCE(player, card).info_slot == 0 && target.player == g_other_player) ||
            (PLAYER_CARD_INSTANCE(player, card).info_slot == 1 && target.player == g_active_player))
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
                               TYPE_ARTIFACT | TYPE_CREATURE | TYPE_LAND,
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
      if (PLAYER_CARD_INSTANCE(player, card).info_slot != 0)
      {
        PLAYER_CARD_INSTANCE(target.player, target.card).state &= ~STATE_TAPPED;
      }
      else
      {
        tap_card_and_dispatch_event(target.player, target.card);
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

// FUNCTION: MAGIC 0x004f7783
// FUNCTION: SHANDALAR 0x004b183c
int tap_card_and_dispatch_event(int player, int card)
{
  if ((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0)
  {
    PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
    if ((global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].extra_ability & 0x1000) != 0)
    {
      g_produced_mana_color = -1;
    }
    dispatch_event(player, card, EVENT_TAP_CARD);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004f7857
// FUNCTION: SHANDALAR 0x004b1910
int card_tunnel(int player, int card, event_t event)
{
  target_t target;

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
                                 ~SUB_WALL,
                                 -1,
                                 -1,
                                 TARGET_SPECIAL_WALL,
                                 0,
                                 0);
  }

  if (((event == EVENT_CAST_SPELL) && (card == g_affected_card)) && (player == g_affected_card_controller))
  {
    load_text("prompts.txt", "TUNNEL");
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
      kill_card(target.player, target.card, KILL_BURY);
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

// FUNCTION: MAGIC 0x004f7b10
// FUNCTION: SHANDALAR 0x004b1bcb
int card_marsh_gas(int player, int card, event_t event)
{
  int current_card;
  int current_player;
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
            (global_cards_data[PLAYER_CARD_INSTANCE(current_player, current_card).internal_card_id].type &
             TYPE_CREATURE) != 0)
        {
          legacy_card = create_legacy_effect(player,
                                             card,
                                             g_duel_generated_internal_card_id_0c,
                                             current_player,
                                             current_card);
          if (legacy_card != -1)
          {
            PLAYER_CARD_INSTANCE(player, legacy_card).info_slot = -2;
            PLAYER_CARD_INSTANCE(player, legacy_card).eot_toughness = 0;
          }
        }
      }
    }
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004f7c84
// FUNCTION: SHANDALAR 0x004b1d3f
int card_howl_from_beyond(int player, int card, event_t event)
{
  struct
  {
    target_t target;
    int effect_card;
  } s;

  if (event == EVENT_CAN_CAST)
  {
    if (((player == g_other_player) && ((g_duel_network_flags & 2) == 0)) && has_mana(player, 7, 2) == 0)
    {
      return 0;
    }
    load_recorded_action_target(0);
    return real_target_available((int *)0,
                                 TARGET_SCAN_DIRECT,
                                 player,
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
                                 -1,
                                 -1,
                                 -1,
                                 0,
                                 0,
                                 0);
  }

  if (((event == EVENT_CAST_SPELL) && (card == g_affected_card)) && (player == g_affected_card_controller))
  {
    load_text("prompts.txt", "HOWL_FROM_BEYOND");
    if (select_target_creature_and_store(player, player, card))
    {
      g_ai_modifier -= (g_current_phase < PHASE_DECLARE_ATTACKERS ? 3 : 1) * 12;
      PLAYER_CARD_INSTANCE(player, card).info_slot = g_x_value;
      if ((player == g_other_player) &&
          (TARGET_CARD_INSTANCE(player, card, 0).state &
           STATE_SUMMONSICK_BOTH) != 0)
      {
        g_ai_modifier += -99;
      }
    }
    else
    {
      g_spell_fizzled = 1;
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    SET_TARGET(s.target, PLAYER_CARD_INSTANCE(player, card).targets[0]);
    if (C_real_validate_target(s.target.player,
                               s.target.card,
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
      s.effect_card = create_legacy_effect(player, card, LEGACY_EFFECT_PUMP, s.target.player, s.target.card);
      if (s.effect_card != -1)
      {
        PLAYER_CARD_INSTANCE(player, s.effect_card).counter_power =
            (short)PLAYER_CARD_INSTANCE(player, card).info_slot;
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

// FUNCTION: MAGIC 0x004f801d
// FUNCTION: SHANDALAR 0x004b20dc
int card_berserk(int player, int card, event_t event)
{
  int effect_card;

  if (event == EVENT_CAN_CAST)
  {
    load_recorded_action_target(0);
    if (g_current_phase < 0x1e && real_target_available((int *)0,
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
                                                        0) != 0)
    {
      return 1;
    }
    return 0;
  }

  if (((event == EVENT_CAST_SPELL) && (card == g_affected_card)) && (player == g_affected_card_controller))
  {
    load_text("prompts.txt", "BERSERK");
    if (!select_target_creature_and_store(player, player, card))
    {
      g_spell_fizzled = 1;
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
      effect_card =
          create_legacy_effect(player, card, g_duel_generated_internal_card_id_0f, PLAYER_CARD_INSTANCE(player, card).targets[0].player, PLAYER_CARD_INSTANCE(player, card).targets[0].card);
      if (effect_card != -1)
      {
        PLAYER_CARD_INSTANCE(player, effect_card).info_slot = 0x80;
        PLAYER_CARD_INSTANCE(player, effect_card).counter_power =
            TARGET_CARD_INSTANCE(player, card, 0).power;
        PLAYER_CARD_INSTANCE(player, effect_card).token_status |= 0x4000;
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

// FUNCTION: MAGIC 0x004f8305
// FUNCTION: SHANDALAR 0x004b23c6
int card_righteousness(int player, int card, event_t event)
{
  struct
  {
    target_t selected_target;
    int effect_card;
  } s;

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
                                 ~SUB_WALL,
                                 -1,
                                 -1,
                                 0,
                                 TARGET_STATE_BLOCKING,
                                 0);
  }

  if (((event == EVENT_CAST_SPELL) && (card == g_affected_card)) && (player == g_affected_card_controller))
  {
    load_text("prompts.txt", "RIGHTEOUSNESS");
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
                             0,
                             TARGET_STATE_BLOCKING,
                             0,
                             g_text_lines[0],
                             1,
                             &s.selected_target))
    {
      SET_TARGET(PLAYER_CARD_INSTANCE(player, card).targets[0], s.selected_target);
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
    }
    else
    {
      g_spell_fizzled = 1;
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    SET_TARGET(s.selected_target, PLAYER_CARD_INSTANCE(player, card).targets[0]);
    if (C_real_validate_target(s.selected_target.player,
                               s.selected_target.card,
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
                               TARGET_STATE_BLOCKING,
                               0))
    {
      s.effect_card = create_legacy_effect(player, card, LEGACY_EFFECT_PUMP,
                                           s.selected_target.player, s.selected_target.card);
      if (s.effect_card != -1)
      {
        PLAYER_CARD_INSTANCE(player, s.effect_card).counter_power = 7;
        PLAYER_CARD_INSTANCE(player, s.effect_card).counter_toughness = 7;
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

// FUNCTION: MAGIC 0x004f8639
// FUNCTION: SHANDALAR 0x004b26fc
int card_blood_lust(int player, int card, event_t event)
{
  int effect_card;

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
                                 ~SUB_WALL,
                                 -1,
                                 -1,
                                 0,
                                 0,
                                 0);
  }

  if (event == EVENT_CAST_SPELL && g_affected_card == card && g_affected_card_controller == player)
  {
    load_text("prompts.txt", "BLOODLUST");
    if (!select_target_creature_and_store(player, player, card))
    {
      g_spell_fizzled = 1;
    }
    else
    {
      g_ai_modifier += ((g_current_phase < PHASE_DECLARE_ATTACKERS) * 3 + 3) * -8;
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
      effect_card =
          create_legacy_effect(player, card, LEGACY_EFFECT_PUMP, PLAYER_CARD_INSTANCE(player, card).targets[0].player, PLAYER_CARD_INSTANCE(player, card).targets[0].card);
      if (effect_card != -1)
      {
        PLAYER_CARD_INSTANCE(player, effect_card).counter_power = 4;
        PLAYER_CARD_INSTANCE(player, effect_card).counter_toughness =
            -ClampIntToRange(4,
                             0,
                             TARGET_CARD_INSTANCE(player, card, 0).toughness -
                                 1);
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

// FUNCTION: MAGIC 0x004f8901
// FUNCTION: SHANDALAR 0x004b29c6
int card_fog(int player, int card, event_t event)
{
  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    create_legacy_effect(player, card, g_duel_generated_internal_card_id_0d, -1, -1);
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004f895d
// FUNCTION: SHANDALAR 0x004b2a22
int card_swords_to_plowshares(int player, int card, event_t event)
{
  int target_player;
  int target_card;

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
                                 ~SUB_WALL,
                                 -1,
                                 -1,
                                 0,
                                 0,
                                 0);
  }

  if (((event == EVENT_CAST_SPELL) && (card == g_affected_card)) && (player == g_affected_card_controller))
  {
    load_text("prompts.txt", "SWORD_TO_PLOWSHARES");
    if (!select_target_creature_and_store(player, 1 - player, card))
    {
      g_spell_fizzled = 1;
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
      gain_life(target_player, C_get_abilities(target_player, target_card, EVENT_POWER, -1));
      kill_card(target_player, target_card, KILL_REMOVE);
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

// FUNCTION: MAGIC 0x004f8b77
// FUNCTION: SHANDALAR 0x004b2c3e
int card_death_ward(int player, int card, event_t event)
{
  struct
  {
    int found_dead_creature;
    int current_player;
    int current_card;
    int internal_card_id;
    int target_player;
  } s;

  if (event == EVENT_CAN_CAST && (g_land_can_be_played & LCBP_REGENERATION) != 0)
  {
    s.found_dead_creature = 0;

    load_recorded_action_target(0);
    for (s.current_player = 0; s.current_player < 2; ++s.current_player)
    {
      for (s.current_card = 0; s.current_card < g_active_cards_count[s.current_player]; ++s.current_card)
      {
        if (s.found_dead_creature == 0)
        {
          s.internal_card_id = PLAYER_CARD_INSTANCE(s.current_player, s.current_card).internal_card_id;
          if (is_in_play(s.current_player, s.current_card) &&
              (global_cards_data[s.internal_card_id].type & TYPE_CREATURE) != 0 &&
              (int)(char)PLAYER_CARD_INSTANCE(s.current_player, s.current_card).kill_code == 2)
          {
            s.found_dead_creature = 1;
          }
        }
      }
    }

    if (s.found_dead_creature != 0)
    {
      return 99;
    }
    return 0;
  }

  if (event == EVENT_CAST_SPELL && card == g_affected_card && player == g_affected_card_controller && (g_land_can_be_played & LCBP_REGENERATION) != 0)
  {
    s.found_dead_creature = 0;
    do
    {
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
      load_text("prompts.txt", "DEATH_WARD");
      if (!select_target_creature_and_store_without_protection(player, -1, card))
      {
        g_spell_fizzled = 1;
      }

      s.target_player = PLAYER_CARD_INSTANCE(player, card).targets[0].player;
      s.current_card = PLAYER_CARD_INSTANCE(player, card).targets[0].card;
      if ((int)(char)PLAYER_CARD_INSTANCE(s.target_player, s.current_card).kill_code == 2)
      {
        if ((PLAYER_CARD_INSTANCE(s.target_player, s.current_card).regen_status & KEYWORD_REGENERATION) != 0 ||
            s.target_player == g_active_player)
        {
          g_ai_modifier -= 0x30;
        }
        s.found_dead_creature = 1;
      }
      else if (g_duel_ai_mode_state == 1)
      {
        rewind_recorded_action();
      }
      else
      {
        load_text("prompts.txt", "DEATH_WARD2");
        set_duel_prompt_text(g_text_lines[1]);
        Sleep(0x9c4);
        set_duel_prompt_text("");
      }
    } while ((g_spell_fizzled != 1) && s.found_dead_creature == 0);
  }

  if (event == EVENT_RESOLVE_SPELL && (g_land_can_be_played & LCBP_REGENERATION) != 0)
  {
    s.target_player = PLAYER_CARD_INSTANCE(player, card).targets[0].player;
    s.current_card = PLAYER_CARD_INSTANCE(player, card).targets[0].card;
    if (C_real_validate_target(s.target_player,
                               s.current_card,
                               (char *)0,
                               player,
                               2,
                               2,
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
                               0,
                               0))
    {
      regenerate_card(s.target_player, s.current_card);
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

// FUNCTION: MAGIC 0x004f9092
// FUNCTION: SHANDALAR 0x004b315b
int card_hurkyl_s_recall(int player, int card, event_t event)
{
  target_t selected_target;
  int current_card;
  int current_player;
  int owner_state;

  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if (event == EVENT_CAST_SPELL && g_affected_card == card && g_affected_card_controller == player)
  {
    if (g_other_player == player && (g_duel_network_flags & 2) == 0)
    {
      if (g_duel_ai_mode_state == 1)
      {
        g_ai_recorded_choice = internal_rand(3) == 0;
        if (g_ai_recorded_choice != 0)
        {
          if (has_permanent_of_type(1 - player, TYPE_ARTIFACT) == 0)
          {
            g_ai_recorded_choice = 0;
          }
          else if (has_permanent_of_type(player, TYPE_ARTIFACT) == 0)
          {
            g_ai_recorded_choice = 1;
          }
        }
        record_ai_action_selection();
      }
      else
      {
        replay_ai_action_selection();
      }

      if (g_ai_recorded_choice == 0)
      {
        PLAYER_CARD_INSTANCE(player, card).targets[0].player = 1 - player;
      }
      else
      {
        PLAYER_CARD_INSTANCE(player, card).targets[0].player = player;
      }
      PLAYER_CARD_INSTANCE(player, card).targets[0].card = -1;
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
    }
    else
    {
      load_text("prompts.txt", "HURKYLS_RECALL");
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
                               &selected_target))
      {
        SET_TARGET(PLAYER_CARD_INSTANCE(player, card).targets[0], selected_target);
        PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
      }
      else
      {
        g_spell_fizzled = 1;
      }
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    owner_state = PLAYER_CARD_INSTANCE(player, card).targets[0].player == 0 ? 0 : STATE_OWNED_BY_OPPONENT;
    for (current_player = 0; current_player < 2; ++current_player)
    {
      for (current_card = 0; current_card < g_active_cards_count[current_player]; ++current_card)
      {
        if (is_in_play(current_player, current_card) != 0 &&
            (global_cards_data[PLAYER_CARD_INSTANCE(current_player, current_card).internal_card_id].type & TYPE_ARTIFACT) != 0 &&
            (PLAYER_CARD_INSTANCE(current_player, current_card).state & STATE_OWNED_BY_OPPONENT) == owner_state)
        {
          hurkyls_recall_bounce_artifact(current_player, current_card);
        }
      }
    }
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: SHANDALAR 0x004ba41b
void hurkyls_recall_bounce_artifact(int target_player, int target_card)
{
  int new_card;
  int owner;

  load_text("prompts.txt", "PROMPT_FE_LEAVEPLAY");
  strcpy(gs_card_leaving_play_007aaef0, g_text_lines[0]);
  g_trigger_cause_controller = target_player;
  g_trigger_cause = target_card;
  dispatch_trigger_twice_once_with_each_player_as_reason(g_current_player, TRIGGER_LEAVE_PLAY, gs_card_leaving_play_007aaef0, 0);
  PLAYER_CARD_INSTANCE(target_player, target_card).internal_card_id = -1;
  clear_card_attachments(target_player, target_card);
  if ((PLAYER_CARD_INSTANCE(target_player, target_card).token_status & STATUS_TOKEN) == 0)
  {
    owner = (PLAYER_CARD_INSTANCE(target_player, target_card).state & STATE_OWNED_BY_OPPONENT) != 0;
    new_card = add_card_to_hand(owner, PLAYER_CARD_INSTANCE(target_player, target_card).original_internal_card_id);
    if (new_card != -1)
    {
      request_duel_display_refresh_if_human(owner, new_card, 8, 2);
    }
    ++g_duel_summary.hand_counts[owner];
  }
}

// FUNCTION: MAGIC 0x004f94f3
// FUNCTION: SHANDALAR 0x004b35bf
int card_jump(int player, int card, event_t event)
{
  int target_player;
  int target_card;
  int effect_card;

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
                                 ~SUB_WALL,
                                 -1,
                                 -1,
                                 0,
                                 0,
                                 0);
  }

  if (((event == EVENT_CAST_SPELL) && (card == g_affected_card)) && (player == g_affected_card_controller))
  {
    load_text("prompts.txt", "JUMP");
    if (!select_target_creature_and_store(player, player, card))
    {
      g_spell_fizzled = 1;
    }
    else
    {
      if (PLAYER_CARD_INSTANCE(player, card).targets[0].player == g_active_player)
      {
        g_ai_modifier -= 0x18;
      }
      if ((TARGET_CARD_INSTANCE(player, card, 0).regen_status & 0x20) != 0)
      {
        g_ai_modifier += -99;
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
      effect_card = create_legacy_effect(player, card, g_duel_generated_internal_card_id_03, target_player, target_card);
      if (effect_card != -1)
      {
        PLAYER_CARD_INSTANCE(player, effect_card).regen_status = 0;
        PLAYER_CARD_INSTANCE(player, effect_card).info_slot = 0x20;
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

// GLOBAL: SHANDALAR 0x005aa4e8
static int morale_source_player;

// GLOBAL: SHANDALAR 0x005aa4ec
static int morale_source_card;

static int __cdecl morale_pump_attacker(int target_player, int target_card, int unused);

// FUNCTION: MAGIC 0x004f9834
// FUNCTION: SHANDALAR 0x004b3902
int card_morale(int player, int card, event_t event)
{

  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    morale_source_player = player;
    morale_source_card = card;
    dispatch_three_arg_callback_to_cards_in_play(morale_pump_attacker, g_current_player);
    kill_card(player, card, KILL_BURY);
  }

  if (event == EVENT_CHECK_PUMP)
  {
    if (has_mana(player, COLOR_WHITE, 2) != 0 &&
        has_mana(player, COLOR_ANY, 3) != 0)
    {
      ++g_global_power_bonus[player];
      ++g_global_toughness_bonus[player];
    }
  }

  return 0;
}

// FUNCTION: SHANDALAR 0x004b39b5
static int __cdecl morale_pump_attacker(int target_player, int target_card, int unused)
{
  int effect_card;

  if ((PLAYER_CARD_INSTANCE(target_player, target_card).state & STATE_ATTACKING) != 0)
  {
    effect_card = create_legacy_effect(morale_source_player,
                                       morale_source_card,
                                       LEGACY_EFFECT_PUMP,
                                       target_player,
                                       target_card);
    if (effect_card != -1)
    {
      PLAYER_CARD_INSTANCE(morale_source_player, effect_card).counter_power = 1;
      PLAYER_CARD_INSTANCE(morale_source_player, effect_card).counter_toughness = 1;
    }
  }

  return 0;
}

// GLOBAL: SHANDALAR 0x005aa61c
static int piety_source_card;

// GLOBAL: SHANDALAR 0x005aa620
static int piety_source_player;

static int __cdecl piety_pump_blocker(int target_player, int target_card, int unused);

// FUNCTION: MAGIC 0x004f99c8
// FUNCTION: SHANDALAR 0x004b3a96
int card_piety(int player, int card, event_t event)
{
  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    piety_source_player = player;
    piety_source_card = card;
    dispatch_three_arg_callback_to_cards_in_play(piety_pump_blocker, 1 - g_current_player);
    kill_card(player, card, KILL_BURY);
  }

  if (event == EVENT_CHECK_PUMP)
  {
    if (has_mana(player, COLOR_WHITE, 1) != 0 &&
        has_mana(player, COLOR_ANY, 2) != 0)
    {
      g_global_toughness_bonus[player] += 3;
    }
  }

  return 0;
}

// FUNCTION: SHANDALAR 0x004b3b46
static int __cdecl piety_pump_blocker(int target_player, int target_card, int unused)
{
  int effect_card;

  if (PLAYER_CARD_INSTANCE(target_player, target_card).blocking != -1 &&
      (PLAYER_CARD_INSTANCE(target_player, target_card).state & STATE_ATTACKING) == 0)
  {
    effect_card = create_legacy_effect(piety_source_player,
                                       piety_source_card,
                                       LEGACY_EFFECT_PUMP,
                                       target_player,
                                       target_card);
    if (effect_card != -1)
    {
      PLAYER_CARD_INSTANCE(piety_source_player, effect_card).counter_power = 0;
      PLAYER_CARD_INSTANCE(piety_source_player, effect_card).counter_toughness = 3;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x004f9b91
// FUNCTION: SHANDALAR 0x004b3c5f
int card_terror(int player, int card, event_t event)
{
  target_t target;

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
                                 TYPE_ARTIFACT,
                                 0,
                                 get_protections_from(player, card),
                                 COLOR_TEST_0,
                                 1 << (unsigned char)get_sleighted_color(player, card, 1),
                                 -1,
                                 ~SUB_WALL,
                                 -1,
                                 -1,
                                 0,
                                 0,
                                 0);
  }

  if (((event == EVENT_CAST_SPELL) && (g_affected_card == card)) && (player == g_affected_card_controller))
  {
    load_text("prompts.txt", "TERROR");

    if (C_real_select_target(player,
                             2,
                             1 - player,
                             TARGET_ZONE_IN_PLAY,
                             TYPE_CREATURE,
                             TYPE_ARTIFACT,
                             0,
                             get_protections_from(player, card),
                             COLOR_TEST_0,
                             1 << (unsigned char)get_sleighted_color(player, card, 1),
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
                               TYPE_CREATURE,
                               TYPE_ARTIFACT,
                               0,
                               get_protections_from(player, card),
                               COLOR_TEST_0,
                               1 << (unsigned char)get_sleighted_color(player, card, 1),
                               -1,
                               ~SUB_WALL,
                               -1,
                               -1,
                               0,
                               0,
                               0))
    {
      kill_card(target.player, target.card, KILL_BURY);
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

// FUNCTION: MAGIC 0x004f9e9e
// FUNCTION: SHANDALAR 0x004b3f6b
int card_lightning_bolt(int player, int card, event_t event)
{
  if (event == EVENT_CAN_CAST)
  {
    load_recorded_action_target(1);
    return 1;
  }

  if (((event == EVENT_CAST_SPELL) && (card == g_affected_card)) && (player == g_affected_card_controller))
  {
    load_text("prompts.txt", "LIGHTNING_BOLT");
    if (select_damage_target(player, card, 3))
    {
      g_ai_modifier -= 0x24;
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    deal_damage_to_selected_target(player, card, event, 3);
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004f9f59
// FUNCTION: SHANDALAR 0x004b4028
int card_crumble(int player, int card, event_t event)
{
  target_t target;
  int internal_card_id;

  if (event == EVENT_CAN_CAST)
  {
    load_recorded_action_target(0);
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
                                 0);
  }

  if (event == EVENT_CAST_SPELL && g_affected_card == card && g_affected_card_controller == player)
  {
    load_text("prompts.txt", "CRUMBLE");
    if (!select_target_artifact_and_store(player, 1 - player, card))
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
      internal_card_id = TARGET_CARD_INSTANCE(player, card, 0).internal_card_id;
      gain_life(target.player,
                (int)(char)global_cards_data[internal_card_id].cc[0] +
                    ClampIntToRange((int)(char)global_cards_data[internal_card_id].cc[1], 0, 99),
                player,
                card);
      kill_card(target.player, target.card, KILL_BURY);
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

// FUNCTION: MAGIC 0x004fa213
// FUNCTION: SHANDALAR 0x004b42e2
int card_giant_growth(int player, int card, event_t event)
{
  struct
  {
    target_t target;
    int effect_card;
  } s;

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
                                 ~SUB_WALL,
                                 -1,
                                 -1,
                                 0,
                                 0,
                                 0);
  }

  if (((event == EVENT_CAST_SPELL) && (g_affected_card == card)) && (g_affected_card_controller == player))
  {
    g_ai_modifier -= (g_current_phase < PHASE_DECLARE_ATTACKERS ? 48 : 24);
    if (g_current_phase <= 0x14)
    {
      g_ai_modifier -= 0xc;
    }
    load_text("prompts.txt", "GIANT_GROWTH");
    if (!select_target_creature_and_store(player, player, card))
    {
      g_spell_fizzled = 1;
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    SET_TARGET(s.target, PLAYER_CARD_INSTANCE(player, card).targets[0]);
    if (C_real_validate_target(s.target.player,
                               s.target.card,
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
      s.effect_card =
          create_legacy_effect(player, card, LEGACY_EFFECT_PUMP, s.target.player, s.target.card);
      if (s.effect_card != -1)
      {
        PLAYER_CARD_INSTANCE(player, s.effect_card).counter_power = 3;
        PLAYER_CARD_INSTANCE(player, s.effect_card).counter_toughness = 3;
      }
    }
    else
    {
      g_spell_fizzled = 1;
    }
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    kill_card(player, card, KILL_BURY);
  }

  if ((event == 0x3b) && has_mana(player, 3, 1) != 0)
  {
    g_global_power_bonus[player] += 3;
    g_global_toughness_bonus[player] += 3;
  }

  return 0;
}

// FUNCTION: MAGIC 0x004fa4eb
// FUNCTION: SHANDALAR 0x004b45ba
int card_unsummon(int player, int card, event_t event)
{
  int target_player;
  int target_card;

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
                                 ~SUB_WALL,
                                 -1,
                                 -1,
                                 0,
                                 0,
                                 0);
  }

  if (((event == EVENT_CAST_SPELL) && (g_affected_card == card)) && (g_affected_card_controller == player))
  {
    g_ai_modifier -= 0x30;
    load_text("prompts.txt", "UNSUMMON");
    if (!select_target_creature_and_store(player, 1 - player, card))
    {
      g_spell_fizzled = 1;
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
      hurkyls_recall_bounce_artifact(target_player, target_card);
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

// FUNCTION: MAGIC 0x004fa77b
static int __cdecl sandstorm_damage_attacking_creature(int source_player,
                                                       int source_card,
                                                       int player,
                                                       int card,
                                                       int internal_card_id)
{
  (void)internal_card_id;

  if ((PLAYER_CARD_INSTANCE(player, card).state & STATE_ATTACKING) != 0)
  {
    damage_creature(player, card, 1, source_player, source_card);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004fa6e3
// FUNCTION: SHANDALAR 0x004b47b1
int card_sandstorm(int player, int card, event_t event)
{
  if (event == EVENT_CAN_CAST)
  {
    if (*(&player) == g_active_player)
    {
      return 1;
    }

    if ((g_duel_network_flags & 2) != 0)
    {
      return 1;
    }

    if (*(&player) == g_current_player)
    {
      return 0;
    }

    return 1;
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    dispatch_function_to_all_cards_in_play(player, card, sandstorm_damage_attacking_creature, -1);
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004fa7dc
// FUNCTION: SHANDALAR 0x004b48ac
int card_purelace(int player, int card, event_t event)
{
  target_t target;
  int color;
  int valid;

  if (event == EVENT_CAN_CAST)
  {
    if (g_current_spell_player == -1)
    {
      load_recorded_action_target(0);
      return real_target_available((int *)0, TARGET_SCAN_DIRECT, player, 2, 2, 0x200, 0xff, 0, 0,
                                   get_protections_from(player, card), 0, 0, -1, -1, 0xffffffff,
                                   0xffffffff, 0, 0, 0);
    }

    if ((((player == g_active_player) || ((g_duel_network_flags & 2) != 0)) || ((g_other_player == player) && (g_active_player == g_current_player))) && C_real_validate_target(g_current_spell_player, g_current_spell_card, (char *)0, player, 2, 2, 0,
                                                                                                                                                                                TYPE_EFFECT | TYPE_ARTIFACT | TYPE_INTERRUPT | TYPE_INSTANT | TYPE_SORCERY |
                                                                                                                                                                                    TYPE_ENCHANTMENT | TYPE_CREATURE | TYPE_LAND,
                                                                                                                                                                                TYPE_NONE, 0, 0, COLOR_TEST_0, COLOR_TEST_0, -1,
                                                                                                                                                                                -1, -1, -1, TARGET_SPECIAL_SPELL_ON_STACK, 0, 0))
    {
      return 99;
    }
    return 0;
  }

  if ((event == EVENT_CAST_SPELL) && (card == g_affected_card) && (player == g_affected_card_controller))
  {
    g_ai_modifier -= 0x18;
    if (g_current_spell_player == -1)
    {
      load_text("prompts.txt", "ANY_LACE");
      if (C_real_select_target(player, 2, 2, TARGET_ZONE_IN_PLAY,
                               TYPE_EFFECT | TYPE_ARTIFACT | TYPE_INTERRUPT | TYPE_INSTANT | TYPE_SORCERY |
                                   TYPE_ENCHANTMENT | TYPE_CREATURE | TYPE_LAND,
                               TYPE_NONE, 0, get_protections_from(player, card), COLOR_TEST_0,
                               COLOR_TEST_0, -1, -1, -1, -1, 0, 0, 0,
                               g_text_lines[0], 1, &target))
      {
        SET_TARGET(PLAYER_CARD_INSTANCE(player, card).targets[0], target);
        PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
        if ((g_other_player == player) && (((global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].color & PLAYER_CARD_INSTANCE(target.player, target.card).color) != 0) || (target.player == g_active_player)))
        {
          g_ai_modifier -= 0x30;
        }
      }
      else
      {
        g_spell_fizzled = 1;
      }
    }
    else
    {
      PLAYER_CARD_INSTANCE(player, card).targets[0].player = g_current_spell_player;
      PLAYER_CARD_INSTANCE(player, card).targets[0].card = g_current_spell_card;
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    SET_TARGET(target, PLAYER_CARD_INSTANCE(player, card).targets[0]);
    if (g_current_spell_player == -1)
    {
      valid = C_real_validate_target(target.player, target.card,
                                     (char *)0, player, 2, 2, TARGET_ZONE_IN_PLAY,
                                     TYPE_EFFECT | TYPE_ARTIFACT | TYPE_INTERRUPT | TYPE_INSTANT |
                                         TYPE_SORCERY | TYPE_ENCHANTMENT | TYPE_CREATURE | TYPE_LAND,
                                     TYPE_NONE, 0, get_protections_from(player, card), COLOR_TEST_0,
                                     COLOR_TEST_0, -1, -1, -1, -1, 0, 0, 0);
    }
    else
    {
      valid = C_real_validate_target(target.player, target.card,
                                     (char *)0, player, 2, 2, 0,
                                     TYPE_EFFECT | TYPE_ARTIFACT | TYPE_INTERRUPT | TYPE_INSTANT |
                                         TYPE_SORCERY | TYPE_ENCHANTMENT | TYPE_CREATURE | TYPE_LAND,
                                     TYPE_NONE, 0, 0, COLOR_TEST_0, COLOR_TEST_0, -1,
                                     -1, -1, -1, TARGET_SPECIAL_SPELL_ON_STACK, 0, 0);
    }

    if (!valid)
    {
      g_spell_fizzled = 1;
    }
    else
    {
      color = single_color_test_bit_to_color_t(global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].color);
      color = get_sleighted_color(player, card, color);
      PLAYER_CARD_INSTANCE(target.player, target.card).color = (char)(1 << ((unsigned char)color));
      PLAYER_CARD_INSTANCE(target.player, target.card).token_status |= STATUS_COLOR_FORCED;
      if (g_duel_ai_mode_state != 1)
      {
        play_sound_effect(WAV_CHANGEC);
      }
    }

    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004faee2
// FUNCTION: SHANDALAR 0x004b4fb3
void replace_card_hack_color(int player, int card, int color_from, unsigned char color_to)
{
  int current_color;

  for (current_color = 1; current_color < 6; ++current_color)
  {
    if (PLAYER_CARD_INSTANCE(player, card).hack_mode[current_color] == color_from)
    {
      PLAYER_CARD_INSTANCE(player, card).hack_mode[current_color] = (char)color_to;
    }
  }
  if (PLAYER_CARD_INSTANCE(player, card).hack_mode[color_from] == 0)
  {
    PLAYER_CARD_INSTANCE(player, card).hack_mode[color_from] = (char)color_to;
  }
}

// FUNCTION: MAGIC 0x004fafeb
// FUNCTION: SHANDALAR 0x004b50c0
int card_magical_hack(int player, int card, event_t event)
{
  extern card_ptr_t global_raw_cards_storage[2000];
  struct
  {
    unsigned int available_colors;
    int old_color;
    int new_color;
    target_t target;
    char prompt[200];
  } s;
  card_ptr_t *raw_card;
  int target_iid;

  if (event == EVENT_CAN_CAST)
  {
    if ((g_active_player == player) || (((unsigned char)g_duel_network_flags & 2) != 0))
    {
      if (g_current_spell_player == -1)
      {
        load_recorded_action_target(0);
        return 1;
      }
      return 99;
    }
    if ((g_current_spell_player == -1) || (g_active_player != g_current_player))
    {
      load_recorded_action_target(0);
      return 1;
    }
    return 99;
  }

  if (event == EVENT_CAST_SPELL && g_affected_card == card && g_affected_card_controller == player)
  {
    if (g_current_spell_player == -1)
    {
      load_text("prompts.txt", "MAGICAL_HACK");
      if (C_real_select_target(player,
                               2,
                               2,
                               TARGET_ZONE_IN_PLAY,
                               TYPE_ARTIFACT | TYPE_INTERRUPT | TYPE_INSTANT | TYPE_SORCERY | TYPE_ENCHANTMENT | TYPE_CREATURE | TYPE_LAND,
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
      }
      else
      {
        g_spell_fizzled = 1;
      }
    }
    else
    {
      PLAYER_CARD_INSTANCE(player, card).targets[0].player = g_current_spell_player;
      PLAYER_CARD_INSTANCE(player, card).targets[0].card = g_current_spell_card;
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
    }

    if (g_spell_fizzled != 1)
    {
      s.old_color = internal_rand(5) + 1;
      if ((player == g_other_player) && (((unsigned char)g_duel_network_flags & 2) == 0))
      {
        target_iid =
            TARGET_CARD_INSTANCE(player, card, 0).internal_card_id;
        raw_card = &global_raw_cards_storage[global_cards_data[target_iid].id];
        s.available_colors = *(unsigned int *)&raw_card->hack_colors;
        if (s.available_colors == 0)
        {
          g_spell_fizzled = 1;
        }
        else
        {
          if ((TARGET_CARD_INSTANCE(player, card, 0).token_status & STATUS_HACKED) != 0)
          {
            s.old_color = single_color_test_bit_to_color_t((unsigned char)s.available_colors);
            s.old_color = get_hacked_color(PLAYER_CARD_INSTANCE(player, card).targets[0].player, PLAYER_CARD_INSTANCE(player, card).targets[0].card, s.old_color);
            s.available_colors = 1 << ((unsigned char)s.old_color);
          }

          do
          {
            s.old_color = internal_rand(5) + 1;
          } while ((s.available_colors & (1 << ((unsigned char)s.old_color))) == 0);

          do
          {
            s.new_color = internal_rand(5) + 1;
          } while (s.new_color == s.old_color);

          if (g_duel_ai_mode_state == 1)
          {
            g_ai_recorded_choice = s.old_color;
            record_ai_action_selection();
            g_ai_recorded_choice = s.new_color;
            record_ai_action_selection();
          }
          else
          {
            replay_ai_action_selection();
            s.old_color = g_ai_recorded_choice;
            replay_ai_action_selection();
            s.new_color = g_ai_recorded_choice;
          }

          PLAYER_CARD_INSTANCE(player, card).info_slot = s.new_color * 0x100 + s.old_color;
          if (g_duel_ai_mode_state != 1)
          {
            load_text("prompts.txt", "MAGICAL_HACK");
            strcpy(s.prompt, "\n");
            sprintf(s.prompt + strlen(s.prompt),
                    g_text_lines[1],
                    gs_land_word_lower_00709310[s.old_color],
                    gs_land_word_upper_00709410[s.new_color]);
            do_dialog(player,
                      player,
                      card,
                      PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                      PLAYER_CARD_INSTANCE(player, card).targets[0].card,
                      s.prompt,
                      0);
          }
        }
      }
      else
      {
        s.available_colors = choose_magical_hack_colors(
            player,
            &PLAYER_CARD_INSTANCE(player, card).targets[0],
            get_displayed_card_name(player, card),
            (1 << (unsigned char)s.old_color) << 8,
            1);
        if (s.available_colors == 0xffffffff)
        {
          s.old_color = -1;
          g_spell_fizzled = 1;
        }
        else
        {
          s.new_color = single_color_test_bit_to_color_t((s.available_colors & 0xffff) >> 8);
          s.old_color = single_color_test_bit_to_color_t(s.available_colors & 0xff);
          PLAYER_CARD_INSTANCE(player, card).info_slot = s.new_color * 0x100 + s.old_color;
        }
      }
    }

    if (g_spell_fizzled == 1)
    {
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    SET_TARGET(s.target, PLAYER_CARD_INSTANCE(player, card).targets[0]);
    if (g_duel_ai_mode_state != 1)
    {
      play_sound_effect(WAV_CHANGET);
    }
    PLAYER_CARD_INSTANCE(s.target.player, s.target.card).token_status |= STATUS_HACKED;
    replace_card_hack_color(s.target.player,
                            s.target.card,
                            (unsigned char)PLAYER_CARD_INSTANCE(player, card).info_slot,
                            BYTE1(PLAYER_CARD_INSTANCE(player, card).info_slot));
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004fb8fd
// FUNCTION: SHANDALAR 0x004b59fb
void replace_card_sleight_color(int player, int card, int color_from, unsigned char color_to)
{
  int current_color;
  for (current_color = 1; current_color < 6; ++current_color)
  {
    if (PLAYER_CARD_INSTANCE(player, card).color_id[current_color] == color_from)
    {
      PLAYER_CARD_INSTANCE(player, card).color_id[current_color] = color_to;
    }
  }
  if (PLAYER_CARD_INSTANCE(player, card).color_id[color_from] == 0)
  {
    PLAYER_CARD_INSTANCE(player, card).color_id[color_from] = color_to;
  }
}

// FUNCTION: MAGIC 0x004fba06
// FUNCTION: SHANDALAR 0x004b5b0c
int card_sleight_of_mind(int player, int card, event_t event)
{
  extern card_ptr_t global_raw_cards_storage[2000];
  struct
  {
    target_t target;
    unsigned int available_colors;
    int initial_color;
    int old_color;
    int new_color;
    char prompt[200];
  } s;

  if (event == EVENT_CAN_CAST)
  {
    if ((g_active_player == player) || ((g_duel_network_flags & 2) != 0))
    {
      if (g_current_spell_player != -1)
      {
        return 99;
      }
      load_recorded_action_target(0);
      return 1;
    }
    if (g_current_spell_player != -1 && g_active_player == g_current_player)
    {
      return 99;
    }
    load_recorded_action_target(0);
    return 1;
  }

  if (event == EVENT_CAST_SPELL && g_affected_card == card && g_affected_card_controller == player)
  {
    if (g_current_spell_player == -1)
    {
      load_text("prompts.txt", "SLEIGHT_OF_MIND");
      if (C_real_select_target(player,
                               2,
                               2,
                               TARGET_ZONE_IN_PLAY,
                               TYPE_ARTIFACT | TYPE_INTERRUPT | TYPE_INSTANT | TYPE_SORCERY | TYPE_ENCHANTMENT | TYPE_CREATURE | TYPE_LAND,
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
        PLAYER_CARD_INSTANCE(player, card).targets[0] = s.target;
        PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
      }
      else
      {
        g_spell_fizzled = 1;
      }
    }
    else
    {
      PLAYER_CARD_INSTANCE(player, card).targets[0].player = g_current_spell_player;
      PLAYER_CARD_INSTANCE(player, card).targets[0].card = g_current_spell_card;
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
    }

    if (g_spell_fizzled != 1)
    {
      s.initial_color = internal_rand(5) + 1;
      if ((player == g_other_player && (g_duel_network_flags & 2) == 0) ||
          g_duel_ai_mode_state == 1 ||
          g_duel_network_state != 0)
      {
        s.available_colors =
            global_raw_cards_storage
                [global_cards_data
                     [TARGET_CARD_INSTANCE(player, card, 0).internal_card_id]
                         .id]
                    .sleight_color;
        if (s.available_colors == 0)
        {
          g_spell_fizzled = 1;
        }
        else
        {
          if ((TARGET_CARD_INSTANCE(player, card, 0).token_status &
               STATUS_SLEIGHTED) != 0)
          {
            s.old_color = single_color_test_bit_to_color_t((unsigned char)s.available_colors);
            s.old_color =
                get_sleighted_color(PLAYER_CARD_INSTANCE(player, card).targets[0].player, PLAYER_CARD_INSTANCE(player, card).targets[0].card, s.old_color);
            s.available_colors = 1 << ((unsigned char)s.old_color);
          }

          do
          {
            s.old_color = internal_rand(5) + 1;
          } while ((s.available_colors & (1 << ((unsigned char)s.old_color))) == 0);

          do
          {
            s.new_color = internal_rand(5) + 1;
          } while (s.new_color == s.old_color);

          if (g_duel_ai_mode_state == 1)
          {
            g_ai_recorded_choice = s.old_color;
            record_ai_action_selection();
            g_ai_recorded_choice = s.new_color;
            record_ai_action_selection();
          }
          else
          {
            replay_ai_action_selection();
            s.old_color = g_ai_recorded_choice;
            replay_ai_action_selection();
            s.new_color = g_ai_recorded_choice;
          }

          if (s.new_color == 0 || s.old_color == 0)
          {
            internal_rand(5);
          }

          PLAYER_CARD_INSTANCE(player, card).info_slot = s.new_color * 0x100 + s.old_color;
          if (g_duel_ai_mode_state != 1)
          {
            load_text("prompts.txt", "SLEIGHT_OF_MIND");
            strcpy(s.prompt, "\n");
            sprintf(s.prompt + strlen(s.prompt),
                    g_text_lines[1],
                    gs_color_word_lower_00709210[s.old_color],
                    gs_color_word_upper_00709190[s.new_color]);
            do_dialog(player,
                      player,
                      card,
                      PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                      PLAYER_CARD_INSTANCE(player, card).targets[0].card,
                      s.prompt,
                      0);
          }
        }
      }
      else
      {
        s.available_colors = choose_magical_hack_colors(
            player,
            &PLAYER_CARD_INSTANCE(player, card).targets[0],
            get_displayed_card_name(player, card),
            (1 << (unsigned char)s.initial_color) << 8,
            0);
        if (s.available_colors == 0xffffffff)
        {
          g_spell_fizzled = 1;
        }
        else
        {
          s.new_color = single_color_test_bit_to_color_t((s.available_colors & 0xffff) >> 8);
          s.old_color = single_color_test_bit_to_color_t(s.available_colors & 0xff);
          PLAYER_CARD_INSTANCE(player, card).info_slot = s.new_color * 0x100 + s.old_color;
        }
      }
    }

    if (g_spell_fizzled == 1)
    {
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    SET_TARGET(s.target, PLAYER_CARD_INSTANCE(player, card).targets[0]);
    if (g_duel_ai_mode_state != 1)
    {
      play_sound_effect(WAV_CHANGET);
    }
    PLAYER_CARD_INSTANCE(s.target.player, s.target.card).token_status |= STATUS_SLEIGHTED;
    replace_card_sleight_color(s.target.player,
                               s.target.card,
                               (unsigned char)PLAYER_CARD_INSTANCE(player, card).info_slot,
                               (unsigned char)((unsigned int)PLAYER_CARD_INSTANCE(player, card).info_slot >> 8));
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004fc353
// FUNCTION: SHANDALAR 0x004b647a
int card_blue_elemental_blast(int player, int card, event_t event)
{
  target_t target;
  int valid;

  if (event == EVENT_CAN_CAST)
  {
    if (g_current_spell_player == -1)
    {
      load_recorded_action_target(0);
      return real_target_available((int *)0, TARGET_SCAN_DIRECT, player, 2, 2, 0x200, 0x1047, 0, 0,
                                   get_protections_from(player, card), 1 << ((unsigned char)get_sleighted_color(player, card, 4)), 0, -1, -1,
                                   0xffffffff, 0xffffffff, 0, 0, 0);
    }
    return C_real_validate_target(g_current_spell_player, g_current_spell_card, (char *)0, player, 2, 2, 0,
                                  TYPE_NONE, TYPE_NONE, 0, 0, 1 << ((unsigned char)get_sleighted_color(player, card, 4)),
                                  COLOR_TEST_0, -1, ~SUB_WALL, -1, -1,
                                  TARGET_SPECIAL_SPELL_ON_STACK, 0, 0)
               ? 99
               : 0;
  }

  if ((event == EVENT_CAST_SPELL) && (card == g_affected_card) && (player == g_affected_card_controller))
  {
    if (g_current_spell_player == -1)
    {
      load_text("prompts.txt", "BLUE_ELEMENTAL_BLAST");
      if (C_real_select_target(player, 2, 1 - player, TARGET_ZONE_IN_PLAY,
                               TARGET_TYPE_TOKEN | TYPE_ARTIFACT | TYPE_ENCHANTMENT | TYPE_CREATURE | TYPE_LAND,
                               TYPE_NONE, 0, get_protections_from(player, card),
                               1 << ((unsigned char)get_sleighted_color(player, card, 4)), COLOR_TEST_0, -1, -1, -1, -1,
                               0, 0, 0, g_text_lines[0], 1, &target))
      {
        SET_TARGET(PLAYER_CARD_INSTANCE(player, card).targets[0], target);
        PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
      }
      else
      {
        g_spell_fizzled = 1;
      }
    }
    else
    {
      PLAYER_CARD_INSTANCE(player, card).targets[0].player = g_current_spell_player;
      PLAYER_CARD_INSTANCE(player, card).targets[0].card = g_current_spell_card;
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    valid = 0;
    if (g_current_spell_player == -1)
    {
      valid = C_real_validate_target(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                                     PLAYER_CARD_INSTANCE(player, card).targets[0].card,
                                     (char *)0, player, 2, 2, TARGET_ZONE_IN_PLAY, TYPE_NONE,
                                     TYPE_NONE, 0, get_protections_from(player, card),
                                     1 << ((unsigned char)get_sleighted_color(player, card, 4)), COLOR_TEST_0, -1, -1,
                                     -1, -1, 0, 0, 0);
    }
    else
    {
      valid = C_real_validate_target(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                                     PLAYER_CARD_INSTANCE(player, card).targets[0].card,
                                     (char *)0, player, 2, 2, 0, TYPE_NONE, TYPE_NONE, 0, 0,
                                     1 << ((unsigned char)get_sleighted_color(player, card, 4)), COLOR_TEST_0, -1, -1,
                                     -1, -1, TARGET_SPECIAL_SPELL_ON_STACK, 0, 0);
    }

    if (valid)
    {
      if ((TARGET_CARD_INSTANCE(player, card, 0).color &
           (1 << ((unsigned char)get_sleighted_color(player, card, 4)))) != 0)
      {
        kill_card(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                  PLAYER_CARD_INSTANCE(player, card).targets[0].card, KILL_DESTROY);
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

// FUNCTION: MAGIC 0x004fc90c
// FUNCTION: SHANDALAR 0x004b6a32
int card_counterspell(int player, int card, event_t event)
{
  int valid;

  if (event == EVENT_CAN_CAST)
  {
    load_recorded_action_target(0);
    if (g_current_spell_player == -1)
    {
      return 0;
    }
    return C_real_validate_target(g_current_spell_player, g_current_spell_card, (char *)0, player, 2, 2, 0,
                                  TYPE_NONE, TYPE_NONE, 0, 0, COLOR_TEST_0, COLOR_TEST_0,
                                  -1, ~SUB_WALL, -1, -1, TARGET_SPECIAL_SPELL_ON_STACK, 0, 0)
               ? 99
               : 0;
  }

  if ((event == EVENT_CAST_SPELL) && (card == g_affected_card) && (player == g_affected_card_controller))
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

  if (event == EVENT_CAN_COUNTER && g_special_mana_pool[player][COLOR_BLUE] > 1)
  {
    g_ai_modifier += 0x18;
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    valid = C_real_validate_target(PLAYER_CARD_INSTANCE(player, card).targets[0].player, PLAYER_CARD_INSTANCE(player, card).targets[0].card,
                                   (char *)0, player, 2, 2, 0, TYPE_NONE, TYPE_NONE, 0, 0,
                                   COLOR_TEST_0, COLOR_TEST_0, -1, ~SUB_WALL, -1, -1,
                                   TARGET_SPECIAL_SPELL_ON_STACK, 0, 0);
    if (valid)
    {
      if ((TARGET_CARD_INSTANCE(player, card, 0).state & 0x20) != 0)
      {
        kill_card(PLAYER_CARD_INSTANCE(player, card).targets[0].player, PLAYER_CARD_INSTANCE(player, card).targets[0].card, KILL_BURY);
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

// FUNCTION: MAGIC 0x004fcc3f
// FUNCTION: SHANDALAR 0x004b6d67
int card_power_sink(int player, int card, event_t event)
{
  struct
  {
    int target_player;
    int target_card;
    int mana_paid;
    int color;
    int current_card;
  } s;

  if (event == EVENT_CAN_CAST)
  {
    load_recorded_action_target(0);
    if (g_current_spell_player == -1)
    {
      return 0;
    }
    if (player == g_other_player && (g_duel_network_flags & 2) == 0 && has_mana(player, COLOR_ANY, 2) == 0)
    {
      return 0;
    }
    if (C_real_validate_target(g_current_spell_player,
                               g_current_spell_card,
                               (char *)0,
                               player,
                               2,
                               2,
                               0,
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
                               TARGET_SPECIAL_SPELL_ON_STACK,
                               0,
                               0))
    {
      return 99;
    }
    return 0;
  }

  if (event == EVENT_CAST_SPELL && card == g_affected_card && player == g_affected_card_controller)
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
      PLAYER_CARD_INSTANCE(player, card).info_slot = g_x_value;
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
                               0,
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
                               TARGET_SPECIAL_SPELL_ON_STACK,
                               0,
                               0))
    {
      s.target_player = PLAYER_CARD_INSTANCE(player, card).targets[0].player;
      s.target_card = PLAYER_CARD_INSTANCE(player, card).targets[0].card;

      if (s.target_player == g_active_player || (g_duel_network_flags & 2) != 0)
      {
        push_card_onto_stack(player, card, 0x7e, 0, 0);
        s.mana_paid = charge_mana(s.target_player, COLOR_COLORLESS, PLAYER_CARD_INSTANCE(player, card).info_slot);
        obliterate_top_card_of_stack();
        g_spell_fizzled = 0;
      }
      else if (has_mana(s.target_player, COLOR_ANY, 1) == 0)
      {
        s.mana_paid = 0;
      }
      else
      {
        s.mana_paid = charge_mana(s.target_player, COLOR_COLORLESS, PLAYER_CARD_INSTANCE(player, card).info_slot);
      }

      if (s.mana_paid < PLAYER_CARD_INSTANCE(player, card).info_slot)
      {
        for (s.color = 0; s.color < 7 && s.mana_paid < PLAYER_CARD_INSTANCE(player, card).info_slot; ++s.color)
        {
          while (g_raw_mana_available[s.target_player][s.color] > 0 &&
                 s.mana_paid < PLAYER_CARD_INSTANCE(player, card).info_slot)
          {
            --g_raw_mana_available[s.target_player][s.color];
            --g_raw_mana_available[s.target_player][7];
            if (s.color != 6)
            {
              ++s.mana_paid;
            }
          }
        }

        g_mana_charge[0] = PLAYER_CARD_INSTANCE(player, card).info_slot;
        for (s.current_card = 0;
             s.current_card < g_active_cards_count[s.target_player] &&
             s.mana_paid < PLAYER_CARD_INSTANCE(player, card).info_slot;
             ++s.current_card)
        {
          if (is_in_play(s.target_player, s.current_card))
          {
            if ((global_cards_data[PLAYER_CARD_INSTANCE(s.target_player, s.current_card).internal_card_id].type & TYPE_LAND) != 0 &&
                (global_cards_data[PLAYER_CARD_INSTANCE(s.target_player, s.current_card).internal_card_id].extra_ability & 0x1000) != 0 &&
                (PLAYER_CARD_INSTANCE(s.target_player, s.current_card).state & STATE_TAPPED) == 0 &&
                (((PLAYER_CARD_INSTANCE(s.target_player, s.current_card).state & STATE_SUMMONSICK_BOTH) == 0) ||
                 (global_cards_data[PLAYER_CARD_INSTANCE(s.target_player, s.current_card).internal_card_id].type & TYPE_CREATURE) == 0))
            {
              activate_mana_source_for_payment(s.target_player, s.current_card);

              for (s.color = 0; s.color < 7 && s.mana_paid < PLAYER_CARD_INSTANCE(player, card).info_slot; ++s.color)
              {
                while (g_raw_mana_available[s.target_player][s.color] > 0 &&
                       s.mana_paid < PLAYER_CARD_INSTANCE(player, card).info_slot)
                {
                  --g_raw_mana_available[s.target_player][s.color];
                  --g_raw_mana_available[s.target_player][7];
                  if (s.color != 6)
                  {
                    ++s.mana_paid;
                  }
                }
              }
            }
          }
        }
        g_mana_charge[0] = 0;
      }

      if (s.mana_paid < PLAYER_CARD_INSTANCE(player, card).info_slot)
      {
        kill_card(s.target_player, s.target_card, KILL_BURY);
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

// FUNCTION: MAGIC 0x004fd465
// FUNCTION: SHANDALAR 0x004b7590
int card_spell_blast(int player, int card, event_t event)
{
  int cost;

  if (event == EVENT_CAN_CAST)
  {
    load_recorded_action_target(0);
    if (g_current_spell_player == -1)
    {
      return 0;
    }

    if ((g_other_player == player) && ((g_duel_network_flags & 2) == 0) && !has_mana_w_global_cost_mod(player, card, 7, 2))
    {
      return 0;
    }

    cost = (int)(char)global_cards_data[PLAYER_CARD_INSTANCE(g_current_spell_player, g_current_spell_card).internal_card_id].cc[1];
    if (cost == -1)
    {
      cost = g_x_value;
    }
    cost += (int)(char)global_cards_data[PLAYER_CARD_INSTANCE(g_current_spell_player, g_current_spell_card).internal_card_id].cc[0];
    PLAYER_CARD_INSTANCE(player, card).info_slot = cost;

    if (has_mana(player, 2, 1) && has_mana_w_global_cost_mod(player, card, 7, cost + 1) && C_real_validate_target(g_current_spell_player, g_current_spell_card, (char *)0, player, 2, 2, 0, TYPE_NONE, TYPE_NONE, 0, 0, COLOR_TEST_0, COLOR_TEST_0, -1, ~SUB_WALL, -1, -1, TARGET_SPECIAL_SPELL_ON_STACK, 0, 0))
    {
      g_adventure_world_state = 1;
      return 99;
    }
    return 0;
  }

  if ((event == EVENT_CAST_SPELL) && (card == g_affected_card) && (player == g_affected_card_controller) && (g_current_spell_player != -1))
  {
    g_mana_charge[COLOR_BLUE] = 1;
    charge_mana_w_global_cost_mod(player, card, 0, PLAYER_CARD_INSTANCE(player, card).info_slot);
    if (g_spell_fizzled != 1)
    {
      PLAYER_CARD_INSTANCE(player, card).targets[0].player = g_current_spell_player;
      PLAYER_CARD_INSTANCE(player, card).targets[0].card = g_current_spell_card;
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
      g_x_value = PLAYER_CARD_INSTANCE(player, card).info_slot;
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    if (C_real_validate_target(PLAYER_CARD_INSTANCE(player, card).targets[0].player, PLAYER_CARD_INSTANCE(player, card).targets[0].card, (char *)0,
                               player, 2, 2, 0, TYPE_NONE, TYPE_NONE, 0, 0, COLOR_TEST_0,
                               COLOR_TEST_0, -1, ~SUB_WALL, -1, -1,
                               TARGET_SPECIAL_SPELL_ON_STACK, 0, 0))
    {
      if ((TARGET_CARD_INSTANCE(player, card, 0).state & 0x20) != 0)
      {
        kill_card(PLAYER_CARD_INSTANCE(player, card).targets[0].player, PLAYER_CARD_INSTANCE(player, card).targets[0].card, KILL_DESTROY);
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

// FUNCTION: MAGIC 0x004fd933
// FUNCTION: SHANDALAR 0x004b7a5b
int card_red_elemental_blast(int player, int card, event_t event)
{
  target_t target;
  int valid;

  if (event == EVENT_CAN_CAST)
  {
    if (g_current_spell_player == -1)
    {
      load_recorded_action_target(0);
      return real_target_available((int *)0, TARGET_SCAN_DIRECT, player, 2, 2, 0x200, 0x1047, 0, 0,
                                   get_protections_from(player, card), 1 << ((unsigned char)get_sleighted_color(player, card, 2)), 0, -1, -1,
                                   0xffffffff, 0xffffffff, 0, 0, 0);
    }
    return C_real_validate_target(g_current_spell_player, g_current_spell_card, (char *)0, player, 2, 2, 0,
                                  TYPE_NONE, TYPE_NONE, 0, 0, 1 << ((unsigned char)get_sleighted_color(player, card, 2)),
                                  COLOR_TEST_0, -1, ~SUB_WALL, -1, -1,
                                  TARGET_SPECIAL_SPELL_ON_STACK, 0, 0)
               ? 99
               : 0;
  }

  if ((event == EVENT_CAST_SPELL) && (card == g_card_on_stack) && (player == g_card_on_stack_controller))
  {
    if (g_current_spell_player == -1)
    {
      load_text("prompts.txt", "RED_ELEMENTAL_BLAST");
      if (C_real_select_target(player, 2, 1 - player, TARGET_ZONE_IN_PLAY,
                               TARGET_TYPE_TOKEN | TYPE_ARTIFACT | TYPE_ENCHANTMENT | TYPE_CREATURE | TYPE_LAND,
                               TYPE_NONE, 0, get_protections_from(player, card),
                               1 << ((unsigned char)get_sleighted_color(player, card, 2)), COLOR_TEST_0, -1, -1, -1, -1,
                               0, 0, 0, g_text_lines[0], 1, &target))
      {
        SET_TARGET(PLAYER_CARD_INSTANCE(player, card).targets[0], target);
        PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
      }
      else
      {
        g_spell_fizzled = 1;
      }
    }
    else
    {
      PLAYER_CARD_INSTANCE(player, card).targets[0].player = g_current_spell_player;
      PLAYER_CARD_INSTANCE(player, card).targets[0].card = g_current_spell_card;
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    valid = 0;
    if (g_current_spell_player == -1)
    {
      valid = C_real_validate_target(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                                     PLAYER_CARD_INSTANCE(player, card).targets[0].card,
                                     (char *)0, player, 2, 2, TARGET_ZONE_IN_PLAY, TYPE_NONE,
                                     TYPE_NONE, 0, get_protections_from(player, card),
                                     1 << ((unsigned char)get_sleighted_color(player, card, 2)), COLOR_TEST_0, -1, -1,
                                     -1, -1, 0, 0, 0);
    }
    else
    {
      valid = C_real_validate_target(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                                     PLAYER_CARD_INSTANCE(player, card).targets[0].card,
                                     (char *)0, player, 2, 2, 0, TYPE_NONE, TYPE_NONE, 0, 0,
                                     1 << ((unsigned char)get_sleighted_color(player, card, 2)), COLOR_TEST_0, -1, -1,
                                     -1, -1, TARGET_SPECIAL_SPELL_ON_STACK, 0, 0);
    }

    if (valid)
    {
      if ((TARGET_CARD_INSTANCE(player, card, 0).color &
           (1 << ((unsigned char)get_sleighted_color(player, card, 2)))) != 0)
      {
        kill_card(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                  PLAYER_CARD_INSTANCE(player, card).targets[0].card, KILL_DESTROY);
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

// FUNCTION: MAGIC 0x004fdeec
// FUNCTION: SHANDALAR 0x004b8014
int card_dark_ritual(int player, int card, event_t event)
{
  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if ((event == EVENT_CAST_SPELL) && (card == g_affected_card) && (player == g_affected_card_controller))
  {
    if (g_duel_summary.hand_counts[player] < 8)
    {
      g_ai_modifier -= 0x3c;
    }
    else
    {
      g_ai_modifier -= 0x18;
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    produce_mana(player, COLOR_BLACK, 3);
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004fdf88
// FUNCTION: SHANDALAR 0x004b80b2
int card_alabaster_potion(int player, int card, event_t event)
{
  return gain_life_or_prevent_damage(player, card, event, g_x_value);
}

// FUNCTION: MAGIC 0x004fdfb2
// FUNCTION: SHANDALAR 0x004b80dc
int card_healing_salve(int player, int card, event_t event)
{
  return gain_life_or_prevent_damage(player, card, event, 3);
}

// FUNCTION: MAGIC 0x004fdfd8
// FUNCTION: SHANDALAR 0x004b8102
int gain_life_or_prevent_damage(int player, int card, event_t event, int amount)
{
  target_t selected_target;
  int done;
  int cancelled;
  int target_index;
  int source_player;
  int source_card;
  char prompt[300];

  if (event == EVENT_CAN_CAST)
  {
    if (((player == g_other_player) && ((g_duel_network_flags & 2) == 0)) && (amount == 0))
    {
      return 0;
    }
    if ((g_land_can_be_played & LCBP_DAMAGE_PREVENTION) == 0)
    {
      return 1;
    }
    return 99;
  }

  if (((event == EVENT_CAST_SPELL) && (g_affected_card == card)) && (g_affected_card_controller == player))
  {
    g_ai_modifier -= 0x60;
    if ((g_land_can_be_played & LCBP_DAMAGE_PREVENTION) == 0)
    {
      load_text("prompts.txt", "HEALING_SALVE");
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
                               &selected_target))
      {
        SET_TARGET(PLAYER_CARD_INSTANCE(player, card).targets[0], selected_target);
        PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
        PLAYER_CARD_INSTANCE(player, card).info_slot = amount;
      }
      else
      {
        g_spell_fizzled = 1;
      }
    }
    else
    {
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
      source_player = -1;
      source_card = 0;
      cancelled = 0;
      done = 0;
      g_spell_fizzled = -1;
      while ((PLAYER_CARD_INSTANCE(player, card).number_of_targets < amount) &&
             !cancelled &&
             (g_spell_fizzled != 1) &&
             !done)
      {
        load_text("prompts.txt", "HEALING_SALVE");
        sprintf(prompt, g_text_lines[1], PLAYER_CARD_INSTANCE(player, card).number_of_targets + 1, amount);
        if (C_real_select_target(player,
                                 2,
                                 player,
                                 TARGET_ZONE_IN_PLAY,
                                 TYPE_NONE,
                                 TYPE_NONE,
                                 0,
                                 0,
                                 COLOR_TEST_0,
                                 COLOR_TEST_0,
                                 g_damage_card_internal_card_id,
                                 ~SUB_WALL,
                                 -1,
                                 -1,
                                 0,
                                 0,
                                 0,
                                 prompt,
                                 3,
                                 &selected_target))
        {
          if (((PLAYER_CARD_INSTANCE(selected_target.player, selected_target.card).damage_source_player == source_player) && (PLAYER_CARD_INSTANCE(selected_target.player, selected_target.card).damage_source_card == source_card)) || source_player == -1)
          {
            source_player = PLAYER_CARD_INSTANCE(selected_target.player, selected_target.card).damage_source_player;
            source_card = PLAYER_CARD_INSTANCE(selected_target.player, selected_target.card).damage_source_card;
            PLAYER_CARD_INSTANCE(selected_target.player, selected_target.card).state |= 0x200000;
            TENTATIVE_reassess_all_cards(0, 0x20);
            SET_TARGET(PLAYER_CARD_INSTANCE(player, card).targets[PLAYER_CARD_INSTANCE(player, card).number_of_targets], selected_target);
            ++PLAYER_CARD_INSTANCE(player, card).number_of_targets;
            if (PLAYER_CARD_INSTANCE(player, card).number_of_targets == 19)
            {
              done = 1;
            }
            if (g_recorded_action_player == 1)
            {
              while ((PLAYER_CARD_INSTANCE(player, card).number_of_targets < amount) && !done)
              {
                SET_TARGET(PLAYER_CARD_INSTANCE(player, card).targets[PLAYER_CARD_INSTANCE(player, card).number_of_targets], selected_target);
                ++PLAYER_CARD_INSTANCE(player, card).number_of_targets;
                if (PLAYER_CARD_INSTANCE(player, card).number_of_targets == 19)
                {
                  done = 1;
                }
              }
            }
          }
          else if (g_duel_ai_mode_state == 1)
          {
            rewind_recorded_action();
          }
          else
          {
            load_text("prompts.txt", "HEALING_SALVE");
            set_duel_prompt_text(g_text_lines[2]);
            Sleep(0x9c4);
            set_duel_prompt_text("");
          }
        }
        else
        {
          if (selected_target.card == -1)
          {
            g_spell_fizzled = 1;
          }
          else
          {
            cancelled = 1;
          }
        }
      }
      for (target_index = 0;
           target_index < PLAYER_CARD_INSTANCE(player, card).number_of_targets;
           ++target_index)
      {
        TARGET_CARD_INSTANCE(player, card, target_index).state &= ~0x300000;
      }
      if (done)
      {
        done = 0;
        if (g_duel_ai_mode_state != 1)
        {
          show_duel_system_error("Target array overflow in HealingPotion()");
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
    while (PLAYER_CARD_INSTANCE(player, card).number_of_targets > 0)
    {
      --PLAYER_CARD_INSTANCE(player, card).number_of_targets;
      SET_TARGET(selected_target, PLAYER_CARD_INSTANCE(player, card).targets[PLAYER_CARD_INSTANCE(player, card).number_of_targets]);
      if ((g_land_can_be_played & LCBP_DAMAGE_PREVENTION) == 0)
      {
        gain_life(selected_target.player, PLAYER_CARD_INSTANCE(player, card).info_slot);
      }
      else if (C_real_validate_target(selected_target.player,
                                      selected_target.card,
                                      (char *)0,
                                      player,
                                      2,
                                      2,
                                      TARGET_ZONE_IN_PLAY,
                                      TYPE_NONE,
                                      TYPE_NONE,
                                      0,
                                      0,
                                      COLOR_TEST_0,
                                      COLOR_TEST_0,
                                      g_damage_card_internal_card_id,
                                      ~SUB_WALL,
                                      -1,
                                      -1,
                                      0,
                                      0,
                                      0))
      {
        if (PLAYER_CARD_INSTANCE(selected_target.player, selected_target.card).info_slot != 0)
        {
          --PLAYER_CARD_INSTANCE(selected_target.player, selected_target.card).info_slot;
        }
      }
      else
      {
        g_spell_fizzled = 1;
      }
    }
    kill_card(player, card, KILL_DESTROY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004feb03
// FUNCTION: SHANDALAR 0x004b8c28
int card_reverse_damage(int player, int card, event_t event)
{
  struct
  {
    target_t target;
    int best_damage;
    int selection_index;
    int damage_amounts[150];
    int count;
    int controller;
    int selected;
    int damage_sources[300];
    int internal_card_ids[150];
    int current_card;
  } s;

  if (event == EVENT_CAN_CAST)
  {
    load_recorded_action_target(0);
    if ((g_land_can_be_played & LCBP_DAMAGE_PREVENTION) == 0)
    {
      return 1;
    }
    if (real_target_available((int *)0,
                              TARGET_SCAN_DIRECT,
                              player,
                              2,
                              2,
                              TARGET_ZONE_IN_PLAY,
                              TYPE_NONE,
                              TYPE_NONE,
                              0,
                              0,
                              0,
                              0,
                              g_damage_card_internal_card_id,
                              -1,
                              0xffffffff,
                              0xffffffff,
                              0x20,
                              0,
                              0) != 0)
    {
      return 99;
    }
    return 0;
  }
  else
  {
    if ((((event == EVENT_CAST_SPELL) && (card == g_affected_card)) && (player == g_affected_card_controller)) && ((g_land_can_be_played & LCBP_DAMAGE_PREVENTION) != 0))
    {
      load_text("prompts.txt", "REVERSE_DAMAGE");
      if (C_real_select_target(player,
                               2,
                               2,
                               TARGET_ZONE_IN_PLAY,
                               TYPE_NONE,
                               TYPE_NONE,
                               0,
                               0,
                               COLOR_TEST_0,
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
                               &s.target))
      {
        SET_TARGET(PLAYER_CARD_INSTANCE(player, card).targets[0], s.target);
        PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
        *(unsigned int *)&PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
      }
      else
      {
        g_spell_fizzled = 1;
      }
    }

    if (event == EVENT_RESOLVE_SPELL)
    {
      if ((g_land_can_be_played & LCBP_DAMAGE_PREVENTION) == 0)
      {
        s.count = 0;
        s.best_damage = 0;
        for (s.controller = 0; s.controller < 2; s.controller = s.controller + 1)
        {
          for (s.current_card = 0; s.current_card < 150; s.current_card = s.current_card + 1)
          {
            if (0 < g_damage_accumulators[s.controller][s.current_card][player].amount)
            {
              s.damage_sources[s.count * 2] = s.controller;
              s.damage_sources[s.count * 2 + 1] = s.current_card;
              s.damage_amounts[s.count] = g_damage_accumulators[s.controller][s.current_card][player].amount;
              if (PLAYER_CARD_INSTANCE(s.controller, s.current_card).internal_card_id == -1)
              {
                s.internal_card_ids[s.count] = PLAYER_CARD_INSTANCE(s.controller, s.current_card).original_internal_card_id;
                s.count = s.count + 1;
              }
              else
              {
                s.internal_card_ids[s.count] = PLAYER_CARD_INSTANCE(s.controller, s.current_card).internal_card_id;
                s.count = s.count + 1;
              }
            }
          }
        }
        if (0 < s.count)
        {
          if (((player == g_other_player) && ((g_duel_network_flags & 2) == 0)) || (g_duel_ai_mode_state == 1))
          {
            s.best_damage = 0;
            for (s.selection_index = 0; s.selection_index < s.count; s.selection_index = s.selection_index + 1)
            {
              if (s.best_damage < s.damage_amounts[s.selection_index])
              {
                s.best_damage = s.selection_index;
              }
            }
            s.selected = s.best_damage;
          }
          else
          {
            load_text("prompts.txt", "REVERSE_DAMAGE");
            s.selected = select_damage_card_from_list(player,
                                                      s.internal_card_ids,
                                                      s.damage_amounts,
                                                      s.count,
                                                      (int)g_text_lines[1],
                                                      1,
                                                      "");
          }
          gain_life(player,
                    g_damage_accumulators[s.damage_sources[s.selected * 2]]
                                         [s.damage_sources[s.selected * 2 + 1]]
                                         [player]
                                             .amount *
                        2);
          g_damage_accumulators[s.damage_sources[s.selected * 2]][s.damage_sources[s.selected * 2 + 1]][player].amount = 0;
        }
        if (g_life[1 - player] < 1)
        {
          g_ai_modifier += 1000;
        }
        else
        {
          g_ai_modifier += (s.best_damage * 100) / g_life[1 - player] - 100;
        }
      }
      else
      {
        SET_TARGET(s.target, PLAYER_CARD_INSTANCE(player, card).targets[0]);
        if (C_real_validate_target(s.target.player,
                                   s.target.card,
                                   (char *)0,
                                   player,
                                   2,
                                   2,
                                   TARGET_ZONE_IN_PLAY,
                                   TYPE_NONE,
                                   TYPE_NONE,
                                   0,
                                   0,
                                   COLOR_TEST_0,
                                   COLOR_TEST_0,
                                   g_damage_card_internal_card_id,
                                   ~SUB_WALL,
                                   -1,
                                   -1,
                                   TARGET_SPECIAL_DAMAGE_PLAYER,
                                   0,
                                   0))
        {
          if (PLAYER_CARD_INSTANCE(s.target.player, s.target.card).info_slot != 0)
          {
            gain_life(player,
                      g_damage_accumulators[(char)PLAYER_CARD_INSTANCE(s.target.player, s.target.card).damage_source_player]
                                           [PLAYER_CARD_INSTANCE(s.target.player, s.target.card).damage_source_card][player]
                                               .amount *
                              2 +
                          PLAYER_CARD_INSTANCE(s.target.player, s.target.card).info_slot);
            PLAYER_CARD_INSTANCE(s.target.player, s.target.card).info_slot = 0;
          }
        }
        else
        {
          g_spell_fizzled = 1;
        }
        PARENT_CARD_INSTANCE(player, card).number_of_targets = 0;
      }
      kill_card(player, card, KILL_BURY);
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x004ff33d
// FUNCTION: SHANDALAR 0x004b9465
int card_eye_for_an_eye(int player, int card, event_t event)
{
  target_t selected_target;

  if (event == EVENT_CAN_CAST)
  {
    load_recorded_action_target(0);
    return ((g_land_can_be_played & LCBP_DAMAGE_PREVENTION) != 0 &&
            real_target_available((int *)0,
                                  TARGET_SCAN_DIRECT,
                                  player,
                                  2,
                                  2,
                                  TARGET_ZONE_IN_PLAY,
                                  TYPE_NONE,
                                  TYPE_NONE,
                                  0,
                                  0,
                                  COLOR_TEST_0,
                                  COLOR_TEST_0,
                                  g_damage_card_internal_card_id,
                                  -1,
                                  -1,
                                  -1,
                                  TARGET_SPECIAL_DAMAGE_PLAYER,
                                  0,
                                  0))
               ? 99
               : 0;
  }

  if (event == EVENT_CAST_SPELL && g_affected_card == card && g_affected_card_controller == player)
  {
    load_text("prompts.txt", "EYE_FOR_EYE");
    if (C_real_select_target(player,
                             2,
                             2,
                             TARGET_ZONE_IN_PLAY,
                             TYPE_NONE,
                             TYPE_NONE,
                             0,
                             0,
                             COLOR_TEST_0,
                             COLOR_TEST_0,
                             g_damage_card_internal_card_id,
                             -1,
                             -1,
                             -1,
                             TARGET_SPECIAL_DAMAGE_PLAYER,
                             0,
                             0,
                             g_text_lines[0],
                             1,
                             &selected_target))
    {
      if (g_life[1 - player] > 0)
      {
        g_ai_modifier += (PLAYER_CARD_INSTANCE(selected_target.player, selected_target.card).info_slot * 100) / g_life[1 - player];
      }
      else
      {
        g_ai_modifier += 1000;
      }
      SET_TARGET(PLAYER_CARD_INSTANCE(player, card).targets[0], selected_target);
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
    }
    else
    {
      g_spell_fizzled = 1;
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    damage_player((int)TARGET_CARD_INSTANCE(player, card, 0).damage_source_player,
                  TARGET_CARD_INSTANCE(player, card, 0).info_slot,
                  player,
                  card);
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004ff68a
// FUNCTION: SHANDALAR 0x004b97b4
int card_inferno(int player, int card, event_t event)
{
  int current_card;
  int current_player;

  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if (event == EVENT_CAST_SPELL &&
      g_affected_card == card &&
      g_affected_card_controller == player)
  {
    PLAYER_CARD_INSTANCE(player, card).info_slot = 6;
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    current_player = player;
    while (current_player >= 0 && current_player < 2)
    {
      damage_player(current_player, PLAYER_CARD_INSTANCE(player, card).info_slot, player, card);
      for (current_card = 0; current_card < g_active_cards_count[current_player]; ++current_card)
      {
        if (is_in_play(current_player, current_card) != 0 &&
            (global_cards_data[PLAYER_CARD_INSTANCE(current_player, current_card).internal_card_id].type &
             TYPE_CREATURE) != 0)
        {
          damage_creature(current_player,
                          current_card,
                          PLAYER_CARD_INSTANCE(player, card).info_slot,
                          player,
                          card);
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

// FUNCTION: MAGIC 0x004ff86e
// FUNCTION: SHANDALAR 0x004b9998
int card_fissure(int player, int card, event_t event)
{
  target_t target;

  if (event == EVENT_CAN_CAST)
  {
    load_recorded_action_target(0);
    return real_target_available((int *)0,
                                 TARGET_SCAN_DIRECT,
                                 player,
                                 2,
                                 2,
                                 TARGET_ZONE_IN_PLAY,
                                 TYPE_CREATURE | TYPE_LAND,
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
    load_text("prompts.txt", "FISSURE");
    if (C_real_select_target(player,
                             2,
                             1 - player,
                             TARGET_ZONE_IN_PLAY,
                             TYPE_CREATURE | TYPE_LAND,
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
                               TYPE_CREATURE | TYPE_LAND,
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
      kill_card(target.player, target.card, KILL_BURY);
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

// FUNCTION: MAGIC 0x004ffb2e
// FUNCTION: SHANDALAR 0x004b9c58
int card_orcish_catapult(int player, int card, event_t event)
{
  int current_target;
  int random_target;
  int target_count;
  target_t targets[300];

  if (event == EVENT_CAN_CAST)
  {
    if (player == g_other_player && (g_duel_network_flags & 2) == 0 && has_mana(player, COLOR_ANY, 3) == 0)
    {
      return 0;
    }
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

  if (event == EVENT_CAST_SPELL && g_affected_card == card && player == g_affected_card_controller)
  {
    target_count = choose_orcish_catapult_targets(player, card, targets);
    if (target_count != 0)
    {
      for (current_target = 0; current_target < g_x_value; ++current_target)
      {
        if ((g_duel_network_flags & 2) == 0)
        {
          random_target = internal_rand(target_count);
        }
        else
        {
          random_target = network_random(player, target_count);
        }
        PLAYER_CARD_INSTANCE(player, card).targets[(int)PLAYER_CARD_INSTANCE(player, card).number_of_targets] =
            targets[random_target];
        ++PLAYER_CARD_INSTANCE(player, card).number_of_targets;
      }
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    if (g_duel_ai_mode_state != 1)
    {
      play_sound_effect(WAV_ORCART);
      Sleep(0xdac);
    }
    while (PLAYER_CARD_INSTANCE(player, card).number_of_targets != 0)
    {
      --PLAYER_CARD_INSTANCE(player, card).number_of_targets;
      if (C_real_validate_target(
              PLAYER_CARD_INSTANCE(player, card)
                  .targets[(int)PLAYER_CARD_INSTANCE(player, card).number_of_targets]
                  .player,
              PLAYER_CARD_INSTANCE(player, card)
                  .targets[(int)PLAYER_CARD_INSTANCE(player, card).number_of_targets]
                  .card,
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
              0) != 0)
      {
        --PLAYER_CARD_INSTANCE(
              PLAYER_CARD_INSTANCE(player, card)
                  .targets[(int)PLAYER_CARD_INSTANCE(player, card).number_of_targets]
                  .player,
              PLAYER_CARD_INSTANCE(player, card)
                  .targets[(int)PLAYER_CARD_INSTANCE(player, card).number_of_targets]
                  .card)
              .counter_toughness;
        PLAYER_CARD_INSTANCE(
            PLAYER_CARD_INSTANCE(player, card)
                .targets[(int)PLAYER_CARD_INSTANCE(player, card).number_of_targets]
                .player,
            PLAYER_CARD_INSTANCE(player, card)
                .targets[(int)PLAYER_CARD_INSTANCE(player, card).number_of_targets]
                .card)
            .special_counters += 0x1000000;
        if (g_duel_ai_mode_state != 1)
        {
          play_sound_effect(WAV_CATATAP);
        }
      }
    }
    kill_card(player, card, KILL_DESTROY);
  }

  return 0;
}

// FUNCTION: SHANDALAR 0x0041a42f
int choose_orcish_catapult_targets(int player, int card, target_t *targets)
{
  int count;
  int current_card;
  int current_player;
  int network_count;
  unsigned int protections;

  count = 0;
  if (g_other_player == player && (g_duel_network_flags & 2) != 0)
  {
    TENTATIVE_wait_for_network_result(player, 0x14);
    network_count = g_network_result_packet.result;
    count = g_network_result_packet.result;
    for (current_card = 0; current_card < network_count; ++current_card)
    {
      TENTATIVE_wait_for_network_result(player, 0x1b);
      targets[current_card] = g_target_pair_network_packet.target;
    }
  }
  else
  {
    for (current_player = 0; current_player < 2; ++current_player)
    {
      for (current_card = 0; current_card < g_active_cards_count[current_player]; ++current_card)
      {
        if (is_in_play(current_player, current_card) != 0 &&
            (global_cards_data[PLAYER_CARD_INSTANCE(current_player, current_card).internal_card_id].type & TYPE_CREATURE) != 0)
        {
          protections = get_protections_from(player, card);
          if ((PLAYER_CARD_INSTANCE(current_player, current_card).regen_status & protections) == 0)
          {
            targets[count].player = current_player;
            targets[count].card = current_card;
            ++count;
          }
        }
      }
    }
    if ((g_duel_network_flags & 2) != 0)
    {
      g_network_result_packet.result = count;
      g_network_result_packet.packet_type = 0x14;
      TENTATIVE_send_network_result(player, 0x14);
      g_target_pair_network_packet.packet_type = 0x1b;
      for (current_card = 0; current_card < count; ++current_card)
      {
        g_target_pair_network_packet.target.player = 1 - targets[current_card].player;
        g_target_pair_network_packet.target.card = targets[current_card].card;
        TENTATIVE_send_network_result(player, 0x1b);
      }
    }
  }

  return count;
}
