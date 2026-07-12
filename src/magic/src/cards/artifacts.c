#include <string.h>
#include "../cards.h"
#include "../game_support.h"
#include "../global_strings.h"

// FUNCTION: MAGIC 0x0040cfa0
// FUNCTION: SHANDALAR 0x0050f7c0
int card_mox_emerald(int player, int card, event_t event)
{
  return helper_mox(player, card, event, COLOR_GREEN);
}

// FUNCTION: MAGIC 0x0040cfc6
// FUNCTION: SHANDALAR 0x0050f7e6
int card_mox_jet(int player, int card, event_t event)
{
  return helper_mox(player, card, event, COLOR_BLACK);
}

// FUNCTION: MAGIC 0x0040cfec
// FUNCTION: SHANDALAR 0x0050f80c
int card_mox_pearl(int player, int card, event_t event)
{
  return helper_mox(player, card, event, COLOR_WHITE);
}

// FUNCTION: MAGIC 0x0040d012
// FUNCTION: SHANDALAR 0x0050f832
int card_mox_ruby(int player, int card, event_t event)
{
  return helper_mox(player, card, event, COLOR_RED);
}

// FUNCTION: MAGIC 0x0040d038
// FUNCTION: SHANDALAR 0x0050f858
int card_mox_sapphire(int player, int card, event_t event)
{
  return helper_mox(player, card, event, COLOR_BLUE);
}

// FUNCTION: MAGIC 0x0040d05e
// FUNCTION: SHANDALAR 0x0050f87e
int card_black_lotus(int player, int card, event_t event)
{
  struct
  {
    color_t choice_hint;
    color_t color;
    unsigned int available_colors;
  } s;

  if (event == EVENT_CAN_ACTIVATE)
  {
    return CAN_TAP(player, card);
  }

  if (event == EVENT_ACTIVATE)
  {
    ai_modifier -= 0x24;
    s.available_colors = PLAYER_CARD_INSTANCE(player, card).mana_color;
    if ((((active_player == player) && ((g_duel_network_flags & 2) == 0)) || (g_duel_ai_mode_state == 1)) || (g_duel_network_state != 0))
    {
      s.choice_hint = ~COLOR_COLORLESS;
      for (s.color = COLOR_BLACK; s.color <= COLOR_WHITE && s.choice_hint == ~COLOR_COLORLESS; s.color += COLOR_BLACK)
      {
        if (((unk_00938e2c & (1 << (unsigned char)s.color)) != 0) && ((s.available_colors & (1 << (unsigned char)s.color)) != 0))
        {
          s.choice_hint = s.color;
        }
      }
      if (s.choice_hint == ~COLOR_COLORLESS && (unk_00938e2c & 1) != 0)
      {
        s.choice_hint = COLOR_BLACK;
      }
      if (s.choice_hint == ~COLOR_COLORLESS && (unk_00938e2c & 0x40) != 0)
      {
        s.choice_hint = COLOR_BLACK;
      }
      if (s.choice_hint == ~COLOR_COLORLESS)
      {
        spell_fizzled = 1;
      }
    }
    else
    {
      s.choice_hint = ~COLOR_COLORLESS;
    }

    if (spell_fizzled != 1)
    {
      if (g_duel_ai_mode_state != 1)
      {
        load_text("prompts.txt", "BLACK_LOTUS");
      }
      s.choice_hint = choose_a_color(player, text_lines[0], 1, s.choice_hint, PLAYER_CARD_INSTANCE(player, card).mana_color);
      if (s.choice_hint == ~COLOR_COLORLESS)
      {
        spell_fizzled = 1;
      }
      else
      {
        produce_mana(player, s.choice_hint, 3);
        produced_mana_color = s.choice_hint;
        PLAYER_CARD_INSTANCE(player, card).state |= 0x10;
        if (((active_player == player) && ((g_duel_network_flags & 2) == 0)) && (g_duel_ai_mode_state != 1))
        {
          char message[300];
          load_text("prompts.txt", "BLACK_LOTUS");
          if (s.choice_hint == COLOR_BLACK)
          {
            strcpy(message, text_lines[1]);
          }
          else if (s.choice_hint == COLOR_BLUE)
          {
            strcpy(message, text_lines[2]);
          }
          else if (s.choice_hint == COLOR_GREEN)
          {
            strcpy(message, text_lines[3]);
          }
          else if (s.choice_hint == COLOR_RED)
          {
            strcpy(message, text_lines[4]);
          }
          else
          {
            strcpy(message, text_lines[5]);
          }
          do_dialog(player, player, card, -1, -1, message, 0);
        }
        if (g_duel_ai_mode_state != 1)
        {
          play_sound_effect(WAV_SACRFICE);
        }
        kill_card(player, card, KILL_SACRIFICE);
        unk_007a7c1c = 1;
      }
    }
  }

  if ((event == EVENT_COUNT_MANA) && (card == affected_card) && (player == affected_card_controller) && ((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0))
  {
    declare_mana_available_hex(player, PLAYER_CARD_INSTANCE(player, card).mana_color, 3);
  }

  return 0;
}

// FUNCTION: MAGIC 0x0040d52f
// FUNCTION: SHANDALAR 0x0050fd4d
int card_time_vault(int player, int card, event_t event)
{
  int effect_found;
  char prompt[600];
  int effect_card;
  int current_player;
  int current_card;

  if (event == 0x82 && PLAYER_CARD_INSTANCE(player, card).info_slot != 0)
  {
    PLAYER_CARD_INSTANCE(player, card).untap_status &= ~3;
  }

  if (event == EVENT_CAST_SPELL && affected_card == card && affected_card_controller == player)
  {
    ai_modifier += 0x18;
    PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
    PLAYER_CARD_INSTANCE(player, card).info_slot = 1;
  }

  if (event == 0x6a && human_player == player && (PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) != 0 && C_get_special_counters(player, card) == 0)
  {
    load_text("prompts.txt", "TIME_VAULT");
    sprintf(prompt, " %s\n %s", text_lines, text_lines + 0x12c);
    if (do_dialog(player, player, card, -1, -1, prompt, internal_rand(5) < 1) != 0)
    {
      unk_008b4278 |= 0x8000;
      PLAYER_CARD_INSTANCE(player, card).state &= ~STATE_TAPPED;
      add_special_counter(player, card);
      PLAYER_CARD_INSTANCE(player, card).info_slot = 0;
    }
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    if ((PLAYER_CARD_INSTANCE(player, card).untap_status & 3) != 0 && (global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].type & TYPE_CREATURE) != 0)
    {
      return 0;
    }
    if ((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) != 0)
    {
      return 0;
    }
    if (C_get_special_counters(player, card) == 0)
    {
      return 0;
    }
    return 1;
  }

  if (event == EVENT_ACTIVATE && (PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0 && C_get_special_counters(player, card) != 0)
  {
    PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    if (rand() % 5 < 1)
    {
      ai_modifier += 0x30;
    }

    if (g_duel_extra_turn_player == -1)
    {
      effect_found = 0;
      for (current_player = 0; current_player < 2 && !effect_found; ++current_player)
      {
        for (current_card = 0; current_card < active_cards_count[current_player]; ++current_card)
        {
          if (PLAYER_CARD_INSTANCE(current_player, current_card).internal_card_id == unk_008b49c4 && (PLAYER_CARD_INSTANCE(current_player, current_card).token_status & 0x100) != 0)
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

    FUN_00551334(card_on_stack_controller, card_on_stack);
    PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                         PLAYER_CARD_INSTANCE(player, card).parent_card)
        .info_slot = 1;
    effect_card = create_legacy_effect(card_on_stack_controller, card_on_stack, unk_008b49c4, -1, -1);
    if (effect_card != -1)
    {
      PLAYER_CARD_INSTANCE(player, effect_card).token_status |= 0x120;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0040db0e
// FUNCTION: SHANDALAR 0x0051032b
int card_black_vise(int player, int card, event_t event)
{
  struct
  {
    int damaged_player;
    int damage;
    int internal_card_id;
    int tmp;
  } s;

  if (event == EVENT_SHOULD_AI_PLAY)
  {
    if (is_in_play(player, card))
    {
      if ((((unsigned char *)&PLAYER_CARD_INSTANCE(player, card).state)[1] & 0x10) == 0)
      {
        s.damaged_player = active_player;
      }
      else
      {
        s.damaged_player = unk_008b35ec;
      }

      s.damage = hand_count[s.damaged_player] - 4;
      if (s.damage < 1)
      {
        s.damage = 0;
      }

      if (s.damage != 0)
      {
        s.tmp = life[s.damaged_player] / s.damage;
        s.damage = 0x18 - s.tmp;
        if (s.damage < 2)
        {
          s.damage = 1;
        }
        if (s.damaged_player == 0)
        {
          ai_modifier += (s.damage * 3) << 3;
        }
        else
        {
          ai_modifier -= (s.damage * 3) << 3;
        }
      }
    }
  }

  if (trigger_condition == TRIGGER_END_UPKEEP && card == affected_card && player == affected_card_controller &&
      player == current_turn)
  {
    if ((((unsigned char *)&PLAYER_CARD_INSTANCE(player, card).state)[1] & 0x10) != 0)
    {
      s.internal_card_id = PLAYER_CARD_INSTANCE(player, card).internal_card_id;
      if ((global_cards_data[s.internal_card_id].type & TYPE_CREATURE) == 0)
      {
        return 0;
      }
    }

    if ((((unsigned char *)&PLAYER_CARD_INSTANCE(player, card).state)[1] & 0x10) == 0)
    {
      s.damaged_player = active_player;
    }
    else
    {
      s.damaged_player = unk_008b35ec;
    }

    if (human_player == s.damaged_player && hand_count[s.damaged_player] > 4)
    {
      if (event == EVENT_TRIGGER)
      {
        event_result |= 2;
      }
      if (event == EVENT_RESOLVE_TRIGGER)
      {
        damage_player(s.damaged_player, hand_count[s.damaged_player] - 4, player, card);
      }
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0040dd78
// FUNCTION: SHANDALAR 0x00510594
int card_the_rack(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x0040dfe6
// FUNCTION: SHANDALAR 0x00510802
int card_ivory_tower(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x0040e1a7
// FUNCTION: SHANDALAR 0x005109c7
int card_cursed_rack(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x0040e2db
// FUNCTION: SHANDALAR 0x00510afa
int card_aladdin_s_lamp(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x0040eb7d
// FUNCTION: SHANDALAR 0x00511395
int card_feldon_s_cane(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x0040ed84
// FUNCTION: SHANDALAR 0x0051159c
int card_mishra_s_war_machine(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x0040efdc
// FUNCTION: SHANDALAR 0x005117f9
int card_primal_clay(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x0040f3a6
// FUNCTION: SHANDALAR 0x00511bc3
int card_shapeshifter(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x0040fb86
// FUNCTION: SHANDALAR 0x0051239f
int card_tetravus(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x00410756
// FUNCTION: SHANDALAR 0x00512f6e
int card_tetravite(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x004107ff
// FUNCTION: SHANDALAR 0x00513019
int card_triskelion(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x00410a22
// FUNCTION: SHANDALAR 0x0051323c
int card_urza_s_avenger(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x00411509
// FUNCTION: SHANDALAR 0x00513d22
int card_millstone(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x00411981
// FUNCTION: SHANDALAR 0x0051419a
int card_celestial_prism(int player, int card, event_t event)
{
  char message[300];
  color_t color;

  if (event == EVENT_CAN_ACTIVATE)
  {
    if (has_mana(player, 7, 2) && CAN_TAP(player, card))
    {
      return 1;
    }
    return 0;
  }

  if ((event == EVENT_ACTIVATE) && has_mana(player, 7, 2))
  {
    ai_modifier -= 0x18;
    charge_mana(player, 0, 2);
    if (spell_fizzled != 1)
    {
      if ((active_player == player) && ((g_duel_network_flags & 2) == 0))
      {
        if (g_duel_ai_mode_state == 1)
        {
          unk_00939340 = unk_008cc840 % 5 + 1;
          color = unk_00939340;
          FUN_004e4f11();
        }
        else
        {
          FUN_004e5089();
          if (unk_00939340 < 6)
          {
            color = unk_00939340;
          }
          else
          {
            spell_fizzled = 1;
            color = ~COLOR_COLORLESS;
          }
        }
      }
      else
      {
        color = -1;
      }

      if (spell_fizzled != 1)
      {
        if (g_duel_ai_mode_state != 1)
        {
          load_text("prompts.txt", "CELESTIAL_PRISM");
        }
        color = choose_a_color(player, text_lines[0], 1, color, PLAYER_CARD_INSTANCE(player, card).damage_source_card);
        if (color == ~COLOR_COLORLESS)
        {
          spell_fizzled = 1;
        }
        else if (spell_fizzled != 1)
        {
          produce_mana(player, color, 1);
          undeclare_mana_available_hex(player, PLAYER_CARD_INSTANCE(player, card).damage_source_card, 1);
          PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
          produced_mana_color = color;
          if (((active_player == player) && ((g_duel_network_flags & 2) == 0)) && (g_duel_ai_mode_state != 1))
          {
            load_text("prompts.txt", "CELESTIAL_PRISM2");
            if (color == COLOR_BLACK)
            {
              strcpy(message, text_lines[1]);
            }
            else if (color == COLOR_BLUE)
            {
              strcpy(message, text_lines[2]);
            }
            else if (color == COLOR_GREEN)
            {
              strcpy(message, text_lines[3]);
            }
            else if (color == COLOR_RED)
            {
              strcpy(message, text_lines[4]);
            }
            else
            {
              strcpy(message, text_lines[5]);
            }
            do_dialog(player, player, card, -1, -1, message, 0);
          }
        }
      }
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x00411d7a
// FUNCTION: SHANDALAR 0x00514591
int card_fellwar_stone(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x0041240b
// FUNCTION: SHANDALAR 0x00514c20
int card_ashnod_s_battle_gear(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x00412e9c
// FUNCTION: SHANDALAR 0x005156b0
int card_tawnos_s_weaponry(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x00413c87
// FUNCTION: SHANDALAR 0x0051649a
int card_battering_ram(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x00413eb8
// FUNCTION: SHANDALAR 0x005166cc
int card_candelabra_of_tawnos(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x0041465a
// FUNCTION: SHANDALAR 0x00516e6e
int card_clay_statue(int player, int card, event_t event)
{
  if (event == EVENT_CAN_ACTIVATE || event == EVENT_ACTIVATE || event == EVENT_RESOLVE_ACTIVATION)
  {
    return FUN_0054276d(player, card, event, 0, 2);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004146a7
// FUNCTION: SHANDALAR 0x00516ebb
int card_diabolic_machine(int player, int card, event_t event)
{
  if (event == EVENT_CAN_ACTIVATE || event == EVENT_ACTIVATE || event == EVENT_RESOLVE_ACTIVATION)
  {
    return FUN_0054276d(player, card, event, 0, 3);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004146f4
// FUNCTION: SHANDALAR 0x00516f08
int card_forcefield(int player, int card, event_t event)
{
  card_instance_t *instance;
  target_t target;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_CAN_ACTIVATE)
  {
    if (((instance->state & 0x10) != 0 && (global_cards_data[instance->internal_card_id].type & TYPE_CREATURE) == 0) || (unk_008b4278 & 4) == 0 || (current_phase != 0x19 && current_phase != 0x1a) || has_mana(player, COLOR_COLORLESS, 1) == 0)
    {
      return 0;
    }
    return 99;
  }

  if (event == 0x90)
  {
    FUN_004e4ff3(0);
    return 0;
  }

  if (event == EVENT_ACTIVATE)
  {
    if (has_mana(player, COLOR_COLORLESS, 1) != 0 && (unk_008b4278 & 4) != 0 && (current_phase == 0x19 || current_phase == 0x1a))
    {
      charge_mana(player, COLOR_COLORLESS, 1);
      if (g_duel_ai_mode_state != 1)
      {
        load_text("prompts.txt", "FORCEFIELD");
      }
      if (!C_real_select_target(player,
                                player,
                                player,
                                TARGET_ZONE_IN_PLAY,
                                TYPE_NONE,
                                TYPE_NONE,
                                0,
                                0,
                                COLOR_TEST_0,
                                COLOR_TEST_0,
                                unk_009266a4,
                                ~SUB_WALL,
                                -1,
                                -1,
                                TARGET_SPECIAL_DAMAGE_PLAYER,
                                0,
                                0,
                                text_lines[0],
                                1,
                                &target))
      {
        spell_fizzled = 1;
      }
      else if ((global_cards_data[PLAYER_CARD_INSTANCE(*((char *)&PLAYER_CARD_INSTANCE(target.player, target.card) + 8),
                                                       *((int *)((char *)&PLAYER_CARD_INSTANCE(target.player, target.card) + 0x44)))
                                      .internal_card_id]
                    .type &
                TYPE_CREATURE) != 0 &&
               ((*((unsigned char *)&PLAYER_CARD_INSTANCE(*((char *)&PLAYER_CARD_INSTANCE(target.player, target.card) + 8),
                                                          *((int *)((char *)&PLAYER_CARD_INSTANCE(target.player, target.card) + 0x44))) +
                   5) &
                 2) == 0))
      {
        instance->targets[0] = target;
        *((char *)instance + 0x32) = 1;
      }
      else
      {
        spell_fizzled = 1;
      }
    }
    return 0;
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    *((char *)&PLAYER_CARD_INSTANCE(card_on_stack_controller, card_on_stack) + 0x32) = 0;
    if (C_real_validate_target(instance->targets[0].player,
                               instance->targets[0].card,
                               (char *)0,
                               player,
                               player,
                               player,
                               TARGET_ZONE_IN_PLAY,
                               TYPE_NONE,
                               TYPE_NONE,
                               0,
                               0,
                               COLOR_TEST_0,
                               COLOR_TEST_0,
                               unk_009266a4,
                               ~SUB_WALL,
                               -1,
                               -1,
                               0,
                               0,
                               0))
    {
      if (PLAYER_CARD_INSTANCE(instance->targets[0].player, instance->targets[0].card).info_slot != 0)
      {
        PLAYER_CARD_INSTANCE(instance->targets[0].player, instance->targets[0].card).info_slot = 1;
      }
    }
    else
    {
      spell_fizzled = 1;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x00414cec
// FUNCTION: SHANDALAR 0x00517500
int card_disrupting_scepter(int player, int card, event_t event)
{
  target_t target;

  if (event == EVENT_CAN_ACTIVATE)
  {
    if (has_mana(player, 7, 3) && player == human_player && CAN_TAP(player, card))
    {
      return 1;
    }
    return 0;
  }

  if ((event == EVENT_ACTIVATE) && ((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0) && has_mana(player, 7, 3) && player == human_player)
  {
    charge_mana(player, 0, 3);
    if (spell_fizzled != 1)
    {
      if (g_duel_ai_mode_state != 1)
      {
        load_text("prompts.txt", "DISRUPTING_SCEPTER");
      }
      if (!C_real_select_target(player, 2, 1 - player, TARGET_ZONE_PLAYERS, TYPE_NONE, TYPE_NONE,
                                0, 0, COLOR_TEST_0, COLOR_TEST_0, -1, ~SUB_WALL, -1, -1,
                                0, 0, 0, text_lines[0], 1, &target))
      {
        spell_fizzled = 1;
      }
      else
      {
        PLAYER_CARD_INSTANCE(player, card).targets[0].player = target.player;
        PLAYER_CARD_INSTANCE(player, card).targets[0].card = target.card;
        PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
        PLAYER_CARD_INSTANCE(player, card).state |= 0x10;
      }
    }
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    discard(PLAYER_CARD_INSTANCE(player, card).targets[0].player, 0, 0);
    PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller, PLAYER_CARD_INSTANCE(player, card).parent_card).number_of_targets = 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x00415072
// FUNCTION: SHANDALAR 0x00517888
int card_howling_mine(int player, int card, event_t event)
{
  // TODO: why does this check if it's a creature?
  if (event == EVENT_DRAW_PHASE && (((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0) || (global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].type & TYPE_CREATURE)))
  {
    ++event_result;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0041510c
// FUNCTION: SHANDALAR 0x00517922
int card_blue_mana_battery(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x00415132
// FUNCTION: SHANDALAR 0x00517948
int card_red_mana_battery(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x00415158
// FUNCTION: SHANDALAR 0x0051796e
int card_green_mana_battery(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x0041517e
// FUNCTION: SHANDALAR 0x00517994
int card_white_mana_battery(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x004151a4
// FUNCTION: SHANDALAR 0x005179ba
int card_black_mana_battery(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x0041598f
// FUNCTION: SHANDALAR 0x005181a3
int card_conservator(int player, int card, event_t event)
{
  card_instance_t *instance;
  target_t selected_target;
  int target_index;
  int stop_selecting;
  int invalid_targets;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_SHOULD_AI_PLAY && affected_card == card && affected_card_controller == player)
  {
    ai_modifier += basiclandtypes_controlled[player][7] * 6;
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    if ((unk_008b4278 & 4) == 0 || is_animated_and_sick(player, card) || (instance->state & STATE_TAPPED) || !has_mana_w_global_cost_mod(player, card, COLOR_COLORLESS, 3) || !real_target_available((int *)0, TARGET_SCAN_DIRECT, player, player, player, TARGET_ZONE_IN_PLAY, TYPE_NONE, TYPE_NONE, 0, 0, COLOR_TEST_0, COLOR_TEST_0, unk_009266a4, ~SUB_WALL, -1, -1, TARGET_SPECIAL_DAMAGE_PLAYER, 0, 0))
    {
      return 0;
    }
    return 99;
  }

  if (event == EVENT_GET_SELECTED_CARD)
  {
    FUN_004e4ff3(0);
    return 0;
  }

  if (event == EVENT_ACTIVATE && (instance->state & STATE_TAPPED) == 0)
  {
    charge_mana_w_global_cost_mod(player, card, COLOR_COLORLESS, 3);
    if (spell_fizzled != 1)
    {
      instance->number_of_targets = 0;
      target_index = 0;
      stop_selecting = 0;
      while (target_index < 2 && stop_selecting == 0 && spell_fizzled != 1)
      {
        if (g_duel_ai_mode_state != 1)
        {
          load_text("prompts.txt", "CONSERVATOR");
        }
        if (!C_real_select_target(player,
                                  player,
                                  player,
                                  TARGET_ZONE_IN_PLAY,
                                  TYPE_NONE,
                                  TYPE_NONE,
                                  0,
                                  0,
                                  COLOR_TEST_0,
                                  COLOR_TEST_0,
                                  unk_009266a4,
                                  ~SUB_WALL,
                                  -1,
                                  -1,
                                  TARGET_SPECIAL_DAMAGE_PLAYER,
                                  0,
                                  0,
                                  text_lines[0],
                                  3,
                                  &selected_target))
        {
          if (selected_target.card == -1)
          {
            spell_fizzled = 1;
          }
          else
          {
            stop_selecting = 1;
          }
        }
        else
        {
          PLAYER_CARD_INSTANCE(selected_target.player, selected_target.card).state |= 0x200000;
          TENTATIVE_reassess_all_cards();
          instance->targets[instance->number_of_targets] = selected_target;
          ++instance->number_of_targets;
        }
        ++target_index;
      }

      for (target_index = 0; target_index < instance->number_of_targets; ++target_index)
      {
        PLAYER_CARD_INSTANCE(instance->targets[target_index].player, instance->targets[target_index].card).state &=
            ~0x200000;
      }

      if (spell_fizzled == 1)
      {
        instance->number_of_targets = 0;
      }
      else
      {
        instance->state |= STATE_TAPPED;
      }
    }
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    invalid_targets = 0;
    for (target_index = 0; target_index < instance->number_of_targets; ++target_index)
    {
      selected_target = instance->targets[target_index];
      if (!C_real_validate_target(selected_target.player,
                                  selected_target.card,
                                  (char *)0,
                                  player,
                                  player,
                                  player,
                                  TARGET_ZONE_IN_PLAY,
                                  TYPE_NONE,
                                  TYPE_NONE,
                                  0,
                                  0,
                                  COLOR_TEST_0,
                                  COLOR_TEST_0,
                                  unk_009266a4,
                                  ~SUB_WALL,
                                  -1,
                                  -1,
                                  TARGET_SPECIAL_DAMAGE_PLAYER,
                                  0,
                                  0))
      {
        ++invalid_targets;
      }
      else if (PLAYER_CARD_INSTANCE(selected_target.player, selected_target.card).info_slot != 0)
      {
        --PLAYER_CARD_INSTANCE(selected_target.player, selected_target.card).info_slot;
      }
    }

    if (instance->number_of_targets == invalid_targets)
    {
      spell_fizzled = 1;
    }

    PLAYER_CARD_INSTANCE(instance->parent_controller, instance->parent_card).number_of_targets = 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x004161ad
// FUNCTION: SHANDALAR 0x005189c3
int card_iron_star(int player, int card, event_t event)
{
  return helper_lucky_charm(player, card, event, 4);
}

// FUNCTION: MAGIC 0x004161d3
// FUNCTION: SHANDALAR 0x005189e9
int card_ivory_cup(int player, int card, event_t event)
{
  return helper_lucky_charm(player, card, event, 5);
}

// FUNCTION: MAGIC 0x004161f9
// FUNCTION: SHANDALAR 0x00518a0f
int card_crystal_rod(int player, int card, event_t event)
{
  return helper_lucky_charm(player, card, event, 2);
}

// FUNCTION: MAGIC 0x0041621f
// FUNCTION: SHANDALAR 0x00518a35
int card_throne_of_bone(int player, int card, event_t event)
{
  return helper_lucky_charm(player, card, event, 1);
}

// FUNCTION: MAGIC 0x00416245
// FUNCTION: SHANDALAR 0x00518a5b
int card_wooden_sphere(int player, int card, event_t event)
{
  return helper_lucky_charm(player, card, event, 3);
}

// FUNCTION: MOK 0x00423dc0
// FUNCTION: MAGIC 0x0041626b
// FUNCTION: SHANDALAR 0x00518a81
int helper_lucky_charm(int player, int card, int event, int color)
{
  if (event == EVENT_CAST_SPELL && affected_card == card && affected_card_controller == player)
  {
    ai_modifier += basiclandtypes_controlled[active_player][color] * 0xc;
  }

  if (trigger_condition == TRIGGER_SPELL_CAST && affected_card == card && affected_card_controller == player && player == current_turn && (((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0) || (global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].type & TYPE_CREATURE)) && is_in_play(player, card) && has_mana_w_global_cost_mod(player, card, COLOR_COLORLESS, 1))
  {
    if (((1 << ((unsigned char)get_sleighted_color(player, card, color) & 0x1f)) & (unsigned int)(unsigned char)PLAYER_CARD_INSTANCE(trigger_cause_controller, trigger_cause).color) != 0 && PLAYER_CARD_INSTANCE(trigger_cause_controller, trigger_cause).internal_card_id != -1 && global_cards_data[PLAYER_CARD_INSTANCE(trigger_cause_controller, trigger_cause).internal_card_id].type != TYPE_LAND)
    {
      if (event == EVENT_TRIGGER)
      {
        if (active_player == player && (g_duel_network_flags & 2) == 0)
        {
          event_result |= 2;
        }
        else
        {
          event_result |= 1;
        }
      }
      if (event == EVENT_RESOLVE_TRIGGER)
      {
        FUN_00443ee2(player, card, EVENT_RESOLVE_ACTIVATION, 0, 0);
        charge_mana(player, COLOR_COLORLESS, 1);
        obliterate_top_card_of_stack();
        if (spell_fizzled != 1)
        {
          dispatch_event(player, card, EVENT_PLAY_ABILITY);
          gain_life(player, 1, player, card);
          if (active_player == player)
          {
            ai_modifier -= 0x18;
          }
        }
      }
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0041653e
// FUNCTION: SHANDALAR 0x00518d54
int card_ankh_of_mishra(int player, int card, event_t event)
{
  if (event == EVENT_CAST_SPELL && affected_card == card && affected_card_controller == player)
  {
    ai_modifier += (basiclandtypes_controlled[active_player][7] - basiclandtypes_controlled[unk_008b35ec][7]) * 0xc;
  }

  if ((trigger_condition == 0xdb || trigger_condition == 0xd3) && affected_card == card && affected_card_controller == player && human_player == current_turn && PLAYER_CARD_INSTANCE(trigger_cause_controller, trigger_cause).internal_card_id != -1 && (global_cards_data[PLAYER_CARD_INSTANCE(trigger_cause_controller, trigger_cause).internal_card_id].type & TYPE_LAND) && (PLAYER_CARD_INSTANCE(player, card).state & 0x20) == 0 && (((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0) || (global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].type & TYPE_CREATURE)))
  {
    if (event == EVENT_TRIGGER)
    {
      event_result |= 2;
    }
    if (event == EVENT_RESOLVE_TRIGGER)
    {
      damage_player(trigger_cause_controller, 2, player, card);
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0041674e
// FUNCTION: SHANDALAR 0x00518f64
int card_armageddon_clock(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x00416dd5
// FUNCTION: SHANDALAR 0x005195e9
int card_dingus_egg(int player, int card, event_t event)
{
  card_instance_t *instance;
  int current_player;
  int damage_count;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_GRAVEYARD_FROM_PLAY && PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).kill_code != 0 && (global_cards_data[PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).internal_card_id].type & TYPE_LAND) && PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).kill_code != 4 && (instance->state & 0x20) == 0 && (((instance->state & STATE_TAPPED) == 0) || (global_cards_data[instance->internal_card_id].type & TYPE_CREATURE)))
  {
    if (affected_card_controller == 0)
    {
      ++instance->info_slot;
    }
    else
    {
      instance->info_slot += 0x100;
    }
  }

  if (trigger_condition == 0xd5 && affected_card == card && affected_card_controller == player && (instance->info_slot & 0xffff) != 0 && player == current_turn && (instance->state & 0x20) == 0 && (((instance->state & STATE_TAPPED) == 0) || (global_cards_data[instance->internal_card_id].type & TYPE_CREATURE)))
  {
    if (event == EVENT_TRIGGER)
    {
      event_result |= 2;
    }
    if (event == EVENT_RESOLVE_TRIGGER)
    {
      for (current_player = 0; current_player < 2; ++current_player)
      {
        if ((instance->info_slot & 0xff) != 0)
        {
          for (damage_count = 0; damage_count < (instance->info_slot & 0xff); ++damage_count)
          {
            damage_player(current_player, 2, player, card);
          }
        }
        instance->info_slot >>= 8;
      }
      instance->info_slot = 0;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x00417228
// FUNCTION: SHANDALAR 0x00519a3d
int card_soul_net(int player, int card, event_t event)
{
  int dummy;

  dummy = 0;
  if (event == EVENT_SHOULD_AI_PLAY && affected_card == card && affected_card_controller == player)
  {
    ai_modifier += 0x90;
  }

  if (event == EVENT_GRAVEYARD_FROM_PLAY && PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).kill_code != 0 && (global_cards_data[PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).internal_card_id].type & TYPE_CREATURE) && (PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).token_status & 0x10) == 0 && PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).kill_code != 4 && (PLAYER_CARD_INSTANCE(player, card).state & 0x20) == 0 && (PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).state & 0x20) == 0 && (((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0) || (global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].type & TYPE_CREATURE)))
  {
    if ((PLAYER_CARD_INSTANCE(player, card).targets[0].player & 1) == 0)
    {
      ++PLAYER_CARD_INSTANCE(player, card).info_slot;
    }
    else
    {
      PLAYER_CARD_INSTANCE(player, card).info_slot = 1;
    }
  }

  if (trigger_condition == 0xd5 && affected_card == card && affected_card_controller == player && PLAYER_CARD_INSTANCE(player, card).info_slot != 0 && player == current_turn && ((((unsigned int)PLAYER_CARD_INSTANCE(player, card).info_slot |= 0x100), ((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0)) || (global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].type & TYPE_CREATURE)))
  {
    if (event == EVENT_TRIGGER)
    {
      if (!has_mana_w_global_cost_mod(player, card, COLOR_COLORLESS, 1))
      {
        PLAYER_CARD_INSTANCE(player, card).info_slot = 0;
      }
      else if (active_player == player && (g_duel_network_flags & 2) == 0)
      {
        event_result |= 2;
      }
      else
      {
        event_result |= 1;
      }
    }
    if (event == EVENT_RESOLVE_TRIGGER)
    {
      FUN_00443ee2(player, card, EVENT_RESOLVE_ACTIVATION, 0, 0);
      charge_mana(player, COLOR_COLORLESS, 1);
      obliterate_top_card_of_stack();
      if (spell_fizzled == 1)
      {
        spell_fizzled = -1;
      }
      else
      {
        dispatch_event(player, card, EVENT_PLAY_ABILITY);
        gain_life(player, 1, player, card);
        --PLAYER_CARD_INSTANCE(player, card).info_slot;
      }
    }
    if (PLAYER_CARD_INSTANCE(player, card).info_slot != 0)
    {
      PLAYER_CARD_INSTANCE(player, card).state &= ~0x100;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x00417809
// FUNCTION: SHANDALAR 0x0051a01c
int card_ebony_horse(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x00417d1f
// FUNCTION: SHANDALAR 0x0051a531
int card_jayemdae_tome(int player, int card, event_t event)
{
  if (event == EVENT_SHOULD_AI_PLAY && affected_card == card && affected_card_controller == player && active_player == player && (g_duel_network_flags & 2) == 0)
  {
    int artifact_count;

    artifact_count = FUN_00404cff(player, PLAYER_CARD_INSTANCE(player, card).internal_card_id, player);
    if (artifact_count != 0 && basiclandtypes_controlled[active_player][7] / artifact_count > 3)
    {
      ai_modifier += 0x30;
    }
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    if (has_mana_w_global_cost_mod(player, card, COLOR_COLORLESS, 4) && !is_animated_and_sick(player, card) && (PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0)
    {
      return 1;
    }
    return 0;
  }

  if (event == EVENT_ACTIVATE && has_mana_w_global_cost_mod(player, card, COLOR_COLORLESS, 4))
  {
    charge_mana_w_global_cost_mod(player, card, COLOR_COLORLESS, 4);
    if (spell_fizzled != 1)
    {
      PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
    }
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    FUN_0043e18b(player);
  }

  return 0;
}

// FUNCTION: MAGIC 0x00417f48
// FUNCTION: SHANDALAR 0x0051a759
int card_mana_vault(int player, int card, event_t event)
{
  int can_activate;

  if (event == EVENT_UNTAP && affected_card == card && affected_card_controller == player)
  {
    PLAYER_CARD_INSTANCE(player, card).untap_status &= ~3;
  }

  if (event == EVENT_CAST_SPELL && affected_card == card && affected_card_controller == player)
  {
    ai_modifier += 0xc;
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    can_activate = 1;
    if (IS_SICK(player, card))
    {
      can_activate = 0;
    }
    else if ((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) != 0 &&
             ((has_mana(player, COLOR_ANY, 4) == 0 || current_phase != 4) || player != human_player))
    {
      can_activate = 0;
    }
    else if (player == active_player && (g_duel_network_flags & 2) == 0 && PLAYER_CARD_INSTANCE(player, card).info_slot != 0)
    {
      can_activate = 0;
    }

    return can_activate;
  }

  if (event == EVENT_ACTIVATE)
  {
    if ((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) != 0)
    {
      if (has_mana(player, COLOR_ANY, 4))
      {
        charge_mana(player, COLOR_COLORLESS, 4);
        if (spell_fizzled != 1)
        {
          PLAYER_CARD_INSTANCE(player, card).eot_toughness = 1;
        }
      }
    }
    else
    {
      undeclare_mana_available_and_produce_it(player, COLOR_COLORLESS, 3);
      produced_mana_color = 0;
      PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
    }
  }

  if (event == EVENT_RESOLVE_ACTIVATION && (PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller, PLAYER_CARD_INSTANCE(player, card).parent_card).state & STATE_TAPPED) != 0 && PLAYER_CARD_INSTANCE(player, card).eot_toughness != 0)
  {
    PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                         PLAYER_CARD_INSTANCE(player, card).parent_card)
        .info_slot = 1;
    PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                         PLAYER_CARD_INSTANCE(player, card).parent_card)
        .eot_toughness = 0;
  }

  if (((trigger_condition == 0xcb) || event == EVENT_SHOULD_AI_PLAY) && affected_card == card && affected_card_controller == player && player == human_player && current_turn == player && (PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) != 0)
  {
    if (event == EVENT_TRIGGER)
    {
      event_result |= 2;
    }
    if (event == EVENT_RESOLVE_TRIGGER || event == EVENT_SHOULD_AI_PLAY)
    {
      if (PLAYER_CARD_INSTANCE(player, card).info_slot != 0)
      {
        PLAYER_CARD_INSTANCE(player, card).state &= ~STATE_TAPPED;
        PLAYER_CARD_INSTANCE(player, card).info_slot = 0;
      }
      else
      {
        damage_player(player, 1, player, card);
      }
    }
  }

  if (event == EVENT_SHOULD_AI_PLAY && basiclandtypes_controlled[player][COLOR_ANY] < 4 && (PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) != 0)
  {
    life[player] -= 4 - basiclandtypes_controlled[player][COLOR_ANY];
  }

  if (event == EVENT_COUNT_MANA && affected_card == card && affected_card_controller == player && (PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0 && (((PLAYER_CARD_INSTANCE(player, card).state & (STATE_SUMMONSICK_NOATTACK | STATE_SUMMONSICK_NOTAP)) == 0) || ((global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].type & TYPE_CREATURE) == 0)))
  {
    declare_mana_available(player, COLOR_COLORLESS, 3);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004186d8
// FUNCTION: SHANDALAR 0x0051aee6
int card_sol_ring(int player, int card, event_t event)
{
  if (event == EVENT_CAST_SPELL && affected_card == card && affected_card_controller == player)
  {
    ai_modifier += 0xc0 / (basiclandtypes_controlled[active_player][7] + 1);
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    return (!is_animated_and_sick(player, card) && (PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0) ? 1 : 0;
  }

  if (event == EVENT_ACTIVATE)
  {
    ai_modifier -= 0xc;
    undeclare_mana_available_and_produce_it(player, COLOR_COLORLESS, 2);
    PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
    produced_mana_color = 0;
  }

  if (event == EVENT_COUNT_MANA && affected_card == card && affected_card_controller == player && (PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0)
  {
    declare_mana_available(player, COLOR_COLORLESS, 2);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004188ba
// FUNCTION: SHANDALAR 0x0051b0c8
int card_meekstone(int player, int card, event_t event)
{
  int current_card;
  int max_cards;
  int ai_delta;

  if (event == EVENT_CAST_SPELL && affected_card == card && affected_card_controller == player && FUN_00404cff(player, PLAYER_CARD_INSTANCE(player, card).internal_card_id, -1) != 0)
  {
    ai_modifier -= 0xf0;
  }

  if (((event == 0x82) && ((global_cards_data[PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).internal_card_id].type & TYPE_CREATURE) != 0)) && ((2 < C_get_abilities(affected_card_controller, affected_card, EVENT_POWER, -1)) && (((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0) && ((global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].type & TYPE_CREATURE) == 0))))
  {
    *(unsigned int *)&PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).dummy3 &= 0xfffffffc;
  }

  if (event == 199)
  {
    max_cards = active_cards_count[active_player];
    if (max_cards <= active_cards_count[unk_008b35ec])
    {
      max_cards = active_cards_count[unk_008b35ec];
    }
    ai_delta = 0;
    for (current_card = 0; current_card < max_cards; ++current_card)
    {
      if (is_in_play(unk_008b35ec, current_card) && PLAYER_CARD_INSTANCE(unk_008b35ec, current_card).power > 2)
      {
        if ((PLAYER_CARD_INSTANCE(unk_008b35ec, current_card).state & STATE_TAPPED) == 0)
        {
          if (has_vigilance(unk_008b35ec, current_card) == 0)
          {
            ai_delta += PLAYER_CARD_INSTANCE(unk_008b35ec, current_card).power;
          }
        }
        else if (PLAYER_CARD_INSTANCE(unk_008b35ec, current_card).untap_status == 0)
        {
          ai_delta += PLAYER_CARD_INSTANCE(unk_008b35ec, current_card).power * 2;
        }
      }

      if (is_in_play(active_player, current_card) && PLAYER_CARD_INSTANCE(active_player, current_card).power > 2)
      {
        if ((PLAYER_CARD_INSTANCE(active_player, current_card).state & STATE_TAPPED) == 0)
        {
          if (has_vigilance(active_player, current_card) == 0)
          {
            ai_delta -= PLAYER_CARD_INSTANCE(active_player, current_card).power;
          }
        }
        else if (PLAYER_CARD_INSTANCE(active_player, current_card).untap_status == 0)
        {
          ai_delta += PLAYER_CARD_INSTANCE(active_player, current_card).power * -2;
        }
      }
    }
    ai_modifier += ai_delta * 0xc;
  }

  return 0;
}

// FUNCTION: MAGIC 0x00418d7c
// FUNCTION: SHANDALAR 0x0051b58d
int card_jandor_s_saddlebags(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x00419295
// FUNCTION: SHANDALAR 0x0051baa5
int card_jade_monolith(int player, int card, event_t event)
{
  target_t selected_target;

  if (event == EVENT_SHOULD_AI_PLAY && affected_card == card && affected_card_controller == player)
  {
    ai_modifier += (life[active_player] - life[unk_008b35ec]) * 0xc;
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    if ((unk_008b4278 & 4) != 0 && has_mana_w_global_cost_mod(player, card, COLOR_COLORLESS, 1) && (PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0 && real_target_available((int *)0, TARGET_SCAN_DAMAGE_TARGET, player, 2, 2, TARGET_ZONE_IN_PLAY, TYPE_CREATURE, TYPE_NONE, 0, 0, COLOR_TEST_0, COLOR_TEST_0, -1, ~SUB_WALL, -1, -1, 0, TARGET_STATE_DAMAGED, 0))
    {
      return 99;
    }
  }
  else if (event == EVENT_GET_SELECTED_CARD)
  {
    FUN_004e4ff3(0);
  }
  else
  {
    if (event == EVENT_ACTIVATE && has_mana_w_global_cost_mod(player, card, COLOR_COLORLESS, 1))
    {
      charge_mana_w_global_cost_mod(player, card, COLOR_COLORLESS, 1);
      if (spell_fizzled != 1)
      {
        if (g_duel_ai_mode_state != 1)
        {
          load_text("prompts.txt", "JADE_MONOLITH");
        }
        if (!C_real_select_target(player,
                                  2,
                                  player,
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
                                  TARGET_STATE_DAMAGED,
                                  0,
                                  text_lines[0],
                                  1,
                                  &selected_target))
        {
          spell_fizzled = 1;
        }
        else
        {
          PLAYER_CARD_INSTANCE(player, card).targets[0] = selected_target;
          PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
        }
      }
    }
    if (event == EVENT_RESOLVE_ACTIVATION)
    {
      selected_target = PLAYER_CARD_INSTANCE(player, card).targets[0];
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                           PLAYER_CARD_INSTANCE(player, card).parent_card)
          .number_of_targets = 0;
      if (!C_real_validate_target(selected_target.player,
                                  selected_target.card,
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
                                  TARGET_STATE_DAMAGED,
                                  0))
      {
        spell_fizzled = 1;
      }
      else
      {
        FUN_00419667(selected_target.player, selected_target.card, player);
      }
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x00419667
void FUN_00419667(int target_player, int target_card, int damage_target_player)
{
  int current_player;
  int current_card;

  for (current_player = 0; current_player < 2; ++current_player)
  {
    for (current_card = 0; current_card < active_cards_count[current_player]; ++current_card)
    {
      if (PLAYER_CARD_INSTANCE(current_player, current_card).internal_card_id == unk_009266a4 && (PLAYER_CARD_INSTANCE(current_player, current_card).state & 0x800002) == 2 && PLAYER_CARD_INSTANCE(current_player, current_card).damage_target_player == target_player && PLAYER_CARD_INSTANCE(current_player, current_card).damage_target_card == target_card)
      {
        PLAYER_CARD_INSTANCE(current_player, current_card).internal_card_id = -1;
        damage_player(damage_target_player,
                      PLAYER_CARD_INSTANCE(current_player, current_card).info_slot,
                      *(char *)((char *)&PLAYER_CARD_INSTANCE(current_player, current_card) + 0xc),
                      *(int *)((char *)&PLAYER_CARD_INSTANCE(current_player, current_card) + 0x48));
        PLAYER_CARD_INSTANCE(current_player, current_card).info_slot = 0;
      }
    }
  }
}

// FUNCTION: MAGIC 0x0041989d
// FUNCTION: SHANDALAR 0x0051c0ad
int card_onulet(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x00419ae1
// FUNCTION: SHANDALAR 0x0051c2f1
int card_amulet_of_kroog(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x00419fc3
// FUNCTION: SHANDALAR 0x0051c7d3
int card_grapeshot_catapult(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x0041a389
// FUNCTION: SHANDALAR 0x0051cb99
int card_bronze_tablet(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x0041ae2a
// FUNCTION: SHANDALAR 0x0051d63a
int card_nevinyrral_s_disk(int player, int card, event_t event)
{
  int result;
  int current_card;
  int guardian_beast_survives[2];
  int max_active_cards;

  if (event == 0x6c && affected_card == card && affected_card_controller == player)
  {
    result = FUN_00404cff(player, PLAYER_CARD_INSTANCE(player, card).internal_card_id, -1);
    if (result == 0)
    {
      ai_modifier += (landsofcolor_controlled[1 - player][2] - landsofcolor_controlled[player][2]) *
                     0xc;
    }
    PLAYER_CARD_INSTANCE(player, card).state |= 0x10;
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    result = has_mana(player, COLOR_ANY, 1);
    if (result == 0 ||
        (((PLAYER_CARD_INSTANCE(player, card).state &
           (STATE_SUMMONSICK_NOATTACK | STATE_SUMMONSICK_NOTAP)) != 0 &&
          (global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].type &
           TYPE_CREATURE) != 0) ||
         (PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) != 0))
    {
      result = 0;
    }
    else
    {
      result = 1;
    }
  }
  else
  {
    if (event == EVENT_ACTIVATE &&
        (result = has_mana(player, COLOR_ANY, 1), result != 0))
    {
      charge_mana(player, COLOR_COLORLESS, 1);
      if (spell_fizzled != 1)
      {
        PLAYER_CARD_INSTANCE(player, card).state |= 0x10;
      }
    }
    if (event == EVENT_RESOLVE_ACTIVATION)
    {
      guardian_beast_survives[active_player] = 1;
      guardian_beast_survives[unk_008b35ec] = guardian_beast_survives[active_player];

      current_card = 0;
      while (1)
      {
        max_active_cards = active_cards_count[active_player];
        if (max_active_cards <= active_cards_count[unk_008b35ec])
        {
          max_active_cards = active_cards_count[unk_008b35ec];
        }
        if (max_active_cards <= current_card)
        {
          break;
        }

        result = is_in_play(unk_008b35ec, current_card);
        if (result != 0 &&
            (global_cards_data[PLAYER_CARD_INSTANCE(unk_008b35ec, current_card).internal_card_id].type &
             TYPE_CREATURE) != 0)
        {
          if (global_cards_data[PLAYER_CARD_INSTANCE(unk_008b35ec, current_card).internal_card_id]
                      .code_pointer ==
                  card_guardian_beast &&
              (PLAYER_CARD_INSTANCE(unk_008b35ec, current_card).state & 0x10) == 0)
          {
            guardian_beast_survives[unk_008b35ec] = 0;
          }
          kill_card(unk_008b35ec, current_card, KILL_DESTROY);
        }

        result = is_in_play(active_player, current_card);
        if (result != 0 &&
            (global_cards_data[PLAYER_CARD_INSTANCE(active_player, current_card).internal_card_id].type &
             TYPE_CREATURE) != 0)
        {
          if (global_cards_data[PLAYER_CARD_INSTANCE(active_player, current_card).internal_card_id]
                      .code_pointer ==
                  card_guardian_beast &&
              (PLAYER_CARD_INSTANCE(active_player, current_card).state & 0x10) == 0)
          {
            guardian_beast_survives[active_player] = 0;
          }
          kill_card(active_player, current_card, KILL_DESTROY);
        }

        ++current_card;
      }

      regenerate_or_graveyard_triggers();

      current_card = 0;
      while (1)
      {
        max_active_cards = active_cards_count[active_player];
        if (max_active_cards <= active_cards_count[unk_008b35ec])
        {
          max_active_cards = active_cards_count[unk_008b35ec];
        }
        if (max_active_cards <= current_card)
        {
          break;
        }

        result = is_in_play(unk_008b35ec, current_card);
        if (result != 0 && guardian_beast_survives[unk_008b35ec] != 0 &&
            (global_cards_data[PLAYER_CARD_INSTANCE(unk_008b35ec, current_card).internal_card_id].type &
             TYPE_ARTIFACT) != 0)
        {
          kill_card(unk_008b35ec, current_card, KILL_DESTROY);
        }

        result = is_in_play(active_player, current_card);
        if (result != 0 && guardian_beast_survives[active_player] != 0 &&
            (global_cards_data[PLAYER_CARD_INSTANCE(active_player, current_card).internal_card_id].type &
             TYPE_ARTIFACT) != 0)
        {
          kill_card(active_player, current_card, KILL_DESTROY);
        }

        ++current_card;
      }

      regenerate_or_graveyard_triggers();

      current_card = 0;
      while (1)
      {
        max_active_cards = active_cards_count[active_player];
        if (max_active_cards <= active_cards_count[unk_008b35ec])
        {
          max_active_cards = active_cards_count[unk_008b35ec];
        }
        if (max_active_cards <= current_card)
        {
          break;
        }

        result = is_in_play(unk_008b35ec, current_card);
        if (result != 0 &&
            (global_cards_data[PLAYER_CARD_INSTANCE(unk_008b35ec, current_card).internal_card_id].type &
             TYPE_ENCHANTMENT) != 0)
        {
          kill_card(unk_008b35ec, current_card, KILL_DESTROY);
        }

        result = is_in_play(active_player, current_card);
        if (result != 0 &&
            (global_cards_data[PLAYER_CARD_INSTANCE(active_player, current_card).internal_card_id].type &
             TYPE_ENCHANTMENT) != 0)
        {
          kill_card(active_player, current_card, KILL_DESTROY);
        }

        ++current_card;
      }
    }

    result = 0;
  }

  return result;
}

// FUNCTION: MAGIC 0x0041b573
// FUNCTION: SHANDALAR 0x0051dd8d
int card_aladdin_s_ring(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x0041b801
// FUNCTION: SHANDALAR 0x0051e01b
int card_rod_of_ruin(int player, int card, event_t event)
{
  if (event == EVENT_SHOULD_AI_PLAY && affected_card == card && affected_card_controller == player)
  {
    ai_modifier += (unk_00742f80[active_player][3] * 3 - 6) * 4;
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    if (has_mana_w_global_cost_mod(player, card, COLOR_COLORLESS, 3) && !is_animated_and_sick(player, card) && (PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0)
    {
      return 1;
    }
  }
  else if (event == EVENT_GET_SELECTED_CARD)
  {
    FUN_004e4ff3(1);
  }
  else
  {
    if (event == EVENT_ACTIVATE)
    {
      charge_mana_w_global_cost_mod(player, card, COLOR_COLORLESS, 3);
      if (spell_fizzled != 1)
      {
        if (g_duel_ai_mode_state != 1)
        {
          load_text("prompts.txt", "ROD_OF_RUIN");
        }
        FUN_0054ac4d(player, card, 1);
        if (spell_fizzled != 1)
        {
          PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
        }
      }
    }
    if (event == EVENT_RESOLVE_ACTIVATION)
    {
      FUN_0054af10(player, card, EVENT_RESOLVE_ACTIVATION, 1);
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                           PLAYER_CARD_INSTANCE(player, card).parent_card)
          .number_of_targets = 0;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0041ba8b
// FUNCTION: SHANDALAR 0x0051e2a5
int card_winter_orb(int player, int card, event_t event)
{
  target_t selected_target;
  int current_card;

  if (event == EVENT_SHOULD_AI_PLAY && affected_card == card && affected_card_controller == player && FUN_00404cff(player, PLAYER_CARD_INSTANCE(player, card).internal_card_id, -1) == 0)
  {
    ai_modifier += (landsofcolor_controlled[active_player][7] - landsofcolor_controlled[unk_008b35ec][7]) * 0xc;
  }

  if (event == 0x82 && (global_cards_data[PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).internal_card_id].type & TYPE_LAND) && (PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0 && (global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].type & TYPE_CREATURE) == 0)
  {
    PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).dummy3 &= ~2;
    unk_00925d3c |= 1;
  }

  if (current_phase == PHASE_UPKEEP && affected_card == card && affected_card_controller == player && (((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0) || (global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].type & TYPE_CREATURE)))
  {
    if (event == EVENT_TRIGGER && !real_target_available((int *)0, TARGET_SCAN_DIRECT, human_player, human_player, human_player, TARGET_ZONE_IN_PLAY, TYPE_LAND, TYPE_NONE, 0, 0, COLOR_TEST_0, COLOR_TEST_0, -1, ~SUB_WALL, -1, -1, 0, TARGET_STATE_COULD_UNTAP, 0) && real_target_available((int *)0, TARGET_SCAN_DIRECT, human_player, human_player, human_player, TARGET_ZONE_IN_PLAY, TYPE_LAND, TYPE_NONE, 0, 0, COLOR_TEST_0, COLOR_TEST_0, -1, ~SUB_WALL, -1, -1, 0, TARGET_STATE_TAPPED, 0))
    {
      event_result |= 2;
    }

    if (event == EVENT_RESOLVE_TRIGGER)
    {
      if (active_player == human_player && (g_duel_network_flags & 2) == 0)
      {
        selected_target.player = human_player;
        selected_target.card = FUN_00534ddb(human_player, 1);
        load_text("prompts.txt", "WINTERORB");
        do_dialog(player, player, card, selected_target.player, selected_target.card, text_lines[1], 0);
      }
      else
      {
        load_text("prompts.txt", "WINTERORB");
        C_real_select_target(human_player,
                             human_player,
                             human_player,
                             TARGET_ZONE_IN_PLAY,
                             TYPE_LAND,
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
                             text_lines[0],
                             0,
                             &selected_target);
      }

      PLAYER_CARD_INSTANCE(selected_target.player, selected_target.card).dummy3 |= 2;
      for (current_card = 0; current_card < active_cards_count[human_player]; ++current_card)
      {
        if (is_in_play(human_player, current_card) && (PLAYER_CARD_INSTANCE(human_player, current_card).state & STATE_TAPPED) && (global_cards_data[PLAYER_CARD_INSTANCE(human_player, current_card).internal_card_id].type & TYPE_LAND) && (PLAYER_CARD_INSTANCE(human_player, current_card).dummy3 & 2) == 0)
        {
          PLAYER_CARD_INSTANCE(human_player, current_card).dummy3 &= ~1;
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

// FUNCTION: MAGIC 0x0041bffa
// FUNCTION: SHANDALAR 0x0051e814
int card_brass_man(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x0041c1ef
// FUNCTION: SHANDALAR 0x0051ea11
int card_dragon_engine(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x0041c706
// FUNCTION: SHANDALAR 0x0051ef24
int card_clockwork_beast(int player, int card, event_t event)
{
  return FUN_0041c752(player, card, event, 7);
}

// FUNCTION: MAGIC 0x0041c72c
// FUNCTION: SHANDALAR 0x0051ef4a
int card_clockwork_avian(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x0041c752
int FUN_0041c752(int player, int card, int event, int amount)
{
  unsigned int special_counters;
  int old_max_x_value;
  int mana_result;
  int local_zero;

  if (event == EVENT_CAST_SPELL && affected_card == card && affected_card_controller == player)
  {
    FUN_00551572(player, card, amount);
  }

  if (trigger_condition == 0xcc && (special_counters = C_get_special_counters(player, card)) != 0 && affected_card == card && affected_card_controller == player && player == current_turn && ((PLAYER_CARD_INSTANCE(player, card).state & 4) != 0 || (PLAYER_CARD_INSTANCE(player, card).untap_status != -1 && human_player != player)))
  {
    if (event == EVENT_TRIGGER)
    {
      event_result |= 2;
    }
    if (event == EVENT_RESOLVE_TRIGGER)
    {
      FUN_00551334(player, card);
    }
  }

  if (event == EVENT_POWER && affected_card == card && affected_card_controller == player)
  {
    special_counters = C_get_special_counters(player, card);
    event_result += special_counters;
  }

  old_max_x_value = max_x_value;
  if (event == EVENT_CAN_ACTIVATE && current_phase == PHASE_MAIN1 && human_player == player && (PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0 && unk_00742f60 == player)
  {
    special_counters = C_get_special_counters(player, card);
    if ((int)special_counters < amount && (mana_result = has_mana(player, COLOR_ANY, 1)) != 0)
    {
      if (active_player == player && (g_duel_network_flags & 2) == 0 && PLAYER_CARD_INSTANCE(player, card).toughness < life[player])
      {
        unk_008b3270 |= 3;
      }
      return 1;
    }
  }
  else if (event == EVENT_GET_SELECTED_CARD)
  {
    mana_result = has_mana(player, COLOR_ANY, 1);
    local_zero = 0;
    special_counters = C_get_special_counters(player, card);
    unk_00715fa8 = ClampIntToRange(amount - special_counters, local_zero, mana_result);
  }
  else
  {
    if (event == EVENT_ACTIVATE && affected_card == card && affected_card_controller == player)
    {
      special_counters = C_get_special_counters(player, card);
      max_x_value = amount - special_counters;
      if (unk_008b35ec == player || (g_duel_network_flags & 2) != 0)
      {
        mana_result = charge_mana(player, 0, -1);
      }
      else
      {
        mana_result = charge_mana(player, 0, unk_00715fa8);
      }
      PLAYER_CARD_INSTANCE(player, card).info_slot = mana_result;
      max_x_value = old_max_x_value;
      if (spell_fizzled == 1)
      {
        PLAYER_CARD_INSTANCE(player, card).info_slot = 0;
      }
      else
      {
        PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
      }
    }
    if (event == EVENT_RESOLVE_ACTIVATION && PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                                                                  PLAYER_CARD_INSTANCE(player, card).parent_card)
                                                     .internal_card_id != -1)
    {
      local_zero = 0;
      mana_result = PLAYER_CARD_INSTANCE(player, card).info_slot;
      special_counters = C_get_special_counters(card_on_stack_controller, card_on_stack);
      mana_result = ClampIntToRange(mana_result + special_counters, local_zero, amount);
      FUN_00551572(card_on_stack_controller, card_on_stack, mana_result);
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0041cc95
// FUNCTION: SHANDALAR 0x0051f4af
int card_colossus_of_sardia(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x0041ce96
// FUNCTION: SHANDALAR 0x0051f6af
int card_flying_carpet(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x0041d4f9
// FUNCTION: SHANDALAR 0x0051fd11
int card_helm_of_chatzuk(int player, int card, event_t event)
{
  target_t selected_target;

  if (event == EVENT_SHOULD_AI_PLAY && affected_card == card && affected_card_controller == player)
  {
    ai_modifier += (artifact_cards_in_play[player] * 0xc) / 2;
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    if (!is_animated_and_sick(player, card) && (PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0 && has_mana_w_global_cost_mod(player, card, COLOR_COLORLESS, 1) && real_target_available((int *)0, TARGET_SCAN_DIRECT, player, 2, 2, TARGET_ZONE_IN_PLAY, TYPE_CREATURE, TYPE_NONE, 0, get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0, -1, ~SUB_WALL, -1, -1, 0, 0, 0))
    {
      return 1;
    }
  }
  else if (event == EVENT_GET_SELECTED_CARD)
  {
    FUN_004e4ff3(0);
  }
  else
  {
    if (event == EVENT_ACTIVATE && has_mana_w_global_cost_mod(player, card, COLOR_COLORLESS, 1))
    {
      charge_mana_w_global_cost_mod(player, card, COLOR_COLORLESS, 1);
      if (spell_fizzled != 1)
      {
        if (g_duel_ai_mode_state != 1)
        {
          load_text("prompts.txt", "HELM_OF_CHATZUK");
        }
        if (!C_real_select_target(player,
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
                                  0,
                                  0,
                                  text_lines[0],
                                  1,
                                  &selected_target))
        {
          spell_fizzled = 1;
        }
        else
        {
          PLAYER_CARD_INSTANCE(player, card).targets[0] = selected_target;
          PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
          if (active_player == player && (((PLAYER_CARD_INSTANCE(selected_target.player, selected_target.card).token_status & 0x40) != 0) || selected_target.player == unk_008b35ec))
          {
            ai_modifier -= 0x30;
          }
          PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
        }
      }
    }

    if (event == EVENT_RESOLVE_ACTIVATION)
    {
      selected_target = PLAYER_CARD_INSTANCE(player, card).targets[0];
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                           PLAYER_CARD_INSTANCE(player, card).parent_card)
          .number_of_targets = 0;
      if (!C_real_validate_target(selected_target.player,
                                  selected_target.card,
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
        spell_fizzled = 1;
      }
      else
      {
        int legacy_card;

        legacy_card =
            create_legacy_effect(card_on_stack_controller, card_on_stack, unk_00896534, selected_target.player, selected_target.card);
        if (legacy_card != -1)
        {
          PLAYER_CARD_INSTANCE(player, legacy_card).info_slot = 0x40;
        }
        PLAYER_CARD_INSTANCE(selected_target.player, selected_target.card).token_status = 0x8000000;
      }
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0041dad6
// FUNCTION: SHANDALAR 0x005202ec
int card_coral_helm(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x0041df71
// FUNCTION: SHANDALAR 0x00520787
int card_tawnos_s_wand(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x0041e45a
// FUNCTION: SHANDALAR 0x00520c71
int card_the_hive(int player, int card, event_t event)
{
  int bee_card;

  if (event == EVENT_CAN_ACTIVATE)
  {
    if (PLAYER_CARD_INSTANCE(player, card).info_slot == 0 && has_mana(player, 7, 5) != 0 && (((PLAYER_CARD_INSTANCE(player, card).state & STATE_SUMMONSICK_BOTH) == 0) || ((global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].type & TYPE_CREATURE) == 0)) && (PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0)
    {
      if (active_player == player && (g_duel_network_flags & 2) == 0 && unk_00939330 > 0)
      {
        unk_008b3270 |= 3;
      }
      return 1;
    }
    else
    {
      return 0;
    }
  }

  if (event == EVENT_ACTIVATE)
  {
    if (has_mana(player, 7, 5) != 0)
    {
      charge_mana(player, COLOR_COLORLESS, 5);
      if (spell_fizzled != 1)
      {
        PLAYER_CARD_INSTANCE(player, card).state |= 0x10;
        PLAYER_CARD_INSTANCE(player, card).info_slot |= 1;
        if (unk_00939330 > 0)
        {
          --unk_00939330;
        }
      }
    }
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    int tmp = FUN_004b5f03(0x375);
    bee_card = add_card_to_hand(player, tmp);
    if (bee_card != -1)
    {
      FUN_0051a41c(player, bee_card);
      PLAYER_CARD_INSTANCE(player, bee_card).token_status |= STATUS_TOKEN;
    }
    PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                         PLAYER_CARD_INSTANCE(player, card).parent_card)
        .info_slot = 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0041e767
// FUNCTION: SHANDALAR 0x00520f7d
int card_bottle_of_suleiman(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x0041ea45
// FUNCTION: SHANDALAR 0x0052125a
int card_glasses_of_urza(int player, int card, event_t event)
{
  card_instance_t *instance;
  target_t target;
  int target_player;
  int count;
  int current_card;
  int internal_card_ids[30];

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_CAN_ACTIVATE)
  {
    if (((instance->state & 3) == 0 || (global_cards_data[instance->internal_card_id].type & TYPE_CREATURE) == 0) && (instance->state & STATE_TAPPED) == 0)
    {
      return 1;
    }
    return 0;
  }

  if (event == EVENT_ACTIVATE)
  {
    if (g_duel_ai_mode_state != 1)
    {
      load_text("prompts.txt", "GLASSES_OF_URZA");
    }
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
                              ~SUB_WALL,
                              -1,
                              -1,
                              0,
                              0,
                              0,
                              text_lines[0],
                              1,
                              &target))
    {
      spell_fizzled = 1;
    }
    else
    {
      instance->targets[0].player = target.player;
      instance->targets[0].card = target.card;
      instance->number_of_targets = 1;
      instance->state |= STATE_TAPPED;
    }
    return 0;
  }

  if (event == EVENT_RESOLVE_ACTIVATION && (player == human_player || (g_duel_network_flags & 2) != 0) && g_duel_ai_mode_state != 1 && g_duel_network_state == 0)
  {
    target_player = instance->targets[0].player;
    count = 0;

    if (player == human_player)
    {
      for (current_card = 0; current_card < active_cards_count[target_player]; ++current_card)
      {
        if (PLAYER_CARD_INSTANCE(target_player, current_card).internal_card_id != -1 && (PLAYER_CARD_INSTANCE(target_player, current_card).state & 2) == 0)
        {
          internal_card_ids[count] =
              PLAYER_CARD_INSTANCE(target_player, current_card).internal_card_id;
          ++count;
        }
      }
      if (g_duel_ai_mode_state != 1)
      {
        load_text("prompts.txt", "GLASSES_OF_URZA_2");
      }
    }

    show_deck(player, internal_card_ids, count, (char (*)[300])0x89684c, 0, &gs_cancel_008a8c20);
  }

  return 0;
}

// FUNCTION: MAGIC 0x0041edf5
// FUNCTION: SHANDALAR 0x0052160a
int card_library_of_leng(int player, int card, event_t event)
{
  if (event == EVENT_CAST_SPELL && card == affected_card && player == affected_card_controller)
  {
    unk_007a79b0[player] |= 2;
  }

  if (event == 0x1f && human_player == player && ((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0 || (global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].type & TYPE_CREATURE) != 0))
  {
    ++event_result;
  }

  if (event == 0x77 && card == affected_card && player == affected_card_controller)
  {
    unk_007a79b0[player] &= ~2;
  }

  if (trigger_condition == 0xd4 && card == affected_card && player == affected_card_controller && player == trigger_cause_controller && card == trigger_cause && player == current_turn)
  {
    if (event == 0x7d)
    {
      event_result |= 2;
    }
    if (event == 0x7e)
    {
      unk_007a79b0[player] &= ~2;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0041ef7c
// FUNCTION: SHANDALAR 0x00521791
int card_pandora_s_box(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x0041f3d3
// FUNCTION: SHANDALAR 0x00521be8
int card_sunglasses_of_urza(int player, int card, event_t event)
{
  if (event == EVENT_COUNT_MANA && affected_card == card && affected_card_controller == player && (((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0) || (global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].type & TYPE_CREATURE)))
  {
    FUN_004eaceb(player, get_sleighted_color(player, card, 5), get_sleighted_color(player, card, 4));
  }

  return 0;
}

// FUNCTION: MOK 0x00427740
// FUNCTION: MAGIC 0x0041f4c0
int helper_mox(int player, int card, event_t event, int color)
{
  if (event == EVENT_CAN_ACTIVATE)
  {
    return ((((PLAYER_CARD_INSTANCE(player, card).state & STATE_SUMMONSICK_BOTH) == 0) || ((global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].type & TYPE_CREATURE) == 0)) && ((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0)) ? 1 : 0;
  }

  if (event == EVENT_ACTIVATE)
  {
    ai_modifier -= 0xc;
    undeclare_mana_available_and_produce_it(player, color, 1);
    PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
    produced_mana_color = color;
  }

  if ((event == EVENT_COUNT_MANA) && (card == card_on_stack) && (player == card_on_stack_controller) && ((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0))
  {
    declare_mana_available(player, color, 1);
  }

  return 0;
}
