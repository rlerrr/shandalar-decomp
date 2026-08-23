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
    unk_007a7d80[player] += 2;
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
      PLAYER_CARD_INSTANCE(player, card).targets[0].player = selected_target.player;
      PLAYER_CARD_INSTANCE(player, card).targets[0].card = selected_target.card;
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
    }
    else
    {
      g_spell_fizzled = 1;
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    selected_target.player = PLAYER_CARD_INSTANCE(player, card).targets[0].player;
    selected_target.card = PLAYER_CARD_INSTANCE(player, card).targets[0].card;
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
      create_legacy_effect(player, card, unk_008a8df0, selected_target.player, selected_target.card);
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
  card_instance_t *instance;
  card_instance_t *damage;
  target_t selected_target;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_CAN_CAST)
  {
    if ((g_land_can_be_played & 4) == 0 || !real_target_available((int *)0,
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
    {
      return 0;
    }
    return 99;
  }

  if (((event == EVENT_CAST_SPELL) && (card == g_affected_card)) && (player == g_affected_card_controller))
  {
    load_text("promptsX1.txt", "GUARDIAN_ANGLE");
    if (!C_real_select_target(player,
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
      g_spell_fizzled = 1;
    }
    else
    {
      instance->targets[0] = selected_target;
      instance->number_of_targets = 1;
      instance->info_slot = g_x_value;
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    selected_target = instance->targets[0];
    if (!C_real_validate_target(selected_target.player,
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
      g_spell_fizzled = 1;
    }
    else
    {
      damage = &PLAYER_CARD_INSTANCE(selected_target.player, selected_target.card);
      if (instance->info_slot < damage->info_slot)
      {
        damage->info_slot -= instance->info_slot;
      }
      else
      {
        damage->info_slot = 0;
      }
      create_legacy_effect(player,
                           card,
                           unk_008cf1bc,
                           (int)damage->damage_source_player,
                           damage->damage_source_card);
    }
    instance->number_of_targets = 0;
    kill_card(player, card, KILL_DESTROY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004ac114
// FUNCTION: SHANDALAR 0x004a3988
int card_natural_selection(int player, int card, event_t event)
{
  card_instance_t *instance;
  target_t target;
  int available[10];
  int selected[3];
  int top_three[3];
  int count;
  int choice_count;
  int index;
  char (*prompt)[300];

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if (event == EVENT_CAST_SPELL && card == g_affected_card && player == g_affected_card_controller)
  {
    load_text("promptsX1.txt", "NATURAL_SELECTION");
    if (!C_real_select_target(player,
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
      g_spell_fizzled = 1;
    }
    else
    {
      instance->targets[0].player = target.player;
      instance->targets[0].card = target.card;
      instance->number_of_targets = 1;
      if (player == g_other_player)
      {
        g_ai_modifier += 0x18;
      }
    }
    return 0;
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    load_text("promptsX1.txt", "NATURAL_SELECTION");
    count = 0;
    for (index = 0; index < 3; ++index)
    {
      top_three[index] = -1;
      available[index] = 0;
      if (global_library[instance->targets[0].player][index] != -1)
      {
        top_three[index] = global_library[instance->targets[0].player][index];
        available[index] = 1;
        ++count;
      }
    }

    if ((player == g_active_player || (g_duel_network_flags & 2) != 0) && g_duel_ai_mode_state != 1)
    {
      if (count > 0)
      {
        prompt = g_text_lines;
        choice_count = select_from_graveyard_with_dialog(
            player, top_three, available, count, (int)&prompt, 1, (int)selected, 0, count);
      }
    }
    else if (internal_rand(2) == 0)
    {
      choice_count = 0;
    }
    else
    {
      choice_count = 0;
      for (index = 0; index < count; ++index)
      {
        selected[index] = index;
        ++choice_count;
      }
    }

    if (choice_count < count)
    {
      shuffle_duel_library(player, instance->targets[0].player);
    }
    else
    {
      for (index = 0; index < choice_count; ++index)
      {
        global_library[instance->targets[0].player][index] = top_three[selected[index]];
      }
    }

    instance->number_of_targets = 0;
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004ac52b
// FUNCTION: SHANDALAR 0x004a3da3
int card_psionic_blast(int player, int card, event_t event)
{
  card_instance_t *instance;

  instance = &PLAYER_CARD_INSTANCE(player, card);

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
      g_ai_modifier += (3 - PLAYER_CARD_INSTANCE(instance->targets[0].player, instance->targets[0].card).toughness) * 0xc;
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    if (deal_damage_to_selected_target(player, card, EVENT_RESOLVE_SPELL, 4) != 0)
    {
      damage_player(player, 2, player, card);
    }
    instance->number_of_targets = 0;
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
    int damage_sources[300];
    int damage_amounts[150];
    int internal_card_ids[150];
    int count;
    int controller;
    int current_card;
    int selected;
    int best_damage;
  } s;
  int result;

  if (event == EVENT_CAN_CAST)
  {
    load_recorded_action_target(0);
    if ((g_land_can_be_played & 4) == 0)
    {
      if (player == g_other_player && (g_duel_network_flags & 2) == 0 && g_damage_accumulators[1][150][player].amount == 0)
      {
        return 0;
      }
      return 1;
    }

    result = has_effect_source_type(player, -1, TYPE_ARTIFACT);
    return (result == 0) ? 0 : 99;
  }

  if (event == EVENT_CAST_SPELL && g_affected_card == card && g_affected_card_controller == player &&
      (g_land_can_be_played & 4) != 0)
  {
    load_text("promptsX1.txt", "REVERSE_POLARITY");
    if (C_real_select_target(player, 2, 2, TARGET_ZONE_IN_PLAY, TYPE_NONE, TYPE_NONE,
                             0, 0, COLOR_TEST_0, COLOR_TEST_0, g_damage_card_internal_card_id,
                             -1, -1, -1, TARGET_SPECIAL_DAMAGE_PLAYER, 0, 0,
                             g_text_lines[0], 1, &s.target) == 0)
    {
      g_spell_fizzled = 1;
    }
    else
    {
      PLAYER_CARD_INSTANCE(player, card).targets[0] = s.target;
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    if ((g_land_can_be_played & 4) == 0)
    {
      s.count = 0;
      s.best_damage = 0;
      for (s.controller = 0; s.controller < 2; ++s.controller)
      {
        for (s.current_card = 0; s.current_card < 150; ++s.current_card)
        {
          if (g_damage_accumulators[s.controller][s.current_card][player].amount > 0 &&
              (global_cards_data[PLAYER_CARD_INSTANCE(s.controller, s.current_card).internal_card_id].type & TYPE_ARTIFACT) != 0)
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
          for (s.current_card = 0; s.current_card < s.count; ++s.current_card)
          {
            if (s.best_damage < s.damage_amounts[s.current_card])
            {
              s.best_damage = s.current_card;
            }
          }
          s.selected = s.best_damage;
        }
        else
        {
          load_text("promptsX1.txt", "REVERSE_POLARITY");
          s.selected = select_damage_card_from_list(player, s.internal_card_ids, s.damage_amounts, s.count,
                                                    0x8aaa4c, 1, g_text_lines[1]);
        }

        gain_life(player,
                  g_damage_accumulators[s.damage_sources[s.selected * 2]][s.damage_sources[s.selected * 2 + 1]][player].amount * 2);
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
      s.target = PLAYER_CARD_INSTANCE(player, card).targets[0];
      if (C_real_validate_target(s.target.player, s.target.card, (char *)0, player, 2, 2,
                                 TARGET_ZONE_IN_PLAY, TYPE_NONE, TYPE_NONE, 0, 0,
                                 COLOR_TEST_0, COLOR_TEST_0, g_damage_card_internal_card_id,
                                 -1, -1, -1, TARGET_SPECIAL_DAMAGE_PLAYER, 0, 0) == 0)
      {
        g_spell_fizzled = 1;
      }
      else if (PLAYER_CARD_INSTANCE(s.target.player, s.target.card).info_slot != 0)
      {
        gain_life(player,
                  g_damage_accumulators[(char)PLAYER_CARD_INSTANCE(s.target.player, s.target.card).damage_source_player]
                                       [PLAYER_CARD_INSTANCE(s.target.player, s.target.card).damage_source_card][player].amount * 2 +
                      PLAYER_CARD_INSTANCE(s.target.player, s.target.card).info_slot);
        PLAYER_CARD_INSTANCE(s.target.player, s.target.card).info_slot = 0;
      }
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                           PLAYER_CARD_INSTANCE(player, card).parent_card)
          .number_of_targets = 0;
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
                             -1, -1, -1, -1, 0, 0, 0, g_text_lines[0], 1, &target) == 0)
    {
      g_spell_fizzled = 1;
    }
    else
    {
      PLAYER_CARD_INSTANCE(player, card).targets[0] = target;
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    target = PLAYER_CARD_INSTANCE(player, card).targets[0];
    if (C_real_validate_target(target.player, target.card, (char *)0, player, 2, 2,
                               TARGET_ZONE_IN_PLAY, TYPE_CREATURE, TYPE_NONE, 0,
                               get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0,
                               -1, -1, -1, -1, 0, 0, 0) == 0)
    {
      g_spell_fizzled = 1;
    }
    else if ((PLAYER_CARD_INSTANCE(target.player, target.card).state & STATE_ATTACKING) != 0)
    {
      create_legacy_effect(player, card, unk_007abc7c, target.player, target.card);
    }
    else if (PLAYER_CARD_INSTANCE(target.player, target.card).blocking != -1 &&
             target.player != g_current_player)
    {
      create_legacy_effect(player, card, DAT_008b40cc, target.player, target.card);
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
  card_instance_t *instance;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_CAN_CAST)
  {
    load_recorded_action_target(0);
    if (g_current_spell_player == -1)
    {
      return 0;
    }
    if (C_real_validate_target(g_current_spell_player, g_current_spell_card, (char *)0,
                               player, 2, 2, 0, TYPE_ARTIFACT, TYPE_NONE, 0, 0,
                               COLOR_TEST_0, COLOR_TEST_0, -1, -1, -1, -1,
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
      instance->targets[0].player = g_current_spell_player;
      instance->targets[0].card = g_current_spell_card;
      instance->number_of_targets = 1;
    }
    g_ai_modifier -= 0x24;
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    if (C_real_validate_target(instance->targets[0].player, instance->targets[0].card, (char *)0,
                               player, 2, 2, 0, TYPE_ARTIFACT, TYPE_NONE, 0, 0,
                               COLOR_TEST_0, COLOR_TEST_0, -1, -1, -1, -1,
                               TARGET_SPECIAL_SPELL_ON_STACK, 0, 0) == 0)
    {
      g_spell_fizzled = 1;
    }
    else if ((PLAYER_CARD_INSTANCE(instance->targets[0].player, instance->targets[0].card).state & STATE_INVISIBLE) != 0)
    {
      kill_card(instance->targets[0].player, instance->targets[0].card, KILL_BURY);
    }
    instance->number_of_targets = 0;
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
    iid = PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                               PLAYER_CARD_INSTANCE(player, card).targets[0].card).original_internal_card_id;
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
