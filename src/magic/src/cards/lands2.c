#include <stdio.h>
#include <string.h>
#include "../game_support.h"
#include "../global_strings.h"

extern card_ptr_t global_raw_cards_storage[2000];

// FUNCTION: SHANDALAR 0x004882b5
unsigned int get_urza_land_types_in_play(int player)
{
  int current_card;
  int internal_card_id;
  unsigned int result;

  result = 0;
  for (current_card = 0; current_card < g_active_cards_count[player]; ++current_card)
  {
    internal_card_id = PLAYER_CARD_INSTANCE(player, current_card).internal_card_id;
    if (is_in_play(player, current_card) && global_cards_data[internal_card_id].subtype == SUB_RAT)
    {
      if (global_cards_data[internal_card_id].id == CARD_ID_URZAS_MINE)
      {
        result |= 1;
      }
      if (global_cards_data[internal_card_id].id == CARD_ID_URZAS_POWER_PLANT)
      {
        result |= 2;
      }
      if (global_cards_data[internal_card_id].id == CARD_ID_URZAS_TOWER)
      {
        result |= 4;
      }
    }
  }

  return result;
}

// FUNCTION: MAGIC 0x004828ce
// FUNCTION: SHANDALAR 0x004863e1
int arena_has_targets(int player, int card)
{
  int current_card;
  int found_active_player;
  int found_other_player;

  found_active_player = 0;
  found_other_player = 0;
  current_card = 0;

  while (current_card < g_active_cards_count[g_active_player] && found_active_player == 0)
  {
    if (is_in_play(g_active_player, current_card) &&
        (global_cards_data[PLAYER_CARD_INSTANCE(g_active_player, current_card).internal_card_id].type & TYPE_CREATURE) != 0 &&
        (PLAYER_CARD_INSTANCE(g_active_player, current_card).regen_status & get_protections_from(player, card)) == 0)
    {
      found_active_player = 1;
    }
    ++current_card;
  }

  if (found_active_player != 0)
  {
    current_card = 0;
    while (current_card < g_active_cards_count[g_other_player] && found_other_player == 0)
    {
      if (is_in_play(g_other_player, current_card) &&
          (global_cards_data[PLAYER_CARD_INSTANCE(g_other_player, current_card).internal_card_id].type & TYPE_CREATURE) != 0 &&
          (PLAYER_CARD_INSTANCE(g_other_player, current_card).regen_status & get_protections_from(player, card)) == 0)
      {
        found_other_player = 1;
      }
      ++current_card;
    }
  }

  return found_active_player != 0 && found_other_player != 0;
}

// FUNCTION: SHANDALAR 0x00487749
int select_best_arena_creature(int unused, int target_player, unsigned char required_type, target_t *target)
{
  int current_player;
  int current_card;
  int score;
  int best_score;
  unsigned int abilities;
  int result;

  best_score = -1;
  result = 0;
  target->player = -1;
  target->card = -1;
  for (current_player = 0; current_player < 2; ++current_player)
  {
    if (target_player == -1 || target_player == current_player)
    {
      for (current_card = 0; current_card < g_active_cards_count[current_player]; ++current_card)
      {
        if (is_in_play(current_player, current_card) &&
            (global_cards_data[PLAYER_CARD_INSTANCE(current_player, current_card).internal_card_id].type & required_type) != 0 &&
            PLAYER_CARD_INSTANCE(current_player, current_card).kill_code != KILL_REMOVE &&
            (PLAYER_CARD_INSTANCE(current_player, current_card).state & STATE_CANNOT_TARGET) == 0)
        {
          score = C_get_abilities(current_player, current_card, EVENT_POWER, -1);
          score += C_get_abilities(current_player, current_card, EVENT_TOUGHNESS, -1);
          abilities = C_get_abilities(current_player, current_card, EVENT_ABILITIES, -1);
          if ((abilities & KEYWORD_FLYING) != 0)
          {
            ++score;
          }
          if ((abilities & KEYWORD_TRAMPLE) != 0)
          {
            ++score;
          }
          while (abilities != 0)
          {
            if ((abilities & 1) != 0)
            {
              ++score;
            }
            abilities >>= 1;
          }
          if ((global_cards_data[PLAYER_CARD_INSTANCE(current_player, current_card).internal_card_id].extra_ability & 0x1000) != 0)
          {
            ++score;
          }
          if ((global_cards_data[PLAYER_CARD_INSTANCE(current_player, current_card).internal_card_id].extra_ability & 1) != 0)
          {
            ++score;
          }
          if (best_score < score)
          {
            best_score = score;
            target->player = current_player;
            target->card = current_card;
            result = 1;
          }
        }
      }
    }
  }

  return result;
}

// FUNCTION: SHANDALAR 0x00558828
int send_arena_network_choice(int player, int choice)
{
  if (g_active_player == player && (g_duel_network_flags & 2) != 0)
  {
    g_network_result_packet_type = 0xd;
    g_network_result_value = choice;
    TENTATIVE_send_network_result(player, 0xd);
  }
  return choice;
}

// FUNCTION: MAGIC 0x005022a0
// FUNCTION: SHANDALAR 0x00552320
int card_arena(int player, int card, event_t event)
{
  card_instance_t *instance;
  card_instance_t *parent;
  target_t target;
  int first_valid;
  int second_valid;
  int first_power;
  int second_power;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_CAST_SPELL &&
      g_affected_card == card &&
      g_affected_card_controller == player &&
      player == g_other_player)
  {
    g_ai_modifier += 0x30;
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    return (instance->state & STATE_TAPPED) == 0 &&
           !is_animated_and_sick(player, card) &&
           has_mana(player, COLOR_ANY, 3) &&
           arena_has_targets(player, card);
  }

  if (event == EVENT_ACTIVATE)
  {
    if (unk_00938e2c == 0)
    {
      charge_mana(player, COLOR_COLORLESS, 3);
      if (g_spell_fizzled != 1)
      {
        if (g_active_player == player)
        {
          load_text("promptsX1.txt", "ARENA");
          if (C_real_select_target(player, player, player, TARGET_ZONE_IN_PLAY, TYPE_CREATURE, TYPE_NONE, 0,
                                   get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0,
                                   -1, -1, -1, -1, 0, 0, 0, g_text_lines[0], 1, &target) == 0)
          {
            g_spell_fizzled = 1;
          }
          else
          {
            instance->targets[instance->number_of_targets] = target;
            ++instance->number_of_targets;
            instance->state |= STATE_TAPPED;
            if ((g_duel_network_flags & 2) == 0)
            {
              select_best_arena_creature(1 - player, 1 - player, TYPE_CREATURE, &target);
            }
            else
            {
              send_arena_network_choice(player, 0);
              load_text("promptsX1.txt", "ARENA");
              C_real_select_target(1 - player, 1 - player, 1 - player, TARGET_ZONE_IN_PLAY,
                                   TYPE_CREATURE, TYPE_NONE, 0, get_protections_from(player, card),
                                   COLOR_TEST_0, COLOR_TEST_0, -1, -1, -1, -1, 0, 0, 0,
                                   g_text_lines[0], 0, &target);
            }
            instance->targets[instance->number_of_targets] = target;
            ++instance->number_of_targets;
            do_dialog(1 - player, player, card, target.player, target.card, g_text_lines[1], 0);
          }
        }
        else
        {
          load_text("promptsX1.txt", "ARENA");
          if ((g_duel_network_flags & 2) == 0)
          {
            g_spell_fizzled = select_best_arena_creature(player, player, TYPE_CREATURE, &target) == 0;
          }
          else
          {
            g_spell_fizzled = C_real_select_target(player, player, player, TARGET_ZONE_IN_PLAY,
                                                   TYPE_CREATURE, TYPE_NONE, 0,
                                                   get_protections_from(player, card), COLOR_TEST_0,
                                                   COLOR_TEST_0, -1, -1, -1, -1, 0, 0, 0,
                                                   g_text_lines[0], 1, &target) == 0;
          }
          if (g_spell_fizzled != 1)
          {
            instance->targets[instance->number_of_targets] = target;
            ++instance->number_of_targets;
            do_dialog(player, player, card, target.player, target.card, g_text_lines[1], 0);
            load_text("promptsX1.txt", "ARENA");
            C_real_select_target(1 - player, 1 - player, 1 - player, TARGET_ZONE_IN_PLAY,
                                 TYPE_CREATURE, TYPE_NONE, 0, get_protections_from(player, card),
                                 COLOR_TEST_0, COLOR_TEST_0, -1, -1, -1, -1, 0, 0, 0,
                                 g_text_lines[0], 0, &target);
            instance->targets[instance->number_of_targets] = target;
            ++instance->number_of_targets;
            instance->state |= STATE_TAPPED;
            if ((g_duel_network_flags & 2) != 0)
            {
              send_arena_network_choice(1 - player, 0);
            }
          }
        }

        if (g_spell_fizzled == 1)
        {
          instance->number_of_targets = 0;
        }
      }
    }
    else
    {
      g_spell_fizzled = 1;
    }
    return 0;
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    first_valid = C_real_validate_target(instance->targets[0].player,
                                         instance->targets[0].card,
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
                                         -1,
                                         -1,
                                         -1,
                                         0,
                                         0,
                                         0);
    second_valid = C_real_validate_target(instance->targets[1].player,
                                          instance->targets[1].card,
                                          (char *)0,
                                          1 - player,
                                          1 - player,
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
                                          0);

    if (first_valid)
    {
      PLAYER_CARD_INSTANCE(instance->targets[0].player, instance->targets[0].card).state |= STATE_TAPPED;
    }
    if (second_valid)
    {
      PLAYER_CARD_INSTANCE(instance->targets[1].player, instance->targets[1].card).state |= STATE_TAPPED;
    }

    if (first_valid && second_valid)
    {
      PLAYER_CARD_INSTANCE(instance->targets[0].player, instance->targets[0].card).token_status |= 0x04000000;
      PLAYER_CARD_INSTANCE(instance->targets[1].player, instance->targets[1].card).token_status |= 0x04000000;
      first_power = C_get_abilities(instance->targets[0].player, instance->targets[0].card, EVENT_POWER, -1);
      second_power = C_get_abilities(instance->targets[1].player, instance->targets[1].card, EVENT_POWER, -1);
      damage_creature(instance->targets[0].player,
                      instance->targets[0].card,
                      second_power,
                      instance->targets[1].player,
                      instance->targets[1].card);
      damage_creature(instance->targets[1].player,
                      instance->targets[1].card,
                      first_power,
                      instance->targets[0].player,
                      instance->targets[0].card);
    }
    else if (first_valid && PLAYER_CARD_INSTANCE(instance->targets[1].player, instance->targets[1].card).internal_card_id != -1)
    {
      first_power = C_get_abilities(instance->targets[0].player, instance->targets[0].card, EVENT_POWER, -1);
      damage_creature(instance->targets[1].player,
                      instance->targets[1].card,
                      first_power,
                      instance->targets[0].player,
                      instance->targets[0].card);
    }
    else if (second_valid && PLAYER_CARD_INSTANCE(instance->targets[0].player, instance->targets[0].card).internal_card_id != -1)
    {
      second_power = C_get_abilities(instance->targets[1].player, instance->targets[1].card, EVENT_POWER, -1);
      damage_creature(instance->targets[0].player,
                      instance->targets[0].card,
                      second_power,
                      instance->targets[1].player,
                      instance->targets[1].card);
    }

    parent = &PLAYER_CARD_INSTANCE(instance->parent_controller, instance->parent_card);
    parent->number_of_targets = 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x00503044
// FUNCTION: SHANDALAR 0x005530c4
int card_bazaar_of_baghdad(int player, int card, event_t event)
{
  struct
  {
    int i;
    int num_available;
    int num_lands;
    int internal_card_id;
  } s;

  if (event == EVENT_CAST_SPELL && g_affected_card == card && g_affected_card_controller == player)
  {
    if (g_other_player == player)
    {
      g_ai_modifier += 0x30;
    }
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
    if (g_other_player == player && (g_duel_network_flags & 2) == 0)
    {
      s.num_available = 0;
      s.num_lands = 0;

      for (s.i = 0; s.i < g_active_cards_count[player]; ++s.i)
      {
        s.internal_card_id = PLAYER_CARD_INSTANCE(player, s.i).internal_card_id;
        if (s.internal_card_id != -1)
        {
          if ((PLAYER_CARD_INSTANCE(player, s.i).state & 0x22) == 0 &&
              (global_cards_data[s.internal_card_id].type & TYPE_INTERRUPT) == 0)
          {
            ++s.num_available;
          }
          if ((global_cards_data[s.internal_card_id].type & TYPE_LAND) != 0)
          {
            ++s.num_lands;
          }
        }
      }

      g_ai_modifier += (7 - s.num_available) * -0x18;
      if (s.num_lands < 3 || 8 < s.num_lands)
      {
        g_ai_modifier += 0x30;
      }
    }

    PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
    return 0;
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    draw_card_for_player(player);
    draw_card_for_player(player);
    discard(player, 0, 0);
    discard(player, 0, 0);
    discard(player, 0, 0);
    return 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x00503331
// FUNCTION: SHANDALAR 0x005533b2
int card_city_of_brass(int player, int card, event_t event)
{
  struct
  {
    int color;
    int current_color;
    color_test_t available_colors;
  } s;
  card_instance_t *instance;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_CAN_ACTIVATE)
  {
    return (instance->state & STATE_TAPPED) == 0 && !is_animated_and_sick(player, card);
  }

  if (event == EVENT_ACTIVATE && (instance->state & STATE_TAPPED) == 0)
  {
    s.available_colors = (color_test_t)(unsigned char)instance->mana_color;
    s.color = -1;

    if ((player == g_other_player && (g_duel_network_flags & 2) == 0) ||
        g_duel_ai_mode_state == 1 ||
        g_duel_network_state != 0)
    {
      for (s.current_color = COLOR_BLACK; s.current_color <= COLOR_WHITE && s.color == -1; ++s.current_color)
      {
        if ((unk_00938e2c & (1 << (unsigned char)s.current_color)) != 0 &&
            (s.available_colors & (1 << (unsigned char)s.current_color)) != 0)
        {
          s.color = s.current_color;
        }
      }
      if (s.color == -1 && (unk_00938e2c & (COLOR_TEST_COLORLESS | COLOR_TEST_ARTIFACT)) != 0)
      {
        s.color = COLOR_BLACK;
      }
    }

    if (g_spell_fizzled != 1)
    {
      if (unk_00938e2c == COLOR_TEST_COLORLESS ||
          unk_00938e2c == COLOR_TEST_ARTIFACT ||
          unk_00938e2c == (COLOR_TEST_COLORLESS | COLOR_TEST_ARTIFACT))
      {
        for (s.current_color = COLOR_COLORLESS; s.current_color < 7 && s.color == -1; ++s.current_color)
        {
          if ((s.available_colors & (1 << (unsigned char)s.current_color)) != 0)
          {
            s.color = s.current_color;
          }
        }
      }
      else if (s.color == -1)
      {
        s.color = choose_a_color(player, "Choose a color.", 1, -1, s.available_colors);
      }

      if (s.color == -1)
      {
        g_spell_fizzled = 1;
      }
      else
      {
        produce_mana(player, s.color, 1);
        undeclare_mana_available_hex(player, s.available_colors, 1);
        instance->state |= STATE_TAPPED;
        g_produced_mana_color = s.color;
      }
    }
  }

  if ((event == EVENT_TAP_CARD || event == EVENT_PLAY_ABILITY) &&
      g_affected_card == card &&
      g_affected_card_controller == player)
  {
    damage_player(player, 1, player, card);
  }

  if (event == EVENT_COUNT_MANA &&
      g_affected_card == card &&
      g_affected_card_controller == player &&
      (instance->state & STATE_TAPPED) == 0 &&
      !is_animated_and_sick(player, card))
  {
    declare_mana_available_hex(player, (color_test_t)(unsigned char)instance->mana_color, 1);
  }

  return 0;
}

// FUNCTION: MAGIC 0x00503987
// FUNCTION: SHANDALAR 0x00553a0e
int card_desert(int player, int card, event_t event)
{
  struct
  {
    char dialog[600];
    target_t target;
    int choice;
    int can_damage;
    int result;
  } s;
  card_instance_t *instance;
  card_instance_t *parent;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_UNTAP_PHASE || event == EVENT_RESOLVE_SPELL || event == EVENT_CAN_ACTIVATE)
  {
    return mana_producer_sound_on_resolve(player, card, event, COLOR_COLORLESS);
  }

  if (event == EVENT_ACTIVATE)
  {
    s.result = 0;
    s.choice = 0;
    if (DAT_007aadf0 != 0 && unk_00938e2c == 0)
    {
      s.can_damage = real_target_available((int *)0,
                                           TARGET_SCAN_DIRECT,
                                           player,
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
                                           TARGET_STATE_ATTACKING,
                                           0);
      if (s.can_damage)
      {
        if (player == g_active_player || (g_duel_network_flags & 2) != 0)
        {
          sprintf(s.dialog, " Add colorless mana\n Damage target attacking creature\n Cancel");
          s.choice = do_dialog(player, player, card, -1, -1, s.dialog, 1);
        }
        else
        {
          s.choice = 1;
        }
      }
    }

    if (s.choice == 0)
    {
      s.result = mana_producer_sound_on_resolve(player, card, event, COLOR_COLORLESS);
      instance->number_of_targets = 0;
    }
    else if (s.choice == 1)
    {
      g_produced_mana_color = -1;
      if (!C_real_select_target(player,
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
                                TARGET_STATE_ATTACKING,
                                0,
                                "Select target attacking creature.",
                                1,
                                &s.target))
      {
        g_spell_fizzled = 1;
      }
      else
      {
        instance->targets[0] = s.target;
        instance->number_of_targets = 1;
        instance->state |= STATE_TAPPED;
        undeclare_mana_available(player, COLOR_COLORLESS, 1);
      }
    }
    else
    {
      g_spell_fizzled = 1;
    }
    return s.result;
  }

  if (event == EVENT_RESOLVE_ACTIVATION && instance->number_of_targets != 0)
  {
    s.target = instance->targets[0];
    if (!C_real_validate_target(s.target.player,
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
                                TARGET_STATE_ATTACKING,
                                0))
    {
      g_spell_fizzled = 1;
    }
    else
    {
      create_legacy_effect(g_card_on_stack_controller,
                           g_card_on_stack,
                           DAT_008b3278,
                           s.target.player,
                           s.target.card);
    }
    parent = &PLAYER_CARD_INSTANCE(instance->parent_controller, instance->parent_card);
    parent->number_of_targets = 0;
    return 0;
  }

  if (event == EVENT_COUNT_MANA)
  {
    return mana_producer_sound_on_resolve(player, card, event, COLOR_COLORLESS);
  }

  return 0;
}

// FUNCTION: MAGIC 0x00503ef5
// FUNCTION: SHANDALAR 0x00553f7d
int card_diamond_valley(int player, int card, event_t event)
{
  card_instance_t *instance;
  target_t target;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_CAST_SPELL &&
      g_affected_card == card &&
      g_affected_card_controller == player &&
      player == g_other_player)
  {
    g_ai_modifier += 0x30;
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    return (instance->state & STATE_TAPPED) == 0 &&
           !is_animated_and_sick(player, card) &&
           real_target_available((int *)0,
                                 TARGET_SCAN_DIRECT,
                                 player,
                                 player,
                                 player,
                                 TARGET_ZONE_IN_PLAY,
                                 TYPE_CREATURE,
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

  if (event == EVENT_ACTIVATE)
  {
    if (player == g_other_player)
    {
      g_ai_modifier += (g_current_phase == PHASE_DISCARD && g_current_player != player) ? 0x30 : -0x18;
    }

    if (!C_real_select_target(player,
                              player,
                              player,
                              TARGET_ZONE_IN_PLAY,
                              TYPE_CREATURE,
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
                              "Select a creature to sacrifice.",
                              1,
                              &target))
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
      instance->info_slot = C_get_abilities(target.player, target.card, EVENT_TOUGHNESS, -1);
      instance->state |= STATE_TAPPED;
      g_produced_mana_color = -1;
    }
    return 0;
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    gain_life(player, instance->info_slot);
  }

  return 0;
}

// FUNCTION: MAGIC 0x00504340
// FUNCTION: SHANDALAR 0x005543ce
int card_elephant_graveyard(int player, int card, event_t event)
{
  struct
  {
    char dialog[0x384];
    int choice;
    int current_player;
    int result;
    int done;
    int current_card;
  } s;
  card_instance_t *instance;
  card_instance_t *target_instance;
  target_t target;
  int subtype;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_UNTAP_PHASE)
  {
    return mana_producer_sound_on_resolve(player, card, event, COLOR_COLORLESS);
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    instance->info_slot = 0;
    if ((instance->state & STATE_TAPPED) != 0 || is_animated_and_sick(player, card))
    {
      return 0;
    }

    if ((g_land_can_be_played & LCBP_REGENERATION) == 0)
    {
      return 1;
    }

    for (s.current_player = 0; s.current_player < 2; ++s.current_player)
    {
      for (s.current_card = 0;
           s.current_card < g_active_cards_count[s.current_player] && instance->info_slot == 0;
           ++s.current_card)
      {
        target_instance = &PLAYER_CARD_INSTANCE(s.current_player, s.current_card);
        if (is_in_play(s.current_player, s.current_card) &&
            (int)(char)target_instance->kill_code == KILL_DESTROY &&
            (global_cards_data[target_instance->internal_card_id].type & TYPE_CREATURE) != 0)
        {
          subtype = global_raw_cards_storage[global_cards_data[target_instance->internal_card_id].id].subtype;
          if ((subtype == 0x42 || subtype == 0x78) &&
              (player == g_active_player ||
               (g_duel_network_flags & 2) != 0 ||
               g_other_player == s.current_player))
          {
            instance->info_slot = 1;
          }
        }
      }
    }

    return instance->info_slot != 0 ? 99 : 0;
  }

  if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_target(0);
    return 0;
  }

  if (event == EVENT_ACTIVATE)
  {
    s.result = 0;
    s.choice = 0;
    if (DAT_007aadf0 != 0 &&
        instance->info_slot != 0 &&
        (g_land_can_be_played & LCBP_REGENERATION) != 0)
    {
      if (unk_00938e2c == 0)
      {
        if (player == g_active_player || (g_duel_network_flags & 2) != 0)
        {
          sprintf(s.dialog, " Add colorless mana\n Regenerate an Elephant or Mammoth\n Cancel");
          s.choice = do_dialog(player, player, card, -1, -1, s.dialog, 1);
        }
        else
        {
          s.choice = 1;
        }
      }
    }

    if (s.choice == 0)
    {
      s.result = mana_producer_sound_on_resolve(player, card, event, COLOR_COLORLESS);
      instance->number_of_targets = 0;
    }
    else if (s.choice == 1)
    {
      g_produced_mana_color = -1;
      s.done = 0;
      do
      {
        instance->number_of_targets = 0;
        if (!select_target_creature_and_store_without_protection(player, player, card))
        {
          g_spell_fizzled = 1;
        }
        else
        {
          target = instance->targets[0];
          target_instance = &PLAYER_CARD_INSTANCE(target.player, target.card);
          subtype = global_raw_cards_storage[global_cards_data[target_instance->internal_card_id].id].subtype;
          if ((int)(char)target_instance->kill_code == KILL_DESTROY &&
              (subtype == 0x42 || subtype == 0x78))
          {
            if ((target_instance->token_status & STATUS_CANNOT_REGENERATE) != 0 ||
                target.player == g_active_player)
            {
              g_ai_modifier -= 0x30;
            }
            s.done = 1;
            instance->state |= STATE_TAPPED;
            undeclare_mana_available(player, COLOR_COLORLESS, 1);
          }
          else if (g_duel_ai_mode_state == 1)
          {
            rewind_recorded_action();
          }
        }
      } while (g_spell_fizzled != 1 && s.done == 0);
    }
    else
    {
      g_spell_fizzled = 1;
    }

    if (g_spell_fizzled == 1)
    {
      instance->number_of_targets = 0;
    }
    return s.result;
  }

  if (event == EVENT_RESOLVE_ACTIVATION && g_produced_mana_color == -1)
  {
    target = instance->targets[0];
    if (!C_real_validate_target(target.player,
                                target.card,
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
                                -1,
                                -1,
                                -1,
                                0,
                                0,
                                0) ||
        (int)(char)PLAYER_CARD_INSTANCE(target.player, target.card).kill_code != KILL_DESTROY)
    {
      g_spell_fizzled = 1;
    }
    else
    {
      regenerate_card(target.player, target.card);
    }
    instance->number_of_targets = 0;
  }

  if (event == EVENT_COUNT_MANA)
  {
    return mana_producer_sound_on_resolve(player, card, event, COLOR_COLORLESS);
  }

  return 0;
}

// FUNCTION: MAGIC 0x00504ec7
// FUNCTION: SHANDALAR 0x00554f53
int card_island_of_wak_wak(int player, int card, event_t event)
{
  card_instance_t *instance;
  card_instance_t *parent;
  target_t target;
  int legacy_card;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_CAST_SPELL &&
      g_affected_card == card &&
      g_affected_card_controller == player &&
      player == g_other_player)
  {
    g_ai_modifier += 0x30;
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    return (instance->state & STATE_TAPPED) == 0 &&
           !is_animated_and_sick(player, card) &&
           real_target_available((int *)0,
                                 TARGET_SCAN_DIRECT,
                                 player,
                                 2,
                                 1 - player,
                                 TARGET_ZONE_IN_PLAY,
                                 TYPE_CREATURE,
                                 TYPE_NONE,
                                 KEYWORD_FLYING,
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

  if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_target(0);
    return 0;
  }

  if (event == EVENT_ACTIVATE)
  {
    if (!C_real_select_target(player,
                              2,
                              1 - player,
                              TARGET_ZONE_IN_PLAY,
                              TYPE_CREATURE,
                              TYPE_NONE,
                              KEYWORD_FLYING,
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
                              "Select target creature with flying.",
                              1,
                              &target))
    {
      g_spell_fizzled = 1;
    }
    else
    {
      instance->targets[0] = target;
      instance->number_of_targets = 1;
      instance->state |= STATE_TAPPED;
      g_produced_mana_color = -1;
    }
    return 0;
  }

  if (event == EVENT_RESOLVE_ACTIVATION && instance->number_of_targets != 0)
  {
    target = instance->targets[0];
    if (!C_real_validate_target(target.player,
                                target.card,
                                (char *)0,
                                player,
                                2,
                                2,
                                TARGET_ZONE_IN_PLAY,
                                TYPE_CREATURE,
                                TYPE_NONE,
                                KEYWORD_FLYING,
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
      g_spell_fizzled = 1;
    }
    else
    {
      legacy_card = create_legacy_effect(g_card_on_stack_controller,
                                         g_card_on_stack,
                                         LEGACY_EFFECT_PUMP,
                                         target.player,
                                         target.card);
      if (legacy_card != -1)
      {
        PLAYER_CARD_INSTANCE(player, legacy_card).counter_power =
            (short)-C_get_abilities(target.player, target.card, EVENT_POWER, -1);
      }
    }
    parent = &PLAYER_CARD_INSTANCE(instance->parent_controller, instance->parent_card);
    parent->number_of_targets = 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0050535f
// FUNCTION: SHANDALAR 0x005553ee
int card_urza_s_mine(int player, int card, event_t event)
{
  if (event == EVENT_UNTAP_PHASE)
  {
    return mana_producer_sound_on_resolve(player, card, event, COLOR_COLORLESS);
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    return mana_producer_sound_on_resolve(player, card, event, COLOR_COLORLESS);
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    if ((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0 &&
        ((PLAYER_CARD_INSTANCE(player, card).state & (STATE_SUMMONSICK_NOATTACK | STATE_SUMMONSICK_NOTAP)) == 0 ||
         (global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].type & TYPE_CREATURE) == 0))
    {
      return 1;
    }
    else
    {
      return 0;
    }
  }

  if (event == EVENT_ACTIVATE)
  {
    undeclare_mana_available_and_produce_it(player, COLOR_COLORLESS, 1);
    PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
    g_produced_mana_color = COLOR_COLORLESS;
    if (get_urza_land_types_in_play(player) == 7)
    {
      undeclare_mana_available_and_produce_it(player, COLOR_COLORLESS, 1);
    }
  }

  if (event == EVENT_COUNT_MANA && card == g_affected_card && player == g_affected_card_controller)
  {
    if ((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0 &&
        ((PLAYER_CARD_INSTANCE(player, card).state & (STATE_SUMMONSICK_NOATTACK | STATE_SUMMONSICK_NOTAP)) == 0 ||
         (global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].type & TYPE_CREATURE) == 0))
    {
      declare_mana_available(player, COLOR_COLORLESS, 1);
    }
    if (get_urza_land_types_in_play(player) == 7)
    {
      declare_mana_available(player, COLOR_COLORLESS, 1);
    }
  }
  return 0;
}

// FUNCTION: MAGIC 0x00505608
// FUNCTION: SHANDALAR 0x00555699
int card_urza_s_tower(int player, int card, event_t event)
{
  if (event == EVENT_UNTAP_PHASE)
  {
    return mana_producer_sound_on_resolve(player, card, event, COLOR_COLORLESS);
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    return mana_producer_sound_on_resolve(player, card, event, COLOR_COLORLESS);
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    if ((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0 &&
        ((PLAYER_CARD_INSTANCE(player, card).state & (STATE_SUMMONSICK_NOATTACK | STATE_SUMMONSICK_NOTAP)) == 0 ||
         (global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].type & TYPE_CREATURE) == 0))
    {
      return 1;
    }
    else
    {
      return 0;
    }
  }

  if (event == EVENT_ACTIVATE)
  {
    undeclare_mana_available_and_produce_it(player, COLOR_COLORLESS, 1);
    PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
    g_produced_mana_color = COLOR_COLORLESS;
    if (get_urza_land_types_in_play(player) == 7)
    {
      undeclare_mana_available_and_produce_it(player, COLOR_COLORLESS, 2);
    }
  }

  if (event == EVENT_COUNT_MANA && card == g_affected_card && player == g_affected_card_controller)
  {
    if ((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0 &&
        ((PLAYER_CARD_INSTANCE(player, card).state & (STATE_SUMMONSICK_NOATTACK | STATE_SUMMONSICK_NOTAP)) == 0 ||
         (global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].type & TYPE_CREATURE) == 0))
    {
      declare_mana_available(player, COLOR_COLORLESS, 1);
    }
    if (get_urza_land_types_in_play(player) == 7)
    {
      declare_mana_available(player, COLOR_COLORLESS, 2);
    }
  }
  return 0;
}

// FUNCTION: MOK 0x004a9310
// FUNCTION: MAGIC 0x005058b1
// FUNCTION: SHANDALAR 0x00555944
int helper_dual_land(int player, int card, event_t event, color_test_t available_colors)
{
  int color;
  int first_available_color;
  int num_available_colors;
  color_test_t colors_to_choose_from;

  if (event == EVENT_TAP_CARD && g_duel_ai_mode_state != 1)
  {
    play_sound_effect(PLAYER_CARD_INSTANCE(player, card).internal_card_id + 0x16);
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    if ((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0 && ((PLAYER_CARD_INSTANCE(player, card).state & (STATE_SUMMONSICK_NOATTACK | STATE_SUMMONSICK_NOTAP)) == 0 || (global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].type & TYPE_CREATURE) == 0))
    {
      return 1;
    }
    return 0;
  }

  if (event == EVENT_ACTIVATE)
  {
    if (unk_00938e2c == 1 || unk_00938e2c == 0x40 || unk_00938e2c == 0x41)
    {
      color = -1;
      first_available_color = 0;
      while (first_available_color < 7 && color == -1)
      {
        if ((available_colors & (1 << ((unsigned char)first_available_color & 0x1f))) != 0)
        {
          color = first_available_color;
        }
        ++first_available_color;
      }
    }
    else
    {
      if (unk_00938e2c == 0)
      {
        colors_to_choose_from = available_colors;
      }
      else
      {
        colors_to_choose_from = available_colors & unk_00938e2c;
      }

      first_available_color = -1;
      num_available_colors = 0;
      for (color = 0; color < 7; ++color)
      {
        if ((colors_to_choose_from & (1 << ((unsigned char)color & 0x1f))) != COLOR_TEST_0)
        {
          ++num_available_colors;
          if (first_available_color == -1)
          {
            first_available_color = color;
          }
        }
      }

      if (player == g_other_player && (g_duel_network_flags & 2) == 0)
      {
        color = first_available_color;
      }
      else if (num_available_colors == 1)
      {
        color = choose_a_color(player, g_text_lines[0], 1, -1, colors_to_choose_from);
      }
      else
      {
        color = choose_a_color(player, g_text_lines[0], 1, -1, available_colors);
      }
    }

    if (color == -1)
    {
      g_spell_fizzled = 1;
    }
    else
    {
      produce_mana(player, color, 1);
      undeclare_mana_available_hex(player, color, 1);
      PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
      g_produced_mana_color = color;

      if (player == g_other_player && (g_duel_network_flags & 2) == 0)
      {
        load_text("promptsX1.txt", "MULTI_LANDS");
        switch (color)
        {
        case COLOR_BLACK:
          do_dialog(player, player, card, -1, -1, g_text_lines[0], 0);
          break;

        case COLOR_BLUE:
          do_dialog(player, player, card, -1, -1, g_text_lines[1], 0);
          break;

        case COLOR_GREEN:
          do_dialog(player, player, card, -1, -1, g_text_lines[2], 0);
          break;

        case COLOR_RED:
          do_dialog(player, player, card, -1, -1, g_text_lines[3], 0);
          break;

        case COLOR_WHITE:
          do_dialog(player, player, card, -1, -1, g_text_lines[4], 0);
          break;
        }
      }
    }
  }

  if (event == EVENT_COUNT_MANA && card == g_affected_card && player == g_affected_card_controller && ((PLAYER_CARD_INSTANCE(player, card).state & (STATE_SUMMONSICK_NOATTACK | STATE_SUMMONSICK_NOTAP)) == 0 || (global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].type & TYPE_CREATURE) == 0) && (PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0)
  {
    declare_mana_available_hex(player, available_colors, 1);
  }

  return 0;
}

// FUNCTION: MAGIC 0x00505dd2
// FUNCTION: SHANDALAR 0x00555e68
int card_badlands(int player, int card, event_t event)
{
  if (event == EVENT_ACTIVATE)
  {
    load_text("promptsX1.txt", "BADLANDS");
  }

  return helper_dual_land(player, card, event, PLAYER_CARD_INSTANCE(player, card).mana_color);
}

// FUNCTION: MAGIC 0x00505e42
// FUNCTION: SHANDALAR 0x00555ed8
int card_bayou(int player, int card, event_t event)
{
  if (event == EVENT_ACTIVATE)
  {
    load_text("promptsX1.txt", "BAYOU");
  }

  return helper_dual_land(player, card, event, PLAYER_CARD_INSTANCE(player, card).mana_color);
}

// FUNCTION: MAGIC 0x00505eb2
// FUNCTION: SHANDALAR 0x00555f48
int card_plateau(int player, int card, event_t event)
{
  if (event == EVENT_ACTIVATE)
  {
    load_text("promptsX1.txt", "PLATEAU");
  }

  return helper_dual_land(player, card, event, PLAYER_CARD_INSTANCE(player, card).mana_color);
}

// FUNCTION: MAGIC 0x00505f22
// FUNCTION: SHANDALAR 0x00555fb8
int card_savannah(int player, int card, event_t event)
{
  if (event == EVENT_ACTIVATE)
  {
    load_text("promptsX1.txt", "SAVANNAH");
  }

  return helper_dual_land(player, card, event, PLAYER_CARD_INSTANCE(player, card).mana_color);
}

// FUNCTION: MAGIC 0x00505f92
// FUNCTION: SHANDALAR 0x00556028
int card_scrubland(int player, int card, event_t event)
{
  if (event == EVENT_ACTIVATE)
  {
    load_text("promptsX1.txt", "SCRUBLAND");
  }

  return helper_dual_land(player, card, event, PLAYER_CARD_INSTANCE(player, card).mana_color);
}

// FUNCTION: MAGIC 0x00506002
// FUNCTION: SHANDALAR 0x00556098
int card_taiga(int player, int card, event_t event)
{
  if (event == EVENT_ACTIVATE)
  {
    load_text("promptsX1.txt", "TAIGA");
  }

  return helper_dual_land(player, card, event, PLAYER_CARD_INSTANCE(player, card).mana_color);
}

// FUNCTION: MAGIC 0x00506072
// FUNCTION: SHANDALAR 0x00556108
int card_tropical_island(int player, int card, event_t event)
{
  if (event == EVENT_ACTIVATE)
  {
    load_text("promptsX1.txt", "TROPICAL_ISLAND");
  }

  return helper_dual_land(player, card, event, PLAYER_CARD_INSTANCE(player, card).mana_color);
}

// FUNCTION: MAGIC 0x005060e2
// FUNCTION: SHANDALAR 0x00556178
int card_tundra(int player, int card, event_t event)
{
  if (event == EVENT_ACTIVATE)
  {
    load_text("promptsX1.txt", "TUNDRA");
  }

  return helper_dual_land(player, card, event, PLAYER_CARD_INSTANCE(player, card).mana_color);
}

// FUNCTION: MAGIC 0x00506152
// FUNCTION: SHANDALAR 0x005561e8
int card_underground_sea(int player, int card, event_t event)
{
  if (event == EVENT_ACTIVATE)
  {
    load_text("promptsX1.txt", "UNDERGROUND_SEA");
  }

  return helper_dual_land(player, card, event, PLAYER_CARD_INSTANCE(player, card).mana_color);
}

// FUNCTION: MAGIC 0x005061c2
// FUNCTION: SHANDALAR 0x00556258
int card_volcanic_island(int player, int card, event_t event)
{
  if (event == EVENT_ACTIVATE)
  {
    load_text("promptsX1.txt", "VOLCANIC_ISLAND");
  }

  return helper_dual_land(player, card, event, PLAYER_CARD_INSTANCE(player, card).mana_color);
}
