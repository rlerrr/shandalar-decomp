#include <string.h>
#include "../game_support.h"
#include "../global_strings.h"

// FUNCTION: SHANDALAR 0x00480f45
int army_of_allah_pump_creature(int player, int card, int target_player, int target_card, int internal_card_id)
{
  int legacy_card;

  (void)internal_card_id;

  if ((PLAYER_CARD_INSTANCE(target_player, target_card).state & STATE_ATTACKING) != 0)
  {
    legacy_card = create_legacy_effect(player, card, LEGACY_EFFECT_PUMP, target_player, target_card);
    if (legacy_card != -1)
    {
      PLAYER_CARD_INSTANCE(player, legacy_card).counter_power = 2;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x004ab920
// FUNCTION: SHANDALAR 0x004a3190
int card_army_of_allah(int player, int card, event_t event)
{
  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    dispatch_function_to_all_cards_in_play(player, card, army_of_allah_pump_creature, g_current_player);
    kill_card(player, card, KILL_BURY);
  }

  if (event == EVENT_CHECK_PUMP && has_mana(player, COLOR_WHITE, 2) != 0 && has_mana(player, COLOR_ANY, 3) != 0)
  {
    g_global_power_bonus[player] += 2;
  }

  return 0;
}

// FUNCTION: MAGIC 0x004ab9c2
// FUNCTION: SHANDALAR 0x004a3232
int card_blaze_of_glory(int player, int card, event_t event)
{
  target_t selected_target;

  if (event == EVENT_CAN_CAST)
  {
    return (g_current_phase == PHASE_BEFORE_BLOCKING && real_target_available((int *)0,
                                                                              TARGET_SCAN_DIRECT,
                                                                              player,
                                                                              1 - g_current_player,
                                                                              1 - g_current_player,
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
                                                                              0))
               ? 1
               : 0;
  }

  if (((event == EVENT_CAST_SPELL) && (g_affected_card == card)) && (g_affected_card_controller == player))
  {
    load_text("promptsX1.txt", "BLAZE_OF_GLORY");
    if (C_real_select_target(player,
                             1 - g_current_player,
                             1 - g_current_player,
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
                               1 - g_current_player,
                               1 - g_current_player,
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
      create_legacy_effect(player, card, g_duel_generated_internal_card_id_29, selected_target.player, selected_target.card);
    }
    else
    {
      g_spell_fizzled = 1;
    }
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    kill_card(player, card, KILL_DESTROY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004abcd8
// FUNCTION: SHANDALAR 0x004a354a
int card_guardian_angel(int player, int card, event_t event)
{
  target_t selected_target;

  if (event == EVENT_CAN_CAST)
  {
    return ((g_land_can_be_played & LCBP_DAMAGE_PREVENTION) != 0 && real_target_available((int *)0,
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
                                                                                          ~SUB_WALL,
                                                                                          -1,
                                                                                          -1,
                                                                                          0,
                                                                                          0,
                                                                                          0))
               ? 99
               : 0;
  }

  if (((event == EVENT_CAST_SPELL) && (card == g_affected_card)) && (player == g_affected_card_controller))
  {
    load_text("promptsX1.txt", "GUARDIAN_ANGLE");
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
                             0,
                             0,
                             0,
                             g_text_lines[0],
                             1,
                             &selected_target))
    {
      SET_TARGET(PLAYER_CARD_INSTANCE(player, card).targets[0], selected_target);
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
      PLAYER_CARD_INSTANCE(player, card).info_slot = g_x_value;
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
      if (PLAYER_CARD_INSTANCE(player, card).info_slot < PLAYER_CARD_INSTANCE(selected_target.player, selected_target.card).info_slot)
      {
        PLAYER_CARD_INSTANCE(selected_target.player, selected_target.card).info_slot -= PLAYER_CARD_INSTANCE(player, card).info_slot;
      }
      else
      {
        PLAYER_CARD_INSTANCE(selected_target.player, selected_target.card).info_slot = 0;
      }
      create_legacy_effect(player,
                           card,
                           g_duel_generated_internal_card_id_24,
                           (int)(char)PLAYER_CARD_INSTANCE(selected_target.player, selected_target.card).damage_target_player, PLAYER_CARD_INSTANCE(selected_target.player, selected_target.card).damage_target_card);
    }
    else
    {
      g_spell_fizzled = 1;
    }
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    kill_card(player, card, KILL_DESTROY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004ac114
// FUNCTION: SHANDALAR 0x004a3988
int card_natural_selection(int player, int card, event_t event)
{
  struct
  {
    char (*prompt)[300];
    int count;
    int choice_count;
    target_t target;
    int available[3];
    int index;
    int selected[3];
    int top_three[3];
  } s;

  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if (event == EVENT_CAST_SPELL && card == g_affected_card && player == g_affected_card_controller)
  {
    load_text("promptsX1.txt", "NATURAL_SELECTION");
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
                             &s.target))
    {
      SET_TARGET(PLAYER_CARD_INSTANCE(player, card).targets[0], s.target);
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
      if (player == g_other_player)
      {
        g_ai_modifier += 0x18;
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
    load_text("promptsX1.txt", "NATURAL_SELECTION");
    s.count = 0;
    for (s.index = 0; s.index < 3; ++s.index)
    {
      s.top_three[s.index] = -1;
      s.available[s.index] = 0;
      if (global_library[PLAYER_CARD_INSTANCE(player, card).targets[0].player][s.index] != -1)
      {
        s.top_three[s.index] = global_library[PLAYER_CARD_INSTANCE(player, card).targets[0].player][s.index];
        s.available[s.index] = 1;
        ++s.count;
      }
    }

    if ((player == g_active_player || (g_duel_network_flags & 2) != 0) && g_duel_ai_mode_state != 1)
    {
      if (s.count > 0)
      {
        s.prompt = &g_text_lines[1];
        s.choice_count = select_from_graveyard_with_dialog(
            player, s.top_three, s.available, s.count, (int)&s.prompt, 1, (int)s.selected, 0, s.count);
      }
    }
    else if (internal_rand(2) != 0)
    {
      s.choice_count = 0;
      for (s.index = 0; s.index < s.count; ++s.index)
      {
        s.selected[s.index] = s.index;
        ++s.choice_count;
      }
    }
    else
    {
      s.choice_count = 0;
    }

    if (s.count > s.choice_count)
    {
      shuffle_duel_library(player, PLAYER_CARD_INSTANCE(player, card).targets[0].player);
    }
    else
    {
      for (s.index = 0; s.index < s.choice_count; ++s.index)
      {
        global_library[PLAYER_CARD_INSTANCE(player, card).targets[0].player][s.index] =
            s.top_three[s.selected[s.index]];
      }
    }

    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004ac52b
// FUNCTION: SHANDALAR 0x004a3da3
int card_psionic_blast(int player, int card, event_t event)
{

  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if (((event == EVENT_CAST_SPELL) && (card == g_affected_card)) && (player == g_affected_card_controller))
  {
    load_text("promptsX1.txt", "PSIONIC_BLAST");
    select_damage_target(player, card, 4);
    if (player == g_other_player)
    {
      g_ai_modifier += (3 - TARGET_CARD_INSTANCE(player, card, 0).toughness) * 0xc;
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    if (deal_damage_to_selected_target(player, card, EVENT_RESOLVE_SPELL, 4) != 0)
    {
      damage_player(player, 2, player, card);
    }
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004ac6c0
// FUNCTION: SHANDALAR 0x004a3f3b
int card_reverse_polarity(int player, int card, event_t event)
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
    if ((g_land_can_be_played & LCBP_DAMAGE_PREVENTION) != 0)
    {
      if (has_effect_source_type(player, -1, TYPE_ARTIFACT))
      {
        return 99;
      }
      else
      {
        return 0;
      }
    }
    else
    {
      if (player == g_other_player && (g_duel_network_flags & 2) == 0 && g_damage_accumulators[1][150][player].amount == 0)
      {
        return 0;
      }
      return 1;
    }
  }

  if (event == EVENT_CAST_SPELL && g_affected_card == card && g_affected_card_controller == player &&
      (g_land_can_be_played & LCBP_DAMAGE_PREVENTION) != 0)
  {
    load_text("promptsX1.txt", "REVERSE_POLARITY");
    if (C_real_select_target(player, 2, 2, TARGET_ZONE_IN_PLAY, TYPE_NONE, TYPE_NONE,
                             0, 0, COLOR_TEST_0, COLOR_TEST_0, g_damage_card_internal_card_id,
                             -1, -1, -1, TARGET_SPECIAL_DAMAGE_PLAYER, 0, 0,
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

  if (event == EVENT_RESOLVE_SPELL)
  {
    if ((g_land_can_be_played & LCBP_DAMAGE_PREVENTION) != 0)
    {
      SET_TARGET(s.target, PLAYER_CARD_INSTANCE(player, card).targets[0]);
      if (C_real_validate_target(s.target.player, s.target.card, (char *)0, player, 2, 2,
                                 TARGET_ZONE_IN_PLAY, TYPE_NONE, TYPE_NONE, 0, 0,
                                 COLOR_TEST_0, COLOR_TEST_0, g_damage_card_internal_card_id,
                                 -1, -1, -1, TARGET_SPECIAL_DAMAGE_PLAYER, 0, 0))
      {
        if (PLAYER_CARD_INSTANCE(s.target.player, s.target.card).info_slot != 0)
        {
          gain_life(player,
                    g_damage_accumulators[(char)PLAYER_CARD_INSTANCE(s.target.player, s.target.card).damage_source_player]
                                         [PLAYER_CARD_INSTANCE(s.target.player, s.target.card).damage_source_card][player]
                                             .amount *
                            2 +
                        PLAYER_CARD_INSTANCE(s.target.player, s.target.card).info_slot,
                    player,
                    card);
          PLAYER_CARD_INSTANCE(s.target.player, s.target.card).info_slot = 0;
        }
      }
      else
      {
        g_spell_fizzled = 1;
      }
      PARENT_CARD_INSTANCE(player, card).number_of_targets = 0;
    }
    else
    {
      s.best_damage = s.count = 0;
      for (s.controller = 0; s.controller < 2; ++s.controller)
      {
        for (s.current_card = 0; s.current_card < 150; ++s.current_card)
        {
          if (g_damage_accumulators[s.controller][s.current_card][player].amount > 0 &&
              (g_damage_accumulators[s.controller][s.current_card][player].source_type & TYPE_ARTIFACT) != 0)
          {
            s.damage_sources[s.count * 2] = s.controller;
            s.damage_sources[s.count * 2 + 1] = s.current_card;
            s.damage_amounts[s.count] = g_damage_accumulators[s.controller][s.current_card][player].amount;
            if (PLAYER_CARD_INSTANCE(s.controller, s.current_card).internal_card_id == -1)
            {
              s.internal_card_ids[s.count] = PLAYER_CARD_INSTANCE(s.controller, s.current_card).original_internal_card_id;
            }
            else
            {
              s.internal_card_ids[s.count] = PLAYER_CARD_INSTANCE(s.controller, s.current_card).internal_card_id;
            }
            ++s.count;
          }
        }
      }

      if (s.count > 0)
      {
        if ((player == g_other_player && (g_duel_network_flags & 2) == 0) || g_duel_ai_mode_state == 1)
        {
          s.best_damage = 0;
          for (s.selection_index = 0; s.selection_index < s.count; ++s.selection_index)
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
          load_text("promptsX1.txt", "REVERSE_POLARITY");
          s.selected = select_damage_card_from_list(player, s.internal_card_ids, s.damage_amounts, s.count,
                                                    g_text_lines[1], 1, "");
        }

        gain_life(player,
                  g_damage_accumulators[s.damage_sources[s.selected * 2]][s.damage_sources[s.selected * 2 + 1]][player].amount * 2,
                  player,
                  card);
        g_damage_accumulators[s.damage_sources[s.selected * 2]][s.damage_sources[s.selected * 2 + 1]][player].amount = 0;
      }

      if (g_life[1 - player] > 0)
      {
        g_ai_modifier += (s.best_damage * 100) / g_life[1 - player] - 100;
      }
      else
      {
        g_ai_modifier += 1000;
      }
    }

    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004acf05
// FUNCTION: SHANDALAR 0x004a4784
int card_sewers_of_estark(int player, int card, event_t event)
{
  target_t target;

  if (event == EVENT_CAN_CAST)
  {
    return real_target_available((int *)0, TARGET_SCAN_DIRECT, player, 2, 2, TARGET_ZONE_IN_PLAY,
                                 TYPE_CREATURE, TYPE_NONE, 0, get_protections_from(player, card),
                                 COLOR_TEST_0, COLOR_TEST_0, -1, -1, -1, -1, 0, 0, 0);
  }

  if (event == EVENT_CAST_SPELL && g_affected_card == card && g_affected_card_controller == player)
  {
    g_ai_modifier += 0x48;
    load_text("promptsX1.txt", "SEWERS_OF_ESTARK");
    if (C_real_select_target(player, 2, 2, TARGET_ZONE_IN_PLAY, TYPE_CREATURE, TYPE_NONE,
                             0, get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0,
                             -1, -1, -1, -1, 0, 0, 0, g_text_lines[0], 1, &target) != 0)
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
                               get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0,
                               -1, -1, -1, -1, 0, 0, 0) != 0)
    {
      if ((PLAYER_CARD_INSTANCE(target.player, target.card).state & STATE_ATTACKING) != 0)
      {
        create_legacy_effect(player, card, g_duel_generated_internal_card_id_02, target.player, target.card);
      }
      else if (PLAYER_CARD_INSTANCE(target.player, target.card).blocking != -1 &&
               target.player != g_current_player)
      {
        create_legacy_effect(player, card, g_duel_generated_internal_card_id_26, target.player, target.card);
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
// FUNCTION: MAGIC 0x004ad263
// FUNCTION: SHANDALAR 0x004a4ae4
int card_artifact_blast(int player, int card, event_t event)
{
  if (event == EVENT_CAN_CAST)
  {
    load_recorded_action_target(0);
    if (g_current_spell_player != -1)
    {
      return C_real_validate_target(g_current_spell_player, g_current_spell_card, (char *)0,
                                    player, 2, 2, 0, TYPE_ARTIFACT, TYPE_NONE, 0, 0,
                                    COLOR_TEST_0, COLOR_TEST_0, -1, -1, -1, -1,
                                    TARGET_SPECIAL_SPELL_ON_STACK, 0, 0)
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
    g_ai_modifier -= 0x24;
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    if (C_real_validate_target(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                               PLAYER_CARD_INSTANCE(player, card).targets[0].card, (char *)0,
                               player, 2, 2, 0, TYPE_ARTIFACT, TYPE_NONE, 0, 0,
                               COLOR_TEST_0, COLOR_TEST_0, -1, -1, -1, -1,
                               TARGET_SPECIAL_SPELL_ON_STACK, 0, 0) != 0)
    {
      if ((TARGET_CARD_INSTANCE(player, card, 0).state &
           STATE_INVISIBLE) != 0)
      {
        kill_card(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                  PLAYER_CARD_INSTANCE(player, card).targets[0].card, KILL_BURY);
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

// FUNCTION: MAGIC 0x004ad5b4
// FUNCTION: SHANDALAR 0x004a4e37
int card_sacrifice(int player, int card, event_t event)
{
  int iid;
  int amount;

  if (event == EVENT_CAN_CAST)
  {
    return real_target_available((int *)0, TARGET_SCAN_DIRECT, player, player, player, 0x200, 2, 0, 0,
                                 0, 0, 0, -1, -1, 0xffffffff, 0xffffffff, 0, 0, 0);
  }

  if (event == EVENT_CAST_SPELL && g_affected_card == card && g_affected_card_controller == player)
  {
    load_text("promptsX1.txt", "SACRIFICE");
    PLAYER_CARD_INSTANCE(player, card).targets[0].card = choose_creature_to_sacrifice(player);
    PLAYER_CARD_INSTANCE(player, card).targets[0].player = player;
    kill_card(player, PLAYER_CARD_INSTANCE(player, card).targets[0].card, KILL_SACRIFICE);
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    iid = TARGET_CARD_INSTANCE(player, card, 0).original_internal_card_id;
    amount = (int)(char)global_cards_data[iid].cc[0] + (int)(char)global_cards_data[iid].cc[1];
    if ((int)(char)global_cards_data[iid].cc[1] == -1)
    {
      ++amount;
    }
    produce_mana(player, COLOR_BLACK, amount);
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}
